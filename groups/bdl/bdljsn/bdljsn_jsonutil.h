// bdljsn_jsonutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONUTIL
#define INCLUDED_BDLJSN_JSONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'Json' objects.
//
//@CLASSES:
//  bdljsn::JsonUtil: namespace for non-primitive operations on 'Json' objects
//
//@DESCRIPTION: This component provides a namespace 'bdljsn::JsonUtil'
// containing utility functions that operate on 'Json' objects.
//
// The following methods are provided by 'JsonUtil':
//: o 'read'  populate a 'Json' object from a JSON text document.
//: o 'write' populate a JSON text document from a 'Json' object.
//
///Configuring the Output Format
///-----------------------------
// There are a number of options to configure the output format produced by
// 'write':
//: o 'sortMembers': sort the members of any Object elements in the output JSON
//:   (default: 'false')
//: o 'style': the style of the resulting output
//:   o 'e_COMPACT' (default): render with no added white space
//:   o 'e_ONELINE': render a human readable single-line format (e.g., for
//:     logging)
//:   o 'e_PRETTY': render a multi-line human readable format
//: o 'spacesPerLevel': for the 'e_PRETTY' style, the number of spaces added
//:   for each additional nesting level (default 4)
//: o 'initialIndentationLevel': for the 'e_PRETTY' style, the number of sets
//:   of 'spacesPerLevel' spaces added to every line of the output, including
//:   the first and last lines (default 0)
//
// The example below shows the various write styles:
//..
//  Compact:
//    {"a":1,"b":[]}
//
//  One-line:
//    {"a": 1, "b": []}
//
//  Pretty:
//    {
//        "a": 1,
//        "b": []
//    }
//..
// For more information, see the 'bdljsn_writeoptions' and
// 'bdljsn_writestyle' components.
//
///Handling of Duplicate Keys
///--------------------------
// 'bdljsn::JsonObject' represents a JSON Object having unique keys. If an
// Object with duplicate keys is found in a JSON document, 'read' will preserve
// the value associated with the FIRST instance of that key.
//
// Per the JSON RFC (https://www.rfc-editor.org/rfc/rfc8259#section-4):
//..
//   "The names within an object SHOULD be unique."
//..
// That is, the expectation is that a JSON document should have unique keys,
// and JSON documents with duplicate keys are not an interoperable
// representation.  JSON parsing implementations vary on how duplicate keys are
// handled (though many represent the object in-memory with unique keys).  Note
// that preserving the value of the first key is consistent with the behavior
// of the existing 'baljsn::DatumUtil' component.
//
///Allowing Trailing Text
///----------------------
// By default, 'bdljsn::JsonUtil::read' will report an error for input where a
// valid JSON document is followed by additional text unless the trailing text
// consists solely of white space characters.  This behavior is configured by
// the 'bdljsn::ReadOptions' attribute, "allowTrailingText" (which defaults to
// 'false').
//
// If "allowTrailingText" is 'true', then 'bdljsn::JsonUtil::read' will return
// success where a valid JSON document is followed by additional text as long
// as that text is separated from the valid JSON by a delimiter character
// (i.e., either the JSON text ends in a delimiter, or the text that follows
// starts with a delimiter).  Here, delimiters are white-space characters,
// '[',']','{','}',',', or '"'.  Per RFC 8259, white space characters are
// Space (0x20), Horizontal tab (0x09), New Line (0x0A), and Carriage Return
// (0x0D).
//
// The table below shows some examples:
//..
//  * "ATT" = "allowTrailingText"
//  * Document is only valid where the result is SUCCESS
//
//  +-----------+------------------------+-------------+-----------+
//  | Input     | ATT = false (default)  | ATT = true  | Document  |
//  +===========+========================+=============+===========+
//  | '[]'      | SUCCESS                | SUCCESS     | []        |
//  | '[] '     | SUCCESS                | SUCCESS     | []        |
//  | '[],'     | ERROR                  | SUCCESS     | []        |
//  | '[]a'     | ERROR                  | SUCCESS     | []        |
//  | 'false '  | SUCCESS                | SUCCESS     | false     |
//  | 'false,'  | ERROR                  | SUCCESS     | false     |
//  | 'falsea'  | ERROR                  | ERROR       |           |
//  | '"a"x'    | ERROR                  | SUCCESS     | "a"       |
//  +-----------+------------------------+-------------+-----------+
//..
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Reading and Writing JSON Data
/// - - - - - - - - - - - - - - - - - - - -
// This component provides methods for reading and writing JSON data to/from
// 'Json' objects.
//
// First, we define the JSON data we plan to read:
//..
//  const char *INPUT_JSON = R"JSON({
//   "a boolean": true,
//   "a date": "1970-01-01",
//   "a number": 2.1,
//   "an integer": 10,
//   "array of values": [
//     -1,
//      0,
//      2.718281828459045,
//      3.1415926535979,
//      "abc",
//      true
//    ],
//    "event": {
//      "date": "1969-07-16",
//      "description": "Apollo 11 Moon Landing",
//      "passengers": [
//        "Neil Armstrong",
//        "Buzz Aldrin"
//      ],
//      "success": true
//    }
//  }})JSON";
//..
// Next, we read the JSON data into a 'Json' object:
//..
//  bdljsn::Json        result;
//  bdljsn::Error       error;
//
//  int rc = bdljsn::JsonUtil::read(&result, &error, INPUT_JSON);
//
//  assert(0 == rc);
//
//  if (0 != rc) {
//      bsl::cout << "Error message: \"" << error.message() << "\""
//                << bsl::endl;
//  }
//..
// Then, we check the values of a few selected fields:
//..
//  assert(result.type() == JsonType::e_OBJECT);
//  assert(result["array of values"][2].theNumber().asDouble()
//         == 2.718281828459045);
//  assert(result["event"]["date"].theString() == "1969-07-16");
//  assert(result["event"]["passengers"][1].theString() == "Buzz Aldrin");
//..
// Finally, we'll 'write' the 'result' back into another string and make sure
// we got the same value back, by using the correct 'WriteOptions' to match
// the input format:
//..
//  bsl::string resultString;
//
//  // Set the WriteOptions to match the initial style:
//  WriteOptions writeOptions;
//  writeOptions.setStyle(bdljsn::WriteStyle::e_PRETTY);
//  writeOptions.setInitialIndentLevel(0);
//  writeOptions.setSpacesPerLevel(2);
//  writeOptions.setSortMembers(true);
//
//  bdljsn::JsonUtil::write(&resultString, result, writeOptions);
//
//  assert(resultString == INPUT_JSON);
//..
//
///Example 2: The Effect of 'options' on 'write'
///- - - - - - - - - - - - - - - - - - - - - - -
// By populating a 'WriteOptions' object and passing it to 'write', the format
// of the resulting JSON can be controlled.
//
// First, let's populate a 'Json' object named 'json' from an input string
// using 'read', and create an empty 'options' (see 'bdljsn::WriteOptions'):
//..
//  const bsl::string JSON = R"JSON(
//    {
//      "a" : 1,
//      "b" : []
//    }
//  )JSON";
//
//  bdljsn::Json         json;
//  bdljsn::WriteOptions options;
//
//  int rc = bdljsn::JsonUtil::read(&json, JSON);
//
//  assert(0 == rc);
//..
// There are 4 options, which can be broken down into 2 unrelated sets.
//
// The first set consists of the 'sortMembers' option, which controls whether
// members of objects are printed in lexicogaphical order.
//
// The second set consists of the 'style', 'initialIndentLevel', and
// 'spacesPerLevel' options - 'style' controls which format is used to render a
// 'Json', and, if 'bdljsn::WriteStyle::e_PRETTY == options.style()', the
// 'spacesPerLevel' and 'initialIndentLevel' options are used to control the
// indentation of the output.  For any other value of 'options.style()', the
// 'spacesPerLevel' and 'initialIndentLevel' options have no effect.
//
///'sortMembers'
///-  -  -  -  -
// If 'sortMembers' is true, then the members of an object output by 'write'
// will be in sorted order.  Otherwise, the elements are written in an
// (implementation defined) order (that may change).
//
// The 'sortMembers' option defaults to 'false' for performance reasons, but
// applications that rely on stable output text should set 'sortMembers' to
// 'true' (e.g., in a test where the resulting JSON text is compared for
// equality) .
//
// Here, we set 'sortMembers' to 'true', and verify the resulting JSON text
// matches the expected text:
//..
//  options.setSortMembers(true);
//  bsl::string output;
//
//  rc = bdljsn::JsonUtil::write(&output, json, options);
//
//  assert(0 == rc);
//  assert(R"JSON({"a":1,"b":[]})JSON" == output);
//..
// Had we not specified 'setSortMembers(true)', the order of the "a" and "b"
// members in the 'output' string would be unpredictable.
//
///'style' And 'style'-related options
/// -  -  -  -  -  -  -  -  -  -  -  -
// There are 3 options for 'style' (see 'bdljsn::WriteStyle'):
//: o bdljsn::WriteStyle::e_COMPACT
//: o bdljsn::WriteStyle::e_ONELINE
//: o bdljsn::WriteStyle::e_PRETTY
//
// Next, we write 'json' using the style 'e_COMPACT' (the default), a single
// line presentation with no added spaces after ':' and ',' elements.
//..
//  rc = bdljsn::JsonUtil::write(&output, json, options);
//
//  assert(0 == rc);
//
//  // Using 'e_COMPACT' style:
//  assert(R"JSON({"a":1,"b":[]})JSON" == output);
//..
// Next, we write 'json' using the 'e_ONELINE' style, another single line
// format, which adds single ' ' characters after ':' and ',' elements for
// readability.
//..
//  options.setStyle(bdljsn::WriteStyle::e_ONELINE);
//  rc = bdljsn::JsonUtil::write(&output, json, options);
//
//  assert(0 == rc);
//
//  // Using 'e_ONELINE' style:
//  assert(R"JSON({"a": 1, "b": []})JSON" == output);
//..
// Next, we write 'json' using the 'e_PRETTY' style, a multiline format where
// newlines are introduced after each (non-terminal) '{', '[', ',', ']', and
// '}' character.  Furthermore, the indentation of JSON rendered in the
// 'e_PRETTY' style is controlled by the other 2 attributes, 'spacesPerLevel'
// and 'initialIndentLevel'.
//
// 'e_PRETTY' styling does not add a newline to the end of the output.
//
// 'spacesPerLevel' controls the number of spaces added for each successive
// indentation level - e.g., if 'spacesPerLevel' is 2, then each nesting level
// of the rendered JSON is indented 2 spaces.
//
// 'initialIndentLevel' controls how much the entire JSON output is indented.
// It defaults to 0 - if it's a positive value, then the entire JSON is
// indented by 'initialIndentLevel * spacesPerLevel' spaces.
//..
//  options.setStyle(bdljsn::WriteStyle::e_PRETTY);
//  options.setSpacesPerLevel(4);     // the default
//  options.setInitialIndentLevel(0); // the default
//
//  rc = bdljsn::JsonUtil::write(&output, json, options);
//
//  assert(0 == rc);
//
//  // Using 'e_PRETTY' style:
//  assert(R"JSON({
//      "a": 1,
//      "b": []
//  })JSON" == output);
//..
// Finally, if we set 'initialIndentLevel' to 1, then an extra set of 4 spaces
// is prepended to each line, where 4 is the value of 'spacesPerLevel':
//..
//  options.setInitialIndentLevel(1);
//
//  rc = bdljsn::JsonUtil::write(&output, json, options);
//
//  assert(0 == rc);
//
//  // Using 'e_PRETTY' style (with 'initialIndentLevel' as 1):
//  assert(R"JSON({
//      "a": 1,
//      "b": []
//  })JSON" == output);
//..

