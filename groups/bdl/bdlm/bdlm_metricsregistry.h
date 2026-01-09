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
//@DESCRIPTION: This component contains a mechanism, `bdlm::MetricsRegistry`,
// that provides a registry of metrics that is transferable to implementations
// of the `bdlm::MetricsAdapter` protocol.  `bdlm`, as a low-level metrics
// facility, does not directly manage schedulers to collect metrics values or
// publishers to publish those value.  Instead `bdlm` is designed to allow
// applications to plug in different high-level feature-rich metrics collection
// and publication frameworks (without requiring a library dependency on those
// frameworks).  A `bdlm::MetricsRegistry` effectively serves as a proxy for a
// higher-level metrics collection system implementing the `MetricsAdapter`
// protocol -- it keeps track of registered metrics allowing a higher-level
// metrics subsystem to be installed (by calling 'setMetricsAdapter) at any
// time, either before or after a metric is registered.
//
// A singleton instance of `MetricsRegistry` is available from the
// `defaultInstance` class method.  This component also provides a registration
// handle class, `bdlm::MetricsRegistryRegistrationHandle`, that provides RAII
// semantics for metric registration.
//
///Thread Safety
///-------------
// The class `bdlm::MetricsRegistry` is *fully thread-safe* (see
// {`bsldoc_glossary`|Fully Thread-Safe}), and
// `bdlm::MetricsRegistryRegistrationHandle` is *minimally thread-safe* (see
// {`bsldoc_glossary`|Minimally Thread-Safe}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bdlm::MetricsRegistry`
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// `bdlm::MetricsRegistry` object, allowing for registering metric callback
// functions with the `bdlm` monitoring system.
//
// First, we declare a class that provides some metric for the `bdlm`
// monitoring system:
// ```
// class LowLevelFacility {
//     // PRIVATE DATA
//     bdlm::MetricsRegistryRegistrationHandle d_metricHandle;
//   public:
//     // CREATORS
//     explicit LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry =
//                                  bdlm::MetricsRegistry::defaultInstance());
//
//     // ACCESSORS
//     int someMetric() const
//     {
//         return 0; // just a stub
//     }
// };
// ```
// Next, we provide a metric function to be used during callback registration:
// ```
// void metricCallback(bdlm::Metric *value, const LowLevelFacility *object)
// {
//     *value = bdlm::Metric::Gauge(object->someMetric());
// }
// ```
// Here is the constructor definition that registers the collection callback:
// ```
// /// Construct a `bdlm::MetricsDescriptor` object to be used when
// /// registering the callback function:
// LowLevelFacility::LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry)
// {
//     bdlm::MetricDescriptor descriptor("bdlm",
//                                       "example",
//                                       1,
//                                       "bdlmmetricsregistry",
//                                       "bmr",
//                                       "identifier");
//
//     // Register the collection callback:
//     metricsRegistry.registerCollectionCallback(
//                                &d_metricHandle,
//                                descriptor,
//                                bdlf::BindUtil::bind(&metricCallback,
//                                                     bdlf::PlaceHolders::_1,
//                                                     this));
//     assert(d_metricHandle.isRegistered());
// }
// ```
// Notice that the compiler-supplied destructor is sufficient because the
// `d_metricHandle` will deregister the metric on destruction.
//
// Now, we construct a `bdlm::MetricsRegistry` object with a test allocator:
// ```
// bslma::TestAllocator  ta;
// bdlm::MetricsRegistry registry(&ta);
// assert(registry.numRegisteredCollectionCallbacks() == 0);
// ```
// Then, we create the object and pass the constructed `bdlm::MetricsRegistry`
// object there:
// ```
// {
//     LowLevelFacility facility(registry);
//     assert(registry.numRegisteredCollectionCallbacks() == 1);
// ```
// If we don't provide a `bdlm::MetricsRegistry` object explicitly, the default
// global instance will be used.
//
// Finally, the callback is removed the monitoring system by the destructor of
// `facility` object:
// ```
// } // 'facility.d_metricHandle.unregister()' is called here
// assert(registry.numRegisteredCollectionCallbacks() == 0);
// ```

