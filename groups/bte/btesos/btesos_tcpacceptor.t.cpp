// btesos_tcpacceptor.t.cpp      -*-C++-*-

#include <btesos_tcpacceptor.h>

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

#include <bcemt_barrier.h>                  // barrier
#include <bcemt_thread.h>                   // thread management util
#include <bcema_testallocator.h>            // thread-safe allocator
#include <bsl_typeinfo.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#endif

#include <bsl_iostream.h>
#include <bsl_vector.h>

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
// [ 2] virtual btesos_TcpAcceptor()
// [ 2] virtual btesos_TcpAcceptor(..., int initialCapacity, ...)
// [ 2] virtual ~btesos_TcpAcceptor()
// [ 4] btesc_Channel *allocate()
// [ 5] btesc_Channel *allocateTimed()
// [ 6] int open();
// [ 6] virtual void invalidate()
// [ 6] int setOption();
// [ 6] int getOption();
// [ 7] int close();
// [ 7] void deallocate();
// [ 3] const bteso_StreamSocket<bteso_IPv4Address> *socket() const;
// [ 3] const bteso_IPv4Address& address() const;
// [ 3] virtual int isInvalid()
// [ 3] int numChannels() const;
//-----------------------------------------------------------------------------
// [10] USAGE example
// [ 1] BREATHING TEST
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

typedef btesos_TcpAcceptor Obj;
typedef bteso_StreamSocket<bteso_IPv4Address> StreamSocket;

static int globalVerbose;           // == 'verbose', used in help functions.
static int globalVeryVerbose;       // == 'veryVerbose'

const char *HOST_NAME = "127.0.0.1";
const bdet_TimeInterval *INFINITED = 0;

enum {
    DEFAULT_PORT_NUMBER     = 0,
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
     bteso_IPv4Address *d_server;
     int                d_numConnections;   // the maximum number of
                                            // connections to be established
     const struct TestCommand * d_commands; // commands executed by the main
                                            // thread
     int                d_numCommands;     // nb of commands executed
     bcemt_Barrier      * d_barrier; // barrier to sync the two threads
};

struct TestCommand {
    // This struct includes needed information for each test operation,
    // e.g., a request to "ACCEPT",  along with it's corresponding
    // test results.

    int                d_lineNum;

    char               d_commandCode;   // command to invoke a corresponding
                                        // function, e.g., 'C' -- invoke the
                                        // acceptor's close(); 'A' --
                                        // an "allocate" function etc.

    int                d_channelType;   // a request for a 'btesos_TcpChannel'
                                        // or 'btesos_TcpTimedChannel', i.e.,
                                        // 0 for a 'btesos_TcpChannel'
                                        // and 1 for a 'btesos_TcpTimedChannel'

    int                d_interruptFlags;// interruptible or not

    const bdet_TimeInterval *d_timeout; // a flag to indicate if it'll timeout

    int                d_expStatus;    // a expected status value from a
                                       // "CONNECT" request

    int                d_validChannel; // It's 1 if a valid channel is
                                       // returned, and 0 otherwise.
    int                d_expNumChannels;
    int                d_signal; // 1 if we need to signal before trying
                                 // to connect
};

bslma_TestAllocator testAllocator;

//=============================================================================
//                      HELPER FUNCTIONS/CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// signal stuff
#ifdef BSLS_PLATFORM__OS_UNIX
volatile sig_atomic_t syncWithSigHandler = 0;
static void signalHandler(int sig)
    // The signal handler does nothing.
{
     // this is NOT SIGNAL SAFE, but it should not matter if the asser is
     // true
     ASSERT(syncWithSigHandler == 0);

     if (globalVeryVerbose) {
         //P_T(bcemt_ThreadUtil::self());
         // We can only use write in the signal handler ...
         write(1, " caught signal\n", sizeof(" caught signal\n"));
         //PT(sig);
     }
     ++syncWithSigHandler;
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

//}
#endif

// Flag to indicate we're past the accept in the helper function
// If it is 0, the client thread will try to connect
// We need that flag since the socket is always in LISTEN state
// so any attempt to connect to it will succeed even if we are
// not in accept().  So the connection will be backlogged and
// no channels will be allocated (since we're past the allocate())
// leading to false results.
// This part is still racy, but there is no fix (FIXME) at the moment.
// You may need to adjust the sleep times in threadAsClient
// even if it is *very* unlikely
int helperAfterAccept;
#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

void* threadAsClient(void *arg)
    // Taking information passed from the specified 'arg', submit the
    // expected number of connection requests and/or generate signal 'SIGSYS'
    // and deliver it to a thread specified in 'arg'.  Note the test can
    // only work on UNIX platforms since the windows doesn't support signal
    // operations.
{
    const ConnectionInfo & info = *(ConnectionInfo*) arg;
    if (globalVerbose) {
        P_T(*(info.d_server));
        PT(info.d_numConnections);
    }
    bslma_TestAllocator threadTestAllocator;
    bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>
                                                 clients(&threadTestAllocator);
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;

    for (int i = 0 ; i < info.d_numCommands ; ++i) {

        // XXX This is still slightly racy ..
        // we'd like to be sure that the helper thread
        // is in the accept() call, hence the yield
        // and sleep.
        info.d_barrier->wait();
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);

        if (info.d_commands[i].d_signal) {
#ifdef BSLS_PLATFORM__OS_UNIX
            pthread_kill(info.d_tid, SIGSYS);
            if (globalVerbose) {
                P_T(bcemt_ThreadUtil::self());
                QT(" generated a SIGSYS signal to ");
                PT(info.d_tid);
            }
            // reasonable spinning
            while (syncWithSigHandler == 0)
                bcemt_ThreadUtil::microSleep(SLEEP_TIME);
            ASSERT(syncWithSigHandler == 1);
            syncWithSigHandler = 0;
#endif
            // XXX RACE AGAIN, but we do not have any choice
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(SLEEP_TIME);
        }

        // XXX at this point if the helper thread has left accept
        // BUT has not set helperAfterAccept to 1, we will get
        // false results... You may need to adjust the sleeping time above
        if (info.d_commands[i].d_commandCode == 'A'
            && 0 == helperAfterAccept) {

            bteso_StreamSocket<bteso_IPv4Address>
                *d_connectingSocket_p = factory.allocate();

            ASSERT(0 != d_connectingSocket_p);
            int s = d_connectingSocket_p->connect(*(info.d_server));

            if (0 == s) {
                if (globalVeryVerbose) {
                    QT("A new connection is established.");
                    P_T(d_connectingSocket_p->handle());
                    PT(*(info.d_server));
                }
                clients.push_back(d_connectingSocket_p);
            } else {
                if (globalVeryVerbose) {
                    QT("No connection could be established.");
                    P_T(d_connectingSocket_p->handle());
                    PT(*(info.d_server));
                }
                factory.deallocate(d_connectingSocket_p);
            }
        }
        if (globalVeryVerbose) {
                QT("Waiting for server thread");
        }
        info.d_barrier->wait();
        if (globalVeryVerbose) {
                QT("End of interation");
        }
    }

    // cleanup
    int length = clients.size();
    for (int i = 0; i < length; ++i) {
        factory.deallocate(clients[i]);
    }
    if (globalVerbose) {
        QT("The client thread exited. ");
    }
    return 0;
}

