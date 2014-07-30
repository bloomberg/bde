// bslx_typecode.cpp                                                  -*-C++-*-

#include <bslx_typecode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_typecode_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

                     // ---------------
                     // struct TypeCode
                     // ---------------

// CLASS METHODS
bsl::ostream& TypeCode::print(bsl::ostream&  stream,
                              TypeCode::Enum value,
                              int            level,
                              int            spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream << TypeCode::toAscii(value);
    printer.end(true);

    return stream << bsl::flush;
}

const char *TypeCode::toAscii(TypeCode::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(INT8)
      CASE(UINT8)
      CASE(INT16)
      CASE(UINT16)
      CASE(INT24)
      CASE(UINT24)
      CASE(INT32)
      CASE(UINT32)
      CASE(INT40)
      CASE(UINT40)
      CASE(INT48)
      CASE(UINT48)
      CASE(INT56)
      CASE(UINT56)
      CASE(INT64)
      CASE(UINT64)
      CASE(FLOAT32)
      CASE(FLOAT64)
      CASE(INVALID)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
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
