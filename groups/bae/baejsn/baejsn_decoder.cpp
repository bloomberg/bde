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
    d_reader.advanceToNextToken();
    if (baejsn_Reader::BAEJSN_VALUE == d_reader.tokenType()) {
        bslstl::StringRef dataValue = d_reader.value();

        baexml_Base64Parser<bsl::vector<char> > base64Parser;

        if (0 != base64Parser.beginParse(value)) {
            return -1;                                                // RETURN
        }

        if (0 != base64Parser.pushCharacters(dataValue.begin(),
                                             dataValue.end())) {
            return -1;                                                // RETURN
        }

        return base64Parser.endParse();                               // RETURN
    }
    return -1;
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
