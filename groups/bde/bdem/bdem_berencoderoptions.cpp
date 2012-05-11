// bdem_berencoderoptions.cpp-- GENERATED FILE - DO NOT EDIT ---*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berencoderoptions_cpp,"$Id$ $CSID$")

#include <bdem_berencoderoptions.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>


#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {

                     // ----------------------------------                     
                     // class bdem_BerEncoderOptions                     
                     // ----------------------------------                     

// CONSTANTS

const char bdem_BerEncoderOptions::CLASS_NAME[] = "bdem_BerEncoderOptions";

const int bdem_BerEncoderOptions::DEFAULT_INITIALIZER_TRACE_LEVEL = 0;

const int bdem_BerEncoderOptions::DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE = 10500;

const bool bdem_BerEncoderOptions::DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS = true;

const bool bdem_BerEncoderOptions::DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = false;

const bdeat_AttributeInfo bdem_BerEncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",
        sizeof("TraceLevel") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE,
        "BdeVersionConformance",
        sizeof("BdeVersionConformance") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS,
        "EncodeEmptyArrays",
        sizeof("EncodeEmptyArrays") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY,
        "EncodeDateAndTimeTypesAsBinary",
        sizeof("EncodeDateAndTimeTypesAsBinary") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *bdem_BerEncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='T'
             && name[1]=='r'
             && name[2]=='a'
             && name[3]=='c'
             && name[4]=='e'
             && name[5]=='L'
             && name[6]=='e'
             && name[7]=='v'
             && name[8]=='e'
             && name[9]=='l')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
            }
        } break;
        case 17: {
            if (name[0]=='E'
             && name[1]=='n'
             && name[2]=='c'
             && name[3]=='o'
             && name[4]=='d'
             && name[5]=='e'
             && name[6]=='E'
             && name[7]=='m'
             && name[8]=='p'
             && name[9]=='t'
             && name[10]=='y'
             && name[11]=='A'
             && name[12]=='r'
             && name[13]=='r'
             && name[14]=='a'
             && name[15]=='y'
             && name[16]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
            }
        } break;
        case 21: {
            if (name[0]=='B'
             && name[1]=='d'
             && name[2]=='e'
             && name[3]=='V'
             && name[4]=='e'
             && name[5]=='r'
             && name[6]=='s'
             && name[7]=='i'
             && name[8]=='o'
             && name[9]=='n'
             && name[10]=='C'
             && name[11]=='o'
             && name[12]=='n'
             && name[13]=='f'
             && name[14]=='o'
             && name[15]=='r'
             && name[16]=='m'
             && name[17]=='a'
             && name[18]=='n'
             && name[19]=='c'
             && name[20]=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
            }
        } break;
        case 30: {
            if (name[0]=='E'
             && name[1]=='n'
             && name[2]=='c'
             && name[3]=='o'
             && name[4]=='d'
             && name[5]=='e'
             && name[6]=='D'
             && name[7]=='a'
             && name[8]=='t'
             && name[9]=='e'
             && name[10]=='A'
             && name[11]=='n'
             && name[12]=='d'
             && name[13]=='T'
             && name[14]=='i'
             && name[15]=='m'
             && name[16]=='e'
             && name[17]=='T'
             && name[18]=='y'
             && name[19]=='p'
             && name[20]=='e'
             && name[21]=='s'
             && name[22]=='A'
             && name[23]=='s'
             && name[24]=='B'
             && name[25]=='i'
             && name[26]=='n'
             && name[27]=='a'
             && name[28]=='r'
             && name[29]=='y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *bdem_BerEncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
      case ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY];
      default:
        return 0;
    }
}

// CREATORS

bdem_BerEncoderOptions::bdem_BerEncoderOptions()
: d_traceLevel(DEFAULT_INITIALIZER_TRACE_LEVEL)
, d_bdeVersionConformance(DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE)
, d_encodeEmptyArrays(DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS)
, d_encodeDateAndTimeTypesAsBinary(DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY)
{
}

bdem_BerEncoderOptions::bdem_BerEncoderOptions(const bdem_BerEncoderOptions& original)
: d_traceLevel(original.d_traceLevel)
, d_bdeVersionConformance(original.d_bdeVersionConformance)
, d_encodeEmptyArrays(original.d_encodeEmptyArrays)
, d_encodeDateAndTimeTypesAsBinary(original.d_encodeDateAndTimeTypesAsBinary)
{
}

bdem_BerEncoderOptions::~bdem_BerEncoderOptions()
{
}

// MANIPULATORS

bdem_BerEncoderOptions&
bdem_BerEncoderOptions::operator=(const bdem_BerEncoderOptions& rhs)
{
    if (this != &rhs) {
        d_traceLevel = rhs.d_traceLevel;
        d_bdeVersionConformance = rhs.d_bdeVersionConformance;
        d_encodeEmptyArrays = rhs.d_encodeEmptyArrays;
        d_encodeDateAndTimeTypesAsBinary = rhs.d_encodeDateAndTimeTypesAsBinary;
    }
    return *this;
}

void bdem_BerEncoderOptions::reset()
{
    d_traceLevel = DEFAULT_INITIALIZER_TRACE_LEVEL;
    d_bdeVersionConformance = DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE;
    d_encodeEmptyArrays = DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;
    d_encodeDateAndTimeTypesAsBinary = DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY;
}

// ACCESSORS

bsl::ostream& bdem_BerEncoderOptions::print(
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
        stream << "TraceLevel = ";
        bdeu_PrintMethods::print(stream, d_traceLevel,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BdeVersionConformance = ";
        bdeu_PrintMethods::print(stream, d_bdeVersionConformance,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "EncodeEmptyArrays = ";
        bdeu_PrintMethods::print(stream, d_encodeEmptyArrays,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "EncodeDateAndTimeTypesAsBinary = ";
        bdeu_PrintMethods::print(stream, d_encodeDateAndTimeTypesAsBinary,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "TraceLevel = ";
        bdeu_PrintMethods::print(stream, d_traceLevel,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "BdeVersionConformance = ";
        bdeu_PrintMethods::print(stream, d_bdeVersionConformance,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "EncodeEmptyArrays = ";
        bdeu_PrintMethods::print(stream, d_encodeEmptyArrays,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "EncodeDateAndTimeTypesAsBinary = ";
        bdeu_PrintMethods::print(stream, d_encodeDateAndTimeTypesAsBinary,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.6.9 Tue Feb 21 16:21:00 2012
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
