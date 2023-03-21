// ball_asyncfileobserver.t.cpp                                       -*-C++-*-
#include <ball_asyncfileobserver.h>

#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_streamobserver.h>

#include <bdlf_bind.h>
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bdls_processutil.h>
#include <bdls_tempdirectoryguard.h>

#include <bdlt_date.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_currenttime.h>
#include <bdlt_localtimeoffset.h>

#include <bslim_testutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmt_barrier.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>      // 'remove'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>       // 'time_t'
#include <bsl_iomanip.h>     // 'setfill'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_stdlib.h>    // 'unsetenv'

#include <sys/types.h>
#include <sys/stat.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <glob.h>
#include <bsl_c_signal.h>
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

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines an asynchronous observer
// ('ball::AsyncFileObserver') that writes log records to a file and stdout
// from a dedicated thread.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] AsyncFileObserver(bslma::Allocator *);
// [ 2] AsyncFileObserver(ball::Severity::Level, bslma::Allocator *);
// [ X] AsyncFileObserver(ball::Severity::Level, bool, bslma::Allocator *);
// [ 5] AsyncFileObserver(Severity::Level, bool, int, bslma::Allocator *);
// [ 5] AsyncFileObserver(Severity, bool, int, Severity, Allocator *);
// [ 2] ~AsyncFileObserver();
//
// MANIPULATORS
// [ 1] void disableFileLogging();
// [ X] void disablePublishInLocalTime();
// [ 6] void disableSizeRotation();
// [ 1] void disableStdoutLoggingPrefix();
// [ 6] void disableTimeIntervalRotation();
// [ 1] int enableFileLogging(const char *logFilenamePattern);
// [ 1] void enableStdoutLoggingPrefix();
// [ 1] void enablePublishInLocalTime();
// [ 6] void forceRotation();
// [ 1] void publish(const Record& record, const Context& context);
// [ 1] void publish(const shared_ptr<Record>&, const Context&);
// [ 4] void releaseRecords();
// [ 6] void forceRotation();
// [ 6] void rotateOnSize(int size);
// [ 6] void rotateOnTimeInterval(const DatetimeInterval timeInterval);
// [ 6] void rotateOnTimeInterval(const DatetimeI&, const Datetime&);
// [ 1] void setLogFormat(const char* logF, const char* stdoutF);
// [ 8] void setOnFileRotationCallback(const OnFileRotationCallback&);
// [ 1] void setStdoutThreshold(ball::Severity::Level stdoutThreshold);
// [ 3] void shutdownPublicationThread();
// [ 3] void startPublicationThread();
// [ 3] void stopPublicationThread();
//
// ACCESSORS
// [ 1] void getLogFormat(const char** logF, const char** stdoutF) const;
// [ 1] bool isFileLoggingEnabled() const;
// [ 1] bool isFileLoggingEnabled(bsl::string *result) const;
// [ 3] bool isPublicationThreadRunning() const;
// [ 1] bool isPublishInLocalTimeEnabled() const;
// [ 1] bool isStdoutLoggingPrefixEnabled() const;
// [ 1] bool isUserFieldsLoggingEnabled() const;
// [11] int recordQueueLength() const;
// [ 6] bdlt::DatetimeInterval rotationLifetime() const;
// [ 6] int rotationSize() const;
// [ 1] ball::Severity::Level stdoutThreshold() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] CONCERN: MEMORY ACCESS AFTER RELEASERECORDS
// [12] CONCERN: DEADLOCK ON RELEASERECORDS (DRQS 164688087)
// [10] CONCERN: CONCURRENT PUBLICATION
// [ 7] CONCERN: LOGGING TO A FAILING STREAM
// [ 5] CONCERN: LOG MESSAGE DROP
// [ 9] CONCERN: ROTATION
// [14] USAGE EXAMPLE

// Note assert and debug macros all output to 'cerr' instead of cout, unlike
// most other test drivers.  This is necessary because test case 2 plays tricks
// with cout and examines what is written there.

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cerr << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

void aSsErT2(bool condition, const char *message, int line)
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

typedef ball::AsyncFileObserver      Obj;
typedef bdls::FilesystemUtil         FsUtil;
typedef bdls::FilesystemUtil::Offset Offset;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

bsl::shared_ptr<ball::Record> createRecord(const bsl::string&     message,
                                           ball::Severity::Level  severity,
                                           bslma::Allocator      *allocator)
    // Return a newly created `ball::Record` having the specifed 'message' and
    // 'severity', and using the specified 'allocator' to allocate memory.  The
    // created 'ball::Record' will have valid but unspecified values for the other record
    // fields.
{
    bsl::shared_ptr<ball::Record> result =
        bsl::allocate_shared<ball::Record>(allocator);

    result->fixedFields().setCategory("asyncfileobserver.t");
    result->fixedFields().setFileName(__FILE__);
    result->fixedFields().setLineNumber(__LINE__);
    result->fixedFields().setMessage(message.c_str());
    result->fixedFields().setProcessID(bdls::ProcessUtil::getProcessId());
    result->fixedFields().setThreadID(bslmt::ThreadUtil::selfIdAsUint64());
    result->fixedFields().setSeverity(severity);
    result->fixedFields().setTimestamp(bdlt::CurrentTime::utc());

    return result;
}

bsl::string::size_type replaceSecondSpace(bsl::string *input, char value)
    // Replace the second space character (' ') in the specified 'input' string
    // with the specified 'value'.  Return the index position of the character
    // that was replaced on success, and 'bsl::string::npos' otherwise.
{
    bsl::string::size_type index = input->find(' ');
    if (bsl::string::npos != index) {
        index = input->find(' ', index + 1);
        if (bsl::string::npos != index) {
            (*input)[index] = value;
        }
    }
    return index;
}

bdlt::Datetime getCurrentTimestamp()
    // Return current local time as 'bdlt::Datetime' value.
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

void removeFilesByPrefix(const char *prefix)
    // Remove the files with the specified 'prefix'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    bsl::string filename(prefix);

    filename += "*";
    WIN32_FIND_DATA findFileData;

    bsl::vector<bsl::string> fileNames;
    HANDLE hFind = FindFirstFile(filename.c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        fileNames.push_back(findFileData.cFileName);
        while(FindNextFile(hFind, &findFileData)) {
            fileNames.push_back(findFileData.cFileName);
        }
        FindClose(hFind);
    }

    char tmpPathBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    bsl::string tmpPath(tmpPathBuf);

    bsl::vector<bsl::string>::iterator itr;
    for (itr = fileNames.begin(); itr != fileNames.end(); ++itr) {
        bsl::string fn = tmpPath + (*itr);
        if (!DeleteFile(fn.c_str()))
        {
            LPVOID lpMsgBuf;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL);
            cerr << "Error, " << (char*)lpMsgBuf << endl;
            LocalFree(lpMsgBuf);
        }
    }
#else
    glob_t      globbuf;
    bsl::string filename(prefix);

    filename += "*";
    glob(filename.c_str(), 0, 0, &globbuf);

    for (size_t i = 0; i < globbuf.gl_pathc; i++) {
        unlink(globbuf.gl_pathv[i]);
    }

    globfree(&globbuf);
#endif
}

bsl::string readPartialFile(bsl::string& fileName, Offset startOffset)
    // Read the content of a file with the specified 'fileName' starting at the
    // specified 'startOffset' to the end-of-file and return it as a string.
{
    bsl::string result;
    result.reserve(static_cast<bsl::string::size_type>(
                             FsUtil::getFileSize(fileName) + 1 - startOffset));

    FILE *fp = fopen(fileName.c_str(), "r");
    BSLS_ASSERT_OPT(fp);

    int rc = fseek(fp, static_cast<long>(startOffset), SEEK_SET);
    BSLS_ASSERT_OPT(0 == rc);

    int c;
    while (EOF != (c = getc(fp))) {
        result += static_cast<char>(c);
    }

    fclose(fp);

    return result;
}

int countLoggedRecords(const bsl::string& fileName)
    // Return the number of log records in a file with the specified
    // 'fileName'.
{
    bsl::string   line;
    int           numLines = 0;
    bsl::ifstream fs;

    fs.open(fileName.c_str(), bsl::ifstream::in);

    ASSERT(fs.is_open());

    while (getline(fs, line)) {
        ++numLines;
    }
    fs.close();

    // Note that we divide 'numLines' by 2 because there are 2 lines written
    // to the log file for each logged record (when using the default record
    // formatter typically used in this test driver).

    return numLines / 2;
}

void waitEmptyRecordQueue(
                       bsl::shared_ptr<const ball::AsyncFileObserver> observer)
    // Wait (for up to 5 seconds) until the specified 'observer' drains its
    // record queue (by processing all pending log records).
{
    bsls::Stopwatch timer;
    timer.start();

    do {
        bslmt::ThreadUtil::microSleep(1000, 0);
    } while (observer->recordQueueLength() > 0
             && timer.elapsedTime() < 5);

    ASSERTV(timer.elapsedTime(),
            observer->recordQueueLength(),
            0 == observer->recordQueueLength());

    bslmt::ThreadUtil::microSleep(1000, 0);
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
        // function-call operation, or 'UNINITIALIZED' if 'numInvocations' is
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

}  // close unnamed namespace


namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY {

void executeInParallel(int                                numThreads,
                       bslmt::ThreadUtil::ThreadFunction  function)
    // Create the specified 'numThreads', each executing the specified 'func'.
{
    bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], function, 0);
    }

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

extern "C" void *workerThread(void *arg)
{
    (void)arg;

    BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

    for (int i = 0; i < 10000; ++i) {
        BALL_LOG_TRACE << "ball::AsyncFileObserver Concurrency Test.";
    }
    return 0;
}

extern "C" void *workerThread2(void *arg)
{
    (void) arg;

    BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

    bsl::shared_ptr<ball::AsyncFileObserver> observer;

    ASSERT(0 == ball::LoggerManager::singleton().findObserver(
                                                             &observer,
                                                             "asyncObserver"));

    int ret;
    for (int i = 0; i < 100; ++i) {
        ret = observer->startPublicationThread();
        ASSERT(0 == ret);
        for (int j = 0; j < 1000; ++j) {
            BALL_LOG_TRACE << "ball::AsyncFileObserver Concurrency Test.";
        }

        // Test both stopPublicationThread and shutdownPublicationThread

        if (i % 2)
            ret = observer->stopPublicationThread();
        else
            ret = observer->shutdownPublicationThread();
        ASSERT(0 == ret);
        ret = observer->startPublicationThread();
        ASSERT(0 == ret);
    }
    return 0;
}

}  // close namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY

