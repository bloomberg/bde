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
        //  TESTING Content-Length in 'startEntity'
        //
        //  Concerns:
        //    -'startEntity' must reject chunked responses/requests with
        //     Content-Length explicitly specified.
        //    -'startEntity' must accept chunked responses/requests with
        //     Content-Length not defined.
        //
        //  Plan:
        //    Start generating a chunked-encoded HTTP entity that does not
        //    define Content-Length and assert that 'startEntity' succeeds.
        //    Then, start generating a chunked-encoded HTTP entity that does
        //    define Content-Length and assert that 'startEntity' fails.
        //    Repeat the test for both requests and responses.
        //
        //  See Also:
        //    RFC 2616, section 4.4 ("Message Length")
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TESTING Content-Length in 'startEntity'"
                      << bsl::endl
                      << "======================================="
                      << bsl::endl;
        }

        int rc;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            bcema_PooledBlobBufferFactory factory(5);

            {
                if (veryVerbose) {
                    bsl::cout << "a) request w/ null Content-Length"
                              << bsl::endl
                              << "================================="
                              << bsl::endl;
                }

                baenet_HttpRequestLine   requestLine;
                baenet_HttpRequestHeader header;

                requsetLine.majorVersion() = 1;
                requestLine.minorVersion() = 1;
                requestLine.method()       = baenet_HttpRequestMethod::
                                             BAENET_GET;
                requestLine.requestUri()   = "/";

                header.basicFields().transferEncoding().push_back(
                                 baenet_HttpTransferEncoding::BAENET_CHUNKED);

                baenet_HttpMessageGenerator messageGenerator(&factory, &ta);

                rc = messageGenerator.startEntity(startLine,
                                                  header,
                                                  &ignoreCallBack);
                ASSERT(0 == rc);

                if (veryVerbose) {
                    bsl::cout << "a) request w/ non-null Content-Length"
                              << bsl::endl
                              << "====================================="
                              << bsl::endl;
                }

                header.basicFields().contentLength() = 0;

                rc = messageGenerator.startEntity(startLine,
                                                  header,
                                                  &ignoreCallBack);
                ASSERT(0 != rc);
            }

            {
                if (veryVerbose) {
                    bsl::cout << "a) response w/ null Content-Length"
                              << bsl::endl
                              << "=================================="
                              << bsl::endl;
                }

                baenet_HttpStatusLine     startLine;
                baenet_HttpResponseHeader header;

                startLine.majorVersion() = 1;
                startLine.minorVersion() = 1;
                startLine.statusCode()   = baenet_HttpStatusCode::BAENET_OK;
                startLine.reasonPhrase() = "OK";

                header.basicFields().transferEncoding().push_back(
                                 baenet_HttpTransferEncoding::BAENET_CHUNKED);

                baenet_HttpMessageGenerator messageGenerator(&factory, &ta);

                rc = messageGenerator.startEntity(startLine,
                                                  header,
                                                  &ignoreCallBack);
                ASSERT(0 == rc);

                if (veryVerbose) {
                    bsl::cout << "a) response w/ non-null Content-Length"
                              << bsl::endl
                              << "======================================"
                              << bsl::endl;
                }

                header.basicFields().contentLength() = 0;

                rc = messageGenerator.startEntity(startLine,
                                                  header,
                                                  &ignoreCallBack);
                ASSERT(0 != rc);
            }
        }
        ASSERT(0 <= ta.numAllocation());
        ASSERT(0 == ta.numBytesInUse());

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
