// bael_testobserver.cpp     -*-C++-*-
#include <bael_testobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_testobserver_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC DATA MEMBER INITIALIZATION
bces_AtomicInt bael_TestObserver::s_count;

// CREATORS
bael_TestObserver::~bael_TestObserver()
{
}

// MANIPULATORS
void bael_TestObserver::publish(const bael_Record&  record,
                                const bael_Context& context)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    d_record  = record;
    d_context = context;
    ++d_numRecords;

    if (d_verboseFlag) {
        d_stream << "Test Observer ID " << d_count
                 << " publishing record number " << d_numRecords << '\n'
                 << "Context: cause = " << context.transmissionCause() << '\n'
                 << "         count = " << context.recordIndex() + 1
                 << " of an expected " << context.sequenceLength()
                 << " total records.\n" << bsl::flush;
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
