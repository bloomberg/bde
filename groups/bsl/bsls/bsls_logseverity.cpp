// bsls_logseverity.cpp                                               -*-C++-*-

#include <bsls_logseverity.h>
#include <bsls_bsltestutil.h>      // for testing only

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_logseverity_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bsls {

                     // ------------------
                     // struct LogSeverity
                     // ------------------

// CLASS METHODS
const char *LogSeverity::toAscii(LogSeverity::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(FATAL)
      CASE(ERROR)
      CASE(WARN)
      CASE(DEBUG)
      CASE(INFO)
      CASE(TRACE)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
