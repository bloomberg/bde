// bdesu_fileutil.t.cpp -*-C++-*-
#include <bdesu_fileutil.h>

#include <bdesu_pathutil.h>
#include <bdef_bind.h>
#include <bdet_datetime.h>
#include <bdetu_systemtime.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_sstream.h>

#ifndef BSLS_PLATFORM__OS_WINDOWS
#include <errno.h>
#include <utime.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Needed for using 'stat64' on HP
#ifdef BSLS_PLATFORM__OS_HPUX
    #ifndef _LARGEFILE64_SOURCE
        #define _LARGEFILE64_SOURCE 1
    #endif
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#else
#include <windows.h>  // for Sleep, GetLastError
#endif

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_c_stdio.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE 1
// [10] USAGE EXAMPLE 2

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
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
inline
bool isBackslash (char t)
{
   return t == '\\';
}

inline
bool isForwardSlash (char t)
{
   return t == '/';
}

#endif

void localSleep(int seconds)
{
#ifdef BSLS_PLATFORM__OS_UNIX
    sleep(seconds);
#else
    ::Sleep(seconds * 1000);
#endif
}

int localErrval()
{
#ifdef BSLS_PLATFORM__OS_UNIX
    return errno;
#else
    return GetLastError();
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

#ifdef BSLS_PLATFORM__OS_WINDOWS
   replace_if(result.begin(), result.end(), isBackslash, '/');
#endif
   return result;
}

inline
void makeArbitraryFile(const char *path)
{
    bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(path,1,0);
    ASSERT(bdesu_FileUtil::INVALID_FD != fd);
    ASSERT(5 == bdesu_FileUtil::write(fd, "hello", 5));
    ASSERT(0 == bdesu_FileUtil::close(fd));
}

inline
bsl::string tempFileName()
{
    bsl::string result;
#ifdef BSLS_PLATFORM__OS_WINDOWS
    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "bde", 0, tmpNameBuf);
    result = tmpNameBuf;
#elif defined(BSLS_PLATFORM__OS_HPUX)
    char tmpPathBuf[L_tmpnam];
    result = tmpnam(tmpPathBuf);
#else
    result = tmpnam(0);
#endif

    // Test Invariant:

    BSLS_ASSERT(!result.empty());
    return result;
}

class MMIXRand {
    // Pseudo-Random number generator based on Donald Knuth's 'MMIX'

    static const bsls_Types::Uint64 A = 6364136223846793005ULL;
    static const bsls_Types::Uint64 C = 1442695040888963407ULL;

