// baenet_httpmessageparser.t.cpp  -*-C++-*-
#include <baenet_httpmessageparser.h>

#include <baenet_httpentityprocessor.h>
#include <baenet_httprequestheader.h>
#include <baenet_httpresponseheader.h>

#include <bcema_blobutil.h>
#include <bcema_blob.h>
#include <bcesb_blobstreambuf.h>
#include <bcema_pooledblobbufferfactory.h>
#include <bcema_sharedptr.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 7] int addData(bsl::ostream& errorStream, const bcema_Blob& data);
// [ 8] int addData(bsl::ostream& errorStream, const bcema_Blob& data);
//-----------------------------------------------------------------------------
// [ 7] exceed maximum values test.
// [ 8] Invalid Content-Length test.
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

template <typename HEADER_TYPE>
class TestEntityProcessor : public baenet_HttpEntityProcessor {
    // Test entity processor.

    // PRIVATE TYPES

    // PRIVATE DATA MEMBERS
    bsl::ostream    *d_stream_p;
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit TestEntityProcessor(bsl::ostream    *stream,
                                 bslma_Allocator *basicAllocator = 0)
    : d_stream_p(stream)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    virtual ~TestEntityProcessor()
    {
    }

    // MANIPULATORS
    virtual void onStartEntity(
                           const baenet_HttpStartLine&               startLine,
                           const bcema_SharedPtr<baenet_HttpHeader>& header)
    {
        const HEADER_TYPE *headerImp
                             = static_cast<const HEADER_TYPE*>(header.ptr());

        (*d_stream_p) << "onStartEntity(startLine = " << startLine
                      << ", header = " << *headerImp << ")"
                      << bsl::endl;
    }

    virtual void onEntityData(const bcema_Blob& data)
    {
        (*d_stream_p) << "onEntityData(data = " << bsl::endl;
        bcema_BlobUtil::hexDump(*d_stream_p, data);
        (*d_stream_p) << ", length = " << data.length() << ")" << bsl::endl;
    }

    virtual void onEndEntity()
    {
        (*d_stream_p) << "onEndEntity()" << bsl::endl;
    }

