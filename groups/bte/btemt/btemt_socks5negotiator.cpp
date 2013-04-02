// btemt_socks5negotiator.cpp               -*-C++-*-
#include <btemt_socks5negotiator.h>

#include <btemt_blobutil.h>
#include <btemt_log.h>
#include <btemt_socksconfiguration.h>
#include <btemt_pack.h>
#include <btemt_packedbigendian.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>
#include <bdeut_bigendian.h>
#include <bdeut_stringref.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <btemt_asyncchannel.h>
#include <bteso_resolveutil.h>

namespace BloombergLP {
namespace btemt {

namespace {

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

#pragma UICT_PACK1
struct MethodRequestPkt {
    char d_ver;
    char d_nmethods;
    char d_methods[2];
};

struct MethodResponsePkt {
    char d_ver;
    char d_method;
};

struct AuthenticationResponsePkt {
    char d_ver;
    char d_status;
};

struct ConnectBase {
    char d_ver;
    char d_cmd;
    char d_rsv;
    char d_atype;
};

struct ConnectRspBase {
    char d_ver;
    char d_rep;
    char d_rsv;
    char d_atype;
};

struct ConnectIPv4AddressRequestPkt {
    unsigned int               d_address;
    PackedBigEndianUint16 d_port;
};

struct ConnectIPv4AddressResponsePkt {
    unsigned int d_bndAddr;
    PackedBigEndianUint16 d_bndPort;
};
#pragma UICT_UNPACK

}  // close anonymous namespace

                        // ----------------------
                        // class Socks5Negotiator
                        // ----------------------

// PRIVATE MANIPULATORS
int Socks5Negotiator::sendMethodRequest()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.sendMethodRequest");

    MethodRequestPkt pkt;
    pkt.d_ver        = VERSION;
    pkt.d_nmethods   = 2;
    pkt.d_methods[0] = NONE;
    pkt.d_methods[1] = PASSWORD;

    int length = sizeof(pkt);
    if (!d_configuration.doAuthentication()) {
        // Don't offer PASSWORD authentication unless the configuration tells
        // us to.
        --pkt.d_nmethods;
        length -= sizeof(pkt.d_methods[1]);
    }
    bcema_Blob blob(d_blobBufferFactory_p, d_allocator_p);
    bcema_BlobUtil::append(&blob, (char*)&pkt, length);

    int rc = d_channel_p->write(blob);
    if (rc) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED;
        BTEMT_LOG_ERROR << "Error writing method request, rc=" << rc
                       << BTEMT_LOG_END;
        return -1;
    }

    return sizeof(MethodResponsePkt);
}

void Socks5Negotiator::methodCallback(int         result,
                                      int        *numNeeded,
                                      bcema_Blob *data,
                                      int         channelId)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.methodCallback");
    *numNeeded = 0;

    if (result != btemt_AsyncChannel::BTEMT_SUCCESS) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
        BTEMT_LOG_ERROR << "Channel error (" << result << ")"
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    if (data->length() < sizeof(MethodResponsePkt)) {
        *numNeeded = sizeof(MethodResponsePkt);
        return;
    }

    MethodResponsePkt pkt;
    BlobUtil::extract((char*)&pkt, sizeof(pkt), data);

    switch (pkt.d_method) {
      case NONE: {
        BTEMT_LOG_DEBUG << "Remote server needs no authentication"
                       << BTEMT_LOG_END;
        connectToEndpoint();

      } break;
      case PASSWORD: {
        BTEMT_LOG_INFO << "Remote server requires user/pass authentication"
                      << BTEMT_LOG_END;
        if (!d_configuration.doAuthentication()) {
            BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_METHOD;
            BTEMT_LOG_ERROR << "Got authentication request when we did not"
                              " offer authentication as an option, closing"
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }

        int rc = sendAuthenticationRequest();
        if (rc < 0) {            
            BTEMT_LOG_ERROR << "Error sending authentication request, rc=" << rc
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }

        btemt_AsyncChannel::BlobBasedReadCallback callback
            = bdef_MemFnUtil::memFn(&Socks5Negotiator::authenticationCallback,
                                    this);

        rc = d_channel_p->read(rc, callback);
        if (rc != 0) {
            BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
            BTEMT_LOG_ERROR << "Error reading authentication response, rc="
                           << rc << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }

      } break;
      case UNACCEPTABLE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_AUTH_FAILED;
        BTEMT_LOG_ERROR << "Remote socks rejected all authentication methods"
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);

      } break;
      default: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_METHOD;
        BTEMT_LOG_ERROR << "Unknown response, code=" << pkt.d_method
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
      } break;
    }
}

