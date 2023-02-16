// ball_fileobserver2.t.cpp                                           -*-C++-*-
#include <ball_fileobserver2.h>

#include <ball_context.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_recordattributes.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>
#include <ball_streamobserver.h>
#include <ball_userfieldvalue.h>

#include <bdlb_tokenizer.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bdls_processutil.h>
#include <bdls_tempdirectoryguard.h>

#include <bdlt_currenttime.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_localtimeoffset.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadutil.h>

#include <bslstl_stringref.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines an observer ('ball::FileObserver2') that
// writes log records to a file.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] FileObserver2(bslma::Allocator *);
// [ 1] ~FileObserver2();
//
// MANIPULATORS
// [ 1] void disableFileLogging();
// [ 2] void disableLifetimeRotation();
// [ 1] void disablePublishInLocalTime();
// [ 2] void disableSizeRotation();
// [ 8] void disableTimeIntervalRotation();
// [ 1] int  enableFileLogging(const char *fileName);
// [ 1] int  enableFileLogging(const char *fileName, bool timestampFlag);
// [ 1] void enablePublishInLocalTime();
// [ 1] void publish(const Record& record, const Context& context);
// [ 1] void publish(const shared_ptr<Record>&, const Context&);
// [ 2] void forceRotation();
// [ 2] void rotateOnSize(int size);
// [ 2] void rotateOnLifetime(DatetimeInterval& interval);
// [ 8] void rotateOnTimeInterval(const DatetimeInterval& interval);
// [ 9] void rotateOnTimeInterval(const DtInterval& i, const Datetime& s);
// [ 1] void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
// [ 5] void setOnFileRotationCallback(const OnFileRotationCallback&);
//
// ACCESSORS
// [ 1] bool isFileLoggingEnabled() const;
// [ 1] bool isFileLoggingEnabled(bsl::string *result) const;
// [ 1] bool isFileLoggingEnabled(std::string *result) const;
// [ 1] bool isFileLoggingEnabled(std::pmr::string *result) const;
// [ 1] bool isPublishInLocalTimeEnabled() const;
// [ 2] DatetimeInterval rotationLifetime() const;
// [ 2] int rotationSize() const;
// ----------------------------------------------------------------------------
// [13] USAGE EXAMPLE
// [12] CONCERN: CURRENT LOCAL-TIME OFFSET IN TIMESTAMP
// [11] CONCERN: TIME CALLBACKS ARE CALLED
// [10] CONCERN: ROTATION CAN BE ENABLED AFTER FILE LOGGING
// [ 7] CONCERN: ROTATION CALLBACK TRIGGERS CORRECTLY FOR EXISTING FILE
// [ 6] CONCERN: ROTATION ON SIZE WORKS FOR EXISTING FILES
// [ 4] CONCERN: ROTATED LOG FILENAMES ARE AS EXPECTED
// [ 3] CONCERN: LOGGING TO A FAILING STREAM

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

