// bael_patternutil.t.cpp            -*-C++-*-

#include <bael_patternutil.h>

#include <bsl_cstdlib.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

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
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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

typedef bael_PatternUtil Util;

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

namespace BAEL_PATTERNUTIL_TEST_CASE_2
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

} // namespace BAEL_PATTERNUTIL_TEST_CASE_2

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
            ASSERT(bael_PatternUtil::isMatch("EQ", "EQ"));

            ASSERT(bael_PatternUtil::isMatch("EQ.MARKET", "EQ*"));
            ASSERT(bael_PatternUtil::isMatch("EQ", "EQ*"));

            ASSERT(false == bael_PatternUtil::isMatch("EQ.MARKET","EQ\\*"));
            ASSERT(bael_PatternUtil::isMatch("EQ*", "EQ\\*"));

            ASSERT(bael_PatternUtil::isMatch("\\EQ", "\\\\EQ"));
            ASSERT(bael_PatternUtil::isMatch("E*Q", "E\\*Q"));

            ASSERT(false == bael_PatternUtil::isValidPattern("E\\Q"));
            ASSERT(false == bael_PatternUtil::isValidPattern("E*Q"));
            ASSERT(true  == bael_PatternUtil::isValidPattern("E\\\\Q"));
            ASSERT(true  == bael_PatternUtil::isValidPattern("E\\*Q"));

            ASSERT(false == bael_PatternUtil::isMatch("E\\Q", "E\\Q"));
            ASSERT(false == bael_PatternUtil::isMatch("E*Q",  "E*Q"));
            ASSERT(false == bael_PatternUtil::isMatch("ETQ",  "E*Q"));
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

        using namespace BAEL_PATTERNUTIL_TEST_CASE_2;
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

        bdet_TimeInterval startTime;
        bdet_TimeInterval endTime;

        bdetu_SystemTime::loadSystemTimeDefault(&startTime);

        for (int j = 0; j < 10000; ++j) {
            for (int i = 0; i < NUM_DATA; ++i) {
                Util::isMatch(DATA[i].d_inputString, DATA[i].d_pattern);
            }
        }

        bdetu_SystemTime::loadSystemTimeDefault(&endTime);

        if (verbose) cout << "\nElapsed time for 'bael_PatternUtil::isMatch': "
                          << (endTime.totalMicroseconds()
                              - startTime.totalMicroseconds()) / 1000000.0
                          << endl;

        bdetu_SystemTime::loadSystemTimeDefault(&startTime);

        for (int j = 0; j < 10000; ++j) {
            for (int i = 0; i < NUM_DATA; ++i) {
                isMatch(DATA[i].d_inputString, DATA[i].d_pattern);
            }
        }

        bdetu_SystemTime::loadSystemTimeDefault(&endTime);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
