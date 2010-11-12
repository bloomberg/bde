// baenet_httpcontenttype.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpcontenttype_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpcontenttype.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                      // --------------------------------                      
                      // class baenet_HttpContentType                      
                      // --------------------------------                      

// CONSTANTS

const char baenet_HttpContentType::CLASS_NAME[] = "baenet_HttpContentType";
const bdeat_AttributeInfo baenet_HttpContentType::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_TYPE,
        "Type",
        sizeof("Type") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_SUB_TYPE,
        "SubType",
        sizeof("SubType") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CHARSET,
        "Charset",
        sizeof("Charset") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_BOUNDARY,
        "Boundary",
        sizeof("Boundary") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ID,
        "Id",
        sizeof("Id") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_NAME,
        "Name",
        sizeof("Name") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpContentType::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 2: {
            if ((name[0]|0x20)=='i'
             && (name[1]|0x20)=='d')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
            }
        } break;
        case 4: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'N': {
                    if ((name[1]|0x20)=='a'
                     && (name[2]|0x20)=='m'
                     && (name[3]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
                    }
                } break;
                case 'T': {
                    if ((name[1]|0x20)=='y'
                     && (name[2]|0x20)=='p'
                     && (name[3]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE];
                    }
                } break;
            }
        } break;
        case 7: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'C': {
                    if ((name[1]|0x20)=='h'
                     && (name[2]|0x20)=='a'
                     && (name[3]|0x20)=='r'
                     && (name[4]|0x20)=='s'
                     && (name[5]|0x20)=='e'
                     && (name[6]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET];
                    }
                } break;
                case 'S': {
                    if ((name[1]|0x20)=='u'
                     && (name[2]|0x20)=='b'
                     && (name[3]|0x20)=='t'
                     && (name[4]|0x20)=='y'
                     && (name[5]|0x20)=='p'
                     && (name[6]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE];
                    }
                } break;
            }
        } break;
        case 8: {
            if ((name[0]|0x20)=='b'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='u'
             && (name[3]|0x20)=='n'
             && (name[4]|0x20)=='d'
             && (name[5]|0x20)=='a'
             && (name[6]|0x20)=='r'
             && (name[7]|0x20)=='y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baenet_HttpContentType::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE];
      case ATTRIBUTE_ID_SUB_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE];
      case ATTRIBUTE_ID_CHARSET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET];
      case ATTRIBUTE_ID_BOUNDARY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY];
      case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpContentType::baenet_HttpContentType(bslma_Allocator *basicAllocator)
: d_type(basicAllocator)
, d_subType(basicAllocator)
, d_charset(basicAllocator)
, d_boundary(basicAllocator)
, d_id(basicAllocator)
, d_name(basicAllocator)
{
}

baenet_HttpContentType::baenet_HttpContentType(const baenet_HttpContentType& original,
                                                       bslma_Allocator *basicAllocator)
: d_type(original.d_type, basicAllocator)
, d_subType(original.d_subType, basicAllocator)
, d_charset(original.d_charset, basicAllocator)
, d_boundary(original.d_boundary, basicAllocator)
, d_id(original.d_id, basicAllocator)
, d_name(original.d_name, basicAllocator)
{
}

baenet_HttpContentType::~baenet_HttpContentType()
{
}

// MANIPULATORS

baenet_HttpContentType&
baenet_HttpContentType::operator=(const baenet_HttpContentType& rhs)
{
    if (this != &rhs) {
        d_type = rhs.d_type;
        d_subType = rhs.d_subType;
        d_charset = rhs.d_charset;
        d_boundary = rhs.d_boundary;
        d_id = rhs.d_id;
        d_name = rhs.d_name;
    }
    return *this;
}

void baenet_HttpContentType::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_type);
    bdeat_ValueTypeFunctions::reset(&d_subType);
    bdeat_ValueTypeFunctions::reset(&d_charset);
    bdeat_ValueTypeFunctions::reset(&d_boundary);
    bdeat_ValueTypeFunctions::reset(&d_id);
    bdeat_ValueTypeFunctions::reset(&d_name);
}

// ACCESSORS

bsl::ostream& baenet_HttpContentType::print(
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
        stream << "Type = ";
        bdeu_PrintMethods::print(stream, d_type,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SubType = ";
        bdeu_PrintMethods::print(stream, d_subType,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Charset = ";
        bdeu_PrintMethods::print(stream, d_charset,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Boundary = ";
        bdeu_PrintMethods::print(stream, d_boundary,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Id = ";
        bdeu_PrintMethods::print(stream, d_id,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Type = ";
        bdeu_PrintMethods::print(stream, d_type,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SubType = ";
        bdeu_PrintMethods::print(stream, d_subType,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Charset = ";
        bdeu_PrintMethods::print(stream, d_charset,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Boundary = ";
        bdeu_PrintMethods::print(stream, d_boundary,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Id = ";
        bdeu_PrintMethods::print(stream, d_id,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
