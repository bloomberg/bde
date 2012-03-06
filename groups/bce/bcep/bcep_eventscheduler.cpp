// bcep_eventscheduler.cpp                                            -*-C++-*-
#include <bcep_eventscheduler.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_eventscheduler_cpp,"$Id$ $CSID$")

#include <bcema_sharedptr.h>
#include <bcemt_lockguard.h>

#include <bdef_bind.h>
#include <bdes_bitutil.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>

namespace BloombergLP {

// STATIC FUNCTIONS
static inline
void defaultDispatcherFunction(const bdef_Function<void (*)()>& callback)
{
    callback();
}

                             // -------------------------
                             // class bcep_EventScheduler
                             // -------------------------

// PRIVATE MANIPULATORS
bsls_PlatformUtil::Int64 bcep_EventScheduler::chooseNextEvent(
                                                 bsls_PlatformUtil::Int64 *now)
{
    BSLS_ASSERT(0 != d_currentRecurringEvent || 0 != d_currentEvent);

    bsls_PlatformUtil::Int64 t = 0;

    if (0 == d_currentRecurringEvent) {
        if (*now <= (t = d_currentEvent->key())) {
            *now = bdetu_SystemTime::now().totalMicroseconds();
        }
    }
    else if (0 == d_currentEvent) {
        if (*now <= (t = d_currentRecurringEvent->key())) {
            *now = bdetu_SystemTime::now().totalMicroseconds();
        }
    }
    else {
        bsls_PlatformUtil::Int64 recurringEventTime =
                                                d_currentRecurringEvent->key();
        bsls_PlatformUtil::Int64 eventTime = d_currentEvent->key();

        // Prefer overdue events over overdue clocks if running behind.

        *now = bdetu_SystemTime::now().totalMicroseconds();
        if (eventTime < recurringEventTime || eventTime < *now) {
            d_recurringQueue.releaseReferenceRaw(d_currentRecurringEvent);
            d_currentRecurringEvent = 0;
            t = eventTime;
        }
        else {
            d_eventQueue.releaseReferenceRaw(d_currentEvent);
            d_currentEvent = 0;
            t = recurringEventTime;
        }
    }

    return t;
}

void bcep_EventScheduler::dispatchEvents()
{
    bsls_PlatformUtil::Int64 now = bdetu_SystemTime::now().totalMicroseconds();

    while (1) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

        // Get ready for the next iteration.

        releaseCurrentEvents();

        if (d_dispatcherAwaited) {
            d_dispatcherAwaited = false;
            d_iterationCondition.broadcast();
        }

        // Now proceed with the next iteration.

        if (!d_running) {
            return;                                                   // RETURN
        }

        BSLS_ASSERT(0 == d_currentRecurringEvent);
        BSLS_ASSERT(0 == d_currentEvent);

        d_recurringQueue.frontRaw(&d_currentRecurringEvent);
        d_eventQueue.frontRaw(&d_currentEvent);

        if (0 == d_currentRecurringEvent && 0 == d_currentEvent) {
            d_queueCondition.wait(&d_mutex);
            continue;
        }

        bsls_PlatformUtil::Int64 t = chooseNextEvent(&now);

        if (t > now) {
            releaseCurrentEvents();
            bdet_TimeInterval w;
            w.addMicroseconds(t);
            d_queueCondition.timedWait(&d_mutex, w);
            continue;
        }

        // We have an event due for execution.

        if (d_currentRecurringEvent) {
            RecurringEventData& data = d_currentRecurringEvent->data();
            int ret = d_recurringQueue.updateR(
                                          d_currentRecurringEvent,
                                          t + data.second.totalMicroseconds());
            if (0 == ret) {
                lock.release()->unlock();
                d_dispatcherFunctor(data.first);
            }
            continue;
        }
        BSLS_ASSERT(0 != d_currentEvent);
        int ret = d_eventQueue.remove(d_currentEvent);
        if (0 == ret) {
            lock.release()->unlock();
            d_dispatcherFunctor(d_currentEvent->data());
        }
    }

}

void bcep_EventScheduler::releaseCurrentEvents()
{
    if (d_currentRecurringEvent) {
        d_recurringQueue.releaseReferenceRaw(d_currentRecurringEvent);
        d_currentRecurringEvent = 0;
    }

    if (d_currentEvent) {
        d_eventQueue.releaseReferenceRaw(d_currentEvent);
        d_currentEvent = 0;
    }
}

// CREATORS
bcep_EventScheduler::bcep_EventScheduler(bslma_Allocator *basicAllocator)
: d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(&defaultDispatcherFunction)
, d_dispatcherThread(bcemt_ThreadUtil::invalidHandle())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
{
}

