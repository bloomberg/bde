// bteso_eventmanagertester.cpp    -*-C++-*-
#include <bteso_eventmanagertester.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_eventmanagertester_cpp,"$Id$ $CSID$")

#include <bteso_flag.h>
#include <bteso_eventtype.h>
#include <bteso_socketimputil.h>
#include <bteso_eventmanager.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>
#include <bteso_ioutil.h>
#include <bteso_platform.h>

#include <bcemt_thread.h>                       // thread management util

#include <bdef_function.h>
#include <bdef_bind.h>

#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>
#include <bslma_testallocator.h>                // allocate memory
#include <bsls_assert.h>
#include <bdetu_systemtime.h>

#include <bsl_fstream.h>
#include <bsl_iomanip.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>                             // abort

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#include <pthread.h>
#endif

namespace BloombergLP {

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::printf("%s = %d\n", #X, (X)); std::fflush(stdout);
    // Print identifier and value.
#define Q(X) std::printf("<| %s |>\n", #X); std::fflush(stdout);
    // Quote identifier literally.
#define P_(X) std::printf("%s = %d, ", #X, (X)); std::fflush(stdout);
    // P(X) without '\n'
#define L_ __LINE__                           // current Line number

// Test success and failure codes.
enum {
    FAIL = -1,
    SUCCESS = 0
};

enum {
    BUF_LEN = 8192                            // socket buffers' size
};

// It turns out different number of bytes are needed to fill in a connection
// pipe on different platforms, even though the same buffer sizes are set.
// That's the reason different data buffer sizes are specified on different
// platforms.
#if defined(BSLS_PLATFORM__OS_WINDOWS)
    enum {
        READ_SIZE  = 8192,    // the size of buffer to read from the pipe
        WRITE_SIZE = 30000    // the size of buffer to write to the pipe
    };
#elif defined (BSLS_PLATFORM__OS_UNIX)
    enum {
        READ_SIZE  = 8192,    // the size of buffer to read from the pipe
        WRITE_SIZE = 73728    // the size of buffer to write to the pipe
    };

    struct ThreadInfo{
        // Use this struct to pass information to the helper thread.
        bteso_SocketHandle::Handle  d_socket;     // client socket
        pthread_t                   d_tid;        // the id of the thread to
                                                  // which a signal's delivered
        int                         d_timeoutFlag;// timeout dispatch or
                                                  // infinite
        int                         d_ctrlFlag;   // verbose, ABORT ......
    };

#endif

typedef bteso_EventManagerTestPair SocketPair;

static void genericCb(bteso_EventType::Type event,
                      int                   fd,
                      int                   bytes,
                      bteso_EventManager   *mX,
                      SocketPair           *fds,
                      const char           *cbScript,
                      int                   flags);

static int ggHelper(bteso_EventManager         *mX,
                    bteso_EventManagerTestPair *fds,
                    const char                 *test,
                    int                         flags);

#ifdef BSLS_PLATFORM__OS_UNIX
extern "C"
void* threadSignalGenerator(void *arg)
    // Generate signal 'SIGSYS' and deliver it to a thread specified in 'arg'.
    // Note the test can only work on UNIX platforms since window doesn't
    // support signal operations.
{
    BSLS_ASSERT(arg);
    enum { BASE_TIME = 1000 };      // the basic sleep time in microseconds
    bcemt_ThreadUtil::microSleep(BASE_TIME);
                                    // The thread waits to make sure
                                    // the main thread is hanging in the
                                    // dispatch() call.

    ThreadInfo socketInfo = *(ThreadInfo*) arg;

    pthread_kill(socketInfo.d_tid, SIGSYS);
    if (socketInfo.d_ctrlFlag & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
        std::printf("Thread %d generated a SIGSYS signal.\n",
                    bcemt_ThreadUtil::selfIdAsInt());
        std::fflush(stdout);
    }
    bcemt_ThreadUtil::microSleep(3 * BASE_TIME);
    pthread_kill(socketInfo.d_tid, SIGSYS);
    if (socketInfo.d_ctrlFlag & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
        std::printf("Thread %d delivered another SIGSYS signal to %d.\n",
                    bcemt_ThreadUtil::selfIdAsInt(),
                    bcemt_ThreadUtil::idAsInt(
                              bcemt_ThreadUtil::handleToId(socketInfo.d_tid)));
        std::fflush(stdout);
    }
    if (!socketInfo.d_timeoutFlag) {
        // We have to make the dispatch() return if timeout is not set.
        enum {
            BUF_SIZE = 1
        };
        char buf[BUF_SIZE];
        bsl::memset(buf, 0xAB, BUF_SIZE); // to keep purify happy

        int len = bteso_SocketImpUtil::write(socketInfo.d_socket,
                                             buf,
                                             sizeof buf);

        if (socketInfo.d_ctrlFlag &
                                bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
            std::printf("Thread %d writes %d bytes to socket %d.\n",
                        bcemt_ThreadUtil::selfIdAsInt(),
                        len,
                        socketInfo.d_socket);
            std::fflush(stdout);
        }
        if (BUF_SIZE != len) {
            if (socketInfo.d_ctrlFlag &
                                       bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                std::printf("Thread %d doesn't write the right number of bytes"
                            " to socket %d.\n",
                            bcemt_ThreadUtil::selfIdAsInt(),
                            socketInfo.d_socket);
                std::fflush(stdout);
            }
        }
    }
    bcemt_ThreadUtil::microSleep(BASE_TIME);
    return 0;
}

static void signalHandler(int)
    // The signal handler does nothing.
{
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
        bsl::perror("The signal handler can't be installed.");
        BSLS_ASSERT(0);
    }
    return;
}

#endif

static const char *getNextCommand(const char *commandSeq)
    // Get the next command to be executed.  The command may include
    // commands to be executed in the user-installed
    // callback function, which is enclosed in '{' and '}'.
    // Return a pointer to the next command string, on success and 0
    // if no valid command exists or the end of a command is reached.
{
    BSLS_ASSERT("command shouldn't be null" && commandSeq);
    int curlyNotBalance = 0;

    // Check if the command is correct.
    while ('\0' != *commandSeq) {
        if ('{' == *commandSeq)
            ++curlyNotBalance;
        else if ('}' == *commandSeq)
            --curlyNotBalance;

        if (0 == curlyNotBalance && ';' == *commandSeq) {
            ++commandSeq;
            while (' ' == *commandSeq || '\t' == *commandSeq) ++commandSeq;
            return commandSeq;
        }
        ++commandSeq;
    }
    return 0;
}

static const char *get1stCbCommand(const char *commandSeq)
    // Get the first callback command to be executed in the user-installed
    // callback function, which is enclosed in '{' and '}'.
    // Return a pointer to the command string, on success and 0
    // if no valid command or reaches the end of the script.
{
    BSLS_ASSERT("command shouldn't be null" && commandSeq);

    while ('{' != *commandSeq && ';' != *commandSeq
                              && '\0' != *commandSeq)  ++commandSeq;
    if (';' == *commandSeq || '\0' == *commandSeq)
        return 0;
    else
        return ++commandSeq;   // skip '{'
}

static const char *getNextCbCommand(const char *cbCmd, int *errCode=0)
    // Return the next callback command in the callback script,
    // e.g., in command "+0r5,{+1w20,{E0rw}; +0r18}", there are
    // 2 commands to be executed in the callback, this function is to
    // return the next callback command.
    // Return 0 if the end of script is reached or no valid command exists.
    // and set the 'errCode' to 'FAIL' if no valid command exists.
{
    int curly = 1;  // the  number of pairs of '{' and '}' flag

    while (curly) {
        if ('{' == *cbCmd)
            curly++;
        else if ('}' == *cbCmd) {
            curly--;
            if (0 == curly) { // No any more callback command
                return 0;     // If there're valid commands, a ';' should be
                              // before this '}'.
            }
        }
        else if ('\0' == *cbCmd) {// We reach the end of script.
            if (0 == curly) {
                *errCode = FAIL;
            }
            else {
                *errCode = SUCCESS;
            }
            return 0;
        }
        else if (';' == *cbCmd) {
            if (1 == curly) {   // We still have more callback commands.
                ++cbCmd;        // We need to skip ';'.
                break;
            }
            else if (0 == curly) { // no any more callback command
                return 0;
            }
        }
        ++cbCmd;
    }
    while (' ' == *cbCmd || '\t' == *cbCmd)  ++cbCmd; // To skip space.
        return cbCmd;
}

static void
genericCb(bteso_EventType::Type event,
          int                   fd,
          int                   bytes,
          bteso_EventManager   *mX,
          SocketPair           *fds,
          const char           *cbScript,
          int                   flags)
    // This generic callback function performs 'event' specific action.
{
    if (0 > bytes) {
        if (flags & bteso_EventManagerTester::BTESO_ABORT) {
            BSLS_ASSERT(0 < bytes);
        }
        else {
            std::printf(" Line: %d Error: Negative number of bytes for IO: %d"
                        " bytes to socket %d\n",
                        L_,
                        bytes,
                        fds[fd].observedFd());
            std::fflush(stdout);
        }
    }
    switch (event) {
      case bteso_EventType::BTESO_READ: {
          enum { BUF_SIZE = 8192 };
          char buffer[BUF_SIZE];

          int rc = bteso_SocketImpUtil::read(buffer, fds[fd].observedFd(),
                                             bytes, 0);
          if (flags & bteso_EventManagerTester::BTESO_VERY_VERY_VERBOSE) {
              std::printf("Generic callback: read %d bytes.\n", rc);
              std::fflush(stdout);
          }
          if (bytes != rc) {
              if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                  BSLS_ASSERT("Read wrong number of bytes" && 0);
              }
              else {
                  std::printf("Error: read %d bytes from %d; "
                              "%d bytes expected.\n",
                              rc, fd, bytes);
                  std::fflush(stdout);
              }
          }
      } break;

      case bteso_EventType::BTESO_WRITE: {
          char wBuffer[WRITE_SIZE + 1];        // data to write to the observed
                                               // file descriptor of a socket
                                               // pair
          bsl::memset(wBuffer,'4',sizeof wBuffer);

          int rc = bteso_SocketImpUtil::write(fds[fd].observedFd(),
                                              &wBuffer, bytes, 0);
          if (flags & bteso_EventManagerTester::BTESO_VERY_VERY_VERBOSE) {
              std::printf("Generic callback: wrote %d bytes.\n", rc);
              std::fflush(stdout);
          }
          if (bytes != rc) {
              if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                  BSLS_ASSERT("Wrote wrong number of bytes" && 0);
              }
              else {
                  std::printf("Error: (%d): Wrote %d bytes to %d "
                              "instead of %d bytes requested.\n",
                              L_, rc, fds[fd].observedFd(), bytes);
                  std::fflush(stdout);
              }
          }
      } break;

