// btlmt_channelstatus.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_channelstatus.h>

#include <btlb_blob.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>       // 'atoi'
#include <bsl_cstring.h>       // 'strcmp', 'memcmp', 'memcpy'
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a namespace for an enumeration listing
// status codes returned from channel operations.  The enumerators in the
// enumeration do not start from 0 and are not sequential, because the status
// codes represented by the enumerators maybe written before their definition.
//
// We will therefore follow our standard 3-step approach to testing
// enumeration.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum ChannelStatus::Enum { ... };
//
// CLASS METHODS
// [ 3] ostream& ChannelStatus::print(s, val, level = 0, sPL = 4);
// [ 1] const char *ChannelStatus::toAscii(val);
//
// FREE OPERATORS
// [ 2] operator<<(ostream& s, ChannelStatus::Enum val);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

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
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

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
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

class MyChannel {
    // This class represents a channel over which data may be sent and
    // received.

  public:
    //...
    int write(const btlb::Blob& blob);
        // Enqueue the specified 'blob' message to be written to this channel.
        // Return 0 on success, and a non-zero value otherwise.  On error, the
        // return value *may* equal to one of the enumerators in
        // 'btlmt::ChannelErrorWrite::Enum'.
    //...
};

int MyChannel::write(const btlb::Blob& blob)
{
    (void)blob;
    return btlmt::ChannelStatus::e_SUCCESS;
}

}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    // bool         veryVerbose = argc > 3;
    // bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose that we have a class 'MyChannel' that defines the following elided
// interface:
//..

//..
// Further suppose that we have a 'btlb::Blob' object, 'blob', and a
// 'MyChannel' object, 'channel'.
        MyChannel channel;
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        btlb::Blob blob(&scratch);
//
// First, we use the 'write' method to write 'blob' into 'channel':
//..
        int rc = channel.write(blob);
