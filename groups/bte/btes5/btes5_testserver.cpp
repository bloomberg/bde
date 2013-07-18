// btes5_testserver.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_testserver_cpp, "$Id$ $CSID$")

#include <btes5_testserver.h>

#include <bcema_sharedptr.h>
#include <bcemt_thread.h>                   // thread management util
#include <bdef_bind.h>
#include <bsls_atomic.h>

#include <bsl_iostream.h>

namespace BloombergLP {

namespace {
// TODO: more flexible way of diagnostics?
#define LOG_DEBUG bsl::cout
#define LOG_ERROR bsl::cout
#define LOG_END bsl::endl

static int globalNumSuccessfulConnections = 0;
static bool globalDone = false;

const int CONN_PORT = 12345;

struct Socks5MethodRequest
{
    char d_version;
    char d_numMethods;
    char d_methods[255]; // max. 255 methods, only first 2 used

    //Socks5MethodRequest()
    //: d_version(5), d_numMethods(1), d_method(0) { }
};

struct Socks5MethodResponse
{
    char d_version;
    char d_method;

    Socks5MethodResponse()
    : d_version(5), d_method(0) { }
};

struct Socks5ConnectBase
{
    char    d_version;
    char    d_command;
    char    d_reserved;
    char    d_addressType;

    //Socks5ConnectBase()
    //: d_version(5), d_command(1), d_reserved(0) { }
};

struct Socks5ConnectBody1
{
    bdeut_BigEndianInt32    d_ip;
    bdeut_BigEndianInt16    d_port;
};

struct Socks5ConnectToIPv4Address
{
    char    d_version;
    char    d_command;
    char    d_reserved;
    char    d_addressType;
    char    d_address[4];
    short   d_port;

    //Socks5ConnectToIPv4Address()
    //: d_version(5), d_command(1), d_reserved(0), d_addressType(1) { }
};

struct Socks5ConnectResponseBase
{
    char    d_version;
    char    d_reply;
    char    d_reserved;
    char    d_addressType;

    Socks5ConnectResponseBase()
    : d_version(5), d_reply(0), d_reserved(0), d_addressType(1) { }
};

struct Socks5ConnectResponse1
{
    Socks5ConnectResponseBase d_base;
    int     d_ip;
    short   d_port;

