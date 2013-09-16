// btes5_testserver.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_testserver_cpp, "$Id$ $CSID$")

#include <btes5_testserver.h>

#include <bcema_blobutil.h>
#include <bcema_sharedptr.h>
#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bcemt_thread.h>                   // thread management util
#include <bdef_bind.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bsl_iostream.h>
#include <bslma_default.h>
#include <bsls_atomic.h>
#include <bsl_sstream.h>
#include <btemt_asyncchannel.h>
#include <btemt_channelpoolchannel.h>
#include <btemt_message.h>
#include <btemt_session.h>
#include <btemt_sessionpool.h>
#include <btemt_tcptimereventmanager.h>
#include <bteso_resolveutil.h>
#include <btesos_tcptimedchannel.h>

#define UNUSED(x) (void) (x)

namespace BloombergLP {

namespace {

static bcemt_Mutex g_logLock;  // serialize diagnostics

#define LOG_STREAM(severity, args) {                      \
    if (severity <= (args).d_verbosity) {                 \
        bcemt_LockGuard<bcemt_Mutex> guard(&g_logLock);   \
        *(args).d_logStream_p << (args).d_label << ": ";  \
        *(args).d_logStream_p

#define LOG_END bsl::endl; \
    }                      \
}

#define LOG_DEBUG LOG_STREAM(btes5_TestServerArgs::e_DEBUG, d_args)
#define LOG_ERROR LOG_STREAM(btes5_TestServerArgs::e_ERROR, d_args)
#define LOG_TRACE LOG_STREAM(btes5_TestServerArgs::e_TRACE, d_args)

}  // close unnamed namespace

struct Socks5MethodRequestHeader {
    unsigned char d_version;
    unsigned char d_numMethods;
};

struct Socks5MethodResponse {
    unsigned char d_version;
    unsigned char d_method;

    Socks5MethodResponse(int method = 0)
    : d_version(5)
    , d_method((unsigned char) method) {
    }
};

struct Socks5CredentialsHeader {
    unsigned char d_version;
    unsigned char d_usernameLength;
};

struct Socks5ConnectBase {
    unsigned char    d_version;
    unsigned char    d_command;
    unsigned char    d_reserved;
    unsigned char    d_addressType;

    //Socks5ConnectBase()
    //: d_version(5), d_command(1), d_reserved(0) { }
};

struct Socks5ConnectBody1 {
    // SOCKS5 Connect Request body in the form of IP address and port.

    bdeut_BigEndianInt32    d_ip;
    bdeut_BigEndianInt16    d_port;
};

struct Socks5ConnectToIPv4Address {
    unsigned char    d_version;
    unsigned char    d_command;
    unsigned char    d_reserved;
    unsigned char    d_addressType;
    unsigned char    d_address[4];
    unsigned short   d_port;

    //Socks5ConnectToIPv4Address()
    //: d_version(5), d_command(1), d_reserved(0), d_addressType(1) { }
};

struct Socks5ConnectResponseBase {
    unsigned char    d_version;
    unsigned char    d_reply;
    unsigned char    d_reserved;
    unsigned char    d_addressType;

    Socks5ConnectResponseBase()
    : d_version(5), d_reply(0), d_reserved(0), d_addressType(1) { }
};

struct Socks5ConnectResponse1 {
    Socks5ConnectResponseBase d_base;
    unsigned char             d_ip[4];
    unsigned char             d_port[2];

    Socks5ConnectResponse1()
    { }
};

struct Socks5ConnectResponse3 {
    Socks5ConnectResponseBase d_base;
    unsigned char d_hostLen;
    unsigned char d_data[1];
};

                             // ====================
                             // struct Socks5Session
                             // ====================

struct Socks5Session : public btemt_Session {
    // A concrete implementation of 'btemt_Session'.  A 'Socks5Session' can
    // represent one of two connections from a proxy: a client connection
    // (which is used for negotiation), or a destination connection.

    // DATA
    btes5_TestServer::SessionFactory * const d_factory_p;
        // factory managing this session, not owned

    const btes5_TestServerArgs&  d_args;  // arguments controlling the proxy

    btemt_TcpTimerEventManager         d_eventManager;  // for delayed writes

