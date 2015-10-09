// btls5_negotiator.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_negotiator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_negotiator_cpp,"$Id$ $CSID$")

#include <btls5_detailedstatus.h>
#include <btls5_testserver.h>  // for testing only

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlt_currenttime.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <btlso_eventmanager.h>
#include <btlso_eventtype.h>

#include <bsl_cstddef.h>
#include <bsl_memory.h>
#include <bsl_new.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace btls5 {

namespace {

// SOCKS5 protocol messages as defined in RFCs 1928 and 1929.

enum Socks5 {
    VERSION                        = 0x05,
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

}  // close unnamed namespace

                        // ============================
                        // class Negotiator_Negotiation
                        // ============================

class Negotiator_Negotiation {
    // This mechanism class describes the state of a SOCKS connection
    // negotiation.  The object lifetime is managed by using a
    // 'bsl::shared_ptr<Negotiator_Negotiation>' in callback registration.  A
    // 'Negotiator_Negotiation' object is in one of two states: it is created
    // in the normal state indicated by 'd_terminating == 0', and it enters a
    // terminating state when 'd_terminating != 0'.

  public:
    // DATA
    Credentials                              d_credentials;     // SOCKS5
                                                                // credentials

    btlso::Endpoint                          d_destination;     // destination

    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;        // proxy socket

    btlso::SocketHandle::Handle              d_handle;          // OS-level
                                                                // socket

    Negotiator::NegotiationStateCallback     d_callback;        // result
                                                                // callback

    btlmt::TcpTimerEventManager             *d_eventManager_p;  // asynch event
                                                                // registration
                                                                // manager, not
                                                                // owned

    bsls::TimeInterval                       d_timeout;         // timeout

    void                                    *d_timer;           // expiration
                                                                // timer

    bslmt::Mutex                             d_timerLock;       // 'd_timer'
                                                                // access

    bsls::AtomicInt                          d_terminating;     // negotiation
                                                                // being
                                                                // terminated

    // CREATORS
    Negotiator_Negotiation(
                      btlso::StreamSocket<btlso::IPv4Address> *socket,
                      const btlso::Endpoint&                   destination,
                      Negotiator::NegotiationStateCallback     callback,
                      const bsls::TimeInterval&                timeout,
                      btlmt::TcpTimerEventManager             *eventManager,
                      bslma::Allocator                        *basicAllocator);
        // Create a 'Negotiator_Negotiation' object to connect to the specified
        // 'destination' over the specified 'socket' and asynchronously invoke
        // the specified 'callback', using the specified 'eventManager' to
        // schedule callbacks.  If the specified 'timeout' is equal to
        // 'bsls::TimeInterval()' the negotiation will not time out.
        // Otherwise, the negotiation will conclude, either successfully or in
        // error, within the 'timeout' interval.  Use the specified
        // 'basicAllocator' to supply memory.

    ~Negotiator_Negotiation();
        // Destroy this object.
};

                       // -----------------------------
                       // struct Negotiator_Negotiation
                       // -----------------------------

// CREATORS
Negotiator_Negotiation::Negotiator_Negotiation(
                       btlso::StreamSocket<btlso::IPv4Address> *socket,
                       const btlso::Endpoint&                   destination,
                       Negotiator::NegotiationStateCallback     callback,
                       const bsls::TimeInterval&                timeout,
                       btlmt::TcpTimerEventManager             *eventManager,
                       bslma::Allocator                        *basicAllocator)
: d_credentials(basicAllocator)
, d_destination(destination, basicAllocator)
, d_socket_p(socket)
, d_handle(socket->handle())
, d_callback(
          bsl::allocator_arg_t(),
          bsl::allocator<Negotiator::NegotiationStateCallback>(basicAllocator),
          callback)
, d_eventManager_p(eventManager)
, d_timeout(timeout)
, d_timer(0)
{
    BSLS_ASSERT(socket);
    BSLS_ASSERT(eventManager);
}

Negotiator_Negotiation::~Negotiator_Negotiation()
{
    // clean up in case 'terminate' was called prior to registering timer

    if (d_timer) {
        d_eventManager_p->deregisterTimer(d_timer);
    }
}

// STATIC HELPER FUNCTIONS

static void terminate(
                     btls5::Negotiator::NegotiationHandle negotiation,
                     btls5::Negotiator::NegotiationStatus status,
                     const btls5::DetailedStatus&         error,
                     bool                                 canceledFlag = false)
    // Terminate the specified 'negotiation'.  If the optionally specified
    // 'canceledFlag' is 'false', invoke the user-supplied callback with the
    // specified 'status' and 'error'.  If 'canceledFlag' is 'true', do not
    // invoke the callback.
{
    if (negotiation->d_terminating.testAndSwap(0, 1)) {

        // this negotiation is already being terminated

        return;                                                       // RETURN
    }

    negotiation->d_eventManager_p->deregisterSocket(negotiation->d_handle);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&negotiation->d_timerLock);
        if (negotiation->d_timer) {
            negotiation->d_eventManager_p->deregisterTimer(
                                                         negotiation->d_timer);
        }
    }

    if (!canceledFlag) {
        negotiation->d_callback(status, error);
    }
}

