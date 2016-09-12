/********************************************************************************/
/*										*/
/*			IWG EK Index Parsing Utilities				*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: ekutils.c 686 2016-07-20 16:30:54Z kgoldman $		*/
/*										*/
/* (c) Copyright IBM Corporation 2016.						*/
/*										*/
/* All rights reserved.								*/
/* 										*/
/* Redistribution and use in source and binary forms, with or without		*/
/* modification, are permitted provided that the following conditions are	*/
/* met:										*/
/* 										*/
/* Redistributions of source code must retain the above copyright notice,	*/
/* this list of conditions and the following disclaimer.			*/
/* 										*/
/* Redistributions in binary form must reproduce the above copyright		*/
/* notice, this list of conditions and the following disclaimer in the		*/
/* documentation and/or other materials provided with the distribution.		*/
/* 										*/
/* Neither the names of the IBM Corporation nor the names of its		*/
/* contributors may be used to endorse or promote products derived from		*/
/* this software without specific prior written permission.			*/
/* 										*/
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS		*/
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		*/
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR	*/
/* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT		*/
/* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	*/
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		*/
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,	*/
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY	*/
/* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		*/
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE	*/
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.		*/
/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include <tss2/tssresponsecode.h>
#include <tss2/tssutils.h>
#include <tss2/tssprint.h>
#include <tss2/Unmarshal_fp.h>

#include "ekutils.h"

/* windows apparently uses _MAX_PATH in stdlib.h */
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

/* The print flag is set by the caller, depending on whether it wants information displayed.

   verbose is a global, used for verbose debug print

   Errors are always printed.
*/

extern int verbose;

static TPM_RC readNvBufferMax(TSS_CONTEXT *tssContext,
			      uint32_t *nvBufferMax)
{
    TPM_RC			rc = 0;
    GetCapability_In 		in;
    GetCapability_Out		out;

    in.capability = TPM_CAP_TPM_PROPERTIES;
    in.property = TPM_PT_NV_BUFFER_MAX;
    in.propertyCount = 1;	/* ask for one property */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&out, 
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_GetCapability,
			 TPM_RH_NULL, NULL, 0);
    }
    /* sanity check that the property name is correct, demo of how to parse the structure */
    if (rc == 0) {
	if (out.capabilityData.data.tpmProperties.tpmProperty[0].property == TPM_PT_NV_BUFFER_MAX) {
	    *nvBufferMax = out.capabilityData.data.tpmProperties.tpmProperty[0].value;
	}
	else {
	    printf("readNvBufferMax: wrong property returned: %08x\n",
		   out.capabilityData.data.tpmProperties.tpmProperty[0].property);
	    /* hard code a value for a TPM that does not implement TPM_PT_NV_BUFFER_MAX */
	    *nvBufferMax = 512;
	}
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("getcapability: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    return rc;
}

/* getIndexSize() uses nvreadpublic to return the NV index size */

TPM_RC getIndexSize(TSS_CONTEXT *tssContext,
		    uint16_t *dataSize,
		    TPMI_RH_NV_INDEX nvIndex)
{
    TPM_RC			rc = 0;
    NV_ReadPublic_In 		in;
    NV_ReadPublic_Out		out;
    
    if (rc == 0) {
	/* if (verbose) printf("getIndexSize: index %08x\n", nvIndex); */
	in.nvIndex = nvIndex;
    }
    /* call TSS to execute the command */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&out,
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_NV_ReadPublic,
			 TPM_RH_NULL, NULL, 0);
	/* only print if verbose, since nonce and template index may not exist */
	if ((rc != 0) && verbose) {
	    const char *msg;
	    const char *submsg;
	    const char *num;
	    printf("nvreadpublic: failed, rc %08x\n", rc);
	    TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	    printf("%s%s%s\n", msg, submsg, num);
	}
    }
    if (rc == 0) {
	/* if (verbose) printf("getIndexSize: size %u\n", out.nvPublic.t.nvPublic.dataSize); */
	*dataSize = out.nvPublic.t.nvPublic.dataSize;
    }
    return rc;
}

/* getIndexData() uses nvread to return the NV index contents.

   It assumes index authorization with an empty password
*/

