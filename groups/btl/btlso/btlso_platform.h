// btlso_platform.h                                                   -*-C++-*-
#ifndef INCLUDED_BTLSO_PLATFORM
#define INCLUDED_BTLSO_PLATFORM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform trait definitions.
//
//@CLASSES:
//   btlso_platform: namespace for platform socket traits
//
//@SEE_ALSO:
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines set of traits that identify and describe
// a platform's socket properties.  In particular, the 'SocketFamily' trait is
// defined and assigned to a 'value' appropriate for each supported platform.
// Values for these socket families are actually types declared in
// 'btlso_platform'.  These incomplete types are intended to be used in
// specializing template implementations or to enable function overloading
// based on the prevalent system's characteristics.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                            // ====================
                            // class btlso::Platform
                            // ====================

namespace BloombergLP {

namespace btlso {
struct Platform {
    // Provide a namespace for socket trait definitions.

    struct BsdSockets {};
    struct WinSockets {};

    struct SELECT {};

    #ifdef BSLS_PLATFORM_OS_UNIX
        typedef BsdSockets SocketFamily;
        #define BTLSO_PLATFORM_BSD_SOCKETS 1

        struct POLL {};       // 'poll' syscall is available
        struct SIGNAL {};
        struct DEVPOLL {};

        #if defined(BSLS_PLATFORM_OS_AIX)
            struct POLLSET {};
            typedef POLLSET DEFAULT_POLLING_MECHANISM;
        #endif

        #if defined(BSLS_PLATFORM_OS_SOLARIS)
            typedef DEVPOLL DEFAULT_POLLING_MECHANISM;
        #endif

        #ifdef BSLS_PLATFORM_OS_LINUX
            struct EPOLL {};
            typedef EPOLL   DEFAULT_POLLING_MECHANISM;
        #endif

        #if defined(BSLS_PLATFORM_OS_CYGWIN)  \
         || defined(BSLS_PLATFORM_OS_FREEBSD) \
         || defined(BSLS_PLATFORM_OS_DARWIN)  \
         || defined(BSLS_PLATFORM_OS_HPUX)
            typedef POLL    DEFAULT_POLLING_MECHANISM;
        #endif

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
        typedef WinSockets SocketFamily;
        #define BTLSO_PLATFORM_WIN_SOCKETS 1

        typedef SELECT  DEFAULT_POLLING_MECHANISM;
    #endif
};
}  // close package namespace

}  // close enterprise namespace

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT

#ifdef BTLSO_PLATFORM_BSD_SOCKETS
# define BTESO_PLATFORM__BSD_SOCKETS BTLSO_PLATFORM_BSD_SOCKETS
#endif
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
# define BTESO_PLATFORM__WIN_SOCKETS BTLSO_PLATFORM_WIN_SOCKETS
#endif

#endif

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
