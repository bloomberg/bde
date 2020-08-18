// baljsn_datumutil.t.cpp                                             -*-C++-*-
#include <baljsn_datumutil.h>

#include <baljsn_simpleformatter.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_ostream.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>            // to verify that we do not
#include <bslma_testallocatormonitor.h>     // allocate any memory

#include <bsls_alignedbuffer.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_types.h>

#include <bdld_datum.h>
#include <bdld_datumarraybuilder.h>
#include <bdld_datumerror.h>
#include <bdld_datummaker.h>
#include <bdld_datummapbuilder.h>
#include <bdld_manageddatum.h>

#include <bdldfp_decimal.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bdlsb_fixedmeminstreambuf.h>  // for testing only
#include <bdlsb_memoutstreambuf.h>      // for testing only

#include <bdlt_date.h>
#include <bdlt_time.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

using namespace BloombergLP;
using namespace bsl;

// Suppress some bde_verify warnings for test driver.
// BDE_VERIFY pragma: -IND01
// BDE_VERIFY pragma: -SP01


// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test implements a suite of functions for converting
// 'bdld::Datum' objects to JSON, and back.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 6] int encode(string *, const Datum&, const DEOptions&, Allocator *);
// [ 6] int encode(string *, const Datum&, const DEOptions*, Allocator *);
// [ 6] int encode(ostream&, const Datum&, const DEOptions&, Allocator *);
// [ 6] int encode(ostream&, const Datum&, const DEOptions*, Allocator *);
// [ 5] int decode(MgedDatum*, const StringRef&);
// [ 5] int decode(MgedDatum*, const StringRef&, const DDOptions&);
// [ 5] int decode(MgedDatum*, os*, const StrRef&);
// [ 5] int decode(MgedDatum*, os*, const StrRef&, const DDOptions&);
// [ 5] int decode(MgedDatum*, streamBuf*);
// [ 5] int decode(MgedDatum*, streamBuf*, const DDOptions&);
// [ 5] int decode(MgedDatum*, ostream*, streamBuf*);
// [ 5] int decode(MgedDatum*, ostream*, streamBuf*, const DDOptions&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BREATHING DECODE TEST
// [ 3] BREATHING ENCODE TEST
// [ 4] BREATHING ROUND-TRIP TEST
// [ 7] USAGE EXAMPLE

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
//                      NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                                USEFUL MACROS
// ----------------------------------------------------------------------------

// The following macros may be used to print an expression 'X' at different
// levels of verbosity.  Note that 'X' is not surrounded with parentheses so
// that expressions containing output stream operations can be supported.

#define PV(X)   if         (verbose) cout << endl << X << endl;
#define PVV(X)  if     (veryVerbose) cout << endl << X << endl;
#define PVVV(X) if (veryVeryVerbose) cout << endl << X << endl;

// ============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::DatumUtil Util;

typedef bdld::ManagedDatum       MD;
typedef bdld::Datum              D;
typedef bdld::DatumArrayBuilder  DAB;
typedef bdld::DatumArrayRef DAR;
typedef bdld::DatumMapBuilder    DMB;
typedef bdld::DatumMapEntry      DME;
typedef bdld::DatumMapRef   DMR;

#define STR1   "xxxxxxxxxxxxxxxx"
#define STR2   STR1   STR1
#define STR4   STR2   STR2
#define STR8   STR4   STR4
#define STR16  STR8   STR8
#define STR64  STR16  STR16  STR16  STR16
#define STR256 STR64  STR64  STR64  STR64

const char *LONG_JSON_ARRAY = "["
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\","
                                  "\"" STR256 "\""
                              "]";

const char *LONG_JSON_OBJECT = "{"
                                  "\"" STR1   "\" : " "\"" STR256 "\","
                                  "\"" STR2   "\" : " "\"" STR256 "\","
                                  "\"" STR4   "\" : " "\"" STR256 "\","
                                  "\"" STR8   "\" : " "\"" STR256 "\","
                                  "\"" STR16  "\" : " "\"" STR256 "\","
                                  "\"" STR64  "\" : " "\"" STR256 "\","
                                  "\"" STR256 "\" : " "\"" STR256 "\""
                              "}";

                              // 96 nested arrays
const char *DEEP_JSON_ARRAY = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";

                              // 1 outer array, 71 nested objects inside
const char *DEEP_JSON_OBJECT = "["
                  "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                   "}}}}}}}}"
                   "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}"
                   "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}"
                               "]";


                              // 1 outer array, 1 nested object, 93 nested
                              // arrays inside
const char *DEEP_JSON_AOA = "["
                               "{\"a\":"
                                   "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                                   "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                                   "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                                   "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                                   "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                                   "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                               "}"
                            "]";

// ============================================================================
//                                TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: DOES NOT ALLOCATE MEMORY FROM GLOBAL ALLOCATOR

    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor gam(&ga);

    switch (test) { case 0:
      case 7: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
///Usage
///-----
// This section illustrates intended use of this component.
//
//
///Example 1: Encode (and decode) 'Datum' to (and from) a JSON string.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates encoding a 'Datum' as a JSON string and
// then decoding that JSON string back into a 'Datum' object.
//
// First, we create our 'Datum' object, using the 'bdld::DatumMaker' utility:
//..
    bsls::AlignedBuffer<8 * 1024>      buffer;
    bdlma::BufferedSequentialAllocator bsa(buffer.buffer(), sizeof(buffer));
    bdld::DatumMaker                   m(&bsa);

    bdld::Datum books = m.a(m.m("Author", "Ann Leckie",
                                "Title", "Ancilliary Justice"),
                            m.m("Author", "John Scalzi",
                                "Title", "Redshirts"));
//..
// Then, we convert the 'books' 'Datum' to formatted JSON:
//..
    baljsn::DatumEncoderOptions bookOptions;
    bookOptions.setEncodingStyle(baljsn::EncodingStyle::e_PRETTY);
    bookOptions.setSpacesPerLevel(4);
    bsl::string booksJSON(&bsa);

    int rc = baljsn::DatumUtil::encode(&booksJSON, books, bookOptions);
    if (0 != rc) {
        // handle error
    }
//..
// Next, we compare the result to the JSON we expect:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
    const bsl::string EXPECTED_BOOKS_JSON = R"JSON([
    {
        "Author" : "Ann Leckie",
        "Title" : "Ancilliary Justice"
    },
    {
        "Author" : "John Scalzi",
        "Title" : "Redshirts"
    }
])JSON";
#else
    const bsl::string EXPECTED_BOOKS_JSON = "[\n"
        "    {\n"
        "        \"Author\" : \"Ann Leckie\",\n"
        "        \"Title\" : \"Ancilliary Justice\"\n"
        "    },\n"
        "    {\n"
        "        \"Author\" : \"John Scalzi\",\n"
        "        \"Title\" : \"Redshirts\"\n"
        "    }\n"
        "]";
#endif

    ASSERTV(EXPECTED_BOOKS_JSON, booksJSON, EXPECTED_BOOKS_JSON == booksJSON);
//..
// Finally, we can decode the 'booksJSON' and make sure we got the same value
// back:
//..
    bdld::ManagedDatum decodedBooks;
    rc = baljsn::DatumUtil::decode(&decodedBooks, booksJSON);
    if (0 != rc) {
        // handle error
    }
    ASSERT(*decodedBooks == books);
//..
///Example 2: Converting JSON to 'Datum'
///- - - - - - - - - - - - - - - - - - -
// The following example illustrates the construction of a 'Datum' object from
// a JSON input.
//
// First, we create the JSON source, in both plain and formatted forms:
//..
    const bsl::string plainFamilyJSON = "["
                                 "{\"firstName\":\"Homer\","
                                 "\"age\":34}"
                                 ",{\"firstName\":\"Marge\","
                                 "\"age\":34}"
                                 ",{\"firstName\":\"Bart\","
                                 "\"age\":10}"
                                 ",{\"firstName\":\"Lisa\","
                                 "\"age\":8}"
                                 ",{\"firstName\":\"Maggie\","
                                 "\"age\":1}"
                                 "]";

    // Note that whitespace formatting is unimportant as long as the result is
    // legal JSON.  This will generate the same 'Datum' as the single-line form
    // above.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
    const bsl::string formattedFamilyJSON = R"JSON([
    {
        "firstName" : "Homer",
        "age" : 34
    },
    {
        "firstName" : "Marge",
        "age" : 34
    },
    {
        "firstName" : "Bart",
        "age" : 10
    },
    {
        "firstName" : "Lisa",
        "age" : 8
    },
    {
        "firstName" : "Maggie",
        "age" : 1
    }
])JSON";
#else
    const bsl::string formattedFamilyJSON =
                                        "[\n"
                                        "    {\n"
                                        "        \"firstName\" : \"Homer\",\n"
                                        "        \"age\" : 34\n"
                                        "    },\n"
                                        "    {\n"
                                        "        \"firstName\" : \"Marge\",\n"
                                        "        \"age\" : 34\n"
                                        "    },\n"
                                        "    {\n"
                                        "        \"firstName\" : \"Bart\",\n"
                                        "        \"age\" : 10\n"
                                        "    },\n"
                                        "    {\n"
                                        "        \"firstName\" : \"Lisa\",\n"
                                        "        \"age\" : 8\n"
                                        "    },\n"
                                        "    {\n"
                                        "        \"firstName\" : \"Maggie\",\n"
                                        "        \"age\" : 1\n"
                                        "    }\n"
                                        "]";
