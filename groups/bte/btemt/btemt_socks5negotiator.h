// btemt_socks5negotiator.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5NEGOTIATOR
#define INCLUDED_BTEMT_SOCKS5NEGOTIATOR

//@PURPOSE:  Provide SOCKS5 client-side handshake negotiation
//
//@CLASSES:
//   Socks5Negotiator: negotiator to facilitate SOCKS5 client-side handshake
//
//@AUTHOR: Eric Vander Weele (evander)
//
//@SEE ALSO: bbcn_socks5sessionfactory bbcn_socksconfiguration
//
//@DESCRIPTION: This component defines a concrete mechanism for negotiating a
// SOCKS5 client-side handshake.  The caller should verify the
// 'Socks5Negotiator::Status' in the callback after attempted SOCKS5
// negotiation.
//
///Usage Example
///-------------
// The following code snippets demonstrate how to use a 'Socks5Negotiator' to
// negotiate a SOCKS5 client-side handshake.  First we will declare and define
// the callback function.
//..
//  void socks5Callback(Socks5Negotiator::Status  result,
//                      Socks5Negotiator         *negotiator)
//  {
//      assert(result == Socks5Negotiator::SUCCESS);
//  }
//..
// Now we will declare and define a SOCKS5 server to facilitate the server-side
// of the SOCKS5 handshake.
//..
//  //TODO: Some long server declaration and implementation here...
//..
// Now we will create a 'btemt::TestChannel' pair to simulate channel
// communication.
//..
//  TestChannelUtil::TestChannelPtr clientChannel;
//  TestChannelUtil::TestChannelPtr serverChannel;
//  TestChannelUtil::allocateChannelPair(&clientChannel, &serverChannel);
//
//..
//  Now we will create a blob buffer factory for allocating new buffers.
//..
//  enum { BUFFER_SIZE = 32 };
//  bcema_BlobBufferFactory bufferFactory(BUFFER_SIZE);
//..
//  Now we will create and start a SOCKS5 server.
//..
//  Socks5Server server(&serverChannel.ptr(),
//                      &bufferFactory);
//  server.start();
//..
// Now we will create 'btemt::SocksConfiguration' for client configuration.
//..
//  SocksConfiguration config;
//  config.addr() = "127.0.0.1";
//  config.port() = 80;
//..
// Now we will create a 'Socks5Negotiator' and start negotiation.
//..
//  Socks5Negotiator negotiator(clientChannel.ptr(),
//                              config,
//                              &socks5Callback,
//                              &bufferFactory);
//  negotiator.start();
//..

#ifndef INCLUDED_BTEMT_INTERNETADDRESS
#include <btemt_internetaddress.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_IPv4Address.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#include <bdeut_variant.h>

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BTEMT_SOCKS5CREDENTIALSPROVIDER
#include <btemt_socks5credentialsprovider.h>
#endif

#ifndef INCLUDED_BTEMT_SOCKSCONFIGURATION
#include <btemt_socksconfiguration.h>
#endif

#ifndef INCLUDED_BTEMT_INTERNETADDRESS
#include <btemt_internetaddress.h>
#endif

namespace BloombergLP {

class bcema_Blob;
class bcema_BlobBufferFactory;
class blsma_Allocator;
class btemt_AsyncChannel;
class bdeut_StringRef;

namespace btemt {

                        // ======================
                        // class Socks5Negotiator
                        // ======================

class Socks5Negotiator {
    // Class documentation goes here

   public:
    // PUBLIC TYPES
    enum Status {
        SUCCESS           = 0
      , FAILURE           = 1
      , CHANNEL_ERROR     = 2
      , NETWORK_ERROR     = 3
      , UNSUPPORTED_ERROR = 4
    };

    enum ConnectionType {
        TCP_CONNECTION           = 0,
        UDP_ASSOCIATE_CONNECTION = 1
    };

    typedef bdef_Function<
                  void(*)(Status, Socks5Negotiator*)> Socks5NegotiatorCallback;


  private:
  
    // DATA
    bslma::Allocator           *d_allocator_p;         // memory (held)
    btemt_AsyncChannel        *d_channel_p;           // channel (held)
    bsl::string                d_host;                // destination address
    int                        d_port;                // destination port
    bsl::string                d_username;            // socks username
    bsl::string                d_password;            // socks password
    Socks5NegotiatorCallback   d_callback;            // asynchronous callback
    bcema_BlobBufferFactory   *d_blobBufferFactory_p; // buffer factory (held)
    ConnectionType             d_connectionType;
    InternetAddress            d_udpAssociateAddress;
    Socks5CredentialsProvider *d_credentialsProvider_p;
    bool                       d_acquiringCredentials;
    SocksConfiguration         d_configuration;

    // PRIVATE MANIPULATORS
    int sendMethodRequest();
        // Send SOCKS5 greeting including list of authentication methods
        // supported.

    void methodCallback(int         result,
                        int        *numNeeded,
                        bcema_Blob *blob,
                        int         channelId);
        // TODO:

    int sendAuthenticationRequest();
        // Send the corresponding 'd_username' and 'd_password' credentials to
        // authenticate with the SOCKS5 server.

    void authenticationCallback(int         result,
                                int        *numNeeded,
                                bcema_Blob *data,
                                int         channelId);
        
    void connectToEndpoint();
        // Send the appropriate request to connect to the endpoint past the
        // SOCKS server either via a connection request or a UDP
        // associate request.

    int sendConnectionRequest();
        // Send connection request to establish external connection to 'd_host'
        // on port 'd_port'.

    void connectCallback(int         result,
                         int        *numNeeded,
                         bcema_Blob *data,
                         int         channelId);

    int sendUdpAssociateRequest();

    void udpAssociateCallback(int         result,
                              int        *numNeeded,
                              bcema_Blob *data,
                              int         channelId);

        // TODO:

  private:
    // NOT IMPLEMENTED
    Socks5Negotiator();
    Socks5Negotiator(const Socks5Negotiator&);
    Socks5Negotiator& operator=(const Socks5Negotiator&);

  public:
    // CREATORS
    Socks5Negotiator(btemt_AsyncChannel              *channel,
                     const SocksConfiguration&        configuration,
                     const Socks5NegotiatorCallback&  callback,
                     Socks5CredentialsProvider       *credentialsProvider,
                     bcema_BlobBufferFactory         *blobBufferFactory,
                     ConnectionType                   connectionType,
                     bslma::Allocator                 *allocator = 0);
        // Create a 'Socks5Negotiator' that can negotiate a client-side SOCKS5
        // handshake on the specified 'channel'.  The specified 'configuration'
        // will be used to establish and negotiate the SOCKS5 connection.
        // The negotiator will try to negotiate a SOCKS5 connection before
        // calling the specified 'callback'.  Use the specified
        // 'blobBufferFactory' for blob creation in this negotiator.
        // 'blobBufferFactory' *must* outlive this negotiator. Use the
        // optionally specified 'allocator' to supply memory.

    ~Socks5Negotiator();
        // Destroy this object.

    // MANIPULATORS
    void start();
        // Start SOCKS5 client-side negotiation.

    const InternetAddress& udpAssociateAddress() const;

    void setCredentials(const bdeut_StringRef& username, 
                        const bdeut_StringRef& password);

    // ACCESSORS
    const SocksConfiguration& configuration() const;
};

struct Socks5NegotiatorUtil {
    static int resolveUdpAddress(bteso_IPv4Address *resolvedAddress,
                          const InternetAddress& address);
};

inline
const InternetAddress& Socks5Negotiator::udpAssociateAddress() const
{
    return d_udpAssociateAddress;
}

}  // close namespace

}  // close namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
