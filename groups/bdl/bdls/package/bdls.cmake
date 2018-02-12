include(bde_interface_target)
include(bde_package)
include(bde_struct)

function(process_package retPackage)
    default_process_package(package ${ARGN})

    bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
    bde_interface_target_link_libraries(
        ${interfaceTarget}
        PUBLIC
            $<$<CXX_COMPILER_ID:SunPro>:socket>
    )

    bde_return(${package})
endfunction()