// ball_recordstringformatter.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_recordstringformatter.h>

#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>
#include <bslmt_threadutil.h>

#include <bdlt_currenttime.h>
#include <bslim_testutil.h>

#include <bdlt_datetime.h>
#include <bdlt_iso8601util.h>
#include <bdlt_localtimeoffset.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_platform.h>
#include <bsls_types.h>


#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>                  // for 'strcmp'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>                      // for 'getpid'
#endif

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::strcmp;
using bsl::ostringstream;
using bsl::string;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is implemented using one 'bsl::string' object and
// one 'bdlt::DatetimeInterval' object.  The value-semantic correctness of this
// component therefore largely depends on that of those two contained class.
// We simply follow the standard 10-case test suite.  In addition, since the
// implemented class is a function object, the 'operator()' method that
// provides string-based formatting support is extensively tested.
//
// CREATORS
// [ 2] ball::RecordStringFormatter(*ba = 0);
// [10] ball::RecordStringFormatter(const char *, *ba = 0);
// [10] ball::RecordStringFormatter(bdet::DtI, *ba = 0);
// [13] ball::RecordStringFormatter(bool, *ba = 0);
// [10] ball::RecordStringFormatter(const char *, bdet::DtI, *ba = 0);
// [13] ball::RecordStringFormatter(const char *, bool, *ba = 0);
// [ 7] ball::RecordStringFormatter(const ball::RSF&, *ba = 0);
// [ 2] ~ball::RecordStringFormatter();
// MANIPULATORS
// [ 9] const ball::RSF& operator=(const ball::RSF& other);
// [13] void disablePublishInLocalTime();
// [13] void enablePublishInLocalTime();
// [ 2] void setFormat(const char *format);
// [ 2] void setTimestampOffset(const bdlt::DatetimeInterval& offset);
// ACCESSORS
// [ 2] const char *format() const;
// [13] bool isPublishInLocalTimeEnabled() const;
// [ 2] const bdlt::DatetimeInterval& timestampOffset() const;
// [11] void operator()(bsl::ostream&, const ball::Record&) const;
// FREE OPERATORS
// [ 6] bool operator==(const ball::RSF& lhs, const ball::RSF& rhs);
// [ 6] bool operator!=(const ball::RSF& lhs, const ball::RSF& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::RSF&);
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [12] USAGE example

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
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef ball::RecordStringFormatter Obj;
typedef ball::Record                Rec;

// Values for testing.
const char *F0 = "\n%d %p:%t %s %f:%l %c %m %u\n";
const bdlt::DatetimeInterval T0(0);

const char *FA = "some format";
const bdlt::DatetimeInterval TA(0, 1);

const char *FB = "another format";
const bdlt::DatetimeInterval TB(0, -1);

const char *FC = "yet another format";
const bdlt::DatetimeInterval TC(1, 0);

const char *MSG_1BYTE   = "0";
const char *MSG_20BYTE  = "01234567890123456789";
const char *MSG_200BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";
const char *MSG_450BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";
const char *MSG_550BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";

