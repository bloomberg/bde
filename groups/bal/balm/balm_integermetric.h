// balm_integermetric.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_INTEGERMETRIC
#define INCLUDED_BALM_INTEGERMETRIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide helper classes for recording int metric values.
//
//@CLASSES:
//   balm::IntegerMetric: container for recording integer metric values
//
//@SEE_ALSO: balm_metric, balm_metrics, balm_metricsmanager,
//           balm_defaultmetricsmanager
//
//@DESCRIPTION: This component provides a class, 'balm::IntegerMetric', to
// simplify the process of collecting integer metrics.  A metric records the
// number of times an event occurs, as well as an associated measurement
// value.  This component does *not* define what constitutes an event or what
// the associated value represents.  A metric maintains a count of event
// occurrences and the aggregated minimum, maximum, and total of the measured
// metric-event values.
//
// The 'balm::IntegerMetric' class, defined in this component, has in-core
// value semantics.  Each 'balm::IntegerMetric' object holds a pointer to a
// 'balm::IntegerCollector' that collects values for a particular integer
// metric.  The 'balm::IntegerCollector' is either supplied at construction, or
// else obtained from a 'balm::MetricsManager' object's
// 'balm::CollectorRepository'.  If the supplied 'balm::MetricsManager' is 0,
// the metric will use the default metrics manager instance
// ('balm::DefaultMetricsManager::instance()'), if initialized; otherwise the
// metric is placed in the inactive state (i.e., 'isActive()' is 'false') and
// operations that would otherwise update the metric will have no effect.
//
///Choosing Between 'balm::IntegerMetric' and Macros
///-------------------------------------------------
// The 'balm::IntegerMetric' class and the macros defined in 'balm_metrics'
// provide the same basic functionality.  Clients may find
// 'balm::IntegerMetric' objects better suited to collecting integer metrics
// associated with a particular instance of a stateful object, while macros are
// better suited to collecting metrics associated with a particular code path
// (rather than an object instance).  In most instances choosing is a matter of
// taste.
//
///Thread Safety
///-------------
// 'balm::IntegerMetric' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
// In addition all the macros defined in this component are fully
// *thread-safe*, meaning that they can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1: Metric Collection With 'balm::IntegerMetric'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can use 'balm::IntegerMetric' objects to record metric values.  In this
// example we implement a hypothetical event manager object.  We use
// 'balm::IntegerMetric' objects to record metrics for the size of the request,
// the elapsed processing time, and the number of failures.
//..
//  class EventManager {
//
//      // DATA
//      balm::IntegerMetric d_messageSize;
//      balm::IntegerMetric d_elapsedTime;
//      balm::IntegerMetric d_failedRequests;
//
//    public:
//
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
//         d_messageSize.update(eventMessage.size());
//
//         bsls::TimeInterval start = bdlt::CurrentTime::now();
//
//         // Process 'data' ('returnCode' may change).
//
//         if (0 != returnCode) {
//             d_failedRequests.increment();
//         }
//
//         bsls::TimeInterval end = bdlt::CurrentTime::now();
//         d_elapsedTime.update((end - start).totalMicroseconds());
//         return returnCode;
//      }
//
//  // ...
//  };
//..
//
///Example 2: Create and Access the Default 'balm::MetricsManager' Instance
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and perform a trivial configuration.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') that it will publish
// metrics to.  Note that the default metrics manager is intended to be created
// and destroyed by the *owner* of 'main'.  An instance of the manager should
// be created during the initialization of an application (while the task has a
// single thread) and destroyed just prior to termination (when there is
// similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//
//      balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation.
//..
//      balm::MetricsManager *manager =
//                                     balm::DefaultMetricsManager::instance();
//      assert(0 != manager);
//..
// Note that the default metrics manager will be released when 'managerGuard'
// exits this scoped and is destroyed.  Clients that choose to explicitly call
// 'balm::DefaultMetricsManager::create' must also explicitly call
// 'balm::DefaultMetricsManager::release()'.
//
// Now that we have created a 'balm::MetricsManager' instance, we can use the
// instance to publish metrics collected using the event manager described in
// {Example 1}:
//..
//      EventManager eventManager;
//
//      eventManager.handleEvent(0, "ab");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abdef");
//
//      manager->publishAll();
//
//      eventManager.handleEvent(0, "ab");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abdef");
//
//      eventManager.handleEvent(0, "a");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abc");
//      eventManager.handleEvent(0, "abdefg");
//
//      manager->publishAll();
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_COLLECTORREPOSITORY
#include <balm_collectorrepository.h>
#endif