namespace {

void aSsErT2(bool condition, const char *message, int line)
{
    if (condition) {
        cerr << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

#define ASSERT2(X) { aSsErT2(!(X), #X, __LINE__); }

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

typedef ball::FileObserver2  Obj;
typedef bdls::FilesystemUtil FsUtil;
typedef bsls::Types::Int64   Int64;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

void enableFileLogging(
                    bsl::shared_ptr<ball::FileObserver2>& observer,
                    const bsl::string&                    fileName,
                    const bool                            addTimestamp = false)
    // Enable file logging for the specified 'observer' to the specified
    // 'fileName'.  Optionally specify 'addTimestamp' to append timestamp to
    // the filename.
{
    ASSERT(false == observer->isFileLoggingEnabled());

    if (!addTimestamp) {
        ASSERT(0     == observer->enableFileLogging(fileName.c_str()));
        ASSERT(true  == observer->isFileLoggingEnabled());
        ASSERT(1     == observer->enableFileLogging(fileName.c_str()));
    }
    else {
        ASSERT(0     == observer->enableFileLogging(fileName.c_str(),
                                                    addTimestamp));
        ASSERT(true  == observer->isFileLoggingEnabled());
        ASSERT(1     == observer->enableFileLogging(fileName.c_str(),
                                                    addTimestamp));
    }
}

bdlt::Datetime getCurrentLocalTime()
    // Return current local time as a 'bdlt::Datetime' value.
{
    time_t    currentTime = time(0);
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
    // Output the specified 'record' into the specified 'stream'.
{
    ball::Severity::Level severityLevel =
        (ball::Severity::Level) record.fixedFields().severity();

    stream << '\n';

    const int k_SIZE = 64;
    char      buffer[k_SIZE];
    const int fractionalSecondPrecision = 3;

    const int numBytesWritten = record.fixedFields().timestamp().printToBuffer(
                                                    buffer,
                                                    k_SIZE,
                                                    fractionalSecondPrecision);
    stream.write(buffer, numBytesWritten);

    stream << " ";
    stream << record.fixedFields().processID()       << ":";
    stream << record.fixedFields().threadID()        << " ";
    stream << ball::Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().fileName()        << ":";
    stream << record.fixedFields().lineNumber()      << " ";
    stream << record.fixedFields().category()        << " ";
    stream << record.fixedFields().message ()        << " ";

    const ball::UserFields& customFields = record.customFields();

    for (int i = 0; i < customFields.length(); ++i) {
        stream << customFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
}

void logRecord2(bsl::ostream& stream, const ball::Record& record)
    // Output the specified 'record' into the specified 'stream'.
{
    ball::Severity::Level severityLevel =
        (ball::Severity::Level) record.fixedFields().severity();

    stream << '\n';

    stream << record.fixedFields().message ()        << " ";
    stream << record.fixedFields().category()        << " ";
    stream << record.fixedFields().fileName()        << ":";
    stream << ball::Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().threadID()        << " ";
    stream << record.fixedFields().processID()       << ":";

    const ball::UserFields& customFields = record.customFields();

    for (int i = 0; i < customFields.length(); ++i) {
        stream << customFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
}

int readFileIntoString(int                lineNum,
                       const bsl::string& fileName,
                       bsl::string&       fileContent)
    // Read the content of the specified 'fileName' file into the specified
    // 'fileContent' string.  Return the number of lines read from the file.
{
    bsl::ifstream fs;

#ifdef BSLS_PLATFORM_OS_UNIX
    glob_t globbuf;
    ASSERTV(lineNum, 0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
    ASSERTV(lineNum, 1 == globbuf.gl_pathc);

    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
    globfree(&globbuf);
#else
    fs.open(fileName.c_str(), bsl::ifstream::in);
#endif

    ASSERTV(lineNum, fs.is_open());
    fileContent = "";

    bsl::string lineContent;
    int         lines = 0;
    while (getline(fs, lineContent))
    {
        fileContent += lineContent;
        fileContent += '\n';
        lines++;
    }
    fs.close();
    return lines;
}

class LogRotationCallbackTester {
    // This class can be used as a functor matching the signature of
    // 'ball::FileObserver2::OnFileRotationCallback'.  This class records every
    // invocation of the function-call operator, and is intended to test
    // whether 'ball::FileObserver2' calls the log-rotation callback
    // appropriately.

    // PRIVATE TYPES
    struct Rep {
      private:
        // NOT IMPLEMENTED
        Rep(const Rep&);
        Rep& operator=(const Rep&);

      public:
        // DATA
        int         d_invocations;
        int         d_status;
        bsl::string d_rotatedFileName;

        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(Rep, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit Rep(bslma::Allocator *basicAllocator)
            // Create an object with default attribute values.  Use the
            // specified 'basicAllocator' to supply memory.
        : d_invocations(0)
        , d_status(0)
        , d_rotatedFileName(basicAllocator)
        {
        }
    };

    enum {
        k_UNINITIALIZED = INT_MIN
    };

    // DATA
    bsl::shared_ptr<Rep> d_rep;

  public:

    // CREATORS
    explicit LogRotationCallbackTester(bslma::Allocator *basicAllocator)
        // Create a callback tester object with default attribute values.  Use
        // the specified 'basicAllocator' to supply memory.
    {
        d_rep.createInplace(basicAllocator, basicAllocator);
        reset();
    }

    // MANIPULATORS
    void operator()(int status, const bsl::string& rotatedFileName)
        // Set the value at the status address supplied at construction to the
        // specified 'status', and set the value at the log file name address
        // supplied at construction to the specified 'rotatedFileName'.
    {
        ++d_rep->d_invocations;
        d_rep->d_status          = status;
        d_rep->d_rotatedFileName = rotatedFileName;
    }

    void reset()
        // Reset the attributes of this object to their default values.
    {
        d_rep->d_invocations     = 0;
        d_rep->d_status          = k_UNINITIALIZED;
        d_rep->d_rotatedFileName.clear();
    }

    // ACCESSORS
    int numInvocations() const { return d_rep->d_invocations; }
        // Return the number of times that the function-call operator has been
        // invoked since the most recent call to 'reset', or if 'reset' has
        // not been called, since this objects construction.

    int status() const { return d_rep->d_status; }
        // Return the status passed to the most recent invocation of the
        // function-call operation, or 'k_UNINITIALIZED' if 'numInvocations' is
        // 0.

    const bsl::string& rotatedFileName() const
        // Return a 'const' reference to the file name supplied to the most
        // recent invocation of the function-call operator, or the empty string
        // if 'numInvocations' is 0.
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
        // Create a rotation callback for the specified 'observer'.
    : d_observer_p(observer)
    {
    }

    void operator()(int, const bsl::string&);
        // Disables file logging for the observer supplied at construction.
};

void ReentrantRotationCallback::operator()(int, const bsl::string& )
{
    d_observer_p->disableFileLogging();
}

void publishRecord(Obj *observer, const char *message)
    // Publish the specified 'message' to the specified 'observer' object.
{
    ball::RecordAttributes attr(bdlt::CurrentTime::utc(),
                               1,
                               2,
                               "FILENAME",
                               3,
                               "CATEGORY",
                               32,
                               message);

    ball::Record  record(attr, ball::UserFields());
    ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);

    observer->publish(record, context);
}


int getNumLines(const char *fileName)
    // Return the number of lines in the file with the specified 'fileName'.
{
    bsl::ifstream fs;
    fs.open(fileName, bsl::ifstream::in);
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
    static bsls::Types::Int64 s_localTimeOffsetInSeconds;
    static int                s_loadCount;

  public:
    // CLASS METHODS
    static bsls::TimeInterval loadLocalTimeOffset(const bdlt::Datetime&);
        // Return the local time offset that was set by the previous call to
        // the 'setLocalTimeOffset' method.  If the 'setLocalTimeOffset' method
        // has not been called, load 0.

    static void setLocalTimeOffset(
                                  bsls::Types::Int64 localTimeOffsetInSeconds);
        // Set the specified 'localTimeOffsetInSeconds' as the value loaded by
        // calls to the loadLocalTimeOffset' method.

    static int loadCount();
        // Return the number of times the 'loadLocalTimeOffset' method has been
        // called since the start of process.
};

bsls::Types::Int64 TestLocalTimeOffsetCallback::s_localTimeOffsetInSeconds = 0;
int                TestLocalTimeOffsetCallback::s_loadCount                = 0;

bsls::TimeInterval TestLocalTimeOffsetCallback::loadLocalTimeOffset(
                                                         const bdlt::Datetime&)
{
    ++s_loadCount;
    return bsls::TimeInterval(s_localTimeOffsetInSeconds, 0);
}

void TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                   bsls::Types::Int64 localTimeOffsetInSeconds)
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

void getDatetimeField(bsl::string        *result,
                      const bsl::string&  fileName,
                      int                 recordNumber)
{
    ASSERT(1 <= recordNumber);

    bsl::string fileContent;

    int lineCount = readFileIntoString(__LINE__, fileName, fileContent);

    ASSERT(recordNumber * 2 <= lineCount);

    bsl::vector<bsl::string> lines;
    splitStringIntoLines(&lines, fileContent.c_str());

    int recordIndex = recordNumber  - 1;

    ASSERT(0 <= recordIndex);
    ASSERT(static_cast<int>(lines.size()) > recordIndex);

    const bsl::string& s = lines[recordIndex];
    *result = s.substr(0, s.find_first_of(' '));
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        // This is standard preamble to create the directory and filename for
        // the test.
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "test.log");

///Example: Basic Usage
/// - - - - - - - - - -
// First, we create a 'ball::LoggerManagerConfiguration' object, 'lmConfig',
// and set the logging "pass-through" level -- the level at which log records
// are published to registered observers -- to 'DEBUG':
//..
        ball::LoggerManagerConfiguration lmConfig;
        lmConfig.setDefaultThresholdLevelsIfValid(ball::Severity::e_DEBUG);
//..
// Next, create a 'ball::LoggerManagerScopedGuard' object whose constructor
// takes the configuration object just created.  The guard will initialize the
// logger manager singleton on creation and destroy the singleton upon
// destruction.  This guarantees that any resources used by the logger manager
// will be properly released when they are not needed:
//..
        ball::LoggerManagerScopedGuard guard(lmConfig);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Next, we create a 'ball::FileObserver2' object and register it with the
// 'ball' logging system;
//..
        bsl::shared_ptr<ball::FileObserver2> observer =
                                       bsl::make_shared<ball::FileObserver2>();
//..
// Next, we configure the log file rotation rules:
//..
        // Rotate the file when its size becomes greater than or equal to 128
        // megabytes.
        observer->rotateOnSize(1024 * 128);

        // Rotate the file every 24 hours.
        observer->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).
//
// Then, we enable logging to a file:
//..
        // Create and log records to a file named "/var/log/task/task.log".
        observer->enableFileLogging("/var/log/task/task.log");
//..
// Finally, we register the file observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// the 'publish' method:
//..
        int rc = manager.registerObserver(observer, "default");
        ASSERT(0 == rc);
//..

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // REPRODUCE BUG FROM DRQS 123123158
        //
        // Concerns:
        //: 1 On Solaris, logging "" wound up calling 'fstream::write(0, 0)'
        //:   which Solaris did not deal with properly, and resulted in the
        //:   fail bit of the stream being set and a (confusing) error message
        //:   being written to the console by the file observer.
        //
        // Plan:
        //: 1 Log "" and assert that the file logging is still enabled
        //:   afterward.  If the bug manifests itself, and error will be
        //:   written to the console and the assert will fail.
        // --------------------------------------------------------------------

        if (verbose) cout << "REPRODUCE BUG FROM DRQS 123123158\n"
                             "=================================\n";

        int rot_size = 1024 * 1024;

        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              baseName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&baseName, "testLog");

        if (veryVeryVerbose) { T_; T_; P(baseName); }

        ball::LoggerManagerConfiguration configuration;

        configuration.setDefaultThresholdLevelsIfValid(
                                                      ball::Severity::e_TRACE,
                                                      ball::Severity::e_TRACE,
                                                      ball::Severity::e_WARN,
                                                      ball::Severity::e_FATAL);

        // Instantiate the logger manager singleton.
        ball::LoggerManagerScopedGuard scopedGuard(configuration);

        bsl::shared_ptr<Obj> observer = bsl::make_shared<Obj>();
        observer->enableFileLogging(baseName.c_str(), false);
        observer->rotateOnSize(rot_size);

        ASSERT(0 == ball::LoggerManager::singleton().registerObserver(
                                                              observer,
                                                              "testObserver"));

        // Set a category -- an arbitrary name.
        BALL_LOG_SET_CATEGORY("main category");

        BALL_LOG_INFO << "LOG";
        BALL_LOG_TRACE << "";
        ASSERT(observer->isFileLoggingEnabled());

        if (verbose) {
            bsl::string out;
            (void) readFileIntoString(__LINE__, baseName, out);
            bsl::cout << out;
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CURRENT LOCAL-TIME OFFSET IN TIMESTAMP
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
        //: 4 When publishing in local time is enabled, confirm that there
        //:   exactly 1 request for local time offset for each published
        //:   record.  (C-3)
        //
        // Testing:
        //   CONCERN: CURRENT LOCAL-TIME OFFSET IN TIMESTAMP
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CURRENT LOCAL-TIME OFFSET IN TIMESTAMP"
                          << "\n=============================================="
                          << endl;

        const bdlt::Datetime UTC_ARRAY[] = { bdlt::EpochUtil::epoch(),
                                             bdlt::Datetime(2017,
                                                            1,
                                                            1,
                                                            12,
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
        enum { NUM_UTC_ARRAY = sizeof UTC_ARRAY / sizeof *UTC_ARRAY };

        if (verbose) cout << "\nTest TestCurrentTimeCallback: Direct" << endl;
        {
            for (int i = 0; i < NUM_UTC_ARRAY; ++i) {
                bdlt::Datetime utcDatetime = UTC_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(utcDatetime) }

                TestCurrentTimeCallback::setUtcDatetime(utcDatetime);
                bsls::TimeInterval result = TestCurrentTimeCallback::load();

                bdlt::Datetime resultAsDatetime =
                              bdlt::EpochUtil::convertFromTimeInterval(result);
                ASSERTV(i, utcDatetime == resultAsDatetime);
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

                ASSERTV(i, utcDatetime == result1);
                ASSERTV(i, result2     == result1);
            }

           // Restore original system-time callback.

            bdlt::CurrentTime::setCurrentTimeCallback(
                                                  originalCurrentTimeCallback);
        }

        const Int64 LTO_ARRAY[] = { -86399, -1, 0, 1, 86399 };
        enum { NUM_LTO_ARRAY = sizeof LTO_ARRAY / sizeof *LTO_ARRAY };

        int loadCount = TestLocalTimeOffsetCallback::loadCount();
        ASSERT(0 == loadCount);

        if (verbose)
            cout << "\nTest TestLocalTimeOffsetCallback: Direct" << endl;
        {
            for (int i = 0; i < NUM_LTO_ARRAY; ++i) {
                Int64 localTimeOffset = LTO_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(localTimeOffset) }

                TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                              localTimeOffset);
                for (int j = 0; j < NUM_UTC_ARRAY; ++j) {
                    bdlt::Datetime utcDatetime  = UTC_ARRAY[j];

                    if (veryVerbose) { T_ T_ P_(j) P(utcDatetime) }

                    Int64 result =
                        TestLocalTimeOffsetCallback::loadLocalTimeOffset(
                                                               utcDatetime)
                                                               .totalSeconds();
                    ++loadCount;

                    ASSERTV(i, j, localTimeOffset == result);
                    ASSERTV(i, j, loadCount       ==
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
                Int64 localTimeOffset = LTO_ARRAY[i];

                if (veryVerbose) { T_ P_(i) P(localTimeOffset) }

                TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                              localTimeOffset);
                for (int j = 0; j < NUM_UTC_ARRAY; ++j) {
                    bdlt::Datetime utcDatetime  = UTC_ARRAY[j];

                    if (veryVerbose) { T_ T_ P_(j) P(utcDatetime) }

                    Int64 result =
                        bdlt::LocalTimeOffset::localTimeOffset(utcDatetime)
                                                               .totalSeconds();
                    ++loadCount;

                    ASSERTV(i, j, localTimeOffset == result);
                    ASSERTV(i, j, loadCount       ==
                                     TestLocalTimeOffsetCallback::loadCount());
                }
            }

            bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                              originalLocalTimeOffsetCallback);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TIME CALLBACKS
        //
        // Concern:
        //:  1 Time converting callbacks are invoked when log string is
        //:    generated.
        //
        // Plan:
        //:  1 Call logging and verify that the installed test time callbacks
        //:    are invoked as expected.
        //
        // Testing:
        //   CONCERN: TIME CALLBACKS ARE CALLED
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TIME CALLBACKS"
                          << "\n======================" << endl;

        const bdlt::Datetime UTC_ARRAY[] = { bdlt::EpochUtil::epoch(),
                                             bdlt::Datetime(2017,
                                                            1,
                                                            1,
                                                            12,
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
        enum { NUM_UTC_ARRAY = sizeof UTC_ARRAY / sizeof *UTC_ARRAY };

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        {
            bslma::TestAllocator         da("da", veryVeryVeryVerbose);
            bslma::TestAllocator         ta("ta", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bdls::TempDirectoryGuard tempDirGuard("ball_");

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            bsl::string baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "testLog");

            if (veryVeryVerbose) { T_; T_; P(baseName); }

            enableFileLogging(mX, baseName, true);

            // Fetch the actual file name with timestamp appended.
            bsl::string fileName;
            ASSERT(true == X->isFileLoggingEnabled(&fileName));

            if (veryVeryVerbose) { T_; T_; P(fileName); }

            BALL_LOG_SET_CATEGORY("TestCategory");

            int                  logRecordCount = 0;
            int                  testLocalTimeOffsetInSeconds;
            bsl::string          datetimeField;
            bsl::ostringstream   expectedDatetimeField;
            const bdlt::Datetime testUtcDatetime = UTC_ARRAY[1];

            bdlt::CurrentTime::CurrentTimeCallback
                originalCurrentTimeCallback =
                    bdlt::CurrentTime::setCurrentTimeCallback(
                                               &TestCurrentTimeCallback::load);
            TestCurrentTimeCallback::setUtcDatetime(testUtcDatetime);

            bdlt::LocalTimeOffset::LocalTimeOffsetCallback
                originalLocalTimeOffsetCallback =
                    bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                            &TestLocalTimeOffsetCallback::loadLocalTimeOffset);

            int expectedLoadCount = TestLocalTimeOffsetCallback::loadCount();

            if (veryVerbose)
                cout << "\tLog with Publish In Local Time Disabled" << endl;

            ASSERT(false == X->isPublishInLocalTimeEnabled());

            BALL_LOG_TRACE << "log 1";
            ++logRecordCount;

            getDatetimeField(&datetimeField, fileName, logRecordCount);

            const int SIZE = 32;
            char      buffer[SIZE];
            bsl::memset(buffer, 'X', SIZE);

            testUtcDatetime.printToBuffer(buffer, SIZE, 3);

            bsl::string EXP(buffer);

            if (veryVerbose) { T_ P_(EXP) P(datetimeField) }

            ASSERT(EXP == datetimeField);
            ASSERT(expectedLoadCount ==
                                     TestLocalTimeOffsetCallback::loadCount());

            BALL_LOG_TRACE << "log 2";
            ++logRecordCount;

            getDatetimeField(&datetimeField, fileName, logRecordCount);

            bsl::memset(buffer, 'X', SIZE);

            testUtcDatetime.printToBuffer(buffer, SIZE, 3);

            EXP.assign(buffer);

            if (veryVerbose) { T_ P_(EXP) P(datetimeField) }

            ASSERT(EXP == datetimeField);
            ASSERT(expectedLoadCount ==
                                     TestLocalTimeOffsetCallback::loadCount());

            if (veryVerbose)
                cout << "\tLog with Publish In Local Time Enabled" << endl;

            mX->enablePublishInLocalTime();
            ASSERT(X->isPublishInLocalTimeEnabled());

            testLocalTimeOffsetInSeconds = -1 * 60 * 60;
            TestLocalTimeOffsetCallback::setLocalTimeOffset(
                                                 testLocalTimeOffsetInSeconds);

            if (veryVerbose) { T_ P(testLocalTimeOffsetInSeconds); }

            BALL_LOG_TRACE << "log 3";
            ++logRecordCount;
            ++expectedLoadCount;

            getDatetimeField(&datetimeField, fileName, logRecordCount);
            bsl::memset(buffer, 'X', SIZE);

            bdlt::Datetime DT = testUtcDatetime +
                           bdlt::DatetimeInterval(0,
                                                  0,
                                                  0,
                                                  testLocalTimeOffsetInSeconds,
                                                  0);
            DT.printToBuffer(buffer, SIZE, 3);

            EXP.assign(buffer);

            if (veryVerbose) { T_ P_(EXP) P(datetimeField) }

            ASSERT(EXP == datetimeField);
            ASSERT(expectedLoadCount ==
                                     TestLocalTimeOffsetCallback::loadCount());

            testLocalTimeOffsetInSeconds = -2 * 60 * 60;
            TestLocalTimeOffsetCallback::setLocalTimeOffset(
                    testLocalTimeOffsetInSeconds);

            if (veryVerbose) { T_ P(testLocalTimeOffsetInSeconds); }

            BALL_LOG_TRACE << "log 4";
            ++logRecordCount;
            ++expectedLoadCount;

            getDatetimeField(&datetimeField, fileName, logRecordCount);

            bsl::memset(buffer, 'X', SIZE);

            DT = testUtcDatetime +
                           bdlt::DatetimeInterval(0,
                                                  0,
                                                  0,
                                                  testLocalTimeOffsetInSeconds,
                                                  0);
            DT.printToBuffer(buffer, SIZE, 3);

            EXP.assign(buffer);

            if (veryVerbose) { T_ P_(EXP) P(datetimeField) }

            ASSERT(EXP == datetimeField);
            ASSERT(expectedLoadCount ==
                                     TestLocalTimeOffsetCallback::loadCount());

            mX->disablePublishInLocalTime();
            ASSERT(false == X->isPublishInLocalTimeEnabled());

            BALL_LOG_TRACE << "log 5";
            ++logRecordCount;

            getDatetimeField(&datetimeField, fileName, logRecordCount);

            bsl::memset(buffer, 'X', SIZE);

            testUtcDatetime.printToBuffer(buffer, SIZE, 3);

            EXP.assign(buffer);

            if (veryVerbose) { T_ P_(EXP) P(datetimeField) }

            ASSERT(EXP == datetimeField);
            ASSERT(expectedLoadCount ==
                                     TestLocalTimeOffsetCallback::loadCount());

            if (veryVerbose)
                cout << "\tLog with Publish In Local Time Disabled Again"
                     << endl;

            if (veryVerbose) cout << "\tCleanup" << endl;

            bdlt::CurrentTime::setCurrentTimeCallback(
                                                  originalCurrentTimeCallback);
            bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                              originalLocalTimeOffsetCallback);

            mX->disableFileLogging();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'rotateOnTimeInterval' AFTER 'enableFileLogging'
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' does not cause rotation immediately if
        //:   invoked after 'enableFileLogging'.
        //
        // Plan:
        //: 1 Invoke 'enableFileLogging' before 'rotateOnTimeInterval' and
        //:   verify rotation does not occur.
        //
        // Testing:
        //   CONCERN: ROTATION CAN BE ENABLED AFTER FILE LOGGING
        // --------------------------------------------------------------------
        if (verbose) cout
                << "\nTESTING 'rotateOnTimeInterval' AFTER 'enableFileLogging'"
                << "\n========================================================"
                << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        {
            bslma::TestAllocator         da("da", veryVeryVeryVerbose);
            bslma::TestAllocator         ta("ta", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            // Set callback to monitor rotation.

            RotCb cb(&ta);
            mX->setOnFileRotationCallback(cb);

            enableFileLogging(mX, fileName);
            ASSERT(0 == cb.numInvocations());

            BALL_LOG_SET_CATEGORY("TestCategory");

            mX->rotateOnTimeInterval(bdlt::DatetimeInterval(1));

            BALL_LOG_TRACE << "log";

            ASSERT(0 == cb.numInvocations());

            mX->disableFileLogging();

            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING TIME-BASED ROTATION
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' accepts any valid datetime value as the
        //:   reference time.
        //:
        //: 2 Reference time is interpreted as ether local or UTC time
        //:   depending on the 'isPublishInLocalTime()'.
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
        //  void rotateOnTimeInterval(const DtInterval& i, const Datetime& s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TIME-BASED ROTATION"
                          << "\n===========================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        {
            bslma::TestAllocator         da("da", veryVeryVeryVerbose);
            bslma::TestAllocator         ta("ta", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            RotCb cb(&ta);

            mX->setOnFileRotationCallback(cb);

            // Testing 3 cases: 0 - default UTC, 1 - local, 2 - UTC
            for (int i = 0; i < 3; ++i) {
                switch (i) {
                  case 1: {
                    mX->enablePublishInLocalTime();
                  } break;
                  case 2: {
                    mX->disablePublishInLocalTime();
                  } break;
                  default:
                    break;
                }

                {
                    bdls::TempDirectoryGuard tempDirGuard("ball_");
                    bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                    bdls::PathUtil::appendRaw(&fileName, "testLog");

                    // Set callback to monitor rotation.

                    enableFileLogging(mX, fileName);
                    ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

                    mX->disableFileLogging();
                }

                if (veryVerbose)
                    cout << "Test lower bound of absolute time reference."
                         << endl;
                {
                    BALL_LOG_SET_CATEGORY("TestCategory");

                    bdls::TempDirectoryGuard tempDirGuard("ball_");
                    bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                    bdls::PathUtil::appendRaw(&fileName, "testLog");

                    mX->rotateOnTimeInterval(
                                            bdlt::DatetimeInterval(0, 0, 0, 1),
                                            bdlt::Datetime(1, 1, 1));

                    enableFileLogging(mX, fileName);

                    bslmt::ThreadUtil::microSleep(0, 1);

                    ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

                    BALL_LOG_TRACE << "log";

                    ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());
                    ASSERT(1 == FsUtil::exists(cb.rotatedFileName().c_str()));

                    mX->disableFileLogging();
                    cb.reset();
                }

                if (veryVerbose)
                    cout << "Test upper bound of absolute time reference."
                         << endl;
                {
                    BALL_LOG_SET_CATEGORY("TestCategory");

                    bdls::TempDirectoryGuard tempDirGuard("ball_");
                    bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                    bdls::PathUtil::appendRaw(&fileName, "testLog");

                    mX->rotateOnTimeInterval(
                                     bdlt::DatetimeInterval(0, 0, 0, 1),
                                     bdlt::Datetime(9999, 12, 31, 23, 59, 59));

                    enableFileLogging(mX, fileName);

                    bslmt::ThreadUtil::microSleep(0, 1);

                    ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

                    BALL_LOG_TRACE << "log";

                    ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());
                    ASSERT(1 == FsUtil::exists(cb.rotatedFileName().c_str()));

                    mX->disableFileLogging();
                    cb.reset();
                }

                if (veryVerbose)
                    cout << "Test absolute time reference." << endl;
                {
                    BALL_LOG_SET_CATEGORY("TestCategory");

                    bdls::TempDirectoryGuard tempDirGuard("ball_");
                    bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                    bdls::PathUtil::appendRaw(&fileName, "testLog");

                    bdlt::Datetime startTime = bdlt::CurrentTime::utc();
                    if (i == 1) {
                        startTime = bdlt::CurrentTime::local();
                    }

                    startTime += bdlt::DatetimeInterval(-1, 0, 0, 1);

                    mX->rotateOnTimeInterval(bdlt::DatetimeInterval(1),
                                             startTime);

                    enableFileLogging(mX, fileName);

                    BALL_LOG_TRACE << "log";
                    bslmt::ThreadUtil::microSleep(0, 1);        // 1s
                    ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

                    bslmt::ThreadUtil::microSleep(250000, 1);  // 2.25s
                    BALL_LOG_TRACE << "log";

                    ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());
                    ASSERT(1 == FsUtil::exists(cb.rotatedFileName().c_str()));

                    mX->disableFileLogging();
                    cb.reset();
                }
            }

            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
      } break;
      case 8: {
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
        //:
        //: 6 A delay between 'enableFileLogging' and 'rotateOnTimeInterval',
        //:   when 'rotateOnItemInterval' uses the current time as its
        //:   reference start time, does not incur an erroneous rotation
        //:   (DRQS 87930585).
        //
        // Plan:
        //: 1 Configure rotation for 1 second.  Log a record before 1 second,
        //:   and ensure that no rotation occurs.  Then log a record after the
        //:   1 second and verify a rotation occurs.  (C-1)
        //:
        //: 2 Delay logging such that the rotation is delayed.  Then verify the
        //:   schedule for the next rotation is not affected.  (C-2)
        //:
        //: 3 Cause a rotation to occur between scheduled rotations and verify
        //:   that the rotation schedule is not affected.  (C-3)
        //:
        //: 4 Disable and then re-enable file logging and verify rotation
        //:   schedule is not affect.  (C-4)
        //:
        //: 5 Call 'disableTimeIntervalRotation' and verify that time-based
        //:   rotation is disabled.  (C-5)
        //:
        //: 6 Insert a delay between 'enableFileLogging' and
        //:   'rotateOnTimeInterval' and verify the log file is not
        //:    immediately rotated. (C-6)
        //:
        // Testing:
        //  void rotateOnTimeInterval(const DatetimeInterval& interval);
        //  void disableTimeIntervalRotation();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TIME-BASED ROTATION"
                          << "\n===========================" << endl;

        if (veryVerbose) cout << "Test normal rotation" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bslmt::ThreadUtil::microSleep(250000, 0);
            publishRecord(&mX, "test message 1");

            ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

            bslmt::ThreadUtil::microSleep(0, 1);
            publishRecord(&mX, "test message 2");

            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

            mX.disableFileLogging();
        }

        if (veryVerbose) cout << "Test delayed logging" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            // Rotate every 1s.

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bslmt::ThreadUtil::microSleep(100000, 1);       // 1.1 s
            publishRecord(&mX, "test message 1");
            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

            // Delay the next message so that it is in the middle of an
            // scheduled rotation.
            bslmt::ThreadUtil::microSleep(750000, 1);  // 1.75s
            publishRecord(&mX, "test message 2");
            ASSERTV(cb.numInvocations(), 2 == cb.numInvocations());

            // Verify we are back on schedule.
            bslmt::ThreadUtil::microSleep(250000, 0);  // .25s
            publishRecord(&mX, "test message 3");
            ASSERTV(cb.numInvocations(), 3 == cb.numInvocations());

            mX.disableFileLogging();
        }

        if (veryVerbose)
            cout << "Test rotation between scheduled rotations" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bslmt::ThreadUtil::microSleep(500000, 0);  // .5s
            mX.forceRotation();
            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

            bslmt::ThreadUtil::microSleep(600000, 0);  // .6s

            publishRecord(&mX, "test message 1");
            ASSERTV(cb.numInvocations(), 2 == cb.numInvocations());

            mX.disableFileLogging();
        }

        if (veryVerbose)
            cout << "Test disabling file logging between rotations" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bslmt::ThreadUtil::microSleep(0, 2);
            ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

            mX.disableFileLogging();
            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            // This rotation should happen immediately.

            publishRecord(&mX, "test message 1");
            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

            mX.disableFileLogging();
        }

        if (veryVerbose) cout << "Test 'disableTimeIntervalRotation" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 0, 500));

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bslmt::ThreadUtil::microSleep(0, 1);
            mX.disableTimeIntervalRotation();

            publishRecord(&mX, "test message 1");
            ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

            mX.disableFileLogging();
        }

        if (veryVerbose) cout << "\tTest a delayed rotateOnTimeInterval"
                              << "(DRQS 87930585)"
                              << bsl::endl;
        {
            // Test if there is a delay between 'enableFileLogging' and
            // 'rotateOnTimeInterval'.
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            mX.enablePublishInLocalTime();

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            // Delay, so that the time stamps for the file log and reference
            // start time (which is supplied by a default parameter value on
            // 'rotateOnTimeInterval') differ by a small amount.

            bslmt::ThreadUtil::microSleep(0, 1);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 1));  // 1 hour
            ASSERT(0 == cb.numInvocations());

            publishRecord(&mX, "test message 1");
            bslmt::ThreadUtil::microSleep(0, 1);
            publishRecord(&mX, "test message 2");

            // Verify the callback has not been invoked.

            ASSERT(0 == cb.numInvocations());

            mX.disableFileLogging();
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ROTATION CALLBACK WITH EXISTING FILE
        //
        // Concerns:
        //: 1 'rotateOnSize' triggers a rotation as expected even if the log
        //:   file already exist.
        //:
        //: 2 'suppressUniqueFileNameOnRotation(true)' suppresses unique
        //:   filename generation for the rotated log file on rotation.
        //
        // Plan:
        //: 1 Set 'rotateOnSize' to 1k, create a file with approximately 0.5k.
        //:
        //: 2 Write another 0.5k to the file and verify that the file is
        //:   rotated.  Ensure the log file name and rotated filename do not
        //:   match. (C-1)
        //:
        //: 3 Call 'suppressUniqueFileNameOnRotation(true)'.  Write another
        //:   0.5k to the file and verify that the rotation took place and the
        //:   log file name and rotated filename are the same. (C-2)
        //
        // Testing:
        //  CONCERN: ROTATION CALLBACK TRIGGERS CORRECTLY FOR EXISTING FILE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ROTATION CALLBACK WITH EXISTING FILE"
                          << "\n============================================"
                          << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bsl::shared_ptr<Obj> mX(new (ta) Obj(&ta), &ta);

        ASSERT(0 == manager.registerObserver(mX, "testObserver"));

        RotCb cb(&ta);
        mX->setOnFileRotationCallback(cb);

        // Set the format to include the message only for better control over
        // rotation events. The long filename part in the logs can lead to
        // unexpected rotations during the test.
        mX->setLogFileFunctor(ball::RecordStringFormatter("%m\n"));

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            if (verbose) cout << "Testing setup." << endl;
            {
                enableFileLogging(mX, fileName);
                mX->rotateOnSize(1);

                BALL_LOG_TRACE << "log 1";

                ASSERT(1 == FsUtil::exists(fileName.c_str()));

                ASSERT(1 == getNumLines(fileName.c_str()));
                ASSERT(0 == cb.numInvocations());
            }

            if (verbose) cout <<
                          "Testing file observer with existing file." << endl;
            {
                char buffer[512];
                memset(buffer, 'x', sizeof buffer);
                buffer[sizeof buffer - 1] = '\0';

                BALL_LOG_TRACE << buffer;
                mX->disableFileLogging();

                enableFileLogging(mX, fileName);

                BALL_LOG_TRACE << 'x';

                ASSERT(0 == cb.numInvocations());

                BALL_LOG_TRACE << buffer;
                BALL_LOG_TRACE << 'x';

                ASSERT(1 == FsUtil::exists(fileName.c_str()));
                ASSERT(1 == cb.numInvocations());
                ASSERT(1 == FsUtil::exists(cb.rotatedFileName().c_str()));
                ASSERTV(fileName,   cb.rotatedFileName(),
                        fileName != cb.rotatedFileName());
            }

            if (verbose) cout <<
                          "Testing file observer with suppressed filename "
                          "uniqueness" << endl;
            {
                char buffer[512];
                memset(buffer, 'x', sizeof buffer);
                buffer[sizeof buffer - 1] = '\0';

                BALL_LOG_TRACE << buffer;
                mX->disableFileLogging();

                enableFileLogging(mX, fileName);

                BALL_LOG_TRACE << 'x';

                ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

                bool suppressUniqueFileNameOnRotation =
                                      mX->isSuppressUniqueFileNameOnRotation();

                ASSERTV(suppressUniqueFileNameOnRotation,
                        false == suppressUniqueFileNameOnRotation);

                mX->suppressUniqueFileNameOnRotation(true);

                suppressUniqueFileNameOnRotation =
                                      mX->isSuppressUniqueFileNameOnRotation();

                ASSERTV(suppressUniqueFileNameOnRotation,
                        true == suppressUniqueFileNameOnRotation);

                BALL_LOG_TRACE << buffer;
                BALL_LOG_TRACE << 'x';

                ASSERT(1 == FsUtil::exists(fileName.c_str()));
                ASSERTV(cb.numInvocations(), 2 == cb.numInvocations());
                ASSERTV(fileName,   cb.rotatedFileName(),
                        fileName == cb.rotatedFileName());
            }

            mX->disableFileLogging();
        }
        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("testObserver"));
      } break;
      case 6: {
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
        // Testing:
        //  CONCERN: ROTATION ON SIZE WORKS FOR EXISTING FILES
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING ROTATE ON SIZE ON EXISTING FILE"
                 << "\n=======================================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
        bsl::shared_ptr<const Obj> X = mX;

        ASSERT(0 == manager.registerObserver(mX, "testObserver"));

        RotCb cb(&ta);
        mX->setOnFileRotationCallback(cb);

        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            enableFileLogging(mX, fileName);

            char buffer[1024];
            memset(buffer, 'x', sizeof buffer);
            buffer[sizeof buffer - 1] = '\0';

            BALL_LOG_TRACE << buffer;

            mX->disableFileLogging();
            ASSERT(false == X->isFileLoggingEnabled());

            ASSERT(1024 < FsUtil::getFileSize(fileName.c_str()));

            ASSERT(0 == X->rotationSize());
            mX->rotateOnSize(1);
            ASSERT(1 == X->rotationSize());

            enableFileLogging(mX, fileName);

            BALL_LOG_TRACE << "x";

            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERTV(cb.status(),         0 == cb.status());

            ASSERT(1 == FsUtil::exists(cb.rotatedFileName()));

            mX->disableFileLogging();
        }

        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("testObserver"));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING LOG FILE ROTATION CALLBACK
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

        if (verbose) cout << "\nTESTING LOG FILE ROTATION CALLBACK"
                          << "\n==================================" << endl;

        if (veryVerbose) cout <<
                "\tTest a rotation that will fail without a callback." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::TestAllocator         ta("test", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&ta);
            mX.forceRotation();

            ASSERT(0 == da.numBytesInUse());
        }

        if (veryVerbose)
            cout << "\tTest a rotation that fails with a callback." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::TestAllocator         ta("test",    veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&ta);

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);

            ASSERT(0 == cb.numInvocations());

            mX.forceRotation();

            ASSERT(0 == cb.numInvocations());
            ASSERT(0 != cb.status());

            ASSERT(0 == da.numBytesInUse());
        }

        if (veryVerbose)
            cout << "\tTest a rotation that renames old log files." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX; const Obj& X = mX;

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            bsl::string logName;
            ASSERT(true == X.isFileLoggingEnabled(&logName));

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);

                ASSERTV(i, cb.numInvocations(), 0 == cb.numInvocations());

                mX.forceRotation();

                ASSERTV(i, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(i, cb.status(), 0 == cb.status());
                ASSERTV(i, logName, cb.rotatedFileName(),
                        logName != cb.rotatedFileName());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }

                cb.reset();
            }
        }

        if (veryVerbose)
            cout << "\tTest a rotation that succeeds with a callback." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog.%T");

            Obj mX; const Obj& X = mX;

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            ASSERT(0 == mX.enableFileLogging(fileName.c_str()));

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);

                bsl::string logName;
                ASSERT(true == X.isFileLoggingEnabled(&logName));

                ASSERTV(i, cb.numInvocations(), 0 == cb.numInvocations());

                mX.forceRotation();

                ASSERTV(i, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(i, cb.status(), 0 == cb.status());
                ASSERTV(i, logName, cb.rotatedFileName(),
                        logName == cb.rotatedFileName());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }
                cb.reset();
            }
        }

        if (veryVerbose) cout << "\tTest a with publication rotation" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            Obj mX; const Obj& X = mX;

            RotCb cb(&ta);
            mX.setOnFileRotationCallback(cb);
            ASSERT(0 == cb.numInvocations());

            ASSERT(0 == mX.enableFileLogging(fileName.c_str(), true));
            mX.rotateOnSize(1);

            ASSERT(true == X.isFileLoggingEnabled());

            char buffer[1025];  // rotateOnSize is specified in multiples of 1K
            bsl::memset(buffer, 'x', sizeof(buffer));
            buffer[sizeof(buffer) - 1] = 0;

            publishRecord(&mX, buffer);

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bslmt::ThreadUtil::microSleep(0, 1);  // 1s

                publishRecord(&mX, buffer);

                ASSERTV(i, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(i, cb.numInvocations(), 0 == cb.status());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }
                cb.reset();
            }

            mX.disableSizeRotation();

            // Set to rotate every second
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0, 0, 0, 1));
            buffer[1] = 0;  // Don't need to write much for time-based rotation

            for (int i = 0; i < 3; ++i) {
                bslmt::ThreadUtil::microSleep(250000, 1);  // 1.25s

                publishRecord(&mX, buffer);

                ASSERTV(i, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(i, cb.numInvocations(), 0 == cb.status());

                if (veryVeryVerbose) {
                    P(cb.rotatedFileName());
                }

                cb.reset();
            }
        }

        if (veryVerbose) cout << "\tTest a re-entrant rotation" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            Obj mX(&ta);

            ReentrantRotationCallback cb(&mX);

            mX.setOnFileRotationCallback(cb);

            mX.forceRotation();  //  The test is whether this call will lock
                                 //  the task.
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ROTATED FILENAME
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
        //  CONCERN: ROTATED LOG FILENAMES ARE AS EXPECTED
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ROTATED FILENAME"
                          << "\n========================" << endl;


        if (veryVerbose) cout << "Test infrastructure setup." << endl;

        const int processId = bdls::ProcessUtil::getProcessId();

        if (veryVeryVerbose) { P(processId) }

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
        bsl::shared_ptr<const Obj> X = mX;

        ASSERT(0 == manager.registerObserver(mX, "testObserver"));

        RotCb cb(&ta);
        mX->setOnFileRotationCallback(cb);
        mX->enablePublishInLocalTime();

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

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_line;
                const char *SUF  = DATA[ti].d_suffix;
                const char *PAT  = DATA[ti].d_expectedPattern;
                const bool  UNI  = DATA[ti].d_uniqueNameFlag;

                if (veryVeryVerbose) { P_(LINE) P_(SUF) P_(PAT) P(UNI) }

                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "testLog");

                bdlt::Datetime startDatetime, endDatetime;

                // loop until startDatetime is equal to endDatetime
                do {
                    startDatetime = getCurrentLocalTime();

                    enableFileLogging(mX, fileName + SUF);

                    endDatetime = getCurrentLocalTime();

                    if (startDatetime.date()   != endDatetime.date()
                        || startDatetime.hour()   != endDatetime.hour()
                        || startDatetime.minute() != endDatetime.minute()
                        || startDatetime.second() != endDatetime.second()) {
                        // not sure the exact time when the log file was
                        // opened because startDatetime and endDatetime are
                        // different; will try it again
                        bsl::string fn;
                        ASSERT(true == mX->isFileLoggingEnabled(&fn));
                        mX->disableFileLogging();
                        ASSERT(0 == FsUtil::remove(fn.c_str()));
                    }
                } while (!X->isFileLoggingEnabled());

                ASSERTV(LINE, startDatetime.year()   == endDatetime.year());
                ASSERTV(LINE, startDatetime.month()  == endDatetime.month());
                ASSERTV(LINE, startDatetime.day()    == endDatetime.day());
                ASSERTV(LINE, startDatetime.hour()   == endDatetime.hour());
                ASSERTV(LINE, startDatetime.minute() == endDatetime.minute());
                ASSERTV(LINE, startDatetime.second() == endDatetime.second());

                BALL_LOG_INFO << "log";

                // now construct the name of the log file from startDatetime

                bsl::ostringstream oss;
                oss << fileName;
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


                const bsl::string actualFileName(oss.str());

                if (veryVeryVerbose) { P(actualFileName) }

                ASSERTV(LINE, actualFileName,
                        1 == FsUtil::exists(actualFileName));

                bslmt::ThreadUtil::microSleep(0, 1);
                mX->forceRotation();

                mX->disableFileLogging();

                ASSERTV(LINE, actualFileName,
                        1 == FsUtil::exists(actualFileName));

                bsl::string rotatedFileName(actualFileName);
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

                    rotatedFileName = oss.str();

                    if (veryVeryVerbose) { P(rotatedFileName) }

                    ASSERTV(LINE, rotatedFileName,
                            1 == FsUtil::exists(rotatedFileName));
                    ASSERTV(LINE, 2 == getNumLines(rotatedFileName.c_str()));
                }

                if (veryVeryVerbose) { P(rotatedFileName) }

                ASSERTV(ti, cb.numInvocations(), 1 == cb.numInvocations());
                ASSERTV(ti, cb.status(), 0 == cb.status());
                ASSERTV(ti, cb.rotatedFileName(),
                        rotatedFileName == cb.rotatedFileName());

                cb.reset();
            }
        }

        if (verbose) cout << "Testing filename time-stamp uses "
                          << "'isPublishInLocalTimeEnabled'" << endl;
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");

            if (veryVerbose)
                cout << "\tTesting utc pattern in filename." << endl;
            {
                // Testing UTC timestamp in filename.
                bsl::string fileName(tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "utc_%T");

                bdlt::Datetime start = bdlt::CurrentTime::utc();

                mX->disablePublishInLocalTime();
                enableFileLogging(mX, fileName);

                BALL_LOG_INFO << "log";

                bdlt::Datetime end = bdlt::CurrentTime::utc();

                bsl::string actualFileName;
                ASSERT(true == X->isFileLoggingEnabled(&actualFileName));

                mX->disableFileLogging();

                // Actual file name suffix should follow the pattern
                // YYYYMMDD_hhmmss and should be UTC time.
                size_t      pos = actualFileName.find("utc_") + 4;
                bsl::string year(actualFileName.substr(  pos +  0, 4));
                bsl::string month(actualFileName.substr( pos +  4, 2));
                bsl::string day(actualFileName.substr(   pos +  6, 2));
                bsl::string hour(actualFileName.substr(  pos +  9, 2));
                bsl::string minute(actualFileName.substr(pos + 11, 2));
                bsl::string second(actualFileName.substr(pos + 13, 2));

                bdlt::Datetime timestamp(bsl::atoi(year.c_str()),
                                         bsl::atoi(month.c_str()),
                                         bsl::atoi(day.c_str()),
                                         bsl::atoi(hour.c_str()),
                                         bsl::atoi(minute.c_str()),
                                         bsl::atoi(second.c_str()));

                if (veryVerbose) {
                    P(actualFileName);
                    P_(year); P_(month); P_(day);
                    P_(hour); P_(minute); P(second);
                }

                start.setMillisecond(0);
                start.setMicrosecond(0);
                ASSERTV(timestamp,
                        start,
                        end,
                        start <= timestamp && timestamp <= end);
            }

            {
                // Testing local timestamp in filename.
                bsl::string fileName(tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "local_%T");

                bdlt::Datetime start = bdlt::CurrentTime::local();

                mX->enablePublishInLocalTime();
                enableFileLogging(mX, fileName);

                BALL_LOG_INFO << "log";

                bdlt::Datetime end = bdlt::CurrentTime::local();

                bsl::string actualFileName;
                ASSERT(true == X->isFileLoggingEnabled(&actualFileName));

                mX->disableFileLogging();

                // Verify the time stamp in the filename.
                size_t      pos = actualFileName.find("local_") + 6;
                bsl::string year(actualFileName.substr(  pos +  0, 4));
                bsl::string month(actualFileName.substr( pos +  4, 2));
                bsl::string day(actualFileName.substr(   pos +  6, 2));
                bsl::string hour(actualFileName.substr(  pos +  9, 2));
                bsl::string minute(actualFileName.substr(pos + 11, 2));
                bsl::string second(actualFileName.substr(pos + 13, 2));

                bdlt::Datetime timestamp(bsl::atoi(year.c_str()),
                                         bsl::atoi(month.c_str()),
                                         bsl::atoi(day.c_str()),
                                         bsl::atoi(hour.c_str()),
                                         bsl::atoi(minute.c_str()),
                                         bsl::atoi(second.c_str()));

                if (veryVerbose) {
                    P(actualFileName);
                    P_(year); P_(month); P_(day);
                    P_(hour); P_(minute); P(second);
                }

                start.setMillisecond(0);
                start.setMicrosecond(0);
                ASSERTV(timestamp,
                        start,
                        end,
                        start <= timestamp && timestamp <= end);
            }

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING LOGGING TO A FAILING STREAM
        //
        // Concerns:
        //:  1 An error message is written to stderr when the stream fails.
        //
        // Plan:
        //:  1 Set the OS to limit the total size of any file we write to a
        //:    fairly small amount, then use BALL to write more than that.
        //:    While doing this, capture the stderr output and verify a message
        //:    has been written.
        //
        // Testing:
        //   CONCERN: LOGGING TO A FAILING STREAM
        // --------------------------------------------------------------------

        if (verbose) cerr << "\nTESTING LOGGING TO A FAILING STREAM."
                          << "\n====================================" << endl;

