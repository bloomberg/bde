// balxml_decoder.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_DECODER
#define INCLUDED_BALXML_DECODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generic translation from XML into C++ objects.
//
//@CLASSES:
//   balxml::Decoder: an XML decoder
//
//@SEE_ALSO: balxml_decoderoptions, balxml_encoder, balber_berdecoder
//
//@DESCRIPTION: This component provides a class 'balxml::Decoder' for decoding
// value-semantic objects in XML format.  The 'decode' methods are function
// templates that will decode any object that meets the requirements of a
// sequence or choice object as defined in the 'bdlat_sequencefunctions' and
// 'bdlat_choicefunctions' components.  These generic frameworks provide a
// common compile-time interface for manipulating struct-like and union-like
// objects.
//
// There are two usage models for using 'balxml::Decoder'.  The common case,
// when the type of object being decoded is known in advance, involves calling
// one of a set of 'decode' method templates that decode a specified
// value-semantic object from a specified stream or other input source.  The
// caller may specify the input for 'decode' as a file, an 'bsl::istream', an
// 'bsl::streambuf', or a memory buffer.
//
// A less common but more flexible usage model involves calling the 'open' to
// open the XML document from the specified input, then calling 'decode' to
// decode to an object without specifying the input source, and finally
// calling 'close' to close the input source.  The 'open' method positions the
// internal reader to the root element node, so the caller can examine the
// root element, decide what type of object is contained in the input
// stream/source, and construct an object of the needed type before calling
// 'decode' to read from the already open input source.  Thus the input data
// is not constrained to a single root element type.
//
// Although the XML format is very useful for debugging and for conforming to
// external data-interchange specifications, it is relatively expensive to
// encode and decode and relatively bulky to transmit.  It is more efficient
// to use a binary encoding (such as BER) if the encoding format is under your
// control.  (See 'balber_berdecoder'.)
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating Code from a Schema
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have the following XML schema inside a file called
// 'employee.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Address' type='test:Address'/>
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
//..
// Using the 'bas_codegen.pl' tool, we can generate C++ classes for this
// schema:
//..
//  $ bas_codegen.pl -m msg -p test -E xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// The following function decodes an XML string into a 'test::Employee' object
// and verifies the results:
//..
//  #include <test_employee.h>
//  #include <balxml_decoder.h>
//  #include <balxml_decoderoptions.h>
//  #include <balxml_errorinfo.h>
//  #include <balxml_minireader.h>
//  #include <bsl_sstream.h>
//
//  using namespace BloombergLP;
//
//  int main()
//  {
//      const char INPUT[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
//                           "<Employee>\n"
//                           "    <name>Bob</name>\n"
//                           "    <homeAddress>\n"
//                           "        <street>Some Street</street>\n"
//                           "        <city>Some City</city>\n"
//                           "        <state>Some State</state>\n"
//                           "    </homeAddress>\n"
//                           "    <age>21</age>\n"
//                           "</Employee>\n";
//
//      bsl::stringstream ss(INPUT);
//
//      test::Employee bob;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//
//      decoder.decode(ss, &bob);
//
//      assert(ss);
//      assert("Bob"         == bob.name());
//      assert("Some Street" == bob.homeAddress().street());
//      assert("Some City"   == bob.homeAddress().city());
//      assert("Some State"  == bob.homeAddress().state());
//      assert(21            == bob.age());
//
//      return 0;
//  }
//..
//
///Example 2: Error and Warning Streams
/// - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to pass an error stream and
// warning stream to the 'decode' function.  We will use the same
// 'test_employee' component from the previous usage example.  Note that the
// input XML string contains an error.  (The 'homeAddress' object has an
// element called 'country', which does not exist in the schema.):
//..
//  #include <test_employee.h>
//  #include <balxml_decoder.h>
//  #include <balxml_decoderoptions.h>
//  #include <balxml_errorinfo.h>
//  #include <balxml_minireader.h>
//  #include <bsl_sstream.h>
//
//  using namespace BloombergLP;
//
//  int main()
//  {
//      const char INPUT[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
//                           "<Employee>\n"
//                           "    <name>Bob</name>\n"
//                           "    <homeAddress>\n"
//                           "        <street>Some Street</street>\n"
//                           "        <city>Some City</city>\n"
//                           "        <state>Some State</state>\n"
//                           "        <country>Some Country</country>\n"
//                           "    </homeAddress>\n"
//                           "    <age>21</age>\n"
//                           "</Employee>\n";
//
//      bsl::stringstream ss(INPUT);
//
//      test::Employee bob;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      options.setSkipUnknownElements(false);
//      balxml::Decoder decoder(&options, &reader, &errInfo,
//                              &bsl::cerr, &bsl::cerr);
//      decoder.decode(ss, &bob);
//
//      assert(!ss);
//
//      return 0;
//  }
//..
// Note that the input stream is invalidated to indicate that an error
// occurred.  Also note that the following error message will be printed on
// 'bsl::cerr':
//..
//  employee.xml:8.18: Error: Unable to decode sub-element 'country'.\n"
//  employee.xml:8.18: Error: Unable to decode sub-element 'homeAddress'.\n";
//..
// The following snippets of code illustrate how to open decoder and read the
// first node before calling 'decode':
//..
//  int main()
//  {
//      const char INPUT[] =
//          "<?xml version='1.0' encoding='UTF-8' ?>\n"
//          "<Employee xmlns='http://www.bde.com/bdem_test'>\n"
//          "    <name>Bob</name>\n"
//          "    <homeAddress>\n"
//          "        <street>Some Street</street>\n"
//          "        <state>Some State</state>\n"
//          "        <city>Some City</city>\n"
//          "        <country>Some Country</country>\n"
//          "    </homeAddress>\n"
//          "    <age>21</age>\n"
//          "</Employee>\n";
//
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//      balxml::DecoderOptions options;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo,
//                              &bsl::cerr, &bsl::cerr);
//
//..
// Now we open the document, but we don't begin decoding yet:
//..
//      int rc = decoder.open(INPUT, sizeof(INPUT) - 1);
//      assert(0 == rc);
//..
// Depending on the value of the first node, we can now determine whether the
// document is an 'Address' object or an 'Employee' object, and construct the
// target object accordingly:
//..
//      if (0 == bsl::strcmp(reader.nodeLocalName(), "Address")) {
//          test::Address addr;
//          rc = decoder.decode(&addr);
//          bsl::cout << addr;
//      }
//      else {
//          test::Employee bob;
//          rc = decoder.decode(&bob);
//          bsl::cout << bob;
//      }
//
//      assert(0 == rc);
//..
// When decoding is complete, we must close the decoder object:
//..
//      decoder.close();
//      return 0;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_BASE64PARSER
#include <balxml_base64parser.h>
#endif

#ifndef INCLUDED_BALXML_DECODEROPTIONS
#include <balxml_decoderoptions.h>
#endif

#ifndef INCLUDED_BALXML_HEXPARSER
#include <balxml_hexparser.h>
#endif

#ifndef INCLUDED_BALXML_LISTPARSER
#include <balxml_listparser.h>
#endif

#ifndef INCLUDED_BALXML_TYPESPARSERUTIL
#include <balxml_typesparserutil.h>
#endif

#ifndef INCLUDED_BALXML_ERRORINFO
#include <balxml_errorinfo.h>
#endif

#ifndef INCLUDED_BALXML_READER
#include <balxml_reader.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_STRING
#include <bdlb_string.h>
#endif

#ifndef INCLUDED_BDLSB_MEMOUTSTREAMBUF
#include <bdlsb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>            // bsl::min
#endif

#ifndef INCLUDED_BSL_ISTREAM
#include <bsl_istream.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CERRNO
#include <bsl_cerrno.h>
#endif

namespace BloombergLP {


namespace balxml {class Reader;
class ErrorInfo;
class Decoder;

                        // ============================
                        // class Decoder_ElementContext
                        // ============================

class Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This protocol class contain functions related to parsing XML elements.
    // When the Decoder reads the XML document, it forwards the information
    // about the current node as events to this protocol.  There are several
    // implementations of this protocol, depending on the type of element.  The
    // correct implementation for each type is selected by the
    // 'Decoder_SelectContext' meta-function.  Each of the functions take a
    // 'context' parameter, which contains members related to the context of
    // the decoder.

  public:
    virtual ~Decoder_ElementContext();
        // For syntactic purposes only.

    // CALLBACKS
    virtual int startElement(Decoder *decoder) = 0;

    virtual int endElement(Decoder *decoder) = 0;

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder) = 0;

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder) = 0;

    virtual int parseSubElement(const char *elementName, Decoder *decoder) = 0;

    int beginParse(Decoder *decoder);
};

                               // =============
                               // class Decoder
                               // =============

class Decoder {
    // Engine for decoding value-semantic objects in XML format.  The 'decode'
    // methods are function templates that will decode any object that meets
    // the requirements of a sequence or choice object as defined in the
    // 'bdlat_sequencefunctions' and 'bdlat_choicefunctions' components.
    // These generic frameworks provide a common compile-time interface for
    // manipulating struct-like and union-like objects.

    friend class  Decoder_ElementContext;
    friend struct Decoder_decodeImpProxy;
    friend class  Decoder_ErrorLogger;

    // PRIVATE TYPES
    class MemOutStream : public bsl::ostream {
        // This class provides stream for logging using
        // 'bdlsb::MemOutStreamBuf' as a streambuf.  The logging stream is
        // created on demand, i.e., during the first attempt to log message.
        bdlsb::MemOutStreamBuf d_sb;

        // NOT IMPLEMENTED
        MemOutStream(const MemOutStream&);
        MemOutStream& operator=(const MemOutStream&);

      public:
        // CREATORS
        MemOutStream(bslma::Allocator *basicAllocator = 0);
            // Create a new stream using the optionally specified
            // 'basicAllocator'.

        virtual ~MemOutStream();
            // Destroy this stream and release memory back to the allocator.

        // MANIPULATORS
        void reset();
            // Reset the internal streambuf to empty.

        // ACCESSORS
        const char *data() const;
            // Return a pointer to the memory containing the formatted values
            // formatted to this stream.  The data is not null-terminated
            // unless a null character was appended onto this stream.

        int length() const;
            // Return the length of of the formatted data, including null
            // characters appended to the stream, if any.
    };

