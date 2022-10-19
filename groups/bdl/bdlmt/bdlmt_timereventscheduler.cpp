// bdlmt_timereventscheduler.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_timereventscheduler.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_timereventscheduler_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_systemtime.h>

#include <bdlt_timeunitratio.h>
#include <bdlb_bitutil.h>
#include <bdlf_bind.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>   // for 'CHAR_BIT'
#include <bsl_functional.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {

const int NUM_INDEX_BITS_DEFAULT = 17;
    // Default number of bits used to represent each 'bdlcc::TimeQueue'
    // handle.

const int NUM_INDEX_BITS_MIN = 8;
    // Minimum number of bits required to represent a 'bdlcc::TimeQueue'
    // handle.

int numBitsRequired(int value)
{
    BSLS_ASSERT(0 <= value);

    // Calculate the smallest number of bits required to represent the
    // specified 'value'.

    return static_cast<int>(  (sizeof(value) * CHAR_BIT)
                            - bdlb::BitUtil::numLeadingUnsetBits(
                                           static_cast<bsl::uint32_t>(value)));
}

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

}  // close unnamed namespace

namespace bdlmt {

                   // ====================================
                   // struct TimerEventSchedulerDispatcher
                   // ====================================

struct TimerEventSchedulerDispatcher {
    // This class just contains the method called to run the dispatcher
    // thread.  Once started, it infinite loops, either waiting for or
    // executing events.

    // CLASS METHODS
    static void dispatchEvents(TimerEventScheduler *scheduler);
};

extern "C" void *TimerEventSchedulerDispatcherThread(void *scheduler)
{
    TimerEventSchedulerDispatcher::dispatchEvents(
                                              (TimerEventScheduler*)scheduler);
    return scheduler;
}

void TimerEventSchedulerDispatcher::dispatchEvents(
                                                TimerEventScheduler* scheduler)
{
    BSLS_ASSERT(0 != scheduler);

    typedef TimerEventScheduler::ClockDataPtr  ClockDataPtr;
    typedef bdlcc::TimeQueueItem<ClockDataPtr> PendingClockItem;

    bsl::vector<PendingClockItem> pendingClockItems;

    while (1) {
        bsl::size_t clockLen;

        // This scope limits the life of several variables, including mutex
        // lock.

        {
            bslmt::LockGuard<bslmt::Mutex> lock(&scheduler->d_mutex);
            if (!scheduler->d_running.loadRelaxed()) {
                return;                                               // RETURN
            }
            ++scheduler->d_iterations;

            int newLengthClock = 0, newLengthEvent = 0;
            bsls::TimeInterval now = scheduler->d_currentTimeFunctor();

            // minTimeClock will be set only if newLengthClock > 0, similar
            // with minTimeEvent

            enum {
                MAX_PENDING_CLOCKS = 64,
                MAX_PENDING_EVENTS = 64
            };
            bsls::TimeInterval minTimeClock, minTimeEvent;

            scheduler->d_clockTimeQueue.popLE(now,
                                              MAX_PENDING_CLOCKS,
                                              &pendingClockItems,
                                              &newLengthClock,
                                              &minTimeClock);

            scheduler->d_eventTimeQueue.popLE(now,
                                              MAX_PENDING_EVENTS,
                                              &scheduler->d_pendingEventItems,
                                              &newLengthEvent,
                                              &minTimeEvent);

            clockLen = pendingClockItems.size();
            if (0 == clockLen && 0 == scheduler->d_pendingEventItems.size()) {
                // There are no pending items.  Wait appropriately.

                if (0 == newLengthClock && 0 == newLengthEvent) {
                    // Wait until interrupted.

                    scheduler->d_condition.wait(&scheduler->d_mutex);
                }
                else {
                    bsls::TimeInterval minTime =
                                        0 == newLengthClock ? minTimeEvent :
                                        0 == newLengthEvent ? minTimeClock :
                                        bsl::min(minTimeClock, minTimeEvent);

                    // Something is meant to happen in 'minTime'.  Wait
                    // (interruptibly) until then.

                    scheduler->d_condition.timedWait(&scheduler->d_mutex,
                                                     minTime);
                }
                continue;
            }

            BSLS_ASSERT(!pendingClockItems.empty()
                        || !scheduler->d_pendingEventItems.empty());
        }

        // We just unlocked the mutex.

        bsl::size_t clockIdx = 0;
        int *eventIdxPtr = &scheduler->d_currentEventIndex;
        *eventIdxPtr = 0;

        PendingClockItem *clockData = 0;
        if (!pendingClockItems.empty()) {
            clockData = &pendingClockItems.front();
        }
        TimerEventScheduler::EventItem *eventData = 0;
        if (!scheduler->d_pendingEventItems.empty()) {
            eventData = &scheduler->d_pendingEventItems.front();
        }

        // Note it is possible for an event in the dispatcher thread (and only
        // for such an event) to delete a future event that is in
        // pendingEventItems.

        while (clockIdx < clockLen
            && *eventIdxPtr < (int) scheduler->d_pendingEventItems.size()) {
            // Both queues had pending events.  Do the events in time order
            // until at least one of the queues is empty.

            const bsls::TimeInterval& clockTime = clockData[clockIdx].time();
            if (clockTime < eventData[*eventIdxPtr].time()) {
                ClockDataPtr cd(clockData[clockIdx].data());
                if (!cd->d_isCancelled) {
                    scheduler->d_dispatcherFunctor(cd->d_callback);
                    if (!cd->d_isCancelled) {
                        cd->d_handle = scheduler->d_clockTimeQueue.add(
                                       clockTime + cd->d_periodicInterval, cd);
                    }
                }

                ++clockIdx;
            }
            else {
                --scheduler->d_numEvents;
                scheduler->d_dispatcherFunctor(eventData[*eventIdxPtr].data());
                ++ *eventIdxPtr;
            }
        }
        // At most one of the pending queues still has events - can deal with
        // the two queues in arbitrary order now.

        for (; clockIdx < clockLen; ++clockIdx) {
            const bsls::TimeInterval& clockTime = clockData[clockIdx].time();
            ClockDataPtr cd(clockData[clockIdx].data());
            if (!cd->d_isCancelled) {
                scheduler->d_dispatcherFunctor(cd->d_callback);
                if (!cd->d_isCancelled) {
                    cd->d_handle = scheduler->d_clockTimeQueue.add(
                                            clockTime + cd->d_periodicInterval,
                                            cd);
                }
            }
        }

        for (; *eventIdxPtr < (int) scheduler->d_pendingEventItems.size();
                                                            ++ *eventIdxPtr) {
            --scheduler->d_numEvents;
            scheduler->d_dispatcherFunctor(eventData[*eventIdxPtr].data());
        }

        pendingClockItems.clear();
        scheduler->d_pendingEventItems.clear();
    }
}

