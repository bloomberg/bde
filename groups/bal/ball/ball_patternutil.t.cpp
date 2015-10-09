// ball_patternutil.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_patternutil.h>

#include <bsl_cstdlib.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

#include <bslim_testutil.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The utility functions will be tested using a table-based approach.  A table
// stores both varied input data to the function and the corresponding
// expected results.  The correctness of the implementation is checked by
// comparing results returned by the function with the expected results stored
// in the table.
//-----------------------------------------------------------------------------
// [ 1] static bool isMatch(const char *i, const char *p);
// [ 1] static bool isValidPattern(const char *p);
//-----------------------------------------------------------------------------
// [ 2] STATE MACHINE TEST
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

typedef ball::PatternUtil Util;

static const struct {
    int         d_line;         // line number
    const char *d_inputString;  // input string
    const char *d_pattern;      // pattern
    int         d_isValid;      // valid flag
    bool        d_value;        // expected value
} DATA[] = {
    // Rows marked with 'ADD' indicate test data that added in order for every
    // state to be visited at least once (as found out by the state
    //
    // line   input string      pattern     validity   expected value
    // ----   ------------      -------     --------   --------------

    // depth = 0
    {  L_,    "",               "",          1,        1         },

    // depth = 1
    {  L_,    "A",              "",          1,        0         },
    {  L_,    "*",              "",          1,        0         },  // ADD
    {  L_,    "\\",             "",          1,        0         },  // ADD
    {  L_,    "",               "A",         1,        0         },
    {  L_,    "",               "*",         1,        1         },
    {  L_,    "",               "\\",        0,        0         },  // ADD
    {  L_,    "",               "\\*",       1,        0         },
    {  L_,    "",               "\\\\",      1,        0         },

    // depth = 2
    {  L_,    "AA",             "",          1,        0         },
    {  L_,    "A",              "A",         1,        1         },
    {  L_,    "A",              "B",         1,        0         },
    {  L_,    "A",              "*",         1,        1         },
    {  L_,    "A",              "\\",        0,        0         },  // ADD
    {  L_,    "A",              "\\*",       1,        0         },
    {  L_,    "A",              "\\\\",      1,        0         },
    {  L_,    "*",              "A",         1,        0         },
    {  L_,    "*",              "*",         1,        1         },
    {  L_,    "*",              "\\",        0,        0         },  // ADD
    {  L_,    "*",              "\\*",       1,        1         },
    {  L_,    "*",              "\\\\",      1,        0         },
    {  L_,    "\\",             "A",         1,        0         },
    {  L_,    "\\",             "*",         1,        1         },
    {  L_,    "\\",             "\\*",       1,        0         },
    {  L_,    "\\",             "\\\\",      1,        1         },
    {  L_,    "",               "A*",        1,        0         },
    {  L_,    "",               "*B",        0,        0         },  // ADD
    {  L_,    "",               "**",        0,        0         },
    {  L_,    "",               "*\\",       0,        0         },  // ADD
    {  L_,    "",               "\\A",       0,        0         },  // ADD

    // depth = 3
    {  L_,    "AAA",            "",          1,        0         },
    {  L_,    "AA",             "A",         1,        0         },
    {  L_,    "A*",             "A",         1,        0         },
    {  L_,    "A*",             "*",         1,        1         },
    {  L_,    "**",             "*",         1,        1         },
    {  L_,    "*\\",            "*",         1,        1         },
    {  L_,    "\\\\",           "\\",        0,        0         },
    {  L_,    "\\*",            "\\",        0,        0         },

    {  L_,    "A",              "AA",        1,        0         },
    {  L_,    "A",              "A*",        1,        1         },
    {  L_,    "B",              "A*",        1,        0         },
    {  L_,    "A",              "A\\*",      1,        0         },
    {  L_,    "A",              "A\\\\",     1,        0         },
    {  L_,    "A",              "*A",        0,        0         },
    {  L_,    "A",              "*B",        0,        0         },  // ADD
    {  L_,    "A",              "**",        0,        0         },
    {  L_,    "A",              "*\\*",      0,        0         },
    {  L_,    "A",              "*\\\\",     0,        0         },
    {  L_,    "A",              "\\**",      1,        0         },
    {  L_,    "A",              "\\*\\*",    1,        0         },
    {  L_,    "A",              "\\*\\*",    1,        0         },
    {  L_,    "A",              "\\A",       0,        0         },  // ADD
    {  L_,    "A",              "\\B",       0,        0         },  // ADD
    {  L_,    "A",              "\\\\A",     1,        0         },
    {  L_,    "A",              "\\\\*",     1,        0         },
    {  L_,    "A",              "\\\\\\*",   1,        0         },
    {  L_,    "A",              "\\\\\\\\",  1,        0         },

    {  L_,    "*",              "AA",        1,        0         },
    {  L_,    "*",              "A*",        1,        0         },
    {  L_,    "*",              "A\\*",      1,        0         },
    {  L_,    "*",              "A\\\\",     1,        0         },
    {  L_,    "*",              "*A",        0,        0         },
    {  L_,    "*",              "**",        0,        0         },
    {  L_,    "*",              "*\\*",      0,        0         },
    {  L_,    "*",              "*\\\\",     0,        0         },
    {  L_,    "*",              "\\A",       0,        0         },  // ADD
    {  L_,    "*",              "\\*A",      1,        0         },
    {  L_,    "*",              "\\**",      1,        1         },
    {  L_,    "*",              "\\*\\*",    1,        0         },
    {  L_,    "*",              "\\*\\*",    1,        0         },
    {  L_,    "*",              "\\\\A",     1,        0         },
    {  L_,    "*",              "\\\\*",     1,        0         },
    {  L_,    "*",              "\\\\\\*",   1,        0         },
    {  L_,    "*",              "\\\\\\\\",  1,        0         },

    {  L_,    "\\*",            "AA",        1,        0         },
    {  L_,    "\\*",            "A*",        1,        0         },
    {  L_,    "\\*",            "A\\*",      1,        0         },
    {  L_,    "\\*",            "A\\\\",     1,        0         },
    {  L_,    "\\*",            "*A",        0,        0         },
    {  L_,    "\\*",            "**",        0,        0         },
    {  L_,    "\\*",            "*\\*",      0,        0         },
    {  L_,    "\\*",            "*\\\\",     0,        0         },
    {  L_,    "\\*",            "\\A",       0,        0         },  // ADD
    {  L_,    "\\*",            "\\*A",      1,        0         },
    {  L_,    "\\*",            "\\**",      1,        0         },
    {  L_,    "\\*",            "\\*\\*",    1,        0         },
    {  L_,    "\\*",            "\\*\\*",    1,        0         },
    {  L_,    "\\*",            "\\\\A",     1,        0         },
    {  L_,    "\\*",            "\\\\*",     1,        1         },
    {  L_,    "\\*",            "\\\\\\*",   1,        1         },
    {  L_,    "\\*",            "\\\\\\\\",  1,        0         },

    {  L_,    "\\\\",           "AA",        1,        0         },
    {  L_,    "\\\\",           "A*",        1,        0         },
    {  L_,    "\\\\",           "A\\*",      1,        0         },
    {  L_,    "\\\\",           "A\\\\",     1,        0         },
    {  L_,    "\\\\",           "*A",        0,        0         },
    {  L_,    "\\\\",           "**",        0,        0         },
    {  L_,    "\\\\",           "*\\*",      0,        0         },
    {  L_,    "\\\\",           "*\\\\",     0,        0         },
    {  L_,    "\\\\",           "\\*A",      1,        0         },
    {  L_,    "\\\\",           "\\**",      1,        0         },
    {  L_,    "\\\\",           "\\*\\*",    1,        0         },
    {  L_,    "\\\\",           "\\*\\*",    1,        0         },
    {  L_,    "\\\\",           "\\\\A",     1,        0         },
    {  L_,    "\\\\",           "\\\\*",     1,        1         },
    {  L_,    "\\\\",           "\\\\\\*",   1,        0         },
    {  L_,    "\\\\",           "\\\\\\\\",  1,        1         },

    {  L_,    "",               "***",       0,        0         },
    {  L_,    "",               "**\\*",     0,        0         },

    // depth >= 4
    {  L_,    "abcd",           "*",         1,        1         },
    {  L_,    "abcd",           "a*",        1,        1         },
    {  L_,    "abcd",           "ab*",       1,        1         },
    {  L_,    "abcd",           "abc*",      1,        1         },
    {  L_,    "abcd",           "abcd",      1,        1         },
    {  L_,    "abcd",           "\\*",       1,        0         },
    {  L_,    "abcd",           "a\\*",      1,        0         },
    {  L_,    "abcd",           "ab\\*",     1,        0         },
    {  L_,    "abcd",           "abc\\*",    1,        0         },
    {  L_,    "abcd",           "abcd\\*",   1,        0         },
};

