/* stddef.h                                                          -*-C-*- */
#ifndef INCLUDED_NATIVE_C_STDDEF
#define INCLUDED_NATIVE_C_STDDEF

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
// Note that 'stddef.h' is meant for multiple inclusion on linux - therefore
// only the ident is protected by the guard.
*/

#endif  /* INCLUDED_NATIVE_C_STDDEF */

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus)

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
                                        (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <stddef.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(stddef.h)
#   endif

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bsl_stdhdrs_prologue.h>
#   endif

#   ifndef BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_EPILOGUE_RUN_BY_c_stddef
#   endif

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_PLATFORM__CMP_GNU) && \
                                        (BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)
#     include_next <stddef.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(stddef.h)
#   endif

// On SUN, 'stddef.h' (a C header) defines 'size_t'.  This odd implementation
// forces the following 'using' directive.
#   ifdef BSLS_PLATFORM__CMP_SUN

// This C header file might be included within an 'extern "C"' block.  To avoid
// defining C++ symbols in an 'extern "C"' block, wrap the C++ code with an
// 'extern "C++"' block.
extern "C++" {

    namespace bsl
    {
        // Import selected symbols into bsl namespace
        using native_std::size_t;
    }

}  // extern "C++"

#   endif  // BSLS_PLATFORM__CMP_SUN

#   ifdef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stddef
#   undef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stddef
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
