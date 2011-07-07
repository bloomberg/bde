// bdema_heapbypassallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BDEMA_HEAPBYPASSALLOCATOR
#define INCLUDED_BDEMA_HEAPBYPASSALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Support memory allocation directly from virtual memory.
//
//@CLASSES:
//   bdema_HeapBypassAllocator: memory allocator directly from virtual memory
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@SEE_ALSO: bsls_alignmentutil
//
//@DESCRIPTION: The 'bdema_HeapBypassAllocator' class provided by this
// component implements a concrete allocator derived from the 'bslma_Allocator'
// interface that allocates memory directly from virtual memory, bypassing the
// heap.  The heap, normally accessed via 'malloc', 'new', 'free' and 'delete',
// maintains a free list of freed memory, which may become corrupt, with
// disastrous results for subsequent heap allocation.  This allocator is thus
// useful when the heap may be corrupt.  All memory allocated by this allocator
// is freed when the allocator's destructor is called, but not before.
//..
//                   ( bdema_HeapBypassAllocator )
//                                 |         ctor
//                                 V
//                        ( bslma_Allocator )
//                                           dtor
//                                           allocate
//                                           deallocate      // no-op
//..
///Usage
///-----
// Here we allocate some memory using a heap bypass allocator, then write to
// that memory, then read from it and verify the values written are preserved.
//..
//  {    
//      enum {
//          LENGTH = 10 * 1000,
//          NUM_SEGMENTS = 60
//      };
//
//      bdema_HeapBypassAllocator hbpa;
//. .
// First we allocate some segments
//..
//      char *segments[NUM_SEGMENTS];
//      for (int i = 0; i < NUM_SEGMENTS; ++i) {
//          segments[i] = static_cast<char *>(hbpa.allocate(LENGTH));
//          BSLS_ASSERT(segments[i]);
//      }
//..
// Next we write to the segments.
//..
//      char c = 'a';
//      for (int i = 0; i < NUM_SEGMENTS; ++i) {
//          char *segment = segments[i];
//          for (int j = 0; j < LENGTH; ++j) {
//              c = (c + 1) & 0x7f;
//              segment[j] = c;
//          }
//      }
//..
// Finally, we read from the segments and verify the written data is still
// there.
//..
//      c = 'a';
//      for (int i = 0; i < NUM_SEGMENTS; ++i) {
//          char *segment = segments[i];
//          for (int j = 0; j < LENGTH; ++j) {
//              c = (c + 1) & 0x7f;
//              BSLS_ASSERT(segment[j] == c);
//          }
//      }
//..
// Memory is released upon destruction of object 'hbpa' when it goes out of
// scope.
//..
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

                      // ===============================
                      // class bdema_HeapBypassAllocator
                      // ===============================

class bdema_HeapBypassAllocator : public bslma_Allocator {
    // This class allows the caller to allocate memory directly from virtual
    // memory, without going through the heap like 'malloc' or 'new' would.
    // Note that the only way to free any memory allocated with this object is
    // to destroy the object, at which point all memory it has allocated is
    // freed.

    // PRIVATE TYPES
    struct BufferHeader;                      // implementation detail

    // DATA
    size_type             d_pageSize;         // virtual memory page size

    BufferHeader         *d_firstBuffer_p;    // first buffer in linked list

    BufferHeader         *d_currentBuffer_p;  // last buffer in linked list,
                                              // also current buffer for
                                              // allocations

    char                 *d_cursor_p;         // pointer into current buffer,
                                              // where next allocation will
                                              // come from

    const char           *d_endOfBuffer_p;    // end of current buffer

    size_type             d_alignment;        // max memory alignment of
                                              // platform

  private:
    // NOT IMPLEMENTED
    bdema_HeapBypassAllocator(const bdema_HeapBypassAllocator&);
    bdema_HeapBypassAllocator& operator=(const bdema_HeapBypassAllocator&);

  private:
    // PRIVATE CLASS METHODS
    static char *map(size_type size);
        // Return a pointer to a newly allocated buffer of memory that is at
        // least the specified 'size' (in bytes) on success, and a 0 value
        // otherwise.

    static void unmap(void *address, size_type size);
        // Release a buffer of memory that starts at the specified 'address'
        // and has the specified 'size' (in bytes).  The behavior is undefined
        // unless 'address' is the value returned from a previous call to
        // 'map', and 'size' is the length (in bytes) of the buffer of memory
        // created by that same call to 'map'.

    // PRIVATE MANIPULATORS
    int replenish(size_type size);
        // Obtain an additional buffer of memory for this object, a multiple of
        // the page size in length, and at least large enough to allow
        // allocation of a segment of the specified 'size'.  Return 0 on
        // success, and a non-zero value otherwise.

  public:
    // CREATORS
    bdema_HeapBypassAllocator();
        // Create an empty heap bypass allocator object.

    virtual ~bdema_HeapBypassAllocator();
        // Destroy this object, releasing all managed buffers of memory that it
        // has allocated.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Allocate a buffer of memory having the specified 'size' (in bytes),
        // and alignment defined by 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT',
        // from virtual memory.  Return the address of the allocated memory on
        // success, and 0 otherwise.

    virtual void deallocate(void *);
        // This method has no effect for this heap bypass allocator.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
