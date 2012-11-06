// baejsn_decoder.cpp                                                 -*-C++-*-
#include <baejsn_decoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoder_cpp,"$Id$ $CSID$")

#include <baexml_base64parser.h>

namespace BloombergLP {

                   // --------------------
                   // class baejsn_Decoder
                   // --------------------

int baejsn_Decoder::decodeBinaryArray(bsl::vector<char> *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    bsl::string base64String;
    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &base64String)) {
        d_logStream << "Expected string containing base64\n";
        return -1;                                                    // RETURN
    }

    baexml_Base64Parser<bsl::vector<char> > base64Parser;

    if (0 != base64Parser.beginParse(value)) {
        return -1;                                                    // RETURN
    }

    if (0 != base64Parser.pushCharacters(base64String.begin(),
                                         base64String.end())) {
        return -1;                                                    // RETURN
    }

    return base64Parser.endParse();
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
