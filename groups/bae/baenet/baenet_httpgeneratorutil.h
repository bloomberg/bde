// baenet_httpgeneratorutil.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPGENERATORUTIL
#define INCLUDED_BAENET_HTTPGENERATORUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for generating HTTP entities
//
//@CLASSES:
//  baenet_HttpGeneratorUtil: functions to generate HTTP entities
//
//@AUTHOR: Matthew Millett (mmillett2@bloomberg.net)
//
//@DESCRIPTION:  This component provides a set of functions to generate HTTP
// entities in part or in whole.  An HTTP entity refers to the information
// transmitted as the payload of a request or response, and consists of 
// meta-data in the form of entity header fields and content in the form of
// a (potentially encoded) entity body.  For a complete description of the HTTP
// protocol see, RFC 2616 at http://www.ietf.org/rfc/rfc2616.txt. 
//
///Transfer Encoding
///-----------------
// RFC 2616 defines a coding of an entity body that allows a sender to 
// transmit the body in "chunks", enabling a server to construct and transmit
// the entity asynchronously.  This component supports a usage to chunk-encode
// an entity body; see the usage example for more details.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we are implementing a simple HTTP server that serves files to
// clients.  The following 'handleRequest' function accepts the name of the
// requested file and also a pointer to a stream buffer to send the response
// to:
//..
//  void handleRequest(bsl::streambuf     *connectionToClient,
//                     const bsl::string&  filename);
//..
// We also need a 'handleError' function that we will define later:
//..
//  void handleError(bsl::streambuf     *connectionToClient,
//                   const bsl::string&  body);
//..
// Now let's implement the 'handleRequest' function to read the file from disk
// in chunks, transmitting each chunk to the client using the chunked 
// transfer coding.
//..
//  void handleRequest(bsl::streambuf     *connectionToClient,
//                     const bsl::string&  filename)
//  {
//      bsl::fstream file(filename.c_str(), bsl::ios_base::in);
//
//      if (!file.is_open()) {
//          bsl::stringstream ss;
//          ss << "The file '" << requestedFileName << "' was not found.";
//
//          handleError(connectionToClient, ss.str());
//          return;
//      }
//
//      baenet_HttpResponseHeader header;
//      baenet_HttpStatusLine     statusLine;
//
//      statusLine.statusCode()   = baenet_HttpStatusCode::OK;
//      statusLine.reasonPhrase() = "OK";
//
//      header.generalFields().transferEncoding() = 
//                                baenet_HttpTransferEncoding::BAENET_CHUNKED;
//
//      int retCode = baenet_HttpGeneratorUtil::generateHeader(
//                                                       connectionToClient,
//                                                       statusLine,
//                                                       header);
//      assert(0 == retCode);
//
//      do {
//          bdex_ByteInStreamFormatter bisf(file.rdbuf());
//          bcema_Blob                 data;
//
//          bcema_BlobUtil::read(bisf, &data, 1024);
//
//          bool isFinal = (file.peek() == bsl::fstream::traits_type::eof());
//
//          retCode = baenet_HttpGeneratorUtil::generateBody(
//                                 connectionToClient,
//                                 data,
//                                 baenet_HttpTransferEncoding::BAENET_CHUNKED,
//                                 isFinal);
//          assert(0 == retCode);
//      }
//      while (!isFinal);
//  }
//..
// Now let's implement the 'handleError' function to synchronous construct
// an HTTP entity that describes the error loading the file from disk.
//..
//  void handleError(bsl::streambuf     *connectionToClient,
//                   const bsl::string&  body)
//  {
//      int retCode;
//
//      baenet_HttpResponseHeader header;
//      baenet_HttpStatusLine     statusLine;
//
//      statusLine.statusCode()   = baenet_HttpStatusCode::NOT_FOUND;
//      statusLine.reasonPhrase() = "File not found";
//
//      header.entityFields().contentLength() = body.size();
//
//      retCode = baenet_HttpGeneratorUtil::generateHeader(connectionToClient,
//                                                         statusLine,
//                                                         header);
//      assert(0 == retCode);
//
//      retCode = baenet_HttpGeneratorUtil::generateBody(connectionToClient,
//                                                       reason.data(),
//                                                       reason.size());
//      assert(0 == retCode);
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPTRANSERENCODING
#include <baenet_httptransferencoding.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bcema_Blob;

class baenet_HttpRequestHeader;
class baenet_HttpRequestLine;
class baenet_HttpResponseHeader;
class baenet_HttpStatusLine;

                     // ===============================
                     // struct baenet_HttpGeneratorUtil
                     // ===============================

struct baenet_HttpGeneratorUtil {
    // This struct provides a suite of utilities to encode HTTP messages.

