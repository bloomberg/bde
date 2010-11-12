// baexml_encoderoptions.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_encoderoptions_cpp,"$Id$ $CSID$ $CCId$")

#include <baexml_encoderoptions.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baexml_encodingstyle.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                      // -------------------------------                       
                      // class baexml_EncoderOptions                       
                      // -------------------------------                       

// CONSTANTS

const char baexml_EncoderOptions::CLASS_NAME[] = "baexml_EncoderOptions";

const int baexml_EncoderOptions::DEFAULT_FORMATTING_MODE = 0;

const int baexml_EncoderOptions::DEFAULT_INITIAL_INDENT_LEVEL = 0;

const int baexml_EncoderOptions::DEFAULT_SPACES_PER_LEVEL = 4;

const int baexml_EncoderOptions::DEFAULT_WRAP_COLUMN = 80;

const int baexml_EncoderOptions::DEFAULT_MAX_DECIMAL_TOTAL_DIGITS = 18;

const int baexml_EncoderOptions::DEFAULT_MAX_DECIMAL_FRACTION_DIGITS = 18;

const int baexml_EncoderOptions::DEFAULT_SIGNIFICANT_DOUBLE_DIGITS = 18;

const baexml_EncodingStyle::Value baexml_EncoderOptions::DEFAULT_ENCODING_STYLE = baexml_EncodingStyle::BAEXML_COMPACT;

const bool baexml_EncoderOptions::DEFAULT_ALLOW_CONTROL_CHARACTERS = false;

