// bsltf_copymovestate.cpp                                            -*-C++-*-
#include <bsltf_copymovestate.h>

#include <cstdio>  // for 'printf'

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bsltf {

                     // --------------------
                     // struct CopyMoveState
                     // --------------------

// CLASS METHODS
const char *CopyMoveState::toAscii(CopyMoveState::Enum value)
{
#define CASE(X) case int(e_ ## X): return #X
#define MOVED_FROM_CASE(X) case e_ ## X | e_MOVED_FROM: \
    return #X ", MOVED_FROM"

    switch (int(value)) {
      CASE(ORIGINAL);
      CASE(COPIED_INTO);
      CASE(COPIED_CONST_INTO);
      CASE(COPIED_NONCONST_INTO);
      CASE(MOVED_INTO);
      CASE(MOVED_FROM);
      MOVED_FROM_CASE(COPIED_INTO);
      MOVED_FROM_CASE(COPIED_CONST_INTO);
      MOVED_FROM_CASE(COPIED_NONCONST_INTO);
      MOVED_FROM_CASE(MOVED_INTO);
      CASE(UNKNOWN);
      default: return "(* INVALID *)";
    }

#undef MOVED_FROM_CASE
#undef CASE
}

/// Print the specified `value` as a string.
void debugprint(const CopyMoveState::Enum& value)
{
    std::printf("%s", CopyMoveState::toAscii(value));
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
