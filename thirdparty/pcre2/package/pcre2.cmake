include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)
include(bde_uor)

function(process outInfoTarget listFile)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET pcre2)

    bde_struct_create(BDE_PACKAGE_TYPE ${TARGET})

    set(headers
        ${rootDir}/config.h
        ${rootDir}/pcre2.h
        ${rootDir}/pcre2_internal.h
        ${rootDir}/pcre2_intmodedep.h
        ${rootDir}/pcre2_ucp.h
    )
    bde_struct_set_field(${TARGET} HEADERS ${headers})

    bde_struct_set_field(${TARGET} SOURCES
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
    )

    bde_add_interface_target(${TARGET})
    bde_struct_set_field(${TARGET} INTERFACE_TARGET ${TARGET})

    # Compile options and definitions.
    bde_interface_target_compile_definitions(
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
        bde_interface_target_compile_definitions(
            ${TARGET}
            PRIVATE
                "SUPPORT_JIT"
        )
        bde_log(VERBOSE "JIT support is enabled.")
    endif()

    bde_interface_target_compile_options(
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

    bde_interface_target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    install(
        FILES ${headers}
        COMPONENT "${TARGET}-headers"
        DESTINATION "include/${TARGET}"
    )

    set(infoTarget ${TARGET}-standalone)
    bde_prepare_uor(${TARGET} ${infoTarget} "" LIBRARY)
    bde_project_add_uor(${infoTarget} ${TARGET})
    set(${outInfoTarget} ${infoTarget} PARENT_SCOPE)
endfunction()
