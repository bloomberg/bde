// bdesu_fileutil.cpp                                                 -*-C++-*-
#include <bdesu_fileutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_fileutil_cpp,"$Id$ $CSID$")

#include <bdema_managedptr.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdesu_pathutil.h>
#include <bdetu_epoch.h>
#include <bdetu_systemtime.h> // for testing only

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_c_stdio.h> // needed for rename on AIX & snprintf everywhere

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#include <io.h>
#include <direct.h>
#undef MIN
#define getcwd _getcwd
#define chdir _chdir
#define snprintf _snprintf
#else

#ifdef BSLS_PLATFORM__OS_HPUX
#define _LARGEFILE64_SOURCE  // activates '64' variants of open() etc
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <glob.h>
#include <bsl_c_errno.h>
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS
#endif
#include <unistd.h>
#include <dirent.h>
#include <utime.h> // for testing only ... for now
#include <sys/uio.h>
#include <bsl_c_limits.h>
#include <sys/resource.h>
#include <sys/statvfs.h>
#endif

#include <bsl_algorithm.h>

// STATIC HELPER FUNCTIONS
static
void pushBackWrapper(bsl::vector<bsl::string> *vector, const char *item)
    // A 'thunk' to be bound to a vector that can be called to push an item to
    // the vector.
{
    BSLS_ASSERT(vector);

    vector->push_back(item);
}

#ifdef BSLS_PLATFORM__OS_WINDOWS
static inline
void invokeFindClose(HANDLE *handle, void *)
    // Provides a function signature which can be used as a
    // 'bdema_ManagedPtr' deleter (i.e., we need to define a second argument
    // of type 'void *', which will be ignored).
{
    BSLS_ASSERT_SAFE(handle);

    FindClose(*handle);
}

static inline
int makeDirectory(const char *path)
    // Create a directory
{
    BSLS_ASSERT_SAFE(path);

    return 0 == CreateDirectory(path, 0);
}

static inline
int removeDirectory(const char *path)
    // Remove directory 'path'.  Return 0 on success and a non-zero value
    // otherwise.
{
    BSLS_ASSERT_SAFE(path);

    return 0 == RemoveDirectory(path);
}

static inline
int removeFile(const char *path)
    // Remove plain file 'path'.  Return 0 on success and a non-zero value
    // otherwise.
{
    BSLS_ASSERT_SAFE(path);

    return 0 == DeleteFile(path);
}

#else
// unix-specific helper functions

extern "C" {
    // Need a special 'typedef' because some compilers put the 'extern'
    // into the function type itself...but provide no way to declare
    // a function pointer of extern type local to a function.

    typedef int (*StatFuncType)(const char *, struct stat *);
}

static
int localFcntlLock(int fd, int cmd, int type)
{
    int rc;
    do {
        struct flock flk;
        flk.l_type = type;
        flk.l_whence = SEEK_SET;
        flk.l_start = 0;
        flk.l_len = 0;
        rc = fcntl(fd, cmd, &flk);
    } while (rc == EINTR);
    return rc;
}

static inline
void invokeGlobFree(glob_t *pglob, void *)
{
    BSLS_ASSERT_SAFE(pglob);

    globfree(pglob);
}

static inline
void invokeCloseDir(DIR *dir, void *)
{
    BSLS_ASSERT_SAFE(dir);

    closedir(dir);
}

static
bool isDotOrDots(const char *path)
    // Return 'true' if the specified 'path' is "." or ".." or ends in
    // "/." or "/..", and 'false' otherwise.
{
    BSLS_ASSERT(path);

    const int length = static_cast<int>(bsl::strlen(path));

    return  (length >= 2 && '/' == path[length - 2] &&
                            '.' == path[length - 1]) ||

            (length >= 3 && '/' == path[length - 3] &&
                            '.' == path[length - 2] &&
                            '.' == path[length - 1]) ||

            (length == 1 && '.' == path[0]) ||

            (length == 2 && '.' == path[0] &&
                            '.' == path[1]);
}

static inline
int makeDirectory(const char *path)
    // Create a directory
{
    BSLS_ASSERT_SAFE(path);

    // 755 octal = RWX by user, RX by group, RX by all
    return mkdir(path, 0755);
}

