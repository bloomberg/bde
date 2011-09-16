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
#include <baenet_httpparserutil.h>
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
#include <bcesb_blobstreambuf.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>

#include <bdet_datetimetz.h>
#include <bdetu_systemtime.h>
#include <bdeu_print.h>
#include <bdex_byteoutstreamformatter.h>
#include <bdex_byteinstreamformatter.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_fstream.h>
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
// specified.  To exercise the utilities four data sets are used.
//
// Data Set     Description
// --------     -----------
// 0            Minimally correct header, zero length content
// 1            Fully defined header, zero length content
// 2            Minimally correct header, non-zero length content
// 3            Fully defined header, non-zero length content
//
// Where "minimally correct" refers to a header containing only those fields
// required by RFC 2616, and "fully defined" refers to a header containing
// every field specified in RFC 2616 plus additional HTTP extension headers
// defined by this test driver.
//
// This test driver ensures the validity of the generated data stream through
// the use of 'baenet_httpmessageparser' and 'baenet_httpparserutil'.
//-----------------------------------------------------------------------------
// [1,7,9]  int generateHeader(bcema_Blob                       *result,
//                             const baenet_HttpRequestLine&     requestLine,
//                             const baenet_HttpRequestHeader&   header);
//
// [2]      int generateHeader(bsl::streambuf                   *destination,
//                             const baenet_HttpRequestLine&     requestLine,
//                             const baenet_HttpRequestHeader&   header);
//
// [3,8,10] int generateHeader(bcema_Blob                       *result,
//                             const baenet_HttpStatusLine&      statusLine,
//                             const baenet_HttpResponseHeader&  header);
//
// [4]      int generateHeader(bsl::streambuf                   *destination,
//                             const baenet_HttpStatusLine&      statusLine,
//                             const baenet_HttpResponseHeader&  header);
//
// [5,7-10] int generateBody(bcema_Blob                         *result,
//                           const bcema_Blob&                   data);
//
// [5]      int generateBody(bcema_Blob                         *result,
//                           const void                         *data,
//                           int                                 numBytes);
//
// [5]      int generateBody(bsl::streambuf                     *destination,
//                           const bcema_Blob&                   data);
//
// [5]      int generateBody(bsl::streambuf                     *destination,
//                           const void                         *data,
//                           int                                 numBytes);
//
// [6]      int generateBody(bcema_Blob                         *result,
//                           const bcema_Blob&                   data,
//                           baenet_HttpTransferEncoding::Value  encoding,
//                           bool                                isFinalFlag);
//
// [6-10]   int generateBody(bcema_Blob                         *result,
//                           const void                         *data,
//                           int                                 length,
//                           baenet_HttpTransferEncoding::Value  encoding,
//                           bool                                isFinalFlag);
//
// [6]      int generateBody(bsl::streambuf                     *destination,
//                           const bcema_Blob&                   data,
//                           baenet_HttpTransferEncoding::Value  encoding,
//                           bool                                isFinalFlag);
//
// [6]      int generateBody(bsl::streambuf                     *destination,
//                           const void                         *data,
//                           int                                 length,
//                           baenet_HttpTransferEncoding::Value  encoding,
//                           bool                                isFinalFlag);
//-----------------------------------------------------------------------------
// [ 1] CLASS METHOD 'generateHeader' to blobs from request headers
// [ 2] CLASS METHOD 'generateHeader' to streambufs from request headers
// [ 3] CLASS METHOD 'generateHeader' to blobs from response headers
// [ 4] CLASS METHOD 'generateHeader' to streambufs from response headers
// [ 5] CLASS METHOD 'generateBody' with the identify transfer encoding
// [ 6] CLASS METHOD 'generateBody' with the chunked transfer encoding
// [ 7] CONCERN: Generating requests with the identity transfer encoding
// [ 8] CONCERN: Generating responses with the identity transfer encoding
// [ 9] CONCERN: Generating requests with the chunked transfer encoding
// [10] CONCERN: Generating responses with the chunked transfer encoding
// [11] USAGE EXAMPLE
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

const int  HTTP_MAJOR_VERSION = 1;
const int  HTTP_MINOR_VERSION = 1;
const char REQUEST_URI[]      = "/test";
const int  CONTENT_LENGTH     = 100;
const int  BUFFER_SIZE        = 32;

