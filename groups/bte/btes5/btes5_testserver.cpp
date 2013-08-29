// btes5_testserver.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_testserver_cpp, "$Id$ $CSID$")

#include <btes5_testserver.h>

#include <bcema_sharedptr.h>
#include <bcemt_thread.h>                   // thread management util
#include <bdef_bind.h>
#include <bdet_timeinterval.h>
#include <bsl_iostream.h>
#include <bslma_default.h>
#include <bsls_atomic.h>
#include <bsl_sstream.h>
#include <bteso_resolveutil.h>
#include <btesos_tcptimedchannel.h>

namespace BloombergLP {

namespace {

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

struct Socks5ConnectBase {
    unsigned char    d_version;
    unsigned char    d_command;
    unsigned char    d_reserved;
    unsigned char    d_addressType;

    //Socks5ConnectBase()
    //: d_version(5), d_command(1), d_reserved(0) { }
};

struct Socks5ConnectBody1 {
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
    int                       d_ip;  // should change to unsigned char[4]
    unsigned short            d_port;

    Socks5ConnectResponse1(int ip, int port)
    : d_ip(ip), d_port(port) { }
};

struct Socks5ConnectResponse3 {
    Socks5ConnectResponseBase d_base;
    unsigned char d_hostLen;
    unsigned char d_data[1];
};

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
#define LOG_STREAM(severity, args) {                       \
    if (severity <= (args)->d_verbosity) {                 \
        *(args)->d_logStream_p << (args)->d_label << ": "; \
        *(args)->d_logStream_p

#define LOG_END bsl::endl; \
    }                      \
}

#define LOG_DEBUG LOG_STREAM(btes5_TestServerArgs::e_DEBUG, args)
#define LOG_ERROR LOG_STREAM(btes5_TestServerArgs::e_ERROR, args)

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
#undef LOG_STREAM
#undef LOG_END
#undef LOG_ERROR
#undef LOG_DEBUG
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

}  // close anonymous namespace

                         // ---------------------------
                         // struct btes5_TestServerArgs
                         // ---------------------------
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
                                   const btes5_TestServerArgs *userArgs,
                                   bslma::Allocator           *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(0 == bteso_SocketImpUtil::startup());

    bcema_SharedPtr<btes5_TestServerArgs> args(new (*d_allocator_p)
                                                   btes5_TestServerArgs);
    if (userArgs) {
        *args = *userArgs;
    }
    bcemt_ThreadUtil::Handle threadHandle;

    // create server thread to accept connections
    int rc = createServerThread(&threadHandle, args, proxy, d_allocator_p);
    BSLS_ASSERT(0 == rc);
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
