// baljsn_decoderoptionsutil.cpp                                      -*-C++-*-
#include <baljsn_decoderoptionsutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_printutil_cpp,"$Id$ $CSID$")

#include <baljsn_decoderoptions.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace baljsn {

                              // ------------------------
                              // class DecoderOptionsUtil
                              // ------------------------

void DecoderOptionsUtil::setMode(DecoderOptions *options, Mode mode)
{

    BSLS_ASSERT(options);

    switch (mode) {
      case e_DEFAULT: {
        *options = DecoderOptions();
      } break;
      case e_STRICT_20240423: {
        options->setValidateInputIsUtf8            (true );
        options->setAllowConsecutiveSeparators     (false);
        options->setAllowFormFeedAsWhitespace      (false);
        options->setAllowUnescapedControlCharacters(false);
      } break;
      default: {
        BSLS_ASSERT_OPT(!"reachable");
      } break;
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