    // ACCESSORS
    virtual bcema_SharedPtr<baenet_HttpHeader> createHeader() const
    {
        // working around IBM compiler bug
        //
        // bcema_SharedPtr<HEADER_TYPE> ret;
        // ret.createInplace(d_allocator_p, d_allocator_p);
        // return ret;

        baenet_HttpHeader *header = new (*d_allocator_p) HEADER_TYPE(
                                                            d_allocator_p);

        bcema_SharedPtr<baenet_HttpHeader> ret(header, d_allocator_p);

        return ret;
    }
};

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

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
      case 8: {
        // --------------------------------------------------------------------
        // Invalid Content-Length test.
        //
        // Concerns:
        //   Content-Length is an indication of the amount of data to come.
        //   Since the entire http message is stored in memory, if the
        //   Content-Length must be limited to reasonable values.
        //
        // Plan:
        //   If send messages with a number of valid and invalid Content-Length
        //   values.  When content length comes into play (only during IDENTITY
        //   transfer encoding) then if the content length is invalid, then
        //   reject the message.
        //
        //   However, if there is a non identity transfer encoding, then all
        //   content-lengths should be ignored.  Thus, create messages with
        //   content lengths with a http transfer encoding and without.  And
        //   verify only ones with IDENTITY + invalid content lengths are
        //   rejected.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Invalid Content Length test\n"
                               << "==========================="
                               << bsl::endl;

        enum { BUFFER_SIZE = 1024 };

        bcema_PooledBlobBufferFactory factory(BUFFER_SIZE);

        // Err output.  This is to suppress output by the parser that we don't
        // care about.

        char err[BUFFER_SIZE];
        bcema_Blob errBlob(&factory);
        errBlob.appendDataBuffer(
                bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                err,
                                                bcema_SharedPtrNilDeleter(),0),
                                 sizeof(err)));
        bcesb_OutBlobStreamBuf osb(&errBlob);
        bsl::ostream           errOut(&osb);

        {
            // Create a parser
            TestEntityProcessor<baenet_HttpResponseHeader>
                                     entityProcessor(&errOut);
            baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

            // Create a valid start line.
            int contentLengthList[] = {
                0,1,2,3,4,256   // 0,1,2,3,4,256 bytes
              , 5*1024*1024-2   // 5   megs  - 2 bytes.
              , 5*1024*1024-1   // 5   megs - 1 byte
              , 5*1024*1024     // 5   megs
              , 5*1024*1024+1   // 5   megs + 1 byte
              , 5*1024*1024+2   // 5   megs + 2 bytes
              , 500*1024*1024-2 // 500 megs - 2 bytes.
              , 500*1024*1024-1 // 500 megs - 1 byte
              , 500*1024*1024   // 500 megs

              // Things past this should fail, fail, fail.
              , 500*1024*1024+1 // 500 megs + 1 byte
              , 500*1024*1024+2 // 500 megs + 2 bytes
              , 1024*1024*1024  // 1 gig
              , 2048*1024*1024  // 2 gigs
              , INT_MIN
              , INT_MAX
              , -1              // If you need a comment for this value
              , -2              // you need to get your head examined.
              , -4
              , -256
              , -5*1024*1024-2   // -5   megs  - 2 bytes.
              , -5*1024*1024-1   // -5   megs - 1 byte
              , -5*1024*1024     // -5   megs
              , -5*1024*1024+1   // -5   megs + 1 byte
              , -5*1024*1024+2   // -5   megs + 2 bytes
              , -500*1024*1024-2 // -500 megs - 2 bytes.
              , -500*1024*1024-1 // -500 megs - 1 byte
              , -500*1024*1024   // -500 megs
              , -500*1024*1024+1 // -500 megs + 1 byte
              , -500*1024*1024+2 // -500 megs + 2 bytes
              , -1024*1024*1024  // -1 gig
              , -2048*1024*1024  // -2 gigs

            };

            char VALID_START_LINE[] = {
                "HTTP/1.1 530 Special\r\n"
            };

            bcema_Blob validStartLine(&factory);
            validStartLine.appendDataBuffer(
                    bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                VALID_START_LINE,
                                                bcema_SharedPtrNilDeleter(),0),
                                     sizeof(VALID_START_LINE) - 1));

            bcema_Blob blob(&factory);

            for (int i = 0; i < sizeof(contentLengthList)/sizeof(int); ++i) {
                {
                    parser.reset();
                    blob.setLength(0);
                    ASSERT(0 == parser.addData(errOut, validStartLine));
                    {
                        bcesb_OutBlobStreamBuf osb(&blob);
                        bsl::ostream           os(&osb);
                        os << "Content-Length: " << contentLengthList[i]
                           << "\r\n\r\n ";
                    }
                    if (verbose) {
                        bsl::cout << bcema_BlobUtilHexDumper(&blob)
                                  << bsl::endl;
                    }
                    if (contentLengthList[i] > 500*1024*1024
                     || contentLengthList[i] < 0) {
                        // Content length > 500  or less than 0 megs are
                        // invalid

                        LOOP_ASSERT(contentLengthList[i],
                                         0 != parser.addData(errOut, blob));
                    }
                    else {
                        // Content length other content lengths are valid.

                        LOOP_ASSERT(contentLengthList[i],
                                         0 == parser.addData(errOut, blob));
                    }
                }
                {
                    // Now with a identify transfer encoding
                    // should work just as above.

                    parser.reset();
                    blob.setLength(0);
                    ASSERT(0 == parser.addData(errOut, validStartLine));
                    {
                        bcesb_OutBlobStreamBuf osb(&blob);
                        bsl::ostream           os(&osb);
                        os << "Transfer-Encoding:IDENTITY\r\n"
                           << "Content-Length: " << contentLengthList[i]
                           << "\r\n\r\n ";
                    }
                    if (verbose) {
                        bsl::cout << bcema_BlobUtilHexDumper(&blob)
                                  << bsl::endl;
                    }
                    if (contentLengthList[i] > 500*1024*1024
                     || contentLengthList[i] < 0) {
                        // Content length > 500  or less than 0 megs are
                        // invalid

                        LOOP_ASSERT(contentLengthList[i],
                                         0 != parser.addData(errOut, blob));
                    }
                    else {
                        // Content length other content lengths are valid.

                        LOOP_ASSERT(contentLengthList[i],
                                         0 == parser.addData(errOut, blob));
                    }
                }
            }

        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Infinite data test.
        //
        // Concerns:
        //
        // There are a number of ways to have corrupted data, based on size
        // alone.  This test will test for
        //      1) infinite start line without CR/LF
        //      2) infinite header field size
        //      3) infinite header value size
        //      4) infinite headers (each header field-value pair
        //           is small, however there are infinite number of them)
        //
        // Contingent on all these tests is that the characters being provided
        // in the stream are all valid for their respective fields.  i.e.:
        // header fields cannot contain ':'.
        //
        // Plan:
        //   For each of the above situation, create a blob containing
        //   only valid characters.  Then append this blob to the parser
        //   over and over and see if we can exceed the max values.
        //
        //   1. append only valid characters without CR/LF to the stream until
        //      the MAX_START_LINE_LENGTH is exceeded.
        //
        //   2. First input a valid start line, then append only valid
        //      characters without CR/LF or : to the stream until the
        //      MAX_HEADER_SIZE is exceeded.
        //
        //   3. First input a valid start line and header, including the ':',
        //      then append only valid characters without CR/LF to the
        //      stream until the MAX_HEADER_SIZE is exceeded.
        //
        //   4. First input a valid start line.  Then append a valid header
        //      name, the ':', and header value, repeat until the
        //      MAX_HEADER_SIZE is exceeded.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "INFINITE DATA TEST\n"
                               << "==================" << bsl::endl;

        enum {
            BUFFER_SIZE = 8192 + 256 // Larger than blob + buffer
          , MAX_HEADER_LENGTH = 8192 // max length without CR/LF before
                                     // copied from .cpp
        };

        // List of lengths to try to add

        int lengthList[] = {
            1, 2, 3, 4, 5, 32, 256
          , MAX_HEADER_LENGTH - 256
          , MAX_HEADER_LENGTH - 128
          , MAX_HEADER_LENGTH - 64
          , MAX_HEADER_LENGTH - 32
          , MAX_HEADER_LENGTH - 16
          , MAX_HEADER_LENGTH - 8
          , MAX_HEADER_LENGTH - 4
          , MAX_HEADER_LENGTH - 2
          , MAX_HEADER_LENGTH - 1
          , MAX_HEADER_LENGTH
          , MAX_HEADER_LENGTH + 1
          , MAX_HEADER_LENGTH + 2
          , MAX_HEADER_LENGTH + 4
          , MAX_HEADER_LENGTH + 16
        };

        bcema_PooledBlobBufferFactory factory(BUFFER_SIZE);

        // Err output.  This is to suppress output by the parser that we don't
        // care about.

        char err[BUFFER_SIZE];
        bcema_Blob errBlob(&factory);
        errBlob.appendDataBuffer(
                bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                err,
                                                bcema_SharedPtrNilDeleter(),0),
                                 sizeof(err)));
        bcesb_OutBlobStreamBuf osb(&errBlob);
        bsl::ostream           os(&osb);

        // Create a buffer with bogus, however valid, data.

        char data[BUFFER_SIZE];
        bsl::memset(data, 'A', BUFFER_SIZE);
        bcema_Blob blob(&factory);
        blob.appendDataBuffer(
                bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                data,
                                                bcema_SharedPtrNilDeleter(),0),
                                 sizeof(data)));

        // offset Data.
        char offsetData[MAX_HEADER_LENGTH];
        bsl::memset(offsetData, 'A', MAX_HEADER_LENGTH);
        bcema_Blob offsetDataBlob(&factory);
        offsetDataBlob.appendDataBuffer(
                bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                offsetData,
                                                bcema_SharedPtrNilDeleter(),0),
                                 sizeof(offsetData)));

        {
            // Infinite start line test
            if (verbose) {
                bsl::cout << "Infinite start line test\n"
                          << "------------------------" << bsl::endl;
            }

            TestEntityProcessor<baenet_HttpResponseHeader>
                                     entityProcessor(&os);
            baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

            enum { MAX_START_LINE_LENGTH = 1024 }; // max length without CR/LF
                                                   // before copied from .cpp

            int length = 1;
            int ret = 0;
            while (length < MAX_START_LINE_LENGTH + 20) {
                blob.setLength(length++);
                parser.reset();

                int bytesConsumed =0;
                while (1) {
                    int ret = parser.addData(os, blob);
                    bytesConsumed +=blob.length();
                    if (veryVerbose) {
                        bsl::cout << " Add len: " << length
                                  << " bytes consumed: " << bytesConsumed
                                  << bsl::endl;
                    }

                    if (bytesConsumed <= MAX_START_LINE_LENGTH ) {
                        LOOP_ASSERT(ret, 0 == ret); // Reached eof, success
                    }
                    else {
                        // no newlines, failure
                        LOOP2_ASSERT(bytesConsumed, ret, 0 != ret);
                        break;
                    }
                }
            }
        }
        {
            // Infinite Header field
            if (verbose) {
                bsl::cout << "Infinite Header Name Test\n"
                          << "-------------------------" << bsl::endl;
            }

            // Create a parser
            TestEntityProcessor<baenet_HttpResponseHeader>
                                     entityProcessor(&os);
            baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

            // Create a valid start line.
            char VALID_START_LINE[] = {
                "HTTP/1.1 530 Special\r\n",
            };
            bcema_Blob validStartLine(&factory);
            validStartLine.appendDataBuffer(
                    bcema_BlobBuffer(bcema_SharedPtr<char>(
                                                VALID_START_LINE,
                                                bcema_SharedPtrNilDeleter(),0),
                                     sizeof(VALID_START_LINE)-1));

            for (int lengthIndex = 0;
                                  lengthIndex < sizeof(lengthList)/sizeof(int);
                                                               ++lengthIndex) {
                int length = lengthList[lengthIndex] ;
                blob.setLength(length);

                parser.reset();

                // Add the start line
                ASSERT(0 == parser.addData(os, validStartLine));
                int bytesConsumed = validStartLine.length();

                if (validStartLine.length() + length + 20 < MAX_HEADER_LENGTH){
                    // Skip all the middle boring stuff.

                    offsetDataBlob.setLength(
                                    MAX_HEADER_LENGTH - validStartLine.length()
                                    - length - 20);
                    bytesConsumed += offsetDataBlob.length();
                    ASSERT(0 == parser.addData(os, offsetDataBlob));
                }

                while (1) {
                    int ret = parser.addData(os, blob);
                    bytesConsumed+=blob.length();
                    if (veryVerbose) {
                        bsl::cout << " Add len: " << length
                                  << " bytes consumed: " << bytesConsumed
                                  << bsl::endl;
                    }

                    if (bytesConsumed <= MAX_HEADER_LENGTH) {
                        LOOP_ASSERT(ret, 0 == ret); // Reached eof, success
                    }
                    else {
                        LOOP_ASSERT(ret, 0 != ret); // no newlines, failure
                        break;
                    }
                }
            }
        }
        {
            // Infinite Header Value field
            if (verbose) {
                bsl::cout << "Infinite Header Value Test\n"
                          << "--------------------------" << bsl::endl;
            }

            // Create a parser
            TestEntityProcessor<baenet_HttpResponseHeader>
                                     entityProcessor(&os);
            baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

            // Create a valid start line and header.
            char VALID_START_LINE_AND_HEADER_NAME[] = {
                "HTTP/1.1 530 Special\r\nAuthorization:",
            };
            bcema_Blob validStartLineAndHeaderName(&factory);
            validStartLineAndHeaderName.appendDataBuffer(
                    bcema_BlobBuffer(bcema_SharedPtr<char>(
                                              VALID_START_LINE_AND_HEADER_NAME,
                                              bcema_SharedPtrNilDeleter(),0),
                                  sizeof(VALID_START_LINE_AND_HEADER_NAME)-1));

            for (int lengthIndex = 0;
                                  lengthIndex < sizeof(lengthList)/sizeof(int);
                                                               ++lengthIndex) {
                int length = lengthList[lengthIndex] ;
                blob.setLength(length);

                parser.reset();
                ASSERT(0 == parser.addData(os, validStartLineAndHeaderName));

                int bytesConsumed = validStartLineAndHeaderName.length();
                if (validStartLineAndHeaderName.length() + length + 20
                                                         < MAX_HEADER_LENGTH) {
                    // Skip all the middle boring stuff.
                    offsetDataBlob.setLength(MAX_HEADER_LENGTH
                                         - validStartLineAndHeaderName.length()
                                         - length - 20);
                    bytesConsumed += offsetDataBlob.length();
                    ASSERT(0 == parser.addData(os, offsetDataBlob));
                }

                while (1) {
                    int ret = parser.addData(os, blob);
                    bytesConsumed+=blob.length();

                    if (veryVerbose) {
                        bsl::cout << " Add len: " << length
                                  << " bytes consumed: " << bytesConsumed
                                  << bsl::endl;
                    }

                    if (bytesConsumed <= MAX_HEADER_LENGTH) {
                        LOOP_ASSERT(ret, 0 == ret);
                    }
                    else {
                        LOOP_ASSERT(ret, 0 != ret);
                        // no newlines, failure
                        break;
                    }
                }
            }
        }
        {
            // Infinite Header field
            if (verbose) {
                bsl::cout << "Infinite Header Test\n"
                          << "--------------------" << bsl::endl;
            }

            // Create a parser
            TestEntityProcessor<baenet_HttpResponseHeader>
                                     entityProcessor(&os);
            baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

            // Create a valid start line and header.
            char VALID_START_LINE[] = {
                "HTTP/1.1 530 Special\r\n"
            };
            bcema_Blob validStartLine(&factory);
            validStartLine.appendDataBuffer(
                    bcema_BlobBuffer(bcema_SharedPtr<char>(
                                              VALID_START_LINE,
                                              bcema_SharedPtrNilDeleter(),0),
                                    sizeof(VALID_START_LINE)-1));

            // Create a valid start line and header.
            char HEADER_LINE[] = {
                "HeaderName:\r\n"
            };
            bcema_Blob validHeaderLine(&factory);
            validHeaderLine.appendDataBuffer(
                    bcema_BlobBuffer(bcema_SharedPtr<char>(
                                              HEADER_LINE,
                                              bcema_SharedPtrNilDeleter(),0),
                                    sizeof(HEADER_LINE)-1));

            for (int lengthIndex = 0;
                                  lengthIndex < sizeof(lengthList)/sizeof(int);
                                                               ++lengthIndex) {
                int length = lengthList[lengthIndex] ;
                blob.setLength(length);
                parser.reset();
                parser.addData(os, validStartLine);
                int bytesConsumed = validStartLine.length();

                if (validStartLine.length() + length + 256< MAX_HEADER_LENGTH){
                    // Skip all the middle boring stuff.
                    offsetDataBlob.setLength(
                                    MAX_HEADER_LENGTH - validStartLine.length()
                                    - length - 256);
                    bytesConsumed += offsetDataBlob.length();
                    ASSERT(0 == parser.addData(os, offsetDataBlob));
                }

                if (veryVerbose) {
                    bsl::cout <<"  addBlob Length: " << length
                              <<"\n    Start Length  :" << bytesConsumed
                              << bsl::endl;
                }

                while (1) {
                    int ret = parser.addData(os, validHeaderLine);
                    bytesConsumed += validHeaderLine.length();

                    if (veryVerbose) {
                        bsl::cout << "    bytesConsumed:"
                                  << bytesConsumed << bsl::endl;
                    }

                    if (bytesConsumed <= MAX_HEADER_LENGTH) {
                        LOOP_ASSERT(ret, 0 == ret);
                    }
                    else {
                        LOOP_ASSERT(ret, 0 != ret);
                        // no newlines, failure
                        break;
                    }

                    ret = parser.addData(os, blob);
                    bytesConsumed+=blob.length();

                    if (bytesConsumed<= MAX_HEADER_LENGTH) {
                        LOOP_ASSERT(ret, 0 == ret);
                    }
                    else {
                        LOOP_ASSERT(ret, 0 != ret);
                        // no newlines, failure
                        break;
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // RESPONSE BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RESPONSE BREATHING TEST" << bsl::endl
                               << "=======================" << bsl::endl;

        const char *DATA[] = { // chunked - with trailer contents.
            "HTTP/1.1 530 Special\r\n",
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n",
            "Content-Type: text/plain\r\n",
            "\r\n",
            "I am special",
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_PooledBlobBufferFactory factory(5);

        TestEntityProcessor<baenet_HttpResponseHeader>
                                 entityProcessor(&bsl::cout);
        baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            LOOP_ASSERT(DATA[i], 0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RESPONSE BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RESPONSE BREATHING TEST" << bsl::endl
                               << "=======================" << bsl::endl;

        const char *DATA[] = { // chunked - with trailer contents.
            "HTTP/1.1 200 OK\r\n",
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n",
            "Content-Type: text/plain\r\n",
            "Transfer-Encoding: chunked\r\n",
            "\r\n",
            "1a; ignore-stuff-here\r\n",
            "abcdefghijklmnopqrstuvwxyz\r\n",
            "10\r\n",
            "1234567890abcdef\r\n",
            "0\r\n",
            "Expires: Thu, 01 Dec 2002 16:00:00 GMT\r\n",
            "Last-Modified: Fri, 31 Dec 1999 12:59:59 GMT\r\n",
            "\r\n",
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_PooledBlobBufferFactory factory(5);

        TestEntityProcessor<baenet_HttpResponseHeader>
                                 entityProcessor(&bsl::cout);
        baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            LOOP_ASSERT(DATA[i], 0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RESPONSE BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RESPONSE BREATHING TEST" << bsl::endl
                               << "=======================" << bsl::endl;

        const char *DATA[] = { // chunked - no trailer contents.
            "HTTP/1.1 200 OK\r\n",
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n",
            "Content-Type: text/plain\r\n",
            "Transfer-Encoding: chunked\r\n",
            "\r\n",
            "1a\r\n",
            "abcdefghijkl",
            "mnopqrstuvwxyz\r\n",
            "10\r\n",
            "1234567890abcdef\r\n",
            "0; ignore-stuff-here\r\n",
            "\r\n",
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_PooledBlobBufferFactory factory(5);

        TestEntityProcessor<baenet_HttpResponseHeader>
                                 entityProcessor(&bsl::cout);
        baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            LOOP_ASSERT(DATA[i], 0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RESPONSE BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RESPONSE BREATHING TEST" << bsl::endl
                               << "=======================" << bsl::endl;

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
        baenet_HttpMessageParser parser(
                                      &entityProcessor,
                                      baenet_HttpMessageType::BAENET_RESPONSE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            LOOP_ASSERT(i, 0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // REQUEST BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "REQUEST BREATHING TEST" << bsl::endl
                               << "======================" << bsl::endl;

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
        baenet_HttpMessageParser parser(
                                       &entityProcessor,
                                       baenet_HttpMessageType::BAENET_REQUEST);

        for (int i = 0; i < NUM_DATA; ++i) {
            bcema_Blob blob(&factory);

            {
                bcesb_OutBlobStreamBuf osb(&blob);
                bsl::ostream           os(&osb);

                os << DATA[i];
            }

            int ret = parser.addData(bsl::cerr, blob);
            LOOP_ASSERT(i, 0 == ret);
        }
        int ret = parser.onEndData(bsl::cerr);
        ASSERT(0 == ret);

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
