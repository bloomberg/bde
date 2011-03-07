// bdes_alignment.h            -*-C++-*-
#ifndef INCLUDED_BDES_ALIGNMENT
#define INCLUDED_BDES_ALIGNMENT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-specific alignment information.
//
//@DEPRECATED: Use 'bsls_alignment' instead.
//
//@CLASSES:
// bdes_AlignmentOf: alignment requirements for a given type
// bdes_AlignmentToType: fundamental type for a given alignment
// bdes_Alignment: namespace for alignment constants and utility functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
// This component computes (at compile-time) platform-dependent alignment
// information.  The meta-function class 'bdes_AlignmentOf' defines the
// alignment requirements of its template argument.  For a specified
// alignment, the meta-function class 'bdes_AlignmentToType' defines the
// smallest primitive type requiring that alignment.
//
// This component also provides a namespace, 'bdes_Alignment', for a suite of
// compile-time constants and pure procedures that yield platform-dependent
// alignment information for primitive and user-defined types, including the
// maximum alignment for any type and the smallest fundamental type requiring
// maximum alignment.  These functions, used in conjunction, are especially
// useful for creating pools and allocators that rely on efficient alignment.
//
///Surprises and Anomalies
///-----------------------
// Note that *efficient* *alignment* and *size* are not identical on all
// platforms.  For example, Linux on 32-bit Intel aligns an eight-byte
// 'double' on a 4-byte boundary within a 'struct'.
//
// On platforms with 32-bit words, there is usually no efficiency gain by
// using more than 4-byte alignment.  Yet some compilers use 8-byte alignment
// for 'long long' or 'double', presumably so that the code will run faster on
// a future 64-bit CPU.  The program loader, however, has no reason to presume
// more than 4-byte alignment when allocating the program stack.  This can
// result in stack objects appearing to be misaligned relative to the
// alignments computed by this component.  This is not a bug in either this
// component nor in the compiler, but it is somewhat surprising.  We have seen
// this behavior on the MS VC++ 7 platform.
//
// The Data General C compiler (which is is the back-end for the como C++
// compiler) is very inconsistent when aligning 'long long' values.  It
// computes structure size as though 'long long' were 8-byte aligned but it
// will often put a 'long long' at a 4-byte boundary within a 'struct'.  To
// work around this compiler bug, this component truncates any alignment
// computation to 4-bytes on the DG.  Since the Intel CPU in the DG has 32-bit
// words, this truncation causes no loss of efficiency
//
///Terminology
///-----------
// *Efficient* *Alignment* is any alignment that prevents the CPU from
// performing unaligned memory access.
//
// *Compiler* *Alignment* is the alignment chosen for a data type by a
// specific compiler with a specific set of compile-time options.  On most
// platforms, the compiler can be instructed to pack all structures with
// 1-byte alignment, even if inefficient memory access results.
//
// *Required* *Alignment* is synonymous with *Compiler* *Alignment*, even when
// the CPU supports unaligned access.  The terms "required alignment" and
// "alignment requirement" are in common use even though "compiler alignment"
// is a more precise term.
//
///Assumptions
///-----------
// The required alignment for type 'T' evenly divides 'sizeof(T)'.  This
// implies that the required alignment is never larger than 'sizeof(T)'.
//
// The efficient alignment for any given type is a power of two.
//
// No type has a required alignment greater than 16.
//
// The required alignment for a 'struct', 'class', or 'union' is the same as
// the required alignment of its most restrictive non-static data member
// (including the implicit '_vtbl' pointer in polymorphic classes and internal
// pointers in virtually-derived classes).
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
//                                  bsls_Alignment::Strategy  strategy)
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
// 'bsls_Alignment::BSLS_NATURAL', we calculate the alignment from 'size'; for
// 'bsls_Alignment::BSLS_MAXIMUM', we use the platform-dependent
// 'my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT' value:
//..
//      const int alignment =
//                         strategy == bsls_Alignment::BSLS_NATURAL
//                         ? my_AlignmentUtil::calculateAlignmentFromSize(size)
//                         : my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT;
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
///Usage
///-----
///Usage Example 1:
///- - - - - - - -
// The following shows how 'bdes_AlignmentOf' can be used to create a static
// "database" of types and their attributes:
//..
//  enum my_ElemType { MY_CHAR, MY_INT, MY_DOUBLE, MY_POINTER };
//
//  struct my_ElemAttr {
//      my_ElemType d_type;
//      int         d_size;
//      int         d_alignment;
//  };
//
//  static const my_ElemAttr MY_ATTRIBUTES[] = {
//      { MY_CHAR,      sizeof(char),   bdes_AlignmentOf<char>::VALUE },
//      { MY_INT,       sizeof(int),    bdes_AlignmentOf<int>::VALUE },
//      { MY_DOUBLE,    sizeof(double), bdes_AlignmentOf<double>::VALUE },
//      { MY_POINTER,   sizeof(void *), bdes_AlignmentOf<void *>::VALUE }
//  };
//
//  int main()
//  {
//      assert(1 == MY_ATTRIBUTES[MY_CHAR].d_alignment);
//      return 0;
//  }
//..
///Usage Example 2:
///- - - - - - - -
// The following shows how 'bdes_AlignmentOf' automatically computes the
// alignment of a 'struct' as the alignment of its most restrictively aligned
// member.
//..
//  class my_String {
//      char d_buf[5];
//      int  d_len;
//    public:
//      my_String(const char* s) : d_len(strlen(s)) { strcpy(d_buf, s); }
//      ~my_String() { }
//
//      int length() const { return d_len; }
//      const char* c_str() const { return d_buf; }
//  };
//
//  int main()
//  {
//      assert((int) bdes_AlignmentOf<my_String>::VALUE ==
//             (int) bdes_AlignmentOf<int>::VALUE);
//      return 0;
//  }
//
//..
///Usage Example 3:
///- - - - - - - -
// 'bdes_AlignmentOf<TYPE>::Type' can be used to create properly-aligned
// buffers for constructing objects of 'TYPE'.  If 'TYPE' is
// 'bdes_Alignment::MaxAlignedType' the resulting buffer can hold any type at
// all.
//..
//  template <typename TYPE, int SIZE = sizeof(TYPE)>
//  union my_AlignedBuffer
//  {
//      typename bdes_AlignmentOf<TYPE>::Type d_align;      // force alignment
//      char                                  d_buff[SIZE]; // force size
//  };
//..
// This small test program creates a couple of aligned buffers and constructs
// objects into the aligned memory.
//..
//  int main()
//  {
//      // Aligned
//      my_AlignedBuffer<my_String> buf;
//      my_String* const strptr = reinterpret_cast<my_String*>(&buf);
//      new (strptr) my_String("yes"); // Construct my_String obj into buffer
//      assert(0 == strcmp("yes", strptr->c_str()));
//      strptr->~my_String();          // Destroy my_String object
//
//      // 1000-byte buffer aligned for any type.
//      my_AlignedBuffer<bdes_Alignment::MaxAlignedType, 1000> buf2;
//      double* doublePtr = reinterpret_cast<double*>(&buf2);
//      doublePtr[9] = 10.5; // No bus errors: doublePtr is properly aligned.
//
//      return 0;
//  }
//..
///Usage Example 4:
///- - - - - - - -
// The following function illustrates how to use the
// 'calculateAlignmentOffset' function to align a memory address properly and
// 'calculateAlignmentFromSize' to compute a safe alignment when none is
// supplied.  This function allocates memory with specified size and alignment
// requirements from a global buffer:
//..
//  enum { BUFFER_SIZE = 1000 };
//
//  static union {
//      bdes_Alignment::MaxAlignedType d_dummy;  // maximally align buffer
//      char                           d_buffer[BUFFER_SIZE];
//  } memory;
//
//  static int cursor = 0;
//
//  static void *allocateFromBuffer(int size, int alignment = 0)
//      // Allocate memory of the specified 'size' and 'alignment' from
//      // a fixed-size global buffer.  Return the address of the allocated
//      // memory, or 0 if insufficient space remains in the buffer.
//      // If 'alignment' is zero (i.e., not specified), then compute the
//      // alignment based on size.
//      // The behavior is undefined unless 0 < size and 0 <= alignment.
//  {
//      assert(0 < size); assert(0 <= alignment);
//
//      if (0 == alignment) {
//          alignment = bdes_Alignment::calculateAlignmentFromSize(size);
//      }
//
//      int offset = bdes_Alignment::calculateAlignmentOffset(
//                   memory.d_buffer + cursor, alignment);
//
//      if (cursor + offset + size > BUFFER_SIZE) {
//          return 0;   // Insufficient space remains.
//      }
//
//      cursor += offset;
//      void *result = memory.d_buffer + cursor;
//      cursor += size;
//
//      return result;
//  }
//
//  class Object {     // Assume we cannot know the specific data
//      char  d_c;     // members of 'Object'.
//      int   d_i;
//      void *d_ptr;
//  };
//..
// This test program uses the 'allocateFromBuffer' function above to allocate
// memory for various different types of objects.
//..
//  int main()
//  {
//      enum {
//          CHAR_ALIGNMENT  = bsls_AlignmentFromType<char>::VALUE,
//          SHORT_ALIGNMENT = bsls_AlignmentFromType<short>::VALUE
//      };
//
//      char *charPtr   = (char *)   allocateFromBuffer(3 * sizeof(char),
//                                                      CHAR_ALIGNMENT);
//
//      short *shortPtr = (short *)  allocateFromBuffer(3 * sizeof(short),
//                                                      SHORT_ALIGNMENT);
//
//      assert(0 == (((char*) shortPtr - memory.d_buffer) % SHORT_ALIGNMENT));
//
//      // Let 'allocateFromBuffer' calculate a safe alignment for 'Object'
//      // based on its size.
//      Object *objPtr = (Object *)  allocateFromBuffer(3 * sizeof(Object));
//
//      assert(0 == (((char*) objPtr - memory.d_buffer) %
//                   bdes_AlignmentOf<Object>::VALUE));
//
//      // Assuming 1, 2, and 4 are the alignments for 'char', 'short', and
//      // 'Object' respectively, the following diagram illustrates the memory
//      // layout within 'memory.d_buffer':
//      //
//      //   charPtr         shortPtr                        objPtr
//      //      |               |                               |
//      //      V               V                               V
//      //    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--
//      //    | x | x | x |   | x | x | x | x | x | x |   |   | x | x | x |...
//      //    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--
//      //      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
//
//      if (!charPtr || !shortPtr || !objPtr) {
//          bsl::cerr << "Global buffer is not large enough." << bsl::endl;
//          return -1;
//      }
//
//      // ...
//
//      return 0;
//  }
//..
///Principles of Operation
///-----------------------
// The compiler alignment for a given type, 'T', can be computed by creating a
// structure containing a single 'char' member and a 'T' member:
//..
//  struct X {
//      char d_c;
//      T    d_t;
//  };
//..
// The compiler lays this structure out in memory like this:
//..
//      +---+---+-------+
//      |d_c| P |  d_t  |
//      +---+---+-------+
//..
// Where 'P' is padding added by the compiler to ensure that 'd_t' is properly
// aligned.  The alignment for 'T' is the number of bytes from the start of
// the structure to the beginning of 'd_t', which is also the total size of
// the structure minus the size of 'd_t':
//..
//    alignmentOf(T) == sizeof(X) - sizeof(T);
//..
// Since 'sizeof' yields a compile-time constant, the alignment can be
// computed at compile time.
//
// A much more difficult compile-time computation performed by this component
// is finding a fundamental type with the same alignment as 'T'.  This
// involves computing the alignment for 'T' as above and then performing an
// alignment-to-type lookup, all within the compiler.  Although the user is
// only concerned with three "public" classes in this component, a number of
// "private" support classes are needed for this complex meta-computation.
//
// We won't go into every detail of the metaprogramming used to do the
// alignment-to-type lookup, but the general principles are as follows: We
// would like to create a template class which is specialized for each
// fundamental type's alignment.  Unfortunately, multiple types will have the
// same alignment and the compiler would complain if the same specialization
// were defined multiple times.  To disambiguate, we create a "priority" class
// for each fundamental type which arbitrarily ranks it relative to all of the
// other fundamental types.  Each priority class is derived from the
// next-lower priority class.  A set of overloaded functions are created such
// that, given, two fundamental types with the same alignment, overload
// resolution will pick the one with the highest priority (most derived
// priority type).  The 'sizeof' operator and lots of template specialization
// are used to determine the compiler's choice of overloaded function and look
// up the corresponding fundamental type.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

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

#define bdes_AlignmentOf      bsls_AlignmentFromType
#define bdes_AlignmentToType  bsls_AlignmentToType

namespace BloombergLP {

typedef bsls_Alignment bdes_Alignment;

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
