function(process_project outInfoTarget list_dir)
    set(projName bde_project) # postfix needed because of clash with bde-classic group name
    set(${outInfoTarget} ${projName} PARENT_SCOPE)
    bde_project(
        ${projName}
        COMMON_INTERFACE_TARGET
            bde_ufid_flags
        PACKAGE_GROUPS
            ${list_dir}/groups/bal
            ${list_dir}/groups/bbl
            ${list_dir}/groups/bdl
            ${list_dir}/groups/bsl
            ${list_dir}/groups/btl
        STANDALONE_PACKAGES
            ${list_dir}/thirdparty/decnumber
            ${list_dir}/thirdparty/inteldfp
            ${list_dir}/thirdparty/pcre2
    )
endfunction()