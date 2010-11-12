// bdempu_schema.t.cpp              -*-C++-*-

#include <bdempu_schema.h>

#include <bdempu_elemtype.h>

#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>

#include <bdem_elemtype.h>
#include <bdem_schema.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


enum { SUCCESS = 0, FAILURE = 1 };

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// We use the following abbreviations:
//  'cchar'      for 'const char'
//  'b_ca'       for 'bsl::vector<char>'
//  'Type'       for 'bdem_ElemType::Type'
//
//-----------------------------------------------------------------------------
// [ 1] parseElemType(cchar **endPos, Type *result, cchar *input);
// [ 1] generateElemType(b_ca *buffer, const Type& val, int level, int spaces);

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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

    typedef bdepu_ParserImpUtil ImpUtil;

    const char *BLANKS[] = {
        // This structure is used in "orthogonal perturbation" to test each
        // input string with every type of the <WHITESPACE>.
        "",
        "// C++ blank \n",
        "/*C blanks*/",
        "  \n\n\t/* //*/",
        "  ///*  \t\n ",
        " ",
        "  ",
        "\t",
        "\n",
        " \t",
        " \n",
        "\t\n         ",
        "\n\t           "
    };
    const int NUM_BLANKS = sizeof BLANKS / sizeof *BLANKS;

    template <class T>
    struct DataParsing {
        // Facilitate testing of parsing functions

        int         d_lineNum; // source line number
        const char *d_str;     // Input String
        int         d_blanks;  // Number of inserted 'BLANKS' before 'd_endPos'
                               // is encountered. (*)
        T           d_result;  // expected resulting value
        int         d_retVal;  // expected return value
        int         d_endPos;  // expected offset from the start position
                               // when blanks are not inserted
    };

    // (*) 'd_blanks' is used to calculate the expected 'd_endPos' (where
    // 'd_endPos' is a parameter in the parse function).  It is computed as a
    // sum of 'd_endPos' (from the 'DataParsing' 'struct') and a product of
    // 'd_blanks' and the length of the currently used element of 'BLANKS'.
    // In some cases it is more convenient to define the expected ending
    // position from the end of the input string.  The following two constants
    // are used to identify the end position as a length of the input string
    // and length of the input string minus one.

    const int LEN = 1000;     // Ending offset is length of the input
    const int LENSUB1 = 999;  // Ending offset is length of the input minus
                              // one.

    template <class T>
    struct DataGenerating {
        // Facilitate testing generating functions

        int         d_lineNum; // source line number
        const char *d_prefix;  // characters in output buffer before  a string
                               // generated from 'd_val' is placed in it
        int         d_level;   // indentation level
        int         d_spaces;  // spaces per level
        T           d_val;     // input value used to generate a string
                               // representation
        const char *d_str;     // output String (combination of 'd_prefix' and
                               // a string generated from 'd_val'
    };

