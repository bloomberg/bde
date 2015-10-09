// bdls_filesystemutil.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_FILESYSTEMUTIL
#define INCLUDED_BDLS_FILESYSTEMUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods for filesystem access with multi-language names.
//
//@CLASSES:
//  bdls::FilesystemUtil: namespace for filesystem access methods
//
//@SEE_ALSO: bdls_pathutil
//
//@DESCRIPTION: This component provides a platform-independent interface to
// filesystem utility methods, supporting multi-language file and path names.
// Each method in the 'bdls::FilesystemUtil' namespace is a thin wrapper on top
// of the operating system's own filesystem access functions, providing a
// consistent and unambiguous interface for handling files on all supported
// platforms.
//
// Methods in this component can be used to manipulate files with any name in
// any language on all supported platforms.  To provide such support, the
// following restrictions are applied to file names and patterns passed to
// methods of this component: On Windows, all file names and patterns must be
// passed as UTF-8-encoded strings; file search results will similarly be
// encoded as UTF-8.  On Posix, file names and patterns may be passed in any
// encoding, but all processes accessing a given file must encode its name in
// the same encoding.  On modern Posix installations, this effectively means
// that file names and patterns should be encoded in UTF-8, just as on Windows.
// See the section "Platform-Specific File Name Encoding Caveats" below.
//
///Policies for 'open'
///-------------------
// The behavior of the 'open' method is governed by three sets of enumerations:
//
///Open/Create Policy: 'bdls::FilesystemUtil::FileOpenPolicy'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdls::FilesystemUtil::FileOpenPolicy' governs whether 'open' creates a new
// file or opens an existing one.  The following values are possible:
//
//: 'e_OPEN':
//:   Open an existing file.
//:
//: 'e_CREATE':
//:   Create a new file.
//:
//: 'e_OPEN_OR_CREATE':
//:   Open a file if it exists, and create a new file otherwise.
//
///Input/Output Access Policy: 'bdls::FilesystemUtil::FileIOPolicy'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdls::FilesystemUtil::FileIOPolicy' governs what Input/Output operations
// are allowed on a file after it is opened.  The following values are
// possible:
//
//: 'e_READ_ONLY':
//:   Allow reading only.
//:
//: 'e_WRITE_ONLY':
//:    Allow writing only.
//:
//: 'e_READ_WRITE':
//:    Allow both reading and writing.
//:
//: 'e_APPEND_ONLY':
//:    Allow appending to end-of-file only.
//:
//: 'e_READ_APPEND':
//:    Allow both reading and appending to end-of-file.
//
///Truncation Policy: 'bdls::FilesystemUtil::FileTruncatePolicy'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdls::FilesystemUtil::FileTruncatePolicy' governs whether 'open' deletes
// the existing contents of a file when it is opened.  The following values are
// possible:
//
//: 'e_TRUNCATE':
//:   Delete the file's contents.
//:
//: 'e_KEEP':
//:    Keep the file's contents.
//
///Starting Points for 'seek'
///--------------------------
// The behavior of the 'seek' method is governed by an enumeration that
// determines the point from which the seek operation starts:
//
//: 'e_SEEK_FROM_BEGINNING':
//:   Seek from the beginning of the file.
//:
//: 'e_SEEK_FROM_CURRENT':
//:   Seek from the current position in the file.
//:
//: 'e_SEEK_FROM_END':
//:   Seek from the end of the file.
//
///Platform-Specific File Locking Caveats
///--------------------------------------
// Locking has the following caveats for the following operating systems:
//:
//: o On Posix, closing a file releases all locks on all file descriptors
//:   referring to that file within the current process.  [doc 1] [doc 2]
//:
//: o On Posix, the child of a fork does not inherit the locks of the parent
//:   process.  [doc 1] [doc 2]
//:
//: o On at least some flavors of Unix, you can't lock a file for writing using
//:   a file descriptor opened in read-only mode.
//
///Platform-Specific Atomicity Caveats
///-----------------------------------
// The 'bdls::FilesystemUtil::read' and 'bdls::FilesystemUtil::write' methods
// add no atomicity guarantees for reading and writing to those provided (if
// any) by the underlying platform's methods for reading and writing (see
// 'http://lwn.net/articles/180387/').
//
///Platform-Specific File Name Encoding Caveats
///--------------------------------------------
// File-name encodings have the following caveats for the following operating
// systems:
//
//: o On Windows, methods of 'bdls::FilesystemUtil' that take a file or
//:   directory name or pattern as a 'char*' or 'bsl::string' type assume that
//:   the name is encoded in UTF-8.  The routines attempt to convert the name
//:   to a UTF-16 'wchar_t' string via 'bdlde::CharConvertUtf16::utf8ToUtf16',
//:   and if the conversion succeeds, call the Windows wide-character 'W' APIs
//:   with the UTF-16 name.  If the conversion fails, the method fails.
//:   Similarly, file searches returning file names call the Windows
//:   wide-character 'W' APIs and convert the resulting UTF-16 names to UTF-8.
//:
//:   o Narrow-character file names in other encodings, containing characters
//:     with values in the range 128 - 255, will likely result in files being
//:     created with names that appear garbled if the conversion from UTF-8 to
//:     UTF-16 happens to succeed.
//:
//:   o Neither 'utf8ToUtf16' nor the Windows 'W' APIs do any normalization of
//:     the UTF-16 strings resulting from UTF-8 conversion, and it is therefore
//:     possible to have sets of file names that have the same visual
//:     representation but are treated as different names by the filesystem.
//:
//: o On Posix, a file name or pattern supplied to methods of
//:   'bdls::FilesystemUtil' as a 'char*' or 'bsl::string' type is passed
//:   unchanged to the underlying system file APIs.  Because the file names and
//:   patterns are passed unchanged, 'bdls::FilesystemUtil' methods will work
//:   correctly on Posix with any encoding, but will *interoperate* only with
//:   processes that use the same encoding as the current process.
//:
//: o For compatibility with most modern Posix installs, and consistency with
//:   this component's Windows API, best practice is to encode all file names
//:   and patterns in UTF-8.
//
///File Truncation Caveats
///-----------------------
// In order to provide consistent behavior across both Posix and Windows
// platforms, when the 'open' method is called file truncation is allowed only
// if the client requests an 'openPolicy' containing the word 'CREATE' and/or
// an 'ioPolicy' containing the word 'WRITE'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: General Usage
/// - - - - - - - - - - - -
// In this example, we start with a (relative) native path to a directory
// containing log files:
//..
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//    bsl::string logPath = "temp.1\\logs";
//  #else
//    bsl::string logPath = "temp.1/logs";
//  #endif
//..
// Suppose that we want to separate files into "old" and "new" subdirectories
// on the basis of modification time.  We will provide paths representing these
// locations, and create the directories if they do not exist:
//..
//  bsl::string oldPath(logPath), newPath(logPath);
//  bdls::PathUtil::appendRaw(&oldPath, "old");
//  bdls::PathUtil::appendRaw(&newPath, "new");
//  int rc = bdls::FilesystemUtil::createDirectories(oldPath.c_str(), true);
//  assert(0 == rc);
//  rc = bdls::FilesystemUtil::createDirectories(newPath.c_str(), true);
//  assert(0 == rc);
//..
// We know that all of our log files match the pattern "*.log", so let's search
// for all such files in the log directory:
//..
//  bdls::PathUtil::appendRaw(&logPath, "*.log");
//  bsl::vector<bsl::string> logFiles;
//  bdls::FilesystemUtil::findMatchingPaths(&logFiles, logPath.c_str());
//..
// Now for each of these files, we will get the modification time.  Files that
// are older than 2 days will be moved to "old", and the rest will be moved to
// "new":
//..
//  bdlt::Datetime modTime;
//  bsl::string   fileName;
//  for (bsl::vector<bsl::string>::iterator it = logFiles.begin();
//                                                it != logFiles.end(); ++it) {
//    assert(0 ==
//               bdls::FilesystemUtil::getLastModificationTime(&modTime, *it));
//    assert(0 == bdls::PathUtil::getLeaf(&fileName, *it));
//    bsl::string *whichDirectory =
//                2 < (bdlt::CurrentTime::utc() - modTime).totalDays()
//                ? &oldPath
//                : &newPath;
//    bdls::PathUtil::appendRaw(whichDirectory, fileName.c_str());
//    assert(0 == bdls::FilesystemUtil::move(it->c_str(),
//                                         whichDirectory->c_str()));
//    bdls::PathUtil::popLeaf(whichDirectory);
//  }
//..
//
///Example 2: Using 'bdls::FilesystemUtil::visitPaths'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdls::FilesystemUtil::visitPaths' enables clients to define a functor to
// operate on file paths that match a specified pattern.  In this example, we
// create a function that can be used to filter out files that have a last
// modified time within a particular time frame.
//
// First we define our filtering function:
//..
//  void getFilesWithinTimeframe(bsl::vector<bsl::string> *vector,
//                               const char               *item,
//                               const bdlt::Datetime&     start,
//                               const bdlt::Datetime&     end)
//  {
//      bdlt::Datetime datetime;
//      int ret = bdls::FilesystemUtil::getLastModificationTime(&datetime,
//                                                              item);
//
//      if (ret) {
//          return;                                                   // RETURN
//      }
//
//      if (datetime < start || datetime > end) {
//          return;                                                   // RETURN
//      }
//
//      vector->push_back(item);
//  }
//..
// Then, with the help of 'bdls::FilesystemUtil::visitPaths' and
// 'bdlf::BindUtil::bind', we create a function for finding all file paths that
// match a specified pattern and have a last modified time within a specified
// start and end time (both specified as a 'bdlt::Datetime'):
//..
//  void findMatchingFilesInTimeframe(bsl::vector<bsl::string> *result,
//                                    const char               *pattern,
//                                    const bdlt::Datetime&     start,
//                                    const bdlt::Datetime&     end)
//  {
//      result->clear();
//      bdls::FilesystemUtil::visitPaths(
//                               pattern,
//                               bdlf::BindUtil::bind(&getFilesWithinTimeframe,
//                                                    result,
//                                                    bdlf::PlaceHolders::_1,
//                                                    start,
//                                                    end));
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_TYPES_H
#include <sys/types.h>
#define INCLUDED_TYPES_H
#endif

