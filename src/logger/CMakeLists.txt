# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(thermos-logger)

set(thermos_logger_sources
    ../../lib/device.cpp
    ../../lib/device_reading.hpp
    ../../lib/load/calculator.cpp
    ../../lib/load/read.cpp
    ../../lib/load/read_linux.cpp
    ../../lib/load/read_windows.cpp
    ../../lib/load/reading.cpp
    ../../lib/reading_base.cpp
    ../../lib/reading_type.cpp
    ../../lib/sqlite/database.cpp
    ../../lib/sqlite/statement.cpp
    ../../lib/storage/csv.hpp
    ../../lib/storage/db.cpp
    ../../lib/storage/factory.cpp
    ../../lib/storage/type.cpp
    ../../lib/storage/utilities.cpp
    ../../lib/thermal/read.cpp
    ../../lib/thermal/read_linux.cpp
    ../../lib/thermal/read_windows.cpp
    ../../lib/thermal/reading.cpp
    ../util/GitInfos.cpp
    Logger.cpp
    main.cpp)

if (NOT NO_SQLITE AND USE_BUNDLED_SQLITE)
    list(APPEND thermos_logger_sources
    ../../third-party/sqlite/sqlite3.c)
    # add definitions to get rid of some unused stuff
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions ( -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DQS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 -DSQLITE_OMIT_DECLTYPE=1 -DSQLITE_OMIT_DEPRECATED=1 -DSQLITE_OMIT_COMPLETE=1 -DSQLITE_OMIT_JSON=1 )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions ( /DSQLITE_DEFAULT_MEMSTATUS=0 /DSQLITE_DQS=0 /DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 /DSQLITE_OMIT_DECLTYPE=1 /DSQLITE_OMIT_DEPRECATED=1 /DSQLITE_OMIT_COMPLETE=1 /DSQLITE_OMIT_JSON=1 )
    endif ()

    message(STATUS "logger is built with bundled version of SQLite.")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -pedantic-errors -pedantic -Wextra -fexceptions)
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

add_executable(thermos-logger ${thermos_logger_sources})

if (NOT NO_SQLITE)
    # find sqlite3 library
    if (USE_BUNDLED_SQLITE)
        include_directories("../../third-party/sqlite/")
        # link to some libraries required on Linux / Unix-like systems
        if (UNIX)
            target_link_libraries(thermos-logger dl pthread)
        endif ()
    else ()
        if (CMAKE_VERSION VERSION_LESS "3.14.0")
            # Find module for sqlite3 was added in CMake 3.14.0, so any earlier
            # version needs an extra configuration file to find it.
            set(SQLite3_DIR "../../cmake/" )
        endif ()
        find_package (SQLite3)
        if (SQLite3_FOUND)
            include_directories(${SQLite3_INCLUDE_DIRS})
            target_link_libraries (thermos-logger ${SQLite3_LIBRARIES})
            if (ENABLE_STATIC_LINKING)
                target_link_libraries(thermos-logger dl z pthread)
            endif ()
        else ()
            message ( FATAL_ERROR "SQLite3 was not found!" )
        endif (SQLite3_FOUND)
    endif (USE_BUNDLED_SQLITE)
endif ()


if (MINGW)
     # MSVC links to them via "#pragma comment(lib, "foo.lib")", but MinGW does
     # not support that.
     target_link_libraries(thermos-logger wbemuuid kernel32)
endif ()

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(thermos-logger stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(thermos-logger stdc++fs)
  else ()
   # Use Clang's C++ filesystem library, it is recent enough.
   target_link_libraries(thermos-logger c++fs)
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
add_test(NAME thermos_logger_help
         COMMAND $<TARGET_FILE:thermos-logger> --help)

# short help parameter with question mark "-?"
add_test(NAME thermos_logger_help_question_mark
         COMMAND $<TARGET_FILE:thermos-logger> -?)

# Windows-style help parameter "/?"
add_test(NAME thermos_logger_help_question_mark_windows
         COMMAND $<TARGET_FILE:thermos-logger> /?)

# parameter to show version information
add_test(NAME thermos_logger_version
         COMMAND $<TARGET_FILE:thermos-logger> --version)
