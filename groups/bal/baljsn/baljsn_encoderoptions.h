// baljsn_encoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BALJSN_ENCODEROPTIONS
#define INCLUDED_BALJSN_ENCODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying JSON encoding options.
//
//@CLASSES:
//  baljsn::EncoderOptions: options for encoding objects in the JSON format
//
//@SEE_ALSO: baljsn_encoder, baljsn_decoderoptions
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baljsn::EncoderOptions', that is used to
// specify options for encoding objects in the JSON format.
//
///Attributes
///----------
//..
//  Name                Type           Default         Simple Constraints
//  ------------------  -----------    -------         ------------------
//  encodingStyle       EncodingStyle  BAEJSN_COMPACT  none
//  initialIndentLevel  int            0               >= 0
//  spacesPerLevel      int            0               >= 0
//  encodeEmptyArrays   bool           false           none
//  encodeNullElements  bool           false           none
//..
//: o 'encodingStyle': encoding style used to encode the JSON data.
//:
//: o 'initialIndentLevel': Initial indent level for the topmost element.
//:
//: o 'spacesPerLevel': spaces per additional indent level.
//:
//: o 'encodeEmptyArrays': option specifying if empty arrays should be encoded.
//:
//: o 'encodeNullElements': option specifying if null elements should be
//:                         encoded.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for encoding objects in the JSON format.  This example shows how to create
// and populate an options object.
//
// First, we default-construct a 'baljsn::EncoderOptions' object:
//..
//  const int  INITIAL_INDENT_LEVEL = 1;
//  const int  SPACES_PER_LEVEL     = 4;
//  const bool ENCODE_EMPTY_ARRAYS  = true;
//  const bool ENCODE_NULL_ELEMENTS = true;
//
//  baljsn::EncoderOptions options;
//  assert(0 == options.initialIndentLevel());
//  assert(0 == options.spacesPerLevel());
//  assert(baljsn::EncoderOptions::e_COMPACT == options.encodingStyle());
//  assert(false == options.encodeEmptyArrays());
//  assert(false == options.encodeNullElements());
//..
// Next, we populate that object to encode in a pretty format using a
// pre-defined initial indent level and spaces per level:
//..
//  options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
//  assert(baljsn::EncoderOptions::e_PRETTY == options.encodingStyle());
//
//  options.setInitialIndentLevel(INITIAL_INDENT_LEVEL);
//  assert(INITIAL_INDENT_LEVEL == options.initialIndentLevel());
//
//  options.setSpacesPerLevel(SPACES_PER_LEVEL);
//  assert(SPACES_PER_LEVEL == options.spacesPerLevel());
//
//  options.setEncodeEmptyArrays(ENCODE_EMPTY_ARRAYS);
//  assert(ENCODE_EMPTY_ARRAYS == options.encodeEmptyArrays());
//
//  options.setEncodeNullElements(ENCODE_NULL_ELEMENTS);
//  assert(ENCODE_NULL_ELEMENTS == options.encodeNullElements());
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
                            // class EncoderOptions
                            // ====================

class EncoderOptions {
    // This simply constrained (value-semantic) attribute class describes
    // options for encoding an object in the JSON format.  See the Attributes
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

  public:
    // TYPES
    enum EncodingStyle {
        // This 'enum' provides enumerators to specify the encoding styles.

        e_COMPACT = 0,  // compact encoding without any whitespace
        e_PRETTY        // pretty encoding with appropriate indentation
    };

  private:
    // DATA
    int           d_initialIndentLevel;  // initial indentation level

    int           d_spacesPerLevel;      // spaces per level per indent level

    EncodingStyle d_encodingStyle;       // encoding style

    bool          d_encodeEmptyArrays;   // flag specifying if empty arrays
                                         // should be encoded

    bool          d_encodeNullElements;  // flag specifying if null elements
                                         // should be encoded

  public:
    // CREATORS
    EncoderOptions();
        // Create a 'EncoderOptions' object having the (default) attribute
        // values:
        //..
        //  encodingStyle      == BAEJSN_COMPACT
        //  initialIndentLevel == 0
        //  spacesPerLevel     == 0
        //  encodeEmptyArrays  == false
        //  encodeNullElements == false
        //..

    //! EncoderOptions(const EncoderOptions& original) = default;
        // Create a 'EncoderOptions' object having the same value as the
        // specified 'original' object.

