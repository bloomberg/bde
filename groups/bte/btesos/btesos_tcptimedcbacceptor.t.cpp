// btesos_tcptimedcbacceptor.t.cpp       -*-C++-*-
#include <btesos_tcptimedcbacceptor.h>
#include <btesos_tcptimedcbchannel.h>
#include <btesos_tcpcbchannel.h>
#include <bteso_ipv4address.h>
#include <bteso_tcptimereventmanager.h>
#include <bteso_inetstreamsocketfactory.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>

#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>
#include <bdex_byteoutstream.h>
#include <bdex_byteinstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_c_stdio.h>
#include <bsl_vector.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdef_PlaceHolders;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
// This component under test consists of a set of member functions that are
// used to provide an implementation with a blocking mechanism that allocates
// and deallocates blocking channels having the optional timeout capability.
// The basic plan for the testing is to ensure that all methods in this
// component work as expected.  Besides, because member functions in this
// component are strongly coupled, we also have to make sure that invocations
// on other member functions will not affect a member function being tested,
// that is, the function under test should still work fine.  By using the
// table-driven strategy, different test data will be adopted to "probe" the
// function being tested to address all concerns about it.
// ----------------------------------------------------------------------------
// [ 2]  int allocate();
// [ 2]  const bteso_IPv4Address& address() const;
// [ 2]  int isInvalid() const;
// [ 2]  int numChannels() const;
// [ 3]  int allocate();
// [ 4]  int allocateTimed();
// [ 5]  int cancelAll();
// [ 6]  int close();
// [ 7]  int invalidate();
//-----------------------------------------------------------------------------
// [ 1]  BREATHING TEST
// [ 8]  USAGE EXAMPLE
// [ 9]  USAGE EXAMPLE
// [] USAGE EXAMPLE
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

//============================================================================
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

static int verbose;
static int veryVerbose;

typedef btesos_TcpTimedCbAcceptor  Obj;

struct ConnectionInfo {
     bteso_StreamSocketFactory<bteso_IPv4Address> *d_factory_p;
     bteso_IPv4Address                            *d_server;
     int                                           d_numConnections;
     int                                           d_sleepTime;
  //  bcemt_Mutex                                  *d_mutex_p;
  //  bcemt_Condition    *d_cv_p;
};

enum {
    DEFAULT_PORT_NUMBER     = 0,
    DEFAULT_NUM_CONNECTIONS = 10,
    DEFAULT_EQUEUE_SIZE     = 5,
    DEFAULT_SLEEP_TIME      = 100000
};

//=============================================================================
//                      USAGE EXAMPLE
//-----------------------------------------------------------------------------
///  Usage example

class my_EchoServer {
    enum {
        READ_SIZE = 200,
        DEFAULT_PORT_NUMBER = 1234,   // As specified by RFC
        QUEUE_SIZE = 16
    };
    btesos_TcpTimedCbAcceptor   d_allocator;
    bdet_TimeInterval           d_acceptTimeout;
    bdet_TimeInterval           d_readTimeout;
    bdet_TimeInterval           d_writeTimeout;

    bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                d_allocateFunctor;

    bslma_Allocator            *d_allocator_p;
    char                        d_buffer[READ_SIZE];

    void allocateCb(btesc_TimedCbChannel *channel, int status);
         // Invoked by the socket event manager when a connection is
         // accepted.

    void bufferedReadCb(const char *buffer, int status, int asyncStatus,
                        btesc_TimedCbChannel *channel);

    void readCb(int status, int asyncStatus,
                btesc_TimedCbChannel *channel);

    void writeCb(int status, int asyncStatus, btesc_TimedCbChannel *channel,
                 int numBytes);
  private:
    my_EchoServer(const my_EchoServer&);    // Not implemented.
    my_EchoServer&
        operator=(const my_EchoServer&);    // Not implemented.
  public:
    // CREATORS
    my_EchoServer(bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                  bteso_TimerEventManager                     *manager,
                  bslma_Allocator                            *basicAllocator);
    ~my_EchoServer();

    //  MANIPULATORS
    int open(int portNumber = DEFAULT_PORT_NUMBER);
    int close();
};

// CREATORS
my_EchoServer::my_EchoServer(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        bteso_TimerEventManager                      *manager,
        bslma_Allocator                              *basicAllocator)
: d_allocator(factory, manager, basicAllocator)
, d_acceptTimeout(120, 0)
, d_readTimeout(5, 0)
, d_writeTimeout(5.0)
, d_allocator_p(basicAllocator)
{
    ASSERT(factory);
    ASSERT(manager);
    d_allocateFunctor
        = bdef_Function<void (*)(btesc_TimedCbChannel*, int)>(
                bdef_MemFnUtil::memFn(&my_EchoServer::allocateCb, this)
              , basicAllocator);
}

my_EchoServer::~my_EchoServer() {

}

// MANIPULATORS
void my_EchoServer::allocateCb(btesc_TimedCbChannel *channel, int status) {
    if (channel) {
        // Accepted a connection.  Issue a read raw request.
        bdef_Function<void (*)(int, int)> callback(
                bdef_BindUtil::bindA(
                        d_allocator_p
                      , &my_EchoServer::readCb
                      , this
                      , _1, _2
                      , channel));

        if (channel->timedReadRaw(d_buffer, READ_SIZE,
                bdetu_SystemTime::now() + d_readTimeout, callback))
        {
            cout << "Failed to enqueue buffered read request." << endl;
            d_allocator.deallocate(channel);
        }
        if (d_allocator.timedAllocateTimed(d_allocateFunctor,
                bdetu_SystemTime::now() + d_acceptTimeout))
        {
            d_allocator.close();
        }
        return;
    }
    ASSERT(0 >= status);    // Interrupts are not enabled.
    if (0 == status) {
        cout << "Timed out accepting a connection." << endl;
        // Re-register the functor
        if (d_allocator.timedAllocateTimed(d_allocateFunctor,
                bdetu_SystemTime::now() + d_acceptTimeout))
        {
            d_allocator.close();
        }
    }
    else if (-1 == status) {
        cout << "Allocation request was dequeued." << endl;
    }
    else {
        // Hard-error accepting a connection, invalidate the allocator.
        cout << "Hard error while accepting a connection: " << status
             << endl;
        d_allocator.invalidate();
    }
}

