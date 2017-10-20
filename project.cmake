function(process_project outName list_dir)
    set(projName bde_project) # postfix needed because of clash with bde-classic group name
    set(${outName} ${projName} PARENT_SCOPE)
    bde_project(
        ${projName}
        COMMON_INTERFACE_TARGET
            bde_ufid_flags
        FILES
            ${list_dir}/thirdparty/decnumber/package/decnumber.cmake
            ${list_dir}/thirdparty/inteldfp/package/inteldfp.cmake
            ${list_dir}/thirdparty/pcre2/package/pcre2.cmake
            ${list_dir}/groups/bal/group/bal.cmake
            ${list_dir}/groups/bbl/group/bbl.cmake
            ${list_dir}/groups/bdl/group/bdl.cmake
            ${list_dir}/groups/bsl/group/bsl.cmake
            ${list_dir}/groups/btl/group/btl.cmake
    )
endfunction()