// bdesu_filesystemutil.h                                             -*-C++-*-
#ifndef INCLUDED_BDESU_FILESYSTEMUTIL
#define INCLUDED_BDESU_FILESYSTEMUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@purpose: provide methods for file system access.
//
//@classes:
//  bdesu_filesystemutil: namespace for file system access methods
//
//@see_also: bdesu_fileutil, bdesu_pathutil
//
//@author: andrei basov (abasov), oleg semenov (osemenov),
// hyman rosen (hrosen4), alexander beels (abeels)
//
//@description: this component provides a utf8-encoded platform-independent
// interface to filesystem utility methods.  each function in the
// 'bdesu_filesystemutil' namespace is a thin wrapper on top of the operating
// system's own file system access functions, providing a consistent and
// unambiguous interface for handling files on all supported platforms.
//
// all methods in this component require that all file and path names be passed
// as utf-8-encoded strings.  similarly, methods that return file and path
// names return utf-8-encoded strings.  because this component has no direct
// knowledge of the underlying file system's native encoding, these
// requirements are *assumed* on posix platforms, and *enforced* on windows
// platforms.  see the section "platform-specific file name encoding caveats"
// below.
//
///policies for 'open'
///-------------------
// the behavior of the 'open' method is governed by three sets of enumerations:
//
///open/create policy: 'bdesu_filesystemutil::fileopenpolicy'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdesu_filesystemutil::fileopenpolicy' governs whether 'open' creates a new
// file or opens an existing one.  the following values are possible:
//
//: o e_open          : open an existing file.
//:
//: o e_create        : create a new file.
//:
//: o e_open_or_create: open a file if it exists, and create a new file
//:                     otherwise.
//
///input/output access policy: 'bdesu_filesystemutil::fileiopolicy'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdesu_filesystemutil::fileiopolicy' governs what input/output operations
// are allowed on a file after it is opened.  the following values are
// possible:
//
//: o e_read_only  : allow reading only.
//: o e_write_only : allow writing only.
//: o e_read_write : allow both reading and writing.
//: o e_append_only: allow appending to end-of-file only.
//: o e_read_append: allow both reading and appending to end-of-file.
//
///truncation policy: 'bdesu_filesystemutil::filetruncatepolicy'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdesu_filesystemutil::filetruncatepolicy' governs whether 'open' deletes
// the existing contents of a file when it is opened.  the following values are
// possible:
//
//: o e_truncate: delete the file's contents.
//: o e_keep    : keep the file's contents.
//
///starting points for 'seek'
///--------------------------
// the behavior of the 'seek' method is governed by an enumeration that
// determines the point from which the seek operation starts:
//
//: o e_seek_from_beginning: seek from the beginning of the file.
//: o e_seek_from_current  : seek from the current position in the file.
//: o e_seek_from_end      : seek from the end of the file.
//
///platform-specific file locking caveats
///--------------------------------------
// locking has the following caveats for the following operating systems:
//:
//: o on posix, closing a file releases all locks on all file descriptors
//:   referring to that file within the current process.  [doc 1] [doc 2]
//:
//: o on posix, the child of a fork does not inherit the locks of the parent
//:   process.  [doc 1] [doc 2]
//:
//: o on at least some flavors of unix, you can't lock a file for writing using
//:   a file descriptor opened in read-only mode.
//
///platform-specific atomicity caveats
///-----------------------------------
// the 'bdesu_filesystemutil::read' and 'bdesu_filesystemutil::write' methods
// add no atomicity guarantees for reading and writing to those provided (if
// any) by the underlying platform's methods for reading and writing (see
// 'http://lwn.net/articles/180387/').
//
///platform-specific file name encoding caveats
///--------------------------------------------
// file-name encodings have the following caveats for the following operating
// systems:
//
//: o on windows, methods of 'bdesu_filesystemutil' that take a file or
//:   directory name or pattern as a 'char*' or 'bsl::string' type assume that
//:   the name is encoded in utf-8. the routines attempt to convert the name to
//:   a utf-16 'wchar_t' string via 'bdede_charconvertutf16::utf8toutf16', and
//:   if the conversion succeeds, call the windows wide-character 'w' apis with
//:   the utf-16 name. if the conversion fails, the methods fail. similarly,
//:   file searches returning file names call the windows wide-character 'w'
//:   apis and convert the resulting utf-16 names to utf-8.
//:
//:   narrow-character file names in other encodings, containing characters
//:   with values in the range 128 - 255, will likely result in files being
//:   created with names that appear garbled.
//:
//:   neither 'utf8toutf16' nor the windows 'w' apis do any normalization of
//:   the utf-16 strings resulting from utf-8 conversion, and it is therefore
//:   possible to have sets of file names which display as identical strings
//:   but are treated as different names by the file system.
//:
//: o on posix, a file name or pattern supplied to methods of
//:   'bdesu_filesystemutil' as a 'char*' or 'bsl::string' type is assumed to
//:   be encoded in utf-8, and is passed unchanged to the underlying system
//:   file apis, which are assumed to be interfacing with a filesystem encoded
//:   in utf-8. because the file names and patterns are passed unchanged,
//:   'bdesu_filesystemutil' methods will work correctly on posix with other
//:   encodings, providing that the strings supplied to the methods are in the
//:   same encoding as the underlying file system.
//
///file truncation caveats
///-----------------------
// in order to provide consistent behavior across both posix and windows
// platforms, when the 'open' method is called file truncation is allowed only
// if the client requests an 'openpolicy' containing the word 'create' and/or
// an 'iopolicy' containing the word 'write'.
//
///usage
///-----
///example 1: general usage
/// - - - - - - - - - - - -
// in this example, we start with a (relative) native path to a directory
// containing log files:
//..
//  #ifdef bsls_platform_os_windows
//    bsl::string logpath = "temp.1\\logs";
//  #else
//    bsl::string logpath = "temp.1/logs";
//  #endif
//..
// suppose that we want to separate files into "old" and "new" subdirectories
// on the basis of modification time.  we will provide paths representing these
// locations, and create the directories if they do not exist:
//..
//  bsl::string oldpath(logpath), newpath(logpath);
//  bdesu_pathutil::appendraw(&oldpath, "old");
//  bdesu_pathutil::appendraw(&newpath, "new");
//  int rc = bdesu_filesystemutil::createdirectories(oldpath.c_str(), true);
//  assert(0 == rc);
//  rc = bdesu_filesystemutil::createdirectories(newpath.c_str(), true);
//  assert(0 == rc);
//..
// we know that all of our log files match the pattern "*.log", so let's search
// for all such files in the log directory:
//..
//  bdesu_pathutil::appendraw(&logpath, "*.log");
//  bsl::vector<bsl::string> logfiles;
//  bdesu_filesystemutil::findmatchingpaths(&logfiles, logpath.c_str());
//..
// now for each of these files, we will get the modification time.  files that
// are older than 2 days will be moved to "old", and the rest will be moved to
// "new":
//..
//  bdet_datetime modtime;
//  bsl::string   filename;
//  for (bsl::vector<bsl::string>::iterator it = logfiles.begin();
//                                                it != logfiles.end(); ++it) {
//    assert(0 == bdesu_filesystemutil::getlastmodificationtime(&modtime,
//                                                              *it));
//    assert(0 == bdesu_pathutil::getleaf(&filename, *it));
//    bsl::string *whichdirectory =
//                2 < (bdetu_systemtime::nowasdatetime() - modtime).totaldays()
//                ? &oldpath
//                : &newpath;
//    bdesu_pathutil::appendraw(whichdirectory, filename.c_str());
//    assert(0 == bdesu_filesystemutil::move(it->c_str(),
//                                           whichdirectory->c_str()));
//    bdesu_pathutil::popleaf(whichdirectory);
//  }
//..
///example 2: using 'bdesu_filesystemutil::visitpaths'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdesu_filesystemutil::visitpaths' enables clients to define a functor to
// operate on file paths that match a specified pattern.  in this example, we
// create a function that can be used to filter out files that have a last
// modified time within a particular time frame.
//
// first we define our filtering function:
//..
//  void getfileswithintimeframe(bsl::vector<bsl::string> *vector,
//                               const char               *item,
//                               const bdet_datetime&      start,
//                               const bdet_datetime&      end)
//  {
//      bdet_datetime datetime;
//      int ret = bdesu_filesystemutil::getlastmodificationtime(&datetime,
//                                                               item);
//
//      if (ret) {
//          return;                                                   // return
//      }
//
//      if (datetime < start || datetime > end) {
//          return;                                                   // return
//      }
//
//      vector->push_back(item);
//  }
//..
// then, with the help of 'bdesu_filesystemutil::visitpaths' and
// 'bdef_bindutil::bind', we create a function for finding all file paths that
// match a specified pattern and have a last modified time within a specified
// start and end time (both specified as a 'bdet_datetime'):
//..
//  void findmatchingfilesintimeframe(bsl::vector<bsl::string> *result,
//                                    const char               *pattern,
//                                    const bdet_datetime&      start,
//                                    const bdet_datetime&      end)
//  {
//      result->clear();
//      bdesu_filesystemutil::visitpaths(
//                                pattern,
//                                bdef_bindutil::bind(&getfileswithintimeframe,
//                                                    result,
//                                                    bdef_placeholders::_1,
//                                                    start,
//                                                    end));
//  }
//..

