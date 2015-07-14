// balm_metric.cpp      -*-C++-*-
#include <balm_metric.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metric_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                          // -----------------
                          // class balm::Metric
                          // -----------------

// PRIVATE CONSTANTS
const bool balm::Metric::NOT_ACTIVE = false;

namespace balm {
                      // --------------------------
                      // class Metric_MacroImp
                      // --------------------------

// CLASS METHODS
void Metric_MacroImp::getCollector(Collector      **collector,
                                        CategoryHolder  *holder,
                                        const char           *category,
                                        const char           *metric)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    MetricsManager *manager = DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultCollector(category,
                                                                    metric);
    manager->metricRegistry().registerCategoryHolder(
                                 (*collector)->metricId().category(), holder);
}

void Metric_MacroImp::getCollector(
                        Collector             **collector,
                        CategoryHolder         *holder,
                        const char                  *category,
                        const char                  *metric,
                        PublicationType::Value  preferredPublicationType)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    MetricsManager *manager = DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultCollector(category,
                                                                    metric);
    manager->metricRegistry().registerCategoryHolder(
                                 (*collector)->metricId().category(), holder);
    manager->metricRegistry().setPreferredPublicationType(
                          (*collector)->metricId(), preferredPublicationType);
}
}  // close package namespace


}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
