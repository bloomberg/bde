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
//  encodingStyle       EncodingStyle  e_COMPACT       none
//  initialIndentLevel  int            0               >= 0
//  spacesPerLevel      int            0               >= 0
//  encodeEmptyArrays   bool           false           none
//  encodeNullElements  bool           false           none
//  encodeInfAndNaNAsStrings
//                      bool           false           none
//  datetimeFractionalSecondPrecision
//                      int            3               >= 0 and <= 6
//  maxFloatPrecision   int            bsl::numeric_limits<float>::digits10
//                                                     >= 1 and <= 9
//  maxDoublePrecision  int            bsl::numeric_limits<double>::digits10
//                                                     >= 1 and <= 17
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
//:
//: o 'encodeInfAndNaNAsStrings': JSON does not provide a way to encode these
//:                               values as they are not numbers.  This option
//:                               provides a way to encode these values.
//:                               Although the resulting output is a valid
//:                               JSON document, decoders expecting floating
//:                               point numbers to be encoded only as numbers
//:                               will fail to decode.  Users of this option
//:                               must therefore exercise caution and ensure
//:                               that if this option is used then the parser
//:                               decoding the generated JSON can handle
//:                               doubles as strings.
//:
//: o 'datetimeFractionalSecondPrecision': option specifying the number of
//:                                        decimal places used for seconds when
//:                                        encoding 'Datetime' and
//:                                        'DatetimeTz'.
//:
//: o 'maxFloatPrecision': option specifying the maximum number of decimal
//:                        places used to encode each 'float' value.
//:
//: o 'maxDoublePrecision': option specifying the maximum number of decimal
//:                         places used to encode each 'double' value.
//
///Implementation Note
///- - - - - - - - - -
// This file was generated from a script and was subsequently modified to add
// documentation and to make other changes.  The steps to generate and update
// this file can be found in the 'doc/generating_codec_options.txt' file.
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
//  const int  INITIAL_INDENT_LEVEL      = 1;
//  const int  SPACES_PER_LEVEL          = 4;
//  const bool ENCODE_EMPTY_ARRAYS       = true;
//  const bool ENCODE_NULL_ELEMENTS      = true;
//  const bool ENCODE_INF_NAN_AS_STRINGS = true;
//  const int  DATETIME_PRECISION        = 6;
//  const int  FLOAT_PRECISION           = 3;
//  const int  DOUBLE_PRECISION          = 9;
//
//  baljsn::EncoderOptions options;
//  assert(0     == options.initialIndentLevel());
//  assert(0     == options.spacesPerLevel());
//  assert(baljsn::EncoderOptions::e_COMPACT == options.encodingStyle());
//  assert(false == options.encodeEmptyArrays());
//  assert(false == options.encodeNullElements());
//  assert(false == options.encodeInfAndNaNAsStrings());
//  assert(3     == options.datetimeFractionalSecondPrecision());
//  assert(bsl::numeric_limits<float>::digits10
//                                             == options.maxFloatPrecision());
//  assert(bsl::numeric_limits<double>::digits10
//                                            ==
//                                            options.maxDoublePrecision());
//..
// Next, we populate that object to encode in a pretty format using a
// pre-defined initial indent level and spaces per level:
//..
//  options.setEncodingStyle(baljsn::EncodingStyle::e_PRETTY);
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
//
//  options.setEncodeInfAndNaNAsStrings(ENCODE_INF_NAN_AS_STRINGS);
//  assert(ENCODE_INF_NAN_AS_STRINGS == options.encodeInfAndNaNAsStrings());
//
//  options.setDatetimeFractionalSecondPrecision(DATETIME_PRECISION);
//  assert(DATETIME_PRECISION == options.datetimeFractionalSecondPrecision());
//
//  options.setMaxFloatPrecision(FLOAT_PRECISION);
//  assert(FLOAT_PRECISION == options.maxFloatPrecision());
//
//  options.setMaxDoublePrecision(DOUBLE_PRECISION);
//  assert(DOUBLE_PRECISION == options.maxDoublePrecision());
//..

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

#ifndef INCLUDED_BALJSN_ENCODINGSTYLE
#include <baljsn_encodingstyle.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace baljsn { class EncoderOptions; }
namespace baljsn {

