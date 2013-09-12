// btes5_negotiator.cpp               -*-C++-*-
#include <btes5_negotiator.h>

// btes5_testserver is used in the test driver
#include <btes5_testserver.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_sharedptr.h>
#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>
#include <bdetu_systemtime.h>
#include <bdeut_bigendian.h>
#include <bdeut_stringref.h>
#include <bsl_iostream.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bteso_eventmanager.h>
#include <bteso_resolveutil.h>

namespace BloombergLP {

namespace {

// SOCKS5 protocol messages as defined in RFCs 1928 and 1929.
enum Socks5 {
    VERSION = 0x05,
    VERSION_USERNAME_PASSWORD_AUTH = 0x01
};

enum Authentication {
    NONE         = 0x00
  , PASSWORD     = 0x02
  , UNACCEPTABLE = 0xFF
};

enum CommandCode {
    TCP_STREAM_CONNECTION = 0x01
  , TCP_PORT_BINDING      = 0x02
  , UDP_PORT              = 0x03
};

enum ATYP {
    IPv4ADDRESS = 0x01
  , DOMAINNAME  = 0x03
  , IPv6ADDRESS = 0x04
};

enum StatusCode {
    REQUEST_GRANTED            = 0x00
  , GENERAL_FAILURE            = 0x01
  , CONNECTION_NOT_ALLOWED     = 0x02
  , NETWORK_UNREACHABLE        = 0x03
  , HOST_UNREACHABLE           = 0x04
  , CONNECTION_REFUSED         = 0x05
  , TTL_EXPIRED                = 0x06
  , CMD_PROTO_NOT_SUPPORTED    = 0x07
  , ADDRESS_TYPE_NOT_SUPPORTED = 0x08
};

struct MethodRequestPkt {
    unsigned char d_ver;
    unsigned char d_nmethods;
    unsigned char d_methods[2];
};

struct MethodResponsePkt {
    unsigned char d_ver;
    unsigned char d_method;
};

struct AuthenticationResponsePkt {
    unsigned char d_ver;
    unsigned char d_status;
};

struct ConnectBase {
    unsigned char d_ver;
    unsigned char d_cmd;
    unsigned char d_rsv;
    unsigned char d_atype;
};

struct ConnectRspBase {
    unsigned char d_ver;
    unsigned char d_rep;
    unsigned char d_rsv;
    unsigned char d_atype;
};

                              // ==================
                              // struct Negotiation
                              // ==================
struct Negotiation {
    // Objects describing the state of a SOCKS connection negotiation. The
    // object lifetime is managed by using a 'bcema_SharedPtr<Negotiation>' in
    // callback registration. A 'Negotiation' object is in one of two states:
    // it's created in the normal state  indicated by 'd_terminating == 0', and
    // it enters a terminating state when 'd_terminating != 0'.

    // TYPES
    typedef bcema_SharedPtr<Negotiation> Context;
        // A shared pointer of this type is bound into the callback functors
        // for object lifetime management.

    // DATA
    btes5_Credentials          d_credentials; // SOCKS5 credentials
    btes5_CredentialsProvider *d_provider_p; // provider
        // A negotiation request may have no credentials, pre-defined
        // credentials ('d_credentials.isSet()') or 'd_provider_p' supplied.

    bteso_Endpoint                             d_destination;
    bteso_StreamSocket<bteso_IPv4Address>     *d_socket_p; // proxy socket
    bteso_SocketHandle::Handle                 d_handle;   // OS-level socket
    btes5_Negotiator::NegotiationStateCallback d_callback; // result callback

    bool                                       d_acquiringCredentials;
        // 'true' iff 'd_provider_p->acquireCredentials()' has been invoked but
        // associated callback 'setCredentials()' has not; and the acquisitions
        // has not been stopped by
        // 'd_provider_p->cancelAcquiringCredentials()'.

    bteso_TimerEventManager                   *d_eventManager_p;
        // asynchronous event registration manager, not owned

    bdet_TimeInterval      d_timeout;   // timeout
    void                  *d_timer;     // expiration timer
    bcemt_Mutex            d_timerLock; // 'd_timer' access

    bsls::AtomicInt                            d_terminating;
        // negotiation being terminated

    bslma::Allocator                          *d_allocator_p;
        // memory allocator, not owned

    // CREATORS
    Negotiation(bteso_StreamSocket<bteso_IPv4Address>     *socket,
                const bteso_Endpoint&                      destination,
                btes5_Negotiator::NegotiationStateCallback callback,
                const bdet_TimeInterval&                   timeout,
                bteso_TimerEventManager                   *eventManager,
                bslma::Allocator                          *allocator);
        // Create a 'Negotiation' object to connect to the specified
        // 'destination' over the specified 'socket' and asynchrounously invoke
        // the specified 'callback', using the specified 'eventManager' to
        // schedule callbacks and 'allocator' to supply memory.  If the
        // specified 'timeout' is not empty a successful negotiation must
        // complete within this time period.

