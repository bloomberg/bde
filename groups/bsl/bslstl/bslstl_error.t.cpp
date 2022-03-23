// bslstl_error.t.cpp                                                 -*-C++-*-
#include <bslstl_error.h>

#include <bsls_buildtarget.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>

#include <bslh_hash.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The component under test defines a systematic way to create sets of error
// categories and error codes and conditions of those categories.  Error
// categories, in addition to serving as tags, are also responsible for
// providing string versions of error values.
//
// ----------------------------------------------------------------------------
//
// TRAITS
// [ 2] is_error_code_enum<TYPE>
// [ 2] is_error_condition_enum<TYPE>
//
// FREE FUNCTIONS
// [ 3] generic_category()
// [ 3] system_category()
// [ 3] make_error_code(errc::errc)
// [ 3] make_error_condition(errc::errc)
// [ 3] hashAppend(HASHALG&, const error_code&)
// [ 3] hashAppend(HASHALG&, const error_condition&)
//
// FREE OPERATORS
// [ 4] bool operator==(const error_code&, const error_code&)
// [ 4] bool operator==(const error_code&, const error_condition&)
// [ 4] bool operator==(const error_condition&, const error_code&)
// [ 4] bool operator==(const error_condition&, const error_condition&)
// [ 4] bool operator!=(const error_code&, const error_code&)
// [ 4] bool operator!=(const error_code&, const error_condition&)
// [ 4] bool operator!=(const error_condition&, const error_code&)
// [ 4] bool operator!=(const error_condition&, const error_condition&)
// [ 4] bool operator<(const error_code&, const error_code&)
// [ 4] bool operator<(const error_condition&, const error_condition&)
// [ 4] std::ostream& operator<<(std::ostream&, const error_code&)
//
// ERROR CATEGORY METHODS
// [ 5] error_category()
// [ 5] ~error_category()
// [ 5] error_condition default_error_condition(int)
// [ 5] bool equivalent(int, const error_condition&) const
// [ 5] bool equivalent(const error_code&, int) const
// [ 5] std::string message(int) const
// [ 5] const char *name() const
// [ 5] bool operator==(const error_category&) const
// [ 5] bool operator!=(const error_category&) const
// [ 5] bool operator<(const error_category&) const
//
// ERROR CODE METHODS
// [ 6] error_code()
// [ 6] error_code(int, const error_category&)
// [ 6] error_code(ERROR_CODE_ENUM)
// [ 6] void assign(int, const error_category&)
// [ 6] error_code& operator=(ERROR_CODE_ENUM)
// [ 6] void clear()
// [ 6] const error_category& category() const
// [ 6] error_condition default_error_condition() const
// [ 6] std::string message() const
// [ 6] int value() const
// [ 6] operator int BloombergLP::bsls::UnspecifiedBool::*()
//
// ERROR CONDITION METHODS
// [ 7] error_condition()
// [ 7] error_condition(int, const error_category&)
// [ 7] error_condition(ERROR_CODE_ENUM)
// [ 7] void assign(int, const error_category&)
// [ 7] error_condition& operator=(ERROR_CODE_ENUM)
// [ 7] void clear()
// [ 7] const error_category& category() const
// [ 7] std::string message() const
// [ 7] int value() const
// [ 7] operator int BloombergLP::bsls::UnspecifiedBool::*()
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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
//            GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {
namespace bsls {
template <>
void BslTestUtil::callDebugprint(const bsl::error_category&  object,
                                 const char                 *leadingString,
                                 const char                 *trailingString)
    // Print a descriptive form of the specified 'category' bracketed by the
    // specified 'leadingString' and 'trailingString'.
{
    printf("%serror_category<%s>%s",
           leadingString,
           object.name(),
           trailingString);
}

template <>
void BslTestUtil::callDebugprint(const bsl::error_code&  object,
                                 const char             *leadingString,
                                 const char             *trailingString)
    // Print a descriptive form of the specified 'code' bracketed by the
    // specified 'leadingString' and 'trailingString'.
{
    printf("%serror_code<%d, '%s', %s>%s",
           leadingString,
           object.value(),
           object.message().c_str(),
           object.category().name(),
           trailingString);
}

template <>
void BslTestUtil::callDebugprint(const bsl::error_condition&  object,
                                 const char                  *leadingString,
                                 const char                  *trailingString)
    // Print a descriptive form of the specified 'condition' bracketed by the
    // specified 'leadingString' and 'trailingString'.
{
    printf("%serror_condition<%d, '%s', %s>%s",
           leadingString,
           object.value(),
           object.message().c_str(),
           object.category().name(),
           trailingString);
}

}  // close namespace bsls

namespace {

// The following classes are used to test that assorted virtual overrides work
// correctly.  They have been moved here from inside 'main()' due to an error
// in the AIX compiler causing test cases to crash - '{DRQS 162760925}'.

struct concrete_error_category : public bsl::error_category
    // This class represents a concrete error category.
{
    // ACCESSORS
    std::string message(int) const BSLS_KEYWORD_OVERRIDE;
        // Unused implementation.

    const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Unused implementation.
};

// ACCESSORS
std::string concrete_error_category::message(int) const
{
    ASSERT(false);
    abort();
}

const char *concrete_error_category::name() const BSLS_KEYWORD_NOEXCEPT
{
    ASSERT(false);
    return 0;
}

struct test_error_category : public concrete_error_category
    // This class overrides virtual methods for testing.
{
    // CLASS DATA
    static bool destructor_called;

    // CREATORS
    ~test_error_category() BSLS_KEYWORD_OVERRIDE;
        // Destroy this object.

    // ACCESSORS
    error_condition default_error_condition(int value) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return the result of invoking the base implementation with the
        // specified 'value' modified.

    bool equivalent(int code, const error_condition&) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return whether the specified 'code' is 3.

    bool equivalent(const error_code&, int condition) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return whether the specified 'condition' is 2.

    std::string message(int value) const BSLS_KEYWORD_OVERRIDE;
        // Return a string describing the specified 'value' using extra
        // annotation.

    const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return a string naming this category.
};

// CLASS DATA
bool test_error_category::destructor_called;

// CREATORS
test_error_category::~test_error_category()
{
    destructor_called = true;
}

// ACCESSORS
error_condition test_error_category::default_error_condition(int value) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return error_category::default_error_condition(value + 1);
}

bool test_error_category::equivalent(int code, const error_condition&) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 3 == code;
}

bool test_error_category::equivalent(const error_code&, int condition) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 2 == condition;
}

