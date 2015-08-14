// bdlsu_xxxfileutil.t.cpp                                            -*-C++-*-
#include <bdlsu_xxxfileutil.h>

#include <bdlsu_memoryutil.h>
#include <bdlsu_pathutil.h>

#include <bdlf_bind.h>
#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsl_algorithm.h>
#include <bsl_c_errno.h>
#include <bsl_c_stdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <utime.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#else // BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>  // for Sleep, GetLastError
#include <io.h>
#endif


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static void findMatchingPaths(bsl::vector<bsl::string> *,const char *);
// [ 3] static bool isRegularFile(const bsl::string&, bool);
// [ 3] static bool isRegularFile(const char *, bool);
// [ 3] static bool isDirectory(const bsl::string&, bool);
// [ 3] static bool isDirectory(const char *, bool);
// [ 4] static int rollFileChain(const bsl::string&, int);
// [ 4] static int rollFileChain(const char *, int);
// [ 5] static Offset getAvailableSpace(const bsl::string&);
// [ 5] static Offset getAvailableSpace(const char *);
// [ 5] static Offset getAvailableSpace(FileDescriptor);
// [ 6] static Offset getFileSize(const bsl::string&);
// [ 6] static Offset getFileSize(const char *);
// [ 8] FD open(const char *p, bool writable, bool exist, bool append);
// [ 9] static Offset getFileSizeLimit()
// [10] int tryLock(FileDescriptor, bool ) (Unix)
// [11] int tryLock(FileDescriptor, bool ) (Windows)
// [13] int sync(char *, int , bool )
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCERN: findMatchingPaths incorrect on ibm 64-bit
// [12] CONCERN: Open in append-mode behavior (particularly on windows)
// [14] CONCERN: Unix File Permissions for 'open'
// [15] CONCERN: Unix File Permissions for 'createDirectories'
// [16] USAGE EXAMPLE 1
// [17] USAGE EXAMPLE 2

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define PL(X) cout << "Line: " << __LINE__ << ", " #X " = " << (X) << endl;
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                           GLOBAL DATA FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS
#   define PS "\\"
#else
#   define PS "/"
#endif

//=============================================================================
//                  GLOBAL HELPER TYPE FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlsu::FileUtil Obj;

#ifdef BSLS_PLATFORM_OS_WINDOWS
inline
bool isBackSlash (char t)
{
    return t == '\\';
}

inline
bool isForwardSlash (char t)
{
    return t == '/';
}

#endif

void localTouch(const char *fileName)
{
    Obj::FileDescriptor fd = Obj::open(fileName, true, false);
    ASSERT(Obj::INVALID_FD != fd);
    // ASSERT(1 == Obj::write(fd, "x", 1));

    Obj::close(fd);
}

void localSleep(int seconds)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    sleep(seconds);
#else
    ::Sleep(seconds * 1000);
#endif
}

bsls::Types::Int64 localGetPId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

static
void localForkExec(bsl::string command)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    if (0 == fork()) {
        // child process

        bsl::vector<char *>  argvec;
        const char          *endp = command.data() + command.length();
        BSLS_ASSERT_OPT(*endp == 0);

        char *pc;
        for (pc = &command[0]; pc < endp; ) {
            argvec.push_back(pc);
            while (*pc && ' ' != *pc) {
                ++pc;
            }
            if (pc < endp) {
                *pc++ = 0;
            }
        }
        BSLS_ASSERT_OPT(endp == pc);
        argvec.push_back(0);

        execv(argvec[0], argvec.data());

        BSLS_ASSERT_OPT(0 && "execv failed");
    }
#else
    STARTUPINFO sui;
    GetStartupInfo(&sui);

    PROCESS_INFORMATION proci;

    char *cmd = const_cast<char *>(command.c_str());
    bool  rc  = CreateProcess(NULL,     // lpApplicationName
                              cmd,      // lpCommandLine
                              NULL,     // lpProcessAttributes
                              NULL,     // lpThreadAttributes
                              true,     // bInheritHandles
                              0,        // dwCreationFlags
                              NULL,     // lpEnvironment
                              NULL,     // lpCurrentDirectory
                              &sui,     // lpStartupInfo - in
                              &proci);  // lpProcessInformation - out
    ASSERT(rc);
#endif
}

string rollupPaths(vector<bsl::string>& paths)
{
   string result;

   sort(paths.begin(), paths.end());

   for (vector<bsl::string>::const_iterator it = paths.begin();
        it != paths.end(); ++it) {
      result.append(*it);
      result.push_back(':');
   }

   if (!result.empty()) {
      result.erase(result.end()-1);
   }

#ifdef BSLS_PLATFORM_OS_WINDOWS
   replace_if(result.begin(), result.end(), ::isBackSlash, '/');
#endif
   return result;
}

inline
void makeArbitraryFile(const char *path)
{
    bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(path,1,0);
    ASSERT(bdlsu::FileUtil::INVALID_FD != fd);
    ASSERT(5 == bdlsu::FileUtil::write(fd, "hello", 5));
    ASSERT(0 == bdlsu::FileUtil::close(fd));
}

static
bsl::string tempFileName(int         testCase,
                         const char *fnTemplate = 0)
    // Return a temporary file name, with 'testCase' being part of the file
    // name, with with 'fnTemplate', if specified, also being part of the file
    // name.
{

#ifndef BSLS_PLATFORM_OS_WINDOWS
    bsl::ostringstream oss;
    oss << "tmp.filesystemutil." << testCase << '.' << ::localGetPId();
    if (fnTemplate) {
        oss << '.' << fnTemplate;
    }

    bsl::string result(oss.str());
    result += "_XXXXXX";
    close(mkstemp(&result[0]));
#else
    // We can't make proper use of 'fnTemplate' on Windows.  We have created
    // a local directory to put our files in and chdir'ed to it, so
    // 'tmpPathBuf' should just be ".".  'GetTempFileName' is a really lame
    // utility, other than the path, it allows us to specify only 3 chars of
    // file name (!????!!!!!).
    //: o The first will be 'T' (for 'tmp').
    //: o The next will be 'A' + test case #, accomodating up to 25 test cases.
    //: o The third will be 'A' - 1 + '# of calls' allowing this function to
    //:   be called 26 times in any one process (each test case is run in a
    //:   separate process).

    (void) fnTemplate;    // We have to ignore this -- can't use it.

    static int calls = 0;
    char tplt[4] = { 'T', char('A' + testCase), char('A' + calls++), '\0' };
    ASSERT(tplt[1] <= 'Z');
    ASSERT(tplt[2] <= 'Z');

    char tmpPathBuf[MAX_PATH] = { "." }, tmpNameBuf[MAX_PATH];
    GetTempFileName(tmpPathBuf, tplt, 0, tmpNameBuf);
    bsl::string result(tmpNameBuf);
#endif

    // Test Invariant:

    ASSERT(!result.empty());
    return result;
}

class MMIXRand {
    // Pseudo-Random number generator based on Donald Knuth's 'MMIX'

    static const bsls::Types::Uint64 A = 6364136223846793005ULL;
    static const bsls::Types::Uint64 C = 1442695040888963407ULL;

    // DATA
    bsls::Types::Uint64 d_reg;
    bsl::stringstream   d_ss;
    char                d_outBuffer[17];

  public:
    // CREATOR
    MMIXRand()
    : d_reg(0)
    {
        memset(d_outBuffer, 0, sizeof(d_outBuffer));
    }

    // MANIPULATORS
    void munge()
        // Iterate 'd_reg' through one cycle
    {
        d_reg = d_reg * A + C;
    }

    void reset()
        // Reset 'd_reg'
    {
        d_reg = 0;
    }

    const char *display()
        // Display the current state of d_reg in hex
    {
        d_ss.str("");
        memset(d_outBuffer, ' ', 16);

        d_ss << bsl::hex << d_reg;
        const bsl::string& str = d_ss.str();
        LOOP_ASSERT(str.length(), 16 >= str.length());
        char *writeTo = d_outBuffer + (16 - str.length());

        bsl::strcpy(writeTo, str.c_str());
        ASSERT(16 == bsl::strlen(d_outBuffer));

        return d_outBuffer;
    }
};