void my_EchoServer::bufferedReadCb(const char *buffer, int status,
                                   int asyncStatus,
                                   btesc_TimedCbChannel *channel)
{
    cout << "my_EchoServer::bufferedReadCb: "
         << " read " << status << " bytes." << endl;
    ASSERT(channel);
    if (0 < status) {
        bdef_Function<void (*)(int, int)> callback(
                bdef_BindUtil::bindA(
                        d_allocator_p
                      , &my_EchoServer::writeCb
                      , this
                      , _1, _2
                      , channel
                      , status));
        if (channel->timedBufferedWrite(buffer, status,
                bdetu_SystemTime::now() + d_writeTimeout, callback))
        {
            cout << "Failed to enqueue write request" << endl;
            d_allocator.deallocate(channel);
            return;
        }
        // Re-register read request
        bdef_Function<void (*)(const char *, int, int)> readCallback(
                bdef_BindUtil::bindA(
                    d_allocator_p
                  , &my_EchoServer::bufferedReadCb
                  , this
                  , _1, _2, _3
                  , channel));
        if (channel->timedBufferedRead(READ_SIZE,
                bdetu_SystemTime::now() + d_readTimeout, readCallback)) {
            cout << "Failed to enqueue read request." << endl;
            d_allocator.deallocate(channel);
        }
    }
    else if (0 == status) {
        if (0 > asyncStatus) {
            cout << "Callback dequeued" << endl;
        }
        else {
            cout << "Timed out on read" << endl;
            d_allocator.deallocate(channel);
        }
    }
    else {
        // Either time out or an error on the channel
        cout << "Failed to read data." << endl;
        d_allocator.deallocate(channel);
    }
}

void my_EchoServer::readCb(int status, int asyncStatus,
                           btesc_TimedCbChannel *channel)
{
    cout << "my_EchoServer::readCb: "
         << " read " << status << " bytes." << endl;
    ASSERT(channel);
    if (0 < status) {
        bdef_Function<void (*)(int, int)> callback(
                bdef_BindUtil::bindA(
                        d_allocator_p
                      , &my_EchoServer::writeCb
                      , this
                      , _1, _2
                      , channel
                      , status));
        if (channel->timedBufferedWrite(d_buffer, status,
                bdetu_SystemTime::now() + d_writeTimeout, callback))
        {
            cout << "Failed to enqueue write request" << endl;
            d_allocator.deallocate(channel);
            return;
        }
        // Re-register read request
        bdef_Function<void (*)(int, int)> readCallback(
                bdef_BindUtil::bindA(
                        d_allocator_p
                      , &my_EchoServer::readCb
                      , this
                      , _1, _2
                      , channel));
        if (channel->timedReadRaw(d_buffer, READ_SIZE,
                bdetu_SystemTime::now() + d_readTimeout, readCallback)) {
            cout << "Failed to enqueue read request." << endl;
            d_allocator.deallocate(channel);
        }
    }
    else if (0 == status) {
        if (0 > asyncStatus) {
            cout << "Callback dequeued" << endl;
        }
        else {
            cout << "Timed out on read" << endl;
            d_allocator.deallocate(channel);
        }
    }
    else {
        // Either time out or an error on the channel
        cout << "Failed to read data." << endl;
        d_allocator.deallocate(channel);
    }
}

void my_EchoServer::writeCb(int status, int asyncStatus,
                            btesc_TimedCbChannel *channel, int numBytes)
{
    if (status != numBytes) {
        cout << "Failed to send data." << endl;
        channel->invalidate();
        d_allocator.deallocate(channel);
    }
}

int my_EchoServer::open(int portNumber) {
    bteso_IPv4Address serverAddress;
    serverAddress.setPortNumber(portNumber);
    int s = d_allocator.open(serverAddress, QUEUE_SIZE);
    if (s) {
        cout << "Failed to open listening port." << endl;
        return s;
    }
    cout << "server's socket: " << d_allocator.address() << endl;
    ASSERT(0 == d_allocator.isInvalid());
    s = d_allocator.timedAllocateTimed(d_allocateFunctor,
            bdetu_SystemTime::now() + d_acceptTimeout);
    if (s) {
        cout << "Can't enqueue an allocation request." << endl;
        ASSERT(d_allocator.isInvalid());
        d_allocator.close();
        return s;
    }
    return 0;
}

int my_EchoServer::close() {
    return d_allocator.close();
}

//=============================================================================
//                      USAGE EXAMPLE IN THE PROTOCOL HEADER FILE
//-----------------------------------------------------------------------------
// Usage example 3: Tick Reporter
// -------------------------------
class my_Tick {
    char   d_name[5];
    double d_bestBid;
    double d_bestOffer;
  public:
    my_Tick() { }
    my_Tick(const char *ticker);
    my_Tick(const char *ticker, double bestBid, double bestOffer);

    static int maxSupportedBdexVersion() { return 1; }
    static int maxSupportedVersion() { return maxSupportedBdexVersion(); }

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this 'my_Tick' to the specified output 'stream' formatted
        // in the specified 'version' and return a reference to the modifiable
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
        // specified 'version' is not supported, 'stream' is marked invalid,
        // but this object is unaltered.  Note that no version is read from
        // 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    void print(bsl::ostream& stream) const;
};

