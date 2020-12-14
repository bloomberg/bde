// bdlc_flathashtable_groupcontrol.t.cpp                              -*-C++-*-
#include <bdlc_flathashtable_groupcontrol.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a mechanism,
// 'bdlc::FlatHashTable_GroupControl'.  No allocator is involved.  Testing
// concerns are (safely) limited to the mechanical functioning of the various
// methods.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] FlatHashTable_GroupControl(const bsl::uint8_t *data);
// [ 2] ~FlatHashTable_GroupControl();
//
// ACCESSORS
// [ 3] bsl::uint32_t available() const;
// [ 3] bsl::uint32_t inUse() const;
// [ 3] bsl::uint32_t match(bsl::uint8_t value) const;
// [ 3] bool neverFull() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlc::FlatHashTable_GroupControl Obj;

const bsl::uint8_t EE = Obj::k_EMPTY;
const bsl::uint8_t XX = Obj::k_ERASED;
const bsl::uint8_t VA = 0x00;
const bsl::uint8_t VB = 0x7F;
const bsl::uint8_t VC = 0x01;
const bsl::uint8_t VD = 0x10;
const bsl::uint8_t VE = 0x11;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void print(const bsl::uint8_t *data)
    // Print a representation of the specified 'data'.
{
    const char *hex = "0123456789abcdef";

    bsl::cout << "   ";
    for (bsl::size_t i = 0; i < Obj::k_SIZE; ++i) {
        bsl::cout << ' '
                  << hex[static_cast<bsl::uint32_t>((data[i] >> 4) & 0xF)]
                  << hex[static_cast<bsl::uint32_t>(data[i] & 0xF)];
    }
    bsl::cout << bsl::endl;
}

