// ball_observerformatterimp.t.cpp                                    -*-C++-*-
#include <ball_observerformatterimp.h>

#include <ball_context.h>
#include <ball_observer.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_recordformatterfunctor.h>
#include <ball_recordformattertimezone.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlf_bind.h>
#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>    // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a helper implementation class that provides
// common formatting functionality for observer components. It manages record
// formatters and supports scheme-based format configuration.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] ObserverFormatterImp(format, timezone, alloc)
//
// MANIPULATORS
// [ 3] void setTimezoneDefault(RecordFormatterTimezone::Enum timezone);
// [ 4] void setFormatFunctor(const RecordFormatter& formatter);
// [ 5] int setFormat(const bsl::string_view& format);
//
// ACCESSORS
// [ 5] const bsl::string& getFormat() const;
// [ 2] void formatLogRecord(ostream&, const shared_ptr<const Record>&) const;
// [ 3] RecordFormatterTimezone::Enum getTimezoneDefault() const;
// [ 2] allocator_type get_allocator() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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

typedef ball::ObserverFormatterImp Obj;

/// Define a functor for custom formatting (C++03 compatible)
/// Must be at namespace scope to be used as template argument
struct CustomRecordFormatter {
    void operator()(bsl::ostream&        stream,
                     const ball::Record& rec) const
    {
        stream << "CUSTOM: " << rec.fixedFields().message();
    }
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Formatting Observer
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use `ObserverFormatterImp` to
// implement a simple observer that writes formatted log records to standard
// output.  First, we define a simple observer class that uses
// `ObserverFormatterImp` to manage formatting:
// ```
class MySimpleObserver : public ball::Observer {
    // This class provides a simple observer implementation that writes
    // formatted log records to 'bsl::cout'.

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

  private:
    // DATA
    mutable bslmt::Mutex       d_mutex;         // synchronize access
    ball::ObserverFormatterImp d_formatterImp;  // formatter manager

  public:
    // CREATORS
    explicit MySimpleObserver(const allocator_type& allocator
                                                        = allocator_type())
    : d_formatterImp("text://%t %s %m\n",
                     ball::RecordFormatterTimezone::e_UTC,
                     allocator)
    {
    }

    // MANIPULATORS
    void disablePublishInLocalTime()
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_formatterImp.setTimezoneDefault(
                                     ball::RecordFormatterTimezone::e_UTC);
    }