    ~Negotiation();
        // Destroy this object.

};

                             // ------------------
                             // struct Negotiation
                             // ------------------
// CREATORS
Negotiation::Negotiation(
    bteso_StreamSocket<bteso_IPv4Address>      *socket,
    const bteso_Endpoint&                      destination,
    btes5_Negotiator::NegotiationStateCallback callback,
    const bdet_TimeInterval&                    timeout,
    bteso_TimerEventManager                    *eventManager,
    bslma::Allocator                           *allocator)
: d_credentials(allocator)
, d_provider_p(0)
, d_destination(destination, allocator)
, d_socket_p(socket)
, d_handle(socket->handle())
, d_callback(callback, allocator)
, d_eventManager_p(eventManager)
, d_timeout(timeout)
, d_timer(0)
, d_allocator_p(allocator)
{
}

Negotiation::~Negotiation()
{
    // clean up in case 'terminate' was called prior to registering timer
    if (d_timer) {
        d_eventManager_p->deregisterTimer(d_timer);
    }
}

static void terminate(Negotiation::Context                negotiation,
                      btes5_Negotiator::NegotiationStatus status,
                      const btes5_DetailedError&          error)
    // Terminate the specified 'negotiation', and invoke the user-supplied
    // callback with the specified 'status' and 'error'.
{
    if (negotiation->d_terminating.testAndSwap(0, 1)) {
        return;  // this negotiation is already being terminated
    }

    negotiation->d_eventManager_p->deregisterSocket(negotiation->d_handle);

    {
        bcemt_LockGuard<bcemt_Mutex> lock(&negotiation->d_timerLock);
        if (negotiation->d_timer) {
            negotiation->d_eventManager_p->deregisterTimer(
                                                         negotiation->d_timer);
        }
    }

    negotiation->d_callback(status, error);
}

static int registerReadCb(void (*cb) (Negotiation::Context),
                          Negotiation::Context     negotiation)
    // Register the specified 'cb' to be invoked with the argument of the
    // specified 'negotiation' on read events on 'negotiation->d_handle'.
    // Return 0 on success and a negative value otherwise.
{
    bteso_EventManager::Callback
        readCb = bdef_BindUtil::bind(cb, negotiation);
    int rc = negotiation->d_eventManager_p->registerSocketEvent(
                                                   negotiation->d_handle,
                                                   bteso_EventType::READ,
                                                   readCb);
    if (rc < 0) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error registering read handler"));
        return btes5_Negotiator::e_ERROR;
    }
    return 0;
}

static void connectCallback(Negotiation::Context negotiation)
    // Process response from the SOCKS5 connect request for the specified
    // 'negotiation'.
{
    bsl::ostringstream e("connect response: ", negotiation->d_allocator_p);

    ConnectRspBase hdr;
    int rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&hdr),
                                           sizeof(hdr));
    if (sizeof(hdr) != rc) {
        e << "error reading: " << rc;
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    if (hdr.d_ver != VERSION) {
        e << "invalid SOCK version, expected " << VERSION
          << " got " << hdr.d_ver;
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    int addressLength; // bytes to read for the bound address
    if (hdr.d_atype == IPv4ADDRESS) {
        addressLength = 4;
    } else if (hdr.d_atype == IPv6ADDRESS) {
        addressLength = 16;
    } else if (hdr.d_atype == DOMAINNAME) {
        unsigned char length = 0;
        rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&length),
                                           1);
        if (1 != rc) {
            e << "error reading domainname length: " << rc;
            terminate(negotiation,
                      btes5_Negotiator::e_ERROR,
                      btes5_DetailedError(e.str()));
            return;
        }
        addressLength = length;
    } else {
        e << "received invalid address type: " << hdr.d_atype;
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    // consume address and port (not presently used)
    char buf[260]; // maximum length of address + 2 is 257
    BSLS_ASSERT(addressLength + 2 <= sizeof(buf));
    rc = negotiation->d_socket_p->read(buf, addressLength + 2);
    if (addressLength + 2 != rc) {
        e << "error reading bound address, expected " << addressLength + 2
          << " got " << rc;
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError(e.str()));
        return;
    }

    switch (hdr.d_rep) {
      case REQUEST_GRANTED: {
        e << "request granted";
      } break;
      case GENERAL_FAILURE: {
        e << "status: general failure";
      } break;
      case CONNECTION_NOT_ALLOWED: {
        e << "status: connection not allowed";
      } break;
      case NETWORK_UNREACHABLE: {
        e << "status: network unreachable";
      } break;
      case HOST_UNREACHABLE: {
        e << "status: host unreachable";
      } break;
      case CONNECTION_REFUSED: {
        e << "status: connection refused";
      } break;
      case TTL_EXPIRED: {
        e << "time to live expired";
      } break;
      case CMD_PROTO_NOT_SUPPORTED: {
        e << "status: cmd protocol not supported";
       } break;
      case ADDRESS_TYPE_NOT_SUPPORTED: {
        e << "status: cmd protocol not supported";
      } break;
      default: {
        e << "status: unknown = " << hdr.d_rep;
      } break;
    }
    terminate(negotiation,
              REQUEST_GRANTED == hdr.d_rep ? btes5_Negotiator::e_SUCCESS
                                           : btes5_Negotiator::e_ERROR,
              btes5_DetailedError(e.str()));
}

