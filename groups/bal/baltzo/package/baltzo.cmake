include(bde_interface_target)
include(bde_package)
include(bde_struct)

function(process outInfoTarget)
    bde_force_default_process_package(infoTarget ${ARGN})

    bde_struct_get_field(interfaceTarget ${infoTarget} INTERFACE_TARGET)
    bde_interface_target_link_libraries(
        ${interfaceTarget}
        PUBLIC
            $<$<CXX_COMPILER_ID:MSVC>:advapi32>
    )

    set(${outInfoTarget} ${infoTarget} PARENT_SCOPE)
endfunction()
