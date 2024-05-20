set(target bdl)

add_library(${target} STATIC)

macro(add_extra_libs target scope)
    if (CMAKE_SYSTEM_NAME STREQUAL "SunOS")
        target_link_libraries(${target} ${scope} -lsocket)
    endif()
endmacro()

add_extra_libs(${target} PUBLIC)

bbs_setup_target_uor(${target})

# Custom tweaks for individual packages
target_compile_options(bdlde-iface
                       PRIVATE
                           "$<$<AND:$<PLATFORM_ID:Darwin>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>>:-msse4.2>"
                           "$<$<AND:$<PLATFORM_ID:Linux>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>>:-msse4.2>"
                           "$<$<PLATFORM_ID:SunOs>:-xarch=sparc4>"
)

# Those flags are needed to compile test drivers on Windows
# TODO: probably, make sense to make it default argument in
# the cl toolchain.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(bdlf INTERFACE /bigobj)
    target_compile_options(bdlcc INTERFACE /bigobj)
endif()

add_extra_libs(bdls INTERFACE)

