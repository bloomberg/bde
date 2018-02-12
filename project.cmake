include(bde_project)
include(bde_utils)

function(process_project retProject listDir)
    bde_assert_no_extra_args()

    bde_struct_create(proj BDE_PROJECT_TYPE NAME bde)

    bde_add_interface_target(bsl_override_std)
    bde_interface_target_compile_definitions(
        bsl_override_std
            PUBLIC
                $<$<NOT:$<BOOL:$<TARGET_PROPERTY:SUPPRESS_BSL_OVERRIDES_STD>>>:BSL_OVERRIDES_STD>
    )

    bde_process_project_uors(
        ${proj}
        COMMON_INTERFACE_TARGETS
            bde_ufid_flags
            bsl_override_std
        PACKAGE_GROUPS
            ${listDir}/groups/bal
            ${listDir}/groups/bbl
            ${listDir}/groups/bdl
            ${listDir}/groups/bsl
            ${listDir}/groups/btl
    )

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