    // DATA
    bsls_Types::Uint64       d_reg;
    bsl::stringstream        d_ss;
    char                     d_outBuffer[17];

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


//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
// Ensures that the following functions in usage example 2 compiles and runs on
// all platform.
//-----------------------------------------------------------------------------

namespace UsageExample2 {

void getFilesWithinTimeframe(bsl::vector<bsl::string> *vector,
                             const char               *item,
                             const bdet_Datetime&      start,
                             const bdet_Datetime&      end)
{
    bdet_Datetime datetime;
    int ret = bdesu_FileUtil::getLastModificationTime(&datetime, item);

    if (ret) {
        return;                                                       // RETURN
    }

    if (datetime < start || datetime > end) {
        return;                                                       // RETURN
    }

    vector->push_back(item);
}

void findMatchingFilesInTimeframe(bsl::vector<bsl::string> *result,
                                  const char               *pattern,
                                  const bdet_Datetime&      start,
                                  const bdet_Datetime&      end)
{
    result->clear();
    bdesu_FileUtil::visitPaths(
                              pattern,
                              bdef_BindUtil::bind(&getFilesWithinTimeframe,
                                                  result,
                                                  bdef_PlaceHolders::_1,
                                                  start,
                                                  end));
}

}  // close namespace UsageExample2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch(test) { case 0:
      case 12: {
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

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;

        // make sure there isn't an unfortunately named file in the way

#ifdef BSLS_PLATFORM__OS_WINDOWS
        bdesu_FileUtil::remove("temp.2", true);
        bsl::string logPath =  "temp.2\\logs2\\";
#else
        bdesu_FileUtil::remove("tmp.2",  true);
        bsl::string logPath =  "tmp.2/logs2/";
#endif

        ASSERT(0 == bdesu_FileUtil::createDirectories(logPath.c_str(), true));
        const int TESTSIZE = 10;
        bdet_Datetime modTime[TESTSIZE];

        for (int i = 0; i < TESTSIZE; ++i) {
            stringstream s;
            s << logPath << "file" << i << ".log";

            if (veryVerbose) {
                cout << "Creating file: " << s.str() << endl;
            }

            bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(s.str(),
                                                                     true,
                                                                     false);
            ASSERT(0 != fd);

            const char buffer[] = "testing";
            int bytes           = sizeof buffer;

            bdesu_FileUtil::write(fd, buffer, bytes);
            bdesu_FileUtil::close(fd);

#ifdef BSLS_PLATFORM__OS_WINDOWS
            Sleep(1000);  // 'Sleep' is in milliseconds on Windows.
#else
            sleep(1);
#endif

            bdesu_FileUtil::getLastModificationTime(&modTime[i], s.str());
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
        ASSERT(0 == bdesu_FileUtil::remove(logPath.c_str(), true));

      } break;
      case 11: {
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

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;

        // make sure there isn't an unfortunately named file in the way

#ifdef BSLS_PLATFORM__OS_WINDOWS
        bdesu_FileUtil::remove("temp.1");
        bsl::string logPath =  "temp.1\\logs";
#else
        bdesu_FileUtil::remove("tmp.1");
        bsl::string logPath =  "tmp.1/logs";
#endif

        bsl::string oldPath(logPath), newPath(logPath);
        bdesu_PathUtil::appendRaw(&oldPath, "old");
        bdesu_PathUtil::appendRaw(&newPath, "new");
        ASSERT(0 == bdesu_FileUtil::createDirectories(oldPath.c_str(), true));
        ASSERT(0 == bdesu_FileUtil::createDirectories(newPath.c_str(), true));
        bdesu_PathUtil::appendRaw(&logPath, "*.log");
        vector<bsl::string> logFiles;
        bdesu_FileUtil::findMatchingPaths(&logFiles, logPath.c_str());

        bdet_Datetime modTime;
        string        fileName;
        for (vector<bsl::string>::iterator it = logFiles.begin();
                                              it != logFiles.end(); ++it) {
           ASSERT(0 == bdesu_FileUtil::getLastModificationTime(&modTime, *it));
           bdesu_PathUtil::getLeaf(&fileName, *it);
           bsl::string* whichDirectory =
                2 < (bdetu_SystemTime::nowAsDatetime() - modTime).totalDays()
                  ? &oldPath
                  : &newPath;
           bdesu_PathUtil::appendRaw(whichDirectory, fileName.c_str());
           ASSERT(0 == bdesu_FileUtil::move(it->c_str(),
                                            whichDirectory->c_str()));
           bdesu_PathUtil::popLeaf(whichDirectory);
        }

#if 0
        // file i/o

        // create a new file
        bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(
            "tempfile", true, false);
        ASSERT(fd != bdesu_FileUtil::INVALID_FD);
        // allocate a buffer with the size equal to memory page size and
        // fill with some data
        int size = bdesu_FileUtil::pageSize();
        char* buf = new char[size];
        for(int i=0; i<size; ++i) {
            buf[i] = i & 0xFF;
        }

        // write data to the file
        bdesu_FileUtil::seek(fd, size, bdesu_FileUtil::FROM_BEGINNING);
        int rc = bdesu_FileUtil::write(fd, buf, size);
        ASSERT(rc == size);

        // map the data page into memory
        char* data;
        rc = bdesu_FileUtil::map(fd, (void**)&data, 0, size, true);
        ASSERT(0 == rc);

        // verify the data is equal to what we have written
        ASSERT(0 != data);
        ASSERT(0 == memcmp(buf, data, size));

        // unmap the page, delete the buffer and close the file
        rc = bdesu_FileUtil::unmap(data, size);
        ASSERT(0 == rc);
        delete[] buf;
        bdesu_FileUtil::close(fd);
#endif

        // NOT IN USAGE EXAMPLE: CLEAN UP

        ASSERT(0 == bdesu_PathUtil::popLeaf(&logPath));
        ASSERT(0 == bdesu_FileUtil::remove(logPath.c_str(), true));
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TRYLOCK TEST
        //
        // Concerns:
        //   That 'tryLock' returns proper status on failure, depending upon
        //   the type of failure.
        //
        // Plan:
        //   Create a file and try to lock it twice, then try to lock a file
        //   that doesn't exist.  Verify different return codes in those two
        //   cases.
        // --------------------------------------------------------------------

        int rc;

        typedef bdesu_FileUtil::FileDescriptor FD;

        const char *fileNameWrite   = "tmp.bdesu_fileutil_10.write.txt";
        const char *fileNameRead    = "tmp.bdesu_fileutil_10.read.txt";
        const char *fileNameSuccess = "tmp.bdesu_fileutil_10.success.txt";

        bool isChild = verbose && argv[2] == bsl::string("child");
        if (isChild) {
            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = false;
        }
        else {
            if (verbose) cout << "tryLock test\n"
                                 "============\n";

            bdesu_FileUtil::remove(fileNameWrite);
            bdesu_FileUtil::remove(fileNameRead);
            bdesu_FileUtil::remove(fileNameSuccess);
        }

        FD fdWrite = bdesu_FileUtil::open(fileNameWrite, true, isChild);
        FD fdRead  = bdesu_FileUtil::open(fileNameRead,  true, isChild);

        if (!isChild) {
            // parent process

            bdesu_FileUtil::write(fdWrite, "woof", 4);
            rc = bdesu_FileUtil::tryLock(fdWrite, true);
            ASSERT(0 == rc);
            bdesu_FileUtil::write(fdRead,  "woof", 4);
            rc = bdesu_FileUtil::tryLock(fdRead,  false);
            ASSERT(0 == rc);

            bsl::stringstream cmd;
            cmd << argv[0] << ' ' << argv[1] << " child";
            cmd << (verbose ? " v" : "");
            cmd << (veryVerbose ? " v" : "");
            cmd << " &";

            bsl::system(cmd.str().c_str());

            localSleep(3);

            ASSERT(bdesu_FileUtil::exists(fileNameSuccess));

            rc = bdesu_FileUtil::unlock(fdWrite);
            ASSERT(0 == rc);
            rc = bdesu_FileUtil::unlock(fdRead);
            ASSERT(0 == rc);
            bdesu_FileUtil::close(fdWrite);
            bdesu_FileUtil::close(fdRead);

            // try to lock a closed file descriptor

            if (verbose) Q(Locking closed file descriptor);

            rc = bdesu_FileUtil::tryLock(fdWrite, true);
            ASSERT(0 != rc);

            if (verbose) P(localErrval());

#ifdef BSLS_PLATFORM__OS_UNIX
            LOOP_ASSERT(localErrval(), EBADF == localErrval());
#endif

            bdesu_FileUtil::remove(fileNameWrite);
            bdesu_FileUtil::remove(fileNameRead);
            bdesu_FileUtil::remove(fileNameSuccess);
        }
        else {
            // child process

            if (verbose) Q(Locked for write twice);

            rc = bdesu_FileUtil::tryLock(fdWrite, true);
            ASSERT(0 != rc);

            if (verbose) P(localErrval());

#ifdef BSLS_PLATFORM__OS_UNIX
# if defined(BSLS_PLATFORM__OS_HPUX) || defined(BSLS_PLATFORM__OS_AIX)
            LOOP_ASSERT(localErrval(), EACCES == localErrval());
# else
            LOOP_ASSERT(localErrval(), EAGAIN == localErrval());
# endif
#endif
            if (verbose) Q(Locked for read then write);

            rc = bdesu_FileUtil::tryLock(fdRead, true);
            ASSERT(0 != rc);

            if (verbose) P(localErrval());

#ifdef BSLS_PLATFORM__OS_UNIX
# if defined(BSLS_PLATFORM__OS_HPUX) || defined(BSLS_PLATFORM__OS_AIX)
            LOOP_ASSERT(localErrval(), EACCES == localErrval());
# else
            LOOP_ASSERT(localErrval(), EAGAIN == localErrval());
# endif
#endif

            if (0 == testStatus) {
                // Touch the 'success' file to tell the parent process we
                // succeeded.

                FD fdSuccess = bdesu_FileUtil::open(fileNameSuccess, true,
                                                    false);
                bdesu_FileUtil::close(fdSuccess);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GETFILESIZELIMIT TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "getFileSizeLimit test\n"
                             "=====================\n";

        bdesu_FileUtil::Offset limit = bdesu_FileUtil::getFileSizeLimit();

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
        //  1. Create a file in append mode, write a charater, use seek to
        //     change the position of output, write another character, and
        //     verify that the new character is added after the original
        //     character.
        //  2. Reopen the file in append mode, write a charater and ensure that
        //     it is added to the end of the file.
        //  3. Reopen the file in normal mode, write a charater and ensure that
        //     it overwrites the data in the file instead of appending to it.
        //
        // Testing:
        //   FD open(const char *p, bool writable, bool exist, bool append);
        // --------------------------------------------------------------------

        bsl::string fileName(tempFileName());

        if (verbose) { P(fileName) }

        bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(
                                                  fileName, true, false, true);
        ASSERT(bdesu_FileUtil::INVALID_FD != fd);

        bdesu_FileUtil::write(fd, "A", 1);
        char result[16];

        bdesu_FileUtil::seek(fd, 0, bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(1 == bdesu_FileUtil::read(fd, result, sizeof result));

        bdesu_FileUtil::seek(fd, 0, bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING);
        bdesu_FileUtil::write(fd, "B", 1);

        bdesu_FileUtil::seek(fd, 0, bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(2 == bdesu_FileUtil::read(fd, result, sizeof result));

        bdesu_FileUtil::close(fd);

        fd = bdesu_FileUtil::open(fileName, true, true, true);
        bdesu_FileUtil::write(fd, "C", 1);
        bdesu_FileUtil::seek(fd, 0, bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(3 == bdesu_FileUtil::read(fd, result, sizeof result));

        bdesu_FileUtil::close(fd);

        fd = bdesu_FileUtil::open(fileName, true, true);
        bdesu_FileUtil::write(fd, "D", 1);
        bdesu_FileUtil::close(fd);

        fd = bdesu_FileUtil::open(fileName, false, true);
        ASSERT(3 == bdesu_FileUtil::read(fd, result, sizeof result));
        bdesu_FileUtil::close(fd);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SIMPLE MATCHING TEST
        //
        // Concerns:
        //   Unix "glob()", which is called by bdesu_FileUtil::visitPaths, is
        //   failing on ibm 64 bit, unfortunately the test driver has not
        //   detected or reproduced this error.  This test case is an attempt
        //   to get this test driver reproducing the problem.
        //
        // Plan:
        //   Run the usage example 1
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSimple matching test"
                          << "\n====================" << endl;

        const char* dirName = "testDirCase7";
        bdesu_FileUtil::remove(dirName, true);
        bdesu_FileUtil::createDirectories(dirName, true);
        bdesu_FileUtil::setWorkingDirectory(dirName);
        for(int i=0; i<4; ++i) {
            char name[16];
            sprintf(name, "woof.a.%d", i);
            bdesu_FileUtil::FileDescriptor fd =
                bdesu_FileUtil::open(name, true, false);
            bdesu_FileUtil::close(fd);
        }

        vector<string> vs;
        bdesu_FileUtil::findMatchingPaths(&vs, "woof.a.?");
        sort(vs.begin(), vs.end());

        ASSERT(vs.size() == 4);
        ASSERT(vs[0] == "woof.a.0");
        ASSERT(vs[1] == "woof.a.1");
        ASSERT(vs[2] == "woof.a.2");
        ASSERT(vs[3] == "woof.a.3");

        bdesu_FileUtil::setWorkingDirectory("..");
        bdesu_FileUtil::remove(dirName, true);
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

        if (verbose) cout << "\nTesting 'getFileSize'"
                          << "\n=====================" << endl;

        // Setup by first creating a tmp file
#ifdef BSLS_PLATFORM__OS_WINDOWS
        string fileName("getFileSizeTest.txt");  // not sure where to put it
#else
        string fileName("/tmp/getFileSizeTest.txt");
#endif
        bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(fileName,
                                                                 true,
                                                                 false);
        ASSERT(0 != fd);

        const char buffer[] = "testing";
        int bytes           = sizeof buffer;

        bdesu_FileUtil::write(fd, buffer, bytes);
        bdesu_FileUtil::close(fd);

        // Concern 1

        {
            if (veryVerbose) cout << "\n1. Normal file" << endl;

            bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize(fileName);
            LOOP2_ASSERT(bytes, off, bytes == off);

            bdesu_FileUtil::Offset off2 = bdesu_FileUtil::getFileSize(
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

#ifdef BSLS_PLATFORM__OS_WINDOWS
            string dirName("getFileSizeDir");

            // windows directories are 0 sized

            const bdesu_FileUtil::Offset EXPECTED = 0;
#else
            string dirName("/tmp/getFileSizeDir");
#endif

            int ret = bdesu_FileUtil::createDirectories(dirName, true);
            ASSERT(0 == ret);

            // On UNIX use stat64 as an oracle: the file size of a directory
            // depends on the file system.

#ifndef BSLS_PLATFORM__OS_WINDOWS
            struct stat64 oracleInfo;
            int rc = ::stat64(dirName.c_str(), &oracleInfo);
            ASSERT(0 == rc);

            bdesu_FileUtil::Offset EXPECTED = oracleInfo.st_size;
#endif

            bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize(dirName);
            LOOP2_ASSERT(EXPECTED, off, EXPECTED == off);

            bdesu_FileUtil::Offset off2 = bdesu_FileUtil::getFileSize(
                                                             dirName.c_str());
            LOOP2_ASSERT(EXPECTED, off2, EXPECTED == off2);

            if (veryVerbose) {
                cout << "Expected " << EXPECTED << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdesu_FileUtil::remove(dirName);
        }

        // Concern 3

        {
            if (veryVerbose) cout << "\n4. Relative Path" << endl;

            string fileName("../getFileSizeTest.txt");
            bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(fileName,
                                                                     true,
                                                                     false);
            ASSERT(0 != fd);

            const char buffer[] = "testing";
            int bytes           = sizeof buffer;

            bdesu_FileUtil::write(fd, buffer, bytes);
            bdesu_FileUtil::close(fd);

            bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize(fileName);
            ASSERT(bytes == off);

            bdesu_FileUtil::Offset off2 = bdesu_FileUtil::getFileSize(
                                                             fileName.c_str());
            ASSERT(bytes == off2);

            if (veryVerbose) {
                cout << "Expected " << bytes << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdesu_FileUtil::remove(fileName);
        }

#ifndef BSLS_PLATFORM__OS_WINDOWS
        // Concern 4
        // No symbolic links on windows.

        {
            if (veryVerbose) cout << "\n5. Symbolic Links" << endl;
            system("ln -s /tmp/getFileSizeTest.txt testLink");

            string fileName("testLink");
            bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize(fileName);
            ASSERT(bytes == off);

            bdesu_FileUtil::Offset off2 = bdesu_FileUtil::getFileSize(
                                                             fileName.c_str());
            ASSERT(bytes == off2);

            if (veryVerbose) {
                cout << "Expected " << bytes << endl;
                cout << "Actual ";
                P_(off) P(off2)
            }

            bdesu_FileUtil::remove(fileName);
        }
#endif

        // Concert 5

        {
            if (veryVerbose) cout << "\n6. Non existent file" << endl;

            // Use a random name.

            bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize("acasdf");

            ASSERT(-1 == off);
            if (veryVerbose) {
                cout << "Expected -1" << endl;
                cout << "Actual " << off << endl;
            }
        }

        // Clean up the tmp file.

        bdesu_FileUtil::remove(fileName);

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

        if (verbose) cout << "\nTesting 'getAvailableSpace'"
                          << "\n===========================" << endl;

        bdesu_FileUtil::Offset avail = bdesu_FileUtil::getAvailableSpace(".");
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERT(0 <= avail);

        bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(
                                                  tempFileName(), true, false);
        ASSERT(bdesu_FileUtil::INVALID_FD != fd);

        avail = bdesu_FileUtil::getAvailableSpace(fd);
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERT(0 <= avail);

        bdesu_FileUtil::close(fd);
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

        if (verbose) cout << "\nTesting 'rollFileChain' (files)"
                          << "\n===============================" << endl;

        enum { MAXSUFFIX=3 };
#ifdef BSLS_PLATFORM__OS_WINDOWS
        bsl::string SEP = "\\";
#else
        bsl::string SEP = "/";
#endif
        bsl::string tmpFile(tempFileName());
        bdesu_FileUtil::FileDescriptor f;

        ASSERT(0 == bdesu_FileUtil::rollFileChain(tmpFile, MAXSUFFIX));

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            f = bdesu_FileUtil::open(tmpFile, true, false);
            LOOP_ASSERT(tmpFile, f != bdesu_FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdesu_FileUtil::write(f, &i, sizeof(int)));
            ASSERT(0 == bdesu_FileUtil::close(f));

            // Roll the file(s).

            ASSERT(0 == bdesu_FileUtil::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != bdesu_FileUtil::remove(tmpFile)); // does not exist
        tmpFile += ".0";
        int pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = bdesu_FileUtil::open(tmpFile, false, true); // must exist
            LOOP_ASSERT(tmpFile, f != bdesu_FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdesu_FileUtil::read(
                                                      f, &value, sizeof(int)));
            ASSERT(0 == bdesu_FileUtil::close(f));
            ASSERT(0 == bdesu_FileUtil::remove(tmpFile));
            LOOP2_ASSERT(i, value, i == value);
        }

        if (verbose) {
           cout << "rollFileChain test (directories)" << endl;
        }
        tmpFile = "tmpDir";

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            ASSERT(0 == bdesu_FileUtil::createDirectories(tmpFile, true));
            f = bdesu_FileUtil::open(tmpFile+SEP+"file", true, false);
            ASSERT(f != bdesu_FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdesu_FileUtil::write(f, &i, sizeof(int)));
            ASSERT(0 == bdesu_FileUtil::close(f));

            // Roll the file(s).

            ASSERT(0 == bdesu_FileUtil::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != bdesu_FileUtil::remove(tmpFile, true)); // does not exist
        tmpFile += ".0";
        pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = bdesu_FileUtil::open(tmpFile+SEP+"file", false, true);
            ASSERT(f != bdesu_FileUtil::INVALID_FD);
            ASSERT(sizeof(int) == bdesu_FileUtil::read(
                                                      f, &value, sizeof(int)));
            ASSERT(0 == bdesu_FileUtil::close(f));
            ASSERT(0 == bdesu_FileUtil::remove(tmpFile, true));
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

        if (verbose) cout << "\nTesting 'isRegularFile' & 'isDirectory'"
                          << "\n======================================="
                          << endl;

        struct Parameters {
            const char* good;
            const char* badNoExist;
            const char* badWrongType;
        };

        struct ParametersByType {
            Parameters regular;
            Parameters directory;
        } parameters = {
#ifdef BSLS_PLATFORM__OS_WINDOWS
            { "case4\\file", "case4\\file2", "case4\\dir" },
            { "case4\\dir", "case4\\dir2", "case4\\file"}
#else
            { "case4/file", "case4/file2", "case4/dir" },
            { "case4/dir", "case4/dir2", "case4/file"}
#endif
        };

        const Parameters& r = parameters.regular;
        const Parameters& d = parameters.directory;

        ASSERT(0 == bdesu_FileUtil::createDirectories(r.good));

        makeArbitraryFile(r.good);
        ASSERT(0 == bdesu_FileUtil::createDirectories(r.badWrongType, true));
        ASSERT(true == bdesu_FileUtil::isRegularFile(r.good));
        ASSERT(false == bdesu_FileUtil::isRegularFile(r.badNoExist));
        ASSERT(false == bdesu_FileUtil::isRegularFile(r.badWrongType));

        makeArbitraryFile(d.badWrongType);
        ASSERT(0 == bdesu_FileUtil::createDirectories(d.good, true));
        ASSERT(true == bdesu_FileUtil::isDirectory(d.good));
        ASSERT(false == bdesu_FileUtil::isDirectory(d.badNoExist));
        ASSERT(false == bdesu_FileUtil::isDirectory(d.badWrongType));

#ifndef BSLS_PLATFORM__OS_WINDOWS
        if (veryVerbose) {
           cout << "...symbolic link tests..." << endl;
        }

        bsl::string absolute;
        ASSERT(0 == bdesu_FileUtil::getWorkingDirectory(&absolute));
        bdesu_PathUtil::appendRaw(&absolute, r.good);

        bsl::string link = absolute;
        bdesu_PathUtil::popLeaf(&link);
        bdesu_PathUtil::appendRaw(&link, "link_rg");
        int rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isRegularFile(link.c_str()));
        ASSERT(true  == bdesu_FileUtil::isRegularFile(link.c_str(), true));

        bsl::string link2 = r.good;
        bdesu_PathUtil::popLeaf(&link2);
        bdesu_PathUtil::appendRaw(&link2, "link_rg2");
        rc = symlink(link.c_str(), link2.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isRegularFile(link2));
        ASSERT(true  == bdesu_FileUtil::isRegularFile(link2, true));

        bdesu_PathUtil::popLeaf(&link);
        bdesu_PathUtil::appendRaw(&link, "link_rbw");
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::appendRaw(&absolute, r.badWrongType);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isRegularFile(link));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link, true));

        bdesu_PathUtil::popLeaf(&link);
        bdesu_PathUtil::appendRaw(&link, "link_rbn");
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::appendRaw(&absolute, r.badNoExist);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isRegularFile(link));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link, true));

        bdesu_PathUtil::popLeaf(&link);
        bdesu_PathUtil::appendRaw(&link, "link_dg");
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::popLeaf(&absolute);
        bdesu_PathUtil::appendRaw(&absolute, d.good);
        rc = symlink(absolute.c_str(), link.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isDirectory(link));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link));
        ASSERT(true  == bdesu_FileUtil::isDirectory(link, true));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link, true));

        bdesu_PathUtil::popLeaf(&link2);
        bdesu_PathUtil::appendRaw(&link2, "link_dg2");
        rc = symlink(link.c_str(), link2.c_str());

        // test invariant:

        ASSERT(0 == rc);

        ASSERT(false == bdesu_FileUtil::isDirectory(link2));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link2));
        ASSERT(true  == bdesu_FileUtil::isDirectory(link2, true));
        ASSERT(false == bdesu_FileUtil::isRegularFile(link2, true));