#ifndef included_bdescm_version
#include <bdescm_version.h>
#endif

#ifndef included_bdesu_memoryutil
#include <bdesu_memoryutil.h>
#endif

#ifndef included_bdef_function
#include <bdef_function.h>
#endif

#ifndef included_bsls_assert
#include <bsls_assert.h>
#endif

#ifndef included_bsls_platform
#include <bsls_platform.h>
#endif

#ifndef included_bsl_string
#include <bsl_string.h>
#endif

#ifndef included_bsl_vector
#include <bsl_vector.h>
#endif

#ifndef included_bsl_cstddef
#include <bsl_cstddef.h>
#endif

#ifndef bsls_platform_os_windows
#ifndef included_sys_types
#include <sys/types.h>
#define included_sys_types
#endif
#endif

namespace BloombergLP {

class bdet_Datetime;

                        // ===========================
                        // struct bdesu_FilesystemUtil
                        // ===========================

struct bdesu_FilesystemUtil {
    // The static methods of this structure provide platform-independent
    // mechanisms for file system access.

    // TYPES
#ifdef BSLS_PLATFORM_OS_WINDOWS
    typedef void *HANDLE;
    typedef HANDLE FileDescriptor;
    typedef __int64 Offset;
    static const Offset OFFSET_MAX = _I64_MAX;
    static const Offset OFFSET_MIN = _I64_MIN;
#else
    typedef int     FileDescriptor;
#if defined(BSLS_PLATFORM_OS_FREEBSD) \
 || defined(BSLS_PLATFORM_OS_DARWIN)  \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
    // 'off_t' is 64-bit on Darwin/FreeBSD/cygwin (even when running 32-bit),
    // so they do not have an 'off64_t' type.

