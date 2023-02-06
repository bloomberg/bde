// bslmf_typelist.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_TYPELIST
#define INCLUDED_BSLMF_TYPELIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typelist component.
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
// predefined type members 'TypeN' (where '1 <= t_N <= 20'), or another
// meta-function 'bslmf::TypeListTypeOf':
//..
//  assert(1 == bsl::is_same<int,    List::Type1>::value)
//  assert(1 == bsl::is_same<double, List::Type2>::value)
//  assert(1 == bsl::is_same<char,   List::Type3>::value)
//
//  typedef bslmf::TypeListTypeOf<1, List>::Type my_Type1;
//  typedef bslmf::TypeListTypeOf<2, List>::Type my_Type2;
//  typedef bslmf::TypeListTypeOf<3, List>::Type my_Type3;
//
//  assert(1 == bsl::is_same<int,    my_Type1>::value)
//  assert(1 == bsl::is_same<double, my_Type2>::value)
//  assert(1 == bsl::is_same<char,   my_Type3>::value)
//..

#include <bslscm_version.h>

#include <bslmf_nil.h>

#include <bsls_compilerfeatures.h>


#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BSLMF_TYPELIST_USING_VARIADIC_TEMPLATES
    // This macro indicates that we have all the needed features for an
    // implementation of the typelist facility that is source-compatible with
    // the pre-existing C++03 facility.  The main change is that the numbered
    // classes TypeList0-TypeList20 are no longer distinct classes, but aliases
    // of specific instantiations of the primary TypeList template.  Eventually
    // these partial template specializations will be eliminated, when the
    // individually named members are no longer used throughout the whole of
    // the Bloomberg codebase.
#endif

namespace BloombergLP {

namespace bslmf {

// For backwards compatibility.
typedef Nil TypeListNil;

#if defined(BSLMF_TYPELIST_USING_VARIADIC_TEMPLATES)

                         // =====================
                         // struct TypeListTypeAt
                         // =====================

template <unsigned t_INDEX,  // 'unsigned' is a proxy for 'size_t'
          class t_LIST,
          class t_DEFAULTTYPE = Nil,
          bool t_INRANGE      = (t_INDEX < t_LIST::LENGTH)>
struct TypeListTypeAt {
    // This template is specialized below to return the type the 't_INDEX'th
    // member of the typelist 't_LIST'.  If '0 > t_INDEX <= t_LIST::LENGTH'
    // then 'Type' will be defined as the type of the member.  Note that
    // t_INDEX is relative to 1.

    // PUBLIC t_TYPES
    typedef t_LIST ListType;
    enum { LENGTH = ListType::LENGTH };

    typedef t_DEFAULTTYPE TypeOrDefault;
};

                         // =====================
                         // struct TypeListTypeOf
                         // =====================

template <int t_INDEX, class t_LIST, class t_DEFAULTTYPE = Nil>
using TypeListTypeOf = TypeListTypeAt<static_cast<unsigned int>(t_INDEX - 1),
                                      t_LIST,
                                      t_DEFAULTTYPE>;
    // Classic Bloomberg code uses a 1-based index into the type list.

                              // ===============
                              // struct TypeList
                              // ===============

template <class ...t_TYPES>
struct TypeList {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum { LENGTH = sizeof...(t_TYPES) };

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;
};


using TypeList0 = TypeList<>;

template <class t_A1>
using TypeList1 = TypeList<t_A1>;

template <class t_A1, class t_A2>
using TypeList2 = TypeList<t_A1, t_A2>;

template <class t_A1, class t_A2, class t_A3>
using TypeList3 = TypeList<t_A1, t_A2, t_A3>;

template <class t_A1, class t_A2, class t_A3, class t_A4>
using TypeList4 = TypeList<t_A1, t_A2, t_A3, t_A4>;

template <class t_A1, class t_A2, class t_A3, class t_A4, class t_A5>
using TypeList5 = TypeList<t_A1, t_A2, t_A3, t_A4, t_A5>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6>
using TypeList6 = TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7>
using TypeList7 = TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8>
using TypeList8 = TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9>
using TypeList9 =
                TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10>
using TypeList10 =
         TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9, t_A10>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11>
using TypeList11 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12>
using TypeList12 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13>
using TypeList13 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14>
using TypeList14 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15>
using TypeList15 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16>
using TypeList16 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15,
                            t_A16>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17>
using TypeList17 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15,
                            t_A16,
                            t_A17>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18>
using TypeList18 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15,
                            t_A16,
                            t_A17,
                            t_A18>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19>
using TypeList19 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15,
                            t_A16,
                            t_A17,
                            t_A18,
                            t_A19>;

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19,
          class t_A20>
using TypeList20 = TypeList<t_A1,
                            t_A2,
                            t_A3,
                            t_A4,
                            t_A5,
                            t_A6,
                            t_A7,
                            t_A8,
                            t_A9,
                            t_A10,
                            t_A11,
                            t_A12,
                            t_A13,
                            t_A14,
                            t_A15,
                            t_A16,
                            t_A17,
                            t_A18,
                            t_A19,
                            t_A20>;

