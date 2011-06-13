// baenet_httpgeneratorutil.t.cpp                                  -*-C++-*-
#include <baenet_httpgeneratorutil.h>
#include <baenet_httpgeneratorutil.h>

#include <baenet_httpbasicheaderfields.h>
#include <baenet_httpcontenttype.h>
#include <baenet_httpentityprocessor.h>
#include <baenet_httpextendedheaderfields.h>
#include <baenet_httpheader.h>
#include <baenet_httphost.h>
#include <baenet_httpmessageparser.h>
#include <baenet_httprequestheader.h>
#include <baenet_httprequestheaderfields.h>
#include <baenet_httprequestline.h>
#include <baenet_httprequestmethod.h>
#include <baenet_httpresponseheader.h>
#include <baenet_httpresponseheaderfields.h>
#include <baenet_httpstartline.h>
#include <baenet_httpstatuscode.h>
#include <baenet_httpstatusline.h>
#include <baenet_httptransferencoding.h>
#include <baenet_httpviarecord.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>

#include <bdet_datetimetz.h>
#include <bdetu_systemtime.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under tests provides utilities to generate HTTP messages. 
// This test driver ensures that this component generates valid HTTP headers 
// as defined in RFC 2616.  It also ensure that this component generates 
// properly "chunked" content when the "chunked" transfer encoding is 
// specified.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1] TESTING REQUEST GENERATION WITH NO TRANSFER ENCODING
// [2] TESTING RESPONSE GENERATION WITH NO TRANSFER ENCODING
// [3] TESTING REQUEST GENERATION WITH CHUNKED TRANSFER ENCODING
// [4] TESTING RESPONSE GENERATION WITH CHUNKED TRANSFER ENCODING
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

const int HTTP_MAJOR_VERSION = 1;
const int HTTP_MINOR_VERSION = 1;

const char REQUEST_URI[] = "/test";

//=============================================================================
//                            CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                          // ======================
                          // class RequestAssembler
                          // ======================

class RequestAssembler : public baenet_HttpEntityProcessor {
    // Test entity processor.

    // PRIVATE DATA MEMBERS
    baenet_HttpRequestLine   *d_requestLine_p;
    baenet_HttpRequestHeader *d_header_p;
    bcema_Blob               *d_body_p;
    bool                     *d_isCompleteFlag_p;
    bsl::ostream             *d_diagnostics_p;
    bslma_Allocator          *d_allocator_p;

  public:
    // CREATORS
    explicit RequestAssembler(bsl::ostream             *diagnostics,
                              baenet_HttpRequestLine   *requestLine,
                              baenet_HttpRequestHeader *header,
                              bcema_Blob               *body,
                              bool                     *isCompleteFlag,
                              bslma_Allocator          *basicAllocator = 0)
    : d_requestLine_p(requestLine)
    , d_header_p(header)
    , d_body_p(body)
    , d_isCompleteFlag_p(isCompleteFlag)
    , d_diagnostics_p(diagnostics)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        *d_isCompleteFlag_p = false;
    }

    virtual ~RequestAssembler()
    {
    }

    // MANIPULATORS
    virtual void onStartEntity(
                           const baenet_HttpStartLine&               startLine,
                           const bcema_SharedPtr<baenet_HttpHeader>& header)
    {
        BSLS_ASSERT(d_requestLine_p);
        BSLS_ASSERT(d_header_p);
        BSLS_ASSERT(startLine.isRequestLineValue());

        *d_requestLine_p = startLine.requestLine();
        
        baenet_HttpRequestHeader *requestHeader = 
            reinterpret_cast<baenet_HttpRequestHeader*>(header.ptr());
        BSLS_ASSERT(requestHeader);

        *d_header_p = *requestHeader;
    }

    virtual void onEntityData(const bcema_Blob& data)
    {
        bcema_BlobUtil::append(d_body_p, data);
    }

    virtual void onEndEntity()
    {
        *d_isCompleteFlag_p = true;
    }

    // ACCESSORS
    virtual bcema_SharedPtr<baenet_HttpHeader> createHeader() const
    {
        bcema_SharedPtr<baenet_HttpRequestHeader> result;
        result.createInplace(d_allocator_p, d_allocator_p);
        return result;
    }
};

                        // =======================
                        // class ResponseAssembler
                        // =======================

class ResponseAssembler : public baenet_HttpEntityProcessor {
    // Test entity processor.

