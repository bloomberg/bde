// btlb_pooledblobbufferfactory.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlb_pooledblobbufferfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlb_pooledblobbufferfactory_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace btlb {

                      // -----------------------------
                      // class PooledBlobBufferFactory
                      // -----------------------------

// CREATORS
PooledBlobBufferFactory::PooledBlobBufferFactory(
        int               bufferSize,
        bslma::Allocator *basicAllocator)
: d_bufferSize(bufferSize)
, d_spPool(basicAllocator)
{
}

PooledBlobBufferFactory::~PooledBlobBufferFactory()
{
}

// MANIPULATORS
void PooledBlobBufferFactory::allocate(BlobBuffer *buffer)
{
    buffer->reset(bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                                               d_bufferSize, &d_spPool),
                  d_bufferSize);
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
