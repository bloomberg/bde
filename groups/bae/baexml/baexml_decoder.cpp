// baexml_decoder.cpp              -*-C++-*-
#include <baexml_decoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_decoder_cpp,"$Id$ $CSID$")

#include <baexml_elementattribute.h>

#ifdef TEST
#include <baexml_minireader.h>      // for testing purposes only
#include <baexml_schemaparser.h>    // for testing purposes only
#endif

#include <bslalg_typetraits.h>

#include <bdeu_string.h>
#include <bdeut_stringref.h>

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
// 'baexml_Decoder_SelectContext' meta-function returns an appropriate context
// type to handle the specified 'TYPE'.  All the contexts implement the
// 'baexml_Decoder_ElementContext' protocol.  When the 'Parser' class detects
// certain parsing events, it uses the 'baexml_Decoder_ElementContext'
// protocol to allow the context of each type to handle the event.
//
// The following contexts are defined in this component:
//..
//  Context Name                       Comments
//  ------------                       --------
//  baexml_Decoder_ChoiceContext          Used for 'TYPE's that fall under
//                                     'bdeat_TypeCategory::Choice'.
//
//  baexml_Decoder_PushParserContext      Used for 'TYPE's that can be parsed
//                                     using one of the push parsers (i.e.,
//                                     'baexml_Base64Parser',
//                                     'baexml_HexParser', and
//                                     'baexml_Decoder_ListParser').
//
//  baexml_Decoder_SequenceContext        Used for 'TYPE's that fall under
//                                     'bdeat_TypeCategory::Sequence'.
//
//  baexml_Decoder_SimpleContext          Used for 'TYPE's that can be parsed
//                                     using 'baexml_TypesParserUtil'.
//
//  baexml_Decoder_UTF8Context            Used for 'bsl::string' and
//                                     'bsl::vector<char>' types when the
//                                     formatting mode is
//                                     'bdeat_FormattingMode::BDEAT_TEXT'.
//                                     Note that this is just an optimization
//                                     of 'baexml_Decoder_SimpleContext' where
//                                     the characters are copied directly into
//                                     the destination object, instead of a
//                                     temporary 'd_chars' member.
//
//  baexml_Decoder_StdStringContext       Used for 'bsl::string'.  This context
//                                     simply delegates the callbacks to
//                                     another context.  The delegated context
//                                     is selected using the formatting mode.
//
//  baexml_Decoder_StdVectorCharContext   Used for 'bsl::vector<char>'.  This
//                                     context simply delegates the callbacks
//                                     to another context.  The delegated
//                                     context is selected using the formatting
//                                     mode.
//..
// The main parsing logic happens in the 'Parser' class (defined in the CPP
// file).  It uses Xerces to parse the input stream and can be used
// recursively - i.e., the 'Parser' class calls the 'parseSubElement' function
// in the 'baexml_Decoder_ElementContext' protocol and the implementation of
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

                      // -----------------------------------
                      // class baexml_Decoder_ElementContext
                      // -----------------------------------

baexml_Decoder_ElementContext::~baexml_Decoder_ElementContext()
{
}

int
baexml_Decoder_ElementContext::beginParse(baexml_Decoder *decoder)
{
    return decoder->parse(this);
}

                   // -------------------------------------
                   // class baexml_BerDecoder::MemOutStream
                   // -------------------------------------

baexml_Decoder::MemOutStream::~MemOutStream()
{
}

                      // --------------------
                      // class baexml_Decoder
                      // --------------------

// CREATORS
baexml_Decoder::baexml_Decoder(
                        const baexml_DecoderOptions *options,
                        baexml_Reader               *reader,
                        baexml_ErrorInfo            *errInfo,
                        bslma_Allocator             *basicAllocator)
: d_options       (options)
, d_reader        (reader)
, d_errorInfo     (errInfo)
, d_allocator     (bslma_Default::allocator(basicAllocator))
, d_logStream     (0)
, d_errorStream   (0)
, d_warningStream (0)
, d_sourceUri     (d_allocator)
, d_errorCount    (0)
, d_warningCount  (0)
, d_fatalError    (false)
, d_remainingDepth(1)
{
    BSLS_ASSERT(d_options != 0);
    BSLS_ASSERT(d_reader != 0);
}

