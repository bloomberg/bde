// bdef_placeholder.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEF_PLACEHOLDER
#define INCLUDED_BDEF_PLACEHOLDER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized placeholder and specialized placeholders.
//
//@CLASSES:
//   bdef_PlaceHolder: namespace for parameterized placeholders
//
//@SEE_ALSO: bdef_bind
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a parameterized placeholder,
// 'bdef_PlaceHolder', and fourteen placeholder objects ('_1', '_2', etc.),
// each of a distinct type.  '_1' is of type 'const bdef_PlaceHolder<1>', '_2'
// is of type 'const bdef_PlaceHolder<2>', and so on.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

template <int I>
struct bdef_PlaceHolder {
    enum { VALUE = I };
};

namespace bdef_PlaceHolders {

extern const bdef_PlaceHolder< 1>  _1;
extern const bdef_PlaceHolder< 2>  _2;
extern const bdef_PlaceHolder< 3>  _3;
extern const bdef_PlaceHolder< 4>  _4;
extern const bdef_PlaceHolder< 5>  _5;
extern const bdef_PlaceHolder< 6>  _6;
extern const bdef_PlaceHolder< 7>  _7;
extern const bdef_PlaceHolder< 8>  _8;
extern const bdef_PlaceHolder< 9>  _9;
extern const bdef_PlaceHolder<10> _10;
extern const bdef_PlaceHolder<11> _11;
extern const bdef_PlaceHolder<12> _12;
extern const bdef_PlaceHolder<13> _13;
extern const bdef_PlaceHolder<14> _14;

}  // close namespace bdef_PlaceHolders
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
