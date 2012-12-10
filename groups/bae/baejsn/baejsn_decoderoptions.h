// baejsn_decoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BAEJSN_DECODEROPTIONS
#define INCLUDED_BAEJSN_DECODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide options for decoding objects in the JSON format
//
//@CLASSES:
//  baejsn_DecoderOptions: JSON decoding options
//
//@SEE_ALSO: baejsn_decoder, baejsn_encoderoptions
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
                      // class baejsn_DecoderOptions
                      // ===========================

class baejsn_DecoderOptions {
    // Options for decoding objects in the JSON format.

    // TYPES
    enum {
        MAX_DEPTH = 32
    };

  public:
    // TYPES
    enum EncodingStyle {
        BAEJSN_COMPACT,
        BAEJSN_PRETTY
    };

  private:

    // DATA
    int  d_maxDepth;             // maximum recursion depth
    bool d_skipUnknownElements;  // flag specifying if unknown elements
                                 // should be skipped

  public:
    // TRAITS -- TBD ?
    // BDEX   -- TBD ?

    // CREATORS
    baejsn_DecoderOptions();
        // Create an object of type 'baejsn_DecoderOptions' having the
        // default value.

    baejsn_DecoderOptions(const baejsn_DecoderOptions& original);
        // Create an object of type 'baejsn_DecoderOptions' having the
        // value of the specified 'original' object.

    ~baejsn_DecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    baejsn_DecoderOptions& operator=(const baejsn_DecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    void setMaxDepth(int value);
        // Set the "MaxDepth" attribute of this object to the specified
        // 'value'.

    void setSkipUnknownElements(bool value);
        // Set the "SkipUnknownElements" attribute of this object to the
        // specified 'value'.

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

    int maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.

    bool skipUnknownElements() const;
        // Return a reference to the non-modifiable "SkipUnknownElements"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const baejsn_DecoderOptions& lhs,
                const baejsn_DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baejsn_DecoderOptions& lhs,
                const baejsn_DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_DecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// TRAITS -- TBD

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------------
                      // class baejsn_DecoderOptions
                      // ---------------------------

// MANIPULATORS
inline
void baejsn_DecoderOptions::setMaxDepth(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_maxDepth = value;
}

inline
void baejsn_DecoderOptions::setSkipUnknownElements(bool value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_skipUnknownElements = value;
}

// ACCESSORS
inline
int baejsn_DecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
bool baejsn_DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

// FREE FUNCTIONS
inline
bool operator==(const baejsn_DecoderOptions& lhs,
                const baejsn_DecoderOptions& rhs)
{
    return  lhs.maxDepth()            == rhs.maxDepth()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements();
}

inline
bool operator!=(const baejsn_DecoderOptions& lhs,
                const baejsn_DecoderOptions& rhs)
{
    return  lhs.maxDepth()            != rhs.maxDepth()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements();
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_DecoderOptions& rhs)
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
