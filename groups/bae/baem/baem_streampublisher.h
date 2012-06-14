// baem_streampublisher.h              -*-C++-*-
#ifndef INCLUDED_BAEM_STREAMPUBLISHER
#define INCLUDED_BAEM_STREAMPUBLISHER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a 'baem_Publisher' implementation that writes to a stream.
//
//@CLASSES:
//  baem_StreamPublisher: publishes collected metric samples to a stream
//
//@SEE_ALSO: baem_publisher, baem_metricrecord, baem_metricsmanager
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a concrete class that implements the
// 'baem_Publisher' protocol for publishing metric records:
//..
//               ( baem_StreamPublisher )
//                           |              ctor
//                           |
//                           V
//                   ( baem_Publisher )
//                                          dtor
//                                          publish
//..
// This implementation of the publisher protocol publishes records to an output
// stream that is supplied at construction.
//
///Usage
///-----
// In the following example we illustrate how to create and publish records
// with a 'baem_StreamPublisher'.  First we define a couple of metric ids.
// Note that we create the 'baem_MetricId' objects by hand, but in practice an
// id should be obtained from a 'baem_MetricRegistry' object (such as the
// one owned by a 'baem_MetricsManager').
//..
//  baem_Category myCategory("MyCategory");
//  baem_MetricDescription descA(&myCategory, "MetricA");
//  baem_MetricDescription descB(&myCategory, "MetricB");
//
//  baem_MetricId metricA(&descA);
//  baem_MetricId metricB(&descB);
//..
// Now we create a 'baem_StreamPublisher' object, supplying it the 'stdout'
// stream:
//..
//  baem_StreamPublisher myPublisher(bsl::cout);
//..
// Next we create a metric sample containing some records and pass it to the
// 'baem_StreamPublisher' we created.  Note that because we are not actually
// collecting the metrics we set the elapsed time of the sample to an
// arbitrary 5s interval.
//..
//  bslma_Allocator *allocator = bslma_Default::allocator(0);
//  bsl::vector<baem_MetricRecord> records(allocator);
//
//  records.push_back(baem_MetricRecord(metricA, 5, 25.0, 6.0, 25.0));
//  records.push_back(baem_MetricRecord(metricB, 2,  7.0, 3.0, 11.0));
//
//  baem_MetricSample sample(allocator);
//
//  sample.setElapsedTime(bdet_TimeInterval(5));
//  sample.setTimeStamp(
//                 bdet_DatetimeTz(bdetu_SystemTime::nowAsDatetimeUtc(), 0));
//  sample.appendRecords(records.data(), records.size());
//
//  myPublisher.publish(sample);
//..
// The output of this example would look similar to:
//..
// 05FEB2009_19:52:11.723+0000 2 Records
//     Elapsed Time: 5s
//         MyCategory.MetricA [ count = 5, total = 25, min = 6, max = 25 ]
//         MyCategory.MetricB [ count = 2, total = 7, min = 3, max = 11 ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_PUBLISHER
#include <baem_publisher.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class baem_MetricSample;

                      // ==========================
                      // class baem_StreamPublisher
                      // ==========================

class baem_StreamPublisher : public baem_Publisher {
    // This class provides an implementation of the 'baem_Publisher' protocol.
    // This stream publisher publishes recorded metric values to an output
    // stream specified at construction.

    // DATA
    bsl::ostream& d_stream;   // stream to which to write data

    // NOT IMPLEMENTED
    baem_StreamPublisher(const baem_StreamPublisher& );
    baem_StreamPublisher& operator=(const baem_StreamPublisher& );

  public:
    // CREATORS
    baem_StreamPublisher(bsl::ostream& stream);
        // Create a streampublisher and initialize it to publish metrics to
        // the specified 'stream'.

    virtual ~baem_StreamPublisher();
        // Destroy this stream publisher.

    // MANIPULATORS
    virtual void publish(const baem_MetricSample& metricValues);
        // Publish the specified 'metricValues' to the output stream specified
        // at construction.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // --------------------------
                          // class baem_StreamPublisher
                          // --------------------------

// CREATORS
inline
baem_StreamPublisher::baem_StreamPublisher(bsl::ostream& stream)
: d_stream(stream)
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
