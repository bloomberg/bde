// balb_pipecontrolchannel.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALB_PIPECONTROLCHANNEL
#define INCLUDED_BALB_PIPECONTROLCHANNEL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for reading control messages from a named pipe.
//
//@CLASSES:
// balb::PipeControlChannel: Mechanism for reading messages from a named pipe
//
//@SEE_ALSO: bdls_pipeutil
//
//@DESCRIPTION: This component provides a platform-independent mechanism,
// 'balb::PipeControlChannel', for establishing, monitoring, and shutting down
// a named pipe.  It reads text messages (generally short operational commands)
// from the pipe and passes them to a callback function.  Note that this
// component does *not* provide a general transport mechanism over a named
// pipe: it is specialized for the case that the messages to be read are
// relatively short newline-terminated strings.
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
///Pipe Atomicity
/// - - - - - - -
// Users that expect multiple concurrent writers to a single pipe must be aware
// that the message content might be corrupted (interleaved) unless:
//
//: 1 Each message is written to the pipe in a single 'write' system call.
//: 2 The length of each message is less than 'PIPE_BUF' (the limit for
//:   guaranteed atomicity).
//
// The value 'PIPE_BUF' depends on the platform:
//..
//   +------------------------------+------------------+
//   | Platform                     | PIPE_BUF (bytes) |
//   +------------------------------+------------------+
//   | POSIX (minimum requirement)) |    512           |
//   | IBM                          | 32,768           |
//   | SUN                          | 32,768           |
//   | Linux                        | 65,536           |
//   | Windows                      | 65,536           |
//   +------------------------------+------------------+
//..
//
// Also note that Linux allows the 'PIPE_BUF' size to be changed via the
// 'fcntl' system call.
//
///Pipe Names
///----------
// This component requires a fully-qualified native pipe name.
// 'bdlsu::PipeUtil' provides a portable utility method to generate such names.
//
///Message Format
///--------------
// This component requires a trailing newline ('\n') character at the end of
// each message.  This trailing newline is stripped from the message before
// the message is passed to the control callback.
//
///Platform-Specific Pipe Name Encoding Caveats
///--------------------------------------------
// Pipe-name encodings have the following caveats for the following operating
// systems:
//
//: o On Windows, methods of 'balb::PipeControlChannel' that take or return a
//:   pipe name as 'bsl::string' (or a reference to it) type assume that the
//:   name is encoded in UTF-8.  The routines attempt to convert the name to a
//:   UTF-16 'wchar_t' string via 'bdlde::CharConvertUtf16::utf8ToUtf16', and
//:   if the conversion succeeds, call the Windows wide-character 'W' APIs with
//:   the UTF-16 name.  If the conversion fails, the method fails.
//:
//:   o Narrow-character pipe names in other encodings, containing characters
//:     with values in the range 128 - 255, will likely result in pipes being
//:     created with names that appear garbled if the conversion from UTF-8 to
//:     UTF-16 happens to succeed.
//:
//:   o Neither 'utf8ToUtf16' nor the Windows 'W' APIs do any normalization of
//:     the UTF-16 strings resulting from UTF-8 conversion, and it is therefore
//:     possible to have sets of pipe names that have the same visual
//:     representation but are treated as different names by the system.
//:
//: o On Posix, a pipe name supplied to methods of 'balb::PipeControlChannel'
//:   as 'bsl::string' type is passed unchanged to the underlying system file
//:   APIs.  Because the pipe names are passed unchanged,
//:   'balb::PipeControlChannel' methods will work correctly on Posix with any
//:   encoding, but will *interoperate* only with processes that use the same
//:   encoding as the current process.
//:
//: o For compatibility with most modern Posix installs, and consistency with
//:   this component's Windows API, best practice is to encode all pipe names
//:   in UTF-8.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Controlling a Simple Server
/// - - - - - - - - - - - - - - - - - - -
// This example illustrates how to construct a simple server that records
// messages sent on a pipe control channel until "EXIT" is received, at which
// point the channel is closed and the server stops.
//
// First, let's define the implementation of our server.
//..
//                          // ===================
//                          // class ControlServer
//                          // ===================
//
//  class ControlServer {
//
//      // DATA
//      balb::PipeControlChannel d_channel;
//      bsl::vector<bsl::string> d_messages;
//
//      // PRIVATE MANIPULATORS
//      void onMessage(const bslstl::StringRef& message)
//      {
//          if ("EXIT" != message) {
//              d_messages.push_back(message);
//          }
//          else {
//              shutdown();
//          }
//      }
//
//    private:
//      // NOT IMPLEMENTED
//      ControlServer(const ControlServer&);             // = delete
//      ControlServer& operator=(const ControlServer&);  // = delete
//
//    public:
//      // CREATORS
//      explicit ControlServer(bslma::Allocator *basicAllocator = 0)
//      : d_channel(bdlf::BindUtil::bind(&ControlServer::onMessage,
//                                       this,
//                                       bdlf::PlaceHolders::_1),
//                  basicAllocator)
//      , d_messages(basicAllocator)
//      {}
//
//      // MANIPULATORS
//      int start(const bslstl::StringRef& pipeName)
//      {
//          return d_channel.start(pipeName);
//      }
//
//      void shutdown()
//      {
//          d_channel.shutdown();
//      }
//
//      void stop()
//      {
//          d_channel.stop();
//      }
//
//      // ACCESSORS
//      bsl::size_t numMessages() const
//      {
//          return d_messages.size();
//      }
//
//      const bsl::string& message(int index) const
//      {
//          return d_messages[index];
//      }
//  };
//..
// Now, construct and run the server using a canonical name for the pipe:
//..
//  bsl::string pipeName;
//  int         rc = bdls::PipeUtil::makeCanonicalName(&pipeName,
//                                                     "ctrl.pcctest");
//  assert(0 == rc);
//
//  ControlServer server;
//
//  rc = server.start(pipeName);
//  if (0 != rc) {
//      cout << "ERROR: Failed to start pipe control channel" << endl;
//  }
//..
// Once the server is started, clients can send messages to the server.
//..
//  const char MSG0[]  = "this is the first message";
//  const char MSG1[]  = "this is the second message";
//
//  rc = bdls::PipeUtil::send(pipeName, bsl::string(MSG0) + "\n");
//  assert(0 == rc);
//  rc = bdls::PipeUtil::send(pipeName, bsl::string(MSG1) + "\n");
//  assert(0 == rc);
//  rc = bdls::PipeUtil::send(pipeName, "EXIT\n");
//  assert(0 == rc);
//..
// The server shuts down once it processes the "EXIT" control message.
//..
//  server.stop();  // block until shutdown
//..
// Finally, let's ensure the server received each control message sent.
//..
//  assert(2 == server.numMessages());
//  assert(bsl::string(MSG0) == server.message(0));
//  assert(bsl::string(MSG1) == server.message(1));
//..