                            // ====================
                            // class EncoderOptions
                            // ====================

class EncoderOptions {
    // Options for performing JSON encoding.  'EncodingStyle' is either
    // 'COMPACT' or 'PRETTY'.  If 'EncodingStyle' is 'COMPACT', no whitespace
    // will be added between elements.  If encoding style is 'PRETTY', then the
    // 'InitialIndentLevel' and 'SpacesPerLevel' parameters are used to specify
    // the formatting of the output.  Note that 'InitialIndentLevel' and
    // 'SpacesPerLevel' are ignored when 'EncodingStyle' is 'COMPACT' (this is
    // the default).  The 'EncodeEmptyArrays' and 'EncodeNullElements' encode
    // empty array and null elements respectively.  By default empty array and
    // null elements are not encoded.  The 'EncodeInfAndNaNAsStrings' attribute
    // provides users the option to encode 'Infinity' and 'NaN' floating point
    // values as strings.  These values do not have a valid JSON representation
    // and by default such value will result in an encoding error.  The
    // 'DatetimeFractionalSecondPrecision' specifies the precision of
    // milliseconds printed with date time values.  By default a precision of
    // '3' decimal places is used.  The 'MaxFloatPrecision' and
    // 'MaxDoublePrecision' attributes allow specifying the maximum precision
    // for 'float' and 'double' values. 

    // INSTANCE DATA
    int                   d_initialIndentLevel;
        // initial indentation level for the topmost element 
    int                   d_spacesPerLevel;
        // spaces per additional level of indentation 
    int                   d_datetimeFractionalSecondPrecision;
        // option specifying the number of decimal places used for milliseconds
        // when encoding 'Datetime' and 'DatetimeTz' values 
    int                   d_maxFloatPrecision;
        // option specifying the maximum number of decimal places used to
        // encode each 'float' value 
    int                   d_maxDoublePrecision;
        // option specifying the maximum number of decimal places used to
        // encode each 'double' value 
    baljsn::EncodingStyle::Value  d_encodingStyle;
        // encoding style used to encode values 
    bool                  d_encodeEmptyArrays;
        // option specifying if empty arrays should be encoded 
    bool                  d_encodeNullElements;
        // option specifying if null elements should be encoded 
    bool                  d_encodeInfAndNaNAsStrings;
        // option specifying a way to encode 'Infinity' and 'NaN' floating
        // point values.  JSON does not provide a way to encode these values as
        // they are not numbers.  Although the resulting output is a valid JSON
        // document, decoders expecting floating point numbers to be encoded
        // only as numbers will fail to decode.  Users of this option must
        // therefore exercise caution and ensure that if this option is used
        // then the parser decoding the generated JSON can handle doubles as
        // strings. 

  public:
    // TYPES
    enum EncodingStyle {
        // This 'enum' provides enumerators to specify the encoding styles.
        // This 'enum' is replicated in this 'class' for
        // backwards-compatibility with 'baejsn'.  Users should use
        // 'baljsn::EncodingStyle' directly.

        e_COMPACT = baljsn::EncodingStyle::e_COMPACT,
        e_PRETTY  = baljsn::EncodingStyle::e_PRETTY
    };

    enum {
        ATTRIBUTE_ID_INITIAL_INDENT_LEVEL                 = 0
      , ATTRIBUTE_ID_SPACES_PER_LEVEL                     = 1
      , ATTRIBUTE_ID_ENCODING_STYLE                       = 2
      , ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS                  = 3
      , ATTRIBUTE_ID_ENCODE_NULL_ELEMENTS                 = 4
      , ATTRIBUTE_ID_ENCODE_INF_AND_NA_N_AS_STRINGS       = 5
      , ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION = 6
      , ATTRIBUTE_ID_MAX_FLOAT_PRECISION                  = 7
      , ATTRIBUTE_ID_MAX_DOUBLE_PRECISION                 = 8
    };

    enum {
        NUM_ATTRIBUTES = 9
    };

    enum {
        ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL                 = 0
      , ATTRIBUTE_INDEX_SPACES_PER_LEVEL                     = 1
      , ATTRIBUTE_INDEX_ENCODING_STYLE                       = 2
      , ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS                  = 3
      , ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS                 = 4
      , ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS       = 5
      , ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION = 6
      , ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION                  = 7
      , ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION                 = 8
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;

    static const int DEFAULT_INITIALIZER_SPACES_PER_LEVEL;

    static const baljsn::EncodingStyle::Value DEFAULT_INITIALIZER_ENCODING_STYLE;

    static const bool DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;

    static const bool DEFAULT_INITIALIZER_ENCODE_NULL_ELEMENTS;

    static const bool DEFAULT_INITIALIZER_ENCODE_INF_AND_NA_N_AS_STRINGS;

    static const int DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION;

