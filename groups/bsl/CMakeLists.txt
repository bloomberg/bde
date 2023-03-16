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

# For test drivers
bbs_add_target_rt_flags(bsls PUBLIC)
bbs_add_target_thread_flags(bsls PUBLIC)

# For pkg config and higher level test drivers
bbs_add_target_rt_flags(${target} PUBLIC)
bbs_add_target_thread_flags(${target} PUBLIC)

bbs_emit_pkg_config(${target})
