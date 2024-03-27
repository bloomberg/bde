// bdlm_metricsregistry.h                                             -*-C++-*-

#ifndef INCLUDED_BDLM_METRICSREGISTRY
#define INCLUDED_BDLM_METRICSREGISTRY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a transferable registry of metric registrations.
//
//@CLASSES:
// bdlm::MetricsRegistry: transferable registry of metric registrations
// bdlm::MetricsRegistryRegistrationHandle: registration handle providing RAII
//
//@DESCRIPTION: This component contains a mechanism, 'bdlm::MetricsRegistry',
// providing a metric registry that is transferable to implementations of the
// 'bdlm::MetricsAdapter' protocol, a singleton that is the default registry
// for the process, and a registration handle class,
// 'bdlm::MetricsRegistryRegistrationHandle', that provides RAII symantics for
// metric registration.
//
///Thread Safety
///-------------
// The class 'bdlm::MetricsRegistry' is *thread-aware* (see
// {'bsldoc_glossary'|Thread-Aware}), and
// 'bdlm::MetricsRegistryRegistrationHandle' is *minimally thread-safe* (see
// {'bsldoc_glossary'|Minimally Thread-Safe}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlm::MetricsRegistry'
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// 'bdlm::MetricsRegistry' object, allowing for registering metric callback
// functions with the 'bdlm' monitoring system.
//
// First, we provide a metric function to be used during callback registration
// with the 'bdlm' monitoring system:
//..
//  void elidedMetric(BloombergLP::bdlm::Metric *value)
//  {
//      (void)value;
//      // ...
//  }
//..
// Then, we construct a 'bdlm::MetricsDescriptor' object to be used when
// registering the callback function:
//..
//  bdlm::MetricDescriptor descriptor("bdlm",
//                                    "example",
//                                    1,
//                                    "bdlmmetricsregistry",
//                                    "bmr",
//                                    "identifier");
//..
// Next, we construct a 'bdlm::MetricsRegistry' object with a test allocator:
//..
//      bslma::TestAllocator  ta;
//      bdlm::MetricsRegistry registry(&ta);
//..
// Now, we register the collection callback:
//..
//  bdlm::MetricsRegistryRegistrationHandle handle =
//                          registry.registerCollectionCallback(descriptor,
//                                                              &elidedMetric);
//..
// Finally, we remove the callback from the monitoring system, and verify the
// callback was successfully removed:
//..
//  assert(0 == handle.unregister());
//..

#include <bdlm_metricdescriptor.h>
#include <bdlm_metricsadapter.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_keyword.h>

#include <bsl_map.h>

namespace BloombergLP {
namespace bdlm {

class MetricsRegistryRegistrationHandle;

                       // ==========================
                       // class MetricsRegistry_Data
                       // ==========================

class MetricsRegistry_Data {
  public:
    // PUBLIC DATA
    MetricDescriptor               d_descriptor;
    MetricsAdapter::Callback       d_callback;
    MetricsAdapter::CallbackHandle d_handle;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricsRegistry_Data,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit MetricsRegistry_Data(bslma::Allocator *basicAllocator);
        // Create a 'MetricsRegistry_Data' object that uses the specified
        // 'basicAllocator' to supply memory.

    MetricsRegistry_Data();
        // Create a 'MetricsRegistry_Data' object that uses the default
        // allocator to supply memory.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

                          // =====================
                          // class MetricsRegistry
                          // =====================

class MetricsRegistry {
    // This class implements a pure abstract interface for clients and
    // suppliers of metrics registries.  The implementation registers callbacks
    // with an associated 'bdlm::MetricsRegistry' to enable monitoring of
    // statistics collection objects.  This class is *usually* a singleton.

  public:
    // TYPES
    typedef MetricsAdapter::Callback       Callback;
    typedef MetricsAdapter::CallbackHandle CallbackHandle;

  private:
    // PRIVATE TYPES
    typedef bsl::map<CallbackHandle, MetricsRegistry_Data> MetricDataMap;

    // DATA
    MetricsAdapter                 *d_metricsAdapter_p;  // held, but not
                                                         // owned, metrics
                                                         // adapter

    MetricDataMap                   d_metricData;        // registration data

    CallbackHandle                  d_nextKey;           // next key for
                                                         // inserting into
                                                         // 'd_metricData'

    bslmt::Mutex                    d_mutex;             // mutex to protect
                                                         // the metrics
                                                         // registry and
                                                         // 'd_metricData'

    // PRIVATE MANIPULATORS
    int removeCollectionCallback(const CallbackHandle& handle);
        // Remove the callback associated with the specified 'handle' from the
        // associated adapter, if provided, and from this registry.  Return 0
        // on success, or a non-zero value if 'handle' cannot be found.

    // FRIENDS
    friend class MetricsRegistryRegistrationHandle;

    // NOT IMPLEMENTED
    MetricsRegistry(const MetricsRegistry&) BSLS_KEYWORD_DELETED;
    MetricsRegistry& operator=(const MetricsRegistry&) BSLS_KEYWORD_DELETED;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricsRegistry, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static MetricsRegistry& singleton();
        // Return a non-'const' reference to the metrics registry singleton.

