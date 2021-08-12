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

#include <bdlt_timeunitratio.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_systemtime.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>

// Implementation note: The 'EventData' and 'RecurringEventData' structures
// contain two different 'bsl::function's.  The first one, named 'd_callback',
// is the user-provided function that is called when the event occurs.  The
// second one, named 'd_nowOffset', is called when the clock of the
// 'EventScheduler' indicates that it is time for the event to occur.  It
// returns the number of microseconds until the event should actually occur.
// The reason for having two different functions is that the event can be
// scheduled on a different clock than the one that the 'EventScheduler' uses,
// and the two clocks may run at different rates. This "two level" check
// ensures that the events do not occur before the scheduled time on the clock
// where the time was specified.

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

static inline
bsls::Types::Uint64 invalidThreadId()
    // Return a value that is guaranteed never to be a valid thread id.
{
    return bslmt::ThreadUtil::idAsUint64(
            bslmt::ThreadUtil::handleToId(bslmt::ThreadUtil::invalidHandle()));
}

namespace bdlmt {

                 // =======================================
                 // class EventSchedulerTestTimeSource_Data
                 // =======================================

class EventSchedulerTestTimeSource_Data {
    // This 'class' provides storage for the current time and a mutex to
    // protect access to the current time.

    // DATA
    bsls::TimeInterval   d_currentTime;       // the current time

    mutable bslmt::Mutex d_currentTimeMutex;  // mutex used to synchronize
                                              // 'd_currentTime' access

  private:
    // NOT IMPLEMENTED
    EventSchedulerTestTimeSource_Data(
                                     const EventSchedulerTestTimeSource_Data&);
    EventSchedulerTestTimeSource_Data& operator=(
                                     const EventSchedulerTestTimeSource_Data&);

  public:
    // CREATORS
    explicit
    EventSchedulerTestTimeSource_Data(bsls::TimeInterval currentTime);
        // Create a test time-source data object that will store the
        // "system-time", initialized to the specified 'currentTime'.

    //! ~EventSchedulerTestTimeSource_Data() = default;
        // Destroy this object.

    // MANIPULATORS
    bsls::TimeInterval advanceTime(bsls::TimeInterval amount);
        // Advance this object's current-time value by the specified 'amount'
        // of time.  Return the updated current-time value.  The behavior is
        // undefined unless 'amount' is positive, and 'now + amount' is within
        // the range that can be represented with a 'bsls::TimeInterval'.

    // ACCESSORS
    bsls::TimeInterval currentTime() const;
        // Return this object's current-time value.
};

// CREATORS
EventSchedulerTestTimeSource_Data::EventSchedulerTestTimeSource_Data(
                                                bsls::TimeInterval currentTime)
: d_currentTime(currentTime)
{
}

// MANIPULATORS
bsls::TimeInterval EventSchedulerTestTimeSource_Data::advanceTime(
                                                     bsls::TimeInterval amount)
{
    BSLS_ASSERT(amount > 0);

    bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);
    d_currentTime += amount;
    return d_currentTime;
}

// ACCESSORS
bsls::TimeInterval EventSchedulerTestTimeSource_Data::currentTime() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);
    return d_currentTime;
}

                           // --------------------
                           // class EventScheduler
                           // --------------------

// PRIVATE CLASS METHODS
bsls::Types::Int64 EventScheduler::returnZero()
{
    return 0;
}

