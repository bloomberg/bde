// bteso_defaulteventmanagerimpl.h    -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGERIMPL
#define INCLUDED_BTESO_DEFAULTEVENTMANAGERIMPL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide forward declaration and common constants for event managers
//
//@CLASSES:
//
//@SEE_ALSO: bteso_defaulteventmanager
//
//@AUTHOR: Cheenu Srinivasan (csriniva)
//
//@DESCRIPTION: This component provides a forward declaration for
// 'bteso_DefaultEventManager' class along with certain type constants shared
// by various implementations.  This component is used to implement
// bteso_defaulteventmanager component as shown on the following diagram:
//..
//                     bteso_defaulteventmanager
//                    /   /     |      \        \
//             /-----/   /      |       \        \
//         _select    _poll  _devpoll  _pollset   _epoll
//             \-----    \      |       /        /
//                   \    \     |      /        /
//                   bteso_defaulteventmanagerimpl
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
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

#if !defined(BSLS_PLATFORM__OS_WINDOWS)
    #ifndef INCLUDED_SYS_POLL
    #include <sys/poll.h>
    #define INCLUDED_SYS_POLL
    #endif
#endif

namespace BloombergLP {

template <class POLLING_MECHANISM>      // forward declaration
class bteso_DefaultEventManager;

#ifdef BSLS_PLATFORM__OS_WINDOWS

// Ascribe 'HANDLE' with 'bslalg_TypeTraitBitwiseCopyable' trait to use it in
// 'bsl::vector' efficiently.
template <>
struct bslalg_TypeTraits<HANDLE> : public bslalg_TypeTraitBitwiseCopyable {
};

#else

// Ascribe 'pollfd' with 'bslalg_TypeTraitBitwiseCopyable' trait to use it in
// 'bsl::vector' efficiently.
template <>
struct bslalg_TypeTraits<struct ::pollfd> :
                                       public bslalg_TypeTraitBitwiseCopyable {
};

#endif // BSLS_PLATFORM__OS_WINDOWS

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
