// btlsos_tcptimedcbconnector.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcptimedcbconnector.h>

#include <btlsos_tcpcbchannel.h>
#include <btlsos_tcptimedcbchannel.h>

#include <btlso_inetstreamsocketfactory.h>  // HACK
#include <btlso_ipv4address.h>
#include <btlso_platform.h>
#include <btlso_resolveutil.h>
#include <btlso_socketimputil.h>            // cleanup, start-up
#include <btlso_tcptimereventmanager.h>

#include <btlsc_timedcbchannel.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>            // thread-safe allocator
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>
#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>

#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>

#include <bsl_cstdio.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'strcmp'
#include <bsl_functional.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdlf::PlaceHolders;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//-----------------------------------------------------------------------------
bslmt::Mutex  d_mutex;   // for i/o synchronization in all threads

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();
// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
const char* hostName = "127.0.0.1";
typedef btlsos::TcpTimedCbConnector  Obj;

static int globalVerbose;
static int globalVeryVerbose;
static int globalVeryVeryVerbose;

enum {
    k_DEFAULT_PORT_NUMBER     = 0,
    k_DEFAULT_NUM_CONNECTIONS = 10,
    k_DEFAULT_EQUEUE_SIZE     = 5,
    k_SLEEP_TIME              = 1000,
    e_VALID                   = 0,
    e_INVALID                 = -1
};

struct ConnectionInfo {
    // This is the information suite to be passed to the server thread.
    btlso::StreamSocketFactory<btlso::IPv4Address>         *d_factory_p;
    btlso::StreamSocket<btlso::IPv4Address>                *d_serverSocket_p;
    int                                                   d_equeueSize;
    bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> *d_connList;
    int                                                   d_numConnections;
};

struct TestCommand {
  // This struct includes needed information for each test operation, e.g., a
  // request to "CONNECT" or a request to "dispatch", along with it's
  // corresponding test results.

    int  d_lineNum;

    char d_commandCode;    // a command to invoke a corresponding function,
                           // e.g., 'D' -- invoke the event manager's
                           // dispatch(); 'R' -- an "ACCEPT" request etc.
    int d_channelType;     // a request for a 'btlsos::TcpCbChannel' or
                           // 'btlsos::TcpTimedCbChannel', i.e., 1 for a timed
                           // channel and 0 for a 'btlsos::TcpCbChannel'
    int d_timedRequestFlag;// a request with/without timeout request

    int d_timeoutFlag;     // a flag to indicate if the request will timeout

    int d_validChannel;    // a flag to indicate if a new channel is created or
                           // not
    int d_expStatus;       // a expected status value from a "CONNECT" request

    int d_expNumEvents;    // the number of events after the execution of this
                           // command
    int d_expNumTimers;    // the number of timers after the execution of this
                           // command
    int d_expNumChannels;

    int d_cancelFlag;      // a flag to indicate if 'cancelAll' will be called
                           // inside the user callback function.

    int d_returnValue;     // the expected function return value after the
                           // command is executed.
};

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                USAGE EXAMPLES HELPER FUNCTIONS AND CLASSES
// ----------------------------------------------------------------------------

///Usage
///-----
///An example echo client
///- - - - - - - - - -  -
// The following usage example shows a possible implementation of an echo
// client.  An echo client sends connection requests to an echo server, and for
// every connection, sends and receives data.  Various configuration parameters
// such as the timeout values, the number of connections, the port number, and
// the number of messages to be sent should be passed into a client object.
//..
    class my_EchoClient {
        enum {
            k_BUFFER_SIZE = 100
        };

        btlsos::TcpTimedCbConnector d_allocator;
        bsls::TimeInterval          d_connectTimeout;
        bsls::TimeInterval          d_readTimeout;
        bsls::TimeInterval          d_writeTimeout;
        char                        d_controlBuffer[k_BUFFER_SIZE];
        int                         d_numConnections;
        int                         d_maxConnections;
        int                         d_numMessages;

        bsl::function<void(btlsc::TimedCbChannel*, int)>
                                    d_allocateFunctor;

        void allocateCb(btlsc::TimedCbChannel *channel, int status);
            // Invoked by the socket event manager when a connection is
            // accepted.

        void bufferedReadCb(const char            *buffer,
                            int                    status,
                            int                    asyncStatus,
                            btlsc::TimedCbChannel *channel,
                            int                    sequence);

        void writeCb(int                    status,
                     int                    asyncStatus,
                     btlsc::TimedCbChannel *channel,
                     int                    sequence);

      private:
        // Not implemented:
        my_EchoClient(const my_EchoClient&);
        my_EchoClient& operator=(const my_EchoClient&);

      public:
        // CREATORS
        my_EchoClient(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           btlso::TimerEventManager                       *manager,
           int                                             maxConnections,
           int                                             numMessages,
           bslma::Allocator                               *basicAllocator = 0);

        ~my_EchoClient();

        //  MANIPULATORS
        int setPeer(const btlso::IPv4Address& address);
            // Set the address of the echo server to the specified 'address',
            // and start sending messages to the server.
    };

    // CREATORS
    my_EchoClient::my_EchoClient(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                int                                             maxConnections,
                int                                             numMessages,
                bslma::Allocator                               *basicAllocator)
    : d_allocator(factory, manager, basicAllocator)
    , d_connectTimeout(120, 0)
    , d_readTimeout(20.0)
    , d_writeTimeout(5,0)
    , d_numConnections(0)
    , d_maxConnections(maxConnections)
    , d_numMessages(numMessages)
    {
        ASSERT(factory);
        ASSERT(manager);
        d_allocateFunctor = bdlf::MemFnUtil::memFn(&my_EchoClient::allocateCb,
                                                   this);

        bsl::memset(d_controlBuffer, 'A', k_BUFFER_SIZE);
        ((int*)d_controlBuffer)[0] = k_BUFFER_SIZE;
        ((int*)d_controlBuffer)[1] = k_BUFFER_SIZE;
    }

    my_EchoClient::~my_EchoClient() {
    }