#endif  // Symbolic link testing on non-Windows

#ifndef BSLS_PLATFORM__OS_WINDOWS  // (unix domain socket)
        {
            // Unix domain sockets should return 'false' for 'isRegularFile'
            // and 'isDirectory' (DRQS 2071065).

            if (veryVerbose) {
                cout << "...unix domain socket..." << endl;
            }
            bsl::string filename = tempFileName();
            bdesu_FileUtil::remove(filename);

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


            LOOP_ASSERT(filename, bdesu_FileUtil::exists(filename));
            LOOP_ASSERT(filename, !bdesu_FileUtil::isDirectory(filename));
            LOOP_ASSERT(filename, !bdesu_FileUtil::isRegularFile(filename));
            bdesu_FileUtil::remove(filename);
        }
#endif  // BSLS_PLATFORM__OS_WINDOWS (unix domain socket)

        //clean up

        ASSERT(0 == bdesu_FileUtil::remove("case4", true));
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

        if (verbose) cout << "\nTesting pattern matching"
                          << "\n========================" << endl;

        const char* filenames[] = {
           "abcd",
           "zyx",
           "zy.z",
           "abc.zzz",
           "abc.def"
        };

        bsl::string path("futc3");

        // The string literal "futc3/b???/*d*" seems to confuse the
        // Sun compiler, which complains about the character sequence "\*".
        // So let's hardcode it.

        const char tripleQMarkLiteral[] = {'f','u','t','c','3','/','b',
                                           '?','?','?','/','*','d','*', 0};

        struct Parameters {
            const char* pattern;
            const char* result;
        } parameters[] = {
            {"", ""},
            {"futc3/*/*foo*", ""},
            {"futc3/*/*d*", "futc3/alpha/abc.def:futc3/alpha/abcd:"
                            "futc3/beta/abc.def:futc3/beta/abcd"},
            {tripleQMarkLiteral, "futc3/beta/abc.def:futc3/beta/abcd"},
            {"futc3/*b*", "futc3/beta"},
#ifdef BSLS_PLATFORM__OS_WINDOWS
            {"futc3/*b*/*.?","futc3/beta/abcd:futc3/beta/zy.z:futc3/beta/zyx"},
            {"futc?/*b*/*.?","futc3/beta/abcd:futc3/beta/zy.z:futc3/beta/zyx"},
            {"futc?/*/abcd.*","futc3/alpha/abcd:futc3/beta/abcd"},
            {"futc?/*b*/*.*","futc3/beta/abc.def:futc3/beta/abc.zzz:"
                             "futc3/beta/abcd:futc3/beta/zy.z:futc3/beta/zyx"},
            {"futc3*/*/*.?",
               "futc3/alpha/abcd:futc3/alpha/zy.z:futc3/alpha/zyx:"
               "futc3/beta/abcd:futc3/beta/zy.z:futc3/beta/zyx"}
#else
            {"futc3/*b*/*.?", "futc3/beta/zy.z"},
            {"futc?/*b*/*.?", "futc3/beta/zy.z"},
            {"futc?/*/abcd.*", ""},
            {"futc?/*b*/*.*",
                      "futc3/beta/abc.def:futc3/beta/abc.zzz:futc3/beta/zy.z"},
            {"futc3*/*/*.?", "futc3/alpha/zy.z:futc3/beta/zy.z"}
#endif
        };

        const int numFiles = sizeof(filenames) / sizeof(*filenames);

        bdesu_PathUtil::appendRaw(&path, "alpha");
        ASSERT(0 == bdesu_FileUtil::createDirectories(path.c_str(), true));
        for (int i = 0; i < numFiles; ++i) {
            bdesu_PathUtil::appendRaw(&path, filenames[i]);
            bdesu_FileUtil::FileDescriptor f =  bdesu_FileUtil::open(
                                                            path, true, false);
            ASSERT(bdesu_FileUtil::INVALID_FD != f);
            ASSERT(0 == bdesu_FileUtil::close(f));
            bdesu_PathUtil::popLeaf(&path);
        }
        bdesu_PathUtil::popLeaf(&path);

        bdesu_PathUtil::appendRaw(&path, "beta");
        ASSERT(0 == bdesu_FileUtil::createDirectories(path.c_str(), true));
        for (int i = 0; i < numFiles; ++i) {
            bdesu_PathUtil::appendRaw(&path, filenames[i]);
            bdesu_FileUtil::FileDescriptor f =  bdesu_FileUtil::open(
                                                            path, true, false);
            ASSERT(bdesu_FileUtil::INVALID_FD != f);
            ASSERT(0 == bdesu_FileUtil::close(f));
            bdesu_PathUtil::popLeaf(&path);
        }
        bdesu_PathUtil::popLeaf(&path);

        vector<bsl::string> resultPaths;
        enum { NUM_PARAMETERS = sizeof(parameters) / sizeof(*parameters) };
        for (int i = 0; i < NUM_PARAMETERS; ++i) {
            const Parameters& p = parameters[i];
#ifdef BSLS_PLATFORM__OS_WINDOWS
            string filename(p.pattern);
            replace_if(filename.begin(), filename.end(), isForwardSlash, '\\');
            bdesu_FileUtil::findMatchingPaths(&resultPaths, filename.c_str());
#else
            bdesu_FileUtil::findMatchingPaths(&resultPaths, p.pattern);
#endif
            string rollup = rollupPaths(resultPaths);
            LOOP2_ASSERT(p.result, rollup, string(p.result) == rollup);
        }

        ASSERT(0 == bdesu_FileUtil::remove(path.c_str(), true));
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

        if (verbose) cout << "\nUsage Example like Testing"
                          << "\n==========================" << endl;

