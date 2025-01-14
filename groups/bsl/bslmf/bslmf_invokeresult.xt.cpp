// bslmf_invokeresult.xt.cpp                                          -*-C++-*-
#include <bslmf_invokeresult.h>

#include <bsla_fallthrough.h>
#include <bsla_maybeunused.h>

#include <bslmf_integralconstant.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>
#include <bslmf_removepointer.h>
#include <bslmf_voidtype.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <utility>   // `std::move` for C++11

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

#include <bsls_compilerfeatures.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Tue Jan 14 14:12:51 2025
// Command line: sim_cpp11_features.pl bslmf_invokeresult.xt.cpp

# define COMPILING_BSLMF_INVOKERESULT_XT_CPP
# include <bslmf_invokeresult_cpp03.xt.cpp>
# undef COMPILING_BSLMF_INVOKERESULT_XT_CPP

// clang-format on
#else

#if defined(BSLS_PLATFORM_CMP_SUN)
    #pragma error_messages(off, functypequal)
#endif

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
    #pragma GCC diagnostic ignored "-Wvolatile"
    #ifdef BSLS_PLATFORM_CMP_CLANG
        #pragma GCC diagnostic ignored "-Wdeprecated-volatile"
    #endif
#endif

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//@bdetdsplit PARTS (syntax version 1.0.0)
//@
//@# This test driver will be split into multiple parts for faster compilation
//@# using bde_xt_cpp_splitter.py from the bde-tools repo.  Each line below
//@# controls which test cases from this file will be included in one (or more)
//@# standalone test drivers.  Specific contents of each part can be further
//@# controlled by //@bdetdsplit comments throughout this file, for which full
//@# documentation can be found by running:
//@#    bde_xt_cpp_splitter --help usage-guide
//@
//@  CASES: 1
//@  CASES: 2
//@  CASES: 3
//@  CASES: 4
//@  CASES: 5
//@  CASES: 6
//@  CASES: 7
//@  CASES: 8
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component is a transformation metafunction.  Testing consists primarily
// of instantiating `bsl::invoke_result<FN, Args...>` with representative
// combinations of `Fn` and `Args` and verifying that the resulting `type` is
// what was expected.  The test is broken up as one test case per category of
// invocable `Fn` parameter.
//-----------------------------------------------------------------------------
// [2] FUNCTION INVOCABLES
// [3] POINTER-TO-MEMBER-FUNCTION INVOCABLES
// [4] POINTER-TO-MEMBER-OBJECT INVOCABLES
// [5] FUNCTOR CLASS INVOCABLES
// [6] FUNCTION: InvokeResultDeductionFailed
// [7] C++17 SEMANTICS
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [8] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}
}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define MSVC 1
#else
#define MSVC 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1800
#define MSVC_2013 1
#else
#define MSVC_2013 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1900
#define MSVC_2015 1
#else
#define MSVC_2015 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)   \
 && BSLS_PLATFORM_CMP_VERSION >= 1910 \
 && BSLS_PLATFORM_CMP_VERSION <= 1916
#define MSVC_2017 1
#else
#define MSVC_2017 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)   \
 && BSLS_PLATFORM_CMP_VERSION >= 1920 \
 && BSLS_PLATFORM_CMP_VERSION <= 1929
#define MSVC_2019 1
#else
#define MSVC_2019 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)   \
 && BSLS_PLATFORM_CMP_VERSION >= 1930 \
 && BSLS_PLATFORM_CMP_VERSION <= 1940
#define MSVC_2022 1
#else
#define MSVC_2022 0
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 90000
// Prior to version 9, GCC does not retroactively apply Core Working Group
// resolution 616, which applies to C++11 and later.  This resolution, among
// other things not relevant to this test driver, changes the algorithm used to
// determine the value category of pointer-to-member operator expressions,
// which affects the reference qualification (or lack thereof) of the type
// of `bsl::invoke_result` for pointers to data members.  Prior to the
// resolution of CWG616, the value category of a pointer-to-member operator
// expression was the same as the value category of its left-hand side -- that
// of the object.  After the resolution of CWG616, such an expression has
// lvalue category if its left-hand side is an lvalue, and has xvalue category
// otherwise.
#   define GCC_PRE_CWG616_RESOLUTION_SEMANTICS
#endif //defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 90000

              // ===========================================
              // Set macros to indicate MSVC bug workarounds
              // ===========================================

#if defined(BSLS_PLATFORM_CMP_MSVC)
// The MSVC 18.0 and 19.0 compilers have a bug whereby an expression like,
// `func().*ptrtomem`, where `func()` returns `volatile SomeClass&&` and
// `ptrtomem` is a declared as `RET SomeClass::*ptrtomem`, yields an lvalue
// instead of an rvalue reference. Defining this macro will disable certain
// tests that would fail using the MSVC compiler.
#   define MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG

// These compilers also have a bug whereby similar expressions where `func()`
// returns a const- and/or volatile-qualified class type, and `ptrtomem` is
// declared as a `RET SomeClass::*ptrtomem` yields an expression of
// reference-to-`RET` type lacking the appropriate qualifiers if `RET` is an
// array type.
#   define MSVC_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG

#   if BSLS_PLATFORM_CMP_VERSION == 1900
    // MSVC 2015 has compiler version number 1900.  In this version of MSVC,
    // the above mentioned defect affecting pointer-to-member dereference
    // expressions extends to expressions on *unqualified* class types.
#       define MSVC_2015_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
#   endif
#endif //BSLS_PLATFORM_CMP_MSVC

                  // ===========================================
                  // some compilers require abridged tests
                  // (applies even when the test has been split)
                  // ===========================================

#if defined(BSLS_PLATFORM_CMP_IBM) || defined(BSLS_PLATFORM_CMP_SUN)
# define BSLMF_INVOKERESULT_ABRIDGED_TEST 1
    // Use Abridged set of tests for xlC and Sun CC 5.12. These compilers
    // get overwhelmed with too many template instantiations. They will work
    // fine for a real program, but fail for test programs like this that
    // recursively create hundreds of instantiations. The abridged tests lose
    // a bit of redundancy, but are probably adaquate for testing the
    // component, especially when combined with the full tests on other
    // platforms.
#else
# define BSLMF_INVOKERESULT_ABRIDGED_TEST 0
#endif

                      // ================================
                      // class template reference_wrapper
                      // ================================

//@bdetdsplit FOR 3,7 BEGIN
namespace {

/// This class template provides a stub implementation of
/// `bsl::reference_wrapper` with just enough real interface to allow code
/// within this test driver to compile.  The real `reference_wrapper` is
/// not available, as it is in a higher-level component than this one.
template <class TP>
class stub_reference_wrapper {

  public:
    // ACCESSORS

    /// This member operator is declared but not defined.
    operator TP&() const BSLS_KEYWORD_NOEXCEPT;

    /// This member function is declared but not defined.
    TP& get() const BSLS_KEYWORD_NOEXCEPT;
};

}  // close unnamed namespace


namespace BloombergLP {
namespace bslmf {

template <class TP>
struct IsReferenceWrapper<stub_reference_wrapper<TP> > : bsl::true_type {
};

}  // close package namespace
}  // close enterprise namespace
//@bdetdsplit FOR 3,7 END

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

namespace {

                       // ===============================
                       // struct template IsInvokeResult
                       // ===============================

//@bdetdsplit FOR 2,3,4,7 BEGIN

/// This metafunction `struct` provides a compile-time constant, Boolean
/// data member `value` that is `true` if 'bsl::invoke_result<INVOCABLE,
/// ARGS...>::type' is the same as the specified `EXPECTED_INVOKE_RESULT`
/// type, and is `false` otherwise.  Compilation will fail unless
/// `bsl::invoke_result<INVOCABLE, ARGS...>` provides a nested typedef
/// `type`.
template <class EXPECTED_INVOKE_RESULT, class INVOCABLE, class... ARGS>
struct IsInvokeResult
: bsl::is_same<EXPECTED_INVOKE_RESULT,
               typename bsl::invoke_result<INVOCABLE, ARGS...>::type> {
};
//@bdetdsplit FOR 2,3,4,7 END

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

                       // ===============================
                       // struct template IsInvokeResultT
                       // ===============================

//@bdetdsplit FOR 2,3,4,5 BEGIN

/// This metafunction `struct` provides a compile-time constant, Boolean
/// data member `value` that is `true` if 'bsl::invoke_result<INVOCABLE,
/// ARGS...>::type' is the same as the
/// `bsl::invoke_result_t<INVOCABLE, ARGS...>` type, and is `false`
/// otherwise.  Compilation will fail unless
/// `bsl::invoke_result<INVOCABLE, ARGS...>` provides a nested typedef
/// `type`.
template <class INVOCABLE, class... ARGS>
struct IsInvokeResultT
: bsl::is_same<typename bsl::invoke_result  <INVOCABLE, ARGS...>::type,
                        bsl::invoke_result_t<INVOCABLE, ARGS...> > {
};
//@bdetdsplit FOR 2,3,4,5 END

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close unnamed namespace

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5

                   // =====================================
                   // struct template IsInvokeResultDefined
                   // =====================================

//@bdetdsplit FOR 2,3,4,7 BEGIN
namespace {

/// forward declaration
template <class VOID, class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp;

/// This metafunction `struct` provides a compile-time constant, Boolean
/// data member `value` that is `true` if 'bsl::invoke_result<INVOCABLE,
/// ARGS...>` provides a nested typedef `type', and is `false` otherwise.
template <class INVOCABLE, class... ARGS>
struct IsInvokeResultDefined
: IsInvokeResultDefinedImp<void, INVOCABLE, ARGS...> {
};

                  // ========================================
                  // struct template IsInvokeResultDefinedImp
                  // Required for test cases 2,3,4,7
                  // ========================================

template <class VOID, class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp : bsl::false_type {
};

template <class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp<
    typename bslmf::VoidType<
        typename bsl::invoke_result<INVOCABLE, ARGS...>::type>::type,
    INVOCABLE,
    ARGS...> : bsl::true_type {
};
}  // close unnamed namespace
//@bdetdsplit FOR 2,3,4,7 END

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5

                        // ============================
                        // class SfinaeFriendlinessTest
                        // ============================

//@bdetdsplit FOR 7 BEGIN
namespace {

class SfinaeFriendlinessTest {
  public:
    // CREATORS

    /// Simple user-provided constructor.
    SfinaeFriendlinessTest() {}

    // ACCESSORS

    /// Check whether IsInvokeResult is defined or not depending on
    /// specified parameter `LINE`
    template <bool IS_TYPE_DEFINED, class INVOCABLE, class... ARGS>
    void run(int LINE) const
    {
        if (IS_TYPE_DEFINED) {
            ASSERTV(LINE, (IsInvokeResultDefined<INVOCABLE, ARGS...>::value));
        }
        else {
            ASSERTV(LINE, (!IsInvokeResultDefined<INVOCABLE, ARGS...>::value));
        }
    }
};
}  // close unnamed namespace
//@bdetdsplit FOR 7 END

#endif

namespace {

// 15 distinct types

//@bdetdsplit FOR 2,3,4 BEGIN
typedef bsl::integral_constant<int,  0> Ic00;
typedef bsl::integral_constant<int,  1> Ic01;
typedef bsl::integral_constant<int,  2> Ic02;
typedef bsl::integral_constant<int,  3> Ic03;
typedef bsl::integral_constant<int,  4> Ic04;
typedef bsl::integral_constant<int,  5> Ic05;
typedef bsl::integral_constant<int,  6> Ic06;
typedef bsl::integral_constant<int,  7> Ic07;
typedef bsl::integral_constant<int,  8> Ic08;
typedef bsl::integral_constant<int,  9> Ic09;
typedef bsl::integral_constant<int, 10> Ic10;
typedef bsl::integral_constant<int, 11> Ic11;
typedef bsl::integral_constant<int, 12> Ic12;
typedef bsl::integral_constant<int, 13> Ic13;
typedef bsl::integral_constant<int, 14> Ic14;
//@bdetdsplit FOR 2,3,4 END

//@bdetdsplit FOR 1,4,5 BEGIN

/// An object of type `MetaType<TP>` can represent `TP` at run-time without
/// actually creating an instance of `TP`.  Note that `TP` can be a
/// reference and/or cv-qualified.
template <class TP>
struct MetaType {

    typedef TP type;
};
//@bdetdsplit FOR 1,4,5 END

//@bdetdsplit FOR 2..6 BEGIN
enum MyEnum {
    // An enumeration type, for testing invocables that return enumerations.
    MY_ENUMERATOR
};
//@bdetdsplit FOR 2..6 END

//@bdetdsplit FOR 1..7 BEGIN

/// Simple class type that can be returned by value and to which member
/// variables and functions that can be referenced via pointer-to-member.
struct MyClass {

    char *d_str_p;

    /// Declared but not defined.
    int foo();

    /// Declared but not defined.
    MyClass() : d_str_p(0) {}
    MyClass(MyClass&);
    MyClass(const MyClass&);
    MyClass(volatile MyClass&);
    MyClass(const volatile MyClass&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    /// Declared but not defined.
    MyClass(MyClass&&);
    MyClass(const MyClass&&);
    MyClass(volatile MyClass&&);
    MyClass(const volatile MyClass&&);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
};
//@bdetdsplit FOR 1..7 END

//@bdetdsplit FOR 2..5, 7 BEGIN

/// A class derived from MyClass, for testing pointer-to-base-member.
struct MyDerivedClass : MyClass {
};
//@bdetdsplit FOR 2..5, 7 END

//@bdetdsplit FOR 3,4,7 BEGIN

/// This template has the interface of a smart pointer.  Specifically, the
/// dereference operator returns a reference to `TP`.
template <class TP>
struct SmartPtr {

    /// Pointer-like operators (declared but not defined).
    TP& operator*() const volatile;
    TP* operator->() const volatile;
};
//@bdetdsplit FOR 3,4,7 END

//@bdetdsplit FOR 2,3,7 BEGIN

/// Objects of this class are convertible to long long.
struct ToLongLong
{

    /// Convertion to `long long` (declared but not defined).
    operator long long() const;
};
//@bdetdsplit FOR 2,3,7 END

//@bdetdsplit FOR 1,4,5 BEGIN

/// Generic functor class with many overloads of `operator()`.
struct ManyFunc {

    /// Invocation operators for breathing test (declared but not defined)
    bool operator()(float, void*);
    float operator()(float, void*) const;

    /// Invocation overloads based on types and numbers of arguments
    /// (declared but not defined)
    bool  operator()();
    short operator()(int);
    int   operator()(int, float);
    long  operator()(int, float, short, int, int, int, int, int, int, float);
    unsigned int   operator()(int,   int);
    unsigned short operator()(short, int);

    /// Invocation overloads based on `const` and `volatile` (declared but
    /// not defined)
    bool&  operator()(int, int, int);
    short& operator()(int, int, int) const;
    int&   operator()(int, int, int) volatile;
    long&  operator()(int, int, int) const volatile;

#if defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_IBM)
    // sun and xlC do not correctly strip top-level cv qualifiers from return
    // types.
    template <class TP>
    typename bsl::remove_cv<TP>::type operator()(MetaType<TP>, ...);
#else
    template <class TP>
    TP operator()(MetaType<TP>, ...);
#endif
    /// Return type deduced from first argument (declared but not defined)
    template <class TP>
    typename bsl::add_const<TP>::type operator()(MetaType<TP>, ...) const;

    /// Return type is `TP`.  `invoke_result` is specialized for this case
    /// (declared but not defined)
    template <class TP>
    TP operator()(char*, MetaType<TP>);

    typedef void *P;
    /// Invocation overloads for 1 to 10 pointer arguments (declared but
    /// not defined)
    char        operator()(P);
    short       operator()(P,P);
    int         operator()(P,P,P);
    long        operator()(P,P,P,P);
    long long   operator()(P,P,P,P,P);
    float       operator()(P,P,P,P,P,P);
    double      operator()(P,P,P,P,P,P,P);
    long double operator()(P,P,P,P,P,P,P,P);
    const char *operator()(P,P,P,P,P,P,P,P,P);
    void       *operator()(P,P,P,P,P,P,P,P,P,P);
};
//@bdetdsplit FOR 1,4,5 END

//@bdetdsplit FOR 5,7 BEGIN

/// Class that is convertible to pointer-to-function or
/// reference-to-function, depending on the overload used.
struct ConvertibleToFunc {

    typedef bool    (*boolFPtr)();
    typedef short   (*shortFPtr)(int);
    typedef int*    (&intStarFRef)(int, float);
    typedef MyClass (&udtFRef)(int, float, short);

    /// Conversion to pointer to function returning bool.
    operator boolFPtr() const;

    /// Conversion to pointer to function taking one argument.
    operator shortFPtr() const;

    /// Conversion to reference to function returning pointer.
    operator intStarFRef() const;

    /// Conversion to reference to function returning pointer.
    operator udtFRef() const;
};

/// Functor type with two overloads of `operator()`: one that returns
/// `result_type`, which is an alias for `RT`, and another overload that
/// always returns `int`, regardless of `result_type`.
template <class RT>
struct FuncRt1 {

    typedef RT result_type; // Used by `invoke_result` to determine return type

    /// Used to test return type can be deduced via `result_type` (declared
    /// but not defined)
    result_type operator()(const char*);

    /// Used to test that deduction of fundamental type `int` supercedes
    /// `result_type` (declared but not defined)
    int operator()(int);
};

/// Functor type with two overloads of `operator()`, one that returns
/// `ResultType`, which is an alias for `RT`, and another overload that
/// always returns `int`, regardless of `ResultType`.
template <class RT>
struct FuncRt2 {

