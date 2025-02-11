cmake_minimum_required (VERSION 3.8...3.31)

IF (NOT WIN32)
    set(EXT "sh")
else ()
    set(EXT "cmd")
endif ()

# test: invalid handling of file parameter
add_test(NAME db2csv_invalid_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-file.${EXT} $<TARGET_FILE:thermos-db2csv>)

# test: file was not specified
add_test(NAME db2csv_missing_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/missing-file.${EXT} $<TARGET_FILE:thermos-db2csv>)

# test: unknown parameter
add_test(NAME db2csv_unknown_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unknown-parameter.${EXT} $<TARGET_FILE:thermos-db2csv>)

# test: logging fails / is aborted
add_test(NAME db2csv_failure
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/conversion-failure.${EXT} $<TARGET_FILE:thermos-db2csv>)
