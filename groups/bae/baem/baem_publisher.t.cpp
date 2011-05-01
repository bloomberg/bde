// baem_publisher.t.cpp                                               -*-C++-*-
#include <baem_publisher.h>

#include <baem_collector.h>         // for testing only
#include <baem_metricsample.h>

#include <bdetu_systemtime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_protocoltest.h>      // for testing only

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct PublisherTest : bsls_ProtocolTest<baem_Publisher> {
    void publish(const baem_MetricSample&)  { exit(); }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1 -- Implementing the 'baem_Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates a simple implementation of the
// 'baem_Publisher' protocol.  This implementation publishes the metric
// records to an output stream provided on construction.
//..
    // simplestreampublisher.h
    class SimpleStreamPublisher : public baem_Publisher {
        // A simple implementation of the 'baem_Publisher' protocol that
        // writes metric records to a stream.

        // DATA
        bsl::ostream& d_stream; // output stream (held, not owned)

        // NOT IMPLEMENTED
        SimpleStreamPublisher(const SimpleStreamPublisher& );
        SimpleStreamPublisher& operator=(const SimpleStreamPublisher& );

    public:
        // CREATORS
        SimpleStreamPublisher(bsl::ostream& stream);
            // Create this publisher that will publish metrics to the specified
            // 'stream'.

        virtual ~SimpleStreamPublisher();
             // Destroy this publisher.

        // MANIPULATORS
        virtual void publish(const baem_MetricSample& metricValues);
            // Publish the specified 'metricValues'.  This implementation will
            // write the 'metricValues' to the output stream specified on
            // construction.
    };

    // simplestreampublisher.cpp

    // CREATORS
    SimpleStreamPublisher::SimpleStreamPublisher(bsl::ostream& stream)
    : d_stream(stream)
    {
    }

    SimpleStreamPublisher::~SimpleStreamPublisher()
    {
    }

    // MANIPULATORS
    void SimpleStreamPublisher::publish(const baem_MetricSample& metricValues)
    {
        if (0 >= metricValues.numRecords()) {
            return;                                                   // RETURN
        }
        d_stream << metricValues.timeStamp() << " "
                 << metricValues.numRecords() << " Records" << bsl::endl;

        baem_MetricSample::const_iterator sIt = metricValues.begin();
        for (; sIt != metricValues.end(); ++sIt) {
            d_stream << "\tElapsed Time: "
                     << sIt->elapsedTime().totalSecondsAsDouble()
                     << "s" << bsl::endl;
            baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
            for (; gIt != sIt->end(); ++gIt) {
                d_stream << "\t" << gIt->metricId()
                         << " [count = " << gIt->count()
                         << ", total = " << gIt->total()
                         << ", min = "   << gIt->min()
                         << ", max = "   << gIt->max() << "]" << bsl::endl;
            }
        }
    }
//..
///Example 2 -- Using the 'baem_Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following example defines a trivial 'EventManager' class that uses the
// 'baem_Publisher' protocol to publish metrics related to the incoming
// event.  Note that this event manager does no actual processing and is
// intended only to illustrate how the publisher protocol might be used.
//..
    class EventManager {
        // This class provides a dummy event handling mechanism that publishes
        // a metric for the size of the processed event messages.

        // DATA
        baem_Collector  d_eventMessageSize;  // metric for the message size
        bdet_DatetimeTz d_lastPublish;       // time of the last publication

        // NOT IMPLEMENTED
        EventManager(const EventManager& );
        EventManager& operator=(const EventManager& );

      public:
        // CREATORS
        EventManager(const baem_MetricId& messageSizeId)
            // Create this event manager using the specified 'messageSizeId'
            // to identify the event message size metric.
        : d_eventMessageSize(messageSizeId)
        , d_lastPublish(bdetu_SystemTime::nowAsDatetimeGMT(), 0)
        {}

        // MANIPULATORS
        int handleEvent(int eventId, const bsl::string& eventMessage)
            // Process the event described by the specified 'eventId' and
            // 'eventMessage'.  Return 0 on success, and a non-zero value if
            // there was an error processing the event.
        {
            // Update the metrics with the size of the 'eventMessage'.
            d_eventMessageSize.update(eventMessage.size());

            // ...   process the event

            return 0;
        }
//..
// We use a 'baem_Publisher' to publish the metrics recorded by this event
// manager.  Note that most of the functionality illustrated here is
// normally provided by the 'baem_MetricsManager'.
//..
        void publishMetrics(baem_Publisher *publisher)
        {
            bdet_DatetimeTz now(bdetu_SystemTime::nowAsDatetimeGMT(), 0);
            bdet_DatetimeInterval dateInterval = now.gmtDatetime() -
                                                 d_lastPublish.gmtDatetime();
            bdet_TimeInterval interval(dateInterval.totalSeconds(),
                                       dateInterval.milliseconds());

            baem_MetricRecord record;
            d_eventMessageSize.loadAndReset(&record);

            baem_MetricSample sample;
            sample.setTimeStamp(now);
            sample.appendGroup(&record, 1, interval);

            // This is where we make use of the publisher argument to this
            // function.
            publisher->publish(sample);

            d_lastPublish = now;
        }
    };
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma_TestAllocator testAllocator, defaultAllocator;
    bslma_Allocator *Z = &testAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);
    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Example 3 -- Publishing Collected Metrics Using 'EventManager'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we publish metrics collected using a
// 'EventManager' object (defined above).
//
// We start by creating a 'baem_MetricId' object by hand, but in
// practice, an id should be obtained from a 'baem_MetricRegistry' object
// (such as the one owned by a 'baem_MetricsManager').
//..
    baem_Category           myCategory("MyCategory");
    baem_MetricDescription  description(&myCategory, "EventMessageSize");
    baem_MetricId           eventMessageSizeId(&description);
//..
// Now we create a 'EventManager' object and supply it the metric id we have
// created.
//..
    EventManager eventManager(eventMessageSizeId);
//..
// We use the 'EventManager' object to process two events and then publish the
// metrics for those events with a 'SimpleStreamPublisher' object (also defined
// above).
//..
    eventManager.handleEvent(0, "123");
    eventManager.handleEvent(0, "456789");

    SimpleStreamPublisher myPublisher(bsl::cout);
    baem_Publisher *publisher = &myPublisher;
    eventManager.publishMetrics(publisher);
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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VIRTUAL METHODS
        //
        // Concerns:
        //   That the virtual methods of the abstract base class
        //   'bdem_Publisher' will call the methods of concrete
        //   implementations.
        //
        // Plan:
        //  Construct a TestPublisher.  Obtain a 'baem_Publisher' reference to
        //  it and call the various methods on the
        //  'baem_Publisher'.
        //
        // Testing:
        //    ~baem_Publisher();
        //    void publish(const baem_MetricSample& metricValue) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Verify: Virtual Methods"
                          << endl << "======================="
                          << endl;

        bsls_ProtocolTestDriver<PublisherTest> t;

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, publish(baem_MetricSample()));

        testStatus = t.failures();
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
