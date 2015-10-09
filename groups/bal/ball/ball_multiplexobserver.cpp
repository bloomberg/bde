// ball_multiplexobserver.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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
                         // -----------------------
                         // class MultiplexObserver
                         // -----------------------

// CREATORS
MultiplexObserver::~MultiplexObserver()
{
    BSLS_ASSERT(0 <= numRegisteredObservers());
}

// MANIPULATORS
void MultiplexObserver::publish(const Record& record, const Context& context)
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);

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
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);

    bsl::set<Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

void MultiplexObserver::releaseRecords()
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);

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

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return !d_observerSet.insert(observer).second;
}

int MultiplexObserver::deregisterObserver(Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    const bool isRegistered =
                           d_observerSet.find(observer) != d_observerSet.end();

    if (isRegistered) {
        d_observerSet.erase(observer);
        observer->releaseRecords();
    }

    return !isRegistered;
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
