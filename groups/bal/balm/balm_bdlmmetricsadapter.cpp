// balm_bdlmmetricsadapter.cpp                                        -*-C++-*-

#include <balm_bdlmmetricsadapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_bdlmmetricsadapter_cpp,"$Id$ $CSID$")

#include <bsl_sstream.h>

namespace BloombergLP {
namespace balm {

                         // ------------------------
                         // class BdlmMetricsAdapter
                         // ------------------------

// PRIVATE CLASS METHODS
void BdlmMetricsAdapter::metricCb(bsl::vector<MetricRecord> *records,
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
BdlmMetricsAdapter::~BdlmMetricsAdapter()
{
}

// MANIPULATORS
bdlm::MetricsAdapter::CallbackHandle
BdlmMetricsAdapter::registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback)
{
    bdlm::MetricDescriptor md(metricDescriptor);
    if (md.metricNamespace() ==
           bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION) {
        md.setMetricNamespace(d_metricNamespace);
    }
    if (md.objectIdentifier() ==
           bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION) {
        bsl::stringstream identifier;
        identifier << d_objectIdentifierPrefix
                   << '.'
                   << md.objectTypeAbbreviation()
                   << '.'
                   << md.instanceNumber();
        md.setObjectIdentifier(identifier.str());
    }

    bsl::string name = md.objectTypeName()   + '.'
                     + md.metricName()       + '.'
                     + md.objectIdentifier();

    MetricId id = d_metricsManager_p->metricRegistry().getId(
                                   md.metricNamespace().c_str(), name.c_str());

    return d_metricsManager_p->registerCollectionCallback(
                                   md.metricNamespace().c_str(),
                                   bdlf::BindUtil::bind(&metricCb,
                                                        bdlf::PlaceHolders::_1,
                                                        bdlf::PlaceHolders::_2,
                                                        id,
                                                        callback));
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
