cmake_minimum_required(VERSION 3.8)

if (NOT CMAKE_MODULE_PATH)
    message(FATAL "Please specify path to BDE cmake modules.")
endif()

get_filename_component(repoName ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${repoName})

include(bde_workspace)

bde_process_workspace(
    ${CMAKE_CURRENT_LIST_DIR}
)
