// ball_asyncfileobserver.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_asyncfileobserver.h>

#include <ball_context.h>
#include <ball_defaultobserver.h>             // for testing only
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_multiplexobserver.h>           // for testing only
#include <ball_severity.h>                    // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bdls_filesystemutil.h>
#include <bdls_processutil.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_currenttime.h>
#include <bdlt_localtimeoffset.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdio.h>      // 'remove'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>       // 'time_t'
#include <bsl_iomanip.h>     // 'setfill'
#include <bsl_iostream.h>
#include <bsl_sstream.h>


#include <bsl_c_stdio.h>     // 'tempname'
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

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] ball::AsyncFileObserver(ball::Severity::Level, bslma::Allocator)
// [ 1] ~ball::AsyncFileObserver()
//
// MANIPULATORS
// [ 1] publish(const ball::Record& record, const ball::Context& context)
// [ 1] void disableFileLogging()
// [ 3] void disableTimeIntervalRotation()
// [ 3] void disableSizeRotation()
// [ 1] void disableStdoutLoggingPrefix()
// [ 1] int enableFileLogging(const char *logFilenamePattern)
// [ 1] void enableStdoutLoggingPrefix()
// [ 1] void publish(const bcemt::SharedPtr<const ball::Record>& record,
//                   const ball::Context& context)
// [ 2] void releaseRecords()
// [ 2] void shutdownPublicationThread()
// [ 3] void forceRotation()
// [ 3] void rotateOnSize(int size)
// [ 3] void rotateOnTimeInterval(const bdlt::DatetimeInterval timeInterval)
// [ 1] void setStdoutThreshold(ball::Severity::Level stdoutThreshold)
// [ 1] void setLogFormat(const char*, const char*)
// [ 1] void startPublicationThread();
// [ 1] void stopPublicationThread();
//
// ACCESSORS
// [ 9] int recordQueueLength() const
// [ 1] bool isFileLoggingEnabled() const
// [ 1] bool isStdoutLoggingPrefixEnabled() const
// [ 1] void getLogFormat(const char**, const char**) const
// [ 3] bdlt::DatetimeInterval rotationLifetime() const
// [ 3] int rotationSize() const
// [ 1] ball::Severity::Level stdoutThreshold() const
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CONCERN: CONCURRENT PUBLICATION
// [10] USAGE EXAMPLE
//
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// Note assert and debug macros all output to 'cerr' instead of cout, unlike
// most other test drivers.  This is necessary because test case 2 plays
// tricks with cout and examines what is written there.

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cerr << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

static void aSsErT2(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X)  { aSsErT( !(X), #X, __LINE__); }
#define ASSERT2(X) { aSsErT2(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cerr << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cerr << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cerr << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cerr << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef ball::AsyncFileObserver Obj;

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

bdlt::Datetime getCurrentTimestamp()
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

void removeFilesByPrefix(const char *prefix)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    bsl::string filename = prefix;
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
    glob_t globbuf;
    bsl::string filename = prefix;
    filename += "*";
    glob(filename.c_str(), 0, 0, &globbuf);
    for (int i = 0; i < (int)globbuf.gl_pathc; i++)
        unlink(globbuf.gl_pathv[i]);
    globfree(&globbuf);
#endif
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
    if (0 == fn) {
        ASSERTV("Unable to generate temporary file name", false);
        return "ball.faketempfile";                                   // RETURN
    }
    else {
        result = fn;
        bsl::free(fn);
    }
#endif

    if (veryVeryVerbose) {
        cout << "Use " << result << " as a base filename." << endl;
    }
    // Test Invariant:
    BSLS_ASSERT(!result.empty());
    return result;
}

bsl::string readPartialFile(bsl::string& fileName, int startOffset)
    // Read everything after offset the specified 'startOffset' from a file and
    // return it as a string.
{
    bsl::string result;
    result.reserve(
                bdls::FilesystemUtil::getFileSize(fileName) + 1 - startOffset);

    FILE *fp = fopen(fileName.c_str(), "r");
    BSLS_ASSERT_OPT(fp);

    BSLS_ASSERT_OPT(0 == fseek(fp, startOffset, SEEK_SET));

    int c;
    while (EOF != (c = getc(fp))) {
        result += (char) c;
    }

    fclose(fp);

    return result;
}

int countLoggedRecords(const bsl::string& fileName)
{
    bsl::string line;
    int numLines = 0;
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

// Removed with bdlpcre_regex removal.
#if 0
int countMatchingRecords(const bsl::string&  fileName,
                         const char         *pattern,
                         bool                isNegativePattern = false)
    // Return the number of lines in the specified 'fileName' matching
    // the specified regex 'pattern'.  If the optionally specified
    // 'isNegativePattern' flag is 'true', return instead the number of lines
    // *not* matching 'pattern'.
{
    bsl::string line;
    int numLines = 0;
    bsl::ifstream fs;
    fs.open(fileName.c_str(), bsl::ifstream::in);

    ASSERT(fs.is_open());
    bdlpcre::RegEx regex;
    int rc = regex.prepare(0, 0, pattern);
    BSLS_ASSERT_OPT(0 == rc); // test invariant

    while (getline(fs, line)) {
        bool matches = 0 == regex.match(line.c_str(), line.length());
        if (!isNegativePattern == matches) {
            ++numLines;
        }
    }
    fs.close();
    return numLines;
}

int accumulateMatchingRecords(const bsl::string&  fileName,
                              const char         *pattern)
    // Apply the specified regex 'pattern', which must contain one
    // integer-matching subpattern, to each line in the specified 'fileName',
    // and return the sum of all the values of the subpattern for matching
    // lines.
{
    bsl::string line;
    int sum = 0;
    bsl::ifstream fs;
    fs.open(fileName.c_str(), bsl::ifstream::in);

    ASSERT(fs.is_open());
    bdlpcre::RegEx regex;
    int rc = regex.prepare(0, 0, pattern);
    BSLS_ASSERT_OPT(0 == rc); // test invariant
    BSLS_ASSERT_OPT(1 == regex.numSubpatterns()); //test invariant

    while (getline(fs, line)) {
        bsl::vector<bsl::pair<int, int> > result;
        if(0 == regex.match(&result, line.c_str(), line.length())) {
            bsl::istringstream iss(line.substr(result[1].first,
                                               result[1].second));
            int value = 0;
            iss >> value;
            sum += value;
        }
    }
    fs.close();
    return sum;
}
#endif


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

}  // close unnamed namespace


namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY {

void executeInParallel(int                                numThreads,
                       Obj                               *mX,
                       bslmt::ThreadUtil::ThreadFunction  function)
    // Create the specified 'numThreads', each executing the specified 'func'.
{
    bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], function, mX);
    }
    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

extern "C" void *workerThread(void *arg)
{
    BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");
    (void)arg;

    for (int i = 0;i < 10000; ++i) {
        BALL_LOG_TRACE << "ball::AsyncFileObserver Concurrency Test "
                       << BALL_LOG_END;
    }
    return 0;
}

