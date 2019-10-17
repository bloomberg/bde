// bdlma_bufferedsequentialpool.cpp                                   -*-C++-*-
#include <bdlma_bufferedsequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_bufferedsequentialpool_cpp,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bsls_alignmentutil.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace bdlma {

                       // ----------------------------
                       // class BufferedSequentialPool
                       // ----------------------------

// PRIVATE MANIPULATOR
void BufferedSequentialPool::createSequentialPool(
                                  bsls::Types::size_type currentAllocationSize)
{
    // Initial block size of the 'SequentialPool':

    const bsls::Types::size_type initialSize = d_bufferManager.bufferSize();

    // Only have the sequential pool allocate its initial block at construction
    // if we anticipate that the current allocation will fit within that
    // initial block.

    const bool createInitialBlockNow = currentAllocationSize <= initialSize;

    // Note that 'd_pool_p' and 'd_allocator_p' fit in the same footprint in an
    // anonymous union.

    d_pool_p = new (*d_allocator_p) bdlma::SequentialPool(
                                                         initialSize,
                                                         d_maxBufferSize,
                                                         growthStrategy(),
                                                         alignmentStrategy(),
                                                         createInitialBlockNow,
                                                         d_allocator_p);

    d_sequentialPoolIsCreated = true;
}

// CREATORS
BufferedSequentialPool::BufferedSequentialPool(
                                        char                   *buffer,
                                        bsls::Types::size_type  size,
                                        bslma::Allocator       *basicAllocator)
: d_bufferManager(buffer, size)
, d_maxBufferSize(bsl::numeric_limits<bsls::Types::size_type>::max())
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                   char                        *buffer,
                                   bsls::Types::size_type       size,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_bufferManager(buffer, size)
, d_maxBufferSize(bsl::numeric_limits<bsls::Types::size_type>::max())
, d_growthStrategy(growthStrategy)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                  char                      *buffer,
                                  bsls::Types::size_type     size,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_bufferManager(buffer, size, alignmentStrategy)
, d_maxBufferSize(bsl::numeric_limits<bsls::Types::size_type>::max())
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                char                        *buffer,
                                bsls::Types::size_type       size,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
: d_bufferManager(buffer, size, alignmentStrategy)
, d_maxBufferSize(bsl::numeric_limits<bsls::Types::size_type>::max())
, d_growthStrategy(growthStrategy)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                        char                   *buffer,
                                        bsls::Types::size_type  size,
                                        bsls::Types::size_type  maxBufferSize,
                                        bslma::Allocator       *basicAllocator)
: d_bufferManager(buffer, size)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0    <  size);
    BSLS_ASSERT(size <= maxBufferSize);
}

BufferedSequentialPool::BufferedSequentialPool(
                                 char                        *buffer,
                                 bsls::Types::size_type       size,
                                 bsls::Types::size_type       maxBufferSize,
                                 bsls::BlockGrowth::Strategy  growthStrategy,
                                 bslma::Allocator            *basicAllocator)
: d_bufferManager(buffer, size)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(growthStrategy)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0    <  size);
    BSLS_ASSERT(size <= maxBufferSize);
}

BufferedSequentialPool::BufferedSequentialPool(
                                  char                      *buffer,
                                  bsls::Types::size_type     size,
                                  bsls::Types::size_type     maxBufferSize,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_bufferManager(buffer, size, alignmentStrategy)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0    <  size);
    BSLS_ASSERT(size <= maxBufferSize);
}

BufferedSequentialPool::BufferedSequentialPool(
                                char                        *buffer,
                                bsls::Types::size_type       size,
                                bsls::Types::size_type       maxBufferSize,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
: d_bufferManager(buffer, size, alignmentStrategy)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(growthStrategy)
, d_sequentialPoolIsCreated(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0    <  size);
    BSLS_ASSERT(size <= maxBufferSize);
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
