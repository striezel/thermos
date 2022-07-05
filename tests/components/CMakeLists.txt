# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# test binary for JSON parsing
project(component_tests)

set(component_tests_sources
    ../../lib/device.cpp
    ../../lib/device_reading.hpp
    ../../lib/reading_type.cpp
    ../../lib/load/reading.cpp
    ../../lib/reading_base.cpp
    ../../lib/sqlite/database.cpp
    ../../lib/sqlite/statement.cpp
    ../../lib/storage/csv.hpp
    ../../lib/storage/db.cpp
    ../../lib/storage/factory.cpp
    ../../lib/storage/type.cpp
    ../../lib/storage/utilities.cpp
    ../../lib/templating/htmlspecialchars.cpp
    ../../lib/templating/template.cpp
    ../../lib/templating/vectorize.cpp
    ../../lib/thermal/reading.cpp
    device.cpp
    reading_type.cpp
    load/device_reading.cpp
    load/reading.cpp
    sqlite/database.cpp
    sqlite/statement.cpp
    storage/csv.cpp
    storage/db.cpp
    storage/factory.cpp
    storage/to_time.cpp
    storage/type.cpp
    storage/utilities.cpp
    templating/htmlspecialchars.cpp
    templating/template.cpp
    templating/vectorize.cpp
    thermal/device_reading.cpp
    thermal/reading.cpp
    main.cpp)

if (NOT NO_SQLITE AND USE_BUNDLED_SQLITE)
    list(APPEND component_tests_sources
    ../../third-party/sqlite/sqlite3.c)
    # add definitions to get rid of some unused stuff
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions ( -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DQS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 -DSQLITE_OMIT_COMPLETE=1 -DSQLITE_OMIT_DECLTYPE=1 -DSQLITE_OMIT_DEPRECATED=1 -DSQLITE_OMIT_JSON=1 )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions ( /DSQLITE_DEFAULT_MEMSTATUS=0 /DSQLITE_DQS=0 /DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 /DSQLITE_OMIT_COMPLETE=1 /DSQLITE_OMIT_DECLTYPE=1 /DSQLITE_OMIT_DEPRECATED=1 /DSQLITE_OMIT_JSON=1 )
    endif ()

    message(STATUS "Tests are built with bundled version of SQLite.")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
endif ()

if (NO_SQLITE)
    add_definitions( -DTHERMOS_NO_SQLITE=1 )
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(component_tests ${component_tests_sources})

if (NOT NO_SQLITE)
  # find sqlite3 library
  if (USE_BUNDLED_SQLITE)
    include_directories("../../third-party/sqlite/")
    # link to some libraries required on Linux / Unix-like systems
    if (UNIX)
        target_link_libraries(component_tests dl pthread)
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
      target_link_libraries (component_tests ${SQLite3_LIBRARIES})
      if (ENABLE_STATIC_LINKING)
        if (NOT MINGW)
          target_link_libraries(component_tests dl z pthread)
        else ()
          target_link_libraries(component_tests z pthread)
        endif ()
      endif ()
    else ()
      message ( FATAL_ERROR "SQLite3 was not found!" )
    endif (SQLite3_FOUND)
  endif ()
endif ()

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(component_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(component_tests stdc++fs)
  else ()
   # Use Clang's C++ filesystem library, it is recent enough.
   target_link_libraries(component_tests c++fs)
  endif ()
endif ()

# add test case
add_test(NAME test_components
         COMMAND $<TARGET_FILE:component_tests>)
