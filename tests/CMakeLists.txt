cmake_minimum_required (VERSION 3.8...3.31)

include (CheckIncludeFileCXX)
check_include_file_cxx (catch2/catch_test_macros.hpp HAS_CATCH_V3_HEADER)
check_include_file_cxx (catch2/catch.hpp HAS_CATCH_V2_HEADER)

if (HAS_CATCH_V3_HEADER AND NOT HAS_CATCH_V2_HEADER)
  set(HAS_CATCH_V3 ON)
else ()
  set(HAS_CATCH_V3 OFF)
endif ()

# Recurse into subdirectory for component tests.
add_subdirectory (components)

if (NOT NO_SQLITE)
    # Recurse into subdirectory for db2csv tests.
    add_subdirectory (db2csv)

    # Recurse into subdirectory for graph-generator tests.
    add_subdirectory (graph-generator)
endif ()

# Recurse into subdirectory for logger tests.
add_subdirectory (logger)
