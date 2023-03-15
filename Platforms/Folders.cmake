# Enable folders in solution
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

## Aqua Engine ##
# Engine

# Apps
if(AQUA_BUILD_EDITOR)
	set_target_properties(AquaEditor 		 PROPERTIES FOLDER "Apps")
	set_target_properties(GlueGenerator		 PROPERTIES FOLDER "Apps")
	set_target_properties(AquaEditorLauncher PROPERTIES FOLDER "Apps")
endif()

# Tests
if(AQUA_ENGINE_BUILD_TESTS)
	set_target_properties(AquaEngineTest PROPERTIES FOLDER "Tests")
endif()

## Dependencies ##
set(DEPENDENCY_PROJECTS
	glad
	assimp
	imgui
	spdlog
)

if(ASSIMP_BUILD_ZLIB)
	list(APPEND DEPENDENCY_PROJECTS zlibstatic)
endif()

if(BUILD_GMOCK)
	list(APPEND DEPENDENCY_PROJECTS gmock gmock_main)
endif()

if(BUILD_SHARED_LIBS)
	list(APPEND DEPENDENCY_PROJECTS glm_shared)
endif()

if(AQUA_DESKTOP_PLATFORM)
	list(APPEND DEPENDENCY_PROJECTS glfw uninstall update_mappings)
endif()

if(AQUA_BUILD_BASE_GAME)
	set_target_properties(BaseGame PROPERTIES FOLDER "Apps")
endif()

if(AQUA_BUILD_TESTS)
	list(APPEND DEPENDENCY_PROJECTS
		# Google Tests
		gtest
		gtest_main
	)
	set_target_properties(AquaEngineTest PROPERTIES FOLDER "Apps")
endif()

if(WIN32)
	list(APPEND DEPENDENCY_PROJECTS UpdateAssimpLibsDebugSymbolsAndDLLs)
endif()

foreach(DEPENDENCY IN LISTS DEPENDENCY_PROJECTS)
	set_target_properties(${DEPENDENCY} PROPERTIES FOLDER "Dependencies")
endforeach()