static void connectToEndpoint(Negotiation::Context negotiation)
    // Send the appropriate request to connect to the endpoint past the SOCKS
    // server.
{
    bsl::ostringstream request;
    unsigned char buffer;

    buffer = VERSION;
    request << buffer;

    buffer = TCP_STREAM_CONNECTION;
    request << buffer;

    buffer = 0x00; // reserved
    request << buffer;

    buffer = DOMAINNAME; // adress type
    request << buffer;

    int length = negotiation->d_destination.hostname().size();
    buffer = length;
    request << buffer;
    request << negotiation->d_destination.hostname();

    // encode 2-byte port in network (bigendian) order
    buffer = (negotiation->d_destination.port() >> 8) & 0xff; // MSB
    request << buffer;
    buffer = negotiation->d_destination.port() & 0xff; // LSB
    request << buffer;

    if (registerReadCb(connectCallback, negotiation)) {
        return;
    }
    const bsl::string& buf = request.str();
    int rc = negotiation->d_socket_p->write(buf.c_str(), buf.size());
    if (rc != buf.size()) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing connection request"));
        return;
    }
}

static void sendAuthenticationRequest(Negotiation::Context negotiation);
    // Send the username and password credentials to authenticate with the
    // SOCKS5 server.

static void setCredentials(int                      status,
                           const bslstl::StringRef& username,
                           const bslstl::StringRef& password,
                           Negotiation::Context     negotiation)
    // Set user name and password to authenticate the client to the SOCKS5
    // server.
{
    negotiation->d_acquiringCredentials = false;
    if (status) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error acquiring credentials"));
    } else {
        negotiation->d_credentials.set(username, password);
        sendAuthenticationRequest(negotiation);
    }
}

static void authenticationCallback(Negotiation::Context negotiation)
{
    AuthenticationResponsePkt pkt;
    int rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&pkt),
                                           sizeof(pkt));
    if (sizeof(pkt) != rc) {
        if (negotiation->d_acquiringCredentials) {
            negotiation->d_provider_p->cancelAcquiringCredentials();
        }
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error reading auth. response"));
        return;
    }

    if (pkt.d_status) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("authentication rejected"));
        return;
    }

    connectToEndpoint(negotiation);
}

static void sendAuthenticationRequest(Negotiation::Context negotiation)
{
    if (!negotiation->d_credentials.isSet() && negotiation->d_provider_p) {
        negotiation->d_acquiringCredentials = true;

        // asynchronously acquire the username/password to use
        using namespace bdef_PlaceHolders;
        negotiation->d_provider_p->acquireCredentials(
            negotiation->d_destination,
            bdef_BindUtil::bind(setCredentials,
                                _1,
                                _2,
                                _3,
                                negotiation));
        return;
    }

    // The Username/Password request (RFC 1929) format is:
    // +----+------+----------+------+----------+
    // |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
    // +----+------+----------+------+----------+
    // | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
    // +----+------+----------+------+----------+

    bsl::ostringstream request;
    request << (unsigned char) VERSION_USERNAME_PASSWORD_AUTH
            << (unsigned char) negotiation->d_credentials.username().size()
            << negotiation->d_credentials.username()
            << (unsigned char) negotiation->d_credentials.password().size()
            << negotiation->d_credentials.password();

    if (registerReadCb(authenticationCallback, negotiation)) {
        return;
    }
    const bsl::string& buf = request.str();
    int rc = negotiation->d_socket_p->write(buf.c_str(), buf.size());
    if (rc != buf.size()) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing username/password"));
        return;
    }
}