#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        // 'setrlimit' is not implemented on Cygwin.

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only see
        // each message only once.
        ball::LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        if (verbose) cout << "Testing output when the stream fails"
                          << " (UNIX only)."
                          << endl;
        {
            bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

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

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            bsl::stringstream os;

            // we want to capture the error message that will be written to
            // stderr (not cerr).  Redirect stderr to a file.  We can't
            // redirect it back; we'll have to use 'ASSERT2' (which outputs to
            // cout, not cerr) from now on and report a summary to cout at the
            // end of this case.

            bsl::string stderrFileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&stderrFileName, "stderrLog");

            ASSERT(stderr == freopen(stderrFileName.c_str(), "w", stderr));

            enableFileLogging(mX, fileName, true);

            for (int i = 0; i < 40; ++i) {
                BALL_LOG_TRACE << "log";
            }

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFileName.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line));  // caught an error message

            const bool fail = (bsl::string::npos ==
                                            line.find("Error on file stream"));
            ASSERT(!fail);
            if (fail) cout << "line: " << line << endl;

#ifndef BSLS_PLATFORM_CMP_IBM
            // On native IBM, after the error, even when the stream fails,
            // logging will be attempted over and over again, which results in
            // more than one error messages.
            ASSERT2(!getline(stderrFs, line));  // and only one message
