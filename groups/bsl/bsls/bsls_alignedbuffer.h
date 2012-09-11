// bsls_alignedbuffer.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#define INCLUDED_BSLS_ALIGNEDBUFFER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide raw buffers with user-specified size and alignment.
//
//@CLASSES:
//  bsls::AlignedBuffer: Uninitialized buffer of specified size and alignment
//
//@SEE_ALSO: bsls_objectbuffer, bsls_alignmenttotype
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a templated buffer type with a
// user-specified compile-time size and user-specified alignment.  The user
// instantiates 'bsls::AlignedBuffer' with specific size and alignment
// requirements, and then uses that memory as needed.  If an alignment is not
// specified at template instantiation, then the buffer object is maximally
// aligned.
//
// Typically, 'bsls::AlignedBuffer' is used in situations where it is desirable
// to allocate a block of properly-aligned raw memory from somewhere other than
// the heap, e.g., on the stack or within an aggregate object, including within
// a union.  It is a convenient way to create a small heap from which one or
// more objects are allocated at run-time.
//
///Single-object buffers
///---------------------
// Although, for a given type 'T',
// 'bsls::AlignedBuffer<sizeof(T), bsls::AlignmentFromType<T>::VALUE>' will
// produce a buffer properly sized and aligned to hold a 'T' object, it is
// simpler and clearer to use 'bsls::ObjectBuffer<T>' for this purpose.  See
// the 'bsls_objectbuffer' component for more information.
//
///Stack Alignment
///---------------
// On platforms with 32-bit words, there is usually no efficiency gain by using
// more than 4-byte alignment.  Yet some compilers use 8-byte alignment for
// 'long long' or 'double', presumably so that the code will run faster on a
// future 64-bit CPU.  The program loader, however, has no reason to presume
// more than 4-byte alignment when allocating the program stack.  This can
// result in stack objects appearing to be misaligned relative to the
// alignments computed by this component.  This is not a bug in either this
// component nor in the compiler, but it is somewhat surprising.  We have seen
// this behavior on the MS VC++ 7 platform.  See also the "Surprises and
// Anomalies" section in 'bsls_alignmentfromtype.h'.
//
///Usage
///-----
// The 'allocateFromBuffer' function below uses an aligned buffer as a small
// heap from which objects can be allocated.  We choose 'int' alignment (4-byte
// alignment) for our buffer because the objects we are allocating are composed
// of 'char', 'short', and 'int' values only.  If no alignment were specified,
// the buffer would be maximally aligned, which could be wasteful on some
// platforms.
//..
//  const int MY_ALIGNMENT = bsls::AlignmentFromType<int>::VALUE;
//  bsls::AlignedBuffer<1000, MY_ALIGNMENT> my_AllocBuffer;
//  const char* my_AllocEnd = my_AllocBuffer.buffer() + 1000;
//  char *my_AllocPtr = my_AllocBuffer.buffer();
//      // Invariant: my_AllocPtr is always aligned on a multiple of 4 bytes
//
//  static void *allocateFromBuffer(int size)
//  {
//      if (size > my_AllocEnd - my_AllocPtr)
//          return 0;       // Out of buffer space
//
//      void *result = my_AllocPtr;
//      my_AllocPtr += size;
//      if (size % MY_ALIGNMENT) {
//          // re-establish invariant by re-aligning my_AllocPtr
//          my_AllocPtr += MY_ALIGNMENT - size % MY_ALIGNMENT;
//      }
//
//      assert(0 == size_t(my_AllocPtr) % MY_ALIGNMENT);     // Test invariant
//
//      return result;
//  }
//..
// Below, we use our allocation function to allocate arrays of 'char', 'short',
// and user-defined 'Object' types from the static buffer.  Note that our
// 'Object' structure is composed of members that have alignment requirements
// less than or equal to 'int's alignment requirements.
//..
//  struct Object {
//      char  d_c;
//      short d_s;
//      int   d_i;
//  };
//
//  int main()
//  {
//      // Allocate three 'char's from the buffer.
//      char *charPtr   = (char *)   allocateFromBuffer(3 * sizeof(char));
//      assert(0 == size_t(charPtr) % MY_ALIGNMENT);
//
//      // Allocate three 'short's from the buffer.
//      short *shortPtr = (short *)  allocateFromBuffer(3 * sizeof(short));
//      assert(0 == size_t(shortPtr) % MY_ALIGNMENT);
//
//      // Allocate three 'Object's from the buffer
//      Object *objPtr = (Object *)  allocateFromBuffer(3 * sizeof(Object));
//      assert(0 == size_t(objPtr) % MY_ALIGNMENT);
//
//      if (!charPtr || !shortPtr || !objPtr) {
//          fprintf(stderr, "Global buffer is not large enough.\n");
//          return -1;
//      }
//
//      // ...
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLS_ALIGNMENTTOTYPE
#include <bsls_alignmenttotype.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

namespace BloombergLP {

namespace bsls {

                        // ===================
                        // union AlignedBuffer
                        // ===================

template <int SIZE, int ALIGNMENT = AlignmentUtil::BSLS_MAX_ALIGNMENT>
union AlignedBuffer {
    // An instance of this union is a block of raw memory of specified 'SIZE'
    // and 'ALIGNMENT'.  A 'AlignedBuffer' object does not manage the
    // construction or destruction of any other objects.  'SIZE' is rounded up
    // to the nearest multiple of 'ALIGNMENT'.  An instantiation of this union
    // template will not compile unless 'ALIGNMENT' is a power of two not
    // larger than 'AlignmentUtil::BSLS_MAX_ALIGNMENT'.

  public:
    typedef typename AlignmentToType<ALIGNMENT>::Type AlignmentType;
        // Define an alias for alignment type to work around a Sun CC 5.5 bug
        // that gives a warning if the type is directly accessed in the union.
        // Note that to allow the union to access this typedef it must be
        // declared with public access.

  private:
    // Buffer of 'SIZE' bytes, correctly aligned at 'ALIGNMENT' The size of
    // this union will always be an even multiple of 'ALIGNMENT'.
    char          d_buffer[SIZE];
    AlignmentType d_align;

  public:
    // CREATORS Note that We deliberately omit defining constructors and
    // destructors in order to keep this union "POD-like".  In particular, a
    // 'AlignedBuffer' may be used as a member in another 'union'.  Copying a
    // 'AlignedBuffer' assignment or copy construction will result in a
    // bit-wise copy and will not invoke any user-defined assignment operators
    // or copy constructors.

    // MANIPULATORS
    char *buffer();
        // Return a the address of the first byte of this object, cast to a
        // 'char*' pointer.

    // ACCESSORS
    const char *buffer() const;
        // Return a the address of the first byte of this object, cast to a
        // 'const char*' pointer.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS
template <int SIZE, int ALIGNMENT>
inline
char *AlignedBuffer<SIZE, ALIGNMENT>::buffer()
{
    return d_buffer;
}

// ACCESSORS
template <int SIZE, int ALIGNMENT>
inline
const char *AlignedBuffer<SIZE, ALIGNMENT>::buffer() const
{
    return d_buffer;
}

}  // close package namespace

#if defined(BDE_BACKWARD_COMPATIBILITY) && 1 == BDE_BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bsls_AlignedBuffer
#undef bsls_AlignedBuffer
#endif
#define bsls_AlignedBuffer bsls::AlignedBuffer
    // This alias is defined for backward compatibility.
#endif // BDE_BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
