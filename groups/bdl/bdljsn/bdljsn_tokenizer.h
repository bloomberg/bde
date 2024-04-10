// bdljsn_tokenizer.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLJSN_TOKENIZER
#define INCLUDED_BDLJSN_TOKENIZER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tokenizer for extracting JSON data from a 'streambuf'.
//
//@CLASSES:
//  bdljsn::Tokenizer: tokenizer for parsing JSON data from a 'streambuf'
//
//@SEE_ALSO: baljsn_decoder
//
//@DESCRIPTION: This component provides a class, 'bdljsn::Tokenizer', that
// traverses data stored in a 'bsl::streambuf' one node at a time and provides
// clients access to the data associated with that node, including its type and
// data value.  Client code can use the 'reset' function to associate a
// 'bsl::streambuf' containing JSON data with a tokenizer object and then call
// the 'advanceToNextToken' function to extract individual data values.
//
// This 'class' was created to be used by other components in the 'bdljsn' and
// 'baljsn' packages and in most cases clients should use the
// 'bdljsn_jsonutil', 'baljsn_decoder', or 'bdljsn_datumutil' components
// instead of using this 'class'.
//
// On malformed JSON, tokenization may fail before the end of input is reached,
// but not all such errors are detected.  In particular, callers should check
// that closing brackets and braces match opening ones.
//
///Strict Conformance
///------------------
// The 'bdljsn::Tokenizer' class allows several convenient variances from the
// JSON grammar as described in RFC8259 (see
// https://www.rfc-editor.org/rfc/rfc8259).  If strict conformance is needed,
// users can put the tokenizer into strict conformance mode (see
// 'setConformanceMode').  The behavioral differences are each controlled by
// options.  The differences between a default constructed tokenizer and one in
// strict mode are:
//..
//  Option                           Default  Strict
//  -------------------------------- -------  ------
//  allowConsecutiveSeparators       true     false
//  allowFormFeedAsWhitespace        true     false
//  allowHeterogenousArrays          true     true
//  allowNonUtf8StringLiterals       true     false
//  allowStandAloneValues            true     true
//  allowTrailingTopLevelComma       true     false
//  allowUnescapedControlCharacters  true     false
//..
// The default-constructed 'bdljsn::Tokenizer' is created having the options
// shown above (in the"Default" column) and a 'conformancemode' of
// 'bdljsn::e_RELAXED'.  Accordingly, users are free to change any of the
// option values to any combination that may be needed; however, once a
// tokenizer is set to strict mode the options are set to the values shown
// above (in the "Strict" column) and changes are not allowed (doing so leads
// to undefined behavior) unless the conformance mode is again set to relaxed.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Extracting JSON Data into an Object
///----------------------------------------------
// For this example, we will use 'bdljsn::Tokenizer' to read each node in a
// JSON document and populate a simple 'Employee' object.
//
// First, we will define the JSON data that the tokenizer will traverse over:
//..
//  const char *INPUT = "    {\n"
//                      "        \"street\" : \"Lexington Ave\",\n"
//                      "        \"state\" : \"New York\",\n"
//                      "        \"zipcode\" : \"10022-1331\",\n"
//                      "        \"floorCount\" : 55\n"
//                      "    }";
//..
// Next, we will construct populate a 'streambuf' with this data:
//..
//  bdlsb::FixedMemInStreamBuf isb(INPUT, bsl::strlen(INPUT));
//..
// Then, we will create a 'bdljsn::Tokenizer' object and associate the above
// streambuf with it:
//..
//  bdljsn::Tokenizer tokenizer;
//  tokenizer.reset(&isb);
//..
// Next, we will create an address record type and object.
//..
//  struct Address {
//      bsl::string d_street;
//      bsl::string d_state;
//      bsl::string d_zipcode;
//      int         d_floorCount;
//  } address = { "", "", "", 0 };
//..
// Then, we will traverse the JSON data one node at a time:
//..
//  // Read '{'
//
//  int rc = tokenizer.advanceToNextToken();
//  assert(!rc);
//
//  bdljsn::Tokenizer::TokenType token = tokenizer.tokenType();
//  assert(bdljsn::Tokenizer::e_START_OBJECT == token);
//
//  rc = tokenizer.advanceToNextToken();
//  assert(!rc);
//  token = tokenizer.tokenType();
//
//  // Continue reading elements till '}' is encountered
//
//  while (bdljsn::Tokenizer::e_END_OBJECT != token) {
//      assert(bdljsn::Tokenizer::e_ELEMENT_NAME == token);
//
//      // Read element name
//
//      bslstl::StringRef nodeValue;
//      rc = tokenizer.value(&nodeValue);
//      assert(!rc);
//
//      bsl::string elementName = nodeValue;
//
//      // Read element value
//
//      int rc = tokenizer.advanceToNextToken();
//      assert(!rc);
//
//      token = tokenizer.tokenType();
//      assert(bdljsn::Tokenizer::e_ELEMENT_VALUE == token);
//
//      rc = tokenizer.value(&nodeValue);
//      assert(!rc);
//
//      // Extract the simple type with the data
//
//      if (elementName == "street") {
//          rc = bdljsn::StringUtil::readString(&address.d_street, nodeValue);
//          assert(!rc);
//      }
//      else if (elementName == "state") {
//          rc = bdljsn::StringUtil::readString(&address.d_state, nodeValue);
//          assert(!rc);
//      }
//      else if (elementName == "zipcode") {
//          rc = bdljsn::StringUtil::readString(&address.d_zipcode, nodeValue);
//          assert(!rc);
//      }
//      else if (elementName == "floorCount") {
//          rc = bdljsn::NumberUtil::asInt(&address.d_floorCount, nodeValue);
//          assert(!rc);
//      }
//
//      rc = tokenizer.advanceToNextToken();
//      assert(!rc);
//      token = tokenizer.tokenType();
//  }
//..
// Finally, we will verify that the 'address' aggregate has the correct values:
//..
//  assert("Lexington Ave" == address.d_street);
//  assert("New York"      == address.d_state);
//  assert("10022-1331"    == address.d_zipcode);
//  assert(55              == address.d_floorCount);
//..

