// btlso_eventmanagertester.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_eventmanagertester.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_eventmanagertester_cpp,"$Id$ $CSID$")

#include <btlso_flag.h>
#include <btlso_eventtype.h>
#include <btlso_socketimputil.h>
#include <btlso_eventmanager.h>
#include <btlso_socketimputil.h>
#include <btlso_socketoptutil.h>
#include <btlso_ioutil.h>
#include <btlso_platform.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bdlf_bind.h>
#include <bdlt_currenttime.h>
#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>
#include <bslma_testallocator.h>                // allocate memory
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <bsl_algorithm.h>  // max
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>                             // abort
#include <bsl_cstring.h>  // memset

#ifdef BSLS_PLATFORM_OS_UNIX

# include <bsl_c_signal.h>
# include <pthread.h>
# include <sys/types.h>
# include <sys/socket.h>                             // ::socketpair

#if defined(BSLS_PLATFORM_OS_LINUX)  ||                                      \
    defined(BSLS_PLATFORM_OS_CYGWIN) ||                                      \
    defined(BSLS_PLATFORM_OS_SOLARIS)
#   include <sys/resource.h>
# endif

#endif

#define BTESO_EVENTMANAGERTESTER_USE_RAW_SOCKETPAIR 0

namespace BloombergLP {

namespace btlso {

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::printf("%s = %d\n", #X, (X)); bsl::fflush(stdout);
    // Print identifier and value.
#define Q(X) bsl::printf("<| %s |>\n", #X); bsl::fflush(stdout);
    // Quote identifier literally.
#define P_(X) bsl::printf("%s = %d, ", #X, (X)); bsl::fflush(stdout);
    // P(X) without '\n'
#define L_ __LINE__                           // current Line number

// Test success and failure codes.
enum {
    e_FAIL = -1,
    e_SUCCESS = 0
};

enum {
    k_BUF_LEN = 8192                            // socket buffers' size
};

// It turns out different number of bytes are needed to fill in a connection
// pipe on different platforms, even though the same buffer sizes are set.
// That's the reason different data buffer sizes are specified on different
// platforms.

#if defined(BSLS_PLATFORM_OS_WINDOWS)
enum {
    k_READ_SIZE  = 8192,    // the size of buffer to read from the pipe
    k_WRITE_SIZE = 30000    // the size of buffer to write to the pipe
};
#elif defined (BSLS_PLATFORM_OS_UNIX)
enum {
    k_READ_SIZE  = 8192,    // the size of buffer to read from the pipe
    k_WRITE_SIZE = 73728    // the size of buffer to write to the pipe
};

struct ThreadInfo{
    // Use this struct to pass information to the helper thread.

    btlso::SocketHandle::Handle d_socket;      // client socket

    pthread_t                   d_tid;         // the id of the thread to
                                               // which a signal's delivered

    int                         d_timeoutFlag; // timeout dispatch or infinite

    int                         d_ctrlFlag;    // verbose, ABORT ......
};

#endif

typedef btlso::EventManagerTestPair SocketPair;

static void genericCb(btlso::EventType::Type  event,
                      int                     fd,
                      int                     bytes,
                      btlso::EventManager    *mX,
                      SocketPair             *fds,
                      const char             *cbScript,
                      int                     flags);

static int ggHelper(btlso::EventManager         *mX,
                    btlso::EventManagerTestPair *fds,
                    const char                  *test,
                    int                          flags);

extern "C"
void bteso_eventmanagertester_nullFunctor()
{
}

#ifdef BSLS_PLATFORM_OS_UNIX
extern "C"
void* bteso_eventmanagertester_threadSignalGenerator(void *arg)
    // Generate signal 'SIGSYS' and deliver it to a thread specified in 'arg'.
    // Note the test can only work on UNIX platforms since window doesn't
    // support signal operations.
{
    BSLS_ASSERT(arg);

    enum { k_BASE_TIME = 1000 };      // the basic sleep time in microseconds

    bslmt::ThreadUtil::microSleep(k_BASE_TIME);
                                      // The thread waits to make sure
                                      // the main thread is hanging in the
                                      // dispatch() call.

    ThreadInfo socketInfo = *(ThreadInfo*) arg;

    pthread_kill(socketInfo.d_tid, SIGSYS);

    if (socketInfo.d_ctrlFlag & btlso::EventManagerTester::k_VERY_VERBOSE) {
        bsl::printf("Thread %llu generated a SIGSYS signal.\n",
                    bslmt::ThreadUtil::selfIdAsUint64());
        bsl::fflush(stdout);
    }

    bslmt::ThreadUtil::microSleep(3 * k_BASE_TIME);

    pthread_kill(socketInfo.d_tid, SIGSYS);

    if (socketInfo.d_ctrlFlag & btlso::EventManagerTester::k_VERY_VERBOSE) {
        bsl::printf("Thread %llu delivered another SIGSYS signal to %d.\n",
                    bslmt::ThreadUtil::selfIdAsInt(),
                    bslmt::ThreadUtil::idAsInt(
                             bslmt::ThreadUtil::handleToId(socketInfo.d_tid)));
        bsl::fflush(stdout);
    }

    if (!socketInfo.d_timeoutFlag) {
        // We have to make the dispatch() return if timeout is not set.
        enum {
            k_BUF_SIZE = 1
        };
        char buf[k_BUF_SIZE];
        bsl::memset(buf, 0xAB, k_BUF_SIZE); // to keep purify happy

        int len = btlso::SocketImpUtil::write(socketInfo.d_socket,
                                              buf,
                                              sizeof buf);

        if (socketInfo.d_ctrlFlag
          & btlso::EventManagerTester::k_VERY_VERBOSE) {
            bsl::printf("Thread %llu writes %d bytes to socket %d.\n",
                        bslmt::ThreadUtil::selfIdAsUint64(),
                        len,
                        socketInfo.d_socket);
            bsl::fflush(stdout);
        }

        if (k_BUF_SIZE != len) {
            if (socketInfo.d_ctrlFlag & btlso::EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                bsl::printf("Thread %llu doesn't write the right number of"
                            " bytes to socket %d.\n",
                            bslmt::ThreadUtil::selfIdAsUint64(),
                            socketInfo.d_socket);
                bsl::fflush(stdout);
            }
        }
    }
    bslmt::ThreadUtil::microSleep(k_BASE_TIME);
    return 0;
}

static void signalHandler(int)
    // The signal handler does nothing.
{
    return;
}

static void registerSignal(int signo, void (*handler)(int))
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
    // Get the next command to be executed.  The command may include commands
    // to be executed in the user-installed callback function, which is
    // enclosed in '{' and '}'.  Return a pointer to the next command string,
    // on success and 0 if no valid command exists or the end of a command is
    // reached.
{
    BSLS_ASSERT("command shouldn't be null" && commandSeq);
    int curlyNotBalance = 0;

    // Check if the command is correct.
    while ('\0' != *commandSeq) {
        if ('{' == *commandSeq) {
            ++curlyNotBalance;
        }
        else if ('}' == *commandSeq) {
            --curlyNotBalance;
        }

        if (0 == curlyNotBalance && ';' == *commandSeq) {
            ++commandSeq;
            while (' ' == *commandSeq || '\t' == *commandSeq) ++commandSeq;
            return commandSeq;                                        // RETURN
        }
        ++commandSeq;
    }
    return 0;
}

static const char *get1stCbCommand(const char *commandSeq)
    // Get the first callback command to be executed in the user-installed
    // callback function, which is enclosed in '{' and '}'.  Return a pointer
    // to the command string, on success and 0 if no valid command or reaches
    // the end of the script.
{
    BSLS_ASSERT("command shouldn't be null" && commandSeq);

    while ('{'  != *commandSeq
        && ';'  != *commandSeq
        && '\0' != *commandSeq) {
        ++commandSeq;
    }

    if (';' == *commandSeq || '\0' == *commandSeq) {
        return 0;                                                     // RETURN
    }
    else {
        return ++commandSeq;   // skip '{'                            // RETURN
    }
}

static const char *getNextCbCommand(const char *cbCmd, int *errCode = 0)
    // Return the next callback command in the callback script, e.g., in
    // command "+0r5,{+1w20,{E0rw}; +0r18}", there are 2 commands to be
    // executed in the callback, this function is to return the next callback
    // command.  Return 0 if the end of script is reached or no valid command
    // exists. and set the 'errCode' to 'e_FAIL' if no valid command exists.
{
    int curly = 1;  // the  number of pairs of '{' and '}' flag

    while (curly) {
        if ('{' == *cbCmd) {
            curly++;
        }
        else if ('}' == *cbCmd) {
            curly--;
            if (0 == curly) { // No any more callback command
                return 0;                                             // RETURN
                              // If there're valid commands, a ';' should be
                              // before this '}'.
            }
        }
        else if ('\0' == *cbCmd) {// We reach the end of script.
            if (0 == curly) {
                *errCode = e_FAIL;
            }
            else {
                *errCode = e_SUCCESS;
            }
            return 0;                                                 // RETURN
        }
        else if (';' == *cbCmd) {
            if (1 == curly) {   // We still have more callback commands.
                ++cbCmd;        // We need to skip ';'.
                break;
            }
            else if (0 == curly) { // no any more callback command
                return 0;                                             // RETURN
            }
        }
        ++cbCmd;
    }
    while (' ' == *cbCmd || '\t' == *cbCmd) {
        ++cbCmd; // To skip space.
    }
    return cbCmd;
}

static void
genericCb(btlso::EventType::Type  event,
          int                     fd,
          int                     bytes,
          btlso::EventManager    *mX,
          SocketPair             *fds,
          const char             *cbScript,
          int                     flags)
    // This generic callback function performs 'event' specific action.
{
    if (0 > bytes) {
        if (flags & btlso::EventManagerTester::k_ABORT) {
            BSLS_ASSERT(0 < bytes);
        }
        else {
            bsl::printf(" Line: %d Error: Negative number of bytes for IO: %d"
                        " bytes to socket %d\n",
                        L_,
                        bytes,
                        fds[fd].observedFd());
            bsl::fflush(stdout);
        }
    }
    switch (event) {
      case btlso::EventType::e_READ: {
          enum { k_BUF_SIZE = 8192 };
          char buffer[k_BUF_SIZE];

          int rc = btlso::SocketImpUtil::read(buffer,
                                              fds[fd].observedFd(),
                                              bytes,
                                              0);

          if (flags & btlso::EventManagerTester::k_VERY_VERY_VERBOSE) {
              bsl::printf("Generic callback: read %d bytes.\n", rc);
              bsl::fflush(stdout);
          }

          if (bytes != rc) {
              if (flags & btlso::EventManagerTester::k_ABORT) {
                  BSLS_ASSERT("Read wrong number of bytes" && 0);
              }
              else {
                  bsl::printf("Error: read %d bytes from %d; "
                              "%d bytes expected.\n",
                              rc, fd, bytes);
                  bsl::fflush(stdout);
              }
          }
      } break;

      case btlso::EventType::e_WRITE: {
          char wBuffer[k_WRITE_SIZE + 1];      // data to write to the observed
                                               // file descriptor of a socket
                                               // pair
          bsl::memset(wBuffer,'4',sizeof wBuffer);

          int rc = btlso::SocketImpUtil::write(fds[fd].observedFd(),
                                               &wBuffer,
                                               bytes,
                                               0);

          if (flags & btlso::EventManagerTester::k_VERY_VERY_VERBOSE) {
              bsl::printf("Generic callback: wrote %d bytes.\n", rc);
              bsl::fflush(stdout);
          }

          if (bytes != rc) {
              if (flags & btlso::EventManagerTester::k_ABORT) {
                  BSLS_ASSERT("Wrote wrong number of bytes" && 0);
              }
              else {
                  bsl::printf("Error: (%d): Wrote %d bytes to %d "
                              "instead of %d bytes requested.\n",
                              L_, rc, fds[fd].observedFd(), bytes);
                  bsl::fflush(stdout);
              }
          }
      } break;

      case btlso::EventType::e_ACCEPT: {

      } break;

      case btlso::EventType::e_CONNECT: {

      } break;

      default: {
          BSLS_ASSERT("Invalid event code -- must be fixed." && 0);
      } break;
    }

    // If a callback script has been specified, then execute them here by
    // invoking ggHelper().

    while (cbScript) {
        int ret = ggHelper(mX, fds, cbScript, flags);

        if (flags & btlso::EventManagerTester::k_ABORT) {
            BSLS_ASSERT(e_SUCCESS == ret);
        }

        if (e_FAIL == ret) {
            bsl::puts("Callback command execution failed!");
            bsl::fflush(stdout);
            if (flags & btlso::EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
        }

        int errCode = 0;
        cbScript = getNextCbCommand(cbScript, &errCode);

        if (!cbScript) {  // Verify it's the end of script:
                          // make sure it's not due to any invalid command.
            if (e_FAIL == errCode) {
                bsl::puts("Script command is invalid in callback function.");
                bsl::fflush(stdout);
                if (flags & btlso::EventManagerTester::k_ABORT) {
                    BSLS_ASSERT(0);
                }
            }
        }
    }
}

static int ggHelper(btlso::EventManager         *mX,
                    btlso::EventManagerTestPair *fds,
                    const char                  *test,
                    int                          flags)
    // Execute a specified test script command in 'test'.  The specified 'fds'
    // is an array of connected socket pairs on which the user makes operations
    // managed by the specified event manager 'mX'.  The bit flag 'flags' is
    // used to control the process execution and test result output.  Return 0,
    // on success and the number of failures otherwise.
{
    BSLS_ASSERT(test);

    int  fd = -1, nt = 0;
    int  rc = e_FAIL, ret = 0;
    char buf[k_READ_SIZE], c[2];
    c[0] = -1;

    char wBuffer[k_WRITE_SIZE + 1];    // To write to a pipe.

    const char *cbScript = get1stCbCommand(test); // Executed in callback.

    switch (test[0]) {
      case 'T': {         // Commands such as "T5; T3,0" come here.
        rc = bsl::sscanf(test,
                         "T%u,%u",
                         reinterpret_cast<unsigned *>(&nt),
                         reinterpret_cast<unsigned *>(&fd));

        if (1 == rc) {
            if (nt != mX->numEvents()) {
                return e_FAIL;                                        // RETURN
            }
        }
        else if (2 == rc){
            if (nt != mX->numSocketEvents(fds[fd].observedFd())) {
                return e_FAIL;                                        // RETURN
            }
        }
        else {
            return e_FAIL;                                            // RETURN
        }
      } break;
      case '+': {
        int bytes;

        // Read the <fd> field.

        rc = bsl::sscanf(test + 1, "%d", &fd);
        if (1 != rc) {
            return e_FAIL;                                            // RETURN
        }
        char d = test[2];
        switch (d) {
          case 'r':                                             // FALL THROUGH
          case 'w': {
            rc = bsl::sscanf(test + 3, "%d", &bytes);
            if (1 == rc && 0 >= bytes) {
                return e_FAIL;                                        // RETURN
            }
            if (1 != rc) {
                bytes = -1;
            }
          } break;
          case 'a':                                             // FALL THROUGH
          case 'c': {
          } break;
          default: {
            return e_FAIL;                                            // RETURN
          }
        }

        // Create a callback object.

        btlso::EventManager::Callback cb;
        switch (d) {
          case 'r': {
            cb = bdlf::BindUtil::bind(&genericCb,
                                      btlso::EventType::e_READ,
                                      fd,
                                      bytes,
                                      mX,
                                      fds,
                                      cbScript,
                                      flags);

            mX->registerSocketEvent(fds[fd].observedFd(),
                                    btlso::EventType::e_READ,
                                    cb);
          } break;
          case 'w': {
            cb = bdlf::BindUtil::bind(&genericCb,
                                      btlso::EventType::e_WRITE,
                                      fd,
                                      bytes,
                                      mX,
                                      fds,
                                      cbScript,
                                      flags);

            mX->registerSocketEvent(fds[fd].observedFd(),
                                    btlso::EventType::e_WRITE,
                                    cb);
          } break;

          case 'a': {
            cb = bdlf::BindUtil::bind(&genericCb,
                                      btlso::EventType::e_ACCEPT,
                                      fd,
                                      bytes,
                                      mX,
                                      fds,
                                      cbScript,
                                      flags);

            mX->registerSocketEvent(fds[fd].observedFd(),
                                    btlso::EventType::e_ACCEPT,
                                    cb);
          } break;

          case 'c': {
            cb = bdlf::BindUtil::bind(&genericCb,
                                      btlso::EventType::e_CONNECT,
                                      fd,
                                      bytes,
                                      mX,
                                      fds,
                                      cbScript,
                                      flags);

            mX->registerSocketEvent(fds[fd].observedFd(),
                                    btlso::EventType::e_CONNECT,
                                    cb);
          } break;
          default: {
            return e_FAIL;                                            // RETURN
          }
        }
      } break;
      case '-': {             // Commands such as "-1w; -0r; -a; -2" come here.
        rc = bsl::sscanf(test, "-%u%[rwac]", (unsigned *)&fd, &c[0]);
        if (1 > rc) {
            rc = bsl::sscanf(test, "-%c", &c[0]);
            if (1 != rc || 'a' != c[0]) {
                return e_FAIL;                                        // RETURN
            }
            mX->deregisterAll();
            break;
        }

        if (1 == rc) {
            int ret = mX->deregisterSocket(fds[fd].observedFd());
            if (0 <= ret) {
                return e_SUCCESS;                                     // RETURN
            }
            else {
                return e_FAIL;                                        // RETURN
            }
        }

        // Start for "rc == 2".

        switch (c[0]) {
          case 'r': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      btlso::EventType::e_READ);

          } break;
          case 'w': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      btlso::EventType::e_WRITE);
          } break;
          case 'a': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      btlso::EventType::e_ACCEPT);
          } break;
          case 'c': {
            mX->deregisterSocketEvent(fds[fd].observedFd(),
                                      btlso::EventType::e_CONNECT);
          } break;
          default: {
            return e_FAIL;                                            // RETURN
          }
        }
      } break;
      case 'D': {
        int                msecs = 0, rc = -1, nbytes = 0;
        char               ch;
        int                flags = 0;
        bsls::TimeInterval deadline(bdlt::CurrentTime::now());

        if (3 == bsl::sscanf(test, "D%c%d,%d%n", &ch, &msecs, &rc, &nbytes)) {
            if (0 > msecs || 0 > rc) {
                return e_FAIL;                                        // RETURN
            }
            switch (ch) {
              case 'n': {
                flags = 0;
              } break;
              case 'i': {
                flags = btlso::Flag::k_ASYNC_INTERRUPT;
              } break;
              default: {
                return e_FAIL;                                        // RETURN
              }
            }
            deadline.addMilliseconds(msecs);
            ret = mX->dispatch(deadline, flags);

            if (rc != ret) {
                return e_FAIL;                                        // RETURN
            }
        }
        else if (2 == bsl::sscanf(test,
                                  "D%c,%u%n",
                                  &ch,
                                  reinterpret_cast<unsigned *>(&rc),
                                  &nbytes)) {
            if (0 > rc) {
                return e_FAIL;                                        // RETURN
            }

            switch (ch) {
              case 'n': {
                flags = 0;
              } break;
              case 'i': {
                flags = btlso::Flag::k_ASYNC_INTERRUPT;
              } break;
              default: {
                return e_FAIL;                                        // RETURN
              }
            }
            ret = mX->dispatch(flags);
            if (rc != ret) {
                return e_FAIL;                                        // RETURN
            }
        }
        else {
            return e_FAIL;                                            // RETURN
        }
      } break;
      case 'E': {
        rc = bsl::sscanf(test, "E%d%[acrw]", &fd, buf);

        if (rc == 2) {
            if (bsl::strchr(buf, 'a')) {
                ret = mX->isRegistered(fds[fd].observedFd(),
                                       btlso::EventType::e_ACCEPT);
                if (1 != ret) {
                      return e_FAIL;                                  // RETURN
                }
            }

            if (bsl::strchr(buf, 'c')) {
                ret = mX->isRegistered(fds[fd].observedFd(),
                                       btlso::EventType::e_CONNECT);
                if (1 != ret) {
                    return e_FAIL;                                    // RETURN
                }
            }

            if (bsl::strchr(buf, 'r')) {
                ret = mX->isRegistered(
                       fds[fd].observedFd(), btlso::EventType::e_READ);
                if (1 != ret) {
                    return e_FAIL;                                    // RETURN
                }
            }

            if (bsl::strchr(buf, 'w')) {
                ret = mX->isRegistered(fds[fd].observedFd(),
                                       btlso::EventType::e_WRITE);
                if (1 != ret) {
                    return e_FAIL;                                    // RETURN
                }
            }
        }
        else {
            ret = mX->numSocketEvents(fds[fd].observedFd());
            if (0 != ret) {
                return e_FAIL;                                        // RETURN
            }
        }
      } break;
      case 'R': {
        int bytes = 0;
        rc = bsl::sscanf(test,
                         "R%d,%u",
                         &fd,
                         reinterpret_cast<unsigned int *>(&bytes));

        if (2 != rc) {
            return e_FAIL;                                            // RETURN
        }

        int rc = btlso::SocketImpUtil::read(buf,
                                            fds[fd].observedFd(),
                                            bytes,
                                            0);
        if (0 >= rc) {
            return e_FAIL;                                            // RETURN
        }
      } break;
      case 'W': {
        int bytes = 0;
        rc = bsl::sscanf(test, "W%d,%d", &fd, &bytes);

        if (2 != rc) {
            return e_FAIL;                                            // RETURN
        }

        bsl::memset(wBuffer, 0xAB, sizeof wBuffer); // to keep purify happy

        rc = btlso::SocketImpUtil::write(fds[fd].controlFd(),
                                         &wBuffer,
                                         bytes);
        if (0 >= rc) {
            return e_FAIL;                                            // RETURN
        }
      } break;
      case 'S': {
        int milliSeconds = 0;
        rc = bsl::sscanf(test, "S%d", &milliSeconds);
        if (1 != rc) {
            return e_FAIL;                                            // RETURN
        }
        bslmt::ThreadUtil::microSleep(milliSeconds * 1000);
      } break;
      default: {
        return e_FAIL;                                                // RETURN
      }
    }
    return e_SUCCESS;
}

                        // ------------------------
                        // class EventManagerTester
                        // ------------------------

