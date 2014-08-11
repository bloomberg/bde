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
    const int           len   = object.length();
    const char         *data  = object.data();
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
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
