// baljsn_datumutil.t.cpp                                             -*-C++-*-
#include <baljsn_datumutil.h>

#include <baljsn_simpleformatter.h>

#include <bdlb_stringviewutil.h>

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

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>            // to verify that we do not
#include <bslma_testallocatormonitor.h>     // allocate any memory

#include <bsls_alignedbuffer.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

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
// [ 5] int decode(MgedDatum*, const string_view&);
// [ 5] int decode(MgedDatum*, const StringRef&, const DDOptions&);
// [ 5] int decode(MgedDatum*, os*, const string_view&);
// [ 5] int decode(MgedDatum*, os*, const string_view&, const DDOptions&);
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
//                          GLOBAL VARIABLES
// ----------------------------------------------------------------------------

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                HELPERS FOR REVIEW & LOG MESSAGE HANDLING
// ----------------------------------------------------------------------------

static bool containsCaseless(const bsl::string_view& string,
                             const bsl::string_view& subString)
    // Return 'true' if the specified 'subString' is present in the specified
    // 'string' disregarding case of alphabet characters '[a-zA-Z]', otherwise
    // return 'false'.
{
    if (subString.empty()) {
        return true;                                                  // RETURN
    }

    typedef bdlb::StringViewUtil SVU;
    const bsl::string_view rsv = SVU::strstrCaseless(string, subString);

    return !rsv.empty();
}

// ============================================================================
//                    EXPECTED 'BSLS_REVIEW' TEST HANDLERS
// ----------------------------------------------------------------------------

// These handlers are needed only temporarily until we determine how to fix the
// broken contract of 'bdlb::NumericParseUtil::parseDouble()' that says under-
// and overflow is not allowed yet the function supports it.

bool isBdlbNumericParseUtilReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' has been raised by the
    // 'bdlb_numericparseutil' component or no source file names are supported
    // by the build, otherwise return 'false'.
{
    const char *fn = reviewViolation.fileName();
    const bool fileOk = ('\0' == fn[0]) // empty or has the component name
                              || containsCaseless(fn, "bdlb_numericparseutil");
    return fileOk;
}

bool isParseDoubleReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' is an over/underflow or
    // hexfloat message from the 'bdlb_numericparseutil' component (or no
    // source file names are supported by the build configuration), otherwise
    // return 'false'.
{
    return isBdlbNumericParseUtilReview(reviewViolation) &&
           (containsCaseless(reviewViolation.comment(), "overflow") ||
            containsCaseless(reviewViolation.comment(), "underflow") ||
            containsCaseless(reviewViolation.comment(), "hexfloat"));
}

void ignoreParseDoubleMsgs(const bsls::ReviewViolation& reviewViolation)
    // If the specified 'reviewViolation' is an expected parseDouble-related
    // message (over/underflow, parsed a hexfloat) do nothing, otherwise call
    // 'bsls::Review::failByAbort()'.
{
    if (!isParseDoubleReview(reviewViolation)) {
        bsls::Review::failByAbort(reviewViolation);
    }
}


// ============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::DatumUtil Util;

typedef bdld::ManagedDatum       MD;
typedef bdld::Datum              D;
typedef bdld::DatumArrayBuilder  DAB;
typedef bdld::DatumArrayRef      DAR;
typedef bdld::DatumMapBuilder    DMB;
typedef bdld::DatumMapEntry      DME;
typedef bdld::DatumMapRef        DMR;

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

                                // ------
                                // case 7
                                // ------

