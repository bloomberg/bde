// baejsn_decoder.cpp                                                 -*-C++-*-
#include <baejsn_decoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoder_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace {

// BDEM_DATETIME
// BDEM_DATE
// BDEM_TIME
// BDEM_TABLE    // todo need this, its basically an array of sequences
// BDEM_DATETIMETZ
// BDEM_DATETZ
// BDEM_TIMETZ
// BDEM_ROW
// BDEM_CHOICE_ARRAY_ITEM

}

                   // --------------------
                   // class baejsn_Decoder
                   // --------------------

int baejsn_Decoder::decodeBinaryArray(std::vector<char> *value)
{
    // TBD: Review
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    bsl::string hexString;
    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &hexString)) {
        d_logStream << "Expected string containing base64";
        return 1;                                                     // RETURN
    }

    if (0 != (hexString.length() & 0x03)) {
        d_logStream << "Expected string containing base64, "
                       "but string length is not a multiple of 4";
        return 1;                                                     // RETURN
    }

    bdede_Base64Decoder decoder(true);
    value->resize((hexString.length() * 3 + 3) / 4);
    int numOut;
    int numIn;
    int rc = decoder.convert(value->begin(),
                             &numOut,
                             &numIn,
                             hexString.begin(),
                             hexString.end());
    value->resize(numOut);
    return rc;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
