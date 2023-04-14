// bdls_filesystemutil.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_filesystemutil.h>

#include <bdlb_guid.h>
#include <bdlb_guidutil.h>

#include <bdlde_charconvertutf16.h>

#include <bdlf_bind.h>

#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>

#include <bdls_memoryutil.h>
#include <bdls_pathutil.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bsla_maybeunused.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_isconvertible.h>

#include <bslmt_threadutil.h>

#include <bsl_algorithm.h>
#include <bsl_c_errno.h>
#include <bsl_c_stdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

#include <ctime>

#ifndef BSLS_PLATFORM_OS_WINDOWS
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <sys/statvfs.h>
    #include <sys/types.h>
    #include <sys/un.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <utime.h>
#else // BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>
    #include <fcntl.h>    // for _O_U16TEXT
    #include <io.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// typedef bsl::function<void (const char *path)> Func;
//
// CLASS METHODS
// [ 3] FD open(const char * path, openPolicy, ioPolicy, truncatePolicy)
// [ 5] bool isRegularFile(const char *, bool)
// [ 5] bool isDirectory(const char *, bool)
// [ 6] bool move(const char *, const char *)
// [ 6] bool move(STR_TYPE, STR_TYPE)
// [ 7] int rollFileChain(const char *, int)
// [ 8] Offset getAvailableSpace(const char *)
// [ 8] Offset getAvailableSpace(FileDescriptor)
// [ 9] int getSystemTemporaryDirectory(bsl::string *path);
// [10] Offset getFileSize(const char *)
// [10] Offset getFileSize(FileDescriptor)
// [12] FD open(const char * p, bool writable, bool exist, bool append)
// [13] static Offset getFileSizeLimit()
// [14] int tryLock(FileDescriptor, bool ) (Unix)
// [15] int tryLock(FileDescriptor, bool ) (Windows)
// [16] int sync(char *, int , bool )
// [17] int close(FileDescriptor )
// [21] makeUnsafeTemporaryFilename(string *, const string_view&)
// [22] createTemporaryFile(string *, const string_view&)
// [23] createTemporaryDirectory(string *, const string_view&)
// [24] int createDirectories(const char *, bool);
// [24] int createPrivateDirectory(const string_view&);
// [25] int visitTree(const char * , const string&, const Func&, bool);
// [25] int visitPaths(const char * , const Func&);
// [26] int getLastModificationTime(bdlt::Datetime *, FileDescriptor);
// [27] bool isSymbolicLink(STRING_TYPE);
// [27] int getSymbolicLinkTarget(STRING_TYPE *, STRING_TYPE);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] CONCERN: findMatchingPaths incorrect on ibm 64-bit
// [17] CONCERN: Open in append-mode behavior (particularly on windows)
// [18] CONCERN: Unix File Permissions for 'open'
// [19] CONCERN: Unix File Permissions for 'createDirectories' et al
// [20] CONCERN: UTF-8 Filename handling
// [21] CONCERN: entropy in temp file name generation
// [22] CONCERN: file permissions
// [23] CONCERN: directory permissions
// [24] CONCERN: error codes for 'createDirectories'
// [24] CONCERN: error codes for 'createPrivateDirectory'
// [28] USAGE EXAMPLE 1
// [29] USAGE EXAMPLE 2

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

#define ASSERT_EQ(X,Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X,Y,X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X,Y,X < Y)
#define ASSERT_LE(X,Y) ASSERTV(X,Y,X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X,Y,X > Y)
#define ASSERT_GE(X,Y) ASSERTV(X,Y,X >= Y)

#define LOOP_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)
#define LOOP_ASSERT_LT(L,X,Y) ASSERTV(L,X,Y,X < Y)
#define LOOP_ASSERT_LE(L,X,Y) ASSERTV(L,X,Y,X <= Y)
#define LOOP_ASSERT_GT(L,X,Y) ASSERTV(L,X,Y,X > Y)
#define LOOP_ASSERT_GE(L,X,Y) ASSERTV(L,X,Y,X >= Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                          GLOBAL DATA FOR TESTING
// ----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS
#   define PS "\\"
#else
#   define PS "/"
#endif

enum { NAME_ASCII, NAME_UTF8, NAME_ANSI };
static const char *const NAMES[] = {
    "name",                                      // ASCII
    "\x24\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2",  // utf-8
#ifndef BSLS_PLATFORM_OS_DARWIN
// 'NAME_ANSI' is not utf8, and the filesystem translates the name to
// "%F1%E5m%EA".
    "\xf1\xe5m\xea",                             // not utf-8
#endif
};
static const size_t NUM_NAMES  = sizeof NAMES / sizeof *NAMES;

#ifdef BSLS_PLATFORM_OS_WINDOWS
// 'NAME_ANSI' is not utf8, therefore the Windows implementation will refuse to
// create a file with that name.

static const size_t NUM_VALID_NAMES = NUM_NAMES - 1;
#else
static const size_t NUM_VALID_NAMES = NUM_NAMES;
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
enum { e_IS_UNIX = 1 };
#else
enum { e_IS_UNIX = 0 };
#endif

// ============================================================================
//                 GLOBAL HELPER TYPE FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FilesystemUtil Obj;
typedef bsls::Types::Int64   Int64;
typedef bsls::Types::UintPtr UintPtr;

#define INT_SIZEOF(x)    static_cast<int>(sizeof(x))

#ifdef BSLS_PLATFORM_OS_WINDOWS
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

void localTouch(const bsl::string& fileName)
{
    Obj::FileDescriptor fd = Obj::open(fileName,
                                       Obj::e_OPEN_OR_CREATE,
                                       Obj::e_READ_WRITE);
    ASSERT(Obj::k_INVALID_FD != fd);

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
    return GetCurrentProcessId();
#else
    return ::getpid();
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

template <class VECTOR_TYPE, class STRING_TYPE>
STRING_TYPE rollupPaths(VECTOR_TYPE& paths)
{
    STRING_TYPE result;

    sort(paths.begin(), paths.end());

    for (typename VECTOR_TYPE::const_iterator it = paths.begin();
         it != paths.end();
         ++it) {
        result.append(*it);
        result.push_back(':');
    }

    if (!result.empty()) {
        result.erase(result.end() - 1);
    }

#ifdef BSLS_PLATFORM_OS_WINDOWS
    replace_if(result.begin(), result.end(), ::isBackslash, '/');
#endif

    return result;
}

inline
void makeArbitraryFile(const char *path)
{
    Obj::FileDescriptor fd = Obj::open(path,
                                       Obj::e_OPEN_OR_CREATE,
                                       Obj::e_READ_WRITE);
    ASSERT(Obj::k_INVALID_FD != fd);
    ASSERT(5 == Obj::write(fd, "hello", 5));
    ASSERT(0 == Obj::close(fd));
}

static bool createSymlink(const bsl::string& oldPath,
                          const bsl::string& newPath)
    // Create a symbolic link, referring to the specified 'oldPath', at the
    // specified 'newPath' path.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    bsl::wstring wideOld, wideNew;
    int rc = bdlde::CharConvertUtf16::utf8ToUtf16(&wideOld, oldPath);
    ASSERT(rc == 0);
    rc = bdlde::CharConvertUtf16::utf8ToUtf16(&wideNew, newPath);
    ASSERT(rc == 0);

    DWORD dwFlags = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
        // Developer mode must be enabled for this flag to take effect
    if (Obj::isDirectory(oldPath)) {
        dwFlags |= SYMBOLIC_LINK_FLAG_DIRECTORY;
    }
    // Available since Windows Vista / Server 2008
    return CreateSymbolicLinkW(wideNew.c_str(), wideOld.c_str(), dwFlags);
#else // POSIX
    return ::symlink(oldPath.c_str(), newPath.c_str()) == 0;
#endif
}

struct VisitTreeTestVisitor {
    // DATA
    bsl::vector<bsl::string> *d_vec;

    void operator()(const char *filePath)
    {
        d_vec->push_back(filePath);
    }
};

static bsl::string tempFileName(int testCase, const char *fnTemplate = 0)
    // Return a temporary file name, with the specified 'testCase' being part
    // of the file name, and with the optionally specified 'fnTemplate', if
    // specified, also being part of the file name.
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

namespace BloombergLP {
namespace {
namespace u {

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
    // This testing-only utility 'struct' provides a suite of platform-agnostic
    // file operations that this test driver uses in its implementation.  These
    // functions are candidates for promotion to the public interface of
    // 'bdls::FilesystemUtil'.

    // TYPES
    typedef bdls::FilesystemUtil::FileDescriptor FileDescriptor;
        // 'FileDescriptor' is an alias for the operating system's native file
        // descriptor / file handle type.

    typedef bdls::FilesystemUtil::Offset Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.

    // CLASS METHODS
    static FileDescriptor createEphemeralFile();
        // Create an ephemeral file in a temporary directory.  Return a file
        // descriptor to this file on success, and an invalid file descriptor
        // otherwise.  An ephemeral file has all of the characteristics of a
        // temporary file, but is guaranteed to be removed when the current
        // process exits, even abnormally, and may not by accessible by any
        // path.

    static FileDescriptor createTemporaryFile(bsl::string *path);
        // Create a temporary file in a temporary directory.  On success, load
        // the path to the temporary file to the specified 'path' and return a
        // file descriptor to this file.  Otherwise, return an invalid file
        // descriptor and load a valid but unspecified value to the 'path'.  A
        // temporary file is a file that resides in a temporary directory,
        // which the operating system reserves the right to delete the next
        // time the computer reboots.

    static Offset estimateNumBlocks(const char *path);
    static Offset estimateNumBlocks(const bsl::string& path);
        // Return an estimate of the number of physical blocks (sectors on the
        // storage media) that the file system uses to represent the file at
        // the specified 'path'.  Return a non-negative number on success, and
        // a negative number otherwise.  If the 'path' identifies anything
        // other than a regular file, the return value is unspecified.  Note
        // that the operating system, file system, storage media, and storage
        // configuration all affect the estimate this function returns.  The
        // estimate is usually accurate for Unix and Windows platforms with
        // non-esoteric, non-networked file systems backed by disk or
        // solid-state drives.

#if !(defined(BSLS_PLATFORM_OS_WINDOWS) && BSLS_PLATFORM_OS_VER_MAJOR < 6)
    ///Implementation Note
    ///-------------------
    // The Windows implementation of this function requires system functions
    // available in Windows Server 2003, Windows Server 2008, Vista, or later.

    BSLA_MAYBE_UNUSED
    static Offset estimateNumBlocks(FileDescriptor descriptor);
        // Return an estimate of the number of physical blocks (sectors on the
        // storage media) that the file system uses to represent the file that
        // the specified 'descriptor' identifies.  Return a non-negative number
        // on success, and a negative number otherwise.  If 'descriptor'
        // identifies anything other than a regular file, the return value is
        // unspecified.  Note that the operating system, file system, storage
        // media, and storage configuration all affect the estimate this
        // function returns.  The estimate is usually accurate for Unix and
        // Windows platforms with non-esoteric, non-networked file systems
        // backed by disk or solid-state drives.
#endif

    static bdlt::Datetime getMaxFileTime();
        // Return the maximum date and time, in UTC, that the platform can
        // *represent*.  Note that the range and precision of date and time
        // values that a platform can *represent* is a superset of the range
        // and precision that it can actually *store* and *retrieve*.  Just
        // because a file time is less than or equal to the value reported by
        // this function, does not mean that the underlying file system can
        // store the value precisely nor accurately.

    static bdlt::Datetime getMinFileTime();
        // Return the minimum date and time, in UTC, that the platform can
        // *represent*.  Note that the range and precision of date and time
        // values that a platform can *represent* is a superset of the range
        // and precision that it can actually *store* and *retrieve*.  Just
        // because a file time is greater than or equal to the value reported
        // by this function, does not mean that the underlying file system can
        // store the value precisely nor accurately.

    BSLA_MAYBE_UNUSED
    static int getTemporaryFilePath(bsl::string *path);
        // Load a valid path to a likely non-existent file in a temporary
        // directory to the specified 'path'.  Return 0 on success, and a
        // non-zero value otherwise.  The 'path' is not guaranteed to be
        // unique, nor is it guaranteed that a file at the 'path' will not
        // exist by the time this or any other process attempts to create a
        // file at the 'path'.

    BSLA_MAYBE_UNUSED
    static bool isBigtimeSupportAvailable(FileDescriptor fd);
        // Determine whether the filsystem used for the specified 'fd' has
        // support for timestamps beyond that representable by a 32-bit
        // timestamp field (ie beyond the year 2038).  Return 'true' if
        // timestamps beyond 32 bits are supported and 'false' otherwise.  If
        // an error occurs, 'true' is returned.  This is a destructive test in
        // that the modification time of the file referred to by 'fd' may be
        // updated by this function.  Note that unix filesystems typically used
        // to store timestamps in a 32-bit integer representing seconds since
        // 1970, resulting in an upper limit in the year 2038.  Various
        // approaches have been adopted to extend this limit, such as "bigtime"
        // (also referred to elsewhere as "big timestamps") on XFS which
        // extends the limit to 2486: https://lwn.net/Articles/829314/
        // and ext4 which extends the limit to 2446:
        // https://www.kernel.org/doc/html/v5.7/filesystems/ext4/dynamic.html#inode-timestamps

    static bool isValidModificationTime(const bdlt::Datetime& utcTime);
        // Return 'true' if the specified 'utcTime' is neither greater than
        // 'getMaxFileTime()' nor less than 'getMinFileTime()', and return
        // 'false' otherwise.  If this function returns 'true' for a given
        // 'utcTime', that 'utcTime' is said to be a "valid modification time".

    static int modifyTemporaryFile(FileDescriptor fd);
        // Write and flush sample data to the specified 'fd'.  Return 0 on
        // success, and a non-zero value otherwise.

    static int setFileSize(FileDescriptor descriptor, Offset numBytes);
        // Set the size of the file the specified 'descriptor' identifies to
        // the specified 'numBytes' number of bytes.  Return 0 on success, and
        // a non-zero value otherwise.  In the event of failure, the size and
        // content of the file are unspecified.  The behavior is undefined if
        // 'numBytes' is less than 0.

    static int setLastModificationTime(FileDescriptor        descriptor,
                                       const bdlt::Datetime& utcTime);
        // Set the last modification time of the file indicated by the
        // specified 'descriptor' to the specified 'utcTime'.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless the 'utcTime' is a valid modification time.

    BSLA_MAYBE_UNUSED
    static int setLastModificationTimeIfValid(FileDescriptor        descriptor,
                                              const bdlt::Datetime& utcTime);
        // Set the last modification time of the file indicated by the
        // specified 'descriptor' to the specified 'utcTime' if 'utcTime' is a
        // valid modification time.  Return 0 on success, and a non-zero value
        // otherwise.
};

#if defined(BSLS_PLATFORM_OS_UNIX)

                        // ===========================
                        // struct TestUtil_UnixImpUtil
                        // ===========================

struct TestUtil_UnixImpUtil {
    // This testing-only utility 'struct' provides a suite of file operations
    // that this test driver uses in its implementation of tests for Unix
    // platforms.

    // TYPES
    typedef bdls::FilesystemUtil::FileDescriptor FileDescriptor;
        // 'FileDescriptor' is an alias for the operating system's native file
        // descriptor / file handle type.

    typedef bdls::FilesystemUtil::Offset Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.

    static const FileDescriptor k_INVALID_FD = -1;
        // 'k_INVALID_FD' is a 'FileDescriptor' value representing no file,
        // which operations that return file descriptors use to indicate error
        // conditions.

    // CLASS METHODS
    static FileDescriptor createEphemeralFile();
        // Create an ephemeral file in a temporary directory.  Return a file
        // descriptor to this file on success, and an invalid file descriptor
        // otherwise.  An ephemeral file has all of the characteristics of a
        // temporary file, but is guaranteed to be removed when the current
        // process exits, even abnormally, and may not by accessible by any
        // path.

    static FileDescriptor createTemporaryFile(bsl::string *path);
        // Create a temporary file in a temporary directory.  On success, load
        // the path to the temporary file to the specified 'path' and return a
        // file descriptor to this file.  Otherwise, return an invalid file
        // descriptor and load a valid but unspecified value to the 'path'.  A
        // temporary file is a file that resides in a temporary directory,
        // which the operating system reserves the right to delete the next
        // time the computer reboots.

    static bdlt::Datetime convertEpochOffsetToDatetime(time_t seconds,
                                                       long   nanoseconds);
        // Return the time point defined by the specified 'seconds' and
        // (sub-second) 'nanoseconds' since the Unix epoch in Coordinated
        // Universal Time, as a 'bdlt::Datetime'.

    static bool epochOffsetIsConvertibleToDatetime(
                                     time_t seconds,
                                     long   nanoseconds) BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the time point defined by the specified 'seconds'
        // and (sub-second) 'nanoseconds' since the Unix epoch in Coordinated
        // Universal Time is within the representable date-and-time range of
        // 'bdlt::Datetime', and return 'false' otherwise.

    static bdlt::Datetime getMaxFileTime();
        // Return the maximum date and time, in UTC, that the platform can
        // *represent*.  Note that the range and precision of date and time
        // values that a platform can *represent* is a superset of the range
        // and precision that it can actually *store* and *retrieve*.  Just
        // because a file time is less than or equal to the value reported by
        // this function, does not mean that the underlying file system can
        // store the value precisely nor accurately.

    static bdlt::Datetime getMinFileTime();
        // Return the minimum date and time, in UTC, that the platform can
        // *represent*.  Note that the range and precision of date and time
        // values that a platform can *represent* is a superset of the range
        // and precision that it can actually *store* and *retrieve*.  Just
        // because a file time is greater than or equal to the value reported
        // by this function, does not mean that the underlying file system can
        // store the value precisely nor accurately.

    static int getTemporaryFilePath(bsl::string *path);
        // Load a valid path to a likely non-existent file in a temporary
        // directory to the specified 'path'.  Return 0 on success, and a
        // non-zero value otherwise.  The 'path' is not guaranteed to be
        // unique, nor is it guaranteed that a file at the 'path' will not
        // exist by the time this or any other process attempts to create a
        // file at the 'path'.

    static bool isBigtimeSupportAvailable(FileDescriptor fd);
        // Determine whether the filsystem used for the specified 'fd' has
        // support for timestamps beyond that representable by a 32-bit
        // timestamp field (ie beyond the year 2038).  Return 'true' if
        // timestamps beyond 32 bits are supported and 'false' otherwise.  If
        // an error occurs, 'true' is returned.  This is a destructive test in
        // that the modification time of the file referred to by 'fd' may be
        // updated by this function.  Note that unix filesystems typically used
        // to store timestamps in a 32-bit integer representing seconds since
        // 1970, resulting in an upper limit in the year 2038.  Various
        // approaches have been adopted to extend this limit, such as "bigtime"
        // (also referred to elsewhere as "big timestamps") on XFS which
        // extends the limit to 2486: https://lwn.net/Articles/829314/
        // and ext4 which extends the limit to 2446:
        // https://www.kernel.org/doc/html/v5.7/filesystems/ext4/dynamic.html#inode-timestamps

    static int modifyTemporaryFile(FileDescriptor fd);
        // Write and flush sample data to the specified 'fd'.  Return 0 on
        // success, and a non-zero value otherwise.

    static int setLastModificationTime(int                   fileDescriptor,
                                       const bdlt::Datetime& utcTime);
        // Set the last modification time of the file indicated by the
        // specified 'descriptor' to the specified 'utcTime'.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless the 'utcTime' is a valid modification time.
};

#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

                // ============================================
                // struct TestUtil_WindowsTempFileCloseBehavior
                // ============================================

struct TestUtil_WindowsTempFileCloseBehavior {
    // This 'struct' provides a namespace for enumerating a set of options for
    // controlling the behavior of a file when open handles to it are closed.

    // TYPES
    enum Enum {
        e_RETAIN_ON_CLOSE,
            // option to *not* automatically delete a file when all of its open
            // descriptors are closed

        e_REMOVE_ON_CLOSE
            // option to automatically delete a file whwen all of its open
            // descriptors are closed
    };
};

                       // ==============================
                       // struct TestUtil_WindowsImpUtil
                       // ==============================

struct TestUtil_WindowsImpUtil {
    // This testing-only utility 'struct' provides a suite of file operations
    // that this test driver uses in its implementation of tests for Windows
    // platforms.

    // TYPES
    typedef bdls::FilesystemUtil::FileDescriptor FileDescriptor;
        // 'FileDescriptor' is an alias for the operating system's native file
        // descriptor / file handle type.

    typedef bdls::FilesystemUtil::Offset Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.

    static const FileDescriptor k_INVALID_FD;
        // 'k_INVALID_FD' is a 'FileDescriptor' value representing no file,
        // which operations that return file descriptors use to indicate error
        // conditions.

  private:
    // PRIVATE TYPES
    typedef TestUtil_WindowsTempFileCloseBehavior TempFileCloseBehavior;
        // 'TempFileCloseBehavior' is an alias to a namespace for enumerating a
        // set of options for controlling the behavior of a file when open
        // handles to it are closed.

    // PRIVATE CLASS METHODS
    static FileDescriptor createTemporaryFile(
                                   bsl::string                 *path,
                                   TempFileCloseBehavior::Enum  closeBehavior);
        // Create a temporary file in a temporary directory.  If the specified
        // 'closeBehavior' is 'TempFileCloseBehavior::e_REMOVE_ON_CLOSE',
        // guarantee that the file will be removed when all descriptors to the
        // file are closed or this process exits, even abnormally, whichever
        // happens first.  If 'closeBehavior' is any other value, do not
        // automatically remove the file.  On success, load the path to the
        // temporary file to the specified 'path' and return a file descriptor
        // to this file.  Otherwise, return an invalid file descriptor and load
        // a valid but unspecified value to the 'path'.  A temporary file is a
        // file that resides in a temporary directory, which the operating
        // system reserves the right to delete the next time the computer
        // reboots.

  public:
    // PUBLIC CLASS DATA
    static const bsls::Types::Int64 k_NANOSECONDS_PER_WINDOWS_TICK = 100;

    static const bsls::Types::Int64 k_WINDOWS_TICKS_PER_MICROSECOND =
                           bdlt::TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND /
                           k_NANOSECONDS_PER_WINDOWS_TICK;

    // CLASS METHODS
    static FileDescriptor createEphemeralFile();
        // Create an ephemeral file in a temporary directory.  Return a file
        // descriptor to this file on success, and an invalid file descriptor
        // otherwise.  An ephemeral file has all of the characteristics of a
        // temporary file, but is guaranteed to be removed when the current
        // process exits, even abnormally, and may not by accessible by any
        // path.

    static FileDescriptor createTemporaryFile(bsl::string *path);
        // Create a temporary file in a temporary directory.  On success, load
        // the path to the temporary file to the specified 'path' and return a
        // file descriptor to this file.  Otherwise, return an invalid file
        // descriptor and load a valid but unspecified value to the 'path'.  A
        // temporary file is a file that resides in a temporary directory,
        // which the operating system reserves the right to delete the next
        // time the computer reboots.

    static int getTemporaryFilePath(bsl::string *path);
        // Load a valid path to a likely non-existent file in a temporary
        // directory to the specified 'path'.  Return 0 on success, and a
        // non-zero value otherwise.  The 'path' is not guaranteed to be
        // unique, nor is it guaranteed that a file at the 'path' will not
        // exist by the time this or any other process attempts to create a
        // file at the 'path'.

    static bool isBigtimeSupportAvailable(FileDescriptor fd);
        // Determine whether the filsystem used for the specified 'fd' has
        // support for timestamps beyond that representable by a 32-bit
        // timestamp field (ie beyond the year 2038).  Return 'true' if
        // timestamps beyond 32 bits are supported and 'false' otherwise.  If
        // an error occurs, 'true' is returned.  This is a destructive test in
        // that the modification time of the file referred to by 'fd' may be
        // updated by this function.  Note that unix filesystems typically used
        // to store timestamps in a 32-bit integer representing seconds since
        // 1970, resulting in an upper limit in the year 2038.  Various
        // approaches have been adopted to extend this limit, such as "bigtime"
        // (also referred to elsewhere as "big timestamps") on XFS which
        // extends the limit to 2486: https://lwn.net/Articles/829314/
        // and ext4 which extends the limit to 2446:
        // https://www.kernel.org/doc/html/v5.7/filesystems/ext4/dynamic.html#inode-timestamps

    static int modifyTemporaryFile(FileDescriptor fd);
        // Write and flush sample data to the specified 'fd'.  Return 0 on
        // success, and a non-zero value otherwise.
};

#endif

                       // ==============================
                       // class FileDescriptorCloseGuard
                       // ==============================

class FileDescriptorCloseGuard {
    // This class provides a guard that closes a file descriptor on
    // destruction.

  public:
    // TYPES
    typedef bdls::FilesystemUtil::FileDescriptor FileDescriptor;
        // 'FileDescriptor' is an alias for the operating system's native file
        // descriptor / file handle type.

  private:
    // DATA
    FileDescriptor d_fileDescriptor;
        // file descriptor to close on destruction

  private:
    // NOT IMPLEMENTED
    FileDescriptorCloseGuard(const FileDescriptorCloseGuard&);
    FileDescriptorCloseGuard& operator=(const FileDescriptorCloseGuard&);

  public:
    // CREATORS
    explicit FileDescriptorCloseGuard(FileDescriptor descriptor);
        // Create a 'FileDescriptorCloseGuard' object that closes the specified
        // 'descriptor' on destruction, which it is said to "guard".

    ~FileDescriptorCloseGuard();
        // Close the 'descriptor' supplied to this object on construction, if
        // this object is guarding it, and destroy this object.

    // MANIPULATORS
    int closeAndRelease();
        // Close the 'descriptor' supplied to this object on construction and
        // no longer guard it.  Return 0 on success, and non-zero otherwise.
};

                             // =================
                             // class RemoveGuard
                             // =================

class RemoveGuard {
    // This class provides a guard that removes a named file from the file
    // system on destruction.

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;
        // 'allocator_type' is an alias to the type of allocator that supplies
        // memory to 'RemoveGuard' objects.

  private:
    // DATA
    bsl::string d_path;
        // path to the regular file to remove on destruction

  private:
    // NOT IMPLEMENTED
    RemoveGuard(const RemoveGuard&);
    RemoveGuard& operator=(const RemoveGuard&);

  public:
    // CREATORS
    explicit RemoveGuard(const bsl::string_view& path,
                         const allocator_type&   allocator = allocator_type());
        // Create a 'RemoveGuard' object that removes the file at the specified
        // 'path' on destruction.  Optionally specify an 'allocator' used to
        // supply memory; otherwise, the default allocator is used.

    ~RemoveGuard();
        // Remove the file at the 'path' supplied to this object on
        // construction, and destroy this object.
};

// ============================================================================
//                          DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

