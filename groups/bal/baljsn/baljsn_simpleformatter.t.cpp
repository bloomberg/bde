// baljsn_simpleformatter.t.cpp                                       -*-C++-*-
#include <baljsn_simpleformatter.h>

#include <balb_testmessages.h>

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

#include <bdldfp_decimal.h>

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

#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>
#include <bsls_types.h>

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
using namespace bsl;

// Suppress some bde_verify warnings for test driver.
// BDE_VERIFY pragma: -AL01
// BDE_VERIFY pragma: -AR01
// BDE_VERIFY pragma: -FABC01
// BDE_VERIFY pragma: -FD01
// BDE_VERIFY pragma: -SP01
// BDE_VERIFY pragma: -SP03
// BDE_VERIFY pragma: -IND01

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
// [ 2] SimpleFormatter(ostream& stream, Allocator *);
// [ 2] SimpleFormatter(ostream&, const EncoderOptions&, Allocator *);
// [ 2] SimpleFormatter(const SimpleFormatter &, bslma::Allocator *);
// [ 2] ~SimpleFormatter();
//
// MANIPULATORS
// [ 3] void openObject();
// [ 3] void openObject(const bslstl::StringRef& name);
// [ 3] bool isFormattingObject() const;
// [ 4] void closeObject();
// [ 5] void openArray(bool formatAsEmptyArray);
// [ 5] bool isFormattingArray() const;
// [ 6] void closeArray();
// [ 6] bool isCompleteJSON() const;
// [ 7] void addMemberName(const bslstl::StringRef&);
// [ 7] int addValue(const TYPE& value);
// [ 7] int addValue(const StringRef&, const TYPE&);
// [ 7] void addNullValue();
// [ 7] void addNullValue(const bslstl::StringRef& name);
// [ 7] bool isNameNeeded() const;
// [ 8] void openArray(bslstl::StringRef name, bool formatAsEmptyArray);
//
// ACCESSORS
// [ 9] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [-1] SimpleFormatter<->Formatter comparison - SimpleFormatter version

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------
#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::SimpleFormatter      Obj;
typedef baljsn::EncoderOptions       Options;

Obj g(bsl::ostream& os, int style, int indent, int spl)
{
    if (-1 == style && -1 == indent && -1 == spl) {
        return Obj(os);                                               // RETURN
    }

    Options encoderOptions;

    if (-1 != style) {
        encoderOptions.setEncodingStyle(baljsn::EncodingStyle::Value(style));
    }

    if (-1 != indent) {
        encoderOptions.setInitialIndentLevel(indent);
    }

    if (-1 != spl) {
        encoderOptions.setSpacesPerLevel(spl);
    }

    return Obj(os, encoderOptions);

}

