// bdlma_managedallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#define INCLUDED_BDLMA_MANAGEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support 'release'.
//
//@CLASSES:
//   bdlma::ManagedAllocator: protocol for allocators with 'release' capability
//
//@SEE_ALSO: bdlma_bufferedsequentialallocator
//
//@DESCRIPTION: This component provides a 'class', 'bdlma::ManagedAllocator',
// that extends the 'bslma::Allocator' protocol to allocators that support the
// ability to 'release' all memory currently allocated through the protocol
// back to the memory supplier of the derived concrete allocator object.
//..
//   ,-----------------------.
//  ( bdlma::ManagedAllocator )
//   `-----------------------'
//               |        release
//               |
//               v
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                        allocate
//                        deallocate
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing the 'bdlma::ManagedAllocator' Protocol
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlma::ManagedAllocator' interface is especially useful for allocators
// that are based on an underlying pooling mechanism (e.g., 'bdlma::Multipool'
// or 'bdlma::BufferedSequentialPool').  In particular, such an allocator that
// implements the 'bdlma::ManagedAllocator' interface can release, via the
// 'release' method, all outstanding (pooled) memory back to the underlying
// allocator making the memory available for subsequent reuse.  Moreover, use
// of the 'release' method can also often render superfluous the running of
// destructors on the objects making use of a managed allocator.  In this first
// usage example, we define the 'my_BufferAllocator' class, an allocator that
// implements the 'bdlma::ManagedAllocator' interface.  'my_BufferAllocator' is
// a considerably pared down version of 'bdlma::BufferedSequentialAllocator',
// and is intended for illustration purposes only.  Please see the
// 'bdlma_bufferedsequentialallocator' component for full documentation of
// 'bdlma::BufferedSequentialAllocator', a managed allocator meant for
// production use.
//
// First, we define the interface of the 'my_BufferAllocator' class:
//..
//  // my_bufferallocator.h
//
//  class my_BufferAllocator : public bdlma::ManagedAllocator {
//      // This 'class' provides a concrete buffer allocator that implements
//      // the 'bdlma::ManagedAllocator' protocol.
//
//      // DATA
//      char *d_buffer_p;    // external buffer (held, not owned)
//      int   d_bufferSize;  // size (in bytes) of external buffer
//      int   d_cursor;      // offset to next available byte in buffer
//
//    private:
//      // NOT IMPLEMENTED
//      my_BufferAllocator(const my_BufferAllocator&);
//      my_BufferAllocator& operator=(const my_BufferAllocator&);
//
//    public:
//      // CREATORS
//      my_BufferAllocator(char *buffer, size_type bufferSize);
//          // Create a buffer allocator for allocating maximally-aligned
//          // memory blocks from the specified external 'buffer' having the
//          // specified 'bufferSize' (in bytes).
//
//      ~my_BufferAllocator();
//          // Destroy this buffer allocator.
//
//      // MANIPULATORS
//      void *allocate(size_type size);
//          // Return the address of a maximally-aligned contiguous block of
//          // memory of the specified 'size' (in bytes) on success, and 0 if
//          // the allocation request exceeds the remaining free memory space
//          // in the external buffer.
//
//      void deallocate(void *address);
//          // This method has no effect for this buffer allocator.
//
//      void release();
//          // Release all memory allocated through this object.  This
//          // allocator is reset to the state it was in immediately following
//          // construction.
//  };
//..
// Next, we define the 'inline' methods of 'my_BufferAllocator'.  Note that the
// 'release' method resets the internal cursor to 0, effectively making the
// memory from the entire external buffer supplied at construction available
// for subsequent allocations, but has no effect on the contents of the buffer:
//..
//  // CREATORS
//  inline
//  my_BufferAllocator::my_BufferAllocator(char *buffer, size_type bufferSize)
//  : d_buffer_p(buffer)
//  , d_bufferSize(static_cast<int>(bufferSize))
//  , d_cursor(0)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void my_BufferAllocator::deallocate(void *)
//  {
//  }
//
//  inline
//  void my_BufferAllocator::release()
//  {
//      d_cursor = 0;
//  }
//..
// Finally, we provide the implementation of the 'my_BufferAllocator' methods
// that are defined in the '.cpp' file.  A 'static' helper function,
// 'allocateFromBufferImp', provides the bulk of the implementation of the
// 'allocate' method:
//..
//  // my_bufferallocator.cpp
//
//  // STATIC HELPER FUNCTIONS
//  static
//  void *allocateFromBufferImp(int  *cursor,
//                              char *buffer,
//                              int   bufferSize,
//                              int   size)
//      // Allocate a maximally-aligned memory block of the specified 'size'
//      // (in bytes) from the specified 'buffer' having the specified
//      // 'bufferSize' (in bytes) at the specified 'cursor' position.  Return
//      // the address of the allocated memory block if 'buffer' contains
//      // sufficient available memory, and 0 otherwise.  The 'cursor' is set
//      // to the first byte position immediately after the allocated memory if
//      // there is sufficient memory, and not modified otherwise.  The
//      // behavior is undefined unless '0 <= bufferSize', '0 < size',
//      // '0 <= *cursor', and '*cursor <= bufferSize'.
//
//  {
//      const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
//                                    buffer + *cursor,
//                                    bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
//
//      if (*cursor + offset + size > bufferSize) {  // insufficient space
//          return 0;                                                 // RETURN
//      }
//
//      void *result = &buffer[*cursor + offset];
//      *cursor += offset + size;
//
//      return result;
//  }
//
//  // CREATORS
//  my_BufferAllocator::~my_BufferAllocator()
//  {
//  }
//
//  // MANIPULATORS
//  void *my_BufferAllocator::allocate(size_type size)
//  {
//      return 0 == size ? 0 : allocateFromBufferImp(&d_cursor,
//                                                   d_buffer_p,
//                                                   d_bufferSize,
//                                                   static_cast<int>(size));
//  }
//..
//
///Example 2: Using the 'bdlma::ManagedAllocator' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this second usage example, we illustrate how the managed allocator that
// was defined in Example 1, 'my_BufferAllocator', may be used.  Note that
// substantial portions of the sample implementation are elided as they would
// only add unnecessary complications to the usage example.  The portions shown
// are sufficient to illustrate the use of 'bdlma::ManagedAllocator'.
//
// The domain of our example is financial markets.  Suppose that we are given a
// list of market indices (e.g., Dow Jones Industrial Average, S&P 500, etc.),
// and we want to perform some computation on each index, in turn.  In this
// example, the essential attributes of an index are held in a 'bsl::pair'
// consisting of the name of the index (e.g., "DJIA") and the number of
// securities that comprise the index (e.g., 30 in the case of the DJIA).  The
// collection of market indices that we wish to process is given by a vector of
// such pairs.  Thus, we make use of these types related to indices:
//..
//  typedef bsl::pair<const char *, int> IndexAttributes;
//  typedef bsl::vector<IndexAttributes> IndexCollection;
//..
// In our example, a security is defined by the unconstrained attribute type
// 'my_SecurityAttributes', the interface and implementation of which is elided
// except we note that it uses 'bslma' allocators:
//..
//  class my_SecurityAttributes {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(my_SecurityAttributes,
//                                     bslma::UsesBslmaAllocator);
//
//      // ...
//  };
//..
// For the collection of securities comprising an index we use a vector of
// 'my_SecurityAttributes':
//..
//  typedef bsl::vector<my_SecurityAttributes> SecurityCollection;
//..
// Since some indices are quite large (e.g., Russell 3000, Wilshire 5000), for
// performance reasons it is advantageous for a 'SecurityCollection' to use an
// efficient memory allocation strategy.  This is where 'my_BufferAllocator'
// comes into play, which we will see shortly.
//
// The top-level function in our example takes a 'bdlma::ManagedAllocator *'
// and the collection of market indices that we wish to process:
//..
//  static
//  void processIndices(bdlma::ManagedAllocator *managedAllocator,
//                      const IndexCollection&   indices);
//      // Process the specified market 'indices' using the specified
//      // 'managedAllocator' to supply memory.
//..
// 'processIndices' makes use of two helper functions to process each index:
//..
//  static
//  void loadIndex(SecurityCollection      *securities,
//                 bdlma::ManagedAllocator *managedAllocator,
//                 const IndexAttributes&   index);
//      // Load into the specified collection of 'securities' the attributes of
//      // the securities comprising the specified market 'index' using the
//      // specified 'managedAllocator' to supply memory.
//
//  static
//  void processIndex(const SecurityCollection& securities,
//                    const IndexAttributes&    index);
//      // Process the specified collection of 'securities' that comprise the
//      // specified market 'index'.
//..
// Since we plan to use 'my_BufferAllocator' as our managed allocator, we need
// to supply it with an external buffer.  The 'calculateMaxBufferSize' function
// computes the size of the buffer required to store the 'SecurityCollection'
// corresponding to the largest index to be processed by a given call to
// 'processIndices':
//..
//  int calculateMaxBufferSize(const IndexCollection& indices);
//      // Return the maximum buffer size (in bytes) required to process the
//      // specified collection of market 'indices'.
//..
// Before showing the implementation of 'processIndices', where the most
// interesting use of our managed allocator takes place, we show the site of
// the call to 'processIndices'.
//
// First, assume that we have been given an 'IndexCollection' that has been
// populated with one or more 'IndexAttributes':
//..
//  IndexCollection indices;  // assume populated
//..
// Next, we calculate the size of the buffer that is needed, allocate the
// memory for the buffer from the default allocator, create our concrete
// managed allocator (namely, an instance of 'my_BufferAllocator'), and call
// 'processIndices':
//..
//  const int bufferSize = calculateMaxBufferSize(indices);
//
//  bslma::Allocator *allocator = bslma::Default::defaultAllocator();
//  char *buffer = static_cast<char *>(allocator->allocate(bufferSize));
//
//  my_BufferAllocator bufferAllocator(buffer, bufferSize);
//
//  processIndices(&bufferAllocator, indices);
//..
// Next, we show the implementation of 'processIndices', within which we
// iterate over the market 'indices' that are passed to it:
//..
//  static
//  void processIndices(bdlma::ManagedAllocator *managedAllocator,
//                      const IndexCollection&   indices)
//      // Process the specified market 'indices' using the specified
//      // 'managedAllocator' to supply memory.
//  {
//      for (IndexCollection::const_iterator citer = indices.begin();
//                                           citer != indices.end(); ++citer) {
//
//..
// For each index, the 'SecurityCollection' comprising that index is created.
// All of the memory needs of the 'SecurityCollection' are provided by the
// 'managedAllocator'.  Note that even the memory for the footprint of the
// collection comes from the 'managedAllocator':
//..
//          SecurityCollection *securities =
//              new (managedAllocator->allocate(sizeof(SecurityCollection)))
//                                        SecurityCollection(managedAllocator);
//
//..
// Next, we call 'loadIndex' to populate 'securities', followed by the call to
// 'processIndex'.  'loadIndex' also uses the 'managedAllocator', the details
// of which are not shown here:
//..
//          loadIndex(securities, managedAllocator, *citer);
//
//          processIndex(*securities, *citer);
//..
// After the index is processed, 'release' is called on the managed allocator
// making all of the buffer supplied to the allocator at construction available
// for reuse:
//..
//          managedAllocator->release();
//      }
//..
// Finally, we let the 'SecurityCollection' used to process the index go out of
// scope intentionally without deleting 'securities'.  The call to 'release'
// renders superfluous the need to call the 'SecurityCollection' destructor as
// well as the destructor of the contained 'my_SecurityAttributes' elements.
//..
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {
namespace bdlma {

                          // ======================
                          // class ManagedAllocator
                          // ======================

class ManagedAllocator : public bslma::Allocator {
    // This protocol class extends 'bslma::Allocator' for allocators with the
    // ability to 'release' all memory currently allocated through the protocol
    // back to the memory supplier of the derived concrete allocator object.

  public:
    // MANIPULATORS
    virtual void release() = 0;
        // Release all memory currently allocated through this allocator.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
