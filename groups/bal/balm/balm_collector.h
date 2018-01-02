// balm_collector.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_COLLECTOR
#define INCLUDED_BALM_COLLECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: balm_collector.h,v 1.7 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide a container for collecting and aggregating metric values.
//
//@CLASSES:
//   balm::Collector: a container for collecting and aggregating metric values
//
//@SEE_ALSO: balm_collectorrepository, balm_metric
//
//@DESCRIPTION: This component provides a class for collecting and aggregating
// the values of a metric.  The collector records the number of times an event
// occurs as well as an associated measurement value.  This component does
// *not* define what constitutes an event or what the value measures.  The
// collector manages, in a thread-safe manner, the count of event occurrences
// and the aggregated minimum, maximum, and total of the measured metric
// value.  This collector class provides operations to update the aggregated
// value, a 'load' operator to populate a 'balm::MetricRecord' with the current
// state of the collector, a 'reset' method to reset the current state of the
// collector, and finally a combined 'loadAndReset' method that performs both
// a load and a reset as an atomic operation.  Note that in practice, most
// clients should not need to access a 'balm::Collector' directly, but instead
// use it through another type (see 'balm_metric').
//
///Thread Safety
///-------------
// 'balm::Collector' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following example creates a 'balm::Collector', modifies its values, then
// collects a 'balm::MetricRecord'.
//
// We start by creating a 'balm::MetricId' object by hand, but in practice, an
// id should be obtained from a 'balm::MetricRegistry' object (such as the one
// owned by a 'balm::MetricsManager'):
//..
//  balm::Category           myCategory("MyCategory");
//  balm::MetricDescription  description(&myCategory, "MyMetric");
//  balm::MetricId           myMetric(&description);
//..
// Now we create a 'balm::Collector' object for 'myMetric' and use the 'update'
// method to update its collected value:
//..
//  balm::Collector collector(myMetric);
//
//  collector.update(1.0);
//  collector.update(3.0);
//..
// The collector accumulated the values 1 and 3.  The result should have a
// count of 2, a total of 4 (3 + 1), a max of 3 (max(3, 1)), and a min of 1
// (min(3, 1)).
//..
//  balm::MetricRecord record;
//  collector.loadAndReset(&record);
//
//      assert(myMetric == record.metricId());
//      assert(2        == record.count());
//      assert(4        == record.total());
//      assert(1.0      == record.min());
//      assert(3.0      == record.max());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_METRICRECORD
#include <balm_metricrecord.h>
#endif

#ifndef INCLUDED_BALM_METRICID
#include <balm_metricid.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

namespace BloombergLP {


namespace balm {

                              // ===============
                              // class Collector
                              // ===============

class Collector {
    // This class provides a mechanism for collecting and aggregating the
    // value of a metric over a period of time.  The collector contains a
    // 'MetricRecord' object that holds the identity of the metric being
    // collected, the number of times an event occurred, and the total,
    // minimum, and maximum aggregates of the associated measurement value.
    // The default value for the count is 0, the default value for the total
    // is 0.0, the default minimum value is 'MetricRecord::k_DEFAULT_MIN', and
    // the default maximum value is 'MetricRecord::k_DEFAULT_MAX'.

    // DATA
    MetricRecord         d_record;  // the recorded metric information
    mutable bslmt::Mutex d_lock;    // record synchronization mechanism

    // NOT IMPLEMENTED
    Collector(const Collector&);
    Collector& operator=(const Collector&);

  public:
     // CREATORS
    Collector(const MetricId& metricId);
        // Create a collector for a metric having the specified 'metricId',
        // and having an initial count of 0, total of 0.0, min of
        // 'MetricRecord::k_DEFAULT_MIN', and max of
        // 'MetricRecord::k_DEFAULT_MAX'.

    ~Collector();
        // Destroy this object.

    // MANIPULATORS
    void reset();
        // Reset the count, total, minimum, and maximum values of the metric
        // being collected to their default states.  After this operation, the
        // count and total values will be 0, the minimum value will be
        // 'MetricRecord::k_DEFAULT_MIN', and the maximum value will be
        // 'MetricRecord::k_DEFAULT_MAX'.

    void loadAndReset(MetricRecord *record);
        // Load into the specified 'record' the id of the metric being
        // collected as well as the current count, total, minimum, and maximum
        // aggregated values for that metric; then reset the count, total,
        // minimum, and maximum values to their default states.  After this
        // operation, the count and total values will be 0, the minimum value
        // will be 'MetricRecord::k_DEFAULT_MIN', and the maximum value will be
        // 'MetricRecord::k_DEFAULT_MAX'.  Note that this operation is
        // logically equivalent to calling the 'load' and then the 'reset'
        // methods except that it is performed as a single atomic operation.

    void update(double value);
        // Increment the event count by 1, add the specified 'value' to the
        // total, if 'value' is less than the minimum value, set 'value' to be
        // the minimum value, and if 'value' is greater than the maximum
        // value, set 'value' to be the maximum value.

    void accumulateCountTotalMinMax(int    count,
                                    double total,
                                    double min,
                                    double max);
        // Increment the event count by the specified 'count', add the
        // specified 'total' to the accumulated total, if specified 'min' is
        // less than the minimum value, set 'min' to be the minimum value, and
        // if specified 'max' is greater than the maximum value, set 'max' to
        // be the maximum value.

    void setCountTotalMinMax(int count, double total, double min, double max);
        // Set the event count to the specified 'count', the total aggregate to
        // the specified 'total', the minimum aggregate to the specified 'min'
        // and the maximum aggregate to the specified 'max'.

    // ACCESSORS
    const MetricId& metricId() const;
        // Return a reference to the non-modifiable 'MetricId' object
        // identifying the metric for which this object collects values.

    void load(MetricRecord *record) const;
        // Load into the specified 'record' the id of the metric being
        // collected, as well as the current count, total, minimum, and
        // maximum aggregated values for the metric.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // class Collector
                              // ---------------

// CREATORS
inline
Collector::Collector(const MetricId& metricId)
: d_record(metricId)
, d_lock()
{
}

inline
Collector::~Collector()
{
}

// MANIPULATORS
inline
void Collector::reset()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    d_record.count() = 0;
    d_record.total() = 0.0;
    d_record.min()   = MetricRecord::k_DEFAULT_MIN;
    d_record.max()   = MetricRecord::k_DEFAULT_MAX;
}

inline
void Collector::loadAndReset(MetricRecord *record)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    *record          = d_record;
    d_record.count() = 0;
    d_record.total() = 0.0;
    d_record.min()   = MetricRecord::k_DEFAULT_MIN;
    d_record.max()   = MetricRecord::k_DEFAULT_MAX;
}

inline
void Collector::update(double value)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    ++d_record.count();
    d_record.total() += value;
    d_record.min()   =  bsl::min(d_record.min(), value);
    d_record.max()   =  bsl::max(d_record.max(), value);
}

inline
void Collector::accumulateCountTotalMinMax(int    count,
                                           double total,
                                           double min,
                                           double max)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    d_record.count() += count;
    d_record.total() += total;
    d_record.min()   =  bsl::min(d_record.min(), min);
    d_record.max()   =  bsl::max(d_record.max(), max);
}

inline
void Collector::setCountTotalMinMax(int    count,
                                    double total,
                                    double min,
                                    double max)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    d_record.count() = count;
    d_record.total() = total;
    d_record.min()   = min;
    d_record.max()   = max;
}

// ACCESSORS
inline
const MetricId& Collector::metricId() const
{
    return d_record.metricId();
}

inline
void Collector::load(MetricRecord *record) const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
    *record = d_record;
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
