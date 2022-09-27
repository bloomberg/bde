set(target bal)

add_library(${target} STATIC)
bbs_setup_target_uor(${target})

# Those flags are needed to compile/link test drivers on Windows
# TODO: probably, make sense to make it default argument in
# the cl toolchain.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(baljsn INTERFACE /bigobj)
    target_compile_options(balxml INTERFACE /bigobj)
    target_link_libraries(baltzo INTERFACE advapi32)
endif()