#ifdef BSLS_PLATFORM__OS_WINDOWS
        bdesu_FileUtil::remove("temp2", true);
        bsl::string logPath =  "temp2\\logs";
#else
        bdesu_FileUtil::remove("tmp2", true);
        bsl::string logPath =  "tmp2/logs";
#endif

        bsl::string oldPath(logPath), newPath(logPath);
        bdesu_PathUtil::appendRaw(&oldPath, "old");
        bdesu_PathUtil::appendRaw(&newPath, "new");
        ASSERT(0 == bdesu_FileUtil::createDirectories(oldPath.c_str(),
                                                            true));
        ASSERT(0 == bdesu_FileUtil::createDirectories(newPath.c_str(),
                                                            true));

        ASSERT(bdesu_FileUtil::exists(oldPath));
        ASSERT(bdesu_FileUtil::exists(newPath));

        char filenameBuffer[20];

        // TBD: When SetFileInformationByHandle() is available, then
        // we should write a setModificationTime() method and use it
        // here (see bdesu_fileutil.h).  Until then, we use utime() on
        // POSIX directly and we do not attempt to "touch" Windows files.

        enum {
            NUM_TOTAL_FILES = 10,
#ifdef BSLS_PLATFORM__OS_WINDOWS
            NUM_OLD_FILES = 0,
#else
            NUM_OLD_FILES = 3,
#endif
            NUM_NEW_FILES = NUM_TOTAL_FILES - NUM_OLD_FILES
        };

        for (int i = 0; i < NUM_TOTAL_FILES; ++i) {
            bool isOld = i < NUM_OLD_FILES;

            int filenameLength = sprintf(filenameBuffer,
                                         "fileutil%02d_%c.log", i,
                                         isOld ? 'o' : 'n');

            ASSERT(0 == bdesu_PathUtil::appendIfValid(&logPath,
                                                      filenameBuffer));

            if (bdesu_FileUtil::exists(logPath.c_str())) {
                ASSERT(0 == bdesu_FileUtil::remove(logPath.c_str(), true));
            }
            bdesu_FileUtil::FileDescriptor f =
                            bdesu_FileUtil::open(logPath.c_str(), true, false);
            LOOP_ASSERT(logPath, f != bdesu_FileUtil::INVALID_FD);

            filenameBuffer[filenameLength++] = '\n';
            filenameBuffer[filenameLength++] = 0;

            ASSERT(filenameLength == bdesu_FileUtil::write(f,
                                                           filenameBuffer,
                                                           filenameLength));
            ASSERT(0 == bdesu_FileUtil::close(f));

#ifndef BSLS_PLATFORM__OS_WINDOWS
            bdet_TimeInterval threeDaysAgo = bdetu_SystemTime::now() -
                                           bdet_TimeInterval(3 * 24 * 3600, 0);
            if (isOld) {
                struct utimbuf timeInfo;
                timeInfo.actime = timeInfo.modtime = threeDaysAgo.seconds();

                //test invariant:

                ASSERT(0 == utime(logPath.c_str(), &timeInfo));
            }
#endif
            bdesu_PathUtil::popLeaf(&logPath);
        }

        bdesu_PathUtil::appendRaw(&logPath, "*.log");
        vector<bsl::string> logFiles;
        bdesu_FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        bdesu_PathUtil::popLeaf(&logPath);

        bdet_Datetime modTime;
        string        fileName;
        for (vector<bsl::string>::iterator it = logFiles.begin();
                                                  it != logFiles.end(); ++it) {
            ASSERT(0 == bdesu_FileUtil::getLastModificationTime(&modTime,
                                                                 it->c_str()));
            bdesu_PathUtil::getLeaf(&fileName, *it);
            bsl::string* whichDirectory =
                  2 < (bdetu_SystemTime::nowAsDatetime() - modTime).totalDays()
                    ? &oldPath
                    : &newPath;
            bdesu_PathUtil::appendRaw(whichDirectory, fileName.c_str());
            ASSERT(0 == bdesu_FileUtil::move(it->c_str(),
                                                     whichDirectory->c_str()));
            bdesu_PathUtil::popLeaf(whichDirectory);
        }

        // Now validate

        bdesu_PathUtil::appendRaw(&logPath, "*");
        bdesu_PathUtil::appendRaw(&logPath, "*o*.log");
        bdesu_FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        ASSERT(NUM_OLD_FILES == logFiles.size());
        bdesu_PathUtil::popLeaf(&logPath);

        bdesu_PathUtil::appendRaw(&logPath, "*n*.log");
        bdesu_FileUtil::findMatchingPaths(&logFiles, logPath.c_str());
        ASSERT(NUM_NEW_FILES == logFiles.size());
        bdesu_PathUtil::popLeaf(&logPath);
        bdesu_PathUtil::popLeaf(&logPath);

        // Clean up

        ASSERT(0 == bdesu_PathUtil::popLeaf(&logPath));
        ASSERT(0 == bdesu_FileUtil::remove(logPath.c_str(), true));
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

