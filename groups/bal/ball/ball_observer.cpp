// ball_observer.cpp            -*-C++-*-
#include <ball_observer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_observer_cpp,"$Id$ $CSID$")

#ifdef BDE_FOR_TESTING_ONLY
#include <ball_context.h>              // for testing only
#include <ball_countingallocator.h>    // for testing only
#include <ball_record.h>               // for testing only
#include <ball_transmission.h>         // for testing only
#endif

namespace BloombergLP {

namespace ball {
                           // -------------------
                           // class Observer
                           // -------------------

// CREATORS
Observer::~Observer()
{
    BSLMF_ASSERT(sizeof(Observer) >= sizeof(int));

    // TBD: Remove this test once the observer changes in BDE 2.12 have
    // stabilized.

    *((unsigned int*)this) = 0xdeadbeef;
}

// MANIPULATORS
void Observer::publish(const Record&  ,
                            const Context& )
{
    BSLS_ASSERT_OPT(false);  // Should not be called
}

void Observer::publish(const bsl::shared_ptr<const Record>& record,
                            const Context&                       context)
{
    publish(*record, context);
}

void Observer::releaseRecords()
{
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
