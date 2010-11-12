// baem_configurationutil.h              -*-C++-*-
#ifndef INCLUDED_BAEM_CONFIGURATIONUTIL
#define INCLUDED_BAEM_CONFIGURATIONUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for metrics configuration utilities.
//
//@CLASSES:
//    baem_ConfigurationUtil: namespace for metrics configuration utilities
//
//@SEE_ALSO: baem_metricsmanager, baem_defaultmetricsmanager
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a set of utility functions for
// configuring metrics.  The 'baem_ConfigurationUtil' 'struct' provides
// short-cuts for common configuration operations that are performed on other
// components in the 'baem' package.
//
///Thread Safety
///-------------
// 'baem_ConfigurationUtil' is fully *thread-safe*, meaning that all
// the methods can be safely invoked simultaneously from multiple threads.
//
///Usage
///-----
// The following examples demonstrate how to use 'baem_ConfigurationUtil'.
//
///Configuring the Output of a Metric
/// - - - - - - - - - - - - - - - - -
// This example uses 'baem_ConfigurationUtil' to configure the output for a
// metric.
//
// We start by initializing a default metrics manager by creating a
// 'baem_DefaultMetricsManagerScopedGuard', which manages the lifetime of the
// default metrics manager object.  At construction, we provide the scoped
// guard an output stream ('stdout') to which the default metrics manager will
// publish metrics.  Note that the default metrics manager is intended to be
// created and destroyed by the *owner* of 'main'.  A metrics manager
// should be created during the initialization of an application (while the
// task has a single thread) and destroyed just prior to termination (when
// there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//
//      baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Next we create a metric, "avgElapsedTimeMs", that will output the average
// time, in milliseconds, spent in a section of code.  We set the preferred
// publication type for the metric to be average:
//..
//      baem_ConfigurationUtil::setPreferredPublicationType(
//                                             "myCategory",
//                                             "avgElapsedTimeMs",
//                                             baem_PublicationType::BAEM_AVG);
//..
// Next, because we will record the elapsed time in seconds, we configure a
// format to scale the elapsed time by 1000.0:
//..
//     baem_ConfigurationUtil::setFormatSpec(
//                                    "myCategory",
//                                    "avgElapsedTimeMs",
//                                    baem_PublicationType::BAEM_AVG,
//                                    baem_MetricFormatSpec(1000.0, "%.2f ms");
//..
// We now collect an example value of .005:
//..
//     BAEM_METRIC_UPDATE("myCategory", "avgElapsedTimeMs", .005);
//..
// Finally, we publish the metric.  Note that in practice, clients of
// the 'baem' package can use the 'baem_PublicationScheduler' to schedule the
// periodic publication of metrics:
//..
//  baem_DefaultMetricsManager::instance()->publishAll();
//..
// The output for the publication will look like:
//..
//  06AUG2009_20:27:51.982+0000 1 Records
//          Elapsed Time: 0.000816s
//                  myCategory.avgElapsedTimeMs[ avg (total/count) = 5.00 ms ]
//..
//
///Using a Metric's User Data
/// - - - - - - - - - - - - -
// In the following example we configure, using 'baem_ConfigurationUtil',
// application-specific publication thresholds for a series of metrics.  We
// will create an application-specific publisher that will use the configured
// thresholds to determine whether a metric should be written to the console.
// For simplicity, the metric thresholds in this example will be a single
// unsigned integer value that will be compared with the metric's total.
//
// We start by defining an application-specific publisher implementation.
// This implementation is supplied a user data key on construction, which it
// uses to look up the threshold for a particular metric.  If a metric's total
// value is greater than its threshold, it will log the metric to the console.
//..
//  // thresholdpublisher.h
//  class ThresholdPublisher : public baem_Publisher {
//      // A simple implementation of the 'baem_Publisher' protocol that
//      // writes metric records to the console when their value is greater
//      // than an application-specific threshold.
//
//      // DATA
//      baem_MetricDescription::UserDataKey d_thresholdKey;  // key for a
//                                                           // metric's
//                                                           // threshold
//
//      // NOT IMPLEMENTED
//      ThresholdPublisher(const ThresholdPublisher&);
//      ThresholdPublisher& operator=(const ThresholdPublisher&);
//
//    public:
//      // CREATORS
//      ThresholdPublisher(baem_MetricDescription::UserDataKey thresholdKey);
//          // Create a publisher that will publish metrics to the console if
//          // their total value is greater than their associated threshold,
//          // accessed via the specified 'thresholdKey'.
//
//      virtual ~ThresholdPublisher();
//           // Destroy this publisher.
//
//      // MANIPULATORS
//      virtual void publish(const baem_MetricSample& metricValues);
//          // Publish the specified 'metricValues' to the console if they are
//          // greater than their associated threshold.
//  };
//
//  // thresholdpublisher.cpp
//
//  // CREATORS
//  ThresholdPublisher::ThresholdPublisher(
//                            baem_MetricDescription::UserDataKey thresholdKey)
//  : d_thresholdKey(thresholdKey)
//  {
//  }
//
//  ThresholdPublisher::~ThresholdPublisher()
//  {
//  }
//
//  // MANIPULATORS
//  void ThresholdPublisher::publish(const baem_MetricSample& metricValues)
//  {
//      if (0 >= metricValues.numRecords()) {
//          return;                                                   // RETURN
//      }
//      baem_MetricSample::const_iterator sIt = metricValues.begin();
//      for (; sIt != metricValues.end(); ++sIt) {
//          baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
//          for (; gIt != sIt->end(); ++gIt) {
//..
// We now use the user data key to lookup the address of the threshold value.
// If this address is 0, no threshold is specified for the metric.
//..
//              const baem_MetricDescription& description =
//                                              *gIt->metricId().description();
//              unsigned int *thresholdPtr =
//                       (unsigned int *)description.userData(d_thresholdKey);
//              if (thresholdPtr && gIt->total() > *thresholdPtr) {
//                  bsl::cout << "WARNING: " << gIt->metricId()
//                            << " = "       << gIt->total()
//                            << bsl::endl;
//              }
//          }
//      }
//  }
//..
// Now we examine how to configure a metrics manager with a
// 'ThresholdPublisher', and set the thresholds for a couple of metrics.  We
// start by defining a couple of threshold constants for our metrics:
//..
//  static const unsigned int ELAPSED_TIME_THRESHOLD = 10;
//  static const unsigned int NUM_REQUESTS_THRESHOLD = 100;
//..
// Now, we configure a default metrics manager and publish a couple of example
// metrics.  We start by initializing a default metrics manager by creating a
// 'baem_DefaultMetricsManagerScopedGuard', which manages the lifetime of the
// default metrics manager:
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//      bslma_Allocator *allocator = bslma_Default::allocator(0);
//      baem_DefaultMetricsManagerScopedGuard managerGuard;
//..
// Now we create a user data key for our threshold information:
//..
//      baem_MetricDescription::UserDataKey thresholdKey =
//                                 baem_ConfigurationUtil::createUserDataKey();
//..
// Next we create an object of our application-specific publisher type,
// 'ThresholdPublisher', and configure the default metrics manager to publish
// metrics using this publisher:
//..
//      bcema_SharedPtr<baem_Publisher> publisher(
//            new (*allocator) ThresholdPublisher(thresholdKey),
//            allocator);
//      baem_DefaultMetricsManager::instance()->addGeneralPublisher(publisher);
//..
// Next we configure two metric thresholds:
//..
//      baem_ConfigurationUtil::setUserData("myCategory",
//                                          "elapsedTime",
//                                          thresholdKey,
//                                          &ELAPSED_TIME_THRESHOLD);
//      baem_ConfigurationUtil::setUserData("myCategory",
//                                          "numRequests",
//                                          thresholdKey,
//                                          &NUM_REQUESTS_THRESHOLD);
//..
// Now we update the value of a couple of metrics.  Note that the recorded
// number of requests is greater than the metric's configured threshold:
//..
//      BAEM_METRICS_UPDATE("myCategory", "elapsedTime", 2);
//      BAEM_METRICS_UPDATE("myCategory", "numRequests", 150);
//..
// Finally, we publish the collected metrics.  Note that in practice, clients
// of the 'baem' package can use the 'baem_PublicationScheduler' to schedule
// the periodic publication of metrics:
//..
//  baem_DefaultMetricsManager::instance()->publishAll();
//..
// The console output of the call to 'publishAll' will look like:
//..
//  WARNING: myCategory.numRequests = 150
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_METRICDESCRIPTION
#include <baem_metricdescription.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

