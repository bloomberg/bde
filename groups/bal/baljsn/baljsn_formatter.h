// baljsn_formatter.h                                                 -*-C++-*-
#ifndef INCLUDED_BALJSN_FORMATTER
#define INCLUDED_BALJSN_FORMATTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter for encoding data in the JSON format.
//
//@CLASSES:
// baljsn::Formatter: JSON formatter
//
//@SEE_ALSO: baljsn_encoder, baljsn_printutil
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a class, 'baljsn::Formatter', for
// encoding value-semantic objects in the JSON encoding format to a specified
// output stream.
//
// The JSON encoding format (see http://json.org or ECMA-404 standard for more
// information) specifies a self-describing and simple syntax that is built on
// two structures:
//..
//: o Objects. JSON objects are represented as collections of name value pairs.
//: The 'Formatter' class allows encoding objects by providing the 'openObject'
//: and 'closeObject' methods to open and close an object and the
//: 'openElement', 'closeElement', and 'putValue' methods to add elements and
//: values to an object.
//:
//: o Arrays. JSON arrays are specified as an ordered list of values.  The
//: 'Formatter' 'class' provides the 'openArray' and 'closeArray' method to
//: open and close an array
//..
//
// The 'Formatter' 'class' also provides the ability to specify formatting
// options at construction.  The options that can be provided include the
// encoding style (compact or pretty), the initial indentation level and spaces
// per level if encoding in the pretty format.
//
// Valid sequence of operations
// - - - - - - - - - - - - - -
// The 'Formatter' 'class' does not maintain internal state to verify that the
// sequence of operations called on its object result in a valid JSON document.
// It is the user's responsibility to ensure that the methods provided by this
// component are called in the right order.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Let us say that we have to encode a JSON document with the following
// information about stocks that we are interested in (for brevity we just
// show and encode a part of the complete document):
//..
//  {
//    "Stocks" : [
//      {
//        "Name" : "International Business Machines Corp",
//        "Ticker" : "IBM US Equity",
//        "Last Price" : 149.3,
//        "Dividend Yield" : 3.95
//      },
//      ...
//    ]
//  }
//..
// To encode this JSON document we first create a 'baljsn::Formatter' object.
// Since we want the document to be written in a pretty, easy to understand
// format we will specify the 'true' for the 'usePrettyStyle' option and
// provide an appropriate initial indent level and spaces per level values:
//..
//  bsl::ostringstream os;
//  baljsn::Formatter formatter(os, true, 0, 2);
//..
// Next, we start calling the sequence of methods requires to produce this
// document.  We start with the top level object and add an element named
// 'Stocks' to it:
//..
//  formatter.openObject();
//  formatter.openElement("Stocks");
//..
// Then, we see that 'Stocks' is an array element so we specify the start of
// the array:
//..
//  formatter.openArray();
//..
// Next, each element within 'Stocks' is an object that contains the
// information for an individual stock.  So we have to output an object here.
// However, there is an additional step that the 'Formatter' 'class' requires.
// When writing data in pretty style the 'isArrayElement' flag is required to
// be set before encoding elements.  Setting this flag allows the formatter to
// correctly distinguish between elements that are part of an object and those
// that are part of an array and encode them appropriately.
//..
//  formatter.setIsArrayElement(true);
//  formatter.openObject();
//..
// We now revert back the 'isArrayElement' flag as we start encoding elements
// in an object.  Note that the 'isArrayElement' flag is only relevant if the
// pretty encoding style is used.  Otherwise, users can safely ignore that
// flag.
//..
//  formatter.setIsArrayElement(false);
//..
// We now encode the other elements in the stock object.  The 'closeElement'
// terminates the element by adding a ',' at the end.  For the last element in
// an object do not call the 'closeElement' method.
//..
//  formatter.openElement("Name");
//  formatter.putValue("International Business Machines Corp");
//  formatter.closeElement();
//
//  formatter.openElement("Ticker");
//  formatter.putValue("IBM US Equity");
//  formatter.closeElement();
//
//  formatter.openElement("Last Price");
//  formatter.putValue(149.3);
//  formatter.closeElement();
//
//  formatter.openElement("Divident Yield");
//  formatter.putValue(3.95);
//  // Note no call to 'closeElement' for the last element
//..
// Similarly, we can continue to format the rest of the document.  For the
// purpose of this usage example we will complete this document.
//..
//  formatter.closeObject();
//  formatter.closeArray();
//  formatter.closeObject();
//..
// Once the formatting is complete the written data can be viewed from the
// stream passed to the formatter at construction.
//..
//  bsl::cout << os.str() << bsl::endl;
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALJSN_PRINTUTIL
#include <baljsn_printutil.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {
namespace baljsn {

class EncoderOptions;

