// btlmt_tcptimereventmanager.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_tcptimereventmanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_tcptimereventmanager_cpp,"$Id$ $CSID$")

#include <btlso_defaulteventmanager.h>
#include <btlso_defaulteventmanager_devpoll.h>
#include <btlso_defaulteventmanager_epoll.h>
#include <btlso_defaulteventmanager_poll.h>
#include <btlso_defaulteventmanager_select.h>
#include <btlso_eventmanager.h>
#include <btlso_platform.h>
#include <btlso_socketimputil.h>

#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>
#include <bslmt_threadattributes.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bslalg_typetraits.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslma_autorawdeleter.h>
#include <bslma_default.h>
#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>                // printf
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_algorithm.h>

#include <bsl_c_errno.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <bsl_c_signal.h>              // sigfillset
#endif

namespace BloombergLP {

// Confirm that we use the right size atomic variable for the socket handle in
// the control channel.

#if defined(BTLSO_PLATFORM_WIN_SOCKETS) && defined(BSLS_PLATFORM_CPU_64_BIT)

BSLMF_ASSERT(8 == sizeof(btlso::SocketHandle::Handle));

#else

BSLMF_ASSERT(4 == sizeof(btlso::SocketHandle::Handle));

#endif

enum {
   // This 'enum' specifies the constants used in this component.

    k_MAX_NUM_RETRIES = 3 // Maximum number of times the control channel can be
                          // reinitialized.
};

namespace btlmt {

                // =========================================
                // class TcpTimerEventManager_ControlChannel
                // =========================================

// IMPLEMENTATION NOTES: This class manages a client-server pair of connected
// sockets.  It is used to communicate requests to the dispatcher thread while
// it is executing the dispatcher loop.  This communication is done via sockets
// because this ensures that the loop is unblocked if it is blocking in the
// socket polling step (when no timers are enqueued).
//
// Reinitialization of the ControlChannel: Occasionally on Windows machines one
// of the client-server sockets can be killed by firewall or security software.
// This makes the ControlChannel useless and completely paralyzes the
// functioning of 'TcpTimerEventManager'.  The fix to this problem was to
// identify this scenario and restart the ControlChannel.  In that scenario all
// data associated with the channel is lost and the channel opens a new
// client-server socket pair.  Subsequent communication is expected to work as
// before.  'TcpTimerEventManager' tries to recover from this error
// 'k_MAX_NUM_RETRIES' times after which it asserts on detecting this error.

                   // ==================================
                   // class TcpTimerEventManager_Request
                   // ==================================

class TcpTimerEventManager_Request {
    // This class represents a request to the dispatcher thread.  It contains
    // all the parameters associated with the request and, depending on the
    // request type, can be used to report results.  It is simple by design,
    // and does not use dynamic memory.  It is used by the timer event manager
    // to synchronize with the dispatcher thread, and is enqueued onto a
    // ControlChannel (connect pair of sockets) in order to trigger the polling
    // mechanism in the dispatcher thread loop, even if no other event is
    // scheduled.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TcpTimerEventManager_Request,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    enum OpCode {
        e_NO_OP,                         // no operation
        e_TERMINATE,                     // exit signal
        e_DEREGISTER_ALL_SOCKET_EVENTS,  // invoke 'deregisterAllSocketEvents'
        e_DEREGISTER_ALL_TIMERS,         // invoke 'deregisterAllTimers'
        e_DEREGISTER_SOCKET_EVENT,       // invoke 'deregisterSocketEvent'
        e_DEREGISTER_SOCKET,             // invoke 'deregisterSocket'
        e_DEREGISTER_TIMER,              // invoke 'deregisterTimer'
        e_REGISTER_SOCKET_EVENT,         // invoke 'registerSocketEvent'
        e_EXECUTE,                       // invoke 'execute'
        e_REGISTER_TIMER,                // invoke 'registerTimer'
        e_RESCHEDULE_TIMER,              // invoke 'rescheduleTimer'
        e_IS_REGISTERED,                 // invoke 'isRegistered'.
        e_NUM_SOCKET_EVENTS              // invoke 'numSocketEvents'.
    };

  private:
    // DATA
    OpCode                         d_opCode;       // request type
    bslmt::Mutex                  *d_mutex_p;      // result notification
    bslmt::Condition              *d_condition_p;  //

    // The following two fields are used in socket related requests.
    btlso::SocketHandle::Handle    d_handle;       // socket handle associated
                                                   // with this request (in)
    btlso::EventType::Type         d_eventType;    // event code (in)

    // The following two fields are used in timer related requests.
    bsls::TimeInterval             d_timeout;      // timeout interval (in)
    void                          *d_timerId;      // timer ID (in/out)

    // The following field is used in both socket and timer related
    // registration requests.

    btlso::EventManager::Callback  d_callback;     // callback to be registered
                                                   // (in)

    // The following object is used for responses.
    int                            d_result;       // (out)

