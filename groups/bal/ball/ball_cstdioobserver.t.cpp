// ball_cstdioobserver.t.cpp                                          -*-C++-*-
#include <ball_cstdioobserver.h>

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdls_filesystemutil.h>
#include <bdls_tempdirectoryguard.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bsla_maybeunused.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strlen(), memset(), memcpy(), memcmp()
#include <bsl_cstdio.h>     // fopen, fclose
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

// Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(PASSTHROUGH)
#undef PASSTHROUGH
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under the test is a simple observer that prints the log
// records to a `FILE *` supplied at construction.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] CstdioObserver(FILE *file, allocator)
// [ 2] virtual ~CstdioObserver()
//
// MANIPULATORS
// [ 2] virtual void publish(const shared_ptr<const Record>&, Context&);
// [ 2] virtual void releaseRecords();
// [ 3] void disablePublishInLocalTime();
// [ 3] void enablePublishInLocalTime();
// [ 2] void setFormatFunctor(const RecordFormatFunctor& functor);
// [ 3] int setFormat(const bsl::string_view& format);
//
// ACCESSORS
// [ 3] bool isPublishInLocalTimeEnabled() const;
// [ 3] const bsl::string& getFormat() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::CstdioObserver Obj;
typedef bdls::FilesystemUtil FsUtil;

