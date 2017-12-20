// balm_metrics.h                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_METRICS
#define INCLUDED_BALM_METRICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of operations for recording metric values.
//
//@CLASSES:
//
//@SEE_ALSO: balm_collector, balm_integercollector, balm_defaultmetricsmanager
//
//@DESCRIPTION: This component provides a suite of macros to simplify the
// process of collecting metrics.  A metric records the number of times an
// event occurs, as well as an associated measurement value.  A metric
// maintains a count of event occurrences and the aggregated minimum, maximum,
// and total of the measured metric-event values.  Note that this component
// does *not* define what constitutes an event nor what the associated value
// represents.
//
///Thread Safety
///-------------
// All the macros defined in this component are *thread-safe*, meaning that
// they can be safely invoked simultaneously from multiple threads.  It is
// *not* safe, however, to invoke any of the macros defined in this component
// while the default metrics manager is being either created or destroyed.
//
///Macro Summary
///-------------
// This section provides a brief description of the macros defined in this
// component.  In most cases, category and metric names supplied as macro
// arguments are required to be *runtime* *constants*, meaning that their
// values must be constant for the lifetime of the process.  More complete
// documentation is provided in the 'Macro Reference' section.
//..
//   BALM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
//       This macro behaves like an 'if' clause, executing the subsequent
//       (compound) statement if the specified 'CATEGORY' is enabled.
//
//   BALM_METRICS_UPDATE(CATEGORY, METRIC1, VALUE1)
//   BALM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BALM_METRICS_UPDATE3(CATEGORY, METRIC1, VALUE1, ..., METRIC3, VALUE3)
//   BALM_METRICS_UPDATE4(CATEGORY, METRIC1, VALUE1, ..., METRIC4, VALUE4)
//   BALM_METRICS_UPDATE5(CATEGORY, METRIC1, VALUE1, ..., METRIC5, VALUE5)
//   BALM_METRICS_UPDATE6(CATEGORY, METRIC1, VALUE1, ..., METRIC6, VALUE6)
//   BALM_METRICS_INT_UPDATE(CATEGORY, METRIC1, VALUE1)
//   BALM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BALM_METRICS_INT_UPDATE3(CATEGORY, METRIC1, VALUE1, ..., METRIC3, VALUE3)
//   BALM_METRICS_INT_UPDATE4(CATEGORY, METRIC1, VALUE1, ..., METRIC4, VALUE4)
//   BALM_METRICS_INT_UPDATE5(CATEGORY, METRIC1, VALUE1, ..., METRIC5, VALUE5)
//   BALM_METRICS_INT_UPDATE6(CATEGORY, METRIC1, VALUE1, ..., METRIC6, VALUE6)
//       Update each of up to 6 metrics by the corresponding values.
//       The supplied category and metric names must be *runtime* *constants*.
//
//   BALM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)
//   BALM_METRICS_TYPED_INT_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)
//       Update the identified metric by 'VALUE' and set its preferred
//       publication type.  'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.
//
//   BALM_METRICS_INCREMENT(CATEGORY, METRIC)
//       Increment (by 1) the identified metric.  'CATEGORY' and 'METRIC' must
//       be *runtime* *constants*.
//
//   BALM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)
//       Increment (by 1) the identified metric and set the metric's preferred
//       publication type.  'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.
//
//   BALM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE)
//   BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the identified metric by 'VALUE'.  This operation performs a
//       lookup on 'CATEGORY' and 'METRIC' on each invocation, so those values
//       need *not* be runtime constants.
//
//   BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)
//       Increment (by 1) the identified metric.  This operation performs a
//       lookup on 'CATEGORY' and 'METRIC' on each invocation, so those values
//       need *not* be runtime constants.
//
//   BALM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//   BALM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//   BALM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//   BALM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//   BALM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       Update the identified metric by the elapsed (wall) time, in the
//       indicated units, from the instantiation point of the macro to the end
//       of the enclosing lexical scope.  'CATEGORY' and 'METRIC' must
//       be *runtime* *constants*.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       Update the identified metric by the elapsed (wall) time, in the
//       indicated units, from the instantiation point of the macro to the end
//       of the enclosing lexical scope.  This operation performs a lookup on
//       'CATEGORY' and 'METRIC' on each invocation, so those values need *not*
//       be runtime constants.
//..
//
///Macro Reference
///---------------
// The macros defined in this component make use of the default instance of
// 'balm::MetricsManager'.  The macros have no effect unless the metrics
// manager default instance has been initialized via a call to
// 'balm::DefaultMetricsManager::create'.
//
// The macros defined below provide two basic operations identified by their
// suffixes: update ('*_UPDATE') and increment ('*_INCREMENT').  The update
// operation increments (by 1) the identified metric's count of events,
// increases the metric's total by the supplied value, if the supplied value
// is less than the metric's current minimum value then the minimum value is
// set to the supplied value, and if the supplied value is greater than the
// metric's current maximum value then the maximum value is set to the supplied
// value.  The increment operation is logically equivalent to an update of 1.
//
// The update, increment, and timing operations come in two variations:
// standard, and dynamic (*_DYNAMIC_*).  The standard variation maintains a
// (function-scope static) cache containing the identity of the metric being
// collected.  This cache is initialized the *first* time the flow of control
// passes through the instantiated macro, which in practice means that the
// identifiers for the metric(s) passed as parameters to that macro must be
// *runtime* *constants*.  The dynamic variant looks up the supplied category
// and metric(s) on each invocation of the macro, resulting in additional
// runtime overhead compared to the standard variant.
//
// The following are the standard (non-dynamic) macros provided by this
// component for updating a metric's value:
//..
//   BALM_METRICS_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, with the specified 'VALUE'.  'CATEGORY' and
//       'METRIC' must be null-terminated strings of a type convertible to
//       'const char *', and 'VALUE' is assumed to be of a type convertible to
//       'double'.  This macro maintains a (function-scope static) cache
//       containing the identity of the metric being updated.  This cache is
//       initialized using the 'CATEGORY' and 'METRIC' specified on the *first*
//       application of this macro at a particular instantiation point;
//       subsequent applications use that cached information, which in
//       practice means that 'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.  If the default metrics manager has not been
//       initialized, or if the indicated 'CATEGORY' is currently disabled,
//       this macro has no effect.
//
//   BALM_METRICS_INT_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, with the specified *integer* 'VALUE'.  'CATEGORY'
//       and 'METRIC' must be null-terminated strings of a type convertible to
//       'const char *', and 'VALUE' is assumed to be of a type convertible to
//       'int'.  This macro maintains a (function-scope static) cache
//       containing the identity of the metric being updated.  This cache is
//       initialized using the 'CATEGORY' and 'METRIC' specified on the *first*
//       application of this macro at a particular instantiation point;
//       subsequent applications use that cached information, which in
//       practice, means that 'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.  If the default metrics manager has not been initialized,
//       or if the indicated 'CATEGORY' is currently disabled, this macro has
//       no effect.
//
//   BALM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BALM_METRICS_UPDATEn(CATEGORY, METRIC1, VALUE1, ..., METRICn, VALUEn)
//                                                            Where 2 <= n <= 6
//       Update each of the indicated metrics, identified by the specified
//       'CATEGORY' name and the respective metric names 'METRIC1', 'METRIC2',
//       ..., 'METRICn' (2 <= n <= 6), with the corresponding specified
//       'VALUE1', 'VALUE2', ..., 'VALUEn'.  'CATEGORY' and 'METRIC1',
//       'METRIC2', ..., 'METRICn', must be null-terminated strings of a type
//       convertible to 'const char *', and 'VALUE1', 'VALUE2', ..., 'VALUEn'
//       are assumed to be of a type convertible to 'double'.  These macros
//       maintain a (function-scope static) cache containing the identity of
//       the metrics being updated.  This cache is initialized using the
//       'CATEGORY' and metric identifiers specified on the *first*
//       application of these macros at a particular instantiation point;
//       subsequent applications use that cached information, which in
//       practice means that the 'CATEGORY' and metric identifiers must be
//       *runtime* *constants*.  If the default metrics manager has not been
//       initialized, or if the indicated 'CATEGORY' is currently disabled,
//       these macros have no effect.
//
//   BALM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BALM_METRICS_INT_UPDATEn(CATEGORY, METRIC1, VALUE1, ..., METRICn, VALUEn)
//                                                            Where 2 <= n <= 6
//       Update each of the indicated metrics, identified by the specified
//       'CATEGORY' name and the respective metric names 'METRIC1', 'METRIC2',
//       ..., 'METRICn' (2 <= n <= 6), with the corresponding specified
//       *integer* 'VALUE1', 'VALUE2', ..., 'VALUEn'.  'CATEGORY' and
//       'METRIC1', 'METRIC2', ..., 'METRICn', must be null-terminated strings
//       of a type convertible to 'const char *', and 'VALUE1', 'VALUE2', ...,
//       'VALUEn' are assumed to be of a type convertible to 'int'.  These
//       macros maintain a (function-scope static) cache containing the
//       identity of the metrics being updated.  This cache is initialized
//       using the 'CATEGORY' and metric identifiers specified on the *first*
//       application of these macros at a particular instantiation point;
//       subsequent applications use that cached information, which in
//       practice means that the 'CATEGORY' and metric identifiers must be
//       *runtime* *constants*.  If the default metrics manager has not been
//       initialized, or if the indicated 'CATEGORY' is currently disabled,
//       these macros have no effect.
//
//   BALM_METRICS_TYPED_UPDATE(CATEGORY,
//                             METRIC,
//                             VALUE,
//                             PREFERRED_PUBLICATION_TYPE)
//   BALM_METRICS_TYPED_INT_UPDATE(CATEGORY,
//                                 METRIC,
//                                 VALUE,
//                                 PREFERRED_PUBLICATION_TYPE)
//       The behavior of these macros is logically equivalent to
//       'BALM_METRICS_UPDATE(CATEGORY, METRIC, VALUE)' and
//       'BALM_METRICS_INT_UPDATE(CATEGORY, METRIC, VALUE)', respectively,
//       except that, on the first invocation, these macros also set the
//       indicated metric's 'PREFERRED_PUBLICATION_TYPE'.  The preferred
//       publication type of a metric indicates the preferred aggregate to
//       publish for that metric (e.g., 'COUNT', 'TOTAL', 'MIN', 'MAX', or
//       'RATE').  The behavior is undefined unless
//       'PREFERRED_PUBLICATION_TYPE' is convertible to
//       'balm::PublicationType::Value'.  Note that there is no uniform
//       definition for how publishers will interpret this value; an
//       'UNSPECIFIED' value, however, generally indicates that all of the
//       collected aggregates (total, count, minimum, and maximum value)
//       should be published.
//
//   BALM_METRICS_INCREMENT(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to:
//       'BALM_METRICS_INT_UPDATE(CATEGORY, METRIC, 1)'.
//
//   BALM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, 1, PREFERRED_TYPE)'.
//..
//  The following are the dynamic macros provided by this component for
//  updating a metric's value; these macros do not statically cache the
//  identity of the metric and look up the supplied 'CATEGORY' and 'METRIC' on
//  each invocation:
//..
//   BALM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the specified 'VALUE'.  'CATEGORY' and
//       'METRIC' must be null-terminated strings of a type convertible to
//       'const char *', and 'VALUE' is assumed to be of a type convertible to
//       'double'.  If the default metrics manager has not been initialized,
//       or if the indicated 'CATEGORY' is currently disabled, this macro has
//       no effect.  Note that this operation looks up the 'CATEGORY'
//       and 'METRIC' on *each* application, resulting in (unnecessary)
//       additional runtime overhead (if the 'CATEGORY' and 'METRIC' values are
//       always the same for a particular point of call).
//
//   BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the specified *integer* 'VALUE'.  'CATEGORY'
//       and 'METRIC' must be null-terminated strings of a type convertible to
//       'const char *', and 'VALUE' is assumed to be of a type convertible
//       to 'int'.  If the default metrics manager has not been initialized,
//       or if the indicated 'CATEGORY' is currently disabled, this macro has
//       no effect.  Note that this operation looks up the 'CATEGORY'
//       and 'METRIC' on *each* application, resulting in (unnecessary)
//       additional runtime overhead (if the 'CATEGORY' and 'METRIC' values
//       are always the same for a particular point of call).
//
//   BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, 1)'.
//..
// The following macro, 'BALM_METRICS_IF_CATEGORY_ENABLED', allows clients to
// (efficiently) determine if a (*runtime* *constant*) category is enabled:
//..
//   BALM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
//       This macro behaves like an 'if' clause, executing the subsequent
//       (compound) statement if the specified 'CATEGORY' is enabled.
//       'CATEGORY' must be a null-terminated string of a type convertible to
//       'const char *'.  If the default metrics manager has not been
//       initialized, or if the indicated 'CATEGORY' is currently disabled,
//       the following statement is not executed (i.e., the 'if'-condition
//       is 'false').  This macro maintains a (function-scope static) cache
//       containing the identity of the category.  This cache is initialized
//       using the 'CATEGORY' specified on the *first* application of this
//       macro at a particular instantiation point; subsequent applications
//       use that cached information, which in practice means that
//       'CATEGORY' must be a *runtime* *constant*.  *WARNING*: although the
//       'BALM_METRICS_IF_CATEGORY_ENABLED' may look like a function call,
//       it actually consists of a declaration and an 'if' statement, which
//       means that the following is syntactically *incorrect*:
//       'if (BALM_METRICS_IF_CATEGORY_ENABLED("myCategory")) { stuff() }'.
//       That should instead be written as:
//       'BALM_METRICS_IF_CATEGORY_ENABLED("myCategory") { stuff(); }'.
//..
// Finally, this component provides a set of macros to record the elapsed wall
// time of a block of code:
//..
//   BALM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the elapsed (wall) time, in the specified
//       'TIME_UNITS', from the point of instantiation of the macro to the end
//       of the enclosing lexical scope.  'CATEGORY' and 'METRIC'  must be
//       null-terminated strings of a type convertible to 'const char *',
//       while 'TIME_UNITS' is assumed to be of a type convertible to the
//       enumerated type 'balm::StopwatchScopedGuard::Units'.  This macro
//       maintains a (function-scope static) cache containing the identity of
//       the metric being updated.  This cache is initialized using the
//       'CATEGORY' and 'METRIC' specified on the *first* application of this
//       macro at a particular instantiation point; subsequent applications
//       use that cached information, which in practice means that
//       'CATEGORY' and 'METRIC' must be *runtime* *constants*.  If the
//       default metrics manager has not been initialized, or the identified
//       'CATEGORY' is disabled, this macro has no effect.  Note that
//       'TIME_UNITS' indicates the scale of value to report, but does *not*
//       affect the precision of the elapsed time measurement.
//
//   BALM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_SECONDS'.
//
//   BALM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_MILLISECONDS'.
//
//   BALM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_MICROSECONDS'.
//
//   BALM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_NANOSECONDS'.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the elapsed (wall) time, in the specified
//       'TIME_UNITS', from the instantiation of the macro to the end of the
//       enclosing lexical scope.  'CATEGORY' and 'METRIC'  must be
//       null-terminated strings of a type convertible to 'const char *',
//       while 'TIME_UNITS' is assumed to be of a type convertible to the
//       enumerated type 'balm::StopwatchScopedGuard::Units'.  If the default
//       metrics manager has not been initialized, or the identified
//       'CATEGORY' is disabled, this macro has no effect.  Note that
//       this operation looks up the 'CATEGORY' and 'METRIC' on *each*
//       application, resulting in (unnecessary) additional runtime overhead
//       (if the 'CATEGORY' and 'METRIC' values are always the same for a
//       particular point of call).  Also note that 'TIME_UNITS' indicates the
//       scale of value to report, but does *not* affect the precision of the
//       elapsed time measurement.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_SECONDS'.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_MILLISECONDS'.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_MICROSECONDS'.
//
//   BALM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BALM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'balm::StopwatchScopedGuard::k_NANOSECONDS'.
//..
//
///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1: Create and Configure the Default 'balm::MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricsManager'
// instance and perform a trivial configuration.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide this guard with an output stream ('stdout') to which the default
// metrics manager will publish metrics.  Note that the default metrics
// manager is intended to be created and destroyed by the *owner* of 'main':
// An instance of the manager should be created during the initialization of
// an application (while the task has a single thread) and destroyed just
// prior to termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//
//      balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default manager object has been created, it can be accessed using
// the 'instance' operation.
//..
//      balm::MetricsManager *manager =
//                                     balm::DefaultMetricsManager::instance();
//      assert(0 != manager);
//..
// Note that the default metrics manager will be released when the
// 'managerGuard' exits this scoped and is destroyed.  Clients that choose to
// explicitly call 'balm::DefaultMetricsManager::create()' must also explicitly
// call 'balm::DefaultMetricsManager::release()'.
//
///Example 2: Updating a Metric
/// - - - - - - - - - - - - - -
// Once a metrics manager is initialized, we can use the various macros to
// record metric values.  In this second example, we collect metrics from a
// hypothetical event-processing function.  We use 'BALM_METRICS_UPDATE' to
// record the size of the data being processed to a metric named "msgSize",
// and the elapsed time (in milliseconds) to process the event to a metric
// named "elapsedTime".  Finally, we use 'BALM_METRICS_INCREMENT' to record a
// count of failures to a metric named "failureCount".  Note that we do not use
// the '*_DYNAMIC_*' variants of the 'BALM_METRICS_UPDATE' or
// 'BALM_METRICS_INCREMENT' macros because the category and metric names are
// constant across all applications of the macro at a particular instantiation
// point (the 'DYNAMIC' variants look up the category and metric name on each
// application, which would incur unnecessary runtime overhead).
//..
//  int processEvent(int eventId, const bsl::string& eventMessage)
//      // Process the event described by the specified 'eventId' and
//      // 'eventMessage'.  Return 0 on success, and a non-zero
//      // value otherwise.
//  {
//      int returnCode = 0;
//
//      BALM_METRICS_UPDATE("processEvent", "msgSize", eventMessage.size());
//      BALM_METRICS_TIME_BLOCK_MILLISECONDS("processingEvent", "elapsedTime");
//
//      // Process 'data'.
//
//      if (0 != returnCode) {
//          BALM_METRICS_INCREMENT("processEvent", "failureCount");
//      }
//
//      return returnCode;
//  }
//..
//
///Example 3: Using 'BALM_METRICS_IF_CATEGORY_ENABLED'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this next example, we use 'BALM_METRICS_IF_CATEGORY_ENABLED' to
// conditionally disable a (relatively) expensive operation involved in
// computing a metric value.  The 'processEvent2' function, defined below, uses
// a 'bsls::Stopwatch' to record the elapsed system, user, and wall times,
// associated with processing the event.  The system calls used (via
// 'bsls::Stopwatch') to record the elapsed time may be relatively expensive,
// so we use 'BALM_METRICS_IF_CATEGORY_ENABLED' to ensure we perform those
// operations only if metrics collection is enabled.  Finally, we use
// 'BALM_METRICS_UPDATE3' to update the three metrics, which is (slightly) more
// efficient than updating each metric individually using 'BALM_METRIC_UPDATE'.
//..
//  int processEvent2(int eventId, const bsl::string& eventMessage)
//      // Process the event described by the specified 'eventId' and
//      // 'eventMessage'.  Return 0 on success, and a non-zero
//      // value otherwise.
//  {
//      int returnCode = 0;
//
//      bsls::Stopwatch stopwatch;
//      BALM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
//         stopwatch.start(true);
//      }
//
//      // Process 'data'.
//
//      BALM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
//         double systemTime, userTime, wallTime;
//         stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);
//         BALM_METRICS_UPDATE3("processEvent2",
//                              "systemTime", systemTime,
//                              "userTime",   userTime,
//                              "wallTime",   wallTime);
//      }
//
//      return returnCode;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_CATEGORY
#include <balm_category.h>
#endif

