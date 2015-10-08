// balm_publisher.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_publisher.h>

#include <balm_collector.h>         // for testing only
#include <balm_metricsample.h>

#include <bdlt_currenttime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_protocoltest.h>      // for testing only

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct PublisherTest : bsls::ProtocolTestImp<balm::Publisher> {
    void publish(const balm::MetricSample&)  { markDone(); }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1 -- Implementing the 'balm::Publisher' Protocol
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates a simple implementation of the
// 'balm::Publisher' protocol.  This implementation publishes the metric
// records to an output stream provided on construction.
//..
    // simplestreampublisher.h
    class SimpleStreamPublisher : public balm::Publisher {
        // A simple implementation of the 'balm::Publisher' protocol that
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
        virtual void publish(const balm::MetricSample& metricValues);
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
    void SimpleStreamPublisher::publish(const balm::MetricSample& metricValues)
    {
        if (0 >= metricValues.numRecords()) {
            return;                                                   // RETURN
        }
        d_stream << metricValues.timeStamp() << " "
                 << metricValues.numRecords() << " Records" << bsl::endl;

        balm::MetricSample::const_iterator sIt = metricValues.begin();
        for (; sIt != metricValues.end(); ++sIt) {
            d_stream << "\tElapsed Time: "
                     << sIt->elapsedTime().totalSecondsAsDouble()
                     << "s" << bsl::endl;
            balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
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
///Example 2 -- Using the 'balm::Publisher' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following example defines a trivial 'EventManager' class that uses the
// 'balm::Publisher' protocol to publish metrics related to the incoming
// event.  Note that this event manager does no actual processing and is
// intended only to illustrate how the publisher protocol might be used.
//..
    class EventManager {
        // This class provides a dummy event handling mechanism that publishes
        // a metric for the size of the processed event messages.

        // DATA
        balm::Collector  d_eventMessageSize;  // metric for the message size
        bdlt::DatetimeTz d_lastPublish;       // time of the last publication

        // NOT IMPLEMENTED
        EventManager(const EventManager& );
        EventManager& operator=(const EventManager& );

      public:
        // CREATORS
        EventManager(const balm::MetricId& messageSizeId)
            // Create this event manager using the specified 'messageSizeId'
            // to identify the event message size metric.
        : d_eventMessageSize(messageSizeId)
        , d_lastPublish(bdlt::CurrentTime::utc(), 0)
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
// We use a 'balm::Publisher' to publish the metrics recorded by this event
// manager.  Note that most of the functionality illustrated here is
// normally provided by the 'balm::MetricsManager'.
//..
        void publishMetrics(balm::Publisher *publisher)
        {
            bdlt::DatetimeTz now(bdlt::CurrentTime::utc(), 0);
            bdlt::DatetimeInterval dateInterval = now.utcDatetime() -
                                                 d_lastPublish.utcDatetime();
            bsls::TimeInterval interval(dateInterval.totalSeconds(),
                                       dateInterval.milliseconds());

            balm::MetricRecord record;
            d_eventMessageSize.loadAndReset(&record);

            balm::MetricSample sample;
            sample.setTimeStamp(now);
            sample.appendGroup(&record, 1, interval);

            // This is where we make use of the publisher argument to this
            // function.
            publisher->publish(sample);

            d_lastPublish = now;
        }
    };
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator testAllocator, defaultAllocator;
    bslma::Allocator *Z = &testAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
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
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we publish metrics collected using a
// 'EventManager' object (defined above).
//
// We start by creating a 'balm::MetricId' object by hand, but in
// practice, an id should be obtained from a 'balm::MetricRegistry' object
// (such as the one owned by a 'balm::MetricsManager').
//..
    balm::Category           myCategory("MyCategory");
    balm::MetricDescription  description(&myCategory, "EventMessageSize");
    balm::MetricId           eventMessageSizeId(&description);
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
    balm::Publisher *publisher = &myPublisher;
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
        //   'balm::Publisher' will call the methods of concrete
        //   implementations.
        //
        // Plan:
        //  Construct a TestPublisher.  Obtain a 'balm::Publisher' reference to
        //  it and call the various methods on the
        //  'balm::Publisher'.
        //
        // Testing:
        //    ~balm::Publisher();
        //    void publish(const balm::MetricSample& metricValue) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Verify: Virtual Methods"
                          << endl << "======================="
                          << endl;

        bsls::ProtocolTest<PublisherTest> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, publish(balm::MetricSample()));
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
