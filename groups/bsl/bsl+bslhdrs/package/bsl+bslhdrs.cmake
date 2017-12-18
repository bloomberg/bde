function(process outInfoTarget list_file uorName)
    get_filename_component(packageName ${list_file} NAME_WE)
    get_filename_component(list_dir ${list_file} DIRECTORY)
    get_filename_component(root_dir ${list_dir} DIRECTORY)

    bde_add_info_target(${packageName})
    set(${outInfoTarget} ${packageName} PARENT_SCOPE)

    # Sources and headers
    bde_utils_add_meta_file("${list_dir}/${packageName}.pub" headers_list TRACK)
    bde_list_template_substitute(headers "%" "${root_dir}/%" ${headers_list})
    bde_info_target_set_property(${packageName} HEADERS "${headers}")

    # Dependencies
    bde_utils_add_meta_file("${list_dir}/${packageName}.dep" dependencies TRACK)
    bde_info_target_set_property(${packageName} DEPENDS "${dependencies}")

    # Include directories
    bde_add_interface_target(${packageName})
    bde_info_target_set_property(${packageName} INTERFACE_TARGET ${packageName})
    bde_interface_target_include_directories(
        ${packageName}
        PUBLIC
            $<BUILD_INTERFACE:${root_dir}>
            $<INSTALL_INTERFACE:"include">
    )

    install(
        FILES ${headers}
        DESTINATION "include"
        COMPONENT "${uorName}-headers"
    )
endfunction()
