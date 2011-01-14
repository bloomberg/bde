// baem_metrics.h                                                     -*-C++-*-
#ifndef INCLUDED_BAEM_METRICS
#define INCLUDED_BAEM_METRICS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a suite of operations for recording metric values.
//
//@CLASSES:
//
//@SEE_ALSO: baem_collector, baem_integercollector, baem_defaultmetricsmanager
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a suite of macros to simplify the
// process of collecting metrics.  A metric records the number of times an
// event occurs, as well as an associated measurement value.  A metric
// maintains a count of event occurrences and the aggregated minimum,
// maximum, and total of the measured metric-event values.  Note that this
// component does *not* define what constitutes an event nor what the
// associated value represents.
//
///Thread Safety
///-------------
// All the macros defined in this component are *thread-safe*, meaning
// that they can be safely invoked simultaneously from multiple threads.
// It is *not* safe, however, to invoke any of the macros defined in this
// component while the default metrics manager is being either created or
// destroyed.
//
///Macro Summary
///-------------
// This section provides a brief description of the macros defined in this
// component.  In most cases, category and metric names supplied as macro
// arguments are required to be *runtime* *constants*, meaning that their
// values must be constant for the lifetime of the process.  More complete
// documentation is provided in the 'Macro Reference' section.
//..
//   BAEM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
//       This macro behaves like an 'if' clause, executing the subsequent
//       (compound) statement if the specified 'CATEGORY' is enabled.
//
//   BAEM_METRICS_UPDATE(CATEGORY, METRIC1, VALUE1)
//   BAEM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BAEM_METRICS_UPDATE3(CATEGORY, METRIC1, VALUE1, ..., METRIC3, VALUE3)
//   BAEM_METRICS_UPDATE4(CATEGORY, METRIC1, VALUE1, ..., METRIC4, VALUE4)
//   BAEM_METRICS_UPDATE5(CATEGORY, METRIC1, VALUE1, ..., METRIC5, VALUE5)
//   BAEM_METRICS_UPDATE6(CATEGORY, METRIC1, VALUE1, ..., METRIC6, VALUE6)
//   BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC1, VALUE1)
//   BAEM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BAEM_METRICS_INT_UPDATE3(CATEGORY, METRIC1, VALUE1, ..., METRIC3, VALUE3)
//   BAEM_METRICS_INT_UPDATE4(CATEGORY, METRIC1, VALUE1, ..., METRIC4, VALUE4)
//   BAEM_METRICS_INT_UPDATE5(CATEGORY, METRIC1, VALUE1, ..., METRIC5, VALUE5)
//   BAEM_METRICS_INT_UPDATE6(CATEGORY, METRIC1, VALUE1, ..., METRIC6, VALUE6)
//       Update each of up to 6 metrics by the corresponding values.
//       The supplied category and metric names must be *runtime* *constants*.
//
//   BAEM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)
//   BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)
//       Update the identified metric by 'VALUE' and set its preferred
//       publication type.  'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.
//
//   BAEM_METRICS_INCREMENT(CATEGORY, METRIC)
//       Increment (by 1) the identified metric.  'CATEGORY' and 'METRIC' must
//       be *runtime* *constants*.
//
//   BAEM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)
//       Increment (by 1) the identified metric and set the metric's preferred
//       publication type.  'CATEGORY' and 'METRIC' must be *runtime*
//       *constants*.
//
//   BAEM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE)
//   BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE)
//       Update the identified metric by 'VALUE'.  This operation performs a
//       lookup on 'CATEGORY' and 'METRIC' on each invocation, so those values
//       need *not* be runtime constants.
//
//   BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)
//       Increment (by 1) the identified metric.  This operation performs a
//       lookup on 'CATEGORY' and 'METRIC' on each invocation, so those values
//       need *not* be runtime constants.
//
//   BAEM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//   BAEM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       Update the identified metric by the elapsed (wall) time, in the
//       indicated units, from the instantiation point of the macro to the end
//       of the enclosing lexical scope.  'CATEGORY' and 'METRIC' must
//       be *runtime* *constants*.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
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
// 'baem_MetricsManager'.  The macros have no effect unless the metrics manager
// default instance has been initialized via a call to
// 'baem_DefaultMetricsManager::create'.
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
//   BAEM_METRICS_UPDATE(CATEGORY, METRIC, VALUE)
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
//   BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC, VALUE)
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
//   BAEM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BAEM_METRICS_UPDATEn(CATEGORY, METRIC1, VALUE1, ..., METRICn, VALUEn)
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
//   BAEM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2)
//   BAEM_METRICS_INT_UPDATEn(CATEGORY, METRIC1, VALUE1, ..., METRICn, VALUEn)
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
//   BAEM_METRICS_TYPED_UPDATE(CATEGORY,
//                             METRIC,
//                             VALUE,
//                             PREFERRED_PUBLICATION_TYPE)
//   BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY,
//                                 METRIC,
//                                 VALUE,
//                                 PREFERRED_PUBLICATION_TYPE)
//       The behavior of these macros is logically equivalent to
//       'BAEM_METRICS_UPDATE(CATEGORY, METRIC, VALUE)' and
//       'BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC, VALUE)', respectively,
//       except that, on the first invocation, these macros also set the
//       indicated metric's 'PREFERRED_PUBLICATION_TYPE'.  The preferred
//       publication type of a metric indicates the preferred aggregate to
//       publish for that metric (e.g., 'COUNT', 'TOTAL', 'MIN', 'MAX', or
//       'RATE').  The behavior is undefined unless
//       'PREFERRED_PUBLICATION_TYPE' is convertible to
//       'baem_PublicationType::Value'.  Note that there is no uniform
//       definition for how publishers will interpret this value; an
//       'UNSPECIFIED' value, however, generally indicates that all of the
//       collected aggregates (total, count, minimum, and maximum value)
//       should be published.
//
//   BAEM_METRICS_INCREMENT(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to:
//       'BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC, 1)'.
//
//   BAEM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, 1, PREFERRED_TYPE)'.
//..
//  The following are the dynamic macros provided by this component for
//  updating a metric's value; these macros do not statically cache the
//  identity of the metric and look up the supplied 'CATEGORY' and 'METRIC' on
//  each invocation:
//..
//   BAEM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE)
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
//   BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE)
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
//   BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, 1)'.
//..
// The following macro, 'BAEM_METRICS_IF_CATEGORY_ENABLED', allows clients to
// (efficiently) determine if a (*runtime* *constant*) category is enabled:
//..
//   BAEM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
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
//       'BAEM_METRICS_IF_CATEGORY_ENABLED' may look like a function call,
//       it actually consists of a declaration and an 'if' statement, which
//       means that the following is syntactically *incorrect*:
//       'if (BAEM_METRICS_IF_CATEGORY_ENABLED("myCategory")) { stuff() }'.
//       That should instead be written as:
//       'BAEM_METRICS_IF_CATEGORY_ENABLED("myCategory") { stuff(); }'.
//..
// Finally, this component provides a set of macros to record the elapsed wall
// time of a block of code:
//..
//   BAEM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the elapsed (wall) time, in the specified
//       'TIME_UNITS', from the point of instantiation of the macro to the end
//       of the enclosing lexical scope.  'CATEGORY' and 'METRIC'  must be
//       null-terminated strings of a type convertible to 'const char *',
//       while 'TIME_UNITS' is assumed to be of a type convertible to the
//       enumerated type 'baem_StopwatchScopedGuard::Units'.  This macro
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
//   BAEM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::SECONDS'.
//
//   BAEM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::MILLISECONDS'.
//
//   BAEM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::MICROSECONDS'.
//
//   BAEM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::NANOSECONDS'.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
//       Update the indicated metric, identified by the specified 'CATEGORY'
//       and 'METRIC' names, by the elapsed (wall) time, in the specified
//       'TIME_UNITS', from the instantiation of the macro to the end of the
//       enclosing lexical scope.  'CATEGORY' and 'METRIC'  must be
//       null-terminated strings of a type convertible to 'const char *',
//       while 'TIME_UNITS' is assumed to be of a type convertible to the
//       enumerated type 'baem_StopwatchScopedGuard::Units'.  If the default
//       metrics manager has not been initialized, or the identified
//       'CATEGORY' is disabled, this macro has no effect.  Note that
//       this operation looks up the 'CATEGORY' and 'METRIC' on *each*
//       application, resulting in (unnecessary) additional runtime overhead
//       (if the 'CATEGORY' and 'METRIC' values are always the same for a
//       particular point of call).  Also note that 'TIME_UNITS' indicates the
//       scale of value to report, but does *not* affect the precision of the
//       elapsed time measurement.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::BAEM_SECONDS'.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::BAEM_MILLISECONDS'.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::BAEM_MICROSECONDS'.
//
//   BAEM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//       The behavior of this macro is logically equivalent to
//       'BAEM_METRICS_DYNAMIC_TIME_BLOCK' called with
//       'baem_StopwatchScopedGuard::BAEM_NANOSECONDS'.
//..
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
//      baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default manager object has been created, it can be accessed using
// the 'instance' operation.
//..
//      baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
//      assert(0 != manager);
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// goes out of scope; clients who instead choose to call
// 'baem_DefaultMetricsManager::create' explicitly must also explicitly call
// 'baem_DefaultMetricsManager::destroy()'.
//
///Example 2 - Updating a Metric
///- - - - - - - - - - - - - - -
// Once a metrics manager is initialized, we can use the various macros to
// record metric values.  In this second example, we collect metrics from a
// hypothetical event-processing function.  We use 'BAEM_METRICS_UPDATE' to
// record the size of the data being processed to a metric named
// "msgSize", and the elapsed time (in milliseconds) to process the event to a
// metric named "elapsedTime".  Finally, we use 'BAEM_METRICS_INCREMENT'
// to record a count of failures to a metric named "failureCount".
// Note that we do not use the '*_DYNAMIC_*' variants of the
// 'BAEM_METRICS_UPDATE' or 'BAEM_METRICS_INCREMENT' macros because the
// category and metric names are constant across all applications of the macro
// at a particular instantiation point (the 'DYNAMIC' variants look up the
// category and metric name on each application, which would incur unnecessary
// runtime overhead).
//..
//  int processEvent(int eventId, const bsl::string& eventMessage)
//      // Process the event described by the specified 'eventId' and
//      // 'eventMessage'.  Return 0 on success, and a non-zero
//      // value otherwise.
//  {
//      int returnCode = 0;
//
//      BAEM_METRICS_UPDATE("processEvent", "msgSize", eventMessage.size());
//      BAEM_METRICS_TIME_BLOCK_MILLISECONDS("processingEvent", "elapsedTime");
//
//      // Process 'data'.
//
//      if (0 != returnCode) {
//          BAEM_METRICS_INCREMENT("processEvent", "failureCount");
//      }
//
//      return returnCode;
//  }
//..
//
///Example 3 - Using 'BAEM_METRICS_IF_CATEGORY_ENABLED'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this next example, we use 'BAEM_METRICS_IF_CATEGORY_ENABLED' to
// conditionally disable a (relatively) expensive operation involved in
// computing a metric value.  The 'processEvent2' function, defined below, uses
// a 'bsls_Stopwatch' to record the elapsed system, user, and wall times,
// associated with processing the event.  The system calls used (via
// 'bsls_Stopwatch') to record the elapsed time may be relatively expensive,
// so we use 'BAEM_METRICS_IF_CATEGORY_ENABLED' to ensure we perform those
// operations only if metrics collection is enabled.  Finally, we use
// 'BAEM_METRICS_UPDATE3' to update the three metrics, which is (slightly) more
// efficient than updating each metric individually using 'BAEM_METRIC_UPDATE'.
//..
//  int processEvent2(int eventId, const bsl::string& eventMessage)
//      // Process the event described by the specified 'eventId' and
//      // 'eventMessage'.  Return 0 on success, and a non-zero
//      // value otherwise.
//  {
//      int returnCode = 0;
//
//      bsls_Stopwatch stopwatch;
//      BAEM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
//         stopwatch.start(true);
//      }
//
//      // Process 'data'.
//
//      BAEM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
//         double systemTime, userTime, wallTime;
//         stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);
//         BAEM_METRICS_UPDATE3("processEvent2",
//                              "systemTime", systemTime,
//                              "userTime",   userTime,
//                              "wallTime",   wallTime);
//      }
//
//      return returnCode;
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_CATEGORY
#include <baem_category.h>
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

