cmake_minimum_required(VERSION 3.15)

if (NOT CMAKE_MODULE_PATH)
    message(FATAL "Please specify path to BDE cmake modules.")
endif()

get_filename_component(repoName ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${repoName})

# Internal support for Conan build.
set(CONAN_BLD_INFO ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
if (EXISTS ${CONAN_BLD_INFO})
    message(STATUS "Found generated conanbuildinfo.cmake, calling conan_basic_setup")
    include(${CONAN_BLD_INFO})
    conan_basic_setup(TARGETS)
endif()

include(bde_workspace)

bde_process_workspace(
    ${CMAKE_CURRENT_LIST_DIR}
)
