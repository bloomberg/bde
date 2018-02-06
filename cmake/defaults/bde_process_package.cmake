include(bde_package)
include(bde_override_std)

function(process outInfoTarget listFile uorName)
    bde_assert_no_extra_args()
    bde_process_package(infoTarget ${listFile} ${uorName})

    bde_struct_get_field(interfaceTarget ${infoTarget} INTERFACE_TARGET)
    bde_override_std(${interfaceTarget})

    set(${outInfoTarget} ${infoTarget} PARENT_SCOPE)
endfunction()