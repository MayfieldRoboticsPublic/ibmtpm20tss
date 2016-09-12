/********************************************************************************/
/*										*/
/*			   RSA_Decrypt						*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: rsadecrypt.c 682 2016-07-15 18:49:19Z kgoldman $		*/
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

/* 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <tss2/tss.h>
#include <tss2/tssutils.h>
#include <tss2/tssresponsecode.h>
#include <tss2/tssmarshal.h>
#include <tss2/tssprint.h>

static void printRsaDecrypt(RSA_Decrypt_Out *out);
static void printUsage(void);

int verbose = FALSE;

int main(int argc, char *argv[])
{
    TPM_RC			rc = 0;
    int				i;    /* argc iterator */
    TSS_CONTEXT			*tssContext = NULL;
    RSA_Decrypt_In 		in;
    RSA_Decrypt_Out 		out;
    TPMI_DH_OBJECT		keyHandle = 0;
    const char			*encryptFilename = NULL;
    const char			*decryptFilename = NULL;
    const char			*keyPassword = NULL; 
    TPMI_SH_AUTH_SESSION    	sessionHandle0 = TPM_RS_PW;
    unsigned int		sessionAttributes0 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle1 = TPM_RH_NULL;
    unsigned int		sessionAttributes1 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle2 = TPM_RH_NULL;
    unsigned int		sessionAttributes2 = 0;
 
    uint16_t			written;
    size_t			length;
    uint8_t			*buffer = NULL;		/* for the free */
    uint8_t			*buffer1 = NULL;	/* for marshaling */

    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "1");

    /* command line argument defaults */
    for (i=1 ; (i<argc) && (rc == 0) ; i++) {
	if (strcmp(argv[i],"-hk") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x",&keyHandle);
	    }
	    else {
		printf("Missing parameter for -hk\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pwdk") == 0) {
	    i++;
	    if (i < argc) {
		keyPassword = argv[i];
	    }
	    else {
		printf("-pwdk option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-ie") == 0) {
	    i++;
	    if (i < argc) {
		encryptFilename = argv[i];
	    }
	    else {
		printf("-ie option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-od") == 0) {
	    i++;
	    if (i < argc) {
		decryptFilename = argv[i];
	    }
	    else {
		printf("-od option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se0") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle0);
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes0);
		if (sessionAttributes0 > 0xff) {
		    printf("Out of range session attributes for -se0\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se1") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle1);
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes1);
		if (sessionAttributes1 > 0xff) {
		    printf("Out of range session attributes for -se1\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se2") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle2);
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes2);
		if (sessionAttributes2 > 0xff) {
		    printf("Out of range session attributes for -se2\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	}
 	else if (strcmp(argv[i],"-h") == 0) {
	    printUsage();
	}
	else if (strcmp(argv[i],"-v") == 0) {
	    verbose = TRUE;
	    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "2");
	}
	else {
	    printf("\n%s is not a valid option\n", argv[i]);
	    printUsage();
	}
    }
    if (keyHandle == 0) {
	printf("Missing handle parameter -hk\n");
	printUsage();
    }
    if (encryptFilename == NULL) {
	printf("Missing encrypted message -ie\n");
	printUsage();
    }
    if (rc == 0) {
	rc = TSS_File_ReadBinaryFile(&buffer,     /* must be freed by caller */
				     &length,
				     encryptFilename);
    }
    if (rc == 0) {
	/* Handle of key that will perform rsa decrypt */
	in.keyHandle = keyHandle;

	/* Table 158 - Definition of {RSA} TPM2B_PUBLIC_KEY_RSA Structure */
	{
	    in.cipherText.t.size = length;
	    memcpy(in.cipherText.t.buffer, buffer, length);
	}
	/* padding scheme */
	{
	    /* Table 157 - Definition of {RSA} TPMT_RSA_DECRYPT Structure */
	    in.inScheme.scheme = TPM_ALG_NULL;
	}
	/* label */
	{
	    /* Table 73 - Definition of TPM2B_DATA Structure */
	    in.label.t.size = 0;
	}
    }
    free (buffer);
    buffer = NULL;

    /* Start a TSS context */
    if (rc == 0) {
	rc = TSS_Create(&tssContext);
    }
    /* call TSS to execute the command */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&out,
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_RSA_Decrypt,
			 sessionHandle0, keyPassword, sessionAttributes0,
			 sessionHandle1, NULL, sessionAttributes1,
			 sessionHandle2, NULL, sessionAttributes2,
			 TPM_RH_NULL, NULL, 0);
    }
    {
	TPM_RC rc1 = TSS_Delete(tssContext);
	if (rc == 0) {
	    rc = rc1;
	}
    }
    if ((rc == 0) && (decryptFilename != NULL)) {
	written = 0;
	rc = TSS_TPM2B_PUBLIC_KEY_RSA_Marshal(&out.message, &written, NULL, NULL);
    }    
    if ((rc == 0) && (decryptFilename != NULL)) {
	buffer = realloc(buffer, written);
	buffer1 = buffer;
	written = 0;
	rc = TSS_TPM2B_PUBLIC_KEY_RSA_Marshal(&out.message, &written, &buffer1, NULL);
    }    
    if ((rc == 0) && (decryptFilename != NULL)) {
	rc = TSS_File_WriteBinaryFile(buffer + sizeof(uint16_t),
				      written - sizeof(uint16_t),
				      decryptFilename); 
    }    
    free(buffer);
    if (rc == 0) {
	if (verbose) printRsaDecrypt(&out);
	if (verbose) printf("rsadecrypt: success\n");
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("rsadecrypt: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    return rc;
}

static void printRsaDecrypt(RSA_Decrypt_Out *out)
{
    TSS_PrintAll("outData", out->message.t.buffer, out->message.t.size);
}

static void printUsage(void)
{
    printf("\n");
    printf("rsadecrypt\n");
    printf("\n");
    printf("Runs TPM2_Rsadecrypt\n");
    printf("\n");
    printf("\t-hk key handle\n");
    printf("\t-pwdk password for key (default empty)\n");
    printf("\t-ie encrypt file name\n");
    printf("\t-od decrypt file name\n");
    printf("\n");
    printf("\t-se[0-2] session handle / attributes (default PWAP)\n");
    printf("\t\t01 continue\n");
    exit(1);	
}
