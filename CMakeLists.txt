cmake_minimum_required(VERSION 3.21)

project(
    FAE
    VERSION 0.1.0
    DESCRIPTION "Probabilistic array fingerprinting for integral arrays"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(FAE_BUILD_TESTS "Build unit tests and smoke tests" ON)
option(FAE_BUILD_EXAMPLES "Build example programs" ON)
option(FAE_BUILD_BENCHMARKS "Build benchmark and collision-study tools" ON)
option(FAE_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(FAE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

add_library(fae
    src/fingerprint.cpp
    src/pairing/szudzik.cpp
    src/reduction/pairwise_fold.cpp
)

add_library(fae::fae ALIAS fae)

target_compile_features(fae PUBLIC cxx_std_20)
set_target_properties(fae PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME fae
)

target_include_directories(fae
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(MSVC)
    target_compile_options(fae PRIVATE /W4)
    if(FAE_WARNINGS_AS_ERRORS)
        target_compile_options(fae PRIVATE /WX)
    endif()
else()
    target_compile_options(fae PRIVATE -Wall -Wextra -Wpedantic)
    if(FAE_WARNINGS_AS_ERRORS)
        target_compile_options(fae PRIVATE -Werror)
    endif()
endif()

install(
    TARGETS fae
    EXPORT FAETargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT FAETargets
    FILE faeTargets.cmake
    NAMESPACE fae::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fae
)

configure_package_config_file(
    cmake/faeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/faeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fae
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/faeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/faeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/faeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fae
)

if(FAE_BUILD_EXAMPLES)
    add_executable(fae_example_fingerprint examples/basic_fingerprint.cpp)
    target_link_libraries(fae_example_fingerprint PRIVATE fae::fae)

    add_executable(fae_example_compare examples/compare_fingerprints.cpp)
    target_link_libraries(fae_example_compare PRIVATE fae::fae)
endif()

if(FAE_BUILD_BENCHMARKS)
    add_executable(fae_benchmark benchmarks/benchmark_main.cpp)
    target_link_libraries(fae_benchmark PRIVATE fae::fae)

    add_executable(fae_collision_study benchmarks/collision_study.cpp)
    target_link_libraries(fae_collision_study PRIVATE fae::fae)
endif()

include(CTest)
if(BUILD_TESTING AND FAE_BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    add_executable(fae_tests
        tests/fingerprint_tests.cpp
        tests/pairing_tests.cpp
        tests/reduction_tests.cpp
    )
    target_link_libraries(fae_tests PRIVATE fae::fae GTest::gtest_main)
    gtest_discover_tests(fae_tests)

    add_test(
        NAME fae_package_smoke_test
        COMMAND
            ${CMAKE_COMMAND}
            -DFAE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DFAE_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunPackageSmokeTest.cmake
    )

    if(FAE_BUILD_BENCHMARKS)
        add_test(
            NAME fae_benchmark_smoke
            COMMAND fae_benchmark --profile=smoke
        )
        set_tests_properties(fae_benchmark_smoke PROPERTIES TIMEOUT 60)

        add_test(
            NAME fae_collision_study_smoke
            COMMAND fae_collision_study --profile=smoke
        )
        set_tests_properties(fae_collision_study_smoke PROPERTIES TIMEOUT 60)
    endif()
endif()

if(FAE_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    add_custom_target(
        docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
