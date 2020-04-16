// baljsn_formatter.t.cpp                                             -*-C++-*-

#include <baljsn_formatter.h>

#include <s_baltst_address.h>
#include <s_baltst_employee.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_printmethods.h>  // for printing vector
#include <bdlb_chartype.h>

#include <bdlde_utf8util.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

#include <bdlat_typetraits.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_nullableallocatedvalue.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>

#include <bdlt_timetz.h>

#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_review.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
namespace test = BloombergLP::s_baltst;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a formatter for outputting
// 'bdeat'-compatible objects in the JSON format.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::Formatter(bsl::ostream& stream, style, indent, spl);
// [ 2] ~baljsn::Formatter();
//
// MANIPULATORS
// [ 3] void openObject();
// [ 4] void closeObject();
// [ 5] void openArray();
// [ 6] void closeArray();
// [ 7] int openMember();
// [ 8] int putValue(const TYPE& value, const EncoderOptions *options);
// [ 8] int putNullValue();
// [ 9] void closeMember();
// [11] void addArrayElementSeparator();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::Formatter      Obj;
typedef baljsn::EncoderOptions Options;

Obj g(bsl::ostream& os, int style, int indent, int spl)
{
    if (-1 != style) {
        if (-1 != indent) {
            if (-1 != spl) {
                return Obj(os, style, indent, spl);                   // RETURN
            }
            else {
                return Obj(os, style, indent);                        // RETURN
            }
        }
        else {
            return Obj(os, style);                                    // RETURN
        }
    }
    else {
        return Obj(os);                                               // RETURN
    }
}