#include <bdlscm_version.h>

#include <bdljsn_error.h>
#include <bdljsn_json.h>
#include <bdljsn_readoptions.h>
#include <bdljsn_writeoptions.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bslmf_movableref.h>

#include <bsls_libraryfeatures.h>

#include <bsl_cstdint.h>
#include <bsl_iosfwd.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdljsn {

                              // ===============
                              // struct JsonUtil
                              // ===============

struct JsonUtil {
    // This 'struct' provides a namespace for utility functions that provide
    // 'read' and 'write' operations to/from 'Json' objects.

    // TYPES
    typedef bsl::pair<bsl::uint64_t, bsl::uint64_t> LineAndColumnNumber;

    // CLASS METHODS
    static int read(Json               *result,
                    bsl::istream&       input);
    static int read(Json               *result,
                    bsl::istream&       input,
                    const ReadOptions&  options);
    static int read(Json               *result,
                    bsl::streambuf     *input);
    static int read(Json               *result,
                    bsl::streambuf     *input,
                    const ReadOptions&  options);
    static int read(Json                    *result,
                    const bsl::string_view&  input);
    static int read(Json                    *result,
                    const bsl::string_view&  input,
                    const ReadOptions&       options);
    static int read(Json               *result,
                    Error              *errorDescription,
                    bsl::istream&       input);
    static int read(Json               *result,
                    Error              *errorDescription,
                    bsl::istream&       input,
                    const ReadOptions&  options);
    static int read(Json               *result,
                    Error              *errorDescription,
                    bsl::streambuf     *input);
    static int read(Json               *result,
                    Error              *errorDescription,
                    bsl::streambuf     *input,
                    const ReadOptions&  options);
    static int read(Json                    *result,
                    Error                   *errorDescription,
                    const bsl::string_view&  input);
    static int read(Json                    *result,
                    Error                   *errorDescription,
                    const bsl::string_view&  input,
                    const ReadOptions&       options);
        // Load to the specified 'result' a value-semantic representation of
        // the JSON text in the specified 'input'.  Optionally specify an
        // 'errorDescription' that, if an error occurs, is loaded with a
        // description of the error.  Optionally specify 'options' which allow
        // altering the maximum nesting depth.  Return 0 on success, and a
        // non-zero value if 'input' does not consist of valid JSON text or an
        // error occurs when reading from 'input'.  If
        // 'options.allowTrailingText()' is 'false' (the default), then an
        // error will be reported if a valid JSON text is followed by any text
        // that does not consist solely of white-space characters.  If
        // 'options.allowTrailingText()' is 'true', then this function will
        // return success where a valid JSON document is followed by additional
        // text as long as that text is separated from the valid JSON by a
        // delimiter character (i.e., either the JSON text ends in a delimiter,
        // or the text that follows starts with a delimiter).  Here, delimiters
        // are white-space characters, '[',']','{','}',',', or '"'.