    bool                d_client;      // 'true' iff client connection
    btemt_AsyncChannel *d_channel_p;   // connection channel, not owned
    Socks5Session      *d_opposite_p;  // opposite side of the proxy, not owned
    bteso_IPv4Address   d_peer;        // peer address, for diagnostics
    bslma::Allocator   *d_allocator_p; // memory allocator, not owned

    // PRIVATE MANIPULATORS

             // btemt_AsyncChannel interface

    template<typename MSG>
    int readMessage(void (Socks5Session::*func)(const MSG *data,
                                                int        length,
                                                int       *numConsumed,
                                                int       *numNeeded));
        // Queue up reading a message of the specified type 'MSG', and arrange
        // to invoke the specified 'func' when it's received.

    template<typename MSG>
    void readMessageCb(int                    result,
                       int                   *consumed,
                       int                   *needed,
                       const btemt_DataMsg&   msg,
                       void (Socks5Session::*func)(const MSG *data,
                                                   int        length,
                                                   int       *numConsumed,
                                                   int       *numNeeded));
        // If the specified 'result' is 'BTEMT_SUCCESS' invoke the specified
        // 'func' passing it the data from the specified 'msg' as the specified
        // template type 'MSG'.

    int clientWrite(const char *buf, int length);
        // Send the specified 'buf' of the specified 'length' bytes to the
        // client using 'd_channel_p'.  Return 0 on success, and a non-zero
        // value otherwise.  If 'd_args->d_delay' is set, wait that much before
        // sending the data.

    int clientWriteImmediate(bcema_SharedPtr<bcema_Blob> blob);
        // Send the specified 'blob' to the client using 'd_channel_p'.  Return
        // 0 on success, and a non-zero value otherwise.

             // SOCKS5 protocol processing

    void readIgnore(const unsigned char *data,
                          int            length,
                          int           *consumed,
                          int           *needed);
        // Keep reading and discarding input from the client.

    void readMethods(const Socks5MethodRequestHeader *data,
                     int                              length,
                     int                             *consumed,
                     int                             *needed);
        // Read a SOCKS5 Method Request represented by the specified 'data'
        // with the specified 'length', and indicate the consumed bytes and
        // needed bytes in the specified 'consumed' and 'needed' locations.

    void readCredentials(const Socks5CredentialsHeader   *data,
                         int                              length,
                         int                             *consumed,
                         int                             *needed);
        // Read and validate a SOCSK5 Username/Password authentication request
        // represented by the specified 'data' with the specified 'length', and
        // update the specified 'consumed' and 'needed' byte counts.

    void readConnect(const Socks5ConnectBase  *data,
                     int                       length,
                     int                      *consumed,
                     int                      *needed);
        // Read, validate, and process the SOCKS5 connection request
        // represented by the specified 'data' with the specified 'length', and
        // update the specified 'consumed' and 'needed' byte counts.

    void readProxy(int                   result,
                   int                  *consumed,
                   int                  *needed,
                   const btemt_DataMsg&  msg);
        // If the specified 'result' is 'BTEMT_SUCCESS' forward the specified
        // 'msg' to the opposite connection, updating the counters at the
        // specified 'consumed' location to reflect the bytes forwarded, and
        // the specified 'needed' location to request (at least) one more byte.
        // This method actually implements the proxy after negotiation and
        // connection have been completed.

    void startDestination(Socks5Session *clientSession);
        // Start proxy logic with the specified 'clientSession'.

    void startClient();
        // Start SOCSK5 negotiation with a client.

  private:
    // NOT IMPLEMENTED
    Socks5Session(const Socks5Session&);
    Socks5Session& operator=(const Socks5Session&);

  public:
    // CREATORS
    Socks5Session(btes5_TestServer::SessionFactory *factory,
                  btemt_AsyncChannel               *channel,
                  const btes5_TestServerArgs&       arg,
                  bslma::Allocator                 *allocator);
         // Create a new 'Socks5Session' managed by the specified 'factory' for
         // the connection accessed through the specified 'channel', configured
         // by the specified 'args', and use the specified 'allocator' to
         // supply memory.

     ~Socks5Session();
         // Destroy this object.

     // MANIPULATORS
     virtual int start();
         // Begin the asynchronous operation of this session.

     virtual int stop();
         // Stop the operation of this session.

