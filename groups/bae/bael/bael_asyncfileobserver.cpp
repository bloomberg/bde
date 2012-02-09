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

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

#include <bslma_default.h>

using namespace bsl;

namespace {

const char *const DEFAULT_LONG_FORMAT =  "\n%d %p:%t %s %f:%l %c %m %u\n";
const char *const DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS =
                                         "\n%d %p:%t %s %f:%l %c %m\n";

const char *const DEFAULT_SHORT_FORMAT = "\n%s %f:%l %c %m %u\n";
const char *const DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS =
                                         "\n%s %f:%l %c %m\n";

}  // close unnamed namespace

namespace BloombergLP {

                       // ----------------------------
                       // class bael_AsyncFileObserver
                       // ----------------------------

// PRIVATE METHODS
void bael_AsyncFileObserver::publishThreadEntryPoint()
{
    while (1) 
    {
        AsyncRecord r = d_recordQueue.popFront();
        if (bael_Transmission::BAEL_END == r.d_context.transmissionCause()
            || d_clearing) 
            break;
        d_fileObserver.publish(*r.d_record, r.d_context);
    }
}

// CREATORS
bael_AsyncFileObserver::bael_AsyncFileObserver(
                bael_Severity::Level  stdoutThreshold,
                bslma_Allocator      *basicAllocator)
: d_fileObserver(stdoutThreshold, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_clearing(false)
, d_recordQueue(8000, basicAllocator) 
, d_allocator_p(bslma_Default::globalAllocator(basicAllocator))
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
                bslma_Allocator      *basicAllocator)
: d_fileObserver(stdoutThreshold, publishInLocalTime, basicAllocator)
, d_threadHandle(bcemt_ThreadUtil::invalidHandle())
, d_clearing(false)
, d_recordQueue(8000, basicAllocator) 
, d_allocator_p(bslma_Default::globalAllocator(basicAllocator))

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
}

// MANIPULATORS
void bael_AsyncFileObserver::clear()
{
    d_clearing = true;
    stopThread();
    d_recordQueue.removeAll();
    d_clearing = false;
    startThread();
}

void bael_AsyncFileObserver::publish(const bael_Record& record, 
                                     const bael_Context& context)
{   
    // DEPRECATED
}

void bael_AsyncFileObserver::publish(
                const bcema_SharedPtr<const bael_Record>& record,
                const bael_Context& context)
{
    AsyncRecord r;
    r.d_record  = record;
    r.d_context = context;
    d_recordQueue.pushBack(r);
}

void bael_AsyncFileObserver::startThread()
{
    if (bcemt_ThreadUtil::invalidHandle() == d_threadHandle)
    {
        bcemt_ThreadAttributes attr;
        bcemt_ThreadUtil::create(&d_threadHandle, 
                                 attr,
                                 d_publishThreadEntryPoint);
    }
}

void bael_AsyncFileObserver::stopThread()
{
    if (bcemt_ThreadUtil::invalidHandle() != d_threadHandle) 
    {
        // Push an empty record with BAEL_END set in context
        bcema_SharedPtr<const bael_Record> record(
                new (*d_allocator_p) bael_Record(d_allocator_p), 
                d_allocator_p);
        bael_Context context(bael_Transmission::BAEL_END, 0, 1);
        publish(record, context);
        bcemt_ThreadUtil::join(d_threadHandle);
    }
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
