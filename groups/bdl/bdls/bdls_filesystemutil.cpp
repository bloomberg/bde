// bdls_filesystemutil.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_filesystemutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_filesystemutil_cpp, "$Id$ $CSID$")

#include <bdls_memoryutil.h>
#include <bdls_pathutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlt_epochutil.h>
#include <bdlt_currenttime.h> // for testing only
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>
#include <bslh_hash.h>
#include <bsls_timeutil.h>

#include <bsl_algorithm.h>
#include <bsl_c_stdio.h> // needed for rename on AIX & snprintf everywhere
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# include <windows.h>
# include <io.h>
# include <direct.h>
# include <bdlde_charconvertutf16.h>
# include <tchar.h>
# ifdef MIN
#   undef MIN
# endif
# ifndef snprintf
#   define snprintf _snprintf
# endif

#else // !BSLS_PLATFORM_OS_WINDOWS
# ifndef _POSIX_PTHREAD_SEMANTICS
#   define _POSIX_PTHREAD_SEMANTICS
# endif
# include <bsl_c_errno.h>
# include <bsl_c_limits.h>
# include <unistd.h>
# include <fcntl.h>
# include <glob.h>
# include <dirent.h>
# include <utime.h> // for testing only ... for now
# include <sys/mman.h>
# include <sys/resource.h>
# include <sys/statvfs.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/uio.h>
#endif

// PRIVATE CONSTANTS
enum {
    k_UNKNOWN_ERROR = 127
};

// STATIC HELPER FUNCTIONS

namespace {

struct NameRec {
    // This 'struct' is for maintaining file names and whether they are
    // matched as patterns or not.  It is used only by 'visitTree'.

    bsl::string d_basename;
    bool        d_foundAsPattern;

    // CREATORS
    NameRec(const bsl::string& basename, bool foundAsPattern)
    : d_basename(basename)
    , d_foundAsPattern(foundAsPattern)
        // Create a 'NameRec' object having the specified 'basename' and the
        // the specified 'foundAsPattern'.
    {
    }

    NameRec(const char *basename, bool foundAsPattern)
    : d_basename()
    , d_foundAsPattern(foundAsPattern)
        // Create a 'NameRec' object having the specified 'basename' and the
        // the specified 'foundAsPattern'.
    {
        BSLS_ASSERT(0 != basename);

        d_basename = basename;
    }

    // ACCESSOR
    bool operator<(const NameRec& rhs) const
        // Return 'true' if the 'fullName' of this object is less than the
        // the 'fullName' of the specified 'rhs'.
    {
        const int rc = bsl::strcmp(d_basename.c_str(), rhs.d_basename.c_str());

        if      (rc < 0) {
            return true;                                              // RETURN
        }
        else if (rc > 0) {
            return false;                                             // RETURN
        }

        // The file names match.  Exactly one of them will have been found as
        // a pattern, and we want that one to be sorted first.

        return d_foundAsPattern && !rhs.d_foundAsPattern;
    }
};

}  // close unnamed namespace

int getProcessId()
    // Return an identifier for the current running process.  Note that this
    // duplicates functionality in 'ProcessUtil', and is reproduced here to
    // avoid a cycle.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

static inline
bool shortIsDotOrDots(const char *path)
    // Return 'true' if the specified 'path' is "." or ".." and 'false'
    // otherwise.  This is equivalent to 'isDotOrDots', except it is called in
    // the case where we know there are no '/'s in the file name, making the
    // check simpler and faster.
{
    return '.' == *path && (!path[1] || ('.' == path[1] && !path[2]));
}

#if defined(BSLS_PLATFORM_OS_UNIX)

#if defined(BSLS_PLATFORM_OS_CYGWIN) || \
    (defined(BSLS_PLATFORM_OS_DARWIN) && defined(_DARWIN_FEATURE_64_BIT_INODE))
namespace {
    typedef struct stat   StatResult;
}  // close unnamed namespace

#else

namespace {
    typedef struct stat64 StatResult;
}  // close unnamed namespace

#endif

static inline
int performStat(const char *fileName, StatResult *statResult)
    // Run the appropriate 'stat' or 'stat64' function on the specified
    // 'fileName', returning the results in the specified 'statResult'.
{

#if defined(BSLS_PLATFORM_OS_CYGWIN) || \
    (defined(BSLS_PLATFORM_OS_DARWIN) && defined(_DARWIN_FEATURE_64_BIT_INODE))
    return stat  (fileName, statResult);
#else
    return stat64(fileName, statResult);
#endif

}

static inline
int performStat(const char *fileName, StatResult *statResult, bool followLinks)
    // Run the appropriate 'stat' or 'stat64' function on the specified
    // 'fileName', returning the results in the specified 'statResult', where
    // the specified 'followLinks' indicates whether symlinks are to be
    // followed.
{
#if defined(BSLS_PLATFORM_OS_CYGWIN) || \
    (defined(BSLS_PLATFORM_OS_DARWIN) && defined(_DARWIN_FEATURE_64_BIT_INODE))
    return followLinks ?  stat(fileName, statResult)
                       : lstat(fileName, statResult);
#else
    return followLinks ?  stat64(fileName, statResult)
                       : lstat64(fileName, statResult);
#endif
}



extern "C" {

// The following function must have a long, unique name.  Even though it's
// declared 'static', on Solaris CC it winds up having global linkage.

static
int bloombergLP_bdls_FileSystemUtil_isNotFilePermissionsError(
                                                          const char *,
                                                          int         errorNum)
    // Return 0 if the specified 'errorNum' is an 'errno'-type value describing
    // that access wasn't granted due to file permissions, which will manifest
    // itself as one of the values 'EPERM', 'EACCES', or 'ENOENT' and 1
    // otherwise.
{
    // One use of this function is to pass it to 'glob', which will use the
    // function to indicate whether or not to stop traversing files based on
    // the type of error encountered.

    // The only error condition we will tolerate is if we weren't permitted to
    // open one of the files, which may manifest as one of the following 3
    // values.

    return EPERM != errorNum && EACCES != errorNum && ENOENT != errorNum;
}

// Copy that hideously long name to function ptr to make referring to it more
// convenient.

typedef int (*IsNotFilePermissionsErrorFuncPtr)(const char *, int);
static const IsNotFilePermissionsErrorFuncPtr isNotFilePermissionsError_p =
                    &bloombergLP_bdls_FileSystemUtil_isNotFilePermissionsError;

}  // close extern "C"

#endif

