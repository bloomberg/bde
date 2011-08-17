// bael_fileobserver2.t.cpp                                           -*-C++-*-
#include <bael_fileobserver2.h>

#include <bael_context.h>
#include <bael_defaultobserver.h>             // for testing only
#include <bael_log.h>                         // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#include <bael_recordstringformatter.h>
#include <bael_severity.h>                    // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>

#include <bdesu_fileutil.h>
#include <bdetu_datetime.h>
#include <bdetu_systemtime.h>

#include <bdet_date.h>
#include <bdet_datetime.h>

#include <bsls_platform.h>                    // for testing only

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_stdio.h>  // tempname()

#ifdef BSLS_PLATFORM__OS_UNIX
#include <glob.h>
#include <bsl_c_signal.h>
#include <bsl_c_stdlib.h> //unsetenv
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
// [ 1] bael_FileObserver(bslma_Allocator)
// [ 1] ~bael_FileObserver()
//
// MANIPULATORS
// [ 1] publish(const bael_Record& record, const bael_Context& context)
// [ 1] void disableFileLogging()
// [ 2] void disableLifetimeRotation()
// [ 2] void disableSizeRotation()
// [ 1] void disablePublishInLocalTime()
// [ 1] int enableFileLogging(const char *fileName, bool timestampFlag = false)
// [ 1] void enablePublishInLocalTime()
// [ 1] void publish(const bael_Record& record, const bael_Context& context)
// [ 2] void forceRotation()
// [ 2] void rotateOnSize(int size)
// [ 2] void rotateOnLifetime(bdet_DatetimeInterval timeInterval)
// [ 1] void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
// [ 3] void setMaxLogFiles();
// [ 3] int removeExcessLogFiles();
//
// ACCESSORS
// [ 1] bool isFileLoggingEnabled() const
// [ 1] bool isPublishInLocalTimeEnabled() const
// [ 2] bdet_DatetimeInterval rotationLifetime() const
// [ 2] int rotationSize() const
// [ 3] int maxLogFiles() const;
//-----------------------------------------------------------------------------

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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

typedef bael_FileObserver2 Obj;

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

