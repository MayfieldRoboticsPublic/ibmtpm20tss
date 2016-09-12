@echo off

REM #################################################################################
REM #										    #
REM #			TPM2 regression test					    #
REM #			     Written by Ken Goldman				    #
REM #		       IBM Thomas J. Watson Research Center			    #
REM #		$Id$		#
REM #										    #
REM # (c) Copyright IBM Corporation 2016					    #
REM # 										    #
REM # All rights reserved.							    #
REM # 										    #
REM # Redistribution and use in source and binary forms, with or without	    #
REM # modification, are permitted provided that the following conditions are	    #
REM # met:									    #
REM # 									    	    #
REM # Redistributions of source code must retain the above copyright notice,	    #
REM # this list of conditions and the following disclaimer.			    #
REM # 										    #
REM # Redistributions in binary form must reproduce the above copyright		    #
REM # notice, this list of conditions and the following disclaimer in the	    #
REM # documentation and/or other materials provided with the distribution.	    #
REM # 										    #
REM # Neither the names of the IBM Corporation nor the names of its		    #
REM # contributors may be used to endorse or promote products derived from	    #
REM # this software without specific prior written permission.			    #
REM # 										    #
REM # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	    #
REM # "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		    #
REM # LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR	    #
REM # A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT	    #
REM # HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	    #
REM # SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		    #
REM # LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,	    #
REM # DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY	    #
REM # THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT	    #
REM # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE	    #
REM # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.	    #
REM #										    #
REM #################################################################################

setlocal enableDelayedExpansion

REM # PIN Pass index name is
REM 
REM # 00 0b da 1c bd 54 bb 81 54 6c 1c 76 30 dd d4 09 
REM # 50 3a 0d 6d 03 05 16 1b 15 88 d6 6b c8 fa 17 da 
REM # ad 81 
REM 
REM # Policy Secret using PIN Pass index is
REM 
REM # 56 e4 c7 26 d7 d7 dd 3c bd 4c ae 11 c0 1b 2e 83 
REM # 3c 37 33 3c fb c3 b9 c3 5f 05 ab 53 23 0c df 7d 
REM 
REM # PIN Fail index name is
REM 
REM # 00 0b 86 11 40 4a e8 0c 0a 84 e5 b8 97 05 98 f0 
REM # b5 60 2d 14 21 19 bf 44 9d e5 f9 61 84 bc 4c 01 
REM # c4 be 
REM 
REM # Policy Secret using PIN Fail index is
REM 
REM # 9d 56 8f da 52 27 30 dc be a8 ad 59 bc a5 0c 1c 
REM # 16 02 95 03 a0 0b d3 d8 20 a8 b2 d8 5b c5 12 df 
REM 
REM 
REM # 01000000 is PIN pass or PIN fail index
REM # 01000001 is ordinary index with PIN pass policy
REM # 01000002 is ordinary index with PIN fail policy


echo ""
echo "NV PIN Index"
echo ""

echo "NV Define Space, ordinary index, with policysecret for pin pass index"
%TPM_EXE_PATH%nvdefinespace -ha 01000001 -hi o -pwdn ppi -ty o -hia p -sz 1 -pol policies/policysecretnvpp.bin > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000001 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write to set written bit"
%TPM_EXE_PATH%nvwrite -ha 01000001 -hia p -ic 0 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000001 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Define Space, ordinary index, with policysecret for pin pass fail"
%TPM_EXE_PATH%nvdefinespace -ha 01000002 -hi o -pwdn pfi -ty o -hia p -sz 1 -pol policies/policysecretnvpf.bin  > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000002 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write to set written bit"
%TPM_EXE_PATH%nvwrite -ha 01000002 -hia p -ic 0 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000002 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Start a policy session"
%TPM_EXE_PATH%startauthsession -se p > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Pass Index"
echo ""

echo "phEnableNV enable"
hierarchycontrol -hi p -he n > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Define Space"
%TPM_EXE_PATH%nvdefinespace -ha 01000000 -hi p -pwdn nnn -ty p +at wst -at rst -hia p -pol policies/policysecretp.bin > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 use"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read does not affect count"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret platform with PWAP session"
%TPM_EXE_PATH%policysecret -ha 4000000c -hs 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy write, 1 use"
%TPM_EXE_PATH%nvwrite -ha 01000000 -id 0 1 -se0 03000000 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret platform with PWAP session"
%TPM_EXE_PATH%policysecret -ha 4000000c -hs 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy read"
%TPM_EXE_PATH%nvread -ha 01000000 -se0 03000000 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 use"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Index read"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nnn -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Index read, no uses - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nnn -sz 8 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform read, no uses"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Pass Index in Policy Secret"
echo ""

echo "Policy Secret with PWAP session, bad password - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnnx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, 1 use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, bad password does not consume pinCount - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnnx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, pinCount used - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Get Digest, 50 b9 63 d6 ..."
%TPM_EXE_PATH%policygetdigest -ha 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Read ordinary index using PIN pass policy secret"
%TPM_EXE_PATH%nvread -ha 01000001 -sz 1 -se0 03000000 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 use, 1 / 2"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 1 2 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 0 uses, 0 / 0"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 0 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, pinCount used - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, 1 use. 1 / 1, already used"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 1 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, pinCount used - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, 0 uses. 2 / 1, already used"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 2 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, pinCount used - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo ""
echo "NV PIN Pass Index with Write Lock"
echo ""

echo "Platform write, 1 use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Write lock"
%TPM_EXE_PATH%nvwritelock -ha 01000000 -hia p > run.out 
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, pinCount used - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, locked - should fail"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Reboot"
%TPM_EXE_PATH%powerup > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Startup"
%TPM_EXE_PATH%startup > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Start a policy session"
%TPM_EXE_PATH%startauthsession -se p > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Pass Index with Read Lock"
echo ""

