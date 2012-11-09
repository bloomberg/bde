// bsl_unordered_set.h                                                -*-C++-*-
#ifndef INCLUDED_BSL_UNORDERED_SET
#define INCLUDED_BSL_UNORDERED_SET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CPP11_HEADERS
// If the native library does not support unordered containers, then we must
// unconditionally include the BDE implementations.
# include <bslstl_unorderedmultiset.h>
# include <bslstl_unorderedset.h>
#else
// The unordered containers are a feature of the C++11 library, rather than
// C++03, so might not be present in all native libraries on the platforms we
// support.  Currently the 'BSLS_COMPILERFEATURES_SUPPORT_CPP11_HEADERS' is
// never defined, but this sketches out our plan for future support.
# include <unordered_set>

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.
# ifndef BSL_OVERRIDES_STD
#   include <bslstl_unorderedmultiset.h>
#   include <bslstl_unorderedset.h>
# endif // BSL_OVERRIDES_STD

#endif  // BDE_NATIVE_LIBRARY_HAS_CPP11_HEADERS

#endif  // INCLUDED_BSL_UNORDERED_SET

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