#ifndef INCLUDED_BALM_DEFAULTMETRICSMANAGER
#include <balm_defaultmetricsmanager.h>
#endif

#ifndef INCLUDED_BALM_INTEGERCOLLECTOR
#include <balm_integercollector.h>
#endif

#ifndef INCLUDED_BALM_METRICID
#include <balm_metricid.h>
#endif

#ifndef INCLUDED_BALM_METRICSMANAGER
#include <balm_metricsmanager.h>
#endif

#ifndef INCLUDED_BALM_PUBLICATIONTYPE
#include <balm_publicationtype.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {

namespace balm {
                            // ===================
                            // class IntegerMetric
                            // ===================

class IntegerMetric {
    // This class provides an in-core value semantic type for recording and
    // aggregating the values of an integer metric.  The value of a
    // 'IntegerMetric' object is characterized by the 'IntegerCollector' object
    // it uses to collect metric-event values.  Each instance of this class
    // establishes (at construction) an association to an 'IntegerCollector'
    // object to which the metric delegates.  A 'IntegerMetric' value is
    // constant after construction (i.e., it does not support assignment or
    // provide manipulators that modify its collector value), so that
    // synchronization primitives are not required to protect its data members.
    // Note that if a collector or metrics manager is not supplied at
    // construction, and if the default metrics manager has not been
    // instantiated, then the metric will be inactive (i.e., 'isActive()' is
    // 'false') and the manipulator methods of the integer metric object will
    // have no effect.

    // DATA
    IntegerCollector      *d_collector_p;  // collected metric data (held, not
                                           // owned); may be 0, but cannot be
                                           // invalid

    const bsls::AtomicInt *d_isEnabled_p;  // memo for isActive()

    // NOT IMPLEMENTED
    IntegerMetric& operator=(const IntegerMetric& );

  public:
    // CLASS METHODS
    static IntegerCollector *lookupCollector(const char     *category,
                                             const char     *name,
                                             MetricsManager *manager = 0);
        // Return an integer collector corresponding to the specified metric
        // 'category' and 'name'.  Optionally specify a metrics 'manager' used
        // to provide the collector.  If 'manager' is 0, use the default
        // metrics manager if initialized; if 'manager' is 0 and the default
        // metrics manager has not been initialized, return 0.  The behavior
        // is undefined unless 'category' and 'name' are null-terminated.

    static IntegerCollector *lookupCollector(const MetricId&  metricId,
                                             MetricsManager  *manager = 0);
        // Return an integer collector for the specified 'metricId'.
        // Optionally specify a metrics 'manager' used to provide the
        // collector.  If 'manager' is 0, use the default metrics manager, if
        // initialized; if 'manager' is 0 and the default metrics manager has
        // not been initialized, return 0.  The behavior is undefined unless
        // 'metricId' is a valid metric id supplied by the 'MetricsRegistry'
        // of the indicated metrics manager.

    // CREATORS
    IntegerMetric(const char     *category,
                  const char     *name,
                  MetricsManager *manager = 0);
        // Create an integer metric object to collect values for the metric
        // identified by the specified 'category' and 'name'.  Optionally
        // specify a metrics 'manager' used to provide a collector for the
        // indicated metric.  If 'manager' is 0, use the default metrics
        // manager, if initialized; if 'manager' is 0 and the default metrics
        // manager has not been initialized, place this metric object in the
        // inactive state (i.e., 'isActive()' is 'false') in which case
        // instance methods that would otherwise update the metric will have no
        // effect.  The behavior is undefined unless 'category' and 'name' are
        // null-terminated.

    explicit IntegerMetric(const MetricId&  metricId,
                           MetricsManager  *manager = 0);
        // Create an integer metric object to collect values for the specified
        // 'metricId'.  Optionally specify a metrics 'manager' used to provide
        // a collector for 'metricId'.  If 'manager' is 0, use the default
        // metrics manager, if initialized; if 'manager' is 0 and the default
        // metrics manager has not been initialized, place this metric object
        // in the inactive state (i.e., 'isActive()' is 'false') in which case
        // instance methods that would otherwise update the metric will have
        // no effect.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'MetricRepository' object owned by the indicated
        // metrics manager.

