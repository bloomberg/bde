// btes5_testserver.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_testserver_cpp, "$Id$ $CSID$")

#include <btes5_testserver.h>

#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bcemt_thread.h>                   // thread management util
#include <bdef_bind.h>
#include <bdet_timeinterval.h>
#include <bsl_iostream.h>
#include <bslma_default.h>
#include <bsls_atomic.h>
#include <bsl_sstream.h>
#include <btemt_asyncchannel.h>
#include <btemt_channelpoolchannel.h>
#include <btemt_message.h>
#include <btemt_session.h>
#include <btemt_sessionpool.h>
#include <bteso_resolveutil.h>
#include <btesos_tcptimedchannel.h>

#define UNUSED(x) (void) (x)

namespace BloombergLP {

namespace {

#define LOG_STREAM(severity, args) {                       \
    if (severity <= (args).d_verbosity) {                 \
        *(args).d_logStream_p << (args).d_label << ": "; \
        *(args).d_logStream_p

#define LOG_END bsl::endl; \
    }                      \
}

#define LOG_DEBUG LOG_STREAM(btes5_TestServerArgs::e_DEBUG, d_args)
#define LOG_ERROR LOG_STREAM(btes5_TestServerArgs::e_ERROR, d_args)

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
    , d_method(method) {
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

/*
static void ServerThread(
        bcema_SharedPtr<btes5_TestServerArgs>                          args,
        bcema_SharedPtr<bteso_StreamSocketFactory<bteso_IPv4Address> > factory,
        bteso_StreamSocket<bteso_IPv4Address>                         *socket,
        bslma::Allocator                                              *alloc)
    // Create a server thread to accept 1 connection on the specified 'socket
    // and then simulate a socks5 server responding to a connect request. Use
    // the specified 'args' as the test server configuration, the specified
    // 'factory' to deallocate 'socket' and the specified 'alloc' to supply
    // memory. This thread will terminate after either a successful or failure
    // condition.
{
    LOG_DEBUG << "ServerThread starting, mode=" << args->d_mode << LOG_END;

    bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
        socketGuard(socket, factory.ptr());

    // TODO: do we need multi-use test server?
    for (int i = 0; i < 1; ++i) {
        bteso_StreamSocket<bteso_IPv4Address> *clientSocket;
        int acceptStatus = socket->accept(&clientSocket);
        if (0 != acceptStatus) {
            LOG_ERROR << "accept failed: " << acceptStatus << LOG_END;
            break;
        }
        bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
            clientGuard(clientSocket, factory.ptr());

        LOG_DEBUG << "Accepted connection." << LOG_END;
        clientSocket->setBlockingMode(bteso_Flag::BLOCKING_MODE);

        if (btes5_TestServerArgs::e_IGNORE == args->d_mode) {
            char buf[256];
            while (clientSocket->read(buf, sizeof buf) > 0) {
                LOG_DEBUG << "received a request, ignoring" << LOG_END;
            }
            continue;
        }

        Socks5MethodRequestHeader methodRequest;
        int rc;
        rc = clientSocket->read((char *)&methodRequest,
                                        sizeof(methodRequest));
        if (rc <= 0) {
            LOG_DEBUG << "read returned " << rc << LOG_END;
            break;
        }
        LOG_DEBUG << "Read MethodRequest Header"
                  << " version=" << (int) methodRequest.d_version
                  << " numMethods=" << (int) methodRequest.d_numMethods
                  << LOG_END;
        BSLS_ASSERT(5 == methodRequest.d_version);
        BSLS_ASSERT(1 <= methodRequest.d_numMethods);

        // read supported methods

        unsigned char supportedMethods[256];
        rc = clientSocket->read((char *)&supportedMethods,
                                        methodRequest.d_numMethods);
        if (rc != methodRequest.d_numMethods) {
            LOG_DEBUG << "read supported methods failed, rc " << rc << LOG_END;
            break;
        }
        for (int method = 0; method < methodRequest.d_numMethods; ++method) {
            LOG_DEBUG << "  method " << (int) supportedMethods[method]
                      << " supported" << LOG_END;
        }

        int method;  // authentication method required
        if (args->d_expectedCredentials.isSet()) {
            int i;
            method = 2;  // need username/password
            for (i = 0; i < methodRequest.d_numMethods; ++i) {
                if (supportedMethods[i] == method) {
                    break;  // client supports username/password method
                }
                BSLS_ASSERT(i < methodRequest.d_numMethods);
            }
        } else {
            method = 0;  // method: no authentication
            BSLS_ASSERT(method == supportedMethods[0]);
        }

        Socks5MethodResponse methodResponse(method);
        rc = clientSocket->write((char *)&methodResponse,
                                 sizeof(methodResponse));
        BSLS_ASSERT(rc > 0);
        if (rc <= 0) {
            break;
        }
        LOG_DEBUG << "Wrote MethodResponse(method = " << method << ")"
                  << LOG_END;

        if (2 == method) {

            // read username/password request header

            unsigned char requestHeader[2];
            rc = clientSocket->read((char *) &requestHeader,
                                    sizeof(requestHeader));
            if (rc != sizeof(requestHeader)) {
                LOG_ERROR << "read username/password request header failed"
                          << ", rc " << rc << LOG_END;
                break;
            }
            LOG_DEBUG << "received username request"
                      << " version " << (int) requestHeader[0]
                      << " username length " << (int) requestHeader[1]
                      << LOG_END;
            BSLS_ASSERT(1 == requestHeader[0]);  // version must be 1

            // read username and password length (one byte)

            const int ulen = requestHeader[1];
            unsigned char ubuf[256];
            rc = clientSocket->read((char *) ubuf, ulen + 1);
            if (rc != ulen + 1) {
                LOG_ERROR << "read username failed"
                          << ", rc " << rc << LOG_END;
                break;
            }

            // read password

            const int plen = ubuf[ulen];
            unsigned char pbuf[256];
            rc = clientSocket->read((char *) pbuf, plen);
            if (rc != plen) {
                LOG_ERROR << "read password failed"
                          << ", rc " << rc << LOG_END;
                break;
            }

            bsl::string username((char *) ubuf, ulen, alloc);
            bsl::string password((char *) pbuf, plen, alloc);

            btes5_Credentials requestCredentials(username, password, alloc);
            char response[2];
            response[0] = 1;  // version
            if (requestCredentials == args->d_expectedCredentials) {
                LOG_DEBUG << "username authenticated" << LOG_END;
                response[1] = 0;
                rc = clientSocket->write(response, sizeof(response));
                BSLS_ASSERT(rc == sizeof(response));
            } else {
                LOG_ERROR << "authentication failure:"
                          << " expected " << args->d_expectedCredentials
                          << " recieved " << requestCredentials
                          << LOG_END;
                response[1] = 1;
                rc = clientSocket->write(response, sizeof(response));
                BSLS_ASSERT(rc == sizeof(response));
                break;
            }
        }

        Socks5ConnectBase connectBase;
        rc = clientSocket->read((char *)&connectBase,
                                sizeof(connectBase));
        if (rc <= 0) {
            LOG_ERROR << "read connect request failed, rc " << rc << LOG_END;
            break;
        }
        LOG_DEBUG << "Read ConnectBase"
                  << " address type=" << (int)connectBase.d_addressType
                  << " command=" << (int)connectBase.d_command
                  << LOG_END;
        BSLS_ASSERT(5 == connectBase.d_version);
        BSLS_ASSERT(1 == connectBase.d_command);
        BSLS_ASSERT(0 == connectBase.d_reserved);
        BSLS_ASSERT((1 == connectBase.d_addressType) ||
               (3 == connectBase.d_addressType));

        char respBuffer[512];
        bsl::size_t respLen = 0;
        Socks5ConnectResponseBase *respBase =
                reinterpret_cast<Socks5ConnectResponseBase *>(respBuffer);

        respBase->d_version = 5;
        respBase->d_reply = args->d_reply;
        respBase->d_reserved = 0;
        respBase->d_addressType = connectBase.d_addressType;

        bool sendResponse = true;
        bteso_IPv4Address connectAddr;
        bteso_Endpoint destination(alloc);
        if (1 == connectBase.d_addressType) {
            Socks5ConnectBody1 body1;
            rc = clientSocket->read((char *)&body1, sizeof(body1));
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            BSLS_ASSERT(!args->d_expectedIp
                    || body1.d_ip == args->d_expectedIp);
            BSLS_ASSERT(!args->d_expectedPort
                    || body1.d_port == args->d_expectedPort);
            connectAddr.setIpAddress(body1.d_ip);
            connectAddr.setPortNumber(body1.d_port);
            LOG_DEBUG << "connect addr=" << connectAddr
                      << LOG_END;

            Socks5ConnectResponse1
                *resp = (Socks5ConnectResponse1 *)respBuffer;
            memcpy(&resp->d_ip, &body1.d_ip, sizeof(resp->d_ip));
            memcpy(&resp->d_port, &body1.d_port, sizeof(resp->d_port));
            respLen = sizeof(Socks5ConnectResponse1);
        } else if (3 == connectBase.d_addressType) {
            char hostLen;
            rc = clientSocket->read(&hostLen, 1);
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            char hostBuffer[256];
            rc = clientSocket->read(hostBuffer, hostLen);
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            bdeut_BigEndianInt16 port;
            rc = clientSocket->read((char *)&port, sizeof(port));
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            unsigned short nativePort = (short) port;
            destination.set(bsl::string(hostBuffer, hostLen), nativePort);
            LOG_DEBUG << "connect addr=" << destination << LOG_END;
            BSLS_ASSERT(!args->d_expectedDestination.isSet()
                    || args->d_expectedDestination == destination);

            Socks5ConnectResponse3 *resp = (Socks5ConnectResponse3 *)respBuffer;
            resp->d_hostLen = hostLen;
            memcpy(&resp->d_data[0], hostBuffer, hostLen);
            memcpy(&resp->d_data[hostLen], &port, sizeof(port));
            respLen = sizeof(Socks5ConnectResponse3) - 1
                + hostLen + sizeof(port);
        } else {
            LOG_ERROR << "unsupported request address type "
                      << connectBase.d_addressType << ", closing"
                      << LOG_END;
            BSLS_ASSERT(false);
            continue;
        }

        if (btes5_TestServerArgs::e_FAIL == args->d_mode) {
            unsigned char reply = 1; // general SOCKS server failure
            if (args->d_reply) {
                reply = args->d_reply;
            }
            LOG_DEBUG << "sending error code " << reply
                      << " and closing" << LOG_END;
            respBase->d_reply = reply;
            rc = clientSocket->write((char *)respBuffer, respLen);
            BSLS_ASSERT(rc > 0);
            continue;
        }

        if (btes5_TestServerArgs::e_SUCCEED_AND_CLOSE == args->d_mode) {
            LOG_DEBUG << "sending success and closing" << LOG_END;
            rc = clientSocket->write((char *)respBuffer, respLen);
            BSLS_ASSERT(rc > 0);
            continue;
        }

        if (btes5_TestServerArgs::e_CONNECT == args->d_mode) {
            bteso_StreamSocket<bteso_IPv4Address>
                *connectSocket = factory->allocate();
            BSLS_ASSERT(connectSocket);
            bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
                socketGuard(connectSocket, factory.ptr());

            if (args->d_destination.isSet()) {
                destination = args->d_destination;
            }
            if (destination.isSet()) {
                int ret = bteso_ResolveUtil::getAddress(
                                               &connectAddr,
                                               destination.hostname().c_str());
                BSLS_ASSERT(0 == ret);
                connectAddr.setPortNumber(destination.port());
            }

            int rc = connectSocket->connect(connectAddr);
            if (0 == rc) {
                rc = clientSocket->write((char *)respBuffer, respLen);
                BSLS_ASSERT(rc > 0);
                if (rc <= 0) {
                    break;
                }
                LOG_DEBUG << "Connected to " << connectAddr
                          << "; wrote ResponseBase (" << respLen << " bytes)"
                          << LOG_END;

                // Create a relay loop by using timed reads. The relay listens
                // to each side in turn for 100 milliseconds, and writes the
                // receieved data, if any, to the opposite side.

                btesos_TcpTimedChannel clientChannel(clientSocket);
                btesos_TcpTimedChannel connectChannel(connectSocket);
                char buf[256];
                bdet_TimeInterval timeout;
                for (;;) {
                    timeout = bdetu_SystemTime::now();
                    timeout.addMilliseconds(100);
                    int rc = clientChannel.timedRead(buf, sizeof buf, timeout);
                    if (rc < 0) {
                        break; // socket closed or failed
                    }
                    if (rc > 0) {
                        connectChannel.write(buf, rc);
                    }

                    timeout = bdetu_SystemTime::now();
                    timeout.addMilliseconds(100);
                    rc = connectChannel.timedRead(buf, sizeof buf, timeout);
                    if (rc < 0) {
                        break; // socket closed or failed
                    }
                    if (rc > 0) {
                        clientChannel.write(buf, rc);
                    }
                }
            } else {
                LOG_ERROR << "Connect to " << connectAddr
                          << " failed: " << rc << LOG_END;
            }
        }
    }
    sleep(1);

    LOG_DEBUG << "ServerThread ending." << LOG_END;
}

static int createServerThread(
        bcemt_ThreadUtil::Handle              *threadHandle,
        bcema_SharedPtr<btes5_TestServerArgs>  args,
        bteso_Endpoint                        *proxy,
        bslma::Allocator                      *allocator)
    // Create a thread that implements a SOCKS5 server configured per the
    // specified 'args', and load its identifier into the specified
    // 'threadHandle' and the server's IP adddress into the specified 'proxy',
    // using the specified 'allocator' to supply memory. Return 0 for success
    // and a non-zero value on error. Note that the server thread will be
    // "detached" as defined in 'bcemt_threadutil'.
{
    bteso_IPv4Address serverAddress("127.0.0.1", 0);

    bcema_SharedPtr<bteso_StreamSocketFactory<bteso_IPv4Address> > factory(
        new (*allocator)
            bteso_InetStreamSocketFactory<bteso_IPv4Address>(allocator));
    bteso_StreamSocket<bteso_IPv4Address> *socket = factory->allocate();
    if (!socket) {
        return -1;                                                    // RETURN
    }
    int rc;
    bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
        socketGuard(socket, factory.ptr());

    rc = socket->setOption(bteso_SocketOptUtil::SOCKETLEVEL,
                           bteso_SocketOptUtil::REUSEADDRESS,
                           1);
    BSLS_ASSERT(0 == rc);

    rc = socket->bind(serverAddress);
    BSLS_ASSERT(0 == rc);

    bteso_IPv4Address localAddress;
    rc = socket->localAddress(&localAddress);
    BSLS_ASSERT(0 == rc);
    proxy->set("127.0.0.1", localAddress.portNumber());

    if (args->d_label.empty()) {
        bsl::ostringstream s(allocator);
        s << *proxy;
        args->d_label = s.str();  // default label is proxy address
    }

    rc = socket->setBlockingMode(bteso_Flag::BLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    rc = socket->listen(1);
    BSLS_ASSERT(0 == rc);

    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

    socketGuard.release(); // the socket will be managed by the new thread
    bcemt_ThreadUtil::Invokable
        server = bdef_BindUtil::bindA(allocator,
                                      &ServerThread,
                                      args,
                                      factory,
                                      socket,
                                      allocator);
    bcemt_ThreadUtil::create(threadHandle, attributes, server);

    return 0;
}
*/

                             // ===================
                             // class Socks5Session
                             // ===================

struct Socks5Session : public btemt_Session {
    // A concrete implementation of 'btemt_Session'.  This is an adapter to a
    // 'SessionFactory' object that actually implements the SOCKS5 proxy.
    // A 'Socks5Session' can represent one of two connections from a proxy: a
    // client connection (which is used for negotiation), or a destination
    // connection.

    // DATA
    btes5_TestServer::SessionFactory * const d_factory;
        // factory managing this session, not owned

    const bool d_client;  // 'true' iff this is a proxy client connection

  private:
    // NOT IMPLEMENTED
    Socks5Session(const Socks5Session&);
    Socks5Session& operator=(const Socks5Session&);

  public:
    // CREATORS
    Socks5Session(btes5_TestServer::SessionFactory *factory,
                  bool                              client);
         // Create a new 'Socks5Session' managed by the specified 'factory'
         // that represents the client connection if the specified 'client' is
         // true, and the destination connection otherwise.

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
    // (such as pooling) is implemented.  An object of this type supports only
    // one proxy connection at a time; multiple proxy hosts can be supported by
    // multiple objects.
    //
    // A 'SessionFactory' constructs a 'btemt_SessionPool' to manage IO events,
    // and allocates (when requested by the session pool) sessions which serve
    // as adapters to the 'SessionFactory' that allocated them.
    //
    // A 'SessionFactory' object maintains two connections.  One is accepted,
    // and is used for client connection SOCKS5 negotiation.  The other is to
    // the destination, to which the proxy tries to connect after the
    // negotiation is concluded.  Each connection corresponds to a
    // 'btemt_Session' object, but the actual IO is done by the allocating
    // 'SessionFactory' through two 'btemt_AsynChannel' objects.  Since these
    // channels can be modified asynchronously, they are protected by mutexes.

    const btes5_TestServerArgs&  d_args;  // arguments controlling the proxy

    btemt_AsyncChannel          *d_client_p;  // client channel, not owned
    btemt_AsyncChannel          *d_dst_p;     // destination channel, not owned

    bcemt_Mutex d_clientLock;  // serialize access to 'd_client_p'
    bcemt_Mutex d_dstLock;     // serialize access to 'd_dst_p'

    bcema_SharedPtr<btemt_SessionPool> d_sessionPool;  // managed pool

    bslma::Allocator     *d_allocator_p; // memory allocator (held, not owned)

    // PRIVATE MANIPULATORS

             // btemt_AsyncChannel interface

    template<typename MSG>
    int readMessage(void (SessionFactory::*func)(const MSG *data,
                                                int      length,
                                                int     *numConsumed,
                                                int     *numNeeded));
        // Queue up reading a message of the specified type 'MSG', and arrange to
        // invoke the specified 'func' when it's received.

    template<typename MSG>
    void readMessageCb(int                   result,
                       int                  *consumed,
                       int                  *needed,
                       const btemt_DataMsg&  msg,
                       void (SessionFactory::*func)(const MSG *data,
                                                   int      length,
                                                   int     *numConsumed,
                                                   int     *numNeeded));
        // If the specified 'result' is 'BTEMT_SUCCESS' invoke the specified
        // 'func' passing it the data from the specified 'msg' as the specified
        // template type 'MSG'.

    int clientWrite(const char *buf, int length);
        // Send the specified 'buf' of the specified 'length' bytes to the
        // client using 'd_channel_p'.  Return 0 on success, and a non-zero
        // value otherwise.

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
                   const btemt_DataMsg&  msg,
                   bool                  fromClient);
        // If the specified 'result' is 'BTEMT_SUCCESS' forward the specified
        // 'msg' to the opposite connection per the specified 'fromClient',
        // updating the counters at the specified 'consumed' location to
        // reflect the bytes forwarded, and the specified 'needed' location to
        // request (at least) one more byte.  This method actually implements
        // the proxy after negotiation and connection have been completed.

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
                        void          *userData,
                        bool           client);
        // Process the session state change for the specified 'state',
        // 'handle', 'session', 'userData', and 'client'.  If 'client' is
        // 'true' the session represents the client connection; otherwise, it
        // represents the (attempted) connection to the destination.