namespace TestCase07 {

struct TestCase07Data {
    int         d_line;
    const char *d_testName;        // name of the test file
    const char *d_JSON;            // JSON contents of the test
    bool        d_isValid;         // 'true' if d_JSON is valid JSON
};

struct FailureListData {
    int         d_line;
    const char *d_testName;  // name of the test file
    const char *d_reason;    // reason this test is allowed to fail
};

template <class STRING>
void testCase07(const TestCase07Data  *data,
                int                    numData,
                const FailureListData *failureList,
                int                    numFailureList)
{
    bsl::unordered_set<STRING> allowedFailureList;

    for (int ti = 0; ti < numFailureList; ++ti) {
        allowedFailureList.insert(failureList[ti].d_testName);
    }

    if (verbose)
        cout << "\nTest decoding JSON test suite test cases."
             << endl;

    for (int ti = 0; ti < numData; ++ti) {
        const int   LINE      = data[ti].d_line;
        const char *TEST_NAME = data[ti].d_testName;
        const char *JSON      = data[ti].d_JSON;
        const bool  IS_VALID  = data[ti].d_isValid;
        const bool  IS_ALLOWED_FAILURE_LISTED =
            (allowedFailureList.find(TEST_NAME) !=
             allowedFailureList.end());

        bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor tam(&ta);

        MD result(&ta);

        bdlsb::FixedMemInStreamBuf isb(JSON, bsl::strlen(JSON));
        bsl::ostringstream         os(&ta);

        int rc = Util::decode(&result, &os, &isb);

        if (veryVerbose) {
            if (IS_VALID && (0 == rc)) {
                STRING encoded;
                int    encodeRC;

                encodeRC = Util::encode(&encoded, *result);

                cout << "ROUND TRIP: " << LINE << "\n"
                     << "\t" << "TEST_NAME :  "   << TEST_NAME << "\n"
                     << "\t" << "JSON      :    "   << JSON << "\n"
                     << "\t" << "result    :    "   << result << "\n"
                     << "\t" << "encodeRC :    "   << encodeRC << "\n"
                     << "\t" << "ENCODED   :   \""  << encoded << "\"\n";
            }
        }

        if (IS_VALID != (0 == rc)) {
            if (!IS_ALLOWED_FAILURE_LISTED) {
                cout << "FAILED: " << LINE << "\n"
                     << "\t" << "TEST_NAME: " << TEST_NAME << "\n"
                     << "\t" << "JSON   : "   << JSON << "\n"
                     << "\t" << "result : "   << result << "\n";

                ASSERTV(LINE,
                        rc,
                        IS_VALID,
                        IS_VALID == (rc == 0));
            }
        }
        else if (IS_ALLOWED_FAILURE_LISTED) {
            ASSERTV("UNEXPECTED ALLOWED_FAILURE_LIST ENTRY RESULT",
                    LINE,
                    TEST_NAME,
                    IS_VALID,
                    rc,
                    false);
        }
    }

    // Reproduce the "n_structure_100000_opening_arrays.json"
    // test case, which is too large to be placed in the DATA
    // array.

    {
        const int   LINE      = __LINE__;
        const char *TEST_NAME = "n_structure_100000_opening_arrays.json";
        const bool  IS_VALID  = false;
        const bool  IS_ALLOWED_FAILURE_LISTED =
            (allowedFailureList.find(TEST_NAME) !=
             allowedFailureList.end());

        bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor tam(&ta);

        STRING    JSON;
        const int TARGET_SIZE = 100000;
        // Over-reserve, since our construction loop is likely to
        // overshoot.
        JSON.reserve(TARGET_SIZE + 1000);
        while (JSON.length() < TARGET_SIZE) {
            JSON+="[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[";
        }

        MD result(&ta);

        bdlsb::FixedMemInStreamBuf isb(JSON.c_str(), JSON.length());
        bsl::ostringstream         os(&ta);

        int rc = Util::decode(&result, &os, &isb);

        if (veryVerbose) {
            if (IS_VALID && (0 == rc)) {
                STRING encoded;
                int    encodeRC;

                encodeRC = Util::encode(&encoded, *result);

                cout << "ROUND TRIP: " << LINE << "\n"
                     << "\t" << "TEST_NAME :  "   << TEST_NAME << "\n"
                     << "\t" << "JSON      :    "   << JSON << "\n"
                     << "\t" << "result    :    "   << result << "\n"
                     << "\t" << "encodeRC :    "   << encodeRC << "\n"
                     << "\t" << "ENCODED   :   \""  << encoded << "\"\n";
            }
        }

        if (IS_VALID != (0 == rc)) {
            if (!IS_ALLOWED_FAILURE_LISTED) {
                cout << "FAILED: " << LINE << "\n"
                     << "\t" << "TEST_NAME: " << TEST_NAME << "\n"
                     << "\t" << "JSON   : "   << JSON << "\n"
                     << "\t" << "result : "   << result << "\n";

                ASSERTV(LINE,
                        rc,
                        IS_VALID,
                        IS_VALID == (rc == 0));
            }
        }
        else if (IS_ALLOWED_FAILURE_LISTED) {
            ASSERTV("UNEXPECTED ALLOWED_FAILURE_LIST ENTRY RESULT",
                    LINE,
                    TEST_NAME,
                    IS_VALID,
                    rc,
                    false);
        }
    }

    // Reproduce the "n_structure_open_array_object.json"
    // test case, which is too large to be placed in the DATA
    // array.

    {
        const int   LINE      = __LINE__;
        const char *TEST_NAME = "n_structure_open_array_object.json";
        const bool  IS_VALID  = false;
        const bool  IS_ALLOWED_FAILURE_LISTED =
            (allowedFailureList.find(TEST_NAME) !=
             allowedFailureList.end());

        bslma::TestAllocator        ta("test", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor tam(&ta);

        STRING    JSON;
        const int TARGET_SIZE = 250000;
        // Over-reserve, since our construction loop is likely to
        // overshoot.
        JSON.reserve(TARGET_SIZE + 1000);
        while (JSON.length() < TARGET_SIZE) {
            JSON += "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":"
                    "[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":[{\"\":";
        }

        MD result(&ta);

        bdlsb::FixedMemInStreamBuf isb(JSON.c_str(), JSON.length());
        bsl::ostringstream         os(&ta);

        int rc = Util::decode(&result, &os, &isb);

        if (veryVerbose) {
            if (IS_VALID && (0 == rc)) {
                STRING encoded;
                int    encodeRC;

                encodeRC = Util::encode(&encoded, *result);

                cout << "ROUND TRIP: " << LINE << "\n"
                     << "\t" << "TEST_NAME :  "   << TEST_NAME << "\n"
                     << "\t" << "JSON      :    "   << JSON << "\n"
                     << "\t" << "result    :    "   << result << "\n"
                     << "\t" << "encodeRC :    "   << encodeRC << "\n"
                     << "\t" << "ENCODED   :   \""  << encoded << "\"\n";
            }
        }

        if (IS_VALID != (0 == rc)) {
            if (!IS_ALLOWED_FAILURE_LISTED) {
                cout << "FAILED: " << LINE << "\n"
                     << "\t" << "TEST_NAME: " << TEST_NAME << "\n"
                     << "\t" << "JSON   : "   << JSON << "\n"
                     << "\t" << "result : "   << result << "\n";

                ASSERTV(LINE,
                        rc,
                        IS_VALID,
                        IS_VALID == (rc == 0));
            }
        }
        else if (IS_ALLOWED_FAILURE_LISTED) {
            ASSERTV("UNEXPECTED ALLOWED_FAILURE_LIST ENTRY RESULT",
                    LINE,
                    TEST_NAME,
                    IS_VALID,
                    rc,
                    false);
        }
    }
}

}  // close namespace TestCase07

                                // ------
                                // case 6
                                // ------

namespace TestCase06 {

typedef baljsn::EncodingStyle::Value TStyle;

struct TestCase6Data {
    int          d_line;
    bdld::Datum  d_datum;
    TStyle       d_encodingStyle;
    int          d_initialIndentLevel;
    int          d_spacesPerLevel;
    const char  *d_json;
    int          d_rcNoStrictTypes;
    int          d_rcWithStrictTypes;
};

template <class STRING>
void testCase06(const TestCase6Data *data, int numData)
{
    if (verbose)
        cout << "\nTest encoding various datums."
             << endl;

    for (int ti = 0; ti < numData; ++ti) {
        const int           LINE           = data[ti].d_line;
        const bdld::Datum&  DATUM          = data[ti].d_datum;
        const TStyle        ES             = data[ti].d_encodingStyle;
        const int           IIL            = data[ti].d_initialIndentLevel;
        const int           SPL            = data[ti].d_spacesPerLevel;
        const STRING        JSON           = data[ti].d_json;
        const int           RC_BY_STRICT[] = {
                                               data[ti].d_rcNoStrictTypes,
                                               data[ti].d_rcWithStrictTypes
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

            if (bdld::Datum::e_DECIMAL64 == DATUM.type()) {
                // Test when 'EncodeQuotedDecimal64' set to 'false'.
                baljsn::DatumEncoderOptions opts2(opts);

                opts2.setEncodeQuotedDecimal64(false);

                STRING result;
                int rc = Util::encode(&result, DATUM, opts2);

                ASSERTV(rc, 0 == rc);

                bsl::string exp(JSON);
                size_t offset = bsl::string::npos;
                while (bsl::string::npos !=
                                     (offset = exp.find_first_of('"'))) {
                    exp.erase(offset, 1);
                }

                ASSERTV(result, exp, exp == result);
            }

            for (int strictTypes = 0; strictTypes < 2; ++strictTypes) {
                const int RC = RC_BY_STRICT[strictTypes];
                opts.setStrictTypes(bool(RC));

                if (veryVeryVerbose) {
                    T_ T_ P_(strictTypes) P(RC);
                }

                if (veryVeryVerbose)
                    cout << "int encode(string*,const Datum&,Opts&);" << endl;
                {
                    STRING result;

                    int rc = Util::encode(&result, DATUM, opts);
                    ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                    if (0 == rc || 2 <= rc) {
                        ASSERTV(LINE,
                                RC,
                                rc,
                                DATUM,
                                JSON,
                                result,
                                JSON == result);
                    }
                }

                // Test that default Options are handled correctly.
                if (baljsn::EncodingStyle::e_COMPACT == ES && 0 == IIL &&
                    0 == SPL && 0 == strictTypes) {
                    if (veryVeryVerbose)
                        cout << "int encode(string*,const Datum&);"
                             << endl;
                    {
                        STRING result;

                        int rc = Util::encode(&result, DATUM);
                        ASSERTV(LINE, strictTypes, RC, rc, RC == rc);

                        if (0 == rc || 2 <= rc) {
                            ASSERTV(LINE,
                                    RC,
                                    rc,
                                    DATUM,
                                    JSON,
                                    result,
                                    JSON == result);
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

                    if (0 == rc || 2 <= rc) {
                        const bslstl::StringRef result(
                            sb.data(), sb.length());

                        ASSERTV(LINE,
                                RC,
                                rc,
                                DATUM,
                                JSON,
                                result,
                                JSON == result);
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

                            ASSERTV(LINE,
                                    RC,
                                    rc,
                                    DATUM,
                                    JSON,
                                    result,
                                    JSON == result);
                        }
                    }
                }
            }
        }
    }
}

}  // close namespace TestCaes06;


                                    // ------
                                    // case 4
                                    // ------


template <class STRING>
void testCase04()
{
    bslma::TestAllocator ta("test");

    baljsn::DatumEncoderOptions strict_options;
    strict_options.setStrictTypes(true);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("1. Verify we can round-trip strings");

    // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

    PVVV("\ta. round-trip simple string.");
    {
        const STRING      EXP_STRING = "hello";
        const STRING      EXP_JSON   = "\"" + EXP_STRING + "\"";
        MD                datum(D::copyString(EXP_STRING, &ta), &ta);
        STRING            json;

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
        const STRING      EXP_STRING = "\" \\ \b \f \n \r \t \x01";
        const STRING      EXP_JSON =
            "\"\\\" \\\\ \\b \\f \\n \\r \\t \\u0001\"";

        MD          datum(D::copyString(EXP_STRING, &ta), &ta);
        STRING      json;
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
        const STRING      EXP_JSON = "123";

        MD          datum(D::createInteger(obj), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'int's are not a "safely" encode-able type
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
        const STRING      EXP_JSON = "1.375";

        MD          datum(D::createDouble(obj), &ta);
        STRING      json;
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
        const STRING      EXP_JSON = "false";

        MD          datum(D::createBoolean(obj), &ta);
        STRING      json;
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
        const STRING      EXP_JSON = "true";

        MD          datum(D::createBoolean(obj), &ta);
        STRING      json;
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
        const STRING      EXP_JSON = "null";

        MD          datum(D::createNull(), &ta);
        STRING      json;
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
        const STRING      EXP_STRING = "2001-12-25";
        const STRING      EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDate(obj), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'bdlt::Date's are not a "safely" encode-able type
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
        const STRING      EXP_STRING = "2001-12-25T15:59:57.123";
        const STRING      EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDatetime(obj, &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'bdlt::Datetime's are not a "safely" encode-able type
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
        const STRING           EXP_STRING = "+1_23:59:59.987654";
        const STRING           EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDatetimeInterval(obj, &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'bdlt::DatetimeInterval's are not a "safely" encode-able type
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
        const STRING      EXP_STRING = "13:14:15.678";
        const STRING      EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createTime(obj), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'bdlt::Time's are not a "safely" encode-able type
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
        const STRING       EXP_JSON = "12345";

        MD          datum(D::createInteger64(obj, &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum, strict_options);
        // 'bsls::Int64's are not a "safely" encode-able type
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
}

                                // ------
                                // case 3
                                // ------

template <class STRING>
void testCase03()
{
    bslma::TestAllocator ta("test");

    baljsn::DatumEncoderOptions strict_options;
    strict_options.setStrictTypes(true);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("1. Verify we can encode strings");

    // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

    PVVV("\ta. Stringify simple string.");
    {
        MD          datum(D::copyString("hello", &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERT(0 == result);
        LOOP_ASSERT(json, "\"hello\"" == json);

        STRING json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(0 == result);
        ASSERTV(json, json2, json == json2);
    }

    // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

    PVVV("\tb. Stringify string with special characters.");
    {
        MD          datum(D::copyString("\" \\ \b \f \n \r \t \x01", &ta),
                          &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERT(0 == result);
        LOOP_ASSERT(json,
                    "\"\\\" \\\\ \\b \\f \\n \\r \\t \\u0001\"" == json);

        STRING      json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(0 == result);
        ASSERTV(json, json2, json == json2);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("2. Verify we can encode 'Date's");
    {
        bdlt::Date             obj(2001, 12, 25);
        const STRING           EXP_STRING = "2001-12-25";
        const STRING           EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDate(obj), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERTV(result, 0 == result);
        ASSERTV(EXP_JSON, json, EXP_JSON == json);

        STRING      json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(1 == result);
        ASSERTV(json, json2, json == json2);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("3. Verify we can encode 'Datetime's");
    {
        bdlt::Datetime         obj(2001, 12, 25, 15, 59, 57, 123);
        const STRING           EXP_STRING = "2001-12-25T15:59:57.123";
        const STRING           EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDatetime(obj, &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERTV(result, 0 == result);
        ASSERTV(EXP_JSON, json, EXP_JSON == json);

        STRING      json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(1 == result);
        ASSERTV(json, json2, json == json2);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("4. Verify we can encode 'DatetimeInterval's");
    {
        bdlt::DatetimeInterval obj(1, 23, 59, 59, 987, 654);
        const STRING           EXP_STRING = "+1_23:59:59.987654";
        const STRING           EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createDatetimeInterval(obj, &ta), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERTV(result, 0 == result);
        ASSERTV(EXP_JSON, json, EXP_JSON == json);

        STRING      json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(1 == result);
        ASSERTV(json, json2, json == json2);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    PVV("5. Verify we can encode 'Time's");
    {
        bdlt::Time             obj(13, 14, 15, 678);
        const STRING           EXP_STRING = "13:14:15.678";
        const bsl::string      EXP_JSON   = "\"" + EXP_STRING + "\"";

        MD          datum(D::createTime(obj), &ta);
        STRING      json;
        int         result = Util::encode(&json, *datum);
        ASSERTV(result, 0 == result);
        ASSERTV(EXP_JSON, json, EXP_JSON == json);

        STRING      json2;
        result = Util::encode(&json2, *datum, strict_options);
        ASSERT(1 == result);
        ASSERTV(json, json2, json == json2);
    }
}

                                // ------
                                // case 1
                                // ------

template <class STRING>
void testCase01()
{
    bslma::TestAllocator ta("test");

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
    STRING      json;
    int         result = Util::encode(&json, *datum);
    ASSERTV(result, 0 == result);
    if (veryVerbose) P(json);

    MD other(&ta);
    result = Util::decode(&other, json);
    ASSERTV(result, 0 == result);
    ASSERTV(datum, other, datum == other);
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&ignoreParseDoubleMsgs);

    // CONCERN: DOES NOT ALLOCATE MEMORY FROM GLOBAL ALLOCATOR

    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor gam(&ga);

    switch (test) { case 0:
      case 9: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

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
                                "Title", "Ancillary Justice"),
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
        "Title" : "Ancillary Justice"
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
        "        \"Title\" : \"Ancillary Justice\"\n"
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
      case 8: {
        // --------------------------------------------------------------------
        // REPRODUCE BUG FROM DRQS 165776192
        //
        // Concerns:
        //: 1 The 'baljsn::DatumUtil::decode' is exception-neutral w.r.t.
        //:   memory allocation.
        //
        // Plan:
        //:  1 Invoke 'baljsn::DatumUtil::decode' in the presence of exception
        //:    (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "REPRODUCE BUG FROM DRQS 165776192\n"
                             "=================================\n";
        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        const char   *jsn = "[\"first element\",\"second element\","
                            "{\"object\":{\"key\":\"value\"}}]";

        bsl::string   JSON(jsn, &ta);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

            MD result(&ta);

            bdlsb::FixedMemInStreamBuf isb(JSON.c_str(), JSON.length());
            bsl::ostringstream         os(&ta);

            int rc = Util::decode(&result, &os, &isb);
            ASSERTV(rc, 0 == rc);

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
      } break;
      case 7: {
        //---------------------------------------------------------------------
        // JSON VALIDATION SUITE TEST
        //   This case tests that the 'decode' methods give correct results
        //   on the JSON validation suite at
        //   https://github.com/nst/JSONTestSuite.
        //
        // Concerns:
        //: 1 The 'decode' method returns the expected result for the
        //:   validation suite entries.
        //
        // Plan:
        //: 1 'decode' all the validation suite strings into 'Datum's, and
        //:   compare the return code with the source string validity.
        //
        // Testing:
        //---------------------------------------------------------------------

        namespace TC = TestCase07;

        // Test data generated from https://github.com/nst/JSONTestSuite (MIT
        // License)
        // Generation done by the
        // https://bbgithub.dev.bloomberg.com/mgiroux/JSONTestSuite fork's
        // `cpp-test-data-converter.pl` script.

        static const TC::TestCase07Data DATA[] = {
            { L_, "n_array_1_true_without_comma.json",
              "[1 true]",
              false }
          , { L_, "n_array_a_invalid_utf8.json",
              "[a\345]",
              false }
          , { L_, "n_array_colon_instead_of_comma.json",
              "[\"\": 1]",
              false }
          , { L_, "n_array_comma_after_close.json",
              "[\"\"],",
              false }
          , { L_, "n_array_comma_and_number.json",
              "[,1]",
              false }
          , { L_, "n_array_double_comma.json",
              "[1,,2]",
              false }
          , { L_, "n_array_double_extra_comma.json",
              "[\"x\",,]",
              false }
          , { L_, "n_array_extra_close.json",
              "[\"x\"]]",
              false }
          , { L_, "n_array_extra_comma.json",
              "[\"\",]",
              false }
          , { L_, "n_array_incomplete.json",
              "[\"x\"",
              false }
          , { L_, "n_array_incomplete_invalid_value.json",
              "[x",
              false }
          , { L_, "n_array_inner_array_no_comma.json",
              "[3[4]]",
              false }
          , { L_, "n_array_invalid_utf8.json",
              "[\377]",
              false }
          , { L_, "n_array_items_separated_by_semicolon.json",
              "[1:2]",
              false }
          , { L_, "n_array_just_comma.json",
              "[,]",
              false }
          , { L_, "n_array_just_minus.json",
              "[-]",
              false }
          , { L_, "n_array_missing_value.json",
              "[   , \"\"]",
              false }
          , { L_, "n_array_newlines_unclosed.json",
              "[\"a\","
              "4"
              ",1,",
              false }
          , { L_, "n_array_number_and_comma.json",
              "[1,]",
              false }
          , { L_, "n_array_number_and_several_commas.json",
              "[1,,]",
              false }
          , { L_, "n_array_spaces_vertical_tab_formfeed.json",
              "[\"\013a\"\\f]",
              false }
          , { L_, "n_array_star_inside.json",
              "[*]",
              false }
          , { L_, "n_array_unclosed.json",
              "[\"\"",
              false }
          , { L_, "n_array_unclosed_trailing_comma.json",
              "[1,",
              false }
          , { L_, "n_array_unclosed_with_new_lines.json",
              "[1,"
              "1"
              ",1",
              false }
          , { L_, "n_array_unclosed_with_object_inside.json",
              "[{}",
              false }
          , { L_, "n_incomplete_false.json",
              "[fals]",
              false }
          , { L_, "n_incomplete_null.json",
              "[nul]",
              false }
          , { L_, "n_incomplete_true.json",
              "[tru]",
              false }
          , { L_, "n_multidigit_number_then_00.json",
              "123\000",
              false }
          , { L_, "n_number_++.json",
              "[++1234]",
              false }
          , { L_, "n_number_+1.json",
              "[+1]",
              false }
          , { L_, "n_number_+Inf.json",
              "[+Inf]",
              false }
          , { L_, "n_number_-01.json",
              "[-01]",
              false }
          , { L_, "n_number_-1.0..json",
              "[-1.0.]",
              false }
          , { L_, "n_number_-2..json",
              "[-2.]",
              false }
          , { L_, "n_number_-NaN.json",
              "[-NaN]",
              false }
          , { L_, "n_number_.-1.json",
              "[.-1]",
              false }
          , { L_, "n_number_.2e-3.json",
              "[.2e-3]",
              false }
          , { L_, "n_number_0.1.2.json",
              "[0.1.2]",
              false }
          , { L_, "n_number_0.3e+.json",
              "[0.3e+]",
              false }
          , { L_, "n_number_0.3e.json",
              "[0.3e]",
              false }
          , { L_, "n_number_0.e1.json",
              "[0.e1]",
              false }
          , { L_, "n_number_0_capital_E+.json",
              "[0E+]",
              false }
          , { L_, "n_number_0_capital_E.json",
              "[0E]",
              false }
          , { L_, "n_number_0e+.json",
              "[0e+]",
              false }
          , { L_, "n_number_0e.json",
              "[0e]",
              false }
          , { L_, "n_number_1.0e+.json",
              "[1.0e+]",
              false }
          , { L_, "n_number_1.0e-.json",
              "[1.0e-]",
              false }
          , { L_, "n_number_1.0e.json",
              "[1.0e]",
              false }
          , { L_, "n_number_1_000.json",
              "[1 000.0]",
              false }
          , { L_, "n_number_1eE2.json",
              "[1eE2]",
              false }
          , { L_, "n_number_2.e+3.json",
              "[2.e+3]",
              false }
          , { L_, "n_number_2.e-3.json",
              "[2.e-3]",
              false }
          , { L_, "n_number_2.e3.json",
              "[2.e3]",
              false }
          , { L_, "n_number_9.e+.json",
              "[9.e+]",
              false }
          , { L_, "n_number_expression.json",
              "[1+2]",
              false }
          , { L_, "n_number_hex_1_digit.json",
              "[0x1]",
              false }
          , { L_, "n_number_hex_2_digits.json",
              "[0x42]",
              false }
          , { L_, "n_number_Inf.json",
              "[Inf]",
              false }
          , { L_, "n_number_infinity.json",
              "[Infinity]",
              false }
          , { L_, "n_number_invalid+-.json",
              "[0e+-1]",
              false }
          , { L_, "n_number_invalid-negative-real.json",
              "[-123.123foo]",
              false }
          , { L_, "n_number_invalid-utf-8-in-bigger-int.json",
              "[123\345]",
              false }
          , { L_, "n_number_invalid-utf-8-in-exponent.json",
              "[1e1\345]",
              false }
          , { L_, "n_number_invalid-utf-8-in-int.json",
              "[0\345]",
              false }
          , { L_, "n_number_minus_infinity.json",
              "[-Infinity]",
              false }
          , { L_, "n_number_minus_sign_with_trailing_garbage.json",
              "[-foo]",
              false }
          , { L_, "n_number_minus_space_1.json",
              "[- 1]",
              false }
          , { L_, "n_number_NaN.json",
              "[NaN]",
              false }
          , { L_, "n_number_neg_int_starting_with_zero.json",
              "[-012]",
              false }
          , { L_, "n_number_neg_real_without_int_part.json",
              "[-.123]",
              false }
          , { L_, "n_number_neg_with_garbage_at_end.json",
              "[-1x]",
              false }
          , { L_, "n_number_real_garbage_after_e.json",
              "[1ea]",
              false }
          , { L_, "n_number_real_with_invalid_utf8_after_e.json",
              "[1e\345]",
              false }
          , { L_, "n_number_real_without_fractional_part.json",
              "[1.]",
              false }
          , { L_, "n_number_starting_with_dot.json",
              "[.123]",
              false }
          , { L_, "n_number_U+FF11_fullwidth_digit_one.json",
              "[\357\274\221]",
              false }
          , { L_, "n_number_with_alpha.json",
              "[1.2a-3]",
              false }
          , { L_, "n_number_with_alpha_char.json",
              "[1.8011670033376514H-308]",
              false }
          , { L_, "n_number_with_leading_zero.json",
              "[012]",
              false }
          , { L_, "n_object_bad_value.json",
              "[\"x\", truth]",
              false }
          , { L_, "n_object_bracket_key.json",
              "{[: \"x\"}",
              false }
          , { L_, "n_object_comma_instead_of_colon.json",
              "{\"x\", null}",
              false }
          , { L_, "n_object_double_colon.json",
              "{\"x\"::\"b\"}",
              false }
          , { L_, "n_object_emoji.json",
              "{\360\237\207\250\360\237\207\255}",
              false }
          , { L_, "n_object_garbage_at_end.json",
              "{\"a\":\"a\" 123}",
              false }
          , { L_, "n_object_key_with_single_quotes.json",
              "{key: \047value\047}",
              false }
          , { L_,
              "n_object_lone_continuation_byte_in_key_and_trailing_comma.json",
              "{\"\271\":\"0\",}",
              false }
          , { L_, "n_object_missing_colon.json",
              "{\"a\" b}",
              false }
          , { L_, "n_object_missing_key.json",
              "{:\"b\"}",
              false }
          , { L_, "n_object_missing_semicolon.json",
              "{\"a\" \"b\"}",
              false }
          , { L_, "n_object_missing_value.json",
              "{\"a\":",
              false }
          , { L_, "n_object_no-colon.json",
              "{\"a\"",
              false }
          , { L_, "n_object_non_string_key.json",
              "{1:1}",
              false }
          , { L_, "n_object_non_string_key_but_huge_number_instead.json",
              "{9999E9999:1}",
              false }
          , { L_, "n_object_repeated_null_null.json",
              "{null:null,null:null}",
              false }
          , { L_, "n_object_several_trailing_commas.json",
              "{\"id\":0,,,,,}",
              false }
          , { L_, "n_object_single_quote.json",
              "{\047a\047:0}",
              false }
          , { L_, "n_object_trailing_comma.json",
              "{\"id\":0,}",
              false }
          , { L_, "n_object_trailing_comment.json",
              "{\"a\":\"b\"}/**/",
              false }
          , { L_, "n_object_trailing_comment_open.json",
              "{\"a\":\"b\"}/**//",
              false }
          , { L_, "n_object_trailing_comment_slash_open.json",
              "{\"a\":\"b\"}//",
              false }
          , { L_, "n_object_trailing_comment_slash_open_incomplete.json",
              "{\"a\":\"b\"}/",
              false }
          , { L_, "n_object_two_commas_in_a_row.json",
              "{\"a\":\"b\",,\"c\":\"d\"}",
              false }
          , { L_, "n_object_unquoted_key.json",
              "{a: \"b\"}",
              false }
          , { L_, "n_object_unterminated-value.json",
              "{\"a\":\"a",
              false }
          , { L_, "n_object_with_single_string.json",
              "{ \"foo\" : \"bar\", \"a\" }",
              false }
          , { L_, "n_object_with_trailing_garbage.json",
              "{\"a\":\"b\"}#",
              false }
          , { L_, "n_single_space.json",
              " ",
              false }
          , { L_, "n_string_1_surrogate_then_escape.json",
              "[\"\\uD800\\\"]",
              false }
          , { L_, "n_string_1_surrogate_then_escape_u.json",
              "[\"\\uD800\\u\"]",
              false }
          , { L_, "n_string_1_surrogate_then_escape_u1.json",
              "[\"\\uD800\\u1\"]",
              false }
          , { L_, "n_string_1_surrogate_then_escape_u1x.json",
              "[\"\\uD800\\u1x\"]",
              false }
          , { L_, "n_string_accentuated_char_no_quotes.json",
              "[\303\251]",
              false }
          , { L_, "n_string_backslash_00.json",
              "[\"\\\000\"]",
              false }
          , { L_, "n_string_escape_x.json",
              "[\"\\x00\"]",
              false }
          , { L_, "n_string_escaped_backslash_bad.json",
              "[\"\\\\\\\"]",
              false }
          , { L_, "n_string_escaped_ctrl_char_tab.json",
              "[\"\\\t\"]",
              false }
          , { L_, "n_string_escaped_emoji.json",
              "[\"\\\360\237\214\200\"]",
              false }
          , { L_, "n_string_incomplete_escape.json",
              "[\"\\\"]",
              false }
          , { L_, "n_string_incomplete_escaped_character.json",
              "[\"\\u00A\"]",
              false }
          , { L_, "n_string_incomplete_surrogate.json",
              "[\"\\uD834\\uDd\"]",
              false }
          , { L_, "n_string_incomplete_surrogate_escape_invalid.json",
              "[\"\\uD800\\uD800\\x\"]",
              false }
          , { L_, "n_string_invalid-utf-8-in-escape.json",
              "[\"\\u\345\"]",
              false }
          , { L_, "n_string_invalid_backslash_esc.json",
              "[\"\\a\"]",
              false }
          , { L_, "n_string_invalid_unicode_escape.json",
              "[\"\\uqqqq\"]",
              false }
          , { L_, "n_string_invalid_utf8_after_escape.json",
              "[\"\\\345\"]",
              false }
          , { L_, "n_string_leading_uescaped_thinspace.json",
              "[\\u0020\"asd\"]",
              false }
          , { L_, "n_string_no_quotes_with_bad_escape.json",
              "[\\n]",
              false }
          , { L_, "n_string_single_doublequote.json",
              "\"",
              false }
          , { L_, "n_string_single_quote.json",
              "[\047single quote\047]",
              false }
          , { L_, "n_string_single_string_no_double_quotes.json",
              "abc",
              false }
          , { L_, "n_string_start_escape_unclosed.json",
              "[\"\\",
              false }
          , { L_, "n_string_unescaped_crtl_char.json",
              "[\"a\000a\"]",
              false }
          , { L_, "n_string_unescaped_newline.json",
              "[\"new\n"
        "line\"]",
              false }
          , { L_, "n_string_unescaped_tab.json",
              "[\"\t\"]",
              false }
          , { L_, "n_string_unicode_CapitalU.json",
              "\"\\UA66D\"",
              false }
          , { L_, "n_string_with_trailing_garbage.json",
              "\"\"x",
              false }
          , { L_, "n_structure_angle_bracket_..json",
              "<.\076",
              false }
          , { L_, "n_structure_angle_bracket_null.json",
              "[<null\076]",
              false }
          , { L_, "n_structure_array_trailing_garbage.json",
              "[1]x",
              false }
          , { L_, "n_structure_array_with_extra_array_close.json",
              "[1]]",
              false }
          , { L_, "n_structure_array_with_unclosed_string.json",
              "[\"asd]",
              false }
          , { L_, "n_structure_ascii-unicode-identifier.json",
              "a\303\245",
              false }
          , { L_, "n_structure_capitalized_True.json",
              "[True]",
              false }
          , { L_, "n_structure_close_unopened_array.json",
              "1]",
              false }
          , { L_, "n_structure_comma_instead_of_closing_brace.json",
              "{\"x\": true,",
              false }
          , { L_, "n_structure_double_array.json",
              "[][]",
              false }
          , { L_, "n_structure_end_array.json",
              "]",
              false }
          , { L_, "n_structure_incomplete_UTF8_BOM.json",
              "\357\273{}",
              false }
          , { L_, "n_structure_lone-invalid-utf-8.json",
              "\345",
              false }
          , { L_, "n_structure_lone-open-bracket.json",
              "[",
              false }
          , { L_, "n_structure_no_data.json",
              "",
              false }
          , { L_, "n_structure_null-byte-outside-string.json",
              "[\000]",
              false }
          , { L_, "n_structure_number_with_trailing_garbage.json",
              "2@",
              false }
          , { L_, "n_structure_object_followed_by_closing_object.json",
              "{}}",
              false }
          , { L_, "n_structure_object_unclosed_no_value.json",
              "{\"\":",
              false }
          , { L_, "n_structure_object_with_comment.json",
              "{\"a\":/*comment*/\"b\"}",
              false }
          , { L_, "n_structure_object_with_trailing_garbage.json",
              "{\"a\": true} \"x\"",
              false }
          , { L_, "n_structure_open_array_apostrophe.json",
              "[\047",
              false }
          , { L_, "n_structure_open_array_comma.json",
              "[,",
              false }
          , { L_, "n_structure_open_array_open_object.json",
              "[{",
              false }
          , { L_, "n_structure_open_array_open_string.json",
              "[\"a",
              false }
          , { L_, "n_structure_open_array_string.json",
              "[\"a\"",
              false }
          , { L_, "n_structure_open_object.json",
              "{",
              false }
          , { L_, "n_structure_open_object_close_array.json",
              "{]",
              false }
          , { L_, "n_structure_open_object_comma.json",
              "{,",
              false }
          , { L_, "n_structure_open_object_open_array.json",
              "{[",
              false }
          , { L_, "n_structure_open_object_open_string.json",
              "{\"a",
              false }
          , { L_, "n_structure_open_object_string_with_apostrophes.json",
              "{\047a\047",
              false }
          , { L_, "n_structure_open_open.json",
              "[\"\\{[\"\\{[\"\\{[\"\\{",
              false }
          , { L_, "n_structure_single_eacute.json",
              "\351",
              false }
          , { L_, "n_structure_single_star.json",
              "*",
              false }
          , { L_, "n_structure_trailing_#.json",
              "{\"a\":\"b\"}#{}",
              false }
          , { L_, "n_structure_U+2060_word_joined.json",
              "[\342\201\240]",
              false }
          , { L_, "n_structure_uescaped_LF_before_string.json",
              "[\\u000A\"\"]",
              false }
          , { L_, "n_structure_unclosed_array.json",
              "[1",
              false }
          , { L_, "n_structure_unclosed_array_partial_null.json",
              "[ false, nul",
              false }
          , { L_, "n_structure_unclosed_array_unfinished_false.json",
              "[ true, fals",
              false }
          , { L_, "n_structure_unclosed_array_unfinished_true.json",
              "[ false, tru",
              false }
          , { L_, "n_structure_unclosed_object.json",
              "{\"asd\":\"asd\"",
              false }
          , { L_, "n_structure_unicode-identifier.json",
              "\303\245",
              false }
          , { L_, "n_structure_UTF8_BOM_no_data.json",
              "\357\273\277",
              false }
          , { L_, "n_structure_whitespace_formfeed.json",
              "[\014]",
              false }
          , { L_, "n_structure_whitespace_U+2060_word_joiner.json",
              "[\342\201\240]",
              false }
          , { L_, "y_array_arraysWithSpaces.json",
              "[[]   ]",
              true }
          , { L_, "y_array_empty-string.json",
              "[\"\"]",
              true }
          , { L_, "y_array_empty.json",
              "[]",
              true }
          , { L_, "y_array_ending_with_newline.json",
              "[\"a\"]\n",
              true }
          , { L_, "y_array_false.json",
              "[false]",
              true }
          , { L_, "y_array_heterogeneous.json",
              "[null, 1, \"1\", {}]",
              true }
          , { L_, "y_array_null.json",
              "[null]",
              true }
          , { L_, "y_array_with_1_and_newline.json",
              "[1\n"
        "]",
              true }
          , { L_, "y_array_with_leading_space.json",
              " [1]",
              true }
          , { L_, "y_array_with_several_null.json",
              "[1,null,null,null,2]",
              true }
          , { L_, "y_array_with_trailing_space.json",
              "[2] ",
              true }
          , { L_, "y_number.json",
              "[123e65]",
              true }
          , { L_, "y_number_0e+1.json",
              "[0e+1]",
              true }
          , { L_, "y_number_0e1.json",
              "[0e1]",
              true }
          , { L_, "y_number_after_space.json",
              "[ 4]",
              true }
          , { L_, "y_number_double_close_to_zero.json",
              "[-0.00000000000000000000000000"
        "000000000000000000000000000000"
        "0000000000000000000001]",
              true }
          , { L_, "y_number_int_with_exp.json",
              "[20e1]",
              true }
          , { L_, "y_number_minus_zero.json",
              "[-0]",
              true }
          , { L_, "y_number_negative_int.json",
              "[-123]",
              true }
          , { L_, "y_number_negative_one.json",
              "[-1]",
              true }
          , { L_, "y_number_negative_zero.json",
              "[-0]",
              true }
          , { L_, "y_number_real_capital_e.json",
              "[1E22]",
              true }
          , { L_, "y_number_real_capital_e_neg_exp.json",
              "[1E-2]",
              true }
          , { L_, "y_number_real_capital_e_pos_exp.json",
              "[1E+2]",
              true }
          , { L_, "y_number_real_exponent.json",
              "[123e45]",
              true }
          , { L_, "y_number_real_fraction_exponent.json",
              "[123.456e78]",
              true }
          , { L_, "y_number_real_neg_exp.json",
              "[1e-2]",
              true }
          , { L_, "y_number_real_pos_exponent.json",
              "[1e+2]",
              true }
          , { L_, "y_number_simple_int.json",
              "[123]",
              true }
          , { L_, "y_number_simple_real.json",
              "[123.456789]",
              true }
          , { L_, "y_object.json",
              "{\"asd\":\"sdf\", \"dfg\":\"fgh\"}",
              true }
          , { L_, "y_object_basic.json",
              "{\"asd\":\"sdf\"}",
              true }
          , { L_, "y_object_duplicated_key.json",
              "{\"a\":\"b\",\"a\":\"c\"}",
              true }
          , { L_, "y_object_duplicated_key_and_value.json",
              "{\"a\":\"b\",\"a\":\"b\"}",
              true }
          , { L_, "y_object_empty.json",
              "{}",
              true }
          , { L_, "y_object_empty_key.json",
              "{\"\":0}",
              true }
          , { L_, "y_object_escaped_null_in_key.json",
              "{\"foo\\u0000bar\": 42}",
              true }
          , { L_, "y_object_extreme_numbers.json",
              "{ \"min\": -1.0e+28, \"max\": 1.0e"
        "+28 }",
              true }
          , { L_, "y_object_long_strings.json",
              "{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxx\"}], \""
        "id\": \"xxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxx\"}",
              true }
          , { L_, "y_object_simple.json",
              "{\"a\":[]}",
              true }
          , { L_, "y_object_string_unicode.json",
              "{\"title\":\"\\u041f\\u043e\\u043b\\u"
        "0442\\u043e\\u0440\\u0430 \\u0417\\"
        "u0435\\u043c\\u043b\\u0435\\u043a\\"
        "u043e\\u043f\\u0430\" }",
              true }
          , { L_, "y_object_with_newlines.json",
              "{\n"
        "\"a\": \"b\"\n"
        "}",
              true }
          , { L_, "y_string_1_2_3_bytes_UTF-8_sequences.json",
              "[\"\\u0060\\u012a\\u12AB\"]",
              true }
          , { L_, "y_string_accepted_surrogate_pair.json",
              "[\"\\uD801\\udc37\"]",
              true }
          , { L_, "y_string_accepted_surrogate_pairs.json",
              "[\"\\ud83d\\ude39\\ud83d\\udc8d\"]",
              true }
          , { L_, "y_string_allowed_escapes.json",
              "[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]",
              true }
          , { L_, "y_string_backslash_and_u_escaped_zero.json",
              "[\"\\\\u0000\"]",
              true }
          , { L_, "y_string_backslash_doublequotes.json",
              "[\"\\\"\"]",
              true }
          , { L_, "y_string_comments.json",
              "[\"a/*b*/c/*d//e\"]",
              true }
          , { L_, "y_string_double_escape_a.json",
              "[\"\\\\a\"]",
              true }
          , { L_, "y_string_double_escape_n.json",
              "[\"\\\\n\"]",
              true }
          , { L_, "y_string_escaped_control_character.json",
              "[\"\\u0012\"]",
              true }
          , { L_, "y_string_escaped_noncharacter.json",
              "[\"\\uFFFF\"]",
              true }
          , { L_, "y_string_in_array.json",
              "[\"asd\"]",
              true }
          , { L_, "y_string_in_array_with_leading_space.json",
              "[ \"asd\"]",
              true }
          , { L_, "y_string_last_surrogates_1_and_2.json",
              "[\"\\uDBFF\\uDFFF\"]",
              true }
          , { L_, "y_string_nbsp_uescaped.json",
              "[\"new\\u00A0line\"]",
              true }
          , { L_, "y_string_nonCharacterInUTF-8_U+10FFFF.json",
              "[\"\364\217\277\277\"]",
              true }
          , { L_, "y_string_nonCharacterInUTF-8_U+FFFF.json",
              "[\"\357\277\277\"]",
              true }
          , { L_, "y_string_null_escape.json",
              "[\"\\u0000\"]",
              true }
          , { L_, "y_string_one-byte-utf-8.json",
              "[\"\\u002c\"]",
              true }
          , { L_, "y_string_pi.json",
              "[\"\317\200\"]",
              true }
          , { L_, "y_string_reservedCharacterInUTF-8_U+1BFFF.json",
              "[\"\360\233\277\277\"]",
              true }
          , { L_, "y_string_simple_ascii.json",
              "[\"asd \"]",
              true }
          , { L_, "y_string_space.json",
              "\" \"",
              true }
          , { L_, "y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
              "[\"\\uD834\\uDd1e\"]",
              true }
          , { L_, "y_string_three-byte-utf-8.json",
              "[\"\\u0821\"]",
              true }
          , { L_, "y_string_two-byte-utf-8.json",
              "[\"\\u0123\"]",
              true }
          , { L_, "y_string_u+2028_line_sep.json",
              "[\"\342\200\250\"]",
              true }
          , { L_, "y_string_u+2029_par_sep.json",
              "[\"\342\200\251\"]",
              true }
          , { L_, "y_string_uEscape.json",
              "[\"\\u0061\\u30af\\u30EA\\u30b9\"]",
              true }
          , { L_, "y_string_uescaped_newline.json",
              "[\"new\\u000Aline\"]",
              true }
          , { L_, "y_string_unescaped_char_delete.json",
              "[\"\177\"]",
              true }
          , { L_, "y_string_unicode.json",
              "[\"\\uA66D\"]",
              true }
          , { L_, "y_string_unicode_2.json",
              "[\"\342\215\202\343\210\264\342\215\202\"]",
              true }
          , { L_, "y_string_unicode_escaped_double_quote.json",
              "[\"\\u0022\"]",
              true }
          , { L_, "y_string_unicode_U+10FFFE_nonchar.json",
              "[\"\\uDBFF\\uDFFE\"]",
              true }
          , { L_, "y_string_unicode_U+1FFFE_nonchar.json",
              "[\"\\uD83F\\uDFFE\"]",
              true }
          , { L_, "y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
              "[\"\\u200B\"]",
              true }
          , { L_, "y_string_unicode_U+2064_invisible_plus.json",
              "[\"\\u2064\"]",
              true }
          , { L_, "y_string_unicode_U+FDD0_nonchar.json",
              "[\"\\uFDD0\"]",
              true }
          , { L_, "y_string_unicode_U+FFFE_nonchar.json",
              "[\"\\uFFFE\"]",
              true }
          , { L_, "y_string_unicodeEscapedBackslash.json",
              "[\"\\u005C\"]",
              true }
          , { L_, "y_string_utf8.json",
              "[\"\342\202\254\360\235\204\236\"]",
              true }
          , { L_, "y_string_with_del_character.json",
              "[\"a\177a\"]",
              true }
          , { L_, "y_structure_lonely_false.json",
              "false",
              true }
          , { L_, "y_structure_lonely_int.json",
              "42",
              true }
          , { L_, "y_structure_lonely_negative_real.json",
              "-0.1",
              true }
          , { L_, "y_structure_lonely_null.json",
              "null",
              true }
          , { L_, "y_structure_lonely_string.json",
              "\"asd\"",
              true }
          , { L_, "y_structure_lonely_true.json",
              "true",
              true }
          , { L_, "y_structure_string_empty.json",
              "\"\"",
              true }
          , { L_, "y_structure_trailing_newline.json",
              "[\"a\"]\n",
              true }
          , { L_, "y_structure_true_in_array.json",
              "[true]",
              true }
          , { L_, "y_structure_whitespace_array.json",
              " [] ",
              true }

          // Additional test case, not actually from test suite.
          , { L_, "y_henry_verschell_smiley_surrogate_smiley.json",
              "\"\xF0\x9F\x98\x80"
              "\\ud83d\\ude00\"",
              true }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct TC::FailureListData failureList[] = {
            { L_, "n_array_comma_after_close.json", "TBD" }
          , { L_, "n_array_double_comma.json", "TBD" }
          , { L_, "n_multidigit_number_then_00.json", "TBD" }
          , { L_, "n_number_+1.json", "TBD" }
          , { L_, "n_number_+Inf.json", "TBD" }
          , { L_, "n_number_-01.json", "TBD" }
          , { L_, "n_number_-2..json", "TBD" }
          , { L_, "n_number_-NaN.json", "TBD" }
          , { L_, "n_number_.2e-3.json", "TBD" }
          , { L_, "n_number_0.e1.json", "TBD" }
          , { L_, "n_number_2.e+3.json", "TBD" }
          , { L_, "n_number_2.e-3.json", "TBD" }
          , { L_, "n_number_2.e3.json", "TBD" }
          // Skip these tests everywhere except Sun CC or MSVC pre-2015, where
          // they unexpectedly succeed.  Since these are negative tests, being
          // overly permissive in what we accept is not a problem.
#if !((defined(BSLS_PLATFORM_OS_SOLARIS) && defined(BSLS_PLATFORM_CMP_SUN))   \
      || (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900))
          , { L_, "n_number_hex_1_digit.json", "TBD" }
          , { L_, "n_number_hex_2_digits.json", "TBD" }
#endif
          , { L_, "n_number_Inf.json", "TBD" }
          , { L_, "n_number_infinity.json", "TBD" }
          , { L_, "n_number_minus_infinity.json", "TBD" }
          , { L_, "n_number_NaN.json", "TBD" }
          , { L_, "n_number_neg_int_starting_with_zero.json", "TBD" }
          , { L_, "n_number_neg_real_without_int_part.json", "TBD" }
          , { L_, "n_number_real_without_fractional_part.json", "TBD" }
          , { L_, "n_number_starting_with_dot.json", "TBD" }
          , { L_, "n_number_with_leading_zero.json", "TBD" }
          , { L_, "n_object_double_colon.json", "TBD" }
          , { L_, "n_object_trailing_comment.json", "TBD" }
          , { L_, "n_object_trailing_comment_open.json", "TBD" }
          , { L_, "n_object_trailing_comment_slash_open.json", "TBD" }
          , { L_, "n_object_trailing_comment_slash_open_incomplete.json",
              "TBD" }
          , { L_, "n_object_two_commas_in_a_row.json", "TBD" }
          , { L_, "n_object_with_trailing_garbage.json", "TBD" }
          , { L_, "n_string_unescaped_newline.json", "TBD" }
          , { L_, "n_string_unescaped_tab.json", "TBD" }
          , { L_, "n_string_unicode_CapitalU.json", "TBD" }
          , { L_, "n_string_with_trailing_garbage.json", "TBD" }
          , { L_, "n_structure_array_trailing_garbage.json", "TBD" }
          , { L_, "n_structure_double_array.json", "TBD" }
          , { L_, "n_structure_object_with_trailing_garbage.json", "TBD" }
          , { L_, "n_structure_trailing_#.json", "TBD" }
          , { L_, "n_structure_whitespace_formfeed.json", "TBD" }
        };
        enum { k_NUM_FAILURE_LIST = sizeof(failureList) /
                                                        sizeof(*failureList) };

        TC::testCase07<bsl::string>(DATA,
                                    k_NUM_DATA,
                                    failureList,
                                    k_NUM_FAILURE_LIST);
        TC::testCase07<std::string>(DATA,
                                    k_NUM_DATA,
                                    failureList,
                                    k_NUM_FAILURE_LIST);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        TC::testCase07<std::pmr::string>(DATA,
                                         k_NUM_DATA,
                                         failureList,
                                         k_NUM_FAILURE_LIST);
#endif
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

        if (verbose) cout << "\nENCODE AND PRINT TEST"
                          << "\n=====================" << endl;

        namespace TC = TestCase06;

        bsls::AlignedBuffer<8 * 1024>      buffer;
        bdlma::BufferedSequentialAllocator bsa(buffer.buffer(),
                                               sizeof(buffer));

        bdld::DatumMaker m(&bsa);

#define PR   baljsn::EncodingStyle::e_PRETTY
#define CO   baljsn::EncodingStyle::e_COMPACT
#define L   L_

        static const TC::TestCase6Data  DATA[] = {
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

#define DU_Inf  bsl::numeric_limits<double>::infinity()
#define DU_QNaN bsl::numeric_limits<double>::quiet_NaN()
#define DU_SNaN bsl::numeric_limits<double>::signaling_NaN()

            { L, m(DU_Inf), CO,  0,  0,  "\"+inf\"",                 0,    2 },
            { L, m(DU_Inf), PR,  0,  0,  "\"+inf\"",                 0,    2 },
            { L, m(DU_Inf), PR,  1,  0,  "\"+inf\"",                 0,    2 },
            { L, m(DU_Inf), PR,  0,  1,  "\"+inf\"",                 0,    2 },
            { L, m(DU_Inf), PR,  1,  1,  " \"+inf\"",                0,    2 },

            { L, m(-DU_Inf),
                            CO,  0,  0,  "\"-inf\"",                 0,    2 },
            { L, m(-DU_Inf),
                            PR,  0,  0,  "\"-inf\"",                 0,    2 },
            { L, m(-DU_Inf),
                            PR,  1,  0,  "\"-inf\"",                 0,    2 },
            { L, m(-DU_Inf),
                            PR,  0,  1,  "\"-inf\"",                 0,    2 },
            { L, m(-DU_Inf),
                            PR,  1,  1,  " \"-inf\"",                0,    2 },

            { L, m(DU_QNaN),
                            CO,  0,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_QNaN),
                            PR,  0,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_QNaN),
                            PR,  1,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_QNaN),
                            PR,  0,  1,  "\"nan\"",                  0,    2 },
            { L, m(DU_QNaN),
                            PR,  1,  1,  " \"nan\"",                 0,    2 },

            { L, m(-DU_QNaN),
                            CO,  0,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_QNaN),
                            PR,  0,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_QNaN),
                            PR,  1,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_QNaN),
                            PR,  0,  1,  "\"nan\"",          0,    2 },
            { L, m(-DU_QNaN),
                            PR,  1,  1,  " \"nan\"",         0,    2 },


            { L, m(DU_SNaN),
                            CO,  0,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_SNaN),
                            PR,  0,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_SNaN),
                            PR,  1,  0,  "\"nan\"",                  0,    2 },
            { L, m(DU_SNaN),
                            PR,  0,  1,  "\"nan\"",                  0,    2 },
            { L, m(DU_SNaN),
                            PR,  1,  1,  " \"nan\"",                 0,    2 },

            { L, m(-DU_SNaN),
                            CO,  0,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_SNaN),
                            PR,  0,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_SNaN),
                            PR,  1,  0,  "\"nan\"",          0,    2 },
            { L, m(-DU_SNaN),
                            PR,  0,  1,  "\"nan\"",          0,    2 },
            { L, m(-DU_SNaN),
                            PR,  1,  1,  " \"nan\"",         0,    2 },

#undef DU_Inf
#undef DU_QNan
#undef DU_SNan

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
                            CO,  0,  0,  "8e+09",                   0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  0,  0,  "8e+09",                   0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  1,  0,  "8e+09",                   0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  0,  1,  "8e+09",                   0,    1 },
            { L, m(bsls::Types::Int64(8000000000LL)),
                            PR,  1,  1,  " 8e+09",                  0,    1 },

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
        enum { k_NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        TC::testCase06<bsl::string>(     DATA, k_NUM_DATA);
        TC::testCase06<std::string>(     DATA, k_NUM_DATA);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        TC::testCase06<std::pmr::string>(DATA, k_NUM_DATA);
#endif
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
        //:   to make sure errors occur if 'maxNestedDepth' is insufficient and
        //:   the string is otherwise valid.
        //
        // Testing:
        //  int decode(MgedDatum*, const string_view&);
        //  int decode(MgedDatum*, const string_view&, const DDOptions&);
        //  int decode(MgedDatum*, os*, const string_view&);
        //  int decode(MgedDatum*, os*, const string_view&, const DDOptions&);
        //  int decode(MgedDatum*, streamBuf*);
        //  int decode(MgedDatum*, streamBuf*, const DDOptions&);
        //  int decode(MgedDatum*, ostream*, streamBuf*);
        //  int decode(MgedDatum*, ostream*, streamBuf*, const DDOptions&);
        //---------------------------------------------------------------------

        if (verbose) cout << "\nDECODE TEST"
                          << "\n===========" << endl;

        bsls::AlignedBuffer<8 * 1024>      buffer;
        bdlma::BufferedSequentialAllocator bsa(buffer.buffer(),
                                               sizeof(buffer));

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
            // null value tests.
            { L_, "",                     -1,     0, m()                     },
            { L_, "n",                    -2,     0, m()                     },
            { L_, "nu",                   -2,     0, m()                     },
            { L_, "nul",                  -2,     0, m()                     },
            { L_, "null",                  0,     0, m()                     },
            { L_, WS "null",               0,     0, m()                     },
            { L_, "null" WS,               0,     0, m()                     },
            // Numeric value tests.
            { L_, "1",                     0,     0, m(1.0)                  },
            { L_, WS "1",                  0,     0, m(1.0)                  },
            { L_, "1" WS,                  0,     0, m(1.0)                  },
            { L_, "2.5",                   0,     0, m(2.5)                  },
            // String value tests.
            { L_, "\"hello\"",             0,     0, m("hello")              },
            { L_, WS "\"hello\"",          0,     0, m("hello")              },
            { L_, WS "\"hello\"" WS,       0,     0, m("hello")              },
            { L_, "\"hello",              -1,     0, m()                     },
            // Check handling of upper surrogate without following lower.
            { L_, "\"\\ud83d\"",          -2,     0, m()                     },
            { L_, "\"\\ud83d\\ud83d\"",   -2,     0, m()                     },
            { L_, "\"\\ud83da\"",         -2,     0, m()                     },
            { L_, "\"\\ud83d\n\"",        -2,     0, m()                     },
            // Check handling of upper surrogate followed by all possible
            // escaped characters
            { L_, "\"\\ud83d\\\"\"",      -2,     0, m()                     },
            { L_, "\"\\ud83d\\\\\"",      -2,     0, m()                     },
            { L_, "\"\\ud83d\\/\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\b\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\f\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\n\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\r\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\t\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\u\"",       -2,     0, m()                     },
            { L_, "\"\\ud83d\\u0001\"",   -2,     0, m()                     },
            // Check handling of lower surrogate without preceding upper.
            { L_, "\"\\ude00\"",          -2,     0, m()                     },
            { L_, "\"\\u0001\\ude00\"",   -2,     0, m()                     },
            // Check handling of valid surrogate pair (smiley).
            { L_, "\"\\ud83d\\ude00\"",    0,     0, m("\xF0\x9F\x98\x80")   },
            // Test bool values, valid or not.
            { L_, "t",                    -2,     0, m()                     },
            { L_, "tr",                   -2,     0, m()                     },
            { L_, "tru",                  -2,     0, m()                     },
            { L_, "treu",                 -2,     0, m()                     },
            { L_, "true",                  0,     0, m(true)                 },
            { L_, WS "true",               0,     0, m(true)                 },
            { L_, "true" WS,               0,     0, m(true)                 },
            { L_, "false",                 0,     0, m(false)                },
            { L_, "fals",                 -2,     0, m()                     },
            // Array/Object tests
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

            // test character escape sequences in keys and values
            { L_, "{\"a\\/b\":\"c\\/d\"}",
                                           0,     1, m.m("a/b", "c/d")},
            { L_, "{\"a\\bb\":\"c\\bd\"}",
                                           0,     1, m.m("a\bb", "c\bd")},
            { L_, "{\"a\\fb\":\"c\\fd\"}",
                                           0,     1, m.m("a\fb", "c\fd")},
            { L_, "{\"a\\nb\":\"c\\nd\"}",
                                           0,     1, m.m("a\nb", "c\nd")},
            { L_, "{\"a\\rb\":\"c\\rd\"}",
                                           0,     1, m.m("a\rb", "c\rd")},
            { L_, "{\"a\\tb\":\"c\\td\"}",
                                           0,     1, m.m("a\tb", "c\td")},
            { L_, "{\"a\\\\b\":\"c\\\\d\"}",
                                           0,     1, m.m("a\\b", "c\\d")},
            { L_, "{\"a\\u0020b\":\"c\\u0020d\"}",
                                           0,     1, m.m("a b", "c d")},

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
        //   int decode(MgedDatum*, const string_view&);
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
        //   int decode(MgedDatum*, const string_view&, const DDOptions&);
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
        //   int decode(MgedDatum*, os*, const string_view&);
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
        //   int decode(MgedDatum*, os*, const string_view&, const DDOptions&);
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

        if (verbose)cout << "\nBREATHING ROUND-TRIP TEST"
                         << "\n=========================" << endl;

        testCase04<bsl::string>();
        testCase04<std::string>();
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        testCase04<std::pmr::string>();
#endif
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

        if (verbose) cout  << "\nBREATHING ENCODE TEST"
                           << "\n=====================" << endl;

        testCase03<bsl::string>();
        testCase03<std::string>();
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        testCase03<std::pmr::string>();
#endif
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
        //: 1 Construct strings representing the various 'Datum' value types
        //:   and make sure they're parsed correctly.
        //
        // Testing:
        //   BREATHING DECODE TEST
        //---------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING DECODE TEST"
                          << "\n=====================" << endl;

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

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        testCase01<bsl::string>();
        testCase01<std::string>();
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        testCase01<std::pmr::string>();
#endif
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
