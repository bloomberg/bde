// bslx_byteoutstream.cpp                                             -*-C++-*-
#include <bslx_byteoutstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_byteoutstream_cpp,"$Id$ $CSID$")

#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

                        // -------------------
                        // class ByteOutStream
                        // -------------------

// MANIPULATORS
                      // *** string values ***

ByteOutStream& ByteOutStream::putString(const bsl::string& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown and load into the buffer
    // the specified 'value'.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    if (value.size() > 127) {
        const int         length = static_cast<int>(value.size());
        const bsl::size_t nPlusLength =  n + MarshallingUtil::k_SIZEOF_INT32;
        d_buffer.resize(nPlusLength + length * MarshallingUtil::k_SIZEOF_INT8);
        validate();
        MarshallingUtil::putInt32(d_buffer.data() + n, length | (1 << 31));
        MarshallingUtil::putArrayInt8(d_buffer.data() + nPlusLength,
                                      reinterpret_cast<const unsigned char *>(
                                                                 value.data()),
                                      length);
    }
    else {
        const int         length = static_cast<int>(value.size());
        const bsl::size_t nPlusLength =  n + MarshallingUtil::k_SIZEOF_INT8;
        d_buffer.resize(nPlusLength + length * MarshallingUtil::k_SIZEOF_INT8);
        validate();
        MarshallingUtil::putInt8(d_buffer.data() + n, length);
        MarshallingUtil::putArrayInt8(d_buffer.data() + nPlusLength,
                                      reinterpret_cast<const unsigned char *>(
                                                                 value.data()),
                                      length);
    }

    return *this;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const ByteOutStream& object)
{
    const int           len   = static_cast<int>(object.d_buffer.size());
    const char         *data  = object.d_buffer.data();
    bsl::ios::fmtflags  flags = stream.flags();

    stream << bsl::hex;

    for (int i = 0; i < len; ++i) {
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
