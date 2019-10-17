#!/bin/bash -xe

#----------------------------------------------------------------------

OPENSSL_VERSION=1.1.1d
export IPHONEOS_DEPLOYMENT_TARGET="9.3"

#----------------------------------------------------------------------

OPENSSLDIR_IOS_ARM64=~/openssl/ios_arm64
OPENSSLDIR_IOS_X86_64=~/openssl/ios_x86_64
OPENSSLDIR_IOS=~/openssl/ios

#----------------------------------------------------------------------

build_ios_arch() {
  ARCH=$1 # arm64       | x86_64
  SDK=$2  # iphoneos    | iphonesimulator
  HOST=$3 # ios64-xcrun | darwin64-x64_64-cc

  # -----

  OPENSSLDIR_IOS_ARCH_WORKSPACE=/tmp/openssl/ios_${ARCH?}
  OPENSSLDIR_IOS_ARCH=~/openssl/ios_${ARCH?}
  SDKDIR=$(xcrun --sdk ${SDK?} --show-sdk-path)
  export CC=$(xcrun --find --sdk ${SDK?} clang)

  # -----

  OPENSSL_DOWNLOAD_URI=https://www.openssl.org/source/openssl-${OPENSSL_VERSION?}.tar.gz
  OPENSSL_DOWNLOAD=~/Downloads/openssl-${OPENSSL_VERSION?}.tar.gz
  if [ ! -f ${OPENSSL_DOWNLOAD?} ]; then
    curl ${OPENSSL_DOWNLOAD_URI?} -o ${OPENSSL_DOWNLOAD?}
  fi

  # -----

  if [ -d ${OPENSSLDIR_IOS_ARCH_WORKSPACE?} ]; then
    rm -rf ${OPENSSLDIR_IOS_ARCH_WORKSPACE}
  fi
  mkdir -p ${OPENSSLDIR_IOS_ARCH_WORKSPACE?}
  cd ${OPENSSLDIR_IOS_ARCH_WORKSPACE?}
  tar -xzf ${OPENSSL_DOWNLOAD?}

  # -----

  BUILD_CPUS=$(sysctl -n hw.logicalcpu_max)
  BUILD_LOG=${OPENSSLDIR_IOS_ARCH_WORKSPACE?}.log
  export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot ${SDKDIR} -miphoneos-version-min=${IPHONEOS_DEPLOYMENT_TARGET}"
  export LDFLAGS="-arch ${ARCH} -isysroot ${SDKDIR}"
  cd ${OPENSSLDIR_IOS_ARCH_WORKSPACE?}/openssl-${OPENSSL_VERSION?}
  ./configure --prefix=${OPENSSLDIR_IOS_ARCH?} \
    -no-shared -no-engine -no-async -no-hw ${HOST?} 2>&1 | tee "${BUILD_LOG?}"
  make -j ${BUILD_CPUS?} 2>&1 | tee -a "${BUILD_LOG?}"

  # -----

  if [ -d ${OPENSSLDIR_IOS_ARCH?} ]; then
    rm -rf ${OPENSSLDIR_IOS_ARCH?}
  fi
  mkdir -p ${OPENSSLDIR_IOS_ARCH?}
  cd ${OPENSSLDIR_IOS_ARCH_WORKSPACE?}/openssl-${OPENSSL_VERSION?}
  make install
}

#----------------------------------------------------------------------

build_ios() {
  if [ -d ${OPENSSLDIR_IOS?} ]; then
    rm -rf ${OPENSSLDIR_IOS?}
  fi
  mkdir -p ${OPENSSLDIR_IOS?}

  # -----

  tar -C ${OPENSSLDIR_IOS_ARM64?} -cf - include | tar -C ${OPENSSLDIR_IOS?} -xvf -

  # -----

  mkdir -p ${OPENSSLDIR_IOS?}/lib
  lipo                 ${OPENSSLDIR_IOS_ARM64?}/lib/libssl.a \
       -arch x86_64    ${OPENSSLDIR_IOS_X86_64?}/lib/libssl.a \
       -create -output ${OPENSSLDIR_IOS?}/lib/libssl.a
  lipo                 ${OPENSSLDIR_IOS_ARM64?}/lib/libcrypto.a \
       -arch x86_64    ${OPENSSLDIR_IOS_X86_64?}/lib/libcrypto.a \
       -create -output ${OPENSSLDIR_IOS?}/lib/libcrypto.a
}

#----------------------------------------------------------------------

# build_ios_arch arm64  iphoneos        ios64-xcrun
# build_ios_arch x86_64 iphonesimulator darwin64-x86_64-cc
build_ios

#----------------------------------------------------------------------
