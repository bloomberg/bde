// baljsn_decoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BALJSN_DECODEROPTIONS
#define INCLUDED_BALJSN_DECODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying JSON decoding options.
//
//@CLASSES:
//  baljsn::DecoderOptions: options for decoding objects in the JSON format
//
//@SEE_ALSO: baljsn_decoder, baljsn_encoderoptions
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baljsn::DecoderOptions', that is used to
// specify options for decoding objects in the JSON format.
//
///Attributes
///----------
//..
//  Name                  Type           Default         Simple Constraints
//  ------------------    -----------    -------         ------------------
//  maxDepth              int            32              >= 0
//  skipUnknownElements   bool           true            none
//..
//: o 'maxDepth': maximum depth of the decoded data
//:
//: o 'skipUnknownElements': flag specifying if unknown elements are skipped
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for decoding objects in the JSON format.  This example shows how to create
// and populate an options object.
//
// First, we default-construct a 'baljsn::DecoderOptions' object:
//..
//  const int  MAX_DEPTH             = 10;
//  const bool SKIP_UNKNOWN_ELEMENTS = false;
//
//  baljsn::DecoderOptions options;
//  assert(32   == options.maxDepth());
//  assert(true == options.skipUnknownElements());
//..
// Next, we populate that object to decode using a different 'maxDepth' value
// and 'skipUnknownElements' value:
//..
//  options.setMaxDepth(MAX_DEPTH);
//  assert(MAX_DEPTH == options.maxDepth());
//
//  options.setSkipUnknownElements(SKIP_UNKNOWN_ELEMENTS);
//  assert(SKIP_UNKNOWN_ELEMENTS == options.skipUnknownElements());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {
namespace baljsn {

                            // ====================
                            // class DecoderOptions
                            // ====================

class DecoderOptions {
    // This simply constrained (value-semantic) attribute class describes
    // options for decoding an object in the JSON format.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation.  Note
    // that the class invariants are identically the constraints on the
    // individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-safe*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // CONSTANTS
    enum {
        e_DEFAULT_MAX_DEPTH = 32  // default value for
                                       // 'maxDepth' attribute
    };

    // DATA
    int  d_maxDepth;             // maximum recursion depth
    bool d_skipUnknownElements;  // flag specifying if unknown elements should
                                 // be skipped

  public:
    // CREATORS
    DecoderOptions();
        // Create a 'DecoderOptions' object having the (default) attribute
        // values:
        //..
        //  maxDepth            == 0
        //  skipUnknownElements == true
        //..

    //! DecoderOptions(const DecoderOptions& original) = default;
        // Create a 'DecoderOptions' object having the same value as the
        // specified 'original' object.

    ~DecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    //! DecoderOptions& operator=(const DecoderOptions& rhs) =
    //!                                                                default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setMaxDepth(int value);
        // Set the 'maxDepth' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void setSkipUnknownElements(bool value);
        // Set the 'skipUnknownElements' attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    int maxDepth() const;
        // Return the value of the 'maxDepth' attribute of this object.

    bool skipUnknownElements() const;
        // Return the value of the 'skipUnknownElements' attribute of this
        // object.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const DecoderOptions& lhs, const DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderOptions' objects have the
    // same value if all of the corresponding values of their 'maxDepth' and
    // 'skipUnknownElements'attributes are the same.

bool operator!=(const DecoderOptions& lhs, const DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'EncoderOptions' objects do not
    // have the same value if any of the corresponding values of their
    // 'maxDepth' and 'skipUnknownElements' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const DecoderOptions& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)', but with
    // the attribute names elided.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// CREATORS
inline
DecoderOptions::DecoderOptions()
: d_maxDepth(e_DEFAULT_MAX_DEPTH)
, d_skipUnknownElements(true)
{
}

inline
DecoderOptions::~DecoderOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_maxDepth);
}

// MANIPULATORS
inline
void DecoderOptions::setMaxDepth(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_maxDepth = value;
}

inline
void DecoderOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

// ACCESSORS
inline
int DecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
bool DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}
}  // close package namespace

// FREE FUNCTIONS
inline
bool baljsn::operator==(const DecoderOptions& lhs, const DecoderOptions& rhs)
{
    return  lhs.maxDepth()            == rhs.maxDepth()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements();
}

inline
bool baljsn::operator!=(const DecoderOptions& lhs, const DecoderOptions& rhs)
{
    return  lhs.maxDepth()            != rhs.maxDepth()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
