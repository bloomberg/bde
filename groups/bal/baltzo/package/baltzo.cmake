include(bde_interface_target)
include(bde_struct)

bde_prefixed_override(baltzo process_package)
function(baltzo_process_package retPackage)
    process_package_base("" package ${ARGN})

    bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
    bde_interface_target_link_libraries(
        ${interfaceTarget}
        INTERFACE
            $<$<CXX_COMPILER_ID:MSVC>:advapi32>
    )

    bde_return(${package})
endfunction()
