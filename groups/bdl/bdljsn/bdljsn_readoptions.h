// bdljsn_readoptions.h                                               -*-C++-*-
#ifndef INCLUDED_BDLJSN_READOPTIONS
#define INCLUDED_BDLJSN_READOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide options for reading a JSON document.
//
//@CLASSES:
//  bdljsn::ReadOptions: options for reading a JSON document
//
//@SEE_ALSO: bdljsn_jsonutil, bdljsn_json
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'bdljsn::ReadOptions', that is used to
// specify options for reading a JSON document (see {'bdljsn_jsonutil'}).
//
///Attributes
///----------
//..
//  Name                Type           Default         Simple Constraints
//  ------------------  -----------    -------         ------------------
//  maxNestedDepth      int            64              > 0
//  allowTrailingText   bool           false
//..
//: o 'maxNestedDepth': the maximum depth to which JSON objects and arrays are
//:   allowed to be nested before the JSON decoder reports an error.  For
//:   example, if 'maxNestedDepth' is 8, and a JSON text has 9 consecutive open
//:   brackets ('[') then decoding will return an error.  This option can be
//:   used to prevent poorly formed (or malicious) JSON text from causing a
//:   stack overflow.
//:
//: o 'allowTrailingText': whether a read operation will report an error
//:   if any non-white space text follows a valid JSON document.  By default
//:   this option is 'false', indicating the user expects the input to contain
//:   a single valid JSON document (without any subsequent text).  When
//:   set to 'true' a 'read' operation will return success if there is text
//:   following a valid JSON document, assuming that text is separated by
//:   a delimeter.  See {'bdljsn_jsonutil'} for details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for decoding 'Datum' objects in the JSON format.  This example shows how to
// create and populate an options object.
//
// First, we default-construct a 'bdljsn::ReadOptions' object:
//..
//  const int MAX_NESTED_DEPTH = 16;
//
//  bdljsn::ReadOptions options;
//  assert(64    == options.maxNestedDepth());
//  assert(false == options.allowTrailingText());
//..
// Finally, we populate that object to limit the maximum nested depth using a
// pre-defined limit:
//..
//  options.setMaxNestedDepth(MAX_NESTED_DEPTH);
//  assert(MAX_NESTED_DEPTH == options.maxNestedDepth());
//..

#include <bdlscm_version.h>

#include <bsl_iosfwd.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdljsn {

                             // =================
                             // class ReadOptions
                             // =================

class ReadOptions {
    // This simply constrained (value-semantic) attribute class specifies
    // options for reading a JSON document.  See the {Attributes} section under
    // {DESCRIPTION} in the component-level documentation for information on
    // the class attributes.  Note that the class invariants are identically
    // the constraints on the individual attributes.

    // INSTANCE DATA
    bool d_allowTrailingText;
        // whether to permit text after a valid JSON document

    int d_maxNestedDepth;
        // maximum nesting level for JSON objects and arrays

  public:
    // CONSTANTS
    static const bool s_DEFAULT_INITIALIZER_ALLOW_TRAILING_TEXT;
    static const int  s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH;

  public:
    // CREATORS
    ReadOptions();
        // Create an object of type 'ReadOptions' having the (default)
        // attribute values:
        //..
        //  setAllowTrailingText() == false
        //  maxNestedDepth()       == 64
        //..

    ReadOptions(const ReadOptions& original);
        // Create an object of type 'ReadOptions' having the value of the
        // specified 'original' object.

    ~ReadOptions();
        // Destroy this object.

    // MANIPULATORS
    ReadOptions& operator=(const ReadOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a non-'const' reference to this object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    void setAllowTrailingText(bool value);
        // Set the 'allowTrailingText' attribute of this object to the
        // specified 'value'.

    void setMaxNestedDepth(int value);
        // Set the 'maxNestedDepth' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 < value'.

    // ACCESSORS
    bool allowTrailingText() const;
        // Return the 'allowTrailingText' attribute of this object.

    int maxNestedDepth() const;
        // Return the 'maxNestedDepth' attribute of this object.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.  Also note that the format is not fully
        // specified, and can change without notice.
};

// FREE OPERATORS
inline
bool operator==(const ReadOptions& lhs, const ReadOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const ReadOptions& lhs, const ReadOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes do not
    // have the same value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const ReadOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' in a single
    // line format and return a non-'const' reference to 'stream'.

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // class ReadOptions
                             // -----------------
// CREATORS
inline
ReadOptions::ReadOptions(const ReadOptions& original)
: d_allowTrailingText(original.d_allowTrailingText)
, d_maxNestedDepth   (original.d_maxNestedDepth)
{
}

inline
ReadOptions::~ReadOptions()
{
    BSLS_ASSERT(0 < d_maxNestedDepth);
}

// MANIPULATORS
inline
ReadOptions& ReadOptions::operator=(const ReadOptions& rhs)
{
    d_allowTrailingText = rhs.d_allowTrailingText;
    d_maxNestedDepth    = rhs.d_maxNestedDepth;

    return *this;
}

inline
void ReadOptions::setAllowTrailingText(bool value)
{
    d_allowTrailingText = value;
}

inline
void ReadOptions::setMaxNestedDepth(int value)
{
    BSLS_ASSERT(0 < value);

    d_maxNestedDepth = value;
}

// ACCESSORS
inline
bool ReadOptions::allowTrailingText() const
{
    return d_allowTrailingText;
}

inline
int ReadOptions::maxNestedDepth() const
{
    return d_maxNestedDepth;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdljsn::operator==(const bdljsn::ReadOptions& lhs,
                        const bdljsn::ReadOptions& rhs)
{
    return lhs.maxNestedDepth()    == rhs.maxNestedDepth()
        && lhs.allowTrailingText() == rhs.allowTrailingText();
}

inline
bool bdljsn::operator!=(const bdljsn::ReadOptions& lhs,
                        const bdljsn::ReadOptions& rhs)
{
    return lhs.maxNestedDepth()    != rhs.maxNestedDepth()
        || lhs.allowTrailingText() != rhs.allowTrailingText();
}

inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&              stream,
                                 const bdljsn::ReadOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

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
