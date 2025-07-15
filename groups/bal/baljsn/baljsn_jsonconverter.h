// baljsn_jsonconverter.h                                             -*-C++-*-
#ifndef INCLUDED_BALJSN_JSONCONVERTER
#define INCLUDED_BALJSN_JSONCONVERTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide conversions between JSON and `bdlat`-compatible types.
//
//@CLASSES:
// baljsn::JsonConverter: converts between JSON and `bdlat`-compliant types
//
//@SEE_ALSO: baljsn_encoder, baljsn_encodeimplutil, baljsn_jsonformatter,
//           baljsn_jsontokenizer
//
//@DESCRIPTION: This component provides a mechanism, baljsn::JsonConverter, to
// convert from a `bdlat`-compatible object (see [](bdlat)) to a corresponding
// `bdljsn::Json` object, and also to convert back from a `bdlat::Json` object
// to a `bdljsn::Json` object.
//
// The conversion to a `bdljsn::Json` object produces the same result as
// encoding the `bdlat` object to a JSON document using `baljsn::Encoder` and
// then using `bdj::JsonUtil::read` to construct a `bdljsn::Json` object
// from that JSON documents -- however, using `baljsn::convert` avoids the
// creation of that intermediate document.  Conversely, a `bdljsn::Json`
// object could be printed as a JSON document (see `bldjsn::JsonUtil::write`)
// that is decoded into a `bdlat` object (see `baljsn::Decoder`) -- but
// `baljsn::convert` does so directly.
//
///Type Mapping
///------------
// The mapping of `bdlat` and JSON provide different type systems that are
// not entirely congruent.  Notably, several `bdlat` values are converted to
// JSON strings:
//
// * BDE date and time types are represented as JSON strings in ISO 8601
//   format.
//
// * Enumerations are converted to a JSON string formatted as their symbolic
//   (programmatic) representation, not their numeric value.
//
// * Floating point values for INF/-INF/Nan are converted to the strings
//   "+inf"/"-inf"/"nan".
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a `bas_codegen.pl`-generated object into JSON
///-----------------------------------------------------------------
// Consider that we want to exchange an employee's information between two
// processes.  To allow this information exchange we will define the XML schema
// representation for that class, use `bas_codegen.pl` to create the `Employee`
// `class` for storing that information, populate an `Employee` object, and
// encode that object using the baljsn encoder.
//
// First, we will define the XML schema inside a file called `employee.xsd`:
// ```
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
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
// ```
// Then, we will use the `bas_codegen.pl` tool, to generate the C++ classes for
// this schema.  The following command will generate the header and
// implementation files for the all the classes in the `test_messages`
// components in the current directory:
// ```
//  $ bas_codegen.pl -m msg -p test xsdfile.xsd
// ```
// Next, we will populate a `test::Employee` object:
// ```
//  test::Employee employee;
//  employee.name()                 = "Bob";
//  employee.homeAddress().street() = "Lexington Ave";
//  employee.homeAddress().city()   = "New York City";
//  employee.homeAddress().state()  = "New York";
//  employee.age()                  = 21;
// ```
// Then, we will create a `baljsn::JsonConverter` object:
// ```
//  baljsn::JsonConverter converter;
// ```
// Now, we will create a `bdljsn::Json` object having elements that
// match the respecitve elements of `employee`.
// ```
//  bdljsn::Json json;
//  int          rc = converter.convert(&json, employee);
//  assert(0  == rc);
//  assert("" == converter.loggedMessages());
// ```
// Next, we verify that the `json` object has the expected elements, each
// containing the expected value, and having the expected type.
// ```
//  assert(employee.name()                 == json["name"].theString());
//  assert(employee.homeAddress().street() == json["homeAddress"]["street"]
//                                                               .theString());
//  assert(employee.homeAddress().city()   == json["homeAddress"]["city"]
//                                                               .theString());
//  assert(employee.homeAddress().state()  == json["homeAddress"]["state"]
//                                                               .theString());
//  int intValue;                        rc = json["age"]
//                                               .theNumber().asInt(&intValue);
//  assert(0  == rc);
//  assert("" == converter.loggedMessages());
//  assert(employee.age()                  == intValue);
// ```
// Finally, we verify that the `json` object can be converted back to an
// `Employee` object having the same value as the original:
// ```
//  test::Employee employeeFromJson;
//  rc = converter.convert(&employeeFromJson, json);
//  assert(0        == rc);
//  assert(""       == converter.loggedMessages());
//  assert(employee == employeeFromJson);
// ```

