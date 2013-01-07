// btesos_tcpconnector.t.cpp      -*-C++-*-

#include <btesos_tcpconnector.h>
#include <btesos_tcpchannel.h>
#include <btesos_tcptimedchannel.h>

#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <bteso_ipv4address.h>
#include <bteso_socketimputil.h>

#include <btesc_flag.h>
#include <btesc_channel.h>
#include <btesc_timedchannel.h>

#include <bdet_timeinterval.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bcemt_thread.h>                   // thread management util
#include <bcema_testallocator.h>            // thread-safe allocator
#include <bsl_typeinfo.h>
#include <bsl_c_time.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_signal.h>
#endif

#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_c_stdio.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                OVERVIEW
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
//-----------------------------------------------------------------------------
// [ 2] virtual btesos_TcpConnector()
// [ 2] virtual btesos_TcpConnector(..., int initialCapacity, ...)
// [ 2] virtual ~btesos_TcpConnector()
// [ 4] btesc_Channel *allocate()
// [ 5] btesc_Channel *allocateTimed()
// [ 6] int open();
// [ 6] virtual void invalidate()
// [ 6] int setOption();
// [ 6] int getOption();
// [ 6] int close();
// [ 6] void deallocate();
// [ 3] const bteso_StreamSocket<bteso_IPv4Address> *socket() const;
// [ 3] const bteso_IPv4Address& address() const;
// [ 3] virtual int isInvalid()
// [ 3] int numChannels() const;
//-----------------------------------------------------------------------------
// [10] USAGE example
// [ 1] Breathing test
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btesos_TcpConnector Obj;
typedef bteso_StreamSocket<bteso_IPv4Address> StreamSocket;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

const char *HOST_NAME = "127.0.0.1";
const char *REMOTE_HOST = "204.228.67.117"; // An outside host IP address
                                            // (www.cu.edu)

const bdet_TimeInterval *INFINITED = 0;

enum {
    DEFAULT_PORT_NUMBER     = 0,
    REMOTE_PORT_NUMBER      = 7,
    DEFAULT_NUM_CONNECTIONS = 10,
    DEFAULT_EQUEUE_SIZE     = 5,
    SLEEP_TIME              = 100000,
    VALID                   = 0,
    INVALID                 = -1,
    NO_OP                   = -2
};

enum {
    CHANNEL   = 0,              // 'btesos_TcpChannel'
    T_CHANNEL = 1               // 'btesos_TcpTimedChannel'
};

struct ConnectionInfo {
     // Use this struct to pass information to the helper thread.

     bcemt_ThreadUtil::Handle d_tid;        // the id of the thread to
                                            // which a signal's delivered

     bteso_StreamSocket<bteso_IPv4Address>              *d_serverSocket_p;

     int                d_signals;         // flags indicating if signals
                                            // will be raised and how many

     int                d_numConnections;   // the maximum number of
                                            // connections to be established

     bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                       *d_connList;

     int                d_equeueSize;

};

struct TestCommand {
    // This struct includes needed information for each test operation,
    // e.g., a request to "ACCEPT",  along with it's corresponding
    // test results.

    int                d_lineNum;

    char               d_commandCode;    // command to invoke a corresponding
                                         // function, e.g., 'C' -- invoke the
                                         // acceptor's close(); 'A' --
                                         // an "allocate" function etc.

    int                d_channelType;    // a request for a 'btesos_TcpChannel'
                                         // or 'btesos_TcpTimedChannel', i.e.,
                                         // 0 for a 'btesos_TcpChannel' and 1
                                         // for a 'btesos_TcpTimedChannel'

    int                d_interruptFlags; // interruptible or not

    const bdet_TimeInterval *d_timeout;  // a flag to indicate if it'll timeout

    int                d_expStatus;      // an expected status value from a
                                         // "CONNECT" request

    int                d_validChannel;   // It's 1 if a valid channel is
                                         // returned, and 0 otherwise.

    int                d_expNumChannels;
};

bslma_TestAllocator testAllocator;

//=============================================================================
//                      HELPER FUNCTIONS/CLASSES FOR TESTING
//-----------------------------------------------------------------------------

#if !defined(BSLS_PLATFORM_CMP_SUN) \
    || BSLS_PLATFORM_CMP_VER_MAJOR >= 1360
extern "C"
#endif
void* threadAsServer(void *arg)
    // Taking information passed from the specified 'arg', wait to accept the
    // expected number of connections, and/or generate signal 'SIGSYS'
    // and deliver it to the thread specified in 'arg'.  Note the test can
    // only work on UNIX platforms since the windows doesn't support signal
    // operations.
{
    ConnectionInfo info = *(ConnectionInfo*) arg;
    if (verbose) {
        P_T(info.d_serverSocket_p->handle());
        PT(info.d_numConnections);
    }

#ifdef BSLS_PLATFORM_OS_UNIX
    int signals = info.d_signals;    // This flag also indicates the number
                                     // of signals to be generated.
    while (signals-- > 0) {
        bcemt_ThreadUtil::microSleep(2 * SLEEP_TIME);
        pthread_kill(info.d_tid, SIGSYS);
        if (verbose) {
            P_T(bcemt_ThreadUtil::self());
            QT(" generated a SIGSYS signal to the thread:");
            PT(info.d_tid);
        }
    }
#endif
    if (!info.d_numConnections) {
        if (verbose) {
            QT("thread exited. ");
        }
        return 0;
    }
    ASSERT(0 == info.d_serverSocket_p->listen(info.d_equeueSize));

    bslma_TestAllocator ThreadTestAllocator;

    bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                               clients(&ThreadTestAllocator);

    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
    int i = 0;
    while(i < info.d_numConnections) {
        bteso_StreamSocket<bteso_IPv4Address> *connection = 0;

        int stat = info.d_serverSocket_p->accept(&connection);
        if (0 == stat) {
            ++i;
            if (veryVerbose) {
                QT("A new connection is established between client "
                   " and server ");
                PT(info.d_serverSocket_p->handle());
            }
            info.d_connList->push_back(connection);
        }
        else {
            if (veryVerbose) {
                QT("Error: generated from connect(),  return value: ");
                PT(stat);
            }
        }
    }
    if (verbose) {
        QT("The server thread exited. ");
    }
    return 0;
}