namespace BloombergLP {

namespace bdls {
                           // =====================
                           // struct FilesystemUtil
                           // =====================

struct FilesystemUtil {
    // This 'struct' provides a namespace for utility functions dealing with
    // platform-independent filesystem access.

    // TYPES
#ifdef BSLS_PLATFORM_OS_WINDOWS
    typedef void *HANDLE;
        // 'HANDLE' is a stand-in for the Windows API 'HANDLE' type, to allow
        // us to avoid including 'windows.h' in this header.  'HANDLE' should
        // not be used by client code.

    typedef HANDLE FileDescriptor;
        // 'FileDescriptor' is an alias for the underlying file system's native
        // file descriptor / file handle type.

    typedef __int64 Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.

    static const Offset k_OFFSET_MAX = _I64_MAX;  // maximum representable
                                                  // file offset value

    static const Offset k_OFFSET_MIN = _I64_MIN;  // minimum representable file
                                                  // offset value

#else
    typedef int     FileDescriptor;
        // 'FileDescriptor' is an alias for the underlying file system's native
        // file descriptor / file handle type.

#if defined(BSLS_PLATFORM_OS_FREEBSD) \
 || defined(BSLS_PLATFORM_OS_DARWIN)  \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    // 'off_t' is 64-bit on Darwin/FreeBSD/cygwin (even when running 32-bit),
    // so they do not have an 'off64_t' type.

