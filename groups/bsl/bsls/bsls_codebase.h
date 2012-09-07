// bsls_codebase.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_CODEBASE
#define INCLUDED_BSLS_CODEBASE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide code deprecation management.
//
//@CLASSES:
//
//@AUTHOR: Alexander Beels (abeels)
//
//@DESCRIPTION: TBD
//
///Usage
///-----
// TBD

#if 0 //ARB: TESTING

#ifndef BSL_LEGACY
#define BSL_LEGACY 1
    // This macro controls whether we allow features which we must continue to
    // support for our clients but do not want to rely on in our own code base.
    // Clients who want to continue using these features should define
    // 'BSL_LEGACY' as 1, which is the default unless it is already defined.
    // In order to make sure an entire code base does not rely on these
    // features, recompile with this macro set to 0.  Examples of such features
    // are: including '<stdheader>' as opposed to '<bsl_stdheader.h>', or using
    // 'DEBUG' instead of 'BAEL_DEBUG'.
#elif BSL_LEGACY == 0
#define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES 1
    // When we don't want to rely on legacy features, we also want to make sure
    // we are not picking up macros or type aliases via (direct or transitive)
    // includes of headers that have migrated from 'bde' to 'bsl' libraries.
#endif
#define BDE_TRANSITIONAL           1
#define BDE_BACKWARD_COMPATIBILITY 1

#else //ARB: TESTING

#define BSL_LEGACY                 0
#define BDE_TRANSITIONAL           0
#define BDE_BACKWARD_COMPATIBILITY 1

#endif //ARB: TESTING

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
