// bdeu_bitstringutil.cpp                                             -*-C++-*-
#include <bdeu_bitstringutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeu_bitstringutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bdes_bitutil.h>

#include <bdeu_print.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>

namespace {

using namespace BloombergLP;

enum { BITS_PER_INT = bdes_BitUtil::BDES_BITS_PER_INT };

#if defined(BSLS_PLATFORM__CMP_GNU) && __GNUC__ == 4 && __GNUC_MINOR__ == 2

// gcc 4.2 has a bug in that it refuses to instantiate a template based on a
// member function with no external linkage ('Bitwise::setEq' in this case).

}  // close unnamed namespace

#endif
                          // --------------
                          // struct Bitwise
                          // --------------

struct Bitwise {
    static void andEq(int *dst, int src);
        // Assign to the specified 'dst' the value of 'dst' bitwise and-ed with
        // the specified 'src'.

    static void minusEq(int *dst, int src);
        // Assign to the specified 'dst' the value of 'dst' bitwise minus-ed
        // with the specified 'src'.

    static void orEq(int *dst, int src);
        // Assign to the specified 'dst' the value of 'dst' bitwise or-ed with
        // the specified 'src'.

    static void setEq(int *dst, int src);
        // Assign to the specified 'dst' the value of the specified 'src'.

    static void xorEq(int *dst, int src);
        // Assign to the specified 'dst' the value of 'dst' bitwise xor-ed with
        // the specified 'src'.
};

inline
void Bitwise::andEq(int *dst, int src)
{
    BSLS_ASSERT(dst);

    *dst &= src;
}

inline
void Bitwise::minusEq(int *dst, int src)
{
    BSLS_ASSERT(dst);

    *dst &= ~src;
}

inline
void Bitwise::orEq(int *dst, int src)
{
    BSLS_ASSERT(dst);

    *dst |= src;
}

inline
void Bitwise::setEq(int *dst, int src)
{
    BSLS_ASSERT(dst);

    *dst = src;
}

inline
void Bitwise::xorEq(int *dst, int src)
{
    BSLS_ASSERT(dst);

    *dst ^= src;
}

#if defined(BSLS_PLATFORM__CMP_GNU) && __GNUC__ == 4 && __GNUC_MINOR__ == 2
namespace {
#endif
                          // -----------
                          // struct Oper
                          // -----------

template <void OPER(int *, int, int, int)>
struct Oper {
    static void operBits(int *dstBitstring,
                         int  dstIndex,
                         int  srcValue,
                         int  numBits);
        // Set the specified 'numBits' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitstring' to the result
        // of the templatized operation 'OPER' of those bits and the low-order
        // 'numBits' bits in the specified 'srcValue'.  All other bits are
        // unaffected.  The operation 'OPER' has arguments: pointer to
        // destination array, index within destination array, source value, and
        // number of bits to apply the operation upon.  The behavior is
        // undefined unless '0 <= dstIndex < BITS_PER_INT', and
        // '0 <= numBits < BITS_PER_INT'.

    static void operInt(int *dstBitstring, int dstIndex, int srcValue);
        // Set the 'BITS_PER_INT' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitstring' to the result
        // of the templatized operation 'OPER' of those bits and bits in the
        // specified 'srcValue'.  All other bits are unaffected.  The operation
        // 'OPER' has arguments: pointer to destination array, index within
        // destination array, source value, and number of bits to apply the
        // operation upon.  The behavior is undefined unless
        // '0 < dstIndex < BITS_PER_INT'.
};

template <void OPER(int *, int, int, int)>
void Oper<OPER>::operBits(int *dstBitstring,
                          int  dstIndex,
                          int  srcValue,
                          int  numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex < BITS_PER_INT);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits < BITS_PER_INT);

    const int dstLen = BITS_PER_INT - dstIndex;
    if (numBits <= dstLen) {
        // Fits in the 'dstIndex' element.

        OPER(dstBitstring, dstIndex, srcValue, numBits);
    }
    else {
        // Destination bits span two 'int' array elements.

        OPER(dstBitstring, dstIndex, srcValue, dstLen);
        OPER(dstBitstring + 1, 0, srcValue >> dstLen, numBits - dstLen);
    }
}

template <void OPER(int *, int, int, int)>
void Oper<OPER>::operInt(int *dstBitstring, int dstIndex, int srcValue)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 < dstIndex);
    BSLS_ASSERT(    dstIndex < BITS_PER_INT);

    // Since 'dstIndex > 0', destination bits always span two 'int' array
    // elements.

    const int dstLen = BITS_PER_INT - dstIndex;
    OPER(dstBitstring, dstIndex, srcValue, dstLen);
    OPER(dstBitstring + 1, 0, srcValue >> dstLen, dstIndex);
}

                          // -----------
                          // struct Move
                          // -----------

template <void OPER(int *, int, int, int), void ALIGNEDOPER(int *, int)>
struct Move {
    static void left(int       *dstBitstring,
                     int        dstIndex,
                     const int *srcBitstring,
                     int        srcIndex,
                     int        numBits);
        // Set the specified 'numBits' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitstring' to the result
        // of the templatized operation 'OPER' of those bits and the 'numBits'
        // contiguous bits starting at the specified 'srcIndex' in the
        // specified 'srcBitstring'.  If the destination happens to be aligned,
        // the templatized operation 'ALIGNEDOPER' is used in place of 'OPER'.
        // The operation proceeds from the low-order bits to the high-order
        // bits (e.g., 'memcpy').  All other bits are unaffected.  The behavior
        // is undefined unless '0 <= dstIndex', '0 <= srcIndex', 'srcBitstring'
        // contains at least 'srcIndex + numBits' bit values, '0 <= numBits',
        // and 'dstBitstring' contains at least 'dstIndex + numBits'.  Note
        // that this method is alias-safe if 'dstIndex <= srcIndex' or
        // 'srcIndex + numBits <= dstIndex'.

