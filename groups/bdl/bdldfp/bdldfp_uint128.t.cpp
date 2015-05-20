// bdldfp_uint128.t.cpp                                               -*-C++-*-

#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

using namespace bdldfp;

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
//    bool         veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING Breathing test
        // Concerns: All operations work sanely.
        // Plan: Try all operations see if basics work
        // Testing: all functions
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;
        Uint128 val = 127;

        val << 6;
        val >> 6;

        val & val;
        val | val;
        val ^ val;

        ASSERT(true  == (val == val));
        ASSERT(false == (val != val));


        const bsls::Types::Uint64 VALUE_A = 0;
        const bsls::Types::Uint64 VALUE_B = 17;
        const bsls::Types::Uint64 VALUE_C = 19;
        const bsls::Types::Uint64 VALUE_D = 16;

        // Prototype Test driver work:

        // Phase 1: Test 0-based creation:

        // Concern: Check that a default-created object has high and low words
        // set to 0.
        const Uint128 blank;
        ASSERT(blank.low()  == 0);
        ASSERT(blank.high() == 0);


        // Phase 2a: Test integer-init creation:

        //: o Ubiquitous Concern: All int-created values should have the
        //:   high-word set to 0.
        //: o Ubiquitous Concern: All int-created values should have the
        //:   low-word set to the passed parameter.

        const Uint128    fromInt(VALUE_A);
        ASSERT(fromInt.low()  == VALUE_A);
        ASSERT(fromInt.high() == 0);

        const Uint128    fromInt2(VALUE_B);
        ASSERT(fromInt2.low()  == VALUE_B);
        ASSERT(fromInt2.high() == 0);

        const Uint128    fromInt3(VALUE_C);
        ASSERT(fromInt3.low()  == VALUE_C);
        ASSERT(fromInt3.high() == 0);

        const Uint128    fromInt4(VALUE_D);
        ASSERT(fromInt4.low()  == VALUE_D);
        ASSERT(fromInt4.high() == 0);

        // Phase 2b: Test high-low integrity, on synthesize-with-high:

        //: o Ubiquitous Concern: Low words shouldn't be affected by high-word
        //:   manipulation.
        //: o Ubiquitous Concern: High words should be set to the value being
        //:   used.
        //: o Note that this test is effectively not needed, but is included
        //:   for theoretical completeness.

        Uint128 fromHigh;
        fromHigh.setHigh(VALUE_A);
        ASSERT(fromHigh.low()  == 0);
        ASSERT(fromHigh.high() == VALUE_A);

        Uint128 fromHigh2;
        fromHigh2.setHigh(VALUE_B);
        ASSERT(fromHigh2.low()  == 0);
        ASSERT(fromHigh2.high() == VALUE_B);

        Uint128 fromHigh3;
        fromHigh3.setHigh(VALUE_C);
        ASSERT(fromHigh3.low()  == 0);
        ASSERT(fromHigh3.high() == VALUE_C);

        Uint128 fromHigh4;
        fromHigh4.setHigh(VALUE_D);
        ASSERT(fromHigh4.low()  == 0);
        ASSERT(fromHigh4.high() == VALUE_D);



        // Phase 3: Test comparison of values:

        // Concern: Values A through D compare equal to themselves.
        // Special case -- Value A is always 0.

        ASSERT(blank    == fromInt);
        ASSERT(fromInt  == fromInt);
        ASSERT(fromInt2 == fromInt2);
        ASSERT(fromInt3 == fromInt3);
        ASSERT(fromInt4 == fromInt4);

        // Concern: Values B thru D compare not-equal to blank

        ASSERT(fromInt2 != blank);
        ASSERT(fromInt3 != blank);
        ASSERT(fromInt4 != blank);


        // Concern: Values compare equal to a constructed variant of themselves
        // (as a temporary) LHS and RHS variants will be tested.

        ASSERT(fromInt2 == Uint128(VALUE_B));
        ASSERT(fromInt3 == Uint128(VALUE_C));
        ASSERT(fromInt4 == Uint128(VALUE_D));

        ASSERT(Uint128(VALUE_B) == fromInt2);
        ASSERT(Uint128(VALUE_C) == fromInt3);
        ASSERT(Uint128(VALUE_D) == fromInt4);

        // Concern: Values A thru D compare not equal to not-themselves Note
        // that all lhs and rhs pairings are tried.  This helps to guarantee
        // the commutative property of the != operator.

        ASSERT(fromInt  != fromInt2);
        ASSERT(fromInt  != fromInt3);
        ASSERT(fromInt  != fromInt4);

        ASSERT(fromInt2 != fromInt);
        ASSERT(fromInt2 != fromInt3);
        ASSERT(fromInt2 != fromInt4);

        ASSERT(fromInt3 != fromInt);
        ASSERT(fromInt3 != fromInt2);
        ASSERT(fromInt3 != fromInt4);

        ASSERT(fromInt4 != fromInt);
        ASSERT(fromInt4 != fromInt2);
        ASSERT(fromInt4 != fromInt3);


        // Concern: High word checks
        ASSERT(blank            == fromHigh);
        ASSERT(fromHigh4        != fromInt4);
        ASSERT(fromHigh4.high() == fromInt4.low());

        // Phase 4: Test lhs and rhs promotion from integer.  Will test against
        // known values:

        ASSERT(fromInt  == VALUE_A);
        ASSERT(fromInt2 == VALUE_B);
        ASSERT(fromInt3 == VALUE_C);
        ASSERT(fromInt4 == VALUE_D);

        ASSERT(VALUE_A == fromInt);
        ASSERT(VALUE_B == fromInt2);
        ASSERT(VALUE_C == fromInt3);
        ASSERT(VALUE_D == fromInt4);


        // Phase 4: Test 'and', 'or', and 'xor':
        ASSERT((blank & fromInt2) != fromInt2);
        ASSERT((blank & fromInt2) != fromInt3);
        ASSERT((blank & fromInt2) == blank);

        ASSERT((fromInt4 & fromInt3) != blank);
        ASSERT((fromInt4 & fromInt3) != fromInt2);
        ASSERT((fromInt4 & fromInt3) != fromInt3);
        ASSERT((fromInt4 & fromInt3) == fromInt4);

        ASSERT((blank & fromHigh2) != fromHigh2);
        ASSERT((blank & fromHigh2) != fromHigh3);
        ASSERT((blank & fromHigh2) == blank);

        ASSERT((fromHigh4 & fromHigh3) != blank);
        ASSERT((fromHigh4 & fromHigh3) != fromHigh2);
        ASSERT((fromHigh4 & fromHigh3) != fromHigh3);
        ASSERT((fromHigh4 & fromHigh3) == fromHigh4);

        //TODO: Write tests for or and xor


        // Phase 5: Test left and right shift:
        //: o Ubiquitous Concern: All comparisons are tested by high-then-low
        //:   tests for shifting, to provide inspection of implementation
        //:   mechanism.
        //: o Ubiquitous Concern: High bits get tested first for right shifts,
        //:   to make sure they are properly evacuated.
        //: o Ubiquitous Concern: Low bits get tested first for left shifts, to
        //:   make sure they are properly evacuated.

        // Concern: blank numbers shifted to the right stay blank
            ASSERT((blank >> 50).low()  == 0);
            ASSERT((blank >> 50).high() == 0);
            ASSERT((blank >> 50)        == blank);

        // Concern: a small number shifted right falls into 0.
            ASSERT((fromInt4 >> 50) != fromInt4);
            ASSERT((fromInt4 >> 50) == blank);

        // Concern: A large number (with high-word bits) shifted to the right
        // by nearly 128 bits (and missing the high bit set) will compare to
        // zero
            ASSERT((fromHigh4 >> 127).high() == 0);
            ASSERT((fromHigh4 >> 127).low () == 0);
            ASSERT((fromHigh4 >> 127)        == blank);

        // Concern: bits sliding off the high into the low block, should
        // become 0, over 2 shifts.
            ASSERT(((fromHigh4 >> 127) >> 1).high() == 0);
            ASSERT(((fromHigh4 >> 127) >> 1).low()  == 0);
            ASSERT(((fromHigh4 >> 127) >> 1)        == blank);

        // Test high going into low zone.
            ASSERT((fromHigh3 >> 64) != fromHigh3);
            ASSERT((fromHigh3 >> 64) == fromInt3);

        // Test blank numbers to the left stay blank
            ASSERT((blank << 70) != fromHigh2);
            ASSERT((blank << 70) != fromHigh3);
            ASSERT((blank << 70) == blank);

        // Test that a high-word number falls off the top
            ASSERT((fromHigh4 << 70) != fromHigh4);
            ASSERT((fromHigh4 << 70) == blank);

        // Test wrapping from a low word into the high word, and becoming 0
        // over 2 shifts
            ASSERT(((fromInt4 << 127) << 1).low()  == 0);
            ASSERT(((fromInt4 << 127) << 1).high() == 0);
            ASSERT(((fromInt4 << 127) << 1)        != fromInt4);
            ASSERT(((fromInt4 << 127) << 1)        == blank);

        // Test low numbers coming into the high zone
            ASSERT((fromInt3 << 64) != fromInt3);
            ASSERT((fromInt3 << 64) == fromHigh3);


        // Two direction shift tests:

            ASSERT((fromInt3 << 16) != (fromHigh3 >> (128 - 16)));
            ASSERT((fromInt3 << 16) != (fromHigh3 >> (128 - 16)));

            for (int i = 0; i < 64; ++i) {
                // For up to one word shifts, check that they work correctly
                // from opposite directions.
                ASSERT((fromInt3 << i) == (fromHigh3 >> (64 - i)));

                // For compound-shifts beyond one word, check that they go to
                // the other word, from both directions.
                ASSERT((fromInt3 << (64 + i)) == (fromHigh3 << i));
                ASSERT((fromInt3 >> i) == (fromHigh3 >> (64 + i)));
            }

            ASSERT((fromHigh4 >> 32)== (fromInt4 << 32));
      } break;

    default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
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
