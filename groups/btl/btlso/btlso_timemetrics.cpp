// btlso_timemetrics.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_timemetrics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_timemetrics_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>
#include <bdlt_currenttime.h>
#include <bsls_assert.h>

static const int k_MILLION = 1000000;

namespace BloombergLP {

namespace btlso {

                           // -----------------
                           // class TimeMetrics
                           // -----------------

// CREATORS
TimeMetrics::TimeMetrics(int               numCategories,
                         int               currentCategory,
                         bslma::Allocator *basicAllocator)
: d_categoryStartTimes(numCategories, bdlt::CurrentTime::now(), basicAllocator)
, d_categoryTimes(numCategories, 0, basicAllocator)
, d_currentCategory(currentCategory)
, d_currentTotal(0)
{
    BSLS_ASSERT(0 < numCategories);
    BSLS_ASSERT(0 <= currentCategory);
    BSLS_ASSERT(currentCategory < numCategories);
}

TimeMetrics::~TimeMetrics()
{
    BSLS_ASSERT(d_categoryStartTimes.size() == d_categoryTimes.size());
    BSLS_ASSERT(d_currentCategory <
                                static_cast<int>(d_categoryStartTimes.size()));
    BSLS_ASSERT(0 <= d_currentCategory);
}

// MANIPULATORS
void TimeMetrics::switchTo(int category)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_dataLock);

    bsls::TimeInterval now   = bdlt::CurrentTime::now();
    bsls::TimeInterval delta = now - d_categoryStartTimes[d_currentCategory];

    int deltaMS = static_cast<int>(
                     delta.seconds() * 1000 + delta.nanoseconds() / k_MILLION);

    d_categoryTimes[d_currentCategory] += deltaMS;
    d_currentTotal                     += deltaMS;

    d_categoryStartTimes[category] = now;
    d_currentCategory = category;
}

void TimeMetrics::resetAll()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_dataLock);

    int numCategories = d_categoryTimes.size();
    for (int i = 0; i < numCategories; ++i) {
        d_categoryTimes[i] = 0;
    }
    d_currentTotal = 0;
}

void TimeMetrics::resetStartTimes()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_dataLock);

    int                numCategories = d_categoryStartTimes.size();
    bsls::TimeInterval now           = bdlt::CurrentTime::now();

    for (int i = 0; i < numCategories; ++i) {
        d_categoryStartTimes[i] = now;
    }
}

int TimeMetrics::percentage(int category)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_dataLock);

    int result = 0;

    BSLS_ASSERT(0 <= category);
    BSLS_ASSERT(category < (int) d_categoryTimes.size());

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    bsls::TimeInterval delta = now - d_categoryStartTimes[d_currentCategory];

    int deltaMS = static_cast<int>(delta.seconds() * 1000 +
                                              delta.nanoseconds() / k_MILLION);

    d_categoryTimes[d_currentCategory] += deltaMS;
    d_currentTotal += deltaMS;

    d_categoryStartTimes[d_currentCategory] = now;

    if (d_currentTotal) {
        result = static_cast<int>(
                           d_categoryTimes[category] * 100.0 / d_currentTotal);
    }

    return result < 0
           ? 0
           : result > 100
             ? 100
             : result;
}

// ACCESSORS
int TimeMetrics::currentCategory() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_dataLock);

    return d_currentCategory;
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