struct {
    int  d_line;
    int  d_dataSet;
    bool d_completeHeaderFlag;
    int  d_contentLength;
} DATA[] = {
    { L_, 0, false,              0 },
    { L_, 1,  true,              0 },
    { L_, 2, false, CONTENT_LENGTH },
    { L_, 3,  true, CONTENT_LENGTH }
};

enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

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

char generateByte(int position, int dataSet)
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

    dataSet = dataSet % NUM_DATA;
    return DATA[dataSet].source[position % DATA[dataSet].length];
}

void generateData(bcema_Blob *result, int offset, int size, int dataSet)
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
            buffer.data()[j] = generateByte(offset + k, dataSet);
            ++k;
        }
    }
}

void generateData(bsl::streambuf *destination,
                  int             offset,
                  int             size,
                  int             dataSet)
{
    for (int i = 0; i < size; ++i) {
        destination->sputc(generateByte(offset + i, dataSet));
    }
}

void defineHeader(baenet_HttpRequestLine   *requestLine,
                  baenet_HttpRequestHeader *header,
                  int                       dataSet)
{
    typedef baenet_HttpRequestMethod RM;

    bdet_Datetime nowAsDatetime = bdetu_SystemTime::nowAsDatetimeGMT();

    // The HTML Parser does not parse the milliseconds field.
    nowAsDatetime.setMillisecond(0);

    bdet_DatetimeTz now = bdet_DatetimeTz(nowAsDatetime, 0);

    // Define the request line.

    requestLine->majorVersion() = HTTP_MAJOR_VERSION;
    requestLine->minorVersion() = HTTP_MINOR_VERSION;

    requestLine->method()       = RM::BAENET_POST;
    requestLine->requestUri()   = REQUEST_URI;

    if (!DATA[dataSet].d_completeHeaderFlag) {
        // Define the basic header fields.

        header->basicFields().contentLength() = DATA[dataSet].d_contentLength;

        // Define the request header fields.

        baenet_HttpHost& host = header->requestFields().host().makeValue();
        host.name() = "localhost";
        host.port() = 80;

    }
    else {
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

        header->basicFields().contentLength() = DATA[dataSet].d_contentLength;

        header->basicFields().contentLocation() = "resource.html";

        header->basicFields().contentMd5() =
                                     "e72c504dc16c8fcd2fe8c74bb492affa";

        header->basicFields().contentRange() = "bytes 0-499/1234";

        baenet_HttpContentType& contentType =
                                       header->basicFields().contentType()
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

        header->requestFields().userAgent() =
                                    "CERN-LineMode/2.15 libwww/2.17b3";

        // Define extension header fields.

        header->extendedFields().addFieldValue("X-BBG-ALPHA", "1");
        header->extendedFields().addFieldValue("X-BBG-BETA",  "2");
        header->extendedFields().addFieldValue("X-BBG-GAMMA", "3");
    }
}

void defineHeader(baenet_HttpStatusLine     *statusLine,
                  baenet_HttpResponseHeader *header,
                  int                        dataSet)
{
    typedef baenet_HttpRequestMethod RM;

    bdet_Datetime nowAsDatetime = bdetu_SystemTime::nowAsDatetimeGMT();

    // The HTML Parser does not parse the milliseconds field.
    nowAsDatetime.setMillisecond(0);

    bdet_DatetimeTz now = bdet_DatetimeTz(nowAsDatetime, 0);

    // Define the status line.

    statusLine->majorVersion() = HTTP_MAJOR_VERSION;
    statusLine->minorVersion() = HTTP_MINOR_VERSION;

    statusLine->statusCode()   = baenet_HttpStatusCode::BAENET_OK;
    statusLine->reasonPhrase() = "OK";

    if (DATA[dataSet].d_completeHeaderFlag) {
        // Define the basic header fields.

        header->basicFields().contentLength() = DATA[dataSet].d_contentLength;
    }
    else {

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

        header->basicFields().contentLength() = DATA[dataSet].d_contentLength;

        header->basicFields().contentLocation() = "resource.html";

        header->basicFields().contentMd5() =
                                     "e72c504dc16c8fcd2fe8c74bb492affa";

        header->basicFields().contentRange() = "bytes 0-499/1234";

        baenet_HttpContentType& contentType =
                                       header->basicFields().contentType()
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

        header->responseFields().retryAfter() =
                                      "Fri, 31 Dec 1999 23:59:59 GMT";

        header->responseFields().server() = "CERN/3.0 libwww/2.17";

        header->responseFields().vary() = "*";

        header->responseFields().wwwAuthenticate() = "challenge";

        // Define extension header fields.

        header->extendedFields().addFieldValue("X-BBG-ALPHA", "1");
        header->extendedFields().addFieldValue("X-BBG-BETA",  "2");
        header->extendedFields().addFieldValue("X-BBG-GAMMA", "3");
    }
}

