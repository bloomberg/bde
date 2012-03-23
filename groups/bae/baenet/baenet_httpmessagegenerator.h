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
//@SEE_ALSO: baenet_httpgeneratorutil
//
//@AUTHOR: Shezan Baig (sbaig), Matthew Millett (mmillett2)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides a mechanism,
// 'baenet_HttpMessageGenerator', to generate HTTP messages.  The API of this
// component facilitates "asynchronous" message generation required when the
// user does not have a representation of the entire message in memory.  This
// API is required for users wishing to generate HTTP messages using a
// chunked transfer coding.  Regardless of the transfer encoding used, users
// of the 'baenet_HttpMessageGenerator' class must generate HTTP messages by
// first calling 'startEntity' to begin generating a new HTTP entity.  Next,
// users must call 'addEntityData' one or more times to add data to the HTTP
// entity body.  Finally users must call 'endEntityData' to indicate to the
// mechanism that no further entity data is expected.  For a complete
// description of the HTTP protocol, including available transfer codings, see
// RFC 2616 at http://www.ietf.org/rfc/rfc2616.txt.  Also see
// 'baenet_httpgeneratorutil' for primitive generation functions.
//
///Thread Safety
///-------------
// The 'baenet_HttpMessageGenerator' class is is minimally thread safe, that
// is, it is safe for two threads two operate on two distinct objects of this
// class.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing an HTTP file server
/// - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a simple HTTP server that is responsible for
// delivering the contents of a file to a client.  For simplicity, let's
// assume the existence of mechanisms that enable the server implementation to
// parse a message arriving on a connection into a requested filename.  Given
// that assumption, let's now define a simple server API.  We start by
// forward-declaring the three functions that comprise our API:
// 'handleRequest', 'handleError', and 'writeToConnection'.  First, let's
// forward-declare a 'handleRequest' function that accepts the name of the
// requested file and also a pointer to a stream buffer to which to send the
// response.
//..
//  void handleRequest(bsl::streambuf          *connectionToClient,
//                     bcema_BlobBufferFactory *blobBufferFactory,
//                     const bsl::string&       filename);
//..
// Next, let's forward-declare a 'handleError' function that, instead of
// delivering the contents of a file, delivers some sort of error error to the
// client.
//..
//  void handleError(bsl::streambuf          *connectionToClient,
//                   bcema_BlobBufferFactory *blobBufferFactory,
//                   const bsl::string&       body);
//..
// Finally, let's forward-declare callback function to supply to the
// 'baenet_HttpMessageGenerator' mechanism to invoke when fragment of an HTTP
// entity has been generated.
//..
//  void writeToConnection(bsl::streambuf    *connectionToClient,
//                         const bcema_Blob&  data);
//..
// Now let's implement the 'handleRequest' function to read the file from disk
// and send it to the client.  Since the requested file may be larger than
// what can fit in the process's memory space, we'll read the file from disk
// in small chunks, transmitting each chunk to the client using the chunked
// transfer coding.  First, attempt to open a stream to the file, and, if
// unsuccessful, return an error to the client.
//..
//  void handleRequest(bsl::streambuf          *connectionToClient,
//                     bcema_BlobBufferFactory *blobBufferFactory,
//                     const bsl::string&       filename)
//  {
//      bsl::fstream file(filename.c_str(), bsl::ios_base::in);
//
//      if (!file.is_open()) {
//          bsl::stringstream ss;
//          ss << "The file '" << filename << "' was not found.";
//
//          handleError(connectionToClient, blobBufferFactory, ss.str());
//          return;
//      }
//..
// Next, define an HTTP header that indicates a successful response, delivered
// using the chunked transfer coding.
//..
//      baenet_HttpResponseHeader header;
//      baenet_HttpStatusLine     statusLine;
//
//      statusLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;
//      statusLine.reasonPhrase() = "OK";
//
//      header.basicFields().transferEncoding().push_back(
//                              baenet_HttpTransferEncoding::BAENET_CHUNKED);
//
//..
// Then, instantiate a 'baenet_HttpMessageGenerator' mechanism that writes each
// generated HTTP fragment to the 'connectionToClient' and begin generating
// the HTTP entity.
//..
//      baenet_HttpMessageGenerator generator(blobBufferFactory);
//
//      int retCode = generator.startEntity(
//                              statusLine,
//                              header,
//                              bdef_BindUtil::bind(&writeToConnection,
//                                                  connection,
//                                                  bdef_PlaceHolders::_1));
//      assert(0 == retCode);
//..
// Next, read the file into 1K buffers, transmitting each buffer as a "chunk"
// in the HTTP data stream.  Transmit the final "chunk" when no more data can
// be read from the file.
//..
//      bool isFinalFlag;
//      do {
//          bdex_ByteInStreamFormatter bisf(file.rdbuf());
//          bcema_Blob                 data(blobBufferFactory);
//
//          bcema_BlobUtil::read(bisf, &data, 1024);
//
//          isFinalFlag =
//                    (file.peek() == bsl::fstream::traits_type::eof());
//
//          retCode = generator.addEntityData(data);
//          assert(0 == retCode);
//      }
//      while (!isFinalFlag);
//..
// Finally, indicate to the generation mechanism that no more HTTP entity data
// is expected.
//..
//      retCode = generator.endEntity();
//      assert(0 == retCode);
//  }
//..
// Next, let's implement the 'handleError' function to synchronously
// construct an HTTP entity that describes the error loading the file from
// disk.  Note that, unlike the implementation of 'handleRequest', we need
// not use any transfer coding since we have the entire response in memory.
//..
//  void handleError(bsl::streambuf          *connectionToClient,
//                   bcema_BlobBufferFactory *blobBufferFactory,
//                   const bsl::string&       body)
//  {
//      int retCode;
//
//      baenet_HttpResponseHeader header;
//      baenet_HttpStatusLine     statusLine;
//
//      statusLine.statusCode()   = baenet_HttpStatusCode::BAENET_NOT_FOUND;
//      statusLine.reasonPhrase() = "File not found";
//
//      header.basicFields().contentLength() = body.size();
//..
// Then, we again instantiate a 'baenet_HttpMessageGenerator' mechanism that
// writes each generated HTTP fragment to the 'connectionToClient'.
//..
//      baenet_HttpMessageGenerator generator(blobBufferFactory);
//
//      retCode = generator.startEntity(
//                              statusLine,
//                              header,
//                              bdef_BindUtil::bind(&writeToConnection,
//                                                  connectionToClient,
//                                                  bdef_PlaceHolders::_1));
//      assert(0 == retCode);
//
//      bcema_Blob data(blobBufferFactory);
//      bcema_BlobUtil::append(&data, body.c_str(), body.size());
//
//      retCode = generator.addEntityData(data);
//      assert(0 == retCode);
//
//      retCode = generator.endEntity();
//      assert(0 == retCode);
//  }
//..
// Finally, let's implement the function that writes a blob of data to the
// client.
//..
//  void writeToConnection(bsl::streambuf    *connectionToClient,
//                         const bcema_Blob&  data)
//  {
//      bdex_ByteOutStreamFormatter bosf(connectionToClient);
//      bcema_BlobUtil::write(bosf, data);
//  }
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
    // This class provides a mechanism to generate an HTTP message.  This
    // class is not thread safe.

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
    int startEntity(const baenet_HttpRequestLine&   requestLine,
                    const baenet_HttpRequestHeader& header,
                    const MessageDataCallback&      messageDataCallback);
        // Begin generating a new HTTP entity having the specified
        // 'requestLine' and 'header' fields.  Invoke the specified
        // 'messageDataCallback' after each fragment of the HTTP message is
        // formatted.  Return 0 on success and a non-zero value otherwise.

    int startEntity(const baenet_HttpStatusLine&     statusLine,
                    const baenet_HttpResponseHeader& header,
                    const MessageDataCallback&       messageDataCallback);
        // Begin generating a new HTTP entity having the specified 'statusLine'
        // and 'header' fields.  Invoke the specified 'messageDataCallback'
        // after each fragment of the HTTP message is formatted.  Return 0 on
        // success and a non-zero value otherwise.

    int addEntityData(const bcema_Blob& data);
        // Add the specified entity 'data' to the current entity.  Return 0 on
        // success, and a non-zero value otherwise.

    int endEntity();
        // End the current entity.  Return 0 on success, and a non-zero value
        // otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

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
