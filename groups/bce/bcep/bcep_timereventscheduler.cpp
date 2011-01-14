// bcep_timereventscheduler.cpp    -*-C++-*-
#include <bcep_timereventscheduler.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_timereventscheduler_cpp,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bdetu_systemtime.h>
#include <bsls_assert.h>
#include <bdes_bitutil.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {

const int NUM_INDEX_BITS_DEFAULT = 17;
    // Default number of bits used to represent each 'bcec_TimeQueue'
    // handle.

const int NUM_INDEX_BITS_MIN = 8;
    // Minimum number of bits required to represent a 'bcec_TimeQueue'
    // handle.

int numBitsRequired(int value)
{
    // Calculate the smallest number of bits required to represent the
    // specified 'value'.

    return bdes_BitUtil::find1AtLargestIndex(value) + 1;
}

}  // close unnamed namespace

struct bcep_TimerEventSchedulerDispatcher {
    // This class just contains the method called to run the dispatcher
    // thread.  Once started, it infinite loops, either waiting for or
    // executing events.

    // CLASS METHODS
    static void dispatchEvents(bcep_TimerEventScheduler *scheduler);
};

extern "C" void *bcep_TimerEventSchedulerDispatcherThread(void *scheduler)
{
    bcep_TimerEventSchedulerDispatcher::dispatchEvents(
                                        (bcep_TimerEventScheduler*)scheduler);
    return scheduler;
}