    // ACCESSORS
    btemt_AsyncChannel *channel() const;
        // Return the communication channel used by this session.

};

                    // ======================================
                    // class btes5_TestServer::SessionFactory
                    // ======================================

class btes5_TestServer::SessionFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that allocates 'Socks5Session' objects.  No specific allocation strategy
    // (such as pooling) is implemented.
    //
    // A 'SessionFactory' constructs a 'btemt_SessionPool' to manage IO events,
    // and allocates (when requested by the session pool) sessions that
    // implement proxy functionality.  A pair of 'Socks5Session' objects
    // support one logical proxy with two connections.  One is accepted, and is
    // used for client-side SOCKS5 negotiation.  The other is to the
    // destination, to which the proxy tries to connect after the negotiation
    // is concluded.

    const btes5_TestServerArgs&  d_args;  // arguments controlling the proxy

    bcema_SharedPtr<btemt_SessionPool> d_sessionPool;   // managed pool

    bslma::Allocator     *d_allocator_p; // memory allocator (held, not owned)

    // PRIVATE MANIPULATORS

                        // 'btemt_SessionPool' callbacks

    void poolStateCb(int reason, int source, void *userData);
        // Process the session pool state change with the specified 'reason',
        // 'source' and 'userData'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(btes5_TestServer::SessionFactory,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    SessionFactory(bteso_Endpoint              *proxy,
                   const btes5_TestServerArgs&  args = 0,
                   bslma::Allocator            *allocator = 0);
        // Create a new 'SessionFactory' object that will allocate
        // 'Socks5Sessions', start listening for incoming proxy requests, and
        // load the listen address into the specified 'proxy'. If the
        // optionally specified 'args' is not 0 use 'args' to control the proxy
        // session behavior.  Optionally specify 'allocator' used to supply
        // memory.  If 'allocator' is 0, the currently installed default
        // allocator is used.

    virtual ~SessionFactory();
       // Destroy this factory.

    // MANIPULATORS
    void sessionStateCb(int            state,
                        int            handle,
                        btemt_Session *session,
                        void          *clientSession);
        // Process the session state change for the specified 'state',
        // 'handle', 'session' and 'clientSession'.  If 'clientSession' is 0
        // 'session' represents the client connection; otherwise, 'session'
        // represents the destination connection associated with
        // 'clientSession'.

    void connect(const bteso_Endpoint& destination, void *userData);
        // Asynchronously connect to the specified 'destination', using
        // 'btemt_SessionPool::connect', and pass the specified 'userData' to
        // the session state callback.

                      // 'btemt_SessionFactory' interface

    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
       // Asynchronously allocate a 'btemt_Session' object for the
       // specified 'channel', and invoke the specified 'callback' with
       // this session.

    virtual void deallocate(btemt_Session *session);
       // Deallocate the specified 'session'.

};

                        // --------------------
                        // struct Socks5Session
                        // --------------------

// PRIVATE MANIPULATORS
template<typename MSG>
int Socks5Session::readMessage(
    void (Socks5Session::*func)(const MSG *data,
                                int        length,
                                int       *numConsumed,
                                int       *numNeeded))
{
    using namespace bdef_PlaceHolders;
    btemt_AsyncChannel::ReadCallback cb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &Socks5Session::readMessageCb<MSG>,
                               this,
                               _1,
                               _2,
                               _3,
                               _4,
                               func);
    int rc = d_channel_p->read(sizeof(MSG), cb);
    return rc;
}

template<typename MSG>
void Socks5Session::readMessageCb(
                                  int                   result,
                                  int                  *consumed,
                                  int                  *needed,
                                  const btemt_DataMsg&  msg,
                                  void (Socks5Session::*func)(
                                                        const MSG *data,
                                                        int        length,
                                                        int       *numConsumed,
                                                        int       *numNeeded))
{
    if (btemt_AsyncChannel::BTEMT_SUCCESS == result) {
        const int length = msg.data()->length();
        bsl::string buf(length, '\0');
        msg.data()->copyOut(&buf[0], length, 0);
        (this->*func)(reinterpret_cast<MSG*>(&buf[0]),
                                             length,
                                             consumed,
                                             needed);
    } else {
        LOG_ERROR << "async read failed, result " << result << LOG_END;
        stop();
        BSLS_ASSERT(false);
    }
}

