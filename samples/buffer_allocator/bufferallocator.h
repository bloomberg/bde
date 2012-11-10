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
// buffer, the optionally-supplied allocator is used:
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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_types.h>

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
    int               d_bufferSize;    // initial buffer size
    int               d_cursor;        // current cursor
    bslma::Allocator *d_allocator_p;   // memory allocator once 'd_buffer_p'
                                       // is exhausted

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
        // Destroy this buffer allocator.  Note that if the buffer specified at
        // construction was not exhausted then no deallocation overhead is
        // incurred.

    // MANIPULATORS
    virtual void *allocate(bsls_Types::size_type size);
        // Return the address of a contiguous block of maximally-aligned memory
        // of the specified 'size' (in bytes).  If 'size' is 0 no memory is
        // allocated and 0 is returned.  If the allocation request exceeds the
        // remaining free memory space in the external buffer supplied at
        // construction, the allocator specified at construction is used.  The
        // behavior is undefined unless '0 <= size'.

    virtual void deallocate(void *address);
        // Deallocate the specified 'address' if it did not come from the
        // external buffer specified at construction and do nothing otherwise.
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