namespace BALL_ASYNCFILEOBSERVER_RELEASERECORDS_TEST {

void publisher(ball::AsyncFileObserver *observer,
               bsls::AtomicInt         *releaseCounter,
               bslmt::Barrier          *barrier)
    // Publish arbitrary log records to the specifeid 'observer' until the
    // specified 'releaseCounter' is 0, using the specified 'barrier' to
    // synchronize the start and completetion of the publication of records.
    // Note that this method is designed to be the entry point function of a
    // thread that publishes many records, while a second thread in the
    // corresponding 'releaser' test function (below) concurrently calls the
    // 'releaseRecords' function.
{
    bsl::shared_ptr<ball::Record> record = createRecord(
        "test", ball::Severity::e_ERROR, bslma::Default::allocator());

    ball::Context context;
    barrier->wait();
    while (*releaseCounter > 0) {
        observer->publish(record, context);
    }
    barrier->wait();
}

void releaser(ball::AsyncFileObserver *observer,
              bsls::AtomicInt         *releaseCounter,
              bslmt::Barrier          *barrier)
    // Call 'releaseRecords' on the specified 'observer' the specified
    // 'releaseCounter' number of times, each time decrementing
    // 'releaseCounter'; use the specified 'barrier' to synchronize the start
    // and completion of the series of calls to 'releaseRecords'.  Note that
    // this method is designed to be the entry point function of a thread that
    // calls 'releaseRecords' many times, while a second thread in the
    // corresponding 'publisher' test function (above) concurrently calls the
    // 'publish' function.
{
    barrier->wait();
    while (*releaseCounter > 0) {
        observer->releaseRecords();
        bslmt::ThreadUtil::microSleep(100, 0);
        (*releaseCounter)--;
    }
    barrier->wait();
}

}  // close namespace BALL_ASYNCFILEOBSERVER_RELEASERECORDS_TEST

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int  test           = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl << flush;

    bslma::TestAllocator  allocator;
    bslma::TestAllocator *Z = &allocator;

    switch (test) { case 0:
      case 15: {
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
        if (verbose) cout << "\nUSAGE EXAMPLE."
                          << "\n==============" << endl;

        // This is standard preamble to create the directory and filename for
        // the test.
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "testLog");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through the Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::AsyncFileObserver' within the
// 'ball' logging system.
//
// First, we initialize the 'ball' logging subsystem with the default
// configuration:
//..
    ball::LoggerManagerConfiguration configuration;
    ball::LoggerManagerScopedGuard   guard(configuration);

    ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// records will be discarded.
//
// Then, we create a shared pointer to a 'ball::AsyncFileObserver' object,
// 'observer', having default attributes.  Note that a default-constructed
// async file observer has a maximum (fixed) size of 8192 for its log record
// queue and will drop incoming log records when that queue is full.  (See {Log
// Record Queue} for further information.)
//..
    bsl::shared_ptr<ball::AsyncFileObserver> observer =
                                   bsl::make_shared<ball::AsyncFileObserver>();
//..
// Next, we set the required logging format by calling the 'setLogFormat'
// method.  The statement below outputs timestamps in ISO 8601 format to a log
// file and in 'bdlt'-style (default) format to 'stdout', where timestamps are
// output with millisecond precision in both cases:
//..
    observer->setLogFormat("%I %p:%t %s %f:%l %c %m\n",
                           "%d %p:%t %s %f:%l %c %m\n");
//..
// Note that both of the above format specifications omit user fields ('%u') in
// the output.  Also note that, unlike the default, this format does not emit a
// blank line between consecutive log messages.
//
// Next, we start the publication thread by invoking 'startPublicationThread':
//..
    observer->startPublicationThread();
//..
// Then, we register the async file observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// the 'publish' method:
//..
    int rc = manager.registerObserver(observer, "asyncObserver");
    ASSERT(0 == rc);
//..
// Next, we set the log category and log a few records with different logging
// severity.  By default, only the records with 'e_WARN', 'e_ERROR', or
// 'e_FATAL' severity will be logged to 'stdout'.  Note that logging to a file
// is not enabled by default:
//..
    BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

    BALL_LOG_INFO << "Will not be published on 'stdout'.";
    BALL_LOG_WARN << "This warning *will* be published on 'stdout'.";
//..
// Then, we change the default severity for logging to 'stdout' by calling the
// 'setStdoutThreshold' method:
//..
    observer->setStdoutThreshold(ball::Severity::e_INFO);

    BALL_LOG_DEBUG << "This debug message is not published on 'stdout'.";
    BALL_LOG_INFO  << "This info will be published on 'stdout'.";
    BALL_LOG_WARN  << "This warning will be published on 'stdout'.";
//..
// Next, we disable logging to 'stdout' and enable logging to a file:
//..
    observer->setStdoutThreshold(ball::Severity::e_OFF);

    observer->enableFileLogging(fileName.c_str());  // test driver only
//..
// Note that logs are now asynchronously written to the file.
//
// Then, we specify rules for log file rotation based on the size and time
// interval:
//..
    // Rotate the file when its size becomes greater than or equal to 32
    // megabytes.
    observer->rotateOnSize(1024 * 32);

    // Rotate the file every 24 hours.
    observer->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).
//
// Next, we demonstrate how to correctly shut down the async file observer.  We
// first stop the publication thread by explicitly calling the
// 'stopPublicationThread' method.  This method blocks until all the log
// records that were on the record queue on entry to 'stopPublicationThread'
// have been published:
//..
    observer->stopPublicationThread();
//..
// Then, we disable the log rotation rules established earlier and also
// completely disable logging to a file:
//..
    observer->disableSizeRotation();

    observer->disableTimeIntervalRotation();

    observer->disableFileLogging();
//..
// Note that stopping the publication thread and disabling various features of
// the async file observer is not strictly necessary before object destruction.
// In particular, if a publication thread is still running when the destructor
// is invoked, all records on the record queue upon entry are published and
// then the publication thread is automatically stopped before destroying the
// async file observer.  In any case, all resources managed by the async file
// observer will be released when the object is destroyed.
//
// Finally, we can deregister our async file observer from the 'ball' logging
// subsystem entirely (and destroy the observer later):
//..
    rc = manager.deregisterObserver("asyncObserver");
    ASSERT(0 == rc);
