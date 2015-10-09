// balm_configurationutil.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_CONFIGURATIONUTIL
#define INCLUDED_BALM_CONFIGURATIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for metrics configuration utilities.
//
//@CLASSES:
//    balm::ConfigurationUtil: namespace for metrics configuration utilities
//
//@SEE_ALSO: balm_metricsmanager, balm_defaultmetricsmanager
//
//@DESCRIPTION: This component provides a set of utility functions for
// configuring metrics.  The 'balm::ConfigurationUtil' 'struct' provides
// short-cuts for common configuration operations that are performed on other
// components in the 'balm' package.
//
///Thread Safety
///-------------
// 'balm::ConfigurationUtil' is fully *thread-safe*, meaning that all the
// methods can be safely invoked simultaneously from multiple threads.
//
///Usage
///-----
// The following examples demonstrate how to use 'balm::ConfigurationUtil'.
//
///Configuring the Output of a Metric
/// - - - - - - - - - - - - - - - - -
// This example uses 'balm::ConfigurationUtil' to configure the output for a
// metric.
//
// We start by initializing a default metrics manager by creating a
// 'balm::DefaultMetricsManagerScopedGuard', which manages the lifetime of the
// default metrics manager object.  At construction, we provide the scoped
// guard an output stream ('stdout') to which the default metrics manager will
// publish metrics.  Note that the default metrics manager is intended to be
// created and destroyed by the *owner* of 'main'.  A metrics manager should
// be created during the initialization of an application (while the task has
// a single thread) and destroyed just prior to termination (when there is
// similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//
//      balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Next we create a metric, "avgElapsedTimeMs", that will output the average
// time, in milliseconds, spent in a section of code.  We set the preferred
// publication type for the metric to be average:
//..
//      balm::ConfigurationUtil::setPreferredPublicationType(
//                                               "myCategory",
//                                               "avgElapsedTimeMs",
//                                               balm::PublicationType::e_AVG);
//..
// Next, because we will record the elapsed time in seconds, we configure a
// format to scale the elapsed time by 1000.0:
//..
//     balm::ConfigurationUtil::setFormatSpec(
//                                   "myCategory",
//                                   "avgElapsedTimeMs",
//                                   balm::PublicationType::e_AVG,
//                                   balm::MetricFormatSpec(1000.0, "%.2f ms");
//..
// We now collect an example value of .005:
//..
//     BALM_METRIC_UPDATE("myCategory", "avgElapsedTimeMs", .005);
//..
// Finally, we publish the metric.  Note that in practice, clients of the
// 'balm' package can use the 'balm::PublicationScheduler' to schedule the
// periodic publication of metrics:
//..
//  balm::DefaultMetricsManager::instance()->publishAll();
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
// In the following example we configure, using 'balm::ConfigurationUtil',
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
//  class ThresholdPublisher : public balm::Publisher {
//      // A simple implementation of the 'balm::Publisher' protocol that
//      // writes metric records to the console when their value is greater
//      // than an application-specific threshold.
//
//      // DATA
//      balm::MetricDescription::UserDataKey d_thresholdKey;  // key for a
//                                                            // metric's
//                                                            // threshold
//
//      // NOT IMPLEMENTED
//      ThresholdPublisher(const ThresholdPublisher&);
//      ThresholdPublisher& operator=(const ThresholdPublisher&);
//
//    public:
//      // CREATORS
//      ThresholdPublisher(balm::MetricDescription::UserDataKey thresholdKey);
//          // Create a publisher that will publish metrics to the console if
//          // their total value is greater than their associated threshold,
//          // accessed via the specified 'thresholdKey'.
//
//      virtual ~ThresholdPublisher();
//           // Destroy this publisher.
//
//      // MANIPULATORS
//      virtual void publish(const balm::MetricSample& metricValues);
//          // Publish the specified 'metricValues' to the console if they are
//          // greater than their associated threshold.
//  };
//
//  // thresholdpublisher.cpp
//
//  // CREATORS
//  ThresholdPublisher::ThresholdPublisher(
//                           balm::MetricDescription::UserDataKey thresholdKey)
//  : d_thresholdKey(thresholdKey)
//  {
//  }
//
//  ThresholdPublisher::~ThresholdPublisher()
//  {
//  }
//
//  // MANIPULATORS
//  void ThresholdPublisher::publish(const balm::MetricSample& metricValues)
//  {
//      if (0 >= metricValues.numRecords()) {
//          return;                                                   // RETURN
//      }
//      balm::MetricSample::const_iterator sIt = metricValues.begin();
//      for (; sIt != metricValues.end(); ++sIt) {
//          balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
//          for (; gIt != sIt->end(); ++gIt) {
//..
// We now use the user data key to lookup the address of the threshold value.
// If this address is 0, no threshold is specified for the metric.
//..
//              const balm::MetricDescription& description =
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
// 'balm::DefaultMetricsManagerScopedGuard', which manages the lifetime of the
// default metrics manager:
//..
//  int main(int argc, char *argv[])
//  {
//      // ...
//      bslma::Allocator *allocator = bslma::Default::allocator(0);
//      balm::DefaultMetricsManagerScopedGuard managerGuard;
//..
// Now we create a user data key for our threshold information:
//..
//      balm::MetricDescription::UserDataKey thresholdKey =
//                                balm::ConfigurationUtil::createUserDataKey();
//..
// Next we create an object of our application-specific publisher type,
// 'ThresholdPublisher', and configure the default metrics manager to publish
// metrics using this publisher:
//..
//      bsl::shared_ptr<balm::Publisher> publisher(
//            new (*allocator) ThresholdPublisher(thresholdKey),
//            allocator);
//     balm::DefaultMetricsManager::instance()->addGeneralPublisher(publisher);
//..
// Next we configure two metric thresholds:
//..
//      balm::ConfigurationUtil::setUserData("myCategory",
//                                          "elapsedTime",
//                                          thresholdKey,
//                                          &ELAPSED_TIME_THRESHOLD);
//      balm::ConfigurationUtil::setUserData("myCategory",
//                                          "numRequests",
//                                          thresholdKey,
//                                          &NUM_REQUESTS_THRESHOLD);
//..
// Now we update the value of a couple of metrics.  Note that the recorded
// number of requests is greater than the metric's configured threshold:
//..
//      BALM_METRICS_UPDATE("myCategory", "elapsedTime", 2);
//      BALM_METRICS_UPDATE("myCategory", "numRequests", 150);
//..
// Finally, we publish the collected metrics.  Note that in practice, clients
// of the 'balm' package can use the 'balm::PublicationScheduler' to schedule
// the periodic publication of metrics:
//..
//  balm::DefaultMetricsManager::instance()->publishAll();
//..
// The console output of the call to 'publishAll' will look like:
//..
//  WARNING: myCategory.numRequests = 150
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_METRICDESCRIPTION
#include <balm_metricdescription.h>
#endif