namespace BloombergLP {

class baem_MetricFormat;
class baem_MetricFormatSpec;
class baem_MetricsManager;

                      // =============================
                      // struct baem_ConfigurationUtil
                      // =============================

struct baem_ConfigurationUtil {
    // This 'struct' provides utilities for configuring metrics.

    // CLASS METHODS
    static int setFormat(const char               *category,
                         const char               *metricName,
                         const baem_MetricFormat&  format,
                         baem_MetricsManager      *manager = 0);
        // Set the format specification for the metric indicated by the
        // specified 'category' and 'metricName' to the specified 'format'.
        // Optionally specify a metrics 'manager' to configure.  If 'manager'
        // is 0, configure the default metrics manager; if 'manager' is 0 and
        // the default metrics manager has not been initialized, this method
        // has no effect.  Return 0 on success, or a non-zero value if
        // 'manager' is 0 and the default metrics manager has not been
        // initialized.  If a 'baem_MetricId' does not exist for the specified
        // 'category' and 'metricName', create one and add it to the metric
        // registry of the indicated metrics manager.

    static int setFormatSpec(const char                   *category,
                             const char                   *metricName,
                             baem_PublicationType::Value   publicationType,
                             const baem_MetricFormatSpec&  formatSpec,
                             baem_MetricsManager          *manager = 0);
        // Set the format specification for the metric aggregate indicated by
        // the specified 'category', 'metricName', and 'publicationType' to
        // the specified 'formatSpec'.  Optionally specify a metrics 'manager'
        // to configure.  If 'manager' is 0, configure the default metrics
        // manager; if 'manager' is 0 and the default metrics manager has not
        // been initialized, this method has no effect.  Return 0 on success,
        // or a non-zero value if 'manager' is 0 and the default metrics
        // manager has not been initialized.  If a 'baem_MetricId' does not
        // exist for the specified 'category' and 'metricName', create one
        // and add it to the metric registry of the indicated metrics
        // manager.  For example, a publication type of 'BAEM_AVG', and a
        // format spec with a scale of 1000.0 and a format of "%.2f ms",
        // indicates that the average value of the indicated metric should be
        // formatted by scaling the value by 1000 and then rounding the value
        // to the second decimal place and appending " ms".