  public:
    // CREATORS
    TcpTimerEventManager_Request(
                     const btlso::SocketHandle::Handle&    handle,
                     btlso::EventType::Type                event,
                     const btlso::EventManager::Callback&  callback,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create a 'e_REGISTER_SOCKET_EVENT' request containing the specified
        // socket 'handle', the specified 'event' and the specified 'callback'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TcpTimerEventManager_Request(
                     const bsls::TimeInterval&             timeout,
                     const btlso::EventManager::Callback&  callback,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create a 'e_REGISTER_TIMER' request containing the specified
        // 'timeout' and the specified 'callback'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TcpTimerEventManager_Request(
                                const void                *timerId,
                                const bsls::TimeInterval&  timeout,
                                bslma::Allocator          *basicAllocator = 0);
        // Create a 'e_RESCHEDULE_TIMER' request containing the specified
        // 'timerId' and the specified 'timeOut'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TcpTimerEventManager_Request(void             *timerId,
                                 bslma::Allocator *basicAllocator = 0);
        // Create a 'e_DEREGISTER_TIMER' request containing the specified
        // 'timerId'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    TcpTimerEventManager_Request(
                       const btlso::SocketHandle::Handle&  handle,
                       bslma::Allocator                   *basicAllocator = 0);
        // Create a 'e_DEREGISTER_SOCKET' request containing the specified
        // 'handle'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    TcpTimerEventManager_Request(
                              btlso::SocketHandle::Handle  handle,
                              btlso::EventType::Type       event,
                              bslma::Allocator            *basicAllocator = 0);
        // Create a 'e_DEREGISTER_SOCKET_EVENT' request containing the
        // specified 'handle' and the specified 'event'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TcpTimerEventManager_Request(
                              btlso::SocketHandle::Handle  handle,
                              btlso::EventType::Type       event,
                              bslmt::Condition            *condition,
                              bslmt::Mutex                *mutex,
                              bslma::Allocator            *basicAllocator = 0);
        // Create an 'e_IS_REGISTERED' request containing the specified
        // 'handle' and the specified 'event'; the specified 'condition' is
        // signaled when the request is processed and the specified 'mutex' is
        // used to synchronize access to the result (i.e., 'result').
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TcpTimerEventManager_Request(
                       const btlso::SocketHandle::Handle&  handle,
                       bslmt::Condition                   *condition,
                       bslmt::Mutex                       *mutex,
                       bslma::Allocator                   *basicAllocator = 0);
        // Create a 'e_NUM_SOCKET_EVENTS' request containing the specified
        // 'handle' the specified 'condition' is signaled when the request is
        // processed and the specified 'mutex' is used to synchronize access to
        // the result (i.e., 'result').  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    TcpTimerEventManager_Request(OpCode            code,
                                 bslma::Allocator *basicAllocator = 0);
        // Create a request having the specified 'code'.  The behavior is
        // undefined unless 'code' is 'e_DEREGISTER_ALL_SOCKET_EVENTS',
        // 'e_DEREGISTER_ALL_TIMERS' or 'e_NO_OP'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TcpTimerEventManager_Request(OpCode            code,
                                 bslmt::Condition *condition,
                                 bslmt::Mutex     *mutex,
                                 bslma::Allocator *basicAllocator = 0);
        // Create a request with the specified 'code'; the request will be
        // processed according to the value of 'code'.  The behavior is
        // undefined if 'code' has a value that requires some fields other than
        // 'condition' or 'mutex' to be defined.  The treatment of the
        // specified 'condition' and 'mutex' depends on the value of 'opCode'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TcpTimerEventManager_Request(
                     const btlso::EventManager::Callback&  callback,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create an 'e_EXECUTE' request for the specified 'functor.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~TcpTimerEventManager_Request();
        // Destroy this request

    // MANIPULATORS
    void setTimerId(void *value);
        // Set the timer id contained in this request to the specified 'value'.
        // The behavior is undefined unless 'opCode' is 'e_REGISTER_TIMER'.

    void setResult(int value);
        // Set the result contained in this request to the specified 'value'.
        // The behavior is undefined unless 'opCode' is 'e_IS_REGISTERED'.

    void signal();
        // Signal the completion of the processing of this request.  The
        // behavior is undefined unless 'opCode' refers to one of the blocking
        // request types.

    void waitForResult();
        // Suspend the calling thread until this request is processed and the
        // result is available.  The behavior is undefined unless 'opCode' for
        // this request refers to one of the blocking request types.

    // ACCESSORS
    const btlso::EventManager::Callback& callback() const;
        // Return the callback contained in this request.  The behavior is
        // undefined unless 'opCode' is either 'e_REGISTER_SOCKET_EVENT' or
        // 'e_REGISTER_TIMER'.

    btlso::EventType::Type event() const;
        // Return the socket event type contained in this request.  The
        // behavior is undefined unless 'opCode' is one of the following:
        // 'e_REGISTER_SOCKET_EVENT', 'e_DEREGISTER_SOCKET_EVENT'.

    OpCode opCode() const;
        // Return the op code for this request;

    const btlso::SocketHandle::Handle& socketHandle() const;
        // Return the socked handle contained in this request.  The behavior is
        // undefined unless 'opCode' is one of the following:
        // 'e_REGISTER_SOCKET_EVENT', 'e_DEREGISTER_SOCKET_EVENT',
        // 'e_DEREGISTER_SOCKET', 'e_NUM_SOCKET_EVENTS'.

    int result() const;
        // Return the result contained in this request.

    const bsls::TimeInterval& timeout() const;
        // Return the timeout value contained with this request.  The behavior
        // is undefined unless 'opCode' is 'e_REGISTER_TIMER' or
        // 'e_RESCHEDULE_TIMER'.

    const void *timerId() const;
        // Return the timer 'id' contained in this object.  The behavior is
        // undefined unless 'opCode' is 'e_REGISTER_TIMER',
        // 'e_RESCHEDULE_TIMER' or 'e_DEREGISTER_TIMER'.
};

                   // ----------------------------------
                   // class TcpTimerEventManager_Request
                   // ----------------------------------

// CREATORS
inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                          const btlso::SocketHandle::Handle&    handle,
                          btlso::EventType::Type                event,
                          const btlso::EventManager::Callback&  callback,
                          bslma::Allocator                     *basicAllocator)
: d_opCode(e_REGISTER_SOCKET_EVENT)
, d_mutex_p(0)
, d_condition_p(0)
, d_handle(handle)
, d_eventType(event)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator),
             callback)
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                          const bsls::TimeInterval&             timeout,
                          const btlso::EventManager::Callback&  callback,
                          bslma::Allocator                     *basicAllocator)
: d_opCode(e_REGISTER_TIMER)
, d_mutex_p(0)
, d_condition_p(0)
, d_timeout(timeout)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator),
             callback)
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                     const void                *timerId,
                                     const bsls::TimeInterval&  timeout,
                                     bslma::Allocator          *basicAllocator)
: d_opCode(e_RESCHEDULE_TIMER)
, d_mutex_p(0)
, d_condition_p(0)
, d_timeout(timeout)
, d_timerId(const_cast<void *>(timerId))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                              void             *timerId,
                                              bslma::Allocator *basicAllocator)
: d_opCode(e_DEREGISTER_TIMER)
, d_mutex_p(0)
, d_condition_p(0)
, d_timerId(timerId)
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                            const btlso::SocketHandle::Handle&  handle,
                            bslma::Allocator                   *basicAllocator)
: d_opCode(e_DEREGISTER_SOCKET)
, d_mutex_p(0)
, d_condition_p(0)
, d_handle(handle)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                            const btlso::SocketHandle::Handle&  handle,
                            bslmt::Condition                   *condition,
                            bslmt::Mutex                       *mutex,
                            bslma::Allocator                   *basicAllocator)
: d_opCode(e_NUM_SOCKET_EVENTS)
, d_mutex_p(mutex)
, d_condition_p(condition)
, d_handle(handle)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                   btlso::SocketHandle::Handle  handle,
                                   btlso::EventType::Type       event,
                                   bslma::Allocator            *basicAllocator)
: d_opCode(e_DEREGISTER_SOCKET_EVENT)
, d_mutex_p(0)
, d_condition_p(0)
, d_handle(handle)
, d_eventType(event)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                   btlso::SocketHandle::Handle  handle,
                                   btlso::EventType::Type       event,
                                   bslmt::Condition            *condition,
                                   bslmt::Mutex                *mutex,
                                   bslma::Allocator            *basicAllocator)
: d_opCode(e_IS_REGISTERED)
, d_mutex_p(mutex)
, d_condition_p(condition)
, d_handle(handle)
, d_eventType(event)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                              OpCode            code,
                                              bslma::Allocator *basicAllocator)