int Socks5Negotiator::sendAuthenticationRequest()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.sendAuthenticationRequest");

    if (d_username.size() < 1 || d_password.size() < 1) {
        d_acquiringCredentials = true;

        BTEMT_LOG_INFO << "Attempting to fetch username/password"
                      << BTEMT_LOG_END;

        // Async fetch the username/password to use
        using namespace bdef_PlaceHolders;
        d_credentialsProvider_p->acquireSocks5Credentials(bdef_BindUtil::bind(
                                             &Socks5Negotiator::setCredentials,
                                             this, _1, _2));

        return sizeof(AuthenticationResponsePkt);
    }

    bcema_Blob blob(d_blobBufferFactory_p, d_allocator_p);
    char buffer = VERSION_USERNAME_PASSWORD_AUTH;
    bcema_BlobUtil::append(&blob, &buffer, sizeof(buffer));

    buffer = d_username.size();
    bcema_BlobUtil::append(&blob, &buffer, sizeof(buffer));
    bcema_BlobUtil::append(&blob, d_username.c_str(), d_username.size());

    buffer = d_password.size();
    bcema_BlobUtil::append(&blob, &buffer, sizeof(buffer));
    bcema_BlobUtil::append(&blob, d_password.c_str(), d_password.size());

    BTEMT_LOG_TRACE
        << "Sending authentication request --"
        << "blob: " << bcema_BlobUtilHexDumper(&blob)
        << BTEMT_LOG_END;

    int rc = d_channel_p->write(blob);
    if (rc != 0) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED;
        BTEMT_LOG_ERROR << "Error writing user/pass request, rc=" << rc
                       << BTEMT_LOG_END;
        return -1;
    }

    return sizeof(AuthenticationResponsePkt);
}

void Socks5Negotiator::connectToEndpoint()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.connectToEndpoint");

    if (d_connectionType == UDP_ASSOCIATE_CONNECTION) {
        BTEMT_LOG_INFO << "Sending UDP associate request" << BTEMT_LOG_END;

        int rc = sendUdpAssociateRequest();
        if (rc < 0) {
            BTEMT_LOG_ERROR << "Error sending UDP associate request, rc=" << rc
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }

        btemt_AsyncChannel::BlobBasedReadCallback callback
            = bdef_MemFnUtil::memFn(&Socks5Negotiator::udpAssociateCallback,
            this);

        rc = d_channel_p->read(sizeof(ConnectBase), callback);
        if (rc != 0) {
            BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
            BTEMT_LOG_ERROR << "Error reading UDP associate response, rc=" << rc
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }
    }
    else {
        BTEMT_LOG_INFO << "Sending connect request" << BTEMT_LOG_END;

        int rc = sendConnectionRequest();
        if (rc < 0) {
            BTEMT_LOG_ERROR << "Error sending connection request, rc=" << rc
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }

        btemt_AsyncChannel::BlobBasedReadCallback callback
            = bdef_MemFnUtil::memFn(&Socks5Negotiator::connectCallback,
                                    this);

        rc = d_channel_p->read(1, callback);
        if (rc != 0) {
            BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
            BTEMT_LOG_ERROR << "Error reading connection response, rc=" << rc
                           << BTEMT_LOG_END;
            d_channel_p->close();
            d_callback(CHANNEL_ERROR, this);
            return;
        }
    }
}

void Socks5Negotiator::authenticationCallback(int         result,
                                              int        *numNeeded,
                                              bcema_Blob *data,
                                              int         channelId)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.authenticationCallback");
    *numNeeded = 0;

    if (result != btemt_AsyncChannel::BTEMT_SUCCESS) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
        BTEMT_LOG_ERROR << "status: channel error (" << result << ")"
                       << BTEMT_LOG_END;
        if (d_acquiringCredentials) {
            d_credentialsProvider_p->cancelAcquiringCredentials();
        }

        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    if (data->length() < sizeof(AuthenticationResponsePkt)) {
        *numNeeded = sizeof(AuthenticationResponsePkt);
        return;
    }

    AuthenticationResponsePkt pkt;
    BlobUtil::extract((char*)&pkt, sizeof(pkt), data);
    if (pkt.d_status) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_AUTH_FAILED;
        BTEMT_LOG_ERROR << "status: authentication rejected" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
        return;
    }

    BTEMT_LOG_INFO << "Successful authentication" << BTEMT_LOG_END;
    connectToEndpoint();
}

