// balber_berencoder.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berencoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berencoder_cpp,"$Id$ $CSID$")

#include <bdlat_formattingmode.h>
#include <bslma_default.h>

namespace BloombergLP {

                   // --------------------------------------
                   // class balber::BerEncoder::MemOutStream
                   // --------------------------------------

// CREATORS
balber::BerEncoder::MemOutStream::~MemOutStream()
{
}

namespace balber {

                              // ----------------
                              // class BerEncoder
                              // ----------------

// CREATORS
BerEncoder::BerEncoder(const BerEncoderOptions *options,
                       bslma::Allocator        *basicAllocator)
: d_options      (options)
, d_allocator    (bslma::Default::allocator(basicAllocator))
, d_logStream    (0)
, d_severity     (e_BER_SUCCESS)
, d_streamBuf    (0)
, d_currentDepth (0)
{
}

BerEncoder::~BerEncoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

// PRIVATE MANIPULATORS
BerEncoder::ErrorSeverity
BerEncoder::logError(BerConstants::TagClass  tagClass,
                     int                     tagNumber,
                     const char             *name,
                     int                     index)
{
    if (static_cast<int>(d_severity) < static_cast<int>(e_BER_ERROR)) {
        d_severity = e_BER_ERROR;
    }

    return logMsg("ERROR", tagClass, tagNumber, name, index);
}

BerEncoder::ErrorSeverity
BerEncoder::logMsg(const char             *msg,
                   BerConstants::TagClass  tagClass,
                   int                     tagNumber,
                   const char             *name,
                   int                     index)
{
    bsl::ostream& out = logStream();

    out << msg << ": depth=" << d_currentDepth << " tag=(";

    BerUniversalTagNumber::Value  eTagNum;
    const char                   *strTagNum = 0;

    switch (tagClass) {
      case BerConstants::e_UNIVERSAL: {
        out << "UNV-";
        if (0 == BerUniversalTagNumber::fromInt(&eTagNum, tagNumber)) {
            strTagNum = BerUniversalTagNumber::toString(eTagNum);
        }
      } break;
      case BerConstants::e_CONTEXT_SPECIFIC: {
        out << "CTX-";
      } break;
      case BerConstants::e_APPLICATION: {
        out << "APP-";
      } break;
      case BerConstants::e_PRIVATE: {
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

    out << '\n' << bsl::flush;

    return d_severity;
}

int BerEncoder::encodeImpl(const bsl::vector<char>&  value,
                           BerConstants::TagClass    tagClass,
                           int                       tagNumber,
                           int                       formattingMode,
                           bdlat_TypeCategory::Array )
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_DEFAULT:
      case bdlat_FormattingMode::e_BASE64:
      case bdlat_FormattingMode::e_HEX:
      case bdlat_FormattingMode::e_TEXT: {
      } break;
      default: {
        return this->encodeArrayImpl(value,
                                     tagClass,
                                     tagNumber,
                                     formattingMode);                 // RETURN
      }
    }

    const int size = value.size();

    int status = BerUtil::putIdentifierOctets(d_streamBuf,
                                              tagClass,
                                              BerConstants::e_PRIMITIVE,
                                              tagNumber);
    status |= BerUtil::putLength(d_streamBuf, size);

    // If 'size == 0', don't call 'sputn()'.  If 'size != 0', then set 'status'
    // to a non-zero value if 'sputn()' returns a value other than 'size', in
    // which case 'size ^ sputn()' will be non-zero.  If 'sputn() == size',
    // 'status' will not be modified.

    status |= size && (size ^ d_streamBuf->sputn(&value[0], size));

    if (status) {
        logError(tagClass,
                 tagNumber,
                 0 // bdlat_TypeName::name(value)
                );

        return k_FAILURE;
    }

    return k_SUCCESS;
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