#ifndef INCLUDED_BALM_COLLECTOR
#include <balm_collector.h>
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

#ifndef INCLUDED_BALM_METRICREGISTRY
#include <balm_metricregistry.h>
#endif

#ifndef INCLUDED_BALM_METRICSMANAGER
#include <balm_metricsmanager.h>
#endif

#ifndef INCLUDED_BALM_PUBLICATIONTYPE
#include <balm_publicationtype.h>
#endif

#ifndef INCLUDED_BALM_STOPWATCHSCOPEDGUARD
#include <balm_stopwatchscopedguard.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                        // ================================
                        // BALM_METRICS_IF_CATEGORY_ENABLED
                        // ================================

#define BALM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)                            \
    BALM_METRICS_IF_CATEGORY_ENABLED_IMP(                                     \
                            CATEGORY, BALM_METRICS_UNIQUE_NAME(categoryHolder))

                        // ===================
                        // BALM_METRICS_UPDATE
                        // ===================

// Note that the static collector address must be assigned *before*
// initializing category holder to ensure initialization is thread safe.
#define BALM_METRICS_UPDATE(CATEGORY, METRIC1, VALUE1) do {                   \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2) do { \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   static balm::Collector *collector2 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_UPDATE3(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3) do {                                     \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   static balm::Collector *collector2 = 0;                                    \
   static balm::Collector *collector3 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       collector3 = Helper::getCollector(CATEGORY, METRIC3);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_UPDATE4(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3,                                          \
                             METRIC4,                                         \
                             VALUE4) do {                                     \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   static balm::Collector *collector2 = 0;                                    \
   static balm::Collector *collector3 = 0;                                    \
   static balm::Collector *collector4 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       collector3 = Helper::getCollector(CATEGORY, METRIC3);                  \
       collector4 = Helper::getCollector(CATEGORY, METRIC4);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_UPDATE5(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3,                                          \
                             METRIC4,                                         \
                             VALUE4,                                          \
                             METRIC5,                                         \
                             VALUE5) do {                                     \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   static balm::Collector *collector2 = 0;                                    \
   static balm::Collector *collector3 = 0;                                    \
   static balm::Collector *collector4 = 0;                                    \
   static balm::Collector *collector5 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC5, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       collector3 = Helper::getCollector(CATEGORY, METRIC3);                  \
       collector4 = Helper::getCollector(CATEGORY, METRIC4);                  \
       collector5 = Helper::getCollector(CATEGORY, METRIC5);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
       collector5->update(VALUE5);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_UPDATE6(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3,                                          \
                             METRIC4,                                         \
                             VALUE4,                                          \
                             METRIC5,                                         \
                             VALUE5,                                          \
                             METRIC6,                                         \
                             VALUE6) do {                                     \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   static balm::Collector *collector2 = 0;                                    \
   static balm::Collector *collector3 = 0;                                    \
   static balm::Collector *collector4 = 0;                                    \
   static balm::Collector *collector5 = 0;                                    \
   static balm::Collector *collector6 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY, Helper::e_TYPE_CATEGORY,                  \
                            __FILE__, __LINE__);                              \
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC5, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC6, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       collector3 = Helper::getCollector(CATEGORY, METRIC3);                  \
       collector4 = Helper::getCollector(CATEGORY, METRIC4);                  \
       collector5 = Helper::getCollector(CATEGORY, METRIC5);                  \
       collector6 = Helper::getCollector(CATEGORY, METRIC6);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
       collector5->update(VALUE5);                                            \
       collector6->update(VALUE6);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)    \
 do {                                                                         \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::Collector *collector1 = 0;                                    \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC, Helper::e_TYPE_METRIC, __FILE__, __LINE__); \
       collector1 = Helper::getCollector(CATEGORY, METRIC);                   \
       Helper::setPublicationType(collector1->metricId(), PREFERRED_TYPE);    \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE);                                             \
   }                                                                          \
 } while (0)

