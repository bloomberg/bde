// baenet_httpmessagegenerator.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPMESSAGEGENERATOR
#define INCLUDED_BAENET_HTTPMESSAGEGENERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for generating HTTP messages.
//
//@CLASSES:
//  baenet_HttpMessageGenerator: generator for HTTP messages
//
//@SEE_ALSO: RFC 2616
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides a 'bcema_Blob'-based HTTP message
// generator.  Given an HTTP start line, an HTTP header, and a series of entity
// data blobs, the 'baenet_HttpMessageGenerator' class will invoke a
// user-supplied callback with the HTTP message.  Note that the callback may be
// invoked multiple times as data is fed into the generator.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we are implementing a simple HTTP server.  We need a function that
// writes response data to the client, and we need a function that constructs
// the HTTP response.  First, let's define a function that writes data to the
// client.  The connection is represented as a 'bsl::streambuf'.
//..
//   void writeToConnection(bsl::streambuf    *connection,
//                          const bcema_Blob&  data)
//   {
//       // Write the specified HTTP 'data' fragment to the specified 
//       // 'connection'.
//
//       bdex_ByteOutStreamFormatter bosf(connection);
//       bcema_BlobUtil::write(bosf, data);
//   }
//..
// Next, let's define a function that accepts the response and writes the
// formatted HTTP data to the client.
//..
//   int deliverResponse(bsl::streambuf                     *connection,
//                       bcema_BlobBufferFactory            *blobBufferFactory,
//                       const baenet_HttpStatusCode::Value  statusCode,
//                       const char                         *reason,
//                       const bcema_Blob&                   body)
//   {
//       // Write to the specified 'connection' an HTTP/1.1 response 
//       // identified by the specified 'statusCode' and 'reason' consisting
//       // of the specified 'numChunks' of data in the specified 'chunk'
//       // array.  Allocate blob buffers using the specified 
//       // 'blobBufferFactory'.  Return 0 on success and a non-zero value 
//       // otherwise.
//   
//       int rc;
//   
//       baenet_HttpMessageGenerator generator(blobBufferFactory);
//   
//       baenet_HttpStatusLine       statusLine;
//       baenet_HttpResponseHeader   header;
//   
//       statusLine.majorVersion() = 1;
//       statusLine.minorVersion() = 1;
//       statusLine.statusCode()   = statusCode;
//       statusLine.reasonPhrase() = reason;
//   
//       header.basicFields().contentLength() = body.length();
//   
//       rc = generator.startEntity(statusLine,
//                                  header,
//                                  bdef_BindUtil::bind(
//                                                 &writeToConnection,
//                                                 connection,
//                                                 bdef_PlaceHolders::_1));
//       if (0 != rc) {
//           return rc;
//       }
//   
//       rc = generator.addEntityData(body);
//       if (0 != rc) {
//           return rc;
//       }
//   
//      return generator.endEntity();
//   } 
//..
// Finally, define a function that accepts a response and writes the formatted
// HTTP data using the chunked transfer encoding.
//..
//   int deliverResponse(bsl::streambuf*                     connection,
//                       bcema_BlobBufferFactory            *blobBufferFactory,
//                       const baenet_HttpStatusCode::Value  statusCode,
//                       const char                         *reason,
//                       const bcema_Blob*                   chunk,
//                       int                                 numChunks)
//   {
//       // Write to the specified 'connection' an HTTP/1.1 response 
//       // identified by the specified 'statusCode' and 'reason' consisting 
//       // of the specified 'numChunks' of data in the specified 'chunk' 
//       // array.  Transfer the entity using chunked transfer encoding.  
//       // Allocate blob buffers using the specified 'blobBufferFactory'.  
//       // Return 0 on success and a non-zero value otherwise.
//   
//       int rc;
//   
//       baenet_HttpMessageGenerator generator(blobBufferFactory);
//   
//       baenet_HttpStatusLine       statusLine;
//       baenet_HttpResponseHeader   header;
//   
//       statusLine.majorVersion() = 1;
//       statusLine.minorVersion() = 1;
//       statusLine.statusCode()   = statusCode;
//       statusLine.reasonPhrase() = reason;
//   
//       header.basicFields().contentLength() = 0;
//   
//       header.basicFields().transferEncoding().push_back(
//                            baenet_HttpTransferEncoding::BAENET_CHUNKED);
//   
//       rc = generator.startEntity(statusLine,
//                                  header,
//                                  bdef_BindUtil::bind(
//                                                 &writeToConnection,
//                                                 connection,
//                                                 bdef_PlaceHolders::_1));
//       if (0 != rc) {
//           return rc;
//       }
//   
//       for (int i = 0; i < numChunks; ++i) {    
//           rc = generator.addEntityData(chunk[i]);
//           if (0 != rc) {
//               return rc;
//           }
//       }
//   
//       return generator.endEntity();
//   }
//..
// The following snippets illustrate how to call these functions to use
// 'baenet_HttpMessageGenerator' to generate HTTP messages.
//..
//   int rc;
//   
//   bcema_PooledBlobBufferFactory blobBufferFactory(1024, &ta);
//
//   // Encode a response using the identity transfer encoding.
//
//   {
//       bcema_Blob response(&blobBufferFactory);
//       bcema_Blob responseBody(&blobBufferFactory);
//
//       {
//           const char DATA[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//
//           bcesb_OutBlobStreamBuf osb(&responseBody);
//           osb.sputn(DATA, sizeof DATA - 1);
//           osb.pubsync();
//       }
//
//       bcesb_OutBlobStreamBuf responseBuffer(&response);
//
//       rc = deliverResponse(&responseBuffer,
//                            &blobBufferFactory,
//                            baenet_HttpStatusCode::BAENET_OK,
//                            "OK",
//                            responseBody);
//       ASSERT(0 == rc);
//
//       responseBuffer.pubsync();
//       ASSERT(0 != response.length());
//
//       if (veryVerbose) {
//           bsl::cout << "\n\nDelivered response:\n"
//                     << bcema_BlobUtilAsciiDumper(&response) 
//                     << bsl::endl;
//       }
//   }
//
//   // Encode a response using the chunked transfer encoding.
//
//   {
//       bcema_Blob              response(&blobBufferFactory);
//       bsl::vector<bcema_Blob> chunkList;
//       const int               NUM_CHUNKS = 3;
//
//       {
//           for (int i = 0; i < NUM_CHUNKS; ++i)
//           {
//               const char DATA[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//               const int  SIZE   = sizeof DATA - 1;
//
//               bcema_SharedPtr<char> bufferStorage = 
//                    bcema_SharedPtrUtil::
//                    createInplaceUninitializedBuffer(SIZE, &ta);
//
//               bcema_BlobBuffer buffer;
//               buffer.reset(bufferStorage, SIZE);
//
//               bsl::memcpy(buffer.data(), DATA, SIZE);
//
//               bcema_Blob chunk;
//               chunk.appendDataBuffer(buffer);
//
//               chunkList.push_back(chunk);
//           }
//       }
//
//       bcesb_OutBlobStreamBuf responseBuffer(&response);
//
//       rc = deliverResponse(&responseBuffer,
//                            &blobBufferFactory,
//                            baenet_HttpStatusCode::BAENET_OK,
//                            "OK",
//                            &chunkList[0],
//                            chunkList.size());
//       ASSERT(0 == rc);
//
//       responseBuffer.pubsync();
//       ASSERT(0 != response.length());
//
//       if (veryVerbose) {
//           bsl::cout << "\n\nDelivered response:\n"
//                     << bcema_BlobUtilAsciiDumper(&response) 
//                     << bsl::endl;
//       }
//   }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPGENERATORUTIL
#include <baenet_httpgeneratorutil.h>
#endif