#include <bdlscm_version.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_ios.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdljsn {

                              // ===============
                              // class Tokenizer
                              // ===============

class Tokenizer {
    // This 'class' provides a mechanism for traversing JSON data stored in a
    // 'bsl::streambuf' one node at a time and allows clients to access the
    // data associated with that node, including its type and data value.

  public:
    // TYPES
    typedef bsls::Types::IntPtr IntPtr;
    typedef bsls::Types::Uint64 Uint64;

    enum TokenType {
        // This 'enum' lists all the possible token types.

        e_BEGIN = 1,      // starting token
        e_ELEMENT_NAME,   // element name
        e_START_OBJECT,   // start of an object ('{')
        e_END_OBJECT,     // end of an object   ('}')
        e_START_ARRAY,    // start of an array  ('[')
        e_END_ARRAY,      // end of an array    (']')
        e_ELEMENT_VALUE,  // element value of a simple type
        e_ERROR           // error token
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ,
        BAEJSN_ELEMENT_NAME  = e_ELEMENT_NAME,
        BAEJSN_START_OBJECT  = e_START_OBJECT,
        BAEJSN_END_OBJECT    = e_END_OBJECT,
        BAEJSN_START_ARRAY   = e_START_ARRAY,
        BAEJSN_END_ARRAY     = e_END_ARRAY,
        BAEJSN_ELEMENT_VALUE = e_ELEMENT_VALUE,
        BAEJSN_ERROR         = e_ERROR
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum { k_EOF = +1 };

    enum ConformanceMode {
        e_RELAXED = 0,
        e_STRICT_20240119
    };

  private:
    // PRIVATE TYPES
    enum ContextType {
        // This 'enum' lists the possible contexts that the tokenizer can be
        // in.

        e_NO_CONTEXT,                 // context stack is empty
        e_OBJECT_CONTEXT,             // object context
        e_ARRAY_CONTEXT               // array context
    };

    // One intermediate data buffer used for reading data from the stream, and
    // another for the context state stack.

    enum {
        k_BUFSIZE             = 1024 * 8,
        k_MAX_STRING_SIZE     = k_BUFSIZE - 1,

        k_CONTEXTSTACKBUFSIZE = 256
    };

    // DATA
    bsls::AlignedBuffer<k_BUFSIZE>
                        d_buffer;           // string buffer

    bsls::AlignedBuffer<k_CONTEXTSTACKBUFSIZE>
                        d_stackBuffer;      // context stack buffer

    bdlma::BufferedSequentialAllocator
                        d_allocator;        // string allocator (owned)