#define BALM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE) do {             \
    using namespace BloombergLP;                                              \
    if (balm::DefaultMetricsManager::instance()) {                            \
        balm::CollectorRepository& repository =                               \
             balm::DefaultMetricsManager::instance()->collectorRepository();  \
        balm::Collector *collector = repository.getDefaultCollector(          \
                                                        (CATEGORY), (METRIC));\
        if (collector->metricId().category()->enabled()) {                    \
            collector->update((VALUE));                                       \
        }                                                                     \
    }                                                                         \
  } while(0)

                        // =======================
                        // BALM_METRICS_INT_UPDATE
                        // =======================

#define BALM_METRICS_INT_UPDATE(CATEGORY, METRIC1, VALUE1) do {               \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_INT_UPDATE2(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2) do {                                 \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   static balm::IntegerCollector *collector2 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_INT_UPDATE3(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3) do {                                 \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   static balm::IntegerCollector *collector2 = 0;                             \
   static balm::IntegerCollector *collector3 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       collector3 = Helper::getIntegerCollector(CATEGORY, METRIC3);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_INT_UPDATE4(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3,                                      \
                                 METRIC4,                                     \
                                 VALUE4) do {                                 \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   static balm::IntegerCollector *collector2 = 0;                             \
   static balm::IntegerCollector *collector3 = 0;                             \
   static balm::IntegerCollector *collector4 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       collector3 = Helper::getIntegerCollector(CATEGORY, METRIC3);           \
       collector4 = Helper::getIntegerCollector(CATEGORY, METRIC4);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_INT_UPDATE5(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3,                                      \
                                 METRIC4,                                     \
                                 VALUE4,                                      \
                                 METRIC5,                                     \
                                 VALUE5) do {                                 \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   static balm::IntegerCollector *collector2 = 0;                             \
   static balm::IntegerCollector *collector3 = 0;                             \
   static balm::IntegerCollector *collector4 = 0;                             \
   static balm::IntegerCollector *collector5 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC5, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       collector3 = Helper::getIntegerCollector(CATEGORY, METRIC3);           \
       collector4 = Helper::getIntegerCollector(CATEGORY, METRIC4);           \
       collector5 = Helper::getIntegerCollector(CATEGORY, METRIC5);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
       collector5->update(VALUE5);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_INT_UPDATE6(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3,                                      \
                                 METRIC4,                                     \
                                 VALUE4,                                      \
                                 METRIC5,                                     \
                                 VALUE5,                                      \
                                 METRIC6,                                     \
                                 VALUE6) do {                                 \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   static balm::IntegerCollector *collector2 = 0;                             \
   static balm::IntegerCollector *collector3 = 0;                             \
   static balm::IntegerCollector *collector4 = 0;                             \
   static balm::IntegerCollector *collector5 = 0;                             \
   static balm::IntegerCollector *collector6 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC1, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC2, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC3, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC4, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC5, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
     Helper::logEmptyName(METRIC6, Helper::e_TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       collector3 = Helper::getIntegerCollector(CATEGORY, METRIC3);           \
       collector4 = Helper::getIntegerCollector(CATEGORY, METRIC4);           \
       collector5 = Helper::getIntegerCollector(CATEGORY, METRIC5);           \
       collector6 = Helper::getIntegerCollector(CATEGORY, METRIC6);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
       collector3->update(VALUE3);                                            \
       collector4->update(VALUE4);                                            \
       collector5->update(VALUE5);                                            \
       collector6->update(VALUE6);                                            \
   }                                                                          \
 } while (0)

#define BALM_METRICS_TYPED_INT_UPDATE(CATEGORY,                               \
                                      METRIC,                                 \
                                      VALUE,                                  \
                                      PREFERRED_TYPE)                         \
do {                                                                          \
   using namespace BloombergLP;                                               \
   typedef balm::Metrics_Helper Helper;                                       \
   static balm::CategoryHolder holder = { false, 0, 0 };                      \
   static balm::IntegerCollector *collector1 = 0;                             \
   if (0 == holder.category() && balm::DefaultMetricsManager::instance()) {   \
     Helper::logEmptyName(CATEGORY,Helper::e_TYPE_CATEGORY,__FILE__,__LINE__);\
     Helper::logEmptyName(METRIC, Helper::e_TYPE_METRIC, __FILE__, __LINE__); \
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC);            \
       Helper::setPublicationType(collector1->metricId(), PREFERRED_TYPE);    \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE);                                             \
   }                                                                          \
 } while (0)

