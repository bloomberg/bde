cmake_minimum_required(VERSION 3.8)

project(BDE VERSION 4.0)
set(CMAKE_MODULE_PATH "/bb/bde/bbshr/bde-oss-tools/cmake/")

include(bde_project)
bde_process_workspace(
    ${CMAKE_CURRENT_LIST_DIR}
)