TPM_RC getIndexData(TSS_CONTEXT *tssContext,
		    unsigned char **readBuffer,		/* freed by caller */
		    TPMI_RH_NV_INDEX nvIndex,
		    uint16_t readDataSize)		/* total size to read */
{
    TPM_RC			rc = 0;
    int				done = FALSE;
    uint32_t 			nvBufferMax;
    uint16_t 			bytesRead;			/* bytes read so far */
    NV_Read_In 			in;
    NV_Read_Out			out;
    
    /* data may have to be read in chunks.  Read the TPM_PT_NV_BUFFER_MAX, the chunk size */
    if (rc == 0) {
	rc = readNvBufferMax(tssContext,
			     &nvBufferMax);
    }    
    if (rc == 0) {
	if (verbose) printf("getIndexData: index %08x\n", nvIndex);
	in.authHandle = nvIndex;	/* index authorization */
	in.nvIndex = nvIndex;
	in.offset = 0;			/* start at beginning */
	bytesRead = 0;			/* bytes read so far */
    }
    if (rc == 0) {
	rc = TSS_Malloc(readBuffer, readDataSize);
    }
    /* call TSS to execute the command */
    while ((rc == 0) && !done) {
	if (rc == 0) {
	    /* read a chunk */
	    in.offset += bytesRead;
	    if ((uint32_t)(readDataSize - bytesRead) < nvBufferMax) {
		in.size = readDataSize - bytesRead;	/* last chunk */
	    }
	    else {
		in.size = nvBufferMax;		/* next chunk */
	    }
	}
	if (rc == 0) {
	    rc = TSS_Execute(tssContext,
			     (RESPONSE_PARAMETERS *)&out,
			     (COMMAND_PARAMETERS *)&in,
			     NULL,
			     TPM_CC_NV_Read,
			     TPM_RS_PW, NULL, 0,
			     TPM_RH_NULL, NULL, 0);
	    if (rc != 0) {
		const char *msg;
		const char *submsg;
		const char *num;
		printf("nvread: failed, rc %08x\n", rc);
		TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
		printf("%s%s%s\n", msg, submsg, num);
	    }
	}
 	/* copy the results to the read buffer */
	if (rc == 0) {
	    memcpy(*readBuffer + bytesRead, out.data.b.buffer, out.data.b.size);
	    bytesRead += out.data.b.size;
	    if (bytesRead == readDataSize) {
		done = TRUE;
	    }
	}
    }
    return rc;
}

/* getIndexContents() uses nvreadpublic to get the NV index size, then uses nvread to read the
   entire contents

 */

TPM_RC getIndexContents(TSS_CONTEXT *tssContext,
			unsigned char **readBuffer,		/* freed by caller */
			uint16_t *readBufferSize,		/* total size read */
			TPMI_RH_NV_INDEX nvIndex)
{
    TPM_RC			rc = 0;

    /* first read the public index size */
    if (rc == 0) {
	rc = getIndexSize(tssContext, readBufferSize, nvIndex);
    }
    /* read the entire index */
    if (rc == 0) {
	rc = getIndexData(tssContext,
			  readBuffer,			/* freed by caller */
			  nvIndex,
			  *readBufferSize);		/* total size to read */
    }
    return rc;
}

/* IWG default policy */

static const unsigned char iwgPolicy[] = {
    0x83, 0x71, 0x97, 0x67, 0x44, 0x84, 0xB3, 0xF8, 0x1A, 0x90, 0xCC, 0x8D, 0x46, 0xA5, 0xD7, 0x24,
    0xFD, 0x52, 0xD7, 0x6E, 0x06, 0x52, 0x0B, 0x64, 0xF2, 0xA1, 0xDA, 0x1B, 0x33, 0x14, 0x69, 0xAA
};

/* RSA EK primary key IWG default template */

void getRsaTemplate(TPMT_PUBLIC *tpmtPublic)
{
    tpmtPublic->type = TPM_ALG_RSA;
    tpmtPublic->nameAlg = TPM_ALG_SHA256;
    tpmtPublic->objectAttributes.val = TPMA_OBJECT_FIXEDTPM |
				       TPMA_OBJECT_FIXEDPARENT |
				       TPMA_OBJECT_SENSITIVEDATAORIGIN |
				       TPMA_OBJECT_ADMINWITHPOLICY |
				       TPMA_OBJECT_RESTRICTED |
				       TPMA_OBJECT_DECRYPT;
    tpmtPublic->authPolicy.t.size = 32;
    memcpy(&tpmtPublic->authPolicy.t.buffer, iwgPolicy, 32);
    tpmtPublic->parameters.rsaDetail.symmetric.algorithm = TPM_ALG_AES;
    tpmtPublic->parameters.rsaDetail.symmetric.keyBits.aes = 128;
    tpmtPublic->parameters.rsaDetail.symmetric.mode.aes = TPM_ALG_CFB;
    tpmtPublic->parameters.rsaDetail.scheme.scheme = TPM_ALG_NULL;
    tpmtPublic->parameters.rsaDetail.scheme.details.anySig.hashAlg = 0;
    tpmtPublic->parameters.rsaDetail.keyBits = 2048;
    tpmtPublic->parameters.rsaDetail.exponent = 0;
    tpmtPublic->unique.rsa.t.size = 256;
    memset(&tpmtPublic->unique.rsa.t.buffer, 0, 256);
    return;
}

/* ECC EK primary key IWG default template */