#ifdef BSLS_PLATFORM__OS_SOLARIS
static void* threadToCloseServer(void *arg)
{
    StreamSocket *serverSocket = (StreamSocket*) arg;

    if (!serverSocket) {
        if (globalVerbose) {
            QT("Error happens when the server socket to the thread.");
        }
        return 0;
    }
    bcemt_ThreadUtil::microSleep(3 * SLEEP_TIME);

    int ret = bteso_SocketImpUtil::close(serverSocket->handle());

    ASSERT(0 == ret);
    if (globalVerbose) {
        QT("ThreadToCloseServer exits.");
    }
    return 0;
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

static int testExecutionHelper(btesos_TcpAcceptor          *acceptor,
                               int                         *status,
                               const TestCommand           *command,
                               bsl::vector<btesc_Channel*> *channels,
                               btesc_Channel              **newChannel,
                               bcemt_Barrier               *syncBarrier)
    // Process the specified 'command' to invoke some function of the
    // specified 'acceptor'.  If the 'command' is to "allocate" a new channel,
    // the specified 'status' will be passed to the "allocate" function and
    // the specified 'newChannel' will be store the value returned.  If the
    // 'command' is to deallocate a channel, the first channel in the
    // array of 'channels' will be deallocated.  Return 0 on success, and
    // a non-zero value otherwise.
{
    int rCode = 0;

    if (globalVeryVerbose) {
            QT("Synchronizing before accept");
    }
    helperAfterAccept = 0;
    // XXX attempts to synchronize with the client thread
    // we want to be the accept() call when/if a signal
    // is launched to get an interrupted system call
    syncBarrier->wait();
    switch (command->d_commandCode) {
    case 'A': {  // an "ACCEPTOR" request
        if (CHANNEL == command->d_channelType) {
            if (INFINITED == command->d_timeout) {
                *newChannel = acceptor->allocate(status,
                                                 command->d_interruptFlags);
                // XXX this is racy, but we do not have any choice
                helperAfterAccept = 1;
            }
            else {
                ASSERT("Wrong channel type." && 0);
            }
        }
        else {
            if (INFINITED == command->d_timeout) {
                *newChannel = acceptor->allocateTimed(status,
                                                    command->d_interruptFlags);
                // XXX this is racy, but we do not have any choice
                helperAfterAccept = 1;
            }
            else {
                ASSERT("Wrong channel type." && 0);
            }
        }
    } break;
    case 'C': {
        rCode = acceptor->close();
        if (0 == rCode) {
            ASSERT(acceptor->address() == bteso_IPv4Address());
        }
    } break;
    case 'D': {
        if (channels->size()) {
            acceptor->deallocate((*channels)[0]); // This is hard-coded, just
                                                  // to remove a channel.
            channels->erase(channels->begin());
        }
        else {
            rCode = -1;
        }
    } break;
    case 'I': {
        acceptor->invalidate();
    } break;
    default:
        break;
    }

    if (globalVeryVerbose) {
            QT("Waiting for client thread");
    }
    syncBarrier->wait();
    if (globalVeryVerbose) {
            QT("End of server iteration");
    }
    return rCode;
}

static
int processTest(btesos_TcpAcceptor               *acceptor,
                bcemt_ThreadUtil::ThreadFunction  threadFunction,
                bsl::vector<btesc_Channel*>      *channels,
                const TestCommand                *commands,
                int                               numCommands,
                int                               expNumChannels)
    // The specified 'numCommands' of test commands will be issued in the
    // specified 'commands' to invoke some function in the specified
    // 'acceptor'.  Each new channel will be added to the array of channels
    // specified as 'channels'.  Create a thread taking the specified
    // 'threadFunction' as the thread function.  The thread will work as a
    // client to submit the expected number of connection requests and/or
    // generate signals if d_signal is set.  Results after each
    // test will be compared against those expected which are also specified
    // in the specified 'commands'.  Return 0 on success, and a non-zero
    // value otherwise.
{
    bteso_IPv4Address serverAddr(acceptor->address());

    bcemt_ThreadUtil::Handle threadHandle;

    // Create a thread to be a client.
    bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

    bcemt_Barrier syncBarrier(2);
    ConnectionInfo connectInfo = { tid,
                                   &serverAddr,
                                   expNumChannels,
                                   commands,
                                   numCommands,
                                   &syncBarrier
                                 };

    bcemt_Attribute attributes;
    int ret = bcemt_ThreadUtil::create(&threadHandle, attributes,
                                       threadFunction, &connectInfo);
    ASSERT(0 == ret);
    if (ret) {
        if (globalVerbose) {
            QT("Thread creation failed, return value: ");
            PT(ret);
        }
    }
    for (int i = 0; i < numCommands; i++) { // different test data
        int status = 0;

        btesc_Channel *newChannel = 0;

        ret = testExecutionHelper(acceptor, &status, &commands[i],
                                  channels, &newChannel, &syncBarrier);

        if (commands[i].d_validChannel) {
            LOOP_ASSERT(commands[i].d_lineNum, 0 != newChannel);
        }
        else {
            LOOP_ASSERT(commands[i].d_lineNum, 0 == newChannel);
        }

        if (newChannel) {
            channels->push_back(newChannel);
            if (CHANNEL == commands[i].d_channelType) {
              // LOOP_ASSERT(commands[i].d_lineNum, typeid(*newChannel) ==
              //              typeid(btesos_TcpChannel));
            }
            else {
              //  LOOP_ASSERT(commands[i].d_lineNum, typeid(*newChannel) ==
              //              typeid(BloombergLP::btesos_TcpTimedChannel));
            }
        }

        LOOP_ASSERT(commands[i].d_lineNum, status == commands[i].d_expStatus);

        LOOP_ASSERT(commands[i].d_lineNum, commands[i].d_expNumChannels ==
                                                  acceptor->numChannels());
        status = 0;
        if (globalVeryVerbose) {
            P_T(commands[i].d_expNumChannels);
            PT(acceptor->numChannels());
        }
    }
    bcemt_ThreadUtil::join(threadHandle);
    return ret;
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;         globalVerbose = verbose;
    int veryVerbose = argc > 3;     globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    testAllocator.setNoAbort(1);
    testAllocator.setVerbose(veryVeryVerbose);
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    #ifdef BSLS_PLATFORM__OS_UNIX
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
            enum {
                ECHO_PORT = 1888,
                QUEUE_SIZE = 32
            };
            bteso_IPv4Address serverAddress;
            serverAddress.setPortNumber(ECHO_PORT);
            btesos_TcpAcceptor acceptor(&factory); // default allocator.
            ASSERT(0 == acceptor.isInvalid());
            if (0 != acceptor.open(serverAddress, QUEUE_SIZE)) {
               if (verbose) {
                   cout << "Can't open listening socket" << bsl::endl;
               }
               break; // return -1;
            }
            ASSERT(acceptor.address() == serverAddress);
            enum { READ_SIZE = 10 };
            // Note: this is OK *if and only if* it is in the 'main' function.
            bdet_TimeInterval readTimeout(30, 0);  // 30 seconds
            bdet_TimeInterval writeTimeout(5, 0);  // 5 seconds
            while (0 == acceptor.isInvalid()) {
                int status;
                btesc_TimedChannel *channel =
                    acceptor.allocateTimed(&status);
                if (channel) {
                    while(1) {
                         char result[READ_SIZE];
                         int readStatus =
                             channel->read(result, READ_SIZE);
                         if (0 >= readStatus) {
                             if (verbose) {
                                 cout << "Failed to read data, readStatus = "
                                      << readStatus << endl;
                             }
                             break;
                         }
                         else {
                             if (verbose) {
                                 cout << "readStatus = " << readStatus << endl;
                             }
                         }
                         int ws =
                             channel->timedWrite(result,
                                                 readStatus,
                                                 writeTimeout);
                         if (readStatus != ws) {
                             if (verbose) {
                                 cout << "Failed to send data, writeStatus = "
                                      << ws << endl;
                             }
                             break;
                         }
                         else {
                             if (verbose) {
                                 cout << "writeStatus = " << ws << endl;
                             }
                         }
                    }
                    acceptor.deallocate(channel);
                }
                else {
                     ASSERT(status <= 0);   // Interrupts are not enabled.
                     if (0 == status) {
                         if (verbose) {
                             cout << "Timed out accepting a connection"
                                  << endl;
                         }
                     }
                }
            }
            ASSERT(acceptor.isInvalid());
            ASSERT(0 == acceptor.close());
        }
      } break;
      case 7: {
          // ----------------------------------------------------------------
          // TESTING 'deallocate' and 'close' METHOD:
          //   The main concerns about this function is that (1) if it can
          //   delete a channel as expected; (2) if it will not affect other
          //   existing channels; (3) if the 'acceptor' can still establish
          //   new channels with any "allocate" function.
          //
          // Plan:
          //   Build a set 'DATA' of 'TestCommand', where each entry
          //   specifies: (1) which function to be invoked; (2) what
          //   arguments to be passed for this call and (3) the results
          //   expected to be returned.  A thread is created to act as a
          //   client for the test.  The client will submit the expected
          //   number of connection requests and/or generate signals
          //   to the main thread if specified.  From the list of test
          //   commands, a specified number of channels will be established
          //   first, then try deallocating some channels out of those
          //   existing ones, and also verify new channels can be
          //   established after that.  At last close the 'acceptor' and
          //   try allocating new channels, which should fail.
          //
          // Testing:
          //   int deallocate();
          //   int close();
          // ----------------------------------------------------------------
          if (verbose) {
              QT("Testing 'deallocate' and 'close' method");
              QT("=======================================");
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
                {  L_,            0,          0,          128 },
                {  L_,            0,          8,          128 },
                {  L_,     &testAllocator,    2,          128 },
                {  L_,     &testAllocator,    7,          128 },
              };

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  ASSERT(0 == acceptor.isInvalid());

                  bteso_IPv4Address serverAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  // Open the 'acceptor' to establish a listening server
                  // socket.
                  int ret = acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize);
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 != acceptor.socket());
                  LOOP_ASSERT(i, 0 != acceptor.address().portNumber());

                  bdet_TimeInterval timeout(0, 5), time(60, 0);
                  int non_interrupt = 0,
                      interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                  TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,       1,          1,
   0    },
  // A channel is established before "time" is reached.
  {L_, 'A',  CHANNEL, interruptible,   INFINITED,   0,       1,          2,
   0    },

  // Multiple channels can be established.
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,       1,          3,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,       1,          4,
   0    },

  // Now deallocate a channel.
  {L_, 'D',   CHANNEL, non_interrupt,  INFINITED,   0,       0,          3,
   0    },
  {L_, 'D', T_CHANNEL, non_interrupt,  INFINITED,   0,       0,          2,
   0    },

  // Establish new channels after the above deallocate.
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,       1,          3,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,       1,          4,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt,  INFINITED,   0,       1,          5,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,       1,          6,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,   0,       1,          7,
   0    },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'C', T_CHANNEL, non_interrupt,  INFINITED,   0,       0,          7,
   0    },
  {L_, 'A', T_CHANNEL, interruptible,  INFINITED,  -2,       0,          7,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt,  INFINITED,  -2,       0,          7,
   0},

};
// ================>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels = numChannelToBeEstablished(
                                                         DATA, NUM_DATA);

                  LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));

                 if (veryVerbose) {
                     PT(channels.size());
                 }
              }
          }
      } break;
      case 6: {
          // ----------------------------------------------------------------
          // TESTING MANIPULATOR METHODS:
          //   The main concerns about this function is that (1) if it can
          //   open the 'acceptor' as expected; (2) if it will not establish
          //   any new channel with any "allocate" function; (3) if it can
          //   set and get the right socket option values.
          //
          // Plan:
          //   Build a set 'VALUES' to create different acceptor objects.
          //   For each object,  every function under test will be invoked
          //   accordingly, verify the return value after each invocation.
          //
          // Testing:
          //   int open();
          //   int getOption();
          //   int setOption();
          //   void invalidate();
          // ----------------------------------------------------------------
          if (verbose) {
              QT("Testing MANIPULATOR method");
              QT("==========================");
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
                {  L_,     &testAllocator,    0,           64 },
                {  L_,     &testAllocator,    0,          128 },
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
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  ASSERT(0 == acceptor.isInvalid());

                  bteso_IPv4Address serverAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  // Open the 'acceptor' to establish a listening server
                  // socket.
                  int ret = acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize);
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 != acceptor.socket());
                  LOOP_ASSERT(i, 0 != acceptor.address().portNumber());

                  // Get the socket option for Nagle algorithm flag.
                  int level  = bteso_SocketOptUtil::BTESO_TCPLEVEL,
                      option = bteso_SocketOptUtil::BTESO_TCPNODELAY,
                      result = 0;
                  ret = acceptor.getOption(&result, level, option);
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i,
                              bteso_SocketOptUtil::BTESO_TCPNODELAY != result);

                  // Get the socket option for Nagle algorithm flag and verify
                  // the result.
                  int value = !result;
                  ret = acceptor.setOption(level, option, value);
                  LOOP_ASSERT(i, 0 == ret);
                  ret = acceptor.getOption(&result, level, option);
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 != result);
                  value = 0;
                  ret = acceptor.setOption(level, option, value);
                  LOOP_ASSERT(i, 0 == ret);
                  ret = acceptor.getOption(&result, level, option);
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 == result);

                  // Close the 'acceptor'.
                  ret = acceptor.close();
                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 == acceptor.socket());
                  LOOP_ASSERT(i, 0 == acceptor.address().portNumber());

                  // Invalidate the 'acceptor'.
                  acceptor.invalidate();
                  ASSERT(0 != acceptor.isInvalid());
              }
          }

          if (verbose) {
              QT("Testing 'open' method");
              QT("=====================");
          }
          {
              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              struct {
                  int                  d_lineNum;
                  bslma_TestAllocator *d_allocator_p;// memory allocator
                  int                  d_capacity;   // initial capacity: not
                                                     // specified if 0
                  int                  d_queueSize;  // a server socket's
                                                     // back-log value
                  int                  d_reuseAddressFlag;
              } VALUES[] =
                //line         allocator   capacity   queueSize  reuseAddress
                //----         ---------   --------   ---------  ------------
              {
                { L_,              0,          0,         16,           1   },
                { L_,              0,          8,         32,           0   },
              };

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  ASSERT(0 == acceptor.isInvalid());

                  bteso_IPv4Address serverAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  // Open the 'acceptor' to establish a listening server
                  // socket.

                  int ret = acceptor.open(serverAddress,
                                          VALUES[i].d_queueSize,
                                          VALUES[i].d_reuseAddressFlag);

                  LOOP_ASSERT(i, 0 == ret);
                  LOOP_ASSERT(i, 0 != acceptor.socket());
                  LOOP_ASSERT(i, 0 != acceptor.address().portNumber());

                  int level  = bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                      option = bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                      result = 0;
                  ret = acceptor.getOption(&result, level, option);
                  LOOP_ASSERT(i, 0 == ret);

                  int existing = 0;
                  bteso_IPv4Address establishedServerAddr(acceptor.address());

                  TestCommand DATA[] =
// ===============>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          1,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          2,
   0    },
  {L_, 'C',  CHANNEL,  non_interrupt, INFINITED,   0,        0,          2,
   0    },
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,  -2,        0,          2,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,  -2,        0,          2,
   0    },
};
// ===============>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels = numChannelToBeEstablished(DATA,
                                                                 NUM_DATA);

                  LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));
                  existing = channels.size();
                  if (veryVerbose) {
                      PT(channels.size());
                  }
                  if (veryVerbose) {
                      QT("Now the server socket is closed, and try to "
                         "re-establish the server with the same socket.");
                  }

                  // If the 'd_reuseAddressFlag' is on, re-bind the previous
                  // closed server socket and connection try should be able
                  // to succeed.
                  if (VALUES[i].d_reuseAddressFlag) {
                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,   existing + 1,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,   existing + 2,
   0    },
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      // Open the 'acceptor' to establish a listening server
                      // socket.
                      int ret = acceptor.open(establishedServerAddr,
                                              VALUES[i].d_queueSize,
                                              VALUES[i].d_reuseAddressFlag);
                      LOOP_ASSERT(i, 0 == ret);
                      LOOP_ASSERT(i, 0 != acceptor.socket());
                      LOOP_ASSERT(i, 0 != acceptor.address().portNumber());

                      int expNumChannels =
                               numChannelToBeEstablished(DATA, NUM_DATA) -
                                                                  existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));
                      existing = channels.size();
                      if (veryVerbose) {
                          PT(channels.size());
                      }
                  }
                  #ifdef BSLS_PLATFORM__OS_UNIX
                  else {
                      // The 'reuseAddressFlag' is off, it'll fail to
                      // 're-establish' the server with the same socket.
                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   -2,      0,       existing,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   -2,      0,       existing,
   0    },
};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      // Open the 'acceptor' to establish a listening server
                      // socket.
                      int ret = acceptor.open(establishedServerAddr,
                                              VALUES[i].d_queueSize,
                                              VALUES[i].d_reuseAddressFlag);
                      LOOP_ASSERT(i, 0 != ret);
                      LOOP_ASSERT(i, 0 == acceptor.socket());
                      LOOP_ASSERT(i, 0 == acceptor.address().portNumber());

                      int expNumChannels =
                               numChannelToBeEstablished(DATA, NUM_DATA) -
                                                                  existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));
                      existing = channels.size();
                      if (veryVerbose) {
                          PT(channels.size());
                      }
                  }
                  #endif
              }
          }
      } break;
      case 5: {
          // ----------------------------------------------------------------
          // TESTING 'allocateTimed' METHOD:
          //
          // Concerns:
          //   The main concerns about this function are that if it can
          //     1. return the expected 'status' value and the 'null' channel
          //        when an "asynchronous event" happens and this function
          //        is called in the "interruptible" mode;
          //     2. keep trying to accept a new connection until one is
          //        established or an error happens when an "asynchronous
          //        events" happens and the function is called in the
          //        "non_interruptible" mode;
          //     3. establish a channel of the expected type, i.e., the new
          //        established channel should be of type
          //        'btesos_TcpTimedChannel';
          //     4. establish new channels after the previous request is
          //        interrupted by an "asynchronous event";
          //     5. establish multiple channels;
          //     6. establish new channels after a channel is deallocated,
          //        and the other channel is not affected;
          //     7. work fine after calls to other "allocate" functions;
          //     8. not establish any new channels after the 'acceptor' is
          //        closed;
          //     9. establish new channels after the 'acceptor' is opened
          //        again successfully;
          //    10. return the expected 'status' value and the 'null' channel
          //        when an error occurs;
          //    11. not establish any new channels after the 'acceptor' is
          //        invalidated;
          //
          // Plan:
          //   After a 'btesos_TcpAcceptor' object 'acceptor' is
          //   created, four steps are involved to test this function:
          //
          //   Step 1: (for concern 1 - 8)
          //     Build a set 'DATA' of 'TestCommand', where each entry
          //     specifies: (1) which function to be invoked; (2) what
          //     arguments to be passed for this call and (3) the results
          //     expected to be returned.  A thread is created to act as a
          //     client for the test.  The client will submit the expected
          //     number of connection requests and/or generate signals
          //     to the main thread if specified.  The reason to generate
          //     signals is to test if the 'acceptor' works fine when an
          //     "asynchronous event" occurs.  To test the 'acceptor' in
          //     its different states, the signal delivery could happen
          //     (1) before any connection is established; (2) after
          //     some but not all expected connections is established.  The
          //     result from each test will be compared against those expected.
          //
          //   Step 2: (for concern 9)
          //     Reopen the 'acceptor' with a valid server address.  Build a
          //     set 'DATA' of 'TestCommand', where each entry will issue a
          //     request to accept a connection.  A thread will be created as
          //     a client to ask for connections.  The expected number of
          //     connections should be established and thus the concern (9)
          //     can be resolved.
          //
          //   Step 3: (for concern 10)
          //     Extract the 'acceptor's socket and close it to simulate the
          //     scenario that a "hard" error happens from calls to an
          //     "allocate" function.  Build a set 'DATA' of 'TestCommand',
          //     where each entry will issue a request to accept a connection.
          //     A thread will be created as the client to ask a connection
          //     while no connection is expected to be established.
          //
          //   Step 4: (for concern 11)
          //     Close the 'acceptor' and reopen the 'acceptor' with a valid
          //     server address.  Build a set 'DATA' of 'TestCommand' to
          //     accept some connections first and then invalidate the
          //     'acceptor'.  A thread will be created as a client to
          //     ask for connections.  No any new connections are supposed
          //     to be established after the 'acceptor' is invalidated.
          //
          // Testing
          //   int allocateTimed();
          // ----------------------------------------------------------------

#if 0  // TBD test case not exercised
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
                {  L_,            0,          0,           32 },
                {  L_,     &testAllocator,    0,           64 },
                {  L_,     &testAllocator,    0,          128 },
              };
              // Register a signal handler for "SIGSYS".

              bdet_TimeInterval timeout(20, 10000000);
              timeout += bdetu_SystemTime::now();

              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  int existing = 0;
                  if (verbose) {
                      QT("Step 1 for testing 'allocateTimed' method:");
                      QT("==========================================");
                  }

                  {
                      bteso_IPv4Address serverAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // Open the 'acceptor' to establish a listening server
                      // socket.
                      ASSERT(0 == acceptor.open(serverAddress,
                                                VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
#ifdef BSLS_PLATFORM__OS_UNIX
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // Generate a 'SIGSYS' and no channel is established: concern (1).
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   1,        0,          0,
   1    },

  // Generate a 'SIGSYS' but a channel will be established: concern (2), (3).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          1,
   1    },

  // Generates a 'SIGSYS' and no channel is established: concern (4).
  {L_, 'A', T_CHANNEL,  interruptible, INFINITED,  1,        0,          1,
   1    },

  // Multiple channels can be established: concern (4), (5).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          2,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          3,
   0    },

  // Now deallocate a channel: concern (6).
  {L_, 'D', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          2,
   0    },
  {L_, 'D', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          1,
   0    },

  // Establish a new channel after the above deallocate: concern (6).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          2,
   0    },

  // Can still establish channels after calling other "allocate": concern (7).
  {L_, 'A',  CHANNEL,   non_interrupt, INFINITED,  0,        1,          3,
   0    },
  {L_, 'A',  CHANNEL,   non_interrupt, INFINITED,  0,        1,          4,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          5,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          6,
   0    },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'C', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          6,
   0    },
  {L_, 'A', T_CHANNEL,  interruptible, INFINITED, -2,        0,          6,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED, -2,        0,          6,
   0    },

};
#else     // windows test data
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // a channel is established.
  {L_, 'A', T_CHANNEL,  interruptible, INFINITED,  0,        1,          1,
   0    },

  // Multiple channels can be established: concern (4), (5).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          2,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          3,
   0    },

  // Now deallocate a channel: concern (6).
  {L_, 'D', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          2,
   0    },
  {L_, 'D', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          1,
   0    },

  // Establish a new channel after the above deallocate: concern (6).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          2,
   0    },

  // Can still establish channels after calling other "allocate": concern (7).
  {L_, 'A',  CHANNEL,   non_interrupt, INFINITED,  0,        1,          3,
   0    },
  {L_, 'A',  CHANNEL,   non_interrupt, INFINITED,  0,        1,          4,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          5,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,        1,          6,
   0    },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'C', T_CHANNEL,  non_interrupt, INFINITED,  0,        0,          6,
   0    },
  {L_, 'A', T_CHANNEL,  interruptible, INFINITED, -2,        0,          6,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED, -2,        0,          6,
   0    },

};
#endif
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels = numChannelToBeEstablished(
                                                                  DATA,
                                                                  NUM_DATA);
                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));
                      existing = channels.size();
                      if (veryVerbose) {
                          QT("Step 1: channels.size() = ");
                          PT(channels.size());
                      }
                  }

                  if (verbose) {
                      QT("Step 2 for testing 'allocateTimed' method:");
                      QT("==========================================");
                  }

                  {
                      bteso_IPv4Address serverAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // The 'acceptor' is closed in step 1, now reopen it
                      // to establish a listening server socket.

                      ASSERT(0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // New channels can still be established: concern (9).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,       1,   existing + 1,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,       1,   existing + 2,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  0,       1,   existing + 3,
   0    },

};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));

                      if (veryVerbose) {
                          QT("Step 2: channels.size() = ");
                          PT(channels.size());
                      }
                      existing = channels.size();
                  }

                  if (verbose) {
                      QT("Step 3 for testing 'allocateTimed' method:");
                      QT("==========================================");
                  }

