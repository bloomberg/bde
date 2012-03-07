// bael_fileobserver.t.cpp                                            -*-C++-*-
#include <bael_fileobserver.h>

#include <bael_context.h>
#include <bael_defaultobserver.h>             // for testing only
#include <bael_log.h>                         // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#include <bael_severity.h>                    // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdesu_fileutil.h>
#include <bdesu_processutil.h>
#include <bdetu_datetime.h>
#include <bdetu_systemtime.h>

#include <bsls_platform.h>                    // for testing only

#include <bsl_cstdio.h>      // 'remove'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_stdio.h>     // 'tempname'
#include <bsl_c_stdlib.h>    // 'unsetenv'

#include <sys/types.h>
#include <sys/stat.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <glob.h>
#include <bsl_c_signal.h>
#include <sys/resource.h>
#include <bsl_c_time.h>
#include <unistd.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#endif

// Note: on Windows -> WinGDI.h:#define ERROR 0
#if defined(BSLS_PLATFORM__CMP_MSVC) && defined(ERROR)
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
// [ 1] bael_FileObserver(bael_Severity::Level, bslma_Allocator)
// [ 1] ~bael_FileObserver()
//
// MANIPULATORS
// [ 1] publish(const bael_Record& record, const bael_Context& context)
// [ 1] void disableFileLogging()
// [ 2] void disableLifetimeRotation()
// [ 2] void disableSizeRotation()
// [ 1] void disableStdoutLoggingPrefix()
// [ 1] void disableUserFieldsLogging()
// [ 1] int enableFileLogging(const char *fileName, bool timestampFlag = false)
// [ 1] void enableStdoutLoggingPrefix()
// [ 1] void enableUserFieldsLogging()
// [ 1] void publish(const bael_Record& record, const bael_Context& context)
// [ 2] void forceRotation()
// [ 2] void rotateOnSize(int size)
// [ 2] void rotateOnLifetime(bdet_DatetimeInterval timeInterval)
// [ 1] void setStdoutThreshold(bael_Severity::Level stdoutThreshold)
// [ 1] void setLogFormat(const char*, const char*)
// [ 3] void setMaxLogFiles();
// [ 3] int removeExcessLogFiles();
//
// ACCESSORS
// [ 1] bool isFileLoggingEnabled() const
// [ 1] bool isStdoutLoggingPrefixEnabled() const
// [ 1] bool isUserFieldsLoggingEnabled() const
// [ 1] void getLogFormat(const char**, const char**) const
// [ 2] bdet_DatetimeInterval rotationLifetime() const
// [ 2] int rotationSize() const
// [ 1] bael_Severity::Level stdoutThreshold() const
// [ 3] int maxLogFiles() const;
//-----------------------------------------------------------------------------

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// Note assert and debug macros all output to cerr instead of cout, unlike
// most other test drivers.  This is necessary because test case 1 plays
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
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

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

typedef bael_FileObserver Obj;

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

bdet_Datetime getCurrentTimestamp() {
    time_t currentTime = time(0);
    struct tm localtm;
#ifdef BSLS_PLATFORM__OS_WINDOWS
    localtm = *localtime(&currentTime);
#else
    localtime_r(&currentTime, &localtm);
#endif

    bdet_Datetime stamp;
    bdetu_Datetime::convertFromTm(&stamp, localtm);
    return stamp;
}

void removeFilesByPrefix(const char *prefix)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
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
#ifdef BSLS_PLATFORM__OS_WINDOWS
    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "bael", 0, tmpNameBuf);
    result = tmpNameBuf;
#elif defined(BSLS_PLATFORM__OS_HPUX)
    char tmpPathBuf[L_tmpnam];
    result = tempnam(tmpPathBuf, "bael");
#else
    char *fn = tempnam(0, "bael");
    result = fn;
    bsl::free(fn);
#endif

    if (verboseFlag) cout << "\tUse " << result << " as a base filename."
                          << endl;
    // Test Invariant:
    BSLS_ASSERT(!result.empty());
    return result;
}

