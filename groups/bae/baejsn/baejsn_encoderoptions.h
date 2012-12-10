// baejsn_encoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BAEJSN_ENCODEROPTIONS
#define INCLUDED_BAEJSN_ENCODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide options for encoding objects in the JSON format
//
//@CLASSES:
//  baejsn_EncoderOptions: JSON encoding options
//
//@SEE_ALSO: baejsn_encoder, baejsn_decoderoptions
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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
    // Options for encoding objects in the JSON format.

  public:
    enum EncodingStyle {
        BAEJSN_COMPACT,
        BAEJSN_PRETTY
    };

  private:

    // DATA
    int           d_initialIndentLevel;  // initial indentation level
    int           d_spacesPerLevel;      // spaces per level of
                                         // indentation
    EncodingStyle d_encodingStyle;       // encoding style

  public:
    // TRAITS -- TBD ?
    // BDEX   -- TBD ?

    // CREATORS
    baejsn_EncoderOptions();
        // Create an object of type 'baejsn_EncoderOptions' having the
        // default value.

    baejsn_EncoderOptions(const baejsn_EncoderOptions& original);
        // Create an object of type 'baejsn_EncoderOptions' having the
        // value of the specified 'original' object.

    ~baejsn_EncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    baejsn_EncoderOptions& operator=(const baejsn_EncoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    void setInitialIndentLevel(int value);
        // Set the "InitialIndentLevel" attribute of this object to the
        // specified 'value'.

    void setSpacesPerLevel(int value);
        // Set the "SpacesPerLevel" attribute of this object to the specified
        // 'value'.

    void setEncodingStyle(EncodingStyle value);
        // Set the "EncodingStyle" attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
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
        // in multiline mode only.

    int initialIndentLevel() const;
        // Return a reference to the non-modifiable "InitialIndentLevel"
        // attribute of this object.

    int spacesPerLevel() const;
        // Return a reference to the non-modifiable "SpacesPerLevel" attribute
        // of this object.

    EncodingStyle encodingStyle() const;
        // Return a reference to the non-modifiable "EncodingStyle" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_EncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// TRAITS -- TBD

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------------
                      // class baejsn_EncoderOptions
                      // ---------------------------

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

// FREE FUNCTIONS
inline
bool operator==(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.spacesPerLevel()     == rhs.spacesPerLevel()
         && lhs.encodingStyle()      == rhs.encodingStyle();
}

inline
bool operator!=(const baejsn_EncoderOptions& lhs,
                const baejsn_EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() != rhs.initialIndentLevel()
         || lhs.spacesPerLevel()     != rhs.spacesPerLevel()
         || lhs.encodingStyle()      != rhs.encodingStyle();
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_EncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