int EventManagerTester::gg(EventManager *mX,
                           SocketPair   *fds,
                           const char   *script,
                           int           flags)
    // Break up the passed test script into individual test commands and
    // execute each command by invoking ggHelper().
    // Return 0, on success and  the number of failures otherwise.
{
    BSLS_ASSERT(script);
    const char *originalScript = script;

    int fails = 0;
    if ((flags & EventManagerTester::k_DRY_RUN)
      | (flags & EventManagerTester::k_VERY_VERBOSE)) {
        bsl::printf("Executing: %s\n", script);
        bsl::fflush(stdout);
    }

    if (flags & EventManagerTester::k_DRY_RUN) {
        return 0;                                                     // RETURN
    }

    while (script) {
        int ret = ggHelper(mX, fds, script, flags);
        if (e_SUCCESS != ret) {
            bsl::printf("\"%s\" FAILED.\n%*c^\n\n",
                        originalScript,
                        static_cast<int>(script - originalScript + 1),
                        ' ');
            bsl::fflush(stdout);
            if (flags & EventManagerTester::k_ABORT) {
                std::abort();
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
EventManagerTester::testRegisterSocketEvent(EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;
    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing testRegisterSocketEvent() method\n"
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
        enum { k_NUM_PAIRS = 4 };
        SocketPair socketPairs[k_NUM_PAIRS];

        const int NUM_PAIR = sizeof socketPairs / sizeof socketPairs[0];

        for (int j = 0; j < NUM_PAIR; ++j) {
            socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
        }

        ret = gg(mX, socketPairs, SCRIPTS[i].d_script,flags);

        if (e_SUCCESS != ret) {
            if (flags & EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                bsl::printf("ERRORS detected while executing "
                            "the script at line %d:\n", LINE);
                bsl::fflush(stdout);
            }
        }
    }
    return numFailures;
}

int
EventManagerTester::testDeregisterSocketEvent(EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;
    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'testDeregisterSocketEvent' method\n"
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
        enum { k_NUM_PAIRS = 4 };
        SocketPair socketPairs[k_NUM_PAIRS];

        for (int j = 0; j < k_NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
        }

        ret = gg (mX, socketPairs, SCRIPTS[i].d_script, flags);

        if (e_SUCCESS != ret) {
            if (flags & EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & EventManagerTester::k_VERY_VERBOSE) {
                    bsl::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    bsl::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
EventManagerTester::testDeregisterSocket(EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;

    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'testDeregisterSocket' method\n"
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
        enum { k_NUM_PAIRS = 4 };
        SocketPair socketPairs[k_NUM_PAIRS];

        for (int j = 0; j < k_NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
        }

        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);

        if (e_SUCCESS != ret) {
            if (flags & EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & EventManagerTester::k_VERY_VERBOSE) {
                    bsl::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    bsl::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
EventManagerTester::testDeregisterAll(EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;
    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'testDeregisterAll' method\n"
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
        enum { k_NUM_PAIRS = 4 };
        SocketPair socketPairs[k_NUM_PAIRS];

        for (int j = 0; j < k_NUM_PAIRS; ++j) {
            socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
        }

        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);

        if (e_SUCCESS != ret) {
            if (flags & EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numFailures += ret;
                if (flags & EventManagerTester::k_VERY_VERBOSE) {
                    const int LINE =  SCRIPTS[i].d_line;
                    bsl::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    bsl::fflush(stdout);
                }
            }
        }
    }
    return numFailures;
}

int
EventManagerTester::testAccessors(EventManager *mX, int flags)
{
    int ret = 0, numfailures = 0;
    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'testAccessors' method\n"
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
        enum { k_NUM_PAIRS = 4 };
        SocketPair socketPairs[k_NUM_PAIRS];

        const int NUM_PAIR = sizeof socketPairs / sizeof socketPairs[0];
        for (int j = 0; j < NUM_PAIR; ++j) {
            socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
        }

        ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);

        if (e_SUCCESS != ret) {
            if (flags & EventManagerTester::k_ABORT) {
                BSLS_ASSERT(0);
            }
            else {
                numfailures += ret;
                const int LINE =  SCRIPTS[i].d_line;
                if (flags & EventManagerTester::k_VERY_VERBOSE) {
                    bsl::printf("ERRORS detected while executing "
                                "the script at line %d:\n", LINE);
                    bsl::fflush(stdout);
                }
            }
        }
    }
    return numfailures;
}

int
EventManagerTester::testDispatch(EventManager *mX, int flags)
{
    int ret = 0, numFailures = 0;

    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'testDispatch' method\n"
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
#if defined(BSLS_PLATFORM_OS_WINDOWS)
          {L_, "+0w25710; Dn,1; +0w26000; Dn300,1; Dn120,0; -0w; T0"        },
          {L_, "+0w28720; Dn,1; +0w26000; Dn120,0; -0w; T0"                 },
#endif
#if defined(BSLS_PLATFORM_OS_AIX)
          {L_, "+0w131072; Dn,1; +0w8192; Dn300,1; Dn150,0; -0w; T0"        },
          {L_, "+0w131072; Dn,1; +0w8192; Dn120,1; Dn150,0; -0w; T0"        },
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)
          {L_, "+0w73728; Dn,1; +0w26000; Dn150,0; -0w; T0"                 },
#endif
        };
        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            enum { k_NUM_PAIRS = 4 };
            SocketPair socketPairs[k_NUM_PAIRS];

#ifdef BSLS_PLATFORM_OS_HPUX
            // For some reason, sockets on HPUX are woozy for the first ~ 20 ms
            // or so after they're created, after that they seem to be OK.  In
            // a polling interface, this just means events will take a few
            // cycles to catch up.

            bslmt::ThreadUtil::microSleep(40 * 1000);
#endif

            for (int j = 0; j < k_NUM_PAIRS; ++j) {
                socketPairs[j].setObservedBufferOptions(k_BUF_LEN, 1);
                socketPairs[j].setControlBufferOptions(k_BUF_LEN, 1);
            }

            ret = gg(mX, socketPairs, SCRIPTS[i].d_script, flags);

            if (e_SUCCESS != ret) {
                if (flags & EventManagerTester::k_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++numFailures;
                    const int LINE =  SCRIPTS[i].d_line;
                    if (flags & EventManagerTester::k_VERY_VERBOSE) {
                        bsl::printf("Line: %d\n", LINE);
                        bsl::fflush(stdout);
                    }
                }
            }
        }
    }
#ifdef BSLS_PLATFORM_OS_UNIX

    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("Testing 'Interrupt options'\n"
                  "===========================");
    }
    {
        mX->deregisterAll();
        if (flags & EventManagerTester::k_ABORT) {
            BSLS_ASSERT(0 == mX->numEvents());
        }

        registerSignal(SIGSYS, signalHandler);  // a handler for SIGSYS.
                                                // Test the interrupt option in
                                                // the event manager.
        bslmt::ThreadAttributes attributes;
        pthread_t tid = pthread_self();

        enum { INFINITE = 0,  TIMEOUT };

        struct {
            int  d_line;
            int  d_timeFlag; //timeout/infinite
            int  d_flags;    // interrupt options
            int  d_expRet;   // dispatch return.
        } VALUES[] =
        //  line   dispatch      option                           expRet
        //  ----   --------      ------                           ------
        {
           { L_,    INFINITE,      0,                                 1  },
           { L_,    INFINITE,      btlso::Flag::k_ASYNC_INTERRUPT,   -1  },
           { L_,     TIMEOUT,      btlso::Flag::k_ASYNC_INTERRUPT,   -1  },
           { L_,     TIMEOUT,      0,                                 0  },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];
        bslmt::ThreadUtil::Handle threadHandle[NUM_VALUES];
        SocketPair socketPairs[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            socketPairs[i].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[i].setControlBufferOptions(k_BUF_LEN, 1);

            ThreadInfo threadInfo = {
                socketPairs[i].controlFd(),
                tid,
                VALUES[i].d_timeFlag,
                flags
            };

            int ret = bslmt::ThreadUtil::create(
                               &threadHandle[i],
                               attributes,
                               &bteso_eventmanagertester_threadSignalGenerator,
                               &threadInfo);
            if (0 != ret) {
                bsl::printf("bslmt::ThreadUtil::create() call at line %d "
                            "failed. return: %d\n",
                            __LINE__, ret);
                bsl::fflush(stdout);
                return -1;                                            // RETURN
            }

            if (flags & EventManagerTester::k_VERY_VERBOSE) {
#define LLU BSLS_BSLTESTUTIL_FORMAT_U64
                bsl::printf("Created a thread " LLU "; socket: %u\n",
                            bslmt::ThreadUtil::idAsUint64(threadHandle[i]),
                            socketPairs[0].controlFd());
                bsl::fflush(stdout);
#undef LLU
            }

            int bytes = 1;
            const char *cbScript = 0;  // dummy argument.
            EventManager::Callback readCb(
                   bdlf::BindUtil::bind(&genericCb,
                                        EventType::e_READ,
                                        i,
                                        bytes,
                                        mX,
                                        static_cast<SocketPair *>(socketPairs),
                                        cbScript,
                                        flags));

            mX->registerSocketEvent(socketPairs[i].observedFd(),
                                    EventType::e_READ,
                                    readCb);

            if (TIMEOUT == VALUES[i].d_timeFlag) {
                bsls::TimeInterval deadline(bdlt::CurrentTime::now());
                bsls::TimeInterval period(5);
                deadline += period;        // timeout 5 seconds from now

                int ret = mX->dispatch(deadline, VALUES[i].d_flags);

                if (flags & EventManagerTester::k_VERY_VERBOSE) {
                    P_(VALUES[i].d_line); P_(VALUES[i].d_timeFlag);
                    P_(VALUES[i].d_flags);
                    P_(VALUES[i].d_expRet); P(ret);
                }

                if (flags & EventManagerTester::k_ABORT) {
                    BSLS_ASSERT(VALUES[i].d_expRet == ret);
                }
                else {
                    if (VALUES[i].d_expRet != ret) {
                        bsl::printf("%d: TEST FAILURE: Expected and actual "
                                    "values mismatch.\n",
                                    __LINE__);
                        bsl::fflush(stdout);
                    }
                }
            }
            else if (INFINITE == VALUES[i].d_timeFlag) {
                int rcode = mX->dispatch(VALUES[i].d_flags);
                if (VALUES[i].d_expRet != rcode) {
                    if (flags & EventManagerTester::k_ABORT) {
                        BSLS_ASSERT(0);
                    }
                    else {
                        ++numFailures;
                    }
                }
            }
            else {
                bsl::printf("%d is not a valid dispatch type.\n",
                            VALUES[i].d_timeFlag);
                bsl::fflush(stdout);
                if (flags & EventManagerTester::k_ABORT) {
                    BSLS_ASSERT(0);
                }
                else {
                    ++numFailures;
                }
            }
            mX->deregisterAll();
            bslmt::ThreadUtil::join(threadHandle[i]);
        }
    }
