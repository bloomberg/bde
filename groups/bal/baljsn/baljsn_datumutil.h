// baljsn_datumutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BALJSN_DATUMUTIL
#define INCLUDED_BALJSN_DATUMUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide utilities converting between 'bdld::Datum' and JSON data.
//
//@CLASSES:
//  baljsn::DatumUtil: utilities converting between 'bdld::Datum' and JSON data
//
//@DESCRIPTION: This component provides a struct, 'baljsn::DatumUtil', that is
// a namespace for a suite of functions that convert a 'bdld::Datum' into a
// JSON string, and back.
//
///Mapping Data Types between 'Datum' and JSON
///-------------------------------------------
// While most scalar types supported by 'Datum' can be encoded into a JSON
// string, only the subset of types represented natively in JSON -- number
// (represented in C++ as a 'double'), 'string', 'bool', 'null', array, and map
// types -- will be populated in a 'Datum' decoded from a JSON string.  If one
// were to encode a 'Datum' containing a type not natively supported by JSON,
// if that JSON string were decoded back into a 'Datum' object, the resulting
// 'Datum' would not be equal to the original value.  For example, a 'Datum'
// containing an integer would be encoded into a JSON number, and then decoded
// back into a 'Datum' using 'double' to represent that number.  Note that
// 'DatumUtil' uses a *more permissive* parser for numerical values than the
// strict JSON standard specifies.  In particular, it is possible to parse
// 'NaN', 'Inf', or 'Infinity' into the corresponding singular 'double' values
// even though the JSON standard does not permit this, so applications should
// be ready to handle these kinds of values.  Also note that the 'encode'
// routines do not encode these singular 'double' values in these parseable
// formats.  Singular 'double' values will be rendered as strings (e.g., "+inf"
// or "nan") if the 'strictTypes' encoding configuration is 'false', and will
// result generate an encoding error if 'strictTypes' is 'true'.
//
// Clients wishing to ensure that encoding and then decoding results in a
// 'Datum' equal to the original value should use only 'Datum' types natively
// supported in JSON (see {'Supported Types'} below), and ensure that duplicate
// keys are not present in the source 'Datum' (duplicate keys in a 'Datum' map
// are typically an error, but the interface does allow them to be created).
// Enabling the 'strictTypes' option verifies that the types in encoded JSON
// fields can be decoded back into 'Datum' fields of equal value.  So, for
// example, enabling 'strictTypes' will result in 'encode' producing a positive
// return status if one of the encoded types is an 'int', because decoding the
// resulting JSON will produce a 'double'.  The 'strictTypes' option does not,
// however, verify that a Datum map contains unique keys.  For 'double' fields,
// 'strictTypes' will result in 'encode' returning a positive value if a
// singular 'double' value is encountered, such as a NaN or Infinity.
//
// The order of key/value pairs in objects in textual JSON passed to 'decode'
// is preserved in the decoded 'Datum'.  If multiple entries with the same
// 'key' are present in an object, 'decode' will return the *first* such value.
//
// The order of key/value pairs ('DatumMapEntry') in 'Datum' objects passed to
// 'encode' will be preserved in the resulting 'JSON', and all keys/value pairs
// will be present (including duplicate keys).  Duplicate keys will be rendered
// in an encoded JSON, even if 'strictTypes' checking is enabled.  Note that a
// Datum map containing duplicate keys is typically an error (the result of a
// incorrectly constructed Datum), but the public interface for Datum does not
// disallow creating such a 'Datum' object.
//
///Supported Types
///---------------
// The table below describes the set of types that a 'Datum' may be, whether it
// can be 'encode'd to JSON, and, if so, which JSON type will be 'decode'd if
// the value is read back in.
//
// The 'encode' routines will return a negative (error) status if the input
// 'datum' contains any field that is not 'JSON-able' in this table.
//
// If the 'DatumEncoderOptions' parameter is passed to an 'encode' routine and
// its 'strictTypes' field is 'true', then 'encode' will return a positive
// value if any value is encoded where the 'dataType' and 'decode type' columns
// in this table are different (and therefore the 'strictTypes ok?' column is
// 'no').
//
//..
//  dataType              JSON-able  JSON type   decode type    strictTypes ok?
//  --------              ---------  ---------   -----------    ---------------
//  e_NIL                 yes        null        e_NIL          yes
//  e_INTEGER             yes        number      e_DOUBLE       no
//  e_DOUBLE              yes        number      e_DOUBLE       yes [1]
//  e_STRING              yes        string      e_STRING       yes
//  e_BOOLEAN             yes        bool        e_BOOLEAN      yes
//  e_ERROR               no         N/A         N/A            no
//  e_DATE                yes        string      e_STRING       no
//  e_TIME                yes        string      e_STRING       no
//  e_DATETIME            yes        string      e_STRING       no
//  e_DATETIME_INTERVAL   yes        string      e_STRING       no
//  e_INTEGER64           yes        number      e_DOUBLE       no
//  e_USERDEFINED         no         N/A         N/A            no
//  e_BINARY              no         N/A         N/A            no
//  e_DECIMAL64 [2]       yes        number      e_STRING       no
//
//  dataType              JSON-able  JSON type   decode type    strictTypes ok?
//  --------              ---------  ---------   -----------    ---------------
//  e_ARRAY               yes        array       e_ARRAY        yes
//  e_MAP                 yes        map         e_MAP          yes
//  e_INT_MAP             no         N/A         N/A            no
//
// [1] Singular double values (e.g., inf and nan) are not permitted if
//     strictTypes is 'true', and will be rendered as strings if 'strictTypes'
//     is 'false'.
// [2] If the 'encodeQuotedDecimal64' attribute in the 'DatumEncoderOptions' is
//     'true' (the default), the 'Decimal64' values will be encoded as strings,
//     otherwise they will be encoded as numbers. Encoding a Decimal64 as a
//     JSON number will frequently result in it being later decoded as a binary
//     floating point number, and in the process losing digits of precision
//     that were the point of using the Decimal64 type in the first place.
//     Care should be taken when setting this option to 'false' (though it may
//     be useful when communicating with endpoints that are known to correctly
//     handle high precision JSON numbers).
//..
//: o *dataType* - the 'Datum' type value returned by the 'type()'
//:
//: o *JSON-able* - whether the type can be 'encode'd by this component.
//:
//: o *JSON type* - the JSON type used to 'encode' this value, if supported.
//:
//: o *decode type* - the 'Datum' type this 'encode'd value would be
//:    'decode'd into.
//:
//: o *strictTypes ok?* - 'encode' will return 0 on success even if
//:   'options->strictTypes()' is 'true'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encode (and decode) 'Datum' to (and from) a JSON string.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates encoding a 'Datum' as a JSON string and
// then decoding that JSON string back into a 'Datum' object.
//
// First, we create our 'Datum' object, using the 'bdld::DatumMaker' utility:
//..
//  bsls::AlignedBuffer<8 * 1024>      buffer;
//  bdlma::BufferedSequentialAllocator bsa(buffer.buffer(), sizeof(buffer));
//  bdld::DatumMaker                   m(&bsa);
//
//  bdld::Datum books = m.a(m.m("Author", "Ann Leckie",
//                              "Title", "Ancillary Justice"),
//                          m.m("Author", "John Scalzi",
//                              "Title", "Redshirts"));
//..
// Then, we convert the 'books' 'Datum' to formatted JSON:
//..
//  baljsn::DatumEncoderOptions bookOptions;
//  bookOptions.setEncodingStyle(baljsn::EncodingStyle::e_PRETTY);
//  bookOptions.setSpacesPerLevel(4);
//  bsl::string booksJSON(&bsa);
//
//  int rc = baljsn::DatumUtil::encode(&booksJSON, books, bookOptions);
//  if (0 != rc) {
//      // handle error
//  }
//..
// Next, we compare the result to the JSON we expect:
//..
//  const bsl::string EXPECTED_BOOKS_JSON = R"JSON([
//    {
//        "Author" : "Ann Leckie",
//        "Title" : "Ancillary Justice"
//    },
//    {
//        "Author" : "John Scalzi",
//        "Title" : "Redshirts"
//    }
//  ])JSON";
//
//  assert(EXPECTED_BOOKS_JSON == booksJSON);
//..
// Finally, we can decode the 'booksJSON' and make sure we got the same value
// back:
//..
//  bdld::ManagedDatum decodedBooks;
//  rc = baljsn::DatumUtil::decode(&decodedBooks, booksJSON);
//  if (0 != rc) {
//      // handle error
//  }
//  assert(*decodedBooks == books);
//..
///Example 2: Converting JSON to 'Datum'
///- - - - - - - - - - - - - - - - - - -
// The following example illustrates decoding a string into a 'Datum' object.
//
// First, we create the JSON source, in both plain and formatted forms:
//..
//  const bsl::string plainFamilyJSON = "["
//                               "{\"firstName\":\"Homer\","
//                               "\"age\":34}"
//                               ",{\"firstName\":\"Marge\","
//                               "\"age\":34}"
//                               ",{\"firstName\":\"Bart\","
//                               "\"age\":10}"
//                               ",{\"firstName\":\"Lisa\","
//                               "\"age\":8}"
//                               ",{\"firstName\":\"Maggie\","
//                               "\"age\":1}"
//                               "]";
//
//  // Note that whitespace formatting is unimportant as long as the result is
//  // legal JSON.  This will generate the same 'Datum' as the single-line form
//  // above.
//  const bsl::string formattedFamilyJSON = R"JSON([
//      {
//          "firstName" : "Homer",
//          "age" : 34
//      },
//      {
//          "firstName" : "Marge",
//          "age" : 34
//      },
//      {
//          "firstName" : "Bart",
//          "age" : 10
//      },
//      {
//          "firstName" : "Lisa",
//          "age" : 8
//      },
//      {
//          "firstName" : "Maggie",
//          "age" : 1
//      }
//  ])JSON";
//..
// Then, we convert the single-line 'string' to a 'Datum':
//..
//  bdld::ManagedDatum family;
//  rc = baljsn::DatumUtil::decode(&family, plainFamilyJSON);
//  if (0 != rc) {
//      // handle error
//  }
//..
// Next, we convert the formatted 'string' to another 'Datum' and make sure
// that the results match:
//..
//  bdld::ManagedDatum family2;
//  rc = baljsn::DatumUtil::decode(&family2, formattedFamilyJSON);
//  if (0 != rc) {
//      // handle error
//  }
//  assert(family == family2);
//..
// Finally, we make sure that the structure of the resulting datum is as we
// expect.
//..
//  assert(family->isArray());
//  assert(5 == family->theArray().length());
//
//  const bdld::Datum &lisa = family->theArray()[3];
//
//  assert(lisa.isMap());
//  assert(2         == lisa.theMap().size());
//  assert("Lisa"    == lisa.theMap().find("firstName")->theString());
//  assert(8         == lisa.theMap().find("age")->theDouble());
//..
// Notice that the 'type' of "age" is 'double', since "age" was encoded as a
// number, and 'double' is the supported representation of a JSON number (see
// {'Supported Types'}).

