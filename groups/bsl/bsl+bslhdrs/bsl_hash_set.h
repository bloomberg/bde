// bsl_hash_set.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_HASH_SET
#define INCLUDED_BSL_HASH_SET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DEPRECATED: Do not use.
//
//@BDE_TRANSITIONAL
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef BSL_OVERRIDES_STD

#include <bslstp_hashset.h>

#else   // BSL_OVERRIDES_STD

#include <hash_set>

#endif  // BSL_OVERRIDES_STD

#endif  // #if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

#endif  // INCLUDED_BSL_HASH_SET

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