    bdlma::BufferedSequentialAllocator
                        d_stackAllocator;   // context stack allocator (owned)

    bsl::string         d_stringBuffer;     // string buffer

    bsl::streambuf     *d_streambuf_p;      // streambuf (held, not owned)

    bsl::size_t         d_cursor;           // current cursor

    bsl::size_t         d_valueBegin;       // cursor for beginning of value

    bsl::size_t         d_valueEnd;         // cursor for end of value

    bsl::size_t         d_valueIter;        // cursor for iterating value

    Uint64              d_readOffset;       // the offset to the end of the
                                            // current 'd_stringBuffer'
                                            // relative to the start of the
                                            // streambuf

    TokenType           d_tokenType;        // token type

    bsl::vector<char>   d_contextStack;     // context type stack

    int                 d_readStatus;       // 0 until EOF or an error is
                                            // encountered, then indicates
                                            // nature of error.  Returned by
                                            // 'readStatus'

    int                 d_bufEndStatus;     // status of last read from
                                            // '*d_streambuf_p'.  If non-zero,
                                            // copied to 'd_readStatus' on next
                                            // read attempt.

    bool                d_allowConsecutiveSeparators;
                                            // option for allowing consecutive
                                            // separators (i.e., ':', or ',')
    
    bool                d_allowFormFeedAsWhitespace;
                                            // option for allowing '\f' as
                                            // whitespace in addition to ' ',
                                            // '\n', '\t', '\r', and '\v'.

    bool                d_allowHeterogenousArrays;
                                            // option for allowing arrays of
                                            // heterogeneous values

    bool                d_allowNonUtf8StringLiterals;
                                            // Disables UTF-8 validation

    bool                d_allowStandAloneValues;
                                            // option for allowing stand alone
                                            // values

    bool                d_allowTrailingTopLevelComma;
                                            // if 'true', allows '{},'

    bool                d_allowUnescapedControlCharacters;
                                            // option for unescaped control
                                            // characters in JSON strings.

    ConformanceMode     d_conformanceMode;  // "relaxed" (default) or "strict"

    // PRIVATE MANIPULATORS
    int expandBufferForLargeValue();
        // Increase the size of the string buffer, 'd_stringBuffer', and then
        // append additional characters, from the internally-held 'streambuf' (
        // 'd_streambuf_p') to the end of the current sequence of characters.
        // Return 0 on success and a non-zero value otherwise.

    int extractStringValue();
        // Extract the string value starting at the current data cursor and
        // update the value begin and end pointers to refer to the begin and
        // end of the extracted string.  Return 0 on success and a non-zero
        // value otherwise.

    int moveValueCharsToStartAndReloadBuffer();
        // Move the current sequence of characters being tokenized to the front
        // of the internal string buffer, 'd_stringBuffer', and then append
        // additional characters, from the internally-held 'streambuf'
        // ('d_streambuf_p') to the end of that sequence up to a maximum
        // sequence length of 'd_buffer.size()' characters.  Return the number
        // of bytes read from the 'streambuf'.  Note that if 0 is returned, it
        // may mean end of file or, if UTF-8 checking is set, that invalid
        // UTF-8 was encountered.

    ContextType popContext();
        // If the 'd_contextStack' is empty, return 'e_NO_CONTEXT', otherwise
        // pop the top context from the 'd_contextStack' stack, and return it.

    void pushContext(ContextType context);
        // Push the specified 'context' onto the 'd_contextStack' stack.

    int reloadStringBuffer();
        // Reload the string buffer with new data read from the underlying
        // 'streambuf' and overwriting the current buffer.  After reading
        // update the cursor to the new read location.  Return the number of
        // bytes read from the 'streambuf'.

    int skipNonWhitespaceOrTillToken();
        // Skip all characters until a whitespace or a token character is
        // encountered and position the cursor onto the first such character.
        // Return 0 on success and a non-zero value otherwise.

    int skipWhitespace();
        // Skip all whitespace characters and position the cursor onto the
        // first non-whitespace character.  Return 0 on success and a non-zero
        // value otherwise.

    // PRIVATE ACCESSOR
    ContextType context() const;
        // If the 'd_contextStack' is empty, return 'e_NO_CONTEXT', otherwise
        // return the top context from the 'd_contextStack' stack without
        // popping.