#ifdef BSLS_PLATFORM_OS_UNIX

extern "C" {
static void signalHandler(int sig)
    // The signal handler does nothing.
{
     if (veryVerbose) {
         write(2,"caught signal\n", sizeof(" caught signal\n"));
     }
     return;
}

static void registerSignal(int signo, void (*handler)(int) )
    // Register the signal handler for the signal 'signo' to be generated.
{
    struct sigaction act, oact;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(signo, &act, &oact) < 0) {
        perror("The signal handler can't be installed.");
    }
    return;
}

}
#endif

static int numChannelToBeEstablished(const TestCommand *commands,
                                     int                numCommands)
    // Return the number of expected channels to be established
    // for a test set.
{
    int total = 0;
    for (int i = 0; i < numCommands; ++i) {
        if ('D' == commands[i].d_commandCode) {
            ++total;
        }
    }
    // The total expected number of channels are those deallocated during
    // the test and those existing right after the test.
    total += commands[numCommands - 1].d_expNumChannels;
    return total;
}

static int testExecutionHelper(btesos_TcpConnector         *connector,
                               int                         *status,
                               const TestCommand           *command,
                               bsl::vector<btesc_Channel*> *channels,
                               btesc_Channel              **newChannel)
    // Process the specified 'command' to invoke some function of the
    // specified 'acceptor'.  If the 'command' is to "allocate" a new channel,
    // the specified 'status' will be passed to the "allocate" function and
    // the specified 'newChannel' will be store the value returned.  If the
    // 'command' is to deallocate a channel, the first channel in the
    // array of 'channels' will be deallocated.  Return 0 on success, and
    // a non-zero value otherwise.
{
    int rCode = 0;

    switch (command->d_commandCode) {
    case 'A': {  // an "ACCEPTOR" request
        if (CHANNEL == command->d_channelType) {
            if (INFINITED == command->d_timeout) {
                *newChannel = connector->allocate(status,
                                                  command->d_interruptFlags);
            }
            else {
                ASSERT("Wrong channel type." && 0);
            }
        }
        else {
            if (INFINITED == command->d_timeout) {
                *newChannel = connector->allocateTimed(status,
                                                    command->d_interruptFlags);
            }
            else {
                ASSERT("Wrong channel type." && 0);
            }
        }
    } break;
    case 'D': {
        if (channels->size()) {
            connector->deallocate((*channels)[0]);// This is hard-coded, just
                                                  // to remove a channel.
            channels->erase(channels->begin());
        }
        else {
            rCode = -1;
        }
    } break;
    case 'I': {
        connector->invalidate();
    } break;
    default:
        break;
    }
    return rCode;
}

static
int processTest(btesos_TcpConnector                            *connector,
          bcemt_ThreadUtil::ThreadFunction                      threadFunction,
          bteso_StreamSocket<bteso_IPv4Address>                *serverSocket,
          bsl::vector<btesc_Channel*>                          *channels,
          bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> *connList,
          const TestCommand                                    *commands,
          int                                                   numCommands,
          int                                                   signals,
          int                                                   expNumChannels,
          int                                                   equeueSize)
    // The specified 'numCommands' of test commands will be issued in the
    // specified 'commands' to invoke some function in the specified
    // 'acceptor'.  Each new channel will be added to the array of channels
    // specified as 'channels'.  Create a thread taking the specified
    // 'threadFunction' as the thread function.  The thread will work as a
    // client to submit the expected number of connection requests and/or
    // generate signals if the 'signals' is set.  Results after each
    // test will be compared against those expected which are also specified
    // in the specified 'commands'.  Return 0 on success, and a non-zero
    // value otherwise.
{
    bcemt_ThreadUtil::Handle threadHandle;

    // Create a thread to be a client.
    bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

    ConnectionInfo connectInfo = { tid,
                                   serverSocket,
                                   signals,
                                   expNumChannels,
                                   connList,
                                   equeueSize
                                 };

    bcemt_Attribute attributes;
    int ret = bcemt_ThreadUtil::create(&threadHandle, attributes,
                                       threadFunction, &connectInfo);
    ASSERT(0 == ret);
    if (ret) {
        if (verbose) {
            QT("Thread creation failed, return value: ");
            PT(ret);
        }
    }
    if (!signals) {
        bcemt_ThreadUtil::microSleep(2 * SLEEP_TIME);
    }

    for (int i = 0; i < numCommands; i++) { // different test data
        int status = 0;

        btesc_Channel *newChannel = 0;
        ret = testExecutionHelper(connector, &status, &commands[i],
                                  channels, &newChannel);

        if (commands[i].d_validChannel) {
            LOOP_ASSERT(commands[i].d_lineNum, 0 != newChannel);
        }
        else {
            LOOP_ASSERT(commands[i].d_lineNum, 0 == newChannel);
        }

        if (newChannel) {
            channels->push_back(newChannel);
            if (CHANNEL == commands[i].d_channelType) {
                btesos_TcpChannel *c =
                    dynamic_cast<btesos_TcpChannel*>(newChannel);
                LOOP_ASSERT(commands[i].d_lineNum, 0 != c);
            }
            else {
                btesos_TcpTimedChannel *c =
                    dynamic_cast<btesos_TcpTimedChannel*>(newChannel);
                LOOP_ASSERT(commands[i].d_lineNum, 0 != c);
            }
        }

        LOOP_ASSERT(commands[i].d_lineNum, status == commands[i].d_expStatus);
        LOOP_ASSERT(commands[i].d_lineNum, commands[i].d_expNumChannels ==
                                      connector->numChannels());

        status = 0;
        if (veryVerbose) {
            P_T(commands[i].d_expNumChannels);
            PT(connector->numChannels());
        }
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    }
    bcemt_ThreadUtil::join(threadHandle);
    return ret;
}