    typedef off_t Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.
#else
    typedef off64_t Offset;
        // 'Offset' is an alias for a signed value, representing the offset of
        // a location within a file.
#endif

#ifdef BSLS_PLATFORM_CPU_64_BIT
    static const Offset k_OFFSET_MAX =  (9223372036854775807L);
        // maximum representable file offset value

    static const Offset k_OFFSET_MIN = (-9223372036854775807L-1);
        // minimum representable file offset value
#else
    static const Offset k_OFFSET_MAX =  (9223372036854775807LL);
        // maximum representable file offset value

    static const Offset k_OFFSET_MIN = (-9223372036854775807LL-1);
        // minimum representable file offset value
#endif

#endif

    enum Whence {
        // Enumeration used to distinguish among different starting points for
        // a seek operation.

        e_SEEK_FROM_BEGINNING = 0,  // Seek from beginning of file.
        e_SEEK_FROM_CURRENT   = 1,  // Seek from current position.
        e_SEEK_FROM_END       = 2   // Seek from end of file.
    };

    enum {
        k_DEFAULT_FILE_GROWTH_INCREMENT = 65536  // default block size by which
                                                 // to grow files
    };

    enum {
        k_ERROR_LOCKING_CONFLICT =  1, // value representing a failure to
                                       // obtain a lock on a file

