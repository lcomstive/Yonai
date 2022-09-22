# Enable folders in solution
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

## Aqua Engine ##
# Engine

# Apps
set_target_properties(BaseGame 	 PROPERTIES FOLDER "Apps")
set_target_properties(AquaEditor PROPERTIES FOLDER "Apps")

# Tests
set_target_properties(AquaEngineTest PROPERTIES FOLDER "Tests")

## Dependencies ##
# Glad
set_target_properties(glad PROPERTIES FOLDER "Dependencies")

# Google Test
if(AQUA_ENGINE_BUILD_TESTS)
	set_target_properties(gtest 	 PROPERTIES FOLDER "Dependencies")
	set_target_properties(gtest_main PROPERTIES FOLDER "Dependencies")

	if(BUILD_GMOCK)
		set_target_properties(gmock 	 PROPERTIES FOLDER "Dependencies")
		set_target_properties(gmock_main PROPERTIES FOLDER "Dependencies")
	endif()
endif()
