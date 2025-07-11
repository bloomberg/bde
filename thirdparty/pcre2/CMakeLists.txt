set(target pcre2)
message(VERBOSE "Processing target \"${target}\"")

add_library(
    ${target}
    STATIC
        src/pcre2_auto_possess.c
        src/pcre2_chartables.c
        src/pcre2_chkdint.c
        src/pcre2_compile.c
        src/pcre2_compile_class.c
        src/pcre2_config.c
        src/pcre2_context.c
        src/pcre2_convert.c
        src/pcre2_dfa_match.c
        src/pcre2_error.c
        src/pcre2_extuni.c
        src/pcre2_find_bracket.c
        src/pcre2_jit_compile.c
        src/pcre2_maketables.c
        src/pcre2_match.c
        src/pcre2_match_data.c
        src/pcre2_newline.c
        src/pcre2_ord2utf.c
        src/pcre2_pattern_info.c
        src/pcre2_script_run.c
        src/pcre2_serialize.c
        src/pcre2_string_utils.c
        src/pcre2_study.c
        src/pcre2_substitute.c
        src/pcre2_substring.c
        src/pcre2_tables.c
        src/pcre2_ucd.c
        src/pcre2_valid_utf.c
        src/pcre2_xclass.c
)

target_compile_definitions(
    ${target}
    PRIVATE
        PCRE2_CODE_UNIT_WIDTH=8
        HAVE_CONFIG_H
        PCRE2_STATIC
        SLJIT_CONFIG_AUTO
        $<$<C_COMPILER_ID:GNU>:USE_REAL_MALLOC>
)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    target_compile_definitions(
        ${target}
        PRIVATE
            SUPPORT_JIT
    )
endif()

target_include_directories(
    ${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(
    ${target}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/sljit/sljit_src>
)

set(${target}_INCLUDE_FILES
    src/pcre2.h
    src/pcre2_internal.h
    src/pcre2_intmodedep.h
    src/pcre2_ucp.h
)

if(MSVC)
    target_compile_definitions(
        ${target}
        PRIVATE
            _CRT_SECURE_NO_DEPRECATE
            _CRT_SECURE_NO_WARNINGS
    )
endif()

bbs_add_target_thread_flags(${target} PRIVATE)

# This variable controls the set of headers to install
set(${target}_INCLUDE_FILES ${${target}_INCLUDE_FILES} PARENT_SCOPE)

# Set install path for headers instead of INSTALL_INTERFACE
set_target_properties(${target} PROPERTIES INCLUDES_INSTALL_PATH "include/${target}")

bbs_install_target(${target})
bbs_emit_pkg_config(${target})

set_target_properties(${target} PROPERTIES BB_UOR_PARTITION thirdparty)
bbs_emit_bde_metadata(${target})
