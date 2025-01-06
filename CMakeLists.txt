cmake_minimum_required(VERSION 3.24)
project(bde)

enable_testing()

find_package(BdeBuildSystem REQUIRED)

add_subdirectory(thirdparty)
add_subdirectory(groups)
add_subdirectory(standalones)
