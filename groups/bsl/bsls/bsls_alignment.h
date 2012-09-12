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
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace, 'bsls::Alignment', for
// enumerating alignment strategies, and provides a function, 'toAscii', that
// converts each of the enumerators to its corresponding string representation.
//
// *Note:* This component has been factored into multiple components (it is now
// simply an enumeration); types and values previously provided are now defined
// in other components.  Specifically, clients using the previously named
// 'bsls_AlignmentOf', 'bsls::AlignmentToType', and 'bsls::Alignment' types
// should now use the components 'bsls_alignmentfromtype',
// 'bsls_alignmenttotype', and 'bsls_alignmentutil', respectively.  Similarly,
// the enumerator value 'bsls::Alignment::MAX_ALIGNMENT' and the
// 'bsls::Alignment::MaxAlignedType' 'typedef' are now provided by
// 'bsls_alignmentutil'.
//
///Alignment Strategy
///------------------
// This component supports two alignment strategies: 1) MAXIMUM ALIGNMENT and
// 2) NATURAL ALIGNMENT.
//..
//  MAXIMUM ALIGNMENT: This strategy, as indicated by the enumerator
//  'BSLS_MAXIMUM', specifies that a memory block be aligned as per the most
//  restrictive alignment requirement on the host platform.
//
//  NATURAL ALIGNMENT: This strategy, as indicated by the enumerator
//  'BSLS_NATURAL', specifies that a memory block be aligned based on the
//  size (in bytes) of that block.  An object of a fundamental type ('int',
//  etc.) is *naturally* *aligned* when it's size evenly divides its
//  address.  An object of an aggregate type has natural alignment if the
//  alignment of the most-restrictively aligned sub-object evenly divides
//  the address of the aggregate.  Natural alignment is always at least as
//  restrictive as the compiler's required alignment.
//..
///Usage
///-----
// Suppose that we want to create a static function, 'allocateFromBuffer', that
// takes a buffer, the size of the buffer, a cursor indicating a position
// within the buffer, an allocation request size, and a memory alignment
// strategy; 'allocateFromBuffer' returns a pointer to a block of memory,
// wholly contained within the buffer, having the specified size and alignment.
// As a side-effect, the cursor is updated to refer to the next available free
// byte in the buffer.  Such a function could be used by a memory manager to
// satisfy allocate requests from internally-managed buffers.  Clients of this
// function indicate which alignment strategy to use based on their specific
// requirements.
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
//  static void *allocateFromBuffer(int                      *cursor,
//                                  char                     *buffer,
//                                  int                       bufferSize,
//                                  int                       size,
//                                  bsls::Alignment::Strategy strategy)
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
//  assert(cursor);
//  assert(buffer);
//  assert(0 <= bufferSize);
//  assert(0 < size);
//..
// Then, based on the alignment 'strategy', we calculate the alignment value
// that can satisfy the allocation request.  In the case of
// 'bsls::Alignment::BSLS_NATURAL', we calculate the alignment from 'size'; for
// 'bsls::Alignment::BSLS_MAXIMUM', we use the platform-dependent
// 'my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT' value:
//..
//  const int alignment =
//                     strategy == bsls::Alignment::BSLS_NATURAL
//                     ? my_AlignmentUtil::calculateAlignmentFromSize(size)
//                     : my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT;
//..
// Now we calculate the offset from the current 'cursor' value that can satisfy
// the 'alignment' requirements:
//..
//  const int offset = my_AlignmentUtil::calculateAlignmentOffset(
//                                                        buffer + *cursor,
//                                                        alignment);
//
//..
// Next we check if the available free memory in 'buffer' can satisfy the
// allocation request; 0 is returned if the request cannot be satisfied:
//..
//  if (*cursor + offset + size > bufferSize) {
//      return 0;                                                 // RETURN
//  }
//
//  void *result = &buffer[*cursor + offset];
//  *cursor += offset + size;
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTIMP
#include <bsls_alignmentimp.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTTOTYPE
#include <bsls_alignmenttotype.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>      // for std::size_t
#define INCLUDED_CSTDDEF
#endif

