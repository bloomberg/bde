include(bde_package)

function(process_package retPackage)
    default_process_package(package ${ARGN})

    bde_struct_get_field(objlib ${package} OBJ_TARGET)
    bde_struct_get_field(tests ${package} TEST_TARGETS)
    set_target_properties(${objlib} ${tests} PROPERTIES SUPPRESS_BSL_OVERRIDES_STD ON)

    bde_return(${package})
endfunction()