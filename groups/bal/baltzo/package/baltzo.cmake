include(bde_interface_target)
include(bde_struct)

function(process_package retPackage)
    default_process_package(package ${ARGN})

    bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
    bde_interface_target_link_libraries(
        ${interfaceTarget}
        INTERFACE
            $<$<CXX_COMPILER_ID:MSVC>:advapi32>
    )

    bde_return(${package})
endfunction()