#ifndef INCLUDED_BALM_PUBLICATIONTYPE
#include <balm_publicationtype.h>
#endif

namespace BloombergLP {


namespace balm {class MetricFormat;
class MetricFormatSpec;
class MetricsManager;

                          // ========================
                          // struct ConfigurationUtil
                          // ========================

struct ConfigurationUtil {
    // This 'struct' provides utilities for configuring metrics.

    // CLASS METHODS
    static int setFormat(const char          *category,
                         const char          *metricName,
                         const MetricFormat&  format,
                         MetricsManager      *manager = 0);
        // Set the format specification for the metric indicated by the
        // specified 'category' and 'metricName' to the specified 'format'.
        // Optionally specify a metrics 'manager' to configure.  If 'manager'
        // is 0, configure the default metrics manager; if 'manager' is 0 and
        // the default metrics manager has not been initialized, this method
        // has no effect.  Return 0 on success, or a non-zero value if
        // 'manager' is 0 and the default metrics manager has not been
        // initialized.  If a 'MetricId' does not exist for 'category' and
        // 'metricName', create one and add it to the metric registry of the
        // indicated metrics manager.

    static int setFormatSpec(const char              *category,
                             const char              *metricName,
                             PublicationType::Value   publicationType,
                             const MetricFormatSpec&  formatSpec,
                             MetricsManager          *manager = 0);
        // Set the format specification for the metric aggregate indicated by
        // the specified 'category', 'metricName', and 'publicationType' to
        // the specified 'formatSpec'.  Optionally specify a metrics 'manager'
        // to configure.  If 'manager' is 0, configure the default metrics
        // manager; if 'manager' is 0 and the default metrics manager has not
        // been initialized, this method has no effect.  Return 0 on success,
        // or a non-zero value if 'manager' is 0 and the default metrics
        // manager has not been initialized.  If a 'MetricId' does not exist
        // for 'category' and 'metricName', create one and add it to the metric
        // registry of the indicated metrics manager.  For example a
        // publication type of 'e_AVG', and a format spec with a scale of
        // 1000.0 and a format of "%.2f ms", indicates that the average value
        // of the indicated metric should be formatted by scaling the value by
        // 1000 and then rounding the value to the second decimal place and
        // appending " ms".

