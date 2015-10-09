// ball_fileobserver2.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_fileobserver2.h>

#include <ball_context.h>
#include <ball_defaultobserver.h>             // for testing only
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_multiplexobserver.h>           // for testing only
#include <ball_recordattributes.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>
#include <ball_userfieldvalue.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmt_threadutil.h>

#include <bdls_filesystemutil.h>
#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_localtimeoffset.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bdlt_currenttime.h>
#include <bslim_testutil.h>

#include <bslstl_stringref.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bdlb_tokenizer.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_stdio.h>  // tempname()

#ifdef BSLS_PLATFORM_OS_UNIX
#include <glob.h>
#include <bsl_c_signal.h>
#include <bsl_c_stdlib.h> //unsetenv
#include <sys/resource.h>
#include <bsl_c_time.h>
#include <unistd.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

// Note: on Windows -> WinGDI.h:#define ERROR 0
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(ERROR)
#undef ERROR
#endif

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] ball::FileObserver(bslma::Allocator)
// [ 1] ~ball::FileObserver()
//
// MANIPULATORS
// [ 1] publish(const ball::Record& record, const ball::Context& context)
// [ 1] void disableFileLogging()
// [ 2] void disableLifetimeRotation()
// [ 2] void disableSizeRotation()
// [ 1] void disablePublishInLocalTime()
// [ 9] void disableTimeIntervalRotation();
// [ 1] int enableFileLogging(const char *fileName, bool timestampFlag = false)
// [ 1] void enablePublishInLocalTime()
// [ 1] void publish(const ball::Record& record, const ball::Context& context)
// [ 2] void forceRotation()
// [ 2] void rotateOnSize(int size)
// [ 9] void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
// [ 2] void rotateOnLifetime(bdlt::DatetimeInterval timeInterval)
// [10] void rotateOnTimeInterval(const DtInterval& i, const Datetime& r);
// [ 1] void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
// [ 3] void setMaxLogFiles();
// [ 6] void setOnFileRotationCallback(const OnFileRotationCallback&);
// [ 3] int removeExcessLogFiles();
//
// ACCESSORS
// [ 1] bool isFileLoggingEnabled() const
// [ 1] bool isPublishInLocalTimeEnabled() const
// [ 2] bdlt::DatetimeInterval rotationLifetime() const
// [ 2] int rotationSize() const
// [ 3] int maxLogFiles() const;
//-----------------------------------------------------------------------------
// [ 5] CONCERN: Rotated log filenames are as expected
// [ 8] CONCERN: 'rotateOnSize' triggers correctly for existing files
// [ 7] CONCERN: Rotation on size is based on file size
// [12] CONCERN: Published Records Show Current Local-Time Offset

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
//                     ASSERT2 (for cerr reporting)
// ----------------------------------------------------------------------------