static void methodCallback(Negotiation::Context negotiation)
    // Process SOCKS5 Method Selection response.
{
    MethodResponsePkt pkt;
    int rc = negotiation->d_socket_p->read(
                                  reinterpret_cast<char *>(&pkt),
                                  sizeof(pkt));
    if (sizeof(pkt) != rc) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error reading method response"));
        return;
    }

    switch (pkt.d_method) {
      case NONE: {
        connectToEndpoint(negotiation);
      } break;
      case PASSWORD: {
        if (!negotiation->d_credentials.isSet() && !negotiation->d_provider_p)
        {
            terminate(negotiation,
                      btes5_Negotiator::e_ERROR,
                      btes5_DetailedError(
                               "Got authentication request when we did not"
                               " offer authentication as an option, closing"));
            return;
        }
        sendAuthenticationRequest(negotiation);
      } break;
      case UNACCEPTABLE: {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError(
                    "proxy server rejected all authentication methods"));
      } break;
      default: {
        bsl::ostringstream description;
        description << "unknown response "
                    << (int) pkt.d_method
                    << " from proxy server";
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError(description.str()));
      } break;
    }
}

static void timeoutCallback(Negotiation::Context negotiation)
    // Expire the specified 'negotiation'.
{
    negotiation->d_timer = 0;
    terminate(negotiation,
              btes5_Negotiator::e_ERROR, btes5_DetailedError("timeout"));
}

static int sendMethodRequest(Negotiation::Context negotiation)
    // Send SOCKS5 greeting including list of authentication methods supported
    // using the specified 'negotiation' for managing object lifetime. Return 0
    // for success and a non-zero value for error.
{
    MethodRequestPkt pkt;
    pkt.d_ver        = VERSION;
    pkt.d_nmethods   = 2;
    pkt.d_methods[0] = NONE;
    pkt.d_methods[1] = PASSWORD;

    int length = sizeof(pkt);
    if (!negotiation->d_credentials.isSet() && !negotiation->d_provider_p) {
        // Don't offer PASSWORD authentication unless the we have predefined or
        // dynamic way to get it.
        --pkt.d_nmethods;
        length -= sizeof(pkt.d_methods[1]);
    }

    if (registerReadCb(methodCallback, negotiation)) {
        return btes5_Negotiator::e_ERROR;
    }

    if (bdet_TimeInterval() != negotiation->d_timeout) {
        bdet_TimeInterval expiration
            = bdetu_SystemTime::now() + negotiation->d_timeout;
        bteso_EventManager::Callback
           cb = bdef_BindUtil::bind(timeoutCallback, negotiation);
        {
            bcemt_LockGuard<bcemt_Mutex> lock(&negotiation->d_timerLock);
            negotiation->d_timer
                = negotiation->d_eventManager_p->registerTimer(expiration, cb);
        }
    }

    int rc = negotiation->d_socket_p->write(
                                 reinterpret_cast<const char *>(&pkt),
                                 length);
    if (length != rc) {
        terminate(negotiation,
                  btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing method request"));
        return btes5_Negotiator::e_ERROR;
    }

    return 0;
}

}  // close unnamed namespace

                           // ----------------------
                           // class btes5_Negotiator
                           // ----------------------
// CREATORS
btes5_Negotiator::btes5_Negotiator(bteso_TimerEventManager *eventManager,
                                   bslma::Allocator        *allocator)
: d_eventManager_p(eventManager)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

btes5_Negotiator::~btes5_Negotiator()
{
}

// MANIPULATORS
int btes5_Negotiator::negotiate(
    bteso_StreamSocket<bteso_IPv4Address> *socket,
    const bteso_Endpoint&                  destination,
    NegotiationStateCallback               callback,
    const bdet_TimeInterval&               timeout)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     timeout,
                                                     d_eventManager_p,
                                                     d_allocator_p),
                        d_allocator_p);
    return sendMethodRequest(negotiation);
}

int btes5_Negotiator::negotiate(
    bteso_StreamSocket<bteso_IPv4Address> *socket,
    const bteso_Endpoint&                  destination,
    NegotiationStateCallback               callback,
    const bdet_TimeInterval&               timeout,
    const btes5_Credentials&               credentials)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     timeout,
                                                     d_eventManager_p,
                                                     d_allocator_p),
                        d_allocator_p);
    negotiation->d_credentials = credentials;
    return sendMethodRequest(negotiation);
}

int btes5_Negotiator::negotiate(
    bteso_StreamSocket<bteso_IPv4Address> *socket,
    const bteso_Endpoint&                  destination,
    NegotiationStateCallback               callback,
    const bdet_TimeInterval&               timeout,
    btes5_CredentialsProvider             *provider)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     timeout,
                                                     d_eventManager_p,
                                                     d_allocator_p),
                        d_allocator_p);
    negotiation->d_provider_p = provider;
    return sendMethodRequest(negotiation);
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
