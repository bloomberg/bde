// bslmf_invokeresult.00.t.cpp                                        -*-C++-*-
//
// This test driver contains common test machinery for all
// 'bslstl_invokeresult' test drivers.  If '#include'd after defining the
// 'BSLMF_INVOKERESULT_00T_AS_INCLUDE' macro it will just provide those common
// pieces (and no 'main' function).

#ifndef BSLMF_INVOKERESULT_00T_INCLUDED
#define BSLMF_INVOKERESULT_00T_INCLUDED

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

#include <utility>   // native_std::move for C++11

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

#include <bsls_compilerfeatures.h>

// Workaround for the fact that the cpp11 feature simulation script adds the
// variable 'COMPILING_BSLMF_INVOKERESULT_T_CPP' in the generated code, but
// sets the variable 'COMPILING_BSLMF_INVOKERESULT_00_T_CPP' in the generated
// section below.
#define COMPILING_BSLMF_INVOKERESULT_T_CPP

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu May 27 12:55:56 2021
// Command line: sim_cpp11_features.pl bslmf_invokeresult.00.t.cpp
# define COMPILING_BSLMF_INVOKERESULT_00_T_CPP
# include <bslmf_invokeresult_cpp03.00.t.cpp>
# undef COMPILING_BSLMF_INVOKERESULT_00_T_CPP
#else

#if defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif


using namespace BloombergLP;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component is a transformation metafunction.  Testing consists primarily
// of instantiating 'bsl::invoke_result<FN, Args...>' with representative
// combinations of 'Fn' and 'Args' and verifying that the resulting 'type' is
// what was expected.  The test is broken up as one test case per category of
// invocable 'Fn' parameter.
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

                      // =============================================
                      // Common function for test case banner printing
                      // =============================================

namespace {

BSLA_MAYBE_UNUSED
void referUserToElsewhere(int test, bool verbose)
    // I the specified 'verbose' then print a message referring to the file
    // containing the case for the specified 'test'
{
    if (verbose)
         printf("Test case %d is in another test driver part "
                "bslm_invokeresult.%02d.t.cpp\n"
                "See 'bslstl_invokeresult.00.t.cpp' for the test plan.\n",
                test, test);
}

}  // close unnamed namespace

                      // ===========================================
                      // Set macros to indicate MSVC bug workarounds
                      // ===========================================

#if defined(BSLS_PLATFORM_CMP_MSVC)
// The MSVC 18.0 and 19.0 compilers have a bug whereby an expression like,
// 'func().*ptrtomem', where 'func()' returns 'volatile SomeClass&&' and
// 'ptrtomem' is a declared as 'RET SomeClass::*ptrtomem', yields an lvalue
// instead of an rvalue reference. Defining this macro will disable certain
// tests that would fail using the MSVC compiler.
#   define MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG

// These compilers also have a bug whereby similar expressions where 'func()'
// returns a const- and/or volatile-qualified class type, and 'ptrtomem' is
// declared as a 'RET SomeClass::*ptrtomem' yields an expression of
// reference-to-'RET' type lacking the appropriate qualifiers if 'RET' is an
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

namespace {

                      // ================================
                      // class template reference_wrapper
                      // Required for test cases 3,7
                      // ================================

// Required for test cases 3,7
template <class TP>
class stub_reference_wrapper {
    // This class template provides a stub implementation of
    // 'bsl::reference_wrapper' with just enough real interface to allow code
    // within this test driver to compile.  The real 'reference_wrapper' is
    // not available, as it is in a higher-level component than this one.

  public:
    // ACCESSORS
    operator TP&() const BSLS_KEYWORD_NOEXCEPT;
        // This member operator is declared but not defined.

    TP& get() const BSLS_KEYWORD_NOEXCEPT;
        // This member function is declared but not defined.
};

}  // close unnamed namespace


namespace BloombergLP {
namespace bslmf {

// Required for test cases 3,7
template <class TP>
struct IsReferenceWrapper<stub_reference_wrapper<TP> > : bsl::true_type {
};

}  // close package namespace
}  // close enterprise namespace


#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

                       // ===============================
                       // struct template IsInvokeResult
                       // Required for test cases 2,3,4,7
                       // ===============================

