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

#include <bdls_filesystemutil_uniximputil.h>
#include <bdls_filesystemutil_unixplatform.h>
#include <bdls_filesystemutil_transitionaluniximputil.h>
#include <bdls_filesystemutil_windowsimputil.h>
#include <bdls_memoryutil.h>
#include <bdls_pathutil.h>

#include <bdlt_timeunitratio.h>

#include <bdlf_bind.h>
#include <bdlt_epochutil.h>
#include <bdlf_placeholder.h>
#include <bdlt_currenttime.h> // for testing only
#include <bdlma_bufferedsequentialallocator.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslmf_movableref.h>
#include <bslmt_threadutil.h>

#include <bsla_maybeunused.h>
#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>
#include <bslh_hash.h>
#include <bsls_timeutil.h>

#include <bsl_algorithm.h>
#include <bsl_c_stdio.h> // needed for rename on AIX & snprintf everywhere
#include <bsl_cstddef.h>
#include <bsl_cstring.h> // for memcpy
#include <bsl_limits.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# include <windows.h>
# include <winioctl.h>
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

// This definitions are extracted from ntifs.h file that is a part of Windows
// Driver Kit (WDK).  Can be replaced with '#include <ntifs.h>' if we add WDK
// as a required dependency for BDE on Windows.
# ifndef REPARSE_DATA_BUFFER_HEADER_SIZE
typedef struct _REPARSE_DATA_BUFFER
{
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union
    {
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG Flags;
            WCHAR PathBuffer[1];
        }
        SymbolicLinkReparseBuffer;
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        }
        MountPointReparseBuffer;
        struct
        {
            UCHAR  DataBuffer[1];
        }
        GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
# define REPARSE_DATA_BUFFER_HEADER_SIZE \
    FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)
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

// MACROS
#if defined(BSLS_PLATFORM_OS_UNIX)                           \
 && (   defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF) \
     || defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF))
    // If the current platform is a Unix, and either has a 64-bit 'off_t' type,
    // or a 32-bit 'off_t' type and no 'off64_t' type, use the standard Unix
    // file-system functions and types.
# define U_USE_UNIX_FILE_SYSTEM_INTERFACE

#elif defined(BSLS_PLATFORM_OS_UNIX) \
   && defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)
    // Otherwise, if the current platform is a Unix and has a 64-bit 'off64_t'
    // type and 'xxx64'-suffixed functions, use these non-standard,
    // 'xxx64'-suffixed, file-system functions and types where appropriate.
    // Note that these functions are deprecated in many modern Unixes, but are
    // nevertheless required for 64-bit file-offset support in some 32-bit
    // build modes on some Unixes.
# define U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE

#elif defined(BSLS_PLATFORM_OS_UNIX)
    // Otherwise, 'bdls_filesystemutil' supports no other Unix.  Note that some
    // non-Unix systems, like Windows, are supported.  Further, note that
    // 'bdls_filesystemutil' does not consider Windows a Unix even though its
    // Universal C Runtime layer supports a subset of the POSIX interface.
# error "'bdls_filesystemutil' does not support this platform."
#endif

// PRIVATE CONSTANTS
enum {
    k_UNKNOWN_ERROR = 127
};

// STATIC HELPER FUNCTIONS
namespace BloombergLP {
namespace {
namespace u {

#if defined(BSLS_PLATFORM_OS_UNIX) \
 && defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)

                          // ========================
                          // struct UnixInterfaceUtil
                          // ========================

struct UnixInterfaceUtil {
    // This component-private utility 'struct' provides an implementation of
    // the requirements for the 'UNIX_INTERFACE' template parameter of the
    // functions provided by 'FilesystemUtil_UnixImplUtil' in terms of actual
    // Unix interface calls.  See also the notes regarding the 'stat' structure
    // in the documentation for 'struct FilesystemUtil_UnixImpUtil'.

    // TYPES
    typedef ::off_t off_t;
        // 'off_t' is an alias to the 'off_t' type provided by the
        // 'sys/types.h' header.  It is a signed integral type used to
        // represent quantities of bytes.  Note that, depending on the build
        // configuration, this type may have 32 or 64 bits.

    typedef struct ::stat stat;
        // 'stat' is an alias to the 'stat' 'struct' provided by the
        // 'sys/stat.h' header.

    typedef ::time_t time_t;
        // 'time_t' is an alias to the 'time_t' type provided by the
        // 'sys/types.h' header.  It represents a time point as number of
        // seconds since January 1st 1970 in Coordinated Universal Time.

    // CLASS METHODS
    static long get_st_mtim_nsec(const stat& stat);
        // Return the value of the 'st_mtim.nsec' field of the specified 'stat'
        // struct.

    static time_t get_st_mtime(const stat& stat);
        // Return the value of the 'st_mtime' data member of the specified
        // 'stat' struct.

    static off_t get_st_size(const stat& stat);
        // Return the value of the 'st_size' data member of the specified
        // 'stat' struct.  Note that this function is provided in order to
        // create a consistent interface for accessing the data members of a
        // 'stat' struct with 'get_st_mtime'.

    static int fstat(int fildes, stat *buf);
        // Invoke and return the result of '::fstat(fildes, buf)' with the
        // specified  'fildes' and 'buf', where '::fstat' is the function
        // provided by the 'sys/stat.h' header.
};

                             // ==================
                             // struct UnixImpUtil
                             // ==================

typedef bdls::FilesystemUtil_UnixImpUtil<UnixInterfaceUtil> UnixImpUtil;
    // 'UnixImpUtil' is an alias to an utility 'struct' that provides the
    // implementations of some of 'bdls::FilesystemUtil's functions for Unix
    // systems.  Note that this 'struct' is a specialization of a utility class
    // template that defines some file-system operations in terms of a
    // parameterized Unix-interface utility.  This alias instantiates that
    // template with a 'struct' that provides actual Unix interface calls.  The
    // utility class template parameterizes its Unix interface in order to
    // permit tests to instantiate the template with mock Unix interfaces.

                     // =================================
                     // typedef InterfaceUtil and ImpUtil
                     // =================================

typedef UnixInterfaceUtil InterfaceUtil;
typedef UnixImpUtil       ImpUtil;

#elif defined(BSLS_PLATFORM_OS_UNIX) \
   && defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)