extern "C" void *workerThread2(void *arg)
{
    BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");
    Obj *mX = (Obj*)arg;
    int ret;
    for (int i = 0;i < 100; ++i) {
        ret = mX->startPublicationThread();
        ASSERT(0 == ret);
        for (int j = 0; j < 1000; ++j)
            BALL_LOG_TRACE << "ball::AsyncFileObserver Concurrency Test "
                           << BALL_LOG_END;

        // Test both stopPublicationThread and shutdownPublicationThread

        if (i % 2)
            ret = mX->stopPublicationThread();
        else
            ret = mX->shutdownPublicationThread();
        ASSERT(0 == ret);
        ret = mX->startPublicationThread();
        ASSERT(0 == ret);
    }
    return 0;
}

}  // close namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl << flush;

    bslma::TestAllocator allocator; bslma::TestAllocator *Z = &allocator;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The 'Example 1: Publication Through Logger Manager' provided in
        //   the component header file must compile, link, and run on all
        //   platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace explicit log file name to
        //   temporarily created file name.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nUsage Example 1: Publication Through Logger Manager"
                 << "\n==================================================="
                 << endl;

        bsl::string fileName = tempFileName(veryVerbose);

        ball::AsyncFileObserver asyncFileObserver;
        asyncFileObserver.startPublicationThread();

        ball::LoggerManagerConfiguration configuration;
        ball::LoggerManager::initSingleton(&asyncFileObserver, configuration);

        BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

        asyncFileObserver.setLogFormat("%i %p:%t %s %f:%l %c %m",
                                       "%d %p:%t %s %f:%l %c %m");

        BALL_LOG_INFO << "Will not be published on 'stdout'."
                      << BALL_LOG_END;
        BALL_LOG_WARN << "This warning *will* be published on 'stdout'."
                      << BALL_LOG_END;

        asyncFileObserver.setStdoutThreshold(ball::Severity::e_INFO);
        BALL_LOG_DEBUG << "This debug message is not published on 'stdout'."
                       << BALL_LOG_END;
        BALL_LOG_INFO  << "This info will be published on 'stdout'."
                       << BALL_LOG_END;
        BALL_LOG_WARN  << "This warning will be published on 'stdout'."
                       << BALL_LOG_END;

        asyncFileObserver.enableFileLogging(fileName.c_str());
        asyncFileObserver.setStdoutThreshold(ball::Severity::e_OFF);
        asyncFileObserver.rotateOnSize(1024 * 256);
        asyncFileObserver.rotateOnTimeInterval(bdlt::DatetimeInterval(1));
        asyncFileObserver.disableSizeRotation();
        asyncFileObserver.disableFileLogging();

        asyncFileObserver.stopPublicationThread();
        removeFilesByPrefix(fileName.c_str());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: 'recordQueueLength'
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

        if (verbose)
            cout << endl
                 << "Testing: 'recordQueueLength'" << endl
                 << "============================" << endl;

        const int ERROR = ball::Severity::e_ERROR;

        if (veryVerbose) {
            cout << "\tTesting basic 'recordQueueLength' behavior" << endl;
        }
        {
            bsl::string fileName = tempFileName(veryVerbose);
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            enum { MAX_QUEUE_LENGTH = 1024 };

            // Set up a non-blocking async observer
            Obj mX(ball::Severity::e_FATAL,
                   false,
                   MAX_QUEUE_LENGTH,
                   ball::Severity::e_TRACE,
                   &ta);
            const Obj& X = mX;

            for (int i = 0; i < MAX_QUEUE_LENGTH; ++i) {
                ASSERT(i == X.recordQueueLength());

                bsl::shared_ptr<ball::Record> record;
                record.createInplace(&ta);
                record->fixedFields().setSeverity(ERROR);
                ball::Context context;

                mX.publish(record, context);
            }
            ASSERT(MAX_QUEUE_LENGTH == X.recordQueueLength());

            mX.enableFileLogging(fileName.c_str());

            mX.startPublicationThread();


            bsls::Stopwatch timer;
            timer.start();
            while (MAX_QUEUE_LENGTH - X.recordQueueLength() <= 5) {
                bslmt::ThreadUtil::microSleep(100, 0);
                if (timer.elapsedTime() > 5) {
                    ASSERTV("Failed to write any log records",
                            timer.elapsedTime(),
                            false);
                }

            }

            // Note that stopping the publication thread clears the record
            // queue, so we capture a snapshot of the queue length, and then
            // disable publication so we can determine the number of logged
            // records in the file.

            const int queueLength = X.recordQueueLength();

            mX.disableFileLogging();

// Removed with bdlpcre_regex removal.
#if 0

            const int numLoggedRecords =
                countMatchingRecords(fileName,
                                     "Dropped \\d+ log records", true);
#endif
            mX.shutdownPublicationThread();

// Removed with bdlpcre_regex removal.
#if 0
            if (veryVeryVerbose) {
                P_(numLoggedRecords); P_(queueLength);
                P(MAX_QUEUE_LENGTH - queueLength);
            }

            // Records may have been logged between assigning 'queueLength'
            // and disabling publication.  In addition, its possible that 1
            // record may be discarded (without being logged) as part of
            // shutting down the publication of logged records.  Therefore:
            // 'MAX_QUEUE_LENGTH - X.recordQueueLength() - 1 <=
            //                                             numLoggedRecords

            ASSERTV(MAX_QUEUE_LENGTH, queueLength,
                    MAX_QUEUE_LENGTH > queueLength);
            ASSERTV(numLoggedRecords, MAX_QUEUE_LENGTH, queueLength, fileName,
                    MAX_QUEUE_LENGTH - queueLength - 1 <= numLoggedRecords);
#endif

            // After shutting down the publication thread, the queue should be
            // cleared.
            ASSERT(0 == X.recordQueueLength());

            removeFilesByPrefix(fileName.c_str());
        }

        if (veryVerbose) {
            cout << "\tCall 'recordQueueLength' concurrently with publication"
                 << endl;
        }
        {
            bsl::string fileName = tempFileName(veryVerbose);
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            enum { MAX_QUEUE_LENGTH = 10000 };
            enum { NUM_ITERATIONS = 10 };

            // Set up a non-blocking async observer
            Obj mX(ball::Severity::e_FATAL,
                   false,
                   MAX_QUEUE_LENGTH,
                   ball::Severity::e_TRACE,
                   &ta);
            const Obj& X = mX;

            mX.enableFileLogging(fileName.c_str());
            mX.startPublicationThread();
            for (int iteration = 0; iteration < NUM_ITERATIONS; ++iteration){
                for (int i = 0; i < MAX_QUEUE_LENGTH; ++i) {
                    bsl::shared_ptr<ball::Record> record;
                    record.createInplace(&ta);
                    record->fixedFields().setSeverity(ERROR);
                    ball::Context context;
                    mX.publish(record, context);
                }
                int testIteration = 0;
                int prevQueueLength = X.recordQueueLength();
                ASSERT(0 < prevQueueLength);
                while (prevQueueLength > 0) {
                    int queueLength = X.recordQueueLength();
                    ASSERT(queueLength <= prevQueueLength);
                    prevQueueLength = queueLength;
                    ++testIteration;
                }
                if (veryVeryVerbose) {
                    P(testIteration);
                }
                ASSERT(0 == X.recordQueueLength());
            }
            mX.shutdownPublicationThread();
            ASSERT(0 == X.recordQueueLength());
            removeFilesByPrefix(fileName.c_str());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING: CONCURRENT PUBLICATION
        //
        // Concerns:
        //   Concurrent calls to 'publish' should work correctly that writes
        //   records from different threads into same log file in defined
        //   format.
        //
        // Plan:
        //   Concurrently invoke 'publish' of a fair large amount of records
        //   from threads.  Verify that all the records from all threads are
        //   written into log file and the format is not broken.
        //
        // Testing:
        //   This test invokes the 'publish' method, but doesn't test it.
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl << "Testing Concurrent Publication" << endl
                         << "==============================" << endl;

        using namespace BALL_ASYNCFILEOBSERVER_TEST_CONCURRENCY;

        bsl::string fileName = tempFileName(veryVerbose);

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        // Set up a blocking async observer

        Obj mX(ball::Severity::e_WARN,
               false,
               8192,
               ball::Severity::e_TRACE,
               &ta);
        mX.startPublicationThread();
        bslmt::ThreadUtil::microSleep(0, 1);

        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                    ball::Severity::e_OFF,
                                                    ball::Severity::e_TRACE,
                                                    ball::Severity::e_OFF,
                                                    ball::Severity::e_OFF));
        ball::LoggerManager::initSingleton(&mX, configuration);

        mX.enableFileLogging(fileName.c_str());

        ASSERT(0 == bdls::FilesystemUtil::getFileSize(fileName));

        int numThreads = 4;

        // First test if concurrent publish is correct, check the total number
        // of lines afterwards

        if (verbose)
            cout << "Running first concurrency test." << endl;
        executeInParallel(numThreads, &mX, workerThread);

        mX.stopPublicationThread();
        mX.disableFileLogging();

        int numRecords = countLoggedRecords(fileName);
        ASSERT(numRecords == 10000 * numThreads);

        // Next test if all thread-safe public methods can be called
        // concurrently without crash

        if (verbose)
            cout << "Running second concurrency test." << endl;
        executeInParallel(numThreads, &mX, workerThread2);

        mX.stopPublicationThread();

        mX.disableFileLogging();
        removeFilesByPrefix(fileName.c_str());

      } break;
      case 7: {
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

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(ball::Severity::e_WARN, &ta);  const Obj& X = mX;
        mX.startPublicationThread();
        bslmt::ThreadUtil::microSleep(0, 1);

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        ball::LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

        if (veryVerbose) cout << "Testing absolute time reference" << endl;
        {
            // Reset reference start time.

            mX.disableFileLogging();

            // Ensure log file did not exist

            bdls::FilesystemUtil::remove(BASENAME.c_str());

            bdlt::Datetime refTime = bdlt::CurrentTime::local();
            refTime += bdlt::DatetimeInterval(-1, 0, 0, 3);
            mX.rotateOnTimeInterval(bdlt::DatetimeInterval(1), refTime);
            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            bslmt::ThreadUtil::microSleep(0, 1);
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

            bslmt::ThreadUtil::microSleep(0, 3);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            bslmt::ThreadUtil::microSleep(0, 1);

            // Wait up to 3 seconds for the file rotation to complete

            int loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
            } while (0 == cb.numInvocations() && loopCount++ < 3);

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());

            ASSERT(1 == bdls::FilesystemUtil::exists(
                                                cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Testing 'disableTimeIntervalRotation'"
                              << endl;
        {
            cb.reset();

            mX.disableTimeIntervalRotation();
            bslmt::ThreadUtil::microSleep(0, 3);

            BALL_LOG_TRACE << "log" << BALL_LOG_END;
            bslmt::ThreadUtil::microSleep(0, 1);

            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());
        }
        mX.stopPublicationThread();
        mX.disableFileLogging();
        removeFilesByPrefix(BASENAME.c_str());
      } break;
      case 6: {
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

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        Obj mX(ball::Severity::e_WARN, &ta);
        bsl::string filename = tempFileName(veryVerbose);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        ASSERT(0 == cb.numInvocations());

        mX.enableFileLogging(filename.c_str());
        mX.forceRotation();

        ASSERT(1 == cb.numInvocations());
        mX.disableFileLogging();
        removeFilesByPrefix(filename.c_str());
      } break;
      case 5: {
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        // 'setrlimit' is not implemented on Cygwin.

        // Don't run this if we're in the debugger because the debugger stops
        // and refuses to continue when we hit the file size limit.

        if (verbose) cerr << "Testing output when the stream fails"
                          << " (UNIX only)."
                          << endl;

        bslma::TestAllocator ta;

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
        ball::LoggerManager::initSingleton(&multiplexObserver, configuration);

        {
            bsl::string fn = tempFileName(veryVerbose);

            struct rlimit rlim;
            ASSERT(0 == getrlimit(RLIMIT_FSIZE, &rlim));
            rlim.rlim_cur = 2048;
            ASSERT(0 == setrlimit(RLIMIT_FSIZE, &rlim));

            struct sigaction act,oact;
            act.sa_handler = SIG_IGN;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ASSERT(0 == sigaction(SIGXFSZ, &act, &oact));

            Obj mX(ball::Severity::e_OFF, true, 8192, &ta);
            const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            bsl::stringstream os;

            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            // We want to capture the error message that will be written to
            // stderr (not cerr).  Redirect stderr to a file.  We can't
            // redirect it back; we'll have to use 'ASSERT2' (which outputs to
            // cout, not cerr) from now on and report a summary to to cout at
            // the end of this case.

            bsl::string stderrFN = tempFileName(veryVerbose);
            ASSERT(stderr == freopen(stderrFN.c_str(), "w", stderr));

            bsl::string fn_time = fn + bsl::string(".%T");
            ASSERT2(0 == mX.enableFileLogging(fn_time.c_str()));
            ASSERT2(X.isFileLoggingEnabled());
            ASSERT2(1 == mX.enableFileLogging(fn_time.c_str()));

            for (int i = 0 ; i < 40 ;  ++i) {
                BALL_LOG_TRACE << "log"  << BALL_LOG_END;
            }

            // Wait some time for async writing to complete

            bslmt::ThreadUtil::microSleep(0, 1);

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFN.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line)); // we caught an error

            mX.disableFileLogging();
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();

            removeFilesByPrefix(stderrFN.c_str());
            removeFilesByPrefix(fn.c_str());
        }