int Socks5Session::clientWriteImmediate(
    bcema_SharedPtr<bcema_Blob> blob)
{
    if (btes5_TestServerArgs::e_TRACE <= d_args.d_verbosity) {
        *d_args.d_logStream_p << d_args.d_label << ": sending "
                              << blob->length() << " bytes to client "
                              << d_peer << ": ";
        bcema_BlobUtil::hexDump(*d_args.d_logStream_p, *blob);
        *d_args.d_logStream_p << bsl::endl;
    }
    int rc = d_channel_p->write(*blob);
    if (rc) {
        LOG_DEBUG << "cannot send " << blob->length()
                  << " bytes to the client, rc " << rc << LOG_END;
        stop();
    }
    return rc;
}

int Socks5Session::clientWrite(const char *buf, int length)
{
    bcema_SharedPtr<char>
        buffer(reinterpret_cast<char*>(d_allocator_p->allocate(length)),
               d_allocator_p);
    bsl::memcpy(buffer.ptr(), buf, length);
    bcema_BlobBuffer blobBuffer(buffer, length);

    bcema_SharedPtr<bcema_Blob> blob(new (*d_allocator_p)
                                         bcema_Blob(d_allocator_p),
                                     d_allocator_p);
    blob->prependDataBuffer(blobBuffer);

    if (bdet_TimeInterval() == d_args.d_delay) {
        return clientWriteImmediate(blob);                            // RETURN
    }

    bdet_TimeInterval scheduledTime = bdetu_SystemTime::now() + d_args.d_delay;
    bteso_EventManager::Callback cb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &Socks5Session::clientWriteImmediate,
                               this,
                               blob);
    d_eventManager.registerTimer(scheduledTime, cb);
    return 0;
}

void Socks5Session::readIgnore(const unsigned char *data,
                               int                  length,
                               int                 *consumed,
                               int                 *needed)
{
    UNUSED(data);
    UNUSED(length);
    *consumed = 1;
    *needed = 1;  // keep reading one byte at a time
}

void Socks5Session::readMethods(
    const Socks5MethodRequestHeader *data,
    int                              length,
    int                             *consumed,
    int                             *needed)
{
    UNUSED(consumed);
    BSLS_ASSERT(5 == data->d_version);
    BSLS_ASSERT(1 <= data->d_numMethods);
    if (length < (int) sizeof(*data) + data->d_numMethods) {
        *needed = sizeof(*data) + data->d_numMethods;
        return;                                                       // RETURN
    }
    *needed = 0;
    *consumed = sizeof(*data) + data->d_numMethods;

    LOG_DEBUG << "method request: version " << (int) data->d_version
              << ", " << (int) data->d_numMethods << " methods:" << LOG_END;
    const unsigned char *supportedMethods
        = reinterpret_cast<const unsigned char *>(data + 1);
    for (int method = 0; method < data->d_numMethods; ++method) {
        LOG_DEBUG << "  method " << (int) supportedMethods[method]
                  << " supported" << LOG_END;
    }

    int method;  // authentication method required
    if (d_args.d_expectedCredentials.isSet()) {
        int i;
        method = 2;  // need username/password
        for (i = 0; i < data->d_numMethods; ++i) {
            if (supportedMethods[i] == method) {
                break;  // client supports username/password method
            }
            BSLS_ASSERT(i < data->d_numMethods);
        }
        readMessage(&Socks5Session::readCredentials);
    } else {
        method = 0;  // method: no authentication
        BSLS_ASSERT(method == supportedMethods[0]);
        readMessage(&Socks5Session::readConnect);
    }

    Socks5MethodResponse methodResponse(method);
    int rc = clientWrite((char *)&methodResponse, sizeof(methodResponse));
    if (!rc) {
        LOG_DEBUG << "Wrote MethodResponse(method = " << method << ")"
                  << LOG_END;
    }
}