    typedef RT ResultType; // Used by `invoke_result` to determine return type

    /// Used to test return type can be deduced via `ResultType` (declared
    /// but not defined)
    ResultType operator()(const char*);

    /// Used to test that deduction of fundamental type `int` supercedes
    /// `ResultType` (declared but not defined)
    int operator()(int);
};

/// Functor type with two overloads of `operator()`, one that returns
/// `result_type`, which is an alias for `RT`, and another overload that
/// always returns `int`, regardless of `result_type`.  This struct also
/// defines `ResultType`, which should be ignored in preference to
/// `result_type`.
template <class RT>
struct FuncRt3 {

    typedef RT   result_type; // Used by `invoke_result` to get return type
    typedef void ResultType;  // Ignored by `invoke_result`.

    /// Used to test return type can be deduced via `result_type` (declared
    /// but not defined)
    result_type operator()(const char*) const;

    /// Used to test that deduction of fundamental type `int` supercedes
    /// `result_type` (declared but not defined)
    int operator()(int) const;
};
//@bdetdsplit FOR 5,7 END

//@bdetdsplit FOR 2..5 BEGIN

/// Used by `applyPtrAndRef` to apply the specified `TEST_KERNEL` template
/// parameter to each of `RT`, `RT&`, `const RT&`, `volatile RT&`,
/// `const volatile RT&`, and `RT&&`.  Requires: `TEST_KERNEL` is a class
/// type with an `apply<TP>()` static member that tests `invoke_result` on
/// invocables that return `TP`.
template <class TEST_KERNEL, class RT>
struct ApplyRef {

    /// Call, using the specified `LINE`, `TEST_KERNEL::apply<RT>(LINE)` and
    /// `TEST_KERNEL::apply<cv RT&>(LINE)`, where `cv` is all four
    /// combinations of `const` and `volatile`.  On compilers that support
    /// rvalue references, also call `TEST_KERNEL::apply<RT&&>(LINE)`.
    static void apply(int LINE)
    {
        TEST_KERNEL::template apply<RT>(LINE);
        TEST_KERNEL::template apply<RT&>(LINE);
        TEST_KERNEL::template apply<const RT&>(LINE);
        TEST_KERNEL::template apply<volatile RT&>(LINE);
        TEST_KERNEL::template apply<const volatile RT&>(LINE);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCE
# ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#  if !MSVC_2013 // MSVC 2013 does not have a sufficiently functional
                 // implementation of expression sfinae for
                 // `bslmf_invokeresult` to make use of `decltype`.  This means
                 // that `bslmf_invokeresult` cannot support
                 // rvalue-reference-qualified types on MSVC 2013.
        TEST_KERNEL::template apply<RT&&>(LINE);
        TEST_KERNEL::template apply<const RT&&>(LINE);
        TEST_KERNEL::template apply<volatile RT&&>(LINE);
        TEST_KERNEL::template apply<const volatile RT&&>(LINE);
#  endif
# else
#  error "Rvalue refs without `decltype` not supported by `bsl::invoke_result`"
# endif
#endif
    }
};

/// For the specified `TEST_KERNEL` template parameter, apply `TEST_KERNEL`
/// such that it tests invocations that should return `void`.  This
/// specialization avoids trying to create a reference-to-void.
template <class TEST_KERNEL>
struct ApplyRef<TEST_KERNEL, void> {

    /// Call, using the specified `LINE`, `TEST_KERNEL::apply<void>(LINE)`.
    static void apply(int LINE)
    {
        TEST_KERNEL::template apply<void>(LINE);
    }
};

/// Call, using the specified `LINE`, `TEST_KERNEL::apply<X>(LINE)`, where
/// `X` is each of the following: the specified `RT` template parameter, a
/// pointer to (cv-qualified) `RT`, a reference to cv-qualified of `RT`, and
/// a reference to a cv-qualified pointer to cv-qualified `RT` (for every
/// combination of cv qualifications).  Requires: `TEST_KERNEL` is a class
/// type with an `apply<TP>()` static member that tests `invoke_result` on
/// invocables that return `TP`.
template <class TEST_KERNEL, class RT>
static void applyPtrAndRef(int LINE)
{
    ApplyRef<TEST_KERNEL, RT>::apply(LINE);
    ApplyRef<TEST_KERNEL, RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, const RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, volatile RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, const volatile RT*>::apply(LINE);
}
//@bdetdsplit FOR 2..5 END

}  // close unnamed namespace


                        // ===========================================
                        // Macros to enable template pre-instantiation
                        // ===========================================

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macros enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

// Firstly the macros for ApplyRef
#define PREINSTANTIATE_AR_IMPL2(prefix, name, ...) \
    void dummyFunction##prefix##_##name(int line) \
    { \
        ApplyRef< __VA_ARGS__ >::apply(line); \
    }

#define PREINSTANTIATE_AR_IMPL(prefix, line, ...) \
    PREINSTANTIATE_AR_IMPL2(prefix, line, __VA_ARGS__)

#define PREINSTANTIATE_AR(...) PREINSTANTIATE_AR_IMPL(0, L_, __VA_ARGS__)

// Secondly the macros for applyPtrAndRef
#define PREINSTANTIATE_APR_IMPL2(prefix, name, ...) \
    void dummyFunction##prefix##_##name() \
    { \
        applyPtrAndRef< __VA_ARGS__ >(0); \
    }

#define PREINSTANTIATE_APR_IMPL(prefix, line, ...) \
    PREINSTANTIATE_APR_IMPL2(prefix, line, __VA_ARGS__)

#define PREINSTANTIATE_APR(...) PREINSTANTIATE_APR_IMPL(0, L_, __VA_ARGS__)

// The following macro is equivalent to calling PREINSTANTIATE_APR repeatedly
// to cover all of the basic types.
#define PREINSTANTIATE_APR_FOR_FUNDAMENTAL_TYPES(...) \
    PREINSTANTIATE_APR_IMPL(_void, L_, __VA_ARGS__, void) \
    PREINSTANTIATE_APR_IMPL(_int, L_, __VA_ARGS__, int) \
    PREINSTANTIATE_APR_IMPL(_float, L_, __VA_ARGS__, float) \
    PREINSTANTIATE_APR_IMPL(Ic01, L_, __VA_ARGS__, Ic01) \
    PREINSTANTIATE_APR_IMPL(Cint, L_, __VA_ARGS__, const int) \
    PREINSTANTIATE_APR_IMPL(Vshort,L_,  __VA_ARGS__, volatile short) \
    PREINSTANTIATE_APR_IMPL(CVenum, L_, __VA_ARGS__, const volatile MyEnum)

#ifndef MSVC_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
    #define PREINSTANTIATE_APR_FOR_BASIC_TYPES(...)                           \
        PREINSTANTIATE_APR_FOR_FUNDAMENTAL_TYPES(__VA_ARGS__)                 \
        PREINSTANTIATE_APR_IMPL(Arry, L_, __VA_ARGS__, int[10][5])
#else
    #define PREINSTANTIATE_APR_FOR_BASIC_TYPES(...)                           \
        PREINSTANTIATE_APR_FOR_FUNDAMENTAL_TYPES(__VA_ARGS__)
#endif

                            // ===============
                            // struct FuncTest
                            // ===============
//@bdetdsplit FOR 2 BEGIN

/// A test kernel, intended as the `TEST_KERNEL` template argument to
/// `applyPtrAndRef`, which tests `invoke_result<Fn, Args...>`, where `Fn`
/// is a function type, pointer-to-function type, reference to function
/// type, or reference to cv-qualified pointer to function type.
struct FuncTest {

    template <class RT>
    static void apply(int LINE)
    {
        apply<RT, RT>(LINE);
    }

    /// Instantiate `invoke_result` for function type `RT(int)` and pointers
    /// and references to that function type.  Verify that it yields `RT` as
    /// the result type.  Uses `ASSERTV` to show the specified `LINE`.
    template <class RT, class EXPECTED_RT>
    static void apply(int LINE)
    {
        typedef RT Fn(int);

        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *const&, char>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn&&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *&&, char>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        // Testing `bsl::invoke_result_t`.

        ASSERTV(LINE, (IsInvokeResultT<Fn,         char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fn&,        char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fn *,       char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fn *&,      char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fn *const&, char>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResultT<Fn&&,       char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fn *&&,     char>::value));
#endif

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    }
};

// ============================================================================
//                      TEMPLATE PREINSTANTIATIONS
// ----------------------------------------------------------------------------

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macro calls enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

PREINSTANTIATE_APR(FuncTest, void);
PREINSTANTIATE_APR(FuncTest, int);
PREINSTANTIATE_APR(FuncTest, float);
PREINSTANTIATE_APR(FuncTest, MyClass);
PREINSTANTIATE_APR(FuncTest, MyEnum);
//@bdetdsplit FOR 2 END

//@bdetdsplit FOR 3 BEGIN

/// A test kernel, intended as the `TEST_KERNEL` template argument to
/// `applyPtrAndRef`, which tests `invoke_result<Fn, Args...>`, where `Fn`
/// is a pointer-to-member-function type, or reference to cv-qualified
/// pointer-to-member-function type.
template <class T1 = MyClass>
struct PtrToMemFuncTest {

    template <class RT>
    static void apply(int LINE)
    {
        apply<RT, RT>(LINE);
    }

    /// Instantiate `invoke_result` for function type
    /// `RT (MyClass::*Fp)(int);` and references to that function type.
    ///
    /// Verify that it yields `RT` as the result type.  The specified `LINE`
    /// is passed into the `ASSERTV` calls.
    template <class RT, class EXP>
    static void apply(int LINE)
    {
        typedef RT (MyClass::*Fp)(int);

        ASSERTV(LINE, (IsInvokeResult<EXP, Fp, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp&, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp const&, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp volatile&, T1, char>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, Fp const volatile&, T1, char>::value));

        typedef RT (MyClass::*CFp)(int) const;
        typedef RT (MyClass::*VFp)(int) volatile;
        typedef RT (MyClass::*CVFp)(int) const volatile;

        ASSERTV(LINE, (IsInvokeResult<EXP, CFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CFp, const T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VFp, volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, const T1, short>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, CVFp, const volatile T1, short>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp&&, T1, char>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // Test ref-qualified member-function-pointers
        typedef RT (MyClass::*LFp)(int) &;
        typedef RT (MyClass::*CLFp)(int) const &;

        ASSERTV(LINE, (IsInvokeResult<EXP, LFp, T1&, int>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CLFp, T1&, int>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CLFp, const T1&, int>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef RT (MyClass::*RFp)(int) &&;
        typedef RT (MyClass::*VRFp)(int) volatile &&;

        ASSERTV(LINE, (IsInvokeResult<EXP, RFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, RFp, T1&&, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VRFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VRFp, T1&&, short>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, VRFp, volatile T1&&, short>::value));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        // Testing `bsl::invoke_result_t`

        ASSERTV(LINE, (IsInvokeResultT<Fp,                 T1, char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fp&,                T1, char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fp const&,          T1, char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fp volatile&,       T1, char>::value));
        ASSERTV(LINE, (IsInvokeResultT<Fp const volatile&, T1, char>::value));

        ASSERTV(LINE, (IsInvokeResultT<CFp,  T1,          short>::value));
        ASSERTV(LINE, (IsInvokeResultT<CFp,  const T1,    short>::value));
        ASSERTV(LINE, (IsInvokeResultT<VFp,  T1,          short>::value));
        ASSERTV(LINE, (IsInvokeResultT<VFp,  volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResultT<CVFp, T1,          short>::value));
        ASSERTV(LINE, (IsInvokeResultT<CVFp, volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResultT<CVFp, const T1,    short>::value));
        ASSERTV(LINE,
                (IsInvokeResultT<CVFp, const volatile T1, short>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResultT<Fp&&, T1, char>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // Test ref-qualified member-function-pointers

        ASSERTV(LINE, (IsInvokeResultT<LFp,  T1&,       int>::value));
        ASSERTV(LINE, (IsInvokeResultT<CLFp, T1&,       int>::value));
        ASSERTV(LINE, (IsInvokeResultT<CLFp, const T1&, int>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        ASSERTV(LINE, (IsInvokeResultT<RFp,  T1,            short>::value));
        ASSERTV(LINE, (IsInvokeResultT<RFp,  T1&&,          short>::value));
        ASSERTV(LINE, (IsInvokeResultT<VRFp, T1,            short>::value));
        ASSERTV(LINE, (IsInvokeResultT<VRFp, T1&&,          short>::value));
        ASSERTV(LINE, (IsInvokeResultT<VRFp, volatile T1&&, short>::value));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    }
};

// ============================================================================
//                      TEMPLATE PREINSTANTIATIONS
// ----------------------------------------------------------------------------

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macro calls enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, void);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, int);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, float);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, MyClass);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, MyEnum);

/// The number of template instantiations from a single call to
/// `applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3Fp>` overwhelmed AIX, so
/// this function performs the first half of that call (covering
/// non-volatile functions). The specified `LINE` is passed into the
/// `apply()` calls for display using `ASSERTV`.
void applyPtrAndRefMemberFunctionTestForFunctionPointer(int LINE)
{
    typedef Ic01 (*C3Fp)(MyClass, int);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3Fp>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3Fp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const C3Fp *>::apply(LINE);
}

/// The number of template instantiations from a single call to
/// `applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3Fp>` overwhelmed AIX, so
/// this function performs the second half of that call (covering volatile
/// functions). The specified `LINE` is passed into the `apply()` calls for
/// display using `ASSERTV`.
void applyPtrAndRefMemberFunctionTestForFunctionPointerVolatile(int LINE)
{
    typedef Ic01 (*C3Fp)(MyClass, int);
    ApplyRef<PtrToMemFuncTest<MyClass>, volatile C3Fp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const volatile C3Fp *>::apply(LINE);
}

/// The number of template instantiations from a single call to
/// `applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3MFp>` overwhelmed AIX, so
/// this function performs the first half of that call (covering
/// non-volatile functions). The specified `LINE` is passed into the
/// `apply()` calls for display using `ASSERTV`.
void applyPtrAndRefMemberFunctionTestForMemberFunctionPointer(int LINE)
{
    typedef Ic02 (MyClass::*C3MFp)(int);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3MFp>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3MFp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const C3MFp *>::apply(LINE);
}

/// The number of template instantiations from a single call to
/// `applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3MFp>` overwhelmed AIX, so
/// this function performs the second half of that call (covering volatile
/// functions). The specified `LINE` is passed into the `apply()` calls for
/// display using `ASSERTV`.
void applyPtrAndRefMemberFunctionTestForMemberFunctionPointerVolatile(int LINE)
{
    typedef Ic02 (MyClass::*C3MFp)(int);
    ApplyRef<PtrToMemFuncTest<MyClass>, volatile C3MFp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const volatile C3MFp *>::apply(LINE);
}

PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, Ic01 (*)(MyClass, int));
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, Ic02 (MyClass::*)(int));
//@bdetdsplit FOR 3 END

//@bdetdsplit FOR 4 BEGIN

/// For debugging purposes, instantiating this template with two types that
/// are not the same will result in a compile-time error that, on most
/// compilers, will print `T1` and `T2` as well as a trace of the
/// instantiation stack.
template <class T1, class T2>
struct AssertSame;

/// Specialization of `AssertSame` when instantiated with two parameters of
/// the same type yields a successful compilation.
template <class T1>
struct AssertSame<T1, T1> : bsl::true_type
{
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
/// This template has the interface of a smart pointer except that, the
/// dereference operator returns an RVALUE reference to `TP` instead of an
/// LVALUE reference.
template <class TP>
struct MovePtr {

    /// Dereference operator (declared but not defined).
    TP&& operator*() const volatile;
};
#endif

/// This template has the interface of a "proxy" pointer, where the
/// dereference operator returns an rvalue of type `TP`.
template <class TP>
struct ProxyPtr {

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    typedef typename bsl::remove_const<TP>::type value_type;
        // Type being proxied. Detection of const rvalues is problematic on
        // older Sun/Oracle compiler (they get confused with lvalues in our
        // metaprograms).
#else
    /// Type being proxied.
    typedef TP value_type;
#endif

    /// Dereference operator (declared but not defined)
    value_type operator*() const volatile;
};

struct ValueCategory {
    enum Enum {
        e_LVALUE,
        e_PRVALUE,
        e_XVALUE
    };
};

/// Apply the cv-qualifications from the specified template parameter type
/// `T1` to the specified template parameter `T2`, producing a typedef
/// `type`.  More specifically: `type` is `cvq T2&`, where `cvq` is the
/// cv-qualifiers (if any) on `T1`.  Note that any cv-qualifiers already on
/// `T2` are unioned with `cvq` to produce the result.
template <class T1, class T2>
struct PropagateCVQ {

    typedef T2 type;
};

/// Specialization of `PropagateCVQ` for `const T1`.
template <class T1, class T2>
struct PropagateCVQ<const T1, T2> {

    typedef const T2 type;
};

/// Specialization of `PropagateCVQ` for `volatile T1`.
template <class T1, class T2>
struct PropagateCVQ<volatile T1, T2> {

    typedef volatile T2 type;
};

/// Specialization of `PropagateCVQ` for `const volatile T1`.
template <class T1, class T2>
struct PropagateCVQ<const volatile T1, T2> {

