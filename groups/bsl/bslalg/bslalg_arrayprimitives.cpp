// bslalg_arrayprimitives.cpp                                         -*-C++-*-
#include <bslalg_arrayprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//
// IMPLEMENTATION NOTE: cases where 'value == 0' are common enough that it is
// worth avoiding the overhead of the generic bit-wise copyable implementation
// (exponential memcopy, see 'bitwiseFillN' below).  Same note applies for
// other fundamental types below, except that cases in which using memset is
// possible are limited (mostly 0 for fundamental types, -1 for integral
// types).
//
// There are other options to 'bitwiseFillN', one that attempts to be more
// cache-friendly is the 'memcpyBy32' below:
//..
//  enum {
//      BUF_SIZE = 64
//  };
//
//  inline
//  void memcpyBy32(char        *begin,
//                  std::size_t  numBytesInitialized,
//                  std::size_t  numBytes)
//  {
//      BSLS_ASSERT(numBytesInitialized == BUF_SIZE ||
//                     numBytesInitialized == numBytes);
//
//      char *end = begin + numBytesInitialized;
//      numBytes -= numBytesInitialized;        // bytes remaining to be copied
//
//      char buffer[BUF_SIZE];
//      if (sizeof buffer < numBytesInitialized) {
//          std::memcpy((char *)buffer, begin, BUF_SIZE);
//
//          while (BUF_SIZE <= numBytes) {
//              std::memcpy(end, begin, BUF_SIZE);
//              end += BUF_SIZE;
//              numBytes -= BUF_SIZE;
//          }
//          if (0 < numBytes) {
//              std::memcpy(end, begin, numBytes);
//          }
//      }
//  }
//..
// to be used in the routines below as:
//..
//  void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
//                                             int       *begin,
//                                             int        value,
//                                             size_type  numElements,
//                                             void      *allocator,
//                                             bslalg_TypeTraitBitwiseCopyable)
//  {
//      if (0 == value || -1 == value) {
//          std::memset(begin, value, numElements * sizeof value);
//      }
//      else {
//          size_type numBytes = BUF_SIZE;
//          if (numBytes > numElements * sizeof value) {
//              numBytes = numElements * sizeof value;
//          }
//          const size_type numElementsInitialized = numBytes / sizeof value;
//          for (int i = 0; i < numElementsInitialized; ++i) {
//              begin[i] = value;
//          }
//          memcpyBy32((char *)begin, numBytes, numElements * sizeof value);
//      }
//  }
//..
// Overall, the timings (case -1 in test driver) are as follows:
//..
//  uninitializedFillN with char      AIX       Linux     Solaris
//  fill<char>(0) - single loop  : 0.140407   0.593034   0.080676
//  fill<char>(0) - memset       : 0.011672   0.034951   0.013599
//  fill<char>(1) - single loop  : 0.109311   0.524145   0.076906
//  fill<char>(1) - memcpy by 32 : 0.011431   0.042800   0.021871
//  fill<char>(1) - exp memcpy   : 0.015887   0.106401   0.069888
//
//  uninitializedFillN with int       AIX       Linux     Solaris
//  fill<int> (0)- single loop   : 0.027985   0.331247   0.054637
//  fill<int>(0) - memset        : 0.011155   0.034176   0.022044
//  fill<int>(1) - single loop   : 0.028685   0.338459   0.053492
//  fill<int>(1) - memcpy by 32  : 0.016442   0.091342   0.044233
//  fill<int>(1) - exp memcpy    : 0.015754   0.091132   0.044288
//
//  uninitializedFillN with double    AIX       Linux     Solaris
//  fill<double>(0) - single loop: 0.014690   0.331021   0.033191
//  fill<double>(0) - memset     : 0.011942   0.034935   0.013567
//  fill<double>(1) - single loop: 0.015047   0.348272   0.030739
//  fill<double>(1) - memcpy 32  : 0.015777   0.091844   0.044520
//  fill<double>(1) - exp memcpy : 0.015969   0.086752   0.054252
//
//  uninitializedFillN with void *    AIX       Linux     Solaris
//  fill<void *>(0) - single loop: 0.028078   0.331023   0.047254
//  fill<void *>(0) - memset     : 0.011386   0.035344   0.021190
//  fill<void *>(1) - single loop: 0.028160   0.308930   0.046970
//  fill<void *>(1) - memcpy 32  : 0.015719   0.100547   0.071111
//  fill<void *>(1) - exp memcpy : 0.016115   0.090540   0.055150
//..
//
///A note on the usage of 'bslmf_EnableIf'
///---------------------------------------
// This is what it would look like in bslalg_ArrayPrimitives if we had used
// 'bslmf_EnableIf' directly in the return type:
//..
//  template <class TARGET_TYPE, class ALLOCATOR>
//  static typename bslmf_EnableIf<bslalg_HasTrait<
//                                 TARGET_TYPE,
//                                 bslalg_TypeTraitHasTrivialDefaultConstructor
//                                         >::VALUE,
//                                 void>::Type
//  defaultConstruct(TARGET_TYPE *begin, ...);
//..
// The 'bslmf_enableif' is currently not part of the bslmf package because it
// does not work with SunPRO CC 5.5.