    static void right(int       *dstBitstring,
                      int        dstIndex,
                      const int *srcBitstring,
                      int        srcIndex,
                      int        numBits);
        // Set the specified 'numBits' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitstring' to the result
        // of the templatized operation 'OPER' of those bits and the 'numBits'
        // contiguous bits starting at the specified 'srcIndex' in the
        // specified 'srcBitstring'.  If the destination happens to be aligned,
        // the templatized operation 'ALIGNEDOPER' is used in place of 'OPER'.
        // The operation proceeds from the high-order bits to the low-order
        // bits (e.g., reverse of 'memcpy').  All other bits are unaffected.
        // The behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // 'srcBitstring' contains at least 'srcIndex + numBits' bit values,
        // '0 <= numBits', and 'dstBitstring' contains at least
        // 'dstIndex + numBits'.  Note that this method is alias-safe if
        // 'dstIndex >= srcIndex' or 'srcIndex >= dstIndex + numBits'.
};

template <void OPER(int *, int, int, int), void ALIGNEDOPER(int *, int)>
void Move<OPER, ALIGNEDOPER>::left(int       *dstBitstring,
                                   int        dstIndex,
                                   const int *srcBitstring,
                                   int        srcIndex,
                                   int        numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    // TBD: Correct
    BSLS_ASSERT(dstBitstring       != srcBitstring
             || dstIndex           <= srcIndex
             || srcIndex + numBits <= dstIndex);  // alias-safe

    int dstIdx = dstIndex / BITS_PER_INT;
    int dstPos = dstIndex % BITS_PER_INT;

    int srcIdx = srcIndex / BITS_PER_INT;
    int srcPos = srcIndex % BITS_PER_INT;

    // Copy bits to align residual of src on an 'int' boundary.

    if (srcPos) {
        int num = BITS_PER_INT - srcPos;
        if (num >= numBits) {
            Oper<OPER>::operBits(&dstBitstring[dstIdx],
                                 dstPos,
                                 srcBitstring[srcIdx] >> srcPos,
                                 numBits);
            return;                                                   // RETURN
        }
        Oper<OPER>::operBits(&dstBitstring[dstIdx],
                             dstPos,
                             srcBitstring[srcIdx] >> srcPos,
                             num);
        dstPos += num;
        if (dstPos >= BITS_PER_INT) {
            dstPos -= BITS_PER_INT;
            ++dstIdx;
        }
        numBits -= num;
        ++srcIdx;
    }

    // Copy full source ints.

    if (dstPos) {
        // Normal case of the destination location being unaligned.

        while (numBits >= BITS_PER_INT) {
            Oper<OPER>::operInt(&dstBitstring[dstIdx],
                                dstPos,
                                srcBitstring[srcIdx]);
            ++dstIdx;
            ++srcIdx;
            numBits -= BITS_PER_INT;
        }
    }
    else {
        // The destination location is aligned.

        while (numBits >= BITS_PER_INT) {
            ALIGNEDOPER(&dstBitstring[dstIdx], srcBitstring[srcIdx]);
            ++dstIdx;
            ++srcIdx;
            numBits -= BITS_PER_INT;
        }
    }
    BSLS_ASSERT(BITS_PER_INT > numBits);

    // Move residual bits.

    Oper<OPER>::operBits(&dstBitstring[dstIdx],
                         dstPos,
                         srcBitstring[srcIdx],
                         numBits);
}

template <void OPER(int *, int, int, int), void ALIGNEDOPER(int *, int)>
void Move<OPER, ALIGNEDOPER>::right(int       *dstBitstring,
                                    int        dstIndex,
                                    const int *srcBitstring,
                                    int        srcIndex,
                                    int        numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    // TBD: Correct
    BSLS_ASSERT(dstBitstring       != srcBitstring
             || dstIndex           >= srcIndex
             || dstIndex + numBits <= srcIndex);  // alias-safe

    // Copy bits to align residual of src on an 'int' boundary.

    int lastDstBit = dstIndex + numBits;
    int dstIdx     = lastDstBit / BITS_PER_INT;
    int dstPos     = lastDstBit % BITS_PER_INT;

    int lastSrcBit = srcIndex + numBits;
    int srcIdx     = lastSrcBit / BITS_PER_INT;
    int num        = lastSrcBit % BITS_PER_INT;
    if (num) {
        if (num >= numBits) {
            dstPos -= numBits;
            if (dstPos < 0) {
                dstPos += BITS_PER_INT;
                --dstIdx;
            }
            Oper<OPER>::operBits(&dstBitstring[dstIdx],
                                 dstPos,
                                 srcBitstring[srcIdx] >> (num - numBits),
                                 numBits);
            return;                                                   // RETURN
        }
        dstPos -= num;
        if (dstPos < 0) {
            dstPos += BITS_PER_INT;
            --dstIdx;
        }
        Oper<OPER>::operBits(&dstBitstring[dstIdx],
                             dstPos,
                             srcBitstring[srcIdx],
                             num);
        numBits -= num;
    }

    // Copy full source ints.

    if (dstPos) {
        // Normal case of the destination location being unaligned.

        while (numBits >= BITS_PER_INT) {
            --dstIdx;
            --srcIdx;
            Oper<OPER>::operInt(&dstBitstring[dstIdx],
                                dstPos,
                                srcBitstring[srcIdx]);
            numBits -= BITS_PER_INT;
        }
    }
    else {
        // The destination location is aligned.

        while (numBits >= BITS_PER_INT) {
            --dstIdx;
            --srcIdx;
            ALIGNEDOPER(&dstBitstring[dstIdx], srcBitstring[srcIdx]);
            numBits -= BITS_PER_INT;
        }
    }
    BSLS_ASSERT(BITS_PER_INT > numBits);

    // Move residual bits.

    dstPos -= numBits;
    if (dstPos < 0) {
        dstPos += BITS_PER_INT;
        --dstIdx;
    }
    num = BITS_PER_INT - numBits;
    --srcIdx;
    Oper<OPER>::operBits(&dstBitstring[dstIdx],
                         dstPos,
                         srcBitstring[srcIdx] >> num,
                         numBits);
}

