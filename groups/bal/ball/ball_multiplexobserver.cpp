// ball_multiplexobserver.cpp     -*-C++-*-
#include <ball_multiplexobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_multiplexobserver_cpp,"$Id$ $CSID$")

#include <ball_countingallocator.h>      // for testing only
#include <ball_defaultobserver.h>        // for testing only
#include <ball_testobserver.h>           // for testing only
#include <bsls_assert.h>
#include <bsl_iostream.h>                // for warning print only

namespace BloombergLP {

namespace ball {
                         // ----------------------------
                         // class MultiplexObserver
                         // ----------------------------

// CREATORS
MultiplexObserver::~MultiplexObserver()
{
    BSLS_ASSERT(0 <= numRegisteredObservers());
}

// MANIPULATORS
void MultiplexObserver::publish(const Record&  record,
                                     const Context& context)
{
    bdlqq::ReadLockGuard<bdlqq::RWMutex> guard(&d_rwMutex);

    // Print warning once that this publish method is deprecated.

    static bool needWarning = true;
    if (needWarning) {
        bsl::cerr << "WARNING: MultiplexObserver: this publish method is "
                  << "deprecated, please use the alternative publish overload."
                  << bsl::endl;
        needWarning = false;
    }

    bsl::set<Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

void MultiplexObserver::publish(
                            const bsl::shared_ptr<const Record>&  record,
                            const Context&                        context)
{
    bdlqq::ReadLockGuard<bdlqq::RWMutex> guard(&d_rwMutex);

    bsl::set<Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

void MultiplexObserver::releaseRecords()
{
    bdlqq::ReadLockGuard<bdlqq::RWMutex> guard(&d_rwMutex);

    bsl::set<Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        // TBD: Remove this test once the observer changes in BDE 2.12 have
        // stabilized.

        if (0xdeadbeef == *((unsigned int*)(*it))) {
            bsl::cerr << "ERROR: MultiplexObserver: "
                      << "Observer is destroyed before being deregistered."
                      << " [releaseRecords]" << bsl::endl;
        }
        else {
            (*it)->releaseRecords();
        }
    }
}

int MultiplexObserver::registerObserver(Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bdlqq::WriteLockGuard<bdlqq::RWMutex> guard(&d_rwMutex);
    return !d_observerSet.insert(observer).second;
}

int MultiplexObserver::deregisterObserver(Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bdlqq::WriteLockGuard<bdlqq::RWMutex> guard(&d_rwMutex);
    const bool isRegistered =
                           d_observerSet.find(observer) != d_observerSet.end();

    if (isRegistered) {
        d_observerSet.erase(observer);
        observer->releaseRecords();
    }

    return !isRegistered;
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
