// bdlma_localsequentialallocator.h                                   -*-C++-*-
#ifndef INCLUDED_BDLMA_LOCALSEQUENTIALALLOCATOR
#define INCLUDED_BDLMA_LOCALSEQUENTIALALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient managed allocator using a local buffer.
//
//@CLASSES:
//  bdlma::LocalSequentialAllocator: allocator using a local buffer
//
//@SEE_ALSO: bdlma_bufferedsequentialallocator
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdlma::LocalSequentialAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol to very efficiently allocate
// heterogeneous memory blocks (of varying, user-specified sizes) from a local
// buffer.  Note that it derives from 'bdlma::BufferedSequentialAllocator' so
// that the implementations of 'allocate', 'deallocate', and 'release' don't
// need to be instantiated for each user-specified size.
//..
//    ,-------------------------------.
//   ( bdlma::LocalSequentialAllocator )
//    `-------------------------------'
//                    |        ctor
//                    V
//   ,----------------------------------.
//  ( bdlma::BufferedSequentialAllocator )
//   `----------------------------------'
//                    |        ctor/dtor
//                    |        allocate
//                    |        deallocate
//                    |        release
//                    V
//        ,-----------------------.
//       ( bdlma::ManagedAllocator )
//        `-----------------------'
//                    |        release = 0
//                    V
//           ,----------------.
//          ( bslma::Allocator )
//           `----------------'
//                             allocate = 0
//                             deallocate = 0
//..
// If an allocation request exceeds the remaining free memory space in the
// local buffer, the allocator will fall back to a sequence of
// dynamically-allocated buffers.  The 'release' method releases all memory
// allocated through the allocator, as does the destructor.  Note that, even
// though a 'deallocate' method is available, it has no effect: individually
// allocated memory blocks cannot be separately deallocated.
//
// 'bdlma::LocalSequentialAllocator' is typically used when users have a
// reasonable estimation of the amount of memory needed.  This amount of memory
// would then be created directly on the program stack, and used as the local
// buffer by this allocator for very fast memory allocation.  Whilst the buffer
// has sufficient capacity, memory allocations will not trigger *any* dynamic
// memory allocation, will have optimal locality of reference, and will not
// require deallocation upon destruction.
//
// Once the local buffer is exhausted, subsequent allocation requests require
// dynamic memory allocation, and the performance of the allocator degrades.
//
// The main difference between a 'bdlma::LocalSequentialAllocator' and a
// 'bdlma::BufferedSequentialAllocator' is that this class internally maintains
// a buffer, rather than being given one at construction time.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Recommended Usage
/// - - - - - - - - - - - - - -
// Suppose we have a function which takes a map of items to update in some
// database:
//..
//  typedef bsl::string DatabaseKey;
//  typedef bsl::string DatabaseValue;
//
//  void updateRecords_1(const bsl::map<DatabaseKey, DatabaseValue>& values)
//  {
//      for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
//               it = values.begin(), end = values.end();
//           it != end;
//           ++it) {
//          bsl::stringbuf stringBuf;
//          bsl::ostream   ostr(&stringBuf);
//
//          ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
//                  "myKey = '" << it->second << "'";
//
//          // execute query using 'stringBuf.str()'
//      }
//  }
//..
// We call this method a lot, and after profiling, we notice that it's
// contributing a significant proportion of time, due to the allocations it is
// making.  We decide to see whether a LocalSequentialAllocator would help.
//
// First, use a 'bslma::TestAllocator' to track the typical memory usage:
//..
//  void updateRecords_2(const bsl::map<DatabaseKey, DatabaseValue>& values)
//  {
//      bslma::TestAllocator ta;
//
//      for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
//               it = values.begin(), end = values.end();
//           it != end;
//           ++it) {
//          bsl::stringbuf stringBuf(&ta);
//          bsl::ostream   ostr(&stringBuf);
//
//          ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
//                  "myKey = '" << it->second << "'";
//
//          // execute query using 'stringBuf.str()'
//
//          bsl::cout << "In use: " << ta.numBytesInUse() << '\n';
//      }
//
//      bsl::cout << "Max: " << ta.numBytesMax() << '\n';
//  }
//..
// Then we run our program again, and observe the following output:
//..
//  In use: 77
//  In use: 77
//  In use: 77
//  In use: 77
//  In use: 77
//  Max: 129
//..
// It looks like 129 is a good choice for the size of our allocator, so we go
// with that:
//..
//  void updateRecords_3(const bsl::map<DatabaseKey, DatabaseValue>& values)
//  {
//      bdlma::LocalSequentialAllocator<129> lsa;
//
//      for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
//               it = values.begin(), end = values.end();
//           it != end;
//           ++it) {
//          lsa.release();
//
//          bsl::stringbuf stringBuf(&lsa);
//          bsl::ostream   ostr(&stringBuf);
//
//          ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
//                  "myKey = '" << it->second << "'";
//
//          // execute query using 'stringBuf.str()'
//      }
//  }
//..
// Note that we release at the end of every iteration, as the deallocate method
// is a no-op, so without this, subsequent memory would be allocated from the
// default allocator (or the allocator passed to 'bsa' at construction).
//
// Finally, we re-profile our code to determine whether the addition of a
// 'LocalSequentialAllocator' helped.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALALLOCATOR
#include <bdlma_bufferedsequentialallocator.h>
#endif

