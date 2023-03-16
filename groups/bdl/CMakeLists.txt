set(target bdl)

add_library(${target} STATIC)

bbs_setup_target_uor(${target}
                     NO_EMIT_PKG_CONFIG_FILE)

# Custom tweaks for individual packages
target_compile_options(bdlde-iface
                       PRIVATE
                           "$<$<AND:$<PLATFORM_ID:Darwin>,$<STREQUAL:${CMAKE_HOST_SYSTEM_PROCESSOR},x86_64>>:-msse4.2>"
                           "$<$<PLATFORM_ID:Linux>:-msse4.2>"
                           "$<$<PLATFORM_ID:SunOs>:-xarch=sparc4>"
)

# Those flags are needed to compile test drivers on Windows
# TODO: probably, make sense to make it default argument in
# the cl toolchain.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(bdlf INTERFACE /bigobj)
    target_compile_options(bdlcc INTERFACE /bigobj)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "SunPro")
    target_link_libraries(bdls INTERFACE -lsocket)
    target_link_libraries(${target} PUBLIC -lsocket)
endif()

bbs_emit_pkg_config(${target})
