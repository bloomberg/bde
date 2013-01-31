// baejsn_encoder.cpp                                                 -*-C++-*-
#include <baejsn_encoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_encoder_cpp,"$Id$ $CSID$")

#include <bdede_base64encoder.h>

namespace BloombergLP {

                            // -------------------------------
                            // class baejsn_Encoder_EncodeImpl
                            // -------------------------------

// PRIVATE MANIPULATORS
void baejsn_Encoder_EncodeImpl::indentTopElement()
{
    if (baejsn_EncoderOptions::BAEJSN_PRETTY == d_encodingStyle) {
        int numSpaces = d_indentLevel * d_spacesPerLevel;

        const char SPACES[]    = "                                    ";
        const int  SPACES_SIZE = sizeof(SPACES) - 1;

        while (SPACES_SIZE < numSpaces) {
            d_outputStream.write(SPACES, SPACES_SIZE);
            numSpaces -= SPACES_SIZE;
        }

        if (0 < numSpaces) {
            d_outputStream.write(SPACES, numSpaces);
        }
    }
}

int baejsn_Encoder_EncodeImpl::encodeImp(const bsl::vector<char>& value,
                                         bdeat_TypeCategory::Array)
{
    bsl::string base64String;
    bdede_Base64Encoder encoder(0);
    base64String.resize(
        bdede_Base64Encoder::encodedLength(static_cast<int>(value.size()), 0));

    // Ensure length is a multiple of 4.

    BSLS_ASSERT(0 == (base64String.length() & 0x03));

    int numOut;
    int numIn;
    int rc = encoder.convert(base64String.begin(),
                             &numOut,
                             &numIn,
                             value.begin(),
                             value.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = encoder.endConvert(base64String.begin() + numOut);
    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    return encode(base64String);
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
