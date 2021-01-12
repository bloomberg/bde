// ball_asyncfileobserver.cpp                                         -*-C++-*-
#include <ball_asyncfileobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_asyncfileobserver_cpp,"$Id$ $CSID$")

#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_streamobserver.h>              // for testing only

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdls_processutil.h>
#include <bdlt_currenttime.h>

#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutexassert.h>
#include <bslmt_threadattributes.h>

#include <bsls_assert.h>

#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>

///IMPLEMENTATION NOTES
///--------------------
// To guarantee that the publication thread sees when 'd_threadState' is set to
// 'e_SHUTTING_DOWN', a special record (having 'ball::Transmission::e_END') is
// appended to the queue.  Without the special record the publication thread
// may be blocked indefinitely on 'popFront'.  Unfortunately, that potentially
// leaves a bogus record in the queue after the publication thread is shut
// down.  To avoid having to deal with that 'e_END' record when the thread is
// restarted, 'shutdownThread' clears the queue in order to simplify the
// implementation.  Alternative designs are possible, but are not perceived to
// be worth the added complexity.

namespace BloombergLP {
namespace ball {

namespace {

enum {
    k_DEFAULT_FIXED_QUEUE_SIZE = 8192,
    k_FORCE_WARN_THRESHOLD     = 5000
};

static const char *const k_LOG_CATEGORY = "BALL.ASYNCFILEOBSERVER";

static void populateWarnRecord(ball::Record *record,
                               int           lineNumber,
                               int           numDropped)
    // Load the specified 'record' with a warning message indicating the
    // specified 'numDropped' records have been dropped as recorded at the
    // specified 'lineNumber'.
{
    record->fixedFields().messageStreamBuf().pubseekpos(0);
    record->fixedFields().setLineNumber(lineNumber);
    record->fixedFields().setTimestamp(bdlt::CurrentTime::utc());
    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "Dropped " << numDropped << " log records." << bsl::ends;
}

}  // close unnamed namespace

