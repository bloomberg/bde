function(process outInfoTarget list_dir)
    set(projName bde.p)
    bde_struct_create(BDE_PROJECT_TYPE ${projName})
        # postfix needed because of clash with bde-classic group name

    bde_project_process_uors(
        ${projName}
        COMMON_INTERFACE_TARGET
            bde_ufid_flags
        PACKAGE_GROUPS
            ${list_dir}/groups/bal
            ${list_dir}/groups/bbl
            ${list_dir}/groups/bdl
            ${list_dir}/groups/bsl
            ${list_dir}/groups/btl
    )

    # Note, that thirdparty libraries do not use COMMON_INTERFACE_TARGET
    bde_project_process_uors(
        ${projName}
        STANDALONE_PACKAGES
            ${list_dir}/thirdparty/decnumber
            ${list_dir}/thirdparty/inteldfp
            ${list_dir}/thirdparty/pcre2
    )
    set(${outInfoTarget} ${projName} PARENT_SCOPE)
endfunction()