bcep_EventScheduler::bcep_EventScheduler(
                        const bcep_EventScheduler::Dispatcher&  dispatcher,
                        bslma_Allocator                        *basicAllocator)
: d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(dispatcher)
, d_dispatcherThread(bcemt_ThreadUtil::invalidHandle())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
{
}

bcep_EventScheduler::~bcep_EventScheduler()
{
    BSLS_ASSERT(bcemt_ThreadUtil::invalidHandle() == d_dispatcherThread);
}

// MANIPULATORS
int bcep_EventScheduler::start()
{
    bcemt_Attribute attr;

    return start(attr);
}

int bcep_EventScheduler::start(const bcemt_Attribute& threadAttributes)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    if (d_running || bcemt_ThreadUtil::invalidHandle() != d_dispatcherThread) {
        return 0;                                                     // RETURN
    }

    bcemt_Attribute modAttr(threadAttributes);
    modAttr.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);

    if (bcemt_ThreadUtil::create(
            &d_dispatcherThread,
            modAttr,
            bdef_BindUtil::bind(&bcep_EventScheduler::dispatchEvents, this))) {
        return -1;                                                    // RETURN
    }

    d_running = true;
    return 0;
}

void bcep_EventScheduler::stop()
{
    BSLS_ASSERT(!bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                           d_dispatcherThread));

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    if (!d_running) {
        return;                                                       // RETURN
    }

    d_running = false;
    d_queueCondition.signal();

    lock.release()->unlock();

    bcemt_ThreadUtil::join(d_dispatcherThread);
    d_dispatcherThread = bcemt_ThreadUtil::invalidHandle();
}

