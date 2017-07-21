// ball_broadcastobserver.cpp                                         -*-C++-*-
#include <ball_broadcastobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_broadcastobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>               // for testing only
#include <ball_record.h>                // for testing only
#include <ball_recordattributes.h>      // for testing only
#include <ball_testobserver.h>          // for testing only
#include <ball_transmission.h>          // for testing only

#include <bslmt_writelockguard.h>

namespace BloombergLP {
namespace ball {

                         // -----------------------
                         // class BroadcastObserver
                         // -----------------------

// CREATORS
BroadcastObserver::~BroadcastObserver()
{
    deregisterAllObservers();
}

// MANIPULATORS
int BroadcastObserver::deregisterObserver(
                                         const bslstl::StringRef& observerName)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::iterator it = d_observers.find(observerName);

    if (it == d_observers.end() ) {
        return 1;                                                     // RETURN
    }

    bsl::shared_ptr<Observer> observer = it->second;

    d_observers.erase(it);

    observer->releaseRecords();

    return 0;
}

void BroadcastObserver::deregisterAllObservers()
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::iterator it = d_observers.begin();

    while (it != d_observers.end()) {
        bsl::shared_ptr<Observer> observer = it->second;

        it = d_observers.erase(it);

        observer->releaseRecords();
    }
}

bsl::shared_ptr<Observer> BroadcastObserver::findObserver(
                                         const bslstl::StringRef& observerName)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::iterator it = d_observers.find(observerName);

    if (it == d_observers.end()) {
        return bsl::shared_ptr<Observer>();                           // RETURN
    }

    return it->second;
}

void BroadcastObserver::publish(const bsl::shared_ptr<const Record>& record,
                                const Context&                       context)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::const_iterator it  = d_observers.begin();
    ObserverRegistry::const_iterator end = d_observers.end();

    for (; it != end; ++it) {
        (it->second)->publish(record, context);
    }
}

int BroadcastObserver::registerObserver(
                                 const bsl::shared_ptr<Observer>& observer,
                                 const bslstl::StringRef&         observerName)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    return !d_observers.emplace(observerName, observer).second;
}

void BroadcastObserver::releaseRecords()
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::const_iterator it  = d_observers.begin();
    ObserverRegistry::const_iterator end = d_observers.end();

    for (; it != end; ++it) {
        (it->second)->releaseRecords();
    }
}

// ACCESSORS
bsl::shared_ptr<const Observer>
BroadcastObserver::findObserver(const bslstl::StringRef& observerName) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    ObserverRegistry::const_iterator it = d_observers.find(observerName);

    if (it == d_observers.end()) {
        return bsl::shared_ptr<const Observer>();                     // RETURN
    }

    return it->second;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
