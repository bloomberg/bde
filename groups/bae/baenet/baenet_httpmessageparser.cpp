// baenet_httpmessageparser.cpp  -*-C++-*-
#include <baenet_httpmessageparser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpmessageparser_cpp,"$Id$ $CSID$")

#include <baenet_httpbasicheaderfields.h>
#include <baenet_httpentityprocessor.h>
#include <baenet_httpparserutil.h>
#include <baenet_httprequestline.h>
#include <baenet_httpstatusline.h>

#include <baenet_httprequestheader.h>  // for testing only
#include <baenet_httpresponseheader.h> // for testing only

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcesb_blobstreambuf.h>

#include <bdema_sequentialallocator.h>
#include <bdeu_string.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace {

// CONSTANTS

enum {
    STATE_INITIAL,
    STATE_GOT_START_LINE,
    STATE_GOT_HEADER,
    STATE_PROCESSING_BODY,
    STATE_PROCESSING_CHUNKS,
    STATE_PROCESSING_TRAILER,
    STATE_FINISHED,
    STATE_ERROR
};

enum {
    UNKNOWN_REMAINING_LENGTH = -1
};

enum {
    REACHED_EOF        = 1
  , END_OF_HEADER      = 2
  , MAX_BYTES_EXCEEDED = 3
};

enum {
    MAX_HEADER_LENGTH = 8192
  , MAX_CONTENT_LENGTH = 500 * 1024 * 1024
};

// HELPER FUNCTIONS

void trim(bdeut_StringRef *str)
{
    const char *begin = str->begin();
    const char *end   = str->end();
    bdeu_String::skipLeadingTrailing(&begin, &end);
    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&begin, end);
    str->assign(begin, end);
}

bool canHaveMessageBody(const baenet_HttpStartLine& startLine)
    // Return true if, based on the specified 'startLine', whether the message
    // can contain a message body or false otherwise.  This is per RFC2616
    // section 4.4.
{
    if (baenet_HttpStartLine::SELECTION_ID_STATUS_LINE
                                                  != startLine.selectionId()) {
        return false;
    }

    int statusCode = static_cast<int>(startLine.statusLine().statusCode());

    if ((100 <= statusCode && statusCode <= 199)
     || 204 == statusCode || 304 == statusCode) {
        return false;
    }

    return true;
}

}  // close unnamed namespace

                       // ------------------------------
                       // class baenet_HttpMessageParser
                       // ------------------------------

// PRIVATE MANIPULATORS

int baenet_HttpMessageParser::parseStartLine(
                                        bsl::ostream&              errorStream,
                                        bdema_SequentialAllocator *alloc)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };
    enum { MAX_START_LINE_LENGTH = 1024 }; // max length without CR/LF before
                                           // giving up and declaring input
                                           // bogus.

    bdeut_StringRef startLineStr;
    int             numBytesConsumed = 0;

    {
        bcesb_InBlobStreamBuf isb(&d_data);

        while (startLineStr.isEmpty()) {
            int result = baenet_HttpParserUtil::parseLine(
                                                        &startLineStr,
                                                        &numBytesConsumed,
                                                        alloc, &isb,
                                                        MAX_START_LINE_LENGTH);

            if (baenet_HttpParserUtil::BAENET_REACHED_EOF == result) {
                return REACHED_EOF;
            }

            if (baenet_HttpParserUtil::BAENET_MAX_BYTES_EXCEEDED == result) {
                return MAX_BYTES_EXCEEDED;
            }

            trim(&startLineStr);
        }
    }

    bcema_BlobUtil::erase(&d_data, 0, numBytesConsumed);
    d_remainingHeaderLength-=numBytesConsumed;

    if (baenet_HttpMessageType::BAENET_REQUEST == d_messageType) {
        d_startLine.makeRequestLine();

        if (0 != baenet_HttpParserUtil::parseStartLine(
                                                    &d_startLine.requestLine(),
                                                    startLineStr)) {
            errorStream << "Failed to parse request line: '" << startLineStr
                        << "'." << bsl::endl;
            return BAENET_FAILURE;
        }
    }
    else {
        d_startLine.makeStatusLine();

        if (0 != baenet_HttpParserUtil::parseStartLine(
                                                     &d_startLine.statusLine(),
                                                     startLineStr)) {
            errorStream << "Failed to parse status line: '" << startLineStr
                        << "'." << bsl::endl;
            return BAENET_FAILURE;
        }
    }

    return BAENET_SUCCESS;
}

