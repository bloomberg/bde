#ifndef INCLUDED_BALJSN_JSONTOKENIZER
#define INCLUDED_BALJSN_JSONTOKENIZER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tokenizer for viewing parts of a `bdljsn::Json` object.
//
//@CLASSES:
//  baljsn::JsonTokenizer: tokenizer for viewing parts of a `bdljsn::Json`
//
//@SEE_ALSO: baljsn_decoder, baljsn_tokenizer, baljsn_converter
//
//@DESCRIPTION: This component provides a class, `baljsn::JsonTokenizer`, that
// iterates through the structure of a `bdljsn::Json` object and provides
// Json type and (for some types) value data for each subordinate
// `bdljsn::Json` object.  The top-level `bdljsn::Json` object must be of
// either `bdljsn::JsonArray` or  `bdldjn::JsonObject` type.  Both of those
// types can contain subordinate objects that are themselves either arrays or
// objects or any of the four scalar types.  The structure is traversed in
// depth-first order.
//
// The set of tokens returned by this tokenizer is isomorphic with the tokens
// returned by `baljsn_tokenizer` for use by `baljsn_decoder`.  The only
// significant difference is that the latter tokenizer always returns values as
// `bsl::string_view`s whereas this tokenizer returns `bsl::string_view` for
// element names (the name portion of a `bdljsn::JsonObject` member) but the
// value of an element value (the value portion of a `bdljsn::JsonObject`
// member or an element of a `bdljsn::JsonArray) is returned as a
// `const bdljsn::Json *`.
//
///Tokens
///------
// The basic usage pattern for this tokenizer is to iteratively call
// the `advanceToNextToken` method.  If the return value is 0 (success)
// the enumerated token value can be obtained by calling the `tokenType`
// method. `advanceToNextToken` fails when the tokenizer has been advanced
// past the last token.
//
// The following table lists describes the set of enumerated values that
// can be returned by this tokenizer.
// ```
// +-----------------+--------------------+-----------------------------+
// |Enumerator       |Description         | Value                       |
// +-----------------+--------------------+-----------------------------+
// |e_BEGIN          |starting token      | none                        |
// |e_ELEMENT_NAME   |element name        | value(bsl::string_view *)   |
// |e_START_OBJECT   |start of an object  | none                        |
// |e_END_OBJECT     |end of an object    | none                        |
// |e_START_ARRAY    |start of an array   | none                        |
// |e_END_ARRAY      |end of an array     | none                        |
// |e_ELEMENT_VALUE  |scalar element value| value(const bdljsn::Json **)|
// |e_ERROR          |error token         | none                        |
// +-----------------+--------------------+-----------------------------+
// ```
// Note that `e_BEGIN` is *never* returned by `tokenType`. That token type is
// the "prior" token that is discarded in the first call to
// `advanceToNextToken` after construction or `reset`.
//
// Notice that two of the enumerated values -- `e_ELEMENT_NAME` and
// `e_ELEMENT_VALUE` -- have additional information that can be obtained via
// the appropriate `value` overload.
//
///JSON Object Members
///- - - - - - - - - -
// Recall that JSON Objects consist of sequences of name/value pairs called
// "members". 'e_ELEMENT_NAME` is returned when the tokenizer is positioned to
// the name portion of a member and `e_ELEMENT_VALUE` is returned when
// the tokenizer is advanced to the associated scalar value.  The appropriate
// `value` overload is used to get the actual member name (a
// `bsl::string_view`) and member value (a pointer to a `bdljsn::Json` object).
//
// Also recall that the value portion of a member need not be scalar. Those
// can also consist of a subordinate JSON object or JSON array, in whice case,
// `e_START_OBJECT` or `e_START_ARRAY`, repsectively, are returned.
//
///JSON Array Elements
///- - - - - - - - - -
// `e_ELEMENT_VALUE` is also returned for each scalar element when
// the tokenizer is advancing through an array and `value` is invoked to
// get the value of that element. If the element is non-scalar -- i.e.,
// a subordinate object or array -- the appropriate `e_START_*` token is
// returned.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
///----------------------
// The `baljsn::JsonTokenizer` allows the user to iteratively examine each of
// the piece-parts of a `bdljsn::Json` object having arbitrary complexity
// --- containing subordinate `JsonArray`s/`JsonObject`s --- as well as the
// scalar `Json` types.  Suppose one must visualize a `bdljsn::Json` object
// having modest complexity:
// ```
// { "name"        : "Bob",
//   "homeAddress" : { "street" : "Lexington Ave",
//                     "city"   : "New York City",
//                     "state"  : "New York"
//                     },
//   "age"         : 21
// }
// ```
// First, create `JsonDoc`, an ASCII string an representation of the above JSON
// document (having required escape sequences and quoting) -- elided.
// ```
// Then, create `json`, the programmatic representation of the JSON document.
// ```
//  bdljsn::Json json;
//  int          rc = bdljsn::JsonUtil::read(&json, JsonDoc);
//  assert(0 == rc);
// ```
// Next, define two macros and an array of indentation strings to simplify the
// formatting code.
// ```
//  const char *const indents[] = { ""   // Four spaces per level.
//                                , "    "
//                                , "        "
//                                , "                "
//                                };
//
//  #define PRINT(L, X) bsl::cout << indents[L] << (X) << bsl::endl;
//
//  #define PRINTV(L, X, V) bsl::cout            \$
//                          << indents[L]        \$
//                          << (X) << ": "       \$
//                          << (V) << bsl::endl;
//
//  int level = 0; // indentation level
// ```
// Then, create `tokenizer`, a baljsn::JsonTokenizer`, that will generate
// a sequence of tokens (and sometimes values as well) for the previously
// initialized `json` object.  
// ```
//  baljsn::JsonTokenizer tokenizer(&json);
//
//  while (0 == tokenizer.advanceToNextToken()) {
// ```
// Now, for each token in the sequence, dispatch to the appropriate action.
// In each case, we output the value of the token at the current level of
// indentation.
//
// For tokens that indicate the start of an `JsonArray` or `JsonObject`, we
// increase the level of indentation.  That level is decreased when the end of
// the `JsonArray` or `JsonObject` is reported.
//
// We expect two tokens to be generated for each member of a `JsonObject`: the
// element name and an associated value.  In such cases we use the `value`
// method of extract the name (provided via `bsl::string_view`) and the value
// (itself a `bdljsn::Json` object).
//
// Be aware that a `e_ELEMENT_VALUE` is also returned for each scalar element
// of a `JsonArray` -- not featured in this example.
// ```
//      baljsn::JsonTokenizer::TokenType tokenType = tokenizer.tokenType();
//
//      switch(tokenType) {
//          case baljsn::JsonTokenizer::e_START_ARRAY:   {
//              PRINT(level, tokenType);
//              ++level;
//          } break;
//          case baljsn::JsonTokenizer::e_END_ARRAY:     {
//              --level;
//              PRINT(level, tokenType);
//          } break;
//          case baljsn::JsonTokenizer::e_START_OBJECT:  {
//              PRINT(level, tokenType);
//              ++level;
//          } break;
//          case baljsn::JsonTokenizer::e_END_OBJECT:    {
//              --level;
//              PRINT(level, tokenType);
//          } break;
//          case baljsn::JsonTokenizer::e_ELEMENT_NAME:  {
//              bsl::string_view sv;
//              int              rc = tokenizer.value(&sv);
//              assert(0 == rc);
//
//              PRINTV(level, tokenType, sv);
//          } break;
//          case baljsn::JsonTokenizer::e_ELEMENT_VALUE: {
//              const bdljsn::Json *jp;
//              int                 rc = tokenizer.value(&jp);
//              assert(0 == rc);
//              assert( jp);
//              assert(!jp->isArray());
//              assert(!jp->isObject());
//
//              PRINTV(level, tokenType, *jp);
//          } break;
//          case baljsn::JsonTokenizer::e_ERROR:         {
//              PRINT(level, tokenType);
//          } break;
//          default: {
//              assert(false && !"reachable");
//          }
//      }
//  }
// ```
// Notice, that the `e_ELEMENT_VALUE` is always associated with a scalar
// `bdljsn::Json` object.  If the value portion of a member is itself an
// `JsonArray` (or `JsonObject`) then expect `e_START_ARRAY` (or
// `e_START_OBJECT`) at that point.
//
// Finally, we can inspect the output for this `bdljsn::Json` object:
// ```
//  START_OBJECT
//      ELEMENT_NAME: homeAddress
//      START_OBJECT
//          ELEMENT_NAME: state
//          ELEMENT_VALUE: "New York"
//          ELEMENT_NAME: city
//          ELEMENT_VALUE: "New York City"
//          ELEMENT_NAME: street
//          ELEMENT_VALUE: "Lexington Ave"
//      END_OBJECT
//      ELEMENT_NAME: age
//      ELEMENT_VALUE: 21
//      ELEMENT_NAME: name
//      ELEMENT_VALUE: "Bob"
//  END_OBJECT
// ```

