// baexml_encoderoptions.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAEXML_ENCODEROPTIONS
#define INCLUDED_BAEXML_ENCODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baexml_encoderoptions_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION:
// Options for performing XML encodings.  Encoding style is either COMPACT or
// PRETTY.  If encoding style is COMPACT, no whitespace will be added between
// elements.  If encoding style is 'PRETTY', then the 'InitialIndentLevel',
// 'SpacesPerLevel', and 'WrapColumn' parameters are used to specify the
// formatting of the output.  Note that 'InitialIndentLevel', 'SpacesPerLevel',
// and 'WrapColumn' are ignored when 'EncodingStyle' is COMPACT (this is the
// default).
// This class is generated using baexml_generateoptions.pl
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

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BAEXML_ENCODINGSTYLE
#include <baexml_encodingstyle.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {

class bslma_Allocator;


                            // ===========================                     
                            // class baexml_EncoderOptions
                            // ===========================                     

class baexml_EncoderOptions {
    // Options for performing XML encodings.  Encoding style is either COMPACT
    // or PRETTY.  If encoding style is COMPACT, no whitespace will be added
    // between elements.  If encoding style is 'PRETTY', then the
    // 'InitialIndentLevel', 'SpacesPerLevel', and 'WrapColumn' parameters are
    // used to specify the formatting of the output.  Note that
    // 'InitialIndentLevel', 'SpacesPerLevel', and 'WrapColumn' are ignored
    // when 'EncodingStyle' is COMPACT (this is the default).  
    // This class is generated using baexml_generateoptions.pl 