void Socks5Session::readCredentials(
    const Socks5CredentialsHeader *data,
    int                            length,
    int                           *consumed,
    int                           *needed)
{
    const int ulen = data->d_usernameLength;

    LOG_DEBUG << "received username request"
              << " version " << (int) data->d_version
              << " username length " << ulen
              << LOG_END;
    BSLS_ASSERT(1 == data->d_version);  // version must be 1

    if (length < (int) sizeof(*data) + ulen + 1) {
            // read username and password length (one byte)

        *needed = sizeof(*data) + ulen + 1;
        return;                                                       // RETURN
    }
    const unsigned char
        *ubuf = reinterpret_cast<const unsigned char *>(data + 1);

    const int plen = ubuf[ulen];  // following username is the password length
    if (length < (int) sizeof(*data) + ulen + 1 + plen) {
        *needed = sizeof(*data) + ulen + 1 + plen;
        return;                                                       // RETURN
    }

    *needed = 0;
    *consumed = sizeof(*data) + ulen + 1 + plen;
    bsl::string username((char *) ubuf, ulen, d_allocator_p);
    bsl::string password(reinterpret_cast<const char *>(ubuf + ulen + 1),
                         plen,
                         d_allocator_p);
    btes5_Credentials requestCredentials(username, password, d_allocator_p);
    LOG_DEBUG << " credentials " << requestCredentials << LOG_END;

    char response[2];
    response[0] = 1;  // version
    if (requestCredentials == d_args.d_expectedCredentials) {
        LOG_DEBUG << "username authenticated" << LOG_END;
        response[1] = 0;
        int rc = clientWrite(response, sizeof(response));
        BSLS_ASSERT(!rc);
        readMessage(&Socks5Session::readConnect);
    } else {
        LOG_ERROR << "authentication failure:"
                  << " expected " << d_args.d_expectedCredentials
                  << " recieved " << requestCredentials
                  << LOG_END;
        response[1] = 1;
        clientWrite(response, sizeof(response));
        stop();
    }
}

void Socks5Session::readConnect(const Socks5ConnectBase *data,
                                int                      length,
                                int                     *consumed,
                                int                     *needed)
{
    LOG_DEBUG << "Read ConnectBase"
              << " version=" << (int)data->d_version
              << " command=" << (int)data->d_command
              << " address type=" << (int)data->d_addressType
              << LOG_END;
    BSLS_ASSERT(5 == data->d_version);
    BSLS_ASSERT(1 == data->d_command);
    BSLS_ASSERT(0 == data->d_reserved);
    BSLS_ASSERT((1 == data->d_addressType) ||
           (3 == data->d_addressType));

    char respBuffer[512];
    bsl::size_t respLen = 0;
    Socks5ConnectResponseBase *respBase =
            reinterpret_cast<Socks5ConnectResponseBase *>(respBuffer);

    respBase->d_version = 5;
    respBase->d_reply = (unsigned char) d_args.d_reply;
    respBase->d_reserved = 0;
    respBase->d_addressType = data->d_addressType;

    bteso_IPv4Address connectAddr;
    bteso_Endpoint destination(d_allocator_p);
    if (1 == data->d_addressType) {
        if (length < (int) (sizeof(data) + sizeof(Socks5ConnectBody1))) {
            *needed = sizeof(data) + sizeof(Socks5ConnectBody1);
            return;                                                   // RETURN
        }
        *needed = 0;
        *consumed = sizeof(*data) + sizeof(Socks5ConnectBody1);

        const Socks5ConnectBody1&
            body1 = *reinterpret_cast<const Socks5ConnectBody1 *>(data + 1);
        connectAddr.setIpAddress(body1.d_ip);
        connectAddr.setPortNumber(body1.d_port);
        LOG_DEBUG << "connect addr=" << connectAddr
                  << LOG_END;

        BSLS_ASSERT(!d_args.d_expectedIp
                || body1.d_ip == d_args.d_expectedIp);
        BSLS_ASSERT(!d_args.d_expectedPort
                || body1.d_port == d_args.d_expectedPort);

        Socks5ConnectResponse1
            *resp = (Socks5ConnectResponse1 *)respBuffer;
        memcpy(&resp->d_ip, &body1.d_ip, sizeof(resp->d_ip));
        memcpy(&resp->d_port, &body1.d_port, sizeof(resp->d_port));
        respLen = sizeof(Socks5ConnectResponse1);
    } else if (3 == data->d_addressType) {
        if (length < (int) sizeof(data) + 1) {
            *needed = sizeof(data) + 1;
            return;                                                   // RETURN
        }
        const int hostLen = *reinterpret_cast<const unsigned char *>(data + 1);
        if (length < (int) sizeof(data) + 1 + hostLen + 2) {
            *needed = sizeof(data) + 1 + hostLen + 2;
            return;                                                   // RETURN
        }
        *needed = 0;
        *consumed = sizeof(data) + 1 + hostLen + 2;

        const char *hostBuffer = reinterpret_cast<const char *>(data + 1) + 1;
        bdeut_BigEndianInt16 port;
        memcpy(&port, hostBuffer + hostLen, sizeof(port));
        unsigned short nativePort = (short) port;
        destination.set(bsl::string(hostBuffer, hostLen, d_allocator_p),
                        nativePort);
        LOG_DEBUG << "connect addr=" << destination << LOG_END;

        BSLS_ASSERT(!d_args.d_expectedDestination.isSet()
                || d_args.d_expectedDestination == destination);

        Socks5ConnectResponse3 *resp = (Socks5ConnectResponse3 *)respBuffer;
        resp->d_hostLen = (unsigned char) hostLen;
        memcpy(&resp->d_data[0], hostBuffer, hostLen);
        memcpy(&resp->d_data[hostLen], &port, sizeof(port));
        respLen = sizeof(Socks5ConnectResponse3) - 1 + hostLen + sizeof(port);
    } else {
        LOG_ERROR << "unsupported request address type "
                  << data->d_addressType << ", closing"
                  << LOG_END;
        stop();
    }

    if (btes5_TestServerArgs::e_FAIL == d_args.d_mode) {
        unsigned char reply = 1; // general SOCKS server failure
        if (d_args.d_reply) {
            reply = (unsigned char) d_args.d_reply;
        }
        LOG_DEBUG << "sending error code " << reply
                  << " and closing" << LOG_END;
        respBase->d_reply = reply;
        clientWrite((char *)respBuffer, respLen);
    }

    if (btes5_TestServerArgs::e_SUCCEED_AND_CLOSE == d_args.d_mode) {
        LOG_DEBUG << "sending success and closing" << LOG_END;
        clientWrite((char *)respBuffer, respLen);
    }

    if (btes5_TestServerArgs::e_CONNECT == d_args.d_mode) {
        if (d_args.d_destination.isSet()) {
            destination = d_args.d_destination;
        }
        if (destination.isSet()) {
            d_factory_p->connect(destination, this);
        }

    }
}