int Socks5Negotiator::sendUdpAssociateRequest()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.sendUdpAssociateRequest");

    bcema_Blob blob(d_blobBufferFactory_p, d_allocator_p);
    const bool validIP = !bteso_IPv4Address::isValid(d_host.c_str());
    const bool isGwyDirRequest = d_configuration.destinationPort() == 48129;

    ConnectBase pktBase;
    pktBase.d_ver = VERSION;
    pktBase.d_cmd = UDP_PORT;
    pktBase.d_rsv = 0x00;
    pktBase.d_atype =  validIP || isGwyDirRequest
                       ? pktBase.d_atype = IPv4ADDRESS
                       : pktBase.d_atype = DOMAINNAME;
    bcema_BlobUtil::append(&blob, (char*)&pktBase, sizeof(pktBase));

    if (isGwyDirRequest) {
        // Special, if connecting to gwy dir, use a 0 IP address
        // Its what wintrv does, and what works...

        ConnectIPv4AddressRequestPkt pkt;
        memset(&pkt, 0, sizeof(pkt));
        pkt.d_address = 0;
        pkt.d_port    = d_configuration.destinationPort();
        bcema_BlobUtil::append(&blob, (char*)&pkt, sizeof(pkt));
    }
    else if (validIP) {
        bteso_IPv4Address ipv4Addr(d_host.c_str(), d_port);
        ConnectIPv4AddressRequestPkt pkt;
        memset(&pkt, 0, sizeof(pkt));
        pkt.d_address = ipv4Addr.ipAddress();
        pkt.d_port    = ipv4Addr.portNumber();
        bcema_BlobUtil::append(&blob, (char*)&pkt, sizeof(pkt));
    } else {
        char size = d_host.size();
        bcema_BlobUtil::append(&blob, &size, sizeof(size));

        bcema_BlobUtil::append(&blob, d_host.c_str(), d_host.size());
        bdeut_BigEndianInt16 port = bdeut_BigEndianInt16::make(d_port);
        BSLS_ASSERT(sizeof(port) == sizeof(short));
        bcema_BlobUtil::append(&blob, (char*)&port, sizeof(port));
    }

    BTEMT_LOG_TRACE
        << "socks5 udp associate request --"
        << "blob: " << bcema_BlobUtilHexDumper(&blob)
        << BTEMT_LOG_END;

    int rc = d_channel_p->write(blob);
    if (rc) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED;
        BTEMT_LOG_ERROR << "Error writing user/pass request, rc="
                       << rc << BTEMT_LOG_END;
        return -1;
    }
    
    return blob.length();
}