    // DATA
    const DecoderOptions     *d_options;        // held, not owned
    Reader                   *d_reader;         // held, not owned
    ErrorInfo                *d_errorInfo;      // held, not owned

    bslma::Allocator                *d_allocator;      // held, not owned

    bsls::ObjectBuffer<MemOutStream> d_logArea;
        // placeholder for MemOutStream

    MemOutStream                    *d_logStream;
        // if not zero, log stream was created at the moment of first logging
        // and must be destroyed

    bsl::ostream                    *d_errorStream;    // held, not owned
    bsl::ostream                    *d_warningStream;  // held, not owned

    bsl::string                      d_sourceUri;      // URI of input document
    int                              d_errorCount;     // error count
    int                              d_warningCount;   // warning count

    int                              d_numUnknownElementsSkipped;
                                                       // number of unknown
                                                       // elements skipped

    bool                             d_fatalError;     // fatal error flag
    int                              d_remainingDepth;
        // remaining number of nesting levels allowed

    // NOT IMPLEMENTED
    Decoder(const Decoder&);
    Decoder operator=(const Decoder&);

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not been
        // created yet, it will be created during this call.

    void resetErrors();
    int  checkForReaderErrors();
    int  checkForErrors(const ErrorInfo& errInfo);

    void setDecoderError(ErrorInfo::Severity severity, bslstl::StringRef msg);

    int  readTopElement();
    int  parse(Decoder_ElementContext *context);

    template <class TYPE>
    int decodeImp(TYPE *object, bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    int decodeImp(TYPE *object, ANY_CATEGORY);

  public:
    // CREATORS
    Decoder(const DecoderOptions *options,
            Reader               *reader,
            ErrorInfo            *errInfo,
            bslma::Allocator     *basicAllocator);

    Decoder(const DecoderOptions *options,
            Reader               *reader,
            ErrorInfo            *errInfo = 0,
            bsl::ostream         *errorStream = 0,
            bsl::ostream         *warningStream = 0,
            bslma::Allocator     *basicAllocator = 0);
        // Construct a decoder object using the specified 'options' and the
        // specified 'reader' to perform the XML-level parsing.  If the
        // (optionally) specified 'errorInfo' is non-null, it is used to store
        // information about most serious error encountered during parsing.
        // During parsing, error and warning messages will be written to the
        // (optionally) specified 'errorStream' and 'warningStream'
        // respectively.  The behavior is undefined unless 'options' and
        // 'reader' are both non-zero.  The behavior becomes undefined if the
        // objects pointed to by any of the arguments is destroyed before this
        // object has completed parsing.

    ~Decoder();
        // Call 'close' and destroy this object.

    // MANIPULATORS
    void close();
        // Put the associated 'Reader' object (i.e., the 'reader' specified at
        // construction) into a closed state.

    int open(bsl::istream& stream, const char *uri = 0);
        // Open the associated 'Reader' object (see 'Reader::open') to read XML
        // data from the specified 'stream'.  The (optionally) specified 'uri'
        // is used for identifying the input document in error messages.
        // Return 0 on success and non-zero otherwise.

    int open(bsl::streambuf *buffer, const char *uri = 0);
        // Open the associated 'Reader' object (see 'Reader::open') to read XML
        // data from the specified stream 'buffer'.  The (optionally) specified
        // 'uri' is used for identifying the input document in error messages.
        // Return 0 on success and non-zero otherwise.

    int open(const char *buffer, bsl::size_t length, const char *uri = 0);
        // Open the associated 'Reader' object (see 'Reader::open') to read XML
        // data from memory at the specified 'buffer', with the specified
        // 'length'.  The (optionally) specified 'uri' is used for identifying
        // the input document in error messages.  Return 0 on success and
        // non-zero otherwise.

    int open(const char *filename);
        // Open the associated 'Reader' object (see 'Reader::open') to read XML
        // data from the file with the specified 'filename'.  Return 0 on
        // success and non-zero otherwise.

    template <class TYPE>
    bsl::istream& decode(bsl::istream&  stream,
                         TYPE          *object,
                         const char    *uri = 0);
        // Decode the specified 'object' of parameterized 'TYPE' from the
        // specified input 'stream'.  Return a reference to the modifiable
        // 'stream'.  If a decoding error is detected, 'stream.fail()' will be
        // 'true' after this method returns.  The (optionally) specified 'uri'
        // is used for identifying the input document in error messages.  A
        // compilation error will result unless 'TYPE' conforms to the
        // requirements of a 'bdeat' sequence or choice, as described in
        // 'bdlat_sequencefunctions' and 'bdlat_choicefunctions'.

    template <class TYPE>
    int decode(bsl::streambuf *buffer, TYPE *object, const char *uri = 0);
        // Decode the specified 'object' of parameterized 'TYPE' from the
        // specified stream 'buffer'.  The (optionally) specified 'uri' is
        // used for identifying the input document in error messages.  Return
        // 0 on success, and a non-zero value otherwise.  A compilation error
        // will result unless 'TYPE' conforms to the requirements of a bdeat
        // sequence or choice, as described in 'bdlat_sequencefunctions' and
        // 'bdlat_choicefunctions'.

    template <class TYPE>
    int decode(const char  *buffer,
               bsl::size_t  length,
               TYPE        *object,
               const char  *uri = 0);
        // Decode the specified 'object' of parameterized 'TYPE' from the
        // memory at the specified 'buffer' address, having the specified
        // 'length'.  The (optionally) specified 'uri' is used for identifying
        // the input document in error messages.  Return 0 on success, and a
        // non-zero value otherwise.  A compilation error will result unless
        // 'TYPE' conforms to the requirements of a bdeat sequence or choice,
        // as described in 'bdlat_sequencefunctions' and
        // 'bdlat_choicefunctions'.

    template <class TYPE>
    int decode(const char *filename, TYPE *object);
        // Decode the specified 'object' of parameterized 'TYPE' from the file
        // with the specified 'filename'.  Return 0 on success, and a non-zero
        // value otherwise.  A compilation error will result unless 'TYPE'
        // conforms to the requirements of a bdeat sequence or choice, as
        // described in 'bdlat_sequencefunctions' and 'bdlat_choicefunctions'.

    template <class TYPE>
    int decode(TYPE *object);
        // Decode the specified 'object' of parameterized 'TYPE' from the
        // input source specified by a previous call to 'open' and leave the
        // reader in an open state.  Return 0 on success, and a non-zero value
        // otherwise.  A compilation error will result unless 'TYPE' conforms
        // to the requirements of a bdeat sequence or choice, as described in
        // 'bdlat_sequencefunctions' and 'bdlat_choicefunctions'.  The
        // behavior is undefined unless this call was preceded by a prior
        // successful call to 'open'

    void setNumUnknownElementsSkipped(int value);
        // Set the number of unknown elements skipped by the decoder during
        // the current decoding operation to the specified 'value'.  The
        // behavior is undefined unless '0 <= value'.

    //ACCESSORS
    const DecoderOptions *options() const;
        // Return a pointer to the non-modifiable decoder options provided at
        // construction.

    Reader *reader() const;
        // Return the a pointer to the modifiable reader associated with this
        // decoder (i.e., the 'reader' pointer provided at construction).

    ErrorInfo *errorInfo() const;
        // Return a pointer to the modifiable error-reporting structure
        // associated with this decoder (i.e., the 'errInfo' pointer provided
        // at construction).  The value stored in the error structure is reset
        // to indicate no error on a successful call to 'open'.

    bsl::ostream *errorStream() const;
        // Return pointer to the error stream.

    bsl::ostream *warningStream() const;
        // Return pointer to the warning stream.

    int numUnknownElementsSkipped() const;
        // Return the number of unknown elements that were skipped during the
        // previous decoding operation.  Note that unknown elements are skipped
        // only if 'true == options()->skipUnknownElements()'.

    ErrorInfo::Severity  errorSeverity() const;
        // Return the severity of the most severe warning or error encountered
        // during the last call to the 'encode' method.  The severity is reset
        // each time 'decode' is called.

    bslstl::StringRef loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'decode' method.  The
        // log is reset each time 'decode' is called.

    int  errorCount() const;
        // Return the number of errors that occurred during decoding.  This
        // number is reset to zero on a call to 'open'.

    int  warningCount() const;
        // Return the number of warnings that occurred during decoding.  This
        // number is reset to zero on a call to 'open'.
};

                         // =========================
                         // class Decoder_ErrorLogger
                         // =========================

class Decoder_ErrorLogger {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This class is used for logging errors and warnings.  The usage of this
    // class is simplified with macros, which are defined below.

    // DATA
    Decoder::MemOutStream   d_stream;
    ErrorInfo::Severity     d_severity;    // severity
    Decoder                *d_decoder;     // context

  private:
    // NOT IMPLEMENTED
    Decoder_ErrorLogger(const Decoder_ErrorLogger&);
    Decoder_ErrorLogger& operator=(const Decoder_ErrorLogger&);

  public:
    // CREATORS
    Decoder_ErrorLogger(ErrorInfo::Severity severity, Decoder *decoder)
        // Construct a logger for the specified 'decoder'.
    : d_stream(decoder->d_allocator)
    , d_severity(severity)
    , d_decoder(decoder)
    {
    }

    ~Decoder_ErrorLogger()
        // Set the decoder's error message to the contents of the message
        // stream.
    {
        d_decoder->setDecoderError(d_severity,
                                   bslstl::StringRef(d_stream.data(),
                                                     d_stream.length()));
    }

    bsl::ostream& stream()
    {
        return d_stream;
    }
};
}  // close package namespace

// ---  Anything below this line is implementation specific.  Do not use.  ----

