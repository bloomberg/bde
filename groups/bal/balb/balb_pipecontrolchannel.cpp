// balb_pipecontrolchannel.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_pipecontrolchannel.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_pipecontrolchannel_cpp,"$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bdls_pipeutil.h>

#include <bslmt_threadutil.h>

#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
//  Should not be defining project-level configuration macros inside a cpp file
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
#include <windows.h>
#include <winerror.h>
#include <bdlde_charconvertutf16.h>
#else
#include <bsl_c_errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <bsl_c_signal.h>
#include <bsl_cstring.h>
#endif

namespace {

#ifdef BSLS_PLATFORM_OS_WINDOWS
bsl::string describeWin32Error(DWORD lastError)
{
    enum { ERROR_BUFFER_SIZE = 128 };
    char errorBuffer[ERROR_BUFFER_SIZE];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                   0,
                   lastError,
                   LANG_SYSTEM_DEFAULT,
                   errorBuffer,
                   ERROR_BUFFER_SIZE,
                   0);
    errorBuffer[bsl::strlen(errorBuffer) - 1] = 0;
    return bsl::string(errorBuffer);
}
#endif

}  // close unnamed namespace

namespace BloombergLP {

#ifdef BSLS_PLATFORM_OS_WINDOWS
const DWORD MAX_PIPE_BUFFER_LEN = 256; // max "send" message size

namespace balb {
                          // ------------------------
                          // class PipeControlChannel
                          // ------------------------

// PRIVATE MANIPULATORS

// ----------------------------------------------------------------------------
//                          WINDOWS IMPLEMENTATION
// ----------------------------------------------------------------------------

int PipeControlChannel::sendEmptyMessage()
{
    // The goal of this function is to do an asynchronous write to the pipe in
    // case the background thread is blocked on a synchronous 'read'.  So we
    // open our handle with 'FILE_FLAG_OVERLAPPED' to indicate asynchronous
    // I/O.  We write asynchronously because the background thread might have
    // terminated in which case it will never read the pipe, so we have to do a
    // non-blocking 'WriteFile'.

    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, d_pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return 1;                                                     // RETURN
    }

