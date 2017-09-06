#!/bin/sh

set -eu

VERSION=${1:-"1.0.0-0mayfield0"} # Upstream version: 1045
SCRIPTDIR=$(dirname "$(readlink -e ${0})")
REPODIR=${SCRIPTDIR}/..
INSTALLDIR=$(mktemp -dt tpm2_installdir.XXXXXX)

cd ${REPODIR}/utils
make
DESTDIR=${INSTALLDIR} make install
make clean
cd ${REPODIR}

fpm \
  --input-type dir \
  --chdir ${INSTALLDIR} \
  --output-type deb \
  --name "ibm-tpm2-tools" \
  --architecture native \
  --version "${VERSION}" \
  --license "BSD" \
  --vendor "IBM" \
  --description "IBM's TPM 2.0 TSS packaged by Mayfield Robotics." \
  --maintainer "Spyros Maniatopoulos <spyros@mayfieldrobotics.com>" \
  --url "https://github.com/MayfieldRoboticsPublic/ibmtpm20tss" \
  .

rm -rf ${INSTALLDIR}
