// ball_recordformatterregistryutil.t.cpp                             -*-C++-*-
#include <ball_recordformatterregistryutil.h>

#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_recordformatterfunctor.h>
#include <ball_recordformatteroptions.h>
#include <ball_recordformattertimezone.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_typeinfo.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a utility for creating log record
// formatters based on URI-like scheme identifiers.  The primary function
// interprets format strings with schemes ("text://", "json://", "qjson://")
// and delegates to appropriate formatter factories.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] int createRecordFormatter(RecordFormatter *, string_view, options)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
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

typedef ball::RecordFormatterRegistryUtil Util;

//=============================================================================
//                       GLOBAL HELPERS OF TESTING
//-----------------------------------------------------------------------------

ball::Record createTestRecord()
    // Create and return a simple test record for formatter testing.
{
    ball::RecordAttributes attributes;
    attributes.setMessage("Test message");
    attributes.setFileName("test.cpp");
    attributes.setLineNumber(42);
    attributes.setCategory("TEST.CATEGORY");
    attributes.setSeverity(ball::Severity::e_WARN);
    attributes.setThreadID(12345);

    ball::Record record(attributes, ball::UserFields());
    return record;
}

/// This `FakeFormatter` functor is used only for its `typeid` to determine if
/// `createRecordFormatter` had overwritten the provided formatter output with
/// a newly configured, or a fallback formatter.
struct FakeFormatter {
    // CREATORS
    FakeFormatter(int line) : d_line(line) {}

    // ACCESSORS
    void operator()(bsl::ostream& os, const ball::Record&) const
    {
        os << d_line << endl;
    }

  private:
    // DATA
    int d_line;
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Example 1: Creating Formatters from Various Scheme URIs
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have configuration entries that define log record formatters
// using URI-like schemes, and we need to create the corresponding formatters.
// This component supports "text://", "json://", and "qjson://" schemes.
//
// First, we create formatter options and a test record:
// ```
  ball::RecordFormatterOptions options(
                                   ball::RecordFormatterTimezone::e_UTC);
  ball::Record record = createTestRecord();
// ```
// Now, we demonstrate creating formatters with different schemes.  For text
// output, we use the "text://" scheme with printf-style format specifiers:
// ```
  bsl::string textFormat = "text://%d %p:%t %s %f:%l %c %m\n";
  ball::RecordFormatterFunctor::Type textFormatter;
  int rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
                                                            &textFormatter,
                                                            textFormat,
                                                            options);
  ASSERT(0 == rc);
// ```
// For structured JSON output, we use the "json://" scheme with a JSON array
// specifying which fields to include:
// ```
  bsl::string jsonFormat = "json://[\"tid\",\"severity\",\"message\"]";
  ball::RecordFormatterFunctor::Type jsonFormatter;
  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
                                                            &jsonFormatter,
                                                            jsonFormat,
                                                            options);
  ASSERT(0 == rc);
// ```
// For simplified JSON using printf-style specifiers, we use the "qjson://"
// scheme:
// ```
  bsl::string qjsonFormat = "qjson://%i %s %c %m";
  ball::RecordFormatterFunctor::Type qjsonFormatter;
  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
                                                           &qjsonFormatter,
                                                           qjsonFormat,
                                                           options);
  ASSERT(0 == rc);
// ```
// Each formatter can then be used to format log records:
// ```
  bsl::ostringstream textOss, jsonOss, qjsonOss;
  textFormatter(textOss, record);
  jsonFormatter(jsonOss, record);
  qjsonFormatter(qjsonOss, record);

  ASSERT(textOss.str().find("Test message")  != bsl::string::npos);
  ASSERT(jsonOss.str().find("\"message\"")   != bsl::string::npos);
  ASSERT(qjsonOss.str().find("Test message") != bsl::string::npos);