bdet_Datetime getCurrentLocalTime() {
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

void logRecord1(bsl::ostream& stream, const bael_Record& record)
{
    bael_Severity::Level severityLevel =
        (bael_Severity::Level) record.fixedFields().severity();

    stream << '\n';

    stream << record.fixedFields().timestamp()      << " ";
    stream << record.fixedFields().processID()      << ":";
    stream << record.fixedFields().threadID()       << " ";
    stream << bael_Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().fileName()       << ":";
    stream << record.fixedFields().lineNumber()     << " ";
    stream << record.fixedFields().category()       << " ";
    stream << record.fixedFields().message ()       << " ";

    const bdem_List& userFields = record.userFields();
    for (int i = 0; i < userFields.length(); ++i)
    {
        stream << userFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
}

void logRecord2(bsl::ostream& stream, const bael_Record& record)
{
    bael_Severity::Level severityLevel =
        (bael_Severity::Level) record.fixedFields().severity();

    stream << '\n';

    stream << record.fixedFields().message ()       << " ";
    stream << record.fixedFields().category()       << " ";
    stream << record.fixedFields().fileName()       << ":";
    stream << bael_Severity::toAscii(severityLevel) << " ";
    stream << record.fixedFields().threadID()       << " ";
    stream << record.fixedFields().processID()      << ":";

    const bdem_List& userFields = record.userFields();
    for (int i = 0; i < userFields.length(); ++i)
    {
        stream << userFields[i] << "*";
    }

    stream << '\n' << bsl::flush;
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


void removeFilesByPrefix(const char *prefix)
{
#ifdef BSLS_PLATFORM__OS_UNIX
    glob_t globbuf;
    bsl::string filename = prefix;
    filename += "*";
    glob(filename.c_str(), 0, 0, &globbuf);
    for (int i = 0; i < (int)globbuf.gl_pathc; i++)
        unlink(globbuf.gl_pathv[i]);
    globfree(&globbuf);
#endif
}

int readFileIntoString(int lineNum, const bsl::string& filename,
                       bsl::string& fileContent)
{
#ifdef BSLS_PLATFORM__OS_UNIX
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
    // 'bael_FileObserver2::OnFileRotationCallback'.  This class records every
    // invocation of the function-call operator, and is intended to test
    // whether 'bael_FileObserver2' calls the log-rotation callback
    // appropriately.

   // PRIVATE TYPES
   struct Rep {
        int         d_invocations;
        int         d_status;
        bsl::string d_previousFileName;
        bsl::string d_fileName;

        Rep(bslma_Allocator *allocator)
        : d_invocations(0)
        , d_status(0)
        , d_previousFileName(allocator)
        , d_fileName(allocator)
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
                    const bsl::string& previousFileName,
                    const bsl::string& newFileName)
        // Set the value at the status address supplied at construction to the
        // specified 'status', and set the value at the log file name address
        // supplied at construction to the specified 'logFileName'.
    {
        ++d_rep->d_invocations;
        d_rep->d_status           = status;
        d_rep->d_previousFileName = previousFileName;
        d_rep->d_fileName         = newFileName;

    }

    void reset()
        // Set '*status' to 'UNINITIALIZED' and set '*logFileName' to the
        // empty string.
    {
        d_rep->d_invocations = 0;
        d_rep->d_status           = UNINITIALIZED;
        d_rep->d_previousFileName = "";
        d_rep->d_fileName         = "";

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

    const bsl::string& fileName() const { return d_rep->d_fileName; }
        // Return a reference to the non-modifiable file name supplied to the
        // most recent invocation of the function-call operator, or the empty
        // string if 'numInvocations' is 0.

    const bsl::string& previousFileName() const
        // Return a reference to the non-modifiable file name supplied to the
        // most recent invocation of the function-call operator, or the empty
        // string if 'numInvocations' is 0.
    {
        return d_rep->d_previousFileName;
    }

};

typedef LogRotationCallbackTester RotCbTest;

class ReentrantRotationCallback {
    // This class can be used as a functor matching the signature of
    // 'bael_FileObserver2::OnFileRotationCallback'.  This class implements
    // the function-call operator, that will call 'forceRotation' on the
    // file observer supplied at construction.  Note that this type is
    // intended to test whether the rotation callback is called in a way that
    // is re-entrant safe.

    Obj *d_observer_p;

  public:

    ReentrantRotationCallback(Obj *observer)
    : d_observer_p(observer)
    {
    }

    void operator()(int                status,
                    const bsl::string& previousFileName,
                    const bsl::string& newFileName);
        // Set the value at the status address supplied at construction to the
        // specified 'status', and set the value at the log file name address
        // supplied at construction to the specified 'logFileName'.
};

void ReentrantRotationCallback::operator()(int                status,
                                           const bsl::string& previousFileName,
                                           const bsl::string& newFileName)
{
    d_observer_p->disableFileLogging();
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


int getNumLines(const char *filename) {
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

int getFileSize(const char *filename) {
    bsl::ifstream fs;
    fs.open(filename, bsl::ifstream::in);
    fs.clear();
    ASSERT(fs.is_open());

    int fileSize = 0;

    bsl::string line;
    while (getline(fs, line)) {
        fileSize += line.length() + 1;
    }
    fs.close();
    return fileSize;
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

    bslma_TestAllocator allocator; bslma_TestAllocator *Z = &allocator;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
#if 0
        // Test cases for rolling file chain, which is not implemented yet.

      case 6: {
        // --------------------------------------------------------------------
        // ROTATE WHEN OPENING EXISTING FILE
        //
        // Concerns:
        //  1. 'rotateOnSize' triggers a rotation as expected even if the log
        //     file already exist.
        //
        // Plan:
        //  1. Set 'rotateOnSize' to 1k, create a file with approximately 0.5k.
        //  2. Write another 0.5k to the file and verify that that file is
        //     rotated.
        //
        // Testing:
        //  Concern: 'rotateOnSize' triggers correctly for existing files
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
        bael_LoggerManager::initSingleton(&multiplexObserver,
                                          configuration);

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging((filename + "%%").c_str(),
                                                 false));
                mX.rotateOnSize(1);
                ASSERT(X.isFileLoggingEnabled());

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

                ASSERT(1 == bdesu_FileUtil::exists(filename.c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));

                ASSERT(2 == getNumLines(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing rotation." << endl;
            {
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));
                char buffer[512];
                memset(buffer, 'x', sizeof buffer);
                buffer[sizeof buffer - 1] = '\0';

                BAEL_LOG_TRACE << buffer << BAEL_LOG_END;
                mX.disableFileLogging();

                ASSERT(0 == mX.enableFileLogging((filename + "%%").c_str(),
                                                 false));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));

                BAEL_LOG_TRACE << 'x' << BAEL_LOG_END;
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));

                BAEL_LOG_TRACE << buffer << BAEL_LOG_END;
                BAEL_LOG_TRACE << 'x' << BAEL_LOG_END;
                ASSERT(1 == bdesu_FileUtil::exists((filename + ".1").c_str()));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING LOG FILE ROLLING
        //
        // Concerns:
        //  1. The current log file is always rolled with a ".1" extension.
        //  2. If a file with ".N" extension exists, it is renamed to ".N+1".
        //
        // Plan:
        //  1. Setup the observer such that there will be a conflict in the
        //     file name.
        //  2. Call 'forceRotation' to cause file rotation and verify that the
        //     new file has the expected extension.
        //  3. Perform 'forceRotation' again and verify that the file is rolled
        //     properly.
        //  4. Trigger a rotation on file size and verify the files are rolled
        //     as expected.
        //
        // Testing:
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
        bael_LoggerManager::initSingleton(&multiplexObserver,
                                          configuration);

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging((filename + "%%").c_str(),
                                                 false));
                ASSERT(X.isFileLoggingEnabled());

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

                ASSERT(1 == bdesu_FileUtil::exists(filename.c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));

                ASSERT(2 == getNumLines(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
            }

            if (verbose) cout << "Testing forced rotation." << endl;
            {
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".1").c_str()));
                mX.forceRotation();
                ASSERT(1 == bdesu_FileUtil::exists((filename + ".1").c_str()));

                BAEL_LOG_TRACE << "log 2" << BAEL_LOG_END;
                BAEL_LOG_TRACE << "log 2" << BAEL_LOG_END;

                ASSERT(0 == bdesu_FileUtil::exists((filename + ".2").c_str()));
                mX.forceRotation();
                ASSERT(1 == bdesu_FileUtil::exists((filename + ".2").c_str()));

                BAEL_LOG_TRACE << "log 3" << BAEL_LOG_END;
                BAEL_LOG_TRACE << "log 3" << BAEL_LOG_END;
                BAEL_LOG_TRACE << "log 3" << BAEL_LOG_END;

                ASSERT(6 == getNumLines((filename).c_str()));
                ASSERT(4 == getNumLines((filename + ".1").c_str()));
                ASSERT(2 == getNumLines((filename + ".2").c_str()));
            }

            if (verbose) cout << "Testing rotate on size." << endl;
            {
                ASSERT(0 == X.rotationSize());
                mX.rotateOnSize(1);
                ASSERT(1 == X.rotationSize());
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".3").c_str()));

                char buffer[1024];
                memset(buffer, 'x', sizeof buffer);
                buffer[sizeof buffer - 1] = '\0';

                BAEL_LOG_TRACE << buffer << BAEL_LOG_END;
                BAEL_LOG_TRACE << "x" << BAEL_LOG_END;

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".3").c_str()));

                ASSERT(1024 <  getFileSize((filename + ".1").c_str()));
            }

            if (verbose) cout << "Testing max rolling file chain." << endl;
            {
                // Verify default value
                ASSERT(32 == X.maxFileChainSuffix());
                mX.setMaxFileChainSuffix(3);
                ASSERT(3 == X.maxFileChainSuffix());

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".3").c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".4").c_str()));

                mX.forceRotation();

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".3").c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".4").c_str()));

                mX.setMaxFileChainSuffix(4);
                ASSERT(4 == X.maxFileChainSuffix());

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".3").c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".4").c_str()));

                mX.forceRotation();

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".4").c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".5").c_str()));

                mX.forceRotation();

                ASSERT(1 == bdesu_FileUtil::exists((filename + ".4").c_str()));
                ASSERT(0 == bdesu_FileUtil::exists((filename + ".5").c_str()));
            }
        }

      } break;