//..

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING SUPPRESS UNIQUE FILE NAME ON ROTATION
        //
        // Concerns:
        //: 1 'suppressUniqueFileNameOnRotation(true)' suppresses unique
        //:   filename generation for the rotated log file on rotation.
        //
        // Plan:
        //:  Call 'suppressUniqueFileNameOnRotation(true)'.  Force rotation and
        //:  ensure that the rotation took place and the log file name and
        //:  rotated filename are the same.
        //
        // Testing:
        //   CONCERN: suppressUniqueFileNameOnRotation(bool);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING SUPPRESS UNIQUE FILE NAME"
                          << "\n================================="  << endl;

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

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        bsl::shared_ptr<Obj> mX = bsl::allocate_shared<Obj>(
                                                       &ta,
                                                       ball::Severity::e_WARN,
                                                       &ta);

        ASSERT(0 == manager.registerObserver(mX, "testObserver"));

        // Set callback to monitor rotation.
        RotCb cb(&ta);
        mX->setOnFileRotationCallback(cb);
        if (veryVerbose) cout << "\tTesting suppressing rotated file name "
                                 "uniqueness" << endl;
        {
            // Temporary directory for test files.
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            ASSERT(0 == mX->enableFileLogging(fileName.c_str()));

            BALL_LOG_TRACE << "log";
            ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

            mX->suppressUniqueFileNameOnRotation(true);

            mX->forceRotation();
            BALL_LOG_TRACE << "log";

            ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERTV(fileName,   cb.rotatedFileName(),
                    fileName == cb.rotatedFileName());

            mX->disableFileLogging();

            mX->suppressUniqueFileNameOnRotation(false);

            cb.reset();
        }

        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("testObserver"));
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCERN: MEMORY ACCESS AFTER RELEASERECORDS
        //
        // Test case for DRQS 165535115.
        //
        // Concerns:
        //:  1 No memory refering to records remains after a call to
        //:    'releaseRecords'.  This includes the publication thread
        //:    holding a (still valid shared_ptr) to a record it is
        //:    activel published.  Note that, on destruction, the logger
        //:    manager destroys its pool of memory for records after calling
        //:    'releaseRecords' (so any 'shared_ptr' objects will refer to
        //:    deallocated memory, even if they remain valid).
        //
        // Plan:
        //:  1 Create a async-file observer, and publish a series a
        //:    of shared_ptr<Record> objects allocated from a memory
        //:    pool.  Call 'releaseRecords' and then destroy the pool, without
        //:    stopping the publication thread.
        //
        // Testing:
        //   CONCERN: MEMORY ACCESS AFTER RELEASERECORDS
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nCONCERN: MEMORY ACCESS AFTER RELEASERECORDS"
                 << "\n==========================================="
                 << endl;

        bsl::string LONG_MESSAGE(5000, 'x');
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName,
                                  "asyncfileobserver.t.cpp.case.13");


        for (int i = 0; i < 5; ++i) {
            bslma::TestAllocator oa; // object-allocator

            Obj mX(&oa);
            mX.startPublicationThread();
            mX.enableFileLogging(fileName.c_str());
            {
                // To reproduce the crash, the allocator for the records should
                // be destroyed, and at least in practice it helps if that
                // allocator is on the heap.

                bslma::TestAllocator *ra = new bslma::TestAllocator();
                ball::Context context;
                bsl::shared_ptr<ball::Record> record =
                    createRecord(LONG_MESSAGE, ball::Severity::e_INFO, ra);
                for (int numRecords = 0; numRecords < 100; ++numRecords) {
                    mX.publish(record, context);
                }
                record.reset();
                mX.releaseRecords();

                // NOTE: Test allocator 'ra' will be destroyed here while the
                // publication thread continues.

                delete ra;
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCERN: DEADLOCK ON RELEASERECORDS (DRQS 164688087)
        //
        // Concerns:
        //:  1 When the queue of an async-file observer is almost full,
        //:    calling 'releaseRecords' will not block the task
        //:    (previously 'stopThread' would use 'pushBack' which would
        //:    block when the queue is full).
        //
        // Plan:
        //:  1 Create a async-file observer with a very small queue.  Start
        //:    one thread publishing records (filling the queue), start
        //:    a second thread calling 'releaseRecords', which attempts
        //:    to start and then re-start the publication thread. (C-1)
        //
        // Testing:
        //   CONCERN: DEADLOCK ON RELEASERECORDS (DRQS 164688087)
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nCONCERN: DEADLOCK ON RELEASERECORDS (DRQS 164688087)"
                 << "\n===================================================="
                 << endl;


        // Create an observer with a very small queue size.
        Obj mX(ball::Severity::e_FATAL, false, 2);

        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName,
                                  "asyncfileobserver.t.cpp.case.12");
        mX.enableFileLogging(fileName.c_str());
        mX.startPublicationThread();

        using namespace BALL_ASYNCFILEOBSERVER_RELEASERECORDS_TEST;

        bslmt::Barrier barrier(3);
        bsls::AtomicInt releaseCounter(500);

        bsl::function<void()> publisherFunctor =
            bdlf::BindUtil::bind(&publisher, &mX, &releaseCounter, &barrier);
        bsl::function<void()> releaserFunctor =
            bdlf::BindUtil::bind(&releaser, &mX, &releaseCounter, &barrier);

        bslmt::ThreadUtil::Handle publishThread, releaseThread;
        bslmt::ThreadUtil::create(&publishThread, publisherFunctor);
        bslmt::ThreadUtil::create(&releaseThread, releaserFunctor);

        bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();
        bsls::TimeInterval timeout = start + bsls::TimeInterval(10);
        barrier.wait();  // Start of the test.

        int rc;
        do {
            rc = barrier.timedWait(timeout);
        } while (0 != rc && bsls::SystemTime::nowMonotonicClock() < timeout);

        if (0 != rc) {
            ASSERTV(0 &&
                    "FAILURE: case 12 'releaseRecords' timed out (deadlock?)");
            bsl::exit(testStatus);
        }

        bslmt::ThreadUtil::join(publishThread);
        bslmt::ThreadUtil::join(releaseThread);

        mX.stopPublicationThread();

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'recordQueueLength'
        //  Note that this is a white box text, in that 'recordQueueLength'
        //  delegates to 'bdlcc_fixedqueue'.  This test verifies that records
        //  are added from and removed from the queue correctly (and the length
        //  reflects the queue size), and a sanity test for concurrent access.
        //  Exhaustive testing of the thread-safety is left to
        //  'bdlcc_fixedqueue'.
        //
        // Concerns:
        //:  1 'recordQueueLength' returns the current number of log records
        //:    that have been published, but not yet written to the file log.
        //:
        //:  2 That 'recordQueueLength' may be called concurrently with record
        //:    publication.
        //
        // Plan:
        //:  1 Create a async-file observer, publish a series of records,
        //:    and verify the updated 'recordQueueLength' correctly increments
        //:    as records are published.  Start asynchronous record
        //:    publication, then stop it, and verify the length of the record
        //:    queue reflects the number of records that have been published
        //:    to the log file (and removed from the queue). (C-1)
        //:
        //:  2 Create a async-file observer, start asynchronous publication,
        //:    and, for a number of iterations, publish a series of records,
        //:    and then repeatedly call 'recordQueueLength' and sanity test
        //:    the returned value (it should be decreasing) until the record
        //:    queue is empty. (C-2)
        //
        // Testing:
        //   int recordQueueLength() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'recordQueueLength'."
                          << "\n============================" << endl;

        const int ERROR = ball::Severity::e_ERROR;

        if (veryVerbose) {
            cout << "\tTesting basic 'recordQueueLength' behavior" << endl;
        }
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bslma::TestAllocator ta(veryVeryVeryVerbose);

            enum { MAX_QUEUE_LENGTH = 1024 };

            // Set up a non-blocking async observer
            Obj        mX(ball::Severity::e_FATAL,
                          false,
                          MAX_QUEUE_LENGTH,
                          ball::Severity::e_TRACE,
                          &ta);
            const Obj& X = mX;

            bsl::shared_ptr<ball::Record> record;
            record.createInplace(&ta);
            record->fixedFields().setSeverity(ERROR);
            ball::Context context;

            for (size_t i = 0; i < MAX_QUEUE_LENGTH; ++i) {
                ASSERT(i == X.recordQueueLength());

                mX.publish(record, context);
            }
            ASSERT(MAX_QUEUE_LENGTH == X.recordQueueLength());

            mX.enableFileLogging(fileName.c_str());

            mX.startPublicationThread();

            // Allow 20 seconds to complete file write.
            bsls::Stopwatch timer;
            timer.start();

            do {
                bslmt::ThreadUtil::microSleep(1000, 0);
            } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 20 );
            timer.stop();

            ASSERTV(X.recordQueueLength(), 0 == X.recordQueueLength());
            ASSERTV(timer.elapsedTime(),   timer.elapsedTime() < 20 );

            mX.disableFileLogging();
            mX.shutdownPublicationThread();
        }

        if (veryVerbose) {
            cout << "\tCall 'recordQueueLength' concurrently with publication"
                 << endl;
        }
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bslma::TestAllocator ta(veryVeryVeryVerbose);

            enum { MAX_QUEUE_LENGTH = 10000 };
            enum { NUM_ITERATIONS = 10 };

            // Set up a non-blocking async observer
            Obj        mX(ball::Severity::e_FATAL,
                          false,
                          MAX_QUEUE_LENGTH,
                          ball::Severity::e_TRACE,
                          &ta);
            const Obj& X = mX;

            mX.enableFileLogging(fileName.c_str());
            mX.startPublicationThread();

            for (int iteration = 0; iteration < NUM_ITERATIONS; ++iteration) {
                bsl::shared_ptr<ball::Record> record;
                record.createInplace(&ta);
                record->fixedFields().setSeverity(ERROR);
                ball::Context context;

                for (int i = 0; i < MAX_QUEUE_LENGTH; ++i) {
                    mX.publish(record, context);
                }

                ASSERTV(X.recordQueueLength(), 0 < X.recordQueueLength());

                // Allow 20 seconds to complete file write.
                bsls::Stopwatch timer;
                timer.start();

                size_t prevQueueLength = X.recordQueueLength();
                do {
                    size_t queueLength = X.recordQueueLength();
                    ASSERT(queueLength <= prevQueueLength);
                    prevQueueLength = queueLength;
                } while (X.recordQueueLength() > 0
                         && timer.elapsedTime() < 20 );
                timer.stop();

                ASSERTV(X.recordQueueLength(), 0 == X.recordQueueLength());
                ASSERTV(timer.elapsedTime(),   timer.elapsedTime() < 20 );
            }
            mX.shutdownPublicationThread();
            mX.disableFileLogging();
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT PUBLICATION
        //
        // Concerns:
        //:  1 Concurrent calls to 'publish' should work correctly that writes
        //:    records from different threads into same log file in defined
        //:    format.
        //
        // Plan:
        //:  1 Concurrently invoke 'publish' of a fair large amount of records
        //:    from threads.  Verify that all the records from all threads are
        //:    written into log file and the format is not broken.
        //
        // Testing:
        //   CONCERN: CONCURRENT PUBLICATION
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING CONCURRENT PUBLICATION."
                          << "\n===============================" << endl;

        using namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY;

        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "testLog");

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        // Set up a blocking async observer.
        bsl::shared_ptr<Obj>       mX(new(ta) Obj(ball::Severity::e_WARN,
                                                  false,
                                                  8192,
                                                  ball::Severity::e_TRACE,
                                                  &ta),
                                      &ta);
        bsl::shared_ptr<const Obj> X = mX;

        mX->startPublicationThread();

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        mX->enableFileLogging(fileName.c_str());

        ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));
        ASSERT(0 == FsUtil::getFileSize(fileName));

        int numThreads = 4;

        // First test if concurrent publish is correct, check the total number
        // of lines afterwards.

        if (verbose) cout << "Running first concurrency test." << endl;

        executeInParallel(numThreads, workerThread);

        mX->stopPublicationThread();
        mX->disableFileLogging();

        int numRecords = countLoggedRecords(fileName);
        ASSERT(numRecords == 10000 * numThreads);

        // Next test if all thread-safe public methods can be called
        // concurrently without crash.

        if (verbose) cout << "Running second concurrency test." << endl;
        executeInParallel(numThreads, workerThread2);

        mX->stopPublicationThread();
        mX->disableFileLogging();

        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("asyncObserver"));

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING TIME-BASED ROTATION
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' correctly forward call to
        //:   'ball::FileObserver2'.
        //
        // Plan:
        //: 1 Setup test infrastructure.
        //:
        //: 2 Call 'rotateOnTimeInterval' with a large interval and a reference
        //:   time such that the next rotation will occur soon.  Verify that
        //:   rotation occurs on the scheduled time.
        //:
        //: 3 Call 'disableTimeIntervalRotation' and verify that no rotation
        //:   occurs afterwards.
        //
        // Testing:
        //   CONCERN: ROTATION
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING TIME-BASED ROTATION"
                          << "\n===========================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        bsl::shared_ptr<Obj>       mX(new(ta) Obj(ball::Severity::e_WARN, &ta),
                                      &ta);
        bsl::shared_ptr<const Obj> X = mX;

        ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

        mX->startPublicationThread();

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX->setOnFileRotationCallback(cb);

        // Create temporary directory for log files.
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "testLog");

        ASSERT(0    == mX->enableFileLogging(fileName.c_str()));
        ASSERT(true == X->isFileLoggingEnabled());
        ASSERT(0    == cb.numInvocations());

        BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

        if (veryVerbose) cout << "Testing absolute time reference" << endl;
        {
            // Reset reference start time.

            mX->disableFileLogging();

            for (int i = 0; i < 2; ++i) {
                bdlt::Datetime startTime;
                if (i == 0) {
                    mX->enablePublishInLocalTime();
                    startTime = bdlt::CurrentTime::local();
                }
                else {
                    mX->disablePublishInLocalTime();
                    startTime = bdlt::CurrentTime::utc();
                }

                startTime += bdlt::DatetimeInterval(-1, 0, 0, 3);
                mX->rotateOnTimeInterval(bdlt::DatetimeInterval(1), startTime);
                ASSERT(0 == mX->enableFileLogging(fileName.c_str()));

                BALL_LOG_TRACE << "log";
                bslmt::ThreadUtil::microSleep(0, 1);
                ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());

                bslmt::ThreadUtil::microSleep(0, 3);

                BALL_LOG_TRACE << "log";
                bslmt::ThreadUtil::microSleep(0, 1);

                // Wait up to 3 seconds for the file rotation to complete.

                int loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                } while (0 == cb.numInvocations() && loopCount++ < 3);

                ASSERTV(cb.numInvocations(), 1 == cb.numInvocations());

                ASSERT(true == FsUtil::exists(cb.rotatedFileName().c_str()));

                cb.reset();
                mX->disableFileLogging();
            }
        }

        if (veryVerbose) cout << "Testing 'disableTimeIntervalRotation'"
                              << endl;
        {
            cb.reset();

            mX->disableTimeIntervalRotation();
            bslmt::ThreadUtil::microSleep(0, 3);

            BALL_LOG_TRACE << "log";
            bslmt::ThreadUtil::microSleep(0, 1);

            ASSERTV(cb.numInvocations(), 0 == cb.numInvocations());
        }
        mX->stopPublicationThread();
        mX->disableFileLogging();

        // Deregister here as we used local allocator for the observer.
        ASSERT(0 == manager.deregisterObserver("asyncObserver"));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'setOnFileRotationCallback'
        //
        // Concerns:
        //: 1 'setOnFileRotationCallback' is properly forwarded to the
        //:   corresponding function in 'ball::FileObserver2'
        //
        // Plan:
        //: 1 Setup callback with 'setOnFileRotationCallback' and verify that
        //:   the callback is invoked on rotation.
        //
        // Testing:
        //  void setOnFileRotationCallback(const OnFileRotationCallback&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'setOnFileRotationCallback'."
                          << "\n====================================" << endl;

        // Create temporary directory for log files.
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "testLog");

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(ball::Severity::e_WARN, &ta);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        ASSERT(0 == cb.numInvocations());

        mX.enableFileLogging(fileName.c_str());
        mX.forceRotation();

        ASSERT(1 == cb.numInvocations());
        mX.disableFileLogging();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING LOGGING TO A FAILING STREAM
        //
        // Concerns:
        //: 1 Logging can be done to a stream that fails.
        //
        // Plan:
        //: 1 Emulate stream failure.
        //
        // Testing:
        //   CONCERN: LOGGING TO A FAILING STREAM
        // --------------------------------------------------------------------

        if (verbose) cerr << "\nTESTING LOGGING TO A FAILING STREAM."
                          << "\n====================================" << endl;

