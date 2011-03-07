// baexml_decoderoptions.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAEXML_DECODEROPTIONS
#define INCLUDED_BAEXML_DECODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baexml_decoderoptions_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION:
// Options for controlling the XML decoding process.
//

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
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
                            // class baexml_DecoderOptions
                            // ===========================                     

class baexml_DecoderOptions {
    // Options for controlling the XML decoding process. 

    // INSTANCE DATA
    int   d_maxDepth;
        // maximum recursion depth
    int   d_formattingMode;
        // Formatting mode
    bool  d_skipUnknownElements;
        // Option to skip unknown elements 

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MAX_DEPTH             = 0
      , ATTRIBUTE_ID_FORMATTING_MODE       = 1
      , ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_MAX_DEPTH             = 0
      , ATTRIBUTE_INDEX_FORMATTING_MODE       = 1
      , ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    static const int DEFAULT_INITIALIZER_FORMATTING_MODE;

    static const bool DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    baexml_DecoderOptions();
        // Create an object of type 'baexml_DecoderOptions' having the default value.

    baexml_DecoderOptions(const baexml_DecoderOptions& original);
        // Create an object of type 'baexml_DecoderOptions' having the value of the
        // specified 'original' object.

    ~baexml_DecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    baexml_DecoderOptions& operator=(const baexml_DecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    void setMaxDepth(int value);
        // Set the "MaxDepth" attribute of this object to the specified
        // 'value'.

    void setFormattingMode(int value);
        // Set the "FormattingMode" attribute of this object to the specified
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const int& maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.

    const int& formattingMode() const;
        // Return a reference to the non-modifiable "FormattingMode" attribute
        // of this object.

    const bool& skipUnknownElements() const;
        // Return a reference to the non-modifiable "SkipUnknownElements"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const baexml_DecoderOptions& lhs, const baexml_DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baexml_DecoderOptions& lhs, const baexml_DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baexml_DecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baexml_DecoderOptions)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                            // ---------------------------
                            // class baexml_DecoderOptions
                            // ---------------------------

// CLASS METHODS
inline
int baexml_DecoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baexml_DecoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_maxDepth, 1);
            bdex_InStreamFunctions::streamIn(stream, d_formattingMode, 1);
            bdex_InStreamFunctions::streamIn(stream, d_skipUnknownElements, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baexml_DecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baexml_DecoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      } break;
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baexml_DecoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void baexml_DecoderOptions::setMaxDepth(int value)
{
    d_maxDepth = value;
}

inline
void baexml_DecoderOptions::setFormattingMode(int value)
{
    d_formattingMode = value;
}

inline
void baexml_DecoderOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

// ACCESSORS
template <class STREAM>
STREAM& baexml_DecoderOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_maxDepth, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_formattingMode, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_skipUnknownElements, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baexml_DecoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baexml_DecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      } break;
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baexml_DecoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& baexml_DecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
const int& baexml_DecoderOptions::formattingMode() const
{
    return d_formattingMode;
}

inline
const bool& baexml_DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baexml_DecoderOptions& lhs,
        const baexml_DecoderOptions& rhs)
{
    return  lhs.maxDepth() == rhs.maxDepth()
         && lhs.formattingMode() == rhs.formattingMode()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements();
}

inline
bool operator!=(
        const baexml_DecoderOptions& lhs,
        const baexml_DecoderOptions& rhs)
{
    return  lhs.maxDepth() != rhs.maxDepth()
         || lhs.formattingMode() != rhs.formattingMode()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baexml_DecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.6.1 Mon Jan  3 12:39:26 2011
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