#else
        if (verbose) {
            cout << "Skipping case 5 on Windows and Cygwin..." << endl;
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Rotation functions test
        //
        // Concerns:
        //   1. 'rotateOnSize' triggers a rotation when expected.
        //   2. 'disableSizeRotation' disables rotation on size
        //   3. 'forceRotation' triggers a rotation
        //   4. 'rotateOnTimeInterval' triggers a rotation when expected
        //   5. 'disableTimeIntervalRotation' disables rotation on lifetime
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
        //   void disableTimeIntervalRotation()
        //   void disableSizeRotation()
        //   void forceRotation()
        //   void rotateOnSize(int size)
        //   void rotateOnTimeInterval(bdlt::DatetimeInterval timeInterval)
        //   bdlt::DatetimeInterval rotationLifetime() const
        //   int rotationSize() const
        // --------------------------------------------------------------------

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
        ball::LoggerManager::initSingleton(&multiplexObserver, configuration);

#ifdef BSLS_PLATFORM_OS_UNIX
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        int loopCount = 0;
        int fileCount = 0;
        int linesNum  = 0;
        bsl::string line(&ta);

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(ball::Severity::e_OFF, &ta);  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                bsl::string fn_time = filename + bsl::string(".%T");
                ASSERT(0 == mX.enableFileLogging(fn_time.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(fn_time.c_str()));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete

                bsls::Stopwatch timer;
                timer.start();

                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                {
                    ASSERT(1 == countLoggedRecords(globbuf.gl_pathv[0]));
                    globfree(&globbuf);
                    ASSERT(X.isFileLoggingEnabled());
                }
            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) == X.rotationLifetime());
                mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0,0,0,3));
                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                                                         X.rotationLifetime());
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;

                // Wait up to 3 seconds for the rotation to complete

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                    glob_t globbuf;
                    ASSERT(
                       0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                    fileCount = globbuf.gl_pathc;
                    globfree(&globbuf);
                } while (fileCount < 2 && loopCount++ < 3);

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete

                bsls::Stopwatch timer;
                timer.start();

                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                // Check the number of lines in the file.
                {
                    ASSERT(2 == countLoggedRecords(globbuf.gl_pathv[1]));
                    globfree(&globbuf);
                }

                mX.disableTimeIntervalRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

                // Check that no rotation occurred.

                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete
                timer.reset();
                timer.start();

                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                {
                    ASSERT(3 == countLoggedRecords(globbuf.gl_pathv[1]));
                    globfree(&globbuf);
                }
            }

            if (verbose) cout << "Testing forced rotation." << endl;
            {
                bslmt::ThreadUtil::microSleep(0, 2);
                mX.forceRotation();
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
                BALL_LOG_INFO  << "log 3" << BALL_LOG_END;
                BALL_LOG_WARN  << "log 4" << BALL_LOG_END;

                // Check that the rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(3 == globbuf.gl_pathc);

                {
                    ASSERT(4 == countLoggedRecords(globbuf.gl_pathv[2]));
                    globfree(&globbuf);
                }

            }

            if (verbose) cout << "Testing size-constrained rotation." << endl;
            {
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERT(0 == X.rotationSize());
                mX.rotateOnSize(1);
                ASSERT(1 == X.rotationSize());
                for (int i = 0 ; i < 30; ++i) {
                    BALL_LOG_TRACE << "log" << BALL_LOG_END;

                    // We sleep because otherwise, the loop is too fast to make
                    // the timestamp change so we cannot observe the rotation.

                    bslmt::ThreadUtil::microSleep(200 * 1000);
                }

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(4 <= globbuf.gl_pathc);

                // We are not checking the last one since we do not have any
                // information on its size.

                for (int i = 0; i < (int)globbuf.gl_pathc - 3; ++i) {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[i + 2], bsl::ifstream::in);
                    fs.clear();
                    ASSERT(fs.is_open());
                    int fileSize = 0;
                    bsl::string line(&ta);
                    while (getline(fs, line)) {
                        fileSize += line.length() + 1;
                    }
                    fs.close();
                    ASSERT(fileSize > 1024);
                }

                int oldNumFiles = globbuf.gl_pathc;
                globfree(&globbuf);

                ASSERT(1 == X.rotationSize());
                mX.disableSizeRotation();
                ASSERT(0 == X.rotationSize());

                for (int i = 0 ; i < 30; ++i) {
                    BALL_LOG_TRACE << "log" << BALL_LOG_END;
                    bslmt::ThreadUtil::microSleep(50 * 1000);
                }

                // Verify that no rotation occurred.

                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(oldNumFiles == (int)globbuf.gl_pathc);
                globfree(&globbuf);
            }

            mX.disableFileLogging();
            removeFilesByPrefix(filename.c_str());
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }
        {
            // Test with no timestamp.

            if (verbose) cout << "Test-case infrastructure setup." << endl;

            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(ball::Severity::e_OFF, &ta);  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str()));

                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;

                glob_t globbuf;
                ASSERT(0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete
                bsls::Stopwatch timer;
                timer.start();
                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                {
                    ASSERT(1 == countLoggedRecords(globbuf.gl_pathv[0]));
                    globfree(&globbuf);
                }

            }

            if (verbose) cout << "Testing lifetime-constrained rotation."
                              << endl;
            {
                ASSERT(bdlt::DatetimeInterval(0) ==
                       X.rotationLifetime());

                mX.rotateOnTimeInterval(bdlt::DatetimeInterval(0,0,0,3));
                ASSERT(bdlt::DatetimeInterval(0,0,0,3) ==
                       X.rotationLifetime());
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
                BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;

                // Wait up to 3 seconds for the rotation to complete

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    glob_t globbuf;
                    ASSERT(
                       0 == glob((filename + "*").c_str(), 0, 0, &globbuf));
                    fileCount = globbuf.gl_pathc;
                    globfree(&globbuf);
                } while (fileCount < 2 && loopCount++ < 3);

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + "*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete
                bsls::Stopwatch timer;
                timer.start();
                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                {
                    ASSERT(2 == countLoggedRecords(globbuf.gl_pathv[0]));
                    globfree(&globbuf);
                }


                mX.disableTimeIntervalRotation();
                bslmt::ThreadUtil::microSleep(0, 4);
                BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

                // Check that no rotation occurred.

                ASSERT(0 == glob((filename+"*").c_str(), 0, 0, &globbuf));
                ASSERT(2 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete

                timer.reset();
                timer.start();
                do {
                    bslmt::ThreadUtil::microSleep(100, 0);
                } while (X.recordQueueLength() > 0 && timer.elapsedTime() < 3);

                {
                    ASSERT(3 == countLoggedRecords(globbuf.gl_pathv[0]));
                    globfree(&globbuf);
                }
            }

            mX.disableFileLogging();
            removeFilesByPrefix(filename.c_str());
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }
#endif
      } break;
      case -3: // FALL THROUGH
      case 3: {
        // --------------------------------------------------------------------
        // TESTING NON-BLOCKING AND BLOCKING CALLER THREAD
        //
        // Concerns:
        //   - Asynchronous observer is configured to drop records when the
        //     fixed queue is full by default.  An alert should be printed
        //     to the logfile for all dropped records.  This alert should
        //     not be printed excessively.  (Specifically, it should be
        //     printed when the queue is half empty or the count has reached
        //     a threshold).
        //
        //   - Asynchronous observer can be configured to block the caller of
        //     'publish'  when the fixed queue is full instead of dropping
        //     records.  In that case no record should be dropped.
        //
        //   - Note, this test can be run as a negative test case.  Doing so
        //     will preserve the logfiles.
        //
        // Plan:
        //   To test non-blocking caller thread, we will first create an async
        //   file observer. Then publish a fair large amount of records.  We
        //   verify dropped records alerts being raised.
        //
        //   To test blocking caller thread, we will first create an async file
        //   observer by passing 'true' in the 'blocking' parameter.  Then
        //   publish a fairly large amount of records.  We verify all the
        //   records published are actually written to file and nothing gets
        //   dropped.
        //
        // Testing:
        //   This test is for testing non-blocking and blocking caller thread,
        //   not for any particular public method.
        // --------------------------------------------------------------------

        bslma::TestAllocator ta;

        int numTestRecords = 40000;
        ball::MultiplexObserver multiplexObserver;
        ball::LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                    ball::Severity::e_OFF,
                    ball::Severity::e_TRACE,
                    ball::Severity::e_OFF,
                    ball::Severity::e_OFF));
        ball::LoggerManagerScopedGuard guard(&multiplexObserver,
                                             configuration);
        if (verbose) cerr << "Testing blocking caller thread."
                          << endl;
        {
            bsl::string fileName = tempFileName(veryVerbose);

            int fixedQueueSize     = 1000;
            Obj mX(ball::Severity::e_ERROR,
                   false,
                   fixedQueueSize,
                   ball::Severity::e_TRACE,
                   &ta);
            const Obj& X = mX;

            // Start the publication thread, make sure the publication thread
            // started

            mX.startPublicationThread();
            ASSERT(X.isPublicationThreadRunning());
            bslmt::ThreadUtil::microSleep(0, 1);

            multiplexObserver.registerObserver(&mX);
            mX.enableFileLogging(fileName.c_str());
            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            ASSERT(0 == bdls::FilesystemUtil::getFileSize(fileName));

            ball::Context context;
            for (int i = 0;i < numTestRecords; ++i)
                BALL_LOG_TRACE << "This will not be dropped." << BALL_LOG_END;

            mX.stopPublicationThread();
            mX.disableFileLogging();
            multiplexObserver.deregisterObserver(&mX);

            ASSERT(numTestRecords == countLoggedRecords(fileName));

            removeFilesByPrefix(fileName.c_str());
        }

        if (verbose) cerr << "Testing non-blocking caller thread."
                          << endl;
        {
            bsl::string fileName = tempFileName(veryVerbose);

            int fixedQueueSize     = 1000;
            Obj mX(ball::Severity::e_ERROR,
                   false,
                   fixedQueueSize,
                   &ta);
            const Obj& X = mX;

            // Start the publication thread, make sure the publication thread
            // started

            mX.startPublicationThread();
            ASSERT(X.isPublicationThreadRunning());
            bslmt::ThreadUtil::microSleep(0, 1);
            multiplexObserver.registerObserver(&mX);

            mX.enableFileLogging(fileName.c_str());
            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            ball::Context context;
            for (int i = 0;i < numTestRecords; ++i)
                BALL_LOG_TRACE << "This will be dropped." << BALL_LOG_END;

            mX.stopPublicationThread();
            mX.disableFileLogging();
            multiplexObserver.deregisterObserver(&mX);

// Removed with bdlpcre_regex removal.
#if 0
            // We should have got the warning
            int numRecords = countMatchingRecords(fileName, "will be dropped");
            int numDroppedRecords = accumulateMatchingRecords(
                                       fileName,
                                       "Dropped (\\d+) log records");
            int numDroppedRecordMessages = countMatchingRecords(
                                       fileName,
                                       "Dropped \\d+ log records");

            LOOP2_ASSERT(numRecords, numDroppedRecords,
                         numTestRecords == numRecords + numDroppedRecords);

            // The dropped records messages are printed only when the queue
            // is half empty or when there are 5000.  The way we've set up
            // this test, there should be one printed for every 5000 dropped
            // messages plus an additional one, with any remainder, printed
            // while the queue is draining.
            int expectDroppedRecordMessages = (int)bsl::ceil(
                                                  numDroppedRecords / 5000.0);
            LOOP2_ASSERT(
                     numDroppedRecordMessages, expectDroppedRecordMessages,
                     numDroppedRecordMessages == expectDroppedRecordMessages);
#endif

            if (0 < test) {
                removeFilesByPrefix(fileName.c_str());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING RELEASING RECORDS
        //
        // Concerns:
        //   The 'releaseRecords' method clears all shared pointers in async
        //   file observer's fixed queue without logging them.
        //
        // Plan:
        //   We will first create an async file observer and a logger manager
        //   with limited life-time through scoped guard.  Then publish a fair
        //   large amount of records after which let the scoped guard run out
        //   of scope immediately.  The logger manager will be released and it
        //   should call the 'releaseRecords' method of async file observer
        //   before destruction.  We verify the 'releaseRecords' being called
        //   and clears the fixed queue immediately.
        //
        // Tactics:
        //   - Helper Function -1 (see paragraph below)
        //   - Ad-Hoc Data Selection Method
        //   - Brute Force Implementation Technique
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

        bslma::TestAllocator ta;

        // Redirect stdout to temporary file

        bsl::string fileName = tempFileName(veryVerbose);
        {
            const FILE *out = stdout;
            ASSERT(out == freopen(fileName.c_str(), "w", stdout));
            fflush(stdout);
        }
        if (verbose)
            cerr << "Testing 'releaseRecords'.\n"
                 << "=========================\n" << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(ball::Severity::e_WARN == X.stdoutThreshold());

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                &ta);
            int logCount  = 8000;
            {
                ball::LoggerManagerConfiguration configuration;

                // Publish synchronously all messages regardless of their
                // severity.  This configuration also guarantees that the
                // observer will only see each message only once.

                ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                            ball::Severity::e_OFF,
                            ball::Severity::e_TRACE,
                            ball::Severity::e_OFF,
                            ball::Severity::e_OFF));
                ball::LoggerManagerScopedGuard guard(&mX, configuration);

                BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

                // Throw fair large amount of logs into the queue

                for (int i = 0;i < logCount;++i)
                {
                    ball::Context context;
                    mX.publish(record, context);
                }

                ASSERT(record.use_count() > 1);

                // After this code block the logger manager will be destroyed

            }

            // The 'releaseRecord' method should clear the queue immediately

            ASSERT(record.use_count() == 1);

            mX.stopPublicationThread();

            // Check that the records cleared by the 'releaseRecord' do not
            // get published.

            ASSERT(countLoggedRecords(fileName) < logCount);
        }
        fclose(stdout);
        removeFilesByPrefix(fileName.c_str());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Publishing Test
        //
        // Concerns:
        //   1. The publication thread starts and stops properly
        //   2. Records are published asynchronously
        //   3. The 'publish' method logs in the expected format using
        //      enable/disableStdoutLogging
        //   4. The 'publish' method properly ignores the severity below the
        //      one specified at construction on 'stdout'
        //   5. The 'publish' publishes all messages to a file if file logging
        //      is enabled
        //   6. The name of the log file should be in accordance with what is
        //      defined by the given pattern if file logging is enabled by a
        //      pattern
        //   7. The 'setLogFormat' method can change to the desired output
        //      format for both 'stdout' and the log file
        //
        // Plan:
        //   First, we setup up an observer and call 'startPublicationThread'
        //   and 'stopPublicationThread' a couple of times to verify the
        //   publication thread starts and stops as expected.
        //
        //   Then, we directly call 'publish' method to verify the publication
        //   is indeed asynchronous.  This is done by checking records being
        //   written even after the 'publish' methods are done invoked.
        //
        //   Last, we will set up the observer and check if logged messages are
        //   in the expected format and contain the expected data by comparing
        //   the output of this observer with 'ball::DefaultObserver', that we
        //   slightly modify.  Then, we will configure the observer to ignore
        //   different severity and test if only the expected messages are
        //   published.  We will use different manipulators to affect output
        //   format and verify that it has changed where expected.
        //
        // Tactics:
        //   - Helper Function -1 (see paragraph below)
        //   - Ad-Hoc Data Selection Method
        //   - Brute Force Implementation Technique
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
        //   ball::AsyncFileObserver(ball::Severity::Level, bslma::Allocator)
        //   ~ball::AsyncFileObserver()
        //   void startPublicationThread()
        //   void shutdownPublicationThread();
        //   void stopPublicationThread()
        //   bool isPublicationThreadRunning()
        //   publish(const ball::Record& record, const ball::Context& context)
        //   void disableFileLogging()
        //   void disableStdoutLoggingPrefix()
        //   int enableFileLogging(const char *logFilenamePattern)
        //   void enableStdoutLoggingPrefix()
        //   void publish(const ball::Record&, const ball::Context&)
        //   void setStdoutThreshold(ball::Severity::Level stdoutThreshold)
        //   bool isFileLoggingEnabled() const
        //   bool isStdoutLoggingPrefixEnabled() const
        //   ball::Severity::Level stdoutThreshold() const
        //   bool isPublishInLocalTimeEnabled() const
        //   void setLogFormat(const char*, const char*)
        //   void getLogFormat(const char**, const char**) const
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST.\n"
                             "===============\n";

        bslma::TestAllocator ta;

        int loopCount = 0;
        int linesNum  = 0;
        bsl::string line(&ta);

        bsl::string fileName = tempFileName(veryVerbose);
        {
            const FILE *out = stdout;
            ASSERT(out == freopen(fileName.c_str(), "w", stdout));
            fflush(stdout);
        }

        ASSERT(bdls::FilesystemUtil::exists(fileName));
        ASSERT(0 == bdls::FilesystemUtil::getFileSize(fileName));