#ifndef INCLUDED_BAENET_HTTPTRANSFERENCODING
#include <baenet_httptransferencoding.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                     // =================================
                     // class baenet_HttpMessageGenerator
                     // =================================

class baenet_HttpMessageGenerator {
    // This class is used to generate an HTTP message.

  public:
    // TYPES
    typedef bdef_Function<void(*)(const bcema_Blob& data)> MessageDataCallback;
        // Defines a type alias for the functor invoked when a fragment of the
        // HTTP message is generated.

  private:
    // PRIVATE DATA MEMBERS
    bcema_BlobBufferFactory            *d_blobBufferFactory_p;
    MessageDataCallback                 d_messageDataCallback;
    baenet_HttpTransferEncoding::Value  d_transferEncoding;

  private:
    // NOT IMPLEMENTED
    baenet_HttpMessageGenerator(const baenet_HttpMessageGenerator&);
    baenet_HttpMessageGenerator& operator=(const baenet_HttpMessageGenerator&);

  public:
    // CREATORS
    explicit baenet_HttpMessageGenerator(
                                  bcema_BlobBufferFactory *blobBufferFactory,
                                  bslma_Allocator         *basicAllocator = 0);
        // Create an HTTP message generator and use the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator will be used.

    ~baenet_HttpMessageGenerator();
        // Destroy this object.

