// baexml_encoder.cpp              -*-C++-*-
#include <baexml_encoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_encoder_cpp,"$Id$ $CSID$")

#ifdef TEST
#include <baexml_decoder.h>     // for testing only
#include <baexml_minireader.h>  // for testing only
#endif

#include <bdeat_formattingmode.h>
#include <bdeut_stringref.h>

#include <bsl_vector.h>

namespace BloombergLP {

                   // -------------------------------------
                   // class baexml_BerEncoder::MemOutStream
                   // -------------------------------------

baexml_Encoder::MemOutStream::~MemOutStream()
{
}
                       // --------------------
                       // class baexml_Encoder
                       // --------------------

baexml_Encoder::baexml_Encoder(
                   const baexml_EncoderOptions *options,
                   bslma_Allocator             *basicAllocator)
: d_options       (options)
, d_allocator     (bslma_Default::allocator(basicAllocator))
, d_logStream     (0)
, d_severity      (baexml_ErrorInfo::BAEXML_NO_ERROR)
, d_errorStream   (0)
, d_warningStream (0)
{
}

baexml_Encoder::baexml_Encoder(
                   const baexml_EncoderOptions *options,
                   bsl::ostream                *errorStream,
                   bsl::ostream                *warningStream,
                   bslma_Allocator             *basicAllocator)
: d_options       (options)
, d_allocator     (bslma_Default::allocator(basicAllocator))
, d_logStream     (0)
, d_severity      (baexml_ErrorInfo::BAEXML_NO_ERROR)
, d_errorStream   (errorStream)
, d_warningStream (warningStream)
{
}

baexml_Encoder::~baexml_Encoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

baexml_ErrorInfo::Severity  baexml_Encoder::logError(
                         const char             *text,
                         const bdeut_StringRef&  tag,
                         int                     formattingMode,
                         int                     index)
{
    if ((int) d_severity < (int) baexml_ErrorInfo::BAEXML_ERROR) {
        d_severity = baexml_ErrorInfo::BAEXML_ERROR;
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

                       // ----------------------------
                       // class baexml_Encoder_Context
                       // ----------------------------

baexml_Encoder_Context::baexml_Encoder_Context(
                                baexml_Formatter *formatter,
                                baexml_Encoder   *encoder)
: d_formatter(formatter)
, d_encoder(encoder)
{
}

                       // ------------------------------
                       // class baexml_Encoder_EncodeObject
                       // ------------------------------

int baexml_Encoder_EncodeObject::executeImp(
                                      const bsl::vector<char>&  object,
                                      const bdeut_StringRef&    tag,
                                      int                       formattingMode,
                                      bdeat_TypeCategory::Array)
{
    if (formattingMode & bdeat_FormattingMode::BDEAT_LIST) {
        return executeArrayListImp(object, tag);
    }

    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_BASE64:
      case bdeat_FormattingMode::BDEAT_TEXT:
      case bdeat_FormattingMode::BDEAT_HEX: {
        d_context_p->openElement(tag);

        baexml_TypesPrintUtil::print(d_context_p->rawOutputStream(),
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

        return ret;
      }
      default: {
        return executeArrayRepetitionImp(object, tag, formattingMode);
      }
    }
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
