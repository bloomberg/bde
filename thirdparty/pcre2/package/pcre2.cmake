include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)
include(bde_uor)

function(process_uor retUOR listFile)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET pcre2)

    bde_struct_create(package BDE_PACKAGE_TYPE NAME ${TARGET})

    set(headers
        ${rootDir}/config.h
        ${rootDir}/pcre2.h
        ${rootDir}/pcre2_internal.h
        ${rootDir}/pcre2_intmodedep.h
        ${rootDir}/pcre2_ucp.h
    )
    bde_struct_set_field(${package} HEADERS ${headers})

    bde_struct_set_field(${package} SOURCES
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
    bde_struct_set_field(${package} INTERFACE_TARGET ${TARGET})

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        bde_interface_target_set_property(
            ${TARGET}
                POSITION_INDEPENDENT_CODE PUBLIC 1
        )
    endif()

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
            $<$<C_COMPILER_ID:Clang>:
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:GNU>:
                -fdiagnostics-show-option
                -fno-strict-aliasing
                -std=gnu99
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:SunPro>:
                -temp=/bb/data/tmp
                $<IF:${bde_ufid_is_64}, -m64, -m32>
            >
            $<$<C_COMPILER_ID:XL>:
                $<IF:${bde_ufid_is_64}, -q64, -q32>
            >
    )

    bde_interface_target_compile_definitions(
        ${TARGET}
        PRIVATE
            $<${bde_ufid_is_mt}: _REENTRANT>
            $<$<C_COMPILER_ID:GNU>:
                USE_REAL_MALLOC
            >
    )

    get_filename_component(
        EXTERNAL_INCLUDE_DIR
        ${rootDir} DIRECTORY
    )

    bde_interface_target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    install(
        FILES ${headers}
        COMPONENT "${TARGET}-headers"
        DESTINATION "include/${TARGET}"
    )

    add_library(${TARGET} "")
    bde_struct_create(
        uor
        BDE_UOR_TYPE
        NAME "${TARGET}"
        TARGET "${TARGET}"
    )
    bde_project_add_uor(${uor} ${package})

    bde_return(${uor})
endfunction()
