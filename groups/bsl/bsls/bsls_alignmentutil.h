// bsls_alignmentutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#define INCLUDED_BSLS_ALIGNMENTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide constants, types, and operations related to alignment.
//
//@CLASSES:
//  bsls::AlignmentUtil: namespace for alignment functions, types, and constants
//
//@SEE_ALSO: bslma_bufferimputil
//
//@DESCRIPTION: This component defines a 'struct', 'bsls::AlignmentUtil', which
// serves as a namespace for compile-time constants, types, and operations
// associated with alignment on the current platform.
//
// The 'BSLS_MAX_ALIGNMENT' enumerator provides the minimal value that
// satisfies the alignment requirements for all types on the current platform.
// Additionally, the 'MaxAlignedType' 'typedef' specifies a primitive type that
// requires an alignment of 'BSLS_MAX_ALIGNMENT'.
//
// The 'calculateAlignmentFromSize' function calculates a usable (but not
// necessarily minimal) alignment for any object with a memory footprint of the
// specified 'size' (in bytes).  The 'calculateAlignmentOffset' function
// calculates the offset from a specified 'address' where an object having a
// specified 'alignment' requirement can be constructed safely.  The
// 'is2ByteAligned', 'is4ByteAligned', and 'is8ByteAligned' functions each
// return 'true' if a specified 'address' is, respectively, 2-, 4-, or 8-byte
// aligned.  Finally, 'roundUpToMaximalAlignment' returns the smallest whole
// multiple of 'BSLS_MAX_ALIGNMENT' greater than or equal to its argument.
//
///Assumptions
///-----------
// The functionality in this component makes several assumptions:
//
//: o The alignment for any given type is an integral power of 2.
//:
//: o The required alignment for a type 'T' evenly divides 'sizeof(T)', which
//:   implies that the required alignment for 'T' is never larger than
//:   'sizeof(T)'.
//:
//: o The required alignment for a 'struct', 'class', or 'union' is the same as
//:   the required alignment of its most restrictive non-static data member
//:   (including the implicit virtual table pointer in polymorphic classes and
//:   internal pointers in virtually-derived classes).
//
///Usage
///-----
// A sequential memory allocator is used to return a sequence of memory blocks
// of varying requested sizes from a larger chunk of contiguous memory.  Each
// block returned must also have an alignment that is sufficient for any
// conceivable object of that size.  To achieve a fully factored
// implementation, we might choose to provide a low-level helper function
// 'naturallyAlign' that, given the 'address' of the next available byte in the
// larger chunk along with the requested block 'size' (in bytes), returns the
// first appropriately (or *naturally*) aligned address for the requested block
// at or after 'address':
//..
//  void *naturallyAlign(void **currentAddress, int size);
//      // Return the closest memory address at or after the specified
//      // '*currentAddress' that is sufficiently aligned to accommodate any
//      // object of the specified 'size', and update '*currentAddress' to
//      // refer to the first available byte after the allocated object.  The
//      // behavior is undefined unless '1 <= size'.
//..
// We can implement the 'naturallyAlign' helper function easily using the
// methods defined in this class:
//..
//  void *naturallyAlign(void **currentAddress, std::size_t size)
//  {
//      int   alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(
//                                                              size);
//      int   offset    = bsls::AlignmentUtil::calculateAlignmentOffset(
//                                                             *currentAddress,
//                                                              alignment);
//      char *result    = static_cast<char *>(*currentAddress) + offset;
//      *currentAddress = result + size;
//      return result;
//  }
//..
// We will then be able to use this 'naturallyAlign' helper function to
// allocate, from a buffer of contiguous memory, efficiently (but not
// necessarily optimally) aligned memory for objects of varying sizes based
// solely on the size of each object (i.e., determined by its natural, not
// actual, alignment).
//
// To illustrate the functionality provided in this component, we begin by
// assuming that we have some user-defined type, 'MyType', comprising several
// data members:
//..
//  class MyType {          // size 24; actual alignment 8; natural alignment 8
//      int     d_int;
//      double  d_double;   // Assume 8-byte alignment.
//      char   *d_charPtr;  // Assume size <= 8 bytes.
//  };
//..
// We then define a function, 'f', which starts off by creating a maximally
// aligned 'buffer' on the program stack:
//..
//  void f()
//  {
//      const int BUFFER_SIZE = 1024;
//      union {
//          bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. align.
//          char                                d_buffer[BUFFER_SIZE];
//      } buffer;
//..
// Next we use the 'bsls::AlignmentUtil' functions directly to confirm that
// 'buffer' is sufficiently aligned to accommodate a 'MaxAlignedType' object:
//..
//  int alignment = bsls::AlignmentFromType<
//                              bsls::AlignmentUtil::MaxAlignedType>::VALUE;
//  int offset    = bsls::AlignmentUtil::calculateAlignmentOffset(
//                                                         buffer.d_buffer,
//                                                         alignment);
//  assert(0 == offset);  // sufficient alignment
//..
// Below we perform various memory allocations using our 'naturallyAlign'
// helper function:
//..
//  void *p         = static_cast<void *>(buffer.d_buffer);
//
//  void *charPtr   = naturallyAlign(&p, sizeof(char));
//
//  void *shortPtr5 = naturallyAlign(&p, 5 * sizeof(short));
//..
// Note that the address held in 'shortPtr' is numerically divisible by the
// alignment of a 'short' on the current platform:
//..
//  assert(0 == ((static_cast<char *>(shortPtr5) - buffer.d_buffer) %
//                                  bsls::AlignmentFromType<short>::VALUE));
//
//  assert(bsls::AlignmentUtil::is2ByteAligned(shortPtr5));
//..
// Next we use 'naturallyAlign' to allocate a block of appropriate size and
// sufficient alignment to store a 'MyType' object:
//..
//  void *objPtr = naturallyAlign(&p, sizeof(MyType));
//..
// Note that the alignment of the address held in 'objPtr' is numerically
// divisible by the actual alignment requirement:
//..
//      assert(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
//                                    objPtr,
//                                    bsls::AlignmentFromType<MyType>::VALUE));
//  }
//..
// Assuming 'buffer' has sufficient capacity, and the alignments for 'char',
// 'short', and 'MyType' are, respectively, 1, 2, and 8, we would expect this
// layout within 'buffer.d_buffer':
//..
//  charPtr shortPtr5                                               objPtr
//  |       |                                                       |
//  V       V                                                       V
//  .---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.-
//  |ccc|   |sssssss:sssssss:sssssss:sssssss:sssssss|   :   :   :   |oooooo...
//  ^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^-
//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18
//..
// Note that on an atypical 32-bit platform where a 'double' is 4-byte aligned,
// the actual alignment of 'MyType' would be 4, but its natural alignment would
// still be 8 because its size would be 16; it is highly unlikely that 'MyType'
// would have an actual (and therefore natural) alignment of 4 on a 64-bit
// platform when using default compiler settings.

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTIMP
#include <bsls_alignmentimp.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTTOTYPE
#include <bsls_alignmenttotype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_LIMITS
#include <limits>           // 'std::numeric_limits'
#define INCLUDED_LIMITS
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>           // 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bsls {

                            // ====================
                            // struct AlignmentUtil
                            // ====================

