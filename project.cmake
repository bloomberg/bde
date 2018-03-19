include(legacy/wafstyleout)

include(bde_project)
include(bde_utils)

include(layers/package_libs)
include(layers/ufid)
include(layers/install_pkg_config)
include(layers/install_cmake_config)

bde_prefixed_override(bdeproj project_setup_interface)
function(bdeproj_project_setup_interface proj)
    bde_assert_no_extra_args()

    project_setup_interface_base("" ${ARGV})

    bde_struct_get_field(interfaceTarget ${proj} INTERFACE_TARGET)
    bde_interface_target_compile_definitions(
        ${interfaceTarget}
        PUBLIC
            $<$<NOT:$<BOOL:$<TARGET_PROPERTY:SUPPRESS_BSL_OVERRIDES_STD>>>:BSL_OVERRIDES_STD>
    )

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

    bde_project_process_standalone_packages(
        ${proj}
        ${listDir}/thirdparty/decnumber
        ${listDir}/thirdparty/inteldfp
        ${listDir}/thirdparty/pcre2
    )
endfunction()
