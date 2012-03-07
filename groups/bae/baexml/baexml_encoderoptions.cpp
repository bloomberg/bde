// baexml_encoderoptions.cpp-- GENERATED FILE - DO NOT EDIT ---*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_encoderoptions_cpp,"$Id$ $CSID$")

#include <baexml_encoderoptions.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baexml_encodingstyle.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {

                      // -------------------------------                       
                      // class baexml_EncoderOptions                       
                      // -------------------------------                       

// CONSTANTS

const char baexml_EncoderOptions::CLASS_NAME[] = "baexml_EncoderOptions";

const int baexml_EncoderOptions::DEFAULT_INITIALIZER_FORMATTING_MODE = 0;

const int baexml_EncoderOptions::DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL = 0;

const int baexml_EncoderOptions::DEFAULT_INITIALIZER_SPACES_PER_LEVEL = 4;

const int baexml_EncoderOptions::DEFAULT_INITIALIZER_WRAP_COLUMN = 80;

const baexml_EncodingStyle::Value baexml_EncoderOptions::DEFAULT_INITIALIZER_ENCODING_STYLE = baexml_EncodingStyle::BAEXML_COMPACT;

const bool baexml_EncoderOptions::DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS = false;

const bool baexml_EncoderOptions::DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER = true;

const bool baexml_EncoderOptions::DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS = true;

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
    },
    {
        ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER,
        "OutputXMLHeader",
        sizeof("OutputXMLHeader") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS,
        "OutputXSIAlias",
        sizeof("OutputXSIAlias") - 1,
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
            if (name[0]=='T'
             && name[1]=='a'
             && name[2]=='g')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG];
            }
        } break;
        case 10: {
            if (name[0]=='W'
             && name[1]=='r'
             && name[2]=='a'
             && name[3]=='p'
             && name[4]=='C'
             && name[5]=='o'
             && name[6]=='l'
             && name[7]=='u'
             && name[8]=='m'
             && name[9]=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN];
            }
        } break;
        case 13: {
            if (name[0]=='E'
             && name[1]=='n'
             && name[2]=='c'
             && name[3]=='o'
             && name[4]=='d'
             && name[5]=='i'
             && name[6]=='n'
             && name[7]=='g'
             && name[8]=='S'
             && name[9]=='t'
             && name[10]=='y'
             && name[11]=='l'
             && name[12]=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE];
            }
        } break;
        case 14: {
            switch(name[0]) {
                case 'F': {
                    if (name[1]=='o'
                     && name[2]=='r'
                     && name[3]=='m'
                     && name[4]=='a'
                     && name[5]=='t'
                     && name[6]=='t'
                     && name[7]=='i'
                     && name[8]=='n'
                     && name[9]=='g'
                     && name[10]=='M'
                     && name[11]=='o'
                     && name[12]=='d'
                     && name[13]=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
                    }
                } break;
                case 'O': {
                    if (name[1]=='u'
                     && name[2]=='t'
                     && name[3]=='p'
                     && name[4]=='u'
                     && name[5]=='t'
                     && name[6]=='X'
                     && name[7]=='S'
                     && name[8]=='I'
                     && name[9]=='A'
                     && name[10]=='l'
                     && name[11]=='i'
                     && name[12]=='a'
                     && name[13]=='s')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS];
                    }
                } break;
                case 'S': {
                    switch(name[1]) {
                        case 'c': {
                            if (name[2]=='h'
                             && name[3]=='e'
                             && name[4]=='m'
                             && name[5]=='a'
                             && name[6]=='L'
                             && name[7]=='o'
                             && name[8]=='c'
                             && name[9]=='a'
                             && name[10]=='t'
                             && name[11]=='i'
                             && name[12]=='o'
                             && name[13]=='n')
                            {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION];
                            }
                        } break;
                        case 'p': {
                            if (name[2]=='a'
                             && name[3]=='c'
                             && name[4]=='e'
                             && name[5]=='s'
                             && name[6]=='P'
                             && name[7]=='e'
                             && name[8]=='r'
                             && name[9]=='L'
                             && name[10]=='e'
                             && name[11]=='v'
                             && name[12]=='e'
                             && name[13]=='l')
                            {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL];
                            }
                        } break;
                    }
                } break;
            }
        } break;
        case 15: {
            if (name[0]=='O')
            {
                switch(name[1]) {
                    case 'b': {
                        if (name[2]=='j'
                         && name[3]=='e'
                         && name[4]=='c'
                         && name[5]=='t'
                         && name[6]=='N'
                         && name[7]=='a'
                         && name[8]=='m'
                         && name[9]=='e'
                         && name[10]=='s'
                         && name[11]=='p'
                         && name[12]=='a'
                         && name[13]=='c'
                         && name[14]=='e')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE];
                        }
                    } break;
                    case 'u': {
                        if (name[2]=='t'
                         && name[3]=='p'
                         && name[4]=='u'
                         && name[5]=='t'
                         && name[6]=='X'
                         && name[7]=='M'
                         && name[8]=='L'
                         && name[9]=='H'
                         && name[10]=='e'
                         && name[11]=='a'
                         && name[12]=='d'
                         && name[13]=='e'
                         && name[14]=='r')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER];
                        }
                    } break;
                }
            }
        } break;
        case 18: {
            if (name[0]=='I'
             && name[1]=='n'
             && name[2]=='i'
             && name[3]=='t'
             && name[4]=='i'
             && name[5]=='a'
             && name[6]=='l'
             && name[7]=='I'
             && name[8]=='n'
             && name[9]=='d'
             && name[10]=='e'
             && name[11]=='n'
             && name[12]=='t'
             && name[13]=='L'
             && name[14]=='e'
             && name[15]=='v'
             && name[16]=='e'
             && name[17]=='l')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL];
            }
        } break;
        case 21: {
            if (name[0]=='M'
             && name[1]=='a'
             && name[2]=='x'
             && name[3]=='D'
             && name[4]=='e'
             && name[5]=='c'
             && name[6]=='i'
             && name[7]=='m'
             && name[8]=='a'
             && name[9]=='l'
             && name[10]=='T'
             && name[11]=='o'
             && name[12]=='t'
             && name[13]=='a'
             && name[14]=='l'
             && name[15]=='D'
             && name[16]=='i'
             && name[17]=='g'
             && name[18]=='i'
             && name[19]=='t'
             && name[20]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS];
            }
        } break;
        case 22: {
            if (name[0]=='A'
             && name[1]=='l'
             && name[2]=='l'
             && name[3]=='o'
             && name[4]=='w'
             && name[5]=='C'
             && name[6]=='o'
             && name[7]=='n'
             && name[8]=='t'
             && name[9]=='r'
             && name[10]=='o'
             && name[11]=='l'
             && name[12]=='C'
             && name[13]=='h'
             && name[14]=='a'
             && name[15]=='r'
             && name[16]=='a'
             && name[17]=='c'
             && name[18]=='t'
             && name[19]=='e'
             && name[20]=='r'
             && name[21]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS];
            }
        } break;
        case 23: {
            if (name[0]=='S'
             && name[1]=='i'
             && name[2]=='g'
             && name[3]=='n'
             && name[4]=='i'
             && name[5]=='f'
             && name[6]=='i'
             && name[7]=='c'
             && name[8]=='a'
             && name[9]=='n'
             && name[10]=='t'
             && name[11]=='D'
             && name[12]=='o'
             && name[13]=='u'
             && name[14]=='b'
             && name[15]=='l'
             && name[16]=='e'
             && name[17]=='D'
             && name[18]=='i'
             && name[19]=='g'
             && name[20]=='i'
             && name[21]=='t'
             && name[22]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS];
            }
        } break;
        case 24: {
            if (name[0]=='M'
             && name[1]=='a'
             && name[2]=='x'
             && name[3]=='D'
             && name[4]=='e'
             && name[5]=='c'
             && name[6]=='i'
             && name[7]=='m'
             && name[8]=='a'
             && name[9]=='l'
             && name[10]=='F'
             && name[11]=='r'
             && name[12]=='a'
             && name[13]=='c'
             && name[14]=='t'
             && name[15]=='i'
             && name[16]=='o'
             && name[17]=='n'
             && name[18]=='D'
             && name[19]=='i'
             && name[20]=='g'
             && name[21]=='i'
             && name[22]=='t'
             && name[23]=='s')
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
      case ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER];
      case ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS];
      default:
        return 0;
    }
}