    typedef off_t Offset;
#else
    typedef off64_t Offset;
#endif
#ifdef BSLS_PLATFORM_CPU_64_BIT
    static const Offset OFFSET_MAX =  (9223372036854775807L);
    static const Offset OFFSET_MIN = (-9223372036854775807L-1);
#else
    static const Offset OFFSET_MAX =  (9223372036854775807LL);
    static const Offset OFFSET_MIN = (-9223372036854775807LL-1);
#endif
#endif

    enum Whence {
        e_SEEK_FROM_BEGINNING = 0,  // Seek from beginning of file.
        e_SEEK_FROM_CURRENT   = 1,  // Seek from current position.
        e_SEEK_FROM_END       = 2   // Seek from end of file.
    };

    enum {
        k_DEFAULT_GROW_BUFFER_SIZE = 65536
    };

    enum {
        k_ERROR_LOCKING_CONFLICT = 1
    };

    enum FileOpenPolicy {
        e_OPEN,           // Open an existing file.

        e_CREATE,         // Create a new file.

        e_OPEN_OR_CREATE  // Open a file if it exists, and create a new file
                          // otherwise.
    };

    enum FileIOPolicy {
        e_READ_ONLY,    // Allow reading only.
        e_WRITE_ONLY,   // Allow writing only.
        e_APPEND_ONLY,  // Allow appending to end-of-file only.
        e_READ_WRITE,   // Allow both reading and writing.
        e_READ_APPEND   // Allow both reading and appending to end-of-file.
    };

    enum FileTruncatePolicy {
        e_TRUNCATE,  // Delete the file's contents on open.
        e_KEEP       // Keep the file's contents.
    };

    // CLASS DATA
    static const FileDescriptor INVALID_FD;

