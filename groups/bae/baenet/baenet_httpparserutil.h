// baenet_httpparserutil.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPPARSERUTIL
#define INCLUDED_BAENET_HTTPPARSERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a utility for parsing HTTP constructs.
//
//@CLASSES:
//  baenet_HttpParserUtil: utility for parsing HTTP constructs
//  baenet_HttpParserUtilAddField: visitor for adding field values to a field
//
//@SEE_ALSO: RFC 2616
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides utility functions for parsing HTTP
// constructs, such as the start line and field values.  There is also a
// visitor utility for adding values to an HTTP field.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we need to read a request-line from a 'streambuf'.  We can do this
// as follows:
//..
//  bsl::streambuf *source = ...;
//
//  int                       accumNumBytesConsumed = 0;
//  bdema_SequentialAllocator allocator;
//  bdeut_StringRef           requestLineStr;
//  baenet_HttpRequestLine    requestLine;
//
//  if (0 != baenet_HttpParserUtil::parseLine(&requestLineStr,
//                                            &accumNumBytesConsumed,
//                                            &allocator,
//                                            source)) {
//      bsl::cout << "Failed to parse line!" << bsl::endl;
//      return;
//  }
//
//  if (0 != baenet_HttpParserUtil::parseStartLine(&requestLine,
//                                                 requestLineStr)) {
//      bsl::cout << "Failed to parse request line!" << bsl::endl;
//      return;
//  }
//
//  bsl::cout << "Request line = " << requestLine << bsl::endl;
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif


namespace BloombergLP {

class bdet_Datetime;
class bdet_DatetimeTz;
class bdema_SequentialAllocator;

class baenet_HttpContentType;
class baenet_HttpHost;
class baenet_HttpRequestLine;
class baenet_HttpStatusLine;
class baenet_HttpViaRecord;

                        // ============================
                        // struct baenet_HttpParserUtil
                        // ============================

struct baenet_HttpParserUtil {
    // Utility for parsing HTTP constructs.

  private:
    // PRIVATE FUNCTIONS
    template <typename TYPE>
    static int parseFieldValueImp(TYPE                   *result,
                                  const bdeut_StringRef&  str,
                                  bdeat_TypeCategory::Enumeration);
        // Parse field value referenced by the specified 'str' into the
        // specified 'result' using the 'bdeat_EnumFunctions::fromString'
        // function.  Return 0 on success, and a non-zero value otherwise.

    enum {
        BAENET_DEFAULT_MAX_BYTES_CONSUMED = 8192
    };

  public:
    // CONSTANTS
    enum {
        BAENET_REACHED_EOF        = 1
      , BAENET_END_OF_HEADER      = 2
      , BAENET_MAX_BYTES_EXCEEDED = 3
    };

    // FUNCTIONS
    static int parseChunkHeader(int            *result,
                                int            *numBytesConsumed,
                                bsl::streambuf *buffer);
        // Parse the chunk header contained in the specified 'buffer' and load
        // into the specified 'result' the non-negative size of the chunk (in
        // bytes).  Load into the specified 'numBytesConsumed' the number of
        // characters read from 'buffer'.  Return 0 on success, 'REACHED_EOF'
        // if more data is required, or -1 if a parse error occurs.  'result'
        // and 'numBytesConsumed' are not modified unless this function returns
        // successfully.  Note that on success it is possible for 'result' to
        // contain 0 indicating that no further chunks are expected.
        //
        // Following is section 3.6.1 "Chunked Transfer Encoding" from
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html
        // 3.6.1 Chunked Transfer Coding
        // The chunked encoding modifies the body of a message in order to
        // transfer it as a series of chunks, each with its own size indicator,
        // followed by an OPTIONAL trailer containing entity-header fields.
        // This allows dynamically produced content to be transferred along
        // with the information necessary for the recipient to verify that it
        // has received the full message.
        // Chunked-Body   = *chunk
        //                  last-chunk
        //                  trailer
        //                  CRLF
        // chunk          = chunk-size [ chunk-extension ] CRLF
        //                  chunk-data CRLF
        // chunk-size     = 1*HEX
        // last-chunk     = 1*("0") [ chunk-extension ] CRLF
        // chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
        // chunk-ext-name = token
        // chunk-ext-val  = token | quoted-string
        // chunk-data     = chunk-size(OCTET)
        // trailer        = *(entity-header CRLF)
        // The chunk-size field is a string of hex digits indicating the size
        // of the chunk.  The chunked encoding is ended by any chunk whose size
        // is zero, followed by the trailer, which is terminated by an empty
        // line.
        // The trailer allows the sender to include additional HTTP header
        // fields at the end of the message.  The Trailer header field can be
        // used to indicate which header fields are included in a trailer
        // (see section 14.40).
        // A server using chunked transfer-coding in a response MUST NOT use
        // the trailer for any header fields unless at least one of the
        // following is true:
        //   a) the request included a TE header field that indicates
        //      "trailers" is acceptable in the transfer-coding of the
        //      response, as described in section 14.39; or,
        //   b) the server is the origin server for the response, the trailer
        //      fields consist entirely of optional metadata, and the
        //      recipient could use the message (in a manner acceptable to the
        //      origin server) without receiving this metadata.  In other
        //      words, the origin server is willing to accept the possibility
        //      that the trailer fields might be silently discarded along the
        //      path to the client.
        // This requirement prevents an interoperability failure when the
        // message is being received by an HTTP/1.1 (or later) proxy and
        // forwarded to an HTTP/1.0 recipient.  It avoids a situation where
        // compliance with the protocol would have necessitated a possibly
        // infinite buffer on the proxy.
        // An example process for decoding a Chunked-Body is presented in
        // appendix 19.4.6.
        // All HTTP/1.1 applications MUST be able to receive and decode the
        // "chunked" transfer-coding, and MUST ignore chunk-extension
        // extensions they do not understand.

