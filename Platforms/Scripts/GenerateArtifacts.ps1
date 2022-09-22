# Usage ./GenerateArtifacts.ps1 <CONFIG>
#
# 	CONFIG is generally one of 'Debug', 'Release', 'MinSizeRel', or 'RelWithDebInfo'
#	Defaults to 'Release'
#

# Get Git information
. .\GetGitInfo.ps1

$CONFIG = $args[0]

# Check config
if ([string]::IsNullOrEmpty($CONFIG))  { $CONFIG = "Release" }

Set-Location ../..

$ARTIFACT="AquaEngine-$ProjectVersion.zip"
$OUTPUT_DIR="Build/bin/$CONFIG"

# Add .exe files
Compress-Archive -Path $OUTPUT_DIR/*.exe -Force -DestinationPath $ARTIFACT

# Add .dll files
Compress-Archive -Path $OUTPUT_DIR/*.dll -Update -DestinationPath $ARTIFACT

# Add Assets folder
Compress-Archive -Path Apps/Assets/ -Update -DestinationPath $ARTIFACT

# Restore location
Set-Location Platforms/Scripts