#include <bdlscm_version.h>

#include <baljsn_tokenizer.h>  // for interface type  used by `Decoder`.

//#include <bdlma_bufferedsequentialallocator.h>

#include <bdljsn_json.h>  // `bdljsn::Json`,
                          // `bdljsn::JsonObject`, and
                          // `bdljsn::JsonArray`
#include <bdljsn_jsonnull.h>
#include <bdljsn_jsonnumber.h>

#include <bdlb_variant.h>

#include <bslma_aatypeutil.h>
#include <bslma_allocatorutil.h>
#include <bslma_bslallocator.h>

#include <bsla_unreachable.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_deque.h>
#include <bsl_ios.h>
#include <bsl_stack.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

#include <bsl_iomanip.h>  // for 'printTokens'

namespace BloombergLP {
namespace baljsn {

class JsonTokenizer;

                            // =====================
                            // JsonTokenizer_Visitor
                            // =====================

/// This class provides a mechanism for extracting token information from a
/// `bdljsn::Json` object, transferring that information to a parent
/// `baljsn::JsonTokenizer` object (which considers this visitor a `friend`),
/// and, if appropriate, arranging for the visiting of a subordinate
/// `bdljsn::Json` object.
class JsonTokenizer_Visitor {

    // DATA
    JsonTokenizer *d_tokenizer_p;  // parent object (held, not owned)