namespace {

// Required for test cases 2,3,4,7
template <class EXPECTED_INVOKE_RESULT, class INVOCABLE, class... ARGS>
struct IsInvokeResult
: bsl::is_same<EXPECTED_INVOKE_RESULT,
               typename bsl::invoke_result<INVOCABLE, ARGS...>::type> {
    // This metafunction 'struct' provides a compile-time constant, Boolean
    // data member 'value' that is 'true' if 'bsl::invoke_result<INVOCABLE,
    // ARGS...>::type' is the same as the specified 'EXPECTED_INVOKE_RESULT'
    // type, and is 'false' otherwise.  Compilation will fail unless
    // 'bsl::invoke_result<INVOCABLE, ARGS...>' provides a nested typedef
    // 'type'.
};

}  // close unnamed namespace

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5

                   // =====================================
                   // struct template IsInvokeResultDefined
                   // Required for test cases 2,3,4,7
                   // =====================================

namespace {

// Required for test cases 2,3,4,7
template <class VOID, class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp;
    // forward declaration

// Required for test cases 2,3,4,7
template <class INVOCABLE, class... ARGS>
struct IsInvokeResultDefined
: IsInvokeResultDefinedImp<void, INVOCABLE, ARGS...> {
    // This metafunction 'struct' provides a compile-time constant, Boolean
    // data member 'value' that is 'true' if 'bsl::invoke_result<INVOCABLE,
    // ARGS...>' provides a nested typedef 'type', and is 'false' otherwise.
};

                  // ========================================
                  // struct template IsInvokeResultDefinedImp
                  // Required for test cases 2,3,4,7
                  // ========================================

// Required for test cases 2,3,4,7
template <class VOID, class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp : bsl::false_type {
};

// Required for test cases 2,3,4,7
template <class INVOCABLE, class... ARGS>
struct IsInvokeResultDefinedImp<
    typename bslmf::VoidType<
        typename bsl::invoke_result<INVOCABLE, ARGS...>::type>::type,
    INVOCABLE,
    ARGS...> : bsl::true_type {
};

}  // close unnamed namespace

#endif


#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5

                        // ============================
                        // class SfinaeFriendlinessTest
                        // Required for test case 7
                        // ============================

namespace {

// Required for test cases 7
class SfinaeFriendlinessTest {
  public:
    // CREATORS
    SfinaeFriendlinessTest() {}
        // Simple user-provided constructor.

    // ACCESSORS
    template <bool IS_TYPE_DEFINED, class INVOCABLE, class... ARGS>
    void run(int LINE) const
        // Check whether IsInvokeResult is defined or not depending on
        // specified parameter 'LINE'
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

#endif


namespace {

// 15 distinct types
//
// Required for test cases 2,3,4
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

// Required for test cases 1,4,5
template <class TP>
struct MetaType {
    // An object of type 'MetaType<TP>' can represent 'TP' at run-time without
    // actually creating an instance of 'TP'.  Note that 'TP' can be a
    // reference and/or cv-qualified.

    typedef TP type;
};

// Required for test cases 2,3,4,5,6
enum MyEnum {
    // An enumeration type, for testing invocables that return enumerations.
    MY_ENUMERATOR
};


// Required for test cases 1,2,3,4,5,6,7
struct MyClass {
    // Simple class type that can be returned by value and to which member
    // variables and functions that can be referenced via pointer-to-member.

    char *d_str_p;
    int foo();
        // Declared but not defined.

    MyClass() : d_str_p(0) {}
    MyClass(MyClass&);
    MyClass(const MyClass&);
    MyClass(volatile MyClass&);
    MyClass(const volatile MyClass&);
        // Declared but not defined.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass(MyClass&&);
    MyClass(const MyClass&&);
    MyClass(volatile MyClass&&);
    MyClass(const volatile MyClass&&);
        // Declared but not defined.
#endif
};

// Required for test cases 2,3,4,5,7
struct MyDerivedClass : MyClass {
    // A class derived from MyClass, for testing pointer-to-base-member.
};

// Required for test cases 3,4,7
template <class TP>
struct SmartPtr {
    // This template has the interface of a smart pointer.  Specifically, the
    // dereference operator returns a reference to 'TP'.

