// bslmf_invokeresult.t.cpp                                           -*-C++-*-

#include "bslmf_invokeresult.h"

#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>
#include <bslmf_removepointer.h>

#include <bsla_maybeunused.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <utility>   // native_std::move for C++11

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

#if defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
// The MSVC 18.0 and 19.0 compilers have a bug whereby an expression like,
// 'func().*ptrtomem', where 'func()' returns 'volatile SomeClass&&' and
// 'ptrtomem' is a declared as 'RET SomeClass::*ptrtomem', yields an lvalue
// instead of an rvalue reference. Defining this macro will disable certain
// tests that would fail using the MSVC compiler.
#   define MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component is a transformation metafunction. Testing consists primarily
// of instantiating 'bsl::invoke_result<FN, Args...>' with representative
// combinations of 'Fn' and 'Args' and verifying that the resulting 'type' is
// what was expected.  The test is broken up as one test case per category of
// invocable 'Fn' parameter.
//-----------------------------------------------------------------------------
// [2] FUNCTION INVOCABLES
// [3] POINTER-TO-MEMBER-FUNCTION INVOCABLES
// [4] POINTER-TO-MEMBER-OBJECT INVOCABLES
// [5] FUNCTOR CLASS INVOCABLES
// [6] InvokeResultDeductionFailed
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [7] USAGE EXAMPLE
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM) || defined(BSLS_PLATFORM_CMP_SUN)
# define BSLMF_INVOKERESULT_ABRIDGED_TEST 1
    // Use Abridged set of tests for xlC 12 and Sun CC 5.12. These compilers
    // get overwhelmed with too many template instantiations. They will work
    // fine for a real program, but fail for test programs like this that
    // recursively create hundreds of instantiations. The abridged tests lose
    // a bit of redundancy, but are probably adaquate for testing the
    // component, especially when combined with the full tests on other
    // platforms.
#else
# define BSLMF_INVOKERESULT_ABRIDGED_TEST 0
#endif

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

namespace bsl {
template <class TP> class reference_wrapper;
    // Forward declaration of 'bsl::reference_wrapper' (no implementation)
}  // close namespace bsl

template <class T1, class T2>
struct AssertSame;
    // For debugging purposes, instantiating this template with two types that
    // are not the same will result in a compile-time error that, on most
    // compilers, will print 'T1' and 'T2' as well as a trace of the
    // instantiation stack.

template <class T1>
struct AssertSame<T1, T1> : bsl::true_type
{
    // Specialization of 'AssertSame' when instantiated with two parameters of
    // the same type yields a successful compilation.
};


// 15 distinct types
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

template <class TP>
struct MetaType {
    // An object of type 'MetaType<TP>' can represent 'TP' at run-time without
    // actually creating an instance of 'TP'.  Note that 'TP' can be a
    // reference and/or cv-qualified.

    typedef TP type;
};

enum MyEnum {
    // An enumeration type, for testing invocables that return enumerations.
    MY_ENUMERATOR
};

struct MyClass {
    // Simple class type that can be returned by value and to which member
    // variables and functions that can be referenced via pointer-to-member.

    char *d_str_p;
    int foo();
        // Declared but not defined.

    MyClass() { }
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

struct MyDerivedClass : MyClass {
    // A class derived from MyClass, for testing pointer-to-base-member.
};

template <class TP>
struct SmartPtr {
    // This template has the interface of a smart pointer.  Specifically, the
    // dereference operator returns a reference to 'TP'.

    TP& operator*() const volatile;
    TP* operator->() const volatile;
        // Pointer-like operators (declared but not defined).
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
struct MovePtr {
    // This template has the interface of a smart pointer except that, the
    // dereference operator returns an RVALUE reference to 'TP' instead of an
    // LVALUE reference.

    TP&& operator*() const volatile;
        // Dereference operator (declared but not defined).
};
#endif

template <class TP>
struct ProxyPtr {
    // This template has the interface of a "proxy" pointer, where the
    // dereference operator returns an rvalue of type 'TP'.

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    typedef typename bsl::remove_const<TP>::type value_type;
        // Type being proxied. Detection of const rvalues is problematic on
        // older Sun/Oracle compiler (they get confused with lvalues in our
        // metaprograms).
#else
    typedef TP value_type;
        // Type being proxied.
#endif

    value_type operator*() const volatile;
        // Dereference operator (declared but not defined)
};

struct ToLongLong
{
    // Objects of this class are convertible to long long.

    operator long long() const;
        // Convertion to 'long long' (declared but not defined).
};

struct ToIntPtr
{
    // Objects of this class are convertible to pointer-to-int.

    operator int*() const;
        // Convertion to 'int*' (declared but not defined).
};

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

#if defined(BSLS_PLATFORM_CMP_IBM)
    // xlC 12 does not correctly strip top-level cv qualifiers from return
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
       // Conversion to pointer to function taking one arg.

    operator intStarFRef() const;
       // Conversion to reference to function returning pointer.

    operator udtFRef() const;
       // Conversion to reference to function returning pointer.
};

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

namespace bsl {

template <class TP>
struct invoke_result<ManyFunc, char*, MetaType<TP> > {
    // Specialization of 'invoke_result' for a situation where, in C++03 mode,
    // return-type deduction would otherwise fail.

    typedef TP type;
        // Correct return type for 'ManyFunc::operator()(char*, MetaType<TP>)'
};

}  // close namespace bsl

template <class TEST_KERNEL, class RT>
struct ApplyRef {
    // Used by 'applyPtrAndRef' to apply the specified 'TEST_KERNEL' template
    // parameter to each of 'RT', 'RT&', 'const RT&', 'volatile RT&',
    // 'const volatile RT&', and 'RT&&'.  Requires: 'TEST_KERNEL' is a class
    // type with an 'apply<TP>()' static member that tests 'invoke_result' on
    // invocables that return 'TP'.

    static void apply()
        // Call 'TEST_KERNEL::apply<RT>()' and 'TEST_KERNEL::apply<cv RT&>()',
        // where 'cv' is all four combinations of 'const' and 'volatile'.  On
        // compilers that support rvalue references, also call
        // 'TEST_KERNEL::apply<RT&&>()'.
    {
        TEST_KERNEL::template apply<RT>();
        TEST_KERNEL::template apply<RT&>();
        TEST_KERNEL::template apply<const RT&>();
        TEST_KERNEL::template apply<volatile RT&>();
        TEST_KERNEL::template apply<const volatile RT&>();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#   if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        TEST_KERNEL::template apply<RT&&>();
        TEST_KERNEL::template apply<const RT&&>();
        TEST_KERNEL::template apply<volatile RT&&>();
        TEST_KERNEL::template apply<const volatile RT&&>();
#   else
#       error Rvalue refs without 'decltype' not supported by 'invoke_result'.
#   endif
#endif
    }
};

template <class TEST_KERNEL>
struct ApplyRef<TEST_KERNEL, void> {
    // For the specified 'TEST_KERNEL' template parameter, apply 'TEST_KERNEL'
    // such that it tests invocations that should return 'void'.  This
    // specialization avoids trying to create a reference-to-void.