bsl::string readPartialFile(bsl::string& fileName, int startOffset)
    // read everything after offset 'startOffset' from a file and return it
    // as a string.
{
    bsl::string result;
    result.reserve(bdesu_FileUtil::getFileSize(fileName) + 1 - startOffset);

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

void publishRecord(Obj *mX, const char *message)
{
    bael_RecordAttributes attr(bdetu_SystemTime::nowAsDatetime(),
                               1,
                               2,
                               "FILENAME",
                               3,
                               "CATEGORY",
                               32,
                               message);
    bael_Record record(attr, bdem_List());

    bael_Context context(bael_Transmission::BAEL_PASSTHROUGH, 0, 1);

    mX->publish(record, context);
}

class LogRotationCallbackTester {
    // This class can be used as a functor matching the signature of
    // 'bael_FileObserver2::OnFileRotationCallback'.  This class records every
    // invocation of the function-call operator, and is intended to test
    // whether 'bael_FileObserver2' calls the log-rotation callback
    // appropriately.

    // PRIVATE TYPES
    struct Rep {
        int         d_invocations;
        int         d_status;
        bsl::string d_rotatedFileName;

        explicit Rep(bslma_Allocator *allocator)
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
    bcema_SharedPtr<Rep> d_rep;

  public:
    // PUBLIC CONSTANTS

    enum {
        UNINITIALIZED = INT_MIN
    };

    explicit LogRotationCallbackTester(bslma_Allocator *allocator)
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

    bslma_TestAllocator allocator; bslma_TestAllocator *Z = &allocator;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TIME-BASED ROTATION
        //
        // Concern:
        //: 1 'rotateOnTimeInterval' correctly forward call to
        //:   'bael_FileObserver2'.
        //
        // Plan:
        //: 1 Setup test infrastructure.
        //:
        //: 2 Call 'rotateOnTimeInterval' with a large interval and a reference
        //:   time such that the next rotation will occur soon.  Verify that
        //:   rotation occurs on the scheduled time.
        //:
        //: 3 Call 'disableLifetimeRotation' and verify that no rotation occurs
        //:   afterwards.
        //
        // Testing:
        //  void rotateOnTimeInterval(const DtInterval& i, const Datetime& r);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Time-Based Rotation"
                          << "\n===========================" << endl;

        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        if (veryVerbose) cout << "Testing absolute time reference" << endl;
        {
            // Reset reference start time.

            mX.disableFileLogging();

            // Ensure log file did not exist

            bdesu_FileUtil::remove(BASENAME.c_str());

            bdet_Datetime refTime = bdetu_SystemTime::nowAsDatetimeLocal();
            refTime += bdet_DatetimeInterval(-1, 0, 0, 3);
            mX.rotateOnTimeInterval(bdet_DatetimeInterval(1), refTime);
            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

            bcemt_ThreadUtil::microSleep(0, 3);
            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;


            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 ==
                   bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Testing 'disableLifetimeRotation'" << endl;
        {
            cb.reset();

            mX.disableLifetimeRotation();
            bcemt_ThreadUtil::microSleep(0, 3);
            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());
        }
        mX.disableFileLogging();
        removeFilesByPrefix(BASENAME.c_str());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'setOnFileRotationCallback'
        //
        // Concerns:
        //: 1 'setOnFileRotationCallback' is properly forwarded to the
        //:   corresponding function in 'bael_FileObserver2'
        //
        // Plan:
        //: 1 Setup callback with 'setOnFileRotationCallback' and verify that
        //:   the callback is invoked on rotation.
        //
        // Testing:
        //  void setOnFileRotationCallback(const OnFileRotationCallback&);
        // --------------------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        Obj mX(bael_Severity::BAEL_WARN, &ta);
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
      case 4: {
#ifdef BSLS_PLATFORM__OS_UNIX
        // don't run this if we're in the debugger because the debugger
        // stops and refuses to continue when we hit the file size limit.

        if (verbose) cerr << "Testing output when the stream fails"
                          << " (UNIX only)."
                          << endl;

        bcema_TestAllocator ta;

        bael_LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only
        // see each message only once.

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_TRACE,
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_OFF));
        bael_MultiplexObserver multiplexObserver;
        bael_LoggerManager::initSingleton(&multiplexObserver, configuration);

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

            Obj mX(bael_Severity::BAEL_OFF, true, &ta);
            const Obj& X = mX;
            bsl::stringstream os;

            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            // we want to capture the error message that will be
            // written to stderr (not cerr).  Redirect stderr to a
            // file.  We can't redirect it back; we'll have to use
            // 'ASSERT2' (which outputs to cout, not cerr) from now on
            // and report a summary to to cout at the end of this case.
            bsl::string stderrFN = tempFileName(veryVerbose);
            ASSERT(stderr == freopen(stderrFN.c_str(), "w", stderr));

            ASSERT2(0 == mX.enableFileLogging(fn.c_str(), true));
            ASSERT2(X.isFileLoggingEnabled());
            ASSERT2(1 == mX.enableFileLogging(fn.c_str(), true));

            for (int i = 0 ; i < 40 ;  ++i) {
                BAEL_LOG_TRACE << "log"  << BAEL_LOG_END;
            }

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFN.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line)); // we caught an error

            mX.disableFileLogging();
            removeFilesByPrefix(stderrFN.c_str());
            removeFilesByPrefix(fn.c_str());
            multiplexObserver.deregisterObserver(&mX);

            if (testStatus > 0) {
                cout << "Error, non-zero test status = " << testStatus
                     << "." << endl;
            }
        }