    // PRIVATE DATA MEMBERS
    baenet_HttpStatusLine     *d_statusLine_p;
    baenet_HttpResponseHeader *d_header_p;
    bcema_Blob                *d_body_p;
    bool                      *d_isCompleteFlag_p;
    bsl::ostream              *d_diagnostics_p;
    bslma_Allocator           *d_allocator_p;

  public:
    // CREATORS
    explicit ResponseAssembler(bsl::ostream              *diagnostics,
                               baenet_HttpStatusLine     *requestLine,
                               baenet_HttpResponseHeader *header,
                               bcema_Blob                *body,
                               bool                      *isCompleteFlag,
                               bslma_Allocator           *basicAllocator = 0)
    : d_statusLine_p(requestLine)
    , d_header_p(header)
    , d_body_p(body)
    , d_isCompleteFlag_p(isCompleteFlag)
    , d_diagnostics_p(diagnostics)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        *d_isCompleteFlag_p = false;
    }

    virtual ~ResponseAssembler()
    {
    }

    // MANIPULATORS
    virtual void onStartEntity(
                           const baenet_HttpStartLine&               startLine,
                           const bcema_SharedPtr<baenet_HttpHeader>& header)
    {
        BSLS_ASSERT(d_statusLine_p);
        BSLS_ASSERT(d_header_p);
        BSLS_ASSERT(startLine.isStatusLineValue());

        *d_statusLine_p = startLine.statusLine();
        
        baenet_HttpResponseHeader *responseHeader = 
            reinterpret_cast<baenet_HttpResponseHeader*>(header.ptr());
        BSLS_ASSERT(responseHeader);

        *d_header_p = *responseHeader;
    }

    virtual void onEntityData(const bcema_Blob& data)
    {
        bcema_BlobUtil::append(d_body_p, data);
    }

    virtual void onEndEntity()
    {
        *d_isCompleteFlag_p = true;
    }

    // ACCESSORS
    virtual bcema_SharedPtr<baenet_HttpHeader> createHeader() const
    {
        bcema_SharedPtr<baenet_HttpResponseHeader> result;
        result.createInplace(d_allocator_p, d_allocator_p);
        return result;
    }
};

//=============================================================================
//                          GLOBAL HELPER FUNCTIONS
//-----------------------------------------------------------------------------

void printBlob(const bcema_Blob& data)
{
    bcema_BlobUtil::hexDump(bsl::cout, data);
}

