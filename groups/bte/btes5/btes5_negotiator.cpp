// btes5_negotiator.cpp               -*-C++-*-
#include <btes5_negotiator.h>

// btes5_testserver is used in the test driver
#include <btes5_testserver.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_sharedptr.h>
#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>
#include <bdeut_bigendian.h>
#include <bdeut_stringref.h>
#include <bsl_iostream.h>
#include <bslma_default.h>
#include <bsls_assert.h>
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
    // callback registration.

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

    bslma::Allocator                          *d_allocator_p;
        // memory allocator, not owned

    // CREATORS
    Negotiation(bteso_StreamSocket<bteso_IPv4Address>     *socket,
                const bteso_Endpoint&                      destination,
                btes5_Negotiator::NegotiationStateCallback callback,
                bteso_TimerEventManager                   *eventManager,
                bslma::Allocator                          *allocator);
        // Create a 'Negotiation' object to connect to the specified
        // 'destination' over the specified 'socket' and asynchrounously invoke
        // the specified 'callback', using the specified 'eventManager' to
        // schedule callbacks and 'allocator' to supply memory.

    ~Negotiation();
        // Destroy this object.

    // MANIPULATORS
    int sendMethodRequest(Context negotiation);
        // Send SOCKS5 greeting including list of authentication methods
        // supported using the specified 'negotiation' for managing object
        // lifetime. Return 0 for success and a non-zero value for error.

    void methodCallback(Context negotiation);
        // Process SOCKS5 Method Selection response.

    void authenticationCallback(Context negotiation);

    void connectCallback(Context negotiation);

    void sendAuthenticationRequest(Context negotiation);
        // Send the username and password credentials to authenticate with the
        // SOCKS5 server.

    void connectToEndpoint(Context negotiation);
        // Send the appropriate request to connect to the endpoint past the
        // SOCKS server.

    int sendConnectionRequest(Context negotiation);
        // Send connection request to establish external connection to 'd_host'
        // on port 'd_port'.

    int registerReadCb(void (Negotiation::*cb) (Context), Context negotiation);
        // Register the specified 'cb' to be invoked with the argument of the
        // specified 'negotiation' on read events on 'd_handle'. Return 0 on
        // success and a negative value otherwise.

    void terminate(btes5_Negotiator::NegotiationStatus status,
                   const btes5_DetailedError&          error);
        // Terminate current negotiation session, and invoke the user-supplied
        // callback with the specified 'status' and 'error'. If the specified
        // 'status' is not 0, 'd_socket_p' will be closed.

    void setCredentials(int                      status,
                        const bslstl::StringRef& username,
                        const bslstl::StringRef& password,
                        Context                  negotiation);
        // Set user name and password to authenticate the client to the SOCKS5
        // server.
};

                             // ------------------
                             // struct Negotiation
                             // ------------------
// CREATORS
Negotiation::Negotiation(
    bteso_StreamSocket<bteso_IPv4Address>      *socket,
    const bteso_Endpoint&                      destination,
    btes5_Negotiator::NegotiationStateCallback callback,
    bteso_TimerEventManager                    *eventManager,
    bslma::Allocator                           *allocator)
: d_credentials(allocator)
, d_provider_p(0)
, d_destination(destination, allocator)
, d_socket_p(socket)
, d_handle(socket->handle())
, d_callback(callback, allocator)
, d_eventManager_p(eventManager)
, d_allocator_p(allocator)
{
}

Negotiation::~Negotiation()
{
}

// MANIPULATORS
void Negotiation::terminate(btes5_Negotiator::NegotiationStatus status,
                            const btes5_DetailedError&          error)
{
    // TODO: take this out
    bsl::cout << "Negotiation::terminate" << bsl::endl;
    d_eventManager_p->deregisterSocket(d_handle);
    if (status) {
        d_socket_p->shutdown(bteso_Flag::SHUTDOWN_BOTH);
    }
    d_callback(status, error);
}