    typedef const volatile T2 type;
};

/// Apply the reference and cv-qualifications from the specified template
/// parameter type `T1` to the specified template parameter `T2`, producing
/// a typedef `type`.  More specifically: if `T1` is an rvalue, `type` is
/// simply `T2` (unmodified); otherwise, `type` is `cvq T2&`, where `cvq`
/// is the cv-qualifiers (if any) on `T1`.  Note that any cv-qualifiers
/// already on `T2` are unioned with `cvq` to produce the result.  This
/// primary template is instantiated when `T1` is not a reference.
template <class T1, class T2>
struct PropagateCVQRef {

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    typedef T2 type;
        // Result type.  In C++03, if `T1` is an rvalue, then `T2` is returned
        // unchanged.  The cv-qualifications on `T1` are ignored.
#else
    /// Result type.  In C++11 and later, if `T1` is a prvalue, then `T2`
    /// is returned as an rvalue reference, with the cv-qualifications
    /// copied from `T1`.
    typedef typename PropagateCVQ<T1, T2>::type&& type;
#endif
};

/// Specialization of `PropagateCVQRef` for lvalue reference `T1`.
template <class T1, class T2>
struct PropagateCVQRef<T1&, T2> {

    typedef typename PropagateCVQ<T1, T2>::type& type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
/// Specialization of `PropagateCVQRef` for rvalue reference `T1`.
template <class T1, class T2>
struct PropagateCVQRef<T1&&, T2> {

    typedef typename PropagateCVQ<T1, T2>::type&& type;
};
#endif

/// A test kernel, intended as the `TEST_KERNEL` template argument to
/// `applyPtrAndRef`, which tests `invoke_result<Fn, T1>`, where `Fn` is a
/// pointer-to-member-object type, or reference to cv-qualified
/// pointer-to-member-object type.
///
/// If the specified `T1` is `MyClass` or a class derived from `MyClass`,
/// let the expression I be `std::decltype<T1>()`.  If the specified `T1` is
/// a specialization of `bsl::reference_wrapper`, let the expression I be
/// `std::decltype<T1>().get()`.  Otherwise, let the expression I be
/// `*std::decltype<T1>()`.  The behavior is undefined unless the specified
/// `EFFECTIVE_ARG` is the type of the expression I, and the specified
/// `VALUE_CATEGORY` identifies the value category of the expression I.
/// Note that in most cases (e.g. unless `T1` is `bsl::reference_wrapper`, a
/// pointer type, or a smart pointer type,) the default arguments for
/// `EFFECTIVE_ARG` and `VALUE_CATEGORY` are equal to the type and value
/// category of the expression I, respectively.
template <class               T1             = MyClass,
          class               EFFECTIVE_ARG  = T1,
          ValueCategory::Enum VALUE_CATEGORY =
              bsl::is_lvalue_reference<EFFECTIVE_ARG>::value
                  ? ValueCategory::e_LVALUE
                  : ValueCategory::e_XVALUE>
struct PtrToMemObjTest {

    /// For `Fn` of type `RT MyClass::*Mp`, instantiate `invoke_result<Fn>`
    /// and `invoke_result<Fn cvq&>`.  Verify that it yields `RT` as the
    /// result type.
    ///
    /// The specified `LINE` is passed to `ASSERTV` for diagnostics.
    template <class RT>
    static void apply(int LINE)
    {
        // If `RT` is `void` or a reference type, `doApply` is a no-op.  There
        // is no such thing as a member pointer to void or reference.
        enum { kIS_VOID_OR_REF = (bsl::is_void<RT>::value ||
                                  bsl::is_reference<RT>::value) };

        doApply<RT>(bsl::integral_constant<bool, kIS_VOID_OR_REF>(),
                    LINE);
    }

    /// No-op implementation of `apply` for when `RT` is `void` or a
    /// reference type.  There is no such thing as a member pointer to void
    /// or reference.
    ///
    /// The specified `LINE` is passed to `ASSERTV` for diagnostics.
    template <class RT>
    static void doApply(bsl::true_type /* void or reference */,
                        BSLA_MAYBE_UNUSED int                  LINE)
    {
    }

    /// For `Fn` of type `RT MyClass::*Mp`, instantiate
    /// `invoke_result<Fn, T1>` and `invoke_result<Fn cvq&, T1>`.  Verify
    /// that it yields `RT` as the result type.  This overload is called
    /// when `RT` is not `void` and is not a reference.
    ///
    /// The specified `LINE` is passed to `ASSERTV` for diagnostics.
    template <class RT>
    static void doApply(bsl::false_type /* void or reference */,
                        BSLA_MAYBE_UNUSED int                   LINE)
    {
        typedef RT MyClass::*Mp;

#if defined(GCC_PRE_CWG616_RESOLUTION_SEMANTICS)
        typedef typename
            bsl::conditional<
                VALUE_CATEGORY == ValueCategory::e_LVALUE,
                typename bsl::add_lvalue_reference<
                    typename PropagateCVQ<
                        typename bsl::remove_reference<EFFECTIVE_ARG>::type,
                        RT
                    >::type
                >::type,
                typename bsl::conditional<
                    VALUE_CATEGORY == ValueCategory::e_PRVALUE,
                    typename bsl::remove_cv<RT>::type,
                    typename bsl::add_rvalue_reference<
                        typename PropagateCVQ<
                            typename bsl::remove_reference<
                                EFFECTIVE_ARG
                                >::type,
                            RT
                        >::type
                    >::type
                >::type
            >::type CvqR;
#elif defined(MSVC_2015_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG)
        typedef typename
            bsl::conditional<
                bsl::is_array<RT>::value,
                RT,
                typename PropagateCVQRef<EFFECTIVE_ARG, RT>::type
            >::type CvqR;
#else  // MSVC_2015_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
        typedef typename PropagateCVQRef<EFFECTIVE_ARG, RT>::type CvqR;
#endif  // no MSVC_2015_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
        AssertSame<CvqR, typename bsl::invoke_result<Mp , T1>::type>();

        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp const&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp volatile&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp const volatile&, T1>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp&&, T1>::value));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Testing `bsl::invoke_result_t`.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        ASSERTV(LINE, (IsInvokeResultT<Mp,                 T1>::value));
        ASSERTV(LINE, (IsInvokeResultT<Mp&,                T1>::value));
        ASSERTV(LINE, (IsInvokeResultT<Mp const&,          T1>::value));
        ASSERTV(LINE, (IsInvokeResultT<Mp       volatile&, T1>::value));
        ASSERTV(LINE, (IsInvokeResultT<Mp const volatile&, T1>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResultT<Mp&&,               T1>::value));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    }
};

// ============================================================================
//                      TEMPLATE PREINSTANTIATIONS
// ----------------------------------------------------------------------------

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macro calls enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyClass>,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyDerivedClass>,
                        MyDerivedClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass *,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass *,
                        MyDerivedClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass>,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass>,
                        MyDerivedClass&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const volatile ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass>&,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const SmartPtr<MyClass>&,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const volatile SmartPtr<MyClass>&,
                        MyClass&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const*,
                        MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const *,
                        MyDerivedClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass const>,
                        MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass const>,
                        MyDerivedClass const&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass volatile*,
                        MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass volatile*,
                        MyDerivedClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass volatile>,
                        MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass volatile>,
                        MyDerivedClass volatile&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const volatile*,
                        MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const volatile*,
                        MyDerivedClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass const volatile>,
                        MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass const volatile>,
                        MyDerivedClass const volatile&>);
//@bdetdsplit FOR 4 END

//@bdetdsplit FOR 5 BEGIN

/// Objects of this class are convertible to pointer-to-int.
struct ToIntPtr
{

    /// Convertion to `int*` (declared but not defined).
    operator int*() const;
};

namespace bsl {

/// Specialization of `invoke_result` for a situation where, in C++03 mode,
/// return-type deduction would otherwise fail.
template <class TP>
class invoke_result<ManyFunc, char*, MetaType<TP> > {

  public:
    /// Correct return type for `ManyFunc::operator()(char*, MetaType<TP>)`
    typedef TP type;
};

}  // close namespace bsl

/// A test kernel, intended as the `TEST_KERNEL` template argument to
/// `applyPtrAndRef`, which tests `invoke_result<Fn>`, where `Fn` is a class
/// (functor) type, or reference to cv-qualified functor type.  If the
/// specified `RET_USER_TYPE` template parameter is true, it indicates that
/// the caller intends to call `apply<RT>` with user-defined (class or
/// enumeration) type for `RT`.  Otherwise, the caller must supply a basic
/// (numeric, pointer, or void) type for `RT`.
template <bool RET_USER_TYPE = false>
struct FunctorTest
{

    /// Instantiate `invoke_result<Fn, Args...>` where `Fn` is a variety of
    /// functor types, and verify that it yields a `type` of the specified
    /// template parameter type `RT`.  This test checks the return type for
    /// the following functor types:
    ///
    /// * `FuncRt1`, which uses the `result_type` idiom
    /// * `FuncRt2`, which uses the `ResultType` idiom
    /// * `FuncRt3`, which uses both `result_type` and `ResultType`
    /// * `ManyFunc`, which uses neither `result_type` and `ResultType`
    ///
    /// If `RET_USER_TYPE` is true and the compiler does not support
    /// `decltype`, then the expected result type for `ManyFunc` cannot be
    /// deduced and `invoke_result` is expected to yield
    /// `bslmf::InvokeResultDeductionFailed`.  As a special case, for
    /// `FuncRt1`, `FuncRt2`, and `FuncRt3`, test that using `invoke_result`
    /// arguments that choose an overload that returns `int` will correctly
    /// yield `int` regardless of the `result_type` and `ResultType`
    /// idioms.
    ///
    /// The specified parmeter `LINE` is passed to `ASSERTV()` calls.
    template <class RT>
    static void apply(int LINE)
    {
        using bsl::invoke_result;
        using bslmf::InvokeResultDeductionFailed;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        using bsl::invoke_result_t;
#endif

        // The `char*` overload of `FuncRt1<RT>::operator()` will return `RT`,
        // whereas the `int` overload will return `int`.
        typedef typename invoke_result<FuncRt1<RT> , char*>::type Rt1ResultR;
        typedef typename invoke_result<FuncRt1<RT>&, char*>::type Rt1rResultR;
        typedef typename invoke_result<FuncRt1<RT> , int  >::type Rt1ResultInt;
        typedef typename invoke_result<FuncRt1<RT>&, int  >::type
            Rt1rResultInt;

        ASSERTV(LINE, (bsl::is_same<RT , Rt1ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT , Rt1rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt1ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt1rResultInt>::value));

        // The `char*` overload of `FuncRt2<RT>::operator()` will return `RT`,
        // whereas the `int` overload will return `int`.
        typedef typename invoke_result<FuncRt2<RT> , char*>::type Rt2ResultR;
        typedef typename invoke_result<FuncRt2<RT>&, char*>::type Rt2rResultR;
        typedef typename invoke_result<FuncRt2<RT> , int  >::type Rt2ResultInt;
        typedef typename invoke_result<FuncRt2<RT>&, int  >::type
            Rt2rResultInt;
        ASSERTV(LINE, (bsl::is_same<RT,   Rt2ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT,   Rt2rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt2ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt2rResultInt>::value));

        // The `char*` overload of `FuncRt3<RT>::operator()` will return `RT`,
        // whereas the `int` overload will return `int`.
        typedef typename invoke_result<FuncRt3<RT> , char*>::type Rt3ResultR;
        typedef typename invoke_result<FuncRt3<RT>&, char*>::type Rt3rResultR;
        typedef typename invoke_result<FuncRt3<RT> , int  >::type Rt3ResultInt;
        typedef typename invoke_result<FuncRt3<RT>&, int  >::type
            Rt3rResultInt;
        ASSERTV(LINE, (bsl::is_same<RT , Rt3ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT , Rt3rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt3ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt3rResultInt>::value));

        typedef typename invoke_result<ManyFunc,  MetaType<RT>, int>::type
            MFResult;
        typedef typename invoke_result<ManyFunc&, MetaType<RT>, int>::type
            MFrResult;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013
        // `decltype` is supported, the result type will always be correct.
        ASSERTV(LINE, (bsl::is_same<RT, MFResult>::value));
        ASSERTV(LINE, (bsl::is_same<RT, MFrResult>::value));
#else
        // `decltype` is not supported, the result type will always be correct
        // for basic types like `int`, `int*` and `void`, but will be a
        // place-holder for user-defined types that don't support the
        // `result_type` or `ResultType` idioms.
        if (RET_USER_TYPE) {
            // User-defined type, expect `InvokeResultDeductionFailed` place
            // holder.
            ASSERTV(LINE, (bsl::is_same<InvokeResultDeductionFailed,
                                 MFResult>::value));
            ASSERTV(LINE, (bsl::is_same<InvokeResultDeductionFailed,
                                 MFrResult>::value));
        }
        else {
            // Basic type, expect correct result.
            ASSERTV(LINE, (bsl::is_same<RT, MFResult>::value));
            ASSERTV(LINE, (bsl::is_same<RT, MFrResult>::value));
        }
#endif // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        // Testing `bsl::invoke_result_t`.

        ASSERTV(LINE, (IsInvokeResultT<FuncRt1<RT> , char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt1<RT>&, char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt1<RT> , int  >::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt1<RT>&, int  >::value));

        ASSERTV(LINE, (IsInvokeResultT<FuncRt2<RT> , char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt2<RT>&, char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt2<RT> , int  >::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt2<RT>&, int  >::value));

        ASSERTV(LINE, (IsInvokeResultT<FuncRt3<RT> , char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt3<RT>&, char*>::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt3<RT> , int  >::value));
        ASSERTV(LINE, (IsInvokeResultT<FuncRt3<RT>&, int  >::value));

        ASSERTV(LINE, (IsInvokeResultT<ManyFunc,  MetaType<RT>, int>::value));
        ASSERTV(LINE, (IsInvokeResultT<ManyFunc&, MetaType<RT>, int>::value));

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

    }
};

// ============================================================================
//                      TEMPLATE PREINSTANTIATIONS
// ----------------------------------------------------------------------------

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macro calls enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

// Steps 4 & 5, concerns 5, 6, and 7
PREINSTANTIATE_AR(FunctorTest<true>, MyEnum);
PREINSTANTIATE_AR(FunctorTest<true>, MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, MyClass);
PREINSTANTIATE_AR(FunctorTest<true>, MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, MyDerivedClass);
PREINSTANTIATE_AR(FunctorTest<true>, MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyDerivedClass*);

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
/// became overwhelmed with the number of nested template instantiations, so
/// the call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforEnum()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyEnum            >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyEnum>::apply(L_);
    ApplyRef<FunctorTest<true>, MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyEnum*>::apply(L_);
}

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
/// became overwhelmed with the number of nested template instantiations, so
/// the call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforClass()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyClass           >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyClass>::apply(L_);
    ApplyRef<FunctorTest<true>, MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyClass*>::apply(L_);
}

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
/// became overwhelmed with the number of nested template instantiations, so
/// the call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforDerivedClass()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyDerivedClass    >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyDerivedClass>::apply(L_);
    ApplyRef<FunctorTest<true>, MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyDerivedClass*>::apply(L_);
}

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
/// overwhelmed with the number of nested template instantiations, so the
/// call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforVoid()
{
    // Step 3 & 5, concerns 4 and 7
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, void              >(L_);
#endif
    ApplyRef<FunctorTest<>,                void>::apply(L_);
    ApplyRef<FunctorTest<>,                void*>::apply(L_);
    ApplyRef<FunctorTest<>,          const void*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile void*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile void*>::apply(L_);
}

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
/// overwhelmed with the number of nested template instantiations, so the
/// call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforChar()
{
    // Step 3 & 5, concerns 4 and 7
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, char              >(L_);
#endif
    ApplyRef<FunctorTest<>,                char >::apply(L_);
    ApplyRef<FunctorTest<>,                char*>::apply(L_);
    ApplyRef<FunctorTest<>,          const char*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile char*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile char*>::apply(L_);
}

PREINSTANTIATE_AR(FunctorTest<>,                long double);
PREINSTANTIATE_AR(FunctorTest<>,                long double*);
PREINSTANTIATE_AR(FunctorTest<>,          const long double*);
PREINSTANTIATE_AR(FunctorTest<>,       volatile long double*);
PREINSTANTIATE_AR(FunctorTest<>, const volatile long double*);

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
/// overwhelmed with the number of nested template instantiations, so the
/// call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforLongDouble()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, long double       >(L_);
#endif
    ApplyRef<FunctorTest<>,                long double>::apply(L_);
    ApplyRef<FunctorTest<>,                long double*>::apply(L_);
    ApplyRef<FunctorTest<>,          const long double*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile long double*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile long double*>::apply(L_);
}

PREINSTANTIATE_AR(FunctorTest<>,                bsl::nullptr_t);
PREINSTANTIATE_AR(FunctorTest<>,                bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>,          const bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>,       volatile bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>, const volatile bsl::nullptr_t*);

/// This function was originally a call to the function:
/// applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
/// overwhelmed with the number of nested template instantiations, so the
/// call was replaced with a function replicating the behaviour of the
/// original applyPtrAndRef call.  This unfortunately does add duplication.
void testCase5APRforNullptr()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, bsl::nullptr_t    >(L_);
#endif
    ApplyRef<FunctorTest<>,                bsl::nullptr_t>::apply(L_);
    ApplyRef<FunctorTest<>,                bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>,          const bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile bsl::nullptr_t*>::apply(L_);
}