      case bteso_EventType::BTESO_ACCEPT: {

      } break;

      case bteso_EventType::BTESO_CONNECT: {

      } break;

      default: {
          BSLS_ASSERT("Invalid event code -- must be fixed." && 0);
      } break;
    }
    // If a callback script has been specified, then execute them here by
    // invoking ggHelper().
    while (cbScript) {
        int ret = ggHelper(mX, fds, cbScript, flags);

        if (flags & bteso_EventManagerTester::BTESO_ABORT) {
            BSLS_ASSERT(SUCCESS == ret);
        }
        if (FAIL == ret) {
            std::puts("Callback command execution failed!");
            std::fflush(stdout);
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
        }
        int errCode = 0;
        cbScript = getNextCbCommand(cbScript, &errCode);
        if (!cbScript) {  // Verify it's the end of script:
                          // make sure it's not due to any invalid command.
            if (FAIL == errCode) {
                std::puts("Script command is invalid in callback function.");
                std::fflush(stdout);
                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
            }
        }
    }
}

static void
readCallback(bteso_EventType::Type      ,  // event
             bteso_SocketHandle::Handle ,  // socket
             int                        ,  // bytes
             bteso_EventManager *)         // mX
    // This function is used to test the registerSocketEvent() performance,
    // it's a dummy callback function.
{
    return ;
}