#ifdef BSLS_PLATFORM__OS_SOLARIS
                  {
                      const bteso_StreamSocket<bteso_IPv4Address>
                                   *serverSocket = acceptor.socket();
                      StreamSocket *server =
                          const_cast <StreamSocket *> (serverSocket);

                      // If multiple test data is added, be sure the
                      // 'acceptor' should be opened every time, then create
                      // a thread to close its server socket.
                      TestCommand DATA =
// ===================>
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  -3,      0,      existing,
   0    };
// ===================>

                      bcemt_ThreadUtil::Handle threadHandle;
                      bcemt_Attribute attributes;
                      int ret = bcemt_ThreadUtil::create(&threadHandle,
                                                         attributes,
                                                         threadToCloseServer,
                                                         server);
                      ASSERT(0 == ret);

                      int status = 0;
                      btesc_Channel *newChannel =
                                  acceptor.allocateTimed(
                                                    &status,
                                                    DATA.d_interruptFlags);
                      if (!DATA.d_validChannel) {
                          LOOP_ASSERT(i, 0 == newChannel);
                      }
                      else {
                          LOOP_ASSERT(i, 0 != newChannel);
                      }
                  }
#endif
                  if (verbose) {
                      QT("Step 4 for testing 'allocateTimed' method:");
                      QT("==========================================");
                  }

                  {
                      // Close the old server socket to deallocate it.
                      ASSERT(0 == acceptor.close());

                      bteso_IPv4Address serverAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // Reopen the 'acceptor' establish a new listening
                      // server socket.
                      ASSERT(0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // New channels can still be established: concern (9).
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,   0,      1,   existing + 1,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt,  INFINITED,  0,      1,   existing + 2,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,   0,      1,   existing + 3,
   0    },
  {L_, 'I', T_CHANNEL,  non_interrupt, INFINITED,   0,      0,   existing + 3,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  -4,      0,   existing + 3,
   0    },
  {L_, 'A', T_CHANNEL,  non_interrupt, INFINITED,  -4,      0,   existing + 3,
   0    },

};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));

                      if (veryVerbose) {
                          QT("Step 4: channels.size() = ");
                          PT(channels.size());
                      }
                      existing = channels.size();
                  }
                  channels.clear();
              }
          }