template <class TYPE>
void testPutValue(int            line,
                  int            style,
                  int            indent,
                  int            spl,
                  const TYPE&    value,
                  const Options *options,
                  bool           isValid)
{
    for (int i = 0; i < 2; ++i) {
        // i == 0, output as the value of an element, i.e. without indentation
        // i == 1, output as an array element, i.e. with indentation

        bsl::ostringstream os;
        bsl::ostringstream exp;

        Obj mX = g(os, style, indent, spl);

        if (0 == i) {
            mX.openObject();
            exp << '{';
            if (1 == style) {
                exp << '\n';
            }
        }
        else {
            mX.openArray();
            if (1 == style) {
                bdlb::Print::indent(exp, indent, spl);
            }
            exp << '[';
            if (1 == style) {
                exp << '\n';
                bdlb::Print::indent(exp, indent + 1, spl);
            }
        }

        const int rc = mX.putValue(value, options);

        if (isValid) {
            ASSERTV(line, rc, 0 == rc);

            baljsn::PrintUtil::printValue(exp, value, options);
            ASSERTV(line, i, exp.good());

            ASSERTV(line, i, os.str(), exp.str(), os.str() == exp.str());
        }
        else {
            ASSERTV(line, rc, 0 != rc);
        }

        // Restore object to a valid state

        if (0 == i) {
            mX.closeObject();
        }
        else {
            mX.closeArray();
        }
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
// Let us say that we have to encode a JSON document with the following
// information about stocks that we are interested in.  For brevity we just
// show and encode a part of the complete document.
//..
// {
//   "Stocks" : [
//     {
//       "Name" : "International Business Machines Corp",
//       "Ticker" : "IBM US Equity",
//       "Last Price" : 149.3,
//       "Dividend Yield" : 3.95
//     },
//     {
//       "Name" : "Apple Inc",
//       "Ticker" : "AAPL US Equity",
//       "Last Price" : 205.8,
//       "Dividend Yield" : 1.4
//     }
//   ]
// }
//..
// First, we specify the result that we are expecting to get:
//..
    const bsl::string EXPECTED =
        "{\n"
        "  \"Stocks\" : [\n"
        "    {\n"
        "      \"Name\" : \"International Business Machines Corp\",\n"
        "      \"Ticker\" : \"IBM US Equity\",\n"
        "      \"Last Price\" : 149.3,\n"
        "      \"Dividend Yield\" : 3.95\n"
        "    },\n"
        "    {\n"
        "      \"Name\" : \"Apple Inc\",\n"
        "      \"Ticker\" : \"AAPL US Equity\",\n"
        "      \"Last Price\" : 205.8,\n"
        "      \"Dividend Yield\" : 1.4\n"
        "    }\n"
        "  ]\n"
        "}";
//..
// Then, to encode this JSON document we create a 'baljsn::Formatter' object.
// Since we want the document to be written in a pretty, easy to understand
// format we will specify the 'true' for the 'usePrettyStyle' option and
// provide an appropriate initial indent level and spaces per level values:
//..
    bsl::ostringstream os;
    baljsn::Formatter formatter(os, true, 0, 2);
//..
// Next, we start calling the sequence of methods requires to produce this
// document.  We start with the top level object and add an element named
// 'Stocks' to it:
//..
    formatter.openObject();
    formatter.openMember("Stocks");
//..
// Then, we see that 'Stocks' is an array element so we specify the start of
// the array:
//..
    formatter.openArray();
//..
// Next, each element within 'Stocks' is an object that contains the
// information for an individual stock.  So we have to output an object here:
//..
    formatter.openObject();
//..
// We now encode the other elements in the stock object.  The 'closeMember'
// terminates the element by adding a ',' at the end.  For the last element in
// an object do not call the 'closeMember' method.
//..
    formatter.openMember("Name");
    formatter.putValue("International Business Machines Corp");
    formatter.closeMember();

    formatter.openMember("Ticker");
    formatter.putValue("IBM US Equity");
    formatter.closeMember();

    formatter.openMember("Last Price");
    formatter.putValue(149.3);
    formatter.closeMember();

    formatter.openMember("Dividend Yield");
    formatter.putValue(3.95);
    // Note no call to 'closeMember' for the last element
//..
// Then, close the first stock object and separate it from the second one using
// the 'addArrayElementSeparator' method.
//..
    formatter.closeObject();
    formatter.addArrayElementSeparator();
//..
// Next, we add another stock object.  But we don't need to separate it as it
// is the last one.
//..
    formatter.openObject();

    formatter.openMember("Name");
    formatter.putValue("Apple Inc");
    formatter.closeMember();

    formatter.openMember("Ticker");
    formatter.putValue("AAPL US Equity");
    formatter.closeMember();

    formatter.openMember("Last Price");
    formatter.putValue(205.8);
    formatter.closeMember();

    formatter.openMember("Dividend Yield");
    formatter.putValue(1.4);

    formatter.closeObject();
//..
// Similarly, we can continue to format the rest of the document.  For the
// purpose of this usage example we will complete this document.
//..
    formatter.closeArray();
    formatter.closeObject();
//..
// Once the formatting is complete the written data can be viewed from the
// stream passed to the formatter at construction.
//..
    if (verbose)
        bsl::cout << os.str() << bsl::endl;
//..
// Finally, verify the received result:
//..
    ASSERT(EXPECTED == os.str());
//..
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'addArrayElementSeparator' METHOD
        //
        // Concerns:
        //: 1 The 'addArrayElementSeparator' method outputs a ','.
        //:
        //: 2 If pretty style is selected then 'addArrayElementSeparator'
        //:   outputs a newline after the ','.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, number of times
        //:   'addArrayElementSeparator' must be called and the expected output
        //:   after calling 'addArrayElementSeparator'.  Create a formatter
        //:   object using the specified parameters and invoke
        //:   'addArrayElementSeparator' on it.  Verify that the output written
        //:   to the stream is as expected.
        //
        // Testing:
        //   void addArrayElementSeparator();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'addArrayElementSeparator' METHOD" << endl
                 << "=========================================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            int         d_numTimesMethodCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL  NT   EXPECTED
        // ----  -----  ------   ---  --   --------------------

        {   L_,    -1,     -1,   -1,  0,       ","             },
        {   L_,    -1,     -1,   -1,  0,       ","             },

        {   L_,     0,     -1,   -1,  0,       ","             },
        {   L_,     0,     -1,   -1,  0,       ","             },

        {   L_,     1,      2,    2,  0,       ","        NL   },
        {   L_,     1,      2,    2,  0,       ","        NL   },

        {   L_,    -1,     -1,   -1,  3,       ",,,,"          },
        {   L_,    -1,     -1,   -1,  3,       ",,,,"          },

        {   L_,     0,     -1,   -1,  3,       ",,,,"          },
        {   L_,     0,     -1,   -1,  3,       ",,,,"          },

        {   L_,     1,      5,    2,  3,       ","        NL
                                               ","        NL
                                               ","        NL
                                               ","        NL
                                                               },
        {   L_,     1,      5,    2,  3,       ","        NL
                                               ","        NL
                                               ","        NL
                                               ","        NL
                                                               },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const int         NT     = DATA[i].d_numTimesMethodCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) {
                mX.addArrayElementSeparator();
            }

            mX.addArrayElementSeparator();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INTERLEAVING OBJECT AND ARRAY CALLS
        //
        // Concerns:
        //: 1 Interleaving the object and array method calls, 'openObject',
        //:   'openArray', 'closeObject', and 'closeArray' result in the
        //:   appropriate indentation being output.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, the sequence of method
        //:   calls, and the expected output after calling these methods.
        //:   Create a formatter object using the specified parameters and
        //:   invoke the sequence of calls on it.  Verify that the output
        //:   written to the stream is as expected.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INTERLEAVING OBJECT AND ARRAY CALLS"
                          << endl
                          << "==========================================="
                          << endl;

#define NL "\n"

        // Interleave 'openObject' and 'openArray'
        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bsl::string d_methodCalls; // Represent the sequence of method
                                       // calls as a string with the following
                                       // notation:
                                       // '{' - 'openObject'
                                       // '[' - 'openArray'
                                       // ']' - 'closeArray'
                                       // '}' - 'closeObject'
            bsl::string d_expected;
        } DATA[] = {

       // LINE  STYLE  INDENT   SPL  NT        EXPECTED
       // ----  -----  ------   ---  ---       -------

        {   L_,    -1,     -1,   -1,  "",              ""                },
        {   L_,     0,     -1,   -1,  "",              ""                },
        {   L_,     1,      1,    2,  "",              ""                },

        {   L_,    -1,     -1,   -1,  "{}",            "{}"              },
        {   L_,     0,     -1,   -1,  "{}",            "{}"              },
        {   L_,     1,      1,    2,  "{}",            "{"           NL
                                                                     NL
                                                       "  }"             },

        {   L_,    -1,     -1,   -1,  "[]",            "[]"              },
        {   L_,     0,     -1,   -1,  "[]",            "[]"              },
        {   L_,     1,      1,    2,  "[]",            "  ["         NL
                                                                     NL
                                                       "  ]"             },

        {   L_,    -1,     -1,   -1,  "{[]}",          "{[]}"            },
        {   L_,     0,     -1,   -1,  "{[]}",          "{[]}"            },
        {   L_,     1,      1,    2,  "{[]}",          "{"           NL
                                                       "["           NL
                                                                     NL
                                                       "    ]"       NL
                                                       "  }"             },

        {   L_,    -1,     -1,   -1,  "{[{}]}",        "{[{}]}"          },
        {   L_,     0,     -1,   -1,  "{[{}]}",        "{[{}]}"          },
        {   L_,     1,      1,    2,  "{[{}]}",        "{"           NL
                                                       "["           NL
                                                       "      {"     NL
                                                                     NL
                                                       "      }"     NL
                                                       "    ]"       NL
                                                       "  }"             },

        {   L_,    -1,     -1,   -1,  "{[{[]}]}",      "{[{[]}]}"            },
        {   L_,     0,     -1,   -1,  "{[{[]}]}",      "{[{[]}]}"            },
        {   L_,     1,      1,    2,  "{[{[]}]}",      "{"           NL
                                                       "["           NL
                                                       "      {"     NL
                                                       "["           NL
                                                                     NL
                                                       "        ]"   NL
                                                       "      }"     NL
                                                       "    ]"       NL
                                                       "  }"             },

        {   L_,    -1,     -1,   -1,  "{[{[{}]}]}",    "{[{[{}]}]}"         },
        {   L_,     0,     -1,   -1,  "{[{[{}]}]}",    "{[{[{}]}]}"         },
        {   L_,     1,      1,    2,  "{[{[{}]}]}",    "{"           NL
                                                       "["           NL
                                                       "      {"     NL
                                                       "["           NL
                                                       "          {" NL
                                                                     NL
                                                       "          }" NL
                                                       "        ]"   NL
                                                       "      }"     NL
                                                       "    ]"       NL
                                                       "  }"             },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bsl::string CALLS  = DATA[i].d_methodCalls;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (size_t j = 0; j < CALLS.size(); ++j) {
                switch (CALLS[j]) {
                  case '{': mX.openObject(); break;
                  case '[': mX.openArray(); break;
                  case ']': mX.closeArray(); break;
                  case '}': mX.closeObject(); break;
                  default: ASSERT(0);
                }
            }

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'closeMember' METHOD
        //
        // Concerns:
        //: 1 The 'closeMember' method outputs a ','.
        //:
        //: 2 If pretty style is selected then 'closeMember' outputs a newline
        //:   after the ','.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, number of times
        //:   'closeMember' must be called and the expected output after
        //:   calling 'closeMember'.  Create a formatter object using the
        //:   specified parameters and invoke 'closeMember' on it.  Verify
        //:   that the output written to the stream is as expected.
        //
        // Testing:
        //   void closeMember();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeMember' METHOD" << endl
                          << "=============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            int         d_numTimesCloseMemberCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL  NT   EXPECTED
        // ----  -----  ------   ---  ---  -------

        {   L_,    -1,     -1,   -1,  0,       ","             },
        {   L_,    -1,     -1,   -1,  0,       ","             },

        {   L_,     0,     -1,   -1,  0,       ","             },
        {   L_,     0,     -1,   -1,  0,       ","             },

        {   L_,     1,      2,    2,  0,       ","        NL   },
        {   L_,     1,      2,    2,  0,       ","        NL   },

        {   L_,    -1,     -1,   -1,  3,       ",,,,"          },
        {   L_,    -1,     -1,   -1,  3,       ",,,,"          },

        {   L_,     0,     -1,   -1,  3,       ",,,,"          },
        {   L_,     0,     -1,   -1,  3,       ",,,,"          },

        {   L_,     1,      5,    2,  3,       ","        NL
                                               ","        NL
                                               ","        NL
                                               ","        NL
                                                               },
        {   L_,     1,      5,    2,  3,       ","        NL
                                               ","        NL
                                               ","        NL
                                               ","        NL
                                                               },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const int         NT     = DATA[i].d_numTimesCloseMemberCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) mX.closeMember();

            mX.closeMember();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
       case 8: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' METHOD
        //
        // Concerns:
        //: 1 The 'putValue' method outputs the value of the element being
        //:   encoded irrespective of the type of 'value'.
        //:
        //: 2 Errorneous values of 'value' cause 'putValue' to return an error.
        //:
        //: 3 The 'putNullValue' outputs a null value.
        //:
        //: 4 The 'putValue' and 'putNullValue' methods indent before
        //:   outputting their value only if the value is part of an array.
        //:   Otherwise no indentation is done.
        //:
        // Plan:
        //: 1 For all the possible data types create atleast one valid value
        //:   and an invalid value (if an invalid value exists) and invoke
        //:   'putValue' on them.
        //:
        //: 2 Confirm that 'putValue' returns 0 and correctly encodes the valid
        //:   values and returns a non-zero values for invalid values.
        //:
        //: 3 Confirm that 'putValue' indents only for values in an array.
        //:
        //: 4 Repeat steps 1-3 for 'putNullValue'.
        //
        // Testing:
        //   int putValue(const TYPE& value, const EncoderOptions *options);
        //   void putNullValue();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'putValue' METHOD" << endl
                          << "=========================" << endl;

        const bool                A = true;
        const char                B = 'A';
        const signed char         C = '\"';
        const unsigned char       D =
                                     bsl::numeric_limits<unsigned char>::max();
        const short               E = -1;
        const unsigned short      F =
                                    bsl::numeric_limits<unsigned short>::max();
        const int                 G = -10;
        const unsigned int        H = bsl::numeric_limits<unsigned int>::max();
        const bsls::Types::Int64  I = -100;
        const bsls::Types::Uint64 J =
                               bsl::numeric_limits<bsls::Types::Uint64>::max();
        const float               K = -1.5;
        const double              L = 10.5;
        const char               *M = "one";
        const bsl::string         N = "one";
        const bdldfp::Decimal64   O = BDLDFP_DECIMAL_DD(1.13);
        const bdlt::Date          PA(2000,  1, 1);
        const bdlt::Time          QA(0, 1, 2, 3);
        const bdlt::Datetime      R(PA, QA);
        const bdlt::DateTz        S(PA, -5);
        const bdlt::TimeTz        T(QA, -5);
        const bdlt::DatetimeTz    U(R, -5);

        const float               INV1 =
                                        bsl::numeric_limits<float>::infinity();
        const double              INV2 =
                                       bsl::numeric_limits<double>::infinity();
        const char               *INV3 = "\x80";
        const bsl::string         INV4 = "\xc2\x00";

        const Options DO;  const Options *DP = &DO;

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
        } DATA[] = {

            // LINE  STYLE  INDENT   SPL
            // ----  -----  ------   ---

            {   L_,    -1,     -1,   -1  },
            {   L_,    -1,     -1,    2  },
            {   L_,    -1,      3,    2  },

            {   L_,     0,     -1,   -1  },
            {   L_,     0,     -1,    2  },
            {   L_,     0,      3,    2  },

            {   L_,     1,      1,    2  },
            {   L_,     1,      2,    4  },
            {   L_,     1,      3,    5  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE = DATA[i].d_line;
            const int         ES   = DATA[i].d_encodingStyle;
            const int         IIL  = DATA[i].d_initialIndentLevel;
            const int         SPL  = DATA[i].d_spacesPerLevel;

            testPutValue(LINE, ES, IIL, SPL, A,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, B,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, C,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, D,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, E,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, F,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, G,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, H,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, I,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, J,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, K,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, L,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, M,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, N,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, O,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, PA,   DP,   true);
            testPutValue(LINE, ES, IIL, SPL, QA,   DP,   true);
            testPutValue(LINE, ES, IIL, SPL, R,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, S,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, T,    DP,   true);
            testPutValue(LINE, ES, IIL, SPL, U,    DP,   true);

            testPutValue(LINE, ES, IIL, SPL, INV1, DP,   false);
            testPutValue(LINE, ES, IIL, SPL, INV2, DP,   false);
            testPutValue(LINE, ES, IIL, SPL, INV3, DP,   false);
            testPutValue(LINE, ES, IIL, SPL, INV4, DP,   false);

            Options opts;  const Options *OPTS = &opts;
            opts.setEncodeInfAndNaNAsStrings(true);

            testPutValue(LINE, ES, IIL, SPL, INV1, OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, INV2, OPTS, true);

            opts.setDatetimeFractionalSecondPrecision(6);
            testPutValue(LINE, ES, IIL, SPL, PA,   OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, QA,   OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, R,    OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, S,    OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, T,    OPTS, true);
            testPutValue(LINE, ES, IIL, SPL, U,    OPTS, true);

            // testPutNullValue
            {
                for (int j = 0; j < 2; ++j) {
                    // j == 0, output as value of element, i.e. w/o indentation
                    // j == 1, output as array element, i.e. with indentation

                    bsl::ostringstream os;
                    bsl::ostringstream exp;

                    Obj mX = g(os, ES, IIL, SPL);

                    if (0 == j) {
                        mX.openObject();
                        exp << '{';
                        if (1 == ES) {
                            exp << '\n';
                        }
                    }
                    else {
                        mX.openArray();
                        if (1 == ES) {
                            bdlb::Print::indent(exp, IIL, SPL);
                        }
                        exp << '[';
                        if (1 == ES) {
                            exp << '\n';
                            bdlb::Print::indent(exp, IIL + 1, SPL);
                        }
                    }

                    mX.putNullValue();

                    exp << "null";

                    ASSERTV(LINE, os.str(), exp.str(), os.str() == exp.str());

                    // Restore object to a valid state

                    if (0 == j) {
                        mX.closeObject();
                    }
                    else {
                        mX.closeArray();
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'openMember' METHOD
        //
        // Concerns:
        //: 1 The 'openMember' method outputs the name of the element followed
        //:   by ':'.
        //:
        //: 2 If pretty style is selected then 'openMember' indents before
        //:   printing element name and outputs the ':' character with spaces
        //:   around it.
        //:
        //: 3 Providing an invalid (non-UTF8) element name results in a
        //:   non-zero value being returned.  Otherwise 0 is returned.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, element name, expected
        //:   return value, and the expected output after calling
        //:   'openMember'.  Create a formatter object using the specified
        //:   parameters and invoke 'openMember' on it.  Verify that the
        //:   output written to the stream is as expected.
        //
        // Testing:
        //   void openMember(const bsl::string& name);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openMember' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bsl::string d_elementName;
            int         d_expRetCode;
            bsl::string d_expected;
        } DATA[] = {
     // LINE  STYLE  INDENT   SPL   NAME    EXP_RC   EXPECTED
     // ----  -----  ------   ---   ----    ------   -------

     {   L_,    -1,     -1,   -1,   "",         0,   "\"\":"              },
     {   L_,    -1,     -1,   -1,   "name",     0,   "\"name\":"          },

     {   L_,     0,     -1,   -1,   "",         0,   "\"\":"              },
     {   L_,     0,     -1,   -1,   "",         0,   "\"\":"              },

     {   L_,     1,     -1,   -1,   "",         0,   "\"\" : "            },
     {   L_,     1,     -1,   -1,   "",         0,   "\"\" : "            },

     {   L_,     1,      1,    2,   "",         0,   "  \"\" : "          },
     {   L_,     1,      2,    3,   "",         0,   "      \"\" : "      },

     {   L_,     0,     -1,   -1,   "name",     0,   "\"name\":"          },
     {   L_,     0,     -1,   -1,   "name",     0,   "\"name\":"          },

     {   L_,     1,     -1,   -1,   "name",     0,   "\"name\" : "        },
     {   L_,     1,     -1,   -1,   "name",     0,   "\"name\" : "        },

     {   L_,     1,      1,    2,   "name",     0,   "  \"name\" : "      },
     {   L_,     1,      2,    3,   "name",     0,   "      \"name\" : "  },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bsl::string NAME   = DATA[i].d_elementName;
            const int         EXP_RC = DATA[i].d_expRetCode;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            const int rc = mX.openMember(NAME);

            os << bsl::flush;

            ASSERTV(LINE, EXP_RC, rc, EXP_RC == rc);
            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'closeArray' METHOD
        //
        // Concerns:
        //: 1 The 'closeArray' method outputs a ']'.
        //:
        //: 2 If pretty style is selected then 'closeArray' indents before
        //:   printing ']'.
        //:
        //: 3 Each invocation of 'closeArray' decreases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, element name, expected
        //:   return value, and the expected output after calling 'closeArray'.
        //:   Create a formatter object using the specified parameters and
        //:   invoke 'closeArray' on it.  Verify that the output written to the
        //:   stream is as expected.
        //
        // Testing:
        //   void closeArray();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeArray' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_formatAsEmptyArray;
            int         d_numTimesCloseArrayCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    FAEA    NT   EXPECTED
        // ----  -----  ------   ---    ----    ---  -------

        {   L_,    -1,     -1,   -1,  false,    0,       "[]"            },
        {   L_,    -1,     -1,   -1,  true,     0,       "[]"            },

        {   L_,     0,     -1,   -1,  false,    0,       "[]"            },
        {   L_,     0,     -1,   -1,  true,     0,       "[]"            },

        {   L_,     1,      2,    2,  false,    0,       "    ["
                                                    NL
                                                    NL   "    ]"         },
        {   L_,     1,      2,    2,  true,     0,       "    []"        },

        {   L_,    -1,     -1,   -1,  false,    3,       "[[[[]]]]"      },
        {   L_,    -1,     -1,   -1,  true,     3,       "[[[[]]]]"      },

        {   L_,     0,     -1,   -1,  false,    3,       "[[[[]]]]"      },
        {   L_,     0,     -1,   -1,  true,     3,       "[[[[]]]]"      },

        {   L_,     1,      1,    2,  false,    3,       "  ["
                                                    NL   "    ["
                                                    NL   "      ["
                                                    NL   "        ["
                                                    NL
                                                    NL   "        ]"
                                                    NL   "      ]"
                                                    NL   "    ]"
                                                    NL   "  ]"
                                                                        },
        // Nesting arrays when format as empty array is true will result in
        // nonsense, as expected.
        {   L_,     1,      1,    2,  true,    3,        "  [  [  [  []]]]"
                                                                        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        FAEA   = DATA[i].d_formatAsEmptyArray;
            const int         NT     = DATA[i].d_numTimesCloseArrayCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) {
                mX.openArray(FAEA);
            }

            mX.openArray(FAEA);
            mX.closeArray(FAEA);

            for (int k = 0; k < NT; ++k) {
                mX.closeArray(FAEA);
            }

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'openArray' METHOD
        //
        // Concerns:
        //: 1 The 'openArray' method outputs a '['.
        //:
        //: 2 If pretty style is selected then 'openArray' does not result in
        //:   in indentation before printing '['.
        //:
        //: 3 Each invocation of 'openArray' increases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, element name, expected
        //:   return value, and the expected output after calling 'openArray'.
        //:   Create a formatter object using the specified parameters and
        //:   invoke 'openArray' on it.  Verify that the output written to the
        //:   stream is as expected.
        //
        // Testing:
        //   void openArray();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openArray' METHOD" << endl
                          << "===========================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_formatAsEmptyArray;
            int         d_numTimesOpenArrayCalled;
            bsl::string d_expected;
        } DATA[] = {
        // LINE  STYLE  INDENT   SPL    FAEA    NT  EXPECTED
        // ----  -----  ------   ---    ----   ---  --------

        {   L_,    -1,     -1,   -1,  false,    0,  "["                 },
        {   L_,    -1,     -1,   -1,  true,     0,  "["                 },

        {   L_,     0,     -1,   -1,  false,    0,  "["                 },
        {   L_,     0,     -1,   -1,  true,     0,  "["                 },

        {   L_,     1,      1,    2,  false,    0,  "  ["           NL  },
        {   L_,     1,      1,    2,  true,     0,  "  ["               },

        {   L_,    -1,     -1,   -1,  false,    3,  "[[[["              },
        {   L_,    -1,     -1,   -1,  true,     3,  "[[[["              },

        {   L_,     0,     -1,   -1,  false,    3,  "[[[["              },
        {   L_,     0,     -1,   -1,  true,     3,  "[[[["              },

        {   L_,     1,      1,    2,  false,    3,  "  ["           NL
                                                    "    ["         NL
                                                    "      ["       NL
                                                    "        ["     NL
                                                                        },
        // Nesting arrays when format as empty array is true will result in
        // nonsense, as expected.
        {   L_,     1,      1,    2,  true,     3,  "  [  [  [  ["      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        FAEA   = DATA[i].d_formatAsEmptyArray;
            const int         NT     = DATA[i].d_numTimesOpenArrayCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) mX.openArray(FAEA);

            mX.openArray(FAEA);

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());

            // Restore object to a valid state

            for (int k = 0; k <= NT; ++k) mX.closeArray(FAEA);
        }
#undef NL
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'closeObject' METHOD
        //
        // Concerns:
        //: 1 The 'closeObject' method outputs a '}'.
        //:
        //: 2 If pretty style is selected then 'closeObject' indents before
        //:   printing '}'.
        //:
        //: 3 Each invocation of 'closeObject' decreases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, element name, expected
        //:   return value, and the expected output after calling
        //:   'closeObject'.  Create a formatter object using the specified
        //:   parameters and invoke 'closeObject' on it.  Verify that the
        //:   output written to the stream is as expected.
        //
        // Testing:
        //   void closeObject();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeObject' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            int         d_numTimesCloseObjectCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL  NT   EXPECTED
        // ----  -----  ------   ---  ---  -------

        {   L_,    -1,     -1,   -1,  0,       "{}"             },

        {   L_,     0,     -1,   -1,  0,       "{}"             },

        {   L_,     1,      2,    2,  0,       "{"
                                          NL
                                          NL   "    }"          },

        {   L_,    -1,     -1,   -1,  3,       "{{{{}}}}"       },

        {   L_,     0,     -1,   -1,  3,       "{{{{}}}}"       },

        {   L_,     1,      1,    2,  3,       "{"
                                          NL   "{"
                                          NL   "{"
                                          NL   "{"
                                          NL
                                          NL   "        }"
                                          NL   "      }"
                                          NL   "    }"
                                          NL   "  }"
                                                                },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const int         NT     = DATA[i].d_numTimesCloseObjectCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) {
                mX.openObject();
            }

            mX.openObject();
            mX.closeObject();

            for (int k = 0; k < NT; ++k) {
                mX.closeObject();
            }

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'openObject' METHOD
        //
        // Concerns:
        //: 1 The 'openObject' method outputs a '{'.
        //:
        //: 2 If pretty style is selected then 'openObject' outputs a newline
        //:   after printing '{'.
        //:
        //: 3 Each invocation of 'openObject' increases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, element name, expected
        //:   return value, and the expected output after calling
        //:   'openObject'.  Create a formatter object using the specified
        //:   parameters and invoke 'openObject' on it.  Verify that the
        //:   output written to the stream is as expected.
        //
        // Testing:
        //   void openObject();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openObject' METHOD" << endl
                          << "===========================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            int         d_numTimesOpenObjectCalled;
            bsl::string d_expected;
        } DATA[] = {

            // LINE  STYLE  INDENT   SPL  NT   EXPECTED
            // ----  -----  ------   ---  ---  -------

            {   L_,    -1,     -1,   -1,  0,  "{"                 },

            {   L_,     0,     -1,   -1,  0,  "{"                 },

            {   L_,     1,      1,    2,  0,  "{"             NL  },

            {   L_,    -1,     -1,   -1,  3,  "{{{{"              },

            {   L_,     0,     -1,   -1,  3,  "{{{{"              },

            {   L_,     1,      1,    2,  3,  "{"             NL
                                              "{"             NL
                                              "{"             NL
                                              "{"             NL
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const int         NT     = DATA[i].d_numTimesOpenObjectCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            for (int k = 0; k < NT; ++k) mX.openObject();

            mX.openObject();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());

            // Restore object to a valid state

            for (int k = 0; k <= NT; ++k) mX.closeObject();
        }
#undef NL
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 The specified stream is correctly initialized.
        //:
        //: 2 The specified encoder options are correctly initialized.
        //
        // Plan:
        //: 1 Create multiple objects passing them different arguments for
        //:   'stream' and the formatting options.  Invoke a manipulator method
        //:   on each object and confirm that the output is as expected.
        //
        // Testing:
        //   baljsn::Formatter(bsl::ostream& stream, style, indent, spl);
        //   ~baljsn::Formatter();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL   EXPECTED
        // ----  -----  ------   ---   --------

        // No options specified
        {   L_,    -1,     -1,   -1,   "{\"A\":1,\"B\":[2]}"                 },

        // Specify only encoding style
        {   L_,     0,     -1,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,     -1,   -1,   "{"                               NL
                                       "\"A\" : 1,"                      NL
                                       "\"B\" : ["                       NL
                                       "2"                               NL
                                       "]"                               NL
                                       "}"                                   },

        // Specify encoding style and initialIndentLevel
        {   L_,     0,      1,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     0,      5,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,      1,   -1,   "{"                               NL
                                       "\"A\" : 1,"                      NL
                                       "\"B\" : ["                       NL
                                       "2"                               NL
                                       "]"                               NL
                                       "}"                                   },

        // Specify all options
        {   L_,     0,      1,    2,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     0,      5,   10,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,      1,    2,   "{"                               NL
                                       "    \"A\" : 1,"                  NL
                                       "    \"B\" : ["                   NL
                                       "      2"                         NL
                                       "    ]"                           NL
                                       "  }"                                 },
        {   L_,     1,      3,    5,   "{"                               NL
                                       "                    \"A\" : 1,"  NL
                                       "                    \"B\" : ["   NL
                                       "                         2"      NL
                                       "                    ]"           NL
                                       "               }"                    },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os1;

            bdlsb::MemOutStreamBuf mosb;
            bsl::ostream           os2(&mosb);

            Obj mX = g(os1, STYLE, INDENT, SPL);
            Obj mY = g(os2, STYLE, INDENT, SPL);

            mX.openObject();
            mX.openMember("A"); mX.putValue(1); mX.closeMember();
            mX.openMember("B");
            mX.openArray(); mX.putValue(2); mX.closeArray();
            mX.closeObject();

            mY.openObject();
            mY.openMember("A"); mY.putValue(1); mY.closeMember();
            mY.openMember("B");
            mY.openArray(); mY.putValue(2); mY.closeArray();
            mY.closeObject();

            os1 << bsl::flush;
            os2 << bsl::flush;

            bsl::string actual(mosb.data(), mosb.length());

            ASSERTV(LINE, EXP, os1.str(), EXP == os1.str());
            ASSERTV(LINE, EXP, actual,    EXP == actual);
        }