void generatePattern(char *buffer, int length)
    // Load into the specified 'buffer' a generated data pattern of the
    // specified 'length'.
{
    if (buffer) {
        #ifdef BSLS_PLATFORM_OS_UNIX
        snprintf(buffer, length, "%d", length);
        #else
        _snprintf(buffer, length, "%d", length);
        #endif
    }
    return;
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test        = argc > 1 ? atoi(argv[1]) : 0;
    verbose         = argc > 2; // global variable
    veryVerbose     = argc > 3; // global variable
    veryVeryVerbose = argc > 4; // global variable

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    testAllocator.setNoAbort(1);
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    #ifdef BSLS_PLATFORM_OS_UNIX
    registerSignal(SIGSYS, signalHandler);
    #endif

    switch (test) { case 0:
      case 24: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   The following usage example shows a possible implementation of
        //   a single-user echo server.  An echo server accepts a connection
        //   and sends back any received data back to the client (until the
        //   connection is terminated).  This server requires that data is
        //   read from an accepted connection within certain time interval
        //   and be dropped on timeout.
        //   USAGE TEST - Make sure main usage example compiles and works.
        //                TBD.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            enum { ECHO_PORT = 1888 };
            enum {
                NUM_PACKETS = 5,
                PACKET_SIZE = 10
            }; // TCP/IP over Ethernet

            const char *serverIP = 0;
            if (argc > 2) {
                    serverIP = argv[2];
                    // veryVeryVerbose = 0;
                    // globalVeryVeryVerbose = veryVeryVerbose;
            }
            else {
                serverIP = "127.0.0.1";        // assume local host
            }

            bteso_IPv4Address serverAddress(serverIP, ECHO_PORT);

            bdet_TimeInterval connectTimeout(120, 0);   // 2 minutes
            btesos_TcpConnector connector(&factory);//Use default allocator
            ASSERT(0 == connector.isInvalid());
            connector.setPeer(serverAddress);    // 'serverAddress' is valid.

            ASSERT(0 == connector.isInvalid());
            enum { READ_SIZE = 10 };                // only for demo
            bdet_TimeInterval readTimeout(1.0);     // 1 second
            bdet_TimeInterval writeTimeout(30.0);   // 30 seconds
            char controlPacket[PACKET_SIZE];
            char inputPacket[PACKET_SIZE];
            generatePattern(inputPacket, PACKET_SIZE);
            memcpy(controlPacket, inputPacket, PACKET_SIZE);
            int status;
            btesc_TimedChannel *channel = connector.allocateTimed(&status);

            if (!channel) {
                ASSERT(0 >= status);    // Async interrupts are *not* enabled.
                if (verbose) bsl::cout << "Failed to connect to the peer."
                                       << bsl::endl;
                // In any case, invalidate the allocator, and exit.
                connector.invalidate();
                return -1;
            }
            else {
                if (verbose) {
                    cout << " A channel is established." << endl;
                }
            }
            ASSERT(0 == channel->isInvalid());
            char receivedPacket[PACKET_SIZE];
            for (int i = 0; i < NUM_PACKETS; ++i) {
                 // Request/response mechanism
                int writeStatus =
                   channel->timedWrite(inputPacket,
                                       PACKET_SIZE,
                                       bdetu_SystemTime::now() + writeTimeout);
                if (PACKET_SIZE != writeStatus) {
                  if (verbose) cout << "Failed to send data, writeStatus =  "
                                    << writeStatus << bsl::endl;
                    break;
                }
                else {
                    if (verbose) cout << "writeStatus =  "
                                      << writeStatus << endl;
                }

                int readStatus = channel->timedRead(
                                        receivedPacket,
                                        PACKET_SIZE,
                                        bdetu_SystemTime::now() + readTimeout);
                if (PACKET_SIZE != readStatus) {
                    if (verbose) cout << "Failed to read data, readStatus = "
                                      << readStatus << bsl::endl;
                    break;
                }
                else {
                    if (verbose) cout << "readStatus = " << readStatus << endl;
                }
                ASSERT(0 == memcmp(receivedPacket, controlPacket,PACKET_SIZE));
            }

            // Perform proper shut down procedure
            channel->invalidate();
            connector.deallocate(channel);
        }
      } break;

      case 6: {
          // ----------------------------------------------------------------
          // TESTING MANIPULATOR METHODS:
          //   The main concerns about this function is that (1) if it can
          //   delete a channel as expected; (2) if it will not affect other
          //   existing channels; (3) if the 'connector' can still establish
          //   new channels with any "allocate" function.
          //
          // Plan:
          //   Build a set 'DATA' of 'TestCommand', where each entry
          //   specifies: (1) which function to be invoked; (2) what
          //   arguments to be passed for this call and (3) the results
          //   expected to be returned.  A thread is created to act as a
          //   server for the test.  The server will accept the expected
          //   number of connection requests to the main thread if specified.
          //   From the list of test commands, a specified number of channels
          //   will be established first, then try deallocating some
          //   channels out of those existing ones, and also verify new
          //   channels can be established after that.  For the set of test
          //   data, set a different server via 'setPeer' and repeat the test
          //   the specified of 'NUM_PEERS' times.  At last invalidate
          //   the 'connector' and try allocating new channels, which
          //   should fail each time thereafter.
          //
          // Testing:
          //   int deallocate();
          //   int setPeer();
          //   void invalidate();
          // ----------------------------------------------------------------
          if (verbose) {
              QT("Testing Testing MANIPULATOR methods");
              QT("===================================");
          }

          {
              struct {
                  int                  d_lineNum;
                  bslma_TestAllocator *d_allocator_p;// memory allocator
                  int                  d_capacity;   // initial capacity: not
                                                     // specified if 0
                  int                  d_queueSize;  // a server socket's
                                                     // back-log value
              } VALUES[] =
                //line        allocator    capacity   queueSize
                //----        ---------    --------   ---------
              {
                {  L_,     &testAllocator,     7,          128 },
              };

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                              connList(&testAllocator);
                  Obj connector(&factory, VALUES[i].d_allocator_p);

                  int length = 0;  // the existing number of channels
                  ASSERT(0 == connector.isInvalid());
                  if (verbose) {
                      QT("Testing 'deallocate' and 'setPeer' method");
                      QT("=========================================");
                  }

                  enum { NUM_PEERS = 4 };
                  for (int j = 0; j < NUM_PEERS; ++j) {
                      bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                           factory.allocate();
                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);
                      ASSERT(0 == serverSocket->bind(serverAddress));

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      bdet_TimeInterval timeout(0, 5), time(60, 0);
                      int non_interrupt = 0,
                      interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,      1,    length + 1 },

  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,      1,    length + 2 },

  // Multiple channels can be established.
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 3 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 4 },

  // Now deallocate a channel.
  {L_, 'D',   CHANNEL, non_interrupt,  INFINITED,   0,      0,    length + 3 },
  {L_, 'D', T_CHANNEL, non_interrupt,  INFINITED,   0,      0,    length + 2 },

  // Establish new channels after the above deallocate.
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 3 },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 4 },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 5 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 6 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 7 },
  /*
  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'I', T_CHANNEL, non_interrupt,  INFINITED,   0,      0,             7 },
  {L_, 'A', T_CHANNEL, interruptible,  INFINITED,  -4,      0,             7 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,      0,             7 },
  */
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA) - length;

                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));

                     if (veryVerbose) {
                         PT(channels.size()); PT(connector.numChannels());
                     }
                     length = connector.numChannels();
                     if (verbose) {
                         PT(connector.numChannels());
                     }

                     factory.deallocate(serverSocket);
                 }
                 if (verbose) {
                     QT("Testing 'invalidate' method");
                     QT("===========================");
                 }

                 {
                     bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      ASSERT(0 == serverSocket->bind(serverAddress));

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));
                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());
                      bdet_TimeInterval timeout(0, 5), time(60, 0);
                      int non_interrupt = 0,
                      interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,      1,    length + 1 },

  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,      1,    length + 2 },

  // Multiple channels can be established.
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 3 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 4 },

  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 5 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,    length + 6 },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'I', T_CHANNEL, non_interrupt,  INFINITED,   0,      0,    length + 6 },
  {L_, 'A',   CHANNEL, interruptible,  INFINITED,  -4,      0,    length + 6 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,      0,    length + 6 },
  {L_, 'A',   CHANNEL, interruptible,  INFINITED,  -4,      0,    length + 6 },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,      0,    length + 6 },
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA) - length;

                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));

                      if (veryVerbose) {
                          PT(channels.size()); PT(connector.numChannels());
                      }
                      length = connector.numChannels();
                      if (verbose) {
                          PT(connector.numChannels());
                      }
                      factory.deallocate(serverSocket);
                  }
                  length = connList.size();
                  for (int j = 0; j < length; ++j) {
                      factory.deallocate(connList[j]);
                  }
                  connList.clear();
              }
          }
      } break;
      case 5: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
          // ----------------------------------------------------------------
          // TESTING 'allocateTimed' METHOD:
          //
          // Concerns:
          //   The main concerns about this function are that if it can
          //     1. establish a channel of the expected type, i.e., the new
          //        established channel should be of type
          //        'btesos_TcpTimedChannel';
          //     2. establish multiple channels;
          //     3. establish new channels after a channel is deallocated,
          //        and the other channel is not affected;
          //     4. work fine after calls to other "allocate" functions;
          //     5. return the expected 'status' value and the 'null' channel
          //        when an "asynchronous event" happens and this function
          //        is called in the "interruptible" mode;
          //     6. keep trying to establish a new connection until one is
          //        established or an error happens when an "asynchronous
          //        events" happens and the function is called in the
          //        "non_interruptible" mode;
          //     7. establish new channels after the previous request is
          //        interrupted by an "asynchronous event";
          //     8. return the expected 'status' value and the 'null' channel
          //        when an error occurs;
          //     9. not establish any new channels after the 'connector' is
          //        invalidated;
          //
          // Plan:
          //   After a 'btesos_TcpConnector' object 'connector' is
          //   created, four steps are involved to test this function:
          //
          //   Step 1: (for concern 1 - 4)
          //     Build a set 'DATA' of 'TestCommand', where each entry
          //     specifies: (1) which function to be invoked; (2) what
          //     arguments to be passed for this call and (3) the results
          //     expected to be returned.  A thread is created to act as a
          //     server for the test.
          //
          //   Step 2: (concern 5 - 6)
          //     To test how the 'connector' handle "asynchronous events",
          //     make the 'connector' try allocating a connection with a
          //     server outside the company (i.e., outside the firewall).
          //     Create a thread to generate signals  and deliver to the main
          //     thread.  The result from result will be compared against
          //     those expected.  Note that concern(6) can only be tested at
          //     this moment by waiting util the system call timeout--thus an
          //     error occurs and the "allocate" request is stopped.
          //
          //   Step 3: (for concern 7)
          //     Reset the 'connector's server address to a valid one, try
          //     establishing the expected number of connections.  Then
          //     close the server to simulate the scenario that
          //     a "hard" error happens from calls to an "allocate"
          //     function.  Build a set 'DATA' of 'TestCommand', where
          //     each entry will issue a request to establish a connection.
          //     A thread will be created as the server to establish a
          //     connection while no connection is expected to be established.
          //
          //   Step 4: (for concern 8)
          //     Reset the 'connector's server address to a valid one but
          //     don't make the server be listening to simulate the scenario
          //     that a "hard" error happens from calls to an "allocate"
          //     function.  Build a set 'DATA' of 'TestCommand', where
          //     each entry will issue a request to establish a connection.
          //     A thread will be created as the server to establish
          //     a connection,  while no connection is expected to be
          //     established.
          //
          //   Step 5: (for concern 9)
          //     Restart the server for the 'connector' with a valid
          //     server address.  Build a set 'DATA' of 'TestCommand' to
          //     establish some connections first and then invalidate the
          //     'connector'.  No any new connections are supposed
          //     to be established after the 'connector' is invalidated.
          //
          // Testing
          //   int allocateTimed();
          // ----------------------------------------------------------------

          if (verbose) {
              QT("Testing 'allocateTimed' method");
              QT("==============================");
          }

          {
              struct {
                  int                  d_lineNum;
                  bslma_TestAllocator *d_allocator_p;// memory allocator
                  int                  d_capacity;   // initial capacity: not
                                                     // specified if 0
                  int                  d_queueSize;  // a server socket's
                                                     // back-log value
              } VALUES[] =
                //line         allocator   capacity   queueSize
                //----         ---------   --------   ---------
              {
                {  L_,            0,          0,           16 },
              };
              // Register a signal handler for "SIGSYS".
              //registerSignal(SIGSYS, signalHandler);

              bdet_TimeInterval time(60, 10000000);
              time += bdetu_SystemTime::now();

              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

              for (int i = 0; i < NUM_VALUES; i++) { // different connectors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                              connList(&testAllocator);
                  Obj connector(&factory, VALUES[i].d_allocator_p);
                  bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                  int existing = 0;
                  if (verbose) {
                      QT("Step 1 for testing 'allocateTimed' method:");
                      QT("==========================================");
                  }

                  {
                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      ASSERT(0 == serverSocket->bind(serverAddress));

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);
                      //connector.setPeer(serverAddress);
                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A', T_CHANNEL,  interruptible,  INFINITED,   0,      1,         1   },

  // Submit more "allocate" requests to establish multiple channels (concern
  // 2).
  {L_, 'A', T_CHANNEL,  non_interrupt,  INFINITED,   0,      1,         2   },
  {L_, 'A', T_CHANNEL,  non_interrupt,  INFINITED,   0,      1,         3   },

  // Now deallocate a channel (concern 3).
  {L_, 'D', T_CHANNEL, non_interrupt,   INFINITED,   0,      0,         2   },

  // More channels can be established after the deletion (concern 3).
  {L_, 'A', T_CHANNEL,  non_interrupt,  INFINITED,   0,      1,         3   },

  // More channels can be established by other "allocate" functions (concern
  // 4).
  {L_, 'A',   CHANNEL, non_interrupt,   INFINITED,   0,      1,         4   },
  {L_, 'A',   CHANNEL, non_interrupt,   INFINITED,   0,      1,         5   },

  {L_, 'A',   CHANNEL, non_interrupt,   INFINITED,   0,      1,         6   },
  {L_, 'A', T_CHANNEL, non_interrupt,   INFINITED,   0,      1,         7   },
  {L_, 'A', T_CHANNEL, non_interrupt,   INFINITED,   0,      1,         8   },
  {L_, 'A',   CHANNEL, non_interrupt,   INFINITED,   0,      1,         9   },
  {L_, 'A',   CHANNEL, non_interrupt,   INFINITED,   0,      1,        10   },
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 1: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  if (verbose) {
                      QT("Step 2 testing 'allocateTimed' method:");
                      QT("======================================");
                  }
                  {
                      bteso_IPv4Address serverAddress;
                      // Now set the 'connector's server IP address to a host
                      // outside the company.
                      serverAddress.setIpAddress(REMOTE_HOST);
                      serverAddress.setPortNumber(REMOTE_PORT_NUMBER);

                      connector.setPeer(serverAddress);
                      ASSERT(0 == connector.isInvalid());
                      // NOTICE: if more test data are added, the thread
                      // function 'threadAsServer' need to be changed
                      // accordingly, the test will fail otherwise.
                      TestCommand DATA[] =
// ===================>
#ifdef BSLS_PLATFORM_OS_UNIX
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  // New channels can still be established: concern (5).
  {L_, 'A', T_CHANNEL,  interruptible, INFINITED,  1,         0,     existing},
#if defined(BSLS_PLATFORM_OS_SOLARIS) ||  \
  defined(BSLS_PLATFORM_OS_AIX)
  // Solaris and AIX will incorrectly return very quickly from this
  // call to connect.  The other platforms would timeout at some point but
  // it would have taken a very long time.
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED, -3,         0,     existing},
#endif
};
#else
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED, -3,         0,     existing},
};
#endif
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      int signals = 3;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      if (veryVerbose) {
                          QT("Step 2: channels.size() = ");
                          PT(channels.size());
                      }
                      existing = channels.size();
                  }
                  if (verbose) {
                      QT("Step 3 testing 'allocateTimed' method:");
                      QT("======================================");
                  }

                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      int ret= serverSocket->bind(serverAddress);
                      ASSERT(0 == ret);

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A', T_CHANNEL, interruptible,  INFINITED,   0,     1,    existing + 1},
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 2},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 3},
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 4},
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 5},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 3: channels.size() = ");
                          PT(channels.size());
                      }
                  }

                  if (verbose) {
                      QT("Step 4 testing 'allocateTimed' method:");
                      QT("======================================");
                  }

                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER + 5000);

                      connector.setPeer(serverAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit 'allocate' requests, which are supposed to fail.
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  -3,       0,      existing},
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  -3,       0,      existing},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 4: channels.size() = ");
                          PT(channels.size());
                      }
                  }

                  if (verbose) {
                      QT("Step 5 testing 'allocateTimed' method:");
                      QT("======================================");
                  }

                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      int ret= serverSocket->bind(serverAddress);
                      ASSERT(0 == ret);

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A', T_CHANNEL, interruptible,  INFINITED,   0,     1,    existing + 1},
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 2},
  {L_, 'I', T_CHANNEL, non_interrupt,  INFINITED,   0,     0,    existing + 2},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,     0,    existing + 2},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,     0,    existing + 2},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 5: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  channels.clear();

                  int length = connList.size();
                  if (verbose) {
                      PT(connList.size());
                  }

                  for (int j = 0; j < length; ++j) {
                      factory.deallocate(connList[j]);
                  }
                  factory.deallocate(serverSocket);
              }
          }
