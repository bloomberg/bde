// baea_metric.h                                                      -*-C++-*-
#ifndef INCLUDED_BAEA_METRIC
#define INCLUDED_BAEA_METRIC

//@PURPOSE: Provide the means to describe, gather, and report software metrics.
//
//@DEPRECATED: Use 'baem_metrics' instead.
//
//@CLASSES:
//  baea_Metric:         description of a metric
//  baea_MetricReporter: protocol for gathering and reporting metrics
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component provides a value-semantic type, 'baea_Metric',
// to describe a measured property of an application.  This component
// additionally describes a protocol, 'baea_MetricReporter', for gathering and
// reporting such metrics.
//
///Usage
///-----
// This example shows how a user-defined job processing mechanism can publish
// metrics using the 'baea_MetricReporter' interface.  In this example, the
// mechanism contains a queue, and collects metrics representing the minimum
// and maximum queue sizes over a configurable interval.  The metrics are
// "MinQueueSize" and "MaxQueueSize", respectively; and the collection of
// metrics are grouped under the "UserMetrics" category.  The component is
// defined as follows:
//..
//  class Job { /* ... */ };
//
//  class Processor {
//      // This class implements a thread-safe mechanism for processing
//      // 'Job' objects.  Jobs may be queued and processed in separate
//      // threads.  Metrics for minimum and maximum queue size are maintained.
//      // These metrics are defined as 'MinPendingJobs' and 'MaxPendingJobs',
//      // respectively; and the collection of metrics are grouped under the
//      // 'UserMetrics' category.
//
//      // DATA
//      bcemt_FixedThreadPool *d_threadPool_p;    // processing threads (held)
//      bces_AtomicInt64       d_numPendingJobs;  // current num pending jobs
//      bces_AtomicInt64       d_maxPendingJobs;  // maximum num pending jobs
//      bces_AtomicInt64       d_minPendingJobs;  // minimum num pending jobs
//
//    private:
//      // PRIVATE MANIPULATORS
//      double updateMetricCb(baea_Metric *metric, int index);
//          // Update the specified 'metric' with data derived from the
//          // specified 'index', and return the updated metric value.
//
//      void processJobImpl(const Job& job);
//          // Process the next job.
//
//      // not implemented
//      Processor(const Processor&);
//      Processor& operator=(const Processor&);
//
//    public:
//      // CREATORS
//      Processor(baea_MetricReporter  *reporter,
//                bcep_FixedThreadPool *threadPool);
//          // Create a job processor object that uses the specified
//          // 'threadPool' to process jobs, and registers metric callbacks
//          // with the specified metric 'reporter'.
//
//      ~Processor();
//          // Destroy this object.
//
//      // MANIPULATORS
//      void processJob(const Job& job);
//          // Process the specified 'job', possibly in a separate thread.
//  };
//..
// The job processor maintains the two metrics, "MinQueueSize" and
// "MaxQueueSize", and publishes them in the canonical way by registering a
// callback for each configured metric.  The callback, 'updateMetricCb', is
// responsible for updating the specified metric value stored in the metric
// reporter.
//
// As suggested above, components that aggregate metrics typically define the
// name and category identifying each metric within the component
// implementation.  To minimize the number of individual callback functions
// required to update each metric, it is common to see the following pattern
// within such components: The name of each metric is associated with an
// integer index, which is used as an argument to a single update callback to
// update the associated metric value.
//..
//  namespace {
//
//  enum MetricsList {
//      MINPENDINGJOBS
//    , MAXPENDINGJOBS
//  };
//
//  const struct {
//      const char *d_name;
//      int         d_id;
//  } metricsData[] = {
//      {"MinPendingRequests", MINPENDINGJOBS},
//      {"MaxPendingRequests", MAXPENDINGJOBS},
//  };
//  enum { METRICSDATA_SIZE = sizeof metricsData / sizeof *metricsData };
//
//  const char LOG_CATEGORY[] = "JOBPROCESSOR";
//  }  // close unnamed namespace
//
//  // PRIVATE MANIPULATORS
//  double Processor::updateMetricCb(baea_Metric *metric, int index)
//  {
//      // Called by the metric reporter to gather statistics.
//
//      double ret = 0;
//      switch(index) {
//        case MINPENDINGJOBS: {
//          ret = (double) d_minPendingJobs.swap(d_numPendingJobs);
//        } break;
//        case MAXPENDINGJOBS: {
//          ret = (double) d_maxPendingJobs.swap(d_numPendingJobs);
//        } break;
//        default:
//          BSLS_ASSERT(0);
//      }
//
//      metric->setValue(ret);
//      bdet_TimeInterval ts;
//      bdetu_SystemTime::loadSystemTimeDefault(&ts);
//      metric->setUpdateTimestamp(ts);
//
//      return ret;
//  }
//..
// The metric callbacks are set in the 'Processor' constructor:
//..
//  // CREATORS
//  Processor::Processor(
//          baea_MetricReporter  *reporter,
//          bcep_FixedThreadPool *threadPool)
//  : d_threadPool_p(threadPool)
//  , d_numPendingJobs(0)
//  , d_minPendingJobs(0)
//  , d_numPendingJobs(0)
//  {
//      BSLS_ASSERT(threadPool);
//
//      for (int i = 0; i < METRICSDATA_SIZE; ++i) {
//          int rc;
//
//          baea_Metric metric;
//          metric.setMetricName(metricsData[i].d_name);
//          metric.setCategory("UserMetrics");
//          metric.setReportingName(metricsData[i].d_name);
//          metric.setUpdateInterval(30.0);
//          metric.setValue(0.0);
//
//          rc = reporter.registerMetric(metric);
//          BSLS_ASSERT(0 != rc);
//
//          baea_MetricReporter::MetricCb cb =
//                  bdef_BindUtil::bind(&Processor::updateMetricCb,
//                                      this, bdef_PlaceHolders::_1,
//                                      metricsData[i].d_id);
//
//          rc = reporter->setMetricCb(metricsData[i].d_name,
//                                     "UserMetrics",
//                                     cb);
//          BSLS_ASSERT(0 != rc);
//      }
//  }
//
//  Processor::~Processor()
//  {
//  }
//..
// Finally, the metrics are aggregated in the public 'processJob' and
// private 'processJobImpl' methods:
//..
//  void Processor::processJobImpl(const Job& job)
//  {
//      --d_numPendingJobs;
//      d_minPendingJobs = bsl::min(d_minPendingJobs, d_numPendingJobs);
//
//      //...further processing...
//  }
//
//  void Processor::processJob(const Job& job)
//  {
//      // Enqueue a job processing callback to the thread pool.
//
//      bdef_Function<void(*)()> callback =
//                  bdef_BindUtil::bind(&Processor::processJobImpl, this, job);
//
//      ++d_numPendingJobs;
//      d_maxPendingJobs = bsl::max(d_maxPendingJobs, d_numPendingJobs);
//
//      d_threadPool_p->enqueueJob(callback);
//  }
//..
// The way in which metrics are gathered and reported is defined by a concrete
// implementation of the 'baea_MetricReporter' protocol.  Let's define an
// implementation 'MetricReporter' in the 'test' namespace that gathers metrics
// using a 'bcep_TimerEventScheduler' object.
//..
// namespace test {
//
// class MetricReporter {
//     // Provide an implementation of the 'baea_MetricReporter' protocol that
//     // publishes each registered metric at a specified interval.
//
//     // PRIVATE TYPES
//     typedef bsl::pair<bsl::string, bsl::string> RegistryKey;
//     typedef bsl::map<RegistryKey, baea_Metric>  Registry;
//
//     // DATA
//     bsl::ostream*            d_stream;     // output stream
//     Registry                 d_registry;   // tracks registered metrics
//     bcep_TimerEventScheduler d_scheduler;  // used to schedule events
//     bcemt_Mutex              d_mutex;      // ensures thread safety
//
//     // not implemented
//     MetricReporter(const MetricReporter&);
//     MetricReporter& operator=(const MetricReporter&);
//
//     // CREATORS
//     explicit MetricReporter(bslma_Allocator *basicAllocator = 0);
//         // Create a new 'MetricReporter'.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     // MANIPULATORS
//     virtual int registerMetric(const baea_Metric& information);
//         // Register the specify 'information' for gathering and reporting
//         // and return 0 on success, or return a non-zero value if a metric
//         // with the same name and entity has already been registered.
//
//     virtual int setMetricCb(const char      *name,
//                             const char      *category,
//                             const MetricCb&  metricCb);
//         // Set the callback to be called upon update of the metric
//         // registered in this reporter for the specified 'name' and
//         // 'category'.  Return 0 on success, a non-zero value of no metric
//         // was previously registered for the 'name' and 'entity'.
//
//     virtual baea_Metric *lookupMetric(const char *name,
//                                       const char *category);
//         // Return a pointer to the metric information object associated with
//         // the specified by 'name' and 'category', or a null pointer if no
//         // metric information is found.
//
//     // ACCESSORS
//     virtual bool isRegistered(const char *name, const char *category) const;
//         // Return true if a metric has already been registered with this
//         // reporter for the specified 'name' and 'category'.
//
//     virtual void printMetrics(bsl::ostream& stream) const;
//         // Print the metric information for all registered metrics to the
//         // specified 'stream'.
// };
//
// MetricReporter::MetricReporter(
//         bsl::ostream    *stream,
//         bslma_Allocator *basicAllocator)
// : d_stream(stream)
// , d_registry(bsl::less<bsl::string>(), basicAllocator)
// , d_scheduler(basicAllocator)
// , d_mutex()
// {
// }
//
// int MetricReporter::registerMetric(const baea_Metric& metric)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
//     return !d_registry.insert(
//         bsl::make_pair(metric.metricName(), metric.category()),
//             metric).second;
// }
//
// int MetricReporter::setMetricCb(const char      *name,
//                                 const char      *category,
//                                 const MetricCb&  metricCb)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
//     Registry::iterator it = d_registry.find(bsl::make_pair(name, category));
//     if (d_registry.end() == it) {
//         return -1;
//     }
//
//     baea_Metric* metric = &it->second;
//
//     d_scheduler.startClock(metric.updateInterval(),
//                            bdef_BindUtil::bind(metricCb, metric));
//     return 0;
// }
//
// baea_Metric *MetricReporter::lookupMetric(const char *name,
//                                           const char *category)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
//     Registry::iterator it = d_registry.find(bsl::make_pair(name, category));
//     return (d_registry.end() == it) ? 0 : &it->second;
// }
//
// bool MetricReporter::lookupMetric(const char *name,
//                                   const char *category) const
// {
//     return (0 != lookupMetric(name, category);
// }
//
// void MetricReporter::printMetrics(bsl::ostream& stream) const
// {
//     for (Registry::const_iterator it  = d_registry.begin();
//                                   it != d_registry.end();
//                                 ++it)
//     {
//         it->second.rowPrint(stream) << bsl::endl;
//     }
// }
//
// }  // close namespace test
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                            // =================
                            // class baea_Metric
                            // =================

