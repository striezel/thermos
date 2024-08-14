cmake_minimum_required (VERSION 3.8)

if (NOT WIN32)
  set(EXT "sh")
else ()
  set(EXT "cmd")
endif ()

# test: invalid handling of file parameter
add_test(NAME logger_invalid_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-file.${EXT} $<TARGET_FILE:thermos-logger>)

# test: invalid handling of type parameter
add_test(NAME logger_invalid_type_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-type.${EXT} $<TARGET_FILE:thermos-logger>)

# test: file was not specified
add_test(NAME logger_missing_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/missing-logfile.${EXT} $<TARGET_FILE:thermos-logger>)

# test: unknown parameter
add_test(NAME logger_unknown_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unknown-parameter.${EXT} $<TARGET_FILE:thermos-logger>)

# test: logging fails / is aborted
add_test(NAME logger_failure
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/logging-failure.${EXT} $<TARGET_FILE:thermos-logger>)
