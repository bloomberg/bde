// pkg_bufferallocator.h                                              -*-C++-*-
#ifndef INCLUDED_PKG_BUFFERALLOCATOR
#define INCLUDED_PKG_BUFFERALLOCATOR

//@PURPOSE: Provide an efficient allocator using a user-supplied buffer.
//
//@CLASSES:
//  pkg::BufferAllocator: memory allocator from user-supplied buffer
//
//@SEE_ALSO: bslma_allocator
//
//@DESCRIPTION: This component provides a concrete allocator,
// 'pkg::BufferAllocator', that implements the 'bslma::Allocator' protocol and
// sequentially allocates memory blocks from a fixed-size buffer that is
// supplied by the user at construction.  If an allocation request exceeds the
// remaining space in the buffer, the optionally-supplied allocator is used to
// supply memory.  If an allocator was not supplied at construction, the
// currently installed default allocator is used.  For maximum efficiency,
// individual deallocations of memory blocks within the user-supplied buffer
// are no-ops and the deallocated memory is not reused in subsequent allocation
// requests.  Memory blocks allocated from the user-supplied (or default)
// allocator are individually deallocated:
//..
//   ,---------------------.
//  (    BufferAllocator    )
//   `---------------------'
//              |         ctor
//              V
//      ,---------------.
//     ( bslma::Allocator )
//      `---------------'
//                        allocate
//                        deallocate
//                        dtor
//..
///Alignment Strategy
///------------------
// For simplicity, this allocator always returns maximally-aligned memory.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Reading Data From a File
///- - - - - - - - - - - - - - - - - -
// It is often necessary to create temporary container objects with known
// memory requirements.  Since these objects have a short lifetime, their
// memory needs can be satisfied from a static buffer instead of through
// expensive heap allocations and deallocations.
//
// Consider the simplified function, 'readLines', that reads all of the lines
// from a small file into a 'bsl::vector<bsl::string>' before processing them.
// As the data read into the 'vector' is short-lived and will be discarded on
// the return of the function, a 'BufferAllocator' with a sufficiently large
// buffer can be used to allocate the memory.
//..
//  #include <bsl_istream.h>
//  #include <bsl_string.h>
//  #include <bsl_vector.h>
//
//  #include <pkg_bufferallocator.h>
//
//  void readLines(bsl::istream& inputStream)
//  {
//      const int BUFFER_SIZE = 1024;
//      char      buffer[BUFFER_SIZE];
//
//..
// Create a 'pkg::BufferAllocator' object constructed from 'buffer'.  We do not
// supply an allocator to use for memory requests that cannot be satisfied from
// 'buffer'.  Instead, the 'allocator' object will use the currently installed
// default allocator when that happens.
//..
//      pkg::BufferAllocator     allocator(buffer, BUFFER_SIZE);
//      bsl::vector<bsl::string> lines(&allocator);
//
//      while (inputStream) {
//          bsl::string input(&allocator);
//
//          bsl::getline(inputStream, input);
//
//          lines.push_back(input);
//      }
//
//      // Continue processing the read data.
//  }
//..

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace Enterprise {
namespace pkg {

                        // =====================
                        // class BufferAllocator
                        // =====================

class BufferAllocator : public BloombergLP::bslma::Allocator {
    // This 'class' provides a concrete buffer allocator that implements the
    // 'bslma::Allocator' interface, allocating memory blocks from a fixed-size
    // buffer that is supplied by the user at construction, or from an
    // optionally-specified allocator or the default allocator for allocation
    // requests that exceed the remaining space in the buffer.

    // DATA
    char                          *d_buffer_p;      // buffer to use for memory
                                                    // allocations (held, not
                                                    // owned)

    int                            d_bufferSize;    // initial buffer size in
                                                    // bytes

    int                            d_cursor;        // current cursor

    BloombergLP::bslma::Allocator *d_allocator_p;   // memory allocator to use
                                                    // if an allocation
                                                    // request exceeds the
                                                    // remaining space in
                                                    // 'd_buffer_p' (held, not
                                                    // owned)

    // NOT IMPLEMENTED
    BufferAllocator(const BufferAllocator&);
    BufferAllocator& operator=(const BufferAllocator&);

  public:
    // CREATORS
    BufferAllocator(char                          *buffer,
                    int                            bufferSize,
                    BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create a buffer allocator for allocating memory blocks from the
        // specified 'buffer' of the specified 'bufferSize' bytes.  Optionally
        // specify a 'basicAllocator' used to supply memory for allocation
        // requests that exceed the remaining space in 'buffer'.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= bufferSize' and
        // 'buffer' refers to at least 'bufferSize' contiguous bytes in memory.

    virtual ~BufferAllocator();
        // Destroy this buffer allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    virtual void *allocate(BloombergLP::bsls::Types::size_type size);
        // Return the address of a contiguous block of maximally-aligned memory
        // of the specified 'size' (in bytes).  If 'size' is 0, no memory is
        // allocated and 0 is returned.  If an allocation request exceeds the
        // remaining free memory space in the external buffer supplied at
        // construction, use the allocator specified at construction or the
        // currently installed default allocator.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to the
        // external (user-supplied or the default) allocator used by this
        // object if 'address' did not come from the external buffer supplied
        // at construction and do nothing otherwise.  If 'address' is 0, this
        // function has no effect.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class BufferAllocator
                        // ---------------------

// CREATORS
inline
BufferAllocator::BufferAllocator(char                          *buffer,
                                 int                            bufferSize,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_buffer_p(buffer)
, d_bufferSize(bufferSize)
, d_cursor(0)
, d_allocator_p(BloombergLP::bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferSize);
}

inline
BufferAllocator::~BufferAllocator()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