/// Only used for the non-abridged (full) test, this contains all of the
/// calls to applyPtrAndRef previously contained within testCase5() but not
/// split into the individual functions required to support abridged tests.
void testCase5APRforOther()
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    // Step 3 & 5, concerns 4 and 7
    applyPtrAndRef<FunctorTest<>, signed char       >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned char     >(L_);
#   ifdef BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
    applyPtrAndRef<FunctorTest<>, char8_t           >(L_);
#   endif
    applyPtrAndRef<FunctorTest<>, wchar_t           >(L_);
#   ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
    applyPtrAndRef<FunctorTest<>, char16_t          >(L_);
    applyPtrAndRef<FunctorTest<>, char32_t          >(L_);
#   endif
    applyPtrAndRef<FunctorTest<>, short             >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned short    >(L_);
    applyPtrAndRef<FunctorTest<>, int               >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned int      >(L_);
    applyPtrAndRef<FunctorTest<>, long int          >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned long     >(L_);
    applyPtrAndRef<FunctorTest<>, long long         >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned long long>(L_);
    applyPtrAndRef<FunctorTest<>, float             >(L_);
    applyPtrAndRef<FunctorTest<>, double            >(L_);
#endif
}
//@bdetdsplit FOR 5 END

//@bdetdsplit FOR 6 BEGIN

/// Return a value-initialized object of type `TP`.
template <class TP>
TP returnObj()
{
    return TP();
}

/// Return a reference to a value-initialized object of type `TP`. `TP` is
/// assumed not to be cv-qualified.
template <class TP>
TP& returnNoCVRef()
{
    static TP obj;
    return obj;
}

/// Return an lvalue reference to a value-initialized object of type
/// `TP`.  `TP` may be cv-qualified.
template <class TP>
TP& returnLvalueRef()
{
    return returnNoCVRef<typename bsl::remove_cv<TP>::type>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
/// Return an rvalue reference to a value-initialized object of type `TP`.
template <class TP>
TP&& returnRvalueRef()
{
    return std::move(returnLvalueRef<TP>());
}
#endif

/// Return an `bslmf::InvokeResultDeductionFailed` object initialized from
/// an rvalue of type `TP`.
template <class TP>
bslmf::InvokeResultDeductionFailed discardObj()
{
    return returnObj<TP>();
}

/// Return an `bslmf::InvokeResultDeductionFailed` object initialized from
/// an lvalue reference of type `TP&`.  `TP` may be cv-qualified.
template <class TP>
bslmf::InvokeResultDeductionFailed discardLvalueRef()
{
    return returnLvalueRef<TP>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
/// Return an `bslmf::InvokeResultDeductionFailed` object initialized from
/// an rvalue reference of type `TP&&`.  `TP` may be cv-qualified.
template <class TP>
bslmf::InvokeResultDeductionFailed discardRvalueRef()
{
    return returnRvalueRef<TP>();
}
#endif
//@bdetdsplit FOR 6 END

//@bdetdsplit FOR 8 BEGIN
//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Suppose we want to create a wrapper that executes an invocable object and
// sets a `done` flag.  The `done` flag will not be set if the invocation
// exits via an exception.  The wrapper takes an invocable `f` and an argument
// `x` and evaluates `f(x)`, returning the result.  In the absence of C++14
// automatically-deduced function return declarations, we use
// `bsl::invoke_result` to deduce the return type of `f(x)`.
//
// First, we write the wrapper template as follows:
// ```

    /// Use the specified parameters `done`, `f`, and `x` to call `f(x)` and
    /// return the result, having set `*done` to true if no exception (or
    /// false if exception).
    template <class FT, class XT>
    typename bsl::invoke_result<FT, XT>::type
    invokeAndSetFlag(bool *done, FT f, XT x)
    {
        typedef typename bsl::invoke_result<FT, XT>::type ResultType;
        *done = false; // Clear flag in case of exception
        ResultType result = f(x);
        *done = true;  // Set flag on success
        return result;
    }
// ```
// Note that additional metaprogramming would be required to make this
// template work for return type `void`; such metaprogramming is beyond the
// scope of this usage example.
//
// Then we define a couple of simple functors to be used with the wrapper.
// The first functor is a simple template that triples its invocation
// argument:
// ```

    /// Functor that triples its argument.
    template <class TP>
    struct Triple {

        /// Return three times the specified `v` value.
        TP operator()(TP v) const { return static_cast<TP>(v * 3); }
    };
// ```
// Next, we define a second functor that returns an enumerator `ODD` or
// `EVEN`, depending on whether its argument is exactly divisible by 2.  Since
// the return type is not a fundamental type, this functor indicates its
// return type using the `ResultType` idiom:
// ```
    enum EvenOdd { e_EVEN, e_ODD };

    /// Functor that determines whether its argument is odd or even.
    struct CalcEvenOdd {

        typedef EvenOdd ResultType;

        /// Return `e_ODD` if the specified `i` is odd; otherwise return
        /// `e_EVEN`
        EvenOdd operator()(int i) const { return (i & 1) ? e_ODD : e_EVEN; }
    };
// ```
// Finally, we can invoke these functors through our wrapper:
// ```

    /// Run the usage example.
    int usageExample()
    {
        bool done = false;

        Triple<short> ts = {};
        short         r0 = invokeAndSetFlag(&done, ts, short(9));
        ASSERT(done && 27 == r0);

        CalcEvenOdd ceo = {};
        done            = false;
        EvenOdd r1      = invokeAndSetFlag(&done, ceo, 5);
        ASSERT(done && e_ODD == r1);

        done = false;
        EvenOdd r2 = invokeAndSetFlag(&done, ceo, 8);
        ASSERT(done && e_EVEN == r2);

        return 0;
    }
//@bdetdsplit FOR 8 END

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test            = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose         = argc > 2;
    const bool veryVerbose     = argc > 3;  (void)veryVerbose;
    const bool veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;

    using BloombergLP::bslmf::InvokeResultDeductionFailed;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Concerns:
        // 1. The usage example in the component documentation compiles and
        //    runs.
        //
        // Plan:
        // 1.  Copy the usage example verbatim but replace `assert` with
        //    `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE TEST"
                            "\n==========\n");
        usageExample();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING C++17 SEMANTICS
        //
        // Concerns:
        // 1. In C++11, `bsl::invoke_result<F, ARGS...>` has a nested typedef
        //    `type` if the expression `INVOKE(f, args...)` is well-defined
        //    according to the C++17 standard, given `f` is an object of type
        //    `F`, and `args...` are objects of type `ARGS...`, and has no such
        //    typedef otherwise, for all types `F` and `ARGS...`.
        //
        // Plan:
        // 1. For a large array of various `const`-, `volatile`-, and/or
        //    reference-qualified invocable types, instantiate
        //    `bsl::invoke_result` with that invocable type, and several
        //    argument lists for which that type is and is not invocable.
        //    Verify that `bsl::invoke_result` has a nested typedef `type` if
        //    the invocable type is invocable, for each particular argument
        //    list, and has no such typedef otherwise.
        //
        // Testing:
        //   C++17 SEMANTICS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 SEMANTICS"
                            "\n=======================\n");

        // Remove the "defined but not used" warning:
        ASSERT(true);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013

        typedef void                V;
        typedef int                 I;
        typedef const int           cI;
        typedef volatile int        vI;
        typedef const volatile int  cvI;
        typedef int&                rI;
        typedef const int&          crI;
        typedef volatile int&       vrI;
        typedef const volatile int& cvrI;

        typedef void(FV)(V);
        typedef void(FI)(I);
        typedef void(FrI)(rI);
        typedef void(FcrI)(crI);
        typedef void(FvrI)(vrI);
        typedef void(FcvrI)(cvrI);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef int&& rrI;
        typedef void(FrrI)(rrI);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        typedef MyClass                 M;
        typedef const MyClass           cM;
        typedef volatile MyClass        vM;
        typedef const volatile MyClass  cvM;
        typedef MyClass&                rM;
        typedef const MyClass&          rcM;
        typedef volatile MyClass&       rvM;
        typedef const volatile MyClass& rcvM;

        typedef MyClass                   *pM;
        typedef const MyClass             *pcM;
        typedef volatile MyClass          *pvM;
        typedef const volatile MyClass    *pcvM;
        typedef MyClass *&                 rpM;
        typedef const MyClass *&           rpcM;
        typedef volatile MyClass *&        rpvM;
        typedef const volatile MyClass *&  rpcvM;

        typedef SmartPtr<MyClass>                sM;
        typedef SmartPtr<const MyClass>          scM;
        typedef SmartPtr<volatile MyClass>       svM;
        typedef SmartPtr<const volatile MyClass> scvM;

        typedef stub_reference_wrapper<MyClass>                wM;
        typedef stub_reference_wrapper<const MyClass>          wcM;
        typedef stub_reference_wrapper<volatile MyClass>       wvM;
        typedef stub_reference_wrapper<const volatile MyClass> wcvM;

        typedef void (M::*mFV)(V);
        typedef void (M::*cmFV)(V) const;
        typedef void (M::*vmFV)(V) volatile;
        typedef void (M::*cvmFV)(V) const volatile;

        typedef    I M::*mI;
        typedef   cI M::*cmI;
        typedef   vI M::*vmI;
        typedef  cvI M::*cvmI;

        typedef MyDerivedClass                 D;
        typedef const MyDerivedClass           cD;
        typedef volatile MyDerivedClass        vD;
        typedef const volatile MyDerivedClass  cvD;
        typedef MyDerivedClass&                rD;
        typedef const MyDerivedClass&          rcD;
        typedef volatile MyDerivedClass&       rvD;
        typedef const volatile MyDerivedClass& rcvD;

        static const bool YES = true;
        static const bool NO = false;

#if MSVC && !(MSVC_2022 && (BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L))
        // MSVC 2019 and earlier, and MSVC 2022 in C++17 (or earlier) mode
        // incorrectly handles volatile-qualified invocable types.  This has
        // been rectified in MSVC 2022 compiling in C++20 mode.
        static const bool MSVC_PRE_CPP20 = true;
#else
        static const bool MSVC_PRE_CPP20 = false;
