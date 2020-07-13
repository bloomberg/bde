// baljsn_simpleformatter.h                                           -*-C++-*-
#ifndef INCLUDED_BALJSN_SIMPLEFORMATTER
#define INCLUDED_BALJSN_SIMPLEFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple formatter for encoding data in the JSON format.
//
//@CLASSES:
// baljsn::SimpleFormatter: a mechanism to encode data into JSON
//
//@SEE_ALSO: baljsn_encoder, baljsn_formatter, baljsn_printutil
//
//@DESCRIPTION: This component provides a class, 'baljsn::SimpleFormatter', for
// rendering JSON conforming text for objects, arrays, and various scalar
// types.
//
// This component provides an interface that is easier to use, and renders more
// readable "pretty" JSON, than 'baljsn::Formatter'.  Clients are encouraged to
// use 'baljsn::SimpleFormatter' instead of 'baljsn::Formatter' (see
// {Comparison to 'baljsn::Formatter'}).
//
// The 'SimpleFormatter' 'class' also provides the ability to specify
// formatting options at construction.  The options that can be provided
// include the encoding style (compact or pretty), the initial indentation
// level and spaces per level if encoding in the pretty format.
//
///Comparison to 'baljsn::Formatter'
///---------------------------------
//
///API Comparison
/// - - - - - - -
// Here is the side-by-side sequence of calls to create the following JSON
// using both components, assuming an existing stream 'os':
//..
//  {
//    "Object" : {
//      "Field 1" : 1,
//      "Field 2" : null
//    },
//    "Array" : [
//      1,
//      "string",
//      [],
//      [
//        [
//          {
//          }
//        ]
//      ]
//    ],
//    "True" : true
//  }
//..
// Some extra indentation has been added in these examples to show the various
// 'open'/'close' call nesting levels.
//..
//           Formatter                |             SimpleFormatter
// -----------------------------------+----------------------------------------
// baljsn::Formatter f(os);           | baljsn::SimpleFormatter sf(os);
//                                    |
// f.openObject();                    | sf.openObject();
//                                    |
//  f.openMember("Object");           |  sf.openObject("Object");
//   f.openObject();                  |   sf.addValue("Field 1", 1);
//    f.openMember("Field 1");        |   sf.addNullValue("Field 2");
//     f.putValue(1);                 |  sf.closeObject();
//    f.closeMember();                |
//    f.openMember("Field 2");        |  sf.openArray("Array");
//     f.putNullValue();              |   sf.addValue(1);        // No name
//    // Must remember NOT to call    |   sf.addValue("string"); // No name
//    // closeMember here!            |   sf.openArray(e_EMPTY_ARRAY_FORMAT);
//   f.closeObject();                 |   sf.closeArray(e_EMPTY_ARRAY_FORMAT);
//  f.closeMember();                  |   sf.openArray();
//                                    |    sf.openArray();
//  f.openMember("Array");            |     sf.openObject();
//   f.openArray();                   |     sf.closeObject();
//    f.putValue(1);                  |    sf.closeArray();
//    f.addArrayElementSeparator();   |   sf.closeArray();
//    f.putValue("string");           |  sf.closeArray();
//    f.addArrayElementSeparator();   |
//    f.openArray(true);              |  sf.addValue("True", true);
//    f.closeArray(true);             | sf.closeObject();
//    f.addArrayElementSeparator();   |
//    f.openArray();                  |
//     f.openArray();                 |
//      f.openObject();               |
//      f.closeObject();              |
//     f.closeArray();                |
//    f.closeArray();                 |
//                                    |
//    // Must remember NOT to call    |
//    // addArrayElementSeparator     |
//    // here!                        |
//   f.closeArray();                  |
//  f.closeMember();                  |
//                                    |
//  f.openMember("True");             |
//   f.putValue(true);                |
//  // Must remember NOT to call      |
//  // closeMember here!              |
//                                    |
// f.closeObject();                   |
// -----------------------------------+----------------------------------------
//
//..
//
// JSON Format
// -----------
// The JSON encoding format (see http://json.org or ECMA-404 standard for more
// information) specifies a self-describing and simple syntax that is built on
// two structures:
//
//: o Objects: JSON objects are represented as collections of name value
//:   pairs.  The 'SimpleFormatter' 'class' allows encoding objects by
//:   providing the 'openObject' and 'closeObject' methods to open and close an
//:   object and overloads for 'openObject', 'openArray', 'addValue' and
//:   'addNullValue' which take a 'name' to specify the named fields in the
//:   object, or the use of the 'addMemberName' manipulator followed by the
//:   overloads of 'openObject', 'openArray', 'addValue', and 'addNullValue'
//:   which do not take a name.
//:
//: o Arrays: JSON arrays are specified as an ordered list of values.  The
//:   'SimpleFormatter' 'class' provides the 'openArray' and 'closeArray'
//:   method to open and close an array, as well as overloads for 'openObject',
//:   'openArray', 'addValue' and 'addNullValue' which do not take a 'name' for
//:   array elements.
//
// The 'SimpleFormatter' 'class' provides the ability to specify formatting
// options at construction.  The options that can be provided include the
// encoding style (compact or pretty), the initial indentation level and spaces
// per level if encoding in the pretty format.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a Stock Portfolio in JSON
///- - - - - - - - - - - - - - - - - - - - - - -
// Let us suppose we have to encode a JSON document containing information
// about a small portfolio of stocks.  The eventual data we want to encode is
// represented by the following JSON string (which is the expected output of
// the encoding process):
//
// First, we specify the result that we are expecting to get:
//..
//{
//  const bsl::string EXPECTED = R"JSON({
//    "Stocks" : [
//      {
//        "Name" : "International Business Machines Corp",
//        "Ticker" : "IBM US Equity",
//        "Last Price" : 149.3,
//        "Dividend Yield" : 3.95
//      },
//      {
//        "Name" : "Apple Inc",
//        "Ticker" : "AAPL US Equity",
//        "Last Price" : 205.8,
//        "Dividend Yield" : 1.4
//      }
//    ]
//  })JSON";
//..
// Then, to encode this JSON document we create a 'baljsn::SimpleFormatter'
// object.  Since we want the document to be written in a pretty, easy to
// understand format we will specify 'true' for the 'usePrettyStyle' option and
// provide an appropriate initial indent level and spaces per level values:
//..
//  bsl::ostringstream      os;
//  baljsn::EncoderOptions  encoderOptions;
//
//  encoderOptions.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
//  encoderOptions.setSpacesPerLevel(2);
//
//  baljsn::SimpleFormatter formatter(os, encoderOptions);
//..
// Next, we encode the start of the top level object, and open the first member
// "Stocks" (which holds an array of stock information):
//..
//  formatter.openObject();
//  formatter.openArray("Stocks");
//..
// Next, we render each element within the array of "Stocks" as an object that
// contains information for an individual stock:
//..
//  formatter.openObject();
//..
// We now encode the other elements in the stock object.
//..
//  formatter.addValue("Name", "International Business Machines Corp");
//  formatter.addValue("Ticker", "IBM US Equity");
//  formatter.addValue("Last Price", 149.3);
//  formatter.addValue("Dividend Yield", 3.95);
//..
// Then, close the first stock object.
//..
//  formatter.closeObject();
//..
// Next, we add another stock object.
//..
//  formatter.openObject();
//
//  formatter.addValue("Name", "Apple Inc");
//  formatter.addValue("Ticker", "AAPL US Equity");
//  formatter.addValue("Last Price", 205.8);
//  formatter.addValue("Dividend Yield", 1.4);
//
//  formatter.closeObject();
//..
// Similarly, we can continue to format the rest of the document.  For the
// purpose of this usage example we will complete this document.
//..
//  formatter.closeArray();
//  formatter.closeObject();
//..
// Once the formatting is complete the written data can be viewed from the
// stream passed to the formatter at construction.
//..
//  if (verbose)
//      bsl::cout << os.str() << bsl::endl;
//..
// Finally, verify the received result:
//..
//  assert(EXPECTED == os.str());
//}
//..
//
///Example 2: Encoding an array
///- - - - - - - - - - - - - - -
// Let us say we want to encode an array of various values.
//
// First, we create our 'formatter' as we did above:
//..
//{
//  bsl::ostringstream      os;
//  baljsn::EncoderOptions  encoderOptions;
//
//  encoderOptions.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
//  encoderOptions.setSpacesPerLevel(2);
//
//  baljsn::SimpleFormatter formatter(os, encoderOptions);
//..
// Then we open our array.
//..
//  formatter.openArray();
//..
// Next, we populate the array with a series of unnamed values.  Named values
// are only used in objects, not arrays.
//..
//  formatter.addValue("First value");
//  formatter.addValue(2);
//  formatter.addValue(3);
//..
// Then, we demonstrate that arrays can be nested, opening another level of
// array, populating it, and closing it:
//..
//  formatter.openArray();
//  formatter.addValue("First value of inner array");
//  formatter.addValue(3.14159);
//  formatter.closeArray();
//..
// Arrays can also contain (unnamed) objects:
//..
//  formatter.openObject();
//..
// Next, we add (named) values to our object:
//..
//  formatter.addValue("Greeting", "Hello from the first inner object");
//  formatter.addValue("PI approximation", 3.14);
//  // We could, similarly, add nested named objects and/or named arrays
//..
// Then we close the nested object:
//..
//  formatter.closeObject();
//..
// Finally, we close the outer array:
//..
//  formatter.closeArray();
//}
//..

