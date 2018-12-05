// bsls_keyword.t.cpp                                                 -*-C++-*-

#include <bsls_keyword.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
// [ 9] BSLS_KEYWORD_DELETED
// [ 4] BSLS_KEYWORD_EXPLICIT
// [ 5] BSLS_KEYWORD_FINAL (class)
// [ 6] BSLS_KEYWORD_FINAL (function)
// [ 7] BSLS_KEYWORD_NOEXCEPT
// [ 7] BSLS_KEYWORD_NOEXCEPT_SPECIFICATION
// [ 7] BSLS_KEYWORD_NOEXCEPT_OPERATOR
// [ 8] BSLS_KEYWORD_OVERRIDE
//-----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
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
    template <class TYPE>
    class Optional
    {
        TYPE *d_value_p;
    public:
        Optional(): d_value_p() {}
            // Create an empty 'Optional'.
        explicit Optional(const TYPE& value): d_value_p(new TYPE(value)) {}
            // Sets this object to the specified 'value'.
        ~Optional() { delete d_value_p; }
            // Destroy this object.

        // ...

        BSLS_KEYWORD_EXPLICIT
        operator bool() const { return d_value_p; }
            // Return 'true' if this object has a 'value'; and return 'false'
            // otherwise.
    };

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
}  // close unnamed namespace

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
// Thefore we have a separate macro 'BSLS_KEYWORD_CONSTEXPR_RELAXED' that can
// be used to mark functions 'constexpr' when compiling with C++14 mode:
//..
BSLS_KEYWORD_CONSTEXPR_RELAXED
int complexConstexprFunc(bool b)
{
    if (b) {
        return 42;                                                    // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);


    switch (test) { case 0:
      case 10: {
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
            int f()
                // Returns a value associated with the type.
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE) \
 || defined(FAIL_USAGE_OVERRIDE)
                BSLS_KEYWORD_OVERRIDE
#endif
            { return 2; }
        };

        OverrideBase overrideBase;
        ASSERT(overrideBase.f() == 0);
        OverrideSuccess overrideSuccess;
        ASSERT(overrideSuccess.f() == 1);
        ASSERT(static_cast<const OverrideBase&>(overrideSuccess).f() == 1);
        OverrideFailure overrideFailure;
        ASSERT(overrideFailure.f() == 2);
        ASSERT(static_cast<const OverrideBase&>(overrideFailure).f() == 0);

///Example 2: Creating an extended 'constexpr' function
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// See before 'main' for 'complexConstexprFunc'.
//..
// When compiling with C++14 'constexpr' support it is possible to use the
// result of 'complexConstexprFunc' in compile-time constants:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED
   constexpr
#endif
   int result = complexConstexprFunc(true);
   ASSERT(42 == result);
//..
      } break;
      case 9: {
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
      case 8: {
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
            int f()
                // Returns a value specific to this type.
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE) || defined(FAIL_OVERRIDE)
                BSLS_KEYWORD_OVERRIDE
#endif
            {
                return 2;
            }
        };

        OverrideOK ok;
        ASSERT(ok.f() == 1);
        ASSERT(static_cast<const Base&>(ok).f() == 1);
        OverrideFail fail;
        ASSERT(fail.f() == 2);
        ASSERT(static_cast<const Base&>(fail).f() == 0);

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE)
            "override";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_OVERRIDE), expected) == 0);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING NOEXPCEPT SUPPORT
        //
        // Concerns:
        //: 1 Marking a function 'noexcept' using 'BSLS_KEYWORD_NOEXCEPT' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
        //:       BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr))' should result in a
        //:   successful compilation in C++03 mode.
        //:
        //: 2 Marking a function 'noexcept' or 'noexcept(bool)' using
        //:   'BSLS_KEYWORD_NOEXCEPT' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)' or
        //:   'BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
        //:       BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr))' should be detectable
        //:   using 'BSLS_KEYWORD_NOEXCEPT_OPERATOR(function(...))`.
        //:
        //: 3 The `BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(pred)` and
        //:    'BSLS_KEYWORD_NOEXCEPT_OPERATOR(expr)' macros both allow commas
        //:    in template parameter lists.
        //
        // Plan:
        //: 1 Define a function marking it 'noexcept' using the various forms
        //:   of the macro. Then use
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
        if (verbose) printf("\nTESTING NOEXPCEPT SUPPORT"
                            "\n=========================\n");
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
      case 6: {
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
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_KEYWORD_EXPLICIT
        //
        // Concerns:
        //: 1 Marking a conversion operator 'explicit' using
        //:   'BSLS_KEYWORD_EXPLICIT' needs to allow explicit conversions.
        //:
        //: 2 Marking a conversion operator 'explicit' using
        //:   'BSLS_KEYWORD_EXPLICIT' should prevent attempts of implicit
        //:   conversion when compiling with C++11 mode. When compiling with
        //:   C++03 mode compilation will succeed.
        //
        // Plan:
        //: 1 Define a class with an explicit conversion operator and
        //:   verify that explicit and implicit conversions succeed when using
        //:   C++03 mode. When compiling with C++11 mode the implicit
        //:   conversion should fail.
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
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BSLS_KEYWORD_CONSTEXPR_RELAXED
        //
        // Concerns:
        //: 1 Marking a complex function 'constexpr' using
        //:   'BSLS_KEYWORD_CONSTEXPR_RELAXED' should result in a successful
        //:   compilation.
        //:
        //: 2 'BSLS_KEYWORD_CONSTEXPR_RELAXED' member functions are not
        //:    implicitly 'const'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined
        //:   then compile code that uses 'BSLS_KEYWORD_CONSTEXPR_RELAXED' to
        //:   define relaxed constant expression functions.  Invoke the
        //:   function in a compile time constant context ('static_assert').
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined
        //:   then compile code that uses 'BSLS_KEYWORD_CONSTEXPR_RELAXED' to
        //:   define a constexpr member function and detect that it is not a
        //:   const member function.  Use expression SFINAE to detect this
        //:   without invoking a compiler error.
        //
        // Testing:
        //   BSLS_KEYWORD_CONSTEXPR_RELAXED
        // --------------------------------------------------------------------
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED)
        if (verbose) {
            printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_RELAXED SKIPPED"
                    "\n==============================================\n");
        }
#else
        if (verbose) printf("\nTESTING BSLS_KEYWORD_CONSTEXPR_RELAXED"
                            "\n======================================\n");

        static_assert(complexConstexprFunc(true) == 42,
                      "Relaxed (C++14) 'constexpr' is not supported");
#endif

        const char *expected =
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED)
            "constexpr";
#else
            "";
#endif
        ASSERT(strcmp(STRINGIFY(BSLS_KEYWORD_CONSTEXPR_RELAXED), expected)
            == 0);
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
        //:   the feature and still have the test driver pass. As such, these
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
        //: 2 The STRINGIFY macro expands macros before stringifying them.
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