// LOGGING MACROS
#define BALXML_DECODER_LOG_ERROR(reporter)                     \
    do {                                                       \
        balxml::Decoder_ErrorLogger                             \
            logger(balxml::ErrorInfo::e_ERROR, reporter);  \
        logger.stream()
    // Usage: BAEXML_LOG_ERROR(myDecoder) << "Message"
    //                                    << value << BALXML_DECODER_LOG_END;

#define BALXML_DECODER_LOG_WARNING(reporter)                   \
    do {                                                       \
        balxml::Decoder_ErrorLogger                             \
           logger(balxml::ErrorInfo::e_WARNING, reporter); \
        logger.stream()
    // Usage: BAEXML_LOG_WARNING(myDecoder) << "Message"
    //                                      << value << BALXML_DECODER_LOG_END;

#define BALXML_DECODER_LOG_END     \
        bsl::flush;                \
    } while (false)
    // See usage of BALXML_DECODER_LOG_ERROR and BALXML_DECODER_LOG_WARNING,
    // above.

// FORWARD DECLARATIONS


namespace balxml {class Decoder_ElementContext;

template <class TYPE>
class Decoder_ChoiceContext;
template <class TYPE>
class Decoder_CustomizedContext;
template <class TYPE, class PARSER>
class Decoder_PushParserContext;
template <class TYPE>
class Decoder_SequenceContext;
template <class TYPE>
class Decoder_SimpleContext;
template <class TYPE>
class Decoder_UTF8Context;

class Decoder_UnknownElementContext;

class Decoder_StdStringContext;      // proxy context
class Decoder_StdVectorCharContext;  // proxy context

                       // ==============================
                       // class Decoder_ListParser<TYPE>
                       // ==============================

template <class TYPE>
class Decoder_ListParser {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This is a wrapper around 'ListParser<TYPE>'.  The
    // 'Decoder_PushParserContext' class needs a default constructible push
    // parser.  However, 'ListParser<TYPE>' is not default constructible - it
    // requires an 'parseElementCallback'.  This wrapper provides a default
    // constructor that passes 'TypesParserUtil::parseDefault' as the callback.

    // PRIVATE TYPES
    typedef typename
    ListParser<TYPE>::ParseElementFunction ParseElementFunction;
    typedef typename
    ListParser<TYPE>::ParseElementCallback ParseElementCallback;

    // DATA
    ListParser<TYPE> d_imp;  // implementation object

  private:
    // NOT IMPLEMENTED
    Decoder_ListParser(const Decoder_ListParser&);
    Decoder_ListParser& operator=(const Decoder_ListParser&);

    // COMPILER BUG WORKAROUNDS
    static ParseElementCallback convert(ParseElementFunction func)
        // This function is provided to work around a bug in the AIX compiler.
        // It incorrectly complains that the following constructor initializer
        // list for 'd_imp' is invalid:
        //..
        // : d_imp((ParseElementFunction)&TypesParserUtil::parseDefault)
        //..
        // The error message generated by the AIX compiler is:
        //..
        // An object of type "BloombergLP::balxml::ListParser<TYPE>" cannot be
        // constructed from an rvalue of type "ParseElementFunction".
        //..
        // To work around this, an explicit 'convert' function is used to aid
        // the conversion.
    {
        ParseElementCallback temp(func);
        return temp;
    }

  public:
    // CREATORS
    Decoder_ListParser()
    : d_imp(convert(&TypesParserUtil::parseDefault))
    {
    }

    // Using destructor generated by compiler:
    //  ~Decoder_ListParser();

    // MANIPULATORS
    int beginParse(TYPE *object)
    {
        return d_imp.beginParse(object);
    }

    int endParse()
    {
        return d_imp.endParse();
    }

    template <class INPUT_ITERATOR>
    int pushCharacters(INPUT_ITERATOR begin, INPUT_ITERATOR end)
    {
        return d_imp.pushCharacters(begin, end);
    }
};
}  // close package namespace

              // ===============================================
              // struct balxml::Decoder_InstantiateContext<TYPE>
              // ===============================================


namespace balxml {template <class CATEGORY, class TYPE>
struct Decoder_InstantiateContext;
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This 'struct' instantiates a context for the parameterized 'TYPE' that
    // falls under the parameterized 'CATEGORY'.

template <class TYPE>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Array, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_PushParserContext<TYPE, Decoder_ListParser<TYPE> > Type;
};

template <>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Array,
                                      bsl::vector<char> >
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_StdVectorCharContext Type;
};

template <class TYPE>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Choice, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_ChoiceContext<TYPE> Type;
};

template <class TYPE>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Sequence, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_SequenceContext<TYPE> Type;
};

template <class TYPE>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Simple, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_SimpleContext<TYPE> Type;
};

template <>
struct
Decoder_InstantiateContext<bdlat_TypeCategory::Simple, bsl::string>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    typedef Decoder_StdStringContext Type;
};

template <class TYPE>
struct
Decoder_InstantiateContext<bdlat_TypeCategory::CustomizedType, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Note: Customized are treated as simple types (i.e., they are parsed by
    // 'TypesParserUtil').
    typedef Decoder_CustomizedContext<TYPE> Type;
};

template <class TYPE>
struct Decoder_InstantiateContext<bdlat_TypeCategory::Enumeration, TYPE>
{
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Note: Enums are treated as simple types (i.e., they are parsed by
    // 'TypesParserUtil').

    typedef Decoder_SimpleContext<TYPE> Type;
};

                     // ==================================
                     // struct Decoder_SelectContext<TYPE>
                     // ==================================

template <class TYPE>
struct Decoder_SelectContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This meta-function is used to select a context for the parameterized
    // 'TYPE'.

  private:
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

  public:
    typedef typename
    Decoder_InstantiateContext<TypeCategory, TYPE>::Type Type;
};

                     // =================================
                     // class Decoder_ChoiceContext<TYPE>
                     // =================================

template <class TYPE>
class Decoder_ChoiceContext :  public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This is the context for types that fall under
    // 'bdlat_TypeCategory::Choice'.

    // DATA
    bool         d_isSelectionNameKnown;
    TYPE        *d_object_p;
    bool         d_selectionIsRepeatable;
    bsl::string  d_selectionName;

    // NOT IMPLEMENTED
    Decoder_ChoiceContext(const Decoder_ChoiceContext&);
    Decoder_ChoiceContext &operator=(const Decoder_ChoiceContext &);

  public:
    // CREATORS
    Decoder_ChoiceContext(TYPE *object, int formattingMode);

    // Using compiler generated destructor:
    //  ~Decoder_ChoiceContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                       // =============================
                       // class Decoder_NillableContext
                       // =============================

class Decoder_NillableContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for elements that have 'bdlat_FormattingMode::e_NILLABLE'.  It
    // acts as a proxy and forwards all callbacks to the held
    // 'd_elementContext_p'.  If 'endElement' is called directly after
    // 'startElement', then the 'isNil()' accessor will return true.

    // DATA
    Decoder_ElementContext *d_elementContext_p;
    bool                    d_isNil;

    // NOT IMPLEMENTED
    Decoder_NillableContext(const Decoder_NillableContext&);
    Decoder_NillableContext &operator=(const Decoder_NillableContext &);

  public:
    // CREATORS
    Decoder_NillableContext();

    ~Decoder_NillableContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);

    // MANIPULATORS
    void setElementContext(Decoder_ElementContext *elementContext);
        // Set the element context to the specified 'elementContext'.  The
        // behavior of all methods in this class are undefined if this method
        // has not been called.

    // ACCESSORS
    bool isNil() const;
        // Return 'true' if the element is nil.
};

             // ====================================================
             // class Decoder_PushParserContext<TYPE, PARSER>
             // ====================================================

template <class TYPE, class PARSER>
class Decoder_PushParserContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for types that use one of the following push parsers:
    //..
    //    o Base64Parser
    //    o HexParser
    //    o Decoder_ListParser
    //..

    // DATA
    int     d_formattingMode;
    TYPE   *d_object_p;
    PARSER  d_parser;

    // NOT IMPLEMENTED
    Decoder_PushParserContext(const Decoder_PushParserContext&);
    Decoder_PushParserContext &operator=(const Decoder_PushParserContext &);

  public:
    // CREATORS
    Decoder_PushParserContext(TYPE *object, int formattingMode);

    // Using compiler generated destructor:
    //  ~Decoder_PushParserContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                    // ===================================
                    // class Decoder_SequenceContext<TYPE>
                    // ===================================

template <class TYPE>
class Decoder_SequenceContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for types that fall under 'bdlat_TypeCategory::Sequence'.

    // DATA
    bdlb::NullableValue<int>  d_simpleContentId;
    TYPE                     *d_object_p;

    // NOT IMPLEMENTED
    Decoder_SequenceContext(const Decoder_SequenceContext&);
    Decoder_SequenceContext &operator=(const Decoder_SequenceContext &);

  public:
    // CREATORS
    Decoder_SequenceContext(TYPE *object, int formattingMode);

    // Using compiler generated destructor:
    //  ~Decoder_SequenceContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                     // =================================
                     // class Decoder_SimpleContext<TYPE>
                     // =================================

template <class TYPE>
class Decoder_SimpleContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for simple types (uses TypesParserUtil).

    // DATA
    //bsl::string  d_chars;
    int            d_formattingMode;
    TYPE          *d_object_p;

    // NOT IMPLEMENTED
    Decoder_SimpleContext(const Decoder_SimpleContext&);
    Decoder_SimpleContext &operator=(const Decoder_SimpleContext &);

  public:
    // CREATORS
    Decoder_SimpleContext(TYPE *object, int formattingMode);

    // Using compiler generated destructor:
    //  ~Decoder_SimpleContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                   // =====================================
                   // class Decoder_CustomizedContext<TYPE>
                   // =====================================

template <class TYPE>
class Decoder_CustomizedContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This is the context for types that fall under
    // 'bdlat_TypeCategory::Customized'.

    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type         BaseType;

    typedef typename Decoder_InstantiateContext<bdlat_TypeCategory::Simple,
                                                BaseType>::Type BaseContext;
    // DATA
    TYPE       *d_object;
    BaseType    d_baseObj;
    BaseContext d_baseContext;

    // NOT IMPLEMENTED
    Decoder_CustomizedContext(const Decoder_CustomizedContext&);
    Decoder_CustomizedContext &operator=(const Decoder_CustomizedContext &);

  public:
    // CREATORS
    Decoder_CustomizedContext(TYPE *object, int formattingMode);

    // Using compiler generated destructor:
    //  ~Decoder_CustomizedContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                    // ===================================
                    // class Decoder_UnknownElementContext
                    // ===================================