#include <bdlm_metricdescriptor.h>
#include <bdlm_metricsadapter.h>

#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslstl_sharedptr.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bdlm {

class MetricsRegistryRegistrationHandle;
class MetricsRegistry_Impl;

                          // =====================
                          // class MetricsRegistry
                          // =====================

/// This class implements a mechanism that provides a registry of metrics
/// that is transferable to implementations of the `bdlm::MetricsAdapter`
/// protocol.  This class is *usually* a singleton.
class MetricsRegistry {

    // DATA
    bsl::shared_ptr<MetricsRegistry_Impl> d_impl;

  public:
    // TYPES
    typedef MetricsAdapter::Callback       Callback;
    typedef MetricsAdapter::CallbackHandle CallbackHandle;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricsRegistry, bslma::UsesBslmaAllocator);

    // CLASS METHODS

    /// Return a non-`const` reference to the metrics registry singleton.
    static MetricsRegistry& defaultInstance();

    // CREATORS

    /// Create a `MetricsRegistry` object that stores the information
    /// necessary to forward the registration and unregistration of metrics
    /// to an adapter supplied with `setMetricsRegistry`.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit MetricsRegistry(bslma::Allocator *basicAllocator = 0);

    /// Unregister all registered metrics and destroy this `MetricsRegistry`
    /// object.
    ~MetricsRegistry();

    // MANIPULATORS

    /// Disable metrics collection.  If there is an associated metrics adapter,
    /// all the collection callbacks are unregistered from that adapter.  The
    /// adapter remains associated with this registry.  Collection callbacks
    /// registered with this registry are not registered with the associated
    /// adapter until metrics collections is enabled.  Return 0 on success, and
    /// a non-zero value otherwise.  Metrics collection is enabled at
    /// construction of this registry.
    int disableMetricsCollection();

    /// Enable metrics collection.  If there is an associated metrics adapter,
    /// all collection callbacks will be registered with the adapter.  Return
    /// 0 on success, and non-zero value otherwise.  Metrics collection is
    /// enabled at construction of this registry.
    int enableMetricsCollection();

    /// Register the metric described by the specified `descriptor` and
    /// associate it with the specified `callback` to collect data from the
    /// metric, and load the specified `result` with a handle can be used
    /// later to unregister the metric.  Return 0 on success, and a non-zero
    /// value otherwise.  After this operation completes,
    /// `result->isRegistered()` will be `true`.  The metric and associated
    /// callback remain registered with this registry until either the
    /// handle is unregistered or destroyed.  When a `MetricsAdapter` is
    /// associated with this registry using `setMetricsAdapter`, this
    /// object registers all the registered metrics and callbacks with that
    /// adapter, and similarly unregisters them if the `MetricAdapter` is
    /// later disassociated with this registry (either on this objects
    /// destruction, or due to a call to `removeMetricsAdapter` or
    /// `setMetricsAdapter`).  Furthermore, metrics collection can be disable
    /// with `disableMetricsCollection` and enabled with
    /// `enableMetricsCollection`.  In this way, a `MetricsRegistry` serves as
    /// an intermediary between users of `bdlm` that register metrics and the
    /// subsystem for collecting and publishing metrics being adapted by a
    /// concrete instance of `bdlm::MetricAdapter`.
    int registerCollectionCallback(
                                 MetricsRegistryRegistrationHandle *result,
                                 const bdlm::MetricDescriptor&      descriptor,
                                 const Callback&                    callback);

    /// If the specified `adapter` is the currently associated adapter, remove
    /// all registered metrics from it and disassociate with this registry.
    /// Return 0 on success, and a non-zero value otherwise.  Note that this
    /// operation takes an `adapter` to disambiguate multiple, potentially
    /// concurrent, calls to this method and `setMetricsAdapter`.
    int removeMetricsAdapter(MetricsAdapter *adapter);

    /// Configure this metrics registry to register all metrics collection
    /// callbacks with the specified `adapter` when the registry has metrics
    /// collection enabled.  This operation first, if there is already an
    /// associated metrics adapter, unregisters all the collection callbacks
    /// from that adapter, then registers the collection callbacks with the new
    /// `adapter` if metrics collection is enabled.  Return 0 on success, and a
    /// non-zero value otherwise.
    int setMetricsAdapter(MetricsAdapter *adapter);

    // ACCESSORS

    /// Return the number of registered metrics collection callbacks.
    int numRegisteredCollectionCallbacks() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const;
};

                 // =======================================
                 // class MetricsRegistryRegistrationHandle
                 // =======================================

