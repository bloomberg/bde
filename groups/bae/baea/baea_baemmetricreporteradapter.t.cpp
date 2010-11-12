// baea_baemmetricreporteradapter.t.cpp   -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_baemmetricreporteradapter_t_cpp,"$Id$ $CSID$")

#include <baea_baemmetricreporteradapter.h>
#include <baea_baemmetricreporteradapter.h>

#include <baea_metric.h>
#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>
#include <baem_defaultmetricsmanager.h>
#include <baem_metricsmanager.h>

#include <bcema_testallocator.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
// CREATORS
// [  ] baem_BaemMetricReporterAdapter
// [  ] ~baem_BaemMetricReporterAdapter
//
// MANIPULATORS
// [  ] registerMetric
// [  ] setMetricCb
// [  ] lookupMetric
//
// ACCESSORS
// [  ] isRegistered
// [  ] printMetrics
//
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() bsl::cout << '\t' << bsl::flush; // Print tab w/o newline.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baea_BaemMetricReporterAdapter Obj;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

const char LOG_CATEGORY[] = "BAEA.BMRA.TEST";

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace test {

class MetricReporter : public baea_MetricReporter {
    // This component defines the base-level protocol for metric reporting;
    // concrete types derived from this protocol gather and optionally publish
    // metric.

    int *d_funcCode_p;    // held

  private:
    MetricReporter(const MetricReporter&);
    MetricReporter& operator=(MetricReporter&);

  public:
    // CREATORS
    MetricReporter(int *funcCode);

    virtual ~MetricReporter();
        // Destroy this metric reporter.

    // MANIPULATORS
    virtual int registerMetric(const baea_Metric& information);
        // Register the specify 'information' for gathering and reporting and
        // return 0 on success, or return a non-zero value if a metric with the
        // same name and category has already been registered.

    virtual int setMetricCb(const char     *name,
                            const char     *category,
                            const MetricCb& metricCb);
        // Set the callback to be called upon update of the metric registered
        // in this reporter for the specified 'name' and 'category'.  Return 0
        // on success, a non-zero value of no metric was previously registered
        // for the 'name' and 'category'.

    virtual baea_Metric *lookupMetric(const char *name,
                                      const char *category);
        // Return a pointer to the metric information object associated with
        // the specified by 'name' and 'category', or a null pointer if no
        // metric information is found.

    // ACCESSORS
    virtual bool isRegistered(const char *name, const char *category) const;
        // Return true if a metric has already been registered with this
        // reporter for the specified 'name' and 'category'.

    virtual void printMetrics(bsl::ostream& stream) const;
        // Print the metric information for all registered metrics to the
        // specified 'stream'.
};

MetricReporter::MetricReporter(int *funcCode)
: d_funcCode_p(funcCode)
{
    BSLS_ASSERT(d_funcCode_p);

    *d_funcCode_p = 0;
}

MetricReporter::~MetricReporter()
{
    *d_funcCode_p = 6;
}

int MetricReporter::registerMetric(const baea_Metric&)
{
    *d_funcCode_p = 1;
    return 0;
}

int MetricReporter::setMetricCb(const char     *name,
                                    const char     *category,
                                    const MetricCb& metricCb)
{
    *d_funcCode_p = 2;
    return 0;
}

baea_Metric *MetricReporter::lookupMetric(const char *name,
                                              const char *category)
{
    *d_funcCode_p = 3;
    return 0;
}

bool
MetricReporter::isRegistered(const char *name, const char *category) const
{
    *d_funcCode_p = 4;
    return true;
}

void MetricReporter::printMetrics(bsl::ostream& stream) const
{
    *d_funcCode_p = 5;
}

}  // close namespace test

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bael_DefaultObserver            observer(&bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager::initSingleton(&observer, configuration);

    bael_Severity::Level passthrough = bael_Severity::BAEL_OFF;
    if (verbose)         passthrough = bael_Severity::BAEL_WARN;
    if (veryVerbose)     passthrough = bael_Severity::BAEL_INFO;
    if (veryVeryVerbose) passthrough = bael_Severity::BAEL_TRACE;

    bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                       bael_Severity::BAEL_OFF,
                                       passthrough,
                                       bael_Severity::BAEL_OFF,
                                       bael_Severity::BAEL_OFF);

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcema_TestAllocator         da(veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&da);

    baem_DefaultMetricsManagerScopedGuard metricsManagerGuard(bsl::cout);
    baem_MetricsManager *dmm = baem_DefaultMetricsManager::instance();
    BSLS_ASSERT(0 != dmm);

    switch (test) {
      case 0: // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the
        //   'baea_BaemMetricReporterAdapter' class.  Ensure that adapter
        //   objects can be instantiated and destroyed.  Additionally exercise
        //   the primary manipulators and accessors.
        //
        // Plan:
        //   Instantiate a 'baea_BaemMetricReporterAdapter', 'mX'.  Call the
        //   primary manipulators and accessors.
        //
        // Testing:
        //   Exercise basic functionality
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            int fcode = -1;

            bdema_ManagedPtr<baea_MetricReporter> mr_mp(
                    new (ta) test::MetricReporter(&fcode),
                    &ta);

            Obj mX(dmm, mr_mp);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