#ifndef INCLUDED_BAEM_INTEGERCOLLECTOR
#include <baem_integercollector.h>
#endif

#ifndef INCLUDED_BAEM_METRICID
#include <baem_metricid.h>
#endif

#ifndef INCLUDED_BAEM_METRICREGISTRY
#include <baem_metricregistry.h>
#endif

#ifndef INCLUDED_BAEM_METRICSMANAGER
#include <baem_metricsmanager.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

#ifndef INCLUDED_BAEM_STOPWATCHSCOPEDGUARD
#include <baem_stopwatchscopedguard.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                        // ================================
                        // BAEM_METRICS_IF_CATEGORY_ENABLED
                        // ================================

#define BAEM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)                            \
    BAEM_METRICS__IF_CATEGORY_ENABLED_IMP(                                    \
                        CATEGORY, BAEM_METRICS__UNIQUE_NAME(categoryHolder))

                        // ===================
                        // BAEM_METRICS_UPDATE
                        // ===================

// Note that the static collector address must be assigned *before*
// initializing category holder to ensure initialization is thread
// safe.
#define BAEM_METRICS_UPDATE(CATEGORY, METRIC1, VALUE1) do {                   \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, METRIC2, VALUE2) do { \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   static baem_Collector *collector2 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getCollector(CATEGORY, METRIC1);                  \
       collector2 = Helper::getCollector(CATEGORY, METRIC2);                  \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_UPDATE3(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3) do {                                     \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   static baem_Collector *collector2 = 0;                                     \
   static baem_Collector *collector3 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_UPDATE4(CATEGORY,                                        \
                             METRIC1,                                         \
                             VALUE1,                                          \
                             METRIC2,                                         \
                             VALUE2,                                          \
                             METRIC3,                                         \
                             VALUE3,                                          \
                             METRIC4,                                         \
                             VALUE4) do {                                     \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   static baem_Collector *collector2 = 0;                                     \
   static baem_Collector *collector3 = 0;                                     \
   static baem_Collector *collector4 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_UPDATE5(CATEGORY,                                        \
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
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   static baem_Collector *collector2 = 0;                                     \
   static baem_Collector *collector3 = 0;                                     \
   static baem_Collector *collector4 = 0;                                     \
   static baem_Collector *collector5 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC5, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_UPDATE6(CATEGORY,                                        \
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
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   static baem_Collector *collector2 = 0;                                     \
   static baem_Collector *collector3 = 0;                                     \
   static baem_Collector *collector4 = 0;                                     \
   static baem_Collector *collector5 = 0;                                     \
   static baem_Collector *collector6 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY, Helper::TYPE_CATEGORY,                  \
                            __FILE__, __LINE__);                              \
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC5, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC6, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_TYPED_UPDATE(CATEGORY, METRIC, VALUE, PREFERRED_TYPE)    \
 do {                                                                         \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_Collector *collector1 = 0;                                     \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC, Helper::TYPE_METRIC, __FILE__, __LINE__); \
       collector1 = Helper::getCollector(CATEGORY, METRIC);                   \
       Helper::setPublicationType(collector1->metricId(), PREFERRED_TYPE);    \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE);                                             \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_DYNAMIC_UPDATE(CATEGORY, METRIC, VALUE) do {             \
    using namespace BloombergLP;                                              \
    if (baem_DefaultMetricsManager::instance()) {                             \
        baem_CollectorRepository& repository =                                \
             baem_DefaultMetricsManager::instance()->collectorRepository();   \
        baem_Collector *collector = repository.getDefaultCollector((CATEGORY),\
                                                                   (METRIC)); \
        if (collector->metricId().category()->enabled()) {                    \
            collector->update((VALUE));                                       \
        }                                                                     \
    }                                                                         \
  } while(0)

                        // =======================
                        // BAEM_METRICS_INT_UPDATE
                        // =======================

#define BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC1, VALUE1) do {               \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_INT_UPDATE2(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2) do {                                 \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   static baem_IntegerCollector *collector2 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC1);           \
       collector2 = Helper::getIntegerCollector(CATEGORY, METRIC2);           \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE1);                                            \
       collector2->update(VALUE2);                                            \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_INT_UPDATE3(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3) do {                                 \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   static baem_IntegerCollector *collector2 = 0;                              \
   static baem_IntegerCollector *collector3 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_INT_UPDATE4(CATEGORY,                                    \
                                 METRIC1,                                     \
                                 VALUE1,                                      \
                                 METRIC2,                                     \
                                 VALUE2,                                      \
                                 METRIC3,                                     \
                                 VALUE3,                                      \
                                 METRIC4,                                     \
                                 VALUE4) do {                                 \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   static baem_IntegerCollector *collector2 = 0;                              \
   static baem_IntegerCollector *collector3 = 0;                              \
   static baem_IntegerCollector *collector4 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_INT_UPDATE5(CATEGORY,                                    \
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
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   static baem_IntegerCollector *collector2 = 0;                              \
   static baem_IntegerCollector *collector3 = 0;                              \
   static baem_IntegerCollector *collector4 = 0;                              \
   static baem_IntegerCollector *collector5 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC5, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_INT_UPDATE6(CATEGORY,                                    \
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
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   static baem_IntegerCollector *collector2 = 0;                              \
   static baem_IntegerCollector *collector3 = 0;                              \
   static baem_IntegerCollector *collector4 = 0;                              \
   static baem_IntegerCollector *collector5 = 0;                              \
   static baem_IntegerCollector *collector6 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC1, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC2, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC3, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC4, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC5, Helper::TYPE_METRIC, __FILE__, __LINE__);\
       Helper::logEmptyName(METRIC6, Helper::TYPE_METRIC, __FILE__, __LINE__);\
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

#define BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY,                               \
                                      METRIC,                                 \
                                      VALUE,                                  \
                                      PREFERRED_TYPE)                         \
do {                                                                          \
   using namespace BloombergLP;                                               \
   typedef baem_Metrics_Helper Helper;                                        \
   static baem_CategoryHolder holder = { false, 0, 0 };                       \
   static baem_IntegerCollector *collector1 = 0;                              \
   if (0 == holder.category() && baem_DefaultMetricsManager::instance()) {    \
       Helper::logEmptyName(CATEGORY,Helper::TYPE_CATEGORY,__FILE__,__LINE__);\
       Helper::logEmptyName(METRIC, Helper::TYPE_METRIC, __FILE__, __LINE__); \
       collector1 = Helper::getIntegerCollector(CATEGORY, METRIC);            \
       Helper::setPublicationType(collector1->metricId(), PREFERRED_TYPE);    \
       Helper::initializeCategoryHolder(&holder, CATEGORY);                   \
   }                                                                          \
   if (holder.enabled()) {                                                    \
       collector1->update(VALUE);                                             \
   }                                                                          \
 } while (0)

#define BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, VALUE) do {         \
    using namespace BloombergLP;                                              \
    if (baem_DefaultMetricsManager::instance()) {                             \
        baem_CollectorRepository& repository =                                \
             baem_DefaultMetricsManager::instance()->collectorRepository();   \
        baem_IntegerCollector *collector =                                    \
               repository.getDefaultIntegerCollector((CATEGORY), (METRIC));   \
        typedef baem_Metrics_Helper Helper;                                   \
        if (collector->metricId().category()->enabled()) {                    \
            collector->update((VALUE));                                       \
        }                                                                     \
    }                                                                         \
  } while (0)

#define BAEM_METRICS_INCREMENT(CATEGORY, METRIC)                              \
    BAEM_METRICS_INT_UPDATE(CATEGORY, METRIC, 1)

#define BAEM_METRICS_TYPED_INCREMENT(CATEGORY, METRIC, PREFERRED_TYPE)        \
    BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY, METRIC, 1, PREFERRED_TYPE)

#define BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, METRIC)                      \
    BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, METRIC, 1)

                        // =======================
                        // BAEM_METRICS_TIME_BLOCK
                        // =======================

