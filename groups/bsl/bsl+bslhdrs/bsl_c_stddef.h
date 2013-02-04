/* bsl_c_stddef.h                                                    -*-C-*- */
#ifndef INCLUDED_BSL_C_STDDEF
#define INCLUDED_BSL_C_STDDEF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

/*
//@PURPOSE: Provide functionality of the corresponding C Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.
*/

#include <stddef.h>

#ifdef __cplusplus

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_CMP_SUN

// It is valid to include a C header inside an 'extern "C"' block, so C++ code
// within C files should be wrapped around an 'extern "C++"' block.
extern "C++" {

// On Sun, 'std::size_t' is defined in 'stddef.h'.
namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::size_t;
}

}

#endif  // BSLS_PLATFORM_CMP_SUN

#endif  // __cplusplus

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
