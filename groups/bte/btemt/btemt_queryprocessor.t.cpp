// btemt_queryprocessor.t.cpp -*-C++-*-
#include <btemt_queryprocessor.h>
#include <btemt_query.h>

#include <bslma_testallocator.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_placeholder.h>

#include <bcec_queue.h>
#include <bsls_platform.h>

#include <bdema_managedptr.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsls_assert.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // fork()
#endif

#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

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

static void eventCallback(const btemt_QueryProcessorEvent& interestingEvent) {
    P(interestingEvent);
}

static void processingWithSeqCallback(
    btemt_QueryRequest *queryRequest,
    const bdef_Function<void (*)(btemt_QueryResult*)>& replyFunctor,
    int maxSeqNum,
    int delayBetweenResponsesInSeconds,
    int processorId)
{
    BSLS_ASSERT(queryRequest);
    BSLS_ASSERT(1 <= maxSeqNum);
    BSLS_ASSERT(delayBetweenResponsesInSeconds >= 0);

    bdema_ManagedPtr<bdem_List> stolenQuery =
        queryRequest->query();

    int sum = 0;
    int length = stolenQuery->length();
    for (int i = 0; i < length; ++i) {
        sum += stolenQuery->theInt(i);
    }

    //int localMaxSeq = rand() % maxSeqNum + 1;
    int localMaxSeq = maxSeqNum;
    printf("processorId = %d: maxSeq = %d, queryId = %lld\n",
           processorId, localMaxSeq, queryRequest->queryId());
    for (int i = 0; i < localMaxSeq - 1; ++i) {

        bdema_ManagedPtr<bdem_List> resultList(new bdem_List);
            // the user should specify his own allocator.
        resultList->appendInt(sum);

        btemt_QueryResult queryResult;

        queryResult.setQueryResponse(resultList);
        queryResult.setSequenceNum(i);

        bcemt_ThreadUtil::microSleep(
            0, rand() % (delayBetweenResponsesInSeconds + 1));
        replyFunctor(&queryResult);
    }

    // Final packet
    bdema_ManagedPtr<bdem_List> resultList(new bdem_List);
    resultList->appendInt(0);

    btemt_QueryResult queryResult;

    queryResult.setQueryResponse(resultList);
    queryResult.setSequenceNum(localMaxSeq);

    bcemt_ThreadUtil::microSleep(
        0, rand() % (delayBetweenResponsesInSeconds + 1));
    replyFunctor(&queryResult);
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

    bslma_TestAllocator testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
#ifdef BSLS_PLATFORM__OS_UNIX
      case -2: {
        // --------------------------------------------------------------------
        // This test case launches an array of processors, each of which
        // calculates the summation of integers sent in from query.  In
        // addition, the summation response is sent back multiple times with
        // 0-based sequence numbers to create the effect of multiple responses
        // for the same queryId.  The last response in the sequence sends
        // a 0 in place of the summation, and it has the highest sequence
        // number.  For an example of client dispatcher, see test case -2
        // of btemt_QueryDispatcher
        // --------------------------------------------------------------------
        if (argc < 5) {
            cout << "Usage: " << argv[0] << " " << argv[1]
                 << " maxSeqNum maxDelayBetweenResponsesInSeconds"
                 << " startPortNumber <lifeInSeconds> <numProcessors>" << endl;
            break;
        }
        int maxSeqNum = atoi(argv[2]);
        BSLS_ASSERT(1 <= maxSeqNum);
        int maxDelayBetweenResponsesInSeconds = atoi(argv[3]);
        BSLS_ASSERT(0 <= maxDelayBetweenResponsesInSeconds);
        int portNumber = atoi(argv[4]);
        BSLS_ASSERT(10000 <= portNumber);

        int lifeInSeconds = 600;
        if (argc > 5) {
            lifeInSeconds = atoi(argv[5]);
            BSLS_ASSERT(60 <= lifeInSeconds);
        }

        int numProcessors = 1;
        if (argc > 6) {
            numProcessors = atoi(argv[6]);
            BSLS_ASSERT(1 <= numProcessors);
        }

        btemt_QueryProcessorConfiguration cpc;
        cpc.setIncomingMessageSize(1024);
        cpc.setOutgoingMessageSize(1024);
        cpc.setMaxConnections(10000);
        cpc.setMaxIoThreads(20);
        cpc.setMaxWriteCache(1024*1024);
        cpc.setReadTimeout(100);
        cpc.setMetricsInterval(30.0);
        cpc.setProcessingThreads(3, 20);
        cpc.setIdleTimeout(60);

        P(cpc);
        P_(maxSeqNum); P(maxDelayBetweenResponsesInSeconds);

        using namespace bdef_PlaceHolders;

        for (int i = 0; i < numProcessors; ++i) {
            int processorId = portNumber + i;

            pid_t pid = fork();
            if (pid) { // parent
                waitpid(pid, NULL, WNOHANG);
            }
            else { // child
                btemt_QueryProcessor processor(
                    cpc,
                    bdef_BindUtil::bind(&eventCallback, _1),
                    bdef_BindUtil::bind(&processingWithSeqCallback, _1, _2,
                                        maxSeqNum,
                                        maxDelayBetweenResponsesInSeconds,
                                        processorId),
                    &testAllocator);
                bteso_IPv4Address address;
                address.setIpAddress("127.0.0.1");
                address.setPortNumber(portNumber + i);
                P(address);
                processor.start();
                ASSERT(0 == processor.listen(address, 10));

                bcemt_ThreadUtil::microSleep(0, lifeInSeconds);
                return 0;
            }
        }
        bcemt_ThreadUtil::microSleep(0, lifeInSeconds);
      } break;
#endif
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
