// baem_publisher.h              -*-C++-*-
#ifndef INCLUDED_BAEM_PUBLISHER
#define INCLUDED_BAEM_PUBLISHER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_publisher.h,v 1.6 2008/04/09 19:42:15 hversche Exp $")

//@PURPOSE: Provide a protocol to publish recorded metric values.
//
//@CLASSES:
//   baem_Publisher: a protocol providing a method to publish metric values
//
//@SEE_ALSO:
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a protocol class used for publishing
// metric values.  The protocol's primary method is 'publish', which takes
// a 'baem_MetricSample'.  The precise meaning of 'publish' is left to derived
// classes to specify.
//
///Usage
///-----
// In the following examples we create a simple implementation of a
// 'baem_Publisher', and then use it to publish metrics recorded by a trivial
// event manager.
//
///Example 1 -- Implementing the 'baem_Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates a simple implementation of the
// 'baem_Publisher' protocol.  This implementation publishes the metric
// records to an output stream provided on construction.
//..
//  // simplestreampublisher.h
//  class SimpleStreamPublisher : public baem_Publisher {
//      // A simple implementation of the 'baem_Publisher' protocol that
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
//      virtual void publish(const baem_MetricSample& metricValues);
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
//  void SimpleStreamPublisher::publish(const baem_MetricSample& metricValues)
//  {
//      if (0 >= metricValues.numRecords()) {
//          return;                                                   // RETURN
//      }
//      d_stream << metricValues.timeStamp() << " "
//               << metricValues.numRecords() << " Records" << bsl::endl;
//
//      baem_MetricSample::const_iterator sIt = metricValues.begin();
//      for (; sIt != metricValues.end(); ++sIt) {
//          d_stream << "\tElapsed Time: "
//                   << sIt->elapsedTime().totalSecondsAsDouble()
//                   << "s" << bsl::endl;
//          baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
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
///Example 2 -- Using the 'baem_Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following example defines a trivial 'EventManager' class that uses the
// 'baem_Publisher' protocol to publish metrics related to the incoming
// event.  Note that this event manager does no actual processing and is
// intended only to illustrate how the publisher protocol might be used.
//..
//  class EventManager {
//      // This class provides a dummy event handling mechanism that publishes
//      // a metric for the size of the processed event messages.
//
//      // DATA
//      baem_Collector  d_eventMessageSize;  // metric for the message size
//      bdet_DatetimeTz d_lastPublish;       // time of the last publication
//
//      // NOT IMPLEMENTED
//      EventManager(const EventManager& );
//      EventManager& operator=(const EventManager& );
//
//    public:
//      // CREATORS
//      EventManager(const baem_MetricId& messageSizeId)
//          // Create this event manager using the specified 'messageSizeId'
//          // to identify the event message size metric.
//      : d_eventMessageSize(messageSizeId)
//      , d_lastPublish(bdetu_SystemTime::nowAsDatetimeUTC(), 0)
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
// We use a 'baem_Publisher' to publish the metrics recorded by this event
// manager.  Note that most of the functionality illustrated here is
// normally provided by the 'baem_MetricsManager'.
//..
//      void publishMetrics(baem_Publisher *publisher)
//      {
//          bdet_DatetimeTz now(bdetu_SystemTime::nowAsDatetimeUTC(), 0);
//          bdet_DatetimeInterval dateInterval = now.utcDatetime() -
//                                               d_lastPublish.utcDatetime();
//          bdet_TimeInterval interval(dateInterval.totalSeconds(),
//                                     dateInterval.milliseconds());
//
//          baem_MetricRecord record;
//          d_eventMessageSize.loadAndReset(&record);
//
//          baem_MetricSample sample;
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
///Example 3 -- Publishing Collected Metrics Using 'EventManager'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we publish metrics collected for the 'EventManager'
// object (defined above).
//
// We start by creating a 'baem_MetricId' object by hand, but in
// practice, an id should be obtained from a 'baem_MetricRegistry' object
// (such as the one owned by a 'baem_MetricsManager').
//..
//  baem_Category           myCategory("MyCategory");
//  baem_MetricDescription  description(&myCategory, "EventMessageSize");
//  baem_MetricId           eventMessageSizeId(&description);
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
//  baem_Publisher *publisher = &myPublisher;
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

class baem_MetricSample;

                      // ====================
                      // class baem_Publisher
                      // ====================

class baem_Publisher {
    // This protocol class provides a 'publish' method to publish collected
    // sample of recorded metric values.  The precise meaning of publish is
    // left to derived classes to specify.

  public:
    // CREATORS
    virtual ~baem_Publisher();
        // Destroy this object.

    // MANIPULATORS
    virtual void publish(const baem_MetricSample& metricValue) = 0;
        // Publish the specified 'metricValue'.  The exact definition of
        // publish depends on the implementing class, though the intention is
        // that the recorded metric values will be distributed in a human or
        // machine readable form.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // --------------------
                      // class baem_Publisher
                      // --------------------

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
