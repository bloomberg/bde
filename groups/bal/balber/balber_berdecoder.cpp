// balber_berdecoder.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berdecoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berdecoder_cpp,"$Id$ $CSID$")

#include <balber_berencoder.h>          // for testing only
#include <bdlsb_fixedmeminstreambuf.h>  // for testing only
#include <bdlsb_memoutstreambuf.h>      // for testing only

namespace BloombergLP {

                   // --------------------------------------
                   // class balber::BerDecoder::MemOutStream
                   // --------------------------------------
// CREATORS
balber::BerDecoder::MemOutStream::~MemOutStream()
{
}

namespace balber {

                              // ----------------
                              // class BerDecoder
                              // ----------------

// CREATORS
BerDecoder::BerDecoder(const BerDecoderOptions *options,
                       bslma::Allocator        *basicAllocator)
: d_options                  (options)
, d_allocator                (bslma::Default::allocator(basicAllocator))
, d_logStream                (0)
, d_severity                 (e_BER_SUCCESS)
, d_streamBuf                (0)
, d_currentDepth             (0)
, d_numUnknownElementsSkipped(0)
, d_topNode                  (0)
{
}

BerDecoder::~BerDecoder()
{
    if (d_logStream) {
        d_logStream->~MemOutStream();
    }
}

// MANIPULATORS
void BerDecoder::logErrorImp(const char *msg)
{
    if ((int) d_severity < (int) e_BER_ERROR) {
        d_severity = e_BER_ERROR;
    }
    logMsg("ERROR", msg);
}

BerDecoder::ErrorSeverity BerDecoder::logMsg(const char *prefix,
                                             const char *msg)
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

