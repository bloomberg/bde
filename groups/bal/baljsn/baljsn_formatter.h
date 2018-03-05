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
// formatting JSON objects, arrays, and name-value pairs in the JSON encoding
// format to a specified output stream.
//
// The JSON encoding format (see http://json.org or ECMA-404 standard for more
// information) specifies a self-describing and simple syntax that is built on
// two structures:
//
//: o Objects: JSON objects are represented as collections of name value
//:   pairs.  The 'Formatter' class allows encoding objects by providing the
//:   'openObject' and 'closeObject' methods to open and close an object and
//:   the 'openMember', 'closeMember', and 'putValue' methods to add members
//:   and values to an object.
//:
//: o Arrays: JSON arrays are specified as an ordered list of values.  The
//:   'Formatter' 'class' provides the 'openArray' and 'closeArray' method to
//:   open and close an array.
//
// The 'Formatter' 'class' also provides the ability to specify formatting
// options at construction.  The options that can be provided include the
// encoding style (compact or pretty), the initial indentation level and spaces
// per level if encoding in the pretty format.
//
// Valid sequence of operations
// - - - - - - - - - - - - - -
// The 'Formatter' 'class' does only minimal checking to verify that the
// sequence of operations called on its object result in a valid JSON
// document.  It is the user's responsibility to ensure that the methods
// provided by this component are called in the right order.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a Stock Portfolio in JSON
///- - - - - - - - - - - - - - - - - - - - - - -
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
//  formatter.openMember("Stocks");
//..
// Then, we see that 'Stocks' is an array element so we specify the start of
// the array:
//..
//  formatter.openArray();
//..
// Next, each element within 'Stocks' is an object that contains the
// information for an individual stock.  So we have to output an object here:
//..
//  formatter.openObject();
//..
// We now encode the other elements in the stock object.  The 'closeMember'
// terminates the element by adding a ',' at the end.  For the last element in
// an object do not call the 'closeMember' method.
//..
//  formatter.openMember("Name");
//  formatter.putValue("International Business Machines Corp");
//  formatter.closeMember();
//
//  formatter.openMember("Ticker");
//  formatter.putValue("IBM US Equity");
//  formatter.closeMember();
//
//  formatter.openMember("Last Price");
//  formatter.putValue(149.3);
//  formatter.closeMember();
//
//  formatter.openMember("Divident Yield");
//  formatter.putValue(3.95);
//  // Note no call to 'closeMember' for the last element
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

#ifndef INCLUDED_BDLB_PRINT
#include <bdlb_print.h>
#endif

#ifndef INCLUDED_BDLC_BITARRAY
#include <bdlc_bitarray.h>
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
    // This class implements a formatter providing operations for rendering
    // JSON text elements to an output stream (supplied at construction)
    // according to a set of formatting options (also supplied at
    // construction).

    // DATA
    bsl::ostream&     d_outputStream;        // stream for output (held, not
                                             // owned)

    bool              d_usePrettyStyle;      // encoding style

    int               d_indentLevel;         // current indentation level

    int               d_spacesPerLevel;      // spaces per indentation level

    bdlc::BitArray    d_callSequence;        // array specifying the sequence
                                             // in which the 'openObject' and
                                             // 'openArray' methods were
                                             // called.  An 'openObject' call
                                             // is represented by 'false' and
                                             // an 'openArray' call by 'true'.

    // PRIVATE MANIPULATORS
    void indent();
        // Unconditionally print onto the stream supplied at construction the
        // sequence of whitespace characters for the proper indentation of an
        // element at the current indentation level.  Note that this method
        // does not check that 'd_usePrettyStyle' is 'true' before indenting.

    // PRIVATE ACCESSORS
    bool isArrayElement() const;
        // Return 'true' if the value being encoded is an element of an array,
        // and 'false' otherwise.  A value is identified as an element of an
        // array if 'openArray' was called on this object and was not
        // subsequently followed by either an 'openObject' or 'closeArray'
        // call.

  public:
    // CREATORS
    Formatter(bsl::ostream&     stream,
              bool              usePrettyStyle     = false,
              int               initialIndentLevel = 0,
              int               spacesPerLevel     = 0,
              bslma::Allocator *basicAllocator     = 0);
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
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~Formatter();
        // Destroy this object.  The behavior is undefined unless each call to
        // 'openObject', 'openArray', and 'openMember' made on this object was
        // matched with a corresponding call to 'closeObject', 'closeArray',
        // and 'closeMember'.

    // MANIPULATORS
    void openObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an object (referred to as an
        // "object" in JSON).

    void closeObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an object (referred to as an
        // "object" in JSON).  The behavior is undefined unless this
        // 'Formatter' is currently formatting an object.

    void openArray(bool formatAsEmptyArray = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an array (referred to as an
        // "array" in JSON).  Optionally specify 'formatAsEmptyArray' denoting
        // if the array being opened should be formatted as an empty array.  If
        // 'formatAsEmptyArray' is not specified then the array being opened is
        // formatted as an array having elements.  Note that the formatting
        // (and as a consequence the 'formatAsEmptyArray') is relevant only if
        // this formatter encodes in the pretty style and is ignored otherwise.

    void closeArray(bool formatAsEmptyArray = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an array (referred to as an
        // "array" in JSON).  Optionally specify 'formatAsEmptyArray' denoting
        // if the array being closed should be formatted as an empty array.  If
        // 'formatAsEmptyArray' is not specified then the array being closed is
        // formatted as an array having elements.  The behavior is undefined
        // unless this 'Formatter' is currently formatting an array.  Note that
        // the formatting (and as a consequence the 'formatAsEmptyArray') is
        // relevant only if this formatter encodes in the pretty style and is
        // ignored otherwise.

    int openMember(const bsl::string& name);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of a member (referred to as a
        // "name/value pair" in JSON) having the specified 'name'.  Return 0 on
        // success and a non-zero value otherwise.

    void putNullValue();
        // Print onto the stream supplied at construction the value
        // corresponding to a null element.

    template <class TYPE>
    int putValue(const TYPE& value, const EncoderOptions *options = 0);
        // Print onto the stream supplied at construction the specified
        // 'value'.  Optionally specify 'options' according which 'value'
        // should be encoded.  Return 0 on success and a non-zero value
        // otherwise.

    void closeMember();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an member (referred to as a
        // "name/value pair" in JSON).  The behavior is undefined unless this
        // 'Formatter' is currently formatting a member.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // class Formatter
                        // ---------------

// PRIVATE MANIPULATORS
inline
void Formatter::indent()
{
    bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
}

// PRIVATE ACCESSORS
inline
bool Formatter::isArrayElement() const
{
    BSLS_ASSERT_SAFE(d_callSequence.length() >= 1);

    return d_callSequence[d_callSequence.length() - 1];
}

// MANIPULATORS
inline
void Formatter::putNullValue()
{
    if (d_usePrettyStyle && isArrayElement()) {
        indent();
    }
    d_outputStream << "null";
}

template <class TYPE>
int Formatter::putValue(const TYPE& value, const EncoderOptions *options)
{
    if (d_usePrettyStyle && isArrayElement()) {
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