void getEccTemplate(TPMT_PUBLIC *tpmtPublic)
{
    tpmtPublic->type = TPM_ALG_ECC;
    tpmtPublic->nameAlg = TPM_ALG_SHA256;
    tpmtPublic->objectAttributes.val = TPMA_OBJECT_FIXEDTPM |
				       TPMA_OBJECT_FIXEDPARENT |
				       TPMA_OBJECT_SENSITIVEDATAORIGIN |
				       TPMA_OBJECT_ADMINWITHPOLICY |
				       TPMA_OBJECT_RESTRICTED |
				       TPMA_OBJECT_DECRYPT;
    tpmtPublic->authPolicy.t.size = sizeof(iwgPolicy);
    memcpy(tpmtPublic->authPolicy.t.buffer, iwgPolicy, sizeof(iwgPolicy));
    tpmtPublic->parameters.eccDetail.symmetric.algorithm = TPM_ALG_AES;
    tpmtPublic->parameters.eccDetail.symmetric.keyBits.aes = 128;
    tpmtPublic->parameters.rsaDetail.symmetric.mode.aes = TPM_ALG_CFB;
    tpmtPublic->parameters.eccDetail.scheme.scheme = TPM_ALG_NULL;
    tpmtPublic->parameters.eccDetail.scheme.details.anySig.hashAlg = 0;
    tpmtPublic->parameters.eccDetail.curveID = TPM_ECC_NIST_P256;
    tpmtPublic->parameters.eccDetail.kdf.scheme = TPM_ALG_NULL;
    tpmtPublic->parameters.eccDetail.kdf.details.mgf1.hashAlg = 0;
    tpmtPublic->unique.ecc.x.t.size = 32;	
    memset(&tpmtPublic->unique.ecc.x.t.buffer, 0, 32);	
    tpmtPublic->unique.ecc.y.t.size = 32;	
    memset(&tpmtPublic->unique.ecc.y.t.buffer, 0, 32);	
    return;
}

/* getIndexX509Certificate() reads the certificate from nvIndex and converts to openssl X509
   format

*/

TPM_RC getIndexX509Certificate(TSS_CONTEXT *tssContext,
			       X509 **certificate,		/* freed by caller */
			       TPMI_RH_NV_INDEX nvIndex)
{
    TPM_RC			rc = 0;
    unsigned char 		*certData = NULL; 		/* freed @1 */
    uint16_t 			certSize;
    unsigned char 		*tmpData = NULL; 

    /* read the certificate from NV to a DER stream */
    if (rc == 0) {
	rc = getIndexContents(tssContext,
			      &certData,
			      &certSize,
			      nvIndex);
    }
    /* unmarshal the DER stream to X509 structure */
    if (rc == 0) {
	tmpData = certData;		/* temp because d2i moves the pointer */
	*certificate = d2i_X509(NULL,			/* freed by caller */
				 (const unsigned char **)&tmpData, certSize);
	if (*certificate == NULL) {
	    printf("getIndexX509Certificate: Could not parse X509 certificate\n");
	    rc = TPM_RC_INTEGRITY;
	}
    }
    free(certData);			/* @1 */
    return rc;
}


/* getRootCertificateFilenames() reads filename, which is a list of filenames.  The intent is that
   the filenames are a list of EK TPM vendor root certificates. in PEM format.

   It accepts up to rootFilesMax filenames, which s

*/

TPM_RC getRootCertificateFilenames(char *rootFilename[],
				   unsigned int *rootFileCount,
				   const char *listFilename)
{
    TPM_RC		rc = 0;
    int			done = 0;
    FILE		*listFile = NULL;		/* closed @1 */

    if (rc == 0) {
	listFile = fopen(listFilename, "rb");		/* closed @1 */
	if (listFile == NULL) {
	    printf("getRootCertificateFilenames: Error opening list file %s\n",
		   listFilename);  
	    rc = TSS_RC_FILE_OPEN;
	}
    }
    while ((rc == 0) && !done && (*rootFileCount < MAX_ROOTS)) {
	if (rc == 0) {
	    rootFilename[*rootFileCount] = malloc(PATH_MAX);
	    if (rootFilename[*rootFileCount] == NULL) {
		printf("getRootCertificateFilenames: Error allocating memory\n");
		rc = TSS_RC_OUT_OF_MEMORY;
	    }
	}
	if (rc == 0) {
	    char *tmpptr = fgets(rootFilename[*rootFileCount], PATH_MAX-1, listFile);
	    if (tmpptr == NULL) {	/* end of file */
		free(rootFilename[*rootFileCount]);	/* free malloced but unused entry */
		done = 1;
	    }
	}
	size_t rootFilenameLength;
	if ((rc == 0) && !done) {
	    rootFilenameLength = strlen(rootFilename[*rootFileCount]);
	    if (rootFilename[*rootFileCount][rootFilenameLength-1] != '\n') {
		printf("getRootCertificateFilenames: filename %s too long\n",
		       rootFilename[*rootFileCount]);
		rc = TSS_RC_OUT_OF_MEMORY;
		free(rootFilename[*rootFileCount]);	/* free malloced but bad entry */
		done = 1;
	    }
	}
	if ((rc == 0) && !done) {
	    rootFilename[*rootFileCount][rootFilenameLength-1] = '\0';	/* remove newline */
	    (*rootFileCount)++;
	}
    }
    if (listFile != NULL) {
	fclose(listFile);		/* @1 */
    }
    return rc;
}


