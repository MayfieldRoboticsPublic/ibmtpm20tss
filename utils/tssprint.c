/********************************************************************************/
/*										*/
/*			     Structure Print Utilities				*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: tssprint.c 703 2016-07-28 17:21:46Z kgoldman $		*/
/*										*/
/* (c) Copyright IBM Corporation 2015.						*/
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
#include <inttypes.h>

#include <tss2/Unmarshal_fp.h>

#include <tss2/tssprint.h>

/* TSS_PrintAll() prints 'string', the length, and then the entire byte array
 */

void TSS_PrintAll(const char *string, const unsigned char* buff, uint32_t length)
{
    TSS_PrintAlli(string, 1, buff, length);
}

/* TSS_PrintAlli() prints 'string', the length, and then the entire byte array
   
   Each line indented 'indent' spaces.
*/

void TSS_PrintAlli(const char *string, unsigned int indent, const unsigned char* buff, uint32_t length)
{
    uint32_t i;
    if (buff != NULL) {
        printf("%*s" "%s length %u\n" "%*s", indent, "", string, length, indent, "");
        for (i = 0 ; i < length ; i++) {
            if (i && !( i % 16 )) {
                printf("\n" "%*s", indent, "");
            }
            printf("%.2x ",buff[i]);
        }
        printf("\n");
    }
    else {
        printf("%*s" "%s null\n", indent, "", string);
    }
    return;
}

/* Table 9 - Definition of (UINT16) TPM_ALG_ID Constants <IN/OUT, S> */

