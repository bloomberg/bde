// bsls_keyword.t.cpp                                                 -*-C++-*-

#include <bsls_keyword.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma error_messages(off, hidevf)
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 2] BSLS_KEYWORD_CONSTEXPR
// [  ] BSLS_KEYWORD_CONSTEXPR_MEMBER
// [ 3] BSLS_KEYWORD_CONSTEXPR_RELAXED
// [ 3] BSLS_KEYWORD_CONSTEXPR_CPP14
// [ 4] BSLS_KEYWORD_CONSTEXPR_CPP17
// [10] BSLS_KEYWORD_DELETED
// [ 5] BSLS_KEYWORD_EXPLICIT
// [ 6] BSLS_KEYWORD_FINAL (class)
// [ 7] BSLS_KEYWORD_FINAL (function)
// [  ] BSLS_KEYWORD_INLINE_CONSTEXPR
// [  ] BSLS_KEYWORD_INLINE_VARIABLE
// [ 8] BSLS_KEYWORD_NOEXCEPT
// [  ] BSLS_KEYWORD_NOEXCEPT_AVAILABLE
// [ 8] BSLS_KEYWORD_NOEXCEPT_OPERATOR
// [ 8] BSLS_KEYWORD_NOEXCEPT_SPECIFICATION
// [ 9] BSLS_KEYWORD_OVERRIDE
//-----------------------------------------------------------------------------
// [11] USAGE EXAMPLE
// [ 1] Test machinery

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

#define STRINGIFY_(a, b) #b
#define STRINGIFY(...) STRINGIFY_(0, __VA_ARGS__)

//=============================================================================

namespace
{
    template <class, bool Pred>
    struct TestMetafunction {
        // A meta-function for testing.  It takes a type parameter (ignored)
        // and a boolean non-type parameter that is "returned" as the member
        // 'value'.
        enum { value = Pred };
    };

    void noThrow1() BSLS_KEYWORD_NOEXCEPT                           {}
    void noThrow2() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(true)       {}
    void noThrow3() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                        BSLS_KEYWORD_NOEXCEPT_OPERATOR(noThrow1())) {}
    void noThrow4() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                        TestMetafunction<void, true>::value)        {}
    void throws1()  BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)      {}
    void throws2()  BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                        BSLS_KEYWORD_NOEXCEPT_OPERATOR(throws1()))  {}
    void throws3()  BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                        TestMetafunction<void, false>::value)       {}
    template <class, class>
    void throws4() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)       {}
    void throws5() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)       {}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Preparing C++03 Code for C++11 Features
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// To use these macros, simply insert them where the corresponding C++11
// keyword would go.  When compiling with C++03 mode there will be no effect
// but when compiling with C++11 mode additional restrictions will apply.  When
// compiling with C++11 mode the restriction will be checked providing some
// additional checking over what is done with C++11.
//
// C++ uses the 'explicit' keyword to indicate that constructors taking just
// one argument are not considered for implicit conversions.  Instead, they can
// only be used for explicit conversions.  C++ also provides the ability to
// define conversion operators but prior to C++11 these conversion operators
// are considered for implicit conversion.  C++11 allows the use of the
// 'explicit' keyword with conversion operators to avoid its use for implicit
// conversions.  The macro 'BSLS_KEYWORD_EXPLICIT' can be used to mark
// conversions as explicit conversions which will be checked when compiling
// with C++11 mode.  For example, an 'Optional' type may have an explicit
// conversion to 'bool' to indicate that the value is set (note the conversion
// operator):
//..
template <class TYPE>
class Optional
{
    TYPE* d_value_p;
public:
    Optional(): d_value_p() {}
    explicit Optional(const TYPE& value): d_value_p(new TYPE(value)) {}
    ~Optional() { delete d_value_p; }
    // ...