        k_BAD_FILE_DESCRIPTOR    = -1  // value indicating a bad file
                                       // descriptor was supplied
    };

    enum FileOpenPolicy {
        // Enumeration used to determine whether 'open' should open an existing
        // file, or create a new file.

        e_OPEN,           // Open a file if it exists, and fail otherwise.

        e_CREATE,         // Create a new file, and fail if the file already
                          // exists.

        e_OPEN_OR_CREATE  // Open a file if it exists, and create a new file
                          // otherwise.
    };

    enum FileIOPolicy {
        // Enumeration used to distinguish between different sets of actions
        // permitted on an open file descriptor.

        e_READ_ONLY,    // Allow reading only.
        e_WRITE_ONLY,   // Allow writing only.
        e_APPEND_ONLY,  // Allow appending to end-of-file only.
        e_READ_WRITE,   // Allow both reading and writing.
        e_READ_APPEND   // Allow both reading and appending to end-of-file.
    };

    enum FileTruncatePolicy {
        // Enumeration used to distinguish between different ways to handle the
        // contents, if any, of an existing file immediately upon opening the
        // file.

        e_TRUNCATE,  // Delete the file's contents on open.
        e_KEEP       // Keep the file's contents.
    };

    // CLASS DATA
    static const FileDescriptor k_INVALID_FD;  // 'FileDescriptor' value
                                               // representing no file, used
                                               // as the error return for
                                               // 'open'

    // CLASS METHODS
    static FileDescriptor open(const char         *path,
                               FileOpenPolicy      openPolicy,
                               FileIOPolicy        ioPolicy,
                               FileTruncatePolicy  truncatePolicy = e_KEEP);
    static FileDescriptor open(const bsl::string&  path,
                               FileOpenPolicy      openPolicy,
                               FileIOPolicy        ioPolicy,
                               FileTruncatePolicy  truncatePolicy = e_KEEP);
        // Open the file at the specified 'path', using the specified
        // 'openPolicy' to determine whether to open an existing file or create
        // a new file, and using the specified 'ioPolicy' to determine whether
        // the file will be opened for reading, writing, or both.  Optionally
        // specify a 'truncatePolicy' to determine whether any contents of the
        // file will be deleted before 'open' returns.  If 'truncatePolicy' is
        // not supplied, the value 'e_KEEP' will be used.  Return a valid
        // 'FileDescriptor' for the file on success, or 'k_INVALID_FD'
        // otherwise.  If 'openPolicy' is 'e_OPEN', the file will be opened if
        // it exists, and 'open' will fail otherwise.  If 'openPolicy' is
        // 'e_CREATE', and no file exists at 'path', a new file will be
        // created, and 'open' will fail otherwise.  If 'openPolicy' is
        // 'e_OPEN_OR_CREATE', the file will be opened if it exists, and a new
        // file will be created otherwise.  If 'ioPolicy' is 'e_READ_ONLY', the
        // returned 'FileDescriptor' will allow only read operations on the
        // file.  If 'ioPolicy' is 'e_WRITE_ONLY' or 'e_APPEND_ONLY', the
        // returned 'FileDescriptor' will allow only write operations on the
        // file.  If 'ioPolicy' is 'e_READ_WRITE' or 'e_READ_APPEND', the
        // returned 'FileDescriptor' will allow both read and write operations
        // on the file.  Additionally, if 'ioPolicy' is 'e_APPEND_ONLY' or
        // 'e_READ_APPEND' all writes will be made to the end of the file
        // ("append mode").  If 'truncatePolicy' is 'e_TRUNCATE', the file will
        // have zero length when 'open' returns.  If 'truncatePolicy' is
        // 'e_KEEP', the file will be opened with its existing contents, if
        // any.  Note that when a file is opened in 'append' mode, all writes
        // will go to the end of the file, even if there has been seeking on
        // the file descriptor or another process has changed the length of the
        // file, though append-mode writes are not guaranteed to be atomic.
        // Note that 'open' will fail to open a file with a 'truncatePolicy' of
        // 'e_TRUNCATE' unless at least one of the following policies is
        // specified for 'openPolicy' or 'ioPolicy':
        //: o 'e_CREATE'
        //: o 'e_OPEN_OR_CREATE'
        //: o 'e_WRITE_ONLY
        //: o 'e_READ_WRITE'

