// bslx_byteinstream.cpp                                              -*-C++-*-
#include <bslx_byteinstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_byteinstream_cpp,"$Id$ $CSID$")

#include <bslx_byteoutstream.h>                 // for testing only

#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

                        // ------------------
                        // class ByteInStream
                        // ------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const ByteInStream& object)
{
    const bsl::size_t   len   = object.length();
    const char         *data  = object.data();
    bsl::ios::fmtflags  flags = stream.flags();

    stream << bsl::hex;

    for (bsl::size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) { // output newline character and address every 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        stream << bsl::setw(2)
               << bsl::setfill('0')
               << static_cast<int>(static_cast<unsigned char>(data[i]));
    }

    stream.flags(flags);  // reset stream format flags

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