const bdeat_AttributeInfo baexml_EncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_OBJECT_NAMESPACE,
        "ObjectNamespace",
        sizeof("ObjectNamespace") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_SCHEMA_LOCATION,
        "SchemaLocation",
        sizeof("SchemaLocation") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_TAG,
        "Tag",
        sizeof("Tag") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_FORMATTING_MODE,
        "FormattingMode",
        sizeof("FormattingMode") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_INITIAL_INDENT_LEVEL,
        "InitialIndentLevel",
        sizeof("InitialIndentLevel") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_SPACES_PER_LEVEL,
        "SpacesPerLevel",
        sizeof("SpacesPerLevel") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_WRAP_COLUMN,
        "WrapColumn",
        sizeof("WrapColumn") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS,
        "MaxDecimalTotalDigits",
        sizeof("MaxDecimalTotalDigits") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS,
        "MaxDecimalFractionDigits",
        sizeof("MaxDecimalFractionDigits") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS,
        "SignificantDoubleDigits",
        sizeof("SignificantDoubleDigits") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ENCODING_STYLE,
        "EncodingStyle",
        sizeof("EncodingStyle") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS,
        "AllowControlCharacters",
        sizeof("AllowControlCharacters") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baexml_EncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if ((name[0]|0x20)=='t'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='g')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG];
            }
        } break;
        case 10: {
            if ((name[0]|0x20)=='w'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='p'
             && (name[4]|0x20)=='c'
             && (name[5]|0x20)=='o'
             && (name[6]|0x20)=='l'
             && (name[7]|0x20)=='u'
             && (name[8]|0x20)=='m'
             && (name[9]|0x20)=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN];
            }
        } break;
        case 13: {
            if ((name[0]|0x20)=='e'
             && (name[1]|0x20)=='n'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='o'
             && (name[4]|0x20)=='d'
             && (name[5]|0x20)=='i'
             && (name[6]|0x20)=='n'
             && (name[7]|0x20)=='g'
             && (name[8]|0x20)=='s'
             && (name[9]|0x20)=='t'
             && (name[10]|0x20)=='y'
             && (name[11]|0x20)=='l'
             && (name[12]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE];
            }
        } break;
        case 14: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'F': {
                    if ((name[1]|0x20)=='o'
                     && (name[2]|0x20)=='r'
                     && (name[3]|0x20)=='m'
                     && (name[4]|0x20)=='a'
                     && (name[5]|0x20)=='t'
                     && (name[6]|0x20)=='t'
                     && (name[7]|0x20)=='i'
                     && (name[8]|0x20)=='n'
                     && (name[9]|0x20)=='g'
                     && (name[10]|0x20)=='m'
                     && (name[11]|0x20)=='o'
                     && (name[12]|0x20)=='d'
                     && (name[13]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
                    }
                } break;
                case 'S': {
                    switch(bdeu_CharType::toUpper(name[1])) {
                        case 'C': {
                            if ((name[2]|0x20)=='h'
                             && (name[3]|0x20)=='e'
                             && (name[4]|0x20)=='m'
                             && (name[5]|0x20)=='a'
                             && (name[6]|0x20)=='l'
                             && (name[7]|0x20)=='o'
                             && (name[8]|0x20)=='c'
                             && (name[9]|0x20)=='a'
                             && (name[10]|0x20)=='t'
                             && (name[11]|0x20)=='i'
                             && (name[12]|0x20)=='o'
                             && (name[13]|0x20)=='n')
                            {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION];
                            }
                        } break;
                        case 'P': {
                            if ((name[2]|0x20)=='a'
                             && (name[3]|0x20)=='c'
                             && (name[4]|0x20)=='e'
                             && (name[5]|0x20)=='s'
                             && (name[6]|0x20)=='p'
                             && (name[7]|0x20)=='e'
                             && (name[8]|0x20)=='r'
                             && (name[9]|0x20)=='l'
                             && (name[10]|0x20)=='e'
                             && (name[11]|0x20)=='v'
                             && (name[12]|0x20)=='e'
                             && (name[13]|0x20)=='l')
                            {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL];
                            }
                        } break;
                    }
                } break;
            }
        } break;
        case 15: {
            if ((name[0]|0x20)=='o'
             && (name[1]|0x20)=='b'
             && (name[2]|0x20)=='j'
             && (name[3]|0x20)=='e'
             && (name[4]|0x20)=='c'
             && (name[5]|0x20)=='t'
             && (name[6]|0x20)=='n'
             && (name[7]|0x20)=='a'
             && (name[8]|0x20)=='m'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='s'
             && (name[11]|0x20)=='p'
             && (name[12]|0x20)=='a'
             && (name[13]|0x20)=='c'
             && (name[14]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE];
            }
        } break;
        case 18: {
            if ((name[0]|0x20)=='i'
             && (name[1]|0x20)=='n'
             && (name[2]|0x20)=='i'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='i'
             && (name[5]|0x20)=='a'
             && (name[6]|0x20)=='l'
             && (name[7]|0x20)=='i'
             && (name[8]|0x20)=='n'
             && (name[9]|0x20)=='d'
             && (name[10]|0x20)=='e'
             && (name[11]|0x20)=='n'
             && (name[12]|0x20)=='t'
             && (name[13]|0x20)=='l'
             && (name[14]|0x20)=='e'
             && (name[15]|0x20)=='v'
             && (name[16]|0x20)=='e'
             && (name[17]|0x20)=='l')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL];
            }
        } break;
        case 21: {
            if ((name[0]|0x20)=='m'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='x'
             && (name[3]|0x20)=='d'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='i'
             && (name[7]|0x20)=='m'
             && (name[8]|0x20)=='a'
             && (name[9]|0x20)=='l'
             && (name[10]|0x20)=='t'
             && (name[11]|0x20)=='o'
             && (name[12]|0x20)=='t'
             && (name[13]|0x20)=='a'
             && (name[14]|0x20)=='l'
             && (name[15]|0x20)=='d'
             && (name[16]|0x20)=='i'
             && (name[17]|0x20)=='g'
             && (name[18]|0x20)=='i'
             && (name[19]|0x20)=='t'
             && (name[20]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS];
            }
        } break;
        case 22: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='l'
             && (name[2]|0x20)=='l'
             && (name[3]|0x20)=='o'
             && (name[4]|0x20)=='w'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='o'
             && (name[7]|0x20)=='n'
             && (name[8]|0x20)=='t'
             && (name[9]|0x20)=='r'
             && (name[10]|0x20)=='o'
             && (name[11]|0x20)=='l'
             && (name[12]|0x20)=='c'
             && (name[13]|0x20)=='h'
             && (name[14]|0x20)=='a'
             && (name[15]|0x20)=='r'
             && (name[16]|0x20)=='a'
             && (name[17]|0x20)=='c'
             && (name[18]|0x20)=='t'
             && (name[19]|0x20)=='e'
             && (name[20]|0x20)=='r'
             && (name[21]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS];
            }
        } break;
        case 23: {
            if ((name[0]|0x20)=='s'
             && (name[1]|0x20)=='i'
             && (name[2]|0x20)=='g'
             && (name[3]|0x20)=='n'
             && (name[4]|0x20)=='i'
             && (name[5]|0x20)=='f'
             && (name[6]|0x20)=='i'
             && (name[7]|0x20)=='c'
             && (name[8]|0x20)=='a'
             && (name[9]|0x20)=='n'
             && (name[10]|0x20)=='t'
             && (name[11]|0x20)=='d'
             && (name[12]|0x20)=='o'
             && (name[13]|0x20)=='u'
             && (name[14]|0x20)=='b'
             && (name[15]|0x20)=='l'
             && (name[16]|0x20)=='e'
             && (name[17]|0x20)=='d'
             && (name[18]|0x20)=='i'
             && (name[19]|0x20)=='g'
             && (name[20]|0x20)=='i'
             && (name[21]|0x20)=='t'
             && (name[22]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS];
            }
        } break;
        case 24: {
            if ((name[0]|0x20)=='m'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='x'
             && (name[3]|0x20)=='d'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='i'
             && (name[7]|0x20)=='m'
             && (name[8]|0x20)=='a'
             && (name[9]|0x20)=='l'
             && (name[10]|0x20)=='f'
             && (name[11]|0x20)=='r'
             && (name[12]|0x20)=='a'
             && (name[13]|0x20)=='c'
             && (name[14]|0x20)=='t'
             && (name[15]|0x20)=='i'
             && (name[16]|0x20)=='o'
             && (name[17]|0x20)=='n'
             && (name[18]|0x20)=='d'
             && (name[19]|0x20)=='i'
             && (name[20]|0x20)=='g'
             && (name[21]|0x20)=='i'
             && (name[22]|0x20)=='t'
             && (name[23]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baexml_EncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_OBJECT_NAMESPACE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE];
      case ATTRIBUTE_ID_SCHEMA_LOCATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION];
      case ATTRIBUTE_ID_TAG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG];
      case ATTRIBUTE_ID_FORMATTING_MODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL];
      case ATTRIBUTE_ID_SPACES_PER_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL];
      case ATTRIBUTE_ID_WRAP_COLUMN:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN];
      case ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS];
      case ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS];
      case ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS];
      case ATTRIBUTE_ID_ENCODING_STYLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE];
      case ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS];
      default:
        return 0;
    }
}