#endif

        const SfinaeFriendlinessTest TEST;

        //          `type` IS DEFINED                         LINE NUMBER
        //         .=================                         ===========.
        //        /    INVOCABLE TYPE          ARGUMENT TYPES             \.
        //       === ================== ================================  ==

        // Void functions
        TEST.run<YES,                FV                                 >(L_);
        TEST.run< NO,                FV,          I                     >(L_);

        // Functions taking one optionally const- and/or volatile-qualified
        // integer.
        TEST.run< NO,                FI                                 >(L_);
        TEST.run<YES,                FI,          I                     >(L_);
        TEST.run<YES,                FI,         cI                     >(L_);
        TEST.run<YES,                FI,         vI                     >(L_);
        TEST.run<YES,                FI,        cvI                     >(L_);
        TEST.run<YES,                FI,         rI                     >(L_);
        TEST.run<YES,                FI,        crI                     >(L_);
        TEST.run<YES,                FI,        vrI                     >(L_);
        TEST.run<YES,                FI,       cvrI                     >(L_);
        TEST.run< NO,                FI,          I,     I              >(L_);
        TEST.run< NO,               FrI                                 >(L_);
        TEST.run< NO,               FrI,          I                     >(L_);
        TEST.run< NO,               FrI,         cI                     >(L_);
        TEST.run< NO,               FrI,         vI                     >(L_);
        TEST.run< NO,               FrI,        cvI                     >(L_);
        TEST.run<YES,               FrI,         rI                     >(L_);
        TEST.run< NO,               FrI,        crI                     >(L_);
        TEST.run< NO,               FrI,        vrI                     >(L_);
        TEST.run< NO,               FrI,       cvrI                     >(L_);
        TEST.run< NO,               FrI,          I,     I              >(L_);
        TEST.run< NO,              FcrI                                 >(L_);
        TEST.run<YES,              FcrI,          I                     >(L_);
        TEST.run<YES,              FcrI,         cI                     >(L_);
        TEST.run< NO,              FcrI,         vI                     >(L_);
        TEST.run< NO,              FcrI,        cvI                     >(L_);
        TEST.run<YES,              FcrI,         rI                     >(L_);
        TEST.run<YES,              FcrI,        crI                     >(L_);
        TEST.run< NO,              FcrI,        vrI                     >(L_);
        TEST.run< NO,              FcrI,       cvrI                     >(L_);
        TEST.run< NO,              FcrI,          I,     I              >(L_);
        TEST.run< NO,              FvrI                                 >(L_);
        TEST.run< NO,              FvrI,          I                     >(L_);
        TEST.run< NO,              FvrI,         cI                     >(L_);
        TEST.run< NO,              FvrI,         vI                     >(L_);
        TEST.run< NO,              FvrI,        cvI                     >(L_);
        TEST.run<YES,              FvrI,         rI                     >(L_);
        TEST.run< NO,              FvrI,        crI                     >(L_);
        TEST.run<YES,              FvrI,        vrI                     >(L_);
        TEST.run< NO,              FvrI,       cvrI                     >(L_);
        TEST.run< NO,              FvrI,          I,     I              >(L_);
        TEST.run< NO,             FcvrI                                 >(L_);
        TEST.run<MSVC_PRE_CPP20,  FcvrI,          I                     >(L_);
        TEST.run<MSVC_PRE_CPP20,  FcvrI,         cI                     >(L_);
        TEST.run<MSVC_PRE_CPP20,  FcvrI,         vI                     >(L_);
        TEST.run<MSVC_PRE_CPP20,  FcvrI,        cvI                     >(L_);
        TEST.run<YES,             FcvrI,         rI                     >(L_);
        TEST.run<YES,             FcvrI,        crI                     >(L_);
        TEST.run<YES,             FcvrI,        vrI                     >(L_);
        TEST.run<YES,             FcvrI,       cvrI                     >(L_);

        // Functions taking one rvalue-reference-qualified integer.  Note that
        // this is the only section in this test case that considers rvalue
        // references, because this case is already as big enough as it is.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST.run< NO,              FrrI                                 >(L_);
        TEST.run<YES,              FrrI,          I                     >(L_);
        TEST.run< NO,              FrrI,         cI                     >(L_);
        TEST.run< NO,              FrrI,         vI                     >(L_);
        TEST.run< NO,              FrrI,        cvI                     >(L_);
        TEST.run< NO,              FrrI,         rI                     >(L_);
        TEST.run< NO,              FrrI,        crI                     >(L_);
        TEST.run< NO,              FrrI,        vrI                     >(L_);
        TEST.run< NO,              FrrI,       cvrI                     >(L_);
        TEST.run<YES,              FrrI,        rrI                     >(L_);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Pointers to functions taking one optionally const- and/or
        // volatile-qualified integer.
        TEST.run< NO,              FI *                                 >(L_);
        TEST.run<YES,              FI *,          I                     >(L_);
        TEST.run<YES,              FI *,         cI                     >(L_);
        TEST.run<YES,              FI *,         vI                     >(L_);
        TEST.run<YES,              FI *,        cvI                     >(L_);
        TEST.run<YES,              FI *,         rI                     >(L_);
        TEST.run<YES,              FI *,        crI                     >(L_);
        TEST.run<YES,              FI *,        vrI                     >(L_);
        TEST.run<YES,              FI *,       cvrI                     >(L_);
        TEST.run< NO,              FI *,          I,     I              >(L_);
        TEST.run< NO,             FrI *                                 >(L_);
        TEST.run< NO,             FrI *,          I                     >(L_);
        TEST.run< NO,             FrI *,         cI                     >(L_);
        TEST.run< NO,             FrI *,         vI                     >(L_);
        TEST.run< NO,             FrI *,        cvI                     >(L_);
        TEST.run<YES,             FrI *,         rI                     >(L_);
        TEST.run< NO,             FrI *,        crI                     >(L_);
        TEST.run< NO,             FrI *,        vrI                     >(L_);
        TEST.run< NO,             FrI *,       cvrI                     >(L_);
        TEST.run< NO,             FrI *,          I,     I              >(L_);
        TEST.run< NO,            FcrI *                                 >(L_);
        TEST.run<YES,            FcrI *,          I                     >(L_);
        TEST.run<YES,            FcrI *,         cI                     >(L_);
        TEST.run< NO,            FcrI *,         vI                     >(L_);
        TEST.run< NO,            FcrI *,        cvI                     >(L_);
        TEST.run<YES,            FcrI *,         rI                     >(L_);
        TEST.run<YES,            FcrI *,        crI                     >(L_);
        TEST.run< NO,            FcrI *,        vrI                     >(L_);
        TEST.run< NO,            FcrI *,       cvrI                     >(L_);
        TEST.run< NO,            FcrI *,          I,     I              >(L_);
        TEST.run< NO,           FcvrI *                                 >(L_);
        TEST.run<MSVC_PRE_CPP20,FcvrI *,          I                     >(L_);
        TEST.run<MSVC_PRE_CPP20,FcvrI *,         cI                     >(L_);
        TEST.run<MSVC_PRE_CPP20,FcvrI *,         vI                     >(L_);
        TEST.run<MSVC_PRE_CPP20,FcvrI *,        cvI                     >(L_);
        TEST.run<YES,           FcvrI *,         rI                     >(L_);
        TEST.run<YES,           FcvrI *,        crI                     >(L_);
        TEST.run<YES,           FcvrI *,        vrI                     >(L_);
        TEST.run<YES,           FcvrI *,       cvrI                     >(L_);

        // References to functions taking one optionally const- and/or
        // volatile-qualified integer.
        TEST.run< NO,               FI&                                 >(L_);
        TEST.run<YES,               FI&,          I                     >(L_);
        TEST.run<YES,               FI&,         cI                     >(L_);
        TEST.run<YES,               FI&,         vI                     >(L_);
        TEST.run<YES,               FI&,        cvI                     >(L_);
        TEST.run<YES,               FI&,         rI                     >(L_);
        TEST.run<YES,               FI&,        crI                     >(L_);
        TEST.run<YES,               FI&,        vrI                     >(L_);
        TEST.run<YES,               FI&,       cvrI                     >(L_);
        TEST.run< NO,               FI&,          I,     I              >(L_);
        TEST.run< NO,              FrI&                                 >(L_);
        TEST.run< NO,              FrI&,          I                     >(L_);
        TEST.run< NO,              FrI&,         cI                     >(L_);
        TEST.run< NO,              FrI&,         vI                     >(L_);
        TEST.run< NO,              FrI&,        cvI                     >(L_);
        TEST.run<YES,              FrI&,         rI                     >(L_);
        TEST.run< NO,              FrI&,        crI                     >(L_);
        TEST.run< NO,              FrI&,        vrI                     >(L_);
        TEST.run< NO,              FrI&,       cvrI                     >(L_);
        TEST.run< NO,              FrI&,          I,     I              >(L_);
        TEST.run< NO,             FcrI&                                 >(L_);
        TEST.run<YES,             FcrI&,          I                     >(L_);
        TEST.run<YES,             FcrI&,         cI                     >(L_);
        TEST.run< NO,             FcrI&,         vI                     >(L_);
        TEST.run< NO,             FcrI&,        cvI                     >(L_);
        TEST.run<YES,             FcrI&,         rI                     >(L_);
        TEST.run<YES,             FcrI&,        crI                     >(L_);
        TEST.run< NO,             FcrI&,        vrI                     >(L_);
        TEST.run< NO,             FcrI&,       cvrI                     >(L_);
        TEST.run< NO,             FcrI&,          I,     I              >(L_);
        TEST.run< NO,            FcvrI&                                 >(L_);
        TEST.run<MSVC_PRE_CPP20, FcvrI&,          I                     >(L_);
        TEST.run<MSVC_PRE_CPP20, FcvrI&,         cI                     >(L_);
        TEST.run<MSVC_PRE_CPP20, FcvrI&,         vI                     >(L_);
        TEST.run<MSVC_PRE_CPP20, FcvrI&,        cvI                     >(L_);
        TEST.run<YES,            FcvrI&,         rI                     >(L_);
        TEST.run<YES,            FcvrI&,        crI                     >(L_);
        TEST.run<YES,            FcvrI&,        vrI                     >(L_);
        TEST.run<YES,            FcvrI&,       cvrI                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified, int-typed
        // member objects
        TEST.run< NO,                mI                                 >(L_);
        TEST.run<YES,                mI,          M                     >(L_);
        TEST.run< NO,                mI,          M,     I              >(L_);
        TEST.run<YES,                mI,         cM                     >(L_);
        TEST.run<YES,                mI,         vM                     >(L_);
        TEST.run<YES,                mI,        cvM                     >(L_);
        TEST.run<YES,                mI,         rM                     >(L_);
        TEST.run< NO,                mI,         rM,     I              >(L_);
        TEST.run<YES,                mI,        rcM                     >(L_);
        TEST.run<YES,                mI,        rvM                     >(L_);
        TEST.run<YES,                mI,       rcvM                     >(L_);
        TEST.run<YES,                mI,         pM                     >(L_);
        TEST.run< NO,                mI,         pM,     I              >(L_);
        TEST.run<YES,                mI,        pcM                     >(L_);
        TEST.run<YES,                mI,        pvM                     >(L_);
        TEST.run<YES,                mI,       pcvM                     >(L_);
        TEST.run<YES,                mI,        rpM                     >(L_);
        TEST.run< NO,                mI,        rpM,     I              >(L_);
        TEST.run<YES,                mI,       rpcM                     >(L_);
        TEST.run<YES,                mI,       rpvM                     >(L_);
        TEST.run<YES,                mI,      rpcvM                     >(L_);
        TEST.run<YES,                mI,         sM                     >(L_);
        TEST.run< NO,                mI,         sM,     I              >(L_);
        TEST.run<YES,                mI,        scM                     >(L_);
        TEST.run<YES,                mI,        svM                     >(L_);
        TEST.run<YES,                mI,       scvM                     >(L_);
        TEST.run<YES,                mI,         wM                     >(L_);
        TEST.run< NO,                mI,         wM,     I              >(L_);
        TEST.run<YES,                mI,        wcM                     >(L_);
        TEST.run<YES,                mI,        wvM                     >(L_);
        TEST.run<YES,                mI,       wcvM                     >(L_);
        TEST.run<YES,               cmI,          M                     >(L_);
        TEST.run< NO,               cmI,          M,     I              >(L_);
        TEST.run<YES,               cmI,         cM                     >(L_);
        TEST.run<YES,               cmI,         vM                     >(L_);
        TEST.run<YES,               cmI,        cvM                     >(L_);
        TEST.run<YES,               cmI,         rM                     >(L_);
        TEST.run< NO,               cmI,         rM,     I              >(L_);
        TEST.run<YES,               cmI,        rcM                     >(L_);
        TEST.run<YES,               cmI,        rvM                     >(L_);
        TEST.run<YES,               cmI,       rcvM                     >(L_);
        TEST.run<YES,               cmI,         pM                     >(L_);
        TEST.run< NO,               cmI,         pM,     I              >(L_);
        TEST.run<YES,               cmI,        pcM                     >(L_);
        TEST.run<YES,               cmI,        pvM                     >(L_);
        TEST.run<YES,               cmI,       pcvM                     >(L_);
        TEST.run<YES,               cmI,        rpM                     >(L_);
        TEST.run< NO,               cmI,        rpM,     I              >(L_);
        TEST.run<YES,               cmI,       rpcM                     >(L_);
        TEST.run<YES,               cmI,       rpvM                     >(L_);
        TEST.run<YES,               cmI,      rpcvM                     >(L_);
        TEST.run<YES,               cmI,         sM                     >(L_);
        TEST.run< NO,               cmI,         sM,     I              >(L_);
        TEST.run<YES,               cmI,        scM                     >(L_);
        TEST.run<YES,               cmI,        svM                     >(L_);
        TEST.run<YES,               cmI,       scvM                     >(L_);
        TEST.run<YES,               cmI,         wM                     >(L_);
        TEST.run< NO,               cmI,         wM,     I              >(L_);
        TEST.run<YES,               cmI,        wcM                     >(L_);
        TEST.run<YES,               cmI,        wvM                     >(L_);
        TEST.run<YES,               cmI,       wcvM                     >(L_);
        TEST.run<YES,               vmI,          M                     >(L_);
        TEST.run< NO,               vmI,          M,     I              >(L_);
        TEST.run<YES,               vmI,         cM                     >(L_);
        TEST.run<YES,               vmI,         vM                     >(L_);
        TEST.run<YES,               vmI,        cvM                     >(L_);
        TEST.run<YES,               vmI,         rM                     >(L_);
        TEST.run< NO,               vmI,         rM,     I              >(L_);
        TEST.run<YES,               vmI,        rcM                     >(L_);
        TEST.run<YES,               vmI,        rvM                     >(L_);
        TEST.run<YES,               vmI,       rcvM                     >(L_);
        TEST.run<YES,               vmI,         pM                     >(L_);
        TEST.run< NO,               vmI,         pM,     I              >(L_);
        TEST.run<YES,               vmI,        pcM                     >(L_);
        TEST.run<YES,               vmI,        pvM                     >(L_);
        TEST.run<YES,               vmI,       pcvM                     >(L_);
        TEST.run<YES,               vmI,        rpM                     >(L_);
        TEST.run< NO,               vmI,        rpM,     I              >(L_);
        TEST.run<YES,               vmI,       rpcM                     >(L_);
        TEST.run<YES,               vmI,       rpvM                     >(L_);
        TEST.run<YES,               vmI,      rpcvM                     >(L_);
        TEST.run<YES,               vmI,         sM                     >(L_);
        TEST.run< NO,               vmI,         sM,     I              >(L_);
        TEST.run<YES,               vmI,        scM                     >(L_);
        TEST.run<YES,               vmI,        svM                     >(L_);
        TEST.run<YES,               vmI,       scvM                     >(L_);
        TEST.run<YES,               vmI,         wM                     >(L_);
        TEST.run< NO,               vmI,         wM,     I              >(L_);
        TEST.run<YES,               vmI,        wcM                     >(L_);
        TEST.run<YES,               vmI,        wvM                     >(L_);
        TEST.run<YES,               vmI,       wcvM                     >(L_);
        TEST.run<YES,              cvmI,          M                     >(L_);
        TEST.run< NO,              cvmI,          M,     I              >(L_);
        TEST.run<YES,              cvmI,         cM                     >(L_);
        TEST.run<YES,              cvmI,         vM                     >(L_);
        TEST.run<YES,              cvmI,        cvM                     >(L_);
        TEST.run<YES,              cvmI,         rM                     >(L_);
        TEST.run< NO,              cvmI,         rM,     I              >(L_);
        TEST.run<YES,              cvmI,        rcM                     >(L_);
        TEST.run<YES,              cvmI,        rvM                     >(L_);
        TEST.run<YES,              cvmI,       rcvM                     >(L_);
        TEST.run<YES,              cvmI,         pM                     >(L_);
        TEST.run< NO,              cvmI,         pM,     I              >(L_);
        TEST.run<YES,              cvmI,        pcM                     >(L_);
        TEST.run<YES,              cvmI,        pvM                     >(L_);
        TEST.run<YES,              cvmI,       pcvM                     >(L_);
        TEST.run<YES,              cvmI,        rpM                     >(L_);
        TEST.run< NO,              cvmI,        rpM,     I              >(L_);
        TEST.run<YES,              cvmI,       rpcM                     >(L_);
        TEST.run<YES,              cvmI,       rpvM                     >(L_);
        TEST.run<YES,              cvmI,      rpcvM                     >(L_);
        TEST.run<YES,              cvmI,         sM                     >(L_);
        TEST.run< NO,              cvmI,         sM,     I              >(L_);
        TEST.run<YES,              cvmI,        scM                     >(L_);
        TEST.run<YES,              cvmI,        svM                     >(L_);
        TEST.run<YES,              cvmI,       scvM                     >(L_);
        TEST.run<YES,              cvmI,         wM                     >(L_);
        TEST.run< NO,              cvmI,         wM,     I              >(L_);
        TEST.run<YES,              cvmI,        wcM                     >(L_);
        TEST.run<YES,              cvmI,        wvM                     >(L_);
        TEST.run<YES,              cvmI,       wcvM                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified, int-typed
        // member objects, applied to a derived class type
        TEST.run<YES,                mI,          D                     >(L_);
        TEST.run< NO,                mI,          D,     I              >(L_);
        TEST.run<YES,                mI,         cD                     >(L_);
        TEST.run<YES,                mI,         vD                     >(L_);
        TEST.run<YES,                mI,        cvD                     >(L_);
        TEST.run<YES,                mI,         rD                     >(L_);
        TEST.run< NO,                mI,         rD,     I              >(L_);
        TEST.run<YES,                mI,        rcD                     >(L_);
        TEST.run<YES,                mI,        rvD                     >(L_);
        TEST.run<YES,                mI,       rcvD                     >(L_);
        TEST.run<YES,               cmI,          D                     >(L_);
        TEST.run< NO,               cmI,          D,     I              >(L_);
        TEST.run<YES,               cmI,         cD                     >(L_);
        TEST.run<YES,               cmI,         vD                     >(L_);
        TEST.run<YES,               cmI,        cvD                     >(L_);
        TEST.run<YES,               cmI,         rD                     >(L_);
        TEST.run< NO,               cmI,         rD,     I              >(L_);
        TEST.run<YES,               cmI,        rcD                     >(L_);
        TEST.run<YES,               cmI,        rvD                     >(L_);
        TEST.run<YES,               cmI,       rcvD                     >(L_);
        TEST.run<YES,               vmI,          D                     >(L_);
        TEST.run< NO,               vmI,          D,     I              >(L_);
        TEST.run<YES,               vmI,         cD                     >(L_);
        TEST.run<YES,               vmI,         vD                     >(L_);
        TEST.run<YES,               vmI,        cvD                     >(L_);
        TEST.run<YES,               vmI,         rD                     >(L_);
        TEST.run< NO,               vmI,         rD,     I              >(L_);
        TEST.run<YES,               vmI,        rcD                     >(L_);
        TEST.run<YES,               vmI,        rvD                     >(L_);
        TEST.run<YES,               vmI,       rcvD                     >(L_);
        TEST.run<YES,              cvmI,          D                     >(L_);
        TEST.run< NO,              cvmI,          D,     I              >(L_);
        TEST.run<YES,              cvmI,         cD                     >(L_);
        TEST.run<YES,              cvmI,         vD                     >(L_);
        TEST.run<YES,              cvmI,        cvD                     >(L_);
        TEST.run<YES,              cvmI,         rD                     >(L_);
        TEST.run< NO,              cvmI,         rD,     I              >(L_);
        TEST.run<YES,              cvmI,        rcD                     >(L_);
        TEST.run<YES,              cvmI,        rvD                     >(L_);
        TEST.run<YES,              cvmI,       rcvD                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified member
        // functions.
        TEST.run< NO,               mFV                                 >(L_);
        TEST.run<YES,               mFV,          M                     >(L_);
        TEST.run< NO,               mFV,          M,     I              >(L_);
        TEST.run< NO,               mFV,         cM                     >(L_);
        TEST.run< NO,               mFV,         vM                     >(L_);
        TEST.run< NO,               mFV,        cvM                     >(L_);
        TEST.run<YES,               mFV,         rM                     >(L_);
        TEST.run< NO,               mFV,         rM,     I              >(L_);
        TEST.run< NO,               mFV,        rcM                     >(L_);
        TEST.run< NO,               mFV,        rvM                     >(L_);
        TEST.run< NO,               mFV,       rcvM                     >(L_);
        TEST.run<YES,               mFV,         pM                     >(L_);
        TEST.run< NO,               mFV,         pM,     I              >(L_);
        TEST.run< NO,               mFV,        pcM                     >(L_);
        TEST.run< NO,               mFV,        pvM                     >(L_);
        TEST.run< NO,               mFV,       pcvM                     >(L_);
        TEST.run<YES,               mFV,        rpM                     >(L_);
        TEST.run< NO,               mFV,        rpM,     I              >(L_);
        TEST.run< NO,               mFV,       rpcM                     >(L_);
        TEST.run< NO,               mFV,       rpvM                     >(L_);
        TEST.run< NO,               mFV,      rpcvM                     >(L_);
        TEST.run<YES,               mFV,         sM                     >(L_);
        TEST.run< NO,               mFV,         sM,     I              >(L_);
        TEST.run< NO,               mFV,        scM                     >(L_);
        TEST.run< NO,               mFV,        svM                     >(L_);
        TEST.run< NO,               mFV,       scvM                     >(L_);
        TEST.run<YES,               mFV,         wM                     >(L_);
        TEST.run< NO,               mFV,         wM,     I              >(L_);
        TEST.run< NO,               mFV,        wcM                     >(L_);
        TEST.run< NO,               mFV,        wvM                     >(L_);
        TEST.run< NO,               mFV,       wcvM                     >(L_);
        TEST.run<YES,              cmFV,          M                     >(L_);
        TEST.run< NO,              cmFV,          M,     I              >(L_);
        TEST.run<YES,              cmFV,         cM                     >(L_);
        TEST.run< NO,              cmFV,         vM                     >(L_);
        TEST.run< NO,              cmFV,        cvM                     >(L_);
        TEST.run<YES,              cmFV,         rM                     >(L_);
        TEST.run< NO,              cmFV,         rM,     I              >(L_);
        TEST.run<YES,              cmFV,        rcM                     >(L_);
        TEST.run< NO,              cmFV,        rvM                     >(L_);
        TEST.run< NO,              cmFV,       rcvM                     >(L_);
        TEST.run<YES,              cmFV,         pM                     >(L_);
        TEST.run< NO,              cmFV,         pM,     I              >(L_);
        TEST.run<YES,              cmFV,        pcM                     >(L_);
        TEST.run< NO,              cmFV,        pvM                     >(L_);
        TEST.run< NO,              cmFV,       pcvM                     >(L_);
        TEST.run<YES,              cmFV,        rpM                     >(L_);
        TEST.run< NO,              cmFV,        rpM,     I              >(L_);
        TEST.run<YES,              cmFV,       rpcM                     >(L_);
        TEST.run< NO,              cmFV,       rpvM                     >(L_);
        TEST.run< NO,              cmFV,      rpcvM                     >(L_);
        TEST.run<YES,              cmFV,         sM                     >(L_);
        TEST.run< NO,              cmFV,         sM,     I              >(L_);
        TEST.run<YES,              cmFV,        scM                     >(L_);
        TEST.run< NO,              cmFV,        svM                     >(L_);
        TEST.run< NO,              cmFV,       scvM                     >(L_);
        TEST.run<YES,              cmFV,         wM                     >(L_);
        TEST.run< NO,              cmFV,         wM,     I              >(L_);
        TEST.run<YES,              cmFV,        wcM                     >(L_);
        TEST.run< NO,              cmFV,        wvM                     >(L_);
        TEST.run< NO,              cmFV,       wcvM                     >(L_);
        TEST.run<YES,              vmFV,          M                     >(L_);
        TEST.run< NO,              vmFV,          M,     I              >(L_);
        TEST.run< NO,              vmFV,         cM                     >(L_);
        TEST.run<YES,              vmFV,         vM                     >(L_);
        TEST.run< NO,              vmFV,        cvM                     >(L_);
        TEST.run<YES,              vmFV,         rM                     >(L_);
        TEST.run< NO,              vmFV,         rM,     I              >(L_);
        TEST.run< NO,              vmFV,        rcM                     >(L_);
        TEST.run<YES,              vmFV,        rvM                     >(L_);
        TEST.run< NO,              vmFV,       rcvM                     >(L_);
        TEST.run<YES,              vmFV,         pM                     >(L_);
        TEST.run< NO,              vmFV,         pM,     I              >(L_);
        TEST.run< NO,              vmFV,        pcM                     >(L_);
        TEST.run<YES,              vmFV,        pvM                     >(L_);
        TEST.run< NO,              vmFV,       pcvM                     >(L_);
        TEST.run<YES,              vmFV,        rpM                     >(L_);
        TEST.run< NO,              vmFV,        rpM,     I              >(L_);
        TEST.run< NO,              vmFV,       rpcM                     >(L_);
        TEST.run<YES,              vmFV,       rpvM                     >(L_);
        TEST.run< NO,              vmFV,      rpcvM                     >(L_);
        TEST.run<YES,              vmFV,         sM                     >(L_);
        TEST.run< NO,              vmFV,         sM,     I              >(L_);
        TEST.run< NO,              vmFV,        scM                     >(L_);
        TEST.run<YES,              vmFV,        svM                     >(L_);
        TEST.run< NO,              vmFV,       scvM                     >(L_);
        TEST.run<YES,              vmFV,         wM                     >(L_);
        TEST.run< NO,              vmFV,         wM,     I              >(L_);
        TEST.run< NO,              vmFV,        wcM                     >(L_);
        TEST.run<YES,              vmFV,        wvM                     >(L_);
        TEST.run< NO,              vmFV,       wcvM                     >(L_);
        TEST.run<YES,             cvmFV,          M                     >(L_);
        TEST.run< NO,             cvmFV,          M,     I              >(L_);
        TEST.run<YES,             cvmFV,         cM                     >(L_);
        TEST.run<YES,             cvmFV,         vM                     >(L_);
        TEST.run<YES,             cvmFV,        cvM                     >(L_);
        TEST.run<YES,             cvmFV,         rM                     >(L_);
        TEST.run< NO,             cvmFV,         rM,     I              >(L_);
        TEST.run<YES,             cvmFV,        rcM                     >(L_);
        TEST.run<YES,             cvmFV,        rvM                     >(L_);
        TEST.run<YES,             cvmFV,       rcvM                     >(L_);
        TEST.run<YES,             cvmFV,         pM                     >(L_);
        TEST.run< NO,             cvmFV,         pM,     I              >(L_);
        TEST.run<YES,             cvmFV,        pcM                     >(L_);
        TEST.run<YES,             cvmFV,        pvM                     >(L_);
        TEST.run<YES,             cvmFV,       pcvM                     >(L_);
        TEST.run<YES,             cvmFV,        rpM                     >(L_);
        TEST.run< NO,             cvmFV,        rpM,     I              >(L_);
        TEST.run<YES,             cvmFV,       rpcM                     >(L_);
        TEST.run<YES,             cvmFV,       rpvM                     >(L_);
        TEST.run<YES,             cvmFV,      rpcvM                     >(L_);
        TEST.run<YES,             cvmFV,         sM                     >(L_);
        TEST.run< NO,             cvmFV,         sM,     I              >(L_);
        TEST.run<YES,             cvmFV,        scM                     >(L_);
        TEST.run<YES,             cvmFV,        svM                     >(L_);
        TEST.run<YES,             cvmFV,       scvM                     >(L_);
        TEST.run<YES,             cvmFV,         wM                     >(L_);
        TEST.run< NO,             cvmFV,         wM,     I              >(L_);
        TEST.run<YES,             cvmFV,        wcM                     >(L_);
        TEST.run<YES,             cvmFV,        wvM                     >(L_);
        TEST.run<YES,             cvmFV,       wcvM                     >(L_);

        // Function objects invoked with various kinds and sequences of
        // implicit conversions.
        TEST.run<YES, ConvertibleToFunc                                 >(L_);
        TEST.run<YES, ConvertibleToFunc, int                            >(L_);
        TEST.run<YES, ConvertibleToFunc, int       , float              >(L_);
        TEST.run<YES, ConvertibleToFunc, int       , float, short       >(L_);
        TEST.run< NO, ConvertibleToFunc, int       , float, short, short>(L_);
        TEST.run< NO, ConvertibleToFunc, int *                          >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float              >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float, short       >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float, short, short>(L_);
        TEST.run<YES, ConvertibleToFunc, char                           >(L_);
        TEST.run<YES, ConvertibleToFunc, char      , char               >(L_);
        TEST.run<YES, ConvertibleToFunc, char      , char , char        >(L_);
        TEST.run< NO, ConvertibleToFunc, char      , char , char , char >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong                     >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong, char               >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong, char , char        >(L_);
        TEST.run< NO, ConvertibleToFunc, ToLongLong, char , char , char >(L_);