struct AlignmentUtil {
    // This 'struct' provides a namespace for a suite of compile-time
    // constants, types, and pure procedures that provide platform-dependent
    // alignment information.

  private:
    // PRIVATE TYPES
    typedef void (*FuncPtr)();
        // Typedef used as an alias for a function pointer.

    union MaxAlignedUnion;
    friend union MaxAlignedUnion;
        // Afford 'MaxAlignedUnion' access to the (private) 'FuncPtr' type.

    union MaxAlignedUnion {
        // This 'union' is guaranteed to be maximally aligned.  However, this
        // union is *not* a good candidate for 'MaxAlignedType' because it
        // might be larger than necessary.

        char         d_char;
        short        d_short;
        int          d_int;
        long         d_long;
        long long    d_longLong;
        bool         d_bool;
        wchar_t      d_wchar_t;
        void        *d_pointer;
        FuncPtr      d_funcPointer;
        float        d_float;
        double       d_double;
#if ! (defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_OS_LINUX))
        long double  d_longDouble;
#endif
#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))      \
 && !defined(BSLS_PLATFORM_OS_SOLARIS)
        AlignmentImp8ByteAlignedType
                     d_8bytesAlignedType;
#endif
    };

  public:
    // CONSTANTS
    enum {
        // Define the minimal value that satisfies the alignment requirements
        // for all types.