    // PRIVATE TYPES
    typedef JsonTokenizer JT;

  public:
    // CREATORS

    /// Create a `JsonTokenizer_Visitor` object that, when used via the `visit`
    /// method of a `bdljsn::Json` object, transfers token information to the
    /// specified `tokenizer`.
    explicit JsonTokenizer_Visitor(JsonTokenizer *tokenizer);

    /// Destroy this `JsonTokenizer_Visitor` object.
    ~JsonTokenizer_Visitor();

    // ACCESSORS
    void operator()(const bdljsn::JsonObject&  object ) const;
    void operator()(const bdljsn::JsonArray&   array  ) const;

    // No-ops: 'bsl::string', `JsonNumber`, `bool`, and `JsonNull`
    template <class TYPE>
    void operator()(const TYPE& parameter) const;
};

                        // ===================
                        // class JsonTokenizer
                        // ===================

/// This class implements a mechanism that makes the organization and values of
/// a non-scalar `bdljsn::Json` object available as a series of tokens.  Some
/// tokens have associated values.  See [](#Tokens).
class JsonTokenizer {

  public:

    // TYPES

    enum TokenType {
      e_BEGIN = 1      // starting token
    , e_ELEMENT_NAME   // element name
    , e_START_OBJECT   // start of an object
    , e_END_OBJECT     // end of an object
    , e_START_ARRAY    // start of an array
    , e_END_ARRAY      // end of an array
    , e_ELEMENT_VALUE  // element value of a simple (scalar) type
    , e_ERROR          // error token
    };

    typedef bsl::allocator<> allocator_type;

  private:
    // PRIVATE TYPES
    typedef bsl::string_view    ElementName;
    typedef const bdljsn::Json *ElementValue;

    typedef bdlb::Variant <ElementName, ElementValue> TokenValue;

    struct Token {
        TokenType  d_tokenType;   // Token type.
        TokenValue d_tokenValue;  // Array element, member name, member value,
                                  // or unset -- depending on `d_tokenType`.
    };

    enum State {
      e_JSON_START = 0  // at the start of an `JsonArray` or `JsonObject`
    , e_JSON_BODY       // at an interior array element or object member
    , e_JSON_END        // at the end of the  `JsonArray` or `JsonObject`
    };

    typedef bdljsn::JsonArray ::ConstIterator  ElementItr;
    typedef bdljsn::JsonObject::ConstIterator   MemberItr;

    typedef bdlb::Variant <ElementItr, MemberItr> Position;

