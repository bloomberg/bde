// bteso_tcptimereventmanager.cpp -*-C++-*-
#include <bteso_tcptimereventmanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_tcptimereventmanager_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_socketimputil.h>            // for testing only
#include <bteso_ipv4address.h>              // for testing only
#endif

#include <bteso_defaulteventmanager.h>
#include <bteso_defaulteventmanager_devpoll.h>
#include <bteso_defaulteventmanager_epoll.h>
#include <bteso_defaulteventmanager_poll.h>
#include <bteso_defaulteventmanager_select.h>
#include <bteso_flag.h>
#include <bteso_platform.h>

#include <bslma_default.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bcec_timequeue.h>
#include <bdef_function.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_vector.h>

namespace BloombergLP {

    enum {
        CPU_BOUND = bteso_TimeMetrics::BTESO_CPU_BOUND,
        IO_BOUND = bteso_TimeMetrics::BTESO_IO_BOUND,
        NUM_CATEGORIES = 2
    };

                     // ================================
                     // class bteso_TcpTimerEventManager
                     // ================================

// CREATORS

bteso_TcpTimerEventManager::bteso_TcpTimerEventManager(
      bslma_Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_isManagedFlag(1)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_metrics(NUM_CATEGORIES, CPU_BOUND, basicAllocator)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_manager_p = new (*d_allocator_p)
        bteso_DefaultEventManager<bteso_Platform::SELECT>(&d_metrics,
                                                           basicAllocator);
#else
    d_manager_p = new (*d_allocator_p)
        bteso_DefaultEventManager<bteso_Platform::POLL>(&d_metrics,
                                                        basicAllocator);
#endif
}

bteso_TcpTimerEventManager::bteso_TcpTimerEventManager(
      Hint infrequentRegistrationHint,
      bslma_Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_isManagedFlag(1)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_metrics(NUM_CATEGORIES, CPU_BOUND, basicAllocator)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_manager_p = new (*d_allocator_p)
        bteso_DefaultEventManager<bteso_Platform::SELECT>(&d_metrics,
                                                          basicAllocator);
#elif defined(BSLS_PLATFORM__OS_SOLARIS)
    switch (infrequentRegistrationHint) {
      case BTESO_NO_HINT: {
        d_manager_p = new (*d_allocator_p)
            bteso_DefaultEventManager<bteso_Platform::POLL>(&d_metrics,
                                                            basicAllocator);
      } break;
      case BTESO_INFREQUENT_REGISTRATION: {
        d_manager_p = new (*d_allocator_p)
            bteso_DefaultEventManager<bteso_Platform::DEVPOLL>(&d_metrics,
                                                               basicAllocator);
      } break;
      default: {
          BSLS_ASSERT(0);
      }
    }
#elif defined(BSLS_PLATFORM__OS_LINUX)
    d_manager_p = new (*d_allocator_p)
        bteso_DefaultEventManager<bteso_Platform::EPOLL>(&d_metrics,
                                                         basicAllocator);
#else
    d_manager_p = new (*d_allocator_p)
        bteso_DefaultEventManager<bteso_Platform::POLL>(&d_metrics,
                                                        basicAllocator);
#endif
}

bteso_TcpTimerEventManager::bteso_TcpTimerEventManager(
      bteso_EventManager *manager,
      bslma_Allocator *basicAllocator)
: d_timers(basicAllocator)
, d_manager_p(manager)
, d_isManagedFlag(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_metrics(NUM_CATEGORIES, CPU_BOUND, basicAllocator)
{
    BSLS_ASSERT(d_manager_p);
}

bteso_TcpTimerEventManager::~bteso_TcpTimerEventManager() {
    if (d_isManagedFlag) {
        d_allocator_p->deleteObjectRaw(d_manager_p);
    }
}

// MANIPULATORS
int bteso_TcpTimerEventManager::dispatch(int flags) {
    int ret;

    if (d_timers.length()) {
        bdet_TimeInterval minTime;
        if (d_timers.minTime(&minTime)) {
            return -1;
        }
        ret = d_manager_p->dispatch(minTime, flags);
        bdet_TimeInterval now = bdetu_SystemTime::now();
        if (now >= minTime) {
            bsl::vector<bcec_TimeQueueItem<bdef_Function<void (*)()> > >
                requests(d_allocator_p);
            d_timers.popLE(now, &requests);
            int numTimers = requests.size();
            for (int i = 0; i < numTimers; ++i) {
                requests[i].data()();
            }

            return numTimers + (ret >= 0 ? ret : 0);
        }
        else {
            return ret;
        }
    }
    else {
        if (!d_manager_p->numEvents()) {
            return 0;
        }
        return d_manager_p->dispatch(flags);
    }
}

int bteso_TcpTimerEventManager::registerSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        bteso_EventType::Type             eventType,
        const Callback&                   callBack)
{
    return
        d_manager_p->registerSocketEvent(handle, eventType, callBack);
}

void *bteso_TcpTimerEventManager::registerTimer(
        const bdet_TimeInterval& timeout,
        const Callback&          callback)
{
    return (void*)d_timers.add(timeout, callback);
}

int bteso_TcpTimerEventManager::rescheduleTimer(
        const void               *timerId,
        const bdet_TimeInterval&  expiryTime)
{
    return d_timers.update((int)(bsls_PlatformUtil::IntPtr) timerId,
                           expiryTime);
}

void bteso_TcpTimerEventManager::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        bteso_EventType::Type             event)
{
    d_manager_p->deregisterSocketEvent(handle, event);
}

void bteso_TcpTimerEventManager::deregisterSocket(
        const bteso_SocketHandle::Handle& handle)
{
    d_manager_p->deregisterSocket(handle);
}

void bteso_TcpTimerEventManager::deregisterAllSocketEvents() {
    d_manager_p->deregisterAll();
}

void bteso_TcpTimerEventManager::deregisterTimer(const void *handle)
{
    d_timers.remove((int) (bsls_PlatformUtil::IntPtr) handle);
}

void bteso_TcpTimerEventManager::deregisterAllTimers() {
    d_timers.removeAll();
}

void bteso_TcpTimerEventManager::deregisterAll() {
    deregisterAllSocketEvents();
    deregisterAllTimers();
}

// ACCESSORS
int bteso_TcpTimerEventManager::isRegistered(
        const bteso_SocketHandle::Handle& handle,
        bteso_EventType::Type             eventType) const
{
    return d_manager_p->isRegistered(handle, eventType);
}

int bteso_TcpTimerEventManager::numEvents() const {
    return d_timers.length() + d_manager_p->numEvents();
}

int bteso_TcpTimerEventManager::numSocketEvents(
        const bteso_SocketHandle::Handle& handle) const {
    return d_manager_p->numSocketEvents(handle);
}

int bteso_TcpTimerEventManager::numTimers() const {
    return d_timers.length();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
