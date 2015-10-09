// bdlb_bitstringutil.cpp                                             -*-C++-*-
#include <bdlb_bitstringutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_bitstringutil_cpp,"$Id$ $CSID$")

#include <bdlb_bitmaskutil.h>
#include <bdlb_bitstringimputil.h>
#include <bdlb_bitutil.h>

#include <bslmf_assert.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsl_c_limits.h>    // 'CHAR_BIT'

using namespace BloombergLP;
using bsl::size_t;
using bsl::uint64_t;

namespace {

typedef bdlb::BitUtil                 BitUtil;
typedef bdlb::BitMaskUtil             BitMaskUtil;
typedef bdlb::BitStringImpUtil        Imp;
typedef bsls::Types::Int64            Int64;
typedef bsls::Types::IntPtr           IntPtr;
typedef bsls::Types::UintPtr          UintPtr;

enum { k_BITS_PER_UINT64 = bdlb::BitStringUtil::k_BITS_PER_UINT64 };

// For 'k_ALIGNMENT' we want the guaranteed alignment of 'uint64_t' variables.
// Unfortunately, 'bsls::AlignmentFromType' returns the guaranteed alignment
// of 'uint64_t' variables within structs, which is different on Windows than
// 'uint64_t' variables on the stack.

#if defined(BSLS_PLATFORM_OS_WINDOWS)
enum { k_ALIGNMENT       = 4 };
#else
enum { k_ALIGNMENT       = bsls::AlignmentFromType<uint64_t>::VALUE };
#endif

BSLMF_ASSERT(sizeof(uint64_t) * CHAR_BIT == k_BITS_PER_UINT64);
BSLMF_ASSERT(0 == (k_BITS_PER_UINT64 & (k_BITS_PER_UINT64 - 1))); // power of 2

#if !defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    // This table shows two bit strings made of arrays of 'uint64_t', showing
    // the bit index of the low-order bit of consecutive bytes.  As you see, in
    // the case of little-endian, the index always increases by 8 with every
    // byte.  In the case of big-endian, it's much more complicated, with the
    // index *decreasing* by 8 in most cases, yet *increasing* by a lot on
    // 64-bit word boundaries.
    //
    // The bit strings consist of two 'uint64_t' words, or 16 bytes, or 128
    // bits, where bytes 0-7 index into the first word of the bit string, and
    // bytes 8-15 index the second word of the bit string.
    //
    // ----------------------------------+-------------------------------------
    //           Little-Endian           |             Big-Endian
    // ----------------------------------+-------------------------------------
    // Byte Index          Bit Index     |   Byte Index      Bit Index
    //                  (low-order-bit)  |                 (low-order bit)
    //                                   |
    //      0                0           |         0             56
    //      1                8           |         1             48
    //      2               16           |         2             40
    //      3               24           |         3             32
    //      4               32           |         4             24
    //      5               40           |         5             16
    //      6               48           |         6              8
    //      7               54           |         7              0
    //      8               64           |         8            120
    //      9               72           |         9            112
    //     10               80           |        10            104
    //     11               88           |        11             96
    //     12               96           |        12             88
    //     13              104           |        13             80
    //     14              112           |        14             72
    //     15              120           |        15             64
    // ----------------------------------+-------------------------------------
    //
    // Our 'BitPtrDiff' class below can represent a distance, in bits, between
    // any two '(pointer, index)' pairs.  Due to the above, if the pointers are
    // not 64-bit word-aligned on a big-endian machine, this becomes
    // problematic -- one needs to know where the bit arrays begin to know
    // where the 64-bit word boundaries are, and a subtraction between two
    // 'BitPtr's (defined below) becomes problematic unless it is known that
    // the difference between the addresses of both of the bit strings they
    // refer to is a multiple of 'sizeof(uint64_t)'.
    //
    // On a little-endian machine, on the other hand, a distance between any
    // two 'BitPtr's is well-defined regardless of the alignment of the
    // 'BitPtr's.
    //
    // For this reason, we assert on big-endian platforms that the alignment of
    // 'uint64_t's must be at least 'sizeof(uint64_t)'.  If we want to port to
    // a big-endian platform where this is not the case, we will have to
    // re-think the logic of our 'BitPtr' and 'BitPtrDiff' classes.

    BSLMF_ASSERT(k_ALIGNMENT >= sizeof(uint64_t));
#endif

}  // close unnamed namespace

static inline
unsigned int u32(uint64_t value)
    // Return the specified 'value' cast as an 'unsigned int'.  Note that if we
    // want to perform '% k_BITS_PER_UINT64' on a 'size_t', we are only
    // interested in the low-order 6 bits, so it is best to cast it down to 32
    // bits for a cheaper mod operation.  It is important that the type being
    // cast to is 'unsigned int', not 'int', since the 'int' could wind up
    // negative, in which case the '%' operation could return a negative
    // result.
{
    return static_cast<unsigned int>(value);
}

namespace {

                                // ----------------
                                // class BitPtrDiff
                                // ----------------

class BitPtrDiff {
    // This 'class' represents a signed integral value with
    // '<bits in pointer> + 6' bits of resolution.  Thus, it is able to
    // represent the signed distance between any two bits in the address
    // space, and is designed to hold the difference between two objects of
    // type 'BitPtr' defined below.
    //
    // This type has no public manipulators, and variables of this type outside
    // this class are always created 'const'.

    // TYPES
    enum { k_NUM_LO_BITS    = 6,
           k_USED_LO_MASK   = (1 << k_NUM_LO_BITS) - 1,
           k_UNUSED_LO_MASK = static_cast<unsigned>(~k_USED_LO_MASK) };

    BSLMF_ASSERT(k_BITS_PER_UINT64 == (1 << k_NUM_LO_BITS));
    BSLMF_ASSERT(static_cast<unsigned>(-k_BITS_PER_UINT64) ==
                                                             k_UNUSED_LO_MASK);

    // DATA
    IntPtr       d_hi;    // Signed high-order portion.
    unsigned int d_lo;    // Unsigned low-order portion.  Only the low-order 6
                          // bits are used.  Note that this is always a
                          // positive offset, even when 'd_hi' is negative.
                          // Bits above the low-order 6 bits will be set in the
                          // event of overflow.

    // FRIENDS
    friend bool operator< (const BitPtrDiff&, const BitPtrDiff&);
    friend bool operator> (const BitPtrDiff&, const BitPtrDiff&);
    friend bool operator<=(const BitPtrDiff&, const BitPtrDiff&);
    friend bool operator>=(const BitPtrDiff&, const BitPtrDiff&);

  private:
    // NOT IMPLEMENTED
    BitPtrDiff& operator=(const BitPtrDiff&);

  public:
    // CREATORS
    BitPtrDiff(size_t value);                                       // IMPLICIT
        // Create a 'BitPtrDiff' object with the specified 'value'.  The
        // created object will always have a positive value.

    BitPtrDiff(IntPtr hi, unsigned lo);
        // Create a 'BitPtrDiff' object with the specified 'hi' and 'lo'
        // initializing 'd_hi' and 'd_lo' respectively.  The behavior is
        // undefined unless 'lo < k_BITS_PER_UINT64'.

    // BitPtrDiff(const BitPtrDiff&) = default;

    // ~BitPtrDiff() = default;

    // ACCESSORS
    BitPtrDiff operator-() const;
        // Return the negated value of this object.  Note that this is the
        // unary minus operator.
};

                              // ----------------
                              // class BitPtrDiff
                              // ----------------

// CREATORS
inline
BitPtrDiff::BitPtrDiff(size_t value)
: d_hi(value >> k_NUM_LO_BITS)
, d_lo(value & k_USED_LO_MASK)
{
    BSLS_ASSERT_SAFE((static_cast<size_t>(d_hi) * k_BITS_PER_UINT64 | d_lo)
                                                                 == value);
}

inline
BitPtrDiff::BitPtrDiff(IntPtr hi, unsigned lo)
: d_hi(hi)
, d_lo(lo)
{
    BSLS_ASSERT_SAFE(lo < k_BITS_PER_UINT64);
}

// ACCESSOR
inline
BitPtrDiff BitPtrDiff::operator-() const
{
    // Initialize the result with the complement of this object.

    BitPtrDiff ret(~d_hi, k_USED_LO_MASK & ~d_lo);

    // add one

    ++ret.d_lo;
    if (ret.d_lo & k_UNUSED_LO_MASK) {
        // overflow of 'd_lo'

        BSLS_ASSERT_SAFE(k_BITS_PER_UINT64 == ret.d_lo);

        ++ret.d_hi;
        ret.d_lo -= k_BITS_PER_UINT64;
    }

    BSLS_ASSERT_SAFE(0 == (ret.d_lo & k_UNUSED_LO_MASK));

    return ret;
}

// FREE OPERATORS
inline
bool operator<(const BitPtrDiff& lhs, const BitPtrDiff& rhs)
    // Return 'true' if the specified 'lhs' is less than the specified 'rhs',
    // and 'false' otherwise.
{
    return lhs.d_hi <  rhs.d_hi ||
          (lhs.d_hi == rhs.d_hi && lhs.d_lo < rhs.d_lo);
}

inline
bool operator>(const BitPtrDiff& lhs, const BitPtrDiff& rhs)
    // Return 'true' if the specified 'lhs' is greater than the specified
    // 'rhs', and 'false' otherwise.
{
    return lhs.d_hi >  rhs.d_hi ||
          (lhs.d_hi == rhs.d_hi && lhs.d_lo > rhs.d_lo);
}

inline
bool operator<=(const BitPtrDiff& lhs, const BitPtrDiff& rhs)
    // Return 'true' if the specified 'lhs' is less than or equal to the
    // specified 'rhs', and 'false' otherwise.
{
    return lhs.d_hi <  rhs.d_hi ||
          (lhs.d_hi == rhs.d_hi && lhs.d_lo <= rhs.d_lo);
}

inline
bool operator>=(const BitPtrDiff& lhs, const BitPtrDiff& rhs)
    // Return 'true' if the specified 'lhs' is greater than or equal to the
    // specified 'rhs', and 'false' otherwise.
{
    return lhs.d_hi >  rhs.d_hi ||
          (lhs.d_hi == rhs.d_hi && lhs.d_lo >= rhs.d_lo);
}

