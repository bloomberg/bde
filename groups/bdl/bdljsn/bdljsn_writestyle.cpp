// bdljsn_writestyle.cpp                                              -*-C++-*-

#include <bdljsn_writestyle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_writestye_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdljsn {

                             // -----------------
                             // struct WriteStyle
                             // -----------------

// CLASS METHODS
bsl::ostream& WriteStyle::print(bsl::ostream&    stream,
                                WriteStyle::Enum value,
                                int              level,
                                int              spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << WriteStyle::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *WriteStyle::toAscii(WriteStyle::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -IND01
    switch (value) {
      CASE(PRETTY)
      CASE(ONELINE)
      CASE(COMPACT)
      default:
        return "(* UNKNOWN *)";
    }
// BDE_VERIFY pragma: pop

#undef CASE
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
