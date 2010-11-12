// baenet_httpparserutil.t.cpp  -*-C++-*-
#include <baenet_httpparserutil.h>

#include <baenet_httpcontenttype.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define T_ bsl::cout << "\t" << bsl::flush;   // tab without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TEST PARSING OF CONTENT TYPE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "TEST PARSING OF CONTENT TYPE" << bsl::endl
                               << "============================" << bsl::endl;

        const int SUCCESS        = 0;
        const int UNEXPECTED_EOF = -1;
        const int FAILURE        = -2;

        static const struct {
            int         d_lineNum;        // line number
            const char *d_string;         // test string
            int         d_expectedReturn; // expected return value
            const char *d_type;           // content type
            const char *d_subtype;        // content subtype
            const char *d_charset;        // charset parameter
            const char *d_boundary;       // boundary parameter
            const char *d_id;             // id parameter
            const char *d_name;           // name parameter
        } DATA[] = {
            //line   string
            //----   ------
            //       expected return      type      subtype      charset
            //       ---------------      ----      -------      -------
            //       boundary             id        name
            //       --------             --        ----

            // no optional fields
            { L_,    " a / b ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            // wrong optional field
            { L_,    " a / b ; HARSET=char ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    " a / b ; CHARSE=char ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; OUNDARY=bound ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; BOUNDAR=bound ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; I=ident ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; D=ident ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; AME=\"the name\" ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            { L_,    "a / b ; NAM=\"the name\" ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       ""                       },

            // one optional field, case insensitive
            { L_,    " a / b ; cHaRsEt=char ",
                     SUCCESS,             "a",      "b",         "char",
                     "",                  "",       ""                       },

            { L_,    "a / b ; bOuNdArY=bound ",
                     SUCCESS,             "a",      "b",         "",
                     "bound",             "",       ""                       },

            { L_,    "a / b ; iD=ident ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "ident",  ""                       },

            { L_,    "a / b ; nAmE=\"the name\" ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "",       "the name"               },

            // multiple optional fields
            { L_,    " a / b ; charset=char ; boundary = bound \r\nabc ",
                     SUCCESS,             "a",      "b",         "char",
                     "bound",             "",       ""                       },

            { L_,    " a / b ; charset=char ; name = \"the name\" \r\nabc ",
                     SUCCESS,             "a",      "b",         "char",
                     "",                   "",      "the name"               },

            { L_,    " a / b ; boundary=\"b\\\"ound\" ; charset=char \r\nabc ",
                     SUCCESS,             "a",      "b",         "char",
                     "b\"ound",           "",       ""                       },

            { L_,    " a / b ; boundary=\"b\\\"ound\" ; id=ident \r\nabc ",
                     SUCCESS,             "a",      "b",         "",
                     "b\"ound",           "ident",  ""                       },

            { L_,    " a / b ; id=ident ; charset=char \r\nabc ",
                     SUCCESS,             "a",      "b",         "char",
                     "",                  "ident",  ""                       },

            { L_,    " a / b ; id=ident ; name=\"the name\" \r\nabc ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "ident",  "the name"               },

            { L_,    " a / b ; nAmE=\"the name\" ; boundary=bound \r\nabc ",
                     SUCCESS,             "a",      "b",         "",
                     "bound",             "",       "the name"               },

            { L_,    " a / b ; nAmE=\"the name\" ; id=ident \r\nabc ",
                     SUCCESS,             "a",      "b",         "",
                     "",                  "ident",  "the name"               },

            // unknown field
            { L_,    " a / b ; charset=char ; unknown = unk ; id = ident ",
                     SUCCESS,             "a",      "b",         "char",
                     "",                  "ident",  ""                       },

            { L_,    " a / b ; boundary=\"b\\\"ound\" ; unknown = unk ; "
                     "name=\"the name\" ",
                     SUCCESS,             "a",      "b",         "",
                     "b\"ound",           "",       "the name"               },

            { L_,    " a / b ; id=ident ; unknown = unk ; boundary=bound ",
                     SUCCESS,             "a",      "b",         "",
                     "bound",             "ident",  ""                       },

            { L_,    " a / b ; nAmE=\"the name\" ; unknown = unk ; "
                     "charset=char ",
                     SUCCESS,             "a",      "b",         "char",
                     "",                  "",       "the name"               },

            // all optional fields
            { L_,    " a / b ; charset=char ; boundary = bound ; id = ident; "
                     "name = \"the name\" ",
                     SUCCESS,             "a",      "b",         "char",
                     "bound",             "ident",  "the name"               },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE            = DATA[i].d_lineNum;
            const string STRING          = DATA[i].d_string;
            const int    EXPECTED_RETURN = DATA[i].d_expectedReturn;

            const char *begin         = STRING.data();
            const char *end           = begin + STRING.length();
            const char *originalBegin = begin;

            if (veryVerbose) {
                T_ P_(LINE) P_(STRING) P(EXPECTED_RETURN)
            }

            typedef bdeut_StringRef StringRef;

            bdeut_StringRef input(begin, end);
            baenet_HttpContentType result;
            int              retCode = baenet_HttpParserUtil::parseFieldValue(
                                                                       &result,
                                                                       input);
            LOOP2_ASSERT(LINE, retCode, EXPECTED_RETURN == retCode);

            if (SUCCESS == EXPECTED_RETURN) {
                const string TYPE     = DATA[i].d_type;
                const string SUBTYPE  = DATA[i].d_subtype;
                const string CHARSET  = DATA[i].d_charset;
                const string BOUNDARY = DATA[i].d_boundary;
                const string ID       = DATA[i].d_id;
                const string NAME     = DATA[i].d_name;

                const string& type     = result.type();
                const string& subtype  = result.subType();
                string charset;
                if (!result.charset().isNull()) {
                    charset = result.charset().value();
                }
                string boundary;
                if (!result.boundary().isNull()) {
                    boundary = result.boundary().value();
                }
                string id;
                if (!result.id().isNull()) {
                    id = result.id().value();
                }
                string name;
                if (!result.name().isNull()) {
                    name = result.name().value();
                }

                LOOP3_ASSERT(LINE, TYPE,   type,
                                   TYPE == type);
                LOOP3_ASSERT(LINE, SUBTYPE ,   subtype,
                                   SUBTYPE == subtype);
                LOOP3_ASSERT(LINE, CHARSET,   charset,
                                   CHARSET == charset);
                LOOP3_ASSERT(LINE, BOUNDARY,   boundary,
                                   BOUNDARY == boundary);
                LOOP3_ASSERT(LINE, ID,   id,
                                   ID == id);
                LOOP3_ASSERT(LINE, NAME,   name,
                                   NAME == name);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST PARSING OF CHUNK HEADER LINE
        // --------------------------------------------------------------------
        //
        // Concern: hexCharTable[] may be invalid
        // Test Plan: call parser with a string: "X\r\n" for all possible
        // single character values (excluding EOS).  Hex digits will return
        // their value, all other characters will return FAILURE or
        // END_OF_FILE.
        //
        // Concern: hex numbers of 8 digits or less must be parsed, and those
        // larger then 8 digits should fail.
        // Test Plan: Call function under test with hex numbers as strings
        // of 1, 2, 8, and 9 digits.
        //
        // Concern: a CR LF may precede the hex number in the input stream
        // Test Plan: Call the function under test with a hex number with a
        // preceding CR LF and verify that they are skipped.
        //
        // Concern: that an optional string beginning with a semicolon may
        // lie between the hex number and trailing CRLF.
        // Test Plan: Call the function under test with a hex number, a
        // semi-colon and a string followed by a CRLF.  Insure that correct
        // numeric value is returned and that all of the characters including
        // the CRLF are consumed.
        //

        if (verbose) bsl::cout << bsl::endl
                               << "TEST PARSING OF CHUNK HEADER" << bsl::endl
                               << "============================" << bsl::endl;

        enum { FAILURE = -1, SUCCESS = 0, END_OF_FILE = 1 };

        if (veryVerbose) {
            bsl::cout << "Testing hexCharTable." << bsl::endl;
        }

        // test hexCharTable
        for (int c=1; c <= 0xff; ++c) { // skip EOS
            char buf[4];
            buf[0] = static_cast<char>(c);
            buf[1] = '\r';
            buf[2] = '\n';
            buf[3] = '\0';

            if (veryVeryVerbose) {
                T_ P_(L_) P(c)
            }
            int            result = 0;
            int            consumed = 0;
            bsl::stringbuf input(buf);
            int            retCode = baenet_HttpParserUtil::parseChunkHeader(
                                                                  &result,
                                                                  &consumed,
                                                                  &input);
            bsl::istringstream in(buf);
            int n;
            in >> bsl::hex;

            if (in >> n) {
                LOOP2_ASSERT(c, retCode, SUCCESS == retCode);
                LOOP2_ASSERT(c, result,  result  == n);
            }
            else {
                LOOP2_ASSERT(c, retCode, SUCCESS != retCode);
            }
        }

        static struct {
            int         d_lineNum;          // line number
            const char *d_string;           // test string
            int         d_expectedResult;   // expected result
            int         d_expectedReturn;   // expected return value
            int         d_expectedConsumed; // expected consumed
        } DATA[] = {
            //line   string                      result     return     consumed
            //----   ------                      -----       --------- --------
            // add-hoc tests (code coverage complete)
            // Note: no contract on consumed length unless SUCCESS returned
            { L_, "",                             0,         END_OF_FILE, -1 },
            { L_, "1",                            0,         END_OF_FILE, -1 },
            { L_, "10",                           0,         END_OF_FILE, -1 },
            { L_, "10\r",                         0,         END_OF_FILE, -1 },
            { L_, "10\r;",                        0,         FAILURE,     -1 },
            { L_, "10\r\n",                       16,        SUCCESS,     4  },
            { L_, "\r",                           0,         END_OF_FILE, -1 },
            { L_, "\r\n",                         0,         END_OF_FILE, -1 },
            { L_, "\r\nFEDCBA\r\n",               0xFEDCBA,  SUCCESS,     10 },
            { L_, "0\r\n",                        0,         SUCCESS,     3  },
            { L_, "z\r\n",                        0,         FAILURE,     -1 },
            { L_, "1abcdef; ignorestuffhere\r\n", 0x1abcdef, SUCCESS,     26 },
            { L_, "123456789",                    0,         FAILURE,     -1 },

            // test hex numbers of varying length
            { L_, "1\r\n",                        1,         SUCCESS,     3  },
            { L_, "23\r\n",                       0x23,      SUCCESS,     4  },
            { L_, "456789AB\r\n",                 0x456789AB,SUCCESS,     10 },
            { L_, "456789ABC\r\n",                0,         FAILURE,     -1 },

            // test hex numbers of varying length and trailing options
            { L_, "1;\r\n",                       1,         SUCCESS,     4  },
            { L_, "1; foo=bar\r\n",               1,         SUCCESS,     12 },
            { L_, "23;\r\n",                      0x23,      SUCCESS,     5  },
            { L_, "23; somelongoption=somelongvalue\r\n", 0x23, SUCCESS,  34 },
            { L_, "456789AB;\r\n",                0x456789AB,SUCCESS,     11 },
            { L_, "456789AB; FOO=BAR\r\n",        0x456789AB,SUCCESS,     19 },
            { L_, "456789ABC;\r\n",               0,         FAILURE,     -1 },
            { L_, "456789ABC; BAR=FOO\r\n",       0,         FAILURE,     -1 },

            // test entries will trailing content
            { L_, "1\r\nsome content",            1,         SUCCESS,     3  },
            { L_, "23\r\n\r\n",                   0x23,      SUCCESS,     4  },
            { L_, "456789AB\r\n;abcd",            0x456789AB,SUCCESS,     10 },
            { L_, "456789ABC\r\n123\r\n",         0,         FAILURE,     -1 },
            { L_, "1;\r\n;;",                     1,         SUCCESS,     4  },
            { L_, "1; foo=bar\r\nxyzzy",          1,         SUCCESS,     12 },
            { L_, "23;\r\nall good",              0x23,      SUCCESS,     5  },
            { L_, "23; somelongoption=somelongvalue\r\nand more",
                                                  0x23,      SUCCESS,     34 },
            { L_, "456789AB;\r\none more",        0x456789AB,SUCCESS,     11 },
            { L_, "456789AB; FOO=BAR\r\n; and anotther",
                                                  0x456789AB,SUCCESS,     19 },
            { L_, "456789ABC;\r\n; but at last",  0,         FAILURE,     -1 },
            { L_, "456789ABC; BAR=FOO\r\ndone",   0,         FAILURE,     -1 },
        };

        if (veryVerbose) {
            bsl::cout << "Testing with DATA table." << bsl::endl;
        }

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int      LINE              = DATA[i].d_lineNum;
            const string   STRING            = DATA[i].d_string;
            const int      EXPECTED_RESULT   = DATA[i].d_expectedResult;
            const int      EXPECTED_RETURN   = DATA[i].d_expectedReturn;
            const int      EXPECTED_CONSUMED = DATA[i].d_expectedConsumed;

            if (veryVeryVerbose) {
                T_ P_(LINE) P_(STRING) P_(EXPECTED_RESULT)
                   P_(EXPECTED_RETURN) P_(EXPECTED_CONSUMED)
            }
            int            result = 0, consumed = 0;
            bsl::stringbuf input(STRING);
            int            retCode = baenet_HttpParserUtil::parseChunkHeader(
                                                                  &result,
                                                                  &consumed,
                                                                  &input);
            LOOP2_ASSERT(LINE, retCode, EXPECTED_RETURN == retCode);

            if (SUCCESS == retCode) {
                LOOP2_ASSERT(LINE, consumed, EXPECTED_CONSUMED == consumed);
                LOOP2_ASSERT(LINE, result, EXPECTED_RESULT == result);
            }
        };
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
