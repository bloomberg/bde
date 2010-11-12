// baea_baemmetricreporteradapter.cpp   -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_baemmetricreporteradapter_cpp,"$Id$ $CSID$")

#include <baea_baemmetricreporteradapter.h>

#include <bael_log.h>
#include <baem_metric.h>
#include <baem_metricid.h>
#include <baem_metricregistry.h>
#include <baem_metricsmanager.h>
#include <baem_publicationtype.h>

#include <bdef_bind.h>
#include <bdef_placeholder.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace {

const char LOG_CATEGORY[] = "BAEA.BMRA";

}  // close unnamed namespace

                    // ------------------------------------
                    // class baea_BaemMetricReporterAdapter
                    // ------------------------------------

// CREATORS

baea_BaemMetricReporterAdapter::baea_BaemMetricReporterAdapter(
        baem_MetricsManager                  *manager,
        bcema_SharedPtr<baea_MetricReporter>  reporter)
: d_manager_p(manager)
, d_reporter_sp(reporter)
{
    BSLS_ASSERT(manager);
    BSLS_ASSERT(reporter);
}

baea_BaemMetricReporterAdapter::~baea_BaemMetricReporterAdapter()
{
}

// PRIVATE MANIPULATORS

double baea_BaemMetricReporterAdapter::callbackWrapper(
        baea_Metric                          *metric,
        const baea_MetricReporter::MetricCb&  callback)
{
    double value = callback(metric);

    baem_CollectorRepository&  repository = d_manager_p->collectorRepository();
    baem_Collector            *collector  = repository.getDefaultCollector(
                                                metric->category().c_str(),
                                                metric->metricName().c_str());
    collector->update(value);
    return value;
}

// MANIPULATORS

int baea_BaemMetricReporterAdapter::registerMetric(
        const baea_Metric& information)
{
    int rc = d_reporter_sp->registerMetric(information);
    if (0 == rc) {
        baem_MetricRegistry& registry = d_manager_p->metricRegistry();
        baem_MetricId        metricId = registry.addId(
                                            information.category().c_str(),
                                            information.metricName().c_str());
        BSLS_ASSERT(metricId.isValid());

        registry.setPreferredPublicationType(metricId,
                                             baem_PublicationType::BAEM_TOTAL);

        BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
        BAEL_LOG_DEBUG << "Registered metric "
                       << metricId.categoryName()
                       << "."
                       << metricId.metricName()
                       << BAEL_LOG_END;
    }

    return rc;
}

int baea_BaemMetricReporterAdapter::setMetricCb(const char      *name,
                                                const char      *category,
                                                const MetricCb&  callback)
{
    int rc = d_reporter_sp->setMetricCb(
            name,
            category,
            bdef_BindUtil::bind(
                             &baea_BaemMetricReporterAdapter::callbackWrapper,
                             this,
                             bdef_PlaceHolders::_1,
                             callback));
    return rc;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