template <int ACCUMULATOR(int)>
int accumulate(const int *array, int numElements)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= numElements);

    int rv = 0;
    int i  = 0;
    switch (numElements % 8) {
      case 7: rv += ACCUMULATOR(array[i]);  ++i;
      case 6: rv += ACCUMULATOR(array[i]);  ++i;
      case 5: rv += ACCUMULATOR(array[i]);  ++i;
      case 4: rv += ACCUMULATOR(array[i]);  ++i;
      case 3: rv += ACCUMULATOR(array[i]);  ++i;
      case 2: rv += ACCUMULATOR(array[i]);  ++i;
      case 1: rv += ACCUMULATOR(array[i]);  ++i;
      default: ;
    }

    int n = numElements / 8;
    while (n) {
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        rv += ACCUMULATOR(array[i]);  ++i;
        --n;
    }

    return rv;
}

bool requiresAliasedOper(const int *dstBitstring,
                         int        dstIndex,
                         const int *srcBitstring,
                         int        srcIndex,
                         int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    int        srcStartIdx = srcIndex / BITS_PER_INT;
    int        srcEndIdx   = (srcIndex + numBits) / BITS_PER_INT;
    const int *srcStartPtr = &srcBitstring[srcStartIdx];
    const int *srcEndPtr   = &srcBitstring[srcEndIdx];

    int        dstStartIdx = dstIndex / BITS_PER_INT;
    const int *dstStartPtr = &dstBitstring[dstStartIdx];

    if (srcStartPtr <= dstStartPtr && dstStartPtr <= srcEndPtr) {

        int srcStartPos = srcIndex % BITS_PER_INT;
        int srcEndPos   = (srcIndex + numBits) % BITS_PER_INT;
        int dstStartPos = dstIndex % BITS_PER_INT;

        if ((srcStartPtr == dstStartPtr && dstStartPos < srcStartPos)
         || (srcEndPtr   == dstStartPtr && srcEndPos   < dstStartPos)) {
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

bool areBitsInIntEqual(int int1, int pos1, int int2, int pos2, int numBits)
    // Compare the specified 'numBits' sequence of bits starting at the
    // specified 'srcPos' in the specified 'srcInt' with the 'numBits'
    // starting at the specified 'dstPos' in the specified 'dstInt'.  The
    // behavior is undefined unless '0 <= srcPos < BITS_PER_INT',
    // '0 <= dstPos < BITS_PER_INT', '0 <= numBits <= BITS_PER_INT',
    // 'srcPos + numBits <= BITS_PER_INT', and
    // 'dstPos + numBits <= BITS_PER_INT'.
{
    BSLS_ASSERT(0 <= pos1);
    BSLS_ASSERT(     pos1 < BITS_PER_INT);
    BSLS_ASSERT(0 <= pos2);
    BSLS_ASSERT(     pos2 < BITS_PER_INT);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= BITS_PER_INT);
    BSLS_ASSERT(pos1 + numBits <= BITS_PER_INT);
    BSLS_ASSERT(pos2 + numBits <= BITS_PER_INT);

    const int bits1 = (unsigned int)
                              (int1 & bdes_BitUtil::oneMask(pos1, numBits))
                                                                     >> pos1;
    const int bits2 = (unsigned int)
                              (int2 & bdes_BitUtil::oneMask(pos2, numBits))
                                                                     >> pos2;
    return bits1 == bits2;
}

void swapBitsInInt(int *int1,
                   int  index1,
                   int *int2,
                   int  index2,
                   int  numBits)
    // Swap the specified 'numBits' sequence of bits starting at the
    // specified 'index1' in the specified 'int1' with the 'numBits'
    // starting at the specified 'index2' in the specified 'int2'.  The
    // behavior is undefined unless '0 <= index1 < BITS_PER_INT',
    // '0 <= index2 < BITS_PER_INT', '0 <= numBits <= BITS_PER_INT',
    // 'int1 + numBits <= BITS_PER_INT', and
    // 'int2 + numBits <= BITS_PER_INT'.
{
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(     index1 < BITS_PER_INT);
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(     index2 < BITS_PER_INT);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= BITS_PER_INT);
    BSLS_ASSERT(index1 + numBits <= BITS_PER_INT);
    BSLS_ASSERT(index2 + numBits <= BITS_PER_INT);

    const int bits1 = bdeu_BitstringUtil::get(int1, index1, numBits);
    const int bits2 = bdeu_BitstringUtil::get(int2, index2, numBits);
    bdes_BitUtil::replaceValue(int1, index1, bits2, numBits);
    bdes_BitUtil::replaceValue(int2, index2, bits1, numBits);
}

}  // close unnamed namespace

namespace BloombergLP {

                        // =========================
                        // struct bdeu_BitstringUtil
                        // =========================

// CLASS METHODS

                             // Manipulators

void bdeu_BitstringUtil::andEqual(int       *dstBitstring,
                                  int        dstIndex,
                                  const int *srcBitstring,
                                  int        srcIndex,
                                  int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (requiresAliasedOper(dstBitstring,
                            dstIndex,
                            srcBitstring,
                            srcIndex,
                            numBits)) {
        Move<bdes_BitUtil::andEqual,
             Bitwise::andEq>::right(dstBitstring,
                                    dstIndex,
                                    srcBitstring,
                                    srcIndex,
                                    numBits);
    }
    else {
        Move<bdes_BitUtil::andEqual,
             Bitwise::andEq>::left(dstBitstring,
                                   dstIndex,
                                   srcBitstring,
                                   srcIndex,
                                   numBits);
    }
}

void bdeu_BitstringUtil::copy(int       *dstBitstring,
                              int        dstIndex,
                              const int *srcBitstring,
                              int        srcIndex,
                              int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    if (requiresAliasedOper(dstBitstring,
                            dstIndex,
                            srcBitstring,
                            srcIndex,
                            numBits)) {
        Move<bdes_BitUtil::replaceValue, Bitwise::setEq>::right(dstBitstring,
                                                                dstIndex,
                                                                srcBitstring,
                                                                srcIndex,
                                                                numBits);
    }
    else {
        Move<bdes_BitUtil::replaceValue, Bitwise::setEq>::left(dstBitstring,
                                                               dstIndex,
                                                               srcBitstring,
                                                               srcIndex,
                                                               numBits);
    }
}

void bdeu_BitstringUtil::copyRaw(int       *dstBitstring,
                                 int        dstIndex,
                                 const int *srcBitstring,
                                 int        srcIndex,
                                 int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (numBits > 0) {
        Move<bdes_BitUtil::replaceValue, Bitwise::setEq>::left(dstBitstring,
                                                               dstIndex,
                                                               srcBitstring,
                                                               srcIndex,
                                                               numBits);
    }
}

void bdeu_BitstringUtil::insertRaw(int *bitstring,
                                   int  length,
                                   int  dstIndex,
                                   int  numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits || dstIndex == length) {
        return;                                                       // RETURN
    }

