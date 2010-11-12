// baea_pipecontrolchannel.cpp                                        -*-C++-*-
#include <baea_pipecontrolchannel.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_pipecontrolchannel_cpp,"$Id$ $CSID$")

#include <bael_log.h>

#include <bdef_bind.h>
#include <bslma_default.h>
#include <bdesu_fileutil.h>
#include <bdesu_pathutil.h>
#include <bdesu_pipeutil.h>

#include <bsl_cstdlib.h>
#include <bsl_algorithm.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>

#include <bsls_assert.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
//  Should not be defining project-level configuration macros inside a cpp file
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
#include <windows.h>
#include <winerror.h>
#else
#include <bsl_c_errno.h>
#include <fcntl.h>
#include <poll.h>
#ifndef BDES_PLATFORM__OS_FREEBSD
#include <stropts.h>  // not sure what this is necessary on UNIX
#endif
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <bsl_c_signal.h>
#include <bsl_cstring.h>
#endif

namespace {

const char LOG_CATEGORY[] = "BAEA.PIPECONTROLCHANNEL";

#ifdef BSLS_PLATFORM__OS_WINDOWS
bsl::string describeWin32Error(DWORD lastError)
{
    enum {ERROR_BUFFER_SIZE=128};
    char errorBuffer[ERROR_BUFFER_SIZE];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0,
        lastError, LANG_SYSTEM_DEFAULT,
        errorBuffer, ERROR_BUFFER_SIZE, 0);
    errorBuffer[bsl::strlen(errorBuffer) - 1] = 0;
    return bsl::string(errorBuffer);
}
#endif

}  // close unnamed namespace

namespace BloombergLP {

#ifdef BSLS_PLATFORM__OS_WINDOWS
const DWORD MAX_PIPE_BUFFER_LEN = 256; // max "send" message size

                       // -----------------------------
                       // class baea_PipeControlChannel
                       // -----------------------------

// PRIVATE MANIPULATORS

// ----------------------------------------------------------------------------
//                            WINDOWS IMPLEMENTATION
// ----------------------------------------------------------------------------

int baea_PipeControlChannel::sendEmptyMessage()
{
    return bdesu_PipeUtil::send(d_pipeName, "\n");
}

int baea_PipeControlChannel::readNamedPipe()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BSLS_ASSERT(INVALID_HANDLE_VALUE != d_impl.d_windows.d_handle);

    BAEL_LOG_TRACE << "Accepting next pipe client connection" << BAEL_LOG_END;

    if (!ConnectNamedPipe(d_impl.d_windows.d_handle, NULL)) {
        BAEL_LOG_TRACE << "Connecting to named pipe '" << d_pipeName
                       << "': "
                       << describeWin32Error(GetLastError())
                       << BAEL_LOG_END;

        DWORD lastError = GetLastError();
        if (lastError != ERROR_PIPE_CONNECTED
         && lastError != ERROR_NO_DATA)
        {
            BAEL_LOG_TRACE << "Failed to connect to named pipe '" << d_pipeName
                           << "'"
                           << BAEL_LOG_END;
            return -1;
        }
    }

    while(1) {
        char buffer[MAX_PIPE_BUFFER_LEN];
        DWORD bytesRead = 0;

        if (ReadFile(d_impl.d_windows.d_handle,
                     buffer,
                     MAX_PIPE_BUFFER_LEN, &bytesRead, NULL))
        {
           if (bytesRead > 0) {
               if (buffer[bytesRead - 1] == '\n') {
                   bytesRead--;
               }
               bdeut_StringRef stringRef(buffer, bytesRead);
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
            BAEL_LOG_TRACE << "Failed to read from named pipe '" << d_pipeName
                           << "': "
                           << describeWin32Error(GetLastError())
                           << BAEL_LOG_END;
            break;
        }
    }

    DisconnectNamedPipe(d_impl.d_windows.d_handle);

    return 0;
}

int
baea_PipeControlChannel::createNamedPipe(const bsl::string& pipeName)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    BAEL_LOG_TRACE << "Creating named pipe '" << pipeName << "'"
                   << BAEL_LOG_END;

