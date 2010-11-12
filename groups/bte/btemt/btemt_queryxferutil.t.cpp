// btemt_queryxferutil.t.cpp                  -*-C++-*-

#include <btemt_queryxferutil.h>
#include <btemt_query.h>
#include <btemt_message.h>

#include <bslma_testallocator.h>
#include <bdema_managedptr.h>
#include <bcema_sharedptr.h>

#include <bcema_pooledbufferchain.h>
#include <bdem_list.h>
#include <bdet_date.h>
#include <bdex_testoutstream.h>
#include <btesc_channel.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>

#include <btesos_tcpconnector.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_placeholder.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_ctime.h>
#include <bsls_assert.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdef_PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
static int verbose, veryVerbose, veryVeryVerbose;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
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
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

namespace {
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
enum { NUM_QUERIES = 3 };

btemt_Query query[NUM_QUERIES];

void queryRequestCb(bcema_SharedPtr<btemt_QueryRequest> queryRequest,
                    int isVerbose)
{
    static int requestCount = 0;
    BSLS_ASSERT(requestCount < NUM_QUERIES);

    LOOP_ASSERT(requestCount,
                queryRequest->query() ==
                query[requestCount].query());
    if (isVerbose) {
        P_(requestCount); P(*queryRequest);
    }
    ++requestCount;
}

enum { NUM_RESPONSES = 3 };
btemt_QueryResult queryResult[NUM_RESPONSES];

void queryResponseCb(bcema_SharedPtr<btemt_QueryResponse> queryResponse,
                     int isVerbose)
{
    static int responseCount = 0;
    BSLS_ASSERT(responseCount < NUM_RESPONSES);

    LOOP_ASSERT(responseCount,
                queryResponse->queryResponse() ==
                queryResult[responseCount].queryResponse());
    if (isVerbose) {
        P_(responseCount); P(*queryResponse);
    }
    ++responseCount;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <typename A1>
struct NullFunc {
    void operator()(A1) {
    }
};

template <typename A1>
void makeNull(bslma_Allocator * a, bdef_Function<void (*)(A1)> * f) {
    *f = bdef_Function<void (*)(A1)>(NullFunc<A1>(), a);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

} // close anonymous namespace
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " __FILE__ " CASE " << test << endl;

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);