          // ========================================================
          // TypeList specializations for fixed number of  parameters
          // ========================================================

template <class t_A1>
struct TypeList<t_A1> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 1};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
};

template <class t_A1, class t_A2>
struct TypeList<t_A1, t_A2> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 2};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
};

template <class t_A1, class t_A2, class t_A3>
struct TypeList<t_A1, t_A2, t_A3> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 3};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
};

template <class t_A1, class t_A2, class t_A3, class t_A4>
struct TypeList<t_A1, t_A2, t_A3, t_A4> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 4};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
};

template <class t_A1, class t_A2, class t_A3, class t_A4, class t_A5>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 5};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 6};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 7};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 8};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;
    typedef t_A8 Type8;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 9};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;
    typedef t_A8 Type8;
    typedef t_A9 Type9;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10>
struct TypeList<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9, t_A10> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 10};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 11};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 12};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 13};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 14};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 15};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 16};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 17};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                t_A18> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 18};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                t_A18,
                t_A19> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 19};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;
    typedef t_A19 Type19;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19,
          class t_A20>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                t_A18,
                t_A19,
                t_A20> {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 20};

    template <unsigned t_INDEX>
    using TypeOf = TypeListTypeAt<t_INDEX - 1, TypeList>;

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;
    typedef t_A19 Type19;
    typedef t_A20 Type20;
};

                         // ---------------------
                         // struct TypeListTypeAt
                         // ---------------------

// SPECIALIZATIONS
template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<0u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type1 Type;
    typedef typename t_LIST::Type1 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<1u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type2 Type;
    typedef typename t_LIST::Type2 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<2u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type3 Type;
    typedef typename t_LIST::Type3 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<3u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type4 Type;
    typedef typename t_LIST::Type4 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<4u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type5 Type;
    typedef typename t_LIST::Type5 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<5u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type6 Type;
    typedef typename t_LIST::Type6 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<6u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type7 Type;
    typedef typename t_LIST::Type7 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<7u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type8 Type;
    typedef typename t_LIST::Type8 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<8u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type9 Type;
    typedef typename t_LIST::Type9 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<9u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type10 Type;
    typedef typename t_LIST::Type10 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<10u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type11 Type;
    typedef typename t_LIST::Type11 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<11u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type12 Type;
    typedef typename t_LIST::Type12 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<12u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type13 Type;
    typedef typename t_LIST::Type13 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<13u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type14 Type;
    typedef typename t_LIST::Type14 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<14u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type15 Type;
    typedef typename t_LIST::Type15 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<15u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type16 Type;
    typedef typename t_LIST::Type16 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<16u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type17 Type;
    typedef typename t_LIST::Type17 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<17u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type18 Type;
    typedef typename t_LIST::Type18 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<18u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type19 Type;
    typedef typename t_LIST::Type19 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeAt<19u, t_LIST, t_DEFAULTTYPE, true> {
    typedef typename t_LIST::Type20 Type;
    typedef typename t_LIST::Type20 TypeOrDefault;
};

template <unsigned t_N,
          class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19,
          class t_A20,
          class t_A21,
          class t_DEFAULTTYPE,
          class... t_TYPES>