    // CLASS METHODS
    static FileDescriptor open(
                             const char              *path,
                             enum FileOpenPolicy      openPolicy,
                             enum FileIOPolicy        ioPolicy,
                             enum FileTruncatePolicy  truncatePolicy = e_KEEP);
    static FileDescriptor open(
                             const bsl::string&       path,
                             enum FileOpenPolicy      openPolicy,
                             enum FileIOPolicy        ioPolicy,
                             enum FileTruncatePolicy  truncatePolicy = e_KEEP);
        // Open the file at the specified 'path', using the specified
        // 'openPolicy' to determine whether to open an existing file or create
        // a new file, and using the specified 'ioPolicy' to determine whether
        // the file will be opened for reading, writing, or both. Optionally
        // specify a 'truncatePolicy' to determine whether any contents of the
        // file will be deleted before 'open' returns.  If 'truncatePolicy' is
        // not supplied, the value 'e_KEEP' will be used.  Return a valid
        // 'FileDesriptor' for the file on success, or 'INVALID_FD' otherwise.
        // If 'openPolicy' is 'e_OPEN', the file will be opened if it exists,
        // and 'open' will fail otherwise.  If 'openPolicy' is 'e_CREATE', and
        // no file exists at 'path', a new file will be created, and 'open'
        // will fail otherwise.  If 'openPolicy' is 'e_OPEN_OR_CREATE', the
        // file will be opened if it exists, and a new file will be created
        // otherwise.  If 'ioPolicy' is 'e_READ_ONLY', the returned
        // 'FileDescriptor' will allow only read operations on the file.  If
        // 'ioPolicy' is 'e_WRITE_ONLY' or 'e_APPEND_ONLY', the returned
        // 'FileDescriptor' will allow only write operations on the file.  If
        // 'ioPolicy' is 'e_READ_WRITE' or 'e_READ_APPEND', the returned
        // 'FileDescriptor' will allow both read and write operations on the
        // file.  Additionally, if 'ioPolicy' is 'e_APPEND_ONLY' or
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
        // specified for 'openPolicy'
        // or 'ioPolicy':
        //: o 'e_CREATE'
        //: o 'e_OPEN_OR_CREATE'
        //: o 'e_WRITE_ONLY
        //: o 'e_READ_WRITE'

    static int close(FileDescriptor descriptor);
        // Close the specified 'descriptor'.  Return 0 on success and a
        // non-zero value otherwise.

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
                              bool                followLinks = false);
    static bool isRegularFile(const char         *path,
                              bool                followLinks = false);
        // Return 'true' if there currently exists a regular file at the
        // specified 'path', and 'false' otherwise.  If there is a symbolic
        // link at 'path', follow it only if the optionally-specified
        // 'followLinks' flag is 'true' (otherwise, return 'false' as the
        // symbolic link itself is not a regular file irrespective of the file
        // to which it points).  Platform-specific note: On POSIX, this is a
        // positive test on the "regular file" mode; on Windows, this is a
        // negative test on the "directory" attribute, i.e., on Windows,
        // everything that exists and is not a directory is a regular file.

    static bool isDirectory(const bsl::string&  path,
                            bool                followLinks = false);
    static bool isDirectory(const char         *path,
                            bool                followLinks = false);
        // Return 'true' if there currently exists a directory at the specified
        // 'path', and 'false' otherwise.  If there is a symbolic link at
        // 'path', follow it only if the optionally specified 'followLinks'
        // flag is 'true' (otherwise return 'false').  Platform-specific note:
        // On Windows, a "shortcut" is not a symbolic link.

    static int getLastModificationTime(bdet_Datetime      *time,
                                       const bsl::string&  path);
    static int getLastModificationTime(bdet_Datetime      *time,
                                       const char         *path);
        // Load into the specified 'time' the last modification time of the
        // file at the specified 'path', as reported by the filesystem.  Return
        // 0 on success, and a non-zero value otherwise.

    // TBD: write setModificationTime() when SetFileInformationByHandle()
    // becomes available on our standard Windows platforms.

    static int createDirectories(const bsl::string&  path,
                                 bool                isLeafDirectory = false);
    static int createDirectories(const char         *path,
                                 bool                isLeafDirectory = false);
        // Create any directories in 'path' which do not exist.  If the
        // optionally specified 'isLeafDirectory' is 'true', then treat the
        // last filename in the path as a directory and attempt to create it.
        // Otherwise, treat the last filename as a regular file and ignore it.
        // Return 0 on success, and a non-zero value if any needed directories
        // in the path could not be created.

    static void visitPaths(
                     const bsl::string&                               pattern,
                     const bdef_Function<void(*)(const char *path)>&  visitor);
    static void visitPaths(
                     const char                                      *pattern,
                     const bdef_Function<void(*)(const char *path)>&  visitor);
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

