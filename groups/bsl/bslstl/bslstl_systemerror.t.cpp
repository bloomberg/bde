// bslstl_systemerror.t.cpp                                           -*-C++-*-
#include <bslstl_systemerror.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

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
// [  ] error_code()
// [  ] error_code(int, const error_category&)
// [  ] error_code(ERROR_CODE_ENUM)
// [  ] void assign(int, const error_category&)
// [  ] error_code& operator=(ERROR_CODE_ENUM)
// [  ] void clear()
// [  ] const error_category& category() const
// [  ] error_condition default_error_condition() const
// [  ] std::string message() const
// [  ] int value() const
// [  ] operator BoolType() const
//
// ERROR CONDITION METHODS
// [  ] error_condition()
// [  ] error_condition(int, const error_category&)
// [  ] error_condition(ERROR_CODE_ENUM)
// [  ] void assign(int, const error_category&)
// [  ] error_condition& operator=(ERROR_CODE_ENUM)
// [  ] void clear()
// [  ] const error_category& category() const
// [  ] std::string message() const
// [  ] int value() const
// [  ] operator BoolType() const
//
// SYSTEM ERROR METHODS
// [  ] system_error(error_code, const std::string&)
// [  ] system_error(error_code, const char *)
// [  ] system_error(error_code)
// [  ] system_error(int, const error_category&, const std::string&)
// [  ] system_error(int, const error_category&, const char *)
// [  ] system_error(int, const error_category&)
// [  ] const error_code& code() const
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

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
// want to be able to throw descriptive exceptions when an error occurs.  We
// can use the 'system_error' capabilities of the C++ standard for this.
//
// First, we define the set of error codes for our system.
//..
   namespace car_errc {
   enum car_errc {
       car_wheels_came_off = 1,
       car_engine_fell_out = 2
   };
   }  // close namespace car_errc
//..
// Then, we enable the trait marking this as an error code.
//..
   namespace bsl {
   template <>
   struct is_error_code_enum<car_errc::car_errc> : public true_type {
   };
   }  // close namespace bsl
//..
// Next, we create an error category that will give us descriptive messages.
//..
   namespace {
   struct car_category_impl : public bsl::error_category {
       // ACCESSORS
       std::string message(int value) const;
           // Return a string describing the specified 'value'.

       const char *name() const;
           // Return a string describing this error category.
   };

    // ACCESSORS
    std::string car_category_impl::message(int value) const {
        switch (value) {
          case car_errc::car_wheels_came_off: return "The wheels came off";
          case car_errc::car_engine_fell_out: return "The engine fell out";
          default:                            return "Some car problem";
        }
    }

    const char *car_category_impl::name() const {
        return "car_category";
    }
    }  // close unnamed namespace
//..
// Then, we define functions to get our unique category object, and to make
// error codes and error conditions from our enumeration values.
//..
    const error_category& car_category()
        // Return a 'const' reference to the unique car category object.
    {
        static car_category_impl car_category_object;
        return car_category_object;
    }

    bsl::error_code make_error_code(car_errc::car_errc value)
        // Return a car category error code of the specified 'value'.
    {
        return bsl::error_code(static_cast<int>(value), car_category());
    }

    bsl::error_condition make_error_condition(car_errc::car_errc value)
        // Return a car category error condition of the specified 'value'.
    {
        return bsl::error_condition(static_cast<int>(value), car_category());
    }
//..
// Now, we define an exception class for exceptions of our category.
//..
    class car_error : public std::runtime_error {
      public:
        // CREATORS
        car_error(car_errc::car_errc value);                        // IMPLICIT
        car_error(car_errc::car_errc value, const std::string& what);
            // Create an object of this type holding the specified 'value'.
            // Optionally specify 'what' as extra annotation.

        // ACCESSORS
        const error_code& code() const;
            // Return a 'const' reference to the error code of this object.

      private:
        bsl::error_code d_code;  // error code
    };

    // CREATORS
    car_error::car_error(car_errc::car_errc value)
    : std::runtime_error(car_category().message(value))
    , d_code(make_error_code(value))
    {
    }

    car_error::car_error(car_errc::car_errc value, const std::string& what)
    : std::runtime_error(what + ": " + car_category().message(value))
    , d_code(make_error_code(value))
    {
    }

    // ACCESSORS
    const bsl::error_code& car_error::code() const
    {
        return d_code;
    }