    HANDLE pipe;
    if (!WaitNamedPipeW(wPipeName.c_str(), NMPWAIT_USE_DEFAULT_WAIT)) {
        return -1;                                                    // RETURN
    }
    pipe = CreateFileW(wPipeName.c_str(), GENERIC_WRITE, 0, NULL,
                       OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (INVALID_HANDLE_VALUE == pipe) {
        return 2;                                                     // RETURN
    }

    OVERLAPPED overlapped;
    bsl::memset(&overlapped, 0, sizeof(overlapped));
    DWORD dummy;
    char buffer[] = { "\n" };
    bool rc = WriteFile(pipe, buffer, 1, &dummy, &overlapped);
    if (!rc) {
        // 'WriteFile' did not complete so the write is pending.  This means
        // that the I/O could complete at an unpredictable time in the future.
        // This would be problematic if it happened after we left this
        // function, since it would read 'buffer' and update 'overlapped',
        // neither of which would still exist, meaning memory corruption.
        //
        // So we have to cancel the pending I/O on 'pipe'.
        //
        // 'CloseHandle' below probably cancels all pending I/O from the
        // current thread on 'pipe', but MSDN never makes that clear, so let's
        // explicitly cancel it.

        (void) CancelIo(pipe);  // cancel any pending 'write' from this thread
                                // on 'pipe'.  Note that the 'write' might have
                                // completed before we got here, in which case
                                // 'CancelIo' would fail, but we don't care
                                // and we ignore the return value.
    }

    CloseHandle(pipe);

    return 0;
}

int PipeControlChannel::readNamedPipe()
{
    BSLS_ASSERT(INVALID_HANDLE_VALUE != d_impl.d_windows.d_handle);

    if (dispatchLeftoverMessage()) {
        return 0;                                                     // RETURN
    }

    BSLS_LOG_TRACE("Accepting next pipe client connection");

    if (!ConnectNamedPipe(d_impl.d_windows.d_handle, NULL)) {
        BSLS_LOG_TRACE("Connecting to named pipe '%s': %s",
                       d_pipeName.c_str(),
                       describeWin32Error(GetLastError()).c_str());

        DWORD lastError = GetLastError();
        if (lastError != ERROR_PIPE_CONNECTED && lastError != ERROR_NO_DATA) {
            BSLS_LOG_ERROR("Failed to connect to named pipe '%s'",
                           d_pipeName.c_str());
            return -1;                                                // RETURN
        }
    }

    int retval = -1;

    while (true) {
        char buffer[MAX_PIPE_BUFFER_LEN];
        DWORD bytesRead = 0;

        if (ReadFile(d_impl.d_windows.d_handle,
                     buffer,
                     MAX_PIPE_BUFFER_LEN,
                     &bytesRead,
                     NULL)) {
            // Note that if 'bytesRead' is zero, it does not indicate EOF; it
            // indicates that 'WriteFile' was called to write zero bytes to the
            // pipe.  We treat such a read no differently from any other
            // successful read, since a message is only considered complete if
            // a newline has been written.
            d_buffer.insert(d_buffer.end(), buffer, buffer + bytesRead);
            if (dispatchLeftoverMessage()) {
                // If the callback called 'shutdown' on us, that should be
                // sufficient for us to terminate, even if the client forgot
                // to close the connection; we should not wait forever for the
                // client to either close the connection or terminate (see
                // {DRQS 170252895}).  But if the callback didn't call
                // 'shutdown', then the client might have more messages to send
                // us and we should not return prematurely (as that would cause
                // the client to get a broken pipe error upon sending the next
                // message).
                if (d_backgroundState != e_RUNNING) {
                    retval = 0;
                    break;
                }
            }
            // If no newline was found, then it could mean the client sent more
            // data than we could read at once (in which case we need to keep
            // reading on the same connection in order to avoid a broken pipe
            // error on the client) or the client has already closed their end
            // without writing a newline, in which case the subsequent
            // 'ReadFile' call on *this* connection will tell *us* that the
            // pipe is broken.
        } else {
            const DWORD err = GetLastError();
            if (ERROR_BROKEN_PIPE != err) {
                BSLS_LOG_TRACE("Failed read from named pipe '%s': %s",
                               d_pipeName.c_str(),
                               describeWin32Error(err).c_str());
            } else {
                // The 'ERROR_BROKEN_PIPE' case simply means that the client
                // closed the connection but did not tell us to shut down; we
                // should therefore disconnect as well and wait for a new
                // connection to be established.  In contrast, any other error
                // indicates an abnormal condition and the negative return
                // value will result in the termination of this background
                // thread.
                retval = 0;
            }
            break;
        }
    }

    DisconnectNamedPipe(d_impl.d_windows.d_handle);

    return retval;
}

int
PipeControlChannel::createNamedPipe(const char *pipeName)
{
    BSLS_LOG_TRACE("Creating named pipe '%s'", pipeName);

    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return -3;                                                    // RETURN
    }

    d_impl.d_windows.d_handle =
        CreateNamedPipeW(wPipeName.c_str(),
                         PIPE_ACCESS_INBOUND,
                         PIPE_READMODE_BYTE | PIPE_WAIT,
                         1,    // only one server instance is permitted
                         MAX_PIPE_BUFFER_LEN,
                         MAX_PIPE_BUFFER_LEN,
                         2000, // default timeout in ms for pipe operations
                         NULL);

    if (INVALID_HANDLE_VALUE == d_impl.d_windows.d_handle) {
        if (ERROR_PIPE_BUSY == GetLastError()) {
            BSLS_LOG_ERROR("Named pipe '%s' already exists", pipeName);

            return -2;                                                // RETURN
        }
        BSLS_LOG_TRACE("Failed to create named pipe '%s': %s",
                       d_pipeName.c_str(),
                       describeWin32Error(GetLastError()).c_str());
        return -1;                                                    // RETURN
    }

