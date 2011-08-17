// bdemf_isenum.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_ISENUM
#define INCLUDED_BDEMF_ISENUM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide compile-time detection of enumerated types
//
//@DEPRECATED: Use 'bslmf_isenum' instead.
//
//@CLASSES:
//   bdemf_IsEnum: meta-function for detecting enumerated types
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///USAGE
///-----
//

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef bdemf_IsEnum
#define bdemf_IsEnum   bslmf_IsEnum
    // Implement a meta function which computes -- at compile time -- whether
    // 'TYPE' is an enumeration type.

    // Enumeration types are the only user-defined types that have the
    // characteristics of a native numeric type (i.e., They can be promoted to
    // 'int' without invoking user-defined conversions).  This class takes
    // advantage if this quality to distinguish 'enums' from class types that
    // are convertible to 'int'.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BDEMF_ISENUM)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
