// bdlb_bitstringutil.t.cpp                                           -*-C++-*-

#include <bdlb_bitstringutil.h>
#include <bdlb_bitmaskutil.h>

#include <bslim_testutil.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstddef.h>     // 'bsl::size_t'
#include <bsl_cstdlib.h>     // 'bsl::rand'
#include <bsl_cstring.h>
#include <bsl_cctype.h>

#include <bsl_c_limits.h>    // 'CHAR_BIT', 'INT_MAX'

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test provides static methods that perform various bit
// string related computations.  The goal of this 'bdlb::BitStringUtil' test
// suite is to verify that the methods have the desired effect on bit strings
// and/or return the expected values.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 3] void assign(uint64_t *bitString, St index, bool value);
// [ 4] void assign(U64 *bitString, St index, bool value, St numBits);
// [ 3] void assign0(uint64_t *bitString, St index);
// [ 4] void assign0(uint64_t *bitString, St index, St numBits);
// [ 3] void assign1(uint64_t *bitString, St index);
// [ 4] void assign1(uint64_t *bitString, St index, St numBits);
// [ 5] void assignBits(U64 *bitString, St index, U64 srcBits, St nb);
// [15] void andEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
// [16] void minusEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
// [17] void orEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
// [18] void xorEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
// [ 8] void copyRaw(U64 *dstBS, St dIdx, U64 *srcBS St sIdx, St nb);
// [ 8] void copy(U64 *dstBS, St dIdx, U64 *srcBS St sIdx, St nb);
// [ 7] void copyRaw(U64 *dstBS, St dIdx, U64 *srcBS, St sIdx, St nb);
// [ 7] void copy(U64 *dstBS, St dIdx, U64 *srcBS, St sIdx, St nb);
// [ 9] void insert(U64 *bitString, St len, St idx, bool val, St nb);
// [ 9] void insert0(uint64_t *bitString, St length, St idx, St numBits);
// [ 9] void insert1(uint64_t *bitString, St length, St idx, St numBits);
// [ 9] void insertRaw(uint64_t *bitString, St len, St idx, St numBits);
// [10] void remove(uint64_t *bitString, St len, St idx, St numBits);
// [10] void removeAndFill0(U64 *bitString, St len, St idx, St nb);
// [10] void removeAndFill1(U64 *bitString, St len, St idx, St nb);
// [11] void swapRaw(U64 *lhsBS, St lIdx, U64 *rhsBS, St rIdx, St nb);
// [14] void toggle(uint64_t *bitString, St index, St numBits);
// [ 2] bool areEqual(U64 *lhsBitString, U64 *rhsBitString, St numBits);
// [ 2] bool areEqual(U64 *lBS, St lIdx, U64 *rBS, St rIdx, St nb);
// [ 3] bool bit(const uint64_t *bitString, St index);
// [ 4] uint64_t bits(const uint64_t *bitString, St index, St numBits);
// [19] St find0AtMaxIndex(const uint64_t *bitString, St length);
// [19] St find0AtMaxIndex(U64 *bitString, St begin, St end);
// [21] St find0AtMinIndex(const uint64_t *bitString, St length);
// [21] St find0AtMinIndex(U64 *bitString, St begin, St end);
// [20] St find1AtMaxIndex(const uint64_t *bitString, St length);
// [20] St find1AtMaxIndex(U64 *bitString, St begin, St end);
// [22] St find1AtMinIndex(const uint64_t *bitString, St length);
// [22] St find1AtMinIndex(U64 *bitString, St begin, St end);
// [ 6] bool isAny0(const uint64_t *bitString, St index, St numBits);
// [ 6] bool isAny1(const uint64_t *bitString, St index, St numBits);
// [13] St num0(const uint64_t *bitString, St index, St numBits);
// [13] St num1(const uint64_t *bitString, St index, St numBits);
// [12] OS& print(OS& stream, U64 *bs, St nb, int lvl, int spl);
// ----------------------------------------------------------------------------
// [23] USAGE EXAMPLE
// [ 1] void populateBitString(U64 *bitString, St idx, char *ascii);
// [ 1] void populateBitStringHex(U64 *bitString, St idx, char *ascii);
// ----------------------------------------------------------------------------

// Note that it was found that Solaris often took about 15 times as long as
// Linux to run these test cases, so efforts were made to reduce test times
// to 1 or 2 seconds on Linux to avoid test cases timing out on Solaris.
//
// The measures used to accelerate tests were:
//: o Using 'incInt' (defined below) to increment indexes and 'numBits' more
//:   aggressively than just doing '++x' every iteration.
//: o Sometime reducing the size of the arrays being tested from 4 words to 3.

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bdlb::BitStringUtil Util;
typedef bdlb::BitMaskUtil   Mask;
typedef bsl::uint64_t       uint64_t;
typedef bsl::uint64_t       U64;       // for doc only
typedef bsl::ostream        OS;
typedef bsl::size_t         size_t;
typedef bsl::size_t         St;        // for doc only

static const size_t k_INVALID_INDEX = Util::k_INVALID_INDEX;

enum { k_BITS_PER_UINT64 = Util::k_BITS_PER_UINT64 };

// For 'k_ALIGNMENT' we want the guaranteed alignment of 'uint64_t' variables.
// Unfortunately, 'bsls::AlignmentFromType' returns the guaranteed alignment
// of 'uint64_t' variables within structs, which is different on Windows than
// 'uint64_t' variables on the stack.

#if defined(BSLS_PLATFORM_OS_WINDOWS)
enum { k_ALIGNMENT       = 4 };
#else
enum { k_ALIGNMENT       = bsls::AlignmentFromType<uint64_t>::VALUE };
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
void shiftPtrIndexPair(uint64_t **ptr, int *index)
    // Shift the specified '*ptr' down by a random amount, and the specified
    // '*index' up by an amount to compensate for it, so the resulting '*ptr',
    // '*index' pair refers to the same bit as the original '*ptr', '*index'
    // pair.
{
    typedef bsls::Types::UintPtr UintPtr;

    unsigned int ptr4Shift = bsl::rand() & 255;
    if (sizeof(uint64_t) == k_ALIGNMENT) {
        // To thoroughly test on Windows, we increment the pointers by 4 bytes
        // times a random number.  When the alignment requirement is 8 bytes,
        // we make sure that random number is even so we are incrementing the
        // pointers by a multiple of 8 bytes.

        ptr4Shift &= ~1;    // make even
    }
    unsigned int indexShift = ptr4Shift * (k_BITS_PER_UINT64 / 2);

    UintPtr scalarPtr = reinterpret_cast<UintPtr>(*ptr);
    if (scalarPtr >  ptr4Shift * 4) {
        scalarPtr -= ptr4Shift * 4;
        *index    += indexShift;
        *ptr = reinterpret_cast<uint64_t *>(scalarPtr);
    }
}

static
void fillWithGarbage(uint64_t *array, size_t sizeInBytes)
    // Fill the specified 'array' having the specified 'sizeInBytes' with
    // pseudo-random numbers.  The behavior is undefined unless
    // '0 < sizeInBytes' and 'sizeInBytes' is a multiple of 'sizeof(uint64_t)'.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <  sizeInBytes);
    BSLS_ASSERT(0 == sizeInBytes % sizeof(uint64_t));

    // This function uses Knuth's MMIX random number generator.

    uint64_t *end = array + sizeInBytes / sizeof(uint64_t);

    static uint64_t seed  = 0x0123456789abcdefULL;
    uint64_t        accum = seed;

    for (; array < end; ++array) {
        // This is an LCG, so the low-order bits have much poorer quality
        // randomness than the higher order bits, so each word of output is
        // assembled from the high-order bits of two successive iterations.
        // This will give us a period of 2^63 instead of 2^64.

        accum = 6364136223846793005ULL * accum + 1442695040888963407ULL;
        uint64_t firstWord = accum;
        accum = 6364136223846793005ULL * accum + 1442695040888963407ULL;

        *array = (firstWord & (~0ULL << 32)) | (accum >> 32);
    }

    seed = accum;
}

int IDX_TABLE[] = { 0, 1, 2, 3, 4, 8, 15, 16, 17, 23, 24, 25, 31, 32, 33,
                    63, 64, 65, 127, 128, 129, 191, 192, 193, 254, 255 };
enum { NUM_IDX_TABLE = sizeof IDX_TABLE / sizeof *IDX_TABLE };

enum { SET_UP_ARRAY_DIM = 4 };

static
void setUpArray(uint64_t  array[SET_UP_ARRAY_DIM],
                int      *iteration,
                bool      fast = false)
    // Fill the specified 'array' with different values depending on the
    // specified '*iteration'.  Increment '*iteration' rapidly if the
    // optionally specified 'fast' is 'true', and increment it slowly
    // otherwise.
{
    // The idea here is to replicate the advantage of table-driven code by
    // having the first 47 values of '*iteration' drive values of the array
    // that normally would have been chosen as special cases by table-driven
    // code.

    const int II_MOD = *iteration % SET_UP_ARRAY_DIM;

    switch (*iteration) {
      case 0:
      case 1:
      case 2:
      case 3: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
        array[II_MOD] = 1;
      } break;
      case 4:
      case 5:
      case 6:
      case 7: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
        array[II_MOD] = 2;
      } break;
      case 8:
      case 9:
      case 10:
      case 11: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
        array[II_MOD] = 5;
      } break;
      case 12:
      case 13:
      case 14:
      case 15: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
        array[II_MOD] = 0x15;
      } break;
      case 16:
      case 17:
      case 18:
      case 19: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
        array[II_MOD] = ~0ULL;
      } break;
      case 20:
      case 21:
      case 22:
      case 23: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
        array[II_MOD] = ~1ULL;
      } break;
      case 24:
      case 25:
      case 26:
      case 27: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
        array[II_MOD] = ~2ULL;
      } break;
      case 28:
      case 29:
      case 30:
      case 31: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
        array[II_MOD] = ~5ULL;
      } break;
      case 32:
      case 33:
      case 34:
      case 35: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
        array[II_MOD] = ~0x15ULL;
      } break;
      case 36:
      case 37:
      case 38:
      case 39: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
        array[II_MOD] = 0ULL;
      } break;
      case 40: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0ULL);
      } break;
      case 41: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0ULL);
      } break;
      case 42: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 1ULL);
      } break;
      case 43: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 5ULL);
      } break;
      case 44: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, 0x15ULL);
      } break;
      case 45: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~1ULL);
      } break;
      case 46: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~5ULL);
      } break;
      case 47: {
        bsl::fill(array + 0, array + SET_UP_ARRAY_DIM, ~0x15ULL);
      } break;
      default: {
        fillWithGarbage(array, sizeof(array[0]) * SET_UP_ARRAY_DIM);
      }
    }

    // The first 40 cases are just repeating the same theme 4 times each, so
    // if the caller is interested in a briefer test we only take the first of
    // each 4.

    *iteration += fast && *iteration < 40 ? 4 : 1;
}

static inline
void incInt(int *x, int maxVal)
    // Increase the specified '*x'.  If '*x' is less than the specified
    // 'maxVal', increase it, but not to a higher value than 'maxVal'.  If
    // '*x == maxVal', increment it.  The behavior is undefined unless
    // '0 <= *x', '*x <= maxVal', and 'maxVal < INT_MAX'.
{
    // This function is used to increment a specified loop counter '*x' with
    // increasing rapidity as it grows, eventually setting it to EXACTLY the
    // specified 'maxVal', after which it will be set to 'maxVal + 1', causing
    // the loop to terminate.

    BSLS_ASSERT_SAFE(0 <= *x);
    BSLS_ASSERT_SAFE(     *x <= maxVal);
    BSLS_ASSERT_SAFE(           maxVal < INT_MAX);

    if (maxVal == *x) {
        ++*x;
        return;                                                       // RETURN
    }

    *x += bsl::max(*x / 4, 1);
    if (*x > maxVal) {
        *x = maxVal;
    }
}

static inline
void incSizeT(size_t *x, size_t maxVal)
    // Increase the specified '*x'.  If '*x' is less than the specified
    // 'maxVal', increase it, but not to a higher value than 'maxVal'.  If
    // '*x == maxVal', increment it.  The behavior is undefined unless
    // '0 <= *x', '*x <= maxVal', and 'maxVal < INT_MAX'.
{
    // This function is used to increment a specified loop counter '*x' with
    // increasing rapidity as it grows, eventually setting it to EXACTLY the
    // specified 'maxVal', after which it will be set to 'maxVal + 1', causing
    // the loop to terminate.

    BSLS_ASSERT_SAFE(*x <= maxVal);
    BSLS_ASSERT_SAFE(      maxVal < INT_MAX);

    if (maxVal == *x) {
        ++*x;
        return;                                                       // RETURN
    }

    *x += bsl::max<size_t>(*x / 4, 1);
    if (*x > maxVal) {
        *x = maxVal;
    }
}

static inline
int intAbs(int x)
    // Return the absolute value of the specified 'x'.  The behavior is
    // undefined unless 'INT_MIN != x'.
{
    BSLS_ASSERT_SAFE(INT_MIN != x);

    return x < 0 ? -x : x;
}

static
bool areBitsEqual(const uint64_t *bitStringLhs,
                  const uint64_t *bitStringRhs,
                  int             index,
                  int             numBits)
    // Return 'true' if the specified 'numBits' of the specified 'bitStringLhs'
    // and the specified 'bitStringRhs', both starting at the specified
    // 'index', compare equal, and 'false' otherwise.  The behavior is
    // undefined unless '0 <= index' and '0 <= numBits'.
{
    // This is a brute-force but reliable way to compare two bit strings.  It's
    // simpler but much less efficient than 'BitStringUtil::areEqual', and is
    // used until that routine is vetted.

    BSLS_ASSERT_SAFE(bitStringLhs);
    BSLS_ASSERT_SAFE(bitStringRhs);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);

    if (! numBits) {
        return true;                                                  // RETURN
    }

    int idx = index / k_BITS_PER_UINT64;
    int pos = index % k_BITS_PER_UINT64;

    const int endIndex = index + numBits;
    const int lastIdx  = (endIndex - 1) / k_BITS_PER_UINT64;
    const int endPos   = (endIndex - 1) % k_BITS_PER_UINT64 + 1;

    for (; idx < lastIdx; ++idx) {
        for (; pos < k_BITS_PER_UINT64; ++pos) {
            if ((bitStringLhs[idx] & (1ULL << pos)) !=
                                         (bitStringRhs[idx] & (1ULL << pos))) {
                return false;                                         // RETURN
            }
        }
        pos = 0;
    }

    for (; pos < endPos; ++pos) {
        if ((bitStringLhs[idx] & (1ULL << pos)) !=
                                         (bitStringRhs[idx] & (1ULL << pos))) {
            return false;                                             // RETURN
        }
    }

    return true;
}

static
size_t countOnes(const uint64_t *bitString,
                 size_t          idx,
                 size_t          numBits)
    // Return the number of set bits in the specified 'numBits' of the
    // specified 'bitString' starting at the specified 'idx'.  The behavior is
    // undefined unless '0 <= idx' and '0 <= numBits'.
{
    size_t ii =                  idx / k_BITS_PER_UINT64;
    int    jj = static_cast<int>(idx % k_BITS_PER_UINT64);

    size_t ret = 0;

    for (size_t kk = 0; kk < numBits; ++kk) {
        ret += !!(bitString[ii] & (1ULL << jj));

        if (k_BITS_PER_UINT64 == ++jj) {
            jj = 0;
            ++ii;
        }
    }

    return ret;
}

inline
void wordCpy(uint64_t *dst, const uint64_t *src, size_t numBytes)
    // Copy the specified 'numBytes' from the specified 'src' to the specified
    // 'dst'.  The behavior is undefined unless 'numBytes' is a multiple of
    // 'sizeof(uint64_t)'.  Note that this function is like 'memcpy' except
    // faster, since it assumes it must copy an integral number of aligned
    // 64-bit words.
{
    BSLS_ASSERT_SAFE(0 == numBytes % sizeof(uint64_t));

    int numWords = static_cast<int>(numBytes / sizeof(uint64_t));
    for (; numWords > 0; --numWords) {
        *dst++ = *src++;
    }
}

inline
int wordCmp(const uint64_t *dst, const uint64_t *src, size_t numBytes)
    // Compare the specified 'numBytes' from the specified 'src' to the
    // specified 'dst'.  Return 0 if they match and a non-zero value otherwise.
    // The behavior is undefined unless 'numBytes' is a multiple of
    // 'sizeof(uint64_t)'.  Note that this function is like 'memcmp' except
    // faster, since it assumes it must compare an integral number of aligned
    // 64-bit words.
{
    BSLS_ASSERT_SAFE(0 == numBytes % sizeof(uint64_t));

    int numWords = static_cast<int>(numBytes / sizeof(uint64_t));
    for (; numWords > 0;  ++dst, ++src, --numWords) {
        if (*dst != *src) {
            return *dst > *src ? 1 : -1;                              // RETURN
        }
    }

    return 0;
}

static
bsl::string pHex(const uint64_t *bitString,
                 int             numBits)
    // Return a 'bsl::string' displaying the contents of the first specified
    // 'numBits' of the specified 'bitString', in hex.
{
    bsl::ostringstream oss;

    Util::print(oss, bitString, numBits, 0, -1);
    return oss.str();
}

static
void populateBitString(uint64_t *bitString, int index, const char *ascii)
    // Populate the bits starting at the specified 'index' in the specified
    // 'bitString' with the values specified by the characters in the specified
    // 'ascii' string.  The behavior is undefined unless the characters in
    // 'ascii' are either '0' or '1', and 'bitString' has a capacity of at
    // least 'index + bsl::strlen(ascii)' bits.
{
    ASSERT(bitString);
    ASSERT(0 <= index);
    ASSERT(ascii);

    int       idx      = index / k_BITS_PER_UINT64;
    int       pos      = index % k_BITS_PER_UINT64;
    uint64_t *wordPtr  = &bitString[idx];
    int       numChars = static_cast<int>(bsl::strlen(ascii));

    while (numChars > 0) {
        unsigned char currValue = ascii[numChars - 1];
        if (bsl::isspace(currValue)) {
            --numChars;
            continue;
        }

        ASSERT('0' == currValue || '1' == currValue);

        if (k_BITS_PER_UINT64 == pos) {
            pos = 0;
            ++wordPtr;
        }

        if ('1' == currValue) {
            *wordPtr |=   1ULL << pos;
        }
        else {
            *wordPtr &= ~(1ULL << pos);
        }

        ++pos;
        --numChars;
    }
}

static
void populateBitStringHex(uint64_t *bitString, int index, const char *ascii)
    // Populate the bits starting at the specified 'index' in the specified
    // 'bitString' with the values specified by the characters in the specified
    // 'ascii' string, which is in hex.  The behavior is undefined unless the
    // characters in 'ascii' are either valid hex digits or whitespace,
    // 'bitString' has a capacity of at least 'index + bsl::strlen(ascii) * 4'
    // bits, '0 <= index', and 'index' is a multiple of 4.
{
    ASSERT(bitString);
    ASSERT(0 <= index);
    ASSERT(ascii);

    int       idx       = index / k_BITS_PER_UINT64;
    int       pos       = index % k_BITS_PER_UINT64;
    uint64_t *wordPtr   = &bitString[idx];
    int       NUM_CHARS = static_cast<int>(bsl::strlen(ascii));

    uint64_t nibble = 16;
    bool     lastCharWasHex = false;
    uint64_t lastNibble     = 17;
    for (int numChars = NUM_CHARS - 1; numChars >= 0;
                                             --numChars, lastNibble = nibble) {
        unsigned char currValue = ascii[numChars];
        if (bsl::isspace(currValue)) {
            continue;
        }

        currValue = static_cast<char>(bsl::tolower(currValue));

        BSLMF_ASSERT('a' > '9');

        int repeat = 1;
        nibble = currValue <= '9'
                 ? (currValue >= '0' ? currValue - '0' : 16)
                 : (currValue >= 'a'
                    ? (currValue <= 'f' ? 10 + currValue - 'a'
                                        : 16)
                    : 16);
        if (nibble < 16) {
            lastCharWasHex = true;
        }
        else {
            ASSERT(17 != lastNibble);
            nibble = lastNibble;
            switch (currValue) {
              case 'y': {    // byte
                repeat = 1;
              } break;
              case 'q': {    // quarter-word
                repeat = 3;
              } break;
              case 'h': {    // half-word (32-bits)
                repeat = 7;
              } break;
              case 'w': {    // word (64 bits)
                repeat = 15;
              } break;
              default: {
                LOOP_ASSERT(currValue, 0 && "Unrecognized char in"
                                                      " populateBitStringHex");
              } break;
            }
            if (!lastCharWasHex) {
                ++repeat;
            }
            lastCharWasHex = false;
        }

        ASSERT(repeat >= 1);
        LOOP_ASSERT(currValue, nibble < 16);

        for (; repeat > 0; --repeat) {
            *wordPtr &= ~(0xfULL << pos);
            *wordPtr |=   nibble << pos;

            if (pos > k_BITS_PER_UINT64 - 4) {
                uint64_t *nextWordPtr = wordPtr + 1;
                int       nextPos     = pos - k_BITS_PER_UINT64;

                  // note nextPos is in range [ -1 .. -3 ]

                *nextWordPtr &= Mask::ge64(nextPos + 4);
                *nextWordPtr |= nibble >> -nextPos;
            }

            pos += 4;
            if (pos >= k_BITS_PER_UINT64) {
                pos -= k_BITS_PER_UINT64;
                ++wordPtr;
            }
        }
    }
}

int numBits(const char *str)
    // Return the number of bits in the specified 'str' ignoring any
    // whitespace.
{
    int n = 0;
    while (*str) {
        if (!bsl::isspace(*str)) {
            ++n;
        }
        ++str;
    }
    return n;
}

int numHexDigits(const char *str)
    // Return the number of nibbles indicated by the specified 'str'.
{
    bool lastWasHex = false;
    int  n          = 0;

    const char *pc = str;
    while (*pc) {
        ++pc;
    }

    for (--pc; pc >= str; --pc) {
        unsigned char currValue = *pc;
        if (!bsl::isspace(currValue)) {
            currValue = static_cast<char>(bsl::tolower(currValue));

            switch (currValue) {
              case 'y': {    // byte
                n += 1  + !lastWasHex;
                lastWasHex = false;
              } break;
              case 'q': {    // quarter-word
                n += 3  + !lastWasHex;
                lastWasHex = false;
              } break;
              case 'h': {    // half-word (32-bits)
                n += 7  + !lastWasHex;
                lastWasHex = false;
              } break;
              case 'w': {    // word (64 bits)
                n += 15 + !lastWasHex;
                lastWasHex = false;
              } break;
              default: {
                lastWasHex = true;
                ++n;
              } break;
            }
        }
    }
    return n;
}

bool areEqualOracle(uint64_t *lhs,
                    size_t    lhsIdx,
                    uint64_t *rhs,
                    size_t    rhsIdx,
                    size_t    numBits)
    // Return 'true' if the specified 'numBits' beginning at the specified
    // 'lhsIdx' in the specified 'lhs' are bitwise equal to the 'numBits'
    // beginning at the specified 'rhsIdx' in the specified 'rhs', and 'false'
    // otherwise.  The behavior is undefined unless '0 <= lhsIdx',
    // '0 <= rhsIdx', '0 <= numBits', 'lhs' has a length of at least
    // 'lhsIdx + numBits', and 'rhs' has a length of at least
    // 'rhsIdx + numBits'.
{
    // Note that this is a really inefficient but reliable way of implementing
    // the 'areEqual' function as an oracle for testing it.  Also note that we
    // can't use 'bit' because it is not tested until after 'areEqual'.

    const size_t endLhsIdx = lhsIdx + numBits;
    for (; lhsIdx < endLhsIdx; ++lhsIdx, ++rhsIdx) {
        const size_t lhsWord =                  lhsIdx / k_BITS_PER_UINT64;
        const int    lhsPos  = static_cast<int>(lhsIdx % k_BITS_PER_UINT64);

        const size_t rhsWord =                  rhsIdx / k_BITS_PER_UINT64;
        const int    rhsPos  = static_cast<int>(rhsIdx % k_BITS_PER_UINT64);

        if (!(lhs[lhsWord] & (1ULL << lhsPos)) !=
                                          !(rhs[rhsWord] & (1ULL << rhsPos))) {
            return false;                                             // RETURN
        }
    }

    return true;
}

void toggleOracle(uint64_t       *dst,
                  size_t          dstIdx,
                  size_t          numBits)
    // Invert the values of the specified 'numBits' in the specified 'dst'
    // beginning at the specified 'dstIdx'.  The behavior is undefined unless
    // '0 <= dstIdx', '0 <= numBits', and 'dst' has a length of at least
    // 'dstIdx + numBits'.  Note that this is a really inefficient but reliable
    // way of implementing the 'toggle' function as an oracle for testing.
{
    const size_t endDstIdx = dstIdx + numBits;
    for (; dstIdx < endDstIdx; ++dstIdx) {
        Util::assign(dst, dstIdx, !Util::bit(dst, dstIdx));
    }
}

void andOracle(uint64_t       *dst,
               size_t          dstIdx,
               const uint64_t *src,
               size_t          srcIdx,
               size_t          numBits)
    // Bitwise AND the specified 'numBits' of the specified 'dst' starting at
    // the specified 'dstIdx' with the 'numBits' of the specified 'src'
    // starting at the specified 'srcIdx', and write the result over the bits
    // that were read from 'dst'.  The behavior is undefined unless
    // '0 <= dstIdx', '0 <= srcIdx', '0 <= numBits', 'dst' has a length of at
    // least 'dstIdx + numBits', and 'src' has a length of at least
    // 'srcIdx + numBits'.  Note that this is a really inefficient but reliable
    // way of implementing the 'andEqual' function as an oracle for testing.
{
    size_t endSrcIdx = srcIdx + numBits;
    for (; srcIdx < endSrcIdx; ++dstIdx, ++srcIdx) {
        Util::assign(dst, dstIdx, Util::bit(dst, dstIdx) &&
                                                       Util::bit(src, srcIdx));
    }
}