namespace BloombergLP {

static
void pushBackWrapper(bsl::vector<bsl::string> *vector, const char *item)
    // A 'thunk' to be bound to the specified 'vector' that can be called to
    // push the specified 'item' to the 'vector'.
{
    BSLS_ASSERT(vector);

    vector->push_back(item);
}

#ifdef BSLS_PLATFORM_OS_WINDOWS
static inline
void invokeFindClose(void *handle, void *)
    // Provides a function signature which can be used as a 'bslma::ManagedPtr'
    // deleter (i.e., we need to define a second argument of type 'void *',
    // which will be ignored, and the first parameter must be 'void *' which is
    // cast to correct type inside the function).
{
    BSLS_ASSERT_SAFE(handle);

    FindClose(*static_cast<HANDLE *>(handle));
}

static inline
bool narrowToWide(bsl::wstring *result, const bsl::string& path)
    // Copy the UTF-16 version of the specified 'path' to the specified
    // 'result' wide string and return 'true' if successful, or 'false'
    // otherwise.  If conversion is unsuccessful, the contents of 'result' are
    // undefined.
{
    BSLS_ASSERT_SAFE(result);

    return 0 == bdlde::CharConvertUtf16::utf8ToUtf16(result, path.data());
}

static inline
bool wideToNarrow(bsl::string *result, const bsl::wstring& path)
    // Copy the UTF-8 version of the specified 'path' to the specified 'result'
    // narrow string and return 'true' if successful, or 'false' otherwise.  If
    // conversion is unsuccessful, the contents of 'result' are undefined.
{
    BSLS_ASSERT_SAFE(result);

    return 0 == bdlde::CharConvertUtf16::utf16ToUtf8(result, path.c_str());
}

static inline
int makeDirectory(const char *path, bool)
    // Create a directory.  Return 0 on success and a non-zero value otherwise.
{
    BSLS_ASSERT_SAFE(path);

    bsl::wstring wide;
    BOOL         succeeded = 0;

    if (narrowToWide(&wide, path)) {
        succeeded = CreateDirectoryW(wide.c_str(), 0);
    }

    return succeeded ? 0 : -1;
}

static inline
int removeDirectory(const char *path)
    // Remove directory 'path'.  Return 0 on success and a non-zero value
    // otherwise.
{
    BSLS_ASSERT_SAFE(path);

    bsl::wstring wide;
    BOOL         succeeded = 0;

    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    // Occasionally, recursive directory deletes fail with a "not empty" error
    // code, only to succeed a bit later.  Retry on such failures a few times.
    // (Web search reveals a number of non-definitive theories on the cause.)

    const int RMDIR_NOT_EMPTY_RETRIES = 5;
    const int RMDIR_RETRY_SLEEP_MS    = 500;

    for (int i = 0; i < RMDIR_NOT_EMPTY_RETRIES; ++i) {
        succeeded = RemoveDirectoryW(wide.c_str());

        if (succeeded || ERROR_DIR_NOT_EMPTY != GetLastError()) {
            break;
        }

        Sleep(RMDIR_RETRY_SLEEP_MS);
    }

    return succeeded ? 0 : -1;
}

static inline
int removeFile(const char *path)
    // Remove plain file 'path'.  Return 0 on success and a non-zero value
    // otherwise.
{
    BSLS_ASSERT_SAFE(path);

    bsl::wstring wide;
    BOOL         succeeded = 0;

    if (narrowToWide(&wide, path)) {
        succeeded = DeleteFileW(wide.c_str());
    }

    return succeeded ? 0 : -1;
}

#else
// unix-specific helper functions

static
int localFcntlLock(int descriptor, int cmd, int type)
{
    struct flock flk;
    flk.l_type = static_cast<short>(type);
    flk.l_whence = SEEK_SET;
    flk.l_start = 0;
    flk.l_len = 0;
    return fcntl(descriptor, cmd, &flk);
}

static inline
void invokeGlobFree(void *pglob, void *)
    // Free the specified glob data structure, 'pglob'.  Note that this
    // function has a signature appropriate for use as a managed pointer
    // deleter, hence the parameters are both passed as 'void *'.
{
    BSLS_ASSERT_SAFE(pglob);

    globfree(static_cast<glob_t *>(pglob));
}

static inline
void invokeCloseDir(void *dir, void *)
    // Close the specified directory, 'dir'.  Note that this function has a
    // signature appropriate for use as a managed pointer deleter, hence the
    // parameters are both passed as 'void *'.
{
    BSLS_ASSERT_SAFE(dir);

    // Note also that 'closedir' was observed sometimes returning a non-zero
    // value for no particular reason.  Hopefully we'll capture 'errno' in the
    // nightly build.

    int rc = closedir(static_cast<DIR *>(dir));
    int myErrno = errno;        // 'errno' is a function, get the value into
                                // a veriable where we can see it when the
                                // assert fails and we debug the core file.
    (void) rc;    (void) myErrno;

    BSLS_ASSERT_SAFE(0 == rc);
}

static inline
bool isDotOrDots(const char *path)
    // Return 'true' if the specified 'path' is "." or ".." or ends in
    // "/." or "/..", and 'false' otherwise.
{
    BSLS_ASSERT(path);

    const char *end = path;
    while (*end) {
        ++end;
    }
    const long length = end - path;

    if (0 == length) {
        return false;                                                 // RETURN
    }
    BSLS_ASSERT_SAFE(length >= 1);

    if ('.' != end[-1]) {
        return false;                                                 // RETURN
    }

    if (1 == length) {
        return true;                                                  // RETURN
    }
    BSLS_ASSERT_SAFE(length >= 2);

    if ('/' == end[-2]) {
        return true;                                                  // RETURN
    }
    if ('.' != end[-2]) {
        return false;                                                 // RETURN
    }

    if (2 == length) {
        return true;                                                  // RETURN
    }
    BSLS_ASSERT_SAFE(length >= 3);

    return '/' == end[-3];
}

static inline
int makeDirectory(const char *path, bool isPrivate)
    // Create a directory
{
    BSLS_ASSERT_SAFE(path);

    // Permissions of created dir will be ANDed with process '~umask()'.
    static const int PERMS[2] = {
        (S_IRUSR | S_IWUSR | S_IXUSR |  // user   rwx
         S_IRGRP | S_IWGRP | S_IXGRP |  // group  rwx
         S_IROTH | S_IWOTH | S_IXOTH),  // others rwx

        (S_IRUSR | S_IWUSR | S_IXUSR)  // only user rwx
    };
    return mkdir(path, PERMS[isPrivate]);
}

static inline
int removeDirectory(const char *path)
    // Remove the specified directory 'path'.  Return 0 on success and a
    // non-zero value otherwise.
{
    BSLS_ASSERT_SAFE(path);

    return rmdir(path);
}

static inline
int removeFile(const char *path)
{
    BSLS_ASSERT_SAFE(path);

    return unlink(path);
}

#endif

                        // ----------------------------
                        // struct bdls::FilesystemUtil
                        // ----------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS

const bdls::FilesystemUtil::FileDescriptor bdls::FilesystemUtil::k_INVALID_FD =
                                                          INVALID_HANDLE_VALUE;

namespace bdls {
FilesystemUtil::FileDescriptor FilesystemUtil::open(
                                            const char         *pathName,
                                            FileOpenPolicy      openPolicy,
                                            FileIOPolicy        ioPolicy,
                                            FileTruncatePolicy  truncatePolicy)
{
    BSLS_ASSERT(pathName);

    if (   e_OPEN     == openPolicy
        && e_TRUNCATE == truncatePolicy
        && (   e_READ_ONLY   == ioPolicy
            || e_APPEND_ONLY == ioPolicy
            || e_READ_APPEND == ioPolicy)) {
        return k_INVALID_FD;                                          // RETURN
    }

    bool isTruncateMode = (e_TRUNCATE == truncatePolicy);

    DWORD accessMode  = 0;
    switch (ioPolicy) {
      case e_READ_ONLY: {
        accessMode = GENERIC_READ;
      } break;
      case e_READ_WRITE: {
        accessMode = GENERIC_READ | GENERIC_WRITE;
      } break;
      case e_READ_APPEND: {
        accessMode = GENERIC_READ | FILE_APPEND_DATA;
      } break;
      case e_WRITE_ONLY: {
        accessMode = GENERIC_WRITE;
      } break;
      case e_APPEND_ONLY: {
        accessMode = FILE_APPEND_DATA;
      } break;
      default: {
        BSLS_ASSERT_OPT(!"Unknown IO policy");
      } break;
    }

    DWORD creationInfo = 0;
    switch (openPolicy) {
      case e_OPEN: {
        // Both fail if file does not exist.

        if (isTruncateMode) {
            creationInfo = TRUNCATE_EXISTING;
        }
        else {
            creationInfo = OPEN_EXISTING;
        }
      } break;
      case e_CREATE:
      case e_CREATE_PRIVATE: {  // Fails if file exists.

        creationInfo = CREATE_NEW;
      } break;
      case e_OPEN_OR_CREATE: {
        // Both succeed with error code if file exists.

        if (isTruncateMode) {
            creationInfo = CREATE_ALWAYS;
        }
        else {
            creationInfo = OPEN_ALWAYS;
        }
      } break;
      default: {
        BSLS_ASSERT_OPT(!"Unknown open policy");
      } break;
    }

    // The file locking behavior for the opened file
    // ('FILE_SHARE_READ | FILE_SHARE_WRITE') is chosen to match the posix
    // behavior for open (DRQS 30568749).

    bsl::wstring                         wide;
    FilesystemUtil::FileDescriptor descriptor = k_INVALID_FD;

    if (narrowToWide(&wide, pathName)) {
        descriptor = CreateFileW(
                    wide.c_str(),
                    accessMode,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                                        // do not lock
                    NULL,                               // default security
                    creationInfo,                       // existing file only
                    FILE_ATTRIBUTE_NORMAL,              // normal file
                    NULL);                              // no attr
    }

    return descriptor;
}

int FilesystemUtil::close(FileDescriptor descriptor)
{

    if (CloseHandle(descriptor)) {
        return 0;                                                     // RETURN
    }

    DWORD status = GetLastError();
    return (status == ERROR_INVALID_HANDLE) ? int(k_BAD_FILE_DESCRIPTOR)
                                            : int(k_UNKNOWN_ERROR);
}

int FilesystemUtil::remove(const char *fileToRemove, bool recursive)
{
    BSLS_ASSERT(fileToRemove);

    if (isDirectory(fileToRemove)) {
        if (recursive) {
            bsl::string pattern(fileToRemove);
            PathUtil::appendRaw(&pattern, "*");
            bsl::vector<bsl::string> children;
            findMatchingPaths(&children, pattern.c_str());
            for (bsl::vector<bsl::string>::size_type i = 0;
                 i < children.size();
                 ++i) {
                if (0 != remove(children[i].c_str(), true)) {
                    return -1;                                        // RETURN
                }
            }
        }
        return removeDirectory(fileToRemove);                         // RETURN
    }
    else {
        return removeFile(fileToRemove);                              // RETURN
    }
}

FilesystemUtil::Offset FilesystemUtil::seek(FileDescriptor         descriptor,
                                            FilesystemUtil::Offset offset,
                                            int                    whence)
{
    switch (whence) {
      case e_SEEK_FROM_BEGINNING: {
        whence = FILE_BEGIN;
      } break;
      case e_SEEK_FROM_CURRENT: {
        whence = FILE_CURRENT;
      } break;
      case e_SEEK_FROM_END: {
        whence = FILE_END;
      } break;
      default: {
        BSLS_ASSERT(!"Unknown from whence seeking");
      } break;
    }
    LARGE_INTEGER li;
    li.QuadPart = offset;
    li.LowPart = SetFilePointer(descriptor, li.LowPart, &li.HighPart, whence);
    if (INVALID_SET_FILE_POINTER == li.LowPart && NO_ERROR != GetLastError())
    {
        return -1;                                                    // RETURN
    }
    return li.QuadPart;
}

int FilesystemUtil::read(FileDescriptor  descriptor,
                         void           *buffer,
                         int             numBytesToRead)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numBytesToRead);