static void aSsErT2(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT2(X) { aSsErT2(!(X), #X, __LINE__); }

//-----------------------------------------------------------------------------

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef ball::FileObserver2  Obj;
typedef bdls::FilesystemUtil FileUtil;
//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

bsl::string::size_type replaceSecondSpace(bsl::string *s, char value)
    // Replace the second space character (' ') in the specified 'string' with
    // the specified 'value'.  Return the index position of the character that
    // was replaced on success, and 'bsl::string::npos' otherwise.
{
    bsl::string::size_type index = s->find(' ');
    if (bsl::string::npos != index) {
        index = s->find(' ', index + 1);
        if (bsl::string::npos != index) {
            (*s)[index] = value;
        }
    }
    return index;
}

bdlt::Datetime getCurrentLocalTime()
{
    time_t currentTime = time(0);
    struct tm localtm;
#ifdef BSLS_PLATFORM_OS_WINDOWS
    localtm = *localtime(&currentTime);
#else
    localtime_r(&currentTime, &localtm);
#endif

    bdlt::Datetime stamp;
    bdlt::DatetimeUtil::convertFromTm(&stamp, localtm);
    return stamp;
}

void logRecord1(bsl::ostream& stream, const ball::Record& record)
{
    ball::Severity::Level severityLevel =
        (ball::Severity::Level) record.fixedFields().severity();

    stream << '\n';

    stream << record.fixedFields().timestamp()      << " ";
    stream << record.fixedFields().processID()      << ":";
    stream << record.fixedFields().threadID()       << " ";
    stream << ball::Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().fileName()       << ":";
    stream << record.fixedFields().lineNumber()     << " ";
    stream << record.fixedFields().category()       << " ";
    stream << record.fixedFields().message ()       << " ";

    ball::UserFields userFields = record.userFields();
    for (int i = 0; i < userFields.length(); ++i)
    {
        stream << userFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
}

void logRecord2(bsl::ostream& stream, const ball::Record& record)
{
    ball::Severity::Level severityLevel =
        (ball::Severity::Level) record.fixedFields().severity();

    stream << '\n';

    stream << record.fixedFields().message ()       << " ";
    stream << record.fixedFields().category()       << " ";
    stream << record.fixedFields().fileName()       << ":";
    stream << ball::Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().threadID()       << " ";
    stream << record.fixedFields().processID()      << ":";

    ball::UserFields userFields = record.userFields();
    for (int i = 0; i < userFields.length(); ++i)
    {
        stream << userFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
}

bsl::string tempFileName(bool verboseFlag)
{
    bsl::string result;
#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "ball", 0, tmpNameBuf);
    result = tmpNameBuf;
#elif defined(BSLS_PLATFORM_OS_HPUX)
    char tmpPathBuf[L_tmpnam];
    result = tempnam(tmpPathBuf, "ball");
#else
    char *fn = tempnam(0, "ball");
    result = fn;
    bsl::free(fn);
#endif

    if (verboseFlag) cout << "\tUse " << result << " as a base filename."
                          << endl;
    // Test Invariant:
    BSLS_ASSERT(!result.empty());
    return result;
}


void removeFilesByPrefix(const char *prefix)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    glob_t globbuf;
    bsl::string filename = prefix;
    filename += "*";
    glob(filename.c_str(), 0, 0, &globbuf);
    for (int i = 0; i < (int)globbuf.gl_pathc; i++)
        unlink(globbuf.gl_pathv[i]);
    globfree(&globbuf);
#endif
}

int readFileIntoString(int                lineNum,
                       const bsl::string& filename,
                       bsl::string&       fileContent)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    glob_t globbuf;
    LOOP_ASSERT(lineNum, 0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
    LOOP_ASSERT(lineNum, 1 == globbuf.gl_pathc);

    bsl::ifstream fs;
    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
    globfree(&globbuf);
    LOOP_ASSERT(lineNum, fs.is_open());
    fileContent = "";
    bsl::string lineContent;
    int lines = 0;
    while (getline(fs, lineContent))
    {
        fileContent += lineContent;
        fileContent += '\n';
        lines++;
    }
    fs.close();
    //bsl::cerr << "number of lines: " << lines << endl;
    return lines;
#else
    bsl::ifstream fs;
    fs.open(filename.c_str(), bsl::ifstream::in);
    LOOP_ASSERT(lineNum, fs.is_open());
    fileContent = "";
    bsl::string lineContent;
    int lines = 0;
    while (getline(fs, lineContent))
    {
        fileContent += lineContent;
        fileContent += '\n';
        lines++;
    }
    fs.close();
    //bsl::cerr << "number of lines: " << lines << endl;
    return lines;
#endif
}

class LogRotationCallbackTester {
    // This class can be used as a functor matching the signature of
    // 'ball::FileObserver2::OnFileRotationCallback'.  This class records every
    // invocation of the function-call operator, and is intended to test
    // whether 'ball::FileObserver2' calls the log-rotation callback
    // appropriately.

    // PRIVATE TYPES
    struct Rep {
        int         d_invocations;
        int         d_status;
        bsl::string d_rotatedFileName;

        explicit Rep(bslma::Allocator *allocator)
        : d_invocations(0)
        , d_status(0)
        , d_rotatedFileName(allocator)
        {
        }

      private:
        // NOT IMPLEMENTED
        Rep(const Rep&);
        Rep& operator=(const Rep&);
    };

    // DATA
    bsl::shared_ptr<Rep> d_rep;

  public:
    // PUBLIC CONSTANTS

    enum {
        UNINITIALIZED = INT_MIN
    };

    explicit LogRotationCallbackTester(bslma::Allocator *allocator)
        // Create a callback tester that will use the specified 'status' and
        // 'logFileName' to record the arguments to the function call
        // operator.  Set '*status' to 'UNINITIALIZED' and set '*logFileName'
        // to the empty string.
    : d_rep()
    {
        d_rep.createInplace(allocator, allocator);
        reset();
    }

    void operator()(int                status,
                    const bsl::string& rotatedFileName)
        // Set the value at the status address supplied at construction to the
        // specified 'status', and set the value at the log file name address
        // supplied at construction to the specified 'logFileName'.
    {
        ++d_rep->d_invocations;
        d_rep->d_status          = status;
        d_rep->d_rotatedFileName = rotatedFileName;

    }

    void reset()
        // Set '*status' to 'UNINITIALIZED' and set '*logFileName' to the
        // empty string.
    {
        d_rep->d_invocations     = 0;
        d_rep->d_status          = UNINITIALIZED;
        d_rep->d_rotatedFileName = "";

    }

    // ACCESSORS
    int numInvocations() const { return d_rep->d_invocations; }
        // Return the number of times that the function-call operator has been
        // invoked since the most recent call to 'reset', or if 'reset' has
        // not been called, since this objects construction.

    int status() const { return d_rep->d_status; }
        // Return the status passed to the most recent invocation of the
        // function-call operation, or 'UNINITIALIZED' if 'numInvocations' is
        // 0.

    const bsl::string& rotatedFileName() const
        // Return a reference to the non-modifiable file name supplied to the
        // most recent invocation of the function-call operator, or the empty
        // string if 'numInvocations' is 0.
    {
        return d_rep->d_rotatedFileName;
    }

};

typedef LogRotationCallbackTester RotCb;

class ReentrantRotationCallback {
    // This class can be used as a functor matching the signature of
    // 'ball::FileObserver2::OnFileRotationCallback'.  This class implements
    // the function-call operator, that will call 'disableFileLogging' on the
    // file observer supplied at construction.  Note that this type is intended
    // to test whether the rotation callback is called in a way that allows the
    // supplied file observer to be modified on the callback.

    Obj *d_observer_p;

  public:

    explicit ReentrantRotationCallback(Obj *observer)
    : d_observer_p(observer)
    {
    }

    void operator()(int                status,
                    const bsl::string& rotatedFileName);
        // Set the value at the status address supplied at construction to the
        // specified 'status', and set the value at the log file name address
        // supplied at construction to the specified 'logFileName'.
};

void ReentrantRotationCallback::operator()(int                status,
                                           const bsl::string& rotatedFileName)
{
    d_observer_p->disableFileLogging();
}

void publishRecord(Obj *mX, const char *message)
{
    ball::RecordAttributes attr(bdlt::CurrentTime::utc(),
                               1,
                               2,
                               "FILENAME",
                               3,
                               "CATEGORY",
                               32,
                               message);
    ball::Record record(attr, ball::UserFields());

    ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);

    mX->publish(record, context);
}


int getNumLines(const char *filename)
{
    bsl::ifstream fs;
    fs.open(filename, bsl::ifstream::in);
    fs.clear();
    ASSERT(fs.is_open());

    int numLines = 0;

    bsl::string line;
    while (getline(fs, line)) {
        ++numLines;
    }
    fs.close();
    return numLines;
}

bsls::Types::Int64 getFileSize(const char *filename)
{
    bsl::ifstream fs;
    fs.open(filename, bsl::ifstream::in);
    fs.clear();
    ASSERT(fs.is_open());

    bsls::Types::Int64 fileSize = 0;

    bsl::string line;
    while (getline(fs, line)) {
        fileSize += line.length() + 1;
    }
    fs.close();
    return fileSize;
}

struct TestCurrentTimeCallback {
  private:
    // DATA
    static bsls::TimeInterval s_utcTime;

  public:
    // CLASS METHODS
    static bsls::TimeInterval load();
        // Return the value corresponding to the most recent call to the
        // 'setTimeToReport' method.  The behavior is undefined unless
        // 'setUtcTime' has been called.

    static void setUtcDatetime(const bdlt::Datetime& utcTime);
        // Set the specified 'utcTime' as the value obtained (after conversion
        // to 'bdlt::IntervalConversionUtil') from calls to the 'load' method.
        // The behavior is undefined unless
        // 'bdlt::EpochUtil::epoch() <= utcTime'.
};

bsls::TimeInterval TestCurrentTimeCallback::s_utcTime;

bsls::TimeInterval TestCurrentTimeCallback::load()
{
    return s_utcTime;
}

void TestCurrentTimeCallback::setUtcDatetime(const bdlt::Datetime &utcTime)
{
    ASSERT(bdlt::EpochUtil::epoch() <= utcTime);

    int rc = bdlt::EpochUtil::convertToTimeInterval(&s_utcTime, utcTime);
    ASSERT(0 == rc);
}

struct TestLocalTimeOffsetCallback {
  private:
    // DATA
    static int s_localTimeOffsetInSeconds;
    static int s_loadCount;

  public:
    // CLASS METHODS
    static bsls::TimeInterval loadLocalTimeOffset(
                                           const bdlt::Datetime&  utcDatetime);
        // Return the local time offset that was set by the previous call to
        // the 'setLocalTimeOffset' method.  If the 'setLocalTimeOffset' method
        // has not been called, load 0.  Note that the specified 'utcDateime'
        // is ignored.

    static void setLocalTimeOffset(int localTimeOffsetInSeconds);
        // Set the specified 'localTimeOffsetInSeconds' as the value loaded by
        // calls to the loadLocalTimeOffset' method.

    static int loadCount();
        // Return the number of times the 'loadLocalTimeOffset' method has been
        // called since the start of process.
};

int TestLocalTimeOffsetCallback::s_localTimeOffsetInSeconds = 0;
int TestLocalTimeOffsetCallback::s_loadCount                = 0;

bsls::TimeInterval TestLocalTimeOffsetCallback::loadLocalTimeOffset(
                                            const bdlt::Datetime&  utcDatetime)
{
    ++s_loadCount;
    return bsls::TimeInterval(s_localTimeOffsetInSeconds);
}

void TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                  int localTimeOffsetInSeconds)
{
    s_localTimeOffsetInSeconds = localTimeOffsetInSeconds;
}

int TestLocalTimeOffsetCallback::loadCount()
{
    return s_loadCount;
}

void splitStringIntoLines(bsl::vector<bsl::string> *result,
                          const char               *ascii)
{
    ASSERT(result);
    ASSERT(ascii);

    for (bdlb::Tokenizer itr(bslstl::StringRef(ascii),
                             bslstl::StringRef(""),
                             bslstl::StringRef("\n")); itr.isValid(); ++itr) {
        if (itr.token().length() > 0) {
            result->push_back(itr.token());
        }
    }
}

void getDatetimeField(bsl::string       *result,
                      const bsl::string& filename,
                      int                recordNumber)
{
    ASSERT(1 <= recordNumber);

    bsl::string fileContent;
    int lineCount = readFileIntoString(__LINE__, filename, fileContent);
    ASSERT(recordNumber * 2 <= lineCount);

    bsl::vector<bsl::string> lines;
    splitStringIntoLines(&lines, fileContent.c_str());
    int recordIndex = recordNumber  - 1;
    ASSERT(0            <= recordIndex);
    ASSERT(lines.size() >  recordIndex);

    const bsl::size_t dateFieldLength = bsl::strlen("23DEC2013_16:40:44.052");
    *result = lines[recordIndex].substr(0, dateFieldLength);
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator allocator("Test", veryVeryVeryVerbose);
    bslma::TestAllocator *Z = &allocator;
    bslma::TestAllocator defaultAllocator("Default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: Published Records Show Current Local-Time Offset
        //   Per DRQS 13681097, log records observe DST time transitions when
        //   the default logging functor is used and the 'publishInLocalTime'
        //   attribute is 'true'.
        //
        // Concern:
        //: 1 Log records show the current local time offset (possibly
        //:   different from the local time offset in effect on construction),
        //:   when 'true == isPublishInLocalTimeEnabled()'.
        //:
        //: 2 Log records show UTC when
        //:   'false == isPublishInLocalTimeEnabled()'.
        //:
        //: 3 QoI: The local-time offset is obtained not more than once per log
        //:   record.
        //:
        //: 4 The helper class 'TestCurrentTimeCallback' has a method, 'load',
        //:   that loads the user-specified UTC time, and that method can be
        //:   installed as the system-time callback of 'bdlt::CurrentTime'.
        //:
        //: 5 The helper class 'TestLocalTimeOffsetCallback' has a method,
        //:   'loadLocalTimeOffset', that loads the user-specified local-time
        //:   offset value, and that method can be installed as the local-time
        //:   callback of 'bdlt::CurrentTime', and that the value loaded is not
        //:   influenced by the user-specified 'utcDatetime'.
        //:
        //: 6 The helper class method 'TestLocalTimeOffsetCallback::loadCount'
        //:   provides an accurate count of the calls to the
        //:   'TestLocalTimeOffsetCallback::loadLocalTimeOffset' method.
        //
        // Plan:
        //: 1 Test the helper 'TestCurrentTimeCallback' class (C-4):
        //:
        //:   1 Using the array-driven technique, confirm that the 'load'
        //:     method obtains the value last set by the 'setUtcDatetime'
        //:     method.  Use UTC values that do not coincide with the actual
        //:     UTC datetime.
        //:
        //:   2 Install the 'TestCurrentTimeCallback::load' method as the
        //:     system-time callback of system-time offset callback of
        //:     'bdlt::CurrentTime', and run through the same values as used in
        //:     P-1.1.  Confirm that values returned from 'bdlt::CurrentTime'
        //:     match the user-specified values.
        //:
        //: 2 Test the helper 'TestLocalTimeOffsetCallback' class (C-5):
        //:
        //:   1 Using the array-driven technique, confirm that the
        //:     'loadLocalTimeOffset' method obtains the value last set by the
        //:     'setLocalTimeOffset' method.  At least one value should differ
        //:     from the current, actual local-time offset.
        //:
        //:   2 Install the 'TestLocalTimeOffsetCallback::loadLocalTimeOffset'
        //:     method as the local-time offset callback of
        //:     'bdlt::CurrentTime', and run through the same same
        //:     user-specified local time offsets as used in P-2.1.  Confirm
        //:     that values returned from 'bdlt::CurrentTime' match the
        //:     user-specified values.  Repeat the request for (widely)
        //:     different UTC datetime values to confirm that the local time
        //:     offset value remains that defined by the callback.
        //:
        //:   3 Confirm that the value returned by the 'loadCount' method
        //:     increases by exactly 1 each time a local-time offset is
        //:     obtained via 'bdlt::CurrentTime'.  (C-6)
        //:
        //: 3 Using an ad-hoc approach, confirm that the datetime field of a
        //:   published log record is the expected (arbitrary) UTC datetime
        //:   value when publishing in local-time is disabled.  Enable
        //:   publishing in local-time and confirm that the published datetime
        //:   field matches that of the (arbitrary) user-defined local-time
        //:   offsets.  Disable publishing in local time, and confirm that log
        //:   records are again published with the UTC datetime.  (C-1, C-2)
        //:
        //: 4 When publishing in local time is enabled, confirm that that there
        //:   exactly 1 request for local time offset for each published
        //:   record.  (C-3);
        // --------------------------------------------------------------------

        if (verbose) cout
         << endl
         << "TESTING: Published Records Show Current Local-Time Offset" <<endl
         << "=========================================================" <<endl;

        const bdlt::Datetime UTC_ARRAY[] = { bdlt::EpochUtil::epoch(),
                                        bdlt::Datetime(2001,
                                                         9,
                                                        11,
                                                     8 + 4, // UTC
                                                        46,
                                                        30,
                                                         0),
                                        bdlt::Datetime(9999,
                                                        12,
                                                        31,
                                                        23,
                                                        59,
                                                        59,
                                                       999)
                                      };
        const int NUM_UTC_ARRAY = sizeof UTC_ARRAY / sizeof *UTC_ARRAY;

        if (verbose) cout << "\nTest TestCurrentTimeCallback: Direct" << endl;
        {
            for (int i = 0; i < NUM_UTC_ARRAY; ++i) {
                bdlt::Datetime utcDatetime = UTC_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(utcDatetime) }

                TestCurrentTimeCallback::setUtcDatetime(utcDatetime);
                bsls::TimeInterval result = TestCurrentTimeCallback::load();

                bdlt::Datetime resultAsDatetime =
                              bdlt::EpochUtil::convertFromTimeInterval(result);
                LOOP_ASSERT(i, utcDatetime == resultAsDatetime);
            }
        }

        if (verbose) cout << "\nTest TestCurrentTimeCallback: Installed"
                          << endl;
        {
            // Install callback from 'TestCurrentTimeCallback'.

            bdlt::CurrentTime::CurrentTimeCallback
                originalCurrentTimeCallback =
                                     bdlt::CurrentTime::setCurrentTimeCallback(
                                         &TestCurrentTimeCallback::load);

            for (int i = 0; i < NUM_UTC_ARRAY; ++i) {
                bdlt::Datetime utcDatetime = UTC_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(utcDatetime) }

                TestCurrentTimeCallback::setUtcDatetime(utcDatetime);

                bdlt::Datetime result1 = bdlt::CurrentTime::utc();
                bslmt::ThreadUtil::microSleep(0, 2); // two seconds
                bdlt::Datetime result2 = bdlt::CurrentTime::utc();

                LOOP_ASSERT(i, utcDatetime == result1);
                LOOP_ASSERT(i, result2     == result1);
            }

           // Restore original system-time callback.

            bdlt::CurrentTime::setCurrentTimeCallback(
                                                  originalCurrentTimeCallback);
        }

        const int     LTO_ARRAY[] = { -86399, -1, 0, 1, 86399 };
        const int NUM_LTO_ARRAY   = sizeof LTO_ARRAY / sizeof *LTO_ARRAY;

        int loadCount = TestLocalTimeOffsetCallback::loadCount();
        ASSERT(0 ==  loadCount);

        if (verbose) cout << "\nTest TestLocalTimeOffsetCallback: Direct"
                          << endl;
        {
            for (int i = 0; i < NUM_LTO_ARRAY; ++i) {
                int localTimeOffset = LTO_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(localTimeOffset) }

                TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                              localTimeOffset);
                for (int j = 0; j < NUM_UTC_ARRAY; ++j) {
                    bdlt::Datetime utcDatetime  = UTC_ARRAY[j];

                    if (veryVerbose) { T_ T_ P_(j) P(utcDatetime) }

                    int result =
                        TestLocalTimeOffsetCallback::loadLocalTimeOffset(
                                                               utcDatetime)
                                                               .totalSeconds();
                    ++loadCount;

                    LOOP2_ASSERT(i, j, localTimeOffset == result);
                    LOOP2_ASSERT(i, j, loadCount       ==
                                     TestLocalTimeOffsetCallback::loadCount());
                }
            }

        }

        if (verbose) cout << "\nTest TestLocalTimeOffsetCallback: Installed"
                          << endl;
        {
            bdlt::LocalTimeOffset::LocalTimeOffsetCallback
                originalLocalTimeOffsetCallback =
                        bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                            &TestLocalTimeOffsetCallback::loadLocalTimeOffset);

            for (int i = 0; i < NUM_LTO_ARRAY; ++i) {
                int localTimeOffset = LTO_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(localTimeOffset) }

                TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                              localTimeOffset);
                for (int j = 0; j < NUM_UTC_ARRAY; ++j) {
                    bdlt::Datetime utcDatetime  = UTC_ARRAY[j];

                    if (veryVerbose) { T_ T_ P_(j) P(utcDatetime) }

                    int result =
                        bdlt::LocalTimeOffset::localTimeOffset(utcDatetime)
                                                               .totalSeconds();
                    ++loadCount;

                    LOOP2_ASSERT(i, j, localTimeOffset == result);
                    LOOP2_ASSERT(i, j, loadCount       ==
                                     TestLocalTimeOffsetCallback::loadCount());
                }
            }

            bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                              originalLocalTimeOffsetCallback);
        }

        if (verbose) cout << "\nTest Logger" << endl;

        if (veryVerbose) cout << "\tConfigure Logger and Callbacks" << endl;

        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        ball::LoggerManager::initSingleton(&mX, configuration);

        bsl::string BASENAME = tempFileName(veryVerbose);
        if (veryVeryVerbose) { T_; T_; P(BASENAME); }
        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str(), true));

        bsl::string logfilename;
        ASSERT(X.isFileLoggingEnabled(&logfilename));
        if (veryVeryVerbose) { T_; T_; P(logfilename); }

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        int                 logRecordCount  = 0;
        int                 testLocalTimeOffsetInSeconds;
        bsl::string         datetimeField;
        bsl::ostringstream  expectedDatetimeField;
        const bdlt::Datetime testUtcDatetime = UTC_ARRAY[1];

        bdlt::CurrentTime::CurrentTimeCallback originalCurrentTimeCallback =
                                     bdlt::CurrentTime::setCurrentTimeCallback(
                                         &TestCurrentTimeCallback::load);
        TestCurrentTimeCallback::setUtcDatetime(testUtcDatetime);

        bdlt::LocalTimeOffset::LocalTimeOffsetCallback
            originalLocalTimeOffsetCallback =
                        bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                            &TestLocalTimeOffsetCallback::loadLocalTimeOffset);

        int expectedLoadCount = TestLocalTimeOffsetCallback::loadCount();

        if (veryVerbose) cout << "\tLog with Publish In Local Time Disabled"
                              << endl;

        ASSERT(!X.isPublishInLocalTimeEnabled());

        BALL_LOG_TRACE << "log 1" << BALL_LOG_END; ++logRecordCount;
        getDatetimeField(&datetimeField, logfilename, logRecordCount);
        expectedDatetimeField.str("");
        expectedDatetimeField << testUtcDatetime;
        if (veryVerbose) { T_
                           P_(expectedDatetimeField.str())
                           P(datetimeField) }
        ASSERT(expectedDatetimeField.str() == datetimeField);
        ASSERT(expectedLoadCount           ==
                                     TestLocalTimeOffsetCallback::loadCount());

        BALL_LOG_TRACE << "log 2" << BALL_LOG_END; ++logRecordCount;
        getDatetimeField(&datetimeField, logfilename, logRecordCount);
        expectedDatetimeField.str("");
        expectedDatetimeField << testUtcDatetime;
        if (veryVerbose) { T_
                           P_(expectedDatetimeField.str())
                           P(datetimeField) }
        ASSERT(expectedDatetimeField.str() == datetimeField);
        ASSERT(expectedLoadCount           ==
                                     TestLocalTimeOffsetCallback::loadCount());

        if (veryVerbose) cout << "\tLog with Publish In Local Time Enabled"
                              << endl;

        mX.enablePublishInLocalTime();
        ASSERT(X.isPublishInLocalTimeEnabled());

        testLocalTimeOffsetInSeconds = -1 * 60 * 60;
        TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                 testLocalTimeOffsetInSeconds);

        if (veryVerbose) { T_ P(testLocalTimeOffsetInSeconds); }

        BALL_LOG_TRACE << "log 3" << BALL_LOG_END; ++logRecordCount;
                                                   ++expectedLoadCount;
        getDatetimeField(&datetimeField, logfilename, logRecordCount);
        expectedDatetimeField.str("");
        expectedDatetimeField << testUtcDatetime +
                                 bdlt::DatetimeInterval(
                                                  0,
                                                  0,
                                                  0,
                                                  testLocalTimeOffsetInSeconds,
                                                  0);
        if (veryVerbose) { T_
                           P_(expectedDatetimeField.str())
                           P(datetimeField) }
        ASSERT(expectedDatetimeField.str() == datetimeField);
        ASSERT(expectedLoadCount           ==
                                     TestLocalTimeOffsetCallback::loadCount());

        testLocalTimeOffsetInSeconds = -2 * 60 * 60;
        TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                 testLocalTimeOffsetInSeconds);

        if (veryVerbose) { T_ P(testLocalTimeOffsetInSeconds); }

        BALL_LOG_TRACE << "log 4" << BALL_LOG_END; ++logRecordCount;
                                                   ++expectedLoadCount;
        getDatetimeField(&datetimeField, logfilename, logRecordCount);
        expectedDatetimeField.str("");
        expectedDatetimeField << testUtcDatetime +
                                 bdlt::DatetimeInterval(
                                                  0,
                                                  0,
                                                  0,
                                                  testLocalTimeOffsetInSeconds,
                                                  0);
        if (veryVerbose) { T_
                           P_(expectedDatetimeField.str())
                           P(datetimeField) }
        ASSERT(expectedDatetimeField.str() == datetimeField);
        ASSERT(expectedLoadCount           ==
                                     TestLocalTimeOffsetCallback::loadCount());

        mX.disablePublishInLocalTime();
        ASSERT(!X.isPublishInLocalTimeEnabled());

        BALL_LOG_TRACE << "log 5" << BALL_LOG_END; ++logRecordCount;
                                                // ++expectedLoadCount;
        getDatetimeField(&datetimeField, logfilename, logRecordCount);
        expectedDatetimeField.str("");
        expectedDatetimeField << testUtcDatetime;
        if (veryVerbose) { T_
                           P_(expectedDatetimeField.str())
                           P(datetimeField) }
        ASSERT(expectedDatetimeField.str() == datetimeField);
        ASSERT(expectedLoadCount           ==
                                     TestLocalTimeOffsetCallback::loadCount());

        if (veryVerbose) cout
                           << "\tLog with Publish In Local Time Disabled Again"
                           << endl;

        if (veryVerbose) cout << "\tCleanup" << endl;

        bdlt::CurrentTime::setCurrentTimeCallback(originalCurrentTimeCallback);
        bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                              originalLocalTimeOffsetCallback);

        mX.disableFileLogging();
        FileUtil::remove(logfilename.c_str());

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'rotateOnTimeInterval' after 'enableFileLogging'
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' does not cause rotation immediately if
        //:   invoked after 'enableFileLogging'.
        //
        // Plan:
        //: 1 Invoke 'enableFileLogging' before 'rotateOnTimeInterval' and
        //:   verify rotation does not occur.
        // --------------------------------------------------------------------
        if (verbose) cout
                << "\nTESTING 'rotateOnTimeInterval' after 'enableFileLogging'"
                << "\n========================================================"
                << endl;

        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        mX.rotateOnTimeInterval(bdlt::DatetimeInterval(1));

        BALL_LOG_TRACE << "log" << BALL_LOG_END;

        ASSERT(0 == cb.numInvocations());

        mX.disableFileLogging();
        FileUtil::remove(BASENAME.c_str());

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING TIME-BASED ROTATION
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' accepts any valid datetime value as the
        //:   reference time.
        //:
        //: 2 Reference time is a local time.
        //
        // Plan:
        //: 1 Setup test infrastructure.
        //:
        //: 2 Call 'rotateOnTimeInterval' with boundary values on the reference
        //:   start time, and verify that rotation occurs on schedule.  (C-1)
        //:
        //: 3 Call 'rotateOnTimeInterval' with a large interval and a reference
        //:   time such that the next rotation will occur soon.  Verify that
        //:   rotation occurs on the scheduled time.  (C-2)
        //
        // Testing:
        //  void rotateOnTimeInterval(const DtInterval& i, const Datetime& r);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Time-Based Rotation"
                          << "\n===========================" << endl;

        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);

        mX.setOnFileRotationCallback(cb);
        mX.enablePublishInLocalTime();
        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        if (veryVerbose) cout <<
                         "Test lower bound of absolute time reference" << endl;
        {
            mX.disableFileLogging();

            // Ensure log file did not exist

            FileUtil::remove(BASENAME.c_str());

            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 2),
                                    bdlt::Datetime(1, 1, 1));

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            bslmt::ThreadUtil::microSleep(0, 2);
            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));

            cb.reset();
        }

        if (veryVerbose) cout <<
                         "Test upper bound of absolute time reference" << endl;
        {
            mX.disableFileLogging();

            // Ensure log file did not exist

            FileUtil::remove(BASENAME.c_str());

            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 2),
                                    bdlt::Datetime(9999, 12, 31, 23, 59, 59));

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            bslmt::ThreadUtil::microSleep(0, 2);
            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));

            cb.reset();
        }

        if (veryVerbose) cout << "Testing absolute time reference" << endl;
        {
            // Reset reference start time.

            mX.disableFileLogging();

            // Ensure log file did not exist

            FileUtil::remove(BASENAME.c_str());

            bdlt::Datetime refTime = bdlt::CurrentTime::local();
            refTime += bdlt::DatetimeInterval(-1, 0, 0, 3);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(1), refTime);
            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

            bslmt::ThreadUtil::microSleep(0, 3);
            BALL_LOG_TRACE << "log" << BALL_LOG_END;


            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING TIME-BASED ROTATION
        //
        // Concern:
        //: 1 Time-based rotation occurs as scheduled
        //:
        //: 2 Rotation schedule is not affected if the previous rotation was
        //:   delayed because not record was published at the scheduled time
        //:   of rotation.
        //:
        //: 3 A rotation that occurred between two scheduled rotations does
        //:   not affect the schedule.
        //:
        //: 4 Disabling file logging does not affect rotation schedule.
        //:
        //: 5 'disableLifetimeRotation' prevents further time-base rotation.
        //
        // Plan:
        //: 1 Setup test infrastructure.
        //:
        //: 2 Set to rotate every 3 seconds.
        //:
        //: 3 Log an record before 3 seconds, and ensure that no rotation
        //:   occurs.  Then log a record after the 3 second and verify an
        //:   rotation occurs.  (C-1)
        //:
        //: 4 Delay logging such that the rotation is delayed.  Then verify the
        //:   schedule for the next rotation is not affected.  (C-2)
        //:
        //: 5 Cause a rotation to occur between scheduled rotations and verify
        //:   that the rotation schedule is not affected.  (C-3)
        //:
        //: 6 Disable and then re-enable file logging and verify rotation
        //:   schedule is not affect.  (C-4)
        //:
        //: 7 Call 'disableTimeIntervalRotation' and verify that time-based
        //:   rotation is disabled.  (C-5)
        //
        // Testing:
        //  void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
        //  void disableTimeIntervalRotation();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Time-Based Rotation"
                          << "\n===========================" << endl;


        if (veryVerbose) cout << "Test infrastructure setup" << endl;

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        Obj mX(&ta); const Obj& X = mX;

        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 3));

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

        ASSERT(1 == FileUtil::exists(BASENAME.c_str()));
        LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

        if (veryVerbose) cout << "Test normal rotation" << endl;
        {
            cb.reset();
            bslmt::ThreadUtil::microSleep(0, 2);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

            bslmt::ThreadUtil::microSleep(0, 1);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Test delayed logging" << endl;
        {
            cb.reset();
            bslmt::ThreadUtil::microSleep(0, 5);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));

            bslmt::ThreadUtil::microSleep(0, 1);
            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 2 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout <<
                           "Test rotation between scheduled rotations" << endl;
        {
            cb.reset();
            bslmt::ThreadUtil::microSleep(0, 1);

            mX.forceRotation();

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));

            bslmt::ThreadUtil::microSleep(0, 2);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 2 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout <<
                       "Test disabling file logging between rotations" << endl;
        {
            cb.reset();
            bslmt::ThreadUtil::microSleep(0, 3);

            mX.disableFileLogging();

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Test 'disableTimeIntervalRotation" << endl;
        {
            cb.reset();
            bslmt::ThreadUtil::microSleep(0, 3);

            mX.disableTimeIntervalRotation();

            BALL_LOG_TRACE << "log" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());
        }

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ROTATE WHEN OPENING EXISTING FILE
        //
        // Concerns:
        //: 1 'rotateOnSize' triggers a rotation as expected even if the log
        //:   file already exist.
        //
        // Plan:
        //: 1 Set 'rotateOnSize' to 1k, create a file with approximately 0.5k.
        //:
        //: 2 Write another 0.5k to the file and verify that that file is
        //:   rotated.  (C-1)
        //
        // Testing:
        //  CONCERN: 'rotateOnSize' triggers correctly for existing files
        // --------------------------------------------------------------------
        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_TRACE,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        ball::LoggerManager::initSingleton(&mX, configuration);

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            const bsl::string BASENAME = tempFileName(veryVerbose);

            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
                mX.rotateOnSize(1);
                ASSERT(X.isFileLoggingEnabled());

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

                ASSERT(1 == FileUtil::exists(BASENAME.c_str()));

                ASSERT(2 == getNumLines(BASENAME.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(0 == cb.numInvocations());
            }

            if (verbose) cout <<
                          "Testing file observer with existing file." << endl;
            {
                char buffer[512];
                memset(buffer, 'x', sizeof buffer);
                buffer[sizeof buffer - 1] = '\0';

                BALL_LOG_TRACE << buffer << BALL_LOG_END;
                mX.disableFileLogging();

                ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

                BALL_LOG_TRACE << 'x' << BALL_LOG_END;

                ASSERT(0 == cb.numInvocations());

                BALL_LOG_TRACE << buffer << BALL_LOG_END;
                BALL_LOG_TRACE << 'x' << BALL_LOG_END;

                ASSERT(1 == FileUtil::exists(BASENAME.c_str()));
                ASSERT(1 == cb.numInvocations());
                ASSERT(1 ==
                         FileUtil::exists(cb.rotatedFileName().c_str()));
            }
        }

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ROTATE ON SIZE ON EXISTING FILE
        //
        // Concerns:
        //: 1 Rotate on size is based on file size and not the number of bytes
        //:   written to the file.
        //
        // Plan:
        //: 1 Disable any rotation and create a file over 1 KB and then disable
        //:   file logging.  Enable 'rotateOnSize', start logging again and
        //:   verify that the file is rotated on first log.  (C-1)
        //
        // Concern:
        //  CONCERN: Rotation on size is based on file size
        // --------------------------------------------------------------------

        if (verbose) cout << "Test-case infrastructure setup." << endl;

        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta); const Obj& X = mX;

        ball::LoggerManager::initSingleton(&mX, configuration);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        {
            const bsl::string BASENAME = tempFileName(veryVerbose);

            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
            ASSERT(X.isFileLoggingEnabled());

            char buffer[1024];
            memset(buffer, 'x', sizeof buffer);
            buffer[sizeof buffer - 1] = '\0';

            BALL_LOG_TRACE << buffer << BALL_LOG_END;

            mX.disableFileLogging();
            ASSERT(!X.isFileLoggingEnabled());

            ASSERT(1024 < getFileSize(BASENAME.c_str()));

            ASSERT(0 == X.rotationSize());
            mX.rotateOnSize(1);
            ASSERT(1 == X.rotationSize());

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
            ASSERT(X.isFileLoggingEnabled());

            BALL_LOG_TRACE << "x" << BALL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            LOOP_ASSERT(cb.status(), 0 == cb.status());

            ASSERT(1 == FileUtil::exists(cb.rotatedFileName()));
        }

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING Log file rotation callback
        //
        // Concerns:
        //: 1 Rotation callback is invoked when a rotation occurs, regardless
        //:   of the event that causes the rotation.
        //:
        //: 2 Setting a callback is not necessary for file rotation whether the
        //:   rotation succeed or not.
        //:
        //: 3 Rotation callback is *not* invoked if a rotation did not occur.
        //:
        //: 4 Rotation callback supply the name of the rotated log file.
        //:
        //: 5 Rotation callback is invoked in such a way that the file observer
        //:   is not locked during the callback.
        //
        // Plan:
        //: 1 Call 'forceRotation' without setting the callback. (C-2)
        //:
        //: 2 Call 'forceRotation' when logging is not enable and verify that
        //:   the callback is not invoked. (C-3)
        //:
        //: 3 Setup the test callback function.  Call 'enableFileLogging' with
        //:   a pattern that produces unique log names.  Call 'forceRotation'
        //:   and verify that the callback is invoked and the rotated log name
        //:   is as expected.
        //:
        //: 4 Setup the test callback function.  Call 'enableFileLogging' with
        //:   a pattern that produces logs with the same names.  Call
        //:   'forceRotation' and verify that the callback is invoked and the
        //:   rotated log name is changed.
        //:
        //: 5 Setup the test callback function.  Cause a rotation-on-size and
        //:   rotation-on-time-interval, and verify that the callback is
        //:   invoked.
        //:
        //: 6 Set file rotation callback with a function that will call
        //:   'disableFileLogging'.  Call 'forceRotation' and verify it does
        //:   not result in a deadlock.
        //
        // Testing:
        //    void setOnFileRotationCallback(const OnFileRotationCallback&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Log Rotation Callback"
                          << "\n=============================" << endl;

        if (veryVerbose) cout <<
                "\tTest a rotation that will fail without a callback." << endl;
        {
            Obj mX(Z); const Obj& X = mX;
            mX.forceRotation();

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (veryVerbose) cout <<
                   "\tTest a rotation that will fail with a callback." << endl;
        {
            Obj mX(Z); const Obj& X = mX;

            RotCb cb(Z);
            mX.setOnFileRotationCallback(cb);

            ASSERT(0 == cb.numInvocations());

            mX.forceRotation();

            ASSERT(0 == cb.numInvocations());
            ASSERT(0 != cb.status());
        }

        if (veryVerbose) cout <<
                     "\tTest a rotation that succeed with a callback." << endl;
        {
            Obj mX(Z); const Obj& X = mX;

            RotCb cb(Z);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            const bsl::string BASENAME = tempFileName(veryVerbose);

            ASSERT(0 == mX.enableFileLogging((BASENAME + ".%T").c_str()));

            bsl::vector<bsl::string> files(Z);
            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);

                bsl::string logName;
                ASSERT(X.isFileLoggingEnabled(&logName));

                mX.forceRotation();
                LOOP2_ASSERT(i, cb.numInvocations(), 1 == cb.numInvocations());
                LOOP2_ASSERT(i, cb.status(), 0 == cb.status());
                LOOP3_ASSERT(i, logName, cb.rotatedFileName(),
                             logName == cb.rotatedFileName());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }
                files.push_back(cb.rotatedFileName());
                cb.reset();
            }

            for (int i = 0; i < files.size(); ++i) {
                FileUtil::remove(files[i]);
            }
        }
        if (veryVerbose) cout <<
                       "\tTest a rotation that renames old log files." << endl;
        {
            Obj mX(Z); const Obj& X = mX;

            RotCb cb(Z);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            const bsl::string BASENAME = tempFileName(veryVerbose);

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            bsl::vector<bsl::string> files(Z);
            bsl::string logName;
            ASSERT(X.isFileLoggingEnabled(&logName));

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);

                mX.forceRotation();
                ASSERT(1       == cb.numInvocations());
                ASSERT(0       == cb.status());
                ASSERT(logName != cb.rotatedFileName());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }

                files.push_back(cb.rotatedFileName());
                cb.reset();
            }
            files.push_back(logName);

            for (int i = 0; i < files.size(); ++i) {
                 FileUtil::remove(files[i]);
            }
        }

        if (veryVerbose) cout << "\tTest a with publication rotation" << endl;
        {
            Obj mX(Z); const Obj& X = mX;

            RotCb cb(Z);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            const bsl::string BASENAME = tempFileName(veryVerbose);

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str(), true));
            mX.rotateOnSize(1);

            ASSERT(X.isFileLoggingEnabled());

            char buffer[1025];  // rotateOnSize is specified in multiples of 1K
            bsl::memset(buffer, 'x', sizeof(buffer));
            buffer[sizeof(buffer) - 1] = 0;

            bsl::vector<bsl::string> files(Z);

            publishRecord(&mX, buffer);

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);

                publishRecord(&mX, buffer);

                LOOP2_ASSERT(i, cb.numInvocations(), 1 == cb.numInvocations());
                LOOP2_ASSERT(i, cb.numInvocations(), 0 == cb.status());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }
                files.push_back(cb.rotatedFileName());
                cb.reset();
            }

            mX.disableSizeRotation();

            // Set to rotate every second
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));
            buffer[1] = 0;  // Don't need to write much for time-based rotation

            for (int i = 0; i < 3; ++i) {
                bslmt::ThreadUtil::microSleep(0, 1);

                publishRecord(&mX, buffer);

                LOOP2_ASSERT(i, cb.numInvocations(), 1 == cb.numInvocations());
                LOOP2_ASSERT(i, cb.numInvocations(), 0 == cb.status());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }

                files.push_back(cb.rotatedFileName());
                cb.reset();
            }

            for (int i = 0; i < files.size(); ++i) {
                FileUtil::remove(files[i]);
            }
        }

        if (veryVerbose) cout << "\tTest a re-entrant rotation" << endl;
        {

            Obj mX(Z); const Obj& X = mX;
            ReentrantRotationCallback cb(&mX);

            mX.setOnFileRotationCallback(cb);

            mX.forceRotation();  //  The test is whether this call will lock
                                 //  the task.
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING: ROTATED FILENAME
        //
        // Concerns:
        //: 1 A timestamp is appended to the filename if the rotated file has
        //:   the same name as the current log file.
        //:
        //: 2 The callback function is correctly supplied with the name of the
        //:   rotated file.
        //:
        //: 3 The user specified '%p'-escape sequence expands to the to the
        //:   process id (in decimal format) in the rotated file name.
        //:
        //: 4 The user specified escape sequences for time stamp (%Y, %M, %D,
        //:   %h, %m, s, %T) are expanded to their corresponding time element
        //:   (year, month, day, hour, minute, second, complete-timestamp,
        //:   respectively)
        //:
        //: 5 That the time stamp elements of the file name are appended in
        //:   localtime if 'isPublishInLocalTime' is 'true' and in UTC
        //:   otherwise.
        //:
        // Plans:
        //: 1 Setup the test infrastructure, including a callback.
        //:
        //: 2 Create a table of patterns and the expected names.
        //:
        //: 3 For each row in the table:
        //:   1 Enable file logging and ensure the time before and after call
        //:     to 'enableFileLogging' is within the same second.  If not,
        //:     disable and re-enable file logging again until we can get the
        //:     log file timestamp with certainty.
        //:
        //:   2 Verify the correct log is created
        //:
        //:   3 Perform a force rotation and verify the rotated log has the
        //:     correct filename.
        //:
        //: 4 Use 'enablePublishInLocalTime' and 'disablePublishInLocalTime'
        //:   to configure the time stamp on the generated log file.  Enable
        //:   file logging and verify the generated file name has a time stamp
        //:   in local-time or UTC, respectively.
        //
        // Testing:
        //  CONCERN: Rotated log filenames are as expected
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING: ROTATED FILENAME"
                          << "\n=========================" << endl;


        if (veryVerbose) cout << "Test infrastructure setup." << endl;

        const int processId = bdls::ProcessUtil::getProcessId();

        if (veryVeryVerbose) { P(processId) }

         bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        Obj mX(&ta); const Obj& X = mX;

        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);
        mX.enablePublishInLocalTime();

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");


        if (verbose) cout << "Test table of file names" << endl;
        {
            static const struct {
                int         d_line;
                const char *d_suffix;
                const char *d_expectedPattern;
                bool        d_uniqueNameFlag;
            } DATA[] = {

                //LINE  SUFFIX             PATTERN         UNIQUE
                //----  -----------------  -------------   ------

                { L_,   "",                "",             false  },
                { L_,   "%Y",              "Y",            false  },
                { L_,   "%Y%M%D",          "YMD",          false  },

                { L_,   "%p-%Y",           "p-Y",          false  },
                { L_,   "%p%Y%p%M%p%D%p",  "pYpMpDp",      false  },
                { L_,   ".%p.%T.%p",       ".p.YMD_hms.p",  true  },

                { L_,   ".%T",             ".YMD_hms",      true  },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_line;
                const char *SUF  = DATA[ti].d_suffix;
                const char *PAT  = DATA[ti].d_expectedPattern;
                const bool  UNI  = DATA[ti].d_uniqueNameFlag;

                if (veryVeryVerbose) { P_(LINE) P_(SUF) P_(PAT) P(UNI) }

                bdlt::Datetime startDatetime, endDatetime;

                // loop until startDatetime is equal to endDatetime
                do {
                    startDatetime = getCurrentLocalTime();

                    LOOP_ASSERT(LINE, 0 == mX.enableFileLogging(
                                    (BASENAME + SUF).c_str()));
                    LOOP_ASSERT(LINE, X.isFileLoggingEnabled());
                    LOOP_ASSERT(LINE, 1 == mX.enableFileLogging(
                                    (BASENAME + SUF).c_str()));

                    endDatetime = getCurrentLocalTime();

                    if (startDatetime.date()   != endDatetime.date()
                        || startDatetime.hour()   != endDatetime.hour()
                        || startDatetime.minute() != endDatetime.minute()
                        || startDatetime.second() != endDatetime.second()) {
                        // not sure the exact time when the log file was
                        // opened because startDatetime and endDatetime are
                        // different; will try it again
                        bsl::string fn;
                        ASSERT(1 == mX.isFileLoggingEnabled(&fn));
                        mX.disableFileLogging();
                        ASSERT(0 == FileUtil::remove(fn.c_str()));
                    }

                } while (!X.isFileLoggingEnabled());

                ASSERTV(LINE, startDatetime.year()   == endDatetime.year());
                ASSERTV(LINE, startDatetime.month()  == endDatetime.month());
                ASSERTV(LINE, startDatetime.day()    == endDatetime.day());
                ASSERTV(LINE, startDatetime.hour()   == endDatetime.hour());
                ASSERTV(LINE, startDatetime.minute() == endDatetime.minute());
                ASSERTV(LINE, startDatetime.second() == endDatetime.second());

                BALL_LOG_INFO<< "log" << BALL_LOG_END;

                // now construct the name of the log file from startDatetime

                bsl::ostringstream oss;
                oss << BASENAME;
                for (const char *c = PAT; *c != '\0'; ++c) {
                    switch (*c) {
                      case 'Y': {
                          oss << bsl::setw(4) << bsl::setfill('0')
                              << startDatetime.year();
                      } break;
                      case 'M': {
                          oss << bsl::setw(2) << bsl::setfill('0')
                              << startDatetime.month();
                      } break;
                      case 'D': {
                          oss << bsl::setw(2) << bsl::setfill('0')
                              << startDatetime.day();
                      } break;
                      case 'h': {
                          oss << bsl::setw(2) << bsl::setfill('0')
                              << startDatetime.hour();
                      } break;
                      case 'm': {
                          oss << bsl::setw(2) << bsl::setfill('0')
                              << startDatetime.minute();
                      } break;
                      case 's': {
                          oss << bsl::setw(2) << bsl::setfill('0')
                              << startDatetime.second();
                      } break;
                      case 'p': {
                          oss << processId;
                      } break;
                      default: {
                          oss << *c;
                      } break;
                    }
                }


                const bsl::string LOGNAME = oss.str().c_str();

                if (veryVeryVerbose) { P(LOGNAME) }

                LOOP2_ASSERT(LINE, LOGNAME.c_str(),
                             1 == FileUtil::exists(LOGNAME.c_str()));

                bslmt::ThreadUtil::microSleep(0, 1);
                mX.forceRotation();

                mX.disableFileLogging();

                LOOP2_ASSERT(LINE, LOGNAME.c_str(),
                             1 == FileUtil::exists(LOGNAME.c_str()));

                bsl::string ROTATED_NAME = LOGNAME;
                if (!UNI) {
                    oss << '.';
                    oss << bsl::setw(4) << bsl::setfill('0')
                        << startDatetime.year();
                    oss << bsl::setw(2) << bsl::setfill('0')
                        << startDatetime.month();
                    oss << bsl::setw(2) << bsl::setfill('0')
                        << startDatetime.day();
                    oss << '_';
                    oss << bsl::setw(2) << bsl::setfill('0')
                        << startDatetime.hour();
                    oss << bsl::setw(2) << bsl::setfill('0')
                        << startDatetime.minute();
                    oss << bsl::setw(2) << bsl::setfill('0')
                        << startDatetime.second();

                    ROTATED_NAME = oss.str();

                    if (veryVeryVerbose) { P(ROTATED_NAME) }

                    ASSERTV(LINE, ROTATED_NAME.c_str(),
                            1 == FileUtil::exists(ROTATED_NAME.c_str()));
                    ASSERTV(LINE, 2 == getNumLines(ROTATED_NAME.c_str()));
                }

                if (veryVeryVerbose) { P(ROTATED_NAME) }

                ASSERTV(ti, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(ti, cb.status(), 0 == cb.status());
                ASSERTV(ti, cb.rotatedFileName(),
                        ROTATED_NAME == cb.rotatedFileName());

                cb.reset();
                removeFilesByPrefix(BASENAME.c_str());
            }
        }

        if (verbose) cout << "Testing filename time-stamp uses "
                          << "'isPublishInLocalTimeEnabled'" << endl;
        {
            const char *UTC_PATTERN = "utc_%T";
            {
                // Testing UTC filename time stamp.

                bdlt::Datetime start = bdlt::CurrentTime::utc();

                mX.disablePublishInLocalTime();
                mX.enableFileLogging(UTC_PATTERN);

                BALL_LOG_INFO << "log" << BALL_LOG_END;

                bdlt::Datetime end = bdlt::CurrentTime::utc();

                bsl::string logFilename;
                ASSERT(X.isFileLoggingEnabled(&logFilename));

                mX.disableFileLogging();

                // Verify the time stamp in the filename.
                bsl::string   year(logFilename.substr(4,  4));
                bsl::string  month(logFilename.substr(8,  2));
                bsl::string    day(logFilename.substr(10, 2));
                bsl::string   hour(logFilename.substr(13, 2));
                bsl::string minute(logFilename.substr(15, 2));
                bsl::string second(logFilename.substr(17, 2));

                bdlt::Datetime timestamp(bsl::atoi(year.c_str()),
                                        bsl::atoi(month.c_str()),
                                        bsl::atoi(day.c_str()),
                                        bsl::atoi(hour.c_str()),
                                        bsl::atoi(minute.c_str()),
                                        bsl::atoi(second.c_str()));

                if (veryVerbose) {
                    P(logFilename);
                    P_(year); P_(month); P_(day);
                    P_(hour); P_(minute); P(second);
                }

                start.setMillisecond(0);
                ASSERTV(timestamp, end, start<=timestamp && timestamp<=end);

                FileUtil::remove(logFilename.c_str());
            }

            const char *LOCAL_PATTERN = "local_%T";
            {
                // Testing UTC filename time stamp.

                bdlt::Datetime start = bdlt::CurrentTime::local();

                mX.enablePublishInLocalTime();
                mX.enableFileLogging(LOCAL_PATTERN);

                BALL_LOG_INFO << "log" << BALL_LOG_END;

                bdlt::Datetime end = bdlt::CurrentTime::local();

                bsl::string logFilename;
                ASSERT(X.isFileLoggingEnabled(&logFilename));

                mX.disableFileLogging();

                // Verify the time stamp in the filename.
                bsl::string   year(logFilename.substr(6,  4));
                bsl::string  month(logFilename.substr(10,  2));
                bsl::string    day(logFilename.substr(12, 2));
                bsl::string   hour(logFilename.substr(15, 2));
                bsl::string minute(logFilename.substr(17, 2));
                bsl::string second(logFilename.substr(19, 2));

                bdlt::Datetime timestamp(bsl::atoi(year.c_str()),
                                        bsl::atoi(month.c_str()),
                                        bsl::atoi(day.c_str()),
                                        bsl::atoi(hour.c_str()),
                                        bsl::atoi(minute.c_str()),
                                        bsl::atoi(second.c_str()));

                if (veryVerbose) {
                    P(logFilename);
                    P_(year); P_(month); P_(day);
                    P_(hour); P_(minute); P(second);
                }

                start.setMillisecond(0);
                ASSERTV(timestamp, end, start<=timestamp && timestamp<=end);

                FileUtil::remove(logFilename.c_str());
            }
        }

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CAPTURE ERROR MESSAGE WHEN THE STREAM FAILS
        //
        // Concerns:
        //   That an error message is written to stderr when the stream fails.
        //
        // Plan:
        //   Set the OS to limit the total size of any file we write to to
        //   a fairly small amount, then use BALL to write more than that.
        //   while doing this, capture the stderr output and verify a message
        //   has been written.
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        // 'setrlimit' is not implemented on Cygwin.

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only see
        // each message only once.
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_TRACE,
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_OFF));

        ball::MultiplexObserver multiplexObserver;
        ball::LoggerManager::initSingleton(&multiplexObserver,
                                          configuration);

        if (verbose) cout << "Testing output when the stream fails"
                          << " (UNIX only)."
                          << endl;
        {
            bsl::string smallFile = tempFileName(veryVerbose);

            // Set the limit of the file size we can produce to 2048 bytes.
            struct rlimit rlim;
            ASSERT(0 == getrlimit(RLIMIT_FSIZE, &rlim));
            rlim.rlim_cur = 2048;
            ASSERT(0 == setrlimit(RLIMIT_FSIZE, &rlim));

            // I think this sets it so we won't trap when we get the file size
            // limit exception.

            struct sigaction act,oact;
            act.sa_handler = SIG_IGN;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ASSERT(0 == sigaction(SIGXFSZ, &act, &oact));

            Obj mX(&ta);  const Obj& X = mX;

            multiplexObserver.registerObserver(&mX);

            bsl::stringstream os;

            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

            // we want to capture the error message that will be written to
            // stderr (not cerr).  Redirect stderr to a file.  We can't
            // redirect it back; we'll have to use 'ASSERT2' (which outputs to
            // cout, not cerr) from now on and report a summary to to cout at
            // the end of this case.

            bsl::string stderrFN = tempFileName(veryVerbose);
            ASSERT(stderr == freopen(stderrFN.c_str(), "w", stderr));

            ASSERT2(0 == mX.enableFileLogging(smallFile.c_str(), true));
            ASSERT2(X.isFileLoggingEnabled());
            ASSERT2(1 == mX.enableFileLogging(smallFile.c_str(), true));

            for (int i = 0 ; i < 40 ;  ++i) {
                BALL_LOG_TRACE << "log"  << BALL_LOG_END;
            }

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFN.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line));  // caught an error message

            const bool fail = (bsl::string::npos ==
                                     line.find("ERROR: ball::FileObserver2:"));
            ASSERT(!fail);
            if (fail) cout << "line: " << line << endl;