                      // 'btemt_SessionFactory' interface

    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
       // Asynchronously allocate a 'btemt_Session' object for the
       // specified 'channel', and invoke the specified 'callback' with
       // this session.

    virtual void deallocate(btemt_Session *session);
       // Deallocate the specified 'session'.

                          // 'btemt_Session' interface

     int start(bool client);
         // Begin the asynchronous operation on the client connection if the
         // specified 'client' is true, or on the destination connection
         // otherwise.

     int stop(bool client);
         // Stop operations on the client connection if the specified 'client'
         // is true, or on the destination connection otherwise.

    // ACCESSORS
    btemt_AsyncChannel *channel(bool client) const;
        // Return the communication channel to the client, if the specified
        // 'client' is true, and to the destination otherwise.
};

                        // --------------------
                        // class Socks5Session
                        // --------------------

// CREATORS
Socks5Session::Socks5Session(btes5_TestServer::SessionFactory *factory,
                             bool                              client)
: d_factory(factory)
, d_client(client)
{
}

Socks5Session::~Socks5Session()
{
}

// MANIPULATORS
int Socks5Session::start()
{
    return d_factory->start(d_client);
    return 0;
}

int Socks5Session::stop()
{
    return d_factory->stop(d_client);
}

// ACCCESSORS
btemt_AsyncChannel* BloombergLP::Socks5Session::channel() const
{
    return d_factory->channel(d_client);
}

                    // --------------------------------------
                    // class btes5_TestServer::SessionFactory
                    // --------------------------------------

