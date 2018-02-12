include(bde_interface_target)
include(bde_override_std)
include(bde_struct)
include(bde_utils)

function(process_package retPackage listFile uorName)
    bde_assert_no_extra_args()

    get_filename_component(packageName ${listFile} NAME_WE)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    # Sources and headers
    bde_utils_add_meta_file("${listDir}/${packageName}.pub" headers TRACK)
    bde_utils_list_template_substitute(headers "%" "${rootDir}/%" ${headers})

    # Dependencies
    bde_utils_add_meta_file("${listDir}/${packageName}.dep" dependencies TRACK)

    # Include directories
    bde_add_interface_target(${packageName})
    bde_interface_target_include_directories(
        ${packageName}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
            $<INSTALL_INTERFACE:"include/stlport">
    )
    bde_override_std(${packageName})

    # Custom install of all headers in 'stlport'.
    # Note that this code correctly handles install of subdirs.
    foreach(file IN LISTS headers)
        get_filename_component(dir ${file} DIRECTORY)
        string(CONCAT fullFile ${rootDir} "/" ${file})
        install(
            FILES ${fullFile}
            DESTINATION "include/stlport/${dir}"
            COMPONENT "${uorName}-headers"
        )
    endforeach()

    bde_struct_create(
        package
        BDE_PACKAGE_TYPE
        NAME ${packageName}
        HEADERS "${headers}"
        DEPENDS "${dependencies}"
        INTERFACE_TARGET ${packageName}
    )

    bde_return(${package})
endfunction()
