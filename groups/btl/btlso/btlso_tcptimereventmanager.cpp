// btlso_tcptimereventmanager.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_tcptimereventmanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_tcptimereventmanager_cpp,"$Id$ $CSID$")

#include <btlso_defaulteventmanager.h>
#include <btlso_defaulteventmanager_devpoll.h>
#include <btlso_defaulteventmanager_epoll.h>
#include <btlso_defaulteventmanager_poll.h>
#include <btlso_defaulteventmanager_select.h>
#include <btlso_flag.h>
#include <btlso_platform.h>

#include <bslma_default.h>
#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bdlcc_timequeue.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_vector.h>

namespace BloombergLP {

enum {
    k_CPU_BOUND = btlso::TimeMetrics::e_CPU_BOUND,
    k_NUM_CATEGORIES = 2
};

namespace btlso {

                     // --------------------------
                     // class TcpTimerEventManager
                     // --------------------------

// CREATORS

TcpTimerEventManager::TcpTimerEventManager(bslma::Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_isManagedFlag(1)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_metrics(k_NUM_CATEGORIES, k_CPU_BOUND, basicAllocator)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::SELECT>(
                                                               &d_metrics,
                                                               basicAllocator);
#else
    d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::POLL>(
                                                               &d_metrics,
                                                               basicAllocator);
#endif
}

TcpTimerEventManager::TcpTimerEventManager(Hint              hint,
                                           bslma::Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_isManagedFlag(1)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_metrics(k_NUM_CATEGORIES, k_CPU_BOUND, basicAllocator)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    (void) hint;    // silence unused warning

    d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::SELECT>(
                                                               &d_metrics,
                                                               basicAllocator);
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    switch (hint) {
      case e_NO_HINT: {
        d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::POLL>(
                                                               &d_metrics,
                                                               basicAllocator);
      } break;
      case e_INFREQUENT_REGISTRATION: {
        d_manager_p = new (*d_allocator_p)
                        DefaultEventManager<Platform::DEVPOLL>(&d_metrics,
                                                               basicAllocator);
      } break;
      default: {
          BSLS_ASSERT(0);
      }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    (void) hint;    // silence unused warning

    d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::EPOLL>(
                                                               &d_metrics,
                                                               basicAllocator);
#else
    (void) hint;    // silence unused warning

    d_manager_p = new (*d_allocator_p) DefaultEventManager<Platform::POLL>(
                                                               &d_metrics,
                                                               basicAllocator);
#endif
}

TcpTimerEventManager::TcpTimerEventManager(EventManager     *manager,
                                           bslma::Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_manager_p(manager)
, d_isManagedFlag(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_metrics(k_NUM_CATEGORIES, k_CPU_BOUND, basicAllocator)
{
    BSLS_ASSERT(d_manager_p);
}

TcpTimerEventManager::~TcpTimerEventManager()
{
    if (d_isManagedFlag) {
        d_allocator_p->deleteObjectRaw(d_manager_p);
    }
}

// MANIPULATORS
int TcpTimerEventManager::dispatch(int flags)
{
    int ret;

    if (d_timers.length()) {
        bsls::TimeInterval minTime;
        if (d_timers.minTime(&minTime)) {
            return -1;                                                // RETURN
        }
        ret = d_manager_p->dispatch(minTime, flags);
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (now >= minTime) {
            bsl::vector<bdlcc::TimeQueueItem<bsl::function<void()> > >
                                                       requests(d_allocator_p);
            d_timers.popLE(now, &requests);
            int numTimers = requests.size();
            for (int i = 0; i < numTimers; ++i) {
                requests[i].data()();
            }

            return numTimers + (ret >= 0 ? ret : 0);                  // RETURN
        }
        else {
            return ret;                                               // RETURN
        }
    }
    else {
        if (!d_manager_p->numEvents()) {
            return 0;                                                 // RETURN
        }
        return d_manager_p->dispatch(flags);                          // RETURN
    }
}

int TcpTimerEventManager::registerSocketEvent(
                                         const SocketHandle::Handle& handle,
                                         EventType::Type             eventType,
                                         const Callback&             callBack)
{
    return d_manager_p->registerSocketEvent(handle, eventType, callBack);
}

void *TcpTimerEventManager::registerTimer(const bsls::TimeInterval& timeout,
                                          const Callback&           callback)
{
    return reinterpret_cast<void *>(d_timers.add(timeout, callback));
}

int TcpTimerEventManager::rescheduleTimer(const void                *timerId,
                                          const bsls::TimeInterval&  time)
{
    return d_timers.update(static_cast<int>(
                               reinterpret_cast<bsls::Types::IntPtr>(timerId)),
                           time);
}

void TcpTimerEventManager::deregisterSocketEvent(
                                            const SocketHandle::Handle& handle,
                                            EventType::Type             event)
{
    d_manager_p->deregisterSocketEvent(handle, event);
}

void TcpTimerEventManager::deregisterSocket(const SocketHandle::Handle& handle)
{
    d_manager_p->deregisterSocket(handle);
}

void TcpTimerEventManager::deregisterAllSocketEvents()
{
    d_manager_p->deregisterAll();
}

void TcpTimerEventManager::deregisterTimer(const void *handle)
{
    d_timers.remove(static_cast<int>(
                               reinterpret_cast<bsls::Types::IntPtr>(handle)));
}

void TcpTimerEventManager::deregisterAllTimers()
{
    d_timers.removeAll();
}

void TcpTimerEventManager::deregisterAll()
{
    deregisterAllSocketEvents();
    deregisterAllTimers();
}

// ACCESSORS
int TcpTimerEventManager::isRegistered(
                                   const SocketHandle::Handle& handle,
                                   EventType::Type             eventType) const
{
    return d_manager_p->isRegistered(handle, eventType);
}

int TcpTimerEventManager::numEvents() const
{
    return d_timers.length() + d_manager_p->numEvents();
}

int TcpTimerEventManager::numSocketEvents(
                                      const SocketHandle::Handle& handle) const
{
    return d_manager_p->numSocketEvents(handle);
}

int TcpTimerEventManager::numTimers() const
{
    return d_timers.length();
}

}  // close package namespace

}  // close enterprise namespace

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