bsls::Types::Int64 EventScheduler::returnZeroInt(int)
{
    return 0;
}

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
    // set the dispatcher thread id
    d_dispatcherThreadId.storeRelease(bslmt::ThreadUtil::selfIdAsUint64());

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
            // reset the dispatcher thread id, as the same id may be reused by
            // the OS after the thread terminates
            d_dispatcherThreadId.storeRelease(invalidThreadId());

            return;                                                   // RETURN
        }

        BSLS_ASSERT(0 == d_currentRecurringEvent);
        BSLS_ASSERT(0 == d_currentEvent);

        d_recurringQueue.frontRaw(&d_currentRecurringEvent);
        d_eventQueue.frontRaw(&d_currentEvent);

        if (0 == d_currentRecurringEvent && 0 == d_currentEvent) {
            ++d_waitCount;
            d_queueCondition.wait(&d_mutex);
            continue;
        }

        bsls::Types::Int64 t = chooseNextEvent(&now);

        if (t > now) {
            releaseCurrentEvents();
            bsls::TimeInterval w;
            w.addMicroseconds(t);
            ++d_waitCount;
            d_queueCondition.timedWait(&d_mutex, w);
            continue;
        }

        // We have an event due for execution.
        BSLS_ASSERT(0 != d_currentEvent || 0 != d_currentRecurringEvent);

        if (d_currentRecurringEvent) {
            RecurringEventData& data = d_currentRecurringEvent->data();
            bsls::Types::Int64 nowOffset = data.d_nowOffset(data.d_eventIdx);
            if (nowOffset <= 0) {
                ++data.d_eventIdx;
                int ret = d_recurringQueue.updateR(
                                      d_currentRecurringEvent,
                                      t + data.d_interval.totalMicroseconds());
                if (0 == ret) {
                    lock.release()->unlock();
                    d_dispatcherFunctor(data.d_callback);
                }
            }
            else {
                int ret = d_recurringQueue.updateR(
                                      d_currentRecurringEvent,
                                      now + nowOffset);
                (void) ret;
                d_recurringQueue.releaseReferenceRaw(d_currentRecurringEvent);
                d_currentRecurringEvent = 0;
            }
        }
        else { // d_currentEvent
            EventData& data = d_currentEvent->data();
            bsls::Types::Int64 nowOffset = data.d_nowOffset();
            if (nowOffset <= 0) {
                int ret = d_eventQueue.remove(d_currentEvent);
                if (0 == ret) {
                    lock.release()->unlock();
                    d_dispatcherFunctor(data.d_callback);
                }
            }
            else {
                int ret = d_eventQueue.updateR(
                                d_currentEvent,
                                now + nowOffset);
                (void) ret;
                d_eventQueue.releaseReferenceRaw(d_currentEvent);
                d_currentEvent = 0;
            }
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

void
EventScheduler::scheduleEvent(EventHandle               *event,
                              const bsls::TimeInterval&  epochTime,
                              const EventData&           eventData)
{
    bool newTop;

    d_eventQueue.addR(&event->d_handle,
                      epochTime.totalMicroseconds(),
                      eventData,
                      &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void EventScheduler::scheduleEventRaw(EventHandle               **event,
                                      const bsls::TimeInterval&   epochTime,
                                      const EventData&            eventData)
{
    bool newTop;

    d_eventQueue.addRawR((EventQueue::Pair **)event,
                         epochTime.totalMicroseconds(),
                         eventData,
                         &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
EventScheduler::scheduleRecurringEvent(
                                     RecurringEventHandle      *event,
                                     const RecurringEventData&  eventData,
                                     const bsls::TimeInterval&  startEpochTime)
{
    bsls::Types::Int64 stime(startEpochTime.totalMicroseconds());
    if (0 == stime) {
        stime =
           (d_currentTimeFunctor() + eventData.d_interval).totalMicroseconds();
    }

    bool newTop;
    d_recurringQueue.addR(&event->d_handle,
                          stime,
                          eventData,
                          &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

void
EventScheduler::scheduleRecurringEventRaw(
                                 RecurringEvent            **event,
                                 const RecurringEventData&   eventData,
                                 const bsls::TimeInterval&   startEpochTime)
{
    bsls::Types::Int64 stime(startEpochTime.totalMicroseconds());
    if (0 == stime) {
        stime = (d_currentTimeFunctor() +
                                    eventData.d_interval).totalMicroseconds();
    }

    bool newTop;
    d_recurringQueue.addRawR((RecurringEventQueue::Pair **)event,
                             stime,
                             eventData,
                             &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
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
, d_dispatcherThreadId(invalidThreadId())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
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
, d_dispatcherThreadId(invalidThreadId())
, d_queueCondition(clockType)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
EventScheduler::EventScheduler(
                             const bsl::chrono::system_clock&,
                             bslma::Allocator                 *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(bsls::SystemClockType::e_REALTIME)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

EventScheduler::EventScheduler(
                             const bsl::chrono::steady_clock&,
                             bslma::Allocator                  *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                           bsls::SystemClockType::e_MONOTONIC))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(bsls::SystemClockType::e_MONOTONIC)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

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
, d_dispatcherThreadId(invalidThreadId())
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
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
, d_dispatcherThreadId(invalidThreadId())
, d_queueCondition(clockType)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
EventScheduler::EventScheduler(
                          const EventScheduler::Dispatcher&  dispatcherFunctor,
                          const bsl::chrono::system_clock&,
                          bslma::Allocator                  *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(bsls::SystemClockType::e_REALTIME)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

EventScheduler::EventScheduler(
                          const EventScheduler::Dispatcher&  dispatcherFunctor,
                          const bsl::chrono::steady_clock&,
                          bslma::Allocator                  *basicAllocator)
: d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                           bsls::SystemClockType::e_MONOTONIC))
, d_eventQueue(basicAllocator)
, d_recurringQueue(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_queueCondition(bsls::SystemClockType::e_MONOTONIC)
, d_running(false)
, d_dispatcherAwaited(false)
, d_currentRecurringEvent(0)
, d_currentEvent(0)
, d_waitCount(0)
, d_clockType(bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

EventScheduler::~EventScheduler()
{
    BSLS_ASSERT(bslmt::ThreadUtil::invalidHandle() == d_dispatcherThread);
}

// MANIPULATORS
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

    if (h) {
        h->data().d_nowOffset = returnZero;
    }

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

        if (h) {
            h->data().d_nowOffset = returnZero;
        }

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

void EventScheduler::scheduleEventRaw(Event                        **event,
                                      const bsls::TimeInterval&      epochTime,
                                      const bsl::function<void()>&   callback)
{
    bool newTop;

    d_eventQueue.addRawR((EventQueue::Pair **)event,
                         epochTime.totalMicroseconds(),
                         EventData(callback, returnZero),
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
    // Note that when this review is converted to an assert, the following
    // assert is redundant and can be removed.
    BSLS_REVIEW(1 <= interval.totalMicroseconds());
    BSLS_ASSERT(0 != interval);

    bsls::Types::Int64 stime(startEpochTime.totalMicroseconds());
    if (0 == stime) {
        stime = (d_currentTimeFunctor() + interval).totalMicroseconds();
    }

    RecurringEventData eventData(interval, callback, returnZeroInt);

    bool newTop;
    d_recurringQueue.addRawR((RecurringEventQueue::Pair **)event,
                             stime,
                             eventData,
                             &newTop);

    if (newTop) {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queueCondition.signal();
    }
}

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

    if (bslmt::ThreadUtil::createWithAllocator(
                &d_dispatcherThread,
                modAttr,
                bdlf::BindUtil::bind(&EventScheduler::dispatchEvents, this),
                allocator())) {
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

                    // ----------------------------------
                    // class EventSchedulerTestTimeSource
                    // ----------------------------------

// CREATORS
EventSchedulerTestTimeSource::EventSchedulerTestTimeSource(
                                                     EventScheduler *scheduler)
: d_scheduler_p(scheduler)
{
    BSLS_ASSERT(0 != scheduler);

    // The event scheduler is constructed with a "now" that is 1000 days in the
    // future.  This point in time is arbitrary, but is chosen to ensure that
    // in any reasonable test driver, the system clock (which controls the
    // scheduler's condition variable) will always lag behind the test time
    // source.
    //
    // If the system clock were ever to catch up with the test time source, the
    // 'EventScheduler::dispatchEvents' could go into a tight loop waiting for
    // the next event instead of sleeping until the next call to
    // 'EventSchedulerTestTimeSource::advanceTime'.  See the call to
    // 'timedWait' in 'EventScheduler::dispatchEvents'.

    // The following uses the default allocator since the created object's
    // lifetime is shared between 'EventSchedulerTestTimeSource' and the
    // associated 'EventScheduler', so the data may outlive either individual
    // object.

    d_data_p = bsl::make_shared<EventSchedulerTestTimeSource_Data>(
                                bsls::SystemTime::now(scheduler->d_clockType)
                              + 1000 * bdlt::TimeUnitRatio::k_SECONDS_PER_DAY);

    // Bind the member function 'now' to 'this', and let the scheduler call
    // this binder as its current time callback.

    d_scheduler_p->d_currentTimeFunctor = bdlf::BindUtil::bind(
                               &EventSchedulerTestTimeSource_Data::currentTime,
                               d_data_p);
}

// MANIPULATORS
bsls::TimeInterval EventSchedulerTestTimeSource::advanceTime(
                                                     bsls::TimeInterval amount)
{
    BSLS_ASSERT(amount > 0);

    // Returning the new time allows an atomic 'advance' + 'now' operation.
    // This feature may not be necessary.

    bsls::TimeInterval ret = d_data_p->advanceTime(amount);

    unsigned int waitCount;
    {
        // This scope limits how long we lock the scheduler's mutex

        bslmt::LockGuard<bslmt::Mutex> lock(&d_scheduler_p->d_mutex);

        waitCount = d_scheduler_p->d_waitCount;

        // Now that the time has changed, signal the scheduler's condition
        // variable so that the event dispatcher thread can be alerted to the
        // change.

        d_scheduler_p->d_queueCondition.signal();
    }

    {
        // To avoid adding logic to the normal (non-testing) use of this event
        // scheduler, a simple yield-spin is performed to wait for the
        // scheduling of events due to this time change to complete.

        unsigned int currentWaitCount = waitCount;
        while (currentWaitCount == waitCount) {
            bslmt::ThreadUtil::yield();
            bslmt::LockGuard<bslmt::Mutex> lock(&d_scheduler_p->d_mutex);
            currentWaitCount = d_scheduler_p->d_waitCount;
            if (!d_scheduler_p->d_running) {
                currentWaitCount = ~waitCount;  // exit loop
            }
        }
    }

    return ret;
}

// ACCESSORS
bsls::TimeInterval EventSchedulerTestTimeSource::now() const
{
    return d_data_p->currentTime();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