template <class TYPE>
void testPutValue(int            line,
                  int            callLine,
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

        Options encoderOptions(*options);

        if (-1 != style) {
            encoderOptions.setEncodingStyle(
                baljsn::EncodingStyle::Value(style));
        }

        if (-1 != indent) {
            encoderOptions.setInitialIndentLevel(indent);
        }

        if (-1 != spl) {
            encoderOptions.setSpacesPerLevel(spl);
        }

        Obj mX(os, encoderOptions);

        ASSERT(!mX.isCompleteJSON());

        if (1 == style) {
            bdlb::Print::indent(exp, indent, spl);
        }

        if (0 == i) {
            mX.openObject();
            exp << '{';

            bsl::string spaceOrNot;

            if (1 == style) {
                spaceOrNot = " ";
                exp << '\n';
                bdlb::Print::indent(exp, indent + 1, spl);
            }

            ASSERT(mX.isNameNeeded());
            mX.addMemberName("name");
            ASSERT(!mX.isNameNeeded());

            exp << "\"name\"" << spaceOrNot << ":" << spaceOrNot;
        }
        else {
            mX.openArray();
            exp << '[';
            if (1 == style) {
                exp << '\n';
                bdlb::Print::indent(exp, indent + 1, spl);
            }

            ASSERT(!mX.isNameNeeded());
        }

        ASSERT(!mX.isCompleteJSON());

        const int rc = mX.addValue(value);

        ASSERT(!mX.isCompleteJSON());

        if (isValid) {
            ASSERTV(line, callLine, rc, 0 == rc);

            baljsn::PrintUtil::printValue(exp, value, options);
            ASSERTV(line, callLine, exp.good());

            ASSERTV(
                line, callLine, os.str(), exp.str(), os.str() == exp.str());
        }
        else {
            ASSERTV(line, callLine, rc, 0 != rc);
        }

        // Restore object to a valid state

        if (0 == i) {
            mX.closeObject();
        }
        else {
            mX.closeArray();
        }

        ASSERT(!mX.isNameNeeded());
        ASSERT( mX.isCompleteJSON());
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
      case 10: {
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
///Example 1: Encoding a Stock Portfolio in JSON
///- - - - - - - - - - - - - - - - - - - - - - -
// Let us say that we have to encode a JSON document with the following
// information about stocks that we are interested in.  For brevity we just
// show and encode a part of the complete document.
//
// First, we specify the result that we are expecting to get:
//..
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
    const bsl::string EXPECTED =
R"JSON({
  "Stocks" : [
    {
      "Name" : "International Business Machines Corp",
      "Ticker" : "IBM US Equity",
      "Last Price" : 149.3,
      "Dividend Yield" : 3.95
    },
    {
      "Name" : "Apple Inc",
      "Ticker" : "AAPL US Equity",
      "Last Price" : 205.8,
      "Dividend Yield" : 1.4
    }
  ]
})JSON";
#else
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
#endif // def BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
//..
// Then, to encode this JSON document we create a 'baljsn::SimpleFormatter'
// object.  Since we want the document to be written in a pretty, easy to
// understand format we will specify the 'true' for the 'usePrettyStyle' option
// and provide an appropriate initial indent level and spaces per level values:
//..
    bsl::ostringstream      os;
    baljsn::EncoderOptions  encoderOptions;

    encoderOptions.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
    encoderOptions.setSpacesPerLevel(2);

    baljsn::SimpleFormatter formatter(os, encoderOptions);
//..
// Next, we start calling the sequence of methods requires to produce this
// document.  We start with the top level object and add an array element named
// 'Stocks' to it:
//..
    formatter.openObject();
    formatter.openArray("Stocks");
//..
// Next, each element within 'Stocks' is an object that contains the
// information for an individual stock.  So we have to output an object here:
//..
    formatter.openObject();
//..
// We now encode the other elements in the stock object.
//..
    formatter.addValue("Name", "International Business Machines Corp");
    formatter.addValue("Ticker", "IBM US Equity");
    formatter.addValue("Last Price", 149.3);
    formatter.addValue("Dividend Yield", 3.95);
//..
// Then, close the first stock object.
//..
    formatter.closeObject();