#endif // def BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
//..
// Then, we convert the single-line 'string' to a 'Datum':
//..
    bdld::ManagedDatum family;
    rc = baljsn::DatumUtil::decode(&family, plainFamilyJSON);
    if (0 != rc) {
        // handle error
    }
//..
// Next, we convert the formatted 'string' to another 'Datum' and make sure
// that the results match:
//..
    bdld::ManagedDatum family2;
    rc = baljsn::DatumUtil::decode(&family2, formattedFamilyJSON);
    if (0 != rc) {
        // handle error
    }
    ASSERT(family == family2);
//..
// Finally, we make sure that the structure of the resulting datum is as we
// expect.
//..
    ASSERT(family->isArray());
    ASSERT(5 == family->theArray().length());

    const bdld::Datum &lisa = family->theArray()[3];

    ASSERT(lisa.isMap());
    ASSERT(2         == lisa.theMap().size());
    ASSERT("Lisa"    == lisa.theMap().find("firstName")->theString());
    ASSERT(8         == lisa.theMap().find("age")->theDouble());
//..
// Notice that the 'type' of "age" is 'double', since "age" was encoded as a
// number, and 'double' is the supported representation of a JSON number (see
// {'Supported Types'}).
      } break;
      case 6: {
        //---------------------------------------------------------------------
        // ENCODE AND PRINT TEST
        //   This case tests the 'encode' and 'print' methods.
        //
        // Concerns:
        //: 1 The 'encode' overloads and 'print' method can all encode valid
        //:   Datum's of all JSON-able types, and return an error when
        //:   presented with invalid Datum types.
        //:
        //: 2 The NULL Datum is encoded correctly.
        //:
        //: 3 The formatting options are propagated and handled correctly.
        //:
        //: 4 The 'strictTypes()' option is handled correctly.
        //:
        //: 5 Datum maps with duplicate keys are propagated and handled
        //:   correctly.
        //:
        //: 6 All allocations are done via the passed-in allocator.
        //
        // Plan:
        //: 1 'encode' 'Datum's, and compare them with the expected return
        //:   codes and strings, using 'TestAllocator's to ensure allocations
        //:   are handled properly.
        //
        // Testing:
        //   int encode(string *, const Datum&, const DEOptions&, Allocator *);
        //   int encode(string *, const Datum&, const DEOptions*, Allocator *);
        //   int encode(ostream&, const Datum&, const DEOptions&, Allocator *);
        //   int encode(ostream&, const Datum&, const DEOptions*, Allocator *);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "ENCODE AND PRINT TEST" << endl
                                  << "=====================" << endl;

        bsls::AlignedBuffer<8 * 1024>      buffer;
        bdlma::BufferedSequentialAllocator bsa(
            buffer.buffer(), sizeof(buffer));

        bdld::DatumMaker m(&bsa);

        typedef baljsn::EncodingStyle::Value TStyle;

#define PR   baljsn::EncodingStyle::e_PRETTY
#define CO   baljsn::EncodingStyle::e_COMPACT
#define L   L_

        struct {
            int          d_line;
            bdld::Datum  d_datum;
            TStyle       d_encodingStyle;
            int          d_initialIndentLevel;
            int          d_spacesPerLevel;
            bsl::string  d_json;
            int          d_rcNoStrictTypes;
            int          d_rcWithStrictTypes;
        } DATA[] = {
           // L  datum      ES  IIL SPL  d_json                    rcNS rcWS
           // -- -----      --  --- ---  ------                    ---- ----
           // e_NIL
            { L, m(),       CO,  0,  0,  "null",                     0,    0 },
            { L, m(),       PR,  0,  0,  "null",                     0,    0 },
            { L, m(),       PR,  1,  0,  "null",                     0,    0 },
            { L, m(),       PR,  0,  1,  "null",                     0,    0 },
            { L, m(),       PR,  1,  1,  " null",                    0,    0 },

           // e_INTEGER
            { L, m(1),      CO,  0,  0,  "1",                        0,    1 },
            { L, m(1),      PR,  0,  0,  "1",                        0,    1 },
            { L, m(1),      PR,  1,  0,  "1",                        0,    1 },
            { L, m(1),      PR,  0,  1,  "1",                        0,    1 },
            { L, m(1),      PR,  1,  1,  " 1",                       0,    1 },

           // e_DOUBLE
            { L, m(1.5),    CO,  0,  0,  "1.5",                      0,    0 },
            { L, m(1.5),    PR,  0,  0,  "1.5",                      0,    0 },
            { L, m(1.5),    PR,  1,  0,  "1.5",                      0,    0 },
            { L, m(1.5),    PR,  0,  1,  "1.5",                      0,    0 },
            { L, m(1.5),    PR,  1,  1,  " 1.5",                     0,    0 },

           // e_STRING
            { L, m("Hello"),
                            CO,  0,  0,  "\"Hello\"",                0,    0 },
            { L, m("Hello"),
                            PR,  0,  0,  "\"Hello\"",                0,    0 },
            { L, m("Hello"),
                            PR,  1,  0,  "\"Hello\"",                0,    0 },
            { L, m("Hello"),
                            PR,  0,  1,  "\"Hello\"",                0,    0 },
            { L, m("Hello"),
                            PR,  1,  1,  " \"Hello\"",               0,    0 },

            { L, m(STR256),
                            CO,  0,  0,  "\"" STR256 "\"",           0,    0 },
            { L, m(STR256),
                            PR,  0,  0,  "\"" STR256 "\"",           0,    0 },
            { L, m(STR256),
                            PR,  1,  0,  "\"" STR256 "\"",           0,    0 },
            { L, m(STR256),
                            PR,  0,  1,  "\"" STR256 "\"",           0,    0 },
            { L, m(STR256),
                            PR,  1,  1,  " \"" STR256 "\"",          0,    0 },

           // e_BOOLEAN
            { L, m(true),   CO,  0,  0,  "true",                     0,    0 },
            { L, m(true),   PR,  0,  0,  "true",                     0,    0 },
            { L, m(true),   PR,  1,  0,  "true",                     0,    0 },
            { L, m(true),   PR,  0,  1,  "true",                     0,    0 },
            { L, m(true),   PR,  1,  1,  " true",                    0,    0 },

            { L, m(false),  CO,  0,  0,  "false",                    0,    0 },
            { L, m(false),  PR,  0,  0,  "false",                    0,    0 },
            { L, m(false),  PR,  1,  0,  "false",                    0,    0 },
            { L, m(false),  PR,  0,  1,  "false",                    0,    0 },
            { L, m(false),  PR,  1,  1,  " false",                   0,    0 },

           // e_ERROR
            { L, m(bdld::DatumError(-1)),
                            CO,  0,  0,  "N/A",                     -1,   -1 },
            { L, m(bdld::DatumError(-1)),
                            PR,  0,  0,  "N/A",                     -1,   -1 },
            { L, m(bdld::DatumError(-1)),
                            PR,  1,  0,  "N/A",                     -1,   -1 },
            { L, m(bdld::DatumError(-1)),
                            PR,  0,  1,  "N/A",                     -1,   -1 },
            { L, m(bdld::DatumError(-1)),
                            PR,  1,  1,  "N/A",                     -1,   -1 },

           // e_DATE
            { L, m(bdlt::Date(2019, 8, 30)),
                            CO,  0,  0,  "\"2019-08-30\"",           0,    1 },
            { L, m(bdlt::Date(2019, 8, 30)),
                            PR,  0,  0,  "\"2019-08-30\"",           0,    1 },
            { L, m(bdlt::Date(2019, 8, 30)),
                            PR,  1,  0,  "\"2019-08-30\"",           0,    1 },
            { L, m(bdlt::Date(2019, 8, 30)),
                            PR,  0,  1,  "\"2019-08-30\"",           0,    1 },
            { L, m(bdlt::Date(2019, 8, 30)),
                            PR,  1,  1,  " \"2019-08-30\"",          0,    1 },

           // e_TIME
            { L, m(bdlt::Time(23, 59, 58, 765)),
                            CO,  0,  0,  "\"23:59:58.765\"",         0,    1 },
            { L, m(bdlt::Time(23, 59, 58, 765)),
                            PR,  0,  0,  "\"23:59:58.765\"",         0,    1 },
            { L, m(bdlt::Time(23, 59, 58, 765)),
                            PR,  1,  0,  "\"23:59:58.765\"",         0,    1 },
            { L, m(bdlt::Time(23, 59, 58, 765)),
                            PR,  0,  1,  "\"23:59:58.765\"",         0,    1 },
            { L, m(bdlt::Time(23, 59, 58, 765)),
                            PR,  1,  1,  " \"23:59:58.765\"",        0,    1 },

           // e_DATETIME
            { L, m(bdlt::Datetime(2019, 8, 30, 23, 59, 58, 765)),
                            CO,  0,  0,  "\"2019-08-30T23:59:58.765\"",
                                                                     0,    1 },
            { L, m(bdlt::Datetime(2019, 8, 30, 23, 59, 58, 765)),
                            PR,  0,  0,  "\"2019-08-30T23:59:58.765\"",
                                                                     0,    1 },
            { L, m(bdlt::Datetime(2019, 8, 30, 23, 59, 58, 765)),
                            PR,  1,  0,  "\"2019-08-30T23:59:58.765\"",
                                                                     0,    1 },
            { L, m(bdlt::Datetime(2019, 8, 30, 23, 59, 58, 765)),
                            PR,  0,  1,  "\"2019-08-30T23:59:58.765\"",
                                                                     0,    1 },
            { L, m(bdlt::Datetime(2019, 8, 30, 23, 59, 58, 765)),
                            PR,  1,  1,  " \"2019-08-30T23:59:58.765\"",
                                                                     0,    1 },

           // e_DATETIME_INTERVAL
            { L, m(bdlt::DatetimeInterval(234, 11, 23, 45, 678, 900)),
                            CO,  0,  0,  "\"+234_11:23:45.678900\"",
                                                                     0,    1 },
            { L, m(bdlt::DatetimeInterval(234, 11, 23, 45, 678, 900)),
                            PR,  0,  0,  "\"+234_11:23:45.678900\"",
                                                                     0,    1 },
            { L, m(bdlt::DatetimeInterval(234, 11, 23, 45, 678, 900)),
                            PR,  1,  0,  "\"+234_11:23:45.678900\"",
                                                                     0,    1 },
            { L, m(bdlt::DatetimeInterval(234, 11, 23, 45, 678, 900)),
                            PR,  0,  1,  "\"+234_11:23:45.678900\"",
                                                                     0,    1 },
            { L, m(bdlt::DatetimeInterval(234, 11, 23, 45, 678, 900)),
                            PR,  1,  1,  " \"+234_11:23:45.678900\"",
                                                                     0,    1 },
           // e_INTEGER64
            { L, m(bsls::Types::Int64(8000000000LL)),
                            CO,  0,  0,  "8000000000",               0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  0,  0,  "8000000000",               0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  1,  0,  "8000000000",               0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  0,  1,  "8000000000",               0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  1,  1,  " 8000000000",              0,    1 },

           // e_USERDEFINED
            { L, bdld::Datum::createUdt(0, 1),
                            CO,  0,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::createUdt(0, 1),
                            PR,  0,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::createUdt(0, 1),
                            PR,  1,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::createUdt(0, 1),
                            PR,  0,  1,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::createUdt(0, 1),
                            PR,  1,  1,  "N/A",                     -1,   -1 },

           // e_BINARY
            { L, bdld::Datum::copyBinary(0, 0, &bsa),
                            CO,  0,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::copyBinary(0, 0, &bsa),
                            PR,  0,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::copyBinary(0, 0, &bsa),
                            PR,  1,  0,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::copyBinary(0, 0, &bsa),
                            PR,  0,  1,  "N/A",                     -1,   -1 },
            { L, bdld::Datum::copyBinary(0, 0, &bsa),
                            PR,  1,  1,  "N/A",                     -1,   -1 },

           // e_DECIMAL64
            { L, bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), &bsa),
                            CO,  0,  0,  "\"0.0\"",                  0,    1 },
            { L, bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), &bsa),
                            PR,  0,  0,  "\"0.0\"",                  0,    1 },
            { L, bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), &bsa),
                            PR,  1,  0,  "\"0.0\"",                  0,    1 },
            { L, bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), &bsa),
                            PR,  0,  1,  "\"0.0\"",                  0,    1 },
            { L, bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), &bsa),
                            PR,  1,  1,  " \"0.0\"",                 0,    1 },

           // e_ARRAY (empty)
            { L, m.a(),     CO,  0,  0,  "[]",                       0,    0 },
            { L, m.a(),     PR,  0,  0,  "[]",                       0,    0 },
            { L, m.a(),     PR,  1,  0,  "[]",                       0,    0 },
            { L, m.a(),     PR,  0,  1,  "[]",                       0,    0 },
            { L, m.a(),     PR,  1,  1,  " []",                      0,    0 },

           // e_ARRAY of e_INTEGER
            { L, m.a(1, 1), CO,  0,  0,  "[1,1]",                    0,    1 },
            { L, m.a(1, 1), PR,  0,  0,  "[\n1,\n1\n]",              0,    1 },
            { L, m.a(1, 1), PR,  1,  0,  "[\n1,\n1\n]",              0,    1 },
            { L, m.a(1, 1), PR,  0,  1,  "[\n 1,\n 1\n]",            0,    1 },
            { L, m.a(1, 1), PR,  1,  1,  " [\n  1,\n  1\n ]",        0,    1 },

           // e_ARRAY of e_DOUBLE
            { L, m.a(1.0, 1.0),
                            CO,  0,  0,  "[1,1]",                    0,    0 },
            { L, m.a(1.0, 1.0),
                            PR,  0,  0,  "[\n1,\n1\n]",              0,    0 },
            { L, m.a(1.0, 1.0),
                            PR,  1,  0,  "[\n1,\n1\n]",              0,    0 },
            { L, m.a(1.0, 1.0),
                            PR,  0,  1,  "[\n 1,\n 1\n]",            0,    0 },
            { L, m.a(1.0, 1.0),
                            PR,  1,  1,  " [\n  1,\n  1\n ]",        0,    0 },

           // e_MAP (empty)
            { L, m.m(),
                            CO,  0,  0,  "{}",                       0,    0 },
            { L, m.m(),
                            PR,  0,  0,  "{\n}",                     0,    0 },
            { L, m.m(),
                            PR,  1,  0,  "{\n}",                     0,    0 },
            { L, m.m(),
                            PR,  0,  1,  "{\n}",                     0,    0 },
            { L, m.m(),
                            PR,  1,  1,  " {\n }",                   0,    0 },

           // e_ARRAY of e_MAP (empty)
            { L, m.a(m.m()),
                            CO,  0,  0,  "[{}]",                     0,    0 },
            { L, m.a(m.m()),
                            PR,  0,  0,  "[\n{\n}\n]",               0,    0 },
            { L, m.a(m.m()),
                            PR,  1,  0,  "[\n{\n}\n]",               0,    0 },
            { L, m.a(m.m()),
                            PR,  0,  1,  "[\n {\n }\n]",             0,    0 },
            { L, m.a(m.m()),
                            PR,  1,  1,  " [\n  {\n  }\n ]",         0,    0 },

           // e_MAP of e_ARRAY (empty)
            { L, m.m("a", m.a()),
                            CO,  0,  0,  "{\"a\":[]}",               0,    0 },
            { L, m.m("a", m.a()),
                            PR,  0,  0,  "{\n\"a\" : []\n}",         0,    0 },
            { L, m.m("a", m.a()),
                            PR,  1,  0,  "{\n\"a\" : []\n}",         0,    0 },
            { L, m.m("a", m.a()),
                            PR,  0,  1,  "{\n \"a\" : []\n}",        0,    0 },
            { L, m.m("a", m.a()),
                            PR,  1,  1,  " {\n  \"a\" : []\n }",     0,    0 },

           // e_MAP of e_INTEGER
            { L, m.m("a", 1, "b", 2),
                            CO,  0,  0,  "{\"a\":1,\"b\":2}",        0,    1 },
            { L, m.m("a", 1, "b", 2),
                            PR,  0,  0,  "{\n\"a\" : 1,\n\"b\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "b", 2),
                            PR,  1,  0,  "{\n\"a\" : 1,\n\"b\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "b", 2),
                            PR,  0,  1,  "{\n \"a\" : 1,\n \"b\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "b", 2),
                            PR,  1,  1,  " {\n  \"a\" : 1,\n  \"b\" : 2\n }",
                                                                     0,    1 },

           // e_MAP of e_DOUBLE
            { L, m.m("a", 1.0, "b", 2.0),
                            CO,  0,  0,  "{\"a\":1,\"b\":2}",        0,    0 },
            { L, m.m("a", 1.0, "b", 2.0),
                            PR,  0,  0,  "{\n\"a\" : 1,\n\"b\" : 2\n}",
                                                                     0,    0 },
            { L, m.m("a", 1.0, "b", 2.0),
                            PR,  1,  0,  "{\n\"a\" : 1,\n\"b\" : 2\n}",
                                                                     0,    0 },
            { L, m.m("a", 1.0, "b", 2.0),
                            PR,  0,  1,  "{\n \"a\" : 1,\n \"b\" : 2\n}",
                                                                     0,    0 },
            { L, m.m("a", 1.0, "b", 2.0),
                            PR,  1,  1,  " {\n  \"a\" : 1,\n  \"b\" : 2\n }",
                                                                     0,    0 },

           // e_MAP of e_ARRAY of e_DOUBLE
            { L, m.m("a", m.a(1.0), "b", m.a(2.0)),
                            CO,  0,  0,  "{\"a\":[1],\"b\":[2]}",    0,    0 },
            { L, m.m("a", m.a(1.0), "b", m.a(2.0)),
                            PR,  0,  0,
                                     "{\n\"a\" : [\n1\n],\n\"b\" : [\n2\n]\n}",
                                                                     0,    0 },
            { L, m.m("a", m.a(1.0), "b", m.a(2.0)),
                            PR,  1,  0,
                                     "{\n\"a\" : [\n1\n],\n\"b\" : [\n2\n]\n}",
                                                                     0,    0 },
            { L, m.m("a", m.a(1.0), "b", m.a(2.0)),
                            PR,  0,  1,
                             "{\n \"a\" : [\n  1\n ],\n \"b\" : [\n  2\n ]\n}",
                                                                     0,    0 },
            { L, m.m("a", m.a(1.0), "b", m.a(2.0)),
                            PR,  1,  1,
                   " {\n  \"a\" : [\n   1\n  ],\n  \"b\" : [\n   2\n  ]\n }",
                                                                     0,    0 },

           // e_MAP of e_MAP of e_DOUBLE
            { L, m.m("a", m.m("a", 1.0, "b", 2.0)),
                            CO,  0,  0,  "{\"a\":{\"a\":1,\"b\":2}}",
                                                                     0,    0 },
            { L, m.m("a",m.m("a", 1.0, "b", 2.0)),
                            PR,  0,  0,
                                   "{\n\"a\" : {\n\"a\" : 1,\n\"b\" : 2\n}\n}",
                                                                     0,    0 },
            { L, m.m("a",m.m("a", 1.0, "b", 2.0)),
                            PR,  1,  0,
                                   "{\n\"a\" : {\n\"a\" : 1,\n\"b\" : 2\n}\n}",
                                                                     0,    0 },
            { L, m.m("a",m.m("a", 1.0, "b", 2.0)),
                            PR,  0,  1,
                             "{\n \"a\" : {\n  \"a\" : 1,\n  \"b\" : 2\n }\n}",
                                                                     0,    0 },
            { L, m.m("a",m.m("a", 1.0, "b", 2.0)),
                            PR,  1,  1,
                       " {\n  \"a\" : {\n   \"a\" : 1,\n   \"b\" : 2\n  }\n }",
                                                                     0,    0 },
           // e_MAP of e_INTEGER with duplicate keys
            { L, m.m("a", 1, "a", 2),
                            CO,  0,  0,  "{\"a\":1,\"a\":2}",        0,    1 },
            { L, m.m("a", 1, "a", 2),
                            PR,  0,  0,  "{\n\"a\" : 1,\n\"a\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "a", 2),
                            PR,  1,  0,  "{\n\"a\" : 1,\n\"a\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "a", 2),
                            PR,  0,  1,  "{\n \"a\" : 1,\n \"a\" : 2\n}",
                                                                     0,    1 },
            { L, m.m("a", 1, "a", 2),
                            PR,  1,  1,  " {\n  \"a\" : 1,\n  \"a\" : 2\n }",
                                                                     0,    1 },

        };
