// balxml_encoder.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_encoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_encoder_cpp,"$Id$ $CSID$")

#ifdef TEST
#include <balxml_decoder.h>     // for testing only
#include <balxml_minireader.h>  // for testing only
#endif

#include <bdlat_formattingmode.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

                   // --------------------------------------
                   // class baexml::BerEncoder::MemOutStream
                   // --------------------------------------

balxml::Encoder::MemOutStream::~MemOutStream()
{
}

namespace balxml {                       // --------------------
                       // class Encoder
                       // --------------------

Encoder::Encoder(const EncoderOptions *options,
                 bslma::Allocator     *basicAllocator)
: d_options       (options)
, d_allocator     (bslma::Default::allocator(basicAllocator))
, d_logStream     (0)
, d_severity      (ErrorInfo::e_NO_ERROR)
, d_errorStream   (0)
, d_warningStream (0)
{
}

Encoder::Encoder(const EncoderOptions *options,
                 bsl::ostream         *errorStream,
                 bsl::ostream         *warningStream,
                 bslma::Allocator     *basicAllocator)
: d_options       (options)
, d_allocator     (bslma::Default::allocator(basicAllocator))
, d_logStream     (0)
, d_severity      (ErrorInfo::e_NO_ERROR)
, d_errorStream   (errorStream)
, d_warningStream (warningStream)
{
}

Encoder::~Encoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

ErrorInfo::Severity  Encoder::logError(
                                      const char               *text,
                                      const bslstl::StringRef&  tag,
                                      int                       formattingMode,
                                      int                       index)
{
    if ((int) d_severity < (int) ErrorInfo::e_ERROR) {
        d_severity = ErrorInfo::e_ERROR;
    }

    bsl::ostream& out = logStream();

    out << text << ':';

    if (index >= 0) {
        out << " index=" << index;
    }

    out << " tag=" << tag
        << " formattingMode=" << formattingMode
        << bsl::endl;

    return d_severity;
}

                           // ---------------------
                           // class Encoder_Context
                           // ---------------------

Encoder_Context::Encoder_Context(
                                Formatter *formatter,
                                Encoder   *encoder)
: d_formatter(formatter)
, d_encoder(encoder)
{
}

                         // --------------------------
                         // class Encoder_EncodeObject
                         // --------------------------

int Encoder_EncodeObject::executeImp(const bsl::vector<char>&  object,
                                     const bslstl::StringRef&  tag,
                                     int                       formattingMode,
                                     bdlat_TypeCategory::Array)
{
    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        return executeArrayListImp(object, tag);                      // RETURN
    }

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_BASE64:
      case bdlat_FormattingMode::e_TEXT:
      case bdlat_FormattingMode::e_HEX: {
        d_context_p->openElement(tag);

        TypesPrintUtil::print(d_context_p->rawOutputStream(),
                                     object,
                                     formattingMode,
                                     &d_context_p->encoderOptions());

        d_context_p->closeElement(tag);

        int ret = d_context_p->status();
        if (ret) {

            d_context_p->logError("Failed to encode",
                                 tag,
                                 formattingMode);
        }

        return ret;                                                   // RETURN
      }
      default: {
        return executeArrayRepetitionImp(object, tag, formattingMode);
                                                                      // RETURN
      }
    }
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