void bcep_TimerEventSchedulerDispatcher::dispatchEvents(
                                           bcep_TimerEventScheduler *scheduler)
{
    BSLS_ASSERT(0 != scheduler);
    typedef bcep_TimerEventScheduler::ClockDataPtr ClockDataPtr;
    typedef bcec_TimeQueueItem<ClockDataPtr>       PendingClockItem;

    bsl::vector<PendingClockItem> pendingClockItems;

    while (1) {
        int clockLen;

        // This scope limits the life of several variables, including
        // mutex lock.

        {
            bcemt_LockGuard<bcemt_Mutex> lock(&scheduler->d_mutex);
            if (!scheduler->d_running) {
                return;
            }
            ++scheduler->d_iterations;

            int newLengthClock = 0, newLengthEvent = 0;
            bdet_TimeInterval now = bdetu_SystemTime::now();

            // minTimeClock will be set only if newLengthClock > 0, similar
            // with minTimeEvent

            enum {
                MAX_PENDING_CLOCKS = 64,
                MAX_PENDING_EVENTS = 64
            };
            bdet_TimeInterval minTimeClock, minTimeEvent;

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
                    bdet_TimeInterval minTime =
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

        int clockIdx = 0;
        int *eventIdxPtr = &scheduler->d_currentEventIndex;
        *eventIdxPtr = 0;

        PendingClockItem *clockData = 0;
        if (!pendingClockItems.empty()) {
            clockData = &pendingClockItems.front();
        }
        bcep_TimerEventScheduler::EventItem *eventData = 0;
        if (!scheduler->d_pendingEventItems.empty()) {
            eventData = &scheduler->d_pendingEventItems.front();
        }

        // Note it is possible for an event in the dispatcher thread
        // (and only for such an event) to delete a future event that is in
        // pendingEventItems.

        while (clockIdx < clockLen
            && *eventIdxPtr < (int) scheduler->d_pendingEventItems.size()) {
            // Both queues had pending events.  Do the events in time
            // order until at least one of the queues is empty.

            const bdet_TimeInterval& clockTime = clockData[clockIdx].time();
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
        // At most one of the pending queues still has events - can deal
        // with the two queues in arbitrary order now.

        for (; clockIdx < clockLen; ++clockIdx) {
            const bdet_TimeInterval& clockTime = clockData[clockIdx].time();
            ClockDataPtr cd(clockData[clockIdx].data());
            if (!cd->d_isCancelled) {
                scheduler->d_dispatcherFunctor(cd->d_callback);
                if (!cd->d_isCancelled) {
                    cd->d_handle = scheduler->d_clockTimeQueue.add(
                                    clockTime + cd->d_periodicInterval, cd);
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

static
void defaultDispatcherFunction(const bdef_Function<void(*)()>& callback) {
    callback();
}

                           // ------------------------------
                           // class bcep_TimerEventScheduler
                           // ------------------------------

// PRIVATE MANIPULATORS
void bcep_TimerEventScheduler::yieldToDispatcher()
{
    if (d_running && !bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                                d_dispatcherThread)) {
        const int it = d_iterations;
        while (it == d_iterations && d_running) {
            d_condition.signal();
            bcemt_ThreadUtil::yield();
        }
    }
}

// CREATORS
bcep_TimerEventScheduler::bcep_TimerEventScheduler(
                                               bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_clockDataAllocator(sizeof(bcep_TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(&defaultDispatcherFunction)
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
{
}

bcep_TimerEventScheduler::bcep_TimerEventScheduler(
                const bcep_TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                bslma_Allocator                             *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_clockDataAllocator(sizeof(bcep_TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clockTimeQueue(NUM_INDEX_BITS_DEFAULT, basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(dispatcherFunctor)
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
{
}

bcep_TimerEventScheduler::bcep_TimerEventScheduler(
                                               int              numEvents,
                                               int              numClocks,
                                               bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_clockDataAllocator(sizeof(bcep_TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(&defaultDispatcherFunction)
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

bcep_TimerEventScheduler::bcep_TimerEventScheduler(
                int                                          numEvents,
                int                                          numClocks,
                const bcep_TimerEventScheduler::Dispatcher&  dispatcherFunctor,
                bslma_Allocator                             *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_clockDataAllocator(sizeof(bcep_TimerEventScheduler::ClockData),
                       basicAllocator)
, d_eventTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numEvents)),
                   basicAllocator)
, d_clockTimeQueue(bsl::max(NUM_INDEX_BITS_MIN, numBitsRequired(numClocks)),
                   basicAllocator)
, d_clocks(basicAllocator)
, d_dispatcherFunctor(dispatcherFunctor)
, d_running(0)
, d_iterations(0)
, d_pendingEventItems(basicAllocator)
, d_currentEventIndex(-1)
, d_numEvents(0)
, d_numClocks(0)
{
    BSLS_ASSERT(numEvents < (1 << 24) - 1);
    BSLS_ASSERT(numClocks < (1 << 24) - 1);
}

bcep_TimerEventScheduler::~bcep_TimerEventScheduler()
{
    stop();
}

// MANIPULATORS
int bcep_TimerEventScheduler::start()
{
    bcemt_Attribute attr;

    return start(attr);
}

int bcep_TimerEventScheduler::start(const bcemt_Attribute& threadAttributes)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    if (d_running) {
        return 0;
    }

    bcemt_Attribute modAttr(threadAttributes);
    modAttr.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);

    if (bcemt_ThreadUtil::create(&d_dispatcherThread, modAttr,
                                 &bcep_TimerEventSchedulerDispatcherThread,
                                 this))
    {
        return -1;
    }
    d_running = 1;

    return 0;
}

void bcep_TimerEventScheduler::stop()
{
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        if (!d_running) {
            return;
        }

        d_running = 0;
        d_condition.signal();
    }

    bcemt_ThreadUtil::join(d_dispatcherThread);
}

bcep_TimerEventScheduler::Handle
bcep_TimerEventScheduler::scheduleEvent(
                                      const bdet_TimeInterval&        timer,
                                      const bdef_Function<void(*)()>& callback,
                                      const EventKey&                 key)
{
    Handle handle;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        int isNewTop = 0;
        handle = d_eventTimeQueue.add(timer, callback, key, &isNewTop);

        if (-1 == handle) {
            return BCEP_INVALID_HANDLE;
        }

        ++d_numEvents;

        if (isNewTop) {
            d_condition.signal();
        }
    }

    return handle;
}

int bcep_TimerEventScheduler::rescheduleEvent(
                                     bcep_TimerEventScheduler::Handle handle,
                                     const EventKey&                  key,
                                     const bdet_TimeInterval&         newTime,
                                     bool                             wait)
{
    int status;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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

int bcep_TimerEventScheduler::cancelEvent(
                                       bcep_TimerEventScheduler::Handle handle,
                                       const EventKey&                  key,
                                       bool                             wait)
{
    // First search in the event queue if we can find the event there.

    if (!d_eventTimeQueue.remove(handle, key)) {
        --d_numEvents;

        // it was in the event queue, therefore it is not in the
        // pendingEventItems.

        return 0;
    }

    // Following code optimizes for the case when this method is being called
    // from the dispatcher thread.  In that case, the following code fragments
    // tries to cancel the event even if it has been put onto the pending list.
    // Note that there is no need for a lock here.

    if (bcemt_ThreadUtil::isEqual(bcemt_ThreadUtil::self(),
                                  d_dispatcherThread))
    {
        // If there are pending items, search among them, starting with
        // the event after the current one.

        if (d_currentEventIndex < (int) d_pendingEventItems.size()) {
            bsl::vector<bcep_TimerEventScheduler::EventItem>::iterator
                begin = d_pendingEventItems.begin() + d_currentEventIndex,
                end   = d_pendingEventItems.end(),
                it    = begin;

            while (++it != end) {
                if (it->handle() == handle && it->key() == key) {
                    --d_numEvents;
                    d_pendingEventItems.erase(it);
                    return 0;
                }
            }
        }
        // Else it is not found in the pending items, nor in the event queue.

        return -1;
    }

    // The rest of this code is guaranteed not to execute in the dispatcher
    // thread.

    if (wait) {
        yieldToDispatcher();
    }

    return -1;
}

void bcep_TimerEventScheduler::cancelAllEvents(bool wait)
{
    bsl::vector<EventItem> buffer;

    d_eventTimeQueue.removeAll(&buffer);
    d_numEvents -= buffer.size();

    // wait for a cycle if needed

    if (wait) {
        yieldToDispatcher();
    }
}

bcep_TimerEventScheduler::Handle
bcep_TimerEventScheduler::startClock(const bdet_TimeInterval&        interval,
                                     const bdef_Function<void(*)()>& callback,
                                     const bdet_TimeInterval&        startTime)
{
    BSLS_ASSERT(0 != interval);

    bdet_TimeInterval stime(startTime);
    if (0 == stime) {
        stime = bdetu_SystemTime::now() + interval;
    }

    ClockData *pClockData =
                new (d_clockDataAllocator.allocate()) ClockData(callback,
                                                                interval,
                                                                d_allocator_p);
    ClockDataPtr p(pClockData, &d_clockDataAllocator, d_allocator_p);

    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
        int isNewTop = 0;
        p->d_handle = d_clockTimeQueue.add(stime, p, &isNewTop);

        if (-1 == p->d_handle) {
            return BCEP_INVALID_HANDLE;
        }

        ++d_numClocks;

        if (isNewTop) {
            d_condition.signal();
        }
    }

    return d_clocks.add(p);
}

int bcep_TimerEventScheduler::cancelClock(Handle handle, bool wait)
{
    ClockDataPtr p;
    if (d_clocks.remove(handle, &p)) {
        return -1;
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

void bcep_TimerEventScheduler::cancelAllClocks(bool wait)
{
    bsl::vector<ClockDataPtr> buffer;
    d_clocks.removeAll(&buffer);

    d_numClocks -= buffer.size();

    const int length = buffer.size();

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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
