include(bde_interface_target)
include(bde_package)
include(bde_struct)
include(bde_utils)

include(bde_struct)
include(bde_utils)

bde_prefixed_override(stdhdrs package_process_components)
function(stdhdrs_package_process_components package listFile)
    get_filename_component(packageName ${listFile} NAME_WE)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    bde_utils_add_meta_file("${listDir}/${packageName}.pub" headers TRACK)
    bde_utils_list_template_substitute(headers "%" "${rootDir}/%" ${headers})
    bde_struct_set_field(${package} HEADERS ${headers})
        # Custom header logic
endfunction()

bde_prefixed_override(stdhdrs package_setup_interface)
function(stdhdrs_package_setup_interface package listFile)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    # Include directories
    # Important: the '/' before '${rootDir}' forces Ninja generator to
    # produce the absolute path. This is critical to use Ninja generator
    # with SunOS compilers.
    bde_struct_get_field(packageInterface ${package} INTERFACE_TARGET)

    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "SunOS")
        bde_interface_target_include_directories(
            ${packageInterface}
            INTERFACE
                $<BUILD_INTERFACE:/${rootDir}>
        )
    else()
        bde_interface_target_include_directories(
            ${packageInterface}
            INTERFACE
                $<BUILD_INTERFACE:${rootDir}>
        )
    endif()
endfunction()

bde_prefixed_override(stdhdrs package_install)
function(stdhdrs_package_install package listFile installOpts)
    bde_struct_get_field(headers ${package} HEADERS)

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    bde_struct_get_field(component ${installOpts} COMPONENT)
    bde_struct_get_field(exportSet ${installOpts} EXPORT_SET)
    bde_struct_get_field(includeDir ${installOpts} INCLUDE_DIR)

    # Custom install of all headers in 'stlport'.
    # Note that this code correctly handles install of subdirs.
    foreach(header IN LISTS headers)
        get_filename_component(fullDir ${header} DIRECTORY)
        string(REPLACE ${rootDir} "" dir ${fullDir})
        install(
            FILES ${header}
            DESTINATION "${includeDir}/stlport/${dir}"
            COMPONENT ${component}-headers
        )
    endforeach()

    # CMake config
    bde_struct_get_field(packageInterface ${package} INTERFACE_TARGET)
    bde_interface_target_include_directories(
        ${packageInterface}
        INTERFACE
            $<INSTALL_INTERFACE:${includeDir}/stlport>
    )

    bde_install_interface_target(
        ${packageInterface}
        EXPORT ${exportSet}InterfaceTargets
    )
endfunction()