    if (bdesu_PipeUtil::isOpenForReading(pipeName)) {
        BAEL_LOG_ERROR << "Named pipe '" << pipeName << "' already exists"
                       << BAEL_LOG_END;
        return -2;
    }

    d_impl.d_windows.d_handle =
        CreateNamedPipe(pipeName.c_str(),
                        PIPE_ACCESS_INBOUND,
                        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                        PIPE_UNLIMITED_INSTANCES,
                        MAX_PIPE_BUFFER_LEN,
                        MAX_PIPE_BUFFER_LEN,
                        2000, // default timeout in ms for pipe operations
                        NULL);

    if (INVALID_HANDLE_VALUE == d_impl.d_windows.d_handle) {
        BAEL_LOG_TRACE << "Failed to create named pipe '" << pipeName
                       << "': "
                       << describeWin32Error(GetLastError())
                       << BAEL_LOG_END;
        return -1;
    }

    return 0;
}

void baea_PipeControlChannel::destroyNamedPipe()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    CloseHandle(d_impl.d_windows.d_handle);
    d_impl.d_windows.d_handle = INVALID_HANDLE_VALUE;

    BAEL_LOG_TRACE << "Closed named pipe '" << d_pipeName << "'"
                   << BAEL_LOG_END;
}

#else

// ----------------------------------------------------------------------------
//                              UNIX IMPLEMENTATION
// ----------------------------------------------------------------------------

void baea_PipeControlChannel::destroyNamedPipe()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    close(d_impl.d_unix.d_writeFd);
    close(d_impl.d_unix.d_readFd);

    d_impl.d_unix.d_writeFd = 0;
    d_impl.d_unix.d_readFd = 0;

    unlink(d_pipeName.c_str());

    BAEL_LOG_TRACE << "Destroyed pipe '" << d_pipeName << "'"
                   << BAEL_LOG_END;
}

int baea_PipeControlChannel::readNamedPipe()
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
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    // A non-discarded message, or portion of a message, is left from the
    // previous invocation of this function.
    if (d_buffer.size() != 0) {
        bsl::vector<char>::iterator it = bsl::find(d_buffer.begin(),
                                                   d_buffer.end(),
                                                   '\n');
        if (it != d_buffer.end()) {
            dispatchMessageUpTo(it);
            return 0;
        }
    }

    if (d_impl.d_unix.d_readFd == 0) {
        // Pipe was closed before read was issued.
        return 0;
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

        int rc = poll(&fds, 1, -1);

        if (0 >= rc) {
            if (EINTR == errno) {
                BAEL_LOG_DEBUG << "EINTR polling pipe '"
                               << d_pipeName << "'" << BAEL_LOG_END;
                continue;
            }
            BAEL_LOG_ERROR << "Failed to poll pipe '"
                           << d_pipeName << "', errno = " << errno << ": "
                           << bsl::strerror(errno) << BAEL_LOG_END;
            return -1;
        }

        if ((fds.revents & POLLERR) || (fds.revents & POLLNVAL)) {
            BAEL_LOG_TRACE << "Polled POLLERROR or POLLINVAL from file "
                              "descriptor of pipe '"
                           << d_pipeName << "', errno = " << errno << ": "
                           << bsl::strerror(errno) << BAEL_LOG_END;
            return -1;
        }

        if (fds.revents & POLLIN) {
            BAEL_LOG_TRACE << "Polled POLLIN from file descriptor of pipe '"
                           << d_pipeName << "'" << BAEL_LOG_END;

            int bytesRead = read(d_impl.d_unix.d_readFd, buffer, BUFFER_SIZE);

            if (0 == bytesRead) {
                BAEL_LOG_TRACE << "Zero bytes read from the pipe"
                               << BAEL_LOG_END;
                continue;
            }
            else if (0 > bytesRead) {
                BAEL_LOG_ERROR << "Failed to read from pipe '"
                               << d_pipeName << "', errno = " << errno << ": "
                               << bsl::strerror(errno) << BAEL_LOG_END;
                return -1;
            }
            else {
               BAEL_LOG_TRACE << "Read data from pipe: '";
               bsl::copy(buffer,
                         buffer + bytesRead,
                         bsl::ostream_iterator<char>(BAEL_STREAM));
               BAEL_STREAM << "'" << BAEL_LOG_END;

               d_buffer.insert(d_buffer.end(), buffer, buffer + bytesRead);
               bsl::vector<char>::iterator it = bsl::find(d_buffer.begin(),
                                                          d_buffer.end(),
                                                          '\n');

               if (it != d_buffer.end()) {
                   dispatchMessageUpTo(it);
                   return 0;
               }
            }
        }
    }

    BSLS_ASSERT(!"unreachable");
}