int Negotiation::registerReadCb(void (Negotiation::*cb) (Context),
                                Context             negotiation)
{
    bteso_EventManager::Callback
        readCb = bdef_BindUtil::bind(cb, negotiation, negotiation);
    int rc = d_eventManager_p->registerSocketEvent(d_handle,
                                                   bteso_EventType::READ,
                                                   readCb);
    if (rc < 0) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error registering read handler"));
        return -1;
    }
    return 0;
}

int Negotiation::sendMethodRequest(Context negotiation)
{
    MethodRequestPkt pkt;
    pkt.d_ver        = VERSION;
    pkt.d_nmethods   = 2;
    pkt.d_methods[0] = NONE;
    pkt.d_methods[1] = PASSWORD;

    int length = sizeof(pkt);
    if (!d_credentials.isSet() && !d_provider_p) {
        // Don't offer PASSWORD authentication unless the we have predefined or
        // dynamic way to get it.
        --pkt.d_nmethods;
        length -= sizeof(pkt.d_methods[1]);
    }

    if (registerReadCb(&Negotiation::methodCallback, negotiation)) {
        return -1;
    }

    int rc = d_socket_p->write(reinterpret_cast<const char *>(&pkt), length);
    if (length != rc) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing method request"));
        return -2;
    }

    return 0;
}

void Negotiation::methodCallback(Context negotiation)
{
    MethodResponsePkt pkt;
    int rc = d_socket_p->read(reinterpret_cast<char *>(&pkt), sizeof(pkt));
    if (sizeof(pkt) != rc) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error reading method request"));
        return;
    }

    switch (pkt.d_method) {
      case NONE: {
        connectToEndpoint(negotiation);
      } break;
      case PASSWORD: {
        if (!d_credentials.isSet() && !d_provider_p) {
            terminate(btes5_Negotiator::e_ERROR,
                      btes5_DetailedError(
                               "Got authentication request when we did not"
                               " offer authentication as an option, closing"));
            return;
        }
        sendAuthenticationRequest(negotiation);
      } break;
      case UNACCEPTABLE: {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError(
                    "proxy server rejected all authentication methods"));
      } break;
      default: {
        bsl::ostringstream description;
        description << "unknown response from proxy server " << pkt.d_method;
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError(description.str()));
      } break;
    }
}

void Negotiation::sendAuthenticationRequest(Context negotiation)
{
    if (!d_credentials.isSet() && d_provider_p) {
        d_acquiringCredentials = true;

        // asynchronously acquire the username/password to use
        using namespace bdef_PlaceHolders;
        d_provider_p->acquireCredentials(
            d_destination,
            bdef_BindUtil::bind(&Negotiation::setCredentials,
                                negotiation,
                                _1,
                                _2,
                                _3,
                                negotiation));
        return;
    }
    bsl::ostringstream request;

    unsigned char buffer = VERSION_USERNAME_PASSWORD_AUTH;
    request << buffer;

    buffer = d_credentials.password().size();
    request << buffer;

    if (registerReadCb(&Negotiation::authenticationCallback, negotiation)) {
        return;
    }
    int rc = d_socket_p->write(request.str().c_str(), request.str().size());
    if (rc != request.str().size()) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing username/password"));
        return;
    }
}

void Negotiation::connectToEndpoint(Context negotiation)
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
    // TODO: is dotted-decimal address ever invalid as SOCKS5 argument?

    int length = d_destination.hostname().size();
    buffer = length;
    request << buffer;
    request << d_destination.hostname();

    // encode 2-byte port in network (bigendian) order
    buffer = (d_destination.port() >> 8) & 0xff; // MSB
    request << buffer;
    buffer = d_destination.port() & 0xff; // LSB
    request << buffer;

    if (registerReadCb(&Negotiation::connectCallback, negotiation)) {
        return;
    }
    int rc = d_socket_p->write(request.str().c_str(), request.str().size());
    if (rc != request.str().size()) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error writing connection request"));
        return;
    }
}