//=============================================================================
//              GENERATOR FUNCTION 'g'
//-----------------------------------------------------------------------------

    static void appendToBuffer(bsl::vector<char> *buffer, const char *str)
    {
        while (*str) {
            buffer->push_back(*str);
            ++str;
        }
    }

    void g(bsl::vector<char> *buffer, const char *inStr, int index)
        // Append the content of the specified 'inStr' to the specified
        // 'buffer' replacing every occurrence of the character '@' with the
        // array element of 'BLANKS' at the specified 'index'.
    {
        // 'BLANKS' is a global array of strings representing sample set of
        // possible <WHITESPACE> strings.

        buffer->clear();
        while (*inStr != '\0') {
            if (*inStr == '@') {
                appendToBuffer(buffer, BLANKS[index]);
            }
            else {
                buffer->push_back(*inStr);
            }
            ++inStr;
        }
    }

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
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE SCHEMA
        //   Concerns:
        //     For the parsing method, the following properties must hold:
        //       When parsing is successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is correct
        //           4.  <WHITESPACE> characters are skipped
        //       When parsing is not successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The result is null-terminated.
        //       3. The generated result can be parsed back with the
        //          corresponding parser function.
        //       4. The values that were in the buffer prior to the call are
        //          not changed.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with the following valid schemas:
        //       1. Empty schema
        //       2. Schema containing one record with/without name
        //       3. Schema containing multiple records
        //
        //       Test with records with the following fields:
        //           1. <ELEMENT_TYPE> with/without <NAME>
        //           2. "RECORD" with/without <NAME>
        //           3. "RECORD_ARRAY" with/without <NAME>
        //
        //       Test records containing 0 - 2 fields.
        //
        //   Test with the following invalid inputs:
        //       1. Schema's opening/closing curly brace is missing
        //       2. <RECORD>'s "RECORD" is misspelled
        //       3. <RECORD>'s opening/closing curly brace is missing
        //       4. <RECORD>s are coma-delimited
        //       5. <FIELD>s are not coma-delimited
        //       6. <FIELD> does not have terminating ';'
        //       7. <CONSTRAINT> does not have opening/closing angle brackets
        //       8. <RECORD_TYPE_LOCATOR> starts with a digit
        //       9. <RECORD_TYPE_LOCATOR> has an opening but no closing '\"'
        //      10. <ELEMENT_TYPE> is invalid
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates indentation when 'level' is
        //   less than and greater than zero.
        //
        // Testing:
        //    parseSchema(cchar **endPos, bdem_Schema *result, cchar *input);
        //    generateSchema(b_ca *buff, cbdem_Schema& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING PARSE/GENERATE SCHEMA" << endl
                 << "=============================" << endl;

        typedef bdem_Schema T;
        T stub;      // Identifies undefined value
        T initValue; // Used to initialize result before parsing
        initValue.createRecord("init");

        T x0, x1, x2, x3, x4, x5;

        bdem_RecordDef *recordDef;

        recordDef = x1.createRecord();
        recordDef = x2.createRecord("x2");

        recordDef = x3.createRecord("3x");
        recordDef = x3.createRecord("x3");
        recordDef = x3.createRecord();

        recordDef = x4.createRecord("x4_1");

        bdem_FieldDefAttributes charAttr(bdem_ElemType::BDEM_CHAR),
                                stringAttr(bdem_ElemType::BDEM_STRING),
                                intAttr(bdem_ElemType::BDEM_INT);
        
        charAttr.setIsNullable(true);
        stringAttr.setIsNullable(true);
        intAttr.setIsNullable(true);
        recordDef->appendField(charAttr);
        recordDef->appendField(stringAttr, "Hello!");
        recordDef->appendField(intAttr, "I1");

        recordDef = x4.createRecord("x4_2");
        recordDef->appendField(stringAttr, "Hello!");
        recordDef->appendField(charAttr);

        recordDef = x4.createRecord("x4_3");
        bdem_FieldDefAttributes tableAttr1(bdem_ElemType::BDEM_TABLE);
        tableAttr1.setIsNullable(true);
        recordDef->appendField(tableAttr1, x4.lookupRecord("x4_1"));
        recordDef->appendField(tableAttr1, x4.lookupRecord("x4_1"), "x4_3_2");

        recordDef = x4.createRecord("x4_4");
        recordDef->appendField(tableAttr1, x4.lookupRecord("x4_1"), "x4_4_2");
        recordDef->appendField(tableAttr1, x4.lookupRecord("x4_1"));

        recordDef = x4.createRecord("x4_5");
        bdem_FieldDefAttributes listAttr1(bdem_ElemType::BDEM_LIST);
        listAttr1.setIsNullable(false);
        recordDef->appendField(listAttr1, x4.lookupRecord("x4_1"));
        recordDef->appendField(listAttr1, x4.lookupRecord("x4_1"), "x4_5_2");

        recordDef = x4.createRecord("x4_6");
        recordDef->appendField(listAttr1, x4.lookupRecord("x4_1"), "x4_6_2");
        recordDef->appendField(listAttr1, x4.lookupRecord("x4_1"));

        recordDef = x5.createRecord("5_1");
        recordDef->appendField(charAttr);
        recordDef->appendField(stringAttr, "Hello!");
        recordDef->appendField(intAttr, "I1");

        recordDef = x5.createRecord("5_2");
        recordDef->appendField(stringAttr, "Hello!");
        recordDef->appendField(charAttr);

        recordDef = x5.createRecord("5_3");
        bdem_FieldDefAttributes tableAttr2(bdem_ElemType::BDEM_TABLE);
        tableAttr2.setIsNullable(true);
        recordDef->appendField(tableAttr2, x5.lookupRecord("5_1"));
        recordDef->appendField(tableAttr2, x5.lookupRecord("5_1"), "5_3_2");

        recordDef = x5.createRecord("5_4");
        recordDef->appendField(tableAttr2, x5.lookupRecord("5_1"), "5_4_2");
        recordDef->appendField(tableAttr2, x5.lookupRecord("5_1"));

        recordDef = x5.createRecord("5_5");
        bdem_FieldDefAttributes listAttr2(bdem_ElemType::BDEM_LIST);
        listAttr2.setIsNullable(false);
        recordDef->appendField(listAttr2, x5.lookupRecord("5_1"));
        recordDef->appendField(listAttr2, x5.lookupRecord("5_1"), "5_5_2");

        recordDef = x5.createRecord("5_6");
        recordDef->appendField(listAttr2, x5.lookupRecord("5_1"), "5_6_2");
        recordDef->appendField(listAttr2, x5.lookupRecord("5_1"));

        if (verbose)
            cout << "\tTesting parser" << endl;
        {
            const DataParsing<T> DATA[] = {
 //-------------^
 { L_,  "@ { @ RECORD @ { @ } @ }",             5,   x1,    0,      LEN    },

 //line                    d_str                #@ result retVal  endPos
 //---- --------------------------------------- -- ------ ------  ------

 { L_,  "@{@}",                                 2,   x0,    0,      LEN    },
 { L_,  "@ { @ RECORD @ { @ } @ }",             5,   x1,    0,      LEN    },
 { L_,  "@ { @ RECORD @ x2 @ { @ } @ }",        6,   x2,    0,      LEN    },
 { L_,  "@ { @ RECORD @ \"3x\" @ { @ }"
          "@ RECORD @ x3 @ { @ }"
          "@ RECORD @ { @ } @ }",               13,   x3,    0,      LEN    },
 { L_,  "@ { @RECORD@ x4_1 @ {"
             "@ CHAR @ ;"
             "@ STRING @ \"Hello!\" @ ;"
             "@ INT @ I1 @ ;"
          "@ }"
          "@ RECORD @ x4_2 @ {"
               "@ STRING @ \"Hello!\" @ ;"
               "@ CHAR @ ;"
          "@ }"
          "@ RECORD @ x4_3 @{"
               "@ RECORD_ARRAY @ < @ {0} @ > @ ;"
               "@ RECORD_ARRAY @ < @ {0} @ > @ x4_3_2 @ ;"
          "@ }"
          "@ RECORD @ x4_4 @ {"
               "@ RECORD_ARRAY @ < @ {0} @ > @ x4_4_2 @ ;"
               "@ RECORD_ARRAY @ < @ {0} @ > @ ;"
          "@ }"
          "@ RECORD @ x4_5 @ {"
               "@ RECORD @ < @ {0} @ > @ ;"
               "@ RECORD @ < @ {0} @ > @ x4_5_2 @ ;"
          "@ }"
          "@ RECORD @ x4_6 @ {"
               "@ RECORD @ < @ {0} @ > @ x4_6_2 @ ;"
               "@ RECORD @ < @ {0} @ > @ ;"
          "@ }"
        "@ }",                                  82,   x4,    0,     LEN     },
 { L_,  "@ { @ RECORD @ x4_1 @ {"
             "@ CHAR @ ;"
             "@ STRING @ \"Hello!\" @ ;"
             "@ INT @ I1 @ ;"
          "@ }"
          "@ RECORD @ x4_2 @ { "
               "@ STRING @ \"Hello!\" @ ;"
               "@ CHAR @ ;"
          "@ }"
          "@ RECORD @ x4_3 @ {"
               "@ RECORD_ARRAY @ < @ x4_1 @ > @ ;"
               "@ RECORD_ARRAY @ < @ x4_1 @ > @ x4_3_2 @ ;"
          "@ }"
          "@ RECORD @ x4_4 @ {"
               "@ RECORD_ARRAY @ < @ x4_1 @ > @ x4_4_2 @ ;"
               "@ RECORD_ARRAY @ < @ x4_1 @ > @ ;"
          "@ }"
          "@ RECORD @ x4_5 @ {"
               "@ RECORD @ < @ x4_1 @ > @ ;"
               "@ RECORD @ < @ x4_1 @ > @ x4_5_2 @ ;"
          "@ }"
          "@ RECORD @ x4_6 @ {"
               "@ RECORD @ < @ x4_1 @ > @ x4_6_2 @ ;"
               "@ RECORD @ < @ x4_1 @ > @ ;"
          "@ }"
        "@ }",                                  82,   x4,    0,     LEN     },
 { L_,  "@ { @ RECORD @ \"5_1\" @ {"
             "@ CHAR @ ;"
             "@ STRING @ \"Hello!\" @ ;"
             "@ INT@I1 @ ;"
          "@ }"
          "@ RECORD @ \"5_2\" @ {"
               "@ STRING @ \"Hello!\" @ ;"
               "@ CHAR @ ;"
          "@ }"
          "@ RECORD @ \"5_3\" @ {"
                "@ RECORD_ARRAY @ < @ \"5_1\" @ > @;"
                "@ RECORD_ARRAY @ < @ \"5_1\" @ > @ \"5_3_2\" @ ;"
          "@ }"
          "@ RECORD @ \"5_4\" @ {"
                "@ RECORD_ARRAY @ < @ \"5_1\" @ > @ \"5_4_2\" @ ;"
                "@ RECORD_ARRAY @ < @ \"5_1\" @ > @ ;"
             "@ }"
          "@ RECORD @ \"5_5\" @ {"
                "@ RECORD @ < @ \"5_1\" @ > @ ;"
                "@ RECORD @ < @ \"5_1\" @ > @ \"5_5_2\" @ ;"
             "@ }"
          "@ RECORD @ \"5_6\" @ {"
                "@ RECORD @ < @ \"5_1\"@ > @ \"5_6_2\" @ ;"
                "@ RECORD @ < @ \"5_1\" @ > @ ;"
             "@ }"
        "@ }",                                  82,   x5,    0,      LEN     },

                // Testing with invalid input

                // One of the braces is missing
{ L_,  "@ RECORD @ x4_1 @"
           "{ @"
             "@ CHAR @ ;"
             "@ STRING @ \"Hello!\" @ ;"
             "@ INT @ I1 @ ;"
          "@ }"
        "@ }",                                   1,   x5,   FAILURE,    1    },

 { L_,  "@ { @ RECORD @ x4_1 @"
           "{ @"
             "@ CHAR @ ;"
             "@ STRING @ \"Hello!\" @ ;"
             "@ INT @ I1 @ ;"
        "@ }",                                  13,   x5,   FAILURE, LEN     }

            };
 //-------------V
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const DataParsing<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < NUM_BLANKS; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);
                    buffer.push_back('\0');

                    // Initialize 'result'
                    T result = initValue;

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result != initValue);
                    }

                    const char *endPos;
                    int res = bdempu_Schema::parseSchema(
                        &endPos, &result, &buffer.front());

                    LOOP_ASSERT(LINE, DATA_I.d_retVal == res);
                    if (0 == DATA_I.d_retVal) {
                        LOOP3_ASSERT(LINE, DATA_I.d_result, result,
                                     DATA_I.d_result == result);
                    }

                    // Verify correctness of the ending position offset
                    int expectedOffset;
                    if (LEN == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front());
                    }
                    else if (LENSUB1 == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front()) - 1;
                    }
                    else {
                        expectedOffset = DATA_I.d_endPos +
                                         DATA_I.d_blanks * strlen(BLANKS[j]);
                    }
                    int actualOffset = endPos - &buffer.front();
                    LOOP_ASSERT(LINE, expectedOffset == actualOffset);

                    if (expectedOffset != actualOffset) {
                        cout << "\t\texpected: " << expectedOffset
                             << ", " << "parsed: "
                             << actualOffset << endl;

                        cout << &buffer.front() << endl;
                    }
                }
            }
        }

        if (verbose)
            cout << "\tTesting generator" << endl;
        {
            const DataGenerating<T> DATA[] = {
 //-------------^

 //line prefix level spaces d_val                   d_str
 //---- ------ ----- ------ ----- -----------------------------------------
 { L_,   "",    -1,   1,     x0,  "{\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x0,  "  {\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x0,  "x {\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x0,  "x "
                                  "  {\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x1,  "{\n"
                                  "  RECORD {\n"
                                  "  }\n"
                                  " }",                                      },

 { L_,   "",    -1,   1,     x2,  "{\n"
                                  "  RECORD x2 {\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",    -1,   1,     x3,  "{\n"
                                  "  RECORD \"3x\" {\n"
                                  "  }\n"
                                  "  RECORD x3 {\n"
                                  "  }\n"
                                  "  RECORD {\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",    -1,   1,     x4,  "{\n"
                                  "  RECORD x4_1 {\n"
                                  "   CHAR ;\n"
                                  "   STRING \"Hello!\" ;\n"
                                  "   INT I1 ;\n"
                                  "  }\n"
                                  "  RECORD x4_2 {\n"
                                  "   STRING \"Hello!\" ;\n"
                                  "   CHAR ;\n"
                                  "  }\n"
                                  "  RECORD x4_3 {\n"
                                  "   RECORD_ARRAY < {0} > ;\n"
                                  "   RECORD_ARRAY < {0} > x4_3_2 ;\n"
                                  "  }\n"
                                  "  RECORD x4_4 {\n"
                                  "   RECORD_ARRAY < {0} > x4_4_2 ;\n"
                                  "   RECORD_ARRAY < {0} > ;\n"
                                  "  }\n"
                                  "  RECORD x4_5 {\n"
                                  "   RECORD < {0} > ;\n"
                                  "   RECORD < {0} > x4_5_2 ;\n"
                                  "  }\n"
                                  "  RECORD x4_6 {\n"
                                  "   RECORD < {0} > x4_6_2 ;\n"
                                  "   RECORD < {0} > ;\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",    -1,   1,     x5,  "{\n"
                                  "  RECORD \"5_1\" {\n"
                                  "   CHAR ;\n"
                                  "   STRING \"Hello!\" ;\n"
                                  "   INT I1 ;\n"
                                  "  }\n"
                                  "  RECORD \"5_2\" {\n"
                                  "   STRING \"Hello!\" ;\n"
                                  "   CHAR ;\n"
                                  "  }\n"
                                  "  RECORD \"5_3\" {\n"
                                  "   RECORD_ARRAY < {0} > ;\n"
                                  "   RECORD_ARRAY < {0} > \"5_3_2\" ;\n"
                                  "  }\n"
                                  "  RECORD \"5_4\" {\n"
                                  "   RECORD_ARRAY < {0} > \"5_4_2\" ;\n"
                                  "   RECORD_ARRAY < {0} > ;\n"
                                  "  }\n"
                                  "  RECORD \"5_5\" {\n"
                                  "   RECORD < {0} > ;\n"
                                  "   RECORD < {0} > \"5_5_2\" ;\n"
                                  "  }\n"
                                  "  RECORD \"5_6\" {\n"
                                  "   RECORD < {0} > \"5_6_2\" ;\n"
                                  "   RECORD < {0} > ;\n"
                                  "  }\n"
                                  " }",                                      },
            };
 //-------------V


            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const DataGenerating<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);
                bdempu_Schema::generateSchema(
                    &buffer, DATA_I.d_val, DATA_I.d_level, DATA_I.d_spaces);

                buffer.push_back('\0');
                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                T result = initValue;
                LOOP_ASSERT(LINE, DATA_I.d_val != result);
                const char *endPos;
                int ret = bdempu_Schema::parseSchema(&endPos, &result,
                    &buffer.front() + strlen(DATA_I.d_prefix));

                if (veryVerbose) {
                    P(DATA_I.d_val) P(result)
                }

                LOOP_ASSERT(LINE, 0 == ret);
                LOOP_ASSERT(LINE, DATA_I.d_val == result);
                LOOP_ASSERT(LINE, '\0' == *endPos);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS USED IN TESTING
        //   Concerns:
        //     Test that these methods generate output according to their
        //     specification with every type of input.
        //
        // Plan:
        //   To test 'g' create a table containing the input string with '@'
        //   inserted at the beginning, middle, and at the end and the
        //   expected output sting for every index in 'BLANKS'.  Verify that
        //   the result generated by 'g' is identical to the expected for every
        //   string in 'BLANKS'.
        //
        // Testing:
        //    g(bsl::vector<char> *buffer, const char *inStr, int index)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'g'" << endl
                 << "===========" << endl;
        {
            const struct TestData {
                int         d_line;
                const char *d_generated;
                const char *d_input;
                int         d_index;
            } DATA[] = {
                //d_line    d_generated             d_input    d_index
                //------    -----------             -------    -------

                {  L_,       "abcd",                "abcd",        0      },
                {  L_,       "abcd",                "@ab@cd@",     0      },
                {  L_,       "// C++ blank \n"
                             "ab"
                             "// C++ blank \n"
                             "cd"
                             "// C++ blank \n",     "@ab@cd@",     1      },
                {  L_,       "/*C blanks*/"
                             "ab"
                             "/*C blanks*/"
                             "cd"
                             "/*C blanks*/",        "@ab@cd@",     2      },
                {  L_,       "  \n\n\t/* //*/"
                             "ab"
                             "  \n\n\t/* //*/"
                             "cd"
                             "  \n\n\t/* //*/",     "@ab@cd@",     3      },
                {  L_,       "  ///*  \t\n "
                             "ab"
                             "  ///*  \t\n "
                             "cd"
                             "  ///*  \t\n ",       "@ab@cd@",     4      },
                {  L_,       " "
                             "ab"
                             " "
                             "cd"
                             " ",                   "@ab@cd@",     5      },
                {  L_,       "  "
                             "ab"
                             "  "
                             "cd"
                             "  ",                  "@ab@cd@",     6      },
                {  L_,       "\t"
                             "ab"
                             "\t"
                             "cd"
                             "\t",                  "@ab@cd@",     7      },
                {  L_,       "\n"
                             "ab"
                             "\n"
                             "cd"
                             "\n",                  "@ab@cd@",     8      },
                {  L_,       " \t"
                             "ab"
                             " \t"
                             "cd"
                             " \t",                 "@ab@cd@",     9      },
                {  L_,       " \n"
                             "ab"
                             " \n"
                             "cd"
                             " \n",                 "@ab@cd@",     10     },
                {  L_,       "\t\n         "
                             "ab"
                             "\t\n         "
                             "cd"
                             "\t\n         ",       "@ab@cd@",     11     },
                {  L_,       "\n\t           "
                             "ab"
                             "\n\t           "
                             "cd"
                             "\n\t           ",     "@ab@cd@",     12     },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> outputArray;
            int diff;
            for (int i = 0; i < NUM_DATA; ++i) {
                outputArray.clear();
                g(&outputArray, DATA[i].d_input, DATA[i].d_index);
                outputArray.push_back('\0');
                diff = strcmp(DATA[i].d_generated, &outputArray.front());
                LOOP_ASSERT(DATA[i].d_line, 0 == diff);
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
