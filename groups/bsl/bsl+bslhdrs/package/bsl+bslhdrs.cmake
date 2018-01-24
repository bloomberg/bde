include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)

function(process outInfoTarget listFile uorName)
    bde_assert_no_extra_args()

    get_filename_component(packageName ${listFile} NAME_WE)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    bde_struct_create(BDE_PACKAGE_TYPE ${packageName})
    set(${outInfoTarget} ${packageName} PARENT_SCOPE)

    # Sources and headers
    bde_utils_add_meta_file("${listDir}/${packageName}.pub" headers TRACK)
    bde_utils_list_template_substitute(headers "%" "${rootDir}/%" ${headers})
    bde_struct_set_field(${packageName} HEADERS "${headers}")

    # Dependencies
    bde_utils_add_meta_file("${listDir}/${packageName}.dep" dependencies TRACK)
    bde_struct_set_field(${packageName} DEPENDS "${dependencies}")

    # Include directories
    bde_add_interface_target(${packageName})
    bde_struct_set_field(${packageName} INTERFACE_TARGET ${packageName})
    bde_interface_target_include_directories(
        ${packageName}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
            $<INSTALL_INTERFACE:"include">
    )

    install(
        FILES ${headers}
        DESTINATION "include"
        COMPONENT "${uorName}-headers"
    )
endfunction()