    Move<bdes_BitUtil::replaceValue, Bitwise::setEq>::right(bitstring,
                                                            dstIndex + numBits,
                                                            bitstring,
                                                            dstIndex,
                                                            length - dstIndex);
}

void bdeu_BitstringUtil::minusEqual(int       *dstBitstring,
                                    int        dstIndex,
                                    const int *srcBitstring,
                                    int        srcIndex,
                                    int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (requiresAliasedOper(dstBitstring,
                            dstIndex,
                            srcBitstring,
                            srcIndex,
                            numBits)) {
        Move<bdes_BitUtil::minusEqual,
            Bitwise::minusEq>::right(dstBitstring,
                                     dstIndex,
                                     srcBitstring,
                                     srcIndex,
                                     numBits);
    }
    else {
        Move<bdes_BitUtil::minusEqual,
            Bitwise::minusEq>::left(dstBitstring,
                                    dstIndex,
                                    srcBitstring,
                                    srcIndex,
                                    numBits);
    }
}

void bdeu_BitstringUtil::orEqual(int       *dstBitstring,
                                 int        dstIndex,
                                 const int *srcBitstring,
                                 int        srcIndex,
                                 int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (requiresAliasedOper(dstBitstring,
                            dstIndex,
                            srcBitstring,
                            srcIndex,
                            numBits)) {
        Move<bdes_BitUtil::orEqual,
            Bitwise::orEq>::right(dstBitstring,
                                  dstIndex,
                                  srcBitstring,
                                  srcIndex,
                                  numBits);
    }
    else {
        Move<bdes_BitUtil::orEqual,
            Bitwise::orEq>::left(dstBitstring,
                                 dstIndex,
                                 srcBitstring,
                                 srcIndex,
                                 numBits);
    }
}

void bdeu_BitstringUtil::remove(int *bitstring,
                                int  length,
                                int  index,
                                int  numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= length);

    if (numBits > 0) {
        const int remBits = length - index - numBits;

        // copy numBits starting index + numBits to index
        Move<bdes_BitUtil::replaceValue, Bitwise::setEq>::left(bitstring,
                                                               index,
                                                               bitstring,
                                                               index + numBits,
                                                               remBits);
    }
}

void bdeu_BitstringUtil::set(int *bitstring, int index, bool value)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int pos = index % BITS_PER_INT;

    if (value) {
        bitstring[idx] |= bdes_BitUtil::eqMask(pos);
    }
    else {
        bitstring[idx] &= bdes_BitUtil::neMask(pos);
    }
}

void bdeu_BitstringUtil::set(int  *bitstring,
                             int   index,
                             bool  value,
                             int   numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return;
    }

    int idx = index / BITS_PER_INT;
    int pos = index % BITS_PER_INT;
    int numOfBits = BITS_PER_INT - pos <= numBits
                    ? BITS_PER_INT - pos
                    : numBits;

    // Set the partial leading bits
    if (value) {
        bitstring[idx] |= bdes_BitUtil::oneMask( pos, numOfBits);
    }
    else {
        bitstring[idx] &= bdes_BitUtil::zeroMask(pos, numOfBits);
    }

    numBits -= numOfBits;
    if (numBits <= 0) {
        return;
    }

    ++idx;

    // Set the integers with whole bits
    const int VALUE = value ? bdes_BitUtil::oneMask(0, BITS_PER_INT) : 0;
    while (numBits > BITS_PER_INT) {
        bitstring[idx] = VALUE;
        numBits -= BITS_PER_INT;
        ++idx;
    }

    // Set the partial trailing bits
    if (value) {
        bitstring[idx] |= bdes_BitUtil::oneMask( 0, numBits);
    }
    else {
        bitstring[idx] &= bdes_BitUtil::zeroMask(0, numBits);
    }
}

void bdeu_BitstringUtil::swapRaw(int *bitstring1,
                                 int  index1,
                                 int *bitstring2,
                                 int  index2,
                                 int  numBits)
{
    BSLS_ASSERT(bitstring1);
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(bitstring2);
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    // Normalize pointers and indices
    int idx1 = index1 / BITS_PER_INT;
    int pos1 = index1 % BITS_PER_INT;

    int idx2 = index2 / BITS_PER_INT;
    int pos2 = index2 % BITS_PER_INT;

    int rem1 = BITS_PER_INT - pos1;
    int rem2 = BITS_PER_INT - pos2;

    int *int1 = &bitstring1[idx1];
    int *int2 = &bitstring2[idx2];

    if (rem1 == rem2) {
        rem1 = rem1 < numBits ? rem1 : numBits;

        // Copy partial bits
        swapBitsInInt(int1, pos1, int2, pos2, rem1);

        numBits -= rem1;
        if (numBits <= 0) {
            return;                                                   // RETURN
        }

        while (numBits > BITS_PER_INT) {
            ++int1;
            ++int2;

            bsl::swap(*int1, *int2);
            numBits -= BITS_PER_INT;
        }

        swapBitsInInt(++int1, 0, ++int2, 0, numBits);
    }
    else {
        if (rem1 > rem2) {
            bsl::swap(bitstring1, bitstring2);
            bsl::swap(int1, int2);
            bsl::swap(idx1, idx2);
            bsl::swap(pos1, pos2);
            bsl::swap(rem1, rem2);
        }

        do {
            rem1 = rem1 < numBits ? rem1 : numBits;

            swapBitsInInt(int1, pos1, int2, pos2, rem1);

            numBits -= rem1;
            if (numBits <= 0) {
                return;                                               // RETURN
            }

            ++int1;
            rem2 -= rem1;
            pos2 += rem1;
            pos1  = 0;

            rem2 = rem2 < numBits ? rem2 : numBits;

            swapBitsInInt(int1, pos1, int2, pos2, rem2);

            numBits -= rem2;
            if (numBits <= 0) {
                return;                                               // RETURN
            }

            ++int2;
            rem1 = BITS_PER_INT - rem2;
            pos1 = rem2;
            rem2 = BITS_PER_INT;
            pos2 = 0;
        } while (numBits > 0);
    }
}