/* getCaStore() creates an openssl X509_STORE, populated by the root certificates in the
   rootFilename array.

   The caCert array is returned because it must be freed after the caStore is freed

   NOTE:  There is no TPM interaction.
*/ 

TPM_RC getCaStore(X509_STORE **caStore,		/* freed by caller */
		  X509 	*caCert[],		/* freed by caller */
		  const char *rootFilename[],
		  unsigned int rootFileCount)
{
    TPM_RC			rc = 0;
    FILE 			*caCertFile = NULL;		/* closed @1 */
    unsigned int 		i;

    if (rc == 0) {
	*caStore  = X509_STORE_new();
	if (*caStore == NULL) {
	    printf("getCaStore: X509_store_new failed\n");  
	    rc = TSS_RC_OUT_OF_MEMORY;
	}
    }
    for (i = 0 ; (i < rootFileCount) && (rc == 0) ; i++) {
	/* read a root certificate from the file */
	caCertFile = fopen(rootFilename[i], "rb");	/* closed @1 */
	if (caCertFile == NULL) {
	    printf("getCaStore: Error opening CA root certificate file %s\n",
		   rootFilename[i]);  
	    rc = TSS_RC_FILE_OPEN;
	}
	/* convert the root certificate from PEM to X509 */
	if (rc == 0) {
	    caCert[i] = PEM_read_X509(caCertFile , NULL, 0, NULL);	/* freed by caller */
	    if (caCert[i] == NULL) {
		printf("getCaStore: Error reading CA root certificate file %s\n",
		       rootFilename[i]);  
		rc = TSS_RC_FILE_READ;
	    } 
	}
	/* add the CA X509 certificate to the certificate store */
	if (rc == 0) {
	    X509_STORE_add_cert(*caStore, caCert[i]);    
	}
	if (caCertFile != NULL) {
	    fclose(caCertFile);		/* @1 */
	    caCertFile = NULL;
	}
    }
    return rc;
}

/* processEKNonce()reads the EK nonce from NV and returns the contents and size */
   
TPM_RC processEKNonce(TSS_CONTEXT *tssContext,
		      unsigned char **nonce, 	/* freed by caller */
		      uint16_t *nonceSize,
		      TPMI_RH_NV_INDEX ekNonceIndex,
		      int print)
{
    TPM_RC			rc = 0;

    if (rc == 0) { 
	rc = getIndexContents(tssContext,
			      nonce,
			      nonceSize,
			      ekNonceIndex);
    }
    /* optional tracing */
    if (rc == 0) {
	if (print) TSS_PrintAll("EK Nonce: ", *nonce, *nonceSize);
    }
    return rc;
}

/* processEKTemplate() reads the EK template from NV and returns the unmarshaled TPMT_PUBLIC */

TPM_RC processEKTemplate(TSS_CONTEXT *tssContext,
			 TPMT_PUBLIC *tpmtPublic,
			 TPMI_RH_NV_INDEX ekTemplateIndex,
			 int print)
{
    TPM_RC			rc = 0;
    uint16_t 			dataSize;
    unsigned char 		*data = NULL; 		/* freed @1 */
    INT32 			tmpDataSize;
    unsigned char 		*tmpData = NULL; 

    if (rc == 0) {
	rc = getIndexContents(tssContext,
			      &data,
			      &dataSize,
			      ekTemplateIndex);
    }
    /* unmarshal the data stream */
    if (rc == 0) {
	tmpData = data;		/* temps because unmarshal moves the pointers */
	tmpDataSize = dataSize;
	rc = TPMT_PUBLIC_Unmarshal(tpmtPublic, &tmpData, &tmpDataSize, YES);
    }
    /* optional tracing */
    if (rc == 0) {
	if (print) TSS_TPMT_PUBLIC_Print(tpmtPublic, 0);
    }
    free(data);   			/* @1 */
    return rc;
}

/* processEKCertificate() reads the EK certificate from NV and returns an openssl X509 certificate
   structure.  It alse extracts and returns the public modulus.

*/
    
