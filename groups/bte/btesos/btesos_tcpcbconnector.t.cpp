// btesos_tcpcbconnector.t.cpp       -*-C++-*-
#include <btesos_tcpcbconnector.h>
#include <btesos_tcptimedcbchannel.h>
#include <btesos_tcpcbchannel.h>

#include <bteso_platform.h>
#include <bteso_ipv4address.h>
#include <bteso_tcptimereventmanager.h>
#include <bteso_inetstreamsocketfactory.h>  // HACK
#include <bteso_socketimputil.h>            // cleanup, startup

#include <bcemt_thread.h>                   // thread management util
#include <bcema_testallocator.h>            // thread-safe allocator

#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>

#include <bslma_testallocator.h>            // allocate memory
#include <bsls_platform.h>
#include <bdetu_systemtime.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdef_PlaceHolders;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
// TBD
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 8] USAGE EXAMPLE 2
// [ 9] USAGE EXAMPLE 1
// ----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error %s (%d): %s (failed)\n",  __FILE__, i, s);
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

//----------------------------------------------------------------------------
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { printf(#I ": %d\n", I); aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\n", I, J); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { printf(#I ": %d\t" #J << ": %d\t" #K ": %d\n", I, J, K); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\t" #K ": %d\t" #L ": %d\n", I, \
                      J, K, L); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\t" #K ": %d\t" #L ": %d\t" \
                      #M ": %d\n", I, J, K, L, M); \
                      aSsErT(1, #X, __LINE__); } }

//----------------------------------------------------------------------------
bcemt_Mutex  d_mutex;   // for i/o synchronization in all threads

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
const char* hostName = "127.0.0.1";
typedef btesos_TcpCbConnector  Obj;

static int globalVerbose;
static int globalVeryVerbose;
static int globalVeryVeryVerbose;

enum {
    DEFAULT_PORT_NUMBER     = 0,
    DEFAULT_NUM_CONNECTIONS = 10,
    DEFAULT_EQUEUE_SIZE     = 5,
    SLEEP_TIME              = 100000,
    VALID                   = 0,
    INVALID                 = -1
};

struct ConnectionInfo {
    // This is the information suite to be passed to the server thread.
    bteso_StreamSocketFactory<bteso_IPv4Address>         *d_factory_p;
    bteso_StreamSocket<bteso_IPv4Address>                *d_serverSocket_p;
    int                                                   d_equeueSize;
    bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> *d_connList;
    int                                                   d_numConnections;
};

struct TestCommand {
  // This struct includes needed information for each test operation,
  // e.g., a request to "CONNECT" or a request to "dispatch",  along with it's
  // corresponding test results.

    int  d_lineNum;

    char d_commandCode;     // a command to invoke a corresponding function,
                            // e.g., 'D' -- invoke the event manager's
                            // dispatch(); 'R' -- an "ACCEPT" request etc.
    int d_channelType;      // a request for a 'btesos_TcpCbChannel'
                            // or 'btesos_TcpTimedCbChannel', i.e.,
                            // 1 for a timed channel and 0 for
                            // a 'btesos_TcpCbChannel'
    int d_timedRequestFlag; // a request with/without timeout request

    int d_timeoutFlag;      // a flag to indicate if the request will timeout

    int d_validChannel;     // a flag to indicate if a new channel is created
                            // or not
    int d_expStatus;        // a expected status value from a "CONNECT" request

    int d_expNumEvents;     // the number of events after the
                            // execution of this command
    int d_expNumTimers;     // the number of timers after the execution of
                            // this command
    int d_expNumChannels;

    int d_cancelFlag;       // a flag to indicate if 'cancelAll' will be called
                            // inside the user callback function.

    int d_returnValue;      // the expected function return value after the
                            // command is executed.
};

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
///Usage example 1
///---------------

class my_EchoClient {
    enum {
            BUFFER_SIZE     = 100
    };

    btesos_TcpCbConnector  d_allocator;
    bdet_TimeInterval      d_readTimeout;
    bdet_TimeInterval      d_writeTimeout;
    char                   d_controlBuffer[BUFFER_SIZE];
    int                    d_numConnections;
    int                    d_maxConnections;
    int                    d_numMessages;

    bdef_Function<void (*)(btesc_TimedCbChannel*, int)> d_allocateFunctor;

    void allocateCb(btesc_TimedCbChannel *channel, int status);
         // Invoked by the socket event manager when a connection is
         // accepted.

    void bufferedReadCb(const char *buffer, int status, int asyncStatus,
                        btesc_TimedCbChannel *channel, int sequence);

    void writeCb(int status, int asyncStatus, btesc_TimedCbChannel *channel,
                 int numBytes);
  private:
    my_EchoClient(const my_EchoClient&);    // Not implemented.
    my_EchoClient&
        operator=(const my_EchoClient&);    // Not implemented.
  public:
    // CREATORS
    my_EchoClient(bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                  bteso_TimerEventManager                      *manager,
                  int                                    maxConnections,
                  int                                       numMessages,
                  bslma_Allocator *basicAllocator = 0);
    ~my_EchoClient();

    //  MANIPULATORS
    int setPeer(const bteso_IPv4Address& address);
        // Set the address of the echo server to the specified 'address',
        // and start sending messages to the server.
};

// CREATORS
my_EchoClient::my_EchoClient(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        bteso_TimerEventManager                *manager,
        int                                     numConnections,
        int                                     numMessages,
        bslma_Allocator                        *basicAllocator)
: d_allocator(factory, manager, basicAllocator)
, d_readTimeout(20.0)
, d_writeTimeout(5,0)
, d_numConnections(0)
, d_maxConnections(numConnections)
, d_numMessages(numMessages)
{
    ASSERT(factory);
    ASSERT(manager);
    d_allocateFunctor = bdef_MemFnUtil::memFn(&my_EchoClient::allocateCb,
                                              this);
    memset(d_controlBuffer, 'A', BUFFER_SIZE);
}

my_EchoClient::~my_EchoClient() {

}

// MANIPULATORS
void my_EchoClient::allocateCb(btesc_TimedCbChannel *channel, int status) {
    if (channel) {
            // Connected to a server.  Issue a buffered write request.
        if (globalVeryVerbose) {
            cout << "my_EchoClient::allocateCb: Channel allocated." << endl;
        }
        bdef_Function<void (*)(int, int)> callback(
                bdef_BindUtil::bind(
                    bdef_MemFnUtil::memFn(&my_EchoClient::writeCb, this)
                  , _1, _2
                  , channel
                  , 0));
        if (channel->timedBufferedWrite(
                                      d_controlBuffer,
                                      BUFFER_SIZE,
                                      bdetu_SystemTime::now() + d_writeTimeout,
                                      callback))
        {
            cout << "Failed to enqueue write request." << endl;
            ASSERT(channel->isInvalidWrite());
            d_allocator.deallocate(channel);
        }

        if (d_maxConnections > ++d_numConnections) {
            int s = d_allocator.allocateTimed(d_allocateFunctor);
            ASSERT(0 == s);
        }
        return;
    }

    ASSERT(0 >= status);    // Interrupts are not enabled.
    if (0 == status) {
        cerr << "Timed out connecting to the server." << endl;
    }
    else {
            // Hard-error accepting a connection, invalidate the allocator.
        cerr << "Non-recoverable error connecting to the server " << endl;
        d_allocator.invalidate();
        return;
    }
    // In any case, except for hard error on allocator, enqueue another
    // connect request
    if (d_maxConnections > ++d_numConnections) {
        int s = d_allocator.allocateTimed(d_allocateFunctor);
        ASSERT(0 == s);
    }
}

void my_EchoClient::bufferedReadCb(const char *buffer, int status,
                                   int asyncStatus,
                                   btesc_TimedCbChannel *channel,
                                   int sequence)
{
   cout << "my_EchoClient::bufferedReadCb: Read " << status
        << " bytes from server." << endl;

   if (globalVeryVeryVerbose) {
        cout << "my_EchoClient::bufferedReadCb: Read " << status
             << " bytes from server." << endl;
    }
    ASSERT(channel);
    if (0 < status) {
        ASSERT(BUFFER_SIZE == status);
        ASSERT(0 == memcmp(buffer, d_controlBuffer, BUFFER_SIZE));

        // If we're not done -- enqueue another request
        if (sequence < d_numMessages) {
            bdef_Function<void (*)(int, int)> callback(
                    bdef_BindUtil::bind(
                        bdef_MemFnUtil::memFn(&my_EchoClient::writeCb, this)
                      , _1, _2
                      , channel
                      , sequence + 1));
            if (channel->timedBufferedWrite(
                                      d_controlBuffer,
                                      BUFFER_SIZE,
                                      bdetu_SystemTime::now() + d_writeTimeout,
                                      callback))
            {
                cout << "Failed to enqueue write request." << endl;
                ASSERT(channel->isInvalidWrite());
                d_allocator.deallocate(channel);
            }
        }
        else {
            cout << "Done transferring data on a channel. " << endl;
            d_allocator.deallocate(channel);
        }
    }
    else if (0 == status) {
        if (0 > asyncStatus) {
            cout << "Callback dequeued" << endl;
        }
        else {
            cerr << "Timed out on read" << endl;
            d_allocator.deallocate(channel);
        }
    }
    else {
        cerr << "Failed to read data: non-recoverable error on channel."
             << endl;
        d_allocator.deallocate(channel);
    }
}

void my_EchoClient::writeCb(int status, int asyncStatus,
                            btesc_TimedCbChannel *channel, int sequence)
{
    cout << "my_EchoClient::writeCb: Send " << status
         << " bytes to server." << endl;

    if (globalVeryVeryVerbose) {
        cout << "my_EchoClient::writeCb: Send " << status
             << " bytes to server." << endl;
    }
    if (0 < status) {
        if (status != BUFFER_SIZE) {
            d_allocator.deallocate(channel);
            ASSERT("Failed to send data to the server" && 0);

        }
        else {
            bdef_Function<void (*)(const char *, int, int)> callback(
                bdef_BindUtil::bind(
                    bdef_MemFnUtil::memFn(&my_EchoClient::bufferedReadCb, this)
                  , _1, _2, _3
                  , channel
                  , sequence));
            if (channel->timedBufferedRead(
                                       BUFFER_SIZE,
                                       bdetu_SystemTime::now() + d_readTimeout,
                                       callback))
            {
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

int my_EchoClient::setPeer(const bteso_IPv4Address& address) {
    d_allocator.setPeer(address);
    return d_allocator.allocateTimed(d_allocateFunctor);
}

///Usage example 2
///---------------
class my_DataStream {
  enum {
      DEFAULT_PORT_NUMBER = 1234,
      QUEUE_SIZE = 16
  };
  btesos_TcpCbConnector  d_allocator;
  bdet_TimeInterval           d_connectTimeout;

  btesc_TimedCbChannel       *d_controlChannel;
  btesc_TimedCbChannel       *d_dataChannel;

  private:
    // Callbacks
    void allocateCb(btesc_TimedCbChannel *channel, int status,
                        btesc_TimedCbChannel **cachedChannel);
        // Invoked from the socket event manager when a connection is
        // allocated (i.e., established) or an error occurs when
        // allocating.
        // ...

    private:
      my_DataStream(const my_DataStream&);    // Not implemented.
      my_DataStream&
            operator=(const my_DataStream&);    // Not implemented.
    public:
      // CREATORS
      my_DataStream(bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                      bteso_TimerEventManager                *manager,
                      const bteso_IPv4Address&                   peerAddress);
          // Create a controlled data stream that uses the specified stream
          // socket 'factory' for system sockets, uses the specified socket
          // event 'manager' to multiplex events on these sockets, and
          // attempts to connect to the server at the specified
          // 'peerAddress'.   The behavior is undefined if either 'factory'
          // or 'manager' is 0.

      ~my_DataStream();
          // Destroy this server.

      // MANIPULATORS
      int setUpCallbacks();
          // Register callbacks as required for establishing communication
          // channels.  Return 0 on success, and a non-zero value otherwise,
          // in which case all further registration attempts will fail
          // (and the object can be only destroyed).
};

my_DataStream::my_DataStream(
            bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
            bteso_TimerEventManager                      *manager,
            const bteso_IPv4Address&                      peerAddress)
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
    // ASSERT(0 == d_allocator.numChannels());
}

int my_DataStream::setUpCallbacks() {
    if (globalVerbose) {
        cout << " The first channel request. " << endl;
    }

    bdef_Function<void (*)(btesc_TimedCbChannel*, int)> callback(
            bdef_BindUtil::bind(
                bdef_MemFnUtil::memFn(&my_DataStream::allocateCb, this)
              , _1, _2
              , &d_controlChannel));

    if (d_allocator.allocateTimed(callback)) {
        return -1;
    }
    if (globalVerbose) {
        cout << " The second channel request. " << endl;
    }

    callback = bdef_BindUtil::bind(
                    bdef_MemFnUtil::memFn(&my_DataStream::allocateCb, this)
                  , _1, _2
                  , &d_dataChannel);

    return d_allocator.allocateTimed(callback);
}

void my_DataStream::allocateCb(btesc_TimedCbChannel *channel, int status,
                               btesc_TimedCbChannel **cachedChannel) {
     ASSERT(cachedChannel);

     if (channel) {
         if (globalVerbose) {
             cout << "One channel is established." << endl;
         }
         *cachedChannel = channel;
         if (d_controlChannel && d_dataChannel) {
             // Ready to do data processing
             // ...
             if (globalVerbose) {
                 cout << "both channels are established." << endl;
             }
        }
    }
    else {
        cout << "not valid channel: status: " << status << endl;
    }
}

static void connectCb(btesc_CbChannel            *channel,
                      int                         status,
                      btesos_TcpCbConnector *connector,
                      int                        *numConnections,
                      int                         validChannel,
                      int                         expStatus,
                      int                         cancelFlag)
    // Verify the result of an "ACCEPT" request by comparing against the
    // expected values:  If the specified 'validChannel' is nonzero, a new
    // channel should be established; the return 'status' should be the same
    // as the specified 'expStatus'.  If the specified 'cancelFlag' is
    // nonzero, invoke the 'cancelAll()' on the specified 'acceptor'
    // for test.
{
    if (validChannel) {
        ASSERT (channel);
    }
    else {
        ASSERT (0 == channel);
    }
    ASSERT (status == expStatus);

    if (0 == channel) {
        if (globalVeryVerbose) {
            PT(status);
        }
    }
    else {
        ++(*numConnections);
        if (globalVeryVerbose) {
            QT("Created a channel. ");
        }
    }
    if (cancelFlag) {
        if (globalVeryVerbose) {
            QT("User's callback function invokes cancelAll()");
        }
        connector->cancelAll();
    }
}

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif
void *threadToAcceptConnection(void *arg)
    // Create a server socket which is ready for accept connections.
    // The following information will be pass in through the specified 'arg':
    // a pointer to a 'bteso_StreamSocket' to create the server socket;
    // the port number the server will use.
{
    ASSERT(arg);

    ConnectionInfo info = *(ConnectionInfo*) arg;
    if (globalVeryVerbose) {
        PT(info.d_numConnections);
    }

    if (!info.d_numConnections) {
        return 0;
    }
    ASSERT(0 == info.d_serverSocket_p->listen(info.d_equeueSize));

    int i = 0;
    while (1) {
        bteso_StreamSocket<bteso_IPv4Address> *connection = 0;

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

static int testExecutionHelper(btesos_TcpCbConnector *connector,
                               int                        *numConnections,
                               bteso_TcpTimerEventManager *manager,
                               const TestCommand          *command,
                               const bteso_IPv4Address    *newPeer)
{
    int rCode = 0;

    switch (command->d_commandCode) {
    case 'R': {  // a "CONNECT" request
        if (command->d_channelType) {
            // a 'btesos_TcpTimedCbChannel'
            bdef_Function<void (*)(btesc_TimedCbChannel*, int)> cb(
                    bdef_BindUtil::bind( &connectCb
                                       , _1, _2
                                       , connector
                                       , numConnections
                                       , command->d_validChannel
                                       , command->d_expStatus
                                       , command->d_cancelFlag));

            if (command->d_timedRequestFlag) {
                bdet_TimeInterval timeout1(0,  10),
                                  timeout2(50, 10000000);
                timeout1 += bdetu_SystemTime::now();
                timeout2 += bdetu_SystemTime::now();
                if (command->d_timeoutFlag) {
                    // This request should timeout.
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocateTimed(cb));
                }
                else {
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                                connector->allocateTimed(cb));
                }
            }
            else {
                LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocateTimed(cb));
            }
        }
        else {
            // a 'btesos_TcpCbChannel'
            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                    bdef_BindUtil::bind( &connectCb
                                       , _1, _2
                                       , connector
                                       , numConnections
                                       , command->d_validChannel
                                       , command->d_expStatus
                                       , command->d_cancelFlag));

            if (command->d_timedRequestFlag) {
                bdet_TimeInterval timeout1(0,  10),
                                  timeout2(50, 10000000);
                timeout1 += bdetu_SystemTime::now();
                timeout2 += bdetu_SystemTime::now();
                if (command->d_timeoutFlag) {
                    // This request should timeout.
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocate(cb));
                }
                else {
                    LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                                connector->allocate(cb));
                }
            }
            else {
                LOOP_ASSERT(command->d_lineNum, command->d_returnValue ==
                            connector->allocate(cb));
            }
        }
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    }  break;
    case 'C': {
        connector->cancelAll();
    } break;
    case 'D': {
        int ret = manager->dispatch(0);
        bcemt_ThreadUtil::microSleep(SLEEP_TIME * 2);
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

static int createServerThread(bcemt_ThreadUtil::Handle           *threadHandle,
            bteso_InetStreamSocketFactory<bteso_IPv4Address>     *factory,
            bteso_StreamSocket<bteso_IPv4Address>                *serverSocket,
            bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> *connList,
            int                                                   numConnects,
            bteso_IPv4Address                                    *localAddress)
{
    bcemt_Attribute attributes;
    bteso_IPv4Address serverAddress;
    serverAddress.setIpAddress(hostName);
    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

    int ret = serverSocket->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                      1);
    ASSERT(0 == ret);
    ASSERT(0 == serverSocket->bind(serverAddress));

    ASSERT(0 == serverSocket->localAddress(localAddress));

    #ifndef BTESO_PLATFORM__WIN_SOCKETS
    // Windows has a bug -- setting listening socket to non-blocking mode will
    // force subsequent 'accept' calls to return WSAEWOULDBLOCK *even when
    // connection is present*.

    ret = serverSocket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

    ASSERT(0 == ret);

    #endif

    ConnectionInfo connectInfo = { factory,
                                   serverSocket,
                                   DEFAULT_EQUEUE_SIZE,
                                   connList,
                                   numConnects
                                 };

    bcemt_ThreadUtil::create(threadHandle, attributes,
                             threadToAcceptConnection, &connectInfo);

    bcemt_ThreadUtil::microSleep(SLEEP_TIME * 3);
    return ret;
}

int getMaxconnectValue(TestCommand *commands, int maxCommands)
{
    int i = 0;
    while (i < maxCommands) {
        if (0 == commands[i].d_commandCode) {
            return i > 0? commands[i - 1].d_expNumChannels : 0;
        }
        ++i;
    }
    return 0;
}

//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;          globalVerbose = verbose;
    int veryVerbose = argc > 3;      globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4;  globalVeryVeryVerbose = veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bcema_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    ASSERT(0 == bteso_SocketImpUtil::startup());
    switch (test) {
        case 0:
        case 9: {
#ifndef BSLS_PLATFORM__OS_LINUX
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE 1
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE 1
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;
            {
                // Arguments provided to the test driver are
                // <case#> <numConnections> <portNumber> <host>
                enum {
                    DEFAULT_PORT_NUMBER = 1234,
                    DEFAULT_NUM_CONNECTIONS =  10,
                   DEFAULT_NUM_MESSAGES = 10000
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
                    numConnections = DEFAULT_NUM_CONNECTIONS;
                }

                if (argc > 3) {
                    portNumber = atoi(argv[3]);
                    if (portNumber < 0) {
                        portNumber = -portNumber;
                        veryVerbose = 0; globalVeryVerbose = 0;
                    }
                }
                else {
                    portNumber = DEFAULT_PORT_NUMBER;
                }

                if (argc > 4) {
                    hostname = argv[4];
                    veryVeryVerbose = 0;
                    globalVeryVeryVerbose = veryVeryVerbose;
                }
                else {
                    hostname = "127.0.0.1";
                }

                bteso_TcpTimerEventManager::Hint hint
                     = bteso_TcpTimerEventManager::BTESO_NO_HINT;
                if (argc > 6) {
                     hint =
                     bteso_TcpTimerEventManager::BTESO_INFREQUENT_REGISTRATION;
                }
                if (verbose) {
                    P(numConnections);
                    P(hostname);
                    P(portNumber);
                    P(hint);
                }

                bslma_TestAllocator   testAllocator(veryVeryVerbose);
                testAllocator.setNoAbort(1);

                bteso_InetStreamSocketFactory<bteso_IPv4Address>
                                                      factory(&testAllocator);

                bteso_IPv4Address serverAddress (hostname, portNumber);

                bteso_TcpTimerEventManager     manager(hint, &testAllocator);
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
#endif
        } break;
        case 8:{
#ifndef BSLS_PLATFORM__OS_LINUX
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE 2
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE 2
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;
            {
                // Arguments provided to the test driver are
                // <case#> <numConnections> <portNumber> <host> <numMessages>
                enum {
                    DEFAULT_PORT_NUMBER = 1635,
                    DEFAULT_NUM_CONNECTIONS =  10,
                    DEFAULT_NUM_MESSAGES = 10000
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
                    numConnections = DEFAULT_NUM_CONNECTIONS;
                }

                if (argc > 3) {
                    portNumber = atoi(argv[3]);
                    if (portNumber < 0) {
                        portNumber = -portNumber;
                        veryVerbose = 0; globalVeryVerbose = 0;
                    }
                }
                else {
                    portNumber = DEFAULT_PORT_NUMBER;
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
                    numMessages = DEFAULT_NUM_MESSAGES;
                }
                bteso_TcpTimerEventManager::Hint hint
                     = bteso_TcpTimerEventManager::BTESO_NO_HINT;
                if (argc > 6) {
                    hint =
                     bteso_TcpTimerEventManager::BTESO_INFREQUENT_REGISTRATION;
                }
                if (verbose) {
                    P(numConnections);
                    P(hostname);
                    P(portNumber);
                    P(numMessages);
                    P(hint);
                }

                bslma_TestAllocator   testAllocator;
                testAllocator.setNoAbort(1);

                bteso_InetStreamSocketFactory<bteso_IPv4Address>
                    factory(&testAllocator);
                bteso_TcpTimerEventManager     manager(hint, &testAllocator);
                my_EchoClient client(&factory, &manager, numConnections,
                                     numMessages,
                                     &testAllocator);

                bteso_IPv4Address serverAddress (hostname, portNumber);

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
#endif
        } break;
        case 7: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
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
                bteso_IPv4Address serverAddress, localAddress;
                serverAddress.setIpAddress(hostName);
                serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                bteso_StreamSocket<bteso_IPv4Address> *d_serverSocket_p =
                                              factory.allocate();

                ASSERT(0 != d_serverSocket_p);
                int ret = d_serverSocket_p->setOption(
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                       1);
                ASSERT(0 == ret);
                ASSERT(0 == d_serverSocket_p->bind(serverAddress));

                ASSERT(0 == d_serverSocket_p->localAddress(&localAddress));

                const int NUM_CONNECTIONS = 10;
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                 connList(&testAllocator);
                ConnectionInfo connectInfo = { &factory,
                                               d_serverSocket_p,
                                               DEFAULT_EQUEUE_SIZE,
                                               &connList,
                                               NUM_CONNECTIONS
                                             };

                bcemt_ThreadUtil::Handle threadHandle;
                bcemt_Attribute attributes;
                bcemt_ThreadUtil::create(&threadHandle, attributes,
                                     threadToAcceptConnection, &connectInfo);

                int numConnections = 0;
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);
                {
                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj connector(&factory, &manager, &testAllocator);
                    connector.setPeer(localAddress);

                    int validChannel = 1, expStatus = 0, cancelFlag = 0;

                    bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                            bdef_BindUtil::bind( &connectCb
                                               , _1, _2
                                               , &connector
                                               , &numConnections
                                               , validChannel
                                               , expStatus
                                               , cancelFlag));

                    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
                        connector.allocate(cb);
                        manager.dispatch(0);
                        ASSERT(i + 1 == connector.numChannels());
                    }
                }
                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connList.size());
                }
                bcemt_ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(d_serverSocket_p);
            }
        } break;
        case 6: {
// TBD FIX ME
#ifndef BSLS_PLATFORM__OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'invalidate' method:
            //
            // Plan:
            //
            // Testing
            //   int invalidate();
            // ----------------------------------------------------------------
            enum {
                MAX_COMMANDS = 20
            };

            if (verbose) {
                QT("Testing 'invalidate' method");
                QT("===========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column
                // in the table specifies which item.
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue

                TestCommand commands[][MAX_COMMANDS] =
                #ifdef BSLS_PLATFORM__OS_SOLARIS
                  // On Solaris, non-blocking connects to the loopback succeed
                  // right away so we do not need a dispatch call.  This is
                  // absolutely not guaranteed by POSIX though.
                    // A    B    C   D   E   F   G   H   I   J   K     L
                    // -    -    -   -   -   -   -   -   -   -   -     -
                {
                  { // There's no channel established before calling
                    // invalidate().
                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  0,  0,   VALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                  { // There's one channel established before calling
                    // invalidate().
                    { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                };
                #else
                {
                    // Because there are many items listed in the table for
                    // each test command, the following document specify which
                    // column in the table specifies which item.
                    // Column     Item               Column       Item
                    // ------     ----               ------       ----
                    //   A     d_lineNum                G     d_expStatus
                    //   B     d_commandCode            H     d_expNumEvents
                    //   C     d_channelType            I     d_expNumTimers
                    //   D     d_timedRequestFlag       J     d_expNumChannels
                    //   E     d_timeoutFlag            K     d_cancelFlag
                    //   F     d_validChannel           L     d_returnValue

                    // A    B    C   D   E   F   G   H   I   J   K    L
                    // -    -    -   -   -   -   -   -   -   -   -    -
                  { // There's no channel established before calling
                    // invalidate().
                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                  { // A request should be valid if it's submitted before
                    // calling invalidate().
                    { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  0,  0,   VALID },
                    { L_,  'I',  0,  0,  0,  0,  0,  1,  0,  0,  0,   VALID },
                    { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                  { // One channel is established before calling
                    // invalidate().
                    { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  0,  0,   VALID },
                    { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'I',  0,  0,  0,  0,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  0,  0,  1,  0, INVALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                  { // One channel is established and another request has
                    // been submitted before calling invalidate().
                    { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  0,  0,   VALID },
                    { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  1,  0,   VALID },
                    { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  1,  0,   VALID },
                    { L_,  'I',  0,  0,  0,  0,  0,  1,  0,  1,  0, INVALID },
                    { L_,  'R',  1,  1,  0,  0,  0,  1,  0,  1,  0, INVALID },
                    { L_,  'R',  1,  0,  0,  0,  0,  1,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  0,  0,  0,  0,  1,  0,  1,  0, INVALID },
                    { L_,  'R',  0,  1,  0,  0,  0,  1,  0,  1,  0, INVALID },
                    { L_,  'D',  0,  0,  0,  0,  0,  0,  0,  2,  0,   VALID },
                    { L_,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0, INVALID },
                  },
                };
                #endif

                const int NUM_COMMANDS_SET = sizeof commands
                                           / sizeof *commands;

                for (int i = 0; i < NUM_COMMANDS_SET; ++i) {
                    bcemt_ThreadUtil::Handle threadHandle;
                    bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();

                    int maxConnections = 0, numConnections = 0;
                    maxConnections = getMaxconnectValue(commands[i],
                                                        MAX_COMMANDS);
                    bteso_IPv4Address serverAddress;
                    bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                    connList(&testAllocator);

                    LOOP_ASSERT(i, 0 == createServerThread(&threadHandle,
                                              &factory, serverSocket,
                                              &connList, maxConnections,
                                              &serverAddress));
                    // The client is waiting for the server.
                    bcemt_ThreadUtil::microSleep(SLEEP_TIME);

                    bteso_TcpTimerEventManager manager(&testAllocator);

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
                            P_T(i);   P_(j);
                            P_T(manager.numEvents());
                            P_T(manager.numTimers());
                            PT(connector.numChannels());
                        }
                        ++j;
                    }
                    int length = connList.size();
                    if (veryVerbose) {
                        QT("The total number of channels established:");
                        PT(connector.numChannels());
                    }
                    for (int k = 0; k < length; ++k) {
                        factory.deallocate(connList[k]);
                    }
                    factory.deallocate(serverSocket);
                }
            }
#endif
        } break;
        case 5: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM__OS_AIX) && (!defined(BSLS_PLATFORM__OS_SOLARIS)   \
                                           || BSLS_PLATFORM__OS_VER_MAJOR < 10)
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
                // test command, the following document specify which column
                // in the table specifies which item.
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue

                TestCommand commands[] =
                #ifdef BSLS_PLATFORM__OS_SOLARIS
                  // On Solaris, non-blocking connects to the loopback succeed
                  // right away so we do not need a dispatch call.  This is
                  // absolutely not guaranteed by POSIX though.
                  // A    B    C   D   E   F   G   H   I   J   K   L
                  // -    -    -   -   -   -   -   -   -   -   -   -
                {
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  1,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  2,  0, VALID },
                };
                #else // windows platform
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  // A    B    C   D   E   F    G   H   I   J   K    L
                  // -    -    -   -   -   -    -   -   -   -   -    -
                  // There's no channel established before calling cancelAll().
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  0,  0, VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  0,  0, VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  0,  0, VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  0,  0, VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  0,  0, VALID },

                  // One channel is established before calling cancelAll().
                  { L_,  'R',  0,  0,  0,  1,   0,  1,  0,  0,  0, VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  1,  0, VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  1,  0, VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  1,  0, VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  1,  0, VALID },

                  // One channel is established before calling cancelAll(),
                  // and a request is submitted before dispatch() can also be
                  // cancelled.
                  { L_,  'R',  1,  0,  0,  1,   0,  1,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  1,  0, VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  1,  0,  2,  0, VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  2,  0, VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  2,  0, VALID },
                  { L_,  'C',  0,  0,  0,  0,   0,  0,  0,  2,  0, VALID },

                  // The cancelAll() is called in the user-installed callback
                  // function.
                  { L_,  'R',  1,  0,  0,  1,   0,  1,  0,  2,  1, VALID },
                  { L_,  'R',  1,  1,  0,  0,  -1,  1,  0,  2,  0, VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  2,  0, VALID },
                  { L_,  'R',  0,  1,  0,  0,  -1,  1,  0,  2,  0, VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  3,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,   0,  1,  0,  3,  0, VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  4,  0, VALID },

                  // The cancelAll() is called in the user-installed callback
                  // function, different requests from the above test data
                  // are submitted.
                  { L_,  'R',  0,  1,  0,  1,   0,  1,  0,  4,  1, VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  4,  0, VALID },
                  { L_,  'R',  1,  0,  0,  0,  -1,  1,  0,  4,  0, VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  4,  0, VALID },
                  { L_,  'R',  0,  0,  0,  0,  -1,  1,  0,  4,  0, VALID },
                  { L_,  'D',  0,  0,  0,  0,   0,  0,  0,  5,  0, VALID },

                };
                #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bcemt_ThreadUtil::Handle threadHandle;
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                bteso_IPv4Address serverAddress;
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                  connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);

                bteso_TcpTimerEventManager manager(&testAllocator);

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
                bcemt_ThreadUtil::join(threadHandle);

                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connector.numChannels());
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
#ifndef BSLS_PLATFORM__OS_AIX
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
                // test command, the following document specify which column
                // in the table specifies which item.
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue

                TestCommand commands[] =
                #ifdef BSLS_PLATFORM__OS_SOLARIS
                  // On Solaris, non-blocking connects to the loopback succeed
                  // right away so we do not need a dispatch call.  This is
                  // absolutely not guaranteed by POSIX though.
                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                {
                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  1,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  2,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  3,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  4,  0, VALID },

                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  5,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  6,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  7,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  8,  0, VALID },
                };
                #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue

                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  0,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  2,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  3,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  4,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  5,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  6,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  7,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0,  8,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0,  9,  0, VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0, 10,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  1,  0, 11,  0, VALID },
                  { L_,  'D',  1,  0,  0,  1,  0,  0,  0, 12,  0, VALID },
                };
                #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bcemt_ThreadUtil::Handle threadHandle;
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                bteso_IPv4Address serverAddress;
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                   connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);

                bteso_TcpTimerEventManager manager(&testAllocator);

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
                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connector.numChannels());
                }
                bcemt_ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 3: {
// TBD FIX ME
#ifndef BSLS_PLATFORM__OS_AIX
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
                // test command, the following document specify which column
                // in the table specifies which item.
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel

                TestCommand commands[] =
                #ifdef BSLS_PLATFORM__OS_SOLARIS
                  // On Solaris, non-blocking connects to the loopback succeed
                  // right away so we do not need a dispatch call.  This is
                  // absolutely not guaranteed by POSIX though.
                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                {
                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  2,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  3,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  4,  0, VALID },

                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  5,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  6,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  7,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  8,  0, VALID },

                };
                #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannel
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue

                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  0,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  2,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  4,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  6,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  8,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 10,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 11,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, VALID },
                };
                #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bcemt_ThreadUtil::Handle threadHandle;
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                bteso_IPv4Address serverAddress;
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                    connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);

                bteso_TcpTimerEventManager manager(&testAllocator);

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
                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connector.numChannels());
                }
                bcemt_ThreadUtil::join(threadHandle);

                for (int j = 0; j < length; ++j) {
                    factory.deallocate(connList[j]);
                }
                factory.deallocate(serverSocket);
            }