  private:
    // NOT IMPLEMENTED
    Tokenizer(const Tokenizer&);
    Tokenizer& operator=(const Tokenizer&);

  public:
    // CREATORS
    explicit Tokenizer(bslma::Allocator *basicAllocator = 0);
        // Create a 'Tokenizer' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  By default, the
        // 'conformanceMode' is 'e_RELAXED' and the value of the 'Tokenizer'
        // options are:
        //..
        //  allowConsecutiveSeparators()      == true;
        //  allowFormFeedAsWhitespace()       == true;
        //  allowHeterogeneousArrays()        == true;
        //  allowNonUtf8StringLiterals()      == true;
        //  allowStandAloneValues()           == true;
        //  allowTrailingTopLevelComma()      == true;
        //  allowUnescapedControlCharacters() == true;
        //..
        // The 'reset' method must be called before any calls to
        // 'advanceToNextToken' or 'resetStreamBufGetPointer'.

    ~Tokenizer();
        // Destroy this object.

    // MANIPULATORS
    int advanceToNextToken();
        // Move to the next token in the data steam.  Return 0 on success and a
        // non-zero value otherwise.  Each call to 'advanceToNextToken'
        // invalidates the string references returned by the 'value' accessor
        // for prior nodes.  This function *may* fail to move to the next token
        // if doing so would advanced past a character sequence that is not
        // valid JSON, and is guaranteed to do so (fail to move) if
        // 'e_RELAXED != conformanceMode()'.  The behavior is undefined unless
        // 'reset' has been called.

    void reset(bsl::streambuf *streambuf);
        // Reset this tokenizer to read data from the specified 'streambuf'.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextToken' is called.  Note that this function does not
        // change the the 'conformanceMode' nor the values of any of the
        // individual token options:
        //: o 'allowConsecutiveSeparators'
        //: o 'allowFormFeedAsWhitespace'
        //: o 'allowHeterogenousArrays'
        //: o 'allowNonUtf8StringLiterals'
        //: o 'allowStandAloneValues'
        //: o 'allowTrailingTopLevelComma'
        //: o 'allowUnescapedControlCharacters'

    int resetStreamBufGetPointer();
        // Reset the get pointer of the 'streambuf' held by this object to
        // refer to the byte following the last processed byte, if the held
        // 'streambuf' supports seeking, and return an error otherwise leaving
        // this object unchanged.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless 'reset' has been
        // called.  Note that after a successful function return users can read
        // data from the 'streambuf' that was specified during 'reset' from
        // where this object stopped.  Also note that this call implies the end
        // of processing for this object and any subsequent methods invoked on
        // this object should only be done after calling 'reset' and specifying
        // a new 'streambuf'.

    Tokenizer& setAllowConsecutiveSeparators(bool value);
        // Set the 'allowConsecutiveSeparators' option to the specified
        // 'value' and return a non-'const' reference to this tokenizer.  JSON
        // defines two separator tokens: the colon (':') and the comma (',').
        // If the 'allowConsecutiveSeparartors' value is 'true' this tokenizer
        // will accept multiple consecutive sequences of a given separator
        // (e.g., '"a"::b, "c":::d' and '"a":b,, "c":d', ,, "e":f') as if a
        // single separator had appeared (i.e., '"a":b, "c":d' and
        // '"a":b, "c":d', "e":f', respectively).  Otherwise the tokenizer
        // returns an error when multiple consecutive colons are found.  By
        // default, the value of the 'allo ConsecutiveSeparators' option is
        // 'true'.  The behavior is undefined unless
        // 'e_RELAXED == conformanceMode()'.  Note that consecutive sequences
        // using both tokens (e.g., '::,,::') is always an error.

    Tokenizer& setAllowFormFeedAsWhitespace(bool value);
        // Set the 'allowFormFeedAsWhitespace' option to the specifiedd value
        // and return a non-'const' reference to this tokenizer.  If the
        // 'allowFormFeedAsWhitespace' value is 'true' the formfeed character
        // ('\f') is recognized as a whitespace character in addition to '\n',
        // '\t', '\r', and '\v'.  Otherwise, formfeed is diallowed a
        // whitewpace.

