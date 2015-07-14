// ball_asyncfileobserver.cpp                                         -*-C++-*-
#include <ball_asyncfileobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_asyncfileobserver_cpp,"$Id$ $CSID$")

#ifdef FOR_TESTING_ONLY
#include <ball_defaultobserver.h>             // for testing only
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_multiplexobserver.h>           // for testing only
#endif

#include <bdlmtt_lockguard.h>

#include <bdlf_function.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlsu_processutil.h>
#include <bdlt_currenttime.h>

#include <bslma_default.h>
#include <bsl_iostream.h>

// IMPLEMENTATION NOTE: 'shutdownThread' clears the queue in order to simplify
// the implementation.  To guarantee that a thread sees the
// 'd_shuttingDownFlag' a ('ball::Transmission::BAEL_END') record is
// appended to the queue, otherwise the publication thread may be blocked
// indefinitely on 'popFront'.  Unfortunately that potentially leaves a bogus
// record in the queue after the publication thread is shutdown -- to avoid
// dealing with that record (when the thread is restarted) the queue is
// cleared.  Alternative designs are possible, but are not perceived to be
// worth the added complexity.

namespace BloombergLP {

                       // ----------------------------
                       // class ball::AsyncFileObserver
                       // ----------------------------

namespace {

enum {
    DEFAULT_FIXED_QUEUE_SIZE     =  8192,
    FORCE_WARN_THRESHOLD         =  5000
};

const char LOG_CATEGORY[] = "BAEL.ASYNCFILEOBSERVER";

static void populateWarnRecord(ball::Record         *record,
                               int                  lineNumber,
                               int                  numDropped)
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

namespace ball {
// PRIVATE METHODS
void AsyncFileObserver::logDroppedMessageWarning(int numDropped)
{
    // Log the record, unconditionally, to the file observer (i.e., without
    // consulting the logger manager as to whether WARN is enabled) to avoid
    // an observer->loggermanager dependency.

    populateWarnRecord(&d_droppedRecordWarning, __LINE__, numDropped);
    Context context(Transmission::BAEL_PASSTHROUGH, 0, 0);
    d_fileObserver.publish(d_droppedRecordWarning, context);
}

void AsyncFileObserver::publishThreadEntryPoint()
{
    bool done = false;
    d_droppedRecordWarning.fixedFields().setThreadID(
                                          bdlmtt::ThreadUtil::selfIdAsUint64());

    while (!done) {
        AsyncRecord asyncRecord = d_recordQueue.popFront();

        // Publish the next log record on the queue only if the observer is
        // not shutting down.

        if (Transmission::BAEL_END
                == asyncRecord.d_context.transmissionCause()
            || d_shuttingDownFlag) {
            done = true;
        }
        else {
            d_fileObserver.publish(*asyncRecord.d_record,
                                   asyncRecord.d_context);
        }

        // Publish the count of dropped records.  To avoid repeatedly
        // publishing this information when the record queue is full, we
        // publish the number of dropped records only when the queue becomes
        // half empty or when a sufficient number of records have been
        // dropped.  Finally, we publish the dropped record count if the
        // observer is shutting down, so the information is not lost.

        if (0 < d_dropCount.loadRelaxed()) {
            if (d_recordQueue.length() <= d_recordQueue.size() / 2
            ||  d_dropCount.loadRelaxed() >= FORCE_WARN_THRESHOLD
            ||  d_shuttingDownFlag) {
                int numDropped = d_dropCount.swap(0);
                BSLS_ASSERT(0 < numDropped); // No other thread should have
                                             // cleared the count.
                logDroppedMessageWarning(numDropped);
            }
        }
    }
}

int AsyncFileObserver::startThread()
{
    if (bdlmtt::ThreadUtil::invalidHandle() == d_threadHandle) {
        bdlmtt::ThreadAttributes attr;
        return bdlmtt::ThreadUtil::create(&d_threadHandle,
                                        attr,
                                        d_publishThreadEntryPoint);   // RETURN
    }
    return 0;
}

int AsyncFileObserver::stopThread()
{
    if (bdlmtt::ThreadUtil::invalidHandle() != d_threadHandle) {
        // Push an empty record with BAEL_END set in context.

        AsyncRecord asyncRecord;
        bsl::shared_ptr<const Record> record(
                               new (*d_allocator_p) Record(d_allocator_p),
                               d_allocator_p);
        Context context(Transmission::BAEL_END, 0, 1);
        asyncRecord.d_record  = record;
        asyncRecord.d_context = context;
        d_recordQueue.pushBack(asyncRecord);

        int ret = bdlmtt::ThreadUtil::join(d_threadHandle);
        d_threadHandle = bdlmtt::ThreadUtil::invalidHandle();
        return ret;                                                   // RETURN
    }
    return 0;
}

int AsyncFileObserver::shutdownThread()
{
    d_shuttingDownFlag = 1;

    // We call 'stopThread' to enqueue a bogus log record to ensure the
    // publication thread is woken up to see that 'd_shuttingDownFlag' is
    // 'true' (see implementation note).

    int ret =  stopThread();

    // We clear the queue to remove the bogus log record appended by
    // 'stopThread'. 

    d_recordQueue.removeAll();
    d_shuttingDownFlag = 0;
    return ret;
}

void AsyncFileObserver::construct()
{
    d_threadHandle     = bdlmtt::ThreadUtil::invalidHandle();
    d_shuttingDownFlag = 0;
    d_dropCount        = 0;

    d_publishThreadEntryPoint
        = bdlf::Function<void (*)()>(
              bdlf::MemFnUtil::memFn(
                      &AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
    d_droppedRecordWarning.fixedFields().setFileName(__FILE__);
    d_droppedRecordWarning.fixedFields().setCategory(LOG_CATEGORY);
    d_droppedRecordWarning.fixedFields().setSeverity(
                                          Severity::BAEL_WARN);
    d_droppedRecordWarning.fixedFields().setProcessID(
                                          bdlsu::ProcessUtil::getProcessId());
}

// CREATORS
AsyncFileObserver::AsyncFileObserver(
                                         Severity::Level  stdoutThreshold,
                                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, basicAllocator)
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_shuttingDownFlag(0)
, d_dropRecordsOnFullQueueThreshold(Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(
                                      Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_shuttingDownFlag(0)
, d_dropRecordsOnFullQueueThreshold(Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}


AsyncFileObserver::AsyncFileObserver(
                                      Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      int                   maxRecordQueueSize,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_shuttingDownFlag(0)
, d_dropRecordsOnFullQueueThreshold(Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

AsyncFileObserver::AsyncFileObserver(
                         Severity::Level  stdoutThreshold,
                         bool                  publishInLocalTime,
                         int                   maxRecordQueueSize,
                         Severity::Level  dropRecordsOnFullQueueThreshold,
                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_shuttingDownFlag(0)
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
void AsyncFileObserver::releaseRecords()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    if (isPublicationThreadRunning()) {
        shutdownThread();
        startThread();
    }
    else {
        d_recordQueue.removeAll();
    }
}

void AsyncFileObserver::publish(
                const bsl::shared_ptr<const Record>& record,
                const Context&                       context)
{
    AsyncRecord asyncRecord;
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

int AsyncFileObserver::startPublicationThread()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return startThread();
}

int AsyncFileObserver::stopPublicationThread()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return stopThread();
}

int AsyncFileObserver::shutdownPublicationThread()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return shutdownThread();
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
