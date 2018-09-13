include(bde_interface_target)
include(bde_package)
include(bde_struct)

bde_prefixed_override(bdlde process_package)
function(bdlde_process_package retPackage)
    process_package_base("" package ${ARGN})
    bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
    bde_interface_target_compile_options(
        ${interfaceTarget}
        PRIVATE
            $<$<PLATFORM_ID:Darwin>:-msse4.2>
            $<$<PLATFORM_ID:Linux>:-msse4.2>
            $<$<PLATFORM_ID:SunOs>:-xarch=sparc4>
    )
    bde_return(${package})
endfunction()
