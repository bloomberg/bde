// bslmf_typelist.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_TYPELIST
#define INCLUDED_BSLMF_TYPELIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typelist component
//
//@CLASSES:
//         bslmf_TypeList: Typelist of up to 20 types
//   bslmf_TypeListTypeOf: Meta-function to get the type of a typelist member
//        bslmf_TypeList0: Typelist of 0 types
//        bslmf_TypeList1: Typelist of 1 type
//        bslmf_TypeList2: Typelist of 2 types
//        bslmf_TypeList3: Typelist of 3 types
//        bslmf_TypeList4: Typelist of 4 types
//        bslmf_TypeList5: Typelist of 5 types
//        bslmf_TypeList6: Typelist of 6 types
//        bslmf_TypeList7: Typelist of 7 types
//        bslmf_TypeList8: Typelist of 8 types
//        bslmf_TypeList9: Typelist of 9 types
//       bslmf_TypeList10: Typelist of 10 types
//       bslmf_TypeList11: Typelist of 11 types
//       bslmf_TypeList12: Typelist of 12 types
//       bslmf_TypeList13: Typelist of 13 types
//       bslmf_TypeList14: Typelist of 14 types
//       bslmf_TypeList15: Typelist of 15 types
//       bslmf_TypeList16: Typelist of 16 types
//       bslmf_TypeList17: Typelist of 17 types
//       bslmf_TypeList18: Typelist of 18 types
//       bslmf_TypeList19: Typelist of 19 types
//       bslmf_TypeList20: Typelist of 20 types
//
//@SEE_ALSO:
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: 'bslmf_TypeList' provides a compile time list that holds up to
// 20 types.  Users can access the different types the list contains (by
// index), and the length of the typelist.  A 'bslmf_TypeList' is typically
// used when writing templatized classes that can store a variable amount of
// types, such as a bind or variant class.
//
///Usage
///-----
// The following usage example demonstrates how to retrieve information from a
// 'bslmf_TypeList':
//..
//  typedef bslmf_TypeList<int, double, char> List;
//..
// We can access the length of the list using the 'LENGTH' member:
//..
//  assert(3 == List::LENGTH);  // accessing the length of the list
//..
// We can also access the different types stored in the typelist using
// predefined type members 'TypeN' (where '1 <= N <= 20'), or another
// meta-function 'bslmf_TypeListTypeOf':
//..
//  assert(1 == bslmf_IsSame<int,    List::Type1>::VALUE)
//  assert(1 == bslmf_IsSame<double, List::Type2>::VALUE)
//  assert(1 == bslmf_IsSame<char,   List::Type3>::VALUE)
//
//  typedef bslmf_TypeListTypeOf<1, List>::Type my_Type1;
//  typedef bslmf_TypeListTypeOf<2, List>::Type my_Type2;
//  typedef bslmf_TypeListTypeOf<3, List>::Type my_Type3;
//
//  assert(1 == bslmf_IsSame<int,    my_Type1>::VALUE)
//  assert(1 == bslmf_IsSame<double, my_Type2>::VALUE)
//  assert(1 == bslmf_IsSame<char,   my_Type3>::VALUE)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

namespace BloombergLP {

// For backwards compatibility.
typedef bslmf_Nil bslmf_TypeListNil;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct bslmf_TypeList20;

                            // =====================
                            // struct bslmf_TypeList
                            // =====================

template <class A1=bslmf_Nil,  class A2=bslmf_Nil,  class A3=bslmf_Nil,
          class A4=bslmf_Nil,  class A5=bslmf_Nil,  class A6=bslmf_Nil,
          class A7=bslmf_Nil,  class A8=bslmf_Nil,  class A9=bslmf_Nil,
          class A10=bslmf_Nil, class A11=bslmf_Nil, class A12=bslmf_Nil,
          class A13=bslmf_Nil, class A14=bslmf_Nil, class A15=bslmf_Nil,
          class A16=bslmf_Nil, class A17=bslmf_Nil, class A18=bslmf_Nil,
          class A19=bslmf_Nil, class A20=bslmf_Nil>
struct bslmf_TypeList: public bslmf_TypeList20<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,
                                  A11,A12,A13,A14,A15,A16,A17,A18,A19,A20> {
    // This template declares a typelist of 0-20 types.  For each type
    // specified, a corresponding type will be declared as "Type<N>" where
    // where 'N' is the Nth parameter to this typelist(relative to 1).
    // Additionally, the enumeration 'LENGTH' is declared with a value equal to
    // the length of this typelist.  Each typelist also declares a member
    // template 'TypeOf' such that 'TypeOf<N>::Type' evaluates to the type of
    // the Nth the in this typelist.  The the type 'Type' is also declared a
    // length specific version of this typelist.
};