                              // ------------
                              // class BitPtr
                              // ------------

class BitPtr {
    // This 'class' is a type that can represent a pointer to any bit in the
    // address space.
    //
    // A normal pointer can point to any byte in the address space, but more
    // bits of resolution are needed to point to any bit in the address space.
    // We use a high-order 'IntPtr', 'd_hi', that can refer to any 8-byte
    // aligned 'uint64_t' location, and another 'unsigned int', 'd_lo', of
    // which we use the low-order 6 bits to index into any bit within that
    // 'uint64_t'.  On platforms where a 'uint64_t' may not be 8-byte aligned,
    // this is accommodated by varying 'd_lo' to reflect the difference in
    // offset.
    //
    // Note that, though this 'class' is sort of a pointer class, it has no
    // dereference operators.
    //
    // This 'class' has no manipulators; all objects of this type are declared
    // 'const'.  The only thing that can be done with objects of this type is
    // to subtract one from another, yielding a 'BitPtrDiff' (defined above)
    // that will indicate the distance in bits between the two 'BitPtr's.

    enum { k_NUM_LO_BITS    = 6,
           k_USED_LO_MASK   = (1 << k_NUM_LO_BITS) - 1,
           k_UNUSED_LO_MASK = static_cast<unsigned>(~k_USED_LO_MASK) };

    BSLMF_ASSERT(k_BITS_PER_UINT64 == (1 << k_NUM_LO_BITS));
    BSLMF_ASSERT(static_cast<unsigned>(-k_BITS_PER_UINT64) ==
                                                             k_UNUSED_LO_MASK);

    // DATA
    UintPtr      d_hi;    // High-order part.
    unsigned int d_lo;    // Low-order part.  Only the low-order 6 bits are
                          // used.  Bits above the low-order 6 bits will be set
                          // in cases of overflow / underflow.

    // FRIENDS
    friend BitPtrDiff operator-(const BitPtr&, const BitPtr&);

  private:
    // NOT IMPLEMENTED
    BitPtr(const BitPtr&);
    BitPtr& operator=(const BitPtr&);

  public:
    // CREATORS
    BitPtr(const uint64_t *ptr, size_t index);
        // Create a 'BitPtr' object referring to the bit specified by 'ptr' and
        // 'index', the 'index'th bit of the 'uint64_t' pointed at by 'ptr'.

    // ~BitPtr() = default;
};

                                // ------------
                                // class BitPtr
                                // ------------

// CREATORS
BitPtr::BitPtr(const uint64_t *ptr, size_t index)
{
    enum {
        k_NUM_BYTE_PTR_BITS = 3,
            // Number of low-order bits of 'ptr' that are to be shifted
            // down and added to 'd_lo'.  These bits are necessary to access
            // bytes, but not to access aligned 'uint64_t's.
        k_PTR_MASK = (1 << k_NUM_BYTE_PTR_BITS) - 1,
            // Mask for removing the low-order bits from the pointer part.
        k_SHIFT_UP = k_NUM_LO_BITS - k_NUM_BYTE_PTR_BITS
            // The low-order bits of 'ptrPart' get added to the high-order
            // significant bits of 'd_lo', so they have to be shifted up by
            // this amount.
    };

    enum {
        // Put this in a separate 'enum' declaration so the above remain 32-bit
        // constants.

        k_HI_MASK = static_cast<UintPtr>(k_PTR_MASK) <<
                            (sizeof(d_hi) * CHAR_BIT - k_NUM_BYTE_PTR_BITS)
            // The high-order 3 bits of 'd_hi', which should always be 0.
    };

    ptr  +=     index  / k_BITS_PER_UINT64;
    d_lo =  u32(index) % k_BITS_PER_UINT64;

    const UintPtr ptrPart = reinterpret_cast<UintPtr>(ptr);
    d_hi = ptrPart >> k_NUM_BYTE_PTR_BITS;

    // The following condition is a 'const' expression; the optimizer will
    // eliminate the branch not taken.

    if (k_ALIGNMENT < sizeof(uint64_t)) {
        d_lo += (static_cast<unsigned>(ptrPart) & k_PTR_MASK) << k_SHIFT_UP;

        if (d_lo & k_UNUSED_LO_MASK) {
            // overflow in 'd_lo'

            BSLS_ASSERT_SAFE(k_BITS_PER_UINT64 == (d_lo & k_UNUSED_LO_MASK));

            ++d_hi;
            d_lo -= k_BITS_PER_UINT64;
        }
    }
    else {
        BSLS_ASSERT_SAFE(0 == (ptrPart & k_PTR_MASK));
    }

    BSLS_ASSERT_SAFE(0 == (d_hi & k_HI_MASK));
    BSLS_ASSERT_SAFE(0 == (d_lo & k_UNUSED_LO_MASK));
}

// FREE OPERATORS
inline
BitPtrDiff operator-(const BitPtr& lhs, const BitPtr& rhs)
    // Return a 'BitPtrDiff' representing the distance in bits between the
    // specified 'lhs' and 'rhs'.
{
    enum { k_UNUSED_LO_MASK = BitPtr::k_UNUSED_LO_MASK };

    IntPtr   retHi = lhs.d_hi - rhs.d_hi;
    unsigned retLo = lhs.d_lo - rhs.d_lo;

    if (retLo & k_UNUSED_LO_MASK) {
        // 'retLo' underflowed.

        BSLS_ASSERT_SAFE((retLo & k_UNUSED_LO_MASK) == k_UNUSED_LO_MASK);

        --retHi;
        retLo += k_BITS_PER_UINT64;
    }

    BSLS_ASSERT_SAFE(0 == (retLo & k_UNUSED_LO_MASK));

    return BitPtrDiff(retHi, retLo);
}

                              // -----------
                              // class Mover
                              // -----------

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t     )>
class Mover {
    // This template 'class' provides a namespace for static functions that
    // manipulate bit strings.  The clients (local within this file) use
    // 'move', 'left', and 'right' to apply bitwise-logical operations between
    // bit strings.
    //
    // The two template arguments are functions:
    //..
    // void OPER_DO_BITS(uint64_t *dstWord,
    //                   int       dstIndex,
    //                   uint64_t  srcValue,
    //                   int       numBits);
    //..
    // where 'OPER_DO_BITS' applies some bitwise-logical operation between
    // 'numBits' bits in 'dstWord' and the low-order 'numBits' of 'srcValue',
    // assigning the result to the corresponding bits of 'dstWord'.  The
    // behavior is undefined unless 'dstIndex + numBits <= k_BITS_PER_UINT64',
    // so the operation never affects more than a single 64-bit word.
    //
    // And:
    //..
    // void OPER_DO_ALIGNED_WORD(uint64_t *dstWord,
    //                           uint64_t  srcValue);
    //..
    // where 'OPER_DO_ALIGNED_WORD' applies the same bitwise-logical operation
    // between all bits of '*dstWord' and all bits of 'srcValue', assigning the
    // result to '*dstWord'.  Note that a call to
    // 'OPER_DO_ALIGNED_WORD(dstWord, srcValue)' would have exactly the same
    // effect as 'OPER_DO_BITS(dstWord, 0, srcValue, k_BITS_PER_UINT64)', but
    // 'OPER_DO_ALIGNED_WORD' is much more efficient in that case.