#endif
    return numFailures;
}

int
EventManagerTester::testDispatchPerformance(EventManager *mX,
                                            const char   *pollingMechName,
                                            int           flags)
{
#if defined(BSLS_PLATFORM_OS_HPUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
    enum { k_NUM_MEASUREMENTS = 1 };
#else
    enum { k_NUM_MEASUREMENTS = 10 };
#endif
    int         fails = 0, i = 0;
    const char *cbScript = 0;  // dummy argument.

    if (flags & EventManagerTester::k_VERBOSE) {
        bsl::puts("TESTING BUSY 'dispatch()' capacity\n"
                  "==================================");
    }

    bsl::cout << "Enter args: <numPairs> <fractionBusy> <timeOut> <R | N>\n"
                          "    where 'R' means do reads, 'N' means no reads\n";

    int                numSocketPairs;
    double             fractionBusy;
    double             timeOutDouble;
    bsls::TimeInterval timeOut;
    char               reads;    // 'R' for do reads, 'N' for no reads

    {
        bsl::string inString;
        getline(bsl::cin, inString);
        bsl::stringstream ss(inString);

        bool inputSuccess = false;
        ss >> numSocketPairs >> fractionBusy >> timeOutDouble >> reads;
        if (ss.fail()) {
            bsl::cout << "Bad or incomplete input.";
        }
        else if (numSocketPairs < 11) {
            bsl::cout << "'numPairs' must be > 10.";
        }
        else if (fractionBusy < 0.0 || fractionBusy > 1.0) {
            bsl::cout << "'fractionBusy' must be in range [ 0.0, 1.0 ].";
        }
        else if (timeOutDouble < 0.0 || timeOutDouble > 1.0) {
            bsl::cout << "'timeOut' must be in range [ 0.0, 1.0 ].";
        }
        else if ('R' != reads && 'N' != reads) {
            bsl::cout << "'reads' must be 'R' or 'N'.";
        }
        else {
            inputSuccess = true;
        }
        if (!inputSuccess) {
            bsl::cout << "  Test not run.\n";
            return 1;                                                 // RETURN
        }

        timeOut = timeOutDouble;
    }

#ifdef BSLS_PLATFORM_OS_UNIX
    const unsigned  filesNeeded = 2 * numSocketPairs + 10;
    struct ::rlimit rl;

    int rc = getrlimit(RLIMIT_NOFILE, &rl);

    BSLS_ASSERT_OPT(0 == rc);

    if (filesNeeded > rl.rlim_cur) {
        bsl::cout << "Process is currently limited to " << rl.rlim_cur / 2 <<
                                 " socket pairs.  You need to 'ulimit -n " <<
                                                          filesNeeded << "'\n";
        return -1;                                                    // RETURN
    }
#endif

    bsl::stringstream outFileNameSS;
    outFileNameSS << "tmp." << pollingMechName << "_dsp";

#if   defined(BSLS_PLATFORM_OS_LINUX)
    outFileNameSS << "_lnx";
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    outFileNameSS << "_sun";
#elif defined(BSLS_PLATFORM_OS_HPUX)
    outFileNameSS << "_hp_";
#elif defined(BSLS_PLATFORM_OS_AIX)
    outFileNameSS << "_aix";
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
    outFileNameSS << "_fre";
#elif defined(BSLS_PLATFORM_OS_CYGWIN)
    outFileNameSS << "_cyg";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    outFileNameSS << "_win";
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    outFileNameSS << "_darwin";
#else
#   error unrecognized platform
#endif

    outFileNameSS << (sizeof(void *) * 8);

    outFileNameSS << '_' << numSocketPairs;
    outFileNameSS << '_' << fractionBusy;
    outFileNameSS << '_' << timeOutDouble;
    outFileNameSS << (('N' == reads) ? "_noReads.txt" : "_timeReads.txt");

    bsl::cout << "testDispatchPerformance:\n"
                 "Polling Mechanism: " << pollingMechName <<     bsl::endl <<
                 "Num Socket Pairs:  " << numSocketPairs  <<     bsl::endl <<
                 "Fraction Busy:     " << fractionBusy    <<     bsl::endl <<
                 "Timeout:           " << timeOutDouble   <<     bsl::endl <<
                 (('N' == reads) ? "No reads\n" : "Time reads\n") <<
                 "Output in:         " << outFileNameSS.str() << bsl::endl;

    bsl::ofstream os(outFileNameSS.str().c_str());

    {
        bslma::TestAllocator testAllocator(
                              flags & EventManagerTester::k_VERY_VERY_VERBOSE);

        SocketPair *socketPairs = static_cast<SocketPair *>(
                 testAllocator.allocate(numSocketPairs * sizeof (SocketPair)));

        EventManager::Callback *readCb =static_cast<EventManager::Callback *>(
                    testAllocator.allocate(numSocketPairs * sizeof (*readCb)));

        bool *writtenFlags = static_cast<bool *>(
                                       testAllocator.allocate(numSocketPairs));

        for (i = 0; i < numSocketPairs; ++i) {
            bslalg::ScalarPrimitives::defaultConstruct(&socketPairs[i],
                                                       &testAllocator);

            bslalg::ScalarPrimitives::defaultConstruct(&readCb[i],
                                                       &testAllocator);
        }

        for (i = 0; i < numSocketPairs; ++i) {
            if (!socketPairs[i].isValid())  {
                bsl::printf("Invalid socket pair index: %d\n", i);
                bsl::fflush(stdout);
                return 1;                                             // RETURN
            }
        }

#ifdef BSLS_PLATFORM_OS_HPUX
        // On HPUX, newly created sockets need about ~ 20ms to wake up.

        bslmt::ThreadUtil::microSleep(40 * 1000);
#endif

        for (i = 0; i < numSocketPairs; ++i) {
            int bytes = 1;

            socketPairs[i].setObservedBufferOptions(k_BUF_LEN, 1);
            socketPairs[i].setControlBufferOptions(k_BUF_LEN, 1);

            if ('N' == reads) {
                readCb[i] = &bteso_eventmanagertester_nullFunctor;
            }
            else {
                readCb[i] = bdlf::BindUtil::bind(&genericCb,
                                                 EventType::e_READ,
                                                 i,
                                                 bytes,
                                                 mX,
                                                 socketPairs,
                                                 cbScript,
                                                 flags);
            }

            mX->registerSocketEvent(socketPairs[i].observedFd(),
                                    EventType::e_READ,
                                    readCb[i]);

            if (i + 1 != mX->numEvents()) {
                bsl::cout << "Socket registration " << i << "failed\n";
                ++fails;
            }
        }

        double averageResult = 0;

        for (i = numSocketPairs - 10; !fails && i < numSocketPairs; i++) {
            const int SAMPLE_DISTANCE = (i + 1) / k_NUM_MEASUREMENTS;
            int       toWrite = bsl::max((int) ((i + 1) * fractionBusy), 1);

            BSLS_ASSERT_OPT(toWrite <= i + 1);
            BSLS_ASSERT_OPT(toWrite >= 1);

            bsls::TimeInterval timer;
            int actualNumMeasurements = 0;

            for (int j = 0; j < k_NUM_MEASUREMENTS; ++j) {
                char ch = ' ';
                int  idx = 0;
                int  skip;

                if (0 == SAMPLE_DISTANCE) {
                    idx = j % (i + 1);
                    skip = 1;
                }
                else {
                    idx = j * SAMPLE_DISTANCE;
                    skip = SAMPLE_DISTANCE;
                }

                bsl::memset(writtenFlags, 0, i + 1);
                for (int leftToWrite = toWrite;
                     leftToWrite;
                     idx = (idx + skip) % (i+1)) {

                    while (writtenFlags[idx]) {
                        idx = (idx + 1) % (i + 1);
                    }

                    SocketImpUtil::write(socketPairs[idx].controlFd(),
                                         &ch,
                                         sizeof(char));
                    writtenFlags[idx] = true;
                    --leftToWrite;
                }

                // sleep to allow for latency in the sockets, which are not
                // the fault of the dispatcher

                bslmt::ThreadUtil::microSleep(10 * 1000);

                int ret = 0;
                int attempts = 0;
                bsls::TimeInterval t1, t2;
                for (; ret < toWrite; ++attempts) {
                    if (attempts) {
                        bsl::printf(
                              "Repeat on i:%d, %d events missing, errno: %d\n",
                              i, toWrite - ret, errno);
                        bsl::fflush(stdout);
                        if (flags & EventManagerTester::k_ABORT) {
                            BSLS_ASSERT(0);
                        }
                    }

                    if (timeOut > 0.0) {
                        t1 = bdlt::CurrentTime::now();
                        ret += mX->dispatch(timeOut, 0);
                        t2 = bdlt::CurrentTime::now();
                    }
                    else {
                        t1 = bdlt::CurrentTime::now();
                        ret += mX->dispatch(0);
                        t2 = bdlt::CurrentTime::now();
                    }

                    if (t2 >= t1) {
                        // On Linux, time sometimes goes backward according to
                        // 'now()'.

                        timer += t2 - t1;
                        ++actualNumMeasurements;
                    }
                }

                if ('N' == reads) {
                    // actually do all the reads so there is no data in any of
                    // the sockets.

                    for (int k = 0; k <= i; ++k) {
                        if (writtenFlags[k]) {
                            genericCb(EventType::e_READ,
                                      k,
                                      1,
                                      mX,
                                      socketPairs,
                                      cbScript,
                                      flags);
                        }
                    }
                }
            }

            if (k_NUM_MEASUREMENTS - actualNumMeasurements > 2) {
                bsl::printf(" i: %d Time went backward %d out of %d times\n",
                                 i, k_NUM_MEASUREMENTS - actualNumMeasurements,
                                                           k_NUM_MEASUREMENTS);
                bsl::fflush(stdout);
            }

            if (0 == actualNumMeasurements) {
                actualNumMeasurements = 1;
            }

            double microseconds =
                  (1e6 * timer.totalSecondsAsDouble() / actualNumMeasurements);

            os        << microseconds << bsl::endl;
            bsl::cout << microseconds << bsl::endl;
            averageResult += microseconds;
        }

        averageResult /= 10;

        os        << "\n" << averageResult << "\n";
        bsl::cout << "\n" << averageResult << "\n";

        for (int j = 0; j < i; j++){ //have to 'destructor' one by one
            bslalg::ScalarDestructionPrimitives::destroy(socketPairs+j);
            bslalg::ScalarDestructionPrimitives::destroy(readCb+j);
        }

        testAllocator.deallocate(readCb);
        testAllocator.deallocate(socketPairs);
        testAllocator.deallocate(writtenFlags);
    }

    os.close();

    return fails;
}