void NoOpAssertHandler(const char *, const char *, int)
{
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
// Ensures that the following functions in usage example 2 compiles and runs on
// all platforms.
//-----------------------------------------------------------------------------

namespace UsageExample2 {

///Example 2: Using 'bdlsu::FileUtil::visitPaths'
///- - - - - - - - - - - - - - - - - - - - - - -
// 'bdlsu::FileUtil::visitPaths' enables clients to define a functor to operate
// on file paths that match a specified pattern.  In this example, we create a
// function that can be used to filter out files that have a last modified time
// within a particular time frame.
//
// First we define our filtering function:
//..
    void getFilesWithinTimeframe(bsl::vector<bsl::string> *vector,
                                 const char               *item,
                                 const bdlt::Datetime&      start,
                                 const bdlt::Datetime&      end)
    {
        bdlt::Datetime datetime;
        int ret = bdlsu::FileUtil::getLastModificationTime(&datetime, item);

        if (ret) {
            return;                                                   // RETURN
        }

        if (datetime < start || datetime > end) {
            return;                                                   // RETURN
        }

        vector->push_back(item);
    }
//..
// Then, with the help of 'bdlsu::FileUtil::visitPaths' and
// 'bdlf::BindUtil::bind', we create a function for finding all file paths that
// match a specified pattern and have a last modified time within a specified
// start and end time (both specified as a 'bdlt::Datetime'):
//..
    void findMatchingFilesInTimeframe(bsl::vector<bsl::string> *result,
                                      const char               *pattern,
                                      const bdlt::Datetime&      start,
                                      const bdlt::Datetime&      end)
    {
        result->clear();
        bdlsu::FileUtil::visitPaths(
                                  pattern,
                                  bdlf::BindUtil::bind(&getFilesWithinTimeframe,
                                                      result,
                                                      bdlf::PlaceHolders::_1,
                                                      start,
                                                      end));
    }
//..

}  // close namespace UsageExample2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bsl::string origWorkingDir;
    ASSERT(0 == bdlsu::FileUtil::getWorkingDirectory(&origWorkingDir));

    // Make 'absoluteTaskPath' be a absolute path to the executable, for when
    // we fork / exec from different directories.

    const bsl::string absoluteTaskPath = PS[0] == *argv[0] ||
                                                    bsl::strstr(argv[0], ":\\")
                                       ? bsl::string(argv[0])
                                       : origWorkingDir + PS + argv[0];
    LOOP_ASSERT(absoluteTaskPath, bdlsu::FileUtil::exists(absoluteTaskPath));
    if (veryVerbose) P(absoluteTaskPath);

    bsl::string tmpWorkingDir;
    {
        // Must not call 'tempFileName' here, because 'tempFileName' would
        // create a plain file with the result name, and the attempt to
        // create the directory would fail.

#ifdef BSLS_PLATFORM_OS_UNIX
        char host[80];
        ASSERT(0 ==::gethostname(host, sizeof(host)));
#else
        const char *host = "win";     // 'gethostname' is difficult on
                                      // Windows, and we usually aren't using
                                      // nfs there anyway.
#endif

        bsl::ostringstream oss;
        oss << "tmp.fileutil.case_" << test << '.' << host << '.' <<
                                                               ::localGetPId();
        tmpWorkingDir = oss.str();
    }
    if (veryVerbose) P(tmpWorkingDir);

    if (bdlsu::FileUtil::exists(tmpWorkingDir)) {
        // Sometimes the cleanup at the end of this program is unable to clean
        // up files, so we might encounter leftovers from a previous run, but
        // these can usually be deleted if sufficient time has elapsed.  If
        // we're not able to clean it up now, old files may prevent the test
        // case we're running this time from working.

        LOOP_ASSERT(tmpWorkingDir,
                             0 == bdlsu::FileUtil::remove(tmpWorkingDir, true));
    }
    ASSERT(0 == bdlsu::FileUtil::createDirectories(tmpWorkingDir, true));
    ASSERT(0 == bdlsu::FileUtil::setWorkingDirectory(tmpWorkingDir));

    switch(test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Define the functions provided in usage example 2, then test them
        //   and make sure they function as intended, by:
        //
        //   1) Create a series of file in 1 second intervals, and record the
        //      time of creation.
        //   2) Call the 'findMatchingFilesInTimeframe' function on the list of
        //      files created with the timeframe specified as an interval
        //      in between all file creations.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Usage Example 2\n"
                             "=======================\n";

        // make sure there isn't an unfortunately named file in the way

        bdlsu::FileUtil::remove("usage.2", true);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        bsl::string logPath =  "usage.2\\logs2\\";
#else
        bsl::string logPath =  "usage.2/logs2/";
#endif

        ASSERT(0 == bdlsu::FileUtil::createDirectories(logPath.c_str(), true));
        const int TESTSIZE = 10;
        bdlt::Datetime modTime[TESTSIZE];

        for (int i = 0; i < TESTSIZE; ++i) {
            stringstream s;
            s << logPath << "file" << i << ".log";

            if (veryVerbose) {
                cout << "Creating file: " << s.str() << endl;
            }

            bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(s.str(),
                                                                     true,
                                                                     false);
            ASSERT(0 != fd);

            const char buffer[] = "testing";
            int bytes           = sizeof buffer;

            bdlsu::FileUtil::write(fd, buffer, bytes);
            bdlsu::FileUtil::close(fd);

#ifdef BSLS_PLATFORM_OS_WINDOWS
            Sleep(1000);  // 'Sleep' is in milliseconds on Windows.
#else
            sleep(1);
#endif

            bdlsu::FileUtil::getLastModificationTime(&modTime[i], s.str());
            if (veryVerbose) {
                cout << "\tLast modification time: " << modTime[i] << endl;
            }
        }

        bsl::vector<bsl::string> results;
        bsl::string pattern = logPath + "*.log";
        const int START = 3;
        const int END   = 6;
        UsageExample2::findMatchingFilesInTimeframe(&results,
                                                    pattern.c_str(),
                                                    modTime[START],
                                                    modTime[END]);

        if (veryVerbose) {
            cout << "List of files found: " << endl;
            for (int i = 0; i < (int) results.size(); ++i) {
                bsl::cout << "\t" << results[i] << endl;
            }
        }

        ASSERT(results.size() == END - START + 1);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example 1
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Usage Example 1\n"
                             "=======================\n";

        // make sure there isn't an unfortunately named file in the way

        bdlsu::FileUtil::remove("usage.1", true);

///Example 1: General Usage
///- - - - - - - - - - - - -
// In this example, we start with a (relative) native path to a directory
// containing log files:
//..
    #ifdef BSLS_PLATFORM_OS_WINDOWS
      bsl::string logPath = "usage.1\\logs";
    #else
      bsl::string logPath = "usage.1/logs";
    #endif
//..
// Suppose that we want to separate files into "old" and "new" subdirectories
// on the basis of modification time.  We will provide paths representing these
// locations, and create the directories if they do not exist:
//..
    bsl::string oldPath(logPath), newPath(logPath);
    bdlsu::PathUtil::appendRaw(&oldPath, "old");
    bdlsu::PathUtil::appendRaw(&newPath, "new");
    int rc = bdlsu::FileUtil::createDirectories(oldPath.c_str(), true);
    ASSERT(0 == rc);
    rc = bdlsu::FileUtil::createDirectories(newPath.c_str(), true);
    ASSERT(0 == rc);
//..
// We know that all of our log files match the pattern "*.log", so let's search
// for all such files in the log directory:
//..
    bdlsu::PathUtil::appendRaw(&logPath, "*.log");
    bsl::vector<bsl::string> logFiles;
    bdlsu::FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
//..
// Now for each of these files, we will get the modification time.  Files that
// are older than 2 days will be moved to "old", and the rest will be moved to
// "new":
//..
    bdlt::Datetime modTime;
    bsl::string   fileName;
    for (bsl::vector<bsl::string>::iterator it = logFiles.begin();
                                                 it != logFiles.end(); ++it) {
      rc = bdlsu::FileUtil::getLastModificationTime(&modTime, *it);
      ASSERT(0 == rc);

      rc = bdlsu::PathUtil::getLeaf(&fileName, *it);
      ASSERT(0 == rc);

      bsl::string *whichDirectory =
                 2 < (bdlt::CurrentTime::utc() - modTime).totalDays()
                  ? &oldPath
                  : &newPath;
      bdlsu::PathUtil::appendRaw(whichDirectory, fileName.c_str());
      ASSERT(0 == bdlsu::FileUtil::move(it->c_str(), whichDirectory->c_str()));
      bdlsu::PathUtil::popLeaf(whichDirectory);
    }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING: Unix File Permissions for 'open'
        //
        // Concerns:
        //: 1 The permissions of a file created with 'open' on unix are chmod
        //:   0666.  Although not (currently) contractually guaranteed, this
        //:   matches the behavior for std::fstream and is consistent with the
        //:   use of a umask (see DRQS 40563234).
        //
        // Plan:
        //: 1 Open a file, write some data to it, and close it.
        //: 2 Read its permissions via 'stat64' or 'stat'.
        //: 3 Observe that the permission are chmod 0666 (C-1).
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: Unix File Permissions for 'open'\n"
                             "=========================================\n";

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose) cout << "TEST SKIPPED ON WINDOWS\n";
#else
        umask(0);

        if (verbose) cout << "Testing 'open'\n";
        {
            typedef bdlsu::FileUtil::FileDescriptor FD;

            const bsl::string& testFile = ::tempFileName(test, "tmp.open.txt");

            if (veryVerbose) P(testFile);

            (void) bdlsu::FileUtil::remove(testFile, false);

            FD fd = Obj::open(testFile, true, false);
            ASSERT(Obj::INVALID_FD != fd);

            const char *str = "To be or not to be\n";
            const int len   = static_cast<int>(bsl::strlen(str));
            ASSERT(len == Obj::write(fd, str, len));

            ASSERT(0 == Obj::close(fd));

# ifdef BSLS_PLATFORM_OS_CYGWIN
            struct stat info;
            ASSERT(0 == ::stat(  testFile.c_str(), &info));
# else
            struct stat64 info;
            ASSERT(0 == ::stat64(testFile.c_str(), &info));
# endif
            info.st_mode &= 0777;
            const bool eq =
                          (S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH)
                                                               == info.st_mode;
            if (veryVerbose || !eq) {
                bsl::ios_base::fmtflags flags = cout.flags();
                cout << bsl::oct;
                P_((S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH));
                P(info.st_mode);
                cout.flags(flags);
            }
            ASSERT(eq);
        }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: sync
        //
        // Note that this is a white-box test that aims to verify the
        // underlying system call is called with the appropriate arguments (it
        // is not a test of the operating system behavior).
        //
        // Unfortunately, I been unable to find an effective test for
        // concerns  1, 2, and 3, since I've been unable to observe memory
        // pages *not* synchronized to disk.
        //
        // Concerns:
        //: 1 On success the mapped bytes are synchronized with their values
        //:   in the file.
        //:
        //: 2 That only the region of memory at the specified location
        //:   is synchronized.
        //:
        //: 3 That only the indicated number of bytes are synchronized.
        //:
        //: 4 That on failure an error status is returned.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        //
        //:Plan:
        //: 1 Call 'sync' with valid arguments and verify it returns
        //:   successfully. (C-1..3)
        //:
        //: 2 Call 'sync' with an invalid set of arguments (having disabled
        //:   assertions that would prevent the arguments being supplied to the
        //:   underlying system call)  (C-4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-5)
        //
        // Testing:
        //   static int sync(char *, int , bool );
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'sync'\n"
                             "===============\n";

        typedef bdlsu::FileUtil::FileDescriptor FD;

        // Note that there appear to be '#define' for PAGESIZE and PAGE_SIZE
        // on AIX.

        const int MYPAGESIZE = bdlsu::MemoryUtil::pageSize();
        const int SIZE       = MYPAGESIZE;
        const int READ       = bdlsu::MemoryUtil::BDESU_ACCESS_READ;
        const int READ_WRITE = bdlsu::MemoryUtil::BDESU_ACCESS_READ |
                               bdlsu::MemoryUtil::BDESU_ACCESS_WRITE;
        int         rc     = 0;
        Obj::Offset offset = 0;

        bsl::string testFileName = ::tempFileName(test);
        Obj::remove(testFileName);
        FD writeFd = Obj::open(testFileName, true, false, false);
        FD readFd  = Obj::open(testFileName, false, true, false);

        ASSERT(Obj::INVALID_FD != writeFd);
        ASSERT(Obj::INVALID_FD != readFd);

        offset = Obj::seek(writeFd, SIZE, Obj::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(SIZE == offset);
        rc = Obj::write(writeFd, testFileName.c_str(), 1);
        ASSERT(1 == rc);

        offset = Obj::seek(writeFd, 0, Obj::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(0 == offset);

        void *writeMemory, *readMemory;

        rc = Obj::map(writeFd, &writeMemory, 0, SIZE, READ_WRITE);
        ASSERT(0 == rc);

        rc = Obj::map(readFd,   &readMemory, 0, SIZE, READ);
        ASSERT(0 == rc);

        ASSERT(readFd != writeFd);

        char *writeBuffer = static_cast<char *>(writeMemory);

        {

            if (veryVerbose) {
                cout << "\tTesting msync is performed" << endl;
            }

            rc = Obj::sync(writeBuffer, SIZE, true);
            ASSERT(0 == rc);

            // I have not been able to fashion an effective test for 'sync'
            // because I've been unable to observe unsynchronized memory
            // mapped pages (so it cannot be determined whether 'sync' is
            // actually performing synchronization).  For reference, you can
            // find some experiments writing to mapped-memory, and read from a
            // different file descriptor to the same file, in
            // 'devgit:bde/bde-core' commit:
            //..
            //  commit a93a90d9c567d7a24994811f79c65b38c2cb9791
            //  Author: (Henry) Mike Verschell <hverschell@bloomberg.net>
            //  Date:   Fri Apr 19 16:28:50 2013 -0400
            //..
        }
        {
            if (veryVerbose) {
                cout << "\tTesting msync returns an error status" << endl;
            }

            // Note that, experimentally, the only sane way to force an error
            // code from sync is to pass a address that is not aligned on a
            // page boundary.  We must first disable our own assertion handler
            // in order for the underlying system call to be invoked.

            bsls::AssertFailureHandlerGuard hg(NoOpAssertHandler);

            int address;

            rc = Obj::sync((char *)&address, MYPAGESIZE, true);
            ASSERT(0 != rc);
#ifdef BSLS_PLATFORM_OS_UNIX
            // Note that this is a white-box test that we return 'errno' on
            // error, which is not required by the method contract.
            ASSERT(EINVAL == rc);
            if (veryVeryVerbose) {
                P(rc);
            }
#endif
        }
        {
            bsls::AssertFailureHandlerGuard hG(
                                            bsls::AssertTest::failTestDriver);
            if (veryVerbose) cout << "\tTest assertions." << endl;

            ASSERT_PASS(Obj::sync(writeBuffer, SIZE, true));
            ASSERT_FAIL(Obj::sync(0, SIZE, true));
            ASSERT_FAIL(Obj::sync(writeBuffer, SIZE / 2, true));
            ASSERT_FAIL(Obj::sync(writeBuffer + 1, SIZE, true));

        }
        Obj::close(writeFd);
        Obj::close(readFd);

        rc = Obj::unmap(writeMemory, SIZE);
        ASSERT(0 == rc);

        rc = Obj::unmap( readMemory, SIZE);
        ASSERT(0 == rc);

        rc = Obj::remove(testFileName);
        ASSERT(0 == rc);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // Append test
        //
        // Concerns:
        //   On Unix, if a file is opened for append, all writes to that file,
        //   event following an lseek or a write from another process, are to
        //   append to the end of the file.  The MSDN doc specifically says
        //   that the doc's author doesn't know what will happen, so this test
        //   is to verify whatever the behavior is.
        // --------------------------------------------------------------------

        typedef bdlsu::FileUtil::FileDescriptor FD;

        const char *testFile = "tmp.12.append.txt.";
        const char *tag1     = "tmp.12.tag.1.txt";
        const char *success  = "tmp.12.success.txt";

        const char testString[] = { "123456789" };

        enum { SZ10 = sizeof(testString) };  // satisfy pedants who won't
        ASSERT(10 == SZ10);                  // tolerate a magic number

        Obj::Offset fs;                      // file size cache variable

        bool isParent = !verbose || bsl::string(argv[2]) != "child";
        if (isParent)  {
            if (verbose) cout << "APPEND TEST -- PARENT\n"
                                 "=====================\n";

            Obj::remove(testFile);
            Obj::remove(tag1);
            Obj::remove(success);

            // First, test with lseek on one file desc

            FD fd = Obj::open(testFile, true, false, true); // append mode
            ASSERT(Obj::INVALID_FD != fd);

            int rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            Obj::Offset off =Obj::seek(fd, 0, Obj::BDESU_SEEK_FROM_BEGINNING);
            ASSERT(0 == off);

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 2 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::BDESU_SEEK_FROM_CURRENT);
            ASSERT(2 * SZ10 == off);

            // Next, see what happens when another file desc from the same
            // process writes to the end of the file.

            FD fd2 = Obj::open(testFile, true, true);
            ASSERT(Obj::INVALID_FD != fd2);

            off = Obj::seek(fd2, 0, Obj::BDESU_SEEK_FROM_END);
            LOOP_ASSERT(off, 2 * SZ10 == off);

            rc = Obj::write(fd2, testString, SZ10);
            ASSERT(SZ10 == rc);
            LOOP_ASSERT(fs, 3 * SZ10 == (fs = Obj::getFileSize(testFile)));

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 4 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::BDESU_SEEK_FROM_CURRENT);
            LOOP_ASSERT(off, 4 * SZ10 == off);

            bsl::stringstream cmd;
            cmd << absoluteTaskPath << ' ' << argv[1] << " child";
            cmd << (verbose     ? " v" : "");
            cmd << (veryVerbose ? " v" : "");

            ::localForkExec(cmd.str().c_str());

            while (!Obj::exists(tag1)) {
                if (veryVerbose) Q(Parent sleeping);
                ::localSleep(1);
            }
            if (verbose) Q(Parent detected tag1);

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 6 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::BDESU_SEEK_FROM_CURRENT);
            LOOP_ASSERT(off, 6 * SZ10 == off);

            ASSERT(Obj::exists(success));

            Obj::close(fd);
            Obj::close(fd2);

            if (verbose) Q(Parent finished);

#ifdef BSLS_PLATFORM_OS_UNIX
            wait(0);
#endif
        }
        else {
            // child process

            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = false;

            if (verbose) cout << "APPEND TEST -- CHILD\n"
                                 "====================\n";

            ASSERT(0 == bdlsu::FileUtil::setWorkingDirectory(origWorkingDir));

            ASSERT(bdlsu::FileUtil::exists(testFile));
            LOOP_ASSERT(fs, 4 * SZ10 == (fs = Obj::getFileSize(testFile)));

            FD fdChild = Obj::open(testFile, true, true);
            ASSERT(Obj::INVALID_FD != fdChild);

            Obj::Offset off = Obj::seek(fdChild, 0, Obj::BDESU_SEEK_FROM_END);
            LOOP_ASSERT(off, 4 * SZ10 == off);

            int rc = Obj::write(fdChild, testString, SZ10);
            ASSERT(SZ10 == rc);

            if (0 == testStatus) {
                ::localTouch(success);
            }

            ::localTouch(tag1);

            Obj::close(fdChild);

            if (verbose) Q(Child finished);

            // Quit to avoid doing cleanup at end of 'main' twice.

            return testStatus;                                        // RETURN
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TRYLOCK / UNLOCK TEST
        //
        // Concerns:
        //   That 'tryLock' and 'unlock' succeed when expected.  'tryLock' and
        //   'unlock' just call the equivalent functions in
        //   'bdlsu::FilesystemUtil', which are thoroughly tested in that
        //   component's test driver.
        //
        // Plan:
        //   Open a file, lock it, and unlock it, observing that '0 == rc'.
        // --------------------------------------------------------------------

        typedef bdlsu::FileUtil::FileDescriptor FD;
        int rc;

        bsl::string fileNameTest = "bdlsu_xxxfileutil.10.txt";

        bdlsu::FileUtil::remove(fileNameTest);

        FD fd = bdlsu::FileUtil::open(fileNameTest,
                                     true,    // write able
                                     false);  // file doesn't pre-exist

        rc = bdlsu::FileUtil::write(fd, "woof", 4);
        ASSERT(4 == rc);

        rc = bdlsu::FileUtil::tryLock(fd, true);
        ASSERT(0 == rc);

        rc = bdlsu::FileUtil::unlock(fd);
        ASSERT(0 == rc);

        rc = bdlsu::FileUtil::close(fd);
        ASSERT(0 == rc);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GETFILESIZELIMIT TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "getFileSizeLimit test\n"
                             "=====================\n";

        bdlsu::FileUtil::Offset limit = bdlsu::FileUtil::getFileSizeLimit();

        ASSERT(limit > 0);
        ASSERT(limit > (1LL << 32));

        if (verbose) P(limit);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // APPEND TEST
        //
        // Concerns:
        //  1. A 'write' puts data at the end of the file when open in append
        //     mode.
        //  2. A 'write' puts data at the end of the file when open in append
        //     mode even after a seek.
        //  3. 'isAppend' is default to 'false'.
        //
        // Plan:
        //  1. Create a file in append mode, write a character, use seek to
        //     change the position of output, write another character, and
        //     verify that the new character is added after the original
        //     character.
        //  2. Reopen the file in append mode, write a character and ensure
        //     that it is added to the end of the file.
        //  3. Reopen the file in normal mode, write a character and ensure
        //     that it overwrites the data in the file instead of appending to
        //     it.
        //
        // Testing:
        //   FD open(const char *p, bool writable, bool exist, bool append);
        // --------------------------------------------------------------------

        if (verbose) cout << "APPEND TEST -- SINGLE PROCESS\n"
                             "=============================\n";

        bsl::string fileName(::tempFileName(test));

        if (verbose) { P(fileName) }

        bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(
                                                  fileName, true, false, true);
        ASSERT(bdlsu::FileUtil::INVALID_FD != fd);

        bdlsu::FileUtil::write(fd, "A", 1);
        char result[16];

        bdlsu::FileUtil::seek(fd, 0, bdlsu::FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(1 == bdlsu::FileUtil::read(fd, result, sizeof result));

        bdlsu::FileUtil::seek(fd, 0, bdlsu::FileUtil::BDESU_SEEK_FROM_BEGINNING);
        bdlsu::FileUtil::write(fd, "B", 1);

        bdlsu::FileUtil::seek(fd, 0, bdlsu::FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(2 == bdlsu::FileUtil::read(fd, result, sizeof result));

        bdlsu::FileUtil::close(fd);

        fd = bdlsu::FileUtil::open(fileName, true, true, true);
        bdlsu::FileUtil::write(fd, "C", 1);
        bdlsu::FileUtil::seek(fd, 0, bdlsu::FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(3 == bdlsu::FileUtil::read(fd, result, sizeof result));

        bdlsu::FileUtil::close(fd);

        fd = bdlsu::FileUtil::open(fileName, true, true);
        bdlsu::FileUtil::write(fd, "D", 1);
        bdlsu::FileUtil::close(fd);

        fd = bdlsu::FileUtil::open(fileName, false, true);
        ASSERT(3 == bdlsu::FileUtil::read(fd, result, sizeof result));
        bdlsu::FileUtil::close(fd);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SIMPLE MATCHING TEST
        //
        // Concerns:
        //   Unix "glob()", which is called by bdlsu::FileUtil::visitPaths, is
        //   failing on ibm 64 bit, unfortunately the test driver has not
        //   detected or reproduced this error.  This test case is an attempt
        //   to get this test driver reproducing the problem.
        //
        // Plan:
        //   Run the usage example 1
        // --------------------------------------------------------------------

        if (verbose) cout << "Simple matching test\n"
                             "====================\n";

        for (int i=0; i<4; ++i) {
            char name[16];
            sprintf(name, "woof.a.%d", i);
            bdlsu::FileUtil::FileDescriptor fd =
                bdlsu::FileUtil::open(name, true, false);
            bdlsu::FileUtil::close(fd);
        }

        vector<string> vs;
        bdlsu::FileUtil::findMatchingPaths(&vs, "woof.a.?");
        sort(vs.begin(), vs.end());

        ASSERT(vs.size() == 4);
        ASSERT(vs[0] == "woof.a.0");
        ASSERT(vs[1] == "woof.a.1");
        ASSERT(vs[2] == "woof.a.2");
        ASSERT(vs[3] == "woof.a.3");
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getFileSize'
        //
        // Concern: Returns proper file size for the following:
        //   1. A normal file.
        //   2. A normal directory (use empty directory).
        //   3. A file using relative path.
        //   4. A symbolic link (unix only).
        //   5. Non existent file.
        //
        // Plan:
        //   Create the respective files listed in concerns and run
        //   'getFileSize' on it.
        //
        // Testing:
        //   static Offset getFileSize(const bsl::string&);
        //   static Offset getFileSize(const char *);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'getFileSize'\n"
                             "=====================\n";

        // Setup by first creating a tmp file
        string fileName = ::tempFileName(test, "tmp.6.getFileSizeTest");
        if (veryVerbose) P(fileName);
        bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(fileName,
                                                                 true,
                                                                 false);
        ASSERT(0 != fd);

        const char buffer[] = "testing";
        int bytes           = sizeof buffer;

        bdlsu::FileUtil::write(fd, buffer, bytes);
        bdlsu::FileUtil::close(fd);

        // Concern 1

        {
            if (veryVerbose) cout << "\n1. Normal file" << endl;

            bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize(fileName);
            LOOP2_ASSERT(bytes, off, bytes == off);

            bdlsu::FileUtil::Offset off2 = bdlsu::FileUtil::getFileSize(
                                                             fileName.c_str());
            LOOP2_ASSERT(bytes, off2, bytes == off2);

            if (veryVerbose) {
                cout << "Expected " << bytes << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }
        }

        // Concern 2

        {
            if (veryVerbose) cout << "\n2. Normal directory" << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
            string dirName("getFileSizeDir");

            // windows directories are 0 sized

            const bdlsu::FileUtil::Offset EXPECTED = 0;
#else
            string dirName("/tmp/getFileSizeDir");
#endif

            int ret = bdlsu::FileUtil::createDirectories(dirName, true);
            ASSERT(0 == ret);

            // On UNIX use 'stat64' ('stat' on cygwin) as an oracle: the file
            // size of a directory depends on the file system.

#ifdef BSLS_PLATFORM_OS_CYGWIN
            struct stat oracleInfo;
            int rc = ::stat(dirName.c_str(), &oracleInfo);
            ASSERT(0 == rc);

            bdlsu::FileUtil::Offset EXPECTED = oracleInfo.st_size;
#elif !defined BSLS_PLATFORM_OS_WINDOWS
            struct stat64 oracleInfo;
            int rc = ::stat64(dirName.c_str(), &oracleInfo);
            ASSERT(0 == rc);

            bdlsu::FileUtil::Offset EXPECTED = oracleInfo.st_size;
#endif

            bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize(dirName);
            LOOP2_ASSERT(EXPECTED, off, EXPECTED == off);

            bdlsu::FileUtil::Offset off2 = bdlsu::FileUtil::getFileSize(
                                                             dirName.c_str());
            LOOP2_ASSERT(EXPECTED, off2, EXPECTED == off2);

            if (veryVerbose) {
                cout << "Expected " << EXPECTED << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdlsu::FileUtil::remove(dirName);
        }

        // Concern 3

        {
            if (veryVerbose) cout << "\n4. Relative Path" << endl;

            string fileName("../getFileSizeTest.txt");
            bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(fileName,
                                                                     true,
                                                                     false);
            ASSERT(0 != fd);

            const char buffer[] = "testing";
            int bytes           = sizeof buffer;

            bdlsu::FileUtil::write(fd, buffer, bytes);
            bdlsu::FileUtil::close(fd);

            bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize(fileName);
            ASSERT(bytes == off);

            bdlsu::FileUtil::Offset off2 = bdlsu::FileUtil::getFileSize(
                                                             fileName.c_str());
            ASSERT(bytes == off2);

            if (veryVerbose) {
                cout << "Expected " << bytes << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdlsu::FileUtil::remove(fileName);
        }

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // Concern 4
        // No symbolic links on windows.

        {
            if (veryVerbose) cout << "\n5. Symbolic Links" << endl;

            bsl::string cmd = "ln -s " + fileName + " testLink";
            system(cmd.c_str());

            string fileName("testLink");
            bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize(fileName);
            ASSERT(bytes == off);

            bdlsu::FileUtil::Offset off2 = bdlsu::FileUtil::getFileSize(
                                                             fileName.c_str());
            ASSERT(bytes == off2);

            if (veryVerbose) {
                cout << "Expected " << bytes << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdlsu::FileUtil::remove(fileName);
        }
#endif

        // Concert 5

        {
            if (veryVerbose) cout << "\n6. Non existent file" << endl;

            // Use a random name.

            bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize("acasdf");

            ASSERT(-1 == off);
            if (veryVerbose) {
                cout << "Expected -1" << endl;
                cout << "Actual " << off << endl;
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getAvailableSpace'
        //
        // Concern: 'getAvailableSpace' works.
        //
        // Plan:
        //   Just call the function and check it returns a non-negative result.
        //   We cannot verify it properly.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'getAvailableSpace'\n"
                             "===========================\n";

        bdlsu::FileUtil::Offset avail = bdlsu::FileUtil::getAvailableSpace(".");
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERT(0 <= avail);

        string fileName = ::tempFileName(test);
        bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(
                                                        fileName, true, false);
        ASSERT(bdlsu::FileUtil::INVALID_FD != fd);

        avail = bdlsu::FileUtil::getAvailableSpace(fd);
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERT(0 <= avail);

        bdlsu::FileUtil::close(fd);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'rollFileChain'
        //
        // Concern: Files are rolled.
        //
        // Plan:
        //   Create tmpFile and roll until .1-.3 exist, and verify that they
        //   contain 0-2 (3 rolled off the end).
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'rollFileChain' (files)\n"
                             "===============================\n";

        enum { MAXSUFFIX=3 };
        bsl::string tmpFile = ::tempFileName(test);
        bdlsu::FileUtil::FileDescriptor f;

        ASSERT(0 == bdlsu::FileUtil::rollFileChain(tmpFile, MAXSUFFIX));

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            f = bdlsu::FileUtil::open(tmpFile, true, false);
            LOOP_ASSERT(tmpFile, f != bdlsu::FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdlsu::FileUtil::write(f, &i, sizeof(int)));
            ASSERT(0 == bdlsu::FileUtil::close(f));

            // Roll the file(s).

            ASSERT(0 == bdlsu::FileUtil::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != bdlsu::FileUtil::remove(tmpFile)); // does not exist
        tmpFile += ".0";
        int pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = bdlsu::FileUtil::open(tmpFile, false, true); // must exist
            LOOP_ASSERT(tmpFile, f != bdlsu::FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdlsu::FileUtil::read(
                                                      f, &value, sizeof(int)));
            ASSERT(0 == bdlsu::FileUtil::close(f));
            ASSERT(0 == bdlsu::FileUtil::remove(tmpFile));
            LOOP2_ASSERT(i, value, i == value);
        }

        if (verbose) {
           cout << "rollFileChain test (directories)" << endl;
        }
        tmpFile = "tmpDir";

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            ASSERT(0 == bdlsu::FileUtil::createDirectories(tmpFile, true));
            f = bdlsu::FileUtil::open(tmpFile + PS + "file", true, false);
            ASSERT(f != bdlsu::FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdlsu::FileUtil::write(f, &i, sizeof(int)));
            ASSERT(0 == bdlsu::FileUtil::close(f));

            // Roll the file(s).

            ASSERT(0 == bdlsu::FileUtil::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != bdlsu::FileUtil::remove(tmpFile, true)); // does not exist
        tmpFile += ".0";
        pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = bdlsu::FileUtil::open(tmpFile + PS + "file", false, true);
            ASSERT(f != bdlsu::FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdlsu::FileUtil::read(
                                                      f, &value, sizeof(int)));
            ASSERT(0 == bdlsu::FileUtil::close(f));
            ASSERT(0 == bdlsu::FileUtil::remove(tmpFile, true));
            LOOP2_ASSERT(i, value, i == value);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'isRegularFile' & 'isDirectory'
        //
        // Concern: These functions work, including on filenames which do not
        //          exist at all.
        //
        // Plan:
        //   Permutate a test vector with a list of different files and
        //   directories to test.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'isRegularFile' & 'isDirectory'\n"
                             "=======================================\n";

        struct Parameters {
            const char* good;
            const char* badNoExist;
            const char* badWrongType;
        };

        struct ParametersByType {
            Parameters regular;
            Parameters directory;
        } parameters = {
            { "tmp.case3" PS "file",
              "tmp.case3" PS "file2",
              "tmp.case3" PS "dir"  },
            { "tmp.case3" PS "dir",
              "tmp.case3" PS "dir2",
              "tmp.case3" PS "file" }
        };

        const Parameters& r = parameters.regular;
        const Parameters& d = parameters.directory;

        ASSERT(0 == bdlsu::FileUtil::createDirectories(r.good));

        makeArbitraryFile(r.good);
        ASSERT(0 == bdlsu::FileUtil::createDirectories(r.badWrongType, true));
        ASSERT(true == bdlsu::FileUtil::isRegularFile(r.good));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(r.badNoExist));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(r.badWrongType));

        makeArbitraryFile(d.badWrongType);
        ASSERT(0 == bdlsu::FileUtil::createDirectories(d.good, true));
        ASSERT(true == bdlsu::FileUtil::isDirectory(d.good));
        ASSERT(false == bdlsu::FileUtil::isDirectory(d.badNoExist));
        ASSERT(false == bdlsu::FileUtil::isDirectory(d.badWrongType));

#ifndef BSLS_PLATFORM_OS_WINDOWS
        if (veryVerbose) {
           cout << "...symbolic link tests..." << endl;
        }

        bsl::string absolute;
        ASSERT(0 == bdlsu::FileUtil::getWorkingDirectory(&absolute));
        bdlsu::PathUtil::appendRaw(&absolute, r.good);

        bsl::string link = absolute;
        bdlsu::PathUtil::popLeaf(&link);
        bdlsu::PathUtil::appendRaw(&link, "link_rg");
        int rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isRegularFile(link.c_str()));
        ASSERT(true  == bdlsu::FileUtil::isRegularFile(link.c_str(), true));

        bsl::string link2 = r.good;
        bdlsu::PathUtil::popLeaf(&link2);
        bdlsu::PathUtil::appendRaw(&link2, "link_rg2");
        rc = symlink(link.c_str(), link2.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isRegularFile(link2));
        ASSERT(true  == bdlsu::FileUtil::isRegularFile(link2, true));

        bdlsu::PathUtil::popLeaf(&link);
        bdlsu::PathUtil::appendRaw(&link, "link_rbw");
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::appendRaw(&absolute, r.badWrongType);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isRegularFile(link));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link, true));

        bdlsu::PathUtil::popLeaf(&link);
        bdlsu::PathUtil::appendRaw(&link, "link_rbn");
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::appendRaw(&absolute, r.badNoExist);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isRegularFile(link));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link, true));

        bdlsu::PathUtil::popLeaf(&link);
        bdlsu::PathUtil::appendRaw(&link, "link_dg");
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::popLeaf(&absolute);
        bdlsu::PathUtil::appendRaw(&absolute, d.good);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isDirectory(link));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link));
        ASSERT(true  == bdlsu::FileUtil::isDirectory(link, true));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link, true));

        bdlsu::PathUtil::popLeaf(&link2);
        bdlsu::PathUtil::appendRaw(&link2, "link_dg2");
        rc = symlink(link.c_str(), link2.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdlsu::FileUtil::isDirectory(link2));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link2));
        ASSERT(true  == bdlsu::FileUtil::isDirectory(link2, true));
        ASSERT(false == bdlsu::FileUtil::isRegularFile(link2, true));