#include <balscm_version.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslmf_assert.h>

#include <bsls_atomic.h>
#include <bsls_libraryfeatures.h>

#include <bsl_functional.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bslma_allocator.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

#include <string>           // 'std::string', 'std::pmr::string'

namespace BloombergLP {
namespace balb {
                          // ========================
                          // class PipeControlChannel
                          // ========================

class PipeControlChannel {
    // This class is a mechanism for reading control messages from a named
    // pipe.  Use 'start' to spawn a thread that handles messages arriving at
    // the pipe, and 'shutdown' to stop reading.  'stop'  blocks until the
    // processing thread has been terminated by a call to 'shutdown' (either
    // before 'stop' is called, or from some other thread).

  public:
    // TYPES
    typedef bsl::function<void(const bslstl::StringRef& message)>
                                                               ControlCallback;
        // This type of function is called to handle control messages received
        // on the pipe.  The 'message' is one complete message read from the
        // pipe, without the terminating newline character.

  private:
    // TYPES
    enum BackgroundThreadState {
        e_STOPPED,  // The background thread is not running or about to exit.
        e_RUNNING,  // The background thread is running normally.
        e_STOPPING  // The background thread is requested to stop.
    };

    // INSTANCE DATA
    ControlCallback           d_callback;     // callback for control messages
    bsl::string               d_pipeName;     // full path name of pipe
    bsl::vector<char>         d_buffer;       // message buffer
    bslmt::ThreadUtil::Handle d_thread;       // background processing thread
    bsls::AtomicInt           d_backgroundState; // the background thread state
    bool                      d_isPipeOpen;   // true if the pipe is still open

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