: d_opCode(code)
, d_mutex_p(0)
, d_condition_p(0)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
    BSLS_ASSERT(e_NO_OP                        == code
             || e_TERMINATE                    == code
             || e_DEREGISTER_ALL_SOCKET_EVENTS == code
             || e_DEREGISTER_ALL_TIMERS        == code);
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                                              OpCode            code,
                                              bslmt::Condition *condition,
                                              bslmt::Mutex     *mutex,
                                              bslma::Allocator *basicAllocator)
: d_opCode(code)
, d_mutex_p(mutex)
, d_condition_p(condition)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator))
, d_result(-1)
{
    BSLS_ASSERT(e_NO_OP == code || e_TERMINATE == code);
}

inline
TcpTimerEventManager_Request::TcpTimerEventManager_Request(
                          const btlso::EventManager::Callback&  callback,
                          bslma::Allocator                     *basicAllocator)
: d_opCode(e_EXECUTE)
, d_mutex_p(0)
, d_condition_p(0)
, d_timerId(static_cast<void *>(0))
, d_callback(bsl::allocator_arg_t(),
             bsl::allocator<btlso::EventManager::Callback>(basicAllocator),
             callback)
, d_result(-1)
{
}

inline
TcpTimerEventManager_Request::~TcpTimerEventManager_Request()
{
}

// MANIPULATORS
inline
void TcpTimerEventManager_Request::setTimerId(void *value)
{
    bslmt::LockGuard<bslmt::Mutex> lock(d_mutex_p);

    d_timerId = value;
}

inline
void TcpTimerEventManager_Request::setResult(int value)
{
    bslmt::LockGuard<bslmt::Mutex> lock(d_mutex_p);

    d_result = value;
}

inline
void TcpTimerEventManager_Request::signal()
{
    BSLS_ASSERT(d_condition_p);

    d_condition_p->signal();
}

inline
void TcpTimerEventManager_Request::waitForResult()
{
    switch (d_opCode) {
      case e_NO_OP:                                             // FALL THROUGH
      case e_IS_REGISTERED:                                     // FALL THROUGH
      case e_NUM_SOCKET_EVENTS: {
        while (-1 == d_result) {
            d_condition_p->wait(d_mutex_p);
        }
      } break;
      case e_REGISTER_TIMER: {
        while (!d_timerId) {
            d_condition_p->wait(d_mutex_p);
        }
      } break;
      default: {
        BSLS_ASSERT("MUST BE UNREACHABLE BY DESIGN." && 0);
      } break;
    }
}

// ACCESSORS
inline
const btlso::EventManager::Callback&
TcpTimerEventManager_Request::callback() const
{
    BSLS_ASSERT(d_opCode == e_REGISTER_SOCKET_EVENT
             || d_opCode == e_REGISTER_TIMER
             || d_opCode == e_EXECUTE);

    return d_callback;
}

inline
btlso::EventType::Type TcpTimerEventManager_Request::event() const
{
    BSLS_ASSERT(e_REGISTER_SOCKET_EVENT   == d_opCode
             || e_DEREGISTER_SOCKET_EVENT == d_opCode
             || e_IS_REGISTERED           == d_opCode);

    return d_eventType;
}

inline
TcpTimerEventManager_Request::OpCode
TcpTimerEventManager_Request::opCode() const
{
    return d_opCode;
}

inline
const btlso::SocketHandle::Handle&
TcpTimerEventManager_Request::socketHandle() const
{
    BSLS_ASSERT(e_REGISTER_SOCKET_EVENT   == d_opCode
             || e_DEREGISTER_SOCKET_EVENT == d_opCode
             || e_DEREGISTER_SOCKET       == d_opCode
             || e_IS_REGISTERED           == d_opCode
             || e_NUM_SOCKET_EVENTS       == d_opCode);

    return d_handle;
}

inline
int TcpTimerEventManager_Request::result() const
{
    return d_result;
}

inline
const bsls::TimeInterval& TcpTimerEventManager_Request::timeout() const
{
    return d_timeout;
}

inline
const void *TcpTimerEventManager_Request::timerId() const
{
    BSLS_ASSERT(e_EXECUTE          == d_opCode
             || e_REGISTER_TIMER   == d_opCode
             || e_RESCHEDULE_TIMER == d_opCode
             || e_DEREGISTER_TIMER == d_opCode);

    return d_timerId;
}

inline
const char *toAscii(btlmt::TcpTimerEventManager_Request::OpCode value)
{
#define CASE(X) case(X): return #X;

    switch (value) {
      CASE(btlmt::TcpTimerEventManager_Request::e_NO_OP)
      CASE(btlmt::TcpTimerEventManager_Request::e_EXECUTE)
      CASE(btlmt::TcpTimerEventManager_Request::e_TERMINATE)
      CASE(btlmt::TcpTimerEventManager_Request::e_DEREGISTER_ALL_SOCKET_EVENTS)
      CASE(btlmt::TcpTimerEventManager_Request::e_DEREGISTER_ALL_TIMERS)
      CASE(btlmt::TcpTimerEventManager_Request::e_DEREGISTER_SOCKET_EVENT)
      CASE(btlmt::TcpTimerEventManager_Request::e_DEREGISTER_SOCKET)
      CASE(btlmt::TcpTimerEventManager_Request::e_DEREGISTER_TIMER)
      CASE(btlmt::TcpTimerEventManager_Request::e_REGISTER_SOCKET_EVENT)
      CASE(btlmt::TcpTimerEventManager_Request::e_REGISTER_TIMER)
      CASE(btlmt::TcpTimerEventManager_Request::e_IS_REGISTERED)
      CASE(btlmt::TcpTimerEventManager_Request::e_NUM_SOCKET_EVENTS)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         TcpTimerEventManager_Request::OpCode rhs)
{
    return stream << toAscii(rhs);
}

              // -----------------------------------------
              // class TcpTimerEventManager_ControlChannel
              // -----------------------------------------

// CREATORS
TcpTimerEventManager_ControlChannel::TcpTimerEventManager_ControlChannel()
: d_byte(0x53)
, d_numServerReads(0)
, d_numServerBytesRead(0)
{
    const int rc = open();
    BSLS_ASSERT(0 == rc);

    (void) rc;                                   // quash warning in opt builds
}

TcpTimerEventManager_ControlChannel::~TcpTimerEventManager_ControlChannel()
{
    const int rc = close();
    BSLS_ASSERT(0 == rc);

    (void) rc;                                   // quash warning in opt builds
}

// MANIPULATORS
int TcpTimerEventManager_ControlChannel::clientWrite(bool forceWrite)
{
    if (1 == ++d_numPendingRequests || forceWrite) {
        int errorNumber = 0;
        int rc;
        do {
            rc = btlso::SocketImpUtil::write(clientFd(),
                                             &d_byte,
                                             sizeof(char),
                                             &errorNumber);

            if (rc < 0 && btlso::SocketHandle::e_ERROR_INTERRUPTED != rc) {
                --d_numPendingRequests;
                return rc;                                            // RETURN
            }
        } while (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc);
        if (rc >= 0) {
            return rc;                                                // RETURN
        }
        bsl::printf("%s(%d): Failed to communicate request to control channel"
                    " (errno = %d, errorNumber = %d, rc = %d).\n",
                    __FILE__, __LINE__, errno, errorNumber, rc);
        BSLS_ASSERT(errorNumber > 0);
        return -errorNumber;                                          // RETURN
    }
    return 0;
}

int TcpTimerEventManager_ControlChannel::close()
{
    btlso::SocketImpUtil::close(clientFd());
    btlso::SocketImpUtil::close(serverFd());

    return 0;
}

int TcpTimerEventManager_ControlChannel::open()
{
    btlso::SocketHandle::Handle fds[2];

#ifdef BTLSO_PLATFORM_BSD_SOCKETS
    // Use UNIX domain sockets, if possible, rather than a standard socket
    // pair, to avoid using ephemeral ports for the control channel.  AIX and
    // Sun platforms have a more restrictive number of epheremal ports, and
    // several production machines have come close to that limit.  Note that
    // the posix standard 'AF_LOCAL', is not supported by a number of platforms
    // -- use the legacy identifier, 'AF_UNIX', instead.

    int rc = ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
#else
    int rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                        fds,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);

#endif

    if (rc) {
        d_fds[0] = static_cast<int>(
                                   btlso::SocketHandle::INVALID_SOCKET_HANDLE);
        d_fds[1] = static_cast<int>(
                                   btlso::SocketHandle::INVALID_SOCKET_HANDLE);

        bsl::printf("%s(%d): Failed to create control channel"
                    " (errno = %d, rc = %d).\n",
                    __FILE__, __LINE__, errno, rc);
        return rc;                                                    // RETURN
    }

    btlso::IoUtil::setBlockingMode(fds[1],
                                   btlso::IoUtil::e_NONBLOCKING,
                                   0);
    btlso::SocketOptUtil::setOption(fds[0],
                                    btlso::SocketOptUtil::k_TCPLEVEL,
                                    btlso::SocketOptUtil::k_TCPNODELAY,
                                    1);

    d_fds[0] = static_cast<int>(fds[0]);
    d_fds[1] = static_cast<int>(fds[1]);

    return 0;
}