    static int close(FileDescriptor descriptor);
        // Close the specified 'descriptor'.  Return 0 on success and a
        // non-zero value otherwise.  A return value of
        // 'k_BAD_FILE_DESCRIPTOR' indicates that the supplied 'descriptor' is
        // invalid.

    static int getWorkingDirectory(bsl::string *path);
        // Load into the specified 'path' the absolute pathname of the current
        // working directory.  Return 0 on success and a non-zero value
        // otherwise.

    static int setWorkingDirectory(const bsl::string&  path);
    static int setWorkingDirectory(const char         *path);
        // Set the working directory of the current process to the specified
        // 'path'.  Return 0 on success and a non-zero value otherwise.

    static bool exists(const bsl::string&  path);
    static bool exists(const char         *path);
        // Return 'true' if there currently exists a file or directory at the
        // specified 'path', and 'false' otherwise.

    static bool isRegularFile(const bsl::string&  path,
                              bool                followLinksFlag = false);
    static bool isRegularFile(const char         *path,
                              bool                followLinksFlag = false);
        // Return 'true' if there currently exists a regular file at the
        // specified 'path', and 'false' otherwise.  If there is a symbolic
        // link at 'path', follow it only if the optionally specified
        // 'followLinksFlag' is 'true' (otherwise, return 'false' as the
        // symbolic link itself is not a regular file irrespective of the file
        // to which it points).  Platform-specific note: On POSIX, this is a
        // positive test on the "regular file" mode; on Windows, this is a
        // negative test on the "directory" attribute, i.e., on Windows,
        // everything that exists and is not a directory is a regular file.

    static bool isDirectory(const bsl::string&  path,
                            bool                followLinksFlag = false);
    static bool isDirectory(const char         *path,
                            bool                followLinksFlag = false);
        // Return 'true' if there currently exists a directory at the specified
        // 'path', and 'false' otherwise.  If there is a symbolic link at
        // 'path', follow it only if the optionally specified 'followLinksFlag'
        // is 'true' (otherwise return 'false').  Platform-specific note: On
        // Windows, a "shortcut" is not a symbolic link.

    static int getLastModificationTime(bdlt::Datetime     *time,
                                       const bsl::string&  path);
    static int getLastModificationTime(bdlt::Datetime     *time,
                                       const char         *path);
        // Load into the specified 'time' the last modification time of the
        // file at the specified 'path', as reported by the filesystem.  Return
        // 0 on success, and a non-zero value otherwise.  Note that the time is
        // reported in UTC.

    // TBD: write setModificationTime() when SetFileInformationByHandle()
    // becomes available on our standard Windows platforms.

    static int createDirectories(
                              const bsl::string&  path,
                              bool                isLeafDirectoryFlag = false);
    static int createDirectories(
                              const char         *path,
                              bool                isLeafDirectoryFlag = false);
        // Create any directories in the specified 'path' that do not exist.
        // If the optionally specified 'isLeafDirectoryFlag' is 'true', then
        // treat the last filename in 'path' as a directory and attempt to
        // create it.  Otherwise, treat the last filename as a regular file and
        // ignore it.  Return 0 on success, and a non-zero value if any needed
        // directories in 'path' could not be created.  Note that this
        // function may return an error status if another task attempts to
        // create a directory in 'path' concurrently to this function call.