//..
// Now, we compare the return code to the enumerators in
// 'btlmt::ChannelStatus::Enum' to handle the different error conditions:
//..
        switch(rc) {
          case btlmt::ChannelStatus::e_SUCCESS: { // Success

              // ...

          } break;

          // We handle the different types of failures in the cases below.

          case btlmt::ChannelStatus::e_CACHE_OVERFLOW: {

              // ...

          } break;
          case btlmt::ChannelStatus::e_CACHE_HIGHWATER: {

              // ...

          } break;
          case btlmt::ChannelStatus::e_WRITE_CHANNEL_DOWN: {

              // ...

          } break;
          default: { // Handle other failures.

              //...

          }
        };
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'btlmt::ChannelStatus::print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //:
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 There is no output when the stream is invalid.
        //:
        //: 5 The 'print' method has the expected signature.
        //
        // Plan:
        //: 1 Use table-based approach, for each enumerator, some out-of-band
        //:   values, and a range of level and spacing per level values that
        //:   can be specified to the 'print' method:
        //:
        //:   1 Verify that the 'print' method produces the expected results
        //:     (C-1..3)
        //:
        //:   2 Verify that there is no output when the stream is invalid.
        //:     (C-4)
        //:
        //: 2 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-5)
        //
        // Testing:
        //   ostream& btlmt::ChannelStatus::print(s, val, level = 0, sPL = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'btlmt::ChannelStatus::print'" << endl
                          << "============================" << endl;

        typedef btlmt::ChannelStatus Obj;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // level
            int         d_spacePerLevel;  // spaces per level
            Obj::Enum   d_enumerator;     // enumerator
            const char *d_expString;      // expected ascii value
        } DATA[] = {
#define NL "\n"
        //line lvl spl enumerator                     result
        //---- --- --- -----------------------------  ----------------------
        { L_,   0,  4, Obj::e_SUCCESS,            "SUCCESS" NL           },
        { L_,   0,  4, Obj::e_CACHE_OVERFLOW,     "CACHE_OVERFLOW" NL    },
        { L_,   0,  4, Obj::e_CACHE_HIGHWATER,    "CACHE_HIGHWATER" NL   },
        { L_,   0,  4, Obj::e_READ_CHANNEL_DOWN,  "READ_CHANNEL_DOWN" NL },
        { L_,   0,  4, Obj::e_WRITE_CHANNEL_DOWN, "WRITE_CHANNEL_DOWN" NL},
        { L_,   0,  4, Obj::e_ENQUEUE_HIGHWATER,  "ENQUEUE_HIGHWATER" NL },
        { L_,   0,  4, Obj::e_UNKNOWN_ID,         "UNKNOWN_ID" NL        },
        { L_,   0,  4, (Obj::Enum) 1,                 UNKNOWN_FORMAT NL      },

        { L_,   0,  0, Obj::e_SUCCESS,            "SUCCESS" NL           },
        { L_,   0, -1, Obj::e_SUCCESS,            "SUCCESS"              },
        { L_,   0,  2, Obj::e_SUCCESS,            "SUCCESS" NL           },
        { L_,   1,  1, Obj::e_SUCCESS,            " SUCCESS" NL          },
        { L_,   1,  2, Obj::e_SUCCESS,            "  SUCCESS" NL         },
        { L_,  -1,  2, Obj::e_SUCCESS,            "SUCCESS" NL           },
        { L_,  -2,  1, Obj::e_SUCCESS,            "SUCCESS" NL           },
        { L_,   2,  1, Obj::e_SUCCESS,            "  SUCCESS" NL         },
        { L_,   1,  3, Obj::e_SUCCESS,            "   SUCCESS" NL        },
#undef NL
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = (char)0xFF;  // value of an unset 'char'
              char  buf[SIZE];          // output buffer

              char  mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        // Verify enumerators.
        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const int        LINE  = DATA[ti].d_lineNum;
            const int        LEVEL = DATA[ti].d_level;
            const int        SPL   = DATA[ti].d_spacePerLevel;
            const Obj::Enum  ENUM  = DATA[ti].d_enumerator;
            const char      *STR   = DATA[ti].d_expString;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            ostrstream out(buf, sizeof buf);

            Obj::print(out, ENUM, LEVEL, SPL) << ends;


            const int SZ = strlen(STR) + 1;

            // Verify that the buffer is large enough.
            ASSERTV(LINE, ti, SZ  < SIZE);

            ASSERTV(LINE, ti, buf, STR, 0 == memcmp(buf, STR, SZ));

            // Check for overrun.
            ASSERTV(LINE, ti, XX == buf[SIZE - 1]);
            ASSERTV(LINE, ti,  0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));

            // Repeat for 'print' default arguments.
            if (0 == LEVEL && 4 == SPL) {

                memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char'
                                          // values.

                ostrstream out(buf, sizeof buf);
                Obj::print(out, ENUM) << ends;

                ASSERTV(LINE, ti,  0 == memcmp(buf, STR, SZ));

                // Check for overrun.
                ASSERTV(LINE, ti, XX == buf[SIZE - 1]);
                ASSERTV(LINE, ti,
                        0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));
            }
        }

        // Verify that nothing is written to a bad stream.
        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const int        LINE  = DATA[ti].d_lineNum;
            const int        LEVEL = DATA[ti].d_level;
            const int        SPL   = DATA[ti].d_spacePerLevel;
            const Obj::Enum  ENUM  = DATA[ti].d_enumerator;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            ostrstream out(buf, sizeof buf);  out.setstate(ios::badbit);
            Obj::print(out, ENUM, LEVEL, SPL);

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(buf, CTRL, SIZE));
        }

        // Verify 'print' signature.
        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&,
                                             Obj::Enum,
                                             int,
                                             int);

            const FuncPtr FP = &Obj::print;
            (void) FP;  // quash potential compiler warning
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // '<<' OPERATOR FOR 'btlmt::ChannelStatus::Enum'
        //
        // Concerns:
        //: 1 The '<<' operator writes the output to the specified stream.
        //:   (P-1)
        //:
        //: 2 The '<<' operator writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The '<<' operator writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 The output produced by 'stream << value' is the same as that
        //:   produced by 'Obj::print(stream, value, 0, -1)'.
        //:
        //: 5 There is no output when the stream is invalid.
        //:
        //: 6 The '<<' operator has the expected signature.
        //
        // Plan:
        //: 1 Use table-based approach, for each enumerator and some
        //:   out-of-band values:
        //:
        //:   1 Verify that the '<<' operator produces the expected results.
        //:     (C-1..3)
        //:
        //:   2 Verify that 'Obj::print(stream, value, 0, -1)' produces the
        //:     same results.  (C-4)
        //:
        //:   3 Verify that there is no output when stream is invalid.  (C-5)
        //:
        //: 5 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, btlmt::ChannelStatus::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "'<<' OPERATOR FOR 'btlmt::ChannelStatus::Enum'" << endl
                    << "=============================================" << endl;

        typedef btlmt::ChannelStatus Obj;

        static const struct {
            int         d_lineNum;     // source line number
            Obj::Enum   d_enumerator;  // enumerator
            const char *d_expString;   // expected ascii value
        } DATA[] = {
            //line  enumerator                      result
            //----  ------------------------------  --------------------
            { L_,   Obj::e_SUCCESS,             "SUCCESS"            },
            { L_,   Obj::e_CACHE_OVERFLOW,      "CACHE_OVERFLOW"     },
            { L_,   Obj::e_CACHE_HIGHWATER,     "CACHE_HIGHWATER"    },
            { L_,   Obj::e_READ_CHANNEL_DOWN,   "READ_CHANNEL_DOWN"  },
            { L_,   Obj::e_WRITE_CHANNEL_DOWN,  "WRITE_CHANNEL_DOWN" },
            { L_,   Obj::e_ENQUEUE_HIGHWATER,   "ENQUEUE_HIGHWATER"  },
            { L_,   Obj::e_UNKNOWN_ID,          "UNKNOWN_ID"         },

            { L_,   (Obj::Enum) 1,                  UNKNOWN_FORMAT       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = (char)0xFF;  // value of an unset 'char'
              char  buf[SIZE];          // output buffer

              char  mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        // Verify enumerators.
        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const int        LINE = DATA[ti].d_lineNum;
            const Obj::Enum  ENUM = DATA[ti].d_enumerator;
            const char      *STR  = DATA[ti].d_expString;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            ostrstream out(buf, sizeof buf);

            out << ENUM << ends;

            const int SZ = strlen(STR) + 1;

            // Verify that the buffer is large enough.
            ASSERTV(LINE, ti, SZ  < SIZE);

            ASSERTV(LINE, ti,  0 == memcmp(buf, STR, SZ));

            // Check for overrun.
            ASSERTV(LINE, ti, XX == buf[SIZE - 1]);
            ASSERTV(LINE, ti,  0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));


            // Verify that 'Obj::print(stream, value, 0, -1)' produces the same
            // results.
            {
                memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char'
                                          // values.

                ostrstream out(buf, sizeof buf);
                Obj::print(out, ENUM, 0, -1) << ends;

                ASSERTV(LINE, ti,  0 == memcmp(buf, STR, SZ));

                // Check for overrun.
                ASSERTV(LINE, ti, XX == buf[SIZE - 1]);
                ASSERTV(LINE, ti,
                        0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));
            }
        }

        // Verify that nothing is written to a bad stream.
        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const int        LINE = DATA[ti].d_lineNum;
            const Obj::Enum  ENUM = DATA[ti].d_enumerator;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            ostrstream out(buf, sizeof buf);  out.setstate(ios::badbit);
            out << ENUM;

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(buf, CTRL, SIZE));
        }

        // Verify '<<' operator signature.
        {
            using namespace btlmt;
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Obj::Enum);

            const FuncPtr FP = &operator<<;
            (void) FP;  // quash potential compiler warning
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'btlmt::ChannelStatus::Enum' AND 'btlmt::ChannelStatus::toAscii'
        //
        // Concerns:
        //: 1 The enumerators have the correct values.
        //:
        //: 2 The 'toAscii' method returns the expected string representation
        //:   for each enumerator.
        //:
        //: 3 The 'toAscii' method returns a distinguished string when passed
        //:   an out-of-band value.
        //:
        //: 4 The string returned by 'toAscii' is non-modifiable.
        //:
        //: 5 The 'toAscii' method has the expected signature.
        //
        // Plan:
        //: 1 Use table-based approach, for each enumerator:
        //:
        //:   1 Verify that it has the correct value.  (C-1)
        //:
        //:   2 Verify that the 'toAscii' method returns the expected string.
        //:     (C-2)
        //:
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //:
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4, C-5)
        //
        // Testing:
        //   enum ChannelStatus::Enum { ... };
        //   const char *ChannelStatus::toAscii(val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "'ChannelStatus::Enum' AND 'ChannelStatus::toAscii'"
            << endl
            << "=================================================="
            << endl;


        typedef btlmt::ChannelStatus Obj;

        static const struct {
            int         d_lineNum;     // source line number
            Obj::Enum   d_enumerator;  // enumerator
            int         d_expValue;    // expected value
            const char *d_expString;   // expected ascii value
        } DATA[] = {
         //line  enumerator                      value  ascii
         //----  ------------------------------  -----  --------------------
         { L_,   Obj::e_SUCCESS,                0,  "SUCCESS"            },
         { L_,   Obj::e_CACHE_OVERFLOW,        -1,  "CACHE_OVERFLOW"     },
         { L_,   Obj::e_CACHE_HIGHWATER,       -2,  "CACHE_HIGHWATER"    },
         { L_,   Obj::e_READ_CHANNEL_DOWN,     -6,  "READ_CHANNEL_DOWN"  },
         { L_,   Obj::e_WRITE_CHANNEL_DOWN,    -3,  "WRITE_CHANNEL_DOWN" },
         { L_,   Obj::e_ENQUEUE_HIGHWATER,     -4,  "ENQUEUE_HIGHWATER"  },
         { L_,   Obj::e_UNKNOWN_ID,            -5,  "UNKNOWN_ID"         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // Verify enumerators.
        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const Obj::Enum  ENUM = DATA[ti].d_enumerator;
            const int        VAL  = DATA[ti].d_expValue;
            const char      *STR  = DATA[ti].d_expString;

            ASSERTV(ENUM, VAL, VAL == ENUM);

            const char *result = Obj::toAscii(ENUM);

            ASSERTV(STR, result, 0 == strcmp(result, STR));
        }

        // Verify out-of-band values.
        const char *result = Obj::toAscii(static_cast<Obj::Enum>(1));
        ASSERTV(result, 0 == strcmp(UNKNOWN_FORMAT, result));


        // Verify 'toAscii' signature.
        {
            typedef const char *(*FuncPtr)(Obj::Enum);

            const FuncPtr FP = &Obj::toAscii;
            (void) FP;  // quash potential compiler warning
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