int TcpTimerEventManager_ControlChannel::serverRead()
{
    int  rc = d_numPendingRequests.swap(0);
    char byte;

    const int numBytes = btlso::SocketImpUtil::read(&byte, serverFd(), 1);
    if (numBytes <= 0) {
        return -1;                                                    // RETURN
    }

    ++d_numServerReads;
    d_numServerBytesRead += numBytes;

    return rc;
}

                         // --------------------------
                         // class TcpTimerEventManager
                         // --------------------------

// PRIVATE METHODS
void TcpTimerEventManager::initialize()
{
    BSLS_ASSERT(d_allocator_p);

    btlso::TimeMetrics *metrics = d_collectMetrics ? &d_metrics : 0;

    // Initialize the (managed) event manager.
#ifdef BSLS_PLATFORM_OS_LINUX
    if (btlso::DefaultEventManager<>::isSupported()) {
        d_manager_p = new (*d_allocator_p)
                                   btlso::DefaultEventManager<>(metrics,
                                                                d_allocator_p);
    }
    else {
        d_manager_p = new (*d_allocator_p)
              btlso::DefaultEventManager<btlso::Platform::POLL>(metrics,
                                                                d_allocator_p);
    }
#else
    d_manager_p = new (*d_allocator_p)
                                   btlso::DefaultEventManager<>(metrics,
                                                                d_allocator_p);
#endif

    d_isManagedFlag = 1;

    // Initialize the functor containing the dispatch thread's entry point
    // method.

    d_dispatchThreadEntryPoint = bsl::function<void()>(
        bsl::allocator_arg_t(),
        bsl::allocator<bsl::function<void()> >(d_allocator_p),
        bdlf::MemFnUtil::memFn(&TcpTimerEventManager::dispatchThreadEntryPoint,
                               this));

    // Create the queue of executed timers.
    d_executeQueue_p = new (*d_allocator_p)
                            bsl::vector<bsl::function<void()> >(d_allocator_p);

    d_executeQueue_p->reserve(4);
}

int TcpTimerEventManager::initiateControlChannelRead()
{
    // Wait for the dispatcher thread to start and process
    // the request.

    bslmt::Mutex     mutex;
    bslmt::Condition condition;

    TcpTimerEventManager_Request *req =
        new (d_requestPool) TcpTimerEventManager_Request(
                                         TcpTimerEventManager_Request::e_NO_OP,
                                         &condition,
                                         &mutex,
                                         d_allocator_p);

    BSLS_ASSERT(-1 == req->result());
    bslmt::LockGuard<bslmt::Mutex> lock(&mutex);

    d_requestQueue.pushBack(req);
    int ret = d_controlChannel_p->clientWrite(true);

    if (0 > ret) {
        d_requestQueue.popBack();
        d_requestPool.deleteObjectRaw(req);
        return ret;                                                   // RETURN
    }
    req->waitForResult();
    d_requestPool.deleteObjectRaw(req);
    return 0;
}