    BSLS_KEYWORD_EXPLICIT operator bool() const { return d_value_p; }
};
//..
// When using an object of the 'Optional' class in a condition it is desirable
// that it converts to a 'bool':
//..
void testFunction() {
    Optional<int> value;
    if (value) { /*... */ }
//..
// In places where an implicit conversion takes place it is not desirable that
// the conversion is used.  When compiling with C++11 mode the conversion
// operator will not be used, e.g., the following code will result in an error:
//..
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
    bool flag = value;
#endif
}
//..
// The code will compile successfully when using C++03 mode; without the macro,
// when using C++11 or greater mode we get an error like this:
//..
//     error: cannot convert 'Optional<int>' to 'bool' in initialization
//..
//
// When defining conversion operators to 'bool' for code which needs to compile
// with C++03 mode the conversion operator should convert to a member pointer
// type instead: doing so has a similar effect to making the conversion
// operator 'explicit'.
//
// Some classes are not intended for use as a base class.  To clearly label
// these classes and enforce that they can't be derived from C++11 allows using
// the 'final' keyword after the class name in the class definition to label
// classes which are not intended to be derived from.  The macro
// 'BSLS_KEYWORD_FINAL' is replaced by 'final' when compiling with C++11
// causing the compiler to enforce that a class can't be further derived.  The
// code below defines a class which can't be derived from:
//..
class FinalClass BSLS_KEYWORD_FINAL
{
    int d_value;
public:
    explicit FinalClass(int value = 0): d_value(value) {}
    int value() const { return d_value; }
};
//..
// An attempt to derive from this class will fail when compiling with C++11
// mode:
//..
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
class FinalClassDerived : public FinalClass {
    int d_anotherValue;
public:
    explicit FinalClassDerived(int value)
    : d_anotherValue(2 * value) {
    }
    int anotherValue() const { return d_anotherValue; }
};
#endif
//..
// The code will compile successfully when using C++03 mode; without the macro,
// when using C++11 or greater mode we get an error like this:
//..
//    error: cannot derive from 'final' base 'FinalClass' in derived type
//    'FinalClassDerived'
//..
//
// Sometime it is useful to declare that an overriding function is the final
// overriding function and further derived classes won't be allowed to further
// override the function.  One use of this feature could be informing the
// compiler that it won't need to use virtual dispatch when calling this
// function on a pointer or a reference of the corresponding type.  C++11
// allows marking functions as the final overrider using the keyword 'final'.
// The macro 'BSLS_KEYWORD_FINAL' can also be used for this purpose.  To
// demonstrate the use of this keyword first a base class with a 'virtual'
// function is defined:
//..
struct FinalFunctionBase
{
    virtual int f() { return 0; }
};
//..
// When defining a derived class this function 'f' can be marked as the final
// overrider using 'BSLS_KEYWORD_FINAL':
//..
struct FinalFunctionDerived: FinalFunctionBase
{
    int f() BSLS_KEYWORD_FINAL { return 1; }
};
//..
// The semantics of the overriding function aren't changed but a further
// derived class can't override the function 'f', i.e., the following code will
// result in an error when compiling with C++11 mode:
//..
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
struct FinalFunctionFailure: FinalFunctionDerived
{
    int f() { return 2; }
};
#endif
//..
// The code will compile successfully when using C++03 mode; without the macro,
// when using C++11 or greater mode we get an error like this:
//..
//     error: virtual function 'virtual int FinalFunctionFailure::f()'
//     error: overriding final function 'virtual int FinalFunctionDerived::f()'
//..
//
//
// The C++11 keyword 'override' is used to identify functions overriding a
// 'virtual' function from a base class.  If a function identified as
// 'override' does not override a 'virtual' function from a base class the
// compilation results in an error.  The macro 'BSLS_KEYWORD_OVERRIDE' is used
// to insert the 'override' keyword when compiling with C++11 mode.  When
// compiling with C++03 mode it has no effect but it both cases it documents
// that a function is overriding a 'virtual' function from a base class.  To
// demonstrate the use of the 'BSLS_KEYWORD_OVERRIDE' macro first a base class
// is defined:
//..
struct OverrideBase
{
    virtual int f() const { return 0; }
};
//..
// When overriding 'OverrideBase::f' in a derived class the
// 'BSLS_KEYWORD_OVERRIDE' macro should be used to ascertain that the function
// in the derived class is indeed overriding a 'virtual' function:
//..
struct OverrideSuccess: OverrideBase
{
    int f() const BSLS_KEYWORD_OVERRIDE { return 1; }
};
//..
// The above code compiles successfully with both C++03 mode and C++11.  When
// the function meant to be an override actually isn't overriding any function
// the compilation will fail when using C++11 mode as is demonstrated by the
// following example (note the missing 'const' in the function declaration):
//..
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
struct OverrideFailure: OverrideBase
{
    int f() BSLS_KEYWORD_OVERRIDE { return 2; }
};
#endif
//..
// The code will compile successfully when using C++03 mode (though it might
// produce a warning); without the macro, when using C++11 or greater mode we
// get an error like this:
//..
//    error: 'int OverrideFailure::f()' marked 'override', but does not
//    override
//..
//
///Example 2: Creating an extended 'constexpr' function
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// To use these macros, simply insert them where the corresponding C++14
// keyword would go.  When compiling with C++03 or C++11 mode there will be no
// effect but when compiling with C++14 mode additional restrictions will
// apply.  When compiling with C++14 mode the restriction will be checked
// providing some additional checking over what is done with C++11 or C++03.
//
// C++11 uses the 'constexpr' keyword to indicate that a (very simple) function
// may be evaluated compile-time if all its input is known compile time.  C++14
// allows more complex functions to be 'constexpr'.  Also, in C++14,
// 'constexpr' member functions are not implicitly 'const' as in C++11.
// Thefore we have a separate macro 'BSLS_KEYWORD_CONSTEXPR_CPP14' that can be
// used to mark functions 'constexpr' when compiling with C++14 mode:
//..
BSLS_KEYWORD_CONSTEXPR_CPP14
int complexConstexprFunc(bool b)
{
    if (b) {
        return 42;                                                // RETURN
    }
    else {
        return 17;                                                // RETURN
    }
}
//..
// When compiling with C++14 'constexpr' support it is possible to use the
// result of 'complexConstexprFunc' in compile-time constants:
//..
void useComplexConstexprFunc()
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    constexpr
#endif
    int result = complexConstexprFunc(true);
    ASSERT(42 == result);