                    // ====================================
                    // struct TransitionalUnixInterfaceUtil
                    // ====================================

struct TransitionalUnixInterfaceUtil {
    // This component-private utility 'struct' provides an implementation of
    // the requirements for the 'UNIX_INTERFACE' template parameter of the
    // functions provided by 'FilesystemUtil_TransitionalUnixImpUtil' in terms
    // of actual transitional Unix interface calls.  See also the notes
    // regarding the 'stat' structure in the documentation for
    // 'struct FilesystemUtil_TransitionalUnixImpUtil'.

    // TYPES
    typedef ::off64_t off64_t;
        // 'off_t' is an alias to the 'off_t' type provided by the
        // 'sys/types.h' header.  It is a signed integral type used to
        // represent quantities of bytes.

    typedef struct stat64 stat64;
        // 'stat64' is an alias to the 'stat64' 'struct' provided by the
        // 'sys/stat.h' header.

    typedef ::time_t time_t;
        // 'time_t' is an alias to the 'time_t' type provided by the
        // 'sys/types.h' header.  It represents a time point as number of
        // seconds since January 1st 1970 in Coordinated Universal Time.

    // CLASS METHODS
    static long get_st_mtim_nsec(const stat64& stat);
        // Return the value of the 'st_mtim.nsec' field of the specified 'stat'
        // struct.

    static time_t get_st_mtime(const stat64& stat);
        // Return the value of the 'st_mtime' data member of the specified
        // 'stat' struct.

    static off64_t get_st_size(const stat64& stat);
        // Return the value of the 'st_size' data member of the specified
        // 'stat' struct.  Note that this function is provided in order to
        // create a consistent interface for accessing the data members of a
        // 'stat' struct with 'get_st_mtime'.

    static int fstat64(int fildes, stat64 *buf);
        // Invoke and return the result of '::fstat64(fildes, buf)' with the
        // specified 'fildes' and 'buf', where '::fstat64' is the function
        // provided by the 'sys/stat.h' header.
};

                       // ==============================
                       // struct TransitionalUnixImpUtil
                       // ==============================

typedef bdls::FilesystemUtil_TransitionalUnixImpUtil<
    TransitionalUnixInterfaceUtil>
    TransitionalUnixImpUtil;
    // 'TransitionalUnixImpUtil' is an alias to an utility 'struct' that
    // provides the implementations of some of 'bdls::FilesystemUtil's
    // functions for transitional Unix systems.  Note that this 'struct' is a
    // specialization of a utility class template that defines some file-system
    // operations in terms of a parameterized Unix-interface utility.  This
    // alias instantiates that template with a 'struct' that provides actual
    // Unix interface calls.  The utility class template parameterizes its Unix
    // interface in order to permit tests to instantiate the template with mock
    // Unix interfaces.

                     // =================================
                     // typedef InterfaceUtil and ImpUtil
                     // =================================

typedef TransitionalUnixInterfaceUtil InterfaceUtil;
typedef TransitionalUnixImpUtil       ImpUtil;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

                        // ===========================
                        // struct WindowsInterfaceUtil
                        // ===========================

struct WindowsInterfaceUtil {
    // This component-private utility 'struct' provides an implementation of
    // the requirements for the 'WINDOWS_INTERFACE' template parameter of the
    // functions provided by 'FilesystemUtil_WindowsImpUtil' in terms of
    // actual Windows interface calls.

    // TYPES
    typedef ::BOOL BOOL;
        // 'BOOL' is an alias to the unsigned integral 'BOOL' type provided
        // by the 'windows.h' header.

    typedef ::DWORD DWORD;
        // 'DWORD' is an alias to the unsigned integral 'DWORD' type provided
        // by the 'windows.h' header.

    typedef ::FILETIME FILETIME;
        // 'FILETIME' is an alias to the 'FILETIME' struct provided by the
        // 'windows.h' header.

    typedef ::HANDLE HANDLE;
        // 'HANDLE' is an alias to the 'HANDLE' type provided by the
        // 'windows.h' header.

    typedef ::INT64 INT64;
        // 'INT64' is an alias to the signed integral 'INT64' type provided by
        // the 'windows.h' header.

    typedef ::LPDWORD LPDWORD;
        // 'LPDWORD' is an alias to the 'LPDWORD' type provided by the
        // 'windows.h' header.

    typedef ::LPFILETIME LPFILETIME;
        // 'LPFILETIME' is an alias to the 'LPFILETIME' type provided by the
        // 'windows.h' header.

    typedef ::LPSYSTEMTIME LPSYSTEMTIME;
        // 'LPSYSTEMTIME' is an alias to the 'LPSYSTEMTIME' type provided by
        // the 'windows.h' header.

    typedef ::SYSTEMTIME SYSTEMTIME;
        // 'SYSTEMTIME' is an alias to the 'SYSTEMTIME' struct provided by the
        // 'windows.h' header.

    typedef ::ULARGE_INTEGER ULARGE_INTEGER;
        // 'ULARGE_INTEGER' is an alias to the unsigned integral
        // 'ULARGE_INTEGER' type provided by the 'windows.h' header.

    typedef ::ULONG64 ULONG64;
        // 'ULONG64' is an alias to the unsigned integral 'ULONG64' type
        // provided by the 'windows.h' header.

    typedef ::ULONGLONG ULONGLONG;
        // 'ULONGLONG' is an alias to the unsigned integral 'ULONGLONG' type
        // provided by the 'windows.h' header.

    typedef ::WORD WORD;
        // 'WORD' is an alias to the unsigned integral 'WORD' type provided
        // by the 'windows.h' header.

    // CLASS METHODS
    static BOOL FileTimeToSystemTime(const FILETIME *lpFileTime,
                                     LPSYSTEMTIME    lpSystemTime);
        // Invoke and return the result of
        // '::FileTimeToSystemTime(lpFileTime, lpSystemTime)' with the
        // specified 'lpFileTime' and 'lpSystemTime', where
        // '::FileTimeToSystemTime' is the function provided by the 'windows.h'
        // header.

    static DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
        // Invoke and return the result of
        // '::GetFileSize(hFile, lpFileSizeHigh)' with the specified 'hFile'
        // and 'lpFileSizeHigh', where '::GetFileSize' is the function provided
        // by the 'windows.h' header.

    static BOOL GetFileTime(HANDLE     hFile,
                            LPFILETIME lpCreationTime,
                            LPFILETIME lpLastAccessTime,
                            LPFILETIME lpLastWriteTime);
        // Invoke and return the result of '::GetFileTime(hFile,
        // lpCreationTime, lpLastAccessTime, lpLastWriteTime)' with the
        // specified 'hFile', 'lpCreationTime', 'lpLasAccessTime', and
        // 'lpLastWriteTime', where '::GetFileTime' is the function provided by
        // the 'windows.h' header.

