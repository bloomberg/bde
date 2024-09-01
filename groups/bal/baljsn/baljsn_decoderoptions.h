// baljsn_decoderoptions.h                                            -*-C++-*-
#ifndef INCLUDED_BALJSN_DECODEROPTIONS
#define INCLUDED_BALJSN_DECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying JSON decoding options.
//
//@CLASSES:
//  baljsn::DecoderOptions: options for decoding objects in the JSON format
//
//@SEE_ALSO: baljsn_decoder, baljsn_encoderoptions
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `baljsn::DecoderOptions`, that is used to
// specify options for decoding objects in the JSON format.
//
///Attributes
///----------
// ```
// Name                             Type  Default  Simple Constraints
// ------------------               ----  -------  ------------------
// maxDepth                         int   32       >= 0
// skipUnknownElements              bool  true     none
// validateInputIsUtf8              bool  false    none
// allowConsecutiveSeparators       bool  true     none
// allowFormFeedAsWhitespace        bool  true     none
// allowUnescapedControlCharacters  bool  true     none
// ```
// * `maxDepth`: maximum depth of the decoded data
// * `skipUnknownElements`: flag specifying if unknown elements are skipped
// * `validateInputIsUtf8`: flag specifying whether UTF-8 correctness checking
//   is enabled.
// * `allowConsecutiveSeparators`: flag specifying if multiple consecutive
//   separators -- e.g., `"a" :: 1`, `[ 1,, 2 ]` -- are accepted and treated
//   as if one separator had been input.
// * `allowFormFeedAsWhitespace`: flag specifying if the form-feed character,
//   '\f', is treaded as whitespace in addition to ` `, '\t', '\n', '\r', and
//   '\v'.
//
// * `allowUnescapedControlCharacters`: flag specifying if unescaped (raw)
//   control characters (e.g., '\n', '\n') are allowed in JSON strings.
//   Otherwise, control characters are represented by multi-character
//   sequences (e.g., '\\t' or '\u000A').
//
///Implementation Note
///- - - - - - - - - -
// This file was generated from a script and was subsequently modified to add
// documentation and to make other changes.  The steps to generate and update
// this file can be found in the `doc/generating_codec_options.txt` file.
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
// First, we default-construct a `baljsn::DecoderOptions` object:
// ```
// const int  MAX_DEPTH             = 10;
// const bool SKIP_UNKNOWN_ELEMENTS = false;
//
// baljsn::DecoderOptions options;
// assert(32   == options.maxDepth());
// assert(true == options.skipUnknownElements());
// ```
// Next, we populate that object to decode using a different `maxDepth` value
// and `skipUnknownElements` value:
// ```
// options.setMaxDepth(MAX_DEPTH);
// assert(MAX_DEPTH == options.maxDepth());
//
// options.setSkipUnknownElements(SKIP_UNKNOWN_ELEMENTS);
// assert(SKIP_UNKNOWN_ELEMENTS == options.skipUnknownElements());
// ```

#include <balscm_version.h>

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>

#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace baljsn { class DecoderOptions; }
namespace baljsn {

                            // ====================
                            // class DecoderOptions
                            // ====================

/// Options for controlling the JSON decoding process.
class DecoderOptions {

    // INSTANCE DATA

    // maximum recursion depth
    int   d_maxDepth;

    // option to skip unknown elements
    bool  d_skipUnknownElements;

    // option to check that input is valid UTF-8
    bool  d_validateInputIsUtf8;

    // Option to allow multiple consecutive colons or commas.  Set to
    // `false` for strictly conformining JSON.
    bool  d_allowConsecutiveSeparators;

    // Option to allow '\f' (form feed) as whitespace in addition to ` `,
    // '\n', '\t', '\r', and '\v'.  Set to `false` for strictly
    // conformining JSON.
    bool  d_allowFormFeedAsWhitespace;