#include <balscm_version.h>

#include <baljsn_datumdecoderoptions.h>
#include <baljsn_datumencoderoptions.h>

#include <bdld_datum.h>
#include <bdld_manageddatum.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bsls_libraryfeatures.h>

#include <bsl_iosfwd.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
# include <memory_resource>
#endif
#include <string>

namespace BloombergLP {
namespace baljsn {

class SimpleFormatter;

                              // ================
                              // struct DatumUtil
                              // ================

struct DatumUtil {
    // This 'struct' provides a namespace for a suite of functions that convert
    // between a JSON formated string and a 'bdld::Datum'.

    // CLASS METHODS
    static int decode(bdld::ManagedDatum         *result,
                      const bsl::string_view&     json);
    static int decode(bdld::ManagedDatum         *result,
                      const bsl::string_view&     json,
                      const DatumDecoderOptions&  options);
    static int decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      const bsl::string_view&     json);
    static int decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      const bsl::string_view&     json,
                      const DatumDecoderOptions&  options);
        // Decode the specified 'json' into the specified 'result'.  If the
        // optionally specified 'errorStream' is non-null, a description of any
        // errors that occur during parsing will be output to this stream.  If
        // the optionally specified 'options' argument is not present, treat it
        // as a default-constructed 'DatumDecoderOptions'.  Return 0 on
        // success, and a negative value if 'json' could not be decoded (either
        // because it is ill-formed, or if a constraint imposed by 'option' is
        // violated).  An error status will be returned if 'json' contains
        // arrays or objects that are nested beyond a depth configured by
        // 'options.maxNestedDepth()'.  The mapping of types in JSON to the
        // types supported by 'Datum' is described in {Supported Types}.

