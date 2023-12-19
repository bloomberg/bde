// balxml_decoderoptions.h          *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_BALXML_DECODEROPTIONS
#define INCLUDED_BALXML_DECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_decoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace balxml { class DecoderOptions; }
namespace balxml {

                            // ====================
                            // class DecoderOptions
                            // ====================

class DecoderOptions {
    // Options for controlling the XML decoding process.
    // This struct is generated using bas_codegen.pl called by
    // balxml/code_from_xsd.pl

    // INSTANCE DATA
    int   d_maxDepth;
        // maximum recursion depth
    int   d_formattingMode;
        // Formatting mode
    bool  d_skipUnknownElements;
        // Option to skip unknown elements
    bool  d_validateInputIsUtf8;
        // option to check that input is valid UTF-8

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MAX_DEPTH              = 0
      , ATTRIBUTE_ID_FORMATTING_MODE        = 1
      , ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS  = 2
      , ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8 = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_MAX_DEPTH              = 0
      , ATTRIBUTE_INDEX_FORMATTING_MODE        = 1
      , ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS  = 2
      , ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    static const int DEFAULT_INITIALIZER_FORMATTING_MODE;

    static const bool DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;

    static const bool DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8;

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    DecoderOptions(DecoderOptions&& original) = default;
        // Create an object of type 'DecoderOptions' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~DecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    DecoderOptions& operator=(const DecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    DecoderOptions& operator=(DecoderOptions&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    void setValidateInputIsUtf8(bool value);
        // Set the "ValidateInputIsUtf8" attribute of this object to the
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

    int maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.

    int formattingMode() const;
        // Return a reference to the non-modifiable "FormattingMode" attribute
        // of this object.

    bool skipUnknownElements() const;
        // Return a reference to the non-modifiable "SkipUnknownElements"
        // attribute of this object.

    bool validateInputIsUtf8() const;
        // Return a reference to the non-modifiable "ValidateInputIsUtf8"
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

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balxml::DecoderOptions)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace balxml {

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int DecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
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

    ret = manipulator(&d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int DecoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      }
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      }
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      }
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8: {
        return manipulator(&d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int DecoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
                                         lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
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

inline
void DecoderOptions::setValidateInputIsUtf8(bool value)
{
    d_validateInputIsUtf8 = value;
}

// ACCESSORS
template <class ACCESSOR>
int DecoderOptions::accessAttributes(ACCESSOR& accessor) const
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

    ret = accessor(d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int DecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      }
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      }
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      }
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8: {
        return accessor(d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int DecoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
int DecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
int DecoderOptions::formattingMode() const
{
    return d_formattingMode;
}

inline
bool DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

inline
bool DecoderOptions::validateInputIsUtf8() const
{
    return d_validateInputIsUtf8;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool balxml::operator==(
        const balxml::DecoderOptions& lhs,
        const balxml::DecoderOptions& rhs)
{
    return  lhs.maxDepth() == rhs.maxDepth()
         && lhs.formattingMode() == rhs.formattingMode()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements()
         && lhs.validateInputIsUtf8() == rhs.validateInputIsUtf8();
}

inline
bool balxml::operator!=(
        const balxml::DecoderOptions& lhs,
        const balxml::DecoderOptions& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balxml::operator<<(
        bsl::ostream& stream,
        const balxml::DecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// BAS_CODEGEN RUN BY code_from_xsd.pl RUN ON Thu Sep 24 20:40:02 EDT 2020
// GENERATED BY BLP_BAS_CODEGEN_2020.09.14
// USING bas_codegen.pl -m msg -p balxml -E --noExternalization --noAggregateConversion --noHashSupport balxml.xsd

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