// CREATORS

baexml_EncoderOptions::baexml_EncoderOptions(bslma_Allocator *basicAllocator)
: d_objectNamespace(basicAllocator)
, d_schemaLocation(basicAllocator)
, d_tag(basicAllocator)
, d_formattingMode(DEFAULT_INITIALIZER_FORMATTING_MODE)
, d_initialIndentLevel(DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL)
, d_spacesPerLevel(DEFAULT_INITIALIZER_SPACES_PER_LEVEL)
, d_wrapColumn(DEFAULT_INITIALIZER_WRAP_COLUMN)
, d_maxDecimalTotalDigits()
, d_maxDecimalFractionDigits()
, d_significantDoubleDigits()
, d_encodingStyle(DEFAULT_INITIALIZER_ENCODING_STYLE)
, d_allowControlCharacters(DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS)
, d_outputXMLHeader(DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER)
, d_outputXSIAlias(DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS)
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
, d_outputXMLHeader(original.d_outputXMLHeader)
, d_outputXSIAlias(original.d_outputXSIAlias)
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
        d_outputXMLHeader = rhs.d_outputXMLHeader;
        d_outputXSIAlias = rhs.d_outputXSIAlias;
    }
    return *this;
}

void baexml_EncoderOptions::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_objectNamespace);
    bdeat_ValueTypeFunctions::reset(&d_schemaLocation);
    bdeat_ValueTypeFunctions::reset(&d_tag);
    d_formattingMode = DEFAULT_INITIALIZER_FORMATTING_MODE;
    d_initialIndentLevel = DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;
    d_spacesPerLevel = DEFAULT_INITIALIZER_SPACES_PER_LEVEL;
    d_wrapColumn = DEFAULT_INITIALIZER_WRAP_COLUMN;
    bdeat_ValueTypeFunctions::reset(&d_maxDecimalTotalDigits);
    bdeat_ValueTypeFunctions::reset(&d_maxDecimalFractionDigits);
    bdeat_ValueTypeFunctions::reset(&d_significantDoubleDigits);
    d_encodingStyle = DEFAULT_INITIALIZER_ENCODING_STYLE;
    d_allowControlCharacters = DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS;
    d_outputXMLHeader = DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER;
    d_outputXSIAlias = DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS;
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

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "OutputXMLHeader = ";
        bdeu_PrintMethods::print(stream, d_outputXMLHeader,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "OutputXSIAlias = ";
        bdeu_PrintMethods::print(stream, d_outputXSIAlias,
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

        stream << ' ';
        stream << "OutputXMLHeader = ";
        bdeu_PrintMethods::print(stream, d_outputXMLHeader,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "OutputXSIAlias = ";
        bdeu_PrintMethods::print(stream, d_outputXSIAlias,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.6.9 Fri Jan 20 14:14:54 2012
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
