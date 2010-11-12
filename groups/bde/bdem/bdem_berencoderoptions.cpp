// bdem_berencoderoptions.cpp   -*-C++-*-
#include <bdem_berencoderoptions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berencoderoptions_cpp,"$Id$ $CSID$")

#include <bdeat_formattingmode.h>

#include <bsls_assert.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                     // ----------------------------------
                     // class bdem_BerEncoderOptions
                     // ----------------------------------

// CONSTANTS

const char bdem_BerEncoderOptions::CLASS_NAME[] = "bdem_BerEncoderOptions";
    // the name of this class
const int bdem_BerEncoderOptions::DEFAULT_TRACE_LEVEL = 0;
    // default value of 'TraceLevel' attribute
const int bdem_BerEncoderOptions::DEFAULT_BDE_VERSION_CONFORMANCE = 10500;
    // default value of 'BdeVersionConformance' attribute

const bdeat_AttributeInfo bdem_BerEncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",             // name
        sizeof("TraceLevel") - 1, // name length
        "trace (verbosity) level",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE,
        "BdeVersionConformance",             // name
        sizeof("BdeVersionConformance") - 1, // name length
        "The largest BDE version that can be assumed of the corresponding "
        "decoder for the encoded message, expressed as 10000*majorVersion "
        "+ 100*minorVersion + patchVersion (e.g. 1.5.0 is expressed as 105"
        "00). Ideally, the BER encoder should be permitted to generate any"
        " BER that conforms to X.690 (Basic Encoding Rules) and X.694 (map"
        "ping of XSD to ASN.1). In practice, however, certain unimplemente"
        "d features and missunderstandings of these standards have resulte"
        "d in a decoder that cannot accept the full range of legal inputs."
        " Even when the encoder and decoder are both upgraded to a richer "
        "subset of BER, the program receiving the encoded values may not h"
        "ave been recompiled with the latest version and, thus restricting"
        " the encoder to emit BER that can be understood by the decoder at"
        " the other end of the wire. If it is that the receiver has a more"
        " modern decoder, set this variable to a larger value to allow the"
        " encoder to produce BER that is richer and more standards conform"
        "ant. The default should be increased only when old copies of the "
        "decoder are completely out of circulation.",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *bdem_BerEncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='T'
             && bdeu_CharType::toUpper(name[1])=='R'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='C'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='L'
             && bdeu_CharType::toUpper(name[6])=='E'
             && bdeu_CharType::toUpper(name[7])=='V'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='L')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
            }
        } break;
        case 21: {
            if (bdeu_CharType::toUpper(name[0])=='B'
             && bdeu_CharType::toUpper(name[1])=='D'
             && bdeu_CharType::toUpper(name[2])=='E'
             && bdeu_CharType::toUpper(name[3])=='V'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='R'
             && bdeu_CharType::toUpper(name[6])=='S'
             && bdeu_CharType::toUpper(name[7])=='I'
             && bdeu_CharType::toUpper(name[8])=='O'
             && bdeu_CharType::toUpper(name[9])=='N'
             && bdeu_CharType::toUpper(name[10])=='C'
             && bdeu_CharType::toUpper(name[11])=='O'
             && bdeu_CharType::toUpper(name[12])=='N'
             && bdeu_CharType::toUpper(name[13])=='F'
             && bdeu_CharType::toUpper(name[14])=='O'
             && bdeu_CharType::toUpper(name[15])=='R'
             && bdeu_CharType::toUpper(name[16])=='M'
             && bdeu_CharType::toUpper(name[17])=='A'
             && bdeu_CharType::toUpper(name[18])=='N'
             && bdeu_CharType::toUpper(name[19])=='C'
             && bdeu_CharType::toUpper(name[20])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                 ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
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
        return
           &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
      default:
        return 0;
    }
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

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace BloombergLP;

// GENERATED BY BLP_BAS_CODEGEN_2.1.6 Thu Jun  7 12:17:03 2007
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