void Socks5Negotiator::udpAssociateCallback(int         result,
                                            int        *numNeeded,
                                            bcema_Blob *data,
                                            int         channelId)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.udpAssociateCallback");
    *numNeeded = 0;

    if (result != btemt_AsyncChannel::BTEMT_SUCCESS) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
        BTEMT_LOG_ERROR << "Channel error (" << result << ")"
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    if (data->length() < sizeof(ConnectBase)) {
        *numNeeded = sizeof(ConnectBase);
        return;
    }

    int bytesRead = 0;

    ConnectRspBase hdr;
    BlobUtil::copyOut((char*)&hdr, *data, sizeof(hdr), bytesRead);
    bytesRead += sizeof(hdr);

    if (hdr.d_ver != VERSION) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_VER;
        BTEMT_LOG_ERROR << "Invalid socks version number, got="
                       << hdr.d_ver << ", expected=" << VERSION
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
        return;
    }

    // trim off rest of bytes
    if (hdr.d_atype == IPv4ADDRESS) {
        if (data->length() < (bytesRead +
                              (int)sizeof(ConnectIPv4AddressResponsePkt)))
        {
            *numNeeded = bytesRead + sizeof(ConnectIPv4AddressResponsePkt);
            return;
        }

        ConnectIPv4AddressResponsePkt ipv4AddressPkt;
        BlobUtil::copyOut((char*)&ipv4AddressPkt,
                                *data,
                                sizeof(ipv4AddressPkt),
                                bytesRead);
        bytesRead += sizeof(ipv4AddressPkt);
        d_udpAssociateAddress = bteso_IPv4Address(ipv4AddressPkt.d_bndAddr,
                                                  ipv4AddressPkt.d_bndPort);
    } else if (hdr.d_atype == DOMAINNAME) {
        if (data->length() < bytesRead + (int)sizeof(unsigned char)) {
            *numNeeded = bytesRead + sizeof(unsigned char);
            return;
        }

        unsigned char length = 0;
        BlobUtil::copyOut((char*)&length,
                                *data,
                                sizeof(length),
                                bytesRead);
        bytesRead += sizeof(unsigned char);

        if (data->length() < bytesRead + length + 2) {
            *numNeeded = bytesRead + length + 2;
            return;
        }

        bsl::vector<char> addressBuf(length);
        BlobUtil::copyOut(&addressBuf[0], *data, length, bytesRead);
        bytesRead += length;

        bdeut_BigEndianUint16 port;
        BlobUtil::copyOut((char*)&port, *data, sizeof(port), bytesRead);
        bytesRead += sizeof(port);

        d_udpAssociateAddress = InternetAddress(
                                            bdeut_StringRef(&addressBuf[0],
                                                            addressBuf.size()),
                                            port);
    } else {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY;
        BTEMT_LOG_ERROR << "Invalid address type received, type=" << hdr.d_atype
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    bcema_BlobUtil::erase(data, 0, bytesRead);

    switch (hdr.d_rep) {
      case REQUEST_GRANTED: {
        BTEMT_LOG_DEBUG << "Successfully negotiated a UDP port through socks server ("
                      << d_configuration.socksIpAddr() << ":" << d_configuration.socksPort()
                      << ") via UDP associate address ("
                      << d_udpAssociateAddress << ") to remote host ("
                      << d_configuration.destinationIpAddr() << ":" << d_configuration.destinationPort()
                      << ")"
                      << BTEMT_LOG_END;
        d_callback(SUCCESS, this);

      } break;
      case GENERAL_FAILURE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY;
        BTEMT_LOG_ERROR << "status: general failure" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
      } break;
      case CONNECTION_NOT_ALLOWED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY;
        BTEMT_LOG_ERROR << "status: connection not allowed" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
      } break;
      case NETWORK_UNREACHABLE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: network unreachable" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case HOST_UNREACHABLE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: host unreachable" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case CONNECTION_REFUSED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: connection refused" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case TTL_EXPIRED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED;
        BTEMT_LOG_ERROR << "status: ttl expired" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case CMD_PROTO_NOT_SUPPORTED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_VER;
        BTEMT_LOG_ERROR << "status: cmd protocol not supported" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(UNSUPPORTED_ERROR, this);
       } break;
      case ADDRESS_TYPE_NOT_SUPPORTED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_VER;
        BTEMT_LOG_ERROR << "status: cmd protocol not supported" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(UNSUPPORTED_ERROR, this);
      } break;
      default: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED;
        BTEMT_LOG_ERROR << "status: unknown code = " << hdr.d_rep
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
      } break;
    }
}

int Socks5Negotiator::sendConnectionRequest()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.sendConnectionRequest");

    bcema_Blob blob(d_blobBufferFactory_p, d_allocator_p);
    const bool validIP = !bteso_IPv4Address::isValid(d_host.c_str());

    ConnectBase pktBase;
    pktBase.d_ver = VERSION;
    pktBase.d_cmd = TCP_STREAM_CONNECTION;
    pktBase.d_rsv = 0x00;
    pktBase.d_atype =  validIP ? pktBase.d_atype = IPv4ADDRESS
                               : pktBase.d_atype = DOMAINNAME;
    bcema_BlobUtil::append(&blob, (char*)&pktBase, sizeof(pktBase));

    if (validIP) {
        bteso_IPv4Address ipv4Addr(d_host.c_str(), d_port);
        ConnectIPv4AddressRequestPkt pkt;
        pkt.d_address = ipv4Addr.ipAddress();
        pkt.d_port    = ipv4Addr.portNumber();
        bcema_BlobUtil::append(&blob, (char*)&pkt, sizeof(pkt));
    } else {
        char size = bdeut_BigEndianInt16::make(d_host.size());
        bcema_BlobUtil::append(&blob, &size, sizeof(size));

        bcema_BlobUtil::append(&blob, d_host.c_str(), d_host.size());
        bdeut_BigEndianInt16 port = bdeut_BigEndianInt16::make(d_port);
        BSLS_ASSERT(sizeof(port) == sizeof(short));
        bcema_BlobUtil::append(&blob, (char*)&port, sizeof(port));
    }

    BTEMT_LOG_TRACE
        << "socks5 connection request --"
        << "blob: " << bcema_BlobUtilHexDumper(&blob)
        << BTEMT_LOG_END;

    int rc = d_channel_p->write(blob);
    if (rc) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED;
        BTEMT_LOG_ERROR << "Error writing connection request, rc="
                       << rc << BTEMT_LOG_END;
        return -1;
    }
    
    return blob.length();
}

