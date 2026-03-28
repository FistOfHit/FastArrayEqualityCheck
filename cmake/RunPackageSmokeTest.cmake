set(install_prefix "${FAE_BINARY_DIR}/package")
set(consumer_binary_dir "${FAE_BINARY_DIR}/package-smoke")
set(consumer_source_dir "${FAE_SOURCE_DIR}/tests/package_consumer")

file(REMOVE_RECURSE "${install_prefix}" "${consumer_binary_dir}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${FAE_BINARY_DIR}" --prefix "${install_prefix}"
    RESULT_VARIABLE install_result
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "Failed to install FAE into ${install_prefix}")
endif()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}"
        -S "${consumer_source_dir}"
        -B "${consumer_binary_dir}"
        -DCMAKE_PREFIX_PATH=${install_prefix}
    RESULT_VARIABLE configure_result
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "Failed to configure downstream package consumer")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${consumer_binary_dir}"
    RESULT_VARIABLE build_result
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "Failed to build downstream package consumer")
endif()
