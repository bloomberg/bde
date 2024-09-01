// bdls_filesystemutil_transitionaluniximputil.h                      -*-C++-*-
#ifndef INCLUDED_BDLS_FILESYSTEMUTIL_TRANSITIONALUNIXIMPUTIL
#define INCLUDED_BDLS_FILESYSTEMUTIL_TRANSITIONALUNIXIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide testable `bdls::FilesystemUtil` operations for some Unixes.
//
//@CLASSES:
//  bdls::FileystemUtil_TransitionalUnixImpUtil: testable file-system utilities
//
//@SEE_ALSO: bdls_filesystemutil, bdls_filesystemutil_unixplatform
//
//@DESCRIPTION: This subordinate component to `bdls_filesystemutil` provides a
// utlity `struct` template, `bdls::FilesystemUtil_TransitionalUnixImpUtil` for
// implementing some of `bdls::FileystemUtil`s functions on Unix platforms in
// the transitional-compilation environemnt.
// `bdls::FilesystemUtil_TransitionalUnixImpUtil` accesses Unix functions and
// types through its template parameter, `UNIX_INTERFACE`, in order to allow
// tests to supply mock Unix interfaces.
//
///The Transitional-Compilation Environment
///----------------------------------------
// Many Unix systems provide a non-standard "transitional-compilation
// environment", which is a compilation mode that supports large, 64-bit file
// operations in 32-bit (or 64-bit) programs.  For more information about this
// and other large-file compilation modes, please see the `lfcompile64` and/or
// `lfcompile` manual pages provided for the operating system of interest.
// Note that some Unix systems may not support a transitional-compilation
// environment, and some that do may provide descriptions of this environment
// in documentation outside of a `lfcompile64` or `lfcompile` manual page.
// Furthermore, some Unix systems natively provide 64-bit file operations for
// 32-bit programs, in which case a transitional-compilation environment is not
// necessary.
//
// Another subordinate component of `bdls_filesystemutil`,
// `bdls_filesystemutil_unixplatform`, is principally concerned with the
// detection of the transitional-compilation environment on any Unix platform
// supported by the BDE library.

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_timeunitratio.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdls {

               // =============================================
               // struct FilesystemUtil_TransitionalUnixImpUtil
               // =============================================

/// This component-private utility `struct` provides a namespace for a suite
/// of functions that `FilesystemUtil` uses as implementation details.
/// These functions have a `UNIX_INTERFACE` template parameter, which
/// provides access to the entities that transitional-compilation
/// environment Unix systems declare, and that the function implementations
/// need.
///
/// Note that, on some Unix platforms and some build configurations, the
/// `stat` struct does not have an `st_mtime` field, and `st_mtime` is a
/// macro that emulates the access of the field.  Similarly, on some Unix
/// platforms and some build configurations, the `stat` structure does not
/// have an `st_mtim` field or the `st_mtim` struct does not have a
/// `tv_nsec` (or, for some versions of Solaris, `__tv_nsec`) field, in
/// which case the `get_st_mtim_nsec` function returns zero.  For more
/// information, please see the specification of the `sys/stat.h` header
/// from IEEE Std
/// 1003.1-2017, which provides information about the evolution of the
/// `stat` struct in the POSIX specification
/// (https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/).
///
/// The program is ill-formed unless the specified `UNIX_INTERFACE` is a
/// class type that meets the following requirements:
///
/// * `UNIX_INTERFACE::off64_t` is a type alias to the `off64_t` type
///   provided by the `sys/types.h` header.
/// * `UNIX_INTERFACE::stat64` is a type alias to the `stat64` type
///   provided by the `sys/stat.h` header.
/// * `UNIX_INTERFACE::time_t` is a type alias to the `time_t` type
///   provided by the `sys/types.h` header.
/// * `UNIX_INTERFACE::get_st_mtim_nsec` is a public, static member
///   function that has `long (const stat& stat)` type and whose contract
///   is to return the value of the `st_mtim.tv_nsec` field of the
///   specified `stat` struct.
/// * `UNIX_INTERFACE::get_st_mtime` is a public, static member function
///   that has `time_t (const stat& stat)` type and whose contract is to
///   return the value of the `st_mtime` field of the specified `stat`
///   struct.
/// * `UNIX_INTERFACE::get_st_size` is a public, static member function
///   that has `off64_t (const stat& stat)` type and whose contract is to
///   return the value of the `st_size` field of the specified `stat`
///   struct.  Note that this function is required in order to
///   access the data members of a `stat` struct in a manner consistent
///   with the requirements of `get_st_mtime`.
/// * `UNIX_INTERFACE::fstat64` is a public, static member function that
///   has `int (int fildes, stat64 *buf)` type and whose contract is to
///   return the result of `::fstat64(fildes, buf)`, where `::fstat64` is
///   the function provided by the `sys/stat.h` header.
template <class UNIX_INTERFACE>
struct FilesystemUtil_TransitionalUnixImpUtil {

    // TYPES

    /// `FileDescriptor` is an alias for the operating system's native file
    /// descriptor / file handle type.
    typedef int FileDescriptor;

    /// `Offset` is an alias for a signed integral type, and represents the
    /// offset of a location in a file.
    typedef typename UNIX_INTERFACE::off64_t Offset;

  private:
    // PRIVATE TYPES

    /// `off_t` is an alias to the `off64_t` type provided by the
    /// `sys/types.h` header.  It is a signed integral type used to
    /// represent quantities of bytes.
    typedef typename UNIX_INTERFACE::off64_t off64_t;

    /// `stat64` is an alias to the `stat64` `struct` provided by the
    /// `sys/stat.h` header.
    typedef typename UNIX_INTERFACE::stat64 stat64;

    /// `time_t` is an alias to the `time_t` type provided by the
    /// `sys/types.h` header.  It represents a time point as number of
    /// seconds since January 1st 1970 in Coordinated Universal Time.
    typedef typename UNIX_INTERFACE::time_t time_t;

    // PRIVATE CLASS METHODS

    /// Return the value of the `st_mtim.nsec` field of the specified `stat`
    /// struct.
    static long get_st_mtim_nsec(const stat64& stat);

    /// Return the value of the `st_mtime` data member of the specified
    /// `stat`.
    static time_t get_st_mtime(const stat64& stat);

    /// Return the value of the `st_size` data member of the specified
    /// `stat`.  Note that this function is provided in order to create a
    /// consistent interface for accessing the data members of a `stat64`
    /// struct with `get_st_mtime`.
    static off64_t get_st_size(const stat64& stat);

    /// Invoke and return the result of `::fstat64(fildes, buf)` with the
    /// specified  `fildes` and `buf`, where `::fstat64` is the function
    /// provided by the `sys/stat.h` header.
    static int fstat64(int fildes, stat64 *buf);

  public:
    // CLASS METHODS

    /// Return the size, in bytes, of the file with the specified
    /// `descriptor`, or a negative value if an error occurs.
    static Offset getFileSize(FileDescriptor descriptor);

    /// Load into the specified `time` the last modification time of the
    /// file with the specified `descriptor`, as reported by the filesystem.
    /// Return 0 on success, and a non-zero value otherwise.  Note that the
    /// time is reported in UTC.
    static int getLastModificationTime(bdlt::Datetime *time,
                                       FileDescriptor  descriptor);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

               // ---------------------------------------------
               // struct FilesystemUtil_TransitionalUnixImpUtil
               // ---------------------------------------------

// PRIVATE CLASS METHODS
template <class UNIX_INTERFACE>
long
FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::get_st_mtim_nsec(
                                                            const stat64& stat)
{
    return UNIX_INTERFACE::get_st_mtim_nsec(stat);
}

template <class UNIX_INTERFACE>
typename UNIX_INTERFACE::time_t
FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::get_st_mtime(
                                                            const stat64& stat)
{
    return UNIX_INTERFACE::get_st_mtime(stat);
}

template <class UNIX_INTERFACE>
typename UNIX_INTERFACE::off64_t
FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::get_st_size(
                                                            const stat64& stat)
{
    return UNIX_INTERFACE::get_st_size(stat);
}

template <class UNIX_INTERFACE>
int FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::fstat64(
                                                                int     fildes,
                                                                stat64 *buf)
{
    return UNIX_INTERFACE::fstat64(fildes, buf);
}

// CLASS METHODS
template <class UNIX_INTERFACE>
typename FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::Offset
FilesystemUtil_TransitionalUnixImpUtil<UNIX_INTERFACE>::getFileSize(
                                                     FileDescriptor descriptor)
{
    stat64 statResult;
    const int rc = fstat64(descriptor, &statResult);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return get_st_size(statResult);
}

template <class UNIX_INTERFACE>
int FilesystemUtil_TransitionalUnixImpUtil<
    UNIX_INTERFACE>::getLastModificationTime(bdlt::Datetime *time,
                                             FileDescriptor  descriptor)
{
    stat64 statResult;
    int rc = fstat64(descriptor, &statResult);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    bdlt::Datetime result = bdlt::EpochUtil::epoch();
    rc = result.addSecondsIfValid(get_st_mtime(statResult));
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    long nanoseconds = get_st_mtim_nsec(statResult);
    BSLS_ASSERT_SAFE((0 <= nanoseconds) &&
                     (nanoseconds <
                      bdlt::TimeUnitRatio::k_NANOSECONDS_PER_SECOND));
    rc = result.addMicrosecondsIfValid(
          nanoseconds / bdlt::TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND_32);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    *time = result;
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