               // ============================================
               // class TimerEventSchedulerTestTimeSource_Data
               // ============================================

class TimerEventSchedulerTestTimeSource_Data {
    // This 'class' provides storage for the current time and a mutex to
    // protect access to the current time.

    // DATA
    bsls::TimeInterval   d_currentTime;       // the current time

    mutable bslmt::Mutex d_currentTimeMutex;  // mutex used to synchronize
                                              // 'd_currentTime' access

  private:
    // NOT IMPLEMENTED
    TimerEventSchedulerTestTimeSource_Data(
                                const TimerEventSchedulerTestTimeSource_Data&);
    TimerEventSchedulerTestTimeSource_Data& operator=(
                                const TimerEventSchedulerTestTimeSource_Data&);

  public:
    // CREATORS
    explicit
    TimerEventSchedulerTestTimeSource_Data(bsls::TimeInterval currentTime);
        // Construct a test time-source data object that will store the
        // "system-time", initialized to the specified 'currentTime'.

    //! ~TimerEventSchedulerTestTimeSource_Data() = default;
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
TimerEventSchedulerTestTimeSource_Data::TimerEventSchedulerTestTimeSource_Data(
                                                bsls::TimeInterval currentTime)
: d_currentTime(currentTime)
{
}

// MANIPULATORS
bsls::TimeInterval TimerEventSchedulerTestTimeSource_Data::advanceTime(
                                                     bsls::TimeInterval amount)
{
    BSLS_ASSERT(amount > 0);

    bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);
    d_currentTime += amount;
    return d_currentTime;
}

// ACCESSORS
bsls::TimeInterval TimerEventSchedulerTestTimeSource_Data::currentTime() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_currentTimeMutex);
    return d_currentTime;
}

}  // close package namespace