bool compareText(bslstl::StringRef lhs, bslstl::StringRef rhs)
{
    for (unsigned int i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                 << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                 << endl;
            return false;                                             // RETURN
        }
    }

    if (lhs.length() < rhs.length()) {
        unsigned int i = lhs.length();
        cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = END-OF-STRING "
                 << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                 << endl;
        return false;                                                 // RETURN

    }
    if (lhs.length() > rhs.length()) {
        unsigned int i = rhs.length();
        cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                 << "rhs[i] = END-OF-STRING"
                 << endl;
        return false;                                                 // RETURN
    }
    return true;

}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // TESTING: Records Show Calculated Local-Time Offset
        //   Per DRQS 13681097, records observe DST time transitions when
        //   'publishInLocalTime' attribute is 'true'.
        //
        // Concerns:
        //: 1 The CTORs that take a 'publishInLocalTime' parameter create
        //:   objects with the specified attribute.
        //:
        //: 2 The CTORs that do not take a 'publishInLocalTime' parameter
        //:   create objects having a 'publishInLocalTime' attribute that is
        //:   'false'.
        //:
        //: 3 The manipulators of the 'publishInLocalTime' attribute can set
        //:   the unset that attribute, and the accessor for that attribute
        //:   always returns the expected value.
        //:
        //: 4 The manipulators of the 'publishInLocalTime' attribute are
        //:   idempotent.
        //:
        //: 5 Objects having the 'publishInLocalTime' attribute actually
        //:   resolve the '%i' format specifcation in local time, irrespective
        //:   of how that attribute was set (at construction or by
        //:   manipulator).
        //:
        //: 6 Objects having the 'publishInLocalTime' attribute 'false' resolve
        //:   the '%i' format specification with their specified offset.
        //
        // Plan:
        //: 1 Create objects using the different
        //:   constructors and verify that the 'publishInLocalTime' attribute
        //:   has the expected value.  (C-1..2)
        //:
        //: 2 Default create an object and use the manipulator to
        //:   change the attribute, and then reset the attribute to the
        //:   original state.  Each use of a manipulator is done twice
        //:   to confirm idempotence. (C-3..4)
        //:
        //: 3 Create an object with a distinguished local time offset and
        //:   verify that the '%i' format specification is resolved to
        //:   the specified offset or the actual local time offset according
        //:   to the state of the 'publishInLocalTime' attribute.  (C-5..6)
        //
        // Testing:
        //   ball::RecordStringFormatter(bool, *ba = 0);
        //   ball::RecordStringFormatter(const char *, bool, *ba = 0);
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   bool isPublishInLocalTimeEnabled() const;
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "TESTING: Records Show Calculated Local-Time Offset" <<endl
                << "==================================================" <<endl;

        if (verbose) cout << "\nTest Constructors" << endl;
        {
            Obj mX0(true);
            if (veryVerbose) {
                P(mX0.timestampOffset().totalMilliseconds());
            }
            ASSERT( mX0.isPublishInLocalTimeEnabled());

            Obj mX1(false);
            if (veryVerbose) {
                P(mX1.timestampOffset().totalMilliseconds());
            }
                                  ASSERT(!mX1.isPublishInLocalTimeEnabled());
            Obj mX2("%i", true);  ASSERT( mX2.isPublishInLocalTimeEnabled());
            Obj mX3("%i", false); ASSERT(!mX3.isPublishInLocalTimeEnabled());

            Obj mX4;              ASSERT(!mX4.isPublishInLocalTimeEnabled());
            Obj mX5("%i");        ASSERT(!mX5.isPublishInLocalTimeEnabled());
            Obj mX6("%i", bdlt::DatetimeInterval(10));
                                  ASSERT(!mX6.isPublishInLocalTimeEnabled());
        }

        if (verbose) cout << "\nTest Manipulators and Accessor" << endl;
        {
            Obj mX;                  ASSERT(!mX.isPublishInLocalTimeEnabled());

            mX.enablePublishInLocalTime();
                                     ASSERT( mX.isPublishInLocalTimeEnabled());
            mX.enablePublishInLocalTime();
                                     ASSERT( mX.isPublishInLocalTimeEnabled());
            mX.disablePublishInLocalTime();
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());
            mX.disablePublishInLocalTime();
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());
        }

        if (verbose) cout << "\nTest Fixed and Calculated Offsets" << endl;
        {
            Obj mX("%i", bdlt::DatetimeInterval(10));
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());

            bdlt::Datetime         dtUtc(2014, 2, 19);


            ball::RecordAttributes fixedFields(dtUtc,
                                              0,
                                              0,
                                              "",
                                              0,
                                              "",
                                              ball::Severity::e_OFF,
                                              "");
            ball::Record           mRecord(fixedFields, ball::UserFields());
            const ball::Record&    record = mRecord;

            bdlt::Datetime dtWithOffset(dtUtc); dtWithOffset.addDays(10);

            if (veryVerbose) { P_(dtUtc) P(dtWithOffset); }

            ostringstream ossExpected;
            ossExpected
                 << bsl::setw(4) << bsl::setfill('0') << dtWithOffset.year()
                 << '-'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.month()
                 << '-'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.day()
                 << 'T'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.hour()
                 << ':'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.minute()
                 << ':'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.second();

            ostringstream ossActual;
            mX(ossActual, record);

            if (veryVerbose) { T_ P_(ossExpected.str()) P(ossActual.str()) }
            ASSERT((ossExpected.str() == ossActual.str()));

            ossExpected.str(""); ossActual.str("");

            int localTimeOffsetInSeconds =
                  bdlt::LocalTimeOffset::localTimeOffset(dtUtc).totalSeconds();
            ASSERT(10 * 24 * 60 * 60 != localTimeOffsetInSeconds);

            if (veryVerbose) { P(localTimeOffsetInSeconds); }

            bdlt::Datetime dtWithLTO(dtUtc);
            dtWithLTO.addSeconds(localTimeOffsetInSeconds);

            if (veryVerbose) { P_(dtUtc) P(dtWithLTO); }

            ossExpected
                    << bsl::setw(4) << bsl::setfill('0') << dtWithLTO.year()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.month()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.day()
                    << 'T'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.hour()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.minute()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.second();

            mX.enablePublishInLocalTime();
            ASSERT( mX.isPublishInLocalTimeEnabled());

            mX(ossActual, record);
            if (veryVerbose) { T_ P_(ossExpected.str()) P(ossActual.str()) }
            ASSERT((ossExpected.str() == ossActual.str()));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl<< "Testing Usage Example" << endl
                                 << "=====================" << endl;

