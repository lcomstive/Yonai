function (SetWin32Properties)
	if(NOT WIN32)
		return()
	endif()

	# Link flags
	#	/ignore:4099 - Ignores warning when no pdb (debug info) is found with linking target (sucha as the mono library)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		LINK_FLAGS "/ignore:4099"
		LINK_FLAGS_DEBUG   			"/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_RELEASE 			"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
		LINK_FLAGS_MINSIZEREL 		"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
		LINK_FLAGS_RELWITHDEBINFO 	"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
	)
endfunction()