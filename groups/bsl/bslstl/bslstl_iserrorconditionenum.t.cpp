// bslstl_iserrorconditionenum.t.cpp                                  -*-C++-*-
#include <bslstl_iserrorconditionenum.h>

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
// The component under test defines a type trait for use in marking an
// enumeration type as eligible to participate in the '<system_error>' facility
// and a macro designating the namespace in which that trait should be
// specialized.
//
// ----------------------------------------------------------------------------
//
// TRAITS
// [ 1] is_error_condition_enum<TYPE>::value
// [ 1] BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
// ----------------------------------------------------------------------------
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

//=============================================================================
//            GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace ErrorValues {
enum Eligible         { e_eligible          = 1 };
enum Ineligible       { e_ineligible        = 2 };
enum MarkedIneligible { e_marked_ineligible = 3 };
}  // close namespace ErrorValues

// BDE_VERIFY pragma: -NT01  // namespace comment depends on macro

#ifdef BSL_OVERRIDES_STD
#undef std
#endif
namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE {
template <>
struct is_error_condition_enum<ErrorValues::Eligible> : bsl::true_type
    // Specialize trait to mark 'Eligible' as an error condition.
{
};

template struct is_error_condition_enum<ErrorValues::Ineligible>;
    // Use the default trait to see whether 'Ineligible' is an error condition.

template <>
struct is_error_condition_enum<ErrorValues::MarkedIneligible> : bsl::false_type
    // Specialize trait to mark 'MarkedIneligible' as not an error condition.
{
};
}  // close namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
#ifdef BSL_OVERRIDES_STD
#define std bsl
#endif

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Dedicated Error Values
///- - - - - - - - - - - - - - - - -
// Suppose we have a dedicated system with a set of possible errors, and we
// want to be able to throw descriptive exceptions when an error occurs.  We
// need to work with the '<system_error>' facility to support this, starting by
// marking the enumeration type that defines the error literals as eligible to
// participate.  We can use 'bsl::is_error_condition_enum' to do this.
//
// First, we define the set of error values for our system.
//..
    struct CarError {
        // TYPES
        enum Enum {
            car_wheels_came_off = 1,
            car_engine_fell_out = 2
        };
    };
//..
// Then, we enable the trait marking this as an error condition.  (Note that if
// this code is intended to be used with 'BSL_OVERRIDES_STD' and C++11, it is
// necessary to bracket it as shown, or it will not compile.)
//..
    #ifdef BSL_OVERRIDES_STD
    #undef std
    #endif
    namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE {
    template <>
    struct is_error_condition_enum<CarError::Enum>
    : public bsl::true_type { };
    }  // close namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
    #ifdef BSL_OVERRIDES_STD
    #define std bsl
    #endif
//..

// BDE_VERIFY pragma: +NT01  // namespace comment depends on macro

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

// Finally, we verify that the trait marks our type as eligible.
//..
    ASSERT(is_error_condition_enum<CarError::Enum>::value);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'is_error_condition_enum<TYPE>'
        //   Verify that the condition identification traits are set correctly.
        //
        // Concerns:
        //: 1 A type marked as a code reports that it is.
        //: 2 By default types do not report as codes.
        //: 3 A type marked as not a code reports that it is not.
        //: 4 Specializing in BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE works.
        //
        // Plan:
        //: 1 Create an enum with the trait specialized as true and test that
        //:   the trait reports as true.  (C-1,4)
        //:
        //: 2 Create an enum without specializing the trait and test that the
        //:   trait reports as false.  (C-2)
        //:
        //: 3 Create an enum with the trait specialized as false and test that
        //:   the trait reports as false.  (C-3,4)
        //
        // Testing:
        //   is_error_condition_enum<TYPE>::value
        //   BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'is_error_condition_enum<TYPE>'"
                            "\n=======================================\n");

        using namespace ErrorValues;
#ifdef BSL_OVERRIDES_STD
#undef std
#endif
        using namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE;
#ifdef BSL_OVERRIDES_STD
#define std bsl
#endif

        if (veryVerbose) {
            P(is_error_condition_enum<Eligible>::value)
            P(is_error_condition_enum<Ineligible>::value)
            P(is_error_condition_enum<MarkedIneligible>::value)
        }

        ASSERT(is_error_condition_enum<Eligible>::value);
        ASSERT(!is_error_condition_enum<Ineligible>::value);
        ASSERT(!is_error_condition_enum<MarkedIneligible>::value);
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