    // PRIVATE CLASS METHODS
    static void doPartialWord(uint64_t *dstBitString,
                              int       dstIndex,
                              uint64_t  srcValue,
                              int       numBits);
        // Set the specified 'numBits' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitString' to the result
        // of the operation 'OPER_DO_BITS' of those bits and the low-order
        // 'numBits' bits in the specified 'srcValue'.  All other bits are
        // unaffected.  The behavior is undefined unless
        // '0 <= dstIndex < k_BITS_PER_UINT64', and
        // '0 <= numBits < k_BITS_PER_UINT64'.  Note that this operation may
        // affect up to two 64-bit words of 'dstBitString'.

    static void doFullNonAlignedWord(uint64_t *dstBitString,
                                     int       dstIndex,
                                     uint64_t  srcValue);
        // Set the 'k_BITS_PER_UINT64' contiguous bits starting at the
        // specified 'dstIndex' in the specified 'dstBitString' to the result
        // of the operation 'OPER_DO_BITS' of those bits and the bits in the
        // specified 'srcValue'.  All other bits are unaffected.  The operation
        // 'OPER_DO_BITS' has arguments: pointer to destination array, index
        // within destination array, source value, and number of bits to apply
        // the operation upon.  The behavior is undefined unless
        // '0 < dstIndex < k_BITS_PER_UINT64'.

  public:
    // PUBLIC CLASS METHODS

                                // directional moves

    static void left(uint64_t       *dstBitString,
                     size_t          dstIndex,
                     const uint64_t *srcBitString,
                     size_t          srcIndex,
                     size_t          numBits);
        // Apply the bitwise-logical operation indicated by 'OPER_DO_BITS' and
        // 'OPER_DO_ALIGNED_WORD' between the specified 'numBits' of the
        // specified 'dstBitString' and the specified 'srcBitString', beginning
        // at the specified 'dstIndex' and the specified 'srcIndex',
        // respectively.  Use 'doPartialWord', 'doFullNonAlignedWord', and
        // 'OPER_DO_ALIGNED_WORD' to apply the operation.  The operation
        // proceeds from the low-order bits to the high-order bits.  All other
        // bits are unaffected.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= srcIndex', 'srcBitString' contains at least
        // 'srcIndex + numBits' bits, '0 <= numBits', and 'dstBitString'
        // contains at least 'dstIndex + numBits' bits.  Note that this method
        // is alias-safe provided the destination range is located at the same
        // place as or below the source range.

    static void right(uint64_t       *dstBitString,
                      size_t          dstIndex,
                      const uint64_t *srcBitString,
                      size_t          srcIndex,
                      size_t          numBits);
        // Apply the bitwise-logical operation indicated by 'OPER_DO_BITS' and
        // 'OPER_DO_ALIGNED_WORD' between the specified 'numBits' of the
        // specified 'dstBitString' and the specified 'srcBitString', beginning
        // at the specified 'dstIndex' and the specified 'srcIndex',
        // respectively.  Use 'doPartialWord', 'doFullNonAlignedWord', and
        // 'OPER_DO_ALIGNED_WORD' to apply the operation.  The operation
        // proceeds from the high-order bits to the low-order bits (e.g., the
        // opposite of 'left').  All other bits are unaffected.  The behavior
        // is undefined unless '0 <= dstIndex', '0 <= srcIndex', 'srcBitString'
        // contains at least 'srcIndex + numBits' bits, '0 <= numBits', and
        // 'dstBitString' contains at least 'dstIndex + numBits' bits.  Note
        // that this method is alias-safe if the destination range is located
        // at the same place as or above the source range.

                                // ambidextrous move

    static void move(uint64_t       *dstBitString,
                     size_t          dstIndex,
                     const uint64_t *srcBitString,
                     size_t          srcIndex,
                     size_t          numBits);
        // Apply the bitwise-logical operation indicated by 'OPER_DO_BITS' and
        // 'OPER_DO_ALIGNED_WORD' between the specified 'numBits' of the
        // specified 'dstBitString' and the specified 'srcBitString', beginning
        // at the specified 'dstIndex' and 'srcIndex', respectively.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // 'srcBitString' contains at least 'srcIndex + numBits' bits,
        // '0 <= numBits', and 'dstBitString' contains at least
        // 'dstIndex + numBits' bits.  Note that this function is implemented
        // by simply calling 'left' or 'right' as appropriate, depending upon
        // whether, and how, 'dstBitString' and 'srcBitString' overlap.
};

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t)>
inline
void Mover<OPER_DO_BITS, OPER_DO_ALIGNED_WORD>::doPartialWord(
                                                        uint64_t *dstBitString,
                                                        int       dstIndex,
                                                        uint64_t  srcValue,
                                                        int       numBits)
{
    BSLS_ASSERT_SAFE(dstBitString);
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex < k_BITS_PER_UINT64);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(     numBits  < k_BITS_PER_UINT64);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    const int dstLen = k_BITS_PER_UINT64 - dstIndex;
    if (numBits <= dstLen) {
        // Fits in the 'dstIndex' element.

        OPER_DO_BITS(dstBitString, dstIndex, srcValue, numBits);
    }
    else {
        // Destination bits span two 'uint64_t' array elements.

        OPER_DO_BITS(dstBitString,   dstIndex,           srcValue,     dstLen);
        OPER_DO_BITS(dstBitString+1,        0, srcValue >> dstLen,
                                                             numBits - dstLen);
    }
}

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t)>
inline
void Mover<OPER_DO_BITS, OPER_DO_ALIGNED_WORD>::doFullNonAlignedWord(
                                                        uint64_t *dstBitString,
                                                        int       dstIndex,
                                                        uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstBitString);
    BSLS_ASSERT_SAFE(0 < dstIndex);
    BSLS_ASSERT_SAFE(    dstIndex < k_BITS_PER_UINT64);

    // Since 'dstIndex > 0', destination bits always span two 'uint64_t' array
    // elements.

    const int dstLen = k_BITS_PER_UINT64 - dstIndex;
    OPER_DO_BITS(dstBitString,     dstIndex,           srcValue,   dstLen);
    OPER_DO_BITS(dstBitString + 1,        0, srcValue >> dstLen, dstIndex);
}

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t)>
void Mover<OPER_DO_BITS, OPER_DO_ALIGNED_WORD>::left(
                                                  uint64_t       *dstBitString,
                                                  size_t          dstIndex,
                                                  const uint64_t *srcBitString,
                                                  size_t          srcIndex,
                                                  size_t          numBits)
{
    // Preconditions can be checked with safe asserts since they are always
    // checked prior to this function being called.

    BSLS_ASSERT_SAFE(dstBitString);
    BSLS_ASSERT_SAFE(srcBitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    {
        const BitPtr     dst(dstBitString, dstIndex);
        const BitPtr     src(srcBitString, srcIndex);
        const BitPtrDiff diff(dst - src);

        BSLS_ASSERT_SAFE(diff <= 0 || diff >= numBits);
    }
#endif

    dstBitString    +=     dstIndex  / k_BITS_PER_UINT64;
    unsigned dstPos =  u32(dstIndex) % k_BITS_PER_UINT64;
    srcBitString    +=     srcIndex  / k_BITS_PER_UINT64;
    unsigned srcPos =  u32(srcIndex) % k_BITS_PER_UINT64;

    dstIndex = 0;
    srcIndex = 0;

    // Copy bits to align residual of source on a 'uint64_t' boundary.

    if (srcPos) {
        const unsigned srcLen = k_BITS_PER_UINT64 - srcPos;
        if (srcLen >= numBits) {
            doPartialWord(&dstBitString[dstIndex],
                          dstPos,
                          srcBitString[ srcIndex] >> srcPos,
                          u32(numBits));
            return;                                                   // RETURN
        }

        doPartialWord(&dstBitString[dstIndex],
                      dstPos,
                      srcBitString[ srcIndex] >> srcPos,
                      srcLen);
        dstPos += srcLen;
        if (dstPos >= k_BITS_PER_UINT64) {
            dstPos -= k_BITS_PER_UINT64;
            ++dstIndex;
        }
        numBits -= srcLen;

        srcPos = 0;     // srcPos = (srcPos + srcLen) % k_BITS_PER_UINT64;
                        // where 'srcLen == k_BITS_PER_UINT64 - srcPos'.
        ++srcIndex;
    }

    BSLS_ASSERT_SAFE(0 == srcPos);

    // Copy full source 'uint64_t' elements.

    if (dstPos) {
        // Normal case of the destination location being unaligned.

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            doFullNonAlignedWord(&dstBitString[dstIndex++],
                                 dstPos,
                                 srcBitString[ srcIndex++]);
        }
    }
    else {
        // The source and destination locations are both aligned.

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            OPER_DO_ALIGNED_WORD(&dstBitString[dstIndex++],
                                 srcBitString[ srcIndex++]);
        }
    }
    BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);

    // Move residual bits, if any.

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    doPartialWord(&dstBitString[dstIndex],
                  dstPos,
                  srcBitString[ srcIndex],
                  u32(numBits));
}

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t)>
void Mover<OPER_DO_BITS, OPER_DO_ALIGNED_WORD>::right(
                                                  uint64_t       *dstBitString,
                                                  size_t          dstIndex,
                                                  const uint64_t *srcBitString,
                                                  size_t          srcIndex,
                                                  size_t          numBits)
{
    // Preconditions can be checked with safe asserts since they are always
    // checked prior to this function being called.

    BSLS_ASSERT_SAFE(dstBitString);
    BSLS_ASSERT_SAFE(srcBitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    {
        const BitPtr     dst(dstBitString, dstIndex);
        const BitPtr     src(srcBitString, srcIndex);
        const BitPtrDiff diff(dst - src);

        // Either we're non-overlapping right, or overlapping such that a right
        // copy is necessary.  Note that this function is never called (and
        // this assert would fail) in the case of a non-overlapping left copy.

        BSLS_ASSERT_SAFE(diff > 0);
    }
#endif

    // Copy bits to align residual of source on a 'uint64_t' boundary.

    size_t   endDstBit =  dstIndex + numBits;
    dstBitString       +=     endDstBit  / k_BITS_PER_UINT64;
    unsigned dstPos    =  u32(endDstBit) % k_BITS_PER_UINT64;

    size_t   endSrcBit =  srcIndex + numBits;
    srcBitString       +=     endSrcBit  / k_BITS_PER_UINT64;
    unsigned srcPos    =  u32(endSrcBit) % k_BITS_PER_UINT64;

    dstIndex = 0;
    srcIndex = 0;

    // Note that in fact the function 'left' is always called instead of this
    // one whenever the source and destination ranges do not overlap, so this
    // function is only actually called in the case of overlapping right.

    if (srcPos) {
        if (srcPos >= numBits) {
            BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);
            const unsigned nb = u32(numBits);

            if (dstPos < nb) {
                dstPos += k_BITS_PER_UINT64;
                --dstIndex;
            }
            dstPos -= nb;
            srcPos -= nb;

            doPartialWord(&dstBitString[dstIndex],
                          dstPos,
                          srcBitString[        0] >> srcPos,
                          nb);
            return;                                                   // RETURN
        }

        if (dstPos < srcPos) {
            dstPos += k_BITS_PER_UINT64;
            --dstIndex;
        }
        dstPos -= srcPos;

        doPartialWord(&dstBitString[dstIndex],
                      dstPos,
                      srcBitString[        0],
                      srcPos);
        numBits -= srcPos;
        srcPos = 0;
    }

    // Source is now aligned.  Copy words at a time.

    BSLS_ASSERT_SAFE(0 == srcPos);

    if (dstPos) {
        // Normal case of the destination location being unaligned.

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            doFullNonAlignedWord(&dstBitString[--dstIndex],
                                 dstPos,
                                 srcBitString[ --srcIndex]);
        }
    }
    else {
        // The source and destination locations are both aligned.

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            OPER_DO_ALIGNED_WORD(&dstBitString[--dstIndex],
                                 srcBitString[ --srcIndex]);
        }
    }
    BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);
    const unsigned nb = u32(numBits);

    if (0 == nb) {
        return;                                                       // RETURN
    }

    // Move the residual high-order 'numBits' bits of
    // 'srcBitString[srcIndex - 1]'.

    if (dstPos < nb) {
        dstPos += k_BITS_PER_UINT64;
        --dstIndex;
    }
    dstPos -= nb;

    doPartialWord(&dstBitString[dstIndex],
                  dstPos,
                  srcBitString[srcIndex - 1] >> (k_BITS_PER_UINT64 - nb),
                  nb);
}

