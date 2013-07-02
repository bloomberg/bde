// baejsn_encoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BAEJSN_ENCODEROPTIONS
#define INCLUDED_BAEJSN_ENCODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying JSON encoding options.
//
//@CLASSES:
//  baejsn_EncoderOptions: options for encoding objects in the JSON format
//
//@SEE_ALSO: baejsn_encoder, baejsn_decoderoptions
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baejsn_EncoderOptions', that is used to
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
///Example 1: Creating and populating an options object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for encoding objects in the JSON format.  This example shows how to create
// and populate an options object.
//
// First, we default-construct a 'baejsn_EncoderOptions' object:
//..
//  const int  INITIAL_INDENT_LEVEL = 1;
//  const int  SPACES_PER_LEVEL     = 4;
//  const bool ENCODE_EMPTY_ARRAYS  = true;
//  const bool ENCODE_NULL_ELEMENTS = true;
//
//  baejsn_EncoderOptions options;
//  assert(0 == options.initialIndentLevel());
//  assert(0 == options.spacesPerLevel());
//  assert(baejsn_EncoderOptions::BAEJSN_COMPACT == options.encodingStyle());
//  assert(false == options.encodeEmptyArrays());
//  assert(false == options.encodeNullElements());
//..
// Next, we populate that object to encode in a pretty format using a
// pre-defined initial indent level and spaces per level:
//..
//  options.setEncodingStyle(baejsn_EncoderOptions::BAEJSN_PRETTY);
//  assert(baejsn_EncoderOptions::BAEJSN_PRETTY == options.encodingStyle());
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {


                      // ===========================
                      // class baejsn_EncoderOptions
                      // ===========================

class baejsn_EncoderOptions {
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

        BAEJSN_COMPACT = 0,  // compact encoding without any whitespace
        BAEJSN_PRETTY        // pretty encoding with appropriate indentation
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
    baejsn_EncoderOptions();
        // Create a 'baejsn_EncoderOptions' object having the (default)
        // attribute values:
        //..
        //  encodingStyle      == BAEJSN_COMPACT
        //  initialIndentLevel == 0
        //  spacesPerLevel     == 0
        //  encodeEmptyArrays  == false
        //  encodeNullElements == false
        //..

    //! baejsn_EncoderOptions(const baejsn_EncoderOptions& original) = default;
        // Create a 'baejsn_EncoderOptions' object having the same value as the
        // specified 'original' object.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baejsn_EncoderOptions();
        // Destroy this object.
#endif

    // MANIPULATORS
    //! baejsn_EncoderOptions& operator=(const baejsn_EncoderOptions& rhs) =
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
bool operator==(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baejsn_EncoderOptions' objects have
    // the same value if all of the corresponding values of their
    // 'initialIndentLevel', 'spacesPerLevel', 'encodingStyle',
    // 'encodeEmptyArrays', and 'encodeNullElements' attributes are the same.

bool operator!=(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baejsn_EncoderOptions' objects
    // do not have the same value if any of the corresponding values of their
    // 'initialIndentLevel', 'spacesPerLevel', 'encodingStyle',
    // 'encodeEmptyArrays', and 'encodeNullElements' attributes are not the
    // same.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_EncoderOptions& object);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference
    // providing modifiable access to 'stream'.  If 'stream' is not valid on
    // entry, this operation has no effect.  Note that this human-readable
    // format is not fully specified and can change without notice.  Also note
    // that this method has the same behavior as 'object.print(stream, 0, -1)',
    // but with the attribute names elided.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------------
                      // class baejsn_EncoderOptions
                      // ---------------------------

// CREATORS
inline
baejsn_EncoderOptions::baejsn_EncoderOptions()
: d_initialIndentLevel(0)
, d_spacesPerLevel(0)
, d_encodingStyle(BAEJSN_COMPACT)
, d_encodeEmptyArrays(false)
, d_encodeNullElements(false)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baejsn_EncoderOptions::~baejsn_EncoderOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_initialIndentLevel);
    BSLS_ASSERT_SAFE(0 <= d_spacesPerLevel);
}
#endif

// MANIPULATORS
inline
void baejsn_EncoderOptions::setInitialIndentLevel(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_initialIndentLevel = value;
}

inline
void baejsn_EncoderOptions::setSpacesPerLevel(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_spacesPerLevel = value;
}

inline
void baejsn_EncoderOptions::setEncodingStyle(EncodingStyle value)
{
    d_encodingStyle = value;
}

inline
void baejsn_EncoderOptions::setEncodeEmptyArrays(bool value)
{
    d_encodeEmptyArrays = value;
}

inline
void baejsn_EncoderOptions::setEncodeNullElements(bool value)
{
    d_encodeNullElements = value;
}

// ACCESSORS
inline
int baejsn_EncoderOptions::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
int baejsn_EncoderOptions::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
baejsn_EncoderOptions::EncodingStyle
baejsn_EncoderOptions::encodingStyle() const
{
    return d_encodingStyle;
}

inline
bool baejsn_EncoderOptions::encodeEmptyArrays() const
{
    return d_encodeEmptyArrays;
}

inline
bool baejsn_EncoderOptions::encodeNullElements() const
{
    return d_encodeNullElements;
}

// FREE FUNCTIONS
inline
bool operator==(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.spacesPerLevel()     == rhs.spacesPerLevel()
         && lhs.encodingStyle()      == rhs.encodingStyle()
         && lhs.encodeEmptyArrays()  == rhs.encodeEmptyArrays()
         && lhs.encodeNullElements() == rhs.encodeNullElements();
}

inline
bool operator!=(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs)
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
