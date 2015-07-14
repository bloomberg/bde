// bdlde_base64encoder.cpp                                            -*-C++-*-
#include <bdlde_base64encoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_base64encoder_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

// The following table is a map of a 6-bit index value to the corresponding
// Base64 encoding of that index.

static const char enc[] = {
//   0    1    2    3    4    5    6    7
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  // 000
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',  // 010
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',  // 020
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',  // 030
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',  // 040
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 050
    'w', 'x', 'y', 'z', '0', '1', '2', '3',  // 060
    '4', '5', '6', '7', '8', '9', '+', '/',  // 070
};

                        // -------------------------
                        // class bdlde::Base64Encoder
                        // -------------------------

const char *const bdlde::Base64Encoder::s_encodedChars_p = enc;

const int bdlde::Base64Encoder::s_defaultMaxLineLength = 76;

namespace bdlde {
// CREATORS

Base64Encoder::~Base64Encoder()
{
    // Assert invariants:

    BSLS_ASSERT(BDEDE_ERROR_STATE <= d_state);
    BSLS_ASSERT(d_state <= BDEDE_DONE_STATE);
    BSLS_ASSERT(0 <= d_maxLineLength);
    BSLS_ASSERT(0 <= d_outputLength);
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
