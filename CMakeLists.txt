cmake_minimum_required (VERSION 3.8...3.31)

project(thermos)

enable_testing()

# If the option ENABLE_LTO is enabled (e. g. via `cmake -DENABLE_LTO=ON`)
# during the build, then all binaries will use link-time optimization (LTO).
option(ENABLE_LTO "Enable link-time optimization" OFF)
# Not all compilers support LTO / IPO, so it has to be checked.
if (ENABLE_LTO)
  cmake_policy(SET CMP0069 NEW)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT HAS_LTO_SUPPORT OUTPUT LTO_FAIL_REASON
                      LANGUAGES C CXX)
  if (NOT HAS_LTO_SUPPORT)
    message(FATAL "IPO / LTO is not supported: ${LTO_FAIL_REASON}")
  else()
    message(STATUS "IPO / LTO is supported. Using it.")
  endif()
endif(ENABLE_LTO)

# If ENABLE_STATIC_LINKING is on (e. g. via `cmake -DENABLE_STATIC_LINKING=ON`),
# then all libraries are linked statically. The option is off by default.
#
# Static linking increases the size of the binaries, but those binaries do not
# need the libraries to be present on the system.
option(ENABLE_STATIC_LINKING "Links all libraries statically" OFF)
if (ENABLE_STATIC_LINKING)
  set(CMAKE_LINK_SEARCH_START_STATIC 1)
  set(CMAKE_LINK_SEARCH_END_STATIC 1)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif ()

# option for code coverage
option(CODE_COVERAGE "Build with code coverage (GCC only)" OFF)
# If CODE_COVERAGE is on (e. g. via `cmake -DCODE_COVERAGE=ON`), then all
# binaries are built with code coverage. The option is off by default.
# Currently this only works with GCC.
if (CODE_COVERAGE)
  if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Code coverage builds need GCC compiler!")
  endif ()
  # set coverage options for GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif ()

# If the option NO_SQLITE is enabled (e. g. via `cmake -DNO_SQLITE=ON`)
# during the build, then all features using SQLite 3 are removed.
option(NO_SQLITE "Remove SQLite 3 functionality" OFF)

# If the option USE_BUNDLED_SQLITE is enabled (e. g. via
#`cmake -DUSE_BUNDLED_SQLITE=ON`) during the build, then the SQLite 3 version
# provided in the directory third-party/ is used. If it is disabled, then the
# SQLite 3 library that is provided by the operating system is used instead.
# This option can be useful on operating systems where the package manager does
# not provide SQLite 3 or that do not have a package manager (e. g. Windows).
option(USE_BUNDLED_SQLITE "Use the bundled version of the SQLite 3 library." OFF)

# Recurse into subdirectory for the executable.
add_subdirectory (src)

# Recurse into subdirectory for tests, if we aren't building with MSVC or MinGW.
if (NOT MSVC)
  add_subdirectory (tests)
endif ()