//..
// All the work of accepting connections and reading/writing the data is done
// in the (private) callback methods of 'my_EchoClient'.  When the connection
// is established and the 'allocateCb' method is invoked, the buffered write
// with timeout is initiated on the channel and another allocation request is
// enqueued.  If any error occurs while allocating, the acceptor is shut down.
// If any error, including timeout, occurs while reading or writing data, the
// channel is shut down.  Note that the allocation functor is cached to improve
// performance:
//..
    // MANIPULATORS
    void my_EchoClient::allocateCb(btlsc::TimedCbChannel *channel,
                                   int                    status) {
        if (channel) {
            // Connected to a server.  Issue a buffered write request.

            bsl::function<void(int, int)> callback(
                                  bdlf::BindUtil::bind(&my_EchoClient::writeCb,
                                                       this,
                                                       _1,
                                                       _2,
                                                       channel,
                                                       0));
            if (channel->timedBufferedWrite(d_controlBuffer,
                                            k_BUFFER_SIZE,
                                            bdlt::CurrentTime::now()
                                                 + d_writeTimeout, callback)) {
                puts("Failed to enqueue write request.");
                ASSERT(channel->isInvalidWrite());
                d_allocator.deallocate(channel);
            }

            if (d_maxConnections > ++d_numConnections) {
                int s = d_allocator.timedAllocateTimed(d_allocateFunctor,
                                                       bdlt::CurrentTime::now()
                                                           + d_connectTimeout);
                ASSERT(0 == s);
            }
            return;                                                   // RETURN
        }

        ASSERT(0 >= status);    // Interrupts are not enabled.
        if (0 == status) {
            cerr << "Timed out connecting to the server." << endl;
        }
        else {
            // Hard-error accepting a connection, invalidate the allocator.
            printf("Non-recoverable error connecting to the server %d "
                   "(Connection %d of of %d)\n",
                   status, d_numConnections, d_maxConnections);
            d_allocator.invalidate();
            return;                                                   // RETURN
        }
        // In any case, except for hard error on allocator, enqueue another
        // connect request
        if (d_maxConnections > ++d_numConnections) {
            int s = d_allocator.timedAllocateTimed(d_allocateFunctor,
                                                   bdlt::CurrentTime::now()
                                                           + d_connectTimeout);
            ASSERT(0 == s);
        }
    }

    void my_EchoClient::bufferedReadCb(const char            *buffer,
                                       int                    status,
                                       int                    asyncStatus,
                                       btlsc::TimedCbChannel *channel,
                                       int                    sequence)
    {
        ASSERT(channel);
        if (0 < status) {
            ASSERT(k_BUFFER_SIZE == status);
            ASSERT(0 == bsl::memcmp(buffer, d_controlBuffer, k_BUFFER_SIZE));

            // If we're not done -- enqueue another request
            if (sequence < d_numMessages) {
                bsl::function<void(int, int)> callback(
                                  bdlf::BindUtil::bind(&my_EchoClient::writeCb,
                                                       this,
                                                       _1,
                                                       _2,
                                                       channel,
                                                       sequence + 1));
                if (channel->timedBufferedWrite(d_controlBuffer,
                                                k_BUFFER_SIZE,
                                                bdlt::CurrentTime::now()
                                                   + d_writeTimeout, callback))
                    {
                        puts("Failed to enqueue write request.");
                        ASSERT(channel->isInvalidWrite());
                        d_allocator.deallocate(channel);
                    }
            }
            else {
                d_allocator.deallocate(channel);
            }
        }
        else if (0 == status) {
            if (0 > asyncStatus) {
            }
            else {
                printf("Timed out: Can't read data from server for more than"
                       " %lld seconds\n", d_readTimeout.seconds());
                d_allocator.deallocate(channel);
            }
        }
        else {
            printf("Failed to read data: non-recoverable error: %d\n",
                   status);
            d_allocator.deallocate(channel);
        }
    }

    void my_EchoClient::writeCb(int                    status,
                                int                    asyncStatus,
                                btlsc::TimedCbChannel *channel,
                                int                    sequence)
    {
        if (0 < status) {
            if (status != k_BUFFER_SIZE) {
                d_allocator.deallocate(channel);
                ASSERT("Failed to send data to the server" && 0);
            }
            else {
                bsl::function<void(const char *, int, int)> callback(
                           bdlf::BindUtil::bind(&my_EchoClient::bufferedReadCb,
                                                this,
                                                _1,
                                                _2,
                                                _3,
                                                channel,
                                                sequence));
                if (channel->timedBufferedRead(k_BUFFER_SIZE,
                                               bdlt::CurrentTime::now()
                                                               + d_readTimeout,
                                               callback)) {
                    ASSERT(channel->isInvalidRead());
                    d_allocator.deallocate(channel);
                    ASSERT("Failed to enqueue read request" && 0);
                }
            }
        }
        else if (0 == status) {
            if (0 > asyncStatus) {
                cout << "Request dequeued." << endl;
            }
            else {
                d_allocator.deallocate(channel);
                ASSERT("Timed out sending data to the server" && 0);
            }
        }
        else {
            d_allocator.deallocate(channel);
            ASSERT("Failed to send data: non-recoverable error on the channel"
                   && 0);
        }
    }

    int my_EchoClient::setPeer(const btlso::IPv4Address& address) {
        d_allocator.setPeer(address);
        return d_allocator.timedAllocateTimed(d_allocateFunctor,
                                              bdlt::CurrentTime::now()
                                                           + d_connectTimeout);
    }
//..
//-----------------------------------------------------------------------------
// Though formally not a part of the usage example, a multi-threaded echo
// client is useful.

struct my_MtEchoClientState {
    int         d_numConnections;  // number of connections for each thread
    int         d_numMessages;     // number of messages to send on each
                                   // connection
    const char *d_hostname;        // hostname of the echo server
    int         d_portNumber;      // port number of the echo server
    btlso::TcpTimerEventManager::Hint
                d_hint;            // infrequent registration hint
};

extern "C"
void *echoClientThread(void *arg) {
    my_MtEchoClientState *config = (my_MtEchoClientState*)arg;
    ASSERT(config);
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
    btlso::TcpTimerEventManager manager(config->d_hint);
    my_EchoClient client(&factory, &manager,
                         config->d_numConnections,
                         config->d_numMessages);

    btlso::IPv4Address serverAddress;

    int ret = btlso::ResolveUtil::getAddress(&serverAddress,
                                            config->d_hostname);
    ASSERT(0 == ret);
    serverAddress.setPortNumber(config->d_portNumber);
    ASSERT(0 == client.setPeer(serverAddress));

    if (globalVerbose) {
        PT(serverAddress);
    }

    while (manager.numEvents()) {
        int s = manager.dispatch(0);
        ASSERT(0 <= s);
    }
    return arg;
}

// ----------------------------------------------------------------------------
//                         USAGE EXAMPLE: Data stream
// ----------------------------------------------------------------------------

///Dual control and data channels
///- - - - - - - - - - - - - -  -
// The following usage example shows a possible implementation of a client-side
// communication layer where two communication channels are established for a
// server: one is for control messages and another for the data stream.  The
// client is implemented as a separate class that owns the connector and uses a
// socket event manager and (corresponding) stream socket factory installed at
// creation.  Various configuration parameters such as the timeout values, the
// queue size, and input buffer size are constants within this class.
//..
    class my_DataStream {
        enum {
            k_DEFAULT_PORT_NUMBER = 1234,
            k_QUEUE_SIZE          =   16
        };

        btlsos::TcpTimedCbConnector  d_allocator;
        bsls::TimeInterval           d_connectTimeout;
        btlsc::TimedCbChannel       *d_controlChannel;
        btlsc::TimedCbChannel       *d_dataChannel;

      private:
        // Callbacks
        void allocateCb(btlsc::TimedCbChannel  *channel,
                        int                     status,
                        btlsc::TimedCbChannel **cachedChannel);
            // Invoked from the socket event manager when a connection is
            // allocated (i.e., established) or an error occurs when
            // allocating.

      private:
        // Not implemented:
        my_DataStream(const my_DataStream&);
        my_DataStream& operator=(const my_DataStream&);

      public:
        // CREATORS
        my_DataStream(
                  btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                  btlso::TimerEventManager                       *manager,
                  const btlso::IPv4Address&                       peerAddress);
            // Create a controlled data stream that uses the specified stream
            // socket 'factory' for system sockets, uses the specified socket
            // event 'manager' to multiplex events on these sockets, and
            // attempts to connect to the server at the specified
            // 'peerAddress'.  The behavior is undefined if either 'factory'
            // or 'manager' is 0.

        ~my_DataStream();
            // Destroy this server.

        // MANIPULATORS
        int setUpCallbacks();
            // Register callbacks as required for establishing communication
            // channels.  Return 0 on success, and a non-zero value otherwise,
            // in which case all further registration attempts will fail (and
            // the object can be only destroyed).
    };
//..
// The implementation of the public methods of 'my_DataStream' is trivial.  For
// the constructor, the socket factory and socket event manager are passed to
// the connector, and the allocate callback and (three) event timeouts are
// initialized; the peer address is passed to the connector.  The destructor
// verifies that the state of the connector is valid:
//..
    my_DataStream::my_DataStream(
                   btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                   btlso::TimerEventManager                       *manager,
                   const btlso::IPv4Address&                       peerAddress)
    : d_allocator(factory, manager)
    , d_connectTimeout(120, 0)
    , d_controlChannel(NULL)
    , d_dataChannel(NULL)
    {
        ASSERT(factory);
        ASSERT(manager);
        d_allocator.setPeer(peerAddress);
    }

    my_DataStream::~my_DataStream() {
        ASSERT(0 == d_allocator.numChannels());
    }

    int my_DataStream::setUpCallbacks() {
        bsl::function<void(btlsc::TimedCbChannel*, int)> callback(
                               bdlf::BindUtil::bind(&my_DataStream::allocateCb,
                                                    this,
                                                    _1,
                                                    _2,
                                                    &d_controlChannel));

        if (d_allocator.timedAllocateTimed(callback,
                                           bdlt::CurrentTime::now()
                                                         + d_connectTimeout)) {
            return -1;                                                // RETURN
        }

        callback = bdlf::BindUtil::bind(&my_DataStream::allocateCb,
                                        this,
                                        _1,
                                        _2,
                                        &d_dataChannel);
        return d_allocator.timedAllocateTimed(callback,
                                              bdlt::CurrentTime::now()
                                                           + d_connectTimeout);
    }