// PRIVATE MANIPULATORS
template<typename MSG>
int btes5_TestServer::SessionFactory::readMessage(
    void (SessionFactory::*func)(const MSG *data,
                                int      length,
                                int     *numConsumed,
                                int     *numNeeded))
{
    using namespace bdef_PlaceHolders;
    btemt_AsyncChannel::ReadCallback cb
        = bdef_BindUtil::bindA(d_allocator_p,
                               &SessionFactory::readMessageCb<MSG>,
                               this,
                               _1,
                               _2,
                               _3,
                               _4,
                               func);
    int rc = -1;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
        if (d_client_p) {
            rc = d_client_p->read(sizeof(MSG), cb);
        }
    }
    return rc;
}

template<typename MSG>
void btes5_TestServer::SessionFactory::readMessageCb(
                                  int                   result,
                                  int                  *consumed,
                                  int                  *needed,
                                  const btemt_DataMsg&  msg,
                                  void (SessionFactory::*func)(
                                                        const MSG *data,
                                                        int        length,
                                                        int       *numConsumed,
                                                        int       *numNeeded))
{
    if (btemt_AsyncChannel::BTEMT_SUCCESS == result) {
        const int length = msg.data()->length();
        char data[length];
        msg.data()->copyOut(data, length, 0);
        (this->*func)(reinterpret_cast<MSG*>(data), length, consumed, needed);
    } else {
        LOG_ERROR << "async read failed, result " << result << LOG_END;
        stop(true);
        BSLS_ASSERT(false);
    }
}

