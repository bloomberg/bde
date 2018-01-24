include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)
include(bde_uor)

function(process outInfoTarget listFile)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET decnumber)

    bde_struct_create(BDE_PACKAGE_TYPE ${TARGET})

    set(headers
        ${rootDir}/decContext.h
        ${rootDir}/decDouble.h
        ${rootDir}/decDPD.h
        ${rootDir}/decimal32.h
        ${rootDir}/decimal64.h
        ${rootDir}/decimal128.h
        ${rootDir}/decNumber.h
        ${rootDir}/decNumberLocal.h
        ${rootDir}/decPacked.h
        ${rootDir}/decQuad.h
        ${rootDir}/decSingle.h
    )
    bde_struct_set_field(${TARGET} HEADERS ${headers})

    bde_struct_set_field(${TARGET} SOURCES
        ${rootDir}/decContext.c
        ${rootDir}/decDouble.c
        ${rootDir}/decimal32.c
        ${rootDir}/decimal64.c
        ${rootDir}/decimal128.c
        ${rootDir}/decNumber.c
        ${rootDir}/decPacked.c
        ${rootDir}/decQuad.c
        ${rootDir}/decSingle.c
    )

    bde_add_interface_target(${TARGET})
    bde_struct_set_field(${TARGET} INTERFACE_TARGET ${TARGET})

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
        bde_interface_target_compile_options(${TARGET} PRIVATE "-std=gnu89")
    endif()

    bde_interface_target_compile_options(
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
    include (TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)

    bde_interface_target_compile_definitions(
        ${TARGET}
        PRIVATE
            "DECLITEND=$<NOT:$<BOOL:${IS_BIG_ENDIAN}>>"
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