    struct Context {
        State               d_state;    // beginging/middle/end of non-scalar
        const bdljsn::Json *d_json_p;   // the non-scalar `Json` being examined
        Position            d_position; // position within the non-scalar
    };

    typedef bsl::deque<Token>   TokenList;
    typedef bsl::stack<Context> ContextStack;

    TokenList             d_tokenList;    // Token(s) staged for `tokenType()`.
                                          // Note: List size is 2 after
                                          // processing an `JsonObject` member
                                          // an 1 otherwise.

    ContextStack          d_contextStack; // State of examination of the
                                          // specified `Json` object.

    JsonTokenizer_Visitor d_visitor;      // Visitor object for extracting
                                          // token information.

    // DATA
    static const Token s_BEGIN;        // Preformed token for `tokenType()`.
    static const Token s_START_OBJECT; // "                                "
    static const Token s_END_OBJECT;   // "                                "
    static const Token s_START_ARRAY;  // "                                "
    static const Token s_END_ARRAY;    // "                                "
    static const Token s_ERROR;        // "                                "

    // PRIVATE MANIPULATORS
    int getNextToken();

    // PRIVATE ACCESSORS
    void printTokens() const;

    // FRIENDS
    friend class JsonTokenizer_Visitor;

    // NOT IMPLEMENTED
    JsonTokenizer(const JsonTokenizer&);             // = delete;
    JsonTokenizer& operator=(const JsonTokenizer&);  // = delete

  public:

    // CLASS METHODS

    /// Write the string representation of the specified enumeration `value`
    /// to the specified output `stream`, and return a reference to
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  See `toAscii` for
    /// what constitutes the string representation of a
    /// `JsonTokenizer::TokenType` value.
    static bsl::ostream& print(bsl::ostream& stream,
                               TokenType     value,
                               int           level          = 0,
                               int           spacesPerLevel = 4);

    /// Return the non-modifiable string representation corresponding to the
    /// specified enumeration `value`, if it exists, and a unique (error)
    /// string otherwise.  The string representation of `value` matches its
    /// corresponding enumerator name with the "e_" prefix elided.  For
    /// example:
    /// ```
    /// bsl::cout << DstPolicy::toAscii(JsonTokenizer::e_ELEMENT_VALUE);
    /// ```
    /// will print the following on standard output:
    /// ```
    /// ELEMENT_VALUE
    /// ```
    /// Note that specifying a `value` that does not match any of the
    /// enumerators will result in a string representation that is distinct
    /// from any of those corresponding to the enumerators, but is otherwise
    /// unspecified.
    static const char *toAscii(TokenType value);

    // CREATORS

    /// Create a `JsonTokenizer` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply
    /// memory; otherwise, the default allocator is used.  No tokens can
    /// be generated until a source `bdljsn::Json` object is specified
    /// via the `reset` method.
    JsonTokenizer();
    explicit JsonTokenizer(const allocator_type& allocator);

    /// Create a `JsonTokenizer` object that can supply a series of tokens
    /// describing the specified `json` object.  Optionally specify an
    /// `allocator` (e.g., the address of a `bslma::Allocator` object) to
    /// supply memory; otherwise, the default allocator is used.  The behavior
    /// is undefined unless `json->isArray()` or `json->isObject()`.  Note that
    /// `json` can also be specified via the `reset` method.
    JsonTokenizer(const bdljsn::Json    *json,
                  const allocator_type&  allocator = allocator_type());

    /// Destroy this object.
    ~JsonTokenizer();

    // MANIPULATORS

    /// Move to the next token in the source object.  Return 0 on success and a
    /// non-zero value otherwise.  The return value is non-zero when there
    /// are no more tokens in the source object.  The behavior is undefined
    /// unless a source object was specified on construction and/or a call to
    /// `reset`.
    int advanceToNextToken();

    /// Reset this tokenizer to use the specified `json` as a source object
    /// for tokens.  All state associated with a previously specified source
    /// object (if any) is discarded.  The behavior is undefined unless
    /// `json->isArray()` or `json->isObject()`.
    int reset(const bdljsn::Json *json);

    // ACCESSORS

    /// Return the type of the current token.
    TokenType tokenType() const;

    /// Load into the specified `data` the value of the current token.  Return
    /// 0 on success and a non-zero value otherwise.  The behavior is undefined
    /// unless `e_ELEMENT_NAME == tokenType()`.
    int value(bsl::string_view *data) const;