template <void OPER_DO_BITS(        uint64_t *, int, uint64_t, int),
          void OPER_DO_ALIGNED_WORD(uint64_t *,      uint64_t)>
inline
void Mover<OPER_DO_BITS, OPER_DO_ALIGNED_WORD>::move(
                                                  uint64_t       *dstBitString,
                                                  size_t          dstIndex,
                                                  const uint64_t *srcBitString,
                                                  size_t          srcIndex,
                                                  size_t          numBits)
{
    // Preconditions can be checked with safe asserts since they are always
    // checked prior to this function being called.

    BSLS_ASSERT_SAFE(dstBitString);
    BSLS_ASSERT_SAFE(srcBitString);

    const BitPtr     dst(dstBitString, dstIndex);
    const BitPtr     src(srcBitString, srcIndex);
    const BitPtrDiff diff(dst - src);

    if (diff > 0) {
        right(dstBitString,
              dstIndex,
              srcBitString,
              srcIndex,
              numBits);
    }
    else {
        left(dstBitString,
             dstIndex,
             srcBitString,
             srcIndex,
             numBits);
    }
}

}  // close unnamed namespace


                                // widely used

static inline
uint64_t lt64Raw(int numBits)
    // Return a 'uint64_t' value with the low-order specified 'numBits' set and
    // the rest cleared.  The behavior is undefined unless
    // '0 <= numBits < k_BITS_PER_UINT64'.  Note that this function performs
    // the same calculation as 'BitMaskUtil::lt64', except that it doesn't
    // waste time handling the case of 'k_BITS_PER_UINT64 == numBits'.
{
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(     numBits < k_BITS_PER_UINT64);

    return (1ULL << numBits) - 1;
}

static inline
uint64_t ge64Raw(int numBits)
    // Return a 'uint64_t' value with the low-order specified 'numBits' cleared
    // and the rest set.  The behavior is undefined unless
    // '0 <= numBits < k_BITS_PER_UINT64'.  Note that this function performs
    // the same calculation as 'BitMaskUtil::ge64', except that it doesn't
    // waste time handling the case of 'k_BITS_PER_UINT64 == numBits'.
{
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(     numBits < k_BITS_PER_UINT64);

    return ~0ULL << numBits;
}

template <class TYPE>
static inline
TYPE absRaw(TYPE x)
    // Return the absolute value of the specified 'x'.  The behavior is
    // undefined unless '-x != x' or '0 == x'.
{
    BSLS_ASSERT_SAFE(-x != x || 0 == x);

    return x < 0 ? -x : x;
}

                        // for 'areEqual'

static inline
bool bitsInWordsDiffer(uint64_t word1,
                       int      pos1,
                       uint64_t word2,
                       int      pos2,
                       int      numBits)
    // Return 'true' if the specified 'numBits' bits of the specified 'word1'
    // starting at the specified position 'pos1' differ from the 'numBits' bits
    // of the specified 'word2' starting at the specified position 'pos2', and
    // 'false' if the bit patterns are identical.  The behavior is undefined
    // unless '0 <= pos1', '0 <= pos2', '0 < numBits',
    // 'pos1 + numBits <= k_BITS_PER_UINT64', and
    // 'pos2 + numBits <= k_BITS_PER_UINT64'.  Note that this function does not
    // handle the case of '0 == numBits'.
{
    BSLS_ASSERT_SAFE(numBits + pos1 <= k_BITS_PER_UINT64);
    BSLS_ASSERT_SAFE(numBits + pos2 <= k_BITS_PER_UINT64);

    // If two down-shifted bits don't match, the XOR of them will be 1.  We
    // mask out the low-order 'numBits' (the bits we are interested in), and
    // if they're all 0, then the lower-order 'numBits' of the two down-shifted
    // words must have matched and we should return 'false'.

    return ((word1 >> pos1) ^ (word2 >> pos2)) & BitMaskUtil::lt64(numBits);
}

                        // for 'swapRaw'

static
void swapBitsInWords(uint64_t *word1,
                     int       index1,
                     uint64_t *word2,
                     int       index2,
                     int       numBits)
    // Swap the specified 'numBits' sequence of bits starting at the specified
    // 'index1' in the specified 'word1' with the 'numBits' starting at the
    // specified 'index2' in the specified 'word2'.  The behavior is undefined
    // unless '0 <= index1', '0 <= index2', '0 < numBits',
    // 'index1 + numBits <= k_BITS_PER_UINT64',
    // 'index2 + numBits <= k_BITS_PER_UINT64', and the two sets of bits
    // specified are not overlapping regions of the same word.  Note that the
    // non-overlapping condition is unchecked and is the responsibility of the
    // caller to ensure that it doesn't happen.  Also note that it is
    // permissible for 'word1' and 'word2' to refer to the same word, provided
    // the bits being swapped don't overlap.  Also note this function doesn't
    // handle the case of '0 == numBits'.
{
    BSLS_ASSERT_SAFE(word1);
    BSLS_ASSERT_SAFE(word2);
    BSLS_ASSERT_SAFE(0 < numBits);
    BSLS_ASSERT_SAFE(index1 + numBits <= k_BITS_PER_UINT64);
    BSLS_ASSERT_SAFE(index2 + numBits <= k_BITS_PER_UINT64);

    const uint64_t mask  = BitMaskUtil::lt64(numBits);
    const uint64_t bits1 = (*word1 >> index1) & mask;
    const uint64_t bits2 = (*word2 >> index2) & mask;

    // Zero out the footprint we will write to.

    *word1 &= ~(mask << index1);
    *word2 &= ~(mask << index2);

    // Now OR in the bits from the other word.

    *word1 |= bits2 << index1;
    *word2 |= bits1 << index2;
}

                        // for 'print'