void TcpTimerEventManager::controlCb()
    // At least one request is pending on the queue.  Process as many
    // as there are.
{
    int numRequests = d_controlChannel_p->serverRead();

    if (numRequests < 0) {
        const int rc = reinitializeControlChannel();
        BSLS_ASSERT_OPT(0 == rc);

        return;                                                       // RETURN
    }

    BSLS_ASSERT(0 <= numRequests);
    BSLS_ASSERT(numRequests <= d_requestQueue.length());

    for (int i = 0; i < numRequests; ++i) {
        BSLS_ASSERT_SAFE(d_requestQueue.length() > 0);

        TcpTimerEventManager_Request *req = d_requestQueue.popFront();

        switch (req->opCode()) {
          case TcpTimerEventManager_Request::e_TERMINATE: {
            BSLS_ASSERT(-1 == req->result());
            req->setResult(0);
            d_terminateThread = 1;
            req->signal();
          } break;
          case TcpTimerEventManager_Request::e_NO_OP: {
            BSLS_ASSERT(-1 == req->result());
            req->setResult(0);
            req->signal();
          } break;
          case TcpTimerEventManager_Request::e_REGISTER_SOCKET_EVENT: {
            d_manager_p->registerSocketEvent(req->socketHandle(),
                                             req->event(),
                                             req->callback());
            d_numTotalSocketEvents = d_manager_p->numEvents()-1;
            d_requestPool.deleteObjectRaw(req);
          } break;
          case TcpTimerEventManager_Request::e_EXECUTE:         // FALL THROUGH
          case TcpTimerEventManager_Request::e_REGISTER_TIMER: {
            BSLS_ASSERT(0 == req->timerId());
            d_requestPool.deleteObjectRaw(req);
          } break;
          case TcpTimerEventManager_Request::e_RESCHEDULE_TIMER: {
            BSLS_ASSERT(0 != req->timerId());
            d_requestPool.deleteObjectRaw(req);
          } break;
          case TcpTimerEventManager_Request::e_DEREGISTER_TIMER: {
            BSLS_ASSERT(0 != req->timerId());
            d_timerQueue.remove((int)(bsls::Types::IntPtr)req->timerId());
          } break;
          case TcpTimerEventManager_Request::e_DEREGISTER_ALL_TIMERS: {
            d_timerQueue.removeAll();
          } break;
          case TcpTimerEventManager_Request::e_DEREGISTER_ALL_SOCKET_EVENTS: {
            d_manager_p->deregisterAll();
          } break;
          case TcpTimerEventManager_Request::e_DEREGISTER_SOCKET_EVENT: {
            d_manager_p->deregisterSocketEvent(req->socketHandle(),
                                               req->event());
            d_numTotalSocketEvents = d_manager_p->numEvents()-1;
            d_requestPool.deleteObjectRaw(req);
          } break;
          case TcpTimerEventManager_Request::e_DEREGISTER_SOCKET: {
            d_manager_p->deregisterSocket(req->socketHandle());
            d_numTotalSocketEvents = d_manager_p->numEvents()-1;
            d_requestPool.deleteObjectRaw(req);
          } break;
          case TcpTimerEventManager_Request::e_NUM_SOCKET_EVENTS: {
            int result = d_manager_p->numSocketEvents(req->socketHandle());
            req->setResult(result);
            req->signal();
          } break;
          case TcpTimerEventManager_Request::e_IS_REGISTERED: {
            req->setResult(d_manager_p->isRegistered(req->socketHandle(),
                                                     req->event()));
            req->signal();
          } break;
          default: {
            BSLS_ASSERT("MUST BE UNREACHABLE BY DESIGN" && 0);
          }
        }
    }
}

void TcpTimerEventManager::dispatchThreadEntryPoint()
{
    if (d_collectMetrics) {
        d_metrics.switchTo(btlso::TimeMetrics::e_CPU_BOUND);
    }

    bsl::vector<bsl::function<void()> > *requestsPtr = new (*d_allocator_p)
                            bsl::vector<bsl::function<void()> >(d_allocator_p);

    requestsPtr->reserve(4);

    bslma::AutoRawDeleter<bsl::vector<bsl::function<void()> >,
                          bslma::Allocator> autoDelete(&requestsPtr,
                                                       d_allocator_p,
                                                       1);

    // Set the state to e_ENABLED before dispatching any events.  Note that the
    // thread calling 'enable' should be blocked (awaiting a response on the
    // control channel) and holding a write lock to 'd_stateLock'.

    d_state = e_ENABLED;

    while (1) {
        // Dispatch socket events, from shorter to longer timeout.
        int rc = 0;
        if (d_executeQueue_p->size()) {
            bsls::TimeInterval zeroTimeout;
            rc = d_manager_p->dispatch(zeroTimeout, 0);// non-blocking
        } else if (d_timerQueue.length()) {
            bsls::TimeInterval timeout;
            d_timerQueue.minTime(&timeout);
            rc = d_manager_p->dispatch(timeout, 0);    // blocking w/ timeout
        }
        else {
            rc = d_manager_p->dispatch(0);             // blocking indefinitely
        }

        if (rc < 0) {
            // Check if the control channel is still connected or not

            btlso::IPv4Address address;
            rc = btlso::SocketImpUtil::getPeerAddress(
                                               &address,
                                               d_controlChannel_p->serverFd());
            if (0 != rc) {
                rc = reinitializeControlChannel();
                BSLS_ASSERT_OPT(0 == rc);
            }
        }

        // Process executed callbacks (without timeouts), in respective order.
        {
            // A lock is necessary here, and not just an atomic pointer, since
            // we must guarantee that the pop_back in execute() must hold the
            // same functor that was enqueued, which would no longer be true if
            // swap below could occur between the push_back(functor) and
            // pop_back(functor).

            bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_executeQueueLock);

            BSLS_ASSERT(0 == requestsPtr->size());

            using bsl::swap;
            swap(d_executeQueue_p, requestsPtr);
        }

        int numCallbacks = requestsPtr->size();
        for (int i = 0; i < numCallbacks; ++i) {
            (*requestsPtr)[i]();
        }
        requestsPtr->clear();

        // Process expired timers in increasing time order.
        if (d_timerQueue.length()) {
            const int NUM_TIMERS = 32;
            const int SIZE = NUM_TIMERS *
                     sizeof(bdlcc::TimeQueueItem<bsl::function<void()> >);

            char BUFFER[SIZE];
            bdlma::BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

            bsl::vector<bdlcc::TimeQueueItem<bsl::function<void()> > >
                                                    requests(&bufferAllocator);
            d_timerQueue.popLE(bdlt::CurrentTime::now(), &requests);
            int numTimers = requests.size();
            for (int i = 0; i < numTimers; ++i) {
                requests[i].data()();
            }
        }

        // If a signal to quit has been issued leave immediately,
        // but only after processing expired callbacks (above).
        // This guarantees that memory associated with open channels
        // is deallocated.
        {
            if (d_terminateThread.loadRelaxed()) {  // it is volatile
                if (d_collectMetrics) {
                    d_metrics.switchTo(btlso::TimeMetrics::e_IO_BOUND);
                }
                BSLS_ASSERT(0 == d_requestQueue.queue().length());
                BSLS_ASSERT(e_ENABLED == d_state);
                return;                                               // RETURN
            }
        }
    }
    BSLS_ASSERT("MUST BE UNREACHABLE BY DESIGN." && 0);
}

int TcpTimerEventManager::reinitializeControlChannel()
{
    ++d_numControlChannelReinitializations;
    BSLS_ASSERT_OPT(d_numControlChannelReinitializations <= k_MAX_NUM_RETRIES);

    d_manager_p->deregisterSocket(d_controlChannel_p->serverFd());

    int rc = d_controlChannel_p->close();
    BSLS_ASSERT_OPT(0 == rc);

    rc = d_controlChannel_p->open();
    BSLS_ASSERT_OPT(0 == rc);

    // Register the server fd of 'd_controlChannel_p' for READs.
    btlso::EventManager::Callback cb(
               bsl::allocator_arg_t(),
               bsl::allocator<btlso::EventManager::Callback>(d_allocator_p),
               bdlf::MemFnUtil::memFn(&TcpTimerEventManager::controlCb, this));

    rc = d_manager_p->registerSocketEvent(d_controlChannel_p->serverFd(),
                                          btlso::EventType::e_READ,
                                          cb);
    if (rc) {
        printf("%s(%d): Failed to register controlChannel for READ events"
               " in TcpTimerEventManager constructor\n",
               __FILE__, __LINE__);
        BSLS_ASSERT("Failed to register controlChannel for READ events" &&
                    0);
        return rc;                                                    // RETURN
    }

    bslmt::ThreadUtil::Handle handle;
    bslmt::ThreadAttributes   attributes;
    attributes.setDetachedState(bslmt::ThreadAttributes::e_CREATE_DETACHED);

    bsl::function<void()> initiateReadFunctor(bdlf::BindUtil::bind(
                             &TcpTimerEventManager::initiateControlChannelRead,
                              this));

    rc = bslmt::ThreadUtil::create(&handle, attributes, initiateReadFunctor);
    BSLS_ASSERT_OPT(0 == rc);
    return 0;
}

