// btlso_timemetrics.cpp         -*-C++-*-
#include <btlso_timemetrics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_timemetrics_cpp,"$Id$ $CSID$")

#include <bdlqq_lockguard.h>
#include <bdlt_currenttime.h>
#include <bsls_assert.h>

static const int MILLION = 1000000;

namespace BloombergLP {

namespace btlso {
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

TimeMetrics::~TimeMetrics() {
    BSLS_ASSERT(d_categoryStartTimes.size() == d_categoryTimes.size());
    BSLS_ASSERT(d_currentCategory < (int) d_categoryStartTimes.size());
    BSLS_ASSERT(0 <= d_currentCategory);
}

// MANIPULATORS
void TimeMetrics::switchTo(int category) {
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_dataLock);

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    bsls::TimeInterval delta = now - d_categoryStartTimes[d_currentCategory];

    int deltaMS = static_cast<int>(delta.seconds() * 1000 +
                                                delta.nanoseconds() / MILLION);

    d_categoryTimes[d_currentCategory] += deltaMS;
    d_currentTotal += deltaMS;

    d_categoryStartTimes[category] = now;
    d_currentCategory = category;
}

void TimeMetrics::resetAll()
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_dataLock);
    int numCategories = d_categoryTimes.size();
    for (int i = 0; i < numCategories; ++i) {
        d_categoryTimes[i] = 0;
    }
    d_currentTotal = 0;
}

void TimeMetrics::resetStartTimes()
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_dataLock);
    int numCategories = d_categoryStartTimes.size();
    bsls::TimeInterval now = bdlt::CurrentTime::now();

    for (int i = 0; i < numCategories; ++i) {
        d_categoryStartTimes[i] = now;
    }
}

int TimeMetrics::percentage(int category)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_dataLock);

    int result = 0;
    BSLS_ASSERT(0 <= category);
    BSLS_ASSERT(category < (int) d_categoryTimes.size());

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    bsls::TimeInterval delta = now -
        d_categoryStartTimes[d_currentCategory];
    int deltaMS = static_cast<int>(delta.seconds() * 1000 +
                                                delta.nanoseconds() / MILLION);
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
int TimeMetrics::currentCategory() const
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_dataLock);
    return d_currentCategory;
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