static inline
int removeDirectory(const char *path)
    // Remove directory 'path'.  Return 0 on success and a non-zero value
    // otherwise.
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

namespace BloombergLP {

                              // ---------------------
                              // struct bdesu_FileUtil
                              // ---------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS

const bdesu_FileUtil::FileDescriptor bdesu_FileUtil::INVALID_FD =
                                                          INVALID_HANDLE_VALUE;

bdesu_FileUtil::FileDescriptor
bdesu_FileUtil::open(const char *pathName,
                     bool        writableFlag,
                     bool        existFlag,
                     bool        appendFlag)
{
    BSLS_ASSERT(pathName);

    DWORD accessMode   = GENERIC_READ | (writableFlag
                                         ? appendFlag
                                           ? FILE_APPEND_DATA
                                           : GENERIC_WRITE
                                         : 0);
    DWORD creationInfo = existFlag ? OPEN_EXISTING : CREATE_ALWAYS;

    // The file locking behavior for the opened file 
    // ('FILE_SHARE_READ | FILE_SHARE_WRITE') is chosen to match the posix
    // behavior for open (DRQS 30568749).
 
    return CreateFile(pathName,
                      accessMode,
                      FILE_SHARE_READ | FILE_SHARE_WRITE, // do not lock
                      NULL,                               // default security
                      creationInfo,                       // existing file only
                      FILE_ATTRIBUTE_NORMAL,              // normal file
                      NULL);                              // no attr

}

int bdesu_FileUtil::close(FileDescriptor fd)
{
    return CloseHandle(fd) ? 0 : -1;
}

int bdesu_FileUtil::remove(const char *fileToRemove, bool recursive)
{
    BSLS_ASSERT(fileToRemove);

    if (isDirectory(fileToRemove)) {
        if (recursive) {
            bsl::string pattern(fileToRemove);
            bdesu_PathUtil::appendRaw(&pattern, "*");
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

bdesu_FileUtil::Offset
bdesu_FileUtil::seek(FileDescriptor         fd,
                     bdesu_FileUtil::Offset offset,
                     int                    whence)
{
    switch (whence) {
      case BDESU_SEEK_FROM_BEGINNING: whence = FILE_BEGIN; break;
      case BDESU_SEEK_FROM_CURRENT:   whence = FILE_CURRENT; break;
      case BDESU_SEEK_FROM_END:       whence = FILE_END; break;
      default: BSLS_ASSERT(0);  break;
    }
    LARGE_INTEGER li;
    li.QuadPart = offset;
    li.LowPart = SetFilePointer(fd, li.LowPart, &li.HighPart, whence);
    if (li.LowPart == INVALID_SET_FILE_POINTER
       && GetLastError() != NO_ERROR )
    {
        return -1;                                                    // RETURN
    }
    return li.QuadPart;
}

int bdesu_FileUtil::read(FileDescriptor  fd,
                         void           *buf,
                         int             numBytesToRead)
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(0 <= numBytesToRead);

    DWORD n;
    return ReadFile(fd, buf, numBytesToRead, &n, 0) ? n : -1;
}

int bdesu_FileUtil::write(FileDescriptor  fd,
                          const void     *buf,
                          int             numBytesToWrite)
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(0 <= numBytesToWrite);

    DWORD n;
    return WriteFile(fd, buf, numBytesToWrite, &n, 0) ? n : -1;
}

int bdesu_FileUtil::map(FileDescriptor           fd,
                        void                   **addr,
                        bdesu_FileUtil::Offset   offset,
                        int                      len,
                        int                      mode)
{
    BSLS_ASSERT(addr);
    BSLS_ASSERT(0 <= len);

    HANDLE hMap;

    if (mode == bdesu_MemoryUtil::BDESU_ACCESS_NONE) {
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

    bdesu_FileUtil::Offset maxLength = offset + len;
    hMap = CreateFileMapping(fd,
                             NULL,
                             protectAccess[mode][0],
                             (DWORD)(maxLength>>32),
                             (DWORD)(maxLength&0xFFFFFFFF),
                             NULL);

    if (hMap == NULL) {
        *addr = 0;
        return -1;                                                    // RETURN
    }

    *addr = MapViewOfFile(hMap,
                          protectAccess[mode][1],
                          (DWORD)(offset>>32),
                          (DWORD)(offset&0xFFFFFFFF),
                          len);
    CloseHandle(hMap);
    if (!*addr) {
        return -1;                                                    // RETURN
    }
    return 0;
}

int bdesu_FileUtil::unmap(void *addr, int)
{
    BSLS_ASSERT(addr);

    return UnmapViewOfFile(addr) ? 0 : -1;
}

int bdesu_FileUtil::sync(char *addr, int numBytes, bool)  // 3rd arg is sync
{
    BSLS_ASSERT(addr);
    BSLS_ASSERT(0 <= numBytes);

    // The meaning of the 'sync' flag (cause this function to be
    // synchronous vs. asynchronous) does not appear to be supported by
    // 'FlushViewOfFile'.

    return FlushViewOfFile(addr, numBytes) ? 0 : -1;
}

int bdesu_FileUtil::lock(FileDescriptor fd, bool lockWrite)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    return !LockFileEx(fd, lockWrite ? LOCKFILE_EXCLUSIVE_LOCK
                                     : 0, 0, 1, 0, &overlapped);
}
int bdesu_FileUtil::tryLock(FileDescriptor fd, bool lockWrite)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    bool success = LockFileEx(fd, LOCKFILE_FAIL_IMMEDIATELY |
                              (lockWrite ? LOCKFILE_EXCLUSIVE_LOCK : 0),
                              0, 1, 0, &overlapped);
    return success ? 0
                   : ERROR_LOCK_VIOLATION == GetLastError()
                     ? BDESU_ERROR_LOCKING_CONFLICT
                     : -1;
}

int bdesu_FileUtil::unlock(FileDescriptor fd)
{
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    return !UnlockFileEx(fd, 0, 1, 0,  &overlapped);
}

int bdesu_FileUtil::move(const char *oldName, const char *newName)
{
    BSLS_ASSERT(oldName);
    BSLS_ASSERT(newName);

    if (exists(newName)) {
        DeleteFile(newName);
    }
    return MoveFile(oldName, newName) ? 0 : -1;
}

bool bdesu_FileUtil::exists(const char *pathName)
{
    BSLS_ASSERT(pathName);

    return GetFileAttributes(pathName) != INVALID_FILE_ATTRIBUTES;
}

int bdesu_FileUtil::getLastModificationTime(bdet_Datetime *time,
                                            const char    *path)
{
    BSLS_ASSERT(time);
    BSLS_ASSERT(path);

    WIN32_FIND_DATA findData;
    HANDLE handle = FindFirstFile(path, &findData);
    if (handle == INVALID_HANDLE_VALUE) {
        return -1;                                                    // RETURN
    }
    bdema_ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);

