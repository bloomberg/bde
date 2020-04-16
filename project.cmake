include(legacy/wafstyleout)

include(bde_project)
include(bde_utils)

include(layers/package_libs)
include(layers/ufid)
include(layers/install_pkg_config)
include(layers/install_cmake_config)

function(bdeproj_package_setup_interface package)
    package_setup_interface_base(bdeproj_package_setup_interface ${ARGV})

    bde_struct_get_field(pkgName ${package} NAME)

    #[[
    set(noOverrideStdPackages bslstl bslstp)
    if(NOT ${pkgName} IN_LIST noOverrideStdPackages)
        bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
        bde_interface_target_compile_definitions(
            ${interfaceTarget} PRIVATE BSL_OVERRIDES_STD
        )
    endif()
    ]]
endfunction()

bde_prefixed_override(bdeproj project_process_uors)
function(bdeproj_project_process_uors proj listDir)
    bde_assert_no_extra_args()

    bde_record_overrides()
    bde_prefixed_override(bdeproj package_setup_interface)

    bde_project_process_package_groups(
        ${proj}
        ${listDir}/groups/bal
        ${listDir}/groups/bbl
        ${listDir}/groups/bdl
        ${listDir}/groups/bsl
    )

    bde_remove_recorded_overrides()

    bde_project_process_standalone_packages(
        ${proj}
        ${listDir}/thirdparty/inteldfp
        ${listDir}/thirdparty/pcre2
        ${listDir}/standalones/s_baltst
    )
endfunction()