struct TypeListTypeAt<t_N,
                      TypeList<t_A1,
                               t_A2,
                               t_A3,
                               t_A4,
                               t_A5,
                               t_A6,
                               t_A7,
                               t_A8,
                               t_A9,
                               t_A10,
                               t_A11,
                               t_A12,
                               t_A13,
                               t_A14,
                               t_A15,
                               t_A16,
                               t_A17,
                               t_A18,
                               t_A19,
                               t_A20,
                               t_A21,
                               t_TYPES...>,
                      t_DEFAULTTYPE,
                      true> {
    typedef typename TypeListTypeAt<t_N - 20,
                                    TypeList<t_A21, t_TYPES...>,
                                    t_DEFAULTTYPE>::Type Type;

    typedef
        typename TypeListTypeAt<t_N - 20,
                                TypeList<t_A21, t_TYPES...>,
                                t_DEFAULTTYPE>::TypeOrDefault TypeOrDefault;
};

#else
template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19,
          class t_A20>
struct TypeList20;

                              // ===============
                              // struct TypeList
                              // ===============

template <class t_A1  = Nil,
          class t_A2  = Nil,
          class t_A3  = Nil,
          class t_A4  = Nil,
          class t_A5  = Nil,
          class t_A6  = Nil,
          class t_A7  = Nil,
          class t_A8  = Nil,
          class t_A9  = Nil,
          class t_A10 = Nil,
          class t_A11 = Nil,
          class t_A12 = Nil,
          class t_A13 = Nil,
          class t_A14 = Nil,
          class t_A15 = Nil,
          class t_A16 = Nil,
          class t_A17 = Nil,
          class t_A18 = Nil,
          class t_A19 = Nil,
          class t_A20 = Nil>
struct TypeList : public TypeList20<t_A1,
                                    t_A2,
                                    t_A3,
                                    t_A4,
                                    t_A5,
                                    t_A6,
                                    t_A7,
                                    t_A8,
                                    t_A9,
                                    t_A10,
                                    t_A11,
                                    t_A12,
                                    t_A13,
                                    t_A14,
                                    t_A15,
                                    t_A16,
                                    t_A17,
                                    t_A18,
                                    t_A19,
                                    t_A20> {
    // This template declares a typelist of 0-20 types.  For each type
    // specified, a corresponding type will be declared as "Type<t_N>" where
    // where 't_N' is the Nth parameter to this typelist(relative to 1).
    // Additionally, the enumeration 'LENGTH' is declared with a value equal to
    // the length of this typelist.  Each typelist also declares a member
    // template 'TypeOf' such that 'TypeOf<t_N>::Type' evaluates to the type of
    // the Nth the in this typelist.  The type 'Type' is also declared a length
    // specific version of this typelist.
};

                           // =====================
                           // struct TypeListTypeOf
                           // =====================

template <int t_INDEX,
          class t_LIST,
          class t_DEFAULTTYPE = Nil,
          int t_INRANGE =
              ((1 <= t_INDEX && t_INDEX <= (int)t_LIST::LENGTH) ? 1 : 0)>
struct TypeListTypeOf {
    // This template is specialized below to return the type the 't_INDEX'th
    // member of the typelist 't_LIST'.  If '1 <= t_INDEX <= t_LIST::LENGTH'
    // then 'Type' will be defined as the type of the member.  Note that
    // t_INDEX is relative to 1.

    // PUBLIC t_TYPES
    typedef t_LIST ListType;
    enum { LENGTH = ListType::LENGTH };

    typedef t_DEFAULTTYPE TypeOrDefault;
};

// ============================================================================
//                      VARIABLE NUMBER OF ARGUMENTS LISTS
// ============================================================================

                            // ================
                            // struct TypeList*
                            // ================

struct TypeList0 {
    // List of 0 types
    typedef TypeList0 ListType;
    enum { LENGTH = 0 };
    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };
    typedef ListType Type;
};

template <class t_A1>
struct TypeList1 {
    // List of a single type 't_A1' types
    typedef TypeList1<t_A1> ListType;
    enum { LENGTH = 1 };

    typedef t_A1 Type1;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1, class t_A2>
struct TypeList2 {
    // List of a two types 't_A1', 't_A2' types
    typedef TypeList2<t_A1, t_A2> ListType;
    enum { LENGTH = 2 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1, class t_A2, class t_A3>
struct TypeList3 {
    // List of a three types 't_A1', 't_A2', 't_A3' types

