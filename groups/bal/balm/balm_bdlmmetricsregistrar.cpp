// balm_bdlmmetricsregistrar.cpp                                      -*-C++-*-

#include <balm_bdlmmetricsregistrar.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_bdlmmetricsregistrar_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace balm {

                        // --------------------------
                        // class BdlmMetricsRegistrar
                        // --------------------------

// PRIVATE CLASS METHODS
void BdlmMetricsRegistrar::metricCb(bsl::vector<MetricRecord> *records,
                                    bool                       resetFlag,
                                    MetricId                   id,
                                    const Callback&            callback)
{
    (void)resetFlag;

    bdlm::Metric value;
    callback(&value);

    if (value.isGauge()) {
        balm::MetricRecord record(id);

        record.count() = 1;
        record.total() = value.theGauge();

        records->push_back(record);
    }
}

// CREATORS
BdlmMetricsRegistrar::~BdlmMetricsRegistrar()
{
}

// MANIPULATORS
int BdlmMetricsRegistrar::incrementInstanceCount(
                                const bdlm::MetricDescriptor& metricDescriptor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return ++d_count[  metricDescriptor.metricNamespace() + '.'
                     + metricDescriptor.metricName() + '.'
                     + metricDescriptor.objectTypeName()];
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