                              // ---------------
                              // struct TestUtil
                              // ---------------

// CLASS METHODS

///Implementation Note
///-------------------
// This test driver defines the following class methods out of order because
// they do not depend on any features of the underlying platform.

bool TestUtil::isValidModificationTime(const bdlt::Datetime& utcTime)
{
    bdlt::Datetime normalizedUtcTime(utcTime);
    normalizedUtcTime.addTime(0);

    return getMaxFileTime() >= normalizedUtcTime &&
           getMinFileTime() <= normalizedUtcTime;
}

int TestUtil::setLastModificationTimeIfValid(FileDescriptor        descriptor,
                                             const bdlt::Datetime& utcTime)
{
    if (!isValidModificationTime(utcTime)) {
        return -1;                                                    // RETURN
    }

    return setLastModificationTime(descriptor, utcTime);
}

// This test driver defines the following class methods in order, but grouped
// according to the platform for which they provide an implementation.

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
 || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)

TestUtil::FileDescriptor TestUtil::createEphemeralFile()
{
    return TestUtil_UnixImpUtil::createEphemeralFile();
}

TestUtil::FileDescriptor TestUtil::createTemporaryFile(bsl::string *path)
{
    return TestUtil_UnixImpUtil::createTemporaryFile(path);
}

TestUtil::Offset TestUtil::estimateNumBlocks(const char *path)
{
    struct stat buf;
    int rc = ::stat(path, &buf);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return buf.st_blocks;
}

TestUtil::Offset TestUtil::estimateNumBlocks(const bsl::string& path)
{
    return estimateNumBlocks(path.c_str());
}

TestUtil::Offset TestUtil::estimateNumBlocks(FileDescriptor descriptor)
{
    struct stat buf;
    int rc = ::fstat(descriptor, &buf);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return buf.st_blocks;
}

bdlt::Datetime TestUtil::getMaxFileTime()
{
    return TestUtil_UnixImpUtil::getMaxFileTime();
}

bdlt::Datetime TestUtil::getMinFileTime()
{
    return TestUtil_UnixImpUtil::getMinFileTime();
}

int TestUtil::getTemporaryFilePath(bsl::string *path)
{
    return TestUtil_UnixImpUtil::getTemporaryFilePath(path);
}

bool TestUtil::isBigtimeSupportAvailable(
                                           TestUtil::FileDescriptor fd)
{
    return TestUtil_UnixImpUtil::isBigtimeSupportAvailable(fd);
}

int TestUtil::modifyTemporaryFile(TestUtil::FileDescriptor fd)
{
    return TestUtil_UnixImpUtil::modifyTemporaryFile(fd);
}

int TestUtil::setFileSize(FileDescriptor descriptor, Offset numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    int rc = ::ftruncate(descriptor, numBytes);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int TestUtil::setLastModificationTime(FileDescriptor        descriptor,
                                      const bdlt::Datetime& utcTime)
{
    BSLS_ASSERT(utcTime >= TestUtil_UnixImpUtil::getMinFileTime());
    BSLS_ASSERT(utcTime <= TestUtil_UnixImpUtil::getMaxFileTime());

    return TestUtil_UnixImpUtil::setLastModificationTime(descriptor, utcTime);
}

#elif defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)

TestUtil::FileDescriptor TestUtil::createEphemeralFile()
{
    return TestUtil_UnixImpUtil::createEphemeralFile();
}

TestUtil::FileDescriptor TestUtil::createTemporaryFile(bsl::string *path)
{
    return TestUtil_UnixImpUtil::createTemporaryFile(path);
}

TestUtil::Offset TestUtil::estimateNumBlocks(const char *path)
{
    struct stat64 buf;
    int rc = ::stat64(path, &buf);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return buf.st_blocks;
}

TestUtil::Offset TestUtil::estimateNumBlocks(const bsl::string& path)
{
    return estimateNumBlocks(path.c_str());
}

TestUtil::Offset TestUtil::estimateNumBlocks(FileDescriptor descriptor)
{
    struct stat64 buf;
    int rc = ::fstat64(descriptor, &buf);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return buf.st_blocks;
}

bdlt::Datetime TestUtil::getMaxFileTime()
{
    return TestUtil_UnixImpUtil::getMaxFileTime();
}

bdlt::Datetime TestUtil::getMinFileTime()
{
    return TestUtil_UnixImpUtil::getMinFileTime();
}

int TestUtil::getTemporaryFilePath(bsl::string *path)
{
    return TestUtil_UnixImpUtil::getTemporaryFilePath(path);
}

bool TestUtil::isBigtimeSupportAvailable(
                                           TestUtil::FileDescriptor fd)
{
    return TestUtil_UnixImpUtil::isBigtimeSupportAvailable(fd);
}

int TestUtil::modifyTemporaryFile(TestUtil::FileDescriptor fd)
{
    return TestUtil_UnixImpUtil::modifyTemporaryFile(fd);
}

int TestUtil::setFileSize(FileDescriptor descriptor, Offset numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    int rc = ::ftruncate64(descriptor, numBytes);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int TestUtil::setLastModificationTime(FileDescriptor        descriptor,
                                      const bdlt::Datetime& utcTime)
{
    BSLS_ASSERT(utcTime >= TestUtil_UnixImpUtil::getMinFileTime());
    BSLS_ASSERT(utcTime <= TestUtil_UnixImpUtil::getMaxFileTime());
    return TestUtil_UnixImpUtil::setLastModificationTime(descriptor, utcTime);
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

TestUtil::FileDescriptor TestUtil::createEphemeralFile()
{
    return TestUtil_WindowsImpUtil::createEphemeralFile();
}

TestUtil::FileDescriptor TestUtil::createTemporaryFile(bsl::string *path)
{
    return TestUtil_WindowsImpUtil::createTemporaryFile(path);
}

TestUtil::Offset TestUtil::estimateNumBlocks(const char *path)
{
    bsl::wstring utf16Path;

    const int charConvertStatus =
        bdlde::CharConvertUtf16::utf8ToUtf16(&utf16Path, path);
    if (0 != charConvertStatus) {
        return -1;                                                    // RETURN
    }

    wchar_t volumePathName[MAX_PATH];
    const BOOL getVolumePathNameStatus =
        ::GetVolumePathNameW(utf16Path.c_str(), volumePathName, MAX_PATH);
    if (!getVolumePathNameStatus) {
        return -1;                                                    // RETURN
    }

    DWORD ignoredSectorsPerCluster;
    DWORD bytesPerSector;
    DWORD ignoredNumberOfFreeClusters;
    DWORD ignoredTotalNumberOfClusters;
    const BOOL getDiskFreeSpaceStatus =
        ::GetDiskFreeSpaceW(volumePathName,
                            &ignoredSectorsPerCluster,
                            &bytesPerSector,
                            &ignoredNumberOfFreeClusters,
                            &ignoredTotalNumberOfClusters);
    if (!getDiskFreeSpaceStatus) {
        return -1;                                                    // RETURN
    }

    DWORD dwFileSizeHigh32;
    const DWORD dwFileSizeLow32 =
        ::GetCompressedFileSizeW(utf16Path.c_str(), &dwFileSizeHigh32);

    if (INVALID_FILE_SIZE == dwFileSizeLow32 && NO_ERROR != ::GetLastError()) {
        return -1;                                                    // RETURN
    }

    const ULONG64 ul64FileSizeHigh32 = static_cast<ULONG64>(dwFileSizeHigh32);
    const ULONG64 ul64FileSizeLow32  = static_cast<ULONG64>(dwFileSizeLow32);
    const ULONG64 ul64FileSize =
        (ul64FileSizeHigh32 << 32) | ul64FileSizeLow32;

    const ULONG64 ul64BytesPerSector = static_cast<ULONG64>(bytesPerSector);

    const ULONG64 ul64FileNumSectors =
        (ul64FileSize / ul64BytesPerSector) +
        (ul64FileSize % ul64BytesPerSector == 0 ? 0 : 1);

    return ul64FileNumSectors;
}

TestUtil::Offset TestUtil::estimateNumBlocks(const bsl::string& path)
{
    return estimateNumBlocks(path.c_str());
}

#if BSLS_PLATFORM_OS_VER_MAJOR >= 6
TestUtil::Offset TestUtil::estimateNumBlocks(FileDescriptor descriptor)
{
    FILE_STANDARD_INFO standardFileInfo;
    const BOOL         getFileInformationStatus =
        ::GetFileInformationByHandleEx(descriptor,
                                       FileStandardInfo,
                                       &standardFileInfo,
                                       sizeof(standardFileInfo));
    if (!getFileInformationStatus) {
        return -1;                                                    // RETURN
    }

    const ULONG64 ul64NumBlocksLow32 =
        static_cast<ULONG64>(standardFileInfo.AllocationSize.LowPart);
    const ULONG64 ul64NumBlocksHigh32 =
        static_cast<ULONG64>(standardFileInfo.AllocationSize.HighPart);

    const ULONG64 ul64NumBlocks =
        (ul64NumBlocksHigh32 << 32) | ul64NumBlocksLow32;

    return ul64NumBlocks;
}
#endif

bdlt::Datetime TestUtil::getMinFileTime()
{
    return bdlt::Datetime(1, 1, 1);
}

bdlt::Datetime TestUtil::getMaxFileTime()
{
    return bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 999);
}

int TestUtil::getTemporaryFilePath(bsl::string *path)
{
    return TestUtil_WindowsImpUtil::getTemporaryFilePath(path);
}

bool TestUtil::isBigtimeSupportAvailable(
                                           TestUtil::FileDescriptor fd)
{
    return TestUtil_WindowsImpUtil::isBigtimeSupportAvailable(fd);
}

int TestUtil::modifyTemporaryFile(TestUtil::FileDescriptor fd)
{
    return TestUtil_WindowsImpUtil::modifyTemporaryFile(fd);
}

int TestUtil::setFileSize(FileDescriptor descriptor, Offset numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    const ULONG64 ul64NumBytes64     = static_cast<ULONG64>(numBytes);
    const ULONG64 ul64NumBytesHigh32 = (ul64NumBytes64 >> 32) & 0xFFFFFFFF;
    const ULONG64 ul64NumBytesLow32  = ul64NumBytes64 & 0xFFFFFFFF;

    LONG       lNumBytesHigh32 = static_cast<LONG>(ul64NumBytesHigh32);
    const LONG lNumBytesLow32  = static_cast<LONG>(ul64NumBytesLow32);

    const DWORD setFilePointerStatus = ::SetFilePointer(
        descriptor, lNumBytesLow32, &lNumBytesHigh32, FILE_BEGIN);

    if (INVALID_SET_FILE_POINTER == setFilePointerStatus &&
        NO_ERROR != ::GetLastError()) {
        return -1;                                                    // RETURN
    }

    const LONG64 l64NewFilePosition =
        static_cast<LONG64>(
            (static_cast<ULONG64>(setFilePointerStatus) << 32)
            | static_cast<ULONG64>(static_cast<LONG64>(lNumBytesHigh32))
        );

    if (l64NewFilePosition != numBytes) {
        return -1;                                                    // RETURN
    }

    const BOOL setEndOfFileStatus = ::SetEndOfFile(descriptor);
    if (!setEndOfFileStatus) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int TestUtil::setLastModificationTime(FileDescriptor        descriptor,
                                      const bdlt::Datetime& utcTime)
{
    const SYSTEMTIME utcSystemTime = {
        static_cast<WORD>(utcTime.year()),
        static_cast<WORD>(utcTime.month()),
        static_cast<WORD>(utcTime.dayOfWeek() - 1),
        static_cast<WORD>(utcTime.day()),
        static_cast<WORD>(utcTime.hour() == 24 ? 0 : utcTime.hour()),
        static_cast<WORD>(utcTime.minute()),
        static_cast<WORD>(utcTime.second()),
        static_cast<WORD>(utcTime.millisecond())
    };

    FILETIME utcFileTime;
    const BOOL systemTimeToFileTimeStatus =
        ::SystemTimeToFileTime(&utcSystemTime, &utcFileTime);
    if (!systemTimeToFileTimeStatus) {
        return -1;                                                    // RETURN
    }

    // Copy the individual parts per the Microsoft recommendation at
    // https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
    ULARGE_INTEGER utcFileTimeInTicks;
    utcFileTimeInTicks.HighPart = utcFileTime.dwHighDateTime;
    utcFileTimeInTicks.LowPart  = utcFileTime.dwLowDateTime;

    utcFileTimeInTicks.QuadPart +=
        utcTime.microsecond() *
        TestUtil_WindowsImpUtil::k_WINDOWS_TICKS_PER_MICROSECOND;

    utcFileTime.dwHighDateTime = utcFileTimeInTicks.HighPart;
    utcFileTime.dwLowDateTime  = utcFileTimeInTicks.LowPart;

    static const FILETIME *const s_DO_NOT_UPDATE_CREATION_TIME = 0;
    static const FILETIME *const s_DO_NOT_UPDATE_ACCESS_TIME = 0;

    const BOOL setFileTimeStatus =
        ::SetFileTime(descriptor,
                      s_DO_NOT_UPDATE_CREATION_TIME,
                      s_DO_NOT_UPDATE_ACCESS_TIME,
                      const_cast<const FILETIME *>(&utcFileTime));
    if (!setFileTimeStatus) {
        return -1;                                                    // RETURN
    }

    return 0;
}

#endif

#if defined(BSLS_PLATFORM_OS_UNIX)

                        // ---------------------------
                        // struct TestUtil_UnixImpUtil
                        // ---------------------------

///Implementation Note
///--------------------
// The following function implementations are valid on Unix systems in both
// 32-bit and 64-bit compilation modes.  In addition, they handle both 32-bit
// and 64-bit 'time_t' representations, as well as all possible numeric ranges
// of 'long' on the ILP32, LLP64, and LP64 data models.

// CLASS METHODS
bdlt::Datetime TestUtil_UnixImpUtil::convertEpochOffsetToDatetime(
                                                            time_t seconds,
                                                            long   nanoseconds)
{
    BSLS_ASSERT(epochOffsetIsConvertibleToDatetime(seconds, nanoseconds));

    const bsls::TimeInterval maxTimeSinceEpoch(
        static_cast<bsls::Types::Int64>(seconds),
        static_cast<int>(nanoseconds));

    return bdlt::EpochUtil::convertFromTimeInterval(maxTimeSinceEpoch);
}

TestUtil_UnixImpUtil::FileDescriptor
TestUtil_UnixImpUtil::createEphemeralFile()
{
    bsl::string path;
    FileDescriptor fileDescriptor = createTemporaryFile(&path);
    if (k_INVALID_FD == fileDescriptor) {
        return k_INVALID_FD;
    }

    int rc = ::unlink(path.c_str());
    if (0 != rc) {
        return k_INVALID_FD;                                          // RETURN
    }

    return fileDescriptor;
}

TestUtil_UnixImpUtil::FileDescriptor
TestUtil_UnixImpUtil::createTemporaryFile(bsl::string *path)
{
    bsl::string pathValue;
    int rc = getTemporaryFilePath(&pathValue);
    if (0 != rc) {
        return k_INVALID_FD;                                          // RETURN
    }

    // IEEE Std 1003.1 (POSIX) specifies 'mkstemp' requires the last 6
    // characters of a path-name passed to 'mkstemp' be "X", which it
    // overwrites with characters that guarantee a unique name for the
    // to-be-created file in the directory of the path-name.  Note that
    // unique-name generation can fail, in which case 'mkstemp' returns an
    // invalid file descriptor.

    pathValue += "-XXXXXX";

    const int fileDescriptor = ::mkstemp(pathValue.data());
    if (-1 == fileDescriptor) {
        return k_INVALID_FD;                                          // RETURN
    }

    *path = pathValue;
    return fileDescriptor;
}

bool TestUtil_UnixImpUtil::epochOffsetIsConvertibleToDatetime(
                                      time_t seconds,
                                      long   nanoseconds) BSLS_KEYWORD_NOEXCEPT
{
    const bdlt::Datetime     maxDatetime(9999, 12, 31, 23, 59, 59, 999, 999);
    const bsls::TimeInterval maxDatetimeAsInterval =
        bdlt::EpochUtil::convertToTimeInterval(maxDatetime);

    const bdlt::Datetime     minDatetime(1, 1, 1);
    const bsls::TimeInterval minDatetimeAsInterval =
        bdlt::EpochUtil::convertToTimeInterval(minDatetime);

    typedef bsls::Types::Int64 Int64;
    const Int64 maxSeconds = maxDatetimeAsInterval.seconds();
    const Int64 minSeconds = minDatetimeAsInterval.seconds();

    const bool signsAreEqual = (seconds >= 0 && nanoseconds >= 0) ||
                               (seconds <= 0 && nanoseconds <= 0);

    return nanoseconds >= -999999999
        && nanoseconds <=  999999999
        && seconds     >= minSeconds
        && seconds     <= maxSeconds
        && signsAreEqual;
}

bdlt::Datetime TestUtil_UnixImpUtil::getMaxFileTime()
{
    const time_t         maxSeconds     = bsl::numeric_limits<time_t>::max();
    const long           maxNanoseconds = 0;
    const bdlt::Datetime maxDatetime(9999, 12, 31, 23, 59, 59, 999, 999);

    return epochOffsetIsConvertibleToDatetime(maxSeconds, maxNanoseconds)
               ? convertEpochOffsetToDatetime(maxSeconds, maxNanoseconds)
               : maxDatetime;
}

bdlt::Datetime TestUtil_UnixImpUtil::getMinFileTime()
{
    const time_t         minSeconds     = bsl::numeric_limits<time_t>::min();
    const long           minNanoseconds = 0;
    const bdlt::Datetime minDatetime(1, 1, 1);

    return epochOffsetIsConvertibleToDatetime(minSeconds, minNanoseconds)
               ? convertEpochOffsetToDatetime(minSeconds, minNanoseconds)
               : minDatetime;
}

int TestUtil_UnixImpUtil::getTemporaryFilePath(bsl::string *path)
{
    bsl::string result;
    int rc = bdls::FilesystemUtil::getSystemTemporaryDirectory(&result);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    bdlb::Guid guid;
    bdlb::GuidUtil::generate(&guid);

    bsl::string baseName;
    bdlb::GuidUtil::guidToString(&baseName, guid);

    rc = bdls::PathUtil::appendIfValid(&result, baseName);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    *path = result;
    return 0;
}

bool TestUtil_UnixImpUtil::isBigtimeSupportAvailable(
                               TestUtil_UnixImpUtil::FileDescriptor fd)
{
    // 'tm32' and 'smallModTime' will hold the maximum possible date and time
    // that can be stored in a 32-bit timestamp.
    bsl::tm tm32 = bsl::tm();     // zero initialise
    tm32.tm_year  = 2038 - 1900;  // 2038
    tm32.tm_mon   = 1 - 1;        // January
    tm32.tm_mday  = 19;           // 19th
    tm32.tm_hour  = 3;            // 03:00
    tm32.tm_min   = 14;           // 00:14
    tm32.tm_sec   = 7;            // 03:14:07
    tm32.tm_isdst = 0;            // Not daylight saving

    bsl::time_t smallModTime = bsl::mktime(&tm32);

    // 'tmw' and 'writeModTime' represent a date and time that is too large to
    // store in a 32-bit filesystem timestamp, but can be stored if "big
    // timestamp" support is available.
    bsl::tm tmw = bsl::tm();     // zero initialise
    tmw.tm_year  = 2200 - 1900;  // 2020
    tmw.tm_mon   = 1 - 1;        // January
    tmw.tm_mday  = 1;            // 1st
    tmw.tm_hour  = 0;            // 00:00
    tmw.tm_min   = 0;            // 00:00
    tmw.tm_isdst = 0;            // Not daylight saving

    bsl::time_t writeModTime = bsl::mktime(&tmw);

    // Update the file modification time to 'writeModTime'.
    struct timespec times[2] = {};

    struct timespec& lastAccessTime = times[0];
    lastAccessTime.tv_sec           = 0;
    lastAccessTime.tv_nsec          = UTIME_OMIT;

    struct timespec& lastModificationTime = times[1];
    lastModificationTime.tv_sec           = writeModTime;
    lastModificationTime.tv_nsec          = 0L;

    int rc = ::futimens(fd, times);
    if (0 != rc)
        return true;                                                  // RETURN

    // Read back the file modification time.
    struct ::stat statResult;

    rc = fstat(fd, &statResult);
    if (0 != rc)
        return true;                                                  // RETURN

    bsl::time_t readModTime = statResult.st_mtime;

    return (readModTime != smallModTime);
}

int TestUtil_UnixImpUtil::modifyTemporaryFile(
                                       TestUtil_UnixImpUtil::FileDescriptor fd)
{
    int rc = Obj::write(fd, "hello world\n", strlen("hello world\n"));
    if (strlen("hello world\n") != rc) {
        return -1;                                                    // RETURN
    }
    rc = fsync(fd);
    return rc;
}


int TestUtil_UnixImpUtil::setLastModificationTime(
                                          int                   fileDescriptor,
                                          const bdlt::Datetime& utcTime)
{
    BSLS_ASSERT(utcTime >= getMinFileTime());
    BSLS_ASSERT(utcTime <= getMaxFileTime());

    const bsls::TimeInterval utcTimeSinceEpoch =
        bdlt::EpochUtil::convertToTimeInterval(utcTime);

    const bsls::Types::Int64 utcSeconds = utcTimeSinceEpoch.seconds();
    BSLS_ASSERT(utcSeconds >= bsl::numeric_limits<time_t>::min());
    BSLS_ASSERT(utcSeconds <= bsl::numeric_limits<time_t>::max());

    const int utcNanoseconds = utcTimeSinceEpoch.nanoseconds();
    BSLS_ASSERT(utcNanoseconds >= -999999999LL);
    BSLS_ASSERT(utcNanoseconds <=  999999999LL);

    struct timespec times[2] = {};

    struct timespec& lastAccessTime = times[0];
    lastAccessTime.tv_sec           = 0;
    lastAccessTime.tv_nsec          = UTIME_OMIT;

    struct timespec& lastModificationTime = times[1];
    lastModificationTime.tv_sec           = static_cast<time_t>(utcSeconds);
    lastModificationTime.tv_nsec          = static_cast<long>(utcNanoseconds);

    int rc = ::futimens(fileDescriptor, times);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return 0;
}

#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

                       // ------------------------------
                       // struct TestUtil_WindowsImpUtil
                       // ------------------------------

// TYPES
const TestUtil_WindowsImpUtil::FileDescriptor
    TestUtil_WindowsImpUtil::k_INVALID_FD = INVALID_HANDLE_VALUE;

// PRIVATE CLASS METHODS
TestUtil_WindowsImpUtil::FileDescriptor
TestUtil_WindowsImpUtil::createTemporaryFile(
                                    bsl::string                 *path,
                                    TempFileCloseBehavior::Enum  closeBehavior)
{
    bsl::string pathValue;
    int rc = getTemporaryFilePath(&pathValue);
    if (0 != rc) {
        return k_INVALID_FD;                                          // RETURN
    }

    bsl::wstring utf16Path;
    const int    charConvertStatus =
        bdlde::CharConvertUtf16::utf8ToUtf16(&utf16Path, pathValue);
    if (0 != charConvertStatus) {
        return k_INVALID_FD;                                          // RETURN
    }

    static const DWORD                 k_NOT_SHARED             = 0;
    static const HANDLE                k_NO_TEMPLATE_FILE       = 0;
    static const LPSECURITY_ATTRIBUTES k_NO_SECURITY_ATTRIBUTES = 0;

    DWORD deleteOnCloseFlag = 0;
    if (TempFileCloseBehavior::e_REMOVE_ON_CLOSE == closeBehavior) {
        deleteOnCloseFlag = FILE_FLAG_DELETE_ON_CLOSE;
    }

    const HANDLE fileDescriptor =
        ::CreateFileW(utf16Path.c_str(),
                      GENERIC_READ | GENERIC_WRITE,
                      k_NOT_SHARED,
                      k_NO_SECURITY_ATTRIBUTES,
                      CREATE_NEW,
                      FILE_ATTRIBUTE_TEMPORARY | deleteOnCloseFlag,
                      k_NO_TEMPLATE_FILE);

    if (INVALID_HANDLE_VALUE == fileDescriptor) {
        return k_INVALID_FD;                                          // RETURN
    }

    *path = pathValue;
    return fileDescriptor;
}

// CLASS METHODS
TestUtil_WindowsImpUtil::FileDescriptor
TestUtil_WindowsImpUtil::createEphemeralFile()
{
    bsl::string path;
    return createTemporaryFile(&path,
                               TempFileCloseBehavior::e_REMOVE_ON_CLOSE);
}

TestUtil_WindowsImpUtil::FileDescriptor
TestUtil_WindowsImpUtil::createTemporaryFile(bsl::string *path)
{
    return createTemporaryFile(path, TempFileCloseBehavior::e_RETAIN_ON_CLOSE);
}

int TestUtil_WindowsImpUtil::getTemporaryFilePath(bsl::string *path)
{
    bsl::string result;
    int rc = bdls::FilesystemUtil::getSystemTemporaryDirectory(&result);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    bdlb::Guid guid;
    bdlb::GuidUtil::generate(&guid);

    bsl::string baseName;
    bdlb::GuidUtil::guidToString(&baseName, guid);

    rc = bdls::PathUtil::appendIfValid(&result, baseName);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    *path = result;
    return 0;
}

bool TestUtil_WindowsImpUtil::isBigtimeSupportAvailable(
                            TestUtil_WindowsImpUtil::FileDescriptor fd)
{
    (void) fd;
    // We assume that windows supports timestamps beyond 2038.
    return true;
}

int TestUtil_WindowsImpUtil::modifyTemporaryFile(
                                    TestUtil_WindowsImpUtil::FileDescriptor fd)
{
    int rc = Obj::write(fd, "hello world\n", sizeof("hello world\n"));
    if (sizeof("hello world\n") != rc) {
        return -1;                                                    // RETURN
    }
    rc = ::FlushFileBuffers(fd);
    return (rc != 0) ? 0 : -1;
}

#endif

                       // ------------------------------
                       // class FileDescriptorCloseGuard
                       // ------------------------------

// CREATORS
FileDescriptorCloseGuard::FileDescriptorCloseGuard(FileDescriptor descriptor)
: d_fileDescriptor(descriptor)
{
    BSLS_ASSERT(bdls::FilesystemUtil::k_INVALID_FD != descriptor);
}

FileDescriptorCloseGuard::~FileDescriptorCloseGuard()
{
    if (bdls::FilesystemUtil::k_INVALID_FD == d_fileDescriptor) {
        return;
    }

    int rc = bdls::FilesystemUtil::close(d_fileDescriptor);
    if (0 != rc) {
        bsl::cerr << "Failed to close file descriptor: '" << d_fileDescriptor
                  << "'.\n";
    }
}

// MANIPULATORS
int FileDescriptorCloseGuard::closeAndRelease()
{
    const bdls::FilesystemUtil::FileDescriptor fileDescriptor =
        d_fileDescriptor;
    d_fileDescriptor = bdls::FilesystemUtil::k_INVALID_FD;

    return bdls::FilesystemUtil::close(fileDescriptor);
}

                      // -------------------------------
                      // class FileDescriptorRemoveGuard
                      // -------------------------------

// CREATORS
RemoveGuard::RemoveGuard(const bsl::string_view& path,
                         const allocator_type&   allocator)
: d_path(path, allocator.mechanism())
{
}

RemoveGuard::~RemoveGuard()
{
    int rc = bdls::FilesystemUtil::remove(d_path);
    if (0 != rc) {
        bsl::cerr << "Failed to remove file at path: '" << d_path << "'.\n";
    }
}

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
// Ensures that the following functions in usage example 2 compiles and runs on
// all platforms.
//-----------------------------------------------------------------------------

namespace UsageExample2 {

///Example 2: Using 'bdls::FilesystemUtil::visitPaths'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdls::FilesystemUtil::visitPaths' enables clients to define a functor to
// operate on file paths that match a specified pattern.  In this example, we
// create a function that can be used to filter out files that have a last
// modified time within a particular time frame.
//
// First we define our filtering function:
//..
    void getFilesWithinTimeframe(bsl::vector<bsl::string> *vector,
                                 const char               *item,
                                 const bdlt::Datetime&     start,
                                 const bdlt::Datetime&     end)
    {
        bdlt::Datetime datetime;
        int ret = bdls::FilesystemUtil::getLastModificationTime(&datetime,
                                                                item);

        if (ret) {
            return;                                                   // RETURN
        }

        if (datetime < start || datetime > end) {
            return;                                                   // RETURN
        }

        vector->push_back(item);
    }
//..
// Then, with the help of 'bdls::FilesystemUtil::visitPaths' and
// 'bdlf::BindUtil::bind', we create a function for finding all file paths that
// match a specified pattern and have a last modified time within a specified
// start and end time (both specified as a 'bdlt::Datetime'):
//..
    void findMatchingFilesInTimeframe(bsl::vector<bsl::string> *result,
                                      const char               *pattern,
                                      const bdlt::Datetime&     start,
                                      const bdlt::Datetime&     end)
    {
        result->clear();
        bdls::FilesystemUtil::visitPaths(
                                 pattern,
                                 bdlf::BindUtil::bind(&getFilesWithinTimeframe,
                                                      result,
                                                      bdlf::PlaceHolders::_1,
                                                      start,
                                                      end));
    }
//..

}  // close namespace UsageExample2

// ============================================================================
//                            TEMPLATED TEST CASES
// ----------------------------------------------------------------------------

template <class STRING_TYPE>
void testCase23_createTemporaryDirectory(const char         *typeName,
                                         int                 test,
                                         const bsl::string&  tmpWorkingDir,
                                         int                 verbose,
                                         int                 veryVerbose,
                                         int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir; (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    bsl::string_view prefix = "name_prefix_";
    STRING_TYPE dirName;

    int madeDir = Obj::createTemporaryDirectory(&dirName, prefix);
    ASSERT(madeDir == 0);
    if (madeDir == 0) {
        if (veryVerbose) {
            cout << ":" << dirName << ":\n";
        }
        ASSERT(Obj::isDirectory(dirName));

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose) {
            cout << "DIR PERMISSIONS CHECK SKIPPED ON WINDOWS\n";
        }
#else
        struct stat info;
        memset(&info, 0, sizeof(info));
        ASSERT(0 == ::stat(dirName.c_str(), &info));
        info.st_mode &= 0777;
        ASSERT((S_IRUSR|S_IWUSR|S_IXUSR) == info.st_mode);
        if ((verbose && (S_IRUSR|S_IWUSR|S_IXUSR) != info.st_mode)
                                                      || veryVeryVerbose) {
            cout.flush(); fflush(stdout);
            printf("Temp file permissions 0%o\n",
                                      static_cast<unsigned>(info.st_mode));
            fflush(stdout);
        }
#endif
        ASSERT(0 == Obj::remove(dirName));

        STRING_TYPE dirName2, dirName3;
        int madeDir2 = Obj::createTemporaryDirectory(&dirName2, prefix);
        ASSERT(madeDir2 == 0);
        int madeDir3 = Obj::createTemporaryDirectory(&dirName3, dirName2);
        ASSERT(madeDir3 == 0);
        if (madeDir2 == 0) {
            if (veryVerbose) {
                cout << ":" << dirName2 << ": :" << dirName3 << ":\n";
            }
            ASSERT(Obj::isDirectory(dirName2));
            ASSERT(Obj::isDirectory(dirName3));
            ASSERT(dirName2 != dirName);
            ASSERT(0 == Obj::remove(dirName2));
        }
    }
}

template <class STRING_TYPE>
void testCase22_createTemporaryFile(const char         *typeName,
                                    int                 test,
                                    const bsl::string&  tmpWorkingDir,
                                    int                 verbose,
                                    int                 veryVerbose,
                                    int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir; (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    bsl::string_view prefix = "name_prefix_";
    STRING_TYPE fileName;
    Obj::FileDescriptor fd = Obj::createTemporaryFile(&fileName, prefix);
    if (veryVerbose) {
        cout << ":" << fileName << ":\n";
    }
    ASSERT(fd != Obj::k_INVALID_FD);
    ASSERT(fileName.size() >= prefix.size() + 6);
    ASSERT(prefix == fileName.substr(0, prefix.size()));
    if (fd != Obj::k_INVALID_FD) {
        static const char hello[] = "hello, world\n";
        int wrote = Obj::write(fd, hello, INT_SIZEOF(hello) - 1);
        ASSERT(wrote == sizeof(hello) - 1);
        Obj::close(fd);
        ASSERT(Obj::isRegularFile(fileName));

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose) {
            cout << "FILE PERMISSIONS CHECK SKIPPED ON WINDOWS\n";
        }
#else
        struct stat info;
        memset(&info, 0, sizeof(info));
        ASSERT(0 == ::stat(fileName.c_str(), &info));
        info.st_mode &= 0777;
        ASSERT((S_IRUSR|S_IWUSR) == info.st_mode);
        if ((verbose && (S_IRUSR|S_IWUSR|S_IXUSR) != info.st_mode)
                                                      || veryVeryVerbose) {
            cout.flush(); fflush(stdout);
            printf("Temp file permissions 0%o\n",
                                      static_cast<unsigned>(info.st_mode));
            fflush(stdout);
        }
#endif
        STRING_TYPE fileName2;
        Obj::FileDescriptor fd2 =
                              Obj::createTemporaryFile(&fileName2, prefix);
        if (veryVerbose) {
            cout << ":" << fileName2 << ":\n";
        }
        ASSERT(fd2 != Obj::k_INVALID_FD);
        if (fd2 != Obj::k_INVALID_FD) {
            ASSERT(fileName2 != fileName);
            Obj::close(fd2);
            ASSERT(Obj::isRegularFile(fileName2));
            Obj::remove(fileName2);
        }

        int removedFile = Obj::remove(fileName);
        ASSERT(removedFile == 0);
    }
}

template <class STRING_TYPE>
void testCase21_makeUnsafeTemporaryFilename(
                                           const char         *typeName,
                                           int                 test,
                                           const bsl::string&  tmpWorkingDir,
                                           int                 verbose,
                                           int                 veryVerbose,
                                           int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir; (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    enum { k_NUM_NAMES = 100 };

    const STRING_TYPE prefix = "name_prefix_";
    bsl::deque<STRING_TYPE> names;
    for (unsigned ii = 0; ii < k_NUM_NAMES; ++ii) {
        ASSERT(names.size() == ii);

        STRING_TYPE name;
        Obj::makeUnsafeTemporaryFilename(&name, prefix);

        if (veryVerbose) P(name);

        ASSERT(!bsl::strncmp(name.c_str(),
                             prefix.c_str(),
                             prefix.length()));
        ASSERT(prefix.length() + 6 <= name.length());

        // 'jj' is the index of a name in 'names' made prior to 'name'.

        for (unsigned jj = 0; jj < ii; ++jj) {
            const STRING_TYPE& otherName = names[jj];
            const bool otherNameWasPrevious = jj == ii - 1;

            ASSERTV(name, otherName, otherName != name);
            const bsl::size_t minLen = bsl::min(name.length(),
                                                otherName.length());

            // 'numRandChars' is the number of random characters that both
            // names have.

            const int numRandChars = static_cast<int>(
                                                 minLen - prefix.length());

            // Names created adjacently in sequence must differ in at least
            // 'numRandChars / 2' characters, all names must differ in at
            // least 2 characters.

            const int minNumCharDiffs = otherNameWasPrevious
                                      ? numRandChars / 2
                                      : 2;

            // Iterate 'kk', an index into 'name' and 'otherName', through
            // the random parts of the names.

            bool charDiffEnough = false;    // Did at least one pair of
                                            // corresponding chars differ
                                            // by >= 3?
            int numCharDiffs = 0;           // How many corresponding chars
                                            // differed?
            for (bsl::size_t kk = prefix.length(); kk < minLen; ++kk) {
                const int charDiff    = name[kk] - otherName[kk];
                numCharDiffs   += 0 != charDiff;

                const int absCharDiff = charDiff < 0 ? -charDiff
                                                     :  charDiff;
                charDiffEnough |= 3 <= absCharDiff;
            }
            ASSERTV(minNumCharDiffs, numCharDiffs, name, otherName,
                                          minNumCharDiffs <= numCharDiffs);
            ASSERTV(name, otherName, charDiffEnough);
        }

        names.push_back(name);
    }

    ASSERTV(names.size(), k_NUM_NAMES == names.size());
}

template <class STRING_TYPE>
void testCase10_getFileSize(const char         *typeName,
                            const bsl::string&  tmpWorkingDir,
                            int                 test,
                            int                 verbose,
                            int                 veryVerbose,
                            int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    // Setup by first creating a tmp file

    bsl::string tmpFilename = ::tempFileName(test, "tmp.getFileSizeTest.8");
    STRING_TYPE fileName = tmpFilename;

    if (veryVerbose) P(fileName);
    Obj::FileDescriptor fd = Obj::open(fileName,
                                       Obj::e_OPEN_OR_CREATE,
                                       Obj::e_READ_WRITE);
    ASSERTV(Obj::k_INVALID_FD, fd, Obj::k_INVALID_FD != fd);
    if (Obj::k_INVALID_FD == fd) {
        P(fileName);
    }

    const char buffer[] = "testing";
    int bytes           = INT_SIZEOF(buffer);

    Obj::write(fd, buffer, bytes);
    Obj::close(fd);

    // Concern 1.1

    {
        if (veryVerbose) cout << "\n1. Normal file" << endl;

        Obj::Offset off = Obj::getFileSize(fileName);
        LOOP2_ASSERT(bytes, off, bytes == off);

        bsl::string tmpFileName(fileName.data(), fileName.length());
        Obj::Offset off2 = Obj::getFileSize(tmpFileName.c_str());
        LOOP2_ASSERT(bytes, off2, bytes == off2);

        if (veryVerbose) {
            cout << "Expected " << bytes << endl;
            cout << "Actual ";
            P_(off); P(off2)
        }
    }

    // Concern 1.2

    {
        if (veryVerbose) cout << "\n2. Normal directory" << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
        STRING_TYPE dirName("getFileSizeDir");

        // windows directories are 0 sized

        const Obj::Offset EXPECTED = 0;
#else
        STRING_TYPE dirName("/tmp/getFileSizeDir");
#endif

        bsl::string tmpDirName(dirName.data(), dirName.length());
        int ret = Obj::createDirectories(dirName, true);
        ASSERT(0 == ret);

        // On UNIX use 'stat64' ('stat' on cygwin) as an oracle: the file
        // size of a directory depends on the file system.

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
|| defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
        struct stat oracleInfo;
        int rc = ::stat(tmpDirName.c_str(), &oracleInfo);
        ASSERT(0 == rc);

        Obj::Offset EXPECTED = oracleInfo.st_size;
#elif !defined BSLS_PLATFORM_OS_WINDOWS
        struct stat64 oracleInfo;
        int rc = ::stat64(tmpDirName.c_str(), &oracleInfo);
        ASSERT(0 == rc);

        Obj::Offset EXPECTED = oracleInfo.st_size;
#endif

        Obj::Offset off = Obj::getFileSize(dirName);
        LOOP2_ASSERT(EXPECTED, off, EXPECTED == off);

        Obj::Offset off2 = Obj::getFileSize(tmpDirName.c_str());
        LOOP2_ASSERT(EXPECTED, off2, EXPECTED == off2);

        if (veryVerbose) {
            cout << "Expected " << EXPECTED << endl;
            cout << "Actual ";
            P_(off); P(off2)
        }

        Obj::remove(dirName);
    }

#ifndef BSLS_PLATFORM_OS_WINDOWS
    // Concern 1.3

    // This block used to claim that there are no symbolic links on
    // Windows.  However, Windows began supporting symbolic links if the
    // host uses the NTFS file system as of the release of Vista.
    //
    // Still, this test verifies the behavior of 'getFileSize' on a
    // symbolic link only for non-Windows (which, for BDE, means Unix)
    // systems.

    {
        if (veryVerbose) cout << "\n3. Symbolic Links" << endl;

        bsl::string cmd = "ln -s " + bsl::string(fileName) + " testLink";
        if (veryVerbose) P(cmd);
        system(cmd.c_str());

        STRING_TYPE fileName("testLink");
        Obj::Offset off = Obj::getFileSize(fileName);
        ASSERT(bytes == off);

        Obj::Offset off2;
        {
            bsl::string fileName("testLink");
            off2 = Obj::getFileSize(fileName.c_str());
            ASSERT(bytes == off2);
        }

        if (veryVerbose) {
            cout << "Expected " << bytes << endl;
            cout << "Actual ";
            P_(off); P(off2)
        }

        Obj::remove(fileName);
        Obj::remove("testLink");
    }
#endif

    // Concert 1.4

    {
        if (veryVerbose) cout << "\n4. Non existent file" << endl;

        // Use a random name.

        Obj::Offset off = Obj::getFileSize("acasdf");

        ASSERT(-1 == off);
        if (veryVerbose) {
            cout << "Expected -1" << endl;
            cout << "Actual " << off << endl;
        }
    }

    // Concern 1.5

    {
        if (veryVerbose) cout << "\n5. Relative Path" << endl;

        ASSERT(0 == Obj::setWorkingDirectory(".."));

        bsl::string relFileNameStr =
            tmpWorkingDir + PS + bsl::string(fileName);
        STRING_TYPE relFileName = relFileNameStr;

        if (veryVerbose) P(relFileName);

        Obj::Offset off = Obj::getFileSize(relFileName);
        ASSERTV(bytes, off, bytes == off);

        Obj::Offset off2;
        {
            off2 = Obj::getFileSize(relFileNameStr.c_str());
            ASSERTV(bytes, off2, bytes == off2);
        }

        if (veryVerbose) {
            cout << "Expected " << bytes << endl;
            cout << "Actual ";
            P_(off);
            P(off2)
        }

        Obj::remove(relFileName);

        ASSERT(0 == Obj::setWorkingDirectory(tmpWorkingDir));
    }

    // Note that the file having the 'fileName' is removed in the following
    // blocks of this test case because it is no longer needed.

    Obj::remove(fileName);

    // Concern 2

    // The following blocks test the overload of 'getFileSize' that has a
    // 'Obj::FileDescriptor' parameter.

    {
        // This block tests that 'getFileSize' unconditionally returns a
        // negative value if one gives it an invalid file handle.

        const Obj::Offset fileSize = Obj::getFileSize(Obj::k_INVALID_FD);
        ASSERT_EQ(-1, fileSize);
    }

    // The following blocks terminate the test unless they confirm that the
    // host file system supports sparse files.  Subsequent blocks of this
    // test case create large, zero-filled files that may exhaust file
    // systems unless they compress all-zero files.
    {
        bsl::string fileName;
        const Obj::FileDescriptor fd =
            u::TestUtil::createTemporaryFile(&fileName);
        ASSERT_NE(Obj::k_INVALID_FD, fd);
        const u::FileDescriptorCloseGuard closeGuard(fd);
        const u::RemoveGuard              removeGuard(fileName);

        const Obj::Offset fileSize = Obj::getFileSize(fd);
        ASSERT_EQ(0, fileSize);
        if (0 != fileSize) {
            return;                                                   // RETURN
        }

        const Obj::Offset fileNumBlocks =
            u::TestUtil::estimateNumBlocks(fileName);
        ASSERT_GE(1, fileNumBlocks);
        if (1 < fileNumBlocks) {
            return;                                                   // RETURN
        }
    }

    {
        bsl::string fileName;
        const Obj::FileDescriptor fd =
            u::TestUtil::createTemporaryFile(&fileName);
        ASSERT_NE(Obj::k_INVALID_FD, fd);
        const u::FileDescriptorCloseGuard closeGuard(fd);
        const u::RemoveGuard              removeGuard(fileName);

        const Obj::Offset fileSize = Obj::getFileSize(fd);
        ASSERT_EQ(0, fileSize);
        if (0 != fileSize) {
            return;                                                   // RETURN
        }

        static const int k_LARGEST_EXPECTED_BLOCK_SIZE = 8192;

        const int setFileSizeRc = u::TestUtil::setFileSize(
            fd, 2 * k_LARGEST_EXPECTED_BLOCK_SIZE);
        if (0 != setFileSizeRc) {
            return;                                                   // RETURN
        }

        const Obj::Offset newFileSize = Obj::getFileSize(fd);
        ASSERT_EQ(2 * k_LARGEST_EXPECTED_BLOCK_SIZE, newFileSize);

        const Obj::Offset fileNumBlocks =
            u::TestUtil::estimateNumBlocks(fileName);
        ASSERT_GE(1, fileNumBlocks);
        if (1 < fileNumBlocks) {
            return;                                                   // RETURN
        }
    }

    {
        bsl::string fileName;
        const Obj::FileDescriptor fd =
            u::TestUtil::createTemporaryFile(&fileName);
        ASSERT_NE(Obj::k_INVALID_FD, fd);
        const u::FileDescriptorCloseGuard closeGuard(fd);
        const u::RemoveGuard              removeGuard(fileName);

        const Obj::Offset fileSize = Obj::getFileSize(fd);
        ASSERT_EQ(0, fileSize);
        if (0 != fileSize) {
            return;                                                   // RETURN
        }

        static const Obj::Offset k_4_GB_FILE_SIZE =
            4LL * 1024LL * 1024LL * 1024LL;

        const int setFileSizeRc =
            u::TestUtil::setFileSize(fd, k_4_GB_FILE_SIZE);
        if (0 != setFileSizeRc) {
            return;                                                   // RETURN
        }

        const Obj::Offset newFileSize = Obj::getFileSize(fd);
        ASSERT_EQ(k_4_GB_FILE_SIZE, newFileSize);

        const Obj::Offset fileNumBlocks =
            u::TestUtil::estimateNumBlocks(fileName);
        ASSERT_GE(1, fileNumBlocks);
        if (1 < fileNumBlocks) {
            return;                                                   // RETURN
        }
    }

    // Concern 3

    {
        // This block tests that 'FilesystemUtil' measures the sizes of
        // real files correctly, using sparse files from 0 gigabytes to 8
        // gigabytes.  Unfortunately, 'FilesystemUtil::getFileSizeLimit'
        // does not guarantee the file system allows one to create a file
        // as large as it may suggest.  Empirically, our supported
        // operating and file systems permit files at least 8 gigabytes in
        // size.  The test drivers for 'bdls_filesystemutil's subordinate
        // components test 'FilesystemUtil's measurement of files larger
        // than 8 gigabytes using mock operating-system interfaces.

        const struct {
            int d_line;
                // the line number

            Obj::Offset d_fileSize;
                // the size of file to make and then measure
        } DATA[] = {
            // LINE   FILE SIZE
            // ---- ---------------
            { L_   ,           0LL },
            { L_   ,           1LL },
            { L_   ,        1023LL },
            { L_   ,        1024LL },
            { L_   ,        1025LL },
            { L_   ,        8191LL },
            { L_   ,        8192LL },
            { L_   ,        8193LL },
            { L_   ,  0xFFFFFFFELL },
            { L_   ,  0xFFFFFFFFLL },
            { L_   , 0x100000000LL },
            { L_   , 0x100000001LL },
            { L_   , 0x1FFFFFFFFLL },
            { L_   , 0x200000000LL },
            { L_   , 0x200000001LL },
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int         LINE      = DATA[i].d_line;
            const Obj::Offset FILE_SIZE = DATA[i].d_fileSize;

            const Obj::FileDescriptor fd =
                u::TestUtil::createEphemeralFile();
            LOOP_ASSERT_NE(LINE, Obj::k_INVALID_FD, fd);
            if (Obj::k_INVALID_FD == fd) {
                continue;                                       // CONTINUE
            }
            const u::FileDescriptorCloseGuard closeGuard(fd);

            const Obj::Offset fileSize = Obj::getFileSize(fd);
            LOOP_ASSERT_EQ(LINE, 0, fileSize);
            if (0 != fileSize) {
                continue;                                       // CONTINUE
            }

            const int setFileSizeRc =
                u::TestUtil::setFileSize(fd, FILE_SIZE);
            LOOP_ASSERT_EQ(LINE, 0, setFileSizeRc);
            if (0 != setFileSizeRc) {
                continue;                                       // CONTINUE
            }

            const Obj::Offset newFileSize = Obj::getFileSize(fd);
            LOOP_ASSERT_EQ(LINE, FILE_SIZE, newFileSize);
        }
    }
}

template <class STRING_TYPE>
void testCase9_createSystemTemporaryDirectory(
                                           const char         *typeName,
                                           const bsl::string&  tmpWorkingDir,
                                           int                 verbose,
                                           int                 veryVerbose,
                                           int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    STRING_TYPE tempFileName = "tmp.getSystemTemporaryDirectoryTest.7";

    STRING_TYPE tempDirectoryName;
    ASSERT(0 == Obj::getSystemTemporaryDirectory(&tempDirectoryName));
    ASSERT(true == Obj::exists(tempDirectoryName));

    STRING_TYPE absoluteFileName = tempDirectoryName;
    bdls::PathUtil::appendRaw(&absoluteFileName, tempFileName.c_str());

    ASSERT(false == Obj::exists(absoluteFileName));

    Obj::FileDescriptor tempFD =
        Obj::open(absoluteFileName, Obj::e_CREATE, Obj::e_READ_ONLY);
    ASSERT(Obj::k_INVALID_FD != tempFD);

    Obj::close(tempFD);

    ASSERT(true == Obj::exists(absoluteFileName));

    ASSERT(0 == Obj::remove(absoluteFileName));

    ASSERT(false == Obj::exists(absoluteFileName));

}

void testCase6_move_normal(const char *oldName,
                           const char *newName,
                           bool        expectOldCreateFailure,
                           bool        expectMoveFailure,
                           int         verbose,
                           int         veryVerbose,
                           int         veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;

    Obj::FileDescriptor fd = Obj::open(oldName,
                                       Obj::e_OPEN_OR_CREATE,
                                       Obj::e_READ_WRITE);
    bool createFailed = (Obj::k_INVALID_FD == fd);

    if (createFailed) {
        if (!expectOldCreateFailure) {
            ASSERTV(oldName,
                    createFailed && "Did not expect file creation to fail");
        }

        return;                                                       // RETURN
    }

    ASSERTV(oldName, true == Obj::exists(oldName));

    int result = Obj::move(oldName, newName);

    if (0 != result) {
        if (!expectMoveFailure) {
            ASSERTV(oldName, newName, result,
                    (0 != result) && "Did not expect file move to fail");
        }

        ASSERTV(oldName, 0 == Obj::remove(oldName));

        return;                                                       // RETURN
    }

    if (0 != std::strcmp(oldName, newName)) {
        ASSERTV(oldName, false == Obj::exists(oldName));
    }

    ASSERTV(newName, true == Obj::exists(newName));

    ASSERTV(newName, 0 == Obj::remove(newName));
}

template <class OLD_STRING_TYPE, class NEW_STRING_TYPE>
void testCase6_move_template(const OLD_STRING_TYPE& oldName,
                             const NEW_STRING_TYPE& newName,
                             bool                   expectOldCreateFailure,
                             bool                   expectMoveFailure,
                             int                    verbose,
                             int                    veryVerbose,
                             int                    veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;

    Obj::FileDescriptor fd = Obj::open(oldName,
                                       Obj::e_OPEN_OR_CREATE,
                                       Obj::e_READ_WRITE);
    bool createFailed = (Obj::k_INVALID_FD == fd);

    if (createFailed) {
        if (!expectOldCreateFailure) {
            ASSERTV(oldName,
                    createFailed && "Did not expect file creation to fail");
        }

        return;                                                       // RETURN
    }

    ASSERTV(oldName, true == Obj::exists(oldName));

    int result = Obj::move(oldName, newName);

    if (0 != result) {
        if (!expectMoveFailure) {
            ASSERTV(oldName, newName, result,
                    (0 != result) && "Did not expect file move to fail");
        }

        ASSERTV(oldName, 0 == Obj::remove(oldName));

        return;                                                       // RETURN
    }

    ASSERTV(oldName, false == Obj::exists(oldName));
    ASSERTV(newName, true == Obj::exists(newName));

    ASSERTV(newName, 0 == Obj::remove(newName));
}

template <class STRING_TYPE>
void testCase5_isRegularFile_isDirectory(const char         *typeName,
                                         int                 test,
                                         const bsl::string&  tmpWorkingDir,
                                         int                 verbose,
                                         int                 veryVerbose,
                                         int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) tmpWorkingDir;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

    struct Parameters {
        const char* good;
        const char* badNoExist;
        const char* badWrongType;
    };

#define CASE5_TMPDIR "tmp.filesystemutil.case5"

    struct ParametersByType {
        Parameters regular;
        Parameters directory;
    } parameters = {
        { CASE5_TMPDIR PS "file",
          CASE5_TMPDIR PS "file2",
          CASE5_TMPDIR PS "dir"  },
        { CASE5_TMPDIR PS "dir",
          CASE5_TMPDIR PS "dir2",
          CASE5_TMPDIR PS "file" }
    };

    const Parameters& r = parameters.regular;
    const Parameters& d = parameters.directory;

    ASSERT(0 == Obj::createDirectories(r.good));

    ::makeArbitraryFile(r.good);
    ASSERT(0 == Obj::createDirectories(r.badWrongType, true));
    ASSERT(true == Obj::isRegularFile(r.good));
    ASSERT(false == Obj::isRegularFile(r.badNoExist));
    ASSERT(false == Obj::isRegularFile(r.badWrongType));

    ::makeArbitraryFile(d.badWrongType);
    ASSERT(0 == Obj::createDirectories(d.good, true));
    ASSERT(true == Obj::isDirectory(d.good));
    ASSERT(false == Obj::isDirectory(d.badNoExist));
    ASSERT(false == Obj::isDirectory(d.badWrongType));

#ifndef BSLS_PLATFORM_OS_WINDOWS
    if (veryVerbose) {
       cout << "...symbolic link tests..." << endl;
    }

    bsl::string absolute;
    ASSERT(0 == Obj::getWorkingDirectory(&absolute));
    bdls::PathUtil::appendRaw(&absolute, r.good);

    bsl::string link = absolute;
    bdls::PathUtil::popLeaf(&link);
    bdls::PathUtil::appendRaw(&link, "link_rg");
    int rc = symlink(absolute.c_str(), link.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isRegularFile(link.c_str()));
    ASSERT(true  == Obj::isRegularFile(link.c_str(), true));

    bsl::string link2 = r.good;
    bdls::PathUtil::popLeaf(&link2);
    bdls::PathUtil::appendRaw(&link2, "link_rg2");
    rc = symlink(link.c_str(), link2.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isRegularFile(link2));
    ASSERT(true  == Obj::isRegularFile(link2, true));

    bdls::PathUtil::popLeaf(&link);
    bdls::PathUtil::appendRaw(&link, "link_rbw");
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::appendRaw(&absolute, r.badWrongType);
    rc = symlink(absolute.c_str(), link.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isRegularFile(link));
    ASSERT(false == Obj::isRegularFile(link, true));

    bdls::PathUtil::popLeaf(&link);
    bdls::PathUtil::appendRaw(&link, "link_rbn");
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::appendRaw(&absolute, r.badNoExist);
    rc = symlink(absolute.c_str(), link.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isRegularFile(link));
    ASSERT(false == Obj::isRegularFile(link, true));

    bdls::PathUtil::popLeaf(&link);
    bdls::PathUtil::appendRaw(&link, "link_dg");
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::popLeaf(&absolute);
    bdls::PathUtil::appendRaw(&absolute, d.good);
    rc = symlink(absolute.c_str(), link.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isDirectory(link));
    ASSERT(false == Obj::isRegularFile(link));
    ASSERT(true  == Obj::isDirectory(link, true));
    ASSERT(false == Obj::isRegularFile(link, true));

    bdls::PathUtil::popLeaf(&link2);
    bdls::PathUtil::appendRaw(&link2, "link_dg2");
    rc = symlink(link.c_str(), link2.c_str());

    // test invariant:

    ASSERT(0 == rc);

    ASSERT(false == Obj::isDirectory(link2));
    ASSERT(false == Obj::isRegularFile(link2));
    ASSERT(true  == Obj::isDirectory(link2, true));
    ASSERT(false == Obj::isRegularFile(link2, true));

#endif  // Symbolic link testing on non-Windows

#ifndef BSLS_PLATFORM_OS_WINDOWS  // (unix domain socket)
    {
        // Unix domain sockets should return 'false' for 'isRegularFile'
        // and 'isDirectory' (DRQS 2071065).

        if (veryVerbose) {
            cout << "...unix domain socket..." << endl;
        }
        bsl::string filename = ::tempFileName(test);
        Obj::remove(filename);

        int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
        LOOP_ASSERT(socketFd, socketFd >= 0);

        struct sockaddr_un address;
        address.sun_family = AF_UNIX;
        sprintf(address.sun_path, "%s", filename.c_str());

        // Add one to account for the null terminator for the filename.

        const int ADDR_LEN = (int) (sizeof(address.sun_family) +
                                    filename.size() +
                                    1);

        int rc = ::bind(socketFd, (struct sockaddr *)&address, ADDR_LEN);
        LOOP3_ASSERT(rc, errno, strerror(errno), 0 == rc);


        LOOP_ASSERT(filename, Obj::exists(filename));
        LOOP_ASSERT(filename, !Obj::isDirectory(filename));
        LOOP_ASSERT(filename, !Obj::isRegularFile(filename));
        Obj::remove(filename);
    }
#endif  // BSLS_PLATFORM_OS_WINDOWS (unix domain socket)

    //clean up

    ASSERT(0 == Obj::remove(CASE5_TMPDIR, true));

#undef CASE5_TMPDIR
}

template <class VECTOR_TYPE>
void testCase4_findMatchingPaths(const char         *vectorTypeName,
                                 const char         *stringTypeName,
                                 int                 test,
                                 const bsl::string&  tmpWorkingDir,
                                 int                 verbose,
                                 int                 veryVerbose,
                                 int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) test; (void) tmpWorkingDir;

    if (verbose) cout << "\n\t+++++++++++++++ Testing "
                      << vectorTypeName
                          << " / "
                      << stringTypeName
                      << endl;

    const char* filenames[] = {
       "abcd",
       "zyx",
       "zy.z",
       "abc.zzz",
       "abc.def",
    };

#define PATH  "bdls_filesystemutil.temp.3.futc3"
#define PATHQ "bdls_filesystemutil.temp.3.futc?"

    bsl::string path(PATH);

    ASSERT(Obj::remove(path.c_str(), true));

    const char tripleQMarkLiteral[] = "bdls_filesystemutil.temp.3.futc3/b"
                                      "???" "/*d*";

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

#undef PATH
#undef PATHQ

    const int numFiles = INT_SIZEOF(filenames) / INT_SIZEOF(*filenames);

    bdls::PathUtil::appendRaw(&path, "alpha");

    if (veryVerbose) { T_; cout << "Creating directory "; P(path); }

    ASSERT(0 == Obj::createDirectories(path.c_str(), true));
    for (int i = 0; i < numFiles; ++i) {
        bdls::PathUtil::appendRaw(&path, filenames[i]);

        if (veryVerbose) { T_; T_; cout << "Creating file "; P(path); }

        Obj::FileDescriptor f =  Obj::open(path,
                                           Obj::e_CREATE,
                                           Obj::e_READ_WRITE);
        ASSERT(Obj::k_INVALID_FD != f);
        ASSERT(0 == Obj::close(f));

        if (veryVerbose) { T_; T_; cout << "Looking up file "; P(path); }

        VECTOR_TYPE lookup;
        bsl::string pathStr(path);

        lookup.resize(1);
        lookup.front() = "woof";

        int rc = Obj::findMatchingPaths(&lookup, path.c_str());
        ASSERT(1 == rc);
        ASSERT(1 == lookup.size());
        ASSERT(path == lookup.front());
        bsl::string rollup =
            ::rollupPaths<VECTOR_TYPE, bsl::string>(lookup);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        replace_if(rollup.begin(), rollup.end(), ::isForwardSlash,*PS);
#endif
        LOOP2_ASSERT(path, rollup, path == rollup);

        bdls::PathUtil::popLeaf(&path);
    }
    bdls::PathUtil::popLeaf(&path);

    bdls::PathUtil::appendRaw(&path, "beta");

    if (veryVerbose) { T_; cout << "Creating directory "; P(path); }

    ASSERT(0 == Obj::createDirectories(path.c_str(), true));
    for (int i = 0; i < numFiles; ++i) {
        bdls::PathUtil::appendRaw(&path, filenames[i]);

        if (veryVerbose) { T_; T_; cout << "Creating file "; P(path); }

        Obj::FileDescriptor f = Obj::open(path,
                                          Obj::e_CREATE,
                                          Obj::e_READ_WRITE);
        ASSERT(Obj::k_INVALID_FD != f);
        ASSERT(0 == Obj::close(f));

        if (veryVerbose) { T_; T_; cout << "Looking up "; P(path); }

        VECTOR_TYPE lookup;
        bsl::string pathStr(path);

        lookup.resize(1);
        lookup.front() = "woof";

        int rc = Obj::findMatchingPaths(&lookup, path.c_str());

        ASSERT(1 == rc);
        ASSERT(1 == lookup.size());
        ASSERT(path == lookup.front());
        bsl::string rollup =
            ::rollupPaths<VECTOR_TYPE, bsl::string>(lookup);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        replace_if(rollup.begin(), rollup.end(), ::isForwardSlash,*PS);
#endif
        LOOP2_ASSERT(path, rollup, path == rollup);

        bdls::PathUtil::popLeaf(&path);
    }
    bdls::PathUtil::popLeaf(&path);

    VECTOR_TYPE resultPaths;
    enum { NUM_PARAMETERS = sizeof(parameters) / sizeof(*parameters) };
    for (int i = 0; i < NUM_PARAMETERS; ++i) {
        const Parameters& p = parameters[i];
        const int LINE      =  p.line;

        bsl::string patternStr(p.pattern);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        replace_if(
            patternStr.begin(), patternStr.end(), ::isForwardSlash, *PS);
#endif
        bsl::string pattern(patternStr);

        if (veryVerbose) { T_; T_; cout << "Looking up "; P(path); }

        resultPaths.resize(1);
        resultPaths.front() = "woof";

        int rc = Obj::findMatchingPaths(&resultPaths, pattern.c_str());

        const int np = static_cast<int>(resultPaths.size());
        ASSERT(np == rc);

        bsl::string rollup =
            ::rollupPaths<VECTOR_TYPE, bsl::string>(resultPaths);
        LOOP3_ASSERT(LINE, p.result, rollup,
                                           string(p.result) == rollup);
    }

    // non-existent file

    resultPaths.push_back(bsl::string("meow"));

    int rc = Obj::findMatchingPaths(&resultPaths, "idontexist*");

    ASSERT(0 == rc);
    ASSERT(resultPaths.empty());

    ASSERT(0 == Obj::remove(path, true));
}

template <class STRING_TYPE>
void testCase2_CStringUtil(const char         *stringTypeName,
                           const STRING_TYPE&  str,
                           const char         *strPayload,
                           int                 test,
                           int                 verbose,
                           int                 veryVerbose,
                           int                 veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing "
                      << stringTypeName
                      << endl;

    const char *flattened = bdls::FilesystemUtil_CStringUtil::flatten(str);

    ASSERT(0 == strcmp(flattened, strPayload));
}

void testCase2_CStringUtil_StringRef(const char               *stringTypeName,
                                     const bslstl::StringRef&  str,
                                     const char               *strPayload,
                                     int                       test,
                                     int                       verbose,
                                     int                       veryVerbose,
                                     int                       veryVeryVerbose)
{
    (void) verbose; (void) veryVerbose; (void) veryVeryVerbose;
    (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing "
                      << stringTypeName
                      << endl;

    bsl::string tmpString = bdls::FilesystemUtil_CStringUtil::flatten(str);

    ASSERT(0 == strcmp(tmpString.c_str(), strPayload));
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bsl::string origWorkingDirectory;
    ASSERT(0 == Obj::getWorkingDirectory(&origWorkingDirectory));

    // Make 'taskAbsolutePath' be a absolute path to the executable, for when
    // we fork / exec from different directories.

    const bsl::string taskAbsolutePath = PS[0] == argv[0][0] ||
                     bsl::strstr(argv[0], ":\\") ||  bsl::strstr(argv[0], ":/")
                                       ? bsl::string(argv[0])
                                       : origWorkingDirectory + PS + argv[0];
    ASSERTV(origWorkingDirectory, argv[0], taskAbsolutePath,
                                                Obj::exists(taskAbsolutePath));

    bsl::string tmpWorkingDir;
    {
        // Must not call 'tempFileName' here, because 'tempFileName' would
        // create a plain file with the result name, and the attempt to create
        // the directory would fail.

#ifdef BSLS_PLATFORM_OS_UNIX
        char host[80];
        ASSERT(0 == ::gethostname(host, sizeof(host)));
#else
        const char *host = "win";     // 'gethostname' is difficult on
                                      // Windows, and we usually aren't using
                                      // nfs there anyway.
#endif

        bsl::ostringstream oss;
        oss << "tmp.filesystemutil.case_" << test << '.' << host << '.' <<
                                                               ::localGetPId();
        tmpWorkingDir = oss.str();
    }
    if (veryVerbose) P(tmpWorkingDir);

    if (Obj::exists(tmpWorkingDir)) {
        // Sometimes the cleanup at the end of this program is unable to clean
        // up files, so we might encounter leftovers from a previous run, but
        // these can usually be deleted if sufficient time has elapsed.  If
        // we're not able to clean it up now, old files may prevent the test
        // case we're running this time from working.  So we want this assert
        // to fail to give the tester a 'heads-up' as to what went wrong.

        LOOP_ASSERT(tmpWorkingDir, 0 == Obj::remove(tmpWorkingDir, true));
    }
    ASSERT(0 == Obj::createDirectories(  tmpWorkingDir, true));
    ASSERT(0 == Obj::setWorkingDirectory(tmpWorkingDir));

    switch(test) { case 0:
      case 29: {
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

        Obj::remove("bdls_filesystemutil.t.temp.28", true);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        bsl::string logPath =  "bdls_filesystemutil.t.temp.28\\logs28\\";
#else
        bsl::string logPath =  "bdls_filesystemutil.t.temp.28/logs28/";
#endif

        ASSERT(0 == Obj::createDirectories(logPath.c_str(), true));
        const int TESTSIZE = 10;
        bdlt::Datetime modTime[TESTSIZE];

        for (int i = 0; i < TESTSIZE; ++i) {
            stringstream s;
            s << logPath << "file" << i << ".log";

            if (veryVerbose) {
                cout << "Creating file: " << s.str() << endl;
            }

            Obj::FileDescriptor fd = Obj::open(s.str(),
                                               Obj::e_OPEN_OR_CREATE,
                                               Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fd);

            const char buffer[] = "testing";
            int bytes           = INT_SIZEOF(buffer);

            Obj::write(fd, buffer, bytes);
            Obj::close(fd);

            bslmt::ThreadUtil::microSleep(10000);

            Obj::getLastModificationTime(&modTime[i], s.str());
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
        ASSERT(0 == bdls::PathUtil::popLeaf(&logPath));
        ASSERT(0 == Obj::remove(logPath.c_str(), true));
      } break;
      case 28: {
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

        Obj::remove("bdls_filesystemutil.temp.temp.1");

///Example 1: General Usage
/// - - - - - - - - - - - -
// In this example, we start with a (relative) native path to a directory
// containing log files:
//..
    #ifdef BSLS_PLATFORM_OS_WINDOWS
      bsl::string logPath = "bdls_filesystemutil.temp.temp.1\\logs";
    #else
      bsl::string logPath = "bdls_filesystemutil.temp.temp.1/logs";
    #endif
//..
// Suppose that we want to separate files into "old" and "new" subdirectories
// on the basis of modification time.  We will provide paths representing these
// locations, and create the directories if they do not exist:
//..
    bsl::string oldPath(logPath), newPath(logPath);
    bdls::PathUtil::appendRaw(&oldPath, "old");
    bdls::PathUtil::appendRaw(&newPath, "new");
    int rc = bdls::FilesystemUtil::createDirectories(oldPath, true);
    ASSERT(0 == rc);
    rc = bdls::FilesystemUtil::createDirectories(newPath, true);
    ASSERT(0 == rc);
//..
// We know that all of our log files match the pattern "*.log", so let's search
// for all such files in the log directory:
//..
    bdls::PathUtil::appendRaw(&logPath, "*.log");
    bsl::vector<bsl::string> logFiles;
    Obj::findMatchingPaths(&logFiles, logPath.c_str());
//..
// Now for each of these files, we will get the modification time.  Files that
// are older than 2 days will be moved to "old", and the rest will be moved to
// "new":
//..
    bdlt::Datetime modTime;
    bsl::string   fileName;
    for (bsl::vector<bsl::string>::iterator it = logFiles.begin();
                                                  it != logFiles.end(); ++it) {
      ASSERT(0 == Obj::getLastModificationTime(&modTime,
                                                                *it));
      ASSERT(0 == bdls::PathUtil::getLeaf(&fileName, *it));
      bsl::string *whichDirectory =
                  2 < (bdlt::CurrentTime::utc() - modTime).totalDays()
                  ? &oldPath
                  : &newPath;
      bdls::PathUtil::appendRaw(whichDirectory, fileName.c_str());
      ASSERT(0 == Obj::move(it->c_str(),
                                             whichDirectory->c_str()));
      bdls::PathUtil::popLeaf(whichDirectory);
    }
//..

#if 0
        // file i/o

        // create a new file
        Obj::FileDescriptor fd = Obj::open(
                                                         "tempfile",.
                                                         Obj::e_OPEN_OR_CREATE,
                                                         Obj::e_READ_WRITE);
        ASSERT(fd != Obj::k_INVALID_FD);
        // allocate a buffer with the size equal to memory page size and
        // fill with some data
        int size = Obj::pageSize();
        char* buf = new char[size];
        for(int i=0; i<size; ++i) {
            buf[i] = i & 0xFF;
        }

        // write data to the file
        Obj::seek(fd, size,
                                         Obj::FROM_BEGINNING);
        int rc = Obj::write(fd, buf, size);
        ASSERT(rc == size);

        // map the data page into memory
        char* data;
        rc = Obj::map(fd, (void**)&data, 0, size, true);
        ASSERT(0 == rc);

        // verify the data is equal to what we have written
        ASSERT(0 != data);
        ASSERT(0 == memcmp(buf, data, size));

        // unmap the page, delete the buffer and close the file
        rc = Obj::unmap(data, size);
        ASSERT(0 == rc);
        delete[] buf;
        Obj::close(fd);
#endif

        // NOT IN USAGE EXAMPLE: CLEAN UP

        ASSERT(0 == bdls::PathUtil::popLeaf(&logPath));
        ASSERT(0 == bdls::PathUtil::popLeaf(&logPath));
        ASSERT(0 == Obj::remove(logPath.c_str(), true));
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING SYMLINKS
        //
        // Concerns:
        //: 1 'isSymbolicLink' returns 'true' for file and directory symlinks
        //:   and 'false' for other FS objects.
        //:
        //: 2 'getSymbolicLinkTarget' correctly returns the symlink target.
        //:
        //: 3 Windows directory junctions are treated as directory symlinks.
        //
        // Plan:
        //: 1 Create a file and a directory.
        //:
        //: 2 Create a symlink for the file and for the directory.
        //:
        //: 3 Verify that 'isSymbolicLink' returns 'true' for the both symlinks
        //:   and 'false' for the file and the directory. (C1)
        //:
        //: 4 Verify that 'getSymbolicLinkTarget' returns the directory name
        //:   for the directory symlink and the file name for the file symlink.
        //:   (C2)
        //:
        //: 6 If running on Windows, create a directory junction for the
        //:   directory and verify the functions work correctly with it. (C3)
        //
        // TESTING
        //   bool isSymbolicLink(STRING_TYPE);
        //   int getSymbolicLinkTarget(STRING_TYPE *, STRING_TYPE);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING SYMLINKS\n"
                             "================\n";

        const bsl::string dir("dir");
        const bsl::string file("file");
        const bsl::string dir_symlink("dir_symlink");
        const bsl::string file_symlink("file_symlink");
        bsl::string st;

        ASSERT(Obj::createDirectories(dir, true) == 0);
        ASSERT(Obj::isDirectory(dir));

        localTouch(file);
        ASSERT(Obj::isRegularFile(file));

        ASSERT(createSymlink(dir, dir_symlink));
        ASSERT( Obj::isSymbolicLink(dir_symlink));
        ASSERT(!Obj::isSymbolicLink(dir));
        ASSERT(Obj::getSymbolicLinkTarget(&st, dir_symlink) == 0);
        ASSERT(st == dir);
        ASSERT(Obj::getSymbolicLinkTarget(&st, dir        ) != 0);

        ASSERT(createSymlink(file, file_symlink));
        ASSERT( Obj::isSymbolicLink(file_symlink));
        ASSERT(!Obj::isSymbolicLink(file));
        ASSERT(Obj::getSymbolicLinkTarget(&st, file_symlink) == 0);
        ASSERT(st == file);
        ASSERT(Obj::getSymbolicLinkTarget(&st, file        ) != 0);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        // TEST directory junctions
        const bsl::string dir_junction("dir_junction");

        bsl::string cmd("mklink /J ");
        cmd += dir_junction;
        cmd += ' ';
        cmd += dir;
        ASSERT(system(cmd.c_str()) == 0);

        ASSERT(Obj::isSymbolicLink(dir_junction));
        st.clear();
        ASSERT(Obj::getSymbolicLinkTarget(&st, dir_junction) == 0);
        ASSERT(st == dir);
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING GET LAST MODIFICATION TIME FOR FILE DESCRIPTORS
        //
        // Concerns:
        //: o Attempting to get the last modification time of an invalid file
        //:   descriptor returns a non-zero status.
        //:
        //: o Attempting to get the last modification time of a closed file
        //:   descriptor returns a non-zero status.
        //:
        //: o The last modification time of a newly-created file is close to
        //:   the time at which the file was created.  Additionally, that time
        //:   is no earlier than when the file was created.
        //:
        //: o The last modification time is reported correctly according to the
        //:   underlying file-systems interface, with a precision of 1 second.
        //
        // Plan:
        //: 1 Get the last modification time of an invalid file descriptor and
        //:   verify the return code indicates an error occurred.
        //:
        //: 2 Get the last modification time of closed file descriptor
        //:   and verify that the return code indicates an error occurred.
        //:
        //: 3 Get the last modification time of a newly-created file and verify
        //:   that the time is within 1 minute of when the file was created
        //:   according to the system's real-time clock.
        //:
        //: 4 Create a table of many times known to be acceptable file
        //:   timestamps across the majority of file-systems, and for each time
        //:   'T', do the following:
        //:
        //:   1 Create a new file 'F'.
        //:
        //:   2 Set the last modification time of 'F' to 'T'.
        //:
        //:   3 Get the last modification time of 'T' as 'U'.
        //:
        //:   4 Verify that 'U' is equal to 'T' up to 1 second precision.
        //
        // TESTING
        //   int getLastModificationTime(bdlt::Datetime *, FileDescriptor);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << bsl::endl
                << "TESTING GET LAST MODIFICATION TIME FOR FILE DESCRIPTORS"
                << bsl::endl
                << "======================================================="
                << bsl::endl;
        }

        {
            // This block tests that attempting to get the modification time of
            // an invalid file descriptor returns with an error status.

            bdlt::Datetime lastModificationTime;
            int rc = Obj::getLastModificationTime(&lastModificationTime,
                                                  Obj::k_INVALID_FD);
            ASSERT_EQ(-1, rc);
            ASSERT_EQ(bdlt::Datetime(), lastModificationTime);
        }

        {
            // This block tests that attempting to get the modification time of
            // a closed file descriptor returns with an error status.

            const Obj::FileDescriptor fd = u::TestUtil::createEphemeralFile();
            ASSERT(Obj::k_INVALID_FD != fd);
            u::FileDescriptorCloseGuard closeGuard(fd);

            int rc = closeGuard.closeAndRelease();
            ASSERT_EQ(0, rc);

            bdlt::Datetime lastModificationTime;
            rc = Obj::getLastModificationTime(&lastModificationTime, fd);
            ASSERT_EQ(-1, rc);
        }

        {
            // This block tests that attempting to get the modification time of
            // a newly-created file yields a time close to (specifically,
            // within 1 minute) of the time measured just after the file was
            // created.  Note that while most file systems are accurate to
            // within 1 second, and many are accurate to the microsecond (and
            // claim nanosecond precision), others (*cough* NFS *cough*) are
            // not so well-behaved.

            const Obj::FileDescriptor fd = u::TestUtil::createEphemeralFile();
            ASSERT(Obj::k_INVALID_FD != fd);
            const u::FileDescriptorCloseGuard closeGuard(fd);

            const bdlt::Datetime currentDatetime =
                bdlt::EpochUtil::convertFromTimeInterval(
                    bsls::SystemTime::nowRealtimeClock());

            bdlt::Datetime lastModificationTime;
            int rc = Obj::getLastModificationTime(&lastModificationTime, fd);
            ASSERT_EQ(0, rc);

            const bdlt::DatetimeInterval modificationTimeSkew =
                    lastModificationTime - currentDatetime;

            ASSERT_EQ(0, modificationTimeSkew.days());

            const bsls::Types::Int64 modificationTimeSkewMicroSec =
                    modificationTimeSkew.fractionalDayInMicroseconds() < 0
                 ? -modificationTimeSkew.fractionalDayInMicroseconds()
                 :  modificationTimeSkew.fractionalDayInMicroseconds();

            static const bsls::Types::Int64 oneMinuteMicroSec =
                bdlt::TimeUnitRatio::k_MICROSECONDS_PER_MINUTE;
            ASSERT_LE(0, modificationTimeSkewMicroSec);
            ASSERT_GE(oneMinuteMicroSec, modificationTimeSkewMicroSec);
        }

        {
            // This block tests sub-second modification time retrieval by
            // creating two temporary files with a slight delay, then
            // retrieving the modification times of both files, and verifying
            // that the retrieved modification times are different and in the
            // correct order.  Note that while most file systems are accurate
            // to within 1 second, and many are accurate to the microsecond
            // (and claim nanosecond precision), others (*cough* NFS *cough*)
            // are not so well-behaved.

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_SOLARIS)
            static const int clockFileDelay =
                            bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND;
                // How long to wait between reading the realtime clock and
                // creating the first file.
            static const int fileFileDelay =
                            bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND / 2;
                // How long to wait between creating files.
            static const int clockFileCheck =
                        2 * bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND - 1;
                // The maximum permitted difference between the queried
                // realtime clock and the timestamp of the first created file.
            static const int fileFileCheck =
                            bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND - 1;
                // The maximum permitted difference between the timestamps of
                // created files.
#else
            static const int clockFileDelay =
                            bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND / 2;
                // How long to wait between reading the realtime clock and
                // creating the first file.
            static const int fileFileDelay = 10001;
                // How long to wait between creating files.
            static const int clockFileCheck =
                            bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND - 1;
                // The maximum permitted difference between the queried
                // realtime clock and the timestamp of the first created file.
            static const int fileFileCheck =
                        bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND / 2 - 1;
                // The maximum permitted difference between the timestamps of
                // created files.
#endif

            // The first time we write to a file on a filesystem there can be
            // initial delays, notably over NFS which doesn't mount a filsystem
            // until first usage. Those delays can be large and overwhelm small
            // relative differences between file timestamps, so we deliberately
            // create a tempoorary file in the same location before we start
            // the test.
            {
                const Obj::FileDescriptor fd0 =
                                            u::TestUtil::createEphemeralFile();
                ASSERT(Obj::k_INVALID_FD != fd0);
                const u::FileDescriptorCloseGuard closeGuard1(fd0);
            }

            bslmt::ThreadUtil::microSleep(
                               bdlt::TimeUnitRatio::k_MICROSECONDS_PER_SECOND);

            const bdlt::Datetime startDatetime =
                bdlt::EpochUtil::convertFromTimeInterval(
                    bsls::SystemTime::nowRealtimeClock());

            // A brief sleep to ensure the file timestamp exceeds the clock
            // time.  Note that the delay has to be relatively high as some
            // file systems have relatively poor timestamp precision.
            bslmt::ThreadUtil::microSleep(clockFileDelay);

            // Create the first file.
            const Obj::FileDescriptor fd1 = u::TestUtil::createEphemeralFile();
            ASSERT(Obj::k_INVALID_FD != fd1);
            const u::FileDescriptorCloseGuard closeGuard1(fd1);

            // Force a write to ensure the modification time is up to date.
            int rc = u::TestUtil::modifyTemporaryFile(fd1);
            ASSERT_EQ(0, rc);

            // A brief sleep to ensure the file timestamps are different.  Note
            // that the delay has to be relatively high as some file systems
            // have relatively poor timestamp precision.
            bslmt::ThreadUtil::microSleep(fileFileDelay);

            // Create the second file.
            const Obj::FileDescriptor fd2 = u::TestUtil::createEphemeralFile();
            ASSERT(Obj::k_INVALID_FD != fd2);
            const u::FileDescriptorCloseGuard closeGuard2(fd2);

            // Force a write to ensure the modification time is up to date.
            rc = u::TestUtil::modifyTemporaryFile(fd2);
            ASSERT_EQ(0, rc);

            // A brief sleep to ensure the clock time exceeds the file
            // timestamp.  Note that the delay has to be relatively high as
            // some file systems have relatively poor timestamp precision.
            bslmt::ThreadUtil::microSleep(clockFileDelay);

            const bdlt::Datetime endDatetime =
                bdlt::EpochUtil::convertFromTimeInterval(
                    bsls::SystemTime::nowRealtimeClock());

            bdlt::Datetime lastModificationTime1;
            rc = Obj::getLastModificationTime(&lastModificationTime1, fd1);
            ASSERT_EQ(0, rc);

            bdlt::Datetime lastModificationTime2;
            rc = Obj::getLastModificationTime(&lastModificationTime2, fd2);
            ASSERT_EQ(0, rc);

            const bdlt::DatetimeInterval skew1 =
                                 lastModificationTime1 - startDatetime;
            const bdlt::DatetimeInterval skew2 =
                                 lastModificationTime2 - lastModificationTime1;
            const bdlt::DatetimeInterval skew3 =
                                 endDatetime - lastModificationTime2;
            ASSERT_LT(0, skew1.fractionalDayInMicroseconds());
            ASSERT_LT(0, skew2.fractionalDayInMicroseconds());
            ASSERT_LT(0, skew3.fractionalDayInMicroseconds());
            ASSERT_GT(clockFileCheck,
                      skew1.fractionalDayInMicroseconds());
            ASSERT_GT(fileFileCheck,
                      skew2.fractionalDayInMicroseconds());
            ASSERT_GT(clockFileCheck,
                      skew3.fractionalDayInMicroseconds());
        }

        enum {
            NA = 0
        };

        // All contemporary file systems that support modification timestamps
        // provide at least microsecond precision for those timestamps.
        const bdlt::DatetimeInterval MOD_TIME_PRECISION(
            0, 0, 0, 0, 0, 1);

        typedef bdlt::Datetime DT;

        // The relatively short range of dates that this table enumerates are
        // those that all of our supported file systems can accurately store
        // and retrieve (and can do so with second precision).  The lower bound
        // of January 1st, 1970 is the start of the Unix epoch, which many file
        // systems on Unix platforms use as the minimum-supported date.  The
        // upper bound of December 31st, 2200 is a "nice round date" in the
        // last century supported by Unix file systems that store timestamps as
        // an unsigned 64-bit number of nanoseconds since the Unix epoch (such
        // as APFS on some Darwin platforms.)

        const struct {
            int                 d_line;
                // the line number

            bdlt::Datetime      d_modTime;
                // the modification time to set

        } DATA[] = {
            // LINE  MODIFICATION TIME
            // ----  --------------------------------------
            {    L_, DT(1970,  1,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(1970,  1,  1,  0,  0,  0,   0,   1) },
            {    L_, DT(1970,  1,  1,  0,  0,  0,   1,   0) },
            {    L_, DT(1970,  1,  1,  0,  0,  1,   0,   0) },
            {    L_, DT(1970,  1,  1,  0,  1,  0,   0,   0) },
            {    L_, DT(1970,  1,  1,  1,  0,  0,   0,   0) },
            {    L_, DT(1970,  1,  2,  0,  0,  0,   0,   0) },
            {    L_, DT(1970,  2,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(1970,  1,  1,  1, 59, 59, 999, 999) },
            {    L_, DT(1970, 12, 31, 23, 59, 59, 999, 999) },
#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
            {    L_, DT(1970, 12, 31, 24,  0,  0,   0,   0) },
#endif

            {    L_, DT(1972,  2, 29,  0,  0,  0,   0,   0) },
            {    L_, DT(1972,  2, 29, 23, 59, 59, 999, 999) },

            {    L_, DT(1972,  2, 29,  0,  0,  0,   0,   0) },
            {    L_, DT(1972,  2, 29, 23, 59, 59, 999, 999) },

            {    L_, DT(2000,  1,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2000,  1,  1,  0,  0,  0,   0,   1) },
            {    L_, DT(2000,  1,  1,  0,  0,  0,   1,   0) },
            {    L_, DT(2000,  1,  1,  0,  0,  1,   0,   0) },
            {    L_, DT(2000,  1,  1,  0,  1,  0,   0,   0) },
            {    L_, DT(2000,  1,  1,  1,  0,  0,   0,   0) },
            {    L_, DT(2000,  1,  2,  0,  0,  0,   0,   0) },
            {    L_, DT(2000,  2,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2000,  1,  1,  1, 59, 59, 999, 999) },
            {    L_, DT(2000, 12, 31, 23, 59, 59, 999, 999) },

            {    L_, DT(2020,  1,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2020,  1,  1,  0,  0,  0,   0,   1) },
            {    L_, DT(2020,  1,  1,  0,  0,  0,   1,   0) },
            {    L_, DT(2020,  1,  1,  0,  0,  1,   0,   0) },
            {    L_, DT(2020,  1,  1,  0,  1,  0,   0,   0) },
            {    L_, DT(2020,  1,  1,  1,  0,  0,   0,   0) },
            {    L_, DT(2020,  1,  2,  0,  0,  0,   0,   0) },
            {    L_, DT(2020,  2,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2020,  1,  1,  1, 59, 59, 999, 999) },
            {    L_, DT(2020, 12, 31, 23, 59, 59, 999, 999) },

            {    L_, DT(2038,  1, 19,  3, 14,  7,   0,   0) },

            {    L_, DT(2200,  1,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2200,  1,  1,  0,  0,  0,   0,   1) },
            {    L_, DT(2200,  1,  1,  0,  0,  0,   1,   0) },
            {    L_, DT(2200,  1,  1,  0,  0,  1,   0,   0) },
            {    L_, DT(2200,  1,  1,  0,  1,  0,   0,   0) },
            {    L_, DT(2200,  1,  1,  1,  0,  0,   0,   0) },
            {    L_, DT(2200,  1,  2,  0,  0,  0,   0,   0) },
            {    L_, DT(2200,  2,  1,  0,  0,  0,   0,   0) },
            {    L_, DT(2200,  1,  1,  1, 59, 59, 999, 999) },
            {    L_, DT(2200, 12, 31, 23, 59, 59, 999, 999) },
        };

        bool haveBigtimeSupport = true;
        {
            const Obj::FileDescriptor fd = u::TestUtil::createEphemeralFile();
            ASSERT(Obj::k_INVALID_FD != fd);
            const u::FileDescriptorCloseGuard closeGuard(fd);
            haveBigtimeSupport =
                               u::TestUtil::isBigtimeSupportAvailable(fd);
        }

        const DT min32BitUnixTimeAsDatetime(1901, 12, 13, 20, 45, 52);
        const DT max32BitUnixTimeAsDatetime(2038, 1, 19, 3, 14, 7);

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int LINE     = DATA[i].d_line;
            DT        MOD_TIME = DATA[i].d_modTime;

            if (!haveBigtimeSupport &&
                 MOD_TIME > max32BitUnixTimeAsDatetime) {
                continue;
            }

            int setModTimeStatus = -1;
            int getModTimeStatus = -1;
            DT  modTime          = DT();

            if (!u::TestUtil::isValidModificationTime(MOD_TIME)) {

                // If the modification time in the test table is not valid for
                // the current platform, verify that this is *only* because
                // the platform stores dates and times as a 32-bit signed
                // number of seconds since the Unix epoch, and for no other
                // reason.

                LOOP_ASSERT_EQ(LINE,
                               u::TestUtil::getMinFileTime(),
                               min32BitUnixTimeAsDatetime);
                LOOP_ASSERT_EQ(LINE,
                               u::TestUtil::getMaxFileTime(),
                               max32BitUnixTimeAsDatetime);
                LOOP_ASSERT(LINE,
                            (min32BitUnixTimeAsDatetime > MOD_TIME) ||
                            (max32BitUnixTimeAsDatetime < MOD_TIME));
                continue;
            }

            {
                const Obj::FileDescriptor fd =
                    u::TestUtil::createEphemeralFile();
                ASSERT(Obj::k_INVALID_FD != fd);
                if (Obj::k_INVALID_FD == fd) {
                    continue;
                }
                const u::FileDescriptorCloseGuard closeGuard(fd);

                setModTimeStatus =
                    u::TestUtil::setLastModificationTime(fd, MOD_TIME);

                getModTimeStatus = Obj::getLastModificationTime(&modTime, fd);
            }

            LOOP_ASSERT_EQ(LINE, 0, setModTimeStatus);
            LOOP_ASSERT_EQ(LINE, 0, getModTimeStatus);

            // The behavior of comparing two 'bdlt::Datetime' values is
            // undefined if either of them have a default time value of 24
            // hours.  Adding 0 hours to the value of a 'bdlt::Datetime'
            // "normalizes" the time value by converting any 24-hour time value
            // to the 0-hour time value.

            MOD_TIME.addTime(0);

            const bdlt::DatetimeInterval modTimeSkew =
                (modTime < MOD_TIME) ? MOD_TIME - modTime : modTime - MOD_TIME;

            LOOP_ASSERT_LE(LINE, bdlt::DatetimeInterval(), modTimeSkew);
            LOOP_ASSERT_GT(LINE, MOD_TIME_PRECISION, modTimeSkew);
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING VISITTREE AND VISITPATHS
        //
        // Concerns:
        //: 1 That 'visitTree' will visit every matching node in a directory
        //:   tree.
        //: 2 That if 'sortFlag' is specified, nodes will be visited in sorted
        //:   order, with directories visited before their contents.
        //: 3 That 'visitPaths' and 'visitTree' respond approriately to
        //:   non-readable directories on Unix (it did not seem to be possible
        //:   to make a directory non-readable on Windows).
        //
        // Plan:
        //: 1 Create a tree with two types of nodes, 'woof*' nodes and 'meow*'
        //:   nodes.  'woof*' nodes include both plain files and directories,
        //:   'meow*' nodes include only plain files.
        //: 2 Create a function object type, 'VisitTreeTestVisitor', that will
        //:   append file names to the end of a vector.
        //: 3 Apply 'visitTree' with the sort flag clear, doing 2 separate
        //:   searches, one for 'woof*' nodes and one for 'meow*' nodes.  Sort
        //:   the vector afterward and verify it matches the sorted expected
        //:   value vector.
        //: 4 Apply 'visitTree' with the sort flag set, doing 2 separate
        //:   searches, one for 'woof*' nodes and one for 'meow*' nodes.
        //:   Verify after that the vector matches the sorted expected value
        //:   vector without itself having had to be sorted.
        //: 5 Make one of the directories temporarily non-readable on Unix,
        //:   and observe that the functions perform appropriately.
        //
        // TESTING
        //   typedef bsl::function<void (const char *path)> Func;
        //
        //   int visitTree(const char *, const string&, const Func&, bool);
        //   int visitTree(const string&, const string&, const Func&, bool);
        //   int visitPaths(const string&, const Func&);
        //   int visitPaths(const char *, const Func&);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'visitTree'\n"
                             "===================\n";

        typedef bsl::vector<bsl::string> FileNameVec;

        const char *root = "woof";
        const bsl::string rootStr(root);

        FileNameVec woofExpVec, woofPathsExpVec, meowExpVec;

        // Note that we don't put 'root' into 'woofExpVec', even though
        // it would match the root pattern, because pattern matching is
        // not performed on the root.

        bsl::string hiddenDir;
        bsl::string ossStr;
        bsl::ostringstream oss, ossB;
        for (int ii = 0; ii < 5; ++ii) {
            oss.str("");
            oss << root << PS << "woof." << ii;

            ossStr = oss.str();

            const bool hide = e_IS_UNIX && 2 == ii;

            woofExpVec.push_back(ossStr);
            ASSERT(0 == Obj::createDirectories(ossStr, 1));

            for (int jj = 0; jj < 5; ++jj) {
                ossB.str("");
                ossB << ossStr << PS << "meow." << jj;

                const bsl::string& ossBStr = ossB.str();

                if (!hide) {
                    meowExpVec.push_back(ossBStr);
                }
                ::localTouch(ossBStr);
            }

            oss << PS << "woof";

            const bsl::string& ossCStr = oss.str();

            if (!hide) {
                woofExpVec.     push_back(ossCStr);
                woofPathsExpVec.push_back(ossCStr);
            }
            ::localTouch(ossCStr);

            if (hide) {
                ASSERT(hiddenDir.empty());
                hiddenDir = ossStr;
                ::chmod(hiddenDir.c_str(), 0177);   // not readable or writable
                                                    // by user
            }
        }

        for (int flags = 0; flags < 4; ++flags) {
            bool sortFlag   = flags & 0x1;
            bool stringFlag = flags & 0x2;

            FileNameVec woofTravVec, meowTravVec;

            VisitTreeTestVisitor woofVisitor;
            woofVisitor.d_vec = &woofTravVec;

            int rc = stringFlag
                     ? Obj::visitTree(rootStr, "woof*", woofVisitor, sortFlag)
                     : Obj::visitTree(root,    "woof*", woofVisitor, sortFlag);
            ASSERTV(rc, errno, 0 == rc);
            ASSERT((e_IS_UNIX ? 9 : 10) == woofTravVec.size());

            if (verbose) {
                cout << "woofExpVec:\n";
                for (FileNameVec::iterator it = woofExpVec.begin();
                                                woofExpVec.end() != it; ++it) {
                    cout << *it << endl;
                }
                cout << "woofTravVec:\n";
                for (FileNameVec::iterator it = woofTravVec.begin();
                                               woofTravVec.end() != it; ++it) {
                    cout << *it << endl;
                }
            }
            if (!sortFlag) {
                ASSERT(woofExpVec != woofTravVec);
                bsl::sort(woofTravVec.begin(), woofTravVec.end());
            }
            ASSERT(woofExpVec == woofTravVec);

            VisitTreeTestVisitor meowVisitor;
            meowVisitor.d_vec = &meowTravVec;

            rc = stringFlag
                     ? Obj::visitTree(rootStr, "meow*", meowVisitor, sortFlag)
                     : Obj::visitTree(root,    "meow*", meowVisitor, sortFlag);
            ASSERTV(rc, errno, 0 == rc);
            ASSERT((e_IS_UNIX ? 20 : 25) == meowTravVec.size());

            if (verbose) {
                cout << "meowTravVec:\n";
                for (FileNameVec::iterator it = meowTravVec.begin();
                                               meowTravVec.end() != it; ++it) {
                    cout << *it << endl;
                }
            }
            if (!sortFlag) {
                bsl::sort(meowTravVec.begin(), meowTravVec.end());
            }
            ASSERT(meowExpVec == meowTravVec);
        }

        for (int stringFlag = 0; stringFlag < 2; ++stringFlag) {
            bsl::vector<bsl::string> woofPathsVec;
            VisitTreeTestVisitor woofVisitor;
            woofVisitor.d_vec = &woofPathsVec;

            int rc;
            const char *str = "woo*" PS "woo*" PS "woo*";
            rc = stringFlag ? Obj::visitPaths(bsl::string(str), woofVisitor)
                            : Obj::visitPaths(str, woofVisitor);
            ASSERT(static_cast<int>(woofPathsExpVec.size()) == rc);

            bsl::sort(woofPathsVec.begin(), woofPathsVec.end());

            LOOP2_ASSERT(woofPathsExpVec.size(), woofPathsVec.size(),
                                woofPathsExpVec.size() == woofPathsVec.size());
            ASSERT(woofPathsExpVec == woofPathsVec);

            woofPathsVec.clear();

#ifndef BSLS_PLATFORM_OS_DARWIN
            str = NAMES[NAME_ANSI];     // String containing invalid UTF-8,
                                        // which non-Darwin Unix can handle
                                        // but Windows can't.
            rc = stringFlag ? Obj::visitPaths(bsl::string(str), woofVisitor)
                            : Obj::visitPaths(str, woofVisitor);
# ifdef BSLS_PLATFORM_OS_WINDOWS
            LOOP2_ASSERT(stringFlag, rc, 0 != rc);
# else
            ASSERT(0 == rc);
# endif
            ASSERT(woofPathsVec.empty());    // Shouldn't have found anything.
#endif

            woofPathsVec.clear();
            str = "tmp.non_existent_file.txt";
            rc = stringFlag ? Obj::visitPaths(bsl::string(str), woofVisitor)
                            : Obj::visitPaths(str, woofVisitor);
            ASSERT(0 == rc);
        }

        if (!hiddenDir.empty()) {
            ::chmod(hiddenDir.c_str(), 0777);    // writeable so we can delete
        }

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // Windows remove has some strange problem here.  Everything will be
        // cleaned up at the end of 'main' anyway, and this code is just
        // intended to test some changes made to Unix 'remove'.

        int rc = Obj::remove(root, true);
        ASSERT(0 == rc);
        ASSERT(!Obj::exists(root));
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING: Specific error codes for 'createDirectories' and
        // 'createPrivateDirectory'
        //
        // Concerns:
        //:  That 'createDirectories' and 'createPrivateDirectory' return
        //:  proper status on failure, depending upon the type of failure (see
        //:  DRQS 123561805).
        //
        // Plan:
        //:  1 Verify that 'createDirectories' call with a path indicating an
        //:    existing file fails with 'k_ERROR_PATH_NOT_FOUND'.
        //:  2 Verify that 'createDirectories' call with the path that would
        //:    indicate child directory of an existing file fails with
        //:    'k_ERROR_PATH_NOT_FOUND'.
        //:  3 Verify that 'createPrivateDirectory' call with a path indicating
        //:    an existing file fails with 'k_ERROR_ALREADY_EXISTS'.
        //:  4 Verify that 'createPrivateDirectory' call with a path indicating
        //:     an existing directory fails with 'k_ERROR_ALREADY_EXISTS'.
        //:  5 Verify that 'createPrivateDirectory' call with the path that
        //:    would indicate child directory of an existing file fails with
        //:    'k_ERROR_PATH_NOT_FOUND'.
        //:  6 Verify that 'createPrivateDirectory' call with the path that
        //:    would indicate child directory of an non-existent directory
        //:    fails with 'k_ERROR_PATH_NOT_FOUND'.
        //
        // Testing:
        //   int createDirectories(const string&, bool);
        //   int createPrivateDirectory(const string&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "TESTING: Specific error codes for 'createDirectories' et al.\n"
            "============================================================\n";

        if (verbose) cout << "Testing 'createDirectories'\n";
        {
            const bsl::string& testBaseDir = "tmpDir";

            if (veryVerbose) { P(testBaseDir); }
            (void)Obj::remove(testBaseDir, true);
            ASSERT(0 == Obj::createDirectories(testBaseDir, true));
            ASSERT(Obj::exists(testBaseDir));

            bsl::string existingFilePath = testBaseDir;
            bdls::PathUtil::appendRaw(&existingFilePath, "file");
            if (veryVerbose) { P(existingFilePath); }
            localTouch(existingFilePath.c_str());
            ASSERT(Obj::exists(existingFilePath));

            bsl::string fullDirPath = existingFilePath;
            if (veryVerbose) { P(existingFilePath); }
            ASSERT(Obj::k_ERROR_PATH_NOT_FOUND ==
                   Obj::createDirectories(existingFilePath, true));

            bsl::string childOfFilePath = existingFilePath;
            bdls::PathUtil::appendRaw(&childOfFilePath, "dir");
            if (veryVerbose) { P(childOfFilePath); }
            ASSERT(Obj::k_ERROR_PATH_NOT_FOUND ==
                   Obj::createDirectories(childOfFilePath));

            ASSERT(0 == Obj::remove(testBaseDir, true));
        }

        if (verbose) cout << "Testing 'createPrivateDirectory'\n";
        {
            const bsl::string& testBaseDir = "tmpDir";

            if (veryVerbose) { P(testBaseDir); }
            (void) Obj::remove(testBaseDir, true);
            ASSERT(0 == Obj::createDirectories(testBaseDir, true));
            ASSERT(Obj::exists(testBaseDir));

            bsl::string existingFilePath = testBaseDir;
            bdls::PathUtil::appendRaw(&existingFilePath, "file");
            if (veryVerbose) { P(existingFilePath); }
            localTouch(existingFilePath.c_str());
            ASSERT(Obj::exists(existingFilePath));

            if (veryVerbose) { P(existingFilePath); }
            ASSERT(Obj::k_ERROR_ALREADY_EXISTS ==
                   Obj::createPrivateDirectory(existingFilePath));

            bsl::string existingDirPath = testBaseDir;
            if (veryVerbose) { P(existingDirPath); }
            ASSERT(Obj::k_ERROR_ALREADY_EXISTS ==
                Obj::createPrivateDirectory(existingDirPath));

            bsl::string childOfFilePath = existingFilePath;
            bdls::PathUtil::appendRaw(&childOfFilePath, "dir");
            if (veryVerbose) { P(childOfFilePath); }
            ASSERT(Obj::k_ERROR_PATH_NOT_FOUND ==
                   Obj::createPrivateDirectory(childOfFilePath));

            bsl::string clildOfNonexistentDirPath = testBaseDir;
            bdls::PathUtil::appendRaw(&clildOfNonexistentDirPath, "dir");
            bdls::PathUtil::appendRaw(&clildOfNonexistentDirPath, "dir2");
            if (veryVerbose) { P(clildOfNonexistentDirPath); }
            ASSERT(Obj::k_ERROR_PATH_NOT_FOUND ==
                   Obj::createPrivateDirectory(clildOfNonexistentDirPath));

            ASSERT(0 == Obj::remove(testBaseDir, true));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'createTemporaryDirectory' METHOD
        //
        // Concerns:
        //: 1 It actually creates a new directory.
        //: 2 A directory created has a different name than a previous one.
        //: 3 Files can be created in the directory.
        //: 4 The created directory has the correct permissions.
        //
        // Plan:
        //: 1 Create directories and a subdirectory. (C-1,3)
        //: 2 Check names, types, and permission. (C-2,4)
        //
        // Testing:
        //   createTemporaryDirectory(string *, const string_view&)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'createTemporaryDirectory' METHOD"
                    "\n=========================================\n";
        }

        testCase23_createTemporaryDirectory<bsl::string>("bsl::string",
                                                         test,
                                                         tmpWorkingDir,
                                                         verbose,
                                                         veryVerbose,
                                                         veryVeryVerbose);
        testCase23_createTemporaryDirectory<std::string>("std::string",
                                                         test,
                                                         tmpWorkingDir,
                                                         verbose,
                                                         veryVerbose,
                                                         veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase23_createTemporaryDirectory<std::pmr::string>(
            "std::pmr::string",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'createTemporaryFile' METHOD
        //
        // Concerns:
        //: 1 It actually creates a new file.
        //: 2 A file created has a different name than a previous one.
        //: 3 The created file has the correct permissions.
        //: 4 The created file is open for writing.
        //
        // Plan:
        //: 1 Create files. (C-1)
        //: 2 Check names, types, and permissions. (C-2,3,4)
        //
        // Testing:
        //   createTemporaryFile(string *, const string_view&)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'createTemporaryFile' METHOD"
                    "\n====================================\n";
        }

        testCase22_createTemporaryFile<bsl::string>("bsl::string",
                                                    test,
                                                    tmpWorkingDir,
                                                    verbose,
                                                    veryVerbose,
                                                    veryVeryVerbose);
        testCase22_createTemporaryFile<std::string>("std::string",
                                                    test,
                                                    tmpWorkingDir,
                                                    verbose,
                                                    veryVerbose,
                                                    veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase22_createTemporaryFile<std::pmr::string>("std::pmr::string",
                                                         test,
                                                         tmpWorkingDir,
                                                         verbose,
                                                         veryVerbose,
                                                         veryVeryVerbose);
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'makeUnsafeTemporaryFilename' METHOD
        //
        // Concerns:
        //: 1 Prefix is copied to output with stuff appended.
        //:
        //: 2 Stuff appended is enough to make it unique.
        //:
        //: 3 Stuff appended is different from previous calls.
        //
        // Plan:
        //: 1 Create names. (C-1)
        //:
        //: 2 Check invariant and variant parts. (C-1,2)
        //:   o The first 'prefix.length()' characters will all be the same and
        //:     match the string 'prefix'.  Everything after that will be
        //:     randomly generated.
        //:
        //: 3 Sample entropy.  Use the function under test to create
        //:   'k_NUM_NAMES' temporary file names, and then compare them against
        //:   each other and verify not only that they all differ, but that
        //:   they differ by at least a certain number of characters.
        //:   o Keep all the names created so far in a random-access container,
        //:     and after each name is created, analyze how many characters it
        //:     differs from all the previously created names.
        //:
        //:   o Go through all previous names:
        //:     1 For each previous name, calculate 'minNumDiffs', the minimum
        //:       number characters that must differ between the two names.
        //:       For the most recently created previous name, at least half of
        //:       the randomly generated characters must differ; for other
        //:       names, at least 2 characters must differ.
        //:
        //:     2 Compare corresponding characters in the random parts of the
        //:       the two names, keeping a tally, 'numDiffs', of the number of
        //:       times they differ, and then verify that
        //:       'minNumDiffs <= numDiff's.
        //:
        //:     3 While comparing corresponding chars of the two strings, make
        //:       sure that at least one such pair differs by at least 3.
        //
        // Testing:
        //   makeUnsafeTemporaryFilename(string *, const string_view&)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'makeUnsafeTemporaryFilename' METHOD"
                    "\n============================================\n";
        }

        testCase21_makeUnsafeTemporaryFilename<bsl::string>("bsl::string",
                                                            test,
                                                            tmpWorkingDir,
                                                            verbose,
                                                            veryVerbose,
                                                            veryVeryVerbose);
        testCase21_makeUnsafeTemporaryFilename<std::string>("std::string",
                                                            test,
                                                            tmpWorkingDir,
                                                            verbose,
                                                            veryVerbose,
                                                            veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase21_makeUnsafeTemporaryFilename<std::pmr::string>(
            "std::pmr::string",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#endif
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING: UTF-8 Filenames
        //
        // DRQS 45032518 requested that for Windows, we treat file names as
        // being UTF-8 encoded and translate them to wide-character format so
        // as to enable correct use of native-language names.  The change made
        // in response is to attempt such conversion, and fail if the
        // conversion does not succeed.
        //
        // Concerns:
        //: 1 We can convert from wchar_t to utf-8 filenames and then back
        //:   again, getting back the original wchar_t name.  Note that this
        //:   does not test 'Obj' functionality, but is
        //:   necessary for further testing.
        //:
        //: 2 We can create files using the utf-8 names.
        //:
        //: 3 We can retrieve these names using path searching.
        //:
        //: 4 In Windows, the names should be the wchar_t names.
        //
        // Plan:
        //: 1 Create a set of wchar_t filenames, convert them to utf-8 and back
        //:   again, and verify that the original names are recovered. (C-1)
        //:
        //: 2 Create each of the files using its utf-8 name, write to it, and
        //:   close it, checking for failures.  (C-2)
        //:
        //: 3 Use 'Obj::findMatchingPaths' to look up the
        //:   names we just created and verify that the returned names are the
        //:   full set of utf-8 names we created.  (C-3)
        //:
        //: 4 In Windows, use the wchar_t path lookup function to look up each
        //:   wchar_t name and verify that it is the correct name. (C-4)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: UTF-8 Filenames\n"
                             "========================\n";

        static const wchar_t *const filenames[] = {
            L"\u00de\u0127\u2021\u20ac\u00b2\u2116",
            L"\u03b6\u0434\u05d8\u0679\u0564\u0e3f",
           // Parens to quell concatenation warning
           (L"a\u0303a\u030ae\u0300e\u0301i\u0302i\u0308o\u0302o\u0303u"
                                                            L"\u0300u\u0301"),
#ifndef BSLS_PLATFORM_OS_DARWIN
    // Darwin canonicalizes these into the above name.
            L"\u00e3\u00e5\u00e8\u00e9\u00ee\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"a\u0303\u00e5\u00e8\u00e9\u00ee\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3a\u030a\u00e8\u00e9\u00ee\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5e\u0300\u00e9\u00ee\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8e\u0301\u00ee\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9i\u0302\u00ef\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9\u00eei\u0308\u00f4\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9\u00ee\u00efo\u0302\u00f5\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9\u00ee\u00ef\u00f4o\u0303\u00f9\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9\u00ee\u00ef\u00f4\u00f5u\u0300\u00fa",
            L"\u00e3\u00e5\u00e8\u00e9\u00ee\u00ef\u00f4\u00f5\u00f9u\u0301",
#endif
        };
        static const size_t NUM_FILES = sizeof filenames / sizeof *filenames;

        // make sure there isn't an unfortunately named file in the way

        bsl::string dir = "bdls_filesystemutil.temp.18";
        bsl::string logPath = dir;

        Obj::remove(dir, true);
        bdls::PathUtil::appendRaw(&logPath, "logs20");

        ASSERT(0 == Obj::createDirectories(logPath.c_str(), true));

        for (size_t fi = 0; fi < NUM_FILES; ++fi) {
            const wchar_t *const NAME = filenames[fi];
            bsl::string          narrow;
            bsl::wstring         wide;
            int                  rc;

            // Verify that converting the filenames from wchar_t to utf-8 and
            // back again leaves them unchanged.

#ifdef BSLS_PLATFORM_OS_WINDOWS  // Posix 'wcout' is too chancy to try.
            if (veryVerbose) {
                int mode = _setmode(_fileno(stdout), _O_U16TEXT);

                wcout << L"\tfi = " << fi << L", NAME = " << NAME << endl;
                _setmode(_fileno(stdout), mode);
            }
#endif

            rc = bdlde::CharConvertUtf16::utf16ToUtf8(&narrow, NAME);

            LOOP_ASSERT(rc, rc == 0);

            rc = bdlde::CharConvertUtf16::utf8ToUtf16(&wide, narrow.c_str());

            LOOP_ASSERT(rc, rc == 0);
            ASSERT(bsl::wstring(NAME) == wide);

            bsl::string name = logPath;
            bdls::PathUtil::appendRaw(&name, (narrow + ".log").c_str());

            if (veryVerbose) { T_; cout << "Creating file: "; P(name); }

            Obj::FileDescriptor fd = Obj::open(name,
                                               Obj::e_OPEN_OR_CREATE,
                                               Obj::e_READ_WRITE);

            LOOP_ASSERT(fd, Obj::k_INVALID_FD != fd);

            Obj::write(fd, name.c_str(), static_cast<int>(name.length()));
            Obj::close(fd);
        }

        for (size_t ni = 0; ni < NUM_NAMES; ++ni) {
            const char *const NAME = NAMES[ni];

            bsl::string name = logPath;
            bdls::PathUtil::appendRaw(&name, NAME);
            name += ".log";

            if (veryVerbose) { T_; cout << "Creating file: "; P(name); }

            Obj::FileDescriptor fd = Obj::open(name,
                                               Obj::e_OPEN_OR_CREATE,
                                               Obj::e_READ_WRITE);

#ifdef BSLS_PLATFORM_OS_WINDOWS
            LOOP2_ASSERT(fd, ni,
                               (ni == NAME_ANSI) == (Obj::k_INVALID_FD == fd));
#else
            LOOP_ASSERT(fd, Obj::k_INVALID_FD != fd);
#endif

            if (Obj::k_INVALID_FD != fd) {
                Obj::write(fd, name.c_str(), static_cast<int>(name.length()));
                Obj::close(fd);
            }
        }

        bsl::vector<bsl::string> results;
        bsl::string              pattern = logPath;

        bdls::PathUtil::appendRaw(&pattern, "*.log");
        int rc = Obj::findMatchingPaths(&results, pattern.c_str());
        LOOP_ASSERT(results.size(),
                                NUM_FILES + NUM_VALID_NAMES == results.size());
        ASSERT(NUM_FILES + NUM_VALID_NAMES == rc);

        rc = Obj::findMatchingPaths(&results, pattern);
        LOOP_ASSERT(results.size(),
                                NUM_FILES + NUM_VALID_NAMES == results.size());
        ASSERT(NUM_FILES + NUM_VALID_NAMES == rc);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        // Use the Windows 'wchar_t' interface to find the files, showing that
        // they have the correct 16-bit filenames.

        for (size_t i = 0; i < NUM_FILES; ++i) {
            WIN32_FIND_DATAW   findDataW;
            const bsl::wstring name = bsl::wstring(filenames[i]) + L".log";
            const bsl::wstring path =
                            L"bdls_filesystemutil.temp.18\\logs20\\" + name;

            if (veryVerbose) {
                int mode = _setmode(_fileno(stdout), _O_U16TEXT);

                wcout << L"\ti = " << i << L", path = " << path << endl;
                _setmode(_fileno(stdout), mode);
            }

            const HANDLE handle = FindFirstFileW(path.c_str(), &findDataW);

            LOOP_ASSERT(handle, INVALID_HANDLE_VALUE != handle);
            ASSERT(bsl::wstring(findDataW.cFileName) == name);

            FindClose(handle);
        }

        // Use the Windows 'A' interface to find the files, showing that they
        // have the correct 8-bit filenames.

        for (size_t i = 0; i < NUM_NAMES; ++i) {
            WIN32_FIND_DATAA findDataA;
            const bsl::string name = bsl::string(NAMES[i]) + ".log";
            const bsl::string path =
                           "bdls_filesystemutil.temp.18" PS "logs20" PS + name;

            if (veryVerbose) { T_; P_(i); P(name); }

            const HANDLE handle = FindFirstFileA(path.c_str(), &findDataA);

            // 'NAME_UTF8' will fail to match a file through the 'A' interface.
            // 'NAME_ANSI' will have failed on the 'open' call above, so the
            // file will not exist at all.

            LOOP2_ASSERT(handle, i,
                        (i == NAME_ASCII) == (INVALID_HANDLE_VALUE != handle));

            if (INVALID_HANDLE_VALUE != handle) {
                FindClose(handle);
            }
        }
#endif

        // Check that the filenames we get are the ones we made (but not
        // necessarily in the same order).

        typedef bsl::map<bsl::string, int> FileSet;
        FileSet fileset;

        for (size_t i = 0; i < NUM_FILES; ++i) {
            // Increment count for each file found.

            ++fileset[results[i]];

            if (veryVerbose) {
                T_; cout << "Found file "; P_(i); P(results[i])
            }

            bsl::string path = logPath;
            bsl::string name;

            bdlde::CharConvertUtf16::utf16ToUtf8(&name, filenames[i]);
            bdls::PathUtil::appendRaw(&path, (name + ".log").c_str());

            // Decrement count for each file expected.

            --fileset[path];
        }

        for (size_t i = NUM_FILES; i < results.size(); ++i) {
            // Increment count for each file found.

            ++fileset[results[i]];

            if (veryVerbose) {
                T_; cout << "Found file "; P_(i); P(results[i])
            }

            bsl::string path = logPath;
            bsl::string name = NAMES[i - NUM_FILES];

#ifdef BSLS_PLATFORM_OS_WINDOWS
            if (i == NUM_FILES + NAME_ANSI) {
                // This file name was not UTF-8, so we should have failed to
                // create it.

                continue;
            }
#endif

            bdls::PathUtil::appendRaw(&path, (name + ".log").c_str());

            // Decrement count for each file expected.

            --fileset[path];
        }

        // Check that found and expected file sets are the same.

        for (FileSet::const_iterator itr = fileset.begin(),
                                     end = fileset.end();
            itr != end; ++itr) {

            if (veryVerbose) {
                T_; cout << "Checking file "; P(itr->first)
            }

            LOOP2_ASSERT(itr->first, itr->second, 0 == itr->second);
        }

        LOOP_ASSERT(dir, 0 == Obj::remove(dir, true));

        // Check narrow and wide lookups.

        for (size_t i = 0; i < NUM_NAMES; ++i) {
            const char *const NAME = NAMES[i];

            Obj::FileDescriptor fd = Obj::open(NAME,
                                               Obj::e_OPEN_OR_CREATE,
                                               Obj::e_READ_WRITE);

#ifdef BSLS_PLATFORM_OS_WINDOWS
            LOOP2_ASSERT(fd, i, (NAME_ANSI == i) == (Obj::k_INVALID_FD == fd));

            if (Obj::k_INVALID_FD != fd) {
                Obj::close(fd);
            }
#else
            LOOP3_ASSERT(i, NAME, fd, Obj::k_INVALID_FD != fd);

            Obj::close(fd);
#endif

            bsl::vector<bsl::string> results;

            if (veryVerbose) { T_; cout << "Finding "; P(NAME); }

            int rc = Obj::findMatchingPaths(&results, NAME);

            if (!e_IS_UNIX && NAME_ANSI == i) {
                ASSERT(rc < 0);
                LOOP_ASSERT(results.size(), 0 == results.size());
                ASSERT(0 != Obj::remove(NAME, false));
            }
            else {
                ASSERT(1 == rc);
                LOOP_ASSERT(results.size(), 1    == results.size());
                LOOP_ASSERT(results[0],     NAME == results[0]);

                LOOP_ASSERT(NAME, 0 == Obj::remove(NAME, false));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING: Unix File Permissions for 'createDirectories' et al
        //
        // Concerns:
        //: 1 The permissions of a file created with 'createDirectories' on
        //:   unix are chmod 0777.  Although not (currently) contractually
        //:   guaranteed, this matches the behavior for std::fstream and is
        //:   consistent with the use of a umask (see DRQS 40563234).
        //
        // Plan:
        //: 1 Create a directory
        //: 2 Read its permissions via 'stat64' or 'stat'.
        //: 3 Observe that the permission are chmod 0777 (C-1).
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "TESTING: Unix File Permissions for 'createDirectories' et al\n"
            "===========================================================\n";

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose) cout << "TEST SKIPPED ON WINDOWS\n";
#else
        umask(0);

        if (verbose) cout << "Testing 'createDirectories'\n";
        {
            const bsl::string& testBaseDir = ::tempFileName(test,
                                         "tmp.bdls_filesystemutil_17.mkdir1");
            bsl::string fullPath = testBaseDir;
            bdls::PathUtil::appendRaw(&fullPath, "dir2");

            if (veryVerbose) { P(fullPath); }

            (void) Obj::remove(testBaseDir, true);

            int rc = Obj::createDirectories(fullPath, true);
            ASSERT(0 == rc);

            ASSERT(Obj::exists(testBaseDir));
            ASSERT(Obj::exists(fullPath));

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
  || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
            struct stat info;
            ASSERT(0 == ::stat(  fullPath.c_str(), &info));
# else
            struct stat64 info;
            ASSERT(0 == ::stat64(fullPath.c_str(), &info));
# endif
            info.st_mode &= 0777;

            enum { EXPECTED_PERMS = S_IRUSR|S_IWUSR|S_IXUSR |
                                    S_IRGRP|S_IWGRP|S_IXGRP |
                                    S_IROTH|S_IWOTH|S_IXOTH };

            const bool eqLeafDir = EXPECTED_PERMS == info.st_mode;

            if (veryVeryVerbose || !eqLeafDir) {
                bsl::ios_base::fmtflags flags = cout.flags();
                cout << bsl::oct << "Leaf dir: ";
                P_(EXPECTED_PERMS);    P(info.st_mode);
                cout.flags(flags);
            }
            ASSERT(eqLeafDir);

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
  || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
            ASSERT(0 == ::stat(  testBaseDir.c_str(), &info));
# else
            ASSERT(0 == ::stat64(testBaseDir.c_str(), &info));
# endif
            info.st_mode &= 0777;

            const bool eqBaseDir = EXPECTED_PERMS == info.st_mode;

            if (veryVeryVerbose || !eqBaseDir) {
                bsl::ios_base::fmtflags flags = cout.flags();
                cout << bsl::oct << "Base dir: ";
                P_(EXPECTED_PERMS);    P(info.st_mode);
                cout.flags(flags);
            }
            ASSERT(eqBaseDir);

            ASSERT(0 == Obj::remove(testBaseDir, true));
        }

        if (verbose) cout << "Testing 'createPrivateDirectory'\n";
        {
            const bsl::string& fullPath = ::tempFileName(test,
                                         "tmp.bdls_filesystemutil_17.mkdir1");

            if (veryVerbose) { P(fullPath); }

            (void) Obj::remove(fullPath, true);  // just a file

            int rc = Obj::createPrivateDirectory(fullPath);
            ASSERT(0 == rc);
            ASSERT(Obj::exists(fullPath));

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
  || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
            struct stat info;
            ASSERT(0 == ::stat(  fullPath.c_str(), &info));
# else
            struct stat64 info;
            ASSERT(0 == ::stat64(fullPath.c_str(), &info));
# endif
            info.st_mode &= 0777;

            enum { EXPECTED_PERMS = S_IRUSR|S_IWUSR|S_IXUSR };
            const bool eqLeafDir = EXPECTED_PERMS == info.st_mode;

            if (veryVeryVerbose || !eqLeafDir) {
                bsl::ios_base::fmtflags flags = cout.flags();
                cout << bsl::oct << "Leaf dir: ";
                P_(EXPECTED_PERMS);    P(info.st_mode);
                cout.flags(flags);
            }
            ASSERT(eqLeafDir);
            ASSERT(0 == Obj::remove(fullPath, true));
        }
#endif
      } break;
      case 18: {
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
            typedef Obj::FileDescriptor FD;

            const bsl::string& testFile = ::tempFileName(test,
                                       "tmp.bdls_filesystemutil_16.open.txt");
            if (veryVerbose) P(testFile);

            (void) Obj::remove(testFile, false);

            FD fd = Obj::open(testFile,
                              Obj::e_OPEN_OR_CREATE,
                              Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fd);

            const char *str = "To be or not to be\n";
            const int len = static_cast<int>(bsl::strlen(str));
            ASSERT(len == Obj::write(fd, str, len));

            ASSERT(0 == Obj::close(fd));

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
  || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
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

            ASSERT(0 == Obj::remove(testFile, false));
        }

        if (verbose) cout << "Testing private 'open'\n";
        {
            typedef Obj::FileDescriptor FD;

            const bsl::string& testFile = ::tempFileName(test,
                                     "tmp.bdls_filesystemutil_16_pv.open.txt");
            if (veryVerbose) P(testFile);

            (void) Obj::remove(testFile, false);

            FD fd = Obj::open(testFile,
                              Obj::e_CREATE_PRIVATE,
                              Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fd);

            const char *str = "To be or not to be\n";
            const int len = static_cast<int>(bsl::strlen(str));
            ASSERT(len == Obj::write(fd, str, len));

            ASSERT(0 == Obj::close(fd));

# if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
  || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
            struct stat info;
            ASSERT(0 == ::stat(  testFile.c_str(), &info));
# else
            struct stat64 info;
            ASSERT(0 == ::stat64(testFile.c_str(), &info));
# endif
            info.st_mode &= 0777;
            const bool eq = (S_IRUSR|S_IWUSR) == info.st_mode;
            if (veryVerbose || !eq) {
                bsl::ios_base::fmtflags flags = cout.flags();
                cout << bsl::oct;
                P_((S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH));
                P(info.st_mode);
                cout.flags(flags);
            }
            ASSERT(eq);

            ASSERT(0 == Obj::remove(testFile, false));
        }
#endif
      } break;
      case 17: {
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

        typedef Obj::FileDescriptor FD;

        const char *testFile = "tmp.bdls_filesystemutil_15.append.txt";
        const char *tag1     = "tmp.bdls_filesystemutil_15.tag.1.txt";
        const char *success  = "tmp.bdls_filesystemutil_15.success.txt";

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

            FD fd = Obj::open(testFile,
                              Obj::e_OPEN_OR_CREATE,
                              Obj::e_READ_APPEND);      // append mode;
            ASSERT(Obj::k_INVALID_FD != fd);

            int rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            Obj::Offset off =Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
            ASSERT(0 == off);

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 2 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
            ASSERT(2 * SZ10 == off);

            // Next, see what happens when another file desc from the same
            // process writes to the end of the file.

            FD fd2 = Obj::open(testFile,
                               Obj::e_OPEN,
                               Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fd2);

            off = Obj::seek(fd2, 0, Obj::e_SEEK_FROM_END);
            LOOP_ASSERT(off, 2 * SZ10 == off);

            rc = Obj::write(fd2, testString, SZ10);
            ASSERT(SZ10 == rc);
            LOOP_ASSERT(fs, 3 * SZ10 == (fs = Obj::getFileSize(testFile)));

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 4 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
            LOOP_ASSERT(off, 4 * SZ10 == off);

            bsl::string cmd = taskAbsolutePath + ' ' + argv[1] + " child";
            cmd += verbose     ? " v" : "";
            cmd += veryVerbose ? " v" : "";

            ::localForkExec(cmd);

            // On a very busy machine, it may take a long while for the child
            // process to get started.

            bsls::TimeInterval t, timeout = bdlt::CurrentTime::now() + 20.0;

            while (!Obj::exists(tag1) &&
                                    (t = bdlt::CurrentTime::now()) < timeout) {
                if (veryVerbose) Q(Parent sleeping);
                ::localSleep(1);
            }
            ASSERTV((t - timeout).totalSecondsAsDouble(), t < timeout);
            ASSERT(Obj::exists(tag1));
            if (verbose && Obj::exists(tag1)) Q(Parent detected tag1);

            rc = Obj::write(fd, testString, SZ10);
            ASSERT(SZ10 == rc);

            LOOP_ASSERT(fs, 6 * SZ10 == (fs = Obj::getFileSize(testFile)));
            off = Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
            LOOP_ASSERT(off, 6 * SZ10 == off);

            ASSERT(Obj::exists(success));

#if defined(BSLS_PLATFORM_OS_UNIX)
            int childSts;
            ::wait(&childSts);
            ASSERT(0 == childSts);
#endif

            Obj::close(fd);
            Obj::close(fd2);
            Obj::remove(testFile);
            Obj::remove(tag1);
            Obj::remove(success);

            if (verbose) Q(Parent finished);
        }
        else {
            // child process

            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = false;

            if (verbose) cout << "APPEND TEST -- CHILD\n"
                                 "====================\n";

            ASSERT(0 == Obj::setWorkingDirectory(origWorkingDirectory));

            ASSERT(Obj::exists(testFile));
            LOOP_ASSERT(fs, 4 * SZ10 == (fs = Obj::getFileSize(testFile)));

            FD fdChild = Obj::open(testFile,
                                   Obj::e_OPEN,
                                   Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdChild);

            Obj::Offset off = Obj::seek(fdChild, 0, Obj::e_SEEK_FROM_END);
            LOOP_ASSERT(off, 4 * SZ10 == off);

            int rc = Obj::write(fdChild, testString, SZ10);
            ASSERT(SZ10 == rc);

            if (0 == testStatus) {
                ::localTouch(success);
            }

            ::localTouch(tag1);

            Obj::close(fdChild);

            if (verbose) Q(Child finished);

            // Exit main to avoid doing cleanup at end of 'main' twice.

            return testStatus;                                        // RETURN
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING: close
        //
        // Note that this is a white-box test that aims to verify the
        // underlying system call is called with the appropriate arguments (it
        // is not a test of the operating system behavior).
        //
        // Concerns:
        //: 1 On success the supplied file descriptor is closed for further
        //:   reads.
        //:
        //: 2 'close' returns 0 on success, and a non-zero value on error.
        //:
        //: 3 'close' returns 'k_BAD_FILE_DESCRIPTOR' if supplied an invalid
        //:   file descriptor.
        //:
        //
        //:Plan:
        //: 1 Call open, write to a file, then call 'close'.  Verify that
        //:   'close' returns 0, and that attempts to subsequently write to
        //:   the file fail.
        //:
        //: 2 Call open with an invalid file descriptor, ensure it returns
        //:   'k_BAD_FILE_DESCRIPTOR'
        //:
        //
        // Testing:
        //   int close(FileDescriptor )
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'close'\n"
                             "================\n";

        typedef Obj::FileDescriptor FD;

        if (verbose) {
            cout << "\tTesting successful 'close'" << endl;
        }
        {
            const char *filename = "tmp.filesystemutil.close.success";
            FD fd = Obj::open(filename, Obj::e_CREATE, Obj::e_READ_WRITE);

            ASSERT(Obj::k_INVALID_FD != fd);
            ASSERT(1 == Obj::write(fd, filename, 1));


            int rc = Obj::close(fd);
            ASSERTV(rc, 0 == rc);

            ASSERT(0 != Obj::write(fd, filename, 1));
        }

        if (verbose) {
            cout << "\tTesting 'close' on invalid handle" << endl;
        }
        {
            int rc = Obj::close(Obj::k_INVALID_FD);
            ASSERTV(rc, Obj::k_BAD_FILE_DESCRIPTOR == rc);

        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING: sync
        //
        // Note that this is a white-box test that aims to verify the
        // underlying system call is called with the appropriate arguments (it
        // is not a test of the operating system behavior).
        //
        // Unfortunately, I been unable to find an effective test for concerns
        // 1, 2, and 3, since I've been unable to observe memory pages *not*
        // synchronized to disk.
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
        //   int sync(char *, int , bool)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'sync'\n"
                             "===============\n";

        typedef Obj::FileDescriptor FD;

        // Note that there appear to be '#define' for PAGESIZE and PAGE_SIZE
        // on AIX.

        const int MYPAGESIZE = bdls::MemoryUtil::pageSize();
        const int SIZE       = MYPAGESIZE;
        const int READ       = bdls::MemoryUtil::k_ACCESS_READ;
        const int READ_WRITE = bdls::MemoryUtil::k_ACCESS_READ |
                               bdls::MemoryUtil::k_ACCESS_WRITE;
        int         rc     = 0;
        Obj::Offset offset = 0;

        bsl::string testFileName(::tempFileName(test));
        Obj::remove(testFileName);
        FD writeFd = Obj::open(testFileName,
                               Obj::e_OPEN_OR_CREATE,
                               Obj::e_READ_WRITE);
        FD readFd  = Obj::open(testFileName,
                               Obj::e_OPEN,
                               Obj::e_READ_ONLY);

        ASSERT(Obj::k_INVALID_FD != writeFd);
        ASSERT(Obj::k_INVALID_FD != readFd);

        offset = Obj::seek(writeFd, SIZE, Obj::e_SEEK_FROM_BEGINNING);
        ASSERT(SIZE == offset);
        rc = Obj::write(writeFd, testFileName.c_str(), 1);
        ASSERT(1 == rc);

        offset = Obj::seek(writeFd, 0, Obj::e_SEEK_FROM_BEGINNING);
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
#ifndef BSLS_ASSERT_IS_ACTIVE
            if (veryVerbose) {
                cout << "\tTesting msync returns an error status" << endl;
            }

            // Note that, experimentally, the only sane way to force an error
            // code from sync is to pass a address that is not aligned on a
            // page boundary.  This can be done only when assertions are not
            // enabled.

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
#endif // BSLS_PLATFORM_OS_UNIX
#endif // BSLS_ASSERT_IS_ACTIVE
        }
        {
            bsls::AssertTestHandlerGuard hG;
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

        rc = Obj::unmap(readMemory,  SIZE);
        ASSERT(0 == rc);

        rc = Obj::remove(testFileName);
        ASSERT(0 == rc);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TRYLOCK TEST
        //
        // Concerns:
        //   That 'tryLock' returns proper status on failure, depending upon
        //   the type of failure.
        //
        // Plan:
        //   This test tracks 5 files, 3 files which are for testing locking,
        //   and 2 'touch' files which the child process touches to communicate
        //   with the parent process.
        //
        //   The 'unlocked' file is not locked by the parent, and the child
        //   verifies that it can lock it for read or write.
        //
        //   The 'writeLocked' file is locked for write by the parent, and
        //   the child verifies that it can't lock it in any way.
        //
        //   The 'readLocked' file is locked for read by the parent, and the
        //   child verifies that it can't lock it for write, but it can lock
        //   it for read.
        //
        //   The 'touchOnSuccess' file is touched by the child only if the
        //   child succeeds at all the locking tests.
        //
        //   The 'touchWhenDone' file is touched by the child only after tests
        //   are completed and the child has either touched the
        //   'touchOnSuccess' file or decided not to touch it because some
        //   failure has occurred.  While the child is executing, the parent is
        //   in a wait loop waiting for the 'touchWhenDone' file to come into
        //   existence.
        //
        // Testing:
        //   int tryLock(FileDescriptor, bool)
        // --------------------------------------------------------------------

        typedef Obj::FileDescriptor FD;

        // Note if we are the parent process, we are in a directory
        // 'tmpWorkingDirectory' that was set up at the start of 'main' that
        // contains the process id in the path so we don't have to create veiry
        // creative file names to avoid collisions.  We can just create all our
        // temp files in the current working dir.

        // The child process will 'chdir' to 'origWorkingDirectory', which is
        // the current directory it inherited from the parent process, so if it
        // refers to the same local file names, they will match.

        // It is important not to use '::tempFileName' here because otherwise
        // the parent and child will have different file names.

        bsl::string unlockedFn     = "tmp.filesystemutil.case_12.unlock.txt";
        bsl::string writeLockedFn  = "tmp.filesystemutil.case_12.write.txt";
        bsl::string readLockedFn   = "tmp.filesystemutil.case_12.read.txt";
        bsl::string touchOnSuccess = "tmp.filesystemutil.case_12.success.txt";
        bsl::string touchWhenDone  = "tmp.filesystemutil.case_12.done.txt";

        FD fdUnlocked = Obj::k_INVALID_FD;
        FD fdWrite    = Obj::k_INVALID_FD;
        FD fdRead     = Obj::k_INVALID_FD;

        bool isParent = !verbose || bsl::string(argv[2]) != "child";
        if (isParent) {
            if (verbose) cout << "TRYLOCK TEST -- PARENT STARTING\n"
                                 "===============================\n";

            if (veryVerbose) {
                P(writeLockedFn);    P(readLockedFn);    P(touchOnSuccess);
                P(touchWhenDone);
            }

            Obj::remove(unlockedFn);
            Obj::remove(writeLockedFn);
            Obj::remove(readLockedFn);
            Obj::remove(touchOnSuccess);
            Obj::remove(touchWhenDone);

            // Create the unlocked, write-locked, and read-locked files.

            fdUnlocked = Obj::open(unlockedFn,
                                   Obj::e_OPEN_OR_CREATE,
                                   Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdUnlocked);

            fdWrite = Obj::open(writeLockedFn,
                                Obj::e_OPEN_OR_CREATE,
                                Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdWrite);

            ::localTouch(readLockedFn);
            fdRead = Obj::open(readLockedFn, Obj::e_OPEN, Obj::e_READ_ONLY);
            ASSERT(Obj::k_INVALID_FD != fdRead);

            // Lock the read and write files.

            ASSERT(0 == Obj::tryLock(fdWrite, true));
            ASSERT(0 == Obj::tryLock(fdRead,  false));

            // Fork the child process.

            bsl::string cmd = taskAbsolutePath + ' ' + argv[1] + " child";
            cmd += (verbose     ? " v" : "");
            cmd += (veryVerbose ? " v" : "");

            ::localForkExec(cmd);

            // Wait until the child process signals it is finished by touching
            // the 'done' file.

            for (int i = 0; ! Obj::exists(touchWhenDone) && i < 120; ++i) {
                ::localSleep(1);
            }
            ASSERT(Obj::exists(touchWhenDone) &&
                                                "timed out after two minutes");

            // See if the child process passed all its tests.

            ASSERT(Obj::exists(touchOnSuccess));

            // Attempt to lock a read-only file for write, which should fail.

            ASSERT(0 != Obj::tryLock(fdRead, true));

            // unlock / close

            ASSERT(0 == Obj::unlock(fdWrite));
            ASSERT(0 == Obj::unlock(fdRead));
            ASSERT(0 == Obj::close(fdUnlocked));
            ASSERT(0 == Obj::close(fdWrite));
            ASSERT(0 == Obj::close(fdRead));

            // try to lock a closed file descriptor, which should fail

            ASSERT(0 != Obj::tryLock(fdWrite, false));

            // try to lock an invalid file descriptor, which should fail

            ASSERT(0 != Obj::tryLock(Obj::k_INVALID_FD, false));

            if (verbose) cout << "TRYLOCK TEST -- PARENT FINISHED\n"
                                 "===============================\n";

            // No need to clean up tmp files, the directory they're in will be
            // cleaned up at the end of 'main'.
        }
        else {
            // child process

            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = false;

            if (verbose) cout << "TRYLOCK TEST -- CHILD STARTING\n"
                                 "==============================\n";

            // chdir to 'origWorkingDirectory' which is the working directory
            // we inherited from the parent process, and the directory where
            // the tmp files are.

            ASSERT(0 == Obj::setWorkingDirectory(origWorkingDirectory));

            // Verify the tmp files are in the state we expect.

            ASSERT(Obj::exists(unlockedFn));
            ASSERT(Obj::exists(writeLockedFn));
            ASSERT(Obj::exists(readLockedFn));
            ASSERT(! Obj::exists(touchOnSuccess));
            ASSERT(! Obj::exists(touchWhenDone));

            // Open the unlocked file.

            fdUnlocked = Obj::open(unlockedFn,
                                   Obj::e_OPEN,
                                   Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdUnlocked);

            // Open the write file.

            fdWrite = Obj::open(writeLockedFn,
                                Obj::e_OPEN,
                                Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdWrite);

            // Open the read file.  Note we open fdRead for 'write' so we can
            // try to lock it for write later

            fdRead  = Obj::open(readLockedFn,
                                Obj::e_OPEN,
                                Obj::e_READ_WRITE);
            ASSERT(Obj::k_INVALID_FD != fdRead);

            // Verify we can lock the unlocked file for read or write.

            ASSERT(0 == Obj::tryLock(fdUnlocked, false));
            ASSERT(0 == Obj::unlock(fdUnlocked));
            ASSERT(0 == Obj::tryLock(fdUnlocked, true));

            // Try to lock the write file for write again, which should fail.

            ASSERT(Obj::k_ERROR_LOCKING_CONFLICT ==
                                                 Obj::tryLock(fdWrite, true));

            // Attempt to lock the write file for read, which should also fail.

            ASSERT(Obj::k_ERROR_LOCKING_CONFLICT ==
                                                 Obj::tryLock(fdWrite, false));

            // Attempt to lock the read file for write, which should also fail.

            ASSERT(Obj::k_ERROR_LOCKING_CONFLICT ==
                                                   Obj::tryLock(fdRead, true));

            // Attempt to lock the read file for read, which should succeed.

            ASSERT(0 == Obj::tryLock(fdRead, false));

            // unlock / close everything

            ASSERT(0 == Obj::unlock(fdUnlocked));
            ASSERT(0 == Obj::unlock(fdRead));
            ASSERT(0 == Obj::close(fdUnlocked));
            ASSERT(0 == Obj::close(fdWrite));
            ASSERT(0 == Obj::close(fdRead));

            if (0 == testStatus) {
                // No ASSERT's have failed in the child process.  Touch the
                // 'success' file to tell the parent process we succeeded.

                ::localTouch(touchOnSuccess);
            }

            if (verbose) cout << "TRYLOCK TEST -- CHILD FINISHED\n"
                                 "==============================\n";

            // Touch the 'done' file to tell the parent process we're finished.

            ::localTouch(touchWhenDone);

            // Exit 'main' to avoid doing the cleanup at the end of 'main'
            // twice.

            return 0;                                                 // RETURN
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // GETFILESIZELIMIT TEST
        //
        // Testing:
        //   static Offset getFileSizeLimit()
        // --------------------------------------------------------------------

        if (verbose) cout << "getFileSizeLimit test\n"
                             "=====================\n";

        Obj::Offset limit = Obj::getFileSizeLimit();

        ASSERT(limit > 0);
        ASSERT(limit > (1LL << 32));

        if (verbose) P(limit);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // APPEND TEST -- SINGLE PROCESS
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
        //   FD open(const char *p, bool writable, bool exist, bool append)
        // --------------------------------------------------------------------

        if (verbose) cout << "APPEND TEST -- SINGLE PROCESS\n"
                             "=============================\n";

        bsl::string fileName(::tempFileName(test));

        if (verbose) { P(fileName); }

        Obj::FileDescriptor fd = Obj::open(fileName,
                                           Obj::e_OPEN_OR_CREATE,
                                           Obj::e_READ_APPEND);
        ASSERT(Obj::k_INVALID_FD != fd);

        Obj::write(fd, "A", 1);
        char result[16];

        Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
        ASSERT(1 == Obj::read(fd, result, INT_SIZEOF(result)));

        Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
        Obj::write(fd, "B", 1);

        Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
        ASSERT(2 == Obj::read(fd, result, INT_SIZEOF(result)));

        Obj::close(fd);

        fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_APPEND);
        Obj::write(fd, "C", 1);
        Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
        ASSERT(3 == Obj::read(fd, result, INT_SIZEOF(result)));

        Obj::close(fd);

        fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
        Obj::write(fd, "D", 1);
        Obj::close(fd);

        fd = Obj::open(fileName, Obj::e_OPEN_OR_CREATE, Obj::e_READ_ONLY);
        ASSERT(3 == Obj::read(fd, result, INT_SIZEOF(result)));
        Obj::close(fd);

        Obj::remove(fileName);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MATCHING TESTS
        //
        // Concerns:
        //
        //: 1 Unix "glob()", which is called by 'Obj::visitPaths', which is
        //:   called by 'Obj::findMatchingPaths', is failing on IBM 64 bit,
        //:   unfortunately the test driver has not detected or reproduced this
        //:   error.  This test case is an attempt to get this test driver
        //:   reproducing the problem.
        //:
        //: 2 Unix "glob()", which is called by 'Obj::visitPaths', which is
        //:   called by 'Obj::findMatchingPaths', on Solaris, may try to use
        //:   files as directories and fail when cannot.  On Solaris we should
        //:   ignore such errors.
        //
        // Plan:
        //: 1 Create files with the pattern "woof.a.n".  Use
        //:   'Obj::findMatchingPaths' with the pattern "woof.a.?" to find
        //:   them.  Verify that they are found.  (C-1)
        //:
        //: 2 Create a directory structure that has a file that matches a
        //:   pattern that is for a directory in the search pattern.  Verify
        //:   that the 'Obj::findMatchingPaths' call succeeds anyway.
        //
        // Testing:
        //   int findMatchingPaths(vector<string> *, const char *)
        // --------------------------------------------------------------------

        if (verbose) cout << "MATCHING TESTS\n"
                             "==============n";

        if (veryVerbose) cout << "Simple 'findMatchingPaths' test.\n";
        {
            for (int i = 0; i < 4; ++i) {
                char name[16];
                sprintf(name, "woof.a.%d", i);
                Obj::FileDescriptor fd = Obj::open(name,
                                                   Obj::e_OPEN_OR_CREATE,
                                                   Obj::e_READ_WRITE);
                Obj::close(fd);
            }

            vector<string> paths;
            int rc = Obj::findMatchingPaths(&paths, "woof.a.?");
            ASSERT(4 == rc);
            sort(paths.begin(), paths.end());

            ASSERT(paths.size() == 4);
            ASSERT(paths[0] == "woof.a.0");
            ASSERT(paths[1] == "woof.a.1");
            ASSERT(paths[2] == "woof.a.2");
            ASSERT(paths[3] == "woof.a.3");

            ASSERT(0 == Obj::remove("woof.a.0"));
            ASSERT(0 == Obj::remove("woof.a.1"));
            ASSERT(0 == Obj::remove("woof.a.2"));
            ASSERT(0 == Obj::remove("woof.a.3"));
        }

        if (veryVerbose) cout << "Test 'ENOTDIR' failure mode.\n";
        {
            // Create directories and files of this tree:
            //   tmp
            //   |--px0
            //   |  `-- log
            //   |       `-- a.log
            //   |--px1
            //   |   `-- log
            //   |         `-- a.log
            //   |--px2
            //   |  `-- log
            //   |       `-- a.log
            //   `-- px_not_dir

            const char *name0 = "tmp" PS "px0" PS "log" PS "a.log";
            const char *name1 = "tmp" PS "px1" PS "log" PS "a.log";
            const char *name2 = "tmp" PS "px2" PS "log" PS "a.log";
            const char *name3 = "tmp" PS "px_not_dir";

            ASSERT(Obj::createDirectories(name0) == 0);
            ASSERT(Obj::createDirectories(name1) == 0);
            ASSERT(Obj::createDirectories(name2) == 0);
            ASSERT(Obj::createDirectories(name3) == 0);

            Obj::FileDescriptor fd = Obj::open(name0,
                                               Obj::e_OPEN_OR_CREATE,
                                               Obj::e_READ_WRITE);
            Obj::close(fd);
            fd = Obj::open(name1, Obj::e_OPEN_OR_CREATE, Obj::e_READ_WRITE);
            Obj::close(fd);
            fd = Obj::open(name2, Obj::e_OPEN_OR_CREATE, Obj::e_READ_WRITE);
            Obj::close(fd);
            fd = Obj::open(name3, Obj::e_OPEN_OR_CREATE, Obj::e_READ_WRITE);
            Obj::close(fd);

            // Verify that "px_not_a_dir" does not kill our 'glob' traversal

            vector<string> paths;
            int rc = Obj::findMatchingPaths(&paths,
                                            "tmp" PS "*" PS "log" PS "*");
            ASSERTV(rc, 3 == rc);
            sort(paths.begin(), paths.end());

            ASSERTV(paths.size(), paths.size() == 3);
            ASSERTV(paths[0], paths[0] == name0);
            ASSERTV(paths[1], paths[1] == name1);
            ASSERTV(paths[2], paths[2] == name2);

            // Clean up the directories

            ASSERT(Obj::remove("tmp", true) == 0);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'getFileSize'
        //
        // Concerns:
        //: 1 'getFileSize' returns the number of bytes stored any of the
        //:   following file system constructs:
        //:   1 a normal file,
        //:   2 a normal directory (for which it uses an empty directory),
        //:   3 a symbolic link (on Unix systems only),
        //:   4 a non-existent file, and
        //:   5 a normal file accessed through a relative path
        //:
        //: 2 The overload of 'getFileSize' that accepts a file descriptor
        //:   returns the number of bytes stored in the described file.
        //
        // Plan:
        //: 1 For each file system construct 'C' in the list of constructs in
        //:   Concern 1, do the following:
        //:
        //:   1 Create an instance of 'C' on the file system having a known
        //:     size 'S'.
        //:
        //:   2 Invoke 'getFileSize' with the name of 'C'.
        //:
        //:   3 Observe that 'getFileSize' returns 'S'.
        //:
        //: 2 Create a set of temporary files having different sizes on the
        //:   file system, and for each file 'F' with size 'S', open a file
        //:   descriptor 'FD' to 'F' and observe that 'getFileSize(FD)' returns
        //:   'S'.
        //:
        //: 3 Verify that all control flow paths of the overload of
        //:   'getFileSize' that accepts a file descriptor are correct on each
        //:   platform by using a mock system interface to check the behavior
        //:   of 'getFileSize' for all possible system responses, for
        //:   extreme input and output values, and for input values that affect
        //:   the control flow of the function.
        //
        // Testing:
        //   Offset getFileSize(const bsl::string&)
        //   Offset getFileSize(const char *)
        //   Offset getFileSize(FileDescriptor)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'getFileSize'\n"
                             "=====================\n";

        // const char * is tested with each 'string' type
        testCase10_getFileSize<bsl::string>("bsl::string",
                                            tmpWorkingDir,
                                            test,
                                            verbose,
                                            veryVerbose,
                                            veryVeryVerbose);
        testCase10_getFileSize<std::string>("std::string",
                                            tmpWorkingDir,
                                            test,
                                            verbose,
                                            veryVerbose,
                                            veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase10_getFileSize<std::pmr::string>("std::pmr::string",
                                                 tmpWorkingDir,
                                                 test,
                                                 verbose,
                                                 veryVerbose,
                                                 veryVeryVerbose);
#endif
      } break;
     case 9: {
        // --------------------------------------------------------------------
        // TESTING 'getSystemTemporaryDirectory'
        //   The only possible ways to obtain the required data (for Unix and
        //   Windows respectively) are already used in the method.  And it
        //   would be pointless to repeat these approaches in the test.
        //   Therefore, we will only check that the specified directory exists,
        //   that the user can create and delete files from it.
        //
        // Concerns:
        //
        //: 1 The returned value indicates the path to an existing directory
        //:
        //: 2 User can create files in the specified directory and delete files
        //:   from it.
        //
        // Plan:
        //: 1 Using 'getSystemTemporaryDirectory' obtain path to the directory
        //:   and verify it exists.  (C-1)
        //:
        //: 2 Create file in the specified directory, verify it shown up and
        //:   delete it.  (C-2)
        //
        // Testing
        //    int getSystemTemporaryDirectory(bsl::string *path);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'getSystemTemporaryDirectory'\n"
                             "=====================================\n";

        testCase9_createSystemTemporaryDirectory<bsl::string>("bsl::string",
                                                              tmpWorkingDir,
                                                              verbose,
                                                              veryVerbose,
                                                              veryVeryVerbose);
        testCase9_createSystemTemporaryDirectory<std::string>("std::string",
                                                              tmpWorkingDir,
                                                              verbose,
                                                              veryVerbose,
                                                              veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase9_createSystemTemporaryDirectory<std::pmr::string>(
            "std::pmr::string",
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'getAvailableSpace'
        //
        // Concern: 'getAvailableSpace' works.
        //
        // Plan:
        //   Just call the function and check it returns a non-negative result.
        //   We cannot verify it properly.
        //
        // Testing:
        //   Offset getAvailableSpace(const char *)
        //   Offset getAvailableSpace(FileDescriptor)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'getAvailableSpace'\n"
                             "===========================\n";

        Obj::Offset avail = Obj::getAvailableSpace(".");
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERTV(avail, 0 <= avail);

        string fileName = ::tempFileName(test);
        Obj::FileDescriptor fd = Obj::open(fileName,
                                           Obj::e_OPEN_OR_CREATE,
                                           Obj::e_READ_WRITE);
        ASSERT(Obj::k_INVALID_FD != fd);

        avail = Obj::getAvailableSpace(fd);
        if (veryVerbose) {
            cout << "Avail = " << avail << endl;
        }
        ASSERT(0 <= avail);

        Obj::close(fd);
        Obj::remove(fileName);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'rollFileChain'
        //
        // Concern: Files are rolled.
        //
        // Plan:
        //   Create tmpFile and roll until .1-.3 exist, and verify that they
        //   contain 0-2 (3 rolled off the end).
        //
        // Testing:
        //    int rollFileChain(const bsl::string&, int)
        //    int rollFileChain(const char *, int)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'rollFileChain' (files)\n"
                             "===============================\n";

        enum { MAXSUFFIX=3 };
        bsl::string tmpFile(::tempFileName(test));
        Obj::FileDescriptor f;

        ASSERT(0 == Obj::rollFileChain(tmpFile, MAXSUFFIX));

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            f = Obj::open(tmpFile,
                          Obj::e_OPEN_OR_CREATE,
                          Obj::e_READ_WRITE);
            LOOP_ASSERT(tmpFile, f != Obj::k_INVALID_FD);
            ASSERT(INT_SIZEOF(int) == Obj::write(f, &i, INT_SIZEOF(int)));
            ASSERT(0 == Obj::close(f));

            // Roll the file(s).

            ASSERT(0 == Obj::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != Obj::remove(tmpFile)); // does not exist
        tmpFile += ".0";
        int pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = Obj::open(tmpFile,
                          Obj::e_OPEN,
                          Obj::e_READ_ONLY); // must exist
            LOOP_ASSERT(tmpFile, f != Obj::k_INVALID_FD);
            ASSERT(INT_SIZEOF(int) == Obj::read(f, &value, INT_SIZEOF(int)));
            ASSERT(0 == Obj::close(f));
            ASSERT(0 == Obj::remove(tmpFile));
            LOOP2_ASSERT(i, value, i == value);
        }

        if (verbose) {
           cout << "rollFileChain test (directories)" << endl;
        }
        tmpFile = "tmpDir";

        for (int i = MAXSUFFIX; i >= 0; --i) {
            // Create the file - place index into it.

            ASSERT(0 == Obj::createDirectories(tmpFile, true));
            f = Obj::open(tmpFile + PS + "file",
                          Obj::e_OPEN_OR_CREATE,
                          Obj::e_READ_WRITE);
            ASSERT(f != Obj::k_INVALID_FD);
            ASSERT(INT_SIZEOF(int) == Obj::write(f, &i, INT_SIZEOF(int)));
            ASSERT(0 == Obj::close(f));

            // Roll the file(s).

            ASSERT(0 == Obj::rollFileChain(tmpFile, MAXSUFFIX));
        }
        ASSERT(0 != Obj::remove(tmpFile, true)); // does not exist
        tmpFile += ".0";
        pos = (int) tmpFile.length()-1;

        for (int i = 0; i < MAXSUFFIX; ++i) {
            int value = -1;
            tmpFile[pos] = "123456789"[i];
            f = Obj::open(tmpFile + PS + "file",
                          Obj::e_OPEN,
                          Obj::e_READ_ONLY);
            ASSERT(f != Obj::k_INVALID_FD);
            ASSERT(INT_SIZEOF(int) == Obj::read(f, &value, INT_SIZEOF(int)));
            ASSERT(0 == Obj::close(f));
            ASSERT(0 == Obj::remove(tmpFile, true));
            LOOP2_ASSERT(i, value, i == value);
        }

        {
            // Test using string_view substring of a name
            tmpFile = "tmpDir-string_view-test";
            ASSERT(0 == Obj::createDirectories(tmpFile, true));

            bsl::string filename = tmpFile + PS + "a";
            bsl::vector<bsl::string> filenameVector;

            filenameVector.reserve(4);

            makeArbitraryFile(filename.c_str());
            ASSERT(true == Obj::exists(filename));
            filenameVector.push_back(filename);

            filename += "b";
            makeArbitraryFile(filename.c_str());
            ASSERT(true == Obj::exists(filename));

            // Don't add this filename to the vector, since it won't exist
            // after the rollFileChain call.

            bsl::string partialFilename = filename;

            filename += "c";
            makeArbitraryFile(filename.c_str());
            ASSERT(true == Obj::exists(filename));
            filenameVector.push_back(filename);

            filename += "d";
            makeArbitraryFile(filename.c_str());
            ASSERT(true == Obj::exists(filename));
            filenameVector.push_back(filename);

            bsl::string_view partialView(filename.data(),
                                         filename.length() - 2);
            ASSERT(partialView == partialFilename);
            ASSERT(true == Obj::exists(partialFilename));
            ASSERT(0 == Obj::rollFileChain(partialView, MAXSUFFIX));
            ASSERT(false == Obj::exists(partialFilename));
            bsl::string rolledFilename = partialFilename + ".1";
            ASSERT(true == Obj::exists(rolledFilename));

            // Make sure the other filenames remain unchanged, and delete them.
            for (bsl::vector<bsl::string>::const_iterator i =
                     filenameVector.begin();
                 i != filenameVector.end();
                 ++i) {
                ASSERTV(*i, true == Obj::exists(*i));
            }

            // Delete the whole directory
            ASSERT(0 == Obj::remove(tmpFile, true));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'move(const char *, const char *)
        //
        // Concern: This function works, including on filenames which do not
        //          exist at all or are (on Windows) invalid UTF-8.
        //
        // Plan:    Populate a test vector with a list of different files and
        //          directories to test.
        //
        // Testing:
        //    bool move(const char *, const char *)
        //    bool move(STR_TYPE, STR_TYPE)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'move'\n"
                             "==============\n";

        // Test NAME_ASCII and NAME_UTF8 on both platforms, and NAME_ANSI on
        // Unix.

        // Expect move-to-self to succeed on Unix.

#ifndef BSLS_PLATFORM_OS_WINDOWS
        testCase6_move_normal(NAMES[NAME_ASCII],
                              NAMES[NAME_ASCII],
                              false,
                              false,
                              verbose,
                              veryVerbose,
                              veryVeryVerbose);
#endif

        testCase6_move_normal(NAMES[NAME_ASCII],
                              NAMES[NAME_UTF8],
                              false,
                              false,
                              verbose,
                              veryVerbose,
                              veryVeryVerbose);

        testCase6_move_normal(NAMES[NAME_UTF8],
                              NAMES[NAME_ASCII],
                              false,
                              false,
                              verbose,
                              veryVerbose,
                              veryVeryVerbose);


#ifdef BSLS_PLATFORM_OS_WINDOWS
        // Ensure that NAME_ANSI results in a "false" result for either
        // argument on windows
        const bool isWindows = true;
#else
        const bool isWindows = false;
#endif

        testCase6_move_normal(NAMES[NAME_ANSI],
                              NAMES[NAME_ASCII],
                              isWindows,
                              false,
                              verbose,
                              veryVerbose,
                              veryVeryVerbose);

        testCase6_move_normal(NAMES[NAME_ASCII],
                              NAMES[NAME_ANSI],
                              isWindows,
                              isWindows,
                              verbose,
                              veryVerbose,
                              veryVeryVerbose);


        // Test the various type combinations for the templated method, in both
        // call orders
#define CASE_6_TEST_TEMPLATE_2_TYPES(TYPE1, TYPE2)                            \
    testCase6_move_template<TYPE1, TYPE2>(                                    \
        (TYPE1)(const_cast<char *>(NAMES[NAME_ASCII])),                       \
        (TYPE2)(const_cast<char *>(NAMES[NAME_UTF8])),                        \
        false,                                                                \
        false,                                                                \
        verbose,                                                              \
        veryVerbose,                                                          \
        veryVeryVerbose)

#define CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(TYPE1, TYPE2)                   \
        CASE_6_TEST_TEMPLATE_2_TYPES(TYPE1, TYPE2);                           \
        CASE_6_TEST_TEMPLATE_2_TYPES(TYPE2, TYPE1);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#define CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS_PMR(TYPE1)                      \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(std::pmr::string, TYPE1);
#else
#define CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS_PMR(TYPE1)
#endif

#define CASE_6_TEST_TEMPLATE(TYPE1)                                           \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(char *, TYPE1);                 \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(const char *, TYPE1);           \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(bsl::string, TYPE1);            \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(std::string, TYPE1);            \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS_PMR(TYPE1);                     \
        CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS(bslstl::StringRef, TYPE1);

        CASE_6_TEST_TEMPLATE(char*);
        CASE_6_TEST_TEMPLATE(const char*);
        CASE_6_TEST_TEMPLATE(bsl::string);
        CASE_6_TEST_TEMPLATE(std::string);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        CASE_6_TEST_TEMPLATE(std::pmr::string);
#endif
        CASE_6_TEST_TEMPLATE(bslstl::StringRef);

#undef CASE_6_TEST_TEMPLATE
#undef CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS_PMR
#undef CASE_6_TEST_TEMPLATE_BOTH_CALL_ORDERS
#undef CASE_6_TEST_TEMPLATE_2_TYPES
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'isRegularFile' & 'isDirectory'
        //
        // Concern: These functions work, including on filenames which do not
        //          exist at all.
        //
        // Plan:
        //   Permutate a test vector with a list of different files and
        //   directories to test.
        //
        // Testing:
        //    bool isRegularFile(const bsl::string&, bool)
        //    bool isRegularFile(const char *, bool)
        //    bool isDirectory(const bsl::string&, bool)
        //    bool isDirectory(const char *, bool)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'isRegularFile' & 'isDirectory'\n"
                             "=======================================\n";

        testCase5_isRegularFile_isDirectory<bsl::string>("bsl::string",
                                                         test,
                                                         tmpWorkingDir,
                                                         verbose,
                                                         veryVerbose,
                                                         veryVeryVerbose);
        testCase5_isRegularFile_isDirectory<std::string>("std::string",
                                                         test,
                                                         tmpWorkingDir,
                                                         verbose,
                                                         veryVerbose,
                                                         veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase5_isRegularFile_isDirectory<std::pmr::string>(
            "std::pmr::string",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#endif

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING pattern matching
        //
        // Concerns:
        //: 1 Both '*' and '?' characters are supported, and can appear in
        //:   multiple directories in the path.
        //:
        //: 2 Paths can be specified either as a 'const char *' or a
        //:   'bsl::string'.
        //:
        //: 3 The return value properly returns the number of files matched,
        //:   including 0 in the case where no files are matched.
        //
        // Plan:
        //: 1 Make sure both '*' and '?' characters are supported with
        //:   'findMatchingPath'.
        //:
        //: 2 Always loop to test both the 'bsl::string' and 'const char *'
        //:   pattern cases.
        //:
        //: 3 Test with a pattern that matches nothing.
        //
        // Testing:
        //   int findMatchingPaths(vector<string>*, const char *)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing pattern matching\n"
                             "========================\n";

        // Note that "const char *" is not tested as an explicit string type,
        // since it's tested as part of each function invocation.
        testCase4_findMatchingPaths<bsl::vector<bsl::string> >(
            "bsl::vector<bsl::string>>",
            "bsl::string_view",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
        testCase4_findMatchingPaths<std::vector<std::string> >(
            "std::vector<std::string>>",
            "bsl::string_view",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase4_findMatchingPaths<std::pmr::vector<std::pmr::string> >(
            "std::pmr::vector<std::pmr::string>>",
            "bsl::string_view",
            test,
            tmpWorkingDir,
            verbose,
            veryVerbose,
            veryVeryVerbose);
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OPEN TEST
        //
        // Concerns:
        //: 1 (For deprecated overload) A 'write' puts data at the end of the
        //:   file when open in append mode.
        //:
        //: 2 (For deprecated overload) A 'write' puts data at the end of the
        //:   file when open in append mode even after a seek.
        //:
        //: 3 (For deprecated overload) 'isAppend' is default to 'false'.
        //:
        //: 4 An existing file can be opened when 'OPEN' is specified, and a
        //:   non-existent file causes an error when 'OPEN' is specified.
        //:
        //: 5 An existing file causes an error when 'CREATE' is specified, and
        //:   a non-existent file can be opened when 'CREATE' is specified.
        //:
        //: 6 An existing file can be opened when 'OPEN_OR_CREATE' is
        //:   specified, and a non-existent file can be opened when
        //:   'OPEN_OR_CREATE' is specified.
        //:
        //: 7 Reading is possible when 'READ_ONLY' is specified, and writing is
        //:   impossible when 'READ_ONLY' is specified.
        //:
        //: 8 Reading is impossible when 'WRITE_ONLY' is specified, and writing
        //:   is possible when 'WRITE_ONLY' is specified.
        //:
        //: 9 Reading is impossible when 'APPEND_ONLY' is specified, and
        //:   writing is possible when 'APPEND_ONLY' is specified.
        //:
        //:10 Reading is possible when 'READ_WRITE' is specified, and writing
        //:   is possible when 'READ_WRITE' is specified.
        //:
        //:11 Reading is possible when 'READ_APPEND' is specified, and writing
        //:   is possible when 'READ_APPEND' is specified.
        //:
        //:12 When either 'APPEND_ONLY' or 'READ_APPEND' is specified, all
        //:   writes are made at the end of file.
        //:
        //:13 When 'TRUNCATE' is specified, the previous contents of the file,
        //:   if any, are discarded.
        //:
        //:14 When 'KEEP' is specified, the previous contents of the file, if
        //:   any, are preserved.
        //
        // Plan:
        //: 1 Create a file in append mode, write a character, use seek to
        //:   change the position of output, write another character, and
        //:   verify that the new character is added after the original
        //:   character.
        //:
        //: 2 Reopen the file in append mode, write a character and ensure that
        //:   it is added to the end of the file.
        //:
        //: 3 Reopen the file in normal mode, write a character and ensure that
        //:   it overwrites the data in the file instead of appending to it.
        //:
        //: 4  Note that the !EXISTS case should be tested before the EXISTS
        //: cases, so that we can establish that 'open' is capable of creating
        //: files before we create files to prepare for subsequent tests.
        //
        // Testing:
        //   FD open(const char *path, openPolicy, ioPolicy, truncatePolicy)
        //   FD open(const string& path, openPolicy, ioPolicy, truncatePolicy)
        // --------------------------------------------------------------------

        if (verbose) cout << "OPEN TEST\n"
                             "=========\n";

        enum {
            k_READ_BUFFER_SIZE = 64
        };

        if (verbose) cout << "\nBootstrap, phase 1: 'open' file mode."
                          << endl;
        {
            const Obj::FileOpenPolicy OP = Obj::e_OPEN;
            const Obj::FileOpenPolicy CR = Obj::e_CREATE;
            const Obj::FileOpenPolicy OC = Obj::e_OPEN_OR_CREATE;

            const Obj::FileIOPolicy RO = Obj::e_READ_ONLY;
            const Obj::FileIOPolicy WO = Obj::e_WRITE_ONLY;
            const Obj::FileIOPolicy AO = Obj::e_APPEND_ONLY;
            const Obj::FileIOPolicy RW = Obj::e_READ_WRITE;
            const Obj::FileIOPolicy RA = Obj::e_READ_APPEND;

            const Obj::FileTruncatePolicy TR = Obj::e_TRUNCATE;
            const Obj::FileTruncatePolicy KP = Obj::e_KEEP;

            const struct DataRow {
                int d_line;
                Obj::FileOpenPolicy d_openPolicy;
                Obj::FileIOPolicy d_ioPolicy;
                Obj::FileTruncatePolicy d_initPolicy;
                bool d_exists;
                bool d_success;
            } DATA[] = {
                //LN openPolicy ioPolicy initPolicy exists success
                //-- ---------- -------- ---------- ------ -------
                {L_, CR,        WO,      KP,       false,   true },
                {L_, CR,        WO,      KP,        true,  false },
                {L_, OP,        WO,      KP,       false,  false },
                {L_, OP,        WO,      KP,        true,   true },
                {L_, OC,        WO,      KP,       false,   true },
                {L_, OC,        WO,      KP,        true,   true },

                {L_, CR,        RO,      KP,       false,   true },
                {L_, CR,        RO,      KP,        true,  false },
                {L_, OP,        RO,      KP,       false,  false },
                {L_, OP,        RO,      KP,        true,   true },
                {L_, OC,        RO,      KP,       false,   true },
                {L_, OC,        RO,      KP,        true,   true },

                {L_, CR,        AO,      KP,       false,   true },
                {L_, CR,        AO,      KP,        true,  false },
                {L_, OP,        AO,      KP,       false,  false },
                {L_, OP,        AO,      KP,        true,   true },
                {L_, OC,        AO,      KP,       false,   true },
                {L_, OC,        AO,      KP,        true,   true },

                {L_, CR,        RW,      KP,       false,   true },
                {L_, CR,        RW,      KP,        true,  false },
                {L_, OP,        RW,      KP,       false,  false },
                {L_, OP,        RW,      KP,        true,   true },
                {L_, OC,        RW,      KP,       false,   true },
                {L_, OC,        RW,      KP,        true,   true },

                {L_, CR,        RA,      KP,       false,   true },
                {L_, CR,        RA,      KP,        true,  false },
                {L_, OP,        RA,      KP,       false,  false },
                {L_, OP,        RA,      KP,        true,   true },
                {L_, OC,        RA,      KP,       false,   true },
                {L_, OC,        RA,      KP,        true,   true },

                {L_, CR,        WO,      TR,       false,   true },
                {L_, CR,        WO,      TR,        true,  false },
                {L_, OP,        WO,      TR,       false,  false },
                {L_, OP,        WO,      TR,        true,   true },
                {L_, OC,        WO,      TR,       false,   true },
                {L_, OC,        WO,      TR,        true,   true },

                {L_, CR,        RO,      TR,       false,   true },
                {L_, CR,        RO,      TR,        true,  false },
                {L_, OP,        RO,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        RO,      TR,        true,  false },
                {L_, OC,        RO,      TR,       false,   true },
                {L_, OC,        RO,      TR,        true,   true },

                {L_, CR,        AO,      TR,       false,   true },
                {L_, CR,        AO,      TR,        true,  false },
                {L_, OP,        AO,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        AO,      TR,        true,  false },
                {L_, OC,        AO,      TR,       false,   true },
                {L_, OC,        AO,      TR,        true,   true },

                {L_, CR,        RW,      TR,       false,   true },
                {L_, CR,        RW,      TR,        true,  false },
                {L_, OP,        RW,      TR,       false,  false },
                {L_, OP,        RW,      TR,        true,   true },
                {L_, OC,        RW,      TR,       false,   true },
                {L_, OC,        RW,      TR,        true,   true },

                {L_, CR,        RA,      TR,       false,   true },
                {L_, CR,        RA,      TR,        true,  false },
                {L_, OP,        RA,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        RA,      TR,        true,  false },
                {L_, OC,        RA,      TR,       false,   true },
                {L_, OC,        RA,      TR,        true,   true },
            };
            enum { NUM_DATA = sizeof(DATA) / sizeof(DataRow) };

            bool isCreateFileTested = false;

            bsl::string fileName(::tempFileName(test));

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                  LINE        = DATA[ti].d_line;
                const bool                 EXISTS      = DATA[ti].d_exists;
                const bool                 EXP_SUCCESS = DATA[ti].d_success;

                const Obj::FileOpenPolicy     OPEN_POLICY
                                                       = DATA[ti].d_openPolicy;
                const Obj::FileIOPolicy       IO_POLICY
                                                       = DATA[ti].d_ioPolicy;
                const Obj::FileTruncatePolicy INIT_POLICY
                                                       = DATA[ti].d_initPolicy;

                if (veryVerbose) {
                    T_;
                    P_(OPEN_POLICY); P_(IO_POLICY); P_(INIT_POLICY); P(EXISTS);
                }

                // Establish a consistent baseline: 'fileName' does not exist.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }

                LOOP2_ASSERT(LINE, fileName, !Obj::exists(fileName));

                // If this test expects 'fileName' to exist, it must be created
                // using the (already-tested) 'e_CREATE' open mode.

                if (EXISTS) {
                    // We cannot use the combination of 'e_CREATE' with
                    // 'e_WRITE_ONLY' until it has been tested.

                    LOOP_ASSERT(LINE, isCreateFileTested);

                    Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_WRITE_ONLY);
                    LOOP_ASSERT(LINE, Obj::k_INVALID_FD != fd);

                    Obj::close(fd);

                    LOOP2_ASSERT(LINE, fileName, Obj::exists(fileName));
                    LOOP_ASSERT(LINE, 0 == Obj::getFileSize(fileName));
                }

                // Test open success

                Obj::FileDescriptor fd = Obj::open(
                                fileName, OPEN_POLICY, IO_POLICY, INIT_POLICY);
                LOOP3_ASSERT(LINE, EXP_SUCCESS, fd,
                                     EXP_SUCCESS == (Obj::k_INVALID_FD != fd));

                if (Obj::k_INVALID_FD == fd) {
                    // Expected failure.  We can do no more testing with an
                    // invalid file descriptor, so go on to the next test.

                    continue;
                }

                // File Descriptor Attribute Testing

#ifndef BSLS_PLATFORM_OS_WINDOWS
                switch (IO_POLICY) {
                  case Obj::e_READ_ONLY: {
                    int flags = fcntl(fd, F_GETFL);
                    LOOP2_ASSERT(LINE, IO_POLICY,
                                               O_RDONLY == (flags & O_RDONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_WRONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_APPEND));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDWR));
                  } break;
                  case Obj::e_WRITE_ONLY: {
                    int flags = fcntl(fd, F_GETFL);
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY,
                                               O_WRONLY == (flags & O_WRONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_APPEND));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDWR));
                  } break;
                  case Obj::e_APPEND_ONLY: {
                    int flags = fcntl(fd, F_GETFL);
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY,
                                               O_WRONLY == (flags & O_WRONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY,
                                               O_APPEND == (flags & O_APPEND));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDWR));
                  } break;
                  case Obj::e_READ_WRITE: {
                    int flags = fcntl(fd, F_GETFL);
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_WRONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_APPEND));
                    LOOP2_ASSERT(LINE, IO_POLICY, O_RDWR == (flags & O_RDWR));
                  } break;
                  case Obj::e_READ_APPEND: {
                    int flags = fcntl(fd, F_GETFL);
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_RDONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY, 0 == (flags & O_WRONLY));
                    LOOP2_ASSERT(LINE, IO_POLICY,
                                               O_APPEND == (flags & O_APPEND));
                    LOOP2_ASSERT(LINE, IO_POLICY, O_RDWR == (flags & O_RDWR));
                  } break;
                  default: {
                      ASSERT(false);
                  }
                }
#endif

                if (   Obj::e_CREATE     == OPEN_POLICY
                    && Obj::e_WRITE_ONLY == IO_POLICY  ) {
                    isCreateFileTested = true;
                }

                Obj::close(fd);
                Obj::remove(fileName);
            }
        }

        if (verbose) cout << "\nBootstrap, phase 2: basic IO test."
                          << endl;
        {
            bsl::string fileName(::tempFileName(test));

            if (Obj::exists(fileName)) {
                Obj::remove(fileName);
            }
            LOOP_ASSERT(fileName, !Obj::exists(fileName));

            Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
            LOOP_ASSERT(fileName, Obj::k_INVALID_FD != fd);

            const char blockA[]   = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            const int  numBytesA  =
                            static_cast<int>(sizeof(blockA) / sizeof(*blockA));
            const char blockB[]   = { '0', '1', '2', '3', '4' };
            const int  numBytesB  =
                            static_cast<int>(sizeof(blockB) / sizeof(*blockB));

            char buffer[k_READ_BUFFER_SIZE];

#ifndef BSLS_PLATFORM_OS_WINDOWS
            // e_READ_WRITE supports writing

            ASSERT(0 == lseek(fd, 0, SEEK_END));
            ASSERT(numBytesA == write(fd, blockA, numBytesA));
            ASSERT(numBytesA == lseek(fd, 0, SEEK_CUR));
            ASSERT(numBytesA == lseek(fd, 0, SEEK_END));

            // e_READ_WRITE supports reading

            ASSERT(0 == lseek(fd, 0, SEEK_SET));
            memset(buffer, 0, k_READ_BUFFER_SIZE);
            int numBytes =
                        static_cast<int>(read(fd, buffer, k_READ_BUFFER_SIZE));
            LOOP2_ASSERT(numBytes, numBytesA, numBytes == numBytesA);
            LOOP2_ASSERT(blockA, buffer,
                                       0 == memcmp(blockA, buffer, numBytesA));

            // e_READ_WRITE write is not append

            ASSERT(0 == lseek(fd, 0, SEEK_SET));
            ASSERT(numBytesB == write(fd, blockB, numBytesB));
            LOOP2_ASSERT(blockA, blockB, numBytesA > numBytesB);
            ASSERT(numBytesB == lseek(fd, 0, SEEK_CUR));
            ASSERT(numBytesA == lseek(fd, 0, SEEK_END));
#else
            // e_READ_WRITE supports writing

            ASSERT(0 == SetFilePointer(fd, 0, NULL, FILE_END));
            DWORD numBytes;
            ASSERT(WriteFile(fd, blockA, numBytesA, &numBytes, NULL));
            LOOP2_ASSERT(numBytes, numBytesA, numBytes == numBytesA);
            ASSERT(numBytesA == SetFilePointer(fd, 0, NULL, FILE_CURRENT));
            ASSERT(numBytesA == SetFilePointer(fd, 0, NULL, FILE_END));

            // e_READ_WRITE supports reading

            ASSERT(0 == SetFilePointer(fd, 0, NULL, FILE_BEGIN));
            memset(buffer, 0, k_READ_BUFFER_SIZE);
            ASSERT(ReadFile(fd, buffer, k_READ_BUFFER_SIZE, &numBytes, NULL));
            LOOP2_ASSERT(numBytes, numBytesA, numBytes == numBytesA);
            LOOP2_ASSERT(blockA, buffer,
                                       0 == memcmp(blockA, buffer, numBytesA));

            // e_READ_WRITE write is not append

            ASSERT(0 == SetFilePointer(fd, 0, NULL, FILE_BEGIN));
            ASSERT(WriteFile(fd, blockB, numBytesB, &numBytes, NULL));
            LOOP2_ASSERT(blockA, blockB, numBytesA > numBytesB);
            LOOP2_ASSERT(numBytes, numBytesB, numBytes == numBytesB);
            ASSERT(numBytesB == SetFilePointer(fd, 0, NULL, FILE_CURRENT));
            ASSERT(numBytesA == SetFilePointer(fd, 0, NULL, FILE_END));
#endif  // BSLS_PLATFORM_OS_WINDOWS

            Obj::close(fd);
            Obj::remove(fileName);
        }

        if (verbose) cout <<
                            "\nBootstrap, phase 3: 'read' and 'write'."
                          << endl;
        {
            const char blockA[]   = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            const int  numBytesA  =
                            static_cast<int>(sizeof(blockA) / sizeof(*blockA));
            const char blockB[]   = { '0', '1', '2', '3', '4' };
            const int  numBytesB  =
                            static_cast<int>(sizeof(blockB) / sizeof(*blockB));
            int        lengths[]  = { 0, 1, 2, 3, 4, 5, 6 };
            const int  numLengths =
                          static_cast<int>(sizeof(lengths) / sizeof(*lengths));
            char       buffer[k_READ_BUFFER_SIZE];

            bsl::string fileName(::tempFileName(test));

            if (veryVerbose) { cout << "\tBasic write" << endl; }

            // Basic write (lengths 0, 1, N)

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                // Write data to empty file.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP2_ASSERT(lengths[i], fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesWritten = Obj::write(fd, blockA, lengths[i]);
                if (0 <= lengths[i]) {
                    LOOP2_ASSERT(lengths[i], bytesWritten,
                                                   lengths[i] == bytesWritten);
                }
                else {
                    LOOP2_ASSERT(lengths[i], bytesWritten, 0 > bytesWritten);
                }

                // Confirm file has expected size.

                Obj::close(fd);
                if (0 <= lengths[i]) {
                    LOOP2_ASSERT(lengths[i], bytesWritten,
                                   bytesWritten == Obj::getFileSize(fileName));
                }
                Obj::remove(fileName);
            }

            // Write data to empty file.

            {
                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);
            }

            if (veryVerbose) { cout << "\tBasic read" << endl; }

            // Basic read (lengths 0, 1, N)

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                LOOP2_ASSERT(lengths[i], fileName, Obj::exists(fileName));

                // Read data from file.
                // A: Read amount requested.

                Obj::FileDescriptor fd = Obj::open(
                                     fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesRead = Obj::read(fd, buffer, lengths[i]);

                // Confirm length of data.

                LOOP2_ASSERT(lengths[i], bytesRead, lengths[i] == bytesRead);

                // Confirm contents of data.

                LOOP_ASSERT(lengths[i],
                                       0 == memcmp(buffer, blockA, bytesRead));

                Obj::close(fd);
            }

            {
                // B: Read until file is exhausted.

                Obj::FileDescriptor fd = Obj::open(
                                     fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                int bytesRead = Obj::read(fd, buffer, numBytesA + 1);

                // Confirm length of data.

                LOOP2_ASSERT(numBytesA, bytesRead, numBytesA == bytesRead);

                // Confirm contents of data.

                ASSERT(0 == memcmp(buffer, blockA, bytesRead));

                Obj::close(fd);
            }

            Obj::remove(fileName);

            if (veryVerbose) {
                cout << "\tPositional properties of 'write'"
                     << endl;
            }

            // Positional properties of write

            // First write in existing file is at beginning.

            {
                // Prepare file with existing data.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);

                //   Re-open file and write data.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                int bytesWritten = Obj::write(fd, blockB, numBytesB);
                ASSERT(numBytesB == bytesWritten);

                Obj::close(fd);

                //   Confirm data is written to start of file.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                memset(buffer, 0, numBytesB);
                int bytesRead = Obj::read(fd, buffer, numBytesB);
                LOOP_ASSERT(bytesRead, 0 < bytesRead);

                LOOP_ASSERT(buffer[0], blockB[0] == buffer[0]);

                Obj::close(fd);
                Obj::remove(fileName);
            }

            // Write moves write position.

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                // Create new file

                // Write data of varying sizes

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP2_ASSERT(lengths[i], fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesWritten = Obj::write(fd, blockA, lengths[i]);
                LOOP2_ASSERT(lengths[i], bytesWritten,
                                                   lengths[i] == bytesWritten);

                //   Write marker.

                bytesWritten = Obj::write(fd, blockB, 1);
                LOOP2_ASSERT(lengths[i], bytesWritten,
                        bytesWritten == 1);
                Obj::close(fd);

                //   Confirm marker is in expected location.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                memset(buffer, 0, sizeof(buffer));
                int bytesRead = Obj::read(fd, buffer, lengths[i] + 1);
                LOOP2_ASSERT(lengths[i], bytesRead,
                                                  lengths[i] + 1 == bytesRead);

                for (int j = 0; j < lengths[i]; ++j) {
                    LOOP2_ASSERT(lengths[i], j, blockB[0] != buffer[j]);
                }
                LOOP2_ASSERT(lengths[i], buffer[lengths[i]],
                                              blockB[0] == buffer[lengths[i]]);

                Obj::close(fd);
                Obj::remove(fileName);
            }

            if (veryVerbose) {
                cout << "\tPositional properties of 'read'"
                     << endl;
            }

            // Positional properties of read

            // Prepare file with existing data.
            {
                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);
            }

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                //   Re-open file and read data of varying sizes
                Obj::FileDescriptor fd = Obj::open(
                                     fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesRead = Obj::read(fd, buffer, lengths[i]);
                LOOP2_ASSERT(lengths[i], bytesRead, lengths[i] == bytesRead);

                //   Then read marker.
                memset(buffer, 0, sizeof(buffer));
                bytesRead = Obj::read(fd, buffer, 1);
                LOOP2_ASSERT(lengths[i], bytesRead, 1 == bytesRead);

                //   Confirm marker is expected.
                LOOP2_ASSERT(lengths[i], buffer[0],
                                              blockA[lengths[i]] == buffer[0]);

                Obj::close(fd);

                //   Re-open file and read until near end of file.

                int fileSize = static_cast<int>(Obj::getFileSize(fileName));
                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                bytesRead = Obj::read(fd, buffer, fileSize - lengths[i]);
                LOOP3_ASSERT(lengths[i], fileSize, bytesRead,
                                           fileSize - lengths[i] == bytesRead);

                //   Read data past end of file.
                memset(buffer, 0, sizeof(buffer));
                bytesRead = Obj::read(fd, buffer, (lengths[i] * 2) + 1);

                //   Confirm read failed to read all data.
                LOOP3_ASSERT(lengths[i], fileSize, bytesRead, 0 <= bytesRead);
                LOOP3_ASSERT(lengths[i], fileSize, bytesRead,
                                             (lengths[i] * 2) + 1 > bytesRead);
                if (0 < bytesRead) {
                    LOOP3_ASSERT(lengths[i], fileSize, bytesRead,
                                   blockA[fileSize - lengths[i]] == buffer[0]);
                }

                Obj::close(fd);
            }

            Obj::remove(fileName);

            if (veryVerbose) {
                cout << "\tCombined positional properties"
                     << endl;
            }

            // Combined positional properties

            if (veryVerbose) {
                cout << "\tWrite followed by read"
                     << endl;
            }

            // Write followed by read.

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                // Prepare file with existing data.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);

                //  Re-open file and write data of various lengths.

                LOOP2_ASSERT(lengths[i], fileName, Obj::exists(fileName));

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesWritten = Obj::write(fd, blockA, lengths[i]);
                LOOP2_ASSERT(lengths[i], bytesWritten,
                                                   bytesWritten == lengths[i]);

                //  Read data.

                memset(buffer, 0, sizeof(buffer));
                int bytesRead = Obj::read(fd, buffer, 1);
                LOOP2_ASSERT(lengths[i], bytesRead, 1 == bytesRead);

                //  Confirm that read occurred at end position of write.

                LOOP2_ASSERT(lengths[i], buffer[0],
                                              buffer[0] == blockA[lengths[i]]);

                Obj::close(fd);
                Obj::remove(fileName);
            }

            if (veryVerbose) {
                cout << "\tRead followed by write"
                     << endl;
            }

            // Read followed by write.

            for (int i = 0; i < numLengths; ++i) {
                if (veryVerbose) { T_; P_(i); P(fileName); }

                // Prepare file with existing data.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);

                //  Re-open file and read data of various lengths.

                LOOP2_ASSERT(lengths[i], fileName, Obj::exists(fileName));

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                int bytesRead = Obj::read(fd, buffer, lengths[i]);
                LOOP2_ASSERT(lengths[i], bytesRead, bytesRead == lengths[i]);

                //  Write marker.

                int bytesWritten = Obj::write(fd, blockB, 1);
                LOOP2_ASSERT(lengths[i], bytesWritten, bytesWritten == 1);

                Obj::close(fd);

                //  Confirm that write began at end position of read.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                LOOP_ASSERT(lengths[i], Obj::k_INVALID_FD != fd);

                memset(buffer, 0, sizeof(buffer));
                bytesRead = Obj::read(fd, buffer, lengths[i] + 1);
                LOOP2_ASSERT(lengths[i], bytesRead,
                                                  lengths[i] + 1 == bytesRead);

                for (int j = 0; j < lengths[i]; ++j) {
                    LOOP2_ASSERT(lengths[i], j, blockB[0] != buffer[j]);
                }
                LOOP2_ASSERT(lengths[i], buffer[lengths[i]],
                                              blockB[0] == buffer[lengths[i]]);

                Obj::close(fd);
                Obj::remove(fileName);
            }

            // Negative testing
            if (verbose) cout << "\nNegative testing" << endl;
            {
                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                bsls::AssertTestHandlerGuard hG;

                ASSERT_PASS(Obj::read(fd, buffer,  1));
                ASSERT_PASS(Obj::read(fd, buffer,  0));
                ASSERT_FAIL(Obj::read(fd, buffer, -1));
                ASSERT_FAIL(Obj::read(fd, 0,       1));

                ASSERT_PASS(Obj::write(fd, blockA,  1));
                ASSERT_PASS(Obj::write(fd, blockA,  0));
                ASSERT_FAIL(Obj::write(fd, blockA, -1));
                ASSERT_FAIL(Obj::write(fd, 0,       1));

                Obj::close(fd);
                Obj::remove(fileName);
            }
        }

        if (verbose) cout << "\nBootstrap, phase 4: 'seek'."
                          << endl;
        {
            const char blockA[]   = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            const int  numBytesA  =
                            static_cast<int>(sizeof(blockA) / sizeof(*blockA));
            const char blockB[]   = { '0', '1', '2', '3', '4' };
            int        lengths[]  = { 0, 1, 2, 3, 4, 5, 6 };
            const int  numLengths =
                          static_cast<int>(sizeof(lengths) / sizeof(*lengths));
            char       buffer[k_READ_BUFFER_SIZE];

            bsl::string fileName(::tempFileName(test));

            if (veryVerbose) { cout << "\tSeek in empty file." << endl; }

            // Seek in empty file.
            {
                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                // Confirm that position starts at 0.

                Obj::Offset position =
                                    Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
                LOOP_ASSERT(position, 0 == position);

                // Confirm that seek to begin succeeds -> 0.

                position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
                LOOP_ASSERT(position, 0 == position);

                // Confirm that seek to end succeeds -> 0.

                position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_END);
                LOOP_ASSERT(position, 0 == position);

                // Confirm that seek to non-0 succeeds.

                position = Obj::seek(fd, 1, Obj::e_SEEK_FROM_BEGINNING);
                LOOP_ASSERT(position, 1 == position);

                // Confirm that seek to negative offset fails.

                position = Obj::seek(fd, -1, Obj::e_SEEK_FROM_BEGINNING);
                LOOP_ASSERT(position, -1 == position);

                Obj::close(fd);
            }

            if (veryVerbose) { cout << "\tSeek in non-empty file." << endl; }

            // Seek in non-empty file.

            {
                // Prepare file with existing data.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);

                //   Re-open file and seek.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                // Confirm that position starts at 0.
                Obj::Offset position =
                                    Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
                LOOP_ASSERT(position, 0 == position);

                for (int i = 0; i < numLengths; ++i) {
                    // Confirm that seek from end succeeds -> N.

                    position = Obj::seek(fd, lengths[i], Obj::e_SEEK_FROM_END);
                    LOOP_ASSERT(position, numBytesA + lengths[i] == position);

                    position = Obj::seek(fd, -lengths[i],
                                                         Obj::e_SEEK_FROM_END);
                    LOOP_ASSERT(position, numBytesA - lengths[i] == position);

                    // Confirm that seek from begin succeeds -> N.

                    position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, lengths[i] == position);

                    // Confirm that seek from current succeeds -> N + M.

                    Obj::Offset origin = position;
                    for (int j = 0; j < lengths[i]; ++j) {
                        // Seek forward

                        position = Obj::seek(fd, j, Obj::e_SEEK_FROM_CURRENT);
                        LOOP_ASSERT(position, origin + j == position);

                        // Reset to 'lengths[i]'.

                        position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                        LOOP_ASSERT(position, lengths[i] == position);

                        // Seek backward

                        position = Obj::seek(fd, -j, Obj::e_SEEK_FROM_CURRENT);
                        LOOP_ASSERT(position, origin - j == position);

                        // Reset to 'lengths[i]'.

                        position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                        LOOP_ASSERT(position, lengths[i] == position);
                    }
                }

                Obj::close(fd);
            }

            if (veryVerbose) { cout << "\tSeek + read." << endl; }

            // Seek + read

            {
                // Prepare file with existing data.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }
                LOOP_ASSERT(fileName, !Obj::exists(fileName));

                Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                Obj::write(fd, blockA, numBytesA);

                Obj::close(fd);

                //   Re-open file and seek.

                fd = Obj::open(fileName, Obj::e_OPEN, Obj::e_READ_WRITE);
                ASSERT(Obj::k_INVALID_FD != fd);

                for (int i = 0; i < numLengths; ++i) {
                    // Confirm that read occurs at seek point, seeking to
                    // (0, 1, N).

                    Obj::Offset position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, lengths[i] == position);

                    memset(buffer, 0, sizeof(buffer));
                    int bytesRead = Obj::read(fd, buffer, 1);
                    LOOP2_ASSERT(lengths[i], bytesRead, 1 == bytesRead);
                    LOOP2_ASSERT(lengths[i], buffer[0], blockA[lengths[i]]);

                    // Confirm that read get 0 bytes if beyond end of file.

                    position = Obj::seek(fd, lengths[i], Obj::e_SEEK_FROM_END);
                    LOOP2_ASSERT(lengths[i], position,
                                           numBytesA + lengths[i] == position);

                    memset(buffer, 0, sizeof(buffer));
                    bytesRead = Obj::read(fd, buffer, 1);
                    LOOP2_ASSERT(lengths[i], bytesRead, 0 == bytesRead);

                    // Confirm that position after read is reported by "tell"

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
                    LOOP2_ASSERT(lengths[i], position, 0 == position);

                    bytesRead = Obj::read(fd, buffer, lengths[i]);
                    LOOP2_ASSERT(lengths[i], bytesRead,
                                                      lengths[i] == bytesRead);

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
                    LOOP2_ASSERT(lengths[i], position, lengths[i] == position);
                }

                Obj::close(fd);
            }


            if (veryVerbose) { cout << "\tSeek + write." << endl; }

            // Seek + write

            {
                for (int i = 0; i < numLengths; ++i) {
                    // Prepare file> with existing data.

                    if (Obj::exists(fileName)) {
                        Obj::remove(fileName);
                    }
                    LOOP_ASSERT(fileName, !Obj::exists(fileName));

                    Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                    ASSERT(Obj::k_INVALID_FD != fd);

                    Obj::write(fd, blockA, numBytesA);

                    // Confirm that write occurs at seek point, seeking to
                    // (0, 1, N).

                    Obj::Offset position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, lengths[i] == position);

                    int bytesWritten = Obj::write(fd, blockB, 1);
                    LOOP2_ASSERT(lengths[i], bytesWritten, 1 == bytesWritten);

                    position = Obj::seek(fd, lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, lengths[i] == position);

                    memset(buffer, 0, sizeof(buffer));
                    int bytesRead = Obj::read(fd, buffer, 1);
                    LOOP2_ASSERT(lengths[i], bytesRead, 1 == bytesRead);
                    LOOP2_ASSERT(lengths[i], buffer[0], blockB[0]);

                    // Reset overwritten content

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, 0 == position);

                    Obj::write(fd, blockA, numBytesA);

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);

                    memset(buffer, 0, sizeof(buffer));
                    bytesRead = Obj::read(fd, buffer, numBytesA);
                    LOOP2_ASSERT(lengths[i], bytesRead,
                                                       numBytesA == bytesRead);
                    LOOP_ASSERT(lengths[i], 0 == memcmp(buffer, blockA,
                                                                   numBytesA));

                    // Confirm that write fails if seek point is beyond end of
                    // file.

                    position = Obj::seek(fd, lengths[i], Obj::e_SEEK_FROM_END);
                    LOOP2_ASSERT(lengths[i], position,
                                           numBytesA + lengths[i] == position);

                    bytesWritten = Obj::write(fd, blockB, 1);
                    LOOP2_ASSERT(lengths[i], bytesWritten, 1 == bytesWritten);

                    position = Obj::seek( fd, numBytesA + lengths[i],
                                                   Obj::e_SEEK_FROM_BEGINNING);
                    LOOP_ASSERT(position, numBytesA + lengths[i] == position);

                    memset(buffer, 0, sizeof(buffer));
                    bytesRead = Obj::read(fd, buffer, 1);
                    LOOP2_ASSERT(lengths[i], bytesRead, 1 == bytesRead);
                    LOOP2_ASSERT(lengths[i], buffer[0], blockB[0]);

                    // Confirm that position after write is reported by "tell"

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING);
                    LOOP2_ASSERT(lengths[i], position, 0 == position);

