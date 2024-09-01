// balm_bdlmmetricsadapter.h                                          -*-C++-*-

#ifndef INCLUDED_BALM_BDLMMETRICSADAPTER
#define INCLUDED_BALM_BDLMMETRICSADAPTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete instance of the `bdlm` metrics adapter.
//
//@CLASSES:
// balm::BdlmMetricsAdapter: concrete instance of the metrics registar
//
//@DESCRIPTION: This component provides a concrete instance,
// `balm::BdlmMetricsAdapter`, of the `bdlm::MetricsAdapter` protocol, enabling
// registration of metric collection callbacks with a provided
// `balm::MetricsManager`.  `balm::BdlmMetricsAdapter` also provides methods to
// aid in population of default `bdlm::MetricDescriptor` attribute values.
//
///Thread Safety
///-------------
// This class is *thread-aware* (see {`bsldoc_glossary`|Thread-Aware}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `balm::BdlmMetricsAdapter`
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// `balm::BdlmMetricsAdapter` object, allowing for registering metric callback
// functions with the `balm` monitoring system.
//
// First, we provide a metric function to be used during callback registration
// with the `balm` monitoring system:
// ```
// void elidedMetric(BloombergLP::bdlm::Metric *value)
// {
//     (void)value;
//     // ...
// }
// ```
// Then, we construct a `balm::MetricsManager` object and use it to construct a
// `balm::BdlmMetricsAdapter` that will use "bdlm" as its default metric
// namespace, "svc" as its default object indentifier prefix, and will not
// attempt to set itself as the default metrics adapter:
// ```
// balm::MetricsManager     manager;
// balm::BdlmMetricsAdapter adapter(&manager, "bdlm", "svc");
// ```
// Next, we construct a `bdlm::MetricsDescriptor` object to be used when
// registering the callback function, using constants from
// `bdlm::MetricDescriptor` for the namespace and identifier to indicate the
// implementation of the `bdlm::MetricsAdapter` protocol should supply values:
// ```
// bdlm::MetricDescriptor descriptor(
//         bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION,
//         "example",
//         1,
//         "balm.bdlmmetricsadapter",
//         "bmr",
//         bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION);
// ```
// Now, we register the collection callback:
// ```
// bdlm::MetricsAdapter::CallbackHandle handle =
//                           adapter.registerCollectionCallback(descriptor,
//                                                              elidedMetric);
// ```
// Finally, presumably during shutdown of the application, we remove the
// callback from the monitoring system, and verify the callback was
// successfully removed:
// ```
// assert(0 == adapter.removeCollectionCallback(handle));
// ```

#include <bdlf_bind.h>

#include <bdlm_metricdescriptor.h>
#include <bdlm_metricsadapter.h>

#include <balm_collectorrepository.h>
#include <balm_defaultmetricsmanager.h>
#include <balm_metricid.h>
#include <balm_metricrecord.h>
#include <balm_metricsmanager.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace balm {

                         // ========================
                         // class BdlmMetricsAdapter
                         // ========================

/// This class implements a pure abstract interface for clients and
/// suppliers of metrics adapters.  The implementation registers callbacks
/// with a provided `balm::MetricsManager` to enable monitoring of
/// statistics collection objects.
class BdlmMetricsAdapter : public bdlm::MetricsAdapter {

    // PRIVATE CLASS METHODS

    /// Load into the specified `records` a new record with the specified
    /// `id` and statistics obtained from the specified `callback` and, if
    /// the specified `resetFlag` is `true`, reset the statistics to their
    /// default state.
    static void metricCb(bsl::vector<MetricRecord> *records,
                         bool                       resetFlag,
                         MetricId                   id,
                         const Callback&            callback);

    // DATA
    MetricsManager    *d_metricsManager_p;        // held, but not owned,
                                                  // metrics manager

    const bsl::string  d_metricNamespace;         // default metric namespace
                                                  // attribute value

    const bsl::string  d_objectIdentifierPrefix;  // default prefix for object
                                                  // identifier attribute
                                                  // values