int btes5_TestServer::SessionFactory::clientWrite(const char *buf, int length)
{
    bcema_SharedPtr<char>
        buffer(reinterpret_cast<char*>(d_allocator_p->allocate(length)),
               d_allocator_p);
    bsl::memcpy(buffer.ptr(), buf, length);
    bcema_BlobBuffer blobBuffer(buffer, length);

    bcema_Blob blob(d_allocator_p);
    blob.prependDataBuffer(blobBuffer);

    int rc = 0;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
        if (d_client_p) {
            rc = d_client_p->write(blob);
        }
    }
    return rc;
}

void btes5_TestServer::SessionFactory::readIgnore(const unsigned char *data,
                               int                  length,
                               int                 *consumed,
                               int                 *needed)
{
    UNUSED(data);
    UNUSED(length);
    *consumed = 1;
    *needed = 1;  // keep reading one byte at a time
}

void btes5_TestServer::SessionFactory::readMethods(
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
        readMessage(&SessionFactory::readCredentials);
    } else {
        method = 0;  // method: no authentication
        BSLS_ASSERT(method == supportedMethods[0]);
        readMessage(&SessionFactory::readConnect);
    }

    Socks5MethodResponse methodResponse(method);
    int rc = clientWrite((char *)&methodResponse, sizeof(methodResponse));
    if (rc) {
        stop(true);
        BSLS_ASSERT(false);
    } else {
        LOG_DEBUG << "Wrote MethodResponse(method = " << method << ")"
                  << LOG_END;
    }

}

