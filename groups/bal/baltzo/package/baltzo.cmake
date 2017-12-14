function(process_package outName)
    bde_process_standard_package(${outName} ${ARGN})
    set(${outName} ${${outName}} PARENT_SCOPE)

    bde_info_target_get_property(interface_target ${${outName}} INTERFACE_TARGET)
    bde_interface_target_link_libraries(${interface_target} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:advapi32>)
endfunction()
