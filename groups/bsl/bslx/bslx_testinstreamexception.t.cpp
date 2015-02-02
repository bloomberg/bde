// bslx_testinstreamexception.t.cpp                                   -*-C++-*-

#include <bslx_testinstreamexception.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a simple exception object that contains an attribute
// initialized by its constructor.  We exercise both the constructor and the
// accessor for the attribute by creating objects initialized with varying
// values, and ensure that the accessor returns the expected values.
// ----------------------------------------------------------------------------
// [1] TestInStreamException(typeCode);
// [1] ~TestInStreamException();
// [1] TypeCode::Enum dataType() const;
// ----------------------------------------------------------------------------
// [2] USAGE TEST - Make sure main usage example compiles and works.
// ----------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

#ifdef BDE_BUILD_TARGET_EXC
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:  Throw and Catch
///- - - - - - - - - - - - - -
// In the following example, the 'bslx::TestInStreamException' object is
// thrown, caught, and examined.   First, set up a 'try' block and throw the
// exception with the indication that the exception occurred during
// unexternalization of an 'unsigned int':
//..
    try {
        throw TestInStreamException(bslx::TypeCode::e_UINT32);
//..
// Then, catch the exception and verify the indicated cause of the exception:
//..
    } catch (const bslx::TestInStreamException& e) {
        ASSERT(bslx::TypeCode::e_UINT32 == e.dataType());
    }
//..
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Ensure the constructor correctly stores the type enumeration value
        //   and that the basic accessor returns the value.
        //
        // Concerns:
        //: 1 The constructor stores the value.
        //:
        //: 2 The basic accessor returns the same value.
        //
        // Plan:
        //: 1 Create 'TestInStreamException' objects with varying values.
        //:
        //: 2 Verify that the basic accessor returns this value.  (C-1..2)
        //
        // Testing:
        //   TestInStreamException(typeCode);
        //   ~TestInStreamException();
        //   TypeCode::Enum dataType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing ctor and accessor." << endl;
        {
            typedef TypeCode TC;
            const TC::Enum DATA[] = {
                TC::e_INT8,     TC::e_UINT8,
                TC::e_INT16,    TC::e_UINT16,
                TC::e_INT24,    TC::e_UINT24,
                TC::e_INT32,    TC::e_UINT32,
                TC::e_INT40,    TC::e_UINT40,
                TC::e_INT48,    TC::e_UINT48,
                TC::e_INT56,    TC::e_UINT56,
                TC::e_INT64,    TC::e_UINT64,
                TC::e_FLOAT32,  TC::e_FLOAT64,
                TC::e_INVALID,  (TC::Enum)0, (TC::Enum)100
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int i = 0; i < NUM_DATA; ++i) {
                const TestInStreamException X(DATA[i]);
                LOOP_ASSERT(i, DATA[i] == X.dataType());
            }
        }
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
// Copyright 2014 Bloomberg Finance L.P.
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