#endif  // Symbolic link testing on non-Windows

#ifndef BSLS_PLATFORM_OS_WINDOWS  // (unix domain socket)
        {
            // Unix domain sockets should return 'false' for 'isRegularFile'
            // and 'isDirectory' (DRQS 2071065).

            if (veryVerbose) {
                cout << "...unix domain socket..." << endl;
            }
            bsl::string filename = ::tempFileName(test);
            bdlsu::FileUtil::remove(filename);

            int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
            LOOP_ASSERT(socketFd, socketFd >= 0);

            struct sockaddr_un address;
            address.sun_family = AF_UNIX;
            sprintf(address.sun_path, "%s", filename.c_str());

            // Add one to account for the null terminator for the filename.

            const int ADDR_LEN = (int) (sizeof(address.sun_family) +
                                        filename.size() +
                                        1);

            int rc = bind(socketFd, (struct sockaddr *)&address, ADDR_LEN);
            LOOP3_ASSERT(rc, errno, strerror(errno), 0 == rc);


            LOOP_ASSERT(filename, bdlsu::FileUtil::exists(filename));
            LOOP_ASSERT(filename, !bdlsu::FileUtil::isDirectory(filename));
            LOOP_ASSERT(filename, !bdlsu::FileUtil::isRegularFile(filename));
            bdlsu::FileUtil::remove(filename);
        }
