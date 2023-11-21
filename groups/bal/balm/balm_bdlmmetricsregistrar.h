// balm_bdlmmetricsregistrar.h                                        -*-C++-*-

#ifndef INCLUDED_BALM_BDLMMETRICSREGISTRAR
#define INCLUDED_BALM_BDLMMETRICSREGISTRAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete instance of the 'bdlm' metrics registrar.
//
//@CLASSES:
// balm::BdlmMetricsRegistrar: concrete instance of the metrics registar
//
//@DESCRIPTION: This component provides a concrete instance,
// 'balm::BdlmMetricsRegistrar', of the 'bdlm::MetricsRegistrar' protocol,
// enabling registration of metric collection callbacks with a provided
// 'balm::MetricsManager'.  'balm::BdlmMetricsRegistrar' also provides methods
// to aid in population of default 'bdlm::MetricDescriptor' attribute values.
//
///Thread Safety
///-------------
// This class is *thread-aware* (see {'bsldoc_glossary'|Thread-Aware}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'balm::BdlmMetricsRegistrar'
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// 'balm::BdlmMetricsRegistrar' object, allowing for registering metric
// callback functions with the 'balm' monitoring system.
//
// First, we provide a metric function to be used during callback registration
// with the 'balm' monitoring system:
//..
//  void elidedMetric(BloombergLP::bdlm::Metric *value)
//  {
//      (void)value;
//      // ...
//  }
//..
// Then, we construct a 'balm::MetricsManager' object and use it to construct a
// 'balm::BdlmMetricsRegistrar' that will use "bdlm" as its default metric
// namespace, "svc" as its default object indentifier prefix, and will not
// attempt to set itself as the default metrics registrar:
//..
//  balm::MetricsManager manager;
//  balm::BdlmMetricsRegistrar registrar(&manager, "bdlm", "svc", false);
//..
// Next, we construct a 'bdlm::MetricsDescriptor' object to be used when
// registering the callback function, making use of the helper methods in the
// 'bdlm::MetricsRegistrar' protocol:
//..
//  bdlm::MetricDescriptor descriptor(registrar.defaultMetricNamespace(),
//                                    "example",
//                                    "balm.bdlmmetricsregistrar",
//                                    "");
//  {
//      bsl::stringstream identifier;
//      identifier << registrar.defaultObjectIdentifierPrefix()
//                 << ".bmr."
//                 << registrar.incrementInstanceCount(descriptor);
//      descriptor.setObjectIdentifier(identifier.str());
//  }
//..
// Now, we register the collection callback:
//..
//  bdlm::MetricsRegistrar::CallbackHandle handle =
//                          registrar.registerCollectionCallback(descriptor,
//                                                               elidedMetric);
//..
// Finally, presumably during shutdown of the application, we remove the
// callback from the monitoring system, and verify the callback was
// successfully removed:
//..
//  assert(0 == registrar.removeCollectionCallback(handle));
//..

#include <bdlf_bind.h>

#include <bdlm_metricdescriptor.h>
#include <bdlm_metricsregistrar.h>
#include <bdlm_defaultmetricsregistrar.h>

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

                        // ==========================
                        // class BdlmMetricsRegistrar
                        // ==========================

class BdlmMetricsRegistrar : public bdlm::MetricsRegistrar {
    // This class implements a pure abstract interface for clients and
    // suppliers of metrics registrars.  The implementation registers callbacks
    // with a provided 'balm::MetricsManager' to enable monitoring of
    // statistics collection objects.

    // PRIVATE CLASS METHODS
    static void metricCb(bsl::vector<MetricRecord> *records,
                         bool                       resetFlag,
                         MetricId                   id,
                         const Callback&            callback);
        // Load into the specified 'records' a new record with the specified
        // 'id' and statistics obtained from the specified 'callback' and, if
        // the specified 'resetFlag' is 'true', reset the statistics to their
        // default state.

    // DATA
    MetricsManager    *d_metricsManager_p;        // held, but not owned,
                                                  // metrics manager

    const bsl::string  d_metricNamespace;         // default metric namespace
                                                  // attribute value

    const bsl::string  d_objectIdentifierPrefix;  // default prefix for object
                                                  // identifier attribute
                                                  // values

    bsl::map<bsl::string, int>
                       d_count;                   // stores instance counts

    bslmt::Mutex       d_mutex;                   // mutex to protect 'd_count'

