// bslmf_functionpointertraits.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#define INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining function pointer traits.
//
//@CLASSES:
//   bslmf_FunctionPointerTraits: meta-function for detecting function pointer
//                                traits
//       bslmf_IsFunctionPointer: meta-function to determine if a type is a
//                                function pointer
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bslmf_memberfunctionpointertraits
//
//@DESCRIPTION: This component provides meta-functions for determining whether
// a type is a pointer to either a free function or a class method (but not to
// a member function, see the component 'bslmf_memberfunctionpointertraits'
// component for that), and some information about this function type.  The
// meta-function 'bslmf_IsFunctionPointer' provides an enumerated 'VALUE' which
// can be either 1 or 0 depending on whether or not the template argument
// 'PROTOTYPE' is a pointer to a free function or class method.  In the
// affirmative, the class 'bslmf_FunctionPointerTraits' also provides
// information regarding the function type, such as its argument list type and
// its return type.
//
// Note that there is no reference to function traits, since whether 'FUNC' is
// a reference to function type can be very easily obtained using the
// meta-function call 'bslmf_IsFunctionPointer<FUNC *>'.
//
///Usage
///-----
// Define the following function types:
//..
//  typedef int  (*IntFunctionIntIntPtr)(int, int);
//  typedef void (*VoidFunc0)();
//..
// The following program should compile and run without errors:
//..
//  int main()
//  {
//      assert(0 == bslmf_IsFunctionPointer<int>::VALUE);
//      assert(0 == bslmf_IsFunctionPointer<int>::VALUE);
//
//      assert(1 == bslmf_IsFunctionPointer<IntFunctionIntIntPtr>::VALUE);
//      typedef bslmf_FunctionPointerTraits<IntFunctionIntIntPtr>::ResultType
//                                                                 ResultType1;
//      assert(1 == (bslmf_IsSame<ResultType1, int>::VALUE));
//
//      assert(1 == bslmf_IsFunctionPointer<VoidFunc0>::VALUE);
//      typedef bslmf_FunctionPointerTraits<VoidFunc0>::ResultType ResultType0;
//      typedef bslmf_FunctionPointerTraits<VoidFunc0>::ArgumentList  ArgList0;
//      assert(1 == (bslmf_IsSame<ResultType0, void>::VALUE));
//      assert(1 == (bslmf_IsSame<ArgList0, bslmf_TypeList0>::VALUE));
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

namespace BloombergLP {

struct bslmf_FunctionPointerCPlusPlusLinkage {
    // C++ function pointer linkage tag.
};

struct bslmf_FunctionPointerCLinkage {
    // C function pointer linkage tag.
};

                   // =============================
                   // class bslmf_IsFunctionPointer
                   // =============================

template <class PROTOTYPE>
struct bslmf_FunctionPointerTraits {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.

    enum { IS_FUNCTION_POINTER = 0 };
};

                   // =============================
                   // class bslmf_IsFunctionPointer
                   // =============================

template <class PROTOTYPE>
struct bslmf_IsFunctionPointer {
    // This template determines if the specified 'PROTOTYPE' is a free (i.e.,
    // non-member) function pointer.  VALUE is defined as 1 if the specified
    // 'PROTOTYPE' is a function pointer type, and a zero value otherwise.

    enum {
        VALUE = bslmf_FunctionPointerTraits<PROTOTYPE>::IS_FUNCTION_POINTER
    };
};

// ---- Anything below this line is implementation specific.  Do not use. ----

// SPECIALIZATIONS
template <class RET>
struct bslmf_FunctionPointerTraits<RET (*)()> {
    // Specialization for function pointers that return 'RET' and accept zero
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList0 ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1>
struct bslmf_FunctionPointerTraits<RET (*)(A1)> {
    // Specialization for function pointers that return 'RET' and accept one
    // argument.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList1<A1> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2)> {
    // Specialization for function pointers that return 'RET' and accept two
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList2<A1,A2> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3)> {
    // Specialization for function pointers that return 'RET' and accept three
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList3<A1,A2,A3> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4)> {
    // Specialization for function pointers that return 'RET' and accept four
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList4<A1,A2,A3,A4> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5)> {
    // Specialization for function pointers that return 'RET' and accept five
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList5<A1,A2,A3,A4,A5> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6)> {
    // Specialization for function pointers that return 'RET' and accept six
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList6<A1,A2,A3,A4,A5,A6> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7)> {
    // Specialization for function pointers that return 'RET' and accept seven
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList7<A1,A2,A3,A4,A5,A6,A7> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8)> {
    // Specialization for function pointers that return 'RET' and accept eight
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9)> {
    // Specialization for function pointers that return 'RET' and accept nine
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10)> {
    // Specialization for function pointers that return 'RET' and accept ten
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10,class A11>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10,A11)> {
    // Specialization for function pointers that return 'RET' and accept eleven
    // arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10,A11,A12)> {
    // Specialization for function pointers that return 'RET' and accept
    // twelve arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12>
        ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12, class A13>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10,A11,A12,A13)> {
    // Specialization for function pointers that return 'RET' and accept
    // thirteen arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
        ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12, class A13, class A14>
struct bslmf_FunctionPointerTraits<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10,A11,A12,A13,A14)> {
    // Specialization for function pointers that return 'RET' and accept
    // fourteen arguments.
    enum { IS_FUNCTION_POINTER = 1 };
    typedef RET ResultType;
    typedef bslmf_TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14>
        ArgumentList;
    typedef bslmf_FunctionPointerCPlusPlusLinkage Linkage;
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