#endif
      } break;
      case 4: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
          // ----------------------------------------------------------------
          // TESTING 'allocate' METHOD:
          //
          // Concerns:
          //   The main concerns about this function are that if it can
          //     1. establish a channel of the expected type, i.e., the new
          //        established channel should be of type
          //        'btesos_TcpChannel';
          //     2. establish multiple channels;
          //     3. establish new channels after a channel is deallocated,
          //        and the other channel is not affected;
          //     4. work fine after calls to other "allocate" functions;
          //     5. return the expected 'status' value and the 'null' channel
          //        when an "asynchronous event" happens and this function
          //        is called in the "interruptible" mode;
          //     6. keep trying to establish a new connection until one is
          //        established or an error happens when an "asynchronous
          //        events" happens and the function is called in the
          //        "non_interruptible" mode;
          //     7. establish new channels after the previous request is
          //        interrupted by an "asynchronous event";
          //     8. return the expected 'status' value and the 'null' channel
          //        when an error occurs;
          //     9. not establish any new channels after the 'connector' is
          //        invalidated;
          //
          // Plan:
          //   After a 'btesos_TcpConnector' object 'connector' is
          //   created, four steps are involved to test this function:
          //
          //   Step 1: (for concern 1 - 4)
          //     Build a set 'DATA' of 'TestCommand', where each entry
          //     specifies: (1) which function to be invoked; (2) what
          //     arguments to be passed for this call and (3) the results
          //     expected to be returned.  A thread is created to act as a
          //     server for the test.
          //
          //   Step 2: (concern 5 - 6)
          //     To test how the 'connector' handle "asynchronous events",
          //     make the 'connector' try allocating a connection with a
          //     server outside the company (i.e., outside the firewall).
          //     Create a thread to generate signals  and deliver to the main
          //     thread.  The result from result will be compared against
          //     those expected.  Note that concern(6) can only be tested at
          //     this moment by waiting util the system call timeout--thus an
          //     error occurs and the "allocate" request is stopped.
          //
          //   Step 3: (for concern 7)
          //     Reset the 'connector's server address to a valid one, try
          //     establishing the expected number of connections.  Then
          //     close the server to simulate the scenario that
          //     a "hard" error happens from calls to an "allocate"
          //     function.  Build a set 'DATA' of 'TestCommand', where
          //     each entry will issue a request to establish a connection.
          //     A thread will be created as the server to accept a connection
          //     while no connection is expected to be established.
          //
          //   Step 4: (for concern 8)
          //     Reset the 'connector's server address to a valid one but
          //     don't make the server be listening to simulate the scenario
          //     that a "hard" error happens from calls to an "allocate"
          //     function.  Build a set 'DATA' of 'TestCommand', where
          //     each entry will issue a request to establish a connection.
          //     A thread will be created as the server to establish
          //     a connection,  while no connection is expected to be
          //     established.
          //
          //   Step 5: (for concern 9)
          //     Restart the server for the 'connector' with a valid
          //     server address.  Build a set 'DATA' of 'TestCommand' to
          //     establish some connections first and then invalidate the
          //     'connector'.  No any new connections are supposed
          //     to be established after the 'connector' is invalidated.
          //
          // Testing
          //   int allocate();
          // ----------------------------------------------------------------

          if (verbose) {
              QT("Testing 'allocate' method");
              QT("=========================");
          }
          {
              struct {
                  int                  d_lineNum;
                  bslma_TestAllocator *d_allocator_p;// memory allocator
                  int                  d_capacity;   // initial capacity: not
                                                     // specified if 0
                  int                  d_queueSize;  // a server socket's
                                                     // back-log value
              } VALUES[] =
                //line         allocator   capacity   queueSize
                //----         ---------   --------   ---------
              {
                {  L_,            0,          0,           16 },
              };
              // Register a signal handler for "SIGSYS".
              //registerSignal(SIGSYS, signalHandler);

              bdet_TimeInterval time(60, 10000000);
              time += bdetu_SystemTime::now();

              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

              for (int i = 0; i < NUM_VALUES; i++) { // different connectors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                              connList(&testAllocator);
                  Obj connector(&factory, VALUES[i].d_allocator_p);
                  bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                  int existing = 0;
                  if (verbose) {
                      QT("Step 1 for testing 'allocate' method:");
                      QT("=====================================");
                  }

                  {
                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      ASSERT(0 == serverSocket->bind(serverAddress));

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A',  CHANNEL,  interruptible,  INFINITED,   0,      1,         1   },

  // Submit more "allocate" requests to establish multiple channels (concern
  // 2).
  {L_, 'A',  CHANNEL,  non_interrupt,  INFINITED,   0,      1,         2   },
  {L_, 'A',  CHANNEL,  non_interrupt,  INFINITED,   0,      1,         3   },

  // Now deallocate a channel (concern 3).
  {L_, 'D',   CHANNEL, non_interrupt,  INFINITED,   0,      0,         2   },

  // More channels can be established after the deletion (concern 3).
  {L_, 'A',  CHANNEL,  non_interrupt,  INFINITED,   0,      1,         3   },

  // More channels can be established by other "allocate" functions (concern
  // 4).
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,         4   },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,         5   },

  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,         6   },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,      1,         7   },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,      1,         8   },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,         9   },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,      1,        10   },
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 1: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  if (verbose) {
                      QT("Step 2 for testing 'allocate' method:");
                      QT("=====================================");
                  }

                  {
                      bteso_IPv4Address serverAddress;
                      // Now set the 'connector's server IP address to a host
                      // outside the company.
                      serverAddress.setIpAddress(REMOTE_HOST);
                      serverAddress.setPortNumber(REMOTE_PORT_NUMBER);

                      connector.setPeer(serverAddress);
                      ASSERT(0 == connector.isInvalid());
                      // NOTICE: if more test data are added, the thread
                      // function 'threadAsServer' need to be changed
                      // accordingly, the test will fail otherwise.
                      TestCommand DATA[] =
// ===================>
#ifdef BSLS_PLATFORM_OS_UNIX
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  // New channels can still be established: concern (5).
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,  1,         0,     existing},
#if defined(BSLS_PLATFORM_OS_SOLARIS) ||  \
  defined(BSLS_PLATFORM_OS_AIX)
  // Solaris and AIX will incorrectly return very quickly from this
  // call to connect.  The other platforms would timeout at some point but
  // it would have taken a very long time.
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED, -3,         0,     existing},
#endif
};
#else
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED, -3,         0,     existing},
};
#endif
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      int signals = 3;  // number of signals
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      if (veryVerbose) {
                          QT("Step 2: channels.size() = ");
                          PT(channels.size());
                      }
                      existing = channels.size();
                  }
                  if (verbose) {
                      QT("Step 3 for testing 'allocate' method:");
                      QT("=====================================");
                  }
                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      int ret= serverSocket->bind(serverAddress);
                      ASSERT(0 == ret);

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A',   CHANNEL, interruptible,  INFINITED,   0,     1,    existing + 1},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 2},
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 3},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 4},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 5},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 3: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  if (verbose) {
                      QT("Step 4 for testing 'allocate' method:");
                      QT("=====================================");
                  }
                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;

                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER + 5000);

                      connector.setPeer(serverAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit 'allocate' requests, which are supposed to fail.
  {L_, 'A',   CHANNEL,  non_interrupt, INFINITED,  -3,       0,      existing},
  {L_, 'A',   CHANNEL,  non_interrupt, INFINITED,  -3,       0,      existing},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 4: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  if (verbose) {
                      QT("Step 5 for testing 'allocate' method:");
                      QT("=====================================");
                  }

                  {
                      factory.deallocate(serverSocket);
                      serverSocket = factory.allocate();

                      bteso_IPv4Address serverAddress, actualAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      int ret= serverSocket->bind(serverAddress);
                      ASSERT(0 == ret);

                      ASSERT(0 == serverSocket->localAddress(&actualAddress));

                      connector.setPeer(actualAddress);

                      ASSERT(0 == connector.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
  // Submit an "allocate" request to establish one channel (concern 1).
  {L_, 'A',   CHANNEL, interruptible,  INFINITED,   0,     1,    existing + 1},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,     1,    existing + 2},
  {L_, 'I',   CHANNEL, non_interrupt,  INFINITED,   0,     0,    existing + 2},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,     0,    existing + 2},
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -4,     0,    existing + 2},
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                                numChannelToBeEstablished(DATA, NUM_DATA)
                                                                 - existing;
                      int signals = 0;
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 5: channels.size() = ");
                          PT(channels.size());
                      }
                  }
                  channels.clear();

                  int length = connList.size();
                  if (verbose) {
                      QT(connList.size());
                  }

                  for (int j = 0; j < length; ++j) {
                      factory.deallocate(connList[j]);
                  }
                  factory.deallocate(serverSocket);
              }
          }
