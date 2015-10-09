// btls5_testserver.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_testserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_testserver_cpp, "$Id$ $CSID$")

#include <btlb_blobutil.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_mutex.h>
#include <bdlf_bind.h>
#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_atomic.h>
#include <btlmt_asyncchannel.h>
#include <btlmt_session.h>
#include <btlmt_sessionfactory.h>
#include <btlmt_sessionpool.h>
#include <btlmt_tcptimereventmanager.h>

#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>

#define UNUSED(x) (void) (x)

namespace BloombergLP {

namespace btls5 {

namespace {

static bslmt::Mutex g_logLock;  // serialize diagnostics

#define LOG_STREAM(severity, args)                                 \
    if (bool stop = false)                                         \
        ;                                                          \
    else                                                           \
        if ((args).d_verbosity < severity)                         \
            ;                                                      \
        else                                                       \
            for (*(args).d_logStream_p << (args).d_label << ": ";  \
                 !stop;                                            \
                 stop = true, *(args).d_logStream_p << bsl::endl)  \
                    *(args).d_logStream_p

#define LOG_DEBUG LOG_STREAM(btls5::TestServerArgs::e_DEBUG, d_args)
#define LOG_ERROR LOG_STREAM(btls5::TestServerArgs::e_ERROR, d_args)
#define LOG_TRACE LOG_STREAM(btls5::TestServerArgs::e_TRACE, d_args)

struct Socks5MethodRequestHeader {
    unsigned char d_version;
    unsigned char d_numMethods;
};

struct Socks5MethodResponse {
    unsigned char d_version;
    unsigned char d_method;

    // CREATORS
    explicit Socks5MethodResponse(int method = 0)
        // Create the a SOCKS5 Method Response encoding the specified 'method'.
    : d_version(5)
    , d_method((unsigned char) method)
    {
    }
};

struct Socks5CredentialsHeader {
    unsigned char d_version;
    unsigned char d_usernameLength;
};

struct Socks5ConnectBase {
    unsigned char d_version;
    unsigned char d_command;
    unsigned char d_reserved;
    unsigned char d_addressType;
};

struct Socks5ConnectBody1 {
    // SOCKS5 Connect Request body in the form of IP address and port.

    bdlb::BigEndianInt32 d_ip;
    bdlb::BigEndianInt16 d_port;
};

struct Socks5ConnectToIPv4Address {
    unsigned char  d_version;
    unsigned char  d_command;
    unsigned char  d_reserved;
    unsigned char  d_addressType;
    unsigned char  d_address[4];
    unsigned short d_port;
};

struct Socks5ConnectResponseBase {
    unsigned char d_version;
    unsigned char d_reply;
    unsigned char d_reserved;
    unsigned char d_addressType;

    // CREATORS
    Socks5ConnectResponseBase()
        // Create a response base having default values.
    : d_version(5), d_reply(0), d_reserved(0), d_addressType(1)
    {
    }
};

struct Socks5ConnectResponse1 {
    Socks5ConnectResponseBase d_base;
    unsigned char             d_ip[4];
    unsigned char             d_port[2];
};

struct Socks5ConnectResponse3 {
    Socks5ConnectResponseBase d_base;
    unsigned char             d_hostLen;
    unsigned char             d_data[1];
};

}  // close unnamed namespace

                            // ====================
                            // struct Socks5Session
                            // ====================

struct Socks5Session : public btlmt::Session {
    // A concrete implementation of 'btlmt::Session'.  A 'Socks5Session' can
    // represent one of two connections from a proxy: a client connection
    // (which is used for negotiation), or a destination connection.

    // DATA
    btls5::TestServer::SessionFactory * const d_factory_p;
        // factory managing this session, not owned

    const btls5::TestServerArgs&              d_args;
        // arguments controlling the proxy

    btlmt::TcpTimerEventManager               d_eventManager;
        // for delayed writes