#define BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE) do {         \
    using namespace BloombergLP;                                              \
    if (balm::DefaultMetricsManager::instance()) {                            \
        balm::CollectorRepository& repository =                               \
             balm::DefaultMetricsManager::instance()->collectorRepository();  \
        balm::IntegerCollector *collector =                                   \
               repository.getDefaultIntegerCollector((CATEGORY), (METRIC));   \
        if (collector->metricId().category()->enabled()) {                    \
            collector->update((VALUE));                                       \
        }                                                                     \
    }                                                                         \
  } while (0)

#define BALM_METRICS_INCREMENT(CATEGORY, METRIC)                              \
    BALM_METRICS_INT_UPDATE(CATEGORY, METRIC, 1)

#define BALM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)        \
    BALM_METRICS_TYPED_INT_UPDATE(CATEGORY, METRIC, 1, PREFERRED_TYPE)

#define BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)                      \
    BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, 1)

                        // =======================
                        // BALM_METRICS_TIME_BLOCK
                        // =======================

#define BALM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)                 \
  BALM_METRICS_TIME_BLOCK_IMP((CATEGORY),                                     \
                               (METRIC),                                      \
                               TIME_UNITS,                                    \
                               BALM_METRICS_UNIQUE_NAME(_bAlM_CoLlEcToR))