static int ggHelper(bteso_EventManager         *mX,
                    bteso_EventManagerTestPair *fds,
                    const char                 *test,
                    int                         flags)
    // Execute a specified test script command in 'test'.  The specified
    // 'fds' is an array of connected socket pairs on which the user makes
    // operations managed by the specified event manager 'mX'.  The bit flag
    // 'flags' is used to control the process execution and test result output.
    // Return 0, on success and the number of failures otherwise.
{
    BSLS_ASSERT(test);
    int fd(-1), nt(0);
    int rc(FAIL), ret(0);
    char buf[READ_SIZE], c[2];
    c[0] = -1;

    char wBuffer[WRITE_SIZE + 1];    // To write to a pipe.

    const char *cbScript = get1stCbCommand(test); // Executed in callback.

    switch (test[0]) {
      case 'T': {         // Commands such as "T5; T3,0" come here.
          rc = bsl::sscanf(test, "T%u,%u", (unsigned *)&nt, (unsigned *)&fd);
          if (1 == rc) {
              if (nt != mX->numEvents()) {
                  return FAIL;
              }
          }
          else if (2 == rc){
              if (nt != mX->numSocketEvents(fds[fd].observedFd())) {
                  return FAIL;
              }
          }
          else {
              return FAIL;
          }
      } break;
      case '+': {
        int bytes;

        // Read the <fd> field.
        rc = bsl::sscanf(test + 1, "%d", &fd);
        if (1 != rc) {
            return FAIL;
        }
        char d = test[2];
        switch (d) {
          case 'r':
          case 'w':
            rc = bsl::sscanf(test + 3, "%d", &bytes);
            if (1 == rc && 0 >= bytes) {
                return FAIL;
            }
            if (1 != rc) {
                bytes = -1;
            }
            break;
          case 'a':
          case 'c':
            break;
          default: {
            return FAIL;
          }
        }

        // Create a callback object.
        bteso_EventManager::Callback cb;
        switch (d) {
          case 'r':
            cb = bdef_BindUtil::bind(&genericCb, bteso_EventType::BTESO_READ,
                                          fd, bytes, mX, fds, cbScript, flags);
            mX->registerSocketEvent(fds[fd].observedFd(),
                                    bteso_EventType::BTESO_READ, cb);
            break;

          case 'w':
            cb = bdef_BindUtil::bind(&genericCb, bteso_EventType::BTESO_WRITE,
                                          fd, bytes, mX, fds, cbScript, flags);
            mX->registerSocketEvent(fds[fd].observedFd(),
                                    bteso_EventType::BTESO_WRITE, cb);
            break;

          case 'a':
            cb = bdef_BindUtil::bind(&genericCb,
                                     bteso_EventType::BTESO_ACCEPT,
                                     fd, bytes, mX, fds, cbScript, flags);
            mX->registerSocketEvent(fds[fd].observedFd(),
                                    bteso_EventType::BTESO_ACCEPT, cb);
            break;

          case 'c':
            cb = bdef_BindUtil::bind(&genericCb,
                                     bteso_EventType::BTESO_CONNECT,
                                     fd, bytes, mX, fds, cbScript, flags);
            mX->registerSocketEvent(fds[fd].observedFd(),
                                    bteso_EventType::BTESO_CONNECT, cb);
            break;
          default:
            return FAIL;
        }
      } break;
      case '-': {             // Commands such as "-1w; -0r; -a; -2" come here.
        rc = bsl::sscanf(test, "-%u%[rwac]", (unsigned *)&fd, &c[0]);
        if (1 > rc) {
            rc = bsl::sscanf(test, "-%c", &c[0]);
            if (1 != rc || 'a' != c[0]) {
                return FAIL;
            }
            mX->deregisterAll();
            break;
        }

        if (1 == rc) {
            int ret = mX->deregisterSocket(fds[fd].observedFd());
            if (0 <= ret) {
                return SUCCESS;
            }
            else {
                return FAIL;
            }
        }
        // Start for "rc == 2".
        switch (c[0]) {
          case 'r': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      bteso_EventType::BTESO_READ);

          } break;
          case 'w': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      bteso_EventType::BTESO_WRITE);
          } break;
          case 'a': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      bteso_EventType::BTESO_ACCEPT);
          } break;
          case 'c': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      bteso_EventType::BTESO_CONNECT);
          } break;
          default:
                return FAIL;
          }
        } break;
      case 'D': {
        int msecs(0), rc(-1), nbytes(0);
        char ch;
        int flags = 0;
        bdet_TimeInterval deadline(bdetu_SystemTime::now());
        //enum { SLEEP_TIME = 200000 };
        //bcemt_ThreadUtil::microSleep(SLEEP_TIME);
        if (3 == bsl::sscanf(test, "D%c%d,%d%n", &ch, &msecs, &rc, &nbytes)) {
            if (0 > msecs || 0 > rc) {
                return FAIL;
            }
            switch (ch) {
              case 'n': {
                flags = 0;
              } break;
              case 'i': {
                flags = bteso_Flag::BTESO_ASYNC_INTERRUPT;
              } break;
              default:
                return FAIL;
            }
            deadline.addMilliseconds(msecs);
            ret = mX->dispatch(deadline, flags);
            if (rc != ret) {
                return FAIL;
            }
        }
        else if (2 == bsl::sscanf(test, "D%c,%u%n", &ch, (unsigned *)&rc,
                                                                    &nbytes)) {
            if (0 > rc) {
                return FAIL;
            }
            switch (ch) {
              case 'n': {
                flags = 0;
              } break;
              case 'i': {
                flags = bteso_Flag::BTESO_ASYNC_INTERRUPT;
              } break;
              default:
                return FAIL;
            }
            ret = mX->dispatch(flags);
            if (rc != ret) {
                return FAIL;
            }
        }
        else {
            return FAIL;
        }
      } break;
      case 'E': {
          rc = bsl::sscanf(test, "E%d%[acrw]", &fd, buf);
          if (rc == 2) {
              if (bsl::strchr(buf, 'a')) {
                  ret = mX->isRegistered(fds[fd].observedFd(),
                                         bteso_EventType::BTESO_ACCEPT);
                  if (1 != ret) {
                        return FAIL;
                  }
              }
              if (bsl::strchr(buf, 'c')) {
                  ret = mX->isRegistered(fds[fd].observedFd(),
                                         bteso_EventType::BTESO_CONNECT);
                  if (1 != ret) {
                      return FAIL;
                  }
              }
              if (bsl::strchr(buf, 'r')) {
                  ret = mX->isRegistered(
                         fds[fd].observedFd(), bteso_EventType::BTESO_READ);
                  if (1 != ret) {
                      return FAIL;
                  }
              }
              if (bsl::strchr(buf, 'w')) {
                  ret = mX->isRegistered(fds[fd].observedFd(),
                                         bteso_EventType::BTESO_WRITE);
                  if (1 != ret) {
                      return FAIL;
                  }
              }
          }
          else {
              ret = mX->numSocketEvents(fds[fd].observedFd());
              if (0 != ret) {
                  return FAIL;
              }
          }
      } break;
      case 'R': {
          int bytes = 0;
          rc = bsl::sscanf(test, "R%d,%u", &fd, (unsigned *)&bytes);
          if (2 != rc) {
              return FAIL;
          }
          int rc = bteso_SocketImpUtil::read(buf, fds[fd].observedFd(),
                                             bytes, 0);
          if (0 >= rc) {
              return FAIL;
          }
      } break;
      case 'W': {
          int bytes = 0;
          rc = bsl::sscanf(test, "W%d,%d", &fd, &bytes);

          if (2 != rc) {
              return FAIL;
          }
          bsl::memset(wBuffer, 0xAB, sizeof wBuffer); // to keep purify happy
          int rc = bteso_SocketImpUtil::write(fds[fd].controlFd(), &wBuffer,
                                              bytes);
          if (0 >= rc) {
              return FAIL;
          }
      } break;
      default:
          return FAIL;
    }
    return SUCCESS;
}

int bteso_EventManagerTester::gg(bteso_EventManager *mX,
                                 SocketPair         *fds,
                                 const char         *script,
                                 int                 flags)
    // Break up the passed test script into individual test commands and
    // execute each command by invoking ggHelper().
    // Return 0, on success and  the number of failures otherwise.
{
    BSLS_ASSERT(script);
    const char *originalScript = script;

    int fails = 0;
    if ((flags & bteso_EventManagerTester::BTESO_DRY_RUN) |
        (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE))
    {
        std::printf("Executing: %s\n", script);
        std::fflush(stdout);
    }
    if (flags & bteso_EventManagerTester::BTESO_DRY_RUN) {
        return 0;
    }

    while (script) {
        int ret = ggHelper(mX, fds, script, flags);
        if (SUCCESS != ret) {
            std::printf("\"%s\" FAILED.\n%*c^\n\n",
                        originalScript,
                        script - originalScript + 1,
                        ' ');
            std::fflush(stdout);
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                bsl::abort();
            }
            else {
                ++fails;
            }
        }
        script = getNextCommand(script);
    }
    return fails;
}