std::string test_error_category::message(int value) const
{
    return std::string("M: ") + strerror(value);
}

const char *test_error_category::name() const BSLS_KEYWORD_NOEXCEPT
{
    return "test_error_category";
}

}  // close anonymous namespace
}  // close enterprise namespace

// BDE_VERIFY pragma: -NT01  // close namespace comment depends on macro

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Dedicated Error Category
///- - - - - - - - - - - - - - - - - -
// Suppose we have a dedicated system with a set of possible errors, and we
// want to check if such an error has occurred.  We can use the 'system_error'
// capabilities of the C++ standard for this.
//
// First, open the namespaces for this component.
//..
    // carfx_errors.h
    namespace BloombergLP {
    namespace carfx {
//..
// Then define an 'enum' component to specify the set of error codes for the
// system, and also use the component to define the system error machinery to
// use it properly.  For exposition, all methods will be defined 'inline' so
// that they can appear in sequence.  In practice, they would be defined in
// separate implementation files.  (For the example, many of the methods
// normally present are elided.)
//..
    struct Errors {
        // TYPES
        enum Enum {
            e_WHEELS_CAME_OFF = 1,
            e_ENGINE_FELL_OUT = 2
        };

        // CLASS METHODS
        static const char *toAscii(Enum value);
            // Return a string representing the specified 'value'.

        // ... other enumeration class methods ...
    };

    // CLASS METHODS
    inline
    const char *Errors::toAscii(Enum value) {
        switch (value) {
          case e_WHEELS_CAME_OFF: return "WHEELS_CAME_OFF";           // RETURN
          case e_ENGINE_FELL_OUT: return "ENGINE_FELL_OUT";           // RETURN
          default: return "(* UNKNOWN *)";                            // RETURN
        }
    }
//..
// Next, we define a category class that represents these errors.  This class
// derives from 'bsl::error_category' and overrides the two pure virtual
// methods required for a minimal implementation.  Note that the return type of
// the virtual 'message' method is 'std::string' which allows the same
// implementation to be used both in C++03 and C++11 modes.  We also define the
// 'category' class method that maintains a singleton instance of the category
// class uniquely identifying the error type.
//..
    struct ErrorsCategory : public bsl::error_category {
        // CREATORS
        ErrorsCategory();
            // Create an object of this type.

        // ACCESSORS
        std::string message(int value) const BSLS_KEYWORD_OVERRIDE;
            // Return a string describing the specified 'value'.

        const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
            // Return a string describing this error category.

        // CLASS METHODS
        static const bsl::error_category& category();
            // Return a 'const' reference to the singleton of this category.
    };

    // CREATORS
    inline
    ErrorsCategory::ErrorsCategory()
    {
    }

    // ACCESSORS
    inline
    std::string ErrorsCategory::message(int value) const
    {
        // Note that an out-of-range cast to 'Errors::Enum' is formally
        // undefined behavior, so the range check is necessary.
        return Errors::e_WHEELS_CAME_OFF <= value &&
               value <= Errors::e_ENGINE_FELL_OUT ?
               Errors::toAscii(Errors::Enum(value)) :
               "(* UNKNOWN *)";
    }

    inline
    const char *ErrorsCategory::name() const BSLS_KEYWORD_NOEXCEPT
    {
        return "carfx::Errors";
    }

    // CLASS METHODS
    inline
    const bsl::error_category& ErrorsCategory::category()
    {
        // Note that for C++03, 'BSLMT_ONCE_DO' may be used to protect the
        // static initialization, although the likelihood of a problem is
        // vanishingly small.
        static const ErrorsCategory singleton;
        return singleton;
    }
//..
// Then define namespace-scope functions to return error codes and conditions
// for this error category.  (Normally a set of error values would be
// specified only as one of the two, but both are used here for illustrative
// purposes.)  These functions are at namespace scope because they need to be
// found by argument-dependent lookup of the enumeration value.
//..
    // FREE FUNCTIONS
    inline
    bsl::error_code make_error_code(Errors::Enum value)
        // Return an error code for the specified 'value'.
    {
        return bsl::error_code(static_cast<int>(value),
                               ErrorsCategory::category());
    }

    inline
    bsl::error_condition make_error_condition(Errors::Enum value)
        // Return an error condition for the specified 'value'.
    {
        return bsl::error_condition(static_cast<int>(value),
                                    ErrorsCategory::category());
    }
//..
// Next, close the component namespaces.
//..
    }  // close package namespace
    }  // close enterprise namespace
//..
// Then create specializations for the class templates that tag enumeration
// types as eligible to be treated as error codes or conditions.  These
// specializations must appear in the same namespace in which the class
// templates are defined, and this component provides macros naming these
// namespaces portably.  Again, for exposition, 'Errors::Enum' is marked as
// both an error code and an error condition.
//..
    namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE {
    template <>
    struct is_error_code_enum<BloombergLP::carfx::Errors::Enum>
    : public bsl::true_type { };
    }  // close namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE

    namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE {
    template <>
    struct is_error_condition_enum<BloombergLP::carfx::Errors::Enum>
    : public bsl::true_type { };
    }  // close namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
//..
// Now, write a function that can potentially have errors.
//..
    void drive(bsl::error_code *code, int distance)
        // Drive a car for the specified 'distance' and set the specified
        // 'code' to describe any problems encountered.
    {
        using namespace BloombergLP::carfx;
        if (distance > 1000) {
            *code = make_error_code(Errors::e_ENGINE_FELL_OUT);
        }
        else if (distance > 100) {
            *code = make_error_code(Errors::e_WHEELS_CAME_OFF);
        }
        else {
            *code = make_error_code(Errors::Enum(0));
        }
    }
//..

// BDE_VERIFY pragma: +NT01  // close namespace comment depends on macro

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3; (void)veryVerbose;
    int veryVeryVerbose     = argc > 4; (void)veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, exercise the function and check for errors.