    static int parseFieldName(bdeut_StringRef           *result,
                              int                       *accumNumBytesConsumed,
                              bdema_SequentialAllocator *alloc,
                              bsl::streambuf            *source,
                              int                        maxNumBytesConsumed
                                          = BAENET_DEFAULT_MAX_BYTES_CONSUMED);
        // Load into the specified 'result' the field name (excluding the ':')
        // from the specified 'source';  load into the specified
        // 'accumNumBytesConsumed' the number of bytes consumed from 'source'
        // (including the ':');  use the specified 'alloc' to allocate memory.
        // Return 0 on success, 'BAENET_REACHED_EOF' if more data is required,
        // 'BAENET_END_OF_HEADER' if the end-of-header tag is found,
        // 'BAENET_MAX_BYTES_EXCEEDED' if the bytes consumed exceed the
        // 'maxNumBytesConsumed', or -1 if an invalid character is found.

    template <typename TYPE>
    static int parseFieldValue(TYPE                   *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(int                    *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(bsl::string            *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(bdet_Datetime          *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(bdet_DatetimeTz        *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(baenet_HttpContentType *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(baenet_HttpViaRecord   *result,
                               const bdeut_StringRef&  str);
    static int parseFieldValue(baenet_HttpHost        *result,
                               const bdeut_StringRef&  str);
        // Parse the field value referenced by the specified 'str' into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    static int parseFieldValueUnstructured(
                              bdeut_StringRef           *result,
                              int                       *accumNumBytesConsumed,
                              bdema_SequentialAllocator *alloc,
                              bsl::streambuf            *source,
                              int                        maxNumBytesConsumed
                                          = BAENET_DEFAULT_MAX_BYTES_CONSUMED);
        // Load into the specified 'result' the unstructured field value
        // (excluding the terminating CRLF) from the specified 'source'; load
        // into the specified 'accumNumBytesConsumed' the number of bytes
        // consumed from 'source' (including the terminating CRLF); use the
        // specified 'alloc' to allocate memory.  Return 0 on success, or
        // 'BAENET_REACHED_EOF' if more data is required, or
        // 'BAENET_MAX_BYTES_EXCEEDED' if the bytes consumed exceed the
        // 'maxNumBytesConsumed'.

    static int parseLine(bdeut_StringRef           *result,
                         int                       *accumNumBytesConsumed,
                         bdema_SequentialAllocator *alloc,
                         bsl::streambuf            *source,
                         int                        maxNumBytesConsumed
                                          = BAENET_DEFAULT_MAX_BYTES_CONSUMED);
        // Load into the specified 'result' a line (excluding the CRLF) from
        // the specified 'source';  load into the specified
        // 'accumNumBytesConsumed' the number of bytes consumed from 'source'
        // (including the CRLF);  use the specified 'alloc' to allocate memory.
        // Return 0 on success, or 'BAENET_REACHED_EOF' if more data is
        // required, or 'BAENET_MAX_BYTES_EXCEEDED' if the number of bytes
        // consumed exceed the 'maxNumBytesConsumed'.

    static int parseStartLine(baenet_HttpRequestLine *result,
                              const bdeut_StringRef&  str);
    static int parseStartLine(baenet_HttpStatusLine  *result,
                              const bdeut_StringRef&  str);
        // Parse the start line referenced by the specified 'str' into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    static void skipCommentsAndFoldedWhitespace(const char **begin,
                                                const char  *end);
        // Load into the specified '*begin' the address of the character
        // following the CFWS starting at '*begin'.  Load into '*begin' the
        // specified 'end' if 'end' is reached before the CFWS finished.

    static void skipDomainLiteral(const char **begin,
                                  const char  *end);
        // Load into the specified '*begin' the address of the character
        // following the domain-literal starting at '*begin'.  Load into
        // '*begin' the specified 'end' if 'end' is reached before the
        // domain-literal finished.  Return with no effect on '*begin' if
        // '*begin' does not point to a '[' character.

    static void skipQuotedString(const char **begin,
                                 const char  *end);
        // Load into the specified '*begin' the address of the character
        // following the quoted-string starting at '*begin'.  Load into
        // '*begin' the specified 'end' if 'end' is reached before the
        // quoted-string finished.  Return with no effect on '*begin' if
        // '*begin' does not point to a '"' character.
};

                    // ===================================
                    // class baenet_HttpParserUtilAddField
                    // ===================================

class baenet_HttpParserUtilAddField {
    // This visitor will add a value to the visited field.  If the field is an
    // array, this visitor will automatically grow the array.  If the field is
    // nullable, this visitor will automatically make the value before parsing
    // the field value.

    // PRIVATE DATA MEMBERS
    const bdeut_StringRef *d_fieldValue_p;  // held, not owned

    // PRIVATE MANIPULATORS
    template <typename TYPE>
    int appendElementToArray(TYPE                   *object,
                             const bdeut_StringRef&  str);
        // Append, to the specified 'object' array, an element obtained by
        // parsing the specified 'str'.  Return 0 on success, and a non-zero
        // value otherwise.

    template <typename TYPE>
    int execute(TYPE *object, bdeat_TypeCategory::Array);
    template <typename TYPE>
    int execute(TYPE *object, bdeat_TypeCategory::NullableValue);
    template <typename TYPE, typename ANY_CATEGORY>
    int execute(TYPE *object, ANY_CATEGORY);
        // Add the associated field value to the specified 'object'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the second
        // argument indicates the type category of the parameterized 'TYPE',
        // and is used for overloading purposes.

  private:
    // NOT IMPLEMENTED
    baenet_HttpParserUtilAddField(const baenet_HttpParserUtilAddField&);
    baenet_HttpParserUtilAddField& operator=(
                                         const baenet_HttpParserUtilAddField&);

  public:
    // CREATORS
    explicit baenet_HttpParserUtilAddField(const bdeut_StringRef *fieldValue);
        // Construct a visitor that will add the specified 'fieldValue' to the
        // visited field.

    // ~baenet_HttpParserUtilAddField();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *object);
        // Add the associated field value to the specified object.  Return 0 on
        // success, and a non-zero value otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // struct baenet_HttpParserUtil
                        // ----------------------------

// PRIVATE FUNCTIONS

template <typename TYPE>
inline
int baenet_HttpParserUtil::parseFieldValueImp(TYPE                   *result,
                                              const bdeut_StringRef&  str,
                                              bdeat_TypeCategory::Enumeration)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    if (0 != bdeat_EnumFunctions::fromString(result,
                                             str.data(),
                                             str.length())) {
        return BAENET_FAILURE;
    }

    return BAENET_SUCCESS;
}

// FUNCTIONS

template <typename TYPE>
inline
int baenet_HttpParserUtil::parseFieldValue(TYPE                   *result,
                                           const bdeut_StringRef&  str)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return parseFieldValueImp(result, str, TypeCategory());
}

                    // -----------------------------------
                    // class baenet_HttpParserUtilAddField
                    // -----------------------------------

// PRIVATE MANIPULATORS

template <typename TYPE>
inline
int baenet_HttpParserUtilAddField::appendElementToArray(
                                                TYPE                   *object,
                                                const bdeut_StringRef&  str)
{
    const int i = static_cast<int>(bdeat_ArrayFunctions::size(*object));

    bdeat_ArrayFunctions::resize(object, i + 1);

    baenet_HttpParserUtilAddField loadElement(&str);

    return bdeat_ArrayFunctions::manipulateElement(object, loadElement, i);
}

template <typename TYPE>
int baenet_HttpParserUtilAddField::execute(TYPE *object,
                                           bdeat_TypeCategory::Array)
{
    enum { FAILURE = -1 };

    const char *begin = d_fieldValue_p->begin();
    const char *p     = begin;
    const char *end   = d_fieldValue_p->end();

    while (p != end) {
        if ('(' == *p) {
            baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);
        }
        else if ('\"' == *p) {
            baenet_HttpParserUtil::skipQuotedString(&p, end);
        }
        else if ('[' == *p) {
            baenet_HttpParserUtil::skipDomainLiteral(&p, end);
        }
        else if (',' == *p) {
            bdeut_StringRef element(begin, p);

            if (0 != appendElementToArray(object, element)) {
                return FAILURE;
            }

            ++p;
            begin = p;
        }
        else {
            ++p;
        }
    }

    bdeut_StringRef element(begin, p);

    return appendElementToArray(object, element);
}

template <typename TYPE>
inline
int baenet_HttpParserUtilAddField::execute(TYPE *object,
                                           bdeat_TypeCategory::NullableValue)
{
    bdeat_NullableValueFunctions::makeValue(object);
    return bdeat_NullableValueFunctions::manipulateValue(object, *this);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baenet_HttpParserUtilAddField::execute(TYPE *object, ANY_CATEGORY)
{
    return baenet_HttpParserUtil::parseFieldValue(object, *d_fieldValue_p);
}

// CREATORS

inline
baenet_HttpParserUtilAddField::baenet_HttpParserUtilAddField(
                                             const bdeut_StringRef *fieldValue)
: d_fieldValue_p(fieldValue)
{
}

// MANIPULATORS

template <typename TYPE>
inline
int baenet_HttpParserUtilAddField::operator()(TYPE *object)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return execute(object, TypeCategory());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
