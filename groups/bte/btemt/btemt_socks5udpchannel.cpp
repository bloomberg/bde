// btemt_socks5udpchannel.cpp               -*-C++-*-

///Implementation Notes
///--------------------
// . . .

#include <btemt_socks5udpchannel.h>

#include <btemt_blobutil.h>

#include <bdex_bytestreamimputil.h>
#include <bcema_blobutil.h>
#include <bdeut_bigendian.h>
#include <bsls_byteorder.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace BloombergLP {
namespace btemt {

namespace {

enum {
    SOCKS5_UDP_ATYP_IPV4 = 1,
    SOCKS5_UDP_ATYP_DOMAINNAME = 3,
    SOCKS5_UDP_ATYP_IPV6 = 4
};

struct SocksUdpPacketHeaderBase {
    char d_rsv[2];
    unsigned char d_frag;
    unsigned char d_atyp;
};

struct SocksUdpPacketHeaderIPv4 {
    char d_dstAddr[4];
    char d_dstPort[2];
};

}  // close anonymous namespace

struct Socks5UdpChannel_ReadCallback {
    //DATA
    btemt_AsyncChannel::BlobBasedReadCallback d_callback;
    Socks5UdpChannel *d_socks5UdpChannel_p;

    //CREATORS
    Socks5UdpChannel_ReadCallback(
                   Socks5UdpChannel                          *socks5UdpChannel,
                   btemt_AsyncChannel::BlobBasedReadCallback  callback)
    : d_callback(callback)
    , d_socks5UdpChannel_p(socks5UdpChannel)
    {
    }

    //MANIPULATORS
    void operator()(int         result,
                    int        *bytesNeeded,
                    bcema_Blob *payload,
                    int         channelId) const
    {
        d_socks5UdpChannel_p->handleRead(result,
                                         bytesNeeded,
                                         payload,
                                         channelId,
                                         d_callback);
    }
};

static void wrapPacket(bcema_Blob             *dest,
                       const bcema_Blob&       packet,
                       const InternetAddress&  destinationAddress)
{
    SocksUdpPacketHeaderBase headerBase;
    memset(&headerBase, 0, sizeof(headerBase));
    if (destinationAddress.isDomainname()) {
        headerBase.d_atyp = SOCKS5_UDP_ATYP_DOMAINNAME;
        bcema_BlobUtil::append(dest,
                               reinterpret_cast<const char*>(&headerBase),
                               0,
                               sizeof(headerBase));
        const bsl::string& host = destinationAddress.domainnameHost();
        BSLS_ASSERT(host.length() < 256);
        unsigned char length = host.length();
        bcema_BlobUtil::append(dest,
                               reinterpret_cast<const char*>(&length),
                               0,
                               1);
        bcema_BlobUtil::append(dest, host.data(), 0, host.length());
        unsigned short port =
           BSLS_BYTEORDER_HOST_U16_TO_BE(destinationAddress.port());
        bcema_BlobUtil::append(dest,
                               reinterpret_cast<const char*>(&port),
                               0,
                               sizeof(port));
    }
    else if (destinationAddress.isIPv4()) {
        headerBase.d_atyp = SOCKS5_UDP_ATYP_IPV4;
        bcema_BlobUtil::append(dest,
                               reinterpret_cast<const char*>(&headerBase),
                               0,
                               sizeof(headerBase));

        struct SocksUdpPacketHeaderIPv4 dst;
        int address = ntohl(destinationAddress.ipv4().ipAddress());
        bdex_ByteStreamImpUtil::putInt32(dst.d_dstAddr, address);
        bdex_ByteStreamImpUtil::putInt16(
                                    dst.d_dstPort,
                                    destinationAddress.ipv4().portNumber());
        bcema_BlobUtil::append(dest,
                               reinterpret_cast<const char*>(&dst),
                               0,
                               sizeof(dst));
    }
    bcema_BlobUtil::append(dest, packet);
}


