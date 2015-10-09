// balxml_encoderoptions.h-- GENERATED FILE - DO NOT EDIT ---*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_ENCODEROPTIONS
#define INCLUDED_BALXML_ENCODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(baexml_encoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@DESCRIPTION:  Options for performing XML encodings.  Encoding style is
// either COMPACT or PRETTY.  If encoding style is COMPACT, no whitespace will
// be added between elements.  If encoding style is 'PRETTY', then the
// 'InitialIndentLevel', 'SpacesPerLevel', and 'WrapColumn' parameters are used
// to specify the formatting of the output.  Note that 'InitialIndentLevel',
// 'SpacesPerLevel', and 'WrapColumn' are ignored when 'EncodingStyle' is
// COMPACT (this is the default).
//
// This class was initially generated using baexml_generateoptions.pl

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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BALXML_ENCODINGSTYLE
#include <balxml_encodingstyle.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace balxml {

                            // ====================
                            // class EncoderOptions
                            // ====================

class EncoderOptions {
    // Options for performing XML encodings.  Encoding style is either COMPACT
    // or PRETTY.  If encoding style is COMPACT, no whitespace will be added
    // between elements.  If encoding style is 'PRETTY', then the
    // 'InitialIndentLevel', 'SpacesPerLevel', and 'WrapColumn' parameters are
    // used to specify the formatting of the output.  Note that
    // 'InitialIndentLevel', 'SpacesPerLevel', and 'WrapColumn' are ignored
    // when 'EncodingStyle' is COMPACT (this is the default).
    // This class is generated using baexml_generateoptions.pl