#endif
      case 4: {
        // --------------------------------------------------------------------
        // CAPTURE ERROR MESSAGE WHEN THE STREAM FAILS
        //
        // Concerns:
        //   That an error message is written to stderr when the stream fails.
        //
        // Plan:
        //   Set the OS to limit the total size of any file we write to to
        //   a fairly small amount, then use BAEL to write more than that.
        //   while doing this, capture the stderr output and verify a message
        //   has been written.
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        bcema_TestAllocator ta(veryVeryVeryVerbose);

        bael_LoggerManagerConfiguration configuration;

        // Publish synchronously all messages regardless of their severity.
        // This configuration also guarantees that the observer will only see
        // each message only once.
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_TRACE,
                                                  bael_Severity::BAEL_OFF,
                                                  bael_Severity::BAEL_OFF));

        bael_MultiplexObserver multiplexObserver;
        bael_LoggerManager::initSingleton(&multiplexObserver,
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

            // I think this sets it so we won't trap when we get the file
            // size limit exception.
            struct sigaction act,oact;
            act.sa_handler = SIG_IGN;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ASSERT(0 == sigaction(SIGXFSZ, &act, &oact));

            Obj mX(&ta);  const Obj& X = mX;

            multiplexObserver.registerObserver(&mX);

            bsl::stringstream os;

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            // we want to capture the error message that will be written
            // to stderr (not cerr).  Redirect stderr to a file.  We can't
            // redirect it back; we'll have to use 'ASSERT2' (which outputs
            // to cout, not cerr) from now on and report a summary to
            // to cout at the end of this case.
            bsl::string stderrFN = tempFileName(veryVerbose);
            ASSERT(stderr == freopen(stderrFN.c_str(), "w", stderr));

            ASSERT2(0 == mX.enableFileLogging(smallFile.c_str(), true));
            ASSERT2(X.isFileLoggingEnabled());
            ASSERT2(1 == mX.enableFileLogging(smallFile.c_str(), true));

            for (int i = 0 ; i < 40 ;  ++i) {
                BAEL_LOG_TRACE << "log"  << BAEL_LOG_END;
            }

            fflush(stderr);
            bsl::fstream stderrFs;
            stderrFs.open(stderrFN.c_str(), bsl::ios_base::in);

            bsl::string line;
            ASSERT2(getline(stderrFs, line));  // caught an error message
#ifndef BSLS_PLATFORM__CMP_IBM
            // On native IBM, after the error, even when the stream fails,
            // logging will be attempted over and over again, which results in
            // more than one error messages.
            ASSERT2(!getline(stderrFs, line));  // and only one message
#endif

            mX.disableFileLogging();
            removeFilesByPrefix(smallFile.c_str());
            multiplexObserver.deregisterObserver(&mX);

            if (testStatus > 0) {
                cout << "Error, non-zero test status = " << testStatus
                     << "." << endl;
            }

            return testStatus;
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

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

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
            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

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
            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
            mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,1));
            ASSERT(bdet_DatetimeInterval(0,0,0,1) == X.rotationLifetime());

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
            bael_LoggerManager::initSingleton(&multiplexObserver,
                                              configuration);