#endif

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

        static bcema_TestAllocator ta(veryVeryVeryVerbose);
        bael_LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only
        // see each message only once.

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_TRACE,
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_OFF));

        bael_MultiplexObserver multiplexObserver;
        bael_LoggerManagerScopedGuard guard(&multiplexObserver,
                                            configuration,
                                            &ta);
        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        if (verbose) cout << "Testing log file deletion." << endl;
        {
            if (verbose) cout << "\t log file opened with timestamp" << endl;

            Obj mX(bael_Severity::BAEL_OFF, &ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM__OS_UNIX
            ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
                bcemt_ThreadUtil::microSleep(1000 * 1000);
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

            Obj mX(bael_Severity::BAEL_OFF, &ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM__OS_UNIX
            ASSERT(0 == mX.enableFileLogging(filename.c_str(), false));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
                bcemt_ThreadUtil::microSleep(1000 * 1000);
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

            Obj mX(bael_Severity::BAEL_OFF, &ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM__OS_UNIX
            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");
            ASSERT(0 == mX.enableFileLogging((filename + "%s").c_str(),
                                             false));
            ASSERT(X.isFileLoggingEnabled());

            for (int i = 0 ; i < 20; ++i) {
                BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
                bcemt_ThreadUtil::microSleep(1000 * 1000);
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
        //   void rotateOnLifetime(bdet_DatetimeInterval timeInterval)
        //   bdet_DatetimeInterval rotationLifetime() const
        //   int rotationSize() const
        // --------------------------------------------------------------------

        bael_LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only
        // see each message only once.

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_TRACE,
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_OFF));

        bael_MultiplexObserver multiplexObserver;
        bael_LoggerManager::initSingleton(&multiplexObserver, configuration);

#ifdef BSLS_PLATFORM__OS_UNIX
        bcema_TestAllocator ta(veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(bael_Severity::BAEL_OFF, &ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
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
                ASSERT(bdet_DatetimeInterval(0) == X.rotationLifetime());
                mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,3));
                ASSERT(bdet_DatetimeInterval(0,0,0,3) ==
                                                         X.rotationLifetime());
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;

                // Check that a rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
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
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_FATAL << "log 3" << BAEL_LOG_END;

                // Check that no rotation occurred.

                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
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
                bcemt_ThreadUtil::microSleep(0, 2);
                mX.forceRotation();
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
                BAEL_LOG_INFO  << "log 3" << BAEL_LOG_END;
                BAEL_LOG_WARN  << "log 4" << BAEL_LOG_END;

                // Check that the rotation occurred.

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
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
                bcemt_ThreadUtil::microSleep(0, 2);
                ASSERT(0 == X.rotationSize());
                mX.rotateOnSize(1);
                ASSERT(1 == X.rotationSize());
                for (int i = 0 ; i < 30; ++i) {
                    BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

                    // We sleep because otherwise, the loop is too fast to make
                    // the timestamp change so we cannot observe the rotation.

                    bcemt_ThreadUtil::microSleep(200 * 1000);
                }

                glob_t globbuf;
                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
                ASSERT(4 <= globbuf.gl_pathc);

                // We are not checking the last one since we do not have any
                // information on its size.

                bsl::ifstream fs;
                for (int i = 0; i < (int)globbuf.gl_pathc - 3; ++i) {
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
                    BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
                    bcemt_ThreadUtil::microSleep(50 * 1000);
                }

                // Verify that no rotation occurred.

                ASSERT(0 == glob((filename + ".2*").c_str(), 0, 0, &globbuf));
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

            Obj mX(bael_Severity::BAEL_OFF, &ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), false));

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

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
                ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
                mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,3));
                ASSERT(bdet_DatetimeInterval(0,0,0,3) == X.rotationLifetime());
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;

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
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_FATAL << "log 3" << BAEL_LOG_END;

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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Publishing Test
        //
        // Concerns:
        //   1. publish() logs in the expected format:
        //      a.using enable/disableUserFieldsLogging
        //      b.using enable/disableStdoutLogging
        //   2. publish() properly ignores the severities below the one
        //      specified at construction on 'stdout'
        //   3. publish() publishes all messages to a file if file logging
        //      is enabled
        //   4. the name of the log file should be in accordance with what is
        //      defined by the given pattern if file logging is enabled by a
        //      pattern
        //   5. setLogFormat can change to the desired output format for both
        //      'stdout' and the log file
        //
        // Plan:
        //   We will set up the observer and check if logged messages are in
        //   the expected format and contain the expected data by comparing the
        //   output of this observer with 'bael_DefaultObserver', that we
        //   slightly modify.  Then, we will configure the observer to ignore
        //   different severities and test if only the expected messages are
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
        //   bael_FileObserver(bael_Severity::Level, bslma_Allocator)
        //   ~bael_FileObserver()
        //   publish(const bael_Record& record, const bael_Context& context)
        //   void disableFileLogging()
        //   void disableStdoutLoggingPrefix()
        //   void disableUserFieldsLogging()
        //   int enableFileLogging(const char *fileName, bool timestampFlag)
        //   void enableStdoutLoggingPrefix()
        //   void enableUserFieldsLogging()
        //   void publish(const bael_Record&, const bael_Context&)
        //   void setStdoutThreshold(bael_Severity::Level stdoutThreshold)
        //   bool isFileLoggingEnabled() const
        //   bool isStdoutLoggingPrefixEnabled() const
        //   bool isUserFieldsLoggingEnabled() const
        //   bael_Severity::Level stdoutThreshold() const
        //   bool isPublishInLocalTimeEnabled() const
        //   void setLogFormat(const char*, const char*)
        //   void getLogFormat(const char**, const char**) const
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing threshold and output format.\n"
                             "====================================\n";

        bcema_TestAllocator ta;

        bsl::string fileName = tempFileName(veryVerbose);
        {
            const FILE *out = stdout;
            ASSERT(out == freopen(fileName.c_str(), "w", stdout));
            fflush(stdout);
        }

        ASSERT(bdesu_FileUtil::exists(fileName));
        ASSERT(0 == bdesu_FileUtil::getFileSize(fileName));