#endif

            mX->disableFileLogging();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
#else
        if (verbose) {
            cout << "Skipping case 4 on Windows and Cygwin..." << endl;
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ROTATION METHODS
        //
        // Concerns:
        //:  1 'rotateOnSize' triggers a rotation when expected.
        //:
        //:  2 'disableSizeRotation' disables rotation on size.
        //:
        //:  3 'forceRotation' triggers a rotation.
        //:
        //:  4 'rotateOnLifetime' triggers a rotation when expected.
        //:
        //:  5 'disableLifetimeRotation' disables rotation on lifetime.
        //
        // Plan:
        //:  1 We will exercise both rotation rules to verify that they work
        //:    properly using glob to count the files and proper timing.  We
        //:    will also verify that the size rule is followed by checking the
        //:    size of log files.
        //
        // Testing:
        //   void disableLifetimeRotation();
        //   void disableSizeRotation();
        //   void forceRotation();
        //   void rotateOnSize(int size);
        //   void rotateOnLifetime(DatetimeInterval& interval);
        //   DatetimeInterval rotationLifetime() const;
        //   int rotationSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ROTATION METHODS"
                          << "\n========================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

#ifdef BSLS_PLATFORM_OS_UNIX
        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            if (verbose) cout << "Testing setup." << endl;
            {
                enableFileLogging(mX, fileName, true);

                BALL_LOG_TRACE << "log 1";

                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                globfree(&globbuf);
                ASSERT(fs.is_open());

                int         linesNum = 0;
                bsl::string line;

                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();

                ASSERT(2 == linesNum);
                ASSERT(true == X->isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;

            {
                ASSERT(bdlt::DatetimeInterval(0) == X->rotationLifetime());

                mX->rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,3));

                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X->rotationLifetime());

                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Check the number of lines in the file.

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[1], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);

                ASSERT(fs.is_open());

                int         linesNum = 0;
                bsl::string line(&ta);

                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();

                ASSERT(4 == linesNum);

                mX->disableLifetimeRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3";

                // Check that no rotation occurred.

                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
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
                mX->forceRotation();
                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";
                BALL_LOG_INFO  << "log 3";
                BALL_LOG_WARN  << "log 4";

                // Check that the rotation occurred.
                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(3 == globbuf.gl_pathc);

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[2], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());

                int         linesNum = 0;
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
                ASSERT(0 == X->rotationSize());
                mX->rotateOnSize(1);
                ASSERT(1 == X->rotationSize());
                for (int i = 0; i < 15; ++i) {
                    BALL_LOG_TRACE << "log";

                    // We sleep because otherwise, the loop is too fast to make
                    // the timestamp change so we cannot observe the rotation.

                    bslmt::ThreadUtil::microSleep(200 * 1000);
                }

                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(4 <= globbuf.gl_pathc);

                // We are not checking the last one since we do not have any
                // information on its size.

                for (int i = 0; i < (int)globbuf.gl_pathc - 3; ++i) {
                    ASSERT(1024 <
                                 FsUtil::getFileSize(globbuf.gl_pathv[i + 2]));
                }

                size_t oldNumFiles = globbuf.gl_pathc;
                globfree(&globbuf);

                ASSERT(1 == X->rotationSize());
                mX->disableSizeRotation();
                ASSERT(0 == X->rotationSize());

                for (int i = 0; i < 15; ++i) {
                    BALL_LOG_TRACE << "log";
                    bslmt::ThreadUtil::microSleep(50 * 1000);
                }

                // Verify that no rotation occurred.

                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(oldNumFiles == globbuf.gl_pathc);
                globfree(&globbuf);
            }

            mX->disableFileLogging();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }

        // Test with no timestamp.

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            // Create a temporary directory for log files.
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            if (verbose) cout << "Testing setup." << endl;
            {
                enableFileLogging(mX, fileName);

                BALL_LOG_TRACE << "log 1";

                glob_t globbuf;
                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                globfree(&globbuf);
                ASSERT(fs.is_open());

                int         linesNum = 0;
                bsl::string line;
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(2    == linesNum);
                ASSERT(true == X->isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) == X->rotationLifetime());

                mX->rotateOnLifetime(bdlt::DatetimeInterval(0,0,0,3));

                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X->rotationLifetime());

                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Check the number of lines in the file.

                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());

                int         linesNum = 0;
                bsl::string line(&ta);
                while (getline(fs, line)) {
                    ++linesNum;
                }
                fs.close();
                ASSERT(4 == linesNum);

                mX->disableLifetimeRotation();

                bslmt::ThreadUtil::microSleep(0, 4);

                BALL_LOG_FATAL << "log 3";

                // Check that no rotation occurred.

                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
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

            mX->disableFileLogging();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING THRESHOLDS AND OUTPUT FORMATS
        //
        // Concerns:
        //:  1 'publish' logs in the default format.
        //:
        //:  2 'publish' publishes all messages to a file if file logging is
        //:    enabled.
        //:
        //:  3 The name of the log file should be in accordance with what is
        //:    defined by the pattern if file logging is enabled by a pattern.
        //:
        //:  4 'setLogFileFunctor' can change the format effectively.
        //
        // Plan:
        //:  1 We will set up the observer and check if logged messages are in
        //:    the expected format and contain the expected data by comparing
        //:    the output of this observer with 'ball::StreamObserver' with a
        //:    record formatter matching the expected default format of
        //:    'ball::FileObserver2'.  Then, we will use different manipulators
        //:    and functors to affect output format and verify that it has
        //:    changed where expected.
        //
        // Testing:
        //   FileObserver(ball::Severity::Level, bslma::Allocator);
        //   ~FileObserver();
        //   void disableFileLogging();
        //   int  enableFileLogging(const char *fileName);
        //   int  enableFileLogging(const char *fileName, bool timestampFlag);
        //   void publish(const Record& record, const Context& context);
        //   void publish(const shared_ptr<Record>&, const Context&);
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
        //   DatetimeInterval localTimeOffset();
        //   bool isFileLoggingEnabled() const;
        //   bool isFileLoggingEnabled(bsl::string *result) const;
        //   bool isFileLoggingEnabled(std::string *result) const;
        //   bool isFileLoggingEnabled(std::pmr::string *result) const;
        //   bool isPublishInLocalTimeEnabled() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING THRESHOLDS AND OUTPUT FORMATS"
                             "\n=====================================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