static
void putSpaces(bsl::ostream& stream, int numSpaces)
    // Efficiently insert the specified 'numSpaces' spaces into the specified
    // 'stream'.  This function has no effect on 'stream' if 'numSpaces < 0'.
{
    // Algorithm: Write spaces in chunks.  The chunk size is large enough so
    // that most times only a single call to the 'write' method is needed.

    // Define the largest chunk of spaces:

    static const char spaces[] = "                                        ";

    enum { k_spaces_SIZE = sizeof(spaces) - 1 };

    while (numSpaces >= k_spaces_SIZE) {
        stream.write(spaces, k_spaces_SIZE);
        numSpaces -= k_spaces_SIZE;
    }

    if (0 < numSpaces) {
        stream.write(spaces, numSpaces);
    }
}

static
bsl::ostream& indent(bsl::ostream& stream,
                     int           level,
                     int           spacesPerLevel)
    // Output indentation to the specified 'stream' that is appropriate
    // according to BDE printing conventions for the specified 'level' and
    // the specified 'spacesPerLevel'.
{
    if (spacesPerLevel < 0) {
        spacesPerLevel = -spacesPerLevel;
    }

    putSpaces(stream, level * spacesPerLevel);
    return stream;
}

static
bsl::ostream& newlineAndIndent(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel)
    // Output a newline and indentation to the specified 'stream' appropriate
    // for the specified 'level' and the specified 'spacesPerLevel'.
{
    if (spacesPerLevel < 0) {
        return stream << ' ';                                         // RETURN
    }

    if (level < 0) {
        level = -level;
    }

    stream << '\n';
    putSpaces(stream, level * spacesPerLevel);
    return stream;
}

namespace BloombergLP {
namespace bdlb {

                        // ====================
                        // struct BitStringUtil
                        // ====================

// CLASS METHODS

                            // Manipulators

                            // Assign

void BitStringUtil::assign(uint64_t *bitString,
                           size_t    index,
                           bool      value,
                           size_t    numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    size_t idx       =     index  / k_BITS_PER_UINT64;
    int    pos       = u32(index) % k_BITS_PER_UINT64;
    int    numOfBits = static_cast<int>(bsl::min<size_t>(
                                            k_BITS_PER_UINT64 - pos, numBits));

    // Set the partial leading bits.

    if (value) {
        bitString[idx] |= BitMaskUtil::one64( pos, numOfBits);
    }
    else {
        bitString[idx] &= BitMaskUtil::zero64(pos, numOfBits);
    }

    numBits -= numOfBits;

    // Note that 'pos' is not updated here as it is no longer used.  Either we
    // are finished ('0 == numBits') or we are now word-aligned.

    // Set the 'uint64_t' elements with whole words.

    const uint64_t valueWord = value ? ~0ULL : 0ULL;
    for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
        bitString[++idx] = valueWord;
    }

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    // Set the partial trailing bits.

    if (value) {
        bitString[++idx] |= lt64Raw(u32(numBits));
    }
    else {
        bitString[++idx] &= ge64Raw(u32(numBits));
    }
}

void BitStringUtil::assign0(uint64_t  *bitString,
                            size_t     index,
                            size_t     numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    size_t idx       =     index  / k_BITS_PER_UINT64;
    int    pos       = u32(index) % k_BITS_PER_UINT64;
    int    numOfBits = static_cast<int>(bsl::min<size_t>(
                                            k_BITS_PER_UINT64 - pos, numBits));

    // Set the partial leading bits.

    bitString[idx] &= BitMaskUtil::zero64(pos, numOfBits);

    numBits -= numOfBits;

    // Note that 'pos' is not updated here as it is no longer used.  Either we
    // are finished ('0 == numBits') or we are now word-aligned.

    // Set the 'uint64_t's with whole words.

    for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
        bitString[++idx] = 0ULL;
    }

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    // Set the partial trailing bits.

    bitString[++idx] &= ge64Raw(u32(numBits));
}

void BitStringUtil::assign1(uint64_t  *bitString,
                            size_t     index,
                            size_t     numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    size_t idx       =     index  / k_BITS_PER_UINT64;
    int    pos       = u32(index) % k_BITS_PER_UINT64;
    int    numOfBits = static_cast<int>(bsl::min<size_t>(
                                            k_BITS_PER_UINT64 - pos, numBits));

    // Set the partial leading bits.

    bitString[idx] |= BitMaskUtil::one64(pos, numOfBits);

    numBits -= numOfBits;

    // Note that 'pos' is not updated here as it is no longer used.  Either we
    // are finished ('0 == numBits') or we are now word-aligned.

    // Set the 'uint64_t' elements with whole words.

    const uint64_t valueWord = ~0LL;
    for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
        bitString[++idx] = valueWord;
    }

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    // Set the partial trailing bits.

    bitString[++idx] |= lt64Raw(u32(numBits));
}

void BitStringUtil::assignBits(uint64_t *bitString,
                               size_t    index,
                               uint64_t  srcValue,
                               size_t    numBits)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(numBits <= k_BITS_PER_UINT64);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    bitString +=     index  / k_BITS_PER_UINT64;
    int pos    = u32(index) % k_BITS_PER_UINT64;
    int dstLen = k_BITS_PER_UINT64 - pos;

    if (dstLen >= static_cast<int>(numBits)) {
        const uint64_t mask = BitMaskUtil::lt64(u32(numBits));
        srcValue &= mask;
        *bitString &= ~(mask   << pos);
        *bitString |= srcValue << pos;
    }
    else {
        BSLS_ASSERT_SAFE(0 < pos);

        uint64_t mask = lt64Raw(dstLen);
        uint64_t bits = srcValue & mask;
        *bitString &= ~(mask << pos);
        *bitString |=   bits << pos;

        ++bitString;

        mask = lt64Raw(u32(numBits) - dstLen);
        bits = (srcValue >> dstLen) & mask;
        *bitString &= ~mask;
        *bitString |=  bits;
    }
}

                             // Bitwise-Logical

void BitStringUtil::andEqual(uint64_t       *dstBitString,
                             size_t          dstIndex,
                             const uint64_t *srcBitString,
                             size_t          srcIndex,
                             size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

    Mover<Imp::andEqBits, Imp::andEqWord>::move(dstBitString,
                                                dstIndex,
                                                srcBitString,
                                                srcIndex,
                                                numBits);
}

void BitStringUtil::minusEqual(uint64_t       *dstBitString,
                               size_t          dstIndex,
                               const uint64_t *srcBitString,
                               size_t          srcIndex,
                               size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

    Mover<Imp::minusEqBits, Imp::minusEqWord>::move(dstBitString,
                                                    dstIndex,
                                                    srcBitString,
                                                    srcIndex,
                                                    numBits);
}

void BitStringUtil::orEqual(uint64_t       *dstBitString,
                            size_t          dstIndex,
                            const uint64_t *srcBitString,
                            size_t          srcIndex,
                            size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

    Mover<Imp::orEqBits, Imp::orEqWord>::move(dstBitString,
                                              dstIndex,
                                              srcBitString,
                                              srcIndex,
                                              numBits);
}

void BitStringUtil::xorEqual(uint64_t       *dstBitString,
                             size_t          dstIndex,
                             const uint64_t *srcBitString,
                             size_t          srcIndex,
                             size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

    Mover<Imp::xorEqBits, Imp::xorEqWord>::move(dstBitString,
                                                dstIndex,
                                                srcBitString,
                                                srcIndex,
                                                numBits);
}

                            // Copy

void BitStringUtil::copy(uint64_t       *dstBitString,
                         size_t          dstIndex,
                         const uint64_t *srcBitString,
                         size_t          srcIndex,
                         size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

    Mover<Imp::setEqBits, Imp::setEqWord>::move(dstBitString,
                                                dstIndex,
                                                srcBitString,
                                                srcIndex,
                                                numBits);
}

void BitStringUtil::copyRaw(uint64_t       *dstBitString,
                            size_t          dstIndex,
                            const uint64_t *srcBitString,
                            size_t          srcIndex,
                            size_t          numBits)
{
    BSLS_ASSERT(dstBitString);
    BSLS_ASSERT(srcBitString);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    {
        const BitPtr     dst(dstBitString, dstIndex);
        const BitPtr     src(srcBitString, srcIndex);
        const BitPtrDiff diff(dst - src);

        BSLS_ASSERT_SAFE(diff <= 0 || diff >= numBits);
    }
#endif

    Mover<Imp::setEqBits, Imp::setEqWord>::left(dstBitString,
                                                dstIndex,
                                                srcBitString,
                                                srcIndex,
                                                numBits);
}

                            // Insert / Remove

