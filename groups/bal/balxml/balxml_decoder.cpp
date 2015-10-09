// balxml_decoder.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_decoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_decoder_cpp,"$Id$ $CSID$")

#include <balxml_elementattribute.h>
#include <balxml_minireader.h>      // for testing purposes only

#include <bslalg_typetraits.h>

#include <bsl_cstddef.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

// IMPLEMENTATION NOTES
// --------------------
// Each of the 'TYPE's is decoded depending on the traits of 'TYPE'.  For
// example, if 'TYPE' is a sequence, then each of the sub-elements is decoded
// by recursively calling the parser.  If 'TYPE' is a choice, then only 1
// sub-element is allowed, and the sub-element is decoded by recursively
// calling the parser.
//
// To accommodate the different behaviors for different types, the decoding
// behavior is implemented in an "element context".  The
// 'balxml::Decoder_SelectContext' meta-function returns an appropriate context
// type to handle the specified 'TYPE'.  All the contexts implement the
// 'balxml::Decoder_ElementContext' protocol.  When the 'Parser' class detects
// certain parsing events, it uses the 'balxml::Decoder_ElementContext'
// protocol to allow the context of each type to handle the event.
//
// The following contexts are defined in this component:
//..
//  Context Name                       Comments
//  ------------                       --------
//  balxml::Decoder_ChoiceContext          Used for 'TYPE's that fall under
//                                     'bdlat_TypeCategory::Choice'.
//
//  balxml::Decoder_PushParserContext      Used for 'TYPE's that can be parsed
//                                     using one of the push parsers (i.e.,
//                                     'balxml::Base64Parser',
//                                     'balxml::HexParser', and
//                                     'balxml::Decoder_ListParser').
//
//  balxml::Decoder_SequenceContext        Used for 'TYPE's that fall under
//                                     'bdlat_TypeCategory::Sequence'.
//
//  balxml::Decoder_SimpleContext          Used for 'TYPE's that can be parsed
//                                     using 'balxml::TypesParserUtil'.
//
//  balxml::Decoder_UTF8Context            Used for 'bsl::string' and
//                                     'bsl::vector<char>' types when the
//                                     formatting mode is
//                                     'bdlat_FormattingMode::e_TEXT'.
//                                     Note that this is just an optimization
//                                     of 'balxml::Decoder_SimpleContext' where
//                                     the characters are copied directly into
//                                     the destination object, instead of a
//                                     temporary 'd_chars' member.
//
//  balxml::Decoder_StdStringContext       Used for 'bsl::string'.  This
//                                     context simply delegates the callbacks
//                                     to another context.  The delegated
//                                     context is selected using the formatting
//                                     mode.
//
//  balxml::Decoder_StdVectorCharContext   Used for 'bsl::vector<char>'.  This
//                                     context simply delegates the callbacks
//                                     to another context.  The delegated
//                                     context is selected using the formatting
//                                     mode.
//..
// The main parsing logic happens in the 'Parser' class (defined in the CPP
// file).  It uses Xerces to parse the input stream and can be used
// recursively - i.e., the 'Parser' class calls the 'parseSubElement' function
// in the 'balxml::Decoder_ElementContext' protocol and the implementation of
// the 'parseSubElement' function can call the 'parse' function in the
// 'Parser' class (through the 'baexml_Decoder_Parser' protocol), passing it
// the context of the sub-element.

namespace BloombergLP {

namespace {

// HELPER CLASSES

template <class T>
class AutoIncDec
{
private:
    // UNIMPLEMENTED
    AutoIncDec (const AutoIncDec<T> & other);
    void operator = (const AutoIncDec<T> & other);

     T & d_val;
public:
    AutoIncDec (T & t) : d_val (t)
    {
        ++d_val;
    }

    ~AutoIncDec ()
    {
        --d_val;
    }

    const T & value () const
    {
        return d_val;
    }
};

template <class T>
class AutoDecInc
{
private:
    // UNIMPLEMENTED
    AutoDecInc (const AutoDecInc<T> & other);
    void operator = (const AutoDecInc<T> & other);

     T & d_val;
public:
    AutoDecInc (T & t) : d_val (t)
    {
        --d_val;
    }

    ~AutoDecInc ()
    {
        ++d_val;
    }

    const T & value () const
    {
        return d_val;
    }
};

}  // close unnamed namespace

namespace balxml {
                        // ----------------------------
                        // class Decoder_ElementContext
                        // ----------------------------

Decoder_ElementContext::~Decoder_ElementContext()
{
}

int
Decoder_ElementContext::beginParse(Decoder *decoder)
{
    return decoder->parse(this);
}
}  // close package namespace