class Decoder_UnknownElementContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for unknown elements.  This context is used when an unknown
    // element is found, and the user selected.

  private:
    // NOT IMPLEMENTED
    Decoder_UnknownElementContext(const Decoder_UnknownElementContext &);
    Decoder_UnknownElementContext& operator=(
                                        const Decoder_UnknownElementContext &);

  public:
    // CREATORS
    Decoder_UnknownElementContext();

    // Using compiler generated destructor:
    //  ~Decoder_UnknownElementContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                         // =========================
                         // class Decoder_UTF8Context
                         // =========================

template <class TYPE>
class Decoder_UTF8Context : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Context for UTF8 strings (i.e., 'bsl::string' and 'bsl::vector<char>').

    // DATA
    TYPE *d_object_p;

    // NOT IMPLEMENTED
    Decoder_UTF8Context(const Decoder_UTF8Context&);
    Decoder_UTF8Context& operator=(const Decoder_UTF8Context&);

  public:
    // CREATORS
    Decoder_UTF8Context(TYPE *object, int formattingMode);

    // Generated by compiler:
    //  ~Decoder_UTF8Context();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                       // ==============================
                       // class Decoder_StdStringContext
                       // ==============================

class Decoder_StdStringContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Proxy context for 'bsl::string'.  This is just a proxy context.  It will
    // forward all callbacks to the appropriate context, based on the
    // formatting mode.

  public:
    // TYPES

    // Note that these typedefs need to be made public because Sun compiler
    // complains that they are inaccessible from the union (below).
    typedef Decoder_PushParserContext<bsl::string, Base64Parser<bsl::string> >
                                             Base64Context;
    typedef Decoder_PushParserContext<bsl::string, HexParser<bsl::string> >
                                             HexContext;
    typedef Decoder_UTF8Context<bsl::string> UTF8Context;

  private:
    // DATA
    union {
        bsls::ObjectBuffer<Base64Context> d_base64Context;
        bsls::ObjectBuffer<HexContext>    d_hexContext;
        bsls::ObjectBuffer<UTF8Context>   d_utf8Context;
    };

    Decoder_ElementContext *d_context_p;

    // NOT IMPLEMENTED
    Decoder_StdStringContext(const Decoder_StdStringContext&);
    Decoder_StdStringContext &operator=(const Decoder_StdStringContext &);

  public:
    // CREATORS
    Decoder_StdStringContext(bsl::string *object, int formattingMode);

    virtual ~Decoder_StdStringContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                     // ==================================
                     // class Decoder_StdVectorCharContext
                     // ==================================

class Decoder_StdVectorCharContext : public Decoder_ElementContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Proxy context for 'bsl::string'.  This is just a proxy context.  It will
    // forward all callbacks to the appropriate context, based on the
    // formatting mode.

  public:
    // TYPES

    // Note that these typedefs need to be made public because Sun compiler
    // complains that they are inaccessible from the union (below).
    typedef Decoder_PushParserContext<bsl::vector<char>,
                                      Base64Parser<bsl::vector<char> > >
                                                    Base64Context;
    typedef Decoder_PushParserContext<bsl::vector<char>,
                                      HexParser<bsl::vector<char> > >
                                                    HexContext;
    typedef Decoder_PushParserContext<bsl::vector<char>,
                                      Decoder_ListParser<bsl::vector<char> > >
                                                    ListContext;
    typedef Decoder_UTF8Context<bsl::vector<char> > UTF8Context;

  private:
    // DATA
    union {
        bsls::ObjectBuffer<Base64Context> d_base64Context;
        bsls::ObjectBuffer<HexContext>    d_hexContext;
        bsls::ObjectBuffer<ListContext>   d_listContext;
        bsls::ObjectBuffer<UTF8Context>   d_utf8Context;
    };

    Decoder_ElementContext *d_context_p;

    // NOT IMPLEMENTED
    Decoder_StdVectorCharContext(const Decoder_StdVectorCharContext &);
    Decoder_StdVectorCharContext& operator=(
                                          const Decoder_StdVectorCharContext&);

  public:
    // CREATORS
    Decoder_StdVectorCharContext(bsl::vector<char> *object,
                                 int                formattingMode);

    virtual ~Decoder_StdVectorCharContext();

    // CALLBACKS
    virtual int startElement(Decoder *decoder);

    virtual int endElement(Decoder *decoder);

    virtual int addCharacters(const char   *chars,
                              unsigned int  length,
                              Decoder      *decoder);

    virtual int parseAttribute(const char  *name,
                               const char  *value,
                               bsl::size_t  lenValue,
                               Decoder     *decoder);

    virtual int parseSubElement(const char *elementName, Decoder *decoder);
};

                    // ====================================
                    // class Decoder_PrepareSequenceContext
                    // ====================================

class Decoder_PrepareSequenceContext {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This class does one thing:
    //..
    //  o finds an element that has the 'IS_SIMPLE_CONTENT' flag set
    //..

    // DATA
    bdlb::NullableValue<int> *d_simpleContentId_p; // held, not owned

    // NOT IMPLEMENTED
    Decoder_PrepareSequenceContext(const Decoder_PrepareSequenceContext &);
    Decoder_PrepareSequenceContext& operator=(
                                        const Decoder_PrepareSequenceContext&);

  public:
    // CREATORS
    Decoder_PrepareSequenceContext(bdlb::NullableValue<int> *simpleContentId);

    // Using compiler generated destructor:
    //  ~Decoder_PrepareSequenceContext();

    // MANIPULATORS
    template <class TYPE, class INFO_TYPE>
    int operator()(const TYPE &object, const INFO_TYPE &info);
};

                  // ========================================
                  // class Decoder_ParseSequenceSimpleContent
                  // ========================================

class Decoder_ParseSequenceSimpleContent {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Parse simple content.

    // DATA
    //const bsl::string *d_chars_p;  // content characters
    const char          *d_chars_p;  // content characters
    bsl::size_t          d_len;
    Decoder             *d_decoder;  // error logger (held)

    // NOT IMPLEMENTED
    Decoder_ParseSequenceSimpleContent(
                                    const Decoder_ParseSequenceSimpleContent&);
    Decoder_ParseSequenceSimpleContent& operator=(
                                    const Decoder_ParseSequenceSimpleContent&);

  public:
    // CREATORS
    Decoder_ParseSequenceSimpleContent(Decoder     *decoder,
                                       const char  *chars,
                                       bsl::size_t  len);

    // Generated by compiler:
    //  ~Decoder_ParseSequenceSimpleContent();

    // MANIPULATORS
    template <class TYPE, class INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE& info);

    template <class INFO_TYPE>
    int operator()(bsl::string *object, const INFO_TYPE& info);
};

                   // =====================================
                   // class Decoder_ParseSequenceSubElement
                   // =====================================

class Decoder_ParseSequenceSubElement {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // This is similar to 'Decoder_ParseObject'.

    // DATA
    Decoder     *d_decoder;        // held, not owned
    const char  *d_elementName_p;  // held, not owned
    bsl::size_t  d_lenName;

    // NOT IMPLEMENTED
    Decoder_ParseSequenceSubElement(const Decoder_ParseSequenceSubElement &);
    Decoder_ParseSequenceSubElement& operator=(
                                       const Decoder_ParseSequenceSubElement&);

  public:
    // CREATORS
    Decoder_ParseSequenceSubElement(Decoder     *decoder,
                                    const char  *elementName,
                                    bsl::size_t  lenName);

    // Using compiler-generated destructor:
    //  ~Decoder_ParseSequenceSubElement();

    // MANIPULATORS
    template <class TYPE, class INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE &info);

    template <class TYPE>
    int execute(TYPE *object, int id, int formattingMode);
};

                        // ============================
                        // class Decoder_ParseAttribute
                        // ============================

class Decoder_ParseAttribute {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Parse an attribute.

    // DATA
    Decoder *d_decoder;     // error logger (held)
    bool            d_failed;      // set to true if parsing failed

    const char     *d_name_p;      // attribute name (held)
    const char     *d_value_p;     // attribute value (held)
    bsl::size_t     d_value_length;

  public:
    // IMPLEMENTATION MANIPULATORS
    template <class TYPE>
    int executeImp(TYPE                              *object,
                   int                                formattingMode,
                   bdlat_TypeCategory::NullableValue);
    template <class TYPE>
    int executeImp(TYPE                            *object,
                   int                              formattingMode,
                   bdlat_TypeCategory::DynamicType);
    template <class TYPE, class ANY_CATEGORY>
    int executeImp(TYPE *object, int formattingMode, ANY_CATEGORY);

  private:
    // NOT IMPLEMENTED
    Decoder_ParseAttribute(const Decoder_ParseAttribute&);
    Decoder_ParseAttribute&
    operator=(const Decoder_ParseAttribute&);

  public:
    // CREATORS
    Decoder_ParseAttribute(Decoder     *decoder,
                           const char  *name,
                           const char  *value,
                           bsl::size_t  lengthValue);

    // Generated by compiler:
    //  ~Decoder_ParseAttribute();

    // MANIPULATORS
    template <class TYPE, class INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE& info);

    template <class TYPE>
    int execute(TYPE *object, int formattingMode);

    // ACCESSORS
    bool failed() const;
};

                         // =========================
                         // class Decoder_ParseObject
                         // =========================

class Decoder_ParseObject {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.
    //
    // Parse the visited object.

    // PRIVATE TYPES
    struct CanBeListOrRepetition { };
    struct CanBeRepetitionOnly   { };

    // DATA
    Decoder *d_decoder;        // held, not owned
    const char     *d_elementName_p;  // held, not owned
    bsl::size_t     d_lenName;

    // NOT IMPLEMENTED
    Decoder_ParseObject(const Decoder_ParseObject&);
    Decoder_ParseObject& operator=(const Decoder_ParseObject&);

  public:
    // IMPLEMENTATION MANIPULATORS
    int executeImp(bsl::vector<char>         *object,
                   int                        formattingMode,
                   bdlat_TypeCategory::Array);

    template <class TYPE>
    int executeImp(bsl::vector<TYPE>         *object,
                   int                        formattingMode,
                   bdlat_TypeCategory::Array);

    template <class TYPE>
    int executeImp(TYPE                      *object,
                   int                        formattingMode,
                   bdlat_TypeCategory::Array);

    template <class TYPE>
    int executeImp(TYPE                         *object,
                   int                           formattingMode,
                   bdlat_TypeCategory::Sequence);

    template <class TYPE>
    int executeImp(TYPE                       *object,
                   int                         formattingMode,
                   bdlat_TypeCategory::Choice);

