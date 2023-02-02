# Enable folders in solution
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

## Aqua Engine ##
# Engine

# Apps
set_target_properties(BaseGame 	 		 PROPERTIES FOLDER "Apps")
set_target_properties(AquaEditor 		 PROPERTIES FOLDER "Apps")
set_target_properties(GlueGenerator		 PROPERTIES FOLDER "Apps")
set_target_properties(AquaEditorLauncher PROPERTIES FOLDER "Apps")

# Tests
if(AQUA_ENGINE_BUILD_TESTS)
	set_target_properties(AquaEngineTest PROPERTIES FOLDER "Tests")
endif()

## Dependencies ##
# Glad
set_target_properties(glad PROPERTIES FOLDER "Dependencies")
