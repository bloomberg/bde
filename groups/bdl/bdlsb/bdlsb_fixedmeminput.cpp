// bdlsb_fixedmeminput.cpp                                            -*-C++-*-
#include <bdlsb_fixedmeminput.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_fixedmeminput_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlsb {
                        // -------------------
                        // class FixedMemInput
                        // -------------------

// MANIPULATORS
FixedMemInput::pos_type
FixedMemInput::pubseekpos(pos_type                position,
                          bsl::ios_base::openmode which)
{
    // This is an input buffer only, so cannot "seek" in "put" area.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(which & bsl::ios_base::in))) {
        return pos_type(-1);                                          // RETURN
    }

    bsl::size_t finalPosition = static_cast<bsl::size_t>(position);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(finalPosition > d_bufferSize)
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(off_type(position) < 0)) {
        return pos_type(-1);                                          // RETURN
    }

    d_pos = finalPosition;
    return position;
}

FixedMemInput::pos_type
FixedMemInput::pubseekoff(off_type                offset,
                          bsl::ios_base::seekdir  way,
                          bsl::ios_base::openmode which)
{
    const char *basePtr = d_buffer_p;

    switch (way) {
      case bsl::ios_base::beg:
        break;
      case bsl::ios_base::cur:
        basePtr += d_pos;
        break;
      case bsl::ios_base::end:
        basePtr += d_bufferSize;
        break;
      default:
        return pos_type(-1);                                          // RETURN
    }

    return pubseekpos(pos_type((basePtr - d_buffer_p) + offset), which);
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
