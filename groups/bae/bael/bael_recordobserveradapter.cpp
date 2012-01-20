// bael_recordobserveradapter.cpp            -*-C++-*-
#include <bael_recordobserveradapter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_recordobserveradapter_cpp,"$Id$ $CSID$")

#ifdef BDE_FOR_TESTING_ONLY
#include <bael_context.h>              // for testing only
#include <bael_countingallocator.h>    // for testing only
#include <bael_record.h>               // for testing only
#include <bael_transmission.h>         // for testing only
#endif

namespace BloombergLP {

                           // -------------------
                           // class bael_Observer
                           // -------------------

// CREATORS
bael_RecordObserverAdapter::bael_RecordObserverAdapter(bael_Observer *observer) 
: d_observer_p(observer)
{
}

bael_RecordObserverAdapter::~bael_RecordObserverAdapter()
{
}

// MANIPULATORS
void bael_RecordObserverAdapter::publish(
                                const bcema_SharedPtr<const bael_Record>& record, 
                                const bael_Context& context)
{
    if (d_observer_p) {
        d_observer_p->publish(*record, context);
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