//..
// The macro 'BSLS_KEYWORD_CONSTEXPR_CPP14' can also be used on variables to
// achieve an identical result:
//..
    BSLS_KEYWORD_CONSTEXPR_CPP14 int result2 = complexConstexprFunc(true);
    ASSERT(42 == result2);
}
//..
// C++17 made small but significant changes to what is allowed in a 'constexpr'
// function.  Notably, a lambda can now be defined in such a function (and, if
// not called at compile time, does not itself need to be 'constexpr').  To
// take advantage of this there is a separate macro
// 'BSLS_KEYWORD_CONSTEXPR_CPP14' that can be used to mark functions
// 'constexpr' when compiling with C++17 mode:
//..
BSLS_KEYWORD_CONSTEXPR_CPP17
int moreComplexConstexprFunc(bool b)
{
    if (b) {
        return 42;                                                // RETURN
    }
    else {
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        return []{
                   static int b = 17;
                   return b;
               }();                                               // RETURN
#else
        return 17;
#endif
    }
}
//..
// Then, just like 'useComplexConstexprFunc', we can invoke
// 'moreComplexConstexprFunc' to populate a compile-time constant when it is
// suported:
//..
void useMoreComplexConstexprFunc()
{
    BSLS_KEYWORD_CONSTEXPR_CPP17 int result
                                          = moreComplexConstexprFunc(true);
    ASSERT(42 == result);
}
//..

}  // close unnamed namespace

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\n  printFlags: bsls_keyword Macros\n");

    printf("\n  BSLS_KEYWORD_CONSTEXPR: ");
#ifdef BSLS_KEYWORD_CONSTEXPR
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_CONSTEXPR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_CONSTEXPR_RELAXED: ");
#ifdef BSLS_KEYWORD_CONSTEXPR_RELAXED
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_CONSTEXPR_RELAXED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_CONSTEXPR_CPP14: ");
#ifdef BSLS_KEYWORD_CONSTEXPR_CPP14
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_CONSTEXPR_CPP14) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_CONSTEXPR_CPP17: ");
#ifdef BSLS_KEYWORD_CONSTEXPR_CPP17
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_CONSTEXPR_CPP17) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_DELETED: ");
#ifdef BSLS_KEYWORD_DELETED
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_DELETED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_EXPLICIT: ");
#ifdef BSLS_KEYWORD_EXPLICIT
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_EXPLICIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_FINAL: ");
#ifdef BSLS_KEYWORD_FINAL
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_FINAL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_INLINE_VARIABLE: ");
#ifdef BSLS_KEYWORD_INLINE_VARIABLE
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_INLINE_VARIABLE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_NOEXCEPT: ");
#ifdef BSLS_KEYWORD_NOEXCEPT
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_NOEXCEPT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_NOEXCEPT_AVAILABLE: ");
#ifdef BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_NOEXCEPT_AVAILABLE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_NOEXCEPT_OPERATOR(...): ");
#ifdef BSLS_KEYWORD_NOEXCEPT_OPERATOR
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_NOEXCEPT_OPERATOR(...)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(...): ");
#ifdef BSLS_KEYWORD_NOEXCEPT_SPECIFICATION
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(...)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_KEYWORD_OVERRIDE: ");
#ifdef BSLS_KEYWORD_OVERRIDE
    printf("%s\n", STRINGIFY(BSLS_KEYWORD_OVERRIDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n  printFlags: bsls_keyword Referenced Macros\n");

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_FINAL: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_FINAL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
    printf("%s\n",
                   STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Run the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE"
                            "\n=====================\n");

        (void)testFunction();

#undef    FAIL_USAGE_EXPLICIT
//#define FAIL_USAGE_EXPLICIT
#undef    FAIL_USAGE_FINAL_CLASS
//#define FAIL_USAGE_FINAL_CLASS
#undef    FAIL_USAGE_FINAL_FUNCTION
//#define FAIL_USAGE_FINAL_FUNCTION
#undef    FAIL_USAGE_OVERRIDE_TYPE
//#define FAIL_USAGE_OVERRIDE_TYPE
#undef    FAIL_USAGE_NO_OVERRIDE
//#define FAIL_USAGE_NO_OVERRIDE

        Optional<int> value;
        ASSERT(bool(value) == false);
        if (value) { /*... */ }
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT) \
 || defined(FAIL_USAGE_EXPLICIT)
        bool flag = value;
        ASSERT(flag == false);
#endif

        class FinalClass BSLS_KEYWORD_FINAL
        {
            int d_value;
        public:
            explicit FinalClass(int value = 0)
                // Initialize the object with the optionally specified 'value'.
                : d_value(value) {}
            int value() const { return d_value; }
                // Returns the object's value.
        };
        class FinalClassDerived
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL) \
 || defined(FAIL_USAGE_FINAL_CLASS)
            : public FinalClass