#endif
      } break;
      case 3: {
          // ----------------------------------------------------------------
          // TESTING BASIC ACCESSORS:
          //
          // Plan:
          //
          // Testing:
          //   const bteso_StreamSocket<bteso_IPv4Address> *socket() const;
          //   const bteso_IPv4Address& address() const;
          //   int isInvalid() const;
          //   int numChannels() const;
          // ----------------------------------------------------------------
          if (verbose) {
              QT("TESTING BASIC ACCESSORS");
              QT("=======================");
          }

          {
              struct {
                  int                  d_lineNum;
                  bslma_TestAllocator *d_allocator_p;// memory allocator
                  int                  d_capacity;   // initial capacity: not
                                                     // specified if 0
                  int                  d_queueSize;  // a server socket's
                                                     // back-log value
              } VALUES[] =
                //line         allocator   capacity   queueSize
                //----         ---------   --------   ---------
              {
                {  L_,            0,          0,           16 },
                {  L_,            0,          0,           32 },
                {  L_,            0,          0,           64 },
                {  L_,            0,          0,          128 },
                {  L_,     &testAllocator,    0,          128 },
                {  L_,     &testAllocator,    0,          128 },
                {  L_,     &testAllocator,    2,          128 },
                {  L_,     &testAllocator,    4,          128 },
                {  L_,            0,          8,          128 },
                {  L_,            0,         16,          128 },
              };

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                              connList(&testAllocator);
                  Obj connector(&factory, VALUES[i].d_allocator_p);
                  bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();
                  LOOP_ASSERT(i, 0 == connector.isInvalid());
                  LOOP_ASSERT(i, 0 == connector.numChannels());

                  bteso_IPv4Address serverAddress, actualAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  ASSERT(0 == serverSocket->bind(serverAddress));

                  ASSERT(0 == serverSocket->localAddress(&actualAddress));
                  // Open the 'connector' to establish a listening server
                  // socket.
                  connector.setPeer(actualAddress);
                  LOOP_ASSERT(i, 0 == connector.isInvalid());
                  int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                  TestCommand DATA[] =
// ===============>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  // Each of the following command is to establish a channel.
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,        1,          1   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,          2   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          3   },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          4   },
  {L_, 'A',   CHANNEL, interruptible, INFINITED,   0,        1,          5   },

  {L_, 'I',   CHANNEL, interruptible, INFINITED,   0,        0,          5   },
};
// ===============>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels =
                        numChannelToBeEstablished(DATA, NUM_DATA);
                  // Ask the thread not to generate any signal.
                  int signals = 0;
                  LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&connector,
                                                   threadAsServer,
                                                   serverSocket,
                                                   &channels,
                                                   &connList,
                                                   DATA,
                                                   NUM_DATA,
                                                   signals,
                                                   expNumChannels,
                                                   VALUES[i].d_queueSize));

                  int length = connList.size();
                  if (veryVerbose) {
                      PT(channels.size());
                  }
                  LOOP_ASSERT(i, 1 == connector.isInvalid());
                  LOOP_ASSERT(i, DATA[NUM_DATA - 1].d_expNumChannels ==
                                                    connector.numChannels());
                  channels.clear();
                  for (int j = 0; j < length; ++j) {
                      factory.deallocate(connList[j]);
                  }
                  factory.deallocate(serverSocket);
              }
          }
      } break;
      case 2: {
          // ----------------------------------------------------------------
          // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
          //   The main concern about this set of functions is that (1) it
          //   can create different 'btesos_TcpConnector' objects
          //   successfully, based on different parameters; (2) if can
          //   destructor any 'btesos_TcpConnector' object successfully.
          // Plan:
          //   Build a set of data 'VALUES' to construct different
          //   'btesos_TcpConnector' objects.  With each objects,
          //   establish a specified number of channels, verify the result
          //   from each channel operation.
          // Testing:
          //   virtual btesos_TcpConnector();
          //   virtual btesos_TcpConnector(..., int initialCapacity, ...);
          //   virtual ~btesos_TcpConnector();
          // ----------------------------------------------------------------
          if (verbose) {
              QT("Testing Primary Manipulator");
              QT("===========================");
          }

          bslma_TestAllocator testAllocator;
          {
              struct {
                  int  d_line;
                  bool d_useAllocator;               // memory allocator
                  int  d_capacity;                   // initial capacity: not
                                                     // specified if 0
                  int  d_queueSize;                  // a server socket's
                                                     // back-log value
              } VALUES[] =
                //line         allocator   capacity   queueSize
                //----         ---------   --------   ---------
              { // Note the 'capacity' can't be 0, or will abort.
                {  L_,        false,          2,           16 },
                {  L_,        false,          2,           32 },
                {  L_,        false,          3,           64 },
                {  L_,        false,          3,          128 },
                {  L_,        true,           4,           64 },
                {  L_,        true,           4,          128 },
                {  L_,        true,           8,          128 },
                {  L_,        true,           8,          128 },
                {  L_,        true,          16,          128 },
                {  L_,        true,          16,          128 },
                {  L_,        false,         32,          128 },
                {  L_,        false,         32,          128 },
              };

              // Register a signal handler for "SIGSYS".
              //registerSignal(SIGSYS, signalHandler);

              bdet_TimeInterval time(60, 10000000);
              time += bdetu_SystemTime::now();

              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
              for (int i = 0; i < NUM_VALUES; i++) { // different connectors
                  const int  LINE          = VALUES[i].d_line;
                  const bool USE_ALLOCATOR = VALUES[i].d_useAllocator;
                  const int  CAPACITY      = VALUES[i].d_capacity;
                  const int  QUEUE_SIZE    = VALUES[i].d_queueSize;

                  bslma_TestAllocator testAllocator(veryVeryVerbose);
                  bslma_TestAllocator *allocator = USE_ALLOCATOR
                                                 ? &testAllocator
                                                 : 0;

                  bsl::vector<btesc_Channel*> channels(allocator);
                  bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                           connList(allocator);
                  Obj connector(&factory, CAPACITY, allocator);
                  bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                         factory.allocate();

                  bteso_IPv4Address serverAddress, actualAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  ASSERT(0 == serverSocket->bind(serverAddress));

                  ASSERT(0 == serverSocket->localAddress(&actualAddress));

                  connector.setPeer(actualAddress);

                  ASSERT(0 == connector.isInvalid());

                  int existing = 0;
                  TestCommand DATA[] =
// ===============>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
  // Each of the following command is to establish a channel.
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          1   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,          2   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          3   },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          4   },
  {L_, 'A',   CHANNEL, interruptible, INFINITED,   0,        1,          5   },

  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          6   },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          7   },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          8   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          9   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         10   },

  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         11   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,         12   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         13   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         14   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,         15   },

  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         16   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,         17   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         18   },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,         19   },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,         20   },
};
// ===============>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels =
                        numChannelToBeEstablished(DATA, NUM_DATA) - existing;

                  // Ask the thread not to generate any signal.
                  int signals = 0;
                  LOOP_ASSERT(LINE,
                              0 == processTest(&connector,
                                               threadAsServer,
                                               serverSocket,
                                               &channels,
                                               &connList,
                                               DATA,
                                               NUM_DATA,
                                               signals,
                                               expNumChannels,
                                               QUEUE_SIZE));
                  int length = channels.size();
                  if (veryVerbose) {
                      PT(channels.size());
                  }
                  channels.clear();

                  for (int j = 0; j < length; ++j) {
                      factory.deallocate(connList[j]);
                  }
                  factory.deallocate(serverSocket);
              }
          }
      } break;
      case 1: {
          // ----------------------------------------------------------------
          // BREATHING TEST:
          //   Exercise a broad cross-section of value-semantic functionality
          //   before beginning testing in earnest.  Probe that functionality
          //   systematically and incrementally to discover basic errors in
          //   isolation.
          //
          // Plan:
          //
          // Testing:
          //   This test case exercises basic value-semantic functionality.
          // ----------------------------------------------------------------
          if (verbose) {
              QT("BREATHING TEST");
              QT("==============");
          }

          {
              bsl::vector<btesc_Channel*> channels(&testAllocator);
              bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                          connList(&testAllocator);
              Obj connector(&factory);
              bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                        factory.allocate();
              {
                  enum { FAILED = -3 };
                  bteso_IPv4Address serverAddress, actualAddress;

                  serverAddress.setPortNumber(REMOTE_PORT_NUMBER);

                  connector.setPeer(serverAddress);
                  ASSERT(0 == connector.isInvalid());
                  int status = 0;

#if 0
                  btesc_Channel *newChannel = connector.allocate(&status);
                  ASSERT(0 == newChannel);
                  ASSERT(FAILED == status);
#endif
                  factory.deallocate(serverSocket);
              }
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