int
EventManagerTester::testRegisterPerformance(EventManager *mX, int flags)
{
    enum { k_NUM_MEASUREMENTS = 10 };

    int numSockets;

    bsl::cout << "Enter arg: <num sockets>\n";
    bsl::cin >> numSockets;

    if (numSockets < 10) {
        bsl::cout << "<num sockets> must be >= 10\n";
        return 1;                                                     // RETURN
    }

    if (numSockets & 1) {
        bsl::cout << "<num sockets> must be even\n";
        return 1;                                                     // RETURN
    }

    BSLS_ASSERT_OPT(numSockets >= 10);

    EventManager::Callback nullCb = &bteso_eventmanagertester_nullFunctor;

    bslma::TestAllocator testAllocator(
                              flags & EventManagerTester::k_VERY_VERY_VERBOSE);

    SocketHandle::Handle *socket = static_cast<SocketHandle::Handle *>(
                        testAllocator.allocate(numSockets * sizeof (*socket)));

    for (int ii = 0; ii < numSockets; ii += 2) {
        bslalg::ScalarPrimitives::defaultConstruct(&socket[ii],
                                                   &testAllocator);
        bslalg::ScalarPrimitives::defaultConstruct(&socket[ii + 1],
                                                   &testAllocator);

#if BTESO_EVENTMANAGERTESTER_USE_RAW_SOCKETPAIR
        // We found creating 40,000 sockets in the -1 and -2 cases of
        // defaulteventmanager_*.t.cpp would crash ibm7.  If we create them
        // this way it will be OK, though some test drivers may complain in
        // other ways.

        static bool firstTime = true;
        if (firstTime) {
            firstTime = false;
            bsl::cout <<
               "testRegisterPerformance: Fix me: using '::socketpair' kluge\n";
        }

        int ret = ::socketpair(AF_UNIX, SOCK_STREAM, 0, &socket[ii]);
#else
        int ret = SocketImpUtil::open<IPv4Address>(
                                               &socket[ii],
                                               SocketImpUtil::k_SOCKET_STREAM);
        ret    |= SocketImpUtil::open<IPv4Address>(
                                               &socket[ii + 1],
                                               SocketImpUtil::k_SOCKET_STREAM);
#endif
        if (0 != ret) {
            bsl::cout << "Unable to open more than " << ii << " sockets\n";
#ifdef BSLS_PLATFORM_OS_UNIX
            bsl::cout << "Try 'ulimit -n " << (numSockets + 10) << "'\n";
#endif
            return 1;                                                 // RETURN
        }
    }

    bsls::TimeInterval t1, t2;
    double             totalMicroseconds = 0;

    for (int j = 0; j < k_NUM_MEASUREMENTS; ++j) {
        t1 = bdlt::CurrentTime::now();

        for (int k = 0; k < numSockets; ++k) {
            mX->registerSocketEvent(socket[k], EventType::e_READ, nullCb);
        }

        t2 = bdlt::CurrentTime::now();

        mX->deregisterAll();
        BSLS_ASSERT_OPT(0 == mX->numEvents());

        double microseconds = 1e6 * (t2 - t1).totalSecondsAsDouble();
        bsl::cout << microseconds << bsl::endl;
        totalMicroseconds += microseconds;
    }

    bsl::cout << bsl::endl
              << (totalMicroseconds / k_NUM_MEASUREMENTS) << bsl::endl;

    for (int j = 0; j < numSockets; j++) {
        SocketImpUtil::close(socket[j]);
        bslalg::ScalarDestructionPrimitives::destroy(&socket[j]);
    }
    testAllocator.deallocate(socket);

    return 0;
}

                        // --------------------
                        // EventManagerTestPair
                        // --------------------

