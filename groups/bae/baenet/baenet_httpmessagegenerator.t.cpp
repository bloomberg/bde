// baenet_httpmessagegenerator.t.cpp                                  -*-C++-*-
#include <baenet_httpmessagegenerator.h>

#include <baenet_httprequestheader.h>
#include <baenet_httprequestline.h>
#include <baenet_httpstatuscode.h>
#include <baenet_httpstatusline.h>
#include <baenet_httpresponseheader.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
//                            CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                          GLOBAL HELPER FUNCTIONS
//-----------------------------------------------------------------------------

void printBlob(const bcema_Blob& data)
{
    bcema_BlobUtil::hexDump(bsl::cout, data);
}

void ignoreCallBack(const bcema_Blob& data) 
{
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
      case 3: {
        // --------------------------------------------------------------------
        //  TESTING sending startEntity handling of content-length in header
        //
        //  Concerns:
        //    -Implementation must reject generated chunked responses/requests
        //     with non-null content-length. 
        //    -Conversely, it must accept valid responses/requests with NULL
        //     content-length.
        //
        //  Plan:
        //    Send valid NULL content-length response and request and assert
        //    that success is returned.  Also send non-NULL response and
        //    request and assert that startEntity fails.
        // --------------------------------------------------------------------
        {
            if (verbose) bsl::cout << bsl::endl
                                   << "SENDING NULL CONTENT-LENGTH RESPONSE" 
                                   << bsl::endl
                                   << "====================================" 
                                   << bsl::endl;
            baenet_HttpStatusLine     startLine;
            baenet_HttpResponseHeader header;

            startLine.majorVersion() = 1;
            startLine.minorVersion() = 1;
            startLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;

            header.basicFields().transferEncoding().push_back( 
                                  baenet_HttpTransferEncoding::BAENET_CHUNKED);

            bcema_PooledBlobBufferFactory factory(5);

            baenet_HttpMessageGenerator msgGen(&factory, 
                                            bslma_Default::defaultAllocator());

            int rc = msgGen.startEntity(startLine, header, &ignoreCallBack);

            ASSERT(0 == rc);
            if (verbose) bsl::cout << bsl::endl
                                   << "SENDING NON-NULL CONTENT-LENGTH "
                                       "RESPONSE" 
                                   << bsl::endl
                                   << "==============================="
                                      "=========" 
                                   << bsl::endl;

            header.basicFields().contentLength() = 0;

            rc = msgGen.startEntity(startLine, header, &ignoreCallBack);

            ASSERT(0 != rc);
        }


        if (verbose) bsl::cout << bsl::endl
                               << "SENDING NULL CONTENT-LENGTH REQUEST" 
                               << bsl::endl
                               << "========================================" 
                               << bsl::endl;

        {
            //Dummy a request with a null and make sure it works
            baenet_HttpRequestLine     startLine;
            baenet_HttpRequestHeader header;

            startLine.majorVersion() = 1;
            startLine.minorVersion() = 1;
            //startLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;

            header.basicFields().transferEncoding().push_back( 
                                  baenet_HttpTransferEncoding::BAENET_CHUNKED);

            bcema_PooledBlobBufferFactory factory(5);

            baenet_HttpMessageGenerator msgGen(&factory, 
                                            bslma_Default::defaultAllocator());

            int rc = msgGen.startEntity(startLine, header, &ignoreCallBack);

            ASSERT(0 == rc);
            if (verbose) bsl::cout << bsl::endl
                                   << "SENDING NON-NULL CONTENT-LENGTH "
                                       "REQUEST" 
                                   << bsl::endl
                                   << "=============================="
                                      "==========" 
                                   << bsl::endl;

            header.basicFields().contentLength() = 0;

            rc = msgGen.startEntity(startLine, header, &ignoreCallBack);

            ASSERT(0 != rc);
        }
        
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RESPONSE BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RESPONSE BREATHING TEST" << bsl::endl
                               << "=======================" << bsl::endl;

        baenet_HttpStatusLine     startLine;
        baenet_HttpResponseHeader header;

        startLine.majorVersion() = 1;
        startLine.minorVersion() = 1;
        startLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;

#if 0
        const char *DATA[] = {
            "HTTP/1.1 200 all",
            " is good\r\nHost: www.bloomberg.com\r\nCon",
            "tent-Length: 10\r\n\r\nabcd",
            "efgh\r\n\r\nHTTP/5.3 404 Ooops, not found!!\r\nHost:",
            " www.somewhere.com : 281\r\nContent-Length: 1",
            "5\r\nSOAPAction: \"http://www.bloomberg.",
            "com/soapActions/abc\"\r\n\r",
            "\nqwertyuiopasd\r\n",
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_PooledBlobBufferFactory factory(5);

        TestEntityProcessor<baenet_HttpResponseHeader>
                                 entityProcessor(&bsl::cout);
        baenet_HttpMessageParser parser(&entityProcessor,
                                        baenet_HttpMessageType::RESPONSE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            ASSERT(0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);
#endif

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // REQUEST BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "REQUEST BREATHING TEST" << bsl::endl
                               << "======================" << bsl::endl;

#if 0
        const char *DATA[] = {
            "POST /public_html/index.html HT",
            "TP/1.1\r\nHost: www.bloomberg.com\r\nCon",
            "tent-Length: 10\r\n\r\nabcd",
            "efgh\r\n\r\nGET /public/other.html HTTP/3.4\r\nHost:",
            " www.somewhere.com : 281\r\nContent-Length: 1",
            "5\r\nSOAPAction: \"http://www.bloomberg.",
            "com/soapActions/abc\"\r\n\r",
            "\nqwertyuiopasd\r\n",
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_PooledBlobBufferFactory factory(5);

        TestEntityProcessor<baenet_HttpRequestHeader>
                                 entityProcessor(&bsl::cout);
        baenet_HttpMessageParser parser(&entityProcessor,
                                        baenet_HttpMessageType::REQUEST);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            ASSERT(0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);
#endif

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
