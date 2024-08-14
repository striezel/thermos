cmake_minimum_required (VERSION 3.8)

project(thermos-graph-generator)

set(thermos_graph_generator_sources
    ../../lib/device.cpp
    ../../lib/load/reading.cpp
    ../../lib/reading_base.cpp
    ../../lib/reading_type.cpp
    ../../lib/sqlite/database.cpp
    ../../lib/sqlite/statement.cpp
    ../../lib/storage/db.cpp
    ../../lib/storage/utilities.cpp
    ../../lib/templating/htmlspecialchars.cpp
    ../../lib/templating/template.cpp
    ../../lib/templating/vectorize.cpp
    ../../lib/thermal/reading.cpp
    ../util/GitInfos.cpp
    ../Version.cpp
    generator.cpp
    main.cpp)

if (NOT NO_SQLITE AND USE_BUNDLED_SQLITE)
    list(APPEND thermos_graph_generator_sources
    ../../third-party/sqlite/sqlite3.c)
    # add definitions to get rid of some unused stuff
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions ( -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DQS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 -DSQLITE_OMIT_COMPLETE=1 -DSQLITE_OMIT_DECLTYPE=1 -DSQLITE_OMIT_DEPRECATED=1 -DSQLITE_OMIT_JSON=1 )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions ( /DSQLITE_DEFAULT_MEMSTATUS=0 /DSQLITE_DQS=0 /DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 /DSQLITE_OMIT_COMPLETE=1 /DSQLITE_OMIT_DECLTYPE=1 /DSQLITE_OMIT_DEPRECATED=1 /DSQLITE_OMIT_JSON=1 )
    endif ()

    message(STATUS "graph-generator is built with bundled version of SQLite.")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -pedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NO_SQLITE)
    add_definitions( -DTHERMOS_NO_SQLITE=1 )
endif ()

add_executable(thermos-graph-generator ${thermos_graph_generator_sources})

if (NOT NO_SQLITE)
    if (USE_BUNDLED_SQLITE)
        include_directories("../../third-party/sqlite/")
        # link to some libraries required on Linux / Unix-like systems
        if (UNIX)
            target_link_libraries(thermos-graph-generator dl pthread)
        endif ()
    else ()
        # find sqlite3 library
        if (CMAKE_VERSION VERSION_LESS "3.14.0")
            # Find module for sqlite3 was added in CMake 3.14.0, so any earlier
            # version needs an extra configuration file to find it.
            set(SQLite3_DIR "../../cmake/" )
        endif ()
        find_package (SQLite3)
        if (SQLite3_FOUND)
            include_directories(${SQLite3_INCLUDE_DIRS})
            target_link_libraries (thermos-graph-generator ${SQLite3_LIBRARIES})
            if (ENABLE_STATIC_LINKING)
                if (NOT MINGW)
                    target_link_libraries(thermos-graph-generator dl z pthread)
                else ()
                    target_link_libraries(thermos-graph-generator z pthread)
                endif ()
            endif ()
        else ()
            message ( FATAL_ERROR "SQLite3 was not found!" )
        endif (SQLite3_FOUND)
    endif (USE_BUNDLED_SQLITE)
endif ()

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(thermos-graph-generator stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(thermos-graph-generator stdc++fs)
  else ()
    # Use Clang's C++ filesystem library, it is recent enough.
    target_link_libraries(thermos-graph-generator c++fs)
  endif ()
endif ()

# create git-related constants
# -- get the current commit hash
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# -- get the commit date
execute_process(
  COMMAND git show -s --format=%ci
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TIME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("GIT_HASH is ${GIT_HASH}.")
message("GIT_TIME is ${GIT_TIME}.")

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.cpp
               ESCAPE_QUOTES)

# #################### #
# tests for executable #
# #################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME thermos_graph_generator_help
         COMMAND $<TARGET_FILE:thermos-graph-generator> --help)

# short help parameter with question mark "-?"
add_test(NAME thermos_graph_generator_help_question_mark
         COMMAND $<TARGET_FILE:thermos-graph-generator> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
    add_test(NAME thermos_graph_generator_help_question_mark_windows
             COMMAND $<TARGET_FILE:thermos-graph-generator> /?)
endif ()

# parameter to show version information
add_test(NAME thermos_graph_generator_version
         COMMAND $<TARGET_FILE:thermos-graph-generator> --version)
