// bslstl_systemerror.t.cpp                                           -*-C++-*-
#include <bslstl_systemerror.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The component under test defines
//
// ============================================================================
//
// CREATORS:
//
/// MANIPULATORS:
//
// ACCESSORS:
//
// FREE OPERATORS:
//-----------------------------------------------------------------------------
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
      case 2: {
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