void TSS_TPM_ALG_ID_Print(TPM_ALG_ID source, unsigned int indent)
{
    switch (source) {
#ifdef TPM_ALG_AES
      case TPM_ALG_AES:
	printf("%*s" "TPM_ALG_ID TPM_ALG_AES\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_CAMELLIA
      case TPM_ALG_CAMELLIA:
	printf("%*s" "TPM_ALG_ID TPM_ALG_CAMELLIA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_CBC
      case TPM_ALG_CBC:
	printf("%*s" "TPM_ALG_ID TPM_ALG_CBC\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_CFB
      case TPM_ALG_CFB:
	printf("%*s" "TPM_ALG_ID TPM_ALG_CFB\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_CTR
      case TPM_ALG_CTR:
	printf("%*s" "TPM_ALG_ID TPM_ALG_CTR\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECB
      case TPM_ALG_ECB:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECB\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECC
      case TPM_ALG_ECC:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECC\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECDAA
      case TPM_ALG_ECDAA:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECDAA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECDH
      case TPM_ALG_ECDH:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECDH\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECDSA
      case TPM_ALG_ECDSA:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECDSA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECMQV
      case TPM_ALG_ECMQV:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECMQV\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECSCHNORR
      case TPM_ALG_ECSCHNORR:
	printf("%*s" "TPM_ALG_ID TPM_ALG_ECSCHNORR\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_HMAC
      case TPM_ALG_HMAC:
	printf("%*s" "TPM_ALG_ID TPM_ALG_HMAC\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_KDF1_SP800_108
      case TPM_ALG_KDF1_SP800_108:
	printf("%*s" "TPM_ALG_ID TPM_ALG_KDF1_SP800_108\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_KDF1_SP800_56A
      case TPM_ALG_KDF1_SP800_56A:
	printf("%*s" "TPM_ALG_ID TPM_ALG_KDF1_SP800_56A\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_KDF2
      case TPM_ALG_KDF2:
	printf("%*s" "TPM_ALG_ID TPM_ALG_KDF2\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_KEYEDHASH
      case TPM_ALG_KEYEDHASH:
	printf("%*s" "TPM_ALG_ID TPM_ALG_KEYEDHASH\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_MGF1
      case TPM_ALG_MGF1:
	printf("%*s" "TPM_ALG_ID TPM_ALG_MGF1\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_OAEP
      case TPM_ALG_OAEP:
	printf("%*s" "TPM_ALG_ID TPM_ALG_OAEP\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_OFB
      case TPM_ALG_OFB:
	printf("%*s" "TPM_ALG_ID TPM_ALG_OFB\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_RSA
      case TPM_ALG_RSA:
	printf("%*s" "TPM_ALG_ID TPM_ALG_RSA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_RSAES
      case TPM_ALG_RSAES:
	printf("%*s" "TPM_ALG_ID TPM_ALG_RSAES\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_RSAPSS
      case TPM_ALG_RSAPSS:
	printf("%*s" "TPM_ALG_ID TPM_ALG_RSAPSS\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_RSASSA
      case TPM_ALG_RSASSA:
	printf("%*s" "TPM_ALG_ID TPM_ALG_RSASSA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SHA1
      case TPM_ALG_SHA1:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SHA1\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SHA256
      case TPM_ALG_SHA256:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SHA256\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SHA384
      case TPM_ALG_SHA384:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SHA384\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SHA512
      case TPM_ALG_SHA512:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SHA512\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SM2
      case TPM_ALG_SM2:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SM2\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SM3_256
      case TPM_ALG_SM3_256:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SM3_256\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SM4
      case TPM_ALG_SM4:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SM4\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SYMCIPHER
      case TPM_ALG_SYMCIPHER:
	printf("%*s" "TPM_ALG_ID TPM_ALG_SYMCIPHER\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_XOR
      case TPM_ALG_XOR:
	printf("%*s" "TPM_ALG_ID TPM_ALG_XOR\n", indent, "");
	break;
#endif
      case TPM_ALG_NULL:
	printf("%*s" "TPM_ALG_ID TPM_ALG_NULL\n", indent, "");
	break;
      default:
	printf("%*s" "TPM_ALG_ID algorithm %04hx unknown\n", indent, "", source);
    }
    return;
}

/* Table 30 - Definition of (UINT32) TPMA_ALGORITHM Bits */

void TSS_TPM_TPMA_ALGORITHM_Print(TPMA_ALGORITHM source, unsigned int indent)
{
    if (source.val &TPMA_ALGORITHM_ASYMMETRIC) printf("%*s" "TPMA_ALGORITHM: asymmetric\n", indent, "");
    if (source.val &TPMA_ALGORITHM_SYMMETRIC) printf("%*s" "TPMA_ALGORITHM: symmetric\n", indent, "");
    if (source.val &TPMA_ALGORITHM_HASH) printf("%*s" "TPMA_ALGORITHM: hash\n", indent, "");
    if (source.val &TPMA_ALGORITHM_OBJECT) printf("%*s" "TPMA_ALGORITHM: object\n", indent, "");
    if (source.val &TPMA_ALGORITHM_SIGNING) printf("%*s" "TPMA_ALGORITHM: signing\n", indent, "");
    if (source.val &TPMA_ALGORITHM_ENCRYPTING) printf("%*s" "TPMA_ALGORITHM: encrypting\n", indent, "");
    if (source.val &TPMA_ALGORITHM_METHOD) printf("%*s" "TPMA_ALGORITHM: method\n", indent, "");
    return;
}

/* Table 32 - Definition of (UINT32) TPMA_OBJECT Bits */

void TSS_TPMA_OBJECT_Print(TPMA_OBJECT source, unsigned int indent)
{
    if (source.val & TPMA_OBJECT_FIXEDTPM) printf("%*s" "TPMA_OBJECT: fixedTpm\n", indent, "");
    if (source.val & TPMA_OBJECT_STCLEAR) printf("%*s" "TPMA_OBJECT: stClear\n", indent, "");
    if (source.val & TPMA_OBJECT_FIXEDPARENT) printf("%*s" "TPMA_OBJECT: fixedParent\n", indent, "");
    if (source.val & TPMA_OBJECT_SENSITIVEDATAORIGIN) printf("%*s" "TPMA_OBJECT: sensitiveDataOrigin\n", indent, "");
    if (source.val & TPMA_OBJECT_USERWITHAUTH) printf("%*s" "TPMA_OBJECT: userWithAuth\n", indent, "");
    if (source.val & TPMA_OBJECT_ADMINWITHPOLICY) printf("%*s" "TPMA_OBJECT: adminWithPolicy\n", indent, "");
    if (source.val & TPMA_OBJECT_NODA) printf("%*s" "TPMA_OBJECT: noDA\n", indent, "");
    if (source.val & TPMA_OBJECT_ENCRYPTEDDUPLICATION) printf("%*s" "TPMA_OBJECT: encryptedDuplication\n", indent, "");
    if (source.val & TPMA_OBJECT_RESTRICTED) printf("%*s" "TPMA_OBJECT: restricted\n", indent, "");
    if (source.val & TPMA_OBJECT_DECRYPT) printf("%*s" "TPMA_OBJECT: decrypt\n", indent, "");
    if (source.val & TPMA_OBJECT_SIGN) printf("%*s" "TPMA_OBJECT: sign\n", indent, "");
    return;
}

/* Table 85 - Definition of TPMS_PCR_SELECTION Structure */

void TSS_TPMS_PCR_SELECTION_Print(TPMS_PCR_SELECTION *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->hash, indent+2);
    TSS_PrintAlli("TPMS_PCR_SELECTION", indent+2,
		  source->pcrSelect,
		  source->sizeofSelect);
    return;
}

/* Table 102 - Definition of TPML_PCR_SELECTION Structure */

void TSS_TPML_PCR_SELECTION_Print(TPML_PCR_SELECTION *source, unsigned int indent)
{
    uint32_t i;
    printf("%*s" "TPML_PCR_SELECTION count %u\n", indent, "", source->count);
    for (i = 0 ; (i < source->count) ; i++) {
	TSS_TPMS_PCR_SELECTION_Print(&source->pcrSelections[i], indent);
    }
    return;
}

/* Table 109 - Definition of TPMS_CLOCK_INFO Structure */

void TSS_TPMS_CLOCK_INFO_Print(TPMS_CLOCK_INFO *source, unsigned int indent)
{
    printf("%*s" "TPMS_CLOCK_INFO clock %"PRIu64"\n", indent, "", source->clock);
    printf("%*s" "TPMS_CLOCK_INFO resetCount %u\n", indent, "", source->resetCount);
    printf("%*s" "TPMS_CLOCK_INFO restartCount %u\n", indent, "", source->restartCount);
    printf("%*s" "TPMS_CLOCK_INFO safe %x\n", indent, "", source->safe);
    return;
}

/* Table 110 - Definition of TPMS_TIME_INFO Structure */

void TSS_TPMS_TIME_INFO_Print(TPMS_TIME_INFO *source, unsigned int indent)
{
    printf("%*s" "TPMS_TIME_INFO time %"PRIu64"\n", indent, "", source->time);
    TSS_TPMS_CLOCK_INFO_Print(&source->clockInfo, indent+2);
    return;
}
    
/* Table 111 - Definition of TPMS_TIME_ATTEST_INFO Structure <OUT> */

void TSS_TPMS_TIME_ATTEST_INFO_Print(TPMS_TIME_ATTEST_INFO *source, unsigned int indent)
{
    TSS_TPMS_TIME_INFO_Print(&source->time, indent+2);
    printf("%*s" "TPMS_TIME_ATTEST_INFO firmwareVersion %"PRIu64"\n", indent, "", source->firmwareVersion);
    return;
}

/* Table 112 - Definition of TPMS_CERTIFY_INFO Structure <OUT> */

void TSS_TPMS_CERTIFY_INFO_Print(TPMS_CERTIFY_INFO *source, unsigned int indent)
{
    TSS_PrintAlli("TPMS_CERTIFY_INFO name", indent,
		  source->name.b.buffer,
		  source->name.b.size);
    TSS_PrintAlli("TPMS_CERTIFY_INFO qualifiedName", indent,
		  source->qualifiedName.b.buffer,
		  source->qualifiedName.b.size);
    return;
}

/* Table 113 - Definition of TPMS_QUOTE_INFO Structure <OUT> */

void TSS_TPMS_QUOTE_INFO_Print(TPMS_QUOTE_INFO *source, unsigned int indent)
{
    TSS_TPML_PCR_SELECTION_Print(&source->pcrSelect, indent+2);
    TSS_PrintAlli("TPMS_QUOTE_INFO pcrDigest", indent+2,
		  source->pcrDigest.b.buffer,
		  source->pcrDigest.b.size);
    return;
}

/* Table 115 - Definition of TPMS_SESSION_AUDIT_INFO Structure <OUT> */

void TSS_TPMS_SESSION_AUDIT_INFO_Print(TPMS_SESSION_AUDIT_INFO *source, unsigned int indent)
{
    printf("%*s" "TPMS_SESSION_AUDIT_INFO exclusiveSession %d\n", indent, "",  source->exclusiveSession);
    TSS_PrintAlli("TPMS_SESSION_AUDIT_INFO sessionDigest", indent,
		  source->sessionDigest.b.buffer,
		  source->sessionDigest.b.size);
    return;
}

/* Table 118 - Definition of (TPM_ST) TPMI_ST_ATTEST Type <OUT> */

void TSS_TPMI_ST_ATTEST_Print(TPMI_ST_ATTEST selector, unsigned int indent)
{
    switch (selector) {
      case TPM_ST_ATTEST_CERTIFY:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_CERTIFY\n", indent, "");
	break;
      case TPM_ST_ATTEST_CREATION:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_CREATION\n", indent, "");
	break;
      case TPM_ST_ATTEST_QUOTE:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_QUOTE\n", indent, "");
	break;
      case TPM_ST_ATTEST_COMMAND_AUDIT:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_COMMAND_AUDIT\n", indent, "");
	break;
      case TPM_ST_ATTEST_SESSION_AUDIT:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_SESSION_AUDIT\n", indent, "");
	break;
      case TPM_ST_ATTEST_TIME:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_TIME\n", indent, "");
	break;
      case TPM_ST_ATTEST_NV:
	printf("%*s" "TPMI_ST_ATTEST TPM_ST_ATTEST_NV\n", indent, "");
	break;
      default:
	printf("%*s" "TPMI_ST_ATTEST_Print: selection %04hx not implemented\n", indent, "", selector);
    }
    return;
    
}

/* Table 119 - Definition of TPMU_ATTEST Union <OUT> */

void TSS_TPMU_ATTEST_Print(TPMU_ATTEST *source, TPMI_ST_ATTEST selector, unsigned int indent)
{
    switch (selector) {
      case TPM_ST_ATTEST_CERTIFY:
	TSS_TPMS_CERTIFY_INFO_Print(&source->certify, indent+2);
	break;
#if 0
      case TPM_ST_ATTEST_CREATION:
	TSS_TPMS_CREATION_INFO_Print(&source->creation, indent+2);
	break;
#endif
      case TPM_ST_ATTEST_QUOTE:
	TSS_TPMS_QUOTE_INFO_Print(&source->quote, indent+2);
	break;
#if 0
      case TPM_ST_ATTEST_COMMAND_AUDIT:
	TSS_TPMS_COMMAND_AUDIT_INFO_Print(&source->commandAudit, indent+2);
	break;
#endif
      case TPM_ST_ATTEST_SESSION_AUDIT:
	TSS_TPMS_SESSION_AUDIT_INFO_Print(&source->sessionAudit, indent+2);
	break;
      case TPM_ST_ATTEST_TIME:
	TSS_TPMS_TIME_ATTEST_INFO_Print(&source->time, indent+2);
	break;
#if 0
      case TPM_ST_ATTEST_NV:
	TSS_TPMS_NV_CERTIFY_INFO_Print(&source->nv, indent+2);
	break;
#endif
      default:
	printf("%*s" "TPMU_ATTEST selection %04hx not implemented\n", indent, "", selector);
	
    }
    return;
}

/* Table 120 - Definition of TPMS_ATTEST Structure <OUT> */

void TSS_TPMS_ATTEST_Print(TPMS_ATTEST *source, unsigned int indent)
{
    printf("%*s" "TPMS_ATTEST magic %08x\n", indent, "", source->magic);
    TSS_TPMI_ST_ATTEST_Print(source->type, indent+2);
    TSS_PrintAlli("TPMS_ATTEST extraData", indent+2,
		  source->extraData.b.buffer,
		  source->extraData.b.size);
    TSS_TPMS_CLOCK_INFO_Print(&source->clockInfo, indent+2);
    TSS_TPMU_ATTEST_Print(&source->attested, source->type, indent+2);
    return;
}

/* Table 121 - Definition of TPM2B_ATTEST Structure <OUT> */

void TSS_TPM2B_ATTEST_Print(TPM2B_ATTEST *source, unsigned int indent)
{
    TPM_RC			rc = 0;
    TPMS_ATTEST 		attests;
    INT32			size;
    uint8_t			*buffer = NULL;

    /* unmarshal the TPMS_ATTEST from the TPM2B_ATTEST */
    if (rc == 0) {
	buffer = source->t.attestationData;
	size = source->t.size;
	rc = TPMS_ATTEST_Unmarshal(&attests, &buffer, &size);
    }
    if (rc == 0) {
	TSS_TPMS_ATTEST_Print(&attests, indent+2);
    }
    else {
	printf("%*s" "TPMS_ATTEST_Unmarshal failed\n", indent, "");
    }
    return;
}

/* Table 129 - Definition of TPMT_SYM_DEF_OBJECT Structure */

void TSS_TPMT_SYM_DEF_OBJECT_Print(TPMT_SYM_DEF_OBJECT *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->algorithm, indent+2);
    printf("%*s" "TPMU_SYM_KEY_BITS: %u\n", indent, "", source->keyBits.sym);
    TSS_TPM_ALG_ID_Print(source->mode.sym, indent+2);
    return;
}

/* Table 150 - Definition of TPMT_KDF_SCHEME Structure */

void TSS_TPMT_KDF_SCHEME_Print(TPMT_KDF_SCHEME *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->scheme, indent+2);
    if (source->scheme != TPM_ALG_NULL) {
	TSS_TPM_ALG_ID_Print(source->details.mgf1.hashAlg, indent+2);
    }
    return;
}

/* Table 155 - Definition of {RSA} TPMT_RSA_SCHEME Structure */


void TSS_TPMT_RSA_SCHEME_Print(TPMT_RSA_SCHEME *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->scheme, indent+2);
    if (source->scheme != TPM_ALG_NULL) {
	TSS_TPM_ALG_ID_Print(source->details.anySig.hashAlg, indent+2);
    }
    return;
}

/* Table 159 - Definition of {RSA} (TPM_KEY_BITS) TPMI_RSA_KEY_BITS Type */

void TSS_TPMI_RSA_KEY_BITS_Print(TPMI_RSA_KEY_BITS source, unsigned int indent)
{
    printf("%*s" "TPM_KEY_BITS: %u\n", indent, "", source);
    return;
}

/* Table 165 - Definition of {ECC} (TPM_ECC_CURVE) TPMI_ECC_CURVE Type */

void TSS_TPMI_ECC_CURVE_Print(TPMI_ECC_CURVE source, unsigned int indent)
{
    switch (source) {
#ifdef TPM_ECC_BN_P256
      case TPM_ECC_BN_P256:
	printf("%*s" "TPMI_ECC_CURVE TPM_ECC_BN_P256\n", indent, "");
	break;
#endif
#ifdef TPM_ECC_NIST_P256
      case TPM_ECC_NIST_P256:
	printf("%*s" "TPMI_ECC_CURVE TPM_ECC_NIST_P256\n", indent, "");
	break;
#endif
#ifdef TPM_ECC_NIST_P384
      case TPM_ECC_NIST_P384:
	printf("%*s" "TPMI_ECC_CURVE TPM_ECC_NIST_P384\n", indent, "");
	break;
#endif
      default:
	printf("%*s" "TPMI_ECC_CURVE %04hx unknown\n", indent, "", source);
    }
    return;
}

/* Table 166 - Definition of (TPMT_SIG_SCHEME) {ECC} TPMT_ECC_SCHEME Structure */

void TSS_TPMT_ECC_SCHEME_Print(TPMT_ECC_SCHEME *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->scheme, indent+2);
    if (source->scheme != TPM_ALG_NULL) {
	TSS_TPM_ALG_ID_Print(source->details.anySig.hashAlg, indent+2);
    }
    return;
}

/* Table 168 - Definition of {RSA} TPMS_SIGNATURE_RSA Structure */

void TSS_TPMS_SIGNATURE_RSA_Print(TPMS_SIGNATURE_RSA *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->hash, indent+2);
    TSS_PrintAlli("TPMS_SIGNATURE_RSA", indent+2,
		  source->sig.t.buffer,
		  source->sig.t.size);
    return;
}

/* Table 169 - Definition of Types for {RSA} Signature */

void TSS_TPMS_SIGNATURE_RSASSA_Print(TPMS_SIGNATURE_RSASSA *source, unsigned int indent)
{
    TSS_TPMS_SIGNATURE_RSA_Print(source, indent+2);
    return;
}

/* Table 172 - Definition of TPMU_SIGNATURE Union <IN/OUT, S> */

void TSS_TPMU_SIGNATURE_Print(TPMU_SIGNATURE *source, TPMI_ALG_SIG_SCHEME selector, unsigned int indent)
{
    switch (selector) {
      case TPM_ALG_RSASSA:
	TSS_TPMS_SIGNATURE_RSASSA_Print(&source->rsassa, indent+2);
	break;
#if 0
      case TPM_ALG_RSAPSS:
	TSS_TPMS_SIGNATURE_RSAPSS_Print(&source->rsapss, indent+2);
	break;
      case TPM_ALG_ECDSA:
	TSS_TPMS_SIGNATURE_ECDSA_Print(&source->ecdsa, indent+2);
	break;
      case TPM_ALG_ECDAA:
	TSS_TPMS_SIGNATURE_ECDSA_Print(&source->ecdaa, indent+2);
	break;
      case TPM_ALG_SM2:
	TSS_TPMS_SIGNATURE_ECDSA_Print(&source->sm2, indent+2);
	break;
      case TPM_ALG_ECSCHNORR:
	TSS_TPMS_SIGNATURE_ECDSA_Print(&source->ecschnorr, indent+2);
	break;
      case TPM_ALG_HMAC:
	TSS_TPMT_HA_Print(&source->hmac, indent+2);
	break;
#endif
      default:
	printf("%*s" "TPMU_SIGNATURE selection not implemented\n", indent, "");
	
    }
}

/* Table 173 - Definition of TPMT_SIGNATURE Structure */

void TSS_TPMT_SIGNATURE_Print(TPMT_SIGNATURE *source, unsigned int indent)
{
    TSS_TPM_ALG_ID_Print(source->sigAlg, indent+2);
    TSS_TPMU_SIGNATURE_Print(&source->signature, source->sigAlg, indent+2);
    return;
}

/* Table 176 - Definition of (TPM_ALG_ID) TPMI_ALG_PUBLIC Type */

void TSS_TPMI_ALG_PUBLIC_Print(TPMI_ALG_PUBLIC source, unsigned int indent)
{
    switch (source) {
#ifdef TPM_ALG_KEYEDHASH
      case TPM_ALG_KEYEDHASH:
	printf("%*s" "TPMI_ALG_PUBLIC: TPM_ALG_KEYEDHASH\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_RSA
      case TPM_ALG_RSA:
	printf("%*s" "TPMI_ALG_PUBLIC: TPM_ALG_RSA\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_ECC
      case TPM_ALG_ECC:
	printf("%*s" "TPMI_ALG_PUBLIC: TPM_ALG_ECC\n", indent, "");
	break;
#endif
#ifdef TPM_ALG_SYMCIPHER
      case TPM_ALG_SYMCIPHER:
	printf("%*s" "TPMI_ALG_PUBLIC: TPM_ALG_SYMCIPHER\n", indent, "");
	break;
#endif
      default:
	printf("%*s" "TPMI_ALG_PUBLIC: %04hx not implemented\n", indent, "", source);
    }
    return;
}
    
/* Table 177 - Definition of TPMU_PUBLIC_ID Union <IN/OUT, S> */

void TSS_TPMU_PUBLIC_ID_Print(TPMU_PUBLIC_ID *source, TPMI_ALG_PUBLIC selector, unsigned int indent)
{
    switch (selector) {
#ifdef TPM_ALG_KEYEDHASH
      case TPM_ALG_KEYEDHASH:
	TSS_PrintAlli("TPM_ALG_KEYEDHASH", indent,
		      source->keyedHash.b.buffer,
		      source->keyedHash.b.size);
	break;
#endif
#ifdef TPM_ALG_SYMCIPHER
      case TPM_ALG_SYMCIPHER:
	TSS_PrintAlli("TPM_ALG_SYMCIPHER", indent,
		      source->sym.b.buffer,
		      source->sym.b.size);
	break;
#endif
#ifdef TPM_ALG_RSA
      case TPM_ALG_RSA: 
	TSS_PrintAlli("TPM_ALG_RSA", indent,
		      source->rsa.b.buffer,
		      source->rsa.b.size);
	break;
#endif
#ifdef TPM_ALG_ECC
      case TPM_ALG_ECC:
	TSS_PrintAlli("TPM_ALG_ECC x", indent,
		      source->ecc.x.b.buffer,
		      source->ecc.x.b.size);
	TSS_PrintAlli("TPM_ALG_ECC y", indent,
		      source->ecc.y.b.buffer,
		      source->ecc.y.b.size);
	break;
#endif
      default:
	printf("%*s" "TPMU_PUBLIC_ID_Print: selection %04hx not implemented\n", indent, "", selector);
    }
    return;
}

/* Table 180 - Definition of {RSA} TPMS_RSA_PARMS Structure */

void TSS_TPMS_RSA_PARMS_Print(TPMS_RSA_PARMS *source, unsigned int indent)
{
    TSS_TPMT_SYM_DEF_OBJECT_Print(&source->symmetric, indent+2);
    TSS_TPMT_RSA_SCHEME_Print(&source->scheme, indent+2);
    TSS_TPMI_RSA_KEY_BITS_Print(source->keyBits, indent+2);
    printf("%*s" "TPMS_RSA_PARMS exponent %08x\n", indent, "", source->exponent);
    return;
}

/* Table 181 - Definition of {ECC} TPMS_ECC_PARMS Structure */

void TSS_TPMS_ECC_PARMS_Print(TPMS_ECC_PARMS *source, unsigned int indent)
{
    TSS_TPMT_SYM_DEF_OBJECT_Print(&source->symmetric, indent+2);
    TSS_TPMT_ECC_SCHEME_Print(&source->scheme, indent+2);
    TSS_TPMI_ECC_CURVE_Print(source->curveID, indent+2);
    TSS_TPMT_KDF_SCHEME_Print(&source->kdf, indent+2);
    return;
}

/* Table 182 - Definition of TPMU_PUBLIC_PARMS Union <IN/OUT, S> */

void TSS_TPMU_PUBLIC_PARMS_Print(TPMU_PUBLIC_PARMS *source, uint32_t selector, unsigned int indent)
{
    switch (selector) {
#if 0
      case TPM_ALG_KEYEDHASH:
	TSS_TPMS_KEYEDHASH_PARMS_Print(&source->keyedHashDetail, indent+2);
	break;
#endif
#if 0
      case TPM_ALG_SYMCIPHER:
	TSS_TPMS_SYMCIPHER_PARMS_Print(&source->symDetail, indent+2);
	break;
#endif
#ifdef TPM_ALG_RSA
      case TPM_ALG_RSA:
	TSS_TPMS_RSA_PARMS_Print(&source->rsaDetail, indent+2);
	break;
#endif
#ifdef TPM_ALG_ECC
      case TPM_ALG_ECC:
	TSS_TPMS_ECC_PARMS_Print(&source->eccDetail, indent+2);
	break;
#endif
      default:
	printf("%*s" "TPMU_PUBLIC_PARMS : selector %08hx not implemented\n", indent, "", selector);
    }
    return;
}

/* Table 184 - Definition of TPMT_PUBLIC Structure */

void TSS_TPMT_PUBLIC_Print(TPMT_PUBLIC *source, unsigned int indent)
{
    TSS_TPMI_ALG_PUBLIC_Print(source->type, indent+2);
    TSS_TPM_ALG_ID_Print(source->nameAlg, indent+2);
    TSS_TPMA_OBJECT_Print(source->objectAttributes, indent+2);	
    TSS_PrintAlli("authPolicy: ", indent+2,
		  source->authPolicy.b.buffer, source->authPolicy.b.size);
    TSS_TPMU_PUBLIC_PARMS_Print(&source->parameters, source->type, indent+2);		
    TSS_TPMU_PUBLIC_ID_Print(&source->unique, source->type, indent+2);			
    return;
}
 

