# We might support earlier versions, too, but try to use a recent one.
cmake_minimum_required (VERSION 3.8)

# test: invalid handling of file parameter
add_test(NAME db2csv_invalid_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-file.sh $<TARGET_FILE:thermos-db2csv>)

# test: file was not specified
add_test(NAME db2csv_missing_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/missing-file.sh $<TARGET_FILE:thermos-db2csv>)

# test: unknown parameter
add_test(NAME db2csv_unknown_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unknown-parameter.sh $<TARGET_FILE:thermos-db2csv>)

# test: logging fails / is aborted
add_test(NAME db2csv_failure
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/conversion-failure.sh $<TARGET_FILE:thermos-db2csv>)