// CREATORS
TcpTimerEventManager::TcpTimerEventManager(
                                         bslma::Allocator *threadSafeAllocator)
: d_requestPool(sizeof(TcpTimerEventManager_Request), threadSafeAllocator)
, d_requestQueue(threadSafeAllocator)
, d_dispatcher(bslmt::ThreadUtil::invalidHandle())
, d_state(e_DISABLED)
, d_terminateThread(0)
, d_timerQueue(threadSafeAllocator)
, d_metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
            btlso::TimeMetrics::e_IO_BOUND,
            threadSafeAllocator)
, d_collectMetrics(true)
, d_numTotalSocketEvents(0)
, d_numControlChannelReinitializations(0)
, d_allocator_p(bslma::Default::allocator(threadSafeAllocator))
{
    initialize();
}

TcpTimerEventManager::TcpTimerEventManager(
                                        bool               collectTimeMetrics,
                                        bslma::Allocator  *threadSafeAllocator)
: d_requestPool(sizeof(TcpTimerEventManager_Request), threadSafeAllocator)
, d_requestQueue(threadSafeAllocator)
, d_dispatcher(bslmt::ThreadUtil::invalidHandle())
, d_state(e_DISABLED)
, d_terminateThread(0)
, d_timerQueue(threadSafeAllocator)
, d_metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
            btlso::TimeMetrics::e_IO_BOUND,
            threadSafeAllocator)
, d_collectMetrics(collectTimeMetrics)
, d_numTotalSocketEvents(0)
, d_numControlChannelReinitializations(0)
, d_allocator_p(bslma::Default::allocator(threadSafeAllocator))
{
    initialize();
}

TcpTimerEventManager::TcpTimerEventManager(
                                        bool               collectTimeMetrics,
                                        bool               poolTimerMemory,
                                        bslma::Allocator  *threadSafeAllocator)
: d_requestPool(sizeof(TcpTimerEventManager_Request), threadSafeAllocator)
, d_requestQueue(threadSafeAllocator)
, d_dispatcher(bslmt::ThreadUtil::invalidHandle())
, d_state(e_DISABLED)
, d_terminateThread(0)
, d_timerQueue(poolTimerMemory, threadSafeAllocator)
, d_metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
            btlso::TimeMetrics::e_IO_BOUND,
            threadSafeAllocator)
, d_collectMetrics(collectTimeMetrics)
, d_numTotalSocketEvents(0)
, d_numControlChannelReinitializations(0)
, d_allocator_p(bslma::Default::allocator(threadSafeAllocator))
{
    initialize();
}

TcpTimerEventManager::TcpTimerEventManager(
                                      btlso::EventManager *rawEventManager,
                                      bslma::Allocator    *threadSafeAllocator)
: d_requestPool(sizeof(TcpTimerEventManager_Request), threadSafeAllocator)
, d_requestQueue(threadSafeAllocator)
, d_dispatcher(bslmt::ThreadUtil::invalidHandle())
, d_state(e_DISABLED)
, d_terminateThread(0)
, d_manager_p(rawEventManager)
, d_isManagedFlag(0)
, d_timerQueue(threadSafeAllocator)
, d_metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
            btlso::TimeMetrics::e_IO_BOUND,
            threadSafeAllocator)
, d_collectMetrics(false)
, d_numTotalSocketEvents(0)
, d_numControlChannelReinitializations(0)
, d_allocator_p(bslma::Default::allocator(threadSafeAllocator))
{
    BSLS_ASSERT(rawEventManager);

    // Initialize the functor containing the dispatch thread's entry point
    // method.

    d_dispatchThreadEntryPoint = bsl::function<void()>(
        bsl::allocator_arg_t(),
        bsl::allocator<bsl::function<void()> >(d_allocator_p),
        bdlf::MemFnUtil::memFn(&TcpTimerEventManager::dispatchThreadEntryPoint,
                               this));

    // Create the queue of executed timers.

    d_executeQueue_p = new (*d_allocator_p)
                            bsl::vector<bsl::function<void()> >(d_allocator_p);
}

TcpTimerEventManager::~TcpTimerEventManager()
{
    disable();
    bsl::vector<bsl::function<void()> > *executeQueue = d_executeQueue_p;
    d_allocator_p->deleteObjectRaw(executeQueue);
    if (d_isManagedFlag) {
        d_allocator_p->deleteObjectRaw(d_manager_p);
    }
}

// MANIPULATORS
int TcpTimerEventManager::disable()
{
    if (d_state == e_DISABLED) {
        return 0;                                                     // RETURN
    }

    if(bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        return 1;                                                     // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_stateLock);
    {
        // Synchronized section.
        if (d_state == e_DISABLED) {
            return 0;                                                 // RETURN
        }

        // Send dispatcher thread request to exit and wait until it
        // terminates, via 'join'.
        bslmt::Mutex              mutex;
        bslmt::Condition          condition;
        bslmt::ThreadUtil::Handle dispatcherHandle = d_dispatcher;

        TcpTimerEventManager_Request *req =
           new (d_requestPool) TcpTimerEventManager_Request(
                                     TcpTimerEventManager_Request::e_TERMINATE,
                                     &condition,
                                     &mutex,
                                     d_allocator_p);

        d_requestQueue.pushBack(req);

        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
            return -1;                                                // RETURN
        }

        // Note that for this function, the wait for result is subsumed
        // by joining with the thread.

        int rc = bslmt::ThreadUtil::join(dispatcherHandle);

        BSLS_ASSERT(0 == rc);
        d_requestPool.deleteObjectRaw(req);
        d_state = e_DISABLED;

        // Release the control channel object.

        BSLS_ASSERT(0 != d_controlChannel_p);
        d_manager_p->deregisterSocket(d_controlChannel_p->serverFd());
        d_controlChannel_p.clear();
    }
    return 0;
}