    static const int DEFAULT_INITIALIZER_MAX_FLOAT_PRECISION;

    static const int DEFAULT_INITIALIZER_MAX_DOUBLE_PRECISION;

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
    EncoderOptions();
        // Create an object of type 'EncoderOptions' having the default value.

    EncoderOptions(const EncoderOptions& original);
        // Create an object of type 'EncoderOptions' having the value of the
        // specified 'original' object.

    ~EncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    EncoderOptions& operator=(const EncoderOptions& rhs);
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

    void setInitialIndentLevel(int value);
        // Set the "InitialIndentLevel" attribute of this object to the
        // specified 'value'.

    void setSpacesPerLevel(int value);
        // Set the "SpacesPerLevel" attribute of this object to the specified
        // 'value'.

    void setEncodingStyle(baljsn::EncodingStyle::Value value);
    void setEncodingStyle(baljsn::EncoderOptions::EncodingStyle value);
        // Set the "EncodingStyle" attribute of this object to the specified
        // 'value'.

    void setEncodeEmptyArrays(bool value);
        // Set the "EncodeEmptyArrays" attribute of this object to the
        // specified 'value'.

    void setEncodeNullElements(bool value);
        // Set the "EncodeNullElements" attribute of this object to the
        // specified 'value'.

    void setEncodeInfAndNaNAsStrings(bool value);
        // Set the "EncodeInfAndNaNAsStrings" attribute of this object to the
        // specified 'value'.

    void setDatetimeFractionalSecondPrecision(int value);
        // Set the "DatetimeFractionalSecondPrecision" attribute of this object
        // to the specified 'value'.

    void setMaxFloatPrecision(int value);
        // Set the "MaxFloatPrecision" attribute of this object to the
        // specified 'value'.

    void setMaxDoublePrecision(int value);
        // Set the "MaxDoublePrecision" attribute of this object to the
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

    int initialIndentLevel() const;
        // Return a reference to the non-modifiable "InitialIndentLevel"
        // attribute of this object.

    int spacesPerLevel() const;
        // Return a reference to the non-modifiable "SpacesPerLevel" attribute
        // of this object.

    baljsn::EncoderOptions::EncodingStyle encodingStyle() const;
        // Return a reference to the non-modifiable "EncodingStyle" attribute
        // of this object.

    bool encodeEmptyArrays() const;
        // Return a reference to the non-modifiable "EncodeEmptyArrays"
        // attribute of this object.

    bool encodeNullElements() const;
        // Return a reference to the non-modifiable "EncodeNullElements"
        // attribute of this object.

    bool encodeInfAndNaNAsStrings() const;
        // Return a reference to the non-modifiable "EncodeInfAndNaNAsStrings"
        // attribute of this object.

    int datetimeFractionalSecondPrecision() const;
        // Return a reference to the non-modifiable
        // "DatetimeFractionalSecondPrecision" attribute of this object.

    int maxFloatPrecision() const;
        // Return a reference to the non-modifiable "MaxFloatPrecision"
        // attribute of this object.

    int maxDoublePrecision() const;
        // Return a reference to the non-modifiable "MaxDoublePrecision"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderOptions& lhs, const EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderOptions& lhs, const EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderOptions)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace baljsn {

