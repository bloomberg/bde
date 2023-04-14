set(target ryu)
message(VERBOSE "Processing target \"${target}\"")

add_library(${target} STATIC
    blp_d2cpp.c
    blp_f2cpp.c
    d2fixed.c
    d2s.c
    f2s.c
)

set(${target}_INCLUDE_FILES
    blp_common.h
    blp_common32.h
    blp_common64.h
    blp_ryu.h
    common.h
    d2fixed_full_table.h
    d2s_full_table.h
    d2s_intrinsics.h
    d2s_small_table.h
    digit_table.h
    f2s_full_table.h
    f2s_intrinsics.h
    ryu.h
)

target_include_directories(
    ${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include/${target}>
)

# This variable controls the set of headers to install
set(${target}_INCLUDE_FILES ${${target}_INCLUDE_FILES} PARENT_SCOPE)

bbs_add_target_thread_flags(${target} PRIVATE)

bbs_install_target(${target})
bbs_emit_pkg_config(${target})

set_target_properties(${target} PROPERTIES BB_UOR_PARTITION thirdparty)
set_target_properties(${target} PROPERTIES BB_UOR_IS_GROUP FALSE)
bbs_emit_bde_metadata(${target})
