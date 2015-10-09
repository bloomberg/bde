// bdls_filesystemutil.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_filesystemutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_filesystemutil_cpp,"$Id$ $CSID$")

#include <bdls_memoryutil.h>
#include <bdls_pathutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlt_epochutil.h>
#include <bdlt_currenttime.h> // for testing only
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>


#include <bsl_algorithm.h>
#include <bsl_c_stdio.h> // needed for rename on AIX & snprintf everywhere
#include <bsl_cstring.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <bdlde_charconvertutf16.h>
#undef MIN
#define snprintf _snprintf

#else // !BSLS_PLATFORM_OS_WINDOWS
#include <bsl_c_errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <glob.h>
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS
#endif
#include <bsl_c_limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h> // for testing only ... for now
#include <sys/uio.h>
#include <sys/resource.h>
#include <sys/statvfs.h>
#endif

// PRIVATE CONSTANTS
enum {
    k_UNKNOWN_ERROR = 127
};

// STATIC HELPER FUNCTIONS

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
int makeDirectory(const char *path)
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
    int rc;
    do {
        struct flock flk;
        flk.l_type = static_cast<short>(type);
        flk.l_whence = SEEK_SET;
        flk.l_start = 0;
        flk.l_len = 0;
        rc = fcntl(descriptor, cmd, &flk);
    } while (EINTR == rc);
    return rc;
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

    closedir(static_cast<DIR *>(dir));
}

static
bool isDotOrDots(const char *path)
    // Return 'true' if the specified 'path' is "." or ".." or ends in
    // "/." or "/..", and 'false' otherwise.
{
    BSLS_ASSERT(path);

    const int length = static_cast<int>(bsl::strlen(path));

    return  (2 <= length && '/' == path[length - 2] &&
                            '.' == path[length - 1]) ||

            (3 <= length && '/' == path[length - 3] &&
                            '.' == path[length - 2] &&
                            '.' == path[length - 1]) ||

            (1 == length && '.' == path[0]) ||

            (2 == length && '.' == path[0] &&
                            '.' == path[1]);
}

static inline
int makeDirectory(const char *path)
    // Create a directory
{
    BSLS_ASSERT_SAFE(path);

    // Permissions of created dir will be 'drwxrwxrwx', ANDed with '~umask'.

    enum { PERMS = S_IRUSR | S_IWUSR | S_IXUSR |    // user   rwx
                   S_IRGRP | S_IWGRP | S_IXGRP |    // group  rwx
                   S_IROTH | S_IWOTH | S_IXOTH };   // others rwx
    return mkdir(path, PERMS);
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
      case e_CREATE: {
        // Fails if file exists.

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
                        int              len,
                        int              mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= len);

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

int FilesystemUtil::unmap(void *address, int)
{
    BSLS_ASSERT(address);

    return UnmapViewOfFile(address) ? 0 : -1;
}

int FilesystemUtil::sync(char *address, int numBytes, bool)
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

void FilesystemUtil::visitPaths(
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

    bsl::string dirName;
    if (0 != PathUtil::getDirname(&dirName, patternStr)) {
        // There is no leaf, therefore there can be nothing to do (but not an
        // error)

        return;                                                       // RETURN
    }

    if (bsl::string::npos != dirName.find_first_of("*?")) {

        bsl::vector<bsl::string> leaves;
        bsl::vector<bsl::string> paths, workingPaths;
        bsl::string pattern = patternStr;
        while (PathUtil::hasLeaf(pattern)) {
            leaves.push_back(bsl::string());
            int rc = PathUtil::getLeaf(&leaves.back(), pattern);
            (void) rc;  // Used only in assert.
            BSLS_ASSERT(0 == rc);
            PathUtil::popLeaf(&pattern);
        }

        paths.push_back(pattern);
        while (!leaves.empty()) {
            for (bsl::vector<bsl::string>::iterator it = paths.begin();
                                                     it != paths.end(); ++it) {
                PathUtil::appendRaw(&(*it), leaves.back().c_str(),
                                                   leaves.back().length(), -1);
            }
            if (bsl::string::npos != leaves.back().find_first_of("*?")) {
                // We just put a leaf pattern onto each path.  Need to expand
                // them recursively.

                workingPaths = paths;
                paths.clear();
                bsl::vector<bsl::string>::iterator it;
                for (it = workingPaths.begin(); it != workingPaths.end();
                                                                        ++it) {
                    visitPaths(it->c_str(), bdlf::BindUtil::bind(
                                                      &pushBackWrapper,
                                                      &paths,
                                                      bdlf::PlaceHolders::_1));
                }
            }
            leaves.pop_back();
        }
        for (bsl::vector<bsl::string>::iterator it = paths.begin();
                                                     it != paths.end(); ++it) {
            visitor(it->c_str());
        }
    }
    else {
        // No special characters except possibly in the leaf.  This is the BASE
        // CASE.

        bsl::string      dirNamePath = dirName;
        WIN32_FIND_DATAW findDataW;
        HANDLE           handle;
        bsl::wstring     widePattern;
        bsl::string      narrowName;

        if (!narrowToWide(&widePattern, patternStr)) {
            return;                                                   // RETURN
        }

        handle = FindFirstFileW(widePattern.c_str(), &findDataW);

        if (INVALID_HANDLE_VALUE == handle) {
            return;                                                   // RETURN
        }

        bslma::ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);

        bool next;

        do {
            static const bsl::string dot    = ".";
            static const bsl::string dotdot = "..";

            if (!wideToNarrow(&narrowName, findDataW.cFileName)) {
                // Can't happen: wideToNarrow won't fail.

                BSLS_ASSERT(!"Wide-to-narrow conversion failed.");
            }

            if (dot == narrowName || dotdot == narrowName) {
                // Do nothing
            }
            else if (0 != PathUtil::appendIfValid(&dirNamePath, narrowName)) {
                // Can't happen: 'findData.cFileName' will never be an absolute
                // path.

                BSLS_ASSERT(!"FindFirstFileW returned an absolute path.");
            }
            else {
                visitor(dirNamePath.c_str());
                PathUtil::popLeaf(&dirNamePath);
            }

            next = FindNextFileW(handle, &findDataW);
        } while (next);
    }
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

    static HMODULE hNtDll = LoadLibrary("ntdll.dll");
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

        return PathUtil::isRelative(*path);                     // RETURN
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
         // What we'd LIKE to do here is findMatchingPaths("path/*") and delete
         // each one.  But glob(), on which findMatchingPaths() is built, will
         // not include the name of a symbolic link if there is no file
         // attached.  Thus a bad link would prevent a directory from being
         // removed.  So instead we must open and read the directory ourselves
         // and remove everything that's not "." or "..", checking for
         // directories (*without* following links) and recursing as necessary.
         DIR *dir = opendir(path);
         if (0 == dir) {
            return -1;                                                // RETURN
         }
         bslma::ManagedPtr<DIR> dirGuard(dir, 0, &invokeCloseDir);

         bsl::string workingPath = path;

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
         StatResult dummy;
         int rc;
         do {
            rc = readdir_r(dir, &entry, &entry_p);
            if (0 != rc) {
               break;
            }

            if (isDotOrDots(entry.d_name)) {
               continue;
            }

            PathUtil::appendRaw(&workingPath, entry.d_name);
            if (0 == ::performStat(workingPath.c_str(), &dummy, false) &&
                0 != remove(workingPath.c_str(), true)) {
               return -1;                                             // RETURN
            }
            PathUtil::popLeaf(&workingPath);
         } while (&entry == entry_p);
      }

      return removeDirectory(path);                                   // RETURN
   }
   else {
      return removeFile(path);                                        // RETURN
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
                        int              size,
                        int              mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= size);

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

