// baem_metric.h              -*-C++-*-
#ifndef INCLUDED_BAEM_METRIC
#define INCLUDED_BAEM_METRIC

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_metric.h,v 1.7 2008/04/17 21:22:34 hversche Exp $")

//@PURPOSE: Provide helper classes for recording metric values.
//
//@CLASSES:
//   baem_Metric: container for recording metric values
//
//@SEE_ALSO: baem_metricsmanager, baem_defaultmetricsmanager,
//           baem_integermetric, baem_metrics
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class, 'baem_Metric', to simplify
// the process of collecting metrics.  A metric records the number of times an
// event occurs, as well as an associated measurement value.  This component
// does *not* define what constitutes an event or what the associated value
// represents.  A metric maintains a count of event occurrences and the
// aggregated minimum, maximum, and total of the measured metric-event values.
//
// The 'baem_Metric' class, defined in this component, has in-core value
// semantics.  Each 'baem_Metric' object holds a pointer to a 'baem_Collector'
// that collects values for a particular metric.  The 'baem_Collector' is
// either supplied at construction, or else obtained from a
// 'baem_MetricsManager' object's 'baem_CollectorRepository'.  If the supplied
// 'baem_MetricsManager' is 0, the metric will use the default metrics manager
// instance ('baem_DefaultMetricsManager::instance()'), if initialized;
// otherwise, the metric is placed in the inactive state (i.e.,
// 'isActive()' is 'false') and operations that would otherwise update the
// metric will have no effect.
//
///Choosing between 'baem_Metric' and Macros
///-----------------------------------------
// The 'baem_Metric' class and the macros defined in 'baem_metrics' provide
// the same basic functionality.  Clients may find 'baem_Metric' objects
// better suited to collecting metrics associated with a particular instance
// of a stateful object, while macros are better suited to collecting metrics
// associated with a particular code path (rather than an object instance).
// In most instances, however, choosing between the two is simply a matter of
// taste.
//
///Thread Safety
///-------------
// 'baem_Metric' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Create and Configure the Default 'baem_MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricsManager'
// instance and perform a trivial configuration.
//
// First we create a 'baem_DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') to which the default
// metrics manager will publish metrics.  Note that the default metrics
// manager is intended to be created and destroyed by the *owner* of 'main'.
// An instance of the manager should be created during the initialization of
// an application (while the task has a single thread) and destroyed just
// prior to termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//
//      // ...
//
//      baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default manager object has been created, it can be accessed using
// the 'instance' operation.
//..
//      baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
//      assert(0 != manager);
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// goes out of scope.  Clients that choose to call
// 'baem_DefaultMetricsManager::create()' explicitly must also explicitly call
// 'baem_DefaultMetricsManager::destroy()'.
//
///Example 2 - Metric Collection with 'baem_Metric'
///- - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively we can use a 'baem_Metric' to record metric values.  In this
// third example we implement a hypothetical event manager object, similar in
// purpose to the 'processEvent' function of example 2.  We use 'baem_Metric'
// objects to record metrics for the size of the event data, the elapsed
// processing time, and the number of failures.
//..
//  class EventManager {
//
//      // DATA
//      baem_Metric d_messageSize;
//      baem_Metric d_elapsedTime;
//      baem_Metric d_failedRequests;
//
//    public:
//      // CREATORS
//      EventManager()
//      : d_messageSize("MyCategory", "EventManager/size")
//      , d_elapsedTime("MyCategory", "EventManager/elapsedTime")
//      , d_failedRequests("MyCategory", "EventManager/failedRequests")
//      {}
//
//      // MANIPULATORS
//      int handleEvent(int eventId, const bsl::string& eventMessage)
//          // Process the event described by the specified 'eventId' and
//          // 'eventMessage' .  Return 0 on success, and a non-zero value
//          // if there was an error handling the event.
//      {
//         int returnCode = 0;
//
//         d_requestSize.update(messageSize.size());
//         bsls_Stopwatch stopwatch;
//         stopwatch.start();
//
//         // Process 'data' ('returnCode' may change).
//
//         if (0 != returnCode) {
//             d_failedRequests.increment();
//         }
//
//         d_elapsedTime.update(stopwatch.elapsedTime());
//         return returnCode;
//      }
//
//  // ...
//  };
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTOR
#include <baem_collector.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTORREPOSITORY
#include <baem_collectorrepository.h>
#endif

