// bdls_filesystemutil_windowsimputil.h                               -*-C++-*-
#ifndef INCLUDED_BDLS_FILESYSTEMUTIL_WINDOWSIMPUTIL
#define INCLUDED_BDLS_FILESYSTEMUTIL_WINDOWSIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide testable 'bdls::FilesystemUtil' operations on Windows.
//
//@CLASSES:
//  bdls::FilesystemUtil_WindowsImpUtil: testable file-system utilities
//
//@SEE_ALSO: bdls_filesystemutil
//
//@DESCRIPTION: This subordinate component to 'bdls_filesystemutil' provides a
// utility 'struct' template, 'bdls::FilesystemUtil_WindowsImpUtil' for
// implementing some of 'bdls::FilesystemUtil's functions on Windows
// platforms.  'bdls::FilesystemUtil_WindowsImpUtil' accesses Windows functions
// and types through its template parameter, 'WINDOWS_INTERFACE', in order to
// allow tests to supply mock Windows interfaces.

#include <bdlt_datetime.h>

namespace BloombergLP {
namespace bdls {

                   // ====================================
                   // struct FilesystemUtil_WindowsImpUtil
                   // ====================================

template <class WINDOWS_INTERFACE>
struct FilesystemUtil_WindowsImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions that 'FilesystemUtil' uses as implementation details.
    // These functions have a 'WINDOWS_INTERFACE' template parameter, which
    // provides access to the entities that Windows systems declare, and that
    // the function implementations need.
    //
    // The program is ill-formed unless the specified 'WINDOWS_INTERFACE' is
    // a class type that meets the following requirements:
    //
    //: o 'WINDOWS_INTERFACE::BOOL' is a type alias to the 'BOOL'
    //:   type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::DWORD' is a type alias to the 'DWORD'
    //:   type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::FILETIME' is a type alias to the 'FILETIME'
    //:   type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::HANDLE' is a type alias to the 'HANDLE'
    //:   type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::INT64' is a type alias to the 'IN64' type
    //:    provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::LPFILETIME' is a type alias to the
    //:   'LPFILETIME' type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::SYSTEMTIME' is a type alias to the
    //:   'LPFILETIME' type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::ULONG64' is a type alias to the 'ULONG64'
    //:   type provided by the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::FileTimeToSystemTime' is a public, static
    //:   member function that has
    //:   'BOOL (const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime)' type
    //:   and whose contract is to return the result of
    //:   '::FileTimeToSystemTime(lpFileTime, lpSystemTime)', where
    //:   '::FileTimeToSystemTime' is the corresponding function declared in
    //:   the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::GetFileSize' is a public, static member
    //:   function that has 'DWORD (HANDLE hFile, LPDWORD lpFileSizeHigh)'
    //:   type and whose contract is to return the result of
    //:   '::GetFileSize(hFile, lpFileSizeHigh)', where '::GetFileSize' is the
    //:   corresponding function declared in the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::GetFileTime' is a public, static member
    //:   function that has
    //:   'BOOL (HANDLE hFile, LPFILETIME lpCreationTime,
    //:   LPFILETIME lpLastAccessTime, LPFILETIME, lpLastWriteTime)' type and
    //:   whose contract is to return the result of
    //:   '::GetFileTime(hFile, lpCreationTime, lpLastAccessTime,
    //:   lpLastWriteTime)', where '::GetFileTime' is the corresponding
    //:   function declared in the 'windows.h' header.
    //:
    //: o 'WINDOWS_INTERFACE::GetLastError' is a public, static
    //:    member function that has 'DWORD ()' type and whose contract
    //:    is to return the result of '::GetLastError()', where
    //:    '::GetLastError' is the corresponding function declared in the
    //:    'windows.h' header.

    // TYPES
    typedef typename WINDOWS_INTERFACE::HANDLE FileDescriptor;
        // 'FileDescriptor' is an alias for operating system's native file
        // descriptor / file handle type.

    typedef typename WINDOWS_INTERFACE::INT64 Offset;
        // 'Offset' is an alias for a signed integral type, and represents the
        // offset of a location in a file.

  private:
    // PRIVATE TYPES
    typedef typename WINDOWS_INTERFACE::BOOL BOOL;
        // 'BOOL' is an alias to the unsigned integral 'BOOL' type provided
        // by the 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::DWORD DWORD;
        // 'DWORD' is an alias to the unsigned integral 'DWORD' type provided
        // by the 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::FILETIME FILETIME;
        // 'FILETIME' is an alias to the 'FILETIME' struct provided by the
        // 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::HANDLE HANDLE;
        // 'HANDLE' is an alias to the 'HANDLE' type provided by the
        // 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::INT64 INT64;
        // 'INT64' is an alias to the signed integral 'INT64' type provided by
        // the 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::LPDWORD LPDWORD;
        // 'LPDWORD' is an alias to the unsigned integral 'LPDWORD' type
        // provided by the 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::LPFILETIME LPFILETIME;
        // 'LPFILETIME' is an alias to the 'LPFILETIME' type provided by the
        // 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::LPSYSTEMTIME LPSYSTEMTIME;
        // 'LPSYSTEMTIME' is an alias to the 'LPSYSTEMTIME' type provided by
        // the 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::SYSTEMTIME SYSTEMTIME;
        // 'SYSTEMTIME' is an alias to the 'SYSTEMTIME' struct provided by the
        // 'windows.h' header.

    typedef typename WINDOWS_INTERFACE::ULONG64 ULONG64;
        // 'ULONG64' is an alias to the unsigned integral 'ULONG64' type
        // provided by the 'windows.h' header.