void Negotiation::authenticationCallback(Context negotiation)
{
    AuthenticationResponsePkt pkt;
    int rc = d_socket_p->read(reinterpret_cast<char *>(&pkt), sizeof(pkt));
    if (sizeof(pkt) != rc) {
        if (d_acquiringCredentials) {
            d_provider_p->cancelAcquiringCredentials();
        }
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error reading auth. response"));
        return;
    }

    if (pkt.d_status) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("authentication rejected"));
        return;
    }

    connectToEndpoint(negotiation);
}

void Negotiation::connectCallback(Context negotiation)
{
    bsl::ostringstream e("connect response: ", d_allocator_p);

    ConnectRspBase hdr;
    int rc = d_socket_p->read(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    if (sizeof(hdr) != rc) {
        e << "error reading: " << rc;
        terminate(btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    if (hdr.d_ver != VERSION) {
        e << "invalid SOCK version, expected " << VERSION
          << " got " << hdr.d_ver;
        terminate(btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    int addressLength; // bytes to read for the bound address
    if (hdr.d_atype == IPv4ADDRESS) {
        addressLength = 4;
    } else if (hdr.d_atype == IPv6ADDRESS) {
        addressLength = 16;
    } else if (hdr.d_atype == DOMAINNAME) {
        unsigned char length = 0;
        rc = d_socket_p->read(reinterpret_cast<char *>(&length), 1);
        if (1 != rc) {
            e << "error reading domainname length: " << rc;
            terminate(btes5_Negotiator::e_ERROR,
                      btes5_DetailedError(e.str()));
            return;
        }
        addressLength = length;
    } else {
        e << "received invalid address type: " << hdr.d_atype;
        terminate(btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
        return;
    }

    // consume address and port (not presently used)
    char buf[260]; // maximum length of address + 2 is 257
    rc = d_socket_p->read(buf, addressLength + 2);
    if (addressLength + 2 != rc) {
        e << "error reading bound address, expected " << addressLength + 2
          << " got " << rc;
        terminate(btes5_Negotiator::e_ERROR, btes5_DetailedError(e.str()));
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
    terminate(REQUEST_GRANTED == hdr.d_rep ? btes5_Negotiator::e_SUCCESS
                                           : btes5_Negotiator::e_ERROR,
              btes5_DetailedError(e.str()));
}

void Negotiation::setCredentials(int                      status,
                                 const bslstl::StringRef& username,
                                 const bslstl::StringRef& password,
                                 Context                  negotiation)
{
    d_acquiringCredentials = false;
    if (status) {
        terminate(btes5_Negotiator::e_ERROR,
                  btes5_DetailedError("error acquiring credentials"));
    } else {
        d_credentials.set(username, password);
        sendAuthenticationRequest(negotiation);
    }
}

}  // close anonymous namespace

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
    NegotiationStateCallback               callback)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     d_eventManager_p,
                                                     d_allocator_p));
    return negotiation->sendMethodRequest(negotiation);
}

int btes5_Negotiator::negotiate(
    bteso_StreamSocket<bteso_IPv4Address> *socket,
    const bteso_Endpoint&                  destination,
    NegotiationStateCallback               callback,
    const btes5_Credentials&               credentials)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     d_eventManager_p,
                                                     d_allocator_p));
    negotiation->d_credentials = credentials;
    return negotiation->sendMethodRequest(negotiation);
}

int btes5_Negotiator::negotiate(
    bteso_StreamSocket<bteso_IPv4Address> *socket,
    const bteso_Endpoint&                  destination,
    NegotiationStateCallback               callback,
    btes5_CredentialsProvider             *provider)
{
    Negotiation::Context
        negotiation(new (*d_allocator_p) Negotiation(socket,
                                                     destination,
                                                     callback,
                                                     d_eventManager_p,
                                                     d_allocator_p));
    negotiation->d_provider_p = provider;
    return negotiation->sendMethodRequest(negotiation);
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