#undef PR
#undef CO
#undef L

        if (verbose)
            cout << "\nTest encoding various datums."
                 << endl;

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE           = DATA[ti].d_line;
            const bdld::Datum&  DATUM          = DATA[ti].d_datum;
            const TStyle        ES             = DATA[ti].d_encodingStyle;
            const int           IIL            = DATA[ti].d_initialIndentLevel;
            const int           SPL            = DATA[ti].d_spacesPerLevel;
            const bsl::string&  JSON           = DATA[ti].d_json;
            const int           RC_BY_STRICT[] = {
                                                   DATA[ti].d_rcNoStrictTypes,
                                                   DATA[ti].d_rcWithStrictTypes
                                                 };

            if(veryVerbose) {
                T_ P_(LINE) P_(DATUM) P_(ES) P_(IIL) P_(SPL) P_(JSON)
                    P_(RC_BY_STRICT[0]) P(RC_BY_STRICT[1]);
            }

            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);

            {
                baljsn::DatumEncoderOptions opts;
                opts.setEncodingStyle(ES);
                opts.setInitialIndentLevel(IIL);
                opts.setSpacesPerLevel(SPL);

                for (int strictTypes = 0; strictTypes < 2; ++strictTypes) {
                    const int RC = RC_BY_STRICT[strictTypes];
                    opts.setStrictTypes(bool(RC));

                    if(veryVeryVerbose) {
                        T_ T_ P_(strictTypes) P(RC);
                    }

                    if (veryVeryVerbose)
                        cout
                            << "int encode(string*,const Datum&,Opts&);"
                            << endl;
                    {
                        bsl::string result(&ta);

                        int rc = Util::encode(&result, DATUM, opts);
                        ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                        if (0 <= rc) {
                            ASSERTV(LINE, JSON, result, JSON == result);
                        }
                    }

                    // Test that default Options are handled correctly.
                    if (baljsn::EncodingStyle::e_COMPACT == ES && 0 == IIL &&
                        0 == SPL && 0 == strictTypes) {
                        if (veryVeryVerbose)
                            cout << "int encode(string*,const Datum&);"
                                 << endl;
                        {
                            bsl::string result(&ta);

                            int rc = Util::encode(&result, DATUM);
                            ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                            if (0 <= rc) {
                                ASSERTV(LINE, JSON, result, JSON == result);
                            }
                        }
                    }

                    if (veryVeryVerbose)
                        cout << "int encode(ostream&, const Datum&,Opts&);"
                             << endl;
                    {
                        bdlsb::MemOutStreamBuf sb(&ta);
                        bsl::ostream           os(&sb);

                        int rc = Util::encode(os, DATUM, opts);
                        ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                        if (0 <= rc) {
                            const bslstl::StringRef result(
                                sb.data(), sb.length());

                            ASSERTV(LINE, JSON, result, JSON == result);
                        }
                    }

                    // Test that default Options are handled correctly.
                    if (baljsn::EncodingStyle::e_COMPACT == ES && 0 == IIL &&
                        0 == SPL && 0 == strictTypes) {
                        if (veryVeryVerbose)
                            cout << "int encode(ostream&,const Datum&);"
                                 << endl;
                        {
                            bdlsb::MemOutStreamBuf sb(&ta);
                            bsl::ostream           os(&sb);

                            int rc = Util::encode(os, DATUM);
                            ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                            if (0 <= rc) {
                                const bslstl::StringRef result(sb.data(),
                                                               sb.length());

                                ASSERTV(LINE, JSON, result, JSON == result);
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 5: {
        //---------------------------------------------------------------------
        // DECODE TEST
        //   This case tests the 'decode' methods.
        //
        // Concerns:
        //: 1 The 'decode' overloads can all decode valid strings of all JSON
        //:   types, and return an error when presented with invalid JSON.
        //:
        //: 2 The empty string is decoded correctly.
        //:
        //: 3 JSON objects with duplicate keys are decoded correctly,
        //:   preserving the first key/value pair for a given key.
        //:
        //: 4 The 'maxNestedDepth' option is handled correctly.
        //:
        //: 5 Whitespace is ignored in all legal locations.
        //:
        //: 6 All allocations are done via the passed-in allocator.
        //
        // Plan:
        //: 1 'decode' strings, and compare them with the expected return codes
        //:   and Datums, using 'TestAllocator's to ensure allocations are
        //:   handled properly.  Pass different 'maxNestedDepth' option values
        //:   to make sure errors occur if 'maxNestedDepth' is insuffient and
        //:   the string is otherwise valid.
        //
        // Testing:
        //  int decode(MgedDatum*, const StringRef&);
        //  int decode(MgedDatum*, const StringRef&, const DDOptions&);
        //  int decode(MgedDatum*, os*, const StrRef&);
        //  int decode(MgedDatum*, os*, const StrRef&, const DDOptions&);
        //  int decode(MgedDatum*, streamBuf*);
        //  int decode(MgedDatum*, streamBuf*, const DDOptions&);
        //  int decode(MgedDatum*, ostream*, streamBuf*);
        //  int decode(MgedDatum*, ostream*, streamBuf*, const DDOptions&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "DECODE TEST" << endl
                                  << "===========" << endl;

        bsls::AlignedBuffer<8 * 1024>      buffer;
        bdlma::BufferedSequentialAllocator bsa(
            buffer.buffer(), sizeof(buffer));

        bdld::DatumMaker m(&bsa);

        const bdld::Datum LONG_DATUM_ARRAY  = m.a(m(STR256),
                                                  m(STR256),
                                                  m(STR256),
                                                  m(STR256),
                                                  m(STR256),
                                                  m(STR256),
                                                  m(STR256),
                                                  m(STR256));
        const bdld::Datum LONG_DATUM_OBJECT = m.m(
                                                  STR1,   STR256,
                                                  STR2,   STR256,
                                                  STR4,   STR256,
                                                  STR8,   STR256,
                                                  STR16,  STR256,
                                                  STR64,  STR256,
                                                  STR256, STR256);
        const bdld::Datum DEEP_DATUM_ARRAY  = m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a(
                 ))))))))))))))))))))))))))))))))))))))))))))))))
                 ))))))))))))))))))))))))))))))))))))))))))))))));
        const bdld::Datum DEEP_DATUM_OBJECT = m.a(
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a", m.m("a",
              m.m(
                 ))))))))))))))))))))))))))))))))))))
                 ))))))))))))))))))))))))))))))))))));
        const bdld::Datum DEEP_DATUM_AOA    = m.a(
              m.m("a",
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a( m.a(
              m.a( m.a(
                 ))))))))))))))))))))))))))))))))
                 ))))))))))))))))))))))))))))))))
                 )))))))))))))))))))))))))))))));