#endif  //  defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `InvokeResultDeductionFailed`
        //
        // Concerns:
        // 1. `bslmf::InvokeResultDeductionFailed` can be constructed from any
        //    object type.
        // 2. `bslmf::InvokeResultDeductionFailed` can be constructed from any
        //    lvalue reference.
        // 3. The previous concerns apply if the initializer is cv qualified.
        // 4. In C++11 and later compilations,
        //    `bslmf::InvokeResultDeductionFailed` can be constructed from any
        //    rvalue reference.
        // 5. The above concerns apply to values that are the result of a
        //    function return.
        // 6. The `return` statement of a function returning a
        //    `bslmf::InvokeResultDeductionFailed` can specify an lvalue or
        //    rvalue of any type, resulting in the value being ignored.
        //
        // Plan:
        // 1. For concern 1, construct `bslmf::InvokeResultDeductionFailed`
        //    objects from value-initialized objects of numeric type,
        //    pointer type, class type, and enumeration type. There is nothing
        //    to verify -- simply compiling successfully is enough.
        // 2. For concern 2, create variables of the same types as in the
        //    previous step. Construct a `bslmf::InvokeResultDeductionFailed`
        //    object from each (lvalue) variable.
        // 3. For concern 3, repeat step 2, using a `const_cast` to add cv
        //    qualifiers to the lvalues.
        // 4. For concern 4, repeat step 2, applying `std::move` to each
        //    variable used to construct an object (C++11 and later only).
        // 5. For concern 5, implement a function `returnObj<TP>` that returns
        //    an object of type `TP`, a function `returnLvalueRef<TP>`, that
        //    returns a reference of type `TP&` and (for C++11 and later)
        //    `returnRvalueRef<TP>` that returns a reference of type
        //    `TP&&`. Construct a `bslmf::InvokeResultDeductionFailed` object
        //    from a call to each function template instantiated with each of
        //    the types from step 1 and various cv-qualifier combinations.
        // 6. For concern 6, implement function templates returning
        //    `bslmf::InvokeResultDeductionFailed` objects `discardObj<TP>`,
        //    `discardLvalueRef<TP>`, and `discardRvalueRef<TP>`. These
        //    functions respectively contain the statements 'return
        //    returnObj<TP>`, `return returnLvalueRef<TP>`, and `return
        //    returnRvalueRef<TP>'. Invoke each function with the same types
        //    as in step 5.
        //
        // Testing
        //     `InvokeResultDeductionFailed`
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `InvokeResultDeductionFailed`"
                            "\n=====================================\n");

        int            v1 = 0;
        bsl::nullptr_t v2;
        const char*    v3 = "hello";
        MyEnum         v4 = MY_ENUMERATOR;
        MyClass        v5;

        ASSERT(true); // Suppress "not used" warning

        // Step 1: Conversion from rvalue
        {
            bslmf::InvokeResultDeductionFailed x1(0);
            (void) x1; // Suppress "set but not used" warning

            bslmf::InvokeResultDeductionFailed x2 = bsl::nullptr_t();
            bslmf::InvokeResultDeductionFailed x3("hello");
            bslmf::InvokeResultDeductionFailed x4(MY_ENUMERATOR); // MyEnum
            bslmf::InvokeResultDeductionFailed x5 = MyClass();

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        // Step 2: Conversion from lvalue
        {
            bslmf::InvokeResultDeductionFailed x1 = v1;
            (void) x1; // Suppress "set but not used" warning
            bslmf::InvokeResultDeductionFailed x2 = v2;
            bslmf::InvokeResultDeductionFailed x3 = v3;
            bslmf::InvokeResultDeductionFailed x4 = v4;
            bslmf::InvokeResultDeductionFailed x5 = v5;

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        // Step 3: Conversion from cv-qualified lvalue
        {
            bslmf::InvokeResultDeductionFailed x1 = const_cast<const int&>(v1);
            bslmf::InvokeResultDeductionFailed x3 =
                const_cast<const char *volatile& >(v3);
            bslmf::InvokeResultDeductionFailed x4 =
                const_cast<const volatile MyEnum&>(v4);
            bslmf::InvokeResultDeductionFailed x5 =
                const_cast<const MyClass&>(v5);

            (void) x1; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Step 4: Conversion from rvalue reference
        {
            bslmf::InvokeResultDeductionFailed x1 = std::move(v1);
            bslmf::InvokeResultDeductionFailed x2 = std::move(v2);
            bslmf::InvokeResultDeductionFailed x3 = std::move(v3);
            bslmf::InvokeResultDeductionFailed x4 = std::move(v4);
            bslmf::InvokeResultDeductionFailed x5 = std::move(v5);

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Step 5: Initialization from function return
        {
            bslmf::InvokeResultDeductionFailed x1 = returnObj<int>();
            (void) x1; // Suppress "set but not used" warning
            bslmf::InvokeResultDeductionFailed x2 =
                returnObj<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 = returnObj<const char *>();
            bslmf::InvokeResultDeductionFailed x4 = returnObj<MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 = returnObj<MyClass>();

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        {
            bslmf::InvokeResultDeductionFailed x1 = returnLvalueRef<int>();
            bslmf::InvokeResultDeductionFailed x2 =
                returnLvalueRef<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 =
                returnLvalueRef<const char *const>();
            bslmf::InvokeResultDeductionFailed x4 =
                returnLvalueRef<volatile MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 =
                returnLvalueRef<const volatile MyClass>();

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        {
            bslmf::InvokeResultDeductionFailed x1 = returnRvalueRef<int>();
            bslmf::InvokeResultDeductionFailed x2 =
                returnRvalueRef<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 =
                returnRvalueRef<const char *>();
            bslmf::InvokeResultDeductionFailed x4 = returnRvalueRef<MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 = returnRvalueRef<MyClass>();

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Step 6: Return `bslmf::InvokeResultDeductionFailed`
        {
            discardObj<int>();
            discardObj<bsl::nullptr_t>();
            discardObj<const char *>();
            discardObj<MyEnum>();
            discardObj<MyClass>();

            discardLvalueRef<int>();
            discardLvalueRef<bsl::nullptr_t>();
            discardLvalueRef<const char *const>();
            discardLvalueRef<volatile MyEnum>();
            discardLvalueRef<const volatile MyClass>();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            discardRvalueRef<int>();
            discardRvalueRef<bsl::nullptr_t>();
            discardRvalueRef<const char *>();
            discardRvalueRef<MyEnum>();
            discardRvalueRef<MyClass>();
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR CLASS INVOCABLES
        //
        // Concerns:
        // 1. For invocables of functor class type, `bsl::invoke_result` can
        //    deduce the return type of `operator()` if that return type is a
        //    simple built-in integral type.
        // 2. The correct overload of `operator()` is selected, depending on
        //    the number and types of arguments.
        // 3. Appropriate overloads of `operator()` are selected depending on
        //    the const and/or volatile qualifiers on the functor type.
        // 4. The return-type deduction described in concerns 1-3 additionally
        //    extends to return types of:
        //     o `void`
        //     o `bsl::nullptr_t`
        //     o built-in numeric types
        //     o pointer to (possibly cv-qualified) numeric type or `void`
        //     o Reference to any of the (possibly cv-qualified) types above
        //       except `void`, including reference-to-pointer types. Rvalue
        //       references should be tested on C++11-compliant compilers.
        // 5. In C++03, if the invocable returns a user-defined type, pointer
        //    to a user-defined type, or reference to one of these, and the
        //    invocable has a nested type `result_type` or `ResultType`,
        //    `invoke_result` yields that nested type; otherwise it yields
        //    `bslmf::InvokeResultDeductionFailed`.  If the invocable has both
        //    `result_type` and `ResultType` nested types, then `invoke_result`
        //    yields `result_type`.  However, if the invocation would return a
        //    type listed in concern 4, `result_type` and `ResultType` are
        //    ignored.
        // 6. In C++11 or later, if the invocable returns a user-defined type,
        //    pointer to a user-defined type, or reference to one of these,
        //    `invoke_result` always yields the correct return type, regardless
        //     of whether or not there exists a nested `result_type` or
        //    `ResultType` type.
        // 7. The previous concerns apply when the invocable is a *reference*
        //    to functor class type.
        // 8. If `invoke_result<FN, args...>` is specialized or partially
        //    specialized for a specific `FN` and/or `args...`, that
        //    specialization is used instead of the primary template.  Such
        //    specializations can produce usable results even if deduction
        //    of the return value would have failed for the unspecialized
        //    template.
        // 9. The above concerns apply to functors taking 0 to 13 arguments.
        // 10. Though not technically a functor, a class that is convertible to
        //    a pointer-to-function or reference-to-function behaves like a
        //    functor within this component.  If the class is convertible to
        //    more than one function type, then the correct one is chosen
        //    based on overloading rules.  If the chosen (pointer or reference
        //    to) function returns a user-defined type, then return
        //    `bslmf::InvokeResultDeductionFailed` in C++03 mode.
        // 11. The `bsl::invoke_result_t` represents the expected type for
        //    invocables of functor class type.
        //
        // Plan:
        // 1. For concerns 1 and 2, define a functor class, `ManyFunc`, with
        //    `operator()` overloaded to return different integral types for a
        //    variety of argument combinations.  Verify that
        //    `bsl::invoke_result` yields the return type corresponding to the
        //    best overload for a set of specific arguments types.
        // 2. For concern 3, add `const` and `volatile` overloads. Using
        //    typedefs for `const` and `volitile` references to `ManyFunc`,
        //    verify that the best overload is selected.
        // 3. For concern 4 add overloads of `operator()` to `ManyFunc` that
        //    return `TP` when called with a first argument of type
        //    `MetaType<TP>`. Define a struct `FunctorTest` whose `apply<R>`
        //    method verifies that `invoke_result` yields the expected result
        //    of invoking a `ManyFunc` object with an argument of type
        //    `MetaType<R>`.  Invoke `FunctorTest` through the function
        //    `applyPtrAndRef` that adds every combination of pointer,
        //    reference, and cv qualifiers to `R`. Repeat this test with every
        //    numeric type and with `void` for `R`.
        // 4. For concerns 5 and 6, define three invocables: `FuncRt1<R>` that
        //    defines `result_type` as `R`, `FuncRt2<R>` that defines
        //    `ResultType` as `R`, and `FuncRt3<R>` that defines `result_type`
        //    as `R` and `ResultType` as `void`.  Each of them would define 'R
        //    operator()(const char*)` and `int operator()(int)'. Modify
        //    `FunctorTest` such that the method `FunctorTest<true>::apply<R>`
        //    verifies that, if `F` is `FuncRt?<R>` and `R` is one of the
        //    types listed in concern 5, `invoke_result<F, char*>::type` is
        //    `R`; `invoke_result<F, int>::type` is `int`, regardless of `R`;
        //    and `invoke_result<ManyFunc, MetaType<R>>::type` is
        //    `bslmf::InvokeResultDeductionFailed` in C++03 mode and `R` in
        //    C++11 mode.  Invoke `FunctorTest<true>` through the function
        //    `applyPtrAndRef` as in step 3, to generate every `R` with every
        //    combination of pointer and reference qualifiers to user-defined
        //    class `MyClass` and user-defined enumeration `MyEnum`.
        // 5. For concern 7, repeat each step using a reference as the first
        //    argument. For many of the tests, the modification would be in
        //    one or more `apply<R>` functions.
        // 6. For concern 8, add an easily-distinguished set of overloads to
        //    `ManyFunc::operator()` that return `MyClass`, `MyClass *`, etc..
        //    Create explicit specializations of `bsl::invoke_result` for those
        //    overloads.  Verify that the explicit specializations yield the
        //    expected type.
        // 7. For concern 9, instantiate `bsl::invoke_result` on overloads of
        //    `ManyFunc::operator()` taking 0 to 10 arguments. Concern 8 does
        //    not interact with the others, so it is not necessary to
        //    test every combination of 0 to 10 arguments with every possible
        //    return type.
        // 8. For concern 10, define a type, `ConvertibleToFunc` which is
        //    convertible to pointer-to-function and reference-to-function,
        //    where the result of each conversion has a different prototype.
        //    Instantiate `bsl::invoke_result` with arguments that select
        //    each of the overloads and verify the correct result.
        // 9. For concern 11, verify, for each of the parameter types specified
        //    in concern 4, that the type yielded by the `bsl::invoke_result_t`
        //    matches the type yielded by the `bsl::invoke_result`
        //    meta-function.
        //
        // Testing:
        //     FUNCTOR CLASS INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTOR CLASS INVOCABLES"
                            "\n================================\n");

        using BloombergLP::bslmf::InvokeResultDeductionFailed;

        typedef ManyFunc const           ManyFuncC;
        typedef ManyFunc volatile        ManyFuncV;
        typedef ManyFunc const volatile  ManyFuncCV;

#define TEST(exp, ...)                                                        \
    ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        // Step 1, concerns 1 and 2
        TEST(bool          , ManyFunc                                  );
        TEST(short         , ManyFunc,short                            );
        TEST(int           , ManyFunc,int,float                        );
        TEST(long          , ManyFunc,int,float,bool,short,unsigned,
                             long,int,int,int,float                    );
        TEST(unsigned int  , ManyFunc,int,short                        );
        TEST(unsigned short, ManyFunc,short,int                        );

        // Step 2 & 5, concern 3 and 7
        TEST(bool&         , ManyFunc,int, short, bool                 );
        TEST(bool&         , ManyFunc&,int, short, bool                );
        TEST(short&        , ManyFuncC,int, short, bool                );
        TEST(short&        , ManyFuncC&,int, short, bool               );
        TEST(int&          , ManyFuncV,int, short, bool                );
        TEST(int&          , ManyFuncV&,int, short, bool               );
        TEST(long&         , ManyFuncCV,int, short, bool               );
        TEST(long&         , ManyFuncCV&,int, short, bool              );

        // Step 3 & 5, concerns 4 and 7
        testCase5APRforVoid();
        testCase5APRforChar();
        testCase5APRforLongDouble();
        testCase5APRforNullptr();
        testCase5APRforOther();

        // Steps 4 & 5, concerns 5, 6, and 7
        testCase5APRforEnum();
        testCase5APRforClass();
        testCase5APRforDerivedClass();

        // Step 6, concern 8
        TEST(MyClass       , ManyFunc,char*,MetaType<MyClass>          );
        TEST(MyEnum *      , ManyFunc,char*,MetaType<MyEnum *>         );
        TEST(MyEnum *const&, ManyFunc,char*,MetaType<MyEnum *const&>   );

        // Step 7, concern 9
        typedef void     *P;
        typedef ToIntPtr  Q;
        typedef char     *S;
        TEST(bool        , ManyFunc                           );
        TEST(char        , ManyFunc,Q                         );
        TEST(short       , ManyFunc,P,S                       );
        TEST(int         , ManyFunc,S,Q,P                     );
        TEST(long        , ManyFunc,P,P,P,P                   );
        TEST(long long   , ManyFunc,P,P,P,P,P                 );
        TEST(float       , ManyFunc,P,P,P,P,P,P               );
        TEST(double      , ManyFunc,P,P,P,P,P,P,P             );
        TEST(long double , ManyFunc,P,P,P,P,P,P,P,P           );
        TEST(const char *, ManyFunc,P,P,P,P,P,P,P,P,P         );
        TEST(void       *, ManyFunc,P,P,P,P,P,P,P,P,P,Q       );

        // step 8, concern 10
        TEST(bool                       , ConvertibleToFunc               );
        TEST(short                      , ConvertibleToFunc,short         );
        TEST(int*                       , ConvertibleToFunc,int,float     );
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013
        TEST(MyClass                    , ConvertibleToFunc,int,float,bool);
#else
        TEST(InvokeResultDeductionFailed, ConvertibleToFunc,int,float,bool);
#endif
#undef TEST
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING POINTER-TO-MEMBER-OBJECT INVOCABLES
        //
        // Concerns:
        // 1. Given class `B`, class `D` derived from `B` and
        //    pointer-to-member type `typedef R B::*MP`,
        //    `bsl::invoke_result<MP, T1>`, yields an rvalue type `R` if `T1`
        //    is (rvalue type) `B` or `D`. In C++03 mode, the rvalue type is
        //    `R`; in C++11 and subsequent modes, the rvalue type is `R&&`.
        // 2. Similar to concern 1, except for `T1` being a smart pointer type
        //    which, when dereferenced, yields an rvalue of `B` or `D`. CV and
        //    reference qualifiers on the smart pointer have no effect.
        // 3. Given the same `MP` as in concern 1,
        //    `bsl::invoke_result<MP, T1>`, yields type `R&` if `T1` is `B&`
        //    or `D&`.
        // 4. Similar to concern 3, except with `T1` being a pointer or smart
        //    pointer type that, when dereferenced, yields an lvalue of type
        //    `B&` or `D&`.
        // 5. Similar to concerns 3 and 4, in C++11 and later,
        //    `bsl::invoke_result<MP, T1>`, yields type `R&&` if `T1` is `B&&`
        //    or is a smart pointer type that, when dereferenced, yields an
        //    rvalue reference of type `B&&` or `D&&`.
        // 6. If `B` or `D` in concerns 1-5 are cv-qualified, the result has
        //    the union of cv qualifications of `R` and the class type `B` or
        //    `D`. Reference and CV qualifiers on a pointer or smart pointer
        //    itself have no effect.
        // 7. Concerns 1-6 apply for `R` of the following types:
        //     o built-in numeric types
        //     o user-defined class and enumeration types
        //     o pointer (possibly cv-qualified) to any of the above
        //     o pointer to (possibly cv-qualified) void
        //     o cv-qualified types
        //     o arrays (which do not decay)
        // 8. The `bsl::invoke_result_t` represents the expected type for
        //    invocables of pointer-to-member-object type.
        //
        // Plan:
        // 1. For concern 1, define a functor template `PtrToMemObjTest<T1>`
        //    with an `apply<R>` member function template that declares a
        //    pointer to `R` member of `MyClass` and verifies that
        //    applying `bsl::invoke_result` to `R (MyClass::*)(T1)` yields
        //    `R`. Invoke this functor through a function `applyPtrAndRef`
        //    that adds every combination of pointer, reference, and const
        //    qualifiers to `R`. Use `MyClass` and `MyDerivedClass` for `T1`,
        //    along with a sampling of CV qualifications for those.
        // 2. For concern 2, add a parameter to `PtrToMemObjTest` to indicate
        //    "effective argument" type, i.e., the variation of `MyClass`
        //    yielded by the second template parameter to `invoke_result`.
        //    Create a smart pointer class template `ProxyPtr<TP>` that defines
        //    `TP operator*()`.  Repeat step 1, using `ProxyPtr<MyClass>` and
        //    `ProxyPtr<MyDerivedClass>` for `T1`, and `MyClass` and
        //    `MyDerivedClass`, respectively, for the effective argument
        //    type. Also try a sampling of CV qualifications on both the
        //    `ProxyPtr` and on its template parameter. Show that a reference
        //    qualifier on the `ProxyPtr` has no effect, as well.
        // 3. Repeat step 1 using `MyClass&` and `MyDerivedClass&` for `T1`
        //    and indicating an expected lvalue result.
        // 4. For concern 4, repeat step 3, using `MyClass*` and
        //    `MyDerivedClass*` insted of `MyClass&` and
        //    `MyDerivedClass&`.  Create a class template `SmartPtr<TP>` that
        //    defines `TP& operator*()` and repeat step 3, using
        //    `SmartPtr<MyClass>` and `SmartPtr<MyDerivedClass>` for `T1` and
        //    using `MyClass&` and `MyDerivedClass&`, respectively, for the
        //    effective argument type.  Use a sampling of reference and CV
        //    qualifiers on the `SmartPtr` (but not it's template parameter)
        //    to show that it has no effect.
        // 5. For concern 5, repeat step 3 using `MyClass&` and
        //    `MyDerivedClass&` for `T1` and indicating an expected rvalue
        //    reference result. Create a class template `MovePtr<TP>` that
        //    defines `TP&& operator*()` and repeat step 3 using
        //    `MovePtr<MyClass>` and `MovePtr<MyDerivedClass>` for `T1` and
        //    using `MyClass&&` and `MyDerivedClass&&`, respectively, for the
        //    effective argument type. Use a sampling of reference and CV
        //    qualifiers on the `MovePtr` (but not it's template parameter) to
        //    show that it has no effect.
        // 6. For concern 6, modify `PtrToMemObjTest` so that it expects the
        //    union of the cv-qualifications specified in the effective
        //    argument type parameter and those already on `R`. Repeat steps
        //    1-5, supplying different cv qualifications for the `MyClass`
        //    and `MyDerivedClass` parameters.
        // 7. For concern 7, modify steps 1-6 with `R` of type `void`,
        //    `int`, `float`, user-defined type `Ic01`, `const int`,
        //    `volatile short`, and `const volatile MyEnum`. Note that `void`
        //    will be ignored, but *pointer to (cv-qualified)* `void` will not.
        // 8. For concern 8, verify, for each of the parameter types specified
        //    in concern 7, that the type yielded by the `bsl::invoke_result_t`
        //    matches the type yielded by the `bsl::invoke_result`
        //    meta-function.
        //
        // Testing:
        //     POINTER-TO-MEMBER-OBJECT INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POINTER-TO-MEMBER-OBJECT INVOCABLES"
                            "\n===========================================\n");

        typedef const int                               Cint;
        typedef volatile short                          Vshort;
        typedef const volatile MyEnum                   CVenum;
        typedef int                                     Arry[10][5];

        typedef MyClass                                 Mc;
        typedef MyClass const                           CMc;
        typedef MyClass volatile                        VMc;
        typedef MyClass const volatile                  CvMc;
        typedef MyDerivedClass                          Mdc;
        typedef MyDerivedClass const                    CMdc;
        typedef MyDerivedClass volatile                 VMdc;
        typedef MyDerivedClass const volatile           CvMdc;

        static const ValueCategory::Enum RV = ValueCategory::e_PRVALUE;

        // Step 1: rvalue of `MyClass` or `MyDerivedClass`
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Arry  >(L_);

        // Step 2: rvalue proxy of `MyClass` or `MyDerivedClass`
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Arry  >(L_);

        // Reference qualifications on `ProxyPtr` have no affect
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>&, Mc, RV>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const ProxyPtr<Mdc>&, Mc, RV>
                                                                , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<const volatile ProxyPtr<Mc>&, Mc, RV>
                                                                 , int>(L_);

        // Step 3: lvalue of `MyClass` or `MyDerivedClass`
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Arry  >(L_);

        // Step 4a: pointer to `MyClass` or `MyDerivedClass`
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Arry  >(L_);

        // Step 4b: smart pointer to (lvalue of) `MyClass` or `MyDerivedClass`
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Arry  >(L_);

        // Reference and CV qualifications on `SmartPtr` have no affect
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>&, CMc&>     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const SmartPtr<VMc>&, VMc&>
                                                                 , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<const volatile SmartPtr<Mc>&, Mc&>
                                                                 , int>(L_);

        // Step 5a: rvalue reference to `MyClass` or `MyDerivedClass`
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Arry  >(L_);

        // Step 5b: smart pointer returning `MyClass&&` or `MyDerivedClass&&`
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Arry  >(L_);

        // Reference and CV qualifications on `MovePtr` have no affect
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>&&, CMc&&>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const volatile MovePtr<Mc>&, Mc&&>
                                                                 , int>(L_);
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<Mc>&, Mc&&> , CVenum>(L_);

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // `MovePtr<volatile MyClass>` conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<VMc>&, VMc&&>
                                                                 , CVenum>(L_);
#endif // ! MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Step 6: Expect CV-qualification on reference should be union of
        // cv-qualification on the effective argument and the cv-qualification
        // of the member.
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&>                     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc&>                   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , CVenum>(L_);

    // Older sun compiler gets confused returning a const-qualified rvalue.
#if ! (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMdc>, CMc, RV>  , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , CVenum>(L_);
#endif // Not Sun CC
#if 0 // Returning volatile prvalues is problematic in most compilers
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , Arry>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMdc>, CvMc>    , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , Arry>();
#endif // 0

        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc*, CMc&>              , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>               , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc*, CvMc&>            , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMdc>, CMc&>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMdc>, CvMc&>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , CVenum>(L_);

#ifndef MSVC_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , Arry  >(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , Arry  >(L_);
#endif
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc&&>                   , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc&&>                  , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMdc>, CMc&&>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , CVenum>(L_);

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // `MovePtr<volatile MyClass>` conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMdc>, CvMc&&>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , Arry  >(L_);
#endif // MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES
        //
        // Concerns:
        // 1. For invocables of type pointer-to-member-function,
        //    `bsl::invoke_result` returns the return type of the function.
        // 2. Concern 1 applies to functions returning
        //     o `void`
        //     o built-in numeric types
        //     o user-defined class and enumeration types
        //     o pointer (possibly cv-qualified) to any of the above
        //     o Pointer-to-function, reference-to-function, and
        //       pointer-to-member function.  No recursion is expected.
        //     o Reference to any of the (possibly cv-qualified) types above
        //       except `void`, including reference-to-pointer types.
        //     o Reference-to-array or (in C++11) rvalue-reference-to-array.
        // 3. Given, `bsl::invoke_result<F, T1, ...>`, where `F` is a pointer
        //    to member function of class `B` and class `D` is either `B` or a
        //    class derived from `B`, `T1` can be `D`, cv-qualified `D&`,
        //    `bsl::reference_wrapper<D>`, `D*`, or `smart_ptr<D>` for some
        //    smart-pointer template.
        // 4. Concern 1 applies to member functions that are cv-qualified
        //    and/or ref-qualified.
        // 5. Concern 1 applies for pointers to member functions taking 0 to 9
        //    arguments, where some of the arguments are convertible to the
        //     function parameters, rather than being an exact match.
        // 6. The `bsl::invoke_result_t` represents the expected type for
        //    invocables of pointer-to-member-function type.
        //
        // Plan:
        // 1. For concerns 1 and 2, define a functor template
        //    `PtrToMemFuncTest` which declares a pointer to
        //    member-function-returning-R type and verifies that applying
        //    `bsl::invoke_result` to that type yields `R`. Invoke this functor
        //    through a function `applyPtrAndRef` that adds every combination
        //    of pointer and reference qualifiers to `R`.
        // 2. For concern 3, parameterize `PtrToMemFuncTest` on the type of
        //    the first argument. Repeat step 1, supplying different
        //    first-argument types that comprise the types in the concern.
        // 3. For concern 4, include in `PtrToMemFuncTest` pointers to member
        //    functions that are lvalue-ref-qualified and
        //    rvalue-ref-qualified, with every combination of cv-qualifiers.
        // 4. For concern 5, instantiate `bsl::invoke_result` on
        //    member-function-pointer types taking 0 to 9 arguments.
        //    Concerns 2 and 3 do not interact, so it is not necessary to
        //    test every combination of 0 to 9 arguments with every possible
        //    return type.
        // 5. For concern 6, verify, for each of the parameter types specified
        //    in concern 2, that the type yielded by the `bsl::invoke_result_t`
        //    matches the type yielded by the `bsl::invoke_result`
        //    meta-function.
        //
        // Testing:
        //     POINTER-TO-MEMBER-FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES"
                          "\n=============================================");

        typedef Ic01 (*Fp)(MyClass, int);
        typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Arry[10];
        typedef Ic04 F(MyClass, int);

        applyPtrAndRef<PtrToMemFuncTest<MyClass>, void   >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, int    >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, float  >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyClass>(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyEnum >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, Fp     >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MFp    >(L_);

        // Arrays and functions cannot be returned by value, so we test only
        // references.  Can't apply a pointer or reference to something that is
        // already a reference, so simply call the `apply` method directly.
        // MSVC 2022 and earlier fails to propagate the reference qualifier.
        // However, MSVC 2013 uses the C++03 implementation of
        // `bsl::invoke_result`, which calculates the return type manually,
        // without `decltype` machinery, and is correct.
#if MSVC_2022 || MSVC_2019 || MSVC_2017 || MSVC_2015
        PtrToMemFuncTest<MyClass>::apply<Arry&      , Arry>(L_);
        PtrToMemFuncTest<MyClass>::apply<Arry const&, Arry const>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<Arry&>(L_);
        PtrToMemFuncTest<MyClass>::apply<Arry const&>(L_);
#endif
        PtrToMemFuncTest<MyClass>::apply<F&>(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#if MSVC_2022 || MSVC_2019 || MSVC_2017 || MSVC_2015
        PtrToMemFuncTest<MyClass>::apply<Arry&&, Arry>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<Arry&&>(L_);
#endif

        // Rvalue references to functions are special in that they are lvalues,
        // unlike rvalue references to other types, which are conditionally
        // either lvalues or xvalues.  MSVC 2022 and earlier appears to get
        // this wrong.
#if MSVC
        PtrToMemFuncTest<MyClass>::apply<F&&, F&&>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<F&&, F&>(L_);
#endif
#endif

        typedef int        i;
        typedef ToLongLong q;
        typedef short      s;

        typedef Ic01 (MyClass::*Fp01)();
        typedef Ic02 (MyClass::*Fp02)(i);
        typedef Ic03 (MyClass::*Fp03)(i,i);
        typedef Ic04 (MyClass::*Fp04)(i,i,i);
        typedef Ic05 (MyClass::*Fp05)(i,i,i,i);
        typedef Ic05 (MyClass::*Fp05c)(i,i,i,i) const;
        typedef Ic05 (MyClass::*Fp05v)(i,i,i,i) volatile;
        typedef Ic05 (MyClass::*Fp05cv)(i,i,i,i) const volatile;
        typedef Ic06 (MyClass::*Fp06)(i,i,i,i,i);
        typedef Ic06 (MyClass::*Fp06c)(i,i,i,i,i) const;
        typedef Ic06 (MyClass::*Fp06v)(i,i,i,i,i) volatile;
        // typedef Ic06 (MyClass::*Fp06cv)(i,i,i,i,i) const volatile;
        typedef Ic07 (MyClass::*Fp07)(i,i,i,i,i,i);
        typedef Ic08 (MyClass::*Fp08)(i,i,i,i,i,i,i);
        typedef Ic09 (MyClass::*Fp09)(i,i,i,i,i,i,i,i) const;
        typedef Ic10 (MyClass::*Fp10)(i,i,i,i,i,i,i,i,i) const;
        // typedef Ic11 (MyClass::*Fp11)(i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic12 (MyClass::*Fp12)(i,i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic13 (MyClass::*Fp13)(i,i,i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic14 (MyClass::*Fp14)(i,i,i,i,i,i,i,i,i,i,i,i,i);

#define TEST(exp, ...) \
      ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        TEST(Ic01, Fp01,MyClass                           );
        TEST(Ic02, Fp02,MyClass,q                         );
        TEST(Ic03, Fp03,MyClass,i,s                       );
        TEST(Ic04, Fp04,MyClass,s,i,q                     );
        TEST(Ic05, Fp05,MyClass,q,s,q,s                   );
        TEST(Ic05, Fp05c,MyClass,i,i,i,i                  );
        TEST(Ic05, Fp05v,MyClass,i,i,i,i                  );
        TEST(Ic05, Fp05cv,MyClass,i,i,i,i                 );
        TEST(Ic06, Fp06,MyClass,i,i,i,i,i                 );
        TEST(Ic07, Fp07,MyClass,i,i,i,i,i,i               );
        TEST(Ic08, Fp08,MyClass,i,i,i,i,i,i,i             );
        TEST(Ic09, Fp09,MyClass,i,i,i,i,i,i,i,i           );
        TEST(Ic10, Fp10,MyClass,i,i,i,i,i,i,i,i,q         );
        // TEST(Ic11, Fp11,MyClass,i,i,i,i,i,i,i,i,i,i       );
        //
        // TEST(Ic12, Fp12,MyClass,i,i,i,i,i,i,i,i,i,i,i     );
        //
        // TEST(Ic13, Fp13,MyClass,i,i,i,i,i,i,i,i,i,i,i,i   );
        //
        // TEST(Ic14, Fp14,MyClass,i,i,i,i,i,i,i,i,i,i,i,i,i );

        typedef SmartPtr<MyClass>              SpMc;
        typedef SmartPtr<MyDerivedClass>       SpMdc;
        typedef SmartPtr<const MyClass>        SpCMc;
        typedef SmartPtr<const MyDerivedClass> SpCMdc;

        typedef stub_reference_wrapper<MyClass>    Rr;

        TEST(Ic01, Fp01  , MyClass&                                  );
        TEST(Ic02, Fp02  , MyClass*,i                                );
        TEST(Ic03, Fp03  , MyDerivedClass,q,s                        );
        TEST(Ic04, Fp04  , MyDerivedClass*,i,i,i                     );
        TEST(Ic05, Fp05  , MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05c , MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05v , MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05cv, MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05c , const MyDerivedClass&,i,i,i,i             );
        TEST(Ic05, Fp05cv, const MyDerivedClass&,i,i,i,i             );
        TEST(Ic05, Fp05v , volatile MyDerivedClass&,i,i,i,i          );
        TEST(Ic05, Fp05cv, volatile MyDerivedClass&,i,i,i,i          );
        TEST(Ic05, Fp05cv, const volatile MyDerivedClass&,i,i,i,i    );
        TEST(Ic06, Fp06c , const MyClass&,i,i,i,i,i                  );
        TEST(Ic06, Fp06v , volatile MyClass&,i,i,i,i,i               );
        TEST(Ic07, Fp07  , SpMc,i,i,i,i,i,i                          );
        TEST(Ic08, Fp08  , SpMdc,i,i,q,i,i,i,i                       );
        TEST(Ic09, Fp09  , SpCMc,i,s,i,i,i,i,i,i                     );
        TEST(Ic10, Fp10  , SpCMdc,q,i,i,i,i,i,i,i,i                  );

        TEST(Ic01, Fp01  , Rr                                        );
        TEST(Ic02, Fp02  , Rr,i                                      );
        TEST(Ic03, Fp03  , Rr&,i,i                                   );
#undef TEST
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION INVOCABLES
        //
        // Concerns:
        // 1. For invocables of function type,
        //    `bsl::invoke_result` returns the return type of the function.
        // 2. Concern 1 applies to functions returning
        //     o `void`
        //     o built-in numeric types
        //     o user-defined class and enumeration types
        //     o pointer (possibly cv-qualified) to any of the above
        //     o Pointer-to-function, reference-to-function, and
        //       pointer-to-member function.  No recursion is expected.
        //     o Reference to any of the (possibly cv-qualified) types above
        //       except `void`, including reference-to-pointer types.
        //     o Reference-to-array or (in C++11) rvalue-reference-to-array.
        // 3. Concern 1 applies for pointers to functions taking 0 to 10
        //    arguments, where some arguments are convertible to the function
        //    argument rather than an exact match.
        // 4. Concerns 1 to 3 apply equally for invocables of
        //    pointer-to-function type.
        // 5. Concerns 1 to 3 apply equally for invocables of (lvalue & rvalue)
        //    reference-to-function type.
        // 6. Results are not affected by arguments that decay (arrays,
        //    functions), reference binding, or valid volatile arguments.
        // 7. The `bsl::invoke_result_t` represents the expected type for
        //    invocables of function type.
        //
        // Plan:
        // 1. For concerns 1 and 2, define a functor template `FuncTest` which
        //    declares a function-returning-R type and verifies that applying
        //    `bsl::invoke_result` to that type yields `R`. Invoke this functor
        //    through a function `applyPtrAndRef` that adds every combination
        //    of pointer and reference qualifiers to `R`.
        // 2. For concerns 1 and 3, instantiate `bsl::invoke_result` on a
        //    function type taking 0 to 10 arguments.  Concerns 2 and
        //   3. do not interract, so it is not necessary to test every
        //    combination of 0 to 10 arguments with every possible return
        //    type.
        // 3. For concern 4, extend `FuncTest` to also test
        //    pointer-to-function, reference to pointer-to-function, and
        //    reference to `const` pointer-to-function.  Also repeat step 2
        //    with pointer-to-function types.
        // 4. For concern 5, extend `FuncTest` to also test (lvalue & rvalue)
        //    reference-to-function.  Also repeat step 2 with
        //    reference-to-function types.
        // 5. For concern 6, invoke `bsl::invoke_result` in a one-off fashion
        //    with the argument categories listed in the concern and verify
        //    correct operation.
        // 6. For concern 7, verify, for each of the parameter types specified
        //    in concern 2, that the type yielded by the `bsl::invoke_result_t`
        //    matches the type yielded by the `bsl::invoke_result`
        //    meta-function.
        //
        // Testing:
        //     FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION INVOCABLES"
                            "\n===========================\n");

        BSLA_MAYBE_UNUSED typedef Ic01 (*Fp)(MyClass, int);
        BSLA_MAYBE_UNUSED typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Elem;
        typedef Elem Arry[10];
        typedef Ic04 F(MyClass, int);

        // Step 1, Concerns 1 & 2
        applyPtrAndRef<FuncTest, void>(L_);
        applyPtrAndRef<FuncTest, int>(L_);
        applyPtrAndRef<FuncTest, float>(L_);
        applyPtrAndRef<FuncTest, MyClass>(L_);
        applyPtrAndRef<FuncTest, MyEnum>(L_);

        // Arrays and functions cannot be returned by value, so we test only
        // references.  We can't apply a pointer or reference to something that
        // is already a reference, so simply call the `apply` method directly.
        // MSVC 2022 and earlier fails to propagate the reference qualifier.
        // However, MSVC 2013 uses the C++03 implementation of
        // `bsl::invoke_result`, which correctly calculates the return type.

#if MSVC_2022 || MSVC_2019 || MSVC_2017 || MSVC_2015
        FuncTest::apply<Arry&      , Arry>(L_);
        FuncTest::apply<Arry const&, Arry const>(L_);
#else
        FuncTest::apply<Arry&>(L_);
        FuncTest::apply<Arry const&>(L_);
#endif
        FuncTest::apply<F&>(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#if MSVC_2022 || MSVC_2019 || MSVC_2017 || MSVC_2015
        FuncTest::apply<Arry&&, Arry>(L_);
#else
        FuncTest::apply<Arry&&>(L_);
#endif

        // Rvalue references to functions are special in that they are lvalues,
        // unlike rvalue references to other types, which are conditionally
        // either lvalues or xvalues.  MSVC 2022 and earlier appears to get
        // this wrong.
#if MSVC
        FuncTest::apply<F&&, F&&>(L_);
#else
        FuncTest::apply<F&&, F&>(L_);
#endif
#endif

        // Step 2, Concern 3
        typedef int        i;
        typedef ToLongLong q;
        typedef short      s;

        typedef Ic00 F00();
        typedef Ic01 F01(i);
        typedef Ic02 F02(i,i);
        typedef Ic03 F03(i,i,i);
        typedef Ic04 F04(i,i,i,i);
        typedef Ic05 F05(i,i,i,i,i);
        typedef Ic06 F06(i,i,i,i,i,i);
        typedef Ic07 F07(i,i,i,i,i,i,i);
        typedef Ic08 F08(i,i,i,i,i,i,i,i);
        typedef Ic09 F09(i,i,i,i,i,i,i,i,i);
        typedef Ic10 F10(i,i,i,i,i,i,i,i,i,i);


#define TEST(exp, ...) \
      ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        TEST(Ic00, F00                             );
        TEST(Ic01, F01,i                           );
        TEST(Ic02, F02,q,i                         );
        TEST(Ic03, F03,i,s,i                       );
        TEST(Ic04, F04,q,s,q,s                     );
        TEST(Ic05, F05,i,i,i,i,i                   );
        TEST(Ic06, F06,i,i,i,i,i,i                 );
        TEST(Ic07, F07,i,i,i,q,i,i,i               );
        TEST(Ic08, F08,i,q,i,i,i,i,i,s             );
        TEST(Ic09, F09,i,i,i,i,i,i,i,i,i           );
        TEST(Ic10, F10,s,i,i,i,i,i,i,i,i,i         );

        TEST(Ic00, F00*                             );
        TEST(Ic01, F01*,s                           );
        TEST(Ic02, F02*,i,q                         );
        TEST(Ic03, F03*,i,i,i                       );
        TEST(Ic04, F04*,i,i,i,i                     );
        TEST(Ic05, F05*,i,i,i,i,i                   );
        TEST(Ic06, F06*,s,q,s,q,s,q                 );
        TEST(Ic07, F07*,i,i,i,i,i,i,i               );
        TEST(Ic08, F08*,i,i,i,i,i,i,i,i             );
        TEST(Ic09, F09*,i,i,i,i,s,i,i,i,i           );
        TEST(Ic10, F10*,i,i,i,i,i,q,i,i,i,i         );


        TEST(Ic00, F00&                             );
        TEST(Ic01, F01&,i                           );
        TEST(Ic02, F02&,i,i                         );
        TEST(Ic03, F03&,q,s,q                       );
        TEST(Ic04, F04&,i,i,i,i                     );
        TEST(Ic05, F05&,i,i,i,i,i                   );
        TEST(Ic06, F06&,i,i,s,i,i,i                 );
        TEST(Ic07, F07&,i,i,i,q,i,s,i               );
        TEST(Ic08, F08&,i,i,i,i,i,i,i,i             );
        TEST(Ic09, F09&,i,q,i,s,i,q,i,s,i           );
        TEST(Ic10, F10&,i,i,i,i,i,i,i,i,i,i         );

        // Steps 3 & 4 incorporated into `FuncTest` (step 1) and into step 2

        // Step 5, concern 6
        typedef Ic05 FArry(Ic03[6]);
        typedef Ic06 FArryUB(Ic03[]);
        typedef Ic07 FPtr(Ic03*);
        TEST(Ic04, F       , MyClass,        volatile int );
        TEST(Ic04, F       , MyDerivedClass, volatile int&);
        TEST(Ic04, F&      , MyClass,        volatile int );
        TEST(Ic04, F*      , MyClass,        volatile int );
        TEST(Ic05, FArry   , Arry                         );
        TEST(Ic05, FArry   , Arry&                        );
        TEST(Ic05, FArry&  , Arry                         );
        TEST(Ic05, FArry*  , Arry&                        );
        TEST(Ic06, FArryUB , Arry                         );
        TEST(Ic06, FArryUB , Arry&                        );
        TEST(Ic06, FArryUB&, Arry&                        );
        TEST(Ic06, FArryUB*, Arry                         );
        TEST(Ic07, FPtr    , Arry                         );
        TEST(Ic07, FPtr    , Arry&                        );
        TEST(Ic07, FPtr&   , Arry                         );
        TEST(Ic07, FPtr*   , Arry                         );
#undef TEST
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. `bsl::invoke_result<FT, ARGS...>` compiles for a sampling of
        //    parameter types
        //
        // 2. The `bsl::invoke_result_t<FT, ARGS...>` represents the expected
        //    type for a sampling of parameter types
        //
        // Plan:
        // 1. Instantiate `bsl::resulf_of` on a sampling of invocable types
        //    and verify that the computed return type matches the expected
        //    return type.
        //
        // 2. Compare `bsl::invoke_result_t` type and the type yielded by the
        //    `bsl::invoke_result` meta-function for a variety of template
        //    parameter types.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
#define TEST(exp, ...)                                                        \
        ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type,           \
                exp>::value))

#define TEST_T(...)                                                           \
        ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type,           \
                             bsl::invoke_result_t<__VA_ARGS__> >::value))

        typedef void (*f1)();
        typedef int (*f2)(int);
        typedef const int& (*f3)(bool, double);
        typedef volatile int& (*f4)(const char*, const int&, int*);
        typedef const volatile int& (*f5)(int, int, int, int);
        typedef ManyFunc (&f6)(bool);
        typedef double (MyClass::*g1)(int);
        typedef double& (MyClass::*g2)(volatile int);
        typedef char *MyClass::*dp;
        typedef const ManyFunc& ConstManyFunc;

        //   Expected             Arguments
        //   ===================  ========================================
        TEST(void               , f1                                      );
        TEST(int                , f2, char                                );
        TEST(const int&         , f3, bool, float                         );
        TEST(volatile int&      , f4, char*, int, int*                    );
        TEST(const volatile int&, f5, short, int&, int, char              );
        TEST(ManyFunc           , f6, bool                                );
        TEST(double             , g1, MyClass, int                        );
        TEST(double&            , g2, MyClass, int                        );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(char*&&            , dp, MyClass                             );
#else
        TEST(char*              , dp, MyClass                             );
#endif
        TEST(bool               , ManyFunc, float, void*                  );
        TEST(float              , ConstManyFunc, float, void*             );

        TEST(void               , ManyFunc, MetaType<void>, int           );
        TEST(int *              , ManyFunc, MetaType<int *>, int          );
#ifndef BSLS_PLATFORM_CMP_SUN
        TEST(int *              , ManyFunc, MetaType<int *volatile>, int  );
#endif
        TEST(int *volatile&     , ManyFunc, MetaType<int *volatile&>, int );
        TEST(const void *       , ManyFunc, MetaType<const void *>, int   );
        TEST(volatile void *const&, ManyFunc
                                , MetaType<volatile void *const&>, int    );
        TEST(const char*        , ManyFunc, MetaType<const char*>, int    );

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        // Testing `bsl::invoke_result_t`.

        //     Arguments
        //     ===============================================
        TEST_T(f1                                             );
        TEST_T(f2, char                                       );
        TEST_T(f3, bool, float                                );
        TEST_T(f4, char*, int, int*                           );
        TEST_T(f5, short, int&, int, char                     );
        TEST_T(f6, bool                                       );
        TEST_T(g1, MyClass, int                               );
        TEST_T(g2, MyClass, int                               );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST_T(dp, MyClass                                    );
#else
        TEST_T(dp, MyClass                                    );
#endif
        TEST_T(ManyFunc, float, void*                         );
        TEST_T(ConstManyFunc, float, void*                    );

        TEST_T(ManyFunc, MetaType<void>, int                  );
        TEST_T(ManyFunc, MetaType<int *>, int                 );
#ifndef BSLS_PLATFORM_CMP_SUN
        TEST_T(ManyFunc, MetaType<int *volatile>, int         );
#endif
        TEST_T(ManyFunc, MetaType<int *volatile&>, int        );
        TEST_T(ManyFunc, MetaType<const void *>, int          );
        TEST_T(ManyFunc, MetaType<volatile void *const&>, int );
        TEST_T(ManyFunc, MetaType<const char*>, int           );

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // BDE_VERIFY pragma: pop

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

#endif // End C++11 code

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