                   // --------------------------------------
                   // class baexml::BerDecoder::MemOutStream
                   // --------------------------------------

balxml::Decoder::MemOutStream::~MemOutStream()
{
}

namespace balxml {
                               // -------------
                               // class Decoder
                               // -------------

// CREATORS
Decoder::Decoder(const DecoderOptions *options,
                 Reader               *reader,
                 ErrorInfo            *errInfo,
                 bslma::Allocator     *basicAllocator)
: d_options(options)
, d_reader(reader)
, d_errorInfo(errInfo)
, d_allocator(bslma::Default::allocator(basicAllocator))
, d_logStream(0)
, d_errorStream(0)
, d_warningStream(0)
, d_sourceUri(d_allocator)
, d_errorCount(0)
, d_warningCount(0)
, d_numUnknownElementsSkipped(0)
, d_fatalError(false)
, d_remainingDepth(1)
{
    BSLS_ASSERT(d_options != 0);
    BSLS_ASSERT(d_reader != 0);
}

Decoder::Decoder(const DecoderOptions *options,
                 Reader               *reader,
                 ErrorInfo            *errInfo,
                 bsl::ostream         *errorStream,
                 bsl::ostream         *warningStream,
                 bslma::Allocator     *basicAllocator)
: d_options(options)
, d_reader(reader)
, d_errorInfo(errInfo)
, d_allocator(bslma::Default::allocator(basicAllocator))
, d_logStream(0)
, d_errorStream(errorStream)
, d_warningStream(warningStream)
, d_sourceUri(d_allocator)
, d_errorCount(0)
, d_warningCount(0)
, d_numUnknownElementsSkipped(0)
, d_fatalError(false)
, d_remainingDepth(1)
{
    BSLS_ASSERT(d_options != 0);
    BSLS_ASSERT(d_reader != 0);
}

Decoder::~Decoder()
{
    this->close();
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

// PRIVATE MANIPULATORS
void
Decoder::resetErrors()
{
    d_errorCount = 0;
    d_warningCount = 0;
    d_fatalError = 0;
    d_numUnknownElementsSkipped = 0;

    if (d_logStream != 0) {
        d_logStream->reset();
    }

    d_remainingDepth = d_options->maxDepth();

    if (d_errorInfo) {
        d_errorInfo->reset();
    }
    this->close();
}

void
Decoder::close()
{
    d_reader->close();
}

int
Decoder::open(bsl::streambuf *buffer, const char *uri)
{
    this->resetErrors();

    uri = uri ? uri : "STREAM.xml";
    d_sourceUri = uri;
    if (d_reader->open(buffer, uri) != 0) {
        BALXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input stream."
                     << BALXML_DECODER_LOG_END;
        return -1;                                                    // RETURN
    }
    return readTopElement();
}

int
Decoder::open(const char *buffer, size_t length, const char *uri)
{
    this->resetErrors();

    uri = uri ? uri : "STREAM.xml";
    d_sourceUri = uri;
    if (d_reader->open(buffer, length, uri) != 0) {
        BALXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input string."
                     << BALXML_DECODER_LOG_END;
        return -1;                                                    // RETURN
    }
    return readTopElement();
}

int
Decoder::open(const char *filename)
{
    this->resetErrors();

    d_sourceUri = filename;
    if (d_reader->open(filename) != 0) {
        BALXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input file."
                     << BALXML_DECODER_LOG_END;
        return -1;                                                    // RETURN
    }
    return readTopElement();
}

int
Decoder::readTopElement()
{
    // read till we find first ELEMENT node and there are no errors
    do {
        int rc1 = d_reader->advanceToNextNode();
        int rc2 = checkForReaderErrors();

        if (rc1 != 0 || rc2 < 0) {
            d_fatalError = true;
            BALXML_DECODER_LOG_ERROR(this)
                     << "Fatal error while looking for root-level element."
                     << BALXML_DECODER_LOG_END;

            return -1;                                                // RETURN
        }

    } while (d_reader->nodeType() != Reader::e_NODE_TYPE_ELEMENT);

    return 0;
}

void
Decoder::setDecoderError(ErrorInfo::Severity severity,
                         bslstl::StringRef   msg)
{
    ErrorInfo errInfo;
    errInfo.setError(severity,
                     d_reader->getLineNumber(),
                     d_reader->getColumnNumber(),
                     d_sourceUri,
                     msg);

    checkForErrors(errInfo);
}

int
Decoder::checkForReaderErrors ()
{
    return checkForErrors(d_reader->errorInfo());
}

int
Decoder::checkForErrors(const ErrorInfo& errInfo)
{
    switch (errInfo.severity()) {
      case ErrorInfo::e_NO_ERROR:
        return 0;                                                     // RETURN

      case ErrorInfo::e_WARNING:
        ++d_warningCount;
        break;

      case ErrorInfo::e_ERROR:
      case ErrorInfo::e_FATAL_ERROR:
        d_fatalError = true;
        ++d_errorCount;
        break;

      default:
        BSLS_ASSERT(0);  // should never be
        break;
    }

    logStream() << errInfo << bsl::endl;

    if (d_errorInfo) {
        d_errorInfo->setError(errInfo);
    }
    return d_fatalError ? -1 : 0;
}

// MANIPULATORS
int
Decoder::parse(Decoder_ElementContext *context)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    // according to protocol parse method is allowed to call only from
    // Decoder_ElementContext::parseSubElement method.  That means the current
    // node is ELEMENT.

    BSLS_ASSERT(d_reader->nodeType() ==
                   Reader::e_NODE_TYPE_ELEMENT);

    int numAttr = d_reader->numAttributes();

    if (d_reader->isEmptyElement() && 0 == numAttr) {
        return BAEXML_SUCCESS; // According to Shezan's advice        // RETURN
    }

    // Auto Decrement on enter and Increment on exit
    AutoDecInc<int>  depth (d_remainingDepth);
    int ret = BAEXML_SUCCESS;

    if (0 >= d_remainingDepth) {
        BALXML_DECODER_LOG_ERROR(this)
            << "Reached maximum depth allowed."
            << BALXML_DECODER_LOG_END;

        return BAEXML_FAILURE;                                        // RETURN
    }

    if (0 != context->startElement(this)) {
        BALXML_DECODER_LOG_ERROR(this)
            << "Unable to start element."
            << BALXML_DECODER_LOG_END;

        ret = BAEXML_FAILURE;
    }

    for (int i = 0; i < numAttr; ++i) {

        ElementAttribute attr;
        d_reader->lookupAttribute(&attr, i);

        unsigned int flags = attr.flags();

        if ((flags & ElementAttribute::k_ATTR_IS_NSDECL) != 0) {
            continue;
        }

        const char *attrName = attr.localName();
        const char *attrVal = attr.value();
        size_t       lenVal = attrVal ? bsl::strlen(attrVal) : 0;

        if (0 != context->parseAttribute(attrName, attrVal, lenVal, this)) {
            BALXML_DECODER_LOG_ERROR(this)
                    << "Unable to parse attribute '"
                    << attrName << "'."
                    << BALXML_DECODER_LOG_END;

            ret = BAEXML_FAILURE;
        }
    }

    bool flgExit = d_reader->isEmptyElement();

    while (!flgExit && !d_fatalError) {

        int rc1 = d_reader->advanceToNextNode();
        int rc2 = checkForReaderErrors ();

        if (rc1 != 0 || rc2 < 0) {
            d_fatalError = true;
            BALXML_DECODER_LOG_ERROR(this)
                << "End of stream reached before element was done."
                << BALXML_DECODER_LOG_END;

            ret = BAEXML_FAILURE;
            break;
        }

        int nodeType = d_reader->nodeType ();

        switch(nodeType) {
          case Reader::e_NODE_TYPE_TEXT :
          case Reader::e_NODE_TYPE_CDATA :
          case Reader::e_NODE_TYPE_SIGNIFICANT_WHITESPACE:
          case Reader::e_NODE_TYPE_WHITESPACE:
            {
                const char *val = d_reader->nodeValue();
                size_t       len = val ? bsl::strlen(val) : 0;
                if (0 != context->addCharacters(val, len, this)) {
                    BALXML_DECODER_LOG_ERROR(this)
                                   << "Unable to add \""
                                   << val
                                   << "\"."
                                  << BALXML_DECODER_LOG_END;

                    ret = BAEXML_FAILURE;
                }
            }
            break;

          case Reader::e_NODE_TYPE_END_ELEMENT:
            flgExit = true;
            break;

          case Reader::e_NODE_TYPE_ELEMENT:
            {
                const char *name = d_reader->nodeLocalName();

                if (0 != context->parseSubElement(name, this)) {
                    BALXML_DECODER_LOG_ERROR(this)
                                           << "Unable to decode sub-element '"
                                           << name
                                           << "'."
                                           << BALXML_DECODER_LOG_END;

                    d_fatalError = true;
                    ret          = BAEXML_FAILURE;
                }
            }
            break;
          default:
            break;
        }
    }

    if (0 != context->endElement(this)) {
        BALXML_DECODER_LOG_ERROR(this)
            << "Unable to end element."
            << BALXML_DECODER_LOG_END;

        ret = BAEXML_FAILURE;
    }

    return ret;
}

bsl::ostream& Decoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

// ACCESSORS
ErrorInfo::Severity Decoder::errorSeverity() const
{
    if (d_fatalError) {
        return ErrorInfo::e_FATAL_ERROR;                              // RETURN
    }
    if (d_errorCount != 0) {
        return ErrorInfo::e_ERROR;                                    // RETURN
    }
    if (d_warningCount != 0) {
        return ErrorInfo::e_WARNING;                                  // RETURN
    }

    return ErrorInfo::e_NO_ERROR;
}

bslstl::StringRef Decoder::loggedMessages() const
{
    if (d_logStream) {
        return bslstl::StringRef(d_logStream->data(),
                               d_logStream->length());                // RETURN
    }
    return bslstl::StringRef();
}