    static int decode(bdld::ManagedDatum         *result,
                      bsl::streambuf             *jsonBuffer);
    static int decode(bdld::ManagedDatum         *result,
                      bsl::streambuf             *jsonBuffer,
                      const DatumDecoderOptions&  options);
    static int decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      bsl::streambuf             *jsonBuffer);
    static int decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      bsl::streambuf             *jsonBuffer,
                      const DatumDecoderOptions&  options);
        // Decode the JSON string provided by the specified 'jsonBuffer' into
        // the specified 'result'.  If the optionally specified 'errorStream'
        // is non-null, a description of any errors that occur during parsing
        // will be output to this stream.  If the optionally specified
        // 'options' argument is not present, treat it as a default-constructed
        // 'DatumDecoderOptions'.  Return 0 on success, and a negative value if
        // 'json' could not be decoded (either because it is ill-formed, or if
        // a constraint imposed by 'option' is violated).  An error status will
        // be returned if 'json' contains arrays or objects that are nested
        // beyond a depth configured by 'options.maxNestedDepth()'.  The
        // mapping of types in JSON to the types supported by 'Datum' is
        // described in {Supported Types}.

    static int encode(bsl::string         *result,
                      const bdld::Datum&   datum);
    static int encode(std::string         *result,
                      const bdld::Datum&   datum);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static int encode(std::pmr::string   *result,
                      const bdld::Datum&  datum);
