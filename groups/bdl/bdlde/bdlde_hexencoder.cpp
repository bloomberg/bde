// bdlde_hexencoder.cpp                                               -*-C++-*-
#include <bdlde_hexencoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_hexencoder_cpp,"$Id$ $CSID$")

namespace {
namespace u {
                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

static const char lowercaseLettersTable[16] =
{
   '0' , //   0
   '1' , //   1
   '2' , //   2
   '3' , //   3
   '4' , //   4
   '5' , //   5
   '6' , //   6
   '7' , //   7
   '8' , //   8
   '9' , //   9
   'a' , //  10
   'b' , //  11
   'c' , //  12
   'd' , //  13
   'e' , //  14
   'f' , //  15
};

static const char uppercaseLettersTable[16] =
{
   '0' , //   0
   '1' , //   1
   '2' , //   2
   '3' , //   3
   '4' , //   4
   '5' , //   5
   '6' , //   6
   '7' , //   7
   '8' , //   8
   '9' , //   9
   'A' , //  10
   'B' , //  11
   'C' , //  12
   'D' , //  13
   'E' , //  14
   'F' , //  15
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlde {

                           // ----------
                           // HexEncoder
                           // ----------

// CREATORS
HexEncoder::HexEncoder(bool upperCaseLetters)
: d_state(e_INPUT_STATE)
, d_deferred(0)
, d_outputLength(0)
, d_upperCaseFlag(upperCaseLetters)
, d_encodeTable_p(upperCaseLetters ? u::uppercaseLettersTable
                                   : u::lowercaseLettersTable)
{
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