int
bteso_EventManagerTester::testRegisterSocketEvent(bteso_EventManager *mX,
                                                  int                 flags)
{
    int ret = 0, numFailures = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing testRegisterSocketEvent() method\n"
                  "========================================");
    }

    struct {
        int         d_line;
        const char *d_script;
    } SCRIPTS[] =
    {   // The reason to prefix "-a;T0;" at the beginning of each second script
        //  is that we only use ONE eventmanager for all scripts here.

        // Test the event manager by registering one socket event.
      {L_, "-a; +0w; T1; E0w; +0w; T1; E0w"                 },
      {L_, "-a; +0r; T1; E0r; +0r; T1; E0r"                 },
      {L_, "-a; +0a; T1; E0a; +0a; T1; E0a"                 },
      {L_, "-a; +0c; T1; E0c; +0c; T1; E0c"                 },

        // Test the event manager by registering two socket events.
        // The two socket events are for the same socket handle.
      {L_, "-a; +0w; T1; E0w; +0r; T2; E0rw; T2,0 +0w; T2,0"},
      {L_, "-a; +0r; T1; E0r; +0w; T2; E0rw; T2,0 +0r; T2,0"},

        // The two socket events are for different socket handles.
      {L_, "-a; +0w; T1; E0w; +1w; E1w; T2; +1w; E1w; T2"   },
      {L_, "-a; +0w; T1; E0w; +1r; E1r; T2; +1r; E1r; T2"   },
      {L_, "-a; +0w; T1; E0w; +1a; E1a; T2; +1a; E1a; T2"   },
      {L_, "-a; +0w; T1; E0w; +1c; E1c; T2; +1c; E1c; T2"   },

      {L_, "-a; +0r; T1; E0r; +1w; E1w; T2; +1w; E1w; T2"   },
      {L_, "-a; +0r; T1; E0r; +1r; E1r; T2; +1r; E1r; T2"   },
      {L_, "-a; +0r; T1; E0r; +1a; E1a; T2; +1a; E1a; T2"   },
      {L_, "-a; +0r; T1; E0r; +1c; E1c; T2; +1c; E1c; T2"   },

      {L_, "-a; +0a; T1; E0a; +1w; E1w; T2; +1w; E1w; T2"   },
      {L_, "-a; +0a; T1; E0a; +1r; E1r; T2; +1r; E1r; T2"   },
      {L_, "-a; +0a; T1; E0a; +1a; E1a; T2; +1a; E1a; T2"   },
      {L_, "-a; +0a; T1; E0a; +1c; E1c; T2; +1c; E1c; T2"   },

      {L_, "-a; +0c; T1; E0c; +1w; E1w; T2; +1w; E1w; T2"   },
      {L_, "-a; +0c; T1; E0c; +1r; E1r; T2; +1r; E1r; T2"   },
      {L_, "-a; +0c; T1; E0c; +1a; E1a; T2; +1a; E1a; T2"   },
      {L_, "-a; +0c; T1; E0c; +1c; E1c; T2; +1c; E1c; T2"   },
    };
    const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

    for (int i = 0; i < NUM_SCRIPTS; ++i) {
        enum { NUM_PAIRS = 4 };
        SocketPair socketPairs[NUM_PAIRS];

        const int NUM_PAIR = sizeof socketPairs / sizeof socketPairs[0];
        for (int j = 0; j < NUM_PAIR; ++j) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        ret = gg(mX, socketPairs, SCRIPTS[i].d_script,flags);
        if (SUCCESS != ret) {
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                std::printf("ERRORS detected while executing "
                            "the script at line %d:\n", LINE);
                std::fflush(stdout);
            }
        }
    }
    return numFailures;
}