void BitStringUtil::insertRaw(uint64_t *bitString,
                              size_t    initialLength,
                              size_t    dstIndex,
                              size_t    numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    if (dstIndex >= initialLength) {
        BSLS_ASSERT(dstIndex == initialLength);

        return;                                                       // RETURN
    }

    Mover<Imp::setEqBits, Imp::setEqWord>::right(bitString,
                                                 dstIndex + numBits,
                                                 bitString,
                                                 dstIndex,
                                                 initialLength - dstIndex);
}

void BitStringUtil::remove(uint64_t *bitString,
                           size_t    length,
                           size_t    index,
                           size_t    numBits)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(index + numBits <= length);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    const size_t remBits = length - (index + numBits);
    if (0 == remBits) {
        return;                                                       // RETURN
    }

    // Copy 'numBits' starting at 'index + numBits' to 'index'.

    Mover<Imp::setEqBits, Imp::setEqWord>::left(bitString,
                                                index,
                                                bitString,
                                                index + numBits,
                                                remBits);
}

                            // Other Manipulators

void BitStringUtil::swapRaw(uint64_t *bitString1,
                            size_t    index1,
                            uint64_t *bitString2,
                            size_t    index2,
                            size_t    numBits)
{
    BSLS_ASSERT(bitString1);
    BSLS_ASSERT(bitString2);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    // Normalize pointers and indices.

    bitString1 +=     index1  / k_BITS_PER_UINT64;
    int pos1   =  u32(index1) % k_BITS_PER_UINT64;
    int rem1   =  k_BITS_PER_UINT64 - pos1;        // bits from 'pos1' to end
                                                   // of word

    bitString2 +=     index2  / k_BITS_PER_UINT64;
    int pos2   =  u32(index2) % k_BITS_PER_UINT64;
    int rem2   =  k_BITS_PER_UINT64 - pos2;        // bits from 'pos2' to end
                                                   // of word

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    {
        // No overlap is allowed.

        const BitPtr     dst(bitString1, pos1);
        const BitPtr     src(bitString2, pos2);
        const BitPtrDiff diff(dst - src);
        const BitPtrDiff nb(numBits);      // Note that 'numBits' is unsigned
                                           // but we'll want its negated value.
                                           // Once we convert it to 'nb', a
                                           // 'BitPtrDiff', it's in a signed
                                           // type and we can negate it.

        BSLS_ASSERT_SAFE(diff <= -nb || diff >= nb);
    }
#endif

    if (pos1 == pos2) {
        int nb = static_cast<int>(bsl::min<size_t>(rem1, numBits));
        BSLS_ASSERT_SAFE(nb > 0);

        swapBitsInWords(bitString1, pos1, bitString2, pos2, nb);

        numBits -= nb;
        if (0 == numBits) {
            return;                                                   // RETURN
        }

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            using bsl::swap;

            swap(*++bitString1, *++bitString2);
        }

        if (0 == numBits) {
            return;                                                   // RETURN
        }

        swapBitsInWords(++bitString1, 0, ++bitString2, 0, u32(numBits));
    }
    else {
        if (rem1 > rem2) {
            using bsl::swap;

            swap(bitString1, bitString2);
            swap(pos1, pos2);
            swap(rem1, rem2);
        }

        while (true) {
            {
                const int numBitsA = static_cast<int>(bsl::min<size_t>(
                                                               rem1, numBits));
                BSLS_ASSERT_SAFE(numBitsA > 0);

                swapBitsInWords(bitString1, pos1, bitString2, pos2, numBitsA);

                numBits -= numBitsA;
                if (0 == numBits) {
                    return;                                           // RETURN
                }

                // Advance 'pos1' and 'pos2' by 'numBitsA == rem1'.  Bear in
                // mind that 'numBitsA == k_BITS_PER_UINT64 - pos1'.  When
                // 'pos1' overflows we advance 'bitString1'.  We know that
                // 'rem2 > rem1', so advancing 'pos2' won't overflow and there
                // is no need to advance 'bitString2'.

                pos1 = 0;
                ++bitString1;

                pos2 += numBitsA;
                rem2 -= numBitsA;
            }

            {
                const int numBitsB = static_cast<int>(bsl::min<size_t>(
                                                               rem2, numBits));
                BSLS_ASSERT_SAFE(numBitsB > 0);

                swapBitsInWords(bitString1, pos1, bitString2, pos2, numBitsB);

                numBits -= numBitsB;
                if (0 == numBits) {
                    return;                                           // RETURN
                }

                // Advance 'pos1' and 'pos2' by 'numBitsB == rem2'.  Note that
                // '0 == pos1' and 'pos2 == k_BITS_PER_UINT64 - rem2'.  When
                // 'pos2' overflows, we must advance 'bitString2'.
                // Re-initialize 'rem1' and 'rem2' to the remainders of 'pos1'
                // and 'pos2'.

                pos1 = numBitsB;
                rem1 = k_BITS_PER_UINT64 - pos1;

                pos2 = 0;
                ++bitString2;
                rem2 = k_BITS_PER_UINT64;
            }
        }
    }
}

void BitStringUtil::toggle(uint64_t *bitString, size_t index, size_t numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    size_t idx =     index  / k_BITS_PER_UINT64;
    int    pos = u32(index) % k_BITS_PER_UINT64;

    // Toggle unaligned initial bits.

    if (pos) {
        uint64_t mask = ge64Raw(pos);

        const unsigned dstLen = k_BITS_PER_UINT64 - pos;
        if (dstLen > numBits) {
            mask &= lt64Raw(u32(pos + numBits));
            bitString[idx] ^= mask;
            return;                                                   // RETURN
        }

        bitString[idx++]   ^= mask;
        numBits -= dstLen;
    }

    // Toggle full words.

    const uint64_t minusOne = ~0ULL;
    for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
        bitString[idx++] ^= minusOne;
    }
    BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);

    // Toggle trailing bits.

    if (0 == numBits) {
        return;                                                       // RETURN
    }

    bitString[idx] ^= lt64Raw(u32(numBits));
}

                            // Accessors

bool BitStringUtil::areEqual(const uint64_t *bitString1,
                             const uint64_t *bitString2,
                             size_t          numBits)
{
    BSLS_ASSERT(bitString1);
    BSLS_ASSERT(bitString2);

    if (0 == numBits) {
        return true;                                                  // RETURN
    }

    const size_t lastWord = (numBits - 1) / k_BITS_PER_UINT64;

    for (size_t ii = 0; ii < lastWord; ++ii) {
        if (bitString1[ii] != bitString2[ii]) {
            return false;                                             // RETURN
        }
    }

    const int endPos = u32(numBits - 1) % k_BITS_PER_UINT64 + 1;

    return 0 == ((bitString1[lastWord] ^ bitString2[lastWord]) &
                                                    BitMaskUtil::lt64(endPos));
}