    return 0;
}

void PipeControlChannel::destroyNamedPipe()
{
    CloseHandle(d_impl.d_windows.d_handle);
    d_impl.d_windows.d_handle = INVALID_HANDLE_VALUE;

    BSLS_LOG_TRACE("Closed named pipe '%s'", d_pipeName.c_str());
}

}  // close package namespace

#else

namespace balb {
// ----------------------------------------------------------------------------
//                            UNIX IMPLEMENTATION
// ----------------------------------------------------------------------------

void PipeControlChannel::destroyNamedPipe()
{
    close(d_impl.d_unix.d_writeFd);
    close(d_impl.d_unix.d_readFd);

    d_impl.d_unix.d_writeFd = -1;
    d_impl.d_unix.d_readFd  = -1;

    unlink(d_pipeName.c_str());

    BSLS_LOG_TRACE("Destroyed pipe '%s'", d_pipeName.c_str());
}

int PipeControlChannel::readNamedPipe()
{
    if (dispatchLeftoverMessage()) {
        return 0;                                                     // RETURN
    }

    if (d_impl.d_unix.d_readFd == -1) {
        // Pipe was closed before read was issued.
        return 0;                                                     // RETURN
    }

    pollfd fds;
    bsl::memset(&fds, 0, sizeof(fds));
    fds.fd      = d_impl.d_unix.d_readFd;
    fds.events  = POLLIN | POLLHUP;

    enum {BUFFER_SIZE = 256};
    char buffer[BUFFER_SIZE];

    while (true) {
        // Read incoming data on the named pipe until a complete message has
        // been assembled.

        int rc         = poll(&fds, 1, -1);

        int savedErrno = errno;

        if (0 >= rc) {
            if (EINTR == savedErrno) {
                BSLS_LOG_DEBUG("EINTR polling pipe '%s'", d_pipeName.c_str());
                continue;
            }
            BSLS_LOG_ERROR("Failed to poll pipe '%s', rc = %d, errno = %d: %s",
                           d_pipeName.c_str(), rc, savedErrno,
                           bsl::strerror(savedErrno));
            return -1;                                                // RETURN
        }

        if ((fds.revents & POLLERR) || (fds.revents & POLLNVAL)) {
            BSLS_LOG_ERROR("Polled POLLERROR or POLLINVAL from file "
                           "descriptor of pipe '%s', errno = %d: %s",
                           d_pipeName.c_str(), savedErrno,
                           bsl::strerror(savedErrno));
            return -1;                                                // RETURN
        }

        if (fds.revents & POLLIN) {
            BSLS_LOG_TRACE("Polled POLLIN from file descriptor of pipe '%s'",
                           d_pipeName.c_str());

            bsls::Types::Int64 bytesRead = read(d_impl.d_unix.d_readFd,
                                                buffer,
                                                BUFFER_SIZE);

            savedErrno    = errno;

            if (0 == bytesRead) {
                BSLS_LOG_TRACE("Zero bytes read from the pipe");
                continue;
            }
            else if (0 > bytesRead) {
                // On some older systems EAGAIN and EWOULDBLOCK have different
                // values.  However, if they have the same value, then testing
                // against both results in an annoying compiler warning.
#if EAGAIN != EWOULDBLOCK
                if (EAGAIN == savedErrno || EWOULDBLOCK == savedErrno) {
#else
                if (EAGAIN == savedErrno) {
#endif
                    BSLS_LOG_ERROR(
                        "Failed to read from pipe '%s', errno = %d: '%s'."
                        " This indicates that another process is reading from "
                        "'%s'."
                        " Messages written to this pipe will be unreliable"
                        " until the conflict is resolved",
                        d_pipeName.c_str(), savedErrno,
                        bsl::strerror(savedErrno), d_pipeName.c_str());
                    continue;
                } else {
                    BSLS_LOG_ERROR(
                               "Failed to read from pipe '%s', errno = %d: %s",
                               d_pipeName.c_str(), savedErrno,
                               bsl::strerror(savedErrno));
                    return -1;                                        // RETURN
                }
            }
            else {
                if (bsls::Log::severityThreshold() >=
                    bsls::LogSeverity::e_TRACE) {
                    bsl::string readBytes(buffer, buffer + bytesRead);
                    BSLS_LOG_TRACE("Read data from pipe: '%s'",
                                   readBytes.c_str());
                }
                d_buffer.insert(d_buffer.end(), buffer, buffer + bytesRead);
                if (dispatchLeftoverMessage()) {
                    return 0;                                         // RETURN
                }
            }
        }
    }

    BSLS_LOG_FATAL("unreachable code in 'readNamedPipe'");

    return -1;    // error
}

int
PipeControlChannel::sendEmptyMessage()
{
    const int flags = fcntl(d_impl.d_unix.d_writeFd, F_GETFL);
    if (-1 == flags) {
        const int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to get 'fcntl' flags on '%s' for"
                                                   " writing. errno = %d (%s)",
                       d_pipeName.c_str(),
                       savedErrno,
                       bsl::strerror(savedErrno));

        return 1;                                                     // RETURN
    }

    if (0 == (flags & O_NONBLOCK)) {
        if (-1 == fcntl(d_impl.d_unix.d_writeFd,
                        F_SETFL,
                        flags | O_NONBLOCK)) {
            const int savedErrno = errno;
            BSLS_LOG_ERROR("Unable to set 'O_NONBLOCK' on '%s' for"
                                                   " writing. errno = %d (%s)",
                           d_pipeName.c_str(),
                           savedErrno,
                           bsl::strerror(savedErrno));

            return 2;                                                 // RETURN
        }
    }

    // Ignore return value of 'write' -- we don't care whether it succeeds or
    // not.  'shutdown', the calling function, will keep calling us until the
    // background thread changes 'd_backgroundState'.  This function only
    // returns an error status if the state of 'd_impl.d_unix.d_writeFd' is
    // such that we would be unable to write.

    (void) write(d_impl.d_unix.d_writeFd, "\n", 1);

    return 0;    // success
}

int
PipeControlChannel::createNamedPipe(const char *pipeName)
{
//  BSLS_ASSERT(0 == d_impl.d_unix.d_readFd);

    if (-1 != d_impl.d_unix.d_readFd) {
        return -7;                                                    // RETURN
    }

    if (bdls::FilesystemUtil::exists(pipeName)) {
       // The pipe already exists, but it may have been left over from a
       // previous crash.  Check whether there is a reader on the pipe, in
       // which case fail; otherwise unlink the pipe and continue.
       if (bdls::PipeUtil::isOpenForReading(pipeName)) {
           BSLS_LOG_ERROR("Named pipe '%s' is already in use by another "
                          "process", pipeName);

           return -2;                                                 // RETURN
       }
       bdls::FilesystemUtil::remove(pipeName);
    }

    bsl::string dirname;
    if (0 == bdls::PathUtil::getDirname(&dirname, pipeName)) {
        if (!bdls::FilesystemUtil::exists(dirname)) {
            BSLS_LOG_ERROR("Named pipe directory '%s' does not exist",
                           dirname.c_str());
            return -3;                                                // RETURN
        }
    }

    int rc = mkfifo(pipeName, 0666);
    if (0 != rc) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to create pipe '%s'. errno = %d (%s)",
                       pipeName, savedErrno, bsl::strerror(savedErrno));
        return -4;                                                    // RETURN
    }

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
    d_impl.d_unix.d_readFd = open(pipeName,
                                  O_RDONLY | O_NONBLOCK | O_CLOEXEC);
