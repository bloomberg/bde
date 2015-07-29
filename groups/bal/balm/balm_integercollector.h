// balm_integercollector.h              -*-C++-*-
#ifndef INCLUDED_BALM_INTEGERCOLLECTOR
#define INCLUDED_BALM_INTEGERCOLLECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for collecting integral metric values.
//
//@CLASSES:
//   balm::IntegerCollector: a container for collecting integral values
//
//@SEE_ALSO:
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class for collecting and aggregating
// the values of an integral metric.  The 'balm::IntegerCollector' records the
// number of times an event occurs as well as an associated integral
// measurement value.  This component does *not* define what constitutes an
// event or what the value measures.  The collector manages, in a fully
// thread-safe manner, the count of event occurrences and the aggregated
// minimum, maximum, and total of the measured metric value.  This collector
// class provides operations to update the aggregated value, a 'load' method
// to populate a 'balm::MetricRecord' with the current state of the collector,
// a 'reset' operator to reset the current state of the integer collector,
// and finally a combined 'loadAndReset' method that performs both a load and a
// reset in a single (atomic) operation.
//
///Thread Safety
///-------------
// 'balm::IntegerCollector' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following example creates a 'balm::IntegerCollector', modifies its
// values, then collects a 'balm::MetricRecord'.
//
// We start by creating a 'balm::MetricId' object by hand, but in practice,
// an id should be obtained from a 'balm_MetricRegistry' object (such as the
// one owned by a 'balm::MetricsManager'):
//..
//  balm::Category           myCategory("MyCategory");
//  balm::MetricDescription  description(&myCategory, "MyMetric");
//  balm::MetricId           myMetric(&description);
//..
// Now we create a 'balm::IntegerCollector' object for 'myMetric' and use the
// 'update' method to update its collected value:
//..
//  balm::IntegerCollector collector(myMetric);
//
//  collector.update(1);
//  collector.update(3);
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
//      assert(1        == record.min());
//      assert(3        == record.max());
//..

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_METRICID
#include <balm_metricid.h>
#endif

#ifndef INCLUDED_BALM_METRICRECORD
#include <balm_metricrecord.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BDLQQ_LOCKGUARD
#include <bdlqq_lockguard.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace balm {
                       // ===========================
                       // class IntegerCollector
                       // ===========================

class IntegerCollector {
    // This class provides a mechanism for collecting and aggregating the
    // value of an integer metric over a period of time.  The collector
    // contains a 'MetricId' object identifying the metric being
    // collected, the number of times an event occurred, and the total,
    // minimum, and maximum aggregates of the associated measurement value.
    // The default value for the count is 0, the default value for the total
    // is 0, the default value for the minimum is 'DEFAULT_MIN', and the
    // default value for the maximum is 'DEFAULT_MAX'.

    // DATA
    MetricId       d_metricId;  // metric identifier
    int                 d_count;     // aggregated count of events
    bsls::Types::Int64  d_total;     // total of values across events
    int                 d_min;       // minimum value across events
    int                 d_max;       // maximum value across events
    mutable bdlqq::Mutex d_mutex;     // synchronizes access to data

    // NOT IMPLEMENTED
    IntegerCollector(const IntegerCollector&);
    IntegerCollector& operator=(const IntegerCollector&);

  public:
    // PUBLIC CONSTANTS
    static const int DEFAULT_MIN;  // default minimum value (INT_MAX)
    static const int DEFAULT_MAX;  // default maximum value (INT_MIN)

    // CREATORS
    IntegerCollector(const MetricId& metricId);
        // Create an integer collector for a metric having the specified
        // 'metricId', and having an initial count of 0, total of 0, min of
        // 'DEFAULT_MIN', and max of 'DEFAULT_MAX'.

    ~IntegerCollector();
        // Destroy this object.

    // MANIPULATORS
    void reset();
        // Reset the count, total, minimum, and maximum values of the metric
        // being collected to their default states.  After this operation, the
        // count and total values will be 0, the minimum value will be
        // 'DEFAULT_MIN', and the maximum value will be 'DEFAULT_MAX'.