int  FilesystemUtil::unmap(void *address, int size)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= size);

    int rc = munmap(static_cast<char *>(address), size);
    return rc;
}

int FilesystemUtil::sync(char *address, int numBytes, bool syncFlag)
{
    BSLS_ASSERT(0 != address);
    BSLS_ASSERT(0 <= numBytes);
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
    return -1 == rc ? -1 : 0;
}

int FilesystemUtil::unlock(FileDescriptor descriptor)
{
    return -1 == localFcntlLock(descriptor, F_SETLK, F_UNLCK) ? -1 : 0;
}

int FilesystemUtil::move(const char *oldPath, const char *newPath)
{
    BSLS_ASSERT(oldPath);
    BSLS_ASSERT(newPath);

    return rename(oldPath, newPath);
}

bool FilesystemUtil::exists(const char *path)
{
    BSLS_ASSERT(path);

    return 0 == access(path, F_OK);
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

void FilesystemUtil::visitPaths(
                             const char                               *pattern,
                             const bsl::function<void(const char *)>&  visitor)
{
    BSLS_ASSERT(pattern);

    glob_t pglob;

    int rc = glob(pattern, GLOB_NOSORT, 0, &pglob);
    bslma::ManagedPtr<glob_t> globGuard(&pglob, 0, &invokeGlobFree);
    if (GLOB_NOMATCH == rc) {
        return;                                                       // RETURN
    }
    if (GLOB_NOSPACE == rc) {
        bsls::BslExceptionUtil::throwBadAlloc();
    }

    for (int i = 0; i < static_cast<int>(pglob.gl_pathc); ++i) {
        BSLS_ASSERT(pglob.gl_pathv[i]);
        if (!isDotOrDots(pglob.gl_pathv[i])) {
            visitor(pglob.gl_pathv[i]);
        }
    }
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
    // Implementation note: some Unix platforms may have mkdirp, which does
    // what this function does.  But not all do, and hyper-fast performance is
    // not a concern for a method like this, so let's just roll our own to
    // ensure maximum portability.
    //
    // Not to mention that we have to do at least a little parsing anyway,
    // since even mkdirp does not provide anything like 'isLeafDirectoryFlag'.

    // Let's first give at least a nod to efficiency and see if we don't need
    // to do anything at all.

    BSLS_ASSERT(path);

    if (exists(path)) {
        return 0;                                                     // RETURN
    }

    bsl::string workingPath(path);
    bsl::vector<bsl::string> directoryStack;
    if (!isLeafDirectoryFlag && PathUtil::hasLeaf(workingPath)) {
        PathUtil::popLeaf(&workingPath);
    }

    while (PathUtil::hasLeaf(workingPath)) {
        directoryStack.push_back(bsl::string());
        int rc = PathUtil::getLeaf(&directoryStack.back(), workingPath);
        BSLS_ASSERT(0 == rc);
        PathUtil::popLeaf(&workingPath);
    }

    while (!directoryStack.empty()) {
        PathUtil::appendRaw(&workingPath,
                            directoryStack.back().c_str(),
                            static_cast<int>(directoryStack.back().length()));
        if (!exists(workingPath.c_str())) {
            if (0 != makeDirectory(workingPath.c_str())) {
                return -1;                                            // RETURN
            }
        }
        directoryStack.pop_back();
    }
    return 0;
}

void FilesystemUtil::findMatchingPaths(bsl::vector<bsl::string> *result,
                                       const char               *pattern)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(pattern);

    result->clear();
    visitPaths(pattern,
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
    if (reserveFlag && 0 == posix_fallocate(descriptor, 0, size)) {
        reserveFlag = false;  //  File space has been allocated
    }
#endif
    if (reserveFlag) {
        // Reserve space the old-fashioned way.
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
