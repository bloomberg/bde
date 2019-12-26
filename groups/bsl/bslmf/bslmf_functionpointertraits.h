// bslmf_functionpointertraits.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#define INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS

#include <bsls_ident.h>
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
// 'FUNC' is a reference to function type can be very easily obtained using the
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

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_typelist.h>

#include <bsls_compilerfeatures.h>

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES) &&                  \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    // All of our compilers which identify 'noexcept' as part of the type
    // system (a C++17 piece of functionality) similarly also support variadic
    // templates, so we refrain from having the dead code to support this case.
#   error Feature not supported for compilers without variadic templates
#endif

                // ---------------------------
                // class FunctionPointerTraits
                // ---------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept a fixed number of arguments

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...,...)> {
    // Specialization for function pointers that return 'BSLMF_RETURN' and
    // accept variable (C-style varargs) number of arguments

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslmf_functionpointertraits.h
#ifndef BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#define BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT 14
#endif
#ifndef BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A
#define BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A                          \
        BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#endif

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0
template <class BSLMF_RETURN>
struct FunctionPointerTraits<BSLMF_RETURN (*)()> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType();
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1
template <class BSLMF_RETURN, class ARGS_01>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12,
                              class ARGS_13>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12,
                                              ARGS_13)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12,
                                                      ARGS_13);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12,
                              class ARGS_13,
                              class ARGS_14>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12,
                                              ARGS_13,
                                              ARGS_14)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12,
                                                      ARGS_13,
                                                      ARGS_14);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14


#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0
template <class BSLMF_RETURN>
struct FunctionPointerTraits<BSLMF_RETURN (*)(...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1
template <class BSLMF_RETURN, class ARGS_01>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12,
                              class ARGS_13>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12,
                                              ARGS_13,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12,
                                                      ARGS_13,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13

#if BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14
template <class BSLMF_RETURN, class ARGS_01,
                              class ARGS_02,
                              class ARGS_03,
                              class ARGS_04,
                              class ARGS_05,
                              class ARGS_06,
                              class ARGS_07,
                              class ARGS_08,
                              class ARGS_09,
                              class ARGS_10,
                              class ARGS_11,
                              class ARGS_12,
                              class ARGS_13,
                              class ARGS_14>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10,
                                              ARGS_11,
                                              ARGS_12,
                                              ARGS_13,
                                              ARGS_14,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS_01,
                                                      ARGS_02,
                                                      ARGS_03,
                                                      ARGS_04,
                                                      ARGS_05,
                                                      ARGS_06,
                                                      ARGS_07,
                                                      ARGS_08,
                                                      ARGS_09,
                                                      ARGS_10,
                                                      ARGS_11,
                                                      ARGS_12,
                                                      ARGS_13,
                                                      ARGS_14,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};
#endif  // BSLMF_FUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14

#else   // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...,...)> {

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 0
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...,...);
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

// }}} END GENERATED CODE
#endif  // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

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
// }}} END GENERATED CODE
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...) noexcept> {
    // Specialization for 'noexcept' function pointers that return
    // 'BSLMF_RETURN' and accept a fixed number of arguments

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 1
    };
    enum { e_IS_VARARG = 0 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...) noexcept;
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

template <class BSLMF_RETURN, class...ARGS>
struct FunctionPointerTraits<BSLMF_RETURN (*)(ARGS...,...) noexcept> {
    // Specialization for 'noexcept' function pointers that return
    // 'BSLMF_RETURN' and accept variable (C-style varargs) number of arguments

    enum {
        IS_FUNCTION_POINTER = 1,
        IS_NOEXCEPT = 1
    };
    enum { e_IS_VARARG = 1 };
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
    typedef BSLMF_RETURN                     FuncType(ARGS...,...) noexcept;
    typedef FunctionPointerCPlusPlusLinkage  Linkage;
};

#endif // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslmf::FunctionPointerCPlusPlusLinkage
                                         bslmf_FunctionPointerCPlusPlusLinkage;
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