                          // ===============
                          // class Formatter
                          // ===============

class Formatter {
    // This class implements a formatter providing operations for rending JSON
    // text elements to an output stream (supplied at construction) according
    // to a set of formatting options (also supplied at construction).

    // DATA
    bsl::ostream& d_outputStream;     // stream for output (held, not owned)
    bool          d_usePrettyStyle;   // encoding style
    int           d_indentLevel;      // current indentation level
    int           d_spacesPerLevel;   // spaces per indentation level
    bool          d_isArrayElement;   // is current element part of an array

  public:
    // CREATORS
    Formatter(bsl::ostream& stream,
              bool          usePrettyStyle = false,
              int           initialIndentLevel = 0,
              int           spacesPerLevel = 0);
        // Create a 'Formatter' object using the specified 'stream'.
        // Optionally specify 'usePrettyStyle' to inform the formatter whether
        // the pretty encoding style should be used when writing data.  If
        // 'usePrettyStyle' is not specified then the data is written in a
        // compact style.  If 'usePrettyStyle' is specified, additionally
        // specify 'initialIndentLevel' and 'spacesPerLevel' to provide the
        // initial indentation level and spaces per level at which the data
        // should be formatted.  If 'initialIndentLevel' or 'spacesPerLevel' is
        // not specified then an initial value of '0' is used for both
        // parameters.  If 'usePrettyStyle' is 'false' then
        // 'initialIndentLevel' and 'spacesPerLevel' are both ignored.

    //! ~Formatter() = default;
        // Destroy this object.

    // MANIPULATORS
    void openObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an object.

    void closeObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an object.

    void openArray(bool formatAsEmptyArray = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an array.  Optionally specify
        // 'formatAsEmptyArray' denoting if the array being opened should be
        // formatted as an empty array.  If 'formatAsEmptyArray' is not
        // specified then the array being opened is formatted as an array
        // having elements.  Note that the formatting (and as a consequence the
        // 'formatAsEmptyArray') is relevant only if this formatter encodes in
        // the pretty style and is ignored otherwise.

    void closeArray(bool formatAsEmptyArray = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an array.  Optionally specify
        // 'formatAsEmptyArray' denoting if the array being closed should be
        // formatted as an empty array.  If 'formatAsEmptyArray' is not
        // specified then the array being closed is formatted as an array
        // having elements.  Note that the formatting (and as a consequence the
        // 'formatAsEmptyArray') is relevant only if this formatter encodes in
        // the pretty style and is ignored otherwise.

    int openElement(const bsl::string& name);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an element having the specified
        // 'name'.  Return 0 on success and a non-zero value otherwise.

    template <class TYPE>
    int putValue(const TYPE& value, const EncoderOptions *options = 0);
        // Print onto the stream supplied at construction the specified
        // 'value'.  Optionally specify 'options' according which 'value'
        // should be encoded.  Return 0 on success and a non-zero value
        // otherwise.

    void closeElement();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an element.

    void setIsArrayElement(bool isArrayElement);
        // Set the flag denoting if the current element refers to an array
        // element to the specified 'isArrayElement'.

    void indent();
        // Print onto the stream supplied at construction the sequence of
        // whitespace characters for the proper indentation of an element at
        // the current indentation level and based on the encoding options
        // supplied at construction.

    // ACCESSORS
    bool isArrayElement() const;
        // Return the value of the flag denoting if the current element refers
        // to an array element.
};


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // class Formatter
                        // ---------------

// MANIPULATORS
inline
void Formatter::setIsArrayElement(bool isArrayElement)
{
    d_isArrayElement = isArrayElement;
}

// ACCESSORS
inline
bool Formatter::isArrayElement() const
{
    return d_isArrayElement;
}

template <class TYPE>
int Formatter::putValue(const TYPE& value, const EncoderOptions *options)
{
    if (d_isArrayElement) {
        indent();
    }
    return baljsn::PrintUtil::printValue(d_outputStream, value, options);
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