int baenet_HttpMessageParser::parseHeaderField(
                                        bsl::ostream&              errorStream,
                                        bdema_SequentialAllocator *alloc)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    bdeut_StringRef fieldName, fieldValue;

    bcesb_InBlobStreamBuf isb(&d_data);
    int                   numBytesConsumed = 0;

    int result = baenet_HttpParserUtil::parseFieldName(
                                                      &fieldName,
                                                      &numBytesConsumed,
                                                      alloc, &isb,
                                                      d_remainingHeaderLength);

    if (baenet_HttpParserUtil::BAENET_REACHED_EOF == result) {
        return REACHED_EOF;
    }

    if (baenet_HttpParserUtil::BAENET_END_OF_HEADER == result) {
        bcema_BlobUtil::erase(&d_data, 0, numBytesConsumed);
        d_remainingHeaderLength-=numBytesConsumed;
        isb.reset(&d_data);  // this is a kludge to avoid invariant assertion
                             // failure on destruction of blob stream buffer

        return END_OF_HEADER;
    }

    if (0 != result) {
        errorStream << "Failed to parse field name!" << bsl::endl;
        return BAENET_FAILURE;
    }

    result = baenet_HttpParserUtil::parseFieldValueUnstructured(
                                                      &fieldValue,
                                                      &numBytesConsumed,
                                                      alloc, &isb,
                                                      d_remainingHeaderLength);

    if (baenet_HttpParserUtil::BAENET_REACHED_EOF == result) {
        return REACHED_EOF;
    }

    if (baenet_HttpParserUtil::BAENET_MAX_BYTES_EXCEEDED == result) {
        return MAX_BYTES_EXCEEDED;
    }

    bcema_BlobUtil::erase(&d_data, 0, numBytesConsumed);
    d_remainingHeaderLength-=numBytesConsumed;
    isb.reset(&d_data);  // this is a kludge to avoid invariant assertion
                         // failure on destruction of blob stream buffer

    trim(&fieldValue);

    if (0 != d_header_sp->addField(fieldName, fieldValue)) {
        errorStream << "Failed to add field: '" << fieldName
                    << "', value = '" << fieldValue << "'!"
                    << bsl::endl;
        return BAENET_FAILURE;
    }

    return BAENET_SUCCESS;
}

int baenet_HttpMessageParser::processHeader(bsl::ostream& errorStream)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    int numTransferEncodings
                        = d_header_sp->basicFields().transferEncoding().size();

    if (1 < numTransferEncodings) {
        errorStream << "Only one transfer encoding supported!" << bsl::endl;
        return BAENET_FAILURE;
    }

    const bdeut_NullableValue<int>& contentLength
                                  = d_header_sp->basicFields().contentLength();

    if (0 == numTransferEncodings) {
        d_transferEncoding  = baenet_HttpTransferEncoding::BAENET_IDENTITY;
    }
    else {
        d_transferEncoding  = d_header_sp->basicFields().transferEncoding()[0];
    }

    if (baenet_HttpTransferEncoding::BAENET_IDENTITY == d_transferEncoding) {
        if (!contentLength.isNull()) {
            if ((unsigned)contentLength.value() > MAX_CONTENT_LENGTH) {
                errorStream << "Content length ("
                            << contentLength.value()
                            <<") is not in valid range (0 - "
                            << MAX_CONTENT_LENGTH << ")" <<bsl::endl;
                return BAENET_FAILURE;
            }
        }

        if (canHaveMessageBody(d_startLine)) {
            d_numBytesRemaining = contentLength.isNull()
                                  ? UNKNOWN_REMAINING_LENGTH
                                  : contentLength.value();
        }
        else {
            d_numBytesRemaining = contentLength.isNull()
                                  ? 0
                                  : contentLength.value();
        }
    }
    else {
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4
        // "If the message does include a non-identity transfer-coding,
        // the Content-Length MUST be ignored."
        d_numBytesRemaining = UNKNOWN_REMAINING_LENGTH;
    }
    return BAENET_SUCCESS;
}