    Tokenizer& setAllowHeterogenousArrays(bool value);
        // Set the 'allowHeterogenousArrays' option to the specified 'value'
        // and return a non-'const' reference to this tokenizer.  If the
        // 'allowHeterogenousArrays' value is 'true' this tokenizer will
        // successfully tokenize heterogeneous values within an array.  If the
        // option's value is 'false' then the tokenizer will return an error
        // for arrays having heterogeneous values.  By default, the value of
        // the 'allowHeterogenousArrays' option is 'true'.  The behavior is
        // undefined unless 'e_RELAXED == conformanceMode()'.

    Tokenizer& setAllowNonUtf8StringLiterals(bool value);
        // Set the 'allowNonUtf8StringLiterals' option to the specified 'value'
        // and return a non-'const' reference to this tokenizer.  If the
        // 'allowNonUtf8StringLiterals' value is 'false' this tokenizer will
        // check string literal tokens for invalid UTF-8, enter an error mode
        // if it encounters a string literal token that has any content that is
        // not UTF-8, and fail to advance to subsequent tokens until 'reset' is
        // called.  By default, the value of the 'allowNonUtf8StringLiterals'
        // option is 'true'.  The behavior is undefined unless
        // 'e_RELAXED == conformanceMode()'.

    Tokenizer& setAllowStandAloneValues(bool value);
        // Set the 'allowStandAloneValues' option to the specified 'value' and
        // return a non-'const' reference to this tokenizer.  If the
        // 'allowStandAloneValues' value is 'true' this tokenizer will
        // successfully tokenize JSON values (strings and numbers).  If the
        // option's value is 'false' then the tokenizer will only tokenize
        // complete JSON documents (JSON objects and arrays) and return an
        // error for stand alone JSON values.  By default, the value of the
        // 'allowStandAloneValues' option is 'true'.  The behavior is undefined
        // unless 'e_RELAXED == conformanceMode()'.

    Tokenizer& setAllowTrailingTopLevelComma(bool value);
        // Set the 'allowTrailingTopLevelComma' option to the specified 'value'
        // and return a non-'const' reference to this tokenizer.  If the
        // 'allowTrailingTopLevelComma' value is 'true' this tokenizer will
        // successfully tokenize JSON values where a comma follows the
        // top-level JSON element.  If the option's value is 'false' then the
        // tokenizer will reject documents with such trailing commas, such as
        // '{},'.  By default, the value of the 'allowTrailingTopLevelComma'
        // option is 'true' for backwards compatibility.  Note that a document
        // without any JSON elements is invalid whether or not it contains
        // commas.  The behavior is undefined unless
        // 'e_RELAXED == conformanceMode()'.

    Tokenizer& setAllowUnescapedControlCharacters(bool value);
        // Set the 'allowUnescapedControlCharacters' option of this tokenizer
        // to the specified 'value'.  If 'true', characters in the range
        // '[ 0x00 .. 0x1F ]' are allowed in JSON strings.  If the option is
        // 'false', these characters must be represented by their six byte
        // escape sequences '[ \u0000 .. \u001F ]'.  Several values in that
        // range are also (conveniently) represented by two byte sequences:
        //..
        //  \"    quotation mark
        //  \\    reverse solidus
        //  \/    solidus
        //  \b    backspace
        //  \f    form feed
        //  \n    line feed
        //  \r    carriage return
        //  \t    tab
        //..
        // The 'DEL' control character ('0x7F') is accepted even in strict
        // mode.
        //
        // The behavior is undefined unless 'e_RELAXED == conformanceMode()'.
        // Note that the representation of these byte sequences as C/C++ string
        // literals requires that the escape character itself must be escaped:
        //..
        //  "Hello,\\tworld\\n";  // Can alwas initialize a JSON string with
        //                        // containing tab and a newline
        //                        // escape sequences
        //                        // whether the option is set or not.
        //
        //  "Hello,\tworld\n";    // When this option is 'true'.
        //                        // can also initialize a JSON string
        //                        // with an actual and newline characters.
        //..
        // Also note that the two resulting strings do *not* compare equal. 

    Tokenizer& setConformanceMode(ConformanceMode mode);
        // Set the 'conformanceMode' of this tokenizer to the specified 'mode'
        // and return a non-'const' reference to this tokenizer.  If 'mode' is
        // 'e_STRICT_20240119' the option values of this tokenizer are set to
        // be fully compliant with RFC8259 (see
        // https://www.rfc-editor.org/rfc/rfc8259)
        //
        // Specifically, those option values are:
        //..
        //  allowConsecutiveSeparartor       == false;
        //  allowFormFeedAsWhitespace()      == false;
        //  allowHeterogeneousArrays()       == true;
        //  allowNonUtf8StringLiterals()     == false;
        //  allowStandAloneValues()          == true;
        //  allowTrailingTopLevelComma()     == false;
        //  allowUnescapedControlCharacters() = false;
        //..
        // Otherwise (i.e., 'mode' is 'e_RELAXED'), those option values can be
        // set in any combination.  Note that the behavior is undefined if
        // individual options are set when 'conformanceMode' is *not*
        // 'e_RELAXED'.

