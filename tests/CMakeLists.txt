# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

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