#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        // 'setrlimit' is not implemented on Cygwin.

        // Don't run this if we're in the debugger because the debugger stops
        // and refuses to continue when we hit the file size limit.

        bslma::TestAllocator ta;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        // Create a temporary directory for a log files.
        bdls::TempDirectoryGuard tempDirGuard("ball_");

        {

            bsl::stringstream os;

            struct rlimit rlim;
            ASSERT(0 == getrlimit(RLIMIT_FSIZE, &rlim));
            rlim.rlim_cur = 2048;
            ASSERT(0 == setrlimit(RLIMIT_FSIZE, &rlim));

            struct sigaction act,oact;
            act.sa_handler = SIG_IGN;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ASSERT(0 == sigaction(SIGXFSZ, &act, &oact));

            bsl::shared_ptr<Obj>       mX(new(ta) Obj(ball::Severity::e_OFF,
                                                      true,
                                                      8192,
                                                      &ta),
                                          &ta);
            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            // We want to capture the error message that will be written to
            // stderr (not cerr).  Redirect stderr to a file.  We can't
            // redirect it back; we'll have to use 'ASSERT2' (which outputs to
            // cout, not cerr) from now on and report a summary to cout at the
            // end of this case.

            bsl::string stderrFileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&stderrFileName, "strderrLog");

            ASSERT(stderr == freopen(stderrFileName.c_str(), "w", stderr));

            bsl::string  fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::string fn_time = fileName + bsl::string(".%T");
            ASSERT2(0    == mX->enableFileLogging(fn_time.c_str()));
            ASSERT2(true == X->isFileLoggingEnabled());
            ASSERT2(1    == mX->enableFileLogging(fn_time.c_str()));

            for (int i = 0; i < 40; ++i) {
                BALL_LOG_TRACE << "log";
            }

            // Wait some time for async writing to complete.
            waitEmptyRecordQueue(X);

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFileName.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line)); // we caught an error

            mX->disableFileLogging();
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
        }