                    bytesWritten = Obj::write(fd, blockB, lengths[i]);
                    LOOP2_ASSERT(lengths[i], bytesWritten,
                                                   lengths[i] == bytesWritten);

                    position = Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT);
                    LOOP2_ASSERT(lengths[i], position, lengths[i] == position);

                    Obj::close(fd);
                    Obj::remove(fileName);
                }
            }
        }

        if (verbose) cout <<
                        "\nBootstrap, phase 5: 'open' with truncation."
                          << endl;
        {
            const Obj::FileOpenPolicy OP = Obj::e_OPEN;
            const Obj::FileOpenPolicy CR = Obj::e_CREATE;
            const Obj::FileOpenPolicy OC = Obj::e_OPEN_OR_CREATE;

            const Obj::FileIOPolicy RO = Obj::e_READ_ONLY;
            const Obj::FileIOPolicy WO = Obj::e_WRITE_ONLY;
            const Obj::FileIOPolicy AO = Obj::e_APPEND_ONLY;
            const Obj::FileIOPolicy RW = Obj::e_READ_WRITE;
            const Obj::FileIOPolicy RA = Obj::e_READ_APPEND;

            const Obj::FileTruncatePolicy TR = Obj::e_TRUNCATE;
            const Obj::FileTruncatePolicy KP = Obj::e_KEEP;

            const struct DataRow {
                int d_line;
                Obj::FileOpenPolicy d_openPolicy;
                Obj::FileIOPolicy d_ioPolicy;
                Obj::FileTruncatePolicy d_initPolicy;
                bool d_exists;
            } DATA[] = {
                //LN openPolicy ioPolicy initPolicy exists
                //-- ---------- -------- ---------- ------
                {L_, CR,        WO,      KP,       false  },
                {L_, OP,        WO,      KP,        true  },
                {L_, OC,        WO,      KP,       false  },
                {L_, OC,        WO,      KP,        true  },

                {L_, CR,        RO,      KP,       false  },
                {L_, OP,        RO,      KP,        true  },
                {L_, OC,        RO,      KP,       false  },
                {L_, OC,        RO,      KP,        true  },

                {L_, CR,        AO,      KP,       false  },
                {L_, OP,        AO,      KP,        true  },
                {L_, OC,        AO,      KP,       false  },
                {L_, OC,        AO,      KP,        true  },

                {L_, CR,        RW,      KP,       false  },
                {L_, OP,        RW,      KP,        true  },
                {L_, OC,        RW,      KP,       false  },
                {L_, OC,        RW,      KP,        true  },

                {L_, CR,        RA,      KP,       false  },
                {L_, OP,        RA,      KP,        true  },
                {L_, OC,        RA,      KP,       false  },
                {L_, OC,        RA,      KP,        true  },

                {L_, CR,        WO,      TR,       false  },
                {L_, OP,        WO,      TR,        true  },
                {L_, OC,        WO,      TR,       false  },
                {L_, OC,        WO,      TR,        true  },

                {L_, CR,        RO,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        RO,      TR,        true  },
                {L_, OC,        RO,      TR,       false  },
                {L_, OC,        RO,      TR,        true  },

                {L_, CR,        AO,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        AO,      TR,        true  },
                {L_, OC,        AO,      TR,       false  },
                {L_, OC,        AO,      TR,        true  },

                {L_, CR,        RW,      TR,       false  },
                {L_, OP,        RW,      TR,        true  },
                {L_, OC,        RW,      TR,       false  },
                {L_, OC,        RW,      TR,        true  },

                {L_, CR,        RA,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        RA,      TR,        true  },
                {L_, OC,        RA,      TR,       false  },
                {L_, OC,        RA,      TR,        true  },
            };
            enum { NUM_DATA = sizeof(DATA) / sizeof(DataRow) };

            bsl::string fileName(::tempFileName(test));

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                       LINE   = DATA[ti].d_line;
                const bool                      EXISTS = DATA[ti].d_exists;

                const Obj::FileOpenPolicy     OPEN_POLICY
                                                       = DATA[ti].d_openPolicy;
                const Obj::FileIOPolicy       IO_POLICY
                                                       = DATA[ti].d_ioPolicy;
                const Obj::FileTruncatePolicy INIT_POLICY
                                                       = DATA[ti].d_initPolicy;

                if (veryVerbose) {
                    T_;
                    P_(OPEN_POLICY); P_(IO_POLICY); P_(INIT_POLICY); P(EXISTS);
                }

                // Establish a consistent baseline: 'fileName' does not exist.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }

                LOOP2_ASSERT(LINE, fileName, !Obj::exists(fileName));

                // If this test expects 'fileName' to exist, it must be created
                // using the (already-tested) 'e_CREATE' open mode.

                if (EXISTS) {
                    Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                    LOOP_ASSERT(LINE, Obj::k_INVALID_FD != fd);
                    LOOP_ASSERT(LINE, 1 == Obj::write(fd, " ", 1));

                    Obj::close(fd);

                    LOOP2_ASSERT(LINE, fileName, Obj::exists(fileName));
                    LOOP_ASSERT(LINE, 1 == Obj::getFileSize(fileName));
                }

                Obj::FileDescriptor fd = Obj::open(
                                fileName, OPEN_POLICY, IO_POLICY, INIT_POLICY);
                LOOP2_ASSERT(LINE, fd, Obj::k_INVALID_FD != fd);

                if (Obj::k_INVALID_FD == fd) {
                    // We can do no more testing with an invalid file
                    // descriptor, so go on to the next test.

                    continue;
                }

                // Test truncation

                if (!EXISTS || Obj::e_TRUNCATE == INIT_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                       Obj::seek(fd, 0, Obj::e_SEEK_FROM_END));
                }
                else if (Obj::e_KEEP == INIT_POLICY) {
                    LOOP_ASSERT(LINE, 0 !=
                                       Obj::seek(fd, 0, Obj::e_SEEK_FROM_END));
                }
                else {
                    LOOP_ASSERT(LINE, false);
                }

                Obj::close(fd);
                Obj::remove(fileName);
            }
        }

        if (verbose) cout <<
                            "\nBootstrap, phase 6: 'open' with append."
                          << endl;
        {
            const Obj::FileOpenPolicy OP = Obj::e_OPEN;
            const Obj::FileOpenPolicy CR = Obj::e_CREATE;
            const Obj::FileOpenPolicy OC = Obj::e_OPEN_OR_CREATE;

            const Obj::FileIOPolicy RO = Obj::e_READ_ONLY;
            const Obj::FileIOPolicy WO = Obj::e_WRITE_ONLY;
            const Obj::FileIOPolicy AO = Obj::e_APPEND_ONLY;
            const Obj::FileIOPolicy RW = Obj::e_READ_WRITE;
            const Obj::FileIOPolicy RA = Obj::e_READ_APPEND;

            const Obj::FileTruncatePolicy TR = Obj::e_TRUNCATE;
            const Obj::FileTruncatePolicy KP = Obj::e_KEEP;

            const struct DataRow {
                int d_line;
                Obj::FileOpenPolicy d_openPolicy;
                Obj::FileIOPolicy d_ioPolicy;
                Obj::FileTruncatePolicy d_initPolicy;
                bool d_exists;
            } DATA[] = {
                //LN openPolicy ioPolicy initPolicy exists
                //-- ---------- -------- ---------- ------
                {L_, CR,        WO,      KP,       false  },
                {L_, OP,        WO,      KP,        true  },
                {L_, OC,        WO,      KP,       false  },
                {L_, OC,        WO,      KP,        true  },

                {L_, CR,        RO,      KP,       false  },
                {L_, OP,        RO,      KP,        true  },
                {L_, OC,        RO,      KP,       false  },
                {L_, OC,        RO,      KP,        true  },

                {L_, CR,        AO,      KP,       false  },
                {L_, OP,        AO,      KP,        true  },
                {L_, OC,        AO,      KP,       false  },
                {L_, OC,        AO,      KP,        true  },

                {L_, CR,        RW,      KP,       false  },
                {L_, OP,        RW,      KP,        true  },
                {L_, OC,        RW,      KP,       false  },
                {L_, OC,        RW,      KP,        true  },

                {L_, CR,        RA,      KP,       false  },
                {L_, OP,        RA,      KP,        true  },
                {L_, OC,        RA,      KP,       false  },
                {L_, OC,        RA,      KP,        true  },

                {L_, CR,        WO,      TR,       false  },
                {L_, OP,        WO,      TR,        true  },
                {L_, OC,        WO,      TR,       false  },
                {L_, OC,        WO,      TR,        true  },

                {L_, CR,        RO,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        RO,      TR,        true  },
                {L_, OC,        RO,      TR,       false  },
                {L_, OC,        RO,      TR,        true  },

                {L_, CR,        AO,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        AO,      TR,        true  },
                {L_, OC,        AO,      TR,       false  },
                {L_, OC,        AO,      TR,        true  },

                {L_, CR,        RW,      TR,       false  },
                {L_, OP,        RW,      TR,        true  },
                {L_, OC,        RW,      TR,       false  },
                {L_, OC,        RW,      TR,        true  },

                {L_, CR,        RA,      TR,       false  },
                // Truncate without Write or Create disallowed:
                // {L_, OP,        RA,      TR,        true  },
                {L_, OC,        RA,      TR,       false  },
                {L_, OC,        RA,      TR,        true  },
            };
            enum { NUM_DATA = sizeof(DATA) / sizeof(DataRow) };

            const char blockA[]  = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            const int  numBytesA = INT_SIZEOF(blockA);
            const char blockB[]  = { '0', '1', '2', '3', '4' };
            const int  numBytesB = INT_SIZEOF(blockB);

            bsl::string fileName(::tempFileName(test));

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                       LINE   = DATA[ti].d_line;
                const bool                      EXISTS = DATA[ti].d_exists;

                const Obj::FileOpenPolicy     OPEN_POLICY
                                                       = DATA[ti].d_openPolicy;
                const Obj::FileIOPolicy       IO_POLICY
                                                       = DATA[ti].d_ioPolicy;
                const Obj::FileTruncatePolicy INIT_POLICY
                                                       = DATA[ti].d_initPolicy;

                if (veryVerbose) {
                    T_;
                    P_(OPEN_POLICY); P_(IO_POLICY); P_(INIT_POLICY); P(EXISTS);
                }

                // Establish a consistent baseline: 'fileName' does not exist.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }

                LOOP2_ASSERT(LINE, fileName, !Obj::exists(fileName));

                // If this test expects 'fileName' to exist, it must be created
                // using the (already-tested) 'e_CREATE' open mode.

                if (EXISTS) {
                    Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_READ_WRITE);
                    LOOP_ASSERT(LINE, Obj::k_INVALID_FD != fd);
                    LOOP_ASSERT(LINE, 1 == Obj::write(fd, " ", 1));

                    Obj::close(fd);

                    LOOP2_ASSERT(LINE, fileName, Obj::exists(fileName));
                    LOOP_ASSERT(LINE, 1 == Obj::getFileSize(fileName));
                }

                Obj::FileDescriptor fd = Obj::open(
                                fileName, OPEN_POLICY, IO_POLICY, INIT_POLICY);
                LOOP2_ASSERT(LINE, fd, Obj::k_INVALID_FD != fd);

                if (Obj::k_INVALID_FD == fd) {
                    // We can do no more testing with an invalid file
                    // descriptor, so go on to the next test.

                    continue;
                }

                // Test append

                if (Obj::e_WRITE_ONLY  == IO_POLICY ||
                    Obj::e_READ_WRITE  == IO_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesA ==
                                            Obj::write(fd, blockA, numBytesA));
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesB ==
                                            Obj::write(fd, blockB, numBytesB));

                    LOOP_ASSERT(LINE, numBytesB ==
                                   Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT));
                }
                else if (Obj::e_APPEND_ONLY == IO_POLICY ||
                         Obj::e_READ_APPEND == IO_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesA ==
                                            Obj::write(fd, blockA, numBytesA));
                    Obj::Offset endpoint =
                                        Obj::seek(fd, 0, Obj::e_SEEK_FROM_END);
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesB ==
                            Obj::write(fd, blockB, numBytesB));

                    LOOP_ASSERT(LINE, endpoint + numBytesB ==
                                   Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT));
                }
                else {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, 0 > Obj::write(fd, blockA, numBytesA));
                }

                Obj::close(fd);
                Obj::remove(fileName);
            }

        }

        if (verbose) cout << "\nFull test of enum-based four-argument open"
                          << endl;
        {
            const Obj::FileOpenPolicy OP = Obj::e_OPEN;
            const Obj::FileOpenPolicy CR = Obj::e_CREATE;
            const Obj::FileOpenPolicy OC = Obj::e_OPEN_OR_CREATE;

            const Obj::FileIOPolicy RO = Obj::e_READ_ONLY;
            const Obj::FileIOPolicy WO = Obj::e_WRITE_ONLY;
            const Obj::FileIOPolicy AO = Obj::e_APPEND_ONLY;
            const Obj::FileIOPolicy RW = Obj::e_READ_WRITE;
            const Obj::FileIOPolicy RA = Obj::e_READ_APPEND;

            const Obj::FileTruncatePolicy TR = Obj::e_TRUNCATE;
            const Obj::FileTruncatePolicy KP = Obj::e_KEEP;

            const struct DataRow {
                int d_line;
                Obj::FileOpenPolicy d_openPolicy;
                Obj::FileIOPolicy d_ioPolicy;
                Obj::FileTruncatePolicy d_initPolicy;
                bool d_exists;
                bool d_success;
            } DATA[] = {
                //LN openPolicy ioPolicy initPolicy exists success
                //-- ---------- -------- ---------- ------ -------
                {L_, CR,        WO,      KP,       false,   true },
                {L_, CR,        WO,      KP,        true,  false },
                {L_, OP,        WO,      KP,       false,  false },
                {L_, OP,        WO,      KP,        true,   true },
                {L_, OC,        WO,      KP,       false,   true },
                {L_, OC,        WO,      KP,        true,   true },

                {L_, CR,        RO,      KP,       false,   true },
                {L_, CR,        RO,      KP,        true,  false },
                {L_, OP,        RO,      KP,       false,  false },
                {L_, OP,        RO,      KP,        true,   true },
                {L_, OC,        RO,      KP,       false,   true },
                {L_, OC,        RO,      KP,        true,   true },

                {L_, CR,        AO,      KP,       false,   true },
                {L_, CR,        AO,      KP,        true,  false },
                {L_, OP,        AO,      KP,       false,  false },
                {L_, OP,        AO,      KP,        true,   true },
                {L_, OC,        AO,      KP,       false,   true },
                {L_, OC,        AO,      KP,        true,   true },

                {L_, CR,        RW,      KP,       false,   true },
                {L_, CR,        RW,      KP,        true,  false },
                {L_, OP,        RW,      KP,       false,  false },
                {L_, OP,        RW,      KP,        true,   true },
                {L_, OC,        RW,      KP,       false,   true },
                {L_, OC,        RW,      KP,        true,   true },

                {L_, CR,        RA,      KP,       false,   true },
                {L_, CR,        RA,      KP,        true,  false },
                {L_, OP,        RA,      KP,       false,  false },
                {L_, OP,        RA,      KP,        true,   true },
                {L_, OC,        RA,      KP,       false,   true },
                {L_, OC,        RA,      KP,        true,   true },

                {L_, CR,        WO,      TR,       false,   true },
                {L_, CR,        WO,      TR,        true,  false },
                {L_, OP,        WO,      TR,       false,  false },
                {L_, OP,        WO,      TR,        true,   true },
                {L_, OC,        WO,      TR,       false,   true },
                {L_, OC,        WO,      TR,        true,   true },

                {L_, CR,        RO,      TR,       false,   true },
                {L_, CR,        RO,      TR,        true,  false },
                {L_, OP,        RO,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        RO,      TR,        true,  false },
                {L_, OC,        RO,      TR,       false,   true },
                {L_, OC,        RO,      TR,        true,   true },

                {L_, CR,        AO,      TR,       false,   true },
                {L_, CR,        AO,      TR,        true,  false },
                {L_, OP,        AO,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        AO,      TR,        true,  false },
                {L_, OC,        AO,      TR,       false,   true },
                {L_, OC,        AO,      TR,        true,   true },

                {L_, CR,        RW,      TR,       false,   true },
                {L_, CR,        RW,      TR,        true,  false },
                {L_, OP,        RW,      TR,       false,  false },
                {L_, OP,        RW,      TR,        true,   true },
                {L_, OC,        RW,      TR,       false,   true },
                {L_, OC,        RW,      TR,        true,   true },

                {L_, CR,        RA,      TR,       false,   true },
                {L_, CR,        RA,      TR,        true,  false },
                {L_, OP,        RA,      TR,       false,  false },
                // Truncate without Write or Create disallowed:
                {L_, OP,        RA,      TR,        true,  false },
                {L_, OC,        RA,      TR,       false,   true },
                {L_, OC,        RA,      TR,        true,   true },
            };
            enum { NUM_DATA = sizeof(DATA) / sizeof(DataRow) };

            const char blockA[]  = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            const int  numBytesA = INT_SIZEOF(blockA);
            const char blockB[]  = { '0', '1', '2', '3', '4' };
            const int  numBytesB = INT_SIZEOF(blockB);
            char       buffer[8];

            bool isCreateFileTested = false;

            bsl::string fileName(::tempFileName(test));

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                  LINE        = DATA[ti].d_line;
                const bool                 EXISTS      = DATA[ti].d_exists;
                const bool                 EXP_SUCCESS = DATA[ti].d_success;

                const Obj::FileOpenPolicy     OPEN_POLICY
                                                       = DATA[ti].d_openPolicy;
                const Obj::FileIOPolicy       IO_POLICY
                                                       = DATA[ti].d_ioPolicy;
                const Obj::FileTruncatePolicy INIT_POLICY
                                                       = DATA[ti].d_initPolicy;

                if (verbose) {
                    T_;
                    P_(OPEN_POLICY); P_(IO_POLICY); P_(INIT_POLICY); P(EXISTS);
                }

                // Establish a consistent baseline: 'fileName' does not exist.

                if (Obj::exists(fileName)) {
                    Obj::remove(fileName);
                }

                LOOP2_ASSERT(LINE, fileName, !Obj::exists(fileName));

                // If this test expects 'fileName' to exist, it must be created
                // using the (already-tested) 'e_CREATE' open mode.

                if (EXISTS) {
                    // We cannot use the combination of 'e_CREATE' with
                    // 'e_WRITE_ONLY' until it has been tested.

                    LOOP_ASSERT(LINE, isCreateFileTested);

                    Obj::FileDescriptor fd = Obj::open(
                                   fileName, Obj::e_CREATE, Obj::e_WRITE_ONLY);
                    LOOP_ASSERT(LINE, Obj::k_INVALID_FD != fd);
                    LOOP_ASSERT(LINE, 1 == Obj::write(fd, " ", 1));

                    Obj::close(fd);

                    LOOP2_ASSERT(LINE, fileName, Obj::exists(fileName));
                    LOOP_ASSERT(LINE, 1 == Obj::getFileSize(fileName));
                }

                // Test open success

                Obj::FileDescriptor fd = Obj::open(
                                fileName, OPEN_POLICY, IO_POLICY, INIT_POLICY);
                LOOP3_ASSERT(LINE, EXP_SUCCESS, fd,
                                     EXP_SUCCESS == (Obj::k_INVALID_FD != fd));

                if (Obj::k_INVALID_FD == fd) {
                    // Expected failure.  We can do no more testing with an
                    // invalid file descriptor, so go on to the next test.

                    continue;
                }

                // We expect to start at offset 0

                LOOP_ASSERT(LINE, 0 == Obj::seek(fd, 0,
                                                    Obj::e_SEEK_FROM_CURRENT));

                // Test truncation

                if (!EXISTS || Obj::e_TRUNCATE == INIT_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                       Obj::seek(fd, 0, Obj::e_SEEK_FROM_END));
                }
                else if (Obj::e_KEEP == INIT_POLICY) {
                    LOOP_ASSERT(LINE, 0 !=
                                       Obj::seek(fd, 0, Obj::e_SEEK_FROM_END));
                }
                else {
                    LOOP_ASSERT(LINE, false);
                }

                // Test writing

                if (Obj::e_WRITE_ONLY  == IO_POLICY ||
                    Obj::e_READ_WRITE  == IO_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesA ==
                                            Obj::write(fd, blockA, numBytesA));
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesB ==
                                            Obj::write(fd, blockB, numBytesB));

                    LOOP_ASSERT(LINE, numBytesB ==
                                   Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT));
                }
                else if (Obj::e_APPEND_ONLY == IO_POLICY ||
                         Obj::e_READ_APPEND == IO_POLICY) {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesA ==
                                            Obj::write(fd, blockA, numBytesA));
                    Obj::Offset endpoint =
                                        Obj::seek(fd, 0, Obj::e_SEEK_FROM_END);
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, numBytesB ==
                                            Obj::write(fd, blockB, numBytesB));

                    LOOP_ASSERT(LINE, endpoint + numBytesB ==
                                   Obj::seek(fd, 0, Obj::e_SEEK_FROM_CURRENT));
                }
                else {
                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, 0 > Obj::write(fd, blockA, numBytesA));
                }

                // Test reading

                if (   Obj::e_READ_ONLY   == IO_POLICY
                    || Obj::e_READ_WRITE  == IO_POLICY
                    || Obj::e_READ_APPEND == IO_POLICY) {
                    // Seek to prepared read location.

                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, 0 <= Obj::read(fd, buffer, 1));
                }
                else {
                    // Seek to prepared read location.

                    LOOP_ASSERT(LINE, 0 ==
                                 Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
                    LOOP_ASSERT(LINE, 0 > Obj::read(fd, buffer, 1));
                }

                if (   Obj::e_CREATE     == OPEN_POLICY
                    && Obj::e_WRITE_ONLY == IO_POLICY) {
                    isCreateFileTested = true;
                }

                Obj::close(fd);
                Obj::remove(fileName);
            }
        }

        if (verbose) cout << "\nNegative testing" << endl;
        {
            bsl::string fileName(::tempFileName(test));

            if (Obj::exists(fileName)) {
                Obj::remove(fileName);
            }
            LOOP_ASSERT(fileName, !Obj::exists(fileName));

            bsls::AssertTestHandlerGuard hG;

            {
                Obj::FileDescriptor fd = Obj::k_INVALID_FD;

                ASSERT_PASS(fd = Obj::open(fileName.c_str(),
                                           Obj::e_OPEN_OR_CREATE,
                                           Obj::e_READ_ONLY));

                if (fd != Obj::k_INVALID_FD) {
                    Obj::close(fd);
                }
            }

            {
                Obj::FileDescriptor fd = Obj::k_INVALID_FD;

                ASSERT_FAIL(fd = Obj::open(static_cast<const char *>(0),
                                           Obj::e_OPEN_OR_CREATE,
                                           Obj::e_READ_ONLY));

                ASSERT(Obj::k_INVALID_FD == fd);
                if (fd != Obj::k_INVALID_FD) {
                    Obj::close(fd);
                }
            }

            if (Obj::exists(fileName)) {
                Obj::remove(fileName);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FilesystemUtil_CStringUtil
        //
        // Concerns:
        //: 1 FilesystemUtil_CStringUtil::flatten correctly proxies the
        //:   null-terminated strings corresponding to 'const char *',
        //:   'bsl::string', :   'std::string', and, if available,
        //:   'std::pmr::string' arguments, without additional allocations.
        //:
        //: 2 FilesystemUtil_CStringUtil::flatten correctly proxies the
        //:   null-terminated string corresponding the 'bslstl::StringRef'
        //:   arguments, possibly allocating memory to do so.
        //:
        //: 3 FilesystemUtil_CStringUtil::flatten is not convertible from
        //:   'bsl::string_view'.
        //:
        //
        // Plan:
        //: 1 Invoke 'FilesystemUtil_CStringUtil::flatten' for each supported
        //:   type and ensure no memory is allocated (except possibly in the
        //:   'bslstl::StringRef' case).
        //:
        //: 2 Make sure 'bsl::string_view' is not convertible to
        //:   FilesystemUtil_CStringUtil.
        //
        // Testing:
        //   FilesystemUtil_CStringUtil
        // --------------------------------------------------------------------

        const char *SHORT_STRING = "short string";
        const char
            *LONG_STRING =
                "long string requires alloc for bsl::string 00123456789012";


        {
            // Test source types not expected to allocate
            bslma::TestAllocator ta("default test allocator");
            ta.setAllocationLimit(0);
            bslma::DefaultAllocatorGuard daGuard(&ta);

            // bsl::string
            {
                bslma::TestAllocator ta2("local test allocator");
                bsl::string          shortStr(SHORT_STRING, &ta2);
                testCase2_CStringUtil<bsl::string>("bsl::string",
                                                   shortStr,
                                                   SHORT_STRING,
                                                   test,
                                                   verbose,
                                                   veryVerbose,
                                                   veryVeryVerbose);

                bsl::string longStr(LONG_STRING, &ta2);
                testCase2_CStringUtil<bsl::string>("bsl::string",
                                                   longStr,
                                                   LONG_STRING,
                                                   test,
                                                   verbose,
                                                   veryVerbose,
                                                   veryVeryVerbose);
            }

            // std::string
            {
                std::string shortStr(SHORT_STRING);
                testCase2_CStringUtil<std::string>("std::string",
                                                   shortStr,
                                                   SHORT_STRING,
                                                   test,
                                                   verbose,
                                                   veryVerbose,
                                                   veryVeryVerbose);

                std::string longStr(LONG_STRING);
                testCase2_CStringUtil<std::string>("std::string",
                                                   longStr,
                                                   LONG_STRING,
                                                   test,
                                                   verbose,
                                                   veryVerbose,
                                                   veryVeryVerbose);
            }

            // std::pmr::string
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                std::pmr::string shortStr(SHORT_STRING);
                testCase2_CStringUtil<std::pmr::string>("std::pmr::string",
                                                        shortStr,
                                                        SHORT_STRING,
                                                        test,
                                                        verbose,
                                                        veryVerbose,
                                                        veryVeryVerbose);

                std::pmr::string longStr(LONG_STRING);
                testCase2_CStringUtil<std::pmr::string>("std::pmr::string",
                                                        longStr,
                                                        LONG_STRING,
                                                        test,
                                                        verbose,
                                                        veryVerbose,
                                                        veryVeryVerbose);
            }
#endif

            // bslstl::StringRef (SHORT STRING)
            {
                bslstl::StringRef shortStr(SHORT_STRING);
                testCase2_CStringUtil_StringRef(
                    "bslstl::StringRef",
                    shortStr,
                    SHORT_STRING,
                    test,
                    verbose,
                    veryVerbose,
                    veryVeryVerbose);
            }
        }

        {
            // Test source type expected to allocate

            // bslstl::StringRef (LONG STRING)
            {
                bslstl::StringRef longStr(LONG_STRING);
                testCase2_CStringUtil_StringRef(
                    "bslstl::StringRef",
                    longStr,
                    LONG_STRING,
                    test,
                    verbose,
                    veryVerbose,
                    veryVeryVerbose);
            }
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

        {
            const char hello[] = "hello, world\n";
            const char *pHello = hello;
            ASSERT(INT_SIZEOF(hello) > INT_SIZEOF(pHello));
        }

        for (size_t ni = 0; ni < NUM_VALID_NAMES; ++ni) {
            const char *const NAME = NAMES[ni];

            if (veryVerbose) { T_; P_(ni); P(NAME); }

            bsl::wstring wide;

            int rc = bdlde::CharConvertUtf16::utf8ToUtf16(&wide, NAME);

            LOOP2_ASSERT(ni, rc, ni == NAME_ANSI ? rc != 0 : rc == 0);

            bsl::string logPath = NAME;

            Obj::remove(logPath.c_str(), true);
            bdls::PathUtil::appendRaw(&logPath, "logs");

            bsl::string oldPath(logPath), newPath(logPath);
            bdls::PathUtil::appendRaw(&oldPath, "old");
            bdls::PathUtil::appendRaw(&newPath, "new");
            LOOP_ASSERT(oldPath,
                           0 == Obj::createDirectories(oldPath.c_str(), true));
            LOOP_ASSERT(newPath,
                           0 == Obj::createDirectories(newPath.c_str(), true));

            ASSERT(Obj::exists(oldPath));
            ASSERT(Obj::exists(newPath));

            enum {
                k_FILENAME_BUFFER_SIZE = 32
            };

            char filenameBuffer[k_FILENAME_BUFFER_SIZE];

            // TBD: When SetFileInformationByHandle() is available, then we
            // should write a setModificationTime() method and use it here (see
            // bdls_filesystemutil.h).  Until then, we use utime() on POSIX
            // directly and we do not attempt to "touch" Windows files.

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
                                             "filesystemutil%02d_%c.log", i,
                                             isOld ? 'o' : 'n');

                ASSERT(0 == bdls::PathUtil::appendIfValid(&logPath,
                            filenameBuffer));

                if (Obj::exists(logPath.c_str())) {
                    ASSERT(0 == Obj::remove(logPath.c_str(), true));
                }
                Obj::FileDescriptor f = Obj::open(logPath.c_str(),
                                                  Obj::e_OPEN_OR_CREATE,
                                                  Obj::e_READ_WRITE);
                LOOP_ASSERT(logPath, f != Obj::k_INVALID_FD);

                filenameBuffer[filenameLength++] = '\n';
                filenameBuffer[filenameLength++] = 0;

                ASSERT(filenameLength == Obj::write(f,
                                                    filenameBuffer,
                                                    filenameLength));
                ASSERT(0 == Obj::close(f));

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
                bdls::PathUtil::popLeaf(&logPath);
            }

            bdls::PathUtil::appendRaw(&logPath, "*.log");
            vector<bsl::string> logFiles;
            rc = Obj::findMatchingPaths(&logFiles, logPath.c_str());
            ASSERT(static_cast<int>(logFiles.size()) == rc);
            bdls::PathUtil::popLeaf(&logPath);

            bdlt::Datetime modTime;
            string        fileName;
            bdlt::Datetime nowTime = bdlt::CurrentTime::utc();
            for (vector<bsl::string>::iterator it = logFiles.begin();
                    it != logFiles.end(); ++it) {
                ASSERTV(it->c_str(),
                        0 == Obj::getLastModificationTime(&modTime,
                                                          it->c_str()));
                bdls::PathUtil::getLeaf(&fileName, *it);
                bsl::string* whichDirectory =
                     2 < (nowTime - modTime).totalDays() ? &oldPath : &newPath;
                bdls::PathUtil::appendRaw(whichDirectory, fileName.c_str());
                ASSERT(0 == Obj::move(it->c_str(),
                            whichDirectory->c_str()));
                bdls::PathUtil::popLeaf(whichDirectory);
            }

            // Now validate

            bdls::PathUtil::appendRaw(&logPath, "*");
            bdls::PathUtil::appendRaw(&logPath, "*o*.log");
            rc = Obj::findMatchingPaths(&logFiles, logPath.c_str());
            LOOP_ASSERT(ni, NUM_OLD_FILES == rc);
            LOOP5_ASSERT(ni, logPath, logPath.length(), NUM_OLD_FILES,
                                                               logFiles.size(),
                                             NUM_OLD_FILES == logFiles.size());
            bdls::PathUtil::popLeaf(&logPath);

            bdls::PathUtil::appendRaw(&logPath, "*n*.log");
            rc = Obj::findMatchingPaths(&logFiles, logPath.c_str());
            LOOP_ASSERT(ni, NUM_NEW_FILES == rc);
            LOOP5_ASSERT(ni, logPath, logPath.length(), NUM_NEW_FILES,
                                                               logFiles.size(),
                                             NUM_NEW_FILES == logFiles.size());
            bdls::PathUtil::popLeaf(&logPath);
            bdls::PathUtil::popLeaf(&logPath);

            logFiles.clear();
            rc = Obj::findMatchingPaths(&logFiles,
                                       "tmp.non_existent_file.txt");
            LOOP_ASSERT(rc, 0 == rc);    // no such file

            // Clean up

            ASSERT(0 == bdls::PathUtil::popLeaf(&logPath));
            ASSERT(0 == Obj::remove(logPath.c_str(), true));

            // exercise temp name, file, directory

            bsl::string fixedPrefix = "name_prefix_";
            bsl::string varPrefix;
            Obj::makeUnsafeTemporaryFilename(&varPrefix, fixedPrefix);

            bsl::string dirName;
            int madeDir = Obj::createTemporaryDirectory(&dirName, varPrefix);
            ASSERT(madeDir == 0);
            if (madeDir) {
                bsl::string subDirName = dirName + "/file_base";
                bsl::string fileName;
                Obj::FileDescriptor fd =
                               Obj::createTemporaryFile(&fileName, subDirName);
                if (fd != Obj::k_INVALID_FD) {
                    static const char hello[] = "hello, world\n";
                    Obj::write(fd, hello, INT_SIZEOF(hello) - 1);
                    Obj::close(fd);
                    int removedFile = Obj::remove(fileName);
                    ASSERT(removedFile == 0);
                }
                int removedDir = Obj::remove(subDirName);
                ASSERT(removedDir == 0);
            }
        }
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