#ifdef BSLS_PLATFORM__OS_UNIX
        bcema_TestAllocator ta(veryVeryVeryVerbose);
        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            bsl::string filename = tempFileName(veryVerbose);

            Obj mX(&ta);  const Obj& X = mX;
            multiplexObserver.registerObserver(&mX);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

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
                ASSERT(bdet_DatetimeInterval(0)       == X.rotationLifetime());
                mX.rotateOnLifetime(bdet_DatetimeInterval(0,0,0,3));
                ASSERT(bdet_DatetimeInterval(0,0,0,3) == X.rotationLifetime());
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;

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
                bcemt_ThreadUtil::microSleep(0, 4);
                BAEL_LOG_FATAL << "log 3" << BAEL_LOG_END;

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
                bcemt_ThreadUtil::microSleep(0, 2);
                mX.forceRotation();
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
                BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
                BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;

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
                bcemt_ThreadUtil::microSleep(0, 2);
                ASSERT(0 == X.rotationSize());
                mX.rotateOnSize(1);
                ASSERT(1 == X.rotationSize());
                for (int i = 0 ; i < 15; ++i) {
                    BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

                    // We sleep because otherwise, the loop is too fast to make
                    // the timestamp change so we cannot observe the rotation.

                    bcemt_ThreadUtil::microSleep(200 * 1000);
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
                    BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
                    bcemt_ThreadUtil::microSleep(50 * 1000);
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
        //   output of this observer with 'bael_DefaultObserver', that we
        //   slightly modify.  Then, We will use different manipulators and
        //   functors to affect output format and verify that it has changed
        //   where expected.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   bael_FileObserver(bael_Severity::Level, bslma_Allocator)
        //   ~bael_FileObserver()
        //   publish(const bael_Record& record, const bael_Context& context)
        //   void disableFileLogging()
        //   int enableFileLogging(const char *fileName, bool timestampFlag)
        //   void publish(const bael_Record&, const bael_Context&)
        //   bool isFileLoggingEnabled() const
        //   void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
        //   bool isPublishInLocalTimeEnabled() const;
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   bdet_DatetimeInterval localTimeOffset();
        // --------------------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVeryVerbose);
// TBD fix this for Windows !!!
#ifndef BSLS_PLATFORM__OS_WINDOWS
#if (!defined(BSLS_PLATFORM__OS_SOLARIS) || BSLS_PLATFORM__OS_VER_MAJOR >= 10)\
  && !defined(BSLS_PLATFORM__OS_AIX)
        // For the localtime to be picked to avoid the all.pl env to pollute
        // us.
        unsetenv("TZ");
#endif
        {
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
            bael_LoggerManager::initSingleton(&multiplexObserver,
                                              configuration);

            if (verbose) cout << "Testing default output format." << endl;
            {
                Obj mX;  const Obj& X = mX;
                bsl::ostringstream dos;

                time_t currentTime = time(0);
                struct tm localtm, gmtm;
#ifdef BSLS_PLATFORM__OS_WINDOWS
                localtm = *localtime(&currentTime);
                gmtm = *gmtime(&currentTime);
#else
                localtime_r(&currentTime, &localtm);
                gmtime_r(&currentTime, &gmtm);
#endif

                bdet_Datetime currentLocalTime, currentUTCTime;
                bdetu_Datetime::convertFromTm(&currentLocalTime, localtm);
                bdetu_Datetime::convertFromTm(&currentUTCTime, gmtm);

                ASSERT(currentLocalTime - currentUTCTime ==
                                                          X.localTimeOffset());

                bael_DefaultObserver defaultObserver(&dos);
                bael_MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX);
                localMultiObserver.registerObserver(&defaultObserver);
                multiplexObserver.registerObserver(&localMultiObserver);

                bsl::string filename = tempFileName(veryVerbose);
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                BAEL_LOG_WARN << "log WARN" << BAEL_LOG_END;

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

                bael_DefaultObserver defaultObserver(&dos);
                bael_MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX);
                localMultiObserver.registerObserver(&defaultObserver);
                multiplexObserver.registerObserver(&localMultiObserver);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                bsl::string filename = tempFileName(veryVerbose);
                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BAEL_LOG_FATAL << "log FATAL" << BAEL_LOG_END;

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, filename, os));

                testOs << "\nFATAL " << __FILE__ << ":" << __LINE__ - 1 <<
                          " bael_FileObserverTest log FATAL " << "\n";
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
#ifdef BSLS_PLATFORM__OS_UNIX
                tzset();
                time_t currentTime;
                struct tm *ts;
                ::time(&currentTime);
                ts = ::localtime(&currentTime);
