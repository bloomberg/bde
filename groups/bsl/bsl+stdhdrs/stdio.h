/* stdio.h                                                           -*-C-*- */
#ifndef INCLUDED_NATIVE_C_STDIO
#define INCLUDED_NATIVE_C_STDIO

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
//@DESCRIPTION: Provide functionality of the corresponding C++ standard
// header.  This file includes the compiler provided native standard header.
// In addition, in 'bde-stl' mode (used by Bloomberg managed code, see
// 'bsl+stdhdrs.txt' for more information) include the corresponding header in
// 'bsl+bslhdrs' as well as 'bsl_stdhdrs_prologue.h' and
// 'bsl_stdhdrs_epilogue.h'.  This includes the respective 'bsl' types and
// places them in the 'std' namespace.
*/

/*
// Note that 'stdio.h' is meant for multiple inclusion on linux - therefore
// only the ident is protected by the guard.
*/

#endif  /* INCLUDED_NATIVE_C_STDIO */

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus)

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   include BSL_NATIVE_C_LIB_HEADER(stdio.h)

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bsl_stdhdrs_prologue.h>
#   endif

#   ifndef BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdio
#   endif

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   include BSL_NATIVE_C_LIB_HEADER(stdio.h)

// This native header does not define any symbols in namespace 'std' to import,
// so the following include is not necessary:
// #include <bsl_c_stdio.h>

#   ifdef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdio
#   undef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdio
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