//..
    bsl::error_code code;
    drive(&code, 50);
    ASSERT(!code);
    drive(&code, 500);
    ASSERT(strstr(code.message().c_str(), "WHEELS"));
    drive(&code, 5000);
    ASSERT(strstr(code.message().c_str(), "ENGINE"));
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ERROR CONDITION METHODS
        //   Test the methods of the 'bsl::error_condition' class.
        //
        // Concerns:
        //:  1 The default constructor makes a generic condition with value 0.
        //:  2 The value/category constructor preserves its arguments.
        //:  3 The templated constructor makes a matching condition.
        //:  4 The 'assign' method preserves its arguments.
        //:  5 Templated assignment makes a matching condition.
        //:  6 'clear' makes a generic condition with value 0.
        //:  7 The 'category' method retrieves the correct category.
        //:  8 The 'message' method uses 'strerror' for its result.
        //:  9 The 'value' method retrieves the correct value.
        //: 10 Use in boolean context checks whether the value is non-zero.
        //
        // Plan:
        //:  1 Exercise each method in entirely obvious ways.  (C-1..10)
        //
        // Testing:
        //   error_condition()
        //   error_condition(int, const error_category&)
        //   error_condition(ERROR_CODE_ENUM)
        //   void assign(int, const error_category&)
        //   error_condition& operator=(ERROR_CODE_ENUM)
        //   void clear()
        //   const error_category& category() const
        //   std::string message() const
        //   int value() const
        //   operator int BloombergLP::bsls::UnspecifiedBool::*()
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ERROR CONDITION METHODS"
                   "\n===============================\n");

        if (veryVerbose) {
            printf("error_condition()\n");
        }
        {
            error_condition        mX;
            const error_condition& X = mX;
            ASSERT(&generic_category() == &X.category());
            ASSERT(0 == X.value());
        }

        if (veryVerbose) {
            printf("error_condition(int, const error_category&)\n");
        }
        {
            error_condition        mX(static_cast<int>(errc::no_link),
                                      generic_category());
            const error_condition& X = mX;
            ASSERT(&generic_category() == &X.category());
            ASSERT(ENOLINK == X.value());
        }

        if (veryVerbose) {
            printf("error_condition(ERROR_CODE_ENUM)\n");
        }
        {
            error_condition        mX(carfx::Errors::e_ENGINE_FELL_OUT);
            const error_condition& X = mX;
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("void assign(int, const error_category&)\n");
        }
        {
            error_condition        mX;
            const error_condition& X = mX;
            mX.assign(static_cast<int>(carfx::Errors::e_ENGINE_FELL_OUT),
                      carfx::ErrorsCategory::category());
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("error_condition& operator=(ERROR_CODE_ENUM)\n");
        }
        {
            error_condition        mX;
            const error_condition& X = (mX = carfx::Errors::e_ENGINE_FELL_OUT);
            ASSERT(&mX == &X);
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("void clear()\n");
        }
        {
            error_condition        mX(static_cast<int>(errc::no_link),
                                      system_category());
            const error_condition& X = mX;
            mX.clear();
            ASSERT(&generic_category() == &X.category());
            ASSERT(0 == X.value());
        }

        if (veryVerbose) {
            printf("const error_category& category() const\n");
        }
        {
            error_condition        mX(0, system_category());
            const error_condition& X = mX;
            ASSERT(&system_category() == &X.category());
        }

        if (veryVerbose) {
            printf("std::string message() const\n");
        }
        {
            error_condition        mX(static_cast<int>(errc::no_link),
                                      generic_category());
            const error_condition& X = mX;
#ifdef BSLS_PLATFORM_OS_WINDOWS
            ASSERT("no link" == X.message());
#else
            ASSERT(strerror(ENOLINK) == X.message());
#endif
        }

        if (veryVerbose) {
            printf("int value() const\n");
        }
        {
            error_condition        mX(static_cast<int>(errc::no_link),
                                      generic_category());
            const error_condition& X = mX;
            ASSERT(ENOLINK == X.value());
        }

        if (veryVerbose) {
            printf("operator BoolType() const\n");
        }
        {
            error_condition        mX;
            const error_condition& X = mX;
            ASSERT(!(X));
            if (X) {
                ASSERT(false);
            }
            mX = carfx::Errors::e_ENGINE_FELL_OUT;
            ASSERT(X);
            if (X) {
            }
            else {
                ASSERT(false);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ERROR CODE METHODS
        //   Test the methods of the 'bsl::error_code' class.
        //
        // Concerns:
        //:  1 The default constructor makes a system code with value 0.
        //:  2 The value/category constructor preserves its arguments.
        //:  3 The templated constructor makes a matching code.
        //:  4 The 'assign' method preserves its arguments.
        //:  5 Templated assignment makes a matching code.
        //:  6 'clear' makes a system code with value 0.
        //:  7 The 'category' method retrieves the correct category.
        //:  8 The 'default_error_condition' makes an equivalent condition.
        //:  9 The 'message' method uses 'strerror' for its result.
        //: 10 The 'value' method retrieves the correct value.
        //: 11 Use in boolean context checks whether the value is non-zero.
        //
        // Plan:
        //:  1 Exercise each method in entirely obvious ways.  (C-1..11)
        //
        // Testing:
        //   error_code()
        //   error_code(int, const error_category&)
        //   error_code(ERROR_CODE_ENUM)
        //   void assign(int, const error_category&)
        //   error_code& operator=(ERROR_CODE_ENUM)
        //   void clear()
        //   const error_category& category() const
        //   error_condition default_error_condition() const
        //   std::string message() const
        //   int value() const
        //   operator int BloombergLP::bsls::UnspecifiedBool::*()
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ERROR CODE METHODS"
                   "\n==========================\n");

        if (veryVerbose) {
            printf("error_code()\n");
        }
        {
            error_code        mX;
            const error_code& X = mX;
            ASSERT(&system_category() == &X.category());
            ASSERT(0 == X.value());
        }

        if (veryVerbose) {
            printf("error_code(int, const error_category&)\n");
        }
        {
            error_code        mX(static_cast<int>(errc::no_link),
                                 generic_category());
            const error_code& X = mX;
            ASSERT(&generic_category() == &X.category());
            ASSERT(ENOLINK == X.value());
        }

        if (veryVerbose) {
            printf("error_code(ERROR_CODE_ENUM)\n");
        }
        {
            error_code        mX(carfx::Errors::e_ENGINE_FELL_OUT);
            const error_code& X = mX;
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("void assign(int, const error_category&)\n");
        }
        {
            error_code        mX;
            const error_code& X = mX;
            mX.assign(carfx::Errors::e_ENGINE_FELL_OUT,
                      carfx::ErrorsCategory::category());
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("error_code& operator=(ERROR_CODE_ENUM)\n");
        }
        {
            error_code        mX;
            const error_code& X = (mX = carfx::Errors::e_ENGINE_FELL_OUT);
            ASSERT(&mX == &X);
            ASSERT(&carfx::ErrorsCategory::category() == &X.category());
            ASSERT(carfx::Errors::e_ENGINE_FELL_OUT == X.value());
        }

        if (veryVerbose) {
            printf("void clear()\n");
        }
        {
            error_code        mX(static_cast<int>(errc::no_link),
                                 generic_category());
            const error_code& X = mX;
            mX.clear();
            ASSERT(&system_category() == &X.category());
            ASSERT(0 == X.value());
        }

        if (veryVerbose) {
            printf("const error_category& category() const\n");
        }
        {
            error_code        mX(0, system_category());
            const error_code& X = mX;
            ASSERT(&system_category() == &X.category());
        }

        if (veryVerbose) {
            printf("error_condition default_error_condition() const\n");
        }
#if defined(BSLS_PLATFORM_OS_LINUX) &&                                        \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        // See {DRQS 138124392}.  There appears to be an error in some versions
        // of the Linux system library (not the bslstl implementation) that
        // corrupts the generic category object when
        // 'default_error_condition()' is called, leading to crashes.
        if (0)
#endif
        {
            error_code             mX(static_cast<int>(errc::no_link),
                                      generic_category());
            const error_code&      X  = mX;
            error_condition        mY = X.default_error_condition();
            const error_condition& Y  = mY;
            ASSERT(&Y.category() == &X.category());
            ASSERT(Y.value() == X.value());
        }

        if (veryVerbose) {
            printf("std::string message() const\n");
        }
        {
            error_code        mX(static_cast<int>(errc::no_link),
                                 generic_category());
            const error_code& X = mX;
#ifdef BSLS_PLATFORM_OS_WINDOWS
            ASSERT("no link" == X.message());
#else
            ASSERT(strerror(ENOLINK) == X.message());
#endif
        }

        if (veryVerbose) {
            printf("int value() const\n");
        }
        {
            error_code        mX(static_cast<int>(errc::no_link),
                                 generic_category());
            const error_code& X = mX;
            ASSERT(ENOLINK == X.value());
        }

        if (veryVerbose) {
            printf("operator BoolType() const\n");
        }
        {
            error_code        mX;
            const error_code& X = mX;
            ASSERT(!(X));
            if (X) {
                ASSERT(false);
            }
            mX = carfx::Errors::e_ENGINE_FELL_OUT;
            ASSERT(X);
            if (X) {
            }
            else {
                ASSERT(false);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ERROR CATEGORY METHODS
        //   Test the methods of the 'bsl::error_category' class.
        //
        // Concerns:
        //: 1 This class is abstract, so testing methods can only be done via
        //:   a derived class.
        //:
        //: 2 Verify that this class has a default constructor.
        //:
        //: 3 Verify that this class has a virtual destructor.
        //:
        //: 4 Verify that 'default_error_condition' is virtual and that the
        //:   default implementation creates an error condition using the
        //:   object as category.
        //:
        //: 5 Verify that the 'equivalent' methods are virtual and that the
        //:   default implementations match values correctly.
        //:
        //: 6 Verify that the 'message' method is virtual and that the default
        //:   implementation uses 'strerror' to translate an error value.
        //:
        //: 7 Verify that the 'name' method is virtual and that there is a
        //:   default implementation.
        //:
        //: 8 Verify that comparison operations form a total order on category
        //:   objects.
        //
        // Plan:
        //: 1 Create a concrete derived class that throws from its abstract
        //:   method overrides, to serve as a base for further testing.  (C-1)
        //:
        //: 2 Have no user-defined constructors in the concrete class to show
        //:   that the base-class default constructor is invoked.  (C-2)
        //:
        //: 3 Create a derived class that sets a variable in its destructor,
        //:   allocate an object of that class, delete it via a pointer to
        //:   'error_category', and observe that the variable is set.  (C-3)
        //:
        //: 4 Create a derived class that invokes 'default_error_condition' of
        //:   'error_category' with a modified parameter, and observe that the
        //:   result has the same category as the invoking object and the
        //:   modified value.  (C-4)
        //:
        //: 5 Create error code and condition objects with the concrete class
        //:   as category and observe that the equivalence values are correct.
        //:   Then define a derived class with different rules for equivalence
        //:   and observe that those rules are followed when invoked from base
        //:   references.  (C-5)
        //:
        //: 6 Create a derived class with a 'message' method that invokes the
        //:   'error_category' method and prepends extra text.  Observe that
        //:   the result via invocation from a base reference contains both of
        //:   the expected texts.  (C-6)
        //:
        //: 7 Create a derived class that overrides the 'name' method and
        //:   observe the value returned when invoked from a base class
        //:   reference.  (C-7)
        //:
        //: 8 Create an array of two of the concrete objects, initialize base
        //:   references to the two objects, and verify that the comparisons
        //:   return values consistent with the array ordering.  (C-8)
        //
        // Testing:
        //   error_category()
        //   ~error_category()
        //   error_condition default_error_condition(int)
        //   bool equivalent(int, const error_condition&) const
        //   bool equivalent(const error_code&, int) const
        //   std::string message(int) const
        //   const char *name() const
        //   bool operator==(const error_category&) const
        //   bool operator!=(const error_category&) const
        //   bool operator<(const error_category&) const
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ERROR CATEGORY METHODS"
                   "\n==============================\n");


        if (veryVerbose) {
            printf("default constructor\n");
        }
        {
            concrete_error_category mX;
        }

        if (veryVerbose) {
            printf("virtual destructor\n");
        }
        {
            test_error_category::destructor_called = false;
            {
                test_error_category *pmX = new test_error_category;
                error_category      *pX = pmX;
                test_error_category::destructor_called = false;
                delete pX;
            }
            ASSERT(test_error_category::destructor_called);
        }

        if (veryVerbose) {
            printf("default_error_condition\n");
        }
        {
            test_error_category mX;
            error_category&     X = mX;
            ASSERT(3 == X.default_error_condition(2).value());
            ASSERT(&mX == &X.default_error_condition(2).category());
        }

        if (veryVerbose) {
            printf("equivalent\n");
        }
        {
            {
                if (veryVerbose) {
                    printf("\tbase implementation\n");
                }

                if (veryVerbose) {
                    printf("\t\tsame categories\n");
                }
                concrete_error_category mX;
                error_category&         X = mX;
                ASSERT(!X.equivalent(2, X.default_error_condition(3)));
                ASSERT( X.equivalent(2, X.default_error_condition(2)));
                ASSERT(!X.equivalent(error_code(2, X), 3));
                ASSERT( X.equivalent(error_code(2, X), 2));

                if (veryVerbose) {
                    printf("\t\tdifferent categories\n");
                }
                concrete_error_category mY;
                error_category&         Y = mY;
                ASSERT(!X.equivalent(2, Y.default_error_condition(2)));
                ASSERT(!X.equivalent(error_code(2, Y), 2));
            }
            {
                if (veryVerbose) {
                    printf("\toverridden implementation\n");
                }

                if (veryVerbose) {
                    printf("\t\tsame categories\n");
                }
                test_error_category mX;
                error_category&     X = mX;
                ASSERT(!X.equivalent(2, X.default_error_condition(3)));
                ASSERT( X.equivalent(3, X.default_error_condition(2)));
                ASSERT(!X.equivalent(error_code(2, X), 3));
                ASSERT( X.equivalent(error_code(2, X), 2));

                if (veryVerbose) {
                    printf("\t\tdifferent categories\n");
                }
                concrete_error_category mY;
                error_category&         Y = mY;
                ASSERT(X.equivalent(3, Y.default_error_condition(2)));
                ASSERT(X.equivalent(error_code(2, Y), 2));
            }
        }

        if (veryVerbose) {
            printf("message\n");
        }
        {
            test_error_category mX;
            error_category&     X = mX;
            ASSERT(strstr(X.message(static_cast<int>(errc::no_link)).data(),
                          "M: "));
            ASSERT(strstr(X.message(static_cast<int>(errc::no_link)).data(),
                          strerror(ENOLINK)));
        }

        if (veryVerbose) {
            printf("name\n");
        }
        {
            test_error_category mX;
            error_category&     X = mX;
            ASSERT(0 == strcmp("test_error_category", X.name()));
        }

        if (veryVerbose) {
            printf("comparison operators\n");
        }
        {
            concrete_error_category mX[2];
            error_category&         X1 = mX[0];
            error_category&         X2 = mX[1];

            ASSERT(X1 == X1);
            ASSERT(X2 == X2);
            ASSERT(!(X1 == X2));
            ASSERT(!(X2 == X1));

            ASSERT(!(X1 != X1));
            ASSERT(!(X2 != X2));
            ASSERT(X1 != X2);
            ASSERT(X2 != X1);

            ASSERT(!(X1 < X1));
            ASSERT(!(X2 < X2));
            ASSERT(X1 < X2);
            ASSERT(!(X2 < X1));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        //   Test the several comparison operators of this component.
        //
        // Concerns:
        //: 1 Pairs of error codes are equal if and only if their values and
        //:   categories match.
        //:
        //: 2 Pairs of error conditions are equal if and only if their values
        //:   and categories match.
        //:
        //: 3 An error code and error condition are equal if and only if their
        //:   values and categories match.
        //:
        //: 4 Pairs of error codes sort lexicographically by category then
        //:   value.
        //:
        //: 5 Pairs of error conditions sort lexicographically by category then
        //:   value.
        //
        // Plan:
        //: 1 Create four error codes with combinations of two values and two
        //:   categories and verify the equality operations among them.  (C-1)
        //:
        //: 2 Create four error conditions with combinations of two values and
        //:   two categories and verify the equality operations among them.
        //:   (C-2)
        //:
        //: 3 Perform cross comparisons between the error codes and conditions
        //:   from the above steps.  (C-3)
        //:
        //: 4 Verify that the ordering among codes matches the ordering among
        //:   categories and values.  (C-4)
        //:
        //: 5 Verify that the ordering among conditions matches the ordering
        //:   among categories and values.  (C-5)
        //
        // Testing:
        //   bool operator==(const error_code&, const error_code&)
        //   bool operator==(const error_code&, const error_condition&)
        //   bool operator==(const error_condition&, const error_code&)
        //   bool operator==(const error_condition&, const error_condition&)
        //   bool operator!=(const error_code&, const error_code&)
        //   bool operator!=(const error_code&, const error_condition&)
        //   bool operator!=(const error_condition&, const error_code&)
        //   bool operator!=(const error_condition&, const error_condition&)
        //   bool operator<(const error_code&, const error_code&)
        //   bool operator<(const error_condition&, const error_condition&)
        //   std::ostream& operator<<(std::ostream&, const error_code&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING FREE COMPARISON OPERATORS"
                   "\n=================================\n");

        const int k_no_link   = static_cast<int>(errc::no_link);
        const int k_timed_out = static_cast<int>(errc::timed_out);

        const bsl::error_code      codes[4] = {
            bsl::error_code(k_no_link,   bsl::generic_category()),
            bsl::error_code(k_timed_out, bsl::generic_category()),
            bsl::error_code(k_no_link,   bsl::system_category()),
            bsl::error_code(k_timed_out, bsl::system_category())
        };
        const bsl::error_condition conditions[4] = {
            bsl::error_condition(k_no_link,   bsl::generic_category()),
            bsl::error_condition(k_timed_out, bsl::generic_category()),
            bsl::error_condition(k_no_link,   bsl::system_category()),
            bsl::error_condition(k_timed_out, bsl::system_category())
        };

        if (veryVerbose) {
            printf("Comparing error codes\n");
        }
        for (int i = 0; i < 4; ++i) {
            const bsl::error_code &ci = codes[i];
            if (veryVeryVerbose) {
                printf("%d %s %d\n",
                       i, ci.category().name(), ci.value());
            }
            for (int j = 0; j < 4; ++j) {
                const bsl::error_code &cj = codes[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d\n",
                           j, cj.category().name(), cj.value());
                }
                ASSERT((i == j) == (ci == cj));
                ASSERT((i != j) == (ci != cj));
                if (ci.category() < cj.category()) {
                    ASSERT(ci < cj);
                }
                else if (cj.category() < ci.category()) {
                    ASSERT(cj < ci);
                }
                else {
                    ASSERT((ci.value() < cj.value()) == (ci < cj));
                }
            }
        }

        if (veryVerbose) {
            printf("Comparing error conditions\n");
        }
        for (int i = 0; i < 4; ++i) {
            const bsl::error_condition &ci = conditions[i];
            if (veryVeryVerbose) {
                printf("%d %s %d\n",
                       i, ci.category().name(), ci.value());
            }
            for (int j = 0; j < 4; ++j) {
                const bsl::error_condition &cj = conditions[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d\n",
                           j, cj.category().name(), cj.value());
                }
                ASSERT((i == j) == (ci == cj));
                ASSERT((i != j) == (ci != cj));
                if (ci.category() < cj.category()) {
                    ASSERT(ci < cj);
                }
                else if (cj.category() < ci.category()) {
                    ASSERT(cj < ci);
                }
                else {
                    ASSERT((ci.value() < cj.value()) == (ci < cj));
                }
            }
        }

        if (veryVerbose) {
            printf("Comparing error codes and conditions\n");
        }
        for (int i = 0; i < 4; ++i) {
            const bsl::error_code &ci = codes[i];
            for (int j = 0; j < 4; ++j) {
                const bsl::error_condition &cj = conditions[j];
                if (veryVeryVerbose) {
                    printf("%d code %7s %3d "
                           "%d condition %7s %3d "
                           "ci==cj:%d ci!=cj:%d cj==ci:%d cj!=ci:%d\n",
                           i, ci.category().name(), ci.value(),
                           j, cj.category().name(), cj.value(),
                           ci == cj, ci != cj, cj == ci, cj != ci);
                }

                // if you're comparing an error_code and an error_condition,
                // and they have different categories, you're into system-
                // specific behavior.  On Windows, the system_category remaps
                // error numbers to match the values that Windows uses.  This
                // happens inside 'operator==', where a temporary
                // error_condition is created from the error_code, and then
                // *that* is compared to the other error_condition.  The
                // generic category doesn't do any such remapping.  Since we
                // are only testing values from 'system_category' and
                // 'generic_category' here, it is sufficient to check if the
                // categories differ.
                if (ci.category() != cj.category()) {
                    if (veryVeryVerbose) {
                        printf("\tskipping system-dependent comparison\n");
                    }
                    continue;
                }
                bool equal = ci.value() == cj.value();
                ASSERTV(ci, cj, ci == cj, equal == (ci == cj));
                ASSERTV(ci, cj, cj == ci, equal == (cj == ci));
                ASSERTV(ci, cj, ci != cj, !equal == (ci != cj));
                ASSERTV(ci, cj, cj != ci, !equal == (cj != ci));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS
        //   Test the several free functions of this component.
        //
        // Concerns:
        //: 1 generic_category() returns the same unique object each call
        //: 2 system_category() returns the same unique object each call
        //: 3 make_error_code returns a generic error code
        //: 4 make_error_condition returns a generic error condition
        //: 5 an error_code can be hashed
        //: 6 an error_condition can be hashed
        //
        // Plan:
        //: 1 Verify that several calls to generic_category() return the same
        //:   object.  (C-1)
        //:
        //: 2 Verify that several calls to system_category() return the same
        //:   object, distinct from generic_category().  (C-2)
        //:
        //: 3 Verify that a code created by make_error_code contains the value
        //:   with which it was constructed, and is of generic category.  (C-3)
        //:
        //: 4 Verify that a condition created by make_error_condition contains
        //:   the value with which it was constructed, and is of generic
        //:   category.  (C-4)
        //:
        //: 5 Verify that a code can be hashed, and that codes with different
        //:   values or categories hash to different values.  (C-5)
        //:
        //: 6 Verify that a condition can be hashed, and that codes with
        //:   different values or categories hash to different values.  (C-6)
        //
        // Testing:
        //   generic_category()
        //   system_category()
        //   make_error_code(errc::errc)
        //   make_error_condition(errc::errc)
        //   hashAppend(HASHALG&, const error_code&)
        //   hashAppend(HASHALG&, const error_condition&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING FREE FUNCTIONS"
                   "\n======================\n");

        if (veryVerbose) {
            printf("generic_category() is always the same\n");
        }
        const bsl::error_category *g1 = &generic_category();
        const bsl::error_category *g2 = &generic_category();
        const bsl::error_category *g3 = &generic_category();
        ASSERT(g1 == g2);
        ASSERT(g2 == g3);

        if (veryVerbose) {
            printf("system_category() is always the same\n");
        }
        const bsl::error_category *s1 = &system_category();
        const bsl::error_category *s2 = &system_category();
        const bsl::error_category *s3 = &system_category();
        ASSERT(s1 == s2);
        ASSERT(s2 == s3);

        if (veryVerbose) {
            printf("generic_category() and system_category() are distinct\n");
        }
        ASSERT(g1 != s1);

        if (veryVerbose) {
            printf("make_error_code\n");
        }
        bsl::error_code code = bsl::make_error_code(bsl::errc::timed_out);
        ASSERT(static_cast<int>(bsl::errc::timed_out) == code.value());
        ASSERT(g1 == &code.category());

        if (veryVerbose) {
            printf("make_error_condition\n");
        }
        bsl::error_condition condition =
            bsl::make_error_condition(bsl::errc::no_link);
        ASSERT(static_cast<int>(bsl::errc::no_link) == condition.value());
        ASSERT(g1 == &condition.category());

        if (veryVerbose) {
            printf("hashing error codes\n");
        }
        const bsl::error_code codes[4] = {
            bsl::error_code(static_cast<int>(bsl::errc::no_link),
                            generic_category()),
            bsl::error_code(static_cast<int>(bsl::errc::timed_out),
                            generic_category()),
            bsl::error_code(static_cast<int>(bsl::errc::no_link),
                            system_category()),
            bsl::error_code(static_cast<int>(bsl::errc::timed_out),
                            system_category()),
        };
        for (int i = 0; i < 4; ++i) {
            std::hash<bsl::error_code> hasher;
            const bsl::error_code&     ci = codes[i];
            if (veryVeryVerbose) {
                printf("%d %s %d %zu\n",
                       i, ci.category().name(), ci.value(), hasher(ci));
            }
            for (int j = i; j < 4; ++j) {
                const bsl::error_code &cj = codes[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d %zu\n",
                           j, cj.category().name(), cj.value(), hasher(cj));
                }
#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
                // macOS/Windows implementations appear to not include the category.
                bool equal = ci.value() == cj.value();
#else
                bool equal = i == j;
#endif
                ASSERTV(equal, equal == (hasher(ci) == hasher(cj)));
            }
        }

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // The '<system_error>' header does not specialize 'std::hash' for
        // 'std::error_condition' until C++17.
        if (veryVerbose) {
            printf("cannot test hashing error conditions\n");
        }