#define BAEM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)                 \
  BAEM_METRICS__TIME_BLOCK_IMP((CATEGORY),                                    \
                               (METRIC),                                      \
                               TIME_UNITS,                                    \
                               BAEM_METRICS__UNIQUE_NAME(_bAeM_CoLlEcToR))

#define BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)         \
  BAEM_METRICS__DYNAMIC_TIME_BLOCK_IMP(                                       \
                                  (CATEGORY),                                 \
                                  (METRIC),                                   \
                                  TIME_UNITS,                                 \
                                  BAEM_METRICS__UNIQUE_NAME(_bAeM_CoLlEcToR))

#define BAEM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)                     \
  BAEM_METRICS_TIME_BLOCK((CATEGORY),                                         \
                          (METRIC),                                           \
                          baem_StopwatchScopedGuard::BAEM_SECONDS);

#define BAEM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)                \
  BAEM_METRICS_TIME_BLOCK((CATEGORY),                                         \
                          (METRIC),                                           \
                          baem_StopwatchScopedGuard::BAEM_MILLISECONDS);

#define BAEM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)                \
  BAEM_METRICS_TIME_BLOCK((CATEGORY),                                         \
                          (METRIC),                                           \
                          baem_StopwatchScopedGuard::BAEM_MICROSECONDS);

