# Enable folders in solution
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

## Yonai Engine ##
# Engine

# Apps
set_target_properties(GlueGenerator		 PROPERTIES FOLDER "Apps")
if(YONAI_BUILD_EDITOR)
	set_target_properties(YonaiEditor 		 PROPERTIES FOLDER "Apps")
endif()

# Tests
if(YONAI_ENGINE_BUILD_TESTS)
	set_target_properties(YonaiEngineTest PROPERTIES FOLDER "Tests")
endif()

## Dependencies ##
set(DEPENDENCY_PROJECTS
	assimp
	spdlog
)

if(YONAI_BUILD_EDITOR)
	list(APPEND DEPENDENCY_PROJECTS imgui)
endif()

if(ASSIMP_BUILD_ZLIB)
	list(APPEND DEPENDENCY_PROJECTS zlibstatic)
endif()

if(BUILD_GMOCK)
	list(APPEND DEPENDENCY_PROJECTS gmock gmock_main)
endif()

if(BUILD_SHARED_LIBS)
	list(APPEND DEPENDENCY_PROJECTS glm_shared)
endif()

if(YONAI_DESKTOP_PLATFORM)
	list(APPEND DEPENDENCY_PROJECTS glfw uninstall update_mappings)
endif()

if(YONAI_BUILD_BASE_GAME)
	set_target_properties(BaseGame PROPERTIES FOLDER "Apps")
endif()

if(YONAI_BUILD_TESTS)
	list(APPEND DEPENDENCY_PROJECTS
		# Google Tests
		gtest
		gtest_main
	)
	set_target_properties(YonaiTest PROPERTIES FOLDER "Apps")
endif()

if(WIN32)
	list(APPEND DEPENDENCY_PROJECTS UpdateAssimpLibsDebugSymbolsAndDLLs)
endif()

foreach(DEPENDENCY IN LISTS DEPENDENCY_PROJECTS)
	set_target_properties(${DEPENDENCY} PROPERTIES FOLDER "Dependencies")
endforeach()
