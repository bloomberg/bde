set(target bsl)

add_library(${target} STATIC)

bbs_setup_target_uor(${target}
                     NO_EMIT_PKG_CONFIG_FILE
                     CUSTOM_PACKAGES "bsl+bslhdrs")

# Those flags are needed to compile test drivers on Windows
# TODO: probably, make sense to make it default argument in
# the cl toolchain.

if (CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(bslma INTERFACE /bigobj)
    target_compile_options(bslstl INTERFACE /bigobj)
    target_compile_options(bsltf INTERFACE /bigobj)
endif()

# To link bsl test drivers that link with package libraries.
target_link_libraries(bsls INTERFACE bbs_threads bbs_bsl_rt)

# This piece of code controls the entries in the bsl.pc file
foreach (t bbs_threads bbs_bsl_rt)
    foreach (prop INTERFACE_COMPILE_DEFINITIONS INTERFACE_COMPILE_OPTIONS)
        get_target_property(value ${t} ${prop})
        if (value)
            set_property(TARGET ${target} APPEND PROPERTY ${prop} ${value})
        endif()
    endforeach()

    get_target_property(value ${t} INTERFACE_LINK_LIBRARIES)
    if (value)
        target_link_libraries(${target} PUBLIC ${value})
    endif()
endforeach()

bbs_emit_pkg_config(${target})