#define BAEM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)                 \
  BAEM_METRICS_TIME_BLOCK((CATEGORY),                                         \
                          (METRIC),                                           \
                          baem_StopwatchScopedGuard::BAEM_NANOSECONDS);

#define BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)             \
  BAEM_METRICS_DYNAMIC_TIME_BLOCK((CATEGORY),                                 \
                                  (METRIC),                                   \
                                  baem_StopwatchScopedGuard::BAEM_SECONDS);

#define BAEM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)        \
  BAEM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                                 (CATEGORY),                                  \
                                 (METRIC),                                    \
                                 baem_StopwatchScopedGuard::BAEM_MILLISECONDS);

#define BAEM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)        \
  BAEM_METRICS_DYNAMIC_TIME_BLOCK(                                            \
                                 (CATEGORY),                                  \
                                 (METRIC),                                    \
                                 baem_StopwatchScopedGuard::BAEM_MICROSECONDS);

#define BAEM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)         \
  BAEM_METRICS_DYNAMIC_TIME_BLOCK((CATEGORY),                                 \
                                  (METRIC),                                   \
                                  baem_StopwatchScopedGuard::BAEM_NANOSECONDS);

                        // =====================
                        // Macro Implementations
                        // =====================

#define BAEM_METRICS__IF_CATEGORY_ENABLED_IMP(CATEGORY, HOLDER_NAME)          \
    static BloombergLP::baem_CategoryHolder HOLDER_NAME = { false, 0, 0 };    \
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!HOLDER_NAME.category())        \
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(                                \
                 BloombergLP::baem_DefaultMetricsManager::instance() != 0)) { \
        BloombergLP::baem_Metrics_Helper::initializeCategoryHolder(           \
                                                   &HOLDER_NAME, CATEGORY);   \
    }                                                                         \
    if (HOLDER_NAME.enabled())