#include <balscm_version.h>

#include <baljsn_decoderoptions.h>
#include <baljsn_encodeimplutil.h>
#include <baljsn_encoderoptions.h>
#include <baljsn_jsonformatter.h>
#include <baljsn_jsonparserutil.h>
#include <baljsn_jsontokenizer.h>
#include <baljsn_parserutil.h>

#include <bdlde_base64decoder.h>

#include <bdljsn_json.h>

#include <bdlat_typecategory.h>
#include <bdlat_enumutil.h>

#include <bslma_aatypeutil.h>
#include <bslma_allocatorutil.h>
#include <bslma_bslallocator.h>

#include <bsla_fallthrough.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace baljsn {

struct JsonConverter_ElementVisitor;

                               // ===================
                               // class JsonConverter
                               // ===================

/// This class provides a mechanism for creating `bdljsn::Json` objects
/// having the same values as a given `bdlat`-compatible object.
/// The `convert` method is function template that will
/// accepts any object that meets the requirements of a sequence, choice, or
/// array object as defined in the `bdlat_sequencefunctions`,
/// `bdlat_choicefunctions`, and `bdlat_arrayfunctions` components
/// respectively.
class JsonConverter {

    // PRIVATE TYPES
    typedef JsonTokenizer Tokenizer;

    // DATA
    bsl::ostringstream d_logStream;            // stream used for logging
    Tokenizer          d_tokenizer;            // mechanism to examine a `Json`
    bsl::string        d_elementName;          // current element name
    int                d_maxDepth;             // max decoding depth
    int                d_currentDepth;         // current decoding depth
    bool               d_skipUnknownElements;  // skip unknown elements flag

    // FRIENDS
    friend JsonConverter_ElementVisitor;

    // PRIVATE MANIPULATORS

    /// Log the latest tokenizer error to `d_logStream`.  If the tokenizer
    /// did not have an error, log the specified `alternateString`.  Return
    /// a reference to `d_logStream`.
    bsl::ostream& logTokenizerError(const char *alternateString);

    /// Skip the unknown element specified by `elementName` by discarding
    /// all the data associated with it and advancing the parser to the next
    /// element.  Return 0 on success and a non-zero value otherwise.
    int skipUnknownElement(const bsl::string_view& elementName);

    /// Decode into the specified `value`, of a (template parameter) `TYPE`
    /// corresponding to the specified `bdeat` `category`, the JSON data
    /// currently referred to by the tokenizer owned by this object, using
    /// the specified formatting `mode`.  Return 0 on success and a non-zero
    /// value otherwise.  The behavior is undefined unless `value`
    /// corresponds to the specified `bdeat` category and `mode` is a valid
    /// formatting mode as specified in `bdlat_FormattingMode`.  Note that
    /// `ANY_CATEGORY` shall be a tag-type defined in `bdlat_TypeCategory`.
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::DynamicType );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::Sequence );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::Choice );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::Enumeration );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::CustomizedType );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::Simple );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::Array );
    template <class TYPE>
    int decodeImp(TYPE *value, int mode, bdlat_TypeCategory::NullableValue );
    int decodeImp(bsl::vector<char>         *value,
                  int                        mode,
                  bdlat_TypeCategory::Array );
    template <class TYPE, class ANY_CATEGORY>
    int decodeImp(TYPE *value, ANY_CATEGORY category );

    // NOT IMPLEMENTED
    JsonConverter(const JsonConverter&);             // = delete;
    JsonConverter& operator=(const JsonConverter&);  // = delete

  public:
    // TYPES

    typedef bsl::allocator<> allocator_type;

    // CREATORS

    /// Create a `JsonConverter` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply memory;
    /// otherwise, the default allocator is used.
    JsonConverter();
    explicit JsonConverter(const allocator_type& allocator);

    /// Destroy this object.
    //! ~JsonConverter() = default;

    // MANIPULATORS

    /// Load to the specified `json` the specified `value` of (template
    /// parameter) `TYPE`.  Return 0 on success, and a  non-zero value
    /// otherwise.  `json` set to its default value (`json.isNull()`) before
    /// `value` is loaded.  An error is returned if `TYPE` is neither a
    /// top-level `Sequence` nor a `Choice` nor an `Array`.
    template <class TYPE>
    int convert(bdljsn::Json *json, const TYPE& value);

    /// Load to the specified `value` of (template parameter) `TYPE` the
    /// value of the specified `json` as qualified by the specified `options`.
    /// Return 0 on success and a non-zero value otherwise.
    /// The attributes of `options` are ignored expect for:
    ///
    ///  * `maxDepth`            : maximum depth of the decoded data
    ///                           (512 default)
    ///  * `skipUnknownElements` : allow unknown elements in 'json' to be
    ///                            skipped (default `true`).
    ///
    /// Return 0 on success and a non-zero value otherwise.  An error is
    /// returned if `TYPE` is neither a top-level `Sequence` nor a `Choice`
    /// nor an `Array`.
    template <class TYPE>
    int convert(TYPE                  *value,
                const bdljsn::Json&    json,
                const DecoderOptions&  options = DecoderOptions());

    // ACCESSORS

    /// Return a string containing any error, warning, or trace messages
    /// that were logged during the last call to the `encode` method.  The
    /// log is reset each time `convert` is called.
    bsl::string loggedMessages() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;
};

                       // ===================================
                       // struct JsonConverter_ElementVisitor
                       // ===================================

