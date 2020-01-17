// balcl_constraint.t.cpp                                             -*-C++-*-
#include <balcl_constraint.h>

#include <bslim_testutil.h>

#include <bsls_types.h>  // 'bsls::Types::Int64'

#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_ostream.h>     // 'bsl::operator<<'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// ----------------------------------------------------------------------------
// TYPES
// [ 1] struct Constraint
//
// ----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

}  // close unnamed namespace

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::Constraint Constraint;

                         // =========================
                         // functions test*Constraint
                         // =========================

// BDE_VERIFY pragma: -FABC01  // Function ... not in alphabetic order
bool testCharConstraint    (const char *              , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testIntConstraint     (const int *               , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testInt64Constraint   (const bsls::Types::Int64 *, bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testDoubleConstraint  (const double *            , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testStringConstraint  (const bsl::string *       , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testDatetimeConstraint(const bdlt::Datetime *    , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testDateConstraint    (const bdlt::Date *        , bsl::ostream&)
    // Return 'true'.
{
    return true;
}

bool testTimeConstraint    (const bdlt::Time *        , bsl::ostream&)
    // Return 'true'.
{
    return true;
}
    // These functions, never invoked, provide a default value for the
    // 'balcl::Constraint' nested 'typedef's.

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int     test = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::Constraint'
        //
        // Concerns:
        //: 1 This class provides a namespace for a set of useful constraints
        //:   for each of the 'balcl::OptionType' element types.  We need only
        //:   make sure that the types correspond to the expected function
        //:   types.
        //
        // Plan:
        //: 1 Instantiate a constraint for each type, and load a function
        //:   pointer of the appropriate prototype.
        //
        // Testing:
        //   struct Constraint
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::Constraint'" << endl
                          << "===========================" << endl;

        Constraint::CharConstraint         charF(&testCharConstraint);
        Constraint::IntConstraint           intF(&testIntConstraint);
        Constraint::Int64Constraint       int64F(&testInt64Constraint);
        Constraint::DoubleConstraint     doubleF(&testDoubleConstraint);
        Constraint::StringConstraint     stringF(&testStringConstraint);
        Constraint::DatetimeConstraint datetimeF(&testDatetimeConstraint);
        Constraint::DateConstraint         dateF(&testDateConstraint);
        Constraint::TimeConstraint         timeF(&testTimeConstraint);

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