#else
        if (verbose) {
            cout << "Skipping case 7 on Windows and Cygwin..." << endl;
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING FILE ROTATION
        //
        // Concerns:
        //:  1 'rotateOnSize' triggers a rotation when expected.
        //:  2 'disableSizeRotation' disables rotation on size
        //:  3 'forceRotation' triggers a rotation
        //:  4 'rotateOnTimeInterval' triggers a rotation when expected
        //:  5 'disableTimeIntervalRotation' disables rotation on lifetime
        //
        // Plan:
        //:  1 We will exercise both rotation rules to verify that they work
        //:    properly using glob to count the files and proper timing.  We
        //:    also verify that the size rule is followed by checking the size
        //:    of log files.
        //
        // Testing:
        //   void disableTimeIntervalRotation();
        //   void disableSizeRotation();
        //   void forceRotation();
        //   void rotateOnSize(int size);
        //   void rotateOnTimeInterval(const DatetimeInterval timeInterval);
        //   void rotateOnTimeInterval(const DatetimeI&, const Datetime&);
        //   bdlt::DatetimeInterval rotationLifetime() const;
        //   int rotationSize() const;
        // --------------------------------------------------------------------
        if (verbose) cerr << "\nTESTING FILE ROTATION."
                          << "\n======================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

#ifdef BSLS_PLATFORM_OS_UNIX
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        int         loopCount = 0;
        int         fileCount = 0;
        bsl::string line(&ta);

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            // Create a temporary directory for log files.
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::shared_ptr<Obj>       mX(new(ta) Obj(ball::Severity::e_OFF,
                                                      &ta),
                                          &ta);
            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                bsl::string fn_time = fileName + bsl::string(".%T");
                ASSERT(0    == mX->enableFileLogging(fn_time.c_str()));
                ASSERT(true == X->isFileLoggingEnabled());
                ASSERT(1    == mX->enableFileLogging(fn_time.c_str()));

                BALL_LOG_TRACE << "log 1";

                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                int logRecordCount = countLoggedRecords(globbuf.gl_pathv[0]);
                ASSERTV(globbuf.gl_pathv[0],
                        logRecordCount,
                        1 == logRecordCount);

                ASSERT(true == X->isFileLoggingEnabled());

                globfree(&globbuf);
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) == X->rotationLifetime());

                mX->rotateOnTimeInterval(bdlt::DatetimeInterval(0,0,0,3));

                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                                                        X->rotationLifetime());

                // Wait for [longer than] rotation interval and log new
                // messages.
                bslmt::ThreadUtil::microSleep(0, 4);

                // Those logs will go into a file after rotation.
                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";

                // Wait up to 3 seconds for the rotation to complete.
                bsls::Stopwatch timer;
                timer.start();
                do {
                    bslmt::ThreadUtil::microSleep(1000, 0);
                    glob_t globbuf;
                    ASSERT(
                       0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                    fileCount = static_cast<int>(globbuf.gl_pathc);
                    globfree(&globbuf);
                } while (fileCount < 2 && timer.elapsedTime() < 3);

                // Check that a rotation occurred.
                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERTV(globbuf.gl_pathc, 2 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                // Check the number of lines in the file.
                ASSERT(2 == countLoggedRecords(globbuf.gl_pathv[1]));
                globfree(&globbuf);

                mX->disableTimeIntervalRotation();

                // Wait for [longer than] rotation interval and log new
                // message.
                bslmt::ThreadUtil::microSleep(0, 4);

                BALL_LOG_FATAL << "log 3";

                // Check that no rotation occurred.
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                ASSERT(3 == countLoggedRecords(globbuf.gl_pathv[1]));
                globfree(&globbuf);
            }

            if (verbose) cout << "Testing forced rotation." << endl;
            {
                mX->disableTimeIntervalRotation();
                mX->forceRotation();

                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";
                BALL_LOG_INFO  << "log 3";
                BALL_LOG_WARN  << "log 4";

                // Check that the rotation occurred.
                glob_t globbuf;
                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(3 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                ASSERT(4 == countLoggedRecords(globbuf.gl_pathv[2]));
                globfree(&globbuf);
            }

            if (verbose) cout << "Testing size-constrained rotation." << endl;
            {
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERT(0 == X->rotationSize());
                mX->rotateOnSize(1);
                ASSERT(1 == X->rotationSize());
                for (int i = 0 ; i < 30; ++i) {
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

                for (size_t i = 0; i < globbuf.gl_pathc - 3; ++i) {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[i + 2], bsl::ifstream::in);
                    fs.clear();

                    ASSERT(fs.is_open());

                    bsl::string::size_type fileSize = 0;
                    bsl::string            line(&ta);

                    while (getline(fs, line)) {
                        fileSize += line.length() + 1;
                    }
                    fs.close();

                    ASSERT(fileSize > 1024);
                }

                int oldNumFiles = static_cast<int>(globbuf.gl_pathc);
                globfree(&globbuf);

                ASSERT(1 == X->rotationSize());
                mX->disableSizeRotation();
                ASSERT(0 == X->rotationSize());

                for (int i = 0 ; i < 30; ++i) {
                    BALL_LOG_TRACE << "log";
                    bslmt::ThreadUtil::microSleep(50 * 1000);
                }

                // Verify that no rotation occurred.

                ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(oldNumFiles == (int)globbuf.gl_pathc);
                globfree(&globbuf);
            }

            mX->disableFileLogging();
            mX->stopPublicationThread();

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

            bsl::shared_ptr<Obj> mX(new(ta) Obj(ball::Severity::e_OFF, &ta),
                                    &ta);

            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(0 == manager.registerObserver(mX, "testObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX->enableFileLogging(fileName.c_str()));
                ASSERT(X->isFileLoggingEnabled());
                ASSERT(1 == mX->enableFileLogging(fileName.c_str()));

                BALL_LOG_TRACE << "log 1";

                glob_t globbuf;
                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                ASSERT(1 == countLoggedRecords(globbuf.gl_pathv[0]));
                globfree(&globbuf);
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) == X->rotationLifetime());

                mX->rotateOnTimeInterval(bdlt::DatetimeInterval(0,0,0,3));
                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X->rotationLifetime());

                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1";
                BALL_LOG_DEBUG << "log 2";

                // Wait up to 3 seconds for the rotation to complete.

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    glob_t globbuf;
                    ASSERT(
                       0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                    fileCount = static_cast<int>(globbuf.gl_pathc);
                    globfree(&globbuf);
                } while (fileCount < 2 && loopCount++ < 3);

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                ASSERT(2 == countLoggedRecords(globbuf.gl_pathv[0]));
                globfree(&globbuf);

                mX->disableTimeIntervalRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3";

                // Check that no rotation occurred.

                ASSERT(0 == glob((fileName + "*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait for the async logging to complete.
                waitEmptyRecordQueue(X);

                ASSERT(3 == countLoggedRecords(globbuf.gl_pathv[0]));
                globfree(&globbuf);
            }

            mX->disableFileLogging();
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING LOG RECORDS DROP
        //
        // Concerns:
        //:  1 Asynchronous observer is configured to drop records when the
        //:    fixed queue is full by default.  An alert should be printed to
        //:    the logfile for all dropped records.  This alert should not be
        //:    printed excessively.  (Specifically, it should be printed when
        //:    the queue is half empty or the count has reached a threshold).
        //:
        //:  2 Asynchronous observer can be configured to block the caller of
        //:    'publish'  when the fixed queue is full instead of dropping
        //:    records.  In that case no record should be dropped.
        //:
        //:  3 Note, this test can be run as a negative test case.  Doing so
        //:    will preserve the logfiles.
        //
        // Plan:
        //:  1 To test non-blocking caller thread, we will first create an
        //:    async file observer. Then publish a fair large amount of
        //:    records.  We verify dropped records alerts being raised.
        //:
        //:  2 To test blocking caller thread, we will first create an async
        //:    file observer by passing 'true' in the 'blocking' parameter.
        //:    Then publish a fairly large amount of records.  We verify all
        //:    the records published are actually written to file and nothing
        //:    gets dropped.
        //
        // Testing:
        //   AsyncFileObserver(Severity::Level, bool, int, bslma::Allocator *);
        //   AsyncFileObserver(Severity, bool, int, Severity, Allocator *);
        //   CONCERN: LOG MESSAGE DROP
        // --------------------------------------------------------------------
        if (verbose) cerr << "\nTESTING LOG RECORDS DROP."
                          << "\n=========================" << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        BALL_LOG_SET_CATEGORY("TestCategory");

        bslma::TestAllocator ta;

        int numTestRecords = 40000;

        if (verbose) cerr << "Testing blocking caller thread." << endl;
        {
            // Create a temporary directory for log files.
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            int fixedQueueSize = 1000;

            bsl::shared_ptr<Obj> mX(new (ta) Obj(ball::Severity::e_ERROR,
                                                 false,
                                                 fixedQueueSize,
                                                 ball::Severity::e_TRACE,
                                                 &ta),
                                    &ta);
            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();
            mX->enableFileLogging(fileName.c_str());

            ASSERT(true == X->isPublicationThreadRunning());
            ASSERT(true == X->isFileLoggingEnabled());

            ASSERT(0 == manager.registerObserver(mX, "testObserver"));
            ASSERT(0 == FsUtil::getFileSize(fileName));

            for (int i = 0; i < numTestRecords; ++i) {
                BALL_LOG_TRACE << "This will not be dropped.";
            }

            mX->stopPublicationThread();
            mX->disableFileLogging();

            ASSERT(false == X->isPublicationThreadRunning());
            ASSERT(false == X->isFileLoggingEnabled());

            ASSERT(numTestRecords == countLoggedRecords(fileName));

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("testObserver"));
        }

        if (verbose) cerr << "Testing non-blocking caller thread."
                          << endl;
        {
            // Create a temporary directory for log files.
            bdls::TempDirectoryGuard tempDirGuard("ball_");
            bsl::string              fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            int fixedQueueSize = 1000;

            bsl::shared_ptr<Obj>       mX(new(ta) Obj(ball::Severity::e_ERROR,
                                                      false,
                                                      fixedQueueSize,
                                                      &ta),
                                          &ta);
            bsl::shared_ptr<const Obj> X = mX;

            // Start the publication thread, make sure the publication thread
            // started.

            mX->startPublicationThread();
            mX->enableFileLogging(fileName.c_str());

            ASSERT(true == X->isPublicationThreadRunning());
            ASSERT(true == X->isFileLoggingEnabled());

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            for (int i = 0; i < numTestRecords; ++i) {
                BALL_LOG_TRACE << "This will be dropped.";
            }

            mX->stopPublicationThread();
            mX->disableFileLogging();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));

            ASSERT(numTestRecords >= countLoggedRecords(fileName));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRecords'
        //
        // Concerns:
        //:  1 The 'releaseRecords' method clears all shared pointers in async
        //:    file observer's fixed queue without logging them.
        //
        // Plan:
        //:  1 We will first create an async file observer and a logger manager
        //:    with limited life-time through scoped guard.  Then publish a
        //:    fairly large amount of records after which let the scoped guard
        //:    run out of scope immediately.  The logger manager will be
        //:    released and it should call the 'releaseRecords' method of async
        //:    file observer before destruction.  We verify the
        //:    'releaseRecords' being called and clears the fixed queue
        //:    immediately.
        //
        // Helper Function:
        //   The helper function is run as a child task with stdout redirected
        //   to a file.  This captures stdout in the file so it can be
        //   compared to output to the stringstream passed to the file
        //   observer.  The name of the file is generated here and put into
        //   an environment variable so the child process can read it and
        //   compare it with the expected output.
        //
        // Testing:
        //   void releaseRecords();
        // --------------------------------------------------------------------
        if (verbose) cerr << "\nTESTING 'releaseRecords'"
                          << "\n========================" << endl;

        bslma::TestAllocator ta;

        // Create a temporary directory for log files.
        bdls::TempDirectoryGuard tempDirGuard("ball_");

        // Redirecting stdout to a file.
        bsl::string stdoutFileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&stdoutFileName, "stdoutLog");
        {
            const FILE *out = stdout;
            ASSERT(out == freopen(stdoutFileName.c_str(), "w", stdout));
            fflush(stdout);
        }

        {
            bsl::shared_ptr<Obj>       mX(new(ta) Obj(), &ta);
            bsl::shared_ptr<const Obj> X = mX;
            int                        logCount = 8000;


            ASSERT(ball::Severity::e_WARN == X->stdoutThreshold());

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                &ta);
            {
                // This configuration guarantees that the logger manager will
                // publish all messages regardless of their severity and the
                // observer will see each message only once.

                ball::LoggerManagerConfiguration configuration;
                ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

                ball::LoggerManagerScopedGuard guard(configuration);

                ball::LoggerManager& manager =
                                              ball::LoggerManager::singleton();

                ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

                BALL_LOG_SET_CATEGORY("TestCategory");

                // Throw a fairly large amount of logs into the queue.

                mX->startPublicationThread();
                for (int i = 0; i < logCount; ++i)
                {
                    ball::Context context;
                    mX->publish(record, context);
                }

                // Verify some of the records may have been published
                ASSERTV(record.use_count(), 1 <= record.use_count());

                ASSERTV(record.use_count(),
                        logCount,
                        record.use_count() <= logCount + 1);

                // After this code block the logger manager will be destroyed.
            }

            // The 'releaseRecords' method should clear the queue immediately.

            ASSERTV(record.use_count(), record.use_count() == 1);

            mX->stopPublicationThread();

            // Check that the records cleared by 'releaseRecords' do not get
            // published.

            int actuallyLogged = countLoggedRecords(stdoutFileName);
            ASSERT(actuallyLogged < logCount);
        }
        fclose(stdout);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PUBLICATION THREAD
        //
        // Concerns:
        //: 1 Publication thread can be started/stopped and shutdown at any
        //:   object state.
        //:
        //: 2 Shutdown of the publication thread also drain all log records
        //:   held in the internal queue.
        //
        // Plan:
        //: 1 Start/stop/shutdown the publication thread at various initial
        //:   object states.
        //:
        //: 2 Verify that the after publication thread is shutdown, there are
        //:   no log records held by the object.
        //
        // Testing:
        //   void startPublicationThread();
        //   void shutdownPublicationThread();
        //   void stopPublicationThread();
        //   bool isPublicationThreadRunning() const;
        // --------------------------------------------------------------------
        if (verbose) cerr << "\nTESTING PUBLICATION THREAD"
                          << "\n==========================" << endl;

        if (veryVerbose) cerr << "\tTesting thread start/stop." << endl;
        {
            Obj mX;  const Obj& X = mX;

            ASSERT(false == X.isPublicationThreadRunning());

            // Start/stop publication thread.
            mX.startPublicationThread();
            ASSERT(true  == X.isPublicationThreadRunning());

            mX.stopPublicationThread();
            ASSERT(false == X.isPublicationThreadRunning());

            // Double start the publication thread.
            mX.startPublicationThread();
            ASSERT(true  == X.isPublicationThreadRunning());

            mX.startPublicationThread();
            ASSERT(true  == X.isPublicationThreadRunning());

            // Double stop the publication thread.
            mX.stopPublicationThread();
            ASSERT(false == X.isPublicationThreadRunning());

            mX.stopPublicationThread();
            ASSERT(false == X.isPublicationThreadRunning());
        }

        if (veryVerbose) cerr << "\tTesting thread shutdown." << endl;
        {
            Obj mX; const Obj& X = mX;

            ASSERT(false == X.isPublicationThreadRunning());

            mX.shutdownPublicationThread();
            ASSERT(false == X.isPublicationThreadRunning());
        }
        {
            Obj mX; const Obj& X = mX;

            mX.startPublicationThread();
            ASSERT(true == X.isPublicationThreadRunning());

            mX.shutdownPublicationThread();
            ASSERT(false == X.isPublicationThreadRunning());
            ASSERT(0     == X.recordQueueLength());
        }

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) cerr << "\tTesting thread shutdown" << endl;

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            Obj mX; const Obj& X = mX;

            if (CONFIG == 'b') {
                mX.startPublicationThread();
            }

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                 &ta);
            ball::Context                 context;

            for (int i = 0; i < 1024; ++i) {
                mX.publish(record, context);
            }

            ASSERTV(CONFIG, 0 != X.recordQueueLength());

            mX.shutdownPublicationThread();

            ASSERTV(CONFIG, false == X.isPublicationThreadRunning());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 An object created with the default and value constructor (with or
        //:   without a supplied allocator) has the contractually specified
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the constructor, the default
        //:   allocator in effect at the time of construction becomes the
        //:   object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the constructor, that allocator
        //:   becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Using a loop-based approach, construct distinct objects, in turn,
        //:   but configured differently: (a) without passing an allocator, (b)
        //:   passing a null allocator address explicitly, and (c) passing the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations:  (C-1..8)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use a constructor to dynamically create an object 'X', with its
        //:     object allocator configured appropriately (see P-1); use a
        //:     distinct test allocator for the object's footprint.
        //:
        //:   4 Use the appropriate test allocators to verify that memory is
        //:     allocated by the correct allocator.  (C-2..4,6)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the constructed value.  (C-1,5)
        //:
        //:   6 Verify that all object memory is released when the object is
        //:     destroyed.  (C-7)
        //
        // Testing:
        //   AsyncFileObserver(bslma::Allocator *);
        //   AsyncFileObserver(ball::Severity::Level, bslma::Allocator *);
        //   ~AsyncFileObserver();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CREATORS."
                             "\n=================" << endl;

        for (char cfg = 'a'; cfg <= 'f'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the creator variant

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                if (veryVerbose) {
                    cout << "\t\tTesting default constructor." << endl;
                }
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                if (veryVerbose) {
                    cout << "\t\tTesting constructor with null allocator."
                         << endl;
                }
                objPtr = new (fa) Obj((bslma::Allocator *) 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                if (veryVerbose) {
                    cout << "\t\tTesting constructor with no allocator."
                         << endl;
                }
                objPtr = new (fa) Obj(ball::Severity::e_WARN);
                objAllocatorPtr = &da;
              } break;
              case 'd': {
                if (veryVerbose) {
                    cout << "\t\tTesting constructor with null allocator."
                         << endl;
                }
                objPtr = new (fa) Obj(ball::Severity::e_WARN,
                                      (bslma::Allocator *) 0);
                objAllocatorPtr = &da;
              } break;
              case 'e': {
                if (veryVerbose) {
                    cout << "\t\tTesting constructor with an allocator."
                         << endl;
                }
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              case 'f': {
                if (veryVerbose) {
                    cout << "\t\tTesting constructor with an allocator."
                         << endl;
                }
                objPtr = new (fa) Obj(ball::Severity::e_WARN, &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = CONFIG < 'e' ? sa : da;

            ASSERTV(CONFIG, oa.numBlocksTotal(),  0 != oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Verify the object attributes.
            ASSERT(false == X.isFileLoggingEnabled());
            ASSERT(false == X.isPublicationThreadRunning());
            ASSERT(false == X.isPublishInLocalTimeEnabled());
            ASSERT(true  == X.isStdoutLoggingPrefixEnabled());
            ASSERT(true  == X.isUserFieldsLoggingEnabled());
            ASSERT(0     == X.recordQueueLength());
            ASSERT(ball::Severity::e_WARN == X.stdoutThreshold());


            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //:  1 The publication thread starts and stops properly
        //:
        //:  2 Records are published asynchronously
        //:
        //:  3 The 'publish' method logs in the expected format using
        //:    enable/disableStdoutLogging
        //:
        //:  4 The 'publish' method properly ignores the severity below the one
        //:    specified at construction on 'stdout'
        //:
        //:  5 The 'publish' publishes all messages to a file if file logging
        //:    is enabled
        //:
        //:  6 The name of the log file should be in accordance with what is
        //:    defined by the given pattern if file logging is enabled by a
        //:    pattern
        //:
        //:  7 The 'setLogFormat' method can change to the desired output
        //:    format for both 'stdout' and the log file
        //
        // Plan:
        //:  1 First, we setup up an observer and call 'startPublicationThread'
        //:    and 'stopPublicationThread' a couple of times to verify the
        //:    publication thread starts and stops as expected.
        //:
        //:  2 Then, we directly call 'publish' method to verify the
        //:    publication is indeed asynchronous.  This is done by checking
        //:    records being written even after the 'publish' methods are done
        //:    invoked.
        //:
        //:  3 Last, we will set up the observer and check if logged messages
        //:    are in the expected format and contain the expected data by
        //:    comparing the output of this observer with
        //:    'ball::StreamObserver', that we slightly modify.  Then, we will
        //:    configure the observer to ignore different severity and test if
        //:    only the expected messages are published.  We will use different
        //:    manipulators to affect output format and verify that it has
        //:    changed where expected.
        //
        // Helper Function:
        //   The helper function is run as a child task with stdout redirected
        //   to a file.  This captures stdout in the file so it can be
        //   compared to output to the stringstream passed to the file
        //   observer.  The name of the file is generated here and put into
        //   an environment variable so the child process can read it and
        //   compare it with the expected output.
        //
        // Testing:
        //   void disableFileLogging();
        //   void disableStdoutLoggingPrefix();
        //   int enableFileLogging(const char *logFilenamePattern);
        //   void enablePublishInLocalTime();
        //   void enableStdoutLoggingPrefix();
        //   void publish(const Record& record, const Context& context);
        //   void publish(const shared_ptr<Record>&, const Context&);
        //   void setLogFormat(const char* logF, const char* stdoutF);
        //   void setStdoutThreshold(ball::Severity::Level stdoutThreshold);
        //   void getLogFormat(const char** logF, const char** stdoutF) const;
        //   bool isFileLoggingEnabled() const;
        //   bool isFileLoggingEnabled(bsl::string *result) const;
        //   bool isStdoutLoggingPrefixEnabled() const;
        //   bool isPublishInLocalTimeEnabled() const;
        //   bool isUserFieldsLoggingEnabled() const;
        //   ball::Severity::Level stdoutThreshold() const;
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============" << endl;

        if (verbose) cerr << "Testing temp directory guard" << endl;
        bsl::string tempDirectory;
        {
            bdls::TempDirectoryGuard tempDirGuard("ball_");

            tempDirectory.assign(tempDirGuard.getTempDirName());

            cerr << tempDirectory << endl;
            ASSERT(true == FsUtil::exists(tempDirectory));
            ASSERT(true == FsUtil::isDirectory(tempDirectory));
        }
        // The temporary directory guard should remove the directory in
        // destructor.
        ASSERT(false == FsUtil::exists(tempDirectory));
        ASSERT(false == FsUtil::isDirectory(tempDirectory));

        bslma::TestAllocator ta;

        int         loopCount = 0;
        int         linesNum  = 0;
        bsl::string line(&ta);

        // Create a temporary directory for log files.
        bdls::TempDirectoryGuard tempDirGuard("ball_");

        // Redirecting stdout to a file.
        bsl::string stdoutFileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&stdoutFileName, "stdoutLog");
        {
            const FILE *out = stdout;
            ASSERT(out == freopen(stdoutFileName.c_str(), "w", stdout));
            fflush(stdout);
        }

        ASSERT(true == FsUtil::exists(stdoutFileName));
        ASSERT(0    == FsUtil::getFileSize(stdoutFileName));

#if defined(BSLS_PLATFORM_OS_UNIX) && \
   (!defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR >= 10)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif
        if (verbose) cerr << "Testing asynchronous publication."
                          << endl;
        {
            Obj mX;
            mX.startPublicationThread();

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                &ta);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            Offset beginFileOffset = FsUtil::getFileSize(stdoutFileName);
            if (verbose)
                cout << "Begin file offset: " << beginFileOffset << endl;

            // Throw a fairly large amount of logs into the queue.

            int logCount = 8000;
            for (int i = 0; i < logCount; ++i)
            {
                ball::Context context;
                mX.publish(record, context);
            }

            // Verify there are still records left not published.

            ASSERT(record.use_count() > 1);
            Offset afterFileOffset = FsUtil::getFileSize(stdoutFileName);
            if (verbose)
                cout << "FileOffset after publish: " << afterFileOffset
                     << endl;

            // Verify writing is in process even after all 'publish' calls are
            // finished.

            bslmt::ThreadUtil::microSleep(0, 1);
            Offset endFileOffset = FsUtil::getFileSize(stdoutFileName);
            if (verbose) cout << "End file offset: " << endFileOffset << endl;

            ASSERT(afterFileOffset < endFileOffset);

            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing threshold and output format."
                          << endl;

        // This configuration guarantees that the logger manager will publish
        // all messages regardless of their severity and the observer will see
        // each message only once.

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     ball::Severity::e_TRACE));

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        {
            bsl::ostringstream os, dos;

            bsl::shared_ptr<Obj>       mX(new(ta) Obj(), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            bsl::shared_ptr<ball::StreamObserver> o1(
                                            new(ta) ball::StreamObserver(&dos),
                                            &ta);

            ASSERT(ball::Severity::e_WARN == X->stdoutThreshold());

            mX->startPublicationThread();

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));
            ASSERT(0 == manager.registerObserver(o1, "coutObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            // These two lines are a desperate kludge to make windows work --
            // this test driver works everywhere else without them.

            (void) readPartialFile(stdoutFileName, 0);
            fileOffset = bdls::FilesystemUtil::getFileSize(stdoutFileName);

            BALL_LOG_TRACE << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_DEBUG << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_INFO << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_WARN << "log WARN";
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file.
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                cerr << "[" << dos.str() << "]" << endl;
                cerr << "[" << coutS << "]" << endl;
                ASSERTV(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            dos.str("");

            mX->setStdoutThreshold(ball::Severity::e_ERROR);

            ASSERT(ball::Severity::e_ERROR == X->stdoutThreshold());

            BALL_LOG_WARN << "not logged";

            ASSERT("" == readPartialFile(stdoutFileName, fileOffset));
            dos.str("");

            BALL_LOG_ERROR << "log ERROR";
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file.
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            dos.str("");

            BALL_LOG_FATAL << "log FATAL";
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file.
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            dos.str("");

            bsl::cout.rdbuf(coutSbuf);
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
            ASSERT(0 == manager.deregisterObserver("coutObserver"));
        }

        if (verbose) cerr << "Testing constructor threshold." << endl;
        {
            bsl::ostringstream os, dos;

            bsl::shared_ptr<Obj>       mX(
                                     new(ta) Obj(ball::Severity::e_FATAL, &ta),
                                     &ta);
            bsl::shared_ptr<const Obj> X = mX;

            bsl::shared_ptr<ball::StreamObserver> o1(
                                            new(ta) ball::StreamObserver(&dos),
                                            &ta);


            ASSERT(ball::Severity::e_FATAL == X->stdoutThreshold());
            mX->startPublicationThread();

            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));
            ASSERT(0 == manager.registerObserver(o1, "coutObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_TRACE << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_DEBUG << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_INFO << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_WARN << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_ERROR << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);
            dos.str("");

            BALL_LOG_FATAL << "log";
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file.
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }
            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(dos.str(), coutS, dos.str() == coutS);
            }
            ASSERT(dos.str() == readPartialFile(stdoutFileName, fileOffset));
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            dos.str("");

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
            ASSERT(0 == manager.deregisterObserver("coutObserver"));
        }

        if (verbose) cerr << "Testing short format." << endl;
        {
            bsl::ostringstream os, testOs, dos;

            bsl::shared_ptr<Obj>       mX(new(ta) Obj(), &ta);
            bsl::shared_ptr<const Obj> X = mX;

            bsl::shared_ptr<ball::StreamObserver> o1(
                                            new(ta) ball::StreamObserver(&dos),
                                            &ta);


            mX->startPublicationThread();
            ASSERT(!X->isPublishInLocalTimeEnabled());
            ASSERT( X->isStdoutLoggingPrefixEnabled());
            mX->disableStdoutLoggingPrefix();
            ASSERT(!X->isStdoutLoggingPrefixEnabled());

            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));
            ASSERT(0 == manager.registerObserver(o1, "coutObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BALL_LOG_TRACE << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_DEBUG << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_INFO << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_WARN << "log WARN";
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log WARN " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            testOs.str("");

            BALL_LOG_ERROR << "log ERROR";
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log ERROR " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            testOs.str("");

            ASSERT(!X->isStdoutLoggingPrefixEnabled());
            mX->enableStdoutLoggingPrefix();
            ASSERT( X->isStdoutLoggingPrefixEnabled());

            dos.str("");

            BALL_LOG_FATAL << "log FATAL";
            testOs << "\nFATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log FATAL " << "\n";
            {
                // Replace the spaces after pid, __FILE__ to make dos match the
                // file.
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);

                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                if (veryVeryVerbose) { P_(dos.str()); P(coutS); }
                ASSERTV(dos.str(), coutS, dos.str() == coutS);
                ASSERT(testOs.str() != coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
            ASSERT(0 == manager.deregisterObserver("coutObserver"));
        }

        if (verbose) cerr << "Testing short format with local time "
                          << "offset."
                          << endl;
        {
            bsl::ostringstream os, testOs, dos;

            bsl::shared_ptr<Obj>       mX(
                          new(ta) Obj(ball::Severity::e_WARN, true, 8192, &ta),
                          &ta);
            bsl::shared_ptr<const Obj> X = mX;

            bsl::shared_ptr<ball::StreamObserver> o1(
                                            new(ta) ball::StreamObserver(&dos),
                                            &ta);


            mX->startPublicationThread();
            ASSERT( X->isPublishInLocalTimeEnabled());
            ASSERT( X->isStdoutLoggingPrefixEnabled());
            mX->disableStdoutLoggingPrefix();
            ASSERT(!X->isStdoutLoggingPrefixEnabled());

            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));
            ASSERT(0 == manager.registerObserver(o1, "coutObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BALL_LOG_TRACE << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_DEBUG << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_INFO << "not logged";
            ASSERT(FsUtil::getFileSize(stdoutFileName) == fileOffset);

            BALL_LOG_WARN << "log WARN";
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log WARN " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            testOs.str("");

            BALL_LOG_ERROR << "log ERROR";
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log ERROR " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                ASSERTV(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = FsUtil::getFileSize(stdoutFileName);
            testOs.str("");

            ASSERT(!X->isStdoutLoggingPrefixEnabled());
            mX->enableStdoutLoggingPrefix();
            ASSERT( X->isStdoutLoggingPrefixEnabled());

            dos.str("");

            BALL_LOG_FATAL << "log FATAL";
            testOs << "FATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log FATAL " << "\n";
            // Replace the spaces after pid, __FILE__.
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            {
                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(stdoutFileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                if (0 ==
                    bdlt::LocalTimeOffset::localTimeOffset(
                                    bdlt::CurrentTime::utc()).totalSeconds()) {
                    ASSERTV(dos.str(), os.str(), dos.str() == coutS);
                }
                else {
                    ASSERTV(dos.str(), os.str(), dos.str() != coutS);
                }
                ASSERT(testOs.str() != coutS);
                ASSERTV(coutS, testOs.str(),
                            bsl::string::npos != coutS.find(testOs.str()));

                // Now let's verify the actual difference.
                int defaultObsHour = 0;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(dos.str().substr(11, 2));
                    ASSERT(is >> defaultObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int fileObsHour = 0;
                if (coutS.length() >= 11) {
                    bsl::istringstream is(coutS.substr(11, 2));
                    ASSERT(is >> fileObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int difference = bdlt::CurrentTime::utc().hour() -
                                 bdlt::CurrentTime::local().hour();
                ASSERTV(fileObsHour, defaultObsHour, difference,
                       (fileObsHour + difference + 24) % 24 == defaultObsHour);
                {
                    bsl::string temp = dos.str();
                    temp[11] = coutS[11];
                    temp[12] = coutS[12];
                    dos.str(temp);
                }

                if (defaultObsHour - difference >= 0 &&
                    defaultObsHour - difference < 24) {
                    // UTC and local time are on the same day.
                    if (veryVeryVerbose) { P_(dos.str()); P(coutS); }

                }
                else if (coutS.length() >= 11) {
                    // UTC and local time are on different days.  Ignore date.

                    ASSERT(dos.str().substr(10) == os.str().substr(10));
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }

                fileOffset = FsUtil::getFileSize(stdoutFileName);
                ASSERT(0 == os.str().length());

                bsl::cout.rdbuf(coutSbuf);
                mX->stopPublicationThread();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("asyncObserver"));
                ASSERT(0 == manager.deregisterObserver("coutObserver"));
            }
        }

        if (verbose) cerr << "Testing file logging." << endl;
        {
            bsl::stringstream ss;

            bsl::string  fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "testLog");

            bsl::shared_ptr<Obj> mX(new(ta) Obj(ball::Severity::e_WARN, &ta),
                                    &ta);

            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            Q(Ignore warning about /bogus/path/foo -- it is expected);
            bsl::cout.flush();

            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            ASSERT(-1 == mX->enableFileLogging("/bogus/path/foo"));

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(ss.rdbuf());

            ASSERT(0    == mX->enableFileLogging(fileName.c_str()));
            ASSERT(true == X->isFileLoggingEnabled());
            ASSERT(1    == mX->enableFileLogging(fileName.c_str()));

            BALL_LOG_TRACE << "log 1";
            BALL_LOG_DEBUG << "log 2";
            BALL_LOG_INFO <<  "log 3";
            BALL_LOG_WARN <<  "log 4";
            BALL_LOG_ERROR << "log 5";
            BALL_LOG_FATAL << "log 6";

            // Wait up to 3 seconds for the async logging to complete.

            loopCount = 0;
            do {
                linesNum = 0;
                bslmt::ThreadUtil::microSleep(0, 1);
                bsl::ifstream fs1;
                fs1.open(fileName.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fileName.c_str(), bsl::ifstream::in);
                bsl::ifstream coutFs;
                coutFs.open(stdoutFileName.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                ASSERT(coutFs.is_open());
                coutFs.seekg(fileOffset);
                linesNum = 0;
                while (getline(fs, line)) {
                    if (linesNum >= 6) {
                        // check format
                        bsl::string coutLine;
                        getline(coutFs, coutLine);
                        ASSERTV(coutLine, line, coutLine == line);
                    }
                    ++linesNum;
                }
                fs.close();
                ASSERT(!getline(coutFs, line));
                coutFs.close();
                ASSERT(12 == linesNum);
            }

            ASSERT(true  == X->isFileLoggingEnabled());
            mX->disableFileLogging();
            ASSERT(false == X->isFileLoggingEnabled());
            BALL_LOG_TRACE << "log 1";
            BALL_LOG_DEBUG << "log 2";
            BALL_LOG_INFO <<  "log 3";
            BALL_LOG_WARN <<  "log 4";
            BALL_LOG_ERROR << "log 5";
            BALL_LOG_FATAL << "log 6";

            // Wait up to 3 seconds for the async logging to complete.

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(fileName.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fileName.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                fs.clear();
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();
                ASSERT(12 == linesNum);
            }

            ASSERT(0    == mX->enableFileLogging(fileName.c_str()));
            ASSERT(true == X->isFileLoggingEnabled());
            ASSERT(1    == mX->enableFileLogging(fileName.c_str()));

            BALL_LOG_TRACE << "log 7";
            BALL_LOG_DEBUG << "log 8";
            BALL_LOG_INFO <<  "log 9";
            BALL_LOG_WARN <<  "log 1";
            BALL_LOG_ERROR << "log 2";
            BALL_LOG_FATAL << "log 3";

            // Wait up to 3 seconds for the async logging to complete.

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(fileName.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fileName.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                fs.clear();
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();
                ASSERT(24 == linesNum);
                bsl::cout.rdbuf(coutSbuf);
            }

            mX->disableFileLogging();
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
        }

#ifdef BSLS_PLATFORM_OS_UNIX
        if (verbose) cerr << "Testing file logging with timestamp."
                          << endl;
        {
            bsl::ostringstream os;

            bsl::string  fileName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&fileName, "test2Log");

            bsl::shared_ptr<Obj> mX(new(ta) Obj(ball::Severity::e_WARN, &ta),
                                    &ta);

            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(os.rdbuf());
            bsl::string fn_time = fileName + bsl::string(".%T");

            ASSERT(0    == mX->enableFileLogging(fn_time.c_str()));
            ASSERT(true == X->isFileLoggingEnabled());
            ASSERT(1    == mX->enableFileLogging(fn_time.c_str()));

            BALL_LOG_TRACE << "log 1";
            BALL_LOG_DEBUG << "log 2";
            BALL_LOG_INFO <<  "log 3";
            BALL_LOG_WARN <<  "log 4";
            BALL_LOG_ERROR << "log 5";
            BALL_LOG_FATAL << "log 6";

            glob_t globbuf;
            ASSERT(0 == glob((fileName + ".2*").c_str(), 0, 0, &globbuf));
            ASSERT(1 == globbuf.gl_pathc);

            // Wait up to 3 seconds for the async logging to complete.

            loopCount = 0;
            do {
                linesNum = 0;
                bslmt::ThreadUtil::microSleep(0, 1);
                bsl::ifstream fs1;
                fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                ASSERT(fs.is_open());
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();

                ASSERT(12 == linesNum);
                ASSERT(true ==X->isFileLoggingEnabled());

                bsl::cout.rdbuf(coutSbuf);
            }

            ASSERT("" == os.str());

            mX->disableFileLogging();
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
        }

        if (verbose) cerr << "Testing log file name pattern." << endl;
        {
            bsl::string baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "test3Log");

            bsl::string pattern  = baseName + "%Y%M%D%h%m%s-%p";

            bsl::shared_ptr<Obj> mX(new(ta) Obj(ball::Severity::e_WARN, &ta),
                                    &ta);

            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bdlt::Datetime startDatetime, endDatetime;

            mX->disableTimeIntervalRotation();
            mX->disableSizeRotation();
            mX->disableFileLogging();
            mX->enablePublishInLocalTime();

            // loop until startDatetime is equal to endDatetime.
            do {
                startDatetime = getCurrentTimestamp();

                ASSERT(0    == mX->enableFileLogging(pattern.c_str()));
                ASSERT(true == X->isFileLoggingEnabled());
                ASSERT(1    == mX->enableFileLogging(pattern.c_str()));

                endDatetime = getCurrentTimestamp();

                if (startDatetime.date()   != endDatetime.date()
                 || startDatetime.hour()   != endDatetime.hour()
                 || startDatetime.minute() != endDatetime.minute()
                 || startDatetime.second() != endDatetime.second()) {
                    // not sure the exact time when the log file was opened
                    // because startDatetime and endDatetime are different;
                    // will try it again.
                    bsl::string fn;
                    ASSERT(true == mX->isFileLoggingEnabled(&fn));

                    mX->disableFileLogging();

                    ASSERT(0 == bsl::remove(fn.c_str()));
                }
            } while (!X->isFileLoggingEnabled());

            ASSERT(startDatetime.year()   == endDatetime.year());
            ASSERT(startDatetime.month()  == endDatetime.month());
            ASSERT(startDatetime.day()    == endDatetime.day());
            ASSERT(startDatetime.hour()   == endDatetime.hour());
            ASSERT(startDatetime.minute() == endDatetime.minute());
            ASSERT(startDatetime.second() == endDatetime.second());

            BALL_LOG_INFO<< "log";

            // Construct the name of the log file from startDatetime.

            bsl::ostringstream fnOs;
            fnOs << baseName;
            fnOs << bsl::setw(4) << bsl::setfill('0')
                 << startDatetime.year();
            fnOs << bsl::setw(2) << bsl::setfill('0')
                 << startDatetime.month();
            fnOs << bsl::setw(2) << bsl::setfill('0')
                 << startDatetime.day();
            fnOs << bsl::setw(2) << bsl::setfill('0')
                 << startDatetime.hour();
            fnOs << bsl::setw(2) << bsl::setfill('0')
                 << startDatetime.minute();
            fnOs << bsl::setw(2) << bsl::setfill('0')
                 << startDatetime.second();
            fnOs << "-";
            fnOs << bdls::ProcessUtil::getProcessId();

            // Look for the file with the constructed name.

            glob_t globbuf;
            ASSERTV(fnOs.str(), 0 == glob(fnOs.str().c_str(), 0, 0, &globbuf));
            ASSERTV(globbuf.gl_pathc, 1 == globbuf.gl_pathc);

            // Wait up to 3 seconds for the async logging to complete.

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 2 && loopCount++ < 3);

            mX->disableFileLogging();

            // Read the file to get the number of lines.

            {
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();
                ASSERT(2 == linesNum);
            }

            mX->disableFileLogging();
            mX->stopPublicationThread();

            // Deregister here as we used local allocator for the observer.
            ASSERT(0 == manager.deregisterObserver("asyncObserver"));
        }

        if (verbose) cerr << "Testing '%%' in file name pattern." << endl;
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
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;
                const char *PATTERN  = DATA[ti].d_patternSuffix_p;
                const char *FILENAME = DATA[ti].d_filenameSuffix_p;

                bsl::string baseName(tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&baseName, "test4Log");

                bsl::string pattern(baseName);   pattern  += PATTERN;
                bsl::string expected(baseName);  expected += FILENAME;
                bsl::string actual;

                Obj        mX(ball::Severity::e_WARN, &ta);
                const Obj& X = mX;

                ASSERTV(LINE, 0    == mX.enableFileLogging(pattern.c_str()));
                ASSERTV(LINE, true == X.isFileLoggingEnabled(&actual));

                if (veryVeryVerbose) {
                    P_(PATTERN); P_(expected); P(actual);
                }

                ASSERTV(LINE, expected == actual);

                mX.disableFileLogging();

                // Look for the file with the expected name.

                glob_t globbuf;
                ASSERTV(LINE, 0 == glob(expected.c_str(), 0, 0, &globbuf));
                ASSERTV(LINE, 1 == globbuf.gl_pathc);

                removeFilesByPrefix(expected.c_str());
            }
        }

        if (verbose) cerr << "Testing customized format." << endl;
        {
            Offset fileOffset = FsUtil::getFileSize(stdoutFileName);

            bsl::shared_ptr<Obj> mX(new(ta) Obj(ball::Severity::e_WARN, &ta),
                                    &ta);

            bsl::shared_ptr<const Obj> X = mX;

            mX->startPublicationThread();

            ASSERT(ball::Severity::e_WARN == X->stdoutThreshold());

            ASSERT(0 == manager.registerObserver(mX, "asyncObserver"));

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            // Redirect 'stdout' to a string stream.

            {
                bsl::string baseName(tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&baseName, "test5Log");

                ASSERT(0    == mX->enableFileLogging(baseName.c_str()));
                ASSERT(true == X->isFileLoggingEnabled());
                ASSERT(1    == mX->enableFileLogging(baseName.c_str()));

                bsl::stringstream  os;
                bsl::streambuf    *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                // For log file, use bdlt::Datetime format For stdout, use ISO
                // format.

                mX->setLogFormat("%d %p %t %s %l %c %m %u",
                                 "%i %p %t %s %l %c %m %u");

                fileOffset = FsUtil::getFileSize(stdoutFileName);

                BALL_LOG_WARN << "log";

                // Look for the file with the constructed name.

                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    linesNum = 0;
                    bsl::ifstream fs1;
                    fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    while (getline(fs1, line)) { ++linesNum; }
                    fs1.close();
                } while (!linesNum && loopCount++ < 3);

                // Read the log file to get the record.

                {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    fs.clear();
                    globfree(&globbuf);
                    ASSERT(fs.is_open());
                    ASSERT(getline(fs, line));
                    fs.close();
                }

                bsl::string            datetime1, datetime2;
                bsl::string            log1, log2;
                bsl::string::size_type pos;

                // Divide line into datetime and the rest.

                pos = line.find(' ');
                datetime1 = line.substr(0, pos);
                log1 = line.substr(pos, line.length());

                fflush(stdout);
                bsl::string fStr = readPartialFile(stdoutFileName, fileOffset);

                ASSERT("" == os.str());

                // Divide os.str() into datetime and the rest.

                pos = fStr.find(' ');
                ASSERT(bsl::string::npos != pos);
                datetime2 = fStr.substr(0, pos);

                log2 = fStr.substr(pos, fStr.length()-pos);

                ASSERTV(log1, log2, log1 == log2);

                // Now we try to convert datetime2 from ISO to bdlt::Datetime.

                bsl::istringstream iss(datetime2);

                int  year, month, day, hour, minute, second;
                char c;

                iss >> year >> c >> month >> c >> day >> c
                    >> hour >> c >> minute >> c >> second;

                bdlt::Datetime datetime3(year, month, day,
                                        hour, minute, second);

                bsl::ostringstream oss;
                oss << datetime3;

                // Ignore the millisecond field so don't compare the entire
                // strings.

                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                mX->disableFileLogging();

                ASSERT("" == os.str());
                fileOffset = FsUtil::getFileSize(stdoutFileName);
                bsl::cout.rdbuf(coutSbuf);

                mX->disableFileLogging();

                removeFilesByPrefix(baseName.c_str());
            }

            // Swap the two string formats.

            if (verbose) cerr << "   .. customized format swapped.\n";
            {
                bsl::string baseName(tempDirGuard.getTempDirName());
                bdls::PathUtil::appendRaw(&baseName, "test5Log");

                ASSERT(0    == mX->enableFileLogging(baseName.c_str()));
                ASSERT(true == X->isFileLoggingEnabled());
                ASSERT(1    == mX->enableFileLogging(baseName.c_str()));
                ASSERT(true == X->isFileLoggingEnabled());

                fileOffset = FsUtil::getFileSize(stdoutFileName);

                bsl::stringstream  os;
                bsl::streambuf    *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                mX->setLogFormat("%i %p %t %s %f %l %c %m %u",
                                 "%d %p %t %s %f %l %c %m %u");

                BALL_LOG_WARN << "log";

                // Look for the file with the constructed name.

                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete.

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    linesNum = 0;
                    bsl::ifstream fs1;
                    fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    while (getline(fs1, line)) { ++linesNum; }
                    fs1.close();
                } while (!linesNum && loopCount++ < 3);

                // Read the log file to get the record.

                {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    fs.clear();
                    globfree(&globbuf);
                    ASSERT(fs.is_open());
                    ASSERT(getline(fs, line));
                    fs.close();
                }

                bsl::string            datetime1, datetime2;
                bsl::string            log1, log2;
                bsl::string::size_type pos;

                // Get datetime and the rest from stdout.

                bsl::string soStr =
                                   readPartialFile(stdoutFileName, fileOffset);
                pos = soStr.find(' ');
                datetime1 = soStr.substr(0, pos);
                log1 = soStr.substr(pos, soStr.length());

                // Divide line into datetime and the rest.

                pos = line.find(' ');
                datetime2 = line.substr(0, pos);
                log2 = line.substr(pos, line.length()-pos);

                ASSERTV(log1, log2, log1 == log2);

                // Now we try to convert datetime2 from ISO to bdlt::Datetime.

                bsl::istringstream iss(datetime2);

                int  year, month, day, hour, minute, second;
                char c;

                iss >> year >> c >> month >> c >> day >> c >> hour
                    >> c >> minute >> c >> second;

                bdlt::Datetime datetime3(year, month, day, hour,
                                         minute, second);

                bsl::ostringstream oss;
                oss << datetime3;

                // Ignore the millisecond field so don't compare the entire
                // strings.

                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                if (veryVerbose) {
                    bsl::cerr << "datetime3: " << datetime3 << bsl::endl;
                    bsl::cerr << "datetime2: " << datetime2 << bsl::endl;
                    bsl::cerr << "datetime1: " << datetime1 << bsl::endl;
                }

                fileOffset = FsUtil::getFileSize(stdoutFileName);
                bsl::cout.rdbuf(coutSbuf);
                mX->disableFileLogging();
                mX->stopPublicationThread();

                // Deregister here as we used local allocator for the observer.
                ASSERT(0 == manager.deregisterObserver("asyncObserver"));
                removeFilesByPrefix(baseName.c_str());
            }
        }