int
baea_PipeControlChannel::sendEmptyMessage()
{
    write(d_impl.d_unix.d_writeFd, "\n", 1);
    return 0;
}

int
baea_PipeControlChannel::createNamedPipe(const bsl::string& pipeName)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
//  BSLS_ASSERT(0 == d_impl.d_unix.d_readFd);

    if (0 != d_impl.d_unix.d_readFd) {
        return -7;
    }

    if (bdesu_FileUtil::exists(pipeName)) {
       // The pipe already exists, but it may have been left over from a
       // previous crash.  Check whether there is a reader on the pipe, in
       // which case fail; otherwise unlink the pipe and continue.
       if (bdesu_PipeUtil::isOpenForReading(pipeName)) {
           BAEL_LOG_ERROR << "Named pipe "
                             "'" << pipeName << "' "
                             "is already in use by another process"
                          << BAEL_LOG_END;
           return -2;
       }
       bdesu_FileUtil::remove(pipeName.c_str());
    }

    // TBD: USE BDEU_PATHUTIL
    bsl::string dirname;
    if (0 == bdesu_PathUtil::getDirname(&dirname, pipeName)) {
        if (!bdesu_FileUtil::exists(dirname)) {
            BAEL_LOG_ERROR << "Named pipe directory "
                              "'" << dirname << "' "
                              "does not exist"
                           << BAEL_LOG_END;
            return -3;
        }
    }

    const char *rawPipeName = pipeName.c_str();

    int rc = mkfifo(rawPipeName, 0666);

    if (0 != rc) {
        BAEL_LOG_ERROR << "Unable to create pipe "
                          "'"          << rawPipeName          << "'"
                          ": errno = " << errno                << " "
                          "("          << bsl::strerror(errno) << ")"
                       << BAEL_LOG_END;
        return -4;
    }

    d_impl.d_unix.d_readFd = open(rawPipeName, O_RDONLY | O_NONBLOCK);
    if (-1 == d_impl.d_unix.d_readFd) {
        BAEL_LOG_ERROR << "Cannot open pipe "
                          "'"           << rawPipeName          << "' "
                          "for reading"
                          ": errno = "  << errno                << " "
                          "("           << bsl::strerror(errno) << ")"
                       << BAEL_LOG_END;
        return -5;
    }

    d_impl.d_unix.d_writeFd = open(rawPipeName, O_WRONLY);
    if (-1 == d_impl.d_unix.d_writeFd) {
        BAEL_LOG_ERROR << "Failed to open named pipe "
                          "'"           << rawPipeName          << "' "
                          "for writing"
                          ": errno = "  << errno                << " "
                          "("           << bsl::strerror(errno) << ")"
                       << BAEL_LOG_END;
        return -6;
    }

    BAEL_LOG_TRACE << "Created named pipe '" << rawPipeName << "'"
                   << BAEL_LOG_END;

    return 0;
}

#endif // END PLATFORM-SPECIFIC FUNCTION IMPLEMENTATIONS

// CREATORS

baea_PipeControlChannel::baea_PipeControlChannel(
        const ControlCallback&  callback,
        bslma_Allocator        *basicAllocator)
