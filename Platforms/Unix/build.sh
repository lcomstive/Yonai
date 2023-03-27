#!/bin/bash

# Flags:
#	c: Build type [Debug, Release]
#	t: Run tests after building
#	p: Package output

CONFIG=Release
TESTING=false
PACKAGE=false

# Parse input args
while getopts c:tp flag
do
	echo "Flag: ${flag}"
	case "${flag}" in
		c) 	CONFIG=${OPTARG};;
		t)	TESTING=true;;
		p)	PACKAGE=true;;
		*) ;;
	esac
done

echo "Building AquaEngine in ${CONFIG} mode"

# Get script directory
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
# Get root source directory
ROOT=${SCRIPT_DIR}/../..

# Configure CMake
cmake -B ${ROOT}/build/ -DCMAKE_BUILD_TYPE=${CONFIG} ${ROOT}

# Build AquaScriptCore C# project manually
msbuild ${ROOT}/Apps/AquaScriptCore/AquaScriptCore.csproj -verbosity:minimal -property:Configuration=${CONFIG}

# Build AquaScriptEditor C# project manually
msbuild ${ROOT}/Apps/AquaScriptEditor/AquaScriptEditor.csproj -verbosity:minimal -property:Configuration=${CONFIG}

# Build AquaEngine and apps
cmake --build ${ROOT}/build --config ${CONFIG}

# Check for build failure
if [ "$?" -ne "0" ]; then
	echo "Early exit - build failure"
	exit $?
fi

# Run tests
if [ "$TESTING" = true ]; then
	${ROOT}/build/bin/AquaEngineTest

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