echo "Platform write, 1 use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Read lock"
%TPM_EXE_PATH%nvreadlock -ha 01000000 -hia p  > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read, locked - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, read locked"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Pass Index with phEnableNV clear"
echo ""

echo "Platform write, 1 use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Clear phEnableNV"
%TPM_EXE_PATH%hierarchycontrol -hi p -he n -state 0 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, phEnableNV disabled - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Set phEnableNV"
%TPM_EXE_PATH%hierarchycontrol -hi p -he n -state 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "Cleanup"
echo ""

echo "NV Undefine Space"
%TPM_EXE_PATH%nvundefinespace -hi p -ha 01000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Flush the session"
%TPM_EXE_PATH%flushcontext -ha 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Fail Index"
echo ""

echo "NV Define Space"
%TPM_EXE_PATH%nvdefinespace -ha 01000000 -hi p -pwdn nnn -ty f +at wst -at rst -hia p -pol policies/policysecretp.bin > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Read Public"
%TPM_EXE_PATH%nvreadpublic -ha 01000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 failure"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read with bad password - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 -pwdn xxx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Start a policy session"
%TPM_EXE_PATH%startauthsession -se p > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret platform with PWAP session"
%TPM_EXE_PATH%policysecret -ha 4000000c -hs 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy write, 1 failure"
%TPM_EXE_PATH%nvwrite -ha 01000000 -id 0 1 -se0 03000000 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret platform with PWAP session"
%TPM_EXE_PATH%policysecret -ha 4000000c -hs 03000000 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy read"
%TPM_EXE_PATH%nvread -ha 01000000 -sz 8 -se0 03000000 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 failure"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Index read, correct password"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nnn -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Index read, bad password - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nn -sz 8  > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Index read, correct password - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nnn -sz 8 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, 1 failure"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Index read"
%TPM_EXE_PATH%nvread -ha 01000000 -pwdn nnn -sz 8 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Fail Index in Policy Secret"
echo ""

echo "Platform write, 2 failures, 0 / 2"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 2 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, good password"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, bad password uses pinCount - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnnx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, good password, resets pinCount"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, bad password uses pinCount - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnnx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, bad password uses pinCount - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnnx > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, good password - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Platform write, 1 failure use, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, good password, resets pinCount"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 0 failures , 1 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 1 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, good password, resets pinCount"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo ""
echo "NV PIN Fail Index with Write Lock"
echo ""

echo "Platform write, 1 fail , 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Write lock"
%TPM_EXE_PATH%nvwritelock -ha 01000000 -hia p > run.out 
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, locked - should fail"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Reboot"
%TPM_EXE_PATH%powerup > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Startup"
%TPM_EXE_PATH%startup > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Start a policy session"
%TPM_EXE_PATH%startauthsession -se p > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform write, 1 failure, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Fail Index with Read Lock"
echo ""

echo "Platform write, 1 failure, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Read lock"
%TPM_EXE_PATH%nvreadlock -ha 01000000 -hia p > run.out 
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Platform read, locked - should fail"
%TPM_EXE_PATH%nvread -ha 01000000 -hia p -sz 8 > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, read locked"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "NV PIN Fail Index with phEnableNV clear"
echo ""

echo "Platform write, 1 failure, 0 / 1"
%TPM_EXE_PATH%nvwrite -ha 01000000 -hia p -id 0 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Clear phEnableNV"
%TPM_EXE_PATH%hierarchycontrol -hi p -he n -state 0 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Policy Secret with PWAP session, phEnableNV disabled - should fail"
%TPM_EXE_PATH%policysecret -ha 01000000 -hs 03000000 -pwde nnn > run.out
IF !ERRORLEVEL! EQU 0 (
    exit /B 1
)

echo "Set phEnableNV"
%TPM_EXE_PATH%hierarchycontrol -hi p -he n -state 1 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo ""
echo "Cleanup"
echo ""

echo "NV Undefine Space"
%TPM_EXE_PATH%nvundefinespace -hi p -ha 01000000 > run.out 
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Undefine Space"
%TPM_EXE_PATH%nvundefinespace -hi o -ha 01000001 > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "NV Undefine Space"
%TPM_EXE_PATH%nvundefinespace -hi o -ha 01000002  > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

echo "Flush the session"
%TPM_EXE_PATH%flushcontext -ha 03000000 > run.out > run.out
IF !ERRORLEVEL! NEQ 0 (
    exit /B 1
)

exit /B 0










REM # pinfail
REM 
REM # create 
REM # least one of TPMA_NV_PPWRITE, TPMA_NV_OWNERWRITE, or TPMA_NV_POLICYWRITE
REM # TPMA_NV_AUTHWRITE shall be clear
REM # must have noda
REM # test plat write, owner write, policy write, auth write
REM # write 
REM # pin count pinlimit 4 bytes each,big endian
REM 
REM # try reset to zero
REM # try reset to nonzero
REM # try reset to limit
REM # reset to > limit
REM 
REM # auth fail , policysecret
REM # pincount incremented
REM # fail if written clear
REM 
REM # auth success, policysecret
REM # pincount set to zero
REM # fail if pincount not < pinlimit
REM 
REM # write lock blocks write
REM # write locked, can still use in policy
REM # phenablenv causes fail
REM # read lock causes fail, no increment
REM 
REM # policy platform policy ownner read does not affect count
REM # authread affects count
REM 
REM 
REM # function lock
REM 
REM # function unlock
REM # shutdown clear startup clear
REM 
REM 
REM # getcapability  -cap 1 -pr 80000000
REM # getcapability  -cap 1 -pr 02000000
REM # getcapability  -cap 1 -pr 01000000
