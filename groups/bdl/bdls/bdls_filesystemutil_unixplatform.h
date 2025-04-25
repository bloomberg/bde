// bdls_filesystemutil_unixplatform.h                                 -*-C++-*-
#ifndef INCLUDED_BDLS_FILESYSTEMUTIL_UNIXPLATFORM
#define INCLUDED_BDLS_FILESYSTEMUTIL_UNIXPLATFORM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide Unix timestamp and offset type width detection macros.
//
//@MACROS:
// BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF:   1 if `off_t` is 64 bits, else
// BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64: 1 if `off64_t` exists, else
// BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF:   1 (`off_t` is 32 bits)
//
// BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER:   The `stat` nanosecond
//                                                    member name
//
//@SEE_ALSO: bdls_filesystemutil, bdls_filesystemutil_unixplatform,
// bdls_filesystemutil_transitionalunixplatform
//
//@DESCRIPTION: This subordinate component to `bdls_filesystemutil` provides:
//
// a) a set of macros for detecting the most appropriate type to use for byte
// quantities, and most appropriate set of functions to use for file
// operations, on Unix platforms, and
//
// b) a macro which specifies the name of the member variable, within a `stat`
// structure, that holds the nanosecond part of the file timestamp.
//
///Nanosecond Support Macros
///-------------------------
// This component defines the following Macros if the `stat` structure defined
// in `sys/stat.h` contains the `st_mtim.tv_nsec` member.  This is determined
// by the feature tests specified in POSIX.1-2008 - see
// https://linux.die.net/man/2/fstat.
//
//: `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER`: This macro is defined to
//:  be the name of the member of the `stat` structure that contains the
//:  nanosecond part of the timestamp value.
//
///Byte Quantity Macros
///--------------------
// This component defines exactly 1 of 3 macros depending on the current
// platform configuration, which are used to determine which offset type and
// associated set of file-system functions a program should use on Unix.  These
// macros are:
//
//: `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF`: The presence of this macro
//: definition indicates that the platform natively supports large (64-bit)
//: file operations, and that the `off_t` type provided by the `sys/types.h`
//: header is 64-bit.
//:
//: `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64`: The presence of this macro
//: definition indicates that the platform *does* *not* natively support large
//: (64-bit) file operations, but that it does support the
//: "transitional-compilation environment" that provides non-standard large
//: file types and operations.  Specifically, this indicates that the platform
//: supports a 64-bit `off64_t` type, and provides associated `xxx64` file
//: operations.  For more information about the transitional-compilation
//: environment, see the component documentation of
//: {`bdls_filesystemutil_transitionaluniximputil`}.
//:
//: `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF`: The presence of this macro
//: definition indicates that the platform does not natively support large
//: (64-bit) file operations, nor is it configured to provide the
//: transitional-compilation environment.  Specifically, this indicates that
//: the platform provides a 32-bit `off_t` type and associated 32-bit file
//: operations.  If this macro is defined, a program may be unable to operate
//: on files larger than approximately 2 Gigabytes.
//
///Byte Quantity Macro Selection
///-----------------------------
// Which of the 3 macros this component defines depends upon the capabilities
// of the platform's configuration.  There are 4 criteria used to determine
// which macro is defined, and they are
//
// * the operating system,
// * whether the CPU is 32 or 64-bit,
// * whether a `_FILE_OFFSET_BITS` macro is defined to `64`, and
// * whether the transitional-compilation environment is enabled
//
// The set of possible combinations of these criteria, and consequently which
// macros are defined, is specified in the table below.  The legend for this
// table is the following:
//
//: `OS`:
//:   Short for "Operating System".  This column lists the operating system
//:   associated with the platform.
//:
//: `Mode`:
//:   Either `32` or `64`.  This column lists the number of bits in a machine
//:   word associated with the platform.  A program compiled for an x86
//:   processor would be `32`, and one compiled for an x86_64 processor would
//:   be `64`, for example.
//:
//: `FOB=64`:
//:   Short for "File-Offset Bits == 64".  This column lists whether or not the
//:   `_FILE_OFFSET_BITS` macro is defined to the value `64`, or is otherwise
//:   defined to another value or undefined.  A value of `Yes` indicates
//:   `_FILE_OFFSET_BITS` is defined to `64`, and a value of `No` indicates
//:   that it is not.  Note that this macro is not a part of any Unix standard,
//:   but nevertheless is supported by several Unixes.  On most Unix platforms,
//:   defining `_FILE_OFFSET_BITS` to a value other than `64` has no meaning.
//:
//: `LF`:
//:   Short for `Large File Source`.  This column lists whether or not a macro
//:   is defined that indicates the current program is compiled in the
//:   "Transitional-Compilation Environment."  A value of `Yes` indicates the
//:   transitional-compilation environment is enabled, and a value of `No`
//:   indicates that it is not.  Different macros enable this environment on
//:   different Unixes, if they support it.  This environment is enabled on AIX
//:   if the `_LARGE_FILE` macro is defined, and it is enabled on Linux,
//:   Solaris, and SunOS if the `_LARGEFILE64_SOURCE` macro is defined.
//:   Neither Cygwin, Darwin, nor FreeBSD support this environment, because
//:   they natively support large (64-bit) file operations in all compilation
//:   modes.
//:
//: `Off Type`:
//:   Short for "Offset Type".  This column lists the type that a program
//:   should use to represent file offsets on Unix platforms.  The offset type
//:   should be `off_t` (64-bit) if the program natively supports large
//:   (64-bit) file operations, `off64_t` if the program does not natively
//:   support these operations, but provides the transitional-compilation
//:   environment, and `off_t` (32-bit) if the program neither natively support
//:   large (64-bit) file operations nor provides the transitional-compilation
//:   environment.  Note that a 32-bit `off_t` is used as a "last resort" when
//:   no 64-bit offset type is available on the platform.  A program may be
//:   unable to operate on files larger than approximately 2 Gigabytes if a
//:   32-bit `off_t` is used.
//:
//: `Off Bits`:
//:   This column lists the number of bits used in the offset type to represent
//:   a byte quantity.  A value of `64` indicates that the offset type has 64
//:   bits, and a value of `32` indicates that the offset type has 32 bits.
//:
//: `Macro Defined`:
//:   This column lists which macro provided by this component is defined, in
//:   order to indicate the offset type and set of file-system functions that a
//:   program should use on Unix.  A value of `*_32_BIT_OFF` indicates that the
//:   `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF` macro is defined, and that
//:   a program should use the `off_t` offset type and associated file-system
//:   functions, a value of "*<u>64</u>BIT_OFF" indicates that the
//:   `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF` macro is defined, and that
//:   a program should use the `off_t` offset type and associated file-system
//:   functions, and a value of `*_64_BIT_OFF64` indicates that the
//:   `BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64` macro is defined, and
//:   that a program should use the `off64_t` offset type, and associated
//:   file-system functions from the transitional-compilation environment.
//
// The table follows.  Note that a value of `N/A` in any cell of the table
// indicates that the value of that cell does affect the determination of the
// value of the associated `Off Type`, `Off-Type Bits` and `Macro Defined`
// cells.
//
// ```
// |---------|------|--------|-----|----------|----------|---------------|
// | OS      | Mode | FOB=64 | LF  | Off Type | Off Bits | Macro Defined |
// |=========+======+========+=====+==========+==========+===============|
// | AIX     |   32 |    N/A |  No | 'off64_t'|       64 | *_64_BIT_OFF64|
// | AIX     |   32 |    N/A | Yes | 'off_t'  |       64 | *_64_BIT_OFF  |
// | AIX     |   64 |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Cygiwn  |  N/A |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Darwin  |  N/A |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | FreeBSD |  N/A |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Linux   |   32 |     No |  No | 'off_t'  |       32 | *_32_BIT_OFF  |
// | Linux   |   32 |     No | Yes | 'off64_t'|       64 | *_64_BIT_OFF64|
// | Linux   |   32 |    Yes | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Linux   |   64 |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | SunOS   |   32 |     No |  No | 'off_t'  |       32 | *_32_BIT_OFF  |
// | SunOS   |   32 |     No | Yes | 'off64_t'|       64 | *_64_BIT_OFF64|
// | SunOS   |   32 |    Yes | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | SunOS   |   64 |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Solaris |   32 |     No |  No | 'off_t'  |       32 | *_32_BIT_OFF  |
// | Solaris |   32 |     No | Yes | 'off64_t'|       64 | *_64_BIT_OFF64|
// | Solaris |   32 |    Yes | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// | Solaris |   64 |    N/A | N/A | 'off_t'  |       64 | *_64_BIT_OFF  |
// |---------+------+--------+-----+----------+----------+---------------+
// ```

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_UNIX)

