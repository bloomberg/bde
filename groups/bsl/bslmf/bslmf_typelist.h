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
//  bslmf::TypeList: Typelist of up to 20 types
//  bslmf::TypeListTypeOf: Meta-function to get the type of a typelist member
//  bslmf::TypeList0: Typelist of 0 types
//  bslmf::TypeList1: Typelist of 1 type
//  bslmf::TypeList2: Typelist of 2 types
//  bslmf::TypeList3: Typelist of 3 types
//  bslmf::TypeList4: Typelist of 4 types
//  bslmf::TypeList5: Typelist of 5 types
//  bslmf::TypeList6: Typelist of 6 types
//  bslmf::TypeList7: Typelist of 7 types
//  bslmf::TypeList8: Typelist of 8 types
//  bslmf::TypeList9: Typelist of 9 types
//  bslmf::TypeList10: Typelist of 10 types
//  bslmf::TypeList11: Typelist of 11 types
//  bslmf::TypeList12: Typelist of 12 types
//  bslmf::TypeList13: Typelist of 13 types
//  bslmf::TypeList14: Typelist of 14 types
//  bslmf::TypeList15: Typelist of 15 types
//  bslmf::TypeList16: Typelist of 16 types
//  bslmf::TypeList17: Typelist of 17 types
//  bslmf::TypeList18: Typelist of 18 types
//  bslmf::TypeList19: Typelist of 19 types
//  bslmf::TypeList20: Typelist of 20 types
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: 'bslmf::TypeList' provides a compile time list that holds up to
// 20 types.  Users can access the different types the list contains (by
// index), and the length of the typelist.  A 'bslmf::TypeList' is typically
// used when writing templatized classes that can store a variable amount of
// types, such as a bind or variant class.
//
///Usage
///-----
// The following usage example demonstrates how to retrieve information from a
// 'bslmf::TypeList':
//..
//  typedef bslmf::TypeList<int, double, char> List;
//..
// We can access the length of the list using the 'LENGTH' member:
//..
//  assert(3 == List::LENGTH);  // accessing the length of the list
//..
// We can also access the different types stored in the typelist using
// predefined type members 'TypeN' (where '1 <= N <= 20'), or another
// meta-function 'bslmf::TypeListTypeOf':
//..
//  assert(1 == bslmf::IsSame<int,    List::Type1>::value)
//  assert(1 == bslmf::IsSame<double, List::Type2>::value)
//  assert(1 == bslmf::IsSame<char,   List::Type3>::value)
//
//  typedef bslmf::TypeListTypeOf<1, List>::Type my_Type1;
//  typedef bslmf::TypeListTypeOf<2, List>::Type my_Type2;
//  typedef bslmf::TypeListTypeOf<3, List>::Type my_Type3;
//
//  assert(1 == bslmf::IsSame<int,    my_Type1>::value)
//  assert(1 == bslmf::IsSame<double, my_Type2>::value)
//  assert(1 == bslmf::IsSame<char,   my_Type3>::value)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

namespace BloombergLP {

namespace bslmf {

// For backwards compatibility.
typedef Nil TypeListNil;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct TypeList20;

                            // ===============
                            // struct TypeList
                            // ===============

template <class A1=Nil,  class A2=Nil,  class A3=Nil,
          class A4=Nil,  class A5=Nil,  class A6=Nil,
          class A7=Nil,  class A8=Nil,  class A9=Nil,
          class A10=Nil, class A11=Nil, class A12=Nil,
          class A13=Nil, class A14=Nil, class A15=Nil,
          class A16=Nil, class A17=Nil, class A18=Nil,
          class A19=Nil, class A20=Nil>
struct TypeList: public TypeList20<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,
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

                         // =====================
                         // struct TypeListTypeOf
                         // =====================

template <int INDEX, class LIST, class DEFAULTTYPE=Nil,
          int INRANGE=((INDEX > 0 && INDEX <= (int)LIST::LENGTH) ? 1 : 0)>
struct TypeListTypeOf {
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

                            // ================
                            // struct TypeList*
                            // ================

struct TypeList0 {
    // List of 0 types
    typedef TypeList0 ListType;
    enum {LENGTH = 0};
    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };
    typedef ListType Type;
};

template <class A1>
struct TypeList1  {
    // List of a single type 'A1' types
    typedef TypeList1<A1> ListType;
    enum {LENGTH = 1};

    typedef A1 Type1;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2>
struct TypeList2 {
    // List of a two types 'A1', 'A2' types
    typedef TypeList2<A1,A2> ListType;
    enum {LENGTH = 2};

