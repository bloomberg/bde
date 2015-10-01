// bdlsb_fixedmeminstreambuf.cpp                                      -*-C++-*-
#include <bdlsb_fixedmeminstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_fixedmeminstreambuf_cpp,"$Id$ $CSID$")

#include <bsls_types.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {
namespace bdlsb {

// PROTECTED MANIPULATORS
FixedMemInStreamBuf::pos_type
FixedMemInStreamBuf::seekoff(off_type                offset,
                             bsl::ios_base::seekdir  way,
                             bsl::ios_base::openmode which)
{
    // This is an input buffer only, so cannot "seek" in "put" area.
    if (!(which & bsl::ios_base::in)) {
        return pos_type(-1);                                          // RETURN
    }

    char_type *basePtr;
    switch (way) {
        case bsl::ios_base::beg: {
            basePtr = eback();
        } break;
        case bsl::ios_base::cur: {
            basePtr = gptr();
        } break;
        case bsl::ios_base::end: {
            basePtr = egptr();
        } break;
        default:
            return pos_type(-1);                                      // RETURN
    }

    pos_type newPos = static_cast<pos_type> (basePtr - eback() + offset);

    if (newPos < 0 || static_cast<bsl::size_t>(newPos) > d_bufferSize ) {
        return pos_type(-1);                                          // RETURN
    }

    setg(d_buffer_p, d_buffer_p + newPos, d_buffer_p + d_bufferSize);

    return newPos;
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
