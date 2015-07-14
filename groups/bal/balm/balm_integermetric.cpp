// balm_integermetric.cpp      -*-C++-*-
#include <balm_integermetric.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_integermetric_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                          // ------------------------
                          // class balm::IntegerMetric
                          // ------------------------

// PRIVATE CONSTANTS
const bool balm::IntegerMetric::NOT_ACTIVE = false;

namespace balm {
                      // ---------------------------------
                      // class IntegerMetric_MacroImp
                      // ---------------------------------

// CLASS METHODS
void IntegerMetric_MacroImp::getCollector(
                                          IntegerCollector **collector,
                                          CategoryHolder    *holder,
                                          const char             *category,
                                          const char             *metric)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    MetricsManager *manager = DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultIntegerCollector(
                                                             category, metric);
    manager->metricRegistry().registerCategoryHolder(
                                 (*collector)->metricId().category(), holder);
}

void IntegerMetric_MacroImp::getCollector(
                        IntegerCollector      **collector,
                        CategoryHolder         *holder,
                        const char                  *category,
                        const char                  *metric,
                        PublicationType::Value  preferredPublicationType)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    MetricsManager *manager = DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultIntegerCollector(
                                                             category, metric);
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
