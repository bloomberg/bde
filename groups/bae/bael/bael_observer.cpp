// bael_observer.cpp            -*-C++-*-
#include <bael_observer.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_observer_cpp,"$Id$ $CSID$")

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
bael_Observer::~bael_Observer()
{
}

// MANIPULATORS
void bael_Observer::publish(const bael_Record&  record,
                            const bael_Context& context) 
{
}

void bael_Observer::publish(const bcema_SharedPtr<const bael_Record>& record,
                            const bael_Context& context) 
{
    publish(*record, context);
}

void bael_Observer::clean()
{
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