    switch (test) { case 0:/* { // Zero is always the leading case.
        bdem_List sampleList;
        sampleList.appendInt(0);
        btemt_QueryResponse sampleResponse;
        sampleResponse.setResult(sampleList);
        bcema_PooledBufferChainFactory factory(100, &testAllocator);
        bcema_PooledBufferChain *chain = factory.allocate(0);
        btemt_DataMsg sampleMsg(chain, &factory, 0);
        btemt_QueryXferUtil::serializeQueryResponse(&sampleMsg,
                                                    sampleResponse);
        P(sampleMsg.data()->length());
    } break;

    case 3: {
        // --------------------------------------------------------------------
        // TESTING 'parseQuery' and 'serializeQuery'
        //
        // Concerns:
        //    o 'parseQuery' and 'serializeQuery' are inverse of each other
        // Plan:
        //    Create a number of long lists.  Serialize them using
        //    'serializeQuery' then parse them using 'parseQuery'.
        //    Verify that the lists are the same.
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING INVERSE PROPERTIES OF "
                    "'parseQuery' "
                 << "AND 'serializeQuery(btemt_DataMsg*, bdem_List, int)'"
                 << endl;

        enum { NUM_LISTS = 10, NUM_ELEMENTS = 1000 };
        enum { CHUNK_SIZE = 4 };

        for (int i = 0; i < NUM_LISTS; ++i) {
            bcema_PooledBufferChainFactory factory(CHUNK_SIZE * (i + 1),
                                                   &testAllocator);
            bdem_List testList;
            for (int j = 0; j < NUM_ELEMENTS; ++j) {
                testList.appendInt(j);
            }

            btemt_Query query(testList, i);
            bcema_PooledBufferChain *chain = factory.allocate(0);
            btemt_DataMsg serializedQuery(chain, &factory, -i);

            btemt_QueryXferUtil::serializeQuery(&serializedQuery, testList, i);
            if (veryVerbose) {
                P_(serializedQuery.data()->length());
                P(serializedQuery.data()->numBuffers());
            }

            if (veryVerbose) {
                Q("Parsing back");
            }
            bsl::vector<btemt_Query> result;
            int numConsumed = -1, numNeeded = -1;
            btemt_QueryXferUtil::parseQuery(&result, &numConsumed, &numNeeded,
                                           serializedQuery);
            if (veryVerbose) {
                P_(numConsumed); P(numNeeded);
                P(result.size());
            }
            LOOP_ASSERT(i, 1 == result.size());
            LOOP_ASSERT(i, testList == result[0].query());
            LOOP_ASSERT(i, numConsumed == serializedQuery.data()->length());
            LOOP_ASSERT(i, 4 == numNeeded); // white-box knowledge
        }

    } break;
                           */
    case 2: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   parseQueryResponse and SerializeQueryResponse
        // Concerns:
        //   Same as in case 1, but this is for QueryResponse
        // Plan:
        //   Same as in case 1, but this is for QueryResponse
        // --------------------------------------------------------------------
        if (verbose)
            cout << "BREATHING TEST FOR "
                 << "parseQueryResponse & SerializeQueryResponse" << endl;

        if (verbose) {
            cout << "Creating " << NUM_RESPONSES << " query responses" << endl;
        }

        bdema_ManagedPtr<bdem_List> mpList[NUM_RESPONSES];
            // default construction doesn't use any custom allocators

        for (int i = 0; i < NUM_RESPONSES; ++i) {
            mpList[i].load(new bdem_List); // not using testAllocator
        }

        mpList[0]->appendChar('A'); mpList[0]->appendInt(12345);
        mpList[0]->appendDatetime(bdet_Date(2004, 8, 31));

        mpList[1]->appendChar('B'); mpList[1]->appendFloat(3.14159);

        mpList[2]->appendChar('C'); mpList[2]->appendString("Hello world");
        mpList[2]->appendString("Olympics");

        for (int i = 0; i < NUM_RESPONSES; ++i) {
            queryResult[i].setQueryResponse(mpList[i]);
            queryResult[i].setSequenceNum(i);
        }

        if (verbose) {
            cout << "Created queryResults:" << endl;
            for (int i = 0; i < NUM_RESPONSES; ++i) {
                P(queryResult[i]);
            }
        }

        enum { CHUNK_SIZE = 64, LENGTH = 0, CHANNELID = 1 };
        bcema_PooledBufferChainFactory factory(CHUNK_SIZE,
                                               &testAllocator);

        bcema_PooledBufferChain *chain[NUM_RESPONSES];
        btemt_DataMsg serializedQueryResponse[NUM_RESPONSES];
        for (int i = 0; i < NUM_RESPONSES; ++i) {
            chain[i] = factory.allocate(LENGTH);
            serializedQueryResponse[i].setData(chain[i], &factory);

            btemt_QueryXferUtil::
                serializeQueryResult(&serializedQueryResponse[i],
                                     queryResult[i],
                                     i * 100 + i * 10 + i,
                                     btemt_QueryResponse::BTEMT_SUCCESS);
            if (veryVerbose) {
                P_(i);
                P_(chain[i]->bufferSize()); P_(chain[i]->numBuffers());
                P(chain[i]->length());
            }
        }

        // Concatenate above btemt_DataMsg's into one
        bcema_PooledBufferChain *bigChain = factory.allocate(LENGTH);

        for (int i = 0; i < NUM_RESPONSES; ++i) {
            if (verbose) {
                P_(i); P_(bigChain->length()); P(bigChain->numBuffers());
            }
            bigChain->replace(bigChain->length(), *chain[i],
                              0, chain[i]->length());
        }

        btemt_DataMsg serializedQueryResponses(bigChain, &factory, CHANNELID);

        if (verbose) {
            cout << "Concatenated these responses into one" << endl;
            P_(bigChain->length()); P(bigChain->numBuffers());
        }

        int numConsumed, numNeeded;

        if (verbose) {
            cout << "Parsing consolidated responses" << endl;
        }

        bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>
            resultFunctor(
                    bdef_BindUtil::bindA( &testAllocator
                                        , &queryResponseCb
                                        , _1
                                        , verbose));
        int numPackets =
            btemt_QueryXferUtil::parseQueryResponse(resultFunctor,
                                                    &numConsumed,
                                                    &numNeeded,
                                                    serializedQueryResponses);
        if (verbose) {
            P_(numConsumed); P(numNeeded);
        }
        ASSERT(numConsumed == bigChain->length());
        ASSERT(numNeeded == 4);

        if (verbose) {
            cout << "Testing with different sent lengths" << endl;
        }

        // Experiment with different sent lengths for
        // above concatenated serializedQueryResponses.  Simulation method
        // is the same as corresponding test in case 1
        static const struct {
            int           d_lineNum;      // source line number
            int           d_sentLength;   // number of bytes actually sent
            int           d_expectedConsumed;
            int           d_expectedNeeded;
            int           d_parsedPackets;
        } DATA[] = {
            //line len  consumed needed  parsed
            //---- ---  -------- ------  ------
            { L_,  3,   0,       1,      0},
            { L_,  5,   0,       36,     0},
            { L_,  40,  0,       1,      0},
            { L_,  41,  41,      4,      1},
            { L_,  43,  41,      2,      1},
            { L_,  45,  41,      29,     1},
            { L_,  59,  41,      15,     1},
            { L_,  69,  41,      5,      1},
            { L_,  77,  74,      1,      2},
            { L_,  79,  74,      46,     2},
            { L_,  83,  74,      42,     2},
            { L_,  124, 74,      1,      2},
            { L_,  125, 125,     4,      3}
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;
            btemt_DataMsg localQueryResponses(serializedQueryResponses);
            bcema_PooledBufferChain *localChain = localQueryResponses.data();
            localChain->setLength(DATA[i].d_sentLength);

            bdef_Function<void (*)(
                const bcema_SharedPtr<btemt_QueryResponse>&)> resultFunctor;
            makeNull(&testAllocator, &resultFunctor);

            int parsedPackets, numConsumed, numNeeded;
            parsedPackets =
                btemt_QueryXferUtil::parseQueryResponse(resultFunctor,
                                                        &numConsumed,
                                                        &numNeeded,
                                                        localQueryResponses);
            LOOP_ASSERT(LINE, numConsumed == DATA[i].d_expectedConsumed);
            LOOP_ASSERT(LINE, numNeeded == DATA[i].d_expectedNeeded);
            LOOP_ASSERT(LINE, parsedPackets == DATA[i].d_parsedPackets);
        }
      } break;

