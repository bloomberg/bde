// bdepu_parserimputil.t.cpp              -*-C++-*-

#include <bdepu_parserimputil.h>

#include <bslma_testallocator.h>

#include <bsl_cstddef.h>      // size_t
#include <bsl_cstring.h>
#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This driver tests four types of utility functions implemented by
// 'bdepu_ParserImpUtil': 'parse', 'generate', 'skip', and 'append'.
//
// We use the standard table-based approach to testing, where we put both the
// input and the expected output in a table row, apply the tested function to
// the specified input, and verify that the result matches the expected value.
// The test vectors placed in each table are selected via enumeration over
// the appropriate region for each tested function.  Additionally, we use
// orthogonal perturbation to ensure that various suffixes do not hamper
// recognition of correct parse types.
//
// We use the following abbreviations: (for documentation purposes only)
//  'cchar'      for 'const char'
//  'b_ca'       for 'vector<char>'
//  'Int64'      for 'bsls_PlatformUtil::Int64'
//  'cInt64'     for 'const bsls_PlatformUtil::Int64'
//  'Uint64'     for 'bsls_PlatformUtil::Uint64'
//  'cUint64'    for 'const bsls_PlatformUtil::Uint64'
//
//-----------------------------------------------------------------------------
// [ 1] characterToDigit(char character, int base);
// [ 5] compactMultibyteChars(bsl::string *inOutString);
// [ 7] isIdentifier(const bsl::string& inString);
// [ 4] parseEscapeSequence(cchar **endPos, char *result, cchar *input);
// [ 8] parseIdentifier(cchar **endPos, bsl::string *result, cchar *input);
// [ 6] parseNull(cchar **endPos, cchar *inputString);
// [ 3] parseSignedInteger(cchar **, Int64 *, cchar *, int, cInt64, cInt64);
// [ 2] parseUnsignedInteger(cchar **, Int64 *, cchar *, int, cUint64, int);
// [11] skipRequiredCaselessToken(cchar **endPos, cchar *input, cchar tok);
// [11] skipRequiredCaselessToken(cchar **endPos, cchar *input, cchar *tok);
// [10] skipRequiredToken(cchar **endPos, cchar *input, char token);
// [10] skipRequiredToken(cchar **endPos, cchar *input, cchar *token);
// [ 9] skipWhiteSpace(const char **endPos, const char *inputString);
// [14] generateIdentifier(b_ca *buffer, cchar *val, int level, int spaces);
// [13] generateIndentation(b_ca *buffer, int level, int spacesPerLevel);
// [12] generateNull(vector<char> *buffer);
//-----------------------------------------------------------------------------
// TBD usage

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
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
#define T_()  cout << '\t' << flush;          // Print a tab character

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64 Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    bslma_TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // TESTING IDENTIFIER
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        // generateIdentifier(b_ca *buffer, cchar *val, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateIdentifier" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_val_p;           // specification identifier
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // specification level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec         val    level  SPL  exp
                //----  -----------  -----  -----  ---  -------------
                { L_,   "",          "",       0,   0,  ""            },
                { L_,   "",          "",       0,   1,  ""            },
                { L_,   "",          "",       1,   0,  ""            },
                { L_,   "",          "",       1,   1,  " "           },
                { L_,   "",          "",       2,   3,  "      "      },
                { L_,   "",          "",      -1,   3,  ""            },
                { L_,   "",          "x",      0,   0,  "x"           },
                { L_,   "",          "x",      0,   1,  "x"           },
                { L_,   "",          "x",      1,   0,  "x"           },
                { L_,   "",          "x",      1,   1,  " x"          },
                { L_,   "",          "x",      2,   3,  "      x"     },
                { L_,   "",          "x",     -1,   3,  "x"           },

                { L_,   "a",         "",       0,   0,  "a"           },
                { L_,   "a",         "",       0,   1,  "a"           },
                { L_,   "a",         "",       1,   0,  "a"           },
                { L_,   "a",         "",       1,   1,  "a "          },
                { L_,   "a",         "",       2,   3,  "a      "     },
                { L_,   "a",         "",      -1,   3,  "a"           },
                { L_,   "a",         "x",      0,   0,  "ax"          },
                { L_,   "a",         "x",      0,   1,  "ax"          },
                { L_,   "a",         "x",      1,   0,  "ax"          },
                { L_,   "a",         "x",      1,   1,  "a x"         },
                { L_,   "a",         "x",      2,   3,  "a      x"    },
                { L_,   "a",         "x",     -1,   3,  "ax"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *const VAL   = DATA[ti].d_val_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_ParserImpUtil::
                                  generateIdentifier(&buffer, VAL, LEVEL, SPL);
                const bsl::size_t LEN = bsl::strlen(EXP);
                LOOP_ASSERT(LINE, LEN == buffer.size());
                for (bsl::size_t i = 0; i < LEN; ++i) {
                    LOOP2_ASSERT(LINE, i, EXP[i] == buffer[i]);
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING INDENTATION
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateIndentation(b_ca *buffer, int level, int spacesPerLevel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateIndentation" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // specification level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec         level  SPL  exp
                //----  -----------  -----  ---  -------------
                { L_,   "",             0,   0,  ""            },
                { L_,   "",             0,   1,  ""            },
                { L_,   "",             1,   0,  ""            },
                { L_,   "",             1,   1,  " "           },
                { L_,   "",             2,   3,  "      "      },

                { L_,   "a",            0,   0,  "a"           },
                { L_,   "a",            0,   1,  "a"           },
                { L_,   "a",            1,   0,  "a"           },
                { L_,   "a",            1,   1,  "a "          },
                { L_,   "a",            2,   3,  "a      "     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_ParserImpUtil::generateIndentation(&buffer, LEVEL, SPL);
                const bsl::size_t LEN = bsl::strlen(EXP);
                LOOP_ASSERT(LINE, LEN == buffer.size());
                for (bsl::size_t i = 0; i < LEN; ++i) {
                    LOOP2_ASSERT(LINE, i, EXP[i] == buffer[i]);
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING GENERATE NULL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateNull(vector<char> *buffer);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateNull" << endl
                          << "====================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_exp_p;    // expected result
            } DATA[] = {
                //line  spec         exp
                //----  -----------  -------------
                { L_,   "",          "NULL"        },
                { L_,   "a",         "aNULL"       },
                { L_,   "ab",        "abNULL"      },
                { L_,   "abc",       "abcNULL"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_ParserImpUtil::generateNull(&buffer);
                const bsl::size_t LEN = bsl::strlen(EXP);
                LOOP_ASSERT(LINE, LEN == buffer.size());
                for (bsl::size_t i = 0; i < LEN; ++i) {
                    LOOP2_ASSERT(LINE, i, EXP[i] == buffer[i]);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING SKIP REQUIRED TOKEN
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        // skipRequiredCaselessToken(cchar **endPos, cchar *input, cchar tok);
        // skipRequiredCaselessToken(cchar **endPos, cchar *input, cchar *tok);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING skipRequiredToken" << endl
                          << "=========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_token_p;  // specification token
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
            } DATA[] = {
                //line  spec                   token  offset  fail
                //----  ---------------------  -----  ------  ----
                { L_,   "",                    "",        0,     0 },

                { L_,   "b",                   "a",       0,     1 },
                { L_,   "A",                   "a",       1,     0 },
                { L_,   "a",                   "",        0,     0 },
                { L_,   "a",                   "a",       1,     0 },
                { L_,   "a",                   "ab",      1,     1 },
                { L_,   "a",                   "abc",     1,     1 },

                { L_,   "ab",                  "",        0,     0 },
                { L_,   "ab",                  "a",       1,     0 },
                { L_,   "ab",                  "ab",      2,     0 },
                { L_,   "ab",                  "abc",     2,     1 },
                { L_,   "aB",                  "ab",      2,     0 },
                { L_,   "aB",                  "aB",      2,     0 },
                { L_,   "aB",                  "Ab",      2,     0 },

                { L_,   "abc",                 "",        0,     0 },
                { L_,   "abc",                 "a",       1,     0 },
                { L_,   "abc",                 "ab",      2,     0 },
                { L_,   "abc",                 "abc",     3,     0 },
                { L_,   "aBc",                 "ab",      2,     0 },
                { L_,   "aBc",                 "aBc",     3,     0 },
                { L_,   "aBc",                 "Abc",     3,     0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *const TOKEN = DATA[ti].d_token_p;
                const int NUM           = DATA[ti].d_offset;
                const int FAIL          = DATA[ti].d_fail;
                const int curLen        = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {
                    const char *endPos = 0;
                    int rv = bdepu_ParserImpUtil::
                               skipRequiredCaselessToken(&endPos, SPEC, TOKEN);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                }

                if (1 == bsl::strlen(TOKEN)) {
                    const char *endPos = 0;
                    int rv = bdepu_ParserImpUtil::
                              skipRequiredCaselessToken(&endPos, SPEC, *TOKEN);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING SKIP REQUIRED TOKEN
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   skipRequiredToken(cchar **endPos, cchar *input, char token);
        //   skipRequiredToken(cchar **endPos, cchar *input, cchar *token);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING skipRequiredToken" << endl
                          << "=========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_token_p;  // specification token
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
            } DATA[] = {
                //line  spec                   token  offset  fail
                //----  ---------------------  -----  ------  ----
                { L_,   "",                    "",        0,     0 },

                { L_,   "b",                   "a",       0,     1 },
                { L_,   "A",                   "a",       0,     1 },
                { L_,   "a",                   "",        0,     0 },
                { L_,   "a",                   "a",       1,     0 },
                { L_,   "a",                   "ab",      1,     1 },
                { L_,   "a",                   "abc",     1,     1 },

                { L_,   "ab",                  "",        0,     0 },
                { L_,   "ab",                  "a",       1,     0 },
                { L_,   "ab",                  "ab",      2,     0 },
                { L_,   "ab",                  "abc",     2,     1 },
                { L_,   "aB",                  "ab",      1,     1 },
                { L_,   "aB",                  "aB",      2,     0 },
                { L_,   "aB",                  "Ab",      0,     1 },

                { L_,   "abc",                 "",        0,     0 },
                { L_,   "abc",                 "a",       1,     0 },
                { L_,   "abc",                 "ab",      2,     0 },
                { L_,   "abc",                 "abc",     3,     0 },
                { L_,   "aBc",                 "ab",      1,     1 },
                { L_,   "aBc",                 "aBc",     3,     0 },
                { L_,   "aBc",                 "Abc",     0,     1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *const TOKEN = DATA[ti].d_token_p;
                const int NUM           = DATA[ti].d_offset;
                const int FAIL          = DATA[ti].d_fail;
                const int curLen        = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {
                    const char *endPos = 0;
                    int rv = bdepu_ParserImpUtil::
                                       skipRequiredToken(&endPos, SPEC, TOKEN);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                }

                if (1 == bsl::strlen(TOKEN)) {
                    const char *endPos = 0;
                    int rv = bdepu_ParserImpUtil::
                                      skipRequiredToken(&endPos, SPEC, *TOKEN);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING SKIP WHITE SPACE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   skipWhiteSpace(const char **endPos, const char *inputString);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING skipWhiteSpace" << endl
                          << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
            } DATA[] = {
                //line  spec                   offset  fail
                //----  ---------------------  ------  ----
                { L_,   "",                         0,    0 },

                { L_,   "a",                        0,    0 },
                { L_,   "/",                        0,    0 },
                { L_,   " ",                        1,    0 },
                { L_,   "\t",                       1,    0 },
                { L_,   "\n",                       1,    0 },

                { L_,   "  ",                       2,    0 },
                { L_,   "//",                       2,    0 },
                { L_,   "/*",                       2,    1 },

                { L_,   "   ",                      3,    0 },
                { L_,   "//a",                      3,    0 },

                { L_,   "    ",                     4,    0 },
                { L_,   "//ab",                     4,    0 },
                { L_,   "/**/",                     4,    0 },
                { L_,   "//\na",                    3,    0 },

                { L_,   "/**/a",                    4,    0 },
                { L_,   "/*\n*/",                   5,    0 },

                { L_,   "/*\n*/a",                  5,    0 },
                { L_,   "/*//*/",                   6,    0 },

                { L_,   "/*//*/a",                  6,    0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int curLen       = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                const char *endPos = 0;
                int rv = bdepu_ParserImpUtil::skipWhiteSpace(&endPos, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PARSE IDENTIFIER
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //  parseIdentifier(cchar **endPos, bsl::string *result, cchar *input);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseIdentifier" << endl
                          << "==========================" << endl;

        {
            const char *INITIAL_VALUE_1 = " 37";  // first initial value
            const char *INITIAL_VALUE_2 = "-58";  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                const char *d_value;    // expected return value
            } DATA[] = {
                //line  spec            offset  fail  value
                //----  --------------  ------  ----  ------------
                { L_,   "",                 0,    1,               },

                { L_,   " ",                0,    1,               },
                { L_,   "a",                1,    0,  "a"          },
                { L_,   "z",                1,    0,  "z"          },
                { L_,   "A",                1,    0,  "A"          },
                { L_,   "Z",                1,    0,  "Z"          },
                { L_,   "_",                1,    0,  "_"          },
                { L_,   "0",                0,    1,               },
                { L_,   "9",                0,    1,               },

                { L_,   "a.",               1,    0,  "a"          },
                { L_,   "aa",               2,    0,  "aa"         },
                { L_,   "a_",               2,    0,  "a_"         },
                { L_,   "a1",               2,    0,  "a1"         },
                { L_,   "a9",               2,    0,  "a9"         },
                { L_,   "_1",               2,    0,  "_1"         },
                { L_,   "_9",               2,    0,  "_9"         },
                { L_,   "__",               2,    0,  "__"         },

                { L_,   "aa.",              2,    0,  "aa"         },
                { L_,   "a_b",              3,    0,  "a_b"        },
                { L_,   "a__",              3,    0,  "a__"        },
                { L_,   "__a",              3,    0,  "__a"        },

                { L_,   "aaa.",             3,    0,  "aaa"        },
                { L_,   "a_b_",             4,    0,  "a_b_"       },
                { L_,   "a_3_",             4,    0,  "a_3_"       },
                { L_,   "a__3",             4,    0,  "a__3"       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int NUM           = DATA[ti].d_offset;
                const int FAIL          = DATA[ti].d_fail;
                const char *const VALUE = DATA[ti].d_value;
                const int curLen        = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        bsl::string result(INITIAL_VALUE_1, &testAllocator);
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::parseIdentifier(&endPos,
                                                                      &result,
                                                                      buffer);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                        LOOP2_ASSERT(LINE, si,
                            FAIL || bdepu_ParserImpUtil::isIdentifier(result));
                    }

                    {  // test with second initial value
                        bsl::string result(INITIAL_VALUE_2, &testAllocator);
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::parseIdentifier(&endPos,
                                                                      &result,
                                                                      buffer);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_2 : VALUE));
                        LOOP2_ASSERT(LINE, si,
                            FAIL || bdepu_ParserImpUtil::isIdentifier(result));
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ISIDENTIFIER
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   isIdentifier(const bsl::string& inString);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING isIdentifier" << endl
                          << "====================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_exp;      // expected return value
            } DATA[] = {
                //line  spec                   exp
                //----  ---------------------  ---
                { L_,   "",                      0 },

                { L_,   " ",                     0 },
                { L_,   "a",                     1 },
                { L_,   "z",                     1 },
                { L_,   "A",                     1 },
                { L_,   "Z",                     1 },
                { L_,   "_",                     1 },
                { L_,   "0",                     0 },
                { L_,   "9",                     0 },

                { L_,   "_ ",                    0 },
                { L_,   "a ",                    0 },
                { L_,   "aa",                    1 },
                { L_,   "a_",                    1 },
                { L_,   "a1",                    1 },
                { L_,   "a9",                    1 },
                { L_,   "_1",                    1 },
                { L_,   "_9",                    1 },
                { L_,   "__",                    1 },

                { L_,   "ab ",                   0 },
                { L_,   "a_b",                   1 },
                { L_,   "a__",                   1 },
                { L_,   "__a",                   1 },

                { L_,   "abc ",                  0 },
                { L_,   "a_b_",                  1 },
                { L_,   "a_3_",                  1 },
                { L_,   "a__3",                  1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int EXP          = DATA[ti].d_exp;
                const int curLen       = bsl::strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::string str(SPEC, &testAllocator);
                int rv = bdepu_ParserImpUtil::isIdentifier(str);
                LOOP_ASSERT(LINE, EXP == rv);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PARSE NULL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseNull(cchar **endPos, cchar *inputString);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseNull" << endl
                          << "=================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
            } DATA[] = {
                //line  spec                   offset  fail
                //----  ---------------------  ------  ----
                { L_,   "",                         0,    1 },

                { L_,   "a",                        0,    1 },
                { L_,   "n",                        1,    1 },
                { L_,   "N",                        1,    1 },

                { L_,   "na",                       1,    1 },
                { L_,   "nu",                       2,    1 },
                { L_,   "nU",                       2,    1 },
                { L_,   "Nu",                       2,    1 },
                { L_,   "NU",                       2,    1 },

                { L_,   "nua",                      2,    1 },
                { L_,   "nul",                      3,    1 },
                { L_,   "nuL",                      3,    1 },
                { L_,   "nUl",                      3,    1 },
                { L_,   "nUL",                      3,    1 },
                { L_,   "Nul",                      3,    1 },
                { L_,   "NuL",                      3,    1 },
                { L_,   "NUl",                      3,    1 },
                { L_,   "NUL",                      3,    1 },

                { L_,   "nula",                     3,    1 },
                { L_,   "null",                     4,    0 },
                { L_,   "nulL",                     4,    0 },
                { L_,   "nuLl",                     4,    0 },
                { L_,   "nuLL",                     4,    0 },
                { L_,   "nUll",                     4,    0 },
                { L_,   "nUlL",                     4,    0 },
                { L_,   "nULl",                     4,    0 },
                { L_,   "nULL",                     4,    0 },
                { L_,   "Null",                     4,    0 },
                { L_,   "NulL",                     4,    0 },
                { L_,   "NuLl",                     4,    0 },
                { L_,   "NuLL",                     4,    0 },
                { L_,   "NUll",                     4,    0 },
                { L_,   "NUlL",                     4,    0 },
                { L_,   "NULl",                     4,    0 },
                { L_,   "NULL",                     4,    0 },

                { L_,   "nulla",                    4,    0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int curLen       = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    const char *endPos = 0;
                    int rv = bdepu_ParserImpUtil::parseNull(&endPos, buffer);
                    LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                    LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COMPACT MULTIBYTE CHARS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   compactMultibyteChars(bsl::string *inOutString);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING compactMultibyteChars" << endl
                          << "=============================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_exp_p;    // expected string
            } DATA[] = {
                //line  spec                   exp
                //----  ---------------------  ---------------------
                { L_,   "",                    ""                    },

                { L_,   " ",                   " "                   },
                { L_,   "a",                   "a"                   },
                { L_,   "\\",                  "\\"                  },

                { L_,   "ab",                  "ab"                  },
                { L_,   "a\\",                 "a\\"                 },
                { L_,   "\\a",                 "\a"                  },
                { L_,   "\\b",                 "\b"                  },
                { L_,   "\\f",                 "\f"                  },
                { L_,   "\\n",                 "\n"                  },
                { L_,   "\\r",                 "\r"                  },
                { L_,   "\\t",                 "\t"                  },
                { L_,   "\\v",                 "\v"                  },
                { L_,   "\\?",                 "\?"                  },
                { L_,   "\\'",                 "\'"                  },
                { L_,   "\\\"",                "\""                  },
                { L_,   "\\\\",                "\\"                  },
                { L_,   "\\x",                 "\\x"                 },
                { L_,   "\\1",                 "\1"                  },
                { L_,   "\\7",                 "\7"                  },
                { L_,   "\\8",                 "\\8"                 },

                { L_,   "a\\a",                "a\a"                 },
                { L_,   "a\\b",                "a\b"                 },
                { L_,   "a\\f",                "a\f"                 },
                { L_,   "a\\n",                "a\n"                 },
                { L_,   "a\\r",                "a\r"                 },
                { L_,   "a\\t",                "a\t"                 },
                { L_,   "a\\v",                "a\v"                 },
                { L_,   "a\\?",                "a\?"                 },
                { L_,   "a\\'",                "a\'"                 },
                { L_,   "a\\\"",               "a\""                 },
                { L_,   "a\\\\",               "a\\"                 },
                { L_,   "a\\x",                "a\\x"                },
                { L_,   "a\\1",                "a\1"                 },
                { L_,   "a\\7",                "a\7"                 },
                { L_,   "a\\8",                "a\\8"                },

                { L_,   "\n\\a",               "\n\a"                },
                { L_,   "\n\\b",               "\n\b"                },
                { L_,   "\n\\f",               "\n\f"                },
                { L_,   "\n\\n",               "\n\n"                },
                { L_,   "\n\\r",               "\n\r"                },
                { L_,   "\n\\t",               "\n\t"                },
                { L_,   "\n\\v",               "\n\v"                },
                { L_,   "\n\\?",               "\n\?"                },
                { L_,   "\n\\'",               "\n\'"                },
                { L_,   "\n\\\"",              "\n\""                },
                { L_,   "\n\\\\",              "\n\\"                },
                { L_,   "\n\\x",               "\n\\x"               },
                { L_,   "\n\\1",               "\n\1"                },
                { L_,   "\n\\7",               "\n\7"                },
                { L_,   "\n\\8",               "\n\\8"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_spec_p;
                const char *EXP  = DATA[ti].d_exp_p;

                bsl::string str(SPEC, &testAllocator);
                bdepu_ParserImpUtil::compactMultibyteChars(&str);
                LOOP_ASSERT(LINE, EXP == str);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PARSE ESCAPE SEQUENCE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseEscapeSequence(cchar **endPos, char *result, cchar *input);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseEscapeSequence" << endl
                          << "===========================" << endl;

        {
            const char INITIAL_VALUE_1 = ' ';  // first initial value
            const char INITIAL_VALUE_2 = 'a';  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                char        d_value;    // expected return value
            } DATA[] = {
                //line  spec                   offset  fail  value
                //----  ---------------------  ------  ----  ---------
                { L_,   "",                         0,    1,           },

                { L_,   " ",                        0,    1,           },
                { L_,   "a",                        1,    0, '\a'      },
                { L_,   "b",                        1,    0, '\b'      },
                { L_,   "f",                        1,    0, '\f'      },
                { L_,   "n",                        1,    0, '\n'      },
                { L_,   "r",                        1,    0, '\r'      },
                { L_,   "t",                        1,    0, '\t'      },
                { L_,   "v",                        1,    0, '\v'      },
                { L_,   "?",                        1,    0, '\?'      },
                { L_,   "'",                        1,    0, '\''      },
                { L_,   "\"",                       1,    0, '"'       },
                { L_,   "\\",                       1,    0, '\\'      },
                { L_,   "x",                        1,    1,           },
                { L_,   "1",                        1,    0, (char)1   },
                { L_,   "7",                        1,    0, (char)7   },
                { L_,   "8",                        0,    1,           },

                { L_,   "11",                       2,    0, (char)9   },
                { L_,   "x1",                       2,    0, (char)1   },

                { L_,   "111",                      3,    0, (char)73  },
                { L_,   "xff",                      3,    0, (char)255 },
                { L_,   "xFF",                      3,    0, (char)255 },

                { L_,   "1111",                     3,    0, (char)73  },
                { L_,   "xFFa",                     3,    0, (char)255 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const short VALUE      = DATA[ti].d_value;
                const int curLen       = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        char result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                 parseEscapeSequence(&endPos, &result, buffer);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        char result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                 parseEscapeSequence(&endPos, &result, buffer);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE SIGNED INTEGER
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        // parseSignedInteger(cchar **, Int64 *, cchar *, int, cInt64, cInt64);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseSignedInteger" << endl
                          << "==========================" << endl;

        {
            const Int64 INITIAL_VALUE_1 =  37;  // first initial value
            const Int64 INITIAL_VALUE_2 = -58;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Int64       d_min;      // specification min
                Int64       d_max;      // specification max
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Int64       d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   min      max    off  fail  value
                //----  ---------- ---- -------- -------- ---  ----  -------
                { L_,   "",         10,  -32768,   32767,  0,    1,          },

                { L_,   "a",        10,  -32768,   32767,  0,    1,          },
                { L_,   "+",        10,  -32768,   32767,  1,    1,          },
                { L_,   "-",        10,  -32768,   32767,  1,    1,          },
                { L_,   "0",        10,  -32768,   32767,  1,    0,        0 },
                { L_,   "a",        16,  -32768,   32767,  1,    0,       10 },
                { L_,   "f",        16,  -32768,   32767,  1,    0,       15 },
                { L_,   "A",        16,  -32768,   32767,  1,    0,       10 },
                { L_,   "F",        16,  -32768,   32767,  1,    0,       15 },
                { L_,   "g",        16,  -32768,   32767,  0,    1,          },
                { L_,   "0",         2,  -32768,   32767,  1,    0,        0 },
                { L_,   "1",         2,  -32768,   32767,  1,    0,        1 },
                { L_,   "a",        11,  -32768,   32767,  1,    0,       10 },
                { L_,   "A",        11,  -32768,   32767,  1,    0,       10 },
                { L_,   "z",        36,  -32768,   32767,  1,    0,       35 },
                { L_,   "Z",        36,  -32768,   32767,  1,    0,       35 },

                { L_,   "++",       10,  -32768,   32767,  1,    1,          },
                { L_,   "+-",       10,  -32768,   32767,  1,    1,          },
                { L_,   "-+",       10,  -32768,   32767,  1,    1,          },
                { L_,   "--",       10,  -32768,   32767,  1,    1,          },
                { L_,   "+a",       10,  -32768,   32767,  1,    1,          },
                { L_,   "-a",       10,  -32768,   32767,  1,    1,          },
                { L_,   "+0",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "+9",       10,  -32768,   32767,  2,    0,        9 },
                { L_,   "-0",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "-9",       10,  -32768,   32767,  2,    0,       -9 },
                { L_,   "0a",       10,  -32768,   32767,  1,    0,        0 },
                { L_,   "9a",       10,  -32768,   32767,  1,    0,        9 },
                { L_,   "00",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "01",       10,  -32768,   32767,  2,    0,        1 },
                { L_,   "19",       10,  -32768,   32767,  2,    0,       19 },
                { L_,   "99",       10,  -32768,   32767,  2,    0,       99 },
                { L_,   "+g",       16,  -32768,   32767,  1,    1,          },
                { L_,   "-g",       16,  -32768,   32767,  1,    1,          },
                { L_,   "+a",       16,  -32768,   32767,  2,    0,       10 },
                { L_,   "+f",       16,  -32768,   32767,  2,    0,       15 },
                { L_,   "-a",       16,  -32768,   32767,  2,    0,      -10 },
                { L_,   "-f",       16,  -32768,   32767,  2,    0,      -15 },
                { L_,   "ff",       16,  -32768,   32767,  2,    0,      255 },
                { L_,   "FF",       16,  -32768,   32767,  2,    0,      255 },
                { L_,   "+0",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "-0",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "+1",        2,  -32768,   32767,  2,    0,        1 },
                { L_,   "-1",        2,  -32768,   32767,  2,    0,       -1 },
                { L_,   "00",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "01",        2,  -32768,   32767,  2,    0,        1 },
                { L_,   "10",        2,  -32768,   32767,  2,    0,        2 },
                { L_,   "11",        2,  -32768,   32767,  2,    0,        3 },
                { L_,   "+z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "-z",       36,  -32768,   32767,  2,    0,      -35 },
                { L_,   "0z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "0Z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "10",       36,  -32768,   32767,  2,    0,       36 },
                { L_,   "z0",       36,  -32768,   32767,  2,    0,     1260 },
                { L_,   "Z0",       36,  -32768,   32767,  2,    0,     1260 },

                { L_,   "+0a",      10,  -32768,   32767,  2,    0,        0 },
                { L_,   "+9a",      10,  -32768,   32767,  2,    0,        9 },
                { L_,   "-0a",      10,  -32768,   32767,  2,    0,        0 },
                { L_,   "-9a",      10,  -32768,   32767,  2,    0,       -9 },
                { L_,   "+12",      10,  -32768,   32767,  3,    0,       12 },
                { L_,   "+89",      10,  -32768,   32767,  3,    0,       89 },
                { L_,   "-12",      10,  -32768,   32767,  3,    0,      -12 },
                { L_,   "123",      10,  -32768,   32767,  3,    0,      123 },
                { L_,   "789",      10,  -32768,   32767,  3,    0,      789 },
                { L_,   "+fg",      16,  -32768,   32767,  2,    0,       15 },
                { L_,   "-fg",      16,  -32768,   32767,  2,    0,      -15 },
                { L_,   "+ff",      16,  -32768,   32767,  3,    0,      255 },
                { L_,   "+FF",      16,  -32768,   32767,  3,    0,      255 },
                { L_,   "-ff",      16,  -32768,   32767,  3,    0,     -255 },
                { L_,   "-FF",      16,  -32768,   32767,  3,    0,     -255 },
                { L_,   "fff",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "fFf",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "FfF",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "FFF",      16,  -32768,   32767,  3,    0,     4095 },

                { L_,   "1234",     10,  -32768,   32767,  4,    0,     1234 },
                { L_,   "-123",     10,  -32768,   32767,  4,    0,     -123 },
                { L_,   "7FFF",     16,  -32768,   32767,  4,    0,    32767 },

                { L_,   "12345",    10,  -32768,   32767,  5,    0,    12345 },
                { L_,   "-1234",    10,  -32768,   32767,  5,    0,    -1234 },
                { L_,   "-7FFF",    16,  -32768,   32767,  5,    0,   -32767 },
                { L_,   "32766",    10,  -32768,   32767,  5,    0,    32766 },
                { L_,   "32767",    10,  -32768,   32767,  5,    0,    32767 },
                { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "32769",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "-8000",    16,  -32768,   32767,  5,    0,   -32768 },
                { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,
                             -2147483648LL, 2147483647LL,  5,    0,    32768 },
                { L_,   "32769",    10,
                             -2147483648LL, 2147483647LL,  5,    0,    32769 },

                { L_,   "123456",   10,  -32768,   32767,  5,    0,    12345 },
                { L_,   "-32766",   10,  -32768,   32767,  6,    0,   -32766 },
                { L_,   "-32767",   10,  -32768,   32767,  6,    0,   -32767 },
                { L_,   "-32768",   10,  -32768,   32767,  6,    0,   -32768 },
                { L_,   "-32769",   10,  -32768,   32767,  5,    0,    -3276 },

                { L_,   "-123456",  10,  -32768,   32767,  6,    0,   -12345 },

                { L_,   "2147483647", 10,
                             -2147483648LL, 2147483647LL, 10,    0,
                                                                  2147483647 },
                { L_,   "2147483648", 10,
                             -2147483648LL, 2147483647LL,  9,    0,
                                                                   214748364 },
                { L_,   "2147483649", 10,
                             -2147483648LL, 2147483647LL,  9,    0,
                                                                   214748364 },
                { L_,   "2147483648", 10,
                         -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                                      10,    0, 2147483648LL },
                { L_,   "2147483649", 10,
                         -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                                      10,    0, 2147483649LL },

                { L_,   "-2147483647", 10,
                             -2147483648LL, 2147483647LL, 11,    0,
                                                                 -2147483647 },
                { L_,   "-2147483648", 10,
                             -2147483648LL, 2147483647LL, 11,    0,
                                                               -2147483648LL },
                { L_,   "-2147483649", 10,
                             -2147483648LL, 2147483647LL, 10,    0,
                                                                  -214748364 },
                { L_,   "-2147483648", 10,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                                     11,    0, -2147483648LL },
                { L_,   "-2147483649", 10,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                                     11,    0, -2147483649LL },

                { L_,   "7fffffffffffffff", 16,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                               16,   0, 0x7FFFFFFFFFFFFFFFLL },
                { L_,   "8000000000000000", 16,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                               15,   0,  0x800000000000000LL },

                { L_,   "-8000000000000000", 16,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                              17,   0, -0x8000000000000000LL },
                { L_,   "-8000000000000001", 16,
                       -0x8000000000000000LL, 0x7FFFFFFFFFFFFFFFLL,
                                              16,   0,  -0x800000000000000LL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const Int64 MIN        = DATA[ti].d_min;
                const Int64 MAX        = DATA[ti].d_max;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const Int64 VALUE      = DATA[ti].d_value;
                const int curLen       = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Int64 result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                    parseSignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Int64 result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                    parseSignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE UNSIGNED INTEGER
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        // parseUnsignedInteger(cchar **, Int64 *, cchar *, int, cUint64, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseUnsignedInteger" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nDefault maximum number of digits." << endl;
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Uint64      d_max;      // specification max
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Uint64      d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   max    off  fail  value
                //----  ---------- ---- -------- ---  ----  -------
                { L_,   "",         10,   32767,  0,    1,          },

                { L_,   "a",        10,   32767,  0,    1,          },
                { L_,   "+",        10,   32767,  0,    1,          },
                { L_,   "-",        10,   32767,  0,    1,          },
                { L_,   "0",        10,   32767,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,    0,       15 },
                { L_,   "g",        16,   32767,  0,    1,          },
                { L_,   "0",         2,   32767,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  0,    1,          },
                { L_,   "-a",       10,   32767,  0,    1,          },
                { L_,   "+0",       10,   32767,  0,    1,          },
                { L_,   "+9",       10,   32767,  0,    1,          },
                { L_,   "-0",       10,   32767,  0,    1,          },
                { L_,   "-9",       10,   32767,  0,    1,          },
                { L_,   "0a",       10,   32767,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,    0,     1260 },

                { L_,   "123",      10,   32767,  3,    0,      123 },
                { L_,   "789",      10,   32767,  3,    0,      789 },
                { L_,   "fff",      16,   32767,  3,    0,     4095 },
                { L_,   "fFf",      16,   32767,  3,    0,     4095 },
                { L_,   "FfF",      16,   32767,  3,    0,     4095 },
                { L_,   "FFF",      16,   32767,  3,    0,     4095 },

                { L_,   "1234",     10,   32767,  4,    0,     1234 },
                { L_,   "7FFF",     16,   32767,  4,    0,    32767 },

                { L_,   "12345",    10,   32767,  5,    0,    12345 },
                { L_,   "32766",    10,   32767,  5,    0,    32766 },
                { L_,   "32767",    10,   32767,  5,    0,    32767 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32769",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,
                                     2147483647,  5,    0,    32768 },
                { L_,   "32769",    10,
                                     2147483647,  5,    0,    32769 },

                { L_,   "123456",   10,   32767,  5,    0,    12345 },

                { L_,   "2147483647", 10,
                                     2147483647, 10,    0, 2147483647 },
                { L_,   "2147483648", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483649", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483648", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483648uLL },
                { L_,   "2147483649", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483649uLL },

                { L_,   "7fffffffffffffff", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 16,    0,
                                                0x7FFFFFFFFFFFFFFFuLL },
                { L_,   "8000000000000000", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 15,    0,
                                                 0x800000000000000uLL },
                { L_,   "8000000000000000", 16,
                          0x8000000000000000uLL, 16,    0,
                                                0x8000000000000000uLL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const Uint64 MAX       = DATA[ti].d_max;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const Uint64 VALUE     = DATA[ti].d_value;
                const int curLen       = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Uint64 result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                  parseUnsignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64 result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                  parseUnsignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
        if (verbose) cout << "\nSpecified maximum number of digits." << endl;
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Uint64      d_max;      // specification max
                int         d_digit;    // specification number of digits
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Uint64      d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   max    dig off  fail  value
                //----  ---------- ---- -------- --- ---  ----  -------
                { L_,   "a",        10,   32767,  1,  0,    1,          },
                { L_,   "+",        10,   32767,  1,  0,    1,          },
                { L_,   "-",        10,   32767,  1,  0,    1,          },
                { L_,   "0",        10,   32767,  1,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,  1,    0,       15 },
                { L_,   "g",        16,   32767,  1,  0,    1,          },
                { L_,   "0",         2,   32767,  1,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  1,  0,    1,          },
                { L_,   "-a",       10,   32767,  1,  0,    1,          },
                { L_,   "+0",       10,   32767,  1,  0,    1,          },
                { L_,   "+9",       10,   32767,  1,  0,    1,          },
                { L_,   "-0",       10,   32767,  1,  0,    1,          },
                { L_,   "-9",       10,   32767,  1,  0,    1,          },
                { L_,   "0a",       10,   32767,  1,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,  1,    0,        9 },
                { L_,   "00",       10,   32767,  1,  1,    0,        0 },
                { L_,   "01",       10,   32767,  1,  1,    0,        0 },
                { L_,   "19",       10,   32767,  1,  1,    0,        1 },
                { L_,   "99",       10,   32767,  1,  1,    0,        9 },
                { L_,   "ff",       16,   32767,  1,  1,    0,       15 },
                { L_,   "FF",       16,   32767,  1,  1,    0,       15 },
                { L_,   "00",        2,   32767,  1,  1,    0,        0 },
                { L_,   "01",        2,   32767,  1,  1,    0,        0 },
                { L_,   "10",        2,   32767,  1,  1,    0,        1 },
                { L_,   "11",        2,   32767,  1,  1,    0,        1 },
                { L_,   "0z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "0Z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "10",       36,   32767,  1,  1,    0,        1 },
                { L_,   "z0",       36,   32767,  1,  1,    0,       35 },
                { L_,   "Z0",       36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  2,  0,    1,          },
                { L_,   "-a",       10,   32767,  2,  0,    1,          },
                { L_,   "+0",       10,   32767,  2,  0,    1,          },
                { L_,   "+9",       10,   32767,  2,  0,    1,          },
                { L_,   "-0",       10,   32767,  2,  0,    1,          },
                { L_,   "-9",       10,   32767,  2,  0,    1,          },
                { L_,   "0a",       10,   32767,  2,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  2,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,  2,    0,     1260 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const Uint64 MAX       = DATA[ti].d_max;
                const int DIGIT        = DATA[ti].d_digit;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const Uint64 VALUE     = DATA[ti].d_value;
                const int curLen       = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Uint64 result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                  parseUnsignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64 result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdepu_ParserImpUtil::
                                                  parseUnsignedInteger(&endPos,
                                                                       &result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CHARACTER TO DIGIT
        //
        // Concerns:
        //   1. Correct value is returned.
        //   2. Characters that comprise the legal set varies correctly with
        //      'base'.
        //
        // Plan:
        //   Use the table-driven approach with columns for input, base, and
        //   expected result.  Use category partitioning to create a suite of
        //   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   characterToDigit(char character, int base);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING characterToDigit" << endl
                          << "========================" << endl;

        {
            static const struct {
                int  d_lineNum;  // source line number
                char d_spec;     // specification character
                int  d_base;     // specification base
                int  d_value;    // expected return value
            } DATA[] = {
                //line  spec  base  value
                //----  ----  ----  -----
                { L_,    '0',    2,     0 },
                { L_,    '1',    2,     1 },
                { L_,    '2',    2,    -1 },
                { L_,    '7',    2,    -1 },
                { L_,    '8',    2,    -1 },
                { L_,    '9',    2,    -1 },
                { L_,    'a',    2,    -1 },
                { L_,    'f',    2,    -1 },
                { L_,    'z',    2,    -1 },
                { L_,    'A',    2,    -1 },
                { L_,    'F',    2,    -1 },
                { L_,    'Z',    2,    -1 },
                { L_,    ' ',    2,    -1 },
                { L_,    '+',    2,    -1 },
                { L_,    '-',    2,    -1 },

                { L_,    '0',    8,     0 },
                { L_,    '1',    8,     1 },
                { L_,    '2',    8,     2 },
                { L_,    '7',    8,     7 },
                { L_,    '8',    8,    -1 },
                { L_,    '9',    8,    -1 },
                { L_,    'a',    8,    -1 },
                { L_,    'f',    8,    -1 },
                { L_,    'z',    8,    -1 },
                { L_,    'A',    8,    -1 },
                { L_,    'F',    8,    -1 },
                { L_,    'Z',    8,    -1 },
                { L_,    ' ',    8,    -1 },
                { L_,    '+',    8,    -1 },
                { L_,    '-',    8,    -1 },

                { L_,    '0',   10,     0 },
                { L_,    '1',   10,     1 },
                { L_,    '2',   10,     2 },
                { L_,    '7',   10,     7 },
                { L_,    '8',   10,     8 },
                { L_,    '9',   10,     9 },
                { L_,    'a',   10,    -1 },
                { L_,    'f',   10,    -1 },
                { L_,    'z',   10,    -1 },
                { L_,    'A',   10,    -1 },
                { L_,    'F',   10,    -1 },
                { L_,    'Z',   10,    -1 },
                { L_,    ' ',   10,    -1 },
                { L_,    '+',   10,    -1 },
                { L_,    '-',   10,    -1 },

                { L_,    '0',   16,     0 },
                { L_,    '1',   16,     1 },
                { L_,    '2',   16,     2 },
                { L_,    '7',   16,     7 },
                { L_,    '8',   16,     8 },
                { L_,    '9',   16,     9 },
                { L_,    'a',   16,    10 },
                { L_,    'f',   16,    15 },
                { L_,    'z',   16,    -1 },
                { L_,    'A',   16,    10 },
                { L_,    'F',   16,    15 },
                { L_,    'Z',   16,    -1 },
                { L_,    ' ',   16,    -1 },
                { L_,    '+',   16,    -1 },
                { L_,    '-',   16,    -1 },

                { L_,    '0',   36,     0 },
                { L_,    '1',   36,     1 },
                { L_,    '2',   36,     2 },
                { L_,    '7',   36,     7 },
                { L_,    '8',   36,     8 },
                { L_,    '9',   36,     9 },
                { L_,    'a',   36,    10 },
                { L_,    'f',   36,    15 },
                { L_,    'z',   36,    35 },
                { L_,    'A',   36,    10 },
                { L_,    'F',   36,    15 },
                { L_,    'Z',   36,    35 },
                { L_,    ' ',   36,    -1 },
                { L_,    '+',   36,    -1 },
                { L_,    '-',   36,    -1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char  SPEC  = DATA[ti].d_spec;
                const int   BASE  = DATA[ti].d_base;
                const short VALUE = DATA[ti].d_value;

                int rv = bdepu_ParserImpUtil::characterToDigit(SPEC, BASE);
                LOOP_ASSERT(LINE, VALUE == rv);
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
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
