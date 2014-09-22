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