bool BitStringUtil::areEqual(const uint64_t *bitString1,
                             size_t          index1,
                             const uint64_t *bitString2,
                             size_t          index2,
                             size_t          numBits)
{
    BSLS_ASSERT(bitString1);
    BSLS_ASSERT(bitString2);

    if (0 == numBits) {
        return true;                                                  // RETURN
    }

    bitString1 +=     index1  / k_BITS_PER_UINT64;
    int pos1   =  u32(index1) % k_BITS_PER_UINT64;
    int rem1   =  k_BITS_PER_UINT64 - pos1;

    bitString2 +=     index2  / k_BITS_PER_UINT64;
    int pos2   =  u32(index2) % k_BITS_PER_UINT64;
    int rem2   =  k_BITS_PER_UINT64 - pos2;

    BSLS_ASSERT_SAFE(rem1 > 0 && rem2 > 0);

    if (pos1 == pos2) {
        int nb = static_cast<int>(bsl::min<size_t>(rem1, numBits));

        if (bitsInWordsDiffer(*bitString1, pos1, *bitString2, pos2, nb)) {
            return false;                                             // RETURN
        }

        numBits -= nb;
        if (0 == numBits) {
            return true;                                              // RETURN
        }

        // We are currently aligned to word boundaries in both 'bitString1' and
        // 'bitString2'.  'pos1', 'pos2', 'rem1', 'rem2', and 'nb' are no
        // longer updated or relevant.

        for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
            if (*++bitString1 != *++bitString2) {
                return false;                                         // RETURN
            }
        }

        if (0 == numBits) {
            return true;                                              // RETURN
        }

        nb = u32(numBits);
        return !bitsInWordsDiffer(*++bitString1, 0, *++bitString2, 0, nb);
                                                                      // RETURN
    }

    if (rem1 > rem2) {
        using bsl::swap;

        swap(bitString1, bitString2);
        swap(pos1, pos2);
        swap(rem1, rem2);
    }

    while (true) {
        // One iteration through the loop will either get us to the end, or
        // advance us 'rem2' bits, which will get 'pos2' word-aligned for the
        // next iteration, so each iteration after the first and before the
        // last will cover a full word.

        // The first part of the loop will compare 'numBitsA' bits, which will
        // either get 'pos1' to the end of the data, or to a word boundary of
        // 'bitString1'.

        {
            BSLS_ASSERT_SAFE(rem1 < rem2);
            BSLS_ASSERT_SAFE(rem1 > 0 && rem2 > 0);
            BSLS_ASSERT_SAFE(rem1 <  k_BITS_PER_UINT64);
            BSLS_ASSERT_SAFE(rem2 <= k_BITS_PER_UINT64);

            const int numBitsA = static_cast<int>(
                                              bsl::min<size_t>(rem1, numBits));
            BSLS_ASSERT_SAFE(numBitsA > 0);

            if (bitsInWordsDiffer(*bitString1,
                                  pos1,
                                  *bitString2,
                                  pos2,
                                  numBitsA)) {
                return false;                                         // RETURN
            }

            numBits -= numBitsA;
            if (0 == numBits) {
                return true;                                          // RETURN
            }

            BSLS_ASSERT_SAFE(numBitsA == rem1);

            // Advance 'pos1' and 'pos2' by 'numBitsA == rem1'.  Bear in mind
            // that 'numBitsA == k_BITS_PER_UINT64 - pos1'.  When 'pos1'
            // overflows we advance 'bitString1'.  We know that 'rem2 > rem1',
            // so advancing 'pos2' won't overflow and there is no need to
            // advance 'bitString2'.

            ++bitString1;
            pos1 = 0;

            pos2 += numBitsA;
            BSLS_ASSERT_SAFE(pos2 < k_BITS_PER_UINT64);
            rem2 -= numBitsA;
        }

        // The second part of the loop we will compare 'numBitsB' bits, which
        // when we advance will get 'pos2' either to the end of the data, or to
        // a word boundary of 'bitString2'.

        {
            const int numBitsB = static_cast<int>(
                                              bsl::min<size_t>(rem2, numBits));
            BSLS_ASSERT_SAFE(numBitsB > 0);

            if (bitsInWordsDiffer(*bitString1,
                                  pos1,
                                  *bitString2,
                                  pos2,
                                  numBitsB)) {
                return false;                                         // RETURN
            }

            numBits -= numBitsB;
            if (0 == numBits) {
                return true;                                          // RETURN
            }

            // Advance 'pos1' and 'pos2' by 'numBitsB == rem2'.  Note that
            // '0 == pos1', and 'pos2 == k_BITS_PER_UINT64 - rem2'.  When
            // 'pos2' overflows, we must advance 'bitString2'.  Re-initialize
            // 'rem1' and 'rem2' to the remainders of 'pos1' and 'pos2'.

            ++bitString2;
            pos2 = 0;
            rem2 = k_BITS_PER_UINT64;

            pos1 = numBitsB;
            rem1 = k_BITS_PER_UINT64 - pos1;
        }
    }

    return true;
}

uint64_t BitStringUtil::bits(const uint64_t *bitString,
                             size_t          index,
                             size_t          numBits)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(numBits <= k_BITS_PER_UINT64);

    if (0 == numBits) {
        return 0;                                                     // RETURN
    }

    size_t   idx    =     index  / k_BITS_PER_UINT64;
    int      pos    = u32(index) % k_BITS_PER_UINT64;

    int      remLen = k_BITS_PER_UINT64 - pos;
    int      nb     = static_cast<int>(bsl::min<size_t>(numBits, remLen));
    uint64_t ret    = (bitString[idx] >> pos) & BitMaskUtil::lt64(nb);

    numBits -= nb;
    if (0 == numBits) {
        return ret;                                                   // RETURN
    }

    ret |= (bitString[idx + 1] & lt64Raw(u32(numBits))) << remLen;

    return ret;
}

// Implementation note for all the 'find[01]At{Max,Min}Index' functions:
// 'Imp::find1At{Max,Min}IndexRaw' can be very fast on some platforms, but on
// others it is a quite expensive operation.  Hence we avoid calling them until
// we are certain there is a set bit in the word it is searching.  Note that
// the behavior of those 2 functions is undefined unless '0 != value'.

size_t BitStringUtil::find0AtMaxIndex(const uint64_t *bitString, size_t length)
{
    BSLS_ASSERT(bitString);

    if (0 == length) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t lastWord =    (length - 1) / k_BITS_PER_UINT64;
    const int    endPos   = u32(length - 1) % k_BITS_PER_UINT64 + 1;

    uint64_t     value    = ~bitString[lastWord] & BitMaskUtil::lt64(endPos);

    for (size_t ii = lastWord; true; value = ~bitString[--ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value);
                                                                      // RETURN
        }

        if (0 == ii) {
            return k_INVALID_INDEX;                                   // RETURN
        }
    }
}

size_t BitStringUtil::find0AtMaxIndex(const uint64_t *bitString,
                                      size_t          begin,
                                      size_t          end)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t beginWord =        begin / k_BITS_PER_UINT64;
    const size_t lastWord  =    (end - 1) / k_BITS_PER_UINT64;
    const int    endPos    = u32(end - 1) % k_BITS_PER_UINT64 + 1;

    uint64_t     value     = ~bitString[lastWord] & BitMaskUtil::lt64(endPos);

    for (size_t ii = lastWord; ii > beginWord; value = ~bitString[--ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value);
                                                                      // RETURN
        }
    }

    const int beginIdx = u32(begin) % k_BITS_PER_UINT64;

    value &= ge64Raw(beginIdx);
    return value
           ? beginWord * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value)
           : k_INVALID_INDEX;
}

size_t BitStringUtil::find0AtMinIndex(const uint64_t *bitString, size_t length)
{
    BSLS_ASSERT(bitString);

    if (0 == length) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t lastWord = (length - 1) / k_BITS_PER_UINT64;
    uint64_t     value;

    for (size_t ii = 0; ii < lastWord; ++ii) {
        value = ~bitString[ii];
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value);
                                                                      // RETURN
        }
    }

    const int endPos = u32(length - 1) % k_BITS_PER_UINT64 + 1;

    value = ~bitString[lastWord] & BitMaskUtil::lt64(endPos);
    return value
           ? lastWord * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value)
           : k_INVALID_INDEX;
}

size_t BitStringUtil::find0AtMinIndex(const uint64_t *bitString,
                                      size_t          begin,
                                      size_t          end)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t beginWord =       begin  / k_BITS_PER_UINT64;
    const int    beginIdx  =   u32(begin) % k_BITS_PER_UINT64;
    const size_t lastWord  =    (end - 1) / k_BITS_PER_UINT64;
    const int    endPos    = u32(end - 1) % k_BITS_PER_UINT64 + 1;

    uint64_t     value     = ~bitString[beginWord] & ge64Raw(beginIdx);

    for (size_t ii = beginWord; ii < lastWord; value = ~bitString[++ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value);
                                                                      // RETURN
        }
    }

    value &= BitMaskUtil::lt64(endPos);
    return value
           ? lastWord * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value)
           : k_INVALID_INDEX;
}

size_t BitStringUtil::find1AtMaxIndex(const uint64_t *bitString, size_t length)
{
    BSLS_ASSERT(bitString);

    if (0 == length) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t lastWord =    (length - 1) / k_BITS_PER_UINT64;
    const int    endPos   = u32(length - 1) % k_BITS_PER_UINT64 + 1;

    uint64_t     value    = bitString[lastWord] & BitMaskUtil::lt64(endPos);

    for (size_t ii = lastWord; true; value = bitString[--ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value);
                                                                      // RETURN
        }

        if (0 == ii) {
            return k_INVALID_INDEX;                                   // RETURN
        }
    }
}

size_t BitStringUtil::find1AtMaxIndex(const uint64_t *bitString,
                                      size_t          begin,
                                      size_t          end)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t lastWord  =    (end - 1) / k_BITS_PER_UINT64;
    const int    endPos    = u32(end - 1) % k_BITS_PER_UINT64 + 1;
    const size_t beginWord =       begin  / k_BITS_PER_UINT64;
    const int    beginIdx  =   u32(begin) % k_BITS_PER_UINT64;

    uint64_t  value     = bitString[lastWord] & BitMaskUtil::lt64(endPos);

    for (size_t ii = lastWord; ii > beginWord; value = bitString[--ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value);
                                                                      // RETURN
        }
    }

    value &= ge64Raw(beginIdx);
    return value
           ? beginWord * k_BITS_PER_UINT64 + Imp::find1AtMaxIndexRaw(value)
           : k_INVALID_INDEX;
}