//..

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
      case 99: {
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

// Finally, we can throw, catch, and examine these exceptions.
//..
    try {
        throw car_error(car_errc::car_engine_fell_out, "testing car_errc");
    }
    catch (const std::runtime_error& e) {
        if (verbose) {
            P(e.what());
        }
        ASSERT(strstr(e.what(), "testing car_errc"));
        ASSERT(strstr(e.what(), "The engine fell out"));
        try {
            throw;
        }
        catch (const car_error& e) {
            if (verbose) {
                P_(e.code().category().name()) P(e.code().value())
            }
            ASSERT(car_errc::car_engine_fell_out == e.code().value());
            ASSERT(car_category() == e.code().category());
        }
    }
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ERROR CATEGORY METHODS
        //   Test the metthods of the 'bsl::error_caegory' class.
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
        //: 3 Create a derived class that sets a valraible in its destructor,
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
        //:   observe the valu ereturned when invoked from a base class
        //:   reference.  (C-7)
        //:
        //: 8 Create an array of two of the concrete objects, initialize base
        //:   references to the two objects, and verify that the comparisons
        //:   return values consistent with the array ordering.  (C-8)
        //
        // Testing:
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
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ERROR CATEGORY METHODS"
                   "\n==============================\n");

        struct concrete_error_category : public error_category {
            std::string message(int) const { throw 0; }
            const char *name() const { throw 0; }
        };

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
            static bool destructor_called;
            {
                struct test_error_category : concrete_error_category {
                    ~test_error_category() { destructor_called = true; }
                };
                test_error_category *pmX = new test_error_category;
                error_category *pX = pmX;
                destructor_called = false;
                delete pX;
            }
            ASSERT(destructor_called);
        }

        if (veryVerbose) {
            printf("default_error_condition\n");
        }
        {
            struct test_error_category : concrete_error_category {
                error_condition default_error_condition(int value) const {
                    return error_category::default_error_condition(value + 1);
                }
            };
            test_error_category mX;
            error_category &X = mX;
            ASSERT(3 == X.default_error_condition(2).value());
            ASSERT(&mX == &X.default_error_condition(2).category());
        }

        if (veryVerbose) {
            printf("equivalent\n");
        }
        {
            struct test_error_category : concrete_error_category {
                bool equivalent(int code, const error_condition&) const {
                    return code == 3;
                }
                bool equivalent(const error_code&, int condition) const {
                    return condition == 2;
                }
            };
            {
                if (veryVerbose) {
                    printf("\tbase implementation\n");
                }

                if (veryVerbose) {
                    printf("\t\tsame categories\n");
                }
                concrete_error_category mX;
                error_category &X = mX;
                ASSERT(!X.equivalent(2, X.default_error_condition(3)));
                ASSERT( X.equivalent(2, X.default_error_condition(2)));
                ASSERT(!X.equivalent(error_code(2, X), 3));
                ASSERT( X.equivalent(error_code(2, X), 2));

                if (veryVerbose) {
                    printf("\t\tdifferent categories\n");
                }
                concrete_error_category mY;
                error_category &Y = mY;
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
                error_category &X = mX;
                ASSERT(!X.equivalent(2, X.default_error_condition(3)));
                ASSERT( X.equivalent(3, X.default_error_condition(2)));
                ASSERT(!X.equivalent(error_code(2, X), 3));
                ASSERT( X.equivalent(error_code(2, X), 2));

                if (veryVerbose) {
                    printf("\t\tdifferent categories\n");
                }
                concrete_error_category mY;
                error_category &Y = mY;
                ASSERT(X.equivalent(3, Y.default_error_condition(2)));
                ASSERT(X.equivalent(error_code(2, Y), 2));
            }
        }

        if (veryVerbose) {
            printf("message\n");
        }
        {
            struct test_error_category : concrete_error_category {
                std::string message(int value) const {
                    return "M: " + error_category::message(value);
                }
            };
            test_error_category mX;
            error_category &X = mX;
            ASSERT(strstr(X.message(errc::no_link).data(), "M: "));
            ASSERT(strstr(X.message(errc::no_link).data(), strerror(ENOLINK)));
        }

        if (veryVerbose) {
            printf("name\n");
        }
        {
            struct test_error_category : concrete_error_category {
                const char *name() const {
                    return "test_error_category";
                }
            };
            test_error_category mX;
            error_category &X = mX;
            ASSERT(0 == strcmp("test_error_category", X.name()));
        }

        if (veryVerbose) {
            printf("comparison operators\n");
        }
        {
            concrete_error_category mX[2];
            error_category &X1 = mX[0];
            error_category &X2 = mX[1];

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
        //: 2 Pairs of error condtions are equal if and only if their values
        //:   and categories match.
        //:
        //: 3 An error code and error condition are equal if and only if their
        //:   values and categories match.
        //:
        //: 4 Pairs of error codes sort lexicographically by category then
        //:   value.
        //:
        //: 5 Pairs of error condtions sort lexicographically by category then
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

        const bsl::error_code codes[4] = {
            bsl::error_code(bsl::errc::no_link, generic_category()),
            bsl::error_code(bsl::errc::timed_out, generic_category()),
            bsl::error_code(bsl::errc::no_link, system_category()),
            bsl::error_code(bsl::errc::timed_out, system_category()),
        };
        const bsl::error_condition conditions[4] = {
            bsl::error_condition(bsl::errc::no_link, generic_category()),
            bsl::error_condition(bsl::errc::timed_out, generic_category()),
            bsl::error_condition(bsl::errc::no_link, system_category()),
            bsl::error_condition(bsl::errc::timed_out, system_category()),
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
                ASSERT((j == i) == (cj == ci));
                ASSERT((i != j) == (ci != cj));
                ASSERT((j != i) == (cj != ci));
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
        //:   onject, distinct from generic_category().  (C-2)
        //:
        //: 3 Verify that a code created by make_error_code contains the value
        //:   with which it was constructed, and is of generic category.  (C-3)
        //:
        //: 4 Verify that a condition created by make_error_code contains the
        //:   value with which it was constructed, and is of generic category.
        //:   (C-4)
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
        ASSERT(bsl::errc::timed_out == code.value());
        ASSERT(g1 == &code.category());

        if (veryVerbose) {
            printf("make_error_condition\n");
        }
        bsl::error_condition condition =
            bsl::make_error_condition(bsl::errc::no_link);
        ASSERT(bsl::errc::no_link == condition.value());
        ASSERT(g1 == &condition.category());

        BloombergLP::bslh::Hash<> hasher;

        if (veryVerbose) {
            printf("hashing error codes\n");
        }
        const bsl::error_code codes[4] = {
            bsl::error_code(bsl::errc::no_link, generic_category()),
            bsl::error_code(bsl::errc::timed_out, generic_category()),
            bsl::error_code(bsl::errc::no_link, system_category()),
            bsl::error_code(bsl::errc::timed_out, system_category()),
        };
        for (int i = 0; i < 4; ++i) {
            const bsl::error_code &ci = codes[i];
            if (veryVeryVerbose) {
                printf("%d %s %d %u\n",
                       i, ci.category().name(), ci.value(), hasher(ci));
            }
            for (int j = i; j < 4; ++j) {
                const bsl::error_code &cj = codes[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d %u\n",
                           j, cj.category().name(), cj.value(), hasher(cj));
                }
                ASSERT((i == j) == (hasher(ci) == hasher(cj)));
            }
        }

        if (veryVerbose) {
            printf("hashing error conditions\n");
        }
        const bsl::error_condition conditions[4] = {
            bsl::error_condition(bsl::errc::no_link, generic_category()),
            bsl::error_condition(bsl::errc::timed_out, generic_category()),
            bsl::error_condition(bsl::errc::no_link, system_category()),
            bsl::error_condition(bsl::errc::timed_out, system_category()),
        };
        for (int i = 0; i < 4; ++i) {
            const bsl::error_condition &ci = conditions[i];
            if (veryVeryVerbose) {
                printf("%d %s %d %u\n",
                       i, ci.category().name(), ci.value(), hasher(ci));
            }
            for (int j = i; j < 4; ++j) {
                const bsl::error_condition &cj = conditions[j];
                if (veryVeryVerbose) {
                    printf("\t%d %s %d %u\n",
                           j, cj.category().name(), cj.value(), hasher(cj));
                }
                ASSERT((i == j) ==
                       (hasher(conditions[i]) == hasher(conditions[j])));
            }
        }
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
            P(bsl::is_error_code_enum<bsl::errc::errc>::value);
            P(bsl::is_error_condition_enum<bsl::errc::errc>::value);
            P(bsl::is_error_code_enum<char>::value);
            P(bsl::is_error_condition_enum<char>::value);
        }
        ASSERT(!bsl::is_error_code_enum<bsl::errc::errc>::value);
        ASSERT(bsl::is_error_condition_enum<bsl::errc::errc>::value);
        ASSERT(!bsl::is_error_code_enum<char>::value);
        ASSERT(!bsl::is_error_condition_enum<char>::value);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   We want to exercise basic
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        system_error s(errc::file_exists, system_category());
        system_error g(errc::file_exists, generic_category());
        system_error sw(errc::file_exists, system_category(), "sw");
        system_error gw(errc::file_exists, generic_category(), "gw");
        P(s.what());
        P(g.what());
        P(sw.what());
        P(gw.what());
        ASSERT(true);
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