// ```
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING createRecordFormatter
        //
        // Concerns:
        // 1. `createRecordFormatter` returns 0 for supported schemes
        //    ("text", "json", "qjson").
        //
        // 2. `createRecordFormatter` returns a non-zero error code for
        //    unsupported schemes.
        //
        // 3. Even with an unsupported scheme, the result formatter is loaded
        //    with a fallback formatter that is usable.
        //
        // 4. The method correctly parses scheme prefixes and extracts the
        //    format specification.
        //
        // 5. Created formatters are functional and can format records to
        //    output streams.
        //
        // 6. When an invalid format is provided, the method does not overwrite
        //    an existing non-empty formatter.
        //
        // Plan:
        // 1. Create a table of test data with various format strings and
        //    expected return codes.
        //
        // 2. For each test case, call `createRecordFormatter` and verify:
        //    a. The return code matches expectations.
        //    b. The formatter is callable.
        //    c. The formatter produces non-empty output.
        //
        // 3. Test supported schemes: "text", "json", "qjson".
        //
        // 4. Test supported schemes with broken format specifications and
        //    verify fallback behavior.  (Note that "text" scheme cannot be
        //    invalid as all format strings are valid for it.)
        //
        // 5. Test unsupported schemes and verify fallback behavior.
        //
        // Testing:
        //   int createRecordFormatter(RecordFormatter *, string_view, options)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING createRecordFormatter\n"
                             "=============================\n";

        ball::RecordFormatterOptions options(
                                         ball::RecordFormatterTimezone::e_UTC);

        struct TestData {
            int              d_line;
            const char      *d_format;
            bool             d_valid;  // true if we expect rc == 0
            const char      *d_description;
            bsl::string_view d_outputFormat;
        } DATA[] = {
            //LINE  FORMAT STRING                                SUCCESS
            //----  -------------------------------------------  -------
            //      DESCRIPTION                                  RESULT SCHEME
            //      -----------------------                      -------------
            { L_,   "%d %m\n",                                   true,
                    "default text scheme",                       "text"      },
            { L_,   "text://%d %m\n",                            true,
                    "text scheme",                               "text"      },
            { L_,   "text://%d %p:%t %s %m\n",                   true,
                    "text detailed",                             "text"      },
            // `text://` cannot have a bad format, all are valid
            { L_,   "json://[\"timestamp\",\"message\"]",        true,
                    "json scheme",                               "json"      },
            { L_,   "json://[\"tid\",\"severity\",\"message\"]", true,
                    "json detailed",                             "json"      },
            { L_,   "json://verynotgood",                        false,
                    "json bad format",                           "json"      },
            { L_,   "qjson://%d %m",                             true,
                    "qjson scheme",                              "json"      },
            { L_,   "qjson://%i %s %c %m",                       true,
                    "qjson detailed",                            "json"      },
            { L_,   "qjson://%42 %-",                            false,
                    "qjson scheme",                              "json"      },
            { L_,   "xml://some-format",                         false,
                    "unknown scheme",                            "text"      },
            { L_,   "yaml://config",                             false,
                    "unsupported",                               "text"      },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE       = DATA[ti].d_line;
            const char              *FORMAT     = DATA[ti].d_format;
            const bool               VALID      = DATA[ti].d_valid;
            const char              *DESC       = DATA[ti].d_description;
            const bsl::string_view&  OUT_FORMAT = DATA[ti].d_outputFormat;

            if (veryVerbose) {
                T_ P_(LINE) P_(FORMAT) P_(VALID) P_(OUT_FORMAT) P(DESC)
            }

            ball::RecordFormatterFunctor::Type formatter;  // empty formatter
            int rc = Util::createRecordFormatter(&formatter, FORMAT, options);

            if (VALID) {
                ASSERTV(LINE, FORMAT, rc, 0 == rc);
            } else {
                ASSERTV(LINE, FORMAT, rc, 0 != rc);
            }

            // Even for unsupported schemes, formatter should be usable
            const ball::Record record = createTestRecord();
            bsl::ostringstream oss;
            formatter(oss, record);
            const bsl::string output = oss.str();
            ASSERTV(LINE, FORMAT, output, !output.empty());

            // Verify output contains test message, both the fallbacks and the
            // valid formatters do.
            ASSERTV(LINE, FORMAT, output,
                    output.find("Test message") != bsl::string::npos);
            if (OUT_FORMAT == "text") {
                ASSERTV(LINE, FORMAT, output,
                        output.find("Test message") != bsl::string::npos);
            } else if (OUT_FORMAT == "json") {
                ASSERTV(LINE, FORMAT, output,
                        output.find("\"message\"") != bsl::string::npos);
                ASSERTV(LINE, FORMAT, output,
                        output.find("{") != bsl::string::npos);
            }

            if (veryVeryVerbose) {
                T_ T_ P(output)
            }

            // Test that the provided non-empty formatter was overwritten only
            // for valid formats, and not overwritten for invalid formats

            formatter = FakeFormatter(LINE);  // non-empty formatter with
                                              // identifiable typeid
            rc = Util::createRecordFormatter(&formatter, FORMAT, options);

            if (VALID) {
                // Valid format should overwrite with a new formatter.
                ASSERTV(formatter.target_type().name(),
                        formatter.target_type() != typeid(FakeFormatter));
            } else {
                // Invalid format should not overwrite, should still be the
                // fake (because it looks like a non-empty formatter-function).
                ASSERTV(formatter.target_type().name(),
                        formatter.target_type() == typeid(FakeFormatter));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class method `createRecordFormatter` can create formatters
        //    for supported schemes.
        //
        // 2. Created formatters are callable and can format records.
        //
        // 3. The component is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create a formatter using the "text://" scheme and verify it
        //    succeeds.
        //
        // 2. Format a test record and verify output is produced.
        //
        // 3. Create a formatter using the "json://" scheme and verify it
        //    succeeds.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        ball::RecordFormatterOptions options(
                                         ball::RecordFormatterTimezone::e_UTC);

        if (verbose) cout << "\t1. Create text formatter.\n";
        {
            ball::RecordFormatterFunctor::Type formatter;
            int rc = Util::createRecordFormatter(
                                            &formatter,
                                            "text://%d %p:%t %s %f:%l %c %m\n",
                                            options);
            ASSERT(0 == rc);

            if (verbose) cout << "\t2. Format a test record.\n";

            const ball::Record record = createTestRecord();
            bsl::ostringstream oss;
            formatter(oss, record);

            const bsl::string output = oss.str();
            ASSERT(!output.empty());
            ASSERT(output.find("WARN")          != bsl::string::npos);
            ASSERT(output.find("TEST.CATEGORY") != bsl::string::npos);
            ASSERT(output.find("Test message")  != bsl::string::npos);

            if (veryVerbose) {
                P(output);
            }
        }

        if (verbose) cout << "\t3. Create json formatter.\n";
        {
            ball::RecordFormatterFunctor::Type jsonFormatter;
            int rc = Util::createRecordFormatter(
                                           &jsonFormatter,
                                           "json://[\"tid\",\"message\"]",
                                           options);
            ASSERT(0 == rc);

            const ball::Record record = createTestRecord();
            bsl::ostringstream oss;
            jsonFormatter(oss, record);

            const bsl::string output = oss.str();
            ASSERT(!output.empty());
            ASSERT(output.find("\"tid\"")      != bsl::string::npos);
            ASSERT(output.find("\"message\"")  != bsl::string::npos);
            ASSERT(output.find("Test message") != bsl::string::npos);

            if (veryVerbose) {
                P(output);
            }
        }

        if (verbose) cout << "\t4. Create qjson formatter.\n";
        {
            ball::RecordFormatterFunctor::Type qjsonFormatter;
            int rc = Util::createRecordFormatter(&qjsonFormatter,
                                                 "qjson://%i %s %m",
                                                 options);
            ASSERT(0 == rc);

            const ball::Record record = createTestRecord();
            bsl::ostringstream oss;
            qjsonFormatter(oss, record);

            const bsl::string output = oss.str();
            ASSERT(!output.empty());
            ASSERT(output.find("WARN")         != bsl::string::npos);
            ASSERT(output.find("Test message") != bsl::string::npos);

            if (veryVerbose) {
                P(output);
            }
        }

        if (verbose) cout << "\t5. Test unknown scheme fallback.\n";
        {
            ball::RecordFormatterFunctor::Type fallbackFormatter;
            const int rc = Util::createRecordFormatter(&fallbackFormatter,
                                                       "unknown://format",
                                                       options);

            ASSERT(0 != rc);  // Should fail for unknown scheme

            // But formatter should still be usable (fallback)
            const ball::Record record = createTestRecord();
            bsl::ostringstream oss;
            fallbackFormatter(oss, record);

            const bsl::string output = oss.str();
            ASSERT(!output.empty());

            if (veryVerbose) {
                P(output);
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

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