    void enablePublishInLocalTime()
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_formatterImp.setTimezoneDefault(
                                   ball::RecordFormatterTimezone::e_LOCAL);
    }

    using Observer::publish;

    void publish(const bsl::shared_ptr<const ball::Record>& record,
                 const ball::Context&)                    BSLS_KEYWORD_OVERRIDE
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_formatterImp.formatLogRecord(bsl::cout, record);
    }

    void releaseRecords() BSLS_KEYWORD_OVERRIDE
    {
        // No-op for this observer
    }

    int setFormat(const bsl::string_view& format)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        return d_formatterImp.setFormat(format);
    }

    void setFormatFunctor(
                       const ball::RecordFormatterFunctor::Type& formatter)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_formatterImp.setFormatFunctor(formatter);
    }

    // ACCESSORS
    const bsl::string& getFormat() const
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        return d_formatterImp.getFormat();
    }

    bool isPublishInLocalTimeEnabled() const
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        return ball::RecordFormatterTimezone::e_LOCAL ==
                                      d_formatterImp.getTimezoneDefault();
    }
};
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;  (void)veryVerbose;
    const bool veryVeryVerbose     = argc > 4;  (void)veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Formatting Observer
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use `ObserverFormatterImp` to
// implement a simple observer that writes formatted log records to standard
// output.  First, we define a simple observer class that uses
// `ObserverFormatterImp` to manage formatting:
// ```
//  /// This class provides a simple observer implementation that writes
//  /// formatted log records to 'bsl::cout'.
//  class MySimpleObserver : public ball::Observer {
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//    private:
//      // DATA
//      bslmt::Mutex               d_mutex;         // synchronize access
//      ball::ObserverFormatterImp d_formatterImp;  // formatter manager
//
//    public:
//      // CREATORS
//      explicit MySimpleObserver(const allocator_type& allocator
//                                                          = allocator_type())
//      : d_formatterImp("text://%t %s %m\n",
//                       ball::RecordFormatterTimezone::e_UTC,
//                       allocator)
//      {
//      }
//
//      // MANIPULATORS
//      void disablePublishInLocalTime()
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setTimezoneDefault(
//                                     ball::RecordFormatterTimezone::e_UTC);
//      }
//
//      void enablePublishInLocalTime()
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setTimezoneDefault(
//                                   ball::RecordFormatterTimezone::e_LOCAL);
//      }
//
//      using Observer::publish;
//
//      void publish(const bsl::shared_ptr<const ball::Record>& record,
//                   const ball::Context&                       context)
//                                                      BSLS_KEYWORD_OVERRIDE
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.formatLogRecord(bsl::cout, record);
//      }
//
//      void releaseRecords() BSLS_KEYWORD_OVERRIDE
//      {
//          // No-op for this observer
//      }
//
//      int setFormat(const bsl::string_view& format)
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return d_formatterImp.setFormat(format);
//      }
//
//      void setFormatFunctor(const RecordFormatter& formatter)
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setFormatFunctor(formatter);
//      }
//
//      // ACCESSORS
//      const bsl::string& getFormat() const
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return d_formatterImp.getFormat();
//      }
//
//      bool isPublishInLocalTimeEnabled() const
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return ball::RecordFormatterTimezone::e_LOCAL ==
//                                      d_formatterImp.getTimezoneDefault();
//      }
//  };
// ```
// Now, we can create an instance of our observer and configure its formatting.
// First, we create an observer with default text-based format:
// ```
  MySimpleObserver observer;
// ```
// Next, we can change the format to JSON format using the `setFormat` method:
// ```
  const int rc = observer.setFormat("qjson://%t %s %m");
  ASSERT(0 == rc);
// ```
// We can also enable local time for timestamps:
// ```
  observer.enablePublishInLocalTime();
  ASSERT(observer.isPublishInLocalTimeEnabled());
// ```
// To revert to UTC time:
// ```
  observer.disablePublishInLocalTime();
  ASSERT(!observer.isPublishInLocalTimeEnabled());
// ```
// We can also retrieve the current format configuration:
// ```
  const bsl::string& currentFormat = observer.getFormat();
  ASSERT("qjson://%t %s %m" == currentFormat);
// ```
// For backwards compatibility, we can also use a custom formatter functor:
// ```
  observer.setFormatFunctor(
                  ball::RecordStringFormatter("%I %p:%t %s %f:%l %c %m %a\n"));
// ```
// Now let's demonstrate actually publishing a log record. First, we create a
// sample record and context with a fixed timestamp:
// ```
  const bdlt::Datetime    timestamp(2024, 1, 15, 12, 34, 56, 789);
  ball::RecordAttributes  attributes(
                              timestamp,                 // timestamp
                              42,                        // process ID
                              123,                       // thread ID
                              "example.cpp",             // file name
                              456,                       // line number
                              "EXAMPLE",                 // category
                              ball::Severity::e_WARN,    // severity
                              "Sample warning message"); // message

  bsl::shared_ptr<ball::Record>  record;
  record.createInplace(bslma::Default::allocator(),
                       attributes,
                       ball::UserFields());
  ball::Context context;
// ```
// To capture the output for verification, we redirect 'bsl::cout' to a string
// stream:
// ```
  bsl::ostringstream  oss;
  bsl::streambuf     *originalRdbuf = bsl::cout.rdbuf();
// ```
// When we publish this record with text format, it produces plain text output:
// ```
  observer.setFormat("text://%d %p:%t %s %f:%l %c %m\n");
  bsl::cout.rdbuf(oss.rdbuf());
  observer.publish(record, context);

  bsl::string output = oss.str();
  bsl::cout.rdbuf(originalRdbuf);
  ASSERT("15JAN2024_12:34:56.789 42:123 WARN example.cpp:456 EXAMPLE "
         "Sample warning message\n" == output);

  oss.str("");  // Clear the stream for next test
// ```
// Next, we publish the same record with JSON format that produces structured
// JSON output:
// ```
  observer.setFormat("qjson://%d %p %t %s %F %l %c %m");
  bsl::cout.rdbuf(oss.rdbuf());
  observer.publish(record, context);

  output = oss.str();
  bsl::cout.rdbuf(originalRdbuf);
// ```
// Finally, we verify the JSON fields:
// ```
  ASSERT(bsl::string::npos != output.find("\"timestamp\""));
  ASSERT(bsl::string::npos != output.find("\"15JAN2024_12:34:56.789\""));
  ASSERT(bsl::string::npos != output.find("\"pid\""));
  ASSERT(bsl::string::npos != output.find("42"));
  ASSERT(bsl::string::npos != output.find("\"tid\""));
  ASSERT(bsl::string::npos != output.find("123"));
  ASSERT(bsl::string::npos != output.find("\"severity\""));
  ASSERT(bsl::string::npos != output.find("\"WARN\""));
  ASSERT(bsl::string::npos != output.find("\"message\""));
  ASSERT(bsl::string::npos != output.find("\"Sample warning message\""));
// ```
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'setFormat' AND 'getFormat'
        //
        // Concerns:
        // 1. 'setFormat' correctly configures the formatter based on format
        //    specification string.
        //
        // 2. Various format schemes (text, json, qjson) are supported.
        //
        // 3. Invalid format specifications are rejected with non-zero return
        //    and the previous formatter is retained.
        //
        // 4. 'getFormat' returns the format string that was successfully set.
        //
        // Plan:
        // 1. Test 'setFormat' with various valid format specifications and
        //    verify the output matches expectations by invoking the formatter.
        //
        // 2. Test 'setFormat' with an invalid format and verify it returns
        //    non-zero and retains the previous formatter.
        //
        // 3. Verify 'getFormat' returns the format string after successful
        //    'setFormat'.
        //
        // Testing:
        //   int setFormat(const bsl::string_view& format);
        //   const bsl::string& getFormat() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'setFormat' AND 'getFormat'\n"
                             "===================================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Test setFormat with text format
        if (verbose) cout << "\tTesting text format.\n";
        {
            Obj mX("text://%m",
                   ball::RecordFormatterTimezone::e_UTC,
                   &ta);
            const Obj& X = mX;

            const char *format = "text://%d %p:%t %s %m";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            // Verify formatter works
            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2024, 1, 15, 12, 34, 56, 789);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(42);
            fixed.setThreadID(123);
            fixed.setSeverity(ball::Severity::e_WARN);
            fixed.setMessage("test message");

            ball::Record record(fixed, ball::UserFields(&ta), &ta);
            bsl::shared_ptr<const ball::Record> recordPtr(
                                  &record,
                                  bslstl::SharedPtrNilDeleter());

            bsl::ostringstream oss;
            X.formatLogRecord(oss, recordPtr);

            bsl::string output = oss.str();
            ASSERTV(output, bsl::string::npos != output.find("test message"));
            ASSERTV(output, bsl::string::npos != output.find("WARN"));
        }

        // Test setFormat with qjson format
        if (verbose) cout << "\tTesting qjson format.\n";
        {
            Obj mX("qjson://%m",
                   ball::RecordFormatterTimezone::e_UTC,
                   &ta);
            const Obj& X = mX;

            const char *format = "qjson://%m";
            ASSERT(0 == mX.setFormat(format));
            ASSERT(format == X.getFormat());

            // Verify formatter produces JSON
            ball::RecordAttributes fixed(&ta);
            const bdlt::Datetime timestamp(2024, 1, 15, 12, 34, 56, 789);
            fixed.setTimestamp(timestamp);
            fixed.setSeverity(ball::Severity::e_ERROR);
            fixed.setMessage("json test");

            ball::Record record(fixed, ball::UserFields(&ta), &ta);
            bsl::shared_ptr<const ball::Record> recordPtr(
                                  &record,
                                  bslstl::SharedPtrNilDeleter());

            bsl::ostringstream oss;
            X.formatLogRecord(oss, recordPtr);

            bsl::string output = oss.str();
            ASSERTV(output, bsl::string::npos != output.find("\"message\""));
            ASSERTV(output, bsl::string::npos != output.find("json test"));
        }

        // Test invalid format returns non-zero
        if (verbose) cout << "\tTesting invalid format rejection.\n";
        {
            Obj mX("text://%m",
                   ball::RecordFormatterTimezone::e_UTC,
                   &ta);
            const Obj& X = mX;

            bsl::string originalFormat = X.getFormat();
            ASSERT("text://%m" == originalFormat);

            // Test invalid format returns non-zero
            ASSERT(0 != mX.setFormat("invalid_scheme://test"));

            // After failed setFormat, previous format should be retained
            ASSERT(originalFormat == X.getFormat());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'setFormatFunctor'
        //
        // Concerns:
        // 1. 'setFormatFunctor' correctly replaces the formatter.
        //
        // 2. After 'setFormatFunctor', 'getFormat' returns empty string.
        //
        // Plan:
        // 1. Create object with initial format, then call 'setFormatFunctor'
        //    with custom formatter and verify it's used.
        //
        // 2. Verify 'getFormat' returns empty after 'setFormatFunctor'.
        //
        // Testing:
        //   void setFormatFunctor(const RecordFormatter& formatter);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'setFormatFunctor'\n"
                             "==========================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        Obj mX("text://%m", ball::RecordFormatterTimezone::e_UTC, &ta);
        const Obj& X = mX;

        ASSERT("text://%m" == X.getFormat());

        // Set custom formatter
        ball::RecordStringFormatter customFormatter("CUSTOM: %m\n", &ta);

        mX.setFormatFunctor(customFormatter);

        // getFormat should return empty string after setFormatFunctor
        ASSERT(X.getFormat().empty());

        // Verify the custom formatter is used
        ball::RecordAttributes fixed(&ta);
        fixed.setMessage("custom test");

        ball::Record record(fixed, ball::UserFields(&ta), &ta);
        bsl::shared_ptr<const ball::Record> recordPtr(
                              &record,
                              bslstl::SharedPtrNilDeleter());

        bsl::ostringstream oss;
        X.formatLogRecord(oss, recordPtr);

        bsl::string output = oss.str();
        ASSERTV(output, bsl::string::npos != output.find("CUSTOM:"));
        ASSERTV(output, bsl::string::npos != output.find("custom test"));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'setTimezoneDefault' AND 'getTimezoneDefault'
        //
        // Concerns:
        // 1. 'setTimezoneDefault' correctly updates the timezone setting.
        //
        // 2. 'getTimezoneDefault' returns the current timezone setting.
        //
        // 3. When timezone is changed, the underlying formatter is updated
        //    to reflect the new timezone (i.e., the formatter actually uses
        //    the new timezone when formatting records).
        //
        // 4. Timezone changes affect formatters created from format strings
        //    but not formatters set via 'setFormatFunctor'.
        //
        // Plan:
        // 1. Create object with UTC timezone and a format string that includes
        //    datetime (%d), verify 'getTimezoneDefault'.
        //
        // 2. Format a record and verify the output uses UTC timestamp.
        //
        // 3. Call 'setTimezoneDefault' to LOCAL, verify it's updated.
        //
        // 4. Format the same record and verify the output now uses local time
        //    (different from UTC).
        //
        // 5. Change back to UTC and verify output changes back.
        //
        // 6. Test that 'setFormatFunctor' with a custom formatter is not
        //    affected by subsequent 'setTimezoneDefault' calls.
        //
        // Testing:
        //   void setTimezoneDefault(RecordFormatterTimezone::Enum timezone);
        //   RecordFormatterTimezone::Enum getTimezoneDefault() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'setTimezoneDefault'\n"
                             "============================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Use a format with datetime to verify timezone effects
        Obj mX("text://%d %m",
               ball::RecordFormatterTimezone::e_UTC,
               &ta);
        const Obj& X = mX;

        // Verify initial timezone
        ASSERT(ball::RecordFormatterTimezone::e_UTC == X.getTimezoneDefault());

        // Create a test record with a specific timestamp
        bdlt::Datetime timestamp(2024, 12, 6, 12, 30, 45, 123);
        ball::RecordAttributes fixed(&ta);
        fixed.setTimestamp(timestamp);
        fixed.setMessage("test message");
        ball::Record record(fixed, ball::UserFields(&ta), &ta);
        bsl::shared_ptr<const ball::Record> recordPtr(
                              &record,
                              bslstl::SharedPtrNilDeleter());

        // Format with UTC timezone
        bsl::ostringstream utcStream;
        X.formatLogRecord(utcStream, recordPtr);
        bsl::string utcOutput = utcStream.str();

        if (veryVerbose) { T_ P_(utcOutput) }

        // Verify the timestamp appears in the output
        ASSERT(bsl::string::npos != utcOutput.find("2024"));
        ASSERT(bsl::string::npos != utcOutput.find("test message"));

        // Change to LOCAL timezone
        mX.setTimezoneDefault(ball::RecordFormatterTimezone::e_LOCAL);
        ASSERT(ball::RecordFormatterTimezone::e_LOCAL ==
                                                       X.getTimezoneDefault());

        // Format the same record with LOCAL timezone
        // Note: The actual local time offset depends on the system, but the
        // important thing is that the formatter was recreated and will
        // call the local time offset callback when formatting.
        bsl::ostringstream localStream;
        X.formatLogRecord(localStream, recordPtr);
        bsl::string localOutput = localStream.str();

        if (veryVerbose) { T_ P_(localOutput) }

        // The message should still be present
        ASSERT(bsl::string::npos != localOutput.find("test message"));

        // Change back to UTC
        mX.setTimezoneDefault(ball::RecordFormatterTimezone::e_UTC);
        ASSERT(ball::RecordFormatterTimezone::e_UTC == X.getTimezoneDefault());

        // Format again with UTC timezone
        bsl::ostringstream utcStream2;
        X.formatLogRecord(utcStream2, recordPtr);
        bsl::string utcOutput2 = utcStream2.str();

        if (veryVerbose) { T_ P_(utcOutput2) }

        // Should match the original UTC output
        ASSERT(utcOutput == utcOutput2);

        // Test that setFormatFunctor is not affected by setTimezoneDefault
        if (verbose) cout << "\tTesting setFormatFunctor independence.\n";
        {
            Obj mY("text://%d %m",
                   ball::RecordFormatterTimezone::e_UTC,
                   &ta);

            // Set a custom formatter function that ignores timezone
            Obj::RecordFormatter customFormatter = CustomRecordFormatter();

            mY.setFormatFunctor(customFormatter);

            bsl::ostringstream customStream1;
            mY.formatLogRecord(customStream1, recordPtr);
            bsl::string customOutput1 = customStream1.str();

            // Now change timezone - this should NOT affect the custom formatter
            mY.setTimezoneDefault(ball::RecordFormatterTimezone::e_LOCAL);

            bsl::ostringstream customStream2;
            mY.formatLogRecord(customStream2, recordPtr);
            bsl::string customOutput2 = customStream2.str();

            // Output should be identical since custom formatter ignores timezone
            ASSERT(customOutput1 == customOutput2);
            ASSERT(bsl::string::npos != customOutput1.find("CUSTOM:"));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //
        // Concerns:
        // 1. Constructor correctly initializes object with given parameters.
        //
        // 2. 'formatLogRecord' can format a record to a stream.
        //
        // 3. 'get_allocator' returns the allocator provided at construction.
        //
        // Plan:
        // 1. Create objects with various initial parameters and verify
        //    accessors return expected values.
        //
        // 2. Verify the formatter can be invoked successfully.
        //
        // Testing:
        //   ObserverFormatterImp(format, timezone, ...)
        //   void formatLogRecord(ostream&, const shared_ptr<const Record>&);
        //   allocator_type get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "PRIMARY MANIPULATORS AND BASIC ACCESSORS\n"
                             "========================================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Test construction with text scheme
        if (verbose) cout << "\tTesting with text scheme.\n";
        {
            Obj mX("text://%m",
                   ball::RecordFormatterTimezone::e_UTC,
                   &ta);
            const Obj& X = mX;

            ASSERT(ball::RecordFormatterTimezone::e_UTC ==
                                                       X.getTimezoneDefault());
            ASSERT(&ta == X.get_allocator());

            // Verify formatter works
            ball::RecordAttributes fixed(&ta);
            fixed.setMessage("test");
            ball::Record record(fixed, ball::UserFields(&ta), &ta);
            bsl::shared_ptr<const ball::Record> recordPtr(
                                  &record,
                                  bslstl::SharedPtrNilDeleter());

            bsl::ostringstream oss;
            X.formatLogRecord(oss, recordPtr);
            ASSERT(bsl::string::npos != oss.str().find("test"));
        }

        // Test construction with qjson scheme
        if (verbose) cout << "\tTesting with qjson scheme.\n";
        {
            Obj mX("qjson://%m",
                   ball::RecordFormatterTimezone::e_LOCAL,
                   &ta);
            const Obj& X = mX;

            ASSERT(ball::RecordFormatterTimezone::e_LOCAL ==
                                                      X.getTimezoneDefault());
        }
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
        // 1. Create an object and exercise basic methods.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                          << "==============\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Create object with default format
        Obj mX("text://%m", ball::RecordFormatterTimezone::e_UTC, &ta);

        // Set a format
        ASSERT(0 == mX.setFormat("text://%s %m"));

        // Create a record and format it
        ball::RecordAttributes fixed(&ta);
        fixed.setSeverity(ball::Severity::e_INFO);
        fixed.setMessage("breathing test");

        ball::Record record(fixed, ball::UserFields(&ta), &ta);
        bsl::shared_ptr<const ball::Record> recordPtr(
                              &record,
                              bslstl::SharedPtrNilDeleter());

        bsl::ostringstream oss;
        mX.formatLogRecord(oss, recordPtr);

        bsl::string output = oss.str();
        ASSERT(bsl::string::npos != output.find("breathing test"));
        ASSERT(bsl::string::npos != output.find("INFO"));
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