    static void visitPaths(
                         const bsl::string&                           pattern,
                         const bsl::function<void(const char *path)>& visitor);
    static void visitPaths(
                        const char                                   *pattern,
                        const bsl::function<void(const char *path)>&  visitor);
        // Call the specified 'visitor' functor for each path in the filesystem
        // matching the specified 'pattern'.  If 'visitor' deletes files or
        // directories during the search, the behavior is
        // implementation-dependent: 'visitor' may subsequently be called with
        // paths which have already been deleted, or it may not.  Note that
        // there is no stability risk in that case.  See 'findMatchingPaths'
        // for a discussion of how 'pattern' is interpreted.

    static void findMatchingPaths(bsl::vector<bsl::string> *result,
                                  const char               *pattern);
        // Load into the specified 'result' vector all paths in the filesystem
        // matching the specified 'pattern'.  The '*' character will match any
        // number of characters in a filename; however, this matching will not
        // span a directory separator (e.g., "logs/m*.txt" will not match
        // "logs/march/001.txt").  '?' will match any one character.  '*' and
        // '?' may be used any number of times in the pattern.  The special
        // directories "." and ".." will not be matched against any pattern.
        // Note that any initial contents of 'result' will be erased, and that
        // the paths in 'result' will not be in any particular guaranteed
        // order.
        //
        // WINDOWS-SPECIFIC NOTE: To support DOS idioms, the OS-provided search
        // function has behavior that we have chosen not to work around: an
        // extension consisting of wild-card characters ('?', '*') can match
        // an extension or *no* extension.  E.g., "file.?" matches "file.z",
        // but not "file.txt"; however, it also matches "file" (without any
        // extension).  Likewise, "*.*" matches any filename, including
        // filenames having no extension.

    static Offset getAvailableSpace(const bsl::string&  path);
    static Offset getAvailableSpace(const char         *path);
        // Return the number of bytes available for allocation in the file
        // system where the file or directory with the specified 'path'
        // resides, or a negative value if an error occurs.

    static Offset getAvailableSpace(FileDescriptor descriptor);
        // Return the number of bytes available for allocation in the file
        // system where the file with the specified 'descriptor' resides, or a
        // negative value if an error occurs.

    static Offset getFileSize(const bsl::string&  path);
    static Offset getFileSize(const char         *path);
        // Return the size, in bytes, of the file or directory at the specified
        // 'path', or a negative value if an error occurs.  Note that the size
        // of a symbolic link is the size of the file or directory to which it
        // points.

    static Offset getFileSizeLimit();
        // Return the file size limit for this process, 'k_OFFSET_MAX' if no
        // limit is set, or a negative value if an error occurs.  Note that if
        // you are doing any calculations involving the returned value, it is
        // recommended to check for 'k_OFFSET_MAX' specifically to avoid
        // integer overflow in your calculations.

    static int lock(FileDescriptor descriptor, bool lockWriteFlag);
        // Acquire a lock for the file with the specified 'descriptor'.  If
        // 'lockWriteFlag' is true, acquire an exclusive write lock; otherwise
        // acquire a (possibly) shared read lock.  The calling thread will
        // block until the lock is acquired.  Return 0 on success, and a
        // non-zero value otherwise.  Note that this operation locks the
        // indicated file for use by the current *process*, but the behavior is
        // unspecified (and platform-dependent) when either attempting to lock
        // 'descriptor' multiple times, or attempting to lock another
        // descriptor referring to the same file, within a single process.

    static int tryLock(FileDescriptor descriptor, bool lockWriteFlag);
        // Acquire a lock for the file with the specified 'descriptor' if it is
        // currently available.  If the specified 'lockWriteFlag' is true,
        // acquire an exclusive write lock unless another process has any type
        // of lock on the file.  If 'lockWriteFlag' is false, acquire a shared
        // read lock unless a process has a write lock.  This method will not
        // block.  Return 0 on success, 'k_ERROR_LOCKING_CONFLICT' if the
        // platform reports the lock could not be acquired because another
        // process holds a conflicting lock, and a negative value for any other
        // kind of error.  Note that this operation locks the indicated file
        // for the current *process*, but the behavior is unspecified (and
        // platform-dependent) when either attempting to lock 'descriptor'
        // multiple times, or attempting to lock another descriptor referring
        // to the same file, within a single process.

    static int unlock(FileDescriptor descriptor);
        // Release any lock this process holds on the file with the specified
        // 'descriptor'.  Return 0 on success, and a non-zero value otherwise.

