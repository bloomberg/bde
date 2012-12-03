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
    enum { BAEJSN_MIN_ENUM_STRING_LENGTH = 2 };

    if (baejsn_Reader::BAEJSN_VALUE == d_reader.tokenType()) {
        bslstl::StringRef dataValue;
        int rc = d_reader.value(&dataValue);
        if (rc
         || dataValue.length() <= BAEJSN_MIN_ENUM_STRING_LENGTH
         || '"' != dataValue[0]
         || '"' != dataValue[dataValue.length() - 1]) {
            return -1;
        }

        dataValue.assign(dataValue.begin() + 1, dataValue.end() - 1);

        bdede_Base64Decoder base64Decoder(true);
        bsl::back_insert_iterator<bsl::vector<char> > outputIterator(*value);

        rc = base64Decoder.convert(outputIterator,
                                   dataValue.begin(),
                                   dataValue.end());

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
