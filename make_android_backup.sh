#!/bin/bash

export ANDROID_SDK=/d/Sdk
export ANDROID_NDK=/d/Sdk/ndk/android-ndk-r21e
export JAVA_HOME=/d/jre

export APK_TARGET_ID=android-29
export ANDROID_SDK_ROOT_PATH=%ANDROID_SDK%
export ANDROID_NDK_ROOT_PATH=%ANDROID_NDK%

mkdir -p build-android-v8a
cd build-android-v8a
cmake -DBUILD_ANDROID=On -DANDROID_ABI=arm64-v8a -DCMAKE_BUILD_TYPE=Release -DSTRIP_ANDROID_LIBRARY=On \
	-DUSE_INTERCEPTOR_LIB=On -DLLVM_DIR=/E/llvm_4.0_src/llvm/build_native/install_arm64/lib/cmake/llvm \
	-DBUILD_VERSION_HASH=cc05b288b6d1660ab04c6cf01173f1bb62e6f5dd \
	-G "MinGW Makefiles" ..
/C/msys64/usr/bin/make.exe VERBOSE=1