#else
    d_impl.d_unix.d_readFd = open(pipeName, O_RDONLY | O_NONBLOCK);
#endif

    if (-1 == d_impl.d_unix.d_readFd) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to open pipe '%s' for reading. errno = %d (%s)",
                       pipeName, savedErrno, bsl::strerror(savedErrno));

        return -5;                                                    // RETURN
    }

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
    d_impl.d_unix.d_writeFd = open(pipeName, O_WRONLY | O_CLOEXEC);
#else
    d_impl.d_unix.d_writeFd = open(pipeName, O_WRONLY);
#endif

    if (-1 == d_impl.d_unix.d_writeFd) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to open pipe '%s' for writing. errno = %d (%s)",
                       pipeName, savedErrno, bsl::strerror(savedErrno));
        return -6;                                                    // RETURN
    }

#if !defined(BSLS_PLATFORM_OS_LINUX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
    if (-1 == fcntl(d_impl.d_unix.d_readFd, F_SETFD, FD_CLOEXEC)) {
        int savedErrno = errno;
        BSLS_LOG_ERROR(
             "Unable to set 'FD_CLOEXEC' on '%s' for reading. errno = %d (%s)",
             pipeName,
             savedErrno,
             bsl::strerror(savedErrno));
        return -8;                                                    // RETURN
    }

    if (-1 == fcntl(d_impl.d_unix.d_writeFd, F_SETFD, FD_CLOEXEC)) {
        int savedErrno = errno;
        BSLS_LOG_ERROR(
             "Unable to set 'FD_CLOEXEC' on '%s' for writing. errno = %d (%s)",
             pipeName,
             savedErrno,
             bsl::strerror(savedErrno));
        return -9;                                                    // RETURN
    }