//..
// Next, we add another stock object.
//..
    formatter.openObject();

    formatter.addValue("Name", "Apple Inc");
    formatter.addValue("Ticker", "AAPL US Equity");
    formatter.addValue("Last Price", 205.8);
    formatter.addValue("Dividend Yield", 1.4);

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
}
//..
//
///Example 2: Encoding an array
///- - - - - - - - - - - - - - -
// Let us say we want to encode an array of various values.
//
// First, we create our 'formatter' as we did above:
//..
{
    bsl::ostringstream      os;
    baljsn::EncoderOptions  encoderOptions;

    encoderOptions.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
    encoderOptions.setSpacesPerLevel(2);

    baljsn::SimpleFormatter formatter(os, encoderOptions);
//..
// Then we open our array.
//..
    formatter.openArray();
//..
// Next, we populate the array with a series of unnamed values.  Named values
// are only used in objects, not arrays.
//..
    formatter.addValue("First value");
    formatter.addValue(2);
    formatter.addValue(3);
//..
// Then, we demonstrate that arrays can be nested, opening another level of
// array, populating it, and closing it:
//..
    formatter.openArray();
    formatter.addValue("First value of inner array");
    formatter.addValue(3.14159);
    formatter.closeArray();
//..
// Arrays can also contain (unnamed) objects:
//..
    formatter.openObject();
//..
// Next, we add (named) values to our object:
//..
    formatter.addValue("Greeting", "Hello from the first inner object");
    formatter.addValue("PI approximation", 3.14);
    // We could, similarly, add nested named objects and/or named arrays
//..
// Then we close the nested object:
//..
    formatter.closeObject();
//..
// Finally, we close the outer array:
//..
    formatter.closeArray();
}
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'allocator' METHOD
        //
        // Concerns:
        //: 1 The 'allocator' method returns the allocator passed in at
        //:   construction.
        //:
        //: 2 If no allocator is passed at construction, 'allocator()' returns
        //:   the installed default allocator.
        //
        // Plan:
        //: 1 Construct 'SimpleFormatter' objects with and without an allocator
        //:   argument and check the result of calling 'allocator()'.
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n"
                 << "TESTING 'allocator' METHOD" << "\n"
                 << "==========================" << endl;

        bsl::ostringstream os;
        Obj                withoutAllocatorArg(os);
        ASSERTV(withoutAllocatorArg.allocator(),
               bslma::Default::allocator(0),
               withoutAllocatorArg.allocator() ==
                   bslma::Default::allocator(0));

        bslma::TestAllocator ta("test", false);

        Obj withAllocatorArg(os, &ta);
        ASSERTV(withAllocatorArg.allocator(),
               &ta,
               withAllocatorArg.allocator() == &ta);

        Obj withAllocatorArgCopy(withoutAllocatorArg, &ta);
        ASSERTV(withAllocatorArgCopy.allocator(),
               &ta,
               withAllocatorArgCopy.allocator() == &ta);
      } break;
      case 8: {
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
        //   void openArray(bslstl::StringRef name, bool formatAsEmptyArray);
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

        {   L_,    -1,     -1,   -1,  "",              ""                    },
        {   L_,     0,     -1,   -1,  "",              ""                    },
        {   L_,     1,      1,    2,  "",              ""                    },

        {   L_,    -1,     -1,   -1,  "{}",            "{}"                  },
        {   L_,     0,     -1,   -1,  "{}",            "{}"                  },
        {   L_,     1,      1,    2,  "{}",            "  {"             NL
                                                       "  }"                 },

        {   L_,    -1,     -1,   -1,  "[]",            "[]"                  },
        {   L_,     0,     -1,   -1,  "[]",            "[]"                  },
        {   L_,     1,      1,    2,  "[]",            "  ["             NL
                                                       "  ]"                 },

        {   L_,    -1,     -1,   -1,  "{a]}",          "{\"a\":[]}"          },
        {   L_,     0,     -1,   -1,  "{a]}",          "{\"a\":[]}"          },
        {   L_,     1,      1,    2,  "{a]}",          "  {"             NL
                                                       "    \"a\" : ["   NL
                                                       "    ]"           NL
                                                       "  }"                 },

        {   L_,    -1,     -1,   -1,  "[{}]",          "[{}]"                },
        {   L_,     0,     -1,   -1,  "[{}]",          "[{}]"                },
        {   L_,     1,      1,    2,  "[{}]",          "  ["             NL
                                                       "    {"           NL
                                                       "    }"           NL
                                                       "  ]"                 },

        {   L_,    -1,     -1,   -1,  "{a{}]}",        "{\"a\":[{}]}"        },
        {   L_,     0,     -1,   -1,  "{a{}]}",        "{\"a\":[{}]}"        },
        {   L_,     1,      1,    2,  "{a{}]}",        "  {"             NL
                                                       "    \"a\" : ["   NL
                                                       "      {"         NL
                                                       "      }"         NL
                                                       "    ]"           NL
                                                       "  }"                 },

        {   L_,    -1,     -1,   -1,  "{a{a]}]}",      "{\"a\":[{\"a\":[]}]}"},
        {   L_,     0,     -1,   -1,  "{a{a]}]}",      "{\"a\":[{\"a\":[]}]}"},
        {   L_,     1,      1,    2,  "{a{a]}]}",      "  {"             NL
                                                       "    \"a\" : ["   NL
                                                       "      {"         NL
                                                       "        \"a\" : ["
                                                                         NL
                                                       "        ]"       NL
                                                       "      }"         NL
                                                       "    ]"           NL
                                                       "  }"                 },

        {   L_,    -1,     -1,   -1,  "{a{a{}]}]}",    "{\"a\":[{\"a\":[{}]}]}"
                                                                             },
        {   L_,     0,     -1,   -1,  "{a{a{}]}]}",    "{\"a\":[{\"a\":[{}]}]}"
                                                                             },
        {   L_,     1,      1,    2,  "{a{a{}]}]}",    "  {"             NL
                                                       "    \"a\" : ["   NL
                                                       "      {"         NL
                                                       "        \"a\" : ["
                                                                         NL
                                                       "          {"     NL
                                                       "          }"     NL
                                                       "        ]"       NL
                                                       "      }"         NL
                                                       "    ]"           NL
                                                       "  }"                 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bsl::string CALLS  = DATA[i].d_methodCalls;
            const bsl::string EXP    = DATA[i].d_expected;

            if (veryVerbose)
                T_ P_(LINE) P_(INDENT) P_(SPL) P_(CALLS) P(EXP);

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            ASSERT(!mX.isCompleteJSON());

            // BDE_VERIFY pragma: -TP21     // No point in a 'veryVerbose'
                                            // print for this loop.
            for (size_t j = 0; j < CALLS.size(); ++j) {
                switch (CALLS[j]) {
                  case 'a': mX.openArray(bslstl::StringRef("a"));
                  break;
                  case 'o': mX.openObject("a");
                  break;
                  case '{': mX.openObject();
                  break;
                  case '[': mX.openArray();
                  break;
                  case ']': mX.closeArray();
                  break;
                  case '}': mX.closeObject();
                  break;
                  default: ASSERT(0);
                }
            }

            if (0 < CALLS.size()) {
                ASSERT( mX.isCompleteJSON());
            }

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'addValue' METHOD
        //
        // Concerns:
        //: 1 The 'addValue' method outputs the value of the element being
        //:   encoded irrespective of the type of 'value'.
        //:
        //: 2 Erroneous values of 'value' cause 'addValue' to return an error.
        //:
        //: 3 The 'addNullValue' outputs a null value.
        //:
        //: 4 The 'addValue' and 'addNullValue' methods indent before
        //:   outputting their value only if the value is part of an array.
        //:   Otherwise no indentation is done.
        //:
        // Plan:
        //: 1 For all the possible data types create at least one valid value
        //:   and an invalid value (if an invalid value exists) and invoke
        //:   'putValue' on them.
        //:
        //: 2 Confirm that 'addValue' returns 0 and correctly encodes the valid
        //:   values and returns a non-zero values for invalid values.
        //:
        //: 3 Confirm that 'addValue' indents only for values in an array.
        //:
        //: 4 Repeat steps 1-3 for 'addNullValue'.
        //
        // Testing:
        //   void addMemberName(const bsl::string_view& name);
        //   int addValue(const TYPE& value);
        //   int addValue(const bsl::string_view&, const TYPE&);
        //   void addNullValue();
        //   void addNullValue(const bsl::string_view& name);
        //   bool isNameNeeded() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addValue' METHOD" << endl
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

            if (veryVerbose) T_ P_(LINE) P_(ES) P_(IIL) P(SPL);

            testPutValue(LINE, L_, ES, IIL, SPL, A,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, B,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, C,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, D,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, E,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, F,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, G,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, H,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, I,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, J,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, K,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, L,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, M,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, N,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, O,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, PA,   DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, QA,   DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, R,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, S,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, T,    DP,   true);
            testPutValue(LINE, L_, ES, IIL, SPL, U,    DP,   true);

            testPutValue(LINE, L_, ES, IIL, SPL, INV1, DP,   false);
            testPutValue(LINE, L_, ES, IIL, SPL, INV2, DP,   false);
            testPutValue(LINE, L_, ES, IIL, SPL, INV3, DP,   false);
            testPutValue(LINE, L_, ES, IIL, SPL, INV4, DP,   false);

            Options opts;  const Options *OPTS = &opts;
            opts.setEncodeInfAndNaNAsStrings(true);

            testPutValue(LINE, L_, ES, IIL, SPL, INV1, OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, INV2, OPTS, true);

            opts.setDatetimeFractionalSecondPrecision(6);
            testPutValue(LINE, L_, ES, IIL, SPL, PA,   OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, QA,   OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, R,    OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, S,    OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, T,    OPTS, true);
            testPutValue(LINE, L_, ES, IIL, SPL, U,    OPTS, true);

            // testPutNullValue
            {
                for (int j = 0; j < 2; ++j) {
                    // j == 0, output as value of element, i.e. w/o indentation
                    // j == 1, output as array element, i.e. with indentation

                    if (veryVeryVerbose) T_ T_ P(j);

                    bsl::ostringstream os;
                    bsl::ostringstream exp;

                    Obj mX = g(os, ES, IIL, SPL);

                    if (1 == ES) {
                        bdlb::Print::indent(exp, IIL, SPL);
                    }

                    if (0 == j) {
                        mX.openObject();
                        exp << '{';

                        bsl::string spaceOrNot;

                        if (1 == ES) {
                            spaceOrNot = " ";
                            exp << '\n';
                            bdlb::Print::indent(exp, IIL + 1, SPL);
                        }

                        mX.addMemberName("name");
                        exp << "\"name\"" << spaceOrNot << ":" << spaceOrNot;
                    }
                    else {
                        mX.openArray();
                        exp << '[';
                        if (1 == ES) {
                            exp << '\n';
                            bdlb::Print::indent(exp, IIL + 1, SPL);
                        }
                    }

                    mX.addNullValue();

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

        if (verbose) cout << "\n\tNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bsl::ostringstream os;

            if (veryVerbose) cout << "\t\t'addMemberName()'"
                                  << endl;
            {
                Obj mX(os);
                ASSERT(!mX.isNameNeeded());
                ASSERT_FAIL(mX.addMemberName("name"));
                mX.openObject();
                ASSERT(mX.isNameNeeded());
                ASSERT_PASS(mX.addMemberName("name"));
                ASSERT(!mX.isNameNeeded());
                // Following an 'addMemberName', 'addMemberName' and the named
                // 'open*' and 'add*' methods should fail.
                ASSERT_FAIL(mX.addMemberName("name"));
                ASSERT_FAIL(mX.openArray("name"));
                ASSERT_FAIL(mX.openObject("name"));
                ASSERT_FAIL(mX.addNullValue("name"));
                ASSERT_FAIL(mX.addValue("name", 1));
                // Following an 'addMemberName', the un-named 'open*' and
                // 'add*' methods should pass.
                ASSERT(!mX.isNameNeeded());
                ASSERT_PASS(mX.openObject());
                ASSERT(mX.isNameNeeded());
                ASSERT_PASS(mX.addMemberName("name3"));
                ASSERT(!mX.isNameNeeded());
                ASSERT_PASS(mX.addNullValue());
                ASSERT(mX.isNameNeeded());
                ASSERT_PASS(mX.addMemberName("name4"));
                ASSERT_PASS(mX.addValue(1));
                ASSERT_PASS(mX.addMemberName("name5"));
                ASSERT_PASS(mX.openArray());
            }
        }
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
        //: 4 Matching 'openArray'/'closeArray' and 'openObject'/'closeObject'
        //:   calls results in 'isCompleteJSON()' returning 'true', and it
        //:   returning 'false' otherwise.
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
        //   bool isCompleteJSON() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
                          << "TESTING 'closeArray' METHOD" << "\n"
                          << "===========================" << endl;
#define NL "\n"
#define EMPTY Obj::e_EMPTY_ARRAY_FORMAT
#define NORMAL Obj::e_REGULAR_ARRAY_FORMAT

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int                       d_line;  // source line number
            int                       d_encodingStyle;
            int                       d_initialIndentLevel;
            int                       d_spacesPerLevel;
            Obj::ArrayFormattingStyle d_formatAsEmptyArray;
            int                       d_numTimesCloseArrayCalled;
            bsl::string               d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    FAEA    NT   EXPECTED
        // ----  -----  ------   ---    ----    ---  -------

        {   L_,    -1,     -1,   -1,  NORMAL,   0,       "[]"            },
        {   L_,    -1,     -1,   -1,  EMPTY,    0,       "[]"            },

        {   L_,     0,     -1,   -1,  NORMAL,   0,       "[]"            },
        {   L_,     0,     -1,   -1,  EMPTY,    0,       "[]"            },

        {   L_,     1,      2,    2,  NORMAL,   0,       "    ["
                                                    NL   "    ]"         },
        {   L_,     1,      2,    2,  EMPTY,    0,       "    []"        },

        {   L_,    -1,     -1,   -1,  NORMAL,   3,       "[[[[]]]]"      },
        {   L_,    -1,     -1,   -1,  EMPTY,    3,       "[[[[]]]]"      },

        {   L_,     0,     -1,   -1,  NORMAL,   3,       "[[[[]]]]"      },
        {   L_,     0,     -1,   -1,  EMPTY,    3,       "[[[[]]]]"      },

        {   L_,     1,      1,    2,  NORMAL,   3,       "  ["
                                                    NL   "    ["
                                                    NL   "      ["
                                                    NL   "        ["
                                                    NL   "        ]"
                                                    NL   "      ]"
                                                    NL   "    ]"
                                                    NL   "  ]"
                                                                        },
        {   L_,     1,      1,    2,  EMPTY,   3,        "  [  [  [  []]]]"
                                                                        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                       LINE   = DATA[i].d_line;
            const int                       STYLE  = DATA[i].d_encodingStyle;
            const int                       INDENT =
                                        DATA[i].d_initialIndentLevel;
            const int                       SPL    = DATA[i].d_spacesPerLevel;
            const Obj::ArrayFormattingStyle FAEA =
                                        DATA[i].d_formatAsEmptyArray;
            const int                       NT     =
                                        DATA[i].d_numTimesCloseArrayCalled;
            const bsl::string               EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            ASSERT(!mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());

            for (int k = 0; k < NT; ++k) {
                ASSERT(!mX.isCompleteJSON());
                mX.openArray(FAEA);
            }

            ASSERT(!mX.isFormattingObject());
            ASSERT(0 == NT || mX.isFormattingArray());
            ASSERT(!mX.isCompleteJSON());

            mX.openArray(FAEA);

            ASSERT(!mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT( mX.isFormattingArray());

            mX.closeArray(FAEA);

            ASSERT(0 == NT || mX.isFormattingArray());
            ASSERT(!mX.isFormattingObject());

            for (int k = 0; k < NT; ++k) {
                ASSERT(!mX.isCompleteJSON());
                mX.closeArray(FAEA);
            }

            ASSERT( mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
#undef EMPTY
#undef NORMAL

        if (verbose) cout << "\n\tNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bsl::ostringstream os;

            if (veryVerbose) cout << "\t\t'closeArray()'"
                                  << endl;
            {
                Obj mX(os);
                ASSERT_FAIL(mX.closeArray());
                mX.openObject();
                ASSERT_FAIL(mX.closeArray());
                mX.openArray("name");
                ASSERT_PASS(mX.closeArray());
            }
        }
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
        //   void openArray(bool formatAsEmptyArray);
        //   bool isFormattingArray() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
                          << "TESTING 'openArray' METHOD" << "\n"
                          << "==========================" << endl;
#define NL "\n"
#define EMPTY Obj::e_EMPTY_ARRAY_FORMAT
#define NORMAL Obj::e_REGULAR_ARRAY_FORMAT

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int                       d_line;  // source line number
            int                       d_encodingStyle;
            int                       d_initialIndentLevel;
            int                       d_spacesPerLevel;
            Obj::ArrayFormattingStyle d_formatAsEmptyArray;
            int                       d_numTimesOpenArrayCalled;
            bsl::string               d_expected;
        } DATA[] = {
        // LINE  STYLE  INDENT   SPL    FAEA    NT  EXPECTED
        // ----  -----  ------   ---    ----   ---  --------

        {   L_,    -1,     -1,   -1,  NORMAL,   0,  "["                 },
        {   L_,    -1,     -1,   -1,  EMPTY,    0,  "["                 },

        {   L_,     0,     -1,   -1,  NORMAL,   0,  "["                 },
        {   L_,     0,     -1,   -1,  EMPTY,    0,  "["                 },

        {   L_,     1,      1,    2,  NORMAL,   0,  "  ["           NL  },
        {   L_,     1,      1,    2,  EMPTY,    0,  "  ["               },

        {   L_,    -1,     -1,   -1,  NORMAL,   3,  "[[[["              },
        {   L_,    -1,     -1,   -1,  EMPTY,    3,  "[[[["              },

        {   L_,     0,     -1,   -1,  NORMAL,   3,  "[[[["              },
        {   L_,     0,     -1,   -1,  EMPTY,    3,  "[[[["              },

        {   L_,     1,      1,    2,  NORMAL,   3,  "  ["             NL
                                                    "    ["           NL
                                                    "      ["         NL
                                                    "        ["       NL
                                                                        },
        {   L_,     1,      1,    2,  EMPTY,    3,  "  [  [  [  ["      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                       LINE   = DATA[i].d_line;
            const int                       STYLE  = DATA[i].d_encodingStyle;
            const int                       INDENT =
                                        DATA[i].d_initialIndentLevel;
            const int                       SPL    = DATA[i].d_spacesPerLevel;
            const Obj::ArrayFormattingStyle FAEA =
                                        DATA[i].d_formatAsEmptyArray;
            const int                       NT     =
                                        DATA[i].d_numTimesOpenArrayCalled;
            const bsl::string               EXP    = DATA[i].d_expected;

            bsl::ostringstream os;

            Obj mX = g(os, STYLE, INDENT, SPL);

            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());
            ASSERT(!mX.isCompleteJSON());
            for (int k = 0; k < NT; ++k) mX.openArray(FAEA);

            ASSERT(!mX.isFormattingObject());
            ASSERT(0 == NT || mX.isFormattingArray());
            ASSERT(!mX.isCompleteJSON());
            mX.openArray(FAEA);

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());

            // Restore object to a valid state

            ASSERT(!mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(0 == NT || mX.isFormattingArray());
            for (int k = 0; k <= NT; ++k) mX.closeArray(FAEA);
            ASSERT( mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());
        }
#undef NL
#undef EMPTY
#undef NORMAL
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

        {   L_,     1,      2,    2,  0,       "    {"
                                          NL   "    }"          },

        {   L_,    -1,     -1,   -1,  3,       "{\"a\":{\"a\":{\"b\":{}}}}"
                                                                },

        {   L_,     0,     -1,   -1,  3,       "{\"a\":{\"a\":{\"b\":{}}}}"
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

            ASSERT(!mX.isCompleteJSON());
            for (int k = 0; k < NT; ++k) {
                if (0 == k) {
                    mX.openObject();
                }
                else {
                    mX.openObject("a");
                }
            }

            ASSERT(!mX.isCompleteJSON());
            if (0 == NT) {
                mX.openObject();
            }
            else {
                mX.openObject("b");
            }

            mX.closeObject();

            for (int k = 0; k < NT; ++k) {
                ASSERT(!mX.isCompleteJSON());
                mX.closeObject();
            }
            ASSERT( mX.isCompleteJSON());

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }

        if (verbose) cout << "\n\tNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bsl::ostringstream os;

            if (veryVerbose) cout << "\t\t'closeObject()'"
                                  << endl;
            {
                Obj mX(os);
                ASSERT_FAIL(mX.closeObject());
                mX.openObject();
                ASSERT_PASS(mX.closeObject());
                mX.openArray();
                ASSERT_FAIL(mX.closeObject());
            }
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
        //   void openObject(const bslstl::StringRef& name);
        //   bool isFormattingObject() const;
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

            {   L_,    -1,     -1,   -1,  0,  "{"                            },

            {   L_,     0,     -1,   -1,  0,  "{"                            },

            {   L_,     1,      1,    2,  0,  "  {"           NL             },

            {   L_,    -1,     -1,   -1,  3,  "{\"a\":{\"a\":{\"a\":{"       },

            {   L_,     0,     -1,   -1,  3,  "{\"a\":{\"a\":{\"a\":{"       },

            {   L_,     1,      1,    2,  3,  "  {"                   NL
                                              "    \"a\" : {"         NL
                                              "      \"a\" : {"       NL
                                              "        \"a\" : {"     NL     },
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

            ASSERT(!mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());
            mX.openObject();

            ASSERT( mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());

            for (int k = 0; k < NT; ++k) {
               mX.openObject("a");
               ASSERT(!mX.isCompleteJSON());

                ASSERT( mX.isFormattingObject());
                ASSERT(!mX.isFormattingArray());
            }

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());

            // Restore object to a valid state

            for (int k = 0; k <= NT; ++k) mX.closeObject();
            ASSERT( mX.isCompleteJSON());
            ASSERT(!mX.isFormattingObject());
            ASSERT(!mX.isFormattingArray());
        }

        if (verbose) cout << "\n\tNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bsl::ostringstream os;

            if (veryVerbose) cout << "\t\t'openArray()' and 'openArray(name)'"
                                  << endl;
            {
                Obj mX(os);
                ASSERT_PASS(mX.openArray());
                ASSERT_FAIL(mX.openArray("name"));
                mX.openObject();
                ASSERT_FAIL(mX.openArray());
                ASSERT_PASS(mX.openArray("name"));
            }

            if (veryVerbose) cout << "\t\t'openObject()' and "
                                  << "'openObject(name)'"
                                  << endl;
            {
                Obj mX(os);
                ASSERT_FAIL(mX.openObject("name"));
                ASSERT_PASS(mX.openObject());
                ASSERT_FAIL(mX.openObject());
                ASSERT_PASS(mX.openObject("name"));
            }
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
        //   SimpleFormatter(ostream& stream, Allocator *);
        //   SimpleFormatter(ostream&, const EncoderOptions&, Allocator *);
        //   SimpleFormatter(const SimpleFormatter &, bslma::Allocator *);
        //   ~SimpleFormatter();
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
        {   L_,     1,      1,    2,   "  {"                             NL
                                       "    \"A\" : 1,"                  NL
                                       "    \"B\" : ["                   NL
                                       "      2"                         NL
                                       "    ]"                           NL
                                       "  }"                                 },
        {   L_,     1,      3,    5,   "               {"                NL
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

            ASSERT(!mX.isCompleteJSON());
            mX.openObject();
            ASSERT(!mX.isCompleteJSON());
            mX.addValue("A", 1);
            ASSERT(!mX.isCompleteJSON());
            mX.openArray("B");
            ASSERT(!mX.isCompleteJSON());
            mX.addValue(2);
            ASSERT(!mX.isCompleteJSON());
            mX.closeArray();
            ASSERT(!mX.isCompleteJSON());
            mX.closeObject();
            ASSERT( mX.isCompleteJSON());

            ASSERT(!mY.isCompleteJSON());
            mY.openObject();
            ASSERT(!mY.isCompleteJSON());
            mY.addValue("A", 1);
            ASSERT(!mY.isCompleteJSON());
            mY.openArray(bslstl::StringRef("B"));
            ASSERT(!mY.isCompleteJSON());
            mY.addValue(2);
            ASSERT(!mY.isCompleteJSON());
            mY.closeArray();
            ASSERT(!mY.isCompleteJSON());
            mY.closeObject();
            ASSERT( mY.isCompleteJSON());

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
        //: 1 N/A
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

        bsl::string name = "name";

        mX.addNullValue(name);

        exp += "\"" + name + '"' + ':' + "null";

        mX.closeObject();

        exp += '}';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.openArray();

        // 'openArray' auto-inserts comma when it follows a 'close*' or
        // 'put*Value'.
        exp += ",[";
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeArray();

        exp += ']';
        ASSERTV(exp, os.str(), exp == os.str());

        ASSERTV(exp, os.str(), exp == os.str());
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // SimpleFormatter<->Formatter comparison
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A
        //
        // Testing:
        //   SimpleFormatter<->Formatter comparison - SimpleFormatter version
        // --------------------------------------------------------------------

        // BDE_VERIFY pragma: -TP18
        // BDE_VERIFY pragma: -TP22

        if (verbose) cout << endl
                          << "SimpleFormatter<->Formatter comparison" << endl
                          << "======================================" << endl;

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

            baljsn::SimpleFormatter sf = g(os, PRETTY, 0, SPACES);

            sf.openObject();

             sf.openObject("Object");
              sf.addValue("Field 1", 1);
              sf.addNullValue("Field 2");
             sf.closeObject();

             sf.openArray("Array");
              sf.addValue(1);        // No name
              sf.addValue("string"); // No name
              sf.openArray(baljsn::SimpleFormatter::e_EMPTY_ARRAY_FORMAT);
              sf.closeArray(baljsn::SimpleFormatter::e_EMPTY_ARRAY_FORMAT);
              sf.openArray();
               sf.openArray();
                sf.openObject();
                sf.closeObject();
               sf.closeArray();
              sf.closeArray();
             sf.closeArray();

             sf.addValue("True", true);
            sf.closeObject();

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
// Copyright 2019 Bloomberg Finance L.P.
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