                       // -----------------------------
                       // private class BerDecoder_Node
                       // -----------------------------

// ACCESSORS
int
BerDecoder_Node::startPos() const
{
    int ret = 0;

    BerDecoder_Node *node = d_parent;

    for (; node != 0; node = node->d_parent) {
        ret += node->d_consumedHeaderBytes
            +  node->d_consumedBodyBytes;
    }
    return ret;
}

void
BerDecoder_Node::printStack(bsl::ostream& out) const
{
    int depth = d_decoder->d_currentDepth;

    for (const BerDecoder_Node *node = this;
         node != 0; node = node->parent()) {
        node->print(out, depth--, 0, " within ");
    }
}

void BerDecoder_Node::print(bsl::ostream&  out,
                            int            depth,
                            int            spacePerLevel,
                            const char    *prefixText) const
{
    static const char indentLine [] =
        "                                        ";
    // "1234567890123456789012345678901234567890" 1 2 3 4

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

    BerUniversalTagNumber::Value  eTagNum;
    const char *strTagNum = 0;

    switch(d_tagClass) {
      case BerConstants::e_UNIVERSAL:        out << "UNV-";
        if (0 == BerUniversalTagNumber::fromInt(&eTagNum, d_tagNumber)) {
            strTagNum = BerUniversalTagNumber::toString(eTagNum);
        }
                                                            break;
      case BerConstants::e_CONTEXT_SPECIFIC: out << "CTX-"; break;
      case BerConstants::e_APPLICATION:      out << "APP-"; break;
      case BerConstants::e_PRIVATE:          out << "PRV-"; break;
      default:                               out << "***-"; break;
    }

    switch(d_tagType) {
      case BerConstants::e_CONSTRUCTED:       out << "C-";  break;
      case BerConstants::e_PRIMITIVE:         out << "P-";  break;
      default:                                out << "*-";  break;
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

    out << bsl::endl;
}

// MANIPULATORS
int BerDecoder_Node::logError(const char *msg)
{
    BerDecoder::ErrorSeverity rc = d_decoder->logError(msg);

    bsl::ostream& out = d_decoder->logStream();
    printStack(out);
    return rc;
}

int BerDecoder_Node::decode(bsl::vector<char> *variable,
                            bdlat_TypeCategory::Array)
{
    switch(d_formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_DEFAULT:
      case bdlat_FormattingMode::e_BASE64:
      case bdlat_FormattingMode::e_HEX:
      case bdlat_FormattingMode::e_TEXT:
        return this->readVectorChar(variable);                        // RETURN

      default:
        break;
    }

    return this->decodeArray(variable);
}

int BerDecoder_Node::readTagHeader()
{
    if (d_decoder->maxDepthExceeded()) {
        return logError("Max depth exceeded");                        // RETURN
    }

    if (0 != BerUtil::getIdentifierOctets(d_decoder->d_streamBuf,
                                               &d_tagClass,
                                               &d_tagType,
                                               &d_tagNumber,
                                               &d_consumedHeaderBytes)) {
        return logError("Error reading BER tag");
    }

    if (0 != BerUtil::getLength(d_decoder->d_streamBuf,
                                     &d_expectedLength,
                                     &d_consumedHeaderBytes)) {
        return logError("Error reading BER length");                  // RETURN
    }

    if (d_decoder->decoderOptions()->traceLevel() > 0) {

        bsl::ostream& out = d_decoder->logStream();
        this->print(out, d_decoder->d_currentDepth, 2, "Enter ");
    }

    return BerDecoder::e_BER_SUCCESS;
}

int BerDecoder_Node::readTagTrailer()
{
    if (BerUtil::e_INDEFINITE_LENGTH == d_expectedLength) {

        if (0 != BerUtil::getEndOfContentOctets(
                                            d_decoder->d_streamBuf,
                                            &d_consumedTailBytes)) {
            return logError("Error reading end-of-contents octets");  // RETURN
        }
    }
    else if (d_expectedLength != d_consumedBodyBytes) {
        return logError("Expected length is not equal to consumed length");
                                                                      // RETURN
    }

    if (d_decoder->decoderOptions()->traceLevel() > 0) {
        bsl::ostream& out = d_decoder->logStream();
        this->print(out, d_decoder->d_currentDepth, 2, "Leave ");

    }

    return BerDecoder::e_BER_SUCCESS;
}

int BerDecoder_Node::skipField()
{
    if (!d_decoder->decoderOptions()->skipUnknownElements()) {
        return logError("Unknown element (skipping is disabled)");
                                                                      // RETURN
    }

    if (BerUtil::e_INDEFINITE_LENGTH != d_expectedLength ) {

        // We would do this, but not every streambuf is seekable:
        //..
        //  d_decoder->d_streamBuf->pubseekoff(d_expectedLength,
        //                                     bsl::ios_base::cur,
        //..                                   bsl::ios_base::in);

        char buffer[1024];
        int  remainLength = d_expectedLength;

        while (remainLength > 0) {

            int numRead = remainLength < (int) sizeof(buffer)
                        ? remainLength : (int) sizeof(buffer);

            if (numRead != d_decoder->d_streamBuf->sgetn(buffer, numRead)) {
                return logError("Error reading stream while skipping field");
                                                                      // RETURN
            }

            d_consumedBodyBytes += numRead;
            remainLength -= numRead;
        }

        return BerDecoder::e_BER_SUCCESS;
    }

    // must be CONSTRUCTED, so recursively skip sub-fields
    if (d_tagType != BerConstants::e_CONSTRUCTED) {
        return logError(
            "Only CONSTRUCTED fields with INDEFINITE length can be skipped");
                                                                      // RETURN
    }

    while (hasMore())  {

        BerDecoder_Node innerNode(d_decoder);

        int rc = innerNode.readTagHeader();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged
                                                                      // RETURN
        }

        rc = innerNode.skipField();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged
                                                                      // RETURN
        }

        rc = innerNode.readTagTrailer();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged
                                                                      // RETURN
        }
    }

    return BerDecoder::e_BER_SUCCESS;
}

int BerDecoder_Node::readVectorChar(bsl::vector<char> *variable)
{
    if (d_tagType != BerConstants::e_PRIMITIVE) {
        return logError("Expected PRIMITIVE tag type for 'vector<char>'");
                                                                      // RETURN
    }

    if (d_expectedLength < 0) {
        return logError("'vector<char>' with indefinite length "
                        "is not supported at this time");
                                                                      // RETURN

        // TBD X.690 has a formula for transmitting string types in chunks,
        // where each chunk has pre-defined length but the overall string has
        // indefinite length.  We should implement this algorithm.
    }

    int maxSize = d_decoder->decoderOptions()->maxSequenceSize();
    if (d_expectedLength > maxSize) {
        return logError("'vector<char>' length more then limit");
                                                                      // RETURN
    }

    variable->resize(d_expectedLength);

    if (0 != d_expectedLength &&
        d_expectedLength != d_decoder->d_streamBuf->sgetn(&(*variable)[0],
                                                          d_expectedLength)) {
        return logError("Stream error while reading 'vector<char>'");
                                                                      // RETURN
    }

    d_consumedBodyBytes += d_expectedLength;

    return BerDecoder::e_BER_SUCCESS;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