    int createNamedPipe(const char *pipeName);
        // Open a named pipe having the specified 'pipeName'.  Return 0 on
        // success, and a non-zero value otherwise.

    void dispatchMessageUpTo(const bsl::vector<char>::iterator& iter);
        // Dispatch the message that extends up to (but not including) the
        // specified 'iter' (which is an iterator into 'd_buffer'), then erase
        // the prefix that extends up to (and including) 'iter'.

    bool dispatchLeftoverMessage();
        // If there is a newline character in 'd_buffer', call
        // 'dispatchMessageUpTo' with the location of that newline character
        // and return 'true'; otherwise, return 'false'.

    void destroyNamedPipe();
        // Close the named pipe.

    int readNamedPipe();
        // Block until bytes are available on the named pipe, and read them
        // into the internal buffer.  If a message is encountered, dispatch it.
        // Return 0 on success, and a non-zero value otherwise.

    int sendEmptyMessage();
        // Writes a '\n' character, only, to the pipe.  Returns 0 on success, a
        // value greater than 0 on error, and a value less than 0 in case of a
        // timeout.  Used to unblock the reading thread so it can detect a
        // shutdown condition.  Note that this method is not to be called
        // anywhere except from 'shutdown'.

  private:
    // NOT IMPLEMENTED
    PipeControlChannel(const PipeControlChannel&);
    PipeControlChannel& operator=(const PipeControlChannel&);

  public:
    // CREATORS
    explicit
    PipeControlChannel(const ControlCallback&  callback,
                       bslma::Allocator       *basicAllocator = 0);
        // Create a pipe control mechanism that dispatches messages to the
        // specified 'callback'.  Optionally specify 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is zero, the currently installed
        // default allocator is used.

    ~PipeControlChannel();
        // Destroy this object.  Shut down the processing thread if it is still
        // running and block until it terminates.  Close the named pipe and
        // clean up any associated system resources.

    // MANIPULATORS
    int start(const char                     *pipeName);
    template <class STRING_TYPE>
    int start(const STRING_TYPE&              pipeName);
    int start(const char                     *pipeName,
              const bslmt::ThreadAttributes&  attributes);
    template <class STRING_TYPE>
    int start(const STRING_TYPE&              pipeName,
              const bslmt::ThreadAttributes&  attributes);
        // Open a named pipe having the specified 'pipeName', and start a
        // thread to read messages and dispatch them to the callback specified
        // at construction.  Optionally specify 'attributes' of the background
        // processing thread.  If 'attributes' is not supplied, a default
        // constructed 'ThreadAttributes' object will be used.  Return 0 on
        // success, and a non-zero value otherwise.  In particular, return a
        // non-zero value if the pipe cannot be opened or if it is detected
        // that another process is reading from the pipe.  'pipeName' must be
        // of the types 'const char *', 'char *', 'bsl::string', 'std::string',
        // 'std::pmr::string' (if supported), or 'bslstl::StringRef'.

    void shutdown();
        // Stop reading from the pipe and dispatching messages.  If the
        // background thread has begun processing a message, this method will
        // block until a message that is currently being processed completes.

    void stop();
        // Block until the background thread has been terminated by a call to
        // 'shutdown'.  Then close the pipe and clean up the associated file.

    // ACCESSORS
    const bsl::string& pipeName() const;
        // Return the fully qualified system name of the pipe.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.
};