TPM_RC processEKCertificate(TSS_CONTEXT *tssContext,
			    X509 **ekCertificate,	/* freed by caller */
			    uint8_t **modulusBin,	/* freed by caller */
			    int *modulusBytes,
			    TPMI_RH_NV_INDEX ekCertIndex,
			    int print)
{
    TPM_RC			rc = 0;

    /* read the EK X509 certificate from NV */
    if (rc == 0) {
	rc = getIndexX509Certificate(tssContext,
				     ekCertificate,	/* freed by caller */
				     ekCertIndex);
    }
    if (rc == 0) {
	rc = convertCertificatePubKey(modulusBin,	/* freed by caller */
				      modulusBytes,
				      *ekCertificate,
				      ekCertIndex,
				      print);
    }
    return rc;
}

/* convertCertificatePubKey() returns the public modulus from an openssl X509 certificate
   structure.  ekCertIndex determines whether the algorithm is RSA or ECC.  */

TPM_RC convertCertificatePubKey(uint8_t **modulusBin,	/* freed by caller */
				int *modulusBytes,
				X509 *ekCertificate,
				TPMI_RH_NV_INDEX ekCertIndex,
				int print)
{
    TPM_RC			rc = 0;
    EVP_PKEY 			*pkey = NULL;
    BIGNUM			*modulusBn;
    
    /* extract the public key */
    if (rc == 0) {
	pkey = X509_get_pubkey(ekCertificate);		/* freed @2 */
	if (pkey == NULL) {
	    printf("ERROR: Could not extract public key from X509 certificate\n");
	    rc = TPM_RC_INTEGRITY;
	}
    }
    if (ekCertIndex == EK_CERT_RSA_INDEX) {
	RSA *rsaKey = NULL;
	if (rc == 0) {
	    if (pkey->type != EVP_PKEY_RSA) {
		printf("ERROR: Public key from X509 certificate is not RSA\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	/* convert the public key to openssl structure */
	if (rc == 0) {
	    rsaKey = EVP_PKEY_get1_RSA(pkey);		/* freed @3 */
	    if (rsaKey == NULL) {
		printf("ERROR: Could not extract RSA public key from X509 certificate\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	/* convert the bignum to binary */
	if (rc == 0) {
	    modulusBn = rsaKey->n;
	    *modulusBytes = BN_num_bytes(modulusBn);
	    rc = TSS_Malloc(modulusBin, *modulusBytes);	/* freed by caller */
	}
	if (rc == 0) {
	    BN_bn2bin(rsaKey->n, *modulusBin);
	    if (print) TSS_PrintAll("Certificate public key:", *modulusBin, *modulusBytes);
	}    
	/* use openssl to print the X509 certificate */
	if (rc == 0) {
	    if (print) X509_print_fp(stdout, ekCertificate);
	}
	RSA_free(rsaKey);   		/* @3 */
    }
    else {
	EC_KEY *ecKey = NULL;
	const EC_POINT *ecPoint;
	const EC_GROUP *ecGroup;
	if (rc == 0) {
	    if (pkey->type != EVP_PKEY_EC) {
		printf("Public key from X509 certificate is not EC\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	/* convert the public key to openssl structure */
	if (rc == 0) {
	    ecKey = EVP_PKEY_get1_EC_KEY(pkey);		/* freed @3 */
	    if (ecKey == NULL) {
		printf("Could not extract EC public key from X509 certificate\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	if (rc == 0) {
	    ecPoint = EC_KEY_get0_public_key(ecKey);
	    if (ecPoint == NULL) {
		printf("Could not extract EC point from EC public key\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	if (rc == 0) {   
	    ecGroup = EC_KEY_get0_group(ecKey);
	    if (ecGroup  == NULL) {
		printf("Could not extract EC group from EC public key\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	if (rc == 0) {   
	    *modulusBytes = EC_POINT_point2oct(ecGroup, ecPoint,
					       POINT_CONVERSION_UNCOMPRESSED,
					       NULL, 0, NULL);
	    rc = TSS_Malloc(modulusBin, *modulusBytes);	/* freed by caller */
	}
	if (rc == 0) {
	    EC_POINT_point2oct(ecGroup, ecPoint,
			       POINT_CONVERSION_UNCOMPRESSED,
			       *modulusBin, *modulusBytes, NULL);
	    if (print) TSS_PrintAll("Certificate public key:", *modulusBin, *modulusBytes);
	}
	/* use openssl to print the X509 certificate */
	if (rc == 0) {
	    if (print) X509_print_fp(stdout, ekCertificate);
	}
	EC_KEY_free(ecKey);   		/* @3 */
    }
    EVP_PKEY_free(pkey);   		/* @2 */
    return rc;
}

/* processRoot() validates the certificate at ekCertIndex against the root CA certificate at
   rootFilename.
 */

TPM_RC processRoot(TSS_CONTEXT *tssContext,
		   TPMI_RH_NV_INDEX ekCertIndex,
		   const char *rootFilename[],
		   unsigned int rootFileCount,
		   int print)
{
    TPM_RC			rc = 0;
    unsigned int		i;
    X509 			*ekCertificate = NULL;		/* freed @1 */
    X509_STORE 			*caStore = NULL;		/* freed @3 */
    X509 			*caCert[MAX_ROOTS];		/* freed @4 */
    X509_STORE_CTX 		*verifyCtx = NULL;		/* freed @5 */

    /* for free */
    for (i = 0 ; i < rootFileCount ; i++) {
	caCert[i] = NULL;
    }
    /* read the EK X509 certificate from NV */
    if (rc == 0) {
	rc = getIndexX509Certificate(tssContext,
				     &ekCertificate,		/* freed @1 */
				     ekCertIndex);
    }
    /* get the root CA certificate chain */
    if (rc == 0) {
	rc = getCaStore(&caStore,			/* freed @3 */
			caCert,				/* freed @4 */
			rootFilename,
			rootFileCount);
    }
    /* create the certificate verify context */
    if (rc == 0) {
	verifyCtx = X509_STORE_CTX_new();
	if (verifyCtx == NULL) {
	    printf("processRoot: X509_STORE_CTX_new failed\n");  
	    rc = TSS_RC_OUT_OF_MEMORY;
	}
    }
    /* add the root certificate store and EK certificate to be verified to the verify context */
    if (rc == 0) {
	int irc = X509_STORE_CTX_init(verifyCtx, caStore, ekCertificate, NULL);
	if (irc != 1) {
	    printf("processRoot: Error in X509_STORE_CTX_init initialzing verify context\n");  
	    rc = TSS_RC_RSA_SIGNATURE;
	}	    
    }
    /* walk the certificate chain */
    if (rc == 0) {
	int irc = X509_verify_cert(verifyCtx);
	if (irc != 1) {
	    printf("processRoot: Error in X590_verify_cert verifying certificate\n");  
	    rc = TSS_RC_RSA_SIGNATURE;
	}
	else {
	    if (print) printf("EK certificate verified against the root\n");
	}
    }
    if (ekCertificate != NULL) {
	X509_free(ekCertificate);   	/* @1 */
    }
    if (caStore != NULL) {
	X509_STORE_free(caStore);	/* @3 */
    }
    for (i = 0 ; i < rootFileCount ; i++) {
	X509_free(caCert[i]);	   	/* @4 */
    }
    if (verifyCtx != NULL) {
	X509_STORE_CTX_free(verifyCtx);	/* @5 */
    }
    return rc;
}

/* processCreatePrimary() combines the EK nonce and EK template from NV to form the
   createprimary input.  It creates the primary key.

   ekCertIndex determines whether an RSA or ECC key is created.
   
   If nonce is NULL, the default IWG templates are used.  If nonce is non-NULL, the nonce and
   tpmtPublicIn are used.

   After returning the TPMT_PUBLIC, flushes the primary key unless noFlush is TRUE.  If noFlush is
   FALSE, returns the loaded handle, else returns TPM_RH_NULL.
*/

TPM_RC processCreatePrimary(TSS_CONTEXT *tssContext,
			    TPM_HANDLE *keyHandle,		/* primary key handle */
			    TPMI_RH_NV_INDEX ekCertIndex,
			    unsigned char *nonce,
			    uint16_t nonceSize,
			    TPMT_PUBLIC *tpmtPublicIn,		/* template */
			    TPMT_PUBLIC *tpmtPublicOut,		/* primary key */
			    unsigned int noFlush,	/* TRUE - don't flush the primary key */
			    int print)
{
    TPM_RC			rc = 0;
    CreatePrimary_In 		inCreatePrimary;
    CreatePrimary_Out 		outCreatePrimary;

    /* set up the createprimary in parameters */
    if (rc == 0) {
	inCreatePrimary.primaryHandle = TPM_RH_ENDORSEMENT;
	inCreatePrimary.inSensitive.t.sensitive.userAuth.t.size = 0;
	inCreatePrimary.inSensitive.t.sensitive.data.t.size = 0;
	/* creation data */
	inCreatePrimary.outsideInfo.t.size = 0;
	inCreatePrimary.creationPCR.count = 0;
    }
    /* construct the template from the NV template and nonce */
    if ((rc == 0) && (nonce != NULL)) {
	inCreatePrimary.inPublic.t.publicArea = *tpmtPublicIn;
	if (ekCertIndex == EK_CERT_RSA_INDEX) {			/* RSA primary key */
	    /* unique field is 256 bytes */
	    inCreatePrimary.inPublic.t.publicArea.unique.rsa.t.size = 256;
	    /* first part is nonce */
	    memcpy(inCreatePrimary.inPublic.t.publicArea.unique.rsa.t.buffer, nonce, nonceSize);
	    /* padded with zeros */
	    memset(inCreatePrimary.inPublic.t.publicArea.unique.rsa.t.buffer + nonceSize, 0,
		   256 - nonceSize);
	}
	else {							/* EC primary key */
	    /* unique field is X and Y points */
	    /* X gets nonce and pad */
	    inCreatePrimary.inPublic.t.publicArea.unique.ecc.x.t.size = 32;
	    memcpy(inCreatePrimary.inPublic.t.publicArea.unique.ecc.x.t.buffer, nonce, nonceSize);
	    memset(inCreatePrimary.inPublic.t.publicArea.unique.ecc.x.t.buffer + nonceSize, 0,
		   32 - nonceSize);
	    /* Y gets zeros */
	    inCreatePrimary.inPublic.t.publicArea.unique.ecc.y.t.size = 32;
	    memset(inCreatePrimary.inPublic.t.publicArea.unique.ecc.y.t.buffer, 0, 32);
	}
    }
    /* construct the template from the default IWG template */
    if ((rc == 0) && (nonce == NULL)) {
	if (ekCertIndex == EK_CERT_RSA_INDEX) {			/* RSA primary key */
	    getRsaTemplate(&inCreatePrimary.inPublic.t.publicArea);
	}
	else {							/* EC primary key */
	    getEccTemplate(&inCreatePrimary.inPublic.t.publicArea);
	}
    }
    /* call TSS to execute the command */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&outCreatePrimary,
			 (COMMAND_PARAMETERS *)&inCreatePrimary,
			 NULL,
			 TPM_CC_CreatePrimary,
			 TPM_RS_PW, NULL, 0,
			 TPM_RH_NULL, NULL, 0);
	if (rc != 0) {
	    const char *msg;
	    const char *submsg;
	    const char *num;
	    printf("createprimary: failed, rc %08x\n", rc);
	    TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	    printf("%s%s%s\n", msg, submsg, num);
	}
    }
    /* return the primary key */
    if (rc == 0) {
	*tpmtPublicOut = outCreatePrimary.outPublic.t.publicArea;
    }
    /* flush the primary key */
    if (rc == 0) {
	if (print) printf("Primary key Handle %08x\n", outCreatePrimary.objectHandle);
	if (!noFlush) {		/* flush the primary key */
	    *keyHandle = TPM_RH_NULL;	    
	    FlushContext_In 		inFlushContext;
	    inFlushContext.flushHandle = outCreatePrimary.objectHandle;
	    rc = TSS_Execute(tssContext,
			     NULL, 
			     (COMMAND_PARAMETERS *)&inFlushContext,
			     NULL,
			     TPM_CC_FlushContext,
			     TPM_RH_NULL, NULL, 0);
	    if (rc != 0) {
		const char *msg;
		const char *submsg;
		const char *num;
		printf("flushcontext: failed, rc %08x\n", rc);
		TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
		printf("%s%s%s\n", msg, submsg, num);
	    }
	}
	else {	/* not flushed, return the handle */
	    *keyHandle = outCreatePrimary.objectHandle;
	}
    }	    
    /* trace the public key */
    if (rc == 0) {
	if (ekCertIndex == EK_CERT_RSA_INDEX) {
	    if (print) TSS_PrintAll("createprimary: RSA public key",
				    outCreatePrimary.outPublic.t.publicArea.unique.rsa.t.buffer,
				    outCreatePrimary.outPublic.t.publicArea.unique.rsa.t.size);
	}
	else {
	    if (print) TSS_PrintAll("createprimary: ECC public key x",
				    outCreatePrimary.outPublic.t.publicArea.unique.ecc.x.t.buffer,
				    outCreatePrimary.outPublic.t.publicArea.unique.ecc.x.t.size);
	    if (print) TSS_PrintAll("createprimary: ECC public key y",
				    outCreatePrimary.outPublic.t.publicArea.unique.ecc.y.t.buffer,
				    outCreatePrimary.outPublic.t.publicArea.unique.ecc.y.t.size);
	}
    }
    return rc;
}

/* processValidatePrimary() compares the public key in the EK certificate to the public key output
   of createprimary.  */

TPM_RC processValidatePrimary(uint8_t *publicKeyBin,		/* from certificate */
			      int publicKeyBytes,
			      TPMT_PUBLIC *tpmtPublic,		/* primary key */
			      TPMI_RH_NV_INDEX ekCertIndex,
			      int print)
{
    TPM_RC			rc = 0;

    print = print;
    /* compare the X509 certificate public key to the createprimary public key */
    if (ekCertIndex == EK_CERT_RSA_INDEX) {
	int irc;
	/* RSA just has a public modulus */
	if (rc == 0) {
	    if (tpmtPublic->unique.rsa.t.size != publicKeyBytes) {
		printf("X509 certificate key length %u does not match output of createprimary %u\n",
		       publicKeyBytes,
		       tpmtPublic->unique.rsa.t.size);
		rc = TPM_RC_INTEGRITY;
	    }
	}
	if (rc == 0) {
	    irc = memcmp(publicKeyBin,
			 tpmtPublic->unique.rsa.t.buffer,
			 publicKeyBytes);
	    if (irc != 0) {
		printf("Public key from X509 certificate does not match output of createprimary\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
    }
    else {
	int irc;
	/* ECC has X and Y points */
	/* compression algorithm is the extra byte at the beginning of the certificate */
	if (rc == 0) {
	    if (tpmtPublic->unique.ecc.x.t.size +
		tpmtPublic->unique.ecc.x.t.size + 1
		!= publicKeyBytes) {
		printf("X509 certificate key length %u does not match "
		       "output of createprimary x %u +y %u\n",
		       publicKeyBytes,
		       tpmtPublic->unique.ecc.x.t.size,
		       tpmtPublic->unique.ecc.y.t.size);
		rc = TPM_RC_INTEGRITY;
	    }
	}
	/* check X */
	if (rc == 0) {
	    irc = memcmp(publicKeyBin +1,
			 tpmtPublic->unique.ecc.x.t.buffer,
			 tpmtPublic->unique.ecc.x.t.size);
	    if (irc != 0) {
		printf("Public key X from X509 certificate does not match "
		       "output of createprimary\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}
	/* check Y */
	if (rc == 0) {
	    irc = memcmp(publicKeyBin + 1 + tpmtPublic->unique.ecc.x.t.size,
			 tpmtPublic->unique.ecc.y.t.buffer,
			 tpmtPublic->unique.ecc.y.t.size);
	    if (irc != 0) {
		printf("Public key Y from X509 certificate does not match "
		       "output of createprimary\n");
		rc = TPM_RC_INTEGRITY;
	    }
	}	
    }
    if (rc == 0) {
	if (print) printf("processValidatePrimary: "
			    "Public key from X509 certificate matches output of createprimary\n");
    } 
    return rc;

}

/* processPrimary() reads the EK nonce and EK template from NV.  It combines them to form the
   createprimary input.  It creates the primary key.

   It reads the EK certificate from NV.  It extracts the public key.

   Finally, it compares the public key in the certificate to the public key output of createprimary.
*/

TPM_RC processPrimary(TSS_CONTEXT *tssContext,
		      TPMI_RH_NV_INDEX ekCertIndex,
		      TPMI_RH_NV_INDEX ekNonceIndex, 
		      TPMI_RH_NV_INDEX ekTemplateIndex,
		      unsigned int noFlush,		/* TRUE - don't flush the primary key */
		      int print)
{
    TPM_RC			rc = 0;
    X509 			*ekCertificate = NULL;
    unsigned char 		*nonce = NULL;
    uint16_t 			nonceSize;
    TPM_HANDLE			keyHandle;		/* primary key handle */
    TPMT_PUBLIC 		tpmtPublicIn;		/* template */
    TPMT_PUBLIC 		tpmtPublicOut;		/* primary key */
    uint8_t 			*publicKeyBin = NULL;	/* from certificate */
    int				publicKeyBytes;

    /* get the EK nonce */
    if (rc == 0) {
	rc = processEKNonce(tssContext, &nonce, &nonceSize, ekNonceIndex, print); /* freed @1 */
	if ((rc & 0xff) == TPM_RC_HANDLE) {
	    if (print) printf("EK nonce not found, use default template\n");
	    rc = 0;
	}
    }
    if (rc == 0) {
	/* if the nonce was found, get the EK template */
	if (nonce != NULL) {
	    rc = processEKTemplate(tssContext, &tpmtPublicIn, ekTemplateIndex, print);
	}
    }
    /* create the primary key */
    if (rc == 0) {
	rc = processCreatePrimary(tssContext,
				  &keyHandle,
				  ekCertIndex,
				  nonce, nonceSize,		/* EK nonce, can be NULL */
				  &tpmtPublicIn,		/* template */
				  &tpmtPublicOut,		/* primary key */
				  noFlush,
				  print);
    }
    /* get the EK certificate */
    if (rc == 0) {
	rc = processEKCertificate(tssContext,
				  &ekCertificate,			/* freed @2 */
				  &publicKeyBin, &publicKeyBytes,	/* freed @3 */
				  ekCertIndex,
				  print);
    }
    if (rc == 0) {
	rc = processValidatePrimary(publicKeyBin,	/* certificate */
				    publicKeyBytes,
				    &tpmtPublicOut,	/* primary key */
				    ekCertIndex,
				    print);
    }
    if (rc == 0) {
	if (print) printf("Public key from X509 certificate matches output of createprimary\n");
    } 
    free(nonce);			/* @1 */
    if (ekCertificate != NULL) {
	X509_free(ekCertificate);   	/* @2 */
    }
    free(publicKeyBin);			/* @3 */
    return rc;
}