int
bteso_EventManagerTester::testDeregisterSocketEvent(bteso_EventManager *mX,
                                                    int                 flags)
{
    int ret = 0, numFailures = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'testDeregisterSocketEvent' method\n"
                  "==========================================");
    }

    struct {
        int         d_line;
        const char *d_script;
    } SCRIPTS[] =
    {   // The reason to prefix "-a;" at the beginning of each script is that
        // we only use ONE eventmanager for all script here.

        // Test the event manager when one socket event exists.
      {L_, "-a; +0w; T1; -0w; E0; T0; +0w; E0w; T1"                       },
      {L_, "-a; +0r; T1; -0r; E0; T0; +0r; E0r; T1"                       },
      {L_, "-a; +0a; T1; -0a; E0; T0; +0a; E0a; T1"                       },
      {L_, "-a; +0c; T1; -0c; E0; T0; +0c; E0c; T1"                       },

        // Test the event manager when two socket events exist.
        // The two socket events are for the same socket handle.
      {L_, "-a; +0w; +0r; T2; -0w; E0r; -0r; T0; +0r; E0r; T1; -0; T0"    },
      {L_, "-a; +0r; +0w; T2; -0r; E0w; -0w; T0; +0w; E0w; T1; -0; T0"    },

        // The two socket events are for different socket handles.
      {L_, "-a; +0w; +1w; T2; -0w; E0; E1w; T1; -1w; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1r; T2; -0w; E0; E1r; T1; -1r; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1a; T2; -0w; E0; E1a; T1; -1a; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1c; T2; -0w; E0; E1c; T1; -1c; E1; T0; +0w; E0w; T1"},

      {L_, "-a; +0r; +1w; T2; -0r; E0; E1w; T1; -1w; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1r; T2; -0r; E0; E1r; T1; -1r; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1a; T2; -0r; E0; E1a; T1; -1a; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1c; T2; -0r; E0; E1c; T1; -1c; E1; T0; +0r; E0r; T1"},

      {L_, "-a; +0a; +1w; T2; -0a; E0; E1w; T1; -1w; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1r; T2; -0a; E0; E1r; T1; -1r; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1a; T2; -0a; E0; E1a; T1; -1a; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1c; T2; -0a; E0; E1c; T1; -1c; E1; T0; +0a; E0a; T1"},

      {L_, "-a; +0c; +1w; T2; -0c; E0; E1w; T1; -1w; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1r; T2; -0c; E0; E1r; T1; -1r; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1a; T2; -0c; E0; E1a; T1; -1a; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1c; T2; -0c; E0; E1c; T1; -1c; E1; T0; +0c; E0c; T1"},
    };
    const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

    for (int i = 0; i < NUM_SCRIPTS; ++i) {
        enum { NUM_PAIRS = 4 };
        SocketPair socketPairs[NUM_PAIRS];

        for (int j = 0; j < NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        ret = gg (mX, socketPairs, SCRIPTS[i].d_script, flags);
        if (SUCCESS != ret) {
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                    std::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    std::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
bteso_EventManagerTester::testDeregisterSocket(bteso_EventManager *mX,
                                               int                 flags)
{
    int ret = 0, numFailures = 0;

    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'testDeregisterSocket' method\n"
                  "=====================================");
    }

    struct {
        int         d_line;
        const char *d_script;
    } SCRIPTS[] =
    {   // The reason to prefix "-a;" at the beginning of each script is that
        // we only use ONE eventmanager for all script here.

        // Test the event manager when zero socket event exists.
      {L_, "-a; -0; E0; T0"                                             },

        // Test the event manager when one socket event exists.
      {L_, "-a; +0w; T1; -0; E0; T0; +0w; E0w; T1"                      },
      {L_, "-a; +0r; T1; -0; E0; T0; +0r; E0r; T1"                      },
      {L_, "-a; +0a; T1; -0; E0; T0; +0a; E0a; T1"                      },
      {L_, "-a; +0c; T1; -0; E0; T0; +0c; E0c; T1"                      },

        // Test the event manager when two socket events exist.
        // The two socket events are for the same socket handle.
      {L_, "-a; +0w; +0r; T2; -0; E0; T0; +0r; E0r; T1; -0; T0"         },
      {L_, "-a; +0r; +0w; T2; -0; E0; T0; +0w; E0w; T1; -0; T0"         },

        // The two socket events are for different socket handles.
      {L_, "-a; +0w; +1w; T2; -0; E0; E1w; T1; -1; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1r; T2; -0; E0; E1r; T1; -1; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1a; T2; -0; E0; E1a; T1; -1; E1; T0; +0w; E0w; T1"},
      {L_, "-a; +0w; +1c; T2; -0; E0; E1c; T1; -1; E1; T0; +0w; E0w; T1"},

      {L_, "-a; +0r; +1w; T2; -0; E0; E1w; T1; -1; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1r; T2; -0; E0; E1r; T1; -1; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1a; T2; -0; E0; E1a; T1; -1; E1; T0; +0r; E0r; T1"},
      {L_, "-a; +0r; +1c; T2; -0; E0; E1c; T1; -1; E1; T0; +0r; E0r; T1"},

      {L_, "-a; +0a; +1w; T2; -0; E0; E1w; T1; -1; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1r; T2; -0; E0; E1r; T1; -1; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1a; T2; -0; E0; E1a; T1; -1; E1; T0; +0a; E0a; T1"},
      {L_, "-a; +0a; +1c; T2; -0; E0; E1c; T1; -1; E1; T0; +0a; E0a; T1"},

      {L_, "-a; +0c; +1w; T2; -0; E0; E1w; T1; -1; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1r; T2; -0; E0; E1r; T1; -1; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1a; T2; -0; E0; E1a; T1; -1; E1; T0; +0c; E0c; T1"},
      {L_, "-a; +0c; +1c; T2; -0; E0; E1c; T1; -1; E1; T0; +0c; E0c; T1"},
    };
    const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

    for (int i = 0; i < NUM_SCRIPTS; ++i) {
        enum { NUM_PAIRS = 4 };
        SocketPair socketPairs[NUM_PAIRS];

        for (int j = 0; j < NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);
        if (SUCCESS != ret) {
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                    std::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    std::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
bteso_EventManagerTester::testDeregisterAll(bteso_EventManager *mX,
                                            int                 flags)
{
    int ret = 0, numFailures = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'testDeregisterAll' method\n"
                  "==================================");
    }

    struct {
        int         d_line;
        const char *d_script;
    } SCRIPTS[] =
    {
        // Test the event manager when zero socket event exists.
      {L_, "-a; T0"                                            },

        // Test the event manager when one socket event exists.
      {L_, "+0r; T1; -a; E0; T0; +0r; E0r; T1; -a; T0"         },
      {L_, "+0w; T1; -a; E0; T0; +0w; E0w; T1; -a; T0"         },
      {L_, "+0a; T1; -a; E0; T0; +0a; E0a; T1; -a; T0"         },
      {L_, "+0c; T1; -a; E0; T0; +0c; E0c; T1; -a; T0"         },

        // Test the event manager when two socket events exist.
        // The two socket events are for the same socket handle.
      {L_, "+0w; +0r; T2; -a; E0; T0; +0r; E0r; T1; -a; T0"    },
      {L_, "+0r; +0w; T2; -a; E0; T0; +0w; E0w; T1; -a; T0"    },

        // The two socket events are for different socket handles.
      {L_, "+0w; +1w; T2; -a; E0; E1; T0; +0w; E0w; T1; -a; T0"},
      {L_, "+0w; +1r; T2; -a; E0; E1; T0; +0w; E0w; T1; -a; T0"},
      {L_, "+0w; +1a; T2; -a; E0; E1; T0; +0w; E0w; T1; -a; T0"},
      {L_, "+0w; +1c; T2; -a; E0; E1; T0; +0w; E0w; T1; -a; T0"},

      {L_, "+0r; +1w; T2; -a; E0; E1; T0; +0r; E0r; T1; -a; T0"},
      {L_, "+0r; +1r; T2; -a; E0; E1; T0; +0r; E0r; T1; -a; T0"},
      {L_, "+0r; +1a; T2; -a; E0; E1; T0; +0r; E0r; T1; -a; T0"},
      {L_, "+0r; +1c; T2; -a; E0; E1; T0; +0r; E0r; T1; -a; T0"},

      {L_, "+0a; +1w; T2; -a; E0; E1; T0; +0a; E0a; T1; -a; T0"},
      {L_, "+0a; +1r; T2; -a; E0; E1; T0; +0a; E0a; T1; -a; T0"},
      {L_, "+0a; +1a; T2; -a; E0; E1; T0; +0a; E0a; T1; -a; T0"},
      {L_, "+0a; +1c; T2; -a; E0; E1; T0; +0a; E0a; T1; -a; T0"},

      {L_, "+0c; +1w; T2; -a; E0; E1; T0; +0c; E0c; T1; -a; T0"},
      {L_, "+0c; +1r; T2; -a; E0; E1; T0; +0c; E0c; T1; -a; T0"},
      {L_, "+0c; +1a; T2; -a; E0; E1; T0; +0c; E0c; T1; -a; T0"},
      {L_, "+0c; +1c; T2; -a; E0; E1; T0; +0c; E0c; T1; -a; T0"},
    };
    const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

    for (int i = 0; i < NUM_SCRIPTS; ++i) {
        enum { NUM_PAIRS = 4 };
        SocketPair socketPairs[NUM_PAIRS];

        for (int j = 0; j < NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);
        if (SUCCESS != ret) {
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                    const int LINE =  SCRIPTS[i].d_line;
                    std::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    std::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
bteso_EventManagerTester::testAccessors(bteso_EventManager *mX,
                                        int                 flags)
{
    int ret = 0, numfailures = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'testAccessors' method\n"
                  "==============================");
    }
    struct {
        int         d_line;
        const char *d_script;
    } SCRIPTS[] =
    { // The reason to put "-a;" at the beginning of each script is that we
      // only use ONE eventmanager for all script here.
      {L_, "-a; T0,0"},
      {L_, "-a; +0r; T1; E0r"},
      {L_, "-a; +0w; T1; E0w"},
      {L_, "-a; +0w; +0r; T2; E0rw"},
      {L_, "-a; +0w; +0r; T2; E0w; +1r; E1r"},
      {L_, "-a; +0w; +0r; T2; +1r; +1w; T2,1; E0rw"},
      {L_, "-a; +0w; +0r; T2; +1r; +1w; T2,1; +0r; E0r"},
      {L_, "-a; +0w; +0r; T2; +1r; +1w; T2,1; +0w; E0w"},
    };
    const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

    for (int i = 0; i < NUM_SCRIPTS; ++i) {
        enum { NUM_PAIRS = 4 };
        SocketPair socketPairs[NUM_PAIRS];

        const int NUM_PAIR = sizeof socketPairs / sizeof socketPairs[0];
        for (int j = 0; j < NUM_PAIR; ++j) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);
        if (SUCCESS != ret) {
            if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numfailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                    std::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    std::fflush(stdout);
                }
            }
        }
    }
    return numfailures;
}