#endif
      } break;
      case 4: {
          // ----------------------------------------------------------------
          // TESTING 'allocate' METHOD:
          //
          // Concerns:
          //   The main concerns about this function are that if it can
          //     1. return the expected 'status' value and the 'null' channel
          //        when an "asynchronous event" happens and this function
          //        is called in the "interruptible" mode;
          //     2. keep trying to accept a new connection until one is
          //        established or an error happens when an "asynchronous
          //        events" happens and the function is called in the
          //        "non_interruptible" mode;
          //     3. establish a channel of the expected type, i.e., the new
          //        established channel should be of type
          //        'btesos_TcpChannel';
          //     4. establish new channels after the previous request is
          //        interrupted by an "asynchronous event";
          //     5. establish multiple channels;
          //     6. establish new channels after a channel is deallocated,
          //        and the other channel is not affected;
          //     7. work fine after calls to other "allocate" functions;
          //     8. not establish any new channels after the 'acceptor' is
          //        closed;
          //     9. establish new channels after the 'acceptor' is opened
          //        again successfully;
          //    10. return the expected 'status' value and the 'null' channel
          //        when an error occurs;
          //    11. not establish any new channels after the 'acceptor' is
          //        invalidated;
          //
          // Plan:
          //   After a 'btesos_TcpAcceptor' object 'acceptor' is
          //   created, four steps are involved to test this function:
          //
          //   Step 1: (for concern 1 - 8)
          //     Build a set 'DATA' of 'TestCommand', where each entry
          //     specifies: (1) which function to be invoked; (2) what
          //     arguments to be passed for this call and (3) the results
          //     expected to be returned.  A thread is created to act as a
          //     client for the test.  The client will submit the expected
          //     number of connection requests and/or generate signals
          //     to the main thread if specified.  The reason to generate
          //     signals is to test if the 'acceptor' works fine when an
          //     "asynchronous event" occurs.  To test the 'acceptor' in
          //     its different states, the signal delivery could happen
          //     (1) before any connection is established; (2) after
          //     some but not all expected connections is established.  The
          //     result from each test will be compared against those expected.
          //
          //   Step 2: (for concern 9)
          //     Reopen the 'acceptor' with a valid server address.  Build a
          //     set 'DATA' of 'TestCommand', where each entry will issue a
          //     request to accept a connection.  A thread will be created as
          //     a client to ask for connections.  The expected number of
          //     connections should be established and thus the concern (9)
          //     can be resolved.
          //
          //   Step 3: (for concern 10)
          //     Extract the 'acceptor's socket and close it to simulate the
          //     scenario that a "hard" error happens from calls to an
          //     "allocate" function.  Build a set 'DATA' of 'TestCommand',
          //     where each entry will issue a request to accept a connection.
          //     A thread will be created as the client to ask a connection
          //     while no connection is expected to be established.
          //
          //   Step 4: (for concern 11)
          //     Close the 'acceptor' and reopen the 'acceptor' with a valid
          //     server address.  Build a set 'DATA' of 'TestCommand' to
          //     accept some connections first and then invalidate the
          //     'acceptor'.  A thread will be created as a client to
          //     ask for connections.  No any new connections are supposed
          //     to be established after the 'acceptor' is invalidated.
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
                {  L_,            0,          0,           32 },
                {  L_,     &testAllocator,    0,           64 },
                {  L_,     &testAllocator,    0,          128 },
              };

              // only needed for Solaris
              bdet_TimeInterval timeout(5, 10000000);
              timeout += bdetu_SystemTime::now();

              int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

              const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

              for (int i = 0; i < NUM_VALUES; i++) { // different acceptors
                  bsl::vector<btesc_Channel*> channels(&testAllocator);
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  int existing = 0;

                  if (verbose) {
                      QT("Step 1 for testing 'allocate' method:");
                      QT("=====================================");
                  }

                  {
                      bteso_IPv4Address serverAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // Open the 'acceptor' to establish a listening server
                      // socket.
                      ASSERT(0 == acceptor.open(serverAddress,
                                                VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
#ifdef BSLS_PLATFORM__OS_UNIX
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  // Generate a 'SIGSYS' and no channel is established: concern (1).
  {L_, 'A',  CHANNEL, interruptible,  INFINITED,   1,       0,           0,
   1    },

  // Generate a 'SIGSYS' but a channel will be established: concern (2), (3).
  {L_, 'A',  CHANNEL, non_interrupt,  INFINITED,   0,       1,           1,
   1    },

  // Generates a 'SIGSYS' and no channel is established: concern (4).
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,   1,       0,           1,
   1    },

  // Multiple channels can be established: concern (4), (5).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           2,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           3,
   0    },

  // Now deallocate a channel: concern (6).
  {L_, 'D',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           2,
   0    },
  {L_, 'D',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           1,
   0    },

  // Establish a new channel after the above deallocate: concern (6).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           2,
   0    },

  // Can still establish channels after calling other "allocate": concern (7).
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           3,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           4,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           5,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           6,
   0    },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'C',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           6,
   0    },
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,  -2,       0,           6,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,  -2,       0,           6,
   0    },

};
#else  // windows test data
{
//line cmd channelType  interruptFlag  timeout  expStat validChannel expNumConn
//---- --- -----------  -------------  -------  ------- ------------ ----------
// signal
// -----
  // one channel is established.
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,   0,       1,           1,
   0    },

  // Multiple channels can be established: concern (4), (5).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           2,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           3,
   0    },

  // Now deallocate a channel: concern (6).
  {L_, 'D',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           2,
   0    },
  {L_, 'D',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           1,
   0   },

  // Establish a new channel after the above deallocate: concern (6).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           2,
   0    },

  // Can still establish channels after calling other "allocate": concern (7).
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           3,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           4,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           5,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           6,
   0    },

  // Close the 'acceptor' can't establish any more channels: concern (8).
  {L_, 'C',  CHANNEL,  non_interrupt, INFINITED,   0,       0,           6,
   0    },
  {L_, 'A',  CHANNEL,  interruptible, INFINITED,  -2,       0,           6,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,  -2,       0,           6,
   0    },

};
#endif
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels = numChannelToBeEstablished(
                                                                  DATA,
                                                                  NUM_DATA);

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));
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
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // The 'acceptor' is closed in step 1, now reopen it
                      // to establish a listening server socket.

                      ASSERT(0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // New channels can still be established: concern (9).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 1,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 2,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 3,
   0    },

};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));

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

