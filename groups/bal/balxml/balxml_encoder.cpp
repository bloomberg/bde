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

#include <bdlat_formattingmode.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balxml {

                        // ---------------------------
                        // class Encoder::MemOutStream
                        // ---------------------------

Encoder::MemOutStream::~MemOutStream()
{
}

                               // -------------
                               // class Encoder
                               // -------------

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
                                      const bsl::string_view&   tag,
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

                  // ---------------------------------------
                  // struct Encoder_OptionsCompatibilityUtil
                  // ---------------------------------------

// PRIVATE CLASS METHODS
int Encoder_OptionsCompatibilityUtil::getFormatterInitialIndentLevel(
                                         const balxml::EncoderOptions& options)
{
    BSLMF_ASSERT(2 == balxml::EncodingStyle::NUM_ENUMERATORS);

    int result = 0;

    switch (options.encodingStyle()) {
      case balxml::EncodingStyle::COMPACT: {
        result = 0;
      } break;
      case balxml::EncodingStyle::PRETTY: {
        result = options.initialIndentLevel();
      } break;
    }

    return result;
}

int Encoder_OptionsCompatibilityUtil::getFormatterSpacesPerLevel(
                                         const balxml::EncoderOptions& options)
{
    BSLMF_ASSERT(2 == balxml::EncodingStyle::NUM_ENUMERATORS);

    int result = 0;

    switch (options.encodingStyle()) {
      case balxml::EncodingStyle::COMPACT: {
        result = 0;
      } break;
      case balxml::EncodingStyle::PRETTY: {
        result = options.spacesPerLevel();
      } break;
    }

    return result;
}

int Encoder_OptionsCompatibilityUtil::getFormatterWrapColumn(
                                         const balxml::EncoderOptions& options)
{
    BSLMF_ASSERT(2 == balxml::EncodingStyle::NUM_ENUMERATORS);

    int result = 0;

    switch (options.encodingStyle()) {
      case balxml::EncodingStyle::COMPACT: {
        result = -1;
      } break;
      case balxml::EncodingStyle::PRETTY: {
        result = options.wrapColumn();
      } break;
    }

    return result;
}

// CLASS METHODS
void Encoder_OptionsCompatibilityUtil::getFormatterOptions(
                                int                   *formatterIndentLevel,
                                int                   *formatterSpacesPerLevel,
                                int                   *formatterWrapColumn,
                                EncoderOptions        *formatterOptions,
                                const EncoderOptions&  encoderOptions)
{
    BSLS_ASSERT_OPT(formatterIndentLevel);
    BSLS_ASSERT_OPT(formatterSpacesPerLevel);
    BSLS_ASSERT_OPT(formatterWrapColumn);
    BSLS_ASSERT_OPT(formatterOptions);
    BSLS_ASSERT_OPT(EncoderOptions() == *formatterOptions);

    BSLMF_ASSERT(16 == EncoderOptions::NUM_ATTRIBUTES);

    *formatterIndentLevel    = getFormatterInitialIndentLevel(encoderOptions);
    *formatterSpacesPerLevel = getFormatterSpacesPerLevel(encoderOptions);
    *formatterWrapColumn     = getFormatterWrapColumn(encoderOptions);

    // TODO: A prior version of this component's implementation failed to
    // forward all encoder options other than 'InitialIndentLevel',
    // 'SpacesPerLevel', and 'WrapColumn' to the formatter[1].  As a result,
    // the formatter used the default values of these options.
    //
    // This function now forwards 'MaxDecimalTotalDigits' and
    // 'MaxDecimalFractionDigits' to the formatter, but does not yet forward
    // 'DatetimeFractionalSecondPrecision', 'AllowControlCharacters', nor
    // 'UseZAbbreviationForUtc'[2].  Since changing the implementation to
    // (correctly) forward these arguments can result in observable and non-
    // backward-compatible changes to the results of encoding some values, this
    // component elects to not forward these options at this time.
    //
    // [1] Note that this component only uses the formatter to emit the XML
    // header, open elements, close elements, and emit attributes.  In
    // particular, this component directly invokes 'TypesPrintUtil' operations,
    // with correct option forwarding, to emit scalar values that are not
    // attributes.  This may account for encoder behavior that appears
    // surprisingly correct in light of this bug.
    //
    // [2] The formatter uses no other options from the encoder options as of
    // Wednesday, December 16 2020.

    const EncoderOptions defaultEncoderOptions;

    // 1. ObjectNamespace
    formatterOptions->setObjectNamespace(encoderOptions.objectNamespace());

    // 2. SchemaLocation
    formatterOptions->setSchemaLocation(encoderOptions.schemaLocation());

    // 3. Tag
    formatterOptions->setTag(encoderOptions.tag());

    // 4. FormattingMode
    formatterOptions->setFormattingMode(encoderOptions.formattingMode());

    // 5. InitialIndentLevel
    formatterOptions->setInitialIndentLevel(
        encoderOptions.initialIndentLevel());

    // 6. SpacesPerLevel
    formatterOptions->setSpacesPerLevel(encoderOptions.spacesPerLevel());

    // 7. WrapColumn
    formatterOptions->setWrapColumn(encoderOptions.wrapColumn());

    // 8. DatetimeFractionalSecondPrecision
    // TODO: Replace the following code:
    //..
    formatterOptions->setDatetimeFractionalSecondPrecision(
          defaultEncoderOptions.datetimeFractionalSecondPrecision());
    //..
    // with:
    //..
    //  formatterOptions->setDatetimeFractionalSecondPrecision(
    //        encoderOptions.datetimeFractionalSecondPrecision());
    //..
    //

    // 9. MaxDecimalTotalDigits
    formatterOptions->setMaxDecimalTotalDigits(
          encoderOptions.maxDecimalTotalDigits());

    // 10. MaxDecimalFractionDigits
    formatterOptions->setMaxDecimalFractionDigits(
          encoderOptions.maxDecimalFractionDigits());

    // 11. SignificantDoubleDigits
    formatterOptions->setSignificantDoubleDigits(
        encoderOptions.significantDoubleDigits());

    // 12. EncodingStyle
    formatterOptions->setEncodingStyle(encoderOptions.encodingStyle());

    // 13. AllowControlCharacters
    // TODO: Replace the following code:
    //..
    formatterOptions->setAllowControlCharacters(
          defaultEncoderOptions.allowControlCharacters());
    //..
    // with:
    //..
    //  formatterOptions->setAllowControlCharacters(
    //        encoderOptions.allowControlCharacters());
    //..

    // 14. OutputXMLHeader
    formatterOptions->setOutputXMLHeader(encoderOptions.outputXMLHeader());

    // 15. OutputXSIAlias
    formatterOptions->setOutputXSIAlias(encoderOptions.outputXSIAlias());

    // 16. UseZAbbreviationForUtc
    // TODO: Replace the following code:
    //..
    formatterOptions->setUseZAbbreviationForUtc(
          defaultEncoderOptions.useZAbbreviationForUtc());
    //..
    // with:
    //..
    //  formatterOptions->setUseZAbbreviationForUtc(
    //        encoderOptions.useZAbbreviationForUtc());
    //..
}


                         // --------------------------
                         // class Encoder_EncodeObject
                         // --------------------------

int Encoder_EncodeObject::executeImp(const bsl::vector<char>&  object,
                                     const bsl::string_view&   tag,
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