int TcpTimerEventManager::enable(const bslmt::ThreadAttributes& attr)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        return 0;                                                     // RETURN
    }

    if (e_ENABLED == d_state) {
        return 0;                                                     // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_stateLock);
    {
        // Synchronized section.
        if (e_ENABLED == d_state) {
            return 0;                                                 // RETURN
        }

        BSLS_ASSERT(0 == d_controlChannel_p);

        // Create control channel object.
        d_controlChannel_p.load(
                    new (*d_allocator_p) TcpTimerEventManager_ControlChannel(),
                    d_allocator_p);

        if (btlso::SocketHandle::INVALID_SOCKET_HANDLE ==
                                              d_controlChannel_p->serverFd()) {
            // Sockets were not successfully created.

            return -1;                                                // RETURN
        }


        // Register the server fd of 'd_controlChannel_p' for READs.
        btlso::EventManager::Callback cb(
               bsl::allocator_arg_t(),
               bsl::allocator<btlso::EventManager::Callback>(d_allocator_p),
               bdlf::MemFnUtil::memFn(&TcpTimerEventManager::controlCb, this));

        int rc = d_manager_p->registerSocketEvent(
                                                d_controlChannel_p->serverFd(),
                                                btlso::EventType::e_READ,
                                                cb);
        if (rc) {
            printf("%s(%d): Failed to register controlChannel for READ events"
                    " in TcpTimerEventManager constructor\n",
                    __FILE__, __LINE__);
            BSLS_ASSERT("Failed to register controlChannel for READ events" &&
                        0);
            return rc;                                                // RETURN
        }

#if defined(BSLS_PLATFORM_OS_UNIX)
        sigset_t newset, oldset;
        sigfillset(&newset);
        static const int synchronousSignals[] = {
            SIGBUS,
            SIGFPE,
            SIGILL,
            SIGSEGV,
            SIGSYS,
            SIGABRT,
            SIGTRAP,
        #if !defined(BSLS_PLATFORM_OS_CYGWIN) || defined(SIGIOT)
            SIGIOT
        #endif
         };

        static const int SIZE = sizeof synchronousSignals
                                                  / sizeof *synchronousSignals;
        for (int i = 0; i < SIZE; ++i) {
            sigdelset(&newset, synchronousSignals[i]);
        }

        pthread_sigmask(SIG_BLOCK, &newset, &oldset);
#endif

        d_terminateThread = 0;
        rc = bslmt::ThreadUtil::create(
                       static_cast<bslmt::ThreadUtil::Handle *>(&d_dispatcher),
                       attr,
                       d_dispatchThreadEntryPoint);

#if defined(BSLS_PLATFORM_OS_UNIX)
        // Restore the mask.
        pthread_sigmask(SIG_SETMASK, &oldset, &newset);
#endif
        if (rc) {
            return rc;                                                // RETURN
        }
        return initiateControlChannelRead();                          // RETURN
    }
    return 0;
}

int TcpTimerEventManager::registerSocketEvent(
                            const btlso::SocketHandle::Handle&        handle,
                            btlso::EventType::Type                    event,
                            const btlso::TimerEventManager::Callback& callback)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        int rc = d_manager_p->registerSocketEvent(handle, event, callback);
        d_numTotalSocketEvents = d_manager_p->numEvents()-1;
        return rc;                                                    // RETURN
    }

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        TcpTimerEventManager_Request *req =
            new (d_requestPool) TcpTimerEventManager_Request(handle,
                                                             event,
                                                             callback,
                                                             d_allocator_p);
        d_requestQueue.pushBack(req);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
            return -1;                                                // RETURN
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock
        // and process request in this thread.

        int rc = d_manager_p->registerSocketEvent(handle, event, callback);
        d_numTotalSocketEvents = d_manager_p->numEvents();
        return rc;                                                    // RETURN
      }
    }

    return 0;
}

void *TcpTimerEventManager::registerTimer(
                            const bsls::TimeInterval&                 timeout,
                            const btlso::TimerEventManager::Callback& callback)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {

        void *id = reinterpret_cast<void*>(d_timerQueue.add(timeout,
                                                            callback));
        return id;                                                    // RETURN
    }

    void *result = (void *)0;
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);
    {
        switch (d_state) {
          case e_ENABLED: {
            // As performance optimization, we may do the following: Time queue
            // is thread-safe.  Therefore, we don't have to synchronize the
            // following operation.

            int isNewTop = 0;
            int newLength;
            int handle = d_timerQueue.add(timeout,
                                          callback,
                                          &isNewTop,
                                          &newLength);

            if (!isNewTop) {
                result = reinterpret_cast<void*>(handle);
                BSLS_ASSERT(result);
            }
            else {
                // Signal dispatcher for the new minimum, if needed.

                TcpTimerEventManager_Request *req =
                    new (d_requestPool) TcpTimerEventManager_Request(
                                                                timeout,
                                                                callback,
                                                                d_allocator_p);
                d_requestQueue.pushBack(req);
                if (0 > d_controlChannel_p->clientWrite()) {
                    d_requestQueue.popBack();
                    d_requestPool.deleteObjectRaw(req);
                    d_timerQueue.remove(handle);
                    result = (void*)0;
                    BSLS_ASSERT("Failed to register timer" && result);
                }
                else {
                    result = reinterpret_cast<void*>(handle);
                    BSLS_ASSERT(result);
                }
            }
          } break;
          case e_DISABLED: {
            // Processing thread is disabled -- register directly
            // since the timer queue is thread-safe.

            int newTop, newLength = 0;
            result = reinterpret_cast<void *>(d_timerQueue.add(timeout,
                                                               callback,
                                                               &newTop,
                                                               &newLength));
            BSLS_ASSERT(result);
          } break;
        }
    }
    return result;
}

int TcpTimerEventManager::rescheduleTimer(const void                *id,
                                          const bsls::TimeInterval&  timeout)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        return d_timerQueue.update(
                   static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(id)),
                   timeout);
                                                                      // RETURN
    }

    int rc;
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);
    {
        switch (d_state) {
          case e_ENABLED: {
            // As performance optimization, we may do the following: Time queue
            // is thread-safe.  Therefore, we don't have to synchronize the
            // following operation.

            int isNewTop = 0;
            rc = d_timerQueue.update(
                   static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(id)),
                   timeout,
                   &isNewTop);

            if (!rc && isNewTop) {
                // Signal dispatcher for the new minimum, if needed.

                TcpTimerEventManager_Request *req =
                    new (d_requestPool) TcpTimerEventManager_Request(
                                                                id,
                                                                timeout,
                                                                d_allocator_p);

                d_requestQueue.pushBack(req);
                if (0 > d_controlChannel_p->clientWrite()) {
                    d_requestQueue.popBack();
                    d_requestPool.deleteObjectRaw(req);
                    d_timerQueue.remove(
                  static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(id)));
                    BSLS_ASSERT("Failed to reschedule timer" && 0);
                }
            }
          } break;
          case e_DISABLED: {
            // Processing thread is disabled -- register directly
            // since the timer queue is thread-safe.

            rc = d_timerQueue.update(
                   static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(id)),
                   timeout);
          } break;
        }
    }
    return rc;
}

