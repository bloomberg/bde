include(bde_package)
include(bde_override_std)

function(process retPackage listFile uorName)
    bde_assert_no_extra_args()
    bde_process_package(package ${listFile} ${uorName})

    bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
    bde_override_std(${interfaceTarget})

    bde_return(${package})
endfunction()