int
bteso_EventManagerTester::testDispatch(bteso_EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;

    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'testDispatch' method\n"
                  "=============================");
    }
    {
        struct {
            int         d_line;
            const char *d_script;
        } SCRIPTS[] =
        { // The reason to put "-a;" at the beginning of each script is that
          // we only use ONE eventmanager for all script here.

            // Test the event manager when no socket event exists.
          {L_, "-a; Dn0,0"                                                  },
          {L_, "-a; Dn100,0"                                                },

            // Test the event manager when one socket event exists.
          {L_, "-a; +0w2; Dn,1"},
          {L_, "-a; W0,64; +0r24; Dn,1; -0; T0"                             },

            // Test the event manager when two socket events exist.
            // The two socket events are for the same socket handle.
          {L_, "-a; +0w64; +0r24; Dn,1; -0w; Di500,0; -0r; T0"              },
          {L_, "-a; +0r24; +0w64; Dn,1; -0w; Di500,0; -0r; T0"              },
          {L_, "-a; +0w64; +0r24; W0,64; Dn,2; -0w; Di,1; -0r; T0"          },

            // The two socket events are for different socket handles.
          {L_, "-a; +0w64; +1w24; W0,64; Dn,2; -0w; Di500,1; T1"            },
          {L_, "-a; +0w64; +1r24; W0,64; Dn,1; -0w; Di500,0; T1"            },
          {L_, "-a; +0w64; +1a;   W0,64; Dn,1; -0w; Di500,0; T1"            },

          {L_, "-a; +0r64; +1w24; W0,64; Dn,2; -1w; Di500,0; T1"            },
          {L_, "-a; +0r64; +1r24; W0,64; Dn,1; -0r; Di500,0; T1"            },
          {L_, "-a; +0r64; +1a;   W0,64; Dn,1; -0r; Di500,0; T1"            },

          {L_, "-a; +0a;   +1w24; Dn,1; -1w; Di500,0; T1"                   },
          {L_, "-a; +0a;   +1r64; W1,64; Dn,1; -0a; Di500,0; T1"            },

            // Test the event manager when multiple socket events exist.
          {L_, "-a; +0w64; +1r10; +2w100; +1w20; +3w60; Dn,4; -a; T0"       },

            // Test the event manager when a test script need to be executed in
            // the user-installed callback function.
          {L_, "-a; +0r24,{-a}; +0w64; W0,64; T2; Dn,1; E0; T0"             },
          {L_, "-a; +0r24,{-a}; +1r64; W0,64; T2; Dn,1; E0; E1; T0"         },

            // The following tests cannot be in a black-box test since
            // the order of iteration is undefined
            // {L_, "-a; +0r24,{-1}; +1r; W0,64; W1,32; T2; Dn,1; T1"        },
            // {L_, "-a; +0r24; +1r12,{-0}; W0,64; W1,32; T2; Dn,2; T1"      },
          {L_, "-a; W0,60; +0w64,{+2w100; +0r10}; Dn,1; -0w; Di,2; -a; T0"  },
          {L_, "-a; W0,60; +0w64,{-1r; +0r10}; +1r20; E1r; Dn,1; -0w; Di,1;"
                                                                    "-a; T0"},

            // Test the event manager when the socket is not writeable, which
            // forces the request to timeout.
#if defined(BSLS_PLATFORM__OS_WINDOWS)
          {L_, "+0w25710; Dn,1; +0w26000; Dn300,1; Dn120,0; -0w; T0"        },
          {L_, "+0w28720; Dn,1; +0w26000; Dn120,0; -0w; T0"                 },
#endif
#if defined(BSLS_PLATFORM__OS_AIX)
          {L_, "+0w8192; Dn,1; +0w8192; Dn500,1; Dn,1; -0w; T0"             },
          {L_, "+0w8192; Dn,1; +0w8192; Dn300,1; Dn,1; -0w; T0"             },
#endif
#if defined(BSLS_PLATFORM__OS_SOLARIS)
          {L_, "+0w73728; Dn,1; +0w26000; Dn150,0; -0w; T0"                 },
#endif
        };
        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            enum { NUM_PAIRS = 4 };
            SocketPair socketPairs[NUM_PAIRS];

            for (int j = 0; j < NUM_PAIRS; ++j) {
                socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
            }

            ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);
            if (SUCCESS != ret) {
                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++numFailures;
                    const int LINE =  SCRIPTS[i].d_line;
                    if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                        std::printf("Line: %d\n", LINE);
                        std::fflush(stdout);
                    }
                }
            }
        }
    }
    #ifdef BSLS_PLATFORM__OS_UNIX

    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("Testing 'Interrupt options'\n"
                  "===========================");
    }
    {
        mX->deregisterAll();
        if (flags & bteso_EventManagerTester::BTESO_ABORT) {
            BSLS_ASSERT(0 == mX->numEvents());
        }

        registerSignal(SIGSYS, signalHandler);  // a handler for SIGSYS.
                                            // Test the interrupt option in
                                            // the event manager.
        bcemt_Attribute attributes;
        pthread_t tid = pthread_self();
        enum { INFINITE = 0,  TIMEOUT };

        struct {
            int  d_line;
            int  d_timeFlag; //timeout/infinite
            int  d_flags;    // interrupt options
            int  d_expRet;   // dispatch return.
        } VALUES[] =
        //  line   dispatch      option                      expRet
        //  ----   --------      ------                      ------
        {
           { L_,    INFINITE,      0,                           1  },
           { L_,    INFINITE,   bteso_Flag::BTESO_ASYNC_INTERRUPT,   -1  },
           { L_,     TIMEOUT,   bteso_Flag::BTESO_ASYNC_INTERRUPT,   -1  },
           { L_,     TIMEOUT,      0,                           0  },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];
        bcemt_ThreadUtil::Handle threadHandle[NUM_VALUES];
        SocketPair socketPairs[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            socketPairs[i].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[i].setControlBufferOptions(BUF_LEN, 1);

            ThreadInfo threadInfo = {
                socketPairs[i].controlFd(), tid, VALUES[i].d_timeFlag, flags };
            int ret = bcemt_ThreadUtil::create(&threadHandle[i], attributes,
                                            threadSignalGenerator,
                                            &threadInfo);
            if (0 != ret)
            {
                std::printf("bcemt_ThreadUtil::create() call at line %d "
                            "failed. return: %d\n",
                            __LINE__, ret);
                std::fflush(stdout);
                return -1;
            }

            if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                std::printf("Created a thread %d; socket: %d\n",
                            threadHandle[i],
                            socketPairs[0].controlFd());
                std::fflush(stdout);
            }

            int bytes = 1;
            const char *cbScript = 0;  // dummy argument.
            bteso_EventManager::Callback readCb(
                    bdef_BindUtil::bind(&genericCb,
                                        bteso_EventType::BTESO_READ,
                                        i, bytes, mX,
                                        (SocketPair*)socketPairs, cbScript,
                                        flags));

            mX->registerSocketEvent(socketPairs[i].observedFd(),
                                    bteso_EventType::BTESO_READ,
                                    readCb);
            if (TIMEOUT == VALUES[i].d_timeFlag) {
                bdet_TimeInterval deadline(bdetu_SystemTime::now());
                bdet_TimeInterval period(5);
                deadline += period;        // timeout 5 seconds from now
                int ret = mX->dispatch(deadline, VALUES[i].d_flags);
                if (flags & bteso_EventManagerTester::BTESO_VERY_VERBOSE) {
                    P_(VALUES[i].d_line); P_(VALUES[i].d_timeFlag);
                    P_(VALUES[i].d_flags);
                    P_(VALUES[i].d_expRet); P(ret);
                }

                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(VALUES[i].d_expRet == ret);
                }
                else {
                    if (VALUES[i].d_expRet != ret) {
                        std::printf("%d: TEST FAILURE: Expected and actual "
                                    "values mismatch.\n",
                                    __LINE__);
                        std::fflush(stdout);
                    }
                }
            }
            else if (INFINITE == VALUES[i].d_timeFlag) {
                int rcode = mX->dispatch(VALUES[i].d_flags);
                if (VALUES[i].d_expRet != rcode) {
                    if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                        BSLS_ASSERT(0);
                    }
                    else {
                        ++numFailures;
                    }
                }
            }
            else {
                std::printf("%d is not a valid dispatch type.\n",
                            VALUES[i].d_timeFlag);
                std::fflush(stdout);
                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++numFailures;
                }
            }
            mX->deregisterAll();
            bcemt_ThreadUtil::join(threadHandle[i]);
        }
    }
    #endif
    return numFailures;
}