    template <class TYPE>
    int executeImp(TYPE                              *object,
                   int                                formattingMode,
                   bdlat_TypeCategory::NullableValue);

    template <class TYPE>
    int executeImp(TYPE                               *object,
                   int                                 formattingMode,
                   bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    int executeImp(TYPE                            *object,
                   int                              formattingMode,
                   bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    int executeImp(TYPE *object, int formattingMode, ANY_CATEGORY);

    template <class TYPE>
    int executeArrayImp(TYPE                  *object,
                        int                    formattingMode,
                        CanBeListOrRepetition);

    template <class TYPE>
    int executeArrayImp(TYPE *object, int formattingMode, CanBeRepetitionOnly);

    template <class TYPE>
    int executeArrayRepetitionImp(TYPE *object, int formattingMode);

  public:
    // CREATORS
    Decoder_ParseObject(Decoder     *decoder,
                        const char  *elementName,
                        bsl::size_t  lenName);

    // Using compiler-generated destructor:
    //  ~Decoder_ParseObject();

    // MANIPULATORS
    template <class TYPE, class INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE &info);

    template <class TYPE>
    int execute(TYPE *object, int formattingMode);
};

                     // =================================
                     // class Decoder_ParseNillableObject
                     // =================================

class Decoder_ParseNillableObject {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    int                      d_formattingMode;
    Decoder_NillableContext  d_nillableContext;
    Decoder                 *d_decoder;

  public:
    // IMPLEMENTATION MANIPULATORS
    template <class TYPE>
    int executeImp(TYPE *object, bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    int executeImp(TYPE *object, ANY_CATEGORY);

  private:
    // NOT IMPLEMENTED
    Decoder_ParseNillableObject(const Decoder_ParseNillableObject &);
    Decoder_ParseNillableObject& operator=(const Decoder_ParseNillableObject&);

  public:
    Decoder_ParseNillableObject(Decoder *decoder, int formattingMode);
        // Construct a functor to parse nillable objects.

    // Using compiler-generated destructor:
    //  ~Decoder_ParseNillableObject();

    // MANIPULATORS
    template <class TYPE>
    int operator()(TYPE *object);
        // Visit the specified 'object'.

    // ACCESSORS
    bool isNil() const;
        // Return 'true' if the value was nil, and false otherwise.
};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

                       // =============================
                       // struct Decoder_decodeImpProxy
                       // =============================

struct Decoder_decodeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder *d_decoder;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_decoder->decodeImp(object, category);
    }
};

                 // ==========================================
                 // struct Decoder_ParseAttribute_executeProxy
                 // ==========================================

struct Decoder_ParseAttribute_executeProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder_ParseAttribute *d_instance_p;
    int                     d_formattingMode;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *object)
    {
        return d_instance_p->execute(object, d_formattingMode);
    }
};

               // =============================================
               // struct Decoder_ParseAttribute_executeImpProxy
               // =============================================

struct Decoder_ParseAttribute_executeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder_ParseAttribute *d_instance_p;
    int                     d_formattingMode;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_instance_p->executeImp(object, d_formattingMode, category);
    }
};

                  // =======================================
                  // struct Decoder_ParseObject_executeProxy
                  // =======================================

struct Decoder_ParseObject_executeProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder_ParseObject *d_instance_p;
    int                  d_formattingMode;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *object)
    {
        return d_instance_p->execute(object, d_formattingMode);
    }
};

                 // ==========================================
                 // struct Decoder_ParseObject_executeImpProxy
                 // ==========================================

struct Decoder_ParseObject_executeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder_ParseObject *d_instance_p;
    int                  d_formattingMode;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_instance_p->executeImp(object, d_formattingMode, category);
    }
};

             // ==================================================
             // struct Decoder_ParseNillableObject_executeImpProxy
             // ==================================================

struct Decoder_ParseNillableObject_executeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    Decoder_ParseNillableObject *d_instance_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_instance_p->executeImp(object, category);
    }
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                    // -----------------------------------
                    // class balxml::Decoder::MemOutStream
                    // -----------------------------------

inline
balxml::Decoder::MemOutStream::MemOutStream(bslma::Allocator *basicAllocator)
: bsl::ostream(0)
, d_sb(bslma::Default::allocator(basicAllocator))
{
    rdbuf(&d_sb);
}

// MANIPULATORS
inline
void balxml::Decoder::MemOutStream::reset()
{
    d_sb.reset();
}

// ACCESSORS
inline
const char *balxml::Decoder::MemOutStream::data() const
{
    return d_sb.data();
}

inline
int balxml::Decoder::MemOutStream::length() const
{
    return (int)d_sb.length();
}

namespace balxml {
inline
void Decoder::setNumUnknownElementsSkipped(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_numUnknownElementsSkipped = value;
}

