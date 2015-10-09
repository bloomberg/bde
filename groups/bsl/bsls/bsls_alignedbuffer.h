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
///Single-Object Buffers
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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

namespace bsls {

                        // ========================
                        // union AlignedBuffer_Data
                        // ========================

template <int SIZE, int ALIGNMENT>
union AlignedBuffer_Data {
    // This *private* implementation type provides a public 'char' array
    // data member 'd_buffer' whose length is the specifed (template parameter)
    // 'SIZE' and which is aligned according to the specified (template
    // parameter) 'ALIGNMENT'.

  public:
    typedef typename AlignmentToType<ALIGNMENT>::Type AlignmentType;
        // Define an alias for alignment type to work around a Sun CC 5.5 bug
        // that gives a warning if the type is directly accessed in the union.
        // Note that to allow the union to access this typedef it must be
        // declared with public access.

#if defined(BSLS_COMPILERFEATURES_ALIGNAS)
    // The C++11 implementation uses the 'alignas' keyword to ensure the
    // alignment of 'd_buffer'.

    char alignas(ALIGNMENT) d_buffer[SIZE];
#else
    // The C++03 implementation uses a union data member to ensure the
    // alignment of 'd_buffer'.

    char          d_buffer[SIZE];
    AlignmentType d_align;
#endif
};

#if defined(BSLS_PLATFORM_CMP_MSVC) && !defined(BSLS_COMPILERFEATURES_ALIGNAS)
// We provide template specializations for MSVC using __declspec(align).
// Note that MSVC does not enforce the alignment of (at least) 'double'
// variables on the stack.  (internal issue 64140445).

#define BSLS_ALIGNAS(N) __declspec(align(N))
template <int SIZE>
union AlignedBuffer_Data<SIZE, 1> {BSLS_ALIGNAS(1) char d_buffer[SIZE]; };
template <int SIZE>
union AlignedBuffer_Data<SIZE, 2> {BSLS_ALIGNAS(2) char d_buffer[SIZE]; };
template <int SIZE>
union AlignedBuffer_Data<SIZE, 4> {BSLS_ALIGNAS(4) char d_buffer[SIZE]; };
template <int SIZE>
union AlignedBuffer_Data<SIZE, 8> {BSLS_ALIGNAS(8) char d_buffer[SIZE]; };
template <int SIZE>
union AlignedBuffer_Data<SIZE, 16> {BSLS_ALIGNAS(16) char d_buffer[SIZE]; };
#undef BSLS_ALIGNAS
#endif  // defined(BSLS_PLATFORM_CMP_MSVC)

                        // ===================
                        // class AlignedBuffer
                        // ===================

template <int SIZE, int ALIGNMENT = AlignmentUtil::BSLS_MAX_ALIGNMENT>
class AlignedBuffer {
    // An instance of this union is a block of raw memory of specified 'SIZE'
    // and 'ALIGNMENT'.  A 'AlignedBuffer' object does not manage the
    // construction or destruction of any other objects.  'SIZE' is rounded up
    // to the nearest multiple of 'ALIGNMENT'.  An instantiation of this union
    // template will not compile unless 'ALIGNMENT' is a power of two not
    // larger than 'AlignmentUtil::BSLS_MAX_ALIGNMENT'.

    // DATA
    AlignedBuffer_Data<SIZE, ALIGNMENT> d_data;

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// MANIPULATORS
template <int SIZE, int ALIGNMENT>
inline
char *AlignedBuffer<SIZE, ALIGNMENT>::buffer()
{
    return d_data.d_buffer;
}

// ACCESSORS
template <int SIZE, int ALIGNMENT>
inline
const char *AlignedBuffer<SIZE, ALIGNMENT>::buffer() const
{
    return d_data.d_buffer;
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
