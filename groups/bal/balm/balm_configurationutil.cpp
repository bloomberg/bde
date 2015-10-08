// balm_configurationutil.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_configurationutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_configurationutil_cpp,"$Id$ $CSID$")

#include <balm_defaultmetricsmanager.h>
#include <balm_metricformat.h>
#include <balm_metricregistry.h>
#include <balm_metrics.h>
#include <balm_metricsmanager.h>
#include <balm_metricid.h>

#include <balm_metricsample.h>   // for testing only
#include <balm_publisher.h>      // for testing only

#include <bsl_cstring.h>   // 'strlen'
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace balm {
                          // ------------------------
                          // struct ConfigurationUtil
                          // ------------------------

// CLASS METHODS
int ConfigurationUtil::setFormat(const char          *category,
                                 const char          *metricName,
                                 const MetricFormat&  format,
                                 MetricsManager      *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    MetricRegistry& registry = manager->metricRegistry();
    registry.setFormat(registry.getId(category, metricName), format);
    return 0;
}

int ConfigurationUtil::setFormatSpec(const char              *category,
                                     const char              *metricName,
                                     PublicationType::Value   publicationType,
                                     const MetricFormatSpec&  formatSpec,
                                     MetricsManager          *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    MetricRegistry& registry = manager->metricRegistry();
    MetricId        metricId = registry.getId(category, metricName);

    bsl::shared_ptr<const MetricFormat> currentFormat =
                                              metricId.description()->format();

    MetricFormat newFormat;
    if (currentFormat) {
        newFormat = *currentFormat;
    }

    newFormat.setFormatSpec(publicationType, formatSpec);
    registry.setFormat(metricId, newFormat);
    return 0;
}

int ConfigurationUtil::setPreferredPublicationType(
                                       const char             *category,
                                       const char             *metricName,
                                       PublicationType::Value  publicationType,
                                       MetricsManager         *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    MetricRegistry& registry = manager->metricRegistry();
    registry.setPreferredPublicationType(registry.getId(category, metricName),
                                         publicationType);
    return 0;
}

MetricDescription::UserDataKey ConfigurationUtil::createUserDataKey(
                                                  MetricsManager *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    return manager->metricRegistry().createUserDataKey();
}

void ConfigurationUtil::setUserData(const char                     *category,
                                    const char                     *metricName,
                                    MetricDescription::UserDataKey  key,
                                    const void                     *value,
                                    MetricsManager                 *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return;                                                       // RETURN
    }
    MetricRegistry& registry = manager->metricRegistry();

    MetricId id = registry.getId(category, metricName);
    registry.setUserData(id, key, value);
}

void ConfigurationUtil::setUserData(
                                  const char                     *categoryName,
                                  MetricDescription::UserDataKey  key,
                                  const void                     *value,
                                  MetricsManager                 *manager)
{
    manager = DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return;                                                       // RETURN
    }
    MetricRegistry& registry = manager->metricRegistry();

    const int length = bsl::strlen(categoryName) - 1;
    if (length >= 0 && '*' == categoryName[length]) {
        bsl::string prefix(categoryName, length);

        registry.setUserData(prefix.c_str(), key, value, true);
    }
    else {
        registry.setUserData(categoryName, key, value, false);
    }
}

}  // close package namespace
}  // close enterprise namespace

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