/// This class implements a registration handle that provides RAII semantics
/// for metric registration.
class MetricsRegistryRegistrationHandle {

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    // DATA
    bsl::weak_ptr<MetricsRegistry_Impl> d_registry;  // associated registry
    MetricsRegistry::CallbackHandle     d_handle;    // handle for registration

    // PRIVATE CREATORS

    /// Create a `MetricsRegistryRegistrationHandle` object having the
    /// associated specified `registry` and the specified `handle` in the
    /// `registry`.
    MetricsRegistryRegistrationHandle(
                           const bsl::weak_ptr<MetricsRegistry_Impl>& registry,
                           MetricsRegistry::CallbackHandle            handle);

    // FRIENDS
    friend class MetricsRegistry;

    // NOT IMPLEMENTED
    MetricsRegistryRegistrationHandle(
               const MetricsRegistryRegistrationHandle &) BSLS_KEYWORD_DELETED;
    MetricsRegistryRegistrationHandle& operator=(
               const MetricsRegistryRegistrationHandle &) BSLS_KEYWORD_DELETED;
  public:
    // CREATORS

    /// Create a `MetricsRegistryRegstrationHandle` object that is not
    /// associated with a registered metrics collection callback
    /// (`isRegistered` will return `false`).
    MetricsRegistryRegistrationHandle();

    /// Create a `MetricsRegistryRegistrationHandle` object that will manage
    /// the metric collection callback registration associated with the
    /// specified `original` handle.  After creating this object, `original`
    /// will no longer manage the registration, and
    /// `original.isRegistered()` will be `false`.  If `original` does not
    /// manage a registration when this object is created, then neither this
    /// object nor `original` will manage a registration.
    MetricsRegistryRegistrationHandle(
                 bslmf::MovableRef<MetricsRegistryRegistrationHandle> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Unregister the metric collection callback associated with this
    /// handle (if `isRegistered` is `true`), and destroy this object.
    ~MetricsRegistryRegistrationHandle();

    // MANIPULATORS

    /// Unregister the metric collection callback associated with this
    /// handle (if `isRegistered` is `true`).  Take an ownership on the
    /// metric collection callback registration associated with the
    /// specified `original` handle.  Afterwards, `original` will no longer
    /// manage the registration, and `original.isRegistered()` will be
    /// `false`.
    MetricsRegistryRegistrationHandle &operator=(
                 bslmf::MovableRef<MetricsRegistryRegistrationHandle> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Exchange the associated metric collection callback registration with
    /// the one controlled by the specified `other` handle.
    void swap(MetricsRegistryRegistrationHandle& other) BSLS_KEYWORD_NOEXCEPT;

    /// Unregister the metric from the associated registry.  Return 0 on
    /// success, and a non-zero value if this handle is not currently
    /// associated with a registered metrics collection callback.
    int unregister();

    // ACCESSORS

    /// Return `true` if this handle has an associated registered metrics
    /// collection callback.
    bool isRegistered() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                 // ---------------------------------------
                 // class MetricsRegistryRegistrationHandle
                 // ---------------------------------------

// CREATORS
inline
MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle()
: d_registry()
, d_handle(0)
{
}

// MANIPULATORS
inline
void MetricsRegistryRegistrationHandle::swap(
                MetricsRegistryRegistrationHandle& other) BSLS_KEYWORD_NOEXCEPT
{
    bslalg::SwapUtil::swap(&d_registry, &other.d_registry);
    bslalg::SwapUtil::swap(&d_handle, &other.d_handle);
}

// ACCESSORS
inline
bool MetricsRegistryRegistrationHandle::isRegistered() const
{
    return !d_registry.expired();
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
