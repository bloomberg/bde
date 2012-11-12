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

#include <unordered_set>

#ifndef BSL_OVERRIDES_STD
#include <bslstl_unorderedmultiset.h>
#include <bslstl_unorderedset.h>
#endif  // BSL_OVERRIDES_STD

#endif  // INCLUDED_BSL_UNORDERED_SET

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