#ifndef INCLUDED_BAEM_DEFAULTMETRICSMANAGER
#include <baem_defaultmetricsmanager.h>
#endif

#ifndef INCLUDED_BAEM_METRICID
#include <baem_metricid.h>
#endif

#ifndef INCLUDED_BAEM_METRICSMANAGER
#include <baem_metricsmanager.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

namespace BloombergLP {

                         // =================
                         // class baem_Metric
                         // =================

class baem_Metric {
    // This class provides an in-core value semantic type for recording and
    // aggregating the values of a metric.  The value of a 'baem_Metric' object
    // is characterized by the 'baem_Collector' object it uses to collect
    // metric-event values.  Each instance of this class establishes (at
    // construction) an association to a 'baem_Collector' object to which the
    // metric delegates.  A 'baem_Metric' value is constant after
    // construction (i.e., it does not support assignment or provide
    // manipulators that modify its collector value) so that synchronization
    // primitives are not required to protect its data members.  Note that if
    // a collector or metrics manager is not supplied at construction, and if
    // the default metrics manager has not been instantiated, then the metric
    // will be inactive (i.e., 'isActive() == false') and the manipulator
    // methods of the metric object will have no effect

    // DATA
    baem_Collector      *d_collector_p;  // collected metric data (held, not
                                         // owned); may be 0, but cannot be
                                         // invalid

    const volatile bool *d_isEnabled_p;  // is category enabled (held, not
                                         // owned)

    // NOT IMPLEMENTED
    baem_Metric& operator=(baem_Metric& );

    // PRIVATE CONSTANTS
    static const bool NOT_ACTIVE;
        // A static boolean constant for 'false'.  The data member
        // 'd_isEnabled_p' is assigned the *address* of this constant if the
        // 'baem_Metric' object is not supplied a valid collector at
        // construction.

  public:
    // CLASS METHODS
    static baem_Collector *lookupCollector(const char          *category,
                                           const char          *name,
                                           baem_MetricsManager *manager = 0);
        // Return a collector corresponding to the specified metric 'category'
        // and 'name'.  Optionally specify a metrics 'manager' used to provide
        // the collector.  If 'manager' is 0, use the default metrics manager
        // if initialized; if 'manager' is 0 and the default metrics manager
        // has not been initialized, return 0.  The behavior is undefined
        // unless 'category' and 'name' are null-terminated.

    static baem_Collector *lookupCollector(const baem_MetricId&  metricId,
                                           baem_MetricsManager  *manager = 0);
        // Return a collector for the specified 'metricId'.  Optionally specify
        // a metrics 'manager' used to provide the collector.  If 'manager' is
        // 0, use the default metrics manager, if initialized; if 'manager' is
        // 0 and the default metrics manager has not been initialized, return
        // 0.  The behavior is undefined unless 'metricId' is a valid metric
        // id supplied by the 'baem_MetricsRegistry' of the indicated metrics
        // manager.

    // CREATORS
    baem_Metric(const char          *category,
                const char          *name,
                baem_MetricsManager *manager = 0);
        // Create a metric object to collect values for the metric identified
        // by the specified 'category' and 'name'.  Optionally specify a
        // metrics 'manager' used to provide a collector for the indicated
        // metric.  If 'manager' is 0, use the default metrics manager, if
        // initialized; if 'manager' is 0 and the default metrics
        // manager has not been initialized, place this metric object in the
        // inactive state (i.e., 'isActive()' is 'false') in which case
        // instance methods that would otherwise update the metric will have
        // no effect.  The behavior is undefined unless 'category' and 'name'
        // are null-terminated.

    explicit baem_Metric(const baem_MetricId&  metricId,
                         baem_MetricsManager  *manager = 0);
        // Create a metric object to collect values for the specified
        // 'metricId'.  Optionally specify a metrics 'manager' used to provide
        // a collector for 'metricId'.  If 'manager' is 0, use the default
        // metrics manager, if initialized; if 'manager' is 0 and the default
        // metrics manager has not been initialized, place this metric object
        // in the inactive state (i.e., 'isActive()' is 'false') in which case
        // instance methods that would otherwise update the metric will have
        // no effect.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'baem_MetricRepository' object owned by the
        // indicated metrics manager.