// CREATORS
EventManagerTestPair::EventManagerTestPair(int verboseFlag)
: d_verboseFlag(verboseFlag)
{
#if BTESO_EVENTMANAGERTESTER_USE_RAW_SOCKETPAIR
    // We found creating 40,000 sockets in the -1 and -2 cases of
    // defaulteventmanager_*.t.cpp would crash ibm7.  Allegedly, if we create
    // them this way it will be OK, though some test drivers may complain in
    // other ways.

    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        bsl::cout << "EventManagerTestPair: Fix me: using '::socketpair'"
                  << " kluge\n";
    }

    int rc = ::socketpair(AF_UNIX, SOCK_STREAM, 0, d_fds);
#else
    int rc = SocketImpUtil::socketPair<IPv4Address>(
                                               d_fds,
                                               SocketImpUtil::k_SOCKET_STREAM);
#endif

    if (d_verboseFlag) {
        bsl::printf("T%llu: socketPair (%d, %d): %d\n",
                  bslmt::ThreadUtil::selfIdAsUint64(), d_fds[0], d_fds[1], rc);
    }

    if (0 != rc ) {
        d_validFlag = -1;
    }
    else {
        IoUtil::BlockingMode option = IoUtil::e_NONBLOCKING;
        rc |= IoUtil::setBlockingMode(d_fds[0], option);
        if (d_verboseFlag) {
            bsl::printf("T%llu: setBlockingMode (%d): %d\n",
                        bslmt::ThreadUtil::selfIdAsUint64(), d_fds[0], rc);
        }

        rc |= IoUtil::setBlockingMode(d_fds[1], option);
        if (d_verboseFlag) {
            bsl::printf("T%llu: setBlockingMode (%d): %d\n",
                        bslmt::ThreadUtil::selfIdAsUint64(), d_fds[1], rc);
        }
#if !BTESO_EVENTMANAGERTESTER_USE_RAW_SOCKETPAIR
        rc |= SocketOptUtil::setOption(d_fds[0],
                                       SocketOptUtil::k_TCPLEVEL,
                                       SocketOptUtil::k_TCPNODELAY, 1);
        if (d_verboseFlag) {
            bsl::printf("T%llu: setOption (TCPNODELAY) (%d): %d\n",
                        bslmt::ThreadUtil::selfIdAsUint64(), d_fds[0], rc);
        }

        rc |= SocketOptUtil::setOption(d_fds[1],
                                       SocketOptUtil::k_TCPLEVEL,
                                       SocketOptUtil::k_TCPNODELAY, 1);
        if (d_verboseFlag) {
            bsl::printf("T%llu: setOption (TCPNODELAY) (%d): %d\n",
                        bslmt::ThreadUtil::selfIdAsUint64(), d_fds[1], rc);
        }
#endif

        if (rc) {
            if (d_verboseFlag) {
                bsl::printf("T%llu: Closing %d\n",
                            bslmt::ThreadUtil::selfIdAsUint64(),
                            d_fds[1]);
                SocketImpUtil::close(d_fds[1]);
            }
            if (d_verboseFlag) {
                bsl::printf("T%llu: Closing %d\n",
                            bslmt::ThreadUtil::selfIdAsUint64(),
                            d_fds[0]);
                SocketImpUtil::close(d_fds[0]);
            }
            d_validFlag = -1;
        }
        else {
            d_validFlag = 0;
        }
    }
}

