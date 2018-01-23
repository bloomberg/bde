include(bde_project)
include(bde_utils)

function(process outInfoTarget listDir)
    bde_assert_no_extra_args()

    set(projName bde.p)
    bde_struct_create(BDE_PROJECT_TYPE ${projName})
        # postfix needed because of clash with bde-classic group name

    bde_process_project_uors(
        ${projName}
        COMMON_INTERFACE_TARGET
            bde_ufid_flags
        PACKAGE_GROUPS
            ${listDir}/groups/bal
            ${listDir}/groups/bbl
            ${listDir}/groups/bdl
            ${listDir}/groups/bsl
            ${listDir}/groups/btl
    )

    # Note, that thirdparty libraries do not use COMMON_INTERFACE_TARGET
    bde_process_project_uors(
        ${projName}
        STANDALONE_PACKAGES
            ${listDir}/thirdparty/decnumber
            ${listDir}/thirdparty/inteldfp
            ${listDir}/thirdparty/pcre2
    )
    set(${outInfoTarget} ${projName} PARENT_SCOPE)
endfunction()