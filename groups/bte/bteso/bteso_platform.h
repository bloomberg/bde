// bteso_platform.h                 -*-C++-*-
#ifndef INCLUDED_BTESO_PLATFORM
#define INCLUDED_BTESO_PLATFORM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform trait definitions.
//
//@CLASSES:
//   bteso_platform: namespace for platform socket traits
//
//@SEE_ALSO:
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines set of traits that identify and describe
// a platform's socket properties.  In particular, the 'SocketFamily' trait is
// defined and assigned to a 'value' appropriate for each supported platform.
// Values for these socket families are actually types declared in
// 'bteso_platform'.  These incomplete types are intended to be used in
// specializing template implementations or to enable function overloading
// based on the prevalent system's characteristics.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                            // ====================
                            // class bteso_Platform
                            // ====================

namespace BloombergLP {

struct bteso_Platform {
    // Provide a namespace for socket trait definitions.

    struct BsdSockets {};
    struct WinSockets {};

    #ifdef BSLS_PLATFORM__OS_UNIX
        typedef BsdSockets SocketFamily;
        #define BTESO_PLATFORM__BSD_SOCKETS 1
        struct POLL {};       // 'poll' syscall is available
        struct SIGNAL {};
        struct SELECT {};
        #if defined(BSLS_PLATFORM__OS_SOLARIS) || \
            defined(BSLS_PLATFORM__OS_HPUX)
            struct DEVPOLL {};
        #endif
        #ifdef BSLS_PLATFORM__OS_SOLARIS
            typedef DEVPOLL DEFAULT_POLLING_MECHANISM;
        #endif

        #ifdef BSLS_PLATFORM__OS_LINUX
            struct EPOLL {};
            typedef EPOLL   DEFAULT_POLLING_MECHANISM;
        #endif
        #if defined(BSLS_PLATFORM__OS_AIX)     \
         || defined(BSLS_PLATFORM__OS_HPUX)    \
         || defined(BSLS_PLATFORM__OS_CYGWIN)  \
         || defined(BSLS_PLATFORM__OS_FREEBSD)
            typedef POLL    DEFAULT_POLLING_MECHANISM;
        #endif

    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
        typedef WinSockets SocketFamily;
        #define BTESO_PLATFORM__WIN_SOCKETS 1
        struct WFMO {};
        struct SELECT {};

        typedef SELECT  DEFAULT_POLLING_MECHANISM;
        typedef WFMO    SlowPoll;
    #endif
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