/// This `class` implements a visitor for decoding elements within a sequence,
/// choice, or array type.  This is a component-private class and should not be
/// used outside of this component.  Note that the operators provided in this
/// `class` match the function signatures required of visitors decoding into
/// elements of compatible types.
struct JsonConverter_ElementVisitor {

    // DATA
    JsonConverter *d_decoder_p;  // converter (held, not owned)
    int            d_mode;       // formatting mode

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // MANIPULATORS

    /// Decode into the specified `value` the data in the JSON format.
    /// Return 0 on success and a non-zero value otherwise.
    template <class TYPE>
    int operator()(TYPE *value);

    /// Decode into the specified `value` using the specified `info` the data
    /// in the JSON format.  Return 0 on success and a non-zero value
    /// otherwise.
    template <class TYPE, class INFO>
    int operator()(TYPE *value, const INFO& info);
};

                       // ====================================
                       // struct JsonConverter_DecodeImpProxy
                       // ====================================

/// This class provides a functor that dispatches the appropriate `decodeImp`
/// method for a `bdeat` Dynamic type.  Note that the operators provided in
/// this `class` match the function signatures required of visitors decoding
/// into compatible types.
struct JsonConverter_DecodeImpProxy {

    // DATA
    JsonConverter *d_decoder_p;  // converter (held, not owned)
    int            d_mode;       // formatting mode

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // MANIPULATORS
    template <class TYPE>
    int operator()(TYPE *, bslmf::Nil);

    /// Dencode into the specified `value` of the specified `bdeat` `category`
    /// from the data in the JSON format.  Return 0 on success and a non-zero
    /// value otherwise.
    template <class TYPE, class ANY_CATEGORY>
    int operator()(TYPE *object, ANY_CATEGORY category);
};
// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                               // -------------------
                               // class JsonConverter
                               // -------------------
// PRIVATE MANIPULATORS

