// bdlma_memoryblockdescriptor.cpp                                    -*-C++-*-
#include <bdlma_memoryblockdescriptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_memoryblockdescriptor_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <cstdio>                          // for 'std::printf'

namespace BloombergLP {
namespace bdlma {

                       // ---------------------------
                       // class MemoryBlockDescriptor
                       // ---------------------------

// ACCESSORS
void MemoryBlockDescriptor::print() const
{
/// Unfortunately, %zu is not correctly supported across all platforms.
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

    std::printf("[%p, " ZU "]", address(), size());
#undef ZU
}

}  // close package namespace
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
