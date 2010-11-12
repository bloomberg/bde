// baenet_httpmessageparser.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPMESSAGEPARSER
#define INCLUDED_BAENET_HTTPMESSAGEPARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for parsing HTTP messages.
//
//@CLASSES:
//  baenet_HttpMessageParser: HTTP message parser
//
//@SEE_ALSO:
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides a 'bcema_Blob'-based HTTP message
// parser.  Given an entity processor and continuous blobs of HTTP message
// data, the 'baenet_HttpMessageParser' class will parse the messages and
// invoke the 'onStartEntity', 'onEntityData', and 'onEndEntity' methods of the
// entity processor.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we are implementing an HTTP client that sends a request for a file,
// then saves the file locally.  We start by implementing an entity processor
// to do this:
//..
//  class EntityWriter : public baenet_HttpEntityProcessor {
//
//      // PRIVATE DATA MEMBERS
//      const char      *d_fileName;
//      bsl::ofstream    d_outputFile;
//      bslma_Allocator *d_allocator_p;
//
//    public:
//      // CREATORS
//      explicit EntityWriter(const char      *fileName,
//                            bslma_Allocator *basicAllocator = 0)
//      : d_fileName(fileName)
//      , d_allocator_p(bslma_Default::allocator(basicAllocator))
//      {
//      }
//
//      // MANIPULATORS
//      virtual void onStartEntity(
//                         const baenet_HttpStartLine&               startLine,
//                         const bcema_SharedPtr<baenet_HttpHeader>& header)
//      {
//          d_outputFile.open(d_fileName);
//      }
//
//      virtual void onEntityData(const bcema_Blob& data)
//      {
//          bdex_ByteOutStreamFormatter bosf(d_outputFile.rdbuf());
//          bcema_BlobUtil::write(bosf, data);
//      }
//
//      virtual void onEndEntity()
//      {
//          d_outputFile.close();
//      }
//
//      // ACCESSORS
//      virtual bcema_SharedPtr<baenet_HttpHeader> createHeader() const
//      {
//          bcema_SharedPtr<baenet_HttpResponseHeader> ret;
//          ret.createInplace(d_allocator_p, d_allocator_p);
//          return ret;
//      }
//  };
//..
// We can hook up the entity processor with a 'baenet_HttpMessageParser' as
// follows:
//..
//  EntityWriter             writer("filename.dat");
//  baenet_HttpMessageParser parser(&writer, baenet_HttpMessageType::RESPONSE);
//  bcema_Blob               data;
//
//  bool dataAvailable = readDataFromStream(&data);
//
//  while (dataAvailable) {
//      parser.addData(data);
//      dataAvailable = readDataFromStream(&data);
//  }
//
//  parser.onEndData();
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPHEADER
#include <baenet_httpheader.h>
#endif

#ifndef INCLUDED_BAENET_HTTPMESSAGETYPE
#include <baenet_httpmessagetype.h>
#endif

#ifndef INCLUDED_BAENET_HTTPSTARTLINE
#include <baenet_httpstartline.h>
#endif

#ifndef INCLUDED_BAENET_HTTPTRANSFERENCODING
#include <baenet_httptransferencoding.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

class bslma_Allocator;
class bdema_SequentialAllocator;
class bcema_Blob;
class baenet_HttpEntityProcessor;

                       // ==============================
                       // class baenet_HttpMessageParser
                       // ==============================

class baenet_HttpMessageParser {
    // Generic parser for both requests and responses.

    // PRIVATE TYPES
    typedef bcema_SharedPtr<baenet_HttpHeader> HeaderSharedPtr;

    // PRIVATE DATA MEMBERS
    bcema_Blob                          d_data;
    baenet_HttpEntityProcessor         *d_entityProcessor_p;
    HeaderSharedPtr                     d_header_sp;
    baenet_HttpMessageType::Value       d_messageType;
    int                                 d_numBytesRemaining;
    baenet_HttpStartLine                d_startLine;
    int                                 d_state;
    baenet_HttpTransferEncoding::Value  d_transferEncoding;
    int                                 d_remainingHeaderLength;

    // PRIVATE MANIPULATORS
    int parseStartLine(bsl::ostream&              errorStream,
                       bdema_SequentialAllocator *alloc);
    int parseHeaderField(bsl::ostream&              errorStream,
                         bdema_SequentialAllocator *alloc);
    int processHeader(bsl::ostream& errorStream);
    int getEntityData(bsl::ostream& errorStream, bcema_Blob *entityData);
    void prepareForNextMessage();
    int getChunkSize(bsl::ostream&  errorStream);

  private:
    // NOT IMPLEMENTED
    baenet_HttpMessageParser(const baenet_HttpMessageParser&);
    baenet_HttpMessageParser& operator=(const baenet_HttpMessageParser&);

  public:
    // CREATORS
    baenet_HttpMessageParser(
                            baenet_HttpEntityProcessor    *entityProcessor,
                            baenet_HttpMessageType::Value  messageType,
                            bslma_Allocator               *basicAllocator = 0);
        // Create an HTTP parser that uses the specified 'entityProcessor',
        // 'messageType', and 'basicAllocator'.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baenet_HttpMessageParser();
        // Destroy this object.

    // MANIPULATORS
    int addData(bsl::ostream& errorStream, const bcema_Blob& data);
        // Add the specified 'data' to the parser.  Write error messages on the
        // specified 'errorStream'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'onEndData' has been
        // called.

    int onEndData(bsl::ostream& errorStream);
        // Indicate that the message stream has been terminated.  Write error
        // messages on the specified 'errorStream'.  Return 0 on success, and a
        // non-zero value otherwise.

    void reset();
        // Reset this object back to its initial state.
};

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