    DWORD n;
    return ReadFile(descriptor, buffer, numBytesToRead, &n, 0) ? n : -1;
}

int FilesystemUtil::write(FileDescriptor  descriptor,
                          const void     *buffer,
                          int             numBytesToWrite)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numBytesToWrite);

    DWORD n;
    return WriteFile(descriptor, buffer, numBytesToWrite, &n, 0) ? n : -1;
}

int FilesystemUtil::map(FileDescriptor   descriptor,
                        void           **address,
                        Offset           offset,
                        bsl::size_t      len,
                        int              mode)
{
    BSLS_ASSERT(address);

    HANDLE hMap;

    if (MemoryUtil::k_ACCESS_NONE == mode) {
        return -1;                                                    // RETURN
    }
    mode &= 7;

    static const DWORD protectAccess[8][2] = {
        { PAGE_NOACCESS,          0 },                                  // NONE
        { PAGE_READONLY,          FILE_MAP_READ },                      // R
        { PAGE_READWRITE,         FILE_MAP_WRITE },                     // W
        { PAGE_READWRITE,         FILE_MAP_WRITE },                     // RW
        { PAGE_EXECUTE,           FILE_MAP_EXECUTE },                   // X
        { PAGE_EXECUTE_READ,      FILE_MAP_EXECUTE | FILE_MAP_READ},    // RX
        { PAGE_EXECUTE_READWRITE, FILE_MAP_EXECUTE | FILE_MAP_WRITE },  // WX
        { PAGE_EXECUTE_READWRITE, FILE_MAP_EXECUTE | FILE_MAP_WRITE }   // RWX
    };

    FilesystemUtil::Offset maxLength = offset + len;
    hMap = CreateFileMapping(descriptor,
                             NULL,
                             protectAccess[mode][0],
                             (DWORD)(maxLength>>32),
                             (DWORD)(maxLength&0xFFFFFFFF),
                             NULL);

    if (NULL == hMap) {
        *address = 0;
        return -1;                                                    // RETURN
    }

    *address = MapViewOfFile(hMap,
                             protectAccess[mode][1],
                             (DWORD)(offset >> 32),
                             (DWORD)(offset & 0xFFFFFFFF),
                             len);
    CloseHandle(hMap);
    if (!*address) {
        return -1;                                                    // RETURN
    }
    return 0;
}

int FilesystemUtil::unmap(void *address, bsl::size_t)
{
    BSLS_ASSERT(address);

    return UnmapViewOfFile(address) ? 0 : -1;
}

int FilesystemUtil::sync(char *address, bsl::size_t numBytes, bool)
                                                             // 3rd arg is sync
{
    BSLS_ASSERT(0 != address);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 == numBytes % MemoryUtil::pageSize());
    BSLS_ASSERT(0 == reinterpret_cast<bsls::Types::UintPtr>(address) %
                     MemoryUtil::pageSize());

    // The meaning of the 'sync' flag (cause this function to be synchronous
    // vs. asynchronous) does not appear to be supported by 'FlushViewOfFile'.

    return FlushViewOfFile(address, numBytes) ? 0 : -1;
}

int FilesystemUtil::lock(FileDescriptor descriptor, bool lockWrite)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    return !LockFileEx(descriptor,
                       lockWrite ? LOCKFILE_EXCLUSIVE_LOCK : 0,
                       0,
                       1,
                       0,
                       &overlapped);
}

int FilesystemUtil::tryLock(FileDescriptor descriptor, bool lockWrite)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    bool success = LockFileEx(
         descriptor,
         LOCKFILE_FAIL_IMMEDIATELY | (lockWrite ? LOCKFILE_EXCLUSIVE_LOCK : 0),
         0,
         1,
         0,
         &overlapped);
    return success ? 0
                   : ERROR_LOCK_VIOLATION == GetLastError()
                     ? k_ERROR_LOCKING_CONFLICT
                     : -1;
}

int FilesystemUtil::unlock(FileDescriptor descriptor)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    return !UnlockFileEx(descriptor, 0, 1, 0,  &overlapped);
}

int FilesystemUtil::move(const char *oldName, const char *newName)
    // Move the file at the specified 'oldName' path to the specified 'newName'
    // path.  Return 0 on success and non-zero otherwise.
{
    BSLS_ASSERT(oldName);
    BSLS_ASSERT(newName);

    if (exists(newName)) {
        removeFile(newName);
    }

    bsl::wstring oldWide;
    bsl::wstring newWide;
    BOOL         succeeded = 0;

    if (narrowToWide(&oldWide, oldName) && narrowToWide(&newWide, newName)) {
        succeeded = MoveFileW(oldWide.c_str(), newWide.c_str());
    }

    return succeeded ? 0 : -1;
}

bool FilesystemUtil::exists(const char *pathName)
    // Return 'true' if a file exists at the specified 'pathName', and 'false'
    // otherwise.
{
    BSLS_ASSERT(pathName);

    bsl::wstring wide;
    DWORD        attributes = INVALID_FILE_ATTRIBUTES;

    if (narrowToWide(&wide, pathName)) {
        attributes = GetFileAttributesW(wide.c_str());
    }

    return INVALID_FILE_ATTRIBUTES != attributes;
}

int FilesystemUtil::getLastModificationTime(bdlt::Datetime *time,
                                            const char     *path)
    // Set the value of specified 'time' to the last modification time of the
    // file at the specified 'path'.  Return 0 on success and non-zero
    // otherwise.
{
    BSLS_ASSERT(time);
    BSLS_ASSERT(path);

    FILETIME modified;
    HANDLE   handle;

    bsl::wstring wide;

    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    WIN32_FIND_DATAW findDataW;

    handle = FindFirstFileW(wide.c_str(), &findDataW);
    modified = findDataW.ftLastWriteTime;

    if (INVALID_HANDLE_VALUE == handle) {
        return -1;                                                    // RETURN
    }

    bslma::ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);

    SYSTEMTIME stUTC;

    if (0 == FileTimeToSystemTime(&modified, &stUTC)) {
        return -2;                                                    // RETURN
    }

    if (0 != time->setDatetimeIfValid(stUTC.wYear,
                                      stUTC.wMonth,
                                      stUTC.wDay,
                                      stUTC.wHour,
                                      stUTC.wMinute,
                                      stUTC.wSecond,
                                      stUTC.wMilliseconds)) {
        return -3;                                                    // RETURN
    }

    return 0;
}

