// bael_multiplexobserver.cpp     -*-C++-*-
#include <bael_multiplexobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_multiplexobserver_cpp,"$Id$ $CSID$")

#include <bael_countingallocator.h>      // for testing only
#include <bael_defaultobserver.h>        // for testing only
#include <bael_testobserver.h>           // for testing only
#include <bsls_assert.h>

namespace BloombergLP {

                         // ----------------------------
                         // class bael_MultiplexObserver
                         // ----------------------------

// CREATORS
bael_MultiplexObserver::~bael_MultiplexObserver()
{
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(0 <= numRegisteredObservers());
}

// MANIPULATORS
void bael_MultiplexObserver::publish(const bcema_SharedPtr<const bael_Record>&  record,
                                     const bael_Context& context)
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    bsl::set<bael_RecordObserver *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

int bael_MultiplexObserver::registerObserver(bael_RecordObserver *recordObserver)
{
    if (0 == recordObserver) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return !d_observerSet.insert(recordObserver).second;
}

int bael_MultiplexObserver::deregisterObserver(bael_RecordObserver *recordObserver)
{
    if (0 == recordObserver) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    const bool isRegistered =
                           d_observerSet.find(recordObserver) != d_observerSet.end();

    if (isRegistered) {
        d_observerSet.erase(recordObserver);
    }

    return !isRegistered;
}

int bael_MultiplexObserver::registerObserver(bael_Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    d_adapterList.push_back(bael_RecordObserverAdapter(observer));
    return !d_observerSet.insert(&d_adapterList.back()).second;
}

int bael_MultiplexObserver::deregisterObserver(bael_Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    bsl::list<bael_RecordObserverAdapter>::iterator l_it = d_adapterList.begin();
    for (; l_it != d_adapterList.end(); ++l_it) {
        if (l_it->observer() == observer) {

            const bool isRegistered =
                    d_observerSet.find(&(*l_it)) != d_observerSet.end();

            if (isRegistered) {
                d_observerSet.erase(&(*l_it));
                d_adapterList.erase(l_it);
            }

            return !isRegistered;
        }
    }
    return 1;
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
