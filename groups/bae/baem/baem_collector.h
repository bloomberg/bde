// baem_collector.h              -*-C++-*-
#ifndef INCLUDED_BAEM_COLLECTOR
#define INCLUDED_BAEM_COLLECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_collector.h,v 1.7 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide a container for collecting and aggregating metric values.
//
//@CLASSES:
//   baem_Collector: a container for collecting and aggregating metric values
//
//@SEE_ALSO: baem_collectorrepository, baem_metric
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class for collecting and aggregating
// the values of a metric.  The collector records the number of times an event
// occurs as well as an associated measurement value.  This component does
// *not* define what constitutes an event or what the value measures.  The
// collector manages, in a thread-safe manner, the count of event occurrences
// and the aggregated minimum, maximum, and total of the measured metric
// value.  This collector class provides operations to update the aggregated
// value, a 'load' operator to populate a 'baem_MetricRecord' with
// the current state of the collector, a 'reset' method to reset the
// current state of the collector, and finally a combined 'loadAndReset' method
// that performs both a load and a reset as an atomic operation.  Note that
// in practice, most clients should not need to access a 'baem_Collector'
// directly, but instead use it through another type (see 'baem_metric').
//
///Thread Safety
///-------------
// 'baem_Collector' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following example creates a 'baem_Collector', modifies its values, then
// collects a 'baem_MetricRecord'.
//
// We start by creating a 'baem_MetricId' object by hand, but in
// practice, an id should be obtained from a 'baem_MetricRegistry' object
// (such as the one owned by a 'baem_MetricsManager'):
//..
//  baem_Category           myCategory("MyCategory");
//  baem_MetricDescription  description(&myCategory, "MyMetric");
//  baem_MetricId           myMetric(&description);
//..
// Now we create a 'baem_Collector' object for 'myMetric' and use the 'update'
// method to update its collected value:
//..
//  baem_Collector collector(myMetric);
//
//  collector.update(1.0);
//  collector.update(3.0);
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
//      assert(1.0      == record.min());
//      assert(3.0      == record.max());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
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

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

namespace BloombergLP {

class baem_MetricId;

                      // ====================
                      // class baem_Collector
                      // ====================

class baem_Collector {
    // This class provides a mechanism for collecting and aggregating the
    // value of a metric over a period of time.  The collector contains a
    // 'baem_MetricRecord' object that holds the identity of the metric being
    // collected, the number of times an event occurred, and the total,
    // minimum, and maximum aggregates of the associated measurement value.
    // The default value for the count is 0, the default value for the total
    // is 0.0, the default minimum value is 'baem_MetricRecord::DEFAULT_MIN',
    // and the default maximum value is 'baem_MetricRecord::DEFAULT_MAX'.

    // DATA
    baem_MetricRecord   d_record;  // the recorded metric information
    mutable bcemt_Mutex d_lock;    // record synchronization mechanism

    // NOT IMPLEMENTED
    baem_Collector(const baem_Collector&);
    baem_Collector& operator=(const baem_Collector&);

  public:
     // CREATORS
    baem_Collector(const baem_MetricId& metricId);
        // Create a collector for a metric having the specified 'metricId',
        // and having an initial count of 0, total of 0.0, min of
        // 'baem_MetricRecord::DEFAULT_MIN', and max of
        // 'baem_MetricRecord::DEFAULT_MAX'.

    ~baem_Collector();
        // Destroy this object.

    // MANIPULATORS
    void reset();
        // Reset the count, total, minimum, and maximum values of the metric
        // being collected to their default states.  After this operation, the
        // count and total values will be 0, the minimum value will be
        // 'baem_MetricRecord::DEFAULT_MIN', and the maximum value will be
        // 'baem_MetricRecord::DEFAULT_MAX'.

    void loadAndReset(baem_MetricRecord *record);
        // Load into the specified 'record' the id of the metric being
        // collected as well as the current count, total, minimum, and maximum
        // aggregated values for that metric; then reset the count, total,
        // minimum, and maximum values to their default states.  After this
        // operation, the count and total values will be 0, the minimum value
        // will be 'baem_MetricRecord::DEFAULT_MIN', and the maximum value
        // will be 'baem_MetricRecord::DEFAULT_MAX'.  Note that this operation
        // is logically equivalent to calling the 'load' and then the 'reset'
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
        // specified 'total' to the accumulated total, if 'min' is less
        // than the minimum value, set 'min' to be the minimum value, and if
        // 'max' is greater than the maximum value, set 'max' to be the
        // maximum value.

    void setCountTotalMinMax(int count, double total, double min, double max);
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
        // maximum aggregated values for the metric.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // --------------------
                          // class baem_Collector
                          // --------------------

// CREATORS
inline
baem_Collector::baem_Collector(const baem_MetricId& metricId)
: d_record(metricId)
, d_lock()
{
}

inline
baem_Collector::~baem_Collector()
{
}

// MANIPULATORS
inline
void baem_Collector::reset()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_record.count() = 0;
    d_record.total() = 0.0;
    d_record.min()   = baem_MetricRecord::DEFAULT_MIN;
    d_record.max()   = baem_MetricRecord::DEFAULT_MAX;
}

inline
void baem_Collector::loadAndReset(baem_MetricRecord *record)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    *record          = d_record;
    d_record.count() = 0;
    d_record.total() = 0.0;
    d_record.min()   = baem_MetricRecord::DEFAULT_MIN;
    d_record.max()   = baem_MetricRecord::DEFAULT_MAX;
}

inline
void baem_Collector::update(double value)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    ++d_record.count();
    d_record.total() += value;
    d_record.min()   =  bsl::min(d_record.min(), value);
    d_record.max()   =  bsl::max(d_record.max(), value);
}

inline
void baem_Collector::accumulateCountTotalMinMax(int    count,
                                                double total,
                                                double min,
                                                double max)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_record.count() += count;
    d_record.total() += total;
    d_record.min()   =  bsl::min(d_record.min(), min);
    d_record.max()   =  bsl::max(d_record.max(), max);
}

inline
void baem_Collector::setCountTotalMinMax(int    count,
                                         double total,
                                         double min,
                                         double max)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_record.count() = count;
    d_record.total() = total;
    d_record.min()   = min;
    d_record.max()   = max;
}

// ACCESSORS
inline
const baem_MetricId& baem_Collector::metricId() const
{
    return d_record.metricId();
}

inline
void baem_Collector::load(baem_MetricRecord *record) const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    *record = d_record;
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