    // NOT IMPLEMENTED
    BdlmMetricsAdapter() BSLS_KEYWORD_DELETED;
    BdlmMetricsAdapter(const BdlmMetricsAdapter&) BSLS_KEYWORD_DELETED;
    BdlmMetricsAdapter operator=(const BdlmMetricsAdapter&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BdlmMetricsAdapter,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a `BdlmMetricsAdapter` object that uses the specified
    /// `metricsManager` to register and unregister collection callback
    /// functors, the specified `metricNamespace` as the value returned by
    /// `defaultNamespace()`, and the specified `objectIdentifierPrefix` as
    /// the value returned by `defaultObjectIdentifierPrefix`.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    BdlmMetricsAdapter(MetricsManager          *metricsManager,
                       const bsl::string_view&  metricNamespace,
                       const bsl::string_view&  objectIdentifierPrefix,
                       bslma::Allocator        *basicAllocator = 0);

    /// Destroy this `BdlmMetricsAdapter` object.
    ~BdlmMetricsAdapter() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Register the specified `callback` with the metrics manager specified
    /// at construction, using the specified `metricDescriptor`.  If
    /// `metricDescriptor.metricsNamspace()` equals
    /// `bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION`,
    /// use `defaultMetricNamespace()` for the namespace attribute during
    /// registration.  If `metricDescriptor.objectIdentifier()` equals
    /// `bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION`,
    /// use the concatination of `defaultObjectIdentifierPrefix()`, a
    /// period, `metricDescriptor.objectTypeAbbreviation()`, a period, and
    /// `metricDescriptor.instanceNumber()` for the object identifier
    /// attribute during registration.  The category name supplied to the
    /// `MetricsManager` provided at construction is the concatenation of
    /// the object type name attribute, a period, the metric name attribute,
    /// a period, and the object identifier attribute.  Return the callback
    /// handle to be used with `removeCollectionCallback`.
    CallbackHandle registerCollectionCallback(
                 const bdlm::MetricDescriptor& metricDescriptor,
                 const Callback&               callback) BSLS_KEYWORD_OVERRIDE;

    int removeCollectionCallback(const CallbackHandle& handle)
                                                         BSLS_KEYWORD_OVERRIDE;
        // Remove the callback associated with the specified 'handle' from the
        // metrics manager specified at construction.  Return 0 on success, or
        // a non-zero value if 'handle' cannot be found.

    // ACCESSORS

    /// Return the namespace attribute value to be used as the default value
    /// for `MetricDescriptor` instances.
    const bsl::string& defaultMetricNamespace() const;

    /// Return a string to be used as the default prefix for a
    /// `MetricDescriptor` object identifier attribute value.
    const bsl::string& defaultObjectIdentifierPrefix() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // ------------------------
                         // class BdlmMetricsAdapter
                         // ------------------------

// CREATORS
inline
BdlmMetricsAdapter::BdlmMetricsAdapter(
                               balm::MetricsManager    *metricsManager,
                               const bsl::string_view&  metricNamespace,
                               const bsl::string_view&  objectIdentifierPrefix,
                               bslma::Allocator        *basicAllocator)
: d_metricsManager_p(metricsManager)
, d_metricNamespace(metricNamespace, basicAllocator)
, d_objectIdentifierPrefix(objectIdentifierPrefix, basicAllocator)
{
}

// MANIPULATORS
inline
int BdlmMetricsAdapter::removeCollectionCallback(const CallbackHandle& handle)
{
    return d_metricsManager_p->removeCollectionCallback(handle);
}

// ACCESSORS
inline
const bsl::string& BdlmMetricsAdapter::defaultMetricNamespace() const
{
    return d_metricNamespace;
}

inline
const bsl::string& BdlmMetricsAdapter::defaultObjectIdentifierPrefix() const
{
    return d_objectIdentifierPrefix;
}

inline
bslma::Allocator *BdlmMetricsAdapter::allocator() const
{
    return d_metricNamespace.get_allocator().mechanism();
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