#ifdef BSLS_PLATFORM__OS_SOLARIS
                  {
                      const StreamSocket *serverSocket = acceptor.socket();
                      StreamSocket *server =
                          const_cast <StreamSocket *> (serverSocket);

                      // If multiple test data is added, be sure the
                      // 'acceptor' should be opened every time, then create
                      // a thread to close its server socket.
                      TestCommand DATA =
// ===================>
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  {L_, 'A',  CHANNEL,   non_interrupt, &timeout,  -3,      0,      existing,
   0    };
// ===================>

                      bcemt_ThreadUtil::Handle threadHandle;
                      bcemt_Attribute attributes;
                      int ret = bcemt_ThreadUtil::create(&threadHandle,
                                                         attributes,
                                                         threadToCloseServer,
                                                         server);
                      ASSERT(0 == ret);

                      int status = 0;
                      btesc_Channel *newChannel =
                                  acceptor.allocate(&status,
                                                    DATA.d_interruptFlags);
                      if (!DATA.d_validChannel) {
                          LOOP_ASSERT(i, 0 == newChannel);
                      }
                      else {
                          LOOP_ASSERT(i, 0 != newChannel);
                      }
                      LOOP_ASSERT(i, DATA.d_expStatus == status);
                  }
#endif
                  if (verbose) {
                      QT("Step 4 for testing 'allocate' method:");
                      QT("=====================================");
                  }
                  {
                      // Close the old server socket to deallocate it.
                      ASSERT(0 == acceptor.close());

                      bteso_IPv4Address serverAddress;
                      serverAddress.setIpAddress(HOST_NAME);
                      serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                      // Reopen the 'acceptor' establish a new listening
                      // server socket.
                      ASSERT(0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                      ASSERT(0 == acceptor.isInvalid());

                      TestCommand DATA[] =
// ===================>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // New channels can still be established: concern (9).
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 1,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 2,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,   existing + 3,
   0    },
  {L_, 'I',  CHANNEL,  non_interrupt, INFINITED,   0,       0,   existing + 3,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,  -4,       0,   existing + 3,
   0    },
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,  -4,       0,   existing + 3,
   0    },

};
// ===================>
                      const int NUM_DATA = sizeof DATA / sizeof *DATA;
                      int expNumChannels =
                            numChannelToBeEstablished(DATA, NUM_DATA)
                                                                    - existing;

                      LOOP_ASSERT(VALUES[i].d_lineNum,
                                  0 == processTest(&acceptor,
                                                   threadAsClient,
                                                   &channels,
                                                   DATA,
                                                   NUM_DATA,
                                                   expNumChannels));

                      if (veryVerbose) {
                          QT("Step 4: channels.size() = ");
                          PT(channels.size());
                      }
                      existing = channels.size();
                  }
                  channels.clear();
              }
          }
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
              QT("Testing BASIC ACCESSORS");
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
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  LOOP_ASSERT(i, 0 == acceptor.isInvalid());
                  LOOP_ASSERT(i, 0 == acceptor.numChannels());

                  bteso_IPv4Address serverAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  // Open the 'acceptor' to establish a listening server
                  // socket.
                  LOOP_ASSERT(i, 0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                  LOOP_ASSERT(i, 0 == acceptor.isInvalid());
                  int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                  TestCommand DATA[] =
// ===============>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // Each of the following command is to establish a channel.
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,        1,          1,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,        1,          2,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,        1,          3,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,        1,          4,
   0    },
  {L_, 'A',   CHANNEL, interruptible, INFINITED,   0,        1,          5,
   0    },

  {L_, 'I',   CHANNEL, interruptible, INFINITED,   0,        0,          5,
   0    },
};
// ===============>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels = numChannelToBeEstablished(DATA,
                                                                 NUM_DATA);
                  LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));

                  if (veryVerbose) {
                      PT(channels.size());
                  }
                  LOOP_ASSERT(i, 1 == acceptor.isInvalid());
                  LOOP_ASSERT(i, DATA[NUM_DATA - 1].d_expNumChannels ==
                                                    acceptor.numChannels());
                  channels.clear();
              }
          }
      } break;
      case 2: {
          // ----------------------------------------------------------------
          // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
          //   The main concern about this set of functions is that (1) it
          //   can create different 'btesos_TcpAcceptor' objects
          //   successfully, based on different parameters; (2) if can
          //   destructor any 'btesos_TcpAcceptor' object successfully.
          // Plan:
          //   Build a set of data 'VALUES' to construct different
          //   'btesos_TcpAcceptor' objects.  With each objects,
          //   establish a specified number of channels, verify the result
          //   from each channel operation.
          // Testing:
          //   virtual btesos_TcpAcceptor();
          //   virtual btesos_TcpAcceptor(..., int initialCapacity, ...);
          //   virtual ~btesos_TcpAcceptor();
          // ----------------------------------------------------------------
          if (verbose) {
              QT("Testing Primary Manipulator");
              QT("===========================");
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
                {  L_,     &testAllocator,    0,           64 },
                {  L_,     &testAllocator,    0,          128 },
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
                  Obj acceptor(&factory, VALUES[i].d_allocator_p);
                  bteso_IPv4Address serverAddress;
                  serverAddress.setIpAddress(HOST_NAME);
                  serverAddress.setPortNumber(DEFAULT_PORT_NUMBER);

                  // Open the 'acceptor' to establish a listening server
                  // socket.
                  ASSERT(0 == acceptor.open(serverAddress,
                                            VALUES[i].d_queueSize));
                  ASSERT(0 == acceptor.isInvalid());
                  int non_interrupt = 0,
                  interruptible = btesc_Flag::BTESC_ASYNC_INTERRUPT;

                  TestCommand DATA[] =
// ===============>
{
//line cmd channelType  interruptFlag  timeout expStat validChannel expNumConn
//---- --- -----------  -------------  ------- ------- ------------ ----------
// signal
// -----
  // Each of the following command is to establish a channel.
  {L_, 'A',  CHANNEL,  non_interrupt, INFINITED,   0,       1,           1,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,       1,           2,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           3,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,       1,           4,
   0    },
  {L_, 'A',   CHANNEL, interruptible, INFINITED,   0,       1,           5,
   0    },

  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,       1,           6,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,       1,           7,
   0    },
  {L_, 'A',   CHANNEL, non_interrupt, INFINITED,   0,       1,           8,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,           9,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          10,
   0    },

  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          11,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,       1,          12,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          13,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          14,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,       1,          15,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          16,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,       1,          17,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          18,
   0    },
  {L_, 'A', T_CHANNEL, non_interrupt, INFINITED,   0,       1,          19,
   0    },
  {L_, 'A', T_CHANNEL, interruptible, INFINITED,   0,       1,          20,
   0    },
};
// ===============>
                  const int NUM_DATA = sizeof DATA / sizeof *DATA;
                  int expNumChannels = numChannelToBeEstablished(DATA,
                                                                 NUM_DATA);
                  LOOP_ASSERT(VALUES[i].d_lineNum,
                              0 == processTest(&acceptor,
                                               threadAsClient,
                                               &channels,
                                               DATA,
                                               NUM_DATA,
                                               expNumChannels));
                  if (veryVerbose) {
                      PT(channels.size());
                  }
                  channels.clear();
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