EventManagerTestPair::~EventManagerTestPair()
{
    if (d_verboseFlag) {
        bsl::printf("T%llu: Closing %d\n", bslmt::ThreadUtil::selfIdAsUint64(),
                    d_fds[1]);
        SocketImpUtil::close(d_fds[1]);
    }
    if (d_verboseFlag) {
        bsl::printf("T%llu: Closing %d\n", bslmt::ThreadUtil::selfIdAsUint64(),
                    d_fds[0]);
        SocketImpUtil::close(d_fds[0]);
    }
}

int
EventManagerTestPair::setObservedBufferOptions(int bufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM_OS_AIX
                                               int watermark
#else
                                               int  // watermark
#endif
                                                    )
    // Set the send and receive buffer sizes and the watermarks on the
    // observed socket handle to the specified 'bufferSize' and
    // 'watermark' values respectively.
{
    int ret = SocketOptUtil::setOption(
                                      d_fds[0],
                                      SocketOptUtil::k_SOCKETLEVEL,
                                      SocketOptUtil::k_SENDBUFFER, bufferSize);

    if (0 != ret) {
        return ret;                                                   // RETURN
    }

    ret = SocketOptUtil::setOption(d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_RECEIVEBUFFER,
                                   bufferSize);

    if (0 != ret) {
        return ret;                                                   // RETURN
    }

#ifdef BSLS_PLATFORM_OS_AIX
    ret = SocketOptUtil::setOption(d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_SNDLOWAT,
                                   watermark);
    if (0 != ret) {
        return ret;
    }

    ret = SocketOptUtil::setOption(d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_RCVLOWAT,
                                   watermark);
    if (0 != ret) {
        return ret;
    }
#endif

    return 0;
}