#define BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)         \
  BALM_METRICS_DYNAMIC_TIME_BLOCK_IMP(                                        \
                                  (CATEGORY),                                 \
                                  (METRIC),                                   \
                                  TIME_UNITS,                                 \
                                  BALM_METRICS_UNIQUE_NAME(_bAlM_CoLlEcToR))

#define BALM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)                     \
  BALM_METRICS_TIME_BLOCK((CATEGORY),                                         \
                          (METRIC),                                           \
                          BloombergLP::balm::StopwatchScopedGuard::k_SECONDS);

#define BALM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)                \
  BALM_METRICS_TIME_BLOCK(                                                    \
                      (CATEGORY),                                             \
                      (METRIC),                                               \
                      BloombergLP::balm::StopwatchScopedGuard::k_MILLISECONDS);

#define BALM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)                \
  BALM_METRICS_TIME_BLOCK(                                                    \
                      (CATEGORY),                                             \
                      (METRIC),                                               \
                      BloombergLP::balm::StopwatchScopedGuard::k_MICROSECONDS);

#define BALM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)                 \
  BALM_METRICS_TIME_BLOCK(                                                    \
                       (CATEGORY),                                            \
                       (METRIC),                                              \
                       BloombergLP::balm::StopwatchScopedGuard::k_NANOSECONDS);