    Socks5ConnectResponse1(int ip, short port)
    : d_ip(ip), d_port(port) { }
};

struct Socks5ConnectResponse3
{
    Socks5ConnectResponseBase d_base;
    char d_hostLen;
    char d_data[1];
};

static void ServerThread(
    bcema_SharedPtr<btes5_TestServerArgs>                          args,
    bcema_SharedPtr<bteso_StreamSocketFactory<bteso_IPv4Address> > factory,
    bteso_StreamSocket<bteso_IPv4Address>                         *socket)
    // Create a server thread to accept 1 connection and then simulate
    // a socks5 server responding to a connect request. This thread will
    // terminate after either a successful or failure condition.
    // Create a server socket which is ready for accept connections.
    // The configuration for this thread will be passed in through the
    // specified 'arg':
    //   a) d_connection_p - socket to hold accepted connection
    //   b) d_reply - what to reply the connect request with
    //   c) d_status - successful or failure state
{
    LOG_DEBUG << "ServerThread starting." << LOG_END;


    for (int i = 0; i < 1; ++i) {
        int s = socket->accept(&args->d_connection_p);
        BSLS_ASSERT(0 == s);
        if (0 != s) {
            break;
        }

        args->d_status = 0;
        LOG_DEBUG << "Accepted connection." << LOG_END;
        args->d_connection_p->setBlockingMode(bteso_Flag::BLOCKING_MODE);
        Socks5MethodRequest methodRequest;
        int rc;
        rc = args->d_connection_p->read((char *)&methodRequest,
                                        sizeof(methodRequest));
        BSLS_ASSERT(rc > 0);
        if (rc <= 0) {
            break;
        }
        LOG_DEBUG << "Read MethodRequest"
                  << " version=" << (int) methodRequest.d_version
                  << " numMethods=" << (int) methodRequest.d_numMethods
                  << " methods[0]=" << (int) methodRequest.d_methods[0]
                  << LOG_END;
        BSLS_ASSERT(5 == methodRequest.d_version);
        BSLS_ASSERT(1 <= methodRequest.d_numMethods);
        BSLS_ASSERT(0 == methodRequest.d_methods[0]);

        Socks5MethodResponse methodResponse;
        rc = args->d_connection_p->write((char *)&methodResponse,
                                         sizeof(methodResponse));
        BSLS_ASSERT(rc > 0);
        if (rc <= 0) {
            break;
        }
        LOG_DEBUG << "Wrote MethodResponse" << LOG_END;

        Socks5ConnectBase connectBase;
        rc = args->d_connection_p->read((char *)&connectBase,
                                        sizeof(connectBase));
        if (rc <= 0) {
            LOG_ERROR << "read connect request failed, rc " << rc
                            << LOG_END;
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
        unsigned int respLen = 0;
        Socks5ConnectResponseBase *respBase =
                (Socks5ConnectResponseBase *) respBuffer;

        respBase->d_version = 5;
        respBase->d_reply = args->d_reply;
        respBase->d_reserved = 0;
        respBase->d_addressType = connectBase.d_addressType;

        bool sendResponse = true;
        if (1 == connectBase.d_addressType) {
            Socks5ConnectBody1 body1;
            rc = args->d_connection_p->read((char *)&body1, sizeof(body1));
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            BSLS_ASSERT(body1.d_ip == args->d_expectedIp);
            BSLS_ASSERT(body1.d_port == args->d_expectedPort);
            bteso_IPv4Address connectAddr(body1.d_ip, body1.d_port);
            LOG_DEBUG << "connect addr=" << connectAddr << LOG_END;

            Socks5ConnectResponse1 *resp = (Socks5ConnectResponse1 *)respBuffer;
            memcpy(&resp->d_ip, &body1.d_ip, sizeof(resp->d_ip));
            memcpy(&resp->d_port, &body1.d_port, sizeof(resp->d_port));
            respLen = sizeof(Socks5ConnectResponse1);
        } else if (3 == connectBase.d_addressType) {
            char hostLen;
            rc = args->d_connection_p->read(&hostLen, 1);
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            char hostBuffer[256];
            rc = args->d_connection_p->read(hostBuffer, hostLen);
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            bdeut_BigEndianInt16 port;
            rc = args->d_connection_p->read((char *)&port, sizeof(port));
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            bteso_Endpoint destination(bsl::string(hostBuffer, hostLen), port);
            LOG_DEBUG << "connect addr=" << destination << LOG_END;

            BSLS_ASSERT(port == args->d_expectedPort);

            Socks5ConnectResponse3 *resp = (Socks5ConnectResponse3 *)respBuffer;
            resp->d_hostLen = hostLen;
            memcpy(&resp->d_data[0], hostBuffer, hostLen);
            memcpy(&resp->d_data[hostLen], &port, sizeof(port));
            respLen = sizeof(Socks5ConnectResponse3) + hostLen + sizeof(port);
        } else {
            sendResponse = false;
        }

        if (args->d_ignoreConnectAttempts) {
            sendResponse = false;
        }

        if (sendResponse) {
            rc = args->d_connection_p->write((char *)respBuffer, respLen);
            BSLS_ASSERT(rc > 0);
            if (rc <= 0) {
                break;
            }
            LOG_DEBUG << "Wrote ResponseBase" << LOG_END;
        }
    }
    sleep(1);
    factory->deallocate(socket);

    LOG_DEBUG << "ServerThread ending." << LOG_END;
}

static int createServerThread(
        bcemt_ThreadUtil::Handle              *threadHandle,
        bcema_SharedPtr<btes5_TestServerArgs>  args,
        bteso_Endpoint                        *proxy)
{
    bteso_IPv4Address serverAddress("127.0.0.1", 0);

    bcema_SharedPtr<bteso_StreamSocketFactory<bteso_IPv4Address> >
        factory(new bteso_InetStreamSocketFactory<bteso_IPv4Address>);
    bteso_StreamSocket<bteso_IPv4Address> *socket = factory->allocate();
    BSLS_ASSERT(0 == socket->setOption(bteso_SocketOptUtil::SOCKETLEVEL,
                                       bteso_SocketOptUtil::REUSEADDRESS,
                                       1));
    BSLS_ASSERT(0 == socket->bind(serverAddress));

    bteso_IPv4Address localAddress;
    BSLS_ASSERT(0 == socket->localAddress(&localAddress));
    proxy->set("127.0.0.1", localAddress.portNumber());

    BSLS_ASSERT(0 == socket->setBlockingMode(bteso_Flag::BLOCKING_MODE));
    BSLS_ASSERT(0 == socket->listen(1));

    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

    bcemt_ThreadUtil::Invokable
        server = bdef_BindUtil::bind(&ServerThread, args, factory, socket);
    bcemt_ThreadUtil::create(threadHandle, attributes, server);

    return 0;
}

}  // close anonymous namespace

                         // ---------------------------
                         // struct btes5_TestServerArgs
                         // ---------------------------
btes5_TestServerArgs::btes5_TestServerArgs(int numWaitThreads)
: d_connection_p(0)
, d_reply(0)
, d_status(-99)
, d_ignoreConnectAttempts(false)
{
    d_expectedIp = 0x7f000001;
    d_expectedPort = CONN_PORT;
}

                        // ----------------------
                        // class btes5_TestServer
                        // ----------------------

// CREATORS
btes5_TestServer::btes5_TestServer(bteso_Endpoint             *proxy,
                                   const btes5_TestServerArgs *userArgs)
{
    BSLS_ASSERT(0 == bteso_SocketImpUtil::startup());

    bcema_SharedPtr<btes5_TestServerArgs> args(new btes5_TestServerArgs);
    if (userArgs) {
        *args = *userArgs;
    }
    bcemt_ThreadUtil::Handle threadHandle;

    // create server thread to accept connections
    BSLS_ASSERT(0 == createServerThread(&threadHandle, args, proxy));
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
