// bdlat_attributeinfo.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_attributeinfo.h>

#include <bslim_testutil.h>

#include <bsls_compilerfeatures.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 2]
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlat_AttributeInfo Obj;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TBD
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mMI = { 2, "foo", 3, "desc", 0 };  const Obj& MI = mMI;

        ASSERT(2 == MI.id());
        ASSERT(0 == bsl::strcmp(MI.name(), "foo"));
        ASSERT(3 == MI.nameLength());
        ASSERT(0 == bsl::strcmp(MI.annotation(), "desc"));
        ASSERT(1 == (mMI == mMI));
        ASSERT(0 == (mMI != mMI));

        mMI.id()             = 22;
        mMI.name()           = "name";
        mMI.nameLength()     = 4;
        mMI.annotation()     = "csed";

        ASSERT(22 == MI.id());
        ASSERT(0  == bsl::strcmp(MI.name(), "name"));
        ASSERT(4  == MI.nameLength());
        ASSERT(0  == bsl::strcmp(MI.annotation(), "csed"));

        Obj mMI2 = { 2, "foo", 3, "desc", 0 };  const Obj& MI2 = mMI2;

        ASSERT(0 == (mMI == mMI2));
        ASSERT(1 == (mMI != mMI2));

        mMI = MI2;

        ASSERT(1 == (mMI == mMI2));
        ASSERT(0 == (mMI != mMI2));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        {
            constexpr Obj MI3 = { 1, "foo", 3, "desc", 0 };

            constexpr int         id             = MI3.id();
            constexpr const char *name           = MI3.name();
            constexpr int         nameLength     = MI3.nameLength();
            constexpr const char *annotation     = MI3.annotation();
            constexpr int         formattingMode = MI3.formattingMode();

            ASSERT(1 == id                             );
            ASSERT(0 == bsl::strcmp("foo",  name      ));
            ASSERT(3 == nameLength                     );
            ASSERT(0 == bsl::strcmp("desc", annotation));
            ASSERT(0 == formattingMode                 );

            (void) id;               // suppress compiler warning
            (void) name;             // suppress compiler warning
            (void) nameLength;       // suppress compiler warning
            (void) annotation;       // suppress compiler warning
            (void) formattingMode ;  // suppress compiler warning
        }
#endif

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
// Copyright 2015 Bloomberg Finance L.P.
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
