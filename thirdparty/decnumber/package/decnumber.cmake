include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_interface_target)
include(bde_standalone)

bde_prefixed_override(decnumber process_standalone_package)
function(decnumber_process_standalone_package retUOR listFile installOpts)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET decnumber)

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

    set(sources
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
    bde_ufid_add_library(${TARGET} ${sources} ${headers})

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE 1)
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
    target_compile_options(
        ${TARGET}
        PRIVATE
            $<$<C_COMPILER_ID:AppleClang>:
                 -std=gnu89
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
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
                -temp=/bb/data/tmp
                $<IF:${bde_ufid_is_64}, -m64, -m32>
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -xcode=pic32>
            >
            $<$<C_COMPILER_ID:XL>:
                $<IF:${bde_ufid_is_64}, -q64, -q32>
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -qpic>
                $<${bde_ufid_is_mt}: -qthreaded>
            >
    )

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            USE_REAL_MALLOC
            $<${bde_ufid_is_mt}: _REENTRANT>
    )

    # Detecting platform endianess.
    include (TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "DECLITEND=$<NOT:$<BOOL:${IS_BIG_ENDIAN}>>"
    )

    get_filename_component(
        EXTERNAL_INCLUDE_DIR
        ${rootDir} DIRECTORY
    )


    bde_struct_get_field(component ${installOpts} COMPONENT)
    bde_struct_get_field(includeInstallDir ${installOpts} INCLUDE_DIR)
    install(
        FILES ${headers}
        COMPONENT "${component}-headers"
        DESTINATION "${includeInstallDir}/${TARGET}"
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<INSTALL_INTERFACE:${includeInstallDir}>
    )

    # Don't create interfaces to only use our own build/usage reqiurements
    bde_struct_create(
        uor
        BDE_UOR_TYPE
        NAME "${TARGET}"
        TARGET "${TARGET}"
    )
    standalone_package_install(${uor} ${listFile} ${installOpts})

    bde_return(${uor})
endfunction()
