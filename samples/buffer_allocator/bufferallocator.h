// bufferallocator.h                                                  -*-C++-*-
#ifndef INCLUDED_BUFFERALLOCATOR
#define INCLUDED_BUFFERALLOCATOR

//@PURPOSE: Provide an efficient allocator using a user-supplied buffer.
//
//@CLASSES:
//  pkg::BufferAllocator: memory allocator from user-supplied buffer
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides an allocator, 'BufferAllocator', that
// implements the 'bslma::Allocator' protocol and sequentially allocates memory
// blocks from a fixed-size buffer that is supplied by the user at
// construction.  If an allocation request exceeds the remaining space in the
// buffer, the optionally-supplied allocator is used.  For maximum efficiency
// individual deallocations of memory within the user-supplied buffer are
// no-ops and the deallocated memory is not reused in subsequent allocation
// requests.  Memory allocated from the user-supplied allocator (after the
// fixed-sized buffer is exhausted) is individually deallocated:
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
// For simplicity this allocator always returns maximally-aligned memory.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Reading Data From a File
///- - - - - - - - - - - - - - - - - -
// Often objects that allocate memory are stored within standard containers.
//
// Consider the simplified function, 'readLines', that reads all the lines from
// a small file into a 'bsl::vector<bsl::string>' before processing them.  As
// the data read into the 'vector' is short-lived and will be discarded on the
// return of the function, a 'BufferAllocator' with a sufficiently large buffer
// can be used to allocate the memory.
//..
//  #include <bsl_istream.h>
//  #include <bsl_string.h>
//  #include <bsl_vector.h>
//
//  #include <bufferallocator.h>
//
//  void readLines(bsl::istream& inputStream)
//  {
//      const int BUFFER_SIZE = 1024;
//      char      buffer[BUFFER_SIZE];
//
//      BufferAllocator          allocator(buffer, BUFFER_SIZE);
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
//      // Continue processing the read data
//  }
//..

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

namespace Enterprise {
namespace pkg {

                        // =====================
                        // class BufferAllocator
                        // =====================

using namespace BloombergLP;

class BufferAllocator : public bslma::Allocator {
    // This 'class' provides a concrete buffer allocator that implements the
    // 'bslma::Allocator' interface, allocating memory blocks from a fixed-size
    // buffer that is supplied by the user at construction, or from an
    // optionally-specified allocator once that buffer is exhausted.

    // DATA
    char             *d_buffer_p;      // buffer to use for memory allocations
                                       // (held, not owned)
    int               d_bufferSize;    // initial buffer size
    int               d_cursor;        // current cursor
    bslma::Allocator *d_allocator_p;   // memory allocator to use once
                                       // 'd_buffer_p' is exhausted (held, not
                                       // owned)

    // NOT IMPLEMENTED
    BufferAllocator(const BufferAllocator&);
    BufferAllocator& operator=(const BufferAllocator&);

  public:
    // CREATORS
    BufferAllocator(char             *buffer,
                    int               bufferSize,
                    bslma::Allocator *basicAllocator = 0);
        // Create a buffer allocator for allocating memory blocks from the
        // specified 'buffer' of the specified 'bufferSize'.  Optionally
        // specify a 'basicAllocator' used to supply memory after that 'buffer'
        // is exhausted.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    virtual ~BufferAllocator();
        // Destroy this buffer allocator.

    // MANIPULATORS
    virtual void *allocate(bsls::Types::size_type size);
        // Return the address of a contiguous block of maximally-aligned memory
        // of the specified 'size' (in bytes).  If 'size' is 0 no memory is
        // allocated and 0 is returned.  If the allocation request exceeds the
        // remaining free memory space in the external buffer supplied at
        // construction, the allocator specified at construction is used.  The
        // behavior is undefined unless '0 <= size'.

    virtual void deallocate(void *address);
        // Deallocate the specified 'address' if it did not come from the
        // external buffer specified at construction and do nothing otherwise.
        // Note that if the buffer specified at construction was not exhausted
        // then no deallocation overhead is incurred.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class BufferAllocator
                        // ---------------------

// CREATORS
inline
BufferAllocator::BufferAllocator(char             *buffer,
                                 int               bufferSize,
                                 bslma::Allocator *basicAllocator)
: d_buffer_p(buffer)
, d_bufferSize(bufferSize)
, d_cursor(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BufferAllocator::~BufferAllocator()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
