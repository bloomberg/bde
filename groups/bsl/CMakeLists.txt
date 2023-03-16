set(target bsl)

add_library(${target} STATIC)

bbs_setup_target_uor(${target}
                     NO_EMIT_PKG_CONFIG_FILE)

# Those flags are needed to compile test drivers on Windows
# TODO: probably, make sense to make it default argument in
# the cl toolchain.

if (CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(bslma INTERFACE /bigobj)
    target_compile_options(bslstl INTERFACE /bigobj)
    target_compile_options(bsltf INTERFACE /bigobj)
endif()

# This code is needed to correctly generate bls.pc file
if (NOT BDE_BUILD_TARGET_NO_MT)
    find_package(Threads REQUIRED)
    bbs_add_target_thread_flags(bsls PUBLIC)
    bbs_add_target_thread_flags(${target} PUBLIC)
    target_link_libraries(bsls PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(bsl  PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

# For test drivers
bbs_add_target_rt_flags(bsls PUBLIC)

# For pkg config and higher level test drivers
bbs_add_target_rt_flags(${target} PUBLIC)

bbs_emit_pkg_config(${target})