#define BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)             \
  BALM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                           (CATEGORY),                                        \
                           (METRIC),                                          \
                           BloombergLP::balm::StopwatchScopedGuard::k_SECONDS);

#define BALM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)        \
  BALM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                      (CATEGORY),                                             \
                      (METRIC),                                               \
                      BloombergLP::balm::StopwatchScopedGuard::k_MILLISECONDS);

#define BALM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)        \
  BALM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                      (CATEGORY),                                             \
                      (METRIC),                                               \
                      BloombergLP::balm::StopwatchScopedGuard::k_MICROSECONDS);

#define BALM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)         \
  BALM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                       (CATEGORY),                                            \
                       (METRIC),                                              \
                       BloombergLP::balm::StopwatchScopedGuard::k_NANOSECONDS);

                        // =====================
                        // Macro Implementations
                        // =====================

#define BALM_METRICS_IF_CATEGORY_ENABLED_IMP(CATEGORY, HOLDER_NAME)           \
    static BloombergLP::balm::CategoryHolder HOLDER_NAME = { false, 0, 0 };   \
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!HOLDER_NAME.category())        \
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(                                \
                 BloombergLP::balm::DefaultMetricsManager::instance() != 0)) {\
        BloombergLP::balm::Metrics_Helper::initializeCategoryHolder(          \
                                                   &HOLDER_NAME, CATEGORY);   \
    }                                                                         \
    if (HOLDER_NAME.enabled())