int FilesystemUtil::visitPaths(
                           const char                              *patternStr,
                           const bsl::function<void(const char*)>&  visitor)
{
    // Windows 'FindFirstFileW' function only looks at pattern characters ('*'
    // and '?') in the leaf element of the path.  Therefore we must first
    // resolve all special characters in the non-leaf portion of the path
    // recursively before resolving the leaf portion.  Also, unlike glob(), the
    // name returned by 'FindFirstFileW' is *just* the leaf.  Resolving the
    // handle into a full path is not available pre-Vista.

    BSLS_ASSERT(patternStr);

    int numFiles = 0;   // Count # of files visited to be returned if
                        // successful.

    bsl::string dirName;
    if (0 != PathUtil::getDirname(&dirName, patternStr)) {
        // There is no leaf, therefore there can be nothing to do (but not an
        // error).  Return # of files found, which is 0.

        return 0;                                                     // RETURN
    }

    if (bsl::string::npos != dirName.find_first_of("*?")) {
        bsl::vector<bsl::string> leaves;
        bsl::vector<bsl::string> paths, workingPaths;
        bsl::string pattern = patternStr;
        while (PathUtil::hasLeaf(pattern)) {
            leaves.push_back(bsl::string());
            if (0 != PathUtil::getLeaf(&leaves.back(), pattern)) {
                BSLS_ASSERT_OPT(0 && "'getLeaf' failed after"
                                                 " 'hasLeaf' returned 'true'");
            }
            if (0 != PathUtil::popLeaf(&pattern)) {
                BSLS_ASSERT_OPT(0 && "'popLeaf' failed after"
                                                 " 'hasLeaf' returned 'true'");
            }
        }

        paths.push_back(pattern);
        while (!leaves.empty()) {
            for (bsl::vector<bsl::string>::iterator it = paths.begin();
                                                     it != paths.end(); ++it) {
                PathUtil::appendRaw(&(*it), leaves.back().c_str(),
                                             (int) leaves.back().length(), -1);
            }
            if (bsl::string::npos != leaves.back().find_first_of("*?")) {
                // We just put a leaf pattern onto each path.  Need to expand
                // them recursively.

                workingPaths = paths;
                paths.clear();
                bsl::vector<bsl::string>::iterator it;
                for (it = workingPaths.begin(); it != workingPaths.end();
                                                                        ++it) {
                    int rc = visitPaths(it->c_str(),
                                        bdlf::BindUtil::bind(
                                                      &pushBackWrapper,
                                                      &paths,
                                                      bdlf::PlaceHolders::_1));
                    if (rc < 0) {
                        return rc;                                    // RETURN
                    }
                }
            }
            leaves.pop_back();
        }

        numFiles += static_cast<int>(paths.size());

        typedef bsl::vector<bsl::string>::const_iterator CIter;
        CIter end = paths.end();
        for (CIter it = paths.begin(); it != paths.end(); ++it) {
            visitor(it->c_str());
        }
    }
    else {
        // No wild cards except possibly in the leaf.  This is the BASE CASE.

        bsl::string      dirNamePath = dirName;
        WIN32_FIND_DATAW findDataW;
        HANDLE           handle;
        bsl::wstring     widePattern;
        bsl::string      narrowName;

        if (!narrowToWide(&widePattern, patternStr)) {
            // If the user passed invalid UTF-8 in 'patternStr', return
            // failure.

            return -1;                                                // RETURN
        }

        handle = FindFirstFileExW(widePattern.c_str(),
                                  FindExInfoStandard,
                                  &findDataW,
                                  FindExSearchNameMatch,
                                  NULL,
                                  FIND_FIRST_EX_CASE_SENSITIVE);

        if (INVALID_HANDLE_VALUE == handle) {
            // No files found, but not an error.

            return 0;                                                 // RETURN
        }

        bslma::ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);

        bsl::string fullNamePath = dirNamePath;
        for (bool sts = true; sts; sts = FindNextFileW(handle, &findDataW)) {
            if (!wideToNarrow(&narrowName, findDataW.cFileName)) {
                // Can't happen: wideToNarrow won't fail.

                BSLS_ASSERT_OPT(0 && "Wide-to-narrow conversion failed.");
            }

            const char *pc = narrowName.c_str();
            if (shortIsDotOrDots(pc)) {
                // 'narrowName is "." or "..".

                continue;
            }

            fullNamePath.resize(dirNamePath.length());
            if (0 != PathUtil::appendIfValid(&fullNamePath, narrowName)) {
                // Can't happen: 'findDataW.cFileName' will never be an
                // absolute path.

                BSLS_ASSERT_OPT(0 &&
                                  "FindFirstFileW returned an absolute path.");
            }

            ++numFiles;
            visitor(fullNamePath.c_str());
        }
    }

    return numFiles;
}

int FilesystemUtil::visitTree(
                        const bsl::string&                            root,
                        const bsl::string&                            pattern,
                        const bsl::function<void (const char *path)>& visitor,
                        bool                                          sortFlag)
{
    bsl::string rootDir;
    rootDir.reserve(root.length() + 1);
    rootDir = root;
    if (!isDirectory(rootDir)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT_SAFE(!rootDir.empty());    // 'isDirectory' would have been
                                           // 'false' otherwise.
    if (bsl::string::npos != pattern.find('\\')) {
        return -1;                                                    // RETURN
    }

    if ('\\' != rootDir.back()) {
        rootDir += '\\';
    }
    bsl::size_t truncTo = rootDir.length();

    bsl::vector<NameRec> nameRecs;

    bsl::string fullPattern;
    fullPattern.reserve(rootDir.length() + pattern.length());
    fullPattern = rootDir;
    fullPattern += pattern;

    bsl::wstring widePattern;
    WIN32_FIND_DATAW foundData;
    const wchar_t wdot = L'.';
    bsl::string narrowLeafName;

    // We can't use 'wideToNarrow' or 'narrowToWide' because they insert
    // '?' chars for errors in input, which, being a wild card, will
    // confuse us when we recurse.  Instead we use '-' as an error char,
    // which will be less problematic.

    (void) bdlde::CharConvertUtf16::utf8ToUtf16(&widePattern,
                                                fullPattern.c_str(),
                                                0,
                                                '-');
    FileDescriptor handle = FindFirstFileExW(widePattern.c_str(),
                                             FindExInfoStandard,
                                             &foundData,
                                             FindExSearchNameMatch,
                                             NULL,
                                             FIND_FIRST_EX_CASE_SENSITIVE);
    if (INVALID_HANDLE_VALUE != handle) {
        bslma::ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);
        for (bool sts = true; sts; sts = FindNextFileW(handle, &foundData)) {
            const wchar_t *wfn = foundData.cFileName;

            if (wdot == *wfn && (!wfn[1] || (wdot == wfn[1] && !wfn[2]))) {
                continue;
            }

            narrowLeafName.clear();
            (void) bdlde::CharConvertUtf16::utf16ToUtf8(&narrowLeafName,
                                                        wfn,
                                                        0,
                                                        '-');

            nameRecs.push_back(NameRec(narrowLeafName, true));
        }
    }

    fullPattern.resize(truncTo);
    fullPattern += '*';

    widePattern.clear();
    (void) bdlde::CharConvertUtf16::utf8ToUtf16(&widePattern,
                                                fullPattern.c_str(),
                                                0,
                                                '-');
    handle = FindFirstFileExW(widePattern.c_str(),
                              FindExInfoStandard,
                              &foundData,
                              FindExSearchLimitToDirectories,
                              NULL,
                              FIND_FIRST_EX_CASE_SENSITIVE);
    if (INVALID_HANDLE_VALUE != handle) {
        bslma::ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);
        for (bool sts = true; sts; sts = FindNextFileW(handle, &foundData)) {
            if (! (foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                continue;
            }

            const wchar_t *wfn = foundData.cFileName;

            // Skip "." or "..".  See above.

            if (wdot == *wfn && (!wfn[1] || (wdot == wfn[1] && !wfn[2]))) {
                continue;
            }

            narrowLeafName.clear();
            (void) bdlde::CharConvertUtf16::utf16ToUtf8(&narrowLeafName,
                                                        wfn,
                                                        0,
                                                        '-');

            nameRecs.push_back(NameRec(narrowLeafName, false));
        }
    }

    // Sort base names, with names found in pattern match preceding directory
    // names not found as pattern.

    if (sortFlag) {
        bsl::sort(nameRecs.begin(), nameRecs.end());
    }

    bsl::string       fullName(rootDir);

    typedef bsl::vector<NameRec>::const_iterator CIt;

    const CIt end = nameRecs.end();
    for  (CIt it  = nameRecs.begin(); end != it; ++it) {
        fullName.resize(truncTo);
        fullName += it->d_basename;

        if (it->d_foundAsPattern) {
            visitor(fullName.c_str());
        }
        else {
            int rc = visitTree(fullName, pattern, visitor, sortFlag);
            (void) rc;    // silence 'unused' warning

            // Note that if '0 != rc', it means there's a logic error in this
            // function.

            BSLS_ASSERT_SAFE(0 == rc);
        }
    }

    return 0;
}