    static Offset getAvailableSpace(FileDescriptor fd);
        // Return the number of bytes available for allocation in the file
        // system where the file with the specified descriptor 'fd' resides,
        // or a negative value if an error occurs.

    static Offset getFileSize(const bsl::string&  path);
    static Offset getFileSize(const char         *path);
        // Return the size, in bytes, of the file or directory at the specified
        // 'path', or a negative value if an error occurs.  Note that the size
        // of a symbolic link is the size of the file or directory to which it
        // points.

    static Offset getFileSizeLimit();
        // Return the file size limit for this process, 'OFFSET_MAX' if no
        // limit is set, or a negative value if an error occurs.  Note that
        // if you are doing any calculations involving the returned value, it
        // is recommended to check for 'OFFSET_MAX' specifically to avoid
        // integer overflow in your calculations.

    static int lock(FileDescriptor fd, bool lockWrite);
        // Acquire a lock for the file with the specified 'fd'.  If 'lockWrite'
        // is true, acquire an exclusive write lock; otherwise acquire a
        // (possibly) shared read lock.  The calling thread will block until
        // the lock is acquired.  Return 0 on success, and a non-zero value
        // otherwise.  Note that this operation locks the indicated file for
        // use by the current *process*, but the behavior is unspecified (and
        // platform-dependent) when either attempting to lock 'fd' multiple
        // times, or attempting to lock another descriptor referring to the
        // same file, within a single process.

    static int tryLock(FileDescriptor fd, bool lockWrite);
        // Acquire a lock for the file with the specified 'fd' if it is
        // currently available.  If 'lockWrite' is true, acquire an exclusive
        // write lock unless another process has any type of lock on the file.
        // If 'lockWrite' is false, acquire a shared read lock unless a process
        // has a write lock.  This method will not block.  Return 0 on success,
        // 'k_ERROR_LOCKING_CONFLICT' if the platform reports the lock could
        // not be acquired because another process holds a conflicting lock,
        // and a negative value for any other kind of error.  Note that this
        // operation locks the indicated file for the current *process*, but
        // the behavior is unspecified (and platform-dependent) when either
        // attempting to lock 'fd' multiple times, or attempting to lock
        // another descriptor referring to the same file, within a single
        // process.

    static int unlock(FileDescriptor fd);
        // Release any lock this process holds on the file with the specified
        // 'fd'.  Return 0 on success, and a non-zero value otherwise.

    static int map(FileDescriptor   fd,
                   void           **addr,
                   Offset           offset,
                   int              size,
                   int              mode);
        // Map the region of the specified 'size' bytes, starting at the
        // specified 'offset' bytes into the file with the specified 'fd'
        // descriptor to memory, and load into the specified 'addr' the address
        // of the mapped area.  Return 0 on success, and a non-zero value
        // otherwise.  The access permissions for mapping memory are defined by
        // the specified 'mode', which may be a combination of
        // 'bdesu_MemoryUtil::BDESU_ACCESS_READ',
        // 'bdesu_MemoryUtil::BDESU_ACCESS_WRITE' and
        // 'bdesu_MemoryUtil::BDESU_ACCESS_EXECUTE'.  Note that on failure, the
        // value of 'addr' is undefined.  Also note that mapping will succeed
        // even if there are fewer than 'offset + size' bytes in the specified
        // file, and an attempt to access the mapped memory beyond the end of
        // the file will result in undefined behavior (i.e., this function does
        // not grow the file to guarantee it can accommodate the mapped
        // region).

    static int unmap(void *addr, int size);
        // Unmap the memory mapping with the specified base address 'addr' and
        // specified 'size'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless the area with the
        // specified 'address' and 'size' was previously mapped with a 'map'
        // call.

    static int sync(char *addr, int numBytes, bool sync);
        // Synchronize the contents of the specified 'numBytes' of mapped
        // memory beginning at the specified 'addr' with the underlying file
        // on disk.  If the specified 'sync' flag is true, block until all
        // writes to nonvolatile media have actually completed; otherwise,
        // return once they have been scheduled.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined unless 'addr'
        // is aligned on a page boundary, 'numBytes' is a multiple of
        // 'pageSize()', and '0 <= numBytes'.

    static Offset seek(FileDescriptor fd, Offset offset, int whence);
        // Set the file pointer associated with the specified 'fd' file
        // descriptor (used by calls to the 'read' and 'write' system calls)
        // according to the specified 'whence' behavior:
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