    static int setPreferredPublicationType(
                               const char             *category,
                               const char             *metricName,
                               PublicationType::Value  publicationType,
                               MetricsManager         *manager = 0);
        // Set the preferred publication type of the metric identified by the
        // specified 'category' and 'metricName' to the specified
        // 'publicationType'.  Optionally specify a metrics 'manager' to
        // configure.  If 'manager' is 0, configure the default metrics
        // manager; if 'manager' is 0 and the default metrics manager has not
        // been initialized, this method has no effect.  Return 0 on success,
        // or a non-zero value if 'manager' is 0 and the default metrics
        // manager has not been initialized.  The preferred publication type of
        // a metric indicates the preferred aggregate to publish for that
        // metric, or 'PublicationType::e_UNSPECIFIED' if there is no
        // preference.  For example, specifying 'e_AVG' indicates that the
        // average value of the collected metric should be reported.  If a
        // 'MetricId' does not exist for 'category' and 'metricName', create
        // one and add it to the metric registry of the indicated metrics
        // manager.  Note that there is no uniform definition for how
        // publishers will interpret this value.

    static MetricDescription::UserDataKey createUserDataKey(
                                                  MetricsManager *manager = 0);
        // Return a new unique key that can be used to associate (via
        // 'setUserData') a value with a metric (or group of metrics).
        // Optionally specify a metrics 'manager' to configure.  If 'manager'
        // is 0, configure the default metrics manager; if 'manager' is 0 and
        // the default metrics manager has not been initialized, then an
        // unspecified integer value is returned.  Note that the returned key
        // can be used by clients of 'balm' to associate additional
        // information with a metric.

    static void setUserData(const char                     *category,
                            const char                     *metricName,
                            MetricDescription::UserDataKey  key,
                            const void                     *value,
                            MetricsManager                 *manager = 0);
        // Associate the specified 'value' with the specified data 'key' in the
        // description of the metric having the specified 'category' and
        // 'metricName'.  Optionally specify a metrics 'manager' to configure.
        // If 'manager' is 0, configure the default metrics manager; if
        // 'manager' is 0 and the default metrics manager has not been
        // initialized, this method has no effect.  If a 'MetricId' does not
        // exist for the 'category' and 'metricName', create one and add it to
        // the metric registry of the indicated metrics manager.  The behavior
        // is undefined unless 'key' was previously created for the indicated
        // metrics manager's metrics registry (e.g., by calling
        // 'createUserDataKey').  Note that this method allows clients of
        // 'balm' to associate (opaque) application-specific information with a
        // metric.

    static void setUserData(const char                     *categoryName,
                            MetricDescription::UserDataKey  key,
                            const void                     *value,
                            MetricsManager                 *manager = 0);
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
        // is non-null, in which case the unique non-null value is used.  The
        // behavior is undefined unless 'key' was previously created for the
        // indicated metrics manager's metrics registry (e.g., by calling
        // 'createUserDataKey').
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

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
