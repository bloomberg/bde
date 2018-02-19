include(bde_interface_target)
include(bde_package)
include(bde_struct)
include(bde_utils)

function(process_package retPackage listFile uorName)
    bde_assert_no_extra_args()

    get_filename_component(packageName ${listFile} NAME_WE)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    # Sources and headers
    bde_utils_add_meta_file("${listDir}/${packageName}.pub" rawHeaders TRACK)
    bde_utils_list_template_substitute(headers "%" "${rootDir}/%" ${rawHeaders})

    bde_struct_create(
        package
        BDE_PACKAGE_TYPE
        NAME ${packageName}
        HEADERS "${headers}"
    )

    bde_create_package_interfaces(${package} ${listFile})

    # Include directories
    bde_struct_get_field(packageInterface ${package} INTERFACE_TARGET)
    bde_interface_target_include_directories(
        ${packageInterface}
        INTERFACE
            $<BUILD_INTERFACE:${rootDir}>
            $<INSTALL_INTERFACE:"include/stlport">
    )

    # Custom install of all headers in 'stlport'.
    # Note that this code correctly handles install of subdirs.
    foreach(file IN LISTS rawHeaders)
        get_filename_component(dir ${file} DIRECTORY)
        string(CONCAT fullFile ${rootDir} "/" ${file})
        install(
            FILES ${fullFile}
            DESTINATION "include/stlport/${dir}"
            COMPONENT "${uorName}-headers"
        )
    endforeach()

    bde_create_package_target(${package})

    bde_return(${package})
endfunction()
