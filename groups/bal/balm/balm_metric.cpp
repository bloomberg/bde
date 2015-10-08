// balm_metric.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metric.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metric_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace balm {
                           // ---------------------
                           // class Metric_MacroImp
                           // ---------------------

// CLASS METHODS
void Metric_MacroImp::getCollector(Collector      **collector,
                                   CategoryHolder  *holder,
                                   const char      *category,
                                   const char      *metric)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder->enabled()' is
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
                              const char             *category,
                              const char             *metric,
                              PublicationType::Value  preferredPublicationType)
{
    // '*collector' must be assigned *before* registering 'holder' to ensure
    // that the macros have a valid 'collector' when 'holder->enabled()' is
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