#if defined(BSLS_PLATFORM_OS_UNIX) && \
   (!defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR >= 10)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif
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
                                             configuration);

        if (verbose) cerr << "Testing publication thread start and stop."
                          << endl;
        {
            Obj mX;

            // Start the publication thread, make sure the publication thread
            // started

            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(mX.isPublicationThreadRunning());

            // Start the publication thread again, make sure nothing bad occurs

            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(mX.isPublicationThreadRunning());

            // Stop the publication thread, make sure the publication thread
            // stopped

            mX.stopPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(!mX.isPublicationThreadRunning());

            // Stop the publication thread again, make sure nothing bad occurs

            mX.stopPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(!mX.isPublicationThreadRunning());
        }

        if (verbose) cerr << "Testing asynchronous publication."
                          << endl;
        {
            Obj mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                &ta);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            int beginFileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            if (verbose)
                cout << "Begin file offset: " << beginFileOffset << endl;

            // Throw fair large amount of logs into the queue

            int logCount  = 8000;
            for (int i = 0;i < logCount;++i)
            {
                ball::Context context;
                mX.publish(record, context);
            }

            // Verify there are still records left not published

            ASSERT(record.use_count() > 1);
            int afterFileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            if (verbose)
                cout << "FileOffset after publish: " << afterFileOffset
                     << endl;

            // Verify writing is in process even after all 'publish' calls are
            // finished

            bslmt::ThreadUtil::microSleep(0, 1);
            int endFileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            if (verbose) cout << "End file offset: " << endFileOffset << endl;

            ASSERT(afterFileOffset < endFileOffset);

            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing threshold and output format."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(ball::Severity::e_WARN == X.stdoutThreshold());
            bsl::ostringstream os, dos;

            ball::DefaultObserver defaultObserver(&dos);
            ball::MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            // these two lines are a desperate kludge to make windows work --
            // this test driver works everywhere else without them.

            (void) readPartialFile(fileName, 0);
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            BALL_LOG_TRACE << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_DEBUG << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_INFO << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_WARN << "log WARN" << BALL_LOG_END;
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }
            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            dos.str("");

            mX.setStdoutThreshold(ball::Severity::e_ERROR);
            ASSERT(ball::Severity::e_ERROR == X.stdoutThreshold());
            BALL_LOG_WARN << "not logged" << BALL_LOG_END;
            ASSERT("" == readPartialFile(fileName, fileOffset));
            dos.str("");

            BALL_LOG_ERROR << "log ERROR" << BALL_LOG_END;
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }
            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            dos.str("");

            BALL_LOG_FATAL << "log FATAL" << BALL_LOG_END;
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }
            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            dos.str("");

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
            localMultiObserver.deregisterObserver(&defaultObserver);
            localMultiObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing constructor threshold." << endl;
        {
            Obj mX(ball::Severity::e_FATAL, &ta);
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            bsl::ostringstream os, dos;
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            ball::DefaultObserver defaultObserver(&dos);
            ball::MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_TRACE << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_DEBUG << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_INFO << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_WARN << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_ERROR << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BALL_LOG_FATAL << "log" << BALL_LOG_END;
            // Replace the spaces after pid, __FILE__ to make dos match the
            // file
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }
            if (veryVeryVerbose) { P_(dos.str()); P(os.str()); }
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            ASSERT(dos.str() == readPartialFile(fileName, fileOffset));
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            dos.str("");

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
            localMultiObserver.deregisterObserver(&defaultObserver);
            localMultiObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing short format." << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT(!X.isPublishInLocalTimeEnabled());
            ASSERT( X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());

            bsl::ostringstream os, testOs, dos;
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            ball::DefaultObserver defaultObserver(&dos);
            ball::MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BALL_LOG_TRACE << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_DEBUG << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_INFO << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_WARN << "log WARN" << BALL_LOG_END;
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log WARN " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            testOs.str("");

            BALL_LOG_ERROR << "log ERROR" << BALL_LOG_END;
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log ERROR " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            testOs.str("");

            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());

            dos.str("");

            BALL_LOG_FATAL << "log FATAL" << BALL_LOG_END;
            testOs << "\nFATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log FATAL " << "\n";
            {
                // Replace the spaces after pid, __FILE__ to make dos match the
                // file
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);

                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                if (veryVeryVerbose) { P_(dos.str()); P(coutS); }
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
                ASSERT(testOs.str() != coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
            localMultiObserver.deregisterObserver(&defaultObserver);
            localMultiObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing short format with local time "
                          << "offset."
                          << endl;
        {
            Obj mX(ball::Severity::e_WARN, true, 8192, &ta);
            const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            ASSERT( X.isPublishInLocalTimeEnabled());
            ASSERT( X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            bsl::ostringstream os, testOs, dos;

            ball::DefaultObserver defaultObserver(&dos);
            ball::MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BALL_LOG_TRACE << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_DEBUG << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_INFO << "not logged" << BALL_LOG_END;
            ASSERT(bdls::FilesystemUtil::getFileSize(fileName) == fileOffset);

            BALL_LOG_WARN << "log WARN" << BALL_LOG_END;
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log WARN " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            testOs.str("");

            BALL_LOG_ERROR << "log ERROR" << BALL_LOG_END;
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log ERROR " << "\n";
            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
            testOs.str("");

            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());

            dos.str("");

            BALL_LOG_FATAL << "log FATAL" << BALL_LOG_END;
            testOs << "FATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " ball::AsyncFileObserverTest log FATAL " << "\n";
            // Replace the spaces after pid, __FILE__
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            {
                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                bsl::string coutS = "";
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    coutS = readPartialFile(fileName, fileOffset);
                } while (coutS == "" && loopCount++ < 3);
                if (0 ==
                    bdlt::LocalTimeOffset::localTimeOffset(
                                    bdlt::CurrentTime::utc()).totalSeconds()) {
                    LOOP2_ASSERT(dos.str(), os.str(), dos.str() == coutS);
                }
                else {
                    LOOP2_ASSERT(dos.str(), os.str(), dos.str() != coutS);
                }
                ASSERT(testOs.str() != coutS);
                LOOP2_ASSERT(coutS, testOs.str(),
                            bsl::string::npos != coutS.find(testOs.str()));

                // Now let's verify the actual difference.
                int defaultObsHour;
                if (dos.str().length() >= 11) {
                    bsl::istringstream is(dos.str().substr(11, 2));
                    ASSERT(is >> defaultObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int fileObsHour;
                if (coutS.length() >= 11) {
                    bsl::istringstream is(coutS.substr(11, 2));
                    ASSERT(is >> fileObsHour);
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                int difference = bdlt::CurrentTime::utc().hour() -
                                 bdlt::CurrentTime::local().hour();
                LOOP3_ASSERT(fileObsHour, defaultObsHour, difference,
                       (fileObsHour + difference + 24) % 24 == defaultObsHour);
                {
                    bsl::string temp = dos.str();
                    temp[11] = coutS[11];
                    temp[12] = coutS[12];
                    dos.str(temp);
                }

                if (defaultObsHour - difference >= 0 &&
                    defaultObsHour - difference < 24) {
                    // UTC and local time are on the same day
                    if (veryVeryVerbose) { P_(dos.str()); P(coutS); }

                }
                else if (coutS.length() >= 11) {
                    // UTC and local time are on different days.  Ignore date.

                    ASSERT(dos.str().substr(10) == os.str().substr(10));
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
                ASSERT(0 == os.str().length());

                bsl::cout.rdbuf(coutSbuf);
                multiplexObserver.deregisterObserver(&localMultiObserver);
                localMultiObserver.deregisterObserver(&defaultObserver);
                localMultiObserver.deregisterObserver(&mX);
                mX.stopPublicationThread();
            }
        }

        if (verbose) cerr << "Testing file logging." << endl;
        {
            bsl::string fn = tempFileName(veryVerbose);
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            Obj mX(ball::Severity::e_WARN, &ta);
            const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            Q(Ignore warning about /bogus/path/foo -- it is expected);
            ASSERT(-1 == mX.enableFileLogging("/bogus/path/foo"));
            bsl::stringstream ss;

            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(ss.rdbuf());
            ASSERT(0 == mX.enableFileLogging(fn.c_str()));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn.c_str()));

            BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
            BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
            BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
            BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
            BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
            BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

            // Wait up to 3 seconds for the async logging to complete

            loopCount = 0;
            do {
                linesNum = 0;
                bslmt::ThreadUtil::microSleep(0, 1);
                bsl::ifstream fs1;
                fs1.open(fn.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fn.c_str(), bsl::ifstream::in);
                bsl::ifstream coutFs;
                coutFs.open(fileName.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                ASSERT(coutFs.is_open());
                coutFs.seekg(fileOffset);
                linesNum = 0;
                while (getline(fs, line)) {
                    if (linesNum >= 6) {
                        // check format
                        bsl::string coutLine;
                        getline(coutFs, coutLine);
                        ASSERT(coutLine == line);
                        //bsl::cerr << coutLine << endl << line << endl;
                    }
                    ++linesNum;
                }
                fs.close();
                ASSERT(!getline(coutFs, line));
                coutFs.close();
                ASSERT(12 == linesNum);
            }

            ASSERT(X.isFileLoggingEnabled());
            mX.disableFileLogging();
            ASSERT(!X.isFileLoggingEnabled());
            BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
            BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
            BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
            BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
            BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
            BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

            // Wait up to 3 seconds for the async logging to complete

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(fn.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fn.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                fs.clear();
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();
                ASSERT(12 == linesNum);
            }

            ASSERT(0 == mX.enableFileLogging(fn.c_str()));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn.c_str()));

            BALL_LOG_TRACE << "log 7" << BALL_LOG_END;
            BALL_LOG_DEBUG << "log 8" << BALL_LOG_END;
            BALL_LOG_INFO <<  "log 9" << BALL_LOG_END;
            BALL_LOG_WARN <<  "log 1" << BALL_LOG_END;
            BALL_LOG_ERROR << "log 2" << BALL_LOG_END;
            BALL_LOG_FATAL << "log 3" << BALL_LOG_END;

            // Wait up to 3 seconds for the async logging to complete

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(fn.c_str(), bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 12 && loopCount++ < 3);

            {
                bsl::ifstream fs;
                fs.open(fn.c_str(), bsl::ifstream::in);
                ASSERT(fs.is_open());
                fs.clear();
                linesNum = 0;
                while (getline(fs, line)) { ++linesNum; }
                fs.close();
                ASSERT(24 == linesNum);
                bsl::cout.rdbuf(coutSbuf);
            }

            mX.disableFileLogging();
            removeFilesByPrefix(fn.c_str());
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }

#ifdef BSLS_PLATFORM_OS_UNIX
        if (verbose) cerr << "Testing file logging with timestamp."
                          << endl;
        {
            bsl::string fn = tempFileName(veryVerbose);

            Obj mX(ball::Severity::e_WARN, &ta);
            const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);
            bsl::ostringstream os;

            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(os.rdbuf());
            bsl::string fn_time = fn + bsl::string(".%T");
            ASSERT(0 == mX.enableFileLogging(fn_time.c_str()));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn_time.c_str()));

            BALL_LOG_TRACE << "log 1" << BALL_LOG_END;
            BALL_LOG_DEBUG << "log 2" << BALL_LOG_END;
            BALL_LOG_INFO <<  "log 3" << BALL_LOG_END;
            BALL_LOG_WARN <<  "log 4" << BALL_LOG_END;
            BALL_LOG_ERROR << "log 5" << BALL_LOG_END;
            BALL_LOG_FATAL << "log 6" << BALL_LOG_END;

            glob_t globbuf;
            ASSERT(0 == glob((fn + ".2*").c_str(), 0, 0, &globbuf));
            ASSERT(1 == globbuf.gl_pathc);

            // Wait up to 3 seconds for the async logging to complete

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
                ASSERT(X.isFileLoggingEnabled());
                bsl::cout.rdbuf(coutSbuf);
            }

            ASSERT("" == os.str());

            mX.disableFileLogging();
            removeFilesByPrefix(fn.c_str());
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
        }

        if (verbose) cerr << "Testing log file name pattern." << endl;
        {
            bsl::string baseName = tempFileName(veryVerbose);
            bsl::string pattern  = baseName + "%Y%M%D%h%m%s-%p";

            Obj mX(ball::Severity::e_WARN, &ta);
            const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);

            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            bdlt::Datetime startDatetime, endDatetime;

            mX.disableTimeIntervalRotation();
            mX.disableSizeRotation();
            mX.disableFileLogging();
            mX.enablePublishInLocalTime();

            // loop until startDatetime is equal to endDatetime
            do {
                startDatetime = getCurrentTimestamp();

                ASSERT(0 == mX.enableFileLogging(pattern.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(pattern.c_str()));

                endDatetime = getCurrentTimestamp();

                if (startDatetime.date()   != endDatetime.date()
                 || startDatetime.hour()   != endDatetime.hour()
                 || startDatetime.minute() != endDatetime.minute()
                 || startDatetime.second() != endDatetime.second()) {
                    // not sure the exact time when the log file was opened
                    // because startDatetime and endDatetime are different;
                    // will try it again
                    bsl::string fn;
                    ASSERT(1 == mX.isFileLoggingEnabled(&fn));
                    mX.disableFileLogging();
                    ASSERT(0 == bsl::remove(fn.c_str()));
                }
            } while (!X.isFileLoggingEnabled());

            ASSERT(startDatetime.year()   == endDatetime.year());
            ASSERT(startDatetime.month()  == endDatetime.month());
            ASSERT(startDatetime.day()    == endDatetime.day());
            ASSERT(startDatetime.hour()   == endDatetime.hour());
            ASSERT(startDatetime.minute() == endDatetime.minute());
            ASSERT(startDatetime.second() == endDatetime.second());

            BALL_LOG_INFO<< "log" << BALL_LOG_END;

            // Construct the name of the log file from startDatetime

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

            // Look for the file with the constructed name

            glob_t globbuf;
            LOOP_ASSERT(fnOs.str(),
                        0 == glob(fnOs.str().c_str(), 0, 0, &globbuf));
            LOOP_ASSERT(globbuf.gl_pathc, 1 == globbuf.gl_pathc);

            // Wait up to 3 seconds for the async logging to complete

            loopCount = 0;
            do {
                bslmt::ThreadUtil::microSleep(0, 1);
                linesNum = 0;
                bsl::ifstream fs1;
                fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                while (getline(fs1, line)) { ++linesNum; }
                fs1.close();
            } while (linesNum < 2 && loopCount++ < 3);

            mX.disableFileLogging();

            // Read the file to get the number of lines

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

            mX.disableFileLogging();
            removeFilesByPrefix(baseName.c_str());
            multiplexObserver.deregisterObserver(&mX);
            mX.stopPublicationThread();
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
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;
                const char *PATTERN  = DATA[ti].d_patternSuffix_p;
                const char *FILENAME = DATA[ti].d_filenameSuffix_p;

                bsl::string baseName = tempFileName(veryVerbose);
                bsl::string pattern(baseName);   pattern  += PATTERN;
                bsl::string expected(baseName);  expected += FILENAME;
                bsl::string actual;

                Obj mX(ball::Severity::e_WARN, &ta);
                const Obj& X = mX;

                LOOP_ASSERT(LINE, 0 == mX.enableFileLogging(pattern.c_str()));
                LOOP_ASSERT(LINE, X.isFileLoggingEnabled(&actual));

                if (veryVeryVerbose) {
                    P_(PATTERN);  P_(expected);  P(actual);
                }

                LOOP_ASSERT(LINE, expected == actual);

                mX.disableFileLogging();

                // Look for the file with the expected name

                glob_t globbuf;
                LOOP_ASSERT(LINE, 0 == glob(expected.c_str(),
                                            0, 0, &globbuf));
                LOOP_ASSERT(LINE, 1 == globbuf.gl_pathc);

                removeFilesByPrefix(expected.c_str());
            }
        }

        if (verbose) cerr << "Testing customized format." << endl;
        {
            int fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

            Obj mX(ball::Severity::e_WARN, &ta);  const Obj& X = mX;
            mX.startPublicationThread();
            bslmt::ThreadUtil::microSleep(0, 1);

            ASSERT(ball::Severity::e_WARN == X.stdoutThreshold());

            multiplexObserver.registerObserver(&mX);

            BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");

            // Redirect 'stdout' to a string stream

            {
                bsl::string baseName = tempFileName(veryVerbose);

                ASSERT(0 == mX.enableFileLogging(baseName.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(baseName.c_str()));

                bsl::stringstream os;
                bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                // For log file, use bdlt::Datetime format For stdout, use ISO
                // format

                mX.setLogFormat("%d %p %t %s %l %c %m %u",
                                "%i %p %t %s %l %c %m %u");

                fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

                BALL_LOG_WARN << "log" << BALL_LOG_END;

                // Look for the file with the constructed name

                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    linesNum = 0;
                    bsl::ifstream fs1;
                    fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    while (getline(fs1, line)) { ++linesNum; }
                    fs1.close();
                } while (!linesNum && loopCount++ < 3);

                // Read the log file to get the record

                {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    fs.clear();
                    globfree(&globbuf);
                    ASSERT(fs.is_open());
                    ASSERT(getline(fs, line));
                    fs.close();
                }

                bsl::string datetime1, datetime2;
                bsl::string log1, log2;
                bsl::string::size_type pos;

                // Divide line into datetime and the rest

                pos = line.find(' ');
                datetime1 = line.substr(0, pos);
                log1 = line.substr(pos, line.length());

                fflush(stdout);
                bsl::string fStr = readPartialFile(fileName, fileOffset);

                ASSERT("" == os.str());

                // Divide os.str() into datetime and the rest

                pos = fStr.find(' ');
                ASSERT(bsl::string::npos != pos);
                datetime2 = fStr.substr(0, pos);

                log2 = fStr.substr(pos, fStr.length()-pos);

                LOOP2_ASSERT(log1, log2, log1 == log2);

                // Now we try to convert datetime2 from ISO to bdlt::Datetime

                bsl::istringstream iss(datetime2);
                int year, month, day, hour, minute, second;
                char c;
                iss >> year >> c >> month >> c >> day >> c
                    >> hour >> c >> minute >> c >> second;

                bdlt::Datetime datetime3(year, month, day,
                                        hour, minute, second);

                bsl::ostringstream oss;
                oss << datetime3;

                // Ignore the millisecond field so don't compare the entire
                // strings

                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                mX.disableFileLogging();

                ASSERT("" == os.str());
                fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
                bsl::cout.rdbuf(coutSbuf);

                mX.disableFileLogging();
                removeFilesByPrefix(baseName.c_str());
            }

            // Swap the two string formats

            if (verbose) cerr << "   .. customized format swapped.\n";
            {
                bsl::string baseName = tempFileName(veryVerbose);

                ASSERT(0 == mX.enableFileLogging(baseName.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(baseName.c_str()));
                ASSERT(X.isFileLoggingEnabled());

                fileOffset = bdls::FilesystemUtil::getFileSize(fileName);

                bsl::stringstream os;
                bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                mX.setLogFormat("%i %p %t %s %f %l %c %m %u",
                                "%d %p %t %s %f %l %c %m %u");

                BALL_LOG_WARN << "log" << BALL_LOG_END;

                // Look for the file with the constructed name

                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // Wait up to 3 seconds for the async logging to complete

                loopCount = 0;
                do {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    linesNum = 0;
                    bsl::ifstream fs1;
                    fs1.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    while (getline(fs1, line)) { ++linesNum; }
                    fs1.close();
                } while (!linesNum && loopCount++ < 3);

                // Read the log file to get the record

                {
                    bsl::ifstream fs;
                    fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                    fs.clear();
                    globfree(&globbuf);
                    ASSERT(fs.is_open());
                    ASSERT(getline(fs, line));
                    fs.close();
                }

                bsl::string datetime1, datetime2;
                bsl::string log1, log2;
                bsl::string::size_type pos;

                // Get datetime and the rest from stdout

                bsl::string soStr = readPartialFile(fileName, fileOffset);
                pos = soStr.find(' ');
                datetime1 = soStr.substr(0, pos);
                log1 = soStr.substr(pos, soStr.length());

                // Divide line into datetime and the rest

                pos = line.find(' ');
                datetime2 = line.substr(0, pos);
                log2 = line.substr(pos, line.length()-pos);

                LOOP2_ASSERT(log1, log2, log1 == log2);

                // Now we try to convert datetime2 from ISO to bdlt::Datetime

                bsl::istringstream iss(datetime2);
                int year, month, day, hour, minute, second;
                char c;
                iss >> year >> c >> month >> c >> day >> c >> hour
                    >> c >> minute >> c >> second;

                bdlt::Datetime datetime3(year, month, day, hour,
                                        minute, second);

                bsl::ostringstream oss;
                oss << datetime3;

                // Ignore the millisecond field so don't compare the entire
                // strings

                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                if (veryVerbose) {
                    bsl::cerr << "datetime3: " << datetime3 << bsl::endl;
                    bsl::cerr << "datetime2: " << datetime2 << bsl::endl;
                    bsl::cerr << "datetime1: " << datetime1 << bsl::endl;
                }

                fileOffset = bdls::FilesystemUtil::getFileSize(fileName);
                bsl::cout.rdbuf(coutSbuf);
                mX.disableFileLogging();
                removeFilesByPrefix(baseName.c_str());
                multiplexObserver.deregisterObserver(&mX);
                mX.stopPublicationThread();
            }
        }
#endif

        if (verbose) cerr << "Testing User-Defined Fields Toggling\n";
        {
            Obj mX(ball::Severity::e_WARN, &ta);  const Obj& X = mX;
            const char *logFileFormat;
            const char *stdoutFormat;

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

            ASSERT( X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%s %f:%l %c %m %u\n"));

            // Change back to long format for stdout.

            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());
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

            ASSERT(X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));

            // stdoutFormat should change, since even if we are now using
            // customized formats, the format happens to be the same as the
            // default short format.
        }

        if (verbose) cerr << "Testing publication shutdown."
                          << endl;
        {
            Obj mX; const Obj& X = mX;

            // Start the publication thread, make sure the publication thread
            // started

            mX.startPublicationThread();
            ASSERT(X.isPublicationThreadRunning());

            bsl::shared_ptr<ball::Record> record(new (ta) ball::Record(&ta),
                                                &ta);
            ball::Context context;
            for (int i = 0;i < 8000; ++i)
                mX.publish(record, context);
            mX.shutdownPublicationThread();
            ASSERT(!mX.isPublicationThreadRunning());
        }

        fclose(stdout);
        removeFilesByPrefix(fileName.c_str());
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