    // MANIPULATORS
    template <typename STARTLINE_TYPE, typename HEADER_TYPE>
    int startEntity(const STARTLINE_TYPE&      startLine,
                    const HEADER_TYPE&         header,
                    const MessageDataCallback& messageDataCallback);
        // Start a new entity using the specified 'startLine' and the specified
        // 'header'.  Invoke the specified 'messageDataCallback' after each
        // fragment of the HTTP message is formatted.  Return 0 on success, 
        // and a non-zero value otherwise. 

    int addEntityData(const bcema_Blob& data);
        // Add the specified entity 'data' to the current message.  Return 0 on
        // success, and a non-zero value otherwise.

    int endEntity();
        // End the current entity.  Return 0 on success, and a non-zero value
        // otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // class baenet_HttpMessageGenerator
                     // ---------------------------------

// CREATORS
inline
baenet_HttpMessageGenerator::baenet_HttpMessageGenerator(
                                    bcema_BlobBufferFactory *blobBufferFactory,
                                    bslma_Allocator         *basicAllocator)
: d_blobBufferFactory_p(blobBufferFactory)
, d_messageDataCallback(basicAllocator)
, d_transferEncoding(baenet_HttpTransferEncoding::BAENET_IDENTITY)
{
    BSLS_ASSERT_SAFE(d_blobBufferFactory_p);
}

inline
baenet_HttpMessageGenerator::~baenet_HttpMessageGenerator()
{
}

// MANIPULATORS
template <typename STARTLINE_TYPE, typename HEADER_TYPE>
int baenet_HttpMessageGenerator::startEntity(
                                const STARTLINE_TYPE&      startLine,
                                const HEADER_TYPE&         header,
                                const MessageDataCallback& messageDataCallback)
{
    d_messageDataCallback = messageDataCallback;
    BSLS_ASSERT_SAFE(d_messageDataCallback);

    const int numTransferEncodings = static_cast<int>(
                               header.basicFields().transferEncoding().size());

    if (2 <= numTransferEncodings) {
        return -1;
    }

    if (1 == numTransferEncodings) {
        d_transferEncoding = header.basicFields().transferEncoding()[0];
    }

    if (d_transferEncoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (header.basicFields().contentLength().isNull() || 
            header.basicFields().contentLength().value()  != 0) 
        {
            return -2;
        }
    }

    bcema_Blob data(d_blobBufferFactory_p);
    rc = baenet_HttpGeneratorUtil::generateHeader(&data, startLine, header);
    if (0 != rc) {
        return rc;
    }

    d_messageDataCallback(data);
    return 0;
}
 
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
