// bdemf_removecvq.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_REMOVECVQ
#define INCLUDED_BDEMF_REMOVECVQ

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing const/volatile qualifiers.
//
//@DEPRECATED: Use 'bslmf_removecvq' instead.
//
//@CLASSES:
//  bdemf_RemoveCvq: meta-function for stripping const/volatile qualifiers
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// strip of any top-level const/volatile qualifiers from it's single template
// parameter.  The un-qualified type can be accessed via the 'Type' member
// defined in 'bdemf_RemoveCvq'.
//..
//   struct MyType {};
//
//   bdemf_RemoveCvq<int              >::Type i1; // int i1;
//   bdemf_RemoveCvq<const int        >::Type i2; // int i2;
//   bdemf_RemoveCvq<volatile int     >::Type i3; // int i3;
//   bdemf_RemoveCvq<int *            >::Type i4; // int *i4;
//   bdemf_RemoveCvq<int **           >::Type i5; // int **i5;
//   bdemf_RemoveCvq<int *const       >::Type i6; // int *i6;
//   bdemf_RemoveCvq<int *const *     >::Type i7; // int *const *i7;
//   bdemf_RemoveCvq<int *const *const>::Type i8; // int *const *i8;
//   bdemf_RemoveCvq<MyType           >::Type m1; // MyType m1;
//   bdemf_RemoveCvq<MyType const     >::Type m2; // MyType m2;
//..
// TBD: DOC 'bdemf_RemovePtrCvq' was added as a work-around for the Sun
// compiler, but is useful in its own right and should be tested and
// documented.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef bdemf_RemoveCvq
#define bdemf_RemoveCvq   bslmf_RemoveCvq
    // This class implements a meta-function for stripping top-level
    // const/volatile qualifiers from it's parameter type.
    // IMPLEMENTATION NOTE: The Sun Workshop 6 C++ 5.2 compiler works with
    // pointer specializations, but does not work with direct specialization
    // on 'T', 'T const', and 'T volatile'.  Thus, 'bslmf_RemoveCvq' is
    // implemented indirectly in terms of 'bslmf_RemovePtrCvq', above.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