    /// Load into the specified `data` the value of the current token.  Return
    /// 0 on success and a non-zero value otherwise.  The behavior is undefined
    /// unless `e_ELEMENT_VALUE == tokenType()`.
    int value(const bdljsn::Json **data) const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;
};

// FREE OPERATORS

/// Write the string representation of the specified enumeration `value` to
/// the specified output `stream` in a single-line format, and return a
/// reference to `stream`.  See `toAscii` for what constitutes the string
/// representation of a `baljsn::JsonTokenizer::TokenType` value.  Note that
/// this method has the same behavior as
/// ```
/// baljsn::JsonTokenizer::print(stream, value, 0, -1);
/// ```
bsl::ostream& operator<<(bsl::ostream& stream, JsonTokenizer::TokenType value);

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace baljsn {
                            // ---------------------
                            // JsonTokenizer_Visitor
                            // ---------------------

// CREATORS
inline
JsonTokenizer_Visitor::JsonTokenizer_Visitor(JsonTokenizer *tokenizer)
: d_tokenizer_p(tokenizer)
{
    BSLS_ASSERT(tokenizer);
}

inline
JsonTokenizer_Visitor::~JsonTokenizer_Visitor()
{
}

// ACCESSORS
template <class TYPE>
inline
void JsonTokenizer_Visitor::operator()(const TYPE& parameter) const
{
    BSLS_ASSERT_OPT(false && "reachable");
    (void) parameter;
}

                        // -------------------
                        // class JsonTokenizer
                        // -------------------

// CREATORS
inline
JsonTokenizer::JsonTokenizer()
: d_tokenList()
, d_contextStack()
, d_visitor(this)
{
}

inline
JsonTokenizer::JsonTokenizer(const allocator_type& allocator)
: d_tokenList   (bslma::AllocatorUtil::adapt(allocator))
, d_contextStack(bslma::AllocatorUtil::adapt(allocator))
, d_visitor(this)
{
}

inline
JsonTokenizer::JsonTokenizer(const bdljsn::Json    *json,
                             const allocator_type&  allocator)
: d_tokenList   (bslma::AllocatorUtil::adapt(allocator))
, d_contextStack(bslma::AllocatorUtil::adapt(allocator))
, d_visitor(this)
{
    BSLS_ASSERT(json);
    BSLS_ASSERT(json->isArray() || json->isObject());

    Context context;
    context.d_json_p = json;
    context.d_state  = json->isArray()  ? e_JSON_START :
                       json->isObject() ? e_JSON_START :
                                          e_JSON_BODY  ;
    d_contextStack.push(context);
    d_tokenList.push_back(JsonTokenizer::s_BEGIN);
}

// MANIPULATORS

inline
int JsonTokenizer::getNextToken()
{
    if (d_contextStack.empty()) {
        return -1;
    }
    Context& context = d_contextStack.top();

    context.d_json_p->visit<void>(d_visitor);

    return 0;
}

// ACCESSORS
inline
JsonTokenizer::TokenType JsonTokenizer::tokenType() const
{
    BSLS_ASSERT_SAFE(!d_tokenList.empty());

    return d_tokenList.front().d_tokenType;
}

inline
int JsonTokenizer::value(bsl::string_view *data) const
{
    BSLS_ASSERT(data);
    BSLS_ASSERT(!d_tokenList.empty());
    BSLS_ASSERT(e_ELEMENT_NAME == d_tokenList.front().d_tokenType);

    *data = d_tokenList.front().d_tokenValue.the<ElementName>();
    return 0;
}

inline
int JsonTokenizer::value(const bdljsn::Json **data) const
{
    BSLS_ASSERT(data);
    BSLS_ASSERT_SAFE(!d_tokenList.empty());
    BSLS_ASSERT(e_ELEMENT_VALUE == d_tokenList.front().d_tokenType);

    *data = d_tokenList.front().d_tokenValue.the<ElementValue>();;
    return 0;
}

                                  // Aspects

inline
JsonTokenizer::allocator_type JsonTokenizer::get_allocator() const
{
    return bslma::AATypeUtil::getAllocatorFromSubobject<allocator_type>(
                                                                  d_tokenList);
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& baljsn::operator<<(bsl::ostream&            stream,
                                 JsonTokenizer::TokenType value)
{
    return JsonTokenizer::print(stream, value, 0, -1);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONTOKENIZER

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
