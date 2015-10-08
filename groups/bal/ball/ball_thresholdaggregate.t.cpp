// ball_thresholdaggregate.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_thresholdaggregate.h>

#include <bslim_testutil.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a typical and simple value-semantic component,
// so we simply follow the standard 10-case test suite, but with those
// apparatus-based test cases empty.
//-----------------------------------------------------------------------------
// [12] static int hash(const ball::ThresholdAggregate&, int size);
// [11] static bool areValidThresholdLevels(int, int, int, int);
// [ 2] ball::ThresholdAggregate(int, int, int, int);
// [ 7] ball::ThresholdAggregate(const ball::TA&, bdema::Alct * = 0);
// [ 2] ~ball::ThresholdAggregate();
// [10] int setLevels(int, int, int, int);
// [ 9] const ball::TA& operator=(const ball::TA& other);
// [ 4] int recordLevel() const;
// [ 4] int passLevel() const;
// [ 4] int triggerLevel() const;
// [ 4] int triggerAllLevel() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] bool operator==(const ball::TA& lhs, const ball::TA& rhs);
// [ 6] bool operator!=(const ball::TA& lhs, const ball::TA& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::TA&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] UNUSED
// [ 8] UNUSED
// [13] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::ThresholdAggregate Obj;

#define VA 192, 160, 128, 96
#define VB 160, 128, 96,  64
#define VC 128, 96,  64,  32
#define VD 96,  64,  32,  0
#define VE 255, 255, 255, 255

#define VA1 192
#define VA2 160
#define VA3 128
#define VA4 96

#define VB1 160
#define VB2 128
#define VB3 96
#define VB4 64

#define VC1 128
#define VC2 96
#define VC3 64
#define VC4 32

#define VD1 96
#define VD2 64
#define VD3 32
#define VD4 0

