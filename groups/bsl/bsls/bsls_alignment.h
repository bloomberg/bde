// bsls_alignment.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_ALIGNMENT
#define INCLUDED_BSLS_ALIGNMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for enumerating memory alignment strategies.
//
//@CLASSES:
//  bsls::Alignment: namespace for enumerated alignment strategy values
//
//@SEE_ALSO: bsls_alignmentutil
//
//@DESCRIPTION: This component provides a namespace, 'bsls::Alignment', for
// enumerating alignment strategies, and provides a function, 'toAscii', that
// converts each of the enumerators to its corresponding string representation.
//
///Alignment Strategy
///------------------
// This component supports three alignment strategies: 1) MAXIMUM ALIGNMENT,
// 2) NATURAL ALIGNMENT, and 3) 1-BYTE ALIGNMENT.
//
//: 1 MAXIMUM ALIGNMENT: This strategy, as indicated by the enumerator
//:   'BSLS_MAXIMUM', specifies that a memory block be aligned as per the
//:   *most* restrictive alignment requirement on the host platform.
//:
//: 2 NATURAL ALIGNMENT: This strategy, as indicated by the enumerator
//:   'BSLS_NATURAL', specifies that a memory block be aligned based on the
//:   size (in bytes) of that block.  An object of a fundamental type ('int',
//:   etc.) is *naturally* *aligned* when it's size evenly divides its address.
//:   An object of an aggregate type has natural alignment if the alignment of
//:   the most-restrictively aligned sub-object evenly divides the address of
//:   the aggregate.  Natural alignment is always at least as restrictive as
//:   the compiler's required alignment.
//:
//: 3 1-BYTE ALIGNMENT: This strategy, as indicated by the enumerator
//:   'BSLS_BYTEALIGNED', specifies that a memory block may be aligned
//:   arbitrarily on any 1-byte boundary.  This is the *least* restrictive
//:   alignment requirement.
//
///Usage
///-----
// Suppose that we want to create a static function, 'allocateFromBuffer', that
// takes a buffer, the size of the buffer, a cursor indicating a position
// within the buffer, an allocation request size, and a memory alignment
// strategy; 'allocateFromBuffer' returns a pointer to a block of memory,
// wholly contained within the buffer, having the specified size and alignment.
// As a side-effect, the cursor is updated to refer to the next available free
// byte in the buffer.  Such a function could be used by a memory manager to
// satisfy allocation requests from internally-managed buffers.  Clients of
// this function indicate which alignment strategy to use based on their
// specific requirements.
//
// Our 'allocateFromBuffer' function depends on an alignment utility,
// 'my_AlignmentUtil', whose minimal interface is limited to that required by
// this usage example.  (See the 'bsls_alignmentutil' component for a more
// realistic alignment utility.):
//..
//  struct my_AlignmentUtil {
//      // This 'struct' provides a namespace for basic types and utilities
//      // related to memory alignment.
//
//      // TYPES
//      enum {
//          MY_MAX_PLATFORM_ALIGNMENT = 8
//      };
//          // Provide the *minimal* value that satisfies the alignment
//          // requirements for *all* types on the host platform.  Note that 8
//          // is used for illustration purposes only; an actual implementation
//          // would employ template meta-programming to deduce the value at
//          // compile time.
//
//      // CLASS METHODS
//      static int calculateAlignmentFromSize(int size);
//          // Calculate a usable alignment for a memory block of the specified
//          // 'size' (in bytes) in the absence of compile-time knowledge of
//          // the block's alignment requirements.  Return the largest power of
//          // two that evenly divides 'size', up to a maximum of
//          // 'MY_MAX_PLATFORM_ALIGNMENT'.  It is guaranteed that a block of
//          // 'size' bytes can be safely aligned on the return value.   The
//          // behavior is undefined unless '0 < size'.
//
//      static int calculateAlignmentOffset(const void *address,
//                                          int         alignment);
//          // Return the smallest non-negative offset (in bytes) that, when
//          // added to the specified 'address', yields the specified
//          // 'alignment'.  The behavior is undefined unless '0 != alignment'
//          // and 'alignment' is a non-negative, integral power of 2.
//  };
//..
// The definition of our 'allocateFromBuffer' function is as follows:
//..
//  static void *allocateFromBuffer(int                       *cursor,
//                                  char                      *buffer,
//                                  int                        bufferSize,
//                                  int                        size,
//                                  bsls::Alignment::Strategy  strategy)
//      // Allocate a memory block of the specified 'size' (in bytes) from the
//      // specified 'buffer' having the specified 'bufferSize' at the
//      // specified 'cursor' position, using the specified alignment
//      // 'strategy'.  Return the address of the allocated memory block if
//      // 'buffer' contains sufficient available memory, and 0 otherwise.  The
//      // 'cursor' is set to the first byte position immediately after the
//      // allocated memory (which might be 1 byte past the end of 'buffer') if
//      // there is sufficient memory, and is not modified otherwise.  The
//      // behavior is undefined unless '0 <= bufferSize', '0 < size', and
//      // 'cursor' refers to a valid position in 'buffer'.
//  {
//..
// First we assert the function pre-conditions:
//..
//      assert(cursor);
//      assert(buffer);
//      assert(0 <= bufferSize);
//      assert(0 < size);
//..
// Then, based on the alignment 'strategy', we calculate the alignment value
// that can satisfy the allocation request.  In the case of
// 'bsls::Alignment::BSLS_NATURAL', we calculate the alignment from 'size'; for
// 'bsls::Alignment::BSLS_MAXIMUM', we use the platform-dependent
// 'my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT' value; and for
// 'bsls::Alignment::BSLS_BYTEALIGNED', we simply use 1:
//..
//      const int alignment =
//                         strategy == bsls::Alignment::BSLS_NATURAL
//                         ? my_AlignmentUtil::calculateAlignmentFromSize(size)
//                         : strategy == bsls::Alignment::BSLS_MAXIMUM
//                           ? my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT
//                           : 1;
//..
// Now we calculate the offset from the current 'cursor' value that can satisfy
// the 'alignment' requirements:
//..
//      const int offset = my_AlignmentUtil::calculateAlignmentOffset(
//                                                            buffer + *cursor,
//                                                            alignment);
//
//..
// Next we check if the available free memory in 'buffer' can satisfy the
// allocation request; 0 is returned if the request cannot be satisfied:
//..
//      if (*cursor + offset + size > bufferSize) {
//          return 0;                                                 // RETURN
//      }
//
//      void *result = &buffer[*cursor + offset];
//      *cursor += offset + size;
//
//..
// Finally, return the address of the correctly aligned memory block:
//..
//      return result;
//  }
//..
// The 'allocateFromBuffer' function may be used by a memory manager that needs
// to appropriately align memory blocks that are allocated from
// internally-managed buffers.  For an example, see the 'bslma_bufferimputil'
// component.


namespace BloombergLP {

namespace bsls {

                        // ================
                        // struct Alignment
                        // ================

struct Alignment {
    // This 'struct' provides a namespace for enumerating the set of strategies
    // for aligning arbitrary blocks of memory.

    // TYPES
    enum Strategy {
        // Types of alignment strategy.

        BSLS_MAXIMUM = 0,
            // Align memory block based on the most restrictive alignment
            // requirements of the host platform.

        BSLS_NATURAL = 1,
            // Align memory block on an address that is the largest power of
            // two that evenly divides the size (in bytes) of the block.

        BSLS_BYTEALIGNED = 2
            // Align memory block based on the least restrictive alignment
            // requirements of the host platform (1-byte aligned).
    };


    // CLASS METHODS
    static const char *toAscii(Alignment::Strategy value);
        // Return the string representation of the specified enumerator
        // 'value'.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BSLS_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << Alignment::toAscii(Alignment::BSLS_NATURAL);
        //..
        // will print the following on standard output:
        //..
        //  NATURAL
        //..

};

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