template <class TYPE>
inline
int JsonConverter::decodeImp(TYPE                            *value,
                         int                                  mode,
                         bdlat_TypeCategory::DynamicType      )
{
    JsonConverter_DecodeImpProxy proxy = { this, mode };
    return bdlat_TypeCategoryUtil::manipulateByCategory(value, proxy);
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                         *value,
                             int                           mode,
                             bdlat_TypeCategory::Sequence  )
{
    if (bdlat_FormattingMode::e_UNTAGGED & mode) {
        // This is an anonymous element.  Do not read anything and instead
        // decode into the corresponding sub-element.

        if (bdlat_SequenceFunctions::hasAttribute(
                                   *value,
                                   d_elementName.data(),
                                   static_cast<int>(d_elementName.length()))) {
            JsonConverter_ElementVisitor visitor = { this, mode };

            if (0 != bdlat_SequenceFunctions::manipulateAttribute(
                                   value,
                                   visitor,
                                   d_elementName.data(),
                                   static_cast<int>(d_elementName.length()))) {
                d_logStream << "Could not decode sequence, error decoding "
                            << "element or bad element name '"
                            << d_elementName << "' \n";
                return -1;                                            // RETURN
            }
        }
        else {
            if (d_skipUnknownElements) {
                const int rc = skipUnknownElement(d_elementName);
                if (rc) {
                    d_logStream << "Error reading unknown element '"
                                << d_elementName << "' or after it\n";
                    return -1;                                        // RETURN
                }
            }
            else {
                d_logStream << "Unknown element '" << d_elementName
                            << "' found\n";
                return -1;                                            // RETURN
            }
        }
    }
    else {
        if (++d_currentDepth > d_maxDepth) {
            d_logStream << "Maximum allowed decoding depth reached: "
                        << d_currentDepth << "\n";
            return -1;                                                // RETURN
        }

        if (Tokenizer::e_START_OBJECT != d_tokenizer.tokenType()) {
            d_logStream << "Could not decode sequence, missing starting '{'\n";
            return -1;                                                // RETURN
        }

        int rc = d_tokenizer.advanceToNextToken();
        if (rc) {
            d_logStream << "Could not decode sequence, ";
            logTokenizerError("error") << " reading token after '{'\n";
            return -1;                                                // RETURN
        }

        while (Tokenizer::e_ELEMENT_NAME == d_tokenizer.tokenType()) {

            bslstl::StringRef elementName;
            rc = d_tokenizer.value(&elementName);
            if (rc) {
                d_logStream << "Error reading attribute name after '{'\n";
                return -1;                                            // RETURN
            }

            if (bdlat_SequenceFunctions::hasAttribute(
                                     *value,
                                     elementName.data(),
                                     static_cast<int>(elementName.length()))) {
                d_elementName = elementName;

                rc = d_tokenizer.advanceToNextToken();
                if (rc) {
                    logTokenizerError("Error") << " reading value for"
                                 << " attribute '" << d_elementName << "' \n";
                    return -1;                                        // RETURN
                }

                JsonConverter_ElementVisitor visitor = { this, mode };

                if (0 != bdlat_SequenceFunctions::manipulateAttribute(
                                   value,
                                   visitor,
                                   d_elementName.data(),
                                   static_cast<int>(d_elementName.length()))) {
                    d_logStream << "Could not decode sequence, error decoding "
                                << "element or bad element name '"
                                << d_elementName << "' \n";
                    return -1;                                        // RETURN
                }
            }
            else {
                if (d_skipUnknownElements) {
                    rc = skipUnknownElement(elementName);
                    if (rc) {
                        d_logStream << "Error reading unknown element '"
                                    << elementName << "' or after it\n";
                        return -1;                                    // RETURN
                    }
                }
                else {
                    d_logStream << "Unknown element '"
                                << elementName << "' found\n";
                    return -1;                                        // RETURN
                }
            }

            rc = d_tokenizer.advanceToNextToken();
            if (rc) {
                d_logStream << "Could not decode sequence, ";
                logTokenizerError("error") << " reading token"
                            << " after value for attribute '"
                            << d_elementName << "' \n";
                return -1;                                            // RETURN
            }
        }

        if (Tokenizer::e_END_OBJECT != d_tokenizer.tokenType()) {
            d_logStream << "Could not decode sequence, "
                        << "missing terminator '}' or seperator ','\n";
            return -1;                                                // RETURN
        }

        --d_currentDepth;
    }
    return 0;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                     *value,
                           int                         mode,
                           bdlat_TypeCategory::Choice  )
{
    if (bdlat_FormattingMode::e_UNTAGGED & mode) {
        // This is an anonymous element.  Do not read anything and instead
        // decode into the corresponding sub-element.

        bslstl::StringRef selectionName;
        selectionName.assign(d_elementName.begin(), d_elementName.end());

        if (bdlat_ChoiceFunctions::hasSelection(
                                   *value,
                                   selectionName.data(),
                                   static_cast<int>(selectionName.length()))) {
            if (0 != bdlat_ChoiceFunctions::makeSelection(
                                   value,
                                   selectionName.data(),
                                   static_cast<int>(selectionName.length()))) {
                d_logStream << "Could not decode choice, bad selection name '"
                            << selectionName << "' \n";
                return -1;                                            // RETURN
            }

            JsonConverter_ElementVisitor visitor = { this, mode };

            if (0 != bdlat_ChoiceFunctions::manipulateSelection(value,
                                                                visitor)) {
                d_logStream << "Could not decode choice, selection "
                            << "was not decoded\n";
                return -1;                                            // RETURN
            }
        }
        else {
            if (d_skipUnknownElements) {
                const int rc = skipUnknownElement(selectionName);
                if (rc) {
                    d_logStream << "Error reading unknown element '"
                                << selectionName << "' or after that "
                                << "element\n";
                    return -1;                                        // RETURN
                }
            }
            else {
                d_logStream << "Unknown element '"
                            << selectionName << "' found\n";
                return -1;                                            // RETURN
            }
        }
    }
    else {
        if (++d_currentDepth > d_maxDepth) {
            d_logStream << "Maximum allowed decoding depth reached: "
                        << d_currentDepth << "\n";
            return -1;                                                // RETURN
        }

        if (Tokenizer::e_START_OBJECT != d_tokenizer.tokenType()) {
            d_logStream << "Could not decode choice, missing starting {\n";
            return -1;                                                // RETURN
        }

        int rc = d_tokenizer.advanceToNextToken();
        if (rc) {
            d_logStream << "Could not decode choice, ";
            logTokenizerError("error") << " reading token after {\n";
            return -1;                                                // RETURN
        }

        if (Tokenizer::e_ELEMENT_NAME == d_tokenizer.tokenType()) {
            bslstl::StringRef selectionName;
            rc = d_tokenizer.value(&selectionName);
            if (rc) {
                d_logStream << "Error reading selection name after '{'\n";
                return -1;                                            // RETURN
            }

            if (bdlat_ChoiceFunctions::hasSelection(
                                   *value,
                                   selectionName.data(),
                                   static_cast<int>(selectionName.length()))) {
                if (0 != bdlat_ChoiceFunctions::makeSelection(
                                   value,
                                   selectionName.data(),
                                   static_cast<int>(selectionName.length()))) {
                    d_logStream << "Could not decode choice, bad selection "
                                << "name '" << selectionName << "' \n";
                    return -1;                                        // RETURN
                }

                rc = d_tokenizer.advanceToNextToken();
                if (rc) {
                    d_logStream << "Could not decode choice, ";
                    logTokenizerError("error") << " reading value \n";
                    return -1;                                        // RETURN
                }

                JsonConverter_ElementVisitor visitor = { this, mode };

                if (0 != bdlat_ChoiceFunctions::manipulateSelection(value,
                                                                    visitor)) {
                    d_logStream << "Could not decode choice, selection "
                                << "was not decoded\n";
                    return -1;                                        // RETURN
                }
            }
            else {
                if (d_skipUnknownElements) {
                    rc = skipUnknownElement(selectionName);
                    if (rc) {
                        d_logStream << "Error reading unknown element '"
                                    << selectionName << "' or after that "
                                    << "element\n";
                        return -1;                                    // RETURN
                    }
                }
                else {
                    d_logStream << "Unknown element '"
                                << selectionName << "' found\n";
                    return -1;                                        // RETURN
                }
            }

            rc = d_tokenizer.advanceToNextToken();

            if (rc) {
                d_logStream << "Could not decode choice, ";
                logTokenizerError("error") << " reading token after value for"
                                                               " selection \n";

                return -1;                                            // RETURN
            }
        }

        if (Tokenizer::e_END_OBJECT != d_tokenizer.tokenType()) {
            d_logStream << "Could not decode choice, "
                        << "missing terminator '}'\n";
            return -1;                                                // RETURN
        }

        --d_currentDepth;
    }
    return 0;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                            *value,
                             int                              ,
                             bdlat_TypeCategory::Enumeration  )
{
    enum { k_MIN_ENUM_STRING_LENGTH = 2 };

    if (Tokenizer::e_ELEMENT_VALUE != d_tokenizer.tokenType()) {
        d_logStream << "Enumeration element value was not found\n";
        return -1;                                                    // RETURN
    }

    const bdljsn::Json *dataValue;
    int rc = d_tokenizer.value(&dataValue);
    if (rc) {
        d_logStream << "Error reading enumeration value\n";
        return -1;                                                    // RETURN
    }

    BSLS_ASSERT(dataValue);

    if (dataValue->isString()) {

        rc = bdlat::EnumUtil::fromStringOrFallbackIfEnabled(
                                               value,
                                               dataValue->theString().data(),
                              static_cast<int>(dataValue->theString().size()));
        if (0 == rc) {
            return 0;                                                 // RETURN
        } else {
            d_logStream
                  << "Could not decode Enum String, value not allowed \""
                  << dataValue << "\"\n";
            return rc;
        }
    } else if (dataValue->isNumber()) {

        // We also accept an unquoted integer (DRQS 166048981).
        int              intValue;
        bsl::string_view data = dataValue->theNumber().value();
        rc = ParserUtil::getValue(&intValue, data);
        if (rc) {
            d_logStream << "Error reading enumeration value\n";
            return -1;                                                // RETURN
        }

        rc = bdlat::EnumUtil::fromIntOrFallbackIfEnabled(value, intValue);
        if (0 == rc) {
            return 0;                                                 // RETURN
        } else {
            d_logStream << "Could not decode int Enum, value " << intValue
                        << " not allowed\n";
            return rc;                                                // RETURN
        }
    }
    return 666;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                               *value,
                             int                                 ,
                             bdlat_TypeCategory::CustomizedType  )
{
    if (Tokenizer::e_ELEMENT_VALUE != d_tokenizer.tokenType()) {
        d_logStream << "Customized element value was not found\n";
        return -1;                                                    // RETURN
    }

    const bdljsn::Json *dataValue;
    int rc = d_tokenizer.value(&dataValue);
    if (rc) {
        d_logStream << "Error reading customized type value\n";
        return -1;                                                    // RETURN
    }

    typedef
         typename bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType valueBaseType;

    if (bsl::is_integral<BaseType>::value
     && dataValue->isString()) {  // per DRQS 166048981
        bsl::string_view data(dataValue->theString());
        rc =     ParserUtil::getValue(&valueBaseType, data);
    } else {
        rc = JsonParserUtil::getValue(&valueBaseType, *dataValue);
    }

    rc = bdlat_CustomizedTypeFunctions::convertFromBaseType(value,
                                                            valueBaseType);
    if (rc) {
        d_logStream << "Could not convert base type to customized type, "
                    << "base value disallowed: \"";
        bdlb::PrintMethods::print(d_logStream, valueBaseType, 0, -1);
        d_logStream << "\"\n";
    }
    return rc;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                       *value,
                             int                         ,
                             bdlat_TypeCategory::Simple  )
{
    if (Tokenizer::e_ELEMENT_VALUE != d_tokenizer.tokenType()) {
        d_logStream << "Simple element value was not found\n";
        return -1;                                                    // RETURN
    }

    const bdljsn::Json *dataValue;
    int rc = d_tokenizer.value(&dataValue);
    if (rc) {
        d_logStream << "Error reading simple value\n";
        return -1;                                                    // RETURN
    }

    if (bsl::is_integral<TYPE>::value
     && dataValue->isString()) {  // per DRQS 166048981
        bsl::string_view data(dataValue->theString());
        rc =     ParserUtil::getValue(value, data);
    } else {
        rc = JsonParserUtil::getValue(value, *dataValue);
    }

    return rc;
}

