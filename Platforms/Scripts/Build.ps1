# Usage: ./Build.ps1 <CONFIG> <CONFIG_ARGS:string> <BUILD_ARGS:string>
#
# 	CONFIG is generally one of 'Debug', 'Release', 'MinSizeRel', or 'RelWithDebInfo'
#	Defaults to 'Release'
#
#	CONFIG_ARGS is passed to CMake during the configuration step
#
#	BUILD_ARGS is passed to CMake during the build step
#

$SRC_DIR 	= "../.."
$BUILD_DIR 	= "$SRC_DIR/Build"

$CONFIG 	 = $args[0]
$CONFIG_ARGS = $args[1]
$BUILD_ARGS  = $args[2]

# Check config
if ([string]::IsNullOrEmpty($CONFIG))  { $CONFIG = "Release" }

# Output build information
Write-Output "Building with following options:"
Write-Output "	Config:      $CONFIG"
Write-Output "	Config Args: $CONFIG_ARGS"
Write-Output "	Build Args:  $BUILD_ARGS"
Write-Output "	Source:      $SRC_DIR"
Write-Output "	Output:      $BUILD_DIR"
Write-Output ""

# Configure CMake
Start-Process cmake -ArgumentList "-S $SRC_DIR -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$CONFIG $CONFIG_ARGS" -NoNewWindow -PassThru

Write-Output "\n\n"

# Build project
Start-Process cmake -ArgumentList "--build $BUILD_DIR --config $CONFIG $BUILD_ARGS" -NoNewWindow -PassThru