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

    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, d_pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return 1;                                                     // RETURN
    }

    HANDLE pipe;
    if (!WaitNamedPipeW(wPipeName.c_str(), NMPWAIT_USE_DEFAULT_WAIT)) {
        return -1;
    }
    pipe = CreateFileW(wPipeName.c_str(), GENERIC_WRITE, 0, NULL,
                       OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == pipe) {
        return 2;
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(pipe, &mode, NULL, NULL);

    DWORD dummy;
    bool result = WriteFile(pipe, "\n", 1, &dummy, 0);
    CloseHandle(pipe);
    return result ? 0 : 3;

}

int PipeControlChannel::readNamedPipe()
{
    BSLS_ASSERT(INVALID_HANDLE_VALUE != d_impl.d_windows.d_handle);

    BSLS_LOG_TRACE("Accepting next pipe client connection");

    if (!ConnectNamedPipe(d_impl.d_windows.d_handle, NULL)) {
        BSLS_LOG_TRACE("Connecting to named pipe '%s': %s",
                       d_pipeName.c_str(),
                       describeWin32Error(GetLastError()).c_str());

        DWORD lastError = GetLastError();
        if (lastError != ERROR_PIPE_CONNECTED && lastError != ERROR_NO_DATA) {
            BSLS_LOG_TRACE("Failed to connect to named pipe '%s'",
                           d_pipeName.c_str());
            return -1;
        }
    }

    while(1) {
        char buffer[MAX_PIPE_BUFFER_LEN];
        DWORD bytesRead = 0;

        if (ReadFile(d_impl.d_windows.d_handle,
                     buffer,
                     MAX_PIPE_BUFFER_LEN,
                     &bytesRead,
                     NULL))
        {
           if (bytesRead > 0) {
               if (buffer[bytesRead - 1] == '\n') {
                   bytesRead--;
               }
               bslstl::StringRef stringRef(buffer, bytesRead);
               if (!stringRef.isEmpty()) {
                   d_callback(stringRef);
               }
           }
           else {
              // reached EOF on a named pipe.
              break;
           }
        }
        else {
            BSLS_LOG_TRACE("Failed read from named pipe '%s': %s",
                           d_pipeName.c_str(),
                           describeWin32Error(GetLastError()).c_str());
            break;
        }
    }

    DisconnectNamedPipe(d_impl.d_windows.d_handle);

    return 0;
}

int
PipeControlChannel::createNamedPipe(const bsl::string& pipeName)
{
    BSLS_LOG_TRACE("Creating named pipe '%s'", pipeName.c_str());

    if (bdls::PipeUtil::isOpenForReading(pipeName)) {
        BSLS_LOG_ERROR("Named pipe '%s' already exists",
                       pipeName.c_str());
        return -2;
    }

    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return -3;                                                    // RETURN
    }

    d_impl.d_windows.d_handle =
        CreateNamedPipeW(wPipeName.c_str(),
                         PIPE_ACCESS_INBOUND,
                         PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                         PIPE_UNLIMITED_INSTANCES,
                         MAX_PIPE_BUFFER_LEN,
                         MAX_PIPE_BUFFER_LEN,
                         2000, // default timeout in ms for pipe operations
                         NULL);

    if (INVALID_HANDLE_VALUE == d_impl.d_windows.d_handle) {
        BSLS_LOG_TRACE("Failed to create named pipe '%s': %s",
                       d_pipeName.c_str(),
                       describeWin32Error(GetLastError()).c_str());
        return -1;
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

    d_impl.d_unix.d_writeFd = 0;
    d_impl.d_unix.d_readFd = 0;

    unlink(d_pipeName.c_str());

    BSLS_LOG_TRACE("Destroyed pipe '%s'", d_pipeName.c_str());
}

int PipeControlChannel::readNamedPipe()
{
    // Ideally, we would like to simply set the ioctl option I_SRDOPT to RMSGN
    // when we open the named pipe.  This changes the read mode of the pipe
    // from byte stream mode to message mode.  However, some versions of glibc
    // on Linux have defined I_SRDOPT such that it conflicts with other device
    // driver ioctl flags, namely CDROMREADTOCENTRY.  This results in a failed
    // call to ioctl with errno set to EINVAL.
    //
    // Since we cannot reliably set the named pipe to message mode on all *nix
    // platforms, we leave the pipe in byte stream mode and emulate RMSGN in
    // the implementation.

    // A non-discarded message, or portion of a message, is left from the
    // previous invocation of this function.
    if (d_buffer.size() != 0) {
        bsl::vector<char>::iterator it = bsl::find(d_buffer.begin(),
                                                   d_buffer.end(),
                                                   '\n');
        if (it != d_buffer.end()) {
            dispatchMessageUpTo(it);
            return 0;                                                 // RETURN
        }
    }

    if (d_impl.d_unix.d_readFd == 0) {
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
            BSLS_LOG_TRACE("Polled POLLERROR or POLLINVAL from file "
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
                BSLS_LOG_ERROR("Failed to read from pipe '%s', errno = %d: %s",
                               d_pipeName.c_str(), savedErrno,
                               bsl::strerror(savedErrno));
                return -1;                                            // RETURN
            }
            else {
                if (bsls::Log::severityThreshold() >=
                    bsls::LogSeverity::e_TRACE) {
                    bsl::string readBytes(buffer, buffer + bytesRead);
                    BSLS_LOG_TRACE("Read data from pipe: '%s'",
                                   readBytes.c_str());
                }
                d_buffer.insert(d_buffer.end(), buffer, buffer + bytesRead);
                bsl::vector<char>::iterator it =
                    bsl::find(d_buffer.begin(), d_buffer.end(), '\n');

                if (it != d_buffer.end()) {
                    dispatchMessageUpTo(it);
                    return 0;                                         // RETURN
               }
            }
        }
    }

    BSLS_ASSERT(!"unreachable");
}

