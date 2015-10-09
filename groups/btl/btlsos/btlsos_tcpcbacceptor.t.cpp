// btlsos_tcpcbacceptor.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlsos_tcpcbacceptor.h>

#include <btlsos_tcptimedcbchannel.h>
#include <btlsos_tcpcbchannel.h>

#include <btlso_ipv4address.h>
#include <btlso_tcptimereventmanager.h>
#include <btlso_inetstreamsocketfactory.h>

#include <btlsc_timedcbchannel.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslim_testutil.h>

#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bdlt_currenttime.h>

#include <bslma_testallocator.h>

#include <bsls_platform.h>
#include <bsls_timeinterval.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'strcmp'
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdlf::PlaceHolders;

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
// [ 2]  const btlso::IPv4Address& address() const;
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
//-----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

bslmt::Mutex  d_mutex;   // for i/o synchronization in all threads

#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const char* hostName = "127.0.0.1";

static int verbose;
static int veryVerbose;

typedef btlsos::TcpCbAcceptor  Obj;

struct ConnectionInfo {
     btlso::StreamSocketFactory<btlso::IPv4Address> *d_factory_p;
     btlso::IPv4Address                            *d_server;
     int                                           d_numConnections;
     int                                           d_sleepTime;
  //  bslmt::Mutex                                  *d_mutex_p;
  //  bslmt::Condition    *d_cv_p;
};

enum {
    k_DEFAULT_PORT_NUMBER     = 0,
    k_DEFAULT_NUM_CONNECTIONS = 10,
    k_DEFAULT_EQUEUE_SIZE     = 5,
    k_DEFAULT_SLEEP_TIME      = 100000
};

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

///Usage
///-----
// The following usage example shows a possible implementation of a multi-user
// echo server.  An echo server accepts connections and, for every connection,
// sends any received data back to the client (until the connection is
// terminated).  This server requires that data is read from an accepted
// connection within a certain time interval or else the connection is dropped
// on timeout.  The echo server is implemented as a separate class
// ('my_EchoServer') that owns the timed callback acceptor.  Various
// configuration parameters such as the timeout values, the queue size and
// input buffer size, and the default port number (as mandated by RFC 862) are
// constants within this class.
//..
    class my_EchoServer {
        // This class implements a simple multi-user echo server as specified
        // by the RFC 862.

        enum {
            k_READ_SIZE           =  200,  // The number of bytes to be read
                                           // can be changed, but a larger
                                           // 'k_READ_SIZE' will require the
                                           // client to input more data to be
                                           // echoed.
            k_DEFAULT_PORT_NUMBER = 9234,  // As specified by the RFC 862
            k_QUEUE_SIZE          =   16
        };

        btlsos::TcpCbAcceptor  d_allocator;
        bsls::TimeInterval     d_acceptTimeout;
        bsls::TimeInterval     d_readTimeout;
        bsls::TimeInterval     d_writeTimeout;

        bsl::function<void(btlsc::TimedCbChannel*, int)>
                               d_allocateFunctor;
                                           // Cached callback functor.

        bslma::Allocator      *d_allocator_p;
        char                   d_buffer[k_READ_SIZE];

      private:
        // Callbacks
        void allocateCb(btlsc::TimedCbChannel *channel, int status);
            // Invoked from the socket event manager when a connection is
            // allocated (i.e., accepted) or an error occurs when allocating.
            // [...]

        void bufferedReadCb(const char            *buffer,
                            int                    status,
                            int                    asyncStatus,
                            btlsc::TimedCbChannel *channel);
            // Invoked from the socket event manager when data is read.  [...]

        void readCb(int                    status,
                    int                    asyncStatus,
                    btlsc::TimedCbChannel *channel);
            // Invoked from the socket event manager when data is read.  [...]

        void writeCb(int                    status,
                     int                    asyncStatus,
                     btlsc::TimedCbChannel *channel,
                     int                    numBytes);
            // Invoked from the socket event manager when data is written.
            // [...]

      private:
        // Not implemented:
        my_EchoServer(const my_EchoServer&);
        my_EchoServer& operator=(const my_EchoServer&);

      public:
        // CREATORS
        my_EchoServer(
               btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
               btlso::TimerEventManager                       *manager,
               bslma::Allocator                               *basicAllocator);
            // Create an echo server that uses the specified stream socket
            // 'factory' for the system sockets and the specified socket event
            // 'manager' to multiplex the events on these sockets.  The
            // behavior is undefined if either 'factory' or 'manager' is 0.

        ~my_EchoServer();
            // Destroy this server.  The behavior is undefined unless the
            // server is shut down properly (i.e., via 'close').

        // MANIPULATORS
        int open(int portNumber = k_DEFAULT_PORT_NUMBER);
            // Establish a listening socket on the optionally specified
            // 'portNumber'; return 0 on success, and a non-zero value
            // otherwise.  The behavior is undefined unless '0 <= portNumber'
            // and the listening port is not currently open.

        int close();
            // Close the listening socket; return 0 on success and a non-zero
            // value otherwise.  The behavior is undefined unless the listening
            // socket is currently open.
    };