void verifyWithOracle(const bsl::uint8_t *data)
    // Verify the accessor methods of a 'bdlc::FlatHashTable_GroupControl'
    // return the expected values for the specified 'data'.
{
    Obj mX(data);  const Obj& X = mX;

    bsl::uint32_t expAvailable = 0;
    bsl::uint32_t expInUse     = 0;
    bsl::uint32_t expMatchA    = 0;
    bsl::uint32_t expMatchB    = 0;
    bsl::uint32_t expMatchC    = 0;
    bsl::uint32_t expMatchD    = 0;
    bsl::uint32_t expMatchE    = 0;
    bool          expNeverFull = false;
    for (int i = Obj::k_SIZE - 1; i >= 0; --i) {
        expAvailable = expAvailable * 2 + (data[i] >= 0x80u ? 1 : 0);
        expInUse     = expInUse * 2     + (data[i] <  0x80u ? 1 : 0);
        expMatchA    = expMatchA * 2    + (data[i] == VA    ? 1 : 0);
        expMatchB    = expMatchB * 2    + (data[i] == VB    ? 1 : 0);
        expMatchC    = expMatchC * 2    + (data[i] == VC    ? 1 : 0);
        expMatchD    = expMatchD * 2    + (data[i] == VD    ? 1 : 0);
        expMatchE    = expMatchE * 2    + (data[i] == VE    ? 1 : 0);
        expNeverFull = expNeverFull    || (data[i] == 0x80u);
    }

    if (expAvailable != X.available()) {
        print(data);
    }
    ASSERT(expAvailable == X.available());

    if (expInUse != X.inUse()) {
        print(data);
    }
    ASSERT(expInUse == X.inUse());

    if (expMatchA != X.match(VA)) {
        print(data);
    }
    ASSERT(expMatchA == X.match(VA));

    if (expMatchB != X.match(VB)) {
        print(data);
    }
    ASSERT(expMatchB == X.match(VB));

    if (expMatchC != X.match(VC)) {
        print(data);
    }
    ASSERT(expMatchC == X.match(VC));

    if (expMatchD != X.match(VD)) {
        print(data);
    }
    ASSERT(expMatchD == X.match(VD));

    if (expMatchE != X.match(VE)) {
        print(data);
    }
    ASSERT(expMatchE == X.match(VE));

    if (expNeverFull != X.neverFull()) {
        print(data);
    }
    ASSERT(expNeverFull == X.neverFull());
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2; (void)verbose;
    bool veryVerbose         = argc > 3; (void)veryVerbose;
    bool veryVeryVerbose     = argc > 4; (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // ACCESSORS
        //   Ensure the accessors 'available', 'inUse', 'match', and
        //   'neverFull' operate as expected.
        //
        // Concerns:
        //: 1 The accessors 'available', 'inUse', 'match', and 'neverFull'
        //:   operate as expected.
        //:
        //: 2 The cardinality of possible inputs is very large and difficult to
        //:   explore.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the depth-enumerated technique, compare the results of
        //:   'available', 'inUse', 'match(V)', and 'neverFull' methods with
        //:   oracle implementations.  (C-1,2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   bsl::uint32_t available() const;
        //   bsl::uint32_t inUse() const;
        //   bsl::uint32_t match(bsl::uint8_t value) const;
        //   bool neverFull() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ACCESSORS" << endl
                          << "=========" << endl;

        if (verbose) cout << "\nTesting accessors." << endl;

        {
            bsl::uint8_t BACKGROUND[][16] =
                       {
                           { EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE },
                           { XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX },
                       };
            const bsl::size_t NUM_BACKGROUND
                                      = sizeof BACKGROUND / sizeof *BACKGROUND;

            bsl::uint8_t VALUE[] = { VA, VB, VC, VD, VE, EE, XX };
            const bsl::size_t NUM_VALUE = sizeof VALUE / sizeof *VALUE;

            { // depth 0
                for (bsl::size_t bi = 0; bi < NUM_BACKGROUND; ++bi) {
                    verifyWithOracle(BACKGROUND[bi]);
                }
            }
            { // depth 1
                for (bsl::size_t bi = 0; bi < NUM_BACKGROUND; ++bi) {
                    bsl::uint8_t data[16];
                    bsl::memcpy(data, BACKGROUND[bi], 16);

                    for (bsl::size_t i = 0; i < Obj::k_SIZE; ++i) {
                        for (bsl::size_t ii = 0; ii < NUM_VALUE; ++ii) {
                            data[i] = VALUE[ii];
                            verifyWithOracle(data);
                            data[i] = BACKGROUND[bi][i];
                        }
                    }
                }
            }
            { // depth 2
                for (bsl::size_t bi = 0; bi < NUM_BACKGROUND; ++bi) {
                    bsl::uint8_t data[16];
                    bsl::memcpy(data, BACKGROUND[bi], 16);
            //------^
            for (bsl::size_t i = 0; i < Obj::k_SIZE; ++i) {
                for (bsl::size_t ii = 0; ii < NUM_VALUE; ++ii) {
                    data[i] = VALUE[ii];
                    for (bsl::size_t j = i + 1; j < Obj::k_SIZE; ++j) {
                        for (bsl::size_t jj = 0; jj < NUM_VALUE; ++jj) {
                            data[j] = VALUE[jj];
                            verifyWithOracle(data);
                            data[j] = BACKGROUND[bi][j];
                        }
                    }
                    data[i] = BACKGROUND[bi][i];
                }
            }
            //------v
                }
            }
            { // depth 3
                for (bsl::size_t bi = 0; bi < NUM_BACKGROUND; ++bi) {
                    bsl::uint8_t data[16];
                    bsl::memcpy(data, BACKGROUND[bi], 16);
        //----------^
        for (bsl::size_t i = 0; i < Obj::k_SIZE; ++i) {
            for (bsl::size_t ii = 0; ii < NUM_VALUE; ++ii) {
                data[i] = VALUE[ii];
                for (bsl::size_t j = i + 1; j < Obj::k_SIZE; ++j) {
                    for (bsl::size_t jj = 0; jj < NUM_VALUE; ++jj) {
                        data[j] = VALUE[jj];
                        for (bsl::size_t k = j + 1; k < Obj::k_SIZE; ++k) {
                            for (bsl::size_t kk = 0; kk < NUM_VALUE; ++kk) {
                                data[k] = VALUE[kk];
                                verifyWithOracle(data);
                                data[k] = BACKGROUND[bi][k];
                            }
                        }
                        data[j] = BACKGROUND[bi][j];
                    }
                }
                data[i] = BACKGROUND[bi][i];
            }
        }
        //----------v
                }
            }
            { // depth 4
                for (bsl::size_t bi = 0; bi < NUM_BACKGROUND; ++bi) {
                    bsl::uint8_t data[16];
                    bsl::memcpy(data, BACKGROUND[bi], 16);
//------------------^
for (bsl::size_t i = 0; i < Obj::k_SIZE; ++i) {
    for (bsl::size_t ii = 0; ii < NUM_VALUE; ++ii) {
        data[i] = VALUE[ii];
        for (bsl::size_t j = i + 1; j < Obj::k_SIZE; ++j) {
            for (bsl::size_t jj = 0; jj < NUM_VALUE; ++jj) {
                data[j] = VALUE[jj];
                for (bsl::size_t k = j + 1; k < Obj::k_SIZE; ++k) {
                    for (bsl::size_t kk = 0; kk < NUM_VALUE; ++kk) {
                        data[k] = VALUE[kk];
                        for (bsl::size_t m = k + 1; m < Obj::k_SIZE; ++m) {
                            for (bsl::size_t mm = 0; mm < NUM_VALUE; ++mm) {
                                data[m] = VALUE[mm];
                                verifyWithOracle(data);
                                data[m] = BACKGROUND[bi][m];
                            }
                        }
                        data[k] = BACKGROUND[bi][k];
                    }
                }
                data[j] = BACKGROUND[bi][j];
            }
        }
        data[i] = BACKGROUND[bi][i];
    }
 }
//------------------v
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bsl::uint8_t data[16] =
                           { XX,VA,XX,VB,VA,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE };

            Obj mX(data);  const Obj& X = mX;

            ASSERT_SAFE_PASS(X.match(VA));
            ASSERT_SAFE_FAIL(X.match(Obj::k_EMPTY));
            ASSERT_SAFE_FAIL(X.match(Obj::k_ERASED));
            ASSERT_SAFE_FAIL(X.match(0xFF));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //   The creators operate as expected.
        //
        // Concerns:
        //: 1 The constructor correctly stores the provided data.
        //:
        //: 2 The destructor operats as expected.
        //
        // Plan:
        //: 1 Instantiate objects and verify the object's state using the
        //:   (untested) accessors.  (C-1)
        //:
        //: 2 Allow the objects to go out-of-scope (lack of a crash implies
        //:   success).  (C-2)
        //
        // Testing:
        //   FlatHashTable_GroupControl(const bsl::uint8_t *data);
        //   ~FlatHashTable_GroupControl();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS" << endl
                          << "========" << endl;

        {
            bsl::uint8_t data[16] =
                           { XX,VA,XX,VB,VA,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE };

            Obj mX(data);  const Obj& X = mX;

            ASSERT(0x1A == X.inUse());
            ASSERT(0x12 == X.match(VA));
            ASSERT(0x08 == X.match(VB));
            ASSERT(0x00 == X.match(VC));
            ASSERT(0x00 == X.match(VD));
            ASSERT(0x00 == X.match(VE));
            ASSERT(true == X.neverFull());
        }
        {
            bsl::uint8_t data[16] =
                           { VA,VB,VC,VD,VE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE };

            Obj mX(data);  const Obj& X = mX;

            ASSERT(0x1F == X.inUse());
            ASSERT(0x01 == X.match(VA));
            ASSERT(0x02 == X.match(VB));
            ASSERT(0x04 == X.match(VC));
            ASSERT(0x08 == X.match(VD));
            ASSERT(0x10 == X.match(VE));
            ASSERT(true == X.neverFull());
        }
        {
            bsl::uint8_t data[16] =
                           { XX,VA,XX,VB,VA,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX,XX };

            Obj mX(data);  const Obj& X = mX;

            ASSERT(0x1A  == X.inUse());
            ASSERT(0x12  == X.match(VA));
            ASSERT(0x08  == X.match(VB));
            ASSERT(0x00  == X.match(VC));
            ASSERT(0x00  == X.match(VD));
            ASSERT(0x00  == X.match(VE));
            ASSERT(false == X.neverFull());
        }
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
        //: 1 Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bsl::uint8_t data[16] =
                           { XX,VA,XX,VB,VA,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE,EE };

        Obj mX(data);  const Obj& X = mX;

        ASSERT(0x1A == X.inUse());
        ASSERT(0x12 == X.match(VA));
        ASSERT(0x08 == X.match(VB));
        ASSERT(true == X.neverFull());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

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