// Declare a static pointer to a 'balm::Collector' with the specified
// 'VARIABLE_NAME' and an initial value of 0.  If the default metrics manager
// is available and the declared pointer variable (named 'VARIABLE_NAME') is 0,
// assign to 'VARIABLE_NAME' the address of a collector for the specified
// 'CATEGORY' and 'METRIC'.  Finally, declare a 'balm::StopwatchScopedGuard'
// object with a unique variable name and supply its constructor the collector
// address held in 'VARIABLE_NAME' and the specified 'TIME_UNITS'.
#define BALM_METRICS_TIME_BLOCK_IMP(CATEGORY,                                 \
                                    METRIC,                                   \
                                    TIME_UNITS,                               \
                                    VARIABLE_NAME)                            \
    static BloombergLP::balm::Collector *VARIABLE_NAME = 0;                   \
    if (BloombergLP::balm::DefaultMetricsManager::instance()) {               \
       using namespace BloombergLP;                                           \
       if (0 == VARIABLE_NAME) {                                              \
           balm::CollectorRepository& repository =                            \
              balm::DefaultMetricsManager::instance()->collectorRepository(); \
           VARIABLE_NAME = repository.getDefaultCollector((CATEGORY),         \
                                                          (METRIC));          \
       }                                                                      \
    }                                                                         \
    else {                                                                    \
       VARIABLE_NAME = 0;                                                     \
    }                                                                         \
    BloombergLP::balm::StopwatchScopedGuard                                   \
         BALM_METRICS_UNIQUE_NAME(__bAlM_gUaRd)(VARIABLE_NAME, TIME_UNITS);

// Declare a pointer to a 'balm::Collector' with the specified 'VARIABLE_NAME'.
// If the default metrics manager is available, assign to the declared pointer
// variable (named 'VARIABLE_NAME') the address of a collector for the
// specified 'CATEGORY' and 'METRIC'.  Finally, declare a
// 'balm::StopwatchScopedGuard' object with a unique variable name and supply
// its constructor the collector address held in 'VARIABLE_NAME' and the
// specified 'TIME_UNITS'.
#define BALM_METRICS_DYNAMIC_TIME_BLOCK_IMP(CATEGORY,                         \
                                            METRIC,                           \
                                            TIME_UNITS,                       \
                                            VARIABLE_NAME)                    \
    BloombergLP::balm::Collector *VARIABLE_NAME = 0;                          \
    if (BloombergLP::balm::DefaultMetricsManager::instance()) {               \
        using namespace BloombergLP;                                          \
        balm::CollectorRepository& repository =                               \
             balm::DefaultMetricsManager::instance()->collectorRepository();  \
        VARIABLE_NAME = repository.getDefaultCollector((CATEGORY),            \
                                                       (METRIC));             \
    }                                                                         \
    BloombergLP::balm::StopwatchScopedGuard                                   \
         BALM_METRICS_UNIQUE_NAME(__bAlM_gUaRd)(VARIABLE_NAME, TIME_UNITS);

                        // ------------------------
                        // Unique line number macro
                        // ------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC)
// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)
// Fortunately the __COUNTER__ extension provided by MSVC is even better.
#   define BALM_METRICS_UNIQNUM __COUNTER__
#else
#   define BALM_METRICS_UNIQNUM __LINE__
#endif

                        // ----------------------------------
                        // Token concatenation support macros
                        // ----------------------------------

// Second layer needed to ensure that arguments are expanded before
// concatenation.
#define BALM_METRICS_CAT(X, Y) BALM_METRICS_CAT_IMP(X, Y)
#define BALM_METRICS_CAT_IMP(X, Y) X##Y

                        // ----------------------------------
                        // Unique variable name support macro
                        // ----------------------------------

// Create a unique variable name by concatenating the specified 'X' string
// with a unique integer value.
#define BALM_METRICS_UNIQUE_NAME(X)                                           \
           BALM_METRICS_CAT(X, BALM_METRICS_UNIQNUM)

namespace BloombergLP {

namespace balm {
                           // =====================
                           // struct Metrics_Helper
                           // =====================

struct Metrics_Helper {
    // This 'struct' provides a namespace for a suite of functions used in the
    // implementation of the macros defined in this component.
    //
    // This type is an implementation detail and *must* *not* be used
    // (directly) by clients outside of this component.

    // TYPES
    enum NameType {
        // Enumeration indicating the type of identifier supplied to
        // 'logEmptyName'.

        e_TYPE_CATEGORY = 0,
        e_TYPE_METRIC   = 1
    };

    // CLASS METHODS
    static void initializeCategoryHolder(CategoryHolder *holder,
                                         const char     *category);
        // Load into the specified 'holder' the address and enabled status of
        // the specified 'category', and add 'holder' to the list of category
        // holders for 'category'.  The behavior is undefined unless the balm
        // metrics manager singleton is valid.

    static Collector *getCollector(const char *category,
                                   const char *metric);
        // Return the address of the default metrics collector for the metric
        // identified by the specified 'category' and 'metric' names.  The
        // behavior is undefined unless the 'balm' metrics manager singleton is
        // valid.

    static IntegerCollector *getIntegerCollector(const char *category,
                                                 const char *metric);
        // Return the address of the default integer metrics collector for the
        // metric identified by the specified 'category' and 'metric' names.
        // The behavior is undefined unless the 'balm' metrics manager
        // singleton is valid.

    static void setPublicationType(const MetricId&        id,
                                   PublicationType::Value type);
        // Set the publication type for the metric identified by the specified
        // 'id' to the specified 'type'.  The behavior is undefined unless the
        // 'balm' metrics manager singleton is valid, and 'id' is a valid
        // identifier supplied by the singleton metrics manager.

    static void logEmptyName(const char *name,
                             NameType    type,
                             const char *file,
                             int         line);
        // If the specified 'name' is empty or contains only whitespace, then
        // log a warning message indicating whether 'name' is a category or a
        // metric, depending on the specified 'type', and the location at which
        // this function is called, indicated by the specified 'file' and
        // 'line'; otherwise, do nothing.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // struct Metrics_Helper
                           // ---------------------

// CLASS METHODS
inline
void Metrics_Helper::initializeCategoryHolder(CategoryHolder *holder,
                                              const char     *category)
{
    MetricsManager  *manager  = DefaultMetricsManager::instance();
    MetricRegistry&  registry = manager->metricRegistry();
    registry.registerCategoryHolder(registry.getCategory(category), holder);
}

inline
Collector *Metrics_Helper::getCollector(const char *category,
                                        const char *metric)
{
    MetricsManager *manager = DefaultMetricsManager::instance();
    return manager->collectorRepository().getDefaultCollector(category,
                                                              metric);
}

inline
IntegerCollector *Metrics_Helper::getIntegerCollector(const char *category,
                                                      const char *metric)
{
    MetricsManager *manager = DefaultMetricsManager::instance();
    return manager->collectorRepository().getDefaultIntegerCollector(category,
                                                                     metric);
}

inline
void Metrics_Helper::setPublicationType(const MetricId&        id,
                                        PublicationType::Value type)
{
    MetricsManager *manager = DefaultMetricsManager::instance();
    return manager->metricRegistry().setPreferredPublicationType(id, type);

}

}  // close package namespace
}  // close enterprise namespace

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT
#define BALM_METRICS__UNIQUE_NAME(X) BALM_METRICS_UNIQUE_NAME(X)
#endif

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
