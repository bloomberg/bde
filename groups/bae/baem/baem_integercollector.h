// baem_integercollector.h              -*-C++-*-
#ifndef INCLUDED_BAEM_INTEGERCOLLECTOR
#define INCLUDED_BAEM_INTEGERCOLLECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for collecting integral metric values.
//
//@CLASSES:
//   baem_IntegerCollector: a container for collecting integral values
//
//@SEE_ALSO:
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class for collecting and aggregating
// the values of an integral metric.  The 'baem_IntegerCollector' records the
// number of times an event occurs as well as an associated integral
// measurement value.  This component does *not* define what constitutes an
// event or what the value measures.  The collector manages, in a fully
// thread-safe manner, the count of event occurrences and the aggregated
// minimum, maximum, and total of the measured metric value.  This collector
// class provides operations to update the aggregated value, a 'load' method
// to populate a 'baem_MetricRecord' with the current state of the collector,
// a 'reset' operator to reset the current state of the integer collector,
// and finally a combined 'loadAndReset' method that performs both a load and a
// reset in a single (atomic) operation.
//
///Thread Safety
///-------------
// 'baem_IntegerCollector' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following example creates a 'baem_IntegerCollector', modifies its
// values, then collects a 'baem_MetricRecord'.
//
// We start by creating a 'baem_MetricId' object by hand, but in practice,
// an id should be obtained from a 'baem_MetricRegistry' object (such as the
// one owned by a 'baem_MetricsManager'):
//..
//  baem_Category           myCategory("MyCategory");
//  baem_MetricDescription  description(&myCategory, "MyMetric");
//  baem_MetricId           myMetric(&description);
//..
// Now we create a 'baem_IntegerCollector' object for 'myMetric' and use the
// 'update' method to update its collected value:
//..
//  baem_IntegerCollector collector(myMetric);
//
//  collector.update(1);
//  collector.update(3);
//..
// The collector accumulated the values 1 and 3.  The result should have a
// count of 2, a total of 4 (3 + 1), a max of 3 (max(3, 1)), and a min of 1
// (min(3, 1)).
//..
//  baem_MetricRecord record;
//  collector.loadAndReset(&record);
//
//      assert(myMetric == record.metricId());
//      assert(2        == record.count());
//      assert(4        == record.total());
//      assert(1        == record.min());
//      assert(3        == record.max());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_METRICID
#include <baem_metricid.h>
#endif

#ifndef INCLUDED_BAEM_METRICRECORD
#include <baem_metricrecord.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                       // ===========================
                       // class baem_IntegerCollector
                       // ===========================

class baem_IntegerCollector {
    // This class provides a mechanism for collecting and aggregating the
    // value of an integer metric over a period of time.  The collector
    // contains a 'baem_MetricId' object identifying the metric being
    // collected, the number of times an event occurred, and the total,
    // minimum, and maximum aggregates of the associated measurement value.
    // The default value for the count is 0, the default value for the total
    // is 0, the default value for the minimum is 'DEFAULT_MIN', and the
    // default value for the maximum is 'DEFAULT_MAX'.

    // DATA
    baem_MetricId            d_metricId;  // metric identifier
    int                      d_count;     // aggregated count of events
    bsls_PlatformUtil::Int64 d_total;     // total of values across events
    int                      d_min;       // minimum value across events
    int                      d_max;       // maximum value across events
    mutable bcemt_Mutex      d_mutex;     // synchronizes access to data

    // NOT IMPLEMENTED
    baem_IntegerCollector(const baem_IntegerCollector&);
    baem_IntegerCollector& operator=(const baem_IntegerCollector&);

  public:
    // PUBLIC CONSTANTS
    static const int DEFAULT_MIN;  // default minimum value (INT_MAX)
    static const int DEFAULT_MAX;  // default maximum value (INT_MIN)

