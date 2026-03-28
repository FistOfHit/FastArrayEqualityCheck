cmake_minimum_required(VERSION 3.21)

project(FAEPackageConsumer LANGUAGES CXX)

find_package(fae CONFIG REQUIRED)

add_executable(fae_package_consumer main.cpp)
target_link_libraries(fae_package_consumer PRIVATE fae::fae)
target_compile_features(fae_package_consumer PRIVATE cxx_std_20)