    bool                 d_client;     // 'true' iff client connection
    btlmt::AsyncChannel *d_channel_p;  // connection channel, not owned
    Socks5Session       *d_opposite_p; // opposite side of the proxy, not owned
    btlso::IPv4Address   d_peer;       // peer address, for diagnostics
    bslma::Allocator    *d_allocator_p;// memory allocator, not owned

  private:
    // PRIVATE MANIPULATORS

                        // btlmt::AsyncChannel interface

    template<class MSG>
    int readMessage(void (Socks5Session::*func)(const MSG *data,
                                                int        length,
                                                int       *consumed,
                                                int       *numNeeded));
        // Queue up reading a message of the specified type 'MSG', and arrange
        // to invoke the specified 'func' when it's received.

    template<class MSG>
    void readMessageCb(int         result,
                       int        *needed,
                       btlb::Blob *msg,
                       int         channelId,
                       void (Socks5Session::*func)(const MSG *data,
                                                   int        length,
                                                   int       *consumed,
                                                   int       *numNeeded));
        // If the specified 'result' is 'BTLMT_SUCCESS' invoke the specified
        // 'func' passing it the data from the specified 'msg' as the specified
        // template type 'MSG'.

    int clientWriteImmediate(bsl::shared_ptr<btlb::Blob> blob);
        // Send the specified 'blob' to the client using 'd_channel_p'.  Return
        // 0 on success, and a non-zero value otherwise.

                         // SOCKS5 protocol processing

    void readIgnore(const unsigned char *data,
                    int                  length,
                    int                 *consumed,
                    int                 *needed);
        // Keep reading and discarding input from the client.

    void readMethods(const Socks5MethodRequestHeader *data,
                     int                              length,
                     int                             *consumed,
                     int                             *needed);
        // Read a SOCKS5 Method Request represented by the specified 'data'
        // with the specified 'length', and indicate the consumed bytes and
        // needed bytes in the specified 'consumed' and 'needed' locations.

    void readCredentials(const Socks5CredentialsHeader *data,
                         int                            length,
                         int                           *consumed,
                         int                           *needed);
        // Read and validate a SOCKS5 Username/Password authentication request
        // represented by the specified 'data' with the specified 'length', and
        // update the specified 'consumed' and 'needed' byte counts.

    void readConnect(const Socks5ConnectBase *data,
                     int                      length,
                     int                     *consumed,
                     int                     *needed);
        // Read, validate, and process the SOCKS5 connection request
        // represented by the specified 'data' with the specified 'length', and
        // update the specified 'consumed' and 'needed' byte counts.

    void readProxy(int         result,
                   int        *needed,
                   btlb::Blob *msg,
                   int         channelId);
        // If the specified 'result' is 'BTLMT_SUCCESS' forward the specified
        // 'msg' to the opposite connection, updating the counters at the
        // specified 'consumed' location to reflect the bytes forwarded, and
        // the specified 'needed' location to request (at least) one more byte.
        // This method actually implements the proxy after negotiation and
        // connection have been completed.

  private:
    // NOT IMPLEMENTED
    Socks5Session(const Socks5Session&);
    Socks5Session& operator=(const Socks5Session&);

  public:
    // CREATORS
    Socks5Session(btls5::TestServer::SessionFactory *factory,
                  btlmt::AsyncChannel               *channel,
                  const btls5::TestServerArgs&       arg,
                  bslma::Allocator                  *basicAllocator);
        // Create a new 'Socks5Session' managed by the specified 'factory' for
        // the connection accessed through the specified 'channel', configured
        // by the specified 'args'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~Socks5Session() = default;
        // Destroy this object.

    // MANIPULATORS
    int clientWrite(const char *buf, int length);
        // Send the specified 'buf' of the specified 'length' bytes to the
        // client using 'd_channel_p'.  Return 0 on success, and a non-zero
        // value otherwise.  If 'd_args->d_delay' is set, wait that much before
        // sending the data.

    virtual int start();
        // Begin the asynchronous operation of this session.

    void startClient();
        // Start SOCKS5 negotiation with a client.

    void startDestination(Socks5Session *clientSession);
        // Start proxy logic with the specified 'clientSession'.

