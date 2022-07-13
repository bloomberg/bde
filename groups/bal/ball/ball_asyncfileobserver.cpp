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
// To communicate the last record to be published when 'stopThread' (or
// 'stopPublicationThread') is called, a special record is enqueued (created using
// 'createStopRecord') for which 'isStopRecord' is 'false'.

namespace BloombergLP {
namespace ball {

namespace {

enum {
    k_DEFAULT_FIXED_QUEUE_SIZE = 8192,
    k_FORCE_WARN_THRESHOLD     = 5000
};

static const char *const k_LOG_CATEGORY = "BALL.ASYNCFILEOBSERVER";

static const char *const k_THREAD_NAME = "asyncobserver";

bsl::shared_ptr<ball::Record> createEmptyRecord(
                                              int                   lineNumber,
                                              ball::Severity::Level level)
{
    bsl::shared_ptr<ball::Record> record = bsl::make_shared<ball::Record>();
    record->fixedFields().setFileName(__FILE__);
    record->fixedFields().setCategory(k_LOG_CATEGORY);
    record->fixedFields().setSeverity(level);
    record->fixedFields().setProcessID(bdls::ProcessUtil::getProcessId());
    record->fixedFields().setLineNumber(lineNumber);
    record->fixedFields().setTimestamp(bdlt::CurrentTime::utc());
    record->fixedFields().setThreadID(bslmt::ThreadUtil::selfIdAsUint64());

    return record;
}

void logDroppedMessageWarning(ball::FileObserver *fileObserver, int numDropped)
    // Publish a record to the specified 'fileObserver' at 'WARN' severity
    // level that the specified 'numDropped' messages have been dropped from
    // publication because they could not be enqueued on the async file
    // observer.
{
    // We log the record unconditionally (i.e., without consulting the logger
    // manager as to whether WARN is enabled) to avoid an
    // observer->loggermanager dependency.

    bsl::shared_ptr<ball::Record> record =
        createEmptyRecord(__LINE__, ball::Severity::e_WARN);

    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "Dropped " << numDropped << " log records." << bsl::ends;

    Context context(Transmission::e_PASSTHROUGH, 0, 0);
    fileObserver->publish(record, context);
}

void logQueueFailureError(ball::FileObserver *fileObserver)
    // Publish a record to the specified 'fileObserver' at 'ERROR' severity
    // level indicating a failure to dequeue methods off of the async file
    // observers record queue.
{
    // We log the record unconditionally (i.e., without consulting the logger
    // manager as to whether ERROR is enabled) to avoid an
    // observer->loggermanager dependency.

    bsl::shared_ptr<ball::Record> record =
        createEmptyRecord(__LINE__, ball::Severity::e_ERROR);

    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "Unable to dequeue messages for asynchronous publication. "
       << "Stopping asynchronous publication and dropping all messages.";

    Context context(Transmission::e_PASSTHROUGH, 0, 0);
    fileObserver->publish(record, context);
}

void logReleaseRecordsError(ball::FileObserver *fileObserver)
    // Publish a record to the specified 'fileObserver' at 'ERROR' severity
    // level indicating a failure in flushing the queue as part of release
    // records.
{
    // We log the record unconditionally (i.e., without consulting the logger
    // manager as to whether ERROR is enabled) to avoid an
    // observer->loggermanager dependency.

    bsl::shared_ptr<ball::Record> record =
        createEmptyRecord(__LINE__, ball::Severity::e_ERROR);

    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "'releaseRecords' unable to restart the publication thread.";

    Context context(Transmission::e_PASSTHROUGH, 0, 0);
    fileObserver->publish(record, context);
}

void setPublicationThreadAttributes(bslmt::ThreadAttributes *attributes)
    // Load the specified 'attributes' with the attributes for the publication
    // thread (for the moment, the thread name).
{
    attributes->setThreadName(k_THREAD_NAME);
}