    static bsl::ostream& printError(bsl::ostream&          stream,
                                    bsl::istream&           input,
                                    const Error&            error);
    static bsl::ostream& printError(bsl::ostream&           stream,
                                    bsl::streambuf         *input,
                                    const Error&            error);
    static bsl::ostream& printError(bsl::ostream&           stream,
                                    const bsl::string_view& input,
                                    const Error&            error);
        // Print, to the specified 'stream', a description of the specified
        // 'error', containing the line and column in the specified 'input'
        // where the 'error' occured.  Return a reference to the modifiable
        // 'stream'.  If 'error.location()' does not refer to a valid location
        // in 'input' an unspecified error description will be written to
        // 'stream'.  Note that the caller should ensure 'input' refers to the
        // same input position as when 'input' was supplied to 'read' (or
        // whatever operation created 'error').

    static int write(bsl::ostream&       output,
                     const Json&         json);
    static int write(bsl::ostream&       output,
                     const Json&         json,
                     const WriteOptions& options);
    static int write(bsl::streambuf      *output,
                     const Json&          json);
    static int write(bsl::streambuf      *output,
                     const Json&          json,
                     const WriteOptions&  options);
    static int write(bsl::string         *output,
                     const Json&          json);
    static int write(bsl::string         *output,
                     const Json&          json,
                     const WriteOptions&  options);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static int write(std::pmr::string    *output,
                     const Json&          json);
    static int write(std::pmr::string    *output,
                     const Json&          json,
                     const WriteOptions&  options);
#endif
    static int write(std::string         *output,
                     const Json&          json);
    static int write(std::string         *output,
                     const Json&          json,
                     const WriteOptions&  options);
        // Write to the specified 'output' a JSON text representation of the
        // specified 'json' document, using the optionally specified 'options'
        // for formatting the resulting text.  Return 0 on success, and a
        // non-zero value otherwise.  Note that this operation will report an
        // error only if there is an error writing to 'output'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                               // --------------
                               // class JsonUtil
                               // --------------
// CLASS METHODS
inline
int JsonUtil::read(Json               *result,
                   bsl::istream&       input,
                   const ReadOptions&  options)
{
    Error tmp;
    return read(result, &tmp, input, options);
}

inline
int JsonUtil::read(Json               *result,
                   bsl::istream&       input)
{
    ReadOptions options;
    return read(result, input, options);
}

inline
int JsonUtil::read(Json               *result,
                   bsl::streambuf     *input,
                   const ReadOptions&  options)
{
    Error tmp;
    return read(result, &tmp, input, options);
}

inline
int JsonUtil::read(Json               *result,
                   bsl::streambuf     *input)
{
    ReadOptions options;
    return read(result, input, options);
}

inline
int JsonUtil::read(Json                    *result,
                   const bsl::string_view&  input,
                   const ReadOptions&       options)
{
    Error tmp;
    return read(result, &tmp, input, options);
}

inline
int JsonUtil::read(Json                    *result,
                   const bsl::string_view&  input)
{
    ReadOptions options;
    return read(result, input, options);
}

inline
int JsonUtil::read(Json               *result,
                   Error              *errorDescription,
                   bsl::istream&       input,
                   const ReadOptions&  options)
{

    return read(result, errorDescription, input.rdbuf(), options);
}

inline
int JsonUtil::read(Json               *result,
                   Error              *errorDescription,
                   bsl::istream&       input)
{
    ReadOptions options;

    return read(result, errorDescription, input, options);
}

inline
int JsonUtil::read(Json           *result,
                   Error          *errorDescription,
                   bsl::streambuf *input)
{
    ReadOptions options;

    return read(result, errorDescription, input, options);
}

inline
int JsonUtil::read(Json                    *result,
                   Error                   *errorDescription,
                   const bsl::string_view&  input,
                   const ReadOptions&       options)
{
    bdlsb::FixedMemInStreamBuf inputBuf(input.data(), input.size());
    return read(result, errorDescription, &inputBuf, options);
}

inline
int JsonUtil::read(Json                    *result,
                   Error                   *errorDescription,
                   const bsl::string_view&  input)
{
    ReadOptions options;
    return read(result, errorDescription, input, options);
}

inline
bsl::ostream& JsonUtil::printError(bsl::ostream& stream,
                                   bsl::istream& input,
                                   const Error&  error)
{
    return printError(stream, input.rdbuf(), error);
}

inline
bsl::ostream& JsonUtil::printError(bsl::ostream&           stream,
                                   const bsl::string_view& input,
                                   const Error&            error)
{
    bdlsb::FixedMemInStreamBuf inputBuf(input.data(), input.size());
    return printError(stream, &inputBuf, error);
}

inline
int JsonUtil::write(bsl::streambuf      *output,
                    const Json&          json,
                    const WriteOptions&  options)
{
    bsl::ostream outputStream(output);
    return write(outputStream, json, options);
}

inline
int JsonUtil::write(bsl::streambuf      *output,
                    const Json&          json)
{
    WriteOptions options;
    return write(output, json, options);
}

inline
int JsonUtil::write(bsl::string         *output,
                    const Json&          json,
                    const WriteOptions&  options)
{
    bsl::ostringstream stream(output->get_allocator());

    int rc = write(stream, json, options);
    if (0 == rc) {
#ifdef BSLS_PLATFORM_CMP_SUN
        const bsl::string &tmpOutput = stream.str();
#else
        bsl::string tmpOutput = stream.str(output->get_allocator());
#endif
        *output = bslmf::MovableRefUtil::move(tmpOutput);
    }
    return rc;
}

inline
int JsonUtil::write(bsl::string         *output,
                    const Json&          json)
{
    WriteOptions options;

    return write(output, json, options);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
int JsonUtil::write(std::pmr::string    *output,
                    const Json&          json,
                    const WriteOptions&  options)
{
#if defined (BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
    typedef std::basic_ostringstream<char,
                                     std::char_traits<char>,
                                     std::pmr::polymorphic_allocator<char> >
        PmrOstringStream;

    PmrOstringStream stream(std::ios_base::out, output->get_allocator());
    int rc = write(stream, json, options);
    if (0 == rc) {
        std::pmr::string tmpOutput = stream.str(output->get_allocator());
        *output = bslmf::MovableRefUtil::move(tmpOutput);
    }
    return rc;
#else
    std::ostringstream stream;
    int rc = write(stream, json, options);
    if (0 == rc) {
        const bsl::string &tmpOutput = stream.str();
        *output = bslmf::MovableRefUtil::move(tmpOutput);
    }
    return rc;
#endif
}
#endif  // defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
int JsonUtil::write(std::pmr::string    *output,
                    const Json&          json)
{
    WriteOptions options;

    return write(output, json, options);
}
#endif  // defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)

inline
int JsonUtil::write(std::string         *output,
                    const Json&          json,
                    const WriteOptions&  options)
{
    std::ostringstream stream;

    int rc = write(stream, json, options);
    if (0 == rc) {
        std::string tmpOutput(stream.str());
        *output = bslmf::MovableRefUtil::move(tmpOutput);
    }
    return rc;
}

inline
int JsonUtil::write(std::string         *output,
                    const Json&          json)
{
    WriteOptions options;

    return write(output, json, options);
}

inline
int JsonUtil::write(bsl::ostream& output, const Json& json)
{
    WriteOptions options;
    return write(output, json, options);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONUTIL

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