inline
int JsonConverter::decodeImp(bsl::vector<char>         *value,
                             int                        ,
                             bdlat_TypeCategory::Array  )
{
    if (Tokenizer::e_ELEMENT_VALUE != d_tokenizer.tokenType()) {
        d_logStream << "Could not decode vector<char> "
                    << "expected as an element value\n";
        return -1;                                                    // RETURN
    }

    const bdljsn::Json *dataValue;
    int rc = d_tokenizer.value(&dataValue);

    if (rc) {
        d_logStream << "Error reading customized type element value\n";
        return -1;                                                    // RETURN
    }

    {
        BSLS_ASSERT_SAFE(dataValue->isString());

        value->clear();

        const bsl::string& base64String = dataValue->theString();

        bdlde::Base64Decoder base64Decoder(true);
        int                  length = static_cast<int>(base64String.length());

        value->resize(static_cast<bsl::size_t>(
                              bdlde::Base64Decoder::maxDecodedLength(length)));

        rc = base64Decoder.convert(value->begin(),
                                   base64String.begin(),
                                   base64String.end());

        if (rc < 0) {
            return rc;                                                // RETURN
        }

        rc = base64Decoder.endConvert(value->begin() +
                                      base64Decoder.outputLength());

        if (rc < 0) {
            return rc;                                                // RETURN
        }

        value->resize(static_cast<bsl::size_t>(base64Decoder.outputLength()));
    }

    return rc;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                      *value,
                             int                        mode,
                             bdlat_TypeCategory::Array  )
{
    if (Tokenizer::e_START_ARRAY != d_tokenizer.tokenType()) {
        d_logStream << "Could not decode vector, missing start token: '['\n";
        return -1;                                                    // RETURN
    }

    int rc = d_tokenizer.advanceToNextToken();
    if (rc) {
        logTokenizerError("Error") << " reading array.\n";
        return rc;                                                    // RETURN
    }

    int i = 0;
    while (Tokenizer::e_END_ARRAY != d_tokenizer.tokenType()) {

        if (Tokenizer::e_ELEMENT_VALUE == d_tokenizer.tokenType()
         || Tokenizer::e_START_OBJECT  == d_tokenizer.tokenType()
         || Tokenizer::e_START_ARRAY   == d_tokenizer.tokenType()) {
            ++i;
            bdlat_ArrayFunctions::resize(value, i);

            JsonConverter_ElementVisitor visitor = { this, mode };

            if (0 != bdlat_ArrayFunctions::manipulateElement(value,
                                                             visitor,
                                                             i - 1)) {
                d_logStream << "Error adding element '" << i - 1 << "'\n";
                return -1;                                            // RETURN
            }

            rc = d_tokenizer.advanceToNextToken();
            if (rc) {
                logTokenizerError("Error") << " reading token after value of"
                                " element '" << i - 1 << "'\n";
                return rc;                                            // RETURN
            }
        }
        else {
            d_logStream << "Erroneous token found instead of array element\n";
            return -1;                                                // RETURN
        }
    }

    if (Tokenizer::e_END_ARRAY != d_tokenizer.tokenType()) {
        d_logStream << "Could not decode vector, missing end token: ']'\n";
        return -1;                                                    // RETURN
    }

    return 0;
}