void Socks5Session::readProxy(int                   result,
                              int                  *consumed,
                              int                  *needed,
                              const btemt_DataMsg&  msg)
{
    if (btemt_AsyncChannel::BTEMT_SUCCESS == result) {
        const int length = msg.data()->length();
        int rc = -1;
        if (d_opposite_p) {
            rc = d_opposite_p->d_channel_p->write(msg);
            if (rc) {
                LOG_ERROR << "cannot forward " << length << " bytes to "
                          << d_opposite_p->d_peer << ", rc " << rc << LOG_END;
            } else {
                LOG_TRACE << "forwarded " << length << " bytes from "
                          << d_peer << " to " << d_opposite_p->d_peer
                          << LOG_END;
            }
        }
        else {
            LOG_ERROR << "cannot forward: no opposite side" << LOG_END;
        }

        if (rc) {
            stop();
        }
        else {
            *consumed = length;
            *needed = 1;
        }
    }
    else {
        LOG_ERROR << "read from " << d_peer << " failed, result " << result
                  << LOG_END;
        stop();
    }
}

void Socks5Session::startDestination(Socks5Session *clientSession)
{
    d_client = false;
    d_opposite_p = clientSession;
    clientSession->d_opposite_p = this;

    LOG_DEBUG << "Connected to destination " << d_peer << LOG_END;
    Socks5ConnectResponse1 response;  // d_reply = SUCCESS by default
    clientSession->clientWrite((char *) &response, sizeof(response));

    // start proxying by forwarding received data to the opposite side

    using namespace bdef_PlaceHolders;

    btemt_AsyncChannel::ReadCallback destinationCb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &Socks5Session::readProxy,
                               this,
                               _1,
                               _2,
                               _3,
                               _4);
    d_channel_p->read(1, destinationCb);

    btemt_AsyncChannel::ReadCallback clientCb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &Socks5Session::readProxy,
                               clientSession,
                               _1,
                               _2,
                               _3,
                               _4);
    clientSession->d_channel_p->read(1, clientCb);
}