int
bteso_EventManagerTester::testDispatchPerformance(
                                          bteso_EventManager *mX,
                                          bsl::ostream&       stream,
                                          int                 numSocketPairs,
                                          int                 numMeasurements,
                                          int                 flags)
{

    int fails = 0, i = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("TESTING 'dispatch()' capacity\n"
                  "=============================");
    }
    {
        bslma_TestAllocator testAllocator(flags & BTESO_VERY_VERY_VERBOSE);
        SocketPair *socketPairs = (SocketPair *)
                  testAllocator.allocate(numSocketPairs * sizeof (SocketPair));

        bteso_EventManager::Callback *readCb =(bteso_EventManager::Callback *)
                  testAllocator.allocate(numSocketPairs * sizeof (*readCb));

        bdet_TimeInterval averageTimer;

        for (i = 0; i < numSocketPairs; i++) {
            bslalg_ScalarPrimitives::defaultConstruct(&socketPairs[i],
                                                      &testAllocator);

            bslalg_ScalarPrimitives::defaultConstruct(&readCb[i],
                                                      &testAllocator);
            int bytes = 1;
            const char *cbScript = 0;  // dummy argument.

            if (!socketPairs[i].isValid())  {
                if (flags & BTESO_VERBOSE) {
                    std::printf("Invalid socket pair index: %d\n", i);
                    std::fflush(stdout);
                }
                break;
            }
            socketPairs[i].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[i].setControlBufferOptions(BUF_LEN, 1);

            readCb[i] = bdef_BindUtil::bind(&genericCb,
                                            bteso_EventType::BTESO_READ,
                      i, bytes, mX, (SocketPair*)socketPairs, cbScript, flags);

            mX->registerSocketEvent(socketPairs[i].observedFd(),
                                    bteso_EventType::BTESO_READ,
                                    readCb[i]);
            if (i + 1 != mX->numEvents()) {
                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++fails;
                }
            }
            const int SAMPLE_DISTANCE = i / numMeasurements;
            bdet_TimeInterval timer;

            for (int j = 0; j < numMeasurements; ++j) {
                char ch = ' ';
                int idx = 0;
                if (0 == SAMPLE_DISTANCE) {
                    idx = j%(i+1);
                }
                else {
                    idx = j * SAMPLE_DISTANCE;
                }
                bteso_SocketImpUtil::write(socketPairs[idx].controlFd(),
                                           &ch, sizeof(char));
                bdet_TimeInterval t1, t2;
                t1 = bdetu_SystemTime::now();
                int ret = mX->dispatch(0);
                t2 = bdetu_SystemTime::now();
                timer += t2 - t1;
                averageTimer += t2 - t1;

                if (1 != ret) {
                    std::printf(" i: %d; dispatch return : %d; errno: %d\n",
                                i, ret, errno);
                    std::fflush(stdout);
                    if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                        BSLS_ASSERT(0);
                    }
                    else {
                        ++fails;
                    }
                }
            }
            int microseconds = (timer.seconds() * 1000000 +
                      timer.nanoseconds() / 1000) / numMeasurements;

            stream << microseconds << '\n' << bsl::flush;
        }

        for (int j = 0; j < i; j++){ //have to 'destructor' one by one
            bslalg_ScalarDestructionPrimitives::destroy(socketPairs+j);
            bslalg_ScalarDestructionPrimitives::destroy(readCb+j);
        }
        testAllocator.deallocate(readCb);
        testAllocator.deallocate(socketPairs);
    }
    return fails;
}

int
bteso_EventManagerTester::testRegisterPerformance(
                                           bteso_EventManager *mX,
                                           bsl::ostream&       stream,
                                           int                 numSockets,
                                           int                 numMeasurements,
                                           int                 flags)
{
    int fails = 0, i = 0;
    if (flags & bteso_EventManagerTester::BTESO_VERBOSE) {
        std::puts("TESTING 'registerSocketEvent() capacity'\n"
                  "========================================");
    }
    {
        bslma_TestAllocator testAllocator(flags & BTESO_VERY_VERY_VERBOSE);
        bteso_SocketHandle::Handle *socket = (bteso_SocketHandle::Handle *)
                  testAllocator.allocate(numSockets * sizeof (*socket));

        bteso_EventManager::Callback *readCb =(bteso_EventManager::Callback *)
                  testAllocator.allocate(numSockets * sizeof (*readCb));

        bdet_TimeInterval timer;

        for (i = 0; i < numSockets; i++) {
            bslalg_ScalarPrimitives::defaultConstruct(&socket[i],
                                                      &testAllocator);

            bslalg_ScalarPrimitives::defaultConstruct(&readCb[i],
                                                      &testAllocator);

            int ret = bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                 &socket[i],
                                 bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
            if (0 != ret) {
                std::printf(" i: %d; Invalid socket, ret: %d\n", i, ret);
                std::fflush(stdout);

                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++fails;
                }
            }
            const int SAMPLE_DISTANCE = i / numMeasurements;

            for (int j = 0; j < numMeasurements; ++j) {
                int bytes = 1;
                readCb[i] = bdef_BindUtil::bind(&readCallback,
                                                bteso_EventType::BTESO_READ,
                                                socket[j], bytes, mX);

                int idx = 0;
                if (0 == SAMPLE_DISTANCE) {
                    idx = j%(i+1);
                }
                else {
                    idx = j * SAMPLE_DISTANCE;
                }
                bdet_TimeInterval t1, t2;
                t1 = bdetu_SystemTime::now();
                mX->registerSocketEvent(socket[idx],
                                        bteso_EventType::BTESO_READ,
                                        readCb[i]);
                t2 = bdetu_SystemTime::now();
                timer += t2 - t1;
                mX->deregisterAll();
            }
            int microseconds = (timer.seconds() * 1000000 +
                                timer.nanoseconds() / 1000) / numMeasurements;
            stream << microseconds << '\n' << bsl::flush;
            if (0 != mX->numEvents()) {
                if (flags & bteso_EventManagerTester::BTESO_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++fails;
                }
            }
        }
        for (int j = 0; j < i; j++){ //have to 'destructor' one by one
            bteso_SocketImpUtil::close(socket[j]);
            // (socket+j)->~bteso_SocketHandle::Handle();
            bslalg_ScalarDestructionPrimitives::destroy(readCb+j);
        }
        testAllocator.deallocate(readCb);
        testAllocator.deallocate(socket);
    }
    return fails;
}

