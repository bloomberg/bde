// bdemf_issame.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_ISSAME
#define INCLUDED_BDEMF_ISSAME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an meta-function for checking if two types are the same.
//
//@DEPRECATED: Use 'bslmf_issame' instead.
//
//@CLASSES:
// bdemf_IsSame: meta-function evaulating whether two types are the same
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's two parameter have the same type.  'bdemf_IsSame'
// defines a member, 'VALUE', whose value is initailized (at compile-time) to 1
// if the two parameters are the same type, and 0 if they are different types.
//
///Usage
///-----
// For example:
//..
//     typedef int    INT;
//     typedef double DOUBLE;
//
//     const int I = bdemf_IsSame<INT, INT>::VALUE;            assert(1 == I);
//     const int J = bdemf_IsSame<INT, DOUBLE>::VALUE;         assert(0 == J);
//..
// Note that a 'const'-qualified type is considered distinct from the
// non-'const' (but otherwise identical) type:
//..
//     typedef       short       SHORT;
//     typedef const short CONST_SHORT;
//     const int K = bdemf_IsSame<SHORT, CONST_SHORT>::VALUE;  assert(0 == K);
//..
// Similarly, a 'TYPE' and a reference to 'TYPE' ('TYPE&') are distinct:
//..
//     typedef int  INT;
//     typedef int& INT_REF;
//     const int L = bdemf_IsSame<INT, INT_REF>::VALUE;        assert(0 == L);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef bdemf_IsSame
#define bdemf_IsSame   bslmf_IsSame
    // Meta function with 'VALUE == 0' unless 'U' and 'V' are the same type.
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