#ifndef BSLS_PLATFORM__OS_WINDOWS
        // Not sure how to create large files on windows, so test only on UNIX.
        // However, this function had been tested on windows by creating the
        // large file through cygwin.

        // Create a 5 GB file.

        if (veryVerbose) cout << "\n3. Large File" << endl;

        system("dd if=/dev/zero of=/tmp/fiveGBFile "
               "bs=1024000 count=5000");

        string fileName("/tmp/fiveGBFile");

        bdesu_FileUtil::Offset off = bdesu_FileUtil::getFileSize(fileName);
        ASSERT(5120000000LL == off);

        bdesu_FileUtil::Offset off2 = bdesu_FileUtil::getFileSize(
                                                         fileName.c_str());
        ASSERT(5120000000LL == off2);

        if (veryVerbose) {
            cout << "Expected 5120000000LL" << endl;
            cout << "Actual ";
            P_(off) P(off2)
        }

        bdesu_FileUtil::remove(fileName);
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------

        static const char* foo = "/tmp/blahblah.tmp";
        bdesu_FileUtil::remove(foo);
        bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(foo, 1, 0);
        int pageSize = bdesu_MemoryUtil::pageSize();
        bdesu_FileUtil::grow(fd, pageSize);
        int *p;
        ASSERT(0 == bdesu_FileUtil::map(fd, (void**)&p, 0, pageSize,
                                   bdesu_MemoryUtil::BDESU_ACCESS_READ_WRITE));
        printf("mapped at %p\n", p);
        for(int i = 0; i < 10000; ++i) {
          ASSERT(0 == bdesu_FileUtil::seek(fd, 0,
                                   bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING));
          int buf;
          *p = i;
          ASSERT(sizeof(int) == bdesu_FileUtil::read(fd, &buf, sizeof(int)));
          ASSERT(i == buf);
        }
        ASSERT(0 == bdesu_FileUtil::unmap(p, pageSize));