void Socks5Session::startClient()
{
    d_client = true;
    LOG_DEBUG << "Accepted connection from " << d_peer << LOG_END;
    BSLS_ASSERT(!d_eventManager.enable());

    // client connection: start negotiation

    if (btes5_TestServerArgs::e_IGNORE == d_args.d_mode) {
        readMessage(&Socks5Session::readIgnore);
    } else {
        readMessage(&Socks5Session::readMethods);
    }
}

// CREATORS
Socks5Session::Socks5Session(btes5_TestServer::SessionFactory *factory,
                             btemt_AsyncChannel               *channel,
                             const btes5_TestServerArgs&       args,
                             bslma::Allocator                 *allocator)
: d_factory_p(factory)
, d_args(args)
, d_eventManager(allocator)
, d_channel_p(channel)
, d_opposite_p(0)
, d_peer(channel->peerAddress())
, d_allocator_p(allocator)
{
}

Socks5Session::~Socks5Session()
{
}

// MANIPULATORS
int Socks5Session::start()
{
    return 0;  // real processing is in 'startClient' or 'startDestination'
}

int Socks5Session::stop()
{
    d_channel_p->close();
    return 0;
}

// ACCCESSORS
btemt_AsyncChannel *Socks5Session::channel() const
{
    return d_channel_p;
}

                    // --------------------------------------
                    // class btes5_TestServer::SessionFactory
                    // --------------------------------------

// PRIVATE MANIPULATORS
void btes5_TestServer::SessionFactory::poolStateCb(int   reason,
                                                   int   source,
                                                   void *userData)
{
    UNUSED(userData);
    LOG_DEBUG << "Pool state changed: (" << reason << ", " << source
              << ") " << LOG_END;
}

// CREATORS
btes5_TestServer::SessionFactory::SessionFactory(
    bteso_Endpoint              *proxy,
    const btes5_TestServerArgs&  args,
    bslma::Allocator            *allocator)
: d_args(args)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    btemt_ChannelPoolConfiguration config;
    config.setMaxThreads(1);
    config.setMaxConnections(2);  // client + destination connection

    // config.setReadTimeout(5.0);               // in seconds
    // config.setMetricsInterval(10.0);          // seconds
    // config.setMaxWriteCache(1<<10);           // 1Mb
    // config.setIncomingMessageSizes(1, 100, 1024);

    using namespace bdef_PlaceHolders;
    btemt_SessionPool::SessionPoolStateCallback poolStateCb
        = bdef_BindUtil::bindA(
                          d_allocator_p,
                          &btes5_TestServer::SessionFactory::poolStateCb,
                          this,
                          _1,
                          _2,
                          _3);

    d_sessionPool.load(new (*d_allocator_p)
                            btemt_SessionPool(config,
                                              poolStateCb,
                                              allocator),
                        allocator);

    btemt_SessionPool::SessionStateCallback cb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &SessionFactory::sessionStateCb,
                               this,
                               _1,
                               _2,
                               _3,
                               _4);

    int rc;  // return code

    rc = d_sessionPool->start();
    BSLS_ASSERT(!rc);

    int handle;
    rc = d_sessionPool->listen(&handle,
                               cb,
                               0,         // let system assign port
                               5,         // backlog
                               1,         // REUSEADDR
                               this,      // SessionFactory
                               0);        // userData
    BSLS_ASSERT(!rc);

    const int port = d_sessionPool->portNumber(handle);
    LOG_DEBUG << "listening on port " << port << LOG_END;
    if (proxy) {
        proxy->set("localhost", port);
    }
}

btes5_TestServer::SessionFactory::~SessionFactory()
{
    d_sessionPool->stop();
}

