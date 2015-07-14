// btlso_defaulteventmanagerimpl.h    -*-C++-*-
#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGERIMPL
#define INCLUDED_BTLSO_DEFAULTEVENTMANAGERIMPL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide forward declaration and common constants for event managers
//
//@CLASSES:
//
//@SEE_ALSO: btlso_defaulteventmanager
//
//@AUTHOR: Cheenu Srinivasan (csriniva)
//
//@DESCRIPTION: This component provides a forward declaration for
// 'btlso::DefaultEventManager' class along with certain type constants shared
// by various implementations.  This component is used to implement
// btlso_defaulteventmanager component as shown on the following diagram:
//..
//     |                btlso_defaulteventmanager            |
//     |               /   /     |      \        \           |
//     |        /-----/   /      |       \        \          |
//     |    _select    _poll  _devpoll  _pollset   _epoll    |
//     |        \-----    \      |       /        /          |
//     |              \    \     |      /        /           |
//     |              btlso_defaulteventmanagerimpl          |
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if !defined(BSLS_PLATFORM_OS_WINDOWS)
    #ifndef INCLUDED_SYS_POLL
    #include <sys/poll.h>
    #define INCLUDED_SYS_POLL
    #endif
#endif

namespace BloombergLP {

// forward declaration

namespace btlso {template <class POLLING_MECHANISM = Platform::DEFAULT_POLLING_MECHANISM>
class DefaultEventManager;
}  // close package namespace

}  // close namespace BloombergLP

#ifdef BSLS_PLATFORM_OS_WINDOWS

// Ascribe 'HANDLE' with 'bsl::is_trivially_copyable' trait to use it in
// 'bsl::vector' efficiently.
namespace bsl {
template <> struct is_trivially_copyable<HANDLE> : true_type {};
}

#else

// Ascribe 'pollfd' with 'bsl::is_trivially_copyable' trait to use it in
// 'bsl::vector' efficiently.
namespace bsl {
template <> struct is_trivially_copyable<pollfd> : true_type {};
}

#endif // BSLS_PLATFORM_OS_WINDOWS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
