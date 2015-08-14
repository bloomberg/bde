// bdlf_placeholder.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLF_PLACEHOLDER
#define INCLUDED_BDLF_PLACEHOLDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized placeholder and specialized placeholders.
//
//@CLASSES:
//   bdlf::PlaceHolder: namespace for parameterized placeholders
//
//@SEE_ALSO: bdlf_bind
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a parameterized placeholder,
// 'bdlf::PlaceHolder', and fourteen placeholder objects ('_1', '_2', etc.),
// each of a distinct type.  '_1' is of type 'const bdlf::PlaceHolder<1>', '_2'
// is of type 'const bdlf::PlaceHolder<2>', and so on.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {

namespace bdlf {
template <int I>
struct PlaceHolder {
    enum { 
        value = I 
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , VALUE = value
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };
};

namespace PlaceHolders {

extern const PlaceHolder< 1>  _1;
extern const PlaceHolder< 2>  _2;
extern const PlaceHolder< 3>  _3;
extern const PlaceHolder< 4>  _4;
extern const PlaceHolder< 5>  _5;
extern const PlaceHolder< 6>  _6;
extern const PlaceHolder< 7>  _7;
extern const PlaceHolder< 8>  _8;
extern const PlaceHolder< 9>  _9;
extern const PlaceHolder<10> _10;
extern const PlaceHolder<11> _11;
extern const PlaceHolder<12> _12;
extern const PlaceHolder<13> _13;
extern const PlaceHolder<14> _14;

}  // close namespace PlaceHolders
}  // close package namespace
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