#ifdef BDES_PLATFORM__OS_FREEBSD
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

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                ASSERT(0 == mX.enableFileLogging(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str()));

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
                BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
                BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
                BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
                BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

                bsl::string os;
                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                ASSERT( X.isFileLoggingEnabled());
                mX.disableFileLogging();
                ASSERT(!X.isFileLoggingEnabled());
                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
                BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
                BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
                BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
                BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                ASSERT(0 == mX.enableFileLogging(filename.c_str()));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str()));

                BAEL_LOG_TRACE << "log 7" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 8" << BAEL_LOG_END;
                BAEL_LOG_INFO <<  "log 9" << BAEL_LOG_END;
                BAEL_LOG_WARN <<  "log 1" << BAEL_LOG_END;
                BAEL_LOG_ERROR << "log 2" << BAEL_LOG_END;
                BAEL_LOG_FATAL << "log 3" << BAEL_LOG_END;

                ASSERT(24 == readFileIntoString(__LINE__, filename, os));

                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&mX);

            }

#ifdef BSLS_PLATFORM__OS_UNIX
            if (verbose) cout << "Testing file logging with timestamp."
                              << endl;
            {
                bsl::string filename = tempFileName(veryVerbose);

                Obj mX(&ta);  const Obj& X = mX;
                multiplexObserver.registerObserver(&mX);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                ASSERT(0 == mX.enableFileLogging(filename.c_str(), true));
                ASSERT(X.isFileLoggingEnabled());
                ASSERT(1 == mX.enableFileLogging(filename.c_str(), true));

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "log 2" << BAEL_LOG_END;
                BAEL_LOG_INFO <<  "log 3" << BAEL_LOG_END;
                BAEL_LOG_WARN <<  "log 4" << BAEL_LOG_END;
                BAEL_LOG_ERROR << "log 5" << BAEL_LOG_END;
                BAEL_LOG_FATAL << "log 6" << BAEL_LOG_END;

                bsl::string os;
                ASSERT(12 == readFileIntoString(__LINE__, filename, os));

                mX.disableFileLogging();
                removeFilesByPrefix(filename.c_str());
                multiplexObserver.deregisterObserver(&mX);
            }

            if (verbose) cout << "Testing log file name pattern." << endl;
            {
                bsl::string baseName = tempFileName(veryVerbose);
                bsl::string pattern  = baseName + "%Y%M%D%h%m%s";

                Obj mX(&ta);  const Obj& X = mX;
                multiplexObserver.registerObserver(&mX);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                bdet_Datetime startDatetime, endDatetime;

                mX.disableLifetimeRotation();
                mX.disableSizeRotation();
                mX.disableFileLogging();

                // loop until startDatetime is equal to endDatetime
                do {
                    startDatetime = getCurrentLocalTime();

                    ASSERT(0 == mX.enableFileLogging(pattern.c_str(), false));
                    ASSERT(X.isFileLoggingEnabled());
                    ASSERT(1 == mX.enableFileLogging(pattern.c_str(), false));

                    endDatetime = getCurrentLocalTime();

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
                bsl::ostringstream filename;
                filename << baseName;
                filename << bsl::setw(4) << bsl::setfill('0')
                         << startDatetime.year();
                filename << bsl::setw(2) << bsl::setfill('0')
                         << startDatetime.month();
                filename << bsl::setw(2) << bsl::setfill('0')
                         << startDatetime.day();
                filename << bsl::setw(2) << bsl::setfill('0')
                         << startDatetime.hour();
                filename << bsl::setw(2) << bsl::setfill('0')
                         << startDatetime.minute();
                filename << bsl::setw(2) << bsl::setfill('0')
                         << startDatetime.second();

                // look for the file with the constructed name
                glob_t globbuf;
                int x = glob(filename.str().c_str(), 0, 0, &globbuf);
                ASSERT(0 == x);
                ASSERT(1 == globbuf.gl_pathc);

                bsl::string os;
                ASSERT(2 == readFileIntoString(__LINE__, filename.str(), os));

                mX.disableFileLogging();
                removeFilesByPrefix(filename.str().c_str());
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
                bael_MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX1);
                localMultiObserver.registerObserver(&mX2);
                localMultiObserver.registerObserver(&mX3);
                multiplexObserver.registerObserver(&localMultiObserver);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

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
                mX2.setLogFileFunctor(bael_RecordStringFormatter());

                ASSERT(0 == mX3.enableFileLogging(filename3.c_str(), false));
                ASSERT(X3.isFileLoggingEnabled());
                ASSERT(1 == mX3.enableFileLogging(filename3.c_str(), false));
                mX3.setLogFileFunctor(&logRecord1);

                BAEL_LOG_WARN << "log" << BAEL_LOG_END;

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
                bael_MultiplexObserver localMultiObserver;
                localMultiObserver.registerObserver(&mX1);
                localMultiObserver.registerObserver(&mX2);
                localMultiObserver.registerObserver(&mX3);
                multiplexObserver.registerObserver(&localMultiObserver);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

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
                mX2.setLogFileFunctor(bael_RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));

                ASSERT(0 == mX3.enableFileLogging(filename3.c_str(), false));
                ASSERT(X3.isFileLoggingEnabled());
                ASSERT(1 == mX3.enableFileLogging(filename3.c_str(), false));
                mX3.setLogFileFunctor(&logRecord2);

                BAEL_LOG_WARN << "log" << BAEL_LOG_END;

                bsl::string file1, file2, file3;

                ASSERT(2 == readFileIntoString(__LINE__, filename1, file1));
                ASSERT(2 == readFileIntoString(__LINE__, filename2, file2));
                ASSERT(2 == readFileIntoString(__LINE__, filename3, file3));

                ASSERT(file1 != file2);
                ASSERT(file1 != file3);
                ASSERT(file2 != file3);

                // at this point we have three different logs produced by
                // these three fileobservers configured with different
                // formats;
                // now we are going to reuse one of these fileobserver and
                // change its functors to see if the resulting log should
                // be identical to one of those known logs in file1, file2,
                // and file3

                bsl::string filename1a = tempFileName(veryVerbose);
                bsl::string fileContent;

                mX1.disableFileLogging();
                ASSERT(!X1.isFileLoggingEnabled());
                ASSERT(0 == mX1.enableFileLogging(filename1a.c_str(), false));
                ASSERT( X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                mX1.setLogFileFunctor(bael_RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));
                BAEL_LOG_WARN << "log" << BAEL_LOG_END;
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
                BAEL_LOG_WARN << "log" << BAEL_LOG_END;
                ASSERT(2 == readFileIntoString(__LINE__, filename1b,
                                               fileContent));
                ASSERT(file3 == fileContent);

                bsl::string filename1c = tempFileName(veryVerbose);
                mX1.disableFileLogging();
                ASSERT(!X1.isFileLoggingEnabled());
                ASSERT(0 == mX1.enableFileLogging(filename1c.c_str(), false));
                ASSERT( X1.isFileLoggingEnabled());
                ASSERT(1 == mX1.enableFileLogging(filename1.c_str(), false));

                mX1.setLogFileFunctor(bael_RecordStringFormatter(
                                       "\n%p %t %s %f %c %m %u\n"));
                BAEL_LOG_WARN << "log" << BAEL_LOG_END;
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