#include <bslmf_assert.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <cstring>

namespace BloombergLP {

// CLASS METHODS
void bslalg_ArrayPrimitives_Imp::bitwiseFillN(char      *begin,
                                              size_type  numBytesInitialized,
                                              size_type  numBytes)
{
    BSLS_ASSERT(numBytesInitialized <= numBytes);

    // Copy the destination onto itself, doubling size at every iteration.

    char *end = begin + numBytesInitialized;
    numBytes -= numBytesInitialized;            // bytes remaining to be copied

    while (numBytesInitialized <= numBytes) {
        std::memcpy(end, begin, numBytesInitialized);
        end += numBytesInitialized;
        numBytes -= numBytesInitialized;
        numBytesInitialized *= 2;
    }
    if (0 < numBytes) {
        std::memcpy(end, begin, numBytes);   // finish copying end of the range
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                        short                                    *begin,
                        short                                     value,
                        size_type                                 numElements,
                        void                                     *,
                        bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    BSLMF_ASSERT(2 == sizeof(short));

    const char *valueBuffer = (const char *) &value;
    if (valueBuffer[0] == valueBuffer[1]) {  // 0, but also -1, 257, etc.
        std::memset(begin, value, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                         int                                      *begin,
                         int                                       value,
                         size_type                                 numElements,
                         void                                     *,
                         bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    const char  *valueCharBuffer  = (const char *) &value;
    const short *valueShortBuffer = (const short*)(void *) valueCharBuffer;
    if (valueCharBuffer[0]  == valueCharBuffer[1] &&
        valueShortBuffer[0] == valueShortBuffer[1]) {
        // The two tests above make sure all four bytes of value are identical.

        std::memset(begin, value, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                         bsls_Types::Int64                        *begin,
                         bsls_Types::Int64                         value,
                         size_type                                 numElements,
                         void                                     *,
                         bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    const char  *valueCharBuffer  = (const char *) &value;
    const short *valueShortBuffer = (const short *)(void *) valueCharBuffer;
    const int   *valueIntBuffer   = (const int   *)(void *) valueCharBuffer;
    if (valueCharBuffer[0]  == valueCharBuffer[1] &&
        valueShortBuffer[0] == valueShortBuffer[1] &&
        valueIntBuffer[0]   == valueIntBuffer[1]) {
        // The three tests above make sure all eight bytes of value are
        // identical.

        std::memset(begin, value, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                         float                                    *begin,
                         float                                     value,
                         size_type                                 numElements,
                         void                                     *,
                         bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    if (0 == value) {
        std::memset(begin, 0, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                         double                                   *begin,
                         double                                    value,
                         size_type                                 numElements,
                         void                                     *,
                         bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    if (0 == value) {
        std::memset(begin, 0, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                         long double                              *begin,
                         long double                               value,
                         size_type                                 numElements,
                         void                                     *,
                         bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    if (0 == value) {
        std::memset(begin, 0, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                        void                                     **begin,
                        void                                      *value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER>  *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    if (0 == value) {
        std::memset(begin, 0, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

void bslalg_ArrayPrimitives_Imp::uninitializedFillN(
                        const void                               **begin,
                        const void                                *value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf_MetaInt<IS_FUNDAMENTAL_OR_POINTER>  *)
{
    if (0 == numElements) {
        return;                                                      // RETURN
    }
    if (0 == value) {
        std::memset(begin, 0, numElements * sizeof value);
    }
    else {
        *begin = value;
        bitwiseFillN((char *)begin, sizeof value, numElements * sizeof value);
    }
}

                         // *** bitwiseSwapRanges: ***

void bslalg_ArrayPrimitives_Imp::bitwiseSwapRanges(char *begin,
                                                   char *middle,
                                                   char *end)
{
    int numBytes = middle - begin;
    BSLS_ASSERT(numBytes == end - middle);

    union {
        char                               d_buffer[INPLACE_BUFFER_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;
    } arena;

    for (; INPLACE_BUFFER_SIZE <= numBytes; numBytes -= INPLACE_BUFFER_SIZE,
                                            begin    += INPLACE_BUFFER_SIZE,
                                            middle   += INPLACE_BUFFER_SIZE) {
        std::memcpy(arena.d_buffer, middle, INPLACE_BUFFER_SIZE);
        std::memmove(middle, begin, INPLACE_BUFFER_SIZE);
        std::memcpy(begin, arena.d_buffer, INPLACE_BUFFER_SIZE);
    }

    if (0 < numBytes) {
        std::memcpy(arena.d_buffer, middle, numBytes);
        std::memmove(middle, begin, numBytes);
        std::memcpy(begin, arena.d_buffer, numBytes);
    }
}

                       // *** bitwiseRotateBackward: ***

void bslalg_ArrayPrimitives_Imp::bitwiseRotateBackward(char *begin,
                                                       char *middle,
                                                       char *end)
{
    union {
        char                               d_buffer[INPLACE_BUFFER_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;
    } arena;

    const std::size_t numBytes = middle - begin;
    const std::size_t remBytes = end - middle;

    BSLS_ASSERT(numBytes <= INPLACE_BUFFER_SIZE);

    std::memcpy(arena.d_buffer, begin, numBytes);
    std::memmove(begin, middle, remBytes);
    std::memcpy(end - numBytes, arena.d_buffer, numBytes);
}

                       // *** bitwiseRotateForward: ***

void bslalg_ArrayPrimitives_Imp::bitwiseRotateForward(char *begin,
                                                      char *middle,
                                                      char *end)
{
    union {
        char                               d_buffer[INPLACE_BUFFER_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;
    } arena;

    const std::size_t numBytes = middle - begin;
    const std::size_t remBytes = end - middle;

    BSLS_ASSERT(remBytes <= INPLACE_BUFFER_SIZE);

    std::memcpy(arena.d_buffer, middle, remBytes);
    std::memmove(end - numBytes, begin, numBytes);
    std::memcpy(begin, arena.d_buffer, remBytes);
}

                           // *** bitwiseRotate: ***

void bslalg_ArrayPrimitives_Imp::bitwiseRotate(char *begin,
                                               char *middle,
                                               char *end)
{
    BSLS_ASSERT(begin <= middle && middle <= end);

    // These cases are simple enough, they should be taken care of on their
    // own.

    if (begin == middle || middle == end) {
        // This test changes into O(1) what would otherwise be O(N): do not
        // remove!

        return;                                                      // RETURN
    }

    const std::size_t numElements = middle - begin;
    const std::size_t remElements = end - middle;

    if (numElements == remElements) {
        bitwiseSwapRanges(begin, middle, end);
        return;
    }

    // These cases are only interesting for bitwise moveable types, and if we
    // assume a modest amount of memory on the stack (here assumed to be
    // 'INPLACE_BUFFER_SIZE').

    if (numElements <= INPLACE_BUFFER_SIZE) {
        bitwiseRotateBackward(begin, middle, end);
        return;                                                      // RETURN
    }
    if (remElements <= INPLACE_BUFFER_SIZE) {
        bitwiseRotateForward(begin, middle, end);
        return;                                                      // RETURN
    }

    // This algorithm proceeds exactly like the template version, char-by-char.
    // First we compute the 'gcd(end - begin, numElements)' which is the number
    // of cycles in the rotation.

    std::size_t numCycles = end - begin;
    std::size_t remainder = numElements;
    while (remainder != 0) {
        std::size_t tmp = numCycles % remainder;
        numCycles       = remainder;
        remainder       = tmp;
    }

    // However, this version proceeds by executing the 'numCycles' in parallel
    // (as much as possible) by moving 'min(numCycles, INPLACE_BUFFER_SIZE)'
    // characters at the same time.

    union {
        char                               d_buffer[INPLACE_BUFFER_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;
    } arena;

    for (std::size_t i = 0; i < numCycles; i += INPLACE_BUFFER_SIZE) {
        // Let the current cycle be initially 'A__B__C__D__' (note that its
        // stride is 'length / numCycles'), and let (*) denote the current
        // position of 'ptr'.  Within this loop, we will do:
        //..
        //  numBytes = min(numCycles, INPLACE_BUFFER_SIZE)
        //..
        // rotations in parallel; it is most easy to think of it in terms of a
        // synthetic value type whose size is 'numBytes', which fits in the
        // 'arena.d_buffer'.  Thus in the diagrams, "A .. E" represent such a
        // value.

        std::size_t numBytes = numCycles - i < INPLACE_BUFFER_SIZE
                             ? numCycles - i
                             : static_cast<std::size_t>(INPLACE_BUFFER_SIZE);

        char *ptr = begin;

        std::memcpy(arena.d_buffer, ptr, numBytes);
            //..
            //  Seed for current cycle: (*)__B__C__D__
            //..
            // NOTE: the allocator is unused as the type is bitwise moveable.

        if (numElements < remElements) {
            // Rotate the cycle forward by 'numElements' positions (or backward
            // by '-(length - numElements) == -remElements' positions if
            // crossing the boundary forward).  The transformation is:
            //..
            //   (*)__B__C__D__ => B__(*)__C__D__
            //                  => B__C__(*)__D__
            //                  => B__C__D__(*)__
            //..
            // The length of the cycle is always 'length / numCycles', but it
            // crosses the range boundaries 'numElements / numCycles' times,
            // each triggering an extra assignment in the if clause below, so
            // the loop must only be executed:
            //..
            //  (length - numElements) / numCycles == remELements / numCycles
            //..
            // times.

            std::size_t cycleSize = remElements / numCycles;

            for (std::size_t j = 0; j < cycleSize; ++j) {
                if (ptr > begin + remElements) {
                    // Wrap around the range boundaries.  (Note that
                    // '- remElements == numElements - (end - begin)'.)

                    std::memcpy(ptr, ptr - remElements, numBytes);
                    ptr -= remElements;
                }

                std::memcpy(ptr, ptr + numElements, numBytes);
                ptr += numElements;
            }
        }
        else {
            // Rotate the cycle backward by '-remElements' positions (or
            // forward by 'numElements' positions if crossing the boundary
            // backward).  The transformation is:
            //..
            //   (*)__B__C__D__ => D__B__C__(*)__
            //                  => D__B__(*)__C__
            //                  => D__(*)__B__C__
            //..
            // The length of the cycle is always 'length / numCycles', but
            // going backward (which adds an initial extra crossing) crosses
            // the range boundaries 'remElements / numCycles + 1' times each of
            // which trigger an extra assignment in the if clause below, so the
            // loop must only be executed:
            //..
            //  (length - remElements) / numCycles - 1 ==
            //                                      numELements / numCycles - 1
            //..
            // times.

            std::size_t cycleSize = numElements / numCycles - 1;

            for (std::size_t j = 0; j < cycleSize; ++j) {
                if (ptr < end - numElements) {
                    // Wrap around the range boundaries.  (Note that
                    // 'numElements == (end - begin) - remElements'.)

                    std::memcpy(ptr, ptr + numElements, numBytes);
                    ptr += numElements;
                }

                std::memcpy(ptr, ptr - remElements, numBytes);
                ptr -= remElements;
            }
        }

        std::memcpy(ptr, arena.d_buffer, numBytes);
            // Close the cycle, e.g.:
            //..
            // (first case):  B__C__D__(*)__ => B__C__D__A__
            // (second case): D__(*)__B__C__ => D__A__B__C__
            //..

        begin += numBytes;
            // And move on to the next cycle: => _W__X__Y__Z_
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
