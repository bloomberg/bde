// btlsc_flag.cpp                                                     -*-C++-*-
#include <btlsc_flag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsc_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *btesc_Flag::toAscii(Flag value)
{
#define CASE(X) case(k_ ## X): return #X

    switch (value) {
      CASE(ASYNC_INTERRUPT);
      CASE(RAW);
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

bsl::ostream& operator<<(bsl::ostream& stream, btesc_Flag::Flag rhs)
{
    return stream << btesc_Flag::toAscii(rhs);
}

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
