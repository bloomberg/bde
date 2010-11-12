// baea_baemmetricreporteradapter.h   -*-C++-*-
#ifndef INCLUDED_BAEA_BAEMMETRICREPORTERADAPTER
#define INCLUDED_BAEA_BAEMMETRICREPORTERADAPTER

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_baemmetricreporteradapter_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide a BAEM metric reporter adapter.
//
//@DEPRECATED: Use 'baem_metrics' instead.
//
//@CLASSES: baea_BaemMetricReporterAdapter: adapt BAEA metric reporter to BAEM
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO: baea_metric, baem_metric
//
//@DESCRIPTION: This component provides a simple adapter to adapt the baea
// metric reporter protocol to the baem metric reporting framework.  It gathers
// metrics and delegates them to a 'baea_MetricReporter' instance (held), as
// well as exposing a print facility.
//
///Usage Examples
///--------------

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEA_METRIC
#include <baea_metric.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class baem_MetricsManager;

                    // ====================================
                    // class baea_BaemMetricReporterAdapter
                    // ====================================

class baea_BaemMetricReporterAdapter : public baea_MetricReporter {
    // This class provides a simple adapter between the 'baea_MetricReporter'
    // protocol and the BAEM metric reporting framework.  It gathers metrics
    // and delegates them to a 'baea_MetricReporter' instance (held), as well
    // as exposing a print facility.

    // INSTANCE DATA
    baem_MetricsManager                  *d_manager_p;    // metrics manager
    bcema_SharedPtr<baea_MetricReporter>  d_reporter_sp;  // metrics reporter

    double callbackWrapper(
            baea_Metric                          *metric,
            const baea_MetricReporter::MetricCb&  callback);
        // Call the specified 'callback' with the specified 'metric' and then
        // publish the result of the 'callback' to a BAEM metric based on the
        // category and metric name of the 'metric'.

  private:
    // NOT IMPLEMENTED
    baea_BaemMetricReporterAdapter(const baea_BaemMetricReporterAdapter&);
    baea_BaemMetricReporterAdapter& operator=(
                                   const baea_BaemMetricReporterAdapter&);

  public:
    // CREATORS
    explicit
    baea_BaemMetricReporterAdapter(
            baem_MetricsManager                  *manager,
            bcema_SharedPtr<baea_MetricReporter>  reporter);
        // Create a new 'BaemMetricReporter' that uses the specified 'manager'
        // and 'reporter' to report metrics.

    virtual ~baea_BaemMetricReporterAdapter();
        // Destroy this object.

    // MANIPULATORS
    virtual int registerMetric(const baea_Metric& information);
        // Register the specify 'information' for gathering and reporting and
        // return 0 on success, or return a non-zero value if a metric with the
        // same name and entity has already been registered.

    virtual int setMetricCb(const char      *name,
                            const char      *entity,
                            const MetricCb&  metricCb);
        // Set the callback to be called upon update of the metric registered
        // in this reporter for the specified 'name' and 'entity'.  Return 0
        // on success, a non-zero value of no metric was previously registered
        // for the 'name' and 'entity'.

    virtual baea_Metric *lookupMetric(const char *name,
                                      const char *category);
        // Return a pointer to the metric information object associated with
        // the specified by 'name' and 'category', or a null pointer if no
        // metric information is found.

    // ACCESSORS
    virtual bool isRegistered(const char *name, const char *category) const;
        // Return true if a metric has already been registered with this
        // reporter for the specified 'name' and 'category'

    virtual void printMetrics(bsl::ostream& stream) const;
        // Print the metric information for all registered metrics to the
        // specified 'stream'.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// MANIPULATORS
inline
baea_Metric *baea_BaemMetricReporterAdapter::lookupMetric(const char *name,
                                                          const char *category)
{
    return d_reporter_sp->lookupMetric(name, category);
}

// ACCESSORS
inline
bool baea_BaemMetricReporterAdapter::isRegistered(const char *name,
                                                  const char *category) const
{
    return d_reporter_sp->isRegistered(name, category);
}

inline
void baea_BaemMetricReporterAdapter::printMetrics(bsl::ostream& stream) const
{
    d_reporter_sp->printMetrics(stream);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