static
void defaultDispatcherFunction(const bsl::function<void()>& callback)
{
    callback();
}

namespace bdlmt {
                         // -------------------------
                         // class TimerEventScheduler
                         // -------------------------

// PRIVATE CLASS DATA
const char TimerEventScheduler::s_defaultThreadName[16] = { "bdl.TimerEvent" };

// PRIVATE MANIPULATORS
void TimerEventScheduler::yieldToDispatcher()
{
    if (d_running.loadRelaxed()) {
        bsls::Types::Uint64 dispatcherId = static_cast<bsls::Types::Uint64>(
                                                 d_dispatcherId.loadRelaxed());

        if (bslmt::ThreadUtil::selfIdAsUint64() != dispatcherId) {
            const int it = d_iterations.loadRelaxed();
            while (it == d_iterations.loadRelaxed() &&
                   d_running.loadRelaxed()) {
                d_condition.signal();
                bslmt::ThreadUtil::yield();
            }
        }
    }
}

// CREATORS
TimerEventScheduler::TimerEventScheduler(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData), basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

TimerEventScheduler::TimerEventScheduler(
                                   bsls::SystemClockType::Enum  clockType,
                                   bslma::Allocator            *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_condition(clockType)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(clockType)
{
}

TimerEventScheduler::TimerEventScheduler(
                     const TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                     bslma::Allocator                       *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData), basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

TimerEventScheduler::TimerEventScheduler(
                     const TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                     bsls::SystemClockType::Enum             clockType,
                     bslma::Allocator                       *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData), basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_condition(clockType)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(clockType)
{
}

TimerEventScheduler::TimerEventScheduler(int               numEvents,
                                         int               numClocks,
                                         bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData), basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

TimerEventScheduler::TimerEventScheduler(
                                   int                          numEvents,
                                   int                          numClocks,
                                   bsls::SystemClockType::Enum  clockType,
                                   bslma::Allocator            *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_condition(clockType)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      &defaultDispatcherFunction)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(clockType)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

TimerEventScheduler::TimerEventScheduler(
                     int                                     numEvents,
                     int                                     numClocks,
                     const TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                     bslma::Allocator                       *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(
                                            bsls::SystemClockType::e_REALTIME))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

TimerEventScheduler::TimerEventScheduler(
                     int                                     numEvents,
                     int                                     numClocks,
                     const TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                     bsls::SystemClockType::Enum             clockType,
                     bslma::Allocator                       *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_currentTimeFunctor(bsl::allocator_arg_t(), basicAllocator,
                       createDefaultCurrentTimeFunctor(clockType))
, d_clockDataAllocator(sizeof(TimerEventScheduler::ClockData), basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_condition(clockType)
, d_dispatcherFunctor(bsl::allocator_arg_t(), basicAllocator,
                      dispatcherFunctor)
, d_dispatcherId(0)
, d_dispatcherThread(bslmt::ThreadUtil::invalidHandle())
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
, d_clockType(clockType)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

TimerEventScheduler::~TimerEventScheduler()
{
    stop();
}

// MANIPULATORS
int TimerEventScheduler::start()
{
    bslmt::ThreadAttributes attr;

    return start(attr);
}

int TimerEventScheduler::start(const bslmt::ThreadAttributes& threadAttributes)
{
    // Implementation note: 'd_dispatcherMutex' is in a lock hierarchy with
    // 'd_mutex' and must always be locked first.

    bslmt::LockGuard<bslmt::Mutex> dispatcherLock(&d_dispatcherMutex);

    BSLS_ASSERT(! bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                             d_dispatcherThread));

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_running.loadRelaxed()) {
        return 0;                                                     // RETURN
    }

    bslmt::ThreadAttributes modAttr(threadAttributes);
    modAttr.setDetachedState(bslmt::ThreadAttributes::e_CREATE_JOINABLE);
    if (modAttr.threadName().empty()) {
        modAttr.setThreadName(s_defaultThreadName);
    }

    if (bslmt::ThreadUtil::createWithAllocator(
                                          &d_dispatcherThread,
                                          modAttr,
                                          &TimerEventSchedulerDispatcherThread,
                                          this,
                                          d_allocator_p))
    {
        return -1;                                                    // RETURN
    }
    d_dispatcherId = bslmt::ThreadUtil::idAsUint64(
                            bslmt::ThreadUtil::handleToId(d_dispatcherThread));
    d_running = 1;

    return 0;
}

void TimerEventScheduler::stop()
{
    // Implementation note: 'd_dispatcherMutex' is in a lock hierarchy with
    // 'd_mutex' and must always be locked first.

    bslmt::LockGuard<bslmt::Mutex> dispatcherLock(&d_dispatcherMutex);

    BSLS_ASSERT(! bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                             d_dispatcherThread));

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        if (!d_running.loadRelaxed()) {
            return;                                                   // RETURN
        }

        d_running = 0;
        d_condition.signal();
    }

