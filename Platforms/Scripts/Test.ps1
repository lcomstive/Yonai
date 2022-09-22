# Usage ./Run_Tests.ps1 <CONFIG>
#
# 	CONFIG is generally one of 'Debug', 'Release', 'MinSizeRel', or 'RelWithDebInfo'
#	Defaults to 'Release'
#

$CONFIG = $args[0]

# Check config
if ([string]::IsNullOrEmpty($CONFIG))  { $CONFIG = "Release" }

$OUTPUT_DIR="../../Build/bin/$CONFIG"

Start-Process "$OUTPUT_DIR/AquaEngineTest" -ArgumentList "--gtest_output=xml:$OUTPUT_DIR/test_results.xml" -Wait -NoNewWindow -PassThru