void defineBody(bsl::streambuf *destination, int dataSet)
{
    generateData(destination, 0, DATA[dataSet].d_contentLength, dataSet);
}

void defineBody(bcema_Blob *body, int dataSet)
{
    generateData(body, 0, DATA[dataSet].d_contentLength, dataSet);
}

int compare(const bcema_Blob& lhs, const bcema_Blob& rhs)
{
    return bcema_BlobUtil::compare(lhs, rhs);
}

int compare(const bcema_Blob& lhs, const bsl::string& rhs)
{
    bsl::stringbuf sb;
    {
        bdex_ByteOutStreamFormatter bosf(&sb);
        bcema_BlobUtil::write(bosf, lhs);
    }

    return sb.str().compare(rhs);
}

int compare(const bsl::string& lhs, const bcema_Blob& rhs)
{
    bsl::stringbuf sb;
    {
        bdex_ByteOutStreamFormatter bosf(&sb);
        bcema_BlobUtil::write(bosf, rhs);
    }

    return lhs.compare(sb.str());
}

int compare(bsl::string& lhs, const bsl::string& rhs)
{
    return lhs.compare(rhs);
}

int parseChunkedBody(bcema_Blob *result, const bcema_Blob& body)
{
    int rc;

    result->removeAll();

    bcesb_InBlobStreamBuf isb(&body);

    int chunkSize = 0;

    do {
        int numBytesConsumed = 0;
        rc = baenet_HttpParserUtil::parseChunkHeader(&chunkSize,
                                                     &numBytesConsumed,
                                                     &isb);
        if (0 != rc) {
            return rc;
        }

        char chunk[CONTENT_LENGTH];
        ASSERT(sizeof chunk >= chunkSize);

        if (chunkSize > 0) {
            int numBytesRead = isb.sgetn(chunk, chunkSize);
            ASSERT(numBytesRead == chunkSize);

            bcema_BlobUtil::append(result, chunk, 0, numBytesRead);
        }
    }
    while (chunkSize != 0);

    return 0;
}

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
  void handleRequest(bsl::streambuf     *connectionToClient,
                     const bsl::string&  filename);
//..
// We also need a 'handleError' function that, instead of delivering the
// contents of a file, delivers some sort of error error to the client.
//..
  void handleError(bsl::streambuf     *connectionToClient,
                   const bsl::string&  body);