                               // -------------
                               // class Decoder
                               // -------------

inline
const DecoderOptions *Decoder::options() const
{
    return d_options;
}

inline
Reader *Decoder::reader() const
{
    return d_reader;
}

inline
ErrorInfo *Decoder::errorInfo() const
{
    return d_errorInfo;
}

inline
bsl::ostream *Decoder::errorStream() const
{
    return d_errorStream;
}

inline
int Decoder::numUnknownElementsSkipped() const
{
    return d_numUnknownElementsSkipped;
}

inline
bsl::ostream *Decoder::warningStream() const
{
    return d_warningStream;
}

inline
int Decoder::errorCount() const
{
    return d_errorCount;
}

inline
int Decoder::warningCount() const
{
    return d_warningCount;
}

inline
int Decoder::open(bsl::istream& stream, const char *uri)
{
    return open(stream.rdbuf(), uri);
}

template <class TYPE>
bsl::istream& Decoder::decode(bsl::istream&  stream,
                              TYPE          *object,
                              const char    *uri)
{
    if (!stream.good()) {

        BALXML_DECODER_LOG_ERROR(this)
                << "The input stream is invalid. "
                << "Unable to decode XML object. "
                << BALXML_DECODER_LOG_END;

        return stream;                                                // RETURN
    }

    if (0 != this->decode(stream.rdbuf(), object, uri)) {
        stream.setstate(bsl::ios_base::failbit);
    }

    return stream;
}

template <class TYPE>
int
Decoder::decode(bsl::streambuf *buffer, TYPE *object, const char *uri)
{
    if (this->open(buffer, uri) != 0) {

        return this->errorCount();                                    // RETURN

    }

    int ret = this->decode(object);

    switch(errorSeverity()) {
      case ErrorInfo::e_NO_ERROR:
        break;
      case ErrorInfo::e_WARNING:
        if (d_warningStream) {
            *d_warningStream << loggedMessages();
        }
        break;
      default:
        if (d_errorStream) {
            *d_errorStream << loggedMessages();
        }
        break;
    }

    this->close();
    return ret;
}

template <class TYPE>
int Decoder::decode(const char  *buffer,
                    bsl::size_t  buflen,
                    TYPE        *object,
                    const char  *uri)
{
    if (this->open(buffer, buflen, uri) != 0) {

        return this->errorCount();                                    // RETURN
    }

    int ret = this->decode(object);
    this->close();
    return ret;
}

template <class TYPE>
int Decoder::decode(const char *filename, TYPE *object)
{
    if (this->open(filename) != 0) {

        return this->errorCount();                                    // RETURN
    }

    int ret = this->decode(object);
    this->close();
    return ret;
}

template <class TYPE>
inline
int Decoder::decode(TYPE *object)
{
    bdlat_ValueTypeFunctions::reset(object);

    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    this->decodeImp(object, TypeCategory());

    return this->errorCount();
}

template <class TYPE>
inline
int Decoder::decodeImp(TYPE *object, bdlat_TypeCategory::DynamicType)
{
    Decoder_decodeImpProxy proxy = { this };
    return bdlat_TypeCategoryUtil::manipulateByCategory(object, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int Decoder::decodeImp(TYPE *object, ANY_CATEGORY)
{
    typedef typename
    Decoder_InstantiateContext<ANY_CATEGORY, TYPE>::Type ContextType;

    ContextType elementContext(object, d_options->formattingMode());

    return elementContext.beginParse(this);
}

                     // ---------------------------------
                     // class Decoder_ChoiceContext<TYPE>
                     // ---------------------------------

template <class TYPE>
inline
Decoder_ChoiceContext<TYPE>::Decoder_ChoiceContext(TYPE *object,
                                                   int   formattingMode)
: d_isSelectionNameKnown(false)
, d_object_p(object)
, d_selectionIsRepeatable(false)
, d_selectionName()
{
    (void) formattingMode;
    BSLS_ASSERT_SAFE(bdlat_FormattingMode::e_DEFAULT ==
                     (formattingMode & bdlat_FormattingMode::e_TYPE_MASK));
}

// CALLBACKS

template <class TYPE>
int Decoder_ChoiceContext<TYPE>::startElement(Decoder *)
{
    enum { k_SUCCESS = 0 };

    d_isSelectionNameKnown = false;  // no selection seen yet

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_ChoiceContext<TYPE>::endElement(Decoder *decoder)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (!d_isSelectionNameKnown) {
        BALXML_DECODER_LOG_ERROR(decoder)
                 << "No elements selected in choice."
                 << BALXML_DECODER_LOG_END;

        return k_FAILURE;  // will trigger failure in parser          // RETURN
    }

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_ChoiceContext<TYPE>::addCharacters(const char   *chars,
                                               unsigned int  length,
                                               Decoder      *decoder)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(0 != length);

    const char *begin = chars;
    const char *end   = begin + length;

    bdlb::String::skipLeadingTrailing(&begin, &end);

    if (begin != end) {
        BALXML_DECODER_LOG_ERROR(decoder)
                         << "Invalid characters \""
                         << bsl::string(begin, end - begin)
                         << "\" when parsing choice."
                         << BALXML_DECODER_LOG_END;

        return k_FAILURE;  // will trigger failure in parser          // RETURN
    }

    return k_SUCCESS;
}

template <class TYPE>
inline
int Decoder_ChoiceContext<TYPE>::parseAttribute(const char *,
                                                const char *,
                                                bsl::size_t,
                                                Decoder *)
{
    enum { k_ATTRIBUTE_IGNORED = 0 };

    return k_ATTRIBUTE_IGNORED;
}

template <class TYPE>
int Decoder_ChoiceContext<TYPE>::parseSubElement(const char *elementName,
                                                 Decoder    *decoder)
{
    enum { k_FAILURE = -1 };

    const int lenName = static_cast<int>(bsl::strlen(elementName));

    if (d_isSelectionNameKnown
     && (!d_selectionIsRepeatable || d_selectionName != elementName))
    {
        BALXML_DECODER_LOG_ERROR(decoder)
                            << "Only one selection is permitted inside choice."
                            << BALXML_DECODER_LOG_END;

        return k_FAILURE;                                             // RETURN
    }

    bool wasSelectionNameKnown = d_isSelectionNameKnown;
    d_isSelectionNameKnown = true;

    if (decoder->options()->skipUnknownElements() &&
        false == bdlat_ChoiceFunctions::hasSelection(*d_object_p,
                                                     elementName,
                                                     lenName)) {
        decoder->setNumUnknownElementsSkipped(
                                     decoder->numUnknownElementsSkipped() + 1);
        d_selectionIsRepeatable = true;  // assume repeatable
        d_selectionName.assign(elementName, lenName);

        Decoder_UnknownElementContext unknownElement;
        return unknownElement.beginParse(decoder);                    // RETURN
    }

    if (!wasSelectionNameKnown) {
        if (0 != bdlat_ChoiceFunctions::makeSelection(d_object_p,
                                                      elementName,
                                                      lenName)) {
            BALXML_DECODER_LOG_ERROR(decoder)
                                              << "Unable to make selection: \""
                                              << elementName
                                              << "\"."
                                              << BALXML_DECODER_LOG_END;

            return k_FAILURE;                                         // RETURN
        }

        d_selectionIsRepeatable = true;  // TBD: check if repeatable
        d_selectionName.assign(elementName, lenName);
    }

    Decoder_ParseObject parseObject(decoder, elementName, lenName);
    return bdlat_ChoiceFunctions::manipulateSelection(d_object_p, parseObject);
}

             // ----------------------------------------------------
             // class Decoder_PushParserContext<TYPE, PARSER>
             // ----------------------------------------------------

// CREATORS
template <class TYPE, class PARSER>
inline
Decoder_PushParserContext<TYPE, PARSER>::Decoder_PushParserContext(
                                                          TYPE *object,
                                                          int   formattingMode)
: d_formattingMode(formattingMode), d_object_p(object)
{
}

// CALLBACKS

template <class TYPE, class PARSER>
int Decoder_PushParserContext<TYPE, PARSER>::startElement(Decoder *decoder)
{
    int result = d_parser.beginParse(d_object_p);

    if (0 != result) {
        BALXML_DECODER_LOG_ERROR(decoder)
                    << "Unable to begin parsing list or binary type"

                    << "\"."
                    << BALXML_DECODER_LOG_END;
    }

    return result;
}

template <class TYPE, class PARSER>
int Decoder_PushParserContext<TYPE, PARSER>::endElement(Decoder *decoder)
{
    int result = d_parser.endParse();

    if (0 != result) {
        BALXML_DECODER_LOG_ERROR(decoder)
                    << "Unable to end parsing list or binary type"
                    << "\"."
                    << BALXML_DECODER_LOG_END;
    }

    return result;
}

template <class TYPE, class PARSER>
int Decoder_PushParserContext<TYPE, PARSER>::addCharacters(
                                                         const char   *chars,
                                                         unsigned int  length,
                                                         Decoder      *decoder)
{
    const char *begin = chars;
    const char *end   = begin + length;

    int result = d_parser.pushCharacters(begin, end);

    if (0 != result) {
        BALXML_DECODER_LOG_ERROR(decoder)
              << "Unable to push \"" << chars
              << "\" when parsing list or binary type"

              << "\"."
              << BALXML_DECODER_LOG_END;
    }

    return result;
}

template <class TYPE, class PARSER>
inline
int Decoder_PushParserContext<TYPE, PARSER>::parseAttribute(const char *,
                                                            const char *,
                                                            bsl::size_t,
                                                            Decoder *)
{
    enum { k_ATTRIBUTE_IGNORED = 0 };

    return k_ATTRIBUTE_IGNORED;
}

template <class TYPE, class PARSER>
int Decoder_PushParserContext<TYPE, PARSER>::parseSubElement(
                                                       const char *elementName,
                                                       Decoder    *decoder)
{
    enum { k_FAILURE = -1 };

    BALXML_DECODER_LOG_ERROR(decoder)
                  << "Unexpected sub-element \"" << elementName
                  << "\" when parsing list or binary type"
                  << "\"."
                  << BALXML_DECODER_LOG_END;

    return k_FAILURE;
}

                    // -----------------------------------
                    // class Decoder_SequenceContext<TYPE>
                    // -----------------------------------

// CREATORS
template <class TYPE>
inline
Decoder_SequenceContext<TYPE>::Decoder_SequenceContext(TYPE *object,
                                                       int   formattingMode)
: d_object_p(object)
{
    (void) formattingMode;
    BSLS_ASSERT_SAFE(bdlat_FormattingMode::e_DEFAULT ==
                     (formattingMode & bdlat_FormattingMode::e_TYPE_MASK));
}

// CALLBACKS

template <class TYPE>
int Decoder_SequenceContext<TYPE>::startElement(Decoder *decoder)
{
    //d_chars.clear();

    Decoder_PrepareSequenceContext prepareSequenceContext(&d_simpleContentId);

    int ret = bdlat_SequenceFunctions::manipulateAttributes(
                                                       d_object_p,
                                                       prepareSequenceContext);

    if (0 != ret) {
        BALXML_DECODER_LOG_ERROR(decoder)
                                       << "Unable to prepare sequence context!"
                                       << BALXML_DECODER_LOG_END;
    }

    return ret;
}

template <class TYPE>
int Decoder_SequenceContext<TYPE>::endElement(Decoder *)
{
    enum { k_SUCCESS = 0 };

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_SequenceContext<TYPE>::addCharacters(const char   *chars,
                                                 unsigned int  length,
                                                 Decoder      *decoder)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(0 != length);

    if (d_simpleContentId.isNull()) {

        const char *begin = chars;
        const char *end   = begin + length;

        bdlb::String::skipLeadingTrailing(&begin, &end);

        if (begin != end) {
            BALXML_DECODER_LOG_ERROR(decoder)
                                             << "Unexpected characters: \""
                                             << bsl::string(begin, end - begin)
                                             << "\"."
                                             << BALXML_DECODER_LOG_END;

            return k_FAILURE;                                         // RETURN
        }
        return k_SUCCESS;                                             // RETURN
    }

    Decoder_ParseSequenceSimpleContent parseSimpleContent(decoder,
                                                          chars,
                                                          length);

    return bdlat_SequenceFunctions::manipulateAttribute(
                                                    d_object_p,
                                                    parseSimpleContent,
                                                    d_simpleContentId.value());
}

template <class TYPE>
int Decoder_SequenceContext<TYPE>::parseAttribute(const char  *name,
                                                  const char  *value,
                                                  bsl::size_t  lenValue,
                                                  Decoder     *decoder)
{
    enum { k_SUCCESS = 0, k_ATTRIBUTE_IGNORED = 0, k_FAILURE = -1 };

    const int lenName = static_cast<int>(bsl::strlen(name));

    Decoder_ParseAttribute visitor(decoder, name, value, lenValue);

    if (0 != bdlat_SequenceFunctions::manipulateAttribute(d_object_p,
                                                          visitor,
                                                          name,
                                                          lenName)) {
        if (visitor.failed()) {
            return k_FAILURE;                                         // RETURN
        }
        return k_ATTRIBUTE_IGNORED;                                   // RETURN
    }

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_SequenceContext<TYPE>::parseSubElement(const char *elementName,
                                                   Decoder    *decoder)
{
    enum { k_FAILURE = -1 };

    const int lenName = static_cast<int>(bsl::strlen(elementName));

    if (decoder->options()->skipUnknownElements()
     && false == bdlat_SequenceFunctions::hasAttribute(*d_object_p,
                                                       elementName,
                                                       lenName)) {
        decoder->setNumUnknownElementsSkipped(
                                     decoder->numUnknownElementsSkipped() + 1);
        Decoder_UnknownElementContext unknownElement;
        return unknownElement.beginParse(decoder);                    // RETURN
    }

    Decoder_ParseSequenceSubElement visitor(decoder, elementName, lenName);

    return bdlat_SequenceFunctions::manipulateAttribute(d_object_p,
                                                        visitor,
                                                        elementName,
                                                        lenName);
}

                     // ---------------------------------
                     // class Decoder_SimpleContext<TYPE>
                     // ---------------------------------

// CREATORS
template <class TYPE>
inline
Decoder_SimpleContext<TYPE>::Decoder_SimpleContext(TYPE *object,
                                                   int   formattingMode)
: d_formattingMode(formattingMode)
, d_object_p(object)
{
}

// CALLBACKS

template <class TYPE>
int Decoder_SimpleContext<TYPE>::startElement(Decoder *)
{
    enum { k_SUCCESS = 0 };

    //d_chars.clear();

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_SimpleContext<TYPE>::endElement(Decoder *)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    return k_SUCCESS;
}

template <class TYPE>
int Decoder_SimpleContext<TYPE>::addCharacters(const char   *chars,
                                               unsigned int  length,
                                               Decoder      *decoder)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const char *begin = chars;
    const char *end   = begin + length;

    bdlb::String::skipLeadingTrailing(&begin, &end);

    if (0 != TypesParserUtil::parse(d_object_p,
                                    begin,
                                    static_cast<int>(end - begin),
                                    d_formattingMode)) {
        BALXML_DECODER_LOG_ERROR(decoder)
                    << "Unable to parse \""
                    << bsl::string(begin, end)
                    << "\" when parsing list or binary type"
                    << "\".\n"
                    << BALXML_DECODER_LOG_END;

        return k_FAILURE;                                             // RETURN
    }

    return k_SUCCESS;
}

template <class TYPE>
inline
int Decoder_SimpleContext<TYPE>::parseAttribute(const char *,
                                                const char *,
                                                bsl::size_t,
                                                Decoder *)
{
    enum { k_ATTRIBUTE_IGNORED = 0 };

    return k_ATTRIBUTE_IGNORED;
}

template <class TYPE>
int Decoder_SimpleContext<TYPE>::parseSubElement(const char *elementName,
                                                 Decoder    *decoder)
{
    enum { k_FAILURE = -1 };

    BALXML_DECODER_LOG_ERROR(decoder)
               << "Attempted to create sub context for \""
               << elementName << "\" inside simple type"

               << "\"."
               << BALXML_DECODER_LOG_END;

    return k_FAILURE;  // will trigger failure in parser
}

                   // -------------------------------------
                   // class Decoder_CustomizedContext<TYPE>
                   // -------------------------------------

// CREATORS
template <class TYPE>
inline
Decoder_CustomizedContext<TYPE>::Decoder_CustomizedContext(
                                                          TYPE *object,
                                                          int   formattingMode)
: d_object (object)
, d_baseObj()
, d_baseContext(&d_baseObj, formattingMode)
{
}

// CALLBACKS

template <class TYPE>
int Decoder_CustomizedContext<TYPE>::startElement(Decoder *decoder)
{
    return d_baseContext.startElement (decoder);
}

template <class TYPE>
int Decoder_CustomizedContext<TYPE>::endElement(Decoder *decoder)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };
    int rc = d_baseContext.endElement(decoder);
    if (rc == k_SUCCESS
     &&  0 == bdlat_CustomizedTypeFunctions::convertFromBaseType(d_object,
                                                                 d_baseObj)) {
        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

template <class TYPE>
int Decoder_CustomizedContext<TYPE>::addCharacters(const char   *chars,
                                                   unsigned int  length,
                                                   Decoder      *decoder)
{
    return d_baseContext.addCharacters(chars, length, decoder);
}

template <class TYPE>
int Decoder_CustomizedContext<TYPE>::parseAttribute(const char  *name,
                                                    const char  *value,
                                                    bsl::size_t  lenValue,
                                                    Decoder     *decoder)
{
    return d_baseContext.parseAttribute(name, value, lenValue, decoder);
}

template <class TYPE>
int Decoder_CustomizedContext<TYPE>::parseSubElement(const char *elementName,
                                                     Decoder    *decoder)
{
    return d_baseContext.parseSubElement(elementName, decoder);
}

                         // -------------------------
                         // class Decoder_UTF8Context
                         // -------------------------

// CREATORS
template <class TYPE>
inline
Decoder_UTF8Context<TYPE>::Decoder_UTF8Context(TYPE *object, int)
: d_object_p(object)
{
}

// CALLBACKS

template <class TYPE>
inline
int Decoder_UTF8Context<TYPE>::startElement(Decoder *)
{
    enum { k_SUCCESS = 0 };

    d_object_p->clear();

    return k_SUCCESS;
}

template <class TYPE>
inline
int Decoder_UTF8Context<TYPE>::endElement(Decoder *)
{
    enum { k_SUCCESS = 0 };

    return k_SUCCESS;
}

template <class TYPE>
inline int
Decoder_UTF8Context<TYPE>::addCharacters(const char   *chars,
                                         unsigned int  length,
                                         Decoder      *)
{
    enum { k_SUCCESS = 0 };

    d_object_p->insert(d_object_p->end(), chars, chars + length);

    return k_SUCCESS;
}

template <class TYPE>
inline
int Decoder_UTF8Context<TYPE>::parseAttribute(const char *,
                                              const char *,
                                              bsl::size_t,
                                              Decoder *)
{
    enum { k_ATTRIBUTE_IGNORED = 0 };

    return k_ATTRIBUTE_IGNORED;
}

template <class TYPE>
int Decoder_UTF8Context<TYPE>::parseSubElement(const char *elementName,
                                               Decoder    *decoder)
{
    enum { k_FAILURE = -1 };

    BALXML_DECODER_LOG_ERROR(decoder)
                                    << "Attempted to create sub context for \""
                                    << elementName << "\" inside UTF8 type."
                                    << BALXML_DECODER_LOG_END;

    return k_FAILURE;  // will trigger failure in parser
}

                    // ------------------------------------
                    // class Decoder_PrepareSequenceContext
                    // ------------------------------------

// CREATORS
inline
Decoder_PrepareSequenceContext::Decoder_PrepareSequenceContext(
                                     bdlb::NullableValue<int> *simpleContentId)
: d_simpleContentId_p(simpleContentId)
{
    d_simpleContentId_p->reset();
}

// MANIPULATORS
template <class TYPE, class INFO_TYPE>
int Decoder_PrepareSequenceContext::operator()(const TYPE&,
                                               const INFO_TYPE& info)
{
    enum { k_SUCCESS = 0 };

    if (info.formattingMode() & bdlat_FormattingMode::e_SIMPLE_CONTENT) {
        BSLS_ASSERT_SAFE(d_simpleContentId_p->isNull());
        d_simpleContentId_p->makeValue(info.id());
    }

    return k_SUCCESS;
}

                  // ----------------------------------------
                  // class Decoder_ParseSequenceSimpleContent
                  // ----------------------------------------

// CREATORS
inline
Decoder_ParseSequenceSimpleContent::Decoder_ParseSequenceSimpleContent(
                                                          Decoder     *decoder,
                                                          const char  *chars,
                                                          bsl::size_t  len)
: d_chars_p(chars), d_len(len), d_decoder(decoder)
{
    BSLS_ASSERT_SAFE(d_chars_p);
    BSLS_ASSERT_SAFE(d_decoder);
}

// MANIPULATORS
template <class TYPE, class INFO_TYPE>
int Decoder_ParseSequenceSimpleContent::operator()(TYPE             *object,
                                                   const INFO_TYPE&  info)
{
    BSLS_ASSERT_SAFE(info.formattingMode()
                                 & bdlat_FormattingMode::e_SIMPLE_CONTENT);

    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const char *begin = d_chars_p;
    const char *end   = begin + d_len;

    bdlb::String::skipLeadingTrailing(&begin, &end);

    if (0 != TypesParserUtil::parse(object,
                                    begin,
                                    static_cast<int>(end - begin),
                                    info.formattingMode())) {
        BALXML_DECODER_LOG_ERROR(d_decoder)
               << "Unable to parse \""
               << bsl::string(begin, end)
               << "\" within simple content"

               << "\"."
               << BALXML_DECODER_LOG_END;

        return k_FAILURE;                                             // RETURN
    }

    return k_SUCCESS;
}

template <class INFO_TYPE>
inline
int Decoder_ParseSequenceSimpleContent::operator()(bsl::string      *object,
                                                   const INFO_TYPE&  info)
{
    enum { k_SUCCESS = 0 };

    BSLS_ASSERT_SAFE(info.formattingMode()
                   & bdlat_FormattingMode::e_SIMPLE_CONTENT);

    (void) info;

    object->assign(d_chars_p, d_len);

    return k_SUCCESS;
}

                   // -------------------------------------
                   // class Decoder_ParseSequenceSubElement
                   // -------------------------------------

// CREATORS
inline
Decoder_ParseSequenceSubElement::Decoder_ParseSequenceSubElement(
                                                      Decoder     *decoder,
                                                      const char  *elementName,
                                                      bsl::size_t  lenName)
: d_decoder(decoder), d_elementName_p(elementName), d_lenName(lenName)
{
}

// MANIPULATORS
template <class TYPE, class INFO_TYPE>
inline
int Decoder_ParseSequenceSubElement::operator()(TYPE             *object,
                                                const INFO_TYPE&  info)
{
    return execute(object, info.id(), info.formattingMode());
}

template <class TYPE>
int Decoder_ParseSequenceSubElement::execute(TYPE *object,
                                             int,
                                             int   formattingMode)
{
    enum { k_FAILURE = -1 };
    Decoder_ParseObject parseObject(d_decoder, d_elementName_p, d_lenName);

    return parseObject.execute(object, formattingMode);
}

                        // ----------------------------
                        // class Decoder_ParseAttribute
                        // ----------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
int Decoder_ParseAttribute::executeImp(
                             TYPE                              *object,
                             int                                formattingMode,
                             bdlat_TypeCategory::NullableValue)
{
    if (bdlat_NullableValueFunctions::isNull(*object)) {
        bdlat_NullableValueFunctions::makeValue(object);
    }

    Decoder_ParseAttribute_executeProxy proxy = {
        this, formattingMode
    };

    return bdlat_NullableValueFunctions::manipulateValue(object, proxy);
}

template <class TYPE>
inline
int Decoder_ParseAttribute::executeImp(
                               TYPE                            *object,
                               int                              formattingMode,
                               bdlat_TypeCategory::DynamicType)
{
    Decoder_ParseAttribute_executeImpProxy proxy = { this,
                                                         formattingMode };
    return bdlat_TypeCategoryUtil::manipulateByCategory(object, proxy);
}

template <class TYPE, class ANY_CATEGORY>
int Decoder_ParseAttribute::executeImp(TYPE         *object,
                                       int           formattingMode,
                                       ANY_CATEGORY)
{
    enum { k_SUCCESS = 0, k_FAILURE = - 1 };

    bool isAttribute = formattingMode
                     & bdlat_FormattingMode::e_ATTRIBUTE;

    if (!isAttribute) {
        BALXML_DECODER_LOG_WARNING(d_decoder)
                                          << "Object '" << d_name_p << "' is "
                                          << "being parsed as an attribute, "
                                          << "but it does not have the "
                                          << "'IS_ATTRIBUTE' flag set."
                                          << BALXML_DECODER_LOG_END;
    }

    if (0 != TypesParserUtil::parse(object,
                                    d_value_p,
                                    static_cast<int>(d_value_length),
                                    formattingMode)) {
        BALXML_DECODER_LOG_ERROR(d_decoder)
                   << "Unable to parse \""
                   << bsl::string(d_value_p, d_value_length)
                   << "\" (for '" << d_name_p << "' attribute)"

                   << "\".\n"
                   << BALXML_DECODER_LOG_END;

        d_failed = true;

        return k_FAILURE;                                             // RETURN
    }

    return k_SUCCESS;
}

// CREATORS
inline
Decoder_ParseAttribute::Decoder_ParseAttribute(Decoder     *decoder,
                                               const char  *name,
                                               const char  *value,
                                               bsl::size_t  lengthValue)
: d_decoder(decoder)
, d_failed(false)
, d_name_p(name)
, d_value_p(value)
, d_value_length(lengthValue)
{
    BSLS_ASSERT_SAFE(d_decoder);
    BSLS_ASSERT_SAFE(d_name_p);
    BSLS_ASSERT_SAFE(d_value_p);
}

// MANIPULATORS
template <class TYPE, class INFO_TYPE>
inline
int Decoder_ParseAttribute::operator()(TYPE *object, const INFO_TYPE& info)
{
    return execute(object, info.formattingMode());
}

template <class TYPE>
inline
int Decoder_ParseAttribute::execute(TYPE *object, int formattingMode)
{
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return executeImp(object, formattingMode, TypeCategory());
}

// ACCESSORS
inline
bool Decoder_ParseAttribute::failed() const
{
    return d_failed;
}

                         // -------------------------
                         // class Decoder_ParseObject
                         // -------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
inline
int Decoder_ParseObject::executeImp(bsl::vector<TYPE>         *object,
                                    int                        formattingMode,
                                    bdlat_TypeCategory::Array)
{
    typedef bdlat_TypeCategory::Select<TYPE> Selector;

    enum {
        CAN_BE_REPETITION_ONLY
            = (  (int)Selector::e_SELECTION
                           == (int)bdlat_TypeCategory::e_SEQUENCE_CATEGORY
              || (int)Selector::e_SELECTION
                           == (int)bdlat_TypeCategory::e_CHOICE_CATEGORY)
    };

    typedef typename
    bslmf::If<CAN_BE_REPETITION_ONLY, CanBeRepetitionOnly,
                                      CanBeListOrRepetition>::Type Toggle;

    return executeArrayImp(object, formattingMode, Toggle());
}

template <class TYPE>
inline
int Decoder_ParseObject::executeImp(TYPE                      *object,
                                    int                        formattingMode,
                                    bdlat_TypeCategory::Array)
{
    return executeArrayImp(object, formattingMode, CanBeListOrRepetition());
}

template <class TYPE>
int Decoder_ParseObject::executeImp(
                                  TYPE                         *object,
                                  int                           formattingMode,
                                  bdlat_TypeCategory::Sequence)
{
    enum { k_FAILURE = -1 };

    if (formattingMode & bdlat_FormattingMode::e_UNTAGGED) {
        if (d_decoder->options()->skipUnknownElements()
         && false == bdlat_SequenceFunctions::hasAttribute(
                                                *object,
                                                d_elementName_p,
                                                static_cast<int>(d_lenName))) {
            d_decoder->setNumUnknownElementsSkipped(
                                   d_decoder->numUnknownElementsSkipped() + 1);
            Decoder_UnknownElementContext unknownElement;
            return unknownElement.beginParse(d_decoder);              // RETURN
        }

        return bdlat_SequenceFunctions::manipulateAttribute(
                                                  object,
                                                  *this,
                                                  d_elementName_p,
                                                  static_cast<int>(d_lenName));
                                                                      // RETURN
    }

    typedef typename
    Decoder_InstantiateContext<
                             bdlat_TypeCategory::Sequence, TYPE>::Type Context;

    Context context(object, formattingMode);

    return context.beginParse(d_decoder);
}

template <class TYPE>
int Decoder_ParseObject::executeImp(TYPE                       *object,
                                    int                         formattingMode,
                                    bdlat_TypeCategory::Choice)
{
    enum { k_FAILURE = -1 };

    bool isUntagged = formattingMode & bdlat_FormattingMode::e_UNTAGGED;

    if (isUntagged) {
        if (d_decoder->options()->skipUnknownElements()
         && false == bdlat_ChoiceFunctions::hasSelection(
                                                *object,
                                                d_elementName_p,
                                                static_cast<int>(d_lenName))) {
            d_decoder->setNumUnknownElementsSkipped(
                                   d_decoder->numUnknownElementsSkipped() + 1);
            Decoder_UnknownElementContext unknownElement;
            return unknownElement.beginParse(d_decoder);              // RETURN
        }

        if (0 != bdlat_ChoiceFunctions::makeSelection(
                                                object,
                                                d_elementName_p,
                                                static_cast<int>(d_lenName))) {
            BALXML_DECODER_LOG_ERROR(d_decoder)
                                              << "Unable to make selection: \""
                                              << d_elementName_p
                                              << "\"."
                                              << BALXML_DECODER_LOG_END;

            return k_FAILURE;                                         // RETURN
        }

        return bdlat_ChoiceFunctions::manipulateSelection(object, *this);
                                                                      // RETURN
    }

    typedef typename
    Decoder_InstantiateContext<
                               bdlat_TypeCategory::Choice, TYPE>::Type Context;

    Context context(object, formattingMode);

    return context.beginParse(d_decoder);
}

template <class TYPE>
int Decoder_ParseObject::executeImp(
                             TYPE                              *object,
                             int                                formattingMode,
                             bdlat_TypeCategory::NullableValue)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (bdlat_NullableValueFunctions::isNull(*object)) {
        bdlat_NullableValueFunctions::makeValue(object);
    }

    bool isNillable = formattingMode & bdlat_FormattingMode::e_NILLABLE;

    if (isNillable) {
        Decoder_ParseNillableObject parseAsNillable(d_decoder, formattingMode);

        if (0 != bdlat_NullableValueFunctions::manipulateValue(
                                                            object,
                                                            parseAsNillable)) {
            return k_FAILURE;                                         // RETURN
        }

        if (parseAsNillable.isNil()) {
            // reset the object to null
            bdlat_ValueTypeFunctions::reset(object);
        }

        return k_SUCCESS;                                             // RETURN
    }

    Decoder_ParseObject_executeProxy proxy = { this, formattingMode };

    return bdlat_NullableValueFunctions::manipulateValue(object, proxy);
}

template <class TYPE>
int Decoder_ParseObject::executeImp(
                            TYPE                               *object,
                            int                                 formattingMode,
                            bdlat_TypeCategory::CustomizedType)
{
    typedef typename
    Decoder_InstantiateContext<
                     bdlat_TypeCategory::CustomizedType, TYPE>::Type
                     Context;

    Context context(object, formattingMode);

    return context.beginParse(d_decoder);
}

template <class TYPE>
inline
int Decoder_ParseObject::executeImp(
                               TYPE                            *object,
                               int                              formattingMode,
                               bdlat_TypeCategory::DynamicType)
{
    Decoder_ParseObject_executeImpProxy proxy = {
        this, formattingMode
    };

    return bdlat_TypeCategoryUtil::manipulateByCategory(object, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int Decoder_ParseObject::executeImp(TYPE         *object,
                                    int           formattingMode,
                                    ANY_CATEGORY)
{
    typedef typename
    Decoder_InstantiateContext<ANY_CATEGORY, TYPE>::Type Context;

    Context context(object, formattingMode);

    return context.beginParse(d_decoder);
}

template <class TYPE>
int Decoder_ParseObject::executeArrayImp(TYPE                  *object,
                                         int                    formattingMode,
                                         CanBeListOrRepetition)
{
    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        typedef Decoder_PushParserContext<TYPE, Decoder_ListParser<TYPE> >
            ListContext;

        ListContext listContext(object, formattingMode);

        return listContext.beginParse(d_decoder);                     // RETURN
    } else {
        return executeArrayRepetitionImp(object, formattingMode);     // RETURN
    }
}

template <class TYPE>
inline
int Decoder_ParseObject::executeArrayImp(TYPE                *object,
                                         int                  formattingMode,
                                         CanBeRepetitionOnly)
{
    return executeArrayRepetitionImp(object, formattingMode);
}

template <class TYPE>
int Decoder_ParseObject::executeArrayRepetitionImp(TYPE *object,
                                                   int   formattingMode)
{
    BSLS_ASSERT_SAFE(! (formattingMode & bdlat_FormattingMode::e_TYPE_MASK
                                   & bdlat_FormattingMode::e_LIST));

    Decoder_ParseObject_executeProxy proxy = { this, formattingMode };

    const int i = static_cast<int>(bdlat_ArrayFunctions::size(*object));

    bdlat_ArrayFunctions::resize(object, i + 1);

    return bdlat_ArrayFunctions::manipulateElement(object, proxy, i);
}

// CREATORS
inline
Decoder_ParseObject::Decoder_ParseObject(Decoder     *decoder,
                                         const char  *elementName,
                                         bsl::size_t  lenName)
: d_decoder(decoder)
, d_elementName_p(elementName)
, d_lenName(lenName)
{
    BSLS_ASSERT_SAFE(d_elementName_p);
    BSLS_ASSERT_SAFE(d_decoder);
}

// MANIPULATORS
template <class TYPE, class INFO_TYPE>
inline
int Decoder_ParseObject::operator()(TYPE *object, const INFO_TYPE &info)
{
    return execute(object, info.formattingMode());
}

template <class TYPE>
inline
int Decoder_ParseObject::execute(TYPE *object, int formattingMode)
{
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return executeImp(object, formattingMode, TypeCategory());
}

                     // ---------------------------------
                     // class Decoder_ParseNillableObject
                     // ---------------------------------

// IMPLEMENTATION MANIPULATORS
template <class TYPE>
inline
int Decoder_ParseNillableObject::executeImp(
                                       TYPE                            *object,
                                       bdlat_TypeCategory::DynamicType)
{
    Decoder_ParseNillableObject_executeImpProxy proxy = { this };

    return bdlat_TypeCategoryUtil::manipulateByCategory(object, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int Decoder_ParseNillableObject::executeImp(TYPE *object, ANY_CATEGORY)
{
    typedef typename
    Decoder_InstantiateContext<ANY_CATEGORY, TYPE>::Type Context;

    Context elementContext(object, d_formattingMode);

    d_nillableContext.setElementContext(&elementContext);

    return d_nillableContext.beginParse(d_decoder);
}

inline
Decoder_ParseNillableObject::Decoder_ParseNillableObject(
                                                       Decoder *decoder,
                                                       int      formattingMode)
: d_formattingMode(formattingMode)
, d_nillableContext()
, d_decoder(decoder)
{
}

// MANIPULATORS
template <class TYPE>
inline
int Decoder_ParseNillableObject::operator()(TYPE *object)
{
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return executeImp(object, TypeCategory());
}

// ACCESSORS
inline
bool Decoder_ParseNillableObject::isNil() const
{
    return d_nillableContext.isNil();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