typedef void (*ReadCallback)(btls5::Negotiator::NegotiationHandle negotiation);
    // A callback of this type is invoked when data is received for the
    // specified 'negotiation'.

static int registerReadCb(ReadCallback                         callback,
                          btls5::Negotiator::NegotiationHandle negotiation)
    // Register the specified 'callback' to be invoked with the argument of the
    // specified 'negotiation' on read events on 'negotiation->d_handle'.
    // Return 0 on success and a negative value otherwise.
{
    btlso::EventManager::Callback readCb =
                                   bdlf::BindUtil::bind(callback, negotiation);

    int rc = negotiation->d_eventManager_p->registerSocketEvent(
                                                      negotiation->d_handle,
                                                      btlso::EventType::e_READ,
                                                      readCb);

    if (rc < 0) {
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus("error registering read handler"));
        return btls5::Negotiator::e_ERROR;                            // RETURN
    }
    return 0;
}

static void connectCallback(btls5::Negotiator::NegotiationHandle negotiation)
    // Process a response from the SOCKS5 connect request for the specified
    // 'negotiation'.
{
    if (negotiation->d_terminating) {
        return;                                                       // RETURN
    }

    bsl::ostringstream e("connect response: ");
    e.seekp(0, bsl::ios_base::end);

    ConnectRspBase hdr;
    int rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&hdr),
                                           sizeof hdr);

    if (sizeof hdr != rc) {
        e << "error reading: " << rc;

        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(e.str()));
        return;                                                       // RETURN
    }

    if (hdr.d_ver != VERSION) {
        e << "invalid SOCK version, expected " << VERSION
          << ", got " << hdr.d_ver;

        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(e.str()));
        return;                                                       // RETURN
    }

    int addressLength;  // bytes to read for the bound address
    if (IPv4ADDRESS == hdr.d_atype) {
        addressLength = 4;
    } else if (IPv6ADDRESS == hdr.d_atype) {
        addressLength = 16;
    } else if (DOMAINNAME == hdr.d_atype) {
        unsigned char length = 0;
        rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&length),
                                           1);
        if (1 != rc) {
            e << "error reading domainname length: " << rc;

            terminate(negotiation,
                      btls5::Negotiator::e_ERROR,
                      btls5::DetailedStatus(e.str()));
            return;                                                   // RETURN
        }
        addressLength = length;
    } else {
        e << "received invalid address type: " << hdr.d_atype;

        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(e.str()));
        return;                                                       // RETURN
    }

    // consume address and port (not presently used)

    char buf[255 + 2];  // maximum length of address + 2-byte port
    BSLS_ASSERT(addressLength + 2 <= static_cast<int>(sizeof buf));
    rc = negotiation->d_socket_p->read(buf, addressLength + 2);

    if (addressLength + 2 != rc) {
        e << "error reading bound address, expected " << addressLength + 2
          << ", got " << rc;

        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(e.str()));
        return;                                                       // RETURN
    }

    switch (hdr.d_rep) {
      case REQUEST_GRANTED: {
        e << "status: request granted";
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
        e << "status: time to live expired";
      } break;
      case CMD_PROTO_NOT_SUPPORTED: {
        e << "status: cmd protocol not supported";
      } break;
      case ADDRESS_TYPE_NOT_SUPPORTED: {
        e << "status: address type not supported";
      } break;
      default: {
        e << "status: unknown = " << hdr.d_rep;
      } break;
    }
    terminate(negotiation,
              REQUEST_GRANTED == hdr.d_rep
              ? btls5::Negotiator::e_SUCCESS
              : btls5::Negotiator::e_ERROR,
              btls5::DetailedStatus(e.str()));
}

