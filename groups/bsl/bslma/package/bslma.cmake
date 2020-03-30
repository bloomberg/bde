include(bde_package)

bde_prefixed_override(bslma package_setup_interface)
function(bslma_package_setup_interface package)
    package_setup_interface_base("" ${ARGV})

    bde_struct_get_field(testInterface ${package} TEST_INTERFACE_TARGET)
    bde_interface_target_compile_options(
        ${testInterface}
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:
                /bigobj
            >
    )
endfunction()
