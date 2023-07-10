## Get the current Git revision
# Thanks to https://www.mattkeeter.com/blog/2018-01-06-versioning/

execute_process(COMMAND git log --pretty=format:'%h' -n 1
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_REV)

# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout)
if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "N/A")
    set(GIT_BRANCH "N/A")
else()
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

## Get the version numbers from local text file
file (STRINGS "Version.txt" VERSION_RAW)
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_RAW}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_RAW}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_RAW}")
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION_STRING_LONG "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${GIT_REV}")

add_definitions(-DYONAI_VERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DYONAI_VERSION_MINOR=${VERSION_MINOR})
add_definitions(-DYONAI_VERSION_PATCH=${VERSION_PATCH})
add_definitions(-DYONAI_VERSION_REV="${GIT_REV}")
add_definitions(-DYONAI_VERSION_BRANCH="${GIT_BRANCH}")

message("Engine v${VERSION_STRING_LONG} [${GIT_BRANCH}]")