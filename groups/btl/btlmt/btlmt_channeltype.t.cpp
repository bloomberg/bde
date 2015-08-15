// btlmt_channeltype.t.cpp                                            -*-C++-*-

#include <btlmt_channeltype.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// Standard enumeration test plan.
// ----------------------------------------------------------------------------
// [ 1] enum VAlue { ... };
// [ 1] enum { LENGTH = ... };
// [ 1] static const char *toAscii(Day value);
//
// [ 1] operator<<(ostream&, btlmt::ChannelType::Value rhs);
// [ 2] USAGE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btlmt::ChannelType  Class;
typedef Class::Value       Enum;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

// First, create a variable 'channelType' of type 'btlmt::ChannelType::Value'
// and initialize it to the value 'btlmt::ChannelType::BTEMT_CONNECTED_CHANNEL'.
//..
    btlmt::ChannelType::Value channelType =
                                    btlmt::ChannelType::BTEMT_CONNECTED_CHANNEL;
//..
// Next, store its representation in a variable 'rep' of type 'const char*'.
//..
    const char *rep = btlmt::ChannelType::toAscii(channelType);
    ASSERT(0 == strcmp(rep, "CONNECTED_CHANNEL"));
//..
// Finally, print the value of 'channelType' to 'stdout'.
//..
    if (verbose) bsl::cout << channelType << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  CONNECTED_CHANNEL
//..

      } break;
      case 1: {
        // --------------------------------------------------------
        // Testing:
        //   static const char *toAscii(Day dayOfWeek);
        //   ostream& operator<<(ostream& output,
        //                       bdlt::DayOfWeek::Enum dayOfWeek);
        // --------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        static const struct {
            int         d_line;                // Line number
            int         d_intType;             // Enumerated Value as int
            const char *d_exp;                 // Expected String Rep.
        } DATA[] = {
            // line   Enumerated Value                  Expected output
            // ----   ----------------                  ---------------
            {  L_,    Class::BTEMT_LISTENING_CHANNEL,   "LISTENING_CHANNEL"  },
            {  L_,    Class::BTEMT_ACCEPTED_CHANNEL,    "ACCEPTED_CHANNEL"   },
            {  L_,    Class::BTEMT_CONNECTING_CHANNEL,  "CONNECTING_CHANNEL" },
            {  L_,    Class::BTEMT_CONNECTED_CHANNEL,   "CONNECTED_CHANNEL"  },
            {  L_,    Class::BTEMT_IMPORTED_CHANNEL,    "IMPORTED_CHANNEL"   },
            {  L_,    0,                                "(* UNKNOWN *)"      },
            {  L_,    1 + Class::BTEMT_LENGTH,          "(* UNKNOWN *)"      },
            {  L_,    10,                               "(* UNKNOWN *)"      }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Testing 'toAscii'." << endl;
        {
            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const Enum  TYPE = (Enum) DATA[i].d_intType;
                const char *EXP  = DATA[i].d_exp;

                const char *res = Class::toAscii(TYPE);
                if (veryVerbose) { cout << '\t'; P_(i); P_(TYPE); P(res); }
                LOOP2_ASSERT(LINE, i, 0 == strcmp(EXP, res));
            }
        }

        if (verbose) cout << "Testing 'operator<<'." << endl;
        {
            const int   SIZE = 100;
            char        buf[SIZE];
            const char  XX = (char) 0xFF;   // Value for an unset char.
            char        mCtrl[SIZE];           memset(mCtrl, XX, SIZE);
            const char *CTRL = mCtrl;

            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const Enum  TYPE  = (Enum) DATA[i].d_intType;
                const char *EXP  = DATA[i].d_exp;

                memset(buf, XX, SIZE);
                ostrstream out(buf, SIZE);
                out << TYPE << ends;

                const int SZ = strlen(EXP) + 1;
                if (veryVerbose) { cout << '\t'; P_(i); P(buf); }
                LOOP2_ASSERT(LINE, i, XX == buf[SIZE - 1]);
                LOOP2_ASSERT(LINE, i,  0 == memcmp(buf, EXP, SZ));
                LOOP2_ASSERT(LINE, i,
                             0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));
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
