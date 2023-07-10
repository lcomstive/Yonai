# Installation using CPack
set(CPACK_COMPONENTS_ALL "")

set(CPACK_PACKAGE_NAME "Yonai Editor")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_VERSION ${VERSION_STRING})
set(CPACK_PACKAGE_VENDOR "Madissia Technologies")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_STRIP_FILES ".DS_Store;.git;Engine/Vendor;")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Yonai Editor/v${VERSION_STRING}")

if("${GIT_BRANCH}" STREQUAL "main") # Production build
	set(CPACK_PACKAGE_FILE_NAME "Yonai Editor v${VERSION_STRING}")
elseif("${GIT_BRANCH}" STREQUAL "staging") # Staging build
	set(CPACK_PACKAGE_FILE_NAME "Yonai Editor v${VERSION_STRING_LONG}")
else() # Dev build
	set(CPACK_PACKAGE_FILE_NAME "Yonai Editor v${VERSION_STRING_LONG}-dev")
endif()

if(APPLE)
	install(
		TARGETS YonaiEditor
		BUNDLE DESTINATION .
	)

	set(CPACK_GENERATOR "DragNDrop")
	set(CPACK_PACKAGE_EXECUTABLES "YonaiEditor.app" "Yonai Editor")
	set(CPACK_BUNDLE_NAME "Yonai Editor")
	set(CPACK_BUNDLE_PLIST ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/YonaiEditor.app/Contents/Info.plist)
	set(CPACK_DMG_VOLUME_NAME ${CPACK_SOURCE_PACKAGE_FILE_NAME})
	set(CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/Platforms/Mac/AppIcon.icns)
	set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/Platforms/Mac/AppIcon.icns)
elseif(WIN32)
	install(TARGETS YonaiEditor DESTINATION .)

	install(DIRECTORY ${YONAI_RESOURCES_DIR}/Assets DESTINATION .)
	install(FILES ${YONAI_RESOURCES_DIR}/YonaiScriptCore.dll DESTINATION .)
	install(FILES ${YONAI_RESOURCES_DIR}/YonaiScriptEditor.dll DESTINATION .)

	if(YONAI_BUILD_BASE_GAME)
		install(TARGETS BaseGame DESTINATION .)
	endif()

	if(BUILD_SHARED_LIBS)
		install(FILES $<TARGET_RUNTIME_DLLS:YonaiEditor> DESTINATION .)
		install(FILES ${MONO_ROOT}/bin/mono-2.0-sgen.dll DESTINATION .)
	endif()

	if(DEFINED ENV{CMAKE_CONFIG})
		set(YONAI_CPACK_CONFIG $ENV{CMAKE_CONFIG})
	else()
		set(YONAI_CPACK_CONFIG "Release")
	endif()

	# Add start menu shortcut(s)
	set(CPACK_NSIS_CREATE_ICONS_EXTRA
		"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Yonai Editor.lnk' '$INSTDIR\\\\Yonai Editor.exe'"
	)

	# Replace '/' with '\\'; regex so have to escape characters
	STRING(REGEX REPLACE "/" "\\\\\\\\" CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_INSTALL_DIRECTORY} )

	set(CPACK_GENERATOR "NSIS64")
	set(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/Platforms/Windows/YonaiIcon.ico)
	set(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/Platforms/Windows/YonaiIcon.ico)
	set(CPACK_NSIS_INSTALLED_ICON_NAME ${CMAKE_SOURCE_DIR}/Platforms/Windows/YonaiIcon.ico)
else()
	install(FILES
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/YonaiEditor
		DESTINATION .)
	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets DESTINATION .)
	
	if(BUILD_SHARED_LIBS)
		install(FILES
			$<TARGET_FILE:YonaiEngine>
			$<TARGET_FILE:assimp>
			$<TARGET_FILE:glfw>
			$<TARGET_FILE:spdlog>
			DESTINATION .)
	endif()
	
	set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