// Declare a static pointer to a 'baem_Collector' with the specified
// 'VARIABLE_NAME' and an initial value of 0.  If the default metrics manager
// is available and the declared pointer variable (named 'VARIABLE_NAME') is 0,
// assign to 'VARIABLE_NAME' the address of a collector for the specified
// 'CATEGORY' and 'METRIC'.  Finally, declare a 'baem_StopwatchScopedGuard'
// object with a unique variable name and supply its constructor the collector
// address held in 'VARIABLE_NAME' and the specified 'TIME_UNITS'.
#define BAEM_METRICS__TIME_BLOCK_IMP(CATEGORY,                                \
                                     METRIC,                                  \
                                     TIME_UNITS,                              \
                                     VARIABLE_NAME)                           \
    static BloombergLP::baem_Collector *VARIABLE_NAME = 0;                    \
    if (BloombergLP::baem_DefaultMetricsManager::instance()) {                \
       using namespace BloombergLP;                                           \
       if (0 == VARIABLE_NAME) {                                              \
           baem_CollectorRepository& repository =                             \
              baem_DefaultMetricsManager::instance()->collectorRepository();  \
           VARIABLE_NAME = repository.getDefaultCollector((CATEGORY),         \
                                                          (METRIC));          \
       }                                                                      \
    }                                                                         \
    else {                                                                    \
       VARIABLE_NAME = 0;                                                     \
    }                                                                         \
    BloombergLP::baem_StopwatchScopedGuard                                    \
         BAEM_METRICS__UNIQUE_NAME(__bAeM_gUaRd)(VARIABLE_NAME, TIME_UNITS);