void
bcep_EventScheduler::scheduleEvent(EventHandle                      *event,
                                   const bdet_TimeInterval&          timer,
                                   const bdef_Function<void (*)()>&  callback)
{
    bool newTop;

    d_eventQueue.addR(&event->d_handle,
                      timer.totalMicroseconds(),
                      callback,
                      &newTop);

    if (newTop) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
bcep_EventScheduler::scheduleEventRaw(
                                   Event                            **event,
                                   const bdet_TimeInterval&           timer,
                                   const bdef_Function<void (*)()>&   callback)
{
    bool newTop;

    d_eventQueue.addRawR((EventQueue::Pair **)event,
                         timer.totalMicroseconds(),
                         callback,
                         &newTop);

    if (newTop) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
bcep_EventScheduler::scheduleRecurringEvent(
                                   RecurringEventHandle             *event,
                                   const bdet_TimeInterval&          interval,
                                   const bdef_Function<void (*)()>&  callback,
                                   const bdet_TimeInterval&          startTime)
{
    BSLS_ASSERT(0 != interval);

    bsls_PlatformUtil::Int64 stime(startTime.totalMicroseconds());
    if (0 == stime) {
        stime = (bdetu_SystemTime::now() + interval).totalMicroseconds();
    }

    RecurringEventData recurringEventData(callback, interval);

    bool newTop;

    d_recurringQueue.addR(&event->d_handle,
                          stime,
                          recurringEventData,
                          &newTop);

    if (newTop) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
bcep_EventScheduler::scheduleRecurringEventRaw(
                                  RecurringEvent                   **event,
                                  const bdet_TimeInterval&           interval,
                                  const bdef_Function<void (*)()>&   callback,
                                  const bdet_TimeInterval&           startTime)
{
    BSLS_ASSERT(0 != interval);

    bsls_PlatformUtil::Int64 stime(startTime.totalMicroseconds());
    if (0 == stime) {
        stime = (bdetu_SystemTime::now() + interval).totalMicroseconds();
    }

    RecurringEventData recurringEventData(callback, interval);

    bool newTop;
    d_recurringQueue.addRawR((RecurringEventQueue::Pair **)event,
                             stime,
                             recurringEventData,
                             &newTop);

    if (newTop) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

int bcep_EventScheduler::cancelEvent(EventHandle *handle)
{
    if (0 == (const Event *) *handle) {
        return EventQueue::BCEC_INVALID;                              // RETURN
    }

    int ret = cancelEvent((const Event *) *handle);
    handle->release();
    return ret;
}

int bcep_EventScheduler::cancelEvent(RecurringEventHandle *handle)
{
    if (0 == (const RecurringEvent *) *handle) {
        return RecurringEventQueue::BCEC_INVALID;                     // RETURN
    }

    int ret = cancelEvent((const RecurringEvent *) *handle);
    handle->release();
    return ret;
}

int bcep_EventScheduler::cancelEventAndWait(const RecurringEvent *handle)
{
    BSLS_ASSERT(!bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                           d_dispatcherThread));

    const RecurringEventQueue::Pair *itemPtr =
                   reinterpret_cast<const RecurringEventQueue::Pair *>(handle);

    int ret = d_recurringQueue.remove(itemPtr);

    // Cannot 'return' if '0 == ret': since the event is recurring, it may
    // be the currently executing event even if it's still in the queue.

    if (RecurringEventQueue::BCEC_INVALID == ret) {
        return ret;                                                   // RETURN
    }

    bsls_PlatformUtil::Int64 eventTime = itemPtr->key();

    // Wait until the next iteration if currently executing the event.

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    while (1) {
        if (0 == d_currentRecurringEvent
         || d_currentRecurringEvent->key() != eventTime) {
            break;
        }
        else {
            d_dispatcherAwaited = true;
            d_iterationCondition.wait(&d_mutex);
        }
    }

    return ret;
}

int bcep_EventScheduler::cancelEventAndWait(const Event *handle)
{
    BSLS_ASSERT(!bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                           d_dispatcherThread));

    const EventQueue::Pair *itemPtr =
                            reinterpret_cast<const EventQueue::Pair *>(handle);

    int ret = d_eventQueue.remove(itemPtr);
    if (EventQueue::BCEC_NOT_FOUND != ret) {
        return ret;                                                   // RETURN
    }

    // At this point, we know we could not remove the item because it was not
    // in the list.  Check whether the currently executing event is the
    // one we wanted to cancel; if it is, wait until the next iteration.

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    while (1) {
        if (d_currentEvent != itemPtr) {
            break;
        }
        else {
            d_dispatcherAwaited = true;
            d_iterationCondition.wait(&d_mutex);
        }
    }

    return ret;
}

int bcep_EventScheduler::cancelEventAndWait(EventHandle *handle)
{
    if (0 == (const Event *) *handle) {
        return EventQueue::BCEC_INVALID;                              // RETURN
    }

    int ret = cancelEventAndWait((const Event *) *handle);
    handle->release();
    return ret;
}

int bcep_EventScheduler::cancelEventAndWait(RecurringEventHandle *handle)
{
    if (0 == (const RecurringEvent *) *handle) {
        return RecurringEventQueue::BCEC_INVALID;                     // RETURN
    }

    int ret = cancelEventAndWait((const RecurringEvent *) *handle);
    handle->release();
    return ret;
}

int bcep_EventScheduler::rescheduleEvent(const Event              *handle,
                                         const bdet_TimeInterval&  newTime)
{
    const EventQueue::Pair *h =
                            reinterpret_cast<const EventQueue::Pair *>(handle);

    bool isNewTop;
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    int ret = d_eventQueue.updateR(h, newTime.totalMicroseconds(), &isNewTop);

    if (0 == ret && isNewTop) {
        d_queueCondition.signal();
    }
    return ret;
}

int bcep_EventScheduler::rescheduleEventAndWait(
                                            const Event              *handle,
                                            const bdet_TimeInterval&  newTime)
{
    BSLS_ASSERT(!bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                           d_dispatcherThread));

    const EventQueue::Pair *h =
                            reinterpret_cast<const EventQueue::Pair *>(handle);
    int ret;

    {
        bool isNewTop;
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        ret = d_eventQueue.updateR(h, newTime.totalMicroseconds(), &isNewTop);

        if (0 == ret) {
            if (isNewTop) {
                d_queueCondition.signal();
            }
            if (d_currentEvent != h) {
                return 0;                                             // RETURN
            }
        }
    }

    // Wait until event is rescheduled or dispatched.

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    while (1) {
        if (d_currentEvent!=h) {
            break;
        }
        else {
            d_dispatcherAwaited = true;
            d_iterationCondition.wait(&d_mutex);
        }
    }

    return ret;
}

void bcep_EventScheduler::cancelAllEvents()
{
    d_eventQueue.removeAll();
    d_recurringQueue.removeAll();
}

void bcep_EventScheduler::cancelAllEventsAndWait()
{
    BSLS_ASSERT(!bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                           d_dispatcherThread));

    d_eventQueue.removeAll();
    d_recurringQueue.removeAll();

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    while (1) {
        if (0 == d_currentEvent && 0 == d_currentRecurringEvent) {
            break;
        }
        else {
            d_dispatcherAwaited = true;
            d_iterationCondition.wait(&d_mutex);
        }
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
