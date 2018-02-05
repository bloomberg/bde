include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)
include(bde_uor)

function(process retUOR listFile)
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

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        bde_interface_target_set_property(
            ${TARGET}
                POSITION_INDEPENDENT_CODE PUBLIC 1
        )
    endif()

    # As reported here: https://github.com/bloomberg/bde/pull/148
    #
    # The C files in bdl+decnumber must be built in -std=gnu89 mode with
    # Clang to work around a Clang bug when handling GNU-style C89 inlines
    # in C99 compilation mode. This bug is documented in the upstream LLVM
    # bug-tracker here: http://llvm.org/bugs/show_bug.cgi?id=5960
    #
    # Without this change clang/linux builds will fail with multiply defined
    # symbols.
    bde_interface_target_compile_options(
        ${TARGET}
        PRIVATE
            $<$<C_COMPILER_ID:Clang>:
                 -std=gnu89
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
                -std=gnu99
                $<IF:${bde_ufid_is_64}, -m64, -m32>
            >
            $<$<C_COMPILER_ID:XL>:
                -std=gnu99
            >
    )

    bde_interface_target_compile_definitions(
        ${TARGET}
        PRIVATE
            USE_REAL_MALLOC
            $<${bde_ufid_is_mt}: _REENTRANT>
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
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    install(
        FILES ${headers}
        COMPONENT "${TARGET}-headers"
        DESTINATION "include/${TARGET}"
    )

    set(uor ${TARGET}-standalone)
    bde_prepare_uor(${TARGET} ${uor} "" LIBRARY)
    bde_project_add_uor(${uor} ${TARGET})

    bde_return(${uor})
endfunction()