#ifdef BSLS_PLATFORM_OS_WINDOWS
        cout << "Test Disabled on Windows\n";
#else
        if (verbose) cout << "TESTING detection of large files\n"
                             "================================\n";

        // Not sure how to create large files on windows, so test only on UNIX.
        // However, this function had been tested on windows by creating the
        // large file through cygwin.

        // Create a 5 GB file.

        if (veryVerbose) cout << "\n3. Large File" << endl;

        int rc = system("dd if=/dev/zero of=/tmp/fiveGBFile "
               "bs=1024000 count=5000");
        ASSERT(0 == rc);

        string fileName("/tmp/fiveGBFile");

        Obj::Offset off = Obj::getFileSize(fileName);
        ASSERT(5120000000LL == off);

        Obj::Offset off2 = Obj::getFileSize(fileName.c_str());
        ASSERT(5120000000LL == off2);

        if (veryVerbose) {
            cout << "Expected 5120000000LL" << endl;
            cout << "Actual ";
            P_(off); P(off2)
        }

        Obj::remove(fileName);
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------

        bsl::string foo = tempFileName(20);
        Obj::remove(foo);
        Obj::FileDescriptor fd = Obj::open(foo,
                                           Obj::e_OPEN_OR_CREATE,
                                           Obj::e_READ_WRITE);
        int pageSize = bdls::MemoryUtil::pageSize();
        Obj::growFile(fd, pageSize, true);
        int *p;
        ASSERT(0 == Obj::map(fd, (void**)&p, 0, pageSize,
                                   bdls::MemoryUtil::k_ACCESS_READ_WRITE));
        printf("mapped at %p\n", p);
        for (int i = 0; i < 10000; ++i) {
          ASSERT(0 == Obj::seek(fd, 0, Obj::e_SEEK_FROM_BEGINNING));
          int buf;
          *p = i;
          ASSERT(INT_SIZEOF(int) == Obj::read(fd, &buf, INT_SIZEOF(int)));
          ASSERT(i == buf);
        }
        ASSERT(0 == Obj::unmap(p, pageSize));

