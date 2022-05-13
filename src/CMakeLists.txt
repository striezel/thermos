# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

if (NOT NO_SQLITE)
    # Recurse into subdirectory for the db2csv executable.
    add_subdirectory (db2csv)
endif ()

# Recurse into subdirectory for the info executable.
add_subdirectory (info)

# Recurse into subdirectory for the logger executable.
add_subdirectory (logger)