    virtual int stop();
        // Stop the operation of this session.

    // ACCESSORS
    btlmt::AsyncChannel *channel() const;
        // Return the communication channel used by this session.
};

                      // ================================
                      // class TestServer::SessionFactory
                      // ================================

class TestServer::SessionFactory : public btlmt::SessionFactory {
    // This class is a concrete implementation of the 'btlmt::SessionFactory'
    // that allocates 'Socks5Session' objects.  No specific allocation strategy
    // (such as pooling) is implemented.
    //
    // A 'SessionFactory' constructs a 'btlmt::SessionPool' to manage IO
    // events, and allocates (when requested by the session pool) sessions that
    // implement proxy functionality.  A pair of 'Socks5Session' objects
    // support one logical proxy with two connections.  One is accepted, and is
    // used for client-side SOCKS5 negotiation.  The other is to the
    // destination, to which the proxy tries to connect after the negotiation
    // is concluded.

    const TestServerArgs&                d_args;        // arguments
                                                        // controlling the
                                                        // proxy

    bsl::shared_ptr<btlmt::SessionPool>  d_sessionPool; // managed pool

    bslma::Allocator                    *d_allocator_p; // memory allocator
                                                        // (held, not owned)

    // PRIVATE MANIPULATORS

                        // 'btlmt::SessionPool' callbacks

    void poolStateCb(int reason, int source, void *userData);
        // Process the session pool state change with the specified 'reason',
        // 'source' and 'userData'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestServer::SessionFactory,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    SessionFactory(btlso::Endpoint       *proxy,
                   const TestServerArgs&  args,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a new 'SessionFactory' object that will allocate
        // 'Socks5Sessions', start listening for incoming proxy requests, and
        // load the listen address into the specified 'proxy', and use 'args'
        // to control the proxy session behavior.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~SessionFactory();
        // Destroy this object.

    // MANIPULATORS
    void sessionStateCb(int             state,
                        int             handle,
                        btlmt::Session *session,
                        void           *clientSession);
        // Process the session state change for the specified 'state',
        // 'handle', 'session' and 'clientSession'.  If 'clientSession' is 0
        // 'session' represents the client connection; otherwise, 'session'
        // represents the destination connection associated with
        // 'clientSession'.

    void connect(const btlso::Endpoint& destination, void *userData);
        // Asynchronously connect to the specified 'destination', using
        // 'btlmt::SessionPool::connect', and pass the specified 'userData' to
        // the session state callback.

                      // 'btlmt::SessionFactory' interface

    virtual void allocate(btlmt::AsyncChannel                   *channel,
                          const btlmt::SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btlmt::Session' object for the specified
        // 'channel', and invoke the specified 'callback' with this session.

    virtual void deallocate(btlmt::Session *session);
        // Deallocate the specified 'session'.

};