void Socks5Negotiator::connectCallback(int         result,
                                       int        *numNeeded,
                                       bcema_Blob *data,
                                       int         channelId)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.connectCallback");
    *numNeeded = 0;

    if (result != btemt_AsyncChannel::BTEMT_SUCCESS) {
        BTEMT_LOG_ERROR << "Channel error (" << result << ")"
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    if (data->length() < sizeof(ConnectBase)) {
        *numNeeded = sizeof(ConnectBase);
        return;
    }

    ConnectRspBase hdr;
    BlobUtil::extract((char*)&hdr, sizeof(hdr), data);

    if (hdr.d_ver != VERSION) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_VER;
        BTEMT_LOG_ERROR << "Invalid socks version number, got="
                       << hdr.d_ver << ", expected=" << VERSION
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
        return;
    }

    // trim off rest of bytes
    if (hdr.d_atype == IPv4ADDRESS) {
        bcema_BlobUtil::erase(data,
                              0,
                              sizeof(ConnectIPv4AddressResponsePkt));
    } else if (hdr.d_atype == DOMAINNAME) {
        unsigned char length = 0;
        BlobUtil::extract((char*)&length, sizeof(length), data);
        length += sizeof(short); // the length byte and trailing port #
        bcema_BlobUtil::erase(data, 0, length);
    } else {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY;
        BTEMT_LOG_ERROR << "Invalid address type received, type=" << hdr.d_atype
                       << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }

    switch (hdr.d_rep) {
      case REQUEST_GRANTED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SUCCESS;
        //BTEMT_LOG_DIAGNOSTICCODE_SUCCESS("Socks proxy connection [", d_configuration.socksIpAddr(), "]");
        BTEMT_LOG_INFO << "Successfully connected through socks ("
                      << d_configuration.socksIpAddr() << ":" << d_configuration.socksPort()
                      << ") to remote host ("
                      << d_configuration.destinationIpAddr() << ":" << d_configuration.destinationPort()
                      << ")"
                      << BTEMT_LOG_END;
        d_callback(SUCCESS, this);

      } break;
      case GENERAL_FAILURE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY;
        BTEMT_LOG_ERROR << "status: general failure" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
      } break;
      case CONNECTION_NOT_ALLOWED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: connection not allowed" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(FAILURE, this);
      } break;
      case NETWORK_UNREACHABLE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: network unreachable" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case HOST_UNREACHABLE: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: host unreachable" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case CONNECTION_REFUSED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME;
        BTEMT_LOG_ERROR << "status: connection refused" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case TTL_EXPIRED: {
        BTEMT_LOG_ERROR << "status: ttl expired" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(NETWORK_ERROR, this);
      } break;
      case CMD_PROTO_NOT_SUPPORTED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED;
        BTEMT_LOG_ERROR << "status: cmd protocol not supported" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(UNSUPPORTED_ERROR, this);
       } break;
      case ADDRESS_TYPE_NOT_SUPPORTED: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED;
        BTEMT_LOG_ERROR << "status: cmd protocol not supported" << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(UNSUPPORTED_ERROR, this);
      } break;
      default: {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED;
        BTEMT_LOG_ERROR << "status: unknown = " << hdr.d_rep << BTEMT_LOG_END;
       d_channel_p->close();
       d_callback(CHANNEL_ERROR, this);
      } break;
    }
}

