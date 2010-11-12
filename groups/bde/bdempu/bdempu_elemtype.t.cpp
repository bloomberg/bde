// bdempu_elemtype.t.cpp              -*-C++-*-

#include <bdempu_elemtype.h>

#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>

#include <bdem_elemtype.h>

#include <bsl_iostream.h>

#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

enum { SUCCESS = 0, FAILURE = 1 };

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
//
// For each test case the test vectors include both cases where parsable
// string is (a) the only string in the input and (b) followed by a suffix
// string.
//
// We use the following abbreviations: (for documentation purposes only)
//  'cchar'      for 'const char'
//  'b_ca'       for 'bsl::vector<char>'
//  'Type'       for 'bdem_ElemType::Type'
//
//-----------------------------------------------------------------------------
// [ 1] parseElemType(cchar **endPos, Type *result, cchar *input);
// [ 2] generateElemType(b_ca *buf, const Type& val, int level, int spaces);
// [ 2] generateElemTypeRaw(b_ca *buf, const Type& val, int level, int spaces);
//-----------------------------------------------------------------------------

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

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { P_(I) P_(J) P(K) aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERATE ELEMTYPE*
        //   Concerns:
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The result is null-terminated.
        //       3. The generated result can be parsed back with the
        //          corresponding parser function.
        //       4. The values that were in the buffer prior to the call are
        //          not changed.
        //
        // Plan: // TBD doc
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates indentation when 'level' is
        //   less than, equal to, and greater than zero.
        //
        // Testing:
        //    generateElemType(b_ca *buf, const Type& val, int lev, int sp);
        //    generateElemTypeRaw(b_ca *buf, const Type& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING GENERATE ELEMTYPE*" << endl
                 << "==========================" << endl;

        typedef bdem_ElemType ET;

        {
            static const struct {
                int             d_lineNum;         // source line number
                const char     *d_spec_p;          // specification string
                ET::Type        d_value;           // specification value
                int             d_level;           // specification level
                int             d_spacesPerLevel;  // spec. spaces per level
                const char     *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
  { L_,   "",   ET::BDEM_CHAR,           0,   1, "CHAR"             },
  { L_,   "",   ET::BDEM_CHAR,           1,   1, " CHAR"            },
  { L_,   "",   ET::BDEM_CHAR,           1,   2, "  CHAR"           },
  { L_,   "x",  ET::BDEM_CHAR,           0,   1, "CHAR"             },
  { L_,   "x",  ET::BDEM_CHAR,           1,   1, " CHAR"            },
  { L_,   "x",  ET::BDEM_CHAR,           1,   2, "  CHAR"           },

  { L_,   "",   ET::BDEM_DATE,           0,   1, "DATE"             },
  { L_,   "",   ET::BDEM_DATE,           1,   1, " DATE"            },
  { L_,   "",   ET::BDEM_DATE,           1,   2, "  DATE"           },
  { L_,   "x",  ET::BDEM_DATE,           0,   1, "DATE"             },
  { L_,   "x",  ET::BDEM_DATE,           1,   1, " DATE"            },
  { L_,   "x",  ET::BDEM_DATE,           1,   2, "  DATE"           },

  { L_,   "",   ET::BDEM_DATETIME,       0,   1, "DATETIME"         },
  { L_,   "",   ET::BDEM_DATETIME,       1,   1, " DATETIME"        },
  { L_,   "",   ET::BDEM_DATETIME,       1,   2, "  DATETIME"       },
  { L_,   "x",  ET::BDEM_DATETIME,       0,   1, "DATETIME"         },
  { L_,   "x",  ET::BDEM_DATETIME,       1,   1, " DATETIME"        },
  { L_,   "x",  ET::BDEM_DATETIME,       1,   2, "  DATETIME"       },

  { L_,   "",   ET::BDEM_DOUBLE,         0,   1, "DOUBLE"           },
  { L_,   "",   ET::BDEM_DOUBLE,         1,   1, " DOUBLE"          },
  { L_,   "",   ET::BDEM_DOUBLE,         1,   2, "  DOUBLE"         },
  { L_,   "x",  ET::BDEM_DOUBLE,         0,   1, "DOUBLE"           },
  { L_,   "x",  ET::BDEM_DOUBLE,         1,   1, " DOUBLE"          },
  { L_,   "x",  ET::BDEM_DOUBLE,         1,   2, "  DOUBLE"         },

  { L_,   "",   ET::BDEM_FLOAT,          0,   1, "FLOAT"            },
  { L_,   "",   ET::BDEM_FLOAT,          1,   1, " FLOAT"           },
  { L_,   "",   ET::BDEM_FLOAT,          1,   2, "  FLOAT"          },
  { L_,   "x",  ET::BDEM_FLOAT,          0,   1, "FLOAT"            },
  { L_,   "x",  ET::BDEM_FLOAT,          1,   1, " FLOAT"           },
  { L_,   "x",  ET::BDEM_FLOAT,          1,   2, "  FLOAT"          },

  { L_,   "",   ET::BDEM_INT,            0,   1, "INT"              },
  { L_,   "",   ET::BDEM_INT,            1,   1, " INT"             },
  { L_,   "",   ET::BDEM_INT,            1,   2, "  INT"            },
  { L_,   "x",  ET::BDEM_INT,            0,   1, "INT"              },
  { L_,   "x",  ET::BDEM_INT,            1,   1, " INT"             },
  { L_,   "x",  ET::BDEM_INT,            1,   2, "  INT"            },

  { L_,   "",   ET::BDEM_INT64,          0,   1, "INT64"            },
  { L_,   "",   ET::BDEM_INT64,          1,   1, " INT64"           },
  { L_,   "",   ET::BDEM_INT64,          1,   2, "  INT64"          },
  { L_,   "x",  ET::BDEM_INT64,          0,   1, "INT64"            },
  { L_,   "x",  ET::BDEM_INT64,          1,   1, " INT64"           },
  { L_,   "x",  ET::BDEM_INT64,          1,   2, "  INT64"          },

  { L_,   "",   ET::BDEM_SHORT,          0,   1, "SHORT"            },
  { L_,   "",   ET::BDEM_SHORT,          1,   1, " SHORT"           },
  { L_,   "",   ET::BDEM_SHORT,          1,   2, "  SHORT"          },
  { L_,   "x",  ET::BDEM_SHORT,          0,   1, "SHORT"            },
  { L_,   "x",  ET::BDEM_SHORT,          1,   1, " SHORT"           },
  { L_,   "x",  ET::BDEM_SHORT,          1,   2, "  SHORT"          },

  { L_,   "",   ET::BDEM_STRING,         0,   1, "STRING"           },
  { L_,   "",   ET::BDEM_STRING,         1,   1, " STRING"          },
  { L_,   "",   ET::BDEM_STRING,         1,   2, "  STRING"         },
  { L_,   "x",  ET::BDEM_STRING,         0,   1, "STRING"           },
  { L_,   "x",  ET::BDEM_STRING,         1,   1, " STRING"          },
  { L_,   "x",  ET::BDEM_STRING,         1,   2, "  STRING"         },

  { L_,   "",   ET::BDEM_TIME,           0,   1, "TIME"             },
  { L_,   "",   ET::BDEM_TIME,           1,   1, " TIME"            },
  { L_,   "",   ET::BDEM_TIME,           1,   2, "  TIME"           },
  { L_,   "x",  ET::BDEM_TIME,           0,   1, "TIME"             },
  { L_,   "x",  ET::BDEM_TIME,           1,   1, " TIME"            },
  { L_,   "x",  ET::BDEM_TIME,           1,   2, "  TIME"           },

  { L_,   "",   ET::BDEM_CHAR_ARRAY,     0,   1, "CHAR_ARRAY"       },
  { L_,   "",   ET::BDEM_CHAR_ARRAY,     1,   1, " CHAR_ARRAY"      },
  { L_,   "",   ET::BDEM_CHAR_ARRAY,     1,   2, "  CHAR_ARRAY"     },
  { L_,   "x",  ET::BDEM_CHAR_ARRAY,     0,   1, "CHAR_ARRAY"       },
  { L_,   "x",  ET::BDEM_CHAR_ARRAY,     1,   1, " CHAR_ARRAY"      },
  { L_,   "x",  ET::BDEM_CHAR_ARRAY,     1,   2, "  CHAR_ARRAY"     },

  { L_,   "",   ET::BDEM_DATE_ARRAY,     0,   1, "DATE_ARRAY"       },
  { L_,   "",   ET::BDEM_DATE_ARRAY,     1,   1, " DATE_ARRAY"      },
  { L_,   "",   ET::BDEM_DATE_ARRAY,     1,   2, "  DATE_ARRAY"     },
  { L_,   "x",  ET::BDEM_DATE_ARRAY,     0,   1, "DATE_ARRAY"       },
  { L_,   "x",  ET::BDEM_DATE_ARRAY,     1,   1, " DATE_ARRAY"      },
  { L_,   "x",  ET::BDEM_DATE_ARRAY,     1,   2, "  DATE_ARRAY"     },

  { L_,   "",   ET::BDEM_DATETIME_ARRAY, 0,   1, "DATETIME_ARRAY"   },
  { L_,   "",   ET::BDEM_DATETIME_ARRAY, 1,   1, " DATETIME_ARRAY"  },
  { L_,   "",   ET::BDEM_DATETIME_ARRAY, 1,   2, "  DATETIME_ARRAY" },
  { L_,   "x",  ET::BDEM_DATETIME_ARRAY, 0,   1, "DATETIME_ARRAY"   },
  { L_,   "x",  ET::BDEM_DATETIME_ARRAY, 1,   1, " DATETIME_ARRAY"  },
  { L_,   "x",  ET::BDEM_DATETIME_ARRAY, 1,   2, "  DATETIME_ARRAY" },

  { L_,   "",   ET::BDEM_DOUBLE_ARRAY,   0,   1, "DOUBLE_ARRAY"     },
  { L_,   "",   ET::BDEM_DOUBLE_ARRAY,   1,   1, " DOUBLE_ARRAY"    },
  { L_,   "",   ET::BDEM_DOUBLE_ARRAY,   1,   2, "  DOUBLE_ARRAY"   },
  { L_,   "x",  ET::BDEM_DOUBLE_ARRAY,   0,   1, "DOUBLE_ARRAY"     },
  { L_,   "x",  ET::BDEM_DOUBLE_ARRAY,   1,   1, " DOUBLE_ARRAY"    },
  { L_,   "x",  ET::BDEM_DOUBLE_ARRAY,   1,   2, "  DOUBLE_ARRAY"   },

  { L_,   "",   ET::BDEM_FLOAT_ARRAY,    0,   1, "FLOAT_ARRAY"      },
  { L_,   "",   ET::BDEM_FLOAT_ARRAY,    1,   1, " FLOAT_ARRAY"     },
  { L_,   "",   ET::BDEM_FLOAT_ARRAY,    1,   2, "  FLOAT_ARRAY"    },
  { L_,   "x",  ET::BDEM_FLOAT_ARRAY,    0,   1, "FLOAT_ARRAY"      },
  { L_,   "x",  ET::BDEM_FLOAT_ARRAY,    1,   1, " FLOAT_ARRAY"     },
  { L_,   "x",  ET::BDEM_FLOAT_ARRAY,    1,   2, "  FLOAT_ARRAY"    },

  { L_,   "",   ET::BDEM_INT_ARRAY,      0,   1, "INT_ARRAY"        },
  { L_,   "",   ET::BDEM_INT_ARRAY,      1,   1, " INT_ARRAY"       },
  { L_,   "",   ET::BDEM_INT_ARRAY,      1,   2, "  INT_ARRAY"      },
  { L_,   "x",  ET::BDEM_INT_ARRAY,      0,   1, "INT_ARRAY"        },
  { L_,   "x",  ET::BDEM_INT_ARRAY,      1,   1, " INT_ARRAY"       },
  { L_,   "x",  ET::BDEM_INT_ARRAY,      1,   2, "  INT_ARRAY"      },

  { L_,   "",   ET::BDEM_INT64_ARRAY,    0,   1, "INT64_ARRAY"      },
  { L_,   "",   ET::BDEM_INT64_ARRAY,    1,   1, " INT64_ARRAY"     },
  { L_,   "",   ET::BDEM_INT64_ARRAY,    1,   2, "  INT64_ARRAY"    },
  { L_,   "x",  ET::BDEM_INT64_ARRAY,    0,   1, "INT64_ARRAY"      },
  { L_,   "x",  ET::BDEM_INT64_ARRAY,    1,   1, " INT64_ARRAY"     },
  { L_,   "x",  ET::BDEM_INT64_ARRAY,    1,   2, "  INT64_ARRAY"    },

  { L_,   "",   ET::BDEM_SHORT_ARRAY,    0,   1, "SHORT_ARRAY"      },
  { L_,   "",   ET::BDEM_SHORT_ARRAY,    1,   1, " SHORT_ARRAY"     },
  { L_,   "",   ET::BDEM_SHORT_ARRAY,    1,   2, "  SHORT_ARRAY"    },
  { L_,   "x",  ET::BDEM_SHORT_ARRAY,    0,   1, "SHORT_ARRAY"      },
  { L_,   "x",  ET::BDEM_SHORT_ARRAY,    1,   1, " SHORT_ARRAY"     },
  { L_,   "x",  ET::BDEM_SHORT_ARRAY,    1,   2, "  SHORT_ARRAY"    },

  { L_,   "",   ET::BDEM_STRING_ARRAY,   0,   1, "STRING_ARRAY"     },
  { L_,   "",   ET::BDEM_STRING_ARRAY,   1,   1, " STRING_ARRAY"    },
  { L_,   "",   ET::BDEM_STRING_ARRAY,   1,   2, "  STRING_ARRAY"   },
  { L_,   "x",  ET::BDEM_STRING_ARRAY,   0,   1, "STRING_ARRAY"     },
  { L_,   "x",  ET::BDEM_STRING_ARRAY,   1,   1, " STRING_ARRAY"    },
  { L_,   "x",  ET::BDEM_STRING_ARRAY,   1,   2, "  STRING_ARRAY"   },

  { L_,   "",   ET::BDEM_TIME_ARRAY,     0,   1, "TIME_ARRAY"       },
  { L_,   "",   ET::BDEM_TIME_ARRAY,     1,   1, " TIME_ARRAY"      },
  { L_,   "",   ET::BDEM_TIME_ARRAY,     1,   2, "  TIME_ARRAY"     },
  { L_,   "x",  ET::BDEM_TIME_ARRAY,     0,   1, "TIME_ARRAY"       },
  { L_,   "x",  ET::BDEM_TIME_ARRAY,     1,   1, " TIME_ARRAY"      },
  { L_,   "x",  ET::BDEM_TIME_ARRAY,     1,   2, "  TIME_ARRAY"     },

  { L_,   "",   ET::BDEM_LIST,           0,   1, "LIST"             },
  { L_,   "",   ET::BDEM_LIST,           1,   1, " LIST"            },
  { L_,   "",   ET::BDEM_LIST,           1,   2, "  LIST"           },
  { L_,   "x",  ET::BDEM_LIST,           0,   1, "LIST"             },
  { L_,   "x",  ET::BDEM_LIST,           1,   1, " LIST"            },
  { L_,   "x",  ET::BDEM_LIST,           1,   2, "  LIST"           },

  { L_,   "",   ET::BDEM_TABLE,          0,   1, "TABLE"            },
  { L_,   "",   ET::BDEM_TABLE,          1,   1, " TABLE"           },
  { L_,   "",   ET::BDEM_TABLE,          1,   2, "  TABLE"          },
  { L_,   "x",  ET::BDEM_TABLE,          0,   1, "TABLE"            },
  { L_,   "x",  ET::BDEM_TABLE,          1,   1, " TABLE"           },
  { L_,   "x",  ET::BDEM_TABLE,          1,   2, "  TABLE"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const ET::Type    VALUE = DATA[ti].d_value;
                const int         LEVEL = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;

                const bsl::string SPECSTR = SPEC;
                const bsl::string EXPSTR  = EXP;

                bsl::vector<char> buffer;
                bsl::vector<char> exp;

                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                exp.insert(exp.end(), SPECSTR.begin(), SPECSTR.end());
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                bdempu_ElemType::
                               generateElemTypeRaw(&buffer, VALUE, LEVEL, SPL);
                LOOP3_ASSERT(LINE, &exp.front(),
                             &buffer.front(), exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.clear();
                exp.insert(exp.end(), SPECSTR.begin(), SPECSTR.end());
                exp.push_back('\0');
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                bdempu_ElemType::
                               generateElemTypeRaw(&buffer, VALUE, LEVEL, SPL);
                LOOP3_ASSERT(LINE, &exp.front(),
                             &buffer.front(), exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                exp.clear();
                exp.insert(exp.end(), SPECSTR.begin(), SPECSTR.end());
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back('\0');
                bdempu_ElemType::generateElemType(&buffer, VALUE, LEVEL, SPL);
                LOOP3_ASSERT(LINE, &exp.front(),
                             &buffer.front(), exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                bdempu_ElemType::generateElemType(&buffer, VALUE, LEVEL, SPL);
                LOOP3_ASSERT(LINE, &exp.front(),
                             &buffer.front(), exp == buffer);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING PARSE ELEMTYPE
        //   Concerns:
        //     For the parsing method, the following properties must hold:
        //       When parsing is successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is correct
        //           4.  Leading <WHITESPACE> characters are skipped
        //       When parsing is not successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is unchanged
        //
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The result is null-terminated.
        //       3. The generated result can be parsed back with the
        //          corresponding parser function.
        //       4. The values that were in the buffer prior to the call are
        //          not changed.
        //
        // Plan: // TBD doc
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with invalid character inserted in every position of
        //   every element type string.
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates indentation when 'level' is
        //   less than, equal to, and greater than zero.
        //
        //   Test both methods for every valid Type as well as for the invalid
        //   input.
        //
        // Testing:
        //    parseElemType(cchar **endPos, Type *result, cchar *input);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING PARSE ELEMTYPE" << endl
                 << "======================" << endl;

        typedef bdem_ElemType ET;

        {
            const ET::Type INITIAL_VALUE_1 = ET::BDEM_CHAR; // 1st initial val.
            const ET::Type INITIAL_VALUE_2 = ET::BDEM_INT;  // 2nd initial val.

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                short       d_value;    // expected return value
            } DATA[] = {
                //line spec              offset fail  value
                //---- ----------------- ------ ----  -----------------------
                { L_,  "",                    0,   1,                        },

                { L_,  "x",                   0,   1,                        },
                { L_,  "C",                   1,   1,                        },
                { L_,  "D",                   1,   1,                        },
                { L_,  "F",                   1,   1,                        },
                { L_,  "I",                   1,   1,                        },
                { L_,  "L",                   1,   1,                        },
                { L_,  "S",                   1,   1,                        },
                { L_,  "T",                   1,   1,                        },

                { L_,  "Cx",                  1,   1,                        },
                { L_,  "Dx",                  1,   1,                        },
                { L_,  "Fx",                  1,   1,                        },
                { L_,  "Ix",                  1,   1,                        },
                { L_,  "Lx",                  1,   1,                        },
                { L_,  "Sx",                  1,   1,                        },
                { L_,  "Tx",                  1,   1,                        },
                { L_,  "CH",                  2,   1,                        },
                { L_,  "DA",                  2,   1,                        },
                { L_,  "DO",                  2,   1,                        },
                { L_,  "FL",                  2,   1,                        },
                { L_,  "IN",                  2,   1,                        },
                { L_,  "LI",                  2,   1,                        },
                { L_,  "SH",                  2,   1,                        },
                { L_,  "ST",                  2,   1,                        },
                { L_,  "TA",                  2,   1,                        },
                { L_,  "TI",                  2,   1,                        },

                { L_,  "CHx",                 2,   1,                        },
                { L_,  "DAx",                 2,   1,                        },
                { L_,  "DOx",                 2,   1,                        },
                { L_,  "FLx",                 2,   1,                        },
                { L_,  "INx",                 2,   1,                        },
                { L_,  "LIx",                 2,   1,                        },
                { L_,  "SHx",                 2,   1,                        },
                { L_,  "STx",                 2,   1,                        },
                { L_,  "TAx",                 2,   1,                        },
                { L_,  "TIx",                 2,   1,                        },
                { L_,  "CHA",                 3,   1,                        },
                { L_,  "DAT",                 3,   1,                        },
                { L_,  "DOU",                 3,   1,                        },
                { L_,  "FLO",                 3,   1,                        },
                { L_,  "INT",                 3,   0, ET::BDEM_INT           },
                { L_,  "LIS",                 3,   1,                        },
                { L_,  "SHO",                 3,   1,                        },
                { L_,  "STR",                 3,   1,                        },
                { L_,  "TAB",                 3,   1,                        },
                { L_,  "TIM",                 3,   1,                        },

                { L_,  "CHAx",                3,   1,                        },
                { L_,  "DATx",                3,   1,                        },
                { L_,  "DOUx",                3,   1,                        },
                { L_,  "FLOx",                3,   1,                        },
                { L_,  "INTx",                3,   0, ET::BDEM_INT           },
                { L_,  "LISx",                3,   1,                        },
                { L_,  "SHOx",                3,   1,                        },
                { L_,  "STRx",                3,   1,                        },
                { L_,  "TABx",                3,   1,                        },
                { L_,  "TIMx",                3,   1,                        },
                { L_,  "CHAR",                4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE",                4,   0, ET::BDEM_DATE          },
                { L_,  "DOUB",                4,   1,                        },
                { L_,  "FLOA",                4,   1,                        },
                { L_,  "INT_",                3,   0, ET::BDEM_INT           },
                { L_,  "INT6",                3,   0, ET::BDEM_INT           },
                { L_,  "LIST",                4,   0, ET::BDEM_LIST          },
                { L_,  "SHOR",                4,   1,                        },
                { L_,  "STRI",                4,   1,                        },
                { L_,  "TABL",                4,   1,                        },
                { L_,  "TIME",                4,   0, ET::BDEM_TIME          },

                { L_,  "CHARx",               4,   0, ET::BDEM_CHAR          },
                { L_,  "DATEx",               4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBx",               4,   1,                        },
                { L_,  "FLOAx",               4,   1,                        },
                { L_,  "INT_x",               3,   0, ET::BDEM_INT           },
                { L_,  "INT6x",               3,   0, ET::BDEM_INT           },
                { L_,  "LISTx",               4,   0, ET::BDEM_LIST          },
                { L_,  "SHORx",               4,   1,                        },
                { L_,  "STRIx",               4,   1,                        },
                { L_,  "TABLx",               4,   1,                        },
                { L_,  "TIMEx",               4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_",               4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_",               4,   0, ET::BDEM_DATE          },
                { L_,  "DATET",               4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBL",               5,   1,                        },
                { L_,  "FLOAT",               5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_A",               3,   0, ET::BDEM_INT           },
                { L_,  "INT64",               5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT",               5,   0, ET::BDEM_SHORT         },
                { L_,  "STRIN",               5,   1,                        },
                { L_,  "TABLE",               5,   0, ET::BDEM_TABLE         },
                { L_,  "TIME_",               4,   0, ET::BDEM_TIME          },

                { L_,  "CHAR_x",              4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_x",              4,   0, ET::BDEM_DATE          },
                { L_,  "DATETx",              4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBLx",              5,   1,                        },
                { L_,  "FLOATx",              5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_Ax",              3,   0, ET::BDEM_INT           },
                { L_,  "INT64x",              5,   0, ET::BDEM_INT64         },
                { L_,  "SHORTx",              5,   0, ET::BDEM_SHORT         },
                { L_,  "STRINx",              5,   1,                        },
                { L_,  "TABLEx",              5,   0, ET::BDEM_TABLE         },
                { L_,  "TIME_x",              4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_A",              4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_A",              4,   0, ET::BDEM_DATE          },
                { L_,  "DATETI",              4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBLE",              6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_",              5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_AR",              3,   0, ET::BDEM_INT           },
                { L_,  "INT64_",              5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_",              5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING",              6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_A",              4,   0, ET::BDEM_TIME          },

                { L_,  "CHAR_Ax",             4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_Ax",             4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIx",             4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBLEx",             6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_x",             5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARx",             3,   0, ET::BDEM_INT           },
                { L_,  "INT64_x",             5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_x",             5,   0, ET::BDEM_SHORT         },
                { L_,  "STRINGx",             6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_Ax",             4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_AR",             4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_AR",             4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIM",             4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBLE_",             6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_A",             5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARR",             3,   0, ET::BDEM_INT           },
                { L_,  "INT64_A",             5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_A",             5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_",             6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_AR",             4,   0, ET::BDEM_TIME          },

                { L_,  "CHAR_ARx",            4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_ARx",            4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIMx",            4,   0, ET::BDEM_DATE          },
                { L_,  "DOUBLE_x",            6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_Ax",            5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARRx",            3,   0, ET::BDEM_INT           },
                { L_,  "INT64_Ax",            5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_Ax",            5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_x",            6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARx",            4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_ARR",            4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_ARR",            4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIME",            8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_A",            6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_AR",            5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARRA",            3,   0, ET::BDEM_INT           },
                { L_,  "INT64_AR",            5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_AR",            5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_A",            6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARR",            4,   0, ET::BDEM_TIME          },

                { L_,  "CHAR_ARRx",           4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_ARRx",           4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIMEx",           8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_Ax",           6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARx",           5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARRAx",           3,   0, ET::BDEM_INT           },
                { L_,  "INT64_ARx",           5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_ARx",           5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_Ax",           6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARRx",           4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_ARRA",           4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_ARRA",           4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIME_",           8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_AR",           6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARR",           5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARRAY",           9,   0, ET::BDEM_INT_ARRAY     },
                { L_,  "INT64_ARR",           5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_ARR",           5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_AR",           6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARRA",           4,   0, ET::BDEM_TIME          },

                { L_,  "CHAR_ARRAx",          4,   0, ET::BDEM_CHAR          },
                { L_,  "DATE_ARRAx",          4,   0, ET::BDEM_DATE          },
                { L_,  "DATETIME_x",          8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARx",          6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARRx",          5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT_ARRAYx",          9,   0, ET::BDEM_INT_ARRAY     },
                { L_,  "INT64_ARRx",          5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_ARRx",          5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_ARx",          6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARRAx",          4,   0, ET::BDEM_TIME          },
                { L_,  "CHAR_ARRAY",         10,   0, ET::BDEM_CHAR_ARRAY    },
                { L_,  "DATE_ARRAY",         10,   0, ET::BDEM_DATE_ARRAY    },
                { L_,  "DATETIME_A",          8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARR",          6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARRA",          5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT64_ARRA",          5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_ARRA",          5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_ARR",          6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARRAY",         10,   0, ET::BDEM_TIME_ARRAY    },

                { L_,  "CHAR_ARRAYx",        10,   0, ET::BDEM_CHAR_ARRAY    },
                { L_,  "DATE_ARRAYx",        10,   0, ET::BDEM_DATE_ARRAY    },
                { L_,  "DATETIME_Ax",         8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARRx",         6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARRAx",         5,   0, ET::BDEM_FLOAT         },
                { L_,  "INT64_ARRAx",         5,   0, ET::BDEM_INT64         },
                { L_,  "SHORT_ARRAx",         5,   0, ET::BDEM_SHORT         },
                { L_,  "STRING_ARRx",         6,   0, ET::BDEM_STRING        },
                { L_,  "TIME_ARRAYx",        10,   0, ET::BDEM_TIME_ARRAY    },
                { L_,  "DATETIME_AR",         8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARRA",         6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARRAY",        11,   0, ET::BDEM_FLOAT_ARRAY   },
                { L_,  "INT64_ARRAY",        11,   0, ET::BDEM_INT64_ARRAY   },
                { L_,  "SHORT_ARRAY",        11,   0, ET::BDEM_SHORT_ARRAY   },
                { L_,  "STRING_ARRA",         6,   0, ET::BDEM_STRING        },

                { L_,  "DATETIME_ARx",        8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARRAx",        6,   0, ET::BDEM_DOUBLE        },
                { L_,  "FLOAT_ARRAYx",       11,   0, ET::BDEM_FLOAT_ARRAY   },
                { L_,  "INT64_ARRAYx",       11,   0, ET::BDEM_INT64_ARRAY   },
                { L_,  "SHORT_ARRAYx",       11,   0, ET::BDEM_SHORT_ARRAY   },
                { L_,  "STRING_ARRAx",        6,   0, ET::BDEM_STRING        },
                { L_,  "DATETIME_ARR",        8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARRAY",       12,   0, ET::BDEM_DOUBLE_ARRAY  },
                { L_,  "STRING_ARRAY",       12,   0, ET::BDEM_STRING_ARRAY  },

                { L_,  "DATETIME_ARRx",       8,   0, ET::BDEM_DATETIME      },
                { L_,  "DOUBLE_ARRAYx",      12,   0, ET::BDEM_DOUBLE_ARRAY  },
                { L_,  "STRING_ARRAYx",      12,   0, ET::BDEM_STRING_ARRAY  },
                { L_,  "DATETIME_ARRA",       8,   0, ET::BDEM_DATETIME      },

                { L_,  "DATETIME_ARRAx",      8,   0, ET::BDEM_DATETIME      },
                { L_,  "DATETIME_ARRAY",     14,   0, ET::BDEM_DATETIME_ARRAY},

                { L_,  "DATETIME_ARRAYx",    14,   0, ET::BDEM_DATETIME_ARRAY},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE             = DATA[ti].d_lineNum;
                const char *const ORIGSPEC = DATA[ti].d_spec_p;
                const int NUM              = DATA[ti].d_offset;
                const int FAIL             = DATA[ti].d_fail;
                const short VALUE          = DATA[ti].d_value;
                const int curLen           = strlen(ORIGSPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over leading whitespace
                for (int si = 0; si < 3; ++si) {
                    char SPEC[256];
                    ASSERT(256 > curLen + si);
                    for (int i = 0; i < si; ++i) {
                        SPEC[i] = ' ';
                    }
                    strcpy(&SPEC[si], ORIGSPEC);

                    {  // test with first initial value
                        bdem_ElemType::Type result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdempu_ElemType::
                                         parseElemType(&endPos, &result, SPEC);
                        LOOP_ASSERT(LINE, SPEC + NUM + si == endPos);
                        LOOP_ASSERT(LINE, FAIL == !!rv);
                        LOOP_ASSERT(LINE, result ==
                                               (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        bdem_ElemType::Type result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdempu_ElemType::
                                         parseElemType(&endPos, &result, SPEC);
                        LOOP_ASSERT(LINE, SPEC + NUM + si == endPos);
                        LOOP_ASSERT(LINE, FAIL == !!rv);
                        LOOP_ASSERT(LINE, result ==
                                               (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