// TBD fix this for Windows !!!
#ifndef BSLS_PLATFORM_OS_WINDOWS
#if (!defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR >= 10)\
  && !defined(BSLS_PLATFORM_OS_AIX)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        {

            if (verbose) cout << "Testing default output format." << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "testLog");

                bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X = mX;

                bsl::ostringstream dos;

                time_t    currentTime = time(0);
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
                                                         X->localTimeOffset());

                bsl::shared_ptr<ball::StreamObserver>
                                refX(new (ta) ball::StreamObserver(&dos), &ta);

                refX->setRecordFormatFunctor(
                  ball::RecordStringFormatter("\n%d %p:%t %s %f:%l %c %m \n"));

                ASSERT(0 == manager.registerObserver(mX,   "testObserver"));
                ASSERT(0 == manager.registerObserver(refX, "refObserver"));

                enableFileLogging(mX, fileName, true);

                BALL_LOG_WARN << "log WARN";

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, fileName, os));

                ASSERTV(os, dos.str(), dos.str() == os);
                mX->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver"));
                ASSERT(0 == manager.deregisterObserver("refObserver"));
            }

            if (verbose) cout << "Testing format with local time offset."
                              << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "testLog");

                bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X = mX;

                ASSERT(false == X->isPublishInLocalTimeEnabled());
                mX->enablePublishInLocalTime();
                ASSERT(true  == X->isPublishInLocalTimeEnabled());

                mX->disablePublishInLocalTime();
                ASSERT(false == X->isPublishInLocalTimeEnabled());

                mX->enablePublishInLocalTime();
                ASSERT(true == X->isPublishInLocalTimeEnabled());

                bsl::ostringstream testOs, dos;

                bsl::shared_ptr<ball::StreamObserver>
                                refX(new (ta) ball::StreamObserver(&dos), &ta);

                refX->setRecordFormatFunctor(
                  ball::RecordStringFormatter("\n%d %p:%t %s %f:%l %c %m \n"));

                ASSERT(0 == manager.registerObserver(mX,   "testObserver"));
                ASSERT(0 == manager.registerObserver(refX, "refObserver"));

                enableFileLogging(mX, fileName, true);

                BALL_LOG_FATAL << "log FATAL";

                testOs << " "
                       << bdls::ProcessUtil::getProcessId() << ":"
                       << bslmt::ThreadUtil::selfIdAsUint64()
                       << " FATAL " << __FILE__ << ":" << __LINE__ - 5 <<
                          " TestCategory log FATAL " << "\n";

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, fileName, os));

                // Verify the log message after the timestamp.
                bsl::string fileOutput(os);
                bsl::string streamOutput(dos.str());
                bsl::string logMessageExpected(testOs.str());

                fileOutput = fileOutput.substr(fileOutput.find(" "));
                streamOutput = streamOutput.substr(streamOutput.find(" "));

                ASSERTV(fileOutput, logMessageExpected,
                        fileOutput == logMessageExpected);
                ASSERTV(streamOutput, logMessageExpected,
                        streamOutput == logMessageExpected);


                // Now let's verify the actual difference.
                int defaultObsHour = 0;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(dos.str().substr(11, 2));
                    ASSERT(is >> defaultObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int fileObsHour = 0;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(os.substr(11, 2));
                    ASSERT(is >> fileObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }

#ifdef BSLS_PLATFORM_OS_UNIX
                tzset();
                time_t     currentTime;
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

                ASSERTV((fileObsHour + difference + 24) % 24,
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

                mX->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver"));
                ASSERT(0 == manager.deregisterObserver("refObserver"));
            }

            if (verbose) cout << "Testing file logging." << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "testLog");

                bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X = mX;

                ASSERT(-1 == mX->enableFileLogging("/bogus/path/foo"));

                ASSERT(0 == manager.registerObserver(mX, "testObserver"));

                enableFileLogging(mX, fileName);

                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";
                BALL_LOG_INFO  << "log 3";
                BALL_LOG_WARN  << "log 4";
                BALL_LOG_ERROR << "log 5";
                BALL_LOG_FATAL << "log 6";

                bsl::string os;
                ASSERT(12    == readFileIntoString(__LINE__, fileName, os));
                ASSERT(true  == X->isFileLoggingEnabled());

                mX->disableFileLogging();
                ASSERT(false == X->isFileLoggingEnabled());

                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";
                BALL_LOG_INFO  << "log 3";
                BALL_LOG_WARN  << "log 4";
                BALL_LOG_ERROR << "log 5";
                BALL_LOG_FATAL << "log 6";

                ASSERT(12 == readFileIntoString(__LINE__, fileName, os));

                enableFileLogging(mX, fileName);

                BALL_LOG_TRACE << "log 7";
                BALL_LOG_DEBUG << "log 8";
                BALL_LOG_INFO  << "log 9";
                BALL_LOG_WARN  << "log 1";
                BALL_LOG_ERROR << "log 2";
                BALL_LOG_FATAL << "log 3";

                ASSERT(24 == readFileIntoString(__LINE__, fileName, os));

                mX->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver"));
            }

