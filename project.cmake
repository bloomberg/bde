include(bde_project)
include(bde_utils)

function(process_project retProject listDir)
    bde_assert_no_extra_args()

    bde_struct_create(proj BDE_PROJECT_TYPE NAME bde.p)
        # postfix needed because of clash with bde-classic group name

    set(CMAKE_MODULE_PATH "${listDir}/cmake" ${CMAKE_MODULE_PATH})
        # Override defaults

    bde_process_project_uors(
        ${proj}
        COMMON_INTERFACE_TARGETS
            bde_ufid_flags
        PACKAGE_GROUPS
            ${listDir}/groups/bal
            ${listDir}/groups/bbl
            ${listDir}/groups/bdl
            ${listDir}/groups/bsl
            ${listDir}/groups/btl
    )

    list(REMOVE_AT CMAKE_MODULE_PATH 0)
       # Restore defaults

    # Note, that thirdparty libraries do not use COMMON_INTERFACE_TARGETS
    bde_process_project_uors(
        ${proj}
        STANDALONE_PACKAGES
            ${listDir}/thirdparty/decnumber
            ${listDir}/thirdparty/inteldfp
            ${listDir}/thirdparty/pcre2
    )

    bde_return(${proj})
endfunction()