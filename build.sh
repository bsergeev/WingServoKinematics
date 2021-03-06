#!/bin/bash

# This Bash script works in Linux and Windows (Git Bash).

APP_NAME=servo
if [ "$1" != "" ]; then
  APP_NAME="$1"
fi
echo Project name: ${APP_NAME}

BUILD_CONFIGURATION=Debug
if [ "$2" = "Release" ] || [ "$2" = "Debug" ]; then
  BUILD_CONFIGURATION="$2"
fi

# Detect the OS
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)   OS_NAME=Linux;;
    Darwin*)  OS_NAME=Mac;;
    CYGWIN*)  OS_NAME=Windows;;
    MINGW*)   OS_NAME=Windows;;
    *)        OS_NAME="UNKNOWN:${unameOut}"
esac
echo Building in ${OS_NAME}

BUILD_DIRECTORY=build.$OS_NAME
BINARY_LOCATION=$BUILD_DIRECTORY
if [ "$OS_NAME" = "Windows" ]; then
  BINARY_LOCATION="$BUILD_DIRECTORY/$BUILD_CONFIGURATION"
fi

if [ ! -d "$BUILD_DIRECTORY" ]; then
  mkdir $BUILD_DIRECTORY
fi
cd $BUILD_DIRECTORY

if [ "$OS_NAME" = "Linux" ]; then
  cmake -G "Ninja" -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION .. && ninja
elif [ "$OS_NAME" = "Windows" ]; then
  cmake -A x64 -DQt5_DIR=c:/Frameworks/Qt/5.14.1/msvc2017_64/lib/cmake/Qt5 -DAPP_NAME:STRING="$APP_NAME" ..
  cmake --build . --config $BUILD_CONFIGURATION
fi

cd ..
$BINARY_LOCATION/$APP_NAME