    case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   parseQuery and SerializeQuery
        // Concerns:
        //   1. parseQuery and SerializeQuery are inverse of each other
        //   2. parseQuery returns correct combination of numConsumedBytes and
        //      numNeededBytes given different lengths of btemt_DataMsg
        // Plan:
        //   Generate three queries and call serializeQuery() for each
        //   of them into a btemt_DataMsg.  Concatenate the three
        //   btemt_DataMsgs and call parseQuery() on the concatenated
        //   btemt_DataMsg.  Modify the concatenated btemt_DataMsg length
        //   (setLength()) to simulate potential receipt of incomplete
        //   messages in channel.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST FOR parseQuery & SerializeQuery" << endl;
        }

        if (verbose) {
            cout << "Creating " << NUM_QUERIES << " queries" << endl;
        }

        bdema_ManagedPtr<bdem_List> mpList[NUM_QUERIES];
            // default construction doesn't use any custom allocators

        for (int i = 0; i < NUM_QUERIES; ++i) {
            mpList[i].load(new bdem_List); // not using testAllocator
        }

        mpList[0]->appendChar('A'); mpList[0]->appendInt(12345);
        mpList[0]->appendDatetime(bdet_Date(2004, 8, 31));

        mpList[1]->appendChar('B'); mpList[1]->appendFloat(3.14159);

        mpList[2]->appendChar('C'); mpList[2]->appendString("Hello world");
        mpList[2]->appendString("Olympics");

        for (int i = 0; i < NUM_QUERIES; ++i) {
            query[i].setQuery(mpList[i]);
            query[i].setCategory((void*)10000000);
        }

        if (verbose) {
            cout << "Created queries:" << endl;
            for (int i = 0; i < NUM_QUERIES; ++i) {
                P(query[i]);
            }
        }

        enum { CHUNK_SIZE = 64, LENGTH = 0, CHANNELID = 1 };
        bcema_PooledBufferChainFactory factory(CHUNK_SIZE,
                                               &testAllocator);

        bcema_PooledBufferChain *chain[NUM_QUERIES];
        btemt_DataMsg serializedQuery[NUM_QUERIES];

        for (int i = 0; i < NUM_QUERIES; ++i) {
            chain[i] = factory.allocate(LENGTH);
            serializedQuery[i].setData(chain[i], &factory);

            btemt_QueryXferUtil::serializeQuery(&serializedQuery[i], query[i],
                                                i * 100 + i * 10 + i);

            if (veryVerbose) {
                P_(i);
                P_(chain[i]->bufferSize()); P_(chain[i]->numBuffers());
                P(chain[i]->length());
            }
        }

        // Concatenate above btemt_DataMsg's into one
        bcema_PooledBufferChain *bigChain = factory.allocate(LENGTH);

        for (int i = 0; i < NUM_QUERIES; ++i) {
            if (verbose) {
                P_(i); P_(bigChain->length()); P(bigChain->numBuffers());
            }
            bigChain->replace(bigChain->length(), *chain[i],
                              0, chain[i]->length());
        }

        btemt_DataMsg serializedQueries(bigChain, &factory, CHANNELID);

        if (verbose) {
            cout << "Concatenated these queries into one" << endl;
            P_(bigChain->length()); P(bigChain->numBuffers());
        }
        int numConsumed, numNeeded;

        if (verbose) {
            cout << "Parsing consolidated queries" << endl;
        }

        bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryRequest>&)>
            resultFunctor(
                    bdef_BindUtil::bindA( &testAllocator
                                        , &queryRequestCb
                                        , _1
                                        , verbose));

        int numPackets =
            btemt_QueryXferUtil::parseQueryRequest(resultFunctor,
                                                   &numConsumed,
                                                   &numNeeded,
                                                   serializedQueries);
        if (verbose) {
            P_(numConsumed); P_(numNeeded); P(numPackets);
        }
        ASSERT(numConsumed == bigChain->length());
        ASSERT(numNeeded == 4);

        if (verbose) {
            cout << "Testing with different sent lengths" << endl;
        }

        // Experiment with different sent lengths for above concatenated
        // serializedQueries.  The data here depends on the three queries and
        // concatenated query prepared above.  If they're changed, the data
        // must be updated accordingly The concatenated DataMsg looks like
        // this:
        //
        // sections:   head   query1  head  query2 head    query3
        // # of bytes: |.4.|....29....|.4.|...21...|.4.|.....39.....|
        //
        // By truncating the length of concatenated DataMsg, we simulate
        // delivery of an incomplete DataMsg containing a partial query
        // containing zero or more complete query.  parseQuery() should give
        // correct numConsumedBytes, numNeeded and use return value to
        // indicate correct parsedPackets, as specified in the API.
        static const struct {
            int           d_lineNum;      // source line number
            int           d_sentLength;   // number of bytes actually sent
            int           d_expectedConsumed;
            int           d_expectedNeeded;
            int           d_parsedPackets;
        } DATA[] = {
            //line len  consumed needed  parsed
            //---- ---  -------- ------  ------
            { L_,  3,   0,       1,      0},
            { L_,  5,   0,       28,     0},
            { L_,  33,  33,      4,      1},
            { L_,  35,  33,      2,      1},
            { L_,  39,  33,      19,     1},
            { L_,  58,  58,      4,      2},
            { L_,  59,  58,      3,      2},
            { L_,  72,  58,      29,     2},
            { L_,  100,  58,     1,      2},
            { L_,  101,  101,    4,      3},
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;
            btemt_DataMsg localQueries(serializedQueries);
            bcema_PooledBufferChain *localChain = localQueries.data();
            localChain->setLength(DATA[i].d_sentLength);

            bdef_Function<void (*)(
                    const bcema_SharedPtr<btemt_QueryRequest>&)> resultFunctor;
            makeNull(&testAllocator, &resultFunctor);

            vector<btemt_Query> result;
            int parsedPackets, numConsumed, numNeeded;
            parsedPackets =
                btemt_QueryXferUtil::parseQueryRequest(
                    resultFunctor,
                    &numConsumed, &numNeeded,
                    localQueries);
            LOOP_ASSERT(LINE, numConsumed == DATA[i].d_expectedConsumed);
            LOOP_ASSERT(LINE, numNeeded == DATA[i].d_expectedNeeded);
            LOOP_ASSERT(LINE, parsedPackets == DATA[i].d_parsedPackets);
        }

      } break;
