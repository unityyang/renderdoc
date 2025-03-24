#!/bin/bash

export ANDROID_SDK=~/AppData/Local/Android/Sdk
export ANDROID_NDK=~/AppData/Local/Android/Sdk/ndk/25.1.8937393
export JAVA_HOME=/d/TencentKona-17.0.14.b1

export APK_TARGET_ID=android-26
export ANDROID_SDK_ROOT_PATH=%ANDROID_SDK%
export ANDROID_NDK_ROOT_PATH=%ANDROID_NDK%
export GIT_HASH=8a394019fdb62ad82be87bfd338a5ec43f4a5d95

export PATH=/C/Program\ Files/CMake/bin:~/AppData/Local/Android/Sdk/cmake/3.22.1/bin:/F/jdk-1.8/bin:/C/Strawberry/c/bin:/D/Git/mingw64/bin:/D/Git/usr/bin:/D/Git/bin:/C/windows/system32:/C/windows:/C/windows/System32/Wbem:/C/windows/System32/WindowsPowerShell/v1.0:/C/windows/System32/OpenSSH

mkdir -p build-android-v8a
cd build-android-v8a
cmake -DBUILD_ANDROID=On -DANDROID_ABI=arm64-v8a \
	-DBUILD_VERSION_HASH=$GIT_HASH \
	-G "MinGW Makefiles" ..
~/AppData/Local/Android/Sdk/ndk/25.1.8937393/prebuilt/windows-x86_64/bin/make.exe VERBOSE=1

# cd ..
# mkdir -p build-android-v7a
# cd build-android-v7a
# cmake -DBUILD_ANDROID=On -DANDROID_ABI=armeabi-v7a \
	# -DUSE_INTERCEPTOR_LIB=On -DLLVM_DIR=/f/llvm_40_src/llvm/build_native/install_arm32/lib/cmake/llvm \
	# -DBUILD_VERSION_HASH=$GIT_HASH \
	# -G "MinGW Makefiles" ..
# /d/android-ndk-r23b/prebuilt/windows-x86_64/bin/make.exe VERBOSE=1