#endif

    BSLS_LOG_TRACE("Created named pipe '%s'", pipeName);

    return 0;
}

}  // close package namespace

#endif // END PLATFORM-SPECIFIC FUNCTION IMPLEMENTATIONS

namespace balb {

// CREATORS
PipeControlChannel::PipeControlChannel(const ControlCallback&  callback,
                                       bslma::Allocator       *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlCallback>(basicAllocator),
             callback)
, d_pipeName(bslma::Default::allocator(basicAllocator))
, d_buffer(bslma::Default::allocator(basicAllocator))
, d_thread(bslmt::ThreadUtil::invalidHandle())
, d_backgroundState(e_STOPPED)
, d_isPipeOpen(false)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    d_impl.d_windows.d_handle = INVALID_HANDLE_VALUE;
#else
    d_impl.d_unix.d_readFd    = -1;
    d_impl.d_unix.d_writeFd   = -1;
#endif
}

PipeControlChannel::~PipeControlChannel()
{
    shutdown();
    stop();
}

// MANIPULATORS
void PipeControlChannel::backgroundProcessor()
{
    while (d_backgroundState == e_RUNNING) {
        if (0 != readNamedPipe()) {
            BSLS_LOG_ERROR(
               "Error processing message: unable to read from named pipe '%s'",
               d_pipeName.c_str());

            break;
        }
    }

    d_backgroundState = e_STOPPED;

    BSLS_LOG_TRACE("The background thread has stopped");
}

int PipeControlChannel::start(const char *pipeName)
{
    return start(pipeName, bslmt::ThreadAttributes());
}

int PipeControlChannel::start(const char                     *pipeName,
                              const bslmt::ThreadAttributes&  attributes)
{
// BSLS_ASSERT(!d_isRunningFlag); // TBD: DOES createNamedPipe FAIL???
    if (d_backgroundState == e_RUNNING) {
        return 2;                                                     // RETURN
    }
#ifdef BSLS_PLATFORM_OS_WINDOWS
    // See test driver case 9, on windows, createNamedPipe does not
    // fail if the named pipe is already open!!!!!
    // TBD

    if (d_isPipeOpen) {
        return 3;                                                     // RETURN
    }
#endif

    if (0 != createNamedPipe(pipeName)) {
        BSLS_LOG_ERROR("Unable to create named pipe '%s'", pipeName);
        return 1;                                                     // RETURN
    }

    d_pipeName        = pipeName;
    d_backgroundState = e_RUNNING;
    d_isPipeOpen      = true;

    int rc = bslmt::ThreadUtil::create(
         &d_thread,
         attributes,
         bdlf::BindUtil::bind(&PipeControlChannel::backgroundProcessor, this));
    if (rc != 0) {
        BSLS_LOG_ERROR("Cannot create processing thread, rc = %d", rc);
        d_backgroundState = e_STOPPED;
        return rc;                                                    // RETURN
    }

    return 0;
}

