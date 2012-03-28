// baea_pipecontrolchannel.h   -*-C++-*-
#ifndef INCLUDED_BAEA_PIPECONTROLCHANNEL
#define INCLUDED_BAEA_PIPECONTROLCHANNEL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for reading control messages from a named pipe
//
//@AUTHOR: David Schumann (dschumann1)
//
//@CLASSES:
// baea_PipeControlChannel: Mechanism for reading messages from a named pipe
//
//@SEE_ALSO: bdesu_PipeUtil
//
//@DESCRIPTION: This component provides a platform-independent mechanism for
// establishing, monitoring, and shutting down a named pipe.  It reads text
// messages (generally short operational commands) from the pipe and passes
// them to a callback function.  Note that this component does *not* provide
// a general transport mechanism over a named pipe: it is specialized for the
// case that the messages to be read are relatively short newline-terminated
// strings.
//
///Thread Safety
///-------------
// This component is thread-safe but not thread-enabled, meaning that multiple
// threads may safely use their own instances of PipeControlChannel, but may
// not manipulate the same PipeControlChannel simultaneously (except that
// 'shutdown' may always be called safely).  The 'start' function creates a
// new thread which listens to the pipe for messages until 'shutdown' is
// called.
//
///Pipe Names
///----------
// This component requires a fully-qualified native pipe name.
// 'bdesu_PipeUtil' provides a portable utility method to generate such names.
//
///Message Format
///--------------
// This component requires a trailing newline ('\n') character at the end of
// each message.  This trailing newline is stripped from the message before
// the message is passed to the control callback.
//
///Usage Example
///-------------
// This example illustrates how to construct a simple server that records
// messages sent on a pipe control channel until "EXIT" is received, at which
// point the channel is closed and the server stops.
//
// First, let's define the implementation of our server.
//..
//   class ControlServer
//   {
//       // DATA
//       baea_PipeControlChannel  d_channel;
//       bsl::vector<bsl::string> d_messages;
//
//       // PRIVATE MANIPULATORS
//       void onMessage(const bdeut_StringRef& message)
//       {
//           if ("EXIT" != message) {
//               d_messages.push_back(message);
//           }
//           else {
//               shutdown();
//           }
//       }
//
//   public:
//       // CREATORS
//       explicit ControlServer(bslma_Allocator *basicAllocator)
//       : d_channel(bdef_BindUtil::bind(&ControlServer::onMessage,
//                                       this,
//                                       bdef_PlaceHolders::_1),
//                   basicAllocator)
//       , d_messages(basicAllocator)
//       {}
//
//       // MANIPULATORS
//       int start(const char *pipeName) {
//           return d_channel.start(pipeName);
//       }
//
//       void shutdown() {
//           d_channel.shutdown();
//       }
//
//       void stop() {
//           d_channel.stop();
//       }
//
//       // ACCESSORS
//       int numMessages() const {
//           return d_messages.size();
//       }
//
//       const bsl::string& message(int index) const {
//           return d_messages[index];
//       }
//   };
//..
// Now, construct and run the server using a canonical name for the pipe:
//..
//   bsl::string pipeName;
//   bdesu_PipeUtil::makeCanonicalPipeName(&pipeName, "ctrl.pcctest");
//
//   ControlServer server;
//
//   int rc = server.start(pipeName);
//   if (0 != rc) {
//       cout << "ERROR: Failed to start pipe control channel" << endl;
//   }
//..
// Once the server is started, clients can send messages to the server.
//..
//   const char MSG0[]  = "this is the first message";
//   const char MSG1[]  = "this is the second message";
//
//   int rc;
//   rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG0) + "\n");
//   assert(0 == rc);
//   rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG1) + "\n");
//   assert(0 == rc);
//   rc = bdesu_PipeUtil::send(pipeName, "EXIT");
//   assert(0 == rc);
//..
// The server shuts down once it processes the "EXIT" control message.
//..
//   server.stop();  // block until shutdown
//..
// Finally, let's ensure the server received each control message sent.
//..
//   assert(2 == server.numMessages());
//   assert(bsl::string(MSG0) == server.message(0));
//   assert(bsl::string(MSG1) == server.message(1));
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                       // =============================
                       // class baea_PipeControlChannel
                       // =============================