                       // -----------------------------
                       // class Decoder_NillableContext
                       // -----------------------------

// CREATORS
Decoder_NillableContext::Decoder_NillableContext()
: d_elementContext_p(0)
, d_isNil(true)
{
}

Decoder_NillableContext::~Decoder_NillableContext()
{
    BSLS_ASSERT(d_elementContext_p);
}

// CALLBACKS

int Decoder_NillableContext::startElement(Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    BSLS_ASSERT(d_elementContext_p);
    d_isNil = true;
    return BAEXML_SUCCESS;
}

int Decoder_NillableContext::endElement(Decoder *decoder)
{
    enum { BAEXML_SUCCESS = 0 };

    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        return BAEXML_SUCCESS;                                        // RETURN
    }

    return d_elementContext_p->endElement(decoder);
}

int Decoder_NillableContext::addCharacters(const char   *chars,
                                           unsigned int  length,
                                           Decoder      *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->addCharacters(chars, length, decoder);
}

int Decoder_NillableContext::parseAttribute(const char *name,
                                            const char *value,
                                            size_t      lenValue,
                                            Decoder    *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    enum { BAEXML_SUCCESS = 0 };

    if (!bsl::strcmp("nil", name) && !bsl::strcmp("true", value)) {
        // xsi:nil=true attribute means the object is nil.  Do nothing.

        return BAEXML_SUCCESS;                                        // RETURN
    }

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->parseAttribute(name, value, lenValue, decoder);
}

int Decoder_NillableContext::parseSubElement(const char *elementName,
                                             Decoder    *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->parseSubElement(elementName, decoder);
}

// MANIPULATORS

void Decoder_NillableContext::setElementContext(
                                        Decoder_ElementContext *elementContext)
{
    d_elementContext_p = elementContext;
    BSLS_ASSERT(d_elementContext_p);
}

// ACCESSORS

bool Decoder_NillableContext::isNil() const
{
    BSLS_ASSERT(d_elementContext_p);
    return d_isNil;
}