    SYSTEMTIME stUTC;
    if (0 == FileTimeToSystemTime(&findData.ftLastWriteTime, &stUTC)) {
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

void bdesu_FileUtil::visitPaths(
                        const char                                 *patternStr,
                        const bdef_Function<void(*)(const char*)>&  visitor)
{
    // Windows FindFirstFile() function only looks at pattern characters ('*'
    // and '?') in the leaf element of the path.  Therefore we must first
    // resolve all special characters in the non-leaf portion of the path
    // recursively before resolving the leaf portion.  Also, unlike glob(), the
    // name returned by FindFirstFile() is *just* the leaf.  Resolving the
    // handle into a full path is not available pre-Vista.

    BSLS_ASSERT(patternStr);

    bsl::string dirName;
    if (0 != bdesu_PathUtil::getDirname(&dirName, patternStr)) {
        // There is no leaf, therefore there can be nothing to do (but
        // not an error)

        return;                                                       // RETURN
    }

    if (bsl::string::npos != dirName.find_first_of("*?")) {

        bsl::vector<bsl::string> leaves;
        bsl::vector<bsl::string> paths, workingPaths;
        bsl::string pattern = patternStr;
        while(bdesu_PathUtil::hasLeaf(pattern)) {
            leaves.push_back(bsl::string());
            int rc = bdesu_PathUtil::getLeaf(&leaves.back(), pattern);
            BSLS_ASSERT(0 == rc);
            bdesu_PathUtil::popLeaf(&pattern);
        }

        paths.push_back(pattern);
        while (!leaves.empty()) {
            for (bsl::vector<bsl::string>::iterator it = paths.begin();
                                                     it != paths.end(); ++it) {
                bdesu_PathUtil::appendRaw(&(*it), leaves.back().c_str(),
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
                    visitPaths(it->c_str(), bdef_BindUtil::bind(
                                          &pushBackWrapper,
                                          &paths, bdef_PlaceHolders::_1));
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

        bsl::string dirNamePath = dirName.c_str();

        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(patternStr, &findData);
        if (INVALID_HANDLE_VALUE == handle) {
            return;                                                   // RETURN
        }

        bdema_ManagedPtr<HANDLE> handleGuard(&handle, 0, &invokeFindClose);
        do {
            if (0 == strcmp(".", findData.cFileName) ||
                0 == strcmp("..", findData.cFileName)) {
                // Do nothing
            }
            else if (0 != bdesu_PathUtil::appendIfValid(&dirNamePath,
                                                        findData.cFileName)) {
                //TBD
            }
            else {
                visitor(dirNamePath.c_str());
                bdesu_PathUtil::popLeaf(&dirNamePath);
            }
        } while (0 != FindNextFile(handle, &findData));
    }
}

bool bdesu_FileUtil::isRegularFile(const char *path, bool)
{
    BSLS_ASSERT(path);

    DWORD stats = GetFileAttributes(path);
    return (stats != INVALID_FILE_ATTRIBUTES &&
            0 == (stats & FILE_ATTRIBUTE_DIRECTORY));
}

bool bdesu_FileUtil::isDirectory(const char *path, bool)
{
    BSLS_ASSERT(path);

    DWORD stats = GetFileAttributes(path);
    return stats != INVALID_FILE_ATTRIBUTES
        && (stats & FILE_ATTRIBUTE_DIRECTORY);
}

bdesu_FileUtil::Offset bdesu_FileUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

    __int64 avail;
    if (!GetDiskFreeSpaceEx(path, (PULARGE_INTEGER)&avail, NULL, NULL)) {
        return -1;                                                    // RETURN
    }
    return avail;
}

bdesu_FileUtil::Offset bdesu_FileUtil::getAvailableSpace(FileDescriptor fd)
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
        HANDLE FileHandle,
        IO_STATUS_BLOCK *IoStatusBlock,
        PVOID FileInformation,
        ULONG Length,
        INT FileInformationClass);

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
    LONG rc = pNQVIF(fd, &ioStatusBlock, &sizeInfo, sizeof(sizeInfo),
        fileFsSizeInformation);
    if (rc) {
        return -1;                                                    // RETURN
    }
    return sizeInfo.AvailableAllocationUnits.QuadPart
        * sizeInfo.SectorsPerAllocationUnit
        * sizeInfo.BytesPerSector;
}

bdesu_FileUtil::Offset bdesu_FileUtil::getFileSize(const char *path)
{
    BSLS_ASSERT(path);

    WIN32_FILE_ATTRIBUTE_DATA fileAttribute;
    if(!GetFileAttributesEx(path, GetFileExInfoStandard,
                                                     (void *)&fileAttribute)) {
        return -1;                                                    // RETURN
    }
    return (((bdesu_FileUtil::Offset)fileAttribute.nFileSizeHigh) << 32)
           | fileAttribute.nFileSizeLow;
}

bdesu_FileUtil::Offset bdesu_FileUtil::getFileSizeLimit()
{
    // TBD

    return OFFSET_MAX;
}

#else
// unix specific implementation

const bdesu_FileUtil::FileDescriptor bdesu_FileUtil::INVALID_FD = -1;

bdesu_FileUtil::FileDescriptor
bdesu_FileUtil::open(const char *pathName,
                     bool        writableFlag,
                     bool        existFlag,
                     bool        appendFlag)
{
    BSLS_ASSERT(pathName);

    const int oflag = (writableFlag ? O_RDWR : O_RDONLY)
                      | (writableFlag && appendFlag ? O_APPEND : 0);

    if (existFlag) {
#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
        return ::open(  pathName, oflag);                             // RETURN
#elif defined(BSLS_PLATFORM__OS_HPUX)
        // In 64-bit mode, HP-UX defines 'open64' to be 'open', which triggers
        // a lookup failure here (since this class has members named 'open').
        return ::open64(pathName, oflag);                             // RETURN
#else
        return open64(  pathName, oflag);                             // RETURN
#endif
    }

#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
    return ::open(  pathName, oflag | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
#elif defined(BSLS_PLATFORM__OS_HPUX)
    return ::open64(pathName, oflag | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
#else
    return open64(  pathName, oflag | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
#endif
}

int bdesu_FileUtil::close(FileDescriptor fd)
{
    return ::close(fd);
}

bdesu_FileUtil::Offset
bdesu_FileUtil::seek(FileDescriptor fd, Offset offset, int whence)
{
    switch (whence) {
#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
      case BDESU_SEEK_FROM_BEGINNING:
        return lseek(fd, offset, SEEK_SET);                           // RETURN
      case BDESU_SEEK_FROM_CURRENT:
        return lseek(fd, offset, SEEK_CUR);                           // RETURN
      case BDESU_SEEK_FROM_END:
        return lseek(fd, offset, SEEK_END);                           // RETURN
#else
      case BDESU_SEEK_FROM_BEGINNING:
        return lseek64(fd, offset, SEEK_SET);                         // RETURN
      case BDESU_SEEK_FROM_CURRENT:
        return lseek64(fd, offset, SEEK_CUR);                         // RETURN
      case BDESU_SEEK_FROM_END:
        return lseek64(fd, offset, SEEK_END);                         // RETURN
#endif
      default:
        return -1;                                                    // RETURN
    }
}

int bdesu_FileUtil::remove(const char *fileToRemove, bool recursive)
{
   BSLS_ASSERT(fileToRemove);

   if (isDirectory(fileToRemove)) {
      if (recursive) {
         // What we'd LIKE to do here is findMatchingPaths("fileToRemove/*")
         // and delete each one.  But glob(), on which findMatchingPaths()
         // is built, will not include the name of a symbolic link if there
         // is no file attached.  Thus a bad link would prevent a directory
         // from being removed.  So instead we must open and read the
         // directory ourselves and remove everything that's not "." or
         // "..", checking for directories (*without* following links) and
         // recursing as necessary.
         DIR *dir = opendir(fileToRemove);
         if (0 == dir) {
            return -1;                                                // RETURN
         }
         bdema_ManagedPtr<DIR> dirGuard(dir, 0, &invokeCloseDir);

         bsl::string path = fileToRemove;

         // The amount of space available in the 'd_name' member of
         // the dirent struct is apparently "implementation-defined" and
         // in particular is allowed to be less than the maximum
         // path length (!).  The very C-style way to fix this is to make
         // sure that there's lots of extra space available at the end of the
         // struct (d_name is always the last member) so that strcpy can
         // happily copy into it without instigating a buffer overrun attack
         // against us =)

         enum {OVERFLOW_SIZE = 2048}; //probably excessive, but it's just stack
         union {
            struct dirent d_entry;
            char d_overflow[OVERFLOW_SIZE];
         } entryHolder;

         struct dirent& entry = entryHolder.d_entry;
         struct dirent *entry_p;
         struct stat dummy;
         int rc;
         do {
            rc = readdir_r(dir, &entry, &entry_p);
            if (0 != rc) {
               break;
            }

            if (isDotOrDots(entry.d_name)) {
               continue;
            }

            bdesu_PathUtil::appendRaw(&path, entry.d_name);
            if (0 == lstat(path.c_str(), &dummy) &&
                0 != remove(path.c_str(), true)) {
               return -1;                                             // RETURN
            }
            bdesu_PathUtil::popLeaf(&path);
         } while (entry_p == &entry);
      }

      return removeDirectory(fileToRemove);                           // RETURN
   }
   else {
      return removeFile(fileToRemove);                                // RETURN
   }
}

int bdesu_FileUtil::read(FileDescriptor  fd,
                         void           *buf,
                         int             numBytesToRead)
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(0 <= numBytesToRead);

    return static_cast<int>(::read(fd, buf, numBytesToRead));
}

int bdesu_FileUtil::write(FileDescriptor  fd,
                          const void     *buf,
                          int             numBytesToWrite)
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(0 <= numBytesToWrite);

    return static_cast<int>(::write(fd, buf, numBytesToWrite));
}

int bdesu_FileUtil::map(FileDescriptor   fd,
                        void           **addr,
                        Offset           offset,
                        int              size,
                        int              mode)
{
    BSLS_ASSERT(addr);
    BSLS_ASSERT(0 <= size);

    int protect = 0;
    if (mode & bdesu_MemoryUtil::BDESU_ACCESS_READ)    protect |= PROT_READ;
    if (mode & bdesu_MemoryUtil::BDESU_ACCESS_WRITE)   protect |= PROT_WRITE;
    if (mode & bdesu_MemoryUtil::BDESU_ACCESS_EXECUTE) protect |= PROT_EXEC;

#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
    *addr = mmap(0, size, protect, MAP_SHARED, fd, offset);
#else
    *addr = mmap64(0, size, protect, MAP_SHARED, fd, offset);
#endif

    if (*addr == MAP_FAILED) {
        *addr = NULL;
        return -1;                                                    // RETURN
    } else {
        return 0;                                                     // RETURN
    }
}

int  bdesu_FileUtil::unmap(void *addr, int len)
{
    BSLS_ASSERT(addr);
    BSLS_ASSERT(0 <= len);

    int rc = munmap((char *)addr, len);
    return rc;
}

int bdesu_FileUtil::sync(char *addr, int numBytes, bool sync)
{
    BSLS_ASSERT(addr);
    BSLS_ASSERT(0 <= numBytes);

    return ::msync(addr, numBytes, sync ? MS_SYNC : MS_ASYNC);
}

int bdesu_FileUtil::tryLock(FileDescriptor fd, bool lockWrite)
{
    int rc = localFcntlLock(fd, F_SETLK, lockWrite ? F_WRLCK : F_RDLCK);
    return -1 != rc ? 0
                    : EAGAIN == errno || EACCES == errno
                      ? BDESU_ERROR_LOCKING_CONFLICT
                      : -1;
}

int bdesu_FileUtil::lock(FileDescriptor fd, bool lockWrite)
{
    return localFcntlLock(fd, F_SETLKW, lockWrite ? F_WRLCK : F_RDLCK) == -1
           ? -1
           : 0;
}

int bdesu_FileUtil::unlock(FileDescriptor fd)
{
    return localFcntlLock(fd, F_SETLK, F_UNLCK) == -1 ? -1 : 0;
}

int bdesu_FileUtil::move(const char *oldName, const char *newName)
{
    BSLS_ASSERT(oldName);
    BSLS_ASSERT(newName);

    return rename(oldName, newName);
}

bool bdesu_FileUtil::exists(const char *pathName)
{
    BSLS_ASSERT(pathName);

    return access(pathName, F_OK) == 0;
}

bool bdesu_FileUtil::isRegularFile(const char *path, bool followLinks)
{
    BSLS_ASSERT(path);

    struct stat fileStats;

    StatFuncType statFunc = followLinks ? stat : lstat;

    if (0 != statFunc(path, &fileStats)) {
        return false;                                                 // RETURN
    }

    return S_ISREG(fileStats.st_mode);
}

bool bdesu_FileUtil::isDirectory(const char *path, bool followLinks)
{
    BSLS_ASSERT(path);

    struct stat fileStats;

    StatFuncType statFunc = followLinks ? stat : lstat;

    if (0 != statFunc(path, &fileStats)) {
        return false;                                                 // RETURN
    }

    return S_ISDIR(fileStats.st_mode);
}

int bdesu_FileUtil::getLastModificationTime(bdet_Datetime *time,
                                            const char    *path)
{
    BSLS_ASSERT(time);
    BSLS_ASSERT(path);

    struct stat fileStats;

    if (0 != stat(path, &fileStats)) {
        return -1;                                                    // RETURN
    }

    *time = bdetu_Epoch::epoch();
    time->addSeconds(fileStats.st_mtime);
    return 0;
}

void bdesu_FileUtil::visitPaths(
                          const char                                  *pattern,
                          const bdef_Function<void(*)(const char *)>&  visitor)
{
    BSLS_ASSERT(pattern);

    glob_t pglob;

    int rc = glob(pattern, GLOB_NOSORT, 0, &pglob);
    bdema_ManagedPtr<glob_t> globGuard(&pglob, 0, &invokeGlobFree);
    if (GLOB_NOMATCH == rc) {
        return;                                                       // RETURN
    }
    if (GLOB_NOSPACE == rc) {
        bslma::Allocator::throwBadAlloc();
    }

    for (int i = 0; i < static_cast<int>(pglob.gl_pathc); ++i) {
        BSLS_ASSERT(pglob.gl_pathv[i]);
        if (!isDotOrDots(pglob.gl_pathv[i])) {
            visitor(pglob.gl_pathv[i]);
        }
    }
}

bdesu_FileUtil::Offset bdesu_FileUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
    struct statvfs buf;
    int rc = statvfs(path, &buf);
#else
    struct statvfs64 buf;
    int rc = statvfs64(path, &buf);
#endif
    if (rc) {
        return -1;                                                    // RETURN
    } else {
        return buf.f_bavail * buf.f_frsize;                           // RETURN
    }
}

bdesu_FileUtil::Offset bdesu_FileUtil::getAvailableSpace(FileDescriptor fd)
{
#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN)
    struct statvfs buf;
    int rc = fstatvfs(fd, &buf);
#else
    struct statvfs64 buf;
    int rc = fstatvfs64(fd, &buf);
#endif
    if (rc) {
        return -1;                                                    // RETURN
    } else {
        return buf.f_bavail * buf.f_frsize;                           // RETURN
    }
}

bdesu_FileUtil::Offset bdesu_FileUtil::getFileSize(const char *path)
{
    struct stat64 fileStats;

    if (0 != stat64(path, &fileStats)) {
        return -1;                                                    // RETURN
    }

    return fileStats.st_size;
}

bdesu_FileUtil::Offset bdesu_FileUtil::getFileSizeLimit()
{
#if defined(BSLS_PLATFORM__OS_FREEBSD) || defined(BSLS_PLATFORM__OS_DARWIN) \
 || defined(BSLS_PLATFORM__OS_HPUX)
    struct rlimit rl, rlMax, rlInf;
    int rc = getrlimit(RLIMIT_FSIZE, &rl);
#else
    struct rlimit64 rl, rlMax, rlInf;
    int rc = getrlimit64(RLIMIT_FSIZE, &rl);
#endif

    // Often, 'rl.rlim_cur' is an unsigned 64 bit, while 'Offset' is signed,
    // so 'rl.rlim_cur' may have a larger value than can be represented by
    // an 'Offset'.

    rlMax.rlim_cur = OFFSET_MAX;
    rlInf.rlim_cur = RLIM_INFINITY;

    if (rc) {
        return -1;                                                    // RETURN
    } else if (rl.rlim_cur == rlInf.rlim_cur || rl.rlim_cur > rlMax.rlim_cur) {
        return OFFSET_MAX;                                            // RETURN
    } else {
        return rl.rlim_cur;                                           // RETURN
    }
}

#endif  // non-Windows (POSIX)

/////////////////////////////////////
// NON-PLATFORM-SPECIFIC FUNCTIONS //
/////////////////////////////////////

int bdesu_FileUtil::createDirectories(const char *nativePath,
                                      bool        leafIsDirectory)
{
    // Implementation note: some Unix platforms may have mkdirp, which does
    // what this function does.  But not all do, and hyper-fast performance is
    // not a concern for a method like this, so let's just roll our own to
    // ensure maximum portability.
    //
    // Not to mention that we have to do at least a little parsing anyway,
    // since even mkdirp does not provide anything like 'leafIsDirectory'.

    // Let's first give at least a nod to efficiency and see if we don't need
    // to do anything at all.

    BSLS_ASSERT(nativePath);

    if (exists(nativePath)) {
        return 0;                                                     // RETURN
    }

    bsl::string path(nativePath);
    bsl::vector<bsl::string> directoryStack;
    if (!leafIsDirectory && bdesu_PathUtil::hasLeaf(path)) {
        bdesu_PathUtil::popLeaf(&path);
    }

    while(bdesu_PathUtil::hasLeaf(path)) {
        directoryStack.push_back(bsl::string());
        int rc = bdesu_PathUtil::getLeaf(&directoryStack.back(), path);
        BSLS_ASSERT(0 == rc);
        bdesu_PathUtil::popLeaf(&path);
    }

    while (!directoryStack.empty()) {
        bdesu_PathUtil::appendRaw(&path,
                                  directoryStack.back().c_str(),
                                  static_cast<int>(
                                      directoryStack.back().length()));
        if (!exists(path.c_str())) {
            if (0 != makeDirectory(path.c_str())) {
                return -1;                                            // RETURN
            }
        }
        directoryStack.pop_back();
    }
    return 0;
}

int bdesu_FileUtil::getWorkingDirectory(bsl::string *path)
{
    BSLS_ASSERT(path);

    enum {BUFFER_SIZE = 4096};
    char buffer[BUFFER_SIZE];

    // Since we '#define'ed getcwd=_getcwd above on Windows, we
    // can just implement this in terms of getcwd for either
    // type of platform.

    char *retval = getcwd(buffer, BUFFER_SIZE);
    if (retval == buffer) {
        (*path) = buffer;

        //our contract requires an absolute path

        return bdesu_PathUtil::isRelative(*path);                     // RETURN
    }
    return -1;
}

int bdesu_FileUtil::setWorkingDirectory(const char *path)
{
    BSLS_ASSERT(path);

    // Since we '#define'ed chdir=_chdir above on Windows, we
    // can just implement this in terms of chdir for either
    // type of platform.

    return chdir(path);
}

void bdesu_FileUtil::findMatchingPaths(bsl::vector<bsl::string> *result,
                                       const char               *pattern)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(pattern);

    result->clear();
    visitPaths(pattern,
               bdef_BindUtil::bind(&pushBackWrapper,
                                   result, bdef_PlaceHolders::_1));
}

int bdesu_FileUtil::grow(FileDescriptor         fd,
                         bdesu_FileUtil::Offset size,
                         bool                   reserve,
                         bsl::size_t            bufferSize)
{
    bslma::Allocator *allocator_p = bslma::Default::defaultAllocator();
    Offset currentSize = seek(fd, 0, BDESU_SEEK_FROM_END);

    if (currentSize == -1) {
        return -1;                                                    // RETURN
    }
    if (currentSize >= size) {
        return 0;                                                     // RETURN
    }
    if (reserve) {
        char *buf = (char*)allocator_p->allocate(bufferSize);
        bsl::memset(buf, 1, bufferSize);
        Offset bytesToGrow = size - currentSize;
        while(bytesToGrow > 0) {
            int nBytes = static_cast<int>(
                                   bsl::min(bytesToGrow, (Offset)bufferSize));
            int rc = write(fd, buf, nBytes);
            if (rc != nBytes) {
                allocator_p->deallocate(buf);
                return -1;                                            // RETURN
            }
            bytesToGrow -= nBytes;
        }
        allocator_p->deallocate(buf);
        return 0;                                                     // RETURN
    }
    Offset res = seek(fd, size-1, BDESU_SEEK_FROM_BEGINNING);
    if (-1 == res || 1 != write(fd, (const void *)"", 1))
    {
        return -1;                                                    // RETURN
    }
    return 0;
}

int bdesu_FileUtil::rollFileChain(const char *path, int maxSuffix)
{
    BSLS_ASSERT(path);

    enum { MAX_SUFFIX_LENGTH = 10 };
    bslma::Allocator *allocator_p = bslma::Default::defaultAllocator();
    int length = static_cast<int>(bsl::strlen(path)) + MAX_SUFFIX_LENGTH + 2;

    // Use a single allocation to insure exception neutrality.

    char *buf      = (char*)allocator_p->allocate(2 * length);
    char *fromName = buf;
    char *toName   = buf + length;
    snprintf(toName, length, "%s.%d", path, maxSuffix);

    if (!exists(toName) || 0 == remove(toName, true)) {
        while (0 < --maxSuffix) {
            snprintf(fromName, length, "%s.%d", path, maxSuffix);

            if (exists(fromName) && 0 != move(fromName, toName)) {
                break;
            }
            bsl::swap(toName, fromName);
        }

        if (0 == maxSuffix && exists(path) && 0 != move(path, toName)) {
            maxSuffix = -1;
        }
    }
    allocator_p->deallocate(buf);
    return maxSuffix;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