    // ACCESSORS
    bool allowConsecutiveSeparators() const;
        // Return the value of the 'allowConsecutiveSeparators' option of this
        // tokenizer.

    bool allowFormFeedAsWhitespace() const;
        // Return the value of the 'allowFormFeedAsWhitespace' option of this
        // tokenizer.

    bool allowHeterogenousArrays() const;
        // Return the value of the 'allowHeterogenousArrays' option of this
        // tokenizer.

    bool allowNonUtf8StringLiterals() const;
        // Return the value of the 'allowNonUtf8StringLiterals' option of this
        // tokenizer.

    bool allowStandAloneValues() const;
        // Return the value of the 'allowStandAloneValues' option of this
        // tokenizer.

    bool allowTrailingTopLevelComma() const;
        // Return the value of the 'allowTrailingTopLevelComma' option of this
        // tokenizer.

    bool allowUnescapedControlCharacters() const;
        // Return the value of the 'allowUnescapedControlCharacters' option of
        // this tokenizer.

    ConformanceMode conformanceMode() const;
        // Return the 'conformanceMode' of this tokenizer.

    bsls::Types::Uint64 currentPosition() const;
        // Return the offset of the current octet being tokenized in the stream
        // supplied to 'reset', or if an error occurred, the position where the
        // failed attempt to tokenize a token occurred.  Note that this
        // operation is intended to provide additional information in the case
        // of an error.

    bsls::Types::Uint64 readOffset() const;
        // Return the last read position relative to when 'reset' was called.
        // Note that 'readOffset() >= currentPosition()' -- the 'readOffset' is
        // the offset of the last octet read from the stream supplied to
        // 'reset', and is at or beyond the current position being tokenized.

    int readStatus() const;
        // Return the status of the last call to 'reloadStringBuffer()':
        //: o 0 if 'reloadStringBuffer()' has not been called or if a token was
        //:   successfully read.
        //:
        //: o 'k_EOF' (which is positive) if no data could be read before
        //:   reaching EOF.
        //:
        //: o a negative value if the 'allowNonUtf8StringLiterals' option is
        //:   'false' and a UTF-8 error occurred.  The specific value returned
        //:    will be one of the enumerators of the
        //:    'bdlde::Utf8Util::ErrorStatus' 'enum' type indicating the nature
        //:    of the UTF-8 error.

    TokenType tokenType() const;
        // Return the token type of the current token.

