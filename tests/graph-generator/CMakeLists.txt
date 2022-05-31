# We might support earlier versions, too, but try to use a recent one.
cmake_minimum_required (VERSION 3.8)

# test: invalid handling of file parameter
add_test(NAME graph-generator_invalid_file_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-file.sh $<TARGET_FILE:thermos-graph-generator>)

# test: invalid handling of output parameter
add_test(NAME graph-generator_invalid_output_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-output.sh $<TARGET_FILE:thermos-graph-generator> ${CMAKE_CURRENT_SOURCE_DIR}/empty.html)

# test: invalid handling of template parameter
add_test(NAME graph-generator_invalid_template_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse-template.sh $<TARGET_FILE:thermos-graph-generator>)

# test: unknown parameter
add_test(NAME graph-generator_unknown_parameter
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unknown-parameter.sh $<TARGET_FILE:thermos-graph-generator>)