class baea_Metric {
    // This class represents a metric.  More generally, this container class
    // supports a complete set of value semantic operations, including copy
    // construction, assignment, equality comparison, ostream printing, and
    // bdex serialization.  (A precise operational definition of when two
    // objects have the same value can be found in the description of
    // 'operator==' for the class.)  This container is exception neutral with
    // no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing object, the container is left
    // in an undefined state but can be destroyed.  In no event is memory
    // leaked.  Finally, aliasing (e.g., using all or part of an object as both
    // source and destination) is supported in all cases.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<double(*)()> MetricCb;
        // This type alias defines a callback that updates the value of this
        // metric.

  private:
    // DATA
    bsl::string       d_metricName;      // metric name
    bsl::string       d_category;        // category name
    bsl::string       d_reportingName;   // reporting name
    bdet_TimeInterval d_updateInterval;  // interval between metric update
    bdet_TimeInterval d_updateTimestamp; // last update timestamp
    double            d_value;           // last value
    MetricCb          d_metricCb;        // update callback

    // FRIENDS
    friend bool operator==(const baea_Metric& lhs, const baea_Metric& rhs);
    friend bool operator!=(const baea_Metric& lhs, const baea_Metric& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(baea_Metric,
                                  bdeu_TypeTraitHasPrintMethod,
                                  bslalg_TypeTraitUsesBslmaAllocator);

  public:
    // CREATORS
    explicit
    baea_Metric(bslma_Allocator *basicAllocator = 0);
        // Create a new metric information object.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    baea_Metric(const baea_Metric&  original,
                bslma_Allocator    *basicAllocator = 0);
        // Create a new metric information object having the same value as the
        // specified 'original'.  Optionally specify 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~baea_Metric();
        // Destroy this object.