#else
        if (veryVerbose) {
            printf("hashing error conditions\n");
        }
        const bsl::error_condition conditions[4] = {
            bsl::error_condition(static_cast<int>(bsl::errc::no_link),
                                 generic_category()),
            bsl::error_condition(static_cast<int>(bsl::errc::timed_out),
                                 generic_category()),
            bsl::error_condition(static_cast<int>(bsl::errc::no_link),
                                 system_category()),
            bsl::error_condition(static_cast<int>(bsl::errc::timed_out),
                                 system_category()),
        };
        for (int i = 0; i < 4; ++i) {
            bsl::hash<bsl::error_code> hasher;
            const bsl::error_code&     ci = codes[i];
            if (veryVeryVerbose) {
                printf("%d %s %d %zu\n",
                       i, ci.category().name(), ci.value(), hasher(ci));
            }
            for (int j = i; j < 4; ++j) {
                const bsl::error_code &cj = codes[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d %zu\n",
                           j, cj.category().name(), cj.value(), hasher(cj));
                }
#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
                // Windows implementation appears to not include the category.
                bool equal = ci.value() == cj.value();
#else
                bool equal = i == j;
#endif
                ASSERT(equal == (hasher(ci) == hasher(cj)));
            }
        }
        for (int i = 0; i < 4; ++i) {
            bsl::hash<bsl::error_condition> hasher;
            const bsl::error_condition&     ci = conditions[i];
            if (veryVeryVerbose) {
                printf("%d %s %d %zu\n",
                       i, ci.category().name(), ci.value(), hasher(ci));
            }
            for (int j = i; j < 4; ++j) {
                const bsl::error_condition &cj = conditions[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d %zu\n",
                           j, cj.category().name(), cj.value(), hasher(cj));
                }
#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
                // Windows implementation appears to not include the category.
                bool equal = ci.value() == cj.value();
#else
                bool equal = i == j;
#endif
                ASSERT(equal == (hasher(ci) == hasher(cj)));
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //   Verify that the code identification traits are set correctly.
        //
        // Concerns:
        //: 1 bsl::is_error_code_enum<bsl::errc::errc>::value is false
        //: 2 bsl::is_error_condition_enum<bsl::errc::errc>::value is true
        //: 3 bsl::is_error_code_enum<other>::value is false
        //: 4 bsl::is_error_condition_enum<other>::value is false
        //
        // Plan:
        //: 1 Verify the trait value for each concern.  (C-1..4)
        //
        // Testing:
        //   is_error_code_enum<TYPE>
        //   is_error_condition_enum<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        if (veryVerbose) {
            P(bsl::is_error_code_enum<bsl::ErrcEnum>::value);
            P(bsl::is_error_condition_enum<bsl::ErrcEnum>::value);
            P(bsl::is_error_code_enum<char>::value);
            P(bsl::is_error_condition_enum<char>::value);
        }
        ASSERT(!bsl::is_error_code_enum<bsl::ErrcEnum>::value);
        ASSERT(bsl::is_error_condition_enum<bsl::ErrcEnum>::value);
        ASSERT(!bsl::is_error_code_enum<char>::value);
        ASSERT(!bsl::is_error_condition_enum<char>::value);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create four test objects by using the default, initializing, and
        //:   copy constructors.
        //:
        //: 2 Exercise the basic value-semantic methods and the equality
        //:   operators using these test objects.
        //:
        //: 3 Invoke the primary manipulator, copy constructor, and assignment
        //:   operator without and with aliasing.
        //:
        //: 4 Use the basic accessors to verify the expected results.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 1. Create an object x1. to VA).\n");
        const error_category &C1 = system_category();
        if (verbose) { T_ P(C1) }

        error_code        mD1(static_cast<int>(errc::io_error), C1);
        const error_code& D1 = mD1;
        if (verbose) { T_ P(D1) }

        error_condition        mN1(static_cast<int>(errc::no_link), C1);
        const error_condition& N1 = mN1;
        if (verbose) { T_ P(N1) }

        if (verbose) printf("\ta. Check initial state of x1.\n");
        ASSERT(0 == strcmp("system", C1.name()));
        ASSERT(EIO == D1.value());
        ASSERT(&C1 == &D1.category());
        ASSERT(ENOLINK == N1.value());
        ASSERT(&C1 == &N1.category());

        if (verbose) printf("\tb. Equality operators: x1 <op> x1.\n");
        ASSERT(C1 == C1);
        ASSERT(!(C1 != C1));
        ASSERT(D1 == D1);
        ASSERT(!(D1 != D1));
        ASSERT(N1 == N1);
        ASSERT(!(N1 != N1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 2. Create an object x2 (copy from x1).\n");
        error_code        mD2(D1);
        const error_code& D2 = mD2;
        if (verbose) { T_ P(D2) }

        error_condition        mN2(N1);
        const error_condition& N2 = mN2;
        if (verbose) { T_ P(N2) }

        if (verbose) printf("\ta. Check the initial state of x2.\n");
        ASSERT(EIO == D2.value());
        ASSERT(&C1 == &D2.category());
        ASSERT(ENOLINK == N2.value());
        ASSERT(&C1 == &N2.category());

        if (verbose) printf("\tb. Equality operators: x2 <op> x1, x2.\n");
        ASSERT(D2 == D1);
        ASSERT(D2 == D2);
        ASSERT(!(D2 != D1));
        ASSERT(!(D2 != D2));
        ASSERT(N2 == N1);
        ASSERT(N2 == N2);
        ASSERT(!(N2 != N1));
        ASSERT(!(N2 != N2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 3. Set x1 to a new value.\n");

        const error_category &C2 = generic_category();
        if (verbose) { T_ P(C2) }

        mD1.assign(static_cast<int>(errc::timed_out), C2);
        if (verbose) { T_ P(D1) }

        mN1.assign(static_cast<int>(errc::text_file_busy), C2);
        if (verbose) { T_ P(N1) }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(ETIMEDOUT == D1.value());
        ASSERT(&C2 == &D1.category());
        ASSERT(ETXTBSY == N1.value());
        ASSERT(&C2 == &N1.category());

        if (verbose) printf("\tb. Equality operators: x1 <op> x1, x2.\n");
        ASSERT(C1 != C2);
        ASSERT(!(C1 == C2));
        ASSERT(D1 == D1);
        ASSERT(!(D1 != D1));
        ASSERT(D1 != D2);
        ASSERT(!(D1 == D2));
        ASSERT(N1 == N1);
        ASSERT(!(N1 != N1));
        ASSERT(N1 != N2);
        ASSERT(!(N1 == N2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 4. Create a default object x3().\n");

        error_code        mD3;
        const error_code& D3 = mD3;
        if (verbose) { T_ P(D3); }

        error_condition        mN3;
        const error_condition& N3 = mN3;
        if (verbose) { T_ P(N3); }

        if (verbose) printf("\ta. Check initial state of x3.\n");
        ASSERT(0 == D3.value());
        ASSERT(&C1 == &D3.category());
        ASSERT(0 == N3.value());
        ASSERT(&C2 == &N3.category());

        if (verbose) printf("\tb. Equality operators: x3 <op> x1, x2, x3.\n");
        ASSERT(!(D3 == D1));
        ASSERT(!(D3 == D2));
        ASSERT(D3 == D3);
        ASSERT(D3 != D1);
        ASSERT(D3 != D2);
        ASSERT(!(D3 != D3));
        ASSERT(!(N3 == N1));
        ASSERT(!(N3 == N2));
        ASSERT(N3 == N3);
        ASSERT(N3 != N1);
        ASSERT(N3 != N2);
        ASSERT(!(N3 != N3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 5. Create an object x4 (copy from x3).\n");

        error_code        mD4(D3);
        const error_code& D4 = mD4;
        if (verbose) { T_ P(D4) }

        error_condition        mN4(N3);
        const error_condition& N4 = mN4;
        if (verbose) { T_ P(N4) }

        if (verbose) printf("\ta. Check initial state of x4.\n");
        ASSERT(0 == D4.value());
        ASSERT(&C1 == &D4.category());
        ASSERT(0 == N4.value());
        ASSERT(&C2 == &N4.category());

        if (verbose)
            printf("\tb. Equality operators: x4 <op> x1, x2, x3, x4.\n");
        ASSERT(!(D4 == D1));
        ASSERT(!(D4 == D2));
        ASSERT(D4 == D3);
        ASSERT(D4 == D4);
        ASSERT(D4 != D1);
        ASSERT(D4 != D2);
        ASSERT(!(D4 != D4));
        ASSERT(!(D4 != D4));
        ASSERT(!(N4 == N1));
        ASSERT(!(N4 == N2));
        ASSERT(N4 == N3);
        ASSERT(N4 == N4);
        ASSERT(N4 != N1);
        ASSERT(N4 != N2);
        ASSERT(!(N4 != N3));
        ASSERT(!(N4 != N4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 6. Set x3 to a new value VC.\n");

        mD3.assign(static_cast<int>(errc::file_exists), C1);
        if (verbose) { T_ P(D3) }

        mN3.assign(static_cast<int>(errc::not_a_directory), C1);
        if (verbose) { T_ P(N3) }

        if (verbose) printf("\ta. Check new state of x3.\n");
        ASSERT(EEXIST == D3.value());
        ASSERT(&C1 == &D3.category());
        ASSERT(ENOTDIR == N3.value());
        ASSERT(&C1 == &N3.category());

        if (verbose)
            printf("\tb. Equality operators: x4 <op> x1, x2, x3, x4.\n");
        ASSERT(!(D3 == D1));
        ASSERT(!(D3 == D2));
        ASSERT(D3 == D3);
        ASSERT(!(D3 == D4));
        ASSERT(D3 != D1);
        ASSERT(D3 != D2);
        ASSERT(!(D3 != D3));
        ASSERT(D3 != D4);
        ASSERT(!(N3 == N1));
        ASSERT(!(N3 == N2));
        ASSERT(N3 == N3);
        ASSERT(!(N3 == N4));
        ASSERT(N3 != N1);
        ASSERT(N3 != N2);
        ASSERT(!(N3 != N3));
        ASSERT(N3 != N4);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 7. Assign x2 = x1.\n");

        mD2 = D1;
        if (verbose) { T_ P(D2) }

        mN2 = N1;
        if (verbose) { T_ P(N2) }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(ETIMEDOUT == D2.value());
        ASSERT(&C2 == &D2.category());
        ASSERT(ETXTBSY == N2.value());
        ASSERT(&C2 == &N2.category());

        if (verbose)
            printf("\tb. Equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(D2 == D1);
        ASSERT(D2 == D2);
        ASSERT(!(D2 == D3));
        ASSERT(!(D2 == D4));
        ASSERT(!(D2 != D1));
        ASSERT(!(D2 != D2));
        ASSERT(D2 != D3);
        ASSERT(D2 != D4);
        ASSERT(N2 == N1);
        ASSERT(N2 == N2);
        ASSERT(!(N2 == N3));
        ASSERT(!(N2 == N4));
        ASSERT(!(N2 != N1));
        ASSERT(!(N2 != N2));
        ASSERT(N2 != N3);
        ASSERT(N2 != N4);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 8. Assign x2 = x3.\n");

        mD2 = D3;
        if (verbose) { T_ P(D2) }

        mN2 = N3;
        if (verbose) { T_ P(N2) }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(EEXIST == D2.value());
        ASSERT(&C1 == &D2.category());
        ASSERT(ENOTDIR == N2.value());
        ASSERT(&C1 == &N2.category());

        if (verbose)
            printf("\tb. Equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(!(D2 == D1));
        ASSERT(D2 == D2);
        ASSERT(D2 == D3);
        ASSERT(!(D2 == D4));
        ASSERT(D2 != D1);
        ASSERT(!(D2 != D2));
        ASSERT(!(D2 != D3));
        ASSERT(D2 != D4);
        ASSERT(!(N2 == N1));
        ASSERT(N2 == N2);
        ASSERT(N2 == N3);
        ASSERT(!(N2 == N4));
        ASSERT(N2 != N1);
        ASSERT(!(N2 != N2));
        ASSERT(!(N2 != N3));
        ASSERT(N2 != N4);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 9. Assign x1 = x1 (aliasing).\n");

        mD1 = D1;
        if (verbose) { T_ P(D1) }

        mN1 = N1;
        if (verbose) { T_ P(N1) }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(ETIMEDOUT == D1.value());
        ASSERT(&C2 == &D1.category());
        ASSERT(ETXTBSY == N1.value());
        ASSERT(&C2 == &N1.category());

        if (verbose)
            printf("\tb. Equality operators: x1 <op> x1, x2, x3, x4.\n");
        ASSERT(!(D2 == D1));
        ASSERT(D2 == D2);
        ASSERT(D2 == D3);
        ASSERT(!(D2 == D4));
        ASSERT(D2 != D1);
        ASSERT(!(D2 != D2));
        ASSERT(!(D2 != D3));
        ASSERT(D2 != D4);
        ASSERT(!(N2 == N1));
        ASSERT(N2 == N2);
        ASSERT(N2 == N3);
        ASSERT(!(N2 == N4));
        ASSERT(N2 != N1);
        ASSERT(!(N2 != N2));
        ASSERT(!(N2 != N3));
        ASSERT(N2 != N4);
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
