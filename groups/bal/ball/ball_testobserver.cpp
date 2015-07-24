// ball_testobserver.cpp     -*-C++-*-
#include <ball_testobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_testobserver_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC DATA MEMBER INITIALIZATION
bsls::AtomicInt ball::TestObserver::s_count;

namespace ball {
// CREATORS
TestObserver::~TestObserver()
{
}

// MANIPULATORS
void TestObserver::publish(const Record&  record,
                                const Context& context)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

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
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
