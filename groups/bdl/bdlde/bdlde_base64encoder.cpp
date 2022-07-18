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

namespace {
namespace u {

                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

// The following table is a map of a 6-bit index value to the corresponding
// Base64 encoding of that index.

const char base64[] = {
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

// The following table is a map of a 6-bit index value to the corresponding
// Base64 URL and Filename Safe encoding of that index.

const char base64url[] = {
//   0    1    2    3    4    5    6    7
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  // 000
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',  // 010
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',  // 020
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',  // 030
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',  // 040
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 050
    'w', 'x', 'y', 'z', '0', '1', '2', '3',  // 060
    '4', '5', '6', '7', '8', '9', '-', '_',  // 070
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlde {

                         // -------------------
                         // class Base64Encoder
                         // -------------------

// CREATORS
Base64Encoder::Base64Encoder(const EncoderOptions& options)
: d_maxLineLength(options.maxLineLength())
, d_lineLength(0)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
, d_alphabet_p(e_BASIC == options.alphabet() ? u::base64 : u::base64url)
, d_state(e_INITIAL_STATE)
, d_alphabet(options.alphabet())
, d_isPadded(options.isPadded())
{}

Base64Encoder::Base64Encoder(Alphabet alphabet)
: d_maxLineLength(EncoderOptions::k_MIME_MAX_LINE_LENGTH)
, d_lineLength(0)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
, d_alphabet_p(e_BASIC == alphabet ? u::base64 : u::base64url)
, d_state(e_INITIAL_STATE)
, d_alphabet(alphabet)
, d_isPadded(true)
{
    BSLS_ASSERT(e_BASIC == alphabet || e_URL == alphabet);
}

Base64Encoder::Base64Encoder(int maxLineLength, Alphabet alphabet)
: d_maxLineLength(maxLineLength)
, d_lineLength(0)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
, d_alphabet_p(e_BASIC == alphabet ? u::base64 : u::base64url)
, d_state(e_INITIAL_STATE)
, d_alphabet(alphabet)
, d_isPadded(true)
{
    BSLS_ASSERT(0 <= maxLineLength);
    BSLS_ASSERT(e_BASIC == alphabet || e_URL == alphabet);
}

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
// Copyright 2018 Bloomberg Finance L.P.
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