template <class TYPE>
int JsonConverter::decodeImp(TYPE                              *value,
                             int                                mode,
                             bdlat_TypeCategory::NullableValue  )
{
    enum { k_NULL_VALUE_LENGTH = 4 };

    if (Tokenizer::e_ELEMENT_VALUE == d_tokenizer.tokenType()) {
        const bdljsn::Json *dataValue;
        const int rc = d_tokenizer.value(&dataValue);
        if (rc) {
            return rc;                                                // RETURN
        }
        BSLS_ASSERT_SAFE(dataValue);
        if (dataValue->isNull()) {
            return 0;                                                 // RETURN
        }
    }

    bdlat_NullableValueFunctions::makeValue(value);

    JsonConverter_ElementVisitor visitor = { this, mode };
    int rc = bdlat_NullableValueFunctions::manipulateValue(value, visitor);
    return rc;
}

template <class TYPE, class ANY_CATEGORY>
inline
int JsonConverter::decodeImp(TYPE * , ANY_CATEGORY )
{
    BSLS_ASSERT_OPT(0 == "Unreachable");

    return -1;
}

// CREATORS
inline
JsonConverter::JsonConverter()
: d_logStream()
{
}

inline
JsonConverter::JsonConverter(const allocator_type& allocator)
: d_logStream(bslma::AllocatorUtil::adapt(allocator))
{
}