//..
// The allocate callback will cache the newly-allocated channel for future use:
//..
    void my_DataStream::allocateCb(btlsc::TimedCbChannel  *channel,
                                   int                     status,
                                   btlsc::TimedCbChannel **cachedChannel) {
        ASSERT(cachedChannel);

        if (channel) {
            *cachedChannel = channel;
            if (d_controlChannel && d_dataChannel) {
                // Ready to do data processing ...
            }
        }
        else {
            cout << "not valid channel: status: " << status << endl;
        }
    }

    static void connectCb(btlsc::CbChannel            *channel,
                          int                          status,
                          btlsos::TcpTimedCbConnector *connector,
                          int                         *numConnections,
                          int                          validChannel,
                          int                          expStatus,
                          int                          cancelFlag)
        // Verify the result of an "ACCEPT" request by comparing against the
        // expected values: If the specified 'validChannel' is nonzero, a new
        // 'btlsc::CbChannel' should be established; the specified return
        // 'status' should be the same as the specified 'expStatus'.  If the
        // specified 'cancelFlag' is nonzero, invoke the 'cancelAll()' on the
        // specified 'acceptor' for test.
    {
        if (validChannel) {
            ASSERT(channel);
        }
        else {
            ASSERT(0 == channel);
        }
        ASSERT(status == expStatus);

        if (0 == channel) {
        }
        else {
            ++(*numConnections);
        }
        if (cancelFlag) {
            connector->cancelAll();
        }
    }
//..

#if !defined(BSLS_PLATFORM_CMP_SUN) \
    || BSLS_PLATFORM_CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.  This was fixed in Sun Studio
    // 8 compiler.
#endif
void *threadToAcceptConnection(void *arg)
    // Create a server socket which is ready for accept connections.  The
    // following information will be pass in through the specified 'arg': a
    // pointer to a 'btlso::StreamSocket' to create the server socket; the port
    // number the server will use.
{
    ASSERT(arg);

    ConnectionInfo info(*(ConnectionInfo*) arg); // make a full-blown copy
    if (globalVeryVerbose) {
        PT(info.d_numConnections);
    }

    if (!info.d_numConnections) {
        return 0;                                                     // RETURN
    }
    ASSERT(0 == info.d_serverSocket_p->listen(info.d_equeueSize));

    int i = 0;
    while (1) {
        btlso::StreamSocket<btlso::IPv4Address> *connection = 0;

        int s= info.d_serverSocket_p->accept(&connection);

        if (0 == s) {
            ++i;
            if (globalVeryVerbose) {
                QT("A new connection is established.");
            }
            info.d_connList->push_back(connection);
            if (i >= info.d_numConnections) break;
        }
    }
    if (globalVerbose) {
        QT("Server finishes now.");
    }
    return 0;
}

static int testExecutionHelper(btlsos::TcpTimedCbConnector *connector,
                               int                         *numConnections,
                               btlso::TcpTimerEventManager *manager,
                               const TestCommand           *command,
                               const btlso::IPv4Address    *newPeer)
{
    int rCode = 0;

    switch (command->d_commandCode) {
    case 'R': {  // a "CONNECT" request
        if (command->d_channelType) {
            // a 'btlsos::TcpTimedCbChannel'
            bsl::function<void(btlsc::TimedCbChannel*, int)> cb(
                    bdlf::BindUtil::bind(
                                &connectCb,
                                _1, _2,
                                connector, numConnections,
                                command->d_validChannel,
                                command->d_expStatus,
                                command->d_cancelFlag));

            if (command->d_timedRequestFlag) {
                bsls::TimeInterval timeout1(0,  10),
                                  timeout2(50, 10000000);
                timeout1 += bdlt::CurrentTime::now();
                timeout2 += bdlt::CurrentTime::now();
                if (command->d_timeoutFlag) {
                    // This request should timeout.
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->timedAllocateTimed(cb, timeout1));
                }
                else {
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                                connector->timedAllocateTimed(cb, timeout2));
                }
            }
            else {
                LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocateTimed(cb));
            }
        }
        else {
            // a 'btlsos::TcpCbChannel'
            bsl::function<void(btlsc::CbChannel*, int)> cb(
                    bdlf::BindUtil::bind(
                                &connectCb,
                                _1, _2,
                                connector, numConnections,
                                command->d_validChannel,
                                command->d_expStatus,
                                command->d_cancelFlag));

            if (command->d_timedRequestFlag) {
                bsls::TimeInterval timeout1(0,  10),
                                  timeout2(50, 10000000);
                timeout1 += bdlt::CurrentTime::now();
                timeout2 += bdlt::CurrentTime::now();
                if (command->d_timeoutFlag) {
                    // This request should timeout.
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->timedAllocate(cb, timeout1));
                }
                else {
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                                connector->timedAllocate(cb, timeout2));
                }
            }
            else {
                LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocate(cb));
            }
        }
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
    }  break;
    case 'C': {
        connector->cancelAll();
    } break;
    case 'D': {
        int ret = manager->dispatch(0);
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME * 2);
        rCode = !ret;
    } break;
    case 'I': {
        connector->invalidate();
        connector->isInvalid();
    } break;
    case 'S': {
        connector->setPeer(*newPeer);
    } break;
    default:
        break;
    }
    return rCode;
}

static int createServerThread(
          bslmt::ThreadUtil::Handle                              *threadHandle,
          btlso::InetStreamSocketFactory<btlso::IPv4Address>     *factory,
          btlso::StreamSocket<btlso::IPv4Address>                *serverSocket,
          bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> *connList,
          int                                                     numConnects,
          btlso::IPv4Address                                     *localAddress)
{
    bslmt::ThreadAttributes attributes;
    btlso::IPv4Address serverAddress;
    serverAddress.setIpAddress(hostName);
    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

    int ret = serverSocket->setOption(btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_REUSEADDRESS,
                                      1);
    ASSERT(0 == ret);
    ASSERT(0 == serverSocket->bind(serverAddress));

    ASSERT(0 == serverSocket->localAddress(localAddress));

    #ifndef BTLSO_PLATFORM_WIN_SOCKETS
    // Windows has a bug -- setting listening socket to non- blockingmode will
    // force subsequent 'accept' calls to return WSAEWOULDBLOCK *even when
    // connection is present*.

    ret = serverSocket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);

    ASSERT(0 == ret);

    #endif

    ConnectionInfo connectInfo = { factory,
                                   serverSocket,
                                   k_DEFAULT_EQUEUE_SIZE,
                                   connList,
                                   numConnects
                                 };

    bslmt::ThreadUtil::create(threadHandle, attributes,
                             threadToAcceptConnection, &connectInfo);

    bslmt::ThreadUtil::microSleep(k_SLEEP_TIME * 3);
    return ret;
}

int getMaxconnectValue(TestCommand *commands, int maxCommands)
{
    int i = 0;
    while (i < maxCommands) {
        if (0 == commands[i].d_commandCode) {
            return i > 0? commands[i - 1].d_expNumChannels : 0;       // RETURN
        }
        ++i;
    }
    return 0;
}

//  Usage example 3: Ticker client
//  ------------------------------

class my_Tick {
    char   d_name[5];
    double d_bestBid;
    double d_bestOffer;
  public:
    my_Tick() { }
    my_Tick(const char *ticker);
    my_Tick(const char *ticker, double bestBid, double bestOffer);
    ~my_Tick() { ASSERT (d_bestBid > 0); };

    static int maxSupportedBdexVersion(int versionSelector) {
        (void)versionSelector;
        return 1;
    }

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this 'my_Tick' to the specified output 'stream' formatted in
        // the specified 'version' and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that the 'version' is not written to the stream.  See the
        // package group level documentation for more information on 'bdex'
        // streaming of container types.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If the
        // 'version' is not supported, 'stream' is marked invalid, but this
        // object is unaltered.  Note that no version is read from 'stream'.
        // (See the package-group-level documentation for more information on
        // 'bdex' streaming of container types.)

