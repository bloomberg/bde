// bael_asyncfileobserver.cpp                                         -*-C++-*-
#include <bael_asyncfileobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_asyncfileobserver_cpp,"$Id$ $CSID$")

#ifdef FOR_TESTING_ONLY
#include <bael_defaultobserver.h>             // for testing only
#include <bael_log.h>                         // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#endif

#include <bcemt_lockguard.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdesu_processutil.h>
#include <bdetu_systemtime.h>

#include <bslma_default.h>
#include <bsl_iostream.h>

namespace BloombergLP {

                       // ----------------------------
                       // class bael_AsyncFileObserver
                       // ----------------------------

namespace {

enum {
    DEFAULT_FIXED_QUEUE_SIZE     =  8192,
    FORCE_WARN_THRESHOLD         =  5000
};

const char LOG_CATEGORY[] = "BAEL.ASYNCFILEOBSERVER";

static void populateWarnRecord(bael_Record         *record,
                               int                  lineNumber,
                               int                  numDropped)
    // Load the specified 'record' with a warning message indicating the
    // specified 'numDropped' records have been dropped as recorded at the
    // specified 'lineNumber'.
{
    record->fixedFields().messageStreamBuf().pubseekpos(0);
    record->fixedFields().setLineNumber(lineNumber);
    record->fixedFields().setTimestamp(bdetu_SystemTime::nowAsDatetimeUtc());
    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "Dropped " << numDropped << " log records." << bsl::ends;
}

}  // close unnamed namespace

// PRIVATE METHODS
void bael_AsyncFileObserver::logDroppedMessageWarning(int numDropped)
{
    // Log the record, unconditionally, to the file observer (i.e., without
    // consulting the logger manager as to whether WARN is enabled) to avoid
    // an observer->loggermanager dependency.

    populateWarnRecord(&d_droppedRecordWarning, __LINE__, numDropped);
    bael_Context context(bael_Transmission::BAEL_PASSTHROUGH, 0, 0);
    d_fileObserver.publish(d_droppedRecordWarning, context);
}

void bael_AsyncFileObserver::publishThreadEntryPoint()
{
    bool done = false;
    d_droppedRecordWarning.fixedFields().setThreadID(
                                          bcemt_ThreadUtil::selfIdAsUint64());

    while (!done) {
        AsyncRecord asyncRecord = d_recordQueue.popFront();

        // Publish the next log record on the queue only if the observer is
        // not shutting down.

        if (bael_Transmission::BAEL_END
                == asyncRecord.d_context.transmissionCause()
            || d_clearing) {
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

        if (0 < d_dropCount.relaxedLoad()) {
            if (d_recordQueue.length() <= d_recordQueue.size() / 2
            ||  d_dropCount.relaxedLoad() >= FORCE_WARN_THRESHOLD
            ||  d_clearing) {
                int numDropped = d_dropCount.swap(0);
                BSLS_ASSERT(0 < numDropped); // No other thread should have
                                             // cleared the count.
                logDroppedMessageWarning(numDropped);
            }
        }
    }
}

int bael_AsyncFileObserver::startThread()
{
    if (bcemt_ThreadUtil::invalidHandle() == d_threadHandle) {
        bcemt_ThreadAttributes attr;
        return bcemt_ThreadUtil::create(&d_threadHandle,
                                        attr,
                                        d_publishThreadEntryPoint);   // RETURN
    }
    return 0;
}

int bael_AsyncFileObserver::stopThread()
{
    if (bcemt_ThreadUtil::invalidHandle() != d_threadHandle) {
        // Push an empty record with BAEL_END set in context

        AsyncRecord asyncRecord;
        bcema_SharedPtr<const bael_Record> record(
                               new (*d_allocator_p) bael_Record(d_allocator_p),
                               d_allocator_p);
        bael_Context context(bael_Transmission::BAEL_END, 0, 1);
        asyncRecord.d_record  = record;
        asyncRecord.d_context = context;
        d_recordQueue.pushBack(asyncRecord);

        int ret = bcemt_ThreadUtil::join(d_threadHandle);
        d_threadHandle = bcemt_ThreadUtil::invalidHandle();
        return ret;                                                   // RETURN
    }
    return 0;
}

// CREATORS
void
bael_AsyncFileObserver::construct()
{
    d_threadHandle = bcemt_ThreadUtil::invalidHandle();
    d_clearing = false;
    d_dropCount = 0;
    d_publishThreadEntryPoint
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(
                      &bael_AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
    d_droppedRecordWarning.fixedFields().setFileName(__FILE__);
    d_droppedRecordWarning.fixedFields().setCategory(LOG_CATEGORY);
    d_droppedRecordWarning.fixedFields().setSeverity(
                                          bael_Severity::BAEL_WARN);
    d_droppedRecordWarning.fixedFields().setProcessID(
                                          bdesu_ProcessUtil::getProcessId());
}
bael_AsyncFileObserver::bael_AsyncFileObserver(
                                         bael_Severity::Level  stdoutThreshold,
                                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, basicAllocator)
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

bael_AsyncFileObserver::bael_AsyncFileObserver(
                                      bael_Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}


bael_AsyncFileObserver::bael_AsyncFileObserver(
                                      bael_Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      int                   maxRecordQueueSize,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

bael_AsyncFileObserver::bael_AsyncFileObserver(
                         bael_Severity::Level  stdoutThreshold,
                         bool                  publishInLocalTime,
                         int                   maxRecordQueueSize,
                         bael_Severity::Level  dropRecordsOnFullQueueThreshold,
                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_dropRecordsOnFullQueueThreshold(dropRecordsOnFullQueueThreshold)
, d_droppedRecordWarning(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    construct();
}

bael_AsyncFileObserver::~bael_AsyncFileObserver()
{
    stopPublicationThread();
}

// MANIPULATORS
void bael_AsyncFileObserver::releaseRecords()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (isPublicationThreadRunning()) {
        d_clearing = true;
        stopThread();
        d_recordQueue.removeAll();
        d_clearing = false;
        startThread();
    }
    else {
        d_recordQueue.removeAll();
    }
}

void bael_AsyncFileObserver::publish(
                const bcema_SharedPtr<const bael_Record>& record,
                const bael_Context&                       context)
{
    AsyncRecord asyncRecord;
    asyncRecord.d_record  = record;
    asyncRecord.d_context = context;
    if (record->fixedFields().severity() > d_dropRecordsOnFullQueueThreshold) {
        if (0 != d_recordQueue.tryPushBack(asyncRecord)) {
            d_dropCount.relaxedAdd(1);
        }
    }
    else {
        d_recordQueue.pushBack(asyncRecord);
    }
}

int bael_AsyncFileObserver::startPublicationThread()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return startThread();
}

int bael_AsyncFileObserver::stopPublicationThread()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return stopThread();
}

int bael_AsyncFileObserver::shutdownPublicationThread()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_clearing = true;
    int ret =  stopThread();
    d_recordQueue.removeAll();
    d_clearing = false;
    return ret;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
