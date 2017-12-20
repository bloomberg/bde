// balm_publisher.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_PUBLISHER
#define INCLUDED_BALM_PUBLISHER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: balm_publisher.h,v 1.6 2008/04/09 19:42:15 hversche Exp $")

//@PURPOSE: Provide a protocol to publish recorded metric values.
//
//@CLASSES:
//   balm::Publisher: a protocol providing a method to publish metric values
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a protocol class 'balm::Publisher' used
// for publishing metric values.  The protocol's primary method is 'publish',
// which takes a 'balm::MetricSample'.  The precise meaning of 'publish' is
// left to derived classes to specify.
//
///Usage
///-----
// In the following examples we create a simple implementation of a
// 'balm::Publisher', and then use it to publish metrics recorded by a trivial
// event manager.
//
///Example 1: Implementing the 'balm::Publisher' Protocol
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates a simple implementation of the
// 'balm::Publisher' protocol.  This implementation publishes the metric
// records to an output stream provided on construction.
//..
//  // simplestreampublisher.h
//  class SimpleStreamPublisher : public balm::Publisher {
//      // A simple implementation of the 'balm::Publisher' protocol that
//      // writes metric records to a stream.
//
//      // DATA
//      bsl::ostream& d_stream; // output stream (held, not owned)
//
//      // NOT IMPLEMENTED
//      SimpleStreamPublisher(const SimpleStreamPublisher& );
//      SimpleStreamPublisher& operator=(const SimpleStreamPublisher& );
//
//  public:
//      // CREATORS
//      SimpleStreamPublisher(bsl::ostream& stream);
//          // Create this publisher that will publish metrics to the specified
//          // 'stream'.
//
//      virtual ~SimpleStreamPublisher();
//           // Destroy this publisher.
//
//      // MANIPULATORS
//      virtual void publish(const balm::MetricSample& metricValues);
//          // Publish the specified 'metricValues'.  This implementation will
//          // write the 'metricValues' to the output stream specified on
//          // construction.
//  };
//
//  // simplestreampublisher.cpp
//
//  // CREATORS
//  SimpleStreamPublisher::SimpleStreamPublisher(bsl::ostream& stream)
//  : d_stream(stream)
//  {
//  }
//
//  SimpleStreamPublisher::~SimpleStreamPublisher()
//  {
//  }
//
//  // MANIPULATORS
//  void SimpleStreamPublisher::publish(const balm::MetricSample& metricValues)
//  {
//      if (0 >= metricValues.numRecords()) {
//          return;                                                   // RETURN
//      }
//      d_stream << metricValues.timeStamp() << " "
//               << metricValues.numRecords() << " Records" << bsl::endl;
//
//      balm::MetricSample::const_iterator sIt = metricValues.begin();
//      for (; sIt != metricValues.end(); ++sIt) {
//          d_stream << "\tElapsed Time: "
//                   << sIt->elapsedTime().totalSecondsAsDouble()
//                   << "s" << bsl::endl;
//          balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
//          for (; gIt != sIt->end(); ++gIt) {
//              d_stream << "\t" << gIt->metricId()
//                       << " [count = " << gIt->count()
//                       << ", total = " << gIt->total()
//                       << ", min = "   << gIt->min()
//                       << ", max = "   << gIt->max() << "]" << bsl::endl;
//          }
//      }
//  }
//..
//
///Example 2: Using the 'balm::Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following example defines a trivial 'EventManager' class that uses the
// 'balm::Publisher' protocol to publish metrics related to the incoming
// event.  Note that this event manager does no actual processing and is
// intended only to illustrate how the publisher protocol might be used.
//..
//  class EventManager {
//      // This class provides a dummy event handling mechanism that publishes
//      // a metric for the size of the processed event messages.
//
//      // DATA
//      balm::Collector  d_eventMessageSize;  // metric for the message size
//      bdlt::DatetimeTz d_lastPublish;       // time of the last publication
//
//      // NOT IMPLEMENTED
//      EventManager(const EventManager& );
//      EventManager& operator=(const EventManager& );
//
//    public:
//      // CREATORS
//      EventManager(const balm::MetricId& messageSizeId)
//          // Create this event manager using the specified 'messageSizeId'
//          // to identify the event message size metric.
//      : d_eventMessageSize(messageSizeId)
//      , d_lastPublish(bdlt::CurrentTime::nowAsDatetimeUTC(), 0)
//      {}
//
//      // MANIPULATORS
//      int handleEvent(int eventId, const bsl::string& eventMessage)
//          // Process the event described by the specified 'eventId' and
//          // 'eventMessage'.  Return 0 on success, and a non-zero value if
//          // there was an error processing the event.
//      {
//          // Update the metrics with the size of the 'eventMessage'.
//          d_eventMessageSize.update(eventMessage.size());
//
//          // ...   process the event
//
//          return 0;
//      }
//..
// We use a 'balm::Publisher' to publish the metrics recorded by this event
// manager.  Note that most of the functionality illustrated here is normally
// provided by the 'balm::MetricsManager'.
//..
//      void publishMetrics(balm::Publisher *publisher)
//      {
//          bdlt::DatetimeTz now(bdlt::CurrentTime::nowAsDatetimeUTC(), 0);
//          bdlt::DatetimeInterval dateInterval = now.utcDatetime() -
//                                               d_lastPublish.utcDatetime();
//          bsls::TimeInterval interval(dateInterval.totalSeconds(),
//                                     dateInterval.milliseconds());
//
//          balm::MetricRecord record;
//          d_eventMessageSize.loadAndReset(&record);
//
//          balm::MetricSample sample;
//          sample.setTimeStamp(now);
//          sample.appendGroup(&record, 1, interval);
//
//          // This is where we make use of the publisher argument to this
//          // function.
//          publisher->publish(sample);
//
//          d_lastPublish = now;
//      }
//  };
//..
//
///Example 3: Publishing Collected Metrics Using 'EventManager'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we publish metrics collected for the 'EventManager'
// object (defined above).
//
// We start by creating a 'balm::MetricId' object by hand, but in practice, an
// id should be obtained from a 'balm::MetricRegistry' object (such as the one
// owned by a 'balm::MetricsManager').
//..
//  balm::Category           myCategory("MyCategory");
//  balm::MetricDescription  description(&myCategory, "EventMessageSize");
//  balm::MetricId           eventMessageSizeId(&description);
//..
// Now we create a 'EventManager' object and supply it the metric id we have
// created.
//..
//  EventManager eventManager(eventMessageSizeId);
//..
// We use the 'EventManager' object to process two events and then publish the
// metrics for those events with a 'SimpleStreamPublisher' object (also defined
// above).
//..
//  eventManager.handleEvent(0, "123");
//  eventManager.handleEvent(0, "456789");
//
//  SimpleStreamPublisher myPublisher(bsl::cout);
//  balm::Publisher *publisher = &myPublisher;
//  eventManager.publishMetrics(publisher);
//..
// Note that we have delivered two events, with the messages "123" and
// "456789", so the count should be 2, the total message size should be 9, the
// minimum should be 3, and the maximum should be 6.  The output to the
// console should be:
//..
// 05FEB2009_19:49:30.173+0000 1 Records
//         Elapsed Time: 1e-09s
//         MyCategory.EventMessageSize [count = 2, total = 9, min = 3, max = 6]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

namespace BloombergLP {


namespace balm {

class MetricSample;

                              // ===============
                              // class Publisher
                              // ===============

class Publisher {
    // This protocol class provides a 'publish' method to publish collected
    // sample of recorded metric values.  The precise meaning of publish is
    // left to derived classes to specify.

  public:
    // CREATORS
    virtual ~Publisher();
        // Destroy this object.

    // MANIPULATORS
    virtual void publish(const MetricSample& metricValue) = 0;
        // Publish the specified 'metricValue'.  The exact definition of
        // publish depends on the implementing class, though the intention is
        // that the recorded metric values will be distributed in a human or
        // machine readable form.
};

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
