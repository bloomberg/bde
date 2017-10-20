function(process_uor outName list_file)
    get_filename_component(list_dir ${list_file} DIRECTORY)
    get_filename_component(root_dir ${list_dir} DIRECTORY)

    set(TARGET decnumber)

    bde_add_info_target(${TARGET})
    set(${outName} ${TARGET} PARENT_SCOPE)
    bde_info_target_set_property(${TARGET} TARGET "${TARGET}")
    
    # Standard CMake modules.
    include (TestBigEndian)

    set(headers
        ${root_dir}/decContext.h
        ${root_dir}/decDouble.h
        ${root_dir}/decDPD.h
        ${root_dir}/decimal32.h
        ${root_dir}/decimal64.h
        ${root_dir}/decimal128.h
        ${root_dir}/decNumber.h
        ${root_dir}/decNumberLocal.h
        ${root_dir}/decPacked.h
        ${root_dir}/decQuad.h
        ${root_dir}/decSingle.h
    )

    add_library(
        ${TARGET}
        ${root_dir}/decContext.c
        ${root_dir}/decDouble.c
        ${root_dir}/decimal32.c
        ${root_dir}/decimal64.c
        ${root_dir}/decimal128.c
        ${root_dir}/decNumber.c
        ${root_dir}/decPacked.c
        ${root_dir}/decQuad.c
        ${root_dir}/decSingle.c
        ${headers}
    )

    include(bde_ufid)
    #bde_set_ufid_flags(${TARGET})

    # As reported here: https://github.com/bloomberg/bde/pull/148
    #
    # The C files in bdl+decnumber must be built in -std=gnu89 mode with
    # Clang to work around a Clang bug when handling GNU-style C89 inlines
    # in C99 compilation mode. This bug is documented in the upstream LLVM
    # bug-tracker here: http://llvm.org/bugs/show_bug.cgi?id=5960
    #
    # Without this change clang/linux builds will fail with multiply defined
    # symbols.
    if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${TARGET} PRIVATE "-std=gnu89")
    endif()

    target_compile_options(
        ${TARGET}
        PRIVATE
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

    # Detecting platform endianess.
    test_big_endian(IS_BIG_ENDIAN)

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "DECLITEND=$<NOT:$<BOOL:${IS_BIG_ENDIAN}>>"
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