    // CREATORS
    baem_IntegerCollector(const baem_MetricId& metricId);
        // Create an integer collector for a metric having the specified
        // 'metricId', and having an initial count of 0, total of 0, min of
        // 'DEFAULT_MIN', and max of 'DEFAULT_MAX'.

    ~baem_IntegerCollector();
        // Destroy this object.

    // MANIPULATORS
    void reset();
        // Reset the count, total, minimum, and maximum values of the metric
        // being collected to their default states.  After this operation, the
        // count and total values will be 0, the minimum value will be
        // 'DEFAULT_MIN', and the maximum value will be 'DEFAULT_MAX'.

    void loadAndReset(baem_MetricRecord *records);
        // Load into the specified 'record' the id of the metric being
        // collected as well as the current count, total, minimum, and maximum
        // aggregated values for that metric; then reset the count, total,
        // minimum, and maximum values to their default states.  After this
        // operation, the count and total values will be 0, the minimum value
        // will be 'DEFAULT_MIN', and the maximum value will be 'DEFAULT_MAX'.
        // Note that 'DEFAULT_MIN != baem_MetricRecord::DEFAULT_MIN' and
        // 'DEFAULT_MAX != baem_MetricRecord::DEFAULT_MAX'; when populating
        // 'record', this operation will convert default values for minimum
        // and maximum.  A minimum value of 'DEFAULT_MIN' will populate a
        // minimum value of 'baem_MetricRecord::DEFAULT_MIN' and a maximum
        // value of 'DEFAULT_MAX' will populate a maximum value of
        // 'baem_MetricRecord::DEFAULT_MAX'.

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
    const baem_MetricId& metricId() const;
         // Return a reference to the non-modifiable 'baem_MetricId' object
         // identifying the metric for which this object collects values.

    void load(baem_MetricRecord *record) const;
        // Load into the specified 'record' the id of the metric being
        // collected, as well as the current count, total, minimum, and
        // maximum aggregated values for the metric.  Note that
        // 'DEFAULT_MIN != baem_MetricRecord::DEFAULT_MIN' and
        // 'DEFAULT_MAX != baem_MetricRecord::DEFAULT_MAX'; when populating
        // 'record', this operation will convert default values for minimum
        // and maximum.  A minimum value of 'DEFAULT_MIN' will populate a
        // minimum value of 'baem_MetricRecord::DEFAULT_MIN' and a maximum
        // value of 'DEFAULT_MAX' will populate a maximum value of
        // 'baem_MetricRecord::DEFAULT_MAX'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ---------------------------
                          // class baem_IntegerCollector
                          // ---------------------------

// CREATORS
inline
baem_IntegerCollector::baem_IntegerCollector(const baem_MetricId& metricId)
: d_metricId(metricId)
, d_count(0)
, d_total(0)
, d_min(DEFAULT_MIN)
, d_max(DEFAULT_MAX)
, d_mutex()
{
}

inline
baem_IntegerCollector::~baem_IntegerCollector()
{
}

// MANIPULATORS
inline
void baem_IntegerCollector::reset()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_count = 0;
    d_total = 0;
    d_min   = DEFAULT_MIN;
    d_max   = DEFAULT_MAX;
}

inline
void baem_IntegerCollector::update(int value)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    ++d_count;
    d_total += value;
    d_min = bsl::min(value, d_min);
    d_max = bsl::max(value, d_max);
}

inline
void baem_IntegerCollector::accumulateCountTotalMinMax(int count,
                                                       int total,
                                                       int min,
                                                       int max)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_count += count;
    d_total += total;
    d_min   = bsl::min(min, d_min);
    d_max   = bsl::max(max, d_max);
}

inline
void baem_IntegerCollector::setCountTotalMinMax(int count,
                                                int total,
                                                int min,
                                                int max)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_count = count;
    d_total = total;
    d_min   = min;
    d_max   = max;
}

// ACCESSORS
inline
const baem_MetricId& baem_IntegerCollector::metricId() const
{
    return d_metricId;
}

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
