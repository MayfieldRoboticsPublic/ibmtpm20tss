/********************************************************************************/
/*										*/
/*			    NV ReadPublic					*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: nvreadpublic.c 682 2016-07-15 18:49:19Z kgoldman $		*/
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
#include <tss2/tssprint.h>
#include <tss2/tsscrypto.h>

/* for endian conversion */
#ifdef TPM_POSIX
#include <netinet/in.h>
#endif
#ifdef TPM_WINDOWS
#include <winsock2.h>
#endif

static void printUsage(void);

int verbose = FALSE;

int main(int argc, char *argv[])
{
    TPM_RC			rc = 0;
    int				i;    /* argc iterator */
    TSS_CONTEXT			*tssContext = NULL;
    NV_ReadPublic_In 		in;
    NV_ReadPublic_Out		out;
    TPMI_RH_NV_INDEX		nvIndex = 0;
    TPMI_ALG_HASH		nalg = TPM_ALG_SHA256;
    TPMI_ALG_HASH 		nameHashAlg;
   
    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "1");

    for (i=1 ; (i<argc) && (rc == 0) ; i++) {
	if (strcmp(argv[i],"-ha") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &nvIndex);
	    }
	    else {
		printf("Missing parameter for -ha\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-nalg") == 0) {
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"sha1") == 0) {
		    nalg = TPM_ALG_SHA1;
		}
		else if (strcmp(argv[i],"sha256") == 0) {
		    nalg = TPM_ALG_SHA256;
		}
		else if (strcmp(argv[i],"sha384") == 0) {
		    nalg = TPM_ALG_SHA384;
		}
		else {
		    printf("Bad parameter for -nalg\n");
		    printUsage();
		}
	    }
	    else {
		printf("-nalg option needs a value\n");
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
    if ((nvIndex >> 24) != TPM_HT_NV_INDEX) {
	printf("NV index handle not specified or out of range, MSB not 01\n");
	printUsage();
    }
    if (rc == 0) {
	in.nvIndex = nvIndex;
    }
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
			 TPM_CC_NV_ReadPublic,
			 TPM_RH_NULL, NULL, 0);
    }
    {
	TPM_RC rc1 = TSS_Delete(tssContext);
	if (rc == 0) {
	    rc = rc1;
	}
    }
    /* NOTE: The caller validates the result to the extent that it does not trust the NV index to be
       defined properly */
    
    /* Table 197 - Definition of TPM2B_NV_PUBLIC Structure - nvPublic*/
    /* Table 196 - Definition of TPMS_NV_PUBLIC Structure */
    /* Table 83 - Definition of TPM2B_NAME Structure t */

    /* TPMS_NV_PUBLIC hash alg vs expected */
    if (rc == 0) {
	if (out.nvPublic.t.nvPublic.nameAlg != nalg) {
	    printf("nvreadpublic: TPM2B_NV_PUBLIC hash algorithm does not match expected\n");
	    rc = TSS_RC_MALFORMED_NV_PUBLIC;
	}
    }
    /* TPM2B_NAME hash algorithm vs expected */
    if (rc == 0) {
	uint16_t tmp16;
	memcpy(&tmp16, out.nvName.t.name, sizeof(uint16_t));
	/* nameHashAlg = ntohs(*(TPMI_ALG_HASH *)(out.nvName.t.name)); */
	nameHashAlg = ntohs(tmp16);
	if (nameHashAlg != nalg) {
	    printf("nvreadpublic: TPM2B_NAME hash algorithm does not match expected\n");
	    rc = TSS_RC_MALFORMED_NV_PUBLIC;
	}
    }
    /* TPMS_NV_PUBLIC index vs expected */
    if (rc == 0) {
	if (out.nvPublic.t.nvPublic.nvIndex != in.nvIndex) {
	    printf("nvreadpublic: TPM2B_NV_PUBLIC index does not match expected\n");
	    rc = TSS_RC_MALFORMED_NV_PUBLIC;
	}
    }
    if (rc == 0) {
	printf("nvreadpublic: name algorithm %04x\n", out.nvPublic.t.nvPublic.nameAlg);
	printf("nvreadpublic: data size %u\n", out.nvPublic.t.nvPublic.dataSize);
	printf("nvreadpublic: attributes %08x\n", out.nvPublic.t.nvPublic.attributes.val);
	TSS_PrintAll("nvreadpublic: policy",
		     out.nvPublic.t.nvPublic.authPolicy.t.buffer,
		     out.nvPublic.t.nvPublic.authPolicy.t.size);
	TSS_PrintAll("nvreadpublic: name",
		     out.nvName.t.name, out.nvName.t.size);
	if (verbose) printf("nvreadpublic: success\n");
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("nvreadpublic: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    return rc;
}

static void printUsage(void)
{
    printf("\n");
    printf("nvreadpublic\n");
    printf("\n");
    printf("Runs TPM2_NV_ReadPublic\n");
    printf("\n");
    printf("\t-ha NV index handle\n");
    printf("\t[-nalg expected name hash algorithm [sha1, sha256, sha384] (default sha256)]\n");
    exit(1);	
}