                         // ===========================
                         // struct bslmf_TypeListTypeOf
                         // ===========================

template <int INDEX, class LIST, class DEFAULTTYPE=bslmf_Nil,
          int INRANGE=((INDEX > 0 && INDEX <= (int)LIST::LENGTH) ? 1 : 0)>
struct bslmf_TypeListTypeOf {
    // This template is specialized below to return the type the 'INDEX'th
    // member of the typelist 'LIST'.  If '0 > INDEX <= LIST::LENGTH' then
    // 'Type' will be defined as the type of the member.  Note that INDEX is
    // relative to 1.

    // PUBLIC TYPES
    typedef LIST ListType;
    enum {LENGTH = ListType::LENGTH};

    typedef DEFAULTTYPE TypeOrDefault;
};

// ==================================
// VARIABLE NUMBER OF ARGUMENTS LISTS
// ==================================

                            // ======================
                            // struct bslmf_TypeList*
                            // ======================

struct bslmf_TypeList0 {
    // List of 0 types
    typedef bslmf_TypeList0 ListType;
    enum {LENGTH = 0};
    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };
    typedef ListType Type;
};

template <class A1>
struct bslmf_TypeList1  {
    // List of a single type 'A1' types
    typedef bslmf_TypeList1<A1> ListType;
    enum {LENGTH = 1};

    typedef A1 Type1;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2>
struct bslmf_TypeList2 {
    // List of a two types 'A1', 'A2' types
    typedef bslmf_TypeList2<A1,A2> ListType;
    enum {LENGTH = 2};

    typedef A1  Type1;
    typedef A2  Type2;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3>
struct bslmf_TypeList3 {
    // List of a three types 'A1', 'A2', 'A3' types

    typedef bslmf_TypeList3<A1,A2,A3> ListType;