const int NUM_DATA = sizeof DATA / sizeof *DATA;

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BALL_PATTERNUTIL_TEST_CASE_2
{

enum {
    OTHER,         // anything other than '*', '\', and 0
    STAR,          // '*'
    BACKSLASH,     // '\'
    END,           // 0
    NUMCLASSES     // number of equivalent classes
};

enum {
    S0,  // no mismatch so far
    S1,  // saw a '\' in 'pattern'
    S2,  // saw a '*' in 'pattern'
    RT,  // return 'true'
    RF   // return 'false'
};

static const int TOKEN[] = {
    END, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, STAR, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BACKSLASH, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static struct {
    int d_nextState;      // next state
    int d_advanceInput;   // 1: advance to next char; 0: stay
    int d_visited;        // if this state has been visited
} ACTION[][3] = {
    //     S0                 S1                 S2                P  Input
    // ------------       ------------       ------------          -  -----
    {  {  S0,  1,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // a   a
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // a   b
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // a   *
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // a   '\'
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // a   END

    {  {  S2,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // *   a
    {  {  S2,  0,  0  }, {  S0,  1,  0  }, {  RF,  0,  0  }  }, // *   *
    {  {  S2,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // *   '\'
    {  {  S2,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // *   END

    {  {  S1,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // \   a
    {  {  S1,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // \   *
    {  {  S1,  0,  0  }, {  S0,  1,  0  }, {  RF,  0,  0  }  }, // \   '\'
    {  {  S1,  0,  0  }, {  RF,  0,  0  }, {  RF,  0,  0  }  }, // \   END

    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RT,  0,  0  }  }, // END a
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RT,  0,  0  }  }, // END *
    {  {  RF,  0,  0  }, {  RF,  0,  0  }, {  RT,  0,  0  }  }, // END '\'
    {  {  RT,  0,  0  }, {  RF,  0,  0  }, {  RT,  0,  0  }  }, // END END
};

bool isMatch(const char *input, const char *pattern)
{
    int state = S0;

    while (state < RT) {
        int p = TOKEN[*pattern];
        int i = TOKEN[*input];
        int index = p * NUMCLASSES + i + 1
                      - (p == OTHER && *pattern == *input);
        ACTION[index][state].d_visited = 1;
        input += ACTION[index][state].d_advanceInput;
        state = ACTION[index][state].d_nextState;
        ++pattern;
    }

    return state == RT;
}

}  // close namespace BALL_PATTERNUTIL_TEST_CASE_2

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
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and adjust the line lengths.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            ASSERT(ball::PatternUtil::isMatch("EQ", "EQ"));

            ASSERT(ball::PatternUtil::isMatch("EQ.MARKET", "EQ*"));
            ASSERT(ball::PatternUtil::isMatch("EQ", "EQ*"));

            ASSERT(false == ball::PatternUtil::isMatch("EQ.MARKET","EQ\\*"));
            ASSERT(ball::PatternUtil::isMatch("EQ*", "EQ\\*"));

            ASSERT(ball::PatternUtil::isMatch("\\EQ", "\\\\EQ"));
            ASSERT(ball::PatternUtil::isMatch("E*Q", "E\\*Q"));

            ASSERT(false == ball::PatternUtil::isValidPattern("E\\Q"));
            ASSERT(false == ball::PatternUtil::isValidPattern("E*Q"));
            ASSERT(true  == ball::PatternUtil::isValidPattern("E\\\\Q"));
            ASSERT(true  == ball::PatternUtil::isValidPattern("E\\*Q"));

            ASSERT(false == ball::PatternUtil::isMatch("E\\Q", "E\\Q"));
            ASSERT(false == ball::PatternUtil::isMatch("E*Q",  "E*Q"));
            ASSERT(false == ball::PatternUtil::isMatch("ETQ",  "E*Q"));
       }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // STATE MACHINE TEST
        // Concerns:
        //   Our concern is that 'isMatch' should interpret the escape
        //   sequences and '*' correctly.
        //
        // Plan:
        //   We take 'A', '*', and '\' to represent three categories of
        //   characters in the input string, and 'A', '*', '\*', and '\\' to
        //   represent four categories of characters/sequences in the pattern.
        //   Depth ordered enumeration is used to test all combinations of
        //   input string and patterns up to a depth of 3 (except when
        //   length(inputString) == 2 and length(pattern) == 1).  A few
        //   representative combinations are selected for depth >= 4.
        //
        // Testing:
        //   static bool isMatch(const char *inputString, const char *pattern);
        //   static bool isValidPattern(const char *pattern);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nState Machine Test"
                          << "\n=================="
                          << endl;

        if (verbose) cout << "\nTest state-machine-based 'isMatch'." << endl;

        using namespace BALL_PATTERNUTIL_TEST_CASE_2;
        ASSERT(NUMCLASSES * NUMCLASSES + 1
                                          == sizeof ACTION / sizeof ACTION[0]);

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVerbose) {
                P_(DATA[i].d_line);
                P_(DATA[i].d_inputString);
                P_(DATA[i].d_pattern);
                P_(DATA[i].d_isValid);
                P(DATA[i].d_value);
            }
            LOOP_ASSERT(DATA[i].d_line,
                        DATA[i].d_value == isMatch(DATA[i].d_inputString,
                                                   DATA[i].d_pattern));
        }

        if (verbose) cout << "\nVerify that every state has been visited "
                          << "at least once." << endl;

        for (int i = 0; i < NUMCLASSES * NUMCLASSES + 1; ++i) {
            for (int j = 0; j < RT; ++j) {
                LOOP2_ASSERT(i, j, ACTION[i][j].d_visited);
            }
        }

        if (verbose) cout << "\nCompare the efficiency of two 'isMatch' "
                          << "implementations." << endl;

        bsls::TimeInterval startTime;
        bsls::TimeInterval endTime;

        startTime = bsls::SystemTime::nowRealtimeClock();

        for (int j = 0; j < 10000; ++j) {
            for (int i = 0; i < NUM_DATA; ++i) {
                Util::isMatch(DATA[i].d_inputString, DATA[i].d_pattern);
            }
        }

        endTime = bsls::SystemTime::nowRealtimeClock();

        if (verbose) cout << "\nElapsed time for 'PatternUtil::isMatch': "
                          << (endTime.totalMicroseconds()
                              - startTime.totalMicroseconds()) / 1000000.0
                          << endl;

        startTime = bsls::SystemTime::nowRealtimeClock();

        for (int j = 0; j < 10000; ++j) {
            for (int i = 0; i < NUM_DATA; ++i) {
                isMatch(DATA[i].d_inputString, DATA[i].d_pattern);
            }
        }

        endTime = bsls::SystemTime::nowRealtimeClock();

        if (verbose) cout << "\nElapsed time for state-machine-based "
                          << "'isMatch': "
                          << (endTime.totalMicroseconds()
                              - startTime.totalMicroseconds()) / 1000000.0
                          << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING isMatch and isValidPattern
        //
        // Concerns:
        //   Our concern is that 'isMatch' should interpret the escape
        //   sequences and '*' correctly.
        //
        // Plan:
        //   We take 'A', '*', and '\' to represent three categories of
        //   characters in the input string, and 'A', '*', '\*', and '\\' to
        //   represent four categories of characters/sequences in the pattern.
        //   Depth ordered enumeration is used to test all combinations of
        //   input string and patterns up to a depth of 3 (except when
        //   length(inputString) == 2 and length(pattern) == 1).  A few
        //   representative combinations are selected for depth >= 4.
        //
        // Testing:
        //   static bool isMatch(const char *inputString, const char *pattern);
        //   static bool isValidPattern(const char *pattern);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isMatch' and 'isValidPattern'"
                          << "\n======================================"
                          << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
           if (veryVerbose) {
               P_(DATA[i].d_line);
               P_(DATA[i].d_inputString);
               P_(DATA[i].d_pattern);
               P_(DATA[i].d_isValid);
               P(DATA[i].d_value);
           }
           LOOP_ASSERT(DATA[i].d_line,
                       DATA[i].d_value == Util::isMatch(DATA[i].d_inputString,
                                                        DATA[i].d_pattern));
           LOOP_ASSERT(DATA[i].d_line,
                       DATA[i].d_isValid ==
                           Util::isValidPattern(DATA[i].d_pattern));
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