    // Option to allow characters in the range `[0x00 ..  0x1F]`  (e.g.,
    // '\0', '\t', '\n') in JSON strings.  Set to `false` for strictly
    // conformining JSON.
    bool  d_allowUnescapedControlCharacters;

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const DecoderOptions& rhs) const;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MAX_DEPTH                          = 0
      , ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS              = 1
      , ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8             = 2
      , ATTRIBUTE_ID_ALLOW_CONSECUTIVE_SEPARATORS       = 3
      , ATTRIBUTE_ID_ALLOW_FORM_FEED_AS_WHITESPACE      = 4
      , ATTRIBUTE_ID_ALLOW_UNESCAPED_CONTROL_CHARACTERS = 5
    };

    enum {
        NUM_ATTRIBUTES = 6
    };

    enum {
        ATTRIBUTE_INDEX_MAX_DEPTH                          = 0
      , ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS              = 1
      , ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8             = 2
      , ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS       = 3
      , ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE      = 4
      , ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    static const bool DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;

    static const bool DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8;

    static const bool DEFAULT_INITIALIZER_ALLOW_CONSECUTIVE_SEPARATORS;

    static const bool DEFAULT_INITIALIZER_ALLOW_FORM_FEED_AS_WHITESPACE;

    static const bool DEFAULT_INITIALIZER_ALLOW_UNESCAPED_CONTROL_CHARACTERS;

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `DecoderOptions` having the default value.
    DecoderOptions();

    /// Create an object of type `DecoderOptions` having the value of the
    /// specified `original` object.
    DecoderOptions(const DecoderOptions& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `DecoderOptions` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    DecoderOptions(DecoderOptions&& original) = default;
#endif

    /// Destroy this object.
    ~DecoderOptions();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    DecoderOptions& operator=(const DecoderOptions& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    DecoderOptions& operator=(DecoderOptions&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Set the "MaxDepth" attribute of this object to the specified
    /// `value`.
    void setMaxDepth(int value);

    /// Set the "SkipUnknownElements" attribute of this object to the
    /// specified `value`.
    void setSkipUnknownElements(bool value);

    /// Set the "ValidateInputIsUtf8" attribute of this object to the
    /// specified `value`.
    void setValidateInputIsUtf8(bool value);

    /// Set the "AllowConsecutiveSeparators" attribute of this object to the
    /// specified `value`.
    void setAllowConsecutiveSeparators(bool value);

    /// Set the "AllowFormFeedAsWhitespace" attribute of this object to the
    /// specified `value`.
    void setAllowFormFeedAsWhitespace(bool value);

    /// Set the "AllowUnescapedControlCharacters" attribute of this object
    /// to the specified `value`.
    void setAllowUnescapedControlCharacters(bool value);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return the value of the "MaxDepth" attribute of this object.
    int maxDepth() const;

    /// Return the value of the "SkipUnknownElements" attribute of this
    /// object.
    bool skipUnknownElements() const;

    /// Return the value of the "ValidateInputIsUtf8" attribute of this
    /// object.
    bool validateInputIsUtf8() const;

    /// Return the value of the "AllowConsecutiveSeparators" attribute of
    /// this object.
    bool allowConsecutiveSeparators() const;

    /// Return the value of the "AllowFormFeedAsWhitespace" attribute of
    /// this object.
    bool allowFormFeedAsWhitespace() const;

    /// Return the value of the "AllowUnescapedControlCharacters" attribute
    /// of this object.
    bool allowUnescapedControlCharacters() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const DecoderOptions& lhs,
                           const DecoderOptions& rhs)
    {
        return lhs.isEqualTo(rhs);
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const DecoderOptions& lhs,
                           const DecoderOptions& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&         stream,
                                    const DecoderOptions& rhs)
    {
        return rhs.print(stream, 0, -1);
    }

    /// Pass the specified `object` to the specified `hashAlg`.  This
    /// function integrates with the `bslh` modular hashing system and
    /// effectively provides a `bsl::hash` specialization for
    /// `DecoderOptions`.
    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&     hashAlg,
                           const DecoderOptions& object)
    {
        object.hashAppendImpl(hashAlg);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::DecoderOptions)

//=============================================================================
//                          INLINE DEFINITIONS
//=============================================================================

namespace baljsn {

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void DecoderOptions::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->maxDepth());
    hashAppend(hashAlgorithm, this->skipUnknownElements());
    hashAppend(hashAlgorithm, this->validateInputIsUtf8());
    hashAppend(hashAlgorithm, this->allowConsecutiveSeparators());
    hashAppend(hashAlgorithm, this->allowFormFeedAsWhitespace());
    hashAppend(hashAlgorithm, this->allowUnescapedControlCharacters());
}

inline
bool DecoderOptions::isEqualTo(const DecoderOptions& rhs) const
{
    return this->maxDepth() == rhs.maxDepth() &&
           this->skipUnknownElements() == rhs.skipUnknownElements() &&
           this->validateInputIsUtf8() == rhs.validateInputIsUtf8() &&
           this->allowConsecutiveSeparators() == rhs.allowConsecutiveSeparators() &&
           this->allowFormFeedAsWhitespace() == rhs.allowFormFeedAsWhitespace() &&
           this->allowUnescapedControlCharacters() == rhs.allowUnescapedControlCharacters();
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int DecoderOptions::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
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

    ret = manipulator(&d_allowConsecutiveSeparators, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allowFormFeedAsWhitespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allowUnescapedControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int DecoderOptions::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      }
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      }
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8: {
        return manipulator(&d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
      }
      case ATTRIBUTE_ID_ALLOW_CONSECUTIVE_SEPARATORS: {
        return manipulator(&d_allowConsecutiveSeparators, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS]);
      }
      case ATTRIBUTE_ID_ALLOW_FORM_FEED_AS_WHITESPACE: {
        return manipulator(&d_allowFormFeedAsWhitespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE]);
      }
      case ATTRIBUTE_ID_ALLOW_UNESCAPED_CONTROL_CHARACTERS: {
        return manipulator(&d_allowUnescapedControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int DecoderOptions::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
    BSLS_ASSERT(0 <= value);

    d_maxDepth = value;
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

inline
void DecoderOptions::setAllowConsecutiveSeparators(bool value)
{
    d_allowConsecutiveSeparators = value;
}

inline
void DecoderOptions::setAllowFormFeedAsWhitespace(bool value)
{
    d_allowFormFeedAsWhitespace = value;
}

inline
void DecoderOptions::setAllowUnescapedControlCharacters(bool value)
{
    d_allowUnescapedControlCharacters = value;
}

// ACCESSORS
template <typename t_ACCESSOR>
int DecoderOptions::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
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

    ret = accessor(d_allowConsecutiveSeparators, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allowFormFeedAsWhitespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allowUnescapedControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int DecoderOptions::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      }
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      }
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8: {
        return accessor(d_validateInputIsUtf8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8]);
      }
      case ATTRIBUTE_ID_ALLOW_CONSECUTIVE_SEPARATORS: {
        return accessor(d_allowConsecutiveSeparators, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS]);
      }
      case ATTRIBUTE_ID_ALLOW_FORM_FEED_AS_WHITESPACE: {
        return accessor(d_allowFormFeedAsWhitespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE]);
      }
      case ATTRIBUTE_ID_ALLOW_UNESCAPED_CONTROL_CHARACTERS: {
        return accessor(d_allowUnescapedControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int DecoderOptions::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
bool DecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

inline
bool DecoderOptions::validateInputIsUtf8() const
{
    return d_validateInputIsUtf8;
}

inline
bool DecoderOptions::allowConsecutiveSeparators() const
{
    return d_allowConsecutiveSeparators;
}

inline
bool DecoderOptions::allowFormFeedAsWhitespace() const
{
    return d_allowFormFeedAsWhitespace;
}

inline
bool DecoderOptions::allowUnescapedControlCharacters() const
{
    return d_allowUnescapedControlCharacters;
}

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2024.03.02
// USING bas_codegen.pl -m msg -p baljsn -E --noExternalization --noAggregateConversion ../baljsn.xsd
// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