    // INSTANCE DATA
    bsl::string               d_objectNamespace;
        // namespace where object is defined 
    bsl::string               d_schemaLocation;
        // location of the schema
    bsl::string               d_tag;
        // tag for top level
    int                       d_formattingMode;
        // Formatting mode
    int                       d_initialIndentLevel;
        // initial indentation level
    int                       d_spacesPerLevel;
        // spaces per level of indentation 
    int                       d_wrapColumn;
        // number of characters to wrap text 
    bdeut_NullableValue<int>  d_maxDecimalTotalDigits;
        // Maximum total digits of the decimal value that should be displayed 
    bdeut_NullableValue<int>  d_maxDecimalFractionDigits;
        // Maximum fractional digits of the decimal value that should be
        // displayed 
    bdeut_NullableValue<int>  d_significantDoubleDigits;
        // The number of significant digits that must be displayed for the
        // double value. 
    baexml_EncodingStyle::Value d_encodingStyle;
        // encoding style (see component-level doc) 
    bool                      d_allowControlCharacters;
        // Allow control characters to be encoded. 

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_OBJECT_NAMESPACE            = 0
      , ATTRIBUTE_ID_SCHEMA_LOCATION             = 1
      , ATTRIBUTE_ID_TAG                         = 2
      , ATTRIBUTE_ID_FORMATTING_MODE             = 3
      , ATTRIBUTE_ID_INITIAL_INDENT_LEVEL        = 4
      , ATTRIBUTE_ID_SPACES_PER_LEVEL            = 5
      , ATTRIBUTE_ID_WRAP_COLUMN                 = 6
      , ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS    = 7
      , ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS = 8
      , ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS   = 9
      , ATTRIBUTE_ID_ENCODING_STYLE              = 10
      , ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS    = 11
    };

    enum {
        NUM_ATTRIBUTES = 12
    };

    enum {
        ATTRIBUTE_INDEX_OBJECT_NAMESPACE            = 0
      , ATTRIBUTE_INDEX_SCHEMA_LOCATION             = 1
      , ATTRIBUTE_INDEX_TAG                         = 2
      , ATTRIBUTE_INDEX_FORMATTING_MODE             = 3
      , ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL        = 4
      , ATTRIBUTE_INDEX_SPACES_PER_LEVEL            = 5
      , ATTRIBUTE_INDEX_WRAP_COLUMN                 = 6
      , ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS    = 7
      , ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS = 8
      , ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS   = 9
      , ATTRIBUTE_INDEX_ENCODING_STYLE              = 10
      , ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS    = 11
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_FORMATTING_MODE;

    static const int DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;

    static const int DEFAULT_INITIALIZER_SPACES_PER_LEVEL;

    static const int DEFAULT_INITIALIZER_WRAP_COLUMN;

    static const baexml_EncodingStyle::Value DEFAULT_INITIALIZER_ENCODING_STYLE;

    static const bool DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS;

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
    explicit baexml_EncoderOptions(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baexml_EncoderOptions' having the default value. 
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baexml_EncoderOptions(const baexml_EncoderOptions& original,
                   bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baexml_EncoderOptions' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baexml_EncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    baexml_EncoderOptions& operator=(const baexml_EncoderOptions& rhs);
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

    void setMaxDecimalTotalDigits(const bdeut_NullableValue<int>& value);
        // Set the "MaxDecimalTotalDigits" attribute of this object to the
        // specified 'value'.

    void setMaxDecimalFractionDigits(const bdeut_NullableValue<int>& value);
        // Set the "MaxDecimalFractionDigits" attribute of this object to the
        // specified 'value'.

    void setSignificantDoubleDigits(const bdeut_NullableValue<int>& value);
        // Set the "SignificantDoubleDigits" attribute of this object to the
        // specified 'value'.

    void setEncodingStyle(baexml_EncodingStyle::Value value);
        // Set the "EncodingStyle" attribute of this object to the specified
        // 'value'.

    void setAllowControlCharacters(bool value);
        // Set the "AllowControlCharacters" attribute of this object to the
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

    const bsl::string& objectNamespace() const;
        // Return a reference to the non-modifiable "ObjectNamespace" attribute
        // of this object.

    const bsl::string& schemaLocation() const;
        // Return a reference to the non-modifiable "SchemaLocation" attribute
        // of this object.

    const bsl::string& tag() const;
        // Return a reference to the non-modifiable "Tag" attribute of this
        // object.

    const int& formattingMode() const;
        // Return a reference to the non-modifiable "FormattingMode" attribute
        // of this object.

    const int& initialIndentLevel() const;
        // Return a reference to the non-modifiable "InitialIndentLevel"
        // attribute of this object.

    const int& spacesPerLevel() const;
        // Return a reference to the non-modifiable "SpacesPerLevel" attribute
        // of this object.

    const int& wrapColumn() const;
        // Return a reference to the non-modifiable "WrapColumn" attribute of
        // this object.

    const bdeut_NullableValue<int>& maxDecimalTotalDigits() const;
        // Return a reference to the non-modifiable "MaxDecimalTotalDigits"
        // attribute of this object.

    const bdeut_NullableValue<int>& maxDecimalFractionDigits() const;
        // Return a reference to the non-modifiable "MaxDecimalFractionDigits"
        // attribute of this object.

    const bdeut_NullableValue<int>& significantDoubleDigits() const;
        // Return a reference to the non-modifiable "SignificantDoubleDigits"
        // attribute of this object.

    const baexml_EncodingStyle::Value& encodingStyle() const;
        // Return a reference to the non-modifiable "EncodingStyle" attribute
        // of this object.

    const bool& allowControlCharacters() const;
        // Return a reference to the non-modifiable "AllowControlCharacters"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const baexml_EncoderOptions& lhs, const baexml_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baexml_EncoderOptions& lhs, const baexml_EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baexml_EncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baexml_EncoderOptions)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                            // ---------------------------                     
                            // class baexml_EncoderOptions
                            // ---------------------------                     

// CLASS METHODS
inline
int baexml_EncoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baexml_EncoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_objectNamespace, 1);
            bdex_InStreamFunctions::streamIn(stream, d_schemaLocation, 1);
            bdex_InStreamFunctions::streamIn(stream, d_tag, 1);
            bdex_InStreamFunctions::streamIn(stream, d_formattingMode, 1);
            bdex_InStreamFunctions::streamIn(stream, d_initialIndentLevel, 1);
            bdex_InStreamFunctions::streamIn(stream, d_spacesPerLevel, 1);
            bdex_InStreamFunctions::streamIn(stream, d_wrapColumn, 1);
            bdex_InStreamFunctions::streamIn(stream, d_maxDecimalTotalDigits, 1);
            bdex_InStreamFunctions::streamIn(stream, d_maxDecimalFractionDigits, 1);
            bdex_InStreamFunctions::streamIn(stream, d_significantDoubleDigits, 1);
            baexml_EncodingStyle::bdexStreamIn(stream, d_encodingStyle, 1);
            bdex_InStreamFunctions::streamIn(stream, d_allowControlCharacters, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baexml_EncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_objectNamespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_schemaLocation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_tag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_wrapColumn, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baexml_EncoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_OBJECT_NAMESPACE: {
        return manipulator(&d_objectNamespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
      } break;
      case ATTRIBUTE_ID_SCHEMA_LOCATION: {
        return manipulator(&d_schemaLocation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
      } break;
      case ATTRIBUTE_ID_TAG: {
        return manipulator(&d_tag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG]);
      } break;
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return manipulator(&d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      } break;
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return manipulator(&d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
      } break;
      case ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return manipulator(&d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
      } break;
      case ATTRIBUTE_ID_WRAP_COLUMN: {
        return manipulator(&d_wrapColumn, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN]);
      } break;
      case ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS: {
        return manipulator(&d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
      } break;
      case ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS: {
        return manipulator(&d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
      } break;
      case ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS: {
        return manipulator(&d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
      } break;
      case ATTRIBUTE_ID_ENCODING_STYLE: {
        return manipulator(&d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
      } break;
      case ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS: {
        return manipulator(&d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baexml_EncoderOptions::manipulateAttribute(
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
void baexml_EncoderOptions::setObjectNamespace(const bsl::string& value)
{
    d_objectNamespace = value;
}

inline
void baexml_EncoderOptions::setSchemaLocation(const bsl::string& value)
{
    d_schemaLocation = value;
}

inline
void baexml_EncoderOptions::setTag(const bsl::string& value)
{
    d_tag = value;
}

inline
void baexml_EncoderOptions::setFormattingMode(int value)
{
    d_formattingMode = value;
}

inline
void baexml_EncoderOptions::setInitialIndentLevel(int value)
{
    d_initialIndentLevel = value;
}

inline
void baexml_EncoderOptions::setSpacesPerLevel(int value)
{
    d_spacesPerLevel = value;
}

inline
void baexml_EncoderOptions::setWrapColumn(int value)
{
    d_wrapColumn = value;
}

inline
void baexml_EncoderOptions::setMaxDecimalTotalDigits(const bdeut_NullableValue<int>& value)
{
    d_maxDecimalTotalDigits = value;
}

inline
void baexml_EncoderOptions::setMaxDecimalFractionDigits(const bdeut_NullableValue<int>& value)
{
    d_maxDecimalFractionDigits = value;
}

inline
void baexml_EncoderOptions::setSignificantDoubleDigits(const bdeut_NullableValue<int>& value)
{
    d_significantDoubleDigits = value;
}

inline
void baexml_EncoderOptions::setEncodingStyle(baexml_EncodingStyle::Value value)
{
    d_encodingStyle = value;
}

inline
void baexml_EncoderOptions::setAllowControlCharacters(bool value)
{
    d_allowControlCharacters = value;
}

// ACCESSORS
template <class STREAM>
STREAM& baexml_EncoderOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_objectNamespace, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_schemaLocation, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_tag, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_formattingMode, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_initialIndentLevel, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_spacesPerLevel, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_wrapColumn, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_maxDecimalTotalDigits, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_maxDecimalFractionDigits, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_significantDoubleDigits, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_encodingStyle, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_allowControlCharacters, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baexml_EncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_objectNamespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_schemaLocation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_tag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_wrapColumn, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baexml_EncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_OBJECT_NAMESPACE: {
        return accessor(d_objectNamespace, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE]);
      } break;
      case ATTRIBUTE_ID_SCHEMA_LOCATION: {
        return accessor(d_schemaLocation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION]);
      } break;
      case ATTRIBUTE_ID_TAG: {
        return accessor(d_tag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG]);
      } break;
      case ATTRIBUTE_ID_FORMATTING_MODE: {
        return accessor(d_formattingMode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE]);
      } break;
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL: {
        return accessor(d_initialIndentLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL]);
      } break;
      case ATTRIBUTE_ID_SPACES_PER_LEVEL: {
        return accessor(d_spacesPerLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL]);
      } break;
      case ATTRIBUTE_ID_WRAP_COLUMN: {
        return accessor(d_wrapColumn, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN]);
      } break;
      case ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS: {
        return accessor(d_maxDecimalTotalDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS]);
      } break;
      case ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS: {
        return accessor(d_maxDecimalFractionDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS]);
      } break;
      case ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS: {
        return accessor(d_significantDoubleDigits, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS]);
      } break;
      case ATTRIBUTE_ID_ENCODING_STYLE: {
        return accessor(d_encodingStyle, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE]);
      } break;
      case ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS: {
        return accessor(d_allowControlCharacters, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baexml_EncoderOptions::accessAttribute(
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
const bsl::string& baexml_EncoderOptions::objectNamespace() const
{
    return d_objectNamespace;
}

inline
const bsl::string& baexml_EncoderOptions::schemaLocation() const
{
    return d_schemaLocation;
}

inline
const bsl::string& baexml_EncoderOptions::tag() const
{
    return d_tag;
}

inline
const int& baexml_EncoderOptions::formattingMode() const
{
    return d_formattingMode;
}

inline
const int& baexml_EncoderOptions::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
const int& baexml_EncoderOptions::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
const int& baexml_EncoderOptions::wrapColumn() const
{
    return d_wrapColumn;
}

inline
const bdeut_NullableValue<int>& baexml_EncoderOptions::maxDecimalTotalDigits() const
{
    return d_maxDecimalTotalDigits;
}

inline
const bdeut_NullableValue<int>& baexml_EncoderOptions::maxDecimalFractionDigits() const
{
    return d_maxDecimalFractionDigits;
}

inline
const bdeut_NullableValue<int>& baexml_EncoderOptions::significantDoubleDigits() const
{
    return d_significantDoubleDigits;
}

inline
const baexml_EncodingStyle::Value& baexml_EncoderOptions::encodingStyle() const
{
    return d_encodingStyle;
}

inline
const bool& baexml_EncoderOptions::allowControlCharacters() const
{
    return d_allowControlCharacters;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baexml_EncoderOptions& lhs,
        const baexml_EncoderOptions& rhs)
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
         && lhs.allowControlCharacters() == rhs.allowControlCharacters();
}

inline
bool operator!=(
        const baexml_EncoderOptions& lhs,
        const baexml_EncoderOptions& rhs)
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
         || lhs.allowControlCharacters() != rhs.allowControlCharacters();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baexml_EncoderOptions& rhs)
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
