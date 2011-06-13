// baenet_httpmessagegenerator.cpp  -*-C++-*-
#include <baenet_httpmessagegenerator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpmessagegenerator_cpp,"$Id$ $CSID$")

#include <baenet_httpcontenttype.h>     // for testing only
#include <baenet_httpgeneratorutil.h>   // for testing only
#include <baenet_httphost.h>            // for testing only
#include <baenet_httprequestline.h>     // for testing only
#include <baenet_httpstatusline.h>      // for testing only
#include <baenet_httpviarecord.h>       // for testing only

#include <baenet_httprequestheader.h>   // for testing only
#include <baenet_httpresponseheader.h>  // for testing only

namespace BloombergLP {

                     // ---------------------------------
                     // class baenet_HttpMessageGenerator
                     // ---------------------------------

// MANIPULATORS
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
            return rc;
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
            return rc;
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
