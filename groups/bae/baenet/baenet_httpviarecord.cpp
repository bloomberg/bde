// baenet_httpviarecord.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpviarecord_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpviarecord.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httphost.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                       // ------------------------------                       
                       // class baenet_HttpViaRecord                       
                       // ------------------------------                       

// CONSTANTS

const char baenet_HttpViaRecord::CLASS_NAME[] = "baenet_HttpViaRecord";
const bdeat_AttributeInfo baenet_HttpViaRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_PROTOCOL_NAME,
        "ProtocolName",
        sizeof("ProtocolName") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_PROTOCOL_VERSION,
        "ProtocolVersion",
        sizeof("ProtocolVersion") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_VIA_HOST,
        "ViaHost",
        sizeof("ViaHost") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_COMMENT,
        "Comment",
        sizeof("Comment") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpViaRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 7: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'C': {
                    if ((name[1]|0x20)=='o'
                     && (name[2]|0x20)=='m'
                     && (name[3]|0x20)=='m'
                     && (name[4]|0x20)=='e'
                     && (name[5]|0x20)=='n'
                     && (name[6]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMMENT];
                    }
                } break;
                case 'V': {
                    if ((name[1]|0x20)=='i'
                     && (name[2]|0x20)=='a'
                     && (name[3]|0x20)=='h'
                     && (name[4]|0x20)=='o'
                     && (name[5]|0x20)=='s'
                     && (name[6]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA_HOST];
                    }
                } break;
            }
        } break;
        case 12: {
            if ((name[0]|0x20)=='p'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='o'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='o'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='o'
             && (name[7]|0x20)=='l'
             && (name[8]|0x20)=='n'
             && (name[9]|0x20)=='a'
             && (name[10]|0x20)=='m'
             && (name[11]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROTOCOL_NAME];
            }
        } break;
        case 15: {
            if ((name[0]|0x20)=='p'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='o'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='o'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='o'
             && (name[7]|0x20)=='l'
             && (name[8]|0x20)=='v'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='r'
             && (name[11]|0x20)=='s'
             && (name[12]|0x20)=='i'
             && (name[13]|0x20)=='o'
             && (name[14]|0x20)=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROTOCOL_VERSION];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baenet_HttpViaRecord::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_PROTOCOL_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROTOCOL_NAME];
      case ATTRIBUTE_ID_PROTOCOL_VERSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROTOCOL_VERSION];
      case ATTRIBUTE_ID_VIA_HOST:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA_HOST];
      case ATTRIBUTE_ID_COMMENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMMENT];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpViaRecord::baenet_HttpViaRecord(bslma_Allocator *basicAllocator)
: d_protocolVersion(basicAllocator)
, d_protocolName(basicAllocator)
, d_comment(basicAllocator)
, d_viaHost(basicAllocator)
{
}

baenet_HttpViaRecord::baenet_HttpViaRecord(const baenet_HttpViaRecord& original,
                                                   bslma_Allocator *basicAllocator)
: d_protocolVersion(original.d_protocolVersion, basicAllocator)
, d_protocolName(original.d_protocolName, basicAllocator)
, d_comment(original.d_comment, basicAllocator)
, d_viaHost(original.d_viaHost, basicAllocator)
{
}

baenet_HttpViaRecord::~baenet_HttpViaRecord()
{
}

// MANIPULATORS

baenet_HttpViaRecord&
baenet_HttpViaRecord::operator=(const baenet_HttpViaRecord& rhs)
{
    if (this != &rhs) {
        d_protocolName = rhs.d_protocolName;
        d_protocolVersion = rhs.d_protocolVersion;
        d_viaHost = rhs.d_viaHost;
        d_comment = rhs.d_comment;
    }
    return *this;
}

void baenet_HttpViaRecord::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_protocolName);
    bdeat_ValueTypeFunctions::reset(&d_protocolVersion);
    bdeat_ValueTypeFunctions::reset(&d_viaHost);
    bdeat_ValueTypeFunctions::reset(&d_comment);
}

// ACCESSORS

bsl::ostream& baenet_HttpViaRecord::print(
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
        stream << "ProtocolName = ";
        bdeu_PrintMethods::print(stream, d_protocolName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ProtocolVersion = ";
        bdeu_PrintMethods::print(stream, d_protocolVersion,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ViaHost = ";
        bdeu_PrintMethods::print(stream, d_viaHost,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Comment = ";
        bdeu_PrintMethods::print(stream, d_comment,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "ProtocolName = ";
        bdeu_PrintMethods::print(stream, d_protocolName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ProtocolVersion = ";
        bdeu_PrintMethods::print(stream, d_protocolVersion,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ViaHost = ";
        bdeu_PrintMethods::print(stream, d_viaHost,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Comment = ";
        bdeu_PrintMethods::print(stream, d_comment,
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
