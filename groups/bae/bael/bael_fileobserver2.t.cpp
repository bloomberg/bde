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
// [ 1] bael_FileObserver(bslma_Allocator)
// [ 1] ~bael_FileObserver()
//
// MANIPULATORS
// [ 1] publish(const bael_Record& record, const bael_Context& context)
// [ 1] void disableFileLogging()
// [ 2] void disableLifetimeRotation()
// [ 2] void disableSizeRotation()
// [ 1] void disablePublishInLocalTime()
// [ 9] void disableTimeIntervalRotation();
// [ 1] int enableFileLogging(const char *fileName, bool timestampFlag = false)
// [ 1] void enablePublishInLocalTime()
// [ 1] void publish(const bael_Record& record, const bael_Context& context)
// [ 2] void forceRotation()
// [ 2] void rotateOnSize(int size)
// [ 9] void rotateOnTimeInterval(const bdet_DatetimeInterval& interval);
// [ 2] void rotateOnLifetime(bdet_DatetimeInterval timeInterval)
// [10] void rotateOnTimeInterval(const DtInterval& i, const Datetime& r);
// [ 1] void setLogFileFunctor(const logRecordFunctor& logFileFunctor);
// [ 3] void setMaxLogFiles();
// [ 6] void setOnFileRotationCallback(const OnFileRotationCallback&);
// [ 3] int removeExcessLogFiles();
//
// ACCESSORS
// [ 1] bool isFileLoggingEnabled() const
// [ 1] bool isPublishInLocalTimeEnabled() const
// [ 2] bdet_DatetimeInterval rotationLifetime() const
// [ 2] int rotationSize() const
// [ 3] int maxLogFiles() const;
//-----------------------------------------------------------------------------
// [ 5] CONCERN: Rotated log filenames are as expected
// [ 8] CONCERN: 'rotateOnSize' triggers correctly for existing files
// [ 7] CONCERN: Rotation on size is based on file size

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