// MANIPULATORS
template <class TYPE>
int JsonConverter::convert(bdljsn::Json *json, const TYPE& value)
{
    BSLS_ASSERT(json);

    d_logStream.clear();
    d_logStream.str("");

    bdlat_TypeCategory::Value category =
                                    bdlat_TypeCategoryFunctions::select(value);

    switch (category) {
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY:           BSLA_FALLTHROUGH;
      case bdlat_TypeCategory::  e_CHOICE_CATEGORY:           BSLA_FALLTHROUGH;
      case bdlat_TypeCategory::   e_ARRAY_CATEGORY: {
      } break;
      default: {
        d_logStream
            << "Encoded object must be a Sequence, Choice, or Array type."
            << bsl::endl;
        return -1;                                                    // RETURN
      } break;
    }

    static const bool s_FIRST_MEMBER_FLAG = false;

    EncoderOptions mOptions;  const EncoderOptions& cOptions = mOptions;
    mOptions.setEncodeNullElements      (true);  // not the default
    mOptions.setEncodeEmptyArrays       (true);  // not the default
    mOptions.setEncodeInfAndNaNAsStrings(true);  // not the default

    json->makeNull();

    bool          isValueEmpty;
    JsonFormatter formatter(json,
                            bslma::AllocatorUtil::adapt(
                                                 d_logStream.get_allocator()));

    int rc = EncodeImplUtil<JsonFormatter>::encode(
                                               &isValueEmpty,
                                               &formatter,
                                               &d_logStream,
                                               value,
                                               bdlat_FormattingMode::e_DEFAULT,
                                               cOptions,
                                               s_FIRST_MEMBER_FLAG);
    return rc;
}