    // CREATORS
    MetricsRegistry(bslma::Allocator *basicAllocator = 0);
        // Create a 'MetricsRegistry' object that stores the information
        // necessary to forward the register and unregister of metrics to a
        // metrics registry supplied with 'setMetricsRegistry'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MetricsRegistry();
        // Unregister all registered metrics and destroy this 'MetricsRegistry'
        // object.

    // MANIPULATORS
    MetricsRegistryRegistrationHandle registerCollectionCallback(
                                      const bdlm::MetricDescriptor& descriptor,
                                      const Callback&               callback);
        // Store the specified 'descriptor' and 'callback'.  If an adapter has
        // been provided, register the metric.  The returned
        // 'MetricsRegistryRegistrationHandle' will unsubscribe the callback by
        // invoking 'unsubscribe'.

    void removeMetricsAdapter(MetricsAdapter *adapter);
        // If the specified 'adapter' is the currently associated regsitrar,
        // remove all metrics from it and disassociate the registry.

    void setMetricsAdapter(MetricsAdapter *adapter);
        // Transfer all metrics to the specified 'adapter'.  If there is a
        // currently associated adapter, remove all metrics from it.  Register
        // all metrics with the provided 'adapter' and define 'adapter' as the
        // associated registry.

    // ACCESSORS

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

                 // =======================================
                 // class MetricsRegistryRegistrationHandle
                 // =======================================

class MetricsRegistryRegistrationHandle {
    // DATA
    MetricsRegistry                 *d_registry;  // associated registry
    MetricsRegistry::CallbackHandle  d_handle;    // handle for registration

  public:
    // CREATORS
    MetricsRegistryRegistrationHandle();
        // Create a default 'MetricsRegistryRegistrationHandle' object.

    MetricsRegistryRegistrationHandle(
                                 const MetricsRegistryRegistrationHandle& obj);
        // Create a 'MetricsRegistryRegistrationHandle' object having the same
        // registration as the specified 'obj'.

    MetricsRegistryRegistrationHandle(
                                     MetricsRegistry                 *registry,
                                     MetricsRegistry::CallbackHandle  handle);
        // Create a 'MetricsRegistryRegistrationHandle' object having the
        // associated specified 'registry' and the specified 'handle' in the
        // 'registry'.

    ~MetricsRegistryRegistrationHandle();
        // Unregister the metric and destroy this
        // 'MetricsRegistryRegistrationHandle' object.

    // MANIPULATORS
    MetricsRegistryRegistrationHandle& operator=(
                                 const MetricsRegistryRegistrationHandle& rhs);
        // Assign to this object the registration from the specified 'rhs'
        // object to this object.

    int unregister();
        // Unregister the metric from the associated registry.  Return 0 on
        // success, or a non-zero value if this handle cannot be found in the
        // associated registry.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // class MetricsRegistry
                          // ---------------------

// CREATORS
inline
MetricsRegistry::MetricsRegistry(bslma::Allocator *basicAllocator)
: d_metricsAdapter_p(0)
, d_metricData(basicAllocator)
, d_nextKey(0)
{
}

// ACCESSORS

                                  // Aspects

inline
bslma::Allocator *MetricsRegistry::allocator() const
{
    return d_metricData.get_allocator().mechanism();
}

                 // ---------------------------------------
                 // class MetricsRegistryRegistrationHandle
                 // ---------------------------------------

// CREATORS
inline
MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle()
: d_registry(0)
, d_handle(0)
{
}

inline
MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle(
                                     MetricsRegistry                 *registry,
                                     MetricsRegistry::CallbackHandle  handle)
: d_registry(registry)
, d_handle(handle)
{
}

inline
MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle(
                                  const MetricsRegistryRegistrationHandle& obj)
: d_registry(obj.d_registry)
, d_handle(obj.d_handle)
{
}

inline
MetricsRegistryRegistrationHandle::~MetricsRegistryRegistrationHandle()
{
}

// MANIPULATORS
inline
MetricsRegistryRegistrationHandle& MetricsRegistryRegistrationHandle::
                        operator=(const MetricsRegistryRegistrationHandle& rhs)
{
    d_registry = rhs.d_registry;
    d_handle   = rhs.d_handle;

    return *this;
}

inline
int MetricsRegistryRegistrationHandle::unregister()
{
    int rv = 1;

    if (d_registry) {
        rv = d_registry->removeCollectionCallback(d_handle);
        d_registry = 0;
    }

    return rv;
}


                       // ==========================
                       // class MetricsRegistry_Data
                       // ==========================

// CREATORS
inline
MetricsRegistry_Data::MetricsRegistry_Data()
: d_descriptor()
, d_callback()
, d_handle()
{
}

inline
MetricsRegistry_Data::MetricsRegistry_Data(bslma::Allocator *basicAllocator)
: d_descriptor(basicAllocator)
, d_callback(bsl::allocator_arg, basicAllocator)
, d_handle()
{
}

// ACCESSORS
                                  // Aspects

inline
bslma::Allocator *MetricsRegistry_Data::allocator() const
{
    return d_descriptor.allocator();
}

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
