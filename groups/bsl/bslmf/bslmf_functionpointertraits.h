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
//  bslmf::FunctionPointerTraits: meta-function for detecting function pointer
//                                traits
//  bslmf::IsFunctionPointer: meta-function to determine if a type is a
//                            function pointer
//
//@SEE_ALSO: bslmf_memberfunctionpointertraits
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides meta-functions for determining whether
// a type is a pointer to either a free function or a class method (but not to
// a member function, see the component 'bslmf_memberfunctionpointertraits'
// component for that), and some information about this function type.  The
// meta-function 'bslmf::IsFunctionPointer' provides an enumerated 'VALUE'
// which can be either 1 or 0 depending on whether or not the template argument
// 'PROTOTYPE' is a pointer to a free function or class method.  In the
// affirmative, the class 'bslmf::FunctionPointerTraits' also provides
// information regarding the function type, such as its argument list type and
// its return type.
//
// Note that there is no reference to function traits, since whether 'FUNC' is
// a reference to function type can be very easily obtained using the
// meta-function call 'bslmf::IsFunctionPointer<FUNC *>'.
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
//      assert(0 == bslmf::IsFunctionPointer<int>::VALUE);
//      assert(0 == bslmf::IsFunctionPointer<int>::VALUE);
//
//      assert(1 == bslmf::IsFunctionPointer<IntFunctionIntIntPtr>::VALUE);
//      typedef bslmf::FunctionPointerTraits<IntFunctionIntIntPtr>::ResultType
//          ResultType1;
//      assert(1 == (bslmf::IsSame<ResultType1, int>::VALUE));
//
//      assert(1 == bslmf::IsFunctionPointer<VoidFunc0>::VALUE);
//      typedef bslmf::FunctionPointerTraits<VoidFunc0>::ResultType
//          ResultType0;
//      typedef bslmf::FunctionPointerTraits<VoidFunc0>::ArgumentList
//          ArgList0;
//      assert(1 == (bslmf::IsSame<ResultType0, void>::VALUE));
//      assert(1 == (bslmf::IsSame<ArgList0, bslmf::TypeList0>::VALUE));
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

namespace BloombergLP {

namespace bslmf {

struct FunctionPointerCPlusPlusLinkage {
    // C++ function pointer linkage tag.
};

struct FunctionPointerCLinkage {
    // C function pointer linkage tag.
};

                   // ===========================
                   // class FunctionPointerTraits
                   // ===========================

template <class PROTOTYPE>
struct FunctionPointerTraits {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.

    enum { IS_FUNCTION_POINTER = 0 };
};

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_function
    : integer_constant<bool,
                       BloombergLP::bslmf::FunctionPointerTraits<TYPE>
                            ::IS_FUNCTION_POINTER>
{};

}

namespace BloombergLP {

namespace bslmf {

                   // =======================
                   // class IsFunctionPointer
                   // =======================

template <class PROTOTYPE>
struct IsFunctionPointer
: MetaInt<bsl::is_function<PROTOTYPE>::value> {
    // This template determines if the specified 'PROTOTYPE' is a free (i.e.,
    // non-member) function pointer.  VALUE is defined as 1 if the specified
    // 'PROTOTYPE' is a function pointer type, and a zero value otherwise.
};

// ---- Anything below this line is implementation specific.  Do not use. ----

// SPECIALIZATIONS
template <class BSLMF_RETURN>
struct FunctionPointerTraits<BSLMF_RETURN (*)()> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept zero arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList0                       ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept one argument.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList1<A1>                   ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept two arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList2<A1,A2>                ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept three arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList3<A1,A2,A3>             ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept four arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList4<A1,A2,A3,A4>          ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept five arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList5<A1,A2,A3,A4,A5>       ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept six arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList6<A1,A2,A3,A4,A5,A6>    ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept seven arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList7<A1,A2,A3,A4,A5,A6,A7> ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eight arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                       ResultType;
    typedef TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage    Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8, class A9>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,
                                                    A9)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept nine arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                          ResultType;
    typedef TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage       Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8, class A9,
                              class A10>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept ten arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                               ResultType;
    typedef TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage            Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eleven arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                                   ResultType;
    typedef TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage                Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept twelve arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12>
                                            ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12,
                              class A13>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12,A13)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept thirteen arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
                                            ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12,
                              class A13, class A14>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12,A13,A14)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept fourteen arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14>
                                            ArgumentList;
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslmf::FunctionPointerCPlusPlusLinkage bslmf_FunctionPointerCPlusPlusLinkage;
    // This alias is defined for backward compatibility.

typedef bslmf::FunctionPointerCLinkage bslmf_FunctionPointerCLinkage;
    // This alias is defined for backward compatibility.

#ifdef bslmf_FunctionPointerTraits
#undef bslmf_FunctionPointerTraits
#endif
#define bslmf_FunctionPointerTraits bslmf::FunctionPointerTraits
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsFunctionPointer
#undef bslmf_IsFunctionPointer
#endif
#define bslmf_IsFunctionPointer bslmf::IsFunctionPointer
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