                    // -----------------------------------
                    // class Decoder_UnknownElementContext
                    // -----------------------------------

// CREATORS

Decoder_UnknownElementContext::Decoder_UnknownElementContext()
{
}

// CALLBACKS

int Decoder_UnknownElementContext::startElement(Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int Decoder_UnknownElementContext::endElement(Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int Decoder_UnknownElementContext::addCharacters(const char *,
                                                 unsigned int,
                                                 Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int Decoder_UnknownElementContext::parseAttribute(const char *,
                                                  const char *,
                                                  size_t,
                                                  Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int Decoder_UnknownElementContext::parseSubElement(const char *,
                                                   Decoder    *decoder)
{
    return beginParse(decoder);
}

                       // ------------------------------
                       // class Decoder_StdStringContext
                       // ------------------------------

Decoder_StdStringContext::Decoder_StdStringContext(bsl::string *object,
                                                   int          formattingMode)
{
    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_DEFAULT:
      case bdlat_FormattingMode::e_TEXT: {
        new (d_utf8Context.buffer()) UTF8Context(object, formattingMode);
        d_context_p = &d_utf8Context.object();
      } break;
      case bdlat_FormattingMode::e_BASE64: {
        new (d_base64Context.buffer()) Base64Context(object, formattingMode);
        d_context_p = &d_base64Context.object();
      } break;
      case bdlat_FormattingMode::e_HEX: {
        new (d_hexContext.buffer()) HexContext(object, formattingMode);
        d_context_p = &d_hexContext.object();
      } break;
      default: {
        BSLS_ASSERT(!"Unsupported formatting mode!");
      } break;
    }
}

Decoder_StdStringContext::~Decoder_StdStringContext()
{
    d_context_p->~Decoder_ElementContext();
}

// CALLBACKS

int Decoder_StdStringContext::startElement(Decoder *decoder)
{
    return d_context_p->startElement(decoder);
}

int Decoder_StdStringContext::endElement(Decoder *decoder)
{
    return d_context_p->endElement(decoder);
}

int Decoder_StdStringContext::addCharacters(const char   *chars,
                                            unsigned int  length,
                                            Decoder      *decoder)
{
    return d_context_p->addCharacters(chars, length, decoder);
}

int Decoder_StdStringContext::parseAttribute(const char *name,
                                             const char *value,
                                             size_t      lenValue,
                                             Decoder    *decoder)
{
    return d_context_p->parseAttribute(name, value, lenValue, decoder);
}

int Decoder_StdStringContext::parseSubElement(const char *elementName,
                                              Decoder    *decoder)
{
    return d_context_p->parseSubElement(elementName, decoder);
}

                     // ----------------------------------
                     // class Decoder_StdVectorCharContext
                     // ----------------------------------

// CREATORS

Decoder_StdVectorCharContext::Decoder_StdVectorCharContext(
                                             bsl::vector<char> *object,
                                             int                formattingMode)
{
    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        new (d_listContext.buffer()) ListContext(object, formattingMode);
        d_context_p = &d_listContext.object();
        return;                                                       // RETURN
    }

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_DEFAULT:
      case bdlat_FormattingMode::e_BASE64: {
        new (d_base64Context.buffer()) Base64Context(object, formattingMode);
        d_context_p = &d_base64Context.object();
      } break;
      case bdlat_FormattingMode::e_HEX: {
        new (d_hexContext.buffer()) HexContext(object, formattingMode);
        d_context_p = &d_hexContext.object();
      } break;
      case bdlat_FormattingMode::e_TEXT: {
        new (d_utf8Context.buffer()) UTF8Context(object, formattingMode);
        d_context_p = &d_utf8Context.object();
      } break;
      default: {
        BSLS_ASSERT(!"Unsupported formatting mode!");
      } break;
    }
}

Decoder_StdVectorCharContext::~Decoder_StdVectorCharContext()
{
    d_context_p->~Decoder_ElementContext();
}

// CALLBACKS

int Decoder_StdVectorCharContext::startElement(Decoder *decoder)
{
    return d_context_p->startElement(decoder);
}

int Decoder_StdVectorCharContext::endElement(Decoder *decoder)
{
    return d_context_p->endElement(decoder);
}

int Decoder_StdVectorCharContext::addCharacters(const char   *chars,
                                                unsigned int  length,
                                                Decoder      *decoder)
{
    return d_context_p->addCharacters(chars, length, decoder);
}

int Decoder_StdVectorCharContext::parseAttribute(const char *name,
                                                 const char *value,
                                                 size_t      lenValue,
                                                 Decoder    *decoder)
{
    return d_context_p->parseAttribute(name, value, lenValue, decoder);
}

int Decoder_StdVectorCharContext::parseSubElement(const char *elementName,
                                                  Decoder    *decoder)
{
    return d_context_p->parseSubElement(elementName, decoder);
}

                         // -------------------------
                         // class Decoder_ParseObject
                         // -------------------------

// MANIPULATORS

int Decoder_ParseObject::executeImp(bsl::vector<char>         *object,
                                    int                        formattingMode,
                                    bdlat_TypeCategory::Array)
{
    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        return executeArrayImp(object,
                               formattingMode,
                               CanBeListOrRepetition());              // RETURN
    }

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_DEFAULT:
      case bdlat_FormattingMode::e_BASE64:
      case bdlat_FormattingMode::e_HEX:
      case bdlat_FormattingMode::e_TEXT: {
        Decoder_StdVectorCharContext stdVectorCharContext(object,
                                                          formattingMode);

        return stdVectorCharContext.beginParse(d_decoder);            // RETURN
      }
      default: {
        return executeArrayRepetitionImp(object, formattingMode);     // RETURN
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