void bdeu_BitstringUtil::toggle(int *bitstring, int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    int idx = index / BITS_PER_INT;
    int pos = index % BITS_PER_INT;

    // Toggle unaligned initial bits.

    if (pos) {
        const int num = BITS_PER_INT - pos;
        if (num >= numBits) {
            Oper<bdes_BitUtil::replaceValue>::operBits(
                                                      &bitstring[idx],
                                                      pos,
                                                      (~bitstring[idx]) >> pos,
                                                      numBits);
            return;                                                   // RETURN
        }
        Oper<bdes_BitUtil::replaceValue>::operBits(&bitstring[idx],
                                                   pos,
                                                   (~bitstring[idx]) >> pos,
                                                   num);
        numBits -= num;
        ++idx;
    }

    // Toggle full ints.

    while (numBits >= BITS_PER_INT) {
        bitstring[idx] ^= ~0;
        ++idx;
        numBits -= BITS_PER_INT;
    }

    // Toggle trailing bits.
    if (numBits > 0) {
        Oper<bdes_BitUtil::replaceValue>::operBits(&bitstring[idx],
                                                   0,
                                                   ~bitstring[idx],
                                                   numBits);
    }
}

void bdeu_BitstringUtil::xorEqual(int       *dstBitstring,
                                  int        dstIndex,
                                  const int *srcBitstring,
                                  int        srcIndex,
                                  int        numBits)
{
    BSLS_ASSERT(dstBitstring);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(srcBitstring);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);

    if (requiresAliasedOper(dstBitstring,
                            dstIndex,
                            srcBitstring,
                            srcIndex,
                            numBits)) {
        Move<bdes_BitUtil::xorEqual, Bitwise::xorEq>::right(dstBitstring,
                                                            dstIndex,
                                                            srcBitstring,
                                                            srcIndex,
                                                            numBits);
    }
    else {
        Move<bdes_BitUtil::xorEqual, Bitwise::xorEq>::left(dstBitstring,
                                                           dstIndex,
                                                           srcBitstring,
                                                           srcIndex,
                                                           numBits);
    }
}

bsl::ostream& bdeu_BitstringUtil::print(bsl::ostream&  stream,
                                        const int     *bitstring,
                                        int            numBits,
                                        int            level,
                                        int            spacesPerLevel,
                                        int            unitsPerLine,
                                        int            bitsPerUnit)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 < unitsPerLine);
    BSLS_ASSERT(0 < bitsPerUnit);

    if (!stream) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '[';
    if (0 == numBits) {
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << ']';
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
        return stream;                                                // RETURN
    }

    if (level < 0) {
        level = -level;
    }
    int levelPlus1  = level + 1;

    bdeu_Print::newlineAndIndent(stream, levelPlus1, spacesPerLevel);

    int idx                 = 0;
    int digitsInUnitPrinted = 0;
    int unitsPrinted        = 0;

    int mask;
    while (numBits > BITS_PER_INT) {
        mask = 1;
        for (int i = 0; i < BITS_PER_INT; ++i) {
            stream << (bitstring[idx] & mask ? '1' : '0');
            ++digitsInUnitPrinted;
            if (digitsInUnitPrinted == bitsPerUnit) {
                digitsInUnitPrinted = 0;
                ++unitsPrinted;
                if (unitsPrinted == unitsPerLine) {
                    unitsPrinted = 0;
                    bdeu_Print::newlineAndIndent(stream,
                                                 levelPlus1,
                                                 spacesPerLevel);
                }
                else if (i <= BITS_PER_INT - 1) {
                    stream << ' ';
                }
            }
            mask <<= 1;
        }
        ++idx;
        numBits -= BITS_PER_INT;
    }

    mask = 1;
    for (int i = 0; i < numBits; ++i) {
        stream << (bitstring[idx] & mask ? '1' : '0');
        ++digitsInUnitPrinted;
        if (digitsInUnitPrinted == bitsPerUnit) {
            digitsInUnitPrinted = 0;
            ++unitsPrinted;
            if (unitsPrinted == unitsPerLine) {
                unitsPrinted = 0;
                if (i + 1 == numBits) {
                    stream << ((spacesPerLevel >= 0) ? '\n' : ' ');
                }
                else {
                    bdeu_Print::newlineAndIndent(stream,
                                                 levelPlus1,
                                                 spacesPerLevel);
                }
            }
            else if (i < numBits - 1) {
                stream << ' ';
            }
        }
        mask <<= 1;
    }

    if ((0 != digitsInUnitPrinted || unitsPrinted != unitsPerLine)
     && spacesPerLevel >= 0) {
        stream << '\n';
    }

    if (spacesPerLevel >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    else {
        stream << ' ';
    }

    stream << ']';
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                             // Accessors

bool bdeu_BitstringUtil::areEqual(const int *bitstring1,
                                  int        index1,
                                  const int *bitstring2,
                                  int        index2,
                                  int        numBits)
{
    BSLS_ASSERT(bitstring1);
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(bitstring2);
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return true;                                                  // RETURN
    }

    int idx1 = index1 / BITS_PER_INT;
    int pos1 = index1 % BITS_PER_INT;
    int rem1 = BITS_PER_INT - pos1;

    int idx2 = index2 / BITS_PER_INT;
    int pos2 = index2 % BITS_PER_INT;
    int rem2 = BITS_PER_INT - pos2;

    const int *int1 = &bitstring1[idx1];
    const int *int2 = &bitstring2[idx2];

    if (rem1 == rem2) {
        rem1 = rem1 < numBits ? rem1 : numBits;

        if (!areBitsInIntEqual(*int1, pos1, *int2, pos2, rem1)) {
            return false;                                             // RETURN
        }

        numBits -= rem1;
        if (numBits <= 0) {
            return true;                                              // RETURN
        }

        while (numBits > BITS_PER_INT) {
            ++int1;
            ++int2;
            if (*int1 != *int2) {
                return false;                                         // RETURN
            }
            numBits -= BITS_PER_INT;
        }

        return areBitsInIntEqual(*++int1, 0, *++int2, 0, numBits);    // RETURN
    }

    if (rem1 > rem2) {
        bsl::swap(bitstring1, bitstring2);
        bsl::swap(int1, int2);
        bsl::swap(idx1, idx2);
        bsl::swap(pos1, pos2);
        bsl::swap(rem1, rem2);
    }

    do {
        rem1 = rem1 < numBits ? rem1 : numBits;

        if (!areBitsInIntEqual(*int1, pos1, *int2, pos2, rem1)) {
            return false;                                             // RETURN
        }

        numBits -= rem1;
        if (numBits <= 0) {
            return true;                                              // RETURN
        }

        ++int1;
        rem2 -= rem1;
        pos2 += rem1;
        pos1 = 0;

        rem2 = rem2 < numBits ? rem2 : numBits;

        if (!areBitsInIntEqual(*int1, pos1, *int2, pos2, rem2)) {
            return false;                                             // RETURN
        }

        numBits -= rem2;
        if (numBits <= 0) {
            return true;                                              // RETURN
        }

        ++int2;
        rem1 = BITS_PER_INT - rem2;
        pos1 = rem2;
        rem2 = BITS_PER_INT;
        pos2 = 0;
    } while (numBits > 0);

    return true;
}