    void loadAndReset(MetricRecord *records);
        // Load into the specified 'record' the id of the metric being
        // collected as well as the current count, total, minimum, and maximum
        // aggregated values for that metric; then reset the count, total,
        // minimum, and maximum values to their default states.  After this
        // operation, the count and total values will be 0, the minimum value
        // will be 'DEFAULT_MIN', and the maximum value will be 'DEFAULT_MAX'.
        // Note that 'DEFAULT_MIN != MetricRecord::DEFAULT_MIN' and
        // 'DEFAULT_MAX != MetricRecord::DEFAULT_MAX'; when populating
        // 'record', this operation will convert default values for minimum
        // and maximum.  A minimum value of 'DEFAULT_MIN' will populate a
        // minimum value of 'MetricRecord::DEFAULT_MIN' and a maximum
        // value of 'DEFAULT_MAX' will populate a maximum value of
        // 'MetricRecord::DEFAULT_MAX'.

    void update(int value);
        // Increment the event count by 1, add the specified 'value' to the
        // total, if 'value' is less than the minimum value, set 'value' to be
        // the minimum value, and if 'value' is greater than the maximum
        // value, set 'value' to be the maximum value.

    void accumulateCountTotalMinMax(int count, int total, int min, int max);
        // Increment the event count by the specified 'count', add the
        // specified 'total' to the accumulated total, if 'min' is less
        // than the minimum value, set 'min' to be the minimum value, and if
        // 'max' is greater than the maximum value, set 'max' to be the
        // maximum value.

    void setCountTotalMinMax(int count, int total, int min, int max);
        // Set the event count to the specied 'count', the total aggregate to
        // the specified 'total', the minimum aggregate to the specified 'min'
        // and the maximum aggregate to the specified 'max'.

    // ACCESSORS
    const MetricId& metricId() const;
         // Return a reference to the non-modifiable 'MetricId' object
         // identifying the metric for which this object collects values.

    void load(MetricRecord *record) const;
        // Load into the specified 'record' the id of the metric being
        // collected, as well as the current count, total, minimum, and
        // maximum aggregated values for the metric.  Note that
        // 'DEFAULT_MIN != MetricRecord::DEFAULT_MIN' and
        // 'DEFAULT_MAX != MetricRecord::DEFAULT_MAX'; when populating
        // 'record', this operation will convert default values for minimum
        // and maximum.  A minimum value of 'DEFAULT_MIN' will populate a
        // minimum value of 'MetricRecord::DEFAULT_MIN' and a maximum
        // value of 'DEFAULT_MAX' will populate a maximum value of
        // 'MetricRecord::DEFAULT_MAX'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ---------------------------
                          // class IntegerCollector
                          // ---------------------------

// CREATORS
inline
IntegerCollector::IntegerCollector(const MetricId& metricId)
: d_metricId(metricId)
, d_count(0)
, d_total(0)
, d_min(DEFAULT_MIN)
, d_max(DEFAULT_MAX)
, d_mutex()
{
}

inline
IntegerCollector::~IntegerCollector()
{
}

// MANIPULATORS
inline
void IntegerCollector::reset()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_count = 0;
    d_total = 0;
    d_min   = DEFAULT_MIN;
    d_max   = DEFAULT_MAX;
}

inline
void IntegerCollector::update(int value)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    ++d_count;
    d_total += value;
    d_min = bsl::min(value, d_min);
    d_max = bsl::max(value, d_max);
}

inline
void IntegerCollector::accumulateCountTotalMinMax(int count,
                                                       int total,
                                                       int min,
                                                       int max)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_count += count;
    d_total += total;
    d_min   = bsl::min(min, d_min);
    d_max   = bsl::max(max, d_max);
}

inline
void IntegerCollector::setCountTotalMinMax(int count,
                                                int total,
                                                int min,
                                                int max)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_count = count;
    d_total = total;
    d_min   = min;
    d_max   = max;
}

// ACCESSORS
inline
const MetricId& IntegerCollector::metricId() const
{
    return d_metricId;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
