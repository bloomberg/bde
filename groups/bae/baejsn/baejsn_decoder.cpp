// baejsn_decoder.cpp                                                 -*-C++-*-
#include <baejsn_decoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoder_cpp,"$Id$ $CSID$")

#include <bdede_base64decoder.h>

#include <bsl_iterator.h>

namespace BloombergLP {

                   // --------------------
                   // class baejsn_Decoder
                   // --------------------

int baejsn_Decoder::decodeBinaryArray(bsl::vector<char> *value)
{
    if (baejsn_Reader::BAEJSN_ELEMENT_VALUE == d_reader.tokenType()) {
        bslstl::StringRef dataValue;
        int rc = d_reader.value(&dataValue);

        // TBD: See if we can get rid of temp string

        bsl::string base64String;
        rc = baejsn_ParserUtil::getValue(&base64String, dataValue);

        bdede_Base64Decoder base64Decoder(true);
        bsl::back_insert_iterator<bsl::vector<char> > outputIterator(*value);

        rc = base64Decoder.convert(outputIterator,
                                   base64String.begin(),
                                   base64String.end());

        if (rc) {
            return -1;                                                // RETURN
        }

        rc = base64Decoder.endConvert(outputIterator);

        if (rc) {
            return -1;                                                // RETURN
        }
    }
    return 0;
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