// CREATORS

baexml_EncoderOptions::baexml_EncoderOptions(bslma_Allocator *basicAllocator)
: d_objectNamespace(basicAllocator)
, d_schemaLocation(basicAllocator)
, d_tag(basicAllocator)
, d_formattingMode(DEFAULT_FORMATTING_MODE)
, d_initialIndentLevel(DEFAULT_INITIAL_INDENT_LEVEL)
, d_spacesPerLevel(DEFAULT_SPACES_PER_LEVEL)
, d_wrapColumn(DEFAULT_WRAP_COLUMN)
, d_maxDecimalTotalDigits(DEFAULT_MAX_DECIMAL_TOTAL_DIGITS)
, d_maxDecimalFractionDigits(DEFAULT_MAX_DECIMAL_FRACTION_DIGITS)
, d_significantDoubleDigits(DEFAULT_SIGNIFICANT_DOUBLE_DIGITS)
, d_encodingStyle(DEFAULT_ENCODING_STYLE)
, d_allowControlCharacters(DEFAULT_ALLOW_CONTROL_CHARACTERS)
{
}

baexml_EncoderOptions::baexml_EncoderOptions(const baexml_EncoderOptions& original,
                                                     bslma_Allocator *basicAllocator)
: d_objectNamespace(original.d_objectNamespace, basicAllocator)
, d_schemaLocation(original.d_schemaLocation, basicAllocator)
, d_tag(original.d_tag, basicAllocator)
, d_formattingMode(original.d_formattingMode)
, d_initialIndentLevel(original.d_initialIndentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_wrapColumn(original.d_wrapColumn)
, d_maxDecimalTotalDigits(original.d_maxDecimalTotalDigits)
, d_maxDecimalFractionDigits(original.d_maxDecimalFractionDigits)
, d_significantDoubleDigits(original.d_significantDoubleDigits)
, d_encodingStyle(original.d_encodingStyle)
, d_allowControlCharacters(original.d_allowControlCharacters)
{
}

baexml_EncoderOptions::~baexml_EncoderOptions()
{
}

// MANIPULATORS

baexml_EncoderOptions&
baexml_EncoderOptions::operator=(const baexml_EncoderOptions& rhs)
{
    if (this != &rhs) {
        d_objectNamespace = rhs.d_objectNamespace;
        d_schemaLocation = rhs.d_schemaLocation;
        d_tag = rhs.d_tag;
        d_formattingMode = rhs.d_formattingMode;
        d_initialIndentLevel = rhs.d_initialIndentLevel;
        d_spacesPerLevel = rhs.d_spacesPerLevel;
        d_wrapColumn = rhs.d_wrapColumn;
        d_maxDecimalTotalDigits = rhs.d_maxDecimalTotalDigits;
        d_maxDecimalFractionDigits = rhs.d_maxDecimalFractionDigits;
        d_significantDoubleDigits = rhs.d_significantDoubleDigits;
        d_encodingStyle = rhs.d_encodingStyle;
        d_allowControlCharacters = rhs.d_allowControlCharacters;
    }
    return *this;
}

void baexml_EncoderOptions::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_objectNamespace);
    bdeat_ValueTypeFunctions::reset(&d_schemaLocation);
    bdeat_ValueTypeFunctions::reset(&d_tag);
    d_formattingMode = DEFAULT_FORMATTING_MODE;
    d_initialIndentLevel = DEFAULT_INITIAL_INDENT_LEVEL;
    d_spacesPerLevel = DEFAULT_SPACES_PER_LEVEL;
    d_wrapColumn = DEFAULT_WRAP_COLUMN;
    d_maxDecimalTotalDigits = DEFAULT_MAX_DECIMAL_TOTAL_DIGITS;
    d_maxDecimalFractionDigits = DEFAULT_MAX_DECIMAL_FRACTION_DIGITS;
    d_significantDoubleDigits = DEFAULT_SIGNIFICANT_DOUBLE_DIGITS;
    d_encodingStyle = DEFAULT_ENCODING_STYLE;
    d_allowControlCharacters = DEFAULT_ALLOW_CONTROL_CHARACTERS;
}