#include <balscm_version.h>

#include <baljsn_encoderoptions.h>
#include <baljsn_printutil.h>

#include <bdlb_print.h>

#include <bdlc_bitarray.h>

#include <bsl_ostream.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_review.h>

namespace BloombergLP {
namespace baljsn {

class EncoderOptions;

                           // =====================
                           // class SimpleFormatter
                           // =====================

class SimpleFormatter {
    // This class implements a formatter providing operations for rendering
    // JSON text elements to an output stream (supplied at construction)
    // according to a set of formatting options (also supplied at
    // construction).
    //
    // This class has an interface that's easier to use than that of
    // 'baljsn::Formatter', and generates more correctly-formatted 'pretty'
    // output.

  public:
    // TYPES
    enum ArrayFormattingStyle {
        // This 'enum' lists all possible array formatting styles.
        e_REGULAR_ARRAY_FORMAT = 1,
        e_EMPTY_ARRAY_FORMAT
    };

  private:
    // DATA
    bsl::ostream&     d_outputStream;        // stream for output (held, not
                                             // owned)

    bool              d_useComma;            // whether next start item
                                             // ('add*', 'open*') needs a
                                             // preceding comma

    bool              d_started;             // whether we've formatted at
                                             // least one element

    bool              d_memberNameSupplied;  // whether the previous output
                                             // operation was 'addMemberName'