#define WS "   \t       \n      \v       \f       \r       "

        struct {
            int         d_line;
            const char *d_json_p;
            int         d_rc;       // return code.  Only the sign is
                                    // significant, but we're testing exact
                                    // values
            int         d_depth;    // required maxNestedDepth for 'd_json_p'
                                    // to parse, 0 for N/A
            bdld::Datum d_datum;
        } DATA[] =
        {
         //   L  d_json_p                d_rc d_dpth d_datum
         //   -- --------                ---- ------ -------
            { L_, "",                     -1,     0, m()                     },
            { L_, "n",                    -2,     0, m()                     },
            { L_, "nu",                   -2,     0, m()                     },
            { L_, "nul",                  -2,     0, m()                     },
            { L_, "null",                  0,     0, m()                     },
            { L_, WS "null",               0,     0, m()                     },
            { L_, "null" WS,               0,     0, m()                     },
            { L_, "1",                     0,     0, m(1.0)                  },
            { L_, WS "1",                  0,     0, m(1.0)                  },
            { L_, "1" WS,                  0,     0, m(1.0)                  },
            { L_, "2.5",                   0,     0, m(2.5)                  },
            { L_, "\"hello\"",             0,     0, m("hello")              },
            { L_, WS "\"hello\"",          0,     0, m("hello")              },
            { L_, WS "\"hello\"" WS,       0,     0, m("hello")              },
            { L_, "\"hello",              -1,     0, m()                     },
            { L_, "t",                    -2,     0, m()                     },
            { L_, "tr",                   -2,     0, m()                     },
            { L_, "tru",                  -2,     0, m()                     },
            { L_, "treu",                 -2,     0, m()                     },
            { L_, "true",                  0,     0, m(true)                 },
            { L_, WS "true",               0,     0, m(true)                 },
            { L_, "true" WS,               0,     0, m(true)                 },
            { L_, "false",                 0,     0, m(false)                },
            { L_, "fals",                 -2,     0, m()                     },
            { L_, "[]",                    0,     0, m.a()                   },
            { L_, "[}",                   -2,     0, m.a()                   },
            { L_, "{]",                   -2,     0, m.a()                   },
            { L_, "{}}",                  -3,     0, m.a()                   },
            { L_, "[]}",                  -3,     0, m.a()                   },
            { L_, "{}]",                  -3,     0, m.a()                   },
            { L_, "[]]",                  -3,     0, m.a()                   },
            { L_, WS "[]",                 0,     0, m.a()                   },
            { L_, "[" WS "]",              0,     0, m.a()                   },
            { L_, "[]" WS,                 0,     0, m.a()                   },
            { L_, "[",                    -2,     0, m.a()                   },
            { L_, "]",                    -1,     0, m.a()                   },
            { L_, "[1.0]",                 0,     0, m.a(1.0)                },
            { L_, WS "[1.0]",              0,     0, m.a(1.0)                },
            { L_, "[" WS "1.0]",           0,     0, m.a(1.0)                },
            { L_, "[1.0" WS "]",           0,     0, m.a(1.0)                },
            { L_, "[1.0]" WS,              0,     0, m.a(1.0)                },
            { L_, "[[]]",                  0,     2, m.a(m.a())              },
            { L_, WS "[[]]",               0,     2, m.a(m.a())              },
            { L_, "[" WS "[]]",            0,     2, m.a(m.a())              },
            { L_, "[[" WS "]]",            0,     2, m.a(m.a())              },
            { L_, "[[]" WS "]",            0,     2, m.a(m.a())              },
            { L_, "[[]]" WS,               0,     2, m.a(m.a())              },
            { L_, "[[]",                  -2,     2, m()                     },
            { L_, "[[[]",                 -2,     3, m()                     },
            { L_, "[[[]]",                -2,     3, m()                     },
            { L_, "[]]",                  -3,     1, m()                     },
            { L_, "[]]]",                 -3,     1, m()                     },
            { L_, "[[1.0]]",               0,     2, m.a(m.a(1.0))           },
            { L_, "{}",                    0,     1, m.m()                   },
            { L_, WS "{}",                 0,     1, m.m()                   },
            { L_, "{" WS "}",              0,     1, m.m()                   },
            { L_, "{}" WS,                 0,     1, m.m()                   },
            { L_, "{{}",                  -2,     2, m.m()                   },
            { L_, "{{{}",                 -2,     3, m.m()                   },
            { L_, "{{{}}",                -2,     3, m.m()                   },
            { L_, "{}}",                  -3,     1, m.m()                   },
            { L_, "{}}}",                 -3,     1, m.m()                   },
            { L_, "{",                    -2,     1, m()                     },
            { L_, "}",                    -1,     0, m()                     },
            { L_, "{\"object\":{}}",       0,     2, m.m("object", m.m())    },
            { L_, "[{\"obj\":{}}]",        0,     3, m.a(m.m("obj", m.m()))  },
            { L_, WS "{\"object\":{}}",    0,     2, m.m("object", m.m())    },
            { L_, "{" WS "\"object\":{}}", 0,     2, m.m("object", m.m())    },
            { L_, "{\"object\"" WS ":{}}", 0,     2, m.m("object", m.m())    },
            { L_, "{\"object\":" WS "{}}", 0,     2, m.m("object", m.m())    },
            { L_, "{\"object\":{" WS "}}", 0,     2, m.m("object", m.m())    },
            { L_, "{\"object\":{}" WS "}", 0,     2, m.m("object", m.m())    },
            { L_, "{\"object\":{}}}" WS,  -3,     2, m()                     },
            { L_, "{\"firstName\":\"Bart\"}",
                                           0,     1, m.m("firstName", "Bart")},
            { L_, "{\"firstName\":" WS "\"Bart\"}",
                                           0,     1, m.m("firstName", "Bart")},
            { L_, "{\"firstName\":\"Bart\"" WS "}",
                                           0,     1, m.m("firstName", "Bart")},
            { L_, "{\"Name\":{"
                               "\"first\":\"Bart\","
                               "\"last\":\"Simpson\""
                            "}"
                  "}",                     0,     2, m.m("Name", m.m(
                                                        "first", "Bart",
                                                        "last",  "Simpson")) },
            { L_, "{\"Name\":{"
                               "\"first\":\"Bart\"," WS
                               "\"last\":\"Simpson\""
                            "}"
                  "}",                     0,     2, m.m("Name", m.m(
                                                        "first", "Bart",
                                                        "last",  "Simpson")) },
            // Duplicate key test
            { L_, "{\"Name\":{"
                               "\"first\":\"Bart\"," WS
                               "\"first\":\"Lisa\""
                            "}"
                  "}",                     0,     2, m.m("Name", m.m(
                                                        "first",  "Bart")) },
            { L_, "{\"Family\":["
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]"
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":" WS "["
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]"
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":[" WS
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]"
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":["
                         "\"Homer\"," WS
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]"
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":["
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\"" WS
                              "]"
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":["
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]" WS
                  "}",                     0,     2, m.m("Family",
                                                        m.a(
                                                                m("Homer"),
                                                                m("Marge"),
                                                                m("Bart"),
                                                                m("Lisa"),
                                                                m("Maggie"),
                                                                m("Santa's LH")
                                                            ))               },
            { L_, "{\"Family\":["
                         "\"Homer\","
                         "\"Marge\","
                         "\"Bart\","
                         "\"Lisa\","
                         "\"Maggie\","
                         "\"Santa's LH\""
                              "]       ] "  // Note bad extra ']'
                  "}",                    -2,     2, m()                     },
            { L_, LONG_JSON_ARRAY,         0,     1, LONG_DATUM_ARRAY        },
            { L_, LONG_JSON_OBJECT,        0,     1, LONG_DATUM_OBJECT       },
            { L_, DEEP_JSON_ARRAY,         0,    96, DEEP_DATUM_ARRAY        },
            { L_, DEEP_JSON_OBJECT,        0,    72, DEEP_DATUM_OBJECT       },
            { L_, DEEP_JSON_AOA,           0,    95, DEEP_DATUM_AOA          },
        };
