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
//@DESCRIPTION: This component provides meta-functions for determining whether
// a type is a pointer to either a free function or a class method (but not to
// a member function, see the component 'bslmf_memberfunctionpointertraits'
// component for that), and some information about this function type.  The
// meta-function 'bslmf::IsFunctionPointer' provides an enumerated 'value'
// which can be either 1 or 0 depending on whether or not the template argument
// 'PROTOTYPE' is a pointer to a free function or class method.  In the
// affirmative, the class 'bslmf::FunctionPointerTraits' also provides
// information regarding the function type, such as its argument list type and
// its return type.
//
// Note that there is no reference-to-function traits class, since whether
// 'FUNC' is a reference to function type can be very easily obtained using
// the meta-function call 'bslmf::IsFunctionPointer<FUNC *>'.
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
//      assert(0 == bslmf::IsFunctionPointer<int>::value);
//      assert(0 == bslmf::IsFunctionPointer<void>::value);
//
//      assert(1 == bslmf::IsFunctionPointer<IntFunctionIntIntPtr>::value);
//      typedef bslmf::FunctionPointerTraits<IntFunctionIntIntPtr>::ResultType
//          ResultType1;
//      assert(1 == (bsl::is_same<ResultType1, int>::value));
//
//      assert(1 == bslmf::IsFunctionPointer<VoidFunc0>::value);
//      typedef bslmf::FunctionPointerTraits<VoidFunc0>::ResultType
//          ResultType0;
//      typedef bslmf::FunctionPointerTraits<VoidFunc0>::ArgumentList
//          ArgList0;
//      assert(1 == (bsl::is_same<ResultType0, void>::value));
//      assert(1 == (bsl::is_same<ArgList0, bslmf::TypeList0>::value));
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
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

                   // =======================
                   // class IsFunctionPointer
                   // =======================

template <class PROTOTYPE>
struct IsFunctionPointer
: bsl::integral_constant<bool,
                      FunctionPointerTraits<PROTOTYPE>::IS_FUNCTION_POINTER> {
    // This template determines if the specified 'PROTOTYPE' is a free (i.e.,
    // non-member) function pointer.  'value' is defined as 1 if the specified
    // 'PROTOTYPE' is a function pointer type, and a zero value otherwise.
};

// ---- Anything below this line is implementation specific.  Do not use. ----

// SPECIALIZATIONS
template <class BSLMF_RETURN>
struct FunctionPointerTraits<BSLMF_RETURN (*)()> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept zero arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList0                       ArgumentList;
    typedef BSLMF_RETURN                    FuncType();
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept one argument.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList1<A1>                   ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept two arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList2<A1,A2>                ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept three arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList3<A1,A2,A3>             ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept four arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList4<A1,A2,A3,A4>          ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept five arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList5<A1,A2,A3,A4,A5>       ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept six arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList6<A1,A2,A3,A4,A5,A6>    ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept seven arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList7<A1,A2,A3,A4,A5,A6,A7> ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eight arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                       ResultType;
    typedef TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ArgumentList;
    typedef BSLMF_RETURN                       FuncType(A1,A2,A3,A4,A5,A6,A7,
                                                        A8);
    typedef FunctionPointerCPlusPlusLinkage    Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8, class A9>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,
                                                    A9)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept nine arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                          ResultType;
    typedef TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ArgumentList;
    typedef BSLMF_RETURN                          FuncType(A1,A2,A3,A4,A5,A6,
                                                           A7,A8,A9);
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
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eleven arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept twelve arguments.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11,A12);
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
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11,A12,A13);
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
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11,A12,A13,A14);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

// vararg function pointers

template <class BSLMF_RETURN>
struct FunctionPointerTraits<BSLMF_RETURN (*)(...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // whose parameter list is a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList0                       ArgumentList;
    typedef BSLMF_RETURN                    FuncType(...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept one argument followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList1<A1>                   ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept two arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList2<A1,A2>                ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept three arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList3<A1,A2,A3>             ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept four arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList4<A1,A2,A3,A4>          ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept five arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList5<A1,A2,A3,A4,A5>       ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept six arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList6<A1,A2,A3,A4,A5,A6>    ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept seven arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList7<A1,A2,A3,A4,A5,A6,A7> ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eight arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                       ResultType;
    typedef TypeList8<A1,A2,A3,A4,A5,A6,A7,A8> ArgumentList;
    typedef BSLMF_RETURN                       FuncType(A1,A2,A3,A4,A5,A6,A7,
                                                        A8...);
    typedef FunctionPointerCPlusPlusLinkage    Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8, class A9>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,
                                                    A9...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept nine arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                          ResultType;
    typedef TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9> ArgumentList;
    typedef BSLMF_RETURN                          FuncType(A1,A2,A3,A4,A5,A6,
                                                           A7,A8,A9...);
    typedef FunctionPointerCPlusPlusLinkage       Linkage;
};

template <class BSLMF_RETURN, class A1, class A2, class A3, class A4, class A5,
                              class A6, class A7, class A8, class A9,
                              class A10>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept ten arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
                                             ArgumentList;
    typedef BSLMF_RETURN                     FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                      A9,A10...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept eleven arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11>
                                             ArgumentList;
    typedef BSLMF_RETURN                     FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                      A9,A10,A11...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept twelve arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11,A12...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12,
                              class A13>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12,A13...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept thirteen arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                     A9,A10,A11,A12,A13...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

template <class BSLMF_RETURN, class A1,  class A2,  class A3,  class A4,
                              class A5,  class A6,  class A7,  class A8,
                              class A9,  class A10, class A11, class A12,
                              class A13, class A14>
struct FunctionPointerTraits<BSLMF_RETURN (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                    A10,A11,A12,A13,A14...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept fourteen arguments followed by a C-style vararg parameter pack.

    enum { IS_FUNCTION_POINTER = 1 };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                    ResultType;
    typedef TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14>
                                            ArgumentList;
    typedef BSLMF_RETURN                    FuncType(A1,A2,A3,A4,A5,A6,A7,A8,
                                                    A9,A10,A11,A12,A13,A14...);
    typedef FunctionPointerCPlusPlusLinkage Linkage;
};

#if defined(BSLS_PLATFORM_CMP_MSVC)
// Microsoft Visual C++ has a problem matching 'T * const' pointers to the
// appropriate partial specialization, whereas it will match a 'T const'
// specialization.  We could use the Microsoft fix on all platforms, but why
// instantiate more traits than necessary when testing pointer traits of
// cv-qualified types that are not function pointers?

template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE const>
     : FunctionPointerTraits<PROTOTYPE> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};

template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE volatile>
     : FunctionPointerTraits<PROTOTYPE> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};

template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE const volatile>
     : FunctionPointerTraits<PROTOTYPE> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};
#else
template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE * const>
     : FunctionPointerTraits<PROTOTYPE *> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};

template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE * volatile>
     : FunctionPointerTraits<PROTOTYPE *> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};

template <class PROTOTYPE>
struct FunctionPointerTraits<PROTOTYPE * const volatile>
     : FunctionPointerTraits<PROTOTYPE *> {
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
};
#endif

}  // close package namespace


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