    static int setPreferredPublicationType(
                               const char                  *category,
                               const char                  *metricName,
                               baem_PublicationType::Value  publicationType,
                               baem_MetricsManager         *manager = 0);
        // Set the preferred publication type of the metric identified by the
        // specified 'category' and 'metricName' to the specified
        // 'publicationType'.  Optionally specify a metrics 'manager' to
        // configure.  If 'manager' is 0, configure the default metrics
        // manager; if 'manager' is 0 and the default metrics manager has not
        // been initialized, this method has no effect.  Return 0 on success,
        // or a non-zero value if 'manager' is 0 and the default metrics
        // manager has not been initialized.  The preferred publication type of
        // a metric indicates the preferred aggregate to publish for that
        // metric, or 'baem_PublicationType::BAEM_UNSPECIFIED' if there is no
        // preference.  For example, specifying 'BAEM_AVG' indicates that the
        // average value of the collected metric should be reported.  If a
        // 'baem_MetricId' does not exist for the specified 'category' and
        // 'metricName', create one and add it to the metric registry of the
        // indicated metrics manager.  Note that there is no uniform
        // definition for how publishers will interpret this value.

    static baem_MetricDescription::UserDataKey createUserDataKey(
                                             baem_MetricsManager *manager = 0);
        // Return a new unique key that can be used to associate (via
        // 'setUserData') a value with a metric (or group of metrics).
        // Optionally specify a metrics 'manager' to configure.  If 'manager'
        // is 0, configure the default metrics manager; if 'manager' is 0 and
        // the default metrics manager has not been initialized, then an
        // unspecified integer value is returned.  Note that the returned key
        // can be used by clients of 'baem' to associate additional
        // information with a metric.

    static void setUserData(const char                          *category,
                            const char                          *metricName,
                            baem_MetricDescription::UserDataKey  key,
                            const void                          *value,
                            baem_MetricsManager                 *manager = 0);
        // Associate the specified 'value' with the specified data 'key' in the
        // description of the metric having the specified 'category' and
        // 'metricName'.  Optionally specify a metrics 'manager' to configure.
        // If 'manager' is 0, configure the default metrics manager; if
        // 'manager' is 0 and the default metrics manager has not been
        // initialized, this method has no effect.  If a 'baem_MetricId' does
        // not exist for the specified 'category' and 'metricName', create one
        // and add it to the metric registry of the indicated metrics manager.
        // The behavior is undefined unless 'key' was previously created for
        // the indicated metrics manager's metrics registry (e.g., by calling
        // 'createUserDataKey').  Note that this method allows clients of
        // 'baem' to associate (opaque) application-specific information with
        // a metric.

    static void setUserData(const char                          *categoryName,
                            baem_MetricDescription::UserDataKey  key,
                            const void                          *value,
                            baem_MetricsManager                 *manager = 0);
        // Associate the specified 'value' with the specified data 'key' in
        // any metric belonging to a category having the specified
        // 'categoryName', or, if 'categoryName' ends with an asterisk ('*'),
        // any metric belonging to a category whose name begins with
        // 'categoryName' (without the asterisk).  Optionally specify a
        // metrics 'manager' to configure.  If 'manager' is 0, configure the
        // default metrics manager; if 'manager' is 0 and the default metrics
        // manager has not been initialized, this method has no effect.  This
        // association applies to existing metrics as well as any subsequently
        // created ones.  When a metric is created that matches more than one
        // registered category prefix, it is not specified which supplied
        // value will be associated with 'key', unless only one of those values
        // is non-null, in which case the unique non-null value is used.
        // The behavior is undefined unless 'key' was previously created for
        // the indicated metrics manager's metrics registry (e.g., by calling
        // 'createUserDataKey').
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