    bdlc::BitArray    d_callSequence;        // array specifying the sequence
                                             // in which the 'openObject' and
                                             // 'openArray' methods were
                                             // called.  An 'openObject' call
                                             // is represented by 'false' and
                                             // an 'openArray' call by 'true'.

    EncoderOptions    d_encoderOptions;      // formatting and encoding
                                             // options

    int               d_indentLevel;         // current indent level

    // PRIVATE MANIPULATORS
    void indent();
        // Unconditionally print onto the stream supplied at construction the
        // sequence of whitespace characters for the proper indentation of an
        // element at the current indentation level.  Note that this method
        // does not check that 'usePrettyStyle()' is 'true' before indenting.

    void printComma();
        // If 'd_useComma' is 'true', print a comma.  If 'usePrettyStyle()' is
        // also 'true', also print a newline.  This also sets
        // 'd_memberNameSupplied' to 'false'.

    void followWithComma(bool flag);
        // Set 'd_useComma' to the value of the specified 'flag', indicating
        // whether the next 'printComma()' call should actually print a comma.

    void printName(const bslstl::StringRef& name);
        // Print onto the stream supplied at construction the specified 'name',
        // followed by a ':'. The ':' is surrounded by a space on each side if
        // 'usePrettyStyle()' is 'true'.  It is the caller's responsibility to
        // call 'printComma()' - this routine does not handle commas, but does
        // call 'indent()' if necessary.