    // PRIVATE CLASS METHODS
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

                    // ------------------------------------
                    // struct FilesystemUtil_WindowsImpUtil
                    // ------------------------------------

// PRIVATE CLASS METHODS
template <class WINDOWS_INTERFACE>
typename WINDOWS_INTERFACE::BOOL
FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::FileTimeToSystemTime(
                                                  const FILETIME *lpFileTime,
                                                  LPSYSTEMTIME    lpSystemTime)
{
    return WINDOWS_INTERFACE::FileTimeToSystemTime(lpFileTime, lpSystemTime);
}

template <class WINDOWS_INTERFACE>
typename WINDOWS_INTERFACE::DWORD
FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::GetFileSize(
                                                        HANDLE  hFile,
                                                        LPDWORD lpFileSizeHigh)
{
    return WINDOWS_INTERFACE::GetFileSize(hFile, lpFileSizeHigh);
}

template <class WINDOWS_INTERFACE>
typename WINDOWS_INTERFACE::BOOL
FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::GetFileTime(
                                                   HANDLE     hFile,
                                                   LPFILETIME lpCreationTime,
                                                   LPFILETIME lpLastAccessTime,
                                                   LPFILETIME lpLastWriteTime)
{
    return WINDOWS_INTERFACE::GetFileTime(
        hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
}

template <class WINDOWS_INTERFACE>
typename WINDOWS_INTERFACE::DWORD
FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::GetLastError()
{
    return WINDOWS_INTERFACE::GetLastError();
}

// CLASS METHODS
template <class WINDOWS_INTERFACE>
typename FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::Offset
FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::getFileSize(
                                                     FileDescriptor descriptor)
{
    BSLMF_ASSERT(bsl::is_integral<DWORD>::value);
    BSLS_ASSERT(0            == bsl::numeric_limits<DWORD>::min());
    BSLS_ASSERT(0xFFFFFFFFul == bsl::numeric_limits<DWORD>::max());

    BSLMF_ASSERT(bsl::is_integral<ULONG64>::value);
    BSLS_ASSERT(0                     == bsl::numeric_limits<ULONG64>::min());
    BSLS_ASSERT(0xFFFFFFFFFFFFFFFFull == bsl::numeric_limits<ULONG64>::max());

    // The Windows implementation of this function uses 'GetFileSize' on
    // purpose, even though the Win32 API documentation instructs the reader to
    // use 'GetFileSizeEx' instead.  'GetFileSizeEx' returns a 'LARGE_INTEGER',
    // which is a union of two or more layout-incompatible, 64-bit integer
    // representations, and there is no way to know which member is active.
    // This forces the programmer to do "union type punning," which is jargon
    // for invoking undefined behavior by accessing an inactive union member
    // that is not layout-compatible with the active member.
    //
    // 'GetFileSize' has an awkward interface, but it requires no type punning
    // and isn't deprecated.

    DWORD sizeHigh32Bits;
    const DWORD sizeLow32Bits = GetFileSize(descriptor, &sizeHigh32Bits);
    // 'GetFileSize' returns the maximum unsigned, 32-bit integer to indicate
    // that it could not get the file size.  However, this is also a legal
    // value for the file size's low 32-bits.  To remove the ambiguity,
    // this function calls 'GetLastError', which returns non-zero to indicate
    // the last system call had an error, and 0 otherwise.

    static const DWORD k_INVALID_FILE_SIZE = 0xFFFFFFFFul;
    if (k_INVALID_FILE_SIZE == sizeLow32Bits) {
        const DWORD lastError = GetLastError();

        static const DWORD k_NO_ERROR = 0;
        if (k_NO_ERROR != lastError) {
            return -1;                                                // RETURN
        }
    }

    const ULONG64 uSizeHigh32Bits = static_cast<ULONG64>(sizeHigh32Bits);
    const ULONG64 uSizeLow32Bits  = static_cast<ULONG64>(sizeLow32Bits);
    const ULONG64 uSize64Bits     = (uSizeHigh32Bits << 32) | uSizeLow32Bits;

    return static_cast<Offset>(uSize64Bits);
}

template <class WINDOWS_INTERFACE>
int FilesystemUtil_WindowsImpUtil<WINDOWS_INTERFACE>::getLastModificationTime(
                                                    bdlt::Datetime *time,
                                                    FileDescriptor  descriptor)
{
    static const LPFILETIME s_IGNORED_CREATION_TIME    = 0;
    static const LPFILETIME s_IGNORED_LAST_ACCESS_TIME = 0;

    FILETIME lastWriteTime;
    const BOOL getFileTimeSuccessFlag = GetFileTime(descriptor,
                                                    s_IGNORED_CREATION_TIME,
                                                    s_IGNORED_LAST_ACCESS_TIME,
                                                    &lastWriteTime);
    if (!getFileTimeSuccessFlag) {
        return -1;                                                    // RETURN
    }

    SYSTEMTIME lastWriteSystemTime;
    const BOOL getSystemTimeSuccessFlag = FileTimeToSystemTime(
        const_cast<const FILETIME *>(&lastWriteTime), &lastWriteSystemTime);

    if (!getSystemTimeSuccessFlag) {
        return -1;                                                    // RETURN
    }

    bdlt::Datetime result;

    const int rc =
        result.setDatetimeIfValid(lastWriteSystemTime.wYear,
                                  lastWriteSystemTime.wMonth,
                                  lastWriteSystemTime.wDay,
                                  lastWriteSystemTime.wHour,
                                  lastWriteSystemTime.wMinute,
                                  lastWriteSystemTime.wSecond,
                                  lastWriteSystemTime.wMilliseconds);
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