// MANIPULATORS
void btes5_TestServer::SessionFactory::sessionStateCb(
                                                  int            state,
                                                  int            handle,
                                                  btemt_Session *session,
                                                  void          *clientSession)
{
    UNUSED(handle);
    Socks5Session *s = dynamic_cast<Socks5Session *>(session);
    Socks5Session *cs = reinterpret_cast<Socks5Session*>(clientSession);

    if (s && btemt_SessionPool::SESSION_UP == state) {
        if (cs) {
            s->startDestination(cs);
        }
        else {
            s->startClient();
        }
    }
    else {
        if (cs) {
            LOG_DEBUG << "client " << cs->d_peer
                      << ": destination connection state " << state << LOG_END;
            switch (state) {
              case btemt_SessionPool::CONNECT_ATTEMPT_FAILED: {
                return;  // another attempt may succeed
              } break;
              case btemt_SessionPool::SESSION_ALLOC_FAILED:
              case btemt_SessionPool::SESSION_STARTUP_FAILED:
              case btemt_SessionPool::CONNECT_FAILED:
              case btemt_SessionPool::CONNECT_ABORTED:
                Socks5ConnectResponse1 response;
                response.d_base.d_reply = 4;  // Host unreachable
                cs->clientWrite((char *) &response, sizeof(response));
                cs->stop();
              break;
            }
        }
        else {
            LOG_ERROR << "client connection state " << state << LOG_END;
        }
        if (s) {
            s->stop();
        }
    }
}

void btes5_TestServer::SessionFactory::connect(
                                            const bteso_Endpoint&  destination,
                                            void                  *userData)
{
    int handle;

    using namespace bdef_PlaceHolders;
    btemt_SessionPool::SessionStateCallback cb = bdef_BindUtil::bindA(
                             d_allocator_p,
                             &btes5_TestServer::SessionFactory::sessionStateCb,
                             this,
                             _1,
                             _2,
                             _3,
                             _4);

    const int numAttempts = 3;
    bdet_TimeInterval interval(0.2);
    int rc = d_sessionPool->connect(&handle,
                                    cb,
                                    destination.hostname().c_str(),
                                    destination.port(),
                                    numAttempts,
                                    interval,
                                    this,
                                    userData);  // userData = clientSession
    if (rc) {
        LOG_ERROR << "cannot initiate connection to " << destination
                  << ", rc" << rc << LOG_END;
        BSLS_ASSERT(false);
    }
}

void
btes5_TestServer::SessionFactory::allocate(
    btemt_AsyncChannel                    *channel,
    const btemt_SessionFactory::Callback&  callback)
{
    Socks5Session *session = new (*d_allocator_p) Socks5Session(this,
                                                                channel,
                                                                d_args,
                                                                d_allocator_p);
    callback(0, session);
}

void
btes5_TestServer::SessionFactory::deallocate(btemt_Session *session)
{
    Socks5Session *s = dynamic_cast<Socks5Session *>(session);
    BSLS_ASSERT(s);
    LOG_DEBUG << "deallocate "
              << (s->d_client ? "client" : "destination") << " session"
              << LOG_END;
    if (s->d_opposite_p) {
        s->d_opposite_p->d_opposite_p = 0;
    }
    d_allocator_p->deleteObjectRaw(session);
}

                         // ---------------------------
                         // struct btes5_TestServerArgs
                         // ---------------------------

// CREATORS
btes5_TestServerArgs::btes5_TestServerArgs(bslma::Allocator *allocator)
: d_mode(e_SUCCEED_AND_CLOSE)
, d_reply(0)
, d_destination(allocator)
, d_label(allocator)
, d_verbosity(e_DEBUG)
, d_logStream_p(&bsl::cout)
, d_expectedDestination(allocator)
{
    d_expectedIp = 0;
    d_expectedPort = 0;
}

                        // ----------------------
                        // class btes5_TestServer
                        // ----------------------

// CREATORS
btes5_TestServer::btes5_TestServer(bteso_Endpoint             *proxy,
                                   const btes5_TestServerArgs *args,
                                   bslma::Allocator           *allocator)
: d_args(bslma::Default::allocator(allocator))
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(0 == bteso_SocketImpUtil::startup());

    if (args) {
        d_args = *args;
    }
    d_sessionFactory.load(new (*d_allocator_p)
                              SessionFactory(proxy, d_args, d_allocator_p),
                          d_allocator_p);

}

btes5_TestServer::~btes5_TestServer()
{
}

// MANIPULATORS

// ACCESSORS

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