    static void apply()
        // Call 'TEST_KERNEL::apply<void>()'
    {
        TEST_KERNEL::template apply<void>();
    }
};

template <class TEST_KERNEL, class RT>
void applyPtrAndRef()
    // Call 'TEST_KERNEL::apply<X>()', where 'X' is each of the following: the
    // specified 'RT' template parameter, a pointer to (cv-qualified) 'RT', a
    // reference to cv-qualified of 'RT', and a reference to a cv-qualified
    // pointer to cv-qualified 'RT' (for every combination of cv
    // qualifications).  Requires: 'TEST_KERNEL' is a class type with an
    // 'apply<TP>()' static member that tests 'invoke_result' on invocables
    // that return 'TP'.
{
    ApplyRef<TEST_KERNEL, RT>::apply();
    ApplyRef<TEST_KERNEL, RT*>::apply();
    ApplyRef<TEST_KERNEL, const RT*>::apply();
    ApplyRef<TEST_KERNEL, volatile RT*>::apply();
    ApplyRef<TEST_KERNEL, const volatile RT*>::apply();
}

struct FuncTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, Args...>', where 'Fn'
    // is a function type, pointer-to-function type, reference to function
    // type, or reference to cv-qualified pointer to function type.

    template <class RT>
    static void apply()
        // Instantiate 'invoke_result' for function type 'RT(int)' and pointers
        // and references to that function type.  Verify that it yields 'RT' as
        // the result type.
    {
        using bsl::is_same;
        using bsl::invoke_result;

        typedef RT Fn(int);
        ASSERT((is_same<RT, typename invoke_result<Fn,   char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fn&,  char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fn*,  char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fn*&, char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fn *const&,
                                                   char>::type>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT((is_same<RT, typename invoke_result<Fn&&, char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fn*&&,char>::type>::value));
#endif
    }
};

template <class T1 = MyClass>
struct PtrToMemFuncTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, Args...>', where 'Fn'
    // is a pointer-to-member-function type, or reference to cv-qualified
    // pointer-to-member-function type.

    template <class RT>
    static void apply()
        // Instantiate 'invoke_result' for function type
        // 'RT (MyClass::*Fp)(int);' and references to that function type.
        // Verify that it yields 'RT' as the result type.
    {
        using bsl::is_same;
        using bsl::invoke_result;

        typedef RT (MyClass::*Fp)(int);
        ASSERT((is_same<RT, typename invoke_result<Fp, T1,
                                                   char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fp&, T1,
                                                   char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fp const&, T1,
                                                   char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fp volatile&, T1,
                                                   char>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<Fp const volatile&, T1,
                                                   char>::type>::value));
        typedef RT (MyClass::*CFp)(int) const;
        typedef RT (MyClass::*VFp)(int) volatile;
        typedef RT (MyClass::*CVFp)(int) const volatile;
        ASSERT((is_same<RT, typename invoke_result<CFp, T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CFp, const T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<VFp, T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<VFp, volatile T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CVFp, T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CVFp, volatile T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CVFp, const T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CVFp, const volatile T1,
                                                   short>::type>::value));


#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT((is_same<RT, typename invoke_result<Fp&&, T1,
                                                   char>::type>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // Test ref-qualified member-function-pointers
        typedef RT (MyClass::*LFp)(int) &;
        typedef RT (MyClass::*CLFp)(int) const &;
        ASSERT((is_same<RT, typename invoke_result<LFp, T1&,
                                                   int>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CLFp, T1&,
                                                   int>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<CLFp, const T1&,
                                                   int>::type>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef RT (MyClass::*RFp)(int) &&;
        typedef RT (MyClass::*VRFp)(int) volatile &&;
        ASSERT((is_same<RT, typename invoke_result<RFp, T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<RFp, T1&&,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<VRFp, T1,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<VRFp, T1 &&,
                                                   short>::type>::value));
        ASSERT((is_same<RT, typename invoke_result<VRFp, volatile T1&&,
                                                   short>::type>::value));
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    }
};

template <class T1, class T2>
struct PropagateCVQ {
    // Apply the cv-qualifications from the specified template parameter type
    // 'T1' to the specified template parameter 'T2', producing a typedef
    // 'type'.  More specifically: 'type' is 'cvq T2&', where 'cvq' is the
    // cv-qualifiers (if any) on 'T1'.  Note that any cv-qualifiers already on
    // 'T2' are unioned with 'cvq' to produce the result.

    typedef T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<const T1, T2> {
    // Specialization of 'PropagateCVQ' for 'const T1'.

    typedef const T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<volatile T1, T2> {
    // Specialization of 'PropagateCVQ' for 'volatile T1'.

    typedef volatile T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<const volatile T1, T2> {
    // Specialization of 'PropagateCVQ' for 'const volatile T1'.

    typedef const volatile T2 type;
};

template <class T1, class T2>
struct PropagateCVQRef {
    // Apply the reference and cv-qualifications from the specified template
    // parameter type 'T1' to the specified template parameter 'T2', producing
    // a typedef 'type'.  More specifically: if 'T1' is an rvalue, 'type' is
    // simply 'T2' (unmodified); otherwise, 'type' is 'cvq T2&', where 'cvq'
    // is the cv-qualifiers (if any) on 'T1'.  Note that any cv-qualifiers
    // already on 'T2' are unioned with 'cvq' to produce the result.  This
    // primary template is instantiated when 'T1' is not a reference.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    typedef T2 type;
        // Result type.  In C++03, if 'T1' is an rvalue, then 'T2' is returned
        // unchanged.  The cv-qualifications on 'T1' are ignored.
#else
    typedef typename PropagateCVQ<T1, T2>::type&& type;
        // Result type.  In C++11 and later, if 'T1' is a prvalue, then 'T2'
        // is returned as an rvalue reference, with the cv-qualifications
        // copied from 'T1'.
#endif
};

template <class T1, class T2>
struct PropagateCVQRef<T1&, T2> {
    // Specialization of 'PropagateCVQRef' for lvalue reference 'T1'.

    typedef typename PropagateCVQ<T1, T2>::type& type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class T1, class T2>
struct PropagateCVQRef<T1&&, T2> {
    // Specialization of 'PropagateCVQRef' for rvalue reference 'T1'.

    typedef typename PropagateCVQ<T1, T2>::type&& type;
};
#endif

template <class T1 = MyClass, class EFFECTIVE_ARG = T1>
struct PtrToMemObjTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, T1>', where 'Fn' is a
    // pointer-to-member-object type, or reference to cv-qualified
    // pointer-to-member-object type.

    template <class RT>
    static void doApply(bsl::true_type /* void or reference */)
        // No-op implementation of 'apply' for when 'RT' is 'void' or a
        // reference type.  There is no such thing as a member pointer to void
        // or reference.
    {
    }

    template <class RT>
    static void doApply(bsl::false_type /* void or reference */)
        // For 'Fn' of type 'RT MyClass::*Mp', instantiate
        // 'invoke_result<Fn, T1>' and 'invoke_result<Fn cvq&, T1>'.  Verify
        // that it yields 'RT' as the result type.  This overload is called
        // when 'RT' is not 'void' and is not a reference.
    {
        using bsl::is_same;
        using bsl::invoke_result;

        typedef RT MyClass::*Mp;
        typedef typename PropagateCVQRef<EFFECTIVE_ARG, RT>::type CvqR;

        AssertSame<CvqR, typename invoke_result<Mp , T1>::type>();

        ASSERT((is_same<CvqR, typename invoke_result<Mp , T1>::type>::value));
        ASSERT((is_same<CvqR, typename invoke_result<Mp&, T1>::type>::value));
        ASSERT((is_same<CvqR, typename invoke_result<Mp const&,
                                                     T1>::type>::value));
        ASSERT((is_same<CvqR, typename invoke_result<Mp volatile&,
                                                     T1>::type>::value));
        ASSERT((is_same<CvqR, typename invoke_result<Mp const volatile&,
                                                     T1>::type>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT((is_same<CvqR, typename invoke_result<Mp&&, T1>::type>::value));
#endif
    }

    template <class RT>
    static void apply()
        // For 'Fn' of type 'RT MyClass::*Mp', instantiate 'invoke_result<Fn>'
        // and 'invoke_result<Fn cvq&>'.  Verify that it yields 'RT' as the
        // result type.
    {
        // If 'RT' is 'void' or a reference type, 'doApply' is a no-op.  There
        // is no such thing as a member pointer to void or reference.
        enum { kIS_VOID_OR_REF = (bsl::is_void<RT>::value ||
                                  bsl::is_reference<RT>::value) };

        doApply<RT>(bsl::integral_constant<bool, kIS_VOID_OR_REF>());
    }
};

template <bool RET_USER_TYPE = false>
struct FunctorTest
{
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn>', where 'Fn' is a class
    // (functor) type, or reference to cv-qualified functor type.  If the
    // specified 'RET_USER_TYPE' template parameter is true, it indicates that
    // the caller intends to call 'apply<RT>' with user-defined (class or
    // enumeration) type for 'RT'.  Otherwise, the caller must supply a basic
    // (numeric, pointer, or void) type for 'RT'.

    template <class RT>
    static void apply()
        // Instantiate 'invoke_result<Fn, Args...>' where 'Fn' is a variety of
        // functor types, and verify that it yields a 'type' of the specified
        // template parameter type 'RT'.  This test checks the return type for
        // the following functor types:
        //
        //: o 'FuncRt1', which uses the 'result_type' idiom
        //: o 'FuncRt2', which uses the 'ResultType' idiom
        //: o 'FuncRt3', which uses both 'result_type' and 'ResultType'
        //: o 'ManyFunc', which uses neither 'result_type' and 'ResultType'
        //
        // If 'RET_USER_TYPE' is true and the compiler does not support
        // 'decltype', then the expected result type for 'ManyFunc' cannot be
        // deduced and 'invoke_result' is expected to yield
        // 'bslmf::InvokeResultDeductionFailed'.  As a special case, for
        // 'FuncRt1', 'FuncRt2', and 'FuncRt3', test that using 'invoke_result'
        // arguments that choose an overload that returns 'int' will correctly
        // yield 'int' regardless of the 'result_type' and 'ResultType'
        // idioms.
    {
        using bsl::invoke_result;
        using bslmf::InvokeResultDeductionFailed;

        // The 'char*' overload of 'FuncRt1<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt1<RT> , char*>::type Rt1ResultR;
        typedef typename invoke_result<FuncRt1<RT>&, char*>::type Rt1rResultR;
        typedef typename invoke_result<FuncRt1<RT> , int  >::type Rt1ResultInt;
        typedef typename invoke_result<FuncRt1<RT>&, int  >::type
            Rt1rResultInt;
        ASSERT((bsl::is_same<RT , Rt1ResultR>::value));
        ASSERT((bsl::is_same<RT , Rt1rResultR>::value));
        ASSERT((bsl::is_same<int, Rt1ResultInt>::value));
        ASSERT((bsl::is_same<int, Rt1rResultInt>::value));

        // The 'char*' overload of 'FuncRt2<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt2<RT> , char*>::type Rt2ResultR;
        typedef typename invoke_result<FuncRt2<RT>&, char*>::type Rt2rResultR;
        typedef typename invoke_result<FuncRt2<RT> , int  >::type Rt2ResultInt;
        typedef typename invoke_result<FuncRt2<RT>&, int  >::type
            Rt2rResultInt;
        ASSERT((bsl::is_same<RT,   Rt2ResultR>::value));
        ASSERT((bsl::is_same<RT,   Rt2rResultR>::value));
        ASSERT((bsl::is_same<int, Rt2ResultInt>::value));
        ASSERT((bsl::is_same<int, Rt2rResultInt>::value));

        // The 'char*' overload of 'FuncRt3<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt3<RT> , char*>::type Rt3ResultR;
        typedef typename invoke_result<FuncRt3<RT>&, char*>::type Rt3rResultR;
        typedef typename invoke_result<FuncRt3<RT> , int  >::type Rt3ResultInt;
        typedef typename invoke_result<FuncRt3<RT>&, int  >::type
            Rt3rResultInt;
        ASSERT((bsl::is_same<RT , Rt3ResultR>::value));
        ASSERT((bsl::is_same<RT , Rt3rResultR>::value));
        ASSERT((bsl::is_same<int, Rt3ResultInt>::value));
        ASSERT((bsl::is_same<int, Rt3rResultInt>::value));

        typedef typename invoke_result<ManyFunc,  MetaType<RT>, int>::type
            MFResult;
        typedef typename invoke_result<ManyFunc&, MetaType<RT>, int>::type
            MFrResult;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        // 'decltype' is supported, the result type will always be correct.
        ASSERT((bsl::is_same<RT, MFResult>::value));
        ASSERT((bsl::is_same<RT, MFrResult>::value));
#else
        // 'decltype' is not supported, the result type will always be correct
        // for basic types like 'int', 'int*' and 'void', but will be a
        // place-holder for user-defined types that don't support the
        // 'result_type' or 'ResultType' idioms.
        if (RET_USER_TYPE) {
            // User-defined type, expect 'InvokeResultDeductionFailed' place
            // holder.
            ASSERT((bsl::is_same<InvokeResultDeductionFailed,
                                 MFResult>::value));
            ASSERT((bsl::is_same<InvokeResultDeductionFailed,
                                 MFrResult>::value));
        }
        else {
            // Basic type, expect correct result.
            ASSERT((bsl::is_same<RT, MFResult>::value));
            ASSERT((bsl::is_same<RT, MFrResult>::value));
        }
#endif // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    }
};

template <class TP>
TP returnObj()
    // Return a value-initialized object of type 'TP'.
{
    return TP();
}

template <class TP>
TP& returnNoCVRef()
    // Return a reference to a value-initialized object of type 'TP'. 'TP' is
    // assumed not to be cv-qualified.
{
    static TP obj;
    return obj;
}

template <class TP>
TP& returnLvalueRef()
    // Return an lvalue reference to a value-initialized object of type
    // 'TP'.  'TP' may be cv-qualified.
{
    return returnNoCVRef<typename bsl::remove_cv<TP>::type>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
TP&& returnRvalueRef()
    // Return an rvalue reference to a value-initialized object of type 'TP'.
{
    return std::move(returnLvalueRef<TP>());
}
#endif

template <class TP>
bslmf::InvokeResultDeductionFailed discardObj()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an rvalue of type 'TP'.
{
    return returnObj<TP>();
}

template <class TP>
bslmf::InvokeResultDeductionFailed discardLvalueRef()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an lvalue reference of type 'TP&'.  'TP' may be cv-qualified.
{
    return returnLvalueRef<TP>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
bslmf::InvokeResultDeductionFailed discardRvalueRef()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an rvalue reference of type 'TP&&'.  'TP' may be cv-qualified.
{
    return returnRvalueRef<TP>();
}
#endif

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Suppose we want to create a wrapper that executes an invocable object and
// sets a 'done' flag.  The 'done' flag will not be set if the invocation
// exits via an exception.  The wrapper takes an invocable 'f' and an argument
// 'x' and evaluates 'f(x)', returning the result.  In the absence of C++14
// automatically-deduced function return declarations, we use
// 'bsl::invoke_result' to deduce the return type of 'f(x)'.
//
// First, we write the wrapper template as follows:
//..
    template <class FT, class XT>
    typename bsl::invoke_result<FT, XT>::type
    invokeAndSetFlag(bool *done, FT f, XT x)
        // Return 'f(x)' and set '*done' to true if no exception.
    {
        typedef typename bsl::invoke_result<FT, XT>::type ResultType;
        *done = false; // Clear flag in case of exception
        ResultType result = f(x);
        *done = true;  // Set flag on success
        return result;
    }
//..
// Note that additional metaprogramming would be required to make this
// template work for return type 'void'; such metaprogramming is beyond the
// scope of this usage example.
//
// Then we define a couple of simple functors to be used with the wrapper.
// The first functor is a simple template that triples its invocation
// argument:
//..
    template <class TP>
    struct Triple {
        // Functor that triples its argument.

        TP operator()(TP v) const { return static_cast<TP>(v * 3); }
            // Return three times the specified 'v' value.
    };
//..
// Next, we define a second functor that returns an enumerator 'ODD' or
// 'EVEN', depending on whether its argument is exactly divisible by 2.  Since
// the return type is not a fundamental type, this functor indicates its
// return type using the 'ResultType' idiom:
//..
    enum EvenOdd { e_EVEN, e_ODD };

    struct CalcEvenOdd {
        // Functor that determines whether its argument is odd or even.

        typedef EvenOdd ResultType;

        EvenOdd operator()(int i) const { return (i & 1) ? e_ODD : e_EVEN; }
            // Return 'e_ODD' if the specified 'i' is odd; otherwise return
            // 'e_EVEN'
    };
//..
// Finally, we can invoke these functors through our wrapper:
//..
    int usageExample()
        // Run the usage example.
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    using BloombergLP::bslmf::InvokeResultDeductionFailed;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Concerns:
        //: 1 The usage example in the component documentation compiles and
        //:   runs.
        //
        // Plan:
        //: 1  Copy the usage example verbatim but replace 'assert' with
        //:   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE TEST"
                            "\n==========\n");

        usageExample();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'InvokeResultDeductionFailed'
        //
        // Concerns:
        //: 1 'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   object type.
        //: 2 'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   lvalue reference.
        //: 3 The previous concerns apply if the initializer is cv qualified.
        //: 4 In C++11 and later compilations,
        //:   'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   rvalue reference.
        //: 5 The above concerns apply to values that are the result of a
        //:   function return.
        //: 6 The 'return' statement of a function returning a
        //:   'bslmf::InvokeResultDeductionFailed' can specify an lvalue or
        //:   rvalue of any type, resulting in the value being ignored.
        //
        // Plan:
        //: 1 For concern 1, construct 'bslmf::InvokeResultDeductionFailed'
        //:   objects from value-initialized objects of numeric type,
        //:   pointer type, class type, and enumeration type. There is nothing
        //:   to verify -- simply compiling successfully is enough.
        //: 2 For concern 2, create variables of the same types as in the
        //:   previous step. Construct a 'bslmf::InvokeResultDeductionFailed'
        //:   object from each (lvalue) variable.
        //: 3 For concern 3, repeat step 2, using a 'const_cast' to add cv
        //:   qualifiers to the lvalues.
        //: 4 For concern 4, repeat step 2, applying 'std::move' to each
        //:   variable used to construct an object (C++11 and later only).
        //: 5 For concern 5, implement a function 'returnObj<TP>' that returns
        //:   an object of type 'TP', a function 'returnLvalueRef<TP>', that
        //:   returns a reference of type 'TP&' and (for C++11 and later)
        //:   'returnRvalueRef<TP>' that returns a reference of type
        //:   'TP&&'. Construct a 'bslmf::InvokeResultDeductionFailed' object
        //:   from a call to each function template instantiated with each of
        //:   the types from step 1 and various cv-qualifier combinations.
        //: 6 For concern 6, implement function templates returning
        //:   'bslmf::InvokeResultDeductionFailed' objects 'discardObj<TP>',
        //:   'discardLvalueRef<TP>', and 'discardRvalueRef<TP>'. These
        //:   functions respectively contain the statements 'return
        //:   returnObj<TP>', 'return returnLvalueRef<TP>', and 'return
        //:   returnRvalueRef<TP>'. Invoke each function with the same types
        //:   as in step 5.
        //
        // Testing
        //     'InvokeResultDeductionFailed'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'InvokeResultDeductionFailed'"
                            "\n=====================================\n");

        int            v1 = 0;
        bsl::nullptr_t v2;
        const char*    v3 = "hello";
        MyEnum         v4 = MY_ENUMERATOR;
        MyClass        v5;

        // Step 1: Conversion from rvalue
        {
            bslmf::InvokeResultDeductionFailed x1(0);
            bslmf::InvokeResultDeductionFailed x2 = bsl::nullptr_t();
            bslmf::InvokeResultDeductionFailed x3("hello");
            bslmf::InvokeResultDeductionFailed x4(MY_ENUMERATOR); // MyEnum
            bslmf::InvokeResultDeductionFailed x5 = MyClass();

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        // Step 2: Conversion from lvalue
        {
            bslmf::InvokeResultDeductionFailed x1 = v1;
            bslmf::InvokeResultDeductionFailed x2 = v2;
            bslmf::InvokeResultDeductionFailed x3 = v3;
            bslmf::InvokeResultDeductionFailed x4 = v4;
            bslmf::InvokeResultDeductionFailed x5 = v5;

            (void) x1; // Suppress "set but not used" warning
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
#endif

        // Step 5: Initialization from function return
        {
            bslmf::InvokeResultDeductionFailed x1 = returnObj<int>();
            bslmf::InvokeResultDeductionFailed x2 =
                returnObj<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 = returnObj<const char *>();
            bslmf::InvokeResultDeductionFailed x4 = returnObj<MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 = returnObj<MyClass>();

            (void) x1; // Suppress "set but not used" warning
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
#endif
        // Step 6: Return 'bslmf::InvokeResultDeductionFailed'
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
#endif
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR CLASS INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of functor class type, 'bsl::invoke_result' can
        //:   deduce the return type of 'operator()' if that return type is a
        //:   simple built-in integral type.
        //: 2 The correct overload of 'operator()' is selected, depending on
        //:   the number and types of arguments.
        //: 3 Appropriate overloads of 'operator()' are selected depending on
        //:   the const and/or volatile qualifiers on the functor type.
        //: 4 The return-type deduction described in concerns 1-3 additionally
        //:   extends to return types of:
        //:    o 'void'
        //:    o 'bsl::nullptr_t'
        //:    o built-in numeric types
        //:    o pointer to (possibly cv-qualified) numeric type or 'void'
        //:    o Reference to any of the (possibly cv-qualified) types above
        //:      except 'void', including reference-to-pointer types. Rvalue
        //:      references should be tested on C++11-compliant compilers.
        //: 5 In C++03, if the invocable returns a user-defined type, pointer
        //:   to a user-defined type, or reference to one of these, and the
        //:   invocable has a nested type 'result_type' or 'ResultType',
        //:   'invoke_result' yields that nested type; otherwise it yields
        //:   'bslmf::InvokeResultDeductionFailed'.  If the invocable has both
        //:   'result_type' and 'ResultType' nested types, then 'invoke_result'
        //:   yields 'result_type'.  However, if the invocation would return a
        //:   type listed in concern 4, 'result_type' and 'ResultType' are
        //:   ignored.
        //: 6 In C++11 or later, if the invocable returns a user-defined type,
        //:   pointer to a user-defined type, or reference to one of these,
        //:   'invoke_result' always yields the correct return type, regardless
        //:    of whether or not there exists a nested 'result_type' or
        //:   'ResultType' type.
        //: 7 The previous concerns apply when the invocable is a *reference*
        //:   to functor class type.
        //: 8 If an explicit specialization exists for
        //:   'invoke_result<TP, args...>', then the result type of
        //:   'TP(args...)'  (for a specific set of arguments) is returned,
        //:   regardless of C++11 vs. earlier modes, even if deduction of the
        //:   return value would have failed under normal circumstances.
        //: 9 The above concerns apply to functors taking 0 to 10 arguments.
        //: 10 Though not technically a functor, a class that is convertible
        //:   to a pointer-to-function or reference-to-function behaves like a
        //:   functor within this component.  If the class is convertible to
        //:   more than one function type, then the correct one is chosen
        //:   based on overloading rules.  If the chosen (pointer or reference
        //:   to) function returns a user-defined type, then
        //:   'bslmf::InvokeResultDeductionFailed' in C++03 mode.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor class, 'ManyFunc', with
        //:   'operator()' overloaded to return different integral types for a
        //:   variety of argument combinations.  Verify that
        //:   'bsl::invoke_result' yields the return type corresponding to the
        //:   best overload for a set of specific arguments types.
        //: 2 For concern 3, add 'const' and 'volatile' overloads. Using
        //:   typedefs for 'const' and 'volitile' references to 'ManyFunc',
        //:   verify that the best overload is selected.
        //: 3 For concern 4 add overloads of 'operator()' to 'ManyFunc' that
        //:   return 'TP' when called with a first argument of type
        //:   'MetaType<TP>'. Define a struct 'FunctorTest' whose 'apply<R>'
        //:   method verifies that 'invoke_result' yields the expected result
        //:   of invoking a 'ManyFunc' object with an argument of type
        //:   'MetaType<R>'.  Invoke 'FunctorTest' through the function
        //:   'applyPtrAndRef' that adds every combination of pointer,
        //:   reference, and cv qualifiers to 'R'. Repeat this test with every
        //:   numeric type and with 'void' for 'R'.
        //: 4 For concerns 5 and 6, define three invocables: 'FuncRt1<R>' that
        //:   defines 'result_type' as 'R', 'FuncRt2<R>' that defines
        //:   'ResultType' as 'R', and 'FuncRt3<R>' that defines 'result_type'
        //:   as 'R' and 'ResultType' as 'void'.  Each of them would define 'R
        //:   operator()(const char*)' and 'int operator()(int)'. Modify
        //:   'FunctorTest' such that the method 'FunctorTest<true>::apply<R>'
        //:   verifies that, if 'F' is 'FuncRt?<R>' and 'R' is one of the
        //:   types listed in concern 5, 'invoke_result<F, char*>::type' is
        //:   'R'; 'invoke_result<F, int>::type' is 'int', regardless of 'R';
        //:   and 'invoke_result<ManyFunc, MetaType<R>>::type' is
        //:   'bslmf::InvokeResultDeductionFailed' in C++03 mode and 'R' in
        //:   C++11 mode.  Invoke 'FunctorTest<true>' through the function
        //:   'applyPtrAndRef' as in step 3, to generate every 'R' with every
        //:   combination of pointer and reference qualifiers to user-defined
        //:   class 'MyClass' and user-defined enumeration 'MyEnum'.
        //: 5 For concern 7, repeat each step using a reference as the first
        //:   argument. For many of the tests, the modification would be in
        //:   one or more 'apply<R>' functions.
        //: 6 For concern 8, add an easily-distinguished set of overloads to
        //:   'ManyFunc::operator()' that return 'MyClass', 'MyClass *', etc..
        //:   Create explicit specializations of 'bsl::invoke_result' for those
        //:   overloads.  Verify that the explicit specializations yield the
        //:   expected type.
        //: 7 For concern 9, instantiate 'bsl::invoke_result' on overloads of
        //:   'ManyFunc::operator()' taking 0 to 10 arguments. Concern 8 does
        //:   not interract with the others, so it is not necessary to
        //:   test every combination of 0 to 10 arguments with every possible
        //:   return type.
        //: 8 For concern 10, define a type, 'ConvertibleToFunc' which is
        //:   convertible to pointer-to-function and reference-to-function,
        //:   where the result of each conversion has a different prototype.
        //:   Instantiate 'bsl::invoke_result' with arguments that select
        //:   each of the overloads and verify the correct result.
        //
        // Testing:
        //     FUNCTOR CLASS INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTOR CLASS INVOCABLES"
                            "\n================================\n");

        typedef ManyFunc const           ManyFuncC;
        typedef ManyFunc volatile        ManyFuncV;
        typedef ManyFunc const volatile  ManyFuncCV;

#define TEST(exp, ...) \
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
        applyPtrAndRef<FunctorTest<>, void              >();
        applyPtrAndRef<FunctorTest<>, char              >();
        applyPtrAndRef<FunctorTest<>, signed char       >();
#if BSLMF_INVOKERESULT_ABRIDGED_TEST
        // It is not really necessary to do an exhaustive test with every
        // numeric type. Instead, we do the full test for a handful of types
        // and just call 'FunctorTest<>::apply', without every combination of
        // cv references and pointers, for the rest of the numeric types.
        FunctorTest<>::apply<unsigned char     >();
        FunctorTest<>::apply<wchar_t           >();
        FunctorTest<>::apply<short             >();
        FunctorTest<>::apply<unsigned short    >();
        FunctorTest<>::apply<int               >();
        FunctorTest<>::apply<unsigned int      >();
        FunctorTest<>::apply<long int          >();
        FunctorTest<>::apply<unsigned long     >();
        FunctorTest<>::apply<long long         >();
        FunctorTest<>::apply<unsigned long long>();
        FunctorTest<>::apply<float             >();
        FunctorTest<>::apply<double            >();
#else // Full (not abridged) test
        applyPtrAndRef<FunctorTest<>, unsigned char     >();
        applyPtrAndRef<FunctorTest<>, wchar_t           >();
#   ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        applyPtrAndRef<FunctorTest<>, char16_t          >();
        applyPtrAndRef<FunctorTest<>, char32_t          >();
#   endif
        applyPtrAndRef<FunctorTest<>, short             >();
        applyPtrAndRef<FunctorTest<>, unsigned short    >();
        applyPtrAndRef<FunctorTest<>, int               >();
        applyPtrAndRef<FunctorTest<>, unsigned int      >();
        applyPtrAndRef<FunctorTest<>, long int          >();
        applyPtrAndRef<FunctorTest<>, unsigned long     >();
        applyPtrAndRef<FunctorTest<>, long long         >();
        applyPtrAndRef<FunctorTest<>, unsigned long long>();
        applyPtrAndRef<FunctorTest<>, float             >();
        applyPtrAndRef<FunctorTest<>, double            >();
#endif // Full (not abridged) test
        applyPtrAndRef<FunctorTest<>, long double       >();
        applyPtrAndRef<FunctorTest<>, bsl::nullptr_t    >();

        // Steps 4 & 5, concerns 5, 6, and 7
        applyPtrAndRef<FunctorTest<true>, MyEnum        >();
        applyPtrAndRef<FunctorTest<true>, MyClass       >();
        applyPtrAndRef<FunctorTest<true>, MyDerivedClass>();

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
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
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
        //: 1 Given class 'B', class 'D' derived from 'B' and
        //:   pointer-to-member type 'typedef R B::*MP',
        //:   'bsl::invoke_result<MP, T1>', yields an rvalue type 'R' if 'T1'
        //:   is (rvalue type) 'B' or 'D'. In C++03 mode, the rvalue type is
        //:   'R'; in C++11 and subsequent modes, the rvalue type is 'R&&'.
        //: 2 Similar to concern 1, except for 'T1' being a smart pointer type
        //:   which, when dereferenced, yields an rvalue of 'B' or 'D'. CV and
        //:   reference qualifiers on the smart pointer have no effect.
        //: 3 Given the same 'MP' as in concern 1,
        //:   'bsl::invoke_result<MP, T1>', yields type 'R&' if 'T1' is 'B&'
        //:   or 'D&'.
        //: 4 Similar to concern 3, except with 'T1' being a pointer or smart
        //:   pointer type that, when dereferenced, yields an lvalue of type
        //:   'B&' or 'D&'.
        //: 5 Similar to concerns 3 and 4, in C++11 and later,
        //:   'bsl::invoke_result<MP, T1>', yields type 'R&&' if 'T1' is 'B&&'
        //:   or is a smart pointer type that, when dereferenced, yields an
        //:   rvalue reference of type 'B&&' or 'D&&'.
        //: 6 If 'B' or 'D' in concerns 1-5 are cv-qualified, the result has
        //:   the union of cv qualifications of 'R' and the class type 'B' or
        //:   'D'. Reference and CV qualifiers on a pointer or smart pointer
        //:   itself have no effect.
        //: 7 Concerns 1-6 apply for 'R' of the following types:
        //:    o built-in numeric types
        //:    o user-defined class and enumeration types
        //:    o pointer (possibly cv-qualified) to any of the above
        //:    o pointer to (possibly cv-qualified) void
        //:    o cv-qualified types
        //:    o arrays (which do not decay)
        //
        // Plan:
        //: 1 For concern 1, define a functor template 'PtrToMemObjTest<T1>'
        //:   with an 'apply<R>' member function template that declares a
        //:   pointer to 'R' member of 'MyClass' and verifies that
        //:   applying 'bsl::invoke_result' to 'R (MyClass::*)(T1)' yields
        //:   'R'. Invoke this functor through a function 'applyPtrAndRef'
        //:   that adds every combination of pointer, reference, and const
        //:   qualifiers to 'R'. Use 'MyClass' and 'MyDerivedClass' for 'T1',
        //:   along with a sampling of CV qualifications for those.
        //: 2 For concern 2, add a parameter to 'PtrToMemObjTest' to indicate
        //:   "effective argument" type, i.e., the variation of 'MyClass'
        //:   yielded by the second template parameter to 'invoke_result'.
        //:   Create a smart pointer class template 'ProxyPtr<TP>' that defines
        //:   'TP operator*()'.  Repeat step 1, using 'ProxyPtr<MyClass>' and
        //:   'ProxyPtr<MyDerivedClass>' for 'T1', and 'MyClass' and
        //:   'MyDerivedClass', respectively, for the effective argument
        //:   type. Also try a sampling of CV qualifications on both the
        //:   'ProxyPtr' and on its template parameter. Show that a reference
        //:   qualifier on the 'ProxyPtr' has no effect, as well.
        //: 3 Repeat step 1 using 'MyClass&' and 'MyDerivedClass&' for 'T1'
        //:   and indicating an expected lvalue result.
        //: 4 For concern 4, repeat step 3, using 'MyClass*' and
        //:   'MyDerivedClass*' insted of 'MyClass&' and
        //:   'MyDerivedClass&'.  Create a class template 'SmartPtr<TP>' that
        //:   defines 'TP& operator*()' and repeat step 3, using
        //:   'SmartPtr<MyClass>' and 'SmartPtr<MyDerivedClass>' for 'T1' and
        //:   using 'MyClass&' and 'MyDerivedClass&', respectively, for the
        //:   effective argument type.  Use a sampling of reference and CV
        //:   qualifiers on the 'SmartPtr' (but not it's template parameter)
        //:   to show that it has no effect.
        //: 5 For concern 5, repeat step 3 using 'MyClass&' and
        //:   'MyDerivedClass&' for 'T1' and indicating an expected rvalue
        //:   reference result. Create a class template 'MovePtr<TP>' that
        //:   defines 'TP&& operator*()' and repeat step 3 using
        //:   'MovePtr<MyClass>' and 'MovePtr<MyDerivedClass>' for 'T1' and
        //:   using 'MyClass&&' and 'MyDerivedClass&&', respectively, for the
        //:   effective argument type. Use a sampling of reference and CV
        //:   qualifiers on the 'MovePtr' (but not it's template parameter) to
        //:   show that it has no effect.
        //: 6 For concern 6, modify 'PtrToMemObjTest' so that it expects the
        //:   union of the cv-qualifications specified in the effective
        //:   argument type parameter and those already on 'R'. Repeat steps
        //:   1-5, supplying different cv qualifications for the 'MyClass'
        //:   and 'MyDerivedClass' parameters.
        //: 7 For concern 7, modify steps 1-6 with 'R' of type 'void',
        //:   'int', 'float', user-defined type 'Ic01', 'const int',
        //:   'volatile short', and 'const volatile MyEnum'. Note that 'void'
        //:   will be ignored, but *pointer to (cv-qualified)* 'void' will not.
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

        // Step 1: rvalue of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , void>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , int>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , float>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mc>                        , Arry>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , void>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , int>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , float>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mdc>                       , Arry>();

        // Step 2: rvalue proxy of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , void>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , int>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc>          , Arry>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , void>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , int>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc>         , Arry>();

        // Reference qualifications on 'ProxyPtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>&, Mc>        , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<const ProxyPtr<Mdc>&, Mc> , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<const volatile ProxyPtr<Mc>&, Mc>
                                                                  , int>();

        // Step 3: lvalue of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , void>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , int>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , float>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mc&>                       , Arry>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , void>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , int>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , float>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                      , Arry>();

        // Step 4a: pointer to 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , void>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , int>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , float>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                  , Arry>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , void>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , int>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , float>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>                , Arry>();

        // Step 4b: smart pointer to (lvalue of) 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , void>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , int>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , float>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , Cint>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>         , Arry>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , void>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , int>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , float>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , Cint>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>        , Arry>();

        // Reference and CV qualifications on 'SmartPtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>&, CMc&>      , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<const SmartPtr<VMc>&, VMc&>, CVenum>();
        applyPtrAndRef<PtrToMemObjTest<const volatile SmartPtr<Mc>&, Mc&>
                                                                  , int>();

        // Step 5a: rvalue reference to 'MyClass' or 'MyDerivedClass'
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                      , void>();
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                      , int>();
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                      , float>();
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                      , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                     , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                     , Arry>();

        // Step 5b: smart pointer returning 'MyClass&&' or 'MyDerivedClass&&'
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>        , void>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>        , int>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>        , float>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>        , Ic01>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>         , Cint>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>         , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>         , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>         , Arry>();

        // Reference and CV qualifications on 'MovePtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>&&, CMc&&>     , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<const volatile MovePtr<Mc>&, Mc&&>
                                                                  , int>();
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<Mc>&, Mc&&>  , CVenum>();

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // 'MovePtr<volatile MyClass>' conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<VMc>&, VMc&&>, CVenum>();
#endif // ! MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Step 6: Expect CV-qualification on reference should be union of
        // cv-qualification on the effective argument and the cv-qualification
        // of the member.
        applyPtrAndRef<PtrToMemObjTest<CMc&>                      , float>();
        applyPtrAndRef<PtrToMemObjTest<CMdc&>                     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CMc&>                      , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CMc&>                      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CMc&>                      , Arry>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                     , float>();
        applyPtrAndRef<PtrToMemObjTest<VMc&>                      , Cint>();
        applyPtrAndRef<PtrToMemObjTest<VMc&>                      , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                     , Arry>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                     , float>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CvMdc&>                    , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                     , Arry>();

// Older sun compiler gets confused returning a const-qualified rvalue.
#if ! (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc>        , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMdc>, CMc>       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc>        , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc>        , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc>        , Arry>();
#endif // Not Sun CC
#if 0 // Returning volatile prvalues is problematic in most compilers
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>       , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>        , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>        , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>       , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>       , Arry>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>      , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>      , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMdc>, CvMc>     , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>      , Arry>();
#endif // 0

        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>                , float>();
        applyPtrAndRef<PtrToMemObjTest<CMdc*, CMc&>               , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>                , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>                , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>                , Arry>();
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>               , float>();
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>                , Cint>();
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>                , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>               , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>               , Arry>();
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>              , float>();
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>              , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CvMdc*, CvMc&>             , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>              , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>              , Arry>();

        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>       , float>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMdc>, CMc&>      , Cint>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>       , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>       , Arry>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>      , float>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>      , Arry>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>     , float>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMdc>, CvMc&>    , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>     , Arry>();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                     , float>();
        applyPtrAndRef<PtrToMemObjTest<CMdc&&>                    , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                     , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                     , Arry>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                    , float>();
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                     , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                    , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                    , Arry>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                    , float>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                    , Cint>();
        applyPtrAndRef<PtrToMemObjTest<CvMdc&&>                   , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                    , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                    , Arry>();

        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>       , float>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMdc>, CMc&&>      , Cint>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>       , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>       , Arry>();

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // 'MovePtr<volatile MyClass>' conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>      , float>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>      , Arry>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>     , float>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMdc>, CvMc&&>    , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>     , Arry>();
#endif // MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of type pointer-to-member-function,
        //:   'bsl::invoke_result' returns the return type of the function.
        //: 2 Concern 1 applies to functions returning
        //:    o 'void'
        //:    o built-in numeric types
        //:    o user-defined class and enumeration types
        //:    o pointer (possibly cv-qualified) to any of the above
        //:    o Pointer-to-function, reference-to-function, and
        //:      pointer-to-member function.  No recursion is expected.
        //:    o Reference to any of the (possibly cv-qualified) types above
        //:      except 'void', including reference-to-pointer types.
        //:    o Reference-to-array or (in C++11) rvalue-reference-to-array.
        //: 3 Given, 'bsl::invoke_result<F, T1, ...>', where 'F' is a pointer
        //:   to member function of class 'B' and class 'D' is either 'B' or a
        //:   class derived from 'B', 'T1' can be 'D', cv-qualified 'D&',
        //:   'bsl::reference_wrapper<D>', 'D*', or 'smart_ptr<D>' for some
        //:   smart-pointer template.
        //: 4 Concern 1 applies to member functions that are cv-qualified
        //:   and/or ref-qualified.
        //: 5 Concern 1 applies for pointers to member functions taking 0 to 9
        //:   arguments, where some of the arguments are convertible to the
        //:    function parameters, rather than being an exact match.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor template
        //:   'PtrToMemFuncTest' which declares a pointer to
        //:   member-function-returning-R type and verifies that applying
        //:   'bsl::invoke_result' to that type yields 'R'. Invoke this functor
        //:   through a function 'applyPtrAndRef' that adds every combination
        //:   of pointer and reference qualifiers to 'R'.
        //: 2 For concern 3, parameterize 'PtrToMemFuncTest' on the type of
        //:   the first argument. Repeat step 1, supplying different
        //:   first-argument types that comprise the types in the concern.
        //: 3 For concern 4, include in 'PtrToMemFuncTest' pointers to member
        //:   functions that are lvalue-ref-qualified and
        //:   rvalue-ref-qualified, with every combination of cv-qualifiers.
        //: 4 For concern 5, instantiate 'bsl::invoke_result' on
        //:   member-function-pointer types taking 0 to 9 arguments.
        //:   Concerns 2 and 3 do not interract, so it is not necessary to
        //:   test every combination of 0 to 9 arguments with every possible
        //:   return type.
        //
        // Testing:
        //     POINTER-TO-MEMBER-FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES"
                            "\n============================================="
                            "\n");

        typedef Ic01 (*Fp)(MyClass, int);
        typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Arry[10];
        typedef Ic04 F(MyClass, int);

        applyPtrAndRef<PtrToMemFuncTest<MyClass>, void>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, int>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, float>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyClass>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyEnum>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, Fp>();
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MFp>();

        // Arrays and functions cannot be returned by value, so we test only
        // references.  Can't apply a pointer or reference to something that
        // is already a reference, so simply call the 'apply' method directly.
        PtrToMemFuncTest<MyClass>::apply<Arry&>();
        PtrToMemFuncTest<MyClass>::apply<Arry const&>();
        PtrToMemFuncTest<MyClass>::apply<F&>();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        PtrToMemFuncTest<MyClass>::apply<Arry&&>();
        PtrToMemFuncTest<MyClass>::apply<F&&>();
#endif

        typedef int        i;
        typedef ToLongLong q;
        typedef short      s;

        typedef Ic01 (MyClass::*Fp01)(MyClass);
        typedef Ic02 (MyClass::*Fp02)(MyClass,i);
        typedef Ic03 (MyClass::*Fp03)(MyClass,i,i);
        typedef Ic04 (MyClass::*Fp04)(MyClass,i,i,i);
        typedef Ic05 (MyClass::*Fp05)(MyClass,i,i,i,i);
        typedef Ic05 (MyClass::*Fp05c)(MyClass,i,i,i,i) const;
        typedef Ic05 (MyClass::*Fp05v)(MyClass,i,i,i,i) volatile;
        typedef Ic05 (MyClass::*Fp05cv)(MyClass,i,i,i,i) const volatile;
        typedef Ic06 (MyClass::*Fp06)(MyClass,i,i,i,i,i) volatile;
        typedef Ic07 (MyClass::*Fp07)(MyClass,i,i,i,i,i,i);
        typedef Ic08 (MyClass::*Fp08)(MyClass,i,i,i,i,i,i,i);
        typedef Ic09 (MyClass::*Fp09)(MyClass,i,i,i,i,i,i,i,i) const;
        typedef Ic10 (MyClass::*Fp10)(MyClass,i,i,i,i,i,i,i,i,i) const;
        // typedef Ic11 (MyClass::*Fp11)(MyClass,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic12 (MyClass::*Fp12)(MyClass,i,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic13 (MyClass::*Fp13)(MyClass,i,i,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic14 (MyClass::*Fp14)(MyClass,i,i,i,i,i,i,i,i,i,i,i,i,i);

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
        // TEST(Ic12, Fp12,MyClass,i,i,i,i,i,i,i,i,i,i,i     );
        // TEST(Ic13, Fp13,MyClass,i,i,i,i,i,i,i,i,i,i,i,i   );
        // TEST(Ic14, Fp14,MyClass,i,i,i,i,i,i,i,i,i,i,i,i,i );

        typedef SmartPtr<MyClass>              SpMc;
        typedef SmartPtr<MyDerivedClass>       SpMdc;
        typedef SmartPtr<const MyClass>        SpCMc;
        typedef SmartPtr<const MyDerivedClass> SpCMdc;

        typedef bsl::reference_wrapper<MyClass>    Rr;

        TEST(Ic01, Fp01,MyClass&                                  );
        TEST(Ic02, Fp02,MyClass*,i                                );
        TEST(Ic03, Fp03,MyDerivedClass,q,s                        );
        TEST(Ic04, Fp04,MyDerivedClass*,i,i,i                     );
        TEST(Ic05, Fp05,MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05c,MyDerivedClass&,i,i,i,i                  );
        TEST(Ic05, Fp05c,const MyDerivedClass&,i,i,i,i            );
        TEST(Ic05, Fp05cv,const MyDerivedClass&,i,i,i,i           );
        TEST(Ic05, Fp05cv,volatile MyDerivedClass&,i,i,i,i        );
        TEST(Ic05, Fp05cv,const volatile MyDerivedClass&,i,i,i,i  );
        TEST(Ic06, Fp06,const MyClass&,i,i,i,i,i                  );
        TEST(Ic06, Fp06,volatile MyClass&,i,i,i,i,i               );
        TEST(Ic07, Fp07,SpMc,i,i,i,i,i,i                          );
        TEST(Ic08, Fp08,SpMdc,i,i,q,i,i,i,i                       );
        TEST(Ic09, Fp09,SpCMc,i,s,i,i,i,i,i,i                     );
        TEST(Ic10, Fp10,SpCMdc,q,i,i,i,i,i,i,i,i                  );
        TEST(Ic02, Fp02,Rr,i                                      );
        TEST(Ic03, Fp03,Rr&,i,i                                   );

#undef TEST

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of function type,
        //:   'bsl::invoke_result' returns the return type of the function.
        //: 2 Concern 1 applies to functions returning
        //:    o 'void'
        //:    o built-in numeric types
        //:    o user-defined class and enumeration types
        //:    o pointer (possibly cv-qualified) to any of the above
        //:    o Pointer-to-function, reference-to-function, and
        //:      pointer-to-member function.  No recursion is expected.
        //:    o Reference to any of the (possibly cv-qualified) types above
        //:      except 'void', including reference-to-pointer types.
        //:    o Reference-to-array or (in C++11) rvalue-reference-to-array.
        //: 3 Concern 1 applies for pointers to functions taking 0 to 10
        //:   arguments, where some arguments are convertible to the function
        //:   argument rather than an exact match.
        //: 4 Concerns 1 to 3 apply equally for invocables of
        //:   pointer-to-function type.
        //: 5 Concerns 1 to 3 apply equally for invocables of (lvalue & rvalue)
        //:   reference-to-function type.
        //: 6 Results are not affected by arguments that decay (arrays,
        //:   functions), reference binding, or valid volatile arguments.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor template 'FuncTest' which
        //:   declares a function-returning-R type and verifies that applying
        //:   'bsl::invoke_result' to that type yields 'R'. Invoke this functor
        //:   through a function 'applyPtrAndRef' that adds every combination
        //:   of pointer and reference qualifiers to 'R'.
        //: 2 For concerns 1 and 3, instantiate 'bsl::invoke_result' on a
        //:   function type taking 0 to 10 arguments.  Concerns 2 and
        //:   3 do not interract, so it is not necessary to test every
        //:   combination of 0 to 10 arguments with every possible return
        //:   type.
        //: 3 For concern 4, extend 'FuncTest' to also test
        //:   pointer-to-function, reference to pointer-to-function, and
        //:   reference to 'const' pointer-to-function.  Also repeat step 2
        //:   with pointer-to-function types.
        //: 4 For concern 5, extend 'FuncTest' to also test (lvalue & rvalue)
        //:   reference-to-function.  Also repeat step 2 with
        //:   reference-to-function types.
        //: 5 For concern 6, invoke 'bsl::invoke_result' in a one-off fashion
        //:   with the argument categories listed in the concern and verify
        //:   correct operation.
        //
        // Testing:
        //     FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION INVOCABLES"
                            "\n===========================\n");

        BSLA_MAYBE_UNUSED typedef Ic01 (*Fp)(MyClass, int);
        BSLA_MAYBE_UNUSED typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Arry[10];
        typedef Ic04 F(MyClass, int);

        // Step 1, Concerns 1 & 2
        applyPtrAndRef<FuncTest, void>();
        applyPtrAndRef<FuncTest, int>();
        applyPtrAndRef<FuncTest, float>();
        applyPtrAndRef<FuncTest, MyClass>();
        applyPtrAndRef<FuncTest, MyEnum>();

        // Arrays and functions cannot be returned by value, so we test only
        // references.  We can't apply a pointer or reference to something that
        // is already a reference, so simply call the 'apply' method directly.
        FuncTest::apply<Arry&>();
        FuncTest::apply<Arry const&>();
        FuncTest::apply<F&>();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        FuncTest::apply<Arry&&>();
        FuncTest::apply<F&&>();
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
        // typedef Ic11 F11(i,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic12 F12(i,i,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic13 F13(i,i,i,i,i,i,i,i,i,i,i,i,i);
        // typedef Ic14 F14(i,i,i,i,i,i,i,i,i,i,i,i,i,i);

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
        // TEST(Ic11, F11,i,i,i,i,i,i,i,i,i,i,i       );
        // TEST(Ic12, F12,i,i,i,i,i,i,i,i,i,i,i,i     );
        // TEST(Ic13, F13,i,i,i,i,i,i,i,i,i,i,i,i,i   );
        // TEST(Ic14, F14,i,i,i,i,i,i,i,i,i,i,i,i,i,i );

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
        // TEST(Ic11, F11*,i,i,i,i,i,i,i,i,i,i,i       );
        // TEST(Ic12, F12*,i,i,i,i,i,i,i,i,i,i,i,i     );
        // TEST(Ic13, F13*,i,i,i,i,i,i,i,i,i,i,i,i,i   );
        // TEST(Ic14, F14*,i,i,i,i,i,i,i,i,i,i,i,i,i,i );

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
        // TEST(Ic11, F11&,i,i,i,i,i,i,i,i,i,i,i       );
        // TEST(Ic12, F12&,i,i,i,i,i,i,i,i,i,i,i,i     );
        // TEST(Ic13, F13&,i,i,i,i,i,i,i,i,i,i,i,i,i   );
        // TEST(Ic14, F14&,i,i,i,i,i,i,i,i,i,i,i,i,i,i );

        // Steps 3 & 4 incorporated into 'FuncTest' (step 1) and into step 2

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
        //: 1 'bsl::invoke_result<FT, ARGS...>' compiles for a sampling of
        //:   parameter types
        //
        // Plan:
        //: 1 Instantiate 'bsl::resulf_of' on a sampling of invocable types
        //:   and verify that the computed return type matches the expected
        //:   return type.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#define TEST(exp, ...) \
        ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, \
                exp>::value))

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
        //   ===================  ==============================
        TEST(void               , f1                             );
        TEST(int                , f2, char                       );
        TEST(const int&         , f3, bool, float                );
        TEST(volatile int&      , f4, char*, int, int*           );
        TEST(const volatile int&, f5, short, int&, int, char     );
        TEST(ManyFunc           , f6, bool                       );
        TEST(double             , g1, ManyFunc, int              );
        TEST(double&            , g2, ManyFunc, int              );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(char*&&            , dp, MyClass                    );
#else
        TEST(char*              , dp, MyClass                    );
#endif
        TEST(bool               , ManyFunc, float, void*         );
        TEST(float              , ConstManyFunc, float, void*    );

        TEST(void               , ManyFunc, MetaType<void>, int          );
        TEST(int *              , ManyFunc, MetaType<int *>, int         );
        TEST(int *              , ManyFunc, MetaType<int *volatile>, int );
        TEST(int *volatile&     , ManyFunc, MetaType<int *volatile&>, int);
        TEST(const void *       , ManyFunc, MetaType<const void *>, int  );
        TEST(volatile void *const&, ManyFunc
                                , MetaType<volatile void *const&>, int   );
        TEST(const char*        , ManyFunc, MetaType<const char*>, int   );

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

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