    int value(bsl::string_view *data) const;
        // Load into the specified 'data' the value of the specified token if
        // the current token's type is 'e_ELEMENT_NAME' or 'e_ELEMENT_VALUE' or
        // leave 'data' unmodified otherwise.  Return 0 on success and a
        // non-zero value otherwise.  Note that the returned 'data' is only
        // valid until the next manipulator call on this object.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// PRIVATE MANIPULATORS
inline
Tokenizer::ContextType Tokenizer::popContext()
{
    ContextType ret = e_NO_CONTEXT;

    if (!d_contextStack.empty()) {
        ret = static_cast<ContextType>(d_contextStack.back());
        d_contextStack.pop_back();
    }

    return ret;
}

inline
void Tokenizer::pushContext(ContextType context)
{
    d_contextStack.push_back(static_cast<char>(context));
}

// PRIVATE ACCESSOR
inline
Tokenizer::ContextType Tokenizer::context() const
{
    return d_contextStack.empty()
               ? e_NO_CONTEXT
               : static_cast<ContextType>(d_contextStack.back());
}

// CREATORS
inline
Tokenizer::Tokenizer(bslma::Allocator *basicAllocator)
: d_allocator(d_buffer.buffer(), k_BUFSIZE, basicAllocator)
, d_stackAllocator(d_stackBuffer.buffer(),
                   k_CONTEXTSTACKBUFSIZE,
                   basicAllocator)
, d_stringBuffer(&d_allocator)
, d_streambuf_p(0)
, d_cursor(0)
, d_valueBegin(0)
, d_valueEnd(0)
, d_valueIter(0)
, d_readOffset(0)
, d_tokenType(e_BEGIN)
, d_contextStack(200, &d_stackAllocator)
, d_readStatus(0)
, d_bufEndStatus(0)
, d_allowConsecutiveSeparators(true)
, d_allowFormFeedAsWhitespace(true)
, d_allowHeterogenousArrays(true)
, d_allowNonUtf8StringLiterals(true)
, d_allowStandAloneValues(true)
, d_allowTrailingTopLevelComma(true)
, d_allowUnescapedControlCharacters(true)
, d_conformanceMode(e_RELAXED)
{
    d_stringBuffer.reserve(k_MAX_STRING_SIZE);
    d_contextStack.clear();
    pushContext(e_NO_CONTEXT);
}

inline
Tokenizer::~Tokenizer()
{
}

// MANIPULATORS
inline
void Tokenizer::reset(bsl::streambuf *streambuf)
{
    d_streambuf_p  = streambuf;
    d_stringBuffer.clear();
    d_cursor       = 0;
    d_valueBegin   = 0;
    d_valueEnd     = 0;
    d_valueIter    = 0;
    d_readOffset   = 0;
    d_tokenType    = e_BEGIN;
    d_readStatus   = 0;
    d_bufEndStatus = 0;

    d_contextStack.clear();
    pushContext(e_NO_CONTEXT);
}

inline
Tokenizer& Tokenizer::setAllowConsecutiveSeparators(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowConsecutiveSeparators = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowHeterogenousArrays(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowHeterogenousArrays = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowFormFeedAsWhitespace(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowFormFeedAsWhitespace = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowNonUtf8StringLiterals(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowNonUtf8StringLiterals = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowStandAloneValues(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowStandAloneValues = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowTrailingTopLevelComma(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowTrailingTopLevelComma = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setAllowUnescapedControlCharacters(bool value)
{
    BSLS_ASSERT(e_RELAXED == d_conformanceMode);

    d_allowUnescapedControlCharacters = value;
    return *this;
}

inline
Tokenizer& Tokenizer::setConformanceMode(Tokenizer::ConformanceMode mode)
{
    d_conformanceMode = mode;

    switch (mode) {
      case e_RELAXED: {
      } break;
      case e_STRICT_20240119: {
        d_allowConsecutiveSeparators      = false;
        d_allowFormFeedAsWhitespace       = false;
        d_allowHeterogenousArrays         = true;
        d_allowNonUtf8StringLiterals      = false;
        d_allowStandAloneValues           = true;
        d_allowTrailingTopLevelComma      = false;
        d_allowUnescapedControlCharacters = false;
      } break;
      default: {
        BSLS_ASSERT_OPT(!"reached");
      }
    }
    return *this;
}

// ACCESSORS
inline
bool Tokenizer::allowConsecutiveSeparators() const
{
    return d_allowConsecutiveSeparators;
}

inline
bool Tokenizer::allowFormFeedAsWhitespace() const
{
    return d_allowFormFeedAsWhitespace;
}

inline
bool Tokenizer::allowHeterogenousArrays() const
{
    return d_allowHeterogenousArrays;
}

inline
bool Tokenizer::allowNonUtf8StringLiterals() const
{
    return d_allowNonUtf8StringLiterals;
}

inline
bool Tokenizer::allowStandAloneValues() const
{
    return d_allowStandAloneValues;
}

inline
bool Tokenizer::allowTrailingTopLevelComma() const
{
    return d_allowTrailingTopLevelComma;
}

inline
bool Tokenizer::allowUnescapedControlCharacters() const
{
    return d_allowUnescapedControlCharacters;
}

inline
Tokenizer::ConformanceMode Tokenizer::conformanceMode() const
{
    return d_conformanceMode;
}

inline
bsls::Types::Uint64 Tokenizer::currentPosition() const
{
    return d_readOffset - d_stringBuffer.size() + d_cursor;
}

inline
bsls::Types::Uint64 Tokenizer::readOffset() const
{
    return d_readOffset;
}

inline
int Tokenizer::readStatus() const
{
    return d_readStatus;
}

inline
Tokenizer::TokenType Tokenizer::tokenType() const
{
    return d_tokenType;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_TOKENIZER

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