    void print(bsl::ostream& stream) const;
};

my_Tick::my_Tick(const char *ticker)
: d_bestBid(0)
, d_bestOffer(0)
{
    #ifdef BSLS_PLATFORM_OS_UNIX
    snprintf(d_name, sizeof d_name, "%s",ticker);
    #else
    if (_snprintf(d_name, sizeof d_name, "%s",ticker) < 0) {
        d_name[sizeof d_name - 1] = '\0';
    }
    #endif
}

my_Tick::my_Tick(const char *ticker, double bestBid, double bestOffer)
: d_bestBid(bestBid)
, d_bestOffer(bestOffer)
{
    #ifdef BSLS_PLATFORM_OS_UNIX
    snprintf(d_name, sizeof d_name, "%s",ticker);
    #else
    if (_snprintf(d_name, sizeof d_name, "%s",ticker) < 0) {
        d_name[sizeof d_name - 1] = '\0';
    }
    #endif
}

void my_Tick::print(bsl::ostream& stream) const {
    stream << "(" << d_name << ", " << d_bestBid << ", " << d_bestOffer
           << ")" << endl;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const my_Tick& tick) {
    tick.print(stream);
    return stream;
}

template <class STREAM>
inline
STREAM& my_Tick::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putString(d_name);
        stream.putFloat64(d_bestBid);
        stream.putFloat64(d_bestOffer);
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& my_Tick::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
          bsl::string temp1;
          stream.getString(temp1);
          int maxLen = sizeof (d_name) - 1, // the valid name length
              len = temp1.length();
          if (len < maxLen) {
              strcpy(d_name, temp1.c_str());
          }
          else {
              strncpy(d_name, temp1.c_str(), len);
              d_name[len] = 0;
          }
          stream.getFloat64(d_bestBid);
          stream.getFloat64(d_bestOffer);
      } break;
    }
    return stream;
}