void btes5_TestServer::SessionFactory::readCredentials(
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
        readMessage(&SessionFactory::readConnect);
    } else {
        LOG_ERROR << "authentication failure:"
                  << " expected " << d_args.d_expectedCredentials
                  << " recieved " << requestCredentials
                  << LOG_END;
        response[1] = 1;
        int rc = clientWrite(response, sizeof(response));
        if (rc) {
            stop(true);
            BSLS_ASSERT(false);
        }
    }
}

void btes5_TestServer::SessionFactory::readConnect(const Socks5ConnectBase *data,
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
    respBase->d_reply = d_args.d_reply;
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
        resp->d_hostLen = hostLen;
        memcpy(&resp->d_data[0], hostBuffer, hostLen);
        memcpy(&resp->d_data[hostLen], &port, sizeof(port));
        respLen = sizeof(Socks5ConnectResponse3) - 1 + hostLen + sizeof(port);
    } else {
        LOG_ERROR << "unsupported request address type "
                  << data->d_addressType << ", closing"
                  << LOG_END;
        BSLS_ASSERT(false);
        stop(true);
    }

    if (btes5_TestServerArgs::e_FAIL == d_args.d_mode) {
        unsigned char reply = 1; // general SOCKS server failure
        if (d_args.d_reply) {
            reply = d_args.d_reply;
        }
        LOG_DEBUG << "sending error code " << reply
                  << " and closing" << LOG_END;
        respBase->d_reply = reply;
        int rc = clientWrite((char *)respBuffer, respLen);
        if (rc) {
            stop(true);
            BSLS_ASSERT(false);
        }
    }

    if (btes5_TestServerArgs::e_SUCCEED_AND_CLOSE == d_args.d_mode) {
        LOG_DEBUG << "sending success and closing" << LOG_END;
        int rc = clientWrite((char *)respBuffer, respLen);
        if (rc) {
            stop(true);
            BSLS_ASSERT(false);
        }
    }

    if (btes5_TestServerArgs::e_CONNECT == d_args.d_mode) {
        if (d_args.d_destination.isSet()) {
            destination = d_args.d_destination;
        }
        if (destination.isSet()) {
            int handle;

            using namespace bdef_PlaceHolders;
            btemt_SessionPool::SessionStateCallback cb
                = bdef_BindUtil::bindA(d_allocator_p,
                                       &SessionFactory::sessionStateCb,
                                       this,
                                       _1,
                                       _2,
                                       _3,
                                       _4,
                                       false);

/*
bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory(
    new bteso_InetStreamSocketFactory<bteso_IPv4Address>);
bteso_StreamSocket<bteso_IPv4Address>
    *connectSocket = socketFactory->allocate();
BSLS_ASSERT(connectSocket);

if (destination.isSet()) {
    int ret = bteso_ResolveUtil::getAddress(
                                   &connectAddr,
                                   destination.hostname().c_str());
    BSLS_ASSERT(0 == ret);
    connectAddr.setPortNumber(destination.port());
}

int rc = connectSocket->connect(connectAddr);
BSLS_ASSERT(!rc);
rc = d_sessionPool->import(&handle, cb, connectSocket, socketFactory, this);
BSLS_ASSERT(!rc);
*/

            const int numAttempts = 3;
            bdet_TimeInterval interval(0.1);
            int rc = d_sessionPool->connect(&handle,
                                            cb,
                                            destination.hostname().c_str(),
                                            destination.port(),
                                            numAttempts,
                                            interval,
                                            this);
            if (rc) {
                LOG_ERROR << "cannot initiate connection to " << destination
                          << ", rc" << rc << LOG_END;
                BSLS_ASSERT(false);
            }

        }

    }
}