static void connectToEndpoint(btls5::Negotiator::NegotiationHandle negotiation)
    // Send the request for the specified 'negotiation' to connect to
    // 'negotiation->d_destination' via the proxy host reachable by
    // 'negotiation->d_socket_p'.
{
    bsl::ostringstream req;

    req << static_cast<unsigned char>(VERSION)
        << static_cast<unsigned char>(TCP_STREAM_CONNECTION)
        << static_cast<unsigned char>(0x00);                  // reserved

    const bool dottedDecimal = btlso::IPv4Address::isValidAddress(
                               negotiation->d_destination.hostname().c_str());
    if (dottedDecimal) {
        req << static_cast<unsigned char>(IPv4ADDRESS);       // address type
        btlso::IPv4Address   ipv4Address(
                                 negotiation->d_destination.hostname().c_str(),
                                 negotiation->d_destination.port());
        const int            ipAddress = ipv4Address.ipAddress();
        const unsigned char *addrPtr =
                           reinterpret_cast<const unsigned char *>(&ipAddress);
        req << addrPtr[0] << addrPtr[1] << addrPtr[2] << addrPtr[3];
    } else {
        req << static_cast<unsigned char>(DOMAINNAME);        // address type

        bsl::size_t length = negotiation->d_destination.hostname().size();
        req << static_cast<unsigned char>(length)
            << negotiation->d_destination.hostname();
    }

    // encode 2-byte port in network (bigendian) order: MSB first

    req << static_cast<unsigned char>(
                               (negotiation->d_destination.port() >> 8) & 0xff)
        << static_cast<unsigned char>(
                                     negotiation->d_destination.port() & 0xff);

    if (registerReadCb(connectCallback, negotiation)) {
        return;                                                       // RETURN
    }
    const bsl::string& buf = req.str();
    int rc = negotiation->d_socket_p->write(buf.c_str(), buf.size());
    if (rc != static_cast<int>(buf.size())) {
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus("error writing connection request"));
        return;                                                       // RETURN
    }
}

static void sendAuthenticationRequest(
                             btls5::Negotiator::NegotiationHandle negotiation);
    // Send the username and password credentials to authenticate with the
    // SOCKS5 server for the specified 'negotiation'.

static void authenticationCallback(
                              btls5::Negotiator::NegotiationHandle negotiation)
    // Process the response to an authentication request for the specified
    // 'negotiation'.
{
    if (negotiation->d_terminating) {
        return;                                                       // RETURN
    }

    AuthenticationResponsePkt pkt;
    int rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&pkt),
                                           sizeof pkt);
    if (sizeof pkt != rc) {
        terminate(
               negotiation,
               btls5::Negotiator::e_ERROR,
               btls5::DetailedStatus("error reading authentication response"));
        return;                                                       // RETURN
    }

    if (pkt.d_status) {
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus("authentication rejected"));
        return;                                                       // RETURN
    }

    connectToEndpoint(negotiation);
}

static void sendAuthenticationRequest(
                              btls5::Negotiator::NegotiationHandle negotiation)
    // Send an authentication request for the specified 'negotiation'.
{
    // The Username/Password request (RFC 1929) format is:
    // +----+------+----------+------+----------+
    // |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
    // +----+------+----------+------+----------+
    // | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
    // +----+------+----------+------+----------+

    bsl::ostringstream request;
    request << static_cast<unsigned char>(VERSION_USERNAME_PASSWORD_AUTH)
            << static_cast<unsigned char>(
                                  negotiation->d_credentials.username().size())
            << negotiation->d_credentials.username()
            << static_cast<unsigned char>(
                                  negotiation->d_credentials.password().size())
            << negotiation->d_credentials.password();

    if (registerReadCb(authenticationCallback, negotiation)) {
        return;                                                       // RETURN
    }
    const bsl::string& buf = request.str();
    int rc = negotiation->d_socket_p->write(buf.c_str(), buf.size());

    if (rc != static_cast<int>(buf.size())) {
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus("error writing username/password"));
        return;                                                       // RETURN
    }
}

static void methodCallback(btls5::Negotiator::NegotiationHandle negotiation)
    // Process SOCKS5 Method Selection response for the specified
    // 'negotiation'.
{
    if (negotiation->d_terminating) {
        return;                                                       // RETURN
    }

    negotiation->d_eventManager_p->deregisterSocketEvent(
                                                     negotiation->d_handle,
                                                     btlso::EventType::e_READ);
    MethodResponsePkt pkt;
    int rc = negotiation->d_socket_p->read(reinterpret_cast<char *>(&pkt),
                                           sizeof pkt);
    if (sizeof pkt != rc) {
        bsl::ostringstream description;
        description << "error reading method response, rc " << rc;
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(description.str()));
        return;                                                       // RETURN
    }

    switch (pkt.d_method) {
      case NONE: {
        connectToEndpoint(negotiation);
      } break;
      case PASSWORD: {
        if (!negotiation->d_credentials.username().length()) {
            terminate(negotiation,
                      btls5::Negotiator::e_ERROR,
                      btls5::DetailedStatus(
                               "Got authentication request when we did not"
                               " offer authentication as an option, closing"));
            return;                                                   // RETURN
        }
        sendAuthenticationRequest(negotiation);
      } break;
      case UNACCEPTABLE: {
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(
                          "proxy server rejected all authentication methods"));
      } break;
      default: {
        bsl::ostringstream description;
        description << "unknown response "
                    << static_cast<int>(pkt.d_method)
                    << " from proxy server";
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus(description.str()));
      } break;
    }
}