#include <unistd.h>

                       // 32-Bit AIX with '_LARGE_FILE'

    #if defined(BSLS_PLATFORM_OS_AIX)     \
     && defined(BSLS_PLATFORM_CPU_32_BIT) \
     && defined(_LARGE_FILE)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                                 // 32-Bit AIX

    #elif defined(BSLS_PLATFORM_OS_AIX)     \
       && defined(BSLS_PLATFORM_CPU_32_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64 1

                                 // 64-Bit AIX

    #elif defined(BSLS_PLATFORM_OS_AIX)   \
       && defined(BSLS_PLATFORM_CPU_64_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                                   // Cygwin

    #elif defined(BSLS_PLATFORM_OS_CYGWIN)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                                   // Darwin

    #elif defined(BSLS_PLATFORM_OS_DARWIN)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                                  // FreeBSD

    #elif defined(BSLS_PLATFORM_OS_FREEBSD)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                 // 32-Bit Linux with '_FILE_OFFSET_BITS = 64'

    #elif defined(BSLS_PLATFORM_OS_LINUX)   \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_FILE_OFFSET_BITS)        \
       && _FILE_OFFSET_BITS == 64

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                  // 32-Bit Linux with '_LARGEFILE64_SOURCE'

    #elif defined(BSLS_PLATFORM_OS_LINUX)   \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_LARGEFILE64_SOURCE)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64 1

                                // 32-Bit Linux

    #elif defined(BSLS_PLATFORM_OS_LINUX)  \
       && defined(BSLS_PLATFORM_CPU_32_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF 1

                                // 64-Bit Linux

    #elif defined(BSLS_PLATFORM_OS_LINUX)  \
       && defined(BSLS_PLATFORM_CPU_64_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                 // 32-Bit SunOS with '_FILE_OFFSET_BITS = 64'

    #elif defined(BSLS_PLATFORM_OS_SUNOS)   \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_FILE_OFFSET_BITS)        \
       && _FILE_OFFSET_BITS == 64

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                  // 32-Bit SunOS with '_LARGEFILE64_SOURCE'

    #elif defined(BSLS_PLATFORM_OS_SUNOS)   \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_LARGEFILE64_SOURCE)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64 1

                                // 32-Bit SunOS

    #elif defined(BSLS_PLATFORM_OS_SUNOS)   \
       && defined(BSLS_PLATFORM_CPU_32_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF 1

                                // 64-Bit SunOS

    #elif defined(BSLS_PLATFORM_OS_SUNOS)   \
       && defined(BSLS_PLATFORM_CPU_64_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                // 32-Bit Solaris with '_FILE_OFFSET_BITS = 64'

    #elif defined(BSLS_PLATFORM_OS_SOLARIS) \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_FILE_OFFSET_BITS)        \
       && _FILE_OFFSET_BITS == 64

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

                 // 32-Bit Solaris with '_LARGEFILE64_SOURCE'

    #elif defined(BSLS_PLATFORM_OS_SOLARIS) \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && defined(_LARGEFILE64_SOURCE)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64 1

                               // 32-Bit Solaris

    #elif defined(BSLS_PLATFORM_OS_SOLARIS) \
       && defined(BSLS_PLATFORM_CPU_32_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF 1

                               // 64-Bit Solaris

    #elif defined(BSLS_PLATFORM_OS_SOLARIS)   \
       && defined(BSLS_PLATFORM_CPU_64_BIT)

        #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF 1

    #else
    #error "'bdls_filesystemutil_unixplatform.h' does not support this Unix."
    #endif

#if defined(BSLS_PLATFORM_OS_SOLARIS) ||                          \
    defined(BSLS_PLATFORM_OS_AIX) ||                              \
    defined(BSLS_PLATFORM_OS_LINUX)

   #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER st_mtim.tv_nsec

#elif defined(BSLS_PLATFORM_OS_DARWIN)

   #define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER st_mtimespec.tv_nsec

#endif

#endif
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