#ifndef BSLS_PLATFORM_CMP_IBM
            // On native IBM, after the error, even when the stream fails,
            // logging will be attempted over and over again, which results in
            // more than one error messages.
            ASSERT2(!getline(stderrFs, line));  // and only one message
#endif

            mX.disableFileLogging();
            removeFilesByPrefix(smallFile.c_str());
            multiplexObserver.deregisterObserver(&mX);
        }
#else
        if (verbose) {
            cout << "Skipping case 4 on Windows and Cygwin..." << endl;
        }
#endif

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING LOG FILE ROTATION
        //
        // Concerns:
        //   The number of existing log files should not exceed the value of
        //   of 'd_maxLogFiles'.
        //
        // Plan:
        //   Set up the file observer so that it would have generated many log
        //   files if there were no limit.  Verify that the number of log
        //   files that actually exist does not exceed 'd_maxLogFiles'.
        //
        // Testing:
        //   void setMaxLogFiles();
        //   int maxLogFiles() const;
        //   int removeExcessLogFiles();
        // --------------------------------------------------------------------

        static bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        ball::LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only see
        // each message only once.

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_TRACE,
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_OFF));

        ball::MultiplexObserver multiplexObserver;
        ball::LoggerManagerScopedGuard guard(&multiplexObserver,
                                            configuration,
                                            &ta);
        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        if (verbose) cout << "Testing log file deletion." << endl;
        {
            if (verbose) cout << "\t log file opened with timestamp" << endl;

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdlt::DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,1));
            ASSERT(bdlt::DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM_OS_UNIX
            ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BALL_LOG_TRACE << "log" << BALL_LOG_END;
                bslmt::ThreadUtil::microSleep(1000 * 1000);
            }

            glob_t globbuf;
            ASSERT(0 == glob((filename + ".*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            mX.disableFileLogging();

            mX.setMaxLogFiles(2);
            ASSERT(3 == mX.removeExcessLogFiles());
            ASSERT(0 == glob((filename + ".*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            multiplexObserver.deregisterObserver(&mX);
            globfree(&globbuf);
            removeFilesByPrefix(filename.c_str());
#endif
#endif
        }

        if (verbose) cout << "\t log file opened without timestamp" << endl;
        {
            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdlt::DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,1));
            ASSERT(bdlt::DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM_OS_UNIX
            ASSERT(0 == mX.enableFileLogging(filename.c_str(), false));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BALL_LOG_TRACE << "log" << BALL_LOG_END;
                bslmt::ThreadUtil::microSleep(1000 * 1000);
            }

            glob_t globbuf;
            ASSERT(0 == glob((filename + ".*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            mX.disableFileLogging();

            mX.setMaxLogFiles(2);
            ASSERT(2 == mX.removeExcessLogFiles());
            ASSERT(0 == glob((filename + ".*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            multiplexObserver.deregisterObserver(&mX);
            globfree(&globbuf);
            removeFilesByPrefix(filename.c_str());
#endif
#endif
        }

        if (verbose) cout << "\t log file name containing timestamp" << endl;
        {
            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdlt::DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,1));
            ASSERT(bdlt::DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM_OS_UNIX
            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");
            ASSERT(0 == mX.enableFileLogging((filename + "%s").c_str(),
                                             false));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BALL_LOG_TRACE << "log" << BALL_LOG_END;
                bslmt::ThreadUtil::microSleep(1000 * 1000);
            }

            glob_t globbuf;
            ASSERT(0 == glob((filename + "*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            mX.disableFileLogging();

            mX.setMaxLogFiles(2);
            ASSERT(3 == mX.removeExcessLogFiles());
            ASSERT(0 == glob((filename + "*").c_str(), 0, 0, &globbuf));
            ASSERT(X.maxLogFiles() >= (int)globbuf.gl_pathc);

            multiplexObserver.deregisterObserver(&mX);
            globfree(&globbuf);
            removeFilesByPrefix(filename.c_str());
#endif
#endif
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Rotation functions test
        //
        // Concerns:
        //   1. 'rotateOnSize' triggers a rotation when expected.
        //   2. 'disableSizeRotation' disables rotation on size
        //   3. 'forceRotation' triggers a rotation
        //   4. 'rotateOnLifetime' triggers a rotation when expected
        //   5. 'disableLifetimeRotation' disables rotation on lifetime
        //
        // Test plan:
        //   We will exercise both rotation rules to verify that they work
        //   properly using glob to count the files and proper timing.  We will
        //   also verify that the size rule is followed by checking the size of
        //   log files.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   void disableLifetimeRotation()
        //   void disableSizeRotation()
        //   void forceRotation()
        //   void rotateOnSize(int size)
        //   void rotateOnLifetime(bdlt::DatetimeInterval timeInterval)
        //   bdlt::DatetimeInterval rotationLifetime() const
        //   int rotationSize() const
        // --------------------------------------------------------------------

            ball::LoggerManagerConfiguration configuration;

            // Publish synchronously all messages regardless of their severity.
            // This configuration also guarantees that the observer will only
            // see each message only once.

            ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_TRACE,
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_OFF));

            ball::MultiplexObserver multiplexObserver;
            ball::LoggerManager::initSingleton(&multiplexObserver,
                                              configuration);

#ifdef BSLS_PLATFORM_OS_UNIX
        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                globfree(&globbuf);
                ASSERT(fs.is_open());
                int linesNum = 0;
                bsl::string line;
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(2 == linesNum);
                ASSERT(X.isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;

            {
                ASSERT(bdlt::DatetimeInterval(0) ==
                       X.rotationLifetime());

                mX.rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,3));
                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X.rotationLifetime());

                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Check the number of lines in the file.

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[1], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                int linesNum = 0;
                bsl::string line(&ta);
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(4 == linesNum);

                mX.disableLifetimeRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

                // Check that no rotation occurred.

                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);
                fs.open(globbuf.gl_pathv[1], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                linesNum = 0;
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(6 == linesNum);
            }

            if (verbose) cout << "Testing forced rotation." << endl;
            {
                bslmt::ThreadUtil::microSleep(0, 2);
                mX.forceRotation();
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
                BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
                BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;

                // Check that the rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(3 == globbuf.gl_pathc);

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[2], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                int linesNum = 0;
                bsl::string line(&ta);
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(8 == linesNum);
            }

            if (verbose) cout << "Testing size-constrained rotation." << endl;
            {
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERT(0 == X.rotationSize());
                mX.rotateOnSize(1);
                ASSERT(1 == X.rotationSize());
                for (int i = 0 ; i < 15; ++i) {
                    BALL_LOG_TRACE << "log" << BALL_LOG_END;

                    // We sleep because otherwise, the loop is too fast to make
                    // the timestamp change so we cannot observe the rotation.

                    bslmt::ThreadUtil::microSleep(200 * 1000);
                }

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(4 <= globbuf.gl_pathc);

                // We are not checking the last one since we do not have any
                // information on its size.

                bsl::ifstream fs;
                for (int i = 0; i < (int)globbuf.gl_pathc - 3; ++i) {
                    ASSERT(1024 < getFileSize(globbuf.gl_pathv[i + 2]));
                }

                int oldNumFiles = globbuf.gl_pathc;
                globfree(&globbuf);

                ASSERT(1 == X.rotationSize());
                mX.disableSizeRotation();
                ASSERT(0 == X.rotationSize());

                for (int i = 0 ; i < 15; ++i) {
                    BALL_LOG_TRACE << "log" << BALL_LOG_END;
                    bslmt::ThreadUtil::microSleep(50 * 1000);
                }

                // Verify that no rotation occurred.

                ASSERT(0 == glob((filename + ".20*").c_str(), 0, 0, &globbuf));
                ASSERT(oldNumFiles == (int)globbuf.gl_pathc);
                globfree(&globbuf);
            }

            mX.disableFileLogging();
            removeFilesByPrefix(filename.c_str());
            multiplexObserver.deregisterObserver(&mX);
        }

        {
            // Test with no timestamp.

            if (verbose) cout << "Test-case infrastructure setup." << endl;

            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), false));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

                glob_t globbuf;
                ASSERT(0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                globfree(&globbuf);
                ASSERT(fs.is_open());
                int linesNum = 0;
                bsl::string line;
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(2 == linesNum);
                ASSERT(X.isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) ==
                       X.rotationLifetime());

                mX.rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,3));

                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X.rotationLifetime());
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Check the number of lines in the file.

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                int linesNum = 0;
                bsl::string line(&ta);
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(4 == linesNum);

                mX.disableLifetimeRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

                // Check that no rotation occurred.

                ASSERT(0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                linesNum = 0;
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(6 == linesNum);
            }

            mX.disableFileLogging();
            removeFilesByPrefix(filename.c_str());
            multiplexObserver.deregisterObserver(&mX);
        }
