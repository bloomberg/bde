// balm_streampublisher.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_STREAMPUBLISHER
#define INCLUDED_BALM_STREAMPUBLISHER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a 'balm::Publisher' implementation that writes to a stream.
//
//@CLASSES:
//  balm::StreamPublisher: publishes collected metric samples to a stream
//
//@SEE_ALSO: balm_publisher, balm_metricrecord, balm_metricsmanager
//
//@DESCRIPTION: This component defines a concrete class
// 'balm::StreamPublisher' that implements the 'balm::Publisher' protocol for
// publishing metric records:
//..
//               ( balm::StreamPublisher )
//                           |              ctor
//                           |
//                           V
//                   ( balm::Publisher )
//                                          dtor
//                                          publish
//..
// This implementation of the publisher protocol publishes records to an output
// stream that is supplied at construction.
//
///Usage
///-----
// In the following example we illustrate how to create and publish records
// with a 'balm::StreamPublisher'.  First we define a couple of metric ids.
// Note that we create the 'balm::MetricId' objects by hand, but in practice an
// id should be obtained from a 'balm::MetricRegistry' object (such as the one
// owned by a 'balm::MetricsManager').
//..
//  balm::Category myCategory("MyCategory");
//  balm::MetricDescription descA(&myCategory, "MetricA");
//  balm::MetricDescription descB(&myCategory, "MetricB");
//
//  balm::MetricId metricA(&descA);
//  balm::MetricId metricB(&descB);
//..
// Now we create a 'balm::StreamPublisher' object, supplying it the 'stdout'
// stream:
//..
//  balm::StreamPublisher myPublisher(bsl::cout);
//..
// Next we create a metric sample containing some records and pass it to the
// 'balm::StreamPublisher' we created.  Note that because we are not actually
// collecting the metrics we set the elapsed time of the sample to an
// arbitrary 5s interval.
//..
//  bslma::Allocator *allocator = bslma::Default::allocator(0);
//  bsl::vector<balm::MetricRecord> records(allocator);
//
//  records.push_back(balm::MetricRecord(metricA, 5, 25.0, 6.0, 25.0));
//  records.push_back(balm::MetricRecord(metricB, 2,  7.0, 3.0, 11.0));
//
//  balm::MetricSample sample(allocator);
//
//  sample.setTimeStamp(bdlt::DatetimeTz(bdlt::CurrentTime::utc(), 0));
//  sample.appendGroup(records.data(), records.size(),
//                     bsls::TimeInterval(5, 0));
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_PUBLISHER
#include <balm_publisher.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {


namespace balm {

class MetricSample;

                           // =====================
                           // class StreamPublisher
                           // =====================

class StreamPublisher : public Publisher {
    // This class provides an implementation of the 'Publisher' protocol.
    // This stream publisher publishes recorded metric values to an output
    // stream specified at construction.

    // DATA
    bsl::ostream& d_stream;   // stream to which to write data

    // NOT IMPLEMENTED
    StreamPublisher(const StreamPublisher& );
    StreamPublisher& operator=(const StreamPublisher& );

  public:
    // CREATORS
    StreamPublisher(bsl::ostream& stream);
        // Create a streampublisher and initialize it to publish metrics to
        // the specified 'stream'.

    virtual ~StreamPublisher();
        // Destroy this stream publisher.

    // MANIPULATORS
    virtual void publish(const MetricSample& metricValues);
        // Publish the specified 'metricValues' to the output stream specified
        // at construction.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class StreamPublisher
                           // ---------------------

// CREATORS
inline
StreamPublisher::StreamPublisher(bsl::ostream& stream)
: d_stream(stream)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