    // PRIVATE ACCESSORS
    bool usePrettyStyle() const;
        // Return 'true' if 'e_PRETTY == d_encoderOptions.encodingStyle()'.

    int spacesPerLevel() const;
        // Return 'd_encoderOptions.spacesPerLevel()'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SimpleFormatter,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit SimpleFormatter(bsl::ostream&          stream,
                             bslma::Allocator      *basicAllocator = 0);
    explicit SimpleFormatter(bsl::ostream&          stream,
                             const EncoderOptions&  encoderOptions,
                             bslma::Allocator      *basicAllocator = 0);
        // Create a 'SimpleFormatter' object using the specified 'stream'.
        // Optionally specify 'encoderOptions' to configure the output options
        // - if 'encoderOptions' is not supplied, a default-constructed
        // 'EncoderOptions' object will be used.  Note that the
        // 'encodeEmptyArrays' attribute in the 'encoderOptions' is ignored.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SimpleFormatter(const SimpleFormatter&  original,
                    bslma::Allocator       *basicAllocator);
        // Create a 'SimpleFormatter' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~SimpleFormatter();
        // Destroy this object.  Note that correct JSON has been generated if
        // the 'isCompleteJSON()' call returns 'true'.

    // MANIPULATORS
    void openObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an object (referred to as an
        // "object" in JSON), preceded, if necessary, by a comma.  The behavior
        // is undefined unless 'isNameNeeded()' is 'false'.

    void openObject(const bslstl::StringRef& name);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an object (referred to as an
        // "object" in JSON) with the specified 'name' , preceded, if
        // necessary, by a comma.  The behavior is undefined unless
        // 'isNameNeeded()' is 'true'.

    void addMemberName(const bslstl::StringRef& name);
        // Print onto the stream supplied at construction the specified 'name'
        // in double-quotes, preceded, if necessary, by a comma, and followed
        // by a ':'.  The behavior is undefined unless 'isNameNeeded()' is
        // 'true'.  After this operation, 'isNameNeeded()' will be 'false', and
        // an immediately subsequent attempt to add a value (or open an object
        // or array) should not provide a name.

    void closeObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an object (referred to as an
        // "object" in JSON).  The behavior is undefined unless
        // 'isNameNeeded()' is 'true'.

    void openArray(
                ArrayFormattingStyle formattingStyle = e_REGULAR_ARRAY_FORMAT);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an array (referred to as an
        // "array" in JSON), preceded, if necessary, by a comma.  Optionally
        // specify 'formattingStyle' denoting if the array being opened should
        // be formatted as an empty array.  If 'formattingStyle' is not
        // specified then the array being opened is formatted as a regular
        // array having elements.    The behavior is undefined unless
        // 'isNameNeeded()' is 'false'.  Note that the formatting (and as a
        // consequence the 'formattingStyle') is relevant only if this
        // formatter encodes in the pretty style and is ignored otherwise.

    void openArray(
            const bslstl::StringRef& name,
            ArrayFormattingStyle     formattingStyle = e_REGULAR_ARRAY_FORMAT);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an array (referred to as an
        // "array" in JSON) with the specified 'name', preceded, if necessary,
        // by a comma.  Optionally specify 'formattingStyle' denoting if the
        // array being opened should be formatted as an empty array.  If
        // 'formattingStyle' is not specified then the array being opened is
        // formatted as a regular array having elements.  The behavior is
        // undefined unless 'isNameNeeded()' is 'true'.  Note that the
        // formatting (and as a consequence the 'formattingStyle') is relevant
        // only if this formatter encodes in the pretty style and is ignored
        // otherwise.

    void closeArray(
                ArrayFormattingStyle formattingStyle = e_REGULAR_ARRAY_FORMAT);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an array (referred to as an
        // "array" in JSON).  Optionally specify 'formattingStyle' denoting if
        // the array being closed should be formatted as an empty array.  If
        // 'formattingStyle' is not specified then the array being closed is
        // formatted as a regular array having elements.  The behavior is
        // undefined if 'isFormattingArray()' is 'false'.  Note that the
        // formatting (and as a consequence the 'formattingStyle') is relevant
        // only if this formatter encodes in the pretty style and is ignored
        // otherwise.

    void addNullValue();
        // Print onto the stream supplied at construction the value
        // corresponding to a null element, preceded, if necessary, by a comma.
        // The behavior is undefined unless 'isNameNeeded()' is 'false'.

    void addNullValue(const bslstl::StringRef& name);
        // Print onto the stream supplied at construction the value
        // corresponding to a null element with the specified 'name', preceded,
        // if necessary, by a comma.  The behavior is undefined unless
        // 'isNameNeeded()' is 'true'.

    template <class TYPE>
    int addValue(const TYPE& value);
        // Print onto the stream supplied at construction the specified
        // 'value', preceded, if necessary, by a comma, passing the optionally
        // specified 'options' through to the rendering routines.  Return 0 on
        // success and a non-zero value otherwise.  The behavior is undefined
        // unless 'isNameNeeded()' is 'false'.

    template <class TYPE>
    int addValue(const bslstl::StringRef& name, const TYPE& value);
        // Print onto the stream supplied at construction the specified 'name'
        // and the specified 'value', preceded, if necessary, by a comma,
        // passing the optionally specified 'options' through to the rendering
        // routines.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'isNameNeeded()' is 'true'.

    // ACCESSORS
    bool isCompleteJSON() const;
        // Return 'true' if this 'SimpleFormatter' has formatted a complete
        // JSON object, where all 'open*' calls have been balanced by their
        // corresponding 'close*' calls.  Note that a default-constructed
        // 'SimpleFormatter' will return 'false' - an empty string is not valid
        // JSON.

    bool isFormattingArray() const;
        // Return 'true' if this 'SimpleFormatter' is currently formatting an
        // array and 'false' otherwise.  It is formatting an array if the last
        // 'open*' method overload ('openArray' or 'openObject') called on this
        // 'SimpleFormatter' for which the corresponding 'close*' method
        // (respectively, 'closeArray' or 'closeObject') was 'openArray'.  If
        // 'isFormattingArray()' is 'true', then 'isFormattingObject()' is
        // 'false'.  Note that both can be 'false', at the 'top-level' initial
        // scope before anything is added/opened or after the first 'open*'
        // call has been closed'

    bool isFormattingObject() const;
        // Return 'true' if this 'SimpleFormatter' is currently formatting an
        // object scope  and 'false' otherwise.  It is formatting an object
        // scope if the last 'open*' method overload ('openArray' or
        // 'openObject') called on this 'SimpleFormatter' for which the
        // corresponding 'close*' method (respectively, 'closeArray' or
        // 'closeObject') was 'openObject'.  If 'isFormattingObject()' is
        // 'true', then 'isFormattingArray()' is 'false'.  Note that both can
        // be 'false', at the 'top-level' initial scope before anything is
        // added/opened or after the first 'open*' call has been closed'
        // JSON.

    bool isNameNeeded() const;
        // Return 'true' if a subsequent attempt to add a value must supply a
        // 'name', and 'false' otherwise.  This will be 'true' if
        // 'isFormattingObject()' is 'true', and 'addMemberName()' was not the
        // most recently called manipulator.  That is, a name is needed if this
        // formatter is currently in the context of formatting the members of a
        // JSON object, and 'addMemberName' has not been called to explicitly
        // provide a name for the next member.

    // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // class SimpleFormatter
                        // ---------------

// PRIVATE MANIPULATORS
inline
void SimpleFormatter::followWithComma(bool flag)
{
    d_useComma = flag;
}

inline
void SimpleFormatter::indent()
{
    bdlb::Print::indent(d_outputStream, d_indentLevel, spacesPerLevel());
}

inline
void SimpleFormatter::printComma()
{
    d_started = true;

    if (d_useComma) {
        d_outputStream << ',';

        if (usePrettyStyle()) {
            d_outputStream << '\n';
        }
    }

    d_memberNameSupplied = false;
}

inline
void SimpleFormatter::printName(const bslstl::StringRef& name)
{
    if (usePrettyStyle()) {
        indent();
    }

    const int rc = PrintUtil::printValue(d_outputStream, name);
    if (rc) {
        return;                                                       // RETURN
    }

    d_outputStream << (usePrettyStyle() ? " : " : ":");
}

// PRIVATE ACCESSORS
inline
int SimpleFormatter::spacesPerLevel() const
{
    return d_encoderOptions.spacesPerLevel();
}

inline
bool SimpleFormatter::usePrettyStyle() const
{
    return EncoderOptions::e_PRETTY == d_encoderOptions.encodingStyle();
}

// MANIPULATORS
inline
void SimpleFormatter::addMemberName(const bslstl::StringRef& name)
{
    BSLS_ASSERT(isNameNeeded());

    printComma();
    followWithComma(false);

    printName(name);

    d_memberNameSupplied = true;
}

inline
void SimpleFormatter::addNullValue()
{
    BSLS_ASSERT(!isNameNeeded());

    bool needIndent = usePrettyStyle() && !d_memberNameSupplied;

    printComma();
    followWithComma(true);

    if (needIndent) {
        indent();
    }

    d_outputStream << "null";
}

inline
void SimpleFormatter::addNullValue(const bslstl::StringRef& name)
{
    BSLS_ASSERT(isNameNeeded());

    printComma();
    followWithComma(true);

    printName(name);

    d_outputStream << "null";
}

template <class TYPE>
int SimpleFormatter::addValue(const TYPE& value)
{
    BSLS_ASSERT(!isNameNeeded());

    bool needIndent = usePrettyStyle() && !d_memberNameSupplied;

    printComma();
    followWithComma(true);

    if (needIndent) {
        indent();
    }

    return baljsn::PrintUtil::printValue(
        d_outputStream, value, &d_encoderOptions);
}

template <class TYPE>
int SimpleFormatter::addValue(const bslstl::StringRef& name, const TYPE& value)
{
    BSLS_ASSERT(isNameNeeded());

    printComma();
    followWithComma(true);

    printName(name);

    return baljsn::PrintUtil::printValue(
        d_outputStream, value, &d_encoderOptions);
}

// ACCESSORS
inline
bool SimpleFormatter::isCompleteJSON() const
{
    return d_started && (1 == d_callSequence.length());
}

inline
bool SimpleFormatter::isFormattingArray() const
{
    BSLS_ASSERT(d_callSequence.length() >= 1);

    return d_callSequence.length() > 1 &&
           d_callSequence[d_callSequence.length() - 1];
}

inline
bool SimpleFormatter::isFormattingObject() const
{
    BSLS_ASSERT(d_callSequence.length() >= 1);

    return d_callSequence.length() > 1 &&
           !d_callSequence[d_callSequence.length() - 1];
}

inline
bool SimpleFormatter::isNameNeeded() const
{
    return isFormattingObject() && !d_memberNameSupplied;
}

                                  // Aspects

inline
bslma::Allocator *SimpleFormatter::allocator() const
{
    return d_callSequence.allocator();
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