#endif

        if (verbose) cerr << "Testing User-Defined Fields Toggling\n";
        {
            const char *logFileFormat;
            const char *stdoutFormat;

            Obj mX(ball::Severity::e_WARN, &ta);  const Obj& X = mX;

            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));

            mX.setLogFormat("\n%d %p:%t %s %f:%l %c %m\n",
                            "\n%d %p:%t %s %f:%l %c %m\n");
            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m\n"));

            mX.setLogFormat("\n%d %p:%t %s %f:%l %c %m %u\n",
                            "\n%d %p:%t %s %f:%l %c %m %u\n");
            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));

            // Now change to short format for stdout.
            ASSERT(true  == X.isStdoutLoggingPrefixEnabled());

            mX.disableStdoutLoggingPrefix();

            ASSERT(false == X.isStdoutLoggingPrefixEnabled());

            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%s %f:%l %c %m %u\n"));

            // Change back to long format for stdout.
            ASSERT(false == X.isStdoutLoggingPrefixEnabled());

            mX.enableStdoutLoggingPrefix();

            ASSERT(true  == X.isStdoutLoggingPrefixEnabled());

            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));

            // Now see what happens with customized format.  Notice that we
            // intentionally use the default short format.

            const char *newLogFileFormat = "\n%s %f:%l %c %m %u\n";
            const char *newStdoutFormat  = "\n%s %f:%l %c %m %u\n";

            mX.setLogFormat(newLogFileFormat, newStdoutFormat);
            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));

            // Now set short format for stdout.
            ASSERT(true  == X.isStdoutLoggingPrefixEnabled());

            mX.disableStdoutLoggingPrefix();

            ASSERT(false == X.isStdoutLoggingPrefixEnabled());

            X.getLogFormat(&logFileFormat, &stdoutFormat);

            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));

            // stdoutFormat should change, since even if we are now using
            // customized formats, the format happens to be the same as the
            // default short format.
        }
        fclose(stdout);
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