#ifdef BSLS_PLATFORM_OS_UNIX
            if (verbose)
                cout << "Testing file logging with timestamp." << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              fileName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&fileName, "testLog");

                bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X = mX;

                ASSERT(0 == manager.registerObserver(mX, "testObserver"));

                enableFileLogging(mX, fileName, true);

                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";
                BALL_LOG_INFO  << "log 3";
                BALL_LOG_WARN  << "log 4";
                BALL_LOG_ERROR << "log 5";
                BALL_LOG_FATAL << "log 6";

                bsl::string os;
                ASSERT(12 == readFileIntoString(__LINE__, fileName, os));

                mX->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver"));
            }

            if (verbose) cout << "Testing '%%' in file name pattern." << endl;
            {
                static const struct {
                    int         d_lineNum;           // source line number
                    const char *d_patternSuffix_p;   // pattern suffix
                    const char *d_fileNameSuffix_p;  // filename suffix
                } DATA[] = {
                    //line  pattern suffix  filename suffix
                    //----  --------------  ---------------
                    { L_,   "foo",          "foo"           },
                    { L_,   "foo%",         "foo%"          },
                    { L_,   "foo%bar",      "foo%bar"       },
                    { L_,   "foo%%",        "foo"           },
                    { L_,   "foo%%bar",     "foobar"        },
                    { L_,   "foo%%%",       "foo%"          },
                    { L_,   "foo%%%bar",    "foo%bar"       },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE     = DATA[ti].d_lineNum;
                    const char *PATTERN  = DATA[ti].d_patternSuffix_p;
                    const char *FILENAME = DATA[ti].d_fileNameSuffix_p;

                    bdls::TempDirectoryGuard tempDirGuard("ball_");
                    bsl::string              baseName(
                                                tempDirGuard.getTempDirName());
                    bdls::PathUtil::appendRaw(&baseName, "testLog");

                    bsl::string pattern(baseName  + PATTERN);
                    bsl::string expected(baseName + FILENAME);

                    Obj mX(&ta);  const Obj& X = mX;

                    ASSERT(0 == mX.enableFileLogging(pattern.c_str()));

                    {
                        bsl::string actual;
                        ASSERTV(LINE, true == X.isFileLoggingEnabled(&actual));

                        if (veryVeryVerbose) {
                            P_(PATTERN);  P_(expected);  P(actual);
                        }

                        ASSERTV(LINE, expected == actual);
                    }
                    {
                        std::string actual;
                        ASSERTV(LINE, true == X.isFileLoggingEnabled(&actual));

                        if (veryVeryVerbose) {
                            P_(PATTERN);  P_(expected);  P(actual);
                        }

                        ASSERTV(LINE,
                                std::string(expected.cbegin(),
                                            expected.cend()) == actual);
                    }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    {
                        std::pmr::string actual;
                        ASSERTV(LINE, true == X.isFileLoggingEnabled(&actual));

                        if (veryVeryVerbose) {
                            P_(PATTERN);  P_(expected);  P(actual);
                        }

                        ASSERTV(LINE,
                                std::pmr::string(expected.cbegin(),
                                                 expected.cend()) == actual);
                    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

                    mX.disableFileLogging();

                    // look for the file with the expected name
                    glob_t globbuf;
                    ASSERTV(LINE, 0 == glob(expected.c_str(), 0, 0, &globbuf));
                    ASSERTV(LINE, 1 == globbuf.gl_pathc);
                    globfree(&globbuf);
                }
            }

            if (verbose) cout << "Testing functors with the same format."
                              << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              baseName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&baseName, "testLog");

                // Create observers.
                bsl::shared_ptr<Obj>       mX1(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X1 = mX1;
                bsl::shared_ptr<Obj>       mX2(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X2 = mX2;
                bsl::shared_ptr<Obj>       mX3(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X3 = mX3;

                // Register with logger manager.
                ASSERT(0 == manager.registerObserver(mX1, "testObserver1"));
                ASSERT(0 == manager.registerObserver(mX2, "testObserver2"));
                ASSERT(0 == manager.registerObserver(mX3, "testObserver3"));

                bsl::string fileName1 = baseName + "1";
                bsl::string fileName2 = baseName + "2";
                bsl::string fileName3 = baseName + "3";

                // Enable file logging and set the formatting functor.
                enableFileLogging(mX1, fileName1);

                enableFileLogging(mX2, fileName2);
                mX2->setLogFileFunctor(ball::RecordStringFormatter());

                enableFileLogging(mX3, fileName3);
                mX3->setLogFileFunctor(&logRecord1);

                BALL_LOG_WARN << "log";

                bsl::string file1, file2, file3;

                ASSERT(2 == readFileIntoString(__LINE__, fileName1, file1));
                ASSERT(2 == readFileIntoString(__LINE__, fileName2, file2));
                ASSERT(2 == readFileIntoString(__LINE__, fileName3, file3));

                if (veryVeryVerbose) { P_(file1);  P_(file2);  P(file3); }
                ASSERT(file1 == file2);
                ASSERT(file1 == file3);
                ASSERT(file2 == file3);

                mX1->disableFileLogging();
                mX2->disableFileLogging();
                mX3->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver1"));
                ASSERT(0 == manager.deregisterObserver("testObserver2"));
                ASSERT(0 == manager.deregisterObserver("testObserver3"));
            }

            if (verbose) cout << "Testing functors with different formats."
                              << endl;
            {
                bdls::TempDirectoryGuard tempDirGuard("ball_");
                bsl::string              baseName(
                                                tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&baseName, "testLog");

                // Create observers.
                bsl::shared_ptr<Obj>       mX1(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X1 = mX1;
                bsl::shared_ptr<Obj>       mX2(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X2 = mX2;
                bsl::shared_ptr<Obj>       mX3(new (ta) Obj(&ta), &ta);
                bsl::shared_ptr<const Obj> X3 = mX3;

                // Register with logger manager.
                ASSERT(0 == manager.registerObserver(mX1, "testObserver1"));
                ASSERT(0 == manager.registerObserver(mX2, "testObserver2"));
                ASSERT(0 == manager.registerObserver(mX3, "testObserver3"));

                bsl::string fileName1 = baseName + "1";
                bsl::string fileName2 = baseName + "2";
                bsl::string fileName3 = baseName + "3";

                // Enable file logging and set the formatting functor.
                enableFileLogging(mX1, fileName1);

                enableFileLogging(mX2, fileName2);
                mX2->setLogFileFunctor(ball::RecordStringFormatter(
                                                  "\n%p %t %s %f %c %m %u\n"));

                enableFileLogging(mX3, fileName3);
                mX3->setLogFileFunctor(&logRecord2);

                BALL_LOG_WARN << "log";

                bsl::string file1, file2, file3;

                ASSERT(2 == readFileIntoString(__LINE__, fileName1, file1));
                ASSERT(2 == readFileIntoString(__LINE__, fileName2, file2));
                ASSERT(2 == readFileIntoString(__LINE__, fileName3, file3));

                ASSERT(file1 != file2);
                ASSERT(file1 != file3);
                ASSERT(file2 != file3);

                // At this point we have three different logs produced by these
                // three file observers configured with different formats; now
                // we are going to reuse one of these fileobserver and change
                // its functors to see if the resulting log should be identical
                // to one of those known logs in file1, file2, and file3

                bsl::string fileName1a = baseName + "1a";
                bsl::string fileContent;

                mX1->disableFileLogging();
                enableFileLogging(mX1, fileName1a);
                mX1->setLogFileFunctor(ball::RecordStringFormatter(
                                                  "\n%p %t %s %f %c %m %u\n"));

                BALL_LOG_WARN << "log";

                ASSERT(2 == readFileIntoString(__LINE__, fileName1a,
                                               fileContent));
                ASSERT(file2 == fileContent);

                bsl::string fileName1b = baseName + "1b";

                mX1->disableFileLogging();
                enableFileLogging(mX1, fileName1b);
                mX1->setLogFileFunctor(&logRecord2);

                BALL_LOG_WARN << "log";

                ASSERT(2 == readFileIntoString(__LINE__, fileName1b,
                                               fileContent));
                ASSERT(file3 == fileContent);

                bsl::string fileName1c = baseName + "1c";

                mX1->disableFileLogging();
                enableFileLogging(mX1, fileName1c);

                mX1->setLogFileFunctor(ball::RecordStringFormatter(
                                                  "\n%p %t %s %f %c %m %u\n"));

                BALL_LOG_WARN << "log";

                ASSERT(2 == readFileIntoString(__LINE__, fileName1c,
                                               fileContent));
                ASSERT(file2 == fileContent);

                mX1->disableFileLogging();
                mX2->disableFileLogging();
                mX3->disableFileLogging();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("testObserver1"));
                ASSERT(0 == manager.deregisterObserver("testObserver2"));
                ASSERT(0 == manager.deregisterObserver("testObserver3"));
            }
#endif
        }
#endif
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

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_TRACE,
                                                       ball::Severity::e_OFF,
                                                       ball::Severity::e_OFF));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "testLog");

        bsl::shared_ptr<Obj>       mX(new (ta) Obj(&ta), &ta);
        bsl::shared_ptr<const Obj> X = mX;

        ASSERT(0 == manager.registerObserver(mX, "testObserver"));

        enableFileLogging(mX, fileName);

        if (veryVerbose) { P(fileName); }

        char buffer[1026];
        memset(buffer, 'x', sizeof buffer);
        buffer[sizeof buffer - 1] = '\0';

        // Write over 5 GB
        for (int i = 0; i < 5000000; ++i) {
            BALL_LOG_WARN << buffer;
        }

        mX->disableFileLogging();

        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("testObserver"));
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