// Temporarily define the legacy 'bsls_AlignmentOf' to refer to its
// replacement, 'bsls::AlignmentFromType'.

#ifndef bsls_AlignmentOf
#define bsls_AlignmentOf bsls::AlignmentFromType
#endif

#endif

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

        BSLS_NATURAL = 1
            // Align memory block on an address that is the largest power of
            // two that evenly divides the size (in bytes) of the block.
    };

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    enum {
        // Define the minimum alignment that satisfies all types.
        //
        // DEPRECATED: This 'enum' is deprecated.  Use
        // 'AlignmentUtil::BSLS_MAX_ALIGNMENT instead.

        MAX_ALIGNMENT = AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    typedef AlignmentUtil::MaxAlignedType MaxAlignedType;
        // Primitive type with most stringent alignment requirement.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::MaxAlignedType'.

    struct Align {
        // 'struct' with most stringent alignment requirement.
        //
        // DEPRECATED: Replaced by 'Alignment::MaxAlignedType'.

        MaxAlignedType d_align;
    };
#endif

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static int calculateAlignmentFromSize(int size);
        // Calculate a usable alignment for an object of specified 'size' bytes
        // in the absence of compile-time knowledge of the object's alignment
        // requirements.  Return the largest power of two that evenly divides
        // size, up to a maximum of 'MAX_ALIGNMENT'.  It is guaranteed that an
        // object of 'size' bytes can be safely aligned on this return value.
        // Depending on the machine architecture and compiler options, the
        // returned alignment may be more than required for two reasons:
        //..
        //    1. The object may be composed entirely of elements, such as
        //       'char', which have minimal alignment restrictions.
        //
        //    2. The architecture may have lenient alignment requirements.
        //       Even if this is the case, aligning on a stricter boundary may
        //       improve performance.
        //..
        // The behavior is undefined unless '0 < size'.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::calculateAlignmentFromSize'.

    static int calculateAlignmentOffset(const void *address, int alignment);
        // Return the smallest non-negative offset in bytes that, when added to
        // the specified 'address', yields the specified 'alignment'.  The
        // behavior is undefined unless '0 < alignment' and 'alignment' is a
        // power of 2.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::calculateAlignmentOffset'.

    static bool is2ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on a 2-byte
        // boundary (i.e., the integer value of 'address' is divisible by 2),
        // and 'false' otherwise.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::is2ByteAligned'.

    static bool is4ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on a 4-byte
        // boundary (i.e., the integer value of 'address' is divisible by 4),
        // and 'false' otherwise.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::is4ByteAligned'.

    static bool is8ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on an 8-byte
        // boundary (i.e., the integer value of 'address' is divisible by 8),
        // and 'false' otherwise.
        //
        // DEPRECATED: Replaced by 'AlignmentUtil::is8ByteAligned'.
#endif
};

}  // close package namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bsls {

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ----------------
                            // struct Alignment
                            // ----------------

// CLASS METHODS
inline
int Alignment::calculateAlignmentFromSize(int size)
{
    return AlignmentUtil::calculateAlignmentFromSize(size);
}

inline
int Alignment::calculateAlignmentOffset(const void *address,
                                        int         alignment)
{
    return AlignmentUtil::calculateAlignmentOffset(address, alignment);
}

inline
bool Alignment::is2ByteAligned(const void *address)
{
    return AlignmentUtil::is2ByteAligned(address);
}

inline
bool Alignment::is4ByteAligned(const void *address)
{
    return AlignmentUtil::is4ByteAligned(address);
}

inline
bool Alignment::is8ByteAligned(const void *address)
{
    return AlignmentUtil::is8ByteAligned(address);
}

}  // close package namespace

#endif

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bsls::Alignment bsls_Alignment;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