class baea_PipeControlChannel {
    // This class is a mechanism for reading control messages from a named
    // pipe.  Use 'start' to spawn a thread that handles messages arriving at
    // the pipe, and 'shutdown' to stop reading.  'stop'  blocks until the
    // processing thread has been terminated by a call to 'shutdown' (either
    // before 'stop' is called, or from some other thread).

  public:
    // TYPES
    typedef bdef_Function<void (*)(const bdeut_StringRef& message)>
                                                               ControlCallback;
        // This type of function is called to handle control messages received
        // on the pipe.  The 'message' is one complete message read from the
        // pipe, without the terminating newline character.

  private:
    // INSTANCE DATA
    ControlCallback          d_callback;      // Callback for control messages
    bsl::string              d_pipeName;      // full path name of pipe
    bsl::vector<char>        d_buffer;        // message buffer
    bcemt_ThreadUtil::Handle d_thread;        // background processing thread
    bool                     d_isRunningFlag; // true if the channel is running
    bool                     d_isPipeOpen;    // true if the pipe is still open

    union {
        struct {
            int              d_readFd;        // fifo descriptor (read-only)
            int              d_writeFd;       // fifo descriptor (write-only)
        } d_unix;

        struct {
            void*            d_handle;        // pipe handle
        } d_windows;

    } d_impl;                                 // platform-specific imp

    // PRIVATE MANIPULATORS
    void backgroundProcessor();
        // Loop while d_isRunningFlag is true, reading and dispatching messages
        // from the named pipe.

    int createNamedPipe(const bsl::string& pipeName);
        // Open a named pipe having the specified 'pipeName'.  Return 0 on
        // success, and a non-zero value otherwise.

    void dispatchMessageUpTo(const bsl::vector<char>::iterator& iter);
        // Dispatch the message that extends up to the specified 'iter'.

    void destroyNamedPipe();
         // Close the named pipe.

    int readNamedPipe();
        // Block until bytes are available on the named pipe, and read them
        // into the internal buffer.  If a message is encountered, dispatch it.
        // Return 0 on success, and a non-zero value otherwise.

    int sendEmptyMessage();
        // Writes a '\n' character, only, to the pipe.  Used to unblock the
        // reading thread so it can detect a shutdown condition.

  private:
    // NOT IMPLEMENTED
    baea_PipeControlChannel(const baea_PipeControlChannel&);
    baea_PipeControlChannel& operator=(const baea_PipeControlChannel&);

  public:
    // CREATORS
    explicit
    baea_PipeControlChannel(const ControlCallback&  callback,
                            bslma_Allocator        *basicAllocator = 0);
        // Create a pipe control mechanism that dispatches messages to the
        // specified 'callback'.  Use 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is zero, the currently installed default allocator
        // is used.

    ~baea_PipeControlChannel();
        // Destroy this object.  Shut down the processing thread if it is still
        // running and block until it terminates.  Close the named pipe and
        // clean up any associated system resources.

    // MANIPULATORS
    int start(const bsl::string& pipeName);
        // Open a named pipe having the specified 'pipeName', and start a
        // thread to read messages and dispatch them to the callback specified
        // at construction.  Return 0 on success, and a non-zero value
        // otherwise.  In particular, return a non-zero value if the pipe
        // cannot be opened or if it is detected that another process is
        // reading from the pipe.

    void shutdown();
        // Stop reading from the pipe and dispatching messages.  Note that this
        // method does not block.

    void stop();
        // Block until the background thread has been terminated by a call to
        // 'shutdown'.  Then close the pipe and clean up the associated file.

    // ACCESSORS
    const bsl::string& pipeName() const;
        // Return the fully qualified system name of the pipe.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
const bsl::string& baea_PipeControlChannel::pipeName() const
{
    return d_pipeName;
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