void PipeControlChannel::shutdown()
{
    if (e_RUNNING != d_backgroundState.testAndSwap(e_RUNNING, e_STOPPING)) {
        return;                                                       // RETURN
    }

    if (bslmt::ThreadUtil::self() == d_thread) {
        // When 'shutdown' is called from the same thread as the background
        // thread perform a synchronous shutdown.

        return;                                                       // RETURN
    }

    // Note that 'microSleep' usually won't do anything unless requested to
    // sleep for at least 10,000 microseconds, but also, if requested to sleep
    // for a short time, will often sleep for 2 seconds (particularly on
    // Solaris).  For this reason, we avoid doing any 'microSleep's until we've
    // been through the loop many times, doing a 'yield' each time.

    enum { k_MIN_MICRO_SLEEP_TIME =  10 * 1024,
           k_MAX_MICRO_SLEEP_TIME = 250 * 1000 };

    int microSleepTime = k_MIN_MICRO_SLEEP_TIME / 256;   // don't sleep until
                                                         // 8th pass
    while (d_backgroundState != e_STOPPED) {
        // The background thread will set 'd_backgroundState' to 'e_STOPPED'
        // when it exits.  In the meantime, it might or might not be blocked
        // reading from the pipe.  So we iterate doing non-blocking writes,
        // writing empty commands to the pipe.  Once the background thread
        // completes a 'read', it will see that 'd_backgroundState' is no
        // longer 'e_RUNNING', set 'd_backgroundState' to 'e_STOPPED', and
        // exit.

        if (sendEmptyMessage() > 0) {  // Fatal errors
            break;                                                     // BREAK
        }

        // Sleep for increasing times up to a quarter second.

        microSleepTime = bsl::min<int>(k_MAX_MICRO_SLEEP_TIME,
                                       2 * microSleepTime);

        if (k_MIN_MICRO_SLEEP_TIME <= microSleepTime) {
            bslmt::ThreadUtil::microSleep(microSleepTime);
        }
        else {
            bslmt::ThreadUtil::yield();
        }
    }
}

void PipeControlChannel::dispatchMessageUpTo(
                                       const bsl::vector<char>::iterator& iter)
{
    bslstl::StringRef stringRef(&(*d_buffer.begin()),
                                static_cast<int>(iter - d_buffer.begin()));
    BSLS_LOG_TRACE("Assembled complete message '%s'",
                   ((bsl::string)stringRef).c_str());

    if (!stringRef.isEmpty()) {
        d_callback(stringRef);
    }
    d_buffer.erase(d_buffer.begin(), iter+1);
}

bool PipeControlChannel::dispatchLeftoverMessage()
{
    if (d_buffer.size() != 0) {
        const bsl::vector<char>::iterator it = bsl::find(d_buffer.begin(),
                                                         d_buffer.end(),
                                                         '\n');
        if (it != d_buffer.end()) {
            dispatchMessageUpTo(it);
            return true;                                              // RETURN
        }
    }
    return false;
}

void PipeControlChannel::stop()
{
    if (bslmt::ThreadUtil::invalidHandle() != d_thread) {
        bslmt::ThreadUtil::join(d_thread);
        d_thread = bslmt::ThreadUtil::invalidHandle();
    }

    d_backgroundState = e_STOPPED;

    if (d_isPipeOpen) {
        destroyNamedPipe();
        d_pipeName.clear();
        d_isPipeOpen = false;
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