        BSLS_MAX_ALIGNMENT = AlignmentFromType<MaxAlignedUnion>::VALUE
    };

    // TYPES
    typedef AlignmentToType<BSLS_MAX_ALIGNMENT>::Type MaxAlignedType;
        // Alias for a primitive type that has the most stringent alignment
        // requirement.

    // CLASS METHODS
    static int calculateAlignmentFromSize(std::size_t size);
        // Return the *natural* alignment for a memory block of the specified
        // size -- i.e., the largest power of 2 that evenly divides 'size', up
        // to a maximum of 'BSLS_MAX_ALIGNMENT'.  It is guaranteed that this
        // alignment will be sufficient for any object having a footprint of
        // 'size' bytes on the current platform.  The behavior is undefined
        // unless '1 <= size' and 'size <= INT_MAX'.  Note that, depending on
        // the machine architecture and compiler setting, the returned
        // alignment value may be more restrictive than required for a
        // particular object for two reasons:
        //: 1 The object may be composed entirely of elements, such as 'char',
        //:   that have minimal alignment restrictions, and
        //: 2 The architecture and our compiler settings may result in
        //:   unexpectedly lenient alignment requirements.
        // Also note that aligning on a more restrictive boundary may improve
        // performance.

    static int calculateAlignmentOffset(const void *address, int alignment);
        // Return the minimum non-negative integer that, when added to the
        // numerical value of the specified 'address', yields the specified
        // 'alignment'.  The behavior is undefined unless 'alignment' is a
        // positive, integral power of 2.

    static bool is2ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on a 2-byte
        // boundary (i.e., the numerical value of 'address' is evenly divisible
        // by 2), and 'false' otherwise.

    static bool is4ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on a 4-byte
        // boundary (i.e., the numerical value of 'address' is evenly divisible
        // by 4), and 'false' otherwise.

    static bool is8ByteAligned(const void *address);
        // Return 'true' if the specified 'address' is aligned on an 8-byte
        // boundary (i.e., the numerical value of 'address' is evenly divisible
        // by 8), and 'false' otherwise.

