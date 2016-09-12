#!/bin/bash
#

#################################################################################
#										#
#			TPM2 regression test					#
#			     Written by Ken Goldman				#
#		       IBM Thomas J. Watson Research Center			#
#		$Id: testsign.sh 687 2016-07-20 17:07:38Z kgoldman $		#
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
echo "Signing key"
echo ""

# loop over unrestricted hash algorithms

echo "Load the signing key under the primary key"
${PREFIX}load -hp 80000000 -ipr signpriv.bin -ipu signpub.bin -pwdp pps > run.out
checkSuccess $?

echo "Create a key pair in PEM format using openssl"
  
openssl genrsa -out tmpkeypair.pem -aes256 -passout pass:rrrr 2048 > run.out

echo "Convert key pair to plaintext DER format"

openssl rsa -inform pem -outform der -in tmpkeypair.pem -out tmpkeypair.der -passin pass:rrrr > run.out

for HALG in sha1 sha256
do

    echo "Sign a digest - $HALG"
    ${PREFIX}sign -hk 80000001 -halg $HALG -if policies/aaa -os sig.bin -pwdk sig -ipu signpub.bin -ipem signpub.pem > run.out
    checkSuccess $?

    echo "Verify the signature - $HALG"
    ${PREFIX}verifysignature -hk 80000001 -halg $HALG -if policies/aaa -is sig.bin > run.out
    checkSuccess $?

    echo "Load the openssl key pair in the NULL hierarchy - $HALG"
    ${PREFIX}loadexternal -halg $HALG -ider tmpkeypair.der > run.out
    checkSuccess $?

    echo "Use the TPM as a crypto coprocessor to sign - $HALG" 
    ${PREFIX}sign -hk 80000002 -halg $HALG -if policies/aaa -os sig.bin > run.out
    checkSuccess $?

    echo "Verify the signature - $HALG"
    ${PREFIX}verifysignature -hk 80000002 -halg $HALG -if policies/aaa -is sig.bin > run.out
    checkSuccess $?

    echo "Flush the openssl signing key"
    ${PREFIX}flushcontext -ha 80000002 > run.out
    checkSuccess $?

done

echo "Flush the signing key"
${PREFIX}flushcontext -ha 80000001 > run.out
checkSuccess $?

echo ""
echo "External Verification Key"
echo ""

# create rsaprivkey.pem
# > openssl genrsa -out rsaprivkey.pem -aes256 -passout pass:rrrr 2048
# extract the public key
# > openssl pkey -inform pem -outform pem -in rsaprivkey.pem -passin pass:rrrr -pubout -out rsapubkey.pem 
# sign a test message msg.bin
# > openssl dgst -sha1 -sign rsaprivkey.pem -passin pass:rrrr -out pssig.bin msg.bin

echo "Load external just the public part of PEM RSA"
${PREFIX}loadexternal -halg sha1 -nalg sha1 -ipem policies/rsapubkey.pem > run.out
checkSuccess $?

echo "Sign a test message with openssl RSA"
openssl dgst -sha1 -sign policies/rsaprivkey.pem -passin pass:rrrr -out pssig.bin msg.bin

echo "Verify the RSA signature"
${PREFIX}verifysignature -hk 80000001 -halg sha1 -if msg.bin -is pssig.bin -raw > run.out
checkSuccess $?

echo "Flush the signing key"
${PREFIX}flushcontext -ha 80000001 > run.out
checkSuccess $?

# generate the p256 key
# > openssl ecparam -name prime256v1 -genkey -noout -out p256privkey.pem
# extract public key
# > openssl pkey -inform pem -outform pem -in p256privkey.pem -pubout -out p256pubkey.pem

echo "Load external just the public part of PEM ECC"
${PREFIX}loadexternal -halg sha1 -nalg sha1 -ipem policies/p256pubkey.pem -ecc > run.out
checkSuccess $?

echo "Sign a test message with openssl ECC"
openssl dgst -sha1 -sign policies/p256privkey.pem -out pssig.bin msg.bin

echo "Verify the ECC signature"
${PREFIX}verifysignature -hk 80000001 -halg sha1 -if msg.bin -is pssig.bin -raw -ecc > run.out
checkSuccess $?

echo "Flush the signing key"
${PREFIX}flushcontext -ha 80000001 > run.out
checkSuccess $?

rm -f tmpkeypair.pem
rm -f tmpkeypair.der
rm -f signpub.pem
rm -r pssig.bin

# ${PREFIX}getcapability  -cap 1 -pr 80000000
# ${PREFIX}getcapability  -cap 1 -pr 02000000