int
EventManagerTestPair::setControlBufferOptions(int bufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM_OS_AIX
                                              int watermark
#else
                                              int  // watermark
#endif
                                                    )
    // Set the send and receive buffer sizes and the watermarks on the observed
    // socket handle to the specified 'bufferSize' and 'watermark' values
    // respectively.
{
    int ret = SocketOptUtil::setOption(d_fds[1],
                                       SocketOptUtil::k_SOCKETLEVEL,
                                       SocketOptUtil::k_SENDBUFFER,
                                       bufferSize);

    if (0 != ret) {
        return ret;                                                   // RETURN
    }

    ret = SocketOptUtil::setOption(d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_RECEIVEBUFFER,
                                   bufferSize);
    if (0 != ret) {
        return ret;                                                   // RETURN
    }

#ifdef BSLS_PLATFORM_OS_AIX
    ret = SocketOptUtil::setOption(d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_SNDLOWAT,
                                   watermark);
    if (0 != ret) {
        return ret;
    }

    ret = SocketOptUtil::setOption(d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_RCVLOWAT,
                                   watermark);
    if (0 != ret) {
        return ret;
    }

#endif

    return 0;
}

int EventManagerTestPair::getObservedBufferOptions(int *sndBufferSize,
                                                   int *rcvBufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM_OS_AIX
                                                   int *sndLowat,
                                                   int *rcvLowat
#else
                                                   int *,  // sndLowat
                                                   int *   // rcvLowat
#endif
                                                        ) const
{
    int ret = -1;
    ret = SocketOptUtil::getOption(&sndBufferSize,
                                   d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_SENDBUFFER);
    if (0 != ret) {
        return ret;                                                   // RETURN
    }

    ret = SocketOptUtil::getOption(&rcvBufferSize,
                                   d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_RECEIVEBUFFER);
    if (0 != ret) {
        return ret;                                                   // RETURN
    }

    #ifdef BSLS_PLATFORM_OS_AIX
    ret = SocketOptUtil::getOption(&rcvLowat,
                                   d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_RCVLOWAT);

    if (0 != ret) {
        return ret;
    }

    ret = SocketOptUtil::getOption(&sndLowat,
                                   d_fds[0],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_SNDLOWAT);
    if (0 != ret) {
        return ret;
    }
    #endif

    return 0;
}

