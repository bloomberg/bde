// baenet_httpmessagegenerator.cpp  -*-C++-*-
#include <baenet_httpmessagegenerator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpmessagegenerator_cpp,"$Id$ $CSID$")

#include <baenet_httprequestheader.h>   // for testing only
#include <baenet_httpresponseheader.h>  // for testing only

#include <bsls_assert.h>

namespace BloombergLP {

                     // ---------------------------------
                     // class baenet_HttpMessageGenerator
                     // ---------------------------------

// CREATORS
baenet_HttpMessageGenerator::baenet_HttpMessageGenerator(
                                    bcema_BlobBufferFactory *blobBufferFactory,
                                    bslma_Allocator         *basicAllocator)
: d_blobBufferFactory_p(blobBufferFactory)
, d_messageDataCallback(basicAllocator)
, d_transferEncoding(baenet_HttpTransferEncoding::BAENET_IDENTITY)
{
    BSLS_ASSERT(d_blobBufferFactory_p);
}

baenet_HttpMessageGenerator::~baenet_HttpMessageGenerator()
{
}

// MANIPULATORS
int baenet_HttpMessageGenerator::startEntity(
        const baenet_HttpRequestLine&   requestLine,
        const baenet_HttpRequestHeader& header,
        const MessageDataCallback&      messageDataCallback)
{
    BSLS_ASSERT(messageDataCallback);

    d_messageDataCallback = messageDataCallback;

    const int numTransferEncodings = static_cast<int>(
                               header.basicFields().transferEncoding().size());

    if (2 <= numTransferEncodings) {
        return -1;                                                    // RETURN
    }

    if (1 == numTransferEncodings) {
        d_transferEncoding = header.basicFields().transferEncoding()[0];
    }

    bcema_Blob data(d_blobBufferFactory_p);
    int rc = baenet_HttpGeneratorUtil::generateHeader(&data,
                                                      requestLine,
                                                      header);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    d_messageDataCallback(data);
    return 0;
}

int baenet_HttpMessageGenerator::startEntity(
        const baenet_HttpStatusLine&     statusLine,
        const baenet_HttpResponseHeader& header,
        const MessageDataCallback&       messageDataCallback)
{
    BSLS_ASSERT(messageDataCallback);

    d_messageDataCallback = messageDataCallback;

    const int numTransferEncodings = static_cast<int>(
                               header.basicFields().transferEncoding().size());

    if (2 <= numTransferEncodings) {
        return -1;                                                    // RETURN
    }

    if (1 == numTransferEncodings) {
        d_transferEncoding = header.basicFields().transferEncoding()[0];
    }

    if (d_transferEncoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (!header.basicFields().contentLength().isNull())
        {
            return -2;                                                // RETURN
        }
    }

    bcema_Blob data(d_blobBufferFactory_p);
    int rc = baenet_HttpGeneratorUtil::generateHeader(&data,
                                                      statusLine,
                                                      header);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    d_messageDataCallback(data);
    return 0;
}

int baenet_HttpMessageGenerator::addEntityData(const bcema_Blob& data)
{
    typedef baenet_HttpTransferEncoding TE;

    if (d_transferEncoding == TE::BAENET_IDENTITY) {
        // The identity encoding, by definition, does not transform the
        // body.  We can avoid the cost of copying blob buffers in calling
        // 'baenet_HttpGeneratorUtil::generateBody' by simply invoke the
        // callback with the argument directly.

        d_messageDataCallback(data);
    }
    else {
        bcema_Blob chunk(d_blobBufferFactory_p);
        int rc = baenet_HttpGeneratorUtil::generateBody(&chunk,
                                                        data,
                                                        d_transferEncoding,
                                                        false);
        if (0 != rc) {
            return rc;                                                // RETURN
        }

        d_messageDataCallback(chunk);
    }

    return 0;
}

int baenet_HttpMessageGenerator::endEntity()
{
    typedef baenet_HttpTransferEncoding TE;

    if (d_transferEncoding == TE::BAENET_CHUNKED) {
        bcema_Blob chunk(d_blobBufferFactory_p);
        int rc = baenet_HttpGeneratorUtil::generateBody(&chunk,
                                                        0,
                                                        0,
                                                        d_transferEncoding,
                                                        true);
        if (0 != rc) {
            return rc;                                                // RETURN
        }

        d_messageDataCallback(chunk);
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
