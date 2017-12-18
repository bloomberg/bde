function(process outInfoTarget listFile)
    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET pcre2)

    include(bde_uor)
    bde_add_info_target(${TARGET})
    set(${outInfoTarget} ${TARGET} PARENT_SCOPE)
    bde_info_target_set_property(${TARGET} TARGET "${TARGET}")

    set(headers
        ${rootDir}/config.h
        ${rootDir}/pcre2.h
        ${rootDir}/pcre2_internal.h
        ${rootDir}/pcre2_intmodedep.h
        ${rootDir}/pcre2_ucp.h
    )

    add_library(
        ${TARGET}
        ${rootDir}/pcre2_auto_possess.c
        ${rootDir}/pcre2_chartables.c
        ${rootDir}/pcre2_compile.c
        ${rootDir}/pcre2_config.c
        ${rootDir}/pcre2_context.c
        ${rootDir}/pcre2_dfa_match.c
        ${rootDir}/pcre2_error.c
        ${rootDir}/pcre2_jit_compile.c
        ${rootDir}/pcre2_maketables.c
        ${rootDir}/pcre2_match.c
        ${rootDir}/pcre2_match_data.c
        ${rootDir}/pcre2_newline.c
        ${rootDir}/pcre2_ord2utf.c
        ${rootDir}/pcre2_pattern_info.c
        ${rootDir}/pcre2_serialize.c
        ${rootDir}/pcre2_string_utils.c
        ${rootDir}/pcre2_study.c
        ${rootDir}/pcre2_substitute.c
        ${rootDir}/pcre2_substring.c
        ${rootDir}/pcre2_tables.c
        ${rootDir}/pcre2_ucd.c
        ${rootDir}/pcre2_valid_utf.c
        ${rootDir}/pcre2_xclass.c
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
        bde_log(VERBOSE "JIT support is enabled.")
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
        ${rootDir} DIRECTORY
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
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
    endif()

    install(
        EXPORT ${TARGET}Targets
        COMPONENT "${TARGET}"
        DESTINATION "${bde_install_lib_suffix}/${bde_install_ufid}/cmake"
    )

    set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/..")

    configure_file(
        "${rootDir}/${TARGET}Config.cmake.in"
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
