// baem_configurationutil.cpp                                         -*-C++-*-
#include <baem_configurationutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_configurationutil_cpp,"$Id$ $CSID$")

#include <baem_defaultmetricsmanager.h>
#include <baem_metricformat.h>
#include <baem_metricregistry.h>
#include <baem_metrics.h>
#include <baem_metricsmanager.h>

#include <baem_metricsample.h>   // for testing only
#include <baem_publisher.h>      // for testing only

#include <bcema_sharedptr.h>

#include <bsl_cstring.h>   // 'strlen'
#include <bsl_string.h>

namespace BloombergLP {

                      // -----------------------------
                      // struct baem_ConfigurationUtil
                      // -----------------------------

// CLASS METHODS
int baem_ConfigurationUtil::setFormat(const char               *category,
                                      const char               *metricName,
                                      const baem_MetricFormat&  format,
                                      baem_MetricsManager      *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;
    }
    baem_MetricRegistry& registry = manager->metricRegistry();
    registry.setFormat(registry.getId(category, metricName), format);
    return 0;
}

int baem_ConfigurationUtil::setFormatSpec(
                                 const char                   *category,
                                 const char                   *metricName,
                                 baem_PublicationType::Value   publicationType,
                                 const baem_MetricFormatSpec&  formatSpec,
                                 baem_MetricsManager          *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;
    }
    baem_MetricRegistry& registry = manager->metricRegistry();
    baem_MetricId        metricId = registry.getId(category, metricName);

    bcema_SharedPtr<const baem_MetricFormat> currentFormat =
                                              metricId.description()->format();

    baem_MetricFormat newFormat;
    if (currentFormat) {
        newFormat = *currentFormat;
    }

    newFormat.setFormatSpec(publicationType, formatSpec);
    registry.setFormat(metricId, newFormat);
    return 0;
}

int baem_ConfigurationUtil::setPreferredPublicationType(
                                  const char                  *category,
                                  const char                  *metricName,
                                  baem_PublicationType::Value  publicationType,
                                  baem_MetricsManager         *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    baem_MetricRegistry& registry = manager->metricRegistry();
    registry.setPreferredPublicationType(registry.getId(category, metricName),
                                         publicationType);
    return 0;
}

baem_MetricDescription::UserDataKey baem_ConfigurationUtil::createUserDataKey(
                                                  baem_MetricsManager *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return -1;                                                    // RETURN
    }
    return manager->metricRegistry().createUserDataKey();
}

void baem_ConfigurationUtil::setUserData(
                               const char                          *category,
                               const char                          *metricName,
                               baem_MetricDescription::UserDataKey  key,
                               const void                          *value,
                               baem_MetricsManager                 *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return;                                                       // RETURN
    }
    baem_MetricRegistry& registry = manager->metricRegistry();

    baem_MetricId id = registry.getId(category, metricName);
    registry.setUserData(id, key, value);
}

void baem_ConfigurationUtil::setUserData(
                             const char                          *categoryName,
                             baem_MetricDescription::UserDataKey  key,
                             const void                          *value,
                             baem_MetricsManager                 *manager)
{
    manager = baem_DefaultMetricsManager::manager(manager);
    if (0 == manager) {
        return;                                                       // RETURN
    }
    baem_MetricRegistry& registry = manager->metricRegistry();

    const int length = bsl::strlen(categoryName) - 1;
    if (length >= 0 && '*' == categoryName[length]) {
        bsl::string prefix(categoryName, length);

        registry.setUserData(prefix.c_str(), key, value, true);
    }
    else {
        registry.setUserData(categoryName, key, value, false);
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