//..
// The implementation of the public methods of 'my_EchoServer' is trivial.  For
// the constructor, the socket factory and socket event manager are passed to
// the acceptor, and the allocate callback and (three) event timeouts are
// initialized.  The 'open' and 'close' methods just invoke appropriate methods
// of the acceptor.  The destructor ensures that the state of the acceptor is
// valid:
//..
    // CREATORS
    my_EchoServer::my_EchoServer(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                bslma::Allocator                               *basicAllocator)
    : d_allocator(factory, manager, basicAllocator)
    , d_acceptTimeout(120, 0)
    , d_readTimeout(5, 0)
    , d_writeTimeout(5.0)
    , d_allocator_p(basicAllocator)
    {
        ASSERT(factory);
        ASSERT(manager);
        d_allocateFunctor = bsl::function<void(btlsc::TimedCbChannel *, int)>(
            bsl::allocator_arg_t(),
            bsl::allocator<bsl::function<void(btlsc::TimedCbChannel *, int)> >(
                basicAllocator),
            bdlf::MemFnUtil::memFn(&my_EchoServer::allocateCb, this));
    }

    my_EchoServer::~my_EchoServer() {
    }

    // MANIPULATORS
    int my_EchoServer::open(int portNumber) {
        btlso::IPv4Address serverAddress;
        serverAddress.setPortNumber(portNumber);
        int s = d_allocator.open(serverAddress, k_QUEUE_SIZE);
        if (s) {
            cout << "Failed to open listening port." << endl;
            return s;                                                 // RETURN
        }
        cout << "server's socket: " << d_allocator.address() << endl;
        ASSERT(0 == d_allocator.isInvalid());
        s = d_allocator.allocateTimed(d_allocateFunctor);
        if (s) {
            cout << "Can't enqueue an allocation request." << endl;
            ASSERT(d_allocator.isInvalid());
            d_allocator.close();
            return s;                                                 // RETURN
        }
        return 0;
    }

    int my_EchoServer::close() {
        return d_allocator.close();
    }