bdet_Datetime getCurrentLocalTime()
{
    time_t currentTime = time(0);
    struct tm localtm;
#ifdef BSLS_PLATFORM_OS_WINDOWS
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
#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "bael", 0, tmpNameBuf);
    result = tmpNameBuf;
#elif defined(BSLS_PLATFORM_OS_HPUX)
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

int readFileIntoString(int lineNum, const bsl::string& filename,
                       bsl::string& fileContent)
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

class ReentrantRotationCallback {
    // This class can be used as a functor matching the signature of
    // 'bael_FileObserver2::OnFileRotationCallback'.  This class implements
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
    bael_RecordAttributes attr(bdetu_SystemTime::nowAsDatetimeUtc(),
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

bsls_Types::Int64 getFileSize(const char *filename)
{
    bsl::ifstream fs;
    fs.open(filename, bsl::ifstream::in);
    fs.clear();
    ASSERT(fs.is_open());

    bsls_Types::Int64 fileSize = 0;

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

        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        mX.rotateOnTimeInterval(bdet_DatetimeInterval(1));

        BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

        ASSERT(0 == cb.numInvocations());

        mX.disableFileLogging();
        bdesu_FileUtil::remove(BASENAME.c_str());
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

        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        if (veryVerbose) cout <<
                         "Test lower bound of absolute time reference" << endl;
        {
            mX.disableFileLogging();

            // Ensure log file did not exist

            bdesu_FileUtil::remove(BASENAME.c_str());

            mX.rotateOnTimeInterval(bdet_DatetimeInterval(0, 0, 0, 2),
                                    bdet_Datetime(1, 1, 1));

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            bcemt_ThreadUtil::microSleep(0, 2);
            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));

            cb.reset();
        }

        if (veryVerbose) cout <<
                         "Test upper bound of absolute time reference" << endl;
        {
            mX.disableFileLogging();

            // Ensure log file did not exist

            bdesu_FileUtil::remove(BASENAME.c_str());

            mX.rotateOnTimeInterval(bdet_DatetimeInterval(0, 0, 0, 2),
                                    bdet_Datetime(9999, 12, 31, 23, 59, 59));

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            bcemt_ThreadUtil::microSleep(0, 2);
            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));

            cb.reset();
        }

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
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

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
        //  void rotateOnTimeInterval(const bdet_DatetimeInterval& interval);
        //  void disableTimeIntervalRotation();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Time-Based Rotation"
                          << "\n===========================" << endl;


        if (veryVerbose) cout << "Test infrastructure setup" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        bael_LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        Obj mX(&ta); const Obj& X = mX;

        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        mX.rotateOnTimeInterval(bdet_DatetimeInterval(0, 0, 0, 3));

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());
        ASSERT(0 == cb.numInvocations());

        BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

        ASSERT(1 == bdesu_FileUtil::exists(BASENAME.c_str()));
        LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

        if (veryVerbose) cout << "Test normal rotation" << endl;
        {
            cb.reset();
            bcemt_ThreadUtil::microSleep(0, 2);

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());

            bcemt_ThreadUtil::microSleep(0, 1);

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;
            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Test delayed logging" << endl;
        {
            cb.reset();
            bcemt_ThreadUtil::microSleep(0, 5);

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));

            bcemt_ThreadUtil::microSleep(0, 1);
            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 2 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout <<
                           "Test rotation between scheduled rotations" << endl;
        {
            cb.reset();
            bcemt_ThreadUtil::microSleep(0, 1);

            mX.forceRotation();

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));

            bcemt_ThreadUtil::microSleep(0, 2);

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 2 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout <<
                       "Test disabling file logging between rotations" << endl;
        {
            cb.reset();
            bcemt_ThreadUtil::microSleep(0, 3);

            mX.disableFileLogging();

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
        }

        if (veryVerbose) cout << "Test 'disableTimeIntervalRotation" << endl;
        {
            cb.reset();
            bcemt_ThreadUtil::microSleep(0, 3);

            mX.disableTimeIntervalRotation();

            BAEL_LOG_TRACE << "log" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 0 == cb.numInvocations());
        }

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
        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_TRACE,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        // Set callback to monitor rotation.

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        bael_LoggerManager::initSingleton(&mX, configuration);

        if (verbose) cout << "Test-case infrastructure setup." << endl;
        {
            const bsl::string BASENAME = tempFileName(veryVerbose);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            if (verbose) cout << "Testing setup." << endl;
            {
                ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
                mX.rotateOnSize(1);
                ASSERT(X.isFileLoggingEnabled());

                BAEL_LOG_TRACE << "log 1" << BAEL_LOG_END;

                ASSERT(1 == bdesu_FileUtil::exists(BASENAME.c_str()));

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

                BAEL_LOG_TRACE << buffer << BAEL_LOG_END;
                mX.disableFileLogging();

                ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));

                BAEL_LOG_TRACE << 'x' << BAEL_LOG_END;

                ASSERT(0 == cb.numInvocations());

                BAEL_LOG_TRACE << buffer << BAEL_LOG_END;
                BAEL_LOG_TRACE << 'x' << BAEL_LOG_END;

                ASSERT(1 == bdesu_FileUtil::exists(BASENAME.c_str()));
                ASSERT(1 == cb.numInvocations());
                ASSERT(1 ==
                         bdesu_FileUtil::exists(cb.rotatedFileName().c_str()));
            }
        }
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

        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta); const Obj& X = mX;

        bael_LoggerManager::initSingleton(&mX, configuration);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        {
            const bsl::string BASENAME = tempFileName(veryVerbose);

            BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
            ASSERT(X.isFileLoggingEnabled());

            char buffer[1024];
            memset(buffer, 'x', sizeof buffer);
            buffer[sizeof buffer - 1] = '\0';

            BAEL_LOG_TRACE << buffer << BAEL_LOG_END;

            mX.disableFileLogging();
            ASSERT(!X.isFileLoggingEnabled());

            ASSERT(1024 < getFileSize(BASENAME.c_str()));

            ASSERT(0 == X.rotationSize());
            mX.rotateOnSize(1);
            ASSERT(1 == X.rotationSize());

            ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
            ASSERT(X.isFileLoggingEnabled());

            BAEL_LOG_TRACE << "x" << BAEL_LOG_END;

            LOOP_ASSERT(cb.numInvocations(), 1 == cb.numInvocations());
            LOOP_ASSERT(cb.status(), 0 == cb.status());

            ASSERT(1 == bdesu_FileUtil::exists(cb.rotatedFileName()));
        }
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
                bcemt_ThreadUtil::microSleep(0, 1);

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
                bdesu_FileUtil::remove(files[i]);
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
                bcemt_ThreadUtil::microSleep(0, 1);

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
                 bdesu_FileUtil::remove(files[i]);
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
            std::memset(buffer, 'x', sizeof(buffer));
            buffer[sizeof(buffer) - 1] = 0;

            bsl::vector<bsl::string> files(Z);

            publishRecord(&mX, buffer);

            for (int i = 0; i < 3; ++i) {
                // A sleep is required because timestamp resolution is 1 second
                bcemt_ThreadUtil::microSleep(0, 1);

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
            mX.rotateOnTimeInterval(bdet_DatetimeInterval(0, 0, 0, 1));
            buffer[1] = 0;  // Don't need to write much for time-based rotation

            for (int i = 0; i < 3; ++i) {
                bcemt_ThreadUtil::microSleep(0, 1);

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
                bdesu_FileUtil::remove(files[i]);
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
        // TESTING ROTATED FILENAME
        //
        // Concerns:
        //: 1 A timestamp is appended to a file the rotated file has the same
        //:   name as the current log file.
        //:
        //: 2 The callback function is correctly supplied with the name of the
        //:   rotated file.
        //
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
        //
        // Testing:
        //  CONCERN: Rotated log filenames are as expected
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing filename pattern" << endl;

        if (veryVerbose) cout << "Test infrastructure setup." << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        bael_LoggerManagerConfiguration configuration;
        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        Obj mX(&ta); const Obj& X = mX;

        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        RotCb cb(Z);
        mX.setOnFileRotationCallback(cb);

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        if (veryVerbose) cout <<
                               "Create table of distinct test values." << endl;

        static const struct {
            int         d_line;
            const char *d_suffix;
            const char *d_expectedPattern;
            bool        d_uniqueNameFlag;
        } DATA[] = {

        //LINE SUFFIX     PATTERN      UNIQUE

        { L_,  "",        "",          false},
        { L_,  "%Y",      "Y",         false},
        { L_,  "%Y%M%D",  "YMD",       false},
        { L_,  ".%T",     ".YMD_hms",  true},

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose) cout << "Test filenames" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_line;
            const char *SUF  = DATA[ti].d_suffix;
            const char *PAT  = DATA[ti].d_expectedPattern;
            const bool  UNI  = DATA[ti].d_uniqueNameFlag;

            bdet_Datetime startDatetime, endDatetime;

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
                    ASSERT(0 == bsl::remove(fn.c_str()));
                }

            } while (!X.isFileLoggingEnabled());

            LOOP_ASSERT(LINE, startDatetime.year()   == endDatetime.year());
            LOOP_ASSERT(LINE, startDatetime.month()  == endDatetime.month());
            LOOP_ASSERT(LINE, startDatetime.day()    == endDatetime.day());
            LOOP_ASSERT(LINE, startDatetime.hour()   == endDatetime.hour());
            LOOP_ASSERT(LINE, startDatetime.minute() == endDatetime.minute());
            LOOP_ASSERT(LINE, startDatetime.second() == endDatetime.second());

            BAEL_LOG_INFO<< "log" << BAEL_LOG_END;

            // now construct the name of the log file from
            // startDatetime

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
                  default: {
                    oss << *c;
                  } break;
                }
            }

            const bsl::string LOGNAME = oss.str().c_str();
            LOOP_ASSERT(LINE, 1 == bdesu_FileUtil::exists(LOGNAME.c_str()));

            bcemt_ThreadUtil::microSleep(0, 1);
            mX.forceRotation();

            mX.disableFileLogging();

            LOOP_ASSERT(LINE, 1 == bdesu_FileUtil::exists(LOGNAME.c_str()));

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
                LOOP2_ASSERT(LINE, ROTATED_NAME.c_str(),
                            1 == bdesu_FileUtil::exists(ROTATED_NAME.c_str()));
                LOOP_ASSERT(LINE, 2 == getNumLines(ROTATED_NAME.c_str()));
            }

            LOOP2_ASSERT(ti, cb.numInvocations(), 1 == cb.numInvocations());
            LOOP2_ASSERT(ti, cb.status(), 0 == cb.status());
            LOOP2_ASSERT(ti, cb.rotatedFileName(),
                         ROTATED_NAME == cb.rotatedFileName());

            cb.reset();
            removeFilesByPrefix(BASENAME.c_str());
        }
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
        //   a fairly small amount, then use BAEL to write more than that.
        //   while doing this, capture the stderr output and verify a message
        //   has been written.
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_UNIX
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

            multiplexObserver.deregisterObserver(&mX);