// ACCESSORS

bsl::ostream& baexml_EncoderOptions::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ObjectNamespace = ";
        bdeu_PrintMethods::print(stream, d_objectNamespace,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SchemaLocation = ";
        bdeu_PrintMethods::print(stream, d_schemaLocation,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Tag = ";
        bdeu_PrintMethods::print(stream, d_tag,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "FormattingMode = ";
        bdeu_PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "InitialIndentLevel = ";
        bdeu_PrintMethods::print(stream, d_initialIndentLevel,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SpacesPerLevel = ";
        bdeu_PrintMethods::print(stream, d_spacesPerLevel,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "WrapColumn = ";
        bdeu_PrintMethods::print(stream, d_wrapColumn,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MaxDecimalTotalDigits = ";
        bdeu_PrintMethods::print(stream, d_maxDecimalTotalDigits,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MaxDecimalFractionDigits = ";
        bdeu_PrintMethods::print(stream, d_maxDecimalFractionDigits,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SignificantDoubleDigits = ";
        bdeu_PrintMethods::print(stream, d_significantDoubleDigits,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "EncodingStyle = ";
        bdeu_PrintMethods::print(stream, d_encodingStyle,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AllowControlCharacters = ";
        bdeu_PrintMethods::print(stream, d_allowControlCharacters,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "ObjectNamespace = ";
        bdeu_PrintMethods::print(stream, d_objectNamespace,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SchemaLocation = ";
        bdeu_PrintMethods::print(stream, d_schemaLocation,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Tag = ";
        bdeu_PrintMethods::print(stream, d_tag,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "FormattingMode = ";
        bdeu_PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "InitialIndentLevel = ";
        bdeu_PrintMethods::print(stream, d_initialIndentLevel,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SpacesPerLevel = ";
        bdeu_PrintMethods::print(stream, d_spacesPerLevel,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "WrapColumn = ";
        bdeu_PrintMethods::print(stream, d_wrapColumn,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MaxDecimalTotalDigits = ";
        bdeu_PrintMethods::print(stream, d_maxDecimalTotalDigits,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MaxDecimalFractionDigits = ";
        bdeu_PrintMethods::print(stream, d_maxDecimalFractionDigits,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SignificantDoubleDigits = ";
        bdeu_PrintMethods::print(stream, d_significantDoubleDigits,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "EncodingStyle = ";
        bdeu_PrintMethods::print(stream, d_encodingStyle,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AllowControlCharacters = ";
        bdeu_PrintMethods::print(stream, d_allowControlCharacters,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.5.0 Tue May  4 08:22:33 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