#endif

        ball::LoggerManager::shutDownSingleton();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Publishing test
        //
        // Concerns:
        //   1. publish() logs in the default format
        //   2. publish() publishes all messages to a file if file logging
        //      is enabled
        //   3. the name of the log file should be in accordance with what is
        //      defined by the pattern if file logging is enabled by a pattern
        //   4. setLogFileFunctor can change the format effectively
        //
        // Plan:
        //   We will set up the observer and check if logged messages are in
        //   the expected format and contain the expected data by comparing the
        //   output of this observer with 'ball::DefaultObserver', that we
        //   slightly modify.  Then, We will use different manipulators and
        //   functors to affect output format and verify that it has changed
        //   where expected.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   ball::FileObserver(ball::Severity::Level, bslma::Allocator)
        //   ~ball::FileObserver()
        //   publish(const ball::Record& record, const ball::Context& context)
        //   void disableFileLogging()
        //   int enableFileLogging(const char *fileName, bool timestampFlag)
        //   void publish(const ball::Record&, const ball::Context&)
        //   bool isFileLoggingEnabled() const
        //   void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
        //   bool isPublishInLocalTimeEnabled() const;
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   bdlt::DatetimeInterval localTimeOffset();
        // --------------------------------------------------------------------

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
// TBD fix this for Windows !!!
#ifndef BSLS_PLATFORM_OS_WINDOWS
#if (!defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR >= 10)\
  && !defined(BSLS_PLATFORM_OS_AIX)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif
        {
            ball::LoggerManagerConfiguration configuration;

            // Publish synchronously all messages regardless of their severity.
            // This configuration also guarantees that the observer will only
            // see each message only once.

            ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_TRACE,
                                                  ball::Severity::e_OFF,
                                                  ball::Severity::e_OFF));

            ball::MultiplexObserver multiplexObserver;
            ball::LoggerManager::initSingleton(&multiplexObserver,
                                              configuration);

            if (verbose) cout << "Testing default output format." << endl;
            {
                Obj mX;  const Obj& X = mX;
                bsl::ostringstream dos;

                time_t currentTime = time(0);
                struct tm localtm, gmtm;
#ifdef BSLS_PLATFORM_OS_WINDOWS
                localtm = *localtime(&currentTime);
                gmtm = *gmtime(&currentTime);
#else
                localtime_r(&currentTime, &localtm);
                gmtime_r(&currentTime, &gmtm);
#endif

                bdlt::Datetime currentLocalTime, currentUTCTime;
                bdlt::DatetimeUtil::convertFromTm(&currentLocalTime, localtm);
                bdlt::DatetimeUtil::convertFromTm(&currentUTCTime, gmtm);

                ASSERT(currentLocalTime - currentUTCTime ==
                                                          X.localTimeOffset());

                ball::DefaultObserver defaultObserver(&dos);
                ball::MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX);
                localMultiObserver.registerObserver(&defaultObserver);
                multiplexObserver.registerObserver(&localMultiObserver);

                bsl::string filename = tempFileName(veryVerbose);
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                BALL_LOG_WARN << "log WARN" << BALL_LOG_END;

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, filename, os));

                // Replace the spaces after pid, __FILE__
                {
                    bsl::string temp = dos.str();
                    temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                    replaceSecondSpace(&temp, ':');
                    dos.str(temp);
                }

                ASSERT(dos.str() == os);
                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&localMultiObserver);

            }

            if (verbose) cout << "Testing format with local time offset."
                              << endl;
            {
                Obj mX(&ta);  const Obj& X = mX;
                ASSERT(!X.isPublishInLocalTimeEnabled());
                mX.enablePublishInLocalTime();
                ASSERT( X.isPublishInLocalTimeEnabled());

                mX.disablePublishInLocalTime();
                ASSERT(!X.isPublishInLocalTimeEnabled());

                mX.enablePublishInLocalTime();
                ASSERT( X.isPublishInLocalTimeEnabled());

                bsl::ostringstream testOs, dos;

                ball::DefaultObserver defaultObserver(&dos);
                ball::MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX);
                localMultiObserver.registerObserver(&defaultObserver);
                multiplexObserver.registerObserver(&localMultiObserver);

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                bsl::string filename = tempFileName(veryVerbose);
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BALL_LOG_FATAL << "log FATAL" << BALL_LOG_END;

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, filename, os));

                testOs << "\nFATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                          " ball::FileObserverTest log FATAL " << "\n";
                // Replace the spaces after pid, __FILE__
                {
                    bsl::string temp = dos.str();
                    temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                    replaceSecondSpace(&temp, ':');
                    dos.str(temp);
                }

                LOOP2_ASSERT(dos.str(), os, dos.str() != os);
                ASSERT(testOs.str() != os);

                // Now let's verify the actual difference.
                int defaultObsHour;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(dos.str().substr(11, 2));
                    ASSERT(is >> defaultObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int fileObsHour;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(os.substr(11, 2));
                    ASSERT(is >> fileObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
#ifdef BSLS_PLATFORM_OS_UNIX
                tzset();
                time_t currentTime;
                struct tm *ts;
                ::time(&currentTime);
                ts = ::localtime(&currentTime);
#ifdef BSLS_PLATFORM_OS_FREEBSD
                // This is the BSD way.  I am not sure of this accounts for DST
                // or not.  The following if might need to be moved into the
                // #else

                long difference = -ts->tm_gmtoff / 3600;
#else
                long difference = timezone / 3600;
#endif
                if (ts->tm_isdst > 0) {
                    --difference;
                }
#else
                _tzset();
                long difference = _timezone / 3600;
#endif
                LOOP2_ASSERT((fileObsHour + difference + 24) % 24,
                             defaultObsHour,
                       (fileObsHour + difference + 24) % 24 == defaultObsHour);
                {
                    bsl::string temp = dos.str();
                    temp[11] = os[11];
                    temp[12] = os[12];
                    dos.str(temp);
                }

                if (defaultObsHour - difference >= 0 &&
                    defaultObsHour - difference < 24) {
                    // UTC and local time are on the same day
                    ASSERT(dos.str() == os);
                }
                else if (dos.str().length() >= 10 && os.length() >= 10) {
                    // UTC and local time are on different days.  Ignore date.
                    ASSERT(dos.str().substr(10) == os.substr(10));
                } else {
                    ASSERT(0 && "can't substr(10), string too short");
                }

                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&localMultiObserver);

            }

            if (verbose) cout << "Testing file logging." << endl;
            {
                const bsl::string filename = tempFileName(veryVerbose);

                Obj mX(&ta);  const Obj& X = mX;
                ASSERT(-1 == mX.enableFileLogging("/bogus/path/foo"));
                multiplexObserver.registerObserver(&mX);

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                ASSERT(0 == mX.enableFileLogging(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str()));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
                BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
                BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
                BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
                BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

                bsl::string os;
                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                ASSERT( X.isFileLoggingEnabled());
                mX.disableFileLogging();
                ASSERT(!X.isFileLoggingEnabled());
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
                BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
                BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
                BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
                BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                ASSERT(0 == mX.enableFileLogging(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str()));

                BALL_LOG_TRACE << "log 7" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 8" << BALL_LOG_END;
                BALL_LOG_INFO <<  "log 9" << BALL_LOG_END;
                BALL_LOG_WARN <<  "log 1" << BALL_LOG_END;
                BALL_LOG_ERROR << "log 2" << BALL_LOG_END;
                BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

                ASSERT(24 == readFileIntoString(__LINE__, filename, os));

                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&mX);

            }