bool FilesystemUtil::isRegularFile(const char *path, bool)
{
    BSLS_ASSERT(path);

    bsl::wstring wide;
    DWORD        stats;

    if (!narrowToWide(&wide, path)) {
        return false;                                                 // RETURN
    }

    stats = GetFileAttributesW(wide.c_str());

    return (INVALID_FILE_ATTRIBUTES != stats &&
            0 == (stats & FILE_ATTRIBUTE_DIRECTORY));
}

bool FilesystemUtil::isDirectory(const char *path, bool)
{
    BSLS_ASSERT(path);

    bsl::wstring wide;
    DWORD        stats;

    if (!narrowToWide(&wide, path)) {
        return false;                                                 // RETURN
    }

    stats = GetFileAttributesW(wide.c_str());

    return (INVALID_FILE_ATTRIBUTES != stats &&
            0 != (stats & FILE_ATTRIBUTE_DIRECTORY));
}

FilesystemUtil::Offset
FilesystemUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

    ULARGE_INTEGER avail;
    bsl::wstring   wide;

    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    if (!GetDiskFreeSpaceExW(wide.c_str(), &avail, NULL, NULL)) {
        return -1;                                                    // RETURN
    }

    return static_cast<FilesystemUtil::Offset>(avail.QuadPart);
}

FilesystemUtil::Offset
FilesystemUtil::getAvailableSpace(FileDescriptor descriptor)
{
    typedef struct {
        union {
            LONG Status;
            PVOID Pointer;
        };
        ULONG_PTR Information;
    } IO_STATUS_BLOCK;

    typedef struct {
        LARGE_INTEGER TotalAllocationUnits;
        LARGE_INTEGER AvailableAllocationUnits;
        ULONG SectorsPerAllocationUnit;
        ULONG BytesPerSector;
    } FILE_FS_SIZE_INFORMATION;

    typedef LONG __stdcall NTQUERYVOLUMEINFORMATIONFILE(
                                        HANDLE           FileHandle,
                                        IO_STATUS_BLOCK *IoStatusBlock,
                                        PVOID            FileInformation,
                                        ULONG            Length,
                                        INT              FileInformationClass);

    static HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
    static NTQUERYVOLUMEINFORMATIONFILE *pNQVIF =
        hNtDll ? (NTQUERYVOLUMEINFORMATIONFILE*)
                         GetProcAddress(hNtDll, "NtQueryVolumeInformationFile")
               : 0;
    if (!pNQVIF) {
        return -1;                                                    // RETURN
    }
    IO_STATUS_BLOCK ioStatusBlock;
    enum {
        fileFsSizeInformation = 3
    };
    FILE_FS_SIZE_INFORMATION sizeInfo;
    LONG rc = pNQVIF(descriptor,
                     &ioStatusBlock,
                     &sizeInfo,
                     sizeof(sizeInfo),
                     fileFsSizeInformation);
    if (rc) {
        return -1;                                                    // RETURN
    }
    return sizeInfo.AvailableAllocationUnits.QuadPart
        * sizeInfo.SectorsPerAllocationUnit
        * sizeInfo.BytesPerSector;
}

FilesystemUtil::Offset FilesystemUtil::getFileSize(const char *path)
{
    BSLS_ASSERT(path);

    bsl::wstring              wide;
    WIN32_FILE_ATTRIBUTE_DATA fileAttribute;

    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    if (!GetFileAttributesExW(wide.c_str(), GetFileExInfoStandard,
                                                     (void *)&fileAttribute)) {
        return -1;                                                    // RETURN
    }

    const FilesystemUtil::Offset highBits =
        static_cast<FilesystemUtil::Offset>(fileAttribute.nFileSizeHigh);
    return (highBits << 32) | fileAttribute.nFileSizeLow;
}

FilesystemUtil::Offset FilesystemUtil::getFileSizeLimit()
{
    // TBD

    return k_OFFSET_MAX;
}

int FilesystemUtil::getWorkingDirectory(bsl::string *path)
{
    BSLS_ASSERT(path);

    enum { BUFFER_SIZE = 4096 };
    wchar_t buffer[BUFFER_SIZE];

    wchar_t *retval = _wgetcwd(buffer, BUFFER_SIZE);
    if (buffer == retval && wideToNarrow(path, bsl::wstring(buffer))) {
        //our contract requires an absolute path

        return PathUtil::isRelative(*path);                           // RETURN
    }
    return -1;
}

int FilesystemUtil::setWorkingDirectory(const char *path)
{
    BSLS_ASSERT(path);

    bsl::wstring wide;
    int          status;

    if (narrowToWide(&wide, path)) {
        status = _wchdir(wide.c_str());
    }
    else {
        status = -1;
    }

    return status;
}
}  // close package namespace

#else
// unix specific implementation

const bdls::FilesystemUtil::FileDescriptor
                                       bdls::FilesystemUtil::k_INVALID_FD = -1;

namespace bdls {
FilesystemUtil::FileDescriptor FilesystemUtil::open(
                                            const char         *path,
                                            FileOpenPolicy      openPolicy,
                                            FileIOPolicy        ioPolicy,
                                            FileTruncatePolicy  truncatePolicy)
{
    BSLS_ASSERT(path);

    if (   e_OPEN     == openPolicy
        && e_TRUNCATE == truncatePolicy
        && (   e_READ_ONLY   == ioPolicy
            || e_APPEND_ONLY == ioPolicy
            || e_READ_APPEND == ioPolicy)) {
        return k_INVALID_FD;                                          // RETURN
    }

    int oflag = 0;
    int extendedFlags = 0;
    bool useExtendedOpen = false;
    bool isTruncateMode = (e_TRUNCATE == truncatePolicy);

    switch (ioPolicy) {
      case e_READ_ONLY: {
        oflag = O_RDONLY;
      } break;
      case e_READ_WRITE: {
        oflag = O_RDWR;
      } break;
      case e_READ_APPEND: {
        oflag = O_RDWR | O_APPEND;
      } break;
      case e_WRITE_ONLY: {
        oflag = O_WRONLY;
      } break;
      case e_APPEND_ONLY: {
        oflag = O_WRONLY | O_APPEND;
      } break;
      default: {
        BSLS_ASSERT_OPT(!"Unknown IO policy");
      } break;
    }

    switch (openPolicy) {
      case e_OPEN: {
        if (isTruncateMode) {
            oflag |= O_TRUNC;
        }
      } break;
      case e_CREATE: {
        oflag |= O_CREAT | O_EXCL;
        useExtendedOpen = true;
        extendedFlags =
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      } break;
      case e_CREATE_PRIVATE: {
        oflag |= O_CREAT | O_EXCL;
        useExtendedOpen = true;
        extendedFlags = S_IRUSR | S_IWUSR;
      } break;
      case e_OPEN_OR_CREATE: {
        oflag |= O_CREAT;
        if (isTruncateMode) {
            oflag |= O_TRUNC;
        }
        useExtendedOpen = true;
        extendedFlags =
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      } break;
      default: {
        BSLS_ASSERT_OPT(!"Unknown open policy");
      } break;
    }

    if (useExtendedOpen) {
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
        return ::open(path, oflag, extendedFlags);                    // RETURN
#else
        return open64(path, oflag, extendedFlags);                    // RETURN
#endif
    }
    else {
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
        return ::open(path, oflag);                                   // RETURN
#else
        return open64(path, oflag);                                   // RETURN
#endif
    }
}

int FilesystemUtil::close(FileDescriptor descriptor)
{
    int rc = ::close(descriptor);

    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    return (EBADF == errno) ? int(k_BAD_FILE_DESCRIPTOR)
                            : int(k_UNKNOWN_ERROR);
}

FilesystemUtil::Offset FilesystemUtil::seek(FileDescriptor descriptor,
                                            Offset         offset,
                                            int            whence)
{
    switch (whence) {
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
      case e_SEEK_FROM_BEGINNING: {
        return lseek(descriptor, offset, SEEK_SET);                   // RETURN
      }
      case e_SEEK_FROM_CURRENT: {
        return lseek(descriptor, offset, SEEK_CUR);                   // RETURN
      }
      case e_SEEK_FROM_END: {
        return lseek(descriptor, offset, SEEK_END);                   // RETURN
      }
#else
      case e_SEEK_FROM_BEGINNING: {
        return lseek64(descriptor, offset, SEEK_SET);                 // RETURN
      }
      case e_SEEK_FROM_CURRENT: {
        return lseek64(descriptor, offset, SEEK_CUR);                 // RETURN
      }
      case e_SEEK_FROM_END: {
        return lseek64(descriptor, offset, SEEK_END);                 // RETURN
      }
#endif
      default: {
        return -1;                                                    // RETURN
      }
    }
}

int FilesystemUtil::remove(const char *path, bool recursiveFlag)
{
    BSLS_ASSERT(path);

    if (isDirectory(path)) {
        if (recursiveFlag) {
            // What we'd LIKE to do here is findMatchingPaths("path/*") and
            // delete each one.  But glob(), on which findMatchingPaths() is
            // built, will not include the name of a symbolic link if there is
            // no file attached.  Thus a bad link would prevent a directory
            // from being removed.  So instead we must open and read the
            // directory ourselves and remove everything that's not "." or
            // "..", checking for directories (*without* following links) and
            // recursing as necessary.

            DIR *dir = opendir(path);
            if (0 == dir) {
                return -1;                                            // RETURN
            }
            bslma::ManagedPtr<DIR> dirGuard(dir, 0, &invokeCloseDir);

            bsl::string workingPath = path;

            // The amount of space available in the 'd_name' member of the
            // dirent struct is apparently "implementation-defined" and in
            // particular is allowed to be less than the maximum path length
            // (!).  The very C-style way to fix this is to make sure that
            // there's lots of extra space available at the end of the struct
            // (d_name is always the last member) so that strcpy can happily
            // copy into it without instigating a buffer overrun attack against
            // us =)

            enum { OVERFLOW_SIZE = 2048 };    // probably excessive, but it's
                                              // just stack
            union {
                struct dirent d_entry;
                char d_overflow[OVERFLOW_SIZE];
            } entryHolder;

            struct dirent& entry = entryHolder.d_entry;
            struct dirent *entry_p;
            StatResult dummy;
            int rc;
            do {
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
                rc = readdir_r(dir, &entry, &entry_p);
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
                if (0 != rc) {
                    break;
                }

                if (shortIsDotOrDots(entry.d_name)) {
                    continue;
                }

                PathUtil::appendRaw(&workingPath, entry.d_name);
                if (0 == ::performStat(workingPath.c_str(), &dummy, false)
                   && 0 != remove(workingPath.c_str(), true)) {
                    return -1;                                        // RETURN
                }
                PathUtil::popLeaf(&workingPath);
            } while (&entry == entry_p);
        }

        return removeDirectory(path);                                 // RETURN
    }
    else {
        return removeFile(path);                                      // RETURN
    }
}

int FilesystemUtil::read(FileDescriptor descriptor, void *buffer, int numBytes)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numBytes);

    return static_cast<int>(::read(descriptor, buffer, numBytes));
}