    // CLASS METHODS    
    static int generateHeader(bcema_Blob                      *result,
                              const baenet_HttpRequestLine&    requestLine,
                              const baenet_HttpRequestHeader&  header);
        // Append to the specified 'result' the beginning of a new HTTP
        // entity having the specified 'requestLine' and header fields 
        // contained in the specified request 'header'.  Return 0 on success
        // and a non-zero value otherwise.

    static int generateHeader(bsl::streambuf                  *destination,
                              const baenet_HttpRequestLine&    requestLine,
                              const baenet_HttpRequestHeader&  header);
        // Write to the specified 'destination' the beginning of a new
        // HTTP entity having the specified 'requestLine' and header fields 
        // contained in the specified request 'header'.  Return 0 on success 
        // and a non-zero value otherwise.
  
    static int generateHeader(bcema_Blob                       *result,
                              const baenet_HttpStatusLine&      statusLine,
                              const baenet_HttpResponseHeader&  header);
        // Append to the specified 'result' the beginning of a new HTTP entity
        // having the specified 'statusLine' and header fields contained in
        // the specified response 'header'.  Return 0 on success and a non-zero
        // value otherwise.

    static int generateHeader(bsl::streambuf                   *destination,
                              const baenet_HttpStatusLine&      statusLine,
                              const baenet_HttpResponseHeader&  header);
        // Write to the specified 'destination' the beginning of a new HTTP
        // entity having the specified 'statusLine' and header fields 
        // contained in the specified response 'header'.  Return 0 on success 
        // and a non-zero value otherwise.

    static int generateBody(bcema_Blob                         *result,
                            const bcema_Blob&                   data);
        // Append to the specified 'result' the HTTP entity content defined
        // by the specified 'data'.  Note that the entity content is not
        // transfer-encoded.  Return 0 on success and non-zero value otherwise.

    static int generateBody(bcema_Blob                         *result,
                            const void                         *data,
                            int                                 numBytes);
        // Append to the specified 'result' the HTTP entity content defined
        // by the specified 'numBytes' of 'data'.  Note that the entity 
        // content is not transfer-encoded.  Return 0 on success and non-zero
        // value otherwise.

    static int generateBody(bsl::streambuf                     *destination,
                            const bcema_Blob&                   data);
        // Write to the specified 'destination' the HTTP entity content defined
        // by the specified 'data'.  Note that the entity content is not
        // transfer-encoded.  Return 0 on success and non-zero value otherwise.

     static int generateBody(bsl::streambuf                    *destination,
                             const void                        *data,
                             int                                numBytes);
        // Write to the specified 'destination' the HTTP entity content defined
        // by the specified 'numBytes' of 'data'.  Note that the entity 
        // content is not transfer-encoded.  Return 0 on success and non-zero
        // value otherwise.

    static int generateBody(bcema_Blob                         *result, 
                            const bcema_Blob&                   data,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinal);
        // Append to the specified 'result' the next portion of HTTP entity 
        // content defined by the specified 'data' transfer-encoded according
        // to the specified 'encoding'.  Specify an 'isFinal' flag to indicate
        // whether 'data' is the final portion of the HTTP entity content.  
        // Note that 'isFinal' is ignored unless 'encoding' refers to a 
        // chunked transfer coding.  Return 0 on success and a non-zero value
        // otherwise.

    static int generateBody(bcema_Blob                         *result, 
                            const void                         *data,
                            int                                 length,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinal);
        // Append to the specified 'result' the next portion of HTTP entity 
        // content defined by the specified 'data' having the specified
        // 'numBytes' of 'data' transfer-encoded according to the specified 
        // 'encoding'.  Specify an 'isFinal' flag to indicate whether 'data'
        // is the final portion of the HTTP entity content.  Note that 
        // 'isFinal' is ignored unless 'encoding' refers to a chunked transfer
        // coding.  Return 0 on success and a non-zero value otherwise.

    static int generateBody(bsl::streambuf                     *destination, 
                            const bcema_Blob&                   data,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinal);
        // Write to the specified 'destination' the next portion of HTTP 
        // entity content defined by the specified 'data' transfer-encoded 
        // according to the specified 'encoding'.  Specify an 'isFinal' flag
        // to indicate whether 'data' is the final portion of the HTTP entity
        // content.  Note that 'isFinal' is ignored unless 'encoding' refers
        // to a chunked transfer coding.  Return 0 on success and a non-zero
        // value otherwise.

    static int generateBody(bsl::streambuf                     *destination, 
                            const void                         *data,
                            int                                 length,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinal);
        // Write to the specified 'destination' the next portion of HTTP
        // entity content defined by the specified 'data' having the specified
        // 'numBytes' of 'data' transfer-encoded according to the specified 
        // 'encoding'.  Specify an 'isFinal' flag to indicate whether 'data'
        // is the final portion of the HTTP entity content.  Note that 
        // 'isFinal' is ignored unless 'encoding' refers to a chunked transfer
        // coding.  Return 0 on success and a non-zero value otherwise.
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