///Usage
///-----
// The following snippets of code illustrate how to use an instance of
// 'ball::RecordStringFormatter' to format log records.
//
// First we instantiate a record formatter with an explicit format
// specification (but we accept the default timestamp offset since it will not
// be used in this example):
//..
    ball::RecordStringFormatter formatter("\n%t: %m\n");
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Next we create a default 'ball::Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
    ball::Record record;
//
    record.fixedFields().setThreadID(6);
    record.fixedFields().setMessage("Hello, World!");
//..
// The following "invocation" of the 'formatter' function object formats
// 'record' to 'bsl::cout' according to the format specification supplied at
// construction:
//..
//  formatter(bsl::cout, record);
//..
// As a result of this call, the following is printed to 'stdout':
//..
//  6: Hello, World!
//..
        ostringstream oss;
        formatter(oss, record);
        if (veryVerbose) cout << oss.str();

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR()
        //   Operator () should print out the given 'record' in the format
        //   defined by 'd_format'.
        //
        // Test plan:
        //   Test every pattern to see if the output is the same as expected.
        //
        // Testing:
        //   void operator()(bsl::ostream&, const ball::Record&) const;
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        const int   lineNum   = 542;
        const char *filename  = "subdir/process.cpp";
        const bsls::Types::Uint64
                               threadID  = bslmt::ThreadUtil::selfIdAsUint64();
#ifdef BSLS_PLATFORM_OS_UNIX
        const pid_t processID = getpid();
#else
        const int   processID = 0;