: d_callback(callback, bslma_Default::allocator(basicAllocator))
, d_pipeName(bslma_Default::allocator(basicAllocator))
, d_buffer(bslma_Default::allocator(basicAllocator))
, d_thread(bcemt_ThreadUtil::invalidHandle())
, d_isRunningFlag(false)
, d_isPipeOpen(false)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_impl.d_windows.d_handle = INVALID_HANDLE_VALUE;
#else
    d_impl.d_unix.d_readFd    = 0;
    d_impl.d_unix.d_writeFd   = 0;
#endif
}

baea_PipeControlChannel::~baea_PipeControlChannel()
{
    shutdown();
    stop();
}

// MANIPULATORS

void baea_PipeControlChannel::backgroundProcessor()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    while (d_isRunningFlag) {
        if (0 != readNamedPipe()) {
            BAEL_LOG_WARN << "Error processing M-trap: "
                             "unable to read from named pipe '"
                          << d_pipeName << "'"
                          << BAEL_LOG_END;
            return;
        }
    }

    BAEL_LOG_TRACE << "The background thread has stopped" << BAEL_LOG_END;
}

int baea_PipeControlChannel::start(const bsl::string& pipeName)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

// BSLS_ASSERT(!d_isRunningFlag); // TBD: DOES createNamedPipe FAIL???
    if (d_isRunningFlag) {
        return 2;
    }
#ifdef BSLS_PLATFORM__OS_WINDOWS
    // See test driver case 9, on windows, createNamedPipe does not
    // fail if the named pipe is already open!!!!!
    // TBD
    if (d_isPipeOpen) {
        return 3;
    }
#endif

    if (0 != createNamedPipe(pipeName)) {
         BAEL_LOG_ERROR << "Unable to create named pipe '" << pipeName << "'"
                        << BAEL_LOG_END;
         return 1;
    }

    d_pipeName      = pipeName;
    d_isRunningFlag = d_isPipeOpen = true;

    int rc = bcemt_ThreadUtil::create(
                         &d_thread, bdef_BindUtil::bind(
                                &baea_PipeControlChannel::backgroundProcessor,
                                this));
    if (rc != 0) {
        BAEL_LOG_ERROR << "Cannot create processing thread, rc = " << rc
                       << BAEL_LOG_END;
        d_isRunningFlag = false;
        return rc;
    }

    return 0;
}

void baea_PipeControlChannel::shutdown()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    if (!d_isRunningFlag) {
        return;
    }

    if (bcemt_ThreadUtil::self() == d_thread) {
        // When 'shutdown' is called from the same thread as the background
        // thread perform a synchronous shutdown.
        d_isRunningFlag = false;
        return;
    }

    // The background thread is blocked on a call to 'ReadFile'.  Shutdown the
    // background thread by a) indicating that the thread should perform no
    // more reads and b) unblocking the background thread by sending an "empty"
    // message to the named pipe.

    d_isRunningFlag = false;

    int rc = sendEmptyMessage();
    if (0 != rc) {
        BAEL_LOG_ERROR << "Failed to properly close named pipe "
                          "'"       << d_pipeName << "'"
                          ": rc = " << rc
                       << BAEL_LOG_END;
    }
}

void baea_PipeControlChannel::dispatchMessageUpTo(
                                     const bsl::vector<char>::iterator& iter)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bdeut_StringRef stringRef(&(*d_buffer.begin()),
                              iter - d_buffer.begin());
    BAEL_LOG_TRACE << "Assembled complete messsage '"
                   << (bsl::string)stringRef << "'"
                   << BAEL_LOG_END;

    if (!stringRef.isEmpty()) {
        d_callback(stringRef);
    }
    d_buffer.erase(d_buffer.begin(), iter+1);
}

void baea_PipeControlChannel::stop()
{
    if (bcemt_ThreadUtil::invalidHandle() != d_thread) {
        bcemt_ThreadUtil::join(d_thread);
        d_thread = bcemt_ThreadUtil::invalidHandle();
    }

    d_isRunningFlag = false;

    if (d_isPipeOpen) {
        destroyNamedPipe();
        d_pipeName.clear();
        d_isPipeOpen = false;
    }
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