#endif
        {
            int d_anotherValue;
        public:
            explicit FinalClassDerived(int value)
                // Initialize the object with the specified 'value'.
                : d_anotherValue(2 * value) {
            }
            int anotherValue() const { return d_anotherValue; }
                // Returns another value for the object.
        };

        FinalClass finalValue(1);
        ASSERT(finalValue.value() == 1);
        FinalClassDerived derivedValue(2);
        ASSERT(derivedValue.anotherValue() == 4);

        struct FinalFunctionBase
        {
            virtual int f() { return 0; }
                // Returns a value associated with the class's type.
        };
        struct FinalFunctionDerived: FinalFunctionBase
        {
            int f() BSLS_KEYWORD_FINAL
                // Returns a value associated with the class's type.
            { return 1; }
        };
        struct FinalFunctionFailure: FinalFunctionDerived
        {
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL) \
 || defined(FAIL_USAGE_FINAL_FUNCTION)
            int f()
                // Returns a value associated with the class's type.
            { return 2; }
#endif
        };

        FinalFunctionBase finalFunctionBase;
        ASSERT(finalFunctionBase.f()    == 0);

        FinalFunctionDerived finalFunctionDerived;
        ASSERT(finalFunctionDerived.f() == 1);

        FinalFunctionFailure finalFunctionFailure;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
        ASSERT(finalFunctionFailure.f() == 1);
#else
        ASSERT(finalFunctionFailure.f() == 2);
#endif

        struct OverrideBase
        {
            virtual int f() const
                // Returns a value associated with the type.
            { return 0; }
        };
        struct OverrideSuccess: OverrideBase
        {
            int f() const BSLS_KEYWORD_OVERRIDE
                // Returns a value associated with the type.
            { return 1; }
        };
        struct OverrideFailure: OverrideBase
        {
#if !defined(FAIL_USAGE_OVERRIDE_TYPE) && !defined(FAIL_USAGE_NO_OVERRIDE)
            int f() const BSLS_KEYWORD_OVERRIDE
#elif defined(FAIL_USAGE_OVERRIDE_TYPE)
            int f() BSLS_KEYWORD_OVERRIDE
#elif defined(FAIL_USAGE_NO_OVERRIDE)
            int f()
#endif
                // Returns a value associated with the type.
            { return 2; }
        };

        OverrideBase overrideBase;
        ASSERT(overrideBase.f() == 0);
        OverrideSuccess overrideSuccess;
        ASSERT(overrideSuccess.f() == 1);
        ASSERT(static_cast<const OverrideBase&>(overrideSuccess).f() == 1);
        OverrideFailure overrideFailure;
        ASSERT(overrideFailure.f() == 2);
#if defined(FAIL_USAGE_NO_OVERRIDE)
        // We expect the following to fail

        ASSERT(static_cast<const OverrideBase&>(overrideFailure).f() == 2);

        // We expect the following to succeed

        ASSERT(static_cast<const OverrideBase&>(overrideFailure).f() == 0);
#endif

        // See before 'main' for 'useComplexConstexprFunc' and
        // 'useMoreComplexConstexprFunc'.
        useComplexConstexprFunc();
        useMoreComplexConstexprFunc();