size_t BitStringUtil::find1AtMinIndex(const uint64_t *bitString, size_t length)
{
    BSLS_ASSERT(bitString);

    if (0 == length) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t lastWord = (length - 1) / k_BITS_PER_UINT64;
    uint64_t     value;

    for (size_t ii = 0; ii < lastWord; ++ii) {
        value = bitString[ii];
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value);
                                                                      // RETURN
        }
    }

    const int endPos = u32(length - 1) % k_BITS_PER_UINT64 + 1;

    value = bitString[lastWord] & BitMaskUtil::lt64(endPos);
    return value
           ? lastWord * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value)
           : k_INVALID_INDEX;
}

size_t BitStringUtil::find1AtMinIndex(const uint64_t *bitString,
                                      size_t          begin,
                                      size_t          end)
{
    BSLS_ASSERT(bitString);
    BSLS_ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    const size_t beginWord =       begin  / k_BITS_PER_UINT64;
    const int    beginIdx  =   u32(begin) % k_BITS_PER_UINT64;
    const size_t lastWord  =    (end - 1) / k_BITS_PER_UINT64;
    const int    endPos    = u32(end - 1) % k_BITS_PER_UINT64 + 1;

    uint64_t     value     = bitString[beginWord] & ge64Raw(beginIdx);

    for (size_t ii = beginWord; ii < lastWord; value = bitString[++ii]) {
        if (value) {
            return ii * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value);
                                                                      // RETURN
        }
    }

    value &= BitMaskUtil::lt64(endPos);
    return value
           ? lastWord * k_BITS_PER_UINT64 + Imp::find1AtMinIndexRaw(value)
           : k_INVALID_INDEX;
}

bool BitStringUtil::isAny0(const uint64_t *bitString,
                           size_t          index,
                           size_t          numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    size_t idx       =     index  / k_BITS_PER_UINT64;
    int    pos       = u32(index) % k_BITS_PER_UINT64;
    int    numOfBits = static_cast<int>(bsl::min<size_t>(
                                            k_BITS_PER_UINT64 - pos, numBits));

    if (~bitString[idx] & BitMaskUtil::one64(pos, numOfBits)) {
        return true;                                                  // RETURN
    }
    numBits -= numOfBits;

    for (; numBits >= k_BITS_PER_UINT64; numBits -= k_BITS_PER_UINT64) {
        if (~bitString[++idx]) {
            return true;                                              // RETURN
        }
    }
    BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    return ~bitString[++idx] & lt64Raw(u32(numBits));
}

bool BitStringUtil::isAny1(const uint64_t *bitString,
                           size_t          index,
                           size_t          numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    size_t idx       =     index  / k_BITS_PER_UINT64;
    int    pos       = u32(index) % k_BITS_PER_UINT64;
    int    numOfBits = static_cast<int>(
                           bsl::min<size_t>(k_BITS_PER_UINT64 - pos, numBits));

    if (bitString[idx] & BitMaskUtil::one64(pos, numOfBits)) {
        return true;                                                  // RETURN
    }
    numBits -= numOfBits;

    while (numBits >= k_BITS_PER_UINT64) {
        if (bitString[++idx]) {
            return true;                                              // RETURN
        }
        numBits -= k_BITS_PER_UINT64;
    }
    BSLS_ASSERT_SAFE(numBits < k_BITS_PER_UINT64);

    if (0 == numBits) {
        return false;                                                 // RETURN
    }

    return bitString[++idx] & lt64Raw(u32(numBits));
}

size_t BitStringUtil::num1(const uint64_t *bitString,
                           size_t          index,
                           size_t          numBits)
{
    BSLS_ASSERT(bitString);

    if (0 == numBits) {
        return 0;                                                     // RETURN
    }

    size_t    beginIdx  =     index  / k_BITS_PER_UINT64;
    const int beginPos  = u32(index) % k_BITS_PER_UINT64;

    bitString += beginIdx;
    beginIdx  =  0;            // note 'beginPos' is unchanged and still valid

    const size_t lastWord = (beginPos + numBits - 1) / k_BITS_PER_UINT64;

    if (0 == lastWord) {
        // All the bits we are interested in live within a single word.

        return BitUtil::numBitsSet(bitString[0] &
                       BitMaskUtil::one64(beginPos, u32(numBits)));   // RETURN
    }

    // We have multiple words to traverse.  The first and last might be partial
    // words, so we have to mask them.  The words in between will all be full
    // words.  Note that we are traversing from high-order to low-order words.

    const int endPos = u32(beginPos + numBits - 1) % k_BITS_PER_UINT64 + 1;

    size_t ret = BitUtil::numBitsSet(bitString[lastWord] &
                                                    BitMaskUtil::lt64(endPos));

    // We now want to visit all the words not including the highest-order and
    // lowest-order words in 'bitString'.  We set 'array' such that
    // '&array[0] == &bitString[1]'.  The first word we will visit in this
    // array (as we predecrement 'ii') will be
    // 'array[lastWord - 2] == bitString[lastWord - 1]'.  The last element we
    // will visit will be 'array[0] == bitString[1]'.  The initial value of
    // 'ii' below will be the number of words between the first and last words
    // that must be traversed, not including the first word or the last word.

    BSLS_ASSERT_SAFE(lastWord >= 1);    // therefore 'ii' below is >= 0 and can
                                        // be represented as a 'size_t'

    const uint64_t *array = bitString + 1;
    size_t          ii    = lastWord
                                     - 1  // word before last word
                                     - 1  // adjust from 'bitString' to 'array'
                                     + 1; // preparation for pre-decrement

    while (ii >= 8) {
        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);

        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);
        ret +=       BitUtil::numBitsSet(array[--ii]);
    }

    BSLS_ASSERT_SAFE(ii < 8);

    switch (ii) {
      case 7: ret += BitUtil::numBitsSet(array[--ii]);
      case 6: ret += BitUtil::numBitsSet(array[--ii]);
      case 5: ret += BitUtil::numBitsSet(array[--ii]);
      case 4: ret += BitUtil::numBitsSet(array[--ii]);
      case 3: ret += BitUtil::numBitsSet(array[--ii]);
      case 2: ret += BitUtil::numBitsSet(array[--ii]);
      case 1: ret += BitUtil::numBitsSet(array[--ii]);
      default: ;
    }

    BSLS_ASSERT_SAFE(0 == ii);

    // And we are now ready to look at the lowest-order word.

    return ret + BitUtil::numBitsSet(bitString[0] & ge64Raw(beginPos));
}

bsl::ostream& BitStringUtil::print(bsl::ostream&   stream,
                                   const uint64_t *bitString,
                                   size_t          numBits,
                                   int             level,
                                   int             spacesPerLevel)
{
    BSLS_ASSERT(bitString);

    if (!stream) {
        return stream;                                                // RETURN
    }

    indent(stream, level, spacesPerLevel);
    stream << '[';
    if (0 == numBits) {
        newlineAndIndent(stream, level, spacesPerLevel);
        stream << ']';
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
        return stream;                                                // RETURN
    }

    bsl::ios_base::fmtflags oldOptions(stream.flags());
    stream << bsl::hex;

    if (level < 0) {
        level = -level;
    }
    int levelPlus1  = level + 1;

    if (numBits > 0) {
        size_t endPos          =    (numBits - 1) / k_BITS_PER_UINT64 + 1;
        int    lastWordBits    = u32(numBits - 1) % k_BITS_PER_UINT64 + 1;
        int    lastWordNibbles = (lastWordBits - 1) / 4               + 1;

        if (spacesPerLevel >= 0 && endPos > 4) {
            const size_t startIdx = ((endPos - 1) / 4 + 1) * 4 - 1;
            BSLS_ASSERT_SAFE(startIdx >= endPos - 1);
            BSLS_ASSERT_SAFE(startIdx % 4 == 3);

            for (size_t idx = startIdx; true; --idx) {
                if (3 == idx % 4) {
                    newlineAndIndent(stream, levelPlus1, spacesPerLevel);
                }
                else {
                    stream << ' ';
                }

                if (endPos <= idx) {
                    stream << "                ";
                }
                else if (endPos - 1 == idx) {
                    putSpaces(stream, 16 - lastWordNibbles);
                    stream << bsl::setfill('0') << bsl::setw(lastWordNibbles)<<
                            (bitString[idx] & BitMaskUtil::lt64(lastWordBits));
                }
                else {
                    stream << bsl::setfill('0') << bsl::setw(16) <<
                                                                bitString[idx];
                }

                if (0 == idx) {
                    break;
                }
            }
        }
        else {
            newlineAndIndent(stream, levelPlus1, spacesPerLevel);

            size_t idx = endPos - 1;
            stream << bsl::setfill('0') << bsl::setw(lastWordNibbles) <<
                            (bitString[idx] & BitMaskUtil::lt64(lastWordBits));

            while (idx > 0) {
                stream << ' ' <<
                        bsl::setfill('0') << bsl::setw(16) << bitString[--idx];
            }
        }
    }

    stream.flags(oldOptions);

    if (spacesPerLevel >= 0) {
        newlineAndIndent(stream, level, spacesPerLevel);
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

}  // close package namespace
}  // close enterprise namespace

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