// Declare a pointer to a 'baem_Collector' with the specified 'VARIABLE_NAME'.
// If the default metrics manager is available, assign to the declared pointer
// variable (named 'VARIABLE_NAME') the address of a collector for the
// specified 'CATEGORY' and 'METRIC'.  Finally, declare a
// 'baem_StopwatchScopedGuard' object with a unique variable name and supply
// its constructor the collector address held in 'VARIABLE_NAME' and the
// specified 'TIME_UNITS'.
#define BAEM_METRICS__DYNAMIC_TIME_BLOCK_IMP(CATEGORY,                        \
                                             METRIC,                          \
                                             TIME_UNITS,                      \
                                             VARIABLE_NAME)                   \
    BloombergLP::baem_Collector *VARIABLE_NAME = 0;                           \
    if (BloombergLP::baem_DefaultMetricsManager::instance()) {                \
        using namespace BloombergLP;                                          \
        baem_CollectorRepository& repository =                                \
             baem_DefaultMetricsManager::instance()->collectorRepository();   \
        VARIABLE_NAME = repository.getDefaultCollector((CATEGORY),            \
                                                       (METRIC));             \
    }                                                                         \
    BloombergLP::baem_StopwatchScopedGuard                                    \
         BAEM_METRICS__UNIQUE_NAME(__bAeM_gUaRd)(VARIABLE_NAME, TIME_UNITS);

                        // ------------------------
                        // Unique line number macro
                        // ------------------------