    static DWORD GetLastError();
        // Invoke and return the result of '::GetLastError()', where
        // '::GetLastError' is the function provided by the 'windows.h' header.

    static BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime,
                                     LPFILETIME        lpFileTime);
        // Invoke and return the result of
        // '::SystemTimeToFileTime(lpSystemTime, lpFileTime)' with the
        // specified 'lpFileTime' and 'lpSystemTime', where
        // '::SystemTimeToFileTime' is the function provided by the 'windows.h'
        // header.
};

                           // =====================
                           // struct WindowsImpUtil
                           // =====================

typedef bdls::FilesystemUtil_WindowsImpUtil<WindowsInterfaceUtil>
    WindowsImpUtil;
    // 'WindowsImpUtil' is an alias to an utility 'struct' that provides the
    // implementations of some of 'bdls::FilesystemUtil's functions for Windows
    // systems.  Note that this 'struct' is a specialization of a utility class
    // template that defines some file-system operations in terms of a
    // parameterized Windows-interface utility.  This alias instantiates that
    // template with a 'struct' that provides actual Windows interface calls.
    // The utility class template parameterizes its Windows interface in order
    // to permit tests to instantiate the template with mock Windows
    // interfaces.


                     // =================================
                     // typedef InterfaceUtil and ImpUtil
                     // =================================

typedef WindowsInterfaceUtil InterfaceUtil;
typedef WindowsImpUtil       ImpUtil;

#else
# error "'bdls_filesystemutil' does not support this platform."
#endif

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

namespace {