int
PipeControlChannel::sendEmptyMessage()
{
    write(d_impl.d_unix.d_writeFd, "\n", 1);
    return 0;
}

int
PipeControlChannel::createNamedPipe(const bsl::string& pipeName)
{
//  BSLS_ASSERT(0 == d_impl.d_unix.d_readFd);

    if (0 != d_impl.d_unix.d_readFd) {
        return -7;                                                    // RETURN
    }

    if (bdls::FilesystemUtil::exists(pipeName)) {
       // The pipe already exists, but it may have been left over from a
       // previous crash.  Check whether there is a reader on the pipe, in
       // which case fail; otherwise unlink the pipe and continue.
       if (bdls::PipeUtil::isOpenForReading(pipeName)) {
           BSLS_LOG_ERROR("Named pipe '%s' is already in use by another "
                          "process", pipeName.c_str());

           return -2;                                                 // RETURN
       }
       bdls::FilesystemUtil::remove(pipeName.c_str());
    }

    // TBD: USE BDEU_PATHUTIL
    bsl::string dirname;
    if (0 == bdls::PathUtil::getDirname(&dirname, pipeName)) {
        if (!bdls::FilesystemUtil::exists(dirname)) {
            BSLS_LOG_ERROR("Named pipe directory '%s' does not exist",
                           dirname.c_str());
            return -3;                                                // RETURN
        }
    }

    const char *rawPipeName = pipeName.c_str();

    int rc         = mkfifo(rawPipeName, 0666);

    if (0 != rc) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to create pipe '%s'. errno = %d (%s)",
                       rawPipeName, savedErrno, bsl::strerror(savedErrno));
        return -4;                                                    // RETURN
    }

    d_impl.d_unix.d_readFd = open(rawPipeName, O_RDONLY | O_NONBLOCK);

    if (-1 == d_impl.d_unix.d_readFd) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to open pipe '%s' for reading. errno = %d (%s)",
                       rawPipeName, savedErrno, bsl::strerror(savedErrno));

        return -5;                                                    // RETURN
    }

    d_impl.d_unix.d_writeFd = open(rawPipeName, O_WRONLY);
    if (-1 == d_impl.d_unix.d_writeFd) {
        int savedErrno = errno;
        BSLS_LOG_ERROR("Unable to open pipe '%s' for writing. errno = %d (%s)",
                       rawPipeName, savedErrno, bsl::strerror(savedErrno));
        return -6;                                                    // RETURN
    }
    BSLS_LOG_TRACE("Created named pipe '%s'", rawPipeName);

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
    d_impl.d_unix.d_readFd    = 0;
    d_impl.d_unix.d_writeFd   = 0;
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
            BSLS_LOG_WARN("Error processing M-trap: unable to read from named"
                          " pipe '%s'", d_pipeName.c_str());
            return;                                                   // RETURN
        }
    }

    d_backgroundState = e_STOPPED;
    BSLS_LOG_TRACE("The background thread has stopped");
}

int PipeControlChannel::start(const bsl::string& pipeName)
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
        return 3;
    }
#endif

    if (0 != createNamedPipe(pipeName)) {
        BSLS_LOG_ERROR("Unable to create named pipe '%s'", pipeName.c_str());
        return 1;                                                    // RETURN
    }

    d_pipeName        = pipeName;
    d_backgroundState = e_RUNNING;
    d_isPipeOpen      = true;

    int rc = bslmt::ThreadUtil::create(
         &d_thread,
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
    if (d_backgroundState != e_RUNNING) {
        return;                                                       // RETURN
    }

    d_backgroundState = e_STOPPING;

    if (bslmt::ThreadUtil::self() == d_thread) {
        // When 'shutdown' is called from the same thread as the background
        // thread perform a synchronous shutdown.
        return;                                                       // RETURN
    }

    // The background thread is blocked on a call to 'ReadFile'.  Shutdown the
    // background thread by a) indicating that the thread should perform no
    // more reads and b) unblocking the background thread by sending an "empty"
    // message to the named pipe.

    while (d_backgroundState != e_STOPPED) {
        if (sendEmptyMessage() > 0) {  // Fatal errors
            break;                                                     // BREAK
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
