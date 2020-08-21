// bdld_manageddatum.cpp                                              -*-C++-*-
#include <bdld_manageddatum.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_manageddatum_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bslmf_istriviallycopyable.h>

namespace BloombergLP {
namespace bdld {

                            // ------------------
                            // class ManagedDatum
                            // ------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<ManagedDatum>::value);
BSLMF_ASSERT(bslmf::IsBitwiseMoveable<ManagedDatum>::value);
BSLMF_ASSERT(!bsl::is_trivially_copyable<ManagedDatum>::value);

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
