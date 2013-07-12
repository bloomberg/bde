// bael_asyncfileobserver.cpp                                         -*-C++-*-
#include <bael_asyncfileobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_asyncfileobserver_cpp,"$Id$ $CSID$")

#ifdef FOR_TESTING_ONLY
#include <bael_defaultobserver.h>             // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#endif

#include <bael_log.h>

#include <bcemt_lockguard.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdesu_processutil.h>

#include <bslma_default.h>
#include <bsl_iostream.h>

namespace BloombergLP {

                       // ----------------------------
                       // class bael_AsyncFileObserver
                       // ----------------------------

namespace {

enum {
    // This enumeration provides the default values for the attributes of
    // 'bael_AsyncFileObserver'.

    DEFAULT_FIXED_QUEUE_SIZE     =  8192
};

const char LOG_CATEGORY[] = "BAEL.ASYNCFILEOBSERVER";

void populateWarnRecord(bael_Record         *record,
                        int                  lineNumber,
                        const bael_Category *category, 
                        int                  numDropped) {
    record->fixedFields().setFileName(__FILE__);
    record->fixedFields().setLineNumber(__LINE__);
    record->fixedFields().setTimestamp(bdetu_SystemTime::nowAsDatetimeUtc());
    record->fixedFields().setCategory(category->categoryName());
    record->fixedFields().setSeverity(bael_Severity::BAEL_WARN);
    static int pid = bdesu_ProcessUtil::getProcessId();
    record->fixedFields().setProcessID(pid);
    record->fixedFields().setThreadID(bcemt_ThreadUtil::selfIdAsUint64());
    bsl::ostream os(&record->fixedFields().messageStreamBuf());
    os << "Dropped " << numDropped << " log records." << bsl::ends;
}
                            

}

// PRIVATE METHODS
void bael_AsyncFileObserver::logDroppedMessageWarning(int numDropped)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    // This is equivalent to BAEL_LOG_WARN, but logs directly to the file 
    // observer rather than through the logger manager to avoid a loop.
    if (BAEL_LOG_THRESHOLD >= bael_Severity::BAEL_WARN &&
        bael_Log::isCategoryEnabled(&BAEL_LOG_CATEGORYHOLDER, 
                                    bael_Severity::BAEL_WARN)) {
        
        bcema_SharedPtr<bael_Record> record;
        record.createInplace(d_allocator_p, d_allocator_p);
        populateWarnRecord(record.ptr(), __LINE__, 
                           BAEL_LOG_CATEGORY, numDropped);
        bael_Context context(bael_Transmission::BAEL_PASSTHROUGH, 0, 0);
        d_fileObserver.publish(record, context);
    }
}

void bael_AsyncFileObserver::publishThreadEntryPoint()
{
    bool done = false;
    while (!done) {
        AsyncRecord asyncRecord = d_recordQueue.popFront();

        // Publish the record only if not shutting down; but check 
        // the dropped message counter whether shutting down or not.  
        if (bael_Transmission::BAEL_END
                == asyncRecord.d_context.transmissionCause()
            || d_clearing) {
            done = true;
        }
        else {
            d_fileObserver.publish(*asyncRecord.d_record, 
                                   asyncRecord.d_context);
        }

        
        int numDropped = d_dropCount.swap(0);
        if (0 < numDropped) {
            logDroppedMessageWarning(numDropped);
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
bael_AsyncFileObserver::bael_AsyncFileObserver(
                                         bael_Severity::Level  stdoutThreshold,
                                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_clearing(false)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_dropCount(0)
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    d_publishThreadEntryPoint
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(
                      &bael_AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
}

bael_AsyncFileObserver::bael_AsyncFileObserver(
                                      bael_Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_recordQueue(DEFAULT_FIXED_QUEUE_SIZE, basicAllocator)
, d_clearing(false)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_dropCount(0)
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    d_publishThreadEntryPoint
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(
                      &bael_AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
}


bael_AsyncFileObserver::bael_AsyncFileObserver(
                                      bael_Severity::Level  stdoutThreshold,
                                      bool                  publishInLocalTime,
                                      int                   maxRecordQueueSize,
                                      bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_clearing(false)
, d_dropRecordsOnFullQueueThreshold(bael_Severity::BAEL_OFF)
, d_dropCount(0)
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    d_publishThreadEntryPoint
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(
                      &bael_AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
}

bael_AsyncFileObserver::bael_AsyncFileObserver(
                         bael_Severity::Level  stdoutThreshold,
                         bool                  publishInLocalTime,
                         int                   maxRecordQueueSize,
                         bael_Severity::Level  dropRecordsOnFullQueueThreshold,
                         bslma::Allocator     *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_recordQueue(maxRecordQueueSize, basicAllocator)
, d_clearing(false)
, d_dropRecordsOnFullQueueThreshold(dropRecordsOnFullQueueThreshold)
, d_dropCount(0)
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    d_publishThreadEntryPoint
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(
                      &bael_AsyncFileObserver::publishThreadEntryPoint,
                      this),
              d_allocator_p);
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