    // NOT IMPLEMENTED
    BdlmMetricsRegistrar() BSLS_KEYWORD_DELETED;
    BdlmMetricsRegistrar(const BdlmMetricsRegistrar&) BSLS_KEYWORD_DELETED;
    BdlmMetricsRegistrar operator=(const BdlmMetricsRegistrar&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BdlmMetricsRegistrar,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    BdlmMetricsRegistrar(MetricsManager          *metricsManager,
                         const bsl::string_view&  metricNamespace,
                         const bsl::string_view&  objectIdentifierPrefix,
                         bool                     installAsDefault = false,
                         bslma::Allocator        *basicAllocator = 0);
        // Create a 'BdlmMetricsRegistrar' object that uses the specified
        // 'metricsManager' to register and unregister collection callback
        // functors, the specified 'metricNamespace' as the value returned by
        // 'defaultNamespace()', the specified 'objectIdentifierPrefix' as the
        // value returned by 'defaultObjectIdentifierPrefix', and if the
        // specified 'installAsDefault' is 'true', attempt to install this
        // registrar as the one obtained by
        // 'bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar()'.  If the
        // default registrar has already been set, this attempt will have no
        // effect.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~BdlmMetricsRegistrar();
        // Destroy this 'BdlmMetricsRegistrar' object.

    // MANIPULATORS
    int incrementInstanceCount(const bdlm::MetricDescriptor& metricDescriptor);
        // Return the incremented invocation count of this method with the
        // provided 'metricDescriptor' attributes, excluding object identifier.

    CallbackHandle registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback);
        // Register the specified 'callback' with the metrics manager specified
        // at construction, using the specified 'metricDescriptor'.  Return the
        // callback handle to be used with 'removeCollectionCallback'.

    int removeCollectionCallback(const CallbackHandle& handle);
        // Remove the callback associated with the specified 'handle' from the
        // metrics manager specified at construction.  Return 0 on success, or
        // a non-zero value if 'handle' cannot be found.

    // ACCESSORS
    const bsl::string& defaultMetricNamespace() const;
        // Return the namespace attribute value to be used as the default value
        // for 'MetricDescriptor' instances.

    const bsl::string& defaultObjectIdentifierPrefix() const;
        // Return a string to be used as the default prefix for a
        // 'MetricDescriptor' object identifier attribute value.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class BdlmMetricsRegistrar
                        // --------------------------

// CREATORS
inline
BdlmMetricsRegistrar::BdlmMetricsRegistrar(
                               balm::MetricsManager    *metricsManager,
                               const bsl::string_view&  metricNamespace,
                               const bsl::string_view&  objectIdentifierPrefix,
                               bool                     installAsDefault,
                               bslma::Allocator        *basicAllocator)
: d_metricsManager_p(metricsManager)
, d_metricNamespace(metricNamespace, basicAllocator)
, d_objectIdentifierPrefix(objectIdentifierPrefix, basicAllocator)
, d_count(basicAllocator)
{
    if (installAsDefault) {
        bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar(this);
    }
}

// MANIPULATORS
inline
bdlm::MetricsRegistrar::CallbackHandle
BdlmMetricsRegistrar::registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback)
{
    bsl::string name = metricDescriptor.objectTypeName()   + '.'
                     + metricDescriptor.metricName()       + '.'
                     + metricDescriptor.objectIdentifier();

    MetricId id = d_metricsManager_p->metricRegistry().getId(
                     metricDescriptor.metricNamespace().c_str(), name.c_str());

    return d_metricsManager_p->registerCollectionCallback(
                                   metricDescriptor.metricNamespace().c_str(),
                                   bdlf::BindUtil::bind(&metricCb,
                                                        bdlf::PlaceHolders::_1,
                                                        bdlf::PlaceHolders::_2,
                                                        id,
                                                        callback));
}

inline
int BdlmMetricsRegistrar::removeCollectionCallback(
                                                  const CallbackHandle& handle)
{
    return d_metricsManager_p->removeCollectionCallback(handle);
}

// ACCESSORS
inline
const bsl::string& BdlmMetricsRegistrar::defaultMetricNamespace() const
{
    return d_metricNamespace;
}

inline
const bsl::string& BdlmMetricsRegistrar::defaultObjectIdentifierPrefix() const
{
    return d_objectIdentifierPrefix;
}

inline
bslma::Allocator *BdlmMetricsRegistrar::allocator() const
{
    return d_metricNamespace.get_allocator().mechanism();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
