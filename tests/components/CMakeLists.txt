# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# test binary for JSON parsing
project(component_tests)

set(component_tests_sources
    ../../lib/device.cpp
    ../../lib/device_reading.cpp
    ../../lib/reading_type.cpp
    ../../lib/load/reading.cpp
    ../../lib/sqlite/database.cpp
    ../../lib/sqlite/statement.cpp
    ../../lib/storage/csv.hpp
    ../../lib/storage/db.cpp
    ../../lib/storage/factory.cpp
    ../../lib/storage/type.cpp
    ../../lib/storage/utilities.cpp
    ../../lib/templating/htmlspecialchars.cpp
    ../../lib/thermal/reading.cpp
    device.cpp
    reading_type.cpp
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
    thermal/reading.cpp
    main.cpp)

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
  if (CMAKE_VERSION VERSION_LESS "3.14.0")
    # Find module for sqlite3 was added in CMake 3.14.0, so any earlier version
    # needs an extra configuration file to find it.
    set(SQLite3_DIR "../../cmake/" )
  endif ()
  find_package (SQLite3)
  if (SQLite3_FOUND)
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries (component_tests ${SQLite3_LIBRARIES})
    if (ENABLE_STATIC_LINKING)
      target_link_libraries(component_tests dl z pthread)
    endif ()
  else ()
    message ( FATAL_ERROR "SQLite3 was not found!" )
  endif (SQLite3_FOUND)
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