#undef WS

        if (verbose)
            cout << "\nTest decoding various valid or invalid JSON strings."
                 << endl;

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        const int DEFAULT_DEPTH =
            baljsn::DatumDecoderOptions().maxNestedDepth();

        // Testing when DEFAULT_DEPTH is not exceeded.
        //   int decode(MgedDatum*, ostream*, streamBuf*);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE  = DATA[ti].d_line;
            const char         *JSON  = DATA[ti].d_json_p;
            const int           RC    = DATA[ti].d_rc;
            const int           DEPTH = DATA[ti].d_depth;
            const bdld::Datum&  DATUM = DATA[ti].d_datum;

            // Depth limit options tested below
            if (DEPTH > DEFAULT_DEPTH) {
                continue;                                           // CONTINUE
            }

            // Must do this before setting up 'da' - 'datum.print()' uses
            // default allocator.
            if(veryVerbose) {
                T_ P_(LINE) P_(JSON) P_(RC) P_(DEPTH) P(DATUM)
            }

            // Testing
            //   int decode(MgedDatum*, ostream*, streamBuf*);
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);

            MD result(&ta);

            bdlsb::FixedMemInStreamBuf isb(JSON, bsl::strlen(JSON));
            bsl::ostringstream         os(&ta);

            int rc = Util::decode(&result, &os, &isb);

            ASSERTV(LINE,
                    RC,
                    rc,
                    result,
                    result->type(),
                    RC == rc);

            if (0 == rc) {
                ASSERTV(LINE,
                        DATUM,
                        result,
                        DATUM == *result);
                ASSERTV(os.str(), os.str().length(), 0 == os.str().length());
            }
            else {
                ASSERTV(os.str(), os.str().length(), 0 != os.str().length());
            }
        }

        // Testing handling of DEPTH option including exceeding DEFAULT_DEPTH.
        //   int decode(MgedDatum*, ostream*, streamBuf*);
        //   int decode(MgedDatum*, ostream*, streamBuf*, const DDOptions&);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE  = DATA[ti].d_line;
            const char         *JSON  = DATA[ti].d_json_p;
            const int           RC    = DATA[ti].d_rc;
            const int           DEPTH = DATA[ti].d_depth;
            const bdld::Datum&  DATUM = DATA[ti].d_datum;

            // Must do this before setting up 'da' - 'datum.print()' uses
            // default allocator.
            if(veryVerbose) {
                T_ P_(LINE) P_(JSON) P_(RC) P_(DEPTH) P(DATUM)
            }

            if (DEPTH <= DEFAULT_DEPTH) {
                bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
                bslma::TestAllocatorMonitor tam(&ta);

                MD result(&ta);

                bdlsb::FixedMemInStreamBuf isb(JSON, bsl::strlen(JSON));
                bsl::ostringstream         os(&ta);

                // Testing
                // int decode(MgedDatum*, ostream*, streamBuf*);
                int rc = Util::decode(&result, &os, &isb);

                ASSERTV(LINE,
                        RC,
                        rc,
                        result,
                        result->type(),
                        RC == rc);

                if (0 == rc) {
                    ASSERTV(LINE,
                            DATUM,
                            result,
                            DATUM == *result);
                    ASSERTV(
                        os.str(), os.str().length(), 0 == os.str().length());
                }
                else {
                    ASSERTV(
                        os.str(), os.str().length(), 0 != os.str().length());
                }
            }

            bsl::vector<baljsn::DatumDecoderOptions> options;

            options.push_back(baljsn::DatumDecoderOptions());

            baljsn::DatumDecoderOptions opt;

            if (DEPTH) {
                if (DEPTH > 2) {
                    opt.setMaxNestedDepth(DEPTH - 2);
                    options.push_back(opt);
                }

                if (DEPTH > 1) {
                    opt.setMaxNestedDepth(DEPTH - 1);
                    options.push_back(opt);
                }

                opt.setMaxNestedDepth(DEPTH);
                options.push_back(opt);

                opt.setMaxNestedDepth(DEPTH + 1);
                options.push_back(opt);
            }
            else {
                opt.setMaxNestedDepth(DEFAULT_DEPTH);
                options.push_back(opt);
            }

            opt.setMaxNestedDepth(1);
            options.push_back(opt);

            opt.setMaxNestedDepth(INT_MAX);
            options.push_back(opt);

            // Testing
            //   int decode(MgedDatum*, ostream*, streamBuf*, const DDOpts&);
            for (bsl::vector<baljsn::DatumDecoderOptions>::const_iterator i =
                     options.begin();
                 i != options.end();
                 ++i) {
                const int OPTIONS_DEPTH = i->maxNestedDepth();

                if(veryVeryVerbose) {
                    T_ T_ P_(LINE) P(OPTIONS_DEPTH)
                }

                bdlsb::FixedMemInStreamBuf isb(JSON, bsl::strlen(JSON));
                bsl::ostringstream         os(&ta);

                MD result(&ta);

                int rc = Util::decode(&result, &os, &isb, *i);

                // If '0==DEPTH', we expected the parse to fail, so we don't
                // expect  the depth option to change anything.  If
                // 'OPTIONS_DEPTH>=DEPTH', the parse should have the expected
                // 'rc'.
                if (DEPTH == 0 || OPTIONS_DEPTH >= DEPTH) {
                    ASSERTV(LINE,
                            RC,
                            rc,
                            result,
                            result->type(),
                            RC == rc);

                    ASSERTV(LINE,
                            DEPTH,
                            OPTIONS_DEPTH,
                            RC == rc);

                    if (0 == rc) {
                        ASSERTV(LINE,
                                DATUM,
                                result,
                                DATUM == *result);
                        ASSERTV(os.str(),
                                os.str().length(),
                                0 == os.str().length());
                    }
                    else {
                        ASSERTV(os.str(),
                                os.str().length(),
                                0 != os.str().length());
                    }
                }
                else { // 'OPTIONS_DEPTH' is insufficient, we expect a failure.
                    ASSERTV(LINE,
                            DEPTH,
                            OPTIONS_DEPTH,
                            rc,
                            0 > rc);

                    if (0 == rc) {
                        ASSERTV(LINE,
                                DEPTH,
                                OPTIONS_DEPTH,
                                DATUM,
                                result,
                                DATUM == *result);
                        ASSERTV(os.str(),
                                os.str().length(),
                                0 == os.str().length());
                    }
                    else {
                        ASSERTV(os.str(),
                                os.str().length(),
                                0 != os.str().length());
                    }
                }
            }
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, const StringRef&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);

            MD result(&ta);

            int rc = Util::decode(&result, LONG_JSON_ARRAY);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);

            rc = Util::decode(&result, DEEP_JSON_ARRAY);
            ASSERTV(rc, 0 != rc);
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, const StringRef&, const DDOptions&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            baljsn::DatumDecoderOptions opt;

            MD result(&ta);

            int rc = Util::decode(&result, LONG_JSON_ARRAY, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);

            rc = Util::decode(&result, DEEP_JSON_ARRAY, opt);
            ASSERTV(rc, 0 != rc);

            opt.setMaxNestedDepth(96);
            rc = Util::decode(&result, DEEP_JSON_ARRAY, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(DEEP_DATUM_ARRAY, result, DEEP_DATUM_ARRAY == *result);
        }


        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, os*, const StrRef&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);

            bsl::ostringstream          os(&ta);

            MD result(&ta);

            int rc = Util::decode(&result, &os, LONG_JSON_ARRAY);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());

            os.str("");
            os.clear();

            rc = Util::decode(&result, &os, DEEP_JSON_ARRAY);
            ASSERTV(rc, 0 != rc);
            ASSERTV(os.str(), os.str().length(), 0 != os.str().length());
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, os*, const StrRef&, const DDOptions&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            baljsn::DatumDecoderOptions opt;

            bsl::ostringstream          os(&ta);

            MD result(&ta);

            int rc = Util::decode(&result, &os, LONG_JSON_ARRAY, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());

            os.str("");
            os.clear();

            rc = Util::decode(&result, &os, DEEP_JSON_ARRAY, opt);
            ASSERTV(rc, 0 != rc);
            ASSERTV(os.str(), os.str().length(), 0 != os.str().length());

            os.str("");
            os.clear();

            opt.setMaxNestedDepth(96);
            rc = Util::decode(&result, &os, DEEP_JSON_ARRAY, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(DEEP_DATUM_ARRAY, result, DEEP_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, streamBuf*);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            bdlsb::FixedMemInStreamBuf  isb(LONG_JSON_ARRAY,
                                            bsl::strlen(LONG_JSON_ARRAY));

            MD result(&ta);

            int rc = Util::decode(&result, &isb);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);

            bdlsb::FixedMemInStreamBuf  isb2(DEEP_JSON_ARRAY,
                                            bsl::strlen(DEEP_JSON_ARRAY));
            rc = Util::decode(&result, &isb);
            ASSERTV(rc, 0 != rc);
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, streamBuf*, const DDOptions&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            baljsn::DatumDecoderOptions opt;

            bdlsb::FixedMemInStreamBuf  isb(LONG_JSON_ARRAY,
                                            bsl::strlen(LONG_JSON_ARRAY));

            MD result(&ta);

            int rc = Util::decode(&result, &isb, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);

            bdlsb::FixedMemInStreamBuf  isb2(DEEP_JSON_ARRAY,
                                            bsl::strlen(DEEP_JSON_ARRAY));
            rc = Util::decode(&result, &isb, opt);
            ASSERTV(rc, 0 != rc);

            opt.setMaxNestedDepth(96);
            rc = Util::decode(&result, &isb2, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(DEEP_DATUM_ARRAY, result, DEEP_DATUM_ARRAY == *result);
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, os*, streamBuf*);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            bdlsb::FixedMemInStreamBuf  isb(LONG_JSON_ARRAY,
                                            bsl::strlen(LONG_JSON_ARRAY));

            bsl::ostringstream          os(&ta);

            MD result(&ta);

            int rc = Util::decode(&result, &os, &isb);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());

            os.str("");
            os.clear();

            bdlsb::FixedMemInStreamBuf  isb2(DEEP_JSON_ARRAY,
                                            bsl::strlen(DEEP_JSON_ARRAY));
            rc = Util::decode(&result, &os, &isb);
            ASSERTV(rc, 0 != rc);
            ASSERTV(os.str(), os.str().length(), 0 != os.str().length());
        }

        // Testing that args are forwarded correctly for:
        //   int decode(MgedDatum*, os*, streamBuf*, const DDOptions&);
        {
            bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);
            baljsn::DatumDecoderOptions opt;

            bdlsb::FixedMemInStreamBuf  isb(LONG_JSON_ARRAY,
                                            bsl::strlen(LONG_JSON_ARRAY));

            bsl::ostringstream          os(&ta);

            MD result(&ta);

            int rc = Util::decode(&result, &os, &isb, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(LONG_DATUM_ARRAY, result, LONG_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());

            os.str("");
            os.clear();

            bdlsb::FixedMemInStreamBuf  isb2(DEEP_JSON_ARRAY,
                                            bsl::strlen(DEEP_JSON_ARRAY));
            rc = Util::decode(&result, &os, &isb, opt);
            ASSERTV(rc, 0 != rc);
            ASSERTV(os.str(), os.str().length(), 0 != os.str().length());

            os.str("");
            os.clear();

            opt.setMaxNestedDepth(96);
            rc = Util::decode(&result, &isb2, opt);
            ASSERTV(rc, 0 == rc);
            ASSERTV(DEEP_DATUM_ARRAY, result, DEEP_DATUM_ARRAY == *result);
            ASSERTV(os.str(), os.str().length(), 0 == os.str().length());
        }
      } break;
      case 4: {
        //---------------------------------------------------------------------
        // BREATHING ROUND-TRIP TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 Construct a 'ManagedDatum', 'encode' it to a string, 'decode' the
        //:   string, then make sure the values match.
        //
        // Testing:
        //   BREATHING ROUND-TRIP TEST
        //---------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "BREATHING ROUND-TRIP TEST" << endl
                 << "=========================" << endl;

        baljsn::DatumEncoderOptions strict_options;
        strict_options.setStrictTypes(true);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("1. Verify we can round-trip strings");

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\ta. round-trip simple string.");
        {
            const bsl::string EXP_STRING = "hello";
            const bsl::string EXP_JSON   = "\"" + EXP_STRING + "\"";
            MD                datum(D::copyString(EXP_STRING, &ta), &ta);
            bsl::string       json(&ta);

            int result = Util::encode(&json, *datum, strict_options);
            ASSERT(0 == result);
            LOOP_ASSERT(json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tb. round-trip string with special characters.");
        {
            const bsl::string EXP_STRING = "\" \\ \b \f \n \r \t \x01";
            const bsl::string EXP_JSON =
                "\"\\\" \\\\ \\b \\f \\n \\r \\t \\u0001\"";

            MD          datum(D::copyString(EXP_STRING, &ta), &ta);
            bsl::string json(&ta);
            int         result = Util::encode(&json, *datum, strict_options);
            ASSERT(0 == result);
            LOOP_ASSERT(json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("2. Verify we can round-trip 'int's");
        {
            int               obj      = 123;
            const bsl::string EXP_JSON = "123";

            MD          datum(D::createInteger(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // int's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isDouble(), other->isDouble());
            ASSERTV(double(obj),
                    other->theDouble(),
                    double(obj) == other->theDouble());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("3. Verify we can round-trip 'double's");
        {
            double            obj      = 1.375;
            const bsl::string EXP_JSON = "1.375";

            MD          datum(D::createDouble(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isDouble(), other->isDouble());
            ASSERTV(obj, other->theDouble(), obj == other->theDouble());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("4. Verify we can round-trip 'bool's");

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\ta. false");
        {
            bool              obj      = false;
            const bsl::string EXP_JSON = "false";

            MD          datum(D::createBoolean(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isBoolean(), other->isBoolean());
            ASSERTV(obj, other->theBoolean(), obj == other->theBoolean());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tb. true");
        {
            bool              obj      = true;
            const bsl::string EXP_JSON = "true";

            MD          datum(D::createBoolean(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isBoolean(), other->isBoolean());
            ASSERTV(obj, other->theBoolean(), obj == other->theBoolean());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("5. Verify we can round-trip 'null's");
        {
            const bsl::string EXP_JSON = "null";

            MD          datum(D::createNull(), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isNull(), other->isNull());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("6. Verify we can round-trip 'Date's");
        {
            bdlt::Date        obj(2001, 12, 25);
            const bsl::string EXP_STRING = "2001-12-25";
            const bsl::string EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDate(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // Date's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("7. Verify we can round-trip 'Datetime's");
        {
            bdlt::Datetime    obj(2001, 12, 25, 15, 59, 57, 123);
            const bsl::string EXP_STRING = "2001-12-25T15:59:57.123";
            const bsl::string EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDatetime(obj, &ta), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // Datetime's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("8. Verify we can round-trip 'DatetimeInterval's");
        {
            bdlt::DatetimeInterval obj(1, 23, 59, 59, 987, 654);
            const bsl::string      EXP_STRING = "+1_23:59:59.987654";
            const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDatetimeInterval(obj, &ta), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // DatetimeInterval's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("9. Verify we can round-trip 'Time's");
        {
            bdlt::Time        obj(13, 14, 15, 678);
            const bsl::string EXP_STRING = "13:14:15.678";
            const bsl::string EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createTime(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // Time's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);
            ASSERTV(other->isString(), other->isString());
            ASSERTV(EXP_STRING,
                    other->theString(),
                    EXP_STRING == other->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("10. Verify we can round-trip 'int64's");
        {
            bsls::Types::Int64 obj      = 12345;
            const bsl::string  EXP_JSON = "12345";

            MD          datum(D::createInteger64(obj, &ta), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum, strict_options);
            // Int64's are not a "safely" encode-able type
            ASSERTV(result, 1 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            MD other(&ta);
            result = Util::decode(&other, json);
            ASSERTV(result, 0 == result);

            PVVV("\tobj: " << obj
                           << ", other->theDouble(): " << other->theDouble());

            ASSERTV(other->isDouble(), other->isDouble());
            ASSERTV(double(obj),
                    other->theDouble(),
                    double(obj) == other->theDouble());
        }
      } break;
      case 3: {
        //---------------------------------------------------------------------
        // BREATHING ENCODE TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 For each supported 'Datum' value type, construct a 'ManagedDatum'
        //:   containing an instance and make sure it is 'encode'-ed
        //:   correctly.
        //
        // Testing:
        //   BREATHING ENCODE TEST
        //---------------------------------------------------------------------


        //---------------------------------------------------------------------
        // ENCODE TEST
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING ENCODE TEST" << endl
                                  << "=====================" << endl;

        baljsn::DatumEncoderOptions strict_options;
        strict_options.setStrictTypes(true);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("1. Verify we can encode strings");

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\ta. Stringify simple string.");
        {
            MD          datum(D::copyString("hello", &ta), &ta);
            bsl::string json(&ta);
            int         result = Util::encode(&json, *datum);
            ASSERT(0 == result);
            LOOP_ASSERT(json, "\"hello\"" == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(0 == result);
            ASSERTV(json, json2, json == json2);
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tb. Stringify string with special characters.");
        {
            MD          datum(D::copyString("\" \\ \b \f \n \r \t \x01", &ta),
                              &ta);
            bsl::string json(&ta);
            int         result = Util::encode(&json, *datum);
            ASSERT(0 == result);
            LOOP_ASSERT(json,
                        "\"\\\" \\\\ \\b \\f \\n \\r \\t \\u0001\"" == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(0 == result);
            ASSERTV(json, json2, json == json2);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("2. Verify we can encode 'Date's");
        {
            bdlt::Date             obj(2001, 12, 25);
            const bsl::string      EXP_STRING = "2001-12-25";
            const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDate(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(1 == result);
            ASSERTV(json, json2, json == json2);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("3. Verify we can encode 'Datetime's");
        {
            bdlt::Datetime         obj(2001, 12, 25, 15, 59, 57, 123);
            const bsl::string      EXP_STRING = "2001-12-25T15:59:57.123";
            const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDatetime(obj, &ta), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(1 == result);
            ASSERTV(json, json2, json == json2);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("4. Verify we can encode 'DatetimeInterval's");
        {
            bdlt::DatetimeInterval obj(1, 23, 59, 59, 987, 654);
            const bsl::string      EXP_STRING = "+1_23:59:59.987654";
            const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createDatetimeInterval(obj, &ta), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(1 == result);
            ASSERTV(json, json2, json == json2);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("5. Verify we can encode 'Time's");
        {
            bdlt::Time             obj(13, 14, 15, 678);
            const bsl::string      EXP_STRING = "13:14:15.678";
            const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

            MD          datum(D::createTime(obj), &ta);
            bsl::string json;
            int         result = Util::encode(&json, *datum);
            ASSERTV(result, 0 == result);
            ASSERTV(EXP_JSON, json, EXP_JSON == json);

            bsl::string json2(&ta);
            result = Util::encode(&json2, *datum, strict_options);
            ASSERT(1 == result);
            ASSERTV(json, json2, json == json2);
        }
      } break;
      case 2: {
        //---------------------------------------------------------------------
        // BREATHING DECODE TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 Construct strings represting the various 'Datum' value types and
        //:   make sure they're parsed correctly.
        //
        // Testing:
        //   BREATHING DECODE TEST
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING DECODE TEST" << endl
                                  << "=====================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("1. Verify we can parse numbers.");

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\ta. Parse a number with sign, decimal, and exponent.")
        {
            const char JSON[] = "   -3.14159e-1   ";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isDouble());
            LOOP_ASSERT(datum->theDouble(), -0.314159 == datum->theDouble());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tb. Parse a number with decimal, and exponent.");
        {
            const char JSON[] = "3.14159e1";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isDouble());
            LOOP_ASSERT(datum->theDouble(), 31.4159 == datum->theDouble());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tc. Parse a number with sign, and decimal.");
        {
            const char JSON[] = "-3.14159";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isDouble());
            LOOP_ASSERT(datum->theDouble(), -3.14159 == datum->theDouble());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\td. Parse a number with sign, and exponent.");
        {
            const char JSON[] = "-3e2";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isDouble());
            LOOP_ASSERT(datum->theDouble(), -300 == datum->theDouble());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\te. Parse a large number, with a large exponent.");
        {
            const char JSON[] = "3e15";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isDouble());
            LOOP_ASSERT(datum->theDouble(),
                        3000000000000000LL == datum->theDouble());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("2. Verify we can parse 'null' values.")
        {
            const char JSON[] = "null";

            MD datum(&ta);
            datum.adopt(D::createInteger(42));
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isNull());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("3. Verify we can parse boolean values.")
        {
            PVVV("\ta. Parse 'true'.");
            const char JSON_TRUE[] = "true";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON_TRUE);
            ASSERT(0 == result);
            ASSERT(datum->isBoolean());
            ASSERT(datum->theBoolean());

            PVVV("\tb. Parse 'false'.");
            const char JSON_FALSE[] = "false";
            datum.makeNull();
            result = Util::decode(&datum, JSON_FALSE);
            ASSERT(0 == result);
            ASSERT(datum->isBoolean());
            ASSERT(!datum->theBoolean());

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("4. Verify we can parse string values.");

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\ta. Parse a normal string.");
        {
            const char JSON[] = "\"hello\"";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isString());
            ASSERTV(
                datum->theString(), "hello", "hello" == datum->theString());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tb. Parse a string with an escaped quote.");
        {
            const char JSON[] = "\"bob says \\\"hi.\\\"\"";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isString());
            ASSERTV(datum->theString(),
                    "bob says \"hi.\"",
                    "bob says \"hi.\"" == datum->theString());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\tc. Parse a string with escaped unicode characters.");
        {
            const char JSON[] = "\"\\u0041\\u0042\\u0043\"";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isString());
            LOOP_ASSERT(datum->theString(), "ABC" == datum->theString());
        }

        // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

        PVVV("\td. Parse a string with all supported escaped characters.");
        {
            const char JSON[] = "\"\\u0041 \\\" \\\\ \\/ \\b \\f "
                                  "\\n \\r \\t \"";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isString());
            LOOP_ASSERT(datum->theString(),
                        "A \" \\ / \b \f \n \r \t " == datum->theString());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("5. Verify we can parse an array.");
        {
            const char JSON[] = "[ -3.14159e1,"
                                "   3.14159e1,"
                                "  -3.14159,"
                                "  -3e2,"
                                "   3e15,"
                                "   [ ]"
                                "]";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isArray());

            DAR array = datum->theArray();
            ASSERT(6 == array.length());

            ASSERT(array[0].isDouble());
            LOOP_ASSERT(array[0].theDouble(),
                        -31.4159 == array[0].theDouble());

            ASSERT(array[1].isDouble());
            LOOP_ASSERT(array[1].theDouble(), 31.4159 == array[1].theDouble());

            ASSERT(array[2].isDouble());
            LOOP_ASSERT(array[2].theDouble(),
                        -3.14159 == array[2].theDouble());

            ASSERT(array[3].isDouble());
            LOOP_ASSERT(array[3].theDouble(), -300 == array[3].theDouble());

            ASSERT(array[4].isDouble());
            LOOP_ASSERT(array[4].theDouble(),
                        3000000000000000LL == array[4].theDouble());

            ASSERT(array[5].isArray());
            DAR subarray = array[5].theArray();
            ASSERT(0 == subarray.length());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("6. Verify we can parse an object.");
        {
            const char JSON[] = "{"
                                "  \"one\":   -3.14159e1,"
                                "  \"two\":    3.14159e1,"
                                "  \"three\": -3.14159,"
                                "  \"four\":  -3e2,"
                                "  \"five\":   3e15,"
                                "  \"six\":    { }"
                                "}";

            MD  datum(&ta);
            int result = Util::decode(&datum, JSON);
            ASSERT(0 == result);
            ASSERT(datum->isMap());

            DMR map = datum->theMap();
            LOOP_ASSERT(map.size(), 6 == map.size());

            const D *val = map.find("one");
            ASSERT(0 != val);
            ASSERT(val->isDouble());
            LOOP_ASSERT(val->theDouble(), -31.4159 == val->theDouble());

            val = map.find("two");
            ASSERT(0 != val);
            ASSERT(val->isDouble());
            LOOP_ASSERT(val->theDouble(), 31.4159 == val->theDouble());

            val = map.find("three");
            ASSERT(0 != val);
            ASSERT(val->isDouble());
            LOOP_ASSERT(val->theDouble(), -3.14159 == val->theDouble());

            val = map.find("four");
            ASSERT(0 != val);
            ASSERT(val->isDouble());
            LOOP_ASSERT(val->theDouble(), -300 == val->theDouble());

            val = map.find("five");
            ASSERT(0 != val);
            ASSERT(val->isDouble());
            LOOP_ASSERT(val->theDouble(),
                        3000000000000000LL == val->theDouble());

            val = map.find("six");
            ASSERT(0 != val);
            ASSERT(val->isMap());
            DMR submap = val->theMap();
            ASSERT(0 == submap.size());
        }
      } break;
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 Construct a 'ManagedDatum', convert it to a string, parse the
        //:   string, then make sure the values match.
        //
        // Testing:
        //   BREATHING TEST
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        DMB map(&ta);

        map.pushBack("integer", D::createDouble(42));
        map.pushBack("double",  D::createDouble(4.75));
        map.pushBack("string",  D::copyString("hello", &ta));

        DAB array(&ta);
        array.pushBack(D::createDouble(0));
        array.pushBack(D::createDouble(-3.1416));
        array.pushBack(D::copyString("A long string", &ta));
        array.pushBack(D::copyString("Abc", &ta));

        map.pushBack("array", array.commit());
        MD          datum(map.commit(), &ta);
        bsl::string json;
        int         result = Util::encode(&json, *datum);
        ASSERTV(result, 0 == result);
        if (veryVerbose) P(json);

        MD other(&ta);
        result = Util::decode(&other, json);
        ASSERTV(result, 0 == result);
        ASSERTV(datum, other, datum == other);

      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: DOES NOT ALLOCATE MEMORY FROM GLOBAL ALLOCATOR

    ASSERTV(gam.isTotalSame());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
