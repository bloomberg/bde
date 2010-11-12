// bdem_berencoder.cpp                  -*-C++-*-
#include <bdem_berencoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berencoder_cpp,"$Id$ $CSID$")

#include <bdeat_formattingmode.h>
#include <bslma_default.h>

namespace BloombergLP {

                   // -----------------------------------
                   // class bdem_BerEncoder::MemOutStream
                   // -----------------------------------

// CREATORS
bdem_BerEncoder::MemOutStream::~MemOutStream()
{
}

                   // ---------------------
                   // class bdem_BerEncoder
                   // ---------------------

// CREATORS
bdem_BerEncoder::bdem_BerEncoder(const bdem_BerEncoderOptions *options,
                                 bslma_Allocator              *basicAllocator)
: d_options      (options)
, d_allocator    (bslma_Default::allocator(basicAllocator))
, d_logStream    (0)
, d_severity     (BDEM_BER_SUCCESS)
, d_streamBuf    (0)
, d_currentDepth (0)
{
}

bdem_BerEncoder::~bdem_BerEncoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

// PRIVATE MANIPULATORS
bdem_BerEncoder::ErrorSeverity
bdem_BerEncoder::logError(bdem_BerConstants::TagClass  tagClass,
                          int                          tagNumber,
                          const char                  *name,
                          int                          index)
{
    if ((int) d_severity < (int) BDEM_BER_ERROR) {
        d_severity = BDEM_BER_ERROR;
    }

    return logMsg("ERROR", tagClass, tagNumber, name, index);
}

bdem_BerEncoder::ErrorSeverity
bdem_BerEncoder::logMsg(const char                  *msg,
                        bdem_BerConstants::TagClass  tagClass,
                        int                          tagNumber,
                        const char                  *name,
                        int                          index)
{
    bsl::ostream& out = logStream();

    out << msg << ": depth=" << d_currentDepth << " tag=(";

    bdem_BerUniversalTagNumber::Value  eTagNum;
    const char                        *strTagNum = 0;

    switch (tagClass) {
      case bdem_BerConstants::BDEM_UNIVERSAL: {
        out << "UNV-";
        if (0 == bdem_BerUniversalTagNumber::fromInt(&eTagNum, tagNumber)) {
            strTagNum = bdem_BerUniversalTagNumber::toString(eTagNum);
        }
      } break;
      case bdem_BerConstants::BDEM_CONTEXT_SPECIFIC: {
        out << "CTX-";
      } break;
      case bdem_BerConstants::BDEM_APPLICATION: {
        out << "APP-";
      } break;
      case bdem_BerConstants::BDEM_PRIVATE: {
        out << "PRV-";
      } break;
      default: {
        out << "???" "-";  // avoid ? ? - trigraph
      } break;
    }

    if (!strTagNum) {
        out << tagNumber;
    }
    else {
        out << strTagNum;
    }

    out << ')';

    if (name) {
        out << " name="  << name;
    }

    if (index >= 0) {
       out << '[' << index << ']';
    }

    out << bsl::endl;

    return d_severity;
}

int bdem_BerEncoder::encodeImpl(const bsl::vector<char>&    value,
                                bdem_BerConstants::TagClass tagClass,
                                int                         tagNumber,
                                int                         formattingMode,
                                bdeat_TypeCategory::Array)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_DEFAULT:
      case bdeat_FormattingMode::BDEAT_BASE64:
      case bdeat_FormattingMode::BDEAT_HEX:
      case bdeat_FormattingMode::BDEAT_TEXT: {
      } break;
      default: {
        return this->encodeArrayImpl(value,
                                     tagClass,
                                     tagNumber,
                                     formattingMode);
      }
    }

    const int size = value.size();

    int status = bdem_BerUtil::putIdentifierOctets(
                                             d_streamBuf,
                                             tagClass,
                                             bdem_BerConstants::BDEM_PRIMITIVE,
                                             tagNumber);
    status |= bdem_BerUtil::putLength(d_streamBuf, size);

    // If 'size == 0', don't call 'sputn()'.  If 'size != 0', then set 'status'
    // to a non-zero value if 'sputn()' returns a value other than 'size', in
    // which case 'size ^ sputn()' will be non-zero.  If 'sputn() == size',
    // 'status' will not be modified.

    status |= size && (size ^ d_streamBuf->sputn(&value[0], size));

    if (status) {
        logError(tagClass,
                 tagNumber,
                 0 // bdeat_TypeName::name(value)
                );

        return BDEM_FAILURE;
    }

    return BDEM_SUCCESS;
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