    enum {LENGTH = 3}; // Length of this list

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4>
struct bslmf_TypeList4 {
    typedef bslmf_TypeList4<A1,A2,A3,A4> ListType;
    enum {LENGTH = 4};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5>
struct bslmf_TypeList5 {
    typedef bslmf_TypeList5<A1,A2,A3,A4,A5> ListType;
    enum {LENGTH = 5};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct bslmf_TypeList6 {
    typedef bslmf_TypeList6<A1,A2,A3,A4,A5,A6> ListType;
    enum {LENGTH = 6};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
struct bslmf_TypeList7 {
    typedef bslmf_TypeList7<A1,A2,A3,A4,A5,A6,A7> ListType;
    enum {LENGTH = 7};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
struct bslmf_TypeList8 {
    typedef bslmf_TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ListType;
    enum {LENGTH = 8};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
struct bslmf_TypeList9 {
    typedef bslmf_TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ListType;
    enum {LENGTH = 9};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
struct bslmf_TypeList10 {
    typedef bslmf_TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> ListType;
    enum {LENGTH = 10};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
struct bslmf_TypeList11 {
    typedef bslmf_TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> ListType;
    enum {LENGTH = 11};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
struct bslmf_TypeList12 {
    typedef bslmf_TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12> ListType;
    enum {LENGTH = 12};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
struct bslmf_TypeList13 {
    typedef bslmf_TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
        ListType;
    enum {LENGTH = 13};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
struct bslmf_TypeList14 {
    typedef bslmf_TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14> ListType;
    enum {LENGTH = 14};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
struct bslmf_TypeList15 {
    typedef bslmf_TypeList15<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14,A15> ListType;
    enum {LENGTH = 15};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16 >
struct bslmf_TypeList16 {
    typedef bslmf_TypeList16<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14,A15,A16> ListType;
    enum {LENGTH = 16};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct bslmf_TypeList17 {
    typedef bslmf_TypeList17<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14,A15,A16,A17> ListType;
    enum {LENGTH = 17};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct bslmf_TypeList18 {
    typedef bslmf_TypeList18<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14,A15,A16,A17,A18> ListType;
    enum {LENGTH = 18};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct bslmf_TypeList19 {
    typedef bslmf_TypeList19<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                             A14,A15,A16,A17,A18,A19> ListType;
    enum {LENGTH = 19};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;
    typedef A19 Type19;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct bslmf_TypeList20 {
    typedef bslmf_TypeList20<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,
                             A15,A16,A17,A18,A19,A20> ListType;
    enum {LENGTH = 20};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;
    typedef A19 Type19;
    typedef A20 Type20;

    template <int INDEX> struct TypeOf :
        public bslmf_TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

                            // ---------------------
                            // struct bslmf_TypeList
                            // ---------------------

// SPECIALIZATIONS
template <>
struct bslmf_TypeList<bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil>
:  public bslmf_TypeList0 {
    // This template provides a specialization of 'bslmf_TypeList' of 0 types.
};

template <class A1>
struct bslmf_TypeList<A1,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil>
:  public bslmf_TypeList1<A1> {
    // This template provides a specialization of 'bslmf_TypeList' of 1 type.
};

template <class A1, class A2>
struct bslmf_TypeList<A1, A2,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil>
:  public bslmf_TypeList2<A1,A2> {
    // This template provides a specialization of 'bslmf_TypeList' of 2 types.
};

template <class A1, class A2, class A3>
struct bslmf_TypeList<A1 ,A2, A3,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil>
: public bslmf_TypeList3<A1,A2,A3> {
    // This template provides a specialization of 'bslmf_TypeList' of 3 types.
};

template <class A1, class A2, class A3, class A4>
struct bslmf_TypeList<A1, A2, A3, A4,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList4<A1,A2,A3,A4> {
    // This template provides a specialization of 'bslmf_TypeList' of 4 types.
};

template <class A1, class A2, class A3, class A4, class A5>
struct bslmf_TypeList<A1, A2, A3, A4, A5,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList5<A1,A2,A3,A4,A5> {
    // This template provides a specialization of 'bslmf_TypeList' of 5 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList6<A1,A2,A3,A4,A5,A6> {
    // This template provides a specialization of 'bslmf_TypeList' of 6 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil>
: public bslmf_TypeList7<A1,A2,A3,A4,A5,A6,A7> {
    // This template provides a specialization of 'bslmf_TypeList' of 7 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> {
    // This template provides a specialization of 'bslmf_TypeList' of 8 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> {
    // This template provides a specialization of 'bslmf_TypeList' of 9 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> {
    // This template provides a specialization of 'bslmf_TypeList' of 10 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil>
: public bslmf_TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> {
    // This template provides a specialization of 'bslmf_TypeList' of 11 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12> {
    // This template provides a specialization of 'bslmf_TypeList' of 12 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil,
    bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13> {
    // This template provides a specialization of 'bslmf_TypeList' of 13 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14> {
    // This template provides a specialization of 'bslmf_TypeList' of 14 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList15<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15> {
    // This template provides a specialization of 'bslmf_TypeList' of 15 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList16<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16>{
    // This template provides a specialization of 'bslmf_TypeList' of 16 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, A17, bslmf_Nil, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList17<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17> {
    // This template provides a specialization of 'bslmf_TypeList' of 17 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, A17, A18, bslmf_Nil, bslmf_Nil>
: public bslmf_TypeList18<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17,A18> {
    // This template provides a specialization of 'bslmf_TypeList' of 18 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct bslmf_TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
              A14, A15, A16, A17, A18, A19, bslmf_Nil>
: public bslmf_TypeList19<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17,A18,A19> {
    // This template provides a specialization of 'bslmf_TypeList' of 19 types.
};

                         // ---------------------------
                         // struct bslmf_TypeListTypeOf
                         // ---------------------------

// SPECIALIZATIONS
template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<1,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type1 Type;
    typedef typename LIST::Type1 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<2,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type2 Type;
    typedef typename LIST::Type2 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<3,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type3 Type;
    typedef typename LIST::Type3 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<4,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type4 Type;
    typedef typename LIST::Type4 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<5,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type5 Type;
    typedef typename LIST::Type5 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<6,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type6 Type;
    typedef typename LIST::Type6 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<7,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type7 Type;
    typedef typename LIST::Type7 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<8,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type8 Type;
    typedef typename LIST::Type8 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<9,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type9 Type;
    typedef typename LIST::Type9 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<10,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type10 Type;
    typedef typename LIST::Type10 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<11,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type11 Type;
    typedef typename LIST::Type11 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<12,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type12 Type;
    typedef typename LIST::Type12 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<13,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type13 Type;
    typedef typename LIST::Type13 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<14,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type14 Type;
    typedef typename LIST::Type14 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<15,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type15 Type;
    typedef typename LIST::Type15 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<16,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type16 Type;
    typedef typename LIST::Type16 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<17,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type17 Type;
    typedef typename LIST::Type17 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<18,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type18 Type;
    typedef typename LIST::Type18 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<19,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type19 Type;
    typedef typename LIST::Type19 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct bslmf_TypeListTypeOf<20,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type20 Type;
    typedef typename LIST::Type20 TypeOrDefault;
};

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