//..
// Now let's implement the 'handleRequest' function to read the file from disk
// and send it to the client.  Since the requested file may be larger than
// what can fit in the process's memory space, we'll read the file from disk
// in small chunks, transmitting each chunk to the client using the chunked
// transfer coding.  First, attempt to open a stream to the file, and, if
// unsuccessful, return an error to the client.
//..
void handleRequest(bsl::streambuf     *connectionToClient,
                   const bsl::string&  filename)
{
    bsl::fstream file(filename.c_str(), bsl::ios_base::in);

    if (!file.is_open()) {
        bsl::stringstream ss;
        ss << "The file '" << filename << "' was not found.";

        handleError(connectionToClient, ss.str());
        return;
    }
//..
// Next, define an HTTP header that indicates a successful response delivered
// using the chunked transfer coding.
//..
    baenet_HttpResponseHeader header;
    baenet_HttpStatusLine     statusLine;

    statusLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;
    statusLine.reasonPhrase() = "OK";

    header.basicFields().transferEncoding().push_back(
                              baenet_HttpTransferEncoding::BAENET_CHUNKED);

    int retCode = baenet_HttpGeneratorUtil::generateHeader(
                                                     connectionToClient,
                                                     statusLine,
                                                     header);
    ASSERT(0 == retCode);
//..
// Now, read the file into 1K buffers, transmitting each buffer as a "chunk"
// in the HTTP data stream.  Transmit the final "chunk" we no more data can
// be read from the file.
//..
    bool isFinalFlag;
    do {
        bdex_ByteInStreamFormatter bisf(file.rdbuf());
        bcema_Blob                 data;

        bcema_BlobUtil::read(bisf, &data, 1024);

        isFinalFlag =
                  (file.peek() == bsl::fstream::traits_type::eof());

        retCode = baenet_HttpGeneratorUtil::generateBody(
                               connectionToClient,
                               data,
                               baenet_HttpTransferEncoding::BAENET_CHUNKED,
                               isFinalFlag);
        ASSERT(0 == retCode);
    }
    while (!isFinalFlag);
}
//..
// Finally, let's implement the 'handleError' function to synchronously
// construct an HTTP entity that describes the error loading the file from
// disk.
//..
void handleError(bsl::streambuf     *connectionToClient,
                 const bsl::string&  body)
{
    int retCode;

    baenet_HttpResponseHeader header;
    baenet_HttpStatusLine     statusLine;

    statusLine.statusCode()   = baenet_HttpStatusCode::BAENET_NOT_FOUND;
    statusLine.reasonPhrase() = "File not found";

    header.basicFields().contentLength() = body.size();

    retCode = baenet_HttpGeneratorUtil::generateHeader(connectionToClient,
                                                         statusLine,
                                                         header);
    ASSERT(0 == retCode);

    retCode = baenet_HttpGeneratorUtil::generateBody(connectionToClient,
                                                     body.c_str(),
                                                     body.size());
    ASSERT(0 == retCode);
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
    case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //  1 The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        //  1 Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        // See 'handleRequest' and 'handleError' under the section 'GLOBAL
        // HELPER FUNCTIONS'.

    } break;
    case 10: {
        // --------------------------------------------------------------------
        // CONCERN: Generating responses with chunked transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP response value from the current input data set.
        //      Explicitly state the header indicates the content is
        //      chunk-encoded.  Ensure the response does not have the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP response into a blob
        //      using chunked transfer encoding.
        //
        //    3 Add the contents of the blob to the message parsing mechanism,
        //      and ensure the parsed response has the same value as the
        //      originally defined response.
        //
        // Testing:
        //   static int generateHeader(
        //                 bcema_Blob                         *result,
        //                 const baenet_HttpStatusLine&       requestLine,
        //                 const baenet_HttpResponseHeader&   header);
        //
        //   static int generateBody(bcema_Blob               *result,
        //                 const void                         *data,
        //                 int                                 length,
        //                 baenet_HttpTransferEncoding::Value  encoding,
        //                 bool                                isFinalFlag);
        // --------------------------------------------------------------------
        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpStatusLine         statusLine1;
            baenet_HttpStatusLine         statusLine2;
            baenet_HttpResponseHeader     header1;
            baenet_HttpResponseHeader     header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&statusLine1, &header1, dataSet);
            defineBody(&body1, dataSet);

            header1.basicFields().contentLength() = 0;

            header1.basicFields().transferEncoding().clear();
            header1.basicFields().transferEncoding().push_back(
                              baenet_HttpTransferEncoding::BAENET_CHUNKED);

            ASSERT(statusLine1 != statusLine2);
            ASSERT(header1     != header2);
            ASSERT((0          == DATA[dataSet].d_contentLength) ==
                   (0          == compare(body1, body2)));

            bool isCompleteFlag = false;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);

            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

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

            if (body1.length() == 0) {
                rc = baenet_HttpGeneratorUtil::generateBody(
                        &message,
                        0,
                        0,
                        baenet_HttpTransferEncoding::BAENET_CHUNKED,
                        true);
                ASSERT(0 == rc);
            }

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == true);
            ASSERT(statusLine1    == statusLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
    } break;
    case 9: {
        // --------------------------------------------------------------------
        // CONCERN: Generating requests with chunked transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP request value from the current input data set.
        //      Explicitly state the header indicates the content is
        //      chunk-encoded.  Ensure the request does not have the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP request into a blob
        //      using chunked transfer encoding.
        //
        //    3 Add the contents of the blob to the message parsing mechanism,
        //      and ensure the parsed request has the same value as the
        //      originally defined request.
        //
        // Testing:
        //   static int generateHeader(
        //                 bcema_Blob                         *result,
        //                 const baenet_HttpRequestLine&       requestLine,
        //                 const baenet_HttpRequestHeader&     header);
        //
        //   static int generateBody(bcema_Blob               *result,
        //                 const void                         *data,
        //                 int                                 length,
        //                 baenet_HttpTransferEncoding::Value  encoding,
        //                 bool                                isFinalFlag);
        // --------------------------------------------------------------------
        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpRequestLine        requestLine1;
            baenet_HttpRequestLine        requestLine2;
            baenet_HttpRequestHeader      header1;
            baenet_HttpRequestHeader      header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&requestLine1, &header1, dataSet);
            defineBody(&body1, dataSet);

            header1.basicFields().contentLength() = 0;

            header1.basicFields().transferEncoding().clear();
            header1.basicFields().transferEncoding().push_back(
                              baenet_HttpTransferEncoding::BAENET_CHUNKED);

            ASSERT(requestLine1 != requestLine2);
            ASSERT(header1      != header2);
            ASSERT((0           == DATA[dataSet].d_contentLength) ==
                   (0           == compare(body1, body2)));

            bool isCompleteFlag = false;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);

            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

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

            if (body1.length() == 0) {
                rc = baenet_HttpGeneratorUtil::generateBody(
                        &message,
                        0,
                        0,
                        baenet_HttpTransferEncoding::BAENET_CHUNKED,
                        true);
                ASSERT(0 == rc);
            }

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == true);
            ASSERT(requestLine1   == requestLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
    } break;
    case 8: {
        // --------------------------------------------------------------------
        // CONCERN: Generating responses with the identity transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP response value from the current input data set,
        //      and ensure the response does not have the same value as the
        //      default value.
        //
        //    2 Generate an HTTP data stream from the HTTP response into a blob
        //      using the identity transfer encoding.
        //
        //    3 Add the contents of the blob to the message parsing mechanism,
        //      and ensure the parsed response has the same value as the
        //      originally defined sponse.
        //
        // Testing:
        //   static int generateHeader(
        //                      bcema_Blob                       *result,
        //                      const baenet_HttpStatusLine&      statusLine,
        //                      const baenet_HttpResponseHeader&  header);
        //
        //   static int generateBody(
        //                      bsl::streambuf                   *destination,
        //                      const bcema_Blob&                 data);
        // --------------------------------------------------------------------
        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpStatusLine         statusLine1;
            baenet_HttpStatusLine         statusLine2;
            baenet_HttpResponseHeader     header1;
            baenet_HttpResponseHeader     header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&statusLine1, &header1, dataSet);
            defineBody(&body1, dataSet);

            ASSERT(statusLine1 != statusLine2);
            ASSERT(header1     != header2);
            ASSERT((0          == DATA[dataSet].d_contentLength) ==
                   (0          == compare(body1, body2)));

            bool isCompleteFlag = false;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);

            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          statusLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = baenet_HttpGeneratorUtil::generateBody(&message, body1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == true);
            ASSERT(statusLine1    == statusLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
    } break;
    case 7: {
        // --------------------------------------------------------------------
        // CONCERN: Generating requests with the identity transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP request value from the current input data set,
        //      and ensure the request does not have the same value as the
        //      default value.
        //
        //    2 Generate an HTTP data stream from the HTTP request into a blob
        //      using the identity transfer encoding.
        //
        //    3 Add the contents of the blob to the message parsing mechanism,
        //      and ensure the parsed request has the same value as the
        //      originally defined request.
        //
        // Testing:
        //   static int generateHeader(
        //                      bcema_Blob                      *result,
        //                      const baenet_HttpRequestLine&    requestLine,
        //                      const baenet_HttpRequestHeader&  header);
        //
        //   static int generateBody(
        //                      bsl::streambuf                   *destination,
        //                      const bcema_Blob&                 data);
        // --------------------------------------------------------------------
        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpRequestLine        requestLine1;
            baenet_HttpRequestLine        requestLine2;
            baenet_HttpRequestHeader      header1;
            baenet_HttpRequestHeader      header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&requestLine1, &header1, dataSet);
            defineBody(&body1, dataSet);

            ASSERT(requestLine1 != requestLine2);
            ASSERT(header1      != header2);
            ASSERT((0           == DATA[dataSet].d_contentLength) ==
                   (0           == compare(body1, body2)));

            bool isCompleteFlag = false;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);

            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          requestLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = baenet_HttpGeneratorUtil::generateBody(&message, body1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            rc = parser.onEndData(bsl::cout);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == true);
            ASSERT(requestLine1   == requestLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
    } break;
    case 6: {
        // --------------------------------------------------------------------
        // CONCERN: 'generateBody' with the chunked transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP entity body from the current input data set,
        //      and ensure the request does not have the same value as the
        //      default value.
        //
        //    2 Generate an HTTP data stream from the HTTP entity body
        //      using the chunked transfer encoding by way of each overload
        //      being tested.
        //
        //    3 Using previously tested message parsing utilities,
        //      and ensure the parsed entity body has the same value as the
        //      originally defined entity body.
        //
        // Testing:
        //   static int generateBody(
        //                  bcema_Blob                         *result,
        //                  const bcema_Blob&                   data,
        //                  baenet_HttpTransferEncoding::Value  encoding,
        //                  bool                                isFinalFlag);
        //
        //  static int generateBody(
        //                  bcema_Blob                         *result,
        //                  const void                         *data,
        //                  int                                 length,
        //                  baenet_HttpTransferEncoding::Value  encoding,
        //                  bool                                isFinalFlag);
        //
        //  static int generateBody(
        //                  bsl::streambuf                     *destination,
        //                  const bcema_Blob&                   data,
        //                  baenet_HttpTransferEncoding::Value  encoding,
        //                  bool                                isFinalFlag);
        //
        //  static int generateBody(
        //                  bsl::streambuf                     *destination,
        //                  const void                         *data,
        //                  int                                 length,
        //                  baenet_HttpTransferEncoding::Value  encoding,
        //                  bool                                isFinalFlag);
        // --------------------------------------------------------------------

        int rc;

        typedef baenet_HttpTransferEncoding TE;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);

            {
                bcema_Blob dst(&blobBufferFactory);
                bcema_Blob src(&blobBufferFactory);

                defineBody(&src, dataSet);

                bcema_Blob body(&blobBufferFactory);
                rc = baenet_HttpGeneratorUtil::generateBody(&body,
                                                            src,
                                                            TE::BAENET_CHUNKED,
                                                            true);
                ASSERT(0 == rc);

                rc = parseChunkedBody(&dst, body);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }

            {
                bcema_Blob dst(&blobBufferFactory);
                bcema_Blob src(&blobBufferFactory);

                defineBody(&src, dataSet);

                bcema_Blob body(&blobBufferFactory);


                for (int bufferIndex = 0;
                         bufferIndex < src.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer =
                                                src.buffer(bufferIndex);

                    int isFinal = bufferIndex == src.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? src.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &body,
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (src.length() == 0) {
                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &body,
                            0,
                            0,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            true);
                    ASSERT(0 == rc);
                }

                rc = parseChunkedBody(&dst, body);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }

            {
                bcema_Blob dst(&blobBufferFactory);
                bcema_Blob src(&blobBufferFactory);

                defineBody(&src, dataSet);

                bcema_Blob body(&blobBufferFactory);
                bcesb_OutBlobStreamBuf osb(&body);

                rc = baenet_HttpGeneratorUtil::generateBody(&osb,
                                                            src,
                                                            TE::BAENET_CHUNKED,
                                                            true);
                ASSERT(0 == rc);

                osb.pubsync();

                rc = parseChunkedBody(&dst, body);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }

            {
                bcema_Blob dst(&blobBufferFactory);
                bcema_Blob src(&blobBufferFactory);

                defineBody(&src, dataSet);

                bcema_Blob body(&blobBufferFactory);
                bcesb_OutBlobStreamBuf osb(&body);

                for (int bufferIndex = 0;
                         bufferIndex < src.numDataBuffers();
                       ++bufferIndex)
                {
                    const bcema_BlobBuffer& blobBuffer =
                                                src.buffer(bufferIndex);

                    int isFinal = bufferIndex == src.numDataBuffers() - 1;

                    const char *data = blobBuffer.data();
                    int         size = isFinal
                                     ? src.lastDataBufferLength()
                                     : blobBuffer.size();

                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &osb,
                            data,
                            size,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            isFinal);
                    ASSERT(0 == rc);
                }

                if (src.length() == 0) {
                    rc = baenet_HttpGeneratorUtil::generateBody(
                            &osb,
                            0,
                            0,
                            baenet_HttpTransferEncoding::BAENET_CHUNKED,
                            true);
                    ASSERT(0 == rc);
                }

                osb.pubsync();

                rc = parseChunkedBody(&dst, body);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }
        }
    } break;
    case 5: {
        // --------------------------------------------------------------------
        // CONCERN: 'generateBody' with the identify transfer encoding
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP entity body value from the current input data
        //      set, and ensure the entity body does not have the same value
        //      as the default value.
        //
        //    2 Generate the HTTP entity body using each of the APIs that
        //      assume the identify transfer coding, and ensure that no
        //      transformation of the entity body has been performed.
        //
        // Testing:
        //   static int generateBody(bcema_Blob        *result,
        //                           const bcema_Blob&  data);
        //
        //   static int generateBody(bcema_Blob        *result,
        //                           const void        *data,
        //                           int                numBytes);
        //
        //   static int generateBody(bsl::streambuf    *destination,
        //                           const bcema_Blob&  data);
        //
        //   static int generateBody(bsl::streambuf    *destination,
        //                           const void        *data,
        //                           int                numBytes);
        // --------------------------------------------------------------------

        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);

            {
                bcema_Blob dst(&blobBufferFactory);
                bcema_Blob src(&blobBufferFactory);

                defineBody(&src, dataSet);

                rc = baenet_HttpGeneratorUtil::generateBody(&dst, src);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }

            {
                bcema_Blob  dst(&blobBufferFactory);
                bsl::string src;

                {
                    bsl::stringbuf sb;
                    defineBody(&sb, dataSet);
                    src = sb.str();
                }

                rc = baenet_HttpGeneratorUtil::generateBody(&dst,
                                                            src.c_str(),
                                                            src.size());
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst));
            }

            {
                bsl::stringbuf dst;
                bcema_Blob     src(&blobBufferFactory);

                defineBody(&src, dataSet);

                rc = baenet_HttpGeneratorUtil::generateBody(&dst, src);
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst.str()));
            }

            {
                bsl::stringbuf dst;
                bsl::string src;

                {
                    bsl::stringbuf sb;
                    defineBody(&sb, dataSet);
                    src = sb.str();
                }

                rc = baenet_HttpGeneratorUtil::generateBody(&dst,
                                                            src.c_str(),
                                                            src.size());
                ASSERT(0 == rc);
                ASSERT(0 == compare(src, dst.str()));
            }
        }
    } break;
    case 4: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'generateHeader' to streambufs from response headers
        //
        // Concerns:
        //  1 The utility under test generates a valid HTTP/1.1 response header
        //    for a variety of input data sets.
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP response header value from the current input
        //      data set, and ensure the response header value is not the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP response header
        //      value into a customization of 'bsl::streambuf'.
        //
        //    3 Add the contents of the stream buffer to the message parsing
        //      mechanism, and ensure the parsed response header has the same
        //      value as the originally defined response header.
        //
        // Testing:
        //   static int generateHeader(
        //                      bsl::streambuf                  *destination,
        //                      const baenet_HttpStatusLine&     statusLine,
        //                      const baenet_HttpResponseHeader& header);
        // --------------------------------------------------------------------

        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpStatusLine         statusLine1;
            baenet_HttpStatusLine         statusLine2;
            baenet_HttpResponseHeader     header1;
            baenet_HttpResponseHeader     header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&statusLine1, &header1, dataSet);

            ASSERT(statusLine1 != statusLine2);
            ASSERT(header1     != header2);

            bool isCompleteFlag = false;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          statusLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == DATA[dataSet].d_contentLength == 0);
            ASSERT(statusLine1    == statusLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'generateHeader' to blobs from response headers
        //
        // Concerns:
        //  1 The utility under test generates a valid HTTP/1.1 response header
        //    for a variety of input data sets.
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP response header value from the current input
        //      data set, and ensure the response header value is not the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP response header
        //      value into a blob.
        //
        //    3 Add the blob to the message parsing mechanism, and ensure the
        //      parsed response header has the same value as the originally
        //      defined response header.
        //
        // Testing:
        //   static int generateHeader(
        //                      bcema_Blob                       *result,
        //                      const baenet_HttpStatusLine&      statusLine,
        //                      const baenet_HttpResponseHeader&  header);
        // --------------------------------------------------------------------

        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpStatusLine         statusLine1;
            baenet_HttpStatusLine         statusLine2;
            baenet_HttpResponseHeader     header1;
            baenet_HttpResponseHeader     header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&statusLine1, &header1, dataSet);

            ASSERT(statusLine1 != statusLine2);
            ASSERT(header1     != header2);

            bool isCompleteFlag = false;
            ResponseAssembler responseAssembler(&bsl::cout,
                                                &statusLine2,
                                                &header2,
                                                &body2,
                                                &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &responseAssembler,
                              baenet_HttpMessageType::BAENET_RESPONSE,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          statusLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == DATA[dataSet].d_contentLength == 0);
            ASSERT(statusLine1    == statusLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'generateHeader' to streambufs from request headers
        //
        // Concerns:
        //  1 The utility under test generates a valid HTTP/1.1 request header
        //    for a variety of input data sets.
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP request header value from the current input
        //      data set, and ensure the request header value is not the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP request header
        //      value into a customization of 'bsl::streambuf'.
        //
        //    3 Add the contents of the stream buffer to the message parsing
        //      mechanism, and ensure the parsed request header has the same
        //      value as the originally defined request header.
        //
        // Testing:
        //   static int generateHeader(
        //                      bsl::streambuf                  *destination,
        //                      const baenet_HttpRequestLine&    requestLine,
        //                      const baenet_HttpRequestHeader&  header);
        // --------------------------------------------------------------------

        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpRequestLine        requestLine1;
            baenet_HttpRequestLine        requestLine2;
            baenet_HttpRequestHeader      header1;
            baenet_HttpRequestHeader      header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&requestLine1, &header1, dataSet);

            ASSERT(requestLine1 != requestLine2);
            ASSERT(header1      != header2);

            bool isCompleteFlag = false;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          requestLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == DATA[dataSet].d_contentLength == 0);
            ASSERT(requestLine1   == requestLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'generateHeader' to blobs from request headers
        //
        // Concerns:
        //  1 The utility under test generates a valid HTTP/1.1 request header
        //    for a variety of input data sets.
        //
        // Plan:
        //  1 For each data set described in the test overview:
        //
        //    1 Define an HTTP request header value from the current input
        //      data set, and ensure the request header value is not the same
        //      value as the default value.
        //
        //    2 Generate an HTTP data stream from the HTTP request header
        //      value into a blob.
        //
        //    3 Add the blob to the message parsing mechanism, and ensure the
        //      parsed request header has the same value as the originally
        //      defined request header.
        //
        // Testing:
        //   static int generateHeader(
        //                      bcema_Blob                      *result,
        //                      const baenet_HttpRequestLine&    requestLine,
        //                      const baenet_HttpRequestHeader&  header);
        // --------------------------------------------------------------------

        int rc;

        for (int dataSet = 0; dataSet < NUM_DATA; ++dataSet) {
            bcema_PooledBlobBufferFactory blobBufferFactory(BUFFER_SIZE);
            baenet_HttpRequestLine        requestLine1;
            baenet_HttpRequestLine        requestLine2;
            baenet_HttpRequestHeader      header1;
            baenet_HttpRequestHeader      header2;
            bcema_Blob                    body1(&blobBufferFactory);
            bcema_Blob                    body2(&blobBufferFactory);

            defineHeader(&requestLine1, &header1, dataSet);

            ASSERT(requestLine1 != requestLine2);
            ASSERT(header1      != header2);

            bool isCompleteFlag = false;
            RequestAssembler requestAssembler(&bsl::cout,
                                              &requestLine2,
                                              &header2,
                                              &body2,
                                              &isCompleteFlag);
            baenet_HttpMessageParser parser(
                              &requestAssembler,
                              baenet_HttpMessageType::BAENET_REQUEST,
                              &blobBufferFactory);

            bcema_Blob message(&blobBufferFactory);
            rc = baenet_HttpGeneratorUtil::generateHeader(&message,
                                                          requestLine1,
                                                          header1);
            ASSERT(0 == rc);

            rc = parser.addData(bsl::cout, message);
            ASSERT(0 == rc);

            ASSERT(isCompleteFlag == DATA[dataSet].d_contentLength == 0);
            ASSERT(requestLine1   == requestLine2);
            ASSERT(header1        == header2);
            ASSERT(0              == compare(body1, body2));
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