char generateByte(int position, int dataset)
{
    struct {
        const char *source;
        int         length;
    }
    DATA[] = {
        { "0123456789",                 10 },
        { "abcdefghijklmnopqrstuvwxyz", 26 },
        { "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 }
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

    dataset = dataset % NUM_DATA;
    return DATA[dataset].source[position % DATA[dataset].length];
}

void generateData(bcema_Blob *result, int offset, int size, int dataset)
{
    result->removeAll();
    result->setLength(size);

    int k = 0;

    for (int i = 0; i < result->numDataBuffers(); ++i) {
        const bcema_BlobBuffer& buffer = result->buffer(i);

        int numBytesToWrite = i == result->numDataBuffers() - 1
                            ? result->lastDataBufferLength()
                            : buffer.size();

        for (int j = 0; j < numBytesToWrite; ++j) {
            buffer.data()[j] = generateByte(offset + k, dataset);
            ++k;
        }
    }
}

void defineRequest(baenet_HttpRequestLine   *requestLine,
                   baenet_HttpRequestHeader *header,
                   bcema_Blob               *body,
                   int                       requestId,
                   int                       messageSize)
{
    typedef baenet_HttpRequestMethod RM;

    bdet_Datetime nowAsDatetime = bdetu_SystemTime::nowAsDatetimeGMT();
        
    // The HTML Parser does not parse the milliseconds field.
    nowAsDatetime.setMillisecond(0);

    bdet_DatetimeTz now = bdet_DatetimeTz(nowAsDatetime, 0);

    // Generate the body.

    generateData(body, 0, messageSize, requestId);

    // Define the request line.

    requestLine->majorVersion() = HTTP_MAJOR_VERSION;
    requestLine->minorVersion() = HTTP_MINOR_VERSION;

    requestLine->method()       = RM::BAENET_POST;
    requestLine->requestUri()   = REQUEST_URI;

    // Define the basic header fields.

    header->basicFields().cacheControl() = "private, community=\"UCI\"";

    header->basicFields().connection() = "Keep-Alive";

    header->basicFields().date() = now;

    header->basicFields().pragma() = "no-cache";

    header->basicFields().trailer() = "pragma";

    header->basicFields().transferEncoding().push_back(
                                baenet_HttpTransferEncoding::BAENET_IDENTITY);

    header->basicFields().upgrade() = "HTTP/2.0";

    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "alpha";

        header->basicFields().via().push_back(via);
    }
    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "beta";

        header->basicFields().via().push_back(via);
    }
    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "gamma";

        header->basicFields().via().push_back(via);
    }
   
    header->basicFields().warning() = "199 alpha \"test1\"";

    header->basicFields().allow().push_back(RM::BAENET_GET);
    header->basicFields().allow().push_back(RM::BAENET_POST);

    header->basicFields().contentEncoding().push_back("identity");

    header->basicFields().contentLanguage().push_back("en");

    header->basicFields().contentLength() = body->length();

    header->basicFields().contentLocation() = "resource.html";

    header->basicFields().contentMd5() = "e72c504dc16c8fcd2fe8c74bb492affa";

    header->basicFields().contentRange() = "bytes 0-499/1234";

    baenet_HttpContentType& contentType = header->basicFields().contentType()
                                                               .makeValue();
    contentType.type()    = "application";
    contentType.subType() = "octet-stream";

    header->basicFields().expires()      = now;
    header->basicFields().lastModified() = now;

    // Define the request header fields.

    header->requestFields().accept().push_back("application/octet-stream");
    header->requestFields().accept().push_back("text/plain");

    header->requestFields().acceptCharset().push_back("iso-8859-5");
    header->requestFields().acceptCharset().push_back("unicode-1-1");

    header->requestFields().acceptEncoding().push_back("*");
    header->requestFields().acceptEncoding().push_back("compress");
    header->requestFields().acceptEncoding().push_back("gzip");

    header->requestFields().acceptLanguage().push_back("da");
    header->requestFields().acceptLanguage().push_back("en-gb");
    header->requestFields().acceptLanguage().push_back("en");

    header->requestFields().authorization() = "John Doe";

    header->requestFields().expect() = "100-continue";
    header->requestFields().from()   = "webmaster@w3.org";

    baenet_HttpHost& host = header->requestFields().host().makeValue();
    host.name() = "localhost";
    host.port() = 80;

    header->requestFields().ifMatch()           = "*";
    header->requestFields().ifModifiedSince()   = now; 
    header->requestFields().ifNoneMatch()       = "*";
    header->requestFields().ifRange()           = "*";
    header->requestFields().ifUnmodifiedSince() = now;  

    header->requestFields().maxForwards() = 10;

    header->requestFields().proxyAuthorization() = "John Doe";

    header->requestFields().range() = "bytes=0-499";

    header->requestFields().referer() = "http://w3.org/source.html";

    header->requestFields().te() = "trailers, deflate";

    header->requestFields().userAgent() = "CERN-LineMode/2.15 libwww/2.17b3";

    // Define extension header fields.
    
    header->extendedFields().addFieldValue("X-BBG-ALPHA", "1");
    header->extendedFields().addFieldValue("X-BBG-BETA",  "2");
    header->extendedFields().addFieldValue("X-BBG-GAMMA", "3");
}