    TP& operator*() const volatile;
    TP* operator->() const volatile;
        // Pointer-like operators (declared but not defined).
};


// Required for test cases 2,3,7
struct ToLongLong
{
    // Objects of this class are convertible to long long.

    operator long long() const;
        // Convertion to 'long long' (declared but not defined).
};


// Required for test cases 1,4,5
struct ManyFunc {
    // Generic functor class with many overloads of 'operator()'.

    bool operator()(float, void*);
    float operator()(float, void*) const;
        // Invocation operators for breathing test (declared but not defined)

    bool  operator()();
    short operator()(int);
    int   operator()(int, float);
    long  operator()(int, float, short, int, int, int, int, int, int, float);
    unsigned int   operator()(int,   int);
    unsigned short operator()(short, int);
        // Invocation overloads based on types and numbers of arguments
        // (declared but not defined)

    bool&  operator()(int, int, int);
    short& operator()(int, int, int) const;
    int&   operator()(int, int, int) volatile;
    long&  operator()(int, int, int) const volatile;
        // Invocation overloads based on 'const' and 'volatile' (declared but
        // not defined)

#if defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_IBM)
    // sun and xlC do not correctly strip top-level cv qualifiers from return
    // types.
    template <class TP>
    typename bsl::remove_cv<TP>::type operator()(MetaType<TP>, ...);
#else
    template <class TP>
    TP operator()(MetaType<TP>, ...);
#endif
    template <class TP>
    typename bsl::add_const<TP>::type operator()(MetaType<TP>, ...) const;
        // Return type deduced from first argument (declared but not defined)

    template <class TP>
    TP operator()(char*, MetaType<TP>);
        // Return type is 'TP'.  'invoke_result' is specialized for this case
        // (declared but not defined)

    typedef void *P;
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
        // Invocation overloads for 1 to 10 pointer arguments (declared but
        // not defined)
};

// Required for test cases 5,7
struct ConvertibleToFunc {
    // Class that is convertible to pointer-to-function or
    // reference-to-function, depending on the overload used.

    typedef bool    (*boolFPtr)();
    typedef short   (*shortFPtr)(int);
    typedef int*    (&intStarFRef)(int, float);
    typedef MyClass (&udtFRef)(int, float, short);

    operator boolFPtr() const;
        // Conversion to pointer to function returning bool.

    operator shortFPtr() const;
        // Conversion to pointer to function taking one argument.

    operator intStarFRef() const;
        // Conversion to reference to function returning pointer.

    operator udtFRef() const;
        // Conversion to reference to function returning pointer.
};

// Required for test cases 5,7
template <class RT>
struct FuncRt1 {
    // Functor type with two overloads of 'operator()': one that returns
    // 'result_type', which is an alias for 'RT', and another overload that
    // always returns 'int', regardless of 'result_type'.

    typedef RT result_type; // Used by 'invoke_result' to determine return type

    result_type operator()(const char*);
        // Used to test return type can be deduced via 'result_type' (declared
        // but not defined)

    int operator()(int);
        // Used to test that deduction of fundamental type 'int' supercedes
        // 'result_type' (declared but not defined)
};

// Required for test cases 5,7
template <class RT>
struct FuncRt2 {
    // Functor type with two overloads of 'operator()', one that returns
    // 'ResultType', which is an alias for 'RT', and another overload that
    // always returns 'int', regardless of 'ResultType'.

    typedef RT ResultType; // Used by 'invoke_result' to determine return type

    ResultType operator()(const char*);
        // Used to test return type can be deduced via 'ResultType' (declared
        // but not defined)

    int operator()(int);
        // Used to test that deduction of fundamental type 'int' supercedes
        // 'ResultType' (declared but not defined)
};

// Required for test cases 5,7
template <class RT>
struct FuncRt3 {
    // Functor type with two overloads of 'operator()', one that returns
    // 'result_type', which is an alias for 'RT', and another overload that
    // always returns 'int', regardless of 'result_type'.  This struct also
    // defines 'ResultType', which should be ignored in preference to
    // 'result_type'.

    typedef RT   result_type; // Used by 'invoke_result' to get return type
    typedef void ResultType;  // Ignored by 'invoke_result'.

    result_type operator()(const char*) const;
        // Used to test return type can be deduced via 'result_type' (declared
        // but not defined)

