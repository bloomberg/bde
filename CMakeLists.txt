cmake_minimum_required(VERSION 3.24)
project(bde)

enable_testing()

find_package(BdeBuildSystem REQUIRED)

if(NOT BBS_BUILD_SYSTEM)
    bbs_build_tests_with_all()
endif()

add_subdirectory(thirdparty)
add_subdirectory(groups)
add_subdirectory(standalones)