#if 0
        bdesu_FileUtil::FileDescriptor fd =
                       bdesu_FileUtil::open("/bb/data/tmp/blahblah.tmp", 1, 0);
        int pageSize = bdesu_FileUtil::pageSize();
        int nPages = 1024*1024*1024 / pageSize;
        int rc = bdesu_FileUtil::grow(fd, nPages * pageSize, argc > 2);
        printf("grow(fd, %d, %d) rc=%d\n",
               nPages * pageSize, (int)(argc>2), rc);
        int fileSize = bdesu_FileUtil::seek(fd, 0, SEEK_END);
        printf("file size = %d\n", fileSize);
        if (!rc) {
            for(int i=0; i<nPages; i++) {
                bdesu_FileUtilMapping fm =
                         bdesu_FileUtil::map(fd, i * pageSize, pageSize, true);
                memset(fm.addr(), 2, pageSize);
                bdesu_FileUtil::unmap(fm, pageSize);
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

        if (verbose) cout << "SIMPLE 5G FILE TEST CASE\n"
                             "========================\n";

        typedef bdesu_FileUtil Util;

#if 1
        const bsls_Types::Int64 fiveGig = 5LL * 1000LL * 1000LL * 1000LL;
        const bsls_Types::Int64 deltaMileStone = 100LL * 1000LL * 1000LL;
#else
        const bsls_Types::Int64 fiveGig = 5 * 1000LL * 1000LL;
        const bsls_Types::Int64 deltaMileStone = 100LL * 1000LL;
#endif

        bsls_Types::Int64 mileStone = deltaMileStone;

        bsls_Types::Int64 bytesWritten = 0;

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

        char inBuf[80];
        bsls_Types::Int64 bytesRead = 0;
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
      }  break;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