    bslmt::ThreadUtil::join(d_dispatcherThread);
}

TimerEventScheduler::Handle
TimerEventScheduler::scheduleEvent(const bsls::TimeInterval&    time,
                                   const bsl::function<void()>& callback,
                                   const EventKey&              key)
{
    Handle handle;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        int isNewTop = 0;
        handle = d_eventTimeQueue.add(time, callback, key, &isNewTop);

        if (-1 == handle) {
            return e_INVALID_HANDLE;                                  // RETURN
        }

        ++d_numEvents;

        if (isNewTop) {
            d_condition.signal();
        }
    }

    return handle;
}

int TimerEventScheduler::rescheduleEvent(TimerEventScheduler::Handle handle,
                                         const EventKey&             key,
                                         const bsls::TimeInterval&   newTime,
                                         bool                        wait)
{
    int status;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        int isNewTop = 0;
        status = d_eventTimeQueue.update(handle, key, newTime, &isNewTop);
        if (isNewTop) {
            d_condition.signal();
        }
    }

    // wait for a cycle if required

    if (status && wait) {
        yieldToDispatcher();
    }

    return status;
}

int TimerEventScheduler::cancelEvent(TimerEventScheduler::Handle handle,
                                     const EventKey&             key,
                                     bool                        wait)
{
    // First search in the event queue if we can find the event there.

    if (!d_eventTimeQueue.remove(handle, key)) {
        --d_numEvents;

        // it was in the event queue, therefore it is not in the
        // pendingEventItems.

        return 0;                                                     // RETURN
    }

    // Following code optimizes for the case when this method is being called
    // from the dispatcher thread.  In that case, the following code fragments
    // tries to cancel the event even if it has been put onto the pending list.
    // Note that there is no need for a lock here.

    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                  d_dispatcherThread))
    {
        // If there are pending items, search among them, starting with the
        // event after the current one.

        if (d_currentEventIndex < (int) d_pendingEventItems.size()) {
            bsl::vector<TimerEventScheduler::EventItem>::iterator
                begin = d_pendingEventItems.begin() + d_currentEventIndex,
                end   = d_pendingEventItems.end(),
                it    = begin;

            while (++it != end) {
                if (it->handle() == handle && it->key() == key) {
                    --d_numEvents;
                    d_pendingEventItems.erase(it);
                    return 0;                                         // RETURN
                }
            }
        }
        // Else it is not found in the pending items, nor in the event queue.

        return -1;                                                    // RETURN
    }

    // The rest of this code is guaranteed not to execute in the dispatcher
    // thread.

    if (handle != e_INVALID_HANDLE && wait) {
        yieldToDispatcher();
    }

    return -1;
}

void TimerEventScheduler::cancelAllEvents(bool wait)
{
    bsl::vector<EventItem> buffer;

    d_eventTimeQueue.removeAll(&buffer);
    d_numEvents -= static_cast<int>(buffer.size());

    // wait for a cycle if needed

    if (wait) {
        yieldToDispatcher();
    }
}