baexml_Decoder::baexml_Decoder(
                        const baexml_DecoderOptions *options,
                        baexml_Reader               *reader,
                        baexml_ErrorInfo            *errInfo,
                        bsl::ostream                *errorStream,
                        bsl::ostream                *warningStream,
                        bslma_Allocator             *basicAllocator)
: d_options       (options)
, d_reader        (reader)
, d_errorInfo     (errInfo)
, d_allocator     (bslma_Default::allocator(basicAllocator))
, d_logStream     (0)
, d_errorStream   (errorStream)
, d_warningStream (warningStream)
, d_sourceUri     (d_allocator)
, d_errorCount    (0)
, d_warningCount  (0)
, d_fatalError    (false)
, d_remainingDepth(1)
{
    BSLS_ASSERT(d_options != 0);
    BSLS_ASSERT(d_reader != 0);
}

baexml_Decoder::~baexml_Decoder()
{
    this->close();
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

// PRIVATE MANIPULATORS
void
baexml_Decoder::resetErrors()
{
    d_errorCount = 0;
    d_warningCount = 0;
    d_fatalError = 0;

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
baexml_Decoder::close()
{
    d_reader->close();
}

int
baexml_Decoder::open(bsl::streambuf *buffer, const char *uri)
{
    this->resetErrors();

    uri = uri ? uri : "STREAM.xml";
    d_sourceUri = uri;
    if (d_reader->open(buffer, uri) != 0) {
        BAEXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input stream."
                     << BAEXML_DECODER_LOG_END;
        return -1;
    }
    return readTopElement();
}

int
baexml_Decoder::open(const char *buffer, size_t length, const char *uri)
{
    this->resetErrors();

    uri = uri ? uri : "STREAM.xml";
    d_sourceUri = uri;
    if (d_reader->open(buffer, length, uri) != 0) {
        BAEXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input string."
                     << BAEXML_DECODER_LOG_END;
        return -1;
    }
    return readTopElement();
}

int
baexml_Decoder::open(const char *filename)
{
    this->resetErrors();

    d_sourceUri = filename;
    if (d_reader->open(filename) != 0) {
        BAEXML_DECODER_LOG_ERROR(this)
                     << "Unable to open reader for input file."
                     << BAEXML_DECODER_LOG_END;
        return -1;
    }
    return readTopElement();
}

int
baexml_Decoder::readTopElement()
{
    // read till we find first ELEMENT node and there are no errors
    do {
        int rc1 = d_reader->advanceToNextNode();
        int rc2 = checkForReaderErrors();

        if (rc1 != 0 || rc2 < 0) {
            d_fatalError = true;
            BAEXML_DECODER_LOG_ERROR(this)
                     << "Fatal error while looking for root-level element."
                     << BAEXML_DECODER_LOG_END;

            return -1;
        }

    } while (d_reader->nodeType() != baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);

    return 0;
}

void
baexml_Decoder::setDecoderError(baexml_ErrorInfo::Severity  severity,
                                bdeut_StringRef             msg)
{
    baexml_ErrorInfo errInfo;
    errInfo.setError(severity,
                     d_reader->getLineNumber(),
                     d_reader->getColumnNumber(),
                     d_sourceUri,
                     msg);

    checkForErrors(errInfo);
}

int
baexml_Decoder::checkForReaderErrors ()
{
    return checkForErrors(d_reader->errorInfo());
}

int
baexml_Decoder::checkForErrors(const baexml_ErrorInfo& errInfo)
{
    switch (errInfo.severity()) {
      case baexml_ErrorInfo::BAEXML_NO_ERROR:
        return 0;

      case baexml_ErrorInfo::BAEXML_WARNING:
        ++d_warningCount;
        break;

      case baexml_ErrorInfo::BAEXML_ERROR:
      case baexml_ErrorInfo::BAEXML_FATAL_ERROR:
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
baexml_Decoder::parse(baexml_Decoder_ElementContext *elementContext)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    // according to protocol parse method is allowed to call only
    // from baexml_Decoder_ElementContext::parseSubElement method.
    // That means the current node is ELEMENT .
    //
    BSLS_ASSERT(d_reader->nodeType() ==
                   baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);

    int numAttr = d_reader->numAttributes();

    if (d_reader->isEmptyElement() && 0 == numAttr) {
        return BAEXML_SUCCESS; // According to Shezan's advice
    }

    // Auto Decrement on enter and Increment on exit
    AutoDecInc<int>  depth (d_remainingDepth);
    int ret = BAEXML_SUCCESS;

    if (0 >= d_remainingDepth) {
        BAEXML_DECODER_LOG_ERROR(this)
            << "Reached maximum depth allowed."
            << BAEXML_DECODER_LOG_END;

        return BAEXML_FAILURE;
    }

    if (0 != elementContext->startElement(this)) {
        BAEXML_DECODER_LOG_ERROR(this)
            << "Unable to start element."
            << BAEXML_DECODER_LOG_END;

        ret = BAEXML_FAILURE;
    }

    for (int i = 0; i < numAttr; ++i) {

        baexml_ElementAttribute attr;
        d_reader->lookupAttribute(&attr, i);

        unsigned int flags = attr.flags();

        if ((flags & baexml_ElementAttribute::BAEXML_ATTR_IS_NSDECL) != 0) {
            continue;
        }

        const char *attrName = attr.localName();
        const char *attrVal = attr.value();
        size_t       lenVal = attrVal ? bsl::strlen(attrVal) : 0;

        if (0 != elementContext->parseAttribute(attrName,
                                                attrVal,
                                                lenVal,
                                                this)) {
            BAEXML_DECODER_LOG_ERROR(this)
                    << "Unable to parse attribute '"
                    << attrName << "'."
                    << BAEXML_DECODER_LOG_END;

            ret = BAEXML_FAILURE;
        }
    }

    bool flgExit = d_reader->isEmptyElement();

    while (!flgExit && !d_fatalError) {

        int rc1 = d_reader->advanceToNextNode();
        int rc2 = checkForReaderErrors ();

        if (rc1 != 0 || rc2 < 0) {
            d_fatalError = true;
            BAEXML_DECODER_LOG_ERROR(this)
                << "End of stream reached before element was done."
                << BAEXML_DECODER_LOG_END;

            ret = BAEXML_FAILURE;
            break;
        }

        int nodeType = d_reader->nodeType ();

        switch(nodeType) {
          case baexml_Reader::BAEXML_NODE_TYPE_TEXT :
          case baexml_Reader::BAEXML_NODE_TYPE_CDATA :
          case baexml_Reader::BAEXML_NODE_TYPE_SIGNIFICANT_WHITESPACE:
          case baexml_Reader::BAEXML_NODE_TYPE_WHITESPACE:
            {
                const char *val = d_reader->nodeValue();
                size_t       len = val ? bsl::strlen(val) : 0;
                if (0 != elementContext->addCharacters(val,
                                                       len,
                                                       this)) {
                    BAEXML_DECODER_LOG_ERROR(this)
                                   << "Unable to add \""
                                   << val
                                   << "\"."
                                  << BAEXML_DECODER_LOG_END;

                    ret = BAEXML_FAILURE;
                }
            }
            break;

          case baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT:
            flgExit = true;
            break;

          case baexml_Reader::BAEXML_NODE_TYPE_ELEMENT:
            {
                const char *name = d_reader->nodeLocalName();

                if (0 != elementContext->parseSubElement(name,
                                                         this)) {
                    BAEXML_DECODER_LOG_ERROR(this)
                                           << "Unable to decode sub-element '"
                                           << name
                                           << "'."
                                           << BAEXML_DECODER_LOG_END;

                    d_fatalError = true;
                    ret          = BAEXML_FAILURE;
                }
            }
            break;
          default:
            break;
        }
    }

    if (0 != elementContext->endElement(this)) {
        BAEXML_DECODER_LOG_ERROR(this)
            << "Unable to end element."
            << BAEXML_DECODER_LOG_END;

        ret = BAEXML_FAILURE;
    }

    return ret;
}

bsl::ostream& baexml_Decoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

// ACCESSORS
baexml_ErrorInfo::Severity baexml_Decoder::errorSeverity() const
{
    if (d_fatalError) {
        return baexml_ErrorInfo::BAEXML_FATAL_ERROR;
    }
    if (d_errorCount != 0) {
        return baexml_ErrorInfo::BAEXML_ERROR;
    }
    if (d_warningCount != 0) {
        return baexml_ErrorInfo::BAEXML_WARNING;
    }

    return baexml_ErrorInfo::BAEXML_NO_ERROR;
}

bdeut_StringRef baexml_Decoder::loggedMessages() const
{
    if (d_logStream) {
        return bdeut_StringRef(d_logStream->data(),
                               d_logStream->length());
    }
    return bdeut_StringRef();
}

                     // ------------------------------------
                     // class baexml_Decoder_NillableContext
                     // ------------------------------------

// CREATORS
baexml_Decoder_NillableContext::baexml_Decoder_NillableContext()
: d_elementContext_p(0)
, d_isNil(true)
{
}

baexml_Decoder_NillableContext::~baexml_Decoder_NillableContext()
{
    BSLS_ASSERT(d_elementContext_p);
}

// CALLBACKS

int baexml_Decoder_NillableContext::startElement(baexml_Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    BSLS_ASSERT(d_elementContext_p);
    d_isNil = true;
    return BAEXML_SUCCESS;
}

int baexml_Decoder_NillableContext::endElement(
                                          baexml_Decoder *decoder)
{
    enum { BAEXML_SUCCESS = 0 };

    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        return BAEXML_SUCCESS;
    }

    return d_elementContext_p->endElement(decoder);
}

int baexml_Decoder_NillableContext::addCharacters(const char *chars,
                                                 unsigned int length,
                                              baexml_Decoder *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->addCharacters(chars, length, decoder);
}

int baexml_Decoder_NillableContext::parseAttribute(const char *name,
                                                   const char *value,
                                                  size_t       lenValue,
                                               baexml_Decoder *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->parseAttribute(name, value, lenValue, decoder);
}

int baexml_Decoder_NillableContext::parseSubElement(
                                       const char     *elementName,
                                       baexml_Decoder *decoder)
{
    BSLS_ASSERT(d_elementContext_p);

    if (d_isNil) {
        d_elementContext_p->startElement(decoder);
        d_isNil = false;
    }

    return d_elementContext_p->parseSubElement(elementName,
                                               decoder);
}

// MANIPULATORS

void baexml_Decoder_NillableContext::setElementContext(
                                 baexml_Decoder_ElementContext *elementContext)
{
    d_elementContext_p = elementContext;
    BSLS_ASSERT(d_elementContext_p);
}

// ACCESSORS

bool baexml_Decoder_NillableContext::isNil() const
{
    BSLS_ASSERT(d_elementContext_p);
    return d_isNil;
}

                  // ---------------------------------------
                  // class baexml_Decoder_UnknownElementContext
                  // ---------------------------------------

// CREATORS

baexml_Decoder_UnknownElementContext::baexml_Decoder_UnknownElementContext()
{
}

// CALLBACKS

int baexml_Decoder_UnknownElementContext::startElement(baexml_Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int baexml_Decoder_UnknownElementContext::endElement(baexml_Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int baexml_Decoder_UnknownElementContext::addCharacters(const char *,
                                                        unsigned int ,
                                                        baexml_Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int baexml_Decoder_UnknownElementContext::parseAttribute(const char *,
                                                         const char *,
                                                         size_t    ,
                                                         baexml_Decoder *)
{
    enum { BAEXML_SUCCESS = 0 };

    return BAEXML_SUCCESS;
}

int baexml_Decoder_UnknownElementContext::parseSubElement(
                                                      const char     *,
                                                      baexml_Decoder *decoder)
{
    return beginParse(decoder);
}

                     // ----------------------------------
                     // class baexml_Decoder_StdStringContext
                     // ----------------------------------

baexml_Decoder_StdStringContext::baexml_Decoder_StdStringContext(
                                                   bsl::string *object,
                                                   int          formattingMode)
{
    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_DEFAULT:
      case bdeat_FormattingMode::BDEAT_TEXT: {
        new (d_utf8Context.buffer()) UTF8Context(object, formattingMode);
        d_context_p = &d_utf8Context.object();
      } break;
      case bdeat_FormattingMode::BDEAT_BASE64: {
        new (d_base64Context.buffer()) Base64Context(object, formattingMode);
        d_context_p = &d_base64Context.object();
      } break;
      case bdeat_FormattingMode::BDEAT_HEX: {
        new (d_hexContext.buffer()) HexContext(object, formattingMode);
        d_context_p = &d_hexContext.object();
      } break;
      default: {
        BSLS_ASSERT(!"Unsupported formatting mode!");
      } break;
    }
}

baexml_Decoder_StdStringContext::~baexml_Decoder_StdStringContext()
{
    d_context_p->~baexml_Decoder_ElementContext();
}

// CALLBACKS

int baexml_Decoder_StdStringContext::startElement(
                                          baexml_Decoder *decoder)
{
    return d_context_p->startElement(decoder);
}

int baexml_Decoder_StdStringContext::endElement(
                                          baexml_Decoder *decoder)
{
    return d_context_p->endElement(decoder);
}

int baexml_Decoder_StdStringContext::addCharacters(const char *chars,
                                          unsigned int length,
                                          baexml_Decoder *decoder)
{
    return d_context_p->addCharacters(chars, length, decoder);
}

int baexml_Decoder_StdStringContext::parseAttribute(const char *name,
                                                 const char *value,
                                                 size_t    lenValue,
                                      baexml_Decoder *decoder)
{
    return d_context_p->parseAttribute(name, value, lenValue, decoder);
}

int baexml_Decoder_StdStringContext::parseSubElement(
                                       const char *          elementName,
                                       baexml_Decoder *decoder)
{
    return d_context_p->parseSubElement(elementName, decoder);
}

                   // --------------------------------------
                   // class baexml_Decoder_StdVectorCharContext
                   // --------------------------------------

// CREATORS

baexml_Decoder_StdVectorCharContext::baexml_Decoder_StdVectorCharContext(
                                             bsl::vector<char> *object,
                                             int                formattingMode)
{
    if (formattingMode & bdeat_FormattingMode::BDEAT_LIST) {
        new (d_listContext.buffer()) ListContext(object, formattingMode);
        d_context_p = &d_listContext.object();
        return;
    }

    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_DEFAULT:
      case bdeat_FormattingMode::BDEAT_BASE64: {
        new (d_base64Context.buffer()) Base64Context(object, formattingMode);
        d_context_p = &d_base64Context.object();
      } break;
      case bdeat_FormattingMode::BDEAT_HEX: {
        new (d_hexContext.buffer()) HexContext(object, formattingMode);
        d_context_p = &d_hexContext.object();
      } break;
      case bdeat_FormattingMode::BDEAT_TEXT: {
        new (d_utf8Context.buffer()) UTF8Context(object, formattingMode);
        d_context_p = &d_utf8Context.object();
      } break;
      default: {
        BSLS_ASSERT(!"Unsupported formatting mode!");
      } break;
    }
}

baexml_Decoder_StdVectorCharContext::~baexml_Decoder_StdVectorCharContext()
{
    d_context_p->~baexml_Decoder_ElementContext();
}

// CALLBACKS

int baexml_Decoder_StdVectorCharContext::startElement(
                                          baexml_Decoder *decoder)
{
    return d_context_p->startElement(decoder);
}

int baexml_Decoder_StdVectorCharContext::endElement(
                                          baexml_Decoder *decoder)
{
    return d_context_p->endElement(decoder);
}

int baexml_Decoder_StdVectorCharContext::addCharacters(const char *chars,
                                          unsigned int length,
                                          baexml_Decoder *decoder)
{
    return d_context_p->addCharacters(chars, length, decoder);
}

int baexml_Decoder_StdVectorCharContext::parseAttribute(const char *name,
                                                    const char *value,
                                                    size_t    lenValue,
                                      baexml_Decoder *decoder)
{
    return d_context_p->parseAttribute(name, value, lenValue, decoder);
}

int baexml_Decoder_StdVectorCharContext::parseSubElement(
                                        const char *         elementName,
                                        baexml_Decoder *decoder)
{
    return d_context_p->parseSubElement(elementName, decoder);
}

                       // -----------------------------
                       // class baexml_Decoder_ParseObject
                       // -----------------------------

// MANIPULATORS

int baexml_Decoder_ParseObject::executeImp(bsl::vector<char> *object,
                                        int                formattingMode,
                                        bdeat_TypeCategory::Array)
{
    if (formattingMode & bdeat_FormattingMode::BDEAT_LIST) {
        return executeArrayImp(object,
                               formattingMode,
                               CanBeListOrRepetition());
    }

    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_DEFAULT:
      case bdeat_FormattingMode::BDEAT_BASE64:
      case bdeat_FormattingMode::BDEAT_HEX:
      case bdeat_FormattingMode::BDEAT_TEXT: {
        baexml_Decoder_StdVectorCharContext stdVectorCharContext(object,
                                                              formattingMode);

        return stdVectorCharContext.beginParse(d_decoder);
      }
      default: {
        return executeArrayRepetitionImp(object, formattingMode);
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