void btes5_TestServer::SessionFactory::readProxy(
                   int                   result,
                   int                  *consumed,
                   int                  *needed,
                   const btemt_DataMsg&  msg,
                   bool                  fromClient)
{
    if (btemt_AsyncChannel::BTEMT_SUCCESS == result) {
        const int length = msg.data()->length();
        int rc = -1;
        if (fromClient) {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_dstLock);
            if (d_dst_p) {
                rc = d_dst_p->write(msg);
            }
        } else {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
            if (d_client_p) {
                rc = d_client_p->write(msg);
            }
        }
        if (rc) {
            LOG_ERROR << "cannot forward " << length << " bytes, rc " << rc
                      << LOG_END;
            BSLS_ASSERT(false);
        }
        *consumed = length;
        *needed = 1;
    } else {
        LOG_ERROR << "read failed, result " << result << LOG_END;
        BSLS_ASSERT(false);
    }
}

                        // 'btemt_SessionPool' callbacks

void btes5_TestServer::SessionFactory::sessionStateCb(int            state,
                                                      int            handle,
                                                      btemt_Session *session,
                                                      void          *userData,
                                                      bool           client)
{
    UNUSED(handle);
    UNUSED(userData);

    bsl::ostringstream peer(d_allocator_p);  // address for diagnostics
    if (client) {
        peer << "Client";
        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
            if (d_client_p) {
                peer << " from " << d_client_p->peerAddress();
            }
        }
        switch (state) {
          case btemt_SessionPool::SESSION_DOWN: {
              LOG_DEBUG << peer.str() << " has disconnected" << LOG_END;
            stop(true);
          } break;
          case btemt_SessionPool::SESSION_UP: {
              LOG_DEBUG << peer.str() << " connected" << LOG_END;
          } break;
          default: {
            LOG_ERROR << peer.str() << ": unexpected state " << state
                      << LOG_END;
            BSLS_ASSERT(false);
          } break;
        }
    } else {
        peer << "Destination";
        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_dstLock);
            if (d_dst_p) {
                peer << " " << d_dst_p->peerAddress();
            }
        }
        switch (state) {
          case btemt_SessionPool::SESSION_UP: {
            LOG_DEBUG << "Connected to " << peer.str() << LOG_END;
            Socks5ConnectResponse1 response;  // d_reply = SUCCESS by default
            clientWrite((char *) &response, sizeof(response));
          } break;
          default: {
            LOG_ERROR << "Connection to " << peer.str() << " failed, state "
                      << state << LOG_END;
            Socks5ConnectResponse1 response;
            response.d_base.d_reply = 4;  // Host unreachable
            clientWrite((char *) &response, sizeof(response));
          } break;
        }
    }
}

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
, d_client_p(0)
, d_dst_p(0)
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
                               _4,
                               true);

    BSLS_ASSERT(0 == d_sessionPool->start());
    int handle;
    BSLS_ASSERT(0 == d_sessionPool->listen(&handle,
                                           cb,
                                           0,         // let system assign port
                                           1,         // backlog
                                           1,         // REUSEADDR
                                           this));

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
void
btes5_TestServer::SessionFactory::allocate(
    btemt_AsyncChannel                    *channel,
    const btemt_SessionFactory::Callback&  callback)
{
    bool client;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
        if (d_client_p) {
            client = false;
            d_dst_p = channel;
        } else {
            client = true;
            d_client_p = channel;
        }
    }
    LOG_DEBUG << "allocate a "
              << (client ? "client" : "destination") << " session" << LOG_END;
    Socks5Session *session = new (*d_allocator_p) Socks5Session(this, client);
    callback(0, session);
}

