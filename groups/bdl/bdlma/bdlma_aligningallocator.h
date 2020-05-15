// bdlma_aligningallocator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLMA_ALIGNINGALLOCATOR
#define INCLUDED_BDLMA_ALIGNINGALLOCATOR

#include <bsls_ident.h>
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
//               |       ctor/dtor
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
// natural alignment strategy (see {'bsls_alignment'}).  Specifically, the
// aligning allocator will fail to properly align memory if the allocator
// passed to it employs the 1-byte alignment strategy.
//
// Note that in order to provide the requested alignment an 'AligningAllocator'
// will need to always overallocate memory from the underlying allocator.  This
// can "waste" space equal to the alignment plus some extra bytes for
// bookkeeping information.  For small allocations with large alignment values
// this waste can be very impactful.  In general, this means this component
// should be used with small alignments or with larger allocations.
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
//                                     Node::sizeNeededForString(string)));
//          assert(newNode != 0);
//
//          bsl::strcpy(newNode->d_string, string);
//
//          newNode->d_next = *head;
//          *head = newNode;
//      }
//  }
//..
// Next, our example function will begin with the externally-supplied buffered
// sequential allocator that we are to use:
//..
//  void printFromNodes(bsl::ostream *out) {
//      enum { k_BUFFER_SIZE = 4 * 1024 };
//      char                               buffer4k[k_BUFFER_SIZE];
//      bdlma::BufferedSequentialAllocator bsAlloc(buffer4k, k_BUFFER_SIZE);
//..
// There is a problem here, in that the nodes must be aligned by
// 'sizeof(Node *)', but our buffered sequential allocator, like most BDE
// allocators, has a "natural alignment" strategy (see {'bsls_alignment'}),
// meaning that it infers the required alignment from the size of the
// allocation requested.  This would normally give us properly aligned memory
// if we were allocating by 'sizeof(Node)', but our 'Node' objects are variable
// length, which will mislead the allocator to sometimes align the new segments
// by less than 'sizeof(Node *)'.
//
// Then, we solve this problem by using an aligning allocator to wrap the
// buffered sequential allocator, ensuring that the memory will still come from
// the buffered sequential allocator, but nonetheless be aligned to the
// alignment requirement of 'Node'.
//..
//      enum { k_ALIGNMENT = bsls::AlignmentFromType<Node>::VALUE };
//      bdlma::AligningAllocator aligningAllocator(k_ALIGNMENT, &bsAlloc);
//..
// Next, we define a null-terminated array of strings we would like to store in
// the list:
//..
//      const char *strings[] = {
//          "A zinger is not a rebuttal.\n",
//          "Humor is mankind's greatest blessing.\n",
//          "As usual, the facts don't care about our feelings.\n",
//          "Criticism is the only antidote to error.\n",
//          "If you can't annoy somebody, there is little point in writing.\n",
//          "Maybe all one can do is hope to end up with the right regrets.\n",
//          "People may hear your words, but they feel your attitude.\n",
//          "Imagination is a poor substitute for experience.\n",
//          "We wanted a labor force, but human beings came.\n",
//          "The reward of a thing well done is to have done it.\n",
//          "Chance fights ever on the side of the prudent.\n",
//          "The best time to make friends is before you need them.\n",
//          0  };
//..
// Now, we call the function to put the strings into a linked list, passing it
// the aligning allocator:
//..
//      Node *head = 0;
//      externalPopulateStringList(&head, strings, &aligningAllocator);
//..
// Finally, we traverse the list and print out the strings, verifying that the
// nodes are properly aligned:
//..
//      for (const Node *node = head; node; node = node->d_next) {
//          assert(0 == (reinterpret_cast<bsl::size_t>(node) &
//                       (k_ALIGNMENT - 1)));
//          if (out) {
//              *out << node->d_string;
//          }
//      }
//  }
//..

#include <bdlscm_version.h>

#include <bslma_allocator.h>

#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>

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

    bslma::Allocator       *d_allocator_p;      // allocator indicated at
                                                // construction

  private:
    // NOT IMPLEMENTED
    AligningAllocator(const AligningAllocator&) BSLS_KEYWORD_DELETED;
    AligningAllocator& operator=(
                                const AligningAllocator&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit AligningAllocator(bsls::Types::size_type  alignment,
                               bslma::Allocator       *basicAllocator = 0);
        // Create an 'AligningAllocator' object that guarantees alignment by
        // the specified 'alignment' (in bytes) of memory allocated.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or not supplied, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'alignment' is a power of 2, and the alignment strategy of the
        // alocator used to supply memory is 'BSLS_MAXIMUM' or 'BSLS_NATURAL'.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size) BSLS_KEYWORD_OVERRIDE;
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If 'size' is 0, a null pointer is
        // returned with no other effect.  If this allocator cannot return the
        // requested number of bytes, then it will throw a 'std::bad_alloc'
        // exception in an exception-enabled build, or else will abort the
        // program in a non-exception build.  Note that the alignment of the
        // address returned conforms to the platform requirement for any object
        // of size 'size' and is also guaranteed to be at least the alignment
        // passed to the constructor.

    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
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
