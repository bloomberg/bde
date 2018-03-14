include(legacy/wafstyleout)

include(bde_project)
include(bde_utils)

include(layers/package_libs)
include(layers/ufid)
include(layers/install_pkg_config)
include(layers/install_cmake_config)

bde_prefixed_override(bdeproj project_initialize)
function(bdeproj_project_initialize retProj listDir)
    bde_assert_no_extra_args()

    project_initialize_base("" proj ${listDir})

    bde_add_interface_target(bsl_override_std)
    bde_interface_target_compile_definitions(
        bsl_override_std
            PUBLIC
                $<$<NOT:$<BOOL:$<TARGET_PROPERTY:SUPPRESS_BSL_OVERRIDES_STD>>>:BSL_OVERRIDES_STD>
    )

    bde_struct_append_field(${proj} COMMON_INTERFACE_TARGETS bsl_override_std)

    bde_return(${proj})
endfunction()

bde_prefixed_override(bdeproj project_process_uors)
function(bdeproj_project_process_uors proj listDir)
    bde_assert_no_extra_args()

    bde_project_process_package_groups(
        ${proj}
        ${listDir}/groups/bal
        ${listDir}/groups/bbl
        ${listDir}/groups/bdl
        ${listDir}/groups/bsl
        ${listDir}/groups/btl
    )

    # Note, that thirdparty libraries do not use COMMON_INTERFACE_TARGETS
    bde_struct_set_field(${proj} COMMON_INTERFACE_TARGETS "")
    bde_project_process_standalone_packages(
        ${proj}
        ${listDir}/thirdparty/decnumber
        ${listDir}/thirdparty/inteldfp
        ${listDir}/thirdparty/pcre2
    )
endfunction()