    typedef TypeList3<t_A1,t_A2,t_A3> ListType;

    enum {LENGTH = 3}; // Length of this list

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1, class t_A2, class t_A3, class t_A4>
struct TypeList4 {
    typedef TypeList4<t_A1, t_A2, t_A3, t_A4> ListType;
    enum { LENGTH = 4 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1, class t_A2, class t_A3, class t_A4, class t_A5>
struct TypeList5 {
    typedef TypeList5<t_A1, t_A2, t_A3, t_A4, t_A5> ListType;
    enum { LENGTH = 5 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6>
struct TypeList6 {
    typedef TypeList6<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6> ListType;
    enum { LENGTH = 6 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7>
struct TypeList7 {
    typedef TypeList7<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7> ListType;
    enum { LENGTH = 7 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8>
struct TypeList8 {
    typedef TypeList8<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8> ListType;
    enum { LENGTH = 8 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;
    typedef t_A8 Type8;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9>
struct TypeList9 {
    typedef TypeList9<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9>
        ListType;
    enum { LENGTH = 9 };

    typedef t_A1 Type1;
    typedef t_A2 Type2;
    typedef t_A3 Type3;
    typedef t_A4 Type4;
    typedef t_A5 Type5;
    typedef t_A6 Type6;
    typedef t_A7 Type7;
    typedef t_A8 Type8;
    typedef t_A9 Type9;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10>
struct TypeList10 {
    typedef TypeList10<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10>
        ListType;
    enum { LENGTH = 10 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11>
struct TypeList11 {
    typedef TypeList11<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11>
        ListType;
    enum { LENGTH = 11 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12>
struct TypeList12 {
    typedef TypeList12<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12>
        ListType;
    enum { LENGTH = 12 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13>
struct TypeList13 {
    typedef TypeList13<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13>
        ListType;
    enum { LENGTH = 13 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14>
struct TypeList14 {
    typedef TypeList14<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14>
        ListType;
    enum { LENGTH = 14 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15>
struct TypeList15 {
    typedef TypeList15<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15>
        ListType;
    enum { LENGTH = 15 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16>
struct TypeList16 {
    typedef TypeList16<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15,
                       t_A16>
        ListType;
    enum { LENGTH = 16 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17>
struct TypeList17 {
    typedef TypeList17<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15,
                       t_A16,
                       t_A17>
        ListType;
    enum { LENGTH = 17 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18>
struct TypeList18 {
    typedef TypeList18<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15,
                       t_A16,
                       t_A17,
                       t_A18>
        ListType;
    enum { LENGTH = 18 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19>
struct TypeList19 {
    typedef TypeList19<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15,
                       t_A16,
                       t_A17,
                       t_A18,
                       t_A19>
        ListType;
    enum { LENGTH = 19 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;
    typedef t_A19 Type19;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

    typedef ListType Type;
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19,
          class t_A20>
struct TypeList20 {
    typedef TypeList20<t_A1,
                       t_A2,
                       t_A3,
                       t_A4,
                       t_A5,
                       t_A6,
                       t_A7,
                       t_A8,
                       t_A9,
                       t_A10,
                       t_A11,
                       t_A12,
                       t_A13,
                       t_A14,
                       t_A15,
                       t_A16,
                       t_A17,
                       t_A18,
                       t_A19,
                       t_A20>
        ListType;
    enum { LENGTH = 20 };

    typedef t_A1  Type1;
    typedef t_A2  Type2;
    typedef t_A3  Type3;
    typedef t_A4  Type4;
    typedef t_A5  Type5;
    typedef t_A6  Type6;
    typedef t_A7  Type7;
    typedef t_A8  Type8;
    typedef t_A9  Type9;
    typedef t_A10 Type10;
    typedef t_A11 Type11;
    typedef t_A12 Type12;
    typedef t_A13 Type13;
    typedef t_A14 Type14;
    typedef t_A15 Type15;
    typedef t_A16 Type16;
    typedef t_A17 Type17;
    typedef t_A18 Type18;
    typedef t_A19 Type19;
    typedef t_A20 Type20;

