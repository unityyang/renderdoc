#!/bin/bash

export ANDROID_SDK_HOME=/C/Users/unityyang/AppData/Local/Android/Sdk
export ANDROID_NDK_HOME=/%ANDROID_SDK_HOME%/ndk/29.0.13113456
# export JAVA_HOME=/d/jdk-1.8
# export JAVA_HOME=/c/jdk-8.0.302.8-hotspot
export JAVA_HOME=D/TencentKona-17.0.14.b1

export APK_TARGET_ID=android-29
export ANDROID_SDK_ROOT_PATH=%ANDROID_SDK_HOME%
export ANDROID_NDK_ROOT_PATH=%ANDROID_NDK_HOME%

mkdir -p build-android-v8a
cd build-android-v8a
cmake -DBUILD_ANDROID=On -DANDROID_ABI=arm64-v8a \
	-G "MinGW Makefiles" .. C/Users/unityyang/AppData/Local/Android/Sdk/ndk/29.0.13113456/prebuilt/windows-x86_64/bin
# /d/Sdk/ndk/android-ndk-r21e/prebuilt/windows-x86_64/bin/make.exe VERBOSE=1