    static int map(FileDescriptor   descriptor,
                   void           **address,
                   Offset           offset,
                   int              size,
                   int              mode);
        // Map the region of the specified 'size' bytes, starting at the
        // specified 'offset' bytes into the file with the specified
        // 'descriptor' to memory, and load into the specified 'address' of the
        // mapped area.  Return 0 on success, and a non-zero value otherwise.
        // The access permissions for mapping memory are defined by the
        // specified 'mode', which may be a combination of
        // 'MemoryUtil::k_ACCESS_READ', 'MemoryUtil::k_ACCESS_WRITE' and
        // 'MemoryUtil::k_ACCESS_EXECUTE'.  Note that on failure, the value of
        // 'address' is undefined.  Also note that mapping will succeed even if
        // there are fewer than 'offset + size' bytes in the specified file,
        // and an attempt to access the mapped memory beyond the end of the
        // file will result in undefined behavior (i.e., this function does not
        // grow the file to guarantee it can accommodate the mapped region).

    static int unmap(void *address, int size);
        // Unmap the memory mapping with the specified base 'address' and
        // specified 'size'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless this area with
        // 'address' and 'size' was previously mapped with a 'map' call.

    static int sync(char *address, int numBytes, bool syncFlag);
        // Synchronize the contents of the specified 'numBytes' of mapped
        // memory beginning at the specified 'address' with the underlying file
        // on disk.  If the specified 'syncFlag' is true, block until all
        // writes to nonvolatile media have actually completed, otherwise,
        // return once they have been scheduled.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined unless
        // 'address' is aligned on a page boundary, 'numBytes' is a multiple of
        // 'pageSize()', and '0 <= numBytes'.

    static Offset seek(FileDescriptor descriptor, Offset offset, int whence);
        // Set the file pointer associated with the specified 'descriptor'
        // (used by calls to the 'read' and 'write' system calls) according to
        // the specified 'whence' behavior:
        //..
        //   * If 'whence' is e_SEEK_FROM_BEGINNING, set the pointer to
        //     'offset' bytes from the beginning of the file.
        //   * If 'whence' is e_SEEK_FROM_CURRENT, advance the pointer by
        //     'offset' bytes
        //   * If 'whence' is e_SEEK_FROM_END, set the pointer to 'offset'
        //     bytes beyond the end of the file.
        //..
        // Return the new location of the file pointer, in bytes from the
        // beginning of the file, on success; and -1 otherwise.  The effect on
        // the file pointer is undefined unless the file is on a device capable
        // of seeking.  Note that 'seek' does not change the size of the file
        // if the pointer advances beyond the end of the file; instead, the
        // next write at the pointer will increase the file size.

    static int read(FileDescriptor descriptor, void *buffer, int numBytes);
        // Read the specified 'numBytes' bytes beginning at the file pointer of
        // the file with the specified 'descriptor' into the specified
        // 'bufffer'.  Return 'numBytes' on success; the number of bytes read
        // if there were not enough available; or a negative number on some
        // other error.

    static int remove(const bsl::string&  path, bool recursiveFlag = false);
    static int remove(const char         *path, bool recursiveFlag = false);
        // Remove the file or directory at the specified 'path'.  If the 'path'
        // refers to a directory and the optionally specified 'recursiveFlag'
        // is 'true', recursively remove all files and directories within the
        // specified directory before removing the directory itself.  Return 0
        // on success and a non-zero value otherwise.  Note that if 'path' is a
        // directory, and the directory is not empty, and recursive is 'false',
        // this method will fail.  Also note that if the function fails when
        // 'recursive' is 'true', it may or may not have removed *some* files
        // or directories before failing.

    static int rollFileChain(const bsl::string& path, int maxSuffix);
    static int rollFileChain(const char        *path, int maxSuffix);
        // Remove the file at the specified 'path' appended with the specified
        // 'maxSuffix' using a '.' as a separator.  Then move the files with
        // the suffixes '.1' to '.maxSuffix-1' so they have new suffixes from
        // '.2' to '.maxSuffix'.  Finally, move 'path' to 'path' with a '.1'
        // suffix.  Return 0 on success, and non-zero otherwise.