int baenet_HttpMessageParser::getEntityData(bsl::ostream&,
                                            bcema_Blob    *entityData)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    if (baenet_HttpTransferEncoding::BAENET_CHUNKED == d_transferEncoding) {
        if (UNKNOWN_REMAINING_LENGTH == d_numBytesRemaining) {
            return BAENET_FAILURE;
        }
    }
    else if (UNKNOWN_REMAINING_LENGTH == d_numBytesRemaining) {
        // identity encoding
        // use all data until 'baenet_HttpMessageParser::onEndData' is called
        *entityData = d_data;
        d_data.removeAll();
        return REACHED_EOF;
    }

    if (d_numBytesRemaining <= d_data.length()) {
        entityData->removeAll();
        bcema_BlobUtil::append(entityData, d_data, 0, d_numBytesRemaining);
        bcema_BlobUtil::erase(&d_data, 0, d_numBytesRemaining);
        d_numBytesRemaining = 0;
        return BAENET_SUCCESS;
    }
    else {
        *entityData = d_data;
        d_data.removeAll();
        d_numBytesRemaining -= entityData->length();
        return REACHED_EOF;
    }
}

void baenet_HttpMessageParser::prepareForNextMessage()
{
    d_remainingHeaderLength = MAX_HEADER_LENGTH; // hard-coded max header
                                                 // length
    d_startLine.reset();
    d_state = STATE_INITIAL;

    d_header_sp = d_entityProcessor_p->createHeader();

    BSLS_ASSERT(d_header_sp);
}

// CREATORS

baenet_HttpMessageParser::baenet_HttpMessageParser(
                                baenet_HttpEntityProcessor    *entityProcessor,
                                baenet_HttpMessageType::Value  messageType,
                                bslma_Allocator               *basicAllocator)
: d_data(basicAllocator)
, d_entityProcessor_p(entityProcessor)
, d_messageType(messageType)
, d_startLine(basicAllocator)
, d_state(STATE_INITIAL)
, d_remainingHeaderLength(MAX_HEADER_LENGTH)
{
    BSLS_ASSERT(d_entityProcessor_p);

    d_header_sp = d_entityProcessor_p->createHeader();

    BSLS_ASSERT(d_header_sp);
}

baenet_HttpMessageParser::~baenet_HttpMessageParser()
{
}

// MANIPULATORS