#undef    FAIL_USAGE_EXPLICIT
#undef    FAIL_USAGE_FINAL_CLASS
#undef    FAIL_USAGE_FINAL_FUNCTION
#undef    FAIL_USAGE_OVERRIDE_TYPE
#undef    FAIL_USAGE_NO_OVERRIDE
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_KEYWORD_DELETED
        //
        // Concerns:
        //: 1 When 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS' is defined
        //:   'BSLS_KEYWORD_DELETED' evaluates to = delete.
        //:
        //: 2 When 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS' is not
        //:   defined 'BSLS_KEYWORD_DELETED' evaluates to nothing.
        //
        // Plan:
        //: 1 Compare the stringified value of the macro to an oracle.
        //
        // Testing:
        //   BSLS_KEYWORD_DELETED
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: BSLS_KEYWORD_DELETED"
                            "\n=============================\n");

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
            "= delete";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_DELETED), expected) == 0);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_KEYWORD_OVERRIDE
        //
        // Concerns:
        //: 1 Marking an overriding function as 'override' using
        //:   'BSLS_KEYWORD_OVERRIDE' shall compile OK both with C++03 and
        //:   C++11 mode.
        //:
        //: 2 Marking a function which isn't overriding a 'virtual' function as
        //:   'override' using 'BSLS_KEYWORD_OVERRIDE' shall fail to compile
        //:   when compiling using C++11 mode.  It may fail when using C++03
        //:   mode due to a warning about an overload hiding base class
        //:   functions.
        //
        // Plan:
        //: 1 Define a base class with a 'virtual' function and two derived
        //:   classes which override the function correctly and incorrectly.
        //:   In both cases use the 'BSLS_KEYWORD_OVERRIDE' macro and determine
        //:   if the compilation is successful.  The incorrect use is guarded
        //:   by the macro 'FAIL_OVERRIDE' to restrict compilation failure to
        //:   compilations with this macro defined.
        //
        // Testing:
        //   BSLS_KEYWORD_OVERRIDE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: BSLS_KEYWORD_OVERRIDE"
                            "\n==============================\n");

#undef    FAIL_OVERRIDE_TYPE
//#define FAIL_OVERRIDE_TYPE
#undef    FAIL_NO_OVERRIDE
//#define FAIL_NO_OVERRIDE

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE)
            "override";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_OVERRIDE), expected) == 0);

        struct Base
        {
            virtual int f() const
                // Returns a value for each type.
            {
                return 0;
            }
        };
        struct OverrideOK
            : Base
        {
            int f() const BSLS_KEYWORD_OVERRIDE
                // Returns a value specific to this type.
            {
                return 1;
            }
        };
        struct OverrideFail
            : Base
        {
#if   !defined(FAIL_OVERRIDE_TYPE) && !defined(FAIL_NO_OVERRIDE)
            int f() const BSLS_KEYWORD_OVERRIDE
#elif  defined(FAIL_OVERRIDE_TYPE)
            int f()       BSLS_KEYWORD_OVERRIDE
#elif  defined(FAIL_NO_OVERRIDE)
            int f()
#endif
                // Returns a value specific to this type.
            {
                return 2;
            }
        };

        OverrideOK ok;
        ASSERT(ok.f() == 1);
        ASSERT(static_cast<const Base&>(ok).f() == 1);
        OverrideFail fail;
        ASSERT(fail.f() == 2);
#if defined(FAIL_NO_OVERRIDE)
        // expect to fail

        ASSERT(static_cast<const Base&>(fail).f() == 2);

        // expect to succeed

        ASSERT(static_cast<const Base&>(fail).f() == 0);
#elif !defined(FAIL_OVERRIDE_TYPE) && !defined(FAIL_NO_OVERRIDE)
        ASSERT(static_cast<const Base&>(fail).f() == 2);
#endif

