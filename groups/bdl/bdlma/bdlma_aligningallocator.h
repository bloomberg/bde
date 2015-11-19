// bdlma_aligningallocator.h                                          -*-C++-*-

#ifndef INCLUDED_BDLMA_ALIGNINGALLOCATOR
#define INCLUDED_BDLMA_ALIGNINGALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a wrapper to ensure fixed minimum alignment allocation.
//
//@CLASSES:
//  bdlma::AligningAllocator: wrapper to align memory allocation.
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an implementation,
// 'bdlma::AligningAllocator', of the base-level protocol (pure abstract
// interface) class, 'bslma::Allocator', providing the ability to allocate raw
// memory with an alignment specified at construction of the allocator.  The
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
// The 'AligningAllocator' is pass a pointer to another allocator at
// construction, which is held but not owned by the aligning allocator.  The
// held allocator must use either the maximum alignment or the natural
// alignment strategy (see 'bsls_alignment').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::AligningAllocator'
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to store a linked list of null-terminated strings in
// dynamically allocated memory, and we want to use a buffered sequential
// allocator for allocation.
//
// First, we define the nodes of the list -- each node containing a pointer to
// the next element, with the string, which may have any length, immediately
// following the pointer.
//..
//  struct Node {
//      Node *d_next;
//      char  d_string[1];
//  };
//..
// Note that on a 32-bit platform, the pointer must be 4-byte aligned.  We
// could configurre the buffered sequential allocator for max alignment, but
// this would result in an 8-byte alignment, wasting memory.
//
// Instead, we configure the buffered sequential allocator for natural
// alignment (the default), and use an aligning allocator to wrap it, ensuring
// that the memory will be aligned to 'sizeof(Node *)'.
//
// Then, we define our allocators. we configure the buffered sequential
// allocator for natural alignment (the default), and use an aligning allocator
// to wrap it, ensuring that the memory will be aligned to accomodate 'Node'.
//..
//  enum { k_ALIGNMENT = bsls::AlignmentFromType<Node>::VALUE };
//
//  static char                        buffer[4 * 1024];
//  bdlma::BufferedSequentialAllocator bsa(buffer, k_BUFFER_SIZE);
//  bdlma::AligningAllocator           aa(k_ALIGNMENT, &bsa);
//..
// Next, we define some strings we could like to store in the list:
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
//      "The best time to make friends is before you need them.\n" };
//  enum { k_NUM_STRINGS = sizeof strings / sizeof *strings };
//..
// Now, we store the strings in the list, verifying that they are properly
// aligned.
//..
//  Node *head = 0;
//  for (int ii = 0; ii < k_NUM_STRINGS; ++ii) {
//      const char *str = strings[ii];
//      bsl::size_t len = bsl::strlen(str);
//      Node *newNode = static_cast<Node *>(aa.allocate(
//                                                  sizeof(Node *) + len + 1));
//      assert(0 ==
//               (reinterpret_cast<bsl::size_t>(newNode) & (k_ALIGNMENT - 1)));
//
//      bsl::strcpy(newNode->d_string, str);
//
//      newNode->d_next = head;
//      head = newNode;
//  }
//..
// Finally, we traverse our linked list and print out the strings:
//..
//  for (const Node *node = head; node; node = node->d_next) {
//      cout << node->d_string;
//  }
//..


#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlma {

                          // ======================
                          // class AlignedAllocator
                          // ======================

class AligningAllocator : public bslma::Allocator {
    // This 'class' provides a mechanism that serves as a wrapper around
    // another allocator, passed at construction.  The mechanism guarantees
    // that all allocations passed to the underlying allocator will be aligned
    // by the alignment specified at construction.

    // DATA
    size_type         d_mask;                // alignment - 1
    bslma::Allocator *d_heldAllocator_p;     // allocator passed at
                                             // construction

  public:
    // CREATORS
    explicit
    AligningAllocator(size_type         alignment,
                      bslma::Allocator *allocator = 0);
        // Create an 'AligningAllocator' object that will guarantee alignment
        // by the specified 'alignment' using the optionally specified
        // 'allocator'.  If 'alignment' is greaterr than
        // 'bsls::Alignment::MAX_ALIGNMENT', the alignment will be rounded down
        // to max alignment.  The behavior is undefined unless 'alignment' is
        // greater than 0 and a power of 2, and unless the alignment strategy
        // of 'allocator' is 'BSLS_MAXIMUM' or 'BSLS_NATURAL'.  If no
        // 'allocator' is passed, the currently installed default alloctor is
        // used.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Allocate a segment of at least the specified 'size' bytes in length
        // using the held allocator, guaranteeing that the newly allocated
        // block will be properly aligned.

    virtual void deallocate(void *address);
        // Deallocate the segment pointed to by the specified 'address'.
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
