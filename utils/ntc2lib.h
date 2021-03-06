/********************************************************************************/
/*										*/
/*	     	TPM2 Novoton Proprietary Command Utilities			*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: ntc2lib.h 1015 2017-06-07 13:16:34Z kgoldman $		*/
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

#ifndef NTC2LIB_H
#define NTC2LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef TPM_TSS
#include "TpmTypes.h"
#include "Unmarshal_fp.h"
#else
#include <tss2/TPM_Types.h>
#include <tss2/Unmarshal_fp.h>
#endif

/* default values for System P I2C */

#define PREQUIRED_i2cLoc1_2    	0xff
#define PREQUIRED_i2cLoc3_4    	0xff
#define PREQUIRED_AltCfg	0x03
#define PREQUIRED_Direction   	0x00
#define PREQUIRED_PullUp    	0xff
#define PREQUIRED_PushPull    	0xff
#define PREQUIRED_CFG_A    	0xfe
#define PREQUIRED_CFG_B    	0xff
#define PREQUIRED_CFG_C    	0xff
#define PREQUIRED_CFG_D    	0xff
#define PREQUIRED_CFG_E    	0xff
#define PREQUIRED_CFG_F    	0xff
#define PREQUIRED_CFG_G    	0xff
#define PREQUIRED_CFG_H    	0xff
#define PREQUIRED_CFG_I    	0xff
#define PREQUIRED_CFG_J    	0xff
#define PREQUIRED_IsValid    	0xaa
#define PREQUIRED_IsLocked	0x00;

/* required values, others not supported */

#define FIXED_Direction   	0x00
#define FIXED_PullUp    	0xff
#define FIXED_PushPull    	0xff
#define FIXED_CFG_F    		0xff
#define FIXED_CFG_I    		0xff
#define FIXED_CFG_J    		0xff
#define FIXED_IsValid    	0xaa

typedef struct tdNTC2_CFG_STRUCT {
    uint8_t i2cLoc1_2;
    uint8_t i2cLoc3_4;
    uint8_t AltCfg;
    uint8_t Direction;
    uint8_t PullUp;
    uint8_t PushPull;
    uint8_t CFG_A;
    uint8_t CFG_B;
    uint8_t CFG_C;
    uint8_t CFG_D;
    uint8_t CFG_E;
    uint8_t CFG_F;
    uint8_t CFG_G;
    uint8_t CFG_H;
    uint8_t CFG_I;
    uint8_t CFG_J;
    uint8_t IsValid;	/* Must be AAh */
    uint8_t IsLocked;	/* Ignored on NTC2_PreConfig, NTC2_GetConfig returns AAh once configuration
			   is locked. */
} NTC2_CFG_STRUCT;

typedef struct {
    NTC2_CFG_STRUCT preConfig;
} NTC2_PreConfig_In;     

typedef struct {
    NTC2_CFG_STRUCT preConfig;
} NTC2_GetConfig_Out;     


#define RC_NTC2_PreConfig_preConfig (TPM_RC_P + TPM_RC_1)

#ifdef __cplusplus
extern "C" {
#endif

    TPM_RC
    NTC2_PreConfig_In_Unmarshal(NTC2_PreConfig_In *target, BYTE **buffer, INT32 *size, TPM_HANDLE handles[]);
    TPM_RC
    TSS_NTC2_PreConfig_In_Marshal(NTC2_PreConfig_In *source, UINT16 *written, BYTE **buffer, INT32 *size);

    TPM_RC
    NTC2_GetConfig_Out_Unmarshal(NTC2_GetConfig_Out *target, TPM_ST tag, BYTE **buffer, INT32 *size);
    UINT16
    NTC2_GetConfig_Out_Marshal(NTC2_GetConfig_Out *source, TPMI_ST_COMMAND_TAG tag, BYTE **buffer, INT32 *size);

    TPM_RC
    NTC2_CFG_STRUCT_Unmarshal(NTC2_CFG_STRUCT *target, BYTE **buffer, INT32 *size);
    TPM_RC
    TSS_NTC2_CFG_STRUCT_Marshal(NTC2_CFG_STRUCT *source, UINT16 *written, BYTE **buffer, INT32 *size);
    UINT16
    NTC2_CFG_STRUCT_Marshal(NTC2_CFG_STRUCT *source, BYTE **buffer, INT32 *size);

#ifdef __cplusplus
}
#endif

#endif