#undef    FAIL_OVERRIDE_TYPE
#undef    FAIL_NO_OVERRIDE
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING NOEXCEPT SUPPORT
        //
        // Concerns:
        //: 1 Marking a function 'noexcept' using 'BSLS_KEYWORD_NOEXCEPT' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)' or
        //:   BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
        //:       BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr))' should result in a
        //:   successful compilation in C++03 mode.
        //:
        //: 2 Marking a function 'noexcept' or 'noexcept(bool)' using
        //:   'BSLS_KEYWORD_NOEXCEPT' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)' or
        //:   BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
        //:       BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr))' should be detectable
        //:   using 'BSLS_KEYWORD_NOEXCEPT_OPERATOR(function(...))'.
        //:
        //: 3 The 'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)' and
        //:    'BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr)' macros both allow commas
        //:    in template parameter lists.
        //
        // Plan:
        //: 1 Define a function marking it 'noexcept' using the various forms
        //:   of the macro.  Then use
        //:   'BSLS_KEYWORD_NOEXCEPT_OPERATOR(function(...))' to check that the
        //:   function's 'noexcept' specification matches the expected
        //:   specification.
        //
        // NOTE: The test functions are called only to prevent
        //  '-Wunused-function' warning.
        //
        // Testing:
        //   BSLS_KEYWORD_NOEXCEPT
        //   BSLS_KEYWORD_NOEXCEPT_SPECIFICATION
        //   BSLS_KEYWORD_NOEXCEPT_OPERATOR
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING NOEXCEPT SUPPORT"
                            "\n========================\n");
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        const bool hasNoexceptSupport = true;
#else
        const bool hasNoexceptSupport = false;
#endif
        noThrow1();
        const bool isNoThrow1 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(noThrow1());
        ASSERT(isNoThrow1 == hasNoexceptSupport);

        noThrow2();
        const bool isNoThrow2 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(noThrow2());
        ASSERT(isNoThrow2 == hasNoexceptSupport);

        noThrow3();
        const bool isNoThrow3 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(noThrow3());
        ASSERT(isNoThrow3 == hasNoexceptSupport);

        noThrow4();
        const bool isNoThrow4 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(noThrow4());
        ASSERT(isNoThrow4 == hasNoexceptSupport);

        throws1();
        const bool isNoThrow5 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(throws1());
        ASSERT(isNoThrow5 == false);

        throws2();
        const bool isNoThrow6 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(throws2());
        ASSERT(isNoThrow6 == false);

        throws3();
        const bool isNoThrow7 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(throws3());
        ASSERT(isNoThrow7 == false);

        throws4<void, void>();
        const bool isNoThrow8 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                        throws4<void, void>());
        ASSERT(isNoThrow8 == false);

        throws5();
        const bool isNoThrow9 = BSLS_KEYWORD_NOEXCEPT_OPERATOR(throws5());
        ASSERT(isNoThrow9 == false);

        ASSERT(hasNoexceptSupport == BSLS_KEYWORD_NOEXCEPT_AVAILABLE);

        const char *expected1 =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            "noexcept(foo)";
#else
            "false";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_NOEXCEPT_OPERATOR(foo)),
                      expected1) == 0);

        const char *expected2 =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            "true";
#else
            "false";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_NOEXCEPT_AVAILABLE), expected2)
                                                                         == 0);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING BSLS_KEYWORD_FINAL (function)
        //
        // Concerns:
        //: 1 Marking a 'virtual' function as 'final' should compile.
        //:
        //: 2 Trying to override a function marked as 'final' shall fail to
        //:   compile when compiling with C++11 mode.  Since
        //:   'BSLS_KEYWORD_FINAL' is replaced by nothing when compiling with
        //:   C++03 mode the could should compile in this case.
        //
        // Plan:
        //: 1 Define a base class with a 'virtual' function and mark it 'final'
        //:   using 'BSLS_KEYWORD_FINAL' in a derived class.  Creating a
        //:   further derived class which also overrides the function marked as
        //:   'final' should fail compilation when compiling with C++11 mode.
        //
        // Testing:
        //   BSLS_KEYWORD_FINAL (function)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_KEYWORD_FINAL (function)"
                            "\n=====================================\n");

        struct FinalFunctionBase
        {
            virtual int f()
                // Returns a value for each type.
            { return 0; }
        };
        struct FinalFunctionDerived: FinalFunctionBase
        {
            int f()
                // Returns a value for the specific type.
                BSLS_KEYWORD_FINAL
            { return 1; }
        };
        struct FinalFunctionFailure: FinalFunctionDerived
        {
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL) \
 || defined(FAIL_FINAL_FUNCTION)
            int f()
                // Returns a value for the specific type.
            { return 2; }
#endif
        };

        FinalFunctionBase finalFunctionBase;
        ASSERT(finalFunctionBase.f() == 0);

        FinalFunctionDerived finalFunctionDerived;
        ASSERT(finalFunctionDerived.f() == 1);

        FinalFunctionFailure finalFunctionFailure;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
        ASSERT(finalFunctionFailure.f() == 1);
#else
        ASSERT(finalFunctionFailure.f() == 2);
