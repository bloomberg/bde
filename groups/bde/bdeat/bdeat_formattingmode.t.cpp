// bdeat_formattingmode.t.cpp                                         -*-C++-*-

#include <bdeat_formattingmode.h>

#include <bslmf_assert.h>

#include <bsl_c_string.h>     // strlen()
#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// Due to the extremely simple nature of this component, we only have to test
// that the flags are defined and that they do not overlap in bit patterns.
//-----------------------------------------------------------------------------
// bdeat_FormattingMode::DEFAULT
// bdeat_FormattingMode::DEC
// bdeat_FormattingMode::HEX
// bdeat_FormattingMode::BASE64
// bdeat_FormattingMode::TEXT
// bdeat_FormattingMode::LIST
// bdeat_FormattingMode::UNTAGGED
// bdeat_FormattingMode::ATTRIBUTE
// bdeat_FormattingMode::SIMPLE_CONTENT
// bdeat_FormattingMode::NILLABLE
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdeat_FormattingMode FM;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        //
        // Concerns:
        //   - BDEAT_TYPE_MASK and BDEAT_FLAGS_MASK are disjoint (i.e., have no
        //     bits in common).
        //   - Each of the mode enumerations are unique.
        //   - All of the bits of each schema type enumeration are within the
        //     BDEAT_TYPE_MASK bit-mask.
        //   - All of the bits of formatting flag enumeration are within the
        //     BDEAT_FLAGS_MASK bit-mask.
        //   - None of the formatting flag enumerations share any bits in
        //     common with any of the other mode enumerations.
        //
        // Plan:
        //   - Test that BDEAT_TYPE_MASK and BDEAT_FLAGS_MASK have no bits in
        //     common.
        //   - Loop over each schema type enumeration and verify that it is
        //     within the TYPE_MASK bit-mask and does not overlap the
        //     BDEAT_FLAGS_MASK.
        //   - In a nested loop, test that each schema type enumeration is
        //     not equal to another schema type enumeration.
        //   - Loop over each formatting flag enumeration and verify that it is
        //     within the BDEAT_FLAGS_MASK bit-mask and does not overlap the
        //     BDEAT_TYPE_MASK.
        //   - In a nested loop, test that each formatting flag enumeration has
        //     no bits in common with another formatting flag enumeration.
        //
        // Testing:
        //    bdeat_FormattingMode::BDEAT_DEFAULT
        //    bdeat_FormattingMode::BDEAT_DEC
        //    bdeat_FormattingMode::BDEAT_HEX
        //    bdeat_FormattingMode::BDEAT_BASE64
        //    bdeat_FormattingMode::BDEAT_TEXT
        //    bdeat_FormattingMode::BDEAT_TYPE_MASK
        //    bdeat_FormattingMode::BDEAT_UNTAGGED
        //    bdeat_FormattingMode::BDEAT_ATTRIBUTE
        //    bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT
        //    bdeat_FormattingMode::BDEAT_NILLABLE
        //    bdeat_FormattingMode::BDEAT_LIST
        //    bdeat_FormattingMode::BDEAT_FLAGS_MASK
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "====================" << endl;

        // Test that BDEAT_TYPE_MASK and BDEAT_FLAGS_MASK have no bits in
        // common.
        ASSERT(0 == (FM::BDEAT_TYPE_MASK & FM::BDEAT_FLAGS_MASK));

        if (veryVerbose) cout << "\tTesting type masks\n"
                              << "\t------------------" << endl;

        static const int TYPE_MODES[] = {
            FM::BDEAT_DEFAULT,
            FM::BDEAT_DEC,
            FM::BDEAT_HEX,
            FM::BDEAT_BASE64,
            FM::BDEAT_TEXT
        };

        static const int NUM_TYPE_MODES =
            sizeof(TYPE_MODES) / sizeof(TYPE_MODES[0]);

        // Loop over each schema type enumerations.
        for (int i = 0; i < NUM_TYPE_MODES; ++i) {
            const int TYPE_MODE1 = TYPE_MODES[i];

            // Verify that TYPE_MODE1 is within the TYPE_MASK bit-mask and
            // does not overlap the FLAGS_MASK.
            LOOP_ASSERT(TYPE_MODE1,
                        TYPE_MODE1 == (TYPE_MODE1 & FM::BDEAT_TYPE_MASK));
            LOOP_ASSERT(TYPE_MODE1,
                        0          == (TYPE_MODE1 & FM::BDEAT_FLAGS_MASK));

            for (int j = 0; j < NUM_TYPE_MODES; ++j) {
                if (j == i) continue;

                const int TYPE_MODE2 = TYPE_MODES[j];

                // test that each schema type enumeration is not equal to
                // another schema type enumeration.
                LOOP2_ASSERT(TYPE_MODE1, TYPE_MODE2, TYPE_MODE1 != TYPE_MODE2)
            }
        }

        if (veryVerbose) cout << "\tTesting flags masks\n"
                              << "\t-------------------" << endl;

        static const int FLAG_MODES[] = {
            FM::BDEAT_UNTAGGED,
            FM::BDEAT_ATTRIBUTE,
            FM::BDEAT_SIMPLE_CONTENT,
            FM::BDEAT_NILLABLE,
            FM::BDEAT_LIST
        };

        static const int NUM_FLAG_MODES =
            sizeof(FLAG_MODES) / sizeof(FLAG_MODES[0]);

        // Loop over each schema type enumeration.
        for (int i = 0; i < NUM_FLAG_MODES; ++i) {
            const int FLAG_MODE1 = FLAG_MODES[i];

            // Verify that FLAG_MODE1 is within the 'BDEAT_FLAGS_MASK' bit-mask
            // and does not overlap the 'BDEAT_TYPE_MASK'.
            LOOP_ASSERT(FLAG_MODE1,
                        FLAG_MODE1 == (FLAG_MODE1 & FM::BDEAT_FLAGS_MASK));
            LOOP_ASSERT(FLAG_MODE1,
                        0          == (FLAG_MODE1 & FM::BDEAT_TYPE_MASK));

            for (int j = 0; j < NUM_FLAG_MODES; ++j) {
                if (j == i) continue;

                const int FLAG_MODE2 = FLAG_MODES[j];

                // Test that each formatting flag enumeration has no bits in
                // common with another formatting flag enumeration.
                LOOP2_ASSERT(FLAG_MODE1, FLAG_MODE2,
                             0 == (FLAG_MODE1 & FLAG_MODE2));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
