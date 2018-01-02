// bdlma_bufferedsequentialpool.cpp                                   -*-C++-*-
#include <bdlma_bufferedsequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_bufferedsequentialpool_cpp,"$Id$ $CSID$")

#include <bsl_limits.h>

namespace BloombergLP {
namespace bdlma {

                       // ----------------------------
                       // class BufferedSequentialPool
                       // ----------------------------

// CREATORS
BufferedSequentialPool::BufferedSequentialPool(
                                        char                   *buffer,
                                        bsls::Types::size_type  size,
                                        bslma::Allocator       *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size)
, d_pool(size,
         bsl::numeric_limits<bsls::Types::size_type>::max(),
         bsls::BlockGrowth::BSLS_GEOMETRIC,
         bsls::Alignment::BSLS_NATURAL,
         false,
         basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                   char                        *buffer,
                                   bsls::Types::size_type       size,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size)
, d_pool(size,
         bsl::numeric_limits<bsls::Types::size_type>::max(),
         growthStrategy,
         bsls::Alignment::BSLS_NATURAL,
         false,
         basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                  char                      *buffer,
                                  bsls::Types::size_type     size,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size, alignmentStrategy)
, d_pool(size,
         bsl::numeric_limits<bsls::Types::size_type>::max(),
         bsls::BlockGrowth::BSLS_GEOMETRIC,
         alignmentStrategy,
         false,
         basicAllocator)
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
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size, alignmentStrategy)
, d_pool(size,
         bsl::numeric_limits<bsls::Types::size_type>::max(),
         growthStrategy,
         alignmentStrategy,
         false,
         basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                        char                   *buffer,
                                        bsls::Types::size_type  size,
                                        bsls::Types::size_type  maxBufferSize,
                                        bslma::Allocator       *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size)
, d_pool(size,
         maxBufferSize,
         bsls::BlockGrowth::BSLS_GEOMETRIC,
         bsls::Alignment::BSLS_NATURAL,
         false,
         basicAllocator)
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
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size)
, d_pool(size,
         maxBufferSize,
         growthStrategy,
         bsls::Alignment::BSLS_NATURAL,
         false,
         basicAllocator)
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
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size, alignmentStrategy)
, d_pool(size,
         maxBufferSize,
         bsls::BlockGrowth::BSLS_GEOMETRIC,
         alignmentStrategy,
         false,
         basicAllocator)
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
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_bufferManager(buffer, size, alignmentStrategy)
, d_pool(size,
         maxBufferSize,
         growthStrategy,
         alignmentStrategy,
         false,
         basicAllocator)
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