#endif

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
            "final";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_FINAL), expected) == 0);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_KEYWORD_FINAL (class)
        //
        // Concerns:
        //: 1 Marking a class 'final' using 'BSLS_KEYWORD_FINAL' should result
        //:   in a successful compilation.
        //:
        //: 2 Trying to further derive from a function marked as 'final' shall
        //:   fail to compile when compiling with C++11 mode.  Since
        //:   'BSLS_KEYWORD_FINAL' is replaced by nothing when compiling with
        //:   C++03 mode the could should compile in this case.
        //
        // Plan:
        //: 1 Define a class marking it 'final' using 'BSLS_KEYWORD_FINAL'.
        //:   Creating a derived class from the 'final' class should fail
        //:   compilation when compiling with C++11 mode.
        //
        // Testing:
        //   BSLS_KEYWORD_FINAL (class)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: BSLS_KEYWORD_FINAL (class)"
                            "\n===================================\n");

        class FinalClass BSLS_KEYWORD_FINAL
        {
            int d_value;
        public:
            explicit FinalClass(int value = 0)
                // Initialize with the optionally specified 'value'.
                : d_value(value) {}
            int value() const
                // Returns the object's value.
            { return d_value; }
        };
        class FinalClassDerived
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL) || defined(FAIL_FINAL_CLASS)
            : public FinalClass
#endif
        {
            int d_anotherValue;
        public:
            explicit FinalClassDerived(int value)
                // Initialize with the specified 'value'.
                : d_anotherValue(2 * value) {
            }
            int anotherValue() const
                // Returns another value for the object.
            { return d_anotherValue; }
        };

        FinalClass finalValue(1);
        ASSERT(finalValue.value() == 1);
        FinalClassDerived derivedValue(2);
        ASSERT(derivedValue.anotherValue() == 4);

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
            "final";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_FINAL), expected) == 0);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_KEYWORD_EXPLICIT
        //
        // Concerns:
        //: 1 Marking a conversion operator 'explicit' using
        //:   'BSLS_KEYWORD_EXPLICIT' needs to allow explicit conversions.
        //:
        //: 2 Marking a conversion operator 'explicit' using
        //:   'BSLS_KEYWORD_EXPLICIT' should prevent attempts of implicit
        //:   conversion when compiling with C++11 mode.  When compiling with
        //:   C++03 mode compilation will succeed.
        //
        // Plan:
        //: 1 Define a class with an explicit conversion operator and verify
        //:   that explicit and implicit conversions succeed when using C++03
        //:   mode.  When compiling with C++11 mode the implicit conversion
        //:   should fail.
        //
        // Testing:
        //   BSLS_KEYWORD_EXPLICIT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: BSLS_KEYWORD_EXPLICIT"
                            "\n==============================\n");

        struct Explicit
        {
            BSLS_KEYWORD_EXPLICIT
            operator int() const
                // Returns a value for the object.
            { return 3; }
        };

        Explicit explicitObject;

        int explicitResult(explicitObject);
        ASSERT(explicitResult == 3);
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT) \
 || defined(FAIL_EXPLICIT)
        int implicitResult = explicitObject;
        ASSERT(implicitResult == 3);
#endif

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
            "explicit";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_EXPLICIT), expected) == 0);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BSLS_KEYWORD_CONSTEXPR_CPP17
        //
        // Concerns:
        //: 1 Marking a complex function 'constexpr' using
        //:   'BSLS_KEYWORD_CONSTEXPR_CPP17' should result in a successful
        //:   compilation.
        //:
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17' is defined
        //:   then compile code that uses 'BSLS_KEYWORD_CONSTEXPR_CPP17' to
        //:   define constant expression functions.  Invoke the function in a
        //:   compile time constant context ('static_assert').
        //
        // Testing:
        //   BSLS_KEYWORD_CONSTEXPR_CPP17
        // --------------------------------------------------------------------
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
        if (verbose) {
            printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_CPP17 SKIPPED"
                    "\n===========================================\n");
        }

        ASSERT(moreComplexConstexprFunc(true) == 42);
        ASSERT(moreComplexConstexprFunc(false) == 17);
#else
        if (verbose) printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_CPP17"
                            "\n====================================\n");

        static_assert(moreComplexConstexprFunc(true) == 42,
                      "Relaxed (C++17) 'constexpr' is not supported");
        ASSERT(moreComplexConstexprFunc(false) == 17);
