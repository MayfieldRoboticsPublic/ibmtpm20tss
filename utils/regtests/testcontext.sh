#!/bin/bash
#

#################################################################################
#										#
#			TPM2 regression test					#
#			     Written by Ken Goldman				#
#		       IBM Thomas J. Watson Research Center			#
#		$Id: testcontext.sh 663 2016-06-30 18:58:18Z kgoldman $		#
#										#
# (c) Copyright IBM Corporation 2015						#
# 										#
# All rights reserved.								#
# 										#
# Redistribution and use in source and binary forms, with or without		#
# modification, are permitted provided that the following conditions are	#
# met:										#
# 										#
# Redistributions of source code must retain the above copyright notice,	#
# this list of conditions and the following disclaimer.				#
# 										#
# Redistributions in binary form must reproduce the above copyright		#
# notice, this list of conditions and the following disclaimer in the		#
# documentation and/or other materials provided with the distribution.		#
# 										#
# Neither the names of the IBM Corporation nor the names of its			#
# contributors may be used to endorse or promote products derived from		#
# this software without specific prior written permission.			#
# 										#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS		#
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		#
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR		#
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT		#
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	#
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		#
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,		#
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY		#
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		#
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE		#
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.		#
#										#
#################################################################################

echo ""
echo "Context"
echo ""

echo "Start an HMAC auth session"
${PREFIX}startauthsession -se h > run.out
checkSuccess $?

echo "Load the signing key under the primary key"
${PREFIX}load -hp 80000000 -ipr signpriv.bin -ipu signpub.bin -pwdp pps -se0 02000000 1 > run.out
checkSuccess $?

echo "Sign a digest"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkSuccess $?

echo "Verify the signature"
${PREFIX}verifysignature -hk 80000001 -halg sha256 -if msg.bin -is sig.bin > run.out
checkSuccess $?

echo "Save context for the key"
${PREFIX}contextsave -ha 80000001 -of tmp.bin > run.out
checkSuccess $?

echo "Sign to verify that the original key is not flushed"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkSuccess $?

echo "Flush the original key"
${PREFIX}flushcontext -ha 80000001 > run.out
checkSuccess $?

echo "Sign with original key  - should fail"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkFailure $?

echo "Load context"
${PREFIX}contextload -if tmp.bin > run.out
checkSuccess $?

echo "Sign with the loaded context"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkSuccess $?

echo "Save context for the session"
${PREFIX}contextsave -ha 02000000 -of tmp.bin > run.out
checkSuccess $?

echo "Sign with the saved session context - should fail"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkFailure $?

echo "Load context for the session"
${PREFIX}contextload -if tmp.bin > run.out
checkSuccess $?

echo "Sign with the saved session context"
${PREFIX}sign -hk 80000001 -halg sha256 -if msg.bin -os sig.bin -pwdk sig -se0 02000000 1 > run.out
checkSuccess $?

echo "Flush the loaded context"
${PREFIX}flushcontext -ha 80000001 > run.out
checkSuccess $?

echo "Flush the session"
${PREFIX}flushcontext -ha 02000000 > run.out
checkSuccess $?

# ${PREFIX}getcapability  -cap 1 -pr 80000000
# ${PREFIX}getcapability  -cap 1 -pr 02000000
