#!/bin/bash

# Flags:
#	c: Build type [Debug, Release]
#	g: Generator (Unix Makefiles, Ninja, etc.)
#	t: Run tests after building
#	p: Package output

CONFIG=Release
TESTING=false
PACKAGE=false
GENERATOR="Unix Makefiles"
BUILD_FLAGS=""

# Parse input args
while getopts c:g:b:tp flag
do
	case "${flag}" in
		c) 	CONFIG=${OPTARG};;
		g)	GENERATOR=${OPTARG};;
		t)	TESTING=true;;
		p)	PACKAGE=true;;
		b)  BUILD_FLAGS="${BUILD_FLAGS} ${OPTARG}";;
		*) ;;
	esac
done

echo "Building Yonai in ${CONFIG} mode with ${GENERATOR}"

# Get script directory
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
# Get root source directory
ROOT=${SCRIPT_DIR}/../..

# Configure CMake
cmake -B ${ROOT}/build/ -G ${GENERATOR} -DCMAKE_BUILD_TYPE=${CONFIG} ${ROOT} ${BUILD_FLAGS}

# Build YonaiScriptCore C# project
msbuild ${ROOT}/Apps/YonaiScriptCore/YonaiScriptCore.csproj -verbosity:minimal -property:Configuration=${CONFIG}

# Build YonaiScriptEditor C# project
msbuild ${ROOT}/Apps/YonaiScriptEditor/YonaiScriptEditor.csproj -verbosity:minimal -property:Configuration=${CONFIG}

# Build Yonai and apps
cmake --build ${ROOT}/build --config ${CONFIG}

# Check for build failure
if [ "$?" -ne "0" ]; then
	echo "Early exit - build failure"
	exit $?
fi

# Run tests
if [ "$TESTING" = true ]; then
	${ROOT}/build/bin/YonaiTest

	# Check for test failure
	if [ "$?" -ne "0" ]; then
		echo "Early exit - test failure"
		exit $?
	fi
fi

# Package output
if [ "$PACKAGE" = true ]; then
	# Change working directory to build directory
	pushd ${ROOT}/build
	
	# Package output
	cpack
	
	# Return to previous working directory
	popd
fi