void minusOracle(uint64_t       *dst,
                 size_t          dstIdx,
                 const uint64_t *src,
                 size_t          srcIdx,
                 size_t          numBits)
    // Bitwise MINUS the specified 'numBits' of the specified 'src' starting at
    // the specified 'srcIdx' from the 'numBits' of the specified 'dst'
    // starting at the specified 'dstIdx', and write the result over the bits
    // that were read from 'dst'.  The behavior is undefined unless
    // '0 <= dstIdx', '0 <= srcIdx', '0 <= numBits', 'dst' has a length of at
    // least 'dstIdx + numBits', and 'src' has a length of at least
    // 'srcIdx + numBits'.  Note that the logical difference 'A - B' is defined
    // to be 'A & !B'.  Also note that this is a really inefficient but
    // reliable way of implementing the 'minusEqual' function as an oracle for
    // testing.
{
    size_t endSrcIdx = srcIdx + numBits;
    for (; srcIdx < endSrcIdx; ++dstIdx, ++srcIdx) {
        Util::assign(dst, dstIdx, Util::bit(dst, dstIdx) &&
                                                      !Util::bit(src, srcIdx));
    }
}

void orOracle(uint64_t       *dst,
              size_t          dstIdx,
              const uint64_t *src,
              size_t          srcIdx,
              size_t          numBits)
    // Bitwise OR the specified 'numBits' of the specified 'dst' starting at
    // the specified 'dstIdx' with the 'numBits' of the specified 'src'
    // starting at the specified 'srcIdx', and write the result over the bits
    // that were read from 'dst'.  The behavior is undefined unless
    // '0 <= dstIdx', '0 <= srcIdx', '0 <= numBits', 'dst' has a length of at
    // least 'dstIdx + numBits', and 'src' has a length of at least
    // 'srcIdx + numBits'.  Note that this is a really inefficient but reliable
    // way of implementing the 'orEqual' function as an oracle for testing.
{
    size_t endSrcIdx = srcIdx + numBits;
    for (; srcIdx < endSrcIdx; ++dstIdx, ++srcIdx) {
        Util::assign(dst, dstIdx, Util::bit(dst, dstIdx) ||
                                                       Util::bit(src, srcIdx));
    }
}

void xorOracle(uint64_t       *dst,
               size_t          dstIdx,
               const uint64_t *src,
               size_t          srcIdx,
               size_t          numBits)
    // Bitwise XOR the specified 'numBits' of the specified 'dst' starting at
    // the specified 'dstIdx' with the 'numBits' of the specified 'src'
    // starting at the specified 'srcIdx', and write the result over the bits
    // that were read from 'dst'.  The behavior is undefined unless
    // '0 <= dstIdx', '0 <= srcIdx', '0 <= numBits', 'dst' has a length of at
    // least 'dstIdx + numBits', and 'src' has a length of at least
    // 'srcIdx + numBits'.  Note that this is a really inefficient but reliable
    // way of implementing the 'xorEqual' function as an oracle for testing.
{
    size_t endSrcIdx = srcIdx + numBits;
    for (; srcIdx < endSrcIdx; ++dstIdx, ++srcIdx) {
        Util::assign(dst, dstIdx, Util::bit(dst, dstIdx) !=
                                                       Util::bit(src, srcIdx));
    }
}

size_t findAtMaxOracle(uint64_t *bitString,
                       size_t    begin,
                       size_t    end,
                       bool      value)
    // Return the index of the highest-order bit that matches the specified
    // 'value' in the bit string starting at the specified 'begin' index and
    // ending before the specified 'end' index in the specified 'bitString'.
    // The behavior is undefined unless '0 <= begin' and 'begin <= end'.  Note
    // that this function provides an inefficient but reliable way of
    // implementing the 'find*AtMaxIndex' functions for testing.
{
    ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    for (size_t ii = end - 1; true; --ii) {
        if (Util::bit(bitString, ii) == value) {
            return ii;                                                // RETURN
        }

        if (begin == ii) {
            break;
        }
    }

    return k_INVALID_INDEX;
}

size_t findAtMinOracle(uint64_t *bitString,
                       size_t    begin,
                       size_t    end,
                       bool      value)
    // Return the index of the lowest-order bit that matches the specified
    // 'value' in the bit string starting at the specified 'begin' index and
    // ending before the specified 'end' index in the specified 'bitString'.
    // The behavior is undefined unless '0 <= begin' and 'begin <= end'.  Note
    // that this function provides an inefficient but reliable way of
    // implementing the 'find*AtMinIndex' functions for testing.
{
    ASSERT(begin <= end);

    if (begin == end) {
        return k_INVALID_INDEX;                                       // RETURN
    }

    for (size_t ii = begin; ii < end; ++ii) {
        if (Util::bit(bitString, ii) == value) {
            return ii;                                                // RETURN
        }
    }

    return k_INVALID_INDEX;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 23: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE\n"
                               "=============\n";

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Maintaining a Calendar of Business Days
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Bit strings can be used to represent business calendars and facilitate
// efficient operations on such calendars.  We will use bit strings to mark
// properties of days of the year 2013.
//
// First, create an enumeration showing the number of days in the year 2013
// before the beginning of each month, so that:
//..
// <constant for month> + <day of month> == <day of year>

    enum {
        JAN =        0,    // Note: First DOY is 'JAN + 1'.
        FEB = JAN + 31,
        MAR = FEB + 28,    // 2013 was not a leap year.
        APR = MAR + 31,
        MAY = APR + 30,
        JUN = MAY + 31,
        JUL = JUN + 30,
        AUG = JUL + 31,
        SEP = AUG + 31,
        OCT = SEP + 30,
        NOV = OCT + 31,
        DEC = NOV + 30
    };
//..
// Then, create a bit string with sufficient capacity to represent every day
// of a year (note that 64 * 6 = 384) and set a 1-bit in the indices
// corresponding to the day-of-year (DOY) for each weekend day.  For
// convenience in date calculations, the 0 index is not used; the 365 days of
// the year are at indices '[1 .. 365]'.  Further note that the values set
// below correspond to the year 2013:
//..
    uint64_t weekends[6] = { 0 };

    // We are marking only weekend days, so start with the first weekend day
    // of the year: Saturday, January 5, 2013.

    for (int i = 5; i < 366; i += 7) {
        bdlb::BitStringUtil::assign(weekends, i,   1);
        if (i + 1 < 366) {
            bdlb::BitStringUtil::assign(weekends, i + 1, 1);
        }
    }
//..
// Next, we can easily use 'bdlb::BitStringUtil' methods to find days of
// interest.  For example, we can find the first and last weekend days of the
// year:
//..
    const bsl::size_t firstWeekendDay = bdlb::BitStringUtil::find1AtMinIndex(
                                                                      weekends,
                                                                      365 + 1);
    const bsl::size_t lastWeekendDay  = bdlb::BitStringUtil::find1AtMaxIndex(
                                                                      weekends,
                                                                      365 + 1);

    ASSERT(JAN +  5 == firstWeekendDay);
    ASSERT(DEC + 29 ==  lastWeekendDay);
//..
// Then, we define the following enumeration that allows us to easily represent
// the US holidays of the year:
//..
    uint64_t holidays[6] = { 0 };

    enum USHolidays2013 {
        NEW_YEARS_DAY             = JAN +  1,
        MARTIN_LUTHER_KING_JR_DAY = JAN + 21,
        PRESIDENTS_DAY            = FEB + 18,
        GOOD_FRIDAY               = MAR + 29,
        MEMORIAL_DAY              = MAY + 27,
        INDEPENDENCE_DAY          = JUL +  4,
        LABOR_DAY                 = SEP +  2,
        THANKSGIVING              = NOV + 28,
        CHRISTMAS                 = DEC + 25
    };

    bdlb::BitStringUtil::assign(holidays, NEW_YEARS_DAY,             true);
    bdlb::BitStringUtil::assign(holidays, MARTIN_LUTHER_KING_JR_DAY, true);
    bdlb::BitStringUtil::assign(holidays, PRESIDENTS_DAY,            true);
    bdlb::BitStringUtil::assign(holidays, GOOD_FRIDAY,               true);
    bdlb::BitStringUtil::assign(holidays, MEMORIAL_DAY,              true);
    bdlb::BitStringUtil::assign(holidays, INDEPENDENCE_DAY,          true);
    bdlb::BitStringUtil::assign(holidays, LABOR_DAY,                 true);
    bdlb::BitStringUtil::assign(holidays, THANKSGIVING,              true);
    bdlb::BitStringUtil::assign(holidays, CHRISTMAS,                 true);
//..
// Next, the following enumeration indicates the beginning of fiscal quarters:
//..
    enum {
        Q1 = JAN + 1,
        Q2 = APR + 1,
        Q3 = JUN + 1,
        Q4 = OCT + 1
    };
//..
// Now, we can query our calendar for the first holiday in the third quarter,
// if any:
//..
    const bsl::size_t firstHolidayOfQ3 = bdlb::BitStringUtil::find1AtMinIndex(
                                                                      holidays,
                                                                      Q3,
                                                                      Q4);
    ASSERT(INDEPENDENCE_DAY == firstHolidayOfQ3);
//..
// Finally, our weekend and holiday calendars are readily combined to represent
// days off for either reason (i.e., holiday or weekend):
//..
    uint64_t allDaysOff[6] = { 0 };
    bdlb::BitStringUtil::orEqual(allDaysOff, 1, weekends, 1, 365);
    bdlb::BitStringUtil::orEqual(allDaysOff, 1, holidays, 1, 365);

    bool isOffMay24 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 24);
    bool isOffMay25 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 25);
    bool isOffMay26 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 26);
    bool isOffMay27 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 27);
    bool isOffMay28 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 28);

    ASSERT(false == isOffMay24);
    ASSERT(true  == isOffMay25);    // Saturday
    ASSERT(true  == isOffMay26);    // Sunday
    ASSERT(true  == isOffMay27);    // Note May 27, 2013 is Memorial Day.
    ASSERT(false == isOffMay28);