//..
// All the work of accepting connections and reading/writing the data is done
// in the (private) callback methods of 'my_EchoServer'.  When the connection
// is established and the 'allocateCb' method is invoked, the buffered read
// with timeout is initiated on the channel and another allocation request is
// enqueued.  If any error occurs while allocating, the acceptor is shut down.
// If any error, including timeout, occurs while reading or writing data, the
// channel is shut down.  Note that the allocation functor is cached to improve
// performance:
//..
    void my_EchoServer::allocateCb(btlsc::TimedCbChannel *channel,
                                   int                    status) {
        if (channel) {
            // Accepted a connection.  Issue a read raw request.
            bsl::function<void(int, int)> callback(bdlf::BindUtil::bind(
                          bdlf::MemFnUtil::memFn(&my_EchoServer::readCb, this),
                          _1,
                          _2,
                          channel));

            if (channel->timedReadRaw(d_buffer,
                                      k_READ_SIZE,
                                      bdlt::CurrentTime::now()
                                                  + d_readTimeout, callback)) {
                cout << "Failed to enqueue buffered read request." << endl;
                d_allocator.deallocate(channel);
            }
            if (d_allocator.allocateTimed(d_allocateFunctor)) {
                d_allocator.close();
            }
            return;                                                   // RETURN
        }
        ASSERT(0 >= status);    // Interrupts are not enabled.
        if (0 == status) {
            // Re-register the functor
            if (d_allocator.allocateTimed(d_allocateFunctor)) {
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

    void my_EchoServer::bufferedReadCb(const char            *buffer,
                                       int                    status,
                                       int                    asyncStatus,
                                       btlsc::TimedCbChannel *channel)
    {
        cout << "my_EchoServer::bufferedReadCb: "
             << " read " << status << " bytes." << endl;
        ASSERT(channel);
        if (0 < status) {
            bsl::function<void(int, int)> callback(bdlf::BindUtil::bind(
                         bdlf::MemFnUtil::memFn(&my_EchoServer::writeCb, this),
                         _1,
                         _2,
                         channel,
                         status));

            if (channel->timedBufferedWrite(buffer,
                                            status,
                                            bdlt::CurrentTime::now()
                                                 + d_writeTimeout, callback)) {
                cout << "Failed to enqueue write request" << endl;
                d_allocator.deallocate(channel);
                return;                                               // RETURN
            }
            // Re-register read request
            bsl::function<void(const char *, int, int)> readCallback(
                bdlf::BindUtil::bind(
                         bdlf::MemFnUtil::memFn(&my_EchoServer::bufferedReadCb,
                                                this),
                         _1,
                         _2,
                         _3,
                         channel));

            if (channel->timedBufferedRead(k_READ_SIZE,
                     bdlt::CurrentTime::now() + d_readTimeout, readCallback)) {
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

    void my_EchoServer::readCb(int                    status,
                               int                    asyncStatus,
                               btlsc::TimedCbChannel *channel)
    {
        cout << "my_EchoServer::readCb: "
             << " read " << status << " bytes." << endl;
        ASSERT(channel);
        if (0 < status) {
            bsl::function<void(int, int)> callback(
                bdlf::BindUtil::bind(
                         bdlf::MemFnUtil::memFn(&my_EchoServer::writeCb, this),
                         _1,
                         _2,
                         channel,
                         status));

            if (channel->timedBufferedWrite(d_buffer,
                                            status,
                                            bdlt::CurrentTime::now()
                                                   + d_writeTimeout, callback))
                {
                    cout << "Failed to enqueue write request" << endl;
                    d_allocator.deallocate(channel);
                    return;                                           // RETURN
                }
            // Re-register read request
            bsl::function<void(int, int)> readCallback(
                  bdlf::BindUtil::bind(
                          bdlf::MemFnUtil::memFn(&my_EchoServer::readCb, this),
                          _1,
                          _2,
                          channel));

            if (channel->timedReadRaw(d_buffer, k_READ_SIZE,
                                      bdlt::CurrentTime::now()
                                              + d_readTimeout, readCallback)) {
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

    void my_EchoServer::writeCb(int                    status,
                                int                    asyncStatus,
                                btlsc::TimedCbChannel *channel,
                                int                    numBytes)
    {
        (void)asyncStatus;
        if (status != numBytes) {
            cout << "Failed to send data." << endl;
            channel->invalidate();
            d_allocator.deallocate(channel);
        }
    }
//..

//-----------------------------------------------------------------------------
/// Usage example: Tick publisher (see 'btlsc_timedcbchannelallocator.h')

class my_Tick {
    char   d_name[3];
    double d_bestBid;
    double d_bestOffer;
  public:
    my_Tick() { }
    my_Tick(char ticker);
    my_Tick(char ticker, double bestBid, double bestOffer);

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, int version) const;

    void print(bsl::ostream& stream) const;
};

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
STREAM& my_Tick::streamOut(STREAM& stream, int version) const
{
    stream << d_bestBid << d_bestOffer;
    return stream;
}

static void myPrintTick(bsl::ostream& stream, const char *buffer)
    // Print the value of the specified 'buffer' interpreted as a 'bdex'
    // byte-stream representation of a 'my_Tick' value, to the specified
    // 'stream' or report an error to 'stream' if 'buffer' is determined *not*
    // to hold an encoding of a valid 'my_Tick' value.
{
    const my_Tick *tick = (const my_Tick*) buffer;
    stream << *tick;
}

class my_TickReporter {
    // This class implements a server that accepts connections, extracts from
    // each connection a single 'my_Tick' value, and reports that value to a
    // console stream; both the acceptor and console stream are supplied at
    // construction.

    btlsc::CbChannelAllocator      *d_acceptor_p; // incoming connections
    bsl::ostream&                  d_console;    // where to put tick info

    btlsc::CbChannelAllocator::TimedCallback
                                   d_allocFunctor;  // reused

  private:
    void acceptCb(btlsc::TimedCbChannel     *clientChannel,
                  int                        status,
                  const bsls::TimeInterval&  timeout);
        // Called when a new client channel has been accepted.  ...

    void readCb(const char            *buffer,
                int                    status,
                int                    asyncStatus,
                btlsc::TimedCbChannel *clientChannel);
        // Called when a 'my_Tick' value has been read from the channel.  ...

  private:
    my_TickReporter(const my_TickReporter&);             // not impl.
    my_TickReporter& operator=(const my_TickReporter&);  // not impl.

  public:
    my_TickReporter(bsl::ostream&              console,
                    btlsc::CbChannelAllocator *acceptor);
        // Create a non-blocking tick-reporter using the specified 'acceptor'
        // to establish incoming client connections, each transmitting a single
        // 'my_Tick' value; write these values to the specified 'console'
        // stream.  If the 'acceptor' is idle for more than five minutes, print
        // a message to the 'console' stream supplied at construction and
        // continue.  To guard against malicious clients, a connection that
        // does not produce a tick value within one minute will be summarily
        // dropped.

    ~my_TickReporter();
        // Destroy this server object.
};

const double ACCEPT_TIME_LIMIT = 300;               // 5 minutes
const double   READ_TIME_LIMIT =  60;               // 1 minutes

static int calculateMyTickMessageSize()
    // Calculate and return the number of bytes encoding of a 'my_Tick' value
    // (called just once, see below).
{
    return sizeof (my_Tick);
}

static int myTickMessageSize()
    // Return the number of bytes in a 'bdex' byte-stream encoding of a
    // 'my_Tick' value without creating a runtime-initialized file-scope static
    // variable (which is link-order dependent).
{
    static const int MESSAGE_SIZE = calculateMyTickMessageSize();
    return MESSAGE_SIZE;
}

void my_TickReporter::acceptCb(btlsc::TimedCbChannel     *clientChannel,
                               int                        status,
                               const bsls::TimeInterval&  timeout)
{
    bsls::TimeInterval nextTimeout(timeout);

    if (clientChannel) {     // Successfully created a connection.

        const int               numBytes = ::myTickMessageSize();
        const bsls::TimeInterval now      = bdlt::CurrentTime::now();

        // Create one-time (buffered) read functor holding 'clientChannel'.
        btlsc::TimedCbChannel::BufferedReadCallback readFunctor(
                bdlf::BindUtil::bind(
                    bdlf::MemFnUtil::memFn(&my_TickReporter::readCb, this)
                  , _1, _2, _3
                  , clientChannel));

        // Install read callback (timeout, but no raw or asynchronous
        // interrupt).

        if (clientChannel->timedBufferedRead(numBytes, now + READ_TIME_LIMIT,
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

        // Note that attempting to re-register an allocate operation below will
        // fail only if the channel allocator is permanently disabled.
    }

    // In all cases, attempt to reinstall the (reusable) accept callback.

    if (d_acceptor_p->allocateTimed(d_allocFunctor)) {
        d_console << "Error: unable to register accept opeation." << bsl::endl;
        // This server is hosed.
    }
}

void my_TickReporter::readCb(const char            *buffer,
                             int                    status,
                             int                    asyncStatus,
                             btlsc::TimedCbChannel *clientChannel)
{
    ASSERT(clientChannel);

    const int msgSize = ::myTickMessageSize();

    if (msgSize == status) {  // Encoded-tick value read successfully.
        ASSERT(buffer);

        ::myPrintTick(d_console, buffer);
    }
    else if (0 <= status) {   // Tick message was interrupted.

        ASSERT(buffer); // Data in buffer is available for inspection (but
                        // remains in the channel's buffer).

        // Must be a timeout event since neither raw (partial) reads nor
        // (external) asynchronous interrupts were authorized.

        ASSERT(0 == asyncStatus);   // must be timeout event!

        d_console << "Error: Unable to read tick value from channel"
                     " before timing out; read aborted." << bsl::endl;
    }
    else { // Tick-message read failed.
        ASSERT (0 > status);

        d_console << "Error: Unable to read tick value from channel."
                  << bsl::endl;
    }

    d_acceptor_p->deallocate(clientChannel);
}

my_TickReporter::my_TickReporter(bsl::ostream&              console,
                                 btlsc::CbChannelAllocator *acceptor)
: d_acceptor_p(acceptor)
, d_console(console)
{
    ASSERT(&d_console);
    ASSERT(d_acceptor_p);

    // Attempt to install the first accept callback.

    bsls::TimeInterval timeout = bdlt::CurrentTime::now()
        + ACCEPT_TIME_LIMIT;

    // load reusable allocate functor
    d_allocFunctor
        = bdlf::BindUtil::bind(
                bdlf::MemFnUtil::memFn(&my_TickReporter::acceptCb, this)
              , _1, _2
              , timeout);

    if (d_acceptor_p->allocateTimed(d_allocFunctor)) {
        d_console << "Error: Unable to install accept operation." << bsl::endl;
        // This server is hosed.
    }
}

my_TickReporter::~my_TickReporter()
{
    ASSERT(&d_console);
    ASSERT(d_acceptor_p);
}

static void acceptCb(btlsc::CbChannel      *channel,
                     int                    status,
                     btlsos::TcpCbAcceptor *acceptor,
                     int                   *numConnections,
                     int                    validChannel,
                     int                    expStatus,
                     int                    cancelFlag,
                     int                    closeFlag)
    // Verify the result of an "ACCEPT" request by comparing against the
    // expected values: If the specified 'channelFlag' is nonzero, a new
    // 'btlsc::CbChannel' should be established; the specified return 'status'
    // should be the same as the specified 'expStatus'.  If the specified
    // 'cancelFlag' is nonzero, invoke the 'cancelAll()' on the specified
    // 'acceptor' to help the test.  Similarly, if the specified 'closeFlag' is
    // nonzero, invoke the 'close()' on the 'acceptor'.
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

#if !defined(BSLS_PLATFORM_CMP_SUN) \
    || BSLS_PLATFORM_CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.  This was fixed in Sun Studio
    // 8 compiler.
#endif
void *threadToConnect(void *arg)
    // Create the specified number of client sockets, connect each client
    // socket with the server created with a 'btlsos::TcpCbAcceptor' object.
    // The following information will be pass in through the specified 'arg':
    // the btlso::IPv4Address object of the server to be connected; the number
    // of connections to be established.
{
    ASSERT(arg);
    // Since 'bslma::TestAllocator' is not thread-safe, the helper thread can't
    // share the 'factory' (which uses a 'bslma::TestAllocator' object) and so
    // we have to instantiate another factory object for the helper thread.
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;

    ConnectionInfo info = *(ConnectionInfo*) arg;
    if (verbose) {
        P_T(*(info.d_server));
        P_T(info.d_numConnections);
        PT(info.d_sleepTime);
    }
    bslmt::ThreadUtil::microSleep(info.d_sleepTime);
    // bslma::TestAllocator is not thread-safe, so need to provide a new one
    // for the thread.
    bslma::TestAllocator threadTestAllocator;

    bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>
                                          clients(&threadTestAllocator);
    int i = 0;

    while (i < info.d_numConnections) {
        btlso::StreamSocket<btlso::IPv4Address>
            *d_connectingSocket_p = factory.allocate();

        // info.d_mutex_p->unlock();

        ASSERT(0 != d_connectingSocket_p);
        int s = d_connectingSocket_p->connect(*(info.d_server));

        if (0 == s) {
            ++i;
            if (veryVerbose) {
                QT("A new connection is established between clent and "
                   "server: ");
                P_T(d_connectingSocket_p->handle());
                PT(*(info.d_server));
            }
            clients.push_back(d_connectingSocket_p);
        }
        else {
            if (veryVerbose) {
                PT(s);
            }
        }
    }
    int length = static_cast<int>(clients.size());

    for (int i = 0; i < length; ++i) {
        factory.deallocate(clients[i]);
    }
    //info.d_cv->signal();     // To wake the main thread to verify the result.
    //info.d_cv->wait(info.d_mutex); // Wait for the verification to be done.
    if (veryVerbose) {
        QT(" clients exited. ");
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
    verbose = argc > 2;
    veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&testAllocator);

    ASSERT(0 == btlso::SocketImpUtil::startup());
    switch (test) { case 0:
      case -2: {
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
          //   USAGE EXAMPLE // TBD.
          // ----------------------------------------------------------------

          if (verbose) cout << "\nTesting Usage Example: Echo Server"
              << "\n==================================" << endl;
          {
              enum { k_DEFAULT_PORT = 5000, k_LISTEN_QUEUE_SIZE = 64 };

              const int portNumber = argc > 2 ? atoi(argv[2]) : k_DEFAULT_PORT;

              btlso::IPv4Address address(btlso::IPv4Address::k_ANY_ADDRESS,
                                         portNumber);
              btlso::TcpTimerEventManager sem;           // concrete manager
              btlso::InetStreamSocketFactory<btlso::IPv4Address> sf;
              // concrete factory
              btlsos::TcpCbAcceptor acceptor(&sf, &sem);

              if (acceptor.open(address, k_LISTEN_QUEUE_SIZE)) {
                  cout << "Error: Unable to listen on " << address << endl;
                  break; // return -1;
              }

              my_TickReporter reporter(bsl::cout, &acceptor);

              while (0 != sem.numEvents()) {
                  sem.dispatch(btlso::Flag::k_ASYNC_INTERRUPT);
              }
          }

      } break;
      case -1: {
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
          //   USAGE EXAMPLE // TBD.
          // ----------------------------------------------------------------

          if (verbose) cout << "\nTesting Usage Example"
                            << "\n=====================" << endl;

// Finally, we define the 'main' function, which will create the concrete
// socket factory and concrete socket event manager, create 'my_EchoServer' as
// required, and go into "infinite" loop dispatching registered callbacks:
//..
    bslma::TestAllocator testAllocator;
    testAllocator.setNoAbort(1);

    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&testAllocator);

    btlso::TcpTimerEventManager::Hint hint
                                  = btlso::TcpTimerEventManager::e_NO_HINT;

    btlso::TcpTimerEventManager manager(hint, &testAllocator);
    my_EchoServer               echoServer(&factory, &manager, &testAllocator);

    if (echoServer.open()) {
        cout << "Can't open listening socket." << endl;
        break; // return -1;
    }

    for (int i = 0; i < 100; ++i) {
        // Do it for the finite number of times.
        int cbs = manager.dispatch(0);
        ASSERT(0 < cbs);
    }
    echoServer.close();
    // We need another round of dispatch for cleaning things up.
    int cbs = manager.dispatch(0);
    ASSERT(0 < cbs);
    ASSERT(0 == testAllocator.numMismatches());
//..
        } break;
        case 7: {
            // ----------------------------------------------------------------
            // TESTING 'invalidate' METHOD:
            //
            // Plan:
            //
            // Testing:
            //   int invalidate();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'invalidate' method");
                QT("===========================");
            }

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
                  {  L_,        0,         1,        3,     1     },
                  {  L_,        0,         9,        3,     1     },
                  {  L_,        1,         1,        3,     1     },
                  {  L_,        1,         9,        3,     1     },
                  {  L_,        9,         1,        3,     1     },
                  {  L_,        9,         9,        3,     1     },
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                for (int i = 0; i < NUM_VALUES; i++) {
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;

                    btlso::IPv4Address serverAddr(acceptor.address());
                    int totalConnections = VALUES[i].d_numConnections +
                                           VALUES[i].d_initRequests;
                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   totalConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bsls::TimeInterval timeout(50, 10000000);
                    timeout += bdlt::CurrentTime::now();
                    int connections = 0, timeoutChannel = 0;

                    for (int j = 0; j < VALUES[i].d_numConnections; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.allocate(cb));
                        }
                        else {
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                    // connections for suspending requests but can not receive
                    // any new request.
                    connections = 0;
                    while (connections < VALUES[i].d_initRequests) {
                        LOOP_ASSERT(i,
                                  connections + VALUES[i].d_numConnections ==
                                                   acceptor.numChannels());

                        LOOP2_ASSERT(__LINE__, connections,
                                     1 == manager.numEvents());

                        manager.dispatch(0);

                    }
                    LOOP2_ASSERT(__LINE__, connections,
                                     0 == manager.numEvents());

                    for (int j = 0; j < VALUES[i].d_newRequests; ++j) {
                        if (!timeoutChannel) {
                            int channelFlag = 0, expStatus = -1,
                                cancelFlag = 0, closeFlag = 0;

                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                    bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 6: {
            // ----------------------------------------------------------------
            // TESTING 'close' METHOD:
            //
            // Plan:
            //
            // Testing: int close();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'close' method");
                QT("======================");
            }

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
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;

                    btlso::IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bsls::TimeInterval timeout(50, 10000000);
                    timeout += bdlt::CurrentTime::now();
                    int connections = 0, timeoutChannel = 0;
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;
                        if (!timeoutChannel) {
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.allocate(cb));
                        }
                        else {
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                    LOOP_ASSERT(i, 0 == acceptor.close());
                    LOOP_ASSERT(i, acceptor.address() == btlso::IPv4Address());
                    LOOP_ASSERT(i, VALUES[i].d_numConnections ==
                                              acceptor.numChannels());
                    LOOP_ASSERT(i, 0 == manager.numEvents());

                    // We now make sure the acceptor is invalid and can not
                    // receive any new request.
                    int newRequests = 0;
                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (!timeoutChannel) {
                            int channelFlag = 0, expStatus = -2,
                                cancelFlag = 0, closeFlag = 0;

                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                    bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
                }
            }
        } break;
        case 5: {
            // ----------------------------------------------------------------
            // TESTING 'cancelAll' METHOD:
            //
            // Plan:
            //
            // Testing: int cancelAll();
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing 'cancelAll' method");
                QT("==========================");
            }

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
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;

                    btlso::IPv4Address serverAddr(acceptor.address());
                    // We will try establish another connection after the
                    // cancelAll() is called.
                    int totalConnections = VALUES[i].d_numConnections + 1;

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   totalConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    bsls::TimeInterval timeout(50, 10000000);
                    timeout += bdlt::CurrentTime::now();
                    int connections = 0, timeoutChannel = 0;

                    for (int k = 0; k < VALUES[i].d_numConnections; ++k) {
                        if (!timeoutChannel) {
                            int channelFlag = 1, expStatus = 0,
                                cancelFlag  = 0, closeFlag = 0;

                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.allocate(cb));
                        }
                        else {
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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

                    // We now make sure the acceptor is still valid and can
                    // establish new connection upon request.  Besides, we also
                    // test call cancelAll() from the user callback function by
                    // set the 'closeFlag' to 1 to invoke cancelAll() inside
                    // the callback function.

                    for (int j = 0; j < VALUES[i].d_newRequests; ++j) {

                        int channelFlag = 0, expStatus = -1,
                               cancelFlag = 0, closeFlag = 0;
                        // Only make the first request invoke cancelAll() in
                        // the user callback.
                        if (0 == j) {
                            channelFlag = 1;
                            expStatus = 0;
                            cancelFlag = 1;
                        }
                        if (!timeoutChannel) {
                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
                                                       , _1, _2
                                                       , &acceptor
                                                       , &connections
                                                       , channelFlag
                                                       , expStatus
                                                       , cancelFlag
                                                       , closeFlag));

                            LOOP2_ASSERT(i, j,
                              0 == acceptor.allocate(cb));
                        }
                        else {
                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                    LOOP_ASSERT(i, 1 == manager.numEvents());
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
                    bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
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

            if (verbose) {
                QT("Testing 'allocateTimed' method");
                QT("==============================");
            }

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
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;
                    int connections = 0;

                    btlso::IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bsl::function<void(btlsc::TimedCbChannel*, int)>
                            cb(bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::TimedCbChannel*, int)>
                                cb(bdlf::BindUtil::bind( &acceptCb
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
                        QT("The total number of channels established:");
                        PT(acceptor.numChannels());
                    }
                    bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
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

            if (verbose) {
                QT("Testing 'allocate' method");
                QT("=========================");
            }

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
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;
                    int connections = 0;

                    btlso::IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bsl::function<void(btlsc::CbChannel*, int)> cb(
                                bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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
                        QT("The total number of channels established:");
                        PT(acceptor.numChannels());
                    }
                    bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
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
            //   const btlso::IPv4Address& address() const;
            //   int isInvalid() const;
            //   int numChannels() const;
            // ----------------------------------------------------------------

            if (verbose) {
                QT("Testing ACCESSOR METHODS");
                QT("========================");
            }

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
                    btlso::IPv4Address serverAddress;
                    serverAddress.setIpAddress(hostName);
                    serverAddress.setPortNumber(k_DEFAULT_PORT_NUMBER);

                    btlso::TcpTimerEventManager manager(&testAllocator);
                    Obj acceptor(&factory, &manager, &testAllocator);
                    ASSERT(0 == acceptor.open(serverAddress,
                                              k_DEFAULT_EQUEUE_SIZE));

                    bslmt::ThreadUtil::Handle threadHandle;
                    bslmt::ThreadAttributes attributes;
                    int connections = 0;

                    btlso::IPv4Address serverAddr(acceptor.address());

                    ConnectionInfo connectInfo = { &factory,
                                                   &serverAddr,
                                                   VALUES[i].d_numConnections,
                                                   k_DEFAULT_SLEEP_TIME
                                                 };

                    bslmt::ThreadUtil::create(&threadHandle, attributes,
                                             threadToConnect, &connectInfo);

                    for (int j = 0; j < VALUES[i].d_initRequests; ++j) {
                        int channelFlag = 1, expStatus = 0,
                            cancelFlag = 0, closeFlag = 0;

                        bsl::function<void(btlsc::CbChannel*, int)> cb(
                                bdlf::BindUtil::bind( &acceptCb
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

                            bsl::function<void(btlsc::CbChannel*, int)> cb(
                                    bdlf::BindUtil::bind( &acceptCb
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
                        QT("The total number of channels established:");
                        PT(acceptor.numChannels());
                    }
                    LOOP_ASSERT(i, VALUES[i].d_expInvalidFlag ==
                                                   acceptor.isInvalid());
                    if (verbose) {
                        PT(acceptor.address().ipAddress());
                    }
                     bslmt::ThreadUtil::microSleep(k_DEFAULT_SLEEP_TIME * 2);
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
            //   BREATHING TEST
            // ----------------------------------------------------------------
            if (verbose) {
                QT("BREATHING TEST");
                QT("==============");
            }

            btlso::TcpTimerEventManager manager(&testAllocator);
            Obj mX(&factory, &manager, &testAllocator); const Obj& X = mX;
            ASSERT(!X.isInvalid());

            btlso::IPv4Address address;
            address.setPortNumber(0);
            address.setIpAddress(hostName);

            ASSERT(0 == mX.open(address, 5)); // Need to make sure that, e.g.,
                                              // socket is called.
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