void
btes5_TestServer::SessionFactory::deallocate(btemt_Session *session)
{
    Socks5Session *s = dynamic_cast<Socks5Session *>(session);
    BSLS_ASSERT(s);
    d_allocator_p->deleteObjectRaw(session);
    LOG_DEBUG << "deallocated "
              << (s->d_client ? "client" : "destination") << " session"
              << LOG_END;
}

                          // 'btemt_Session' interface

int btes5_TestServer::SessionFactory::start(bool client)
{
    if (client) {
        // client connection: start negotiation

        if (btes5_TestServerArgs::e_IGNORE == d_args.d_mode) {
            readMessage(&SessionFactory::readIgnore);
        } else {
            readMessage(&SessionFactory::readMethods);
        }
    } else {
        // start proxying by forwarding received data to the opposite side

        using namespace bdef_PlaceHolders;

        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
            if (d_client_p) {
                btemt_AsyncChannel::ReadCallback cb
                    = bdef_BindUtil::bindA(d_allocator_p,
                                           &SessionFactory::readProxy,
                                           this,
                                           _1,
                                           _2,
                                           _3,
                                           _4,
                                           true);
                d_client_p->read(1, cb);
            }
        }
        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_dstLock);
            if (d_dst_p) {
                btemt_AsyncChannel::ReadCallback cb
                    = bdef_BindUtil::bindA(d_allocator_p,
                                           &SessionFactory::readProxy,
                                           this,
                                           _1,
                                           _2,
                                           _3,
                                           _4,
                                           false);
                d_dst_p->read(1, cb);
            }
        }

    }
    return 0;
}

int btes5_TestServer::SessionFactory::stop(bool client)
{
    if (client) {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_clientLock);
        if (d_client_p) {
            d_client_p->close();
            d_client_p = 0;
        }
    } else {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_dstLock);
        if (d_dst_p) {
            d_dst_p->close();
            d_dst_p = 0;
        }
    }
    return 0;
}

// ACCCESSORS
btemt_AsyncChannel *
btes5_TestServer::SessionFactory::channel(bool client) const
{
    return client ? d_client_p : d_dst_p;
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