bteso_EventManagerTestPair::bteso_EventManagerTestPair(int verboseFlag)
: d_verboseFlag(verboseFlag)
{
    int rc = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                              d_fds, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
    if (d_verboseFlag) {
        bsl::printf("T%d: socketPair (%d, %d): %d\n",
                    bcemt_ThreadUtil::self(), d_fds[0], d_fds[1], rc);
    }
    if (0 != rc ) {
        d_validFlag = -1;
    }
    else {
        bteso_IoUtil::BlockingMode option = bteso_IoUtil::BTESO_NONBLOCKING;
        rc |= bteso_IoUtil::setBlockingMode(d_fds[0], option);
        if (d_verboseFlag) {
            bsl::printf("T%d: setBlockingMode (%d): %d\n",
                        bcemt_ThreadUtil::self(), d_fds[0], rc);
        }

        rc |= bteso_IoUtil::setBlockingMode(d_fds[1], option);
        if (d_verboseFlag) {
            bsl::printf("T%d: setBlockingMode (%d): %d\n",
                        bcemt_ThreadUtil::self(), d_fds[1], rc);
        }
        rc |= bteso_SocketOptUtil::setOption(d_fds[0],
                      bteso_SocketOptUtil::BTESO_TCPLEVEL,
                      bteso_SocketOptUtil::BTESO_TCPNODELAY, 1);
        if (d_verboseFlag) {
            bsl::printf("T%d: setOption (TCPNODELAY) (%d): %d\n",
                        bcemt_ThreadUtil::self(), d_fds[0], rc);
        }

        rc |= bteso_SocketOptUtil::setOption(d_fds[1],
                      bteso_SocketOptUtil::BTESO_TCPLEVEL,
                      bteso_SocketOptUtil::BTESO_TCPNODELAY, 1);
        if (d_verboseFlag) {
            bsl::printf("T%d: setOption (TCPNODELAY) (%d): %d\n",
                        bcemt_ThreadUtil::self(), d_fds[1], rc);
        }

        if (rc) {
            if (d_verboseFlag) {
                bsl::printf("T%d: Closing %d\n", bcemt_ThreadUtil::self(),
                            d_fds[1]);
                bteso_SocketImpUtil::close(d_fds[1]);
            }
            if (d_verboseFlag) {
                bsl::printf("T%d: Closing %d\n", bcemt_ThreadUtil::self(),
                            d_fds[0]);
                bteso_SocketImpUtil::close(d_fds[0]);
            }
            d_validFlag = -1;
        }
        else {
            d_validFlag = 0;
        }
    }
}

bteso_EventManagerTestPair::~bteso_EventManagerTestPair()
{
    if (d_verboseFlag) {
        bsl::printf("T%d: Closing %d\n", bcemt_ThreadUtil::self(),
                    d_fds[1]);
        bteso_SocketImpUtil::close(d_fds[1]);
    }
    if (d_verboseFlag) {
        bsl::printf("T%d: Closing %d\n", bcemt_ThreadUtil::self(),
                    d_fds[0]);
        bteso_SocketImpUtil::close(d_fds[0]);
    }
}

int
bteso_EventManagerTestPair::setObservedBufferOptions(int bufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM__OS_AIX
                                                     int watermark
#else
                                                     int  // watermark
#endif
                                                    )
    // Set the send and receive buffer sizes and the watermarks on the
    // observed socket handle to the specified 'bufferSize' and
    // 'watermark' values respectively.
{
    int ret = bteso_SocketOptUtil::setOption(d_fds[0],
              bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
              bteso_SocketOptUtil::BTESO_SENDBUFFER, bufferSize);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::setOption(d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER, bufferSize);
    if (0 != ret) {
        return ret;
    }

    #ifdef BSLS_PLATFORM__OS_AIX
    ret = bteso_SocketOptUtil::setOption(d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_SNDLOWAT, watermark);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::setOption(d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_RCVLOWAT, watermark);
    if (0 != ret) {
        return ret;
    }
    #endif

    return 0;
}

int
bteso_EventManagerTestPair::setControlBufferOptions(int bufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM__OS_AIX
                                                    int watermark
#else
                                                    int  // watermark
#endif
                                                    )
    // Set the send and receive buffer sizes and the watermarks on the
    // observed socket handle to the specified 'bufferSize' and
    // 'watermark' values respectively.
{
    int ret = bteso_SocketOptUtil::setOption(d_fds[1],
              bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
              bteso_SocketOptUtil::BTESO_SENDBUFFER, bufferSize);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::setOption(d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER, bufferSize);
    if (0 != ret) {
        return ret;
    }

    #ifdef BSLS_PLATFORM__OS_AIX
    ret = bteso_SocketOptUtil::setOption(d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_SNDLOWAT, watermark);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::setOption(d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_RCVLOWAT, watermark);
    if (0 != ret) {
        return ret;
    }

    #endif

    return 0;
}

int bteso_EventManagerTestPair::getObservedBufferOptions(int *sndBufferSize,
                                                         int *rcvBufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM__OS_AIX
                                                         int *sndLowat,
                                                         int *rcvLowat
#else
                                                         int *,  // sndLowat
                                                         int *   // rcvLowat
#endif
                                                        ) const
{
    int ret = -1;
    ret = bteso_SocketOptUtil::getOption(&sndBufferSize, d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_SENDBUFFER);
    if (0 != ret) {
        return ret;
    }
    ret = bteso_SocketOptUtil::getOption(&rcvBufferSize, d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
    if (0 != ret) {
        return ret;
    }

    #ifdef BSLS_PLATFORM__OS_AIX
    ret = bteso_SocketOptUtil::getOption(&rcvLowat, d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_RCVLOWAT);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::getOption(&sndLowat, d_fds[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_SNDLOWAT);
    if (0 != ret) {
        return ret;
    }
    #endif

    return 0;
}

int bteso_EventManagerTestPair::getControlBufferOptions(int *sndBufferSize,
                                                        int *rcvBufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM__OS_AIX
                                                        int *sndLowat,
                                                        int *rcvLowat
#else
                                                        int *,  // sndLowat
                                                        int *   // rcvLowat
#endif
                                                       ) const
{
    int ret = -1;
    ret = bteso_SocketOptUtil::getOption(&sndBufferSize, d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_SENDBUFFER);
    if (0 != ret) {
        return ret;
    }
    ret = bteso_SocketOptUtil::getOption(&rcvBufferSize, d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
    if (0 != ret) {
        return ret;
    }

    #ifdef BSLS_PLATFORM__OS_AIX
    ret = bteso_SocketOptUtil::getOption(&rcvLowat, d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_RCVLOWAT);
    if (0 != ret) {
        return ret;
    }

    ret = bteso_SocketOptUtil::getOption(&sndLowat, d_fds[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            SO_SNDLOWAT);
    if (0 != ret) {
        return ret;
    }
    #endif

    return 0;
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