    // INSTANCE DATA
    bsl::string                      d_objectNamespace;
        // namespace where object is defined
    bsl::string                      d_schemaLocation;
        // location of the schema
    bsl::string                      d_tag;
        // tag for top level
    int                              d_formattingMode;
        // Formatting mode
    int                              d_initialIndentLevel;
        // initial indentation level
    int                              d_spacesPerLevel;
        // spaces per level of indentation
    int                              d_wrapColumn;
        // number of characters to wrap text
    bdlb::NullableValue<int>         d_maxDecimalTotalDigits;
        // Maximum total digits of the decimal value that should be displayed
    bdlb::NullableValue<int>         d_maxDecimalFractionDigits;
        // Maximum fractional digits of the decimal value that should be
        // displayed
    bdlb::NullableValue<int>         d_significantDoubleDigits;
        // The number of significant digits that must be displayed for the
        // double value.
    EncodingStyle::Value  d_encodingStyle;
        // encoding style (see component-level doc)
    bool                             d_allowControlCharacters;
        // Allow control characters to be encoded.
    bool                             d_outputXMLHeader;
        // This option controls if the baexml encoder should output the XML
        // header.
    bool                             d_outputXSIAlias;
        // This option controls if the baexml encoder should output the XSI
        // alias with the top-level element.

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_OBJECT_NAMESPACE            = 0
      , e_ATTRIBUTE_ID_SCHEMA_LOCATION             = 1
      , e_ATTRIBUTE_ID_TAG                         = 2
      , e_ATTRIBUTE_ID_FORMATTING_MODE             = 3
      , e_ATTRIBUTE_ID_INITIAL_INDENT_LEVEL        = 4
      , e_ATTRIBUTE_ID_SPACES_PER_LEVEL            = 5
      , e_ATTRIBUTE_ID_WRAP_COLUMN                 = 6
      , e_ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS    = 7
      , e_ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS = 8
      , e_ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS   = 9
      , e_ATTRIBUTE_ID_ENCODING_STYLE              = 10
      , e_ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS    = 11
      , e_ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER         = 12
      , e_ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS          = 13
    };

    enum {
        k_NUM_ATTRIBUTES = 14
    };

    enum {
        e_ATTRIBUTE_INDEX_OBJECT_NAMESPACE            = 0
      , e_ATTRIBUTE_INDEX_SCHEMA_LOCATION             = 1
      , e_ATTRIBUTE_INDEX_TAG                         = 2
      , e_ATTRIBUTE_INDEX_FORMATTING_MODE             = 3
      , e_ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL        = 4
      , e_ATTRIBUTE_INDEX_SPACES_PER_LEVEL            = 5
      , e_ATTRIBUTE_INDEX_WRAP_COLUMN                 = 6
      , e_ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS    = 7
      , e_ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS = 8
      , e_ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS   = 9
      , e_ATTRIBUTE_INDEX_ENCODING_STYLE              = 10
      , e_ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS    = 11
      , e_ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER         = 12
      , e_ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS          = 13
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_FORMATTING_MODE;

    static const int DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;

    static const int DEFAULT_INITIALIZER_SPACES_PER_LEVEL;

    static const int DEFAULT_INITIALIZER_WRAP_COLUMN;

    static const EncodingStyle::Value DEFAULT_INITIALIZER_ENCODING_STYLE;

    static const bool DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS;

    static const bool DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER;

    static const bool DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS;

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
    explicit EncoderOptions(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderOptions' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    EncoderOptions(const EncoderOptions& original,
                              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderOptions' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

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

    void setObjectNamespace(const bsl::string& value);
        // Set the "ObjectNamespace" attribute of this object to the specified
        // 'value'.

    void setSchemaLocation(const bsl::string& value);
        // Set the "SchemaLocation" attribute of this object to the specified
        // 'value'.

    void setTag(const bsl::string& value);
        // Set the "Tag" attribute of this object to the specified 'value'.

    void setFormattingMode(int value);
        // Set the "FormattingMode" attribute of this object to the specified
        // 'value'.

    void setInitialIndentLevel(int value);
        // Set the "InitialIndentLevel" attribute of this object to the
        // specified 'value'.

    void setSpacesPerLevel(int value);
        // Set the "SpacesPerLevel" attribute of this object to the specified
        // 'value'.

    void setWrapColumn(int value);
        // Set the "WrapColumn" attribute of this object to the specified
        // 'value'.

    void setMaxDecimalTotalDigits(const bdlb::NullableValue<int>& value);
        // Set the "MaxDecimalTotalDigits" attribute of this object to the
        // specified 'value'.

    void setMaxDecimalFractionDigits(const bdlb::NullableValue<int>& value);
        // Set the "MaxDecimalFractionDigits" attribute of this object to the
        // specified 'value'.

    void setSignificantDoubleDigits(const bdlb::NullableValue<int>& value);
        // Set the "SignificantDoubleDigits" attribute of this object to the
        // specified 'value'.

    void setEncodingStyle(EncodingStyle::Value value);
        // Set the "EncodingStyle" attribute of this object to the specified
        // 'value'.

    void setAllowControlCharacters(bool value);
        // Set the "AllowControlCharacters" attribute of this object to the
        // specified 'value'.

    void setOutputXMLHeader(bool value);
        // Set the "OutputXMLHeader" attribute of this object to the specified
        // 'value'.

    void setOutputXSIAlias(bool value);
        // Set the "OutputXSIAlias" attribute of this object to the specified
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

    const bsl::string& objectNamespace() const;
        // Return a reference to the non-modifiable "ObjectNamespace" attribute
        // of this object.

    const bsl::string& schemaLocation() const;
        // Return a reference to the non-modifiable "SchemaLocation" attribute
        // of this object.

    const bsl::string& tag() const;
        // Return a reference to the non-modifiable "Tag" attribute of this
        // object.

    int formattingMode() const;
        // Return a reference to the non-modifiable "FormattingMode" attribute
        // of this object.

    int initialIndentLevel() const;
        // Return a reference to the non-modifiable "InitialIndentLevel"
        // attribute of this object.

    int spacesPerLevel() const;
        // Return a reference to the non-modifiable "SpacesPerLevel" attribute
        // of this object.

    int wrapColumn() const;
        // Return a reference to the non-modifiable "WrapColumn" attribute of
        // this object.

    const bdlb::NullableValue<int>& maxDecimalTotalDigits() const;
        // Return a reference to the non-modifiable "MaxDecimalTotalDigits"
        // attribute of this object.

    const bdlb::NullableValue<int>& maxDecimalFractionDigits() const;
        // Return a reference to the non-modifiable "MaxDecimalFractionDigits"
        // attribute of this object.

    const bdlb::NullableValue<int>& significantDoubleDigits() const;
        // Return a reference to the non-modifiable "SignificantDoubleDigits"
        // attribute of this object.

    EncodingStyle::Value encodingStyle() const;
        // Return a reference to the non-modifiable "EncodingStyle" attribute
        // of this object.

    bool allowControlCharacters() const;
        // Return a reference to the non-modifiable "AllowControlCharacters"
        // attribute of this object.

    bool outputXMLHeader() const;
        // Return a reference to the non-modifiable "OutputXMLHeader" attribute
        // of this object.

    bool outputXSIAlias() const;
        // Return a reference to the non-modifiable "OutputXSIAlias" attribute
        // of this object.
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


// TRAITS

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balxml::EncoderOptions)

namespace balxml {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================


                            // --------------------
                            // class EncoderOptions
                            // --------------------

// MANIPULATORS
template <class MANIPULATOR>
int EncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_objectNamespace, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_schemaLocation, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_tag, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TAG]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_wrapColumn, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRAP_COLUMN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_outputXMLHeader, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_outputXSIAlias, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_OBJECT_NAMESPACE: {
        return manipulator(&d_objectNamespace, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SCHEMA_LOCATION: {
        return manipulator(&d_schemaLocation, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TAG: {
        return manipulator(&d_tag, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TAG]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_FORMATTING_MODE: {
        return manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRAP_COLUMN: {
        return manipulator(&d_wrapColumn, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRAP_COLUMN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS: {
        return manipulator(&d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS: {
        return manipulator(&d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS: {
        return manipulator(&d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ENCODING_STYLE: {
        return manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODING_STYLE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS: {
        return manipulator(&d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER: {
        return manipulator(&d_outputXMLHeader, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS: {
        return manipulator(&d_outputXSIAlias, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int EncoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void EncoderOptions::setObjectNamespace(const bsl::string& value)
{
    d_objectNamespace = value;
}

inline
void EncoderOptions::setSchemaLocation(const bsl::string& value)
{
    d_schemaLocation = value;
}

inline
void EncoderOptions::setTag(const bsl::string& value)
{
    d_tag = value;
}

inline
void EncoderOptions::setFormattingMode(int value)
{
    d_formattingMode = value;
}

inline
void EncoderOptions::setInitialIndentLevel(int value)
{
    d_initialIndentLevel = value;
}

inline
void EncoderOptions::setSpacesPerLevel(int value)
{
    d_spacesPerLevel = value;
}

inline
void EncoderOptions::setWrapColumn(int value)
{
    d_wrapColumn = value;
}

inline
void EncoderOptions::setMaxDecimalTotalDigits(const bdlb::NullableValue<int>& value)
{
    d_maxDecimalTotalDigits = value;
}

inline
void EncoderOptions::setMaxDecimalFractionDigits(const bdlb::NullableValue<int>& value)
{
    d_maxDecimalFractionDigits = value;
}

inline
void EncoderOptions::setSignificantDoubleDigits(const bdlb::NullableValue<int>& value)
{
    d_significantDoubleDigits = value;
}

inline
void EncoderOptions::setEncodingStyle(EncodingStyle::Value value)
{
    d_encodingStyle = value;
}

inline
void EncoderOptions::setAllowControlCharacters(bool value)
{
    d_allowControlCharacters = value;
}

inline
void EncoderOptions::setOutputXMLHeader(bool value)
{
    d_outputXMLHeader = value;
}

inline
void EncoderOptions::setOutputXSIAlias(bool value)
{
    d_outputXSIAlias = value;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_objectNamespace, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_schemaLocation, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_tag, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TAG]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_wrapColumn, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRAP_COLUMN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_outputXMLHeader, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_outputXSIAlias, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int EncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_OBJECT_NAMESPACE: {
        return accessor(d_objectNamespace, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SCHEMA_LOCATION: {
        return accessor(d_schemaLocation, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TAG: {
        return accessor(d_tag, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TAG]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_FORMATTING_MODE: {
        return accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRAP_COLUMN: {
        return accessor(d_wrapColumn, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRAP_COLUMN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS: {
        return accessor(d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS: {
        return accessor(d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS: {
        return accessor(d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ENCODING_STYLE: {
        return accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODING_STYLE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS: {
        return accessor(d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER: {
        return accessor(d_outputXMLHeader, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS: {
        return accessor(d_outputXSIAlias, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int EncoderOptions::accessAttribute(
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
const bsl::string& EncoderOptions::objectNamespace() const
{
    return d_objectNamespace;
}

inline
const bsl::string& EncoderOptions::schemaLocation() const
{
    return d_schemaLocation;
}

inline
const bsl::string& EncoderOptions::tag() const
{
    return d_tag;
}

inline
int EncoderOptions::formattingMode() const
{
    return d_formattingMode;
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
int EncoderOptions::wrapColumn() const
{
    return d_wrapColumn;
}

inline
const bdlb::NullableValue<int>& EncoderOptions::maxDecimalTotalDigits() const
{
    return d_maxDecimalTotalDigits;
}

inline
const bdlb::NullableValue<int>& EncoderOptions::maxDecimalFractionDigits() const
{
    return d_maxDecimalFractionDigits;
}

inline
const bdlb::NullableValue<int>& EncoderOptions::significantDoubleDigits() const
{
    return d_significantDoubleDigits;
}

inline
EncodingStyle::Value EncoderOptions::encodingStyle() const
{
    return d_encodingStyle;
}

inline
bool EncoderOptions::allowControlCharacters() const
{
    return d_allowControlCharacters;
}

inline
bool EncoderOptions::outputXMLHeader() const
{
    return d_outputXMLHeader;
}

inline
bool EncoderOptions::outputXSIAlias() const
{
    return d_outputXSIAlias;
}
}  // close package namespace


// FREE FUNCTIONS

inline
bool balxml::operator==(
        const EncoderOptions& lhs,
        const EncoderOptions& rhs)
{
    return  lhs.objectNamespace() == rhs.objectNamespace()
         && lhs.schemaLocation() == rhs.schemaLocation()
         && lhs.tag() == rhs.tag()
         && lhs.formattingMode() == rhs.formattingMode()
         && lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.spacesPerLevel() == rhs.spacesPerLevel()
         && lhs.wrapColumn() == rhs.wrapColumn()
         && lhs.maxDecimalTotalDigits() == rhs.maxDecimalTotalDigits()
         && lhs.maxDecimalFractionDigits() == rhs.maxDecimalFractionDigits()
         && lhs.significantDoubleDigits() == rhs.significantDoubleDigits()
         && lhs.encodingStyle() == rhs.encodingStyle()
         && lhs.allowControlCharacters() == rhs.allowControlCharacters()
         && lhs.outputXMLHeader() == rhs.outputXMLHeader()
         && lhs.outputXSIAlias() == rhs.outputXSIAlias();
}

inline
bool balxml::operator!=(
        const EncoderOptions& lhs,
        const EncoderOptions& rhs)
{
    return  lhs.objectNamespace() != rhs.objectNamespace()
         || lhs.schemaLocation() != rhs.schemaLocation()
         || lhs.tag() != rhs.tag()
         || lhs.formattingMode() != rhs.formattingMode()
         || lhs.initialIndentLevel() != rhs.initialIndentLevel()
         || lhs.spacesPerLevel() != rhs.spacesPerLevel()
         || lhs.wrapColumn() != rhs.wrapColumn()
         || lhs.maxDecimalTotalDigits() != rhs.maxDecimalTotalDigits()
         || lhs.maxDecimalFractionDigits() != rhs.maxDecimalFractionDigits()
         || lhs.significantDoubleDigits() != rhs.significantDoubleDigits()
         || lhs.encodingStyle() != rhs.encodingStyle()
         || lhs.allowControlCharacters() != rhs.allowControlCharacters()
         || lhs.outputXMLHeader() != rhs.outputXMLHeader()
         || lhs.outputXSIAlias() != rhs.outputXSIAlias();
}

inline
bsl::ostream& balxml::operator<<(
        bsl::ostream& stream,
        const EncoderOptions& rhs)
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