int FilesystemUtil::write(FileDescriptor  descriptor,
                          const void     *buffer,
                          int             numBytes)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numBytes);

    return static_cast<int>(::write(descriptor, buffer, numBytes));
}

int FilesystemUtil::map(FileDescriptor   descriptor,
                        void           **address,
                        Offset           offset,
                        bsl::size_t      size,
                        int              mode)
{
    BSLS_ASSERT(address);

    int protect = 0;
    if (mode & MemoryUtil::k_ACCESS_READ) {
        protect |= PROT_READ;
    }
    if (mode & MemoryUtil::k_ACCESS_WRITE) {
        protect |= PROT_WRITE;
    }
    if (mode & MemoryUtil::k_ACCESS_EXECUTE) {
        protect |= PROT_EXEC;
    }

#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    *address = mmap(0, size, protect, MAP_SHARED, descriptor, offset);
#else
    *address = mmap64(0, size, protect, MAP_SHARED, descriptor, offset);
#endif

    if (MAP_FAILED == *address) {
        *address = NULL;
        return -1;                                                    // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

int  FilesystemUtil::unmap(void *address, bsl::size_t size)
{
    BSLS_ASSERT(address);

    int rc = munmap(static_cast<char *>(address), size);
    return rc;
}

int FilesystemUtil::sync(char *address, bsl::size_t numBytes, bool syncFlag)
{
    BSLS_ASSERT(0 != address);
    BSLS_ASSERT(0 == numBytes % MemoryUtil::pageSize());
    BSLS_ASSERT(0 == reinterpret_cast<bsls::Types::UintPtr>(address) %
                     MemoryUtil::pageSize());

    int rc = ::msync(address, numBytes, syncFlag ? MS_SYNC : MS_ASYNC);

    // We do not contractually offer meaningful return codes (providing status
    // in a cross-platform way is problematic); however, the returned value may
    // be logged, so providing a more informative value may aid in debugging.

    return 0 == rc ? 0 : errno;
}

int FilesystemUtil::tryLock(FileDescriptor descriptor, bool lockWriteFlag)
{
    int rc = localFcntlLock(descriptor,
                            F_SETLK,
                            lockWriteFlag ? F_WRLCK : F_RDLCK);
    return -1 != rc                           ? 0
         : EAGAIN == errno || EACCES == errno ? k_ERROR_LOCKING_CONFLICT
         :                                      -1;
}

int FilesystemUtil::lock(FileDescriptor descriptor, bool lockWriteFlag)
{
    int rc = localFcntlLock(descriptor,
                            F_SETLKW,
                            lockWriteFlag ? F_WRLCK : F_RDLCK);
    return -1 != rc       ? 0
         : EINTR == errno ? k_ERROR_LOCKING_INTERRUPTED
         :                  -1;
}

int FilesystemUtil::unlock(FileDescriptor descriptor)
{
    return -1 == localFcntlLock(descriptor, F_SETLK, F_UNLCK) ? -1 : 0;
}

int FilesystemUtil::move(const char *oldPath, const char *newPath)
{
    BSLS_ASSERT(oldPath);
    BSLS_ASSERT(newPath);

    return ::rename(oldPath, newPath);
}

bool FilesystemUtil::exists(const char *path)
{
    BSLS_ASSERT(path);

    return 0 == ::access(path, F_OK);
}

bool FilesystemUtil::isRegularFile(const char *path, bool followLinksFlag)
{
    BSLS_ASSERT(path);

    StatResult fileStats;

    if (0 != ::performStat(path, &fileStats, followLinksFlag)) {
        return false;                                                 // RETURN
    }

    return S_ISREG(fileStats.st_mode);
}

bool FilesystemUtil::isDirectory(const char *path, bool followLinksFlag)
{
    BSLS_ASSERT(path);

    StatResult fileStats;

    if (0 != ::performStat(path, &fileStats, followLinksFlag)) {
        return false;                                                 // RETURN
    }

    return S_ISDIR(fileStats.st_mode);
}

int FilesystemUtil::getLastModificationTime(bdlt::Datetime *time,
                                            const char     *path)
{
    BSLS_ASSERT(time);
    BSLS_ASSERT(path);

    StatResult fileStats;

    if (0 != ::performStat(path, &fileStats)) {
        return -1;                                                    // RETURN
    }

    *time = bdlt::EpochUtil::epoch();
    time->addSeconds(fileStats.st_mtime);
    return 0;
}

int FilesystemUtil::visitPaths(
                             const char                               *pattern,
                             const bsl::function<void(const char *)>&  visitor)
{
    BSLS_ASSERT(pattern);

    glob_t pglob;

    int rc = glob(pattern, GLOB_NOSORT, isNotFilePermissionsError_p, &pglob);
    bslma::ManagedPtr<glob_t> globGuard(&pglob, 0, &invokeGlobFree);
    switch (rc) {
      case 0: {
        // matched something

        BSLS_ASSERT(0 < pglob.gl_pathc);

        unsigned numFiles = 0;
        for (unsigned ii = 0; ii < pglob.gl_pathc; ++ii) {
            BSLS_ASSERT(pglob.gl_pathv[ii]);
            if (!isDotOrDots(pglob.gl_pathv[ii])) {
                ++numFiles;
                visitor(pglob.gl_pathv[ii]);
            }
        }

        // Note that if we only matched '.' and/or '..', 'numFiles' is 0.  Not
        // an error.

        return numFiles;                                              // RETURN
      } break;
      case GLOB_NOMATCH: {
        // No files matched, not an error, return the # of files matched.

        return 0;                                                     // RETURN
      } break;
      case GLOB_NOSPACE: {
        // out of memory

        // If exceptions are disabled, 'throwBadAlloc' should abort.

        bsls::BslExceptionUtil::throwBadAlloc();

        return -11;                                                   // RETURN
      } break;
      case GLOB_ABORTED: {
        return -12;                                                   // RETURN
      } break;
      case GLOB_NOSYS: {
        return -13;                                                   // RETURN
      } break;
      default: {
        // unknown error

        return -14;                                                   // RETURN
      } break;
    }
}

int FilesystemUtil::visitTree(
                        const bsl::string&                            root,
                        const bsl::string&                            pattern,
                        const bsl::function<void (const char *path)>& visitor,
                        bool                                          sortFlag)
{
    bsl::string rootDir;
    rootDir.reserve(root.length() + 1);
    rootDir = root;
    if (!isDirectory(rootDir)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT_SAFE(!rootDir.empty());    // 'isDirectory' would have been
                                           // 'false' otherwise.
    if (bsl::string::npos != pattern.find('/')) {
        return -2;                                                    // RETURN
    }

    if ('/' != rootDir.back()) {
        rootDir += '/';
    }
    bsl::string       fullFn(rootDir);
    const bsl::size_t truncTo = rootDir.length();

    bsl::vector<NameRec> nameRecs;

    {
        glob_t pglob;

        bsl::string fullPattern;
        fullPattern.reserve(rootDir.length() + pattern.length());
        fullPattern =  rootDir;
        fullPattern += pattern;

        int rc = ::glob(fullPattern.c_str(),
                        GLOB_NOSORT,
                        isNotFilePermissionsError_p,
                        &pglob);
        bslma::ManagedPtr<glob_t> globGuard(&pglob, 0, &invokeGlobFree);
        bsl::size_t numFound;
        switch (rc) {
          case 0: {
            numFound = pglob.gl_pathc;
          } break;
          case GLOB_NOMATCH: {
            numFound = 0;
          } break;
          case GLOB_NOSPACE: {
            // out of memory

            // If exceptions are disabled, 'throwBadAlloc' should abort.

            bsls::BslExceptionUtil::throwBadAlloc();

            // Should only get here if exceptions are disabled.

            return -3;                                                // RETURN
          } break;
          case GLOB_NOSYS: {
            return -4;                                                // RETURN
          } break;
          case GLOB_ABORTED: {
            return -5;                                                // RETURN
          } break;
          default: {
            return -6;                                                // RETURN
          }
        }
        nameRecs.reserve(2 * numFound);    // worst case is they're all
                                           // directories

        for (unsigned ii = 0; ii < numFound; ++ii) {
            const char *fullPath = pglob.gl_pathv[ii];
            BSLS_ASSERT_SAFE(fullPath);
            const char *basename = fullPath + truncTo;
            BSLS_ASSERT_SAFE('/' == basename[-1]);

            // Ignore '.', '..', symlinks and any other weird files that aren't
            // directories or plain files.

            if (!shortIsDotOrDots(basename)) {
                StatResult fileStats;

                if (0 == ::performStat(fullPath, &fileStats, false) &&
                                                (S_ISREG(fileStats.st_mode) ||
                                                 S_ISDIR(fileStats.st_mode))) {
                    nameRecs.push_back(NameRec(basename, true));
                }
            }
        }
    }

    {
        DIR *dir = opendir(rootDir.c_str());
        if (0 == dir) {
            return (*isNotFilePermissionsError_p)(0, errno) ? -7 : 0; // RETURN
        }
        bslma::ManagedPtr<DIR> dirGuard(dir, 0, &invokeCloseDir);

        // The amount of space available in the 'd_name' member of the dirent
        // struct is apparently "implementation-defined" and in particular is
        // allowed to be less than the maximum path length (!).  The very
        // C-style way to fix this is to make sure that there's lots of extra
        // space available at the end of the struct (d_name is always the last
        // member) so that strcpy can happily copy into it without instigating
        // a buffer overrun attack against us =)

        enum {OVERFLOW_SIZE = 2048}; //probably excessive, but it's just stack
        union {
            struct dirent d_entry;
            char d_overflow[OVERFLOW_SIZE];
        } entryHolder;

        struct dirent& entry = entryHolder.d_entry;
        struct dirent *entry_p;
        int rc;
        while (true) {
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
            rc = readdir_r(dir, &entry, &entry_p);
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
            if (0 != rc || &entry != entry_p) {
                break;
            }

            const char *basename = entry.d_name;
            if (!*basename) {
                continue;
            }

            if (shortIsDotOrDots(basename)) {
                continue;
            }

            fullFn.resize(truncTo);
            fullFn += basename;

            if (isDirectory(fullFn)) {
                nameRecs.push_back(NameRec(basename, false));
            }
        }
    }

    // Sort base names, with names found in pattern match preceding directory
    // names not found as pattern.

    if (sortFlag) {
        bsl::sort(nameRecs.begin(), nameRecs.end());
    }

    typedef bsl::vector<NameRec>::const_iterator CIt;

    const CIt end = nameRecs.end();
    for  (CIt it  = nameRecs.begin(); end != it; ++it) {
        fullFn.resize(truncTo);
        fullFn += it->d_basename;

        if (it->d_foundAsPattern) {
            visitor(fullFn.c_str());
        }
        else {
            int rc = visitTree(fullFn, pattern, visitor, sortFlag);
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }
    }

    return 0;
}

FilesystemUtil::Offset
FilesystemUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    struct statvfs buffer;
    int rc = statvfs(path, &buffer);
#else
    struct statvfs64 buffer;
    int rc = statvfs64(path, &buffer);
#endif
    if (rc) {
        return -1;                                                    // RETURN
    }
    else {
        // Cast arguments to Offset since the f_bavail and f_frsize fields can
        // be 32-bits, leading to overflow on even small disks.

        return Offset(buffer.f_bavail) * Offset(buffer.f_frsize);     // RETURN
    }
}

FilesystemUtil::Offset
FilesystemUtil::getAvailableSpace(FileDescriptor descriptor)
{
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    struct statvfs buffer;
    int rc = fstatvfs(descriptor, &buffer);
#else
    struct statvfs64 buffer;
    int rc = fstatvfs64(descriptor, &buffer);
#endif
    if (rc) {
        return -1;                                                    // RETURN
    }
    else {
        // Cast arguments to Offset since the f_bavail and f_frsize fields can
        // be 32-bits, leading to overflow on even small disks.

        return Offset(buffer.f_bavail) * Offset(buffer.f_frsize);     // RETURN
    }
}

FilesystemUtil::Offset FilesystemUtil::getFileSize(const char *path)
{
    StatResult fileStats;

    if (0 != ::performStat(path, &fileStats)) {
        return -1;                                                    // RETURN
    }

    return fileStats.st_size;
}

FilesystemUtil::Offset FilesystemUtil::getFileSizeLimit()
{
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    struct rlimit rl, rlMax, rlInf;
    int rc = getrlimit(RLIMIT_FSIZE, &rl);
#else
    struct rlimit64 rl, rlMax, rlInf;
    int rc = getrlimit64(RLIMIT_FSIZE, &rl);
#endif

    // Often, 'rl.rlim_cur' is an unsigned 64 bit, while 'Offset' is signed, so
    // 'rl.rlim_cur' may have a larger value than can be represented by an
    // 'Offset'.

    rlMax.rlim_cur = k_OFFSET_MAX;
    rlInf.rlim_cur = RLIM_INFINITY;

    if (rc) {
        return -1;                                                    // RETURN
    }
    else if (rl.rlim_cur == rlInf.rlim_cur || rl.rlim_cur > rlMax.rlim_cur) {
        return k_OFFSET_MAX;                                          // RETURN
    }
    else {
        return rl.rlim_cur;                                           // RETURN
    }
}

int FilesystemUtil::getWorkingDirectory(bsl::string *path)
{
    BSLS_ASSERT(path);

    enum {BUFFER_SIZE = 4096};
    char buffer[BUFFER_SIZE];

    char *retval = getcwd(buffer, BUFFER_SIZE);
    if (buffer == retval) {
        *path = buffer;

        //our contract requires an absolute path

        return PathUtil::isRelative(*path);                           // RETURN
    }
    return -1;
}

int FilesystemUtil::setWorkingDirectory(const char *path)
{
    BSLS_ASSERT(path);

    return chdir(path);
}
}  // close package namespace

#endif  // non-Windows (POSIX)

namespace bdls {
/////////////////////////////////////
// NON-PLATFORM-SPECIFIC FUNCTIONS //
/////////////////////////////////////

int FilesystemUtil::createDirectories(const char *path,
                                      bool        isLeafDirectoryFlag)
{
    BSLS_ASSERT(path);

    // Implementation note: some Unix platforms may have mkdirp, which does
    // some of what this function does, but not all do.  We have to parse
    // 'path' anyway, since mkdirp does nothing like 'isLeafDirectoryFlag'.

    // Let's first give at least a nod to efficiency and see if we don't need
    // to do anything at all.

    if (!isLeafDirectoryFlag && exists(path)) {
        return 0;                                                     // RETURN
    }

    bsl::string workingPath(path);
    bsl::vector<bsl::string> directoryStack;
    if (!isLeafDirectoryFlag && PathUtil::hasLeaf(workingPath)) {
        PathUtil::popLeaf(&workingPath);
    }

    if (isDirectory(workingPath, true)) {
        return 0;                                                     // RETURN
    }

    while (PathUtil::hasLeaf(workingPath)) {
        directoryStack.push_back(bsl::string());
        int rc = PathUtil::getLeaf(&directoryStack.back(), workingPath);
        (void)rc;
        BSLS_ASSERT(0 == rc);
        PathUtil::popLeaf(&workingPath);
    }

    while (!directoryStack.empty()) {
        PathUtil::appendRaw(&workingPath, directoryStack.back().c_str(),
                             static_cast<int>(directoryStack.back().length()));
        if (0 != makeDirectory(workingPath.c_str(), false)) {
            if (!isDirectory(workingPath, true)) {
                return -1;                                            // RETURN
            }
        }
        directoryStack.pop_back();
    }
    return 0;
}

int FilesystemUtil::createPrivateDirectory(const bslstl::StringRef& path)
{
    bsl::string workingPath = path;  // need NUL termination
    if (0 != makeDirectory(workingPath.c_str(), true)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int FilesystemUtil::findMatchingPaths(bsl::vector<bsl::string> *result,
                                      const char               *pattern)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(pattern);

    result->clear();
    return visitPaths(pattern,
                      bdlf::BindUtil::bind(&pushBackWrapper,
                                           result,
                                           bdlf::PlaceHolders::_1));
}

}  // close package namespace

#if defined(BSLS_PLATFORM_OS_SOLARIS)
namespace {
int posix_fallocate(...)
    // Always return -1.  Overload resolution makes this function inferior to
    // the real 'posix_fallocate' provded it exists.  (It doesn't on Solaris 10
    // and earlier.)
{
    return -1;
}
}  // close unnamed namespace
#endif

namespace bdls {
int FilesystemUtil::growFile(FileDescriptor         descriptor,
                             FilesystemUtil::Offset size,
                             bool                   reserveFlag,
                             bsl::size_t            increment)
{
    bslma::Allocator *allocator_p = bslma::Default::defaultAllocator();
    Offset currentSize = seek(descriptor, 0, e_SEEK_FROM_END);

    if (-1 == currentSize) {
        return -1;                                                    // RETURN
    }
    if (currentSize >= size) {
        return 0;                                                     // RETURN
    }
#if defined(BSLS_PLATFORM_OS_DARWIN)
    // TBD.  See mqbs_filestoreutil.cpp for details.
#endif
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    if (reserveFlag) {
        if (size != seek(descriptor, size, e_SEEK_FROM_BEGINNING)) {
            return -1;                                                // RETURN
        }
        if (0 != SetEndOfFile(descriptor)) {
            reserveFlag = false;  //  File space has been allocated
        }
        else {
            // Undo the seek to 'size'.

            if (currentSize != seek(descriptor, 0, e_SEEK_FROM_END)) {
                return -1;                                            // RETURN
            }
        }
    }
#endif
#if defined(BSLS_PLATFORM_OS_LINUX) ||                                        \
    defined(BSLS_PLATFORM_OS_SOLARIS) ||                                      \
    defined(BSLS_PLATFORM_OS_AIX)
    if (   reserveFlag
        && 0 == posix_fallocate(descriptor, 0, static_cast<off_t>(size))) {
        reserveFlag = false;  //  File space has been allocated
    }
#endif
    if (reserveFlag) {
        // Reserve space the old-fashioned way.

        if (0 == increment) {
            increment = k_DEFAULT_FILE_GROWTH_INCREMENT;
        }
        char *buffer = static_cast<char*>(allocator_p->allocate(increment));
        bsl::memset(buffer, 1, increment);
        Offset bytesToGrow = size - currentSize;
        while (bytesToGrow > 0) {
            int nBytes = static_cast<int>(
                       bsl::min(bytesToGrow, static_cast<Offset>(increment)));
            int rc = write(descriptor, buffer, nBytes);
            if (rc != nBytes) {
                allocator_p->deallocate(buffer);
                return -1;                                            // RETURN
            }
            bytesToGrow -= nBytes;
        }
        allocator_p->deallocate(buffer);
        return 0;                                                     // RETURN
    }
    Offset res = seek(descriptor, size - 1, e_SEEK_FROM_BEGINNING);
    if (-1 == res || 1 != write(descriptor, "", 1))
    {
        return -1;                                                    // RETURN
    }
    return 0;
}

int FilesystemUtil::rollFileChain(const char *path, int maxSuffix)
{
    BSLS_ASSERT(path);

    enum { MAX_SUFFIX_LENGTH = 10 };
    bslma::Allocator *allocator_p = bslma::Default::defaultAllocator();
    int length = static_cast<int>(bsl::strlen(path)) + MAX_SUFFIX_LENGTH + 2;

    // Use a single allocation to insure exception neutrality.

    char *buffer   = static_cast<char*>(allocator_p->allocate(2 * length));
    char *fromName = buffer;
    char *toName   = buffer + length;
    snprintf(toName, length, "%s.%d", path, maxSuffix);

    if (!exists(toName) || 0 == remove(toName, true)) {
        while (0 < --maxSuffix) {
            snprintf(fromName, length, "%s.%d", path, maxSuffix);

            if (exists(fromName) && 0 != move(fromName, toName)) {
                break;
            }
            using bsl::swap;
            swap(toName, fromName);
        }

        if (0 == maxSuffix && exists(path) && 0 != move(path, toName)) {
            maxSuffix = -1;
        }
    }
    allocator_p->deallocate(buffer);
    return maxSuffix;
}

FilesystemUtil::FileDescriptor
FilesystemUtil::createTemporaryFile(bsl::string             *outPath,
                                    const bslstl::StringRef& prefix)
{
    BSLS_ASSERT(outPath);

    FileDescriptor result;
    bsl::string localOutPath = *outPath;
    for (int i = 0; i < 10; ++i) {
        makeUnsafeTemporaryFilename(&localOutPath, prefix);
        result = bdls::FilesystemUtil::open(
           (const char*) localOutPath.c_str(), e_CREATE_PRIVATE, e_READ_WRITE);
        if (result != k_INVALID_FD) {
            *outPath = localOutPath;
            break;
        }
    }
    return result;
}

int
FilesystemUtil::createTemporaryDirectory(bsl::string             *outPath,
                                         const bslstl::StringRef& prefix)
{
    BSLS_ASSERT(outPath);

    int result;
    bsl::string localOutPath = *outPath;
    for (int i = 0; i < 10; ++i) {
        makeUnsafeTemporaryFilename(&localOutPath, prefix);
        result = bdls::FilesystemUtil::createPrivateDirectory(localOutPath);
        if (result == 0) {
            *outPath = localOutPath;
            break;
        }
    }
    return result;
}

void
FilesystemUtil::makeUnsafeTemporaryFilename(bsl::string             *outPath,
                                            const bslstl::StringRef& prefix)
{
    BSLS_ASSERT(outPath);

    char suffix[8];
    bsls::Types::Int64 now = bsls::TimeUtil::getTimer();
    bsls::Types::Uint64 tid =
                    bslmt::ThreadUtil::idAsUint64(bslmt::ThreadUtil::selfId());
    using bslh::hashAppend;
    bslh::DefaultHashAlgorithm hashee;
    hashAppend(hashee, now);
    hashAppend(hashee, prefix);
    hashAppend(hashee, *outPath);
    hashAppend(hashee, tid);
    hashAppend(hashee, getProcessId());
    bslh::DefaultHashAlgorithm::result_type hash = hashee.computeHash();
    for (int i = 0; i < int(sizeof(suffix)); ++i) {
        static const char s[63] =
              "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        suffix[i] = s[hash % 62];
        hash /= 62;
    }
    *outPath = prefix;
    outPath->append(suffix, sizeof(suffix));
}

}  // close package namespace

}  // close enterprise namespace

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