#endif
        } break;
        case 2: {
// TBD FIX ME
#ifndef BSLS_PLATFORM__OS_AIX
            // ----------------------------------------------------------------
            // TESTING 'ACCESSOR' methods:
            //
            // Plan:
            //
            // Testing
            //   const bteso_IPv4Address& peer() const;
            //   int isInvalid() const;
            //   int numChannels() const;
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'ACCESSOR' methods");
                QT("==========================");
            }

            {
                // Because there are many items listed in the table for each
                // test command, the following document specify which column
                // in the table specifies which item.
                // Column      Item                Column       Item
                // ------      ----                ------       ----
                //   A      d_lineNum                G       d_expStatus
                //   B      d_commandCode            H       d_expNumEvents
                //   C      d_channelType            I       d_expNumTimers
                //   D      d_timedRequestFlag       J       d_expNumChannels
                //   E      d_timeoutFlag            K       d_cancelFlag
                //   F      d_validChannel           L       d_returnValue

                TestCommand commands[] =
                #ifdef BSLS_PLATFORM__OS_SOLARIS
                  // On Solaris, non-blocking connects to the loopback succeed
                  // right away so we do not need a dispatch call.  This is
                  // absolutely not guaranteed by POSIX though.
                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                {
                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  0,  0,  2,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  3,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  0,  0,  4,  0, VALID },

                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  5,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  0,  0,  6,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  7,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  0,  0,  8,  0, VALID },
                };
                #else
                {
                  // Because there are many items listed in the table for each
                  // test command, the following document specify which column
                  // in the table specifies which item.
                  // Column      Item                Column       Item
                  // ------      ----                ------       ----
                  //   A      d_lineNum                G       d_expStatus
                  //   B      d_commandCode            H       d_expNumEvents
                  //   C      d_channelType            I       d_expNumTimers
                  //   D      d_timedRequestFlag       J       d_expNumChannels
                  //   E      d_timeoutFlag            K       d_cancelFlag
                  //   F      d_validChannel           L       d_returnValue
                  // A    B    C   D   E   F   G   H   I   J   K    L
                  // -    -    -   -   -   -   -   -   -   -   -    -
                  // Submit one request to establish one connection.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  0,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  1,  0, VALID },

                  // Submit two same requests to establish two connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  1,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  2,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  3,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'R',  0,  1,  0,  1,  0,  1,  0,  3,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  4,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  5,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'R',  1,  0,  0,  1,  0,  1,  0,  5,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  6,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  7,  0, VALID },

                  // Submit two different requests to establish two
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  7,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0,  8,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0,  9,  0, VALID },

                  // Submit three different requests to establish three
                  // connections.
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  1,  1,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'R',  0,  0,  0,  1,  0,  1,  0,  9,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 10,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  1,  0, 11,  0, VALID },
                  { L_,  'D',  0,  0,  0,  1,  0,  0,  0, 12,  0, VALID },
                };
                #endif

                const int NUM_COMMANDS = sizeof commands / sizeof *commands;

                bcemt_ThreadUtil::Handle threadHandle;
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                int maxConnections = 0, numConnections = 0;
                bteso_IPv4Address serverAddress;
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                               connList(&testAllocator);
                maxConnections = commands[NUM_COMMANDS - 1].d_expNumChannels;

                ASSERT(0 == createServerThread(&threadHandle, &factory,
                                              serverSocket, &connList,
                                              maxConnections, &serverAddress));
                // The client is waiting for the server.
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);

                bteso_TcpTimerEventManager manager(&testAllocator);

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
                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connector.numChannels());
                }
                bcemt_ThreadUtil::join(threadHandle);
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
                bcemt_ThreadUtil::Handle threadHandle;
                bcemt_Attribute attributes;
                bteso_IPv4Address serverAddress, localAddress;
                serverAddress.setIpAddress(hostName);
                serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                bteso_StreamSocket<bteso_IPv4Address> *d_serverSocket_p =
                                              factory.allocate();

                ASSERT(0 != d_serverSocket_p);
                int ret = d_serverSocket_p->setOption(
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                       1);
                ASSERT(0 == ret);
                ASSERT(0 == d_serverSocket_p->bind(serverAddress));

                ASSERT(0 == d_serverSocket_p->localAddress(&localAddress));

                enum { NUM_CONNECTIONS = 10 };
                bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                   connList(&testAllocator);
                ConnectionInfo connectInfo = { &factory,
                                               d_serverSocket_p,
                                               DEFAULT_EQUEUE_SIZE,
                                               &connList,
                                               NUM_CONNECTIONS
                                             };

                ret = bcemt_ThreadUtil::create(&threadHandle, attributes,
                                     threadToAcceptConnection, &connectInfo);
                ASSERT(0 == ret);

                int numConnections = 0;
                bcemt_ThreadUtil::microSleep(SLEEP_TIME * 10);
                {
                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj connector(&factory, &manager, &testAllocator);
                    connector.setPeer(localAddress);

                    int validChannel = 1, expStatus = 0, cancelFlag = 0;

                    bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                            bdef_BindUtil::bind( &connectCb
                                               , _1, _2
                                               , &connector
                                               , &numConnections
                                               , validChannel
                                               , expStatus
                                               , cancelFlag));

                    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
                        int ret = connector.allocate(cb);
                        LOOP_ASSERT(i, 0 == ret);
                        ret = manager.dispatch(0);

                        ASSERT(i + 1 == connector.numChannels());
                    }
                }
                int length = connList.size();
                if (veryVerbose) {
                    QT("The total number of channels established:");
                    PT(connList.size());
                }
                bcemt_ThreadUtil::join(threadHandle);

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
    ASSERT(0 == bteso_SocketImpUtil::cleanup());
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