    // MANIPULATORS
    baea_Metric& operator=(const baea_Metric& rhs);
        // Assign the value of the specified 'rhs' to this object and return a
        // modifiable reference to this object.

    void setCategory(const char *category);
        // Set the category name linked to the metric represented by this
        // object to the specified 'category'.

    void setMetricCb(const MetricCb& metricCb);
        // Set the metric callback of this metric represented by this object to
        // the specified 'metricCb'.

    void setMetricName(const char *metricName);
        // Set the metric name of this metric represented by this object to the
        // specified 'metricName'.

    void setReportingName(const char *name);
        // Set the name by which this metric is reported to specified 'name'.

    void setUpdateInterval(const bdet_TimeInterval& interval);
        // Set the update interval of the metric represented by this object to
        // the specified 'interval'.

    void setUpdateTimestamp(const bdet_TimeInterval& timestamp);
        // Set the last update timestamp of the metric represented by this
        // object to the specified 'timestamp'.

    void setValue(double value);
        // Set the lastest metric value for the metric represented by this
        // object to the specified 'value'.

    // ACCESSORS
    const bsl::string& category() const;
        // Return the category name measured by the metric represented by this
        // object.

    const MetricCb& metricCb() const;
        // Return the update callback registered for this object.

    const bsl::string& metricName() const;
        // Return the metric name represented by this object.

