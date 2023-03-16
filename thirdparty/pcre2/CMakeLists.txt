set(target pcre2)
message(VERBOSE "Processing target \"${target}\"")

add_library(
    ${target}
    STATIC
        pcre2_auto_possess.c
        pcre2_chartables.c
        pcre2_compile.c
        pcre2_config.c
        pcre2_context.c
        pcre2_convert.c
        pcre2_dfa_match.c
        pcre2_error.c
        pcre2_extuni.c
        pcre2_find_bracket.c
        pcre2_jit_compile.c
        pcre2_maketables.c
        pcre2_match.c
        pcre2_match_data.c
        pcre2_newline.c
        pcre2_ord2utf.c
        pcre2_pattern_info.c
        pcre2_serialize.c
        pcre2_script_run.c
        pcre2_string_utils.c
        pcre2_study.c
        pcre2_substitute.c
        pcre2_substring.c
        pcre2_tables.c
        pcre2_ucd.c
        pcre2_valid_utf.c
        pcre2_xclass.c
)

target_compile_definitions(
    ${target}
    PRIVATE
        PCRE2_CODE_UNIT_WIDTH=8
        HAVE_CONFIG_H
        SUPPORT_UNICODE
        $<$<C_COMPILER_ID:GNU>:USE_REAL_MALLOC>
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    # 64 bits
    target_compile_definitions(
        ${target}
        PRIVATE
            SUPPORT_JIT
    )
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    target_compile_definitions(
        ${target}
        PRIVATE
            SUPPORT_JIT
    )
endif()

target_include_directories(
    ${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include/${target}>)

set(${target}_INCLUDE_FILES
    pcre2.h
    pcre2_internal.h
    pcre2_intmodedep.h
    pcre2_ucp.h
)

# This variable controls the set of headers to install
set(${target}_INCLUDE_FILES ${${target}_INCLUDE_FILES} PARENT_SCOPE)

bbs_add_target_thread_flags(${target} PRIVATE)

bbs_install_target(${target})
bbs_emit_pkg_config(${target})

set_target_properties(${target} PROPERTIES BB_UOR_PARTITION thirdparty)
bbs_emit_bde_metadata(${target})
