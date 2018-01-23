function(process outInfoTarget)
    bde_process_standard_package(${outInfoTarget} ${ARGN})
    set(${outInfoTarget} ${${outInfoTarget}} PARENT_SCOPE)

    bde_struct_get_field(interface_target ${${outInfoTarget}} INTERFACE_TARGET)
    bde_interface_target_link_libraries(
        ${interface_target}
        PUBLIC
            $<$<CXX_COMPILER_ID:SunPro>:socket nsl>
    )
endfunction()