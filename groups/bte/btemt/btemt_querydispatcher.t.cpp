// btemt_querydispatcher.t.cpp                  -*-C++-*-

#include <btemt_querydispatcher.h>
#include <btemt_querydispatcherconfiguration.h>
#include <btemt_channelpoolconfiguration.h>

#include <bcema_testallocator.h>

#include <bteso_ipv4address.h>

#include <bdef_bind.h>
#include <bdef_placeholder.h>

#include <bsls_platformutil.h>

#include <bcemt_thread.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//    [TBD - Overview of the test]
//-----------------------------------------------------------------------------
// [XX] [TBD - Method name]
//=============================================================================

//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Pi(X) printf("%s = %d\n", #X, (X)); // Print int identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define Pi_(X) printf("%s = %d\t", #X, (X)); // Print int identifier and value.
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bsls_PlatformUtil::Int64 Int64;
static void dispatcherEventCb(const btemt_QueryDispatcherEvent& event)
{
    Q(dispatcherEventCb());
    P(event);
}

/*
static void userResponseCb1(int *isFinal, const btemt_QueryResponse& response)
{
    Q(userResponseCb1);
    if (response.status() == btemt_QueryResponse::BTEMT_TIMEOUT) {
        printf("Timeout for queryId = %lld\n", response.queryId());
        *isFinal = 0;
    }
    else {
        P(response);
        *isFinal = 1;
    }
}
*/

