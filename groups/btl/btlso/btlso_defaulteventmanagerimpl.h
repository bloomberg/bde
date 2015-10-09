// btlso_defaulteventmanagerimpl.h                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGERIMPL
#define INCLUDED_BTLSO_DEFAULTEVENTMANAGERIMPL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide forward declaration and constants for event managers.
//
//@CLASSES:
//  btlso::DefaultEventManager<POLLING_MECHANISM>: default multiplexer
//
//@SEE_ALSO: btlso_defaulteventmanager
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
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Default Event Manager
///- - - - - - - - - - - - - - - - - - - - - -
// In the following usage example we show how to create an default instance of
// a default event manager.  First, we need to include this file (shown here
// for completeness):
//..
//  #include <btlso_defaulteventmanagerimpl.h>
//..
// Second, create a 'btlso::TimeMetrics' to give to the event manager:
//..
//  btlso::TimeMetrics metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
//                             btlso::TimeMetrics::e_CPU_BOUND);
//..
// Now, create a default event manager that uses this 'metrics':
//..
//  btlso::DefaultEventManager<> eventManager(&metrics);
//..
// Note that the time metrics is optional.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
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

namespace btlso {

                     // =========================
                     // class DefaultEventManager
                     // =========================

template <class POLLING_MECHANISM = Platform::DEFAULT_POLLING_MECHANISM>
class DefaultEventManager;

}  // close package namespace

}  // close enterprise namespace

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

}  // close namespace bsl

#endif // BSLS_PLATFORM_OS_WINDOWS

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
