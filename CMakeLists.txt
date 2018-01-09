cmake_minimum_required(VERSION 3.8)

project(BDE VERSION 4.0)

if (NOT CMAKE_MODULE_PATH)
    message(FATAL "Please specify path to BDE cmake modules.")
endif()

include(bde_project)
bde_process_workspace(
    ${CMAKE_CURRENT_LIST_DIR}
)