#if defined(BSLS_PLATFORM__CMP_MSVC)
// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)
// Fortunately the __COUNTER__ extension provided by MSVC is even better.
#   define BAEM_METRICS__UNIQNUM __COUNTER__
#else
#   define BAEM_METRICS__UNIQNUM __LINE__
#endif

                        // ----------------------------------
                        // Token concatenation support macros
                        // ----------------------------------

// Second layer needed to ensure that arguments are expanded before
// concatenation.
#define BAEM_METRICS__CAT(X, Y) BAEM_METRICS__CAT_IMP(X, Y)
#define BAEM_METRICS__CAT_IMP(X, Y) X##Y

                        // ----------------------------------
                        // Unique variable name support macro
                        // ----------------------------------

// Create a unique variable name by concatenating the specified 'X' string
// with a unique integer value.
#define BAEM_METRICS__UNIQUE_NAME(X)                                          \
           BAEM_METRICS__CAT(X, BAEM_METRICS__UNIQNUM)

namespace BloombergLP {

                        // ==========================
                        // struct baem_Metrics_Helper
                        // ==========================

struct baem_Metrics_Helper {
    // This 'struct' provides a namespace for a suite of functions used in the
    // implementation of the macros defined in this component.
    //
    // This type is an implementation detail and *must* *not* be used
    // (directly) by clients outside of this component.

    // TYPES
    enum NameType {
        // Enumeration indicating the type of identifier supplied to
        // 'logEmptyName'.

        TYPE_CATEGORY = 0,
        TYPE_METRIC   = 1
    };

    // CLASS METHODS
    static void initializeCategoryHolder(baem_CategoryHolder *holder,
                                         const char          *category);
        // Load into the specified 'holder' the address and enabled status of
        // the specified 'category', and add 'holder' to the list of category
        // holders for 'category'.  The behavior is undefined unless the baem
        // metrics manager singleton is valid.

    static baem_Collector *getCollector(const char *category,
                                        const char *metric);
        // Return the address of the default metrics collector for the metric
        // identified by the specified 'category' and 'metric' names.  The
        // behavior is undefined unless the 'baem' metrics manager singleton is
        // valid.

    static baem_IntegerCollector *getIntegerCollector(const char *category,
                                                      const char *metric);
        // Return the address of the default integer metrics collector for the
        // metric identified by the specified 'category' and 'metric' names.
        // The behavior is undefined unless the 'baem' metrics manager
        // singleton is valid.

    static void setPublicationType(const baem_MetricId&        id,
                                   baem_PublicationType::Value type);
        // Set the publication type for the metric identified by the specified
        // 'id' to the specified 'type'.  The behavior is undefined unless the
        // 'baem' metrics manager singleton is valid, and 'id' is a valid
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

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // --------------------------
                      // struct baem_Metrics_Helper
                      // --------------------------

// CLASS METHODS
inline
void baem_Metrics_Helper::initializeCategoryHolder(
                                         baem_CategoryHolder *holder,
                                         const char          *category)
{
    baem_MetricsManager  *manager  = baem_DefaultMetricsManager::instance();
    baem_MetricRegistry&  registry = manager->metricRegistry();
    registry.registerCategoryHolder(registry.getCategory(category), holder);
}

inline
baem_Collector *baem_Metrics_Helper::getCollector(const char *category,
                                                  const char *metric)
{
   baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
   return manager->collectorRepository().getDefaultCollector(category,
                                                             metric);
}

inline
baem_IntegerCollector *baem_Metrics_Helper::getIntegerCollector(
                                                          const char *category,
                                                          const char *metric)
{
   baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
   return manager->collectorRepository().getDefaultIntegerCollector(category,
                                                                    metric);
}

inline
void baem_Metrics_Helper::setPublicationType(const baem_MetricId&        id,
                                             baem_PublicationType::Value type)
{
   baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
   return manager->metricRegistry().setPreferredPublicationType(id, type);

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