                        // ----------------------
                        // class Socks5UdpChannel
                        // ----------------------

void Socks5UdpChannel::handleRead(
                        int                                        result,
                        int                                       *bytesNeeded,
                        bcema_Blob                                *payload,
                        int                                        channelId,
                        btemt_AsyncChannel::BlobBasedReadCallback  callback)
{
    BSLS_ASSERT(payload);
    BSLS_ASSERT(bytesNeeded);
    *bytesNeeded = 0;
    struct SocksUdpPacketHeaderBase headerBase;
    
    if (result != btemt_AsyncChannel::BTEMT_SUCCESS) {
        callback(result, bytesNeeded, payload, channelId);
        return;
    }
    
    if (payload->length() < sizeof(headerBase)) {
        BSLS_ASSERT(0);
        return;
    }
    btemt::BlobUtil::extract(reinterpret_cast<char*>(&headerBase),
                            sizeof(headerBase),
                            payload);
    if (SOCKS5_UDP_ATYP_IPV4 == headerBase.d_atyp) {
        unsigned int address;
        if (payload->length() < sizeof(address)) {
            BSLS_ASSERT(0);
            return;
        }
        btemt::BlobUtil::extract(reinterpret_cast<char*>(&address),
                                sizeof(address),
                                payload);
        bdeut_BigEndianUint16 port;
        if (payload->length() < sizeof(port)) {
            BSLS_ASSERT(0);
            return;
        }
        btemt::BlobUtil::extract(reinterpret_cast<char*>(&port),
                                sizeof(port),
                                payload);
    }
    else if (SOCKS5_UDP_ATYP_DOMAINNAME == headerBase.d_atyp) {
        unsigned char addrLen;
        if (payload->length() < 1) {
            BSLS_ASSERT(0);
            return;
        }
        btemt::BlobUtil::extract(reinterpret_cast<char*>(&addrLen), 1, payload);
        bsl::vector<char> name(addrLen);
        if (payload->length() < addrLen) {
            BSLS_ASSERT(0);
            return;
        }
        btemt::BlobUtil::extract(&name[0], addrLen, payload);
    }
    else {
        BSLS_ASSERT(0);
        return;
    }
    callback(result, bytesNeeded, payload, channelId);
}


// CREATORS
Socks5UdpChannel::Socks5UdpChannel(
                    bdema_ManagedPtr<btemt_AsyncChannel>  udpAsyncChannel,
                    btemt_AsyncChannel                   *associatedTcpChannel,
                    bcema_BlobBufferFactory              *blobBufferFactory,
                    const InternetAddress&                destAddress,
                    bslma::Allocator                     *allocator)
: d_udpAsyncChannel_mp(udpAsyncChannel)
, d_destinationAddress(destAddress, allocator)
, d_blobBufferFactory_p(blobBufferFactory)
, d_associatedTcpChannel_p(associatedTcpChannel)
{
    BSLS_ASSERT(d_udpAsyncChannel_mp);
    BSLS_ASSERT(d_associatedTcpChannel_p);
}

Socks5UdpChannel::~Socks5UdpChannel()
{
}

// MANIPULATORS

int Socks5UdpChannel::read(int                 numBytes,
                           const ReadCallback& readCallback)
{
    BSLS_ASSERT(0);  // NOT IMPLEMENTED
    return -1;
}

int Socks5UdpChannel::read(int                          numBytes,
                           const BlobBasedReadCallback& readCallback)
{
    btemt_AsyncChannel::BlobBasedReadCallback myCallback = 
                             Socks5UdpChannel_ReadCallback(this, readCallback);
    return d_udpAsyncChannel_mp->read(numBytes, myCallback);
}

int Socks5UdpChannel::timedRead(int                      numBytes,
                                const bdet_TimeInterval& timeout,
                                const ReadCallback&      readCallback)
{
    BSLS_ASSERT(0);  // NOT IMPLEMENTED
    return -1;
}

int Socks5UdpChannel::timedRead(int                          numBytes,
                                const bdet_TimeInterval&     timeout,
                                const BlobBasedReadCallback& readCallback)
{
    btemt_AsyncChannel::BlobBasedReadCallback myCallback = 
                             Socks5UdpChannel_ReadCallback(this, readCallback);
    return d_udpAsyncChannel_mp->timedRead(numBytes, timeout, myCallback);
}


int Socks5UdpChannel::write(const bcema_Blob&    blob,
                            int                  highWaterMark)
{
    bcema_Blob packet(d_blobBufferFactory_p);
    wrapPacket(&packet, blob, d_destinationAddress);
    return d_udpAsyncChannel_mp->write(packet, highWaterMark);
}

int Socks5UdpChannel::write(const btemt_BlobMsg& blob,
                            int                  highWaterMark)
{
    BSLS_ASSERT(0);  // NOT IMPLEMENTED
    return -1;
}

int Socks5UdpChannel::write(const btemt_DataMsg&  data,
                            btemt_BlobMsg        *msg)
{
    BSLS_ASSERT(0);  // NOT IMPLEMENTED
    return -1;
}

int Socks5UdpChannel::write(const btemt_DataMsg&  data,
                            int                   highWaterMark,
                            btemt_BlobMsg        *msg)
{
    BSLS_ASSERT(0);  // NOT IMPLEMENTED
    return -1;
}

int Socks5UdpChannel::setSocketOption(int option, int level, int value)
{
    return d_udpAsyncChannel_mp->setSocketOption(option, level, value);
}

void Socks5UdpChannel::cancelRead()
{
    d_udpAsyncChannel_mp->cancelRead();
}

void Socks5UdpChannel::close()
{
    d_udpAsyncChannel_mp->close();
    d_associatedTcpChannel_p->close();
}

// ACCESSORS

bteso_IPv4Address Socks5UdpChannel::localAddress() const
{
    return d_udpAsyncChannel_mp->localAddress();
}

bteso_IPv4Address Socks5UdpChannel::peerAddress() const
{
    return d_udpAsyncChannel_mp->peerAddress();
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
