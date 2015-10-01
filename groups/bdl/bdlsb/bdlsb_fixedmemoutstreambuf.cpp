// bdlsb_fixedmemoutstreambuf.cpp                                     -*-C++-*-
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_fixedmemoutstreambuf_cpp,"$Id$ $CSID$")

#include <bsls_types.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlsb {

                           // --------------------------
                           // class FixedMemOutStreamBuf
                           // --------------------------

// PROTECTED MANIPULATORS
FixedMemOutStreamBuf::pos_type
FixedMemOutStreamBuf::seekoff(off_type                offset,
                              bsl::ios_base::seekdir  fixedPosition,
                              bsl::ios_base::openmode which)
{
    // This is an output buffer only, so cannot "seek" in "get" area.
    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);                                          // RETURN
    }

    // Compute offset from current position.  In this stream, pptr() defines
    // both the current position and the end of the logical byte stream.
    // Thus, bsl::ios_base::curr and bsl::ios_base::end are handled
    // identically.
    off_type currOffset = (bsl::ios_base::beg == fixedPosition ?
                           offset - length() : offset);

    // currOffset validity check
    if (currOffset > 0 || -currOffset > length()) {
        return pos_type(-1);                                          // RETURN
    }

    pbump(static_cast<int>(currOffset));

    return pos_type(length());
}

FixedMemOutStreamBuf::pos_type
FixedMemOutStreamBuf::seekpos(pos_type                position,
                              bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
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