static const struct {
    int d_line;            // line number
    int d_recordLevel;     // record level
    int d_passLevel;       // pass level
    int d_triggerLevel;    // trigger level
    int d_triggerAllLevel; // trigger all level
    int d_isValidFlag;     // if the above levels are valid
} DATA[] = {
    ///line    record    pass     trigger  triggerAll is
    ///no.     level     level     level     level    valid
    ///----    ------    ------    ------    -----    --------
    {  L_,     0,        0,        0,        0,       1        },
    {  L_,     1,        0,        0,        0,       1        },
    {  L_,     0,        1,        0,        0,       1        },
    {  L_,     0,        0,        1,        0,       1        },
    {  L_,     0,        0,        0,        1,       1        },
    {  L_,     16,       32,       48,       64,      1        },
    {  L_,     64,       48,       32,       16,      1        },
    {  L_,     16,       32,       64,       48,      1        },
    {  L_,     16,       48,       32,       64,      1        },
    {  L_,     32,       16,       48,       64,      1        },
    {  L_,     255,      0,        0,        0,       1        },
    {  L_,     0,        255,      0,        0,       1        },
    {  L_,     0,        0,        255,      0,       1        },
    {  L_,     0,        0,        0,        255,     1        },
    {  L_,     255,      255,      255,      255,     1        },
    {  L_,     256,      0,        0,        0,       0        },
    {  L_,     0,        256,      0,        0,       0        },
    {  L_,     0,        0,        256,      0,       0        },
    {  L_,     0,        0,        0,        256,     0        },
    {  L_,     256,      256,      256,      256,     0        },
    {  L_,     INT_MAX,  0,        0,        0,       0        },
    {  L_,     0,        INT_MAX,  0,        0,       0        },
    {  L_,     0,        0,        INT_MAX,  0,       0        },
    {  L_,     0,        0,        0,        INT_MAX, 0        },
    {  L_,     INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX, 0        },
    {  L_,     -1,       0,        0,        0,       0        },
    {  L_,     0,        -1,       0,        0,       0        },
    {  L_,     0,        0,        -1,       0,       0        },
    {  L_,     0,        0,        0,        -1,      0        },
    {  L_,     -1,       -1,       -1,       -1,      0        },
    {  L_,     INT_MIN,  0,        0,        0,       0        },
    {  L_,     0,        INT_MIN,  0,        0,       0        },
    {  L_,     0,        0,        INT_MIN,  0,       0        },
    {  L_,     0,        0,        0,        INT_MIN, 0        },
    {  L_,     INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN, 0        }
};
const int NUM_DATA = sizeof DATA / sizeof *DATA;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        ball::ThresholdAggregate levels(192, 160, 128, 96);
        ASSERT(192 == levels.recordLevel());
        ASSERT(160 == levels.passLevel());
        ASSERT(128 == levels.triggerLevel());
        ASSERT(96  == levels.triggerAllLevel());

        levels.setLevels(160, 128, 96, 64);
        ASSERT(160 == levels.recordLevel());
        ASSERT(128 == levels.passLevel());
        ASSERT(96  == levels.triggerLevel());
        ASSERT(64  == levels.triggerAllLevel());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION (VALUE):
        //
        // Concerns:
        //   (1) All fields must be incorporated in the hash value.
        //   (2) The hash value must be the same across all platforms for a
        //       given input.
        //
        // Plan:
        //   Specifying a set of test vectors consisting of distinct valid
        //   test values having subtle difference and hash sizes including
        //   powers of two and primes.  For each value and hash size, verify
        //   that the 'hash' function returns expected value.
        //
        //   7 is the smallest hash table size and 1610612741 is largest size
        //   (that can fit into an int) used by stlport hashtable.
        //
        // Testing:
        //   static int hash(const ball::ThresholdAggregate&, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting hash function"
                          << "\n====================="
                          << endl;

        static const struct {
            int d_line;            // line number
            int d_recordLevel;     // record level
            int d_passLevel;       // pass level
            int d_triggerLevel;    // trigger level
            int d_triggerAllLevel; // trigger all level
            int d_size;            // hash size
            int d_hash;            // hash value
        } HDATA[] = {
            ///line record pass  trigger trAll    hash         hash
            ///no.  level  level  level  level    size         value
            ///---- ------ -----  ------ -----    ----         -----
            {  L_,  0,     0,     0,     0,       256,         233         },
            {  L_,  1,     0,     0,     0,       256,         168         },
            {  L_,  0,     1,     0,     0,       256,         19          },
            {  L_,  0,     0,     1,     0,       256,         217         },
            {  L_,  0,     0,     0,     1,       256,         214         },
            {  L_,  16,    32,    48,    64,      256,         225         },
            {  L_,  64,    48,    32,    16,      256,         176         },
            {  L_,  16,    32,    64,    48,      256,         234         },
            {  L_,  16,    48,    32,    64,      256,         31          },
            {  L_,  32,    16,    48,    64,      256,         27          },
            {  L_,  255,   0,     0,     0,       256,         116         },
            {  L_,  0,     255,   0,     0,       256,         8           },
            {  L_,  0,     0,     255,   0,       256,         191         },
            {  L_,  0,     0,     0,     255,     256,         143         },
            {  L_,  255,   255,   255,   255,     256,         89          },
            {  L_,  0,     0,     0,     0,       65536,       48105       },
            {  L_,  1,     0,     0,     0,       65536,       53928       },
            {  L_,  0,     1,     0,     0,       65536,       19731       },
            {  L_,  0,     0,     1,     0,       65536,       40409       },
            {  L_,  0,     0,     0,     1,       65536,       36566       },
            {  L_,  16,    32,    48,    64,      65536,       59617       },
            {  L_,  64,    48,    32,    16,      65536,       24752       },
            {  L_,  16,    32,    64,    48,      65536,       6378        },
            {  L_,  16,    48,    32,    64,      65536,       29727       },
            {  L_,  32,    16,    48,    64,      65536,       32283       },
            {  L_,  255,   0,     0,     0,       65536,       1908        },
            {  L_,  0,     255,   0,     0,       65536,       54536       },
            {  L_,  0,     0,     255,   0,       65536,       60863       },
            {  L_,  0,     0,     0,     255,     65536,       63375       },
            {  L_,  255,   255,   255,   255,     65536,       39513       },
            {  L_,  0,     0,     0,     0,       7,           4           },
            {  L_,  1,     0,     0,     0,       7,           3           },
            {  L_,  0,     1,     0,     0,       7,           4           },
            {  L_,  0,     0,     1,     0,       7,           3           },
            {  L_,  0,     0,     0,     1,       7,           0           },
            {  L_,  16,    32,    48,    64,      7,           3           },
            {  L_,  64,    48,    32,    16,      7,           2           },
            {  L_,  16,    32,    64,    48,      7,           4           },
            {  L_,  16,    48,    32,    64,      7,           2           },
            {  L_,  32,    16,    48,    64,      7,           2           },
            {  L_,  255,   0,     0,     0,       7,           5           },
            {  L_,  0,     255,   0,     0,       7,           2           },
            {  L_,  0,     0,     255,   0,       7,           2           },
            {  L_,  0,     0,     0,     255,     7,           4           },
            {  L_,  255,   255,   255,   255,     7,           1           },
            {  L_,  0,     0,     0,     0,       1610612741,  694533092   },
            {  L_,  1,     0,     0,     0,       1610612741,  982241950   },
            {  L_,  0,     1,     0,     0,       1610612741,  373312777   },
            {  L_,  0,     0,     1,     0,       1610612741,  484416985   },
            {  L_,  0,     0,     0,     1,       1610612741,  1335135953  },
            {  L_,  16,    32,    48,    64,      1610612741,  579004636   },
            {  L_,  64,    48,    32,    16,      1610612741,  426860710   },
            {  L_,  16,    32,    64,    48,      1610612741,  925767904   },
            {  L_,  16,    48,    32,    64,      1610612741,  690648090   },
            {  L_,  32,    16,    48,    64,      1610612741,  688029211   },
            {  L_,  255,   0,     0,     0,       1610612741,  1006438250  },
            {  L_,  0,     255,   0,     0,       1610612741,  413390078   },
            {  L_,  0,     0,     255,   0,       1610612741,  455273919   },
            {  L_,  0,     0,     0,     255,     1610612741,  1538324362  },
            {  L_,  255,   255,   255,   255,     1610612741,  1025677913  },
        };

        const int NUM_HDATA = sizeof HDATA / sizeof *HDATA;

        for (int i = 0; i < NUM_HDATA; ++i) {
            int LINE = HDATA[i].d_line;

            Obj mX(HDATA[i].d_recordLevel,
                   HDATA[i].d_passLevel,
                   HDATA[i].d_triggerLevel,
                   HDATA[i].d_triggerAllLevel);
            const Obj& X = mX;

            int hash = Obj::hash(X, HDATA[i].d_size);
            if (veryVerbose) {
                cout << HDATA[i].d_recordLevel     << ", "
                     << HDATA[i].d_passLevel       << ", "
                     << HDATA[i].d_triggerLevel    << ", "
                     << HDATA[i].d_triggerAllLevel << ", "
                     << HDATA[i].d_size
                     << " ---> " << hash << endl;
            }
            LOOP_ASSERT(LINE, 0 <= hash);
            LOOP_ASSERT(LINE, hash < HDATA[i].d_size);
            LOOP_ASSERT(LINE, HDATA[i].d_hash == hash);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING STATIC 'areValidThresholdLevels' FUNCTION:
        //   The static 'areValidThresholdLevels' function should return
        //   correct value for both valid and invalid input.
        //
        // Plan:
        //   For a sequence of independent test values, verify that the
        //   function returns the expected value.
        //
        // Testing:
        //   static bool areValidThresholdLevels(int, int, int, int);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'areValidThresholdLevels' function"
                          << "\n=========================================="
                          << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P(DATA[i].d_triggerAllLevel);
            }

            bool isValidFlag =
                Obj::areValidThresholdLevels(DATA[i].d_recordLevel,
                                             DATA[i].d_passLevel,
                                             DATA[i].d_triggerLevel,
                                             DATA[i].d_triggerAllLevel);

            LOOP_ASSERT(LINE, isValidFlag == DATA[i].d_isValidFlag);

        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'setLevels' FUNCTION:
        //   The 'setLevels' function should correctly pass the specified
        //   value to the object.
        //
        // Plan:
        //   For a sequence of independent valid test values, use the
        //   constructor to create an object and then use the 'setLevels'
        //   function to set its value.  For the set of destination values
        //   that are valid, verify that the value has been correctly passed
        //   to the object.  Otherwise, verify that the original value of the
        //   object is retained.
        //
        // Testing:
        //   void setLevels(int, int, int, int);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'setLevels' function"
                          << "\n============================" << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE1 = DATA[i].d_line;

            for (int j = 0; j < NUM_DATA; ++j) {
                const int LINE2 = DATA[j].d_line;

                Obj mX(DATA[i].d_recordLevel,
                       DATA[i].d_passLevel,
                       DATA[i].d_triggerLevel,
                       DATA[i].d_triggerAllLevel);
                const Obj& X = mX;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(X);

                    P_(LINE2);
                    P_(DATA[j].d_recordLevel);
                    P_(DATA[j].d_passLevel);
                    P_(DATA[j].d_triggerLevel);
                    P(DATA[j].d_triggerAllLevel);
                }

                mX.setLevels(DATA[j].d_recordLevel,
                             DATA[j].d_passLevel,
                             DATA[j].d_triggerLevel,
                             DATA[j].d_triggerAllLevel);

                int k = DATA[j].d_isValidFlag ? j : i;
                LOOP2_ASSERT(LINE1,
                             LINE2,
                             X.recordLevel()     == DATA[k].d_recordLevel);
                LOOP2_ASSERT(LINE1,
                             LINE2,
                             X.passLevel()       == DATA[k].d_passLevel);
                LOOP2_ASSERT(LINE1,
                             LINE2,
                             X.triggerLevel()    == DATA[k].d_triggerLevel);
                LOOP2_ASSERT(LINE1,
                             LINE2,
                             X.triggerAllLevel() == DATA[k].d_triggerAllLevel);
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   const ball::TA& operator=(const ball::TA& other)
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assigment Operator"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nTesting non-alias case." << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE1 = DATA[i].d_line;

            for (int j = 0; j < NUM_DATA && DATA[j].d_isValidFlag; ++j) {
                const int LINE2 = DATA[j].d_line;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(DATA[i].d_recordLevel);
                    P_(DATA[i].d_passLevel);
                    P_(DATA[i].d_triggerLevel);
                    P(DATA[i].d_triggerAllLevel);

                    P_(LINE2);
                    P_(DATA[j].d_recordLevel);
                    P_(DATA[j].d_passLevel);
                    P_(DATA[j].d_triggerLevel);
                    P(DATA[j].d_triggerAllLevel);
                }

                Obj mX(DATA[i].d_recordLevel,
                       DATA[i].d_passLevel,
                       DATA[i].d_triggerLevel,
                       DATA[i].d_triggerAllLevel);
                const Obj& X = mX;

                Obj mY(DATA[j].d_recordLevel,
                       DATA[j].d_passLevel,
                       DATA[j].d_triggerLevel,
                       DATA[j].d_triggerAllLevel);
                const Obj& Y = mY;

                Obj mW(Y); const Obj& W = mW;
                mX = Y;
                ASSERT(W == X);
                ASSERT(W == Y);
            }
        }

        if (verbose) cout << "\nTesting alias case." << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P(DATA[i].d_triggerAllLevel);
            }

            Obj mX(DATA[i].d_recordLevel,
                   DATA[i].d_passLevel,
                   DATA[i].d_triggerLevel,
                   DATA[i].d_triggerAllLevel);
            const Obj& X = mX;

            Obj mW(X); const Obj& W = mW;
            mX = X;
            ASSERT(W == X);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'ball::ThresholdAggregate'.
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   ball::ThresholdAggregate(const ball::TA&, bdema::Alct * = 0)
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P(DATA[i].d_triggerAllLevel);
            }

            Obj w(DATA[i].d_recordLevel,
                  DATA[i].d_passLevel,
                  DATA[i].d_triggerLevel,
                  DATA[i].d_triggerAllLevel);
            const Obj& W = w;           // control

            Obj x(DATA[i].d_recordLevel,
                  DATA[i].d_passLevel,
                  DATA[i].d_triggerLevel,
                  DATA[i].d_triggerAllLevel);
            const Obj& X = x;

            Obj y(X); const Obj &Y = y;
            LOOP_ASSERT(LINE, X == W);
            LOOP_ASSERT(LINE, Y == W);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique object values that may have various
        //   minor or subtle differences.  Verify the correctness of
        //   'operator==' and 'operator!=' using all elements (u, v) of the
        //   cross product S X S.
        //
        // Testing:
        //   bool operator==(const ball::TA& lhs, const ball::TA& rhs)
        //   bool operator!=(const ball::TA& lhs, const ball::TA& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE1 = DATA[i].d_line;

            for (int j = 0; j < NUM_DATA && DATA[j].d_isValidFlag; ++j) {
                const int LINE2 = DATA[j].d_line;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(DATA[i].d_recordLevel);
                    P_(DATA[i].d_passLevel);
                    P_(DATA[i].d_triggerLevel);
                    P(DATA[i].d_triggerAllLevel);

                    P_(LINE2);
                    P_(DATA[j].d_recordLevel);
                    P_(DATA[j].d_passLevel);
                    P_(DATA[j].d_triggerLevel);
                    P(DATA[j].d_triggerAllLevel);
                }

                Obj mX(DATA[i].d_recordLevel,
                       DATA[i].d_passLevel,
                       DATA[i].d_triggerLevel,
                       DATA[i].d_triggerAllLevel);
                const Obj& X = mX;

                Obj mY(DATA[j].d_recordLevel,
                       DATA[j].d_passLevel,
                       DATA[j].d_triggerLevel,
                       DATA[j].d_triggerAllLevel);
                const Obj& Y = mY;

                ASSERT((i==j) == (X == Y));
                ASSERT((i!=j) == (X != Y));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'operator<<' AND 'print':
        //   The output operator and 'print' method should print out the value
        //   of objects in the expected format.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::TA&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'operator<<' and 'print'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        static const struct {
            int         d_line;            // line number
            int         d_recordLevel;     // record level
            int         d_passLevel;       // pass level
            int         d_triggerLevel;    // trigger level
            int         d_triggerAllLevel; // trigger all level
            const char *d_output;          // expected output format
        } DATA[] = {
            ///line rec   pass  trig  trAll expected
            ///no.  level level level level output
            ///---- ----- ----- ----  ----- --------
            {  L_,  0,    0,    0,    0,    "[  0  0  0  0 ] "         },
            {  L_,  1,    0,    0,    0,    "[  1  0  0  0 ] "         },
            {  L_,  64,   48,   32,   16,   "[  64  48  32  16 ] "     },
            {  L_,  255,  0,    0,    0,    "[  255  0  0  0 ] "       },
            {  L_,  255,  255,  255,  255,  "[  255  255  255  255 ] " },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P_(DATA[i].d_triggerAllLevel);
                P(DATA[i].d_output);
            }

            Obj mX(DATA[i].d_recordLevel,
                   DATA[i].d_passLevel,
                   DATA[i].d_triggerLevel,
                   DATA[i].d_triggerAllLevel);
            const Obj& X = mX;

            ostringstream os;
            os << X;
            LOOP_ASSERT(LINE, os.str() == DATA[i].d_output);
        }

        if (verbose) cout << "\nTesting 'print'." << endl;

        static const struct {
            int         d_line;            // line number
            int         d_level;           // tab level
            int         d_spacesPerLevel;  // spaces per level
           const char  *d_output;          // expected output format
        } PDATA[] = {
            // line level space expected
            // ---- ---- ----- -----------------------
            {  L_,   0,    -1, "[  192  160  128  96 ] "                  },
            {  L_,   1,    2,  "  [\n"
                               "    192\n"
                               "    160\n"
                               "    128\n"
                               "    96\n"
                               "  ]\n"                                    },
            {  L_,   -1,   -2, "[     192     160     128     96   ] "    },
        };

        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        for (int i = 0; i < NUM_PDATA; ++i) {
            int LINE = PDATA[i].d_line;

            const Obj X(VA);

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                P_(LINE);
                P_(X);
                P(PDATA[i].d_output);
                P_(os.str()); cout << endl;
            }

            LOOP_ASSERT(LINE, os.str() == PDATA[i].d_output);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Basic accessors must correctly return each individual field.
        //
        // Plan:
        //   For a sequence of independent valid test values, use the
        //   constructor to create an object having the specified value and
        //   then use the basic accessors to verify that they return the
        //   correct value.
        //
        // Testing:
        //   int recordLevel() const;
        //   int passLevel() const;
        //   int triggerLevel() const;
        //   int triggerAllLevel() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE             = DATA[i].d_line;
            const int ISVALIDFLAG      = DATA[i].d_isValidFlag;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P_(DATA[i].d_triggerAllLevel);
                P(ISVALIDFLAG);
            }

            const Obj X(DATA[i].d_recordLevel,
                        DATA[i].d_passLevel,
                        DATA[i].d_triggerLevel,
                        DATA[i].d_triggerAllLevel);

            LOOP_ASSERT(LINE, X.recordLevel()  == DATA[i].d_recordLevel);
            LOOP_ASSERT(LINE, X.passLevel()    == DATA[i].d_passLevel);
            LOOP_ASSERT(LINE, X.triggerLevel() == DATA[i].d_triggerLevel);
            LOOP_ASSERT(LINE, X.triggerAllLevel()
                                               == DATA[i].d_triggerAllLevel);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE TEST APPARATUS:
        //   Void for 'ball::ThresholdAggregate'.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   Primary manipulator should correctly pass the specified value to
        //   the object.
        //
        // Plan:
        //   For a sequence of independent valid test values, use the
        //   constructor to create an object having the specified
        //   value.  Verify, using the basic accessors, that the object has the
        //   expected value.
        //
        // Testing:
        //   ball::ThresholdAggregate(int, int, int, int);
        //   ~ball::ThresholdAggregate();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        for (int i = 0; i < NUM_DATA && DATA[i].d_isValidFlag; ++i) {
            const int LINE             = DATA[i].d_line;
            const int ISVALIDFLAG      = DATA[i].d_isValidFlag;

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_recordLevel);
                P_(DATA[i].d_passLevel);
                P_(DATA[i].d_triggerLevel);
                P_(DATA[i].d_triggerAllLevel);
                P(ISVALIDFLAG);
            }

            const Obj X(DATA[i].d_recordLevel,
                        DATA[i].d_passLevel,
                        DATA[i].d_triggerLevel,
                        DATA[i].d_triggerAllLevel);

            LOOP_ASSERT(LINE, X.recordLevel()  == DATA[i].d_recordLevel);
            LOOP_ASSERT(LINE, X.passLevel()    == DATA[i].d_passLevel);
            LOOP_ASSERT(LINE, X.triggerLevel() == DATA[i].d_triggerLevel);
            LOOP_ASSERT(LINE, X.triggerAllLevel()
                                               == DATA[i].d_triggerAllLevel);
        }

      } break;
      case 1: {
       // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the initializing and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using these test objects.  Invoke the primary
        //   manipulator [3, 6], copy constructor [2, 8], and assignment
        //   operator without [9, 10] and with [11] aliasing.  Use the direct
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while '0'
        //   denotes the default object value.
        //
        // 1.  Create an object x1 using VA.        { x1:VA }
        // 2.  Create an object x2 (copy from x1).  { x1:VA x2:VA }
        // 3.  Set x1 to VB.                        { x1:VB x2:VA }
        // 4.  Set x2 to VB.                        { x1:VB x2:VB }
        // 5.  Set x2 to VC.                        { x1:VB x2:VC }
        // 6.  Set x1 to VA.                        { x1:VA x2:VC }
        // 7.  Create an object x3 (with value VD). { x1:VA x2:VC x3:VD }
        // 8.  Create an object x4 (copy from x1).  { x1:VA x2:VC x3:VD x4:VA }
        // 9.  Assign x2 = x1.                      { x1:VA x2:VA x3:VD x4:VA }
        // 10. Assign x2 = x3.                      { x1:VA x2:VD x3:VD x4:VA }
        // 11. Assign x1 = x1 (aliasing).           { x1:VA x2:VD x3:VD x4:VA }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create  an object x1 using VA." << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        ASSERT(VA1 == X1.recordLevel());
        ASSERT(VA2 == X1.passLevel());
        ASSERT(VA3 == X1.triggerLevel());
        ASSERT(VA4 == X1.triggerAllLevel());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        ASSERT(VA1 == X2.recordLevel());
        ASSERT(VA2 == X2.passLevel());
        ASSERT(VA3 == X2.triggerLevel());
        ASSERT(VA4 == X2.triggerAllLevel());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        if (verbose) cout << "\n 3. Set x1 to VB." << endl;
        mX1.setLevels(VB);
        ASSERT(VB1 == X1.recordLevel());
        ASSERT(VB2 == X1.passLevel());
        ASSERT(VB3 == X1.triggerLevel());
        ASSERT(VB4 == X1.triggerAllLevel());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VB." << endl;
        mX2.setLevels(VB);
        ASSERT(VB1 == X2.recordLevel());
        ASSERT(VB2 == X2.passLevel());
        ASSERT(VB3 == X2.triggerLevel());
        ASSERT(VB4 == X2.triggerAllLevel());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VC." << endl;
        mX2.setLevels(VC);
        ASSERT(VC1 == X2.recordLevel());
        ASSERT(VC2 == X2.passLevel());
        ASSERT(VC3 == X2.triggerLevel());
        ASSERT(VC4 == X2.triggerAllLevel());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to VA." << endl;
        mX1.setLevels(VA);
        ASSERT(VA1 == X1.recordLevel());
        ASSERT(VA2 == X1.passLevel());
        ASSERT(VA3 == X1.triggerLevel());
        ASSERT(VA4 == X1.triggerAllLevel());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VD)."
                           << endl;
        Obj mX3(VD);  const Obj& X3 = mX3;
        ASSERT(VD1 == X3.recordLevel());
        ASSERT(VD2 == X3.passLevel());
        ASSERT(VD3 == X3.triggerLevel());
        ASSERT(VD4 == X3.triggerAllLevel());
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1);  const Obj& X4 = mX4;
        ASSERT(VA1 == X4.recordLevel());
        ASSERT(VA2 == X4.passLevel());
        ASSERT(VA3 == X4.triggerLevel());
        ASSERT(VA4 == X4.triggerAllLevel());
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT(VA1 == X2.recordLevel());
        ASSERT(VA2 == X2.passLevel());
        ASSERT(VA3 == X2.triggerLevel());
        ASSERT(VA4 == X2.triggerAllLevel());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT(VD1 == X2.recordLevel());
        ASSERT(VD2 == X2.passLevel());
        ASSERT(VD3 == X2.triggerLevel());
        ASSERT(VD4 == X2.triggerAllLevel());
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT(VA1 == X1.recordLevel());
        ASSERT(VA2 == X1.passLevel());
        ASSERT(VA3 == X1.triggerLevel());
        ASSERT(VA4 == X1.triggerAllLevel());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
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
