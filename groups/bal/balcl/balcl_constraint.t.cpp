// balcl_constraint.t.cpp                                             -*-C++-*-
#include <balcl_constraint.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h>    // 'bsl::is_same'

#include <bsls_types.h>      // 'bsls::Types::Int64'

#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_ostream.h>     // 'bsl::operator<<'
#include <bsl_sstream.h>     // 'bsl::ostringstream'
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This utility 'struct' provides convenient type aliases for each of the
// supported constraints (functors).  These are tested by showing that each
// alias has the expected type (a compile-time test), and that functors of each
// of those types can be created.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] struct Constraint

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::Constraint Obj;

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

                         // =========================
                         // functions test*Constraint
                         // =========================

// BDE_VERIFY pragma: -FABC01  // Function ... not in alphabetic order
bool testCharConstraint    (const char *              , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testIntConstraint     (const int *               , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testInt64Constraint   (const bsls::Types::Int64 *, bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testDoubleConstraint  (const double *            , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testStringConstraint  (const bsl::string *       , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testDatetimeConstraint(const bdlt::Datetime *    , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testDateConstraint    (const bdlt::Date *        , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}

bool testTimeConstraint    (const bdlt::Time *        , bsl::ostream& oss)
    // Return 'false' and set an error message to the specified 'oss'.
{
    oss << "error" << flush;
    return false;
}
    // These functions, never invoked, provide default values for creating each
    // of the 'balcl::Constraint' nested 'typedef's.

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? atoi(argv[1]) : 0;
    const bool     verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::Constraint'
        //
        // Concerns:
        //: 1 Each alias represents the intended type.
        //:
        //: 2 Functors for each alias can be created and work as expected.
        //
        // Plan:
        //: 1 Use 'bsl::is_same' to compare each type alias with its expected
        //:   type.  (C-1)
        //:
        //: 2 Create a functor of each type using the degenerate constraint
        //:   functions defined in this test driver.  Invoke each and confirm
        //:   that results match expectations.  (C-2)
        //
        //:   o Pass the address of a 'const'-qualified variable to confirm
        //:     that first parameter of each signature used 'const'.
        //
        //:   o Return an error message from each functor to confirm that the
        //:     second parameter of each signature did *not* use 'const'.
        //
        // Testing:
        //   struct Constraint
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::Constraint'" << endl
                          << "===========================" << endl;

        if (veryVerbose) cout
                             << endl
                             << "Confirm that each alias has the expected type"
                             << endl;
//v-----^
  ASSERT((
  bsl::is_same<bsl::function<bool(const char               *, bsl::ostream&)>,
               Obj::CharConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const int                *, bsl::ostream&)>,
               Obj::IntConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const bsls::Types::Int64 *, bsl::ostream&)>,
               Obj::Int64Constraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const double             *, bsl::ostream&)>,
               Obj::DoubleConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const bsl::string        *, bsl::ostream&)>,
               Obj::StringConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const bdlt::Datetime     *, bsl::ostream&)>,
               Obj::DatetimeConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const bdlt::Date         *, bsl::ostream&)>,
               Obj::DateConstraint>::value));
  ASSERT((
  bsl::is_same<bsl::function<bool(const bdlt::Time         *, bsl::ostream&)>,
               Obj::TimeConstraint>::value));
//^-----v

        if (veryVerbose) cout << endl
                              << "Construct a functor of each type alias."
                              << endl;

        Obj::CharConstraint         charF(&testCharConstraint);
        Obj::IntConstraint           intF(&testIntConstraint);
        Obj::Int64Constraint       int64F(&testInt64Constraint);
        Obj::DoubleConstraint     doubleF(&testDoubleConstraint);
        Obj::StringConstraint     stringF(&testStringConstraint);
        Obj::DatetimeConstraint datetimeF(&testDatetimeConstraint);
        Obj::DateConstraint         dateF(&testDateConstraint);
        Obj::TimeConstraint         timeF(&testTimeConstraint);

        if (veryVerbose) cout << endl
                              << "Exercise a functor of each type alias."
                              << endl;

        const char                   charV = 'a';
        const int                     intV =   0;
        const bsls::Types::Int64    int64V =  0L;
        const double               doubleV = 0.0;
        const bsl::string          stringV;
        const bdlt::Datetime     datetimeV;
        const bdlt::Date             dateV;
        const bdlt::Time             timeV;

        bsl::ostringstream     charOss;
        bsl::ostringstream      intOss;
        bsl::ostringstream    int64Oss;
        bsl::ostringstream   doubleOss;
        bsl::ostringstream   stringOss;
        bsl::ostringstream datetimeOss;
        bsl::ostringstream     dateOss;
        bsl::ostringstream     timeOss;

        ASSERT(!    charF(    &charV,     charOss));
        ASSERT(!     intF(     &intV,      intOss));
        ASSERT(!   int64F(   &int64V,    int64Oss));
        ASSERT(!  doubleF(  &doubleV,   doubleOss));
        ASSERT(!  stringF(  &stringV,   stringOss));
        ASSERT(!datetimeF(&datetimeV, datetimeOss));
        ASSERT(!    dateF(    &dateV,     dateOss));
        ASSERT(!    timeF(    &timeV,     timeOss));

        ASSERT("error" ==     charOss.str());
        ASSERT("error" ==      intOss.str());
        ASSERT("error" ==    int64Oss.str());
        ASSERT("error" ==   doubleOss.str());
        ASSERT("error" ==   stringOss.str());
        ASSERT("error" == datetimeOss.str());
        ASSERT("error" ==     dateOss.str());
        ASSERT("error" ==     timeOss.str());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
