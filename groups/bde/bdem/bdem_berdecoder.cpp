// bdem_berdecoder.cpp                                                -*-C++-*-
#include <bdem_berdecoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berdecoder_cpp,"$Id$ $CSID$")

#include <bdem_berencoder.h>            // for testing only
#include <bdesb_fixedmeminstreambuf.h>  // for testing only
#include <bdesb_memoutstreambuf.h>      // for testing only

namespace BloombergLP {

                   // -----------------------------------
                   // class bdem_BerDecoder::MemOutStream
                   // -----------------------------------
// CREATORS
bdem_BerDecoder::MemOutStream::~MemOutStream()
{
}

                   // ---------------------
                   // class bdem_BerDecoder
                   // ---------------------

// CREATORS
bdem_BerDecoder::bdem_BerDecoder(
                 const bdem_BerDecoderOptions *options,
                 bslma_Allocator               *basicAllocator)
: d_options       (options)
, d_allocator     (bslma_Default::allocator(basicAllocator))
, d_logStream     (0)
, d_severity      (BDEM_BER_SUCCESS)
, d_streamBuf     (0)
, d_currentDepth  (0)
, d_topNode       (0)
{
}

bdem_BerDecoder::~bdem_BerDecoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

// MANIPULATORS
bdem_BerDecoder::ErrorSeverity
bdem_BerDecoder::logError(const char *msg)
{
    if ((int) d_severity < (int) BDEM_BER_ERROR) {
        d_severity = BDEM_BER_ERROR;
    }
    return logMsg("ERROR", msg);
}

bdem_BerDecoder::ErrorSeverity
bdem_BerDecoder::logMsg(const char *prefix, const char *msg)
{
    bsl::ostream& out = logStream();

    if (prefix) {
        out << prefix << ": ";
    }

    if (msg) {
        out << msg;
    }
    out << bsl::endl;
    return d_severity;
}

