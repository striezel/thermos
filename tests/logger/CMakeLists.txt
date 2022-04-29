# We might support earlier versions, too, but try to use a recent one.
cmake_minimum_required (VERSION 3.8)

# test: invalid handling of file parameter
add_test(NAME logger_invalid_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-file.sh $<TARGET_FILE:thermos-logger>)

# test: invalid handling of type parameter
add_test(NAME logger_invalid_type_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-type.sh $<TARGET_FILE:thermos-logger>)

# test: file was not specified
add_test(NAME logger_missing_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/missing-logfile.sh $<TARGET_FILE:thermos-logger>)

# test: unknown parameter
add_test(NAME logger_unknown_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unknown-parameter.sh $<TARGET_FILE:thermos-logger>)

# test: logging fails / is aborted
add_test(NAME logger_failure
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/logging-failure.sh $<TARGET_FILE:thermos-logger>)