    int operator()(int) const;
        // Used to test that deduction of fundamental type 'int' supercedes
        // 'result_type' (declared but not defined)
};



// Required for test cases 2,3,4,5
template <class TEST_KERNEL, class RT>
struct ApplyRef {
    // Used by 'applyPtrAndRef' to apply the specified 'TEST_KERNEL' template
    // parameter to each of 'RT', 'RT&', 'const RT&', 'volatile RT&',
    // 'const volatile RT&', and 'RT&&'.  Requires: 'TEST_KERNEL' is a class
    // type with an 'apply<TP>()' static member that tests 'invoke_result' on
    // invocables that return 'TP'.

    static void apply(int LINE)
        // Call, using the specified 'LINE', 'TEST_KERNEL::apply<RT>(LINE)' and
        // 'TEST_KERNEL::apply<cv RT&>(LINE)', where 'cv' is all four
        // combinations of 'const' and 'volatile'.  On compilers that support
        // rvalue references, also call 'TEST_KERNEL::apply<RT&&>(LINE)'.
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
                 // 'bslmf_invokeresult' to make use of 'decltype'.  This means
                 // that 'bslmf_invokeresult' cannot support
                 // rvalue-reference-qualified types on MSVC 2013.
        TEST_KERNEL::template apply<RT&&>(LINE);
        TEST_KERNEL::template apply<const RT&&>(LINE);
        TEST_KERNEL::template apply<volatile RT&&>(LINE);
        TEST_KERNEL::template apply<const volatile RT&&>(LINE);
#  endif
# else
#  error "Rvalue refs without 'decltype' not supported by 'bsl::invoke_result'"
# endif
#endif
    }
};

// Required for test cases 2,3,4,5
template <class TEST_KERNEL>
struct ApplyRef<TEST_KERNEL, void> {
    // For the specified 'TEST_KERNEL' template parameter, apply 'TEST_KERNEL'
    // such that it tests invocations that should return 'void'.  This
    // specialization avoids trying to create a reference-to-void.

    static void apply(int LINE)
        // Call, using the specified 'LINE', 'TEST_KERNEL::apply<void>(LINE)'.
    {
        TEST_KERNEL::template apply<void>(LINE);
    }
};

// Required for test cases 2,3,4,5
template <class TEST_KERNEL, class RT>
static void applyPtrAndRef(int LINE)
    // Call, using the specified 'LINE', 'TEST_KERNEL::apply<X>(LINE)', where
    // 'X' is each of the following: the specified 'RT' template parameter, a
    // pointer to (cv-qualified) 'RT', a reference to cv-qualified of 'RT', and
    // a reference to a cv-qualified pointer to cv-qualified 'RT' (for every
    // combination of cv qualifications).  Requires: 'TEST_KERNEL' is a class
    // type with an 'apply<TP>()' static member that tests 'invoke_result' on
    // invocables that return 'TP'.
{
    ApplyRef<TEST_KERNEL, RT>::apply(LINE);
    ApplyRef<TEST_KERNEL, RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, const RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, volatile RT*>::apply(LINE);
    ApplyRef<TEST_KERNEL, const volatile RT*>::apply(LINE);
}

}  // close unnamed namespace


                        // ===========================================
                        // Macros to enable template pre-instantiation
                        // Required for test cases 2,3,4,5
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


#ifndef BSLMF_INVOKERESULT_00T_AS_INCLUDE

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main()
{
    bool verbose = false;

    int  test = -1;
    int  testStatus = -1;

    // This switch statement does nothing, it is here to satisfy bde_verify.
    switch (test) { case 0:
      case -1: {
        // --------------------------------------------------------------------
        // DUMMY TEST CASE
        //
        // Concerns:
        //: 1 Dummy entry to satisfy bde_verify.
        //
        // Plan:
        //: 1 Dummy entry to satisfy bde_verify.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "DUMMY TEST CASE" "\n"
                                 "===============" "\n");
      }  break;
      default: {
        testStatus = -1;
      }  break;
    };

    // Remove the "defined but not used" warning:
    ASSERT(true);

    return testStatus;
}

#endif // not defined 'BSLMF_INVOKERESULT_00T_AS_INCLUDE'

#endif // End C++11 code

#endif // include guard BSLMF_INVOKERESULT_00T_INCLUDED

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
