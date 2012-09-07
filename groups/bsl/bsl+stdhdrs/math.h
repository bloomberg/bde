/* math.h                                                            -*-C-*- */
#ifndef INCLUDED_NATIVE_C_MATH
#define INCLUDED_NATIVE_C_MATH

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
// Note that 'math.h' is meant for multiple inclusion on ibm - therefore only
// the ident is protected by the guard.
*/

#endif  /* INCLUDED_NATIVE_C_MATH */

// <math.h> header on Sun defines 'struct exception' which interferes with
// 'std::exception'.  RW library has a workaround for this, but STLPort
// doesn't.
#if defined(BSLS_PLATFORM__CMP_SUN) && defined(BDE_BUILD_TARGET_STLPORT)
#   define exception math_exception
#endif

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus)

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
                                        (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <math.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(math.h)
#   endif

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bsl_stdhdrs_prologue.h>
#   endif

#   ifndef BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_EPILOGUE_RUN_BY_c_math
#   endif

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
                                        (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <math.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(math.h)
#   endif

// This native header does not define any symbols in namespace 'std' to import,
// so the following include is not necessary:
// #include <bsl_c_math.h>

#   ifdef BSL_STDHDRS_EPILOGUE_RUN_BY_c_math
#   undef BSL_STDHDRS_EPILOGUE_RUN_BY_c_math
#   include <bsl_stdhdrs_epilogue.h>
#   endif

#endif  /* BSL_OVERRIDES_STD */

#if defined(BSLS_PLATFORM__CMP_SUN) && defined(BDE_BUILD_TARGET_STLPORT)
#   undef exception
#endif

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
