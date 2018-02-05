include(bde_interface_target)
include(bde_utils)

function(bde_override_std interfaceTarget)
    bde_assert_no_extra_args()

    bde_interface_target_compile_definitions(
        ${interfaceTarget} PUBLIC BDE_OVERRIDES_STD
    )
endfunction()