template <class TYPE>
int JsonConverter::convert(TYPE                  *value,
                           const bdljsn::Json&    json,
                           const DecoderOptions&  options)
{
    BSLS_ASSERT(value);

    d_logStream.clear();
    d_logStream.str("");

    bdlat_TypeCategory::Value category =
                                   bdlat_TypeCategoryFunctions::select(*value);

    switch (category) {
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY:           BSLA_FALLTHROUGH;
      case bdlat_TypeCategory::  e_CHOICE_CATEGORY:           BSLA_FALLTHROUGH;
      case bdlat_TypeCategory::   e_ARRAY_CATEGORY: {
      } break;
      default: {
        d_logStream
            << "Target object must be a Sequence, Choice, or Array type."
            << bsl::endl;
        int rc  = -1;
        return rc;                                                    // RETURN
      } break;
    }

    d_currentDepth        = 0;
    d_maxDepth            = options.maxDepth();
    d_skipUnknownElements = options.skipUnknownElements();

    d_tokenizer.reset(&json);  // Must we call this `reset`?

    BSLS_ASSERT_SAFE(Tokenizer::e_BEGIN == d_tokenizer.tokenType());
    int rc = d_tokenizer.advanceToNextToken();
    if (rc) {
        logTokenizerError("Error") << " advancing to the first token.\n";
        return rc;                                                    // RETURN
    }

    bdlat_ValueTypeFunctions::reset(value);

    typedef typename bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;
    rc = decodeImp(value, 0, TypeCategory());

    return rc;
}
// ACCESSORS
inline
bsl::string JsonConverter::loggedMessages() const
{
    return d_logStream.str();
}

                                  // Aspects

inline
JsonConverter::allocator_type JsonConverter::get_allocator() const
{
    return bslma::AATypeUtil::getAllocatorFromSubobject<allocator_type>(
                                                                  d_logStream);
}

                       // -----------------------------------
                       // struct JsonConverter_ElementVisitor
                       // -----------------------------------

template <class TYPE>
inline
int JsonConverter_ElementVisitor::operator()(TYPE *value)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return d_decoder_p->decodeImp(value, d_mode, TypeCategory());
}

template <class TYPE, class INFO>
inline
int JsonConverter_ElementVisitor::operator()(TYPE *value, const INFO& info)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return d_decoder_p->decodeImp(value,
                                  info.formattingMode(),
                                  TypeCategory());
}

                       // -------------------------------
                       // struct JsonConverter_DecodeImpProxy
                       // -------------------------------

// MANIPULATORS
template <class TYPE>
inline
int JsonConverter_DecodeImpProxy::operator()(TYPE * , bslmf::Nil )
{
    BSLS_ASSERT_OPT(0 == "Unreachable");

    return -1;
}

template <class TYPE, class ANY_CATEGORY>
inline
int JsonConverter_DecodeImpProxy::operator()(TYPE         *object,
                                             ANY_CATEGORY  category)
{
    return d_decoder_p->decodeImp(object, d_mode, category);
}
}  // close package    namespace
}  // close enterprise namespace
#endif

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