void TcpTimerEventManager::deregisterSocketEvent(
                                     const btlso::SocketHandle::Handle& handle,
                                     btlso::EventType::Type             event)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        d_manager_p->deregisterSocketEvent(handle, event);
        d_numTotalSocketEvents = d_manager_p->numEvents() - 1;
        return;                                                       // RETURN
    }

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        TcpTimerEventManager_Request *req =
            new (d_requestPool) TcpTimerEventManager_Request(handle,
                                                             event,
                                                             d_allocator_p);

        d_requestQueue.pushBack(req);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock and process
        // request in this thread.

        d_manager_p->deregisterSocketEvent(handle, event);

        // When disabled, the control channel object is destroyed, no need to
        // minus one from 'numEvents()'.

        d_numTotalSocketEvents = d_manager_p->numEvents();
        return;                                                       // RETURN
      }
    }
}

void TcpTimerEventManager::execute(const bsl::function<void()>& functor)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_executeQueueLock);
        d_executeQueue_p->push_back(functor);
        return;                                                       // RETURN
    }

    bslmt::ReadLockGuard<bslmt::RWMutex> stateLockGuard(&d_stateLock);
    bslmt::LockGuard<bslmt::Mutex> executeQueueGuard(&d_executeQueueLock);

    switch (d_state) {
      case e_ENABLED: {

        // Processing thread is enabled -- enqueue the request.
        d_executeQueue_p->push_back(functor);

        // Signal dispatcher for the new executed events, only if it has not
        // been signaled already.

        if (1 == d_executeQueue_p->size()) {

            TcpTimerEventManager_Request *req =
                new (d_requestPool) TcpTimerEventManager_Request(
                                                                functor,
                                                                d_allocator_p);

            d_requestQueue.pushBack(req);
            if (0 > d_controlChannel_p->clientWrite()) {
                d_requestQueue.popBack();
                d_requestPool.deleteObjectRaw(req);
                d_executeQueue_p->pop_back();

                // guaranteed to be 'functor'

                BSLS_ASSERT("Failed to execute functor" && 0);
            }
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- Simply enqueue the request.

        d_executeQueue_p->push_back(functor);
      }
    }
}

void TcpTimerEventManager::clearExecuteQueue()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_executeQueueLock);
    d_executeQueue_p->clear();
}

void TcpTimerEventManager::deregisterSocket(
                                     const btlso::SocketHandle::Handle& handle)
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        d_manager_p->deregisterSocket(handle);
        d_numTotalSocketEvents = d_manager_p->numEvents()-1;
        return;                                                       // RETURN
    }

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        TcpTimerEventManager_Request *req =
            new (d_requestPool) TcpTimerEventManager_Request(handle,
                                                             d_allocator_p);
        d_requestQueue.pushBack(req);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock
        // and process request in this thread.

        d_manager_p->deregisterSocket(handle);

        // When disabled, the control channel object is destroyed, no need
        // to minus one from 'numEvents()'.

        d_numTotalSocketEvents = d_manager_p->numEvents();
      }
    }
}

void TcpTimerEventManager::deregisterAllSocketEvents()
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        d_manager_p->deregisterAll();
        d_numTotalSocketEvents = 0;
        return;                                                       // RETURN
    }

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        TcpTimerEventManager_Request *req =
           new (d_requestPool) TcpTimerEventManager_Request(
                  TcpTimerEventManager_Request::e_DEREGISTER_ALL_SOCKET_EVENTS,
                  d_allocator_p);

        d_requestQueue.pushBack(req);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock
        // and process request in this thread.

        d_manager_p->deregisterAll();
        d_numTotalSocketEvents = 0;
      }
    }
}

void TcpTimerEventManager::deregisterTimer(const void *id)
{
    // We can just remove it.  If its at the top, dispatcher will
    // pick a new top on the next iteration.

    d_timerQueue.remove(static_cast<int>(
                                   reinterpret_cast<bsls::Types::IntPtr>(id)));
}

void TcpTimerEventManager::deregisterAllTimers()
{
    d_timerQueue.removeAll();
}

void TcpTimerEventManager::deregisterAll()
{
    deregisterAllTimers();
    deregisterAllSocketEvents();
}

// ACCESSORS
int TcpTimerEventManager::isRegistered(
                                const btlso::SocketHandle::Handle& handle,
                                btlso::EventType::Type             event) const
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        return d_manager_p->isRegistered(handle, event);              // RETURN
    }

    int result;

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        bslmt::Mutex                  mutex;
        bslmt::Condition              condition;
        TcpTimerEventManager_Request *req =
               new (d_requestPool) TcpTimerEventManager_Request(handle,
                                                                event,
                                                                &condition,
                                                                &mutex,
                                                                d_allocator_p);
        d_requestQueue.pushBack(req);
        bslmt::LockGuard<bslmt::Mutex> lock(&mutex);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
            result = -1;
        }
        else {
            req->waitForResult();
            result = req->result();
            d_requestPool.deleteObjectRaw(req);
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock
        // and process request in this thread.

        result = d_manager_p->isRegistered(handle, event);
      }
    }

    return result;
}

int TcpTimerEventManager::numEvents() const
{
    return numTimers() + numTotalSocketEvents();
}

int TcpTimerEventManager::numTimers() const
{
    return d_timerQueue.length();
}

int TcpTimerEventManager::numSocketEvents(
                               const btlso::SocketHandle::Handle& handle) const
{
    if (bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(), d_dispatcher)) {
        return d_manager_p->numSocketEvents(handle);                  // RETURN
    }
    int result;

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_stateLock);

    if (e_DISABLED == d_state) {
        d_stateLock.unlock();
        d_stateLock.lockWrite();
    }

    switch (d_state) {
      case e_ENABLED: {
        // Processing thread is enabled -- enqueue the request.

        bslmt::Mutex mutex;
        bslmt::Condition condition;
        TcpTimerEventManager_Request *req =
            new (d_requestPool) TcpTimerEventManager_Request(handle,
                                                             &condition,
                                                             &mutex,
                                                             d_allocator_p);
        d_requestQueue.pushBack(req);
        bslmt::LockGuard<bslmt::Mutex> lock(&mutex);
        if (0 > d_controlChannel_p->clientWrite()) {
            d_requestQueue.popBack();
            d_requestPool.deleteObjectRaw(req);
            result = -1;
        }
        else {
            req->waitForResult();
            result = req->result();
            d_requestPool.deleteObjectRaw(req);
        }
      } break;
      case e_DISABLED: {
        // Processing thread is disabled -- upgrade to write lock
        // and process request in this thread.

        result = d_manager_p->numSocketEvents(handle);
      }
    }

    return result;
}

int TcpTimerEventManager::numTotalSocketEvents() const
{
    return d_numTotalSocketEvents;
}

int TcpTimerEventManager::isEnabled() const
{
    return d_state == e_ENABLED; // d_state is volatile

/*
    bslmt::LockGuard<bslmt::Mutex> lock(&d_cs);
    return d_dispatcher != bslmt::ThreadUtil::invalidHandle();
*/

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