                            // --------------------
                            // class EncoderOptions
                            // --------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodeNullElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodeInfAndNaNAsStrings, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxFloatPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxDoublePrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
      } break;
      case ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
      } break;
      case ATTRIBUTE_ID_ENCODING_STYLE: {
        return manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
      } break;
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return manipulator(&d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      } break;
      case ATTRIBUTE_ID_ENCODE_NULL_ELEMENTS: {
        return manipulator(&d_encodeNullElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS]);
      } break;
      case ATTRIBUTE_ID_ENCODE_INF_AND_NA_N_AS_STRINGS: {
        return manipulator(&d_encodeInfAndNaNAsStrings, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS]);
      } break;
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION: {
        return manipulator(&d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
      } break;
      case ATTRIBUTE_ID_MAX_FLOAT_PRECISION: {
        return manipulator(&d_maxFloatPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION]);
      } break;
      case ATTRIBUTE_ID_MAX_DOUBLE_PRECISION: {
        return manipulator(&d_maxDoublePrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderOptions::manipulateAttribute(
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
void EncoderOptions::setEncodingStyle(baljsn::EncodingStyle::Value value)
{
    d_encodingStyle = value;
}

inline
void EncoderOptions::setEncodingStyle(
                                   baljsn::EncoderOptions::EncodingStyle value)
{
    d_encodingStyle = static_cast<baljsn::EncodingStyle::Value>(value);
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

inline
void EncoderOptions::setEncodeInfAndNaNAsStrings(bool value)
{
    d_encodeInfAndNaNAsStrings = value;
}

inline
void EncoderOptions::setDatetimeFractionalSecondPrecision(int value)
{
    BSLS_ASSERT_SAFE(0 <= value     );
    BSLS_ASSERT_SAFE(     value <= 6);

    d_datetimeFractionalSecondPrecision = value;
}

inline
void EncoderOptions::setMaxFloatPrecision(int value)
{
    BSLS_ASSERT_SAFE(1 <= value     );
    BSLS_ASSERT_SAFE(     value <= 9);

    d_maxFloatPrecision = value;
}

inline
void EncoderOptions::setMaxDoublePrecision(int value)
{
    BSLS_ASSERT_SAFE(1 <= value     );
    BSLS_ASSERT_SAFE(     value <= 17);

    d_maxDoublePrecision = value;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodeNullElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodeInfAndNaNAsStrings, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxFloatPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxDoublePrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
      } break;
      case ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
      } break;
      case ATTRIBUTE_ID_ENCODING_STYLE: {
        return accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
      } break;
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return accessor(d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      } break;
      case ATTRIBUTE_ID_ENCODE_NULL_ELEMENTS: {
        return accessor(d_encodeNullElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS]);
      } break;
      case ATTRIBUTE_ID_ENCODE_INF_AND_NA_N_AS_STRINGS: {
        return accessor(d_encodeInfAndNaNAsStrings, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS]);
      } break;
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION: {
        return accessor(d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
      } break;
      case ATTRIBUTE_ID_MAX_FLOAT_PRECISION: {
        return accessor(d_maxFloatPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION]);
      } break;
      case ATTRIBUTE_ID_MAX_DOUBLE_PRECISION: {
        return accessor(d_maxDoublePrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderOptions::accessAttribute(
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
baljsn::EncoderOptions::EncodingStyle EncoderOptions::encodingStyle() const
{
    return static_cast<baljsn::EncoderOptions::EncodingStyle>(d_encodingStyle);
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

inline
bool EncoderOptions::encodeInfAndNaNAsStrings() const
{
    return d_encodeInfAndNaNAsStrings;
}

inline
int EncoderOptions::datetimeFractionalSecondPrecision() const
{
    return d_datetimeFractionalSecondPrecision;
}

inline
int EncoderOptions::maxFloatPrecision() const
{
    return d_maxFloatPrecision;
}

inline
int EncoderOptions::maxDoublePrecision() const
{
    return d_maxDoublePrecision;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool baljsn::operator==(
        const baljsn::EncoderOptions& lhs,
        const baljsn::EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.spacesPerLevel() == rhs.spacesPerLevel()
         && lhs.encodingStyle() == rhs.encodingStyle()
         && lhs.encodeEmptyArrays() == rhs.encodeEmptyArrays()
         && lhs.encodeNullElements() == rhs.encodeNullElements()
         && lhs.encodeInfAndNaNAsStrings() == rhs.encodeInfAndNaNAsStrings()
         && lhs.datetimeFractionalSecondPrecision() == rhs.datetimeFractionalSecondPrecision()
         && lhs.maxFloatPrecision() == rhs.maxFloatPrecision()
         && lhs.maxDoublePrecision() == rhs.maxDoublePrecision();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderOptions& lhs,
        const baljsn::EncoderOptions& rhs)
{
    return  lhs.initialIndentLevel() != rhs.initialIndentLevel()
         || lhs.spacesPerLevel() != rhs.spacesPerLevel()
         || lhs.encodingStyle() != rhs.encodingStyle()
         || lhs.encodeEmptyArrays() != rhs.encodeEmptyArrays()
         || lhs.encodeNullElements() != rhs.encodeNullElements()
         || lhs.encodeInfAndNaNAsStrings() != rhs.encodeInfAndNaNAsStrings()
         || lhs.datetimeFractionalSecondPrecision() != rhs.datetimeFractionalSecondPrecision()
         || lhs.maxFloatPrecision() != rhs.maxFloatPrecision()
         || lhs.maxDoublePrecision() != rhs.maxDoublePrecision();
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.8.24 Fri Feb 17 12:35:40 2017
// USING bas_codegen.pl -m msg --package baljsn --noExternalization -E --noAggregateConversion baljsn.xsd
// SERVICE VERSION 
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