// TBD
#if 0
            ASSERT(0 == X.maxLogFiles());
            mX.setMaxLogFiles(5);
            ASSERT(5 == X.maxLogFiles());

            bsl::string filename = tempFileName(veryVerbose);

#ifdef BSLS_PLATFORM_OS_UNIX
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

#ifdef BSLS_PLATFORM_OS_UNIX
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
#ifndef BSLS_PLATFORM_OS_WINDOWS
#if (!defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR >= 10)\
  && !defined(BSLS_PLATFORM_OS_AIX)
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
#ifdef BSLS_PLATFORM_OS_WINDOWS
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
#ifdef BSLS_PLATFORM_OS_UNIX
                tzset();
                time_t currentTime;
                struct tm *ts;
                ::time(&currentTime);
                ts = ::localtime(&currentTime);
#ifdef BDES_PLATFORM_OS_FREEBSD
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

#ifdef BSLS_PLATFORM_OS_UNIX
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
                bsl::string pattern  = baseName + ".%Y%M%D_%h%m%s";

                Obj mX(&ta);  const Obj& X = mX;
                multiplexObserver.registerObserver(&mX);

                BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

                bdet_Datetime startDatetime, endDatetime;

                mX.disableLifetimeRotation();
                mX.disableSizeRotation();
                mX.disableFileLogging();

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;

                    // loop until startDatetime is equal to endDatetime
                    do {
                        startDatetime = getCurrentLocalTime();

                        switch (CONFIG) {
                          case 'a': {
                            LOOP_ASSERT(CONFIG,
                                        0 == mX.enableFileLogging(
                                                               pattern.c_str(),
                                                               false));
                            LOOP_ASSERT(CONFIG, X.isFileLoggingEnabled());
                            LOOP_ASSERT(CONFIG,
                                        1 == mX.enableFileLogging(
                                                               pattern.c_str(),
                                                               false));
                          } break;
                          case 'b': {
                            LOOP_ASSERT(CONFIG,
                                        0 == mX.enableFileLogging(
                                                    (baseName + ".%T").c_str(),
                                                    false));
                            LOOP_ASSERT(CONFIG, X.isFileLoggingEnabled());
                            LOOP_ASSERT(CONFIG, 1 == mX.enableFileLogging(
                                                    (baseName + ".%T").c_str(),
                                                    true));
                          } break;
                          case 'c': {
                            LOOP_ASSERT(CONFIG,
                                        0 == mX.enableFileLogging(
                                                              baseName.c_str(),
                                                              true));
                            LOOP_ASSERT(CONFIG, X.isFileLoggingEnabled());
                            LOOP_ASSERT(CONFIG,
                                        1 == mX.enableFileLogging(
                                                              baseName.c_str(),
                                                               true));
                          } break;
                          case 'd': {
                            LOOP_ASSERT(CONFIG,
                                        0 == mX.enableFileLogging(
                                                               pattern.c_str(),
                                                               true));
                            LOOP_ASSERT(CONFIG, X.isFileLoggingEnabled());
                            LOOP_ASSERT(CONFIG,
                                        1 == mX.enableFileLogging(
                                                               pattern.c_str(),
                                                               true));
                          } break;
                        }

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
                            ASSERT(0 == bsl::remove(fn.c_str()));
                        }

                    } while (!X.isFileLoggingEnabled());

                    LOOP_ASSERT(CONFIG,
                                startDatetime.year()   == endDatetime.year());
                    LOOP_ASSERT(CONFIG,
                                startDatetime.month()  == endDatetime.month());
                    LOOP_ASSERT(CONFIG,
                                startDatetime.day()    == endDatetime.day());
                    LOOP_ASSERT(CONFIG,
                                startDatetime.hour()   == endDatetime.hour());
                    LOOP_ASSERT(CONFIG,
                               startDatetime.minute() == endDatetime.minute());
                    LOOP_ASSERT(CONFIG,
                               startDatetime.second() == endDatetime.second());

                    BAEL_LOG_INFO<< "log" << BAEL_LOG_END;

                    mX.disableFileLogging();

                    // now construct the name of the log file from
                    // startDatetime

                    bsl::ostringstream filename;
                    filename << baseName;
                    filename << '.';
                    filename << bsl::setw(4) << bsl::setfill('0')
                             << startDatetime.year();
                    filename << bsl::setw(2) << bsl::setfill('0')
                             << startDatetime.month();
                    filename << bsl::setw(2) << bsl::setfill('0')
                             << startDatetime.day();
                    filename << '_';
                    filename << bsl::setw(2) << bsl::setfill('0')
                             << startDatetime.hour();
                    filename << bsl::setw(2) << bsl::setfill('0')
                             << startDatetime.minute();
                    filename << bsl::setw(2) << bsl::setfill('0')
                             << startDatetime.second();

                    // look for the file with the constructed name
                    glob_t globbuf;
                    int x = glob(filename.str().c_str(), 0, 0, &globbuf);
                    LOOP_ASSERT(CONFIG, 0 == x);
                    LOOP_ASSERT(CONFIG, 1 == globbuf.gl_pathc);

                    bsl::string os;
                    LOOP_ASSERT(CONFIG, 2 == readFileIntoString(__LINE__,
                                                   filename.str(),
                                                   os));

                    mX.disableFileLogging();
                    removeFilesByPrefix(filename.str().c_str());
                }
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
      case -1: {
        // --------------------------------------------------------------------
        // LARGE FILE TEST
        //
        // Concern:
        //: 1 'bael_FileObserver2' is able to write to a file over 2 GB.
        //
        // Plan:
        //: 1 Keep logging to a file until it is over 5 GB.  Manually verify
        //:   the file created is as expected.
        //
        // Testing:
        //   CONCERN: 'bael_FileObserver2' is able to write to a file over 2 GB
        // --------------------------------------------------------------------
        bael_LoggerManagerConfiguration configuration;

        ASSERT(0 == configuration.setDefaultThresholdLevelsIfValid(
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_TRACE,
                                                     bael_Severity::BAEL_OFF,
                                                     bael_Severity::BAEL_OFF));

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;
        bael_LoggerManager::initSingleton(&mX, configuration);

        const bsl::string BASENAME = tempFileName(veryVerbose);

        ASSERT(0 == mX.enableFileLogging(BASENAME.c_str()));
        ASSERT(X.isFileLoggingEnabled());

        BAEL_LOG_SET_CATEGORY("bael_FileObserverTest");

        P(BASENAME);

        char buffer[1026];
        memset(buffer, 'x', sizeof buffer);
        buffer[sizeof buffer - 1] = '\0';

        // Write over 5 GB
        for (int i = 0; i < 5000000; ++i) {
            BAEL_LOG_WARN << buffer << BAEL_LOG_END;
        }

        //bdesu_FileUtil::remove(BASENAME.c_str());

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
