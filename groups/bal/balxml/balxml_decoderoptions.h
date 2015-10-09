// balxml_decoderoptions.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_DECODEROPTIONS
#define INCLUDED_BALXML_DECODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(baexml_decoderoptions_h,"$Id$ $CSID$ $CCId$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@DESCRIPTION:
// Options for controlling the XML decoding process.
//

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_SELECTIONINFO
#include <bdlat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {

namespace balxml {

                            // ====================
                            // class DecoderOptions
                            // ====================

class DecoderOptions {
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
        e_ATTRIBUTE_ID_MAX_DEPTH             = 0
      , e_ATTRIBUTE_ID_FORMATTING_MODE       = 1
      , e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS = 2
    };

    enum {
        k_NUM_ATTRIBUTES = 3
    };

    enum {
        e_ATTRIBUTE_INDEX_MAX_DEPTH             = 0
      , e_ATTRIBUTE_INDEX_FORMATTING_MODE       = 1
      , e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    static const int DEFAULT_INITIALIZER_FORMATTING_MODE;

    static const bool DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    DecoderOptions();
        // Create an object of type 'DecoderOptions' having the default value.

    DecoderOptions(const DecoderOptions& original);
        // Create an object of type 'DecoderOptions' having the value of the
        // specified 'original' object.

    ~DecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    DecoderOptions& operator=(const DecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
bool operator==(const DecoderOptions& lhs, const DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const DecoderOptions& lhs, const DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const DecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balxml::DecoderOptions)

namespace balxml {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================


                            // --------------------
                            // class DecoderOptions
                            // --------------------

// MANIPULATORS
template <class MANIPULATOR>
int DecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int DecoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_FORMATTING_MODE: {
        return manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int DecoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void DecoderOptions::setMaxDepth(int value)
{
    d_maxDepth = value;
}

inline
void DecoderOptions::setFormattingMode(int value)
{
    d_formattingMode = value;
}

inline
void DecoderOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

// ACCESSORS
template <class ACCESSOR>
int DecoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int DecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_FORMATTING_MODE: {
        return accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int DecoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& DecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
const int& DecoderOptions::formattingMode() const
{
    return d_formattingMode;
}

inline
const bool& DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}
}  // close package namespace


// FREE FUNCTIONS

inline
bool balxml::operator==(
        const DecoderOptions& lhs,
        const DecoderOptions& rhs)
{
    return  lhs.maxDepth() == rhs.maxDepth()
         && lhs.formattingMode() == rhs.formattingMode()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements();
}

inline
bool balxml::operator!=(
        const DecoderOptions& lhs,
        const DecoderOptions& rhs)
{
    return  lhs.maxDepth() != rhs.maxDepth()
         || lhs.formattingMode() != rhs.formattingMode()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements();
}

inline
bsl::ostream& balxml::operator<<(
        bsl::ostream& stream,
        const DecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
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
