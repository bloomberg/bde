bde_prefixed_override(bslstp process_package)
function(bslstp_process_package retPackage)
    process_package_base("" package ${ARGN})

    bde_struct_get_field(objlib ${package} OBJ_TARGET)
    bde_struct_get_field(tests ${package} TEST_TARGETS)
    set_target_properties(${objlib} ${tests} PROPERTIES SUPPRESS_BSL_OVERRIDES_STD ON)

    bde_return(${package})
endfunction()