/*
      case -1: {
        // --------------------------------------------------------
        // Connecting to a server <ip, port> and send a few queries
        // of integer arrays
        // --------------------------------------------------------
        if (argc < 4) {
            cout << "Usage: " << argv[0] << " -1 <hostname> <port>" << endl;
            break;
        }
        char *hostname = argv[2];
        int port = atol(argv[3]);
        if (port <= 0) {
            cout << "Invalid port: " << argv[3]
                 << ".  Port must be positive integer" << endl;
            break;
        }
        bteso_IPv4Address serverAddress;
        if (bteso_ResolveUtil::getAddress(&serverAddress, hostname)) {
            cout << "Invalid hostname: " << argv[2] << endl;
            break;
        }
        serverAddress.setPortNumber(port);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
        btesos_TcpConnector connector(&socketFactory);
        connector.setPeer(serverAddress);
        if (connector.isInvalid()) {
            cout << "Invalid serverAddress = " << serverAddress << endl;
            break;
        }

        int status;
        cout << "Connecting to " << serverAddress << endl;
        btesc_Channel *channel = connector.allocate(&status);
        if (!channel) {
            cout << "Failed to connect to server " << serverAddress << endl;
            connector.invalidate();
            break;
        }

        enum { MAX_INT = 100, MAX_VECTOR_SIZE = 1000, NUM_MSGS = 1 };
        enum { CHUNK_SIZE = 8192, LENGTH = 0, CHANNELID = 1 };

        // Generate NUM_MSGS btemt_Query's.

        // time_t timer; srand(time(&timer)); // commented out for pseudo-rand
        bcema_PooledBufferChainFactory factory(CHUNK_SIZE, &testAllocator);
        int sum[NUM_MSGS] = {0};

        for (int queryId = 0; queryId < NUM_MSGS; ++queryId) {
            bdem_List list;
            int vector_size = rand() % MAX_VECTOR_SIZE + 1;
            for (int j = 0; j < vector_size; ++j) {
                int number = rand() % MAX_INT + 1;
                list.appendInt(number);
                sum[queryId] += number;
            }

            btemt_Query query(list, queryId);
            bcema_PooledBufferChain *chain = factory.allocate(LENGTH);
            btemt_DataMsg serializedQuery(chain, &factory, CHANNELID);
            btemt_QueryXferUtil::serializeQuery(&serializedQuery, query);

            int totalSent = 0;

            P_(chain->length());
            int numBuffers = (chain->length() - 1) / CHUNK_SIZE + 1;
            P(numBuffers);

            int numSent = 1;
            for (int i = 0; i < numBuffers && numSent > 0; ++i) {
                int numBytes =
                    i == numBuffers - 1 ?
                    chain->length() % CHUNK_SIZE : CHUNK_SIZE;
                P(numBytes);
                numSent = channel->write(chain->buffer(i), numBytes);
                P(numSent);
                totalSent += numSent;
            }
        }

        const char *buffer;

        // get a sample query response of only one integer in its bdem_List
        bdem_List sampleList;
        sampleList.appendInt(0);
        btemt_QueryResponse sampleResponse;
        sampleResponse.setResult(sampleList);
        bcema_PooledBufferChain *sampleChain = factory.allocate(0);
        btemt_DataMsg sampleMsg(sampleChain, &factory, 0);
        btemt_QueryXferUtil::serializeQueryResponse(&sampleMsg,
                                                    sampleResponse);

        int numReceived =
            channel->bufferedRead(&buffer,
                                  NUM_MSGS * sampleMsg.data()->length());

        P(numReceived);
        ASSERT(numReceived > 0);
        if (numReceived <= 0) {
            cout << "Unable to get a response from the server: "
                 << numReceived << endl;
            return -1;
        }
        bcema_PooledBufferChain *chain = factory.allocate(LENGTH);
        chain->replace(0, buffer, numReceived);
        P_(chain->length()); P_(chain->numBuffers());
        ASSERT(chain->length() == numReceived);

        btemt_DataMsg rawResponses(chain, &factory, CHANNELID);
        vector<btemt_QueryResponse> responses;
        int numConsumed, numNeeded, numPackets;
        numPackets =
            btemt_QueryXferUtil::parseQueryResponse(&responses,
                                                    &numConsumed, &numNeeded,
                                                    rawResponses);
        P_(numConsumed); P_(numNeeded); P(numPackets);
        ASSERT(NUM_MSGS == numPackets);
        ASSERT(numConsumed == chain->length());
        ASSERT(4 == numNeeded);

        for (int i = 0; i < NUM_MSGS; ++i) {
            P_(i); P_(responses[i].queryId());
            P_(responses[i].sequenceNumber()); P(responses[i].status());

            ASSERT(responses[i].result().length() == 1);
            ASSERT(sum[responses[i].queryId()] ==
                   responses[i].result().theInt(0));
            P(responses[i].result().theInt(0));
        }
        connector.deallocate(channel);
        connector.invalidate();

    } break;
*/
      default: {
          cerr << "WARNING: CASE " << test << " NOT FOUND.\n" << endl;
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << endl;
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