void Socks5Negotiator::setCredentials(const bdeut_StringRef& username, 
                                      const bdeut_StringRef& password)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator");

    d_username = username;
    d_password = password;
    d_acquiringCredentials = false;

    int rc = sendAuthenticationRequest();
    if (rc < 0) {            
        BTEMT_LOG_ERROR << "Error sending authentication info, rc=" << rc << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
    }
}

// CREATORS
Socks5Negotiator::Socks5Negotiator(
                            btemt_AsyncChannel              *channel,
                            const SocksConfiguration&        configuration,
                            const Socks5NegotiatorCallback&  callback,
                            Socks5CredentialsProvider       *credentialsProvider,
                            bcema_BlobBufferFactory         *blobBufferFactory,
                            ConnectionType                   connectionType,
                            bslma::Allocator                 *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_channel_p(channel)
, d_host(configuration.destinationIpAddr())
, d_port(configuration.destinationPort())
, d_username(configuration.username())
, d_password(configuration.password())
, d_callback(callback)
, d_credentialsProvider_p(credentialsProvider)
, d_blobBufferFactory_p(blobBufferFactory)
, d_connectionType(connectionType)
, d_acquiringCredentials(false)
, d_configuration(configuration, allocator)
{
    BSLS_ASSERT(channel);
    BSLS_ASSERT(blobBufferFactory);
}

Socks5Negotiator::~Socks5Negotiator()
{
}

// MANIPULATORS
void Socks5Negotiator::start()
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5Negotiator.start");

    BTEMT_LOG_INFO << "Starting SOCKS5 negotiation with socks server ("
                  << d_configuration.socksIpAddr() << ":" << d_configuration.socksPort()
                  << ") using "
                  << (d_connectionType == TCP_CONNECTION ? "TCP" : "UDP")
                  << " to destination ("
                  << d_configuration.destinationIpAddr() << ":" << d_configuration.destinationPort()
                  << ")"
                  << BTEMT_LOG_END;

    bsl::stringstream ss;
    ss << "socks server: ("
        << d_configuration.socksIpAddr() << ":" << d_configuration.socksPort()
        << ") using "
        << (d_connectionType == TCP_CONNECTION ? "TCP" : "UDP")
        << " to destination ("
        << d_configuration.destinationIpAddr() << ":" << d_configuration.destinationPort()
        << ")";
    BTEMT_LOG_DIAGNOSTICCODE_PROXY_START("Proxy connection ", ss.str().c_str());

    int rc = sendMethodRequest();
    if (rc < 0) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED;
        BTEMT_LOG_ERROR << "Error sending method request, rc=" << rc << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    };

    btemt_AsyncChannel::BlobBasedReadCallback callback
        = bdef_MemFnUtil::memFn(&Socks5Negotiator::methodCallback, this);

    rc = d_channel_p->read(rc, callback);
    if (rc != 0) {
        BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED;
        BTEMT_LOG_ERROR << "Error reading method response, rc=" << rc << BTEMT_LOG_END;
        d_channel_p->close();
        d_callback(CHANNEL_ERROR, this);
        return;
    }
}


int Socks5NegotiatorUtil::resolveUdpAddress(bteso_IPv4Address      *resolvedAddress,
                                            const InternetAddress&  address)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5NegotiatorUtil.resolveAddress");

    BSLS_ASSERT(resolvedAddress);

    if (address.isIPv4()) {
        *resolvedAddress = address.ipv4();
    }
    else if (address.isDomainname()) {
        int rv = bteso_ResolveUtil::getAddress(resolvedAddress, address.domainnameHost().c_str());
        if (0 != rv) {
            BTEMT_LOG_DIAGNOSTICCODE_PROXY_RSLV_FAILED;
            BTEMT_LOG_ERROR << "Error resolving address '" << address.domainnameHost()
                           << "', got rc=" << rv
                           << BTEMT_LOG_END;
            return -1;
        }
        resolvedAddress->setPortNumber(address.port());

        BTEMT_LOG_INFO << "Resolved address '" << address.domainnameHost() << "' to "
                      << *resolvedAddress
                      << BTEMT_LOG_END;
    }
    return 0;
}

// ACCESSORS
const SocksConfiguration& Socks5Negotiator::configuration() const
{
    return d_configuration;
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
