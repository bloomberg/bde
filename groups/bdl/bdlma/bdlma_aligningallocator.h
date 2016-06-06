// bdlma_aligningallocator.h                                          -*-C++-*-

#ifndef INCLUDED_BDLMA_ALIGNINGALLOCATOR
#define INCLUDED_BDLMA_ALIGNINGALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide an allocator-wrapper to allocate with a minimum alignment.
//
//@CLASSES:
//  bdlma::AligningAllocator: wrapper to align memory allocation
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a concrete allocator implementation,
// 'bdlma::AligningAllocator', providing the ability to allocate memory with a
// minimum alignment specified at the construction of the allocator.  The
// following inheritance diagram shows the classes involved and their methods:
//..
//   ,------------------------.
//  ( bdlma::AligningAllocator )
//   `------------------------'
//               |
//               V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                       allocate
//                       deallocate
//..
// The 'AligningAllocator' is supplied an allocator at construction, and
// ensures that memory returned by 'allocate' meets a minimum alignment
// requirement.  This may be useful in situations where a user needs to adapt
// an allocator supplying only natural alignment to software expecting an
// allocator with a higher alignment guarantee.  The allocator supplied to an
// 'AligningAllocator' at construction is held, not owned.
//
// The allocator supplied to the aligning allocator must employ at least the
// natural alignment strategy (see bsls_alignment).  Specifically, the aligning
// allocator will fail to align the memory if the allocator passed to it
// employs the 1-Byte alignment strategy.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::AligningAllocator'
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we are dealing with an externally supplied library function that
// creates a linked list of null-terminated strings, and we want to use a
// default-constructed buffered sequential allocator for memory allocation.
//
// First, the externally supplied library defines the 'struct' describing a
// node in the linked list:
//..
//  struct Node {
//      // This 'struct' describes one node in a linked list containing
//      // strings.
//
//      Node *d_next;
//      char  d_string[1];
//
//      // CLASS METHODS
//      static
//      bsl::size_t sizeNeededForString(const char *string)
//          // Return the size in bytes needed to store a 'Node' containing a
//          // copy of the specified 'string'.
//      {
//          Node node;
//
//          return sizeof(node.d_next) + bsl::strlen(string) + 1;
//      }
//  };
//..
// Then, the externally-supplied library defines the function that will create
// the linked list of nodes from a null-terminated array of pointers to
// C-strings:
//..
//  void externalPopulateStringList(Node             **head,
//                                  const char       **stringArray,
//                                  bslma::Allocator  *allocator)
//      // Create a linked list of strings beginning with the specified '*head'
//      // containing the null-terminated strings from the null-terminated
//      // 'stringArray'.  Use the specified 'allocator' to supply memory.
//  {
//      *head = 0;
//      const char *string;
//      for (int ii = 0; 0 != (string = stringArray[ii]); ++ii) {
//          Node *newNode = static_cast<Node *>(allocator->allocate(
//                                         Node::sizeNeededForString(string)));
//          bsl::strcpy(newNode->d_string, string);
//
//          newNode->d_next = *head;
//          *head = newNode;
//      }
//  }
//..
// Next, the externally-supplied buffered sequential allocator that we are to
// use:
//..
//  enum { k_BUFFER_SIZE = 4 * 1024 };
//  char                               buffer4k[k_BUFFER_SIZE];
//  bdlma::BufferedSequentialAllocator bsAlloc(buffer4k, k_BUFFER_SIZE);
//..
// There is a problem here, in that the nodes must be aligned by
// 'sizeof(Node *)', but our buffered sequential allocator, like most BDE
// allocators, has a "natural alignment" strategy (see bsls_alignment), meaning
// that it infers the required alignment from the size of the allocation
// requested.  This would normally give us properly aligned memory if we were
// allocating by 'sizeof(struct)', but our 'Node' objects are variable length,
// which will mislead the allocator to sometimes align the new segments by less
// than 'sizeof(Node *)'.
//
// Then, we solve this problem by using an aligning allocator to wrap the
// buffered sequential allocator, ensuring that the memory will still come from
// the buffered sequential allocator, but nonetheless be aligned to the
// alignment requirement of 'Node'.
//..
//  enum { k_ALIGNMENT = bsls::AlignmentFromType<Node>::VALUE };
//  bdlma::AligningAllocator aligningAllocator(k_ALIGNMENT, &bsAlloc);
//..
// Next, we define a null-terminated array of strings we would like to store in
// the list:
//..
//  const char *strings[] = {
//      "A zinger is not a rebuttal.\n",
//      "Humor is mankind's greatest blessing.\n",
//      "As usual, the facts don't care about our feelings.\n",
//      "Criticism is the only antidote to error.\n",
//      "If you can't annoy somebody, there is little point in writing.\n",
//      "Maybe all one can do is hope to end up with the right regrets.\n",
//      "People may hear your words, but they feel your attitude.\n",
//      "Imagination is a poor substitute for experience.\n",
//      "We wanted a labor force, but human beings came.\n",
//      "The reward of a thing well done is to have done it.\n",
//      "Chance fights ever on the side of the prudent.\n",
//      "The best time to make friends is before you need them.\n",
//      0  };
//..
// Now, we call the function to put the strings into a linked list, passing it
// the aligning allocator:
//..
//  Node *head = 0;
//  externalPopulateStringList(&head, strings, &aligningAllocator);
//..
// Finally, we traverse the list and print out the strings, verifying that the
// nodes are properly aligned:
//..
//  for (const Node *node = head; node; node = node->d_next) {
//      assert(0 == (reinterpret_cast<bsl::size_t>(node) & (k_ALIGNMENT - 1)));
//
//      cout << node->d_string;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlma {

                         // =======================
                         // class AligningAllocator
                         // =======================

class AligningAllocator : public bslma::Allocator {
    // This 'class' provides a mechanism that serves as a wrapper around
    // another allocator, passed at construction.  The mechanism guarantees
    // that all allocations passed to the underlying allocator will be aligned
    // by the alignment specified at construction.

    // DATA
    bsls::Types::size_type  d_mask;             // alignment - 1

    bslma::Allocator       *d_heldAllocator_p;  // allocator passed at
                                                // construction

  public:
    // CREATORS
    explicit
    AligningAllocator(bsls::Types::size_type  alignment,
                      bslma::Allocator       *allocator = 0);
        // Create an 'AligningAllocator' object that guarantees alignment by
        // the specified 'alignment' of memory allocated.  Optionally specify
        // 'allocator', the underlying allocator to be used for memory
        // allocation.  If 'alignment' is greater than max alignment
        // ('bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'), the alignment will be
        // rounded down to max alignment.  The behavior is undefined unless
        // 'alignment' is greater than 0 and a power of 2, unless the alignment
        // strategy of 'allocator' is 'BSLS_MAXIMUM' or 'BSLS_NATURAL'.  If no
        // 'allocator' is passed, the currently installed default allocator is
        // used.

    // MANIPULATORS
    virtual void *allocate(bsls::Types::size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If 'size' is 0, a null pointer is
        // returned with no other effect.  If this allocator cannot return the
        // requested number of bytes, then it will throw a 'std::bad_alloc'
        // exception in an exception-enabled build, or else will abort the
        // program in a non-exception build.  The behavior is undefined unless
        // '0 <= size'.  Note that the alignment of the address returned
        // conforms to the platform requirement for any object of size 'size'
        // and is also guaranteed to be at least the alignment passed to the
        // constructor.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
