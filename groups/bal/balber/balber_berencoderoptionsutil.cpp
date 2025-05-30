// balber_berencoderoptionsutil.cpp                                   -*-C++-*-
#include <balber_berencoderoptionsutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berencoderoptionsutil_cpp,"$Id$ $CSID$")

#include <balber_berencoderoptions.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace balber {

                            // ---------------------------
                            // class BerEncoderOptionsUtil
                            // ---------------------------

void BerEncoderOptionsUtil::setMode(BerEncoderOptions *options, Mode mode)
{
    BSLS_ASSERT(options);

    switch (mode) {
      case e_DEFAULT: {
        *options = BerEncoderOptions();
      } break;
      case e_FAST_20250615: {
        *options = BerEncoderOptions();
        options->setEncodeDateAndTimeTypesAsBinary(true);
        options->setEncodeArrayLengthHints(true);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 == "reachable");
      } break;
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