#undef NL

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bsl::ostringstream os;

        Obj mX(os);

        bsl::string exp;

        mX.openObject();

        exp += '{';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeObject();

        exp += '}';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.openArray();

        exp += '[';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeArray();

        exp += ']';
        ASSERTV(exp, os.str(), exp == os.str());

        bsl::string name = "name";

        const int rc = mX.openMember(name);
        ASSERTV(rc, 0 == rc);

        exp += '"' + name + '"' + ':';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.putNullValue();

        exp += "null";
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeMember();

        exp += ',';
        ASSERTV(exp, os.str(), exp == os.str());
      } break;
      case -1: {
        // SimpleFormatter <-> Formatter comparison code.
        // Formatter version

        struct {
            bsl::string d_description;
            bool        d_pretty;
            int         d_spacesPerLevel;
        } DATA[] = {
            { "Not Pretty", false, 0 },
            { "Pretty",     true,  2 }
        };

        int NUM_DATA=sizeof(DATA)/sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const bsl::string DESC   = DATA[i].d_description;
            const bool        PRETTY = DATA[i].d_pretty;
            const int         SPACES = DATA[i].d_spacesPerLevel;

            bsl::ostringstream     os;

            baljsn::Formatter f(os, PRETTY, 0, SPACES);

            f.openObject();

             f.openMember("Object");
              f.openObject();
               f.openMember("Field 1");
                f.putValue(1);
               f.closeMember();
               f.openMember("Field 2");
                f.putNullValue();
               // Must remember NOT to call
               // closeMember here!
              f.closeObject();
             f.closeMember();

             f.openMember("Array");
              f.openArray();
               f.putValue(1);
               f.addArrayElementSeparator();
               f.putValue("string");
               f.addArrayElementSeparator();
               f.openArray(true);
               f.closeArray(true);
               f.addArrayElementSeparator();
               f.openArray();
               f.openArray();
               f.openObject();
               f.closeObject();
               f.closeArray();
               f.closeArray();

               // Must remember NOT to call
               // addArrayElementSeparator
               // here!
              f.closeArray();
             f.closeMember();

             f.openMember("True");
              f.putValue(true);
             // Must remember NOT to call
             // closeMember here!

            f.closeObject();

            cout << "\n\n" << DESC << ":\n" << os.str() << endl;
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
// Copyright 2017 Bloomberg Finance L.P.
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
