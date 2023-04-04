option(AQUA_BUILD_EDITOR "Build the Aqua Editor" ON)
option(AQUA_BUILD_BASE_GAME "Build the base game application" ON)

if(MSVC)
	option(AQUA_GENERATE_MDBS "Generate mono-compatible debug symbol files for AquaScriptCore and AquaScriptEditor" OFF)
endif()

if(WIN32)
	set(EXECUTABLE_RESOURCES ${CMAKE_SOURCE_DIR}/Platforms/Windows/AppIcon.rc)
else()
	set(EXECUTABLE_RESOURCES "")
endif()

# AquaScriptCore
add_subdirectory(AquaScriptCore)

# Base Game
if(AQUA_BUILD_BASE_GAME)
	add_subdirectory(BaseGame)
endif()

# Editor
if(AQUA_BUILD_EDITOR)
	add_subdirectory(Editor)
	add_subdirectory(EditorLauncher)
	add_subdirectory(AquaScriptEditor)
endif()
