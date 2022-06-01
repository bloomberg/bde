// bdls_filesystemutil_uniximputil.h                                  -*-C++-*-
#ifndef INCLUDED_BDLS_FILESYSTEMUTIL_UNIXIMPUTIL
#define INCLUDED_BDLS_FILESYSTEMUTIL_UNIXIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide testable 'bdls::FilesystemUtil' operations for some Unixes.
//
//@CLASSES:
//  bdls::FilesystemUtil_UnixImpUtil: testable file-system utilities
//
//@SEE_ALSO: bdls_filesystemutil, bdls_filesystemutil_unixplatform
//
//@DESCRIPTION: This subordinate component to 'bdls_filesystemutil' provides a
// utility 'struct' template, 'bdls::FilesystemUtil_UnixImpUtil' for
// implementing some of 'bdls::FilesystemUtil's functions on Unix platforms.
// 'bdls::FilesystemUtil_UnixImpUtil' accesses Unix functions and types through
// its template parameter, 'UNIX_INTERFACE', in order to allow tests to supply
// mock Unix interfaces.

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_timeunitratio.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdls {

                     // =================================
                     // struct FilesystemUtil_UnixImpUtil
                     // =================================

template <class UNIX_INTERFACE>
struct FilesystemUtil_UnixImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions that 'FilesystemUtil' uses as implementation details.
    // These functions have a 'UNIX_INTERFACE' template parameter, which
    // provides access to the entities that large-file environment Unix
    // systems declare, and that the function implementations need.
    //
    // Note that, on some Unix platforms and some build configurations, the
    // 'stat' struct does not have an 'st_mtime' field, and 'st_mtime' is a
    // macro that emulates the access of the field.  Similarly, on some Unix
    // platforms and some build configurations, the 'stat' structure does not
    // have an 'st_mtim' field or the 'st_mtim' struct does not have a
    // 'tv_nsec' (or, for some versions of Solaris, '__tv_nsec') field, in
    // which case the 'get_st_mtim_nsec' function returns zero.  For more
    // information, please see the specification of the 'sys/stat.h' header
    // from IEEE Std
    // 1003.1-2017, which provides information about the evolution of the
    // 'stat' struct in the POSIX specification
    // (https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/).
    //
    // The program is ill-formed unless the specified 'UNIX_INTERFACE' is a
    // class type that meets the following requirements:
    //
    //: o 'UNIX_INTERFACE::off_t' is a type alias to the 'off_t' type provided
    //:    by the 'sys/types.h' header.
    //:
    //: o 'UNIX_INTERFACE::stat' is a type alias to the 'stat' type provided
    //:    by the 'sys/stat.h' header.
    //:
    //: o 'UNIX_INTERFACE::time_t' is a type alias to the 'time_t' type provided
    //:    by the 'sys/types.h' header.
    //:
    //: o 'UNIX_INTERFACE::get_st_mtim_nsec' is a public, static member
    //:   function that has 'long (const stat& stat)' type and whose contract
    //:   is to return the value of the 'st_mtim.tv_nsec' field of the
    //:   specified 'stat' struct.
    //:
    //: o 'UNIX_INTERFACE::get_st_mtime' is a public, static member function
    //:   that has 'time_t (const stat& stat)' type and whose contract is to
    //:   return the value of the 'st_mtime' field of the specified 'stat'
    //:   struct.
    //:
    //: o 'UNIX_INTERFACE::get_st_size' is a public, static member function
    //:   that has 'off_t (const stat& stat)' type and whose contract is to
    //:   return the value of the 'st_size' field of the specified 'stat'
    //:   struct.  Note that this function is required in order to
    //:   access the data members of a 'stat' struct in a manner consistent
    //:   with the requirements of 'get_st_mtime'.
    //:
    //: o 'UNIX_INTERFACE::fstat' is a public, static member function that has
    //:   'int (int fildes, stat *buf)' type and whose contract is to return
    //:   the result of '::fstat(fildes, buf)', where '::fstat' is the
    //:   function provided by the 'sys/stat.h' header.

    // TYPES
    typedef int FileDescriptor;
        // 'FileDescriptor' is an alias for the operating system's native
        // file descriptor / file handle type.

    typedef typename UNIX_INTERFACE::off_t Offset;
        // 'Offset' is an alias for a signed integral type, and represents the
        // offset of a location in a file.

  private:
    // PRIVATE TYPES
    typedef typename UNIX_INTERFACE::off_t off_t;
        // 'off_t' is an alias to the 'off_t' type provided by the
        // 'sys/types.h' header.  It is a signed integral type used to
        // represent quantities of bytes.  Note that, depending on the build
        // configuration, this type may have 32 or 64 bits.

    typedef typename UNIX_INTERFACE::stat stat;
        // 'stat' is an alias to the 'stat' 'struct' provided by the
        // 'sys/stat.h' header.

    typedef typename UNIX_INTERFACE::time_t time_t;
        // 'time_t' is an alias to the 'time_t' type provided by the
        // 'sys/types.h' header.  It represents a time point as number of
        // seconds since January 1st 1970 in Coordinated Universal Time.

    // PRIVATE CLASS METHODS
    static long get_st_mtim_nsec(const stat& stat);
        // Return the value of the 'st_mtim.nsec' field of the specified 'stat'
        // struct.

    static time_t get_st_mtime(const stat& stat);
        // Return the value of the 'st_mtime' data member of the specified
        // 'stat'.

    static off_t get_st_size(const stat& stat);
        // Return the value of the 'st_size' data member of the specified
        // 'stat' struct.  Note that this function is provided in order to
        // create a consistent interface for accessing the data members of a
        // 'stat' struct with 'get_st_mtime'.

    static int fstat(int fildes, stat *buf);
        // Invoke and return the result of '::fstat(fildes, buf)' with the
        // specified  'fildes' and 'buf', where '::fstat' is the function
        // provided by the 'sys/stat.h' header.

  public:
    // CLASS METHODS
    static Offset getFileSize(FileDescriptor descriptor);
        // Return the size, in bytes, of the file with the specified
        // 'descriptor', or a negative value if an error occurs.

    static int getLastModificationTime(bdlt::Datetime *time,
                                       FileDescriptor  descriptor);
        // Load into the specified 'time' the last modification time of the
        // file with the specified 'descriptor', as reported by the filesystem.
        // Return 0 on success, and a non-zero value otherwise.  Note that the
        // time is reported in UTC.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // struct FilesystemUtil_UnixImpUtil
                     // ---------------------------------

// PRIVATE CLASS METHODS
template <class UNIX_INTERFACE>
long
FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::get_st_mtim_nsec(const stat& stat)
{
    return UNIX_INTERFACE::get_st_mtim_nsec(stat);
}

template <class UNIX_INTERFACE>
typename UNIX_INTERFACE::time_t
FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::get_st_mtime(const stat& stat)
{
    return UNIX_INTERFACE::get_st_mtime(stat);
}

template <class UNIX_INTERFACE>
typename UNIX_INTERFACE::off_t
FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::get_st_size(const stat& stat)
{
    return UNIX_INTERFACE::get_st_size(stat);
}

template <class UNIX_INTERFACE>
int FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::fstat(int fildes, stat *buf)
{
    return UNIX_INTERFACE::fstat(fildes, buf);
}

// CLASS METHODS
template <class UNIX_INTERFACE>
typename FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::Offset
FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::getFileSize(
                                                     FileDescriptor descriptor)
{
    stat statResult;
    const int rc = fstat(descriptor, &statResult);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return get_st_size(statResult);
}

template <class UNIX_INTERFACE>
int FilesystemUtil_UnixImpUtil<UNIX_INTERFACE>::getLastModificationTime(
                                                    bdlt::Datetime *time,
                                                    FileDescriptor  descriptor)
{
    stat statResult;
    int rc = fstat(descriptor, &statResult);
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