#if defined(BSLS_PLATFORM__OS_UNIX) && \
   (!defined(BSLS_PLATFORM__OS_SOLARIS) || BSLS_PLATFORM__OS_VER_MAJOR >= 10)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif
        bael_LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only
        // see each message only once.

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_TRACE,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF));
        bael_MultiplexObserver multiplexObserver;
        bael_LoggerManager::initSingleton(&multiplexObserver, configuration);

        if (verbose) cerr << "Testing threshold and output format."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(bael_Severity::BAEL_WARN == X.stdoutThreshold());
            bsl::ostringstream os, dos;

            bael_DefaultObserver defaultObserver(&dos);
            bael_MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            // these two lines are a desperate kludge to make windows
            // work -- this test driver works everywhere else without
            // them.
            (void) readPartialFile(fileName, 0);
            fileOffset = bdesu_FileUtil::getFileSize(fileName);

            BAEL_LOG_TRACE << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_DEBUG << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_INFO << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_WARN << "log WARN" << BAEL_LOG_END;
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
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            dos.str("");

            mX.setStdoutThreshold(bael_Severity::BAEL_ERROR);
            ASSERT(bael_Severity::BAEL_ERROR == X.stdoutThreshold());
            BAEL_LOG_WARN << "not logged" << BAEL_LOG_END;
            ASSERT("" == readPartialFile(fileName, fileOffset));
            dos.str("");

            BAEL_LOG_ERROR << "log ERROR" << BAEL_LOG_END;
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
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            dos.str("");

            BAEL_LOG_FATAL << "log FATAL" << BAEL_LOG_END;
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
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            dos.str("");

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
        }

        if (verbose) cerr << "Testing constructor threshold." << endl;
        {
            Obj mX(bael_Severity::BAEL_FATAL, &ta);
            bsl::ostringstream os, dos;
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            bael_DefaultObserver defaultObserver(&dos);
            bael_MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_TRACE << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_DEBUG << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_INFO << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_WARN << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_ERROR << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);
            dos.str("");

            BAEL_LOG_FATAL << "log" << BAEL_LOG_END;
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
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
            }
            ASSERT(dos.str() == readPartialFile(fileName, fileOffset));
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            dos.str("");

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
        }

        if (verbose) cerr << "Testing short format." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(!X.isPublishInLocalTimeEnabled());
            ASSERT( X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());

            bsl::ostringstream os, testOs, dos;
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            bael_DefaultObserver defaultObserver(&dos);
            bael_MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BAEL_LOG_TRACE << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_DEBUG << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_INFO << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_WARN << "log WARN" << BAEL_LOG_END;
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log WARN " << "\n";
            {
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            testOs.str("");

            BAEL_LOG_ERROR << "log ERROR" << BAEL_LOG_END;
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log ERROR " << "\n";
            {
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            testOs.str("");

            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());

            dos.str("");

            BAEL_LOG_FATAL << "log FATAL" << BAEL_LOG_END;
            testOs << "\nFATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log FATAL " << "\n";
            {
                // Replace the spaces after pid, __FILE__ to make dos match the
                // file
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);

                bsl::string coutS = readPartialFile(fileName, fileOffset);
                if (veryVeryVerbose) { P_(dos.str()); P(coutS); }
                LOOP2_ASSERT(dos.str(), coutS, dos.str() == coutS);
                ASSERT(testOs.str() != coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);

            ASSERT("" == os.str());

            bsl::cout.rdbuf(coutSbuf);
            multiplexObserver.deregisterObserver(&localMultiObserver);
        }

        if (verbose) cerr << "Testing short format with local time "
                          << "offset."
                          << endl;
        {
            Obj mX(bael_Severity::BAEL_WARN, true, &ta); const Obj& X = mX;
            ASSERT( X.isPublishInLocalTimeEnabled());
            ASSERT( X.isStdoutLoggingPrefixEnabled());
            mX.disableStdoutLoggingPrefix();
            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            bsl::ostringstream os, testOs, dos;

            bael_DefaultObserver defaultObserver(&dos);
            bael_MultiplexObserver localMultiObserver;
            localMultiObserver.registerObserver(&mX);
            localMultiObserver.registerObserver(&defaultObserver);
            multiplexObserver.registerObserver(&localMultiObserver);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();

            bsl::cout.rdbuf(os.rdbuf());

            BAEL_LOG_TRACE << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_DEBUG << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_INFO << "not logged" << BAEL_LOG_END;
            ASSERT(bdesu_FileUtil::getFileSize(fileName) == fileOffset);

            BAEL_LOG_WARN << "log WARN" << BAEL_LOG_END;
            testOs << "\nWARN " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log WARN " << "\n";
            {
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            testOs.str("");

            BAEL_LOG_ERROR << "log ERROR" << BAEL_LOG_END;
            testOs << "\nERROR " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log ERROR " << "\n";
            {
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                LOOP2_ASSERT(testOs.str(), coutS, testOs.str() == coutS);
            }
            fileOffset = bdesu_FileUtil::getFileSize(fileName);
            testOs.str("");

            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());

            dos.str("");

            BAEL_LOG_FATAL << "log FATAL" << BAEL_LOG_END;
            testOs << "FATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                      " bael_FileObserverTest log FATAL " << "\n";
            // Replace the spaces after pid, __FILE__
            {
                bsl::string temp = dos.str();
                temp[temp.find(__FILE__) + sizeof(__FILE__) - 1] = ':';
                replaceSecondSpace(&temp, ':');
                dos.str(temp);
            }

            {
                bsl::string coutS = readPartialFile(fileName, fileOffset);
                if (
                   0 == bdetu_SystemTime::localTimeOffset().totalSeconds()
                   ) {
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
                int difference = bdetu_SystemTime::nowAsDatetimeGMT().hour() -
                                 bdetu_SystemTime::nowAsDatetimeLocal().hour();
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
                    // skong: This is a bug, you can not figure out if they
                    // are on the same day with only two hour numbers.
                    //ASSERT(dos.str() == coutS);
                }
                else if (coutS.length() >= 11) {
                    // UTC and local time are on different days.  Ignore
                    // date.

                    ASSERT(dos.str().substr(10) == os.str().substr(10));
                } else {
                    ASSERT(0 && "can't substr(11,2), string too short");
                }
                fileOffset = bdesu_FileUtil::getFileSize(fileName);
                ASSERT(0 == os.str().length());

                bsl::cout.rdbuf(coutSbuf);
                multiplexObserver.deregisterObserver(&localMultiObserver);
            }
        }

        if (verbose) cerr << "Testing file logging." << endl;
        {
            bsl::string fn = tempFileName(veryVerbose);
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;
            Q(Ignore warning about /bogus/path/foo -- it is expected);
            ASSERT(-1 == mX.enableFileLogging("/bogus/path/foo"));
            bsl::stringstream ss;

            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(ss.rdbuf());
            ASSERT(0 == mX.enableFileLogging(fn.c_str()));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn.c_str()));

            BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
            BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
            BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

            bsl::ifstream fs, coutFs;
            fs.open(fn.c_str(),           bsl::ifstream::in);
            coutFs.open(fileName.c_str(), bsl::ifstream::in);
            ASSERT(fs.is_open());
            ASSERT(coutFs.is_open());
            coutFs.seekg(fileOffset);
            int linesNum = 0;
            bsl::string line;
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

            ASSERT(X.isFileLoggingEnabled());
            mX.disableFileLogging();
            ASSERT(!X.isFileLoggingEnabled());
            BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
            BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
            BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

            fs.open(fn.c_str(), bsl::ifstream::in);
            ASSERT(fs.is_open());
            fs.clear();
            linesNum = 0;
            while (getline(fs, line)) {
                ++linesNum;
            }
            fs.close();
            ASSERT(12 == linesNum);

            ASSERT(0 == mX.enableFileLogging(fn.c_str()));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn.c_str()));

            BAEL_LOG_TRACE << "log 7" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "log 8" << BAEL_LOG_END;
            BAEL_LOG_INFO <<  "log 9" << BAEL_LOG_END;
            BAEL_LOG_WARN <<  "log 1" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "log 2" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "log 3" << BAEL_LOG_END;

            fs.open(fn.c_str(), bsl::ifstream::in);
            ASSERT(fs.is_open());
            fs.clear();
            linesNum = 0;
            while (getline(fs, line)) {
                ++linesNum;
            }
            fs.close();
            ASSERT(24 == linesNum);
            bsl::cout.rdbuf(coutSbuf);

            mX.disableFileLogging();
            removeFilesByPrefix(fn.c_str());
            multiplexObserver.deregisterObserver(&mX);
        }

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose) cerr << "Testing file logging with timestamp."
                          << endl;
        {
            bsl::string fn = tempFileName(veryVerbose);

            Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;
            bsl::ostringstream os;
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
            bsl::cout.rdbuf(os.rdbuf());
            ASSERT(0 == mX.enableFileLogging(fn.c_str(), true));
            ASSERT(X.isFileLoggingEnabled());
            ASSERT(1 == mX.enableFileLogging(fn.c_str(), true));

            BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
            BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
            BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

            glob_t globbuf;
            ASSERT(0 == glob((fn + ".2*").c_str(), 0, 0, &globbuf));
            ASSERT(1 == globbuf.gl_pathc);
            bsl::ifstream fs;
            fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
            ASSERT(fs.is_open());
            int linesNum = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++linesNum;
            }
            fs.close();
            ASSERT(12 == linesNum);
            ASSERT(X.isFileLoggingEnabled());
            bsl::cout.rdbuf(coutSbuf);

            ASSERT("" == os.str());

            mX.disableFileLogging();
            removeFilesByPrefix(fn.c_str());
            multiplexObserver.deregisterObserver(&mX);
        }

        if (verbose) cerr << "Testing log file name pattern." << endl;
        {
            bsl::string baseName = tempFileName(veryVerbose);
            bsl::string pattern  = baseName + "%Y%M%D%h%m%s";

            Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            bdet_Datetime startDatetime, endDatetime;

            mX.disableLifetimeRotation();
            mX.disableSizeRotation();
            mX.disableFileLogging();

            // loop until startDatetime is equal to endDatetime
            do {
                startDatetime = getCurrentTimestamp();

                ASSERT(0 == mX.enableFileLogging(pattern.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(pattern.c_str(), false));

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

            BAEL_LOG_INFO<< "log" << BAEL_LOG_END;

            mX.disableFileLogging();

            // now construct the name of the log file from startDatetime
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

            // look for the file with the constructed name
            glob_t globbuf;
            ASSERT(0 == glob(fnOs.str().c_str(), 0, 0, &globbuf));
            ASSERT(1 == globbuf.gl_pathc);

            // read the file to get the number of lines
            bsl::ifstream fs;
            fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
            fs.clear();
            globfree(&globbuf);
            ASSERT(fs.is_open());
            int linesNum = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++linesNum;
            }
            fs.close();

            ASSERT(2 == linesNum);

            mX.disableFileLogging();
            removeFilesByPrefix(baseName.c_str());
            multiplexObserver.deregisterObserver(&mX);
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

                Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;

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

        if (verbose) cerr << "Testing customized format." << endl;
        {
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;

            ASSERT(bael_Severity::BAEL_WARN == X.stdoutThreshold());

            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            // redirect 'stdout' to a string stream
            {
                bsl::string baseName = tempFileName(veryVerbose);

                ASSERT(0 == mX.enableFileLogging(baseName.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(baseName.c_str(), false));

                bsl::stringstream os;
                bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                // for log file, use bdet_Datetime format
                // for stdout, use ISO format
                mX.setLogFormat("%d %p %t %s %l %c %m %u",
                                "%i %p %t %s %l %c %m %u");

                BAEL_LOG_WARN << "log" << BAEL_LOG_END;

                // look for the file with the constructed name
                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // read the log file to get the record
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                bsl::string line;
                ASSERT(getline(fs, line));
                fs.close();

                bsl::string datetime1, datetime2;
                bsl::string log1, log2;
                bsl::string::size_type pos;

                // divide line into datetime and the rest
                pos = line.find(' ');
                datetime1 = line.substr(0, pos);
                log1 = line.substr(pos, line.length());

                fflush(stdout);
                bsl::string fStr = readPartialFile(fileName, fileOffset);

                ASSERT("" == os.str());

                // divide os.str() into datetime and the rest
                pos = fStr.find(' ');
                pos = fStr.find(' ',pos+1);
                ASSERT(bsl::string::npos != pos);
                datetime2 = fStr.substr(0, pos);

                log2 = fStr.substr(pos, fStr.length()-pos);

                LOOP2_ASSERT(log1, log2, log1 == log2);

                // now we try to convert datetime2 from ISO to
                // bdet_Datetime
                bsl::istringstream iss(datetime2);
                int year, month, day, hour, minute, second;
                char c;
                iss >> year >> c >> month >> c >> day
                    >> hour >> c >> minute >> c >> second;

                bdet_Datetime datetime3(year, month, day,
                                        hour, minute, second);

                bsl::ostringstream oss;
                oss << datetime3;
                // ignore the millisecond field so don't compare the entire
                // strings
                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                mX.disableFileLogging();

                ASSERT("" == os.str());
                fileOffset = bdesu_FileUtil::getFileSize(fileName);
                bsl::cout.rdbuf(coutSbuf);

                mX.disableFileLogging();
                removeFilesByPrefix(baseName.c_str());
            }
            // now swap the two string formats

            if (verbose) cerr << "   .. customized format swapped.\n";
            {
                bsl::string baseName = tempFileName(veryVerbose);

                ASSERT(0 == mX.enableFileLogging(baseName.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(baseName.c_str(), false));
                ASSERT(X.isFileLoggingEnabled());

                fileOffset = bdesu_FileUtil::getFileSize(fileName);

                bsl::stringstream os;
                bsl::streambuf *coutSbuf = bsl::cout.rdbuf();
                bsl::cout.rdbuf(os.rdbuf());

                mX.setLogFormat("%i %p %t %s %f %l %c %m %u",
                                "%d %p %t %s %f %l %c %m %u");

                BAEL_LOG_WARN << "log" << BAEL_LOG_END;

                // look for the file with the constructed name
                glob_t globbuf;
                ASSERT(0 == glob(baseName.c_str(), 0, 0, &globbuf));
                ASSERT(1 == globbuf.gl_pathc);

                // read the log file to get the record
                bsl::ifstream fs;
                fs.open(globbuf.gl_pathv[0], bsl::ifstream::in);
                fs.clear();
                globfree(&globbuf);
                ASSERT(fs.is_open());
                bsl::string line;
                ASSERT(getline(fs, line));
                fs.close();

                bsl::string datetime1, datetime2;
                bsl::string log1, log2;
                bsl::string::size_type pos;

                // get datetime and the rest from stdout
                bsl::string soStr = readPartialFile(fileName, fileOffset);
                pos = soStr.find(' ');
                datetime1 = soStr.substr(0, pos);
                log1 = soStr.substr(pos, soStr.length());

                // divide line into datetime and the rest
                pos = line.find(' ');
                pos = line.find(' ', pos+1);
                datetime2 = line.substr(0, pos);
                log2 = line.substr(pos, line.length()-pos);

                LOOP2_ASSERT(log1, log2, log1 == log2);

                // now we try to convert datetime2 from ISO to
                // bdet_Datetime
                bsl::istringstream iss(datetime2);
                int year, month, day, hour, minute, second;
                char c;
                iss >> year >> c >> month >> c >> day >> hour
                    >> c >> minute >> c >> second;

                bdet_Datetime datetime3(year, month, day, hour,
                                        minute, second);

                bsl::ostringstream oss;
                oss << datetime3;
                // ignore the millisecond field so don't compare the entire
                // strings
                ASSERT(0 == oss.str().compare(0, 18, datetime1, 0, 18));

                if (veryVerbose) {
                    bsl::cerr << "datetime3: " << datetime3 << bsl::endl;
                    bsl::cerr << "datetime2: " << datetime2 << bsl::endl;
                    bsl::cerr << "datetime1: " << datetime1 << bsl::endl;
                }

                fileOffset = bdesu_FileUtil::getFileSize(fileName);
                bsl::cout.rdbuf(coutSbuf);
                mX.disableFileLogging();
                removeFilesByPrefix(baseName.c_str());
                multiplexObserver.deregisterObserver(&mX);

            }
        }
#endif

        if (verbose) cerr << "Testing User-Defined Fields Toggling\n";
        {
            Obj mX(bael_Severity::BAEL_WARN, &ta);  const Obj& X = mX;
            const char *logFileFormat;
            const char *stdoutFormat;
            int fileOffset = bdesu_FileUtil::getFileSize(fileName);

            ASSERT(X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));

            mX.disableUserFieldsLogging();
            ASSERT(!X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m\n"));

            mX.enableUserFieldsLogging();
            ASSERT(X.isUserFieldsLoggingEnabled());
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

            mX.disableUserFieldsLogging();
            ASSERT(!X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%s %f:%l %c %m\n"));

            mX.enableUserFieldsLogging();
            ASSERT(X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%s %f:%l %c %m %u\n"));

            // Change back to long format for stdout.
            ASSERT(!X.isStdoutLoggingPrefixEnabled());
            mX.enableStdoutLoggingPrefix();
            ASSERT( X.isStdoutLoggingPrefixEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));
            ASSERT(0 == bsl::strcmp(stdoutFormat,
                                    "\n%d %p:%t %s %f:%l %c %m %u\n"));

            // Now see what happens with customized format.  Notice that
            // we intentionally use the default short format.
            const char *newLogFileFormat = "\n%s %f:%l %c %m %u\n";
            const char *newStdoutFormat  = "\n%s %f:%l %c %m %u\n";
            mX.setLogFormat(newLogFileFormat, newStdoutFormat);
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));

            // Toggling user fields logging should not change the formats.
            mX.disableUserFieldsLogging();
            ASSERT(!X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));

            mX.enableUserFieldsLogging();
            ASSERT( X.isUserFieldsLoggingEnabled());
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
            // customized formats, the format happens to be the same as
            // the default short format.
            mX.disableUserFieldsLogging();
            ASSERT(!X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, "\n%s %f:%l %c %m\n"));

            mX.enableUserFieldsLogging();
            ASSERT(X.isUserFieldsLoggingEnabled());
            X.getLogFormat(&logFileFormat, &stdoutFormat);
            ASSERT(0 == bsl::strcmp(logFileFormat, newLogFileFormat));
            ASSERT(0 == bsl::strcmp(stdoutFormat, newStdoutFormat));
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