#ifdef BSLS_PLATFORM_OS_UNIX
            if (verbose) cout << "Testing file logging with timestamp."
                              << endl;
            {
                bsl::string filename = tempFileName(veryVerbose);

                Obj mX(&ta);  const Obj& X = mX;
                multiplexObserver.registerObserver(&mX);

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
                BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
                BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
                BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
                BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

                bsl::string os;
                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&mX);
            }


            if (verbose) cout << "Testing '%%' in file name pattern." << endl;
            {
                static const struct {
                    int         d_lineNum;           // source line number
                    const char *d_patternSuffix_p;   // pattern suffix
                    const char *d_filenameSuffix_p;  // filename suffix
                } DATA[] = {
                    //line  pattern suffix  filename suffix
                    //----  --------------  ---------------
                    { L_,   "foo",          "foo"                           },
                    { L_,   "foo%",         "foo%"                          },
                    { L_,   "foo%bar",      "foo%bar"                       },
                    { L_,   "foo%%",        "foo"                           },
                    { L_,   "foo%%bar",     "foobar"                        },
                    { L_,   "foo%%%",       "foo%"                          },
                    { L_,   "foo%%%bar",    "foo%bar"                       },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE     = DATA[ti].d_lineNum;
                    const char *PATTERN  = DATA[ti].d_patternSuffix_p;
                    const char *FILENAME = DATA[ti].d_filenameSuffix_p;

                    bsl::string baseName = tempFileName(veryVerbose);
                    bsl::string pattern(baseName);   pattern  += PATTERN;
                    bsl::string expected(baseName);  expected += FILENAME;
                    bsl::string actual;

                    Obj mX(&ta);  const Obj& X = mX;

                    LOOP_ASSERT(LINE, 0 == mX.enableFileLogging(
                                                      pattern.c_str(), false));
                    LOOP_ASSERT(LINE, X.isFileLoggingEnabled(&actual));

                    if (veryVeryVerbose) {
                        P_(PATTERN);  P_(expected);  P(actual);
                    }

                    LOOP_ASSERT(LINE, expected == actual);

                    mX.disableFileLogging();

                    // look for the file with the expected name
                    glob_t globbuf;
                    LOOP_ASSERT(LINE, 0 == glob(expected.c_str(),
                                                0, 0, &globbuf));
                    LOOP_ASSERT(LINE, 1 == globbuf.gl_pathc);

                    removeFilesByPrefix(expected.c_str());
                }
            }

            if (verbose) cout << "Testing functors with the same format."
                              << endl;
            {
                // create three FileObservers
                Obj mX1(&ta);  const Obj& X1 = mX1;
                Obj mX2(&ta);  const Obj& X2 = mX2;
                Obj mX3(&ta);  const Obj& X3 = mX3;

                // connect them to a MultiplexObserver
                ball::MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX1);
                localMultiObserver.registerObserver(&mX2);
                localMultiObserver.registerObserver(&mX3);
                multiplexObserver.registerObserver(&localMultiObserver);

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                // obtain a temporary filename
                bsl::string baseName = tempFileName(veryVerbose);

                // derive three temporary filenames from the basename
                bsl::string filename1 = baseName + "1";
                bsl::string filename2 = baseName + "2";
                bsl::string filename3 = baseName + "3";

                // enable file logging and set the formatting functor
                ASSERT(0 == mX1.enableFileLogging(filename1.c_str(), false));
                ASSERT(X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                ASSERT(0 == mX2.enableFileLogging(filename2.c_str(), false));
                ASSERT(X2.isFileLoggingEnabled());
                ASSERT(1 == mX2.enableFileLogging(filename2.c_str(), false));
                mX2.setLogFileFunctor(ball::RecordStringFormatter());

                ASSERT(0 == mX3.enableFileLogging(filename3.c_str(), false));
                ASSERT(X3.isFileLoggingEnabled());
                ASSERT(1 == mX3.enableFileLogging(filename3.c_str(), false));
                mX3.setLogFileFunctor(&logRecord1);

                BALL_LOG_WARN << "log" << BALL_LOG_END;

                bsl::string file1, file2, file3;

                ASSERT(2 == readFileIntoString(__LINE__, filename1, file1));
                ASSERT(2 == readFileIntoString(__LINE__, filename2, file2));
                ASSERT(2 == readFileIntoString(__LINE__, filename3, file3));

                if (veryVeryVerbose) { P_(file1);  P_(file2);  P(file3); }
                ASSERT(file1 == file2);
                ASSERT(file1 == file3);
                ASSERT(file2 == file3);

                mX1.disableFileLogging();
                mX2.disableFileLogging();
                mX3.disableFileLogging();
                removeFilesByPrefix(filename1.c_str());
                removeFilesByPrefix(filename2.c_str());
                removeFilesByPrefix(filename3.c_str());

                multiplexObserver.deregisterObserver(&localMultiObserver);
            }

            if (verbose) cout << "Testing functors with different formats."
                              << endl;
            {
                // create three FileObservers
                Obj mX1(&ta);  const Obj& X1 = mX1;
                Obj mX2(&ta);  const Obj& X2 = mX2;
                Obj mX3(&ta);  const Obj& X3 = mX3;

                // connect them to a MultiplexObserver
                ball::MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX1);
                localMultiObserver.registerObserver(&mX2);
                localMultiObserver.registerObserver(&mX3);
                multiplexObserver.registerObserver(&localMultiObserver);

                BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

                // obtain a temporary filename
                bsl::string baseName = tempFileName(veryVerbose);

                // derive three temporary filenames from the basename
                bsl::string filename1 = baseName + "1";
                bsl::string filename2 = baseName + "2";
                bsl::string filename3 = baseName + "3";

                // enable file logging and set the formatting functor
                ASSERT(0 == mX1.enableFileLogging(filename1.c_str(), false));
                ASSERT(X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                ASSERT(0 == mX2.enableFileLogging(filename2.c_str(), false));
                ASSERT(X2.isFileLoggingEnabled());
                ASSERT(1 == mX2.enableFileLogging(filename2.c_str(), false));
                mX2.setLogFileFunctor(ball::RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));

                ASSERT(0 == mX3.enableFileLogging(filename3.c_str(), false));
                ASSERT(X3.isFileLoggingEnabled());
                ASSERT(1 == mX3.enableFileLogging(filename3.c_str(), false));
                mX3.setLogFileFunctor(&logRecord2);

                BALL_LOG_WARN << "log" << BALL_LOG_END;

                bsl::string file1, file2, file3;

                ASSERT(2 == readFileIntoString(__LINE__, filename1, file1));
                ASSERT(2 == readFileIntoString(__LINE__, filename2, file2));
                ASSERT(2 == readFileIntoString(__LINE__, filename3, file3));

                ASSERT(file1 != file2);
                ASSERT(file1 != file3);
                ASSERT(file2 != file3);

                // at this point we have three different logs produced by these
                // three fileobservers configured with different formats; now
                // we are going to reuse one of these fileobserver and change
                // its functors to see if the resulting log should be identical
                // to one of those known logs in file1, file2, and file3

                bsl::string filename1a = tempFileName(veryVerbose);
                bsl::string fileContent;

                mX1.disableFileLogging();
                ASSERT(!X1.isFileLoggingEnabled());
                ASSERT(0 == mX1.enableFileLogging(filename1a.c_str(), false));
                ASSERT( X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                mX1.setLogFileFunctor(ball::RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));
                BALL_LOG_WARN << "log" << BALL_LOG_END;
                ASSERT(2 == readFileIntoString(__LINE__, filename1a,
                                               fileContent));
                ASSERT(file2 == fileContent);

                bsl::string filename1b = tempFileName(veryVerbose);
                mX1.disableFileLogging();
                ASSERT(!X1.isFileLoggingEnabled());
                ASSERT(0 == mX1.enableFileLogging(filename1b.c_str(), false));
                ASSERT( X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                mX1.setLogFileFunctor(&logRecord2);
                BALL_LOG_WARN << "log" << BALL_LOG_END;
                ASSERT(2 == readFileIntoString(__LINE__, filename1b,
                                               fileContent));
                ASSERT(file3 == fileContent);

                bsl::string filename1c = tempFileName(veryVerbose);
                mX1.disableFileLogging();
                ASSERT(!X1.isFileLoggingEnabled());
                ASSERT(0 == mX1.enableFileLogging(filename1c.c_str(), false));
                ASSERT( X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                mX1.setLogFileFunctor(ball::RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));
                BALL_LOG_WARN << "log" << BALL_LOG_END;
                ASSERT(2 == readFileIntoString(__LINE__, filename1c,
                                               fileContent));
                ASSERT(file2 == fileContent);

                mX1.disableFileLogging();
                mX2.disableFileLogging();
                mX3.disableFileLogging();
                removeFilesByPrefix(filename1.c_str());
                removeFilesByPrefix(filename2.c_str());
                removeFilesByPrefix(filename3.c_str());
                removeFilesByPrefix(filename1a.c_str());
                removeFilesByPrefix(filename1b.c_str());
                removeFilesByPrefix(filename1c.c_str());
                multiplexObserver.deregisterObserver(&localMultiObserver);
            }
#endif
        }
#endif

        ball::LoggerManager::shutDownSingleton();
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // LARGE FILE TEST
        //
        // Concern:
        //: 1 'ball::FileObserver2' is able to write to a file over 2 GB.
        //
        // Plan:
        //: 1 Keep logging to a file until it is over 5 GB.  Manually verify
        //:   the file created is as expected.
        //
        // Testing:
        //   CONCERN: 'ball::FileObserver2' is able to write to a file over 2GB
        // --------------------------------------------------------------------
        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_TRACE,
                                                     ball::Severity::e_OFF,
                                                     ball::Severity::e_OFF));

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;
        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());

        BALL_LOG_SET_CATEGORY("ball::FileObserverTest");

        P(BASENAME);

        char buffer[1026];
        memset(buffer, 'x', sizeof buffer);
        buffer[sizeof buffer - 1] = '\0';

        // Write over 5 GB
        for (int i = 0; i < 5000000; ++i) {
            BALL_LOG_WARN << buffer << BALL_LOG_END;
        }

        //FileUtil::remove(BASENAME.c_str());

        ball::LoggerManager::shutDownSingleton();
      } break;
      default: {
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