    template <int t_INDEX>
    struct TypeOf : public TypeListTypeOf<t_INDEX, ListType> {
    };

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

template <class t_A1>
struct TypeList<t_A1,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList1<t_A1> {
    // This template provides a specialization of 'TypeList' of 1 type.
};

template <class t_A1, class t_A2>
struct TypeList<t_A1,
                t_A2,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList2<t_A1, t_A2> {
    // This template provides a specialization of 'TypeList' of 2 types.
};

template <class t_A1, class t_A2, class t_A3>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList3<t_A1, t_A2, t_A3> {
    // This template provides a specialization of 'TypeList' of 3 types.
};

template <class t_A1, class t_A2, class t_A3, class t_A4>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList4<t_A1, t_A2, t_A3, t_A4> {
    // This template provides a specialization of 'TypeList' of 4 types.
};

template <class t_A1, class t_A2, class t_A3, class t_A4, class t_A5>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList5<t_A1, t_A2, t_A3, t_A4, t_A5> {
    // This template provides a specialization of 'TypeList' of 5 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList6<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6> {
    // This template provides a specialization of 'TypeList' of 6 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil>
: public TypeList7<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7> {
    // This template provides a specialization of 'TypeList' of 7 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil>
: public TypeList8<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8> {
    // This template provides a specialization of 'TypeList' of 8 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil>
: public TypeList9<t_A1, t_A2, t_A3, t_A4, t_A5, t_A6, t_A7, t_A8, t_A9> {
    // This template provides a specialization of 'TypeList' of 9 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList10<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10> {
    // This template provides a specialization of 'TypeList' of 10 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList11<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11> {
    // This template provides a specialization of 'TypeList' of 11 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList12<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12> {
    // This template provides a specialization of 'TypeList' of 12 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList13<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13> {
    // This template provides a specialization of 'TypeList' of 13 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList14<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14> {
    // This template provides a specialization of 'TypeList' of 14 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                Nil,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList15<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14,
                                         t_A15> {
    // This template provides a specialization of 'TypeList' of 15 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                Nil,
                Nil,
                Nil,
                Nil> : public TypeList16<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14,
                                         t_A15,
                                         t_A16> {
    // This template provides a specialization of 'TypeList' of 16 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                Nil,
                Nil,
                Nil> : public TypeList17<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14,
                                         t_A15,
                                         t_A16,
                                         t_A17> {
    // This template provides a specialization of 'TypeList' of 17 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                t_A18,
                Nil,
                Nil> : public TypeList18<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14,
                                         t_A15,
                                         t_A16,
                                         t_A17,
                                         t_A18> {
    // This template provides a specialization of 'TypeList' of 18 types.
};

template <class t_A1,
          class t_A2,
          class t_A3,
          class t_A4,
          class t_A5,
          class t_A6,
          class t_A7,
          class t_A8,
          class t_A9,
          class t_A10,
          class t_A11,
          class t_A12,
          class t_A13,
          class t_A14,
          class t_A15,
          class t_A16,
          class t_A17,
          class t_A18,
          class t_A19>
struct TypeList<t_A1,
                t_A2,
                t_A3,
                t_A4,
                t_A5,
                t_A6,
                t_A7,
                t_A8,
                t_A9,
                t_A10,
                t_A11,
                t_A12,
                t_A13,
                t_A14,
                t_A15,
                t_A16,
                t_A17,
                t_A18,
                t_A19,
                Nil> : public TypeList19<t_A1,
                                         t_A2,
                                         t_A3,
                                         t_A4,
                                         t_A5,
                                         t_A6,
                                         t_A7,
                                         t_A8,
                                         t_A9,
                                         t_A10,
                                         t_A11,
                                         t_A12,
                                         t_A13,
                                         t_A14,
                                         t_A15,
                                         t_A16,
                                         t_A17,
                                         t_A18,
                                         t_A19> {
    // This template provides a specialization of 'TypeList' of 19 types.
};

                         // ---------------------
                         // struct TypeListTypeOf
                         // ---------------------

// SPECIALIZATIONS
template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<1, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type1 Type;
    typedef typename t_LIST::Type1 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<2, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type2 Type;
    typedef typename t_LIST::Type2 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<3, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type3 Type;
    typedef typename t_LIST::Type3 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<4, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type4 Type;
    typedef typename t_LIST::Type4 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<5, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type5 Type;
    typedef typename t_LIST::Type5 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<6, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type6 Type;
    typedef typename t_LIST::Type6 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<7, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type7 Type;
    typedef typename t_LIST::Type7 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<8, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type8 Type;
    typedef typename t_LIST::Type8 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<9, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type9 Type;
    typedef typename t_LIST::Type9 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<10, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type10 Type;
    typedef typename t_LIST::Type10 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<11, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type11 Type;
    typedef typename t_LIST::Type11 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<12, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type12 Type;
    typedef typename t_LIST::Type12 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<13, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type13 Type;
    typedef typename t_LIST::Type13 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<14, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type14 Type;
    typedef typename t_LIST::Type14 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<15, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type15 Type;
    typedef typename t_LIST::Type15 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<16, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type16 Type;
    typedef typename t_LIST::Type16 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<17, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type17 Type;
    typedef typename t_LIST::Type17 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<18, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type18 Type;
    typedef typename t_LIST::Type18 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<19, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type19 Type;
    typedef typename t_LIST::Type19 TypeOrDefault;
};

template <class t_LIST, class t_DEFAULTTYPE>
struct TypeListTypeOf<20, t_LIST, t_DEFAULTTYPE, 1> {
    typedef typename t_LIST::Type20 Type;
    typedef typename t_LIST::Type20 TypeOrDefault;
};
#endif

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY

// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslmf::TypeList0 bslmf_TypeList0;
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList
#undef bslmf_TypeList
#endif
#define bslmf_TypeList bslmf::TypeList
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeListNil
#undef bslmf_TypeListNil
#endif
#define bslmf_TypeListNil bslmf::TypeListNil
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList1
#undef bslmf_TypeList1
#endif
#define bslmf_TypeList1 bslmf::TypeList1
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList2
#undef bslmf_TypeList2
#endif
#define bslmf_TypeList2 bslmf::TypeList2
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList3
#undef bslmf_TypeList3
#endif
#define bslmf_TypeList3 bslmf::TypeList3
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList4
#undef bslmf_TypeList4
#endif
#define bslmf_TypeList4 bslmf::TypeList4
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList5
#undef bslmf_TypeList5
#endif
#define bslmf_TypeList5 bslmf::TypeList5
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

#ifdef bslmf_TypeList8
#undef bslmf_TypeList8
#endif
#define bslmf_TypeList8 bslmf::TypeList8
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList9
#undef bslmf_TypeList9
#endif
#define bslmf_TypeList9 bslmf::TypeList9
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList10
#undef bslmf_TypeList10
#endif
#define bslmf_TypeList10 bslmf::TypeList10
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList11
#undef bslmf_TypeList11
#endif
#define bslmf_TypeList11 bslmf::TypeList11
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList12
#undef bslmf_TypeList12
#endif
#define bslmf_TypeList12 bslmf::TypeList12
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList13
#undef bslmf_TypeList13
#endif
#define bslmf_TypeList13 bslmf::TypeList13
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList14
#undef bslmf_TypeList14
#endif
#define bslmf_TypeList14 bslmf::TypeList14
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList15
#undef bslmf_TypeList15
#endif
#define bslmf_TypeList15 bslmf::TypeList15
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList16
#undef bslmf_TypeList16
#endif
#define bslmf_TypeList16 bslmf::TypeList16
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList17
#undef bslmf_TypeList17
#endif
#define bslmf_TypeList17 bslmf::TypeList17
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList18
#undef bslmf_TypeList18
#endif
#define bslmf_TypeList18 bslmf::TypeList18
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList19
#undef bslmf_TypeList19
#endif
#define bslmf_TypeList19 bslmf::TypeList19
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeList20
#undef bslmf_TypeList20
#endif
#define bslmf_TypeList20 bslmf::TypeList20
    // This alias is defined for backward compatibility.

#ifdef bslmf_TypeListTypeOf
#undef bslmf_TypeListTypeOf
#endif
#define bslmf_TypeListTypeOf bslmf::TypeListTypeOf
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
