// bteso_timemetrics.cpp         -*-C++-*-
#include <bteso_timemetrics.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_timemetrics_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bdetu_systemtime.h>
#include <bsls_assert.h>

namespace BloombergLP {

// CREATORS
bteso_TimeMetrics::bteso_TimeMetrics(int              numCategories,
                                     int              currentCategory,
                                     bslma_Allocator *basicAllocator)
: d_categoryStartTimes(numCategories, bdetu_SystemTime::now(), basicAllocator)
, d_categoryTimes(numCategories, 0, basicAllocator)
, d_currentCategory(currentCategory)
, d_currentTotal(0)
{
    BSLS_ASSERT(0 < numCategories);
    BSLS_ASSERT(0 <= currentCategory);
    BSLS_ASSERT(currentCategory < numCategories);
}

bteso_TimeMetrics::~bteso_TimeMetrics() {
    BSLS_ASSERT(d_categoryStartTimes.size() == d_categoryTimes.size());
    BSLS_ASSERT(d_currentCategory < (int) d_categoryStartTimes.size());
    BSLS_ASSERT(0 <= d_currentCategory);
}

// MANIPULATORS
void bteso_TimeMetrics::switchTo(int category) {
    bcemt_LockGuard<bcemt_Mutex> lock(&d_dataLock);

    bdet_TimeInterval now = bdetu_SystemTime::now();
    bdet_TimeInterval delta = now - d_categoryStartTimes[d_currentCategory];

    int deltaMS = (int) (delta.seconds() * 1000 + delta.nanoseconds() / 1e6);

    d_categoryTimes[d_currentCategory] += deltaMS;
    d_currentTotal += deltaMS;

    d_categoryStartTimes[category] = now;
    d_currentCategory = category;
}

void bteso_TimeMetrics::resetAll()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_dataLock);
    int numCategories = d_categoryTimes.size();
    for (int i = 0; i < numCategories; ++i) {
        d_categoryTimes[i] = 0;
    }
    d_currentTotal = 0;
}

void bteso_TimeMetrics::resetStartTimes()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_dataLock);
    int numCategories = d_categoryStartTimes.size();
    bdet_TimeInterval now = bdetu_SystemTime::now();

    for (int i = 0; i < numCategories; ++i) {
        d_categoryStartTimes[i] = now;
    }
}

int bteso_TimeMetrics::percentage(int category)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_dataLock);

    int result = 0;
    BSLS_ASSERT(0 <= category);
    BSLS_ASSERT(category < (int) d_categoryTimes.size());

    bdet_TimeInterval now = bdetu_SystemTime::now();
    bdet_TimeInterval delta = now -
        d_categoryStartTimes[d_currentCategory];
    int deltaMS = (int) (delta.seconds() * 1000 + delta.nanoseconds() / 1e6);
    d_categoryTimes[d_currentCategory] += deltaMS;
    d_currentTotal += deltaMS;

    d_categoryStartTimes[d_currentCategory] = now;

    if (d_currentTotal) {
        result = (int) (d_categoryTimes[category] * 100.0 / d_currentTotal);
    }
    return result < 0   ? 0
         : result > 100 ? 100
         : result;
}

// ACCESSORS
int bteso_TimeMetrics::currentCategory() const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_dataLock);
    return d_currentCategory;
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
