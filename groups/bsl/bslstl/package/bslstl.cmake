include(bde_package)

function(process_package retPackage)
    bde_process_package(package ${ARGN})
        # Call bde_process_package directly to avoid setting BSL_OVERRIDES_STD

    bde_struct_get_field(testInterface ${package} TEST_INTERFACE_TARGET)
    bde_interface_target_compile_options(
        ${testInterface}
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:
                /bigobj
            >
    )

    bde_return(${package})
endfunction()