bool bdeu_BitstringUtil::isAny0(const int *bitstring,
                                int        index,
                                int        numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    int idx       = index / BITS_PER_INT;
    int pos       = index % BITS_PER_INT;
    int numOfBits = BITS_PER_INT - pos <= numBits
                    ? BITS_PER_INT - pos
                    : numBits;

    if (~bitstring[idx] & bdes_BitUtil::oneMask(pos, numOfBits)) {
        return true;                                                  // RETURN
    }

    numBits -= numOfBits;
    if (numBits <= 0) {
        return false;                                                 // RETURN
    }

    ++idx;

    while (numBits > BITS_PER_INT) {
        if (bdes_BitUtil::isAnySetZero(bitstring[idx])) {
            return true;                                              // RETURN
        }
        ++idx;
        numBits -= BITS_PER_INT;
    }

    return ~bitstring[idx] & bdes_BitUtil::oneMask(0, numBits);
}

bool bdeu_BitstringUtil::isAny1(const int *bitstring,
                                int        index,
                                int        numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    int idx = index / BITS_PER_INT;
    int pos = index % BITS_PER_INT;
    int numOfBits = BITS_PER_INT - pos <= numBits
                    ? BITS_PER_INT - pos
                    : numBits;

    if (bitstring[idx] & bdes_BitUtil::oneMask(pos, numOfBits)) {
        return true;                                                  // RETURN
    }

    numBits -= numOfBits;
    if (numBits <= 0) {
        return false;                                                 // RETURN
    }

    ++idx;

    while (numBits > BITS_PER_INT) {
        if (bdes_BitUtil::isAnySetOne(bitstring[idx])) {
            return true;                                              // RETURN
        }
        ++idx;
        numBits -= BITS_PER_INT;
    }

    return bitstring[idx] & bdes_BitUtil::oneMask(0, numBits);
}