int EventManagerTestPair::getControlBufferOptions(int *sndBufferSize,
                                                  int *rcvBufferSize,
// quell gcc-4.3.2 warnings
#ifdef BSLS_PLATFORM_OS_AIX
                                                  int *sndLowat,
                                                  int *rcvLowat
#else
                                                  int *,  // sndLowat
                                                  int *   // rcvLowat
#endif
                                                       ) const
{
    int ret = -1;
    ret = SocketOptUtil::getOption(&sndBufferSize,
                                   d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_SENDBUFFER);

    if (0 != ret) {
        return ret;                                                   // RETURN
    }

    ret = SocketOptUtil::getOption(&rcvBufferSize,
                                   d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SocketOptUtil::k_RECEIVEBUFFER);

    if (0 != ret) {
        return ret;                                                   // RETURN
    }

#ifdef BSLS_PLATFORM_OS_AIX
    ret = SocketOptUtil::getOption(&rcvLowat,
                                   d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_RCVLOWAT);

    if (0 != ret) {
        return ret;
    }

    ret = SocketOptUtil::getOption(&sndLowat,
                                   d_fds[1],
                                   SocketOptUtil::k_SOCKETLEVEL,
                                   SO_SNDLOWAT);

    if (0 != ret) {
        return ret;
    }
#endif

    return 0;
}
}  // close package namespace

}  // close enterprise namespace

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