    explicit IntegerMetric(IntegerCollector *collector);
        // Create an integer metric object to collect values for the metric
        // implied by the specified 'collector' (i.e.,
        // 'collector->metricId()').  The behavior is undefined unless
        // 'collector' is a valid address of a 'IntegerCollector' object and
        // the collector object supplied has a valid id (i.e.,
        // 'collector->metricId().isValid()').

    IntegerMetric(const IntegerMetric& original);
        // Create an integer metric object that will record values for the same
        // metric (i.e., using the same 'IntegerCollector' object) as the
        // specified 'original' integer metric.  If the 'original' metric is
        // inactive (i.e., 'isActive() == false'), then this metric will
        // similarly be inactive.

    // ~IntegerMetric();
        // Destroy this integer metric.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void increment();
        // Increase the count and total of this integer metric by 1; if 1 is
        // less than the current minimum recorded value of the metric, set the
        // new minimum value to be 1; if 1 is greater than the current maximum
        // recorded value, set the new maximum value to be 1.  If, however,
        // this integer metric is not active (i.e., 'isActive()' is 'false'),
        // then this method has no effect.  Note that this method is
        // functionally equivalent to 'update(1).'

    void update(int value);
        // Increase the event count by 1 and add the specified 'value' to the
        // total recorded value; if 'value' is less than the current minimum
        // recorded value of the metric, set the new minimum value to be
        // 'value'; if 'value' is greater than the current maximum recorded
        // value, set the new maximum value to be 'value'.  If, however, this
        // integer metric is inactive (i.e., 'isActive()' is 'false'), then
        // this method has no effect.

    void accumulateCountTotalMinMax(int count, int total, int min, int max);
        // Increase the event count by the specified 'count' and add the
        // specified 'total' to the accumulated total; if the specified 'min'
        // is less than the current minimum recorded value of the metric, set
        // the new minimum value to be 'min'; if the specified 'max' is
        // greater than the current maximum recorded value, set the new
        // maximum value to be 'max'.  If, however, this integer metric is
        // inactive (i.e., 'isActive()' is 'false'), then this method has no
        // effect.

    IntegerCollector *collector();
        // Return the address of the modifiable integer collector for this
        // integer metric.

    // ACCESSORS
    const IntegerCollector *collector() const;
        // Return the address of the non-modifiable integer collector for this
        // integer metric.

    MetricId metricId() const;
        // Return a 'MetricId' object identifying this integer metric.  If
        // this metric was not supplied a valid integer collector at
        // construction then the returned id will be invalid (i.e.,
        // 'metricId().isValid()' is 'false').

