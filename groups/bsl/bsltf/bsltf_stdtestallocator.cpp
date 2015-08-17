// bsltf_stdtestallocator.cpp                                         -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_newdeleteallocator.h>

#include <bsls_assert.h>

#include <limits.h>

namespace
{
// STATIC DATA
// This global static data is declared and defined entirely hidden inside
// the .cpp file, as the IBM compiler may create multiple copies if accessed
// through inline functions defined in the header.
    static ::BloombergLP::bslma::Allocator
                              *s_StdTestAllocatorConfiguration_allocator_p = 0;
}  // close unnamed namespace

namespace BloombergLP {
namespace bsltf {


                        // -----------------------------------
                        // class StdTestAllocatorConfiguration
                        // -----------------------------------

// CLASS METHODS
bslma::Allocator* StdTestAllocatorConfiguration::delegateAllocator()
{
    return s_StdTestAllocatorConfiguration_allocator_p
         ? s_StdTestAllocatorConfiguration_allocator_p
         : &bslma::NewDeleteAllocator::singleton();
}

void StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                              bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_OPT(basicAllocator);

    s_StdTestAllocatorConfiguration_allocator_p = basicAllocator;
}

                        // -----------------------
                        // struct StdTestAllocator
                        // -----------------------

unsigned int StdTestAllocator_CommonUtil::maxSize(size_t elementSize)
{
    return static_cast<unsigned int>(UINT_MAX  / elementSize);
}

                        // ----------------------
                        // class StdTestAllocator
                        // ----------------------


}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
