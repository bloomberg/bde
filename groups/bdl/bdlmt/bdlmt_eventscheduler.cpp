// bdlmt_eventscheduler.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_eventscheduler.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_eventscheduler_cpp,"$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlt_timeunitratio.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_systemtime.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>

// Implementation note: When casting, we often cast through 'void *' or
// 'const void *' to avoid getting alignment warnings.

namespace BloombergLP {

// STATIC FUNCTIONS
static inline
void defaultDispatcherFunction(const bsl::function<void()>& callback)
{
    callback();
}

static inline
bsl::function<bsls::TimeInterval()> createDefaultCurrentTimeFunctor(
                                        bsls::SystemClockType::Enum clockType)
{
    // Must cast the pointer to 'now' to the correct signature so that the
    // correct now function is passed to the bind template.
    return bdlf::BindUtil::bind(
              static_cast<bsls::TimeInterval (*)(bsls::SystemClockType::Enum)>(
                                                      &bsls::SystemTime::now),
              clockType);
}

namespace bdlmt {
                            // --------------------
                            // class EventScheduler
                            // --------------------

// PRIVATE MANIPULATORS
bsls::Types::Int64 EventScheduler::chooseNextEvent(bsls::Types::Int64 *now)
{
    BSLS_ASSERT(0 != d_currentRecurringEvent || 0 != d_currentEvent);

    bsls::Types::Int64 t = 0;

    if (0 == d_currentRecurringEvent) {
        if (*now <= (t = d_currentEvent->key())) {
            *now = d_currentTimeFunctor().totalMicroseconds();
        }
    }
    else if (0 == d_currentEvent) {
        if (*now <= (t = d_currentRecurringEvent->key())) {
            *now = d_currentTimeFunctor().totalMicroseconds();
        }
    }
    else {
        bsls::Types::Int64 recurringEventTime = d_currentRecurringEvent->key();
        bsls::Types::Int64 eventTime          = d_currentEvent->key();

        // Prefer overdue events over overdue clocks if running behind.

        *now = d_currentTimeFunctor().totalMicroseconds();
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

void EventScheduler::dispatchEvents()
{
    bsls::Types::Int64 now = d_currentTimeFunctor().totalMicroseconds();

    while (1) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

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

        bsls::Types::Int64 t = chooseNextEvent(&now);

        if (t > now) {
            releaseCurrentEvents();
            bsls::TimeInterval w;
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

void EventScheduler::releaseCurrentEvents()
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
EventScheduler::EventScheduler(bslma::Allocator *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator, 
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

EventScheduler::EventScheduler(bsls::SystemClockType::Enum  clockType,
                               bslma::Allocator            *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(clockType)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_clockType(clockType)
{
}

EventScheduler::EventScheduler(
                          const EventScheduler::Dispatcher&  dispatcherFunctor,
                          bslma::Allocator                  *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

EventScheduler::EventScheduler(
                          const EventScheduler::Dispatcher&  dispatcherFunctor,
                          bsls::SystemClockType::Enum        clockType,
                          bslma::Allocator                  *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(clockType)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_clockType(clockType)
{
}

EventScheduler::~EventScheduler()
{
    BSLS_ASSERT(bslmt::ThreadUtil::invalidHandle() == d_dispatcherThread);
}

// MANIPULATORS
int EventScheduler::start()
{
    bslmt::ThreadAttributes attr;

    return start(attr);
}

int EventScheduler::start(const bslmt::ThreadAttributes& threadAttributes)
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    // Implementation note: d_dispatcherMutex is in a lock hierarchy with
    // d_mutex and must be locked first.
    bslmt::LockGuard<bslmt::Mutex> dispatcherLock(&d_dispatcherMutex);
    
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_running ||
        bslmt::ThreadUtil::invalidHandle() != d_dispatcherThread) {
        return 0;                                                     // RETURN
    }

    bslmt::ThreadAttributes modAttr(threadAttributes);
    modAttr.setDetachedState(bslmt::ThreadAttributes::e_CREATE_JOINABLE);

    if (bslmt::ThreadUtil::create(
                &d_dispatcherThread,
                modAttr,
                bdlf::BindUtil::bind(&EventScheduler::dispatchEvents, this))) {
        return -1;                                                    // RETURN
    }

    d_running = true;
    return 0;
}

void EventScheduler::stop()
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    // Implementation note: d_dispatcherMutex is in a lock hierarchy with
    // d_mutex and must be locked first.
    bslmt::LockGuard<bslmt::Mutex> dispatcherLock(&d_dispatcherMutex);

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (!d_running) {
        return;                                                       // RETURN
    }

    d_running = false;
    d_queueCondition.signal();

    lock.release()->unlock();

    bslmt::ThreadUtil::join(d_dispatcherThread);
    d_dispatcherThread = bslmt::ThreadUtil::invalidHandle();
}

void
EventScheduler::scheduleEvent(EventHandle                  *event,
                              const bsls::TimeInterval&     epochTime,
                              const bsl::function<void()>&  callback)
{
    bool newTop;

    d_eventQueue.addR(&event->d_handle,
                      epochTime.totalMicroseconds(),
                      callback,
                      &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void EventScheduler::scheduleEventRaw(Event                        **event,
                                      const bsls::TimeInterval&      epochTime,
                                      const bsl::function<void()>&   callback)
{
    bool newTop;

    d_eventQueue.addRawR((EventQueue::Pair **)event,
                         epochTime.totalMicroseconds(),
                         callback,
                         &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
EventScheduler::scheduleRecurringEvent(
                                  RecurringEventHandle         *event,
                                  const bsls::TimeInterval&     interval,
                                  const bsl::function<void()>&  callback,
                                  const bsls::TimeInterval&     startEpochTime)
{
    BSLS_ASSERT(0 != interval);

    bsls::Types::Int64 stime(startEpochTime.totalMicroseconds());
    if (0 == stime) {
        stime = (d_currentTimeFunctor() + interval).totalMicroseconds();
    }

    RecurringEventData recurringEventData(callback, interval);

    bool newTop;

    d_recurringQueue.addR(&event->d_handle,
                          stime,
                          recurringEventData,
                          &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
EventScheduler::scheduleRecurringEventRaw(
                                 RecurringEvent               **event,
                                 const bsls::TimeInterval&      interval,
                                 const bsl::function<void()>&   callback,
                                 const bsls::TimeInterval&      startEpochTime)
{
    BSLS_ASSERT(0 != interval);

    bsls::Types::Int64 stime(startEpochTime.totalMicroseconds());
    if (0 == stime) {
        stime = (d_currentTimeFunctor() + interval).totalMicroseconds();
    }

    RecurringEventData recurringEventData(callback, interval);

    bool newTop;
    d_recurringQueue.addRawR((RecurringEventQueue::Pair **)event,
                             stime,
                             recurringEventData,
                             &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

int EventScheduler::cancelEvent(EventHandle *handle)
{
    if (0 == (const Event *) *handle) {
        return EventQueue::e_INVALID;                                 // RETURN
    }

    int ret = cancelEvent((const Event *) *handle);
    handle->release();
    return ret;
}

int EventScheduler::cancelEvent(RecurringEventHandle *handle)
{
    if (0 == (const RecurringEvent *) *handle) {
        return RecurringEventQueue::e_INVALID;                        // RETURN
    }

    int ret = cancelEvent((const RecurringEvent *) *handle);
    handle->release();
    return ret;
}

int EventScheduler::cancelEventAndWait(const RecurringEvent *handle)
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    const RecurringEventQueue::Pair *itemPtr =
                       reinterpret_cast<const RecurringEventQueue::Pair *>(
                                       reinterpret_cast<const void *>(handle));

    int ret = d_recurringQueue.remove(itemPtr);

    // Cannot 'return' if '0 == ret': since the event is recurring, it may be
    // the currently executing event even if it's still in the queue.

    if (RecurringEventQueue::e_INVALID == ret) {
        return ret;                                                   // RETURN
    }

    bsls::Types::Int64 eventTime = itemPtr->key();

    // Wait until the next iteration if currently executing the event.

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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

int EventScheduler::cancelEventAndWait(const Event *handle)
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    const EventQueue::Pair *itemPtr =
                             reinterpret_cast<const EventQueue::Pair *>(
                                       reinterpret_cast<const void *>(handle));

    int ret = d_eventQueue.remove(itemPtr);
    if (EventQueue::e_NOT_FOUND != ret) {
        return ret;                                                   // RETURN
    }

    // At this point, we know we could not remove the item because it was not
    // in the list.  Check whether the currently executing event is the one we
    // wanted to cancel; if it is, wait until the next iteration.

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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

int EventScheduler::cancelEventAndWait(EventHandle *handle)
{
    if (0 == (const Event *) *handle) {
        return EventQueue::e_INVALID;                                 // RETURN
    }

    int ret = cancelEventAndWait((const Event *) *handle);
    handle->release();
    return ret;
}

int EventScheduler::cancelEventAndWait(RecurringEventHandle *handle)
{
    if (0 == (const RecurringEvent *) *handle) {
        return RecurringEventQueue::e_INVALID;                        // RETURN
    }

    int ret = cancelEventAndWait((const RecurringEvent *) *handle);
    handle->release();
    return ret;
}

int EventScheduler::rescheduleEvent(const Event               *handle,
                                    const bsls::TimeInterval&  newEpochTime)
{
    const EventQueue::Pair *h = reinterpret_cast<const EventQueue::Pair *>(
                                       reinterpret_cast<const void *>(handle));

    bool isNewTop;
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int ret = d_eventQueue.updateR(h,
                                   newEpochTime.totalMicroseconds(),
                                   &isNewTop);

    if (0 == ret && isNewTop) {
        d_queueCondition.signal();
    }
    return ret;
}

int EventScheduler::rescheduleEventAndWait(
                                       const Event               *handle,
                                       const bsls::TimeInterval&  newEpochTime)
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    const EventQueue::Pair *h = reinterpret_cast<const EventQueue::Pair *>(
                                       reinterpret_cast<const void *>(handle));
    int ret;

    {
        bool isNewTop;
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        ret = d_eventQueue.updateR(h,
                                   newEpochTime.totalMicroseconds(),
                                   &isNewTop);

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

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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

void EventScheduler::cancelAllEvents()
{
    d_eventQueue.removeAll();
    d_recurringQueue.removeAll();
}

void EventScheduler::cancelAllEventsAndWait()
{
    BSLS_ASSERT(!bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                            d_dispatcherThread));

    d_eventQueue.removeAll();
    d_recurringQueue.removeAll();

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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

                     // ----------------------------------
                     // class EventSchedulerTestTimeSource
                     // ----------------------------------

// CREATORS
EventSchedulerTestTimeSource::EventSchedulerTestTimeSource(
                                                     EventScheduler *scheduler)
: d_currentTime(bsls::SystemTime::now(scheduler->d_clockType)
                + 1000 * bdlt::TimeUnitRatio::k_SECONDS_PER_DAY)
, d_scheduler_p(scheduler)
{
    // The event scheduler is constructed with a "now" that is 1000 days in the
    // future.  This point in time is arbitrary, but is chosen to ensure that
    // in any reasonable test driver, the system clock (which controls the
    // the scheduler's condition variable) will always lag behind the test time
    // source.
    //
    // If the system clock were ever to catch up with the test time source, the
    // 'EventScheduler::dispatchEvents' could go into a tight loop waiting for
    // the next event instead of sleeping until the next call to
    // 'EventSchedulerTestTimeSource::advanceTime'.  See the call to
    // 'timedWait' in 'EventScheduler::dispatchEvents'.

    BSLS_ASSERT(0 != scheduler);

    // Bind the member function 'now' to 'this', and let the scheduler call
    // this binder as its current time callback.

    d_scheduler_p->d_currentTimeFunctor = bdlf::MemFnUtil::memFn(
                                            &EventSchedulerTestTimeSource::now,
                                            this);
}

// MANIPULATORS
bsls::TimeInterval EventSchedulerTestTimeSource::advanceTime(
                                                     bsls::TimeInterval amount)
{
    BSLS_ASSERT(amount > 0);
    bsls::TimeInterval ret;

    {
        // This scope limits how long we lock 'd_currentTimeMutex'

        bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);

        d_currentTime += amount;

        // Returning the new time allows an atomic 'advance' + 'now' operation.
        // This feature may not be necessary.
        ret = d_currentTime;
    }


    {
        // This scope limits how long we lock the scheduler's mutex

        bslmt::LockGuard<bslmt::Mutex> lock(&d_scheduler_p->d_mutex);

        // Now that the time has changed, signal the scheduler's condition
        // variable so that the event dispatcher thread can be alerted to the
        // change.
        d_scheduler_p->d_queueCondition.signal();
    }

    return ret;
}

// ACCESSORS
bsls::TimeInterval EventSchedulerTestTimeSource::now()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);
    return d_currentTime;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
