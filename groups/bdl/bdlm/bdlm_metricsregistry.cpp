// bdlm_metricsregistry.cpp                                           -*-C++-*-

#include <bdlm_metricsregistry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlm_metricsregistry_cpp,"$Id$ $CSID$")

#include <bslma_default.h>

#include <bslmt_once.h>

namespace BloombergLP {
namespace bdlm {

                          // ---------------------
                          // class MetricsRegistry
                          // ---------------------

// PRIVATE MANIPULATORS
int MetricsRegistry::removeCollectionCallback(const CallbackHandle& handle)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    bsl::map<CallbackHandle, Data>::iterator iter = d_metricData.find(handle);
    if (iter != d_metricData.end()) {
        if (d_metricsAdapter_p) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                        iter->second.d_handle);
        }
        d_metricData.erase(iter);
        return 0;
    }
    return 1;
}

// CLASS METHODS
MetricsRegistry& MetricsRegistry::singleton()
{
    static MetricsRegistry *s_metricsRegistry_p;

    BSLMT_ONCE_DO {
        static MetricsRegistry s_metricsRegistry(
                                            bslma::Default::globalAllocator());

        s_metricsRegistry_p = &s_metricsRegistry;
    }

    return *s_metricsRegistry_p;
}

// CREATORS
MetricsRegistry::~MetricsRegistry()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_metricsAdapter_p) {
        for (bsl::map<CallbackHandle, Data>::iterator iter =
                                                          d_metricData.begin();
             iter != d_metricData.end();
             ++iter) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                        iter->second.d_handle);
        }
        d_metricsAdapter_p = 0;
    }
}

// MANIPULATORS
MetricsRegistryRegistrationHandle MetricsRegistry::registerCollectionCallback(
                                      const bdlm::MetricDescriptor& descriptor,
                                      const Callback&               callback)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    Data& data        = d_metricData[d_nextKey];
    data.d_descriptor = descriptor;
    data.d_callback   = callback;

    if (d_metricsAdapter_p) {
        data.d_handle =
                     d_metricsAdapter_p->registerCollectionCallback(descriptor,
                                                                    callback);
    }
    else {
        data.d_handle = 0;
    }

    return MetricsRegistryRegistrationHandle(this, d_nextKey++);
}

void MetricsRegistry::removeMetricsAdapter(MetricsAdapter *adapter)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_metricsAdapter_p && d_metricsAdapter_p == adapter) {
        for (bsl::map<CallbackHandle, Data>::iterator iter =
                                                          d_metricData.begin();
             iter != d_metricData.end();
             ++iter) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                        iter->second.d_handle);
        }
        d_metricsAdapter_p = 0;
    }
}

void MetricsRegistry::setMetricsAdapter(MetricsAdapter *adapter)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    for (bsl::map<CallbackHandle, Data>::iterator iter = d_metricData.begin();
         iter != d_metricData.end();
         ++iter) {
        if (d_metricsAdapter_p) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                        iter->second.d_handle);
        }
        iter->second.d_handle = adapter->registerCollectionCallback(
                                                     iter->second.d_descriptor,
                                                     iter->second.d_callback);
    }

    d_metricsAdapter_p = adapter;
}

}  // close package namespace
}  // close enterprise namespace

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