    explicit baem_Metric(baem_Collector *collector);
        // Create a metric object to collect values for the metric implied by
        // the specified 'collector' (i.e., 'collector->metricId()').  The
        // behavior is undefined unless 'collector' is a valid address of a
        // 'baem_Collector' object and 'collector' has a valid id (i.e.,
        // 'collector->metricId().isValid()').

    baem_Metric(const baem_Metric& original);
        // Create a metric object that will record values for the same metric
        // (i.e., using the same 'baem_Collector' object) as the specified
        // 'original' metric.  If the 'original' metric is inactive (i.e.,
        // 'isActive()' is 'false'), then this metric will similarly be
        // inactive.

    // ~baem_Metric();
        // Destroy this metric.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    void increment();
        // Increase the count and total of this metric by 1; if 1.0 is less
        // than the current minimum recorded value of the metric, set the new
        // minimum value to be 1.0; if 1.0 is greater than the current maximum
        // recorded value, set the new maximum value to be 1.0.  If, however,
        // this metric is not active (i.e., 'isActive()' is 'false') then this
        // method has no effect.  Note that this method is functionally
        // equivalent to 'update(1)'.

    void update(double value);
        // Increase the event count by 1 and add the specified 'value' to the
        // total recorded value; if 'value' is less than the current minimum
        // recorded value of the metric, set the new minimum value to be
        // 'value'; if 'value' is greater than the current maximum recorded
        // value, set the new maximum value to be 'value'.  If, however, this
        // metric is inactive (i.e., 'isActive()' is 'false'), then this method
        // has no effect.

    void accumulateCountTotalMinMax(int    count,
                                    double total,
                                    double min,
                                    double max);
        // Increase the event count by the specified 'count' and add the
        // specified 'total' to the accumulated total; if the specified 'min'
        // is less than the current minimum recorded value of the metric, set
        // the new minimum value to be 'min'; if the specified 'max' is
        // greater than the current maximum recorded value, set the new
        // maximum value to be 'max'.  If, however, this metric is inactive
        // (i.e., 'isActive()' is 'false'), then this method has no effect.

    baem_Collector *collector();
        // Return the address of the modifiable collector for this metric.

    // ACCESSORS
    const baem_Collector *collector() const;
        // Return the address of the non-modifiable collector for this metric.

    baem_MetricId metricId() const;
        // Return a 'baem_MetricId' object identifying this metric.  If this
        // metric was not supplied a valid collector at construction then the
        // returned id will be invalid (i.e., 'metricId().isValid() == false').