    const bsl::string& reportingName() const;
        // Return the name optionally used to publish the value of the metric.

    const bdet_TimeInterval& updateInterval() const;
        // Return the update interval of the metric represented by this object.

    const bdet_TimeInterval& updateTimestamp() const;
        // Return the last update timestamp of the metric represented by this
        // object.

    double value() const;
        // Return the last measured value of the metric represented by this
        // object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change
        // without notice.

    bsl::ostream& prettyPrint(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const;
        // Format this object in an easily readable way to the specified output
        // stream at the (absolute value of) the optionally specified
        // indentation level and return a reference to stream.  If level is
        // specified, optionally specify spacesPerLevel, the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // level is negative, suppress indentation of the first line.  If
        // spacesPerLevel is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by level).
        // If stream is not valid on entry, this operation has no effect.

    bsl::ostream& rowPrint(bsl::ostream& stream) const;
        // Output this object's name, sample interval, and value to the
        // specified 'stream' without newlines or attribute names.  If stream
        // is not valid on entry, this operation has no effect.  This method
        // returns 'stream'.
};

// FREE OPERATORS
bool operator==(const baea_Metric& lhs, const baea_Metric& rhs);
    // Return true if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baea_Metric' objects have the same
    // value if each of their corresponding 'category', 'metricName',
    // 'reportingName', 'updateInterval', 'updateTimestamp' and 'value'
    // attributes have the respectively the same value.

bool operator!=(const baea_Metric& lhs, const baea_Metric& rhs);
    // Return true if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baea_Metric' objects do not
    // have the same value if any of their corresponding 'category',
    // 'metricName', 'reportingName', 'updateInterval', 'updateTimestamp' and
    // 'value' attributes do not have respectively the same value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baea_Metric& rhs);
    // Write the value of the specified 'date' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

                         // =========================
                         // class baea_MetricReporter
                         // =========================

class baea_MetricReporter {
    // This component defines a protocol for metric reporting.  Concrete types
    // derived from this protocol gather and optionally publish metrics.

  public:
    // TYPES
    typedef bdef_Function<double(*)(baea_Metric *metric)> MetricCb;
        // This type alias defines a callback that updates the value of a
        // 'metric'.

    // CREATORS
    virtual ~baea_MetricReporter();
        // Destroy this object.

    // MANIPULATORS
    virtual int registerMetric(const baea_Metric& information) = 0;
        // Register the specified 'information' for gathering and reporting and
        // return 0 on success, or return a non-zero value if a metric with the
        // same name and category has already been registered.

    virtual int setMetricCb(const char      *name,
                            const char      *category,
                            const MetricCb&  metricCb) = 0;
        // Set the callback to be called upon update of the metric registered
        // in this reporter for the specified 'name' and 'category'.  Return 0
        // on success, and a non-zero value if a metric was previously
        // registered for the 'name' and 'category'.

    virtual baea_Metric *lookupMetric(const char *name,
                                      const char *category) = 0;
        // Return a pointer to the metric information object associated with
        // the specified by 'name' and 'category', and a null pointer if no
        // metric information is found.

    // ACCESSORS
    virtual bool isRegistered(const char *name,
                              const char *category) const = 0;
        // Return 'true' if a metric has already been registered with this
        // reporter for the specified 'name' and 'category'.

    virtual void printMetrics(bsl::ostream& stream) const = 0;
        // Print the metric information for all registered metrics to the
        // specified 'stream'.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const baea_Metric& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