#if 0  // not used
static void myPrintTick(bsl::ostream& stream, const char *buffer, int len)
    // Print the value of the specified 'buffer' interpreted as a 'bdex'
    // byte-stream representation of a 'my_Tick' value, to the specified
    // 'stream' or report an error to 'stream' if 'buffer' is determined *not*
    // to hold an encoding of a valid 'my_Tick' value.
{
    my_Tick tick;
    bslx::ByteInStream input(buffer, len);
    input >> tick;
    stream << tick;
}
#endif

    class my_TickerplantSimulator {
        // Accept raw tick values in ASCII sent as fixed-sized packets via a
        // single 'btlsc::TimedCbChannel' and send them asynchronously one by
        // one to a peer (or similar peers) connected via channels provided via
        // a 'btlsc::TimedCbChannelAllocator'.  Both the output channel
        // allocator and the input channel are supplied at construction.

        btlsc::TimedCbChannelAllocator *d_connector_p;  // outgoing connections
        btlso::TcpTimerEventManager    *d_eventManager_p;
        bsl::ostream&                  d_console;      // where to write errors
        const int                      d_inputSize;    // input packet size

        bsl::function<void(btlsc::TimedCbChannel*, int)>
                                       d_allocateFunctor;
        btlsc::TimedCbChannel::BufferedReadCallback d_readFunctor;  // reused

      private:
        void connectCb(btlsc::TimedCbChannel *serverChannel, int status);
            // Called when a new server channel has been established.  ...

        void writeCb(int                    status,
                     int                    asyncStatus,
                     btlsc::TimedCbChannel *serverChannel,
                     int                    messageSize,
                     int                    maxTicks);
            // Called when a write operation to the server channel ends.  ...

        void timeCb(int                 lastNumTicks,
                    int                *curNumTicks,
                    int                 maxTicks,
                    bsls::TimeInterval  lastTime);
            // To calculate the tick send/receive rate (Ticks/second).

      private:  // Not implemented.
        my_TickerplantSimulator(const my_TickerplantSimulator&);
        my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);

      public:
        my_TickerplantSimulator(bsl::ostream&                   console,
                                btlsc::TimedCbChannelAllocator *connector,
                                btlso::TcpTimerEventManager    *eventManager,
                                int                             inputSize);
            // Create a non-blocking ticker-plant simulator using the specified
            // 'input' channel to read ASCII tick records of the specified
            // 'inputSize' and convert each record to a 'my_Tick' structure;
            // each tick value is sent asynchronously to a peer via a distinct
            // channel obtained from the specified 'connector', reporting any
            // errors to the specified 'console'.  If 'connector' fails or is
            // unable to succeed after 30 seconds, or if transmission itself
            // exceeds 10 seconds, display a message on 'console' and abort the
            // transmission.  If three successive reads of the input channel
            // fail to produce a valid ticks, invalidate the channel and shut
            // down this simulator.  The behavior is undefined unless
            // '0 < inputSize'.

        ~my_TickerplantSimulator();
            // Destroy this simulator object.
        int connect();
    };

    static const bsls::TimeInterval CONNECT_TIME_LIMIT(30, 0);  // 30 seconds
    static const bsls::TimeInterval WRITE_TIME_LIMIT(3600, 0);  // 1 hour

    void my_TickerplantSimulator::connectCb(
                                          btlsc::TimedCbChannel *serverChannel,
                                          int                    status)
    {
        if (serverChannel) {     // Successfully created a connection.
            struct Tick {               // for usage example
                char   d_name[5];
                double d_bestBid;
                double d_bestOffer;
            } ticks[] = {
              {"MSF1", 20.5, 10.5},
              {"MSF2", 21.5, 11.5},
              {"MSF3", 22.5, 12.5},
              {"MSF4", 23.5, 13.5},
              {"MSF5", 24.5, 14.5},
              {"MSF6", 25.5, 15.5},
              {"MSF7", 26.5, 16.5},
              {"MSF8", 27.5, 17.5},
            };
            const int NUM_TICKS = sizeof ticks / sizeof *ticks;
            enum { k_NUM_ITERATIONS = 1000000 };

            for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
                 my_Tick tick(ticks[i % NUM_TICKS].d_name,
                              ticks[i % NUM_TICKS].d_bestBid,
                              ticks[i % NUM_TICKS].d_bestOffer);

                 bslx::ByteOutStream bos(20150811);
                 bos << tick;

                 int msgSize = static_cast<int>(bos.length());

                 btlsc::TimedCbChannel::WriteCallback functor(
                         bdlf::BindUtil::bind(
                             &my_TickerplantSimulator::writeCb
                           , this
                           , _1, _2
                           , serverChannel
                           , msgSize
                           , (int) k_NUM_ITERATIONS));

                 bsls::TimeInterval now = bdlt::CurrentTime::now();

                 // Initiate a timed non-blocking write operation.
                 if (serverChannel->timedBufferedWrite(
                                             bos.data(),
                                             msgSize,
                                             now + WRITE_TIME_LIMIT,
                                             functor)) {

                     d_console << "Error: Unable even to register a write"
                                  " operation on this channel." << bsl::endl;

                     d_connector_p->deallocate(serverChannel);
                 }
            }
        }
        else if (status > 0 ) {  // Interrupted due to external event.
            ASSERT (0);  // Impossible, not authorized.
        }
        else if (0 == status) {  // Interrupted due to timeout event.
            d_console << "Error: Connector timed out, transition aborted."
                      << bsl::endl;
        }
        else {  // Connector failed.
            ASSERT (0 < status);
            bsl::cout << "Error: Unable to connect to server." << bsl::endl;

            // The server is down; invalidate the input channel, allowing
            // existing write operations to complete before the simulator shuts
            // down.
        }
    }

    void my_TickerplantSimulator::writeCb(int                    status,
                                          int                    asyncStatus,
                                          btlsc::TimedCbChannel *serverChannel,
                                          int                    messageSize,
                                          int                    maxTicks)
    {
        static int curNumTicks = 0;
        ASSERT(serverChannel);
        ASSERT(0 < messageSize);
        ASSERT(status <= messageSize);

        enum { k_TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        static bsls::TimeInterval lastTime(now);
        const bsls::TimeInterval PERIOD(k_TIME_LEN, 0);

        // Tracing message: Print out any abnormal tick sending information.
        if (messageSize != status) {
            d_console << "messageSize = " << messageSize << "; status = "
                      << status << "; asyncStatus = " << asyncStatus
                      << bsl::endl;
        }

        if (messageSize == status) {
            // Encoded tick value written successfully.
            ++curNumTicks;
            if (0 == (curNumTicks % 200000)) {
                d_console << curNumTicks << " ticks has been sent. "
                          << bsl::endl;
                d_console << "The current time value: "
                          << now << bsl::endl;
            }

            if (1 == curNumTicks) {
                bsl::function<void()> timerFunctor(
                        bdlf::BindUtil::bind(
                            &my_TickerplantSimulator::timeCb
                          , this
                          , 0
                          , &curNumTicks
                          , maxTicks
                          , now));
               d_eventManager_p->registerTimer(now + k_TIME_LEN, timerFunctor);
                d_console << "registered a timer.  time " << now + k_TIME_LEN
                          << bsl::endl;
            }
        }
        else if (0 <= status) {   // Tick message timed out.

            ASSERT(0 == asyncStatus // only form of partial-write authorized
                   || (0 >  asyncStatus // This operations was dequeued due to
                       && 0 == status)); // a previous partial write operation.
            if (0 < asyncStatus) {
                d_console << "The request is interrupted." << bsl::endl;
            }
            else if (0 == asyncStatus) {
                d_console << "Write of tick data timed out." << bsl::endl;

                if (status > 0) {
                    d_console << "Partial tick data written;"
                              << "  status = " << status
                              << "asyncStatus = " << asyncStatus << bsl::endl;
                }
                else {
                    d_console << "No data was written; channel is still valid."
                              << bsl::endl;
                }
            }
            else {
                ASSERT(0 > asyncStatus && 0 == status);
                if (0 == status) {
                    d_console << "This operation was dequeued. "
                              << "status = " << status << "; asyncStatus = "
                              << asyncStatus << bsl::endl;
                }
                else {
                    d_console << "Write (efficiently) transmitted " << status
                              << " of bytes." << bsl::endl;
                }
                ASSERT (0);
            }
        }
        else {  // Tick message write failed.
            ASSERT(0 > status);
            d_console << "Error: Unable to write tick value to server."
                      << bsl::endl;
        }
    }

    void my_TickerplantSimulator::timeCb(int                 lastNumTicks,
                                         int                *curNumTicks,
                                         int                 maxTicks,
                                         bsls::TimeInterval  lastTime)
    {
        int numTicks = *curNumTicks - lastNumTicks;
        enum { k_TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();

        bsls::TimeInterval timePeriod = now - lastTime;
        double numSeconds = static_cast<double>(timePeriod.seconds()) +
                  static_cast<double>(timePeriod.nanoseconds()) / 1000000000.0;
        cout << numTicks <<" ticks were sent in "
             << numSeconds << " seconds." << endl;

        cout << "The send rate is " << (int) (numTicks / numSeconds)
             << " Ticks/second." << endl << endl;
        if (*curNumTicks < maxTicks) {
            bsl::function<void()> timerFunctor(
                    bdlf::BindUtil::bind(
                        &my_TickerplantSimulator::timeCb
                      , this
                      , *curNumTicks
                      , curNumTicks
                      , maxTicks
                      , now));
            d_eventManager_p->registerTimer(now + k_TIME_LEN, timerFunctor);
        }
    }

    my_TickerplantSimulator::
        my_TickerplantSimulator(bsl::ostream&                   console,
                                btlsc::TimedCbChannelAllocator *connector,
                                btlso::TcpTimerEventManager    *eventManager,
                                int                             inputSize)
    : d_connector_p(connector)
    , d_eventManager_p(eventManager)
    , d_console(console)
    , d_inputSize(inputSize)
    {
        ASSERT(&console);
        ASSERT(connector);
        ASSERT(eventManager);
        ASSERT(0 < inputSize);

        d_allocateFunctor
           = bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::connectCb, this);
    }

    int my_TickerplantSimulator::connect() {
        return d_connector_p->timedAllocateTimed(d_allocateFunctor,
                              bdlt::CurrentTime::now() + CONNECT_TIME_LIMIT);

    }

    my_TickerplantSimulator::~my_TickerplantSimulator()
    {
        ASSERT(&d_console);
        ASSERT(d_connector_p);
        ASSERT(0 < d_inputSize);
    }

//-----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;          globalVerbose = verbose;
    int veryVerbose = argc > 3;      globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4;  globalVeryVeryVerbose = veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&testAllocator);

    ASSERT(0 == btlso::SocketImpUtil::startup());
    switch (test) {case 0:
      case -4: {
        // ----------------------------------------------------------------
        // CASE STUDY: Multi-threaded echo client We create a multi-threaded
        // echo client where echo thread creates a number of connections to the
        // server and then sends a configurable number of packets on each
        // connection, read them back and verifies that packets are echoed
        // correctly.
        // ----------------------------------------------------------------

        if (verbose) cout << "Multi-threaded echo client"
                          << "==========================" << endl;
        // Arguments provided to the test driver are <case#> <numThreads>
        // <numConnections> <portNumber> <host>
        //         <numMessages> <hint>

        enum {
            k_DEFAULT_NUM_THREADS     = 8,
            k_DEFAULT_PORT_NUMBER     = 1635,
            k_DEFAULT_NUM_CONNECTIONS = 10,
            k_DEFAULT_NUM_MESSAGES    = 10000
        };

        int numThreads;
        my_MtEchoClientState state;

        if (argc > 2) {
            numThreads = atoi(argv[2]);
            if (numThreads < 0) {
                numThreads = -numThreads;
                verbose = 0; globalVerbose = 0;
            }
        }
        else {
            numThreads = k_DEFAULT_NUM_THREADS;
        }

        if (argc > 3) {
            state.d_numConnections = atoi(argv[3]);
            if (state.d_numConnections < 0) {
                state.d_numConnections = -state.d_numConnections;
                verbose = 0; globalVerbose = 0;
            }
        }
        else {
            state.d_numConnections = k_DEFAULT_NUM_CONNECTIONS;
        }

        if (argc > 4) {
            state.d_portNumber = atoi(argv[4]);
            if (state.d_portNumber < 0) {
                state.d_portNumber = -state.d_portNumber;
                veryVerbose = 0; globalVeryVerbose = 0;
            }
        }
        else {
            state.d_portNumber = k_DEFAULT_PORT_NUMBER;
        }

        if (argc > 5) {
            state.d_hostname = argv[5];
            veryVeryVerbose = 0;
            globalVeryVeryVerbose = veryVeryVerbose;
        }
        else {
            state.d_hostname = "127.0.0.1";
        }

        if (argc > 6) {
            state.d_numMessages = atoi(argv[6]);
            if (state.d_numMessages < 0) {
                state.d_numMessages = -state.d_numMessages;
                veryVerbose = 0; globalVeryVerbose = 0;
            }
        }
        else {
            state.d_numMessages = k_DEFAULT_NUM_MESSAGES;
        }

        state.d_hint
            = btlso::TcpTimerEventManager::e_NO_HINT;
        if (argc > 7) {
            state.d_hint =
                btlso::TcpTimerEventManager::e_INFREQUENT_REGISTRATION;
        }

        Q("Configuration:");
        P(numThreads);
        P(state.d_numConnections);
        P(state.d_hostname);
        P(state.d_portNumber);
        P(state.d_numMessages);
        P(state.d_hint);

        bsl::vector<bslmt::ThreadUtil::Handle> threadHandles(numThreads);

        for (int i = 0; i < numThreads; ++i) {
            LOOP_ASSERT(i, 0 == bslmt::ThreadUtil::create(&threadHandles[i],
                                                      echoClientThread,
                                                      &state));

        }

        for (int i = 0; i < numThreads; ++i) {
            LOOP_ASSERT(i, 0 == bslmt::ThreadUtil::join(threadHandles[i]))

        }
      } break;
      case -1: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE 1
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE 1
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting Ticker Client"
                              << "\n=====================" << endl;
            {
                const char *const DEFAULT_HOST = "sundev1";
                enum { k_DEFAULT_PORT = 5000 };
                enum { k_DEFAULT_SIZE = 10 };

                const char *hostName = argc > 2 ? argv[2]       : DEFAULT_HOST;
              const int portNumber = argc > 3 ? atoi(argv[3]) : k_DEFAULT_PORT;
              const int inputSize  = argc > 4 ? atoi(argv[4]) : k_DEFAULT_SIZE;

                // INBOUND: This simulator accepts connections on port
                // 'DEFAULT_PORT' only.

                btlso::IPv4Address serverAddress;

                int ret = btlso::ResolveUtil::getAddress(&serverAddress,
                                                        hostName);
                ASSERT(0 == ret);
                serverAddress.setPortNumber(portNumber);
                cout << "serverAddress = " << serverAddress << endl;

                btlso::InetStreamSocketFactory<btlso::IPv4Address> sf;
                btlso::TcpTimerEventManager::Hint infrequentRegistrationHint =
                    btlso::TcpTimerEventManager::e_INFREQUENT_REGISTRATION;
                btlso::TcpTimerEventManager sem(infrequentRegistrationHint);

                btlsos::TcpTimedCbConnector connector(&sf, &sem);
                connector.setPeer(serverAddress);

                my_TickerplantSimulator
                     simulator(bsl::cout, &connector, &sem, inputSize);
                ASSERT(0 == simulator.connect());
                if (verbose) {
                    cout << "client begins to dispatch()....." << endl;
                }

                while (0 != sem.dispatch(0)) {
                     // Do nothing.
                }
            }
        } break;
        case -2: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE 1
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE 1
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;
            {
                // Arguments provided to the test driver are <case#>
                // <numConnections> <portNumber> <host>
                enum {
                    k_DEFAULT_PORT_NUMBER     =  1234,
                    k_DEFAULT_NUM_CONNECTIONS =    10,
                    k_DEFAULT_NUM_MESSAGES    = 10000
                };

                int numConnections, portNumber;
                const char *hostname;

                if (argc > 2) {
                    numConnections = atoi(argv[2]);
                    if (numConnections < 0) {
                        numConnections = -numConnections;
                        verbose = 0; globalVerbose = 0;
                    }
                }
                else {
                    numConnections = k_DEFAULT_NUM_CONNECTIONS;
                }

                if (argc > 3) {
                    portNumber = atoi(argv[3]);
                    if (portNumber < 0) {
                        portNumber = -portNumber;
                        veryVerbose = 0; globalVeryVerbose = 0;
                    }
                }
                else {
                    portNumber = k_DEFAULT_PORT_NUMBER;
                }

                if (argc > 4) {
                    hostname = argv[4];
                    veryVeryVerbose = 0;
                    globalVeryVeryVerbose = veryVeryVerbose;
                }
                else {
                    hostname = "127.0.0.1";
                }

                btlso::TcpTimerEventManager::Hint hint
                     = btlso::TcpTimerEventManager::e_NO_HINT;
                if (argc > 6) {
                    hint =
                    btlso::TcpTimerEventManager::e_INFREQUENT_REGISTRATION;
                }
                if (verbose) {
                    P(numConnections);
                    P(hostname);
                    P(portNumber);
                    P(hint);
                }

                bslma::TestAllocator   testAllocator(veryVeryVerbose);
                testAllocator.setNoAbort(1);

                btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                      factory(&testAllocator);

                btlso::IPv4Address serverAddress (hostname, portNumber);

                btlso::TcpTimerEventManager     manager(hint, &testAllocator);
                my_DataStream client(&factory, &manager, serverAddress);

                ASSERT(0 == client.setUpCallbacks());

                if (verbose) {
                    cout << "Echo server is at " << serverAddress << endl;
                }

                while (manager.numEvents()) {
                    int s = manager.dispatch(0);
                    ASSERT(0 <= s);
                }
            }
        } break;
        case -3: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE 2
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE 2
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;
            {
                // Arguments provided to the test driver are <case#>
                // <numConnections> <portNumber> <host> <numMessages>
                enum {
                    k_DEFAULT_PORT_NUMBER     =  1635,
                    k_DEFAULT_NUM_CONNECTIONS =    10,
                    k_DEFAULT_NUM_MESSAGES    = 10000
                };

                int numConnections, portNumber, numMessages;
                const char *hostname;

                if (argc > 2) {
                    numConnections = atoi(argv[2]);
                    if (numConnections < 0) {
                        numConnections = -numConnections;
                        verbose = 0; globalVerbose = 0;
                    }
                }
                else {
                    numConnections = k_DEFAULT_NUM_CONNECTIONS;
                }

                if (argc > 3) {
                    portNumber = atoi(argv[3]);
                    if (portNumber < 0) {
                        portNumber = -portNumber;
                        veryVerbose = 0; globalVeryVerbose = 0;
                    }
                }
                else {
                    portNumber = k_DEFAULT_PORT_NUMBER;
                }

                if (argc > 4) {
                    hostname = argv[4];
                    veryVeryVerbose = 0;
                    globalVeryVeryVerbose = veryVeryVerbose;
                }
                else {
                    hostname = "127.0.0.1";
                }

                if (argc > 5) {
                    numMessages = atoi(argv[5]);
                    if (numMessages < 0) {
                        numMessages = -numMessages;
                        veryVerbose = 0; globalVeryVerbose = 0;
                    }
                }
                else {
                    numMessages = k_DEFAULT_NUM_MESSAGES;
                }
                btlso::TcpTimerEventManager::Hint hint
                     = btlso::TcpTimerEventManager::e_NO_HINT;
                if (argc > 6) {
                    hint =
                    btlso::TcpTimerEventManager::e_INFREQUENT_REGISTRATION;
                }
                if (verbose) {
                    P(numConnections);
                    P(hostname);
                    P(portNumber);
                    P(numMessages);
                    P(hint);
                }

                bslma::TestAllocator   testAllocator;
                testAllocator.setNoAbort(1);

                btlso::InetStreamSocketFactory<btlso::IPv4Address>
                    factory(&testAllocator);
                btlso::TcpTimerEventManager     manager(hint, &testAllocator);
                my_EchoClient client(&factory, &manager, numConnections,
                                     numMessages,
                                     &testAllocator);

                btlso::IPv4Address serverAddress (hostname, portNumber);

                ASSERT(0 == client.setPeer(serverAddress));

                if (verbose) {
                    cout << "Echo server is at " << serverAddress << endl;
                }

                while (manager.numEvents()) {
                    int s = manager.dispatch(0);
                    ASSERT(0 <= s);
                    ASSERT(0 == testAllocator.numMismatches());
                }
                ASSERT(0 == testAllocator.numMismatches());
            }
        } break;
        case 9: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE
            // ----------------------------------------------------------------
            if (verbose) {
                QT("Testing Usage Example");
                QT("=====================");
            }

            {
                btlso::IPv4Address serverAddress, localAddress;
                serverAddress.setIpAddress(hostName);
                serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                btlso::StreamSocket<btlso::IPv4Address> *d_serverSocket_p =
                                              factory.allocate();

                ASSERT(0 != d_serverSocket_p);
                int ret = d_serverSocket_p->setOption(
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_REUSEADDRESS,
                                      1);
                ASSERT(0 == ret);
                ASSERT(0 == d_serverSocket_p->bind(serverAddress));

                ASSERT(0 == d_serverSocket_p->localAddress(&localAddress));

                const int NUM_CONNECTIONS = 10;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                 connList(&testAllocator);
                ConnectionInfo connectInfo = { &factory,
                                               d_serverSocket_p,
                                               k_DEFAULT_EQUEUE_SIZE,
                                               &connList,
                                               NUM_CONNECTIONS
                                             };

                bslmt::ThreadUtil::Handle threadHandle;
                bslmt::ThreadAttributes attributes;
                bslmt::ThreadUtil::create(&threadHandle, attributes,
                                     threadToAcceptConnection, &connectInfo);

                int numConnections = 0;
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
                {
                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj connector(&factory, &manager, &testAllocator);
                    connector.setPeer(localAddress);

                    int validChannel = 1, expStatus = 0, cancelFlag = 0;

                    bsl::function<void(btlsc::CbChannel*, int)> cb(
                            bdlf::BindUtil::bind(
                                        &connectCb,
                                        _1, _2,
                                        &connector,
                                        &numConnections, validChannel,
                                        expStatus, cancelFlag));

                    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
                        connector.allocate(cb);
                        manager.dispatch(0);
                        ASSERT(i + 1 == connector.numChannels());
                    }
                }
                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                bslmt::ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(d_serverSocket_p);
            }
        } break;
        case 8: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'invalidate' method:
            //
            // Plan:
            //
            // Testing
            //   int invalidate();
            // ----------------------------------------------------------------
            enum {
                k_MAX_COMMANDS = 20
            };

            // ----------------------------------------------------------------
            if (verbose) {
                QT("Testing 'invalidate' method");
                QT("===========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[][k_MAX_COMMANDS] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                     //A     B    C   D   E   F   G   H   I   J   K     L
//                     //-     -    -   -   -   -   -   -   -   -   -     -
//                 {
//                   { // There's no channel established before calling
//                     // invalidate().
//                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  0,  0,   e_VALID },
//                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                   },
//                   { // There's one channel established before calling
//                     // invalidate().
//                    { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  1,  0,   e_VALID },
//                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  1,  0,   e_VALID },
//                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
//                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
//                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
//                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
//                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
//                   },
//                 };
//                 #else
                {
                    // Because there are many items listed in the table for
                    // each test command, the following document specify which
                    // column in the table specifies which item.
                    //..
                    // Column     Item               Column       Item
                    // ------     ----               ------       ----
                    //   A     d_lineNum                G     d_expStatus
                    //   B     d_commandCode            H     d_expNumEvents
                    //   C     d_channelType            I     d_expNumTimers
                    //   D     d_timedRequestFlag       J     d_expNumChannels
                    //   E     d_timeoutFlag            K     d_cancelFlag
                    //   F     d_validChannel           L     d_returnValue
                    //..

                    //A     B    C   D   E   F   G   H   I   J   K    L
                    //-     -    -   -   -   -   -   -   -   -   -    -
                  { // There's no channel established before calling
                    // invalidate().
                   { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                   { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                   { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                   { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                   { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                   { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                  },
                  { // A request should be valid if it's submitted before
                    // calling invalidate().
                   { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  0,  0,   e_VALID },
                   { L_,  'I',  0,  0,  0,  0,  0,  2,  1,  0,  0,   e_VALID },
                   { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   e_VALID },
                   { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                  },
                  { // One channel is established before calling invalidate().
                   { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  0,  0,   e_VALID },
                   { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   e_VALID },
                   { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  1,  0,   e_VALID },
                   { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, e_INVALID },
                   { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                  },
                  { // One channel is established and another request has been
                    // submitted before calling invalidate().
                   { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  0,  0,   e_VALID },
                   { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   e_VALID },
                   { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  1,  0,   e_VALID },
                   { L_,  'I',  0,  0,  0,  0,  0,  2,  1,  1,  0, e_INVALID },
                   { L_,  'R',  1,  1,  0,  0,  0,  2,  1,  1,  0, e_INVALID },
                   { L_,  'R',  1,  0,  0,  0,  0,  2,  1,  1,  0, e_INVALID },
                   { L_,  'R',  0,  0,  0,  0,  0,  2,  1,  1,  0, e_INVALID },
                   { L_,  'R',  0,  1,  0,  0,  0,  2,  1,  1,  0, e_INVALID },
                   { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  2,  0,   e_VALID },
                   { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, e_INVALID },
                  },
                };
//                 #endif

                const int NUM_COMMANDS_SET =
                                            sizeof commands / sizeof *commands;

                for (int i = 0; i < NUM_COMMANDS_SET; ++i) {
                    bslmt::ThreadUtil::Handle threadHandle;
                    btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();

                    int maxConnections = 0, numConnections = 0;
                    maxConnections = getMaxconnectValue(commands[i],
                                                        k_MAX_COMMANDS);
                    btlso::IPv4Address serverAddress;
                    bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                    connList(&testAllocator);

                    LOOP_ASSERT(i, 0 == createServerThread(&threadHandle,
                                              &factory, serverSocket,
                                              &connList, maxConnections,
                                              &serverAddress));
                    // The client is waiting for the server.
                    bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                    btlso::TcpTimerEventManager manager(&testAllocator);

                    Obj connector(&factory, &manager, &testAllocator);
                    connector.setPeer(serverAddress);
                    int j = 0;
                    while (commands[i][j].d_commandCode) {
                        LOOP2_ASSERT(i, j, 0 == testExecutionHelper(&connector,
                                                         &numConnections,
                                                         &manager,
                                                         &commands[i][j], 0));
                        LOOP2_ASSERT(i, j, commands[i][j].d_expNumEvents ==
                                                 manager.numEvents());
                        LOOP2_ASSERT(i, j, commands[i][j].d_expNumTimers ==
                                                 manager.numTimers());

                        LOOP2_ASSERT(i, j, commands[i][j].d_expNumChannels ==
                                                 connector.numChannels());
                        if (veryVerbose) {
                            P_T(i);   P_T(j);
                            P_T(manager.numEvents());
                            P_T(manager.numTimers());
                            PT(connector.numChannels());
                        }
                        ++j;
                    }
                    int length = static_cast<int>(connList.size());
                    if (veryVerbose) {
                        QT("The total number of established channels: ");
                        PT(length);
                    }
                    for (int k = 0; k < length; ++k) {
                        factory.deallocate(connList[k]);
                    }
                    factory.deallocate(serverSocket);
                }
            }
#endif
        } break;
        case 7: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'cancelAll' method:
            //
            // Plan:
            //
            // Testing
            //   int cancelAll();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'cancelAll' method");
                QT("==========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                   //A     B    C   D   E   F   G   H   I   J   K   L
//                   //-     -    -   -   -   -   -   -   -   -   -   -
//                 {
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                 };
//                 #else // windows platform
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..
                  //A     B    C   D   E   F    G   H   I   J   K    L
                  //-     -    -   -   -   -    -   -   -   -   -    -
                  // There's no channel established before calling cancelAll().
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  0,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  0,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  0,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  0,  0, e_VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  0,  0, e_VALID },

                  // One channel is established before calling cancelAll().
                  { L_,  'R',  0,  0,  0,  1,   0,  1,  0,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  1,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  1,  0, e_VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  1,  0, e_VALID },

                  // One channel is established before calling cancelAll(), and
                  // a request is submitted before dispatch() can also be
                  // cancelled.
                  { L_,  'R',  1,  0,  0,  1,   0,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  1,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  1,  0,  2,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  2,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  2,  0, e_VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  2,  0, e_VALID },

                  // The cancelAll() is called in the user-installed callback
                  // function.
                  { L_,  'R',  1,  0,  0,  1,   0,  1,  0,  2,  1, e_VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  2,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  2,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  2,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  3,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,   0,  2,  1,  3,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  4,  0, e_VALID },

                  // The cancelAll() is called in the user-installed callback
                  // function, different requests from the above test data are
                  // submitted.
                  { L_,  'R',  0,  1,  0,  1,   0,  2,  1,  4,  1, e_VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  2,  1,  4,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  2,  1,  4,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  2,  1,  4,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  2,  1,  4,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  5,  0, e_VALID },

                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                  connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                bslmt::ThreadUtil::join(threadHandle);

                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 6: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX) && !(defined(BSLS_PLATFORM_OS_SOLARIS) \
                                         || BSLS_PLATFORM_OS_VER_MAJOR < 10)
            // ----------------------------------------------------------------
            // TESTING 'timedAllocateTimed' method:
            //
            // Plan:
            //
            // Testing
            //   int timedAllocateTimed();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'timedAllocateTimed' method");
                QT("===================================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                   //A     B    C   D   E   F   G   H   I   J   K    L
//                   //-     -    -   -   -   -   -   -   -   -   -    -
//                 {
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  4,  0, e_VALID },

//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  6,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  7,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  8,  0, e_VALID },
//                 };
//                 #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..
                  //A     B    C   D   E   F   G   H   I   J   K    L
                  //-     -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  1,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  1,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  2,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  3,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  3,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  4,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  5,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  2,  1,  5,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  6,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  7,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  2,  1,  7,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  8,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, e_VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 10,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1, 11,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, e_VALID },
                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                    connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                bslmt::ThreadUtil::join(threadHandle);

                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 5: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'timedAllocate' methods:
            //
            // Plan:
            //
            // Testing
            //   int timedAllocate();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'timedAllocate' method");
                QT("==============================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   //A     B    C   D   E   F   G   H   I   J   K    L
//                   //-     -    -   -   -   -   -   -   -   -   -    -
//                 {
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  4,  0, e_VALID },

//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  6,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  7,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  8,  0, e_VALID },

//                 };
//                 #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannels
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..

                  //A     B    C   D   E   F   G   H   I   J   K    L
                  //-     -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  1,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  1,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  2,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  3,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  2,  1,  3,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  4,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  5,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  5,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  6,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  7,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  7,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  8,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, e_VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  0,  1,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  2,  1,  9,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1, 10,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 11,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, e_VALID },
                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                     connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                bslmt::ThreadUtil::join(threadHandle);

                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 4: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX) && (!defined(BSLS_PLATFORM_OS_SOLARIS) \
                                        || BSLS_PLATFORM_OS_VER_MAJOR < 10)
            // ----------------------------------------------------------------
            // TESTING 'allocateTimed' methods:
            //
            // Plan:
            //
            // Testing
            //   int allocateTimed();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'allocateTimed' method");
                QT("==============================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                   //A     B    C   D   E   F   G   H   I   J   K    L
//                   //-     -    -   -   -   -   -   -   -   -   -    -
//                 {
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  3,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  4,  0, e_VALID },

//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  6,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  7,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  8,  0, e_VALID },
//                 };
//                 #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..

                  //A     B    C   D   E   F   G   H   I   J   K    L
                  //-     -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  2,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  4,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  2,  1,  6,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  7,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  2,  1,  8,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  9,  0, e_VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  2,  1, 10,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0, 11,  0, e_VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0, 12,  0, e_VALID },
                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                   connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                bslmt::ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 3: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'allocate' methods:
            //
            // Plan:
            //
            // Testing
            //   int allocate();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'allocate' method");
                QT("=========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                   //A     B    C   D   E   F   G   H   I   J   K    L
//                   //-     -    -   -   -   -   -   -   -   -   -    -
//                 {
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  3,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  4,  0, e_VALID },

//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  6,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  7,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  8,  0, e_VALID },

//                 };
//                 #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..

                  //A     B    C   D   E   F   G   H   I   J   K    L
                  //-     -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  2,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  4,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  6,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  8,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, e_VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1, 10,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 11,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, e_VALID },
                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                    connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                bslmt::ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 2: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'ACCESSOR' methods:
            //
            // Plan:
            //
            // Testing
            //   const btlso::IPv4Address& peer() const;
            //   int isInvalid() const;
            //   int numChannels() const;
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'ACCESSOR' method");
                QT("=========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column in
                // the table specifies which item.
                //..
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue
                //..

                TestCommand commands[] =
//                 #ifdef BSLS_PLATFORM_OS_SOLARIS
//                   // On Solaris, non-blocking connects to the loopback succeed
//                   // right away so we do not need a dispatch call.  This is
//                   // absolutely not guaranteed by POSIX though.
//                   //A     B    C   D   E   F   G   H   I   J   K    L
//                   //-     -    -   -   -   -   -   -   -   -   -    -
//                 {
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },
//                   { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  2,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  3,  0, e_VALID },
//                   { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  4,  0, e_VALID },

//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },
//                   { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  6,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  7,  0, e_VALID },
//                   { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  8,  0, e_VALID },
//                 };
//                 #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  //..
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannels
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  //..
                  //A     B    C   D   E   F   G   H   I   J   K    L
                  //-     -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  0,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, e_VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  2,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  3,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  4,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  6,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, e_VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  7,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1,  8,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, e_VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  2,  1, 10,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 11,  0, e_VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, e_VALID },
                };
//                 #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bslmt::ThreadUtil::Handle threadHandle;
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                         factory.allocate();
                ASSERT(serverSocket);
                int maxConnections = 0, numConnections = 0;
                btlso::IPv4Address serverAddress;
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                               connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);

                btlso::TcpTimerEventManager manager(&testAllocator);

                Obj connector(&factory, &manager, &testAllocator);
                connector.setPeer(serverAddress);

                for (int i = 0; i < NUM_COMMANDS; ++i) {
                    LOOP_ASSERT(i, 0 == testExecutionHelper(&connector,
                                                            &numConnections,
                                                            &manager,
                                                            &commands[i], 0));
                    LOOP_ASSERT(i, commands[i].d_expNumEvents ==
                                                 manager.numEvents());
                    LOOP_ASSERT(i, commands[i].d_expNumTimers ==
                                                 manager.numTimers());

                    LOOP_ASSERT(i, commands[i].d_expNumChannels ==
                                                 connector.numChannels());
                    if (veryVerbose) {
                        P_T(i);
                        P_T(manager.numEvents());
                        P_T(manager.numTimers());
                        PT(connector.numChannels());
                    }
                }
                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                bslmt::ThreadUtil::join(threadHandle);
                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 1: {
            // ----------------------------------------------------------------
            // BREATHING TEST:
            //
            // Plan:
            // ----------------------------------------------------------------
            if (verbose) {
                QT("BREATHING TEST");
                QT("==============");
            }

            {
                bslmt::ThreadUtil::Handle threadHandle;
                bslmt::ThreadAttributes attributes;
                btlso::IPv4Address serverAddress, localAddress;
                serverAddress.setIpAddress(hostName);
                serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                btlso::StreamSocket<btlso::IPv4Address> *d_serverSocket_p =
                                              factory.allocate();

                ASSERT(0 != d_serverSocket_p);
                int ret = d_serverSocket_p->setOption(
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_REUSEADDRESS,
                                      1);
                ASSERT(0 == ret);
                ASSERT(0 == d_serverSocket_p->bind(serverAddress));

                ASSERT(0 == d_serverSocket_p->localAddress(&localAddress));

                enum { k_NUM_CONNECTIONS = 10 };
                bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                                   connList(&testAllocator);
                ConnectionInfo connectInfo = { &factory,
                                               d_serverSocket_p,
                                               k_DEFAULT_EQUEUE_SIZE,
                                               &connList,
                                               k_NUM_CONNECTIONS
                                             };

                ret = bslmt::ThreadUtil::create(&threadHandle, attributes,
                                     threadToAcceptConnection, &connectInfo);
                ASSERT(0 == ret);

                int numConnections = 0;
                bslmt::ThreadUtil::microSleep(k_SLEEP_TIME * 10);
                {
                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj connector(&factory, &manager, &testAllocator);
                    connector.setPeer(localAddress);

                    int validChannel = 1, expStatus = 0, cancelFlag = 0;

                    bsl::function<void(btlsc::CbChannel*, int)> cb(
                            bdlf::BindUtil::bind(
                                        &connectCb,
                                        _1, _2,
                                        &connector,
                                        &numConnections, validChannel,
                                        expStatus, cancelFlag));

                    for (int i = 0; i < k_NUM_CONNECTIONS; ++i) {
                        int ret = connector.allocate(cb);
                        LOOP_ASSERT(i, 0 == ret);
                        ret = manager.dispatch(0);

                        ASSERT(i + 1 == connector.numChannels());
                    }
                }
                int length = static_cast<int>(connList.size());
                if (veryVerbose) {
                    QT("The total number of established channels: ");
                    PT(length);
                }
                bslmt::ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(d_serverSocket_p);
            }
        } break;
        default: {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
        }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    ASSERT(0 == btlso::SocketImpUtil::cleanup());
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