    ~EncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    //! EncoderOptions& operator=(const EncoderOptions& rhs) =
    //!                                                                default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setInitialIndentLevel(int value);
        // Set the 'initialIndentLevel' attribute of this object to the
        // specified 'value'.  The behavior is undefined unless '0 <= value'.

    void setSpacesPerLevel(int value);
        // Set the 'spacesPerLevel' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void setEncodingStyle(EncodingStyle value);
        // Set the 'encodingStyle' attribute of this object to the specified
        // 'value'.

    void setEncodeEmptyArrays(bool value);
        // Set the 'encodeEmptyArrays' attribute of this object to the
        // specified 'value'.

    void setEncodeNullElements(bool value);
        // Set the 'encodeNullElements' attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    int initialIndentLevel() const;
        // Return the value of the 'initialIndentLevel' attribute of this
        // object.

    int spacesPerLevel() const;
        // Return the value of the 'spacesPerLevel' attribute of this object.

    EncodingStyle encodingStyle() const;
        // Return the value of the 'encodingStyle' attribute of this object.

    bool encodeEmptyArrays() const;
        // Return the value of the 'encodeEmptyArrays' attribute of this
        // object.

    bool encodeNullElements() const;
        // Return the value of the 'encodeNullElements' attribute of this
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
bool operator==(const EncoderOptions& lhs, const EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderOptions' objects have the
    // same value if all of the corresponding values of their
    // 'initialIndentLevel', 'spacesPerLevel', 'encodingStyle',
    // 'encodeEmptyArrays', and 'encodeNullElements' attributes are the same.

bool operator!=(const EncoderOptions& lhs, const EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'EncoderOptions' objects do not
    // have the same value if any of the corresponding values of their
    // 'initialIndentLevel', 'spacesPerLevel', 'encodingStyle',
    // 'encodeEmptyArrays', and 'encodeNullElements' attributes are not the
    // same.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const EncoderOptions& object);
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
                            // class EncoderOptions
                            // --------------------

// CREATORS
inline
EncoderOptions::EncoderOptions()
: d_initialIndentLevel(0)
, d_spacesPerLevel(0)
, d_encodingStyle(e_COMPACT)
, d_encodeEmptyArrays(false)
, d_encodeNullElements(false)
{
}

inline
EncoderOptions::~EncoderOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_initialIndentLevel);
    BSLS_ASSERT_SAFE(0 <= d_spacesPerLevel);
}

// MANIPULATORS
inline
void EncoderOptions::setInitialIndentLevel(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_initialIndentLevel = value;
}

inline
void EncoderOptions::setSpacesPerLevel(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_spacesPerLevel = value;
}

inline
void EncoderOptions::setEncodingStyle(EncodingStyle value)
{
    d_encodingStyle = value;
}

inline
void EncoderOptions::setEncodeEmptyArrays(bool value)
{
    d_encodeEmptyArrays = value;
}

inline
void EncoderOptions::setEncodeNullElements(bool value)
{
    d_encodeNullElements = value;
}

// ACCESSORS
inline
int EncoderOptions::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
int EncoderOptions::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
EncoderOptions::EncodingStyle
EncoderOptions::encodingStyle() const
{
    return d_encodingStyle;
}

inline
bool EncoderOptions::encodeEmptyArrays() const
{
    return d_encodeEmptyArrays;
}

inline
bool EncoderOptions::encodeNullElements() const
{
    return d_encodeNullElements;
}
}  // close package namespace

// FREE FUNCTIONS
inline
bool baljsn::operator==(const EncoderOptions& lhs, const EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.spacesPerLevel()     == rhs.spacesPerLevel()
         && lhs.encodingStyle()      == rhs.encodingStyle()
         && lhs.encodeEmptyArrays()  == rhs.encodeEmptyArrays()
         && lhs.encodeNullElements() == rhs.encodeNullElements();
}

inline
bool baljsn::operator!=(const EncoderOptions& lhs, const EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() != rhs.initialIndentLevel()
         || lhs.spacesPerLevel()     != rhs.spacesPerLevel()
         || lhs.encodingStyle()      != rhs.encodingStyle()
         || lhs.encodeEmptyArrays()  != rhs.encodeEmptyArrays()
         || lhs.encodeNullElements() != rhs.encodeNullElements();
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
