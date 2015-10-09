// bdlde_base64encoder.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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

                         // --------------------------
                         // class bdlde::Base64Encoder
                         // --------------------------

const char *const bdlde::Base64Encoder::s_encodedChars_p = enc;

const int bdlde::Base64Encoder::s_defaultMaxLineLength = 76;

namespace bdlde {

// CREATORS
Base64Encoder::~Base64Encoder()
{
    // Assert invariants:

    BSLS_ASSERT(e_ERROR_STATE <= d_state);
    BSLS_ASSERT(d_state <= e_DONE_STATE);
    BSLS_ASSERT(0 <= d_maxLineLength);
    BSLS_ASSERT(0 <= d_outputLength);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