//=============================================================================
//                  HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bsl::string readPartialFile(bsl::string&   fileName,
                            FsUtil::Offset startOffset)
    // Read the contents of the specified 'fileName' starting from the
    // specified 'startOffset' and return the result as a string.
{
    bsl::string result;
    result.reserve(static_cast<bsl::string::size_type>(
                             FsUtil::getFileSize(fileName) + 1 - startOffset));

    FILE *fp = fopen(fileName.c_str(), "r");
    BSLS_ASSERT_OPT(fp);

    BSLA_MAYBE_UNUSED int rc = fseek(fp,
                                     static_cast<long>(startOffset),
                                     SEEK_SET);
    BSLS_ASSERT_OPT(0 == rc);

    int c;
    while (EOF != (c = getc(fp))) {
        result += static_cast<char>(c);
    }

    fclose(fp);

    return result;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;  (void) veryVerbose;
    const bool veryVeryVerbose     = argc > 4;  (void) veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;  (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        ball::RecordAttributes attributes;
        ball::UserFields       fieldValues;
        ball::Context          context;

        bslma::TestAllocator ga("scratch", veryVeryVeryVerbose);

        const bsl::shared_ptr<const ball::Record>
            record(new (ga) ball::Record(attributes, fieldValues, &ga), &ga);

        ball::CstdioObserver observer(stdout);

        observer.publish(record, context);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'setFormat', 'getFormat', AND TIMEZONE METHODS
        //
        // Concerns:
        // 1. The 'setFormat' method correctly configures the record formatter
        //    based on the format specification string.
        //
        // 2. Various format schemes (text, json, qjson) are supported and
        //    produce the expected output.
        //
        // 3. Invalid format specifications are rejected with a non-zero
        //    return code and the formatter is reset to the default.
        //
        // 4. 'getFormat' returns the format string that was
        //    successfully set, or an empty string if using the default
        //    formatter or after a failed 'setFormat'.
        //
        // 5. 'enablePublishInLocalTime' and 'disablePublishInLocalTime'
        //    control whether timestamps are published in local time or UTC.
        //
        // 6. 'isPublishInLocalTimeEnabled' correctly reports the timezone
        //    state, with the default being UTC (false).
        //
        // Plan:
        // 1. Test 'setFormat' with various valid format specifications and
        //    verify the output matches expectations.
        //
        // 2. Test 'setFormat' with an invalid format and verify it returns
        //    non-zero and resets to the default formatter.
        //
        // 3. Verify 'getFormat' returns empty by default and returns
        //    the format string after successful 'setFormat'.
        //
        // 4. Test 'enablePublishInLocalTime' and 'disablePublishInLocalTime'
        //    and verify 'isPublishInLocalTimeEnabled' reports the correct
        //    state before and after each call.
        //
        // Testing:
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   int setFormat(const bsl::string_view& format);
        //   bool isPublishInLocalTimeEnabled() const;
        //   const bsl::string& getFormat() const;
        // --------------------------------------------------------------------

        if (verbose) cout <<
                    "TESTING 'setFormat', 'getFormat', AND TIMEZONE METHODS\n"
                    "======================================================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Open the temporary file we will be writing into
        bdls::TempDirectoryGuard tempDirGuard("ball_cstdioobserver_");
        bsl::string fileName = tempDirGuard.getTempDirName() + "/test3.log";
        FILE *const file_p = fopen(fileName.c_str(), "w");

        FsUtil::Offset fileOffset = FsUtil::getFileSize(fileName);

        // Test setFormat with datetime format
        if (verbose) cerr << "\ttesting `setFormat` with BDE timeformat.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            const char *format = "%d %p:%t %s %f:%l %c %m";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2023, 11, 15, 14, 30, 45, 123);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1234);
            fixed.setThreadID(5678);
            fixed.setSeverity(ball::Severity::e_WARN);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(42);
            fixed.setCategory("TEST.CATEGORY");
            fixed.setMessage("test setFormat datetime");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->setFixedFields(fixed);

            mX.publish(record,
                       ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));

            const bsl::string& output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            // Verify contains the message
            ASSERT(
                  bsl::string::npos != output.find("test setFormat datetime"));

            // Verify has datetime format (contains space after date)
            ASSERT(bsl::string::npos != output.find(" WARN "));
        }

        // Test setFormat with ISO format
        if (verbose) cerr << "\ttesting `setFormat` with ISO timeformat.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            const char *format = "%i %p:%t %s %f:%l %c %m";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2023, 11, 15, 14, 30, 45, 123);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1234);
            fixed.setThreadID(5678);
            fixed.setSeverity(ball::Severity::e_INFO);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(42);
            fixed.setCategory("TEST.CATEGORY");
            fixed.setMessage("test setFormat ISO");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->setFixedFields(fixed);

            mX.publish(record,
                       ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));

            const bsl::string& output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            // Verify contains the message
            ASSERT(bsl::string::npos != output.find("test setFormat ISO"));

            // Verify has ISO format (contains 'T' separator)
            ASSERT(bsl::string::npos != output.find("T"));
        }

        // Test setFormat with qjson format
        if (verbose) cerr << "\ttesting `setFormat` with qjson.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            const char *format = "qjson://%s %m";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2023, 11, 15, 14, 30, 45, 123);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1234);
            fixed.setThreadID(5678);
            fixed.setSeverity(ball::Severity::e_ERROR);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(42);
            fixed.setCategory("TEST.CATEGORY");
            fixed.setMessage("test setFormat qjson");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->setFixedFields(fixed);

            mX.publish(record,
                       ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));

            const bsl::string& output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            // Verify contains the message
            ASSERT(bsl::string::npos != output.find("test setFormat qjson"));

            // Output should have JSON-like structure
            ASSERT(bsl::string::npos != output.find("\"severity\""));
            ASSERT(bsl::string::npos != output.find("\"message\""));
        }

        // Test setFormat with json format
        if (verbose) cerr << "\ttesting `setFormat` with json.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            const char *format = "json://[\"severity\",\"message\"]";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2023, 11, 15, 14, 30, 45, 123);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1234);
            fixed.setThreadID(5678);
            fixed.setSeverity(ball::Severity::e_WARN);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(42);
            fixed.setCategory("TEST.CATEGORY");
            fixed.setMessage("test setFormat json");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->setFixedFields(fixed);

            mX.publish(record,
                       ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));

            const bsl::string& output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            // Verify contains the message
            ASSERT(bsl::string::npos != output.find("test setFormat json"));

            // Should have JSON object format
            ASSERT(bsl::string::npos != output.find("{"));
            ASSERT(bsl::string::npos != output.find("}"));
            ASSERT(bsl::string::npos != output.find("\"WARN\""));
        }

        // Test invalid format returns non-zero
        if (verbose) cerr << "\ttesting invalid format rejection.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            // First set a valid format
            const char *validFormat = "%d %p:%t %s %f:%l %c %m";
            ASSERT(0 == mX.setFormat(validFormat));
            ASSERT(validFormat == X.getFormat());

            // Test invalid format returns non-zero and resets to default
            ASSERT(0 != mX.setFormat("invalid_scheme://test"));

            // After failed `setFormat` should be unchanged.
            ASSERT(X.getFormat() == validFormat);
        }

        // Test that `getFormat` returns default format by default
        if (verbose) cerr << "\ttesting default getFormat.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            // Should be the default format, it is part of the contract.
            ASSERT(X.getFormat() == "\n%d %p %t %s %f %l %c %m %u\n");
        }

        // Test timezone defaults (enablePublishInLocalTime,
        // disablePublishInLocalTime, isPublishInLocalTimeEnabled)
        if (verbose) cerr << "\ttesting `setFormat` with timezone.\n";
        {
            Obj mX(file_p, &ta);  const Obj& X = mX;

            // Verify default is UTC (local time disabled)
            ASSERT(!X.isPublishInLocalTimeEnabled());

            // Set format and enable local time
            const char *format = "%d %s %m\n";
            ASSERT(0 == mX.setFormat(format));
            mX.enablePublishInLocalTime();

            // Verify local time is now enabled
            ASSERT(X.isPublishInLocalTimeEnabled());

            // Create and publish a record with fixed time
            const bdlt::Datetime testTime(2024, 5, 15, 14, 30, 45, 123);
            ball::RecordAttributes fixed(&ta);
            fixed.setTimestamp(testTime);
            fixed.setProcessID(1);
            fixed.setThreadID(1);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(123);
            fixed.setSeverity(ball::Severity::e_WARN);
            fixed.setCategory("TEST");
            fixed.setMessage("test timezone local");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->setFixedFields(fixed);

            ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);
            mX.publish(record, context);

            bsl::string output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            if (veryVerbose) {
                cout << "Local time output: " << output << endl;
            }

            // Verify output contains date in local time format (exact format
            // depends on timezone, but should contain "15MAY2024")
            ASSERT(output.find("15MAY2024") != bsl::string::npos);

            // Clear stream and switch to UTC
            mX.disablePublishInLocalTime();

            // Verify local time is now disabled
            ASSERT(!X.isPublishInLocalTimeEnabled());

            // Publish another record
            fixed.setMessage("test timezone utc");
            record.createInplace(&ta);
            record->setFixedFields(fixed);
            mX.publish(record, context);

            output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            if (veryVerbose) {
                cout << "UTC time output: " << output << endl;
            }

            // Verify output contains date
            ASSERT(output.find("15MAY2024") != bsl::string::npos);
        }

        fclose(file_p);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        // 1. Log record is correctly formatted.
        //
        // 2. Formatted log record is written to the output stream.
        //
        // 3. `setFormatFunctor` can change the format effectively.
        //
        // Plan:
        // 1. Create the observer object and publish log record.
        //
        // 2. Use `setFormatFunctor` manipulator to affect output format
        //    and verify that it has changed where expected.
        //
        // Testing:
        //   StreamObserver(bsl::ostream *stream)
        //   virtual ~StreamObserver()
        //   virtual void publish(const shared_ptr<const Record>&, Context&);
        //   virtual void releaseRecords();
        //   void setFormatFunctor(const RecordFormatFunctor& functor);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PRIMARY MANIPULATORS\n"
                             "============================\n";

        {
            Obj mX(stderr);
        }

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        // Open the temporary file we will be writing into
        bdls::TempDirectoryGuard tempDirGuard("ball_cstdioobserver_");
        bsl::string fileName = tempDirGuard.getTempDirName() + "/test2.log";
        FILE *const file_p = fopen(fileName.c_str(), "w");

        FsUtil::Offset fileOffset = FsUtil::getFileSize(fileName);

        if (verbose) cout << "Publish a single message." << endl;
        {
            bslma::TestAllocator   testAllocator("objectAllocator",
                                                 veryVeryVeryVerbose);
            Obj::allocator_type    oa(&testAllocator);
            Obj                    mX(file_p, oa);
            ball::RecordAttributes fixed(&testAllocator);

            bdlt::Datetime timestamp(2017, 4, 1);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1);
            fixed.setThreadID(2);
            fixed.setSeverity(ball::Severity::e_INFO);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(189);
            fixed.setMessage("Log Message");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            mX.publish(record,
                       ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));

            bsl::string output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            ASSERTV(output,
                    "\n01APR2017_00:00:00.000 1 2 INFO test.cpp 189"
                    "  Log Message \n" == output);
        }

        if (verbose) cout << "Publish formatted message." << endl;
        {
            bslma::TestAllocator   testAllocator("objectAllocator",
                                                 veryVeryVeryVerbose);
            bsl::ostringstream     os;
            Obj::allocator_type    oa(&testAllocator);
            Obj                    mX(file_p, oa);
            ball::RecordAttributes fixed(&testAllocator);

            bdlt::Datetime timestamp(2017, 4, 1);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1);
            fixed.setThreadID(2);
            fixed.setSeverity(ball::Severity::e_INFO);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(189);
            fixed.setMessage("Log Message");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            mX.setFormatFunctor(ball::RecordStringFormatter(
                                            "\n%d %p:%t %s %f:%l %c %m %a\n"));

            mX.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                             0,
                                             1));

            bsl::string output = readPartialFile(fileName, fileOffset);
            fileOffset = FsUtil::getFileSize(fileName);

            ASSERTV(output,
                    "\n01APR2017_00:00:00.000 1:2 INFO test.cpp:189"
                    "  Log Message \n" == output);
        }

        if (verbose) cout << "\nNegative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_SAFE_PASS((Obj((stderr))));  // most vexing parse
                ASSERT_SAFE_FAIL((Obj((     0))));
            }

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::shared_ptr<ball::Record>
                            mR(new (scratch) ball::Record(&scratch), &scratch);

                const ball::Context C;

                Obj mX(file_p);

                ASSERT_PASS(mX.publish(mR, C));
                mR.reset();
                ASSERT_FAIL(mX.publish(mR, C));

                fileOffset = FsUtil::getFileSize(fileName);
            }
        }

        fclose(file_p);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Run each method with arbitrary inputs and verify the behavior is
        //    as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                          << "==============\n";

        // Open the temporary file we will be writing into
        bdls::TempDirectoryGuard tempDirGuard("ball_cstdioobserver_");
        bsl::string fileName = tempDirGuard.getTempDirName() + "/test1.log";
        FILE *const file_p = fopen(fileName.c_str(), "w");

        if (verbose) cout << "Publish a single message." << endl;
        {
            Obj X(file_p);

            ball::RecordAttributes fixed;

            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            fixed.setTimestamp(now);
            fixed.setProcessID(100);
            fixed.setThreadID(0);
            fixed.setMessage("Log Message (single)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            X.publish(record,
                      ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));
        }

        if (verbose) cout << "Publish a sequence of three messages." << endl;
        {
            Obj X(file_p);

            ball::RecordAttributes fixed;

            const int NUM_MESSAGES = 3;
            for (int n = 0; n < NUM_MESSAGES; ++n) {
                bdlt::Datetime now =
                                    bdlt::EpochUtil::convertFromTimeT(time(0));
                fixed.setTimestamp(now);
                fixed.setProcessID(201 + n);
                fixed.setThreadID(31 + n);
                fixed.setMessage("Log Message (series)");

                bsl::shared_ptr<ball::Record> record;
                record.createInplace();
                record->setFixedFields(fixed);

                X.publish(record, ball::Context(ball::Transmission::e_TRIGGER,
                                                n,
                                                NUM_MESSAGES));
            }
        }

        fclose(file_p);
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