                       // -----------------------
                       // class AsyncFileObserver
                       // -----------------------

// PRIVATE MANIPULATORS
void AsyncFileObserver::logDroppedMessageWarning(int numDropped)
{
    // Log the record, unconditionally, to the file observer (i.e., without
    // consulting the logger manager as to whether WARN is enabled) to avoid an
    // observer->loggermanager dependency.

    populateWarnRecord(&d_droppedRecordWarning, __LINE__, numDropped);
    Context context(Transmission::e_PASSTHROUGH, 0, 0);
    d_fileObserver.publish(d_droppedRecordWarning, context);
}

void AsyncFileObserver::publishThreadEntryPoint()
{
    // The publication thread may either be 'e_RUNNING' (normal), or
    // 'e_SHUTTING_DOWN' (if a client called a method that stopped the
    // publication thread immediately after starting it, before this thread
    // entry point function is called).  The publication cannot be
    // 'e_NOT_RUNNING', since that state is only returned to after completing
    // this function.

    BSLS_ASSERT(e_NOT_RUNNING != d_threadState);

    bool done = false;
    d_droppedRecordWarning.fixedFields().setThreadID(
                                          bslmt::ThreadUtil::selfIdAsUint64());

    while (!done) {
        AsyncFileObserver_Record asyncRecord = d_recordQueue.popFront();

        // Publish the next log record on the queue only if the observer is not
        // stopping the publication thread.

        if (Transmission::e_END == asyncRecord.d_context.transmissionCause()
            || e_SHUTTING_DOWN  == d_threadState) {
            done = true;
        }
        else {
            d_fileObserver.publish(*asyncRecord.d_record,
                                   asyncRecord.d_context);
        }

        // Publish the count of dropped records.  To avoid repeatedly
        // publishing this information when the record queue is full, we
        // publish the number of dropped records only when the queue becomes
        // half empty or when a sufficient number of records have been dropped.
        // Finally, we publish the dropped record count if the observer is
        // shutting down, so the information is not lost.

        if (0 < d_dropCount.loadRelaxed()) {
            if (d_recordQueue.length() <= d_recordQueue.size() / 2
            ||  d_dropCount.loadRelaxed() >= k_FORCE_WARN_THRESHOLD
            ||  e_SHUTTING_DOWN == d_threadState) {
                int numDropped = d_dropCount.swap(0);
                BSLS_ASSERT(0 < numDropped); // No other thread should have
                                             // cleared the count.
                logDroppedMessageWarning(numDropped);
            }
        }
    }
    d_threadState = e_NOT_RUNNING;
}

int AsyncFileObserver::startThread()
{
    BSLMT_MUTEXASSERT_IS_LOCKED(&d_mutex);

    // d_threadState and d_threadHandle should be consistent.

    BSLS_ASSERT((e_NOT_RUNNING == d_threadState) ==
                (bslmt::ThreadUtil::invalidHandle() == d_threadHandle));

    if (bslmt::ThreadUtil::invalidHandle() == d_threadHandle) {
        BSLS_ASSERT(e_NOT_RUNNING == d_threadState);

        d_threadState = e_RUNNING;
        bslmt::ThreadAttributes attr;
        return bslmt::ThreadUtil::create(&d_threadHandle,
                                         attr,
                                         d_publishThreadEntryPoint);  // RETURN
    }
    return 0;
}

int AsyncFileObserver::stopThread()
{
    BSLMT_MUTEXASSERT_IS_LOCKED(&d_mutex);

    if (bslmt::ThreadUtil::invalidHandle() != d_threadHandle) {
        BSLS_ASSERT(e_RUNNING       == d_threadState ||
                    e_SHUTTING_DOWN == d_threadState);

        // Push an empty record with 'e_END' set in context.

        AsyncFileObserver_Record asyncRecord;
        bsl::shared_ptr<const Record> record(
                                    new (*d_allocator_p) Record(d_allocator_p),
                                    d_allocator_p);

        Context context(Transmission::e_END, 0, 1);
        asyncRecord.d_record  = record;
        asyncRecord.d_context = context;
        {
            // We use 'tryPushBack' because we do not want 'pushBack' to block
            // if queue is full, particulary if the publication thread has
            // already observed 'e_SHUTTING_DOWN' and has stopped removing
            // records from the queue (DRQS 164688087), as may happen on a call
            // to 'shutdownThread'.

            int rc = -1;
            while (0 != rc && d_threadState != e_NOT_RUNNING) {
                rc = d_recordQueue.tryPushBack(asyncRecord);
		bslmt::ThreadUtil::yield();
            }
        }
        int ret = bslmt::ThreadUtil::join(d_threadHandle);
        d_threadHandle = bslmt::ThreadUtil::invalidHandle();
        BSLS_ASSERT(e_NOT_RUNNING == d_threadState);
        return ret;                                                   // RETURN
    }
    BSLS_ASSERT(e_NOT_RUNNING == d_threadState);
    return 0;
}

int AsyncFileObserver::shutdownThread()
{
    BSLMT_MUTEXASSERT_IS_LOCKED(&d_mutex);

    // The publication thread may be running, or stopped, but cannot already
    // be within a call to 'shutdownThread'.

    BSLS_ASSERT(e_RUNNING == d_threadState || e_NOT_RUNNING == d_threadState);

    int result = 0;

    if (bslmt::ThreadUtil::invalidHandle() != d_threadHandle) {
        BSLS_ASSERT(e_RUNNING == d_threadState);

        d_threadState = e_SHUTTING_DOWN;

        result = stopThread();
    }

    // We clear the queue to remove the bogus log record appended by
    // 'stopThread'.

    d_recordQueue.removeAll();

    // The lock on 'd_mutex' should prevent any other thread from modifying
    // 'd_threadState' after the thread is stopped.

    BSLS_ASSERT(e_NOT_RUNNING == d_threadState);

    return result;
}

void AsyncFileObserver::construct()
{
    d_threadHandle = bslmt::ThreadUtil::invalidHandle();
    d_threadState  = e_NOT_RUNNING;
    d_dropCount    = 0;

    d_publishThreadEntryPoint = bsl::function<void()>(
            bsl::allocator_arg_t(),
            bsl::allocator<bsl::function<void()> >(d_allocator_p),
            bdlf::MemFnUtil::memFn(&AsyncFileObserver::publishThreadEntryPoint,
                                   this));
    d_droppedRecordWarning.fixedFields().setFileName(__FILE__);
    d_droppedRecordWarning.fixedFields().setCategory(k_LOG_CATEGORY);
    d_droppedRecordWarning.fixedFields().setSeverity(Severity::e_WARN);
    d_droppedRecordWarning.fixedFields().setProcessID(
                                            bdls::ProcessUtil::getProcessId());
}

// CREATORS
AsyncFileObserver::AsyncFileObserver(bslma::Allocator *basicAllocator)
: d_fileObserver(Severity::e_WARN, basicAllocator)
, d_recordQueue(k_DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(Severity::e_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(Severity::Level   stdoutThreshold,
                                     bslma::Allocator *basicAllocator)
: d_fileObserver(stdoutThreshold, basicAllocator)
, d_recordQueue(k_DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(Severity::e_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(Severity::Level   stdoutThreshold,
                                     bool              publishInLocalTime,
                                     bslma::Allocator *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(k_DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(Severity::e_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(Severity::Level   stdoutThreshold,
                                     bool              publishInLocalTime,
                                     int               maxRecordQueueSize,
                                     bslma::Allocator *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(Severity::e_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(
                             Severity::Level   stdoutThreshold,
                             bool              publishInLocalTime,
                             int               maxRecordQueueSize,
                             Severity::Level   dropRecordsOnFullQueueThreshold,
                             bslma::Allocator *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(dropRecordsOnFullQueueThreshold)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::~AsyncFileObserver()
{
    stopPublicationThread();
}

// MANIPULATORS
void AsyncFileObserver::publish(const bsl::shared_ptr<const Record>& record,
                                const Context&                       context)
{
    BSLS_ASSERT(record);

    AsyncFileObserver_Record asyncRecord;

    asyncRecord.d_record  = record;
    asyncRecord.d_context = context;

    if (record->fixedFields().severity() > d_dropRecordsOnFullQueueThreshold) {
        if (0 != d_recordQueue.tryPushBack(asyncRecord)) {
            d_dropCount.addRelaxed(1);
        }
    }
    else {
        d_recordQueue.pushBack(asyncRecord);
    }
}

void AsyncFileObserver::releaseRecords()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    if (isPublicationThreadRunning()) {
        shutdownThread();
        startThread();
    }
    else {
        d_recordQueue.removeAll();
    }
}

int AsyncFileObserver::shutdownPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return shutdownThread();
}

int AsyncFileObserver::startPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return startThread();
}

int AsyncFileObserver::stopPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return stopThread();
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