                    // ====================================
                    // class PipeControlChannel_CStringUtil
                    // ====================================

struct PipeControlChannel_CStringUtil {
    // This component-private utility 'struct' provides a namespace for the
    // 'flatten' overload set intended to be used in concert with an overload
    // set consisting of a function template with a deduced argument and an
    // non-template overload accepting a 'const char *'.  The actual
    // implementation of the functionality would be in the 'const char *'
    // overload whereas the purpose of the function template is to invoke the
    // 'const char *' overload with a null-terminated string.
    //
    // The function template achieves null-termination by recursively calling
    // the function and supplying it with the result of 'flatten' invoked on
    // the deduced argument.  This 'flatten' invocation will call 'c_str()' on
    // various supported 'string' types, will produce a temporary 'bsl::string'
    // for possibly non-null-terminated 'bslstl::StringRef', and will result in
    // a 'BSLMF_ASSERT' for any unsupported type.  Calling the function with
    // the temporary 'bsl::string' produced from 'bslstl::StringRef' will
    // result in a second invocation of 'flatten', this time producing
    // 'const char *', and finally calling the function with a null-terminated
    // string.
    //
    // Note that the 'bslstl::StringRef' overload for 'flatten' is provided for
    // backwards compatibility.  Without it, the 'bsl::string' and
    // 'std::string' overloads would be ambiguous.  In new code, it is
    // preferable to not provide 'bslstl::StringRef' overload in a similar
    // facility and require the clients to explicitly state the string type in
    // their code, making a potential allocation obvious.  The
    // 'bsl::string_view' overload is not provided for the same reason.
    //
    // Also note that since the constructor for 'string' types from
    // 'bsl::string_view' is explicit, it is not necessary to support
    // 'bsl::string_view' for backwards compatibility, and it is not supported.

    // CLASS METHODS

    static const char *flatten(char *cString);
    static const char *flatten(const char *cString);
        // Return the specified 'cString'.

    static const char *flatten(const bsl::string& string);
    static const char *flatten(const std::string& string);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static const char *flatten(const std::pmr::string& string);
#endif
        // Return the result of invoking 'c_str()' on the specified 'string'.

    static bsl::string flatten(const bslstl::StringRef& stringRef);
        // Return a temporary 'bsl::string' constructed from the specified
        // 'stringRef'.

    template <class TYPE>
    static const char *flatten(const TYPE&);
        // Produce a compile-time error informing the caller that the
        // parameterized 'TYPE' is not supported as the parameter for the call.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class PipeControlChannel
                          // ------------------------

// MANIPULATORS
template <class STRING_TYPE>
int PipeControlChannel::start(const STRING_TYPE& pipeName)
{
    return start(PipeControlChannel_CStringUtil::flatten(pipeName),
                 bslmt::ThreadAttributes());
}

template <class STRING_TYPE>
int PipeControlChannel::start(const STRING_TYPE&             pipeName,
                              const bslmt::ThreadAttributes& threadAttributes)
{
    return start(PipeControlChannel_CStringUtil::flatten(pipeName),
                 threadAttributes);
}

// ACCESSORS
inline
const bsl::string& PipeControlChannel::pipeName() const
{
    return d_pipeName;
}

                    // Aspects

inline
bslma::Allocator *PipeControlChannel::allocator() const
{
    return d_pipeName.get_allocator().mechanism();
}

                    // ------------------------------------
                    // class PipeControlChannel_CStringUtil
                    // ------------------------------------

// CLASS METHODS
inline
const char *PipeControlChannel_CStringUtil::flatten(char *cString)
{
    return cString;
}

inline
const char *PipeControlChannel_CStringUtil::flatten(const char *cString)
{
    return cString;
}

inline
const char *PipeControlChannel_CStringUtil::flatten(const bsl::string& string)
{
    return string.c_str();
}

inline
const char *PipeControlChannel_CStringUtil::flatten(const std::string& string)
{
    return string.c_str();
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
inline
const char *PipeControlChannel_CStringUtil::flatten(
                                                const std::pmr::string& string)
{
    return string.c_str();
}
#endif

inline
bsl::string PipeControlChannel_CStringUtil::flatten(
                                            const bslstl::StringRef& stringRef)
{
    return stringRef;
}

template <class TYPE>
inline
const char *PipeControlChannel_CStringUtil::flatten(const TYPE&)
{
    BSLMF_ASSERT(("Unsupported parameter type." && !sizeof(TYPE)));
    return 0;
}

}  // close package namespace
}  // close enterprise namespace
#endif

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
