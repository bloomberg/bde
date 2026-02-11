// bdlt_timezone_specifierformatter.t.cpp                             -*-C++-*-

#include <bdlt_timezone_specifierformatter.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

//=============================================================================
//                       GLOBAL TYPES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::TimeZone_SpecifierFormatter<char>      Obj;
typedef bdlt::FormatUtil<char>                       Util;

const int k_COLON       = 0x1;
const int k_NO_COLON    = 0x2;
const int k_TIME_ZONE_Z = 0x4;

//=============================================================================
//                       GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2; (void) verbose;
    const bool         veryVerbose = argc > 3; (void) veryVerbose;
    const bool     veryVeryVerbose = argc > 4; (void) veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        enum Mode { k_Z, k_ISO, k_DEF };

        static const struct Data {
            int         d_line;
            int         d_modifiers;
            Mode        d_mode;
            int         d_offset;
            const char *d_expStr;
        } DATA[] = {
            { L_, 0,                          k_Z,     0, "+0000" },
            { L_, 0,                          k_DEF,   0, "+0000" },
            { L_, 0,                          k_ISO,   0, "+00:00" },
            { L_, 0,                          k_Z,   125, "+0205" },
            { L_, 0,                          k_DEF, 125, "+0205" },
            { L_, 0,                          k_ISO, 125, "+02:05" },
            { L_, k_COLON,                    k_Z,     0, "+00:00" },
            { L_, k_COLON,                    k_DEF,   0, "+00:00" },
            { L_, k_COLON,                    k_ISO,   0, "+00:00" },
            { L_, k_NO_COLON,                 k_Z,     0, "+0000" },
            { L_, k_NO_COLON,                 k_DEF,   0, "+0000" },
            { L_, k_NO_COLON,                 k_ISO,   0, "+0000" },
            { L_, k_COLON,                    k_Z,    60, "+01:00" },
            { L_, k_COLON,                    k_DEF,  60, "+01:00" },
            { L_, k_COLON,                    k_ISO,  60, "+01:00" },
            { L_, k_NO_COLON,                 k_Z,   -60, "-0100" },
            { L_, k_NO_COLON,                 k_DEF, -60, "-0100" },
            { L_, k_NO_COLON,                 k_ISO, -60, "-0100" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_Z,     0, "Z" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_DEF,   0, "Z" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_ISO,   0, "Z" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_Z,     0, "Z" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_DEF,   0, "Z" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_ISO,   0, "Z" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_Z,   127, "+02:07" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_DEF, 127, "+02:07" },
            { L_, k_TIME_ZONE_Z | k_COLON,    k_ISO, 127, "+02:07" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_Z,   127, "+0207" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_DEF, 127, "+0207" },
            { L_, k_TIME_ZONE_Z | k_NO_COLON, k_ISO, 127, "+0207" } };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < 2 * k_NUM_DATA; ++ti) {
            const Data&           data       = DATA[ti % k_NUM_DATA];
            const int             LINE       = data.d_line;
            const int             MODIFIERS  = data.d_modifiers;
            const Mode            MODE       = data.d_mode;
            const int             OFFSET     = data.d_offset;
            const char           *EXP        = data.d_expStr;
            const bsl::ptrdiff_t  WIDTH      = bsl::strlen(EXP);
            const bool            NULL_PARSE = k_NUM_DATA <= ti;

            Obj mX;    const Obj& X = mX;

            if (NULL_PARSE) {
                // should not modify 'mX'.

                bsl::string_view sv("%%");
                ASSERT(false == mX.parseNextSpecifier(&sv));
            }

            bsl::string spec;
            if (MODIFIERS & k_COLON) {
                spec += ':';
            }
            if (MODIFIERS & k_NO_COLON) {
                spec += "_";
            }
            if (MODIFIERS & k_TIME_ZONE_Z) {
                spec += "Z";
            }
            if (ti & 1) {
                spec += "%";
            }
            bsl::string_view svSpec(spec);
            while (!svSpec.empty() && '%' != svSpec.front()) {
                ASSERT(mX.parseNextModifier(&svSpec));
            }

            switch (MODE) {
              case k_Z: {
                bsl::string_view sv("z");
                ASSERT(true == mX.parseNextSpecifier(&sv));
                ASSERT(sv.empty());
              } break;
              case k_ISO: {
                mX.parseIso8601();
              } break;
              case k_DEF: {
                mX.parseDefault();
              } break;
              default: {
                ASSERT(0);
              }
            }

            ASSERTV(LINE, WIDTH, X.totalWidth(OFFSET),
                                                WIDTH == X.totalWidth(OFFSET));

            char buf[] = { "**************************" };
            char *pc = k_ISO == MODE ? X.formatIso8601(buf, OFFSET)
                                     : X.formatDefault(buf, OFFSET);
            ASSERTV(LINE, EXP, buf, EXP == bsl::string_view(buf, pc));

            if (veryVerbose) {
                const bool colon     = MODIFIERS & k_COLON;
                const bool noColon   = MODIFIERS & k_NO_COLON;
                const bool timeZoneZ = MODIFIERS & k_TIME_ZONE_Z;

                P_(colon);    P_(noColon);    P_(timeZoneZ);  P(MODE == k_ISO);
                P_(OFFSET);   P_(WIDTH);      P(buf);
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
// Copyright 2026 Bloomberg Finance L.P.
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