#if 0
        Obj::FileDescriptor fd =  // /bb/data is Bloomberg-specific
                       Obj::open("/bb/data/tmp/blahblah.tmp",
                                 Obj::e_OPEN_OR_CREATE,
                                 Obj::e_READ_WRITE);
        int pageSize = Obj::pageSize();
        int nPages = 1024*1024*1024 / pageSize;
        int rc = Obj::growFile(fd, nPages * pageSize, argc > 2);
        printf("growFile(fd, %d, %d) rc=%d\n",
               nPages * pageSize, (int)(argc>2), rc);
        int fileSize = Obj::seek(fd, 0, SEEK_END);
        printf("file size = %d\n", fileSize);
        if (!rc) {
            for(int i=0; i<nPages; i++) {
                ObjMapping fm = Obj::map(fd, i * pageSize, pageSize, true);
                memset(fm.addr(), 2, pageSize);
                Obj::unmap(fm, pageSize);
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

        typedef Obj Util;

        const bsls::Types::Int64 fiveGig = 5LL * 1000LL * 1000LL * 1000LL;
        const bsls::Types::Int64 deltaMileStone = 100LL * 1000LL * 1000LL;

        bsls::Types::Int64 mileStone = deltaMileStone;

        bsls::Types::Int64 bytesWritten = 0;

        char record[80] = "123456789 123456789 123456789 123456789 "
                          "123456789 123456789 123";
        char * const writeTo = record + 63;

        MMIXRand rand;

        LOOP_ASSERT(Util::getFileSizeLimit(),
                                           Util::getFileSizeLimit() > fiveGig);

        const char *fileName = "tmpFiveGig.txt";
        Util::FileDescriptor fd = Util::open(fileName,
                                             Obj::e_OPEN_OR_CREATE,
                                             Obj::e_READ_WRITE);
        ASSERT(Util::k_INVALID_FD != fd);

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
        ASSERT(Util::seek(fd, 0, Util::e_SEEK_FROM_CURRENT) == bytesWritten);
        ASSERT(Util::seek(fd, 0, Util::e_SEEK_FROM_END)     == bytesWritten);

        cout << "Writing done\n";

        if (verbose) P(bytesWritten);

        ASSERT(Util::getFileSize(fileName) == bytesWritten);
        ASSERT(Util::isRegularFile(fileName));

        char inBuf[80];
        bsls::Types::Int64 bytesRead = 0;
        rand.reset();
        mileStone = deltaMileStone;

        ASSERT(0 == Util::seek(fd, 0, Util::e_SEEK_FROM_BEGINNING));

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
        ASSERT(Util::seek(fd, 0, Util::e_SEEK_FROM_CURRENT) == bytesRead);
        ASSERT(Util::seek(fd, 0, Util::e_SEEK_FROM_END)     == bytesRead);

        cout << "Reading done\n";

        // Map the first 3G of that file.

        void *startAddress = 0;
        bsls::Types::Int64 threeGig = 3LL * 1000LL * 1000LL * 1000LL;
        ASSERT(0 == Util::map(fd,
                              &startAddress,
                              0,
                              static_cast<bsl::size_t>(threeGig),
                              bdls::MemoryUtil::k_ACCESS_READ));

        bytesRead = 0;
        mileStone = deltaMileStone;
        rand.reset();
        char *address = static_cast<char *>(startAddress);

        for (;;) {
            ASSERT(0 == bsl::memcmp(record, address, 63));

            rand.munge();
            ASSERT(0 == bsl::memcmp(address + 63, rand.display(), 16));
            ASSERT('\n' == address[79]);

            bytesRead += 80;
            address   += 80;

            if (bytesRead >= mileStone) {
                cout << bytesRead << " checked map -- last: " <<
                                                        rand.display() << endl;
                if (bytesRead >= threeGig) {
                    break;
                }
                mileStone += deltaMileStone;
            }
        }
        ASSERT(threeGig == bytesRead);
        ASSERT(static_cast<char *>(startAddress) + threeGig == address);

        ASSERT(0 == Util::unmap(startAddress,
                                static_cast<bsl::size_t>(threeGig)));

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

    ASSERT(0 == Obj::setWorkingDirectory(origWorkingDirectory));
    LOOP_ASSERT(tmpWorkingDir, Obj::exists(tmpWorkingDir));

    // Sometimes this delete won't work because of '.nfs*' gremlin files that
    // mysteriously get created in the directory.  Seems to especially happen
    // in TC 5 for some reason.  Leave the directory behind and move on.  Also
    // remove twice, because sometimes the first 'remove' 'sorta' fails -- it
    // returns a negative status after successfully killing the gremlin file.
    // Worst case, leave the file there to be cleaned up in a sweep later.

    for (int i = 1; i <= 5; ++i) {
        if (veryVerbose) {
            cout << "Cleaning up " << tmpWorkingDir << " (attempt " << i << ")"
                 << endl;
        }

        Obj::remove(tmpWorkingDir, true);

        if (!Obj::exists(tmpWorkingDir)) {
            if (veryVerbose) {
                cout << "Clean up of " << tmpWorkingDir << " succeeded"
                     << endl;
            }

            break;
        }

        localSleep(1);
    }

    // TODO: Figure out why case 5's tmpWorkingDir is impossible to clean up...
    //
    // The permissions are fine, it's empty, and it removes just fine with
    // rm -rf once the build is done...

    // LOOP_ASSERT(tmpWorkingDir, !Obj::exists(tmpWorkingDir));

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