    bool isActive() const;
        // Return 'true' if this metric will actively record metrics, and
        // 'false' otherwise.  If the returned value is 'false', the
        // manipulator operations will have no effect.  A metric will be
        // inactive if either (1) it was not initialized with a valid metric
        // identifier or (2) the associated metric category has been disabled
        // (see the 'baem_MetricsManager' method 'setCategoryEnabled').  Note
        // that invoking this method is logically equivalent to the expression
        // '0 != collector() && metricId().category()->enabled()'.
};

// FREE OPERATORS
inline
bool operator==(const baem_Metric& lhs, const baem_Metric& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metrics have the same
    // value and 'false' otherwise.  Two metrics have the same value if they
    // record measurements using the same collector object or if they both
    // have a null collector (i.e., 'collector()' is 0).

inline
bool operator!=(const baem_Metric& lhs, const baem_Metric& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metrics do not have the
    // same value and 'false' otherwise.  Two metrics do not have the same
    // value if they record measurements using different collector objects or
    // if one, but not both, have a null collector (i.e., 'collector()' is 0).

                      // ==========================
                      // class baem_Metric_MacroImp
                      // ==========================

struct baem_Metric_MacroImp {
    // This structure provides a namespace for functions used to implement the
    // macros defined by this component.
    //
    // This is an implementation type of this component and *must* *not* be
    // used by clients of the 'baem' package.

    // CLASS METHODS
    static void getCollector(baem_Collector      **collector,
                             baem_CategoryHolder  *holder,
                             const char           *category,
                             const char           *metric);
        // Load the specified 'collector' with the address of the default
        // collector (from the default metrics manager) for the metric
        // identified by the specified 'category' and 'name', and register the
        // specified 'holder' for 'category'.   Note that '*collector' must be
        // assigned *before* registering 'holder' to ensure that the macros
        // always have a valid 'collector' when 'holder.enabled()' is 'true'.

    static void getCollector(
                       baem_Collector              **collector,
                       baem_CategoryHolder          *holder,
                       const char                   *category,
                       const char                   *metric,
                       baem_PublicationType::Value   preferredPublicationType);
        // Load the specified 'collector' with the address of the default
        // collector (from the default metrics manager) for the metric
        // identified by the specified 'category' and 'name', register the
        // specified 'holder' for 'category', and set the identified metric's
        // preferred publication type to the specified
        // 'preferredPublicationType'.  Note that '*collector' must be
        // assigned before 'holder' to ensure that the macros always have a
        // valid collector' when 'holder.enabled()' is 'true'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // -----------------
                         // class baem_Metric
                         // -----------------

// CLASS METHODS
inline
baem_Collector *baem_Metric::lookupCollector(const char          *category,
                                             const char          *name,
                                             baem_MetricsManager *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    return manager
         ? manager->collectorRepository().getDefaultCollector(category, name)
         : 0;
}

inline
baem_Collector *baem_Metric::lookupCollector(const baem_MetricId&  metricId,
                                             baem_MetricsManager  *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    return manager
         ? manager->collectorRepository().getDefaultCollector(metricId)
         : 0;
}

// CREATORS
inline
baem_Metric::baem_Metric(const char          *category,
                         const char          *name,
                         baem_MetricsManager *manager)
: d_collector_p(lookupCollector(category, name, manager))
{
    // 'd_collector_p' can be 0, but it *cannot* have an invalid metric id.
    d_isEnabled_p = d_collector_p
                  ? &d_collector_p->metricId().category()->enabled()
                  : &NOT_ACTIVE;
}

inline
baem_Metric::baem_Metric(const baem_MetricId&  metricId,
                         baem_MetricsManager  *manager)
: d_collector_p(lookupCollector(metricId, manager))
{
    // 'd_collector_p' can be 0, but it *cannot* have an invalid metric id.
    d_isEnabled_p = d_collector_p
                  ? &d_collector_p->metricId().category()->enabled()
                  : &NOT_ACTIVE;
}

inline
baem_Metric::baem_Metric(baem_Collector *collector)
: d_collector_p(collector)
, d_isEnabled_p(&collector->metricId().category()->enabled())
{
}

inline
baem_Metric::baem_Metric(const baem_Metric& original)
: d_collector_p(original.d_collector_p)
, d_isEnabled_p(original.d_isEnabled_p)
{
}

// MANIPULATORS
inline
void baem_Metric::increment()
{
    if (*d_isEnabled_p) {
        d_collector_p->update(1.0);
    }
}

inline
void baem_Metric::update(double value)
{
    if (*d_isEnabled_p) {
        d_collector_p->update(value);
    }
}

inline
void baem_Metric::accumulateCountTotalMinMax(int    count,
                                             double total,
                                             double min,
                                             double max)
{
    if (*d_isEnabled_p) {
        d_collector_p->accumulateCountTotalMinMax(count, total, min, max);
    }
}

inline
baem_Collector *baem_Metric::collector()
{
    return d_collector_p;
}

// ACCESSORS
inline
const baem_Collector *baem_Metric::collector() const
{
    return d_collector_p;
}

inline
baem_MetricId baem_Metric::metricId() const
{
    return d_collector_p ? d_collector_p->metricId() : baem_MetricId();
}

inline
bool baem_Metric::isActive() const
{
    return *d_isEnabled_p;
}

// FREE OPERATORS
inline
bool operator==(const baem_Metric& lhs, const baem_Metric& rhs)
{
    return lhs.collector() == rhs.collector();
}

inline
bool operator!=(const baem_Metric& lhs, const baem_Metric& rhs)
{
    return !(lhs == rhs);
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