#endif
        const char *MSG       = "Hello world!";
        ball::RecordAttributes fixedFields(bdlt::Datetime(),
                                          processID,
                                          threadID,
                                          filename,
                                          lineNum,
                                          "FOO.BAR.BAZ",
                                          ball::Severity::e_WARN,
                                          MSG);

        fixedFields.setTimestamp(bdlt::CurrentTime::utc());

        ball::UserFields userFields;
        userFields.appendString("string");
        userFields.appendDouble(3.14159265);
        userFields.appendInt64(1000000);

        ball::Record mRecord(fixedFields, userFields);
        const ball::Record& record = mRecord;

        ostringstream oss1, oss2;
        bdlt::Datetime timestamp = record.fixedFields().timestamp();
        Obj mX;
        const Obj& X = mX;
        ASSERT(bdlt::DatetimeInterval(0) == X.timestampOffset());

        if (verbose) cout << "\n  Testing default format." << endl;
        {
            oss1.str("");
            X(oss1, record);
            if (veryVerbose) { P(oss1.str()); }
        }

        if (verbose) cout << "\n  Testing \"%%\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%%");
            X(oss1, record);
            if (veryVerbose) { P(oss1.str()); }
            ASSERT(oss1.str() == "%");
        }

        if (verbose) cout << "\n  Testing \"%d\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%d");
            X(oss1, record);
            oss2 << record.fixedFields().timestamp();
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%i\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%i");
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.second()
                << 'Z';
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());

            // Is the resulting string parseable?
            bdlt::Datetime dt;
            int           rc = bdlt::Iso8601Util::parse(
                                              &dt,
                                              oss1.str().c_str(),
                                              bsl::strlen(oss1.str().c_str()));
            bdlt::Datetime adjustedTimestamp(timestamp);
            adjustedTimestamp.setMillisecond(0); // "%i" => no msecs printed

            if (veryVerbose) { P_(rc) P_(adjustedTimestamp) P(dt) }
            ASSERT(0                 == rc);
            ASSERT(adjustedTimestamp == dt);
        }

        if (verbose) cout << "\n  Testing \"%I\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%I");
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.second()
                << '.'
                << bsl::setw(3) << bsl::setfill('0') << timestamp.millisecond()
                << 'Z';
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());

            // Is the resulting string parseable?
            bdlt::Datetime dt;
            int           rc = bdlt::Iso8601Util::parse(
                                              &dt,
                                              oss1.str().c_str(),
                                              bsl::strlen(oss1.str().c_str()));

            if (veryVerbose) { P_(rc) P_(timestamp) P(dt) }
            ASSERT(0         == rc);
            ASSERT(timestamp == dt);
        }

        if (verbose) cout << "\n  Testing \"%i\" with local timestamp."
                          << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%i");
            bdlt::DatetimeInterval offset(0, 2);   // an offset of 2 hours
            mX.setTimestampOffset(offset);
            bdlt::Datetime localTime = timestamp + offset;
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << localTime.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << localTime.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << localTime.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << localTime.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << localTime.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << localTime.second();
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());

            // Is the resulting string parseable?
            bdlt::Datetime dt;
            int           rc = bdlt::Iso8601Util::parse(
                                              &dt,
                                              oss1.str().c_str(),
                                              bsl::strlen(oss1.str().c_str()));
            bdlt::Datetime adjustedTimestamp(localTime);
            adjustedTimestamp.setMillisecond(0); // "%i" => no msecs printed

            if (veryVerbose) { P_(rc) P_(adjustedTimestamp) P(dt) }
            ASSERT(0                 == rc);
            ASSERT(adjustedTimestamp == dt);

            mX.setTimestampOffset(bdlt::DatetimeInterval());
        }

        if (verbose) cout << "\n  Testing \"%p\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%p");
            X(oss1, record);
            oss2 << processID;
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%t\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%t");
            X(oss1, record);
            oss2 << threadID;
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%s\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%s");
            X(oss1, record);
            oss2 << ball::Severity::toAscii(
                       (ball::Severity::Level)record.fixedFields().severity());
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%f\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%f");
            X(oss1, record);
            oss2 << filename;
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%F\"." << endl;
        {
#ifdef BSLS_PLATFORM_OS_WINDOWS
#define SLASH "\\"
#else
#define SLASH "/"
#endif
            static const struct {
                int         d_lineNum;     // line number of test vector
                const char *d_recordFile;  // '__FILE__' in record
                const char *d_outputFile;  // part of '__FILE__' output
            } DATA[] = {
                // line   record filename             output filename
                // ----   ---------------             ---------------
                {  L_,    "foo.c",                    "foo.c"            },
                {  L_,    SLASH "foo.c",              "foo.c"            },
                {  L_,    "foo" SLASH "bar.c",        "bar.c"            },
                {  L_,    SLASH "foo" SLASH "bar.c",  "bar.c"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            mX.setFormat("%F");

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_lineNum;
                const char *RECORDFILE = DATA[ti].d_recordFile;
                const char *OUTPUTFILE = DATA[ti].d_outputFile;

                Rec mR;  const Rec &R = mR;
                mR.fixedFields().setFileName(RECORDFILE);

                ostringstream oss;
                X(oss, R);

                if (veryVerbose) { P_(RECORDFILE);  P(OUTPUTFILE) }
                LOOP_ASSERT(LINE, 0 == bsl::strcmp(OUTPUTFILE,
                                                   oss.str().c_str()));
            }
        }

        if (verbose) cout << "\n  Testing \"%l\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%l");
            X(oss1, record);
            oss2 << lineNum;
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == oss2.str());
        }

        if (verbose) cout << "\n  Testing \"%c\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%c");
            X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == "FOO.BAR.BAZ");
        }

        if (verbose) cout << "\n  Testing \"%m\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%m");
            X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == MSG);
        }

        if (verbose) cout << "\n  Testing \"%x\" and \"%X\"." << endl;
        {
            static const struct {
                int         d_lineNum;
                const char *d_spec;
                const char *d_expectedResult_x;
                const char *d_expectedResult_X;
            } DATA[] = {

                //line   spec             expectedResult_x     expectedResult_X
                //----   ----             ----------------     ----------------

                // Some printable characters.
                { L_,    "a",             "a",                  "61"         },
                { L_,    "A",             "A",                  "41"         },
                { L_,    "aB",            "aB",                 "6142"       },
                { L_,    "aBc",           "aBc",                "614263"     },

                // Hex conversion
                { L_,    "\x01",          "\\x01",              "01"         },
                { L_,    "\x02",          "\\x02",              "02"         },
                { L_,    "\n",            "\\x0A",              "0A"         },
                { L_,    "\\\r",          "\\\\x0D",            "5C0D"       },
                { L_,    "\x7F",          "\\x7F",              "7F"         },
                { L_,    "\x80",          "\\x80",              "80"         },
                { L_,    "\x81",          "\\x81",              "81"         },
                { L_,    "\xFE\\",        "\\xFE\\",            "FE5C"       },
                { L_,    "\\\xFF",        "\\\\xFF",            "5CFF"       },

                // Interleaved printable, non-printable and slash.
                { L_,    "a\rb\nc\\",     "a\\x0Db\\x0Ac\\",  "610D620A635C" },
                { L_,    "\\aA\rbB\ncC",  "\\aA\\x0DbB\\x0AcC",
                                                        "5C61410D62420A6343" },
                { L_,    " \r\n",         " \\x0D\\x0A",      "200D0A"       },
                { L_,    "\r\nx",         "\\x0D\\x0Ax",      "0D0A78"       },
                { L_,    "\r\nz\\y",      "\\x0D\\x0Az\\y",   "0D0A7A5C79"   },
                { L_,    "a\r\nz\\y",     "a\\x0D\\x0Az\\y",  "610D0A7A5C79" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE               = DATA[i].d_lineNum;
                const char *SPEC               = DATA[i].d_spec;
                const char *EXPECTED_RESULT_x  = DATA[i].d_expectedResult_x;
                const char *EXPECTED_RESULT_X  = DATA[i].d_expectedResult_X;

                mRecord.fixedFields().setMessage(SPEC);

                oss1.str("");
                mX.setFormat("%x");
                X(oss1, record);
                if (veryVerbose) {
                    P_(LINE); P_(SPEC); P_(oss1.str()); P(EXPECTED_RESULT_x);
                }
                ASSERT(oss1.str() == EXPECTED_RESULT_x);

                oss2.str("");
                mX.setFormat("%X");
                X(oss2, record);
                if (veryVerbose) {
                    P_(LINE); P_(SPEC); P_(oss2.str()); P(EXPECTED_RESULT_X);
                }
                ASSERT(oss2.str() == EXPECTED_RESULT_X);
            }

            // Reset the message field to the original string.
            mRecord.fixedFields().setMessage(MSG);
        }

        if (verbose) cout << "\n  Testing \"%u\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%u");
            X(oss1, record);
            oss2 << "string " << 3.14159265l << " " << 1000000;
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERTV(oss1.str(), oss2.str(),
                    compareText(oss1.str(),oss2.str()));
        }

        if (verbose) cout << "\n  Testing \"\\n\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("\\n");  X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == "\n");
        }

        if (verbose) cout << "\n  Testing \"\\t\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("\\t");
            X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == "\t");
        }

        if (verbose) cout << "\n  Testing \"\\\\\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("\\\\");
            X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == "\\");
        }

        if (verbose) cout << "\n  Testing an arbitrary string." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("log this");
            X(oss1, record);
            if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
            ASSERT(oss1.str() == "log this");
        }

        if (verbose) cout << "\n Testing allocation behavior." << endl;
        {

            const char *TEST_MESSAGES[] = {
                MSG_1BYTE,
                MSG_20BYTE,
                MSG_200BYTE,
                MSG_450BYTE,
                MSG_550BYTE,
            };
            const int NUM_TEST_MESSAGES = sizeof(TEST_MESSAGES) /
                                          sizeof(*TEST_MESSAGES);

            for (int i = 0; i < NUM_TEST_MESSAGES; ++i) {
                const char *MSG     = TEST_MESSAGES[i];
                const int   MSG_LEN = bsl::strlen(MSG);

                bslma::TestAllocator oa, da;
                Obj x("%m", &oa); const Obj& X = x;

                ball::RecordAttributes fixedFields(bdlt::Datetime(),
                                                  processID,
                                                  threadID,
                                                  filename,
                                                  lineNum,
                                                  "FOO.BAR.BAZ",
                                                  ball::Severity::e_WARN,
                                                  MSG,
                                                  &oa);
                fixedFields.setTimestamp(bdlt::CurrentTime::utc());

                ball::UserFields   userFields(&oa);
                ball::Record record(fixedFields, userFields);

                bsl::ostringstream stream;
                bslma::DefaultAllocatorGuard guard(&da);

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                X(stream, record);

                bool expectIncrease = MSG_LEN > 500;

                ASSERT(oam.isInUseSame());
                ASSERT(oam.isMaxSame());
                ASSERT(dam.isInUseSame());
                ASSERT(expectIncrease == dam.isMaxUp());

                if (veryVeryVerbose) {
                    P_(oam.isInUseSame());
                    P_(oam.isMaxSame());
                    P_(dam.isInUseSame());
                    P(dam.isMaxUp());
                    P(stream.str());
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTORS:
        //   The three constructors must initialize the value correctly.
        //
        // Testing
        //   ball::RecordStringFormatter(const char *, *ba = 0)
        //   ball::RecordStringFormatter(bdet::DtI, *ba = 0)
        //   ball::RecordStringFormatter(const char *, bdet::DtI, *ba = 0)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    "\n%d %p:%t %s %f:%l %c %x %u\n"          },
            {  L_,    "\n%d %p:%t %s %f:%l %c %X %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        for (int i = 0; i < NUM_FVALUES; ++i) {
            Obj x;  const Obj& X = x;
            x.setFormat(FVALUES[i].d_format);

            Obj y(FVALUES[i].d_format); const Obj& Y = y;
            LOOP_ASSERT(FVALUES[i].d_lineNum, Y == X);
        }

        for (int i = 0; i < NUM_TVALUES; ++i) {
            Obj x;  const Obj& X = x;
            x.setTimestampOffset(
                  bdlt::DatetimeInterval(TVALUES[i].d_days,
                                        TVALUES[i].d_hours,
                                        TVALUES[i].d_mins,
                                        TVALUES[i].d_secs,
                                        TVALUES[i].d_msecs));

            Obj y(bdlt::DatetimeInterval(TVALUES[i].d_days,
                                        TVALUES[i].d_hours,
                                        TVALUES[i].d_mins,
                                        TVALUES[i].d_secs,
                                        TVALUES[i].d_msecs));
            const Obj& Y = y;
            LOOP_ASSERT(TVALUES[i].d_lineNum, Y == X);
        }

        for (int i = 0; i < NUM_FVALUES; ++i) {
            for (int j = 0; j < NUM_TVALUES; ++j) {

                Obj x;  const Obj& X = x;
                x.setFormat(FVALUES[i].d_format);
                x.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[j].d_days,
                                            TVALUES[j].d_hours,
                                            TVALUES[j].d_mins,
                                            TVALUES[j].d_secs,
                                            TVALUES[j].d_msecs));

                Obj y(FVALUES[i].d_format,
                      bdlt::DatetimeInterval(TVALUES[j].d_days,
                                            TVALUES[j].d_hours,
                                            TVALUES[j].d_mins,
                                            TVALUES[j].d_secs,
                                            TVALUES[j].d_msecs));
                const Obj& Y = y;
                LOOP2_ASSERT(FVALUES[i].d_lineNum,
                             TVALUES[j].d_lineNum, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   const ball::RSF& operator=(const ball::RSF& other);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj v;  const Obj& V = v;
                v.setFormat(FVALUES[i1].d_format);
                v.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                            TVALUES[i2].d_hours,
                                            TVALUES[i2].d_mins,
                                            TVALUES[i2].d_secs,
                                            TVALUES[i2].d_msecs));

                for (int j1 = 0; j1 < NUM_FVALUES; ++j1) {
                    for (int j2 = 0; j2 < NUM_TVALUES; ++j2) {
                        Obj u;  const Obj& U = u;
                        u.setFormat(FVALUES[j1].d_format);
                        u.setTimestampOffset(
                               bdlt::DatetimeInterval(TVALUES[j2].d_days,
                                                     TVALUES[j2].d_hours,
                                                     TVALUES[j2].d_mins,
                                                     TVALUES[j2].d_secs,
                                                     TVALUES[j2].d_msecs));

                        Obj w(V);  const Obj &W = w;          // control
                        u = V;
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum, W == U);
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum, W == V);
                    }
                }
            }
        }

        // Testing assignment u = u (Aliasing).
        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj u;  const Obj& U = u;
                u.setFormat(FVALUES[i1].d_format);
                u.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                            TVALUES[i2].d_hours,
                                            TVALUES[i2].d_mins,
                                            TVALUES[i2].d_secs,
                                            TVALUES[i2].d_msecs));
                Obj w(U);  const Obj &W = w;
                u = u;
                LOOP2_ASSERT(FVALUES[i1].d_lineNum,
                             TVALUES[i2].d_lineNum, W == U);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g':
        //   Void for 'ball::RecordStringFormatter'.
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   ball::RecordStringFormatter(const ball::RSF&, *ba = 0);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
            int         d_days;
            int         d_hrs;
            int         d_mins;
            int         d_secs;
            int         d_msecs;
        } VALUES[] = {
            //line fmt  days hrs mins secs msecs
            //---- ---  ---- --- ---- ---- -----
            { L_, "%%",  0,   0,   0,   0,   0   },
            { L_, "%d", -1,  -1,  -1,  -1,   -1  },
            { L_, "%i",  0,   0,   0,   0,  999  },
            { L_, "%p",  0,   0,   0,   0, -999  },
            { L_, "%t",  0,   0,  59,   0,   0   },
            { L_, "%s",  0,   0,   0, -59,   0   },
            { L_, "%f",  0,   0,  59,   0,   0   },
            { L_, "%l",  0,   0, -59,   0,   0   },
            { L_, "%c",  0,  23,   0,   0,   0   },
            { L_, "%m",  0, -23,   0,   0,   0   },
            { L_, "%u",  5,   0,   0,   0,   0   },
            { L_, "",   -5,   0,   0,   0,   0   },
            { L_, "",    5,  23,  22,  21,  209  },
            { L_, "",   -5, -23, -59, -59, -999  },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                 VALUES[i].d_hrs,
                                                 VALUES[i].d_mins,
                                                 VALUES[i].d_secs,
                                                 VALUES[i].d_msecs);

            Obj w;  const Obj& W = w;           // control
            w.setFormat(VALUES[i].d_format);
            w.setTimestampOffset(interval);

            Obj x;  const Obj& X = x;
            x.setFormat(VALUES[i].d_format);
            x.setTimestampOffset(interval);

            Obj y(X);  const Obj &Y = y;
            LOOP_ASSERT(VALUES[i].d_lineNum, X == W);
            LOOP_ASSERT(VALUES[i].d_lineNum, Y == W);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const ball::RSF& lhs, const ball::RSF& rhs);
        //   bool operator!=(const ball::RSF& lhs, const ball::RSF& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj mA;  const Obj& A = mA;
                mA.setFormat(FVALUES[i1].d_format);
                mA.setTimestampOffset(
                       bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                             TVALUES[i2].d_hours,
                                             TVALUES[i2].d_mins,
                                             TVALUES[i2].d_secs,
                                             TVALUES[i2].d_msecs));

                for (int j1 = 0; j1 < NUM_FVALUES; ++j1) {
                    for (int j2 = 0; j2 < NUM_TVALUES; ++j2) {
                        Obj mB;  const Obj& B = mB;
                        mB.setFormat(FVALUES[j1].d_format);
                        mB.setTimestampOffset(
                               bdlt::DatetimeInterval(TVALUES[j2].d_days,
                                                     TVALUES[j2].d_hours,
                                                     TVALUES[j2].d_mins,
                                                     TVALUES[j2].d_secs,
                                                     TVALUES[j2].d_msecs));

                        int isSame = (i1 == j1) && (i2 == j2);
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum,
                                     isSame == (A == B));
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum,
                                     !isSame == (A != B));
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum,
                                     isSame == (B == A));
                        LOOP4_ASSERT(FVALUES[i1].d_lineNum,
                                     TVALUES[i2].d_lineNum,
                                     FVALUES[j1].d_lineNum,
                                     TVALUES[j2].d_lineNum,
                                     !isSame == (B != A));
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator is trivially implemented using the
        //   fully-tested 'bsl::string' and 'bdlt::DatetimeInterval' output
        //   operators; a very few test vectors can thoroughly test this
        //   functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::RSF&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_format;    // format string
            bool        d_localTime; // format in local time
            const char *d_output;    // expected output format
        } DATA[] = {
            //line fmt localTime expected output
            //---- --- --------- ---------------
            { L_, "%%", true,    "'%%' local-time" },
            { L_, "%",  true,    "'%' local-time"  },
            { L_, "%t", true,    "'%t' local-time" },
            { L_, "",   false,   "'' UTC"          },
            { L_, "\\", false,   "'\\' UTC"        },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i].d_format, DATA[i].d_localTime);  const Obj& X = mX;

            ostringstream os;
            os << X;
            ASSERTV(os.str(),
                    DATA[i].d_output,
                    DATA[i].d_lineNum,
                    os.str() == DATA[i].d_output);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        //   This test case is redundant as it will be identical to case 2
        //   (primary manipulators)
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //
        //   Void for 'ball::RecordStringFormatter'.
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   Setter functions should correctly pass the specified value to the
        //   object.
        //
        // Plan:
        //   First, verify the default constructor by testing the observable
        //   value of the resulting object.
        //
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the primary manipulators
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   ball::RecordStringFormatter(*ba = 0);
        //   void setFormat(const char *format);
        //   void setTimestampOffset(const bdlt::DatetimeInterval& offset);
        //   ~ball::RecordStringFormatter();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\n  Create a default object." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT( 0 == strcmp(F0, X.format()));
            ASSERT(T0 == X.timestampOffset());
        }

        if (verbose) cout << "\n  Testing setFormat and setTimestampOffset."
                          << endl;
        {
            static const struct {
                int         d_lineNum;
                const char *d_format;
                int         d_days;
                int         d_hrs;
                int         d_mins;
                int         d_secs;
                int         d_msecs;
            } VALUES[] = {
                //line fmt  days hrs mins secs msecs
                //---- ---  ---- --- ---- ---- -----
                { L_, "%%",  0,   0,   0,   0,   0   },
                { L_, "%d", -1,  -1,  -1,  -1,   -1  },
                { L_, "%i",  0,   0,   0,   0,  999  },
                { L_, "%p",  0,   0,   0,   0, -999  },
                { L_, "%t",  0,   0,  59,   0,   0   },
                { L_, "%s",  0,   0,   0, -59,   0   },
                { L_, "%f",  0,   0,  59,   0,   0   },
                { L_, "%l",  0,   0, -59,   0,   0   },
                { L_, "%c",  0,  23,   0,   0,   0   },
                { L_, "%m",  0, -23,   0,   0,   0   },
                { L_, "%x",  0, -23,   0,   0,   0   },
                { L_, "%u",  5,   0,   0,   0,   0   },
                { L_, "",   -5,   0,   0,   0,   0   },
                { L_, "",    5,  23,  22,  21,  209  },
                { L_, "",   -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                     VALUES[i].d_hrs,
                                                     VALUES[i].d_mins,
                                                     VALUES[i].d_secs,
                                                     VALUES[i].d_msecs);

                Obj mX;  const Obj& X = mX;
                mX.setFormat(VALUES[i].d_format);
                mX.setTimestampOffset(interval);
                LOOP_ASSERT(VALUES[i].d_lineNum,
                            0 == strcmp(VALUES[i].d_format, X.format()));
                LOOP_ASSERT(VALUES[i].d_lineNum,
                            interval == X.timestampOffset());
            }

            // reverse the order of two set* functions
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                     VALUES[i].d_hrs,
                                                     VALUES[i].d_mins,
                                                     VALUES[i].d_secs,
                                                     VALUES[i].d_msecs);

                Obj mX;  const Obj& X = mX;
                mX.setTimestampOffset(interval);
                mX.setFormat(VALUES[i].d_format);
                LOOP_ASSERT(VALUES[i].d_lineNum,
                            0 == strcmp(VALUES[i].d_format, X.format()));
                LOOP_ASSERT(VALUES[i].d_lineNum,
                            interval == X.timestampOffset());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the direct accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        // 1.  Create a default object x1.          { x1:0 }
        // 2.  Create an object x2 (copy from x1).  { x1:0  x2:0 }
        // 3.  Set x1 to VA.                        { x1:VA x2:0 }
        // 4.  Set x2 to VA.                        { x1:VA x2:VA }
        // 5.  Set x2 to VB.                        { x1:VA x2:VB }
        // 6.  Set x1 to 0.                         { x1:0  x2:VB }
        // 7.  Create an object x3 (with value VC). { x1:0  x2:VB x3:VC }
        // 8.  Create an object x4 (copy from x1).  { x1:0  x2:VB x3:VC x4:0 }
        // 9.  Assign x2 = x1.                      { x1:0  x2:0  x3:VC x4:0 }
        // 10. Assign x2 = x3.                      { x1:0  x2:VC x3:VC x4:0 }
        // 11. Assign x1 = x1 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create a default object x1." << endl;
        Obj mX1;  const Obj& X1 = mX1;
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        ASSERT( 0 == strcmp(F0, X2.format()));
        ASSERT(T0 == X2.timestampOffset());
        ASSERT( 1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        if (verbose) cout << "\n 3. Set x1 to VA." << endl;
        mX1.setFormat(FA);
        mX1.setTimestampOffset(TA);
        ASSERT( 0 == strcmp(FA, X1.format()));
        ASSERT(TA == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VA." << endl;
        mX2.setFormat(FA);
        mX2.setTimestampOffset(TA);
        ASSERT( 0 == strcmp(FA, X2.format()));
        ASSERT(TA == X2.timestampOffset());
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VB." << endl;
        mX2.setFormat(FB);
        mX2.setTimestampOffset(TB);
        ASSERT( 0 == strcmp(FB, X2.format()));
        ASSERT(TB == X2.timestampOffset());
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to 0." << endl;
        mX1.setFormat(F0);
        mX1.setTimestampOffset(T0);
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VC)."
                           << endl;
        Obj mX3(FC, TC);  const Obj& X3 = mX3;
        ASSERT( 0 == strcmp(FC, X3.format()));
        ASSERT(TC == X3.timestampOffset());
        ASSERT( 1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT( 0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT( 0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1);  const Obj& X4 = mX4;
        ASSERT( 0 == strcmp(F0, X4.format()));
        ASSERT(T0 == X4.timestampOffset());
        ASSERT( 1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT( 0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT( 0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT( 1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT( 0 == strcmp(F0, X2.format()));
        ASSERT(T0 == X2.timestampOffset());
        ASSERT( 1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT( 1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT( 0 == strcmp(FC, X2.format()));
        ASSERT(TC == X2.timestampOffset());
        ASSERT( 0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT( 0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT( 0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT( 1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
      } break;

      default:
        {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
        }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
