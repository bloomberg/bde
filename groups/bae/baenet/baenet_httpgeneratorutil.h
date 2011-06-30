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
//@DESCRIPTION: The Hyper Text Transfer Protocol (HTTP) is a text-based
// protocol for exchanging information between peers in a request/response
// pattern.  The protocol refers to the information exchanged between peers as
// "entities".  Entities consist of meta-data in the form of entity header
// fields and content in the form of a (potentially encoded) entity body.  This
// component provides a utility, 'baenet_HttpGeneratorUtil', which defines two
// class methods, 'generateHeader' and 'generateBody', that may be used to to
// generate HTTP headers and bodies, respectively.  These function names are
// overloaded to support writing to destination 'bcema_Blob's and, more
// generically, to any customization of the 'bsl::streambuf' mechanism.  For a
// complete description of the HTTP protocol see,
// RFC 2616 at http://www.ietf.org/rfc/rfc2616.txt.
//
///Transfer Encoding
///-----------------
// RFC 2616 permits a sender of HTTP to apply a transfer coding to an entity
// body.  A transfer coding defines how the entity body a tranferred through
// the network.  RFC 2616 defines two standard codings: the "identity" coding
// and the "chunked" coding. The identity coding indicates the sender has
// performed no transformation of the entity body.  The chunked coding,
// however, allows a sender to transmit the body as a sequence of "chunks",
// where each chunk consists of a small header followed by a portion of the
// entity body optionally followed by a small footer.  The chunked transfer
// coding enables a sender to transmit earlier protions of an entity body
// before later portions are fully formed.  An example application of the
// chunked transfer coding is an HTTP server asychronously loading a large
// resource from disk.  If the size of the resource exceeds the memory
// constraints of the process the server can use the chunked transfer coding
// to load portions of the resource and transmit them as-needed.  The
// 'baenet_HttpGeneratorUtil' struct overloads a class method, 'generateBody',
// to chunk-encode an entity body; see the usage example for more details.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Writing an HTTP file server
/// - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a simple HTTP server that is responsible for
// delivering the contents of a file to a client.  For simplicity, let's
// assume the existence of mechanisms that enable the server implementation to
// parse a message arriving on a connection into a requested filename.  Given
// that assumption, let's now declare a simple server API.  The following
// 'handleRequest' function accepts the name of the requested file and also a
// pointer to a stream buffer to which to send the response.
//..
//  void handleRequest(bsl::streambuf     *connectionToClient,
//                     const bsl::string&  filename);
//..
// We also need a 'handleError' function that, instead of delivering the
// contents of a file, delivers some sort of error error to the client.
//..
//  void handleError(bsl::streambuf     *connectionToClient,
//                   const bsl::string&  body);
//..
// Now let's implement the 'handleRequest' function to read the file from disk
// and send it to the client.  Since the requested file may be larger than
// what can fit in the process's memory space, we'll read the file from disk
// in small chunks, transmitting each chunk to the client using the chunked
// transfer coding.  First, attempt to open a stream to the file, and, if
// unsuccessful, return an error to the client.
//..
//  void handleRequest(bsl::streambuf     *connectionToClient,
//                     const bsl::string&  filename)
//  {
//      bsl::fstream file(filename.c_str(), bsl::ios_base::in);
//
//      if (!file.is_open()) {
//          bsl::stringstream ss;
//          ss << "The file '" << filename << "' was not found.";
//
//          handleError(connectionToClient, ss.str());
//          return;
//      }
//..
// Next, define an HTTP header that indicates a successful response delivered
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
//      int retCode = baenet_HttpGeneratorUtil::generateHeader(
//                                                       connectionToClient,
//                                                       statusLine,
//                                                       header);
//      assert(0 == retCode);
//..
// Now, read the file into 1K buffers, transmitting each buffer as a "chunk"
// in the HTTP data stream.  Transmit the final "chunk" when no more data can
// be read from the file.
//..
//      bool isFinalFlag;
//      do {
//          bdex_ByteInStreamFormatter bisf(file.rdbuf());
//          bcema_Blob                 data;
//
//          bcema_BlobUtil::read(bisf, &data, 1024);
//
//          isFinalFlag =
//                    (file.peek() == bsl::fstream::traits_type::eof());
//
//          retCode = baenet_HttpGeneratorUtil::generateBody(
//                                 connectionToClient,
//                                 data,
//                                 baenet_HttpTransferEncoding::BAENET_CHUNKED,
//                                 isFinalFlag);
//          assert(0 == retCode);
//      }
//      while (!isFinalFlag);
//  }
//..
// Finally, let's implement the 'handleError' function to synchronously
// construct an HTTP entity that describes the error loading the file from
// disk.
//..
//  void handleError(bsl::streambuf     *connectionToClient,
//                   const bsl::string&  body)
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
//
//      retCode = baenet_HttpGeneratorUtil::generateHeader(connectionToClient,
//                                                         statusLine,
//                                                         header);
//      assert(0 == retCode);
//
//      retCode = baenet_HttpGeneratorUtil::generateBody(connectionToClient,
//                                                       body.c_str(),
//                                                       body.size());
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
    // This struct provides a suite of utilities to generate HTTP entities.
    // This struct is completely thread safe.

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
                            bool                                isFinalFlag);
        // Append to the specified 'result' the next portion of HTTP entity
        // content defined by the specified 'data' transfer-encoded according
        // to the specified 'encoding'.  Specify an 'isFinalFlag' to indicate
        // whether 'data' is the final portion of the HTTP entity content.
        // Note that 'isFinalFlag' is ignored unless 'encoding' refers to a
        // chunked transfer coding.  Return 0 on success and a non-zero value
        // otherwise.

    static int generateBody(bcema_Blob                         *result,
                            const void                         *data,
                            int                                 length,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinalFlag);
        // Append to the specified 'result' the next portion of HTTP entity
        // content defined by the specified 'data' having the specified
        // 'numBytes' of 'data' transfer-encoded according to the specified
        // 'encoding'.  Specify an 'isFinalFlag' flag to indicate whether
        // 'data' is the final portion of the HTTP entity content.  Note that
        // 'isFinalFlag' is ignored unless 'encoding' refers to a chunked
        // transfer coding.  Return 0 on success and a non-zero value
        // otherwise.

    static int generateBody(bsl::streambuf                     *destination,
                            const bcema_Blob&                   data,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinalFlag);
        // Write to the specified 'destination' the next portion of HTTP
        // entity content defined by the specified 'data' transfer-encoded
        // according to the specified 'encoding'.  Specify an 'isFinalFlag'
        // flag to indicate whether 'data' is the final portion of the HTTP
        // entity content.  Note that 'isFinalFlag' is ignored unless
        // 'encoding' refers to a chunked transfer coding.  Return 0 on
        // success and a non-zero value otherwise.

    static int generateBody(bsl::streambuf                     *destination,
                            const void                         *data,
                            int                                 length,
                            baenet_HttpTransferEncoding::Value  encoding,
                            bool                                isFinalFlag);
        // Write to the specified 'destination' the next portion of HTTP
        // entity content defined by the specified 'data' having the specified
        // 'numBytes' of 'data' transfer-encoded according to the specified
        // 'encoding'.  Specify an 'isFinalFlag' to indicate whether 'data'
        // is the final portion of the HTTP entity content.  Note that
        // 'isFinalFlag' is ignored unless 'encoding' refers to a chunked
        // transfer coding.  Return 0 on success and a non-zero value
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