void defineResponse(baenet_HttpStatusLine     *statusLine,
                    baenet_HttpResponseHeader *header,
                    bcema_Blob                *body,
                    int                        requestId,
                    int                        messageSize)
{
    typedef baenet_HttpRequestMethod RM;

    bdet_Datetime nowAsDatetime = bdetu_SystemTime::nowAsDatetimeGMT();
        
    // The HTML Parser does not parse the milliseconds field.
    nowAsDatetime.setMillisecond(0);

    bdet_DatetimeTz now = bdet_DatetimeTz(nowAsDatetime, 0);

    // Generate the body.

    generateData(body, 0, messageSize, requestId);

    // Define the status line.
    
    statusLine->majorVersion() = HTTP_MAJOR_VERSION;
    statusLine->minorVersion() = HTTP_MINOR_VERSION;

    statusLine->statusCode()   = baenet_HttpStatusCode::BAENET_OK;
    statusLine->reasonPhrase() = "OK";

    // Define the basic header fields.

    header->basicFields().cacheControl() = "private, community=\"UCI\"";

    header->basicFields().connection() = "Keep-Alive";

    header->basicFields().date() = now;

    header->basicFields().pragma() = "no-cache";

    header->basicFields().trailer() = "pragma";

    header->basicFields().transferEncoding().push_back(
                                baenet_HttpTransferEncoding::BAENET_IDENTITY);

    header->basicFields().upgrade() = "HTTP/2.0";

    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "alpha";

        header->basicFields().via().push_back(via);
    }
    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "beta";

        header->basicFields().via().push_back(via);
    }
    {
        baenet_HttpViaRecord via;
        via.protocolName()    = "HTTP";
        via.protocolVersion() = "1.0";
        via.viaHost().name()  = "gamma";

        header->basicFields().via().push_back(via);
    }
   
    header->basicFields().warning() = "199 alpha \"test1\"";

    header->basicFields().allow().push_back(RM::BAENET_GET);
    header->basicFields().allow().push_back(RM::BAENET_POST);

    header->basicFields().contentEncoding().push_back("identity");

    header->basicFields().contentLanguage().push_back("en");

    header->basicFields().contentLength() = body->length();

    header->basicFields().contentLocation() = "resource.html";

    header->basicFields().contentMd5() = "e72c504dc16c8fcd2fe8c74bb492affa";

    header->basicFields().contentRange() = "bytes 0-499/1234";

    baenet_HttpContentType& contentType = header->basicFields().contentType()
                                                               .makeValue();
    contentType.type()    = "application";
    contentType.subType() = "octet-stream";

    header->basicFields().expires()      = now;
    header->basicFields().lastModified() = now;

    // Define the response header fields.
    
    header->responseFields().acceptRanges() = "bytes";

    header->responseFields().age() = 60;

    header->responseFields().eTag() = "W/\"xyzzy\"";

    header->responseFields().location() = "http://ww3.org/resource.html";

    header->responseFields().proxyAuthenticate() = "challange";

    header->responseFields().retryAfter() = "Fri, 31 Dec 1999 23:59:59 GMT";

    header->responseFields().server() = "CERN/3.0 libwww/2.17";

    header->responseFields().vary() = "*";

    header->responseFields().wwwAuthenticate() = "challange";

    // Define extension header fields.
    
    header->extendedFields().addFieldValue("X-BBG-ALPHA", "1");
    header->extendedFields().addFieldValue("X-BBG-BETA",  "2");
    header->extendedFields().addFieldValue("X-BBG-GAMMA", "3");
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING RESPONSE GENERATION WITH CHUNKED TRANSFER ENCODING
        //
        // Concerns:
        //   The generation utilities produce a valid HTTP 1.1 response using
        //   the chunked transfer encoding.
        //
        // Plan:
        //   Define an HTTP response containing a well-formed header a response
        //   intended to be chunked-encoded.  Generate an HTTP response into
        //   a blob.  Add that blob to a message parsing mechanism, and ensure
        //   the parsed response matches the originally defined response.  
        //   Repeat the test using 'bsl::streambuf's instead of blobs.
        // --------------------------------------------------------------------

        enum { TEST_BLOB    = 0,   TEST_STREAMBUF = 1 };
        enum { MESSAGE_SIZE = 128, BUFFER_SIZE    = 7 };

        int rc;

        for (int iteration = 0; iteration < 2; ++iteration) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);

            baenet_HttpStatusLine     statusLine1;
            baenet_HttpStatusLine     statusLine2;
            baenet_HttpResponseHeader header1;
            baenet_HttpResponseHeader header2;
            bcema_Blob                body1(&blobBufferFactory);
            bcema_Blob                body2(&blobBufferFactory);

            {
                statusLine1.majorVersion() = HTTP_MAJOR_VERSION;
                statusLine1.minorVersion() = HTTP_MINOR_VERSION;

                statusLine1.statusCode()   = baenet_HttpStatusCode::BAENET_OK;
                statusLine1.reasonPhrase() = "OK";

                header1.basicFields().transferEncoding().push_back(
                              baenet_HttpTransferEncoding::BAENET_CHUNKED);

                baenet_HttpContentType& contentType = 
                              header1.basicFields().contentType().makeValue();
                contentType.type()    = "application";
                contentType.subType() = "octet-stream";
 
                generateData(&body1, 0, MESSAGE_SIZE, 0);
            }

            bool isCompleteFlag;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

            if (iteration == TEST_BLOB) {
                bcema_Blob message(&blobBufferFactory);
                    
                rc = baenet_HttpGeneratorUtil::generateHeader(&message, 
                                                              statusLine1,
                                                              header1);
                ASSERT(0 == rc);

                for (int bufferIndex = 0; 
                         bufferIndex < body1.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer = 
                                                body1.buffer(bufferIndex);

                    int isFinal = bufferIndex == body1.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? body1.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &message, 
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (veryVerbose) {
                    bsl::cout << bcema_BlobUtilAsciiDumper(&message) 
                              << bsl::endl;
                }

                rc = parser.addData(bsl::cout, message);
                ASSERT(0 == rc);
            }
            else {
                ASSERT(iteration == TEST_STREAMBUF);

                bsl::stringstream ss;
                rc = baenet_HttpGeneratorUtil::generateHeader(ss.rdbuf(), 
                                                              statusLine1,
                                                              header1);
                ASSERT(0 == rc);

                for (int bufferIndex = 0; 
                         bufferIndex < body1.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer = 
                                                body1.buffer(bufferIndex);

                    int isFinal = bufferIndex == body1.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? body1.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            ss.rdbuf(), 
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (veryVerbose) {
                    bsl::cout << ss.str() << bsl::endl;
                }

                rc = parser.addData(bsl::cout, ss.rdbuf());
                ASSERT(0 == rc);
            }

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag);

            if (statusLine1 != statusLine2) {
                P(statusLine1);
                P(statusLine2);
            }
            ASSERT(statusLine1 == statusLine2);

            if (header1 != header2) {
                P(header1);
                P(header2);
            }
            ASSERT(header1 == header2);

            if (0 != bcema_BlobUtil::compare(body1, body2)) {
                bsl::cout << "body1: " 
                          << bcema_BlobUtilAsciiDumper(&body1) 
                          << bsl::endl;

                bsl::cout << "body2: " 
                          << bcema_BlobUtilAsciiDumper(&body2) 
                          << bsl::endl;
            }
            ASSERT(0 == bcema_BlobUtil::compare(body1, body2));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING REQUEST GENERATION WITH CHUNKED TRANSFER ENCODING
        //
        // Concerns:
        //   The generation utilities produce a valid HTTP 1.1 request using
        //   the chunked transfer encoding.
        //
        // Plan:
        //   Define an HTTP request containing a well-formed header a request
        //   intended to be chunked-encoded.  Generate an HTTP request into
        //   a blob.  Add that blob to a message parsing mechanism, and ensure
        //   the parsed request matches the originally defined request.  Repeat
        //   the test using 'bsl::streambuf's instead of blob.
        // --------------------------------------------------------------------

        enum { TEST_BLOB    = 0,   TEST_STREAMBUF = 1 };
        enum { MESSAGE_SIZE = 128, BUFFER_SIZE    = 7 };

        int rc;

        for (int iteration = 0; iteration < 2; ++iteration) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);

            baenet_HttpRequestLine   requestLine1;
            baenet_HttpRequestLine   requestLine2;
            baenet_HttpRequestHeader header1;
            baenet_HttpRequestHeader header2;
            bcema_Blob               body1(&blobBufferFactory);
            bcema_Blob               body2(&blobBufferFactory);

            {
                requestLine1.majorVersion() = HTTP_MAJOR_VERSION;
                requestLine1.minorVersion() = HTTP_MINOR_VERSION;

                requestLine1.method()       = baenet_HttpRequestMethod::
                                              BAENET_POST;
                requestLine1.requestUri()   = REQUEST_URI;

                header1.basicFields().transferEncoding().push_back(
                              baenet_HttpTransferEncoding::BAENET_CHUNKED);

                baenet_HttpContentType& contentType = 
                              header1.basicFields().contentType().makeValue();
                contentType.type()    = "application";
                contentType.subType() = "octet-stream";

                baenet_HttpHost& host = 
                              header1.requestFields().host().makeValue();
                host.name() = "localhost";
                host.port() = 80;

                generateData(&body1, 0, MESSAGE_SIZE, 0);
            }

            bool isCompleteFlag;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

            if (iteration == TEST_BLOB) {
                bcema_Blob message(&blobBufferFactory);
                    
                rc = baenet_HttpGeneratorUtil::generateHeader(&message, 
                                                              requestLine1,
                                                              header1);
                ASSERT(0 == rc);

                for (int bufferIndex = 0; 
                         bufferIndex < body1.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer = 
                                                body1.buffer(bufferIndex);

                    int isFinal = bufferIndex == body1.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? body1.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &message, 
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (veryVerbose) {
                    bsl::cout << bcema_BlobUtilAsciiDumper(&message) 
                              << bsl::endl;
                }

                rc = parser.addData(bsl::cout, message);
                ASSERT(0 == rc);
            }
            else {
                ASSERT(iteration == TEST_STREAMBUF);

                bsl::stringstream ss;
                rc = baenet_HttpGeneratorUtil::generateHeader(ss.rdbuf(), 
                                                              requestLine1,
                                                              header1);
                ASSERT(0 == rc);

                for (int bufferIndex = 0; 
                         bufferIndex < body1.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer = 
                                                body1.buffer(bufferIndex);

                    int isFinal = bufferIndex == body1.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? body1.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            ss.rdbuf(), 
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (veryVerbose) {
                    bsl::cout << ss.str() << bsl::endl;
                }

                rc = parser.addData(bsl::cout, ss.rdbuf());
                ASSERT(0 == rc);
            }

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag);

            if (requestLine1 != requestLine2) {
                P(requestLine1);
                P(requestLine2);
            }
            ASSERT(requestLine1 == requestLine2);

            if (header1 != header2) {
                P(header1);
                P(header2);
            }
            ASSERT(header1 == header2);

            if (0 != bcema_BlobUtil::compare(body1, body2)) {
                bsl::cout << "body1: " 
                          << bcema_BlobUtilAsciiDumper(&body1) 
                          << bsl::endl;

                bsl::cout << "body2: " 
                          << bcema_BlobUtilAsciiDumper(&body2) 
                          << bsl::endl;
            }
            ASSERT(0 == bcema_BlobUtil::compare(body1, body2));
        }
      } break;
      case 2: { 
        // --------------------------------------------------------------------
        // TESTING RESPONSE GENERATION WITH NO TRANSFER-ENCODING
        // 
        // Concerns:
        //   The generation utilities produce a valid HTTP 1.1 response using
        //   the identity transfer encoding.
        //
        // Plan:
        //   Define an HTTP response containing a well-formed header for each
        //   HTTP header pertaining to responses defined in RFC 2616.  Ensure
        //   no transfer encoding is specified.  Generate an HTTP response into
        //   a blob.  Add that blob to a message parsing mechanism, and ensure
        //   the parsed response matches the originally defined response.  
        //   Repeat the test using 'bsl::streambuf's instead of blob.
        // --------------------------------------------------------------------

        enum { TEST_BLOB = 0, TEST_STREAMBUF = 1 };

        int rc;

        for (int iteration = 0; iteration < 2; ++iteration) {
            bcema_PooledBlobBufferFactory blobBufferFactory(1024);

            baenet_HttpStatusLine     statusLine1;
            baenet_HttpStatusLine     statusLine2;
            baenet_HttpResponseHeader header1;
            baenet_HttpResponseHeader header2;
            bcema_Blob                body1(&blobBufferFactory);
            bcema_Blob                body2(&blobBufferFactory);

            defineResponse(&statusLine1, &header1, &body1, 0, 32);

            bool isCompleteFlag;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

            if (iteration == TEST_BLOB) {
                bcema_Blob message(&blobBufferFactory);

                rc = baenet_HttpGeneratorUtil::generateHeader(&message, 
                                                              statusLine1,
                                                              header1);
                ASSERT(0 == rc);

                rc = baenet_HttpGeneratorUtil::generateBody(&message, body1);
                ASSERT(0 == rc);

                if (verbose) {
                    bsl::cout << bcema_BlobUtilAsciiDumper(&message) 
                              << bsl::endl;
                }

                rc = parser.addData(bsl::cout, message);
                ASSERT(0 == rc);
            }
            else {
                ASSERT(iteration == TEST_STREAMBUF);

                bsl::stringstream ss;
                rc = baenet_HttpGeneratorUtil::generateHeader(ss.rdbuf(), 
                                                              statusLine1,
                                                              header1);
                ASSERT(0 == rc);

                rc = baenet_HttpGeneratorUtil::generateBody(ss.rdbuf(), body1);
                ASSERT(0 == rc);

                rc = ss.rdbuf()->pubsync();
                ASSERT(0 == rc);

                if (verbose) {
                    bsl::cout << ss.str() << bsl::endl;
                }

                rc = parser.addData(bsl::cout, ss.rdbuf());
                ASSERT(0 == rc);
            }

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag);

            if (statusLine1 != statusLine2) {
                P(statusLine1);
                P(statusLine2);
            }
            ASSERT(statusLine1 == statusLine2);

            if (header1 != header2) {
                P(header1);
                P(header2);
            }
            ASSERT(header1 == header2);

            if (0 != bcema_BlobUtil::compare(body1, body2)) {
                bsl::cout << "body1: " 
                          << bcema_BlobUtilAsciiDumper(&body1) 
                          << bsl::endl;

                bsl::cout << "body2: " 
                          << bcema_BlobUtilAsciiDumper(&body2) 
                          << bsl::endl;
            }
            ASSERT(0 == bcema_BlobUtil::compare(body1, body2));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING REQUEST GENERATION WITH NO TRANSFER ENCODING
        // 
        // Concerns:
        //   The generation utilities produce a valid HTTP 1.1 request using
        //   the identity transfer encoding.
        //
        // Plan:
        //   Define an HTTP request containing a well-formed header for each
        //   HTTP header pertaining to requests defined in RFC 2616.  Ensure
        //   no transfer encoding is specified.  Generate an HTTP request into
        //   a blob.  Add that blob to a message parsing mechanism, and ensure
        //   the parsed request matches the originally defined request.  Repeat
        //   the test using 'bsl::streambuf's instead of blob.
        // --------------------------------------------------------------------

        enum { TEST_BLOB = 0, TEST_STREAMBUF = 1 };

        int rc;

        for (int iteration = 0; iteration < 2; ++iteration) {
            bcema_PooledBlobBufferFactory blobBufferFactory(1024);

            baenet_HttpRequestLine   requestLine1;
            baenet_HttpRequestLine   requestLine2;
            baenet_HttpRequestHeader header1;
            baenet_HttpRequestHeader header2;
            bcema_Blob               body1(&blobBufferFactory);
            bcema_Blob               body2(&blobBufferFactory);

            defineRequest(&requestLine1, &header1, &body1, 0, 32);

            bool isCompleteFlag;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

            if (iteration == TEST_BLOB) {
                bcema_Blob message(&blobBufferFactory);

                rc = baenet_HttpGeneratorUtil::generateHeader(&message, 
                                                              requestLine1,
                                                              header1);
                ASSERT(0 == rc);

                rc = baenet_HttpGeneratorUtil::generateBody(&message, body1);
                ASSERT(0 == rc);

                if (verbose) {
                    bsl::cout << bcema_BlobUtilAsciiDumper(&message) 
                              << bsl::endl;
                }

                rc = parser.addData(bsl::cout, message);
                ASSERT(0 == rc);
            }
            else {
                ASSERT(iteration == TEST_STREAMBUF);

                bsl::stringstream ss;
                rc = baenet_HttpGeneratorUtil::generateHeader(ss.rdbuf(), 
                                                              requestLine1,
                                                              header1);
                ASSERT(0 == rc);

                rc = baenet_HttpGeneratorUtil::generateBody(ss.rdbuf(), body1);
                ASSERT(0 == rc);

                rc = ss.rdbuf()->pubsync();
                ASSERT(0 == rc);

                if (verbose) {
                    bsl::cout << ss.str() << bsl::endl;
                }

                rc = parser.addData(bsl::cout, ss.rdbuf());
                ASSERT(0 == rc);
            }

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag);

            if (requestLine1 != requestLine2) {
                P(requestLine1);
                P(requestLine2);
            }
            ASSERT(requestLine1 == requestLine2);

            if (header1 != header2) {
                P(header1);
                P(header2);
            }
            ASSERT(header1 == header2);

            if (0 != bcema_BlobUtil::compare(body1, body2)) {
                bsl::cout << "body1: " 
                          << bcema_BlobUtilAsciiDumper(&body1) 
                          << bsl::endl;

                bsl::cout << "body2: " 
                          << bcema_BlobUtilAsciiDumper(&body2) 
                          << bsl::endl;
            }
            ASSERT(0 == bcema_BlobUtil::compare(body1, body2));
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

