// bdemf_typelist.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_TYPELIST
#define INCLUDED_BDEMF_TYPELIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a typelist component
//
//@DEPRECATED: Use 'bslmf_typelist' instead.
//
//@CLASSES:
//         bdemf_TypeList: Typelist of upto 20 types
//   bdemf_TypeListTypeOf: Meta-function to get the type of a typelist member
//        bdemf_TypeList0: Typelist of 0 types
//        bdemf_TypeList1: Typelist of 1 type
//        bdemf_TypeList2: Typelist of 2 types
//        bdemf_TypeList3: Typelist of 3 types
//        bdemf_TypeList4: Typelist of 4 types
//        bdemf_TypeList5: Typelist of 5 types
//        bdemf_TypeList6: Typelist of 6 types
//        bdemf_TypeList7: Typelist of 7 types
//        bdemf_TypeList8: Typelist of 8 types
//        bdemf_TypeList9: Typelist of 9 types
//       bdemf_TypeList10: Typelist of 10 types
//       bdemf_TypeList11: Typelist of 11 types
//       bdemf_TypeList12: Typelist of 12 types
//       bdemf_TypeList13: Typelist of 13 types
//       bdemf_TypeList14: Typelist of 14 types
//       bdemf_TypeList15: Typelist of 15 types
//       bdemf_TypeList16: Typelist of 16 types
//       bdemf_TypeList17: Typelist of 17 types
//       bdemf_TypeList18: Typelist of 18 types
//       bdemf_TypeList19: Typelist of 19 types
//       bdemf_TypeList20: Typelist of 20 types
//
//@SEE_ALSO:
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION:
//
///Usage
///-----
//..
// typedef bdemf_TypeList2<int, double> List;
// List::Type1 v1;
// List::Type2 v2;
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

#define bdemf_TypeListNil      bslmf_TypeListNil
    // This type is used internally to indicate an Nil argument of a
    // variable length typelist declaration.

#define bdemf_TypeList         bslmf_TypeList
    // This template declares a typelist of 0-20 types.  For each type
    // specifed, a corresponding type will be declared as "Type<N>" where
    // where "N" is the Nth parameter to this typelist(relative to 1).
    // Additionally, the enumeration 'LENGTH' is declared with a value equal
    // to the length of this typelist.  Each typelist also declares a member
    // template 'TypeOf' such that "TypeOf<N>::Type"  evaluates to the type of
    // the Nth the in this typelist.  The the type 'Type' is also declared a
    // length specific version of this typelist.

#define bdemf_TypeListTypeOf   bslmf_TypeListTypeOf
    // This template is specialized below to return the type the 'INDEX'th
    // member of the typelist 'LIST'.  If 0 > INDEX <= LIST::LENGTH then 'Type'
    // will be defined as the type of the member.  Note that INDEX is relative
    // to 1.

#define bdemf_TypeList0        bslmf_TypeList0

#define bdemf_TypeList1        bslmf_TypeList1
#define bdemf_TypeList2        bslmf_TypeList2
#define bdemf_TypeList3        bslmf_TypeList3
#define bdemf_TypeList4        bslmf_TypeList4
#define bdemf_TypeList5        bslmf_TypeList5
#define bdemf_TypeList6        bslmf_TypeList6
#define bdemf_TypeList7        bslmf_TypeList7
#define bdemf_TypeList8        bslmf_TypeList8
#define bdemf_TypeList9        bslmf_TypeList9
#define bdemf_TypeList10       bslmf_TypeList10
#define bdemf_TypeList11       bslmf_TypeList11
#define bdemf_TypeList12       bslmf_TypeList12
#define bdemf_TypeList13       bslmf_TypeList13
#define bdemf_TypeList14       bslmf_TypeList14
#define bdemf_TypeList15       bslmf_TypeList15
#define bdemf_TypeList16       bslmf_TypeList16
#define bdemf_TypeList17       bslmf_TypeList17
#define bdemf_TypeList18       bslmf_TypeList18
#define bdemf_TypeList19       bslmf_TypeList19
#define bdemf_TypeList20       bslmf_TypeList20

namespace BloombergLP {

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