my_Tick::my_Tick(const char *ticker)
: d_bestBid(0)
, d_bestOffer(0)
{
    #ifdef BSLS_PLATFORM__OS_UNIX
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
    #ifdef BSLS_PLATFORM__OS_UNIX
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

static void myPrintTick(bsl::ostream& stream, const char *buffer, int len)
    // Print the value of the specified 'buffer' interpreted as a
    // 'bdex' byte-stream representation of a 'my_Tick' value, to the
    // specified 'stream' or report an error to 'stream' if 'buffer' is
    // determined *not* to hold an encoding of a valid 'my_Tick' value.
{
    my_Tick tick;
    bdex_ByteInStream input(buffer, len);
    bdex_InStreamFunctions::streamIn(input,
                                     tick,
                                     my_Tick::maxSupportedBdexVersion());
    stream << tick;
}

class my_TickReporter {
    // This class implements a server that accepts connections, extracts
    // from each connection a single 'my_Tick' value, and reports that
    // value to a console stream; both the acceptor and console stream
    // are supplied at construction.

    btesc_TimedCbChannelAllocator *d_acceptor_p; // incoming connections
    bteso_TcpTimerEventManager    *d_eventManager_p; // Only needed for
                                                     // timeCb()'s use,
                                                     // could be erased
                                                     // otherwise.
    bsl::ostream&                  d_console;    // where to put tick info

    btesc_TimedCbChannelAllocator::TimedCallback
                                   d_allocFunctor;  // reused

  private:
    void acceptCb(btesc_TimedCbChannel     *clientChannel,
                  int                       status,
                  const bdet_TimeInterval&  timeout);
        // Called when a new client channel has been accepted.
        // ...

    void readCb(const char           *buffer,
                int                   status,
                int                   asyncStatus,
                btesc_TimedCbChannel *clientChannel);
        // Called when a 'my_Tick' value has been read from the channel.
        // ...

    void timeCb(int                  lastNumTicks,
                int                 *curNumTicks,
                bdet_TimeInterval    lastTime);
        // To calculate the tick send/receive rate (Ticks/second).

  private:
    my_TickReporter(const my_TickReporter&);             // not impl.
    my_TickReporter& operator=(const my_TickReporter&);  // not impl.

  public:
    my_TickReporter(bsl::ostream&                  console,
                    btesc_TimedCbChannelAllocator *acceptor,
                    bteso_TcpTimerEventManager    *d_eventManager_p);
        // Create a non-blocking tick-reporter using the specified
        // 'acceptor' to establish incoming client connections, each
        // transmitting a single 'my_Tick' value; write these values
        // to the specified 'console' stream.  If the acceptor is idle
        // for more than five minutes, print a message to the 'console'
        // stream supplied at construction and continue.  To guard
        // against malicious clients, a connection that does not produce
        // a tick value within one minute will be summarily dropped.

    ~my_TickReporter();
        // Destroy this server object.
};

const double ACCEPT_TIME_LIMIT = 300;               // 5 minutes
const double   READ_TIME_LIMIT =  60;               // 1 minutes

static int calculateMyTickMessageSize()
    // Calculate and return the number of bytes encoding of a
    // 'my_Tick' value (called just once, see below).
{
    double bid = 0, offer = 0;
    my_Tick dummy("dummy", bid, offer);

    //typedef bdex_ByteOutSTREAM bdex_ByteOutStream;
    bdex_ByteOutStream bos;
    bdex_OutStreamFunctions::streamOut(bos,
                                       dummy,
                                       my_Tick::maxSupportedBdexVersion());
    return bos.length();
}

static int myTickMessageSize()
    // Return the number of bytes in a 'bdex' byte-stream encoding
    // of a 'my_Tick' value without creating a runtime-initialized
    // file-scope static variable (which is link-order dependent).
{
    static const int MESSAGE_SIZE = calculateMyTickMessageSize();
    return MESSAGE_SIZE;
}

void my_TickReporter::acceptCb(btesc_TimedCbChannel     *clientChannel,
                               int                       status,
                               const bdet_TimeInterval&  timeout)
{
    bdet_TimeInterval nextTimeout(timeout);

    if (clientChannel) {     // Successfully created a connection.

        const int               numBytes = ::myTickMessageSize();

        const bdet_TimeInterval now      = bdetu_SystemTime::now();

        // Create one-time (buffered) read functor holding 'clientChannel'.
        btesc_TimedCbChannel::BufferedReadCallback readFunctor(
                bdef_BindUtil::bind(
                        &my_TickReporter::readCb
                      , this
                      , _1, _2, _3
                      , clientChannel));

        // Install read callback (timeout, but no raw or async interrupt).

        if (clientChannel->timedBufferedRead(numBytes,
                                             now + READ_TIME_LIMIT,
                                             readFunctor)) {
            d_console << "Error: Unable even to register a read operation"
                         " on this channel." << bsl::endl;
            d_acceptor_p->deallocate(clientChannel);
        }
        nextTimeout += ACCEPT_TIME_LIMIT;
    }
    else if (0 == status) {  // Interrupted due to timeout event.
        d_console << "Acceptor timed out, continuing..." << bsl::endl;
        nextTimeout += ACCEPT_TIME_LIMIT;
    }
    else if (status > 0) {   // Interrupted by unspecified event.
        ASSERT(0); // Impossible, "async interrupts" were not authorized.
    }
    else {                   // Allocation operation is unable to succeed.
        ASSERT(status < 0);
        d_console << "Error: The channel allocator is not working now."
                  << bsl::endl;

        // Note that attempting to re-register an allocate operation below
        // will fail only if the channel allocator is permanently disabled.
    }

    // In all cases, attempt to reinstall the (reusable) accept callback.

    if (d_acceptor_p->timedAllocateTimed(d_allocFunctor,
                                     nextTimeout + bdetu_SystemTime::now())) {
        d_console << "Error: unable to register accept opeation." << bsl::endl;
        // This server is hosed.
    }
}

void my_TickReporter::readCb(const char           *buffer,
                             int                   status,
                             int                   asyncStatus,
                             btesc_TimedCbChannel *clientChannel)
{
    static int curNumTicks = 0;

    const int msgSize = ::myTickMessageSize();

    enum { TIME_LEN = 15 };  // 15 seconds
    bdet_TimeInterval now = bdetu_SystemTime::now();
    static bdet_TimeInterval lastTime(now);
    const bdet_TimeInterval PERIOD(TIME_LEN, 0);

    if (msgSize == status) {  // Encoded-tick value read successfully.
        ASSERT(buffer);
        ++curNumTicks;
        if (0 == (curNumTicks % 200000)) {
            cout << curNumTicks << " ticks has been received. " << endl;
            cout << "The current time value: " << now << endl;
        }
        // Just to print one tick to verify the format.
        if (1 == curNumTicks) {
            cout << "The first tick's value: ";
            ::myPrintTick(bsl::cout, buffer, msgSize);
        }
        if (1 == curNumTicks) {
            bdef_Function<void (*)()> timerFunctor(
                    bdef_BindUtil::bind(
                        &my_TickReporter::timeCb
                      , this
                      , 0
                      , &curNumTicks
                      , now));
            d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
        }

        btesc_TimedCbChannel::BufferedReadCallback readFunctor(
                bdef_BindUtil::bind(
                        &my_TickReporter::readCb
                      , this
                      , _1, _2, _3
                      , clientChannel));
        if (clientChannel->timedBufferedRead(msgSize,
                                             now + READ_TIME_LIMIT,
                                             readFunctor)) {
            d_console << "Error: Unable even to register a read operation"
                         " on this channel." << bsl::endl;
            d_acceptor_p->deallocate(clientChannel);
        }
    }
    else if (0 <= status) {   // Tick message was interrupted.

        ASSERT(buffer); // Data in buffer is available for inspection
                        // (but remains in the channel's buffer).

        // Must be a TIMEOUT event since neither raw (partial) reads
        // nor (external) asynchronous interrupts were authorized.

        ASSERT(0 == asyncStatus);   // must be timeout event!

        d_console << "Error: Unable to read tick value before timing out; "
                     "read aborted."
                  << " status = " << status << "; asyncStatus = "
                  << asyncStatus << bsl::endl;
    }
    else { // Tick-message read failed.
        if (-1 == status) {
            d_console << "Error: Unable to read tick value from channel: "
                         " The connection is closed by the client."
                      << bsl::endl;
        }
        else {
            d_console << "Unkown Error: Unable to read tick from channel. "
                      << bsl::endl;
        }
    }
}

void my_TickReporter::timeCb(int                lastNumTicks,
                              int               *curNumTicks,
                              bdet_TimeInterval  lastTime)
{
    int numTicks = *curNumTicks - lastNumTicks;
    enum { TIME_LEN = 15 };  // 15 seconds
    bdet_TimeInterval now = bdetu_SystemTime::now();

    bdet_TimeInterval timePeriod = now - lastTime;
    double numSeconds = timePeriod.seconds() +
                            (double) timePeriod.nanoseconds() / 1000000000;
    cout << numTicks <<" ticks were sent in "
         << numSeconds << " seconds." << endl;

    cout << "The read rate is " << (int) (numTicks / numSeconds)
         << " Ticks/second." << endl << endl;

    bdef_Function<void (*)()> timerFunctor(
            bdef_BindUtil::bind(
                    &my_TickReporter::timeCb
                  , this
                  , *curNumTicks
                  , curNumTicks
                  , now));
    d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
}

my_TickReporter::my_TickReporter(bsl::ostream&                  console,
                                 btesc_TimedCbChannelAllocator *acceptor,
                                 bteso_TcpTimerEventManager    *eventManager)
: d_acceptor_p(acceptor)
, d_eventManager_p(eventManager)
, d_console(console)
{
    ASSERT(&d_console);
    ASSERT(d_acceptor_p);

    // Attempt to install the first accept callback.

    bdet_TimeInterval timeout = bdetu_SystemTime::now()
        + ACCEPT_TIME_LIMIT;

    // load reusable allocate functor
    d_allocFunctor
        = bdef_BindUtil::bind(
                &my_TickReporter::acceptCb
              , this
              , _1, _2
              , timeout);

    if (d_acceptor_p->timedAllocateTimed(d_allocFunctor, timeout)) {
        d_console << "Error: Unable to install accept operation." << bsl::endl;
        // This server is hosed.
    }
}

my_TickReporter::~my_TickReporter()
{
    ASSERT(&d_console);
    ASSERT(d_acceptor_p);
}

static void acceptCb(btesc_CbChannel           *channel,
                     int                        status,
                     btesos_TcpTimedCbAcceptor *acceptor,
                     int                       *numConnections,
                     int                        validChannel,
                     int                        expStatus,
                     int                        cancelFlag,
                     int                        closeFlag)
    // Verify the result of an "ACCEPT" request by comparing against the
    // expected values:  If the specified 'channelFlag' is nonzero, a new
    // channel should be established; the return 'status' should be the same
    // as the specified 'expStatus'.  If the specified 'cancelFlag' is
    // nonzero, invoke the 'cancelAll()' on the specified 'acceptor'
    // to help the test.  Similarly, if the specified 'closeFlag' is nonzero,
    // invoke the 'close()' on the specified 'acceptor'.
{
    if (validChannel) {
        ASSERT (channel);
    }
    else {
        ASSERT (0 == channel);
    }
    ASSERT (status == expStatus);

    if (0 == channel) {
        if (veryVerbose) {
            PT(status);
        }
    }
    else {
        ++(*numConnections);
        if (veryVerbose) {
            QT(" created a channel. ");
        }
    }
    if (closeFlag) {
        acceptor->close();
    }
    else if (cancelFlag) {
        if (veryVerbose) {
            QT("callback invoke cancelAll()");
        }
        acceptor->cancelAll();
    }
}

static void *threadToConnect(void *arg)
    // Create the specified number of client sockets, connect each client
    // socket with the server created with a 'btesos_TcpTimedCbAcceptor'
    // object.
    // The following information will be pass in through the specified 'arg':
    // the bteso_IPv4Address object of the server to be connected; the
    // number of connections to be established.
{
    ASSERT(arg);
    // Since 'bslma_TestAllocator' is not thread-safe, the helper thread
    // can't share the 'factory' (which uses a 'bslma_TestAllocator' object)
    // and so we have to instantiate another factory object for the helper
    // thread.
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;

    ConnectionInfo info = *(ConnectionInfo*) arg;
    if (verbose) {
        P_T(*(info.d_server));
        P_T(info.d_numConnections);
        PT(info.d_sleepTime);
    }
    bcemt_ThreadUtil::microSleep(info.d_sleepTime);
    // bdema_testAllocator is not thread-safe, so need to provide a new one
    // for the thread.
    bslma_TestAllocator threadTestAllocator;

    bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                          clients(&threadTestAllocator);
    int i = 0;

    while(i < info.d_numConnections) {
        bteso_StreamSocket<bteso_IPv4Address>
            *d_connectingSocket_p = factory.allocate();

        // info.d_mutex_p->unlock();

        ASSERT(0 != d_connectingSocket_p);
        int stat = d_connectingSocket_p->connect(*(info.d_server));

        if (0 == stat) {
            ++i;
            if (veryVerbose) {
                QT(" A new connection is established between clent and "
                   "server: ")
                P_T(d_connectingSocket_p->handle());
                PT(*(info.d_server));
            }
            clients.push_back(d_connectingSocket_p);
        }
        else {
            if (veryVerbose) {
                QT(" Error status: "); PT(stat);
            }
        }
    }
    int length = clients.size();

    for (int i = 0; i < length; ++i) {
        factory.deallocate(clients[i]);
    }
    if (veryVerbose) {
        QT(" clients exited. ");
    }
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2; // global variable
    veryVerbose = argc > 3; // global variable
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    ASSERT(0 == bteso_SocketImpUtil::startup());
    switch (test) { case 0:
      case -1: {
            // ----------------------------------------------------------------
            // TESTING PROTOCOL USAGE EXAMPLE
            //   The usage example provided in the protocol's component header
            //   must compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   PROTOCOL USAGE EXAMPLE
            // ----------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example: Ticker Reporter"
                          << "\n======================================"
                          << endl;
        {
            enum { DEFAULT_PORT = 5000 };

            const int portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;

            bteso_IPv4Address address;
            address.setPortNumber(portNumber);

            bteso_TcpTimerEventManager sem;    // concrete manager
            bteso_InetStreamSocketFactory<bteso_IPv4Address> sf;
            btesos_TcpTimedCbAcceptor acceptor(&sf, &sem);

            ASSERT(0 == acceptor.open(address, DEFAULT_EQUEUE_SIZE));

            if (acceptor.isInvalid()) {
                 bsl::cout << "Error: Unable to create acceptor" << bsl::endl;
                 break; // return -1;
            }

            my_TickReporter reporter(bsl::cout, &acceptor, &sem);
            if (verbose) {
                cout << "server: " << address
                     << "; begins to dispatch()....." << endl;
            }
            while (0 != sem.dispatch(0)) {
                // Do nothing.
            }
        }
      } break;
      case -2: {
            // ----------------------------------------------------------------
            // TESTING PROTOCOL USAGE EXAMPLE
            //   The usage example provided in the protocol's component header
            //   must compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'ASSERT' with
            //   'ASSERT'.
            //
            // Testing:
            //   PROTOCOL USAGE EXAMPLE
            // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example: Ticker Reporter"
                          << "\n======================================"
                          << endl;

        {
            enum { DEFAULT_PORT = 5000, LISTEN_QUEUE_SIZE = 64 };

            const int portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;

            bteso_IPv4Address address(bteso_IPv4Address::BTESO_ANY_ADDRESS,
                                      portNumber);
            bteso_TcpTimerEventManager sem;           // concrete manager
            bteso_InetStreamSocketFactory<bteso_IPv4Address> sf;
                                                      // concrete factory
            btesos_TcpTimedCbAcceptor acceptor(&sf, &sem);

            if (acceptor.open(address, LISTEN_QUEUE_SIZE)) {
                bsl::cout << "Error: Unable to listen on " << address
                          << bsl::endl;
                break; // return -1;
            }

            my_TickReporter reporter(bsl::cout, &acceptor, &sem);

            while (0 != sem.numEvents()) {
                sem.dispatch(bteso_Flag::BTESO_ASYNC_INTERRUPT);
            }
        }
      } break;
      case -3: {
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

          if (verbose) cout << "\nTesting Usage Example: Echo Server"
                            << "\n==================================" << endl;
          {
                bslma_TestAllocator testAllocator;
                testAllocator.setNoAbort(1);

                bteso_InetStreamSocketFactory<bteso_IPv4Address>
                    factory(&testAllocator);

                bteso_TcpTimerEventManager::Hint hint
                     = bteso_TcpTimerEventManager::BTESO_NO_HINT;
                if (atoi(argv[2])) {
                    hint =
                     bteso_TcpTimerEventManager::BTESO_INFREQUENT_REGISTRATION;
                }
                bteso_TcpTimerEventManager   manager(hint, &testAllocator);
                my_EchoServer echoServer(&factory, &manager, &testAllocator);

                if (echoServer.open()) {
                    cout << "Can't open listening socket." << endl;
                    break; // return -1;
                }

                for (int i = 0; i < 100; ++i) {
                    // Do it for the finite number of times.
                    int cbs = manager.dispatch(0);
                    LOOP_ASSERT(i, 0 < cbs);
                }
                echoServer.close();
                // We need another round of dispatch for cleaning things up.
                int cbs = manager.dispatch(0);
                ASSERT(0 < cbs);
                ASSERT(0 == testAllocator.numMismatches());
            }
        } break;
        case 10: {
            // ----------------------------------------------------------------
            // TESTING 'invalidate' METHOD:
            //
            // Plan:
            //
            // Testing: int invalidate();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'invalidate' method"
                              << "\n===========================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call
                    int d_newRequests;    // requests to be resubmitted after
                                          // cancel
                    int d_numEvents;      // expected number of events right
                                          // after invalidate() is called.
                } VALUES[] =
                  //line    numConnect  initReq  newsReq numEvents
                  //----    ----------  -------  ------- ---------
                {
                  {  L_,        0,         0,        3,     0     },
                  {  L_,        0,         1,        3,     2     },
                  {  L_,        0,         9,        3,     2     },
                  {  L_,        1,         1,        3,     2     },
                  {  L_,        1,         9,        3,     2     },
                  {  L_,        9,         1,        3,     2     },
                  {  L_,        9,         9,        3,     2     },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;

                    bteso_IPv4Address serverAddr(acceptor.address());
                    int totalConnections = VALUES[i].d_numConnections +
                                           VALUES[i].d_initRequests;
                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   totalConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bdet_TimeInterval timeout(50, 10000000);
                    timeout += bdetu_SystemTime::now();
                    int connections = 0, timeoutChannel = 0;

                    for (int j = 0; j < VALUES[i].d_numConnections; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocate(cb));
                        }
                        else {
                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocateTimed(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());
                        manager.dispatch(0);

                        LOOP2_ASSERT(__LINE__, connections,
                                     0 == manager.numEvents());
                    }
                    // Now expected number of channels have been established.
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    if (veryVeryVerbose) {
                        P_T(VALUES[i].d_numConnections);
                        PT(acceptor.numChannels());
                    }

                    // Create expected number of requests before call
                    // invalidate().
                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocate(cb, timeout));
                        }
                        else {
                           bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                               cb(bdef_BindUtil::bind( &acceptCb
                                                     , _1, _2
                                                     , &acceptor
                                                     , &connections
                                                     , channelFlag
                                                     , expStatus
                                                     , cancelFlag
                                                     , closeFlag));

                           LOOP2_ASSERT(i, j,
                              0 == acceptor.allocateTimed(cb));
                        }
                    }
                    acceptor.invalidate();
                    LOOP_ASSERT(i, 1 == acceptor.isInvalid());
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    LOOP_ASSERT(i, VALUES[i].d_numEvents ==
                                          manager.numEvents());

                    // We now make sure the acceptor can establish new
                    // connections for suspending requests but can not
                    // receive any new request.
                    connections = 0;
                    while (connections < VALUES[i].d_initRequests) {
                        LOOP_ASSERT(i,
                                  connections + VALUES[i].d_numConnections ==
                                                   acceptor.numChannels());

                        LOOP2_ASSERT(__LINE__, connections,
                                     2 == manager.numEvents());

                        manager.dispatch(0);

                    }
                    LOOP2_ASSERT(__LINE__, connections,
                                     0 == manager.numEvents());

                    for (int j = 0; j < VALUES[i].d_newRequests; ++j) {
                        if (!timeoutChannel) {
                            int channelFlag = 0, expStatus = -1,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, j,
                                  0 != acceptor.allocate(cb));
                            LOOP2_ASSERT(__LINE__, j,
                                  totalConnections == acceptor.numChannels());
                        }
                        else {
                            int channelFlag = 0, expStatus = -1,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, j,
                                  0 != acceptor.allocateTimed(cb));
                            LOOP2_ASSERT(__LINE__, j,
                                  totalConnections == acceptor.numChannels());
                        }

                        LOOP_ASSERT(__LINE__, totalConnections ==
                                                    acceptor.numChannels());
                        manager.dispatch(0);

                        LOOP2_ASSERT(__LINE__, j,
                                   0 == manager.numEvents());

                    }
                    // Now expected number of channels are still valid.
                    LOOP_ASSERT(i, totalConnections == acceptor.numChannels());
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 9: {
            // ----------------------------------------------------------------
            // TESTING 'close' METHOD:
            //
            // Plan:
            //
            // Testing: int close();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'close' method"
                              << "\n======================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call
                    int d_newRequests;    // requests to be resubmitted after
                                          // cancel

                } VALUES[] =
                  //line    numConnections  initRequests  newsRequest
                  //----    --------------  ------------  -----------
                {
                  {  L_,           0,             1,           3     },
                  {  L_,           0,             9,           3     },
                  {  L_,           1,             1,           3     },
                  {  L_,           1,             9,           3     },
                  {  L_,           9,             1,           3     },
                  {  L_,           9,             9,           3     },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bdet_TimeInterval timeout(50, 10000000);
                    timeout += bdetu_SystemTime::now();
                    int connections = 0, timeoutChannel = 0;
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocate(cb));
                        }
                        else {
                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocateTimed(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());
                        manager.dispatch(0);

                        LOOP2_ASSERT(__LINE__, connections,
                                     0 == manager.numEvents());

                    }
                    // Now expected number of channels have been established.
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    if (veryVeryVerbose) {
                        P_T(VALUES[i].d_numConnections);
                        PT(acceptor.numChannels());
                    }

                    // Create expected number of request before call close().
                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 0, expStatus = -1,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocate(cb, timeout));
                        }
                        else {
                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                           LOOP2_ASSERT(i, j,
                              0 == acceptor.allocateTimed(cb));
                        }
                    }
                    acceptor.close();
                    LOOP_ASSERT(i, acceptor.address() == bteso_IPv4Address());
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    LOOP_ASSERT(i, 0 == manager.numEvents());

                    // We now make sure the acceptor is invalid and
                    // can not receive any new request.
                    int newRequests = 0;
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (!timeoutChannel) {
                            int channelFlag = 0, expStatus = -2,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, newRequests,
                                  0 != acceptor.allocate(cb));
                            LOOP2_ASSERT(__LINE__, newRequests,
                                      connections == acceptor.numChannels());
                        }
                        else {
                            int channelFlag = 0, expStatus = -2,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, newRequests,
                                  0 != acceptor.allocateTimed(cb));
                            LOOP2_ASSERT(__LINE__, newRequests,
                                      connections == acceptor.numChannels());
                        }
                        ++newRequests;
                        LOOP_ASSERT(__LINE__, connections ==
                                                    acceptor.numChannels());
                        manager.dispatch(0);

                        LOOP2_ASSERT(__LINE__, newRequests,
                                   0 == manager.numEvents());

                    }
                    // Now expected number of channels have been established.
                    LOOP_ASSERT(i, connections == acceptor.numChannels());
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 8: {
            // ----------------------------------------------------------------
            // TESTING 'cancelAll' METHOD:
            //
            // Plan:
            //
            // Testing: int cancelAll();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'cancelAll' method"
                              << "\n==========================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call
                    int d_newRequests; // requests to be resubmitted after
                                          // cancel

                } VALUES[] =
                  //line    numConnections  initRequests  newRequests
                  //----    --------------  ------------  --------------
                {
                  {  L_,           0,             1,           3        },
                  {  L_,           0,             9,           3        },
                  {  L_,           1,             1,           3        },
                  {  L_,           1,             9,           3        },
                  {  L_,           9,             1,           3        },
                  {  L_,           9,             9,           3        },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;

                    bteso_IPv4Address serverAddr(acceptor.address());
                    // We will try establish another connection after the
                    // cancelAll() is called.
                    int totalConnections = VALUES[i].d_numConnections + 1;

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   totalConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bdet_TimeInterval timeout(50, 10000000);
                    timeout += bdetu_SystemTime::now();
                    int connections = 0, timeoutChannel = 0;

                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (!timeoutChannel) {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocate(cb));
                        }
                        else {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                  0 == acceptor.allocateTimed(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());
                        manager.dispatch(0);

                        LOOP2_ASSERT(__LINE__, connections,
                                     0 == manager.numEvents());

                    }
                    // Now expected number of channels have been established.
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    if (veryVeryVerbose) {
                        P_T(VALUES[i].d_numConnections);
                        PT(acceptor.numChannels());
                    }

                    // Create expected number of request before call
                    // cancelAll().
                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 0, expStatus = -1,
                               cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocate(cb, timeout));
                        }
                        else {
                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                           LOOP2_ASSERT(i, j,
                              0 == acceptor.allocateTimed(cb));
                        }
                    }
                    acceptor.cancelAll();
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    LOOP_ASSERT(i, 0 == manager.numEvents());

                    // We now make sure the acceptor is still valid and
                    // can establish new connection upon request.
                    // Besides, we also test call cancelAll() from the user
                    // callback function by set the 'closeFlag' to 1 to invoke
                    // cancelAll() inside the callback function.

                    for (int j = 0; j < VALUES[i].d_newRequests; ++j) {

                        int channelFlag = 0, expStatus = -1,
                               cancelFlag = 0, closeFlag = 0;
                        // Only make the first request invoke cancelAll()
                        // in the user callback.
                        if (0 == j) {
                            channelFlag = 1;
                            expStatus = 0;
                            cancelFlag = 1;
                        }
                        if (!timeoutChannel) {
                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocate(cb, timeout));
                        }
                        else {
                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                           LOOP2_ASSERT(i, j,
                              0 == acceptor.allocateTimed(cb));
                        }
                    }
                    LOOP_ASSERT(i, 2 == manager.numEvents());
                    connections = 0;

                    while (1 > connections) {
                        ASSERT(connections + VALUES[i].d_numConnections ==
                                                     acceptor.numChannels());
                        manager.dispatch(0);
                    }
                    LOOP2_ASSERT(__LINE__, connections,
                                 0 == manager.numEvents());

                    // Now expected number of channels have been established.
                    LOOP_ASSERT(i, VALUES[i].d_numConnections + 1 ==
                                   acceptor.numChannels());
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 7: {
            // ----------------------------------------------------------------
            // TESTING 'TimedAllocate' methods:
            //
            // Plan:
            //
            // Testing:
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting timedAllocate"
                              << "\n=====================" << endl;
            {
                enum {
                    MAX_COMMANDS = 50 // the maximum commands in one test
                };
                struct {
                    int  d_lineNum;
                    char d_commandCode;// a command to invoke a
                                       // corresponding function, e.g.,
                                       // 'D' -- invoke the event
                                       // manager's dispatch()
                                       // 'R' -- an "ACCEPT" request etc.
                    int d_channelFlag; // a request for a 'btesos_TcpCbChannel'
                                       // or 'btesos_TcpTimedCbChannel', i.e.,
                                       // 1 for a timed channel and 0 for
                                       // a 'btesos_TcpCbChannel'
                    int d_timeoutFlag; // the flag to indicate if the request
                                       // will timeout
                    int d_expNumEvents;// the number of events after the
                                       // execution of this command
                    int d_expNumTimers;// the number of timers after the
                                       // execution of this command
                    int d_expNumChannels;
                } VALUES[][MAX_COMMANDS] =
                  //line command  channelFlag timeout  Events  Timers channels
                  //---- -------  ----------- -------  ------  ------ --------
                {
                  {
                    { L_,   'R',        0,       1,       2,      1,      0  },
                    { L_,   'D',        0,       0,       0,      0,      0  },
                    { 0,     0,         0,       0,       0,      0,      0  },
                  },
                  {
                    { L_,   'R',        1,       1,       2,      1,      0  },
                    { L_,   'D',        0,       0,       0,      0,      0  },
                    { 0,     0,         0,       0,       0,      0,      0  },
                  },
                  {
                    { L_,   'R',        0,       1,       2,      1,      0  },
                    { L_,   'R',        1,       0,       2,      1,      0  },
                    { L_,   'D',        0,       0,       2,      1,      0  },
                    { L_,   'D',        0,       0,       0,      0,      1  },
                    { 0,     0,         0,       0,       0,      0,      0  },
                  },
                  {
                    { L_,   'R',        1,       1,       2,      1,      0  },
                    { L_,   'R',        1,       0,       2,      1,      0  },
                    { L_,   'D',        0,       0,       2,      1,      0  },
                    { L_,   'D',        0,       0,       0,      0,      1  },
                    { 0,     0,         0,       0,       0,      0,      0  },
                  },
                  {
                    { L_,   'R',        1,       1,       2,      1,      0  },
                    { L_,   'R',        0,       0,       2,      1,      0  },
                    { L_,   'D',        0,       0,       1,      0,      0  },
                    { L_,   'D',        0,       0,       0,      0,      1  },
                    { 0,     0,         0,       0,       0,      0,      0  },
                  }
                };
                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());
                    ConnectionInfo
                        connectInfo =
                        { &factory,
                          &serverAddr,
                          1,       // the number of channels to be established
                          DEFAULT_SLEEP_TIME,
                        };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bdet_TimeInterval timeout1(0,  10),
                                      timeout2(50, 10000000);
                    timeout1 += bdetu_SystemTime::now();
                    timeout2 += bdetu_SystemTime::now();

                    for (int j = 0; j < MAX_COMMANDS; ++j) {
                        if (!VALUES[i][j].d_commandCode) {
                            break;
                        }
                        switch (VALUES[i][j].d_commandCode) {
                        case 'R': {
                            int validChannel = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                            if (VALUES[i][j].d_channelFlag) {
                                // a 'btesos_TcpTimedCbChannel'
                                bdef_Function< void (*)(btesc_TimedCbChannel*
                                             , int)> cb;

                                if (VALUES[i][j].d_timeoutFlag) {
                                    validChannel = 0;
                                    cb = bdef_BindUtil::bind(
                                            &acceptCb,
                                            _1, _2,
                                            &acceptor, &connections,
                                            validChannel, expStatus,
                                            cancelFlag, closeFlag);
                                    LOOP2_ASSERT(VALUES[i][j].d_lineNum,
                                                 i, 0 ==
                                    acceptor.timedAllocateTimed(cb, timeout1));
                                }
                                else {
                                    cb = bdef_BindUtil::bind(
                                            &acceptCb,
                                            _1, _2,
                                            &acceptor, &connections,
                                            validChannel, expStatus,
                                            cancelFlag, closeFlag);
                                    LOOP2_ASSERT(VALUES[i][j].d_lineNum,
                                                 i, 0 ==
                                    acceptor.timedAllocateTimed(cb, timeout1));
                                }
                            }
                            else {
                                // a 'btesos_TcpCbChannel'
                                bdef_Function<void (*)(btesc_CbChannel*, int)>
                                                                            cb;
                                if (VALUES[i][j].d_timeoutFlag) {
                                    validChannel = 0;
                                    cb = bdef_BindUtil::bind(
                                            &acceptCb,
                                            _1, _2,
                                            &acceptor, &connections,
                                            validChannel, expStatus,
                                            cancelFlag, closeFlag);

                                    LOOP2_ASSERT(VALUES[i][j].d_lineNum,
                                                 i, 0 ==
                                    acceptor.timedAllocate(cb, timeout1));
                                }
                                else {
                                    cb = bdef_BindUtil::bind(
                                            &acceptCb,
                                            _1, _2,
                                            &acceptor, &connections,
                                            validChannel, expStatus,
                                            cancelFlag, closeFlag);
                                          LOOP2_ASSERT(VALUES[i][j].d_lineNum,
                                                       i, 0 ==
                                                       acceptor.allocate(cb));
                                }
                            }
                        } break;
                        case 'D': {
                            manager.dispatch(0);
                            bcemt_ThreadUtil::microSleep(
                                                    DEFAULT_SLEEP_TIME * 2);
                        } break;
                        default:
                            break;
                        }
                        LOOP_ASSERT(VALUES[i][j].d_lineNum,
                           VALUES[i][j].d_expNumEvents == manager.numEvents());
                        LOOP_ASSERT(VALUES[i][j].d_lineNum,
                          VALUES[i][j].d_expNumTimers == manager.numTimers());
                        LOOP_ASSERT(VALUES[i][j].d_lineNum,
                          VALUES[i][j].d_expNumChannels ==
                                                      acceptor.numChannels());
                        if (veryVerbose) {
                            PT(manager.numEvents());
                            PT(manager.numTimers());
                            PT(acceptor.numChannels());
                        }
                    }
                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }

        } break;
        case 6: {
            // ----------------------------------------------------------------
            // TESTING 'timedAllocateTimed' METHOD:
            //
            // Plan:
            //
            // Testing: int timedAllocateTimed();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'timedAllocateTimed' method\n"
                              << "\n===================================\n";
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call

                } VALUES[] =
                  //line    numConnections  initRequests
                  //----    --------------  ------------
                {
                  {  L_,           1,                  0 },
                  {  L_,           9,                  1 },
                  {  L_,           9,                  5 },
                  {  L_,           9,                  9 },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bdet_TimeInterval timeout(50, 100000);
                    timeout += bdetu_SystemTime::now();

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                            cb(bdef_BindUtil::bind( &acceptCb
                                                  , _1, _2
                                                  , &acceptor
                                                  , &connections
                                                  , channelFlag
                                                  , expStatus
                                                  , cancelFlag
                                                  , closeFlag));

                        LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocateTimed(cb, timeout));

                    }
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (connections >= VALUES[i].d_initRequests) {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                               0 == acceptor.timedAllocateTimed(cb, timeout));
                        }
                        ASSERT(connections == acceptor.numChannels());

                        manager.dispatch(0);

                        if (connections < VALUES[i].d_initRequests) {
                            LOOP2_ASSERT(__LINE__, connections,
                                         2 == manager.numEvents());
                            LOOP2_ASSERT(__LINE__, connections,
                                         1 == manager.numTimers());
                        }
                        else {
                            LOOP2_ASSERT(__LINE__, connections,
                                   0 == manager.numEvents());
                        }
                    }
                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 5: {
            // ----------------------------------------------------------------
            // TESTING 'timedAllocate' METHOD:
            //
            // Plan:
            //
            // Testing: int timedAllocate();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'timedAllocate' method"
                              << "\n==============================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call

                } VALUES[] =
                  //line    numConnections  initRequests
                  //----    --------------  ------------
                {
                  {  L_,           1,                  0 },
                  {  L_,           9,                  1 },
                  {  L_,           9,                  5 },
                  {  L_,           9,                  9 },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    int ret = bcemt_ThreadUtil::create(&threadHandle,
                                                       attributes,
                                                       threadToConnect,
                                                       &connectInfo);
                    LOOP_ASSERT(i, 0 == ret);

                    bdet_TimeInterval timeout(50, 10000000);
                    timeout += bdetu_SystemTime::now();

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                bdef_BindUtil::bind( &acceptCb
                                                   , _1, _2
                                                   , &acceptor
                                                   , &connections
                                                   , channelFlag
                                                   , expStatus
                                                   , cancelFlag
                                                   , closeFlag));

                        LOOP2_ASSERT(i, j,
                              0 == acceptor.timedAllocate(cb, timeout));

                    }

                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (connections >= VALUES[i].d_initRequests) {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                 0 == acceptor.timedAllocate(cb, timeout));
                        }
                        ASSERT(connections == acceptor.numChannels());
                        manager.dispatch(0);

                        if (connections < VALUES[i].d_initRequests) {
                            LOOP2_ASSERT(__LINE__, connections,
                                         2 == manager.numEvents());
                            LOOP2_ASSERT(__LINE__, connections,
                                         1 == manager.numTimers());
                        }
                        else {
                            LOOP2_ASSERT(__LINE__, connections,
                                         0 == manager.numEvents());
                        }
                    }
                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 4: {
            // ----------------------------------------------------------------
            // TESTING 'allocateTimed' METHOD:
            //
            // Plan:
            //
            // Testing: int allocateTimed();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'allocateTimed' method"
                              << "\n==============================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call

                } VALUES[] =
                  //line    numConnections  initRequests
                  //----    --------------  ------------
                {
                  {  L_,           1,                  0 },
                  {  L_,           9,                  1 },
                  {  L_,           9,                  5 },
                  {  L_,           9,                  9 },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                            cb(bdef_BindUtil::bind( &acceptCb
                                                  , _1, _2
                                                  , &acceptor
                                                  , &connections
                                                  , channelFlag
                                                  , expStatus
                                                  , cancelFlag
                                                  , closeFlag));

                        LOOP2_ASSERT(i, j, 0 == acceptor.allocateTimed(cb));

                    }

                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (connections >= VALUES[i].d_initRequests) {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
                                cb(bdef_BindUtil::bind( &acceptCb
                                                      , _1, _2
                                                      , &acceptor
                                                      , &connections
                                                      , channelFlag
                                                      , expStatus
                                                      , cancelFlag
                                                      , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                         0 == acceptor.allocateTimed(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());

                        manager.dispatch(0);

                        if (connections < VALUES[i].d_initRequests) {
                            LOOP2_ASSERT(__LINE__, connections,
                                         1 == manager.numEvents());

                        }
                        else {
                            LOOP2_ASSERT(__LINE__, connections,
                                   0 == manager.numEvents());
                        }
                    }
                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 3: {
            // ----------------------------------------------------------------
            // TESTING 'allocate' METHOD:
            //
            // Plan:
            //
            // Testing: int allocate();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'allocate' method"
                              << "\n=========================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call

                } VALUES[] =
                  //line    numConnections  initRequests
                  //----    --------------  ------------
                {
                  {  L_,           1,                  0 },
                  {  L_,           9,                  1 },
                  {  L_,           9,                  5 },
                  {  L_,           9,                  9 },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                bdef_BindUtil::bind( &acceptCb
                                                   , _1, _2
                                                   , &acceptor
                                                   , &connections
                                                   , channelFlag
                                                   , expStatus
                                                   , cancelFlag
                                                   , closeFlag));

                        LOOP2_ASSERT(i, j, 0 == acceptor.allocate(cb));
                    }

                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (connections >= VALUES[i].d_initRequests) {
                            int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                         0 == acceptor.allocate(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());

                        manager.dispatch(0);

                        if (connections < VALUES[i].d_initRequests) {
                            LOOP2_ASSERT(__LINE__, connections,
                                         1 == manager.numEvents());

                        }
                        else {
                            LOOP2_ASSERT(__LINE__, connections,
                                   0 == manager.numEvents());
                        }
                    }
                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 2: {
            // ----------------------------------------------------------------
            // TESTING ACCESSOR METHODS:
            //
            // Plan:
            //
            // Testing:
            //   int allocate();
            //   const bteso_IPv4Address& address() const;
            //   int isInvalid() const;
            //   int numChannels() const;
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting 'allocate' method"
                              << "\n=========================" << endl;
            {
                struct {
                    int d_lineNum;
                    int d_numConnections; // the total number of connections
                                          // for this test
                    int d_initRequests;   // the number of "ACCEPT" requests
                                          // before any dispatch() call
                    int d_expInvalidFlag;
                } VALUES[] =
                  //line    Connections   initRequests  expInvalidFlag
                  //----    -----------   ------------  ------------
                {
                  {  L_,           1,          0,                 0 },
                  {  L_,           9,          1,                 0 },
                  {  L_,           9,          5,                 0 },
                  {  L_,           9,          9,                 0 },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    bteso_IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                    bteso_TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              DEFAULT_EQUEUE_SIZE));

                    bcemt_ThreadUtil::Handle threadHandle;
                    bcemt_Attribute attributes;
                    int connections = 0;

                    bteso_IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   DEFAULT_SLEEP_TIME
                                                 };

                    bcemt_ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                bdef_BindUtil::bind( &acceptCb
                                                   , _1, _2
                                                   , &acceptor
                                                   , &connections
                                                   , channelFlag
                                                   , expStatus
                                                   , cancelFlag
                                                   , closeFlag));

                        LOOP2_ASSERT(i, j, 0 == acceptor.allocate(cb));

                    }
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (connections >= VALUES[i].d_initRequests) {
                            int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                            bdef_Function<void (*)(btesc_CbChannel*, int)> cb(
                                    bdef_BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(__LINE__, connections,
                                         0 == acceptor.allocate(cb));
                        }
                        ASSERT(connections == acceptor.numChannels());

                        manager.dispatch(0);

                        if (connections < VALUES[i].d_initRequests) {
                            LOOP2_ASSERT(__LINE__, connections,
                                         1 == manager.numEvents());

                        }
                        else {
                            LOOP2_ASSERT(__LINE__, connections,
                                   0 == manager.numEvents());
                        }
                    }

                    if (veryVeryVerbose) {
                        QT("The total number of channels established: ");
                        PT(acceptor.numChannels());
                    }
                    LOOP_ASSERT(i, VALUES[i].d_expInvalidFlag ==
                                                   acceptor.isInvalid());
                    if (verbose) {
                        PT(acceptor.address().ipAddress());
                    }
                     bcemt_ThreadUtil::microSleep(DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 1: {
            // ----------------------------------------------------------------
            // BREATHING TEST:
            //
            // Plan:
            //
            // Testing:
            //   This Test Case exercises basic functionality but tests NOTHING
            // ----------------------------------------------------------------

            if (verbose) cout << "\nBREATHING TEST"
                              << "\n==============" << endl;

            bteso_TcpTimerEventManager manager(&testAllocator);
            Obj mX(&factory, &manager, &testAllocator); const Obj& X = mX;
            ASSERT(!X.isInvalid());

            bteso_IPv4Address address;
            address.setPortNumber(0);
            address.setIpAddress(hostName);

            ASSERT(0 == mX.open(address, 5)); // Need to make sure that socket,
                                              // etc. is called.
            ASSERT(0 == mX.close());

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
