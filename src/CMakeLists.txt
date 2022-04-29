# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# Recurse into subdirectory for the info executable.
add_subdirectory (info)

# Recurse into subdirectory for the logger executable.
add_subdirectory (logger)