int bdeu_BitstringUtil::find0AtLargestIndex(const int *bitstring, int length)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);

    if (length) {
        const int idx    = (length - 1) / BITS_PER_INT;
        const int endPos = (length - 1) % BITS_PER_INT + 1;
        const int rv     = bdes_BitUtil::find0AtLargestIndex(
                                  bdes_BitUtil::setOne(bitstring[idx],
                                                       endPos,
                                                       BITS_PER_INT - endPos));
        if (-1 != rv) {
            return idx * BITS_PER_INT + rv;                           // RETURN
        }
        for (int i = idx - 1; i >= 0; --i) {
            const int value = bitstring[i];
            if (~value) {
                return i * BITS_PER_INT
                     + bdes_BitUtil::find0AtLargestIndex(value);      // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtLargestIndexGE(const int *bitstring,
                                              int        length,
                                              int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtLargestIndexGE(
                                      bdes_BitUtil::setOne(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                      index % BITS_PER_INT);
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtLargestIndex(
                                     bdes_BitUtil::setOne(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = len - 1; i > idx; --i) {
                const int value = bitstring[i];
                if (~value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find0AtLargestIndex(value);  // RETURN
                }
            }
            rv = bdes_BitUtil::find0AtLargestIndexGE(bitstring[idx],
                                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtLargestIndexGT(const int *bitstring,
                                              int        length,
                                              int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtLargestIndexGT(
                                      bdes_BitUtil::setOne(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                      index % BITS_PER_INT);
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtLargestIndex(
                                     bdes_BitUtil::setOne(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = len - 1; i > idx; --i) {
                const int value = bitstring[i];
                if (~value) {
                    return i * BITS_PER_INT
                        + bdes_BitUtil::find0AtLargestIndex(value);   // RETURN
                }
            }
            rv = bdes_BitUtil::find0AtLargestIndexGT(bitstring[idx],
                                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtLargestIndexLE(const int *bitstring, int index)
{
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int rv  = bdes_BitUtil::find0AtLargestIndexLE(bitstring[idx],
                                                        index % BITS_PER_INT);
    if (-1 != rv) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    for (int i = idx - 1; i >= 0; --i) {
        const int value = bitstring[i];
        if (~value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find0AtLargestIndex(value);        // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtLargestIndexLT(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int rv  = bdes_BitUtil::find0AtLargestIndexLT(bitstring[idx],
                                                        index % BITS_PER_INT);
    if (-1 != rv) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    for (int i = idx - 1; i >= 0; --i) {
        const int value = bitstring[i];
        if (~value) {
            return i * BITS_PER_INT
                + bdes_BitUtil::find0AtLargestIndex(value);           // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtSmallestIndex(const int *bitstring, int length)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        for (int i = 0; i < len; ++i) {
            const int value = bitstring[i];
            if (~value) {
                return i * BITS_PER_INT
                     + bdes_BitUtil::find0AtSmallestIndex(value);     // RETURN
            }
        }

        length = (length - 1) % BITS_PER_INT + 1;
        int rv = bdes_BitUtil::find0AtSmallestIndex(
                                  bdes_BitUtil::setOne(bitstring[len],
                                                       length,
                                                       BITS_PER_INT - length));
        if (rv < BITS_PER_INT) {
            return len * BITS_PER_INT + rv;                           // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtSmallestIndexGE(const int *bitstring,
                                               int        length,
                                               int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtSmallestIndexGE(
                                      bdes_BitUtil::setOne(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                      index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            int rv = bdes_BitUtil::find0AtSmallestIndexGE(
                                                         bitstring[idx],
                                                         index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = idx + 1; i < len; ++i) {
                const int value = bitstring[i];
                if (~value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find0AtSmallestIndex(value); // RETURN
                }
            }
            const int pos = (length - 1) % BITS_PER_INT + 1;
            rv = bdes_BitUtil::find0AtSmallestIndex(
                                     bdes_BitUtil::setOne(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtSmallestIndexGT(const int *bitstring,
                                               int        length,
                                               int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find0AtSmallestIndexGT(
                                      bdes_BitUtil::setOne(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                      index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            int rv = bdes_BitUtil::find0AtSmallestIndexGT(
                                                         bitstring[idx],
                                                         index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = idx + 1; i < len; ++i) {
                const int value = bitstring[i];
                if (~value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find0AtSmallestIndex(value); // RETURN
                }
            }
            const int pos = (length - 1) % BITS_PER_INT + 1;
            rv = bdes_BitUtil::find0AtSmallestIndex(
                                     bdes_BitUtil::setOne(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtSmallestIndexLE(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    for (int i = 0; i < idx; ++i) {
        const int value = bitstring[i];
        if (~value) {
            return i * BITS_PER_INT
                + bdes_BitUtil::find0AtSmallestIndex(value);          // RETURN
        }
    }
    const int rv = bdes_BitUtil::find0AtSmallestIndexLE(bitstring[idx],
                                                        index % BITS_PER_INT);

    if (rv < BITS_PER_INT) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    return -1;
}

int bdeu_BitstringUtil::find0AtSmallestIndexLT(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    for (int i = 0; i < idx; ++i) {
        const int value = bitstring[i];
        if (~value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find0AtSmallestIndex(value);       // RETURN
        }
    }
    const int rv = bdes_BitUtil::find0AtSmallestIndexLT(bitstring[idx],
                                                        index % BITS_PER_INT);

    if (rv < BITS_PER_INT) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtLargestIndex(const int *bitstring, int length)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);

    if (length) {
        const int idx    = (length - 1) / BITS_PER_INT;
        const int endPos = (length - 1) % BITS_PER_INT + 1;
        const int rv     = bdes_BitUtil::find1AtLargestIndex(
                                 bdes_BitUtil::setZero(bitstring[idx],
                                                       endPos,
                                                       BITS_PER_INT - endPos));
        if (-1 != rv) {
            return idx * BITS_PER_INT + rv;                           // RETURN
        }
        for (int i = idx - 1; i >= 0; --i) {
            const int value = bitstring[i];
            if (value) {
                return i * BITS_PER_INT
                       + bdes_BitUtil::find1AtLargestIndex(value);    // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtLargestIndexGE(const int *bitstring,
                                              int        length,
                                              int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtLargestIndexGE(
                                     bdes_BitUtil::setZero(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtLargestIndex(
                                    bdes_BitUtil::setZero(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }

            for (int i = len - 1; i > idx; --i) {
                const int value = bitstring[i];
                if (value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find1AtLargestIndex(value);  // RETURN
                }
            }
            rv = bdes_BitUtil::find1AtLargestIndexGE(bitstring[idx],
                                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtLargestIndexGT(const int *bitstring,
                                              int        length,
                                              int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtLargestIndexGT(
                                     bdes_BitUtil::setZero(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtLargestIndex(
                                    bdes_BitUtil::setZero(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (-1 != rv) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }

            for (int i = len - 1; i > idx; --i) {
                const int value = bitstring[i];
                if (value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find1AtLargestIndex(value);  // RETURN
                }
            }
            rv = bdes_BitUtil::find1AtLargestIndexGT(bitstring[idx],
                                                     index % BITS_PER_INT);
            if (-1 != rv) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtLargestIndexLE(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int rv  = bdes_BitUtil::find1AtLargestIndexLE(bitstring[idx],
                                                        index % BITS_PER_INT);
    if (-1 != rv) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    for (int i = idx - 1; i >= 0; --i) {
        const int value = bitstring[i];
        if (value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find1AtLargestIndex(value);        // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtLargestIndexLT(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int rv  = bdes_BitUtil::find1AtLargestIndexLT(bitstring[idx],
                                                        index % BITS_PER_INT);
    if (-1 != rv) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    for (int i = idx - 1; i >= 0; --i) {
        const int value = bitstring[i];
        if (value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find1AtLargestIndex(value);        // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtSmallestIndex(const int *bitstring, int length)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        for (int i = 0; i < len; ++i) {
            const int value = bitstring[i];
            if (value) {
                return i * BITS_PER_INT
                       + bdes_BitUtil::find1AtSmallestIndex(value);   // RETURN
            }
        }

        length = (length - 1) % BITS_PER_INT + 1;
        int rv  = bdes_BitUtil::find1AtSmallestIndex(
                                 bdes_BitUtil::setZero(bitstring[len],
                                                       length,
                                                       BITS_PER_INT - length));
        if (rv < BITS_PER_INT) {
            return len * BITS_PER_INT + rv;                           // RETURN
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtSmallestIndexGE(const int *bitstring,
                                               int        length,
                                               int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtSmallestIndexGE(
                                     bdes_BitUtil::setZero(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                     index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            int rv = bdes_BitUtil::find1AtSmallestIndexGE(
                                                         bitstring[idx],
                                                         index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = idx + 1; i < len; ++i) {
                const int value = bitstring[i];
                if (value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find1AtSmallestIndex(value); // RETURN
                }
            }
            const int pos = (length - 1) % BITS_PER_INT + 1;
            rv = bdes_BitUtil::find1AtSmallestIndex(
                                    bdes_BitUtil::setZero(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtSmallestIndexGT(const int *bitstring,
                                               int        length,
                                               int        index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length);

    if (length) {
        const int len = (length - 1) / BITS_PER_INT;
        const int idx = index / BITS_PER_INT;
        if (len == idx) {
            const int pos = (length - 1) % BITS_PER_INT + 1;
            int       rv  = bdes_BitUtil::find1AtSmallestIndexGT(
                                     bdes_BitUtil::setZero(bitstring[len],
                                                           pos,
                                                           BITS_PER_INT - pos),
                                     index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
        else {
            int rv = bdes_BitUtil::find1AtSmallestIndexGT(
                                                         bitstring[idx],
                                                         index % BITS_PER_INT);
            if (rv < BITS_PER_INT) {
                return idx * BITS_PER_INT + rv;                       // RETURN
            }
            for (int i = idx + 1; i < len; ++i) {
                const int value = bitstring[i];
                if (value) {
                    return i * BITS_PER_INT
                         + bdes_BitUtil::find1AtSmallestIndex(value); // RETURN
                }
            }
            const int pos = (length - 1) % BITS_PER_INT + 1;
            rv = bdes_BitUtil::find1AtSmallestIndex(
                                    bdes_BitUtil::setZero(bitstring[len],
                                                          pos,
                                                          BITS_PER_INT - pos));
            if (rv < BITS_PER_INT) {
                return len * BITS_PER_INT + rv;                       // RETURN
            }
        }
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtSmallestIndexLE(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    for (int i = 0; i < idx; ++i) {
        const int value = bitstring[i];
        if (value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find1AtSmallestIndex(value);       // RETURN
        }
    }
    const int rv = bdes_BitUtil::find1AtSmallestIndexLE(bitstring[idx],
                                                        index % BITS_PER_INT);

    if (rv < BITS_PER_INT) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    return -1;
}

int bdeu_BitstringUtil::find1AtSmallestIndexLT(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    for (int i = 0; i < idx; ++i) {
        const int value = bitstring[i];
        if (value) {
            return i * BITS_PER_INT
                   + bdes_BitUtil::find1AtSmallestIndex(value);       // RETURN
        }
    }
    const int rv = bdes_BitUtil::find1AtSmallestIndexLT(bitstring[idx],
                                                        index % BITS_PER_INT);

    if (rv < BITS_PER_INT) {
        return idx * BITS_PER_INT + rv;                               // RETURN
    }
    return -1;
}

bool bdeu_BitstringUtil::get(const int *bitstring, int index)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);

    const int idx = index / BITS_PER_INT;
    const int pos = index % BITS_PER_INT;

    return bitstring[idx] & bdes_BitUtil::eqMask(pos);
}

int bdeu_BitstringUtil::get(const int *bitstring, int index, int numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= BITS_PER_INT);

    if (0 == numBits) {
        return 0;                                                     // RETURN
    }

    int idx       = index / BITS_PER_INT;
    int pos       = index % BITS_PER_INT;
    int numRem    = BITS_PER_INT - pos;
    int numOfBits = numBits <= numRem ? numBits : numRem;
    int result    = ((unsigned int) (bitstring[idx]
                              & bdes_BitUtil::oneMask(pos, numOfBits))) >> pos;

    numBits -= numOfBits;
    if (numBits <= 0) {
        return result;                                                // RETURN
    }

    result |= (bitstring[idx + 1]
                                & bdes_BitUtil::oneMask(0, numBits)) << numRem;
    return result;
}

int bdeu_BitstringUtil::num0(const int *bitstring, int index, int numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    const int idx     = index / BITS_PER_INT;
    const int pos     = index % BITS_PER_INT;
    int       remBits = numBits < BITS_PER_INT - pos
                        ? numBits
                        : BITS_PER_INT - pos;

    int numSet = bdes_BitUtil::numSetZero(bitstring[idx]
                                       | bdes_BitUtil::zeroMask(pos, remBits));

    remBits = numBits - remBits;

    if (remBits <= 0) {
        return numSet;
    }

    const int numInts = remBits / BITS_PER_INT;
    remBits           = remBits % BITS_PER_INT;

    numSet += accumulate<bdes_BitUtil::numSetZero>(&bitstring[idx + 1],
                                                   numInts);

    if (remBits) {
        numSet += bdes_BitUtil::numSetZero(bitstring[idx + numInts + 1]
                              | bdes_BitUtil::oneMask(remBits,
                                                      BITS_PER_INT - remBits));
    }

    return numSet;
}

int bdeu_BitstringUtil::num1(const int *bitstring, int index, int numBits)
{
    BSLS_ASSERT(bitstring);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);

    const int idx     = index / BITS_PER_INT;
    const int pos     = index % BITS_PER_INT;
    int       remBits = numBits < BITS_PER_INT - pos
                        ? numBits
                        : BITS_PER_INT - pos;

    int numSet = bdes_BitUtil::numSetOne(bitstring[idx]
                                     & bdes_BitUtil::oneMask(pos, remBits));

    remBits = numBits - remBits;

    if (remBits <= 0) {
        return numSet;
    }

    const int numInts = remBits / BITS_PER_INT;
    remBits           = remBits % BITS_PER_INT;

    numSet += accumulate<bdes_BitUtil::numSetOne>(&bitstring[idx + 1],
                                                  numInts);

    if (remBits) {
        numSet += bdes_BitUtil::numSetOne(bitstring[idx + numInts + 1]
                             & bdes_BitUtil::zeroMask(remBits,
                                                      BITS_PER_INT - remBits));
    }

    return numSet;
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
