// balb_assertiontracker.cpp                                          -*-C++-*-

#include <balb_assertiontracker.h>
#include <bsls_ident.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslma_default.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_stackaddressutil.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

namespace BloombergLP {
namespace balb {

namespace {

class ThreadLocalDataGuard {
    // This class implements a guard that sets thread-local data to non-zero on
    // creation and resets the data to zero on destruction.

  private:
    // DATA
    bslmt::ThreadUtil::Key d_key;  // thread-local data key

    // PRIVATE CREATORS
    ThreadLocalDataGuard(const ThreadLocalDataGuard&);             // = delete

    // PRIVATE MANIPULATORS
    ThreadLocalDataGuard& operator=(const ThreadLocalDataGuard&);  // = delete

  public:
    // CREATORS
    explicit ThreadLocalDataGuard(bslmt::ThreadUtil::Key key);
        // Create an object of this type, setting the thread-local data
        // corresponding to the specified 'key' to a non-zero value.

    ~ThreadLocalDataGuard();
        // Destroy this object, resetting the thread-local data to zero.
};

ThreadLocalDataGuard::ThreadLocalDataGuard(bslmt::ThreadUtil::Key key)
: d_key(key)
{
    bslmt::ThreadUtil::setSpecific(d_key, this);
}

ThreadLocalDataGuard::~ThreadLocalDataGuard()
{
    bslmt::ThreadUtil::setSpecific(d_key, 0);
}

}  // close unnamed namespace

                            // ----------------------
                            // class AssertionTracker
                            // ----------------------

// CLASS METHODS
void AssertionTracker::reportAssertion(bsl::ostream               *out,
                                       int                         count,
                                       const char                 *text,
                                       const char                 *file,
                                       int                         line,
                                       const bsl::vector<void *>&  stack)
{
    (*out) << count << " " << file << ":" << line << ":" << text << " [";
    bsl::vector<void *>::const_iterator b = stack.begin();
    bsl::vector<void *>::const_iterator e = stack.end();
    for (; b != e; b++) {
        (*out) << " " << b;
    }
    (*out) << " ]\n";
}

// CREATORS
AssertionTracker::AssertionTracker(bsls::Assert::Handler  fallbackHandler,
                                   bslma::Allocator      *basicAllocator)
: d_fallbackHandler(fallbackHandler)
, d_allocator_p(basicAllocator)
, d_maxAssertions(-1)
, d_maxLocations(-1)
, d_maxStackTracesPerLocation(-1)
, d_assertionCount(0)
, d_trackingData(basicAllocator)
, d_callback(bdlf::BindUtil::bindS(d_allocator_p,
                                   &reportAssertion,
                                   &bsl::cout,
                                   bdlf::PlaceHolders::_1,
                                   bdlf::PlaceHolders::_2,
                                   bdlf::PlaceHolders::_3,
                                   bdlf::PlaceHolders::_4,
                                   bdlf::PlaceHolders::_5))
, d_onEachAssertion(false)
, d_onNewLocation(false)
, d_onNewStackTrace(true)
{
    if (bslmt::ThreadUtil::createKey(&d_recursionCheck, 0) != 0) {
        d_fallbackHandler("Cannot get thread-local key", __FILE__, __LINE__);
    }
}

// MANIPULATORS
void AssertionTracker::callback(Callback cb)
{
    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);
    d_callback = cb;
}

void AssertionTracker::operator()(const char *text, const char *file, int line)
{
    if (bslmt::ThreadUtil::getSpecific(d_recursionCheck)) {
        d_fallbackHandler(text, file, line);
        return;                                                       // RETURN
    }

    ThreadLocalDataGuard recursionGuard(d_recursionCheck);

    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);

    if (++d_assertionCount > d_maxAssertions && d_maxAssertions >= 0) {
        d_fallbackHandler(text, file, line);
        return;                                                       // RETURN
    }

    bool newStack = false;
    bool newLocation = false;