        // stop-record functions

AsyncFileObserver_Record createStopRecord()
    // Return a 'AsyncFileObserver_Record' object for which 'isStopRecord' will
    // return 'true', and that cannot be a user created record from
    // 'ball::AsyncFileObserver::publish'.  Note that this record is used by
    // 'stopThread' to signal to the publication thread to stop.  Note also that
    // this is not simply a constant because such a constant would require a
    // constructor be called before 'main' in C++03.
{
    AsyncFileObserver_Record record;
    return record;
}

bool isStopRecord(const AsyncFileObserver_Record& record)
    // Return 'true' if the specified 'record' was created by
    // 'createStopRecord' and 'false' otherwise.  Note that this record is used
    // by 'stopThread' to signal to the publication thread to stop.
{
    return 0 == record.d_record.get();
}

}  // close unnamed namespace

                       // -----------------------
                       // class AsyncFileObserver
                       // -----------------------

void AsyncFileObserver::publishThreadEntryPoint()
{
    typedef bdlcc::BoundedQueue<AsyncFileObserver_Record> Status;

    // The publication thread may either be 'e_RUNNING' (normal), or
    // 'e_SHUTTING_DOWN' (if a client called a method that stopped the
    // publication thread immediately after starting it, before this thread
    // entry point function is called).  The publication cannot be
    // 'e_NOT_RUNNING', since that state is only returned to after completing
    // this function.

    BSLS_ASSERT(e_RUNNING == d_threadState);

    bool done = false;

    while (!done) {
        AsyncFileObserver_Record record;

        int rc = d_recordQueue.popFront(&record);

        BSLS_ASSERT(0 == rc
                 || Status::e_DISABLED == rc
                 || Status::e_FAILED   == rc);

        if (Status::e_SUCCESS == rc && !isStopRecord(record)) {
            d_fileObserver.publish(record.d_record, record.d_context);
        }
        else {
            done = true;
        }

        // Publish the count of dropped records.  To avoid repeatedly
        // publishing this information when the record queue is full, we
        // publish the number of dropped records only when the queue becomes
        // half empty or when a sufficient number of records have been dropped.
        // Finally, we publish the dropped record count if the observer is
        // shutting down, so the information is not lost.

        if (0 < d_dropCount.loadRelaxed()) {
            if (d_recordQueue.numElements() <= d_recordQueue.numElements() / 2
            ||  d_dropCount.loadRelaxed()   >= k_FORCE_WARN_THRESHOLD
            ||  done) {
                int numDropped = d_dropCount.swap(0);
                BSLS_ASSERT(0 < numDropped); // No other thread should have
                                             // cleared the count.
                logDroppedMessageWarning(&d_fileObserver, numDropped);
            }
        }

        if (Status::e_FAILED == rc) {
            // This is likely a catastrophic unrecoverable error, and one which
            // is very difficult to simular in testing.  There may be
            // "stop-records" still in the queue, rather than adding a
            // complicated mechanism handle them, it is simpler to just empty
            // the queue.

            logQueueFailureError(&d_fileObserver);
            d_recordQueue.removeAll();
        }
    }
    d_threadState = e_NOT_RUNNING;
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
}

// CREATORS
AsyncFileObserver::AsyncFileObserver(bslma::Allocator *basicAllocator)
: d_fileObserver(Severity::e_WARN, basicAllocator)
, d_recordQueue(k_DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_threadState(e_NOT_RUNNING)
, d_dropRecordsOnFullQueueThreshold(Severity::e_OFF)
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

    int rc = (record->fixedFields().severity() > d_dropRecordsOnFullQueueThreshold)
      ? d_recordQueue.tryPushBack(asyncRecord)
      : d_recordQueue.pushBack(asyncRecord);

    if (0 != rc) {
      d_dropCount.addRelaxed(1);
    }

}

void AsyncFileObserver::releaseRecords()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    if (isPublicationThreadRunning()) {
        d_recordQueue.disablePopFront();

        // If either destroying or creating the publicaiton thread fails, it is
        // a catastrophic error.

        int rc = bslmt::ThreadUtil::join(d_threadHandle);

        if (0 != rc) {
            logReleaseRecordsError(&d_fileObserver);
            return;                                                   // RETURN
        }

        d_recordQueue.removeAll();

        d_threadState = e_RUNNING;
        d_recordQueue.enablePopFront();

        bslmt::ThreadAttributes attr;
        setPublicationThreadAttributes(&attr);
        rc = bslmt::ThreadUtil::create(
            &d_threadHandle, attr, d_publishThreadEntryPoint);

        if (0 != rc) {
            logReleaseRecordsError(&d_fileObserver);
        }
    }
    else {
        d_recordQueue.removeAll();
    }
}

int AsyncFileObserver::shutdownPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    BSLMT_MUTEXASSERT_IS_LOCKED(&d_mutex);

