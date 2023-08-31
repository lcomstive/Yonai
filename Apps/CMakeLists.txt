if(MSVC)
	option(YONAI_GENERATE_MDBS "Generate mono-compatible debug symbol files for YonaiScriptCore and YonaiScriptEditor" ON)
endif()

if(WIN32)
	set(EXECUTABLE_RESOURCES ${CMAKE_SOURCE_DIR}/Platforms/Windows/AppIcon.rc)
else()
	set(EXECUTABLE_RESOURCES "")
endif()

# YonaiScriptCore
add_subdirectory(YonaiScriptCore)

# Base Game
if(YONAI_BUILD_BASE_GAME)
	add_subdirectory(BaseGame)
endif()

# Editor
if(YONAI_BUILD_EDITOR)
	add_subdirectory(Editor)
	add_subdirectory(YonaiScriptEditor)
endif()
