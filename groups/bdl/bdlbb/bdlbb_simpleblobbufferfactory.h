// bdlbb_simpleblobbufferfactory.h                                    -*-C++-*-
#ifndef INCLUDED_BDLBB_SIMPLEBLOBBUFFERFACTORY
#define INCLUDED_BDLBB_SIMPLEBLOBBUFFERFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple implementation of 'bdlbb::BlobBufferFactory'.
//
//@CLASSES:
//  bdlbb::SimpleBlobBufferFactory: simple form of 'bdlbb::BlobBufferFactory'
//
//@DESCRIPTION: This component provides a mechanism for allocating
// 'bdlbb::BlobBuffer' objects of a specified size.  The size is passed at
// construction of the 'bdlbb::SimpleBlobBufferFactory' instance.  A
// 'bdlbb::BlobBuffer' contains a shared pointer to a buffer of 'char' of the
// prescribed size.  A 'bslma::Allocator *' is passed at construction to
// allocate the buffers (if none is passed the default allocator is used), and
// the buffers are allocated by separate calls to the allocator.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Simple Blob Buffer Factory
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to make a blob that can be grown via calls to 'setLength',
// meaning that it must have a factory, and suppose you want all the memory for
// the blob buffers created for the factory to be allocated directly from a
// certain test allocator for test purposes.  We use a
// 'SimpleBlobBufferFactory'.
//..
// First, we create our allocator:
//..
//  bslma::TestAllocator testAllocator;
//..
// Then, we create our factor using that allocator:
//..
//  bdlbb::SimpleBlobBufferFactory factory(1024, &testAllocator);
//  assert(factory.bufferSize() == 1024);
//..
// Next, we create our blob using that factory:
//..
//  bdlbb::Blob blob(&factory);
//..
// Next, we set the length big enough to require 20 blob buffers:
//..
//  blob.setLength(1024 * 20);
//..
// Then, we verify that the memory came from 'testAllocator'.  Note that since
// the blob buffers contain shared pointers, additional memory other than the
// writable areas of the blob buffers is allocated:
//..
//  assert(1024 * 20 < testAllocator.numBytesInUse());
//..
// Now, we examine the state of the blob:
//..
//  assert(20 == blob.numDataBuffers());
//  assert(20 == blob.numBuffers());
//  assert(1024 * 20 == blob.length());
//..
// Finally, we examine the blob buffers:
//..
//  for (int ii = 0; ii < blob.numDataBuffers(); ++ii) {
//      assert(1024 == blob.buffer(ii).size());
//  }
//..

#include <bdlscm_version.h>

#include <bdlbb_blob.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_isbitwisemoveable.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlbb {

                       // =============================
                       // class SimpleBlobBufferFactory
                       // =============================

class SimpleBlobBufferFactory : public BlobBufferFactory {
    // This 'class' declares an implementation of 'BlobBufferFactory' where
    // each segment loaded to a 'BlobBuffer' is created with a separate calls
    // to the allocator specified at construction, or the default allocator is
    // no allocator was specified.

    // PRIVATE DATA
    int               d_size;
    bslma::Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED:
    SimpleBlobBufferFactory(           const SimpleBlobBufferFactory&);
    SimpleBlobBufferFactory& operator=(const SimpleBlobBufferFactory&);

  public:
    // CREATORS
    explicit
    SimpleBlobBufferFactory(bsl::size_t       bufferSize,
                            bslma::Allocator *basicAllocator = 0);
        // Create a 'SimpleBlobBufferFactory' object that will create blob
        // buffers of specified length 'bufferSize' using the specified
        // 'basicAllocator' to supply memory, or the default allocator if no
        // allocator is passed.  The behavior is undefined unless 'bufferSize'
        // can be represented as an 'int'.

    virtual ~SimpleBlobBufferFactory();
        // Destroy this 'SimpleBlobBufferFactory' object.

    // MANIPULATORS
    virtual void allocate(bdlbb::BlobBuffer *buffer);
        // Allocate a segment and set the specified 'buffer' to it.

    void setBufferSize(bsl::size_t bufferSize);
        // Set the buffer size for future buffers created by this factory to
        // the specified 'bufferSize'.  The behavior is undefined unless
        // 'bufferSize' can be represented as an 'int'.

    // ACCESSORS
    int bufferSize() const;
        // Return the current size with which this factory will allocate
        // buffers.
};

}  // close package namespace

namespace bslmf {
template <>
struct IsBitwiseMoveable<bdlbb::SimpleBlobBufferFactory> : bsl::true_type {
};
}  // close namespace bslmf

namespace bslma {
template <>
struct UsesBslmaAllocator<bdlbb::SimpleBlobBufferFactory> : bsl::true_type {
};
}  // close namespace bslma

}  // close enterprise namespace

#endif

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