    static std::size_t roundUpToMaximalAlignment(std::size_t size);
        // Return the specified 'size' (in bytes) rounded up to the smallest
        // integral multiple of the maximum alignment.  The behavior is
        // undefined unless '0 <= size' and 'size' satisfies:
        //..
        //  size <= std::numeric_limits<std::size_t>::max()
        //          - BSLS_MAX_ALIGNMENT + 1
        //..
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ----------------
                            // struct Alignment
                            // ----------------

// CLASS METHODS
inline
int AlignmentUtil::calculateAlignmentFromSize(std::size_t size)
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    // It is assumed that 'BSLS_MAX_ALIGNMENT' is a positive, integral power of
    // 2 (see the checks to that effect in the '.cpp' file).  For example,
    // suppose that 'BSLS_MAX_ALIGNMENT' is 16:
    //..
    //                                negated
    //                    modified    modified  intersect
    //  size     size       size       size       size     returned
    //  ----   --------   --------   --------   --------   --------
    //    0    00000000   00010000   11110000   00010000      16
    //    1    00000001   00010001   11101111   00000001       1
    //    2    00000010   00010010   11101110   00000010       2
    //    3    00000011   00010011   11101101   00000001       1
    //
    //    4    00000100   00010100   11101100   00000100       4
    //    5    00000101   00010101   11101011   00000001       1
    //    6    00000110   00010110   11101010   00000010       2
    //    7    00000110   00010111   11101001   00000001       1
    //
    //    8    00001000   00011000   11101000   00001000       8
    //    9    00001001   00011001   11100111   00000001       1
    //   10    00001010   00011010   11100110   00000010       2
    //   11    00001011   00011011   11100101   00000001       1
    //
    //   12    00001100   00011100   11100100   00000100       4
    //   13    00001101   00011101   11100011   00000001       1
    //   14    00001110   00011110   11100010   00000010       2
    //   15    00001110   00011111   11100001   00000001       1
    //
    //   16    00010000   00010000   11110000   00010000      16
    //   17    00010001   00010001   11101111   00000001       1
    //   18    00010010   00010010   11101110   00000010       2
    //   19    00010011   00010011   11101101   00000001       1
    //
    //   :         :          :          :         :           :
    //
    //   32    00100000   00110000   11010000   00010000      16
    //   33    00100001   00110001   11001111   00000001       1
    //   34    00100010   00110010   11001110   00000010       2
    //   35    00100011   00110011   11001101   00000001       1
    //
    //   :         :          :          :         :           :
    //..

    BSLS_ASSERT_SAFE(1 <= size);

    // It is safe to cast our value from a 'size_t' to an int, because all
    // bits that are higher order that 'BSLS_MAX_ALIGNMENT' are ignored by
    // the arithmetic that is done.

    int alignment = static_cast<int>(size | BSLS_MAX_ALIGNMENT);
    alignment &= -alignment;              // clear all but lowest order set bit

    // Verify at most one bit is set (should be impossible to fail)

    BSLS_ASSERT_SAFE(0 == (alignment & (alignment - 1)));

    return alignment;
}

inline
int AlignmentUtil::calculateAlignmentOffset(const void *address,
                                            int         alignment)
{
    BSLS_ASSERT_SAFE(1 <= alignment);
    BSLS_ASSERT_SAFE(0 == (alignment & (alignment - 1)));

    // Note that if 'address' is null, this function will correctly return zero
    // only if 'alignment' is a positive, integral power of 2.  Also note that
    // two other alternative implementations proved to be less efficient:
    //..
    //  return static_cast<int>(alignment - 1 -
    //               (reinterpret_cast<std::size_t>(address - 1)) % alignment);
    //..
    // and:
    //..
    //  const int mask = alignment - 1;
    //  return int(mask -
    //                  ((reinterpret_cast<std::size_t>(address - 1)) & mask));
    //..

    return static_cast<int>(
                           (alignment - reinterpret_cast<std::size_t>(address))
                         & (alignment - 1));
}

inline
bool AlignmentUtil::is2ByteAligned(const void *address)
{
    return 0 == (reinterpret_cast<std::size_t>(address) & 0x1);
}

inline
bool AlignmentUtil::is4ByteAligned(const void *address)
{
    return 0 == (reinterpret_cast<std::size_t>(address) & 0x3);
}

inline
bool AlignmentUtil::is8ByteAligned(const void *address)
{
    return 0 == (reinterpret_cast<std::size_t>(address) & 0x7);
}

inline
std::size_t AlignmentUtil::roundUpToMaximalAlignment(std::size_t size)
{
    BSLS_ASSERT_SAFE(size <= std::numeric_limits<std::size_t>::max()
                           - BSLS_MAX_ALIGNMENT + 1);

    return ((size + BSLS_MAX_ALIGNMENT - 1) / BSLS_MAX_ALIGNMENT)
                                                          * BSLS_MAX_ALIGNMENT;
}

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
