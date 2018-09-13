include(bde_struct)
include(bde_utils)

bde_prefixed_override(bslhdrs package_process_components)
function(bslhdrs_package_process_components package listFile)
    get_filename_component(packageName ${listFile} NAME_WE)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    bde_utils_add_meta_file("${listDir}/${packageName}.pub" headers TRACK)
    bde_utils_list_template_substitute(headers "%" "${rootDir}/%" ${headers})
    bde_struct_set_field(${package} HEADERS ${headers})
        # Custom header logic
endfunction()