                               // ==============
                               // struct NameRec
                               // ==============

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
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
namespace {
    typedef struct ::stat   StatResult;
}  // close unnamed namespace
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
namespace {
    typedef struct ::stat64 StatResult;
}  // close unnamed namespace
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif

static inline
int performStat(const char *fileName, StatResult *statResult)
    // Run the appropriate 'stat' or 'stat64' function on the specified
    // 'fileName', returning the results in the specified 'statResult'.
{
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    return ::stat  (fileName, statResult);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    return ::stat64(fileName, statResult);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
}

static inline
int performStat(const char *fileName, StatResult *statResult, bool followLinks)
    // Run the appropriate 'stat' or 'stat64' function on the specified
    // 'fileName', returning the results in the specified 'statResult', where
    // the specified 'followLinks' indicates whether symlinks are to be
    // followed.
{
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    return followLinks ?  ::stat(fileName, statResult)
                       : ::lstat(fileName, statResult);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    return followLinks ?  ::stat64(fileName, statResult)
                       : ::lstat64(fileName, statResult);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
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

# ifdef BSLS_PLATFORM_OS_SOLARIS

    // The Solaris 'glob' function sometimes tries to treat files as
    // directories, which normally aborts the traversal.  So on Solaris we
    // treat 'ENOTDIR' as an error that can be safely and completely ignored
    // (not even reported).

    if (ENOTDIR == errorNum) {
        return 0;                                                     // RETURN
    }
# endif

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

namespace {
template <class VECTOR_TYPE>
void pushBackWrapper(VECTOR_TYPE *vector, const char *item)
    // A 'thunk' to be bound to the specified 'vector' that can be called to
    // push the specified 'item' to the 'vector'.
{
    BSLS_ASSERT(vector);

    vector->push_back(item);
}
}  // close unnamed namespace

#ifdef BSLS_PLATFORM_OS_WINDOWS
static inline
void invokeFindClose(void *handle, void *)
    // Provides a function signature which can be used as a 'bslma::ManagedPtr'
    // deleter (i.e., we need to define a second argument of type 'void *',
    // which will be ignored, and the first parameter must be 'void *' which is
    // cast to correct type inside the function).
{
    BSLS_ASSERT(handle);

    FindClose(*static_cast<HANDLE *>(handle));
}

static inline
bool narrowToWide(bsl::wstring *result, const bsl::string& path)
    // Copy the UTF-16 version of the specified 'path' to the specified
    // 'result' wide string and return 'true' if successful, or 'false'
    // otherwise.  If conversion is unsuccessful, the contents of 'result' are
    // undefined.
{
    BSLS_ASSERT(result);

    int rc = bdlde::CharConvertUtf16::utf8ToUtf16(result, path.data());

    return 0 == rc;
}

namespace {
template <class STRING_TYPE>
inline
bool wideToNarrow(STRING_TYPE *result, const bsl::wstring_view& path)
    // Copy the UTF-8 version of the specified 'path' to the specified 'result'
    // narrow string and return 'true' if successful, or 'false' otherwise.  If
    // conversion is unsuccessful, the contents of 'result' are undefined.
{
    BSLS_ASSERT(result);

    return 0 == bdlde::CharConvertUtf16::utf16ToUtf8(result, path);
}
}  // close unnamed namespace

static inline
int makeDirectory(const char *path, bool)
    // Create a directory.  Return 0 on success, 'k_ERROR_PATH_NOT_FOUND' if a
    // component used as a directory in the specified 'path' either does not
    // exist or is not a directory, 'k_ERROR_ALREADY_EXISTS' if the file system
    // entry (not necessarily a directory) with the name 'path' already exists,
    // and a negative value for any other kind of error.
{
    BSLS_ASSERT(path);

    bsl::wstring wide;

    if (narrowToWide(&wide, path)) {
        if (CreateDirectoryW(wide.c_str(), 0)) {
            return 0;                                                 // RETURN
        }

        switch (GetLastError()) {
          case ERROR_ALREADY_EXISTS: {
            return bdls::FilesystemUtil::k_ERROR_ALREADY_EXISTS;      // RETURN
          } break;
          case ERROR_PATH_NOT_FOUND: {
            return bdls::FilesystemUtil::k_ERROR_PATH_NOT_FOUND;      // RETURN
          } break;
        }
    }

    return -1;
}

static inline
int removeDirectory(const char *path)
    // Remove directory 'path'.  Return 0 on success and a non-zero value
    // otherwise.
{
    BSLS_ASSERT(path);

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
    BSLS_ASSERT(path);

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
    BSLS_ASSERT(pglob);

    globfree(static_cast<glob_t *>(pglob));
}

static inline
void invokeCloseDir(void *dir, void *)
    // Close the specified directory, 'dir'.  Note that this function has a
    // signature appropriate for use as a managed pointer deleter, hence the
    // parameters are both passed as 'void *'.
{
    BSLS_ASSERT(dir);

    // Note also that 'closedir' was observed sometimes returning a non-zero
    // value for no particular reason.  Hopefully we'll capture 'errno' in the
    // nightly build.

    int rc = closedir(static_cast<DIR *>(dir));
    int myErrno = errno;        // 'errno' is a function, get the value into
                                // a veriable where we can see it when the
                                // assert fails and we debug the core file.
    (void) rc;    (void) myErrno;

    BSLS_ASSERT(0 == rc);
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
    BSLS_ASSERT(length >= 1);

    if ('.' != end[-1]) {
        return false;                                                 // RETURN
    }

    if (1 == length) {
        return true;                                                  // RETURN
    }
    BSLS_ASSERT(length >= 2);

    if ('/' == end[-2]) {
        return true;                                                  // RETURN
    }
    if ('.' != end[-2]) {
        return false;                                                 // RETURN
    }

    if (2 == length) {
        return true;                                                  // RETURN
    }
    BSLS_ASSERT(length >= 3);

    return '/' == end[-3];
}

static inline
int makeDirectory(const char *path, bool isPrivate)
    // Create a directory.  Return 0 on success, 'k_ERROR_PATH_NOT_FOUND' if a
    // component used as a directory in the specified 'path' either does not
    // exist or is not a directory, 'k_ERROR_ALREADY_EXISTS' if the file system
    // entry (not necessarily a directory) with the name 'path' already exists,
    // and a negative value for any other kind of error.
{
    BSLS_ASSERT(path);

    // Permissions of created dir will be ANDed with process '~umask()'.
    static const mode_t PERMS[2] = {
        (S_IRUSR | S_IWUSR | S_IXUSR |  // user   rwx
         S_IRGRP | S_IWGRP | S_IXGRP |  // group  rwx
         S_IROTH | S_IWOTH | S_IXOTH),  // others rwx

        (S_IRUSR | S_IWUSR | S_IXUSR)  // only user rwx
    };
    if (mkdir(path, PERMS[isPrivate])) {
        switch (errno) {
          case EEXIST:
            return bdls::FilesystemUtil::k_ERROR_ALREADY_EXISTS;      // RETURN
          case ENOTDIR:
          case ENOENT:
            return bdls::FilesystemUtil::k_ERROR_PATH_NOT_FOUND;      // RETURN
          default:
            return -1;                                                // RETURN
        }
    }
    else {
        return 0;                                                     // RETURN
    }
}

static inline
int removeDirectory(const char *path)
    // Remove the specified directory 'path'.  Return 0 on success and a
    // non-zero value otherwise.
{
    BSLS_ASSERT(path);

    return rmdir(path);
}

static inline
int removeFile(const char *path)
{
    BSLS_ASSERT(path);

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

    bsl::wstring                   wide;
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
    if (k_INVALID_FD == descriptor) {
        return int(k_BAD_FILE_DESCRIPTOR);                            // RETURN
    }

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

int FilesystemUtil::truncateFileSize(FileDescriptor descriptor, Offset size)
{
    BSLS_ASSERT(size <= getFileSize(descriptor));

    const Offset pos = seek(descriptor, size, e_SEEK_FROM_BEGINNING);
    if (pos != size) {
        return -1;                                                    // RETURN
    }

    bool rc = ::SetEndOfFile(descriptor);
    if (!rc) {
        return -1;                                                    // RETURN
    }

    return 0;
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

    if (narrowToWide(&oldWide, oldName) &&
        narrowToWide(&newWide, newName)) {
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

    typedef FilesystemUtil_WindowsImpUtil<u::WindowsInterfaceUtil>
        WindowsImpUtil;

    return WindowsImpUtil::convertFileTimeToDatetime(time, &modified);

}

int FilesystemUtil::getLastModificationTime(bdlt::Datetime *time,
                                            FileDescriptor  descriptor)
{
    BSLS_ASSERT(time);

    typedef FilesystemUtil_WindowsImpUtil<u::WindowsInterfaceUtil>
        WindowsImpUtil;

    return WindowsImpUtil::getLastModificationTime(time, descriptor);
}

int FilesystemUtil::visitPaths(
                             const char                               *pattern,
                             const bsl::function<void(const char *)>&  visitor)
{
    // Windows 'FindFirstFileW' function only looks at pattern characters ('*'
    // and '?') in the leaf element of the path.  Therefore we must first
    // resolve all special characters in the non-leaf portion of the path
    // recursively before resolving the leaf portion.  Also, unlike glob(), the
    // name returned by 'FindFirstFileW' is *just* the leaf.  Resolving the
    // handle into a full path is not available pre-Vista.

    int numFiles = 0;   // Count # of files visited to be returned if
                        // successful.

    bsl::string dirName;
    bsl::string_view patternSV(pattern);
    if (0 != PathUtil::getDirname(&dirName, patternSV)) {
        // There is no leaf, therefore there can be nothing to do (but not an
        // error).  Return # of files found, which is 0.

        return 0;                                                     // RETURN
    }

    if (bsl::string::npos != dirName.find_first_of("*?")) {
        bsl::vector<bsl::string> leaves;
        bsl::vector<bsl::string> paths, workingPaths;
        bsl::string pattern(patternSV);
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
                    int rc = visitPaths(
                        it->c_str(),
                        bdlf::BindUtil::bind(
                            &pushBackWrapper<bsl::vector<bsl::string> >,
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

        if (!narrowToWide(&widePattern, pattern)) {
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

namespace {
template <class STRING_TYPE>
int u_getSystemTemporaryDirectory(STRING_TYPE *path)
{
    WCHAR wpath[MAX_PATH + 1];
    const DWORD getTempPathStatus = ::GetTempPathW(MAX_PATH, wpath);
    if (0 == getTempPathStatus) {
        return -1;                                                    // RETURN
    }

    STRING_TYPE result;
    int rc = bdlde::CharConvertUtf16::utf16ToUtf8(&result, wpath);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    *path = bslmf::MovableRefUtil::move(result);
    return 0;
}
}  // close unnamed namespace

int FilesystemUtil::visitTree(
                         const bsl::string_view&                      root,
                         const bsl::string_view&                      pattern,
                         const bsl::function<void(const char *path)>& visitor,
                         bool                                         sortFlag)
{
    bsl::string rootDir;
    rootDir.reserve(root.length() + 1);
    rootDir = root;
    if (!isDirectory(rootDir)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(!rootDir.empty());    // 'isDirectory' would have been
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

    (void)bdlde::CharConvertUtf16::utf8ToUtf16(
        &widePattern, fullPattern, 0, '-');
    FileDescriptor handle = FindFirstFileExW(widePattern.c_str(),
                                             FindExInfoStandard,
                                             &foundData,
                                             FindExSearchNameMatch,
                                             NULL,
                                             FIND_FIRST_EX_CASE_SENSITIVE);
    if (INVALID_HANDLE_VALUE != handle) {
        bslma::ManagedPtr<FileDescriptor> handleGuard(
            &handle, 0, &invokeFindClose);
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
    (void)bdlde::CharConvertUtf16::utf8ToUtf16(
        &widePattern, fullPattern, 0, '-');
    handle = FindFirstFileExW(widePattern.c_str(),
                              FindExInfoStandard,
                              &foundData,
                              FindExSearchLimitToDirectories,
                              NULL,
                              FIND_FIRST_EX_CASE_SENSITIVE);
    if (INVALID_HANDLE_VALUE != handle) {
        bslma::ManagedPtr<FileDescriptor> handleGuard(
            &handle, 0, &invokeFindClose);
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

            BSLS_ASSERT(0 == rc);
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

bool FilesystemUtil::isSymbolicLink(const char *path)
{
    BSLS_ASSERT(path);

    bsl::wstring wide;
    if (!narrowToWide(&wide, path)) {
        return false;                                                 // RETURN
    }

    DWORD stats = GetFileAttributesW(wide.c_str());

    return (INVALID_FILE_ATTRIBUTES != stats &&
            0 != (stats & FILE_ATTRIBUTE_REPARSE_POINT));
}

FilesystemUtil::Offset FilesystemUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

    ULARGE_INTEGER avail;
    bsl::wstring   wide;

    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    if (!GetDiskFreeSpaceExW(wide.c_str(), &avail, NULL, NULL)) {
        return -2;                                                    // RETURN
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
                         HANDLE                          FileHandle,
                         IO_STATUS_BLOCK                *IoStatusBlock,
                         PVOID                           FileInformation,
                         ULONG                           Length,
                         INT                             FileInformationClass);

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

FilesystemUtil::Offset FilesystemUtil::getFileSize(FileDescriptor descriptor)
{
    return u::WindowsImpUtil::getFileSize(descriptor);
}

FilesystemUtil::Offset FilesystemUtil::getFileSizeLimit()
{
    // TBD

    return k_OFFSET_MAX;
}

namespace {
template <class STRING_TYPE>
int u_getSymbolicLinkTarget(STRING_TYPE *result,
                            const char  *path)
{
    BSLS_ASSERT(path);
    BSLS_ASSERT(result);

    bsl::wstring wide;
    if (!narrowToWide(&wide, path)) {
        return -1;                                                    // RETURN
    }

    //char memory[MAXIMUM_REPARSE_DATA_BUFFER_SIZE]; //can be large so use heap
    bsl::string memory(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, '\x0');
    REPARSE_DATA_BUFFER *rdb =
                         reinterpret_cast<REPARSE_DATA_BUFFER*>(memory.data());

    HANDLE hFile = CreateFileW(
        wide.c_str(),
        FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        return -1;                                                    // RETURN
    }

    DWORD bytesReturned;
    bool ok = DeviceIoControl(
        hFile,
        FSCTL_GET_REPARSE_POINT,
        0,
        0,
        rdb,
        MAXIMUM_REPARSE_DATA_BUFFER_SIZE,
        &bytesReturned,
        0
    );
    CloseHandle(hFile);
    if (!ok) {
        return -1;                                                    // RETURN
    }

    const wchar_t *name;
    size_t         nameLen;
    switch (rdb->ReparseTag) {
      case IO_REPARSE_TAG_SYMLINK: {  // symlink
        name =
              rdb->SymbolicLinkReparseBuffer.PathBuffer +
              rdb->SymbolicLinkReparseBuffer.PrintNameOffset / sizeof(wchar_t);
        nameLen =
              rdb->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(wchar_t);
      } break;
      case IO_REPARSE_TAG_MOUNT_POINT: {  // directory junction
        name =  rdb->MountPointReparseBuffer.PathBuffer +
                rdb->MountPointReparseBuffer.PrintNameOffset / sizeof(wchar_t);
        nameLen =
                rdb->MountPointReparseBuffer.PrintNameLength / sizeof(wchar_t);
      } break;
      default: {
        return -1;                                                    // RETURN
      }
    }

    if(wideToNarrow(result, bsl::wstring_view(name, nameLen))) {
        return -1;                                                    // RETURN
    }
    return 0;
}
}  // close unnamed namespace

namespace {
template <class STRING_TYPE>
int u_getWorkingDirectory(STRING_TYPE *path)
{
    BSLS_ASSERT(path);

    enum { BUFFER_SIZE = 4096 };
    wchar_t buffer[BUFFER_SIZE];

    wchar_t *retval = _wgetcwd(buffer, BUFFER_SIZE);
    if (buffer == retval && wideToNarrow(path, buffer)) {
        //our contract requires an absolute path

        return PathUtil::isRelative(*path);                           // RETURN
    }
    return -1;
}
}  // close unnamed namespace

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
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
        return ::open(path, oflag, extendedFlags);                    // RETURN
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
        return ::open64(path, oflag, extendedFlags);                  // RETURN
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
    }
    else {
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
        return ::open(path, oflag);                                   // RETURN
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
        return ::open64(path, oflag);                                 // RETURN
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
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
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
      case e_SEEK_FROM_BEGINNING: {
        return ::lseek(descriptor, offset, SEEK_SET);                 // RETURN
      }
      case e_SEEK_FROM_CURRENT: {
        return ::lseek(descriptor, offset, SEEK_CUR);                 // RETURN
      }
      case e_SEEK_FROM_END: {
        return ::lseek(descriptor, offset, SEEK_END);                 // RETURN
      }
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
      case e_SEEK_FROM_BEGINNING: {
        return ::lseek64(descriptor, offset, SEEK_SET);               // RETURN
      }
      case e_SEEK_FROM_CURRENT: {
        return ::lseek64(descriptor, offset, SEEK_CUR);               // RETURN
      }
      case e_SEEK_FROM_END: {
        return ::lseek64(descriptor, offset, SEEK_END);               // RETURN
      }
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
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

# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    *address = ::mmap(0, size, protect, MAP_SHARED, descriptor, offset);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    *address = ::mmap64(0, size, protect, MAP_SHARED, descriptor, offset);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif

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

int FilesystemUtil::truncateFileSize(FileDescriptor descriptor, Offset size)
{
    BSLS_ASSERT(size <= getFileSize(descriptor));

#if defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    int rc = ::ftruncate64(descriptor, size);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }
#else
    int rc = ::ftruncate(descriptor, size);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }
#endif

    const Offset pos = seek(descriptor, 0, e_SEEK_FROM_END);
    if (pos != size) {
        return -1;                                                    // RETURN
    }

    return 0;
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

bool FilesystemUtil::isSymbolicLink(const char *path)
{
    BSLS_ASSERT(path);

    StatResult fileStats;

    if (0 != ::performStat(path, &fileStats, false)) {
        return false;                                                 // RETURN
    }

    return S_ISLNK(fileStats.st_mode);
}

int FilesystemUtil::getLastModificationTime(bdlt::Datetime *time,
                                            const char     *path)
{
    BSLS_ASSERT(time);
    BSLS_ASSERT(path);

    StatResult fileStats;

    int rc = ::performStat(path, &fileStats);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    *time = bdlt::EpochUtil::epoch();

    u::InterfaceUtil::time_t seconds =
                                     u::InterfaceUtil::get_st_mtime(fileStats);

    long nanoseconds = u::InterfaceUtil::get_st_mtim_nsec(fileStats);

    rc = time->addSecondsIfValid(seconds);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT_SAFE((0 <= nanoseconds) &&
                     (nanoseconds <
                      bdlt::TimeUnitRatio::k_NANOSECONDS_PER_SECOND));
    rc = time->addMicrosecondsIfValid(
          nanoseconds / bdlt::TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND_32);

    return rc;
}

int FilesystemUtil::getLastModificationTime(bdlt::Datetime *time,
                                            FileDescriptor  descriptor)
{
    BSLS_ASSERT(time);

    return u::ImpUtil::getLastModificationTime(time, descriptor);
}

namespace {
template <class STRING_TYPE>
int u_getSystemTemporaryDirectory(STRING_TYPE *path)
{
    // 'TMPDIR' is not chosen arbitrarily, it is an environment variable that
    // all versions of the IEEE Std 1003.1 (POSIX) specification mandate exist
    // and contain a path-name of a directory made available for programs that
    // need a place to create temporary files.

    const char *const tmpdir = ::getenv("TMPDIR");
    if (0 == tmpdir) {
        return -1;                                                    // RETURN
    }

    *path = tmpdir;
    return 0;
}
}  // close unnamed namespace

int FilesystemUtil::visitPaths(
                             const char                               *pattern,
                             const bsl::function<void(const char *)>&  visitor)
{
    glob_t pglob;

    int rc = glob(
        pattern, GLOB_NOSORT, isNotFilePermissionsError_p, &pglob);
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
                         const bsl::string_view&                      root,
                         const bsl::string_view&                      pattern,
                         const bsl::function<void(const char *path)>& visitor,
                         bool                                         sortFlag)
{
    bsl::string rootDir;
    rootDir.reserve(root.length() + 1);
    rootDir = root;
    if (!isDirectory(rootDir)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(!rootDir.empty());    // 'isDirectory' would have been
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
            BSLS_ASSERT(fullPath);
            const char *basename = fullPath + truncTo;
            BSLS_ASSERT('/' == basename[-1]);

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

FilesystemUtil::Offset FilesystemUtil::getAvailableSpace(const char *path)
{
    BSLS_ASSERT(path);

# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::statvfs buffer;
    int rc = ::statvfs(path, &buffer);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::statvfs64 buffer;
    int rc = ::statvfs64(path, &buffer);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
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
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::statvfs buffer;
    int rc = ::fstatvfs(descriptor, &buffer);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::statvfs64 buffer;
    int rc = ::fstatvfs64(descriptor, &buffer);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif
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

FilesystemUtil::Offset FilesystemUtil::getFileSize(FileDescriptor descriptor)
{
    return u::ImpUtil::getFileSize(descriptor);
}

FilesystemUtil::Offset FilesystemUtil::getFileSizeLimit()
{
# if defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::rlimit rl, rlMax, rlInf;
    int rc = ::getrlimit(RLIMIT_FSIZE, &rl);
# elif defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    struct ::rlimit64 rl, rlMax, rlInf;
    int rc = ::getrlimit64(RLIMIT_FSIZE, &rl);
# else
#  error "'bdls_filesystemutil' does not support this platform."
# endif

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

namespace {
template <class STRING_TYPE>
int u_getSymbolicLinkTarget(STRING_TYPE *result,
                            const char  *path)
{
    BSLS_ASSERT(path);
    BSLS_ASSERT(result);

    char buffer[4096];

    ssize_t nBytes = ::readlink(path, buffer, sizeof buffer);
    if (nBytes >= 0) {
        result->assign(buffer, nBytes);
        return 0;                                                     // RETURN
    }
    // 'errno' contains the error code
    return -1;
}
}  // close unnamed namespace

namespace {
template <class STRING_TYPE>
int u_getWorkingDirectory(STRING_TYPE *path)
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
}  // close unnamed namespace

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

int FilesystemUtil::getSymbolicLinkTarget(bsl::string *result,
                                          const char  *path)
{
    return u_getSymbolicLinkTarget(result, path);
}

int FilesystemUtil::getSymbolicLinkTarget(std::string *result,
                                          const char  *path)
{
    return u_getSymbolicLinkTarget(result, path);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int FilesystemUtil::getSymbolicLinkTarget(std::pmr::string *result,
                                          const char       *path)
{
    return u_getSymbolicLinkTarget(result, path);
}
#endif

int FilesystemUtil::getWorkingDirectory(bsl::string *path)
{
    return u_getWorkingDirectory(path);
}

int FilesystemUtil::getWorkingDirectory(std::string *path)
{
    return u_getWorkingDirectory(path);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int FilesystemUtil::getWorkingDirectory(std::pmr::string *path)
{
    return u_getWorkingDirectory(path);
}
#endif

int FilesystemUtil::getSystemTemporaryDirectory(bsl::string *path)
{
    return u_getSystemTemporaryDirectory(path);
}

int FilesystemUtil::getSystemTemporaryDirectory(std::string *path)
{
    return u_getSystemTemporaryDirectory(path);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int FilesystemUtil::getSystemTemporaryDirectory(std::pmr::string *path)
{
    return u_getSystemTemporaryDirectory(path);
}
#endif

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
        PathUtil::appendRaw(&workingPath,
                            directoryStack.back().c_str(),
                            static_cast<int>(directoryStack.back().length()));
        int rc = makeDirectory(workingPath.c_str(), false);
        if (0 != rc) {
            if (rc == k_ERROR_ALREADY_EXISTS) {
                if (!isDirectory(workingPath, true)) {
                    return k_ERROR_PATH_NOT_FOUND;                    // RETURN
                }
            }
            else {
                return rc;                                            // RETURN
            }
        }
        directoryStack.pop_back();
    }
    return 0;
}

int FilesystemUtil::createPrivateDirectory(const char *path)
{
    return makeDirectory(path, true);
}

namespace {
template <class VECTOR_TYPE>
int u_findMatchingPaths(VECTOR_TYPE *result, const char *pattern)
{
    BSLS_ASSERT(result);

    result->clear();
    return FilesystemUtil::visitPaths(
        pattern,
        bdlf::BindUtil::bind(
            &pushBackWrapper<VECTOR_TYPE>, result, bdlf::PlaceHolders::_1));
}
}  // close unnamed namespace

int FilesystemUtil::findMatchingPaths(bsl::vector<bsl::string> *result,
                                      const char               *pattern)
{
    return u_findMatchingPaths<bsl::vector<bsl::string> >(result, pattern);
}

int FilesystemUtil::findMatchingPaths(std::vector<std::string> *result,
                                      const char               *pattern)
{
    return u_findMatchingPaths<std::vector<std::string> >(result, pattern);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int FilesystemUtil::findMatchingPaths(
                                   std::pmr::vector<std::pmr::string> *result,
                                   const char                         *pattern)
{
    return u_findMatchingPaths<std::pmr::vector<std::pmr::string> >(result,
                                                                    pattern);
}
#endif

}  // close package namespace

#if defined(BSLS_PLATFORM_OS_SOLARIS)
namespace {
BSLA_MAYBE_UNUSED
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
#if defined(BSLS_PLATFORM_OS_LINUX) ||          \
    defined(BSLS_PLATFORM_OS_SOLARIS) ||        \
    defined(BSLS_PLATFORM_OS_AIX)
#if defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)
    if (reserveFlag && 0 == ftruncate64(descriptor, size)) {
        reserveFlag = false;  //  File space has been allocated
    }
#else
    if (reserveFlag && 0 == ftruncate(descriptor, size)) {
        reserveFlag = false;  //  File space has been allocated
    }
#endif
#endif
    if (reserveFlag) {
        // Reserve space the old-fashioned way.

        if (0 == increment) {
            increment = k_DEFAULT_FILE_GROWTH_INCREMENT;
        }

        bsl::vector<char> bufferVector(increment, '\x1');

        Offset bytesToGrow = size - currentSize;
        while (bytesToGrow > 0) {
            int nBytes = static_cast<int>(
                       bsl::min(bytesToGrow, static_cast<Offset>(increment)));
            int rc = write(descriptor, bufferVector.data(), nBytes);
            if (rc != nBytes) {
                return -1;                                            // RETURN
            }
            bytesToGrow -= nBytes;
        }

        return 0;                                                     // RETURN
    }
    Offset res = seek(descriptor, size - 1, e_SEEK_FROM_BEGINNING);
    if (-1 == res || 1 != write(descriptor, "", 1))
    {
        return -1;                                                    // RETURN
    }

    return 0;
}

int FilesystemUtil::rollFileChain(const bsl::string_view& path, int maxSuffix)
{
    enum { MAX_SUFFIX_LENGTH = 10 };

    bsl::size_t length =
        static_cast<int>(path.length()) + MAX_SUFFIX_LENGTH + 2;

    // Use a single RAII allocation to ensure exception neutrality.
    bsl::vector<char> bufferVector(2 * length, '\0');
    char *buffer   = bufferVector.data();
    char *fromName = buffer;
    char *toName   = buffer + length;

    bsl::memcpy(toName, path.data(), path.length());
    bsl::memcpy(fromName, path.data(), path.length());

    char *fromSuffixPos = fromName + path.length();
    char *toSuffixPos   = toName + path.length();

    bsl::size_t suffixLen = length - path.length();

    snprintf(toSuffixPos, suffixLen, ".%d", maxSuffix);

    if (!exists(toName) ||
        0 == remove(toName, true)) {
        while (0 < --maxSuffix) {
            snprintf(fromSuffixPos, suffixLen, ".%d", maxSuffix);

            if (exists(fromName) && 0 != move(fromName, toName)) {
                break;
            }

            using bsl::swap;
            swap(toName, fromName);
            swap(toSuffixPos, fromSuffixPos);
        }

        // NUL-terminate 'fromName' so it matches 'path', removing any suffix.
        *fromSuffixPos = '\0';
        if (0 == maxSuffix && exists(fromName) &&
            0 != move(fromName, toName)) {
            maxSuffix = -1;
        }
    }

    return maxSuffix;
}

namespace {
template <class STRING_TYPE>
FilesystemUtil::FileDescriptor u_createTemporaryFile(
                                              STRING_TYPE             *outPath,
                                              const bsl::string_view&  prefix)
{
    BSLS_ASSERT(outPath);

    FilesystemUtil::FileDescriptor result;
    STRING_TYPE localOutPath = *outPath;
    for (int i = 0; i < 10; ++i) {
        FilesystemUtil::makeUnsafeTemporaryFilename(&localOutPath, prefix);
        result = FilesystemUtil::open(localOutPath.c_str(),
                                      FilesystemUtil::e_CREATE_PRIVATE,
                                      FilesystemUtil::e_READ_WRITE);
        if (result != FilesystemUtil::k_INVALID_FD) {
            *outPath = localOutPath;
            break;
        }
    }
    return result;
}
}  // close unnamed namespace

FilesystemUtil::FileDescriptor FilesystemUtil::createTemporaryFile(
                                              bsl::string             *outPath,
                                              const bsl::string_view&  prefix)
{
    return u_createTemporaryFile(outPath, prefix);
}

FilesystemUtil::FileDescriptor FilesystemUtil::createTemporaryFile(
                                              std::string             *outPath,
                                              const bsl::string_view&  prefix)
{
    return u_createTemporaryFile(outPath, prefix);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
FilesystemUtil::FileDescriptor FilesystemUtil::createTemporaryFile(
                                              std::pmr::string        *outPath,
                                              const bsl::string_view&  prefix)
{
    return u_createTemporaryFile(outPath, prefix);
}
#endif

namespace {
template <class STRING_TYPE>
void u_makeUnsafeTemporaryFilename(STRING_TYPE             *outPath,
                                   const bsl::string_view&  prefix)
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
    outPath->assign(prefix.data(), prefix.length());
    outPath->append(suffix, sizeof(suffix));
}
}  // close unnamed namespace

void FilesystemUtil::makeUnsafeTemporaryFilename(
                                              bsl::string             *outPath,
                                              const bsl::string_view&  prefix)
{
    u_makeUnsafeTemporaryFilename(outPath, prefix);
}

void FilesystemUtil::makeUnsafeTemporaryFilename(
                                              std::string             *outPath,
                                              const bsl::string_view&  prefix)
{
    u_makeUnsafeTemporaryFilename(outPath, prefix);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void FilesystemUtil::makeUnsafeTemporaryFilename(
                                              std::pmr::string        *outPath,
                                              const bsl::string_view&  prefix)
{
    u_makeUnsafeTemporaryFilename(outPath, prefix);
}
#endif


namespace {
template <class STRING_TYPE>
int u_createTemporaryDirectory(STRING_TYPE             *outPath,
                               const bsl::string_view&  prefix)
{
    BSLS_ASSERT(outPath);

    int result;
    STRING_TYPE localOutPath = *outPath;
    for (int i = 0; i < 10; ++i) {
        FilesystemUtil::makeUnsafeTemporaryFilename(&localOutPath, prefix);
        result = FilesystemUtil::createPrivateDirectory(localOutPath);
        if (result == 0) {
            *outPath = localOutPath;
            break;
        }
    }
    return result;
}
}  // close unnamed namespace

int FilesystemUtil::createTemporaryDirectory(bsl::string             *outPath,
                                             const bsl::string_view&  prefix)
{
    return u_createTemporaryDirectory(outPath, prefix);
}

int FilesystemUtil::createTemporaryDirectory(std::string             *outPath,
                                             const bsl::string_view&  prefix)
{
    return u_createTemporaryDirectory(outPath, prefix);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int FilesystemUtil::createTemporaryDirectory(std::pmr::string        *outPath,
                                             const bsl::string_view&  prefix)
{
    return u_createTemporaryDirectory(outPath, prefix);
}
#endif

}  // close package namespace

namespace {
namespace u {

#if defined(BSLS_PLATFORM_OS_UNIX) \
 && defined(U_USE_UNIX_FILE_SYSTEM_INTERFACE)

                          // ------------------------
                          // struct UnixInterfaceUtil
                          // ------------------------

// CLASS METHODS
long UnixInterfaceUtil::get_st_mtim_nsec(const stat& stat)
{
    (void) stat;
     return stat.BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER;
}

UnixInterfaceUtil::time_t UnixInterfaceUtil::get_st_mtime(const stat& stat)
{
    // Note that 'st_mtime' is actually the name of a macro in some build
    // configurations in some Unix systems (see notes above).

    return stat.st_mtime;
}

UnixInterfaceUtil::off_t UnixInterfaceUtil::get_st_size(const stat& stat)
{
    // Note that 'st_size' is generally not a macro, and this function is
    // provided for the sake of consistency.

    return stat.st_size;
}

int UnixInterfaceUtil::fstat(int fildes, stat *buf)
{
    return ::fstat(fildes, buf);
}

#elif defined(BSLS_PLATFORM_OS_UNIX) \
   && defined(U_USE_TRANSITIONAL_UNIX_FILE_SYSTEM_INTERFACE)

                    // ------------------------------------
                    // struct TransitionalUnixInterfaceUtil
                    // ------------------------------------

long TransitionalUnixInterfaceUtil::get_st_mtim_nsec(const stat64& stat)
{
    (void) stat;
     return stat.BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER;
}

TransitionalUnixInterfaceUtil::time_t
TransitionalUnixInterfaceUtil::get_st_mtime(const stat64& stat)
{
    // Note that 'st_mtime' is actually the name of a macro in some build
    // configurations in some Unix systems (see notes above).

    return stat.st_mtime;
}

TransitionalUnixInterfaceUtil::off64_t
TransitionalUnixInterfaceUtil::get_st_size(const stat64& stat)
{
    // Note that 'st_size' is generally not a macro, and this function is
    // provided for the sake of consistency.

    return stat.st_size;
}

int TransitionalUnixInterfaceUtil::fstat64(int fildes, stat64 *buf)
{
    return ::fstat64(fildes, buf);
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

                        // ---------------------------
                        // struct WindowsInterfaceUtil
                        // ---------------------------

// CLASS METHODS
WindowsInterfaceUtil::BOOL WindowsInterfaceUtil::FileTimeToSystemTime(
                                                  const FILETIME *lpFileTime,
                                                  LPSYSTEMTIME    lpSystemTime)
{
    return ::FileTimeToSystemTime(lpFileTime, lpSystemTime);
}

WindowsInterfaceUtil::DWORD WindowsInterfaceUtil::GetFileSize(
                                                       HANDLE  hFile,
                                                       LPDWORD lpFileSizeHigh)
{
    return ::GetFileSize(hFile, lpFileSizeHigh);
}

WindowsInterfaceUtil::BOOL
WindowsInterfaceUtil::GetFileTime(HANDLE     hFile,
                                  LPFILETIME lpCreationTime,
                                  LPFILETIME lpLastAccessTime,
                                  LPFILETIME lpLastWriteTime){
    return ::GetFileTime(
        hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
}

WindowsInterfaceUtil::DWORD WindowsInterfaceUtil::GetLastError()
{
    return ::GetLastError();
}

WindowsInterfaceUtil::BOOL WindowsInterfaceUtil::SystemTimeToFileTime(
                                                const SYSTEMTIME *lpSystemTime,
                                                LPFILETIME        lpFileTime)
{
    return ::SystemTimeToFileTime(lpSystemTime, lpFileTime);
}

#else
# error "'bdls_filesystemutil' does not support this platform."
#endif

}  // close namespace u
}  // close unnamed namespace
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