static void timeoutCallback(btls5::Negotiator::NegotiationHandle negotiation)
    // Expire the specified 'negotiation'.
{
    if (negotiation->d_terminating) {
        return;                                                       // RETURN
    }

    negotiation->d_timer = 0;
    terminate(negotiation,
              btls5::Negotiator::e_ERROR,
              btls5::DetailedStatus("timeout"));
}

static int sendMethodRequest(btls5::Negotiator::NegotiationHandle negotiation)
    // Send SOCKS5 greeting including list of authentication methods supported
    // using the specified 'negotiation' for managing object lifetime.  Return
    // 0 on success and a non-zero value otherwise.
{
    MethodRequestPkt pkt;
    pkt.d_ver        = VERSION;
    pkt.d_nmethods   = 2;
    pkt.d_methods[0] = NONE;
    pkt.d_methods[1] = PASSWORD;

    int length = sizeof pkt;
    if (!negotiation->d_credentials.username().length()) {

        // Don't offer PASSWORD authentication unless we have credentials.

        --pkt.d_nmethods;
        length -= sizeof *pkt.d_methods;
    }

    if (registerReadCb(methodCallback, negotiation)) {
        return btls5::Negotiator::e_ERROR;                            // RETURN
    }

    if (bsls::TimeInterval() != negotiation->d_timeout) {
        bsls::TimeInterval expiration =
                             bdlt::CurrentTime::now() + negotiation->d_timeout;
        btlso::EventManager::Callback cb =
                            bdlf::BindUtil::bind(timeoutCallback, negotiation);
        {
            bslmt::LockGuard<bslmt::Mutex> lock(&negotiation->d_timerLock);
            negotiation->d_timer =
                  negotiation->d_eventManager_p->registerTimer(expiration, cb);
        }
    }

    int rc = negotiation->d_socket_p->write(
                                          reinterpret_cast<const char *>(&pkt),
                                          length);
    if (length != rc) {
        // Since we indicate an immediate error via return code, do not invoke
        // the callback.

        const bool noCallback = true;
        terminate(negotiation,
                  btls5::Negotiator::e_ERROR,
                  btls5::DetailedStatus("error writing method request"),
                  noCallback);
        return btls5::Negotiator::e_ERROR;                            // RETURN
    }

    return 0;
}

                              // ----------------
                              // class Negotiator
                              // ----------------

// CREATORS
Negotiator::Negotiator(btlmt::TcpTimerEventManager *eventManager,
                       bslma::Allocator            *basicAllocator)
: d_eventManager_p(eventManager)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(eventManager);
}

// MANIPULATORS
Negotiator::NegotiationHandle Negotiator::makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback)
{
    BSLS_ASSERT(socket);

    Negotiator::NegotiationHandle negotiation(
              new (*d_allocator_p) Negotiator_Negotiation(socket,
                                                          destination,
                                                          callback,
                                                          bsls::TimeInterval(),
                                                          d_eventManager_p,
                                                          d_allocator_p),
              d_allocator_p);

    return negotiation;
}

Negotiator::NegotiationHandle Negotiator::makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback,
                          const bsls::TimeInterval&                timeout)
{
    BSLS_ASSERT(socket);

    Negotiator::NegotiationHandle negotiation(
                  new (*d_allocator_p) Negotiator_Negotiation(socket,
                                                              destination,
                                                              callback,
                                                              timeout,
                                                              d_eventManager_p,
                                                              d_allocator_p),
                  d_allocator_p);

    return negotiation;
}

Negotiator::NegotiationHandle Negotiator::makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback,
                          const Credentials&                       credentials)
{
    BSLS_ASSERT(socket);

    Negotiator::NegotiationHandle negotiation(
              new (*d_allocator_p) Negotiator_Negotiation(socket,
                                                          destination,
                                                          callback,
                                                          bsls::TimeInterval(),
                                                          d_eventManager_p,
                                                          d_allocator_p),
              d_allocator_p);

    negotiation->d_credentials = credentials;
    return negotiation;
}

Negotiator::NegotiationHandle Negotiator::makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback,
                          const bsls::TimeInterval&                timeout,
                          const Credentials&                       credentials)
{
    BSLS_ASSERT(socket);

    Negotiator::NegotiationHandle negotiation(
                  new (*d_allocator_p) Negotiator_Negotiation(socket,
                                                              destination,
                                                              callback,
                                                              timeout,
                                                              d_eventManager_p,
                                                              d_allocator_p),
                  d_allocator_p);

    negotiation->d_credentials = credentials;
    return negotiation;
}

int Negotiator::startNegotiation(const NegotiationHandle& handle)
{
    return sendMethodRequest(handle);
}

void Negotiator::cancelNegotiation(const NegotiationHandle& handle)
{
    terminate(handle,
              Negotiator::e_ERROR,
              DetailedStatus("SOCKS5 negotiation canceled"),
              true);
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
