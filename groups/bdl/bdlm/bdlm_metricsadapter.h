// bdlm_metricsadapter.h                                              -*-C++-*-
#ifndef INCLUDED_BDLM_METRICSADAPTER
#define INCLUDED_BDLM_METRICSADAPTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an abstract interface for metrics registration mechanisms.
//
//@CLASSES:
//  bdlm::MetricsAdapter: protocol class for registration and de-registration
//
//@DESCRIPTION: This component provides the base-level protocol (pure abstract
// interface) class, 'bdlm::MetricsAdapter', that serves as a ubiquitous
// vocabulary type for various metrics registration mechanisms.  Clients of
// this abstract base class will typically accept a supplied metrics adapter
// (often at construction) and use its 'registerCollectionCallback' and
// 'removeCollectionCallback' methods to register a collection object with a
// monitoring mechanism and remove a collection object from monitoring,
// respectively.
//
// The 'registerCollectionCallback' method supplies a metric descriptor and a
// collection callback to a monitoring system.  Specific monitoring systems may
// adapt the provided metric descriptor attributes to their needs.
//
///Thread Safety
///-------------
// This class is *minimally* *thread-safe* (see {'bsldoc_glossary'|Minimally
// Thread-Safe}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing the 'bdlm::MetricsAdapter' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates an elided concrete implementation of the
// 'bdlm::MetricsAdapter' protocol that allows for registering metric callback
// functions with a monitoring system.
//
// First, we define the interface of a limited 'my_MetricsMonitor' class that
// allows only one metric collection function to be registered:
//..
//  class my_MetricsMonitor {
//      // This class implements a metric monitoring system.
//
//      // ...
//
//      // DATA
//      bdlm::Metric                   d_value;     // metric supplied to
//                                                  // 'd_callback'
//
//      bsl::string                    d_name;      // register metric name
//
//      bdlm::MetricsAdapter::Callback d_callback;  // registered callback
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      bdlm::MetricsAdapter::CallbackHandle registerCallback(
//                              const bsl::string&                   name,
//                              const bdlm::MetricsAdapter::Callback callback);
//          // Register the specified 'callback' with this monitoring system,
//          // using the specified 'name' for display purposes.  Return a
//          // callback handle to be used with 'removeCallback'.
//
//      int removeCallback(const bdlm::MetricsAdapter::CallbackHandle& handle);
//          // Remove the callback associated with the specified 'handle'.
//          // Return 0 on success, or a non-zero value if 'handle' cannot be
//          // found.
//
//      void update();
//          // Invoke the registered callback.
//
//      // ACCESSORS
//      const bsl::string& name() const;
//          // Return the name of the registered metric.
//
//      double value() const;
//          // Return the value computed by the invocations of the registered
//          // callback.
//  };
//..
// Then, we implement the functions:
//..
//  // MANIPULATORS
//  bdlm::MetricsAdapter::CallbackHandle my_MetricsMonitor::registerCallback(
//                               const bsl::string&                   name,
//                               const bdlm::MetricsAdapter::Callback callback)
//  {
//      d_value    = bdlm::Metric::Gauge(0.0);
//      d_name     = name;
//      d_callback = callback;
//
//      return 1;
//  }
//
//  int my_MetricsMonitor::removeCallback(
//                          const bdlm::MetricsAdapter::CallbackHandle& handle)
//  {
//      (void)handle;
//      return 0;
//  }
//
//  void my_MetricsMonitor::update()
//  {
//      d_callback(&d_value);
//  }
//
//  // ACCESSORS
//  const bsl::string& my_MetricsMonitor::name() const
//  {
//      return d_name;
//  }
//
//  double my_MetricsMonitor::value() const
//  {
//      return d_value.theGauge();
//  }
//..
// Next, we define the implementation class of the 'bdlm::MetricsAdapter'
// protocol:
//..
//  class my_MetricsAdapter : public bdlm::MetricsAdapter {
//      // This class implements an interface for clients and suppliers of
//      // metrics adapters.
//
//      // DATA
//      my_MetricsMonitor *d_monitor_p;  // pointer to monitor to use for
//                                       // metrics (held not owned)
//
//    public:
//      // CREATORS
//      my_MetricsAdapter(my_MetricsMonitor *monitor);
//          // Create a 'my_MetricsAdapter' using the specified 'monitor' for
//          // registered callbacks.
//
//      ~my_MetricsAdapter();
//          // Destroy this object.
//
//      // MANIPULATORS
//      CallbackHandle registerCollectionCallback(
//                              const bdlm::MetricDescriptor& metricDescriptor,
//                              const Callback&               callback);
//          // Register the specified 'callback' with a monitoring system,
//          // using the specified 'metricDescriptor' for the registration.
//          // Return the callback handle to be used with
//          // 'removeCollectionCallback'.  Note the information used for
//          // registration is implementation dependant, and may involve values
//          // computed from the supplied arguments.
//
//      int removeCollectionCallback(const CallbackHandle& handle);
//          // Remove the callback associated with the specified 'handle'.
//          // Return 0 on success, or a non-zero value if 'handle' cannot be
//          // found.
//  };
//..
// Then, we implement the methods of 'myMetricsAdapter':
//..
//  // CREATORS
//  my_MetricsAdapter::my_MetricsAdapter(my_MetricsMonitor *monitor)
//  : d_monitor_p(monitor)
//  {
//  }
//
//  my_MetricsAdapter::~my_MetricsAdapter()
//  {
//  }
//
//  // MANIPULATORS
//  bdlm::MetricsAdapter::CallbackHandle
//                               my_MetricsAdapter::registerCollectionCallback(
//                              const bdlm::MetricDescriptor& metricDescriptor,
//                              const Callback&               callback)
//  {
//      bsl::string name = metricDescriptor.metricNamespace() + '.'
//                       + metricDescriptor.metricName() + '.'
//                       + metricDescriptor.objectTypeName() + '.'
//                       + metricDescriptor.objectTypeAbbreviation() + '.'
//                       + metricDescriptor.objectIdentifier();
//
//      return d_monitor_p->registerCallback(name, callback);
//  }
//
//  int my_MetricsAdapter::removeCollectionCallback(
//                          const bdlm::MetricsAdapter::CallbackHandle& handle)
//  {
//      return d_monitor_p->removeCallback(handle);
//  }
//..
// Next, we provide the metric method, 'my_metric', which will compute its
// invocation count:
//..
//  void my_metric(BloombergLP::bdlm::Metric *value)
//  {
//      *value = value->theGauge() + 1.0;
//  }
//..
// Then, we instantiate a 'my_MetricsMonitor' and a 'myMetricsAdapter':
//..
//      my_MetricsMonitor monitor;
//      my_MetricsAdapter adapter(&monitor);
//..
// Next, we construct a 'bdlm::MetricDescriptor', register the 'my_metric'
// method with the 'monitor', and verify the 'monitor' has the expected name
// for the metric:
//..
//      bdlm::MetricDescriptor descriptor("a", "b", 1, "c", "d", "e");
//
//      adapter.registerCollectionCallback(descriptor, my_metric);
//
//      assert(monitor.name() == "a.b.c.d.e");
//..
// Now, we invoke the 'update' method a few times:
//..
//      monitor.update();
//      monitor.update();
//      monitor.update();
//..
// Finally, we verify the metric has the expected value:
//..
//      assert(monitor.value() == 3.0);
//..

#include <bdlscm_version.h>

#include <bdlm_metric.h>

#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlm {

class MetricDescriptor;

                           // ====================
                           // class MetricsAdapter
                           // ====================

class MetricsAdapter {
    // This protocol class provides a pure abstract interface and contract for
    // clients and suppliers of metrics adapters.

  public:
    // TYPES
    typedef int                            CallbackHandle;  // identifies a
                                                            // callback functor

    typedef bsl::function<void (Metric *)> Callback;        // callback functor

  public:
    // CREATORS
    virtual ~MetricsAdapter() = 0;
        // Destroy this 'MetricsAdapter'.

    // MANIPULATORS
    virtual CallbackHandle registerCollectionCallback(
                                      const MetricDescriptor& metricDescriptor,
                                      const Callback&         callback) = 0;
        // Register the specified 'callback' with a monitoring system, using
        // the specified 'metricDescriptor' for the registration.  Return the
        // callback handle to be used with 'removeCollectionCallback'.  Note
        // the information used for registration is implementation dependant,
        // and may involve values computed from the supplied arguments.

    virtual int removeCollectionCallback(const CallbackHandle& handle) = 0;
        // Remove the callback associated with the specified 'handle'.  Return
        // 0 on success, or a non-zero value if 'handle' cannot be found.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