static void userResponseCb2(int *isFinal, btemt_QueryResponse *response)
{
    if (response->status() == btemt_QueryResponse::BTEMT_TIMEOUT) {
        printf("userResponseCb2 reads Timeout for queryId = %lld\n",
               response->queryId());
        *isFinal = 0;
    }
    else {
        bdema_ManagedPtr<bdem_List> stolenQueryResponse =
            response->queryResponse();

        int value = stolenQueryResponse->theInt(0);
        *isFinal = !value;
        if (*isFinal) {
            printf(
                "userResponseCb2 reads Final ""for queryId = %lld, seq = %d\n",
                response->queryId(), response->sequenceNum());
        }
        else {
            printf("userResponseCb2 queryId = %lld, seq = %d, value = %d\n",
                   response->queryId(), response->sequenceNum(), value);
        }
    }
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
class Bar {
    bdema_ManagedPtr<int> d_bar;
  public:
    Bar() { }
    Bar(Bar& rhs)
        : d_bar(rhs.d_bar) { }
    const int& bar() const
        { return *d_bar; }
    void setBar(bdema_ManagedPtr<int>& bar)
        { d_bar = bar; }
};

bsl::ostream& operator<<(bsl::ostream& os, const Bar& rhs)
{
    os << "{ d_bar = " << rhs.bar() << '}';
    return os;
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bcema_TestAllocator testAllocator(false);

/*
    bael_DefaultObserver observer(bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager& manager =
            bael_LoggerManager::initSingleton(&observer, configuration);
    manager.setDefaultThresholdLevels(
        bael_Severity::INFO,
        bael_Severity::INFO,
        bael_Severity::WARN,
        bael_Severity::WARN);
*/

    switch (test) { case 0:  // Zero is always the leading case.
      case -3: {
        bdema_ManagedPtr<int> intBar(new(testAllocator) int, &testAllocator);
        *intBar = 123;

        Bar bar;
        bar.setBar(intBar);

        bsl::cout << "bar = " << bar << bsl::endl;

        Bar foo(bar);

        bsl::cout << "foo = " << foo << bsl::endl;
        bsl::cout << "bar = " << bar << bsl::endl;

    } break;
      case -2: {
        // --------------------------------------------------------------------
        // This test case creates a query dispatcher that  connects to the
        // specified number of processors, which are located on the local
        // machine.  The array of processors occupy consecutive port numbers,
        // starting from the specified startPortNumber.  To launch such an
        // array of processor on the local machine, please find test case -2
        // of btemt_QueryProcessor.  A user-supplied v2 functor determines the
        // end of response sequence for a queryId by checking if a received
        // response contains integer 0.
        // --------------------------------------------------------------------
        if (argc < 5) {
            cout << "Usage: " << argv[0] << " " << argv[1]
                 << " maxDelayBetweenResponsesInSeconds numQueries"
                 << " startPortNumber"
                 << "<numThreadPoolThreads> <numChannelThreads>"
                 << " <numProcessors>" << endl;
            break;
        }

        int maxDelayBetweenResponsesInSeconds = atoi(argv[2]);
        BSLS_ASSERT(0 <= maxDelayBetweenResponsesInSeconds);

        const int MAX_NUM_QUERIES = 200000;
        int numQueries = atoi(argv[3]);
        BSLS_ASSERT(1 <= numQueries && numQueries <= MAX_NUM_QUERIES);

        int portNumber = atoi(argv[4]);
        BSLS_ASSERT(10000 <= portNumber);

        enum { MIN_THREADPOOL_THREADS = 3,
               MAX_THREADPOOL_IDLETIME = 2000  // milliseconds
        };

        int numThreadPoolThreads = 6;
        if (argc > 5) {
            numThreadPoolThreads = atoi(argv[5]);
            BSLS_ASSERT(MIN_THREADPOOL_THREADS <= numThreadPoolThreads);
        }

        int numChannelThreads = 10;
        if (argc > 6) {
            numChannelThreads = atoi(argv[6]);
            BSLS_ASSERT(1 <= numChannelThreads);
        }

        int numProcessors = 1;
        if (argc > 7) {
            numProcessors = atoi(argv[7]);
            BSLS_ASSERT(1 <= numProcessors);
        }

        btemt_QueryDispatcherConfiguration qdConfig;

        qdConfig.setMaxConnections(100);
        qdConfig.setMaxIoThreads(numChannelThreads);
        qdConfig.setMaxWriteCache(1024*1024);
        qdConfig.setProcessingThreads(MIN_THREADPOOL_THREADS,
                                      numThreadPoolThreads);
        qdConfig.setIdleTimeout(MAX_THREADPOOL_IDLETIME);

        using namespace bdef_PlaceHolders;
        bcemt_Attribute attrib; // default thread attributes
        btemt_QueryDispatcher dispatcher(
            qdConfig, attrib,
            bdef_BindUtil::bind(&dispatcherEventCb, _1),
            &testAllocator);

        enum { CONNECT_TIMEOUT = 5 };    // seconds

        dispatcher.start();

        for (int i = 0; i < numProcessors; ++i) {
            bteso_IPv4Address procAddress;
            procAddress.setIpAddress ("127.0.0.1");
            procAddress.setPortNumber(portNumber + i);
            LOOP_ASSERT(i, 0 == dispatcher.connect(
                            procAddress,
                            bdet_TimeInterval(CONNECT_TIMEOUT)));
            bcemt_ThreadUtil::microSleep(100000, 0);
                // give sometime for connection
        }

        vector<Int64> queryIds;

        enum { PRINT_PERIOD = 5000,   // print status every 5000 queries sent
               CANCEL_TRIES = 10,     // try to cancel queries for 10 times
               RECENT_QUERIES = 100,  // Try to cancel within recent 100
                                      // queries.
               NUM_CATEGORIES = 10
        };

        if (veryVerbose) {
            bsl::cerr << "Preparing all queries" << bsl::endl;
        }
        bsl::vector<btemt_Query> query(MAX_NUM_QUERIES);

        for (int i = 0; i < numQueries; ++i) {
            bdema_ManagedPtr<bdem_List>
                queryList(new(testAllocator) bdem_List(&testAllocator),
                          &testAllocator);

            for (int num = 0; num < 50; ++num) {
                queryList->appendInt(num);
            }

            query[i].setQuery(queryList);
//            query[i].setCategory((void *)(i % NUM_CATEGORIES + 1));
            query[i].setCategory(0);
        }
        if (veryVerbose) {
            bsl::cerr << "Starting all queries" << bsl::endl;
        }

        for (int i = 0; i < numQueries; ++i) {
            Int64 queryId =
                dispatcher.timedQuery(
                    &query[i],
                    bdef_BindUtil::bind(&userResponseCb2, _1, _2),
                    bdet_TimeInterval(maxDelayBetweenResponsesInSeconds));
            if (queryId) {
                queryIds.push_back(queryId);
            }
            if (RECENT_QUERIES == i % PRINT_PERIOD) {
                // Once every PRINT_PERIOD queryId's,
                // try to cancel CANCEL_TRIES queryId's that were
                // recently (within past RECENT_QUERIES) sent to dispatcher
                for (int c = 0; c < CANCEL_TRIES; ++c) {
                    int index = rand() % RECENT_QUERIES + i - RECENT_QUERIES;
                    dispatcher.cancel(queryIds[index]);
                }
                // try to cancel all queries associated with one random
                // category
                dispatcher.cancel((void *)(rand() % NUM_CATEGORIES + 1));

                dispatcher.printStatus(cerr);
            }
        }
        while (1) {
            bcemt_ThreadUtil::microSleep(0, 5);
            if (dispatcher.successCount() +
                dispatcher.timeoutCount() +
                dispatcher.cancelCount() >= numQueries) {
                break;
            }
        }
        dispatcher.printStatus(cerr);

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