    static int read(FileDescriptor fd, void *buf, int numBytes);
        // Read 'numBytes' bytes beginning at the file pointer of the file with
        // the specified 'fd' into the specified 'buf' buffer.  Return
        // 'numBytes' on success; the number of bytes read if there were not
        // enough available; or a negative number on some other error.

    static int remove(const bsl::string&  path, bool recursive = false);
    static int remove(const char         *path, bool recursive = false);
        // Remove the file or directory at the specified 'path'.  If the 'path'
        // refers to a directory and the optionally specified 'recursive' flag
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

    static int write(FileDescriptor fd, const void *buf, int numBytes);
        // Write 'numBytes' beginning at the specified 'buf' address to the
        // file with the specified 'fd'.  Return 'numBytes' on success; the
        // number of bytes written if space was exhausted; or a negative value
        // on some other error.

    static int grow(FileDescriptor fd,
                    Offset         size,
                    bool           reserve = false,
                    bsl::size_t    bufferSize = k_DEFAULT_GROW_BUFFER_SIZE);
        // Grow the file with the specified 'fd' to the size of at least 'size'
        // bytes.  Return 0 on success, and a non-zero value otherwise.  If the
        // optionally specified 'reserve' flag is true, make sure the space on
        // disk is preallocated and not allocated on demand, preventing a
        // possible out-of-disk-space error when accessing the data on file
        // systems with sparse file support.  Preallocation is done by writing
        // unspecified data to file in blocks of the specified 'bufferSize'.
        // Note that if the size of the file is greater than or equal to
        // 'size', this function has no effect.  Also note that the contents of
        // the newly grown portion of the file is undefined.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // struct bdesu_FilesystemUtil
                        // ---------------------------

// CLASS METHODS
inline
int bdesu_FilesystemUtil::createDirectories(const bsl::string& path,
                                            bool               isLeafDirectory)
{
    return createDirectories(path.c_str(), isLeafDirectory);
}

inline
void bdesu_FilesystemUtil::visitPaths(
                       const bsl::string&                              pattern,
                       const bdef_Function<void(*)(const char *path)>& visitor)
{
    return visitPaths(pattern.c_str(), visitor);
}

inline
bdesu_FilesystemUtil::FileDescriptor
bdesu_FilesystemUtil::open(const bsl::string&      path,
                           enum FileOpenPolicy     openPolicy,
                           enum FileIOPolicy       ioPolicy,
                           enum FileTruncatePolicy truncatePolicy)
{
    return open(path.c_str(), openPolicy, ioPolicy, truncatePolicy);
}

inline
bool bdesu_FilesystemUtil::exists(const bsl::string& path)
{
    return exists(path.c_str());
}

inline
bool bdesu_FilesystemUtil::isRegularFile(const bsl::string& path,
                                         bool               followLinks)
{
    return isRegularFile(path.c_str(), followLinks);
}

inline
bool bdesu_FilesystemUtil::isDirectory(const bsl::string& path,
                                       bool               followLinks)
{
    return isDirectory(path.c_str(), followLinks);
}

inline
int bdesu_FilesystemUtil::getLastModificationTime(bdet_Datetime      *time,
                                                  const bsl::string&  path)
{
    BSLS_ASSERT_SAFE(time);

    return getLastModificationTime(time, path.c_str());
}

inline
int bdesu_FilesystemUtil::remove(const bsl::string& path, bool recursive)
{
    return remove(path.c_str(), recursive);
}

inline
int bdesu_FilesystemUtil::rollFileChain(const bsl::string& path, int maxSuffix)
{
    return rollFileChain(path.c_str(), maxSuffix);
}

inline
int bdesu_FilesystemUtil::move(const bsl::string& oldPath,
                               const bsl::string& newPath)
{
    return move(oldPath.c_str(), newPath.c_str());
}

inline
int bdesu_FilesystemUtil::setWorkingDirectory(const bsl::string& path)
{
    return setWorkingDirectory(path.c_str());
}

inline
bdesu_FilesystemUtil::Offset bdesu_FilesystemUtil::getFileSize(
                                                       const bsl::string& path)
{
    return getFileSize(path.c_str());
}

inline
bdesu_FilesystemUtil::Offset bdesu_FilesystemUtil::getAvailableSpace(
                                                       const bsl::string& path)
{
    return getAvailableSpace(path.c_str());
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