#endif

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
            "constexpr";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_CONSTEXPR_CPP17), expected)
            == 0);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BSLS_KEYWORD_CONSTEXPR_CPP14
        //
        // Concerns:
        //: 1 Marking a complex function 'constexpr' using
        //:   'BSLS_KEYWORD_CONSTEXPR_CPP14' should result in a successful
        //:   compilation.
        //:
        //: 2 'BSLS_KEYWORD_CONSTEXPR_CPP14' member functions are not
        //:    implicitly 'const'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined
        //:   then compile code that uses 'BSLS_KEYWORD_CONSTEXPR_CPP14' to
        //:   define relaxed constant expression functions.  Invoke the
        //:   function in a compile time constant context ('static_assert').
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined
        //:   then compile code that uses 'BSLS_KEYWORD_CONSTEXPR_CPP14' to
        //:   define a constexpr member function and detect that it is not a
        //:   const member function.  Use expression SFINAE to detect this
        //:   without invoking a compiler error.
        //
        // Testing:
        //   BSLS_KEYWORD_CONSTEXPR_CPP14
        //   BSLS_KEYWORD_CONSTEXPR_RELAXED
        // --------------------------------------------------------------------
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
        if (verbose) {
            printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_CPP14 SKIPPED"
                    "\n===========================================\n");
        }
        ASSERT(complexConstexprFunc(true) == 42);
        ASSERT(complexConstexprFunc(false) == 17);
#else
        if (verbose) printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_CPP14"
                            "\n======================================\n");

        static_assert(complexConstexprFunc(true) == 42,
                      "Relaxed (C++14) 'constexpr' is not supported");
        ASSERT(complexConstexprFunc(false) == 17);
#endif

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
            "constexpr";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_CONSTEXPR_CPP14), expected)
            == 0);

        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_CONSTEXPR_CPP14),
                      STRINGIFY(BSLS_KEYWORD_CONSTEXPR_RELAXED)) == 0);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BSLS_KEYWORD_CONSTEXPR
        //
        // Concerns:
        //: 1 Marking a function 'constexpr' using 'BSLS_KEYWORD_CONSTEXPR'
        //:   should result in a successful compilation.
        //:
        //: 2 Marking a function 'constexpr' using 'BSLS_KEYWORD_CONSTEXPR'
        //:   should make the test driver not compile if the use of the
        //:   resulting constexpr function is used illegally.
        //
        // Plan:
        //: 1 Define a struct marking its various member functions as
        //:   'constexpr' functions.  Verify that if the 'constexpr' member
        //:   functions are not used appropriately the program will fail to
        //:   compile in C++11 mode.
        //:
        //: 2 Since the correct behavior will case the program to not compile,
        //:   it is rather difficult to create test cases that actually tests
        //:   the feature and still have the test driver pass.  As such, these
        //:   tests must be manually checked to ensure that the program does
        //:   not compile if testStruct is not used correctly.
        //
        // Testing:
        //   BSLS_KEYWORD_CONSTEXPR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_KEYWORD_CONSTEXPR"
                            "\n==============================\n");
        struct testStruct {
            BSLS_KEYWORD_CONSTEXPR testStruct (int i) : d_value(i) { }
            BSLS_KEYWORD_CONSTEXPR operator int() const { return d_value; }
            BSLS_KEYWORD_CONSTEXPR operator long() const { return 1.0; }
          private:
            int d_value;
        };

        BSLS_KEYWORD_CONSTEXPR testStruct B (15);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        BSLS_KEYWORD_CONSTEXPR int X (B);
#else
        int X (B);
#endif
        (void)X;  // unused variable warning

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
            "constexpr";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_CONSTEXPR), expected) == 0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST MACHINERY
        //
        // Concerns:
        //: 1 The 'STRINGIFY' macro creates a string literal out of its
        //:   argument.
        //:
        //: 2 The 'STRINGIFY' macro creates an empty string literal if it has
        //:   no argument.
        //:
        //: 3 The STRINGIFY macro expands macros before stringifying them.
        //
        // Plan:
        //: 1 Create a non-empty string using 'STRINGIFY', verify its content.
        //:
        //: 2 Create an empty string using 'STRINGIFY', verify its content.
        //:
        //: 3 Use a macro as argument to 'STRINGIFY', verify the result.
        //
        // Testing:
        //   Test machinery
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST MACHINERY"
                            "\n======================\n");

        const char s1[] = STRINGIFY(A string literal);
        const char l1[] = "A string literal";
        ASSERTV(strcmp(s1, l1) == 0);

        const char s2[] = STRINGIFY();
        const char l2[] = "";
        ASSERTV(strcmp(s2, l2) == 0);

#define SOME_TEXT A string literal

        const char s3[] = STRINGIFY(SOME_TEXT);
        ASSERTV(strcmp(s3, l1) == 0);

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
// Copyright 2019 Bloomberg Finance L.P.
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