    // d_threadState and d_threadHandle should be consistent.

    BSLS_ASSERT((e_NOT_RUNNING == d_threadState) ==
                (bslmt::ThreadUtil::invalidHandle() == d_threadHandle));

    int result = 0;

    if (bslmt::ThreadUtil::invalidHandle() != d_threadHandle) {
        BSLS_ASSERT(e_RUNNING == d_threadState);

        d_recordQueue.disablePopFront();

        result = bslmt::ThreadUtil::join(d_threadHandle);
        d_threadHandle = bslmt::ThreadUtil::invalidHandle();

    }

    // The lock on 'd_mutex' should prevent any other thread from modifying
    // 'd_threadState' after the thread is stopped, so 'd_threadState'
    // and 'd_threadHandle' should be consistent.

    BSLS_ASSERT(e_NOT_RUNNING == d_threadState);
    BSLS_ASSERT(bslmt::ThreadUtil::invalidHandle() == d_threadHandle);

    return result;
}

int AsyncFileObserver::startPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    // d_threadState and d_threadHandle should be consistent.

    BSLS_ASSERT((e_NOT_RUNNING == d_threadState) ==
                (bslmt::ThreadUtil::invalidHandle() == d_threadHandle));

    int result = 0;

    if (bslmt::ThreadUtil::invalidHandle() == d_threadHandle) {
        BSLS_ASSERT(e_NOT_RUNNING == d_threadState);

        d_threadState = e_RUNNING;
        d_recordQueue.enablePopFront();

        bslmt::ThreadAttributes attr;
        setPublicationThreadAttributes(&attr);
        result = bslmt::ThreadUtil::create(&d_threadHandle,
                                           attr,
                                           d_publishThreadEntryPoint);
                                                               // RETURN
    }
    return result;
}

int AsyncFileObserver::stopPublicationThread()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    typedef bdlcc::BoundedQueue<AsyncFileObserver_Record> Status;

    BSLMT_MUTEXASSERT_IS_LOCKED(&d_mutex);

    // d_threadState and d_threadHandle should be consistent.

    BSLS_ASSERT((e_NOT_RUNNING == d_threadState) ==
                (bslmt::ThreadUtil::invalidHandle() == d_threadHandle));


    int result = 0;
    if (bslmt::ThreadUtil::invalidHandle() != d_threadHandle) {
        BSLS_ASSERT(e_RUNNING == d_threadState);

        AsyncFileObserver_Record asyncRecord = createStopRecord();
        {
            // We use 'tryPushBack' because we do not want 'pushBack' to block
            // if queue is full, particulary if the publication thread has
            // already observed 'e_SHUTTING_DOWN' and has stopped removing
            // records from the queue (DRQS 164688087), as may happen on a call
            // to 'shutdownThread'.

            int rc;
            do {
                rc = d_recordQueue.tryPushBack(asyncRecord);
                bslmt::ThreadUtil::yield();
            } while (Status::e_FULL == rc && d_threadState != e_NOT_RUNNING);
        }
        result = bslmt::ThreadUtil::join(d_threadHandle);
        d_threadHandle = bslmt::ThreadUtil::invalidHandle();
    }

    BSLS_ASSERT(e_NOT_RUNNING == d_threadState);
    BSLS_ASSERT(bslmt::ThreadUtil::invalidHandle() == d_threadHandle);

    return result;
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