namespace BloombergLP {
namespace bslma { class Allocator; }
namespace bdlma {

                      // ==============================
                      // class LocalSequentialAllocator
                      // ==============================

template <int t_SIZE>
class LocalSequentialAllocator : public BufferedSequentialAllocator {
    // This class implements the 'ManagedAllocator' protocol to provide a fast
    // allocator that dispenses heterogeneous blocks of memory (of varying,
    // user-specified sizes) from a local buffer whose capacity is the
    // specified 't_SIZE' (in bytes).  If an allocation request exceeds the
    // remaining free memory space in the local buffer, memory will be supplied
    // by an (optional) allocator supplied at construction; if no allocator is
    // supplied, the currently installed default allocator is used.  This class
    // is *exception* *neutral*; if memory cannot be allocated, the behavior is
    // defined by the (optional) allocator supplied at construction.

    // PRIVATE TYPES
    typedef BufferedSequentialAllocator                          AllocatorBase;
    typedef bsls::AlignmentToType<
                  bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT>::Type AlignmentType;

    // DATA
    union {
        // This anonymous union is 'bsls::AlignedBuffer', but typed out again
        // so that extra template instantiations are avoided.
        char          d_buffer[t_SIZE];
        AlignmentType d_align;
    };

  private:
    // NOT IMPLEMENTED
    //! LocalSequentialAllocator(const LocalSequentialAllocator&) = delete;
    //! LocalSequentialAllocator& operator=(
    //                               const LocalSequentialAllocator&) = delete;

  public:
    // CREATORS
    explicit LocalSequentialAllocator(bslma::Allocator *basicAllocator = 0);
        // Create a local sequential allocator for allocating memory blocks
        // from a local buffer having the specified 't_SIZE' (in bytes).
        // Optionally specify a 'basicAllocator' used to supply memory should
        // the capacity of the local buffer be exhausted.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.

    //! virtual ~LocalSequentialAllocator() = default;
        // Destroy this local sequential allocator.  All memory allocated from
        // this allocator is released.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // ------------------------------
                      // class LocalSequentialAllocator
                      // ------------------------------

// CREATORS
template <int t_SIZE>
inline
LocalSequentialAllocator<t_SIZE>::LocalSequentialAllocator(
                                              bslma::Allocator *basicAllocator)
: AllocatorBase(this->d_buffer, t_SIZE, basicAllocator)
{
}

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
