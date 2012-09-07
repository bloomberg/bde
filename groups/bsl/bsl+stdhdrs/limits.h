/* limits.h                                                          -*-C-*- */
#ifndef INCLUDED_NATIVE_C_LIMITS
#define INCLUDED_NATIVE_C_LIMITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

/*
//@PURPOSE: Provide functionality of the corresponding C++ Standard header
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Pablo Halpern (phalpern), Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide functionality of the corresponding C++ standard header.
// This file includes the compiler provided native standard header.  In
// addition, in 'bde-stl' mode (used by Bloomberg managed code, see
// 'bsl+stdhdrs.txt' for more information) include the corresponding header in
// 'bsl+bslhdrs' as well as 'bsl_stdhdrs_prologue.h' and
// 'bsl_stdhdrs_epilogue.h'.  This includes the respective 'bsl' types and
// places them in the 'std' namespace.
*/

#ifndef INCLUDED_BSLS_CODEBASE
#include <bsls_codebase.h>
#endif

/*
// Note that 'limits.h' is meant for multiple inclusion on linux - therefore
// only the ident is protected by the guard.
*/

#if !defined(INCLUDED_BSLS_PLATFORM) && defined(__cplusplus)
#  include <bsls_platform.h>
#else

// Workaround to detect the GCC compiler version, until bsls_platform.h is
// fixed to compile by a C compiler.
#if !defined(__cplusplus)
    #if defined (__GNUC__)
        #define BSLS_PLATFORM__CMP_GNU 1
        #if defined(__GNU_PATCHLEVEL__)
            #define BSLS_PLATFORM__CMP_VER_MAJOR (__GNUC__ * 10000 \
                        + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
        #else
            #define BSLS_PLATFORM__CMP_VER_MAJOR (__GNUC__ * 10000 \
                        + __GNUC_MINOR__ * 100)
        #endif
    #endif
#endif

#endif

#endif  /* INCLUDED_NATIVE_C_LIMITS */

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus)

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
       (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <limits.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(limits.h)
#   endif

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bsl_stdhdrs_prologue.h>
#   endif

#   ifndef BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_EPILOGUE_RUN_BY_c_limits
#   endif

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
       (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <limits.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(limits.h)
#   endif

// This native header does not define any symbols in namespace 'std' to import,
// so the following include is not necessary:
// #include <bsl_c_limits.h>

#   ifdef BSL_STDHDRS_EPILOGUE_RUN_BY_c_limits
#   undef BSL_STDHDRS_EPILOGUE_RUN_BY_c_limits
#   include <bsl_stdhdrs_epilogue.h>
#   endif

#endif  /* BSL_OVERRIDES_STD */

/*
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
*/
