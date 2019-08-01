// bdlbb_simpleblobbufferfactory.cpp                                  -*-C++-*-

#include <bdlbb_simpleblobbufferfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlbb_simpleblobbufferfactory,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bslstl_sharedptr.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace bdlbb {

                         // -------------------------
                         // class LocalTimeDescriptor
                         // -------------------------

// CREATORS
SimpleBlobBufferFactory::SimpleBlobBufferFactory(
                                              int               bufferSize,
                                              bslma::Allocator *basicAllocator)
: d_size(static_cast<int>(bufferSize))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SimpleBlobBufferFactory::~SimpleBlobBufferFactory()
{}

// MANIPULATORS
void SimpleBlobBufferFactory::allocate(bdlbb::BlobBuffer *buffer)
{
    char *segment = static_cast<char *>(d_allocator_p->allocate(d_size));
    bsl::shared_ptr<char> sharedPtr(segment, d_allocator_p);
    buffer->reset(sharedPtr, d_size);
}

void SimpleBlobBufferFactory::setBufferSize(int bufferSize)
{
    d_size = static_cast<int>(bufferSize);
}

// ACCESSORS
int SimpleBlobBufferFactory::bufferSize() const
{
    return d_size;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
