#endif  // BSLS_PLATFORM_OS_WINDOWS (unix domain socket)
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING pattern matching
        //
        // Concern: Both '*' and '?' characters are supported, and
        //          can appear in multiple directories in the path.
        //
        // Plan:
        //   Make sure both '*' and '?' characters are supported with
        //   'findMatchingPath'
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing pattern matching\n"
                             "========================\n";

        const char* filenames[] = {
           "abcd",
           "zyx",
           "zy.z",
           "abc.zzz",
           "abc.def",
        };

#define PATH  "tmp.2.futc3"
#define PATHQ "tmp.2.futc?"

        bsl::string path(PATH);

        ASSERT(bdlsu::FileUtil::remove(path.c_str(), true));

        // The string literal "futc3/b???/*d*" seems to confuse the
        // Sun compiler, which complains about the character sequence "\*".
        // So let's hard-code it.

        const char tripleQMarkLiteral[] = {'t','m','p','.','2',
                                           '.','f','u','t','c','3','/','b',
                                           '?','?','?','/','*','d','*', 0};

        struct Parameters {
            int         line;
            const char* pattern;
            const char* result;
        } parameters[] = {
            {L_, "", ""},
            {L_, PATH "/*/*foo*", ""},
            {L_, PATH "/*/*d*", PATH "/alpha/abc.def:" PATH "/alpha/abcd:"
                                PATH "/beta/abc.def:" PATH "/beta/abcd"},
            {L_, tripleQMarkLiteral, PATH "/beta/abc.def:" PATH "/beta/abcd"},
            {L_, PATH "/*b*", PATH "/beta"},
#ifdef BSLS_PLATFORM_OS_WINDOWS
            {L_, PATH "/*b*/*.?",
                      PATH "/beta/abcd:" PATH "/beta/zy.z:" PATH "/beta/zyx" },
            {L_, PATHQ "/*b*/*.?",
                       PATH "/beta/abcd:" PATH "/beta/zy.z:" PATH "/beta/zyx"},
            {L_, PATHQ "/*/abcd.*",PATH "/alpha/abcd:" PATH "/beta/abcd"},
            {L_, PATHQ "/*b*/*.*", PATH "/beta/abc.def:" PATH "/beta/abc.zzz:"
                                   PATH "/beta/abcd:" PATH "/beta/zy.z:"
                                   PATH "/beta/zyx"},
            {L_, PATH "*/*/*.?",
               PATH "/alpha/abcd:" PATH "/alpha/zy.z:" PATH "/alpha/zyx:"
               PATH "/beta/abcd:" PATH "/beta/zy.z:" PATH "/beta/zyx"}
#else
            {L_, PATH "/*b*/*.?", PATH "/beta/zy.z"},
            {L_, PATHQ "/*b*/*.?", PATH "/beta/zy.z"},
            {L_, PATHQ "/*/abcd.*", ""},
            {L_, PATHQ "/*b*/*.*",
                   PATH "/beta/abc.def:" PATH "/beta/abc.zzz:"
                   PATH "/beta/zy.z"},
            {L_, PATH "*/*/*.?", PATH "/alpha/zy.z:" PATH "/beta/zy.z"}
#endif
        };

        const int numFiles = sizeof(filenames) / sizeof(*filenames);

        bdlsu::PathUtil::appendRaw(&path, "alpha");

        if (veryVerbose) { T_() cout << "Creating directory "; P(path) }

        ASSERT(0 == bdlsu::FileUtil::createDirectories(path.c_str(), true));
        for (int i = 0; i < numFiles; ++i) {
            bdlsu::PathUtil::appendRaw(&path, filenames[i]);

            if (veryVerbose) { T_() T_() cout << "Creating file "; P(path) }

            bdlsu::FileUtil::FileDescriptor f =  bdlsu::FileUtil::open(
                                                            path, true, false);
            ASSERT(bdlsu::FileUtil::INVALID_FD != f);
            ASSERT(0 == bdlsu::FileUtil::close(f));

            if (veryVerbose) { T_() T_() cout << "Looking up file "; P(path) }

            vector<bsl::string> lookup;
            bdlsu::FileUtil::findMatchingPaths(&lookup, path.c_str());
            string rollup = ::rollupPaths(lookup);
#ifdef BSLS_PLATFORM_OS_WINDOWS
            replace_if(rollup.begin(), rollup.end(), ::isForwardSlash, *PS);
#endif
            LOOP2_ASSERT(path, rollup, path == rollup);

            bdlsu::PathUtil::popLeaf(&path);
        }
        bdlsu::PathUtil::popLeaf(&path);

        bdlsu::PathUtil::appendRaw(&path, "beta");

        if (veryVerbose) { T_() cout << "Creating directory "; P(path) }

        ASSERT(0 == bdlsu::FileUtil::createDirectories(path.c_str(), true));
        for (int i = 0; i < numFiles; ++i) {
            bdlsu::PathUtil::appendRaw(&path, filenames[i]);

            if (veryVerbose) { T_() T_() cout << "Creating file "; P(path) }

            bdlsu::FileUtil::FileDescriptor f = bdlsu::FileUtil::open(
                                                            path, true, false);
            ASSERT(bdlsu::FileUtil::INVALID_FD != f);
            ASSERT(0 == bdlsu::FileUtil::close(f));

            if (veryVerbose) { T_() T_() cout << "Looking up "; P(path) }

            vector<bsl::string> lookup;
            bdlsu::FileUtil::findMatchingPaths(&lookup, path.c_str());
            string rollup = ::rollupPaths(lookup);
#ifdef BSLS_PLATFORM_OS_WINDOWS
            replace_if(rollup.begin(), rollup.end(), ::isForwardSlash, *PS);
#endif
            LOOP2_ASSERT(path, rollup, path == rollup);

            bdlsu::PathUtil::popLeaf(&path);
        }
        bdlsu::PathUtil::popLeaf(&path);

        vector<bsl::string> resultPaths;
        enum { NUM_PARAMETERS = sizeof(parameters) / sizeof(*parameters) };
        for (int i = 0; i < NUM_PARAMETERS; ++i) {
            const Parameters& p = parameters[i];
            const int LINE      =  p.line;

            string pattern(p.pattern);
#ifdef BSLS_PLATFORM_OS_WINDOWS
            replace_if(pattern.begin(), pattern.end(), ::isForwardSlash, *PS);
#endif

            if (veryVerbose) { T_() T_() cout << "Looking up "; P(path) }

            bdlsu::FileUtil::findMatchingPaths(&resultPaths, pattern.c_str());
            string rollup = ::rollupPaths(resultPaths);
            LOOP3_ASSERT(LINE, p.result, rollup, string(p.result) == rollup);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Usage-Example-Like Functionality Test
        //
        // Concern: The *functionality* of the component's usage example
        //          is correct.
        //
        // Reason for existence: We want to ensure that the usage example
        //                       as written will compile and execute safely,
        //                       which makes *validating* it awkward: either
        //                       the usage example bloats with setup,
        //                       validation, and cleanup; or the usage example
        //                       test includes a lot of code not in the usage
        //                       example.
        //
        // Plan: copy the usage example test.  Before running, set up
        //       files in the source directory and, if possible, make some
        //       of them old.  (On POSIX, utime() is available to do this; but
        //       SetFileInformationByHandle(), the equivalent Win32 function,
        //       is only available in Vista and later.)  Then after the usage
        //       example executes, verify the number of files in each directory
        //       matches what's expected.
        //
        //       This will also serve as a basic test of findMatchingPaths(),
        //       open(), and write().
        // --------------------------------------------------------------------

        if (verbose) cout << "Usage Example like Testing\n"
                             "==========================\n";

        bsl::string logPath = "temp2";

        bdlsu::FileUtil::remove(logPath.c_str(), true);
        bdlsu::PathUtil::appendRaw(&logPath, "logs");

        bsl::string oldPath(logPath), newPath(logPath);
        bdlsu::PathUtil::appendRaw(&oldPath, "old");
        bdlsu::PathUtil::appendRaw(&newPath, "new");
        ASSERT(0 == bdlsu::FileUtil::createDirectories(oldPath.c_str(), true));
        ASSERT(0 == bdlsu::FileUtil::createDirectories(newPath.c_str(), true));

        ASSERT(bdlsu::FileUtil::exists(oldPath));
        ASSERT(bdlsu::FileUtil::exists(newPath));

        char filenameBuffer[20];

        // TBD: When SetFileInformationByHandle() is available, then we should
        // write a setModificationTime() method and use it here (see
        // bdlsu_xxxfileutil.h).  Until then, we use utime() on POSIX directly and
        // we do not attempt to "touch" Windows files.

        enum {
            NUM_TOTAL_FILES = 10,
#ifdef BSLS_PLATFORM_OS_WINDOWS
            NUM_OLD_FILES = 0,
#else
            NUM_OLD_FILES = 3,
#endif
            NUM_NEW_FILES = NUM_TOTAL_FILES - NUM_OLD_FILES
        };

        for (int i = 0; i < NUM_TOTAL_FILES; ++i) {
            bool isOld = i < NUM_OLD_FILES;

            int filenameLength = sprintf(filenameBuffer,
                                  "fileutil%02d_%c.log", i, isOld ? 'o' : 'n');

            ASSERT(0 ==
                      bdlsu::PathUtil::appendIfValid(&logPath, filenameBuffer));

            if (bdlsu::FileUtil::exists(logPath.c_str())) {
                ASSERT(0 == bdlsu::FileUtil::remove(logPath.c_str(), true));
            }
            bdlsu::FileUtil::FileDescriptor f =
                bdlsu::FileUtil::open(logPath.c_str(), true, false);
            LOOP_ASSERT(logPath, f != bdlsu::FileUtil::INVALID_FD);

            filenameBuffer[filenameLength++] = '\n';
            filenameBuffer[filenameLength++] = 0;

            ASSERT(filenameLength == bdlsu::FileUtil::write(f,
                                                           filenameBuffer,
                                                           filenameLength));
            ASSERT(0 == bdlsu::FileUtil::close(f));

#ifndef BSLS_PLATFORM_OS_WINDOWS
            bsls::TimeInterval threeDaysAgo = bdlt::CurrentTime::now() -
                bsls::TimeInterval(3 * 24 * 3600, 0);
            if (isOld) {
                struct utimbuf timeInfo;
                timeInfo.actime = timeInfo.modtime =
                    (bsl::time_t)threeDaysAgo.seconds();

                //test invariant:

                ASSERT(0 == utime(logPath.c_str(), &timeInfo));
            }
#endif
            bdlsu::PathUtil::popLeaf(&logPath);
        }

        bdlsu::PathUtil::appendRaw(&logPath, "*.log");
        vector<bsl::string> logFiles;
        bdlsu::FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        bdlsu::PathUtil::popLeaf(&logPath);

        bdlt::Datetime modTime;
        string        fileName;
        bdlt::Datetime nowTime = bdlt::CurrentTime::utc();
        for (vector<bsl::string>::iterator it = logFiles.begin();
                it != logFiles.end(); ++it) {
            ASSERT(0 == bdlsu::FileUtil::getLastModificationTime(&modTime,
                                                                it->c_str()));
            bdlsu::PathUtil::getLeaf(&fileName, *it);
            bsl::string* whichDirectory =
                     2 < (nowTime - modTime).totalDays() ? &oldPath : &newPath;
            bdlsu::PathUtil::appendRaw(whichDirectory, fileName.c_str());
            ASSERT(0 == bdlsu::FileUtil::move(it->c_str(),
                                             whichDirectory->c_str()));
            bdlsu::PathUtil::popLeaf(whichDirectory);
        }

        // Now validate

        bdlsu::PathUtil::appendRaw(&logPath, "*");
        bdlsu::PathUtil::appendRaw(&logPath, "*o*.log");
        bdlsu::FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        ASSERT(NUM_OLD_FILES == logFiles.size());
        bdlsu::PathUtil::popLeaf(&logPath);

        bdlsu::PathUtil::appendRaw(&logPath, "*n*.log");
        bdlsu::FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        ASSERT(NUM_NEW_FILES == logFiles.size());
        bdlsu::PathUtil::popLeaf(&logPath);
        bdlsu::PathUtil::popLeaf(&logPath);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING detection of large files
        //
        // Concern: Whether 'getFileSize' can detect a large file (> 4GB) since
        //          the file size becomes a 64-bit number.
        //
        // Plan: Create a large file in "/tmp" and check the file size.
        //       Remove it afterwards.
        //
        // --------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // Not sure how to create large files on windows, so test only on UNIX.
        // However, this function had been tested on windows by creating the
        // large file through cygwin.

        // Create a 5 GB file.

        if (veryVerbose) cout << "\n3. Large File" << endl;

        system("dd if=/dev/zero of=/tmp/fiveGBFile "
               "bs=1024000 count=5000");

        string fileName("/tmp/fiveGBFile");

        bdlsu::FileUtil::Offset off = bdlsu::FileUtil::getFileSize(fileName);
        ASSERT(5120000000LL == off);

        bdlsu::FileUtil::Offset off2 = bdlsu::FileUtil::getFileSize(
                                                         fileName.c_str());
        ASSERT(5120000000LL == off2);

        if (veryVerbose) {
            cout << "Expected 5120000000LL" << endl;
            cout << "Actual ";
            P_(off) P(off2)
        }

        bdlsu::FileUtil::remove(fileName);
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------

        static const char* foo = "/tmp/blahblah.tmp";
        bdlsu::FileUtil::remove(foo);
        bdlsu::FileUtil::FileDescriptor fd = bdlsu::FileUtil::open(foo, 1, 0);
        int pageSize = bdlsu::MemoryUtil::pageSize();
        bdlsu::FileUtil::grow(fd, pageSize);
        int *p;
        ASSERT(0 == bdlsu::FileUtil::map(fd, (void**)&p, 0, pageSize,
                                   bdlsu::MemoryUtil::BDESU_ACCESS_READ_WRITE));
        printf("mapped at %p\n", p);
        for (int i = 0; i < 10000; ++i) {
          ASSERT(0 == bdlsu::FileUtil::seek(fd, 0,
                                   bdlsu::FileUtil::BDESU_SEEK_FROM_BEGINNING));
          int buf;
          *p = i;
          ASSERT(sizeof(int) == bdlsu::FileUtil::read(fd, &buf, sizeof(int)));
          ASSERT(i == buf);
        }
        ASSERT(0 == bdlsu::FileUtil::unmap(p, pageSize));

#if 0
        bdlsu::FileUtil::FileDescriptor fd =  // /bb/data is Bloomberg-specific
                       bdlsu::FileUtil::open("/bb/data/tmp/blahblah.tmp", 1, 0);
        int pageSize = bdlsu::FileUtil::pageSize();
        int nPages = 1024*1024*1024 / pageSize;
        int rc = bdlsu::FileUtil::grow(fd, nPages * pageSize, argc > 2);
        printf("grow(fd, %d, %d) rc=%d\n",
               nPages * pageSize, (int)(argc>2), rc);
        int fileSize = bdlsu::FileUtil::seek(fd, 0, SEEK_END);
        printf("file size = %d\n", fileSize);
        if (!rc) {
            for(int i=0; i<nPages; i++) {
                bdesu::FileUtilMapping fm =
                         bdlsu::FileUtil::map(fd, i * pageSize, pageSize, true);
                memset(fm.addr(), 2, pageSize);
                bdlsu::FileUtil::unmap(fm, pageSize);
            }
        }
#endif
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // LARGE FILE TEST CASE
        //
        // Concern:
        //   We need a straightforward test case, using writes and reads,
        //   to create and read back a 5G file.
        // --------------------------------------------------------------------

        if (verbose) cout << "SIMPLE 5 GIGABYTE FILE TEST CASE\n"
                             "================================\n";

        typedef bdlsu::FileUtil Util;

#if 1
        const bsls::Types::Int64 fiveGig = 5LL * 1000LL * 1000LL * 1000LL;
        const bsls::Types::Int64 deltaMileStone = 100LL * 1000LL * 1000LL;
#else
        const bsls::Types::Int64 fiveGig = 5 * 1000LL * 1000LL;
        const bsls::Types::Int64 deltaMileStone = 100LL * 1000LL;
#endif

        bsls::Types::Int64 mileStone = deltaMileStone;

        bsls::Types::Int64 bytesWritten = 0;

        char record[80] = "123456789 123456789 123456789 123456789 "
                          "123456789 123456789 123";
        char * const writeTo = record + 63;

        MMIXRand rand;

        LOOP_ASSERT(Util::getFileSizeLimit(),
                                           Util::getFileSizeLimit() > fiveGig);

        const char *fileName = "tmpFiveGig.txt";
        Util::FileDescriptor fd = Util::open(fileName, true, false);
        ASSERT(Util::INVALID_FD != fd);

        for (;;) {
            rand.munge();
            bsl::strcpy(writeTo, rand.display());
            record[79] = '\n';

            int rc = Util::write(fd, record, 80);
            if (80 != rc) {
                ASSERT(0 && "80 != rc");
                break;
            }
            bytesWritten += 80;

            if (bytesWritten >= mileStone) {
                cout << bytesWritten << " written -- last: " <<
                                                        rand.display() << endl;
                if (bytesWritten >= fiveGig) {
                    break;
                }
                mileStone += deltaMileStone;
            }
        }
        ASSERT(fiveGig == bytesWritten);
        ASSERT(Util::seek(fd, 0, Util::BDESU_SEEK_FROM_CURRENT) ==
                                                                 bytesWritten);
        ASSERT(Util::seek(fd, 0, Util::BDESU_SEEK_FROM_END) ==   bytesWritten);

        cout << "Writing done\n";

        if (verbose) P(bytesWritten);

        ASSERT(Util::getFileSize(fileName) == bytesWritten);
        ASSERT(Util::isRegularFile(fileName));

        char inBuf[80];
        bsls::Types::Int64 bytesRead = 0;
        rand.reset();
        mileStone = deltaMileStone;

        ASSERT(0 == Util::seek(fd, 0, Util::BDESU_SEEK_FROM_BEGINNING));

        for (;;) {
            int rc = Util::read(fd, inBuf, 80);
            ASSERT(80 == rc);
            ASSERT(0 == bsl::memcmp(record, inBuf, 63));

            rand.munge();
            ASSERT(0 == bsl::memcmp(inBuf + 63, rand.display(), 16));

            ASSERT('\n' == inBuf[79]);

            bytesRead += 80;

            if (bytesRead >= mileStone) {
                cout << bytesRead << " read -- last: " << rand.display() <<
                                                                          endl;
                if (bytesRead >= fiveGig) {
                    break;
                }
                mileStone += deltaMileStone;
            }
        }
        ASSERT(fiveGig == bytesRead);
        ASSERT(bytesWritten == bytesRead);
        ASSERT(Util::seek(fd, 0, Util::BDESU_SEEK_FROM_CURRENT) == bytesRead);
        ASSERT(Util::seek(fd, 0, Util::BDESU_SEEK_FROM_END)     == bytesRead);

        cout << "Reading done\n";

        ASSERT(0 == Util::close(fd));

        ASSERT(Util::getFileSize(fileName) == fiveGig);

        {
            int rc = Util::remove(fileName);
            ASSERT(0 == rc);
        }
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // TESTING WORKING DIRECTORY CREATED
        // --------------------------------------------------------------------

        return -1;                                                    // RETURN
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == bdlsu::FileUtil::setWorkingDirectory(origWorkingDir));
    LOOP_ASSERT(tmpWorkingDir, bdlsu::FileUtil::exists(tmpWorkingDir));

    // Sometimes this delete won't work because of '.nfs*' gremlin files that
    // mysteriously get created in the directory.  Leave the directory behind
    // and move on.  Also remove twice, because sometimes the first 'remove'
    // 'sorta' fails -- it returns a negative status after successfully killing
    // the gremlin file.  Worst case, leave the file there to be cleaned up
    // in a sweep later.

    bdlsu::FileUtil::remove(tmpWorkingDir, true);
    bdlsu::FileUtil::remove(tmpWorkingDir, true);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
