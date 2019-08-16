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
///Mapping Data Types between Datum and JSON
///-----------------------------------------
// While most scalar types supported by 'Datum' can be encoded into a JSON
// string, only the subset of types represented natively in JSON -- number
// (represented in C++ as a 'double'), 'string', 'bool', 'null', array, and map
// types -- will be populated in a Datum decoded from a JSON string.  If one
// were to encode a Datum containing a type not natively supported by JSON, if
// that JSON string were decoded back into a Datum object, the resulting Datum
// would not be equal to the original value.  For example, a Datum containing
// an integer would be encoded into a JSON number, and then decoded back into a
// Datum using 'double' to represent that number.
//
// Clients wishing to ensure that encoding and then decoding results in a Datum
// equal to the original value, should use only Datum types natively supported
// in JSON (see {'Supported Types'} below).  To verify that an encoded JSON can
// be decoded back into a Datum of equal value, clients can supply the
// 'strictTypes' option (see {'DatumEncoderOptions'}) to 'encode'.  A call to
// 'encode' with 'strictTypes' as 'true' will return a positive value if the
// encoded 'Datum' contained a type not natively supported in JSON.
//
// The order of key/value pairs in objects in textual JSON passed to 'decode'
// is preserved in the decoded 'Datum'.  If multiple entries with the same
// 'key' are present in an object, 'decode' will return the *first* such value.
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
//  e_DOUBLE              yes        number      e_DOUBLE       yes
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
//  e_DECIMAL64           yes        number      e_DOUBLE       no
//
//  dataType              JSON-able  JSON type   decode type    strictTypes ok?
//  --------              ---------  ---------   -----------    ---------------
//  e_ARRAY               yes        array       e_ARRAY        yes
//  e_MAP                 yes        map         e_MAP          yes
//  e_INT_MAP             no         N/A         N/A            no
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
//                              "Title", "Ancilliary Justice"),
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
//        "Title" : "Ancilliary Justice"
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
//..

#include <balscm_version.h>

#include <baljsn_datumencoderoptions.h>

#include <bdld_datum.h>
#include <bdld_manageddatum.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bsl_iosfwd.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>

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
    static int encode(bsl::string               *result,
                      const bdld::Datum&         datum);
    static int encode(bsl::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options);
        // Encode the specified 'datum' as a JSON string, and load the
        // specified 'result' with the encoded JSON string.  Return 0 on
        // success, and a negative value if 'datum' could not be encoded (with
        // no effect on 'result').  If the optionally specified 'options'
        // argument is not present, treat it as a default-constructed
        // 'DatumEncoderOptions'.  If 'options.strictTypes' is 'true' and a
        // non-JSON type is being encoded (see {Supported Types}) return a
        // positive value, but also populate 'result' with an encoded JSON
        // string (i.e., the value of 'result' is the same regardless of the
        // 'strictTypes' option, but if 'strictTypes' is 'true' a non-zero
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
        // non-JSON type is being encoded (see {Supported Types}) return a
        // positive value, but also populate 'result' with an encoded JSON
        // string (i.e., the value of 'result' is the same regardless of the
        // 'strictTypes' option, but if 'strictTypes' is 'true' a non-zero
        // positive status will be returned).  The mapping of types supported
        // by 'Datum' to JSON types is described in {Supported Types}.

    static int decode(bdld::ManagedDatum       *result,
                      const bslstl::StringRef&  json);
    static int decode(bdld::ManagedDatum       *result,
                      bsl::ostream             *errorStream,
                      const bslstl::StringRef&  json);
        // Decode the specified 'json' into the specified 'result'.  If the
        // optionally specified 'errorStream' is non-null, a description of any
        // errors that occur during parsing will be output to this stream.
        // Return 0 on success, and a negative value if 'json' could not be
        // decoded (if it is ill-formed).  The mapping of types in JSON to the
        // types supported by 'Datum' is described in {Supported Types}.

    static int decode(bdld::ManagedDatum *result,
                      bsl::streambuf     *jsonBuffer);
    static int decode(bdld::ManagedDatum *result,
                      bsl::ostream       *errorStream,
                      bsl::streambuf     *jsonBuffer);
        // Decode the JSON string provided by the specified 'jsonBuffer' into
        // the specified 'result'.  If the optionally specified 'errorStream'
        // is non-null, a description of any errors that occur during parsing
        // will be output to this stream.  Return 0 on success, and a negative
        // value if the JSON string contained in 'jsonBuffer' could not be
        // decoded (if it is ill-formed).  The mapping of types in JSON to the
        // types supported by 'Datum' is described in {Supported Types}.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CLASS METHODS

inline
int DatumUtil::decode(bdld::ManagedDatum       *result,
                      const bslstl::StringRef&  json)
{
    bdlsb::FixedMemInStreamBuf buffer(json.data(), json.length());
    return decode(result, 0, &buffer);
}

inline
int DatumUtil::decode(bdld::ManagedDatum       *result,
                      bsl::ostream             *errorStream,
                      const bslstl::StringRef&  json)
{
    bdlsb::FixedMemInStreamBuf buffer(json.data(), json.length());
    return decode(result, errorStream, &buffer);
}

inline
int DatumUtil::decode(bdld::ManagedDatum *result, bsl::streambuf *jsonBuffer)
{
    return decode(result, 0, jsonBuffer);
}

inline
int DatumUtil::encode(bsl::string *result, const bdld::Datum& datum)
{
    const DatumEncoderOptions localOpts;

    return encode(result, datum, localOpts);
}

inline
int DatumUtil::encode(bsl::ostream& stream, const bdld::Datum& datum)
{
    const DatumEncoderOptions localOpts;

    return encode(stream, datum, localOpts);
}


}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