TimerEventScheduler::Handle
TimerEventScheduler::startClock(const bsls::TimeInterval&    interval,
                                const bsl::function<void()>& callback,
                                const bsls::TimeInterval&    startTime)
{
    BSLS_ASSERT(0 != interval);

    bsls::TimeInterval stime(startTime);
    if (0 == stime) {
        stime = d_currentTimeFunctor() + interval;
    }

    ClockData *pClockData =
                new (d_clockDataAllocator.allocate()) ClockData(callback,
                                                                interval,
                                                                d_allocator_p);
    ClockDataPtr p(pClockData, &d_clockDataAllocator, d_allocator_p);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        int isNewTop = 0;
        p->d_handle = d_clockTimeQueue.add(stime, p, &isNewTop);

        if (-1 == p->d_handle) {
            return e_INVALID_HANDLE;                                  // RETURN
        }

        ++d_numClocks;

        if (isNewTop) {
            d_condition.signal();
        }
    }

    return d_clocks.add(p);
}

int TimerEventScheduler::cancelClock(Handle handle, bool wait)
{
    ClockDataPtr p;
    if (d_clocks.remove(handle, &p)) {
        return -1;                                                    // RETURN
    }

    --d_numClocks;

    if (d_clockTimeQueue.remove(p->d_handle)) {
        p->d_isCancelled = true;

        if (wait) {
            yieldToDispatcher();
        }
    }

    return 0;
}

void TimerEventScheduler::cancelAllClocks(bool wait)
{
    bsl::vector<ClockDataPtr> buffer;
    d_clocks.removeAll(&buffer);

    d_numClocks -= static_cast<int>(buffer.size());

    const int length = static_cast<int>(buffer.size());

    // mark them all canceled ASAP

    for (int i = 0; i < length; ++i) {
        buffer[i]->d_isCancelled = true;
    }

    bool removeFailed = false;
    for (int i = 0; i < length; ++i) {
        if (d_clockTimeQueue.remove(buffer[i]->d_handle)) {
            removeFailed = true;
        }
    }

    // if 'removeFailed', then we know there was/were clock(s) pending.

    if (wait && removeFailed) {
        yieldToDispatcher();
    }
}

                  // ---------------------------------------
                  // class TimerEventSchedulerTestTimeSource
                  // ---------------------------------------

// CREATORS
TimerEventSchedulerTestTimeSource::TimerEventSchedulerTestTimeSource(
                                                TimerEventScheduler *scheduler)
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
    // 'TimerEventScheduler::dispatchEvents' could go into a tight loop waiting
    // for the next event instead of sleeping until the next call to
    // 'TimerEventSchedulerTestTimeSource::advanceTime'.  See the call to
    // 'timedWait' in 'TimerEventScheduler::dispatchEvents'.

    // The following uses the default allocator since the created object's
    // lifetime is shared between 'TimerEventSchedulerTestTimeSource' and the
    // associated 'TimerEventScheduler', so the data may outlive either
    // individual object.

    d_data_p = bsl::make_shared<TimerEventSchedulerTestTimeSource_Data>(
                                bsls::SystemTime::now(scheduler->d_clockType)
                              + 1000 * bdlt::TimeUnitRatio::k_SECONDS_PER_DAY);

    // Bind the member function 'now' to 'this', and let the scheduler call
    // this binder as its current time callback.

    d_scheduler_p->d_currentTimeFunctor = bdlf::BindUtil::bind(
                          &TimerEventSchedulerTestTimeSource_Data::currentTime,
                          d_data_p);
}

// MANIPULATORS
bsls::TimeInterval TimerEventSchedulerTestTimeSource::advanceTime(
                                                     bsls::TimeInterval amount)
{
    BSLS_ASSERT(amount > 0);

    // Returning the new time allows an atomic 'advance' + 'now' operation.
    // This feature may not be necessary.

    bsls::TimeInterval ret = d_data_p->advanceTime(amount);

    {
        // This scope limits how long we lock the scheduler's mutex

        bslmt::LockGuard<bslmt::Mutex> lock(&d_scheduler_p->d_mutex);

        // Now that the time has changed, signal the scheduler's condition
        // variable so that the event dispatcher thread can be alerted to the
        // change.

        d_scheduler_p->d_condition.signal();
    }

    return ret;
}

// ACCESSORS
bsls::TimeInterval TimerEventSchedulerTestTimeSource::now() const
{
    return d_data_p->currentTime();
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