int baenet_HttpMessageParser::addData(bsl::ostream&     errorStream,
                                      const bcema_Blob& data)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    if (STATE_ERROR == d_state || STATE_FINISHED == d_state) {
        return BAENET_FAILURE;
    }

    bdema_SequentialAllocator alloc;

    bcema_BlobUtil::append(&d_data, data);

    while (true) {
      switch (d_state) {
        case STATE_INITIAL: {
            int result = parseStartLine(errorStream, &alloc);

            if (REACHED_EOF == result) {
                // we need to wait for more data
                return BAENET_SUCCESS;
            }

            if (0 != result) {
                d_state = STATE_ERROR;
                return BAENET_FAILURE;
            }

            d_state = STATE_GOT_START_LINE;
        } break;
        case STATE_GOT_START_LINE: {
            while (d_state != STATE_GOT_HEADER) {
                int result = parseHeaderField(errorStream, &alloc);

                if (REACHED_EOF == result) {
                    // we need to wait for more data
                    return BAENET_SUCCESS;
                }

                if (END_OF_HEADER == result) {
                    d_state = STATE_GOT_HEADER;
                    break;
                }

                if (0 != result) {
                    d_state = STATE_ERROR;
                    return BAENET_FAILURE;
                }
            }
        } break;
        case STATE_GOT_HEADER: {
            if (0 != processHeader(errorStream)) {
                d_state = STATE_ERROR;
                return BAENET_FAILURE;
            }

            d_entityProcessor_p->onStartEntity(d_startLine, d_header_sp);

            if (baenet_HttpTransferEncoding::BAENET_CHUNKED ==
                                                         d_transferEncoding) {
                d_state = STATE_PROCESSING_CHUNKS;
                d_numBytesRemaining = 0;
            }
            else {
                d_state = STATE_PROCESSING_BODY;
            }
        } break;
        case STATE_PROCESSING_BODY: {
            if (0 == d_numBytesRemaining) {
                d_entityProcessor_p->onEndEntity();
                prepareForNextMessage();
                break;
            }

            bcema_Blob entityData;

            int result = getEntityData(errorStream, &entityData);

            if ((0 == result || REACHED_EOF == result)
                                                 && 0 != entityData.length()) {
                d_entityProcessor_p->onEntityData(entityData);
            }

            if (REACHED_EOF == result) {
                // we need to wait for more data
                return BAENET_SUCCESS;
            }

            if (0 != result) {
                d_state = STATE_ERROR;
                return BAENET_FAILURE;
            }
        } break;
        case STATE_PROCESSING_CHUNKS: {
            if (0 == d_numBytesRemaining) {
                int result = getChunkSize(errorStream);

                if (REACHED_EOF == result) {
                    // we need to wait for more data
                    return BAENET_SUCCESS;
                }

                if (0 != result) {
                    d_state = STATE_ERROR;
                    return BAENET_FAILURE;
                }

                if (0 == d_numBytesRemaining) {
                    d_state = STATE_PROCESSING_TRAILER;
                    break;
                }
            }
            bcema_Blob entityData;

            int result = getEntityData(errorStream, &entityData);

            if ((0 == result || REACHED_EOF == result)
                                                 && 0 != entityData.length()) {
                d_entityProcessor_p->onEntityData(entityData);
            }

            if (REACHED_EOF == result) {
                // we need to wait for more data
                return BAENET_SUCCESS;
            }

            if (0 != result) {
                d_state = STATE_ERROR;
                return BAENET_FAILURE;
            }
        } break;
        case STATE_PROCESSING_TRAILER: {
           // http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html
           // See section 3.6.1 "Chunked Transfer Encoding"
            while (d_state != STATE_GOT_HEADER) {
                int result = parseHeaderField(errorStream, &alloc);

                if (REACHED_EOF == result) {
                    // we need to wait for more data
                    return BAENET_SUCCESS;
                }

                if (END_OF_HEADER == result) {
                    d_state = STATE_GOT_HEADER;
                    break;
                }

                if (0 != result) {
                    d_state = STATE_ERROR;
                    return BAENET_FAILURE;
                }
            }
            d_entityProcessor_p->onEndEntity();
            prepareForNextMessage();
        } break;
      }
    };

    return 0;
}

int baenet_HttpMessageParser::onEndData(bsl::ostream& errorStream)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    if (STATE_PROCESSING_BODY == d_state) {
        if (0 < d_numBytesRemaining) {
            errorStream << "More data expected!" << bsl::endl;
            d_state = STATE_ERROR;
            return BAENET_FAILURE;
        }

        d_entityProcessor_p->onEndEntity();
        d_state = STATE_FINISHED;
        return BAENET_SUCCESS;
    }

    if (STATE_INITIAL == d_state) {
        d_state = STATE_FINISHED;
        return BAENET_SUCCESS;
    }

    errorStream << "Reached end of data in an invalid state!" << bsl::endl;
    d_state = STATE_ERROR;
    return BAENET_FAILURE;
}

void baenet_HttpMessageParser::reset()
{
    d_data.removeAll();
    prepareForNextMessage();
}

int baenet_HttpMessageParser::getChunkSize(bsl::ostream&  errorStream)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    bcesb_InBlobStreamBuf isb(&d_data);
    int                   numBytesConsumed = 0;
    int                   result
        = baenet_HttpParserUtil::parseChunkHeader(&d_numBytesRemaining,
                                                  &numBytesConsumed,
                                                  &isb);

    if (baenet_HttpParserUtil::BAENET_REACHED_EOF == result) {
        return REACHED_EOF;
    }

    if (0 == result) {
        bcema_BlobUtil::erase(&d_data, 0, numBytesConsumed);
        isb.reset(&d_data);  // this is a kludge to avoid invariant assertion
                             // failure on destruction of blob stream buffer
        return BAENET_SUCCESS;
    }
    errorStream << "Invalid chunk header line!" << bsl::endl;
    return BAENET_FAILURE;
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
