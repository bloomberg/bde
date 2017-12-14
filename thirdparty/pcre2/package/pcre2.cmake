function(process_uor outName list_file)
    get_filename_component(list_dir ${list_file} DIRECTORY)
    get_filename_component(root_dir ${list_dir} DIRECTORY)

    set(TARGET pcre2)

    include(bde_uor)
    bde_add_info_target(${TARGET})
    set(${outName} ${TARGET} PARENT_SCOPE)
    bde_info_target_set_property(${TARGET} TARGET "${TARGET}")

    set(headers
        ${root_dir}/config.h
        ${root_dir}/pcre2.h
        ${root_dir}/pcre2_internal.h
        ${root_dir}/pcre2_intmodedep.h
        ${root_dir}/pcre2_ucp.h
    )

    add_library(
        ${TARGET}
        ${root_dir}/pcre2_auto_possess.c
        ${root_dir}/pcre2_chartables.c
        ${root_dir}/pcre2_compile.c
        ${root_dir}/pcre2_config.c
        ${root_dir}/pcre2_context.c
        ${root_dir}/pcre2_dfa_match.c
        ${root_dir}/pcre2_error.c
        ${root_dir}/pcre2_jit_compile.c
        ${root_dir}/pcre2_maketables.c
        ${root_dir}/pcre2_match.c
        ${root_dir}/pcre2_match_data.c
        ${root_dir}/pcre2_newline.c
        ${root_dir}/pcre2_ord2utf.c
        ${root_dir}/pcre2_pattern_info.c
        ${root_dir}/pcre2_serialize.c
        ${root_dir}/pcre2_string_utils.c
        ${root_dir}/pcre2_study.c
        ${root_dir}/pcre2_substitute.c
        ${root_dir}/pcre2_substring.c
        ${root_dir}/pcre2_tables.c
        ${root_dir}/pcre2_ucd.c
        ${root_dir}/pcre2_valid_utf.c
        ${root_dir}/pcre2_xclass.c
        ${headers}
    )

    # Compile options and definitions.
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "PCRE2_CODE_UNIT_WIDTH=8"
            "HAVE_CONFIG_H"
            "SUPPORT_UNICODE"
    )

    # JIT support. Supported except 64-bit sparc.
    if(CMAKE_SYSTEM_NAME STREQUAL "SunOS" AND ${bde_ufid_is_64})
        message(WARNING "JIT support is disabled for " ${CMAKE_SYSTEM_NAME})
    else()
        target_compile_definitions(
            ${TARGET}
            PRIVATE
                "SUPPORT_JIT"
        )
        message(STATUS "JIT support is enabled.")
    endif()

    target_compile_options(
        ${TARGET}
        PRIVATE
            # Compiler specific compile options.
            $<$<CXX_COMPILER_ID:Clang>:
                $<IF:${bde_ufid_is_64}, -m64, -m32>
            >
            $<$<CXX_COMPILER_ID:GNU>:
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<${bde_ufid_is_mt}: -pthread>
            >
            $<$<CXX_COMPILER_ID:SunPro>:
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<${bde_ufid_is_mt}: -mt>
            >
            $<$<CXX_COMPILER_ID:XL>:
                $<${bde_ufid_is_mt}: -qthreaded>
            >
    )

    get_filename_component(
        EXTERNAL_INCLUDE_DIR
        ${root_dir} DIRECTORY
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${root_dir}>
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    install(
        TARGETS ${TARGET}
        EXPORT ${TARGET}Targets
        COMPONENT "${TARGET}"
        ARCHIVE DESTINATION ${bde_install_lib_suffix}/${bde_install_ufid}
        LIBRARY DESTINATION ${bde_install_lib_suffix}/${bde_install_ufid}
    )

    if(CMAKE_HOST_UNIX)
        # This code will create a symlink to a corresponding "ufid" build.
        # Use with care.
        set(libName "${CMAKE_STATIC_LIBRARY_PREFIX}${TARGET}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        set(symlink_val "${bde_install_ufid}/${libName}")
        set(symlink_file "\$ENV{DESTDIR}/\${CMAKE_INSTALL_PREFIX}/${bde_install_lib_suffix}/${libName}")

        install(
            CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${symlink_val} ${symlink_file})"
            COMPONENT "${TARGET}-symlinks"
            EXCLUDE_FROM_ALL
        )

        install(
            CODE "message(\" -- ${symlink_val} ${symlink_file}\")"
            COMPONENT "${TARGET}-symlinks"
            EXCLUDE_FROM_ALL
        )
    endif()

    install(
        EXPORT ${TARGET}Targets
        COMPONENT "${TARGET}"
        DESTINATION "${bde_install_lib_suffix}/${bde_install_ufid}/cmake"
    )

    set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/..")

    configure_file(
        "${root_dir}/${TARGET}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${TARGET}Config.cmake"
        @ONLY
    )

    install(
        FILES "${PROJECT_BINARY_DIR}/${TARGET}Config.cmake"
        COMPONENT "${TARGET}"
        DESTINATION "${bde_install_lib_suffix}/${bde_install_ufid}/cmake"
    )

    install(
        FILES ${headers}
        COMPONENT "${TARGET}-headers"
        DESTINATION "include/${TARGET}"
    )
endfunction()
