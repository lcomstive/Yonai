set(YONAI_BUILD_TESTS ON CACHE BOOL "Build the test suite")

include(./Dependencies.cmake)

# Yonai Engine Core
project(Yonai)
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/Yonai/*.hpp")

list(APPEND HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/Yonai/Glue.hpp")

if(APPLE)
	file(GLOB_RECURSE OBJC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.mm")
	list(APPEND SOURCE_FILES ${OBJC_FILES})
endif()

if(NOT BUILD_SHARED_LIBS)
	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
else()
	add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

	target_compile_definitions(${PROJECT_NAME} PUBLIC  BUILD_SHARED_LIBS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE YONAI_EXPORT_DLL)
endif()

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	VERSION ${VERSION_STRING}
)

AddPlatformSpecifics()

# Group source files inside IDE
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"  PREFIX "Source"  FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Include" FILES ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
	Include
	${YONAI_DEPENDENCY_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${YONAI_DEPENDENCY_LIBS})

if(NOT WIN32)
	include(GNUInstallDirs)

	install(
		TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)
endif()

if(UNIX)
	target_compile_options(${PROJECT_NAME} PUBLIC -Wno-delete-incomplete)
endif()

if(YONAI_BUILD_TESTS)
	add_subdirectory(./Tests)
endif()

# Glue generator
add_subdirectory(./GlueGenerator)
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Include/Yonai/Glue.hpp"
	COMMAND GlueGenerator "-prefix=_" "${CMAKE_CURRENT_SOURCE_DIR}/Source" "${CMAKE_CURRENT_SOURCE_DIR}/Include/Yonai/Glue.hpp"
	DEPENDS GlueGenerator
)