    typedef A1  Type1;
    typedef A2  Type2;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3>
struct TypeList3 {
    // List of a three types 'A1', 'A2', 'A3' types

    typedef TypeList3<A1,A2,A3> ListType;

    enum {LENGTH = 3}; // Length of this list

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4>
struct TypeList4 {
    typedef TypeList4<A1,A2,A3,A4> ListType;
    enum {LENGTH = 4};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5>
struct TypeList5 {
    typedef TypeList5<A1,A2,A3,A4,A5> ListType;
    enum {LENGTH = 5};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct TypeList6 {
    typedef TypeList6<A1,A2,A3,A4,A5,A6> ListType;
    enum {LENGTH = 6};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
struct TypeList7 {
    typedef TypeList7<A1,A2,A3,A4,A5,A6,A7> ListType;
    enum {LENGTH = 7};

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;

    template <int INDEX> struct TypeOf :
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
struct TypeList8 {
    typedef TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ListType;
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
struct TypeList9 {
    typedef TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ListType;
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
struct TypeList10 {
    typedef TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> ListType;
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
struct TypeList11 {
    typedef TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> ListType;
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
struct TypeList12 {
    typedef TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12> ListType;
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
struct TypeList13 {
    typedef TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
struct TypeList14 {
    typedef TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
struct TypeList15 {
    typedef TypeList15<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16 >
struct TypeList16 {
    typedef TypeList16<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct TypeList17 {
    typedef TypeList17<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct TypeList18 {
    typedef TypeList18<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct TypeList19 {
    typedef TypeList19<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct TypeList20 {
    typedef TypeList20<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,
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
        public TypeListTypeOf<INDEX,ListType>{ };

    typedef ListType Type;
};

                            // ---------------
                            // struct TypeList
                            // ---------------

// SPECIALIZATIONS
template <>
struct TypeList<Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil>
:  public TypeList0 {
    // This template provides a specialization of 'TypeList' of 0 types.
};

template <class A1>
struct TypeList<A1,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil>
:  public TypeList1<A1> {
    // This template provides a specialization of 'TypeList' of 1 type.
};

template <class A1, class A2>
struct TypeList<A1, A2,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil>
:  public TypeList2<A1,A2> {
    // This template provides a specialization of 'TypeList' of 2 types.
};

template <class A1, class A2, class A3>
struct TypeList<A1 ,A2, A3,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil>
: public TypeList3<A1,A2,A3> {
    // This template provides a specialization of 'TypeList' of 3 types.
};

template <class A1, class A2, class A3, class A4>
struct TypeList<A1, A2, A3, A4,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil>
: public TypeList4<A1,A2,A3,A4> {
    // This template provides a specialization of 'TypeList' of 4 types.
};

template <class A1, class A2, class A3, class A4, class A5>
struct TypeList<A1, A2, A3, A4, A5,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil>
: public TypeList5<A1,A2,A3,A4,A5> {
    // This template provides a specialization of 'TypeList' of 5 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct TypeList<A1, A2, A3, A4, A5, A6,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil>
: public TypeList6<A1,A2,A3,A4,A5,A6> {
    // This template provides a specialization of 'TypeList' of 6 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
struct TypeList<A1, A2, A3, A4, A5, A6, A7,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil>
: public TypeList7<A1,A2,A3,A4,A5,A6,A7> {
    // This template provides a specialization of 'TypeList' of 7 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil>
: public TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> {
    // This template provides a specialization of 'TypeList' of 8 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil>
: public TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> {
    // This template provides a specialization of 'TypeList' of 9 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil, Nil>
: public TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> {
    // This template provides a specialization of 'TypeList' of 10 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil,
    Nil>
: public TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> {
    // This template provides a specialization of 'TypeList' of 11 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil, Nil>
: public TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12> {
    // This template provides a specialization of 'TypeList' of 12 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    Nil, Nil, Nil, Nil,
    Nil, Nil, Nil>
: public TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13> {
    // This template provides a specialization of 'TypeList' of 13 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, Nil, Nil, Nil, Nil, Nil, Nil>
: public TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14> {
    // This template provides a specialization of 'TypeList' of 14 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, Nil, Nil, Nil, Nil, Nil>
: public TypeList15<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15> {
    // This template provides a specialization of 'TypeList' of 15 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, Nil, Nil, Nil, Nil>
: public TypeList16<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16>{
    // This template provides a specialization of 'TypeList' of 16 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, A17, Nil, Nil, Nil>
: public TypeList17<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17> {
    // This template provides a specialization of 'TypeList' of 17 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
    A14, A15, A16, A17, A18, Nil, Nil>
: public TypeList18<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17,A18> {
    // This template provides a specialization of 'TypeList' of 18 types.
};

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
              A14, A15, A16, A17, A18, A19, Nil>
: public TypeList19<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,
                          A16,A17,A18,A19> {
    // This template provides a specialization of 'TypeList' of 19 types.
};

                         // ---------------------
                         // struct TypeListTypeOf
                         // ---------------------

// SPECIALIZATIONS
template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<1,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type1 Type;
    typedef typename LIST::Type1 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<2,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type2 Type;
    typedef typename LIST::Type2 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<3,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type3 Type;
    typedef typename LIST::Type3 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<4,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type4 Type;
    typedef typename LIST::Type4 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<5,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type5 Type;
    typedef typename LIST::Type5 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<6,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type6 Type;
    typedef typename LIST::Type6 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<7,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type7 Type;
    typedef typename LIST::Type7 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<8,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type8 Type;
    typedef typename LIST::Type8 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<9,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type9 Type;
    typedef typename LIST::Type9 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<10,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type10 Type;
    typedef typename LIST::Type10 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<11,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type11 Type;
    typedef typename LIST::Type11 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<12,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type12 Type;
    typedef typename LIST::Type12 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<13,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type13 Type;
    typedef typename LIST::Type13 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<14,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type14 Type;
    typedef typename LIST::Type14 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<15,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type15 Type;
    typedef typename LIST::Type15 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<16,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type16 Type;
    typedef typename LIST::Type16 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<17,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type17 Type;
    typedef typename LIST::Type17 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<18,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type18 Type;
    typedef typename LIST::Type18 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<19,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type19 Type;
    typedef typename LIST::Type19 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeOf<20,LIST,DEFAULTTYPE,1> {
    typedef typename LIST::Type20 Type;
    typedef typename LIST::Type20 TypeOrDefault;
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_TypeList14
#undef bslmf_TypeList14
#endif
#define bslmf_TypeList14 bslmf::TypeList14
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeListTypeOf
#undef bslmf_TypeListTypeOf
#endif
#define bslmf_TypeListTypeOf bslmf::TypeListTypeOf
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList13
#undef bslmf_TypeList13
#endif
#define bslmf_TypeList13 bslmf::TypeList13
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList4
#undef bslmf_TypeList4
#endif
#define bslmf_TypeList4 bslmf::TypeList4
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList12
#undef bslmf_TypeList12
#endif
#define bslmf_TypeList12 bslmf::TypeList12
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList18
#undef bslmf_TypeList18
#endif
#define bslmf_TypeList18 bslmf::TypeList18
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList3
#undef bslmf_TypeList3
#endif
#define bslmf_TypeList3 bslmf::TypeList3
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList15
#undef bslmf_TypeList15
#endif
#define bslmf_TypeList15 bslmf::TypeList15
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList6
#undef bslmf_TypeList6
#endif
#define bslmf_TypeList6 bslmf::TypeList6
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList7
#undef bslmf_TypeList7
#endif
#define bslmf_TypeList7 bslmf::TypeList7
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList10
#undef bslmf_TypeList10
#endif
#define bslmf_TypeList10 bslmf::TypeList10
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList20
#undef bslmf_TypeList20
#endif
#define bslmf_TypeList20 bslmf::TypeList20
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList17
#undef bslmf_TypeList17
#endif
#define bslmf_TypeList17 bslmf::TypeList17
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList1
#undef bslmf_TypeList1
#endif
#define bslmf_TypeList1 bslmf::TypeList1
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList11
#undef bslmf_TypeList11
#endif
#define bslmf_TypeList11 bslmf::TypeList11
    // This alias is defined for backward compatibility.

typedef bslmf::TypeList0 bslmf_TypeList0;
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList9
#undef bslmf_TypeList9
#endif
#define bslmf_TypeList9 bslmf::TypeList9
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList8
#undef bslmf_TypeList8
#endif
#define bslmf_TypeList8 bslmf::TypeList8
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList5
#undef bslmf_TypeList5
#endif
#define bslmf_TypeList5 bslmf::TypeList5
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList
#undef bslmf_TypeList
#endif
#define bslmf_TypeList bslmf::TypeList
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList19
#undef bslmf_TypeList19
#endif
#define bslmf_TypeList19 bslmf::TypeList19
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList16
#undef bslmf_TypeList16
#endif
#define bslmf_TypeList16 bslmf::TypeList16
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList2
#undef bslmf_TypeList2
#endif
#define bslmf_TypeList2 bslmf::TypeList2
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeListNil
#undef bslmf_TypeListNil
#endif
#define bslmf_TypeListNil bslmf::TypeListNil
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
