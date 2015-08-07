// bdlsb_fixedmeminput.cpp                                            -*-C++-*-

#include <bdlsb_fixedmeminput.h>

#include <bsls_performancehint.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlsb {

// MANIPULATORS
FixedMemInput::pos_type
FixedMemInput::pubseekpos(FixedMemInput::pos_type position,
                                bsl::ios_base::openmode       which)
{
    // This is an input buffer only, so cannot "seek" in "put" area.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(which & bsl::ios_base::in))) {
        return pos_type(-1);
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > d_bufferLength)
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(off_type(position) < 0)) {
        return pos_type(-1);
    }

    return d_pos = position;
}

FixedMemInput::pos_type
FixedMemInput::pubseekoff(FixedMemInput::off_type offset,
                                bsl::ios_base::seekdir        fixedPosition,
                                bsl::ios_base::openmode       which)
{
    const char *basePtr;

    basePtr = (fixedPosition == bsl::ios_base::cur) ? d_buffer_p +
                                                     static_cast<IntPtr>(d_pos)
            : (fixedPosition == bsl::ios_base::beg) ? d_buffer_p
                                                    : d_buffer_p +
                                                      d_bufferLength;

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
