function(process_uor outName list_file)
    get_filename_component(list_dir ${list_file} DIRECTORY)
    get_filename_component(root_dir ${list_dir} DIRECTORY)

    set(TARGET inteldfp)

    bde_add_info_target(${TARGET})
    set(${outName} ${TARGET} PARENT_SCOPE)
    bde_info_target_set_property(${TARGET} TARGET "${TARGET}")

    # Standard CMake modules.
    include (TestBigEndian)

    set(headers
        ${root_dir}/LIBRARY/src/inteldfp/bid128_2_str.h
        ${root_dir}/LIBRARY/src/inteldfp/bid128_2_str_macros.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_b2d.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_conf.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_div_macros.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_functions.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_gcc_intrinsics.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_inline_add.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_internal.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_sqrt_macros.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_strtod.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_trans.h
        ${root_dir}/LIBRARY/src/inteldfp/bid_wrap_names.h
        ${root_dir}/LIBRARY/src/inteldfp/dfp754.h
    )

    add_library(
        ${TARGET}
        ${root_dir}/LIBRARY/src/inteldfp/bid64_acos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_acosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_asin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_asinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_atan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_atan2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_atanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_cbrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_cos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_cosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_erf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_erfc.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_exp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_exp10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_exp2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_expm1.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_hypot.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_lgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_log.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_log10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_log1p.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_log2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_pow.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_sin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_sinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_tan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_tanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_tgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_acos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_acosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_asin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_asinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_atan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_atan2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_atanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_cbrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_cos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_cosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_erf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_erfc.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_exp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_exp10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_exp2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_expm1.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_hypot.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_lgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_log.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_log10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_log1p.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_log2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_pow.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_sin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_sinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_tan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_tanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_tgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_add.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_compare.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_div.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_fdimd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_fma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_fmod.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_frexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_ldexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_llrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_logb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_logbd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_lrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_lround.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_minmax.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_modf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_mul.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_nearbyintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_next.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_nexttowardd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_noncomp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_quantexpd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_quantize.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_rem.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_round_integral.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_scalb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_scalbl.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_sqrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_string.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_int16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_int32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_int64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_int8.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_uint16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_uint32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_uint64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_uint8.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_add.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_compare.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_div.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_fdimd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_fma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_fmod.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_frexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_ldexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_llrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_logb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_logbd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_lrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_lround.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_minmax.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_modf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_mul.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_nearbyintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_next.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_nexttowardd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_noncomp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_quantexpd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_quantize.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_rem.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_round_integral.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_scalb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_scalbl.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_sqrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_string.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_int16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_int32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_int64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_int8.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_uint16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_uint32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_uint64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_uint8.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_add.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_compare.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_div.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_fdimd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_fma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_fmod.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_frexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_ldexp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_llrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_logb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_logbd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_lrintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_lround.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_minmax.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_modf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_mul.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_nearbyintd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_next.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_nexttowardd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_noncomp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_quantexpd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_quantize.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_rem.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_round_integral.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_scalb.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_scalbl.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_sqrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_string.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_int16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_int32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_int64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_int8.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_uint16.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_uint32.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_uint64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_to_uint8.c
        ${root_dir}/LIBRARY/src/inteldfp/strtod32.c
        ${root_dir}/LIBRARY/src/inteldfp/strtod64.c
        ${root_dir}/LIBRARY/src/inteldfp/strtod128.c
        ${root_dir}/LIBRARY/src/inteldfp/wcstod32.c
        ${root_dir}/LIBRARY/src/inteldfp/wcstod64.c
        ${root_dir}/LIBRARY/src/inteldfp/wcstod128.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_acos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_acosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_asin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_asinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_atan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_atan2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_atanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_cbrt.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_cos.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_cosh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_erf.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_erfc.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_exp.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_exp10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_exp2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_expm1.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_hypot.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_lgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_log.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_log10.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_log1p.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_log2.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_pow.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_sin.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_sinh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_tan.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_tanh.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_tgamma.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_sub.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_bid128.c
        ${root_dir}/LIBRARY/src/inteldfp/bid32_to_bid64.c
        ${root_dir}/LIBRARY/src/inteldfp/bid64_to_bid128.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128_2_str_tables.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_binarydecimal.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_convert_data.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_decimal_data.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_decimal_globals.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_dpd.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_feclearexcept.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_fegetexceptflag.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_feraiseexcept.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_fesetexceptflag.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_fetestexcept.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_flag_operations.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_from_int.c
        ${root_dir}/LIBRARY/src/inteldfp/bid_round.c
        ${root_dir}/LIBRARY/src/inteldfp/bid128.c
        ${root_dir}/LIBRARY/float128/dpml_ux_bid.c
        ${root_dir}/LIBRARY/float128/dpml_ux_bessel.c
        ${root_dir}/LIBRARY/float128/dpml_ux_cbrt.c
        ${root_dir}/LIBRARY/float128/dpml_ux_erf.c
        ${root_dir}/LIBRARY/float128/dpml_ux_exp.c
        ${root_dir}/LIBRARY/float128/dpml_ux_int.c
        ${root_dir}/LIBRARY/float128/dpml_ux_inv_hyper.c
        ${root_dir}/LIBRARY/float128/dpml_ux_inv_trig.c
        ${root_dir}/LIBRARY/float128/dpml_ux_lgamma.c
        ${root_dir}/LIBRARY/float128/dpml_ux_log.c
        ${root_dir}/LIBRARY/float128/dpml_ux_mod.c
        ${root_dir}/LIBRARY/float128/dpml_ux_powi.c
        ${root_dir}/LIBRARY/float128/dpml_ux_pow.c
        ${root_dir}/LIBRARY/float128/dpml_ux_sqrt.c
        ${root_dir}/LIBRARY/float128/dpml_ux_trig.c
        ${root_dir}/LIBRARY/float128/dpml_ux_ops.c
        ${root_dir}/LIBRARY/float128/dpml_ux_ops_64.c
        ${root_dir}/LIBRARY/float128/dpml_four_over_pi.c
        ${root_dir}/LIBRARY/float128/dpml_exception.c
        ${root_dir}/LIBRARY/float128/sqrt_tab_t.c
        ${root_dir}/LIBRARY/float128/architecture.h
        ${root_dir}/LIBRARY/float128/assert.h
        ${root_dir}/LIBRARY/float128/build.h
        ${root_dir}/LIBRARY/float128/compiler.h
        ${root_dir}/LIBRARY/float128/dpml_acosh_t.h
        ${root_dir}/LIBRARY/float128/dpml_asinh_t.h
        ${root_dir}/LIBRARY/float128/dpml_bessel_x.h
        ${root_dir}/LIBRARY/float128/dpml_bid_x.h
        ${root_dir}/LIBRARY/float128/dpml_cbrt_x.h
        ${root_dir}/LIBRARY/float128/dpml_cons_x.h
        ${root_dir}/LIBRARY/float128/dpml_erf_t.h
        ${root_dir}/LIBRARY/float128/dpml_erf_x.h
        ${root_dir}/LIBRARY/float128/dpml_error_codes.h
        ${root_dir}/LIBRARY/float128/dpml_error_codes_enum.h
        ${root_dir}/LIBRARY/float128/dpml_exception.h
        ${root_dir}/LIBRARY/float128/dpml_exp_x.h
        ${root_dir}/LIBRARY/float128/dpml_function_info.h
        ${root_dir}/LIBRARY/float128/dpml_globals.h
        ${root_dir}/LIBRARY/float128/dpml_int_x.h
        ${root_dir}/LIBRARY/float128/dpml_inv_hyper_x.h
        ${root_dir}/LIBRARY/float128/dpml_inv_trig_x.h
        ${root_dir}/LIBRARY/float128/dpml_lgamma_t.h
        ${root_dir}/LIBRARY/float128/dpml_lgamma_x.h
        ${root_dir}/LIBRARY/float128/dpml_log2_t.h
        ${root_dir}/LIBRARY/float128/dpml_log_t.h
        ${root_dir}/LIBRARY/float128/dpml_log_x.h
        ${root_dir}/LIBRARY/float128/dpml_mod_x.h
        ${root_dir}/LIBRARY/float128/dpml_names.h
        ${root_dir}/LIBRARY/float128/dpml_pow.h
        ${root_dir}/LIBRARY/float128/dpml_pow_x.h
        ${root_dir}/LIBRARY/float128/dpml_powi_x.h
        ${root_dir}/LIBRARY/float128/dpml_private.h
        ${root_dir}/LIBRARY/float128/dpml_rdx_x.h
        ${root_dir}/LIBRARY/float128/dpml_special_exp.h
        ${root_dir}/LIBRARY/float128/dpml_sqrt_x.h
        ${root_dir}/LIBRARY/float128/dpml_trig_x.h
        ${root_dir}/LIBRARY/float128/dpml_ux.h
        ${root_dir}/LIBRARY/float128/dpml_ux_32_64.h
        ${root_dir}/LIBRARY/float128/dpml_ux_alpha_macros.h
        ${root_dir}/LIBRARY/float128/endian.h
        ${root_dir}/LIBRARY/float128/f_format.h
        ${root_dir}/LIBRARY/float128/i_format.h
        ${root_dir}/LIBRARY/float128/ix86_macros.h
        ${root_dir}/LIBRARY/float128/mphoc_functions.h
        ${root_dir}/LIBRARY/float128/mphoc_macros.h
        ${root_dir}/LIBRARY/float128/mtc_macros.h
        ${root_dir}/LIBRARY/float128/op_system.h
        ${root_dir}/LIBRARY/float128/poly_macros.h
        ${root_dir}/LIBRARY/float128/sqrt_macros.h
        ${headers}
    )

    # Common compile definitions.
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "DECIMAL_CALL_BY_REFERENCE=0"
            "DECIMAL_GLOBAL_ROUNDING=1"
            "DECIMAL_GLOBAL_EXCEPTION_FLAGS=0"
    )

    include(bde_ufid)

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
            "BID_BIG_ENDIAN=$<BOOL:${IS_BIG_ENDIAN}>"
    )

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:
                "LINUX"
                "efi2"
            >
            $<$<CXX_COMPILER_ID:GNU>:
                "LINUX"
                "efi2"
            >
            $<$<CXX_COMPILER_ID:MSVC>:
                "WINNT"
                "WINDOWS"
                "WNT"
                $<${CMAKE_CL_64}:
                    "ia32"
                >
            >
            $<$<CXX_COMPILER_ID:SunPro>:
                "SUNOS"
                "LINUX"
                "efi2"
                "__linux"
                "__float80=double"
                "BID_THREAD="
            >
            $<$<CXX_COMPILER_ID:XL>:
                "LINUX"
                "efi2"
                "__linux"
                "__QNX__"
                "__thread="
            >
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${root_dir}/LIBRARY/src>
            $<BUILD_INTERFACE:${root_dir}>
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