    static int move(const bsl::string&  oldPath, const bsl::string&  newPath);
    static int move(const char         *oldPath, const char         *newPath);
        // Move the file or directory at the specified 'oldPath' to the
        // specified 'newPath'.  If there is a file or directory at 'newPath',
        // it will be removed and replaced.  In that case, 'newPath' must refer
        // to the same type of filesystem item as 'oldPath' - that is, they
        // must both be directories or both be files.  Return 0 on success, and
        // a non-zero value otherwise.

    static int write(FileDescriptor  descriptor,
                     const void     *buffer,
                     int             numBytes);
        // Write the specified 'numBytes' from the specified 'buffer' address
        // to the file with the specified 'descriptor'.  Return 'numBytes' on
        // success; the number of bytes written if space was exhausted; or a
        // negative value on some other error.

    static int growFile(
                  FileDescriptor descriptor,
                  Offset         size,
                  bool           reserveFlag = false,
                  bsl::size_t    increment = k_DEFAULT_FILE_GROWTH_INCREMENT);
        // Grow the file with the specified 'descriptor' to the size of at
        // least the specified 'size' bytes.  Return 0 on success, and a
        // non-zero value otherwise.  If the optionally specified 'reserveFlag'
        // is true, make sure the space on disk is preallocated and not
        // allocated on demand, preventing a possible out-of-disk-space error
        // when accessing the data on file systems with sparse file support.
        // Preallocation is done by writing unspecified data to file in blocks
        // of the optionally specified 'increment'.  Note that if the size of
        // the file is greater than or equal to 'size', this function has no
        // effect.  Also note that the contents of the newly grown portion of
        // the file is undefined.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // struct FilesystemUtil
                           // ---------------------

// CLASS METHODS
inline
int FilesystemUtil::createDirectories(const bsl::string& path,
                                      bool               isLeafDirectoryFlag)
{
    return createDirectories(path.c_str(), isLeafDirectoryFlag);
}

inline
void FilesystemUtil::visitPaths(
                          const bsl::string&                           pattern,
                          const bsl::function<void(const char *path)>& visitor)
{
    return visitPaths(pattern.c_str(), visitor);
}

inline
FilesystemUtil::FileDescriptor FilesystemUtil::open(
                                             const bsl::string& path,
                                             FileOpenPolicy     openPolicy,
                                             FileIOPolicy       ioPolicy,
                                             FileTruncatePolicy truncatePolicy)
{
    return open(path.c_str(), openPolicy, ioPolicy, truncatePolicy);
}

inline
bool FilesystemUtil::exists(const bsl::string& path)
{
    return exists(path.c_str());
}

inline
bool FilesystemUtil::isRegularFile(const bsl::string& path,
                                   bool               followLinksFlag)
{
    return isRegularFile(path.c_str(), followLinksFlag);
}

inline
bool FilesystemUtil::isDirectory(const bsl::string& path,
                                 bool               followLinksFlag)
{
    return isDirectory(path.c_str(), followLinksFlag);
}

inline
int FilesystemUtil::getLastModificationTime(bdlt::Datetime     *time,
                                            const bsl::string&  path)
{
    BSLS_ASSERT_SAFE(time);

    return getLastModificationTime(time, path.c_str());
}

inline
int FilesystemUtil::remove(const bsl::string& path, bool recursiveFlag)
{
    return remove(path.c_str(), recursiveFlag);
}

inline
int FilesystemUtil::rollFileChain(const bsl::string& path, int maxSuffix)
{
    return rollFileChain(path.c_str(), maxSuffix);
}

inline
int FilesystemUtil::move(const bsl::string& oldPath,
                         const bsl::string& newPath)
{
    return move(oldPath.c_str(), newPath.c_str());
}

inline
int FilesystemUtil::setWorkingDirectory(const bsl::string& path)
{
    return setWorkingDirectory(path.c_str());
}

inline
FilesystemUtil::Offset FilesystemUtil::getFileSize(const bsl::string& path)
{
    return getFileSize(path.c_str());
}

inline
FilesystemUtil::Offset FilesystemUtil::getAvailableSpace(
                                                       const bsl::string& path)
{
    return getAvailableSpace(path.c_str());
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
