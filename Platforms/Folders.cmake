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

# Assimp
set_target_properties(assimp PROPERTIES FOLDER "Dependencies")

if(UpdateAssimpLibsDebugSymbolsAndDLLs)
	set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies")
endif()

# GLFW
if(glfw)
	set_target_properties(glfw PROPERTIES FOLDER "Dependencies")
	set_target_properties(uninstall PROPERTIES FOLDER "Dependencies")
	set_target_properties(update_mappings PROPERTIES FOLDER "Dependencies")
endif()

# Google Test
if(AQUA_ENGINE_BUILD_TESTS)
	set_target_properties(gtest 	 PROPERTIES FOLDER "Dependencies")
	set_target_properties(gtest_main PROPERTIES FOLDER "Dependencies")

	if(BUILD_GMOCK)
		set_target_properties(gmock 	 PROPERTIES FOLDER "Dependencies")
		set_target_properties(gmock_main PROPERTIES FOLDER "Dependencies")
	endif()
endif()

# Misc.
if(zlibstatic)
	set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies")
endif()