#endif
    static int encode(bsl::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options);
    static int encode(std::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static int encode(std::pmr::string           *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options);
#endif
        // Encode the specified 'datum' as a JSON string, and load the
        // specified 'result' with the encoded JSON string.  Return 0 on
        // success, and a negative value if 'datum' could not be encoded (with
        // no effect on 'result').  If the optionally specified 'options'
        // argument is not present, treat it as a default-constructed
        // 'DatumEncoderOptions'.  If 'options.strictTypes' is 'true' and a
        // type that is not supported by JSON, or a singular double value
        // (e.g., NaN or infinity) is being encoded (see {Supported Types})
        // return a positive value, but also populate 'result' with an encoded
        // JSON string (i.e., the value of 'result' is the same regardless of
        // the 'strictTypes' option, but if 'strictTypes' is 'true' a non-zero
        // positive status will be returned).  The mapping of types supported
        // by 'Datum' to JSON types is described in {Supported Types}.

    static int encode(bsl::ostream&              stream,
                      const bdld::Datum&         datum);
    static int encode(bsl::ostream&              stream,
                      const bdld::Datum&         datum,
                      const DatumEncoderOptions& options);
        // Encode the specified 'datum' as a JSON string, and write it into the
        // specified 'stream'.  Return 0 on success, and a negative value if
        // 'datum' could not be encoded (which may leave a partial JSON
        // sequence on the 'stream').  If the optionally specified 'options'
        // argument is not present, treat it as a default-constructed
        // 'DatumEncoderOptions'.  If 'options.strictTypes' is 'true' and a
        // type that is not supported by JSON, or a singular double value
        // (e.g., NaN or infinity) is being encoded (see {Supported Types})
        // return a positive value, but also populate 'result' with an encoded
        // JSON string (i.e., the value of 'result' is the same regardless of
        // the 'strictTypes' option, but if 'strictTypes' is 'true' a non-zero
        // positive status will be returned).  The mapping of types supported
        // by 'Datum' to JSON types is described in {Supported Types}.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CLASS METHODS

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      const bsl::string_view&     json,
                      const DatumDecoderOptions&  options)
{
    bdlsb::FixedMemInStreamBuf buffer(json.data(), json.length());
    return decode(result, 0, &buffer, options);
}

inline
int DatumUtil::decode(bdld::ManagedDatum       *result,
                      const bsl::string_view&   json)
{
    return decode(result, json, DatumDecoderOptions());
}

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      const bsl::string_view&     json,
                      const DatumDecoderOptions&  options)
{
    bdlsb::FixedMemInStreamBuf buffer(json.data(), json.length());
    return decode(result, errorStream, &buffer, options);
}

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      const bsl::string_view&     json)
{
    return decode(result, errorStream, json, DatumDecoderOptions());
}

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::streambuf             *jsonBuffer,
                      const DatumDecoderOptions&  options)
{
    return decode(result, 0, jsonBuffer, options);
}

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::streambuf             *jsonBuffer)
{
    return decode(result, 0, jsonBuffer, DatumDecoderOptions());
}

inline
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      bsl::streambuf             *jsonBuffer)
{
    return decode(result, errorStream, jsonBuffer, DatumDecoderOptions());
}

inline
int DatumUtil::encode(bsl::string *result, const bdld::Datum& datum)
{
    return encode(result, datum, DatumEncoderOptions());
}

inline
int DatumUtil::encode(std::string *result, const bdld::Datum& datum)
{
    return encode(result, datum, DatumEncoderOptions());
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
int DatumUtil::encode(std::pmr::string   *result,
                      const bdld::Datum&  datum)
{
    return encode(result, datum, DatumEncoderOptions());
}
#endif

inline
int DatumUtil::encode(bsl::ostream& stream, const bdld::Datum& datum)
{
    return encode(stream, datum, DatumEncoderOptions());
}


}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