    AssertionLocation      location          =
        bsl::make_pair(text, bsl::make_pair(file, line));
    TrackingData::iterator location_iterator = d_trackingData.find(location);
    if (location_iterator == d_trackingData.end())
    {
        if (d_maxLocations >= 0 &&
            int(d_trackingData.size()) >= d_maxLocations) {
            d_fallbackHandler(text, file, line);
            return;                                                   // RETURN
        }
        if (d_maxStackTracesPerLocation == 0) {
            d_fallbackHandler(text, file, line);
        }
        location_iterator =
            d_trackingData.emplace(location, AssertionCounts()).first;
        newLocation = true;
    }

    StackTrace trace(128);
    for (;;) {
        int frames = bsls::StackAddressUtil::getStackAddresses(
                                                 &trace[0], int(trace.size()));
        if (frames < int(trace.size())) {
            trace.resize(frames);
            break;
        }
        trace.resize(2 * trace.size());
    }

    AssertionCounts&          counts          = location_iterator->second;
    AssertionCounts::iterator counts_iterator = counts.find(trace);
    if (counts_iterator == counts.end()) {
        if (d_maxStackTracesPerLocation >= 0 &&
            int(counts.size()) >= d_maxStackTracesPerLocation) {
            d_fallbackHandler(text, file, line);
            return;                                                   // RETURN
        }
        counts_iterator = counts.emplace(trace, 0).first;
        newStack = true;
    }
    ++counts_iterator->second;
    if (newStack && d_onNewStackTrace) {
        d_callback(counts_iterator->second,
                   location_iterator->first.first,
                   location_iterator->first.second.first,
                   location_iterator->first.second.second,
                   counts_iterator->first);
    }
    if (newLocation && d_onNewLocation) {
        d_callback(counts_iterator->second,
                   location_iterator->first.first,
                   location_iterator->first.second.first,
                   location_iterator->first.second.second,
                   counts_iterator->first);
    }
    if (d_onEachAssertion) {
        d_callback(counts_iterator->second,
                   location_iterator->first.first,
                   location_iterator->first.second.first,
                   location_iterator->first.second.second,
                   counts_iterator->first);
    }
}

void AssertionTracker::maxAssertions(int value)
{
    d_maxAssertions = bsl::max(value, -1);
}

void AssertionTracker::maxLocations(int value)
{
    d_maxLocations = bsl::max(value, -1);
}

void AssertionTracker::maxStackTracesPerLocation(int value)
{
    d_maxStackTracesPerLocation = bsl::max(value, -1);
}

void AssertionTracker::onEachAssertion(bool value)
{
    d_onEachAssertion = value;
}

void AssertionTracker::onNewLocation(bool value)
{
    d_onNewLocation = value;
}

void AssertionTracker::onNewStackTrace(bool value)
{
    d_onNewStackTrace = value;
}


// ACCESSORS
bslma::Allocator *AssertionTracker::allocator() const
{
    return d_allocator_p;
}

AssertionTracker::Callback AssertionTracker::callback() const
{
    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);
    return d_callback;
}

void AssertionTracker::iterateAll() const
{
    if (bslmt::ThreadUtil::getSpecific(d_recursionCheck)) {
        return;                                                       // RETURN
    }
    ThreadLocalDataGuard           recursionGuard(d_recursionCheck);
    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);

    bsl::cout << d_assertionCount << " total assertions seen\n";
    TrackingData::const_iterator tb = d_trackingData.begin();
    TrackingData::const_iterator te = d_trackingData.end();
    for (; tb != te; ++tb) {
        AssertionCounts::const_iterator cb = tb->second.begin();
        AssertionCounts::const_iterator ce = tb->second.end();
        for (; cb != ce; ++cb) {
            d_callback(cb->second,
                       tb->first.first,
                       tb->first.second.first,
                       tb->first.second.second,
                       cb->first);
        }
    }
}

int AssertionTracker::maxAssertions() const
{
    return d_maxAssertions;
}

int AssertionTracker::maxLocations() const
{
    return d_maxLocations;
}

int AssertionTracker::maxStackTracesPerLocation() const
{
    return d_maxStackTracesPerLocation;
}

bool AssertionTracker::onEachAssertion() const
{
    return d_onEachAssertion;
}

bool AssertionTracker::onNewLocation() const
{
    return d_onNewLocation;
}

bool AssertionTracker::onNewStackTrace() const
{
    return d_onNewStackTrace;
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
