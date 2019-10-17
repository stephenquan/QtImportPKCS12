#!/bin/bash -xe

#----------------------------------------------------------------------

OPENSSL_VERSION=1.1.1d

#----------------------------------------------------------------------

OPENSSL_MACOS_WORKSPACE=/tmp/openssl/macos
OPENSSL_MACOS=~/openssl/macos

#----------------------------------------------------------------------

OPENSSL_DOWNLOAD_URI=https://www.openssl.org/source/openssl-${OPENSSL_VERSION?}.tar.gz
OPENSSL_DOWNLOAD=~/Downloads/openssl-${OPENSSL_VERSION?}.tar.gz
if [ ! -f ${OPENSSL_DOWNLOAD?} ]; then
  curl ${OPENSSL_DOWNLOAD_URI?} -o ${OPENSSL_DOWNLOAD?}
fi

#----------------------------------------------------------------------

if [ -d ${OPENSSL_MACOS_WORKSPACE?} ]; then
  rm -rf ${OPENSSL_MACOS_WORKSPACE?}
fi
mkdir -p ${OPENSSL_MACOS_WORKSPACE?}
cd ${OPENSSL_MACOS_WORKSPACE?}
tar -xzf ${OPENSSL_DOWNLOAD?}

#----------------------------------------------------------------------

BUILD_CPUS=$(sysctl -n hw.logicalcpu_max)
BUILD_LOG=${OPENSSL_MACOS_WORKSPACE?}.log
cd ${OPENSSL_MACOS_WORKSPACE?}/openssl-${OPENSSL_VERSION?}
./configure --prefix=${OPENSSL_MACOS?} \
    darwin64-x86_64-cc 2>&1 | tee "${BUILD_LOG?}"
make -j ${BUILD_CPUS?} 2>&1 | tee -a "${BUILD_LOG?}"

#----------------------------------------------------------------------

if [ -d ${OPENSSL_MACOS?} ]; then
  rm -rf ${OPENSSL_MACOS?}
fi
mkdir -p ${OPENSSL_MACOS?}
cd ${OPENSSL_MACOS_WORKSPACE?}/openssl-${OPENSSL_VERSION?}
make install

#----------------------------------------------------------------------