         // -------------------------------------------------------
         // private class bdem_BerDecoder_Node
         // -------------------------------------------------------

// ACCESSORS
int
bdem_BerDecoder_Node::startPos() const
{
    int ret = 0;

    bdem_BerDecoder_Node *node = d_parent;

    for (; node != 0; node = node->d_parent) {
        ret += node->d_consumedHeaderBytes
            +  node->d_consumedBodyBytes;
    }
    return ret;
}

void
bdem_BerDecoder_Node::printStack(bsl::ostream& out) const
{
    int depth = d_decoder->d_currentDepth;

    for (const bdem_BerDecoder_Node *node = this;
         node != 0; node = node->parent()) {
        node->print(out, depth--, 0, " within ");
    }
}

void
bdem_BerDecoder_Node::print(bsl::ostream&  out,
                            int            depth,
                            int            spacePerLevel,
                            const char    *prefixText) const
{
    static const char indentLine [] =
        "                                        ";
    //  "1234567890123456789012345678901234567890"
    //            1         2         3         4

    static const int  maxSpaces = sizeof(indentLine) - 1;

    if (spacePerLevel != 0) {
        int numSpaces = depth * spacePerLevel;
        if (numSpaces > maxSpaces) {
            numSpaces = maxSpaces;
        }
        out.write(indentLine, numSpaces);
    }

    if (prefixText) {
        out << prefixText;
    }

    int startPos = this->startPos();
    int endPos = startPos
               + this->d_consumedHeaderBytes
               + this->d_consumedBodyBytes
               + this->d_consumedTailBytes;

    out << " depth="
        << depth
        << " pos=("
        << startPos
        << ','
        << endPos
        << ") tag=(";

    bdem_BerUniversalTagNumber::Value  eTagNum;
    const char *strTagNum = 0;

    switch(d_tagClass) {
      case bdem_BerConstants::BDEM_UNIVERSAL:
        out << "UNV-";
        if (0 == bdem_BerUniversalTagNumber::fromInt(&eTagNum, d_tagNumber)) {
            strTagNum = bdem_BerUniversalTagNumber::toString(eTagNum);
        }
        break;

      case bdem_BerConstants::BDEM_CONTEXT_SPECIFIC:
        out << "CTX-";
        break;
      case bdem_BerConstants::BDEM_APPLICATION:
        out << "APP-";
        break;
      case bdem_BerConstants::BDEM_PRIVATE:
        out << "PRV-";
        break;
      default:
        out << "***-";
        break;
    }

    switch(d_tagType) {
      case bdem_BerConstants::BDEM_CONSTRUCTED:  out << "C-";  break;
      case bdem_BerConstants::BDEM_PRIMITIVE:    out << "P-";  break;
      default:                              out << "*-";  break;
    }

    if (!strTagNum) {
        out << d_tagNumber;
    }
    else {
        out << strTagNum;
    }

    out << ") len=" << this->length();

    if (d_fieldName != 0) {
        out << " name=" << d_fieldName;
    }

//     if (d_typeName != 0) {
//         out << " type=" << d_typeName;
//     }

    out << bsl::endl;
}

// MANIPULATORS
int
bdem_BerDecoder_Node::logError(const  char *msg)
{
    bdem_BerDecoder::ErrorSeverity rc = d_decoder->logError(msg);

    bsl::ostream& out = d_decoder->logStream();
    printStack(out);
    return rc;
}

int
bdem_BerDecoder_Node::decode(bsl::vector<char> *variable,
                             bdeat_TypeCategory::Array)
{
    switch(d_formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_DEFAULT:
      case bdeat_FormattingMode::BDEAT_BASE64:
      case bdeat_FormattingMode::BDEAT_HEX:
      case bdeat_FormattingMode::BDEAT_TEXT:
        return this->readVectorChar(variable);

      default:
        break;
    }

    return this->decodeArray(variable);
}

int
bdem_BerDecoder_Node::readTagHeader()
{
    if (d_decoder->maxDepthExceeded()) {
        return logError("Max depth exceeded");
    }

    if (0 != bdem_BerUtil::getIdentifierOctets(d_decoder->d_streamBuf,
                                               &d_tagClass,
                                               &d_tagType,
                                               &d_tagNumber,
                                               &d_consumedHeaderBytes)) {
        return logError("Error reading BER tag");
    }

    if (0 != bdem_BerUtil::getLength(d_decoder->d_streamBuf,
                                     &d_expectedLength,
                                     &d_consumedHeaderBytes)) {
        return logError("Error reading BER length");
    }

    if (d_decoder->decoderOptions()->traceLevel() > 0) {

        bsl::ostream& out = d_decoder->logStream();
        this->print(out, d_decoder->d_currentDepth, 2, "Enter ");
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

int
bdem_BerDecoder_Node::readTagTrailer()
{
    if (bdem_BerUtil::BDEM_INDEFINITE_LENGTH == d_expectedLength) {

        if (0 != bdem_BerUtil::getEndOfContentOctets(
                                            d_decoder->d_streamBuf,
                                            &d_consumedTailBytes)) {
            return logError("Error reading end-of-contents octets");
        }
    }
    else if (d_expectedLength != d_consumedBodyBytes) {
        return logError("Expected length is not equal to consumed length");
    }

    if (d_decoder->decoderOptions()->traceLevel() > 0) {
        bsl::ostream& out = d_decoder->logStream();
        this->print(out, d_decoder->d_currentDepth, 2, "Leave ");

    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

int
bdem_BerDecoder_Node::skipField()
{
    if (!d_decoder->decoderOptions()->skipUnknownElements()) {
        return logError("Unknown element (skipping is disabled)");
    }

    if (bdem_BerUtil::BDEM_INDEFINITE_LENGTH != d_expectedLength ) {

        // We would do this, but not every streambuf is seekable :( .
        //
        // d_decoder->d_streamBuf->pubseekoff(d_expectedLength,
        //                                    bsl::ios_base::cur,
        //                                    bsl::ios_base::in);

        char buffer[1024];
        int  remainLength = d_expectedLength;

        while (remainLength > 0) {

            int numRead = remainLength < (int) sizeof(buffer)
                        ? remainLength : (int) sizeof(buffer);

            if (numRead != d_decoder->d_streamBuf->sgetn(buffer, numRead)) {
                return logError("Error reading stream while skipping field");
            }

            d_consumedBodyBytes += numRead;
            remainLength -= numRead;
        }

        return bdem_BerDecoder::BDEM_BER_SUCCESS;
    }

    // must be CONSTRUCTED, so recursively skip sub-fields
    if (d_tagType != bdem_BerConstants::BDEM_CONSTRUCTED) {
        return logError(
            "Only CONSTRUCTED fields with INDEFINITE length can be skipped");
    }

    while (hasMore())  {

        bdem_BerDecoder_Node innerNode(d_decoder);

        int rc = innerNode.readTagHeader();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }

        rc = innerNode.skipField();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }

        rc = innerNode.readTagTrailer();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

int
bdem_BerDecoder_Node::readVectorChar(bsl::vector<char> *variable)
{
    if (d_tagType != bdem_BerConstants::BDEM_PRIMITIVE) {
        return logError("Expected PRIMITIVE tag type for 'vector<char>'");
    }

    if (d_expectedLength < 0) {
        return logError("'vector<char>' with indefinite length "
                        "is not supported at this time");

        // TBD X.690 has a formula for transmitting string types in chunks,
        // where each chunk has pre-defined length but the overall string has
        // indefinite length.  We should implement this algorithm.
    }

    int maxSize = d_decoder->decoderOptions()->maxSequenceSize();
    if (d_expectedLength > maxSize) {
        return logError("'vector<char>' length more then limit");
    }

    variable->resize(d_expectedLength);

    if (0 != d_expectedLength &&
        d_expectedLength != d_decoder->d_streamBuf->sgetn(&(*variable)[0],
                                                          d_expectedLength)) {
        return logError("Stream error while reading 'vector<char>'");
    }

    d_consumedBodyBytes += d_expectedLength;

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
