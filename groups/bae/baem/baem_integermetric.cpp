// baem_integermetric.cpp      -*-C++-*-
#include <baem_integermetric.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_integermetric_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                          // ------------------------
                          // class baem_IntegerMetric
                          // ------------------------

// PRIVATE CONSTANTS
const bool baem_IntegerMetric::NOT_ACTIVE = false;

                      // ---------------------------------
                      // class baem_IntegerMetric_MacroImp
                      // ---------------------------------

// CLASS METHODS
void baem_IntegerMetric_MacroImp::getCollector(
                                          baem_IntegerCollector **collector,
                                          baem_CategoryHolder    *holder,
                                          const char             *category,
                                          const char             *metric)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultIntegerCollector(
                                                             category, metric);
    manager->metricRegistry().registerCategoryHolder(
                                 (*collector)->metricId().category(), holder);
}

void baem_IntegerMetric_MacroImp::getCollector(
                        baem_IntegerCollector      **collector,
                        baem_CategoryHolder         *holder,
                        const char                  *category,
                        const char                  *metric,
                        baem_PublicationType::Value  preferredPublicationType)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder.enabled()' is
    // 'true'.
    baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
    *collector = manager->collectorRepository().getDefaultIntegerCollector(
                                                             category, metric);
    manager->metricRegistry().registerCategoryHolder(
                                 (*collector)->metricId().category(), holder);
    manager->metricRegistry().setPreferredPublicationType(
                          (*collector)->metricId(), preferredPublicationType);
}


}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
