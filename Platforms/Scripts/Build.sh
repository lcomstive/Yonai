#!/bin/sh
#
# Usage: ./Build.sh <CONFIG> <CONFIG_ARGS:string> <BUILD_ARGS:string>
#
# 	CONFIG is generally one of 'Debug', 'Release', 'MinSizeRel', or 'RelWithDebInfo'
#	Defaults to 'Release'
#
#	CONFIG_ARGS is passed to CMake during the configuration step
#
#	BUILD_ARGS is passed to CMake during the build step
#

$CONFIG 	 = $1
$CONFIG_ARGS = $2
$BUILD_ARGS  = $3
$SRC_DIR 	 = "../.."
$BUILD_DIR 	 = "$SRC_DIR/Build"

# Check config
if [ -z "$CONFIG"  ] then $CONFIG = "Release" fi

# Output build information
echo "Building with following options:"
echo "	Config:      $CONFIG"
echo "	Config Args: $CONFIG_ARGS"
echo "	Build Args:  $BUILD_ARGS"
echo "	Source:      $SRC_DIR"
echo "	Output:      $BUILD_DIR"
echo ""

cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$CONFIG $CONFIG_ARGS

cmake --build $BUILD_DIR --config $CONFIG $BUILD_ARGS