    bool isActive() const;
        // Return 'true' if this integer metric will actively record metrics,
        // and 'false' otherwise.  If the returned value is 'false', the
        // manipulator operations will have no effect.  An integer metric will
        // be inactive if either (1) it was not initialized with a valid metric
        // identifier or (2) the associated metric category has been disabled
        // (see the 'MetricsManager' method 'setCategoryEnabled').  Note that
        // invoking this method is logically equivalent to the expression
        // '0 != collector() && metricId().category()->enabled()'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bool operator==(const IntegerMetric& lhs, const IntegerMetric& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' integer metrics have the
    // same value and 'false' otherwise.  Two integer metrics have the same
    // value if they record measurements using the same integer collector
    // object or if they both have null collectors (i.e., 'collector()' is 0).

inline
bool operator!=(const IntegerMetric& lhs, const IntegerMetric& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' integer metrics do not
    // have the same value and 'false' otherwise.  Two integer metrics do not
    // have the same value if they record measurements using different integer
    // collector objects or if one, but not both, have a null collector (i.e.,
    // 'collector()' is 0).

                        // ============================
                        // class IntegerMetric_MacroImp
                        // ============================

struct IntegerMetric_MacroImp {
    // This structure provides a namespace for functions used to implement the
    // macros defined by this component.
    //
    // This is an implementation type of this component and *must* *not* be
    // used by clients of the 'balm' package.

    // CLASS METHODS
    static void getCollector(IntegerCollector **collector,
                             CategoryHolder    *holder,
                             const char        *category,
                             const char        *metric);
        // Load the specified 'collector' with the address of the default
        // integer collector (from the default metrics manager) for the
        // specified 'metric' identified by the specified 'category' and
        // 'name', and register the specified 'holder' for 'category'.   Note
        // that '*collector' must be assigned *before* registering 'holder' to
        // ensure that the macros always have a valid 'collector' when
        // 'holder->enabled()' is 'true'.

    static void getCollector(
                       IntegerCollector       **collector,
                       CategoryHolder          *holder,
                       const char              *category,
                       const char              *metric,
                       PublicationType::Value   preferredPublicationType);
        // Load the specified 'collector' with the address of the default
        // integer collector (from the default metrics manager) for the
        // specified 'metric' identified by the specified 'category' and
        // 'name', register the specified 'holder' for 'category', and set the
        // identified metric's preferred publication type to the specified
        // 'preferredPublicationType'.  Note that '*collector' must be
        // assigned before 'holder' to ensure that the macros always have a
        // valid 'collector' when 'holder->enabled()' is 'true'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // -------------------
                            // class IntegerMetric
                            // -------------------

// CLASS METHODS
inline
IntegerCollector *IntegerMetric::lookupCollector(const char     *category,
                                                 const char     *name,
                                                 MetricsManager *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    return manager
         ? manager->
               collectorRepository().getDefaultIntegerCollector(category, name)
         : 0;
}

inline
IntegerCollector *IntegerMetric::lookupCollector(const MetricId&  metricId,
                                                 MetricsManager  *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    return manager
         ? manager->collectorRepository().getDefaultIntegerCollector(metricId)
         : 0;
}

// CREATORS
inline
IntegerMetric::IntegerMetric(const char     *category,
                             const char     *name,
                             MetricsManager *manager)
: d_collector_p(lookupCollector(category, name, manager))
{
    d_isEnabled_p = (d_collector_p
                  ? &d_collector_p->metricId().category()->isEnabledRaw() : 0);
}

inline
IntegerMetric::IntegerMetric(const MetricId&  metricId,
                             MetricsManager  *manager)
: d_collector_p(lookupCollector(metricId, manager))
{
    d_isEnabled_p = (d_collector_p
                  ? &d_collector_p->metricId().category()->isEnabledRaw() : 0);
}

inline
IntegerMetric::IntegerMetric(IntegerCollector *collector)
: d_collector_p(collector)
{
    d_isEnabled_p = &d_collector_p->metricId().category()->isEnabledRaw();
}

inline
IntegerMetric::IntegerMetric(const IntegerMetric& original)
: d_collector_p(original.d_collector_p)
, d_isEnabled_p(original.d_isEnabled_p)
{
}

// MANIPULATORS
inline
void IntegerMetric::increment()
{
    if (this->isActive()) {
        d_collector_p->update(1);
    }
}

inline
void IntegerMetric::update(int value)
{
    if (this->isActive()) {
        d_collector_p->update(value);
    }
}

inline
void IntegerMetric::accumulateCountTotalMinMax(int count,
                                               int total,
                                               int min,
                                               int max)
{
    if (this->isActive()) {
        d_collector_p->accumulateCountTotalMinMax(count, total, min, max);
    }
}

inline
IntegerCollector *IntegerMetric::collector()
{
    return d_collector_p;
}

// ACCESSORS
inline
const IntegerCollector *IntegerMetric::collector() const
{
    return d_collector_p;
}

inline
MetricId IntegerMetric::metricId() const
{
    return d_collector_p ? d_collector_p->metricId() : MetricId();
}

inline
bool IntegerMetric::isActive() const
{
    return d_isEnabled_p && d_isEnabled_p->loadRelaxed();
}

}  // close package namespace

// FREE OPERATORS
inline
bool balm::operator==(const IntegerMetric& lhs, const IntegerMetric& rhs)
{
    return lhs.collector() == rhs.collector();
}

inline
bool balm::operator!=(const IntegerMetric& lhs, const IntegerMetric& rhs)
{
    return !(lhs == rhs);
}

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