                            // --------------------
                            // struct Socks5Session
                            // --------------------

// PRIVATE MANIPULATORS
template<class MSG>
int Socks5Session::readMessage(
                             void (Socks5Session::*func)(const MSG *data,
                                                         int        length,
                                                         int       *consumed,
                                                         int       *numNeeded))
{
    using namespace bdlf::PlaceHolders;
    btlmt::AsyncChannel::BlobBasedReadCallback cb =
                       bdlf::BindUtil::bind(&Socks5Session::readMessageCb<MSG>,
                                             this,
                                             _1,
                                             _2,
                                             _3,
                                             _4,
                                             func);

    int rc = d_channel_p->read(sizeof(MSG), cb);
    return rc;
}

template<class MSG>
void Socks5Session::readMessageCb(int         result,
                                  int        *needed,
                                  btlb::Blob *msg,
                                  int         channelId,
                                  void (Socks5Session::*func)(
                                                         const MSG *data,
                                                         int        length,
                                                         int       *consumed,
                                                         int       *numNeeded))
{
    if (btlmt::AsyncChannel::e_SUCCESS == result) {
        const int length = msg->length();
        bsl::string buf(length, '\0');
        btlb::BlobUtil::copy(&buf[0], *msg, 0, length);

        int consumed = 0;
        (this->*func)(reinterpret_cast<MSG *>(&buf[0]),
                      length,
                      &consumed,
                      needed);

        LOG_TRACE << "read " << length << " bytes"
                  << ", needed " << *needed <<", consumed " << consumed;

        btlb::BlobUtil::erase(msg, 0, consumed);
    } else {
        LOG_ERROR << "async read failed, result " << result;
        stop();
        BSLS_ASSERT(false);
    }
}

int Socks5Session::clientWriteImmediate(bsl::shared_ptr<btlb::Blob> blob)
{
    if (btls5::TestServerArgs::e_TRACE <= d_args.d_verbosity) {
        *d_args.d_logStream_p << d_args.d_label << ": sending "
                              << blob->length() << " bytes to client "
                              << d_peer << ": ";
        btlb::BlobUtil::hexDump(*d_args.d_logStream_p, *blob);
        *d_args.d_logStream_p << bsl::endl;
    }
    int rc = d_channel_p->write(*blob);
    if (rc) {
        LOG_DEBUG << "cannot send " << blob->length()
                  << " bytes to the client, rc " << rc;
        stop();
    }
    return rc;
}

int Socks5Session::clientWrite(const char *buf, int length)
{
    bsl::shared_ptr<char> buffer(reinterpret_cast<char *>(
                                              d_allocator_p->allocate(length)),
                                 d_allocator_p);
    bsl::memcpy(buffer.get(), buf, length);
    btlb::BlobBuffer blobBuffer(buffer, length);

    bsl::shared_ptr<btlb::Blob> blob(new (*d_allocator_p) btlb::Blob(
                                                                d_allocator_p),
                                     d_allocator_p);
    blob->prependDataBuffer(blobBuffer);

    if (bsls::TimeInterval() == d_args.d_delay) {
        return clientWriteImmediate(blob);                            // RETURN
    }

    bsls::TimeInterval scheduledTime =
                                     bdlt::CurrentTime::now() + d_args.d_delay;

    btlso::EventManager::Callback cb =
                     bdlf::BindUtil::bind(&Socks5Session::clientWriteImmediate,
                                           this,
                                           blob);

    d_eventManager.registerTimer(scheduledTime, cb);
    LOG_TRACE << "schedule write after " << d_args.d_delay
              << " at " << scheduledTime;
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

void Socks5Session::readMethods(const Socks5MethodRequestHeader *data,
                                int                              length,
                                int                             *consumed,
                                int                             *needed)
{
    BSLS_ASSERT(5 == data->d_version);
    BSLS_ASSERT(1 <= data->d_numMethods);
    if (length < static_cast<int>(sizeof(*data)) + data->d_numMethods) {
        *needed = sizeof(*data) + data->d_numMethods;
        return;                                                       // RETURN
    }
    *needed = 0;
    *consumed = sizeof(*data) + data->d_numMethods;

    LOG_DEBUG << "method request: version "
              << static_cast<int>(data->d_version)
              << ", " << static_cast<int>(data->d_numMethods) << " methods:";

    const unsigned char *supportedMethods
                           = reinterpret_cast<const unsigned char *>(data + 1);
    for (int method = 0; method < data->d_numMethods; ++method) {
        LOG_DEBUG << "  method " << static_cast<int>(supportedMethods[method])
                  << " supported";
    }

    int method;  // authentication method required
    if (d_args.d_expectedCredentials.username().length()) {
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
    int rc = clientWrite(reinterpret_cast<char *>(&methodResponse),
                         sizeof(methodResponse));
    if (!rc) {
        LOG_DEBUG << "Wrote MethodResponse(method = " << method << ")";
    }
}

void Socks5Session::readCredentials(const Socks5CredentialsHeader *data,
                                    int                            length,
                                    int                           *consumed,
                                    int                           *needed)
{
    const int ulen = data->d_usernameLength;

    LOG_DEBUG << "received username request"
              << " version " << static_cast<int>(data->d_version)
              << " username length " << ulen;
    BSLS_ASSERT(1 == data->d_version);  // version must be 1

    if (length < static_cast<int>(sizeof(*data)) + ulen + 1) {
        // read username and password length (one byte)

        *needed = sizeof(*data) + ulen + 1;
        return;                                                       // RETURN
    }
    const unsigned char *ubuf =
                             reinterpret_cast<const unsigned char *>(data + 1);

    const int plen = ubuf[ulen];  // following username is the password length
    if (length < static_cast<int>(sizeof(*data)) + ulen + 1 + plen) {
        *needed = sizeof(*data) + ulen + 1 + plen;
        return;                                                       // RETURN
    }

    *needed = 0;
    *consumed = sizeof(*data) + ulen + 1 + plen;

    bsl::string username(reinterpret_cast<const char *>(ubuf),
                         ulen,
                         d_allocator_p);
    bsl::string password(reinterpret_cast<const char *>(ubuf + ulen + 1),
                         plen,
                         d_allocator_p);

    btls5::Credentials requestCredentials(username, password, d_allocator_p);
    LOG_DEBUG << " credentials " << requestCredentials;

    char response[2];
    response[0] = 1;  // version
    if (requestCredentials == d_args.d_expectedCredentials) {
        LOG_DEBUG << "username authenticated";
        response[1] = 0;
        int rc = clientWrite(response, sizeof(response));
        if (!rc) {
            readMessage(&Socks5Session::readConnect);
        }
    } else {
        LOG_ERROR << "authentication failure:"
                  << " expected " << d_args.d_expectedCredentials
                  << " recieved " << requestCredentials;
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
              << " version="      << (int)data->d_version
              << " command="      << (int)data->d_command
              << " address type=" << (int)data->d_addressType
              << " length="       << length;

    BSLS_ASSERT(5 == data->d_version);
    BSLS_ASSERT(1 == data->d_command);
    BSLS_ASSERT(0 == data->d_reserved);
    BSLS_ASSERT(1 == data->d_addressType || 3 == data->d_addressType);

    char                       respBuffer[512];
    bsl::size_t                respLen  = 0;
    Socks5ConnectResponseBase *respBase =
                     reinterpret_cast<Socks5ConnectResponseBase *>(respBuffer);

    respBase->d_version     = 5;
    respBase->d_reply       = static_cast<unsigned char>(d_args.d_reply);
    respBase->d_reserved    = 0;
    respBase->d_addressType = data->d_addressType;

    btlso::IPv4Address connectAddr;
    btlso::Endpoint    destination(d_allocator_p);

    if (1 == data->d_addressType) {
        if (length < static_cast<int>(
                                 sizeof(*data) + sizeof(Socks5ConnectBody1))) {
            *needed = sizeof(*data) + sizeof(Socks5ConnectBody1);
            return;                                                   // RETURN
        }
        *needed = 0;
        *consumed = sizeof(*data) + sizeof(Socks5ConnectBody1);

        const Socks5ConnectBody1& body1 =
                       *reinterpret_cast<const Socks5ConnectBody1 *>(data + 1);
        connectAddr.setIpAddress(body1.d_ip);
        connectAddr.setPortNumber(body1.d_port);

        LOG_DEBUG << "connect addr=" << connectAddr;

        BSLS_ASSERT(!d_args.d_expectedIp || body1.d_ip == d_args.d_expectedIp);
        BSLS_ASSERT(!d_args.d_expectedPort
                 || body1.d_port == d_args.d_expectedPort);

        Socks5ConnectResponse1 *resp =
                        reinterpret_cast<Socks5ConnectResponse1 *>(respBuffer);
        memcpy(&resp->d_ip,   &body1.d_ip,   sizeof(resp->d_ip));
        memcpy(&resp->d_port, &body1.d_port, sizeof(resp->d_port));

        respLen = sizeof(Socks5ConnectResponse1);
    } else if (3 == data->d_addressType) {
        if (length < static_cast<int>(sizeof(*data)) + 1) {
            *needed = sizeof(*data) + 1;
            return;                                                   // RETURN
        }
        const int hostLen = *reinterpret_cast<const unsigned char *>(data + 1);
        if (length < static_cast<int>(sizeof(*data)) + 1 + hostLen + 2) {
            *needed = sizeof(*data) + 1 + hostLen + 2;
            return;                                                   // RETURN
        }
        *needed = 0;
        *consumed = sizeof(*data) + 1 + hostLen + 2;

        const char *hostBuffer = reinterpret_cast<const char *>(data + 1) + 1;

        bdlb::BigEndianInt16 port;
        memcpy(&port, hostBuffer + hostLen, sizeof(port));
        unsigned short nativePort = static_cast<short>(port);
        destination.set(bsl::string(hostBuffer, hostLen, d_allocator_p),
                        nativePort);
        LOG_DEBUG << "connect addr=" << destination;

        BSLS_ASSERT(!d_args.d_expectedDestination.port()
                 || d_args.d_expectedDestination == destination);

        Socks5ConnectResponse3 *resp =
                        reinterpret_cast<Socks5ConnectResponse3 *>(respBuffer);
        resp->d_hostLen = static_cast<unsigned char>(hostLen);
        memcpy(&resp->d_data[0], hostBuffer, hostLen);
        memcpy(&resp->d_data[hostLen], &port, sizeof(port));
        respLen = sizeof(Socks5ConnectResponse3) - 1 + hostLen + sizeof(port);
    } else {
        LOG_ERROR << "unsupported request address type "
                  << data->d_addressType << ", closing";
        stop();
    }

    if (btls5::TestServerArgs::e_FAIL == d_args.d_mode) {
        unsigned char reply = 1; // general SOCKS server failure
        if (d_args.d_reply) {
            reply = static_cast<unsigned char>(d_args.d_reply);
        }
        LOG_DEBUG << "sending error code " << reply << " and closing";
        respBase->d_reply = reply;
        clientWrite(static_cast<char *>(respBuffer), respLen);
    }

    if (btls5::TestServerArgs::e_SUCCEED_AND_CLOSE == d_args.d_mode) {
        LOG_DEBUG << "sending success and closing";
        clientWrite(static_cast<char *>(respBuffer), respLen);
    }

    if (btls5::TestServerArgs::e_CONNECT == d_args.d_mode) {
        if (d_args.d_destination.port()) {
            destination = d_args.d_destination;
        }
        if (destination.port()) {
            d_factory_p->connect(destination, this);
        }

    }
}

void Socks5Session::readProxy(int         result,
                              int        *needed,
                              btlb::Blob *msg,
                              int         channelId)
{
    if (btlmt::AsyncChannel::e_SUCCESS == result) {
        const int length = msg->length();
        int rc = -1;
        if (d_opposite_p) {
            rc = d_opposite_p->d_channel_p->write(*msg);
            if (rc) {
                LOG_ERROR << "cannot forward " << length << " bytes to "
                          << d_opposite_p->d_peer << ", rc " << rc;
            } else {
                LOG_TRACE << "forwarded " << length << " bytes from "
                          << d_peer << " to " << d_opposite_p->d_peer;
            }
        }
        else {
            LOG_ERROR << "cannot forward: no opposite side";
        }

        if (rc) {
            stop();
        }
        else {
            btlb::BlobUtil::erase(msg, 0, length);
            *needed = 1;
        }
    }
    else {
        LOG_ERROR << "read from " << d_peer << " failed, result " << result;
        stop();
    }
}

void Socks5Session::startDestination(Socks5Session *clientSession)
{
    d_client = false;
    d_opposite_p = clientSession;
    clientSession->d_opposite_p = this;

    LOG_DEBUG << "Connected to destination " << d_peer;
    Socks5ConnectResponse1 response;  // d_reply = SUCCESS by default
    clientSession->clientWrite((char *) &response, sizeof(response));

    // start proxying by forwarding received data to the opposite side

    using namespace bdlf::PlaceHolders;

    btlmt::AsyncChannel::BlobBasedReadCallback destinationCb =
                                bdlf::BindUtil::bind(&Socks5Session::readProxy,
                                                      this,
                                                      _1,
                                                      _2,
                                                      _3,
                                                      _4);
    d_channel_p->read(1, destinationCb);

    btlmt::AsyncChannel::BlobBasedReadCallback clientCb =
                                bdlf::BindUtil::bind(&Socks5Session::readProxy,
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
    LOG_DEBUG << "Accepted connection from " << d_peer;
    BSLS_ASSERT(!d_eventManager.enable());

    // client connection: start negotiation

    if (btls5::TestServerArgs::e_IGNORE == d_args.d_mode) {
        readMessage(&Socks5Session::readIgnore);
    } else {
        if (bsls::TimeInterval() != d_args.d_delay) {
            int rc = d_eventManager.enable();
            if (rc) {
                LOG_ERROR << "cannot enable event manager, rc " << rc;
                stop();
                return;                                               // RETURN
            }
        }
        readMessage(&Socks5Session::readMethods);
    }
}

// CREATORS
Socks5Session::Socks5Session(btls5::TestServer::SessionFactory *factory,
                             btlmt::AsyncChannel               *channel,
                             const btls5::TestServerArgs&       args,
                             bslma::Allocator                  *basicAllocator)
: d_factory_p(factory)
, d_args(args)
, d_eventManager(basicAllocator)
, d_channel_p(channel)
, d_opposite_p(0)
, d_peer(channel->peerAddress())
, d_allocator_p(basicAllocator)
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
btlmt::AsyncChannel *Socks5Session::channel() const
{
    return d_channel_p;
}

                  // ---------------------------------------
                  // class btls5::TestServer::SessionFactory
                  // ---------------------------------------

// PRIVATE MANIPULATORS
void btls5::TestServer::SessionFactory::poolStateCb(int   reason,
                                                    int   source,
                                                    void *userData)
{
    UNUSED(userData);
    LOG_DEBUG << "Pool state changed: (" << reason << ", " << source << ") ";
}

// CREATORS
btls5::TestServer::SessionFactory::SessionFactory(
                                  btlso::Endpoint              *proxy,
                                  const btls5::TestServerArgs&  args,
                                  bslma::Allocator             *basicAllocator)
: d_args(args)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    btlmt::ChannelPoolConfiguration config;
    config.setMaxThreads(3);
    config.setMaxConnections(2);  // client + destination connection

    // config.setReadTimeout(5.0);               // in seconds
    // config.setMetricsInterval(10.0);          // seconds
    // config.setMaxWriteCache(1<<10);           // 1Mb
    // config.setIncomingMessageSizes(1, 100, 1024);

    using namespace bdlf::PlaceHolders;
    btlmt::SessionPool::SessionPoolStateCallback poolStateCb =
          bdlf::BindUtil::bind(&btls5::TestServer::SessionFactory::poolStateCb,
                                this,
                                _1,
                                _2,
                                _3);

    d_sessionPool.reset(new (*d_allocator_p) btlmt::SessionPool(
                                                               config,
                                                               poolStateCb,
                                                               basicAllocator),
                        basicAllocator);

    btlmt::SessionPool::SessionStateCallback cb =
                          bdlf::BindUtil::bind(&SessionFactory::sessionStateCb,
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
                               20,        // backlog
                               1,         // REUSEADDR
                               this,      // SessionFactory
                               0);        // userData
    BSLS_ASSERT(!rc);

    const int port = d_sessionPool->portNumber(handle);
    LOG_DEBUG << "listening on port " << port;
    if (proxy) {
        proxy->set("localhost", port);
    }
}

btls5::TestServer::SessionFactory::~SessionFactory()
{
    d_sessionPool->stop();
}

// MANIPULATORS
void btls5::TestServer::SessionFactory::sessionStateCb(
                                                 int             state,
                                                 int             handle,
                                                 btlmt::Session *session,
                                                 void           *clientSession)
{
    UNUSED(handle);
    Socks5Session  *s = dynamic_cast<Socks5Session *>(session);
    Socks5Session *cs = reinterpret_cast<Socks5Session *>(clientSession);

    if (s && btlmt::SessionPool::e_SESSION_UP == state) {
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
                      << ": destination connection state " << state;
            switch (state) {
              case btlmt::SessionPool::e_CONNECT_ATTEMPT_FAILED: {
                // another attempt may succeed

                return;                                               // RETURN
              } break;
              case btlmt::SessionPool::e_SESSION_ALLOC_FAILED:
              case btlmt::SessionPool::e_SESSION_STARTUP_FAILED:
              case btlmt::SessionPool::e_CONNECT_FAILED:
              case btlmt::SessionPool::e_CONNECT_ABORTED: {
                Socks5ConnectResponse1 response;
                response.d_base.d_reply = 4;  // Host unreachable
                cs->clientWrite((char *) &response, sizeof(response));
                cs->stop();
              } break;
            }
        }
        else {
            if (s) {
                LOG_ERROR << "client " << s->d_peer
                          << ": connection state " << state;
            }
            else {
                LOG_ERROR << "client connection state " << state;
            }
        }
        if (s) {
            s->stop();
        }
    }
}

void btls5::TestServer::SessionFactory::connect(
                                           const btlso::Endpoint&  destination,
                                           void                   *userData)
{
    int handle;

    using namespace bdlf::PlaceHolders;
    btlmt::SessionPool::SessionStateCallback cb = bdlf::BindUtil::bind(
                            &btls5::TestServer::SessionFactory::sessionStateCb,
                             this,
                             _1,
                             _2,
                             _3,
                             _4);

    const int numAttempts = 3;
    bsls::TimeInterval interval(0.2);
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
                  << ", rc" << rc;
        BSLS_ASSERT(false);
    }
    LOG_TRACE << "  attempting connection to " << destination;
}

void
btls5::TestServer::SessionFactory::allocate(
                              btlmt::AsyncChannel                    *channel,
                              const btlmt::SessionFactory::Callback&  callback)
{
    Socks5Session *session = new (*d_allocator_p) Socks5Session(this,
                                                                channel,
                                                                d_args,
                                                                d_allocator_p);
    callback(0, session);
}

void
btls5::TestServer::SessionFactory::deallocate(btlmt::Session *session)
{
    Socks5Session *s = dynamic_cast<Socks5Session *>(session);
    BSLS_ASSERT(s);

    LOG_DEBUG << "deallocate "
              << (s->d_client ? "client" : "destination") << " session";

    if (s->d_opposite_p) {
        s->d_opposite_p->d_opposite_p = 0;
    }
    d_allocator_p->deleteObjectRaw(session);
}

                           // ---------------------
                           // struct TestServerArgs
                           // ---------------------

// CREATORS
TestServerArgs::TestServerArgs(bslma::Allocator *basicAllocator)
: d_mode(e_SUCCEED_AND_CLOSE)
, d_reply(0)
, d_destination(basicAllocator)
, d_label(basicAllocator)
, d_verbosity(e_DEBUG)
, d_logStream_p(&bsl::cout)
, d_expectedDestination(basicAllocator)
{
    d_expectedIp = 0;
    d_expectedPort = 0;
}

                              // ----------------
                              // class TestServer
                              // ----------------

// CREATORS
TestServer::TestServer(btlso::Endpoint  *proxy,
                       bslma::Allocator *basicAllocator)
: d_args(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_sessionFactory.reset(new (*d_allocator_p)
                                  SessionFactory(proxy, d_args, d_allocator_p),
                           d_allocator_p);

}

TestServer::TestServer(btlso::Endpoint      *proxy,
                       const TestServerArgs *args,
                       bslma::Allocator     *basicAllocator)
: d_args(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(args);

    d_args = *args;
    d_sessionFactory.reset(new (*d_allocator_p)
                                  SessionFactory(proxy, d_args, d_allocator_p),
                           d_allocator_p);
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
