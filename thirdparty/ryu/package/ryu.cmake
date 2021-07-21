include(bde_utils)
include(bde_struct)
include(bde_uor)

bde_prefixed_override(ryu process_standalone_package)
function(ryu_process_standalone_package retUOR listFile installOpts)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET ryu)

    set(headers
        ${rootDir}/blp_common.h
        ${rootDir}/blp_ryu.h
        ${rootDir}/common.h
        ${rootDir}/d2fixed_full_table.h
        ${rootDir}/d2s_full_table.h
        ${rootDir}/d2s_intrinsics.h
        ${rootDir}/d2s_small_table.h
        ${rootDir}/digit_table.h
        ${rootDir}/f2s_full_table.h
        ${rootDir}/f2s_intrinsics.h
        ${rootDir}/ryu.h
    )

    set(sources
        ${rootDir}/blp_d2g.c
        ${rootDir}/blp_f2g.c
        ${rootDir}/d2fixed.c
        ${rootDir}/d2s.c
        ${rootDir}/f2s.c
    )

    bde_ufid_add_library(${TARGET} ${sources} ${headers})

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    endif()

    get_filename_component(
        EXTERNAL_INCLUDE_DIR
        ${rootDir} DIRECTORY
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
    )

    bde_struct_get_field(component ${installOpts} COMPONENT)
    bde_struct_get_field(includeInstallDir ${installOpts} INCLUDE_DIR)
    install(
        FILES ${headers}
        COMPONENT "${component}-headers"
        DESTINATION "${includeInstallDir}/${TARGET}"
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<INSTALL_INTERFACE:${includeInstallDir}>
    )

    # Don't create interfaces to only use our own build/usage reqiurements
    bde_struct_create(
        uor
        BDE_UOR_TYPE
        NAME "${TARGET}"
        TARGET "${TARGET}"
    )
    standalone_package_install(${uor} ${listFile} ${installOpts})

    # Meta data install
    install(
        DIRECTORY ${listDir}
        COMPONENT "${component}-meta"
        DESTINATION "share/bdemeta/thirdparty/${component}"
        EXCLUDE_FROM_ALL
    )

    bde_return(${uor})
endfunction()