//..
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'find1AtMinIndex' METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That both 'find1AtMinIndex' functions correctly return the index
        //:   of the lowest-order set bit in a range, or -1 if no set bit
        //:   exists.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use table-driven code to test both 'find1AtMinIndex' and the
        //:   'findAtMinOracle' function in this test driver.
        //:
        //: 2 Iterate over different test arrays with 'setUpArray'.
        //:   o Iterate over 'length' values from 0 to the size of the array.
        //:     1 Find the min 1 bit, if any, in the array using both
        //:       'find1AtMinIndex' and 'findAtMinOracle' and verify their
        //:       results match.
        //:
        //:     2 Iterate 'idx' from 0 to 'length'.
        //:       o Find the min 1 bit, if any, in the array using both
        //:         'find1AtMinIndex' and 'findAtMinOracle' for the given 'idx'
        //:         and 'length' and verify their results match.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   St find1AtMinIndex(const uint64_t *bitString, St length);
        //   St find1AtMinIndex(U64 *bitString, St begin, St end);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'find1AtMinIndex' METHODS\n"
                          << "=================================\n";

        const struct {
            int         d_line;
            const char *d_array_p;
            int         d_index;
            int         d_smallestIdx;
            int         d_lessThanIdx;
            int         d_lessThanEqualIdx;
            int         d_greaterThanIdx;
            int         d_greaterThanEqualIdx;
        } DATA [] = {
// Line  array              idx    S    LT   LE    GT    GE
// ----  -----              ---   --    --   --    --    --
{   L_,  "1",                 0,   0,   -1,   0,   -1,    0   },
{   L_,  "0",                 0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "00",                0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "00",                1,  -1,   -1,  -1,   -1,   -1   },

{   L_,  "01",                0,   0,   -1,   0,   -1,    0   },
{   L_,  "01",                1,   0,    0,   0,   -1,   -1   },
{   L_,  "10",                0,   1,   -1,  -1,    1,    1   },
{   L_,  "10",                1,   1,   -1,   1,   -1,    1   },
{   L_,  "11",                0,   0,   -1,   0,    1,    0   },
{   L_,  "11",                1,   0,    0,   0,   -1,    1   },

{   L_,  "10010000 11110000 11110000 11110000",
                              2,   4,   -1,  -1,    4,    4   },
{   L_,  "10010000 11110000 11110000 00001011",
                              2,   0,    0,   0,    3,    3   },
{   L_,  "00000000 00000000 00000000 00000000",
                              0,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "0 00000000 00000000 00000000 00000000",
                              0,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "00000000 00000000 00000000 00000000",
                              2,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "00000000 00000000 00000000 00000000",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "0 00000000 00000000 00000000 00000000",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11110000 11110000 11110000 11110000",
                              15,   4,   4,   4,   20,    15   },
{   L_,  "11110000 11110000 01110000 11110000",
                              15,   4,   4,   4,   20,    20   },
{   L_,  "10010000 11110000 11110000 11110000",
                              0,    4,  -1,  -1,    4,     4   },
{   L_,  "00010000 11110000 11110000 11110001",
                              0,    0,  -1,   0,    4,     0   },
{   L_,  "11010000 11110000 11110000 11110000",
                             30,    4,   4,   4,   31,    30   },
{   L_,  "01010000 11110000 11110000 11110000",
                             30,    4,   4,   4,   -1,    30   },
{   L_,  "0 11010000 11110000 11110000 11111111",
                             30,    0,   0,   0,   31,    30   },
{   L_,  "01 01010000 11110000 11110000 11111111",
                             30,    0,   0,   0,   32,    30   },
{   L_,  "01010000 11110000 11110000 11110001",
                             31,    0,   0,   0,   -1,    -1   },
{   L_,  "10010000 11110000 11110000 11110001",
                             31,    0,   0,   0,   -1,    31   },
{   L_,  "0 10010000 11110001 00000000 00000000",
                             31,   16,  16,  16,   -1,    31   },
{   L_,  "1 10010000 11110001 00000000 00000000",
                             31,   16,  16,  16,   32,    31   },
{   L_,  "0 00010000 11110001 00000000 00000000",
                             32,   16,  16,  16,   -1,    -1   },
{   L_,  "1 00010000 11110001 00000000 00000000",
                             32,   16,  16,  16,   -1,    32   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 00011111",
                             0,     0,  -1,   0,    1,     0   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 00011110",
                             0,     1,  -1,  -1,    1,     1   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 00001111",
                            63,     0,   0,   0,   -1,    63   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 00001111",
                            63,     0,   0,   0,   -1,    -1   },
{   L_,
"1 10010000 11110000 11110000 11110000 00010000 11110000 11110000 00001111",
                            63,     0,   0,   0,   64,    63   },
{   L_,
"0 10010000 11110000 11110000 11110000 00010000 11110000 11110000 00001111",
                            63,     0,   0,   0,   -1,    63   },
{   L_,
"0 00010000 11110000 11110000 11110000 00010000 11110000 11110000 00001111",
                            63,     0,   0,   0,   -1,    -1   },
{   L_,
"00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000",
                            63,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"0 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000",
                            63,   -1,   -1,  -1,   -1,    -1   },
{ L_,
                                                                "00001000"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000000"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000010"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000000",
                            130,  65,   65,  65,  195,   195   },

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const int MAX_ARRAY_SIZE = 4;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE      = DATA[i].d_line;
            const char   *STR       = DATA[i].d_array_p;
            const size_t  LEN       = numBits(STR);
            const size_t  IDX       = DATA[i].d_index;
            const size_t  EXP_S     = DATA[i].d_smallestIdx;
            const size_t  EXP_GT    = DATA[i].d_greaterThanIdx;
            const size_t  EXP_GE    = DATA[i].d_greaterThanEqualIdx;
            const size_t  EXP_LT    = DATA[i].d_lessThanIdx;
            const size_t  EXP_LE    = DATA[i].d_lessThanEqualIdx;

            if (veryVerbose) {
                P_(LINE) P_(STR) P_(IDX) P_(EXP_S) P_(EXP_LT) P_(EXP_LE)
                P_(EXP_GT) P_(EXP_GE)
            }

            uint64_t array[MAX_ARRAY_SIZE] = { 0 };
            populateBitString(array, 0, STR);

            if (veryVerbose) {
                Util::print(cout, array, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            const size_t S  = Util::find1AtMinIndex(array, LEN);
            LOOP3_ASSERT(LINE,  S, EXP_S,   S == EXP_S);
            ASSERT(EXP_S  == findAtMinOracle(array, 0, LEN, true));

            const size_t GT = Util::find1AtMinIndex(array, IDX + 1, LEN);
            LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
            ASSERT(EXP_GT == findAtMinOracle(array, IDX + 1, LEN, true));

            const size_t GE = Util::find1AtMinIndex(array, IDX, LEN);
            LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
            ASSERT(EXP_GE == findAtMinOracle(array, IDX, LEN, true));

            const size_t LT = Util::find1AtMinIndex(array, IDX);
            LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
            ASSERT(EXP_LT == findAtMinOracle(array, 0, IDX, true));

            const size_t LE = Util::find1AtMinIndex(array, IDX + 1);
            LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
            ASSERT(EXP_LE == findAtMinOracle(array, 0, IDX + 1, true));
        }

        const size_t NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t bits[SET_UP_ARRAY_DIM], control[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 150;) {
            setUpArray(control, &ii);
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }

            for (size_t length = 0; length <= NUM_BITS; ++length) {
                {
                    const size_t EXP = findAtMinOracle(bits, 0, length, true);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny1(bits, 0, length));
                    }
                    else {
                        ASSERT(EXP < length);
                        ASSERT(Util::bit(bits, EXP));
                        ASSERT(! Util::isAny1(bits, 0, EXP));
                    }

                    ASSERT(EXP == Util::find1AtMinIndex(bits, length));
                }

                for (size_t idx = 0; idx <= length; ++idx) {
                    const size_t EXP =
                                      findAtMinOracle(bits, idx, length, true);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny1(bits, idx, length - idx));
                    }
                    else {
                        ASSERT(idx <= EXP && EXP < length);
                        ASSERT(Util::bit(bits, EXP));
                        ASSERT(! Util::isAny1(bits, idx, EXP - idx));
                    }

                    ASSERT(EXP == Util::find1AtMinIndex(bits, idx, length));
                }
            }

            ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::find1AtMinIndex(bits, 0));
            ASSERT_PASS(Util::find1AtMinIndex(bits, 100));
            ASSERT_FAIL(Util::find1AtMinIndex(   0,   0));

            ASSERT_PASS(Util::find1AtMinIndex(bits, 0));
            ASSERT_PASS(Util::find1AtMinIndex(bits, 0,   0));
            ASSERT_PASS(Util::find1AtMinIndex(bits, 0, 100));
            ASSERT_PASS(Util::find1AtMinIndex(bits, 10, 100));
            ASSERT_FAIL(Util::find1AtMinIndex(   0, 0,   0));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'find0AtMinIndex' METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That both 'find0AtMinIndex' functions correctly return the index
        //:   of the lowest-order cleared bit in a range, or -1 if no cleared
        //:   bit exists.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use table-driven code to test both 'find0AtMinIndex' and the
        //:   'findAtMinOracle' function in this test driver.
        //:
        //: 2 Iterate over different test arrays with 'setUpArray'.
        //:   o Iterate over 'length' values from 0 to the size of the array.
        //:     1 Find the min 0 bit, if any, in the array using both
        //:       'find0AtMinIndex' and 'findAtMinOracle' and verify their
        //:       results match.
        //:
        //:     2 Iterate 'index' from 0 to 'length'.
        //:       o Find the min 0 bit, if any, in the array using both
        //:         'find0AtMinIndex' and 'findAtMinOracle' for the given
        //:         'index' and 'length' and verify their results match.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   St find0AtMinIndex(const uint64_t *bitString, St length);
        //   St find0AtMinIndex(U64 *bitString, St begin, St end);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'find0AtMinIndex' METHODS\n"
                          << "=================================\n";

        const struct {
            int         d_line;
            const char *d_array_p;
            int         d_index;
            int         d_smallestIdx;
            int         d_lessThanIdx;
            int         d_lessThanEqualIdx;
            int         d_greaterThanIdx;
            int         d_greaterThanEqualIdx;
        } DATA [] = {
// Line  array              idx    S    LT   LE    GT    GE
// ----  -----              ---   --    --   --    --    --
{   L_,  "1",                 0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "0",                 0,   0,   -1,   0,   -1,    0   },
{   L_,  "00",                0,   0,   -1,   0,    1,    0   },
{   L_,  "00",                1,   0,    0,   0,   -1,    1   },

{   L_,  "01",                0,   1,   -1,  -1,    1,    1   },
{   L_,  "01",                1,   1,   -1,   1,   -1,    1   },
{   L_,  "10",                0,   0,   -1,   0,   -1,    0   },
{   L_,  "10",                1,   0,    0,   0,   -1,   -1   },
{   L_,  "11",                0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "11",                1,  -1,   -1,  -1,   -1,   -1   },

{   L_,  "10010000 11110000 11110000 00001111",
                              2,   4,   -1,  -1,    4,    4   },
{   L_,  "10010000 11110000 11110000 00001011",
                              2,   2,   -1,   2,    4,    2   },
{   L_,  "11111111 11111111 11111111 11111111",
                              0,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11111111 11111111 11111111 11111111",
                              2,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11111111 11111111 11111111 11111111",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "1 11111111 11111111 11111111 11111111",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11110000 11110000 11110000 11110000",
                              15,   0,   0,   0,   16,    16   },
{   L_,  "11110000 11110000 01110000 11110000",
                              15,   0,   0,   0,   16,    15   },
{   L_,  "10010000 11110000 11110000 11110000",
                              0,    0,  -1,   0,    1,     0   },
{   L_,  "00010000 11110000 11110000 11110001",
                              0,    1,  -1,  -1,    1,     1   },
{   L_,  "10010000 11110000 11110000 11110000",
                             30,    0,   0,   0,   -1,    30   },
{   L_,  "00010000 11110000 11110000 11110000",
                             30,    0,   0,   0,   31,    30   },
{   L_,  "0 00010000 11110000 11110000 11111111",
                             30,    8,   8,   8,   31,    30   },
{   L_,  "00 00010000 11110000 11110000 11111111",
                             30,    8,   8,   8,   31,    30   },
{   L_,  "10010000 11110000 11110000 11110000",
                             31,    0,   0,   0,   -1,    -1   },
{   L_,  "00010000 11110000 11110000 11110000",
                             31,    0,   0,   0,   -1,    31   },
{   L_,  "1 00010000 11110000 11111111 11111111",
                             31,   16,  16,  16,   -1,    31   },
{   L_,  "0 00010000 11110000 11111111 11111111",
                             31,   16,  16,  16,   32,    31   },
{   L_,  "1 00010000 11110000 11111111 11111111",
                             32,   16,  16,  16,   -1,    -1   },
{   L_,  "0 00010000 11110000 11111111 11111111",
                             32,   16,  16,  16,   -1,    32   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                             0,     0,  -1,   0,    1,     0   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110001",
                             0,     1,  -1,  -1,    1,     1   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,     0,   0,   0,   -1,    63   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,     0,   0,   0,   -1,    -1   },
{   L_,
"0 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,     0,   0,   0,   64,    63   },
{   L_,
"1 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,     0,   0,   0,   -1,    63   },
{   L_,
"1 10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,     0,   0,   0,   -1,    -1   },
{   L_,
"111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"11 111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                            63,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"1 111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                            63,   -1,   -1,  -1,   -1,    -1   },
{ L_,
                                                                "11110111"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11111111"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11111101"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11111111",
                            69,   65,   65,  65,  195,   195   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE      = DATA[i].d_line;
            const char   *STR       = DATA[i].d_array_p;
            const size_t  LEN       = numBits(STR);
            const size_t  IDX       = DATA[i].d_index;
            const size_t  EXP_S     = DATA[i].d_smallestIdx;
            const size_t  EXP_GT    = DATA[i].d_greaterThanIdx;
            const size_t  EXP_GE    = DATA[i].d_greaterThanEqualIdx;
            const size_t  EXP_LT    = DATA[i].d_lessThanIdx;
            const size_t  EXP_LE    = DATA[i].d_lessThanEqualIdx;

            if (veryVerbose) {
                P_(LINE) P_(STR) P_(IDX) P_(EXP_S) P_(EXP_LT) P_(EXP_LE)
                P_(EXP_GT) P_(EXP_GE)
            }

            const int MAX_ARRAY_SIZE = 4;

            uint64_t array[MAX_ARRAY_SIZE];
            bsl::memset(array, 0xFF, sizeof array);
            populateBitString(array, 0, STR);

            if (veryVerbose) {
                Util::print(cout, array, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            const size_t S  = Util::find0AtMinIndex(array, LEN);
            LOOP3_ASSERT(LINE, S, EXP_S, S == EXP_S);
            ASSERT(EXP_S  == findAtMinOracle(array, 0, LEN, false));

            const size_t GT = Util::find0AtMinIndex(array, IDX + 1, LEN);
            LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
            ASSERT(EXP_GT == findAtMinOracle(array, IDX + 1, LEN, false));

            const size_t GE = Util::find0AtMinIndex(array, IDX, LEN);
            LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
            ASSERT(EXP_GE == findAtMinOracle(array, IDX, LEN, false));

            const size_t LT = Util::find0AtMinIndex(array, IDX);
            LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
            LOOP3_ASSERT(LINE, EXP_LT, findAtMinOracle(array, 0, IDX, false),
                              EXP_LT == findAtMinOracle(array, 0, IDX, false));

            const size_t LE = Util::find0AtMinIndex(array, IDX + 1);
            LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
            ASSERT(EXP_LE == findAtMinOracle(array, 0, IDX + 1, false));
        }

        const size_t NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t bits[SET_UP_ARRAY_DIM], control[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }

            for (size_t length = 0; length <= NUM_BITS; ++length) {
                {
                    const size_t EXP = findAtMinOracle(bits, 0, length, false);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny0(bits, 0, length));
                    }
                    else {
                        ASSERT(EXP < length);
                        ASSERT(! Util::bit(bits, EXP));
                        ASSERT(! Util::isAny0(bits, 0, EXP));
                    }

                    ASSERT(EXP == Util::find0AtMinIndex(bits, length));
                }

                for (size_t idx = 0; idx <= length; ++idx) {
                    const size_t EXP =
                                     findAtMinOracle(bits, idx, length, false);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny0(bits, idx, length - idx));
                    }
                    else {
                        ASSERT(idx <= EXP && EXP < length);
                        ASSERT(! Util::bit(bits, EXP));
                        ASSERT(! Util::isAny0(bits, idx, EXP - idx));
                    }

                    ASSERT(EXP == Util::find0AtMinIndex(bits, idx, length));
                }
            }

            ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::find0AtMinIndex(bits, 0));
            ASSERT_PASS(Util::find0AtMinIndex(bits, 100));
            ASSERT_FAIL(Util::find0AtMinIndex(   0, 0));

            ASSERT_PASS(Util::find0AtMinIndex(bits, 0));
            ASSERT_PASS(Util::find0AtMinIndex(bits,  0,   0));
            ASSERT_PASS(Util::find0AtMinIndex(bits,  0, 100));
            ASSERT_PASS(Util::find0AtMinIndex(bits, 10, 100));
            ASSERT_FAIL(Util::find0AtMinIndex(   0, 0,   0));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'find1AtMaxIndex' METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That both 'find1AtMaxIndex' functions correctly return the index
        //:   of the highest-order set bit in a range, or -1 if no set bit
        //:   exists.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use table-driven code to test both 'find1AtMaxIndex' and the
        //:   'findAtMaxOracle' function in this test driver.
        //:
        //: 2 Iterate over different test arrays with 'setUpArray'.
        //:   o Iterate over 'length' values from 0 to the size of the array.
        //:     1 Find the max 0 bit, if any, in the array using both
        //:       'find1AtMaxIndex' and 'findAtMaxOracle' and verify their
        //:       results match.
        //:
        //:     2 Iterate 'index' from 0 to 'length'.
        //:       o Find the max 0 bit, if any, in the array using both
        //:         'find1AtMaxIndex' and 'findAtMaxOracle' for the given
        //:         'index' and 'length' and verify their results match.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   St find1AtMaxIndex(const uint64_t *bitString, St length);
        //   St find1AtMaxIndex(U64 *bitString, St begin, St end);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'find1AtMaxIndex' METHODS\n"
                             "=================================\n";

        const struct {
            int         d_line;
            const char *d_array_p;
            int         d_index;
            int         d_largestIdx;
            int         d_lessThanIdx;
            int         d_lessThanEqualIdx;
            int         d_greaterThanIdx;
            int         d_greaterThanEqualIdx;
        } DATA [] = {
// Line  array              idx    L    LT   LE    GT    GE
// ----  -----              ---   --    --   --    --    --
{   L_,  "1",                 0,   0,   -1,   0,   -1,    0   },
{   L_,  "0",                 0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "00",                0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "00",                1,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "01",                0,   0,   -1,   0,   -1,    0   },
{   L_,  "01",                1,   0,    0,   0,   -1,   -1   },
{   L_,  "10",                0,   1,   -1,  -1,    1,    1   },
{   L_,  "10",                1,   1,   -1,   1,   -1,    1   },
{   L_,  "11",                0,   1,   -1,   0,    1,    1   },
{   L_,  "11",                1,   1,    0,   1,   -1,    1   },

{   L_,  "10010000 11110000 11110000 11110000",
                              4,   31,  -1,   4,   31,    31   },
{   L_,  "10010000 11110000 11110000 11100001",
                              4,   31,   0,   0,   31,    31   },
{   L_,  "00000000 00000000 00000000 00000000",
                              0,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "00000000 00000000 00000000 00000000",
                              2,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "00000000 00000000 00000000 00000000",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "0 00000000 00000000 00000000 00000000",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "00001111 11110000 00001111 11110000",
                              16,  27,  11,  11,   27,    27   },
{   L_,  "00001111 11110000 10001111 11110000",
                              15,  27,  11,  15,   27,    27   },
{   L_,  "01010000 11110000 11110000 00001111",
                              0,   30,  -1,   0,   30,    30   },
{   L_,  "10010000 11110000 11110000 00001111",
                              0,   31,  -1,   0,   31,    31   },
{   L_,  "1 00010000 11110000 11110000 00001111",
                              0,   32,  -1,   0,   32,    32   },
{   L_,  "11 00010000 11110000 11110000 00001111",
                              0,   33,  -1,   0,   33,    33   },
{   L_,  "01010000 11110000 11110000 11110000",
                             30,   30,  28,  30,   -1,    30   },
{   L_,  "10010000 11110000 11110000 11110000",
                             30,   31,  28,  28,   31,    31   },
{   L_,  "1 01010000 11110000 11110000 11110000",
                             30,   32,  28,  30,   32,    32   },
{   L_,  "11 01010000 11110000 11110000 11110000",
                             30,   33,  28,  30,   33,    33   },
{   L_,  "01010000 11110000 11110000 11110000",
                             31,   30,  30,  30,   -1,    -1   },
{   L_,  "10010000 11110000 11110000 11110000",
                             31,   31,  28,  31,   -1,    31   },
{   L_,  "0 10010000 11110000 11110000 11110000",
                             31,   31,  28,  31,   -1,    31   },
{   L_,  "1 10010000 11110000 11110000 11110000",
                             31,   32,  28,  31,   32,    32   },
{   L_,  "1 10010000 11110000 11110000 11110000",
                             32,   32,  31,  32,   -1,    32   },
{   L_,  "1 10010000 11110000 11110000 11110000",
                             32,   32,  31,  32,   -1,    32   },
{   L_,
"01010000 11110000 11110000 11110000 00010000 11110000 11110000 11110001",
                             0,   62,   -1,   0,   62,    62   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110001",
                             0,   63,   -1,   0,   63,    63   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                             0,   63,   -1,  -1,   63,    63   },
{   L_,
"1 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110001",
                             0,   64,   -1,   0,   64,    64   },
{   L_,
"11010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   63,   62,  63,   -1,    63   },
{   L_,
"01010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   62,   62,  62,   -1,    -1   },
{   L_,
"1 11010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   64,   62,  63,   64,    64   },
{   L_,
"0 11010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   63,   62,  63,   -1,    63   },
{   L_,
"000000000 00000000 00000000 00000000 00000000 00000000 00000000 000000000",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"00 000000000 00000000 00000000 00000000 00000000 00000000 00000000 000000000",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"000000000 00000000 00000000 00000000 00000000 00000000 00000000 000000000",
                            63,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"0 000000000 00000000 00000000 00000000 00000000 00000000 00000000 000000000",
                            63,   -1,   -1,  -1,   -1,    -1   },
{ L_,
                                                                    "0000"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000000"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000000"
"00000000 00000000 00000000 00000000  00000000 00000000 00000000 00001000",
                        64*3+3,    3,    3,   3,   -1,    -1,  },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE      = DATA[i].d_line;
            const char   *STR       = DATA[i].d_array_p;
            const size_t  LEN       = numBits(STR);
            const size_t  IDX       = DATA[i].d_index;
            const size_t  EXP_L     = DATA[i].d_largestIdx;
            const size_t  EXP_GT    = DATA[i].d_greaterThanIdx;
            const size_t  EXP_GE    = DATA[i].d_greaterThanEqualIdx;
            const size_t  EXP_LT    = DATA[i].d_lessThanIdx;
            const size_t  EXP_LE    = DATA[i].d_lessThanEqualIdx;

            const int MAX_ARRAY_SIZE = 4;

            if (veryVerbose) {
                P_(LINE) P_(STR) P_(IDX) P_(EXP_L) P_(EXP_LT) P_(EXP_LE)
                P_(EXP_GT) P_(EXP_GE)
            }

            uint64_t array[MAX_ARRAY_SIZE];
            bsl::fill(array + 0, array + MAX_ARRAY_SIZE, 0ULL);
            populateBitString(array, 0, STR);

            if (veryVerbose) {
                Util::print(cout, array, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            const size_t L  = Util::find1AtMaxIndex(  array, LEN);
            LOOP3_ASSERT(LINE, L, EXP_L, L == EXP_L);
            ASSERT(EXP_L  == findAtMaxOracle(array, 0, LEN, true));

            const size_t GT = Util::find1AtMaxIndex(array,   IDX + 1, LEN);
            LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
            ASSERT(EXP_GT == findAtMaxOracle(array, IDX+1, LEN, true));

            const size_t GE = Util::find1AtMaxIndex(array,   IDX,     LEN);
            LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
            ASSERT(EXP_GE == findAtMaxOracle(array, IDX, LEN, true));

            const size_t LT = Util::find1AtMaxIndex(array, IDX);
            LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
            ASSERT(EXP_LT == findAtMaxOracle(array, 0, IDX, true));

            const size_t LE = Util::find1AtMaxIndex(array, IDX + 1);
            LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
            ASSERT(EXP_LE == findAtMaxOracle(array, 0, IDX + 1, true));
        }

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t bits[SET_UP_ARRAY_DIM] = { 0 }, control[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }

            for (size_t length = 0; length <= NUM_BITS; ++length) {
                {
                    const size_t EXP = findAtMaxOracle(bits, 0, length, true);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny1(bits, 0, length));
                    }
                    else {
                        ASSERT(EXP < length);
                        ASSERT(Util::bit(bits, EXP));
                        ASSERT(! Util::isAny1(bits,
                                              EXP + 1,
                                              length - EXP - 1));
                    }

                    ASSERT(EXP == Util::find1AtMaxIndex(bits, length));
                }

                for (size_t idx = 0; idx <= length; ++idx) {
                    const size_t EXP =findAtMaxOracle(bits, idx, length, true);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny1(bits, idx, length - idx));
                    }
                    else {
                        ASSERT(idx <= EXP && EXP < length);
                        ASSERT(Util::bit(bits, EXP));
                        ASSERT(! Util::isAny1(bits, EXP + 1, length-EXP-1));
                    }

                    ASSERT(EXP == Util::find1AtMaxIndex(bits, idx, length));
                }
            }

            ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::find1AtMaxIndex(bits, 0));
            ASSERT_PASS(Util::find1AtMaxIndex(bits, 100));
            ASSERT_FAIL(Util::find1AtMaxIndex(   0,   0));

            ASSERT_PASS(Util::find1AtMaxIndex(bits, 0, 0));
            ASSERT_PASS(Util::find1AtMaxIndex(bits, 0, 100));
            ASSERT_PASS(Util::find1AtMaxIndex(bits, 10, 100));
            ASSERT_FAIL(Util::find1AtMaxIndex(   0, 0, 0));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'find0AtMaxIndex' METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That both 'find0AtMaxIndex' functions correctly return the index
        //:   of the highest-order cleared bit in a range, or -1 if no cleared
        //:   bit exists.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use table-driven code to test both 'find0AtMaxIndex' and the
        //:   'findAtMaxOracle' function in this test driver.
        //:
        //: 2 Iterate over different test arrays with 'setUpArray'.
        //:   o Iterate over 'length' values from 0 to the size of the array.
        //:     1 Find the max 0 bit, if any, in the array using both
        //:       'find0AtMaxIndex' and 'findAtMaxOracle' and verify their
        //:       results match.
        //:
        //:     2 Iterate 'index' from 0 to 'length'.
        //:       o Find the max 0 bit, if any, in the array using both
        //:         'find0AtMaxIndex' and 'findAtMaxOracle' for the given
        //:         'index' and 'length' and verify their results match.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   St find0AtMaxIndex(const uint64_t *bitString, St length);
        //   St find0AtMaxIndex(U64 *bitString, St begin, St end);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'find0AtMaxIndex' METHODS\n"
                             "=================================\n";

        const struct {
            int         d_line;
            const char *d_array_p;
            int         d_index;
            int         d_largestIdx;
            int         d_lessThanIdx;
            int         d_lessThanEqualIdx;
            int         d_greaterThanIdx;
            int         d_greaterThanEqualIdx;
        } DATA [] = {
// Line  array              idx    L    LT   LE    GT    GE
// ----  -----              ---   --    --   --    --    --
{   L_,  "1",                 0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "0",                 0,   0,   -1,   0,   -1,    0   },
{   L_,  "00",                0,   1,   -1,   0,    1,    1   },
{   L_,  "00",                1,   1,    0,   1,   -1,    1   },
{   L_,  "01",                0,   1,   -1,  -1,    1,    1   },
{   L_,  "01",                1,   1,   -1,   1,   -1,    1   },
{   L_,  "10",                0,   0,   -1,   0,   -1,    0   },
{   L_,  "10",                1,   0,    0,   0,   -1,   -1   },
{   L_,  "11",                0,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "11",                1,  -1,   -1,  -1,   -1,   -1   },
{   L_,  "10010000 11110000 11110000 11110000",
                              2,   30,   1,   2,   30,    30   },
{   L_,  "10010000 11110000 11110000 11110100",
                              2,   30,   1,   1,   30,    30   },
{   L_,  "11111111 11111111 11111111 11111111",
                              0,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11111111 11111111 11111111 11111111",
                              2,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11111111 11111111 11111111 11111111",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "1 11111111 11111111 11111111 11111111",
                             31,   -1,  -1,  -1,   -1,    -1   },
{   L_,  "11110000 11110000 11110000 11110000",
                              15,  27,  11,  11,   27,    27   },
{   L_,  "11110000 11110000 01110000 11110000",
                              15,  27,  11,  15,   27,    27   },
{   L_,  "10010000 11110000 11110000 11110000",
                              0,   30,  -1,   0,   30,    30   },
{   L_,  "00010000 11110000 11110000 11110000",
                              0,   31,  -1,   0,   31,    31   },
{   L_,  "0 00010000 11110000 11110000 11110000",
                              0,   32,  -1,   0,   32,    32   },
{   L_,  "00 00010000 11110000 11110000 11110000",
                              0,   33,  -1,   0,   33,    33   },
{   L_,  "10010000 11110000 11110000 11110000",
                             30,   30,  29,  30,   -1,    30   },
{   L_,  "00010000 11110000 11110000 11110000",
                             30,   31,  29,  30,   31,    31   },
{   L_,  "0 00010000 11110000 11110000 11110000",
                             30,   32,  29,  30,   32,    32   },
{   L_,  "00 00010000 11110000 11110000 11110000",
                             30,   33,  29,  30,   33,    33   },
{   L_,  "10010000 11110000 11110000 11110000",
                             31,   30,  30,  30,   -1,    -1   },
{   L_,  "00010000 11110000 11110000 11110000",
                             31,   31,  30,  31,   -1,    31   },
{   L_,  "1 00010000 11110000 11110000 11110000",
                             31,   31,  30,  31,   -1,    31   },
{   L_,  "0 00010000 11110000 11110000 11110000",
                             31,   32,  30,  31,   32,    32   },
{   L_,  "1 00010000 11110000 11110000 11110000",
                             32,   31,  31,  31,   -1,    -1   },
{   L_,  "0 00010000 11110000 11110000 11110000",
                             32,   32,  31,  32,   -1,    32   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                             0,   62,   -1,   0,   62,    62   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                             0,   63,   -1,   0,   63,    63   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110001",
                             0,   63,   -1,  -1,   63,    63   },
{   L_,
"0 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                             0,   64,   -1,   0,   64,    64   },
{   L_,
"00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   63,   62,  63,   -1,    63   },
{   L_,
"10010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   62,   62,  62,   -1,    -1   },
{   L_,
"0 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   64,   62,  63,   64,    64   },
{   L_,
"1 00010000 11110000 11110000 11110000 00010000 11110000 11110000 11110000",
                            63,   63,   62,  63,   -1,    63   },
{   L_,
"111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"11 111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                             0,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                            63,   -1,   -1,  -1,   -1,    -1   },
{   L_,
"1 111111111 11111111 11111111 11111111 11111111 11111111 11111111 111111111",
                            63,   -1,   -1,  -1,   -1,    -1   },
{ L_,
                                                                    "1111"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11111111"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11111111"
"11111111 11111111 11111111 11111111  11111111 11111111 11111111 11101111",
                      3*64 + 3,    4,    4,   4,   -1,    -1   },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE   = DATA[i].d_line;
            const char   *STR    = DATA[i].d_array_p;
            const size_t  LEN    = numBits(STR);
            const size_t  IDX    = DATA[i].d_index;
            const size_t  EXP_L  = DATA[i].d_largestIdx;
            const size_t  EXP_GT = DATA[i].d_greaterThanIdx;
            const size_t  EXP_GE = DATA[i].d_greaterThanEqualIdx;
            const size_t  EXP_LT = DATA[i].d_lessThanIdx;
            const size_t  EXP_LE = DATA[i].d_lessThanEqualIdx;

            const int MAX_ARRAY_SIZE = 4;

            if (veryVerbose) {
                P_(LINE) P_(STR) P_(IDX) P_(EXP_L) P_(EXP_LT) P_(EXP_LE)
                P_(EXP_GT) P_(EXP_GE)
            }

            uint64_t array[MAX_ARRAY_SIZE];
            bsl::fill(array + 0, array + MAX_ARRAY_SIZE, ~0ULL);
            populateBitString(array, 0, STR);

            if (veryVerbose) {
                Util::print(cout, array, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            const size_t L  = Util::find0AtMaxIndex(  array, LEN);
            LOOP3_ASSERT(LINE, L, EXP_L, L == EXP_L);
            ASSERT(EXP_L  == findAtMaxOracle(array, 0, LEN, false));

            const size_t GT = Util::find0AtMaxIndex(array,   IDX + 1, LEN);
            LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
            ASSERT(EXP_GT == findAtMaxOracle(array, IDX+1, LEN, false));

            const size_t GE = Util::find0AtMaxIndex(array,   IDX,     LEN);
            LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
            ASSERT(EXP_GE == findAtMaxOracle(array, IDX, LEN, false));

            const size_t LT = Util::find0AtMaxIndex(array, IDX);
            LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
            ASSERT(EXP_LT == findAtMaxOracle(array, 0, IDX, false));

            const size_t LE = Util::find0AtMaxIndex(array, IDX + 1);
            LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
            ASSERT(EXP_LE == findAtMaxOracle(array, 0, IDX + 1, false));
        }

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t bits[SET_UP_ARRAY_DIM] = { 0 }, control[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }

            for (size_t length = 0; length <= NUM_BITS; ++length) {
                {
                    const size_t EXP = findAtMaxOracle(bits, 0, length, false);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny0(bits, 0, length));
                    }
                    else {
                        ASSERT(EXP < length);
                        ASSERT(! Util::bit(bits, EXP));
                        ASSERT(! Util::isAny0(bits, EXP + 1, length-EXP-1));
                    }

                    ASSERT(EXP == Util::find0AtMaxIndex(bits, length));
                }

                for (size_t idx = 0; idx <= length; ++idx) {
                    const size_t EXP =
                                     findAtMaxOracle(bits, idx, length, false);
                    if (k_INVALID_INDEX == EXP) {
                        ASSERT(! Util::isAny0(bits, idx, length - idx));
                    }
                    else {
                        ASSERT(idx <= EXP && EXP < length);
                        ASSERT(! Util::bit(bits, EXP));
                        ASSERT(! Util::isAny0(bits, EXP + 1, length-EXP-1));
                    }

                    ASSERT(EXP == Util::find0AtMaxIndex(bits, idx, length));
                }

            }

            ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::find0AtMaxIndex(bits, 0));
            ASSERT_PASS(Util::find0AtMaxIndex(bits, 100));
            ASSERT_FAIL(Util::find0AtMaxIndex(   0, 0));

            ASSERT_PASS(Util::find0AtMaxIndex(bits, 0, 0));
            ASSERT_PASS(Util::find0AtMaxIndex(bits, 0, 100));
            ASSERT_PASS(Util::find0AtMaxIndex(bits, 10, 100));
            ASSERT_FAIL(Util::find0AtMaxIndex(   0, 0, 0));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'xorEqual'
        //   Ensure the method has the right effect on 'dstBitString'.
        //
        // Concerns:
        //: 1 That 'xorEqual' correctly XORs the bits from the range in
        //:   'srcBitString' to the range in 'dstBitString'.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing of both 'xorEqual' and 'xorOracle'.
        //:
        //: 2 Do exhaustive testing with 'dst' and 'src' arrays set up by
        //:   'setUpArray'.
        //:   o Verify that 'xor'ing 'dst' with a toggle of itself results in
        //:     all 1's.
        //:
        //:   o Verify that 'xor'ing 'dst' with itself results in all 0's.
        //:
        //:   o Loop through a variety of values of 'dstIndex', 'srcIndex', and
        //:     'numBits'.
        //:     1 Perform numerous 'xorEqual' operations on the 'src' and 'dst'
        //:       arrays with ALL_TRUE and 'ALL_FALSE' arrays, then test the
        //:       results, which should be predictable.
        //:
        //:     2 Perform 'xorOracle' on the 'src' and 'dst' arrays, storing
        //:       the result in 'result'.
        //:
        //:     3 Perform 'xorEqual' on 'src' and 'dst', and compare 'result'
        //:       with 'dst'.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void xorEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'xorEqual'"
                          << "\n==================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_dstBitString_p;
            int         d_dstIndex;
            const char *d_srcBitString_p;
            int         d_srcIndex;
            int         d_numBits;
            const char *d_result_p;
        } DATA [] = {
//<<------<<
// Line dst  dIdx  src  sIdx  NB   Result
// ---- ---  ----  ---- ----  --   ------
{  L_,  "",     0, "",     0,  0,     ""    },
{  L_,  "0",    0, "0",    0,  1,     "0"   },
{  L_,  "0",    0, "1",    0,  1,     "1"   },
{  L_,  "1",    0, "0",    0,  1,     "1"   },
{  L_,  "1",    0, "1",    0,  1,     "0"   },
{  L_,  "00",   4, "0",    0,  1,     "00"  },
{  L_,  "10",   4, "0",    0,  1,     "10"  },
{  L_,  "00",   4, "1",    0,  1,     "10"  },
{  L_,  "11",   4, "1",    0,  1,     "01"  },
{  L_,  "10",   4, "01",   4,  1,     "10"  },
{  L_,  "00",   4, "11",   4,  1,     "10"  },
{  L_,  "00",   4, "01",   4,  1,     "00"  },
{  L_,  "00",   4, "11",   4,  1,     "10"  },
{  L_,  "11",   4, "11",   4,  1,     "01"  },
{  L_,  "11",   0, "01",   0,  5,     "10"  },
{  L_,  "11",   0, "00",   0,  5,     "11"  },
{  L_,  "11",   0, "10",   0,  5,     "01"  },
{  L_,  "10",   0, "00",   0,  5,     "10"  },
{  L_,  "01",   0, "11",   0,  5,     "10"  },
{  L_,  "10",   0, "10",   0,  5,     "00"  },
{  L_,  "10",   0, "11",   0,  5,     "01"  },
{  L_,  "11",   0, "11",   0,  5,     "00"  },

{ L_, "1234",             0, "1234",            0, 16, "0000" },
{ L_, "1234 ww0",       128, "5670 wh0",       96, 16, "4444 ww0", },
{ L_, "1234 whqy0",     120, "5670 wwh0",     160, 16, "4444 whqy0" },

{ L_, "12341234 yf",      8, "12341234 f",      4, 32, "00000000 yf" },
{ L_, "12341234 ww0",   128, "56705670 wh0",   96, 32, "44444444 ww0", },
{ L_, "12341234 whqy0", 120, "56705670 wwh0", 160, 32, "44444444 whqy0" },
//>>------>>
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char *DSTR      = DATA[i].d_dstBitString_p;
            const int   DI        = DATA[i].d_dstIndex;
            const char *SSTR      = DATA[i].d_srcBitString_p;
            const int   SI        = DATA[i].d_srcIndex;
            const int   NB        = DATA[i].d_numBits;
            const char *RESULT    = DATA[i].d_result_p;

            if (veryVerbose) {
                P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
            }

            const int MAX_ARRAY_SIZE = 4;

            uint64_t dstControl[MAX_ARRAY_SIZE] = { 0 };
            uint64_t srcControl[MAX_ARRAY_SIZE] = { 0 };
            uint64_t result[    MAX_ARRAY_SIZE] = { 0 };
            uint64_t dst[MAX_ARRAY_SIZE], src[MAX_ARRAY_SIZE];
            populateBitStringHex(dstControl, 0, DSTR);
            populateBitStringHex(srcControl, 0, SSTR);
            populateBitStringHex(result,     0, RESULT);

            if (veryVerbose) {
                Util::print(cout, dst, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
                Util::print(cout, src, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
                Util::print(cout, result, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            wordCpy(dst, dstControl, sizeof(dst));
            wordCpy(src, srcControl, sizeof(src));

            Util::xorEqual(dst, DI, src, SI, NB);
            LOOP2_ASSERT(LINE, pHex(dst, DI + NB),
                                      Util::areEqual(dst, DI, result, DI, NB));
            ASSERT(! wordCmp(src, srcControl, sizeof(src)));

            wordCpy(dst, dstControl, sizeof(dst));

            xorOracle(dst, DI, src, SI, NB);
            LOOP_ASSERT(LINE, Util::areEqual(dst, DI, result, DI, NB));
            ASSERT(! wordCmp(src, srcControl, sizeof(src)));
        }

        const int NUM_BITS = 3 * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM];

        uint64_t *endTrue  = ALL_TRUE  + SET_UP_ARRAY_DIM;
        uint64_t *endFalse = ALL_FALSE + SET_UP_ARRAY_DIM;

        bsl::fill(ALL_TRUE  + 0, endTrue,  ~0ULL);
        bsl::fill(ALL_FALSE + 0, endFalse,  0ULL);

        uint64_t controlDst[SET_UP_ARRAY_DIM], dst[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t toggleDst[ SET_UP_ARRAY_DIM];
        uint64_t controlSrc[SET_UP_ARRAY_DIM], src[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t toggleSrc[ SET_UP_ARRAY_DIM];
        uint64_t result[    SET_UP_ARRAY_DIM];

        for (int ii = 0, jj = 36; ii < 72; ) {
            setUpArray(controlDst, &ii, true);
            setUpArray(controlSrc, &jj, true);
            jj %= 72;

            if (veryVerbose) {
                P_(ii);    P(pHex(controlDst, NUM_BITS));
                P_(jj);    P(pHex(controlSrc, NUM_BITS));
            }

            wordCpy(toggleDst, controlDst, sizeof(dst));
            Util::toggle(toggleDst, 0, SET_UP_ARRAY_DIM * k_BITS_PER_UINT64);
            wordCpy(toggleSrc, controlSrc, sizeof(src));
            Util::toggle(toggleSrc, 0, SET_UP_ARRAY_DIM * k_BITS_PER_UINT64);

            wordCpy(dst, controlDst, sizeof(dst));
            Util::xorEqual(dst, 0, toggleDst, 0, NUM_BITS);
            ASSERT(! Util::isAny0(dst, 0, NUM_BITS));

            wordCpy(dst, controlDst, sizeof(dst));
            Util::xorEqual(dst, 0, dst, 0, NUM_BITS);
            ASSERT(! Util::isAny1(dst, 0, NUM_BITS));

            wordCpy(src, controlSrc, sizeof(src));

            const int maxIdx = NUM_BITS - 1;
            for (int dstIdx = 0; dstIdx <= maxIdx; incInt(&dstIdx, maxIdx)) {
                for (int srcIdx = 0; srcIdx <= maxIdx;
                                                     incInt(&srcIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(srcIdx, dstIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        const int dstTop = dstIdx + numBits;

                        // dst | ALL_FALSE

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::xorEqual(dst, dstIdx, ALL_FALSE, srcIdx,numBits);
                        ASSERT(0 == wordCmp(dst, controlDst, sizeof(dst)));

                        // dst | ALL_TRUE

                        Util::xorEqual(dst, dstIdx, ALL_TRUE, srcIdx, numBits);
                        ASSERT(Util::areEqual(dst, 0, controlDst, 0, dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, toggleDst, dstIdx,
                                                                     numBits));
                        ASSERT(Util::areEqual(dst, dstTop, controlDst, dstTop,
                                                           NUM_BITS - dstTop));

                        // (dst = ALL_FALSE) | src

                        bsl::fill(dst + 0, dst + SET_UP_ARRAY_DIM, 0ULL);
                        Util::xorEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny1(dst, 0, dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, src, srcIdx,
                                                                     numBits));
                        ASSERT(! Util::isAny1(dst, dstTop, NUM_BITS - dstTop));

                        // (dst = ALL_TRUE) | src

                        bsl::fill(dst + 0,    dst    + SET_UP_ARRAY_DIM,~0ULL);
                        Util::xorEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny0(dst, 0, dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, toggleSrc, srcIdx,
                                                                     numBits));
                        ASSERT(! Util::isAny0(dst, dstTop, NUM_BITS - dstTop));

                        // result xorOracle src

                        wordCpy(result, controlDst, sizeof(dst));
                        xorOracle(result, dstIdx, src, srcIdx, numBits);

                        // dst | src

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::xorEqual(dst, dstIdx, src, srcIdx, numBits);

                        // confirm result == dst

                        ASSERT(0 == wordCmp(result, dst, sizeof(dst)));

                        // confirm src never changed

                        ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::xorEqual(dst,  0, src,  0,  0));
            ASSERT_PASS(Util::xorEqual(dst,  0, src,  0, 70));
            ASSERT_PASS(Util::xorEqual(dst, 70, src, 70, 70));
            ASSERT_FAIL(Util::xorEqual(  0,  0, src,  0,  0));
            ASSERT_FAIL(Util::xorEqual(dst,  0,   0,  0,  0));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'orEqual'
        //   Ensure the method has the right effect on 'dstBitString'.
        //
        // Concerns:
        //: 1 That 'orEqual' correctly ORs the bits from the range in
        //:   'srcBitString' to the range in 'dstBitString'.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing of both 'orEqual' and 'orOracle'.
        //:
        //: 2 Do exhaustive testing with 'dst' and 'src' arrays set up by
        //:   'setUpArray'.  Loop through a variety of values of 'dstIndex',
        //:   'srcIndex', and 'numBits'.
        //:   o Perform numerous 'orEqual' operations on the 'src' and 'dst'
        //:     arrays with ALL_TRUE and 'ALL_FALSE' arrays, then test the
        //:     results, which should be predictable.
        //:
        //:   o Perform 'orOracle' on the 'src' and 'dst' arrays, storing the
        //:     result in 'result'.
        //:
        //:   o Perform 'orEqual' on 'src' and 'dst', and compare 'result'
        //:     with 'dst'.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void orEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'orEqual'\n"
                             "=================\n";

        const struct {
            int         d_line;
            const char *d_dstBitString_p;
            int         d_dstIndex;
            const char *d_srcBitString_p;
            int         d_srcIndex;
            int         d_numBits;
            const char *d_result_p;
        } DATA [] = {
            // Line dst  dIdx  src  sIdx  NB   Result
            // ---- ---  ----  ---- ----  --   ------
            {  L_,  "",     0, "",     0,  0,     ""    },
            {  L_,  "0",    0, "0",    0,  1,     "0"   },
            {  L_,  "0",    0, "1",    0,  1,     "1"   },
            {  L_,  "1",    0, "0",    0,  1,     "1"   },
            {  L_,  "1",    0, "1",    0,  1,     "1"   },
            {  L_,  "01",   4, "0",    0,  1,     "01"  },
            {  L_,  "11",   4, "0",    0,  1,     "11"  },
            {  L_,  "00",   4, "1",    0,  1,     "10"  },
            {  L_,  "10",   4, "1",    0,  1,     "10"  },
            {  L_,  "10",   4, "01",   4,  1,     "10"  },
            {  L_,  "01",   4, "11",   4,  1,     "11"  },
            {  L_,  "10",   4, "01",   4,  1,     "10"  },
            {  L_,  "00",   4, "11",   4,  1,     "10"  },
            {  L_,  "11",   4, "11",   4,  1,     "11"  },
            {  L_,  "11",   0, "01",   0,  8,     "11"  },
            {  L_,  "11",   0, "00",   0,  8,     "11"  },
            {  L_,  "11",   0, "10",   0,  8,     "11"  },
            {  L_,  "10",   0, "00",   0,  8,     "10"  },
            {  L_,  "01",   0, "11",   0,  8,     "11"  },
            {  L_,  "10",   0, "10",   0,  8,     "10"  },
            {  L_,  "10",   0, "11",   0,  8,     "11"  },
            {  L_,  "11",   0, "11",   0,  8,     "11"  },
//<<-------<<
{ L_, "53cc5d03 1234 5e42f4 hw0", 120, "8888", 0, 16,
                                                  "53cc5d03 9abc 5e42f4 hw0" },
{ L_, "53cc5d03 1234 5e42f4 hw0", 120, "4444", 0, 16,
                                                  "53cc5d03 5674 5e42f4 hw0" },

{ L_, "53cc5d03 8888 5e42f4 hw0", 120, "1234", 0, 16,
                                                  "53cc5d03 9abc 5e42f4 hw0" },
{ L_, "53cc5d03 4444 5e42f4 hw0", 120, "1234", 0, 16,
                                                  "53cc5d03 5674 5e42f4 hw0" },

{ L_, "53cc5d03 1234 5e42f4 hw0", 120, "ea5c6 8888 hqyb", 56, 16,
                                                  "53cc5d03 9abc 5e42f4 hw0" },
{ L_, "53cc5d03 1234 5e42f4 hw0", 120, "ea5c6 4444 hqyb", 56, 16,
                                                  "53cc5d03 5674 5e42f4 hw0" },

{ L_, "53cc5d03 8888 5e42f4 hw0", 120, "ea5c6 1234 hqyb", 56, 16,
                                                  "53cc5d03 9abc 5e42f4 hw0" },
{ L_, "53cc5d03 4444 5e42f4 hw0", 120, "ea5c6 1234 hqyb", 56, 16,
                                                  "53cc5d03 5674 5e42f4 hw0" },

//>>-------<<
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const int MAX_ARRAY_SIZE = 4;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char *DSTR      = DATA[i].d_dstBitString_p;
            const int   DI        = DATA[i].d_dstIndex;
            const char *SSTR      = DATA[i].d_srcBitString_p;
            const int   SI        = DATA[i].d_srcIndex;
            const int   NB        = DATA[i].d_numBits;
            const char *RESULT    = DATA[i].d_result_p;

            if (veryVerbose) {
                P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
            }

            uint64_t controlDst[MAX_ARRAY_SIZE] = { 0 };
            uint64_t dst[       MAX_ARRAY_SIZE] = { 0 };
            uint64_t src[       MAX_ARRAY_SIZE] = { 0 };
            uint64_t result[    MAX_ARRAY_SIZE] = { 0 };
            populateBitStringHex(controlDst, 0, DSTR);
            populateBitStringHex(src,        0, SSTR);
            populateBitStringHex(result,     0, RESULT);

            if (veryVerbose) {
                Util::print(cout, dst, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
                Util::print(cout, src, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
                Util::print(cout, result, MAX_ARRAY_SIZE * k_BITS_PER_UINT64);
            }

            wordCpy(dst, controlDst, sizeof(dst));

            Util::orEqual(dst, DI, src, SI, NB);
            LOOP_ASSERT(LINE, Util::areEqual(dst, DI, result, DI, NB));

            wordCpy(dst, controlDst, sizeof(dst));

            orOracle(dst, DI, src, SI, NB);
            LOOP_ASSERT(LINE, Util::areEqual(dst, DI, result, DI, NB));
        }

        const int NUM_BITS = 3 * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM];

        uint64_t *endTrue  = ALL_TRUE  + SET_UP_ARRAY_DIM;
        uint64_t *endFalse = ALL_FALSE + SET_UP_ARRAY_DIM;

        bsl::fill(ALL_TRUE  + 0, endTrue,  ~0ULL);
        bsl::fill(ALL_FALSE + 0, endFalse,  0ULL);

        uint64_t controlDst[SET_UP_ARRAY_DIM], dst[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t controlSrc[SET_UP_ARRAY_DIM], src[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t result[    SET_UP_ARRAY_DIM];

        for (int ii = 0, jj = 36; ii < 80; ) {
            setUpArray(controlDst, &ii, true);
            setUpArray(controlSrc, &jj, true);
            jj %= 80;

            if (veryVerbose) {
                P_(ii);    P(pHex(controlDst, NUM_BITS));
                P_(jj);    P(pHex(controlSrc, NUM_BITS));
            }

            const int maxIdx = NUM_BITS - 1;
            for (int dstIdx = 0; dstIdx <= maxIdx; incInt(&dstIdx, maxIdx)) {
                for (int srcIdx = 0; srcIdx <= maxIdx;
                                                     incInt(&srcIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(srcIdx, dstIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        const int dstTop = dstIdx + numBits;

                        // dst | ALL_FALSE

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::orEqual(dst, dstIdx, ALL_FALSE, srcIdx, numBits);
                        ASSERT(0 == wordCmp(dst, controlDst, sizeof(dst)));

                        // dst | ALL_TRUE

                        Util::orEqual(dst, dstIdx, ALL_TRUE, srcIdx, numBits);
                        ASSERT(Util::areEqual(dst, 0, controlDst, 0, dstIdx));
                        ASSERT(! Util::isAny0(dst, dstIdx, numBits));
                        ASSERT(Util::areEqual(dst, dstTop, controlDst, dstTop,
                                                           NUM_BITS - dstTop));

                        // (dst = ALL_FALSE) | src

                        bsl::fill(dst + 0, dst + SET_UP_ARRAY_DIM, 0ULL);
                        wordCpy(src, controlSrc, sizeof(src));
                        Util::orEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny1(dst, 0, dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, src, srcIdx,
                                                                     numBits));
                        ASSERT(! Util::isAny1(dst, dstTop, NUM_BITS - dstTop));

                        // (dst = ALL_TRUE) | src

                        bsl::fill(dst + 0,    dst    + SET_UP_ARRAY_DIM,~0ULL);
                        Util::orEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny0(dst, 0, NUM_BITS));

                        // result orOracle src

                        wordCpy(result, controlDst, sizeof(dst));
                        orOracle(result, dstIdx, src, srcIdx, numBits);

                        // dst | src

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::orEqual(dst, dstIdx, src, srcIdx, numBits);

                        // confirm result == dst

                        ASSERT(0 == wordCmp(result, dst, sizeof(dst)));

                        // confirm src never changed

                        ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::orEqual(dst,  0, src,  0,  0));
            ASSERT_PASS(Util::orEqual(dst,  0, src,  0, 70));
            ASSERT_PASS(Util::orEqual(dst, 70, src, 70, 70));
            ASSERT_FAIL(Util::orEqual(  0,  0, src,  0,  0));
            ASSERT_FAIL(Util::orEqual(dst,  0,   0,  0,  0));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'minusEqual'
        //   Ensure the method has the right effect on 'dstBitString'.
        //
        // Concerns:
        //: 1 That 'minusEqual' correctly clears the bits from the range in
        //:   'dstBitString' corresponding to the set bits in the range in
        //:   'srcBitString'.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing of both 'minusEqual' and 'minusOracle'.
        //:
        //: 2 Do exhaustive testing with 'dst' and 'src' arrays set up by
        //:   'setUpArray'.  Loop through a variety of values of 'dstIndex',
        //:   'srcIndex', and 'numBits'.
        //:   o Perform numerous 'minusEqual' operations on the 'src' and 'dst'
        //:     arrays with ALL_TRUE and 'ALL_FALSE' arrays, then test the
        //:     results, which should be predictable.
        //:
        //:   o Perform 'minusOracle' on the 'src' and 'dst' arrays, storing
        //:     the result in 'result'.
        //:
        //:   o Perform 'minusEqual' on 'src' and 'dst', and compare 'result'
        //:     with 'dst'.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void minusEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'minusEqual'\n"
                             "====================\n";

        const struct {
            int         d_line;
            const char *d_dstBitString_p;
            int         d_dstIndex;
            const char *d_srcBitString_p;
            int         d_srcIndex;
            int         d_numBits;
            const char *d_result_p;
        } DATA [] = {
// Line dst  dIdx  src  sIdx  NB   Result
// ---- ---  ----  ---- ----  --   ------
{ L_,  "",     0, "",     0,  0,     ""    },
{ L_,  "0",    0, "0",    0,  1,     "0"   },
{ L_,  "0",    0, "1",    0,  1,     "0"   },
{ L_,  "1",    0, "0",    0,  1,     "1"   },
{ L_,  "1",    0, "1",    0,  1,     "0"   },
{ L_,  "00",   4, "0",    0,  1,     "00"  },
{ L_,  "10",   4, "0",    0,  1,     "10"  },
{ L_,  "00",   4, "1",    0,  1,     "00"  },
{ L_,  "10",   4, "1",    0,  1,     "00"  },
{ L_,  "11",   4, "01",   4,  4,     "11"  },
{ L_,  "01",   4, "11",   4,  4,     "01"  },
{ L_,  "11",   4, "01",   4,  4,     "11"  },
{ L_,  "01",   4, "11",   4,  4,     "01"  },
{ L_,  "11",   4, "11",   4,  4,     "01"  },
{ L_,  "11",   0, "01",   0,  8,     "10"  },
{ L_,  "11",   0, "00",   0,  8,     "11"  },
{ L_,  "11",   0, "10",   0,  8,     "01"  },
{ L_,  "10",   0, "00",   0,  8,     "10"  },
{ L_,  "01",   0, "11",   0,  8,     "00"  },
{ L_,  "10",   0, "10",   0,  8,     "00"  },
{ L_,  "10",   0, "11",   0,  8,     "00"  },
{ L_,  "11",   0, "11",   0,  8,     "00"  },


{ L_, "77901f ffff 611a y0", 24, "0000", 0, 16, "77901f ffff 611a y0" },
{ L_, "77901f ffff 611a y0", 24, "aaaa", 0, 16, "77901f 5555 611a y0" },
{ L_, "77901f ffff 611a y0", 24, "7777", 0, 16, "77901f 8888 611a y0" },
{ L_, "77901f ffff 611a y0", 24, "137f", 0, 16, "77901f ec80 611a y0" },
{ L_, "77901f ffff 611a y0", 24, "f731", 0, 16, "77901f 08ce 611a y0" },

{ L_, "77901f 0000 611a y0", 24, "48c0", 0, 16, "77901f 0000 611a y0" },
{ L_, "77901f 0000 611a y0", 24, "26ae", 0, 16, "77901f 0000 611a y0" },
{ L_, "77901f 0000 611a y0", 24, "159d", 0, 16, "77901f 0000 611a y0" },

{ L_, "77901f 1234 611a y0", 24, "0000", 0, 16, "77901f 1234 611a y0" },
{ L_, "77901f 1234 611a y0", 24, "1111", 0, 16, "77901f 0224 611a y0" },
{ L_, "77901f 1234 611a y0", 24, "2222", 0, 16, "77901f 1014 611a y0" },

{ L_, "77901f 8ace 611a y0", 24, "0000", 0, 16, "77901f 8ace 611a y0" },
{ L_, "77901f 8ace 611a y0", 24, "1111", 0, 16, "77901f 8ace 611a y0" },
{ L_, "77901f 8ace 611a y0", 24, "5555", 0, 16, "77901f 8a8a 611a y0" },
{ L_, "77901f 8ace 611a y0", 24, "7777", 0, 16, "77901f 8888 611a y0" },


{ L_, "77901f ffff 611a y0", 24, "a 0000 611a", 16, 16, "77901f ffff 611ay0" },
{ L_, "77901f ffff 611a y0", 24, "a aaaa 611a", 16, 16, "77901f 5555 611ay0" },
{ L_, "77901f ffff 611a y0", 24, "a 7777 611a", 16, 16, "77901f 8888 611ay0" },
{ L_, "77901f ffff 611a y0", 24, "a 137f 611a", 16, 16, "77901f ec80 611ay0" },
{ L_, "77901f ffff 611a y0", 24, "a f731 611a", 16, 16, "77901f 08ce 611ay0" },

{ L_, "77901f 0000 611a y0", 24, "a 48c0 611a", 16, 16, "77901f 0000 611ay0" },
{ L_, "77901f 0000 611a y0", 24, "a 26ae 611a", 16, 16, "77901f 0000 611ay0" },
{ L_, "77901f 0000 611a y0", 24, "a 159d 611a", 16, 16, "77901f 0000 611ay0" },

{ L_, "77901f 1234 611a y0", 24, "a 0000 611a", 16, 16, "77901f 1234 611ay0" },
{ L_, "77901f 1234 611a y0", 24, "a 1111 611a", 16, 16, "77901f 0224 611ay0" },
{ L_, "77901f 1234 611a y0", 24, "a 2222 611a", 16, 16, "77901f 1014 611ay0" },

{ L_, "77901f 8ace 611a y0", 24, "a 0000 611a", 16, 16, "77901f 8ace 611ay0" },
{ L_, "77901f 8ace 611a y0", 24, "a 1111 611a", 16, 16, "77901f 8ace 611ay0" },
{ L_, "77901f 8ace 611a y0", 24, "a 5555 611a", 16, 16, "77901f 8a8a 611ay0" },
{ L_, "77901f 8ace 611a y0", 24, "a 7777 611a", 16, 16, "77901f 8888 611ay0" },


{ L_, "01f ffff 611a y0", 24, "a 0000 611a wa", 80, 16, "01f ffff 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a aaaa 611a wa", 80, 16, "01f 5555 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a 7777 611a wa", 80, 16, "01f 8888 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a 137f 611a wa", 80, 16, "01f ec80 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a f731 611a wa", 80, 16, "01f 08ce 611ay0" },

{ L_, "01f 0000 611a y0", 24, "a 48c0 611a wa", 80, 16, "01f 0000 611ay0" },
{ L_, "01f 0000 611a y0", 24, "a 26ae 611a wa", 80, 16, "01f 0000 611ay0" },
{ L_, "01f 0000 611a y0", 24, "a 159d 611a wa", 80, 16, "01f 0000 611ay0" },

{ L_, "01f 1234 611a y0", 24, "a 0000 611a wa", 80, 16, "01f 1234 611ay0" },
{ L_, "01f 1234 611a y0", 24, "a 1111 611a wa", 80, 16, "01f 0224 611ay0" },
{ L_, "01f 1234 611a y0", 24, "a 2222 611a wa", 80, 16, "01f 1014 611ay0" },

{ L_, "01f 8ace 611a y0", 24, "a 0000 611a wa", 80, 16, "01f 8ace 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 1111 611a wa", 80, 16, "01f 8ace 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 5555 611a wa", 80, 16, "01f 8a8a 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 7777 611a wa", 80, 16, "01f 8888 611ay0" },


{ L_, "01f ffff 611a y0", 24, "a 0000 611a yhwa", 120, 16, "01f ffff 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a aaaa 611a yhwa", 120, 16, "01f 5555 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a 7777 611a yhwa", 120, 16, "01f 8888 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a 137f 611a yhwa", 120, 16, "01f ec80 611ay0" },
{ L_, "01f ffff 611a y0", 24, "a f731 611a yhwa", 120, 16, "01f 08ce 611ay0" },

{ L_, "01f 0000 611a y0", 24, "a 48c0 611a yhwa", 120, 16, "01f 0000 611ay0" },
{ L_, "01f 0000 611a y0", 24, "a 26ae 611a yhwa", 120, 16, "01f 0000 611ay0" },
{ L_, "01f 0000 611a y0", 24, "a 159d 611a yhwa", 120, 16, "01f 0000 611ay0" },

{ L_, "01f 1234 611a y0", 24, "a 0000 611a yhwa", 120, 16, "01f 1234 611ay0" },
{ L_, "01f 1234 611a y0", 24, "a 1111 611a yhwa", 120, 16, "01f 0224 611ay0" },
{ L_, "01f 1234 611a y0", 24, "a 2222 611a yhwa", 120, 16, "01f 1014 611ay0" },

{ L_, "01f 8ace 611a y0", 24, "a 0000 611a yhwa", 120, 16, "01f 8ace 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 1111 611a yhwa", 120, 16, "01f 8ace 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 5555 611a yhwa", 120, 16, "01f 8a8a 611ay0" },
{ L_, "01f 8ace 611a y0", 24, "a 7777 611a yhwa", 120, 16, "01f 8888 611ay0" },


{ L_, "77901f ffff 611a yhww0", 184, "0000", 0, 16, "77901f ffff 611a yhww0" },
{ L_, "77901f ffff 611a yhww0", 184, "aaaa", 0, 16, "77901f 5555 611a yhww0" },
{ L_, "77901f ffff 611a yhww0", 184, "7777", 0, 16, "77901f 8888 611a yhww0" },
{ L_, "77901f ffff 611a yhww0", 184, "137f", 0, 16, "77901f ec80 611a yhww0" },
{ L_, "77901f ffff 611a yhww0", 184, "f731", 0, 16, "77901f 08ce 611a yhww0" },

{ L_, "77901f 0000 611a yhww0", 184, "48c0", 0, 16, "77901f 0000 611a yhww0" },
{ L_, "77901f 0000 611a yhww0", 184, "26ae", 0, 16, "77901f 0000 611a yhww0" },
{ L_, "77901f 0000 611a yhww0", 184, "159d", 0, 16, "77901f 0000 611a yhww0" },

{ L_, "77901f 1234 611a yhww0", 184, "0000", 0, 16, "77901f 1234 611a yhww0" },
{ L_, "77901f 1234 611a yhww0", 184, "1111", 0, 16, "77901f 0224 611a yhww0" },
{ L_, "77901f 1234 611a yhww0", 184, "2222", 0, 16, "77901f 1014 611a yhww0" },

{ L_, "77901f 8ace 611a yhww0", 184, "0000", 0, 16, "77901f 8ace 611a yhww0" },
{ L_, "77901f 8ace 611a yhww0", 184, "1111", 0, 16, "77901f 8ace 611a yhww0" },
{ L_, "77901f 8ace 611a yhww0", 184, "5555", 0, 16, "77901f 8a8a 611a yhww0" },
{ L_, "77901f 8ace 611a yhww0", 184, "7777", 0, 16, "77901f 8888 611a yhww0" },
};
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char *DSTR      = DATA[i].d_dstBitString_p;
            const int   DI        = DATA[i].d_dstIndex;
            const char *SSTR      = DATA[i].d_srcBitString_p;
            const int   SI        = DATA[i].d_srcIndex;
            const int   NB        = DATA[i].d_numBits;
            const char *RESULT    = DATA[i].d_result_p;

            if (veryVerbose) {
                P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
            }

            uint64_t controlDst[SET_UP_ARRAY_DIM] = {0};
            uint64_t dst[       SET_UP_ARRAY_DIM];
            uint64_t controlSrc[SET_UP_ARRAY_DIM] = {0};
            uint64_t src[       SET_UP_ARRAY_DIM];
            uint64_t result[    SET_UP_ARRAY_DIM] = {0};

            populateBitStringHex(controlDst, 0, DSTR);
            wordCpy(dst, controlDst, sizeof(dst));
            populateBitStringHex(controlSrc, 0, SSTR);
            wordCpy(src, controlSrc, sizeof(src));
            populateBitStringHex(result,     0, RESULT);

            if (veryVerbose) {
                const int maxNumBits = bsl::max(DI, SI) + NB;

                P(LINE);
                Util::print(cout, dst,    maxNumBits);
                Util::print(cout, src,    maxNumBits);
                Util::print(cout, result, maxNumBits);
            }

            const int NUM_DIM = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

            minusOracle(dst, DI, src, SI, NB);
            LOOP3_ASSERT(LINE, pHex(dst, NUM_DIM), pHex(result, NUM_DIM),
                                       0 == wordCmp(dst, result, sizeof(dst)));
            ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));

            wordCpy(dst, controlDst, sizeof(dst));

            Util::minusEqual(dst, DI, src, SI, NB);
            LOOP_ASSERT(LINE, 0 == wordCmp(dst, result, sizeof(dst)));
            ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
        }

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM];

        uint64_t *endTrue  = ALL_TRUE  + SET_UP_ARRAY_DIM;
        uint64_t *endFalse = ALL_FALSE + SET_UP_ARRAY_DIM;

        bsl::fill(ALL_TRUE  + 0, endTrue,  ~0ULL);
        bsl::fill(ALL_FALSE + 0, endFalse,  0ULL);

        uint64_t controlDst[SET_UP_ARRAY_DIM], dst[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t controlSrc[SET_UP_ARRAY_DIM], src[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t result[    SET_UP_ARRAY_DIM];

        for (int ii = 0, jj = 36; ii < 80; ) {
            setUpArray(controlDst, &ii, true);
            setUpArray(controlSrc, &jj, true);
            jj %= 80;

            if (veryVerbose) {
                P_(ii);    P(pHex(controlDst, NUM_BITS));
                P_(jj);    P(pHex(controlSrc, NUM_BITS));
            }

            const int maxIdx = NUM_BITS - 1;
            for (int dstIdx = 0; dstIdx <= maxIdx; incInt(&dstIdx, maxIdx)) {
                for (int srcIdx = 0; srcIdx <= maxIdx;
                                                     incInt(&srcIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(srcIdx, dstIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        const int dstTop = dstIdx + numBits;

                        // dst - ALL_FALSE

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::minusEqual(dst, dstIdx, ALL_FALSE, srcIdx,
                                                                      numBits);
                        ASSERT(0 == wordCmp(dst, controlDst, sizeof(dst)));

                        // dst - ALL_TRUE

                        Util::minusEqual(dst, dstIdx, ALL_TRUE, srcIdx,
                                                                      numBits);
                        ASSERT(Util::areEqual(dst, 0, controlDst, 0, dstIdx))
                        ASSERT(! Util::isAny1(dst, dstIdx, numBits));
                        ASSERT(Util::areEqual(dst, dstTop, controlDst, dstTop,
                                                           NUM_BITS - dstTop));

                        // (dst = ALL_FALSE) - src

                        bsl::fill(dst + 0, dst + SET_UP_ARRAY_DIM, 0ULL);
                        wordCpy(src, controlSrc, sizeof(src));
                        Util::minusEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny1(dst, 0, NUM_BITS));

                        // (dst = ALL_TRUE) - src

                        bsl::fill(result + 0, result + SET_UP_ARRAY_DIM,~0ULL);
                        minusOracle(result, dstIdx, src, srcIdx, numBits);

                        bsl::fill(dst + 0,    dst    + SET_UP_ARRAY_DIM,~0ULL);
                        Util::minusEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! wordCmp(result, dst, sizeof(result)));
                        ASSERT(! Util::isAny0(dst, 0, dstIdx));
                        ASSERT(! Util::isAny0(dst, dstTop, NUM_BITS - dstTop));

                        // result = result oracleAnd src

                        wordCpy(result, controlDst, sizeof(dst));
                        minusOracle(result, dstIdx, src, srcIdx, numBits);

                        // dst = dst & src

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::minusEqual(dst, dstIdx, src, srcIdx, numBits);

                        // confirm result == dst

                        ASSERT(0 == wordCmp(result, dst, sizeof(dst)));

                        // confirm src never changed

                        ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::minusEqual(dst,  0, src,  0,  0));
            ASSERT_PASS(Util::minusEqual(dst,  0, src,  0, 70));
            ASSERT_PASS(Util::minusEqual(dst, 70, src, 70, 70));
            ASSERT_FAIL(Util::minusEqual(  0,  0, src,  0,  0));
            ASSERT_FAIL(Util::minusEqual(dst,  0,   0,  0,  0));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'andEqual'
        //   Ensure the method has the right effect on 'dstBitString'.
        //
        // Concerns:
        //: 1 That 'andEqual' correctly ANDs the bits from the range in
        //:   'srcBitString' to the range in 'dstBitString'.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing of both 'andEqual' and 'andOracle'.
        //:
        //: 2 Do exhaustive testing with 'dst' and 'src' arrays set up by
        //:   'setUpArray'.  Loop through a variety of values of 'dstIndex',
        //:   'srcIndex', and 'numBits'.
        //:   o Perform numerous 'andEqual' operations on the 'src' and 'dst'
        //:     arrays with ALL_TRUE and 'ALL_FALSE' arrays, then test the
        //:     results, which should be predictable.
        //:
        //:   o Perform 'andOracle' on the 'src' and 'dst' arrays, storing the
        //:     result in 'result'.
        //:
        //:   o Perform 'andEqual' on 'src' and 'dst', and compare 'result'
        //:     with 'dst'.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void andEqual(U64 *dBS, St dIdx, U64 *sBS, St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'andEqual'\n"
                          << "==================\n";

        static const struct {
            int         d_line;
            const char *d_dstBitString_p;
            int         d_dstIndex;
            const char *d_srcBitString_p;
            int         d_srcIndex;
            int         d_numBits;
            const char *d_result_p;
        } DATA [] = {
//<<------<<
// Line dst  dIdx  src  sIdx  NB   Result
// ---- ---  ----  ---- ----  --   ------
{ L_,    "",   0,   "",   0,  0,       ""  },


{ L_,   "0",   0,  "0",   0,  1,      "0"  },
{ L_,   "0",   0,  "1",   0,  1,      "0"  },
{ L_,   "1",   0,  "0",   0,  1,      "0"  },
{ L_,   "1",   0,  "1",   0,  1,      "1"  },

{ L_,   "0",   0,  "0",   0,  4,      "0"  },
{ L_,   "0",   0,  "f",   0,  4,      "0"  },
{ L_,   "f",   0,  "0",   0,  4,      "0"  },
{ L_,   "f",   0,  "f",   0,  4,      "f"  },

{ L_,   "0",   0,  "0",   0,  4,      "0"  },
{ L_,   "0",   0,  "a",   0,  4,      "0"  },
{ L_,   "a",   0,  "0",   0,  4,      "0"  },
{ L_,   "a",   0,  "a",   0,  4,      "a"  },

{ L_,   "f",   0,  "a",   0,  4,      "a"  },
{ L_,   "a",   0,  "f",   0,  4,      "a"  },

{ L_,   "0",   0,  "0",   0,  4,      "0"  },
{ L_,   "0",   0,  "7",   0,  4,      "0"  },
{ L_,   "7",   0,  "0",   0,  4,      "0"  },
{ L_,   "7",   0,  "7",   0,  4,      "7"  },

{ L_,   "f",   0,  "7",   0,  4,      "7"  },
{ L_,   "7",   0,  "f",   0,  1,      "7"  },


{ L_,   "01",   4,  "0",   0,  1,      "01"  },
{ L_,   "02",   4,  "1",   0,  1,      "02"  },
{ L_,   "13",   4,  "0",   0,  1,      "03"  },
{ L_,   "14",   4,  "1",   0,  1,      "14"  },

{ L_,   "05",   4,  "0",   0,  4,      "05"  },
{ L_,   "06",   4,  "f",   0,  4,      "06"  },
{ L_,   "f7",   4,  "0",   0,  4,      "07"  },
{ L_,   "f8",   4,  "f",   0,  4,      "f8"  },

{ L_,   "09",   4,  "0",   0,  4,      "09"  },
{ L_,   "0a",   4,  "a",   0,  4,      "0a"  },
{ L_,   "ab",   4,  "0",   0,  4,      "0b"  },
{ L_,   "ac",   4,  "a",   0,  4,      "ac"  },

{ L_,   "fd",   4,  "a",   0,  4,      "ad"  },
{ L_,   "af",   4,  "f",   0,  4,      "af"  },

{ L_,   "00",   4,  "0",   0,  4,      "00"  },
{ L_,   "01",   4,  "7",   0,  4,      "01"  },
{ L_,   "72",   4,  "0",   0,  4,      "02"  },
{ L_,   "73",   4,  "7",   0,  4,      "73"  },

{ L_,   "f4",   4,  "7",   0,  4,      "74"  },
{ L_,   "75",   4,  "f",   0,  1,      "75"  },


{ L_, "101 0123456789abcdef",  68,  "6e20a75",  12,  1,
                                                     "101 0123456789abcdef" },
{ L_, "202 0123456789abcdef",  68,  "6e21a75",  12,  1,
                                                     "202 0123456789abcdef" },
{ L_, "313 0123456789abcdef",  68,  "6e20a75",  12,  1,
                                                     "303 0123456789abcdef" },
{ L_, "414 0123456789abcdef",  68,  "6e21a75",  12,  1,
                                                     "414 0123456789abcdef" },

{ L_, "505 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "505 0123456789abcdef" },
{ L_, "606 0123456789abcdef",  68,  "6e2fa75",  12,  4,
                                                     "606 0123456789abcdef" },
{ L_, "7f7 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "707 0123456789abcdef" },
{ L_, "8f8 0123456789abcdef",  68,  "6e2fa75",  12,  4,
                                                     "8f8 0123456789abcdef" },

{ L_, "909 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "909 0123456789abcdef" },
{ L_, "a0a 0123456789abcdef",  68,  "6e2aa75",  12,  4,
                                                     "a0a 0123456789abcdef" },
{ L_, "bab 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "b0b 0123456789abcdef" },
{ L_, "cac 0123456789abcdef",  68,  "6e2aa75",  12,  4,
                                                     "cac 0123456789abcdef" },

{ L_, "dfd 0123456789abcdef",  68,  "6e2aa75",  12,  4,
                                                     "dad 0123456789abcdef" },
{ L_, "faf 0123456789abcdef",  68,  "6e2fa75",  12,  4,
                                                     "faf 0123456789abcdef" },

{ L_, "000 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "000 0123456789abcdef" },
{ L_, "101 0123456789abcdef",  68,  "6e27a75",  12,  4,
                                                     "101 0123456789abcdef" },
{ L_, "272 0123456789abcdef",  68,  "6e20a75",  12,  4,
                                                     "202 0123456789abcdef" },
{ L_, "373 0123456789abcdef",  68,  "6e27a75",  12,  4,
                                                     "373 0123456789abcdef" },

{ L_, "4f4 0123456789abcdef",  68,  "6e27a75",  12,  4,
                                                     "474 0123456789abcdef" },
{ L_, "575 0123456789abcdef",  68,  "6e2fa75",  12,  4,
                                                     "575 0123456789abcdef" },

{ L_, "110 0123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "110 0123456789abcdef" },
{ L_, "220 0123456789abcdef",  60,  "6e211a75",  12,  8,
                                                     "220 0123456789abcdef" },
{ L_, "331 1123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "330 0123456789abcdef" },
{ L_, "441 1123456789abcdef",  60,  "6e211a75",  12,  8,
                                                     "441 1123456789abcdef" },

{ L_, "550 0123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "550 0123456789abcdef" },
{ L_, "660 0123456789abcdef",  60,  "6e2ffa75",  12,  8,
                                                     "660 0123456789abcdef" },
{ L_, "77f f123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "770 0123456789abcdef" },
{ L_, "88f f123456789abcdef",  60,  "6e2ffa75",  12,  8,
                                                     "88f f123456789abcdef" },

{ L_, "990 0123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "990 0123456789abcdef" },
{ L_, "aa0 0123456789abcdef",  60,  "6e2aaa75",  12,  8,
                                                     "aa0 0123456789abcdef" },
{ L_, "bba a123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "bb0 0123456789abcdef" },
{ L_, "cca a123456789abcdef",  60,  "6e2aaa75",  12,  8,
                                                     "cca a123456789abcdef" },

{ L_, "ddf f123456789abcdef",  60,  "6e2aaa75",  12,  8,
                                                     "dda a123456789abcdef" },
{ L_, "ffa a123456789abcdef",  60,  "6e2ffa75",  12,  8,
                                                     "ffa a123456789abcdef" },

{ L_, "000 0123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "000 0123456789abcdef" },
{ L_, "110 0123456789abcdef",  60,  "6e277a75",  12,  8,
                                                     "110 0123456789abcdef" },
{ L_, "227 7123456789abcdef",  60,  "6e200a75",  12,  8,
                                                     "220 0123456789abcdef" },
{ L_, "337 7123456789abcdef",  60,  "6e277a75",  12,  8,
                                                     "337 7123456789abcdef" },

{ L_, "44f f123456789abcdef",  60,  "6e277a75",  12,  8,
                                                      "447 7123456789abcdef" },
{ L_, "557 7123456789abcdef",  60,  "6e2ffa75",  12,  8,
                                                      "557 7123456789abcdef" },

{ L_, "d6b5 aaaa yqhww0", 184, "6e0 ffff a75wb", 76, 16, "d6b5 aaaa yqhww0" },
{ L_, "d6b5 aaaa yqhww0", 184, "6e0 7777 a75wb", 76, 16, "d6b5 2222 yqhww0" },
{ L_, "d6b5 aaaa yqhww0", 184, "6e0 2468 a75wb", 76, 16, "d6b5 2028 yqhww0" },
{ L_, "d6b5 aaaa yqhww0", 184, "6e0 8642 a75wb", 76, 16, "d6b5 8202 yqhww0" },
{ L_, "d6b5 aaaa yqhww0", 184, "6e0 8663 a75wb", 76, 16, "d6b5 8222 yqhww0" },
//>>------>>
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char *DSTR      = DATA[i].d_dstBitString_p;
            const int   DI        = DATA[i].d_dstIndex;
            const char *SSTR      = DATA[i].d_srcBitString_p;
            const int   SI        = DATA[i].d_srcIndex;
            const int   NB        = DATA[i].d_numBits;
            const char *RESULT    = DATA[i].d_result_p;

            if (veryVerbose) {
                P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
            }

            uint64_t controlDst[SET_UP_ARRAY_DIM] = {0};
            uint64_t dst[       SET_UP_ARRAY_DIM];
            uint64_t controlSrc[SET_UP_ARRAY_DIM] = {0};
            uint64_t src[       SET_UP_ARRAY_DIM];
            uint64_t result[    SET_UP_ARRAY_DIM] = {0};

            populateBitStringHex(controlDst, 0, DSTR);
            wordCpy(dst, controlDst, sizeof(dst));
            populateBitStringHex(controlSrc, 0, SSTR);
            wordCpy(src, controlSrc, sizeof(src));
            populateBitStringHex(result,     0, RESULT);

            if (veryVerbose) {
                const int maxNumBits = bsl::max(DI, SI) + NB;

                P(LINE);
                Util::print(cout, dst,    maxNumBits);
                Util::print(cout, src,    maxNumBits);
                Util::print(cout, result, maxNumBits);
            }

            const int NUM_DIM = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

            andOracle(dst, DI, src, SI, NB);
            LOOP3_ASSERT(LINE, pHex(dst, NUM_DIM), pHex(result, NUM_DIM),
                                       0 == wordCmp(dst, result, sizeof(dst)));
            ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));

            wordCpy(dst, controlDst, sizeof(dst));

            Util::andEqual(dst, DI, src, SI, NB);
            LOOP_ASSERT(LINE, 0 == wordCmp(dst, result, sizeof(dst)));
            ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
        }

        const int NUM_BITS = 3 * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM];

        uint64_t *endTrue  = ALL_TRUE  + SET_UP_ARRAY_DIM;
        uint64_t *endFalse = ALL_FALSE + SET_UP_ARRAY_DIM;

        bsl::fill(ALL_TRUE  + 0, endTrue,  ~0ULL);
        bsl::fill(ALL_FALSE + 0, endFalse,  0ULL);

        uint64_t controlDst[SET_UP_ARRAY_DIM], dst[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t controlSrc[SET_UP_ARRAY_DIM], src[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t result[    SET_UP_ARRAY_DIM];

        for (int ii = 0, jj = 36; ii < 80; ) {
            setUpArray(controlDst, &ii, true);
            setUpArray(controlSrc, &jj, true);
            jj %= 80;

            if (veryVerbose) {
                P_(ii);    P(pHex(controlDst, NUM_BITS));
                P_(jj);    P(pHex(controlSrc, NUM_BITS));
            }

            const int maxIdx = NUM_BITS - 1;
            for (int dstIdx = 0; dstIdx <= maxIdx; incInt(&dstIdx, maxIdx)) {
                for (int srcIdx = 0; srcIdx <= maxIdx;
                                                     incInt(&srcIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(srcIdx, dstIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {

                        // dst & ALL_TRUE

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::andEqual(dst, dstIdx, ALL_TRUE, srcIdx, numBits);
                        ASSERT(0 == wordCmp(dst, controlDst, sizeof(dst)));

                        // dst & ALL_FALSE

                        Util::andEqual(dst, dstIdx, ALL_FALSE, srcIdx,numBits);
                        ASSERT(Util::areEqual(dst, 0, controlDst, 0, dstIdx))
                        ASSERT(! Util::isAny1(dst, dstIdx, numBits));
                        ASSERT(Util::areEqual(dst,        dstIdx + numBits,
                                             controlDst, dstIdx + numBits,
                                             NUM_BITS -  dstIdx - numBits));

                        // (dst = ALL_TRUE) & src

                        bsl::fill(dst + 0, dst + SET_UP_ARRAY_DIM, ~0ULL);
                        wordCpy(src, controlSrc, sizeof(src));
                        Util::andEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny0(dst, 0, dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, src, srcIdx,
                                                                     numBits));
                        ASSERT(! Util::isAny0(dst, dstIdx + numBits,
                                                 NUM_BITS - dstIdx - numBits));

                        // (dst = ALL_FALSE) & src

                        bsl::fill(dst + 0, dst + SET_UP_ARRAY_DIM, 0ULL);
                        Util::andEqual(dst, dstIdx, src, srcIdx, numBits);
                        ASSERT(! Util::isAny1(dst, 0, NUM_BITS));

                        // result = result oracleAnd src

                        wordCpy(result, controlDst, sizeof(dst));
                        andOracle(result, dstIdx, src, srcIdx, numBits);

                        // dst = dst & src

                        wordCpy(dst, controlDst, sizeof(dst));
                        Util::andEqual(dst, dstIdx, src, srcIdx, numBits);

                        // confirm result == dst

                        ASSERT(0 == wordCmp(result, dst, sizeof(dst)));

                        // confirm src never changed

                        ASSERT(0 == wordCmp(src, controlSrc, sizeof(src)));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::andEqual(dst,  0, src,  0,  0));
            ASSERT_PASS(Util::andEqual(dst,  0, src,  0, 70));
            ASSERT_PASS(Util::andEqual(dst, 70, src, 70, 70));
            ASSERT_FAIL(Util::andEqual(  0,  0, src,  0,  0));
            ASSERT_FAIL(Util::andEqual(dst,  0,   0,  0,  0));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'toggle'
        //   Ensure the method has the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That 'toggle' correctly toggle bits in the specified range.
        //:
        //: 2 That 'toggle' doesn't affect bits outside the specified range.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use table-driven code to test both 'toggle' and the simple,
        //:   inefficient 'toggleOracle'.  Also verify that toggling twice
        //:   returns the array to its initial state.
        //:
        //: 2 Loop, populating an array with 'setUpArray', then do nested
        //:   loops iterating over 'index' and 'numBits'.
        //:   o On the first iteration of 'setUpArray', do special testing of
        //:     the cases of all-true and all-false.
        //:
        //:   o Apply 'toggle' and 'toggleOracle' and verify that their results
        //:     match, and that they do not disturb bits outside the range
        //:     specified.  (C-1..2)
        //:
        //:   o Toggle back and verify the array is back to its initial state.
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-3)
        //
        // Testing:
        //   void toggle(uint64_t *bitString, St index, St numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'toggle'"
                          << "\n================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_array_p;
            int         d_index;
            int         d_numBits;
            const char *d_result_p;
        } DATA [] = {
            // Line  array                  idx  NB   Result
            // ----  -----                  ---  --   ------
            {   L_,  "",                     0,   0,     "" },
            {   L_,  "1",                    0,   0,    "1" },
            {   L_,  "1",                    0,   1,    "0" },
            {   L_,  "0",                    0,   1,    "1" },
            {   L_,  "00",                   0,   1,   "01" },
            {   L_,  "00",                   4,   1,   "10" },
            {   L_,  "10",                   4,   1,   "00" },
            {   L_,  "01",                   4,   1,   "11" },
            {   L_,  "101",                  4,   4,  "1f1" },
            {   L_,  "101",                  0,   1,  "100" },
            {   L_,  "101",                  8,   1,  "001" },
            {   L_,  "10",                   0,   5,   "0f" },
            {   L_,  "01",                   0,   8,   "fe" },
            {   L_,  "00",                   0,   8,   "ff" },
            {   L_,  "11",                   0,   8,   "ee" },
            {   L_,  "101",                  4,   8,  "ef1" },
            {   L_,  "010",                  4,   8,  "fe0" },
//<<------<<
{ L_, "6f6f1d ffff 9fdaea 8db265b3 e075ad52", 88, 16,
                                      "6f6f1d 0000 9fdaea 8db265b3 e075ad52" },
{ L_, "6f6f1d 0000 9fdaea 8db265b3 e075ad52", 88, 16,
                                      "6f6f1d ffff 9fdaea 8db265b3 e075ad52" },
{ L_, "6f6f1d aaaa 9fdaea 8db265b3 e075ad52", 88, 16,
                                      "6f6f1d 5555 9fdaea 8db265b3 e075ad52" },
{ L_, "6f6f1d 1234 9fdaea 8db265b3 e075ad52", 88, 16,
                                      "6f6f1d edcb 9fdaea 8db265b3 e075ad52" },

{ L_, "6f6f1d ffff 9fdaea 8db265b3 e075ad52 h7", 120, 16,
                                   "6f6f1d 0000 9fdaea 8db265b3 e075ad52 h7" },
{ L_, "6f6f1d 0000 9fdaea 8db265b3 e075ad52 h7", 120, 16,
                                   "6f6f1d ffff 9fdaea 8db265b3 e075ad52 h7" },
{ L_, "6f6f1d aaaa 9fdaea 8db265b3 e075ad52 h7", 120, 16,
                                   "6f6f1d 5555 9fdaea 8db265b3 e075ad52 h7" },
{ L_, "6f6f1d 1234 9fdaea 8db265b3 e075ad52 h7", 120, 16,
                                   "6f6f1d edcb 9fdaea 8db265b3 e075ad52 h7" },
//>>------>>
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int       LINE      = DATA[i].d_line;
            const char     *STR       = DATA[i].d_array_p;
            const size_t    LEN       = bsl::strlen(STR);
            const int       IDX       = DATA[i].d_index;
            const int       NB        = DATA[i].d_numBits;
            const char     *RESULT    = DATA[i].d_result_p;
            LOOP_ASSERT(LINE, LEN == bsl::strlen(RESULT));

            if (veryVerbose) {
                P_(LINE) P_(STR) P_(IDX) P_(NB) P(RESULT);
            }

            uint64_t control[SET_UP_ARRAY_DIM] = { 0 };
            uint64_t array[  SET_UP_ARRAY_DIM] = { 0 };
            uint64_t result[ SET_UP_ARRAY_DIM] = { 0 };
            populateBitStringHex(control, 0, STR);
            populateBitStringHex(result,  0, RESULT);

            if (veryVerbose) {
                Util::print(cout, array,  NUM_BITS);
                Util::print(cout, result, NUM_BITS);
            }

            // check toggle

            wordCpy(array, control, sizeof(array));
            toggleOracle(array, IDX, NB);
            LOOP_ASSERT(LINE, !wordCmp(array, result,  sizeof(array)));

            // check toggle back

            toggleOracle(array, IDX, NB);
            LOOP_ASSERT(LINE, !wordCmp(array, control, sizeof(array)));

            // check toggle

            wordCpy(array, control, sizeof(array));
            Util::toggle(array, IDX, NB);
            LOOP_ASSERT(LINE, !wordCmp(array, result, sizeof(array)));

            // check toggle back

            Util::toggle(array, IDX, NB);
            LOOP_ASSERT(LINE, !wordCmp(array, control, sizeof(array)));
        }

        uint64_t bits[   SET_UP_ARRAY_DIM] = { 0 };
        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t result[ SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }

            for (int idx = 0; idx <= NUM_BITS; ++idx) {
                const int maxNumBits = NUM_BITS - idx;
                for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                    const int top = idx + numBits;
                    if (0 == ii) {
                        // ALL_FALSE

                        bsl::fill(bits + 0, bits + SET_UP_ARRAY_DIM, 0ULL);
                        Util::toggle(bits, idx, numBits);
                        ASSERT(! Util::isAny1(bits, 0, idx));
                        ASSERT(! Util::isAny0(bits, idx, numBits));
                        ASSERT(! Util::isAny1(bits, top, NUM_BITS - top));

                        // toggle back to ALL_FALSE

                        Util::toggle(bits, idx, numBits);
                        ASSERT(! Util::isAny1(bits, 0, NUM_BITS));

                        // ALL_TRUE

                        bsl::fill(bits + 0, bits + SET_UP_ARRAY_DIM, ~0ULL);
                        Util::toggle(bits, idx, numBits);
                        ASSERT(! Util::isAny0(bits, 0, idx));
                        ASSERT(! Util::isAny1(bits, idx, numBits));
                        ASSERT(! Util::isAny0(bits, top, NUM_BITS - top));

                        // toggle back to ALL_TRUE

                        Util::toggle(bits, idx, numBits);
                        ASSERT(! Util::isAny0(bits, 0, NUM_BITS));
                    }

                    wordCpy(result, control, sizeof(result));
                    toggleOracle(result, idx, numBits);
                    ASSERT(Util::areEqual(result, 0, control, 0, idx));
                    ASSERT(Util::areEqual(result, top, control, top,
                                                              NUM_BITS - top));

                    wordCpy(bits, control, sizeof(bits));
                    Util::toggle(bits, idx, numBits);
                    ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                    ASSERT(Util::areEqual(bits, top, control, top,
                                                              NUM_BITS - top));

                    ASSERT(! wordCmp(bits, result, sizeof(bits)));

                    // toggle back

                    Util::toggle(bits, idx, numBits);
                    ASSERT(! wordCmp(bits, control, sizeof(bits)));
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::toggle(bits, 0, 0));
            ASSERT_PASS(Util::toggle(bits, 0, 70));
            ASSERT_PASS(Util::toggle(bits, 70, 70));
            ASSERT_FAIL(Util::toggle(   0, 0, 0));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'NUM0' AND 'NUM1'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That the return value of 'num0' correctly reflects the
        //:   number of cleared bits in the specified range.
        //:
        //: 2 That the return value of 'num1' correctly reflects the number of
        //:   set bits in the specified range.
        //:
        //: 3 That the functions operate correctly on bit strings longer than
        //:   eight words long (this is important because the functions under
        //:   test have special logic dealing with multiples of 8 words).
        //:
        //: 4 That the array scanned is not modified.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Write the 'countOnes' oracle, which is very simple and
        //:   inefficient and relatively foolproof.
        //:
        //: 2 Do testing of 'num0', 'num1', and 'countOnes' on arrays of
        //:   all-true and all-false, since the results are very predictable.
        //:
        //: 3 Iterate on garbage data of length longer than 8 words with nested
        //:   loops iterating on 'index' and 'numBits'.
        //:
        //: 4 Apply 'num0' and 'num1', verifying their results against
        //:   'countOnes'.  (C-1..3)
        //:
        //: 5 After applying any of those functions, verify that the original
        //:   array has not been modified.  (C-4)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-5)
        //
        // Testing:
        //   St num0(const uint64_t *bitString, St index, St numBits);
        //   St num1(const uint64_t *bitString, St index, St numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'NUM0' AND 'NUM1'\n"
                               "=========================\n";

        const int    MULTIPLE = 5;
        const int    DIM      = SET_UP_ARRAY_DIM * MULTIPLE;
        const size_t NUM_BITS = DIM * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ DIM];
        uint64_t ALL_FALSE[DIM];

        uint64_t *endTrue  = ALL_TRUE  + DIM;
        uint64_t *endFalse = ALL_FALSE + DIM;

        bsl::fill(ALL_TRUE  + 0, endTrue,  ~0ULL);
        bsl::fill(ALL_FALSE + 0, endFalse,  0ULL);

        uint64_t bits[DIM], control[DIM];

        for (int ii = 0; ii < 150; ++ii) {
            fillWithGarbage(control, sizeof(control));
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(bits, NUM_BITS));
            }
            for (size_t idx = 0; idx <= NUM_BITS; incSizeT(&idx, NUM_BITS)) {
                const size_t maxNumBits = NUM_BITS - idx;
                for (size_t numBits = 0; numBits <= maxNumBits;
                                              incSizeT(&numBits, maxNumBits)) {
                    const size_t EXP_1 = countOnes(bits, idx, numBits);
                    const size_t EXP_0 = numBits - EXP_1;
                    ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                    ASSERT(EXP_1 == Util::num1(bits, idx, numBits));
                    ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                    ASSERT(EXP_0 == Util::num0(bits, idx, numBits));
                    ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
                }
            }
        }


#define CHECK_TRUE()  ASSERT(! Util::isAny0(ALL_TRUE,  0, NUM_BITS))
#define CHECK_FALSE() ASSERT(! Util::isAny1(ALL_FALSE, 0, NUM_BITS))

        for (size_t idx = 0; idx <= NUM_BITS; incSizeT(&idx, NUM_BITS)) {
            const size_t maxNumBits = NUM_BITS - idx;
            for (size_t numBits = 0; numBits <= maxNumBits;
                                              incSizeT(&numBits, maxNumBits)) {
                ASSERT(countOnes(ALL_TRUE,  idx, numBits) == numBits);
                CHECK_TRUE();
                ASSERT(Util::num1(ALL_TRUE,  idx, numBits) == numBits);
                CHECK_TRUE();
                ASSERT(Util::num0(ALL_FALSE, idx, numBits) == numBits);
                CHECK_FALSE();

                ASSERT(countOnes(ALL_FALSE, idx, numBits) == 0);
                CHECK_FALSE();
                ASSERT(Util::num0(ALL_TRUE,  idx, numBits) == 0);
                CHECK_TRUE();
                ASSERT(Util::num1(ALL_FALSE, idx, numBits) == 0);
                CHECK_FALSE();
            }
        }
        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::num1(ALL_FALSE, 0, 0));
            ASSERT_PASS(Util::num1(ALL_FALSE, 100, 640));
            ASSERT_FAIL(Util::num1(        0, 0, 0));

            ASSERT_PASS(Util::num0(ALL_FALSE, 0, 0));
            ASSERT_FAIL(Util::num0(        0, 0, 0));
            ASSERT_PASS(Util::num0(ALL_FALSE, 100, 640));

            CHECK_FALSE();
        }

#undef CHECK_TRUE
#undef CHECK_FALSE
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //   Ensure the method produces the correct output.
        //
        // Concerns:
        //: 1 That the function correctly prints out the state of 'bitString'.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing.
        //:   o Each iteration, verify that 'print' produces the correct
        //:     output.  (C-1)
        //:
        //:   o Each iteration, do negative testing passing a negative value to
        //:     'numBits'.
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   OS& print(OS& stream, U64 *bs, St nb, int lvl, int spl);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'print'"
                          << "\n===============" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_inputString_p;
            int         d_numBits;
            int         d_level;
            int         d_spl;
            const char *d_expString_p;
        } DATA [] = {
            {
                L_,
                "",
                0,
                1,
                4,
                "    [\n"
                "    ]\n"
            },
            {
                L_,
                "",
                0,
                -1,
                4,
                "[\n"
                "    ]\n"
            },
            {
                L_,
                "",
                0,
                1,
                -4,
                "    [ ]"
            },
            {
                L_,
                "",
                0,
                -1,
                -4,
                "[ ]"
            },

            {
                L_,
                "",
                0,
                0,
                0,
                "[\n"
                "]\n"
            },
            {
                L_,
                "",
                0,
                0,
                4,
                "[\n"
                "]\n"
            },
            {
                L_,
                "",
                0,
                1,
                0,
                "[\n"
                "]\n"
            },
            {
                L_,
                "",
                0,
                0,
                0,
                "[\n"
                "]\n"
            },

            {
                L_,
                "2",
                2,
                1,
                4,
                "    [\n"
                "        2\n"
                "    ]\n"
            },
            {
                L_,
                "1",
                1,
                -1,
                4,
                "[\n"
                "        1\n"
                "    ]\n"
            },
            {
                L_,
                "1",
                1,
                1,
                -4,
                "    [ 1 ]"
            },
            {
                L_,
                "1",
                1,
                -1,
                -4,
                "[ 1 ]"
            },
            {
                L_,
                "1",
                1,
                0,
                4,
                "[\n"
                "    1\n"
                "]\n"
            },
            {
                L_,
                "1",
                1,
                1,
                0,
                "[\n"
                "1\n"
                "]\n"
            },
            {
                L_,
                "1",
                1,
                0,
                0,
                "[\n"
                "1\n"
                "]\n"
            },

            {
                L_,
                "10",
                5,
                1,
                4,
                "    [\n"
                "        10\n"
                "    ]\n"
            },
            {
                L_,
                "10",
                5,
                -1,
                4,
                "[\n"
                "        10\n"
                "    ]\n"
            },
            {
                L_,
                "10",
                5,
                1,
                -4,
                "    [ 10 ]"
            },
            {
                L_,
                "10",
                5,
                -1,
                -4,
                "[ 10 ]"
            },
            {
                L_,
                "10",
                5,
                0,
                4,
                "[\n"
                "    10\n"
                "]\n"
            },
            {
                L_,
                "10",
                5,
                1,
                0,
                "[\n"
                "10\n"
                "]\n"
            },
            {
                L_,
                "10",
                5,
                0,
                0,
                "[\n"
                "10\n"
                "]\n"
            },

            {
                L_,
                "10",
                5,
                1,
                4,
                "    [\n"
                "        10\n"
                "    ]\n"
            },
            {
                L_,
                "10",
                5,
                -1,
                4,
                "[\n"
                "        10\n"
                "    ]\n"
            },
            {
                L_,
                "10",
                5,
                1,
                -4,
                "    [ 10 ]"
            },
            {
                L_,
                "10",
                5,
                -1,
                -4,
                "[ 10 ]"
            },
            {
                L_,
                "10",
                8,
                0,
                4,
                "[\n"
                "    10\n"
                "]\n"
            },
            {
                L_,
                "10",
                8,
                1,
                0,
                "[\n"
                "10\n"
                "]\n"
            },
            {
                L_,
                "10",
                8,
                0,
                0,
                "[\n"
                "10\n"
                "]\n"
            },

            {
                L_,
                "1 0ef1631b87b47882",
                65,
                1,
                4,
                "    [\n"
                "        "
                    "1 0ef1631b87b47882" "\n"
                "    ]\n"
            },
            {
                L_,
                "1dbcd3e4ad41cc70 17ff2ac621cd9445 fe26dbec0694a88f"
                                                            "3e0ed2f28fbc4c40"
                "b01e38cbcc657646 77dad9a364677b33 c1c5468902bfaff5"
                                                            "8db93c524d9400ff",
                8 * 64,
                -1,
                4,
                "[\n"
                "        "
                "1dbcd3e4ad41cc70 17ff2ac621cd9445 fe26dbec0694a88f" " "
                                                           "3e0ed2f28fbc4c40\n"
                "        "
                "b01e38cbcc657646 77dad9a364677b33 c1c5468902bfaff5" " "
                                                           "8db93c524d9400ff\n"
                "    ]\n"
            },
            {
                L_,
                "                 17ff2ac621cd9445 fe26dbec0694a88f"
                                                            "3e0ed2f28fbc4c40"
                "b01e38cbcc657646 77dad9a364677b33 c1c5468902bfaff5"
                                                            "8db93c524d9400ff",
                6 * 64 + 22,
                1,
                4,
                "    [\n"
                "        "
                "                           0d9445 fe26dbec0694a88f" " "
                                                           "3e0ed2f28fbc4c40\n"
                "        "
                "b01e38cbcc657646 77dad9a364677b33 c1c5468902bfaff5" " "
                                                           "8db93c524d9400ff\n"
                "    ]\n"
            },
            {
                L_,
                "b01e38cbcc657646 77dad9a364677b33 c1c5468902bfaff5"
                                                            "8db93c524d9400ff",
                3,
                -1,
                -4,
                "[ 7 ]"
            },
            {
                L_,
                "d6b5a889fd139dd0 e2530f22dd503f92 22a2aa9b91d7148c"
                                                           " 12e5a74d313b343b"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                           " 43df8d07a9f9f0fc",
                4 * 64 + 3,
                1,
                -4,
                "    [ 3 "
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc ]"
            },
            {
                L_,
                "d6b5a889fd139dd0 e2530f22dd503f92 22a2aa9b91d7148c"
                                                          " 12e5a74d313b343b"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc",
                5 * 64 + 23,
                0,
                4,
                "[\n"
                "    "
                "                                            57148c"
                                                          " 12e5a74d313b343b\n"
                "    "
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc\n"
                "]\n"
            },
            {
                L_,
                "d6b5a889fd139dd0 e2530f22dd503f92 22a2aa9b91d7148c"
                                                          " 12e5a74d313b343b"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc",
                5 * 64 + 23,
                1,
                4,
                "    [\n"
                "        "
                "                                            57148c"
                                                          " 12e5a74d313b343b\n"
                "        "
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc\n"
                "    ]\n"
            },
            {
                L_,
                "d6b5a889fd139dd0 e2530f22dd503f92 22a2aa9b91d7148c"
                                                          " 12e5a74d313b343b"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc",
                5 * 64 + 23,
                1,
                0,
                "[\n"
                "                                            57148c"
                                                          " 12e5a74d313b343b\n"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc\n"
                "]\n"
            },
            {
                L_,
                "d6b5a889fd139dd0 e2530f22dd503f92 22a2aa9b91d7148c"
                                                          " 12e5a74d313b343b"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc",
                6 * 64 + 5,
                0,
                0,
                "[\n"
                "                               12 22a2aa9b91d7148c"
                                                          " 12e5a74d313b343b\n"
                "6dad858fbf046bec 0c65ff70e33b9034 15bd0583dacb86e3"
                                                          " 43df8d07a9f9f0fc\n"
                "]\n"
            },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const int MAX_ARRAY_SIZE = 4 * 4;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *INPUT = DATA[i].d_inputString_p;
            int         NB    = DATA[i].d_numBits;
            int         LEVEL = DATA[i].d_level;
            int         SPL   = DATA[i].d_spl;
            const char *EXP   = DATA[i].d_expString_p;

            if (veryVerbose) {
                P(LINE) P_(INPUT) P_(LEVEL) P_(SPL) P(EXP);
            }

            bsl::ostringstream stream;
            uint64_t           bitstring[MAX_ARRAY_SIZE] = { 0 };

            populateBitStringHex(bitstring, 0, INPUT);
            Util::print(stream,
                       bitstring,
                       NB,
                       LEVEL,
                       SPL);
            const bsl::string& str = stream.str();
            LOOP3_ASSERT(LINE, EXP, str, EXP == str);
        }

        {
            bsls::AssertTestHandlerGuard guard;

            uint64_t           x = 1;
            bsl::ostringstream oss;

            ASSERT_PASS(Util::print(oss, &x, 32, 0, 0));
            ASSERT_FAIL(Util::print(oss,  0, 32, 0, 0));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'swapRaw'
        //   Ensure 'swapRaw' correctly swaps the specified bit strings.
        //
        // Concerns:
        //: 1 That 'swapRaw' performs correctly on valid input.
        //:
        //: 2 That 'swapRaw' triggers an assert when told to swap overlapping
        //:   areas, on all builds.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do nested loops varying two indexes into the array, and varying
        //:   the number of bits to swap.
        //:
        //: 2 Determine if the areas to be swapped overlap.
        //:   o If no overlap, do the swap and then verify that it works.
        //:     (C-1)
        //:
        //:   o If overlap, do negative testing to ensure the assert to detect
        //:     overlaps in 'swapRaw' catches it.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-3)
        //
        // Testing:
        //   void swapRaw(U64 *lhsBS, St lIdx, U64 *rhsBS, St rIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'swapRaw'\n"
                          << "=================\n";

        const int NUM_BITS = 2 * SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[2 * SET_UP_ARRAY_DIM] = { 0 };
        uint64_t bits[   2 * SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 20; ++ii) {
            // To test this code properly it was necessary to have a long array
            // to test for a wide variety of of combinations of 'idxA', 'idxB',
            // and 'numBits'.  To get this done in less than 15 seconds on
            // Solaris we have to do only 20 test cases, so we use
            // 'fillWithGarbage' instead of 'setUpArray'.

            fillWithGarbage(control, sizeof(control));

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            int shiftA = 0;
            int shiftB = 0;

            const int maxIdx = NUM_BITS - 1;
            for (int idxA = 0; idxA <= maxIdx; incInt(&idxA, maxIdx)) {
                for (int idxB = 0; idxB <= maxIdx; incInt(&idxB, maxIdx)) {

                    const int maxNumBits = NUM_BITS - bsl::max(idxA, idxB);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        // Shift the pointers to the arrays around to ensure
                        // the overlap detection logic in 'swapRaw' still holds
                        // up when both bitArray ptrs don't match.

                        shiftA += 3;
                        if (shiftA > 0) {
                            shiftA -= 41;
                        }
                        shiftB += 5;
                        if (shiftB > 0) {
                            shiftB -= 41;
                        }

                        uint64_t *bitsShiftA = bits + shiftA;
                        int       idxShiftA  = idxA -
                                                    shiftA * k_BITS_PER_UINT64;

                        uint64_t *bitsShiftB = bits + shiftB;
                        int       idxShiftB  = idxB -
                                                    shiftB * k_BITS_PER_UINT64;

                        if (idxA + numBits <= idxB || idxB + numBits <= idxA) {
                            wordCpy(bits, control, sizeof(bits));

                            Util::swapRaw(bitsShiftA,
                                         idxShiftA,
                                         bitsShiftB,
                                         idxShiftB,
                                         numBits);

                            const int idxMin    = bsl::min(idxA, idxB);
                            const int endIdxMax = bsl::max(idxA, idxB) +
                                                                       numBits;

                            // First, verify all the bits that should have been
                            // unchanged.

                            ASSERT(Util::areEqual(bits, 0, control, 0,idxMin));
                            ASSERT(Util::areEqual(bits,
                                                 endIdxMax,
                                                 control,
                                                 endIdxMax,
                                                 NUM_BITS - endIdxMax));
                            if (idxMin + numBits < endIdxMax - numBits) {
                                ASSERT(Util::areEqual(
                                            bits,
                                            idxMin + numBits,
                                            control,
                                            idxMin + numBits,
                                            endIdxMax - idxMin - 2 * numBits));
                            }

                            // Then, verify the bits that were swapped.

                            ASSERT(Util::areEqual(bits,
                                                 idxA,
                                                 control,
                                                 idxB,
                                                 numBits));
                            ASSERTV(ii, idxA, idxB, numBits,
                                    pHex(bits,    NUM_BITS),
                                    pHex(control, NUM_BITS),
                                    Util::areEqual(bits,
                                                  idxB,
                                                  control,
                                                  idxA,
                                                  numBits));
                        }
                        else {
                            bsls::AssertTestHandlerGuard guard;

                            const int preTestStatus = testStatus;

                            ASSERT_SAFE_FAIL(Util::swapRaw(bitsShiftA,
                                                           idxShiftA,
                                                           bitsShiftB,
                                                           idxShiftB,
                                                           numBits));

                            if (testStatus > preTestStatus) {
                                ASSERTV(ii, idxA, idxB, numBits, 0);
                            }
                        }
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::swapRaw(bits, 0, control, 0, 0));
            ASSERT_PASS(Util::swapRaw(bits, 0, control, 0, 100));
            ASSERT_PASS(Util::swapRaw(bits, 100, control, 100, 100));
            ASSERT_FAIL(Util::swapRaw(   0, 0, control, 0, 0));
            ASSERT_FAIL(Util::swapRaw(bits, 0,       0, 0, 0));

            ASSERT_PASS(Util::swapRaw(bits, 0, bits + 1, 0, 64));
            ASSERT_PASS(Util::swapRaw(bits + 1, 0, bits, 0, 64));
            ASSERT_SAFE_FAIL(Util::swapRaw(bits, 1, bits + 1, 0, 64));
            ASSERT_SAFE_FAIL(Util::swapRaw(bits, 0, bits + 1, 0, 65));
            ASSERT_SAFE_FAIL(Util::swapRaw(bits + 1, 0, bits, 1, 64));
            ASSERT_SAFE_FAIL(Util::swapRaw(bits + 1, 0, bits, 0, 65));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE' AND 'REMOVEANDFILL[01]'
        //   Ensure the methods have the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That 'remove' and 'removeAndFill[01]' function according to spec.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate through different initial values of an array using
        //:   'setUpArray'.
        //:   o Do nested loops iterating through 'length' and 'idx'.
        //:
        //:   o Apply 'remove', 'removeAndFill0', and 'removeAndFill1' with
        //:     '0 == numBits' and verify they don't change the array.
        //:
        //:   o Iterate over different values of 'numBits'.
        //:     1 Call 'remove' and verify the changes are as expected.
        //:
        //:     2 Call 'removeAndFill0' and 'removeAndFill1' and verify the
        //:       changes are as expected.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void remove(uint64_t *bitString, St len, St idx, St numBits);
        //   void removeAndFill0(U64 *bitString, St len, St idx, St nb);
        //   void removeAndFill1(U64 *bitString, St len, St idx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'REMOVE' AND 'REMOVEANDFILL[01]'\n"
                               "========================================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t bits[   SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            const int maxIdx = NUM_BITS - 1;
            for (int idx = 0; idx <= maxIdx; incInt(&idx, maxIdx)) {
                const int maxDeltaLength = NUM_BITS - idx;
                for (int length = idx, deltaLength = 0; length <= NUM_BITS;
                                          incInt(&deltaLength, maxDeltaLength),
                                                  length = idx + deltaLength) {
                    wordCpy(bits, control, sizeof(bits));

                    Util::remove(bits, length, idx, 0);
                    ASSERT(! wordCmp(bits, control, sizeof(bits)));

                    Util::removeAndFill0(bits, length, idx, 0);
                    ASSERT(! wordCmp(bits, control, sizeof(bits)));

                    const int maxNumBits = length - idx;
                    for (int numBits = 1; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {

                        // remove

                        wordCpy(bits, control, sizeof(bits));
                        Util::remove(bits, length, idx, numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(Util::areEqual(bits, idx,
                                             control, idx + numBits,
                                             length - (idx + numBits)));
                        ASSERT(Util::areEqual(bits,    length - numBits,
                                             control, length - numBits,
                                             NUM_BITS - (length - numBits)));

                        // removeAndFill0

                        wordCpy(bits, control, sizeof(bits));
                        Util::removeAndFill0(bits, length, idx, numBits);

                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(Util::areEqual(bits,    idx,
                                             control, idx + numBits,
                                             length - (idx + numBits)));
                        ASSERT(!Util::isAny1(bits, length - numBits, numBits));
                        ASSERT(Util::areEqual(bits, length, control, length,
                                             NUM_BITS - length));

                        // removeAndFill0

                        wordCpy(bits, control, sizeof(bits));
                        Util::removeAndFill1(bits, length, idx, numBits);

                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(Util::areEqual(bits,    idx,
                                             control, idx + numBits,
                                             length - (idx + numBits)));
                        ASSERT(!Util::isAny0(bits, length - numBits, numBits));
                        ASSERT(Util::areEqual(bits, length, control, length,
                                             NUM_BITS - length));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::remove(bits,   0, 0, 0));
            ASSERT_PASS(Util::remove(bits, 100, 0, 10));
            ASSERT_PASS(Util::remove(bits, 100, 0, 100));
            ASSERT_FAIL(Util::remove(   0,   0, 0, 0));
            ASSERT_FAIL(Util::remove(bits, 100, 0, 101));
            ASSERT_FAIL(Util::remove(   0,  0, 0, 0));

            ASSERT_SAFE_PASS(Util::removeAndFill0(bits,   0, 0, 0));
            ASSERT_SAFE_PASS(Util::removeAndFill0(bits, 100, 0, 0));
            ASSERT_SAFE_PASS(Util::removeAndFill0(bits, 100, 0, 10));
            ASSERT_SAFE_PASS(Util::removeAndFill0(bits, 100, 0, 100));
            ASSERT_SAFE_FAIL(Util::removeAndFill0(   0,   0, 0, 0));
            ASSERT_SAFE_FAIL(Util::removeAndFill0(bits, 100, 0, 101));

            ASSERT_SAFE_PASS(Util::removeAndFill1(bits,   0, 0, 0));
            ASSERT_SAFE_PASS(Util::removeAndFill1(bits, 100, 0, 0));
            ASSERT_SAFE_PASS(Util::removeAndFill1(bits, 100, 0, 10));
            ASSERT_SAFE_PASS(Util::removeAndFill1(bits, 100, 0, 100));
            ASSERT_SAFE_FAIL(Util::removeAndFill1(   0,  0, 0, 0));
            ASSERT_SAFE_FAIL(Util::removeAndFill1(bits, 100, 0, 101));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'insert*'
        //   Ensure the methods have the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That 'insert', 'insert0', 'insert1', and 'insertRaw' all move the
        //:   bits specified appropriately to the left.
        //:
        //: 2 That 'insert', 'insert0', and 'insert1' all initialize the bits
        //:   vacated by the left shift of the other bits appropriately.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate over initial array states using 'setUpArray'.
        //:   o Iterate over values of 'idx' varying from 0 to 'NUM_BITS-1'.
        //:     1 Do inserts with all 4 functions with '0 == numBits' and
        //:       verify there is no change to the bit string.
        //:       o Do nested loops varying 'numBits' and 'length'.
        //:
        //:       o Call the four 'insert' methods.  Verify that the bits were
        //:         shifted appropriately, that the bits before 'idx' were
        //:         unchanged, and (when appropriate) that the range
        //:         '[idx .. idx + numBits)' was filled in appropriately.
        //:         (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-3)
        //
        // Testing:
        //   void insert(U64 *bitString, St len, St idx, bool val, St nb);
        //   void insert0(uint64_t *bitString, St length, St idx, St numBits);
        //   void insert1(uint64_t *bitString, St length, St idx, St numBits);
        //   void insertRaw(uint64_t *bitString, St len, St idx, St numBits);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << endl
                               << "TESTING 'insert*'\n"
                               << "=================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t bits[   SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 70; ) {
            setUpArray(control, &ii, true);

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            const int maxIdx = NUM_BITS - 1;
            for (int idx = 0; idx <= maxIdx; incInt(&idx, maxIdx)) {
                wordCpy(bits, control, sizeof(bits));

                // Note this value of 'length' is overridden in the nested
                // loops below.

                int length = idx < NUM_BITS / 2 ? NUM_BITS / 2 : NUM_BITS;

                // insert true

                Util::insert(   bits, length, idx, true,  0);
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                // insert1

                Util::insert1(  bits, length, idx, 0);
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                // insert false

                Util::insert(   bits, length, idx, false, 0);
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                // insert0

                Util::insert0(  bits, length, idx, 0);
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                // insertRaw

                Util::insertRaw(bits, length, idx, 0);
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                const int maxNumBits = NUM_BITS - idx;
                ASSERT(maxNumBits >= 1);
                for (int numBits = 1; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                    LOOP_ASSERT(numBits, numBits >= 1);
                    const int maxLength = NUM_BITS - numBits;
                    for (length = idx; length <= maxLength;
                                                  incInt(&length, maxLength)) {
                        if (veryVeryVerbose) {
                            P_(idx); P_(numBits); P(length);
                        }

                        // insert true

                        wordCpy(bits, control, sizeof(bits));
                        Util::insert(   bits, length, idx, true,  numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(! Util::isAny0(bits, idx, numBits));
                        ASSERT(Util::areEqual(bits, idx + numBits,
                                             control, idx,
                                             length - idx));
                        ASSERT(Util::areEqual(bits,    length + numBits,
                                             control, length + numBits,
                                             NUM_BITS - length - numBits));
                        // insert1

                        wordCpy(bits, control, sizeof(bits));
                        Util::insert1(  bits, length, idx, numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(! Util::isAny0(bits, idx, numBits));
                        ASSERT(Util::areEqual(bits, idx + numBits,
                                             control, idx,
                                             length - idx));
                        ASSERT(Util::areEqual(bits,    length + numBits,
                                             control, length + numBits,
                                             NUM_BITS - length - numBits));

                        // insert false

                        wordCpy(bits, control, sizeof(bits));
                        Util::insert(   bits, length, idx, false, numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(! Util::isAny1(bits, idx, numBits));
                        ASSERT(Util::areEqual(bits, idx + numBits,
                                             control, idx,
                                             length - idx));
                        ASSERT(Util::areEqual(bits,    length + numBits,
                                             control, length + numBits,
                                             NUM_BITS - length - numBits));

                        // insert0

                        wordCpy(bits, control, sizeof(bits));
                        Util::insert0(  bits, length, idx, numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0, idx));
                        ASSERT(! Util::isAny1(bits, idx, numBits));
                        ASSERT(Util::areEqual(bits, idx + numBits,
                                             control, idx,
                                             length - idx));
                        ASSERT(Util::areEqual(bits,    length + numBits,
                                             control, length + numBits,
                                             NUM_BITS - length - numBits));

                        // insertRaw

                        wordCpy(bits, control, sizeof(bits));
                        Util::insertRaw(bits, length, idx, numBits);
                        ASSERT(Util::areEqual(bits, 0, control, 0,
                                                               idx + numBits));
                        ASSERT(Util::areEqual(bits, idx + numBits,
                                             control, idx,
                                             length - idx));
                        ASSERT(Util::areEqual(bits,    length + numBits,
                                             control, length + numBits,
                                             NUM_BITS - length - numBits));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::insert(bits, 0, 0, true, 0));
            ASSERT_PASS(Util::insert(bits, 10, 0, true, 10));
            ASSERT_PASS(Util::insert(bits, 100, 40, true, 30));
            ASSERT_PASS(Util::insert(bits, 100, 100, true, 60));
            ASSERT_FAIL(Util::insert(   0, 0, 0, true, 0));
            ASSERT_FAIL(Util::insert(bits, 100, 101, true, 60));

            ASSERT_PASS(Util::insert(bits, 0, 0, false, 0));
            ASSERT_PASS(Util::insert(bits, 10, 0, false, 10));
            ASSERT_PASS(Util::insert(bits, 100, 40, false, 30));
            ASSERT_PASS(Util::insert(bits, 100, 100, false, 60));
            ASSERT_FAIL(Util::insert(   0, 0, 0, false, 0));
            ASSERT_FAIL(Util::insert(bits, 100, 101, false, 60));

            ASSERT_PASS(Util::insertRaw(bits, 0, 0, 0));
            ASSERT_PASS(Util::insertRaw(bits, 10, 0, 10));
            ASSERT_PASS(Util::insertRaw(bits, 100, 40, 30));
            ASSERT_PASS(Util::insertRaw(bits, 100, 100, 60));
            ASSERT_FAIL(Util::insertRaw(   0, 0, 0, 0));
            ASSERT_FAIL(Util::insertRaw(bits, 100, 101, 60));

            ASSERT_PASS(Util::insert0(bits, 0, 0, 0));
            ASSERT_PASS(Util::insert0(bits, 10, 0, 10));
            ASSERT_PASS(Util::insert0(bits, 100, 40, 30));
            ASSERT_PASS(Util::insert0(bits, 100, 100, 60));
            ASSERT_FAIL(Util::insert0(   0, 0, 0, 0));
            ASSERT_FAIL(Util::insert0(bits, 100, 101, 60));

            ASSERT_PASS(Util::insert1(bits, 0, 0, 0));
            ASSERT_PASS(Util::insert1(bits, 10, 0, 10));
            ASSERT_PASS(Util::insert1(bits, 100, 40, 30));
            ASSERT_PASS(Util::insert1(bits, 100, 100, 60));
            ASSERT_FAIL(Util::insert1(   0, 0, 0, 0));
            ASSERT_FAIL(Util::insert1(bits, 100, 101, 60));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING OVERLAPPING COPIES
        //   Ensure the copy methods work properly in the overlapping case.
        //
        // Concerns:
        //: 1 That 'copy' and 'copyRaw' correctly deal with overlapping copies.
        //:   Note that 'copyRaw' can only do overlapping copies in the case
        //:   where the destination is lower than the source.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do nested loops varying 'dstIdx', 'srcIdx', and 'numBits'.
        //:   o Skip iterations where there is no overlap.
        //:
        //:   o Call 'copy' and observe it was correct.
        //:
        //:   o If the overlap is a left overlap, call 'copyRaw' and observe it
        //:     was correct.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void copyRaw(U64 *dstBS, St dIdx, U64 *srcBS St sIdx, St nb);
        //   void copy(U64 *dstBS, St dIdx, U64 *srcBS St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OVERLAPPING COPIES\n"
                             "==========================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t bits[   SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            int shiftDstBy = 0;
            int shiftSrcBy = 0;

            const int maxIdx = NUM_BITS - 1;
            for (int srcIdx = 0; srcIdx <= maxIdx; incInt(&srcIdx, maxIdx)) {
                for (int dstIdx = 0; dstIdx <= maxIdx;
                                                     incInt(&dstIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(dstIdx, srcIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        if (intAbs(srcIdx - dstIdx) >= numBits) {
                            // Not overlapping, we already tested in the
                            // previous test case.

                            continue;
                        }

                        // Move the two array around to ensure the functions
                        // aren't confused by shifting the arrays.

                        shiftDstBy += 3;
                        if (shiftDstBy > 0) {
                            shiftDstBy -= 41;
                        }
                        shiftSrcBy += 5;
                        if (shiftSrcBy > 0) {
                            shiftSrcBy -= 41;
                        }

                        uint64_t  *shiftDst    = bits + shiftDstBy;
                        const int  shiftDstIdx = dstIdx -
                                                shiftDstBy * k_BITS_PER_UINT64;

                        uint64_t  *shiftSrc    = bits + shiftSrcBy;
                        const int  shiftSrcIdx = srcIdx -
                                                shiftSrcBy * k_BITS_PER_UINT64;

                        // copy

                        wordCpy(bits, control, sizeof(bits));
                        Util::copy(shiftDst,
                                  shiftDstIdx,
                                  shiftSrc,
                                  shiftSrcIdx,
                                  numBits);

                        ASSERT(Util::areEqual(bits, 0, control, 0, dstIdx));
                        ASSERT(Util::areEqual(bits, dstIdx, control, srcIdx,
                                                                     numBits));
                        ASSERT(Util::areEqual(bits,    dstIdx + numBits,
                                             control, dstIdx + numBits,
                                             NUM_BITS - dstIdx - numBits));

                        if (dstIdx <= srcIdx) {
                            // A left copy should work, try 'copyRaw'.

                            wordCpy(bits, control, sizeof(bits));
                            Util::copyRaw(shiftDst,
                                         shiftDstIdx,
                                         shiftSrc,
                                         shiftSrcIdx,
                                         numBits);

                            ASSERT(Util::areEqual(bits, 0, control, 0,dstIdx));
                            ASSERT(Util::areEqual(bits, dstIdx, control,srcIdx,
                                                                     numBits));
                            ASSERT(Util::areEqual(bits,    dstIdx + numBits,
                                                 control, dstIdx + numBits,
                                                 NUM_BITS - dstIdx - numBits));
                        }
                        else {
                            {
                                bsls::AssertTestHandlerGuard guard;

                                wordCpy(bits, control, sizeof(bits));
                                ASSERT_SAFE_FAIL(Util::copyRaw(shiftDst,
                                                              shiftDstIdx,
                                                              shiftSrc,
                                                              shiftSrcIdx,
                                                              numBits));
                            }
                            ASSERT(Util::areEqual(bits, 0, control, 0,
                                                                    NUM_BITS));
                        }
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::copy(bits, 0, bits, 0, 0));
            ASSERT_PASS(Util::copy(bits, 70, bits, 10, 100));
            ASSERT_FAIL(Util::copy(   0, 0, bits, 0, 0));

            ASSERT_PASS(Util::copyRaw(bits, 0, bits, 0, 0));
            ASSERT_FAIL(Util::copyRaw(   0, 0, bits, 0, 0));
            ASSERT_FAIL(Util::copyRaw(bits, 0, 0,    0, 0));

            enum { HALF_BITS = NUM_BITS / 2 };
            for (int di = 1; di <= HALF_BITS; ++di) {
                for (int si = 0; si < di; ++si) {
                    const int d = di - si;    (void) d;

                    uint64_t *dst = bits, *src = bits;
                    int       di2 = di,    si2 = si;
                    shiftPtrIndexPair(&dst, &di2);
                    shiftPtrIndexPair(&src, &si2);

                    // right overlapping copies

                    ASSERT_SAFE_FAIL(Util::copyRaw(bits, di,  bits, si,  d+1));
                    ASSERT_SAFE_FAIL(Util::copyRaw(dst,  di2, src,  si2, d+1));
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING NON-OVERLAPPING COPIES
        //   Ensure the copy methods work properly in the non-overlapping case.
        //
        // Concerns:
        //: 1 The 'copy' and 'copyRaw' functions work correctly in the case
        //:   where the source and destination are in different buffers and
        //:   hence never overlap.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate, varying src index, dst index, and number of bits to
        //:   copy.  Perform copies and verify the behavior was as expected.
        //:   (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void copyRaw(U64 *dstBS, St dIdx, U64 *srcBS, St sIdx, St nb);
        //   void copy(U64 *dstBS, St dIdx, U64 *srcBS, St sIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING NON-OVERLAPPING COPIES\n"
                             "==============================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t dstControl[SET_UP_ARRAY_DIM], dst[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t srcControl[SET_UP_ARRAY_DIM], src[SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 100; ) {
            int jj = (ii * 11 + 3) % 100;
            setUpArray(dstControl, &ii);
            setUpArray(srcControl, &jj);

            const int maxIdx = NUM_BITS - 1;
            for (int srcIdx = 0; srcIdx <= maxIdx; incInt(&srcIdx, maxIdx)) {
                for (int dstIdx = 0; dstIdx <= maxIdx;
                                                     incInt(&dstIdx, maxIdx)) {
                    int maxNumBits = NUM_BITS - bsl::max(dstIdx, srcIdx);
                    for (int numBits = 0; numBits <= maxNumBits;
                                                incInt(&numBits, maxNumBits)) {
                        // copyRaw

                        wordCpy(dst, dstControl, sizeof(dst));
                        wordCpy(src, srcControl, sizeof(src));

                        Util::copyRaw(dst, dstIdx, src, srcIdx, numBits);

                        ASSERT(!wordCmp(src, srcControl, sizeof(src)));
                        ASSERT(Util::areEqual(dst, 0, dstControl,0,dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, src, srcIdx,
                                                                     numBits));
                        ASSERT(Util::areEqual(dst,        dstIdx + numBits,
                                                dstControl, dstIdx + numBits,
                                                 NUM_BITS - dstIdx - numBits));

                        // copy

                        wordCpy(dst, dstControl, sizeof(dst));

                        Util::copy(dst, dstIdx, src, srcIdx, numBits);

                        ASSERT(!wordCmp(src, srcControl, sizeof(src)));
                        ASSERT(Util::areEqual(dst, 0, dstControl,0,dstIdx));
                        ASSERT(Util::areEqual(dst, dstIdx, src, srcIdx,
                                                                     numBits));
                        ASSERT(Util::areEqual(dst,        dstIdx + numBits,
                                                dstControl, dstIdx + numBits,
                                                 NUM_BITS - dstIdx - numBits));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::copy(dst, 0, src, 0, 0));
            ASSERT_PASS(Util::copy(dst, 0, src, 0, 70));
            ASSERT_PASS(Util::copy(dst, 70, src, 0, 70));
            ASSERT_PASS(Util::copy(dst, 70, src, 170, 70));
            ASSERT_FAIL(Util::copy(  0, 0, src, 0, 0));
            ASSERT_FAIL(Util::copy(dst, 0,   0, 0, 0));

            ASSERT_PASS(Util::copyRaw(dst, 0, src, 0, 0));
            ASSERT_PASS(Util::copyRaw(dst, 0, src, 0, 70));
            ASSERT_PASS(Util::copyRaw(dst, 70, src, 0, 70));
            ASSERT_PASS(Util::copyRaw(dst, 70, src, 170, 70));
            ASSERT_FAIL(Util::copyRaw(  0, 0, src, 0, 0));
            ASSERT_FAIL(Util::copyRaw(dst, 0,   0, 0, 0));

            enum { HALF_BITS = NUM_BITS / 2 };
            for (int di = 1; di <= HALF_BITS; ++di) {
                for (int si = 0; si < di; ++si) {
                    const int d = di - si;

                    uint64_t *dst2 = dst, *src2 = dst;
                    int       di2  = di,    si2 = si;
                    shiftPtrIndexPair(&dst2, &di2);
                    shiftPtrIndexPair(&src2, &si2);

                    // right barely non-overlapping copy

                    ASSERT_PASS(Util::copyRaw(dst,  di,  dst,  si,  d));
                    ASSERT_PASS(Util::copyRaw(dst2, di2, src2, si2, d));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'ISANY0' AND 'ISANY1'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That 'isAny0' and 'isAny1' correctly detect the presence of clear
        //:   or set bits.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate over different values of the 'control' buffer, using
        //:   'setUpArray'.
        //:   o Iterate over values of 'idx' and 'numBits'.
        //:     1 Apply 'isAny0' and 'isAny1' to copies of 'ALL_TRUE' and
        //:       'all_false', with predictable results (given that
        //:       'numBits > 0').
        //:
        //:     2 Apply 'isAny0' and 'isAny1' to copies of the 'control'
        //:       buffer.  If the return value is 'false', that should mean
        //:       that stretch of the buffer should equal 'ALL_TRUE' or
        //:       'ALL_FALSE'.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   bool isAny0(const uint64_t *bitString, St index, St numBits);
        //   bool isAny1(const uint64_t *bitString, St index, St numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'ISANY0' AND 'ISANY1'\n"
                               "=============================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM], allTrue[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM], allFalse[SET_UP_ARRAY_DIM];

        bsl::fill(ALL_TRUE  + 0, ALL_TRUE  + SET_UP_ARRAY_DIM, ~0ULL);
        bsl::fill(ALL_FALSE + 0, ALL_FALSE + SET_UP_ARRAY_DIM, 0ULL);
        bsl::fill(allTrue   + 0, allTrue   + SET_UP_ARRAY_DIM, ~0ULL);
        bsl::fill(allFalse  + 0, allFalse  + SET_UP_ARRAY_DIM, 0ULL);

        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t bits[   SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 75; ) {
            setUpArray(control, &ii);
            wordCpy(bits, control, sizeof(bits));

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            for (int idx = 0; idx < NUM_BITS; ++idx) {
                ASSERT(false == Util::isAny0(bits, idx, 0));
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
                ASSERT(false == Util::isAny1(bits, idx, 0));
                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                for (int numBits = 1; idx + numBits <= NUM_BITS; ++numBits) {
                    // isAny0

                    ASSERT(false == Util::isAny0(allTrue, idx, numBits));
                    ASSERT(true  == Util::isAny1(allTrue, idx, numBits));
                    ASSERT(0 == wordCmp(allTrue, ALL_TRUE, sizeof(bits)));

                    ASSERT(true  == Util::isAny0(allFalse, idx, numBits));
                    ASSERT(false == Util::isAny1(allFalse, idx, numBits));
                    ASSERT(0 == wordCmp(allFalse, ALL_FALSE, sizeof(bits)));

                    wordCpy(bits, control, sizeof(bits));

                    LOOP3_ASSERT(ii, idx, numBits,
                        Util::isAny0(bits, idx, numBits) ==
                           ! Util::areEqual(bits, idx, ALL_TRUE,  0, numBits));
                    ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                    // isAny1

                    LOOP3_ASSERT(ii, idx, numBits,
                        Util::isAny1(bits, idx, numBits) ==
                           ! Util::areEqual(bits, idx, ALL_FALSE, 0, numBits));
                    ASSERT(0 == wordCmp(bits, control, sizeof(bits)));
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::isAny0(bits, 0, 0));
            ASSERT_PASS(Util::isAny0(bits, 70, 70));
            ASSERT_FAIL(Util::isAny0(   0, 0, 0));

            ASSERT_PASS(Util::isAny1(bits, 0, 0));
            ASSERT_PASS(Util::isAny1(bits, 70, 70));
            ASSERT_FAIL(Util::isAny1(   0, 0, 0));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'assignBits'
        //   Ensure the method has the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That 'assignBits' performs properly on valid input.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate, using 'setUpArray' to populate 'control'.
        //:   o Iterate, using 'setUpArray' to populate the one word scalar
        //:     'src'.
        //:     1 Iterate nested loops over values of 'index' and 'numBits'.
        //:       o Call 'assignBits'
        //:
        //:       o Use 'areEqual' to check all of 'dst'.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   void assignBits(U64 *bitString, St index, U64 srcBits, St nb);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'assignBits'\n"
                             "====================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[ SET_UP_ARRAY_DIM];
        uint64_t dst[     SET_UP_ARRAY_DIM];
        uint64_t srcArray[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 72; ) {
            setUpArray(control, &ii);

            for (int jj = 0; jj < 72; ) {
                setUpArray(srcArray, &jj, true);
                const uint64_t src = srcArray[0];

                if (veryVerbose) {
                    P_(ii);    P(pHex(dst,  NUM_BITS));
                    P_(jj);    P(pHex(&src, k_BITS_PER_UINT64));
                }

                const int maxIdx = NUM_BITS - 1;
                for (int index = 0; index <= maxIdx; incInt(&index, maxIdx)) {
                    const int maxNumBits = bsl::min<int>(NUM_BITS - index,
                                                         k_BITS_PER_UINT64);
                    for (int numBits = 0; numBits <= maxNumBits; ++numBits) {
                        wordCpy(dst, control, sizeof(dst));

                        Util::assignBits(dst, index, src, numBits);

                        ASSERT(Util::areEqual(dst, 0, control, 0, index));
                        ASSERT(Util::areEqual(dst, index, &src, 0, numBits));
                        ASSERT(Util::areEqual(dst, index + numBits, control,
                               index + numBits, NUM_BITS - (index + numBits)));
                    }
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::assignBits(dst, 0, 0, 0));
            ASSERT_PASS(Util::assignBits(dst, 0, 0, 10));
            ASSERT_PASS(Util::assignBits(dst, 0, 0, k_BITS_PER_UINT64));

            ASSERT_PASS(Util::assignBits(dst, 0, -1, 0));
            ASSERT_PASS(Util::assignBits(dst, 0, -1, 10));
            ASSERT_PASS(Util::assignBits(dst, 0, -1, k_BITS_PER_UINT64));

            ASSERT_FAIL(Util::assignBits(dst,  0, -1, k_BITS_PER_UINT64 + 1));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MULTI-BIT 'bits', 'assign', 'assign0', 'assign1'
        //   Ensure the methods have the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That 'bits', 'assign', 'assign0', and 'assign1' all function
        //:   properly on valid input.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate, populating a buffer with bit patterns using
        //:   'setUpArray'.
        //:   o Iterate nested loops over 'idx' and 'numBits'.
        //:     1 If 'numBits' fits within a word, tests 'bits'
        //:       o Using C-style bit operations to calculate 'exp', the
        //:         expected return value of 'bits'.
        //:
        //:       o Call 'bits' and compare to 'exp'.
        //:
        //:     2 Call 'assign' with the bool set to 'true'.
        //:
        //:     3 Use 'areEqual' to verify that assign had the correct effect.
        //:
        //:     4 Call 'assign1' on a separate buffer.
        //:
        //:     5 Verify that the buffers operated on by 'assign' and 'assign1'
        //:       match.
        //:
        //:     6 Call 'assign' with the bool set to 'false'.
        //:
        //:     7 Use 'areEqual' to verify that assign had the correct effect.
        //:
        //:     8 Call 'assign0' on a separate buffer.
        //:
        //:     9 Verify that the buffers operated on by 'assign' and 'assign0'
        //:       match.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   uint64_t bits(const uint64_t *bitString, St index, St numBits);
        //   void assign(U64 *bitString, St index, bool value, St numBits);
        //   void assign0(uint64_t *bitString, St index, St numBits);
        //   void assign1(uint64_t *bitString, St index, St numBits);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                "\nTESTING MULTI-BIT 'bits', 'assign', 'assign0', 'assign1'\n"
                  "========================================================\n";

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t ALL_TRUE[ SET_UP_ARRAY_DIM];
        uint64_t ALL_FALSE[SET_UP_ARRAY_DIM];

        bsl::fill(ALL_TRUE  + 0, ALL_TRUE  + SET_UP_ARRAY_DIM, ~0ULL);
        bsl::fill(ALL_FALSE + 0, ALL_FALSE + SET_UP_ARRAY_DIM, 0ULL);

        uint64_t control[SET_UP_ARRAY_DIM];
        uint64_t bits[SET_UP_ARRAY_DIM] = { 0 }, bitsB[SET_UP_ARRAY_DIM];

        for (int ii = 0; ii < 72; ) {
            setUpArray(control, &ii, true);

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            for (int idx = 0; idx < NUM_BITS; ++idx) {
                int word = idx / k_BITS_PER_UINT64;
                int pos  = idx % k_BITS_PER_UINT64;

                for (int numBits = 0; idx + numBits <= NUM_BITS; ++numBits) {
                    if (numBits <= k_BITS_PER_UINT64) {
                        int      rem = k_BITS_PER_UINT64 - pos;
                        uint64_t exp;
                        if (rem >= numBits) {
                            exp =  (control[word] >> pos) &
                                                           Mask::lt64(numBits);
                        }
                        else {
                            ASSERT(word < SET_UP_ARRAY_DIM - 1);

                            exp =   control[word] >> pos; // & lt64(rem) unnec.
                            exp |= (control[word + 1] &
                                             Mask::lt64(numBits - rem)) << rem;
                        }

                        // bits

                        wordCpy(bits, control, sizeof(bits));
                        LOOP_ASSERT(ii, Util::bits(bits, idx, numBits) == exp);
                        LOOP_ASSERT(ii, 0 == wordCmp(bits, control,
                                                                sizeof(bits)));
                    }

                    // assign(... true ...)

                    wordCpy(bits, control, sizeof(bits));
                    Util::assign(bits, idx, true, numBits);
                    LOOP_ASSERT(ii, Util::areEqual(bits, 0, control, 0, idx));
                    LOOP_ASSERT(ii, Util::areEqual(bits, idx, ALL_TRUE, 0,
                                                                     numBits));
                    LOOP_ASSERT(ii, Util::areEqual(bits, idx + numBits,
                                                  control, idx + numBits,
                                                  NUM_BITS - idx - numBits));


                    // assign1

                    wordCpy(bitsB, control, sizeof(bits));
                    Util::assign1(bitsB, idx, numBits);
                    LOOP_ASSERT(ii, ! wordCmp(bitsB, bits, sizeof(bits)));

                    // assign(... false ...)

                    wordCpy(bits, control, sizeof(bits));
                    Util::assign(bits, idx, false, numBits);
                    LOOP_ASSERT(ii, Util::areEqual(bits, 0, control, 0, idx));
                    LOOP_ASSERT(ii, Util::areEqual(bits, idx, ALL_FALSE, 0,
                                                                     numBits));
                    LOOP_ASSERT(ii, Util::areEqual(bits, idx + numBits,
                                                  control, idx + numBits,
                                                  NUM_BITS - idx - numBits));

                    // assign0

                    wordCpy(bitsB, control, sizeof(bits));
                    Util::assign0(bitsB, idx, numBits);
                    LOOP_ASSERT(ii, ! wordCmp(bitsB, bits, sizeof(bits)));
                }
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::bits(bits, 0, 0));
            ASSERT_PASS(Util::bits(bits, 0, 1));
            ASSERT_PASS(Util::bits(bits, 0, 47));
            ASSERT_PASS(Util::bits(bits, 100, 10));
            ASSERT_PASS(Util::bits(bits, 100, k_BITS_PER_UINT64));
            ASSERT_FAIL(Util::bits(   0, 0, 0));
            ASSERT_FAIL(Util::bits(bits, 100, k_BITS_PER_UINT64 + 1));

            ASSERT_PASS(Util::assign(bits,  0, true,  0));
            ASSERT_PASS(Util::assign(bits,  0, true, 70));
            ASSERT_PASS(Util::assign(bits, 70, true, 70));
            ASSERT_FAIL(Util::assign(   0,  0, true,  0));

            ASSERT_PASS(Util::assign(bits,  0, false,  0));
            ASSERT_PASS(Util::assign(bits,  0, false, 70));
            ASSERT_PASS(Util::assign(bits, 70, false, 70));
            ASSERT_FAIL(Util::assign(   0,  0, false,  0));

            ASSERT_PASS(Util::assign0(bits,  0,  0));
            ASSERT_PASS(Util::assign0(bits,  0, 70));
            ASSERT_PASS(Util::assign0(bits, 70, 70));
            ASSERT_FAIL(Util::assign0(   0,  0,  0));

            ASSERT_PASS(Util::assign1(bits,  0,  0));
            ASSERT_PASS(Util::assign1(bits,  0, 70));
            ASSERT_PASS(Util::assign1(bits, 70, 70));
            ASSERT_FAIL(Util::assign1(   0,  0,  0));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SINGLE-BIT 'BIT', 'ASSIGN', 'ASSIGN0', AND 'ASSIGN1'
        //   Ensure 'bit' returns the expected values, and the 'assign*'
        //   methods have the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 That single-bit read and write operations function properly.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate, populating a buffer with 'setUpArray'.
        //:   o Iterate 'idx' over all values across the buffer.
        //:     1 Use bitwise C operations to read EXP, the 'idx'th bit of the
        //:       buffer.
        //:
        //:     2 Compare 'EXP' to a call to 'bit'.
        //:
        //:     3 Use 'assign' to assign a 'true' bit at 'idx' to buffer
        //:       'bits'.
        //:
        //:     4 Verify 'bits' is as expected.
        //:
        //:     5 Use 'assign0' to assign a false bit at 'idx' to buffer
        //:       'bitsB'.
        //:
        //:     6 Confirm 'bits' and 'bitsB' match.
        //:
        //:     7 Use 'assign' to assign a 'false' bit at 'idx' to buffer
        //:       'bits'.
        //:
        //:     8 Verify 'bits' is as expected.
        //:
        //:     9 Use 'assign1' to assign a 'true' bit at 'idx' to buffer
        //:       'bitsB'.
        //:
        //:     10 Confirm 'bits' and 'bitsB' match.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   bool bit(const uint64_t *bitString, St index);
        //   void assign(uint64_t *bitString, St index, bool value);
        //   void assign0(uint64_t *bitString, St index);
        //   void assign1(uint64_t *bitString, St index);
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTESTING SINGLE-BIT 'BIT', 'ASSIGN', 'ASSIGN0', AND 'ASSIGN1'\n"
              "============================================================\n";

        uint64_t BOOL[] = { 0, ~0ULL };

        const int NUM_BITS = SET_UP_ARRAY_DIM * k_BITS_PER_UINT64;

        uint64_t control[SET_UP_ARRAY_DIM], bits[SET_UP_ARRAY_DIM] = { 0 };
        uint64_t bitsB[  SET_UP_ARRAY_DIM] = { 0 };

        for (int ii = 0; ii < 150; ) {
            setUpArray(control, &ii);

            if (veryVerbose) {
                P_(ii);    P(pHex(control, NUM_BITS));
            }

            for (int idx = 0; idx < NUM_BITS; ++idx) {
                const int index = idx / k_BITS_PER_UINT64;
                const int pos   = idx % k_BITS_PER_UINT64;

                wordCpy(bits, control, sizeof(bits));

                bool EXP = control[index] & (1ULL << pos);

                LOOP2_ASSERT(ii, idx, EXP == Util::bit(bits, idx));

                ASSERT(0 == wordCmp(bits, control, sizeof(bits)));

                Util::assign(bits, idx, true);

                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, idx, &BOOL[1],0,1));
                LOOP2_ASSERT(ii, idx, (bits[index] & (1ULL << pos)));
                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, 0, control, 0,idx));
                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, idx+1, control,
                                               idx + 1, NUM_BITS - (idx + 1)));

                wordCpy(bitsB, control, sizeof(bitsB));

                Util::assign1(bitsB, idx);

                ASSERT(0 == wordCmp(bits, bitsB, sizeof(bits)));

                wordCpy(bits, control, sizeof(bits));

                Util::assign(bits, idx, false);

                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, idx, &BOOL[0],0,1));
                LOOP2_ASSERT(ii, idx, !(bits[index] & (1ULL << pos)));
                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, 0, control, 0,idx));
                LOOP2_ASSERT(ii, idx, Util::areEqual(bits, idx+1, control,
                                               idx + 1, NUM_BITS - (idx + 1)));

                wordCpy(bitsB, control, sizeof(bitsB));

                Util::assign0(bitsB, idx);

                ASSERT(0 == wordCmp(bits, bitsB, sizeof(bits)));
            }
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_PASS(Util::bit(bits, 0));
            ASSERT_SAFE_PASS(Util::bit(bits, 1));
            ASSERT_SAFE_FAIL(Util::bit(   0, 0));

            ASSERT_SAFE_PASS(Util::assign(bits, 0, true));
            ASSERT_SAFE_PASS(Util::assign(bits, 1, true));
            ASSERT_SAFE_PASS(Util::assign(bits, 10, true));
            ASSERT_SAFE_FAIL(Util::assign(   0, 0, true));

            ASSERT_SAFE_PASS(Util::assign(bits, 0, false));
            ASSERT_SAFE_PASS(Util::assign(bits, 1, false));
            ASSERT_SAFE_PASS(Util::assign(bits, 10, false));
            ASSERT_SAFE_FAIL(Util::assign(   0, 0, false));

            ASSERT_SAFE_PASS(Util::assign0(bits, 0));
            ASSERT_SAFE_PASS(Util::assign0(bits, 1));
            ASSERT_SAFE_PASS(Util::assign0(bits, 10));
            ASSERT_SAFE_FAIL(Util::assign0(   0, 0));

            ASSERT_SAFE_PASS(Util::assign1(bits, 0));
            ASSERT_SAFE_PASS(Util::assign1(bits, 1));
            ASSERT_SAFE_PASS(Util::assign1(bits, 10));
            ASSERT_SAFE_FAIL(Util::assign1(   0, 0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'areEqual'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 That 'areEqual' correctly compares bit ranges.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing.
        //:
        //: 2 Iterate, populating buffers 'lhs' and 'rhs' with two different
        //:   bit strings.
        //:
        //: 3 Iterate two indexes for the 'lhs' and 'rhs', respectively, and
        //:   iterate 'numBits' over the full range possible for that pair of
        //:   indexes.
        //:   o Use 'areEqualOracle', which is written in a simple, reliable,
        //:     but inefficient way, to determine whether the ranges in the
        //:     two buffers are equal.
        //:
        //:   o Call 'areEqual' and verify that the result matches the oracle.
        //:
        //:   o When '0 == lhsIdx' and '0 == rhsIdx', verify the result of the
        //:     3 argument 'areEqual' function.  (C-1)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values.  (C-2)
        //
        // Testing:
        //   bool areEqual(U64 *lhsBitString, U64 *rhsBitString, St numBits);
        //   bool areEqual(U64 *lBS, St lIdx, U64 *rBS, St rIdx, St nb);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'areEqual'"
                               << "\n==================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_lhsString_p;
            int         d_lhsIndex;
            const char *d_rhsString_p;
            int         d_rhsIndex;
            int         d_numBits;
            bool        d_expResult;
        } DATA [] = {
//  <<----<<
    // Line  LhsString          LI  RhsString          RI    NB      Result
    // ----  ---------          --  ---------          --    --      ------
    {   L_,  "",                0,  "",                0,    0,      true    },
    {   L_,  "",                0,  "1",               0,    0,      true    },
    {   L_,  "",                0,  "10",              0,    0,      true    },
    {   L_,  "",                0,  "10",              1,    0,      true    },
    {   L_,  "",                0,  "101",             0,    0,      true    },
    {   L_,  "",                0,  "101",             1,    0,      true    },
    {   L_,  "",                0,  "101",             2,    0,      true    },

    {   L_,  "1",               0,  "",                0,    0,      true    },
    {   L_,  "10",              0,  "",                0,    0,      true    },
    {   L_,  "10",              1,  "",                0,    0,      true    },
    {   L_,  "101",             0,  "",                0,    0,      true    },
    {   L_,  "101",             1,  "",                0,    0,      true    },
    {   L_,  "101",             2,  "",                0,    0,      true    },

    {   L_,  "1",               0,  "1",               0,    0,      true    },
    {   L_,  "10",              0,  "10",              1,    0,      true    },
    {   L_,  "10",              1,  "10",              0,    0,      true    },
    {   L_,  "101",             0,  "101",             0,    0,      true    },
    {   L_,  "101",             1,  "101",             2,    0,      true    },
    {   L_,  "101",             2,  "101",             1,    0,      true    },

    {   L_,  "1",               0,  "1",               0,    1,      true    },
    {   L_,  "0",               0,  "1",               0,    1,      false   },
    {   L_,  "10",              0,  "10",              0,    1,      true    },
    {   L_,  "10",              0,  "10",              1,    1,      false   },
    {   L_,  "10",              1,  "10",              0,    1,      false   },
    {   L_,  "10",              1,  "10",              1,    1,      true    },

    {   L_,  "10101",           0,  "11101",           0,    3,      true    },
    {   L_,  "10101",           1,  "11101",           0,    3,      false   },
    {   L_,  "10101",           2,  "11101",           0,    3,      true    },
    {   L_,  "10101",           0,  "11101",           1,    3,      false   },
    {   L_,  "10101",           0,  "11101",           2,    3,      false   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0,  0,  true   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  1,
             "11111 11111111 11111111 11111111 11111111",  1,  0,  true   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0,  1,  true   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  1,
             "11111 11111111 11111111 11111111 11111111",  0,  1,  true   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  1,  1,  true   },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0, 35,  true  },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  1,
             "11111 11111111 11111111 11111111 11111111",  0, 35,  true  },

    {   L_,  "11111 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  1, 35,  true  },

    //                                               v
    {   L_,  "11111 11111111 11111111 11111111 11111101",  0,
             "11111 11111111 11111111 11111111 11111111",  0,  0,   true  },

    //                                               v
    {   L_,  "11111 11111111 11111111 11111111 11111101",  1,
             "11111 11111111 11111111 11111111 11111111",  1,  0,   true  },

    //                                               v
    {   L_,  "11111 11111111 11111111 11111111 11111101",  0,
             "11111 11111111 11111111 11111111 11111111",  0,  1,   true  },

    //                                               v
    {   L_,  "11111 11111111 11111111 11111111 11111101",  1,
             "11111 11111111 11111111 11111111 11111111",  0,  1,   false },

    //                                               v
    {   L_,  "11111 11111111 11111111 11111111 11111101",  0,
             "11111 11111111 11111111 11111111 11111111",  1,  1,   true  },

    //              v
    {   L_,  "11111 01111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0, 31,   true  },

    //              v
    {   L_,  "11111 01111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0, 32,   false },

    //            v
    {   L_,  "11110 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  0, 33,   false },

    //            v
    {   L_,  "11110 11111111 11111111 11111111 11111111",  1,
             "11111 11111111 11111111 11111111 11111111",  0, 32,   false },

    //            v
    {   L_,  "11110 11111111 11111111 11111111 11111111",  0,
             "11111 11111111 11111111 11111111 11111111",  1, 33,   false },

    {
      L_,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      63,
      true
    },
    {
      L_,
//     v
 "111 00111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      63,
      false
    },
    {
      L_,
//     v
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "111 00111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      63,
      false
    },
    {
      L_,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
      63,
      false
    },
    {
      L_,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      63,
      false
    },
    {
      L_,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
 "111 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
      63,
      true
    },

    {
      L_,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      64,
      true
    },
    {
      L_,
//    v
 "110 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      64,
      false
    },
    {
      L_,
//    v
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      64,
      false
    },
    {
      L_,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
      64,
      false
    },
    {
      L_,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      64,
      false
    },
    {
      L_,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      1,
      64,
      true
    },

    {
      L_,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      65,
      true
    },
    {
      L_,
//  v
 "111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      65,
      false
    },
    {
      L_,
//  v
 "110 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
 "111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111",
      0,
      65,
      false
    },
//  >>---->>
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *LSTR  = DATA[i].d_lhsString_p;
            const int   LI    = DATA[i].d_lhsIndex;
            const char *RSTR  = DATA[i].d_rhsString_p;
            const int   RI    = DATA[i].d_rhsIndex;
            const int   NB    = DATA[i].d_numBits;
            const bool  EXP   = DATA[i].d_expResult;

            if (veryVerbose) {
                P(LINE) P_(LSTR) P(LI) P_(RSTR) P(RI)
                P_(NB) P(EXP)
            }

            const int MAX_ARRAY_SIZE = 4;

            uint64_t lhs[MAX_ARRAY_SIZE] = { 0 };
            uint64_t rhs[MAX_ARRAY_SIZE] = { 0 };

            populateBitString(lhs, 0, LSTR);
            populateBitString(rhs, 0, RSTR);

            LOOP_ASSERT(LINE, EXP == Util::areEqual(lhs, LI, rhs, RI, NB));
        }

        const int NUM_BITS = 3 * k_BITS_PER_UINT64;

        uint64_t lhs[SET_UP_ARRAY_DIM] = { 0 }, lhsCopy[SET_UP_ARRAY_DIM];
        uint64_t rhs[SET_UP_ARRAY_DIM] = { 0 }, rhsCopy[SET_UP_ARRAY_DIM];

        for (int ii = 0, jj = 20; ii < 200; ) {
            setUpArray(lhs, &ii, true);
            setUpArray(rhs, &jj, true);

            if (veryVerbose) {
                P_(ii);    P(pHex(lhs, NUM_BITS));
                P_(jj);    P(pHex(rhs, NUM_BITS));
            }

            wordCpy(lhsCopy, lhs, sizeof(lhs));
            wordCpy(rhsCopy, rhs, sizeof(rhs));

            const int maxIdx = NUM_BITS - 1;
            for (int lhsIdx = 0; lhsIdx <= maxIdx; incInt(&lhsIdx, maxIdx)) {
                for (int rhsIdx = 0; rhsIdx <= maxIdx;
                                                     incInt(&rhsIdx, maxIdx)) {
                    const int maxNumBits = NUM_BITS - bsl::max(rhsIdx, lhsIdx);
                    for (int numBits = 0; numBits <= maxNumBits; ++numBits) {
                        const bool EXP = areEqualOracle(lhs, lhsIdx,
                                                         rhs, rhsIdx, numBits);
                        if (0 == (lhsIdx | rhsIdx)) {
                            ASSERT(EXP == Util::areEqual(lhs, rhs, numBits));
                        }

                        ASSERTV(pHex(lhs, NUM_BITS) + pHex(rhs, NUM_BITS),
                                                  lhsIdx, rhsIdx, numBits, EXP,
                             Util::areEqual(lhs, lhsIdx, rhs, rhsIdx, numBits),
                                EXP == Util::areEqual(lhs, lhsIdx, rhs, rhsIdx,
                                                                     numBits));
                    }
                }
            }

            ASSERT(0 == wordCmp(lhsCopy, lhs, sizeof(lhs)));
            ASSERT(0 == wordCmp(rhsCopy, rhs, sizeof(rhs)));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(Util::areEqual(lhs, 0, rhs, 0, 0));
            ASSERT_PASS(Util::areEqual(lhs, 0, rhs, 0, 1));
            ASSERT_PASS(Util::areEqual(lhs, 0, rhs, 0, SET_UP_ARRAY_DIM));
            ASSERT_PASS(Util::areEqual(lhs, 1, rhs, 0, SET_UP_ARRAY_DIM - 1));
            ASSERT_PASS(Util::areEqual(lhs, 0, rhs, 1, SET_UP_ARRAY_DIM - 1));
            ASSERT_FAIL(Util::areEqual(  0, 0, rhs, 0, 0));
            ASSERT_FAIL(Util::areEqual(lhs, 0,   0, 0, 0));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'populateBitString' & 'populateBitStringHex' FUNCTIONS
        //   Ensure the methods have the right effect on 'bitString'.
        //
        // Concerns:
        //: 1 The helper functions populate the bit array according to the
        //:   input.
        //
        // Plan:
        //: 1 Do table-driven testing of the binary function by using the
        //:   function to populate a buffer, and comparing the buffer to a
        //:   buffer in the table.
        //:
        //: 2 Do table-driven testing of the hex function by using the function
        //:   to populate a buffer, and comparing the buffer to a buffer in
        //:   the table.  (C-1)
        //
        // Testing:
        //   void populateBitString(U64 *bitString, St idx, char *ascii);
        //   void populateBitStringHex(U64 *bitString, St idx, char *ascii);
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "TESTING 'populateBitString' & 'populateBitStringHex' FUNCTIONS\n"
            "==============================================================\n";

        {
            const int MAX_RESULT_SIZE = 2 ;

            const struct {
                int         d_line;
                const char *d_inputString_p;
                int         d_index;
                uint64_t    d_expResult[MAX_RESULT_SIZE];
            } DATA [] = {
//      <<<<<<<<
        // Line  InputString       Index                      Expected
        // ----  -----------       -----                      --------
        {   L_,  "",                  0,   {          0,          0 }},
        {   L_,  "",                  1,   {          0,          0 }},
        {   L_,  "",                 31,   {          0,          0 }},
        {   L_,  "",                 32,   {          0,          0 }},
        {   L_,  "",                 63,   {          0,          0 }},
        {   L_,  "",                 64,   {          0,          0 }},
        {   L_,  "",                 95,   {          0,          0 }},

        {   L_, "1",                  0,   {          1,          0 }},
        {   L_, "1",                  1,   {          2,          0 }},
        {   L_, "1",                 31,   { 0x80000000,          0 }},
        {   L_, "1",                 32,   { 1ULL << 32,          0 }},
        {   L_, "1",                 63,   { 1ULL << 63,          0 }},
        {   L_, "1",                 64,   {          0,          1 }},
        {   L_, "1",                 95,   {          0, 0x80000000 }},

        {   L_, "10",                 0,   {          2,          0 }},
        {   L_, "10",                 1,   {          4,          0 }},
        {   L_, "10",                30,   { 0x80000000,          0 }},
        {   L_, "10",                31,   { 1ULL << 32,          0 }},
        {   L_, "10",                32,   { 1ULL << 33,          0 }},
        {   L_, "10",                62,   { 1ULL << 63,          0 }},
        {   L_, "10",                63,   {          0,          1 }},
        {   L_, "10",                64,   {          0,          2 }},
        {   L_, "10",                94,   {          0, 0x80000000 }},

        {   L_, "10101",              0,   {       0x15,          0 }},
        {   L_, "10101",              1,   {       0x2A,          0 }},
        {   L_, "10101",             29,   { 0x2A0000000ULL,          0 }},
        {   L_, "10101",             30,   { 0x540000000ULL,          0 }},
        {   L_, "10101",             31,   { 0xA80000000ULL,          0 }},
        {   L_, "10101",             32,   {  0x15ULL << 32,          0 }},
        {   L_, "10101",             62,   { 1ULL << 62,          5 }},
        {   L_, "10101",             63,   { 1ULL << 63,        0xA }},
        {   L_, "10101",             64,   {          0,       0x15 }},
        {   L_, "10101",             91,   {          0, 0xA8000000 }},

        {   L_, "1111 00001111 00001111 00001111 00001111",
                                  0,   {  0xF0F0F0F0FULL,                0 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                  2,   { 0x3C3C3C3C3CULL,                0 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 15,   { 0x7878787878000ULL,             0 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 30,   { 0xC3C3C3C3C0000000ULL,          3 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 31,   { 0x8787878780000000ULL,          7 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 32,   { 0x0F0F0F0F00000000ULL,        0xF }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 48,   { 0x0F0F000000000000ULL,    0xF0F0F }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 55,   { 0x8780000000000000ULL,  0x7878787 }},
        {   L_, "1111 00001111 00001111 00001111 00001111",
                                 60,   { 0xF000000000000000ULL, 0xF0F0F0F0 }},
//      >>>>>>>>
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int        LINE      = DATA[i].d_line;
                const char      *INPUT_STR = DATA[i].d_inputString_p;
                const int        INDEX     = DATA[i].d_index;
                const uint64_t  *EXP       = DATA[i].d_expResult;

                uint64_t actual[MAX_RESULT_SIZE] = { 0 };

                if (veryVerbose) {
                    P_(LINE) P_(INPUT_STR) P(INDEX)
                    P_(EXP[0]) P_(EXP[1]) P_(EXP[2])
                }

                populateBitString(actual, INDEX, INPUT_STR);
                LOOP_ASSERT(LINE, 0 == wordCmp(actual, EXP,
                                                               sizeof actual));
            }
        }

        {
            const int MAX_RESULT_SIZE = 2 ;

            const struct {
                int         d_line;
                const char *d_inputString_p;
                int         d_index;
                uint64_t    d_expResult[MAX_RESULT_SIZE];
            } DATA [] = {
//      <<<<<<<<
        // Line  InputString       Index                      Expected
        // ----  -----------       -----                      --------
        {   L_,  "",                  0,   {          0,          0 }},
        {   L_,  "",                  1,   {          0,          0 }},
        {   L_,  "",                 31,   {          0,          0 }},
        {   L_,  "",                 32,   {          0,          0 }},
        {   L_,  "",                 63,   {          0,          0 }},
        {   L_,  "",                 64,   {          0,          0 }},
        {   L_,  "",                 95,   {          0,          0 }},

        {   L_, "1",                  0,   {          1,          0 }},
        {   L_, "1",                  1,   {          2,          0 }},
        {   L_, "1",                 31,   { 0x80000000,          0 }},
        {   L_, "1",                 32,   { 1ULL << 32,          0 }},
        {   L_, "1",                 63,   { 1ULL << 63,          0 }},
        {   L_, "1",                 64,   {          0,          1 }},
        {   L_, "1",                 95,   {          0, 0x80000000 }},

        {   L_, "2",                  0,   {          2,          0 }},
        {   L_, "2",                  1,   {          4,          0 }},
        {   L_, "2",                 30,   { 0x80000000,          0 }},
        {   L_, "2",                 31,   { 1ULL << 32,          0 }},
        {   L_, "2",                 32,   { 1ULL << 33,          0 }},
        {   L_, "2",                 62,   { 1ULL << 63,          0 }},
        {   L_, "2",                 63,   {          0,          1 }},
        {   L_, "2",                 64,   {          0,          2 }},
        {   L_, "2",                 94,   {          0, 0x80000000 }},

        {   L_, "15",                 0,   {       0x15,          0 }},
        {   L_, "15",                 1,   {       0x2A,          0 }},
        {   L_, "15",                29,   { 0x2A0000000ULL,          0 }},
        {   L_, "15",                30,   { 0x540000000ULL,          0 }},
        {   L_, "15",                31,   { 0xA80000000ULL,          0 }},
        {   L_, "15",                32,   {  0x15ULL << 32,          0 }},
        {   L_, "15",                62,   { 1ULL << 62,          5 }},
        {   L_, "15",                63,   { 1ULL << 63,        0xA }},
        {   L_, "15",                64,   {          0,       0x15 }},
        {   L_, "15",                91,   {          0, 0xA8000000 }},

        {   L_, "f0f0f0f0f",      0,   {  0xF0F0F0F0FULL,                0 }},
        {   L_, "f0f0f0f0f",      2,   { 0x3C3C3C3C3CULL,                0 }},
        {   L_, "f0f0f0f0f",     15,   { 0x7878787878000ULL,             0 }},
        {   L_, "f0f0f0f0f",     30,   { 0xC3C3C3C3C0000000ULL,          3 }},
        {   L_, "f0f0f0f0f",     31,   { 0x8787878780000000ULL,          7 }},
        {   L_, "f0f0f0f0f",     48,   { 0x0F0F000000000000ULL,    0xF0F0F }},
        {   L_, "f0f0f0f0f",     55,   { 0x8780000000000000ULL,  0x7878787 }},
        {   L_, "f0f0f0f0f",     60,   { 0xF000000000000000ULL, 0xF0F0F0F0 }},

        {   L_, "5Wf",            0,   { 0xFFFFFFFFFFFFFFFFULL,          5 }},
        {   L_, "5Hf",            0,   {        0x5FFFFFFFFULL,          0 }},
        {   L_, "5qf",            0,   {            0X5FFFFULL,          0 }},
        {   L_, "5yf",            0,   {              0x5FFULL,          0 }},

        {   L_, "5WfH0",          0,   { 0xFFFFFFFF00000000ULL,
                                                            0x5FFFFFFFFULL }},
        {   L_, "5HfH0q0",        0,   { 0xFFFF000000000000ULL, 0x5FFFFULL }},
        {   L_, "5qfH0q0y0",      0,   { 0xFF00000000000000ULL,   0X5FFULL }},
        {   L_, "5yfH0q0y00",     0,   { 0xF000000000000000ULL,    0x5FULL }},

        {   L_, "5WfH0",          2,   { 0xFFFFFFFC00000000ULL,
                                                            0x17FFFFFFFFULL }},
        {   L_, "5HfH0q0",        2,   { 0xFFFC000000000000ULL, 0x17FFFFULL }},
        {   L_, "5qfH0q0y0",      2,   { 0xFC00000000000000ULL,   0X17FFULL }},
        {   L_, "5yfH0q0y00",     2,   { 0xC000000000000000ULL,    0x17FULL }},

        {   L_, "5HfHq0",         2,   { 0xFFFC000000000000ULL, 0x17FFFFULL }},
        {   L_, "5qfHqy0",        2,   { 0xFC00000000000000ULL,   0X17FFULL }},
        {   L_, "5yfHqy00",       2,   { 0xC000000000000000ULL,    0x17FULL }},

        {   L_, "5WaH0",          2,   { 0xaaaaaaa800000000ULL,
                                                            0x16aaaaaaaaULL }},
        {   L_, "5HaH0q0",        2,   { 0xAAA8000000000000ULL, 0x16AAAAULL }},
        {   L_, "5qaH0q0y0",      2,   { 0xA800000000000000ULL,   0X16AAULL }},
        {   L_, "5yaH0q0y00",     2,   { 0x8000000000000000ULL,    0x16AULL }},

        {   L_, "5HaHq0",         2,   { 0xAAA8000000000000ULL, 0x16AAAAULL }},
        {   L_, "5qaHqy0",        2,   { 0xA800000000000000ULL,   0X16AAULL }},
        {   L_, "5yaHqy00",       2,   { 0x8000000000000000ULL,    0x16AULL }},
//      >>>>>>>>
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };
            const int NUM_BITS = MAX_RESULT_SIZE * k_BITS_PER_UINT64;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int        LINE      = DATA[i].d_line;
                const char      *INPUT_STR = DATA[i].d_inputString_p;
                const int        INDEX     = DATA[i].d_index;
                const uint64_t  *EXP       = DATA[i].d_expResult;

                uint64_t actual[MAX_RESULT_SIZE] = { 0 };

                if (veryVerbose) {
                    P_(LINE) P_(INPUT_STR) P(INDEX)
                    P_(EXP[0]) P_(EXP[1]) P_(EXP[2])
                }

                populateBitStringHex(actual, INDEX, INPUT_STR);
                LOOP3_ASSERT(LINE, pHex(actual, NUM_BITS), pHex(EXP, NUM_BITS),
                                     0 == wordCmp(actual, EXP, sizeof actual));

                // Now check for splash to bits that should have been
                // unaffected.

                for (int jj = 0; jj < 8; ++jj) {
                    const int end = INDEX + 4 * numHexDigits(INPUT_STR);

                    uint64_t control[MAX_RESULT_SIZE];
                    fillWithGarbage(control, sizeof(control));
                    wordCpy(actual, control, sizeof(actual));

                    populateBitStringHex(actual, INDEX, INPUT_STR);
                    ASSERT(areBitsEqual(actual, control, 0, INDEX));
                    ASSERT(areBitsEqual(actual, control, end, NUM_BITS - end));
                }
            }
        }

        if (veryVerbose) P(k_ALIGNMENT);
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

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
