// bdlm_defaultmetricsregistrar.cpp                                   -*-C++-*-
#include <bdlm_defaultmetricsregistrar.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlm_defaultmetricsregistrar_cpp, "$Id$ $CSID$")

#include <bsls_compilerfeatures.h>

#include <bslmf_assert.h>

#include <bslmt_once.h>

namespace BloombergLP {
namespace bdlm {

BSLMF_ASSERT(sizeof(void *) <=
                          sizeof(bsls::AtomicOperations::AtomicTypes::Uint64));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS  // proxy for alignof
BSLMF_ASSERT(1 < alignof(MetricsRegistrar));  // lock bit stored in pointer
#endif

class MetricIdConfig;

                    // ==================================
                    // class NotMonitoredMetricsRegistrar
                    // ==================================

class NotMonitoredMetricsRegistrar : public MetricsRegistrar {
    // This class implements a pure abstract interface for clients and
    // suppliers of metrics registrars.  The implementation does not register
    // callbacks with any monitoring system.

  public:
    // CREATORS
    NotMonitoredMetricsRegistrar();
        // Create a 'NotMonitoredMetricsRegistrar'.

    ~NotMonitoredMetricsRegistrar();
        // Destroy this object.

    // MANIPULATORS
    int incrementInstanceCount(const MetricDescriptor& metricDescriptor);
        // Do nothing with the specified 'metricDescriptor'.  Return 0.

    CallbackHandle registerCollectionCallback(
                                      const MetricDescriptor& metricDescriptor,
                                      const Callback&         callback);
        // Do nothing with the specified 'metricDescriptor' and 'callback'.
        // Return an aribitrary callback handle.

    int removeCollectionCallback(const CallbackHandle& handle);
        // Do nothing with the specified 'handle'.  Return 0.

    // ACCESSORS
    bsl::string defaultNamespace();
        // Return an empty string.

    bsl::string defaultObjectIdentifierPrefix();
        // Return an empty string.
};

                    // ----------------------------------
                    // class NotMonitoredMetricsRegistrar
                    // ----------------------------------

// CREATORS
NotMonitoredMetricsRegistrar::NotMonitoredMetricsRegistrar()
{
}

NotMonitoredMetricsRegistrar::~NotMonitoredMetricsRegistrar()
{
}

// MANIPULATORS
int NotMonitoredMetricsRegistrar::incrementInstanceCount(
                                const MetricDescriptor& /* metricDescriptor */)
{
    return 0;
}

MetricsRegistrar::CallbackHandle
                      NotMonitoredMetricsRegistrar::registerCollectionCallback(
                                const MetricDescriptor& /* metricDescriptor */,
                                const Callback&         /* callback */)
{
    return 0;
}

int NotMonitoredMetricsRegistrar::removeCollectionCallback(
                                            const CallbackHandle& /* handle */)
{
    return 0;
}

// ACCESSORS
bsl::string NotMonitoredMetricsRegistrar::defaultNamespace()
{
    return bsl::string();
}

bsl::string NotMonitoredMetricsRegistrar::defaultObjectIdentifierPrefix()
{
    return bsl::string();
}

                      // ------------------------------
                      // struct DefaultMetricsRegistrar
                      // ------------------------------

// STATIC DATA MEMBERS
bsls::AtomicOperations::AtomicTypes::Uint64
                       DefaultMetricsRegistrar::s_defaultMetricsRegistrar = {};

// CLASS METHODS
MetricsRegistrar *DefaultMetricsRegistrar::defaultMetricsRegistrar()
{
    bsls::Types::Uint64 registrar =
          reinterpret_cast<bsls::Types::Uint64>(notMonitoredMetricsRegistrar())
                                                               + k_LOCKED_FLAG;

    registrar = AtomicOp::testAndSwapUint64AcqRel(&s_defaultMetricsRegistrar,
                                                  0,
                                                  registrar);

    if (0 == (registrar & k_LOCKED_FLAG)) {
        if (0 == registrar) {
            return notMonitoredMetricsRegistrar();
        }

        bsls::Types::Uint64 oldRegistrar = registrar + k_LOCKED_FLAG;

        while (0 == (registrar & k_LOCKED_FLAG) && oldRegistrar != registrar) {
            oldRegistrar = registrar;
            registrar = AtomicOp::testAndSwapUint64AcqRel(
                                                    &s_defaultMetricsRegistrar,
                                                    registrar,
                                                    registrar + k_LOCKED_FLAG);
        }
    }

    return reinterpret_cast<MetricsRegistrar *>(registrar & k_POINTER_MASK);
}

MetricsRegistrar *DefaultMetricsRegistrar::notMonitoredMetricsRegistrar()
{
    static NotMonitoredMetricsRegistrar *s_metricsRegistrar_p;

    BSLMT_ONCE_DO {
        static NotMonitoredMetricsRegistrar s_metricsRegistrar;

        s_metricsRegistrar_p = &s_metricsRegistrar;
    }

    return s_metricsRegistrar_p;
}

int DefaultMetricsRegistrar::setDefaultMetricsRegistrar(
                                       MetricsRegistrar *basicMetricsRegistrar)
{
    BSLS_ASSERT(0 != basicMetricsRegistrar);

    bsls::Types::Uint64 requested = reinterpret_cast<bsls::Types::Uint64>(
                                                        basicMetricsRegistrar);

    BSLS_ASSERT_SAFE(0 == (requested & k_LOCKED_FLAG));

    bsls::Types::Uint64 registrar    = AtomicOp::getUint64Acquire(
                                                   &s_defaultMetricsRegistrar);
    bsls::Types::Uint64 oldRegistrar = requested;

    while (0 == (registrar & k_LOCKED_FLAG) && oldRegistrar != registrar) {
        oldRegistrar = registrar;
        registrar = AtomicOp::testAndSwapUint64AcqRel(
                                                    &s_defaultMetricsRegistrar,
                                                    registrar,
                                                    requested);
    }
    return (registrar & k_LOCKED_FLAG);
}

}  // close package namespace
}  // close enterprise namespace

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
