// bslma_memoryresourceimpsupport.cpp                                 -*-C++-*-

#include <bslma_memoryresourceimpsupport.h>

#include <bslma_memoryresource.h> // for testing only

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslma_memoryresourceimpsupport_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace bslma {

// CLASS DATA

// The `d_data` member of the singular object contains a bit pattern that is
// easily recognized as *bad* data in the debugger.
const MemoryResourceImpSupport_AlignedData
MemoryResourceImpSupport::s_singularObject = {
    0xDeadBeefCafeF00dULL,
    bsls::AlignmentUtil::MaxAlignedType()
};

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
