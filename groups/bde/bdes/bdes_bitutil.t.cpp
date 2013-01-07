// bdes_bitutil.t.cpp                                                 -*-C++-*-

#include <bdes_bitutil.h>

#include <bslmf_assert.h>

#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

#include <bsl_cctype.h>      // isspace()
#include <bsl_climits.h>     // INT_MIN, INT_MAX
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// Exploit sign-extension to simulate leading 1's and 0's portably.
// Loop over the number of bits per word, where that makes sense.  Use a
// special generator function g(spec) that recognizes up to one fill pattern
// of the form 0..0 or 1..1 to create integers of unspecified length more
// easily.  Note that the behavior of shifting the number of bits per word on
// a integer is undefined.  The general category partitioning is based on
// boundary cases such as 0, 1, BITS_PER_WORD - 1, and BITS_PER_WORD.
// Note that it was necessary to break 'main' up into separate files because
// of unacceptably long build times on windows.
//-----------------------------------------------------------------------------
// [ 3] enum { WORD_SIZE = sizeof(int) };
// [ 3] enum { BITS_PER_BYTE = 8 };
// [ 3] enum { BITS_PER_WORD = BITS_PER_BYTE * WORD_SIZE };
// [14] void andEqual(int *dInt, int dIdx, int boolVal)
// [14] void andEqual64(bsls_Types::Int64 *dInt, int dIdx, int boolVal)
// [27] void andEqual(int *dInt, int dIdx, int sInt, int nBits)
// [27] void andEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                      bsls_Types::Int64 sInt, int nBits)
// [27] void andEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits)
// [27] void andEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                      bsls_Types::Int64 sInt, int sIdx, int nBits)
// [ 4] int eqMask(int index)
// [ 4] bsls_Types::Int64 eqMask64(int index)
// [ 7] int extractZero(int sInt, int sIdx, int nBits)
// [ 7] bsls_Types::Int64 extractZero64(
//                          bsls_Types::Int64 sInt, int sIdx, int nBits)
// [26] int extractZero(int sInt, int sIdx, int nBits, int offset)
// [26] bsls_Types::Int64 extractZero64(bsls_Types::Int64 sInt,
//                                             int sIdx, int nBits, int offset)
// [ 7] int extractOne(int sInt, int sIdx, int nBits)
// [ 7] bsls_Types::Int64 extractOne64(bsls_Types::Int64 sInt,
//                                            int sIdx, int nBits)
// [26] int extractOne(int sInt, int sIdx, int nBits, int offset)
// [26] bsls_Types::Int64 extractOne64(bsls_Types::Int64 sInt,
//                                            int sIdx, int nBits, int offset)
// [32] int find0AtLargestIndex(int srcInteger)
// [32] int find0AtLargestIndex64(bsls_Types::Int64 srcInteger)
// [32] int find1AtLargestIndex(int srcInteger)
// [32] int find1AtLargestIndex64(bsls_Types::Int64 srcInteger)
// [32] int find0AtSmallestIndex(int srcInteger)
// [32] int find0AtSmallestIndex64(bsls_Types::Int64 srcInteger)
// [32] int find1AtSmallestIndex(int srcInteger)
// [32] int find1AtSmallestIndex64(bsls_Types::Int64 srcInteger)
// [33] int find0AtLargestIndexGE(int srcInteger, int srcIndex)
// [33] int find0AtLargestIndexGE64(bsls_Types::Int64 srcInteger, int srcIndex)
// [33] int find0AtLargestIndexGT(int srcInteger, int srcIndex)
// [33] int find0AtLargestIndexGT64(bsls_Types::Int64 srcInteger, int srcIndex)
// [33] int find0AtLargestIndexLE(int srcInteger, int srcIndex)
// [33] int find0AtLargestIndexLE64(bsls_Types::Int64 srcInteger, int srcIndex)
// [33] int find0AtLargestIndexLT(int srcInteger, int srcIndex)
// [33] int find0AtLargestIndexLT64(bsls_Types::Int64 srcInteger, int srcIndex)
// [34] int find1AtLargestIndexGE(int srcInteger, int srcIndex)
// [34] int find1AtLargestIndexGE64(bsls_Types::Int64 srcInteger, int srcIndex)
// [34] int find1AtLargestIndexGT(int srcInteger, int srcIndex)
// [34] int find1AtLargestIndexGT64(bsls_Types::Int64 srcInteger, int srcIndex)
// [34] int find1AtLargestIndexLE(int srcInteger, int srcIndex)
// [34] int find1AtLargestIndexLE64(bsls_Types::Int64 srcInteger, int srcIndex)
// [34] int find1AtLargestIndexLT(int srcInteger, int srcIndex)
// [34] int find1AtLargestIndexLT64(bsls_Types::Int64 srcInteger, int srcIndex)
// [35] int find0AtSmallestIndexGE(int srcInteger, int srcIndex)
// [35] int find0AtSmallestIndexGE64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [35] int find0AtSmallestIndexGT(int srcInteger, int srcIndex)
// [35] int find0AtSmallestIndexGT64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [35] int find0AtSmallestIndexLE(int srcInteger, int srcIndex)
// [35] int find0AtSmallestIndexLE64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [35] int find0AtSmallestIndexLT(int srcInteger, int srcIndex)
// [35] int find0AtSmallestIndexLT64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [36] int find1AtSmallestIndexGE(int srcInteger, int srcIndex)
// [36] int find1AtSmallestIndexGE64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [36] int find1AtSmallestIndexGT(int srcInteger, int srcIndex)
// [36] int find1AtSmallestIndexGT64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [36] int find1AtSmallestIndexLE(int srcInteger, int srcIndex)
// [36] int find1AtSmallestIndexLE64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [36] int find1AtSmallestIndexLT(int srcInteger, int srcIndex)
// [36] int find1AtSmallestIndexLT64(bsls_Types::Int64 srcInteger,
//                                                                int srcIndex)
// [ 4] int geMask(int index)
// [ 4] bsls_Types::Int64 geMask(int index)
// [ 4] int gtMask(int index)
// [ 4] bsls_Types::Int64 gtMask(int index)
// [10] void insertZero(int *dInt, int dIdx, int nBits)
// [10] void insertZero64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [10] void insertOne(int *dInt, int dIdx, int nBits)
// [10] void insertOne64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [25] void insertValue(int *dInt, int dIdx, int sInt, int nBits)
// [25] void insertValue(bsls_Types::Int64 *dInt, int dIdx,
//                       bsls_Types::Int64 sInt, int nBits)
// [21] void insertValueUpTo(int *dInt, int nBits, int sInt)
// [21] void insertValueUpTo64(bsls_Types::Int64 *dInt, int nBits,
//                             bsls_Types::Int64 sInt)
// [ 8] int isAnySetZero(int sInt)
// [ 8] int isAnySetZero64(bsls_Types::Int64 sInt)
// [ 8] int isAnySetOne(int sInt)
// [ 8] int isAnySetOne64(bsls_Types::Int64 sInt)
// [23] int isSetZero(int sInt, int sIdx)
// [23] int isSetZero64(bsls_Types::Int64 sInt, int sIdx)
// [23] int isSetOne(int sInt, int sIdx)
// [23] int isSetOne64(bsls_Types::Int64 sInt, int sIdx)
// [ 4] int leMask(int index)
// [ 4] bsls_Types::Int64 leMask64(int index)
// [ 4] int ltMask(int index)
// [ 4] bsls_Types::Int64 ltMask64(int index)
// [ 5] int zeroMask(int index, int nBits)
// [ 5] bsls_Types::Int64 maskZero64(int index, int nBits)
// [ 5] int oneMask(int index, int nBits)
// [ 5] bsls_Types::Int64 maskOne64(int index, int nBits)
// [17] void minusEqual(int *dInt, int dIdx, int boolVal)
// [17] void minusEqual64(bsls_Types::Int64 *dInt, int dIdx, int boolVal)
// [30] void minusEqual(int *dInt, int dIdx, int sInt, int nBits)
// [30] void minusEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                        bsls_Types::Int64 sInt, int nBits)
// [30] void minusEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits)
// [30] void minusEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                        bsls_Types::Int64 sInt, int sIdx, int nBits)
// [ 4] int neMask(int index)
// [ 4] bsls_Types::Int64 neMask64(int index)
// [24] int numSetZero(int sInt)
// [24] int numSetZero64(bsls_Types::Int64 sInt)
// [24] int numSetOne(int sInt)
// [24] int numSetOne64(bsls_Types::Int64 sInt)
// [15] void orEqual(int *dInt, int dIdx, int boolVal)
// [15] void orEqual64(bsls_Types::Int64 *dInt, int dIdx, int boolVal)
// [28] void orEqual(int *dInt, int dIdx, int sInt, int nBits)
// [28] void orEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                     bsls_Types::Int64 sInt, int nBits)
// [28] void orEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits)
// [28] void orEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                     bsls_Types::Int64 sInt, int sIdx, int nBits)
// [30] void print(ostream& output, int sInt)
// [ 6] int rangeZero(int sInt, int sIdx, int nBits)
// [ 6] bsls_Types::Int64 rangeZero64(bsls_Types::Int64 sInt,
//                                           int sIdx, int nBits)
// [ 6] int rangeOne(int sInt, int sIdx, int nBits)
// [ 6] bsls_Types::Int64 rangeOne64(bsls_Types::Int64 sInt,
//                                          int sIdx, int nBits)
// [12] void removeZero(int *dInt, int dIdx, int nBits)
// [12] void removeZero64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [12] void removeOne(int *dInt, int dIdx, int nBits)
// [12] void removeOne64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [ 9] void replaceZero(int *dInt, int dIdx, int nBits)
// [ 9] void replaceZero64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [ 9] void replaceOne(int *dInt, int dIdx, int nBits)
// [ 9] void replaceOne64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [19] void replaceBitZero(int *dInt, int dIdx)
// [19] void replaceBitZero64(bsls_Types::Int64 *dInt, int dIdx)
// [19] void replaceBitOne(int *dInt, int dIdx)
// [19] void replaceBitOne64(bsls_Types::Int64 *dInt, int dIdx)
// [20] void replaceBitValue(int *dInt, int dIdx, int sInt)
// [20] void replaceBitValue64(bsls_Types::Int64 *dInt, int dIdx, int sInt)
// [22] void replaceValue(int *dInt, int dIdx, int sInt, int nBits)
// [22] void replaceValue64(bsls_Types::Int64 *dInt, int dIdx,
//                          int sInt, int nBits)
// [18] void replaceValueUpTo(int *dInt, int dIdx, int sInt)
// [18] void replaceValueUpTo64(bsls_Types::Int64 *dInt, int dIdx, int sInt)
// [ 9] int setZero(int sInt, int sIdx, int nBits)
// [ 9] int setZero64(bsls_Types::Int64 sInt, int sIdx, int nBits)
// [ 9] int setOne(int sInt, int sIdx, int nBits)
// [ 9] int setOne64(bsls_Types::Int64 sInt, int sIdx, int nBits)
// [19] int setBitZero(int sInt, int sIdx)
// [19] int setBitZero64(bsls_Types::Int64 sInt, int sIdx)
// [19] int setBitOne(int sInt, int sIdx)
// [19] int setBitOne64(bsls_Types::Int64 sInt, int sIdx)
// [20] int setBitValue(int sInt, int sIdx, int value)
// [20] int setBitValue64(bsls_Types::Int64 sInt, int sIdx, int value)
// [22] int setValue(int sInt, int sIdx, int value, int nBits)
// [22] int setValue(bsls_Types::Int64 sInt, int sIdx,
//                   bsls_Types::Int64 value, int nBits)
// [18] int setValueUpTo(int sInt, int sIdx, int value)
// [18] int setValueUpTo(bsls_Types::Int64 sInt, int sIdx, int value)
// [11] void toggle(int *dInt, int dIdx, int nBits)
// [11] void toggle64(bsls_Types::Int64 *dInt, int dIdx, int nBits)
// [13] void toggleBit(int *dInt, int dIdx)
// [13] void toggleBit64(bsls_Types::Int64 *dInt, int dIdx)
// [16] void xorEqual(int *dInt, int dIdx, int boolVal)
// [16] void xorEqual64(bsls_Types::Int64 *dInt, int dIdx, int boolVal)
// [29] void xorEqual(int *dInt, int dIdx, int sInt, int nBits)
// [29] void xorEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                      bsls_Types::Int64 sInt, int nBits)
// [29] void xorEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits)
// [29] void xorEqual64(bsls_Types::Int64 *dInt, int dIdx,
//                      bsls_Types::Int64 sInt, int sIdx, int nBits)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST GENERATOR FUNCTION: int g(const char *spec)
// [ 3] QUICK REFERENCE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
        aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\t" << #M << ": " << M << "\n"; \
        aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\t" << #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
        aSsErT(1, #X, __LINE__); } }

#define LOOP7_ASSERT(I,J,K,L,M,N,P,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\t" << #M << ": " << M << "\t" << #N << ": " << N \
        << "\t" << #P << ": " << P << "\n"; \
        aSsErT(1, #X, __LINE__); } }

//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdes_BitUtil       Util;

typedef bsls_Types::Int64  Int64;
typedef bsls_Types::Uint64 Uint64;

const static Uint64 uint64Max = (Uint64) -1;
const static Int64  int64Max  = (Int64) (uint64Max / 2);
const static Int64  int64Min  = -int64Max - 1;

enum { BITS_PER_WORD  = 8 * sizeof(int),   BPW = BITS_PER_WORD };
enum { BITS_PER_INT64 = 8 * sizeof(Int64), BPS = BITS_PER_INT64 };

// Hex Bit Strings
#define x0_ "0000"
#define x5_ "0101"
#define xA_ "1010"
#define xF_ "1111"

// Longer Bit Strings
#define B8_0 x0_ x0_
#define B8_1 xF_ xF_
#define B8_01 x5_ x5_
#define B8_10 xA_ xA_
#define B12_0 B8_0 x0_
#define B12_1 B8_1 xF_
#define B16_0 B8_0 B8_0
#define B16_1 B8_1 B8_1
#define B16_01 B8_01 B8_01
#define B16_10 B8_10 B8_10
#define B32_0 B16_0 B16_0
#define B32_1 B16_1 B16_1
#define B32_01 B16_01 B16_01
#define B32_10 B16_10 B16_10

// Platform-Dependent Bit Strings
#if INT_MAX == 0x7fffffff
/*REFERENCED*/
struct Assertions {
    char assertion[4 == sizeof(int)];  // 32-bit architecture
    char int64Min_assertion[
                 (long long) ((unsigned long long) int64Min - 1) > int64Min];
};
#define EW_0 x0_
#define EW_1 xF_
#define FW_01 B32_01
#define FW_10 B32_10
#define SW_01 B32_01 B32_01
#define SW_10 B32_10 B32_10
#define MID_0   " 00000000 00000000 "
#define MID_1   " 11111111 11111111 "
#define MID_01  " 01010101 01010101 "
#define MID_10  " 10101010 10101010 "
#else
/*REFERENCED*/
static char assertion[8 == sizeof(int)]; // assume 64-bit architecture
#define EW_0 B8_0
#define EW_1 B8_1
#define FW_01 B32_01 B32_01
#define FW_10 B32_10 B32_10
#define SW_01 B32_01 B32_01 B32_01 B32_01
#define SW_10 B32_10 B32_10 B32_10 B32_10
#define MID_0   " 00000000 00000000 00000000 00000000 00000000 00000000 "
#define MID_1   " 11111111 11111111 11111111 11111111 11111111 11111111 "
#define MID_01  " 01010101 01010101 01010101 01010101 01010101 01010101 "
#define MID_10  " 10101010 10101010 10101010 10101010 10101010 10101010 "
#endif

// Word Relative Bit Strings
#define QW_0 EW_0 EW_0
#define QW_1 EW_1 EW_1
#define HW_0 QW_0 QW_0
#define HW_1 QW_1 QW_1
#define FW_0 HW_0 HW_0
#define FW_1 HW_1 HW_1
#define SW_0 FW_0 FW_0
#define SW_1 FW_1 FW_1

//=============================================================================
//                          GLOBAL VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

//=============================================================================
//        GENERATOR FUNCTION 'int g(const char *spec)' FOR TESTING
//-----------------------------------------------------------------------------
// The following function interprets the given 'spec' in order from left to
// right to configure an integer according to a custom language.  Valid
// meaningful characters are the binary digits ('0' and '1') and a period
// ('.') used to indicate a sequence (e.g., "0..0" or "1..1").  At most one
// sequence may appear in a single spec.  Space characters are ignored; all
// other characters are invalid.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>                    ; ""
//                | <DIGIT_LIST>               ; "1100"
//                | <ITEM_LIST>                ; "1110..0"
//                | <ITEM_LIST><DIGIT_LIST>    ; "1110..01100"
//
// <ITEM_LIST>  ::= <ITEM>                     ; item without leading digits
//                | <DIGIT_LIST><ITEM>         ; item with leading digits
//
// <DIGIT_LIST> ::= <DIGIT>                    ; digit list of length 1
//                | <DIGIT_LIST><DIGIT>        ; digit list of length > 1
//
// <ITEM>       ::= 0..0 | 1..1                ; 0.., 1..0, ..1 are illegal
//
// <DIGIT>      ::= 0 | 1
//
// <EMPTY>      ::=                            ; ignore all whitespace
//
// Spec String          VALUE   Description
// -----------          -----   --------------------------------------------
// ""                       0   default is 0's
// "   "                    0   white space is ignored
// " 1 "                    1   one
// " 10"                    2   two
// "110"                    6   six
// "0..0"                   0   fill up with 0's
// "0..0110"                6   fill up with leading 0's
// "1..1"                  -1   fill up with 1's
// "1..10"                 -2   fill up with leading 1's
// "1 .\t. 1 0 1"          -3   white space is ignored
// "11..1"                 -1   1 followed by trailing 1's
// "01..1"            INT_MAX   0 followed by trailing 1's
// "10..0"            INT_MIN   1 followed by trailing 0's
//
// "a"                  error   bad character
// "0..1"               error   left and right fill value must match
// "..1"                error   missing left fill value
// "0.."                error   missing right fill value
// "1..11..1"           error   at most one fill item per spec
// "11111111..1111111"  error   if number of digits exceeds BITS_PER_WORD
//
//-----------------------------------------------------------------------------
//                      Helper Functions for 'g'
//-----------------------------------------------------------------------------

inline void setBits(int *integer, int mask, int booleanValue)
    // Set each bit in the specified 'integer' at positions corresponding
    // to '1'-bits in the specified 'mask' to the specified booleanValue.
{
    if (booleanValue) {
        *integer |= mask;
    }
    else {
        *integer &= ~mask;
    }
}

inline void setLSB(int *integer, const char *endOfSpec, int charCount)
    // Set the specified 'charCount' least significant bits in the specified
    // 'integer' to the bit pattern corresponding to '0' and '1' characters in
    // the 'charCount' characters *preceding* the specified 'endOfSpec',
    // leaving all other bits of 'integer' unaffected.  Note that endOfSpec[-1]
    // corresponds to the least significant bit of 'integer'.
{
    const int start = -charCount;
    int mask = 1;
    for (int i = -1; i >= start; --i) {
        char ch = endOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            setBits(integer, mask, '1' == ch);
            mask <<= 1;
        }
    }
}

inline void setMSB(int *integer, const char *startOfSpec, int charCount)
    // Set the specified 'charCount' most significant bits in the specified
    // 'integer' to the bit pattern corresponding to '0' and '1' characters in
    // the 'charCount' characters starting at the specified 'startOfSpec',
    // leaving all other bits of 'integer' unaffected.  Note that endOfSpec[0]
    // corresponds to the most significant bit of 'integer'.
{
    unsigned int mask = (unsigned)(1 << (BITS_PER_WORD - 1));
    for (int i = 0; i < charCount; ++i) {
        char ch = startOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            setBits(integer, mask, '1' == ch);
            mask >>= 1;
        }
    }
}

static int G_OFF = 0;  // set to 1 only to enable testing of G function errors

enum {
    G_ILLEGAL_CHARACTER     = 1001,
    G_MISMATCHED_RANGE      = 1002,
    G_MISSING_RANGE_END     = 1003,
    G_MISSING_RANGE_START   = 1004,
    G_MISSING_SECOND_DOT    = 1005,
    G_MULTIPLE_RANGES       = 1006,
    G_TOO_MANY_BITS         = 1007
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static int g(const char *spec)
    // Return an integer value corresponding the the specified 'spec' as
    // defined above using none of the functions defined in the component
    // under test.
{
    int bitCount = 0;           // total number of bits encountered
    int lastBitIndex = -1;      // index of last bit encountered

    int rangeStartIndex = -1;   // index of  first D in D..D
    int rangeEndIndex = -1;     // index of second D in D..D

    int i;                      // indicates length of spec after loop
    for (i = 0; spec[i]; ++i) {
        switch (spec[i]) {
          case '0':
          case '1': {
            ++bitCount;
            lastBitIndex = i;
          } break;
          case '.': {
            if (rangeStartIndex != -1) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Multiple Ranges");
                return G_MULTIPLE_RANGES;
            }
            if (0 == bitCount) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Range Start");
                return G_MISSING_RANGE_START;
            }
            while (isspace(spec[++i])) {
                // skip white space
            }
            if ('.' != spec[i]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Second Dot");
                return G_MISSING_SECOND_DOT;
            }
            while (isspace(spec[++i])) {
                // skip white space
            }
            if ('0' != spec[i] && '1' != spec[i]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Range End");
                return G_MISSING_RANGE_END;
            }
            if (spec[i] != spec[lastBitIndex]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Nonmatching Range");
                return G_MISMATCHED_RANGE;
            }

            // Found valid range; record index of beginning and of end.
            rangeStartIndex = lastBitIndex;
            rangeEndIndex = i;
            --bitCount;
          } break;
          default: {
            if (!isspace(spec[i])) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Illegal Character");
                return G_ILLEGAL_CHARACTER;
            }
          } break;
        }
    }

    if (bitCount > BITS_PER_WORD) {
        LOOP2_ASSERT(bitCount, BITS_PER_WORD, G_OFF || !"Too Many Bits");
        return G_TOO_MANY_BITS;
    }

    int result;     // value to be returned

    if (rangeStartIndex != -1) {
        result = '1' == spec[rangeStartIndex] ? ~0 : 0;
        setMSB(&result, spec, rangeStartIndex);
        setLSB(&result, spec + i, i - 1 - rangeEndIndex);
    }
    else {
        result = 0;
        setLSB(&result, spec + i, i);
    }

    return result;
}

//
//-----------------------------------------------------------------------------
//                      Helper Functions for 'g64'
//-----------------------------------------------------------------------------
inline void setBits64(Int64 *integer, Int64 mask, int booleanValue)
    // Set each bit in the specified 'integer' at positions corresponding
    // to '1'-bits in the specified 'mask' to the specified booleanValue.
{
    if (booleanValue) {
        *integer |= mask;
    }
    else {
        *integer &= ~mask;
    }
}

inline void setLSB64(Int64 *integer, const char *endOfSpec, int charCount)
    // Set the specified 'charCount' least significant bits in the specified
    // 'integer' to the bit pattern corresponding to '0' and '1' characters in
    // the 'charCount' characters *preceding* the specified 'endOfSpec',
    // leaving all other bits of 'integer' unaffected.  Note that endOfSpec[-1]
    // corresponds to the least significant bit of 'integer'.
{
    const int start = -charCount;
    Int64 mask = 1;
    for (int i = -1; i >= start; --i) {
        char ch = endOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            setBits64(integer, mask, '1' == ch);
            mask <<= 1;
        }
    }
}

inline void setMSB64(Int64 *integer, const char *startOfSpec, int charCount)
    // Set the specified 'charCount' most significant bits in the specified
    // 'integer' to the bit pattern corresponding to '0' and '1' characters in
    // the 'charCount' characters starting at the specified 'startOfSpec',
    // leaving all other bits of 'integer' unaffected.  Note that endOfSpec[0]
    // corresponds to the most significant bit of 'integer'.
{
    Uint64 mask = ((Uint64) 1 << (BITS_PER_INT64 - 1));
    for (int i = 0; i < charCount; ++i) {
        char ch = startOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            setBits64(integer, mask, '1' == ch);
            mask >>= 1;
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static Int64 g64(const char *spec)
    // Return an integer value corresponding the the specified 'spec' as
    // defined above using none of the functions defined in the component
    // under test.
{
    int bitCount = 0;           // total number of bits encountered
    int lastBitIndex = -1;      // index of last bit encountered

    int rangeStartIndex = -1;   // index of  first D in D..D
    int rangeEndIndex = -1;     // index of second D in D..D

    int i;                      // indicates length of spec after loop
    for (i = 0; spec[i]; ++i) {
        switch (spec[i]) {
          case '0':
          case '1': {
            ++bitCount;
            lastBitIndex = i;
          } break;
          case '.': {
            if (rangeStartIndex != -1) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Multiple Ranges");
                return G_MULTIPLE_RANGES;
            }
            if (0 == bitCount) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Range Start");
                return G_MISSING_RANGE_START;
            }
            while (isspace(spec[++i])) {
                // skip white space
            }
            if ('.' != spec[i]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Second Dot");
                return G_MISSING_SECOND_DOT;
            }
            while (isspace(spec[++i])) {
                // skip white space
            }
            if ('0' != spec[i] && '1' != spec[i]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Missing Range End");
                return G_MISSING_RANGE_END;
            }
            if (spec[i] != spec[lastBitIndex]) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Nonmatching Range");
                return G_MISMATCHED_RANGE;
            }

            // Found valid range; record index of beginning and of end.
            rangeStartIndex = lastBitIndex;
            rangeEndIndex = i;
            --bitCount;
          } break;
          default: {
            if (!isspace(spec[i])) {
                LOOP2_ASSERT(i, spec[i], G_OFF || !"Illegal Character");
                return G_ILLEGAL_CHARACTER;
            }
          } break;
        }
    }

    if (bitCount > BITS_PER_INT64) {
        LOOP2_ASSERT(bitCount, BITS_PER_INT64, G_OFF || !"Too Many Bits");
        return G_TOO_MANY_BITS;
    }

    Int64 result;     // value to be returned

    if (rangeStartIndex != -1) {
        result = '1' == spec[rangeStartIndex] ? ~ (Int64) 0 : (Int64) 0;
        setMSB64(&result, spec, rangeStartIndex);
        setLSB64(&result, spec + i, i - 1 - rangeEndIndex);
    }
    else {
        result = 0;
        setLSB64(&result, spec + i, i);
    }

    return result;
}

//=============================================================================
//        Helper Functions for Testing 'bdes_BitUtil' Searching Functions
//-----------------------------------------------------------------------------
// These functions perform operations that are identical to 'bdes_BitUtil'
// methods, but using a completely independent implementation.

static long long roundUpToBinaryPower(Uint64 srcInteger)
{
    // Return the value of 'bdes_BitUtil::roundUpToBinaryPower(srcInteger)'
    // computing it by a different algorithm
    if (0 == srcInteger) {
        return 0;
    }
    Uint64 i = 1;
    for (; i && i < srcInteger; i <<= 1) {
        ;
    }
    return i;
}

static int countOneBits(Uint64 srcInteger)
{
    int ret = 0;
    for (Uint64 mask = 1; mask; mask <<= 1) {
        if (mask & srcInteger) {
            ++ret;
        }
    }
    return ret;
}

static int base2Log(Uint64 srcInteger)
{
    return countOneBits(roundUpToBinaryPower(srcInteger)-1);
}

static int findFirstBit1(Int64 srcInteger)
    // Return the value of 'bdes_BitUtil::find1AtLargestIndex(srcInteger)'
    // but use a completely independent implementation.
{
    for (int i = BPS - 1; i >= 0; --i) {
        if (srcInteger & ((Int64) 1 << i))
            return i;
    }
    return -1;
}

static int findLastBit1(Int64 srcInteger)
    // Return the value of 'bdes_BitUtil::find1AtSmallestIndex(srcInteger)'
    // but use a completely independent implementation.
{
    for (int i = 0; i < BPS; ++i) {
        if (srcInteger & ((Int64) 1 << i))
            return i;
    }
    return Util::BDES_BITS_PER_INT64;
}

//-----------------------------------------------------------------------------

static void performanceTest() {
    if (verbose) cout << endl
                      << "PERFORMANCE TEST" << endl
                      << "================" << endl;

    enum { MAX_ITER = 10* 1000 * 1000 };
    bsls_Stopwatch timer;
    int x = 0;

    if (verbose) cout << "\tRaw loop (no-op)" << endl
                      << "\t----------------" << endl;
    timer.start();
    // The loop below is carefully crafted to exercise numbers with any
    // from 0 to 30 significant bits.  It will be faster for large numbers
    // and slower for small numbers.
    for (int i = 0, n = 0; i < MAX_ITER; ++i, n += 17) {
        x += n;
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    if (verbose) cout << "\tnumSet1" << endl
                      << "\t-------" << endl;
    timer.reset();
    timer.start();
    // The loop below is carefully crafted to exercise numbers with any
    // from 0 to 30 significant bits.  It will be faster for large numbers
    // and slower for small numbers.
    for (int i = 0, n = 0; i < MAX_ITER; ++i, n += 17) {
        x += Util::numSetOne(n);
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    if (verbose) cout << "\tfind1AtLargestIndex for int" << endl
                      << "\t------------------------------" << endl;
    timer.reset();
    timer.start();
    // The loop below is carefully crafted to exercise numbers with any
    // from 0 to 30 significant bits.  It will be faster for large numbers
    // and slower for small numbers.
    for (int i = 0, n = 0; i < MAX_ITER; ++i, n += 17) {
        x += Util::find1AtLargestIndex(n);
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    if (verbose) cout << "\tfind0AtSmallestIndex for int" << endl
                      << "\t------------------------------" << endl;
    timer.reset();
    timer.start();
    for (int i = 0, n = 0; i < MAX_ITER; ++i, n += 17) {
        x += Util::find0AtSmallestIndex(n);
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    if (verbose) cout << "\tfind1AtLargestIndex for long long" << endl
                      << "\t------------------------------------" << endl;
    timer.reset();
    timer.start();
    // The numbers below are to exercise a broad section of the code in the
    // utility: at first, the utility will take longer (for small numbers)
    // but as the numbers grow, it will take less time.  Overall, we will
    // have an average picture.
    long long nn = 0;
    for (int i = 0; i < MAX_ITER; ++i, nn += 3685477) {
        x += Util::find1AtLargestIndex64(nn);
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    if (verbose) cout << "\tfind0AtSmallestIndex for long long" << endl
                      << "\t------------------------------------" << endl;
    timer.reset();
    timer.start();
    // Now we will have the inverse behavior.
    nn = 0;
    for (int i = 0; i < MAX_ITER; ++i, nn += 3685477) {
        x += Util::find0AtSmallestIndex64(nn);
    }
    timer.stop();
    cout << MAX_ITER << " iterations: " << timer.elapsedTime() << endl;

    ASSERT(0 != x);

    timer.reset();
    timer.start();

    const int delta = 3685477;
    volatile register int n = 0;

    {
        // time eqMask versus a shift-free loop
        volatile register int y = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;

            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
        }
        timer.stop();
        cout << "eqMask empty loop: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        ASSERT(0 != y);

        timer.reset();
        timer.start();

        // time eqMask versus a shift-free loop
        for (int i = MAX_ITER; i; --i, n += delta) {
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;

            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
            y += ++n & 31;
        }
        timer.stop();

        cout << "eqMask empty loop: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        ASSERT(0 != y);

        timer.reset();
        timer.start();

        // time eqMask versus a shift
        y = 0;
        n = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);

            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
            y += Util::eqMask(++n & 31);
        }
        timer.stop();
        cout << "eqMask: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        timer.reset();
        timer.start();
        n = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);

            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
            y -= 1 << (++n & 31);
        }
        timer.stop();
        cout << "shift: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        ASSERT(0 == y);

        timer.reset();
        timer.start();
    }

    {
        // time 64 bit eqMask versus a shift-free loop
        volatile register Int64 yy = 0;
        const long long zero = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            int nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;

            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
            nchop = ++n & 31;
            yy += nchop;
        }
        timer.stop();
        cout << "64 bit eqMask empty loop: " << MAX_ITER <<
                            " iterations: " << timer.elapsedTime() << endl;
        ASSERT(0 != yy);

        timer.reset();
        timer.start();
        // time eqMask64 versus a long long shift
        yy = 0;
        n = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            int nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);

            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
            nchop = ++n & 63;
            yy += Util::eqMask64(nchop);
        }
        timer.stop();
        cout << "eqMask64: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        timer.reset();
        timer.start();
        n = 0;
        for (int i = MAX_ITER; i; --i, n += delta) {
            int nchop = ++n & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));

            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
            nchop = ++nn & 63;
            yy -= ((Int64) 1 << nchop) & (zero - ((nchop < 64)&(nchop >= 0)));
        }
        timer.stop();
        cout << "shift64: " << MAX_ITER << " iterations: " <<
                                               timer.elapsedTime() << endl;

        ASSERT(0 == yy);
    }
}

static void domainTest() {
#if 0
    unsigned u;
    unsigned long long ull;
    int i;
    long long ll;

    // shifts by large amount generate a lot of compiler warnings

    u = 1;
    u <<= -1;
    printf("u << -1 = 0x%x\n", u);
    u = 1;
    u <<= 32;
    printf("u << 32 = 0x%x\n", u);

    u = 1;
    u <<= -2;
    printf("u << -2 = 0x%x\n", u);
    u = 1;
    u <<= 33;
    printf("u << 33 = 0x%x\n", u);

    ull = 1;
    u <<= -1;
    printf("ull << -1 = 0x%llx\n", ull);
    ull = 1;
    u <<= 64;
    printf("ull << 64 = 0x%llx\n", ull);

    i = 1;
    i <<= -1;
    printf("i << -1 = 0x%x\n", i);
    i = 1;
    i <<= 32;
    printf("i << 32 = 0x%x\n", i);

    i = 1;
    i <<= -2;
    printf("i << -2 = 0x%x\n", i);
    i = 1;
    i <<= 33;
    printf("i << 33 = 0x%x\n", i);

    ll = 1;
    ll <<= -1;
    printf("ll << -1 = 0x%llx\n", ll);
    ll = 1;
    ll <<= 64;
    printf("ll << 64 = 0x%llx\n", ll);
#endif
}

static int naivemerge(int zeroSrc, int oneSrc, int mask)
{
    int lhs = zeroSrc & ~mask;
    int rhs = oneSrc  &  mask;
    return lhs | rhs;
}

static Int64 naivemerge64(Int64 zeroSrc, Int64 oneSrc, Int64 mask)
{
    return (zeroSrc & ~mask) | (oneSrc & mask);
}

static
void testUsage()
{
    //-------------------------------------------------------------------------
    // TESTING USAGE EXAMPLE
    //
    // Concerns:
    //   That the usage example from the .h file is correct.
    //-------------------------------------------------------------------------

    if (verbose) cout << "Testing Usage Example\n"
                         "=====================\n";

///Mask Generators
///- - - - - - - -
// 'geMask' will return a bit mask with all bits below the specified 'index'
// cleared, and all bits at or above the 'index' set:
//..
    ASSERT((int) 0xffff0000 == bdes_BitUtil::geMask(16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::geMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits at and above bit 16 are set:  11111111111111110000000000000000 |
//  +-------------------------------------------------------------------------+
//..
// Similarly, 'ltMask' return a bit mask with all bits above the specified
// 'index' cleared, and all bits at or below 'index' set:
//..
    ASSERT((int) 0x0000ffff == bdes_BitUtil::ltMask(16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::ltMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits at and below bit 16 are set:  00000000000000001111111111111111 |
//  +-------------------------------------------------------------------------+
//..
// 'eqMask' return a bit mask with only the bit at the specified 'index' set:
//..
    ASSERT((int) 0x00800000 == bdes_BitUtil::eqMask(23));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::eqMask(23)' in binary:                                   |
//  |                                                                         |
//  | bit 23:                                        *                        |
//  | Only bit 23 is set:                    00000000100000000000000000000000 |
//  +-------------------------------------------------------------------------+
//..
// Similarly, 'neMask' return a bit mask with only the bit at the specified
// 'index' cleared:
//..
    ASSERT((int) 0xfffeffff == bdes_BitUtil::neMask(16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::neMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits other than bit 16 are set:    11111111111111101111111111111111 |
//  +-------------------------------------------------------------------------+
//..
// Finally, 'maskOne' and 'maskZero' return a bit mask with either all bits
// within a specified 'range' starting from a specified 'index' set, or
// cleared, respectively:
//..
    ASSERT((int) 0x000f0000 == bdes_BitUtil::oneMask(16, 4));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::oneMask(16, 4)' in binary:                               |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits set:           00000000000011110000000000000000 |
//  +-------------------------------------------------------------------------+
//
    ASSERT((int) 0xfff0ffff == bdes_BitUtil::zeroMask(16, 4));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::zeroMask(16, 4)' in binary:                              |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits cleared:       11111111111100001111111111111111 |
//  +-------------------------------------------------------------------------+
//..
///Manipulators
/// - - - - - -
// 'andEqual' ands a given bit or set of bits with a scalar that it is to
// operate on:
//..
    int myInt = 0x3333;
    bdes_BitUtil::andEqual(&myInt, 9, false);
    ASSERT(0x3133 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::andEqual(&myInt, 9, false)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:                                11001100110011 |
//  | bit 9:                                                       *          |
//  | and Equal(&myInt, 9, false) clears bit 9:                    0          |
//  | 'myInt' 'after in binary:                                11000100110011 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x3333;
    bdes_BitUtil::andEqual(&myInt, 9, true);
    ASSERT(0x3333 == myInt);

    myInt = 0x3333;
    bdes_BitUtil::andEqual(&myInt, 8,  0, 8);
    ASSERT(  0x33 == myInt);

    myInt = 0x3333;
    bdes_BitUtil::andEqual(&myInt, 8, -1, 8);    // Note '-1' is all '1's.
    ASSERT(0x3333 == myInt);
//..
// 'insertOne' and 'insertZero' will split a number open and insert '1's or
// '0's in the middle of it:
//..
    myInt = 0x3333;
    bdes_BitUtil::insertOne( &myInt, 4, 8);
    ASSERT(0x333ff3 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::insertOne( &myInt, 4, 8)' in binary:                     |
//  |                                                                         |
//  | 'myInt' before in binary:                                11001100110011 |
//  | shift bits starting at 4 left 8 to make room:    1100110011        0011 |
//  | insert 8 1's:                                              11111111     |
//  | 'myInt' after in binary:                         1100110011111111110011 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x3333;
    bdes_BitUtil::insertZero(&myInt, 4, 8);
    ASSERT(0x333003 == myInt);
//..
// 'orEqual' will take a slice of a second integer 'srcInteger' and bitwise
// or it with another integer:
//..
    myInt = 0x33333333;
    bdes_BitUtil::orEqual(&myInt, 16, -1, 8);
    ASSERT((int) 0x33ff3333 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::orEqual(&myInt, 16, -1, 8)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:              00110011001100110011001100110011 |
//  | 'srcInteger == -1' in binary:          11111111111111111111111111111111 |
//  | logical or the 8 bits of 'srcInteger'                                   |
//  | at index 16:                                   11111111                 |
//  | 'myInt' after in binary:               00110011111111110011001100110011 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0;
    bdes_BitUtil::orEqual(&myInt, 16, -1, 8);
    ASSERT((int) 0x00ff0000 == myInt);
//..
// 'removeOne' and 'removeZero' will remove bits from the middle of a number.
// 'removeOne' will fill in the vacated high-order bits with '1's, 'removeZero'
// will fill them in with '0's:
//..
    myInt = 0x12345678;
    bdes_BitUtil::removeOne(&myInt, 8, 16);
    ASSERT((int) 0xffff1278 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::removeOne(&myInt, 8, 16)' in binary:                     |
//  |                                                                         |
//  | 'myInt' before in binary:              00010010001101000101011001111000 |
//  | remove 16 bits starting at bit 8:      00010010                01111000 |
//  | replace empty bits with upper bits:                    0001001001111000 |
//  | fill all vacated top bits with 1's:    1111111111111111                 |
//  | 'myInt' after in binary:               11111111111111110001001001111000 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x12345678;
    bdes_BitUtil::removeZero(&myInt, 8, 16);
    ASSERT((int) 0x00001278 == myInt);

    myInt = 0;
    bdes_BitUtil::removeOne(&myInt, 8, 16);
    ASSERT((int) 0xffff0000 == myInt);
//..
// 'toggle' will toggle one bit or a set of bits:
//..
    myInt = 0x11111111;
    bdes_BitUtil::toggle(&myInt, 8, 16);
    ASSERT((int) 0x11eeee11 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::toggle(&myInt, 8, 16)' in binary:                        |
//  |                                                                         |
//  | 'myInt' before in binary:              00010001000100010001000100010001 |
//  | toggle 16 bits starting at bit 8:              ****************         |
//  | 'myInt' after in binary:               00010001111011101110111000010001 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x55555555;
    bdes_BitUtil::toggle(&myInt, 8, 16);
    ASSERT((int) 0x55aaaa55 == myInt);
//..
// 'xorEqual' will xor one bit with a scalar:
//..
    myInt = 0x11111111;
    bdes_BitUtil::xorEqual(&myInt, 16, true);
    ASSERT((int) 0x11101111 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::xorEqual(&myInt, 16, true)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:              00010001000100010001000100010001 |
//  | xor bit 16:                                           1                 |
//  | 'myInt' after in binary:               00010001000100000001000100010001 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x11111111;
    bdes_BitUtil::xorEqual(&myInt, 16, false);
    ASSERT((int) 0x11111111 == myInt);
//..
// Another interface with 'xorEqual' will take a section of a second scalar
// and xor it with a first scalar:
//..
    myInt = 0x77777777;
    bdes_BitUtil::xorEqual(&myInt, 16, 0xff, 8);
    ASSERT((int) 0x77887777 == myInt);
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::xorEqual(&myInt, 16, 0xff, 8)' in binary:                |
//  |                                                                         |
//  | 'myInt' before in binary:              01110111011101110111011101110111 |
//  | bits to affect: 8 bits at offset 16:           ********                 |
//  | value to put in those 8 bits is 0xff:          11111111                 |
//  | xor those 8 bits, leaving other bits unaffected                         |
//  | 'myInt' after in binary:               01110111100010000111011101110111 |
//  +-------------------------------------------------------------------------+
//
    myInt = 0x77777777;
    bdes_BitUtil::xorEqual(&myInt, 16, 0x55, 8);
    ASSERT((int) 0x77227777 == myInt);
//..
///Accessors
///- - - - -
// The 'find[01]At(Largest,Smallest)*' routines are used for finding the
// highest (or lowest) set (or clear) bit in a scalar, possibly within a
// subset of the integer:
//..
    ASSERT( 8 == bdes_BitUtil::find1AtLargestIndex(  0x00000101));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtLargestIndex(  0x00000101)' in binary:            |
//  |                                                                         |
//  | input:                                 00000000000000000000000100000001 |
//  | highest set bit:                                              1         |
//  | index of highest set bit == 8                                           |
//  +-------------------------------------------------------------------------+
//
    ASSERT( 8 == bdes_BitUtil::find0AtLargestIndex( ~0x00000101));
    ASSERT( 8 == bdes_BitUtil::find0AtLargestIndex(  0xfffffe55));
    ASSERT( 8 == bdes_BitUtil::find1AtLargestIndexGE(0x00000101,  3));
    ASSERT(-1 == bdes_BitUtil::find1AtLargestIndexGE(0x00000155, 16));
    ASSERT(-1 == bdes_BitUtil::find1AtLargestIndexLT(0xffff0100,  3));

    ASSERT( 8 == bdes_BitUtil::find1AtLargestIndexLT(0xffff0100, 16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtLargestIndexLT(0xffff0100, 16)' in binary:        |
//  |                                                                         |
//  | input:                                 11111111111111110000000100000000 |
//  | bit 16:                                               *                 |
//  | range to look for highest bit in:                      **************** |
//  | highest set bit in that range:                                1         |
//  | index of that bit is 8                                                  |
//  +-------------------------------------------------------------------------+
//
    ASSERT( 8 == bdes_BitUtil::find1AtSmallestIndex(  0xffff0100));
    ASSERT(16 == bdes_BitUtil::find1AtSmallestIndexGE(0xffff0100, 10));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtSmallestIndexGE(0xffff0100, 10)' in binary:       |
//  |                                                                         |
//  | input:                                 11111111111111110000000100000000 |
//  | bit 10:                                                     *           |
//  | range to look for lowest bit in:       **********************           |
//  | lowest set bit in that range:                         1                 |
//  | index of that bit is 16                                                 |
//  +-------------------------------------------------------------------------+
//
    ASSERT( 8 == bdes_BitUtil::find1AtSmallestIndex(0xffff0100));
//..
// 'intAbs', 'intMax', and 'intMin' are faster than 'bsl::abs', 'bsl::max',
// 'bsl::min', and are not template functions:
//..
    ASSERT(      6 == bdes_BitUtil::intAbs( 6));
    ASSERT(      6 == bdes_BitUtil::intAbs(-6));
    ASSERT(INT_MAX == bdes_BitUtil::intAbs(-INT_MAX));
    ASSERT(INT_MIN == bdes_BitUtil::intAbs( INT_MIN));    // see note below

    ASSERT(12 == bdes_BitUtil::intMax(-5, 12));
    ASSERT(-5 == bdes_BitUtil::intMin(-5, 12));
//..
// Note that 'INT_MIN' is a special case.  Since the maximum representable
// positive integer, 'INT_MAX', is one less the absolute value of 'INT_MIN',
// 'bdes_BitUtil::intAbs(INT_MIN)' cannot be represented as a positive integer.
// Therefore, the function has no effect and returns 'INT_MIN'.
//
// 'merge' takes 3 numbers, 'zeroSrcInteger', 'onsSrcInteger', and 'mask'.
// For each bit of output, it looks at the corresponding bit of 'mask' -- if
// it's a '0', it takes the corresponding bit of 'zeroSrcInteger', if it's '1',
// it takes the corresponding bit of 'oneSrcInteger'.
//..
    ASSERT((int) 0x0f0f0f0f ==
                      bdes_BitUtil::merge(0x0f0f0000, 0xffff0f0f, 0x0000ffff));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::merge(0x0f0f0000, 0xffff0f0f, 0x0000ffff))' in binary:   |
//  |                                                                         |
//  | zeroSrcInteger:                        00001111000011110000000000000000 |
//  | oneSrcInteger:                         11111111111111110000111100001111 |
//  |                                                                         |
//  | mask:                                  00000000000000001111111111111111 |
//  |                                                                         |
//  | bits from zeroSrcInteger used:         0000111100001111                 |
//  | bits from oneSrcInteger used:                          0000111100001111 |
//  | result:                                00001111000011110000111100001111 |
//  +-------------------------------------------------------------------------+
//
    ASSERT((int) 0x55555555 ==
                      bdes_BitUtil::merge(0x00000000, 0xffffffff, 0x55555555));
//..
// 'numSetOne' and 'numSetZero' count the number of set (or clear) bits:
//..
    ASSERT(16 == bdes_BitUtil::numSetOne( 0x55555555));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::numSetOne( 0x55555555)' in binary:                       |
//  |                                                                         |
//  | input in binary:                       01010101010101010101010101010101 |
//  | that has 16 ones set.  result: 16                                       |
//  +-------------------------------------------------------------------------+
//
    ASSERT(16 == bdes_BitUtil::numSetOne( 0xffff0000));
    ASSERT(16 == bdes_BitUtil::numSetOne( 0x0000ffff));
    ASSERT( 1 == bdes_BitUtil::numSetOne( 0x00010000));
    ASSERT(31 == bdes_BitUtil::numSetZero(0x00010000));
//..
// 'rangeZero' and 'rangeOne' mask out a sub-range of bits from a value,
// filling the rest with '0's or '1's:
//..
    ASSERT((int) 0x00333300 == bdes_BitUtil::rangeZero(0x33333333, 8, 16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::rangeZero(0x33333333,  8, 16)' in binary:                |
//  |                                                                         |
//  | input in binary:                       00110011001100110011001100110011 |
//  | select 16 bits starting at bit 8:              ****************         |
//  | all other bits 0:                      00000000                00000000 |
//  | result:                                00000000001100110011001100000000 |
//  +-------------------------------------------------------------------------+
//
    ASSERT((int) 0x00f0f000 == bdes_BitUtil::rangeZero(0xf0f0f0f0, 8, 16));
    ASSERT((int) 0xff3333ff == bdes_BitUtil::rangeOne (0x33333333, 8, 16));
//..
// 'base2Log' takes a binary log of a number, rounded up to the next integer:
//..
    ASSERT(7 == bdes_BitUtil::base2Log(100));
    ASSERT(7 == bdes_BitUtil::base2Log(128));
    ASSERT(8 == bdes_BitUtil::base2Log(129));
//..
// 'roundUpToBinaryPower' rounds a number up to the next power of 2:
//..
    ASSERT(128 == bdes_BitUtil::roundUpToBinaryPower(100));
    ASSERT(128 == bdes_BitUtil::roundUpToBinaryPower(128));
//..
// 'setBitOne' will set a bit (or bits) to one, leaving other bits unaffected:
//..
    ASSERT((int) 0x66676666 == bdes_BitUtil::setBitOne(0x66666666, 16));
//
//  +-------------------------------------------------------------------------+
//  | 'bdes_BitUtil::setBitOne(0x66666666, 16)' in binary:                    |
//  |                                                                         |
//  | srcInteger in binary:                  01100110011001100110011001100110 |
//  | set bit 16:                                           1                 |
//  | result:                                01100110011001110110011001100110 |
//  +-------------------------------------------------------------------------+
//
    ASSERT((int) 0x00010000 == bdes_BitUtil::setBitOne(0, 16));

    ASSERT((int) 0x33ffff33 == bdes_BitUtil::setOne(0x33333333, 8, 16));
//..
// 'sizeInBits' will return the size of an object in bits, rather than bytes:
//..
    char              thisChar   = 0;
    short             thisShort  = 0;
    int               thisInt    = 0;
    bsls_Types::Int64 thisBigInt = 0;

    ASSERT( 8 == bdes_BitUtil::sizeInBits(thisChar));
    ASSERT(16 == bdes_BitUtil::sizeInBits(thisShort));
    ASSERT(32 == bdes_BitUtil::sizeInBits(thisInt));
    ASSERT(64 == bdes_BitUtil::sizeInBits(thisBigInt));
//..
}

static
void testH(int test)
{
    switch (test) {
      case 0:  // Zero is always the leading case.
      case 40: {
        // --------------------------------------------------------------------
        // TESTING 'merge'
        //
        // Concerns:
        //   That the 'merge' function works as expected.
        //
        // Plan:
        //   Create a 'naivemerge' function that works by a simpler algorithm
        //   swapping 'zeroSrc' and 'oneSrc' does not change the result if
        //   one also complements 'mask'.
        //
        // Testing:
        //   int merge(int zeroSrc, int oneSrc, int mask);
        //   Int64 merge64(Int64 zeroSrc, Int64 oneSrc, Int64 mask)
        // --------------------------------------------------------------------

        const int VALUES_A[] = { 0, 1, 2, 3, 4, 7, 8, 17, 27, 93, 127, 128,
                                 129, 254, 255, 256, 257, 258, 347, 749, 1000,
                                 10001, 10002, 123456, 7654321, 87654321,
                                 987654321, 1987654321, 2123456789, INT_MAX-2,
                                 INT_MAX-1, INT_MAX, INT_MIN };
        enum { NUM_VALUES_A = sizeof VALUES_A / sizeof *VALUES_A };

        for (int vix = -NUM_VALUES_A+1; vix < NUM_VALUES_A; ++vix) {
            int vx = vix < 0 ? -VALUES_A[-vix] : VALUES_A[vix];
            for (int viy = -NUM_VALUES_A+1; viy < NUM_VALUES_A; ++viy) {
                int vy = viy < 0 ? -VALUES_A[-viy] : VALUES_A[viy];
                for (int vim = -NUM_VALUES_A+1; vim < NUM_VALUES_A; ++vim) {
                    int mask = vim < 0 ? -VALUES_A[-vim] : VALUES_A[vim];

                    ASSERT(naivemerge(vx, vy, mask) ==
                                                    Util::merge(vx, vy, mask));
                    ASSERT(Util::merge(vx, vy, mask) ==
                                                    Util::merge(vy, vx,~mask));
                }
            }
        }

        const Int64 VALUES_B[] = { 0, 1, 2, 3, 4, 7, 8, 17, 27, 93, 127, 128,
                                 129, 254, 255, 256, 257, 258, 347, 749, 1000,
                                 10001, 10002, 123456, 7654321, 87654321,
                                 987654321, 1987654321, 2123456789, INT_MAX-2,
                                 INT_MAX-1, INT_MAX, INT_MIN, 0x100000000LL,
                                 0x100000001LL, 0x100000002LL, 0x100000010LL,
                                 0x311111111LL, 0x312345678LL, 0xfffffffffLL,
                                 0x1fffffffffLL, 0x7f0f0f0f0fLL,
                                 0xff0f0f0f0fLL, 0x38394837218LL,
                                 0xa38394837218LL, 0xa38394837218cLL,
                                 0x72938172637437LL, 0x9182718294ade7eLL,
                                 0xa8c7d8e7f98429fLL, 0xba8c7d8e7f98429fLL,
                                 int64Max-1, int64Max, int64Min };
        enum { NUM_VALUES_B = sizeof VALUES_B / sizeof *VALUES_B };

        for (int vix = -NUM_VALUES_A+1; vix < NUM_VALUES_A; ++vix) {
            Int64 vx = vix < 0 ? -VALUES_A[-vix] : VALUES_A[vix];
            for (int viy = -NUM_VALUES_A+1; viy < NUM_VALUES_A; ++viy) {
                Int64 vy = viy < 0 ? -VALUES_A[-viy] : VALUES_A[viy];
                for (int vim = -NUM_VALUES_A+1; vim < NUM_VALUES_A; ++vim) {
                    Int64 mask = vim < 0 ? -VALUES_A[-vim] : VALUES_A[vim];

                    ASSERT(naivemerge64(vx, vy, mask) ==
                                                  Util::merge64(vx, vy, mask));
                    ASSERT(Util::merge64(vx, vy, mask) ==
                                                 Util::merge64(vy, vx, ~mask));
                }
            }
        }
      } break;
      case 39: {
        // --------------------------------------------------------------------
        // TESTING 'intAbs', 'intMax', 'intMin'
        //
        // Concerns:
        //   That 'intAbs', 'intMax', and 'intMin' all work as specced.
        //
        // Plan:
        //   Compare results of these to their bsl counterpart and verify
        //   the results are the same.  In the case of 64 bit abs, no bsl
        //   counterpart is available, so calculate it with an expression.
        //
        // Testing:
        //   int intAbs(int srcInteger);
        //   Int64 intAbs64(Int64 srcInteger)
        //   int intMax(int srcIntegerX, int srcIntegerY);
        //   Int64 intMax(Int64 srcIntegerX, Int64 srcIntegerY);
        //   int intMin(int srcIntegerX, int srcIntegerY);
        //   Int64 intMin64(Int64 srcIntegerX, Int64 srcIntegerY);
        // --------------------------------------------------------------------

        const int VALUES_A[] = { 0, 1, 2, 3, 4, 7, 8, 17, 27, 93, 127, 128,
                                 129, 254, 255, 256, 257, 258, 347, 749, 1000,
                                 10001, 10002, 123456, 7654321, 87654321,
                                 987654321, 1987654321, 2123456789, INT_MAX-2,
                                 INT_MAX-1, INT_MAX, INT_MIN };
        enum { NUM_VALUES_A = sizeof VALUES_A / sizeof *VALUES_A };

        for (int vix = -NUM_VALUES_A+1; vix < NUM_VALUES_A; ++vix) {
            int vx = vix < 0 ? -VALUES_A[-vix] : VALUES_A[vix];
            for (int viy = -NUM_VALUES_A+1; viy < NUM_VALUES_A; ++viy) {
                int vy = viy < 0 ? -VALUES_A[-viy] : VALUES_A[viy];
                int sum = vx + vy;

                // Note that 'bsl::abs()' does not work on 'Int64'.
                LOOP2_ASSERT(Util::intAbs(sum), (sum >= 0 ? sum : -sum),
                             Util::intAbs(sum) == (sum >= 0 ? sum : -sum));
                if (INT_MIN != sum) {
                    ASSERT(Util::intAbs(sum) >= 0);
                }

                ASSERT(Util::intMin(vx, vy)  == bsl::min(vx, vy));
                ASSERT(Util::intMin(vx, vy)  == Util::intMin(vy, vx));
                ASSERT(Util::intMin(sum, vy) == bsl::min(sum, vy));
                ASSERT(Util::intMin(sum, vy) == Util::intMin(vy, sum));

                ASSERT(Util::intMax(vx, vy)  == bsl::max(vx, vy));
                ASSERT(Util::intMax(vx, vy)  == Util::intMax(vy, vx));
                ASSERT(Util::intMax(sum, vy) == bsl::max(sum, vy));
                ASSERT(Util::intMax(sum, vy) == Util::intMax(vy, sum));
            }
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing abs32, min32, max32.\n";

        for (int vix = -NUM_VALUES_A+1; vix < NUM_VALUES_A; ++vix) {
            int vx = vix < 0 ? -VALUES_A[-vix] : VALUES_A[vix];
            for (int viy = -NUM_VALUES_A+1; viy < NUM_VALUES_A; ++viy) {
                int vy = viy < 0 ? -VALUES_A[-viy] : VALUES_A[viy];
                int sum = vx + vy;

                // Note that 'bsl::abs()' does not work on 'Int64'.
                LOOP2_ASSERT(Util::abs32(sum), (sum >= 0 ? sum : -sum),
                             Util::abs32(sum) == (sum >= 0 ? sum : -sum));
                if (INT_MIN != sum) {
                    ASSERT(Util::abs32(sum) >= 0);
                }

                ASSERT(Util::min32(vx, vy)  == bsl::min(vx, vy));
                ASSERT(Util::min32(vx, vy)  == Util::min32(vy, vx));
                ASSERT(Util::min32(sum, vy) == bsl::min(sum, vy));
                ASSERT(Util::min32(sum, vy) == Util::min32(vy, sum));

                ASSERT(Util::max32(vx, vy)  == bsl::max(vx, vy));
                ASSERT(Util::max32(vx, vy)  == Util::max32(vy, vx));
                ASSERT(Util::max32(sum, vy) == bsl::max(sum, vy));
                ASSERT(Util::max32(sum, vy) == Util::max32(vy, sum));
            }
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        const Int64 VALUES_B[] = { 0, 1, 2, 3, 4, 7, 8, 17, 27, 93, 127, 128,
                                 129, 254, 255, 256, 257, 258, 347, 749, 1000,
                                 10001, 10002, 123456, 7654321, 87654321,
                                 987654321, 1987654321, 2123456789, INT_MAX-2,
                                 INT_MAX-1, INT_MAX, INT_MIN, 0x100000000LL,
                                 0x100000001LL, 0x100000002LL, 0x100000010LL,
                                 0x311111111LL, 0x312345678LL, 0xfffffffffLL,
                                 0x1fffffffffLL, 0x7f0f0f0f0fLL,
                                 0xff0f0f0f0fLL, 0x38394837218LL,
                                 0xa38394837218LL, 0xa38394837218cLL,
                                 0x72938172637437LL, 0x9182718294ade7eLL,
                                 0xa8c7d8e7f98429fLL, 0xba8c7d8e7f98429fLL,
                                 int64Max-1, int64Max, -int64Max, int64Min };
        enum { NUM_VALUES_B = sizeof VALUES_B / sizeof *VALUES_B };

        for (int vix = -NUM_VALUES_B+1; vix < NUM_VALUES_B; ++vix) {
            Int64 vx = vix < 0 ? -VALUES_B[-vix] : VALUES_B[vix];
            for (int viy = -NUM_VALUES_B+1; viy < NUM_VALUES_B; ++viy) {
                Int64 vy = viy < 0 ? -VALUES_B[-viy] : VALUES_B[viy];
                Int64 sum = vx + vy;

                // Note that 'bsl::abs()' does not work on 'Int64'.
                ASSERT(Util::intAbs64(sum) == (sum >= 0 ? sum : -sum));
                if (int64Min != sum) {
                    ASSERT(Util::intAbs64(sum) >= 0);
                }

                ASSERT(Util::intMin64(vx, vy)  == bsl::min(vx, vy));
                ASSERT(Util::intMin64(vx, vy)  == Util::intMin64(vy, vx));
                ASSERT(Util::intMin64(sum, vy) == bsl::min(sum, vy));
                ASSERT(Util::intMin64(sum, vy) == Util::intMin64(vy, sum));

                ASSERT(Util::intMax64(vx, vy)  == bsl::max(vx, vy));
                ASSERT(Util::intMax64(vx, vy)  == Util::intMax64(vy, vx));
                ASSERT(Util::intMax64(sum, vy) == bsl::max(sum, vy));
                ASSERT(Util::intMax64(sum, vy) == Util::intMax64(vy, sum));
            }
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing abs64, min64, max64.\n";

        for (int vix = -NUM_VALUES_B+1; vix < NUM_VALUES_B; ++vix) {
            Int64 vx = vix < 0 ? -VALUES_B[-vix] : VALUES_B[vix];
            for (int viy = -NUM_VALUES_B+1; viy < NUM_VALUES_B; ++viy) {
                Int64 vy = viy < 0 ? -VALUES_B[-viy] : VALUES_B[viy];
                Int64 sum = vx + vy;

                // Note that 'bsl::abs()' does not work on 'Int64'.
                ASSERT(Util::abs64(sum) == (sum >= 0 ? sum : -sum));
                if (int64Min != sum) {
                    ASSERT(Util::abs64(sum) >= 0);
                }

                ASSERT(Util::min64(vx, vy)  == bsl::min(vx, vy));
                ASSERT(Util::min64(vx, vy)  == Util::min64(vy, vx));
                ASSERT(Util::min64(sum, vy) == bsl::min(sum, vy));
                ASSERT(Util::min64(sum, vy) == Util::min64(vy, sum));

                ASSERT(Util::max64(vx, vy)  == bsl::max(vx, vy));
                ASSERT(Util::max64(vx, vy)  == Util::max64(vy, vx));
                ASSERT(Util::max64(sum, vy) == bsl::max(sum, vy));
                ASSERT(Util::max64(sum, vy) == Util::max64(vy, sum));
            }
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // TESTING 'sizeInBits'
        //
        // Concerns:
        //   That the 'sizeInBits' function correctly returns the size of
        //   variables in bits.
        //
        // Plan:
        //   Test the 'sizeInBits' function on various sized objects and
        //   verify that the results are as expected.
        //
        // Testing:
        //   int sizeInBits(value)
        // --------------------------------------------------------------------

        BSLMF_ASSERT(4 == sizeof(int) && 8 == sizeof(Int64));

        char myChar = 0;
        unsigned char myUnsignedChar = 0;
        signed char mySignedChar = 0;

        short myShort = 0;
        unsigned short myUnsignedShort = 0;

        int myInt = 0;
        unsigned myUnsigned = 0;

        long myLong = 0;
        unsigned long myUnsignedLong = 0;

        void *myPtr = 0;

        Int64 myInt64 = 0;
        Uint64 myUint64 = 0;

        ASSERT(8 == Util::sizeInBits(myChar));
        ASSERT(8 == Util::sizeInBits(myUnsignedChar));
        ASSERT(8 == Util::sizeInBits(mySignedChar));

        ASSERT(16 == Util::sizeInBits(myShort));
        ASSERT(16 == Util::sizeInBits(myUnsignedShort));

        ASSERT(32 == Util::sizeInBits(myInt));
        ASSERT(32 == Util::sizeInBits(myUnsigned));

        ASSERT(sizeof(long) * CHAR_BIT == Util::sizeInBits(myLong));
        ASSERT(sizeof(unsigned long) * CHAR_BIT ==
                                             Util::sizeInBits(myUnsignedLong));

        ASSERT(sizeof(void *) * CHAR_BIT == Util::sizeInBits(myPtr));

        ASSERT(64 == Util::sizeInBits(myInt64));
        ASSERT(64 == Util::sizeInBits(myUint64));

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing numBits.\n";

        ASSERT(8 == Util::numBits(myChar));
        ASSERT(8 == Util::numBits(myUnsignedChar));
        ASSERT(8 == Util::numBits(mySignedChar));

        ASSERT(16 == Util::numBits(myShort));
        ASSERT(16 == Util::numBits(myUnsignedShort));

        ASSERT(32 == Util::numBits(myInt));
        ASSERT(32 == Util::numBits(myUnsigned));

        ASSERT(sizeof(long) * CHAR_BIT == Util::numBits(myLong));
        ASSERT(sizeof(unsigned long) * CHAR_BIT ==
                                                Util::numBits(myUnsignedLong));

        ASSERT(sizeof(void *) * CHAR_BIT == Util::numBits(myPtr));

        ASSERT(64 == Util::numBits(myInt64));
        ASSERT(64 == Util::numBits(myUint64));
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // TESTING numSetOne,numSetZero,roundUpToBinaryPower,base2Log
        // (both 32 bit and 64 bit versions)
        //
        // Concerns:
        //   That numSetOne(), numSetZero(), roundUpToBinaryPower(), and
        //   base2Log() all work as specced.
        //
        // Plan:
        //   Create simple counterparts for the 64 bit versions of all 3
        //   routines, that use totally different (and simpler) algorithms to
        //   achieve the same results.  Run on a set of different values,
        //   verifying that results match and that results have expected
        //   properties.
        //
        // Testing:
        //   numSetOne()
        //   numSetOne64()
        //   numSetZero()
        //   numSetZero64()
        //   roundUpToBinaryPower()
        //   roundUpToBinaryPower64()
        //   base2Log()
        //   base2Log64()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing numSetOne, numSetZero,\n"
                << "==================================\n"
                << "roundUpToBinaryPower, and base2Log\n"
                << "====================================\n";

        struct {
            int d_line;
            unsigned d_value;
        } DATA[] = {
            { L_, 0 },
            { L_, 1 },
            { L_, 2 },
            { L_, 3 },
            { L_, 734 },
            { L_, 1001 },
            { L_, 1025 },
            { L_, 0x12345678 },
            { L_, 0x87654321 },
            { L_, 0x55555555 },
            { L_, 0xaaaaaaaa },
            { L_, 0xeeeeeeee },
            { L_, 60 },
            { L_, 61 },
            { L_, 62 },
            { L_, 63 },
            { L_, 64 },
            { L_, 65 },
            { L_, 66 },
            { L_, (unsigned) -5 },
            { L_, (unsigned) -1 },
            { L_, (unsigned) -104958 },
            { L_, (unsigned) -64 },
            { L_, 0x00000001 },
            { L_, 0x00000002 },
            { L_, 0x00000004 },
            { L_, 0x00000008 },
            { L_, 0x00000010 },
            { L_, 0x00000020 },
            { L_, 0x00000040 },
            { L_, 0x00000080 },
            { L_, 0x00000100 },
            { L_, 0x00000200 },
            { L_, 0x00000400 },
            { L_, 0x00000800 },
            { L_, 0x00001000 },
            { L_, 0x00002000 },
            { L_, 0x00004000 },
            { L_, 0x00008000 },
            { L_, 0x00010000 },
            { L_, 0x00020000 },
            { L_, 0x00040000 },
            { L_, 0x00080000 },
            { L_, 0x00100000 },
            { L_, 0x00200000 },
            { L_, 0x00400000 },
            { L_, 0x00800000 },
            { L_, 0x01000000 },
            { L_, 0x02000000 },
            { L_, 0x04000000 },
            { L_, 0x08000000 },
            { L_, 0x10000000 },
            { L_, 0x20000000 },
            { L_, 0x40000000 },
            { L_, 0x80000000U } };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < DATA_LEN; ++i) {
            for (int j = 0; j < DATA_LEN; ++j) {
                unsigned value = DATA[i].d_value + DATA[j].d_value;

                {
                    int v1 = Util::numSetOne(value);
                    int v3 =    countOneBits(value);
                    LOOP3_ASSERT(value, v1, v3, v1 == v3);
                }

                {
                    int v1 = Util::numSetZero(value);
                    int v3 =     countOneBits(value);
                    LOOP3_ASSERT(value, v1, v3, v1 == 32 - v3);
                }

                {
                    unsigned v1 = Util::roundUpToBinaryPower(  value);
                    Uint64 v3   =       roundUpToBinaryPower(  value);
                    if (0x80000000U >= value) {
                        LOOP3_ASSERT(value, v1, v3, v1 == v3);
                        LOOP2_ASSERT(value, v1, v1 >= value);
                        LOOP2_ASSERT(value, v1, !value ||
                                                        1 == countOneBits(v1));
                    }
                    else {
                        LOOP2_ASSERT(value, v1, 0 == v1);
                    }
                }

                {
                    if (0 != value) {
                        int v1 = Util::base2Log(value);
                        int v3 =       base2Log(value);
                        if (0x80000000U >= value) {
                            LOOP3_ASSERT(value, v1,  v3, v1  == v3);
                        }
                        Uint64 exp = 1;
                        exp <<= v1;
                        LOOP3_ASSERT(value, v1, exp, !v1 || exp >= value);
                    }
                }
            }
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing numSet0, numSet1.\n";

        for (int i = 0; i < DATA_LEN; ++i) {
            for (int j = 0; j < DATA_LEN; ++j) {
                unsigned value = DATA[i].d_value + DATA[j].d_value;

                {
                    int v1 = Util::numSet1(value);
                    int v3 =  countOneBits(value);
                    LOOP3_ASSERT(value, v1, v3, v1 == v3);
                }

                {
                    int v1 = Util::numSet0(value);
                    int v3 =  countOneBits(value);
                    LOOP3_ASSERT(value, v1, v3, v1 == 32 - v3);
                }
            }
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
                << "Testing numSetOne64, numSetZero64,\n"
                << "==================================\n"
                << "roundUpToBinaryPower64, and base2Log64\n"
                << "======================================\n";

        for (int i = 0; i < DATA_LEN; ++i) {
            for (int j = 0; j < DATA_LEN; ++j) {
                for (int k = 0; k < DATA_LEN; ++k) {
                    unsigned hiWord = DATA[i].d_value + DATA[k].d_value;
                    unsigned loWord = DATA[j].d_value + DATA[k].d_value;
                    Uint64 value = hiWord;
                    value <<= 32;
                    value |= loWord;

                    {
                        int v64 = Util::numSetOne64(value);
                        int v3  =      countOneBits(value);
                        LOOP3_ASSERT(value, v64, v3, v64 == v3);
                    }

                    {
                        int v64 = Util::numSetZero64(value);
                        int v3  =      countOneBits(value);
                        LOOP3_ASSERT(value, v64, v3, v64 == 64 - v3);
                    }

                    {
                        Uint64 v64 = Util::roundUpToBinaryPower64(value);
                        Uint64 v3  =       roundUpToBinaryPower(  value);
                        LOOP3_ASSERT(value, v64, v3, v64 == v3);
                        if (value > ((Uint64) 1 << 63)) {
                            LOOP2_ASSERT(value, v64, 0 == v64);
                        }
                        else {
                            LOOP2_ASSERT(value, v64, v64 >= value);
                            LOOP2_ASSERT(value, v64, !value ||
                                                       1 == countOneBits(v64));
                            if (value < ((Uint64) 1 << 63)) {
                                LOOP2_ASSERT(value, v64, !value ||
                                                                v64 < value*2);
                            }
                            else {
                                LOOP2_ASSERT(value, v64, v64 == value);
                            }
                        }
                    }

                    {
                        if (0 != value) {
                            int v64 = Util::base2Log64(value);
                            int v3  =       base2Log(  value);
                            LOOP3_ASSERT(value, v64, v3, v64 == v3);
                            if (v64 < 64) {
                                Uint64 exp = ((Uint64) 1 << v64);
                                LOOP2_ASSERT(value, v64, value <= exp);
                                if (v64 < 63) {
                                    LOOP2_ASSERT(value, v64, 2*value > exp);
                                }
                            }
                            else {
                                LOOP2_ASSERT(value, v64, value >
                                                           ((Uint64) 1 << 63));
                            }
                        }
                    }
                }
            }
        }

      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //   The two-argument "search in range" functions are all implemented
        //   using simple combinations of already-tested functionality, and
        //   require relatively few test vectors.
        //
        // Plan:
        //   Systematically select a few source integers and well-chosen
        //   source indices to lightly probe the underlying implementation as
        //   well as the logic of the proximate inline implementation.
        //
        // Testing:
        //   int find1AtSmallestIndexGE(int srcInteger, int srcIndex)
        //   int find1AtSmallestIndexGE64(
        //                   bsls_Types::Int64 srcInteger, int srcIndex)
        //   int find1AtSmallestIndexGT(int srcInteger, int srcIndex)
        //   int find1AtSmallestIndexGT64(
        //                   bsls_PlatfromUtil::Int64 srcInteger, int srcIndex)
        //   int find1AtSmallestIndexLE(int srcInteger, int srcIndex)
        //   int find1AtSmallestIndexLE64(
        //                   bsls_PlatfromUtil::Int64 srcInteger, int srcIndex)
        //   int find1AtSmallestIndexLT(int srcInteger, int srcIndex)
        //   int find1AtSmallestIndexLT64(
        //                   bsls_PlatfromUtil::Int64 srcInteger, int srcIndex)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing find1AtSmallestIndexXX functions" << endl
            << "========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_A[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "0..0",  BPW,     BPW,     BPW,     BPW,     BPW     },
            { L_,     "0..0",  BPW - 1, BPW,     BPW,     BPW,     BPW     },
            { L_,     "0..0",  0,       BPW,     BPW,     BPW,     BPW     },
            { L_, "0..01011",  BPW,     BPW,     BPW,     0,       0       },
            { L_, "0..01011",  BPW - 1, BPW,     BPW,     0,       0       },
            { L_, "0..01011",  0,       0,       1,       0,       BPW     },
            { L_, "0..01011",  2,       3,       3,       0,       0       },
            { L_, "0..01011",  3,       3,       BPW,     0,       0       },
            { L_, "0..01011",  4,       BPW,     BPW,     0,       0       },
            { L_, "11010..0",  BPW,     BPW,     BPW,     BPW - 4, BPW - 4 },
            { L_, "11010..0",  BPW - 1, BPW - 1, BPW,     BPW - 4, BPW - 4 },
            { L_, "11010..0",  0,       BPW - 4, BPW - 4, BPW,     BPW     },
            { L_, "11010..0",  BPW - 3, BPW - 2, BPW - 2, BPW - 4, BPW - 4 },
            { L_, "11010..0",  BPW - 4, BPW - 4, BPW - 2, BPW - 4, BPW     },
            { L_, "11010..0",  BPW - 5, BPW - 4, BPW - 4, BPW,     BPW     }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::find1AtSmallestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::find1AtSmallestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::find1AtSmallestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::find1AtSmallestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit1AtSmallestIndexXX functions" << endl
            << "===========================================" << endl;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::findBit1AtSmallestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::findBit1AtSmallestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::findBit1AtSmallestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::findBit1AtSmallestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing find1AtSmallestIndexXX64 functions" << endl
            << "==========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_B[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "0..0",  BPS,     BPS,     BPS,     BPS,     BPS     },
            { L_,     "0..0",  BPS - 1, BPS,     BPS,     BPS,     BPS     },
            { L_,     "0..0",  0,       BPS,     BPS,     BPS,     BPS     },
            { L_, "0..01011",  BPS,     BPS,     BPS,     0,       0       },
            { L_, "0..01011",  BPS - 1, BPS,     BPS,     0,       0       },
            { L_, "0..01011",  0,       0,       1,       0,       BPS     },
            { L_, "0..01011",  2,       3,       3,       0,       0       },
            { L_, "0..01011",  3,       3,       BPS,     0,       0       },
            { L_, "0..01011",  4,       BPS,     BPS,     0,       0       },
            { L_, "11010..0",  BPS,     BPS,     BPS,     BPS - 4, BPS - 4 },
            { L_, "11010..0",  BPS - 1, BPS - 1, BPS,     BPS - 4, BPS - 4 },
            { L_, "11010..0",  0,       BPS - 4, BPS - 4, BPS,     BPS     },
            { L_, "11010..0",  BPS - 3, BPS - 2, BPS - 2, BPS - 4, BPS - 4 },
            { L_, "11010..0",  BPS - 4, BPS - 4, BPS - 2, BPS - 4, BPS     },
            { L_, "11010..0",  BPS - 5, BPS - 4, BPS - 4, BPS,     BPS     }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::find1AtSmallestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGE == result);

            result =
                Util::find1AtSmallestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGT == result);

            result =
                Util::find1AtSmallestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLE == result);

            result =
                Util::find1AtSmallestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit1AtSmallestIndexXX64 functions" << endl
            << "=============================================" << endl;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::findBit1AtSmallestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGE == result);

            result =
                Util::findBit1AtSmallestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGT == result);

            result =
                Util::findBit1AtSmallestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLE == result);

            result =
                Util::findBit1AtSmallestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testG(int test)
{
    switch (test) {
      case 35: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //   The two-argument "search in range" functions are all implemented
        //   using simple combinations of already-tested functionality, and
        //   require relatively few test vectors.
        //
        // Plan:
        //   Systematically select a few source integers and well-chosen
        //   source indices to lightly probe the underlying implementation as
        //   well as the logic of the proximate inline implementation.
        //
        // Testing:
        //   int find0AtSmallestIndexGE(  int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexGT(  int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexLE(  int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexLT(  int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexGE64(int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexGT64(int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexLE64(int srcInteger, int srcIndex)
        //   int find0AtSmallestIndexLT64(int srcInteger, int srcIndex)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing find0AtSmallestIndexXX functions" << endl
            << "========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_A[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "1..1",  BPW,     BPW,     BPW,     BPW,     BPW     },
            { L_,     "1..1",  BPW - 1, BPW,     BPW,     BPW,     BPW     },
            { L_,     "1..1",  0,       BPW,     BPW,     BPW,     BPW     },
            { L_, "1..10100",  BPW,     BPW,     BPW,     0,       0       },
            { L_, "1..10100",  BPW - 1, BPW,     BPW,     0,       0       },
            { L_, "1..10100",  0,       0,       1,       0,       BPW     },
            { L_, "1..10100",  2,       3,       3,       0,       0       },
            { L_, "1..10100",  3,       3,       BPW,     0,       0       },
            { L_, "1..10100",  4,       BPW,     BPW,     0,       0       },
            { L_, "00101..1",  BPW,     BPW,     BPW,     BPW - 4, BPW - 4 },
            { L_, "00101..1",  BPW - 1, BPW - 1, BPW,     BPW - 4, BPW - 4 },
            { L_, "00101..1",  0,       BPW - 4, BPW - 4, BPW,     BPW     },
            { L_, "00101..1",  BPW - 3, BPW - 2, BPW - 2, BPW - 4, BPW - 4 },
            { L_, "00101..1",  BPW - 4, BPW - 4, BPW - 2, BPW - 4, BPW     },
            { L_, "00101..1",  BPW - 5, BPW - 4, BPW - 4, BPW,     BPW     }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::find0AtSmallestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::find0AtSmallestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::find0AtSmallestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::find0AtSmallestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit0AtSmallestIndexXX functions" << endl
            << "===========================================" << endl;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::findBit0AtSmallestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::findBit0AtSmallestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::findBit0AtSmallestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::findBit0AtSmallestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing find0AtSmallestIndexXX64 functions" << endl
            << "==========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_B[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "1..1",  BPS,     BPS,     BPS,     BPS,     BPS     },
            { L_,     "1..1",  BPS - 1, BPS,     BPS,     BPS,     BPS     },
            { L_,     "1..1",  0,       BPS,     BPS,     BPS,     BPS     },
            { L_, "1..10100",  BPS,     BPS,     BPS,     0,       0       },
            { L_, "1..10100",  BPS - 1, BPS,     BPS,     0,       0       },
            { L_, "1..10100",  0,       0,       1,       0,       BPS     },
            { L_, "1..10100",  2,       3,       3,       0,       0       },
            { L_, "1..10100",  3,       3,       BPS,     0,       0       },
            { L_, "1..10100",  4,       BPS,     BPS,     0,       0       },
            { L_, "00101..1",  BPS,     BPS,     BPS,     BPS - 4, BPS - 4 },
            { L_, "00101..1",  BPS - 1, BPS - 1, BPS,     BPS - 4, BPS - 4 },
            { L_, "00101..1",  0,       BPS - 4, BPS - 4, BPS,     BPS     },
            { L_, "00101..1",  BPS - 3, BPS - 2, BPS - 2, BPS - 4, BPS - 4 },
            { L_, "00101..1",  BPS - 4, BPS - 4, BPS - 2, BPS - 4, BPS     },
            { L_, "00101..1",  BPS - 5, BPS - 4, BPS - 4, BPS,     BPS     }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::find0AtSmallestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedGE, result,
                                             DATA_B[i].d_expectedGE == result);

            result =
                Util::find0AtSmallestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedGT,
                                     result, DATA_B[i].d_expectedGT == result);

            result =
                Util::find0AtSmallestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedLE, result,
                                             DATA_B[i].d_expectedLE == result);

            result =
                Util::find0AtSmallestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedLT, result,
                                             DATA_B[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit0AtSmallestIndexXX64 functions" << endl
            << "=============================================" << endl;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::findBit0AtSmallestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedGE, result,
                                             DATA_B[i].d_expectedGE == result);

            result =
                Util::findBit0AtSmallestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedGT,
                                     result, DATA_B[i].d_expectedGT == result);

            result =
                Util::findBit0AtSmallestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedLE, result,
                                             DATA_B[i].d_expectedLE == result);

            result =
                Util::findBit0AtSmallestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP3_ASSERT(LINE, DATA_B[i].d_expectedLT, result,
                                             DATA_B[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //   The two-argument "search in range" functions are all implemented
        //   using simple combinations of already-tested functionality, and
        //   require relatively few test vectors.
        //
        // Plan:
        //   Systematically select a few source integers and well-chosen
        //   source indices to lightly probe the underlying implementation as
        //   well as the logic of the proximate inline implementation.
        //
        // Testing:
        //   int find1AtLargestIndexGE(  int srcInteger, int srcIndex)
        //   int find1AtLargestIndexGT(  int srcInteger, int srcIndex)
        //   int find1AtLargestIndexLE(  int srcInteger, int srcIndex)
        //   int find1AtLargestIndexLT(  int srcInteger, int srcIndex)
        //   int find1AtLargestIndexGE64(int srcInteger, int srcIndex)
        //   int find1AtLargestIndexGT64(int srcInteger, int srcIndex)
        //   int find1AtLargestIndexLE64(int srcInteger, int srcIndex)
        //   int find1AtLargestIndexLT64(int srcInteger, int srcIndex)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing find1AtLargestIndexXX functions" << endl
            << "=======================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_A[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "0..0",  BPW,     -1,      -1,      -1,      -1      },
            { L_,     "0..0",  BPW - 1, -1,      -1,      -1,      -1      },
            { L_,     "0..0",  0,       -1,      -1,      -1,      -1      },
            { L_, "0..01011",  BPW,     -1,      -1,      3,       3       },
            { L_, "0..01011",  BPW - 1, -1,      -1,      3,       3       },
            { L_, "0..01011",  0,        3,       3,      0,       -1      },
            { L_, "0..01011",  2,        3,       3,      1,       1       },
            { L_, "0..01011",  3,        3,      -1,      3,       1       },
            { L_, "0..01011",  4,       -1,      -1,      3,       3       },
            { L_, "11010..0",  BPW,     -1,      -1,      BPW - 1, BPW - 1 },
            { L_, "11010..0",  BPW - 1, BPW - 1, -1,      BPW - 1, BPW - 2 },
            { L_, "11010..0",  0,       BPW - 1, BPW - 1, -1,      -1      },
            { L_, "11010..0",  BPW - 3, BPW - 1, BPW - 1, BPW - 4, BPW - 4 },
            { L_, "11010..0",  BPW - 4, BPW - 1, BPW - 1, BPW - 4, -1      },
            { L_, "11010..0",  BPW - 5, BPW - 1, BPW - 1, -1,      -1      },
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE   = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src          = g(spec);
            int result =
                Util::find1AtLargestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::find1AtLargestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::find1AtLargestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::find1AtLargestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit1AtLargestIndexXX functions" << endl
            << "==========================================" << endl;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE   = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src          = g(spec);
            int result =
                Util::findBit1AtLargestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::findBit1AtLargestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::findBit1AtLargestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::findBit1AtLargestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing find1AtLargestIndexXX64 functions" << endl
            << "=========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_B[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "0..0",  BPS,     -1,      -1,      -1,      -1      },
            { L_,     "0..0",  BPS - 1, -1,      -1,      -1,      -1      },
            { L_,     "0..0",  0,       -1,      -1,      -1,      -1      },
            { L_, "0..01011",  BPS,     -1,      -1,      3,       3       },
            { L_, "0..01011",  BPS - 1, -1,      -1,      3,       3       },
            { L_, "0..01011",  0,        3,       3,      0,       -1      },
            { L_, "0..01011",  2,        3,       3,      1,       1       },
            { L_, "0..01011",  3,        3,      -1,      3,       1       },
            { L_, "0..01011",  4,       -1,      -1,      3,       3       },
            { L_, "11010..0",  BPS,     -1,      -1,      BPS - 1, BPS - 1 },
            { L_, "11010..0",  BPS - 1, BPS - 1, -1,      BPS - 1, BPS - 2 },
            { L_, "11010..0",  0,       BPS - 1, BPS - 1, -1,      -1      },
            { L_, "11010..0",  BPS - 3, BPS - 1, BPS - 1, BPS - 4, BPS - 4 },
            { L_, "11010..0",  BPS - 4, BPS - 1, BPS - 1, BPS - 4, -1      },
            { L_, "11010..0",  BPS - 5, BPS - 1, BPS - 1, -1,      -1      },
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::find1AtLargestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGE == result);

            result =
                Util::find1AtLargestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGT == result);

            result =
                Util::find1AtLargestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLE == result);

            result =
                Util::find1AtLargestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit1AtLargestIndexXX64 functions" << endl
            << "============================================" << endl;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src        = g64(spec);
            int result =
                Util::findBit1AtLargestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGE == result);

            result =
                Util::findBit1AtLargestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedGT == result);

            result =
                Util::findBit1AtLargestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLE == result);

            result =
                Util::findBit1AtLargestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_B[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //   The two-argument "search in range" functions are all implemented
        //   using simple combinations of already-tested functionality, and
        //   require relatively few test vectors.
        //
        // Plan:
        //   Systematically select a few source integers and well-chosen
        //   source indices to lightly probe the underlying implementation as
        //   well as the logic of the proximate inline implementation.
        //
        // Testing:
        //   int find0AtLargestIndexGE(  int srcInteger, int srcIndex)
        //   int find0AtLargestIndexGT(  int srcInteger, int srcIndex)
        //   int find0AtLargestIndexLE(  int srcInteger, int srcIndex)
        //   int find0AtLargestIndexLT(  int srcInteger, int srcIndex)
        //   int find0AtLargestIndexGE64(int srcInteger, int srcIndex)
        //   int find0AtLargestIndexGT64(int srcInteger, int srcIndex)
        //   int find0AtLargestIndexLE64(int srcInteger, int srcIndex)
        //   int find0AtLargestIndexLT64(int srcInteger, int srcIndex)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing find0AtLargestIndexXX functions" << endl
            << "=======================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_A[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "1..1",  BPW,     -1,      -1,      -1,      -1      },
            { L_,     "1..1",  BPW - 1, -1,      -1,      -1,      -1      },
            { L_,     "1..1",  0,       -1,      -1,      -1,      -1      },
            { L_, "1..10100",  BPW,     -1,      -1,      3,       3       },
            { L_, "1..10100",  BPW - 1, -1,      -1,      3,       3       },
            { L_, "1..10100",  0,       3,       3,       0,       -1      },
            { L_, "1..10100",  2,       3,       3,       1,       1       },
            { L_, "1..10100",  3,       3,       -1,      3,       1       },
            { L_, "1..10100",  4,       -1,      -1,      3,       3       },
            { L_, "00101..1",  BPW,     -1,      -1,      BPW - 1, BPW - 1 },
            { L_, "00101..1",  BPW - 1, BPW - 1, -1,      BPW - 1, BPW - 2 },
            { L_, "00101..1",  0,       BPW - 1, BPW - 1, -1,      -1      },
            { L_, "00101..1",  BPW - 3, BPW - 1, BPW - 1, BPW - 4, BPW - 4 },
            { L_, "00101..1",  BPW - 4, BPW - 1, BPW - 1, BPW - 4, -1      },
            { L_, "00101..1",  BPW - 5, BPW - 1, BPW - 1, -1,      -1      }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE   = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::find0AtLargestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::find0AtLargestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::find0AtLargestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::find0AtLargestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit0AtLargestIndexXX functions" << endl
            << "==========================================" << endl;

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int LINE   = DATA_A[i].d_lineNum;
            const char *spec = DATA_A[i].d_src;
            int src = g(spec);
            int result =
                Util::findBit0AtLargestIndexGE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGE == result);

            result =
                Util::findBit0AtLargestIndexGT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedGT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedGT == result);

            result =
                Util::findBit0AtLargestIndexLE(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLE); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLE == result);

            result =
                Util::findBit0AtLargestIndexLT(src, DATA_A[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[i].d_srcIndex);
                P_(DATA_A[i].d_expectedLT); P(result);
            }
            LOOP_ASSERT(LINE, DATA_A[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing find0AtLargestIndexXX64 functions" << endl
            << "=========================================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_src;          // source integer
            int         d_srcIndex;     // source index
            int         d_expectedGE;   // expected result for GE function
            int         d_expectedGT;   // expected result for GT function
            int         d_expectedLE;   // expected result for LE function
            int         d_expectedLT;   // expected result for LE function
        } DATA_B[] = {
            //L#         src   srcIndx  expGE    expGT    expLE    expLT
            //--    --------   -------  -------- -------- -------- --------
            { L_,     "1..1",  BPS,     -1,      -1,      -1,      -1      },
            { L_,     "1..1",  BPS - 1, -1,      -1,      -1,      -1      },
            { L_,     "1..1",  0,       -1,      -1,      -1,      -1      },
            { L_, "1..10100",  BPS,     -1,      -1,      3,       3       },
            { L_, "1..10100",  BPS - 1, -1,      -1,      3,       3       },
            { L_, "1..10100",  0,       3,       3,       0,       -1      },
            { L_, "1..10100",  2,       3,       3,       1,       1       },
            { L_, "1..10100",  3,       3,       -1,      3,       1       },
            { L_, "1..10100",  4,       -1,      -1,      3,       3       },
            { L_, "00101..1",  BPS,     -1,      -1,      BPS - 1, BPS - 1 },
            { L_, "00101..1",  BPS - 1, BPS - 1, -1,      BPS - 1, BPS - 2 },
            { L_, "00101..1",  0,       BPS - 1, BPS - 1, -1,      -1      },
            { L_, "00101..1",  BPS - 3, BPS - 1, BPS - 1, BPS - 4, BPS - 4 },
            { L_, "00101..1",  BPS - 4, BPS - 1, BPS - 1, BPS - 4, -1      },
            { L_, "00101..1",  BPS - 5, BPS - 1, BPS - 1, -1,      -1      }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src = g64(spec);
            int result =
                Util::find0AtLargestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedGE, result,
                                             DATA_B[i].d_expectedGE == result);

            result =
                Util::find0AtLargestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedGT, result,
                                             DATA_B[i].d_expectedGT == result);

            result =
                Util::find0AtLargestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedLE, result,
                                             DATA_B[i].d_expectedLE == result);

            result =
                Util::find0AtLargestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedLT, result,
                                             DATA_B[i].d_expectedLT == result);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << endl
            << "Testing findBit0AtLargestIndexXX64 functions" << endl
            << "============================================" << endl;

        for (int i = 0; i < NUM_DATA_B; ++i) {
            const int LINE   = DATA_B[i].d_lineNum;
            const char *spec = DATA_B[i].d_src;
            Int64 src = g64(spec);
            int result =
                Util::findBit0AtLargestIndexGE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGE); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedGE, result,
                                             DATA_B[i].d_expectedGE == result);

            result =
                Util::findBit0AtLargestIndexGT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedGT); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedGT, result,
                                             DATA_B[i].d_expectedGT == result);

            result =
                Util::findBit0AtLargestIndexLE64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLE); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedLE, result,
                                             DATA_B[i].d_expectedLE == result);

            result =
                Util::findBit0AtLargestIndexLT64(src, DATA_B[i].d_srcIndex);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[i].d_srcIndex);
                P_(DATA_B[i].d_expectedLT); P(result);
            }
            LOOP4_ASSERT(LINE, spec, DATA_B[i].d_expectedLT, result,
                                             DATA_B[i].d_expectedLT == result);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //   'find1AtLargestIndex' and 'find1AtSmallestIndex' have
        //   non-trivial implementations that each include a (different)
        //   look-up table (which must be tested exhaustively).  The other
        //   methods trivially use the two non-trivial methods.  It is
        //   sufficient to verify the lookup tables in separate loop-based
        //   tests and then to straightforwardly test the remaining
        //   functionality.
        //
        // Plan:
        //   Verify that the internal tables contain the correct data.
        //   For each of a sequence of individual tests tailed for one of four
        //   function, verify that that function gives the expected value.
        //
        // Testing:
        //   int find0AtLargestIndex(   int srcInteger)
        //   int find0AtLargestIndex64( int srcInteger)
        //   int find1AtLargestIndex(   int srcInteger)
        //   int find1AtLargestIndex64( int srcInteger)
        //   int find0AtSmallestIndex(  int srcInteger)
        //   int find0AtSmallestIndex64(int srcInteger)
        //   int find1AtSmallestIndex(  int srcInteger)
        //   int find1AtSmallestIndex64(int srcInteger)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 32 bit absolute search functions" << endl
            << "========================================" << endl;

        if (verbose) cout << "\tVerifying internal tables: table 1" << endl;
        {
            const int TABLE_SIZE = 1024;  // Overkill, but never mind.
            for(int i = 0; i < TABLE_SIZE; ++i) {
                LOOP3_ASSERT(i, findFirstBit1(i),
                                Util::find1AtLargestIndex(i),
                                findFirstBit1(i) ==
                                Util::find1AtLargestIndex(i));
            }
        }

        if (verbose) cout << "\tVerifying internal tables: table 2" << endl;
        {
            const int TABLE_SIZE = 1024;  // Overkill, but never mind.
            for(int i = 1; i < TABLE_SIZE; ++i) {
                LOOP_ASSERT(i, findLastBit1(i) ==
                               Util::find1AtSmallestIndex(i));
            }
            ASSERT(BPW == Util::find1AtSmallestIndex(0));
        }

        if (verbose) cout << "\tTesting find1AtLargestIndex" << endl;
        {
            static const struct {
                int         d_lineNum;     // line number
                const char *d_src;         // source integer
                int         d_expected;    // expected result
            } DATA[] = {
                //L#                  src          expected result
                //--             --------         ----------------
                { L_,               "0..0",      -1                },
                { L_,              "0..01",       0                },
                { L_,             "0..011",       1                },
                { L_,            "0..0111",       2                },
                { L_,           "0..01111",       3                },
                { L_,          "0..011111",       4                },
                { L_,         "0..0111111",       5                },
                { L_,        "0..01111111",       6                },
                { L_,       "0..011111111",       7                },

                { L_,             "100..0",       BPW - 1          },
                { L_,             "010..0",       BPW - 2          },
                { L_,             "110..0",       BPW - 1          },

                { L_,    "000000" "100..0",       BPW - 8 + 1      },
                { L_,    "000000" "010..0",       BPW - 8          },
                { L_,    "000000" "110..0",       BPW - 8 + 1      },

                { L_,        B8_0 "100..0",       BPW - 8 - 1      },
                { L_,        B8_0 "010..0",       BPW - 8 - 2      },
                { L_,        B8_0 "110..0",       BPW - 8 - 1      },

                { L_, B8_0 "000000" "100..0",     BPW - 16 + 1     },
                { L_, B8_0 "000000" "010..0",     BPW - 16         },
                { L_, B8_0 "000000" "110..0",     BPW - 16 + 1     },

                { L_,       B16_0 "100..0",       BPW - 16 - 1     },
                { L_,       B16_0 "010..0",       BPW - 16 - 2     },
                { L_,       B16_0 "110..0",       BPW - 16 - 1     }

#if INT_MAX > 0x7FFFFFFF
             ,  { L_,       B32_0 "100..0",       BPW - 32 - 1     },
                { L_,       B32_0 "010..0",       BPW - 32 - 2     },
                { L_,       B32_0 "110..0",       BPW - 32 - 1     }
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::find1AtLargestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit1AtLargestIndex" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::findBit1AtLargestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) cout << "\tTesting find1AtLargestIndex64" << endl;

            int j;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_HI < 0) ? EXPECTED_LO
                                                           : EXPECTED_HI + 32;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::find1AtLargestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit1AtLargestIndex64" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_HI < 0) ? EXPECTED_LO
                                                           : EXPECTED_HI + 32;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::findBit1AtLargestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        }

        if (verbose) cout << "\tTesting find0AtLargestIndex" << endl;
        {
            static const struct {
                int         d_lineNum;     // line number
                const char *d_src;         // source integer
                int         d_expected;    // expected result
            } DATA[] = {
                //L#                  src          expected result
                //--             --------         ----------------
                { L_,               "1..1",      -1                },
                { L_,              "1..10",       0                },
                { L_,             "1..100",       1                },
                { L_,            "1..1000",       2                },
                { L_,           "1..10000",       3                },
                { L_,          "1..100000",       4                },
                { L_,         "1..1000000",       5                },
                { L_,        "1..10000000",       6                },
                { L_,       "1..100000000",       7                },

                { L_,             "011..1",       BPW - 1          },
                { L_,             "101..1",       BPW - 2          },
                { L_,             "001..1",       BPW - 1          },

                { L_,    "111111" "011..1",       BPW - 8 + 1      },
                { L_,    "111111" "101..1",       BPW - 8          },
                { L_,    "111111" "001..1",       BPW - 8 + 1      },

                { L_,        B8_1 "011..1",       BPW - 8 - 1      },
                { L_,        B8_1 "101..1",       BPW - 8 - 2      },
                { L_,        B8_1 "001..1",       BPW - 8 - 1      },

                { L_, B8_1 "111111" "011..1",     BPW - 16 + 1     },
                { L_, B8_1 "111111" "101..1",     BPW - 16         },
                { L_, B8_1 "111111" "001..1",     BPW - 16 + 1     },

                { L_,       B16_1 "011..1",       BPW - 16 - 1     },
                { L_,       B16_1 "101..1",       BPW - 16 - 2     },
                { L_,       B16_1 "001..1",       BPW - 16 - 1     }

#if INT_MAX > 0x7FFFFFFF
              , { L_,       B32_1 "100..0",       BPW - 32 - 1     },
                { L_,       B32_1 "010..0",       BPW - 32 - 2     },
                { L_,       B32_1 "110..0",       BPW - 32 - 1     }
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::find0AtLargestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit0AtLargestIndex" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::findBit0AtLargestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) cout << "\tTesting find0AtLargestIndex64" << endl;

            int j;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_HI < 0) ? EXPECTED_LO
                                                           : EXPECTED_HI + 32;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::find0AtLargestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit0AtLargestIndex64" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_HI < 0) ? EXPECTED_LO
                                                           : EXPECTED_HI + 32;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::findBit0AtLargestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        }

        if (verbose) cout << "\tTesting find1AtSmallestIndex" << endl;
        {
            static const struct {
                int         d_lineNum;     // line number
                const char *d_src;         // source integer
                int         d_expected;    // expected result
            } DATA[] = {
                //L#                  src          expected result
                //--             --------         ----------------
                { L_,               "0..0",       BPW              },
                { L_,               "1..1",       0                },
                { L_,              "1..10",       1                },
                { L_,             "1..100",       2                },
                { L_,            "1..1000",       3                },
                { L_,           "1..10000",       4                },
                { L_,          "1..100000",       5                },
                { L_,         "1..1000000",       6                },
                { L_,        "1..10000000",       7                },

                { L_,             "100..0",       BPW - 1          },
                { L_,             "010..0",       BPW - 2          },
                { L_,             "110..0",       BPW - 2          },

                { L_,    "111111" "100..0",       BPW - 8 + 1      },
                { L_,    "111111" "010..0",       BPW - 8          },
                { L_,    "111111" "110..0",       BPW - 8          },

                { L_,        B8_1 "100..0",       BPW - 8 - 1      },
                { L_,        B8_1 "010..0",       BPW - 8 - 2      },
                { L_,        B8_1 "110..0",       BPW - 8 - 2      },

                { L_, B8_1 "111111" "100..0",     BPW - 16 + 1     },
                { L_, B8_1 "111111" "010..0",     BPW - 16         },
                { L_, B8_1 "111111" "110..0",     BPW - 16         },

                { L_,       B16_1 "100..0",       BPW - 16 - 1     },
                { L_,       B16_1 "010..0",       BPW - 16 - 2     },
                { L_,       B16_1 "110..0",       BPW - 16 - 2     }

#if INT_MAX > 0x7FFFFFFF
             ,  { L_,       B32_1 "100..0",       BPW - 32 - 1     },
                { L_,       B32_1 "010..0",       BPW - 32 - 2     },
                { L_,       B32_1 "110..0",       BPW - 32 - 1     }
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::find1AtSmallestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit1AtSmallestIndex" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::findBit1AtSmallestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) cout << "\tTesting find1AtSmallestIndex64" << endl;

            int j;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_LO == BPW)
                                       ? EXPECTED_HI + 32
                                       : EXPECTED_LO;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::find1AtSmallestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit1AtSmallestIndex64" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_LO == BPW)
                                       ? EXPECTED_HI + 32
                                       : EXPECTED_LO;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::findBit1AtSmallestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        }

        if (verbose) cout << "\tTesting find0AtSmallestIndex" << endl;
        {
            static const struct {
                int         d_lineNum;     // line number
                const char *d_src;         // source integer
                int         d_expected;    // expected result
            } DATA[] = {
                //L#                  src          expected result
                //--             --------         ----------------
                { L_,               "1..1",       BPW              },
                { L_,               "0..0",       0                },
                { L_,              "0..01",       1                },
                { L_,             "0..011",       2                },
                { L_,            "0..0111",       3                },
                { L_,           "0..01111",       4                },
                { L_,          "0..011111",       5                },
                { L_,         "0..0111111",       6                },
                { L_,        "0..01111111",       7                },

                { L_,             "011..1",       BPW - 1          },
                { L_,             "101..1",       BPW - 2          },
                { L_,             "001..1",       BPW - 2          },

                { L_,    "000000" "011..1",       BPW - 8 + 1      },
                { L_,    "000000" "101..1",       BPW - 8          },
                { L_,    "000000" "001..1",       BPW - 8          },

                { L_,        B8_0 "011..1",       BPW - 8 - 1      },
                { L_,        B8_0 "101..1",       BPW - 8 - 2      },
                { L_,        B8_0 "001..1",       BPW - 8 - 2      },

                { L_, B8_0 "000000" "011..1",     BPW - 16 + 1     },
                { L_, B8_0 "000000" "101..1",     BPW - 16         },
                { L_, B8_0 "000000" "001..1",     BPW - 16         },

                { L_,       B16_0 "011..1",       BPW - 16 - 1     },
                { L_,       B16_0 "101..1",       BPW - 16 - 2     },
                { L_,       B16_0 "001..1",       BPW - 16 - 2     }

#if INT_MAX > 0x7FFFFFFF
             ,  { L_,       B32_0 "100..0",       BPW - 32 - 1     },
                { L_,       B32_0 "010..0",       BPW - 32 - 2     },
                { L_,       B32_0 "110..0",       BPW - 32 - 1     }
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::find0AtSmallestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            if (verbose) cout << "\tTesting findBit0AtSmallestIndex" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const char *spec = DATA[i].d_src;
                int src = g(spec);
                int result = Util::findBit0AtSmallestIndex(src);
                if (veryVerbose) {
                    P_(LINE); P_(spec);
                    P_(DATA[i].d_expected); P(result);
                }
                LOOP_ASSERT(LINE, DATA[i].d_expected == result);
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) cout << "\tTesting find0AtSmallestIndex64" << endl;

            int j;
            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_LO == BPW)
                                       ? EXPECTED_HI + 32
                                       : EXPECTED_LO;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::find0AtSmallestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) cout << "\tTesting findBit0AtSmallestIndex64" << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int LINE_HI = DATA[i].d_lineNum;
                const char *specHi = DATA[i].d_src;
                int srcHi = g(specHi);
                const int EXPECTED_HI = DATA[i].d_expected;

                for (j = 0; j < NUM_DATA; ++j) {
                    const int LINE_LO = DATA[j].d_lineNum;
                    const char *specLo = DATA[j].d_src;
                    unsigned int srcLo = g(specLo);
                    const int EXPECTED_LO = DATA[j].d_expected;

                    const int EXPECTED = (EXPECTED_LO == BPW)
                                       ? EXPECTED_HI + 32
                                       : EXPECTED_LO;
                    long long src = ((Uint64)srcHi << 32)
                                  | (Uint64)srcLo;
                    int result = Util::findBit0AtSmallestIndex64(src);
                    if (veryVerbose) {
                        P_(LINE_HI); P_(specHi); P(srcHi);
                        P_(LINE_LO); P_(specLo); P(srcLo);
                        P_(src); P_(EXPECTED); P(result);
                    }
                    LOOP4_ASSERT(LINE_HI, LINE_LO, EXPECTED, result,
                                 EXPECTED == result);
                }
            }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        }
      } break;
      case 31: {
////////////////////////////////////////////////////////////////////////////
// print() has been removed from bdes_BitUtil
////////////////////////////////////////////////////////////////////////////
//
//     // --------------------------------------------------------------------
//     // TESTING PRINT FUNCTION:
//     //   'print' has a loop, and so a set of boundary concerns, and also
//     //   a conditionally executed ('if') block that must be explored.
//     //   Still, a relatively small data set addresses these concerns.
//     //
//     // Plan:
//     //   For each of a sequence of individual tests grouped by source
//     //   category, verify that the print function gives the expected value.
//     //
//     // Testing:
//     //   void print(ostream& output, int sInt);
//     // --------------------------------------------------------------------

//         if (verbose) cout << endl
//             << "Testing print Function" << endl
//             << "======================" << endl;

//         const char *SRC1 = "11100100";    // typical case
//         const char *SRC2 = "1110010..0";  // typical case

//         static const struct {
//             int         d_lineNum;   // line number
//             const char *d_src;       // source integer
//             const char *d_print;     // expected result from print function
//         } DATA[] = {
//             //L#           src                   print result
//             //--      --------    ---------------------------
//             { L_,       "0..0",   "00000000" MID_0 "00000000" },
//             { L_,       "1..1",   "11111111" MID_1 "11111111" },

//             { L_,      "0..01",   "00000000" MID_0 "00000001" },
//             { L_,     "0..010",   "00000000" MID_0 "00000010" },
//             { L_,     "0..011",   "00000000" MID_0 "00000011" },
//             { L_,      "10..0",   "10000000" MID_0 "00000000" },
//             { L_,     "010..0",   "01000000" MID_0 "00000000" },
//             { L_,     "110..0",   "11000000" MID_0 "00000000" },
//             { L_,     "10..01",   "10000000" MID_0 "00000001" },

//             { L_,      "1..10",   "11111111" MID_1 "11111110" },
//             { L_,     "1..101",   "11111111" MID_1 "11111101" },
//             { L_,     "1..100",   "11111111" MID_1 "11111100" },
//             { L_,      "01..1",   "01111111" MID_1 "11111111" },
//             { L_,     "101..1",   "10111111" MID_1 "11111111" },
//             { L_,     "001..1",   "00111111" MID_1 "11111111" },
//             { L_,     "01..10",   "01111111" MID_1 "11111110" },

//             { L_,        FW_01,  "01010101" MID_01 "01010101" },
//             { L_,        FW_10,  "10101010" MID_10 "10101010" },

//             { L_,         SRC1,   "00000000" MID_0 "11100100" },
//             { L_,         SRC2,   "11100100" MID_0 "00000000" }
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         char *buf;
//         int len;

//         if (INT_MAX == 0x7fffffff) {
//             len = 36;
//         }
//         else {
//             len = 72;
//         }

//         buf = new char[len];

//         int di;
//         for (di = 0; di < NUM_DATA; ++di) {
//             ostrstream out(buf, len);

//             const int LINE = DATA[di].d_lineNum;
//             const char *spec = DATA[di].d_src;

//             int src = g(spec);

//             Util::print(out, src);

//             buf [len-1] = 0;

//             if (veryVerbose) {
//                 P_(LINE); P_(spec); P_(DATA[di].d_print); P(buf);
//             }

//             LOOP_ASSERT(LINE, 0 == strncmp(buf, DATA[di].d_print, len -1));
//         }

//         delete [] buf;
      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testF30()
{
        // --------------------------------------------------------------------
        // TESTING MINUSEQUAL FUNCTION:
        //   The five-argument 'minusEqual' method has a simple implementation
        //   that perform calculations using already-tested methods.  A
        //   relatively small set of test data is sufficient, but we choose a
        //   more thorough and systematic set of test data to completely probe
        //   the algorithm and not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source and
        //   then source index and initial destination category, verify that
        //   the minusEqual function sets the expected value.
        //
        // Testing:
        //   void minusEqual(int *dInt, int dIdx,int sInt,int nBits);
        //   void minusEqual(int *dInt, int dIdx,int sInt,int sIdx,int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing minusEqual Function" << endl
                          << "===========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_sindex;      // source index
            const char *d_dst;         // destination integer
            int         d_dindex;      // destination index
            int         d_numBits;     // num of bits
            int         d_minusEqual;  // expected result from minusEqual
        } DATA_A[] = {
            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPW,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "0..0", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "1..1",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     0,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "1..1",     0,  "1..1",     0,   BPW,                   0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     0,  "1..1",     1, BPW-1,                   1 },
            { L_, "1..1",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1",     0,   FW_01,     0, BPW-1,                   0 },
            { L_, "1..1",     0,   FW_01,     0,   BPW,                   0 },
            { L_, "1..1",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1, BPW-1,                   1 },
            { L_, "1..1",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0, BPW-1,             INT_MIN },
            { L_, "1..1",     0,   FW_10,     0,   BPW,                   0 },
            { L_, "1..1",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1",     0,   FW_10,     1, BPW-1,                   0 },
            { L_, "1..1",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,  g("1101101000000") },
            { L_, "1..1",     0,    DSTA,     2,     5,  g("1101100000000") },
            { L_, "1..1",     0,    DSTA,     2,     6,  g("1101100000000") },
            { L_, "1..1",     0,    DSTA,     2,     7,  g("1101000000000") },
            { L_, "1..1",     0,    DSTA,     2,     8,  g("1100000000000") },
            { L_, "1..1",     0,    DSTA,     3,     4,  g("1101100000100") },
            { L_, "1..1",     0,    DSTA,     3,     5,  g("1101100000100") },
            { L_, "1..1",     0,    DSTA,     3,     6,  g("1101000000100") },
            { L_, "1..1",     0,    DSTA,     3,     7,  g("1100000000100") },
            { L_, "1..1",     0,    DSTA,     3,     8,  g("1100000000100") },

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     1,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     1,  "1..1",     1, BPW-1,                   1 },
            { L_, "1..1",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1",     1,   FW_01,     0, BPW-1,                   0 },
            { L_, "1..1",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1, BPW-1,                   1 },
            { L_, "1..1",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0, BPW-1,             INT_MIN },
            { L_, "1..1",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1",     1,   FW_10,     1, BPW-1,                   0 },
            { L_, "1..1",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,  g("1101101000000") },
            { L_, "1..1",     1,    DSTA,     2,     5,  g("1101100000000") },
            { L_, "1..1",     1,    DSTA,     2,     6,  g("1101100000000") },
            { L_, "1..1",     1,    DSTA,     2,     7,  g("1101000000000") },
            { L_, "1..1",     1,    DSTA,     2,     8,  g("1100000000000") },
            { L_, "1..1",     1,    DSTA,     3,     4,  g("1101100000100") },
            { L_, "1..1",     1,    DSTA,     3,     5,  g("1101100000100") },
            { L_, "1..1",     1,    DSTA,     3,     6,  g("1101000000100") },
            { L_, "1..1",     1,    DSTA,     3,     7,  g("1100000000100") },
            { L_, "1..1",     1,    DSTA,     3,     8,  g("1100000000100") },

            { L_, "1..1", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,   BPW,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_01,     0,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,  "1..1",     0,   BPW,            g(FW_10) },
            { L_,  FW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_01,     0,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_01,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_01,     0,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_01,     0,   FW_01,     0,   BPW,                   0 },
            { L_,  FW_01,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_01,     0,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_01,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_01,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,    DSTA,     2,     4,  g("1101101100000") },
            { L_,  FW_01,     0,    DSTA,     2,     5,  g("1101100100000") },
            { L_,  FW_01,     0,    DSTA,     2,     6,  g("1101100100000") },
            { L_,  FW_01,     0,    DSTA,     2,     7,  g("1101000100000") },
            { L_,  FW_01,     0,    DSTA,     2,     8,  g("1101000100000") },
            { L_,  FW_01,     0,    DSTA,     3,     4,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     3,     5,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     3,     6,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     3,     7,  g("1100101000100") },
            { L_,  FW_01,     0,    DSTA,     3,     8,  g("1100101000100") },

            { L_,  FW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_,  FW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1, BPW-1,          g(FW_10)|1 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_01,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_01,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     1,    DSTA,     2,     4,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     2,     5,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     2,     6,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     2,     7,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     2,     8,  g("1100101000100") },
            { L_,  FW_01,     1,    DSTA,     3,     4,  g("1101100100100") },
            { L_,  FW_01,     1,    DSTA,     3,     5,  g("1101100100100") },
            { L_,  FW_01,     1,    DSTA,     3,     6,  g("1101000100100") },
            { L_,  FW_01,     1,    DSTA,     3,     7,  g("1101000100100") },
            { L_,  FW_01,     1,    DSTA,     3,     8,  g("1101000100100") },

            { L_,  FW_01, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,   BPW,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_,  FW_10,     0,  "1..1",     0,   BPW,            g(FW_01) },
            { L_,  FW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1, BPW-1,          g(FW_10)|1 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_10,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,   BPW,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_10,     0,     0,            g(FW_10) },

            { L_,  FW_10,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_10,     0,   FW_10,     0,   BPW,                   0 },
            { L_,  FW_10,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,    DSTA,     2,     4,  g("1101101000100") },
            { L_,  FW_10,     0,    DSTA,     2,     5,  g("1101101000100") },
            { L_,  FW_10,     0,    DSTA,     2,     6,  g("1101101000100") },
            { L_,  FW_10,     0,    DSTA,     2,     7,  g("1101101000100") },
            { L_,  FW_10,     0,    DSTA,     2,     8,  g("1100101000100") },
            { L_,  FW_10,     0,    DSTA,     3,     4,  g("1101100100100") },
            { L_,  FW_10,     0,    DSTA,     3,     5,  g("1101100100100") },
            { L_,  FW_10,     0,    DSTA,     3,     6,  g("1101000100100") },
            { L_,  FW_10,     0,    DSTA,     3,     7,  g("1101000100100") },
            { L_,  FW_10,     0,    DSTA,     3,     8,  g("1101000100100") },

            { L_,  FW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_10,     1,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_10,     1,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_10,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_10,     1,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_10,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_10,     1,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_10,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_10,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     1,    DSTA,     2,     4,  g("1101101100000") },
            { L_,  FW_10,     1,    DSTA,     2,     5,  g("1101100100000") },
            { L_,  FW_10,     1,    DSTA,     2,     6,  g("1101100100000") },
            { L_,  FW_10,     1,    DSTA,     2,     7,  g("1101000100000") },
            { L_,  FW_10,     1,    DSTA,     2,     8,  g("1101000100000") },
            { L_,  FW_10,     1,    DSTA,     3,     4,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     3,     5,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     3,     6,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     3,     7,  g("1100101000100") },
            { L_,  FW_10,     1,    DSTA,     3,     8,  g("1100101000100") },

            { L_,  FW_10, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_10, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_10, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_10, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,   BPW,     0,            g(FW_10) }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            int src = g(DATA_A[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_minusEqual); P(dst);
            }

            Util::minusEqual(&dst, DATA_A[di].d_dindex, src,
                                   DATA_A[di].d_sindex, DATA_A[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_A[di].d_minusEqual == dst);

            if (0 == DATA_A[di].d_sindex) {
                dst = g(spec);
                Util::minusEqual(&dst, DATA_A[di].d_dindex, src,
                                                         DATA_A[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_A[di].d_minusEqual == dst);
            }
         }

        if (verbose) cout << endl
                          << "Testing minusEqual64 Function" << endl
                          << "=============================" << endl;

        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_sindex;      // source index
            const char *d_dst;         // destination integer
            int         d_dindex;      // destination index
            int         d_numBits;     // num of bits
            Int64       d_minusEqual;  // expected result from minusEqual
        } DATA_B[] = {
            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPS,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,g64("1101101100100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------

            //                                  // DSTB =  "110110110010..0"
            { L_, "0..0",     0,    DSTB,    53,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     8,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     8,g64("110110110010..0")},

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,g64("1101101100100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------

            //                                  // DSTB =  "110110110010..0"
            { L_, "0..0",     1,    DSTB,    53,     4,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    53,     5,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    53,     6,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    53,     7,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    53,     8,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    54,     4,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    54,     5,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    54,     6,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    54,     7,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTB,    54,     8,g64("110110110010..0")},

            { L_, "0..0", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "1..1",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     0,  "1..1",     0, BPS-1,            int64Min },
            { L_, "1..1",     0,  "1..1",     0,   BPS,                   0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     0,  "1..1",     1, BPS-1,                   1 },
            { L_, "1..1",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1",     0,   SW_01,     0, BPS-1,                   0 },
            { L_, "1..1",     0,   SW_01,     0,   BPS,                   0 },
            { L_, "1..1",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1, BPS-1,                   1 },
            { L_, "1..1",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0, BPS-1,            int64Min },
            { L_, "1..1",     0,   SW_10,     0,   BPS,                   0 },
            { L_, "1..1",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1",     0,   SW_10,     1, BPS-1,                   0 },
            { L_, "1..1",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,g64("1101101000000") },
            { L_, "1..1",     0,    DSTA,     2,     5,g64("1101100000000") },
            { L_, "1..1",     0,    DSTA,     2,     6,g64("1101100000000") },
            { L_, "1..1",     0,    DSTA,     2,     7,g64("1101000000000") },
            { L_, "1..1",     0,    DSTA,     2,     8,g64("1100000000000") },
            { L_, "1..1",     0,    DSTA,     3,     4,g64("1101100000100") },
            { L_, "1..1",     0,    DSTA,     3,     5,g64("1101100000100") },
            { L_, "1..1",     0,    DSTA,     3,     6,g64("1101000000100") },
            { L_, "1..1",     0,    DSTA,     3,     7,g64("1100000000100") },
            { L_, "1..1",     0,    DSTA,     3,     8,g64("1100000000100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            //                                  // DSTB =  "110110110010..0"

            { L_, "1..1",     0,    DSTB,    53,     4,g64("110110100000..0")},
            { L_, "1..1",     0,    DSTB,    53,     5,g64("110110000000..0")},
            { L_, "1..1",     0,    DSTB,    53,     6,g64("110110000000..0")},
            { L_, "1..1",     0,    DSTB,    53,     7,g64("110100000000..0")},
            { L_, "1..1",     0,    DSTB,    53,     8,g64("110000000000..0")},
            { L_, "1..1",     0,    DSTB,    54,     4,g64("110110000010..0")},
            { L_, "1..1",     0,    DSTB,    54,     5,g64("110110000010..0")},
            { L_, "1..1",     0,    DSTB,    54,     6,g64("110100000010..0")},
            { L_, "1..1",     0,    DSTB,    54,     7,g64("110000000010..0")},
            { L_, "1..1",     0,    DSTB,    54,     8,g64("110000000010..0")},

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     1,  "1..1",     0, BPS-1,            int64Min },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     1,  "1..1",     1, BPS-1,                   1 },
            { L_, "1..1",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1",     1,   SW_01,     0, BPS-1,                   0 },
            { L_, "1..1",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1, BPS-1,                   1 },
            { L_, "1..1",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0, BPS-1,            int64Min },
            { L_, "1..1",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1",     1,   SW_10,     1, BPS-1,                   0 },
            { L_, "1..1",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,g64("1101101000000") },
            { L_, "1..1",     1,    DSTA,     2,     5,g64("1101100000000") },
            { L_, "1..1",     1,    DSTA,     2,     6,g64("1101100000000") },
            { L_, "1..1",     1,    DSTA,     2,     7,g64("1101000000000") },
            { L_, "1..1",     1,    DSTA,     2,     8,g64("1100000000000") },
            { L_, "1..1",     1,    DSTA,     3,     4,g64("1101100000100") },
            { L_, "1..1",     1,    DSTA,     3,     5,g64("1101100000100") },
            { L_, "1..1",     1,    DSTA,     3,     6,g64("1101000000100") },
            { L_, "1..1",     1,    DSTA,     3,     7,g64("1100000000100") },
            { L_, "1..1",     1,    DSTA,     3,     8,g64("1100000000100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            //                                  // DSTB =  "110110110010..0"

            { L_, "1..1",     1,    DSTB,    53,     4,g64("110110100000..0")},
            { L_, "1..1",     1,    DSTB,    53,     5,g64("110110000000..0")},
            { L_, "1..1",     1,    DSTB,    53,     6,g64("110110000000..0")},
            { L_, "1..1",     1,    DSTB,    53,     7,g64("110100000000..0")},
            { L_, "1..1",     1,    DSTB,    53,     8,g64("110000000000..0")},
            { L_, "1..1",     1,    DSTB,    54,     4,g64("110110000010..0")},
            { L_, "1..1",     1,    DSTB,    54,     5,g64("110110000010..0")},
            { L_, "1..1",     1,    DSTB,    54,     6,g64("110100000010..0")},
            { L_, "1..1",     1,    DSTB,    54,     7,g64("110000000010..0")},
            { L_, "1..1",     1,    DSTB,    54,     8,g64("110000000010..0")},

            { L_, "1..1", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,   BPS,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_01,     0,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,  "1..1",     0,   BPS,          g64(SW_10) },
            { L_,  SW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_01,     0,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_01,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_01,     0,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_01,     0,   SW_01,     0,   BPS,                   0 },
            { L_,  SW_01,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_01,     0,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_01,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_01,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,    DSTA,     2,     4,g64("1101101100000") },
            { L_,  SW_01,     0,    DSTA,     2,     5,g64("1101100100000") },
            { L_,  SW_01,     0,    DSTA,     2,     6,g64("1101100100000") },
            { L_,  SW_01,     0,    DSTA,     2,     7,g64("1101000100000") },
            { L_,  SW_01,     0,    DSTA,     2,     8,g64("1101000100000") },
            { L_,  SW_01,     0,    DSTA,     3,     4,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTA,     3,     5,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTA,     3,     6,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTA,     3,     7,g64("1100101000100") },
            { L_,  SW_01,     0,    DSTA,     3,     8,g64("1100101000100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            //                                  // DSTB =  "110110110010..0"

            { L_,  SW_01,     0,    DSTB,    53,     4,g64("110110110000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     5,g64("110110010000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     6,g64("110110010000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     7,g64("110100010000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     8,g64("110100010000..0")},
            { L_,  SW_01,     0,    DSTB,    54,     4,g64("110110100010..0")},
            { L_,  SW_01,     0,    DSTB,    54,     5,g64("110110100010..0")},
            { L_,  SW_01,     0,    DSTB,    54,     6,g64("110110100010..0")},
            { L_,  SW_01,     0,    DSTB,    54,     7,g64("110010100010..0")},
            { L_,  SW_01,     0,    DSTB,    54,     8,g64("110010100010..0")},

            { L_,  SW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0, BPS-1, g64(SW_01)|int64Min },
            { L_,  SW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1, BPS-1,        g64(SW_10)|1 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_01,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_01,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     1,    DSTA,     2,     4,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTA,     2,     5,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTA,     2,     6,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTA,     2,     7,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTA,     2,     8,g64("1100101000100") },
            { L_,  SW_01,     1,    DSTA,     3,     4,g64("1101100100100") },
            { L_,  SW_01,     1,    DSTA,     3,     5,g64("1101100100100") },
            { L_,  SW_01,     1,    DSTA,     3,     6,g64("1101000100100") },
            { L_,  SW_01,     1,    DSTA,     3,     7,g64("1101000100100") },
            { L_,  SW_01,     1,    DSTA,     3,     8,g64("1101000100100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            //                                  // DSTB =  "110110110010..0"

            { L_,  SW_01,     1,    DSTB,    53,     4,g64("110110100010..0")},
            { L_,  SW_01,     1,    DSTB,    53,     5,g64("110110100010..0")},
            { L_,  SW_01,     1,    DSTB,    53,     6,g64("110110100010..0")},
            { L_,  SW_01,     1,    DSTB,    53,     7,g64("110110100010..0")},
            { L_,  SW_01,     1,    DSTB,    53,     8,g64("110010100010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     4,g64("110110010010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     5,g64("110110010010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     6,g64("110100010010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     7,g64("110100010010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     8,g64("110100010010..0")},

            { L_,  SW_01, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,   BPS,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0, BPS-1, g64(SW_01)|int64Min },
            { L_,  SW_10,     0,  "1..1",     0,   BPS,          g64(SW_01) },
            { L_,  SW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1, BPS-1,        g64(SW_10)|1 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_10,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,   BPS,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_10,     0,     0,          g64(SW_10) },

            { L_,  SW_10,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_10,     0,   SW_10,     0,   BPS,                   0 },
            { L_,  SW_10,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,    DSTA,     2,     4,g64("1101101000100") },
            { L_,  SW_10,     0,    DSTA,     2,     5,g64("1101101000100") },
            { L_,  SW_10,     0,    DSTA,     2,     6,g64("1101101000100") },
            { L_,  SW_10,     0,    DSTA,     2,     7,g64("1101101000100") },
            { L_,  SW_10,     0,    DSTA,     2,     8,g64("1100101000100") },
            { L_,  SW_10,     0,    DSTA,     3,     4,g64("1101100100100") },
            { L_,  SW_10,     0,    DSTA,     3,     5,g64("1101100100100") },
            { L_,  SW_10,     0,    DSTA,     3,     6,g64("1101000100100") },
            { L_,  SW_10,     0,    DSTA,     3,     7,g64("1101000100100") },
            { L_,  SW_10,     0,    DSTA,     3,     8,g64("1101000100100") },

            { L_,  SW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_10,     1,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_10,     1,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_10,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_10,     1,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_10,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_10,     1,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_10,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_10,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     1,    DSTA,     2,     4,g64("1101101100000") },
            { L_,  SW_10,     1,    DSTA,     2,     5,g64("1101100100000") },
            { L_,  SW_10,     1,    DSTA,     2,     6,g64("1101100100000") },
            { L_,  SW_10,     1,    DSTA,     2,     7,g64("1101000100000") },
            { L_,  SW_10,     1,    DSTA,     2,     8,g64("1101000100000") },
            { L_,  SW_10,     1,    DSTA,     3,     4,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTA,     3,     5,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTA,     3,     6,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTA,     3,     7,g64("1100101000100") },
            { L_,  SW_10,     1,    DSTA,     3,     8,g64("1100101000100") },

            //L#      src   sIdx   dst     dIdx   nBits   minusEqual Result
            //--   ------   ----   -----   ----   -----   -----------------
            //                                  // DSTB =  "110110110010..0"

            { L_,  SW_10,     1,    DSTB,    53,     4,g64("110110110000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     5,g64("110110010000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     6,g64("110110010000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     7,g64("110100010000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     8,g64("110100010000..0")},
            { L_,  SW_10,     1,    DSTB,    54,     4,g64("110110100010..0")},
            { L_,  SW_10,     1,    DSTB,    54,     5,g64("110110100010..0")},
            { L_,  SW_10,     1,    DSTB,    54,     6,g64("110110100010..0")},
            { L_,  SW_10,     1,    DSTB,    54,     7,g64("110010100010..0")},
            { L_,  SW_10,     1,    DSTB,    54,     8,g64("110010100010..0")},

            { L_,  SW_10, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_10, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_10, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_10, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,   BPS,     0,          g64(SW_10) }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Int64 src = g64(DATA_B[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_minusEqual); P(dst);
            }

            Util::minusEqual64(&dst, DATA_B[di].d_dindex, src,
                                   DATA_B[di].d_sindex, DATA_B[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_B[di].d_minusEqual == dst);

            if (0 == DATA_B[di].d_sindex) {
                dst = g64(spec);
                Util::minusEqual64(&dst, DATA_B[di].d_dindex, src,
                                                         DATA_B[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_B[di].d_minusEqual == dst);
            }
         }
}

static
void testF29()
{
        // --------------------------------------------------------------------
        // TESTING XOREQUAL FUNCTION (4 & 5 ARGUMENTS):
        //   The five-argument 'xorEqual' method has a simple implementation
        //   that perform calculations using already-tested methods.  A
        //   relatively small set of test data is sufficient, but we choose a
        //   more thorough and systematic set of test data to completely probe
        //   the algorithm and not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source and
        //   then source index and initial destination category, verify that
        //   the xorEqual function sets the expected value.
        //
        // Testing:
        //   void xorEqual(int *dInt, int dIdx, int sInt, int nBits);
        //   void xorEqual64(bsls_Types::Int64 *dInt,
        //                   int dIdx,
        //                   bsls_Types::Int64 sInt,
        //                   int nBits);
        //   void xorEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits);
        //   void xorEqual64(bsls_Types::Int64 *dInt,
        //                   int dIdx,
        //                   bsls_Types::Int64 sInt,
        //                   int sIdx,
        //                   int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing xorEqual Function" << endl
                          << "=========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_sindex;      // source index
            const char *d_dst;         // destination integer
            int         d_dindex;      // destination index
            int         d_numBits;     // num of bits
            int         d_xorEqual;    // expected result from xorEqual
        } DATA_A[] = {
            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPW,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "0..0", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     0,  "0..0",     0, BPW-1,             INT_MAX },
            { L_, "1..1",     0,  "0..0",     0,   BPW,                  ~0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   2 },
            { L_, "1..1",     0,  "0..0",     1, BPW-1,                  ~1 },
            { L_, "1..1",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     0,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "1..1",     0,  "1..1",     0,   BPW,                   0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     0,  "1..1",     1, BPW-1,                   1 },
            { L_, "1..1",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1",     0,   FW_01,     0, BPW-1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     0,   FW_01,     0,   BPW,            g(FW_10) },
            { L_, "1..1",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1,     1,          g(FW_01)|2 },
            { L_, "1..1",     0,   FW_01,     1, BPW-1,          g(FW_10)|1 },
            { L_, "1..1",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_, "1..1",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_, "1..1",     0,   FW_10,     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_, "1..1",     0,   FW_10,     0,   BPW,            g(FW_01) },
            { L_, "1..1",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1",     0,   FW_10,     1, BPW-1,          g(FW_01)^1 },
            { L_, "1..1",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,  g("1101101011000") },
            { L_, "1..1",     0,    DSTA,     2,     5,  g("1101100011000") },
            { L_, "1..1",     0,    DSTA,     2,     6,  g("1101110011000") },
            { L_, "1..1",     0,    DSTA,     2,     7,  g("1101010011000") },
            { L_, "1..1",     0,    DSTA,     2,     8,  g("1100010011000") },
            { L_, "1..1",     0,    DSTA,     3,     4,  g("1101100011100") },
            { L_, "1..1",     0,    DSTA,     3,     5,  g("1101110011100") },
            { L_, "1..1",     0,    DSTA,     3,     6,  g("1101010011100") },
            { L_, "1..1",     0,    DSTA,     3,     7,  g("1100010011100") },
            { L_, "1..1",     0,    DSTA,     3,     8,  g("1110010011100") },

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     1,  "0..0",     0, BPW-1,             INT_MAX },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   2 },
            { L_, "1..1",     1,  "0..0",     1, BPW-1,                  ~1 },
            { L_, "1..1",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     1,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     1,  "1..1",     1, BPW-1,                   1 },
            { L_, "1..1",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1",     1,   FW_01,     0, BPW-1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1,     1,          g(FW_01)|2 },
            { L_, "1..1",     1,   FW_01,     1, BPW-1,          g(FW_10)|1 },
            { L_, "1..1",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_, "1..1",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_, "1..1",     1,   FW_10,     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_, "1..1",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1",     1,   FW_10,     1, BPW-1,          g(FW_01)^1 },
            { L_, "1..1",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,  g("1101101011000") },
            { L_, "1..1",     1,    DSTA,     2,     5,  g("1101100011000") },
            { L_, "1..1",     1,    DSTA,     2,     6,  g("1101110011000") },
            { L_, "1..1",     1,    DSTA,     2,     7,  g("1101010011000") },
            { L_, "1..1",     1,    DSTA,     2,     8,  g("1100010011000") },
            { L_, "1..1",     1,    DSTA,     3,     4,  g("1101100011100") },
            { L_, "1..1",     1,    DSTA,     3,     5,  g("1101110011100") },
            { L_, "1..1",     1,    DSTA,     3,     6,  g("1101010011100") },
            { L_, "1..1",     1,    DSTA,     3,     7,  g("1100010011100") },
            { L_, "1..1",     1,    DSTA,     3,     8,  g("1110010011100") },

            { L_, "1..1", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     0,     1,                   1 },
            { L_, "1..1", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     1,                   2 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "1..1", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_, "1..1", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     1,          g(FW_01)|2 },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_, "1..1", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_, "1..1", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_, "1..1", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,     1,                   1 },
            { L_,  FW_01,     0,  "0..0",     0, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,  "0..0",     0,   BPW,            g(FW_01) },
            { L_,  FW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     1,                   2 },
            { L_,  FW_01,     0,  "0..0",     1, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_01,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_01,     0,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,  "1..1",     0,   BPW,            g(FW_10) },
            { L_,  FW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_01,     0,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_01,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_01,     0,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_01,     0,   FW_01,     0,   BPW,                   0 },
            { L_,  FW_01,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     1,          g(FW_01)|2 },
            { L_,  FW_01,     0,   FW_01,     1, BPW-1,                  ~0 },
            { L_,  FW_01,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_,  FW_01,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_,  FW_01,     0,   FW_10,     0, BPW-1,                  ~0 },
            { L_,  FW_01,     0,   FW_10,     0,   BPW,                  ~0 },
            { L_,  FW_01,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_01,     0,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_01,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_01,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,    DSTA,     2,     4,  g("1101101110000") },
            { L_,  FW_01,     0,    DSTA,     2,     5,  g("1101100110000") },
            { L_,  FW_01,     0,    DSTA,     2,     6,  g("1101100110000") },
            { L_,  FW_01,     0,    DSTA,     2,     7,  g("1101000110000") },
            { L_,  FW_01,     0,    DSTA,     2,     8,  g("1101000110000") },
            { L_,  FW_01,     0,    DSTA,     3,     4,  g("1101101001100") },
            { L_,  FW_01,     0,    DSTA,     3,     5,  g("1101111001100") },
            { L_,  FW_01,     0,    DSTA,     3,     6,  g("1101111001100") },
            { L_,  FW_01,     0,    DSTA,     3,     7,  g("1100111001100") },
            { L_,  FW_01,     0,    DSTA,     3,     8,  g("1100111001100") },

            { L_,  FW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     0, BPW-1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1, BPW-1,          g(FW_01)^1 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_,  FW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1, BPW-1,          g(FW_10)|1 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0, BPW-1,             INT_MAX },
            { L_,  FW_01,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_01,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_01,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1, BPW-1,                  ~1 },
            { L_,  FW_01,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     1,    DSTA,     2,     4,  g("1101101001100") },
            { L_,  FW_01,     1,    DSTA,     2,     5,  g("1101101001100") },
            { L_,  FW_01,     1,    DSTA,     2,     6,  g("1101111001100") },
            { L_,  FW_01,     1,    DSTA,     2,     7,  g("1101111001100") },
            { L_,  FW_01,     1,    DSTA,     2,     8,  g("1100111001100") },
            { L_,  FW_01,     1,    DSTA,     3,     4,  g("1101100110100") },
            { L_,  FW_01,     1,    DSTA,     3,     5,  g("1101100110100") },
            { L_,  FW_01,     1,    DSTA,     3,     6,  g("1101000110100") },
            { L_,  FW_01,     1,    DSTA,     3,     7,  g("1101000110100") },
            { L_,  FW_01,     1,    DSTA,     3,     8,  g("1111000110100") },

            { L_,  FW_01, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0, BPW-1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_10,     0,  "0..0",     0,   BPW,            g(FW_10) },
            { L_,  FW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     1, BPW-1,          g(FW_01)^1 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0, BPW-1,    g(FW_01)|INT_MIN },
            { L_,  FW_10,     0,  "1..1",     0,   BPW,            g(FW_01) },
            { L_,  FW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1, BPW-1,          g(FW_10)|1 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0, BPW-1,             INT_MAX },
            { L_,  FW_10,     0,   FW_01,     0,   BPW,                  ~0 },
            { L_,  FW_10,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_10,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_10,     0,   FW_10,     0,   BPW,                   0 },
            { L_,  FW_10,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1, BPW-1,                  ~1 },
            { L_,  FW_10,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,    DSTA,     2,     4,  g("1101101001100") },
            { L_,  FW_10,     0,    DSTA,     2,     5,  g("1101101001100") },
            { L_,  FW_10,     0,    DSTA,     2,     6,  g("1101111001100") },
            { L_,  FW_10,     0,    DSTA,     2,     7,  g("1101111001100") },
            { L_,  FW_10,     0,    DSTA,     2,     8,  g("1100111001100") },
            { L_,  FW_10,     0,    DSTA,     3,     4,  g("1101100110100") },
            { L_,  FW_10,     0,    DSTA,     3,     5,  g("1101100110100") },
            { L_,  FW_10,     0,    DSTA,     3,     6,  g("1101000110100") },
            { L_,  FW_10,     0,    DSTA,     3,     7,  g("1101000110100") },
            { L_,  FW_10,     0,    DSTA,     3,     8,  g("1111000110100") },

            { L_,  FW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     0,     1,                   1 },
            { L_,  FW_10,     1,  "0..0",     0, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     1,                   2 },
            { L_,  FW_10,     1,  "0..0",     1, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_10,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_10,     1,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_10,     1,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_10,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_10,     1,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_10,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     1,          g(FW_01)|2 },
            { L_,  FW_10,     1,   FW_01,     1, BPW-1,                  ~0 },
            { L_,  FW_10,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_,  FW_10,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_,  FW_10,     1,   FW_10,     0, BPW-1,                  ~0 },
            { L_,  FW_10,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_10,     1,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_10,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_10,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     1,    DSTA,     2,     4,  g("1101101110000") },
            { L_,  FW_10,     1,    DSTA,     2,     5,  g("1101100110000") },
            { L_,  FW_10,     1,    DSTA,     2,     6,  g("1101100110000") },
            { L_,  FW_10,     1,    DSTA,     2,     7,  g("1101000110000") },
            { L_,  FW_10,     1,    DSTA,     2,     8,  g("1101000110000") },
            { L_,  FW_10,     1,    DSTA,     3,     4,  g("1101101001100") },
            { L_,  FW_10,     1,    DSTA,     3,     5,  g("1101111001100") },
            { L_,  FW_10,     1,    DSTA,     3,     6,  g("1101111001100") },
            { L_,  FW_10,     1,    DSTA,     3,     7,  g("1100111001100") },
            { L_,  FW_10,     1,    DSTA,     3,     8,  g("1100111001100") },

            { L_,  FW_10, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     0,     1,                   1 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     1,                   2 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_10, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_10, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     0,     1,          g(FW_01)^1 },
            { L_,  FW_10, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     1,          g(FW_01)^2 },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     1,    g(FW_01)|INT_MIN },
            { L_,  FW_10, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     0,     1,          g(FW_10)|1 },
            { L_,  FW_10, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     1,          g(FW_10)^2 },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     1,    g(FW_10)^INT_MIN },
            { L_,  FW_10, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,   BPW,     0,            g(FW_10) }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            int src = g(DATA_A[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_xorEqual); P(dst);
            }

            Util::xorEqual(&dst, DATA_A[di].d_dindex, src,
                                   DATA_A[di].d_sindex, DATA_A[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_A[di].d_xorEqual == dst);

            if (0 == DATA_A[di].d_sindex) {
                int dst = g(spec);
                Util::xorEqual(&dst, DATA_A[di].d_dindex, src,
                                                         DATA_A[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_A[di].d_xorEqual == dst);
            }
         }

        if (verbose) cout << endl
                          << "Testing xorEqual64 Function" << endl
                          << "===========================" << endl;

        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_sindex;      // source index
            const char *d_dst;         // destination integer
            int         d_dindex;      // destination index
            int         d_numBits;     // num of bits
            Int64       d_xorEqual;    // expected result from xorEqual
        } DATA_B[] = {
            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPS,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "0..0",     0,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,g64("1101101100100") },

            { L_, "0..0",     0,    DSTB,    53,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    53,     8,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTB,    54,     8,g64("110110110010..0")},

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "0..0",     1,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,g64("1101101100100") },

            { L_, "0..0",     1,    DSTB,     2,     4,  g64(DSTB) },
            { L_, "0..0",     2,    DSTB,     2,     5,  g64(DSTB) },
            { L_, "0..0",     3,    DSTB,     2,     6,  g64(DSTB) },
            { L_, "0..0",     4,    DSTB,     2,     7,  g64(DSTB) },
            { L_, "0..0",     5,    DSTB,     2,     8,  g64(DSTB) },
            { L_, "0..0",     6,    DSTB,     3,     4,  g64(DSTB) },
            { L_, "0..0",     7,    DSTB,     3,     5,  g64(DSTB) },
            { L_, "0..0",    41,    DSTB,     3,     6,  g64(DSTB) },
            { L_, "0..0",    42,    DSTB,     3,     7,  g64(DSTB) },
            { L_, "0..0",    43,    DSTB,     3,     8,  g64(DSTB) },

            { L_, "0..0", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     0,  "0..0",     0, BPS-1,            int64Max },
            { L_, "1..1",     0,  "0..0",     0,   BPS,                  ~0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   2 },
            { L_, "1..1",     0,  "0..0",     1, BPS-1,                  ~1 },
            { L_, "1..1",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     0,  "1..1",     0, BPS-1,            int64Min },
            { L_, "1..1",     0,  "1..1",     0,   BPS,                   0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     0,  "1..1",     1, BPS-1,                   1 },
            { L_, "1..1",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1",     0,   SW_01,     0, BPS-1, g64(SW_10)^int64Min },
            { L_, "1..1",     0,   SW_01,     0,   BPS,          g64(SW_10) },
            { L_, "1..1",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1,     1,        g64(SW_01)|2 },
            { L_, "1..1",     0,   SW_01,     1, BPS-1,        g64(SW_10)|1 },
            { L_, "1..1",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_, "1..1",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_, "1..1",     0,   SW_10,     0, BPS-1, g64(SW_01)|int64Min },
            { L_, "1..1",     0,   SW_10,     0,   BPS,          g64(SW_01) },
            { L_, "1..1",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1",     0,   SW_10,     1, BPS-1,        g64(SW_01)^1 },
            { L_, "1..1",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "1..1",     0,    DSTA,     2,     4,g64("1101101011000") },
            { L_, "1..1",     0,    DSTA,     2,     5,g64("1101100011000") },
            { L_, "1..1",     0,    DSTA,     2,     6,g64("1101110011000") },
            { L_, "1..1",     0,    DSTA,     2,     7,g64("1101010011000") },
            { L_, "1..1",     0,    DSTA,     2,     8,g64("1100010011000") },
            { L_, "1..1",     0,    DSTA,     3,     4,g64("1101100011100") },
            { L_, "1..1",     0,    DSTA,     3,     5,g64("1101110011100") },
            { L_, "1..1",     0,    DSTA,     3,     6,g64("1101010011100") },
            { L_, "1..1",     0,    DSTA,     3,     7,g64("1100010011100") },
            { L_, "1..1",     0,    DSTA,     3,     8,g64("1110010011100") },

            { L_, "1..1",     0,    DSTB,    53,     4,g64("110110101100..0")},
            { L_, "1..1",     0,    DSTB,    53,     5,g64("110110001100..0")},
            { L_, "1..1",     0,    DSTB,    53,     6,g64("110111001100..0")},
            { L_, "1..1",     0,    DSTB,    53,     7,g64("110101001100..0")},
            { L_, "1..1",     0,    DSTB,    53,     8,g64("110001001100..0")},
            { L_, "1..1",     0,    DSTB,    54,     4,g64("110110001110..0")},
            { L_, "1..1",     0,    DSTB,    54,     5,g64("110111001110..0")},
            { L_, "1..1",     0,    DSTB,    54,     6,g64("110101001110..0")},
            { L_, "1..1",     0,    DSTB,    54,     7,g64("110001001110..0")},
            { L_, "1..1",     0,    DSTB,    54,     8,g64("111001001110..0")},

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     1,  "0..0",     0, BPS-1,            int64Max },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   2 },
            { L_, "1..1",     1,  "0..0",     1, BPS-1,                  ~1 },
            { L_, "1..1",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1",     1,  "1..1",     0, BPS-1,            int64Min },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1",     1,  "1..1",     1, BPS-1,                   1 },
            { L_, "1..1",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1",     1,   SW_01,     0, BPS-1, g64(SW_10)^int64Min },
            { L_, "1..1",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1,     1,        g64(SW_01)|2 },
            { L_, "1..1",     1,   SW_01,     1, BPS-1,        g64(SW_10)|1 },
            { L_, "1..1",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_, "1..1",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_, "1..1",     1,   SW_10,     0, BPS-1, g64(SW_01)|int64Min },
            { L_, "1..1",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1",     1,   SW_10,     1, BPS-1,        g64(SW_01)^1 },
            { L_, "1..1",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_, "1..1",     1,    DSTA,     2,     4,g64("1101101011000") },
            { L_, "1..1",     1,    DSTA,     2,     5,g64("1101100011000") },
            { L_, "1..1",     1,    DSTA,     2,     6,g64("1101110011000") },
            { L_, "1..1",     1,    DSTA,     2,     7,g64("1101010011000") },
            { L_, "1..1",     1,    DSTA,     2,     8,g64("1100010011000") },
            { L_, "1..1",     1,    DSTA,     3,     4,g64("1101100011100") },
            { L_, "1..1",     1,    DSTA,     3,     5,g64("1101110011100") },
            { L_, "1..1",     1,    DSTA,     3,     6,g64("1101010011100") },
            { L_, "1..1",     1,    DSTA,     3,     7,g64("1100010011100") },
            { L_, "1..1",     1,    DSTA,     3,     8,g64("1110010011100") },

            { L_, "1..1",     1,    DSTB,    53,     4,g64("110110101100..0")},
            { L_, "1..1",     8,    DSTB,    53,     5,g64("110110001100..0")},
            { L_, "1..1",    16,    DSTB,    53,     6,g64("110111001100..0")},
            { L_, "1..1",    32,    DSTB,    53,     7,g64("110101001100..0")},
            { L_, "1..1",    44,    DSTB,    53,     8,g64("110001001100..0")},
            { L_, "1..1",    45,    DSTB,    54,     4,g64("110110001110..0")},
            { L_, "1..1",    46,    DSTB,    54,     5,g64("110111001110..0")},
            { L_, "1..1",    47,    DSTB,    54,     6,g64("110101001110..0")},
            { L_, "1..1",    48,    DSTB,    54,     7,g64("110001001110..0")},
            { L_, "1..1",    50,    DSTB,    54,     8,g64("111001001110..0")},

            { L_, "1..1", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     0,     1,                   1 },
            { L_, "1..1", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     1,                   2 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_, "1..1", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     1,                  ~2 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "1..1", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_, "1..1", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     1,        g64(SW_01)|2 },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_, "1..1", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_, "1..1", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "1..1", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,     1,                   1 },
            { L_,  SW_01,     0,  "0..0",     0, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,  "0..0",     0,   BPS,          g64(SW_01) },
            { L_,  SW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     1,                   2 },
            { L_,  SW_01,     0,  "0..0",     1, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_01,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_01,     0,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,  "1..1",     0,   BPS,          g64(SW_10) },
            { L_,  SW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_01,     0,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_01,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_01,     0,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_01,     0,   SW_01,     0,   BPS,                   0 },
            { L_,  SW_01,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     1,        g64(SW_01)|2 },
            { L_,  SW_01,     0,   SW_01,     1, BPS-1,                  ~0 },
            { L_,  SW_01,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_,  SW_01,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_,  SW_01,     0,   SW_10,     0, BPS-1,                  ~0 },
            { L_,  SW_01,     0,   SW_10,     0,   BPS,                  ~0 },
            { L_,  SW_01,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_01,     0,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_01,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_01,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------
            { L_,  SW_01,     0,    DSTA,     2,     4,g64("1101101110000") },
            { L_,  SW_01,     0,    DSTA,     2,     5,g64("1101100110000") },
            { L_,  SW_01,     0,    DSTA,     2,     6,g64("1101100110000") },
            { L_,  SW_01,     0,    DSTA,     2,     7,g64("1101000110000") },
            { L_,  SW_01,     0,    DSTA,     2,     8,g64("1101000110000") },
            { L_,  SW_01,     0,    DSTA,     3,     4,g64("1101101001100") },
            { L_,  SW_01,     0,    DSTA,     3,     5,g64("1101111001100") },
            { L_,  SW_01,     0,    DSTA,     3,     6,g64("1101111001100") },
            { L_,  SW_01,     0,    DSTA,     3,     7,g64("1100111001100") },
            { L_,  SW_01,     0,    DSTA,     3,     8,g64("1100111001100") },

            { L_,  SW_01,     0,    DSTB,    53,     4,g64("110110111000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     5,g64("110110011000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     6,g64("110110011000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     7,g64("110100011000..0")},
            { L_,  SW_01,     0,    DSTB,    53,     8,g64("110100011000..0")},
            { L_,  SW_01,     0,    DSTB,    54,     4,g64("110110100110..0")},
            { L_,  SW_01,     0,    DSTB,    54,     5,g64("110111100110..0")},
            { L_,  SW_01,     0,    DSTB,    54,     6,g64("110111100110..0")},
            { L_,  SW_01,     0,    DSTB,    54,     7,g64("110011100110..0")},
            { L_,  SW_01,     0,    DSTB,    54,     8,g64("110011100110..0")},

            { L_,  SW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     0, BPS-1, g64(SW_10)^int64Min },
            { L_,  SW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1, BPS-1,        g64(SW_01)^1 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0, BPS-1, g64(SW_01)|int64Min },
            { L_,  SW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1, BPS-1,        g64(SW_10)|1 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0, BPS-1,            int64Max },
            { L_,  SW_01,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_01,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_01,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1, BPS-1,                  ~1 },
            { L_,  SW_01,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------

            { L_,  SW_01,     1,    DSTA,     2,     4,g64("1101101001100") },
            { L_,  SW_01,     1,    DSTA,     2,     5,g64("1101101001100") },
            { L_,  SW_01,     1,    DSTA,     2,     6,g64("1101111001100") },
            { L_,  SW_01,     1,    DSTA,     2,     7,g64("1101111001100") },
            { L_,  SW_01,     1,    DSTA,     2,     8,g64("1100111001100") },
            { L_,  SW_01,     1,    DSTA,     3,     4,g64("1101100110100") },
            { L_,  SW_01,     1,    DSTA,     3,     5,g64("1101100110100") },
            { L_,  SW_01,     1,    DSTA,     3,     6,g64("1101000110100") },
            { L_,  SW_01,     1,    DSTA,     3,     7,g64("1101000110100") },
            { L_,  SW_01,     1,    DSTA,     3,     8,g64("1111000110100") },

            { L_,  SW_01,     1,    DSTB,    53,     4,g64("110110100110..0")},
            { L_,  SW_01,     1,    DSTB,    53,     5,g64("110110100110..0")},
            { L_,  SW_01,     1,    DSTB,    53,     6,g64("110111100110..0")},
            { L_,  SW_01,     1,    DSTB,    53,     7,g64("110111100110..0")},
            { L_,  SW_01,     1,    DSTB,    53,     8,g64("110011100110..0")},
            { L_,  SW_01,     1,    DSTB,    54,     4,g64("110110011010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     5,g64("110110011010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     6,g64("110100011010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     7,g64("110100011010..0")},
            { L_,  SW_01,     1,    DSTB,    54,     8,g64("111100011010..0")},

            { L_,  SW_01, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0, BPS-1, g64(SW_10)^int64Min },
            { L_,  SW_10,     0,  "0..0",     0,   BPS,          g64(SW_10) },
            { L_,  SW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     1, BPS-1,        g64(SW_01)^1 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0, BPS-1, g64(SW_01)|int64Min },
            { L_,  SW_10,     0,  "1..1",     0,   BPS,          g64(SW_01) },
            { L_,  SW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1, BPS-1,        g64(SW_10)|1 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0, BPS-1,            int64Max },
            { L_,  SW_10,     0,   SW_01,     0,   BPS,                  ~0 },
            { L_,  SW_10,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_10,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_10,     0,   SW_10,     0,   BPS,                   0 },
            { L_,  SW_10,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1, BPS-1,                  ~1 },
            { L_,  SW_10,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            //L#    src    sIdx     dst    dIdx  nBits      xorEqual Result
            //--  ------    ---   ------   ----   ----  -------------------

            { L_,  SW_10,     0,    DSTA,     2,     4,g64("1101101001100") },
            { L_,  SW_10,     0,    DSTA,     2,     5,g64("1101101001100") },
            { L_,  SW_10,     0,    DSTA,     2,     6,g64("1101111001100") },
            { L_,  SW_10,     0,    DSTA,     2,     7,g64("1101111001100") },
            { L_,  SW_10,     0,    DSTA,     2,     8,g64("1100111001100") },
            { L_,  SW_10,     0,    DSTA,     3,     4,g64("1101100110100") },
            { L_,  SW_10,     0,    DSTA,     3,     5,g64("1101100110100") },
            { L_,  SW_10,     0,    DSTA,     3,     6,g64("1101000110100") },
            { L_,  SW_10,     0,    DSTA,     3,     7,g64("1101000110100") },
            { L_,  SW_10,     0,    DSTA,     3,     8,g64("1111000110100") },

            { L_,  SW_10,     0,    DSTB,    53,     4,g64("110110100110..0")},
            { L_,  SW_10,     0,    DSTB,    53,     5,g64("110110100110..0")},
            { L_,  SW_10,     0,    DSTB,    53,     6,g64("110111100110..0")},
            { L_,  SW_10,     0,    DSTB,    53,     7,g64("110111100110..0")},
            { L_,  SW_10,     0,    DSTB,    53,     8,g64("110011100110..0")},
            { L_,  SW_10,     0,    DSTB,    54,     4,g64("110110011010..0")},
            { L_,  SW_10,     0,    DSTB,    54,     5,g64("110110011010..0")},
            { L_,  SW_10,     0,    DSTB,    54,     6,g64("110100011010..0")},
            { L_,  SW_10,     0,    DSTB,    54,     7,g64("110100011010..0")},
            { L_,  SW_10,     0,    DSTB,    54,     8,g64("111100011010..0")},

            { L_,  SW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     0,     1,                   1 },
            { L_,  SW_10,     1,  "0..0",     0, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     1,                   2 },
            { L_,  SW_10,     1,  "0..0",     1, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_10,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_10,     1,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_10,     1,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_10,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_10,     1,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_10,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     1,        g64(SW_01)|2 },
            { L_,  SW_10,     1,   SW_01,     1, BPS-1,                  ~0 },
            { L_,  SW_10,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_,  SW_10,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_,  SW_10,     1,   SW_10,     0, BPS-1,                  ~0 },
            { L_,  SW_10,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_10,     1,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_10,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_10,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     1,    DSTA,     2,     4,g64("1101101110000") },
            { L_,  SW_10,     1,    DSTA,     2,     5,g64("1101100110000") },
            { L_,  SW_10,     1,    DSTA,     2,     6,g64("1101100110000") },
            { L_,  SW_10,     1,    DSTA,     2,     7,g64("1101000110000") },
            { L_,  SW_10,     1,    DSTA,     2,     8,g64("1101000110000") },
            { L_,  SW_10,     1,    DSTA,     3,     4,g64("1101101001100") },
            { L_,  SW_10,     1,    DSTA,     3,     5,g64("1101111001100") },
            { L_,  SW_10,     1,    DSTA,     3,     6,g64("1101111001100") },
            { L_,  SW_10,     1,    DSTA,     3,     7,g64("1100111001100") },
            { L_,  SW_10,     1,    DSTA,     3,     8,g64("1100111001100") },

            { L_,  SW_10,     1,    DSTB,    53,     4,g64("110110111000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     5,g64("110110011000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     6,g64("110110011000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     7,g64("110100011000..0")},
            { L_,  SW_10,     1,    DSTB,    53,     8,g64("110100011000..0")},
            { L_,  SW_10,     1,    DSTB,    54,     4,g64("110110100110..0")},
            { L_,  SW_10,     1,    DSTB,    54,     5,g64("110111100110..0")},
            { L_,  SW_10,     1,    DSTB,    54,     6,g64("110111100110..0")},
            { L_,  SW_10,     1,    DSTB,    54,     7,g64("110011100110..0")},
            { L_,  SW_10,     1,    DSTB,    54,     8,g64("110011100110..0")},

            { L_,  SW_10, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     0,     1,                   1 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     1,                   2 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_10, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_10, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     0,     1,        g64(SW_01)^1 },
            { L_,  SW_10, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     1,        g64(SW_01)^2 },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     1, g64(SW_01)|int64Min },
            { L_,  SW_10, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     0,     1,        g64(SW_10)|1 },
            { L_,  SW_10, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     1,        g64(SW_10)^2 },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_10, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,   BPS,     0,          g64(SW_10) }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Int64 src = g64(DATA_B[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_xorEqual); P(dst);
            }

            Util::xorEqual64(&dst, DATA_B[di].d_dindex, src,
                                   DATA_B[di].d_sindex, DATA_B[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_B[di].d_xorEqual == dst);

            if (0 == DATA_B[di].d_sindex) {
                dst = g64(spec);
                Util::xorEqual64(&dst, DATA_B[di].d_dindex, src,
                                                         DATA_B[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_B[di].d_xorEqual == dst);
            }
         }
}

static
void testF28()
{
        // --------------------------------------------------------------------
        // TESTING OREQUAL FUNCTION (5 ARGUMENTS):
        //   The five-argument 'orEqual' method has a simple implementation
        //   that perform calculations using already-tested methods.  A
        //   relatively small set of test data is sufficient, but we choose a
        //   more thorough and systematic set of test data to completely probe
        //   the algorithm and not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source and
        //   then source index and initial destination category, verify that
        //   the orEqual function sets the expected value.
        //
        // Testing:
        //   void orEqual(int *dInt, int dIdx, int sInt, int nBits);
        //   void orEqual64(bsls_Types::Int64 *dInt, int dIdx,
        //                  bsls_Types::Int64 sInt, int nBits);
        //   void orEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits);
        //   void orEqual64(bsls_Types::Int64 *dInt, int dIdx,
        //                  bsls_Types::Int64 sInt, int sIdx,
        //                  int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing orEqual Function" << endl
                          << "========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        const char *DSTB = "110110110010..0";  // typical case
        const int DBF = 19;     // DSTB floor -- offset of DSTA offsets to
                                // convert them to DSTB offsets
        static const struct {
            int         d_lineNum;   // line number
            const char *d_src;       // source integer
            int         d_sindex;    // source index
            const char *d_dst;       // destination integer
            int         d_dindex;    // destination index
            int         d_numBits;   // num of bits
            int         d_orEqual;   // expected result from orEqual function
        } DATA_A[] = {
            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------

            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPW,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,  g("1101101100100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_, "0..0",     0,    DSTB, DBF+2,     4,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+2,     5,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+2,     6,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+2,     7,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+2,     8,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+3,     4,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+3,     5,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+3,     6,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+3,     7,  g("110110110010..0")},
            { L_, "0..0",     0,    DSTB, DBF+3,     8,  g("110110110010..0")},

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,  g("1101101100100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_, "0..0",     1,    DSTB, DBF+2,     4,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+2,     5,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+2,     6,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+2,     7,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+2,     8,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+3,     4,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+3,     5,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+3,     6,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+3,     7,  g("110110110010..0")},
            { L_, "0..0",     1,    DSTB, DBF+3,     8,  g("110110110010..0")},

            { L_, "0..0", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     0,  "0..0",     0, BPW-1,             INT_MAX },
            { L_, "1..1",     0,  "0..0",     0,   BPW,                  ~0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                1<<1 },
            { L_, "1..1",     0,  "0..0",     1, BPW-1,                  ~1 },
            { L_, "1..1",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,   BPW,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0, BPW-1,             INT_MAX },
            { L_, "1..1",     0,   FW_01,     0,   BPW,                  ~0 },
            { L_, "1..1",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1,     1,   g(FW_01) | (1<<1) },
            { L_, "1..1",     0,   FW_01,     1, BPW-1,                  ~0 },
            { L_, "1..1",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_, "1..1",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_, "1..1",     0,   FW_10,     0, BPW-1,                  ~0 },
            { L_, "1..1",     0,   FW_10,     0,   BPW,                  ~0 },
            { L_, "1..1",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1, BPW-1,                  ~1 },
            { L_, "1..1",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,  g("1101101111100") },
            { L_, "1..1",     0,    DSTA,     2,     5,  g("1101101111100") },
            { L_, "1..1",     0,    DSTA,     2,     6,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     2,     7,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     2,     8,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     4,  g("1101101111100") },
            { L_, "1..1",     0,    DSTA,     3,     5,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     6,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     7,  g("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     8,  g("1111111111100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_, "1..1",     0,    DSTB, DBF+2,     4,  g("110110111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+2,     5,  g("110110111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+2,     6,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+2,     7,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+2,     8,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+3,     4,  g("110110111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+3,     5,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+3,     6,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+3,     7,  g("110111111110..0")},
            { L_, "1..1",     0,    DSTB, DBF+3,     8,  g("111111111110..0")},

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     1,  "0..0",     0, BPW-1,             INT_MAX },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                1<<1 },
            { L_, "1..1",     1,  "0..0",     1, BPW-1,                  ~1 },
            { L_, "1..1",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0, BPW-1,             INT_MAX },
            { L_, "1..1",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1,     1,   g(FW_01) | (1<<1) },
            { L_, "1..1",     1,   FW_01,     1, BPW-1,                  ~0 },
            { L_, "1..1",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_, "1..1",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_, "1..1",     1,   FW_10,     0, BPW-1,                  ~0 },
            { L_, "1..1",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1, BPW-1,                  ~1 },
            { L_, "1..1",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,  g("1101101111100") },
            { L_, "1..1",     1,    DSTA,     2,     5,  g("1101101111100") },
            { L_, "1..1",     1,    DSTA,     2,     6,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     2,     7,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     2,     8,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     4,  g("1101101111100") },
            { L_, "1..1",     1,    DSTA,     3,     5,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     6,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     7,  g("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     8,  g("1111111111100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_, "1..1",     1,    DSTB, DBF+2,     4,  g("110110111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+2,     5,  g("110110111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+2,     6,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+2,     7,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+2,     8,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+3,     4,  g("110110111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+3,     5,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+3,     6,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+3,     7,  g("110111111110..0")},
            { L_, "1..1",     1,    DSTB, DBF+3,     8,  g("111111111110..0")},

            { L_, "1..1", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     0,     1,                   1 },
            { L_, "1..1", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     1,                   2 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_, "1..1", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     1,         g(FW_01) | 2},
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_, "1..1", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_, "1..1", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,     1,                   1 },
            { L_,  FW_01,     0,  "0..0",     0, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,  "0..0",     0,   BPW,            g(FW_01) },
            { L_,  FW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     1,                1<<1 },
            { L_,  FW_01,     0,  "0..0",     1, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_01,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0,   BPW,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     1,   g(FW_01) | (1<<1) },
            { L_,  FW_01,     0,   FW_01,     1, BPW-1,                  ~0 },
            { L_,  FW_01,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_,  FW_01,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_,  FW_01,     0,   FW_10,     0, BPW-1,                  ~0 },
            { L_,  FW_01,     0,   FW_10,     0,   BPW,                  ~0 },
            { L_,  FW_01,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,    DSTA,     2,     4,  g("1101101110100") },
            { L_,  FW_01,     0,    DSTA,     2,     5,  g("1101101110100") },
            { L_,  FW_01,     0,    DSTA,     2,     6,  g("1101101110100") },
            { L_,  FW_01,     0,    DSTA,     2,     7,  g("1101101110100") },
            { L_,  FW_01,     0,    DSTA,     2,     8,  g("1101101110100") },
            { L_,  FW_01,     0,    DSTA,     3,     4,  g("1101101101100") },
            { L_,  FW_01,     0,    DSTA,     3,     5,  g("1101111101100") },
            { L_,  FW_01,     0,    DSTA,     3,     6,  g("1101111101100") },
            { L_,  FW_01,     0,    DSTA,     3,     7,  g("1101111101100") },
            { L_,  FW_01,     0,    DSTA,     3,     8,  g("1101111101100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_,  FW_01,     0,    DSTB, DBF+2,     4,  g("110110111010..0")},
            { L_,  FW_01,     0,    DSTB, DBF+2,     5,  g("110110111010..0")},
            { L_,  FW_01,     0,    DSTB, DBF+2,     6,  g("110110111010..0")},
            { L_,  FW_01,     0,    DSTB, DBF+2,     7,  g("110110111010..0")},
            { L_,  FW_01,     0,    DSTB, DBF+2,     8,  g("110110111010..0")},
            { L_,  FW_01,     0,    DSTB, DBF+3,     4,  g("110110110110..0")},
            { L_,  FW_01,     0,    DSTB, DBF+3,     5,  g("110111110110..0")},
            { L_,  FW_01,     0,    DSTB, DBF+3,     6,  g("110111110110..0")},
            { L_,  FW_01,     0,    DSTB, DBF+3,     7,  g("110111110110..0")},
            { L_,  FW_01,     0,    DSTB, DBF+3,     8,  g("110111110110..0")},

            { L_,  FW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     0, BPW-1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1, BPW-1,        g(FW_01) ^ 1 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0, BPW-1,             INT_MAX },
            { L_,  FW_01,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1, BPW-1,                  ~1 },
            { L_,  FW_01,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     1,    DSTA,     2,     4,  g("1101101101100") },
            { L_,  FW_01,     1,    DSTA,     2,     5,  g("1101101101100") },
            { L_,  FW_01,     1,    DSTA,     2,     6,  g("1101111101100") },
            { L_,  FW_01,     1,    DSTA,     2,     7,  g("1101111101100") },
            { L_,  FW_01,     1,    DSTA,     2,     8,  g("1101111101100") },
            { L_,  FW_01,     1,    DSTA,     3,     4,  g("1101101110100") },
            { L_,  FW_01,     1,    DSTA,     3,     5,  g("1101101110100") },
            { L_,  FW_01,     1,    DSTA,     3,     6,  g("1101101110100") },
            { L_,  FW_01,     1,    DSTA,     3,     7,  g("1101101110100") },
            { L_,  FW_01,     1,    DSTA,     3,     8,  g("1111101110100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_,  FW_01,     1,    DSTB, DBF+2,     4,  g("110110110110..0")},
            { L_,  FW_01,     1,    DSTB, DBF+2,     5,  g("110110110110..0")},
            { L_,  FW_01,     1,    DSTB, DBF+2,     6,  g("110111110110..0")},
            { L_,  FW_01,     1,    DSTB, DBF+2,     7,  g("110111110110..0")},
            { L_,  FW_01,     1,    DSTB, DBF+2,     8,  g("110111110110..0")},
            { L_,  FW_01,     1,    DSTB, DBF+3,     4,  g("110110111010..0")},
            { L_,  FW_01,     1,    DSTB, DBF+3,     5,  g("110110111010..0")},
            { L_,  FW_01,     1,    DSTB, DBF+3,     6,  g("110110111010..0")},
            { L_,  FW_01,     1,    DSTB, DBF+3,     7,  g("110110111010..0")},
            { L_,  FW_01,     1,    DSTB, DBF+3,     8,  g("111110111010..0")},

            { L_,  FW_01, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0, BPW-1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_10,     0,  "0..0",     0,   BPW,            g(FW_10) },
            { L_,  FW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     1, BPW-1,        g(FW_01) ^ 1 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0,   BPW,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     0, BPW-1,             INT_MAX },
            { L_,  FW_10,     0,   FW_01,     0,   BPW,                  ~0 },
            { L_,  FW_10,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1, BPW-1,                  ~1 },
            { L_,  FW_10,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,    DSTA,     2,     4,  g("1101101101100") },
            { L_,  FW_10,     0,    DSTA,     2,     5,  g("1101101101100") },
            { L_,  FW_10,     0,    DSTA,     2,     6,  g("1101111101100") },
            { L_,  FW_10,     0,    DSTA,     2,     7,  g("1101111101100") },
            { L_,  FW_10,     0,    DSTA,     2,     8,  g("1101111101100") },
            { L_,  FW_10,     0,    DSTA,     3,     4,  g("1101101110100") },
            { L_,  FW_10,     0,    DSTA,     3,     5,  g("1101101110100") },
            { L_,  FW_10,     0,    DSTA,     3,     6,  g("1101101110100") },
            { L_,  FW_10,     0,    DSTA,     3,     7,  g("1101101110100") },
            { L_,  FW_10,     0,    DSTA,     3,     8,  g("1111101110100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_,  FW_10,     0,    DSTB, DBF+2,     4,  g("110110110110..0")},
            { L_,  FW_10,     0,    DSTB, DBF+2,     5,  g("110110110110..0")},
            { L_,  FW_10,     0,    DSTB, DBF+2,     6,  g("110111110110..0")},
            { L_,  FW_10,     0,    DSTB, DBF+2,     7,  g("110111110110..0")},
            { L_,  FW_10,     0,    DSTB, DBF+2,     8,  g("110111110110..0")},
            { L_,  FW_10,     0,    DSTB, DBF+3,     4,  g("110110111010..0")},
            { L_,  FW_10,     0,    DSTB, DBF+3,     5,  g("110110111010..0")},
            { L_,  FW_10,     0,    DSTB, DBF+3,     6,  g("110110111010..0")},
            { L_,  FW_10,     0,    DSTB, DBF+3,     7,  g("110110111010..0")},
            { L_,  FW_10,     0,    DSTB, DBF+3,     8,  g("111110111010..0")},

            { L_,  FW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     0,     1,                   1 },
            { L_,  FW_10,     1,  "0..0",     0, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     1,                1<<1 },
            { L_,  FW_10,     1,  "0..0",     1, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_10,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     1,   g(FW_01) | (1<<1) },
            { L_,  FW_10,     1,   FW_01,     1, BPW-1,                  ~0 },
            { L_,  FW_10,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_,  FW_10,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_,  FW_10,     1,   FW_10,     0, BPW-1,                  ~0 },
            { L_,  FW_10,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     1,    DSTA,     2,     4,  g("1101101110100") },
            { L_,  FW_10,     1,    DSTA,     2,     5,  g("1101101110100") },
            { L_,  FW_10,     1,    DSTA,     2,     6,  g("1101101110100") },
            { L_,  FW_10,     1,    DSTA,     2,     7,  g("1101101110100") },
            { L_,  FW_10,     1,    DSTA,     2,     8,  g("1101101110100") },
            { L_,  FW_10,     1,    DSTA,     3,     4,  g("1101101101100") },
            { L_,  FW_10,     1,    DSTA,     3,     5,  g("1101111101100") },
            { L_,  FW_10,     1,    DSTA,     3,     6,  g("1101111101100") },
            { L_,  FW_10,     1,    DSTA,     3,     7,  g("1101111101100") },
            { L_,  FW_10,     1,    DSTA,     3,     8,  g("1101111101100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTB                                        "110110110010..0"

            { L_,  FW_10,     1,    DSTB, DBF+2,     4,  g("110110111010..0")},
            { L_,  FW_10,     1,    DSTB, DBF+2,     5,  g("110110111010..0")},
            { L_,  FW_10,     1,    DSTB, DBF+2,     6,  g("110110111010..0")},
            { L_,  FW_10,     1,    DSTB, DBF+2,     7,  g("110110111010..0")},
            { L_,  FW_10,     1,    DSTB, DBF+2,     8,  g("110110111010..0")},
            { L_,  FW_10,     1,    DSTB, DBF+3,     4,  g("110110110110..0")},
            { L_,  FW_10,     1,    DSTB, DBF+3,     5,  g("110111110110..0")},
            { L_,  FW_10,     1,    DSTB, DBF+3,     6,  g("110111110110..0")},
            { L_,  FW_10,     1,    DSTB, DBF+3,     7,  g("110111110110..0")},
            { L_,  FW_10,     1,    DSTB, DBF+3,     8,  g("110111110110..0")},

            { L_,  FW_10, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     0,     1,                   1 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     1,                   2 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     1,             INT_MIN },
            { L_,  FW_10, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     1,        g(FW_01) | 2 },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     1,  g(FW_01) ^ INT_MIN },
            { L_,  FW_10, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     0,     1,        g(FW_10) | 1 },
            { L_,  FW_10, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,   BPW,     0,            g(FW_10) }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            int src = g(DATA_A[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_orEqual); P(dst);
            }

            Util::orEqual(&dst, DATA_A[di].d_dindex, src,
                                   DATA_A[di].d_sindex, DATA_A[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_A[di].d_orEqual == dst);

            if (0 == DATA_A[di].d_sindex) {
                dst = g(spec);
                Util::orEqual(&dst, DATA_A[di].d_dindex, src,
                                                         DATA_A[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_A[di].d_orEqual == dst);
            }
         }

        if (verbose) cout << endl
                          << "Testing orEqual64 Function" << endl
                          << "==========================" << endl;

        const char *DSTC = "110110110010..0";  // typical case
        const int DCF = 51;     // DSTC floor -- offset of DSTA offsets to
                                // convert them to DSTC offsets
        static const struct {
            int         d_lineNum;   // line number
            const char *d_src;       // source integer
            int         d_sindex;    // source index
            const char *d_dst;       // destination integer
            int         d_dindex;    // destination index
            int         d_numBits;   // num of bits
            Int64       d_orEqual;   // expected result from orEqual function
        } DATA_B[] = {
            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------

            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,   BPS,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     0,    DSTA,     3,     8,g64("1101101100100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_, "0..0",     0,    DSTC, DCF+2,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+2,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+2,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+2,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+2,     8,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+3,     4,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+3,     5,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+3,     6,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+3,     7,g64("110110110010..0")},
            { L_, "0..0",     0,    DSTC, DCF+3,     8,g64("110110110010..0")},

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     2,     8,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     4,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     5,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     6,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     7,g64("1101101100100") },
            { L_, "0..0",     1,    DSTA,     3,     8,g64("1101101100100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_, "0..0",     1,    DSTC, DCF+2,     4,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+2,     5,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+2,     6,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+2,     7,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+2,     8,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+3,     4,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+3,     5,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+3,     6,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+3,     7,g64("110110110010..0")},
            { L_, "0..0",     1,    DSTC, DCF+3,     8,g64("110110110010..0")},

            { L_, "0..0", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     0,  "0..0",     0, BPS-1,            int64Max },
            { L_, "1..1",     0,  "0..0",     0,   BPS,                  ~0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                1<<1 },
            { L_, "1..1",     0,  "0..0",     1, BPS-1,                  ~1 },
            { L_, "1..1",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,   BPS,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0, BPS-1,            int64Max },
            { L_, "1..1",     0,   SW_01,     0,   BPS,                  ~0 },
            { L_, "1..1",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1,     1, g64(SW_01) | (1<<1) },
            { L_, "1..1",     0,   SW_01,     1, BPS-1,                  ~0 },
            { L_, "1..1",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_, "1..1",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_, "1..1",     0,   SW_10,     0, BPS-1,                  ~0 },
            { L_, "1..1",     0,   SW_10,     0,   BPS,                  ~0 },
            { L_, "1..1",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1, BPS-1,                  ~1 },
            { L_, "1..1",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,g64("1101101111100") },
            { L_, "1..1",     0,    DSTA,     2,     5,g64("1101101111100") },
            { L_, "1..1",     0,    DSTA,     2,     6,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     2,     7,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     2,     8,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     4,g64("1101101111100") },
            { L_, "1..1",     0,    DSTA,     3,     5,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     6,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     7,g64("1101111111100") },
            { L_, "1..1",     0,    DSTA,     3,     8,g64("1111111111100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_, "1..1",     0,    DSTC, DCF+2,     4,g64("110110111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+2,     5,g64("110110111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+2,     6,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+2,     7,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+2,     8,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+3,     4,g64("110110111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+3,     5,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+3,     6,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+3,     7,g64("110111111110..0")},
            { L_, "1..1",     0,    DSTC, DCF+3,     8,g64("111111111110..0")},

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   1 },
            { L_, "1..1",     1,  "0..0",     0, BPS-1,            int64Max },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                1<<1 },
            { L_, "1..1",     1,  "0..0",     1, BPS-1,                  ~1 },
            { L_, "1..1",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0, BPS-1,            int64Max },
            { L_, "1..1",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1,     1, g64(SW_01) | (1<<1) },
            { L_, "1..1",     1,   SW_01,     1, BPS-1,                  ~0 },
            { L_, "1..1",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_, "1..1",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_, "1..1",     1,   SW_10,     0, BPS-1,                  ~0 },
            { L_, "1..1",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1, BPS-1,                  ~1 },
            { L_, "1..1",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,g64("1101101111100") },
            { L_, "1..1",     1,    DSTA,     2,     5,g64("1101101111100") },
            { L_, "1..1",     1,    DSTA,     2,     6,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     2,     7,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     2,     8,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     4,g64("1101101111100") },
            { L_, "1..1",     1,    DSTA,     3,     5,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     6,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     7,g64("1101111111100") },
            { L_, "1..1",     1,    DSTA,     3,     8,g64("1111111111100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_, "1..1",     1,    DSTC, DCF+2,     4,g64("110110111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+2,     5,g64("110110111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+2,     6,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+2,     7,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+2,     8,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+3,     4,g64("110110111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+3,     5,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+3,     6,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+3,     7,g64("110111111110..0")},
            { L_, "1..1",     1,    DSTC, DCF+3,     8,g64("111111111110..0")},

            { L_, "1..1", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     0,     1,                   1 },
            { L_, "1..1", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     1,                   2 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     1,            int64Min },
            { L_, "1..1", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     1,       g64(SW_01) | 2},
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_, "1..1", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_, "1..1", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,     1,                   1 },
            { L_,  SW_01,     0,  "0..0",     0, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,  "0..0",     0,   BPS,          g64(SW_01) },
            { L_,  SW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     1,                1<<1 },
            { L_,  SW_01,     0,  "0..0",     1, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_01,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0,   BPS,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     1, g64(SW_01) | (1<<1) },
            { L_,  SW_01,     0,   SW_01,     1, BPS-1,                  ~0 },
            { L_,  SW_01,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_,  SW_01,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_,  SW_01,     0,   SW_10,     0, BPS-1,                  ~0 },
            { L_,  SW_01,     0,   SW_10,     0,   BPS,                  ~0 },
            { L_,  SW_01,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,    DSTA,     2,     4,g64("1101101110100") },
            { L_,  SW_01,     0,    DSTA,     2,     5,g64("1101101110100") },
            { L_,  SW_01,     0,    DSTA,     2,     6,g64("1101101110100") },
            { L_,  SW_01,     0,    DSTA,     2,     7,g64("1101101110100") },
            { L_,  SW_01,     0,    DSTA,     2,     8,g64("1101101110100") },
            { L_,  SW_01,     0,    DSTA,     3,     4,g64("1101101101100") },
            { L_,  SW_01,     0,    DSTA,     3,     5,g64("1101111101100") },
            { L_,  SW_01,     0,    DSTA,     3,     6,g64("1101111101100") },
            { L_,  SW_01,     0,    DSTA,     3,     7,g64("1101111101100") },
            { L_,  SW_01,     0,    DSTA,     3,     8,g64("1101111101100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_,  SW_01,     0,    DSTC, DCF+2,     4,g64("110110111010..0")},
            { L_,  SW_01,     0,    DSTC, DCF+2,     5,g64("110110111010..0")},
            { L_,  SW_01,     0,    DSTC, DCF+2,     6,g64("110110111010..0")},
            { L_,  SW_01,     0,    DSTC, DCF+2,     7,g64("110110111010..0")},
            { L_,  SW_01,     0,    DSTC, DCF+2,     8,g64("110110111010..0")},
            { L_,  SW_01,     0,    DSTC, DCF+3,     4,g64("110110110110..0")},
            { L_,  SW_01,     0,    DSTC, DCF+3,     5,g64("110111110110..0")},
            { L_,  SW_01,     0,    DSTC, DCF+3,     6,g64("110111110110..0")},
            { L_,  SW_01,     0,    DSTC, DCF+3,     7,g64("110111110110..0")},
            { L_,  SW_01,     0,    DSTC, DCF+3,     8,g64("110111110110..0")},

            { L_,  SW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     0, BPS-1, g64(SW_10)^int64Min },
            { L_,  SW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1, BPS-1,      g64(SW_01) ^ 1 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0, BPS-1,            int64Max },
            { L_,  SW_01,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1, BPS-1,                  ~1 },
            { L_,  SW_01,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     1,    DSTA,     2,     4,g64("1101101101100") },
            { L_,  SW_01,     1,    DSTA,     2,     5,g64("1101101101100") },
            { L_,  SW_01,     1,    DSTA,     2,     6,g64("1101111101100") },
            { L_,  SW_01,     1,    DSTA,     2,     7,g64("1101111101100") },
            { L_,  SW_01,     1,    DSTA,     2,     8,g64("1101111101100") },
            { L_,  SW_01,     1,    DSTA,     3,     4,g64("1101101110100") },
            { L_,  SW_01,     1,    DSTA,     3,     5,g64("1101101110100") },
            { L_,  SW_01,     1,    DSTA,     3,     6,g64("1101101110100") },
            { L_,  SW_01,     1,    DSTA,     3,     7,g64("1101101110100") },
            { L_,  SW_01,     1,    DSTA,     3,     8,g64("1111101110100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_,  SW_01,     1,    DSTC, DCF+2,     4,g64("110110110110..0")},
            { L_,  SW_01,     1,    DSTC, DCF+2,     5,g64("110110110110..0")},
            { L_,  SW_01,     1,    DSTC, DCF+2,     6,g64("110111110110..0")},
            { L_,  SW_01,     1,    DSTC, DCF+2,     7,g64("110111110110..0")},
            { L_,  SW_01,     1,    DSTC, DCF+2,     8,g64("110111110110..0")},
            { L_,  SW_01,     1,    DSTC, DCF+3,     4,g64("110110111010..0")},
            { L_,  SW_01,     1,    DSTC, DCF+3,     5,g64("110110111010..0")},
            { L_,  SW_01,     1,    DSTC, DCF+3,     6,g64("110110111010..0")},
            { L_,  SW_01,     1,    DSTC, DCF+3,     7,g64("110110111010..0")},
            { L_,  SW_01,     1,    DSTC, DCF+3,     8,g64("111110111010..0")},

            { L_,  SW_01, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0, BPS-1, g64(SW_10)^int64Min },
            { L_,  SW_10,     0,  "0..0",     0,   BPS,          g64(SW_10) },
            { L_,  SW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     1, BPS-1,      g64(SW_01) ^ 1 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0,   BPS,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     0, BPS-1,            int64Max },
            { L_,  SW_10,     0,   SW_01,     0,   BPS,                  ~0 },
            { L_,  SW_10,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1, BPS-1,                  ~1 },
            { L_,  SW_10,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,    DSTA,     2,     4,g64("1101101101100") },
            { L_,  SW_10,     0,    DSTA,     2,     5,g64("1101101101100") },
            { L_,  SW_10,     0,    DSTA,     2,     6,g64("1101111101100") },
            { L_,  SW_10,     0,    DSTA,     2,     7,g64("1101111101100") },
            { L_,  SW_10,     0,    DSTA,     2,     8,g64("1101111101100") },
            { L_,  SW_10,     0,    DSTA,     3,     4,g64("1101101110100") },
            { L_,  SW_10,     0,    DSTA,     3,     5,g64("1101101110100") },
            { L_,  SW_10,     0,    DSTA,     3,     6,g64("1101101110100") },
            { L_,  SW_10,     0,    DSTA,     3,     7,g64("1101101110100") },
            { L_,  SW_10,     0,    DSTA,     3,     8,g64("1111101110100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_,  SW_10,     0,    DSTC, DCF+2,     4,g64("110110110110..0")},
            { L_,  SW_10,     0,    DSTC, DCF+2,     5,g64("110110110110..0")},
            { L_,  SW_10,     0,    DSTC, DCF+2,     6,g64("110111110110..0")},
            { L_,  SW_10,     0,    DSTC, DCF+2,     7,g64("110111110110..0")},
            { L_,  SW_10,     0,    DSTC, DCF+2,     8,g64("110111110110..0")},
            { L_,  SW_10,     0,    DSTC, DCF+3,     4,g64("110110111010..0")},
            { L_,  SW_10,     0,    DSTC, DCF+3,     5,g64("110110111010..0")},
            { L_,  SW_10,     0,    DSTC, DCF+3,     6,g64("110110111010..0")},
            { L_,  SW_10,     0,    DSTC, DCF+3,     7,g64("110110111010..0")},
            { L_,  SW_10,     0,    DSTC, DCF+3,     8,g64("111110111010..0")},

            { L_,  SW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     0,     1,                   1 },
            { L_,  SW_10,     1,  "0..0",     0, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     1,                1<<1 },
            { L_,  SW_10,     1,  "0..0",     1, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_10,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     1, g64(SW_01) | (1<<1) },
            { L_,  SW_10,     1,   SW_01,     1, BPS-1,                  ~0 },
            { L_,  SW_10,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_,  SW_10,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_,  SW_10,     1,   SW_10,     0, BPS-1,                  ~0 },
            { L_,  SW_10,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     1,    DSTA,     2,     4,g64("1101101110100") },
            { L_,  SW_10,     1,    DSTA,     2,     5,g64("1101101110100") },
            { L_,  SW_10,     1,    DSTA,     2,     6,g64("1101101110100") },
            { L_,  SW_10,     1,    DSTA,     2,     7,g64("1101101110100") },
            { L_,  SW_10,     1,    DSTA,     2,     8,g64("1101101110100") },
            { L_,  SW_10,     1,    DSTA,     3,     4,g64("1101101101100") },
            { L_,  SW_10,     1,    DSTA,     3,     5,g64("1101111101100") },
            { L_,  SW_10,     1,    DSTA,     3,     6,g64("1101111101100") },
            { L_,  SW_10,     1,    DSTA,     3,     7,g64("1101111101100") },
            { L_,  SW_10,     1,    DSTA,     3,     8,g64("1101111101100") },

            //L#    src     sIdx   dst    dIdx   nBits        orEqual Result
            //--  ------    ----  ------  -----  -----  ---------------------
            // DSTC                                        "110110110010..0"

            { L_,  SW_10,     1,    DSTC, DCF+2,     4,g64("110110111010..0")},
            { L_,  SW_10,     1,    DSTC, DCF+2,     5,g64("110110111010..0")},
            { L_,  SW_10,     1,    DSTC, DCF+2,     6,g64("110110111010..0")},
            { L_,  SW_10,     1,    DSTC, DCF+2,     7,g64("110110111010..0")},
            { L_,  SW_10,     1,    DSTC, DCF+2,     8,g64("110110111010..0")},
            { L_,  SW_10,     1,    DSTC, DCF+3,     4,g64("110110110110..0")},
            { L_,  SW_10,     1,    DSTC, DCF+3,     5,g64("110111110110..0")},
            { L_,  SW_10,     1,    DSTC, DCF+3,     6,g64("110111110110..0")},
            { L_,  SW_10,     1,    DSTC, DCF+3,     7,g64("110111110110..0")},
            { L_,  SW_10,     1,    DSTB, DCF+3,     8,g64("110111110110..0")},

            { L_,  SW_10, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     0,     1,                   1 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     1,                   2 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     1,            int64Min },
            { L_,  SW_10, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     1,      g64(SW_01) | 2 },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     1, g64(SW_01)^int64Min },
            { L_,  SW_10, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     0,     1,      g64(SW_10) | 1 },
            { L_,  SW_10, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,   BPS,     0,          g64(SW_10) }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Int64 src = g64(DATA_B[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_orEqual); P(dst);
            }

            Util::orEqual64(&dst, DATA_B[di].d_dindex, src,
                                   DATA_B[di].d_sindex, DATA_B[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_B[di].d_orEqual == dst);

            if (0 == DATA_B[di].d_sindex) {
                dst = g64(spec);
                Util::orEqual64(&dst, DATA_B[di].d_dindex, src,
                                                         DATA_B[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_B[di].d_orEqual == dst);
            }
         }
}

static
void testF27()
{
        // --------------------------------------------------------------------
        // TESTING ANDEQUAL FUNCTION (5 ARGUMENTS):
        //   The five-argument 'andEqual' method has a simple implementation
        //   that perform calculations using already-tested methods.  A
        //   relatively small set of test data is sufficient, but we choose a
        //   more thorough and systematic set of test data to completely probe
        //   the algorithm and not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source and
        //   then source index and initial destination category, verify that
        //   the andEqual function sets the expected value.
        //
        // Testing:
        //   void andEqual(int *dInt, int dIdx, int sInt, int nBits);
        //   void andEqual64(bsls_Types::Int64 *dInt,
        //                   int dIdx,
        //                   bsls_Types::Int64 sInt,
        //                   int nBits);
        //   void andEqual(int *dInt, int dIdx, int sInt, int sIdx, int nBits);
        //   void andEqual64(bsls_Types::Int64 *dInt,
        //                   int dIdx,
        //                   bsls_Types::Int64 sInt,
        //                   int sIdx,
        //                   int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing andEqual Function" << endl
                          << "=========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_src;       // source integer
            int         d_sindex;    // source index
            const char *d_dst;       // destination integer
            int         d_dindex;    // destination index
            int         d_numBits;   // num of bits
            int         d_andEqual;  // expected result from andEqual function
        } DATA_A[] = {
            //L#     src   sIdx      dst   dIdx  nBits      andEqual Result
            //--  ------   ----   ------   ----  -----      ---------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0",     0,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "0..0",     0,  "1..1",     0,   BPW,                   0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0",     0,  "1..1",     1, BPW-1,                   1 },
            { L_, "0..0",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "0..0",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_, "0..0",     0,   FW_01,     0, BPW-1,                   0 },
            { L_, "0..0",     0,   FW_01,     0,   BPW,                   0 },
            { L_, "0..0",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,     1, BPW-1,                   1 },
            { L_, "0..0",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     0, BPW-1,             INT_MIN },
            { L_, "0..0",     0,   FW_10,     0,   BPW,                   0 },
            { L_, "0..0",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_, "0..0",     0,   FW_10,     1, BPW-1,                   0 },
            { L_, "0..0",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     0,   FW_10, BPW-1,     1,  g(FW_10) ^ INT_MIN },
            { L_, "0..0",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     0,    DSTA,     2,     4,  g("1101101000000") },
            { L_, "0..0",     0,    DSTA,     2,     5,  g("1101100000000") },
            { L_, "0..0",     0,    DSTA,     2,     6,  g("1101100000000") },
            { L_, "0..0",     0,    DSTA,     2,     7,  g("1101000000000") },
            { L_, "0..0",     0,    DSTA,     2,     8,  g("1100000000000") },
            { L_, "0..0",     0,    DSTA,     3,     4,  g("1101100000100") },
            { L_, "0..0",     0,    DSTA,     3,     5,  g("1101100000100") },
            { L_, "0..0",     0,    DSTA,     3,     6,  g("1101000000100") },
            { L_, "0..0",     0,    DSTA,     3,     7,  g("1100000000100") },
            { L_, "0..0",     0,    DSTA,     3,     8,  g("1100000000100") },

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0",     1,  "1..1",     0, BPW-1,             INT_MIN },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0",     1,  "1..1",     1, BPW-1,                   1 },
            { L_, "0..0",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "0..0",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_, "0..0",     1,   FW_01,     0, BPW-1,                   0 },
            { L_, "0..0",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,     1, BPW-1,                   1 },
            { L_, "0..0",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     0, BPW-1,             INT_MIN },
            { L_, "0..0",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_, "0..0",     1,   FW_10,     1, BPW-1,                   0 },
            { L_, "0..0",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",     1,   FW_10, BPW-1,     1,  g(FW_10) ^ INT_MIN },
            { L_, "0..0",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",     1,    DSTA,     2,     4,  g("1101101000000") },
            { L_, "0..0",     1,    DSTA,     2,     5,  g("1101100000000") },
            { L_, "0..0",     1,    DSTA,     2,     6,  g("1101100000000") },
            { L_, "0..0",     1,    DSTA,     2,     7,  g("1101000000000") },
            { L_, "0..0",     1,    DSTA,     2,     8,  g("1100000000000") },
            { L_, "0..0",     1,    DSTA,     3,     4,  g("1101100000100") },
            { L_, "0..0",     1,    DSTA,     3,     5,  g("1101100000100") },
            { L_, "0..0",     1,    DSTA,     3,     6,  g("1101000000100") },
            { L_, "0..0",     1,    DSTA,     3,     7,  g("1100000000100") },
            { L_, "0..0",     1,    DSTA,     3,     8,  g("1100000000100") },

            { L_, "0..0", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "0..0", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0", BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_, "0..0", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_, "0..0", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "0..0", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0", BPW-1,   FW_10, BPW-1,     1,  g(FW_10) ^ INT_MIN },
            { L_, "0..0", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "0..0",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "0..0",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "0..0",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "0..0",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "0..0",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "0..0",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "0..0",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "0..0",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     0, BPW-1,                   0 },
            { L_, "1..1",     0,  "0..0",     0,   BPW,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     1, BPW-1,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,   BPW,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     0,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1",     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     0,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1",     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     0,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "1..1",     0,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     0, BPW-1,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     1, BPW-1,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0, BPW-1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1, BPW-1,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",     1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1",     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",     1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1",     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",     1,    DSTA,     2,     4,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     2,     5,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     2,     6,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     2,     7,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     2,     8,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     3,     4,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     3,     5,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     3,     6,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     3,     7,  g("1101101100100") },
            { L_, "1..1",     1,    DSTA,     3,     8,  g("1101101100100") },

            { L_, "1..1", BPW-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     0,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0",     1,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1", BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_, "1..1", BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1", BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_, "1..1", BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1", BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_, "1..1", BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1", BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_, "1..1", BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_, "1..1",   BPW,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_, "1..1",   BPW,  "0..0",   BPW,     0,                   0 },

            { L_, "1..1",   BPW,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_, "1..1",   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_, "1..1",   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_, "1..1",   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_, "1..1",   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_, "1..1",   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_01,     0,  "0..0",     0,   BPW,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     0,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     0, BPW-1,  g(FW_01) ^ INT_MIN },
            { L_,  FW_01,     0,  "1..1",     0,   BPW,            g(FW_01) },
            { L_,  FW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",     1, BPW-1,        g(FW_10) ^ 1 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     0,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_01,     0,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     0,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     0,   BPW,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_01,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_01,     0,   FW_10,     0,   BPW,                   0 },
            { L_,  FW_01,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_01,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     0,    DSTA,     2,     4,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     2,     5,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     2,     6,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     2,     7,  g("1101101000100") },
            { L_,  FW_01,     0,    DSTA,     2,     8,  g("1100101000100") },
            { L_,  FW_01,     0,    DSTA,     3,     4,  g("1101100100100") },
            { L_,  FW_01,     0,    DSTA,     3,     5,  g("1101100100100") },
            { L_,  FW_01,     0,    DSTA,     3,     6,  g("1101000100100") },
            { L_,  FW_01,     0,    DSTA,     3,     7,  g("1101000100100") },
            { L_,  FW_01,     0,    DSTA,     3,     8,  g("1101000100100") },

            { L_,  FW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_01,     1,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1",     1,     1,             ~(1<<1) },
            { L_,  FW_01,     1,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,     1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_01,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_,  FW_01,     1,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_01,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_,  FW_01,     1,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_01,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,     1,   FW_10, BPW-1,     1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_01,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,     1,    DSTA,     2,     4,  g("1101101100000") },
            { L_,  FW_01,     1,    DSTA,     2,     5,  g("1101100100000") },
            { L_,  FW_01,     1,    DSTA,     2,     6,  g("1101100100000") },
            { L_,  FW_01,     1,    DSTA,     2,     7,  g("1101000100000") },
            { L_,  FW_01,     1,    DSTA,     2,     8,  g("1101000100000") },
            { L_,  FW_01,     1,    DSTA,     3,     4,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     3,     5,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     3,     6,  g("1101101000100") },
            { L_,  FW_01,     1,    DSTA,     3,     7,  g("1100101000100") },
            { L_,  FW_01,     1,    DSTA,     3,     8,  g("1100101000100") },

            { L_,  FW_01, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_01, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1",     1,     1,                  ~2 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01, BPW-1,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_01, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_,  FW_01, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_01, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01, BPW-1,   FW_10, BPW-1,     1,   g(FW_10)^INT_MIN },
            { L_,  FW_01, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_01,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_01,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_01,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_01,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_01,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_01,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_01,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_01,   BPW,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_10,     0,  "0..0",     0,   BPW,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     0,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     0,  "0..0",   BPW,     0,                   0 },
            { L_,  FW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  FW_10,     0,  "1..1",     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     0,  "1..1",     0,   BPW,            g(FW_10) },
            { L_,  FW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1",     1,     1,            ~(1<<1) },
            { L_,  FW_10,     0,  "1..1",     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     0,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     0,  "1..1", BPW-1,     1,             INT_MAX },
            { L_,  FW_10,     0,  "1..1",   BPW,     0,                  ~0 },
            { L_,  FW_10,     0,   FW_01,     0,     0,            g(FW_01) },

            { L_,  FW_10,     0,   FW_01,     0,     1,        g(FW_01) ^ 1 },
            { L_,  FW_10,     0,   FW_01,     0, BPW-1,                   0 },
            { L_,  FW_10,     0,   FW_01,     0,   BPW,                   0 },
            { L_,  FW_10,     0,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,     1, BPW-1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     0,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     0,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0, BPW-1,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     0,   BPW,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10,     1,     1,   g(FW_10) ^ (1<<1) },
            { L_,  FW_10,     0,   FW_10,     1, BPW-1,                   0 },
            { L_,  FW_10,     0,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     0,   FW_10, BPW-1,     1,  g(FW_10) ^ INT_MIN },
            { L_,  FW_10,     0,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     0,   DSTA,     2,     4,   g("1101101100000") },
            { L_,  FW_10,     0,   DSTA,     2,     5,   g("1101100100000") },
            { L_,  FW_10,     0,   DSTA,     2,     6,   g("1101100100000") },
            { L_,  FW_10,     0,   DSTA,     2,     7,   g("1101000100000") },
            { L_,  FW_10,     0,   DSTA,     2,     8,   g("1101000100000") },
            { L_,  FW_10,     0,   DSTA,     3,     4,   g("1101101000100") },
            { L_,  FW_10,     0,   DSTA,     3,     5,   g("1101101000100") },
            { L_,  FW_10,     0,   DSTA,     3,     6,   g("1101101000100") },
            { L_,  FW_10,     0,   DSTA,     3,     7,   g("1100101000100") },
            { L_,  FW_10,     0,   DSTA,     3,     8,   g("1100101000100") },

            { L_,  FW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     0,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",     0, BPW-1,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0",     1,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",     1, BPW-1,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,     1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10,     1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     0, BPW-1,  g(FW_01) ^ INT_MIN },
            { L_,  FW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",     1, BPW-1,        g(FW_10) ^ 1 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,     1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10,     1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,     1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     0, BPW-1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,     1, BPW-1,                   1 },
            { L_,  FW_10,     1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10,     1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,     1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     0, BPW-1,             INT_MIN },
            { L_,  FW_10,     1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,     1, BPW-1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10,     1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,     1,    DSTA,     2,     4,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     2,     5,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     2,     6,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     2,     7,  g("1101101000100") },
            { L_,  FW_10,     1,    DSTA,     2,     8,  g("1100101000100") },
            { L_,  FW_10,     1,    DSTA,     3,     4,  g("1101100100100") },
            { L_,  FW_10,     1,    DSTA,     3,     5,  g("1101100100100") },
            { L_,  FW_10,     1,    DSTA,     3,     6,  g("1101000100100") },
            { L_,  FW_10,     1,    DSTA,     3,     7,  g("1101000100100") },
            { L_,  FW_10,     1,    DSTA,     3,     8,  g("1101000100100") },

            { L_,  FW_10, BPW-1,  "0..0",     0,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     0,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",     1,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10, BPW-1,  "0..0", BPW-1,     1,                   0 },
            { L_,  FW_10, BPW-1,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10, BPW-1,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     0,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",     1,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1", BPW-1,     1,                  ~0 },
            { L_,  FW_10, BPW-1,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10, BPW-1,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     0,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,     1,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01, BPW-1,     1,            g(FW_01) },
            { L_,  FW_10, BPW-1,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10, BPW-1,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     0,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,     1,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10, BPW-1,     1,            g(FW_10) },
            { L_,  FW_10, BPW-1,   FW_10,   BPW,     0,            g(FW_10) },

            { L_,  FW_10,   BPW,  "0..0",     0,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",     1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0", BPW-1,     0,                   0 },
            { L_,  FW_10,   BPW,  "0..0",   BPW,     0,                   0 },

            { L_,  FW_10,   BPW,  "1..1",     0,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",     1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1", BPW-1,     0,                  ~0 },
            { L_,  FW_10,   BPW,  "1..1",   BPW,     0,                  ~0 },

            { L_,  FW_10,   BPW,   FW_01,     0,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,     1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01, BPW-1,     0,            g(FW_01) },
            { L_,  FW_10,   BPW,   FW_01,   BPW,     0,            g(FW_01) },

            { L_,  FW_10,   BPW,   FW_10,     0,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,     1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10, BPW-1,     0,            g(FW_10) },
            { L_,  FW_10,   BPW,   FW_10,   BPW,     0,            g(FW_10) }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            int src = g(DATA_A[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_andEqual); P(dst);
            }

            Util::andEqual(&dst, DATA_A[di].d_dindex, src,
                                   DATA_A[di].d_sindex, DATA_A[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_A[di].d_andEqual == dst);

            if (0 == DATA_A[di].d_sindex) {
                int dst = g(spec);
                Util::andEqual(&dst, DATA_A[di].d_dindex, src,
                                                         DATA_A[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_A[di].d_andEqual == dst);
            }
         }

        if (verbose) cout << "Testing andEqual64 Function" << endl
                          << "===========================" << endl;

        const char *DSTB = "1101101100100";    // typical case
        const char *DSTC = "1101101100..0";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_src;       // source integer
            int         d_sindex;    // source index
            const char *d_dst;       // destination integer
            int         d_dindex;    // destination index
            int         d_numBits;   // num of bits
            Int64       d_andEqual;  // expected result from andEqual function
        } DATA_B[] = {
            //L#     src   sIdx      dst   dIdx  nBits      andEqual Result
            //--  ------   ----   ------   ----  -----      ---------------
            { L_, "0..0",     0,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     0,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0",     0,  "1..1",     0, BPS-1,            int64Min },
            { L_, "0..0",     0,  "1..1",     0,   BPS,                   0 },
            { L_, "0..0",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0",     0,  "1..1",     1, BPS-1,                   1 },
            { L_, "0..0",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     0,  "1..1", BPS-1,     1,            int64Max },
            { L_, "0..0",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_, "0..0",     0,   SW_01,     0, BPS-1,                   0 },
            { L_, "0..0",     0,   SW_01,     0,   BPS,                   0 },
            { L_, "0..0",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,     1, BPS-1,                   1 },
            { L_, "0..0",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     0, BPS-1,            int64Min },
            { L_, "0..0",     0,   SW_10,     0,   BPS,                   0 },
            { L_, "0..0",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_, "0..0",     0,   SW_10,     1, BPS-1,                   0 },
            { L_, "0..0",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "0..0",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     0,    DSTB,     2,     4,g64("1101101000000") },
            { L_, "0..0",     0,    DSTB,     2,     5,g64("1101100000000") },
            { L_, "0..0",     0,    DSTB,     2,     6,g64("1101100000000") },
            { L_, "0..0",     0,    DSTB,     2,     7,g64("1101000000000") },
            { L_, "0..0",     0,    DSTB,     2,     8,g64("1100000000000") },
            { L_, "0..0",     0,    DSTB,     3,     4,g64("1101100000100") },
            { L_, "0..0",     0,    DSTB,     3,     5,g64("1101100000100") },
            { L_, "0..0",     0,    DSTB,     3,     6,g64("1101000000100") },
            { L_, "0..0",     0,    DSTB,     3,     7,g64("1100000000100") },
            { L_, "0..0",     0,    DSTB,     3,     8,g64("1100000000100") },

            { L_, "0..0",     0,    DSTC,     0,     1,g64("1101101100..0") },
            { L_, "0..0",     3,    DSTC,    62,     2,g64("0001101100..0") },
            { L_, "0..0",     3,    DSTC,    59,     2,g64("1100001100..0") },
            { L_, "0..0",     0,    DSTC,    59,     2,g64("1100001100..0") },
            { L_, "0..0",     3,    DSTC,    56,     2,g64("1101100000..0") },
            { L_, "0..0",     0,    DSTC,    56,     1,g64("1101101000..0") },
            { L_, "0..0",     0,    DSTC,    58,     1,g64("1101101100..0") },

            { L_, "0..0",     1,  "0..0",     0,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     0,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     0,                   0 },
            { L_, "0..0",     1,  "0..0",     1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0",     1,  "1..1",     0, BPS-1,            int64Min },
            { L_, "0..0",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0",     1,  "1..1",     1, BPS-1,                   1 },
            { L_, "0..0",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",     1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "0..0",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_, "0..0",     1,   SW_01,     0, BPS-1,                   0 },
            { L_, "0..0",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,     1, BPS-1,                   1 },
            { L_, "0..0",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     0, BPS-1,            int64Min },
            { L_, "0..0",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_, "0..0",     1,   SW_10,     1, BPS-1,                   0 },
            { L_, "0..0",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "0..0",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",     1,    DSTB,     2,     4,g64("1101101000000") },
            { L_, "0..0",     1,    DSTB,     2,     5,g64("1101100000000") },
            { L_, "0..0",     1,    DSTB,     2,     6,g64("1101100000000") },
            { L_, "0..0",     1,    DSTB,     2,     7,g64("1101000000000") },
            { L_, "0..0",     1,    DSTB,     2,     8,g64("1100000000000") },
            { L_, "0..0",     1,    DSTB,     3,     4,g64("1101100000100") },
            { L_, "0..0",     1,    DSTB,     3,     5,g64("1101100000100") },
            { L_, "0..0",     1,    DSTB,     3,     6,g64("1101000000100") },
            { L_, "0..0",     1,    DSTB,     3,     7,g64("1100000000100") },
            { L_, "0..0",     1,    DSTB,     3,     8,g64("1100000000100") },

            { L_, "0..0", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "0..0", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_, "0..0", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1",     1,     1,             ~(1<<1) },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0", BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_, "0..0", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_, "0..0", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "0..0", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0", BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_, "0..0", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "0..0",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "0..0",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "0..0",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "0..0",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "0..0",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "0..0",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "0..0",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "0..0",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     0, BPS-1,                   0 },
            { L_, "1..1",     0,  "0..0",     0,   BPS,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     0,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",     1, BPS-1,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     0,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1",     0,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     0,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     0,   BPS,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1",     0,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1",     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1",     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     0,    DSTB,     2,     4,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     2,     5,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     2,     6,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     2,     7,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     2,     8,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     3,     4,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     3,     5,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     3,     6,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     3,     7,g64("1101101100100") },
            { L_, "1..1",     0,    DSTB,     3,     8,g64("1101101100100") },

            { L_, "1..1",     1,  "0..0",     0,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     0,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     0, BPS-1,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     0,                   0 },
            { L_, "1..1",     1,  "0..0",     1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",     1, BPS-1,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",     1,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1",     1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",     1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     0, BPS-1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",     1, BPS-1,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1",     1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1",     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1",     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",     1,    DSTB,     2,     4,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     2,     5,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     2,     6,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     2,     7,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     2,     8,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     3,     4,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     3,     5,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     3,     6,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     3,     7,g64("1101101100100") },
            { L_, "1..1",     1,    DSTB,     3,     8,g64("1101101100100") },

            { L_, "1..1", BPS-1,  "0..0",     0,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     0,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0",     1,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1", BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_, "1..1", BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1", BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_, "1..1", BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1", BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_, "1..1", BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1", BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_, "1..1", BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_, "1..1",   BPS,  "0..0",     0,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",     1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_, "1..1",   BPS,  "0..0",   BPS,     0,                   0 },

            { L_, "1..1",   BPS,  "1..1",     0,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",     1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_, "1..1",   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_, "1..1",   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_, "1..1",   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_, "1..1",   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_, "1..1",   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_01,     0,  "0..0",     0,   BPS,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     0,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     0, BPS-1, g64(SW_01)^int64Min },
            { L_,  SW_01,     0,  "1..1",     0,   BPS,          g64(SW_01) },
            { L_,  SW_01,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",     1, BPS-1,      g64(SW_10) ^ 1 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     0,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_01,     0,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     0,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     0,   BPS,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_01,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_01,     0,   SW_10,     0,   BPS,                   0 },
            { L_,  SW_01,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_01,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     0,    DSTB,     2,     4,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTB,     2,     5,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTB,     2,     6,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTB,     2,     7,g64("1101101000100") },
            { L_,  SW_01,     0,    DSTB,     2,     8,g64("1100101000100") },
            { L_,  SW_01,     0,    DSTB,     3,     4,g64("1101100100100") },
            { L_,  SW_01,     0,    DSTB,     3,     5,g64("1101100100100") },
            { L_,  SW_01,     0,    DSTB,     3,     6,g64("1101000100100") },
            { L_,  SW_01,     0,    DSTB,     3,     7,g64("1101000100100") },
            { L_,  SW_01,     0,    DSTB,     3,     8,g64("1101000100100") },

            { L_,  SW_01,     0,    DSTC,     0,     1,g64("1101101100..0") },
            { L_,  SW_01,     0,    DSTC,    62,     2,g64("0101101100..0") },
            { L_,  SW_01,    54,    DSTC,    62,     2,g64("0101101100..0") },
            { L_,  SW_01,     0,    DSTC,    59,     2,g64("1100101100..0") },
            { L_,  SW_01,    48,    DSTC,    59,     2,g64("1100101100..0") },
            { L_,  SW_01,     0,    DSTC,    56,     5,g64("1101000100..0") },
            { L_,  SW_01,    50,    DSTC,    56,     5,g64("1101000100..0") },
            { L_,  SW_01,     0,    DSTC,    56,     8,g64("0101000100..0") },
            { L_,  SW_01,    54,    DSTC,    56,     8,g64("0101000100..0") },
            { L_,  SW_01,     0,    DSTC,     0,    64,g64("0101000100..0") },

            { L_,  SW_10,     0,    DSTC,     0,     1,g64("1101101100..0") },
            { L_,  SW_10,     0,    DSTC,    62,     2,g64("1001101100..0") },
            { L_,  SW_10,    44,    DSTC,    62,     2,g64("1001101100..0") },
            { L_,  SW_10,     0,    DSTC,    59,     3,g64("1101001100..0") },
            { L_,  SW_10,    50,    DSTC,    59,     3,g64("1101001100..0") },
            { L_,  SW_10,     1,    DSTC,    59,     3,g64("1100101100..0") },
            { L_,  SW_10,    51,    DSTC,    59,     3,g64("1100101100..0") },
            { L_,  SW_10,     0,    DSTC,    56,     8,g64("1000101000..0") },
            { L_,  SW_10,    52,    DSTC,    56,     8,g64("1000101000..0") },
            { L_,  SW_10,     0,    DSTC,     0,    64,g64("1000101000..0") },

            { L_,  SW_01,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_01,     1,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1",     1,     1,             ~(1<<1) },
            { L_,  SW_01,     1,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,     1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_01,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_,  SW_01,     1,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_01,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_,  SW_01,     1,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_01,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,     1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_01,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,     1,    DSTB,     2,     4,g64("1101101100000") },
            { L_,  SW_01,     1,    DSTB,     2,     5,g64("1101100100000") },
            { L_,  SW_01,     1,    DSTB,     2,     6,g64("1101100100000") },
            { L_,  SW_01,     1,    DSTB,     2,     7,g64("1101000100000") },
            { L_,  SW_01,     1,    DSTB,     2,     8,g64("1101000100000") },
            { L_,  SW_01,     1,    DSTB,     3,     4,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTB,     3,     5,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTB,     3,     6,g64("1101101000100") },
            { L_,  SW_01,     1,    DSTB,     3,     7,g64("1100101000100") },
            { L_,  SW_01,     1,    DSTB,     3,     8,g64("1100101000100") },

            //                      DSTC =                 "1101101100..0"
            { L_,  SW_01,     1,    DSTC,     3,     8,g64("1101101100..0") },
            { L_,  SW_01,     1,    DSTC,     3,    61,g64("0101000100..0") },
            { L_,  SW_01,     1,    DSTC,     2,    62,g64("1000101000..0") },
            { L_,  SW_01,     1,    DSTC,     2,    60,g64("1100101000..0") },
            { L_,  SW_01,     1,    DSTC,     2,    57,g64("1101101000..0") },

            { L_,  SW_01, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_01, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1",     1,     1,                  ~2 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01, BPS-1,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_01, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_,  SW_01, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_01, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01, BPS-1,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_01, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_01,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_01,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_01,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_01,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_01,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_01,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_01,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_01,   BPS,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_10,     0,  "0..0",     0,   BPS,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     0,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     0,  "0..0",   BPS,     0,                   0 },
            { L_,  SW_10,     0,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     0,     1,                  ~1 },
            { L_,  SW_10,     0,  "1..1",     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     0,  "1..1",     0,   BPS,          g64(SW_10) },
            { L_,  SW_10,     0,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1",     1,     1,            ~(1<<1) },
            { L_,  SW_10,     0,  "1..1",     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     0,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     0,  "1..1", BPS-1,     1,            int64Max },
            { L_,  SW_10,     0,  "1..1",   BPS,     0,                  ~0 },
            { L_,  SW_10,     0,   SW_01,     0,     0,          g64(SW_01) },

            { L_,  SW_10,     0,   SW_01,     0,     1,      g64(SW_01) ^ 1 },
            { L_,  SW_10,     0,   SW_01,     0, BPS-1,                   0 },
            { L_,  SW_10,     0,   SW_01,     0,   BPS,                   0 },
            { L_,  SW_10,     0,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,     1, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     0,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     0,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     0,   BPS,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10,     1,     1, g64(SW_10) ^ (1<<1) },
            { L_,  SW_10,     0,   SW_10,     1, BPS-1,                   0 },
            { L_,  SW_10,     0,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     0,   SW_10, BPS-1,     1, g64(SW_10)^int64Min },
            { L_,  SW_10,     0,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     0,   DSTB,     2,     4, g64("1101101100000") },
            { L_,  SW_10,     0,   DSTB,     2,     5, g64("1101100100000") },
            { L_,  SW_10,     0,   DSTB,     2,     6, g64("1101100100000") },
            { L_,  SW_10,     0,   DSTB,     2,     7, g64("1101000100000") },
            { L_,  SW_10,     0,   DSTB,     2,     8, g64("1101000100000") },
            { L_,  SW_10,     0,   DSTB,     3,     4, g64("1101101000100") },
            { L_,  SW_10,     0,   DSTB,     3,     5, g64("1101101000100") },
            { L_,  SW_10,     0,   DSTB,     3,     6, g64("1101101000100") },
            { L_,  SW_10,     0,   DSTB,     3,     7, g64("1100101000100") },
            { L_,  SW_10,     0,   DSTB,     3,     8, g64("1100101000100") },

            { L_,  SW_10,     1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     0,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",     0, BPS-1,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0",     1,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",     1, BPS-1,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,     1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10,     1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,     1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     0, BPS-1, g64(SW_01)^int64Min },
            { L_,  SW_10,     1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",     1, BPS-1,      g64(SW_10) ^ 1 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,     1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10,     1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,     1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     0, BPS-1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,     1, BPS-1,                   1 },
            { L_,  SW_10,     1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10,     1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,     1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     0, BPS-1,            int64Min },
            { L_,  SW_10,     1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,     1, BPS-1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10,     1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,     1,    DSTB,     2,     4,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTB,     2,     5,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTB,     2,     6,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTB,     2,     7,g64("1101101000100") },
            { L_,  SW_10,     1,    DSTB,     2,     8,g64("1100101000100") },
            { L_,  SW_10,     1,    DSTB,     3,     4,g64("1101100100100") },
            { L_,  SW_10,     1,    DSTB,     3,     5,g64("1101100100100") },
            { L_,  SW_10,     1,    DSTB,     3,     6,g64("1101000100100") },
            { L_,  SW_10,     1,    DSTB,     3,     7,g64("1101000100100") },
            { L_,  SW_10,     1,    DSTB,     3,     8,g64("1101000100100") },

            { L_,  SW_10, BPS-1,  "0..0",     0,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     0,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",     1,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10, BPS-1,  "0..0", BPS-1,     1,                   0 },
            { L_,  SW_10, BPS-1,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10, BPS-1,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     0,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",     1,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1", BPS-1,     1,                  ~0 },
            { L_,  SW_10, BPS-1,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10, BPS-1,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     0,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,     1,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01, BPS-1,     1,          g64(SW_01) },
            { L_,  SW_10, BPS-1,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10, BPS-1,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     0,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,     1,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10, BPS-1,     1,          g64(SW_10) },
            { L_,  SW_10, BPS-1,   SW_10,   BPS,     0,          g64(SW_10) },

            { L_,  SW_10,   BPS,  "0..0",     0,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",     1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0", BPS-1,     0,                   0 },
            { L_,  SW_10,   BPS,  "0..0",   BPS,     0,                   0 },

            { L_,  SW_10,   BPS,  "1..1",     0,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",     1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1", BPS-1,     0,                  ~0 },
            { L_,  SW_10,   BPS,  "1..1",   BPS,     0,                  ~0 },

            { L_,  SW_10,   BPS,   SW_01,     0,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,     1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01, BPS-1,     0,          g64(SW_01) },
            { L_,  SW_10,   BPS,   SW_01,   BPS,     0,          g64(SW_01) },

            { L_,  SW_10,   BPS,   SW_10,     0,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,     1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10, BPS-1,     0,          g64(SW_10) },
            { L_,  SW_10,   BPS,   SW_10,   BPS,     0,          g64(SW_10) }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Int64 src = g64(DATA_B[di].d_src);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_andEqual); P(dst);
            }

            Util::andEqual64(&dst, DATA_B[di].d_dindex, src,
                                   DATA_B[di].d_sindex, DATA_B[di].d_numBits);
            LOOP_ASSERT(LINE, DATA_B[di].d_andEqual == dst);

            if (0 == DATA_B[di].d_sindex) {
                dst = g64(spec);
                Util::andEqual64(&dst, DATA_B[di].d_dindex, src,
                                                         DATA_B[di].d_numBits);
                LOOP_ASSERT(LINE, DATA_B[di].d_andEqual == dst);
            }
         }
}

static
void testF26()
{
        // --------------------------------------------------------------------
        // TESTING 'extractZero' AND 'extractOne' FUNCTIONS (4 ARGUMENTS):
        //   The four-argument 'extract0' and 'extract1' methods have
        //   relatively simple implementations that perform calculations using
        //   already-tested methods.  A relatively small set of test data is
        //   sufficient, but we choose a more thorough and systematic set of
        //   test data to completely probe the algorithm and not just the
        //   implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by offset
        //   and then source category, verify that the extract0 and extract1
        //   functions each return the expected value.
        //
        // Testing:
        //   int extractZero(  int sInt, int sIdx, int nBits, int offset);
        //   int extractZero64(int sInt, int sIdx, int nBits, int offset);
        //   int extractOne(   int sInt, int sIdx, int nBits, int offset);
        //   int extractOne64( int sInt, int sIdx, int nBits, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing extractZero and extractOne Functions" << endl
            << "============================================" << endl;

        const char *SRCA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            int         d_offset;    // source integer offset
            const char *d_src;       // source integer
            int         d_index;     // source index
            int         d_numBits;   // number of bits
            int         d_extract0;  // expected result from extract0 function
            int         d_extract1;  // expected result from extract1 function
        } DATA_A[] = {
    //v----^
    //L# offset     src   Index NumBits extract0 result     extract1 result
    //-- ------  ------ ------- ------- ---------------     ---------------
    { L_,     0, "0..0",     0,     0,                0,                 ~0 },
    { L_,     0, "0..0",     0,     1,                0,                 ~1 },
    { L_,     0, "0..0",     0, BPW-1,                0,            INT_MIN },
    { L_,     0, "0..0",     0,   BPW,                0,                  0 },
    { L_,     0, "0..0",     1,     0,                0,                 ~0 },
    { L_,     0, "0..0",     1,     1,                0,                 ~1 },
    { L_,     0, "0..0",     1, BPW-1,                0,            INT_MIN },
    { L_,     0, "0..0", BPW-1,     0,                0,                 ~0 },
    { L_,     0, "0..0", BPW-1,     1,                0,                 ~1 },
    { L_,     0, "0..0",   BPW,     0,                0,                 ~0 },

    { L_,     0, "1..1",     0,     0,                0,                 ~0 },
    { L_,     0, "1..1",     0,     1,                1,                 ~0 },
    { L_,     0, "1..1",     0, BPW-1,          INT_MAX,                 ~0 },
    { L_,     0, "1..1",     0,   BPW,               ~0,                 ~0 },
    { L_,     0, "1..1",     1,     0,                0,                 ~0 },
    { L_,     0, "1..1",     1,     1,                1,                 ~0 },
    { L_,     0, "1..1",     1, BPW-1,          INT_MAX,                 ~0 },
    { L_,     0, "1..1", BPW-1,     0,                0,                 ~0 },
    { L_,     0, "1..1", BPW-1,     1,                1,                 ~0 },
    { L_,     0, "1..1",   BPW,     0,                0,                 ~0 },

    { L_,     0,  FW_01,     0,     0,                0,                 ~0 },
    { L_,     0,  FW_01,     0,     1,                1,                 ~0 },
    { L_,     0,  FW_01,     0, BPW-1,         g(FW_01),   g(FW_01)|INT_MIN },
    { L_,     0,  FW_01,     0,   BPW,         g(FW_01),           g(FW_01) },
    { L_,     0,  FW_01,     1,     0,                0,                 ~0 },
    { L_,     0,  FW_01,     1,     1,                0,                 ~1 },
    { L_,     0,  FW_01,     1, BPW-1, g(FW_10)^INT_MIN,           g(FW_10) },
    { L_,     0,  FW_01, BPW-1,     0,                0,                 ~0 },
    { L_,     0,  FW_01, BPW-1,     1,                0,                 ~1 },
    { L_,     0,  FW_01,   BPW,     0,                0,                 ~0 },

    { L_,     0,  FW_10,     0,     0,                0,                 ~0 },
    { L_,     0,  FW_10,     0,     1,                0,                 ~1 },
    { L_,     0,  FW_10,     0, BPW-1, g(FW_10)^INT_MIN,           g(FW_10) },
    { L_,     0,  FW_10,     0,   BPW,         g(FW_10),           g(FW_10) },
    { L_,     0,  FW_10,     1,     0,                0,                 ~0 },
    { L_,     0,  FW_10,     1,     1,                1,                 ~0 },
    { L_,     0,  FW_10,     1, BPW-1,         g(FW_01),   g(FW_01)|INT_MIN },
    { L_,     0,  FW_10, BPW-1,     0,                0,                 ~0 },
    { L_,     0,  FW_10, BPW-1,     1,                1,                 ~0 },
    { L_,     0,  FW_10,   BPW,     0,                0,                 ~0 },

    //             SRCA      =          "1101101100100"
    { L_,     0,   SRCA,     2,     4,    g("00001001"),  g("1..111111001") },
    { L_,     0,   SRCA,     2,     5,    g("00011001"),  g("1..111111001") },
    { L_,     0,   SRCA,     2,     6,    g("00011001"),  g("1..111011001") },
    { L_,     0,   SRCA,     2,     7,    g("01011001"),  g("1..111011001") },
    { L_,     0,   SRCA,     2,     8,    g("11011001"),  g("1..111011001") },
    { L_,     0,   SRCA,     3,     4,    g("00001100"),  g("1..111111100") },
    { L_,     0,   SRCA,     3,     5,    g("00001100"),  g("1..111101100") },
    { L_,     0,   SRCA,     3,     6,    g("00101100"),  g("1..111101100") },
    { L_,     0,   SRCA,     3,     7,    g("01101100"),  g("1..111101100") },
    { L_,     0,   SRCA,     3,     8,    g("01101100"),  g("1..101101100") },

    { L_,     1, "0..0",     0,     0,                0,                 ~0 },
    { L_,     1, "0..0",     0,     1,                0,                 -3 },
    { L_,     1, "0..0",     0, BPW-1,                0,                  1 },
    { L_,     1, "0..0",     0,   BPW,                0,                  1 },
    { L_,     1, "0..0",     1,     0,                0,                 ~0 },
    { L_,     1, "0..0",     1,     1,                0,                 -3 },
    { L_,     1, "0..0",     1, BPW-1,                0,                  1 },
    { L_,     1, "0..0", BPW-1,     0,                0,                 ~0 },
    { L_,     1, "0..0", BPW-1,     1,                0,                 ~2 },
    { L_,     1, "0..0",   BPW,     0,                0,                 ~0 },

    { L_,     1, "1..1",     0,     0,                0,                 ~0 },
    { L_,     1, "1..1",     0,     1,                2,                 ~0 },
    { L_,     1, "1..1",     0, BPW-1,               ~1,                 ~0 },
    { L_,     1, "1..1",     0,   BPW,               ~1,                 ~0 },
    { L_,     1, "1..1",     1,     0,                0,                 ~0 },
    { L_,     1, "1..1",     1,     1,                2,                 ~0 },
    { L_,     1, "1..1",     1, BPW-1,               ~1,                 ~0 },
    { L_,     1, "1..1", BPW-1,     0,                0,                 ~0 },
    { L_,     1, "1..1", BPW-1,     1,                2,                 ~0 },
    { L_,     1, "1..1",   BPW,     0,                0,                 ~0 },

    { L_,     1,  FW_01,     0,     0,                0,                 ~0 },
    { L_,     1,  FW_01,     0,     1,                2,                 ~0 },
    { L_,     1,  FW_01,     0, BPW-1,         g(FW_10),         g(FW_10)|1 },
    { L_,     1,  FW_01,     0,   BPW,         g(FW_10),         g(FW_10)|1 },
    { L_,     1,  FW_01,     1,     0,                0,                 ~0 },
    { L_,     1,  FW_01,     1,     1,                0,                 -3 },
    { L_,     1,  FW_01,     1, BPW-1,       g(FW_01)^1,           g(FW_01) },
    { L_,     1,  FW_01, BPW-1,     0,                0,                 ~0 },
    { L_,     1,  FW_01, BPW-1,     1,                0,                 ~2 },
    { L_,     1,  FW_01,   BPW,     0,                0,                 ~0 },

    { L_,     1,  FW_10,     0,     0,                0,                 ~0 },
    { L_,     1,  FW_10,     0,     1,                0,                 -3 },
    { L_,     1,  FW_10,     0, BPW-1,       g(FW_01)^1 ,          g(FW_01) },
    { L_,     1,  FW_10,     0,   BPW,       g(FW_01)^1,           g(FW_01) },
    { L_,     1,  FW_10,     1,     0,                0,                 ~0 },
    { L_,     1,  FW_10,     1,     1,                2,                 ~0 },
    { L_,     1,  FW_10,     1, BPW-1,         g(FW_10),         g(FW_10)|1 },
    { L_,     1,  FW_10, BPW-1,     0,                0,                 ~0 },
    { L_,     1,  FW_10, BPW-1,     1,                2,                 ~0 },
    { L_,     1,  FW_10,   BPW,     0,                0,                 ~0 },

    //             SRCA      =          "1101101100100"
    { L_,     1,   SRCA,     2,     4,   g("000010010"), g("1..1111110011") },
    { L_,     1,   SRCA,     2,     5,   g("000110010"), g("1..1111110011") },
    { L_,     1,   SRCA,     2,     6,   g("000110010"), g("1..1110110011") },
    { L_,     1,   SRCA,     2,     7,   g("010110010"), g("1..1110110011") },
    { L_,     1,   SRCA,     2,     8,   g("110110010"), g("1..1110110011") },
    { L_,     1,   SRCA,     3,     4,   g("000011000"), g("1..1111111001") },
    { L_,     1,   SRCA,     3,     5,   g("000011000"), g("1..1111011001") },
    { L_,     1,   SRCA,     3,     6,   g("001011000"), g("1..1111011001") },
    { L_,     1,   SRCA,     3,     7,   g("011011000"), g("1..1111011001") },
    { L_,     1,   SRCA,     3,     8,   g("011011000"), g("1..1011011001") },

    { L_, BPW-1, "0..0",     0,     0,                0,                 ~0 },
    { L_, BPW-1, "0..0",     0,     1,                0,            INT_MAX },
    { L_, BPW-1, "0..0",     0, BPW-1,                0,            INT_MAX },
    { L_, BPW-1, "0..0",     0,   BPW,                0,            INT_MAX },
    { L_, BPW-1, "0..0",     1,     0,                0,                 ~0 },
    { L_, BPW-1, "0..0",     1,     1,                0,            INT_MAX },
    { L_, BPW-1, "0..0",     1, BPW-1,                0,            INT_MAX },
    { L_, BPW-1, "0..0", BPW-1,     0,                0,                 ~0 },
    { L_, BPW-1, "0..0", BPW-1,     1,                0,            INT_MAX },
    { L_, BPW-1, "0..0",   BPW,     0,                0,                 ~0 },

    { L_, BPW-1, "1..1",     0,     0,                0,                 ~0 },
    { L_, BPW-1, "1..1",     0,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1",     0, BPW-1,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1",     0,   BPW,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1",     1,     0,                0,                 ~0 },
    { L_, BPW-1, "1..1",     1,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1",     1, BPW-1,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1", BPW-1,     0,                0,                 ~0 },
    { L_, BPW-1, "1..1", BPW-1,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1, "1..1",   BPW,     0,                0,                 ~0 },

    { L_, BPW-1,  FW_01,     0,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_01,     0,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_01,     0, BPW-1,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_01,     0,   BPW,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_01,     1,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_01,     1,     1,                0,            INT_MAX },
    { L_, BPW-1,  FW_01,     1, BPW-1,                0,            INT_MAX },
    { L_, BPW-1,  FW_01, BPW-1,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_01, BPW-1,     1,                0,            INT_MAX },
    { L_, BPW-1,  FW_01,   BPW,     0,                0,                 ~0 },

    { L_, BPW-1,  FW_10,     0,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_10,     0,     1,                0,            INT_MAX },
    { L_, BPW-1,  FW_10,     0, BPW-1,                0,            INT_MAX },
    { L_, BPW-1,  FW_10,     0,   BPW,                0,            INT_MAX },
    { L_, BPW-1,  FW_10,     1,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_10,     1,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_10,     1, BPW-1,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_10, BPW-1,     0,                0,                 ~0 },
    { L_, BPW-1,  FW_10, BPW-1,     1,          INT_MIN,                 ~0 },
    { L_, BPW-1,  FW_10,   BPW,     0,                0,                 ~0 },

    //             SRCA      =          "1101101100100"
    { L_, BPW-1,   SRCA,     2,     4,          INT_MIN,                 ~0 },
    { L_, BPW-1,   SRCA,     2,     5,          INT_MIN,                 ~0 },
    { L_, BPW-1,   SRCA,     2,     6,          INT_MIN,                 ~0 },
    { L_, BPW-1,   SRCA,     2,     7,          INT_MIN,                 ~0 },
    { L_, BPW-1,   SRCA,     2,     8,          INT_MIN,                 ~0 },
    { L_, BPW-1,   SRCA,     3,     4,                0,            INT_MAX },
    { L_, BPW-1,   SRCA,     3,     5,                0,            INT_MAX },
    { L_, BPW-1,   SRCA,     3,     6,                0,            INT_MAX },
    { L_, BPW-1,   SRCA,     3,     7,                0,            INT_MAX },
    { L_, BPW-1,   SRCA,     3,     8,                0,            INT_MAX },

    { L_,   BPW, "0..0",     0,     0,                0,                 ~0 },
    { L_,   BPW, "0..0",     0,     1,                0,                 ~0 },
    { L_,   BPW, "0..0",     0, BPW-1,                0,                 ~0 },
    { L_,   BPW, "0..0",     0,   BPW,                0,                 ~0 },
    { L_,   BPW, "0..0",     1,     0,                0,                 ~0 },
    { L_,   BPW, "0..0",     1,     1,                0,                 ~0 },
    { L_,   BPW, "0..0",     1, BPW-1,                0,                 ~0 },
    { L_,   BPW, "0..0", BPW-1,     0,                0,                 ~0 },
    { L_,   BPW, "0..0", BPW-1,     1,                0,                 ~0 },
    { L_,   BPW, "0..0",   BPW,     0,                0,                 ~0 },

    { L_,   BPW, "1..1",     0,     0,                0,                 ~0 },
    { L_,   BPW, "1..1",     0,     1,                0,                 ~0 },
    { L_,   BPW, "1..1",     0, BPW-1,                0,                 ~0 },
    { L_,   BPW, "1..1",     0,   BPW,                0,                 ~0 },
    { L_,   BPW, "1..1",     1,     0,                0,                 ~0 },
    { L_,   BPW, "1..1",     1,     1,                0,                 ~0 },
    { L_,   BPW, "1..1",     1, BPW-1,                0,                 ~0 },
    { L_,   BPW, "1..1", BPW-1,     0,                0,                 ~0 },
    { L_,   BPW, "1..1", BPW-1,     1,                0,                 ~0 },
    { L_,   BPW, "1..1",   BPW,     0,                0,                 ~0 },

    { L_,   BPW,  FW_01,     0,     0,                0,                 ~0 },
    { L_,   BPW,  FW_01,     0,     1,                0,                 ~0 },
    { L_,   BPW,  FW_01,     0, BPW-1,                0,                 ~0 },
    { L_,   BPW,  FW_01,     0,   BPW,                0,                 ~0 },
    { L_,   BPW,  FW_01,     1,     0,                0,                 ~0 },
    { L_,   BPW,  FW_01,     1,     1,                0,                 ~0 },
    { L_,   BPW,  FW_01,     1, BPW-1,                0,                 ~0 },
    { L_,   BPW,  FW_01, BPW-1,     0,                0,                 ~0 },
    { L_,   BPW,  FW_01, BPW-1,     1,                0,                 ~0 },
    { L_,   BPW,  FW_01,   BPW,     0,                0,                 ~0 },

    { L_,   BPW,  FW_10,     0,     0,                0,                 ~0 },
    { L_,   BPW,  FW_10,     0,     1,                0,                 ~0 },
    { L_,   BPW,  FW_10,     0, BPW-1,                0,                 ~0 },
    { L_,   BPW,  FW_10,     0,   BPW,                0,                 ~0 },
    { L_,   BPW,  FW_10,     1,     0,                0,                 ~0 },
    { L_,   BPW,  FW_10,     1,     1,                0,                 ~0 },
    { L_,   BPW,  FW_10,     1, BPW-1,                0,                 ~0 },
    { L_,   BPW,  FW_10, BPW-1,     0,                0,                 ~0 },
    { L_,   BPW,  FW_10, BPW-1,     1,                0,                 ~0 },
    { L_,   BPW,  FW_10,   BPW,     0,                0,                 ~0 },

    //             SRCA      =          "1101101100100"
    { L_,   BPW,   SRCA,     2,     4,                0,                 ~0 },
    { L_,   BPW,   SRCA,     2,     5,                0,                 ~0 },
    { L_,   BPW,   SRCA,     2,     6,                0,                 ~0 },
    { L_,   BPW,   SRCA,     2,     7,                0,                 ~0 },
    { L_,   BPW,   SRCA,     2,     8,                0,                 ~0 },
    { L_,   BPW,   SRCA,     3,     4,                0,                 ~0 },
    { L_,   BPW,   SRCA,     3,     5,                0,                 ~0 },
    { L_,   BPW,   SRCA,     3,     6,                0,                 ~0 },
    { L_,   BPW,   SRCA,     3,     7,                0,                 ~0 },
    { L_,   BPW,   SRCA,     3,     8,                0,                 ~0 }
    //^----v
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::extractZero(src, DATA_A[di].d_index,
                          DATA_A[di].d_numBits, DATA_A[di].d_offset);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract0);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_extract0 == resA);

            resA = Util::extractOne(src, DATA_A[di].d_index,
                      DATA_A[di].d_numBits, DATA_A[di].d_offset);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract1);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_extract1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing extract0, extract1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::extract0(src, DATA_A[di].d_index,
                          DATA_A[di].d_numBits, DATA_A[di].d_offset);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract0);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_extract0 == resA);

            resA = Util::extract1(src, DATA_A[di].d_index,
                      DATA_A[di].d_numBits, DATA_A[di].d_offset);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract1);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_extract1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing extractZero64 and extractOne64 Functions" << endl
            << "============================================" << endl;

        const char *SRCB = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            int         d_offset;    // source integer offset
            const char *d_src;       // source integer
            int         d_index;     // source index
            int         d_numBits;   // number of bits
            Int64       d_extract0;  // expected result from extract0 function
            Int64       d_extract1;  // expected result from extract1 function
        } DATA_B[] = {
    //v----^
    //L# offset     src   Index NumBits extract0 result     extract1 result
    //-- ------  ------ ------- ------- ---------------     ---------------
    { L_,     0, "0..0",     0,     0,                0,                 ~0 },
    { L_,     0, "0..0",     0,     1,                0,                 ~1 },
    { L_,     0, "0..0",     0, BPS-1,                0,           int64Min },
    { L_,     0, "0..0",     0,   BPS,                0,                  0 },
    { L_,     0, "0..0",     1,     0,                0,                 ~0 },
    { L_,     0, "0..0",     1,     1,                0,                 ~1 },
    { L_,     0, "0..0",     1, BPS-1,                0,           int64Min },
    { L_,     0, "0..0", BPS-1,     0,                0,                 ~0 },
    { L_,     0, "0..0", BPS-1,     1,                0,                 ~1 },
    { L_,     0, "0..0",   BPS,     0,                0,                 ~0 },

    { L_,     0, "1..1",     0,     0,                0,                 ~0 },
    { L_,     0, "1..1",     0,     1,                1,                 ~0 },
    { L_,     0, "1..1",     0, BPS-1,         int64Max,                 ~0 },
    { L_,     0, "1..1",     0,   BPS,               ~0,                 ~0 },
    { L_,     0, "1..1",     1,     0,                0,                 ~0 },
    { L_,     0, "1..1",     1,     1,                1,                 ~0 },
    { L_,     0, "1..1",     1, BPS-1,         int64Max,                 ~0 },
    { L_,     0, "1..1", BPS-1,     0,                0,                 ~0 },
    { L_,     0, "1..1", BPS-1,     1,                1,                 ~0 },
    { L_,     0, "1..1",   BPS,     0,                0,                 ~0 },

    { L_,     0,  SW_01,     0,     0,                0,                 ~0 },
    { L_,     0,  SW_01,     0,     1,                1,                 ~0 },
    { L_,     0,  SW_01,     0, BPS-1,       g64(SW_01),g64(SW_01)|int64Min },
    { L_,     0,  SW_01,     0,   BPS,       g64(SW_01),         g64(SW_01) },
    { L_,     0,  SW_01,     1,     0,                0,                 ~0 },
    { L_,     0,  SW_01,     1,     1,                0,                 ~1 },
    { L_,     0,  SW_01,     1, BPS-1,g64(SW_10)^int64Min,       g64(SW_10) },
    { L_,     0,  SW_01, BPS-1,     0,                0,                 ~0 },
    { L_,     0,  SW_01, BPS-1,     1,                0,                 ~1 },
    { L_,     0,  SW_01,   BPS,     0,                0,                 ~0 },

    { L_,     0,  SW_10,     0,     0,                0,                 ~0 },
    { L_,     0,  SW_10,     0,     1,                0,                 ~1 },
    { L_,     0,  SW_10,     0, BPS-1,g64(SW_10)^int64Min,       g64(SW_10) },
    { L_,     0,  SW_10,     0,   BPS,       g64(SW_10),         g64(SW_10) },
    { L_,     0,  SW_10,     1,     0,                0,                 ~0 },
    { L_,     0,  SW_10,     1,     1,                1,                 ~0 },
    { L_,     0,  SW_10,     1, BPS-1,       g64(SW_01),g64(SW_01)|int64Min },
    { L_,     0,  SW_10, BPS-1,     0,                0,                 ~0 },
    { L_,     0,  SW_10, BPS-1,     1,                1,                 ~0 },
    { L_,     0,  SW_10,   BPS,     0,                0,                 ~0 },

    //             SRCB      =          "1101101100100"
    { L_,     0,   SRCB,     2,     4,  g64("00001001"),g64("1..111111001") },
    { L_,     0,   SRCB,     2,     5,  g64("00011001"),g64("1..111111001") },
    { L_,     0,   SRCB,     2,     6,  g64("00011001"),g64("1..111011001") },
    { L_,     0,   SRCB,     2,     7,  g64("01011001"),g64("1..111011001") },
    { L_,     0,   SRCB,     2,     8,  g64("11011001"),g64("1..111011001") },
    { L_,     0,   SRCB,     3,     4,  g64("00001100"),g64("1..111111100") },
    { L_,     0,   SRCB,     3,     5,  g64("00001100"),g64("1..111101100") },
    { L_,     0,   SRCB,     3,     6,  g64("00101100"),g64("1..111101100") },
    { L_,     0,   SRCB,     3,     7,  g64("01101100"),g64("1..111101100") },
    { L_,     0,   SRCB,     3,     8,  g64("01101100"),g64("1..101101100") },

    { L_,     1, "0..0",     0,     0,                0,                 ~0 },
    { L_,     1, "0..0",     0,     1,                0,                 -3 },
    { L_,     1, "0..0",     0, BPS-1,                0,                  1 },
    { L_,     1, "0..0",     0,   BPS,                0,                  1 },
    { L_,     1, "0..0",     1,     0,                0,                 ~0 },
    { L_,     1, "0..0",     1,     1,                0,                 -3 },
    { L_,     1, "0..0",     1, BPS-1,                0,                  1 },
    { L_,     1, "0..0", BPS-1,     0,                0,                 ~0 },
    { L_,     1, "0..0", BPS-1,     1,                0,                 ~2 },
    { L_,     1, "0..0",   BPS,     0,                0,                 ~0 },

    { L_,     1, "1..1",     0,     0,                0,                 ~0 },
    { L_,     1, "1..1",     0,     1,                2,                 ~0 },
    { L_,     1, "1..1",     0, BPS-1,               ~1,                 ~0 },
    { L_,     1, "1..1",     0,   BPS,               ~1,                 ~0 },
    { L_,     1, "1..1",     1,     0,                0,                 ~0 },
    { L_,     1, "1..1",     1,     1,                2,                 ~0 },
    { L_,     1, "1..1",     1, BPS-1,               ~1,                 ~0 },
    { L_,     1, "1..1", BPS-1,     0,                0,                 ~0 },
    { L_,     1, "1..1", BPS-1,     1,                2,                 ~0 },
    { L_,     1, "1..1",   BPS,     0,                0,                 ~0 },

    { L_,     1,  SW_01,     0,     0,                0,                 ~0 },
    { L_,     1,  SW_01,     0,     1,                2,                 ~0 },
    { L_,     1,  SW_01,     0, BPS-1,       g64(SW_10),       g64(SW_10)|1 },
    { L_,     1,  SW_01,     0,   BPS,       g64(SW_10),       g64(SW_10)|1 },
    { L_,     1,  SW_01,     1,     0,                0,                 ~0 },
    { L_,     1,  SW_01,     1,     1,                0,                 -3 },
    { L_,     1,  SW_01,     1, BPS-1,     g64(SW_01)^1,         g64(SW_01) },
    { L_,     1,  SW_01, BPS-1,     0,                0,                 ~0 },
    { L_,     1,  SW_01, BPS-1,     1,                0,                 ~2 },
    { L_,     1,  SW_01,   BPS,     0,                0,                 ~0 },

    { L_,     1,  SW_10,     0,     0,                0,                 ~0 },
    { L_,     1,  SW_10,     0,     1,                0,                 -3 },
    { L_,     1,  SW_10,     0, BPS-1,     g64(SW_01)^1 ,        g64(SW_01) },
    { L_,     1,  SW_10,     0,   BPS,     g64(SW_01)^1,         g64(SW_01) },
    { L_,     1,  SW_10,     1,     0,                0,                 ~0 },
    { L_,     1,  SW_10,     1,     1,                2,                 ~0 },
    { L_,     1,  SW_10,     1, BPS-1,       g64(SW_10),       g64(SW_10)|1 },
    { L_,     1,  SW_10, BPS-1,     0,                0,                 ~0 },
    { L_,     1,  SW_10, BPS-1,     1,                2,                 ~0 },
    { L_,     1,  SW_10,   BPS,     0,                0,                 ~0 },

    //             SRCB      =          "1101101100100"
    { L_,     1,   SRCB,     2,     4,g64("000010010"),g64("1..1111110011") },
    { L_,     1,   SRCB,     2,     5,g64("000110010"),g64("1..1111110011") },
    { L_,     1,   SRCB,     2,     6,g64("000110010"),g64("1..1110110011") },
    { L_,     1,   SRCB,     2,     7,g64("010110010"),g64("1..1110110011") },
    { L_,     1,   SRCB,     2,     8,g64("110110010"),g64("1..1110110011") },
    { L_,     1,   SRCB,     3,     4,g64("000011000"),g64("1..1111111001") },
    { L_,     1,   SRCB,     3,     5,g64("000011000"),g64("1..1111011001") },
    { L_,     1,   SRCB,     3,     6,g64("001011000"),g64("1..1111011001") },
    { L_,     1,   SRCB,     3,     7,g64("011011000"),g64("1..1111011001") },
    { L_,     1,   SRCB,     3,     8,g64("011011000"),g64("1..1011011001") },

    { L_, BPS-1, "0..0",     0,     0,                0,                 ~0 },
    { L_, BPS-1, "0..0",     0,     1,                0,           int64Max },
    { L_, BPS-1, "0..0",     0, BPS-1,                0,           int64Max },
    { L_, BPS-1, "0..0",     0,   BPS,                0,           int64Max },
    { L_, BPS-1, "0..0",     1,     0,                0,                 ~0 },
    { L_, BPS-1, "0..0",     1,     1,                0,           int64Max },
    { L_, BPS-1, "0..0",     1, BPS-1,                0,           int64Max },
    { L_, BPS-1, "0..0", BPS-1,     0,                0,                 ~0 },
    { L_, BPS-1, "0..0", BPS-1,     1,                0,           int64Max },
    { L_, BPS-1, "0..0",   BPS,     0,                0,                 ~0 },

    { L_, BPS-1, "1..1",     0,     0,                0,                 ~0 },
    { L_, BPS-1, "1..1",     0,     1,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1",     0, BPS-1,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1",     0,   BPS,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1",     1,     0,                0,                 ~0 },
    { L_, BPS-1, "1..1",     1,     1,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1",     1, BPS-1,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1", BPS-1,     0,                0,                 ~0 },
    { L_, BPS-1, "1..1", BPS-1,     1,         int64Min,                 ~0 },
    { L_, BPS-1, "1..1",   BPS,     0,                0,                 ~0 },

    { L_, BPS-1,  SW_01,     0,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_01,     0,     1,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_01,     0, BPS-1,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_01,     0,   BPS,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_01,     1,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_01,     1,     1,                0,           int64Max },
    { L_, BPS-1,  SW_01,     1, BPS-1,                0,           int64Max },
    { L_, BPS-1,  SW_01, BPS-1,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_01, BPS-1,     1,                0,           int64Max },
    { L_, BPS-1,  SW_01,   BPS,     0,                0,                 ~0 },

    { L_, BPS-1,  SW_10,     0,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_10,     0,     1,                0,           int64Max },
    { L_, BPS-1,  SW_10,     0, BPS-1,                0,           int64Max },
    { L_, BPS-1,  SW_10,     0,   BPS,                0,           int64Max },
    { L_, BPS-1,  SW_10,     1,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_10,     1,     1,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_10,     1, BPS-1,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_10, BPS-1,     0,                0,                 ~0 },
    { L_, BPS-1,  SW_10, BPS-1,     1,         int64Min,                 ~0 },
    { L_, BPS-1,  SW_10,   BPS,     0,                0,                 ~0 },

    //             SRCB      =           "1101101100100"
    { L_, BPS-1,   SRCB,     2,     4,         int64Min,                 ~0 },
    { L_, BPS-1,   SRCB,     2,     5,         int64Min,                 ~0 },
    { L_, BPS-1,   SRCB,     2,     6,         int64Min,                 ~0 },
    { L_, BPS-1,   SRCB,     2,     7,         int64Min,                 ~0 },
    { L_, BPS-1,   SRCB,     2,     8,         int64Min,                 ~0 },
    { L_, BPS-1,   SRCB,     3,     4,                0,           int64Max },
    { L_, BPS-1,   SRCB,     3,     5,                0,           int64Max },
    { L_, BPS-1,   SRCB,     3,     6,                0,           int64Max },
    { L_, BPS-1,   SRCB,     3,     7,                0,           int64Max },
    { L_, BPS-1,   SRCB,     3,     8,                0,           int64Max },

    { L_,   BPS, "0..0",     0,     0,                0,                 ~0 },
    { L_,   BPS, "0..0",     0,     1,                0,                 ~0 },
    { L_,   BPS, "0..0",     0, BPS-1,                0,                 ~0 },
    { L_,   BPS, "0..0",     0,   BPS,                0,                 ~0 },
    { L_,   BPS, "0..0",     1,     0,                0,                 ~0 },
    { L_,   BPS, "0..0",     1,     1,                0,                 ~0 },
    { L_,   BPS, "0..0",     1, BPS-1,                0,                 ~0 },
    { L_,   BPS, "0..0", BPS-1,     0,                0,                 ~0 },
    { L_,   BPS, "0..0", BPS-1,     1,                0,                 ~0 },
    { L_,   BPS, "0..0",   BPS,     0,                0,                 ~0 },

    { L_,   BPS, "1..1",     0,     0,                0,                 ~0 },
    { L_,   BPS, "1..1",     0,     1,                0,                 ~0 },
    { L_,   BPS, "1..1",     0, BPS-1,                0,                 ~0 },
    { L_,   BPS, "1..1",     0,   BPS,                0,                 ~0 },
    { L_,   BPS, "1..1",     1,     0,                0,                 ~0 },
    { L_,   BPS, "1..1",     1,     1,                0,                 ~0 },
    { L_,   BPS, "1..1",     1, BPS-1,                0,                 ~0 },
    { L_,   BPS, "1..1", BPS-1,     0,                0,                 ~0 },
    { L_,   BPS, "1..1", BPS-1,     1,                0,                 ~0 },
    { L_,   BPS, "1..1",   BPS,     0,                0,                 ~0 },

    { L_,   BPS,  SW_01,     0,     0,                0,                 ~0 },
    { L_,   BPS,  SW_01,     0,     1,                0,                 ~0 },
    { L_,   BPS,  SW_01,     0, BPS-1,                0,                 ~0 },
    { L_,   BPS,  SW_01,     0,   BPS,                0,                 ~0 },
    { L_,   BPS,  SW_01,     1,     0,                0,                 ~0 },
    { L_,   BPS,  SW_01,     1,     1,                0,                 ~0 },
    { L_,   BPS,  SW_01,     1, BPS-1,                0,                 ~0 },
    { L_,   BPS,  SW_01, BPS-1,     0,                0,                 ~0 },
    { L_,   BPS,  SW_01, BPS-1,     1,                0,                 ~0 },
    { L_,   BPS,  SW_01,   BPS,     0,                0,                 ~0 },

    { L_,   BPS,  SW_10,     0,     0,                0,                 ~0 },
    { L_,   BPS,  SW_10,     0,     1,                0,                 ~0 },
    { L_,   BPS,  SW_10,     0, BPS-1,                0,                 ~0 },
    { L_,   BPS,  SW_10,     0,   BPS,                0,                 ~0 },
    { L_,   BPS,  SW_10,     1,     0,                0,                 ~0 },
    { L_,   BPS,  SW_10,     1,     1,                0,                 ~0 },
    { L_,   BPS,  SW_10,     1, BPS-1,                0,                 ~0 },
    { L_,   BPS,  SW_10, BPS-1,     0,                0,                 ~0 },
    { L_,   BPS,  SW_10, BPS-1,     1,                0,                 ~0 },
    { L_,   BPS,  SW_10,   BPS,     0,                0,                 ~0 },

    //             SRCB      =          "1101101100100"
    { L_,   BPS,   SRCB,     2,     4,                0,                 ~0 },
    { L_,   BPS,   SRCB,     2,     5,                0,                 ~0 },
    { L_,   BPS,   SRCB,     2,     6,                0,                 ~0 },
    { L_,   BPS,   SRCB,     2,     7,                0,                 ~0 },
    { L_,   BPS,   SRCB,     2,     8,                0,                 ~0 },
    { L_,   BPS,   SRCB,     3,     4,                0,                 ~0 },
    { L_,   BPS,   SRCB,     3,     5,                0,                 ~0 },
    { L_,   BPS,   SRCB,     3,     6,                0,                 ~0 },
    { L_,   BPS,   SRCB,     3,     7,                0,                 ~0 },
    { L_,   BPS,   SRCB,     3,     8,                0,                 ~0 }
    //^----v
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE   = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_src;

            Int64 src = g64(spec);
            Int64 res = Util::extractZero64(src, DATA_B[di].d_index,
                          DATA_B[di].d_numBits, DATA_B[di].d_offset);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_extract0); P(res);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_extract0 == res);

            res = Util::extractOne64(src, DATA_B[di].d_index,
                      DATA_B[di].d_numBits, DATA_B[di].d_offset);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_extract1); P(res);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_extract1 == res);
        }
}

static
void testF(int test)
{
    switch (test) {
      case 30: {
        testF30();
      } break;
      case 29: {
        testF29();
      } break;
      case 28: {
        testF28();
      } break;
      case 27: {
        testF27();
      } break;
      case 26: {
        testF26();
      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testE25()
{
        // --------------------------------------------------------------------
        // TESTING INSERTVALUE FUNCTION:
        //   'insertValue' has a reasonably simple implementation that performs
        //   calculations using already-tested methods.  A relatively small set
        //   of test data is sufficient, but we choose a more thorough and
        //   systematic set of test data to completely probe the algorithm and
        //    not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by value and
        //   then destination category, verify that the insertValue function
        //   gives the expected value.
        //
        // Testing:
        //   void insertValue(int *dInt, int dIdx, int sInt, int nBits);
        //   void insertValue64(bsls_Types::Int64 *dInt,
        //                      int dIdx,
        //                      bsls_Types::Int64 sInt,
        //                      int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing insertValue Function" << endl
                          << "============================" << endl;

        const char *DSTA = "1101101100100"; // typical case
        static const struct {
            int         d_lineNum;         // line number
            const char *d_value;           // source value
            const char *d_dst;             // destination integer
            int         d_index;           // destination index
            int         d_nBits;           // number of bits to be inserted
            int         d_insertValue;     // expected result from insertValue
        } DATA_A[] = {
           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,  "0..0",  "0..0",     0,     0,                          0 },
           { L_,  "0..0",  "0..0",     0,     1,                          0 },
           { L_,  "0..0",  "0..0",     0, BPW-1,                          0 },
           { L_,  "0..0",  "0..0",     0,   BPW,                          0 },
           { L_,  "0..0",  "0..0",     1,     0,                          0 },
           { L_,  "0..0",  "0..0",     1,     1,                          0 },
           { L_,  "0..0",  "0..0",     1, BPW-1,                          0 },
           { L_,  "0..0",  "0..0", BPW-1,     0,                          0 },
           { L_,  "0..0",  "0..0", BPW-1,     1,                          0 },
           { L_,  "0..0",  "0..0",   BPW,     0,                          0 },

           { L_,  "0..0",  "1..1",     0,     0,                         ~0 },
           { L_,  "0..0",  "1..1",     0,     1,                         ~1 },
           { L_,  "0..0",  "1..1",     0, BPW-1,                    INT_MIN },
           { L_,  "0..0",  "1..1",     0,   BPW,                          0 },
           { L_,  "0..0",  "1..1",     1,     0,                         ~0 },
           { L_,  "0..0",  "1..1",     1,     1,                         ~2 },
           { L_,  "0..0",  "1..1",     1, BPW-1,                          1 },
           { L_,  "0..0",  "1..1", BPW-1,     0,                         ~0 },
           { L_,  "0..0",  "1..1", BPW-1,     1,                    INT_MAX },
           { L_,  "0..0",  "1..1",   BPW,     0,                         ~0 },

           { L_,  "0..0",   FW_01,     0,     0,                   g(FW_01) },
           { L_,  "0..0",   FW_01,     0,     1,                   g(FW_10) },
           { L_,  "0..0",   FW_01,     0, BPW-1,                    INT_MIN },
           { L_,  "0..0",   FW_01,     0,   BPW,                          0 },
           { L_,  "0..0",   FW_01,     1,     0,                   g(FW_01) },
           { L_,  "0..0",   FW_01,     1,     1,                 g(FW_10)^3 },
           { L_,  "0..0",   FW_01,     1, BPW-1,                          1 },
           { L_,  "0..0",   FW_01, BPW-1,     0,                   g(FW_01) },
           { L_,  "0..0",   FW_01, BPW-1,     1,                   g(FW_01) },
           { L_,  "0..0",   FW_01,   BPW,     0,                   g(FW_01) },

           { L_,  "0..0",   FW_10,     0,     0,                   g(FW_10) },
           { L_,  "0..0",   FW_10,     0,     1,                 g(FW_01)^1 },
           { L_,  "0..0",   FW_10,     0, BPW-1,                          0 },
           { L_,  "0..0",   FW_10,     0,   BPW,                          0 },
           { L_,  "0..0",   FW_10,     1,     0,                   g(FW_10) },
           { L_,  "0..0",   FW_10,     1,     1,                 g(FW_01)^1 },
           { L_,  "0..0",   FW_10,     1, BPW-1,                          0 },
           { L_,  "0..0",   FW_10, BPW-1,     0,                   g(FW_10) },
           { L_,  "0..0",   FW_10, BPW-1,     1,           g(FW_10)^INT_MIN },
           { L_,  "0..0",   FW_10,   BPW,     0,                   g(FW_10) },

           { L_,  "0..0",    DSTA,     2,     4,  g("000011011011001000000")},
           { L_,  "0..0",    DSTA,     2,     5,  g("000110110110010000000")},
           { L_,  "0..0",    DSTA,     2,     6,  g("001101101100100000000")},
           { L_,  "0..0",    DSTA,     2,     7,  g("011011011001000000000")},
           { L_,  "0..0",    DSTA,     2,     8,  g("110110110010000000000")},
           { L_,  "0..0",    DSTA,     3,     4,  g("000011011011000000100")},
           { L_,  "0..0",    DSTA,     3,     5,  g("000110110110000000100")},
           { L_,  "0..0",    DSTA,     3,     6,  g("001101101100000000100")},
           { L_,  "0..0",    DSTA,     3,     7,  g("011011011000000000100")},
           { L_,  "0..0",    DSTA,     3,     8,  g("110110110000000000100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,  "1..1",  "0..0",     0,     0,                          0 },
           { L_,  "1..1",  "0..0",     0,     1,                          1 },
           { L_,  "1..1",  "0..0",     0, BPW-1,                    INT_MAX },
           { L_,  "1..1",  "0..0",     0,   BPW,                         ~0 },
           { L_,  "1..1",  "0..0",     1,     0,                          0 },
           { L_,  "1..1",  "0..0",     1,     1,                          2 },
           { L_,  "1..1",  "0..0",     1, BPW-1,                         ~1 },
           { L_,  "1..1",  "0..0", BPW-1,     0,                          0 },
           { L_,  "1..1",  "0..0", BPW-1,     1,                    INT_MIN },
           { L_,  "1..1",  "0..0",   BPW,     0,                          0 },

           { L_,  "1..1",  "1..1",     0,     0,                         ~0 },
           { L_,  "1..1",  "1..1",     0,     1,                         ~0 },
           { L_,  "1..1",  "1..1",     0, BPW-1,                         ~0 },
           { L_,  "1..1",  "1..1",     0,   BPW,                         ~0 },
           { L_,  "1..1",  "1..1",     1,     0,                         ~0 },
           { L_,  "1..1",  "1..1",     1,     1,                         ~0 },
           { L_,  "1..1",  "1..1",     1, BPW-1,                         ~0 },
           { L_,  "1..1",  "1..1", BPW-1,     0,                         ~0 },
           { L_,  "1..1",  "1..1", BPW-1,     1,                         ~0 },
           { L_,  "1..1",  "1..1",   BPW,     0,                         ~0 },

           { L_,  "1..1",   FW_01,     0,     0,                   g(FW_01) },
           { L_,  "1..1",   FW_01,     0,     1,                 g(FW_10)|1 },
           { L_,  "1..1",   FW_01,     0, BPW-1,                         ~0 },
           { L_,  "1..1",   FW_01,     0,   BPW,                         ~0 },
           { L_,  "1..1",   FW_01,     1,     0,                   g(FW_01) },
           { L_,  "1..1",   FW_01,     1,     1,                 g(FW_10)|1 },
           { L_,  "1..1",   FW_01,     1, BPW-1,                         ~0 },
           { L_,  "1..1",   FW_01, BPW-1,     0,                   g(FW_01) },
           { L_,  "1..1",   FW_01, BPW-1,     1,           g(FW_01)|INT_MIN },
           { L_,  "1..1",   FW_01,   BPW,     0,                   g(FW_01) },

           { L_,  "1..1",   FW_10,     0,     0,                   g(FW_10) },
           { L_,  "1..1",   FW_10,     0,     1,                   g(FW_01) },
           { L_,  "1..1",   FW_10,     0, BPW-1,                    INT_MAX },
           { L_,  "1..1",   FW_10,     0,   BPW,                         ~0 },
           { L_,  "1..1",   FW_10,     1,     0,                   g(FW_10) },
           { L_,  "1..1",   FW_10,     1,     1,                 g(FW_01)+1 },
           { L_,  "1..1",   FW_10,     1, BPW-1,                         ~1 },
           { L_,  "1..1",   FW_10, BPW-1,     0,                   g(FW_10) },
           { L_,  "1..1",   FW_10, BPW-1,     1,                   g(FW_10) },
           { L_,  "1..1",   FW_10,   BPW,     0,                   g(FW_10) },

           { L_,  "1..1",    DSTA,     2,     4,  g("000011011011001111100")},
           { L_,  "1..1",    DSTA,     2,     5,  g("000110110110011111100")},
           { L_,  "1..1",    DSTA,     2,     6,  g("001101101100111111100")},
           { L_,  "1..1",    DSTA,     2,     7,  g("011011011001111111100")},
           { L_,  "1..1",    DSTA,     2,     8,  g("110110110011111111100")},
           { L_,  "1..1",    DSTA,     3,     4,  g("000011011011001111100")},
           { L_,  "1..1",    DSTA,     3,     5,  g("000110110110011111100")},
           { L_,  "1..1",    DSTA,     3,     6,  g("001101101100111111100")},
           { L_,  "1..1",    DSTA,     3,     7,  g("011011011001111111100")},
           { L_,  "1..1",    DSTA,     3,     8,  g("110110110011111111100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----          ------------------
           { L_,   FW_01,  "0..0",     0,     0,                          0 },
           { L_,   FW_01,  "0..0",     0,     1,                          1 },
           { L_,   FW_01,  "0..0",     0, BPW-1,                   g(FW_01) },
           { L_,   FW_01,  "0..0",     0,   BPW,                   g(FW_01) },
           { L_,   FW_01,  "0..0",     1,     0,                          0 },
           { L_,   FW_01,  "0..0",     1,     1,                          2 },
           { L_,   FW_01,  "0..0",     1, BPW-1,                   g(FW_10) },
           { L_,   FW_01,  "0..0", BPW-1,     0,                          0 },
           { L_,   FW_01,  "0..0", BPW-1,     1,                    INT_MIN },
           { L_,   FW_01,  "0..0",   BPW,     0,                          0 },

           { L_,   FW_01,  "1..1",     0,     0,                         ~0 },
           { L_,   FW_01,  "1..1",     0,     1,                         ~0 },
           { L_,   FW_01,  "1..1",     0, BPW-1,           g(FW_01)|INT_MIN },
           { L_,   FW_01,  "1..1",     0,   BPW,                   g(FW_01) },
           { L_,   FW_01,  "1..1",     1,     0,                         ~0 },
           { L_,   FW_01,  "1..1",     1,     1,                         ~0 },
           { L_,   FW_01,  "1..1",     1, BPW-1,                 g(FW_10)|1 },
           { L_,   FW_01,  "1..1", BPW-1,     0,                         ~0 },
           { L_,   FW_01,  "1..1", BPW-1,     1,                         ~0 },
           { L_,   FW_01,  "1..1",   BPW,     0,                         ~0 },

           { L_,   FW_01,   FW_01,     0,     0,                   g(FW_01) },
           { L_,   FW_01,   FW_01,     0,     1,                 g(FW_10)|1 },
           { L_,   FW_01,   FW_01,     0, BPW-1,           g(FW_01)|INT_MIN },
           { L_,   FW_01,   FW_01,     0,   BPW,                   g(FW_01) },
           { L_,   FW_01,   FW_01,     1,     0,                   g(FW_01) },
           { L_,   FW_01,   FW_01,     1,     1,                 g(FW_10)|1 },
           { L_,   FW_01,   FW_01,     1, BPW-1,                 g(FW_10)|1 },
           { L_,   FW_01,   FW_01, BPW-1,     0,                   g(FW_01) },
           { L_,   FW_01,   FW_01, BPW-1,     1,           g(FW_01)|INT_MIN },
           { L_,   FW_01,   FW_01,   BPW,     0,                   g(FW_01) },

           { L_,   FW_01,   FW_10,     0,     0,                   g(FW_10) },
           { L_,   FW_01,   FW_10,     0,     1,                   g(FW_01) },
           { L_,   FW_01,   FW_10,     0, BPW-1,                   g(FW_01) },
           { L_,   FW_01,   FW_10,     0,   BPW,                   g(FW_01) },
           { L_,   FW_01,   FW_10,     1,     0,                   g(FW_10) },
           { L_,   FW_01,   FW_10,     1,     1,                 g(FW_01)+1 },
           { L_,   FW_01,   FW_10,     1, BPW-1,                   g(FW_10) },
           { L_,   FW_01,   FW_10, BPW-1,     0,                   g(FW_10) },
           { L_,   FW_01,   FW_10, BPW-1,     1,                   g(FW_10) },
           { L_,   FW_01,   FW_10,   BPW,     0,                   g(FW_10) },

           { L_,   FW_01,    DSTA,     2,     4,  g("000011011011001010100")},
           { L_,   FW_01,    DSTA,     2,     5,  g("000110110110011010100")},
           { L_,   FW_01,    DSTA,     2,     6,  g("001101101100101010100")},
           { L_,   FW_01,    DSTA,     2,     7,  g("011011011001101010100")},
           { L_,   FW_01,    DSTA,     2,     8,  g("110110110010101010100")},
           { L_,   FW_01,    DSTA,     3,     4,  g("000011011011000101100")},
           { L_,   FW_01,    DSTA,     3,     5,  g("000110110110010101100")},
           { L_,   FW_01,    DSTA,     3,     6,  g("001101101100010101100")},
           { L_,   FW_01,    DSTA,     3,     7,  g("011011011001010101100")},
           { L_,   FW_01,    DSTA,     3,     8,  g("110110110001010101100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,   FW_10,  "0..0",     0,     0,                          0 },
           { L_,   FW_10,  "0..0",     0,     1,                          0 },
           { L_,   FW_10,  "0..0",     0, BPW-1,           g(FW_10)^INT_MIN },
           { L_,   FW_10,  "0..0",     0,   BPW,                   g(FW_10) },
           { L_,   FW_10,  "0..0",     1,     0,                          0 },
           { L_,   FW_10,  "0..0",     1,     1,                          0 },
           { L_,   FW_10,  "0..0",     1, BPW-1,                 g(FW_01)^1 },
           { L_,   FW_10,  "0..0", BPW-1,     0,                          0 },
           { L_,   FW_10,  "0..0", BPW-1,     1,                          0 },
           { L_,   FW_10,  "0..0",   BPW,     0,                          0 },

           { L_,   FW_10,  "1..1",     0,     0,                         ~0 },
           { L_,   FW_10,  "1..1",     0,     1,                         ~1 },
           { L_,   FW_10,  "1..1",     0, BPW-1,                   g(FW_10) },
           { L_,   FW_10,  "1..1",     0,   BPW,                   g(FW_10) },
           { L_,   FW_10,  "1..1",     1,     0,                         ~0 },
           { L_,   FW_10,  "1..1",     1,     1,                         ~2 },
           { L_,   FW_10,  "1..1",     1, BPW-1,                   g(FW_01) },
           { L_,   FW_10,  "1..1", BPW-1,     0,                         ~0 },
           { L_,   FW_10,  "1..1", BPW-1,     1,                    INT_MAX },
           { L_,   FW_10,  "1..1",   BPW,     0,                         ~0 },

           { L_,   FW_10,   FW_01,     0,     0,                   g(FW_01) },
           { L_,   FW_10,   FW_01,     0,     1,                   g(FW_10) },
           { L_,   FW_10,   FW_01,     0, BPW-1,                   g(FW_10) },
           { L_,   FW_10,   FW_01,     0,   BPW,                   g(FW_10) },
           { L_,   FW_10,   FW_01,     1,     0,                   g(FW_01) },
           { L_,   FW_10,   FW_01,     1,     1,                 g(FW_10)^3 },
           { L_,   FW_10,   FW_01,     1, BPW-1,                   g(FW_01) },
           { L_,   FW_10,   FW_01, BPW-1,     0,                   g(FW_01) },
           { L_,   FW_10,   FW_01, BPW-1,     1,                   g(FW_01) },
           { L_,   FW_10,   FW_01,   BPW,     0,                   g(FW_01) },

           { L_,   FW_10,   FW_10,     0,     0,                   g(FW_10) },
           { L_,   FW_10,   FW_10,     0,     1,                 g(FW_01)^1 },
           { L_,   FW_10,   FW_10,     0, BPW-1,           g(FW_10)^INT_MIN },
           { L_,   FW_10,   FW_10,     0,   BPW,                   g(FW_10) },
           { L_,   FW_10,   FW_10,     1,     0,                   g(FW_10) },
           { L_,   FW_10,   FW_10,     1,     1,                 g(FW_01)^1 },
           { L_,   FW_10,   FW_10,     1, BPW-1,                 g(FW_01)^1 },
           { L_,   FW_10,   FW_10, BPW-1,     0,                   g(FW_10) },
           { L_,   FW_10,   FW_10, BPW-1,     1,           g(FW_10)^INT_MIN },
           { L_,   FW_10,   FW_10,   BPW,     0,                   g(FW_10) },

           { L_,   FW_10,    DSTA,     2,     4,  g("000011011011001101000")},
           { L_,   FW_10,    DSTA,     2,     5,  g("000110110110010101000")},
           { L_,   FW_10,    DSTA,     2,     6,  g("001101101100110101000")},
           { L_,   FW_10,    DSTA,     2,     7,  g("011011011001010101000")},
           { L_,   FW_10,    DSTA,     2,     8,  g("110110110011010101000")},
           { L_,   FW_10,    DSTA,     3,     4,  g("000011011011001010100")},
           { L_,   FW_10,    DSTA,     3,     5,  g("000110110110001010100")},
           { L_,   FW_10,    DSTA,     3,     6,  g("001101101100101010100")},
           { L_,   FW_10,    DSTA,     3,     7,  g("011011011000101010100")},
           { L_,   FW_10,    DSTA,     3,     8,  g("110110110010101010100")},

  //v------^
  //L#         value          dst  Index  nBits          insertValue result
  //--   ---------------   ------  -----  -----         -------------------
  { L_, "111011101110111", "0..0",     0,     0,                          0 },
  { L_, "111011101110111", "0..0",     0,     1,                          1 },
  { L_, "111011101110111", "0..0",     0, BPW-1,       g("111011101110111") },
  { L_, "111011101110111", "0..0",     0,   BPW,       g("111011101110111") },
  { L_, "111011101110111", "0..0",     1,     0,                          0 },
  { L_, "111011101110111", "0..0",     1,     1,                          2 },
  { L_, "111011101110111", "0..0",     1, BPW-1,      g("1110111011101110") },
  { L_, "111011101110111", "0..0", BPW-1,     0,                          0 },
  { L_, "111011101110111", "0..0", BPW-1,     1,                    INT_MIN },
  { L_, "111011101110111", "0..0",   BPW,     0,                          0 },

  { L_, "111011101110111", "1..1",     0,     0,                         ~0 },
  { L_, "111011101110111", "1..1",     0,     1,                         ~0 },
  { L_, "111011101110111", "1..1",     0, BPW-1,  g("10..0111011101110111") },
  { L_, "111011101110111", "1..1",     0,   BPW,  g("00..0111011101110111") },
  { L_, "111011101110111", "1..1",     1,     0,                         ~0 },
  { L_, "111011101110111", "1..1",     1,     1,                         ~0 },
  { L_, "111011101110111", "1..1",     1, BPW-1,      g("1110111011101111") },
  { L_, "111011101110111", "1..1", BPW-1,     0,                         ~0 },
  { L_, "111011101110111", "1..1", BPW-1,     1,                         ~0 },
  { L_, "111011101110111", "1..1",   BPW,     0,                         ~0 },

  { L_, "111011101110111",  FW_01,     0,     0,                   g(FW_01) },
  { L_, "111011101110111",  FW_01,     0,     1,                 g(FW_10)|1 },
  { L_, "111011101110111",  FW_01,     0, BPW-1,  g("10..0111011101110111") },
  { L_, "111011101110111",  FW_01,     0,   BPW,       g("111011101110111") },
  { L_, "111011101110111",  FW_01,     1,     0,                   g(FW_01) },
  { L_, "111011101110111",  FW_01,     1,     1,                 g(FW_10)|1 },
  { L_, "111011101110111",  FW_01,     1, BPW-1,      g("1110111011101111") },
  { L_, "111011101110111",  FW_01, BPW-1,     0,                   g(FW_01) },
  { L_, "111011101110111",  FW_01, BPW-1,     1,           g(FW_01)|INT_MIN },
  { L_, "111011101110111",  FW_01,   BPW,     0,                   g(FW_01) },

  { L_, "111011101110111",  FW_10,     0,     0,                   g(FW_10) },
  { L_, "111011101110111",  FW_10,     0,     1,                   g(FW_01) },
  { L_, "111011101110111",  FW_10,     0, BPW-1,       g("111011101110111") },
  { L_, "111011101110111",  FW_10,     0,   BPW,       g("111011101110111") },
  { L_, "111011101110111",  FW_10,     1,     0,                   g(FW_10) },
  { L_, "111011101110111",  FW_10,     1,     1,                 g(FW_01)+1 },
  { L_, "111011101110111",  FW_10,     1, BPW-1,      g("1110111011101110") },
  { L_, "111011101110111",  FW_10, BPW-1,     0,                   g(FW_10) },
  { L_, "111011101110111",  FW_10, BPW-1,     1,                   g(FW_10) },
  { L_, "111011101110111",  FW_10,   BPW,     0,                   g(FW_10) },

  { L_, "111011101110111",   DSTA,     2,     4,  g("000011011011001011100")},
  { L_, "111011101110111",   DSTA,     2,     5,  g("000110110110011011100")},
  { L_, "111011101110111",   DSTA,     2,     6,  g("001101101100111011100")},
  { L_, "111011101110111",   DSTA,     2,     7,  g("011011011001111011100")},
  { L_, "111011101110111",   DSTA,     2,     8,  g("110110110010111011100")},
  { L_, "111011101110111",   DSTA,     3,     4,  g("000011011011000111100")},
  { L_, "111011101110111",   DSTA,     3,     5,  g("000110110110010111100")},
  { L_, "111011101110111",   DSTA,     3,     6,  g("001101101100110111100")},
  { L_, "111011101110111",   DSTA,     3,     7,  g("011011011001110111100")},
  { L_, "111011101110111",   DSTA,     3,     8,  g("110110110001110111100")}
  //^------v
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;
            int val        = g(DATA_A[di].d_value);
            int nBits      = DATA_A[di].d_nBits;
            int expected   = DATA_A[di].d_insertValue;
            int index      = DATA_A[di].d_index;

            int src = g(spec);
            int dst = src;
            Util::insertValue(&dst, index, val, nBits);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(expected); P_(index); P_(val);
                P_(nBits); P(dst);
            }

            LOOP_ASSERT(LINE, expected == dst);
        }

        if (verbose) cout << "Testing insertValue64 Function" << endl
                          << "==============================" << endl;

        static const struct {
            int         d_lineNum;         // line number
            const char *d_value;           // source value
            const char *d_dst;             // destination integer
            int         d_index;           // destination index
            int         d_nBits;           // number of bits to be inserted
            Int64       d_insertValue;     // expected result from insertValue
        } DATA_B[] = {
           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,  "0..0",  "0..0",     0,     0,                          0 },
           { L_,  "0..0",  "0..0",     0,     1,                          0 },
           { L_,  "0..0",  "0..0",     0, BPS-1,                          0 },
           { L_,  "0..0",  "0..0",     0,   BPS,                          0 },
           { L_,  "0..0",  "0..0",     1,     0,                          0 },
           { L_,  "0..0",  "0..0",     1,     1,                          0 },
           { L_,  "0..0",  "0..0",     1, BPS-1,                          0 },
           { L_,  "0..0",  "0..0", BPS-1,     0,                          0 },
           { L_,  "0..0",  "0..0", BPS-1,     1,                          0 },
           { L_,  "0..0",  "0..0",   BPS,     0,                          0 },

           { L_,  "0..0",  "1..1",     0,     0,                         ~0 },
           { L_,  "0..0",  "1..1",     0,     1,                         ~1 },
           { L_,  "0..0",  "1..1",     0, BPS-1,                   int64Min },
           { L_,  "0..0",  "1..1",     0,   BPS,                          0 },
           { L_,  "0..0",  "1..1",     1,     0,                         ~0 },
           { L_,  "0..0",  "1..1",     1,     1,                         ~2 },
           { L_,  "0..0",  "1..1",     1, BPS-1,                          1 },
           { L_,  "0..0",  "1..1", BPS-1,     0,                         ~0 },
           { L_,  "0..0",  "1..1", BPS-1,     1,                   int64Max },
           { L_,  "0..0",  "1..1",   BPS,     0,                         ~0 },

           { L_,  "0..0",   SW_01,     0,     0,                 g64(SW_01) },
           { L_,  "0..0",   SW_01,     0,     1,                 g64(SW_10) },
           { L_,  "0..0",   SW_01,     0, BPS-1,                   int64Min },
           { L_,  "0..0",   SW_01,     0,   BPS,                          0 },
           { L_,  "0..0",   SW_01,     1,     0,                 g64(SW_01) },
           { L_,  "0..0",   SW_01,     1,     1,               g64(SW_10)^3 },
           { L_,  "0..0",   SW_01,     1, BPS-1,                          1 },
           { L_,  "0..0",   SW_01, BPS-1,     0,                 g64(SW_01) },
           { L_,  "0..0",   SW_01, BPS-1,     1,                 g64(SW_01) },
           { L_,  "0..0",   SW_01,   BPS,     0,                 g64(SW_01) },

           { L_,  "0..0",   SW_10,     0,     0,                 g64(SW_10) },
           { L_,  "0..0",   SW_10,     0,     1,               g64(SW_01)^1 },
           { L_,  "0..0",   SW_10,     0, BPS-1,                          0 },
           { L_,  "0..0",   SW_10,     0,   BPS,                          0 },
           { L_,  "0..0",   SW_10,     1,     0,                 g64(SW_10) },
           { L_,  "0..0",   SW_10,     1,     1,               g64(SW_01)^1 },
           { L_,  "0..0",   SW_10,     1, BPS-1,                          0 },
           { L_,  "0..0",   SW_10, BPS-1,     0,                 g64(SW_10) },
           { L_,  "0..0",   SW_10, BPS-1,     1,        g64(SW_10)^int64Min },
           { L_,  "0..0",   SW_10,   BPS,     0,                 g64(SW_10) },

           { L_,  "0..0",    DSTA,     2,     4,g64("000011011011001000000")},
           { L_,  "0..0",    DSTA,     2,     5,g64("000110110110010000000")},
           { L_,  "0..0",    DSTA,     2,     6,g64("001101101100100000000")},
           { L_,  "0..0",    DSTA,     2,     7,g64("011011011001000000000")},
           { L_,  "0..0",    DSTA,     2,     8,g64("110110110010000000000")},
           { L_,  "0..0",    DSTA,     3,     4,g64("000011011011000000100")},
           { L_,  "0..0",    DSTA,     3,     5,g64("000110110110000000100")},
           { L_,  "0..0",    DSTA,     3,     6,g64("001101101100000000100")},
           { L_,  "0..0",    DSTA,     3,     7,g64("011011011000000000100")},
           { L_,  "0..0",    DSTA,     3,     8,g64("110110110000000000100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,  "1..1",  "0..0",     0,     0,                          0 },
           { L_,  "1..1",  "0..0",     0,     1,                          1 },
           { L_,  "1..1",  "0..0",     0, BPS-1,                   int64Max },
           { L_,  "1..1",  "0..0",     0,   BPS,                         ~0 },
           { L_,  "1..1",  "0..0",     1,     0,                          0 },
           { L_,  "1..1",  "0..0",     1,     1,                          2 },
           { L_,  "1..1",  "0..0",     1, BPS-1,                         ~1 },
           { L_,  "1..1",  "0..0", BPS-1,     0,                          0 },
           { L_,  "1..1",  "0..0", BPS-1,     1,                   int64Min },
           { L_,  "1..1",  "0..0",   BPS,     0,                          0 },

           { L_,  "1..1",  "1..1",     0,     0,                         ~0 },
           { L_,  "1..1",  "1..1",     0,     1,                         ~0 },
           { L_,  "1..1",  "1..1",     0, BPS-1,                         ~0 },
           { L_,  "1..1",  "1..1",     0,   BPS,                         ~0 },
           { L_,  "1..1",  "1..1",     1,     0,                         ~0 },
           { L_,  "1..1",  "1..1",     1,     1,                         ~0 },
           { L_,  "1..1",  "1..1",     1, BPS-1,                         ~0 },
           { L_,  "1..1",  "1..1", BPS-1,     0,                         ~0 },
           { L_,  "1..1",  "1..1", BPS-1,     1,                         ~0 },
           { L_,  "1..1",  "1..1",   BPS,     0,                         ~0 },

           { L_,  "1..1",   SW_01,     0,     0,                 g64(SW_01) },
           { L_,  "1..1",   SW_01,     0,     1,               g64(SW_10)|1 },
           { L_,  "1..1",   SW_01,     0, BPS-1,                         ~0 },
           { L_,  "1..1",   SW_01,     0,   BPS,                         ~0 },
           { L_,  "1..1",   SW_01,     1,     0,                 g64(SW_01) },
           { L_,  "1..1",   SW_01,     1,     1,               g64(SW_10)|1 },
           { L_,  "1..1",   SW_01,     1, BPS-1,                         ~0 },
           { L_,  "1..1",   SW_01, BPS-1,     0,                 g64(SW_01) },
           { L_,  "1..1",   SW_01, BPS-1,     1,        g64(SW_01)|int64Min },
           { L_,  "1..1",   SW_01,   BPS,     0,                 g64(SW_01) },

           { L_,  "1..1",   SW_10,     0,     0,                 g64(SW_10) },
           { L_,  "1..1",   SW_10,     0,     1,                 g64(SW_01) },
           { L_,  "1..1",   SW_10,     0, BPS-1,                   int64Max },
           { L_,  "1..1",   SW_10,     0,   BPS,                         ~0 },
           { L_,  "1..1",   SW_10,     1,     0,                 g64(SW_10) },
           { L_,  "1..1",   SW_10,     1,     1,               g64(SW_01)+1 },
           { L_,  "1..1",   SW_10,     1, BPS-1,                         ~1 },
           { L_,  "1..1",   SW_10, BPS-1,     0,                 g64(SW_10) },
           { L_,  "1..1",   SW_10, BPS-1,     1,                 g64(SW_10) },
           { L_,  "1..1",   SW_10,   BPS,     0,                 g64(SW_10) },

           { L_,  "1..1",    DSTA,     2,     4,g64("000011011011001111100")},
           { L_,  "1..1",    DSTA,     2,     5,g64("000110110110011111100")},
           { L_,  "1..1",    DSTA,     2,     6,g64("001101101100111111100")},
           { L_,  "1..1",    DSTA,     2,     7,g64("011011011001111111100")},
           { L_,  "1..1",    DSTA,     2,     8,g64("110110110011111111100")},
           { L_,  "1..1",    DSTA,     3,     4,g64("000011011011001111100")},
           { L_,  "1..1",    DSTA,     3,     5,g64("000110110110011111100")},
           { L_,  "1..1",    DSTA,     3,     6,g64("001101101100111111100")},
           { L_,  "1..1",    DSTA,     3,     7,g64("011011011001111111100")},
           { L_,  "1..1",    DSTA,     3,     8,g64("110110110011111111100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----          ------------------
           { L_,   SW_01,  "0..0",     0,     0,                          0 },
           { L_,   SW_01,  "0..0",     0,     1,                          1 },
           { L_,   SW_01,  "0..0",     0, BPS-1,                 g64(SW_01) },
           { L_,   SW_01,  "0..0",     0,   BPS,                 g64(SW_01) },
           { L_,   SW_01,  "0..0",     1,     0,                          0 },
           { L_,   SW_01,  "0..0",     1,     1,                          2 },
           { L_,   SW_01,  "0..0",     1, BPS-1,                 g64(SW_10) },
           { L_,   SW_01,  "0..0", BPS-1,     0,                          0 },
           { L_,   SW_01,  "0..0", BPS-1,     1,                   int64Min },
           { L_,   SW_01,  "0..0",   BPS,     0,                          0 },

           { L_,   SW_01,  "1..1",     0,     0,                         ~0 },
           { L_,   SW_01,  "1..1",     0,     1,                         ~0 },
           { L_,   SW_01,  "1..1",     0, BPS-1,        g64(SW_01)|int64Min },
           { L_,   SW_01,  "1..1",     0,   BPS,                 g64(SW_01) },
           { L_,   SW_01,  "1..1",     1,     0,                         ~0 },
           { L_,   SW_01,  "1..1",     1,     1,                         ~0 },
           { L_,   SW_01,  "1..1",     1, BPS-1,               g64(SW_10)|1 },
           { L_,   SW_01,  "1..1", BPS-1,     0,                         ~0 },
           { L_,   SW_01,  "1..1", BPS-1,     1,                         ~0 },
           { L_,   SW_01,  "1..1",   BPS,     0,                         ~0 },

           { L_,   SW_01,   SW_01,     0,     0,                 g64(SW_01) },
           { L_,   SW_01,   SW_01,     0,     1,               g64(SW_10)|1 },
           { L_,   SW_01,   SW_01,     0, BPS-1,        g64(SW_01)|int64Min },
           { L_,   SW_01,   SW_01,     0,   BPS,                 g64(SW_01) },
           { L_,   SW_01,   SW_01,     1,     0,                 g64(SW_01) },
           { L_,   SW_01,   SW_01,     1,     1,               g64(SW_10)|1 },
           { L_,   SW_01,   SW_01,     1, BPS-1,               g64(SW_10)|1 },
           { L_,   SW_01,   SW_01, BPS-1,     0,                 g64(SW_01) },
           { L_,   SW_01,   SW_01, BPS-1,     1,        g64(SW_01)|int64Min },
           { L_,   SW_01,   SW_01,   BPS,     0,                 g64(SW_01) },

           { L_,   SW_01,   SW_10,     0,     0,                 g64(SW_10) },
           { L_,   SW_01,   SW_10,     0,     1,                 g64(SW_01) },
           { L_,   SW_01,   SW_10,     0, BPS-1,                 g64(SW_01) },
           { L_,   SW_01,   SW_10,     0,   BPS,                 g64(SW_01) },
           { L_,   SW_01,   SW_10,     1,     0,                 g64(SW_10) },
           { L_,   SW_01,   SW_10,     1,     1,               g64(SW_01)+1 },
           { L_,   SW_01,   SW_10,     1, BPS-1,                 g64(SW_10) },
           { L_,   SW_01,   SW_10, BPS-1,     0,                 g64(SW_10) },
           { L_,   SW_01,   SW_10, BPS-1,     1,                 g64(SW_10) },
           { L_,   SW_01,   SW_10,   BPS,     0,                 g64(SW_10) },

           { L_,   SW_01,    DSTA,     2,     4,g64("000011011011001010100")},
           { L_,   SW_01,    DSTA,     2,     5,g64("000110110110011010100")},
           { L_,   SW_01,    DSTA,     2,     6,g64("001101101100101010100")},
           { L_,   SW_01,    DSTA,     2,     7,g64("011011011001101010100")},
           { L_,   SW_01,    DSTA,     2,     8,g64("110110110010101010100")},
           { L_,   SW_01,    DSTA,     3,     4,g64("000011011011000101100")},
           { L_,   SW_01,    DSTA,     3,     5,g64("000110110110010101100")},
           { L_,   SW_01,    DSTA,     3,     6,g64("001101101100010101100")},
           { L_,   SW_01,    DSTA,     3,     7,g64("011011011001010101100")},
           { L_,   SW_01,    DSTA,     3,     8,g64("110110110001010101100")},

           //L#    value      dst  Index  nBits          insertValue result
           //--   ------   ------  -----  -----         -------------------
           { L_,   SW_10,  "0..0",     0,     0,                          0 },
           { L_,   SW_10,  "0..0",     0,     1,                          0 },
           { L_,   SW_10,  "0..0",     0, BPS-1,        g64(SW_10)^int64Min },
           { L_,   SW_10,  "0..0",     0,   BPS,                 g64(SW_10) },
           { L_,   SW_10,  "0..0",     1,     0,                          0 },
           { L_,   SW_10,  "0..0",     1,     1,                          0 },
           { L_,   SW_10,  "0..0",     1, BPS-1,               g64(SW_01)^1 },
           { L_,   SW_10,  "0..0", BPS-1,     0,                          0 },
           { L_,   SW_10,  "0..0", BPS-1,     1,                          0 },
           { L_,   SW_10,  "0..0",   BPS,     0,                          0 },

           { L_,   SW_10,  "1..1",     0,     0,                         ~0 },
           { L_,   SW_10,  "1..1",     0,     1,                         ~1 },
           { L_,   SW_10,  "1..1",     0, BPS-1,                 g64(SW_10) },
           { L_,   SW_10,  "1..1",     0,   BPS,                 g64(SW_10) },
           { L_,   SW_10,  "1..1",     1,     0,                         ~0 },
           { L_,   SW_10,  "1..1",     1,     1,                         ~2 },
           { L_,   SW_10,  "1..1",     1, BPS-1,                 g64(SW_01) },
           { L_,   SW_10,  "1..1", BPS-1,     0,                         ~0 },
           { L_,   SW_10,  "1..1", BPS-1,     1,                   int64Max },
           { L_,   SW_10,  "1..1",   BPS,     0,                         ~0 },

           { L_,   SW_10,   SW_01,     0,     0,                 g64(SW_01) },
           { L_,   SW_10,   SW_01,     0,     1,                 g64(SW_10) },
           { L_,   SW_10,   SW_01,     0, BPS-1,                 g64(SW_10) },
           { L_,   SW_10,   SW_01,     0,   BPS,                 g64(SW_10) },
           { L_,   SW_10,   SW_01,     1,     0,                 g64(SW_01) },
           { L_,   SW_10,   SW_01,     1,     1,               g64(SW_10)^3 },
           { L_,   SW_10,   SW_01,     1, BPS-1,                 g64(SW_01) },
           { L_,   SW_10,   SW_01, BPS-1,     0,                 g64(SW_01) },
           { L_,   SW_10,   SW_01, BPS-1,     1,                 g64(SW_01) },
           { L_,   SW_10,   SW_01,   BPS,     0,                 g64(SW_01) },

           { L_,   SW_10,   SW_10,     0,     0,                 g64(SW_10) },
           { L_,   SW_10,   SW_10,     0,     1,               g64(SW_01)^1 },
           { L_,   SW_10,   SW_10,     0, BPS-1,        g64(SW_10)^int64Min },
           { L_,   SW_10,   SW_10,     0,   BPS,                 g64(SW_10) },
           { L_,   SW_10,   SW_10,     1,     0,                 g64(SW_10) },
           { L_,   SW_10,   SW_10,     1,     1,               g64(SW_01)^1 },
           { L_,   SW_10,   SW_10,     1, BPS-1,               g64(SW_01)^1 },
           { L_,   SW_10,   SW_10, BPS-1,     0,                 g64(SW_10) },
           { L_,   SW_10,   SW_10, BPS-1,     1,        g64(SW_10)^int64Min },
           { L_,   SW_10,   SW_10,   BPS,     0,                 g64(SW_10) },

           { L_,   SW_10,    DSTA,     2,     4,g64("000011011011001101000")},
           { L_,   SW_10,    DSTA,     2,     5,g64("000110110110010101000")},
           { L_,   SW_10,    DSTA,     2,     6,g64("001101101100110101000")},
           { L_,   SW_10,    DSTA,     2,     7,g64("011011011001010101000")},
           { L_,   SW_10,    DSTA,     2,     8,g64("110110110011010101000")},
           { L_,   SW_10,    DSTA,     3,     4,g64("000011011011001010100")},
           { L_,   SW_10,    DSTA,     3,     5,g64("000110110110001010100")},
           { L_,   SW_10,    DSTA,     3,     6,g64("001101101100101010100")},
           { L_,   SW_10,    DSTA,     3,     7,g64("011011011000101010100")},
           { L_,   SW_10,    DSTA,     3,     8,g64("110110110010101010100")},

  //v------^
  //L#         value          dst  Index  nBits          insertValue result
  //--   ---------------   ------  -----  -----         -------------------
  { L_, "111011101110111", "0..0",     0,     0,                          0 },
  { L_, "111011101110111", "0..0",     0,     1,                          1 },
  { L_, "111011101110111", "0..0",     0, BPS-1,     g64("111011101110111") },
  { L_, "111011101110111", "0..0",     0,   BPS,     g64("111011101110111") },
  { L_, "111011101110111", "0..0",     1,     0,                          0 },
  { L_, "111011101110111", "0..0",     1,     1,                          2 },
  { L_, "111011101110111", "0..0",     1, BPS-1,    g64("1110111011101110") },
  { L_, "111011101110111", "0..0", BPS-1,     0,                          0 },
  { L_, "111011101110111", "0..0", BPS-1,     1,                   int64Min },
  { L_, "111011101110111", "0..0",   BPS,     0,                          0 },

  { L_, "111011101110111", "1..1",     0,     0,                         ~0 },
  { L_, "111011101110111", "1..1",     0,     1,                         ~0 },
  { L_, "111011101110111", "1..1",     0, BPS-1,g64("10..0111011101110111") },
  { L_, "111011101110111", "1..1",     0,   BPS,g64("00..0111011101110111") },
  { L_, "111011101110111", "1..1",     1,     0,                         ~0 },
  { L_, "111011101110111", "1..1",     1,     1,                         ~0 },
  { L_, "111011101110111", "1..1",     1, BPS-1,    g64("1110111011101111") },
  { L_, "111011101110111", "1..1", BPS-1,     0,                         ~0 },
  { L_, "111011101110111", "1..1", BPS-1,     1,                         ~0 },
  { L_, "111011101110111", "1..1",   BPS,     0,                         ~0 },

  { L_, "111011101110111",  SW_01,     0,     0,                 g64(SW_01) },
  { L_, "111011101110111",  SW_01,     0,     1,               g64(SW_10)|1 },
  { L_, "111011101110111",  SW_01,     0, BPS-1,g64("10..0111011101110111") },
  { L_, "111011101110111",  SW_01,     0,   BPS,     g64("111011101110111") },
  { L_, "111011101110111",  SW_01,     1,     0,                 g64(SW_01) },
  { L_, "111011101110111",  SW_01,     1,     1,               g64(SW_10)|1 },
  { L_, "111011101110111",  SW_01,     1, BPS-1,    g64("1110111011101111") },
  { L_, "111011101110111",  SW_01, BPS-1,     0,                 g64(SW_01) },
  { L_, "111011101110111",  SW_01, BPS-1,     1,        g64(SW_01)|int64Min },
  { L_, "111011101110111",  SW_01,   BPS,     0,                 g64(SW_01) },

  { L_, "111011101110111",  SW_10,     0,     0,                 g64(SW_10) },
  { L_, "111011101110111",  SW_10,     0,     1,                 g64(SW_01) },
  { L_, "111011101110111",  SW_10,     0, BPS-1,     g64("111011101110111") },
  { L_, "111011101110111",  SW_10,     0,   BPS,     g64("111011101110111") },
  { L_, "111011101110111",  SW_10,     1,     0,                 g64(SW_10) },
  { L_, "111011101110111",  SW_10,     1,     1,               g64(SW_01)+1 },
  { L_, "111011101110111",  SW_10,     1, BPS-1,    g64("1110111011101110") },
  { L_, "111011101110111",  SW_10, BPS-1,     0,                 g64(SW_10) },
  { L_, "111011101110111",  SW_10, BPS-1,     1,                 g64(SW_10) },
  { L_, "111011101110111",  SW_10,   BPS,     0,                 g64(SW_10) },

  { L_, "111011101110111",   DSTA,     2,     4,g64("000011011011001011100")},
  { L_, "111011101110111",   DSTA,     2,     5,g64("000110110110011011100")},
  { L_, "111011101110111",   DSTA,     2,     6,g64("001101101100111011100")},
  { L_, "111011101110111",   DSTA,     2,     7,g64("011011011001111011100")},
  { L_, "111011101110111",   DSTA,     2,     8,g64("110110110010111011100")},
  { L_, "111011101110111",   DSTA,     3,     4,g64("000011011011000111100")},
  { L_, "111011101110111",   DSTA,     3,     5,g64("000110110110010111100")},
  { L_, "111011101110111",   DSTA,     3,     6,g64("001101101100110111100")},
  { L_, "111011101110111",   DSTA,     3,     7,g64("011011011001110111100")},
  { L_, "111011101110111",   DSTA,     3,     8,g64("110110110001110111100")}
  //^------v
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            Int64 src      = g64(DATA_B[di].d_dst);
            Int64 val      = g64(DATA_B[di].d_value);
            int nBits      = DATA_B[di].d_nBits;
            Int64 expected = DATA_B[di].d_insertValue;
            int index      = DATA_B[di].d_index;

            Int64 dst = src;
            Util::insertValue64(&dst, index, val, nBits);

            if (veryVerbose) {
                P_(LINE); P_(DATA_B[di].d_dst); P_(expected); P_(index);
                P_(val); P_(nBits); P(dst);
            }

            LOOP_ASSERT(LINE, expected == dst);
        }
}

static
void testE24()
{
        // --------------------------------------------------------------------
        // TESTING NUMSET0 AND NUMSET1 FUNCTIONS:
        //   'numSet0' and 'numSet1' are implemented as look-up tables, and so
        //   must be tested exhaustively, in a loop-based test that uses
        //   logically-independent calculations to verify the results.
        //
        // Plan:
        //   For each of a sequence of individual test grouped by source
        //   category, verify that the numSet0 and numSet1 functions
        //   each return the expected value.
        //
        // Testing:
        //   int numSetZero(  int sInt);
        //   int numSetZero64(bsls_Types::Int64 sInt);
        //   int numSetOne(   int sInt);
        //   int numSetOne64( bsls_Types::Int64 sInt);
        // --------------------------------------------------------------------

        const int BYTE_MAX = 255;      // maximum unsigned byte value
        const int BYTES_PER_WORD  = sizeof (int);
        const int BYTES_PER_INT64 = sizeof (Int64);

        if (verbose) cout << endl
            << "Testing numSetZero and numSetOne Functions" << endl
            << "==========================================" << endl;

        for (int di = 0; di <= BYTE_MAX; ++di) {
            int numSet1 = Util::isSetOne(di, 0)
                        + Util::isSetOne(di, 1)
                        + Util::isSetOne(di, 2)
                        + Util::isSetOne(di, 3)
                        + Util::isSetOne(di, 4)
                        + Util::isSetOne(di, 5)
                        + Util::isSetOne(di, 6)
                        + Util::isSetOne(di, 7);

            for (int posA = 0; posA < BYTES_PER_WORD; ++posA){
                for (int posB = 0; posB < BYTES_PER_WORD; ++posB){
                    int src = di << posA * CHAR_BIT;
                    src |=    di << posB * CHAR_BIT;

                    int totalNumSet1 = numSet1;
                    if (posA != posB) {
                        totalNumSet1 *= 2;
                    }
                    int totalNumSet0 =
                                  CHAR_BIT * BYTES_PER_WORD;
                    totalNumSet0 -= totalNumSet1;

                    int resA = Util::numSetZero(src);

                    if (veryVerbose) {
                        P_(src); P_(posA); P_(posB); P_(totalNumSet0);
                        P_(resA);
                    }

                    LOOP3_ASSERT(src, posA, posB, totalNumSet0 == resA);

                    resA = Util::numSetOne(src);

                    if (veryVerbose) {
                        P_(src); P_(posA); P_(posB); P_(totalNumSet1);
                        P_(resA);
                    }

                    LOOP3_ASSERT(src, posA, posB, totalNumSet1 == resA);
                }
            }
        }

        if (verbose) cout << endl
            << "Testing numSetZero64 and numSetOne64 Functions" << endl
            << "==============================================" << endl;

        for (Int64 di = 0; di <= BYTE_MAX; ++di) {
            int numSet1 = Util::isSetOne(di, 0)
                        + Util::isSetOne(di, 1)
                        + Util::isSetOne(di, 2)
                        + Util::isSetOne(di, 3)
                        + Util::isSetOne(di, 4)
                        + Util::isSetOne(di, 5)
                        + Util::isSetOne(di, 6)
                        + Util::isSetOne(di, 7);

            for (int posA = 0; posA < BYTES_PER_INT64; ++posA){
                for (int posB = 0; posB < BYTES_PER_INT64; ++posB){
                    Int64 src = di << posA * CHAR_BIT;
                    src |=      di << posB * CHAR_BIT;

                    int totalNumSet1 = numSet1;
                    if (posA != posB) {
                        totalNumSet1 *= 2;
                    }
                    int totalNumSet0 = CHAR_BIT
                                                             * BYTES_PER_INT64;
                    totalNumSet0 -= totalNumSet1;

                    int resA = Util::numSetZero64(src);

                    if (veryVerbose) {
                        P_(src); P_(posA); P_(posB); P_(totalNumSet0);
                        P_(resA);
                    }

                    LOOP6_ASSERT(di, src, posA, posB, totalNumSet0, resA,
                                                         totalNumSet0 == resA);

                    resA = Util::numSetOne64(src);

                    if (veryVerbose) {
                        P_(src); P_(posA); P_(posB); P_(totalNumSet1);
                        P_(resA);
                    }

                    LOOP6_ASSERT(di, src, posA, posB, totalNumSet1, resA,
                                                         totalNumSet1 == resA);
                }
            }
        }
}

static
void testE23()
{
        // --------------------------------------------------------------------
        // TESTING ISSETZERO AND ISSETONE FUNCTIONS:
        //   The 'setValue' and 'replaceValue' methods have very simple
        //   implementations that perform calculations using already-tested
        //   methods.  A relatively small set of test data is sufficient.
        //
        // Plan:
        //   For each of a sequence of individual test grouped by source
        //   category, verify that the isSet0 and isSet1 functions
        //   each return the expected value.
        //
        // Testing:
        //   bool isSetZero(int sInt, int sIdx);
        //   bool isSetZero64(bsls_Types::Int64 sInt, int sIdx);
        //   bool isSetOne(int sInt, int sIdx);
        //   bool isSetOne64(bsls_Types::Int64 sInt, int sIdx);
        //   bool isSet0(int sInt, int sIdx);
        //   bool isSet1(int sInt, int sIdx);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing isSetZero and isSetOne Functions" << endl
            << "========================================" << endl;

        static const struct {
            int         d_lineNum;  // line number
            const char *d_src;      // source integer
            int         d_index;    // source index
            int         d_isSet0;   // expected result from isSet0 function
            int         d_isSet1;   // expected result from isSet1 function
        } DATA_A[] = {
           //L#         src   index   isSet0 isSet1
           //--   ---------   -----   ------ ------
           { L_,     "0..0",     0,       1,      0 },  // no position
           { L_,     "0..0",     1,       1,      0 },
           { L_,     "0..0", BPW-2,       1,      0 },
           { L_,     "0..0", BPW-1,       1,      0 },
           { L_,     "1..1",     0,       0,      1 },
           { L_,     "1..1",     1,       0,      1 },
           { L_,     "1..1", BPW-2,       0,      1 },
           { L_,     "1..1", BPW-1,       0,      1 },

           { L_,    "0..01",     0,       0,      1 },  // boundary position
           { L_,   "0..010",     1,       0,      1 },
           { L_,   "010..0", BPW-2,       0,      1 },
           { L_,    "10..0", BPW-1,       0,      1 },
           { L_,    "1..10",     0,       1,      0 },
           { L_,   "1..101",     1,       1,      0 },
           { L_,   "101..1", BPW-2,       1,      0 },
           { L_,    "01..1", BPW-1,       1,      0 },

           { L_,  "0..0100",     2,       0,      1 },  // interior position
           { L_, "1..10111",     3,       1,      0 }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::isSetZero(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_isSet0); P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_isSet0 == resA);

            resA = Util::isSetOne(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_isSet1); P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_isSet1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing isSet0, isSet1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::isSet0(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_isSet0); P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_isSet0 == resA);

            resA = Util::isSet1(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_isSet1); P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_isSet1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing isSetZero64 and isSetOne64 Functions" << endl
            << "============================================" << endl;

        static const struct {
            int         d_lineNum;  // line number
            const char *d_src;      // source integer
            int         d_index;    // source index
            Int64       d_isSet0;   // expected result from isSet0 function
            Int64       d_isSet1;   // expected result from isSet1 function
        } DATA_B[] = {
           //L#         src   index   isSet0 isSet1
           //--   ---------   -----   ------ ------
           { L_,     "0..0",     0,       1,      0 },  // no position
           { L_,     "0..0",     1,       1,      0 },
           { L_,     "0..0", BPS-2,       1,      0 },
           { L_,     "0..0", BPS-1,       1,      0 },
           { L_,     "1..1",     0,       0,      1 },
           { L_,     "1..1",     1,       0,      1 },
           { L_,     "1..1", BPS-2,       0,      1 },
           { L_,     "1..1", BPS-1,       0,      1 },

           { L_,    "0..01",     0,       0,      1 },  // boundary position
           { L_,   "0..010",     1,       0,      1 },
           { L_,   "010..0", BPS-2,       0,      1 },
           { L_,    "10..0", BPS-1,       0,      1 },
           { L_,    "1..10",     0,       1,      0 },
           { L_,   "1..101",     1,       1,      0 },
           { L_,   "101..1", BPS-2,       1,      0 },
           { L_,    "01..1", BPS-1,       1,      0 },

           { L_,  "0..0100",     2,       0,      1 },  // interior position
           { L_, "1..10111",     3,       1,      0 }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_src;

            Int64 src = g64(spec);
            int res = Util::isSetZero64(src, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_isSet0); P(res);
            }

            LOOP4_ASSERT(LINE, spec, DATA_B[di].d_isSet0, res,
                                                   DATA_B[di].d_isSet0 == res);

            res = Util::isSetOne64(src, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_isSet1); P(res);
            }

            LOOP4_ASSERT(LINE, spec, DATA_B[di].d_isSet1, res,
                                                   DATA_B[di].d_isSet1 == res);
        }
}

static
void testE22()
{
        // --------------------------------------------------------------------
        // TESTING SETVALUE AND REPLACEVALUE FUNCTION:
        //   The 'setValue' and 'replaceValue' methods have relatively simple
        //   implementations that perform calculations using already-tested
        //   methods.  A relatively small set of test data is sufficient, but
        //   we choose a more thorough and systematic set of test data to
        //   completely probe the algorithm and not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by input
        //   value and then source category, verify that the 'setValue' and
        //   'replaceValue' functions each give the expected value.  Note that
        //   the "set" function returns its result while the "replace"
        //   function loads its result into a specified integer.
        //
        // Testing:
        //   int setValue(int sInt, int sIdx, int value, int nBits);
        //   int setValue64(bsls_Types::Int64 sInt,
        //                  int sIdx,
        //                  bsls_Types::Int64 value,
        //                  int nBits);
        //   void replaceValue(int *dInt, int dIdx, int sInt, int nBits);
        //   void replaceValue64(bsls_Types::Int64 *dInt,
        //                       int dIdx,
        //                       bsls_Types::Int64 sInt,
        //                       int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing setValue & replaceValue Functions\n"
                          << "=========================================\n";

        const char *DSTA = "1101101100100";    // typical case
        const char *VALA = "111011101110111";  // typical case

        static const struct {
            int         d_lineNum;  // line number
            const char *d_value;    // value
            const char *d_dst;      // destination integer
            int         d_index;    // destination index
            int         d_numBits;  // num of bits
            int         d_res;      // expected result (setValue/replaceValue)
        } DATA_A[] = {
            // L#   value      dst  Index  nBits                    result
            // --  ------   ------  -----  -----               -----------
            { L_,  "0..0",  "0..0",     0,     0,                        0 },
            { L_,  "0..0",  "0..0",     0,     1,                        0 },
            { L_,  "0..0",  "0..0",     0, BPW-1,                        0 },
            { L_,  "0..0",  "0..0",     0,   BPW,                        0 },
            { L_,  "0..0",  "0..0",     1,     0,                        0 },
            { L_,  "0..0",  "0..0",     1,     1,                        0 },
            { L_,  "0..0",  "0..0",     1, BPW-1,                        0 },
            { L_,  "0..0",  "0..0", BPW-1,     0,                        0 },
            { L_,  "0..0",  "0..0", BPW-1,     1,                        0 },
            { L_,  "0..0",  "0..0",   BPW,     0,                        0 },

            { L_,  "0..0",  "1..1",     0,     0,                       ~0 },
            { L_,  "0..0",  "1..1",     0,     1,                       ~1 },
            { L_,  "0..0",  "1..1",     0, BPW-1,                  INT_MIN },
            { L_,  "0..0",  "1..1",     0,   BPW,                        0 },
            { L_,  "0..0",  "1..1",     1,     0,                       ~0 },
            { L_,  "0..0",  "1..1",     1,     1,                       ~2 },
            { L_,  "0..0",  "1..1",     1, BPW-1,                        1 },
            { L_,  "0..0",  "1..1", BPW-1,     0,                       ~0 },
            { L_,  "0..0",  "1..1", BPW-1,     1,                  INT_MAX },
            { L_,  "0..0",  "1..1",   BPW,     0,                       ~0 },

            { L_,  "0..0",   FW_01,     0,     0,                 g(FW_01) },
            { L_,  "0..0",   FW_01,     0,     1,               g(FW_01)^1 },
            { L_,  "0..0",   FW_01,     0, BPW-1,                        0 },
            { L_,  "0..0",   FW_01,     0,   BPW,                        0 },
            { L_,  "0..0",   FW_01,     1,     0,                 g(FW_01) },
            { L_,  "0..0",   FW_01,     1,     1,                 g(FW_01) },
            { L_,  "0..0",   FW_01,     1, BPW-1,                        1 },
            { L_,  "0..0",   FW_01, BPW-1,     0,                 g(FW_01) },
            { L_,  "0..0",   FW_01, BPW-1,     1,                 g(FW_01) },
            { L_,  "0..0",   FW_01,   BPW,     0,                 g(FW_01) },

            { L_,  "0..0",   FW_10,     0,     0,                 g(FW_10) },
            { L_,  "0..0",   FW_10,     0,     1,                 g(FW_10) },
            { L_,  "0..0",   FW_10,     0, BPW-1,                  INT_MIN },
            { L_,  "0..0",   FW_10,     0,   BPW,                        0 },
            { L_,  "0..0",   FW_10,     1,     0,                 g(FW_10) },
            { L_,  "0..0",   FW_10,     1,     1,               g(FW_10)^2 },
            { L_,  "0..0",   FW_10,     1, BPW-1,                        0 },
            { L_,  "0..0",   FW_10, BPW-1,     0,                 g(FW_10) },
            { L_,  "0..0",   FW_10, BPW-1,     1,         g(FW_10)&INT_MAX },
            { L_,  "0..0",   FW_10,   BPW,     0,                 g(FW_10) },

            // DSTA                                     g("1101101100100")
            { L_,  "0..0",    DSTA,     2,     4,       g("1101101000000") },
            { L_,  "0..0",    DSTA,     2,     5,       g("1101100000000") },
            { L_,  "0..0",    DSTA,     2,     6,       g("1101100000000") },
            { L_,  "0..0",    DSTA,     2,     7,       g("1101000000000") },
            { L_,  "0..0",    DSTA,     2,     8,       g("1100000000000") },
            { L_,  "0..0",    DSTA,     3,     4,       g("1101100000100") },
            { L_,  "0..0",    DSTA,     3,     5,       g("1101100000100") },
            { L_,  "0..0",    DSTA,     3,     6,       g("1101000000100") },
            { L_,  "0..0",    DSTA,     3,     7,       g("1100000000100") },
            { L_,  "0..0",    DSTA,     3,     8,       g("1100000000100") },

            { L_,  "1..1",  "0..0",     0,     0,                        0 },
            { L_,  "1..1",  "0..0",     0,     1,                        1 },
            { L_,  "1..1",  "0..0",     0, BPW-1,                  INT_MAX },
            { L_,  "1..1",  "0..0",     0,   BPW,                       ~0 },
            { L_,  "1..1",  "0..0",     1,     0,                        0 },
            { L_,  "1..1",  "0..0",     1,     1,                        2 },
            { L_,  "1..1",  "0..0",     1, BPW-1,                       ~1 },
            { L_,  "1..1",  "0..0", BPW-1,     0,                        0 },
            { L_,  "1..1",  "0..0", BPW-1,     1,                  INT_MIN },
            { L_,  "1..1",  "0..0",   BPW,     0,                        0 },

            { L_,  "1..1",  "1..1",     0,     0,                       ~0 },
            { L_,  "1..1",  "1..1",     0,     1,                       ~0 },
            { L_,  "1..1",  "1..1",     0, BPW-1,                       ~0 },
            { L_,  "1..1",  "1..1",     0,   BPW,                       ~0 },
            { L_,  "1..1",  "1..1",     1,     0,                       ~0 },
            { L_,  "1..1",  "1..1",     1,     1,                       ~0 },
            { L_,  "1..1",  "1..1",     1, BPW-1,                       ~0 },
            { L_,  "1..1",  "1..1", BPW-1,     0,                       ~0 },
            { L_,  "1..1",  "1..1", BPW-1,     1,                       ~0 },
            { L_,  "1..1",  "1..1",   BPW,     0,                       ~0 },

            { L_,  "1..1",   FW_01,     0,     0,                 g(FW_01) },
            { L_,  "1..1",   FW_01,     0,     1,                 g(FW_01) },
            { L_,  "1..1",   FW_01,     0, BPW-1,                  INT_MAX },
            { L_,  "1..1",   FW_01,     0,   BPW,                       ~0 },
            { L_,  "1..1",   FW_01,     1,     0,                 g(FW_01) },
            { L_,  "1..1",   FW_01,     1,     1,               g(FW_01)|2 },
            { L_,  "1..1",   FW_01,     1, BPW-1,                       ~0 },
            { L_,  "1..1",   FW_01, BPW-1,     0,                 g(FW_01) },
            { L_,  "1..1",   FW_01, BPW-1,     1,         g(FW_01)|INT_MIN },
            { L_,  "1..1",   FW_01,   BPW,     0,                 g(FW_01) },

            { L_,  "1..1",   FW_10,     0,     0,                 g(FW_10) },
            { L_,  "1..1",   FW_10,     0,     1,               g(FW_10)|1 },
            { L_,  "1..1",   FW_10,     0, BPW-1,                       ~0 },
            { L_,  "1..1",   FW_10,     0,   BPW,                       ~0 },
            { L_,  "1..1",   FW_10,     1,     0,                 g(FW_10) },
            { L_,  "1..1",   FW_10,     1,     1,                 g(FW_10) },
            { L_,  "1..1",   FW_10,     1, BPW-1,                       ~1 },
            { L_,  "1..1",   FW_10, BPW-1,     0,                 g(FW_10) },
            { L_,  "1..1",   FW_10, BPW-1,     1,                 g(FW_10) },
            { L_,  "1..1",   FW_10,   BPW,     0,                 g(FW_10) },

            // DSTA                                     g("1101101100100")
            { L_,  "1..1",    DSTA,     2,     4,       g("1101101111100") },
            { L_,  "1..1",    DSTA,     2,     5,       g("1101101111100") },
            { L_,  "1..1",    DSTA,     2,     6,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     2,     7,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     2,     8,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     4,       g("1101101111100") },
            { L_,  "1..1",    DSTA,     3,     5,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     6,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     7,       g("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     8,       g("1111111111100") },

            { L_,   FW_01,  "0..0",     0,     0,                        0 },
            { L_,   FW_01,  "0..0",     0,     1,                        1 },
            { L_,   FW_01,  "0..0",     0, BPW-1,                 g(FW_01) },
            { L_,   FW_01,  "0..0",     0,   BPW,                 g(FW_01) },
            { L_,   FW_01,  "0..0",     1,     0,                        0 },
            { L_,   FW_01,  "0..0",     1,     1,                        2 },
            { L_,   FW_01,  "0..0",     1, BPW-1,                 g(FW_10) },
            { L_,   FW_01,  "0..0", BPW-1,     0,                        0 },
            { L_,   FW_01,  "0..0", BPW-1,     1,                  INT_MIN },
            { L_,   FW_01,  "0..0",   BPW,     0,                        0 },

            { L_,   FW_01,  "1..1",     0,     0,                       ~0 },
            { L_,   FW_01,  "1..1",     0,     1,                       ~0 },
            { L_,   FW_01,  "1..1",     0, BPW-1,         g(FW_01)|INT_MIN },
            { L_,   FW_01,  "1..1",     0,   BPW,                 g(FW_01) },
            { L_,   FW_01,  "1..1",     1,     0,                       ~0 },
            { L_,   FW_01,  "1..1",     1,     1,                       ~0 },
            { L_,   FW_01,  "1..1",     1, BPW-1,               g(FW_10)|1 },
            { L_,   FW_01,  "1..1", BPW-1,     0,                       ~0 },
            { L_,   FW_01,  "1..1", BPW-1,     1,                       ~0 },
            { L_,   FW_01,  "1..1",   BPW,     0,                       ~0 },

            { L_,   FW_01,   FW_01,     0,     0,                 g(FW_01) },
            { L_,   FW_01,   FW_01,     0,     1,                 g(FW_01) },
            { L_,   FW_01,   FW_01,     0, BPW-1,                 g(FW_01) },
            { L_,   FW_01,   FW_01,     0,   BPW,                 g(FW_01) },
            { L_,   FW_01,   FW_01,     1,     0,                 g(FW_01) },
            { L_,   FW_01,   FW_01,     1,     1,               g(FW_01)|2 },
            { L_,   FW_01,   FW_01,     1, BPW-1,               g(FW_10)|1 },
            { L_,   FW_01,   FW_01, BPW-1,     0,                 g(FW_01) },
            { L_,   FW_01,   FW_01, BPW-1,     1,         g(FW_01)|INT_MIN },
            { L_,   FW_01,   FW_01,   BPW,     0,                 g(FW_01) },

            { L_,   FW_01,   FW_10,     0,     0,                 g(FW_10) },
            { L_,   FW_01,   FW_10,     0,     1,               g(FW_10)|1 },
            { L_,   FW_01,   FW_10,     0, BPW-1,         g(FW_01)|INT_MIN },
            { L_,   FW_01,   FW_10,     0,   BPW,                 g(FW_01) },
            { L_,   FW_01,   FW_10,     1,     0,                 g(FW_10) },
            { L_,   FW_01,   FW_10,     1,     1,                 g(FW_10) },
            { L_,   FW_01,   FW_10,     1, BPW-1,                 g(FW_10) },
            { L_,   FW_01,   FW_10, BPW-1,     0,                 g(FW_10) },
            { L_,   FW_01,   FW_10, BPW-1,     1,                 g(FW_10) },
            { L_,   FW_01,   FW_10,   BPW,     0,                 g(FW_10) },

            // DSTA                                     g("1101101100100")
            { L_,   FW_01,    DSTA,     2,     4,       g("1101101010100") },
            { L_,   FW_01,    DSTA,     2,     5,       g("1101101010100") },
            { L_,   FW_01,    DSTA,     2,     6,       g("1101101010100") },
            { L_,   FW_01,    DSTA,     2,     7,       g("1101101010100") },
            { L_,   FW_01,    DSTA,     2,     8,       g("1100101010100") },
            { L_,   FW_01,    DSTA,     3,     4,       g("1101100101100") },
            { L_,   FW_01,    DSTA,     3,     5,       g("1101110101100") },
            { L_,   FW_01,    DSTA,     3,     6,       g("1101010101100") },
            { L_,   FW_01,    DSTA,     3,     7,       g("1101010101100") },
            { L_,   FW_01,    DSTA,     3,     8,       g("1101010101100") },

            { L_,   FW_10,  "0..0",     0,     0,                        0 },
            { L_,   FW_10,  "0..0",     0,     1,                        0 },
            { L_,   FW_10,  "0..0",     0, BPW-1,         g(FW_10)^INT_MIN },
            { L_,   FW_10,  "0..0",     0,   BPW,                 g(FW_10) },
            { L_,   FW_10,  "0..0",     1,     0,                        0 },
            { L_,   FW_10,  "0..0",     1,     1,                        0 },
            { L_,   FW_10,  "0..0",     1, BPW-1,               g(FW_01)^1 },
            { L_,   FW_10,  "0..0", BPW-1,     0,                        0 },
            { L_,   FW_10,  "0..0", BPW-1,     1,                        0 },
            { L_,   FW_10,  "0..0",   BPW,     0,                        0 },

            { L_,   FW_10,  "1..1",     0,     0,                       ~0 },
            { L_,   FW_10,  "1..1",     0,     1,                       ~1 },
            { L_,   FW_10,  "1..1",     0, BPW-1,                 g(FW_10) },
            { L_,   FW_10,  "1..1",     0,   BPW,                 g(FW_10) },
            { L_,   FW_10,  "1..1",     1,     0,                       ~0 },
            { L_,   FW_10,  "1..1",     1,     1,                       ~2 },
            { L_,   FW_10,  "1..1",     1, BPW-1,                 g(FW_01) },
            { L_,   FW_10,  "1..1", BPW-1,     0,                       ~0 },
            { L_,   FW_10,  "1..1", BPW-1,     1,                  INT_MAX },
            { L_,   FW_10,  "1..1",   BPW,     0,                       ~0 },

            { L_,   FW_10,   FW_01,     0,     0,                 g(FW_01) },
            { L_,   FW_10,   FW_01,     0,     1,               g(FW_01)^1 },
            { L_,   FW_10,   FW_01,     0, BPW-1,         g(FW_10)^INT_MIN },
            { L_,   FW_10,   FW_01,     0,   BPW,                 g(FW_10) },
            { L_,   FW_10,   FW_01,     1,     0,                 g(FW_01) },
            { L_,   FW_10,   FW_01,     1,     1,                 g(FW_01) },
            { L_,   FW_10,   FW_01,     1, BPW-1,                 g(FW_01) },
            { L_,   FW_10,   FW_01, BPW-1,     0,                 g(FW_01) },
            { L_,   FW_10,   FW_01, BPW-1,     1,                 g(FW_01) },
            { L_,   FW_10,   FW_01,   BPW,     0,                 g(FW_01) },

            { L_,   FW_10,   FW_10,     0,     0,                 g(FW_10) },
            { L_,   FW_10,   FW_10,     0,     1,                 g(FW_10) },
            { L_,   FW_10,   FW_10,     0, BPW-1,                 g(FW_10) },
            { L_,   FW_10,   FW_10,     0,   BPW,                 g(FW_10) },
            { L_,   FW_10,   FW_10,     1,     0,                 g(FW_10) },
            { L_,   FW_10,   FW_10,     1,     1,               g(FW_10)^2 },
            { L_,   FW_10,   FW_10,     1, BPW-1,               g(FW_01)^1 },
            { L_,   FW_10,   FW_10, BPW-1,     0,                 g(FW_10) },
            { L_,   FW_10,   FW_10, BPW-1,     1,         g(FW_10)^INT_MIN },
            { L_,   FW_10,   FW_10,   BPW,     0,                 g(FW_10) },

            // DSTA                                     g("1101101100100")
            { L_,   FW_10,    DSTA,     2,     4,       g("1101101101000") },
            { L_,   FW_10,    DSTA,     2,     5,       g("1101100101000") },
            { L_,   FW_10,    DSTA,     2,     6,       g("1101110101000") },
            { L_,   FW_10,    DSTA,     2,     7,       g("1101010101000") },
            { L_,   FW_10,    DSTA,     2,     8,       g("1101010101000") },
            { L_,   FW_10,    DSTA,     3,     4,       g("1101101010100") },
            { L_,   FW_10,    DSTA,     3,     5,       g("1101101010100") },
            { L_,   FW_10,    DSTA,     3,     6,       g("1101101010100") },
            { L_,   FW_10,    DSTA,     3,     7,       g("1100101010100") },
            { L_,   FW_10,    DSTA,     3,     8,       g("1110101010100") },

            // VALA    =                             g("0111011101110111")
            { L_,    VALA,  "0..0",     0,     0,                        0 },
            { L_,    VALA,  "0..0",     0,     1,                        1 },
            { L_,    VALA,  "0..0",     0, BPW-1,                  g(VALA) },
            { L_,    VALA,  "0..0",     0,   BPW,                  g(VALA) },
            { L_,    VALA,  "0..0",     1,     0,                        0 },
            { L_,    VALA,  "0..0",     1,     1,                        2 },
            { L_,    VALA,  "0..0",     1, BPW-1,    g("1110111011101110") },
            { L_,    VALA,  "0..0", BPW-1,     0,                        0 },
            { L_,    VALA,  "0..0", BPW-1,     1,                  INT_MIN },
            { L_,    VALA,  "0..0",   BPW,     0,                        0 },

            { L_,    VALA,  "1..1",     0,     0,                       ~0 },
            { L_,    VALA,  "1..1",     0,     1,                       ~0 },
            { L_,    VALA,  "1..1",     0, BPW-1,g("10..0111011101110111") },
            { L_,    VALA,  "1..1",     0,   BPW,                  g(VALA) },
            { L_,    VALA,  "1..1",     1,     0,                       ~0 },
            { L_,    VALA,  "1..1",     1,     1,                       ~0 },
            { L_,    VALA,  "1..1",     1, BPW-1,    g("1110111011101111") },
            { L_,    VALA,  "1..1", BPW-1,     0,                       ~0 },
            { L_,    VALA,  "1..1", BPW-1,     1,                       ~0 },
            { L_,    VALA,  "1..1",   BPW,     0,                       ~0 },

            { L_,    VALA,   FW_01,     0,     0,                 g(FW_01) },
            { L_,    VALA,   FW_01,     0,     1,                 g(FW_01) },
            { L_,    VALA,   FW_01,     0, BPW-1,                  g(VALA) },
            { L_,    VALA,   FW_01,     0,   BPW,                  g(VALA) },
            { L_,    VALA,   FW_01,     1,     0,                 g(FW_01) },
            { L_,    VALA,   FW_01,     1,     1,               g(FW_01)|2 },
            { L_,    VALA,   FW_01,     1, BPW-1,    g("1110111011101111") },
            { L_,    VALA,   FW_01, BPW-1,     0,                 g(FW_01) },
            { L_,    VALA,   FW_01, BPW-1,     1,         g(FW_01)|INT_MIN },
            { L_,    VALA,   FW_01,   BPW,     0,                 g(FW_01) },

            { L_,    VALA,   FW_10,     0,     0,                 g(FW_10) },
            { L_,    VALA,   FW_10,     0,     1,               g(FW_10)|1 },
            { L_,    VALA,   FW_10,     0, BPW-1,g("10..0111011101110111") },
            { L_,    VALA,   FW_10,     0,   BPW,                  g(VALA) },
            { L_,    VALA,   FW_10,     1,     0,                 g(FW_10) },
            { L_,    VALA,   FW_10,     1,     1,                 g(FW_10) },
            { L_,    VALA,   FW_10,     1, BPW-1,    g("1110111011101110") },
            { L_,    VALA,   FW_10, BPW-1,     0,                 g(FW_10) },
            { L_,    VALA,   FW_10, BPW-1,     1,                 g(FW_10) },
            { L_,    VALA,   FW_10,   BPW,     0,                 g(FW_10) },

            // VALA    =                             g("0111011101110111")
            // DSTA    =                             g("0001101101100100")
            { L_,    VALA,    DSTA,     2,     4,    g("0001101101011100") },
            { L_,    VALA,    DSTA,     2,     5,    g("0001101101011100") },
            { L_,    VALA,    DSTA,     2,     6,    g("0001101111011100") },
            { L_,    VALA,    DSTA,     2,     7,    g("0001101111011100") },
            { L_,    VALA,    DSTA,     2,     8,    g("0001100111011100") },
            { L_,    VALA,    DSTA,     3,     4,    g("0001101100111100") },
            { L_,    VALA,    DSTA,     3,     5,    g("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     6,    g("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     7,    g("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     8,    g("0001101110111100") },
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;
            const char *value = DATA_A[di].d_value;

            int dst = g(spec);
            int val = g(value);
            int res = Util::setValue(dst, DATA_A[di].d_index, val,
                                             DATA_A[di].d_numBits);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res); P(res);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_res == res);

            Util::replaceValue(&dst, DATA_A[di].d_index, val,
                                       DATA_A[di].d_numBits);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res); P(dst);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_res == dst);

        }

        if (verbose) cout << endl
                          << "Testing setValue64 & replaceValue64 Functions\n"
                          << "=============================================\n";

        static const struct {
            int         d_lineNum;  // line number
            const char *d_value;    // value
            const char *d_dst;      // destination integer
            int         d_index;    // destination index
            int         d_numBits;  // num of bits
            Int64       d_res;      // expected result (setValue/replaceValue)
        } DATA_B[] = {
            // L#   value      dst  Index  nBits                    result
            // --  ------   ------  -----  -----               -----------
            { L_,  "0..0",  "0..0",     0,     0,                        0 },
            { L_,  "0..0",  "0..0",     0,     1,                        0 },
            { L_,  "0..0",  "0..0",     0, BPS-1,                        0 },
            { L_,  "0..0",  "0..0",     0,   BPS,                        0 },
            { L_,  "0..0",  "0..0",     1,     0,                        0 },
            { L_,  "0..0",  "0..0",     1,     1,                        0 },
            { L_,  "0..0",  "0..0",     1, BPS-1,                        0 },
            { L_,  "0..0",  "0..0", BPS-1,     0,                        0 },
            { L_,  "0..0",  "0..0", BPS-1,     1,                        0 },
            { L_,  "0..0",  "0..0",   BPS,     0,                        0 },

            { L_,  "0..0",  "1..1",     0,     0,                       ~0 },
            { L_,  "0..0",  "1..1",     0,     1,                       ~1 },
            { L_,  "0..0",  "1..1",     0, BPS-1,                 int64Min },
            { L_,  "0..0",  "1..1",     0,   BPS,                        0 },
            { L_,  "0..0",  "1..1",     1,     0,                       ~0 },
            { L_,  "0..0",  "1..1",     1,     1,                       ~2 },
            { L_,  "0..0",  "1..1",     1, BPS-1,                        1 },
            { L_,  "0..0",  "1..1", BPS-1,     0,                       ~0 },
            { L_,  "0..0",  "1..1", BPS-1,     1,                 int64Max },
            { L_,  "0..0",  "1..1",   BPS,     0,                       ~0 },

            { L_,  "0..0",   SW_01,     0,     0,               g64(SW_01) },
            { L_,  "0..0",   SW_01,     0,     1,             g64(SW_01)^1 },
            { L_,  "0..0",   SW_01,     0, BPS-1,                        0 },
            { L_,  "0..0",   SW_01,     0,   BPS,                        0 },
            { L_,  "0..0",   SW_01,     1,     0,               g64(SW_01) },
            { L_,  "0..0",   SW_01,     1,     1,               g64(SW_01) },
            { L_,  "0..0",   SW_01,     1, BPS-1,                        1 },
            { L_,  "0..0",   SW_01, BPS-1,     0,               g64(SW_01) },
            { L_,  "0..0",   SW_01, BPS-1,     1,               g64(SW_01) },
            { L_,  "0..0",   SW_01,   BPS,     0,               g64(SW_01) },

            { L_,  "0..0",   SW_10,     0,     0,               g64(SW_10) },
            { L_,  "0..0",   SW_10,     0,     1,               g64(SW_10) },
            { L_,  "0..0",   SW_10,     0, BPS-1,                 int64Min },
            { L_,  "0..0",   SW_10,     0,   BPS,                        0 },
            { L_,  "0..0",   SW_10,     1,     0,               g64(SW_10) },
            { L_,  "0..0",   SW_10,     1,     1,             g64(SW_10)^2 },
            { L_,  "0..0",   SW_10,     1, BPS-1,                        0 },
            { L_,  "0..0",   SW_10, BPS-1,     0,               g64(SW_10) },
            { L_,  "0..0",   SW_10, BPS-1,     1,      g64(SW_10)&int64Max },
            { L_,  "0..0",   SW_10,   BPS,     0,               g64(SW_10) },

            // DSTA                                   g64("1101101100100")
            { L_,  "0..0",    DSTA,     2,     4,     g64("1101101000000") },
            { L_,  "0..0",    DSTA,     2,     5,     g64("1101100000000") },
            { L_,  "0..0",    DSTA,     2,     6,     g64("1101100000000") },
            { L_,  "0..0",    DSTA,     2,     7,     g64("1101000000000") },
            { L_,  "0..0",    DSTA,     2,     8,     g64("1100000000000") },
            { L_,  "0..0",    DSTA,     3,     4,     g64("1101100000100") },
            { L_,  "0..0",    DSTA,     3,     5,     g64("1101100000100") },
            { L_,  "0..0",    DSTA,     3,     6,     g64("1101000000100") },
            { L_,  "0..0",    DSTA,     3,     7,     g64("1100000000100") },
            { L_,  "0..0",    DSTA,     3,     8,     g64("1100000000100") },

            { L_,  "1..1",  "0..0",     0,     0,                        0 },
            { L_,  "1..1",  "0..0",     0,     1,                        1 },
            { L_,  "1..1",  "0..0",     0, BPS-1,                 int64Max },
            { L_,  "1..1",  "0..0",     0,   BPS,                       ~0 },
            { L_,  "1..1",  "0..0",     1,     0,                        0 },
            { L_,  "1..1",  "0..0",     1,     1,                        2 },
            { L_,  "1..1",  "0..0",     1, BPS-1,                       ~1 },
            { L_,  "1..1",  "0..0", BPS-1,     0,                        0 },
            { L_,  "1..1",  "0..0", BPS-1,     1,                 int64Min },
            { L_,  "1..1",  "0..0",   BPS,     0,                        0 },

            { L_,  "1..1",  "1..1",     0,     0,                       ~0 },
            { L_,  "1..1",  "1..1",     0,     1,                       ~0 },
            { L_,  "1..1",  "1..1",     0, BPS-1,                       ~0 },
            { L_,  "1..1",  "1..1",     0,   BPS,                       ~0 },
            { L_,  "1..1",  "1..1",     1,     0,                       ~0 },
            { L_,  "1..1",  "1..1",     1,     1,                       ~0 },
            { L_,  "1..1",  "1..1",     1, BPS-1,                       ~0 },
            { L_,  "1..1",  "1..1", BPS-1,     0,                       ~0 },
            { L_,  "1..1",  "1..1", BPS-1,     1,                       ~0 },
            { L_,  "1..1",  "1..1",   BPS,     0,                       ~0 },

            { L_,  "1..1",   SW_01,     0,     0,               g64(SW_01) },
            { L_,  "1..1",   SW_01,     0,     1,               g64(SW_01) },
            { L_,  "1..1",   SW_01,     0, BPS-1,                 int64Max },
            { L_,  "1..1",   SW_01,     0,   BPS,                       ~0 },
            { L_,  "1..1",   SW_01,     1,     0,               g64(SW_01) },
            { L_,  "1..1",   SW_01,     1,     1,             g64(SW_01)|2 },
            { L_,  "1..1",   SW_01,     1, BPS-1,                       ~0 },
            { L_,  "1..1",   SW_01, BPS-1,     0,               g64(SW_01) },
            { L_,  "1..1",   SW_01, BPS-1,     1,      g64(SW_01)|int64Min },
            { L_,  "1..1",   SW_01,   BPS,     0,               g64(SW_01) },

            { L_,  "1..1",   SW_10,     0,     0,               g64(SW_10) },
            { L_,  "1..1",   SW_10,     0,     1,             g64(SW_10)|1 },
            { L_,  "1..1",   SW_10,     0, BPS-1,                       ~0 },
            { L_,  "1..1",   SW_10,     0,   BPS,                       ~0 },
            { L_,  "1..1",   SW_10,     1,     0,               g64(SW_10) },
            { L_,  "1..1",   SW_10,     1,     1,               g64(SW_10) },
            { L_,  "1..1",   SW_10,     1, BPS-1,                       ~1 },
            { L_,  "1..1",   SW_10, BPS-1,     0,               g64(SW_10) },
            { L_,  "1..1",   SW_10, BPS-1,     1,               g64(SW_10) },
            { L_,  "1..1",   SW_10,   BPS,     0,               g64(SW_10) },

            // DSTA                                   g64("1101101100100")
            { L_,  "1..1",    DSTA,     2,     4,     g64("1101101111100") },
            { L_,  "1..1",    DSTA,     2,     5,     g64("1101101111100") },
            { L_,  "1..1",    DSTA,     2,     6,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     2,     7,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     2,     8,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     4,     g64("1101101111100") },
            { L_,  "1..1",    DSTA,     3,     5,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     6,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     7,     g64("1101111111100") },
            { L_,  "1..1",    DSTA,     3,     8,     g64("1111111111100") },

            { L_,   SW_01,  "0..0",     0,     0,                        0 },
            { L_,   SW_01,  "0..0",     0,     1,                        1 },
            { L_,   SW_01,  "0..0",     0, BPS-1,               g64(SW_01) },
            { L_,   SW_01,  "0..0",     0,   BPS,               g64(SW_01) },
            { L_,   SW_01,  "0..0",     1,     0,                        0 },
            { L_,   SW_01,  "0..0",     1,     1,                        2 },
            { L_,   SW_01,  "0..0",     1, BPS-1,               g64(SW_10) },
            { L_,   SW_01,  "0..0", BPS-1,     0,                        0 },
            { L_,   SW_01,  "0..0", BPS-1,     1,                 int64Min },
            { L_,   SW_01,  "0..0",   BPS,     0,                        0 },

            { L_,   SW_01,  "1..1",     0,     0,                       ~0 },
            { L_,   SW_01,  "1..1",     0,     1,                       ~0 },
            { L_,   SW_01,  "1..1",     0, BPS-1,      g64(SW_01)|int64Min },
            { L_,   SW_01,  "1..1",     0,   BPS,               g64(SW_01) },
            { L_,   SW_01,  "1..1",     1,     0,                       ~0 },
            { L_,   SW_01,  "1..1",     1,     1,                       ~0 },
            { L_,   SW_01,  "1..1",     1, BPS-1,             g64(SW_10)|1 },
            { L_,   SW_01,  "1..1", BPS-1,     0,                       ~0 },
            { L_,   SW_01,  "1..1", BPS-1,     1,                       ~0 },
            { L_,   SW_01,  "1..1",   BPS,     0,                       ~0 },

            { L_,   SW_01,   SW_01,     0,     0,               g64(SW_01) },
            { L_,   SW_01,   SW_01,     0,     1,               g64(SW_01) },
            { L_,   SW_01,   SW_01,     0, BPS-1,               g64(SW_01) },
            { L_,   SW_01,   SW_01,     0,   BPS,               g64(SW_01) },
            { L_,   SW_01,   SW_01,     1,     0,               g64(SW_01) },
            { L_,   SW_01,   SW_01,     1,     1,             g64(SW_01)|2 },
            { L_,   SW_01,   SW_01,     1, BPS-1,             g64(SW_10)|1 },
            { L_,   SW_01,   SW_01, BPS-1,     0,               g64(SW_01) },
            { L_,   SW_01,   SW_01, BPS-1,     1,      g64(SW_01)|int64Min },
            { L_,   SW_01,   SW_01,   BPS,     0,               g64(SW_01) },

            { L_,   SW_01,   SW_10,     0,     0,               g64(SW_10) },
            { L_,   SW_01,   SW_10,     0,     1,             g64(SW_10)|1 },
            { L_,   SW_01,   SW_10,     0, BPS-1,      g64(SW_01)|int64Min },
            { L_,   SW_01,   SW_10,     0,   BPS,               g64(SW_01) },
            { L_,   SW_01,   SW_10,     1,     0,               g64(SW_10) },
            { L_,   SW_01,   SW_10,     1,     1,               g64(SW_10) },
            { L_,   SW_01,   SW_10,     1, BPS-1,               g64(SW_10) },
            { L_,   SW_01,   SW_10, BPS-1,     0,               g64(SW_10) },
            { L_,   SW_01,   SW_10, BPS-1,     1,               g64(SW_10) },
            { L_,   SW_01,   SW_10,   BPS,     0,               g64(SW_10) },

            // DSTA                                   g64("1101101100100")
            { L_,   SW_01,    DSTA,     2,     4,     g64("1101101010100") },
            { L_,   SW_01,    DSTA,     2,     5,     g64("1101101010100") },
            { L_,   SW_01,    DSTA,     2,     6,     g64("1101101010100") },
            { L_,   SW_01,    DSTA,     2,     7,     g64("1101101010100") },
            { L_,   SW_01,    DSTA,     2,     8,     g64("1100101010100") },
            { L_,   SW_01,    DSTA,     3,     4,     g64("1101100101100") },
            { L_,   SW_01,    DSTA,     3,     5,     g64("1101110101100") },
            { L_,   SW_01,    DSTA,     3,     6,     g64("1101010101100") },
            { L_,   SW_01,    DSTA,     3,     7,     g64("1101010101100") },
            { L_,   SW_01,    DSTA,     3,     8,     g64("1101010101100") },

            { L_,   SW_10,  "0..0",     0,     0,                        0 },
            { L_,   SW_10,  "0..0",     0,     1,                        0 },
            { L_,   SW_10,  "0..0",     0, BPS-1,      g64(SW_10)^int64Min },
            { L_,   SW_10,  "0..0",     0,   BPS,               g64(SW_10) },
            { L_,   SW_10,  "0..0",     1,     0,                        0 },
            { L_,   SW_10,  "0..0",     1,     1,                        0 },
            { L_,   SW_10,  "0..0",     1, BPS-1,             g64(SW_01)^1 },
            { L_,   SW_10,  "0..0", BPS-1,     0,                        0 },
            { L_,   SW_10,  "0..0", BPS-1,     1,                        0 },
            { L_,   SW_10,  "0..0",   BPS,     0,                        0 },

            { L_,   SW_10,  "1..1",     0,     0,                       ~0 },
            { L_,   SW_10,  "1..1",     0,     1,                       ~1 },
            { L_,   SW_10,  "1..1",     0, BPS-1,               g64(SW_10) },
            { L_,   SW_10,  "1..1",     0,   BPS,               g64(SW_10) },
            { L_,   SW_10,  "1..1",     1,     0,                       ~0 },
            { L_,   SW_10,  "1..1",     1,     1,                       ~2 },
            { L_,   SW_10,  "1..1",     1, BPS-1,               g64(SW_01) },
            { L_,   SW_10,  "1..1", BPS-1,     0,                       ~0 },
            { L_,   SW_10,  "1..1", BPS-1,     1,                 int64Max },
            { L_,   SW_10,  "1..1",   BPS,     0,                       ~0 },

            { L_,   SW_10,   SW_01,     0,     0,               g64(SW_01) },
            { L_,   SW_10,   SW_01,     0,     1,             g64(SW_01)^1 },
            { L_,   SW_10,   SW_01,     0, BPS-1,      g64(SW_10)^int64Min },
            { L_,   SW_10,   SW_01,     0,   BPS,               g64(SW_10) },
            { L_,   SW_10,   SW_01,     1,     0,               g64(SW_01) },
            { L_,   SW_10,   SW_01,     1,     1,               g64(SW_01) },
            { L_,   SW_10,   SW_01,     1, BPS-1,               g64(SW_01) },
            { L_,   SW_10,   SW_01, BPS-1,     0,               g64(SW_01) },
            { L_,   SW_10,   SW_01, BPS-1,     1,               g64(SW_01) },
            { L_,   SW_10,   SW_01,   BPS,     0,               g64(SW_01) },

            { L_,   SW_10,   SW_10,     0,     0,               g64(SW_10) },
            { L_,   SW_10,   SW_10,     0,     1,               g64(SW_10) },
            { L_,   SW_10,   SW_10,     0, BPS-1,               g64(SW_10) },
            { L_,   SW_10,   SW_10,     0,   BPS,               g64(SW_10) },
            { L_,   SW_10,   SW_10,     1,     0,               g64(SW_10) },
            { L_,   SW_10,   SW_10,     1,     1,             g64(SW_10)^2 },
            { L_,   SW_10,   SW_10,     1, BPS-1,             g64(SW_01)^1 },
            { L_,   SW_10,   SW_10, BPS-1,     0,               g64(SW_10) },
            { L_,   SW_10,   SW_10, BPS-1,     1,      g64(SW_10)^int64Min },
            { L_,   SW_10,   SW_10,   BPS,     0,               g64(SW_10) },

            // DSTA                                   g64("1101101100100")
            { L_,   SW_10,    DSTA,     2,     4,     g64("1101101101000") },
            { L_,   SW_10,    DSTA,     2,     5,     g64("1101100101000") },
            { L_,   SW_10,    DSTA,     2,     6,     g64("1101110101000") },
            { L_,   SW_10,    DSTA,     2,     7,     g64("1101010101000") },
            { L_,   SW_10,    DSTA,     2,     8,     g64("1101010101000") },
            { L_,   SW_10,    DSTA,     3,     4,     g64("1101101010100") },
            { L_,   SW_10,    DSTA,     3,     5,     g64("1101101010100") },
            { L_,   SW_10,    DSTA,     3,     6,     g64("1101101010100") },
            { L_,   SW_10,    DSTA,     3,     7,     g64("1100101010100") },
            { L_,   SW_10,    DSTA,     3,     8,     g64("1110101010100") },

            // VALA    =                           g64("0111011101110111")
            { L_,    VALA,  "0..0",     0,     0,                        0 },
            { L_,    VALA,  "0..0",     0,     1,                        1 },
            { L_,    VALA,  "0..0",     0, BPS-1,                g64(VALA) },
            { L_,    VALA,  "0..0",     0,   BPS,                g64(VALA) },
            { L_,    VALA,  "0..0",     1,     0,                        0 },
            { L_,    VALA,  "0..0",     1,     1,                        2 },
            { L_,    VALA,  "0..0",     1, BPS-1,  g64("1110111011101110") },
            { L_,    VALA,  "0..0", BPS-1,     0,                        0 },
            { L_,    VALA,  "0..0", BPS-1,     1,                 int64Min },
            { L_,    VALA,  "0..0",   BPS,     0,                        0 },

            { L_,    VALA,  "1..1",     0,     0,                       ~0 },
            { L_,    VALA,  "1..1",     0,     1,                       ~0 },
            { L_,    VALA,  "1..1",     0, BPS-1,g64("10..0111011101110111") },
            { L_,    VALA,  "1..1",     0,   BPS,                g64(VALA) },
            { L_,    VALA,  "1..1",     1,     0,                       ~0 },
            { L_,    VALA,  "1..1",     1,     1,                       ~0 },
            { L_,    VALA,  "1..1",     1, BPS-1,  g64("1110111011101111") },
            { L_,    VALA,  "1..1", BPS-1,     0,                       ~0 },
            { L_,    VALA,  "1..1", BPS-1,     1,                       ~0 },
            { L_,    VALA,  "1..1",   BPS,     0,                       ~0 },

            { L_,    VALA,   SW_01,     0,     0,               g64(SW_01) },
            { L_,    VALA,   SW_01,     0,     1,               g64(SW_01) },
            { L_,    VALA,   SW_01,     0, BPS-1,                g64(VALA) },
            { L_,    VALA,   SW_01,     0,   BPS,                g64(VALA) },
            { L_,    VALA,   SW_01,     1,     0,               g64(SW_01) },
            { L_,    VALA,   SW_01,     1,     1,             g64(SW_01)|2 },
            { L_,    VALA,   SW_01,     1, BPS-1,  g64("1110111011101111") },
            { L_,    VALA,   SW_01, BPS-1,     0,               g64(SW_01) },
            { L_,    VALA,   SW_01, BPS-1,     1,      g64(SW_01)|int64Min },
            { L_,    VALA,   SW_01,   BPS,     0,               g64(SW_01) },

            { L_,    VALA,   SW_10,     0,     0,               g64(SW_10) },
            { L_,    VALA,   SW_10,     0,     1,             g64(SW_10)|1 },
            { L_,    VALA,   SW_10,     0, BPS-1,g64("10..0111011101110111")},
            { L_,    VALA,   SW_10,     0,   BPS,                g64(VALA) },
            { L_,    VALA,   SW_10,     1,     0,               g64(SW_10) },
            { L_,    VALA,   SW_10,     1,     1,               g64(SW_10) },
            { L_,    VALA,   SW_10,     1, BPS-1,  g64("1110111011101110") },
            { L_,    VALA,   SW_10, BPS-1,     0,               g64(SW_10) },
            { L_,    VALA,   SW_10, BPS-1,     1,               g64(SW_10) },
            { L_,    VALA,   SW_10,   BPS,     0,               g64(SW_10) },

            // VALA    =                           g64("0111011101110111")
            // DSTA    =                           g64("0001101101100100")
            { L_,    VALA,    DSTA,     2,     4,  g64("0001101101011100") },
            { L_,    VALA,    DSTA,     2,     5,  g64("0001101101011100") },
            { L_,    VALA,    DSTA,     2,     6,  g64("0001101111011100") },
            { L_,    VALA,    DSTA,     2,     7,  g64("0001101111011100") },
            { L_,    VALA,    DSTA,     2,     8,  g64("0001100111011100") },
            { L_,    VALA,    DSTA,     3,     4,  g64("0001101100111100") },
            { L_,    VALA,    DSTA,     3,     5,  g64("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     6,  g64("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     7,  g64("0001101110111100") },
            { L_,    VALA,    DSTA,     3,     8,  g64("0001101110111100") },
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE    = DATA_B[di].d_lineNum;
            const char *spec  = DATA_B[di].d_dst;
            const char *value = DATA_B[di].d_value;

            Int64 dst = g64(spec);
            Int64 val = g64(value);
            Int64 res = Util::setValue64(dst, DATA_B[di].d_index, val,
                                                 DATA_B[di].d_numBits);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res); P(res);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_res == res);

            Util::replaceValue64(&dst, DATA_B[di].d_index, val,
                                         DATA_B[di].d_numBits);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res); P(dst);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_res == dst);
        }
}

static
void testE21()
{
        // --------------------------------------------------------------------
        // TESTING INSERTVALUEUPTO FUNCTION:
        //   'insertValueUpTo' has a moderately simple implementations that
        //   perform calculations using already-tested methods.  A relatively
        //   small set of test data is sufficient.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by value and
        //   then destination category, verify that the 'insertValueUpTo'
        //   function gives the expected value.
        //
        // Testing:
        //   void insertValueUpTo(int *dInt, int nBits, int sInt);
        //   void insertValueUpTo(bsls_Types::Int64 *dInt,
        //                        int nBits,
        //                        bsls_Types::Int64 sInt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing insertValueUpTo Function" << endl
                          << "================================" << endl;

        const char *DSTA = "1101101100100"; // typical case
        static const struct {
            int         d_lineNum;         // line number
            int         d_value;           // source value
            const char *d_dst;             // destination integer
            int         d_nBits;           // number of bits to be inserted
            int         d_insertValueUpTo; // expected result (insertValueUpTo)
        } DATA_A[] = {
            //L#     value    dst    nBits      insertValueUpTo result
            //--     -----   ------  -----      ----------------------
            { L_, g("0..0"), "0..0",     0,                           0 },
            { L_, g("0..0"), "0..0",     1,                           0 },
            { L_, g("0..0"), "0..0", BPW-1,                           0 },
            { L_, g("0..0"), "0..0",   BPW,                           0 },

            { L_, g("0..0"), "1..1",     0,                          ~0 },
            { L_, g("0..0"), "1..1",     1,                          ~1 },
            { L_, g("0..0"), "1..1", BPW-1,                     INT_MIN },
            { L_, g("0..0"), "1..1",   BPW,                           0 },

            { L_, g("0..0"),  FW_01,     0,                    g(FW_01) },
            { L_, g("0..0"),  FW_01,     1,                    g(FW_10) },
            { L_, g("0..0"),  FW_01, BPW-1,                     INT_MIN },
            { L_, g("0..0"),  FW_01,   BPW,                           0 },

            { L_, g("0..0"),  FW_10,     0,                    g(FW_10) },
            { L_, g("0..0"),  FW_10,     1,                  g(FW_01)^1 },
            { L_, g("0..0"),  FW_10, BPW-1,                           0 },
            { L_, g("0..0"),  FW_10,   BPW,                           0 },

            // DSTA   =                      g("000000001101101100100") },
            { L_, g("0..0"),   DSTA,     4,  g("000011011011001000000") },
            { L_, g("0..0"),   DSTA,     5,  g("000110110110010000000") },
            { L_, g("0..0"),   DSTA,     6,  g("001101101100100000000") },
            { L_, g("0..0"),   DSTA,     7,  g("011011011001000000000") },
            { L_, g("0..0"),   DSTA,     8,  g("110110110010000000000") },

            { L_,  g(FW_01), "0..0",     0,                           0 },
            { L_,  g(FW_01), "0..0",     1,                           1 },
            { L_,  g(FW_01), "0..0", BPW-1,                    g(FW_01) },
            { L_,  g(FW_01), "0..0",   BPW,                    g(FW_01) },

            { L_,  g(FW_01), "1..1",     0,                          ~0 },
            { L_,  g(FW_01), "1..1",     1,                          ~0 },
            { L_,  g(FW_01), "1..1", BPW-1,            g(FW_01)|INT_MIN },
            { L_,  g(FW_01), "1..1",   BPW,                    g(FW_01) },

            { L_,  g(FW_01),  FW_01,     0,                    g(FW_01) },
            { L_,  g(FW_01),  FW_01,     1,                  g(FW_10)|1 },
            { L_,  g(FW_01),  FW_01, BPW-1,            g(FW_01)|INT_MIN },
            { L_,  g(FW_01),  FW_01,   BPW,                    g(FW_01) },

            { L_,  g(FW_01),  FW_10,     0,                    g(FW_10) },
            { L_,  g(FW_01),  FW_10,     1,                    g(FW_01) },
            { L_,  g(FW_01),  FW_10, BPW-1,                    g(FW_01) },
            { L_,  g(FW_01),  FW_10,   BPW,                    g(FW_01) },

            //DSTA    =                      g("000000001101101100100") },
            { L_,  g(FW_01),   DSTA,     4,  g("000011011011001000101") },
            { L_,  g(FW_01),   DSTA,     5,  g("000110110110010010101") },
            { L_,  g(FW_01),   DSTA,     6,  g("001101101100100010101") },
            { L_,  g(FW_01),   DSTA,     7,  g("011011011001001010101") },
            { L_,  g(FW_01),   DSTA,     8,  g("110110110010001010101") },

            { L_, g("1..1"), "0..0",     0,                           0 },
            { L_, g("1..1"), "0..0",     1,                           1 },
            { L_, g("1..1"), "0..0", BPW-1,                     INT_MAX },
            { L_, g("1..1"), "0..0",   BPW,                          ~0 },

            { L_, g("1..1"), "1..1",     0,                          ~0 },
            { L_, g("1..1"), "1..1",     1,                          ~0 },
            { L_, g("1..1"), "1..1", BPW-1,                          ~0 },
            { L_, g("1..1"), "1..1",   BPW,                          ~0 },

            { L_, g("1..1"),  FW_01,     0,                    g(FW_01) },
            { L_, g("1..1"),  FW_01,     1,                  g(FW_10)|1 },
            { L_, g("1..1"),  FW_01, BPW-1,                          ~0 },
            { L_, g("1..1"),  FW_01,   BPW,                          ~0 },

            { L_, g("1..1"),  FW_10,     0,                    g(FW_10) },
            { L_, g("1..1"),  FW_10,     1,                    g(FW_01) },
            { L_, g("1..1"),  FW_10, BPW-1,                     INT_MAX },
            { L_, g("1..1"),  FW_10,   BPW,                          ~0 },

            // DSTA    =                     g("000000001101101100100")
            { L_, g("1..1"),   DSTA,     4,  g("000011011011001001111") },
            { L_, g("1..1"),   DSTA,     5,  g("000110110110010011111") },
            { L_, g("1..1"),   DSTA,     6,  g("001101101100100111111") },
            { L_, g("1..1"),   DSTA,     7,  g("011011011001001111111") },
            { L_, g("1..1"),   DSTA,     8,  g("110110110010011111111") },

            { L_,  g(FW_10), "0..0",     0,                           0 },
            { L_,  g(FW_10), "0..0",     1,                           0 },
            { L_,  g(FW_10), "0..0", BPW-1,            g(FW_10)&INT_MAX },
            { L_,  g(FW_10), "0..0",   BPW,                    g(FW_10) },

            { L_,  g(FW_10), "1..1",     0,                          ~0 },
            { L_,  g(FW_10), "1..1",     1,                          ~1 },
            { L_,  g(FW_10), "1..1", BPW-1,                    g(FW_10) },
            { L_,  g(FW_10), "1..1",   BPW,                    g(FW_10) },

            { L_,  g(FW_10),  FW_01,     0,                    g(FW_01) },
            { L_,  g(FW_10),  FW_01,     1,                    g(FW_10) },
            { L_,  g(FW_10),  FW_01, BPW-1,                    g(FW_10) },
            { L_,  g(FW_10),  FW_01,   BPW,                    g(FW_10) },

            { L_,  g(FW_10),  FW_10,     0,                    g(FW_10) },
            { L_,  g(FW_10),  FW_10,     1,                  g(FW_01)^1 },
            { L_,  g(FW_10),  FW_10, BPW-1,            g(FW_10)&INT_MAX },
            { L_,  g(FW_10),  FW_10,   BPW,                    g(FW_10) },

            //DSTA    =                      g("000000001101101100100")
            { L_,  g(FW_10),   DSTA,     4,  g("000011011011001001010") },
            { L_,  g(FW_10),   DSTA,     5,  g("000110110110010001010") },
            { L_,  g(FW_10),   DSTA,     6,  g("001101101100100101010") },
            { L_,  g(FW_10),   DSTA,     7,  g("011011011001000101010") },
            { L_,  g(FW_10),   DSTA,     8,  g("110110110010010101010") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            Util::insertValueUpTo(&dst, DATA_A[di].d_nBits,
                                          DATA_A[di].d_value);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_insertValueUpTo); P(dst);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_insertValueUpTo == dst);
        }

        if (verbose) cout << endl
                          << "Testing insertValueUpTo64 Function" << endl
                          << "==================================" << endl;

        static const struct {
            int         d_lineNum;         // line number
            const char *d_valueSpec;       // source value spec
            const char *d_dst;             // destination integer
            int         d_nBits;           // number of bits to be inserted
            Int64       d_insertValueUpTo; // expected result (insertValueUpTo)
        } DATA_B[] = {
            //L#     value   dst    nBits      insertValueUpTo result
            //--     -----  ------  -----      ----------------------
            { L_,   "0..0", "0..0",     0,                           0 },
            { L_,   "0..0", "0..0",     1,                           0 },
            { L_,   "0..0", "0..0", BPS-1,                           0 },
            { L_,   "0..0", "0..0",   BPS,                           0 },

            { L_,   "0..0", "1..1",     0,                          ~0 },
            { L_,   "0..0", "1..1",     1,                          ~1 },
            { L_,   "0..0", "1..1", BPS-1,                    int64Min },
            { L_,   "0..0", "1..1",   BPS,                           0 },

            { L_,   "0..0",  SW_01,     0,                  g64(SW_01) },
            { L_,   "0..0",  SW_01,     1,                  g64(SW_10) },
            { L_,   "0..0",  SW_01, BPS-1,                    int64Min },
            { L_,   "0..0",  SW_01,   BPS,                           0 },

            { L_,   "0..0",  SW_10,     0,                  g64(SW_10) },
            { L_,   "0..0",  SW_10,     1,                g64(SW_01)^1 },
            { L_,   "0..0",  SW_10, BPS-1,                           0 },
            { L_,   "0..0",  SW_10,   BPS,                           0 },

            // DSTA   =                   g64("000000001101101100100") },
            { L_,   "0..0",   DSTA,     4,g64("000011011011001000000") },
            { L_,   "0..0",   DSTA,     5,g64("000110110110010000000") },
            { L_,   "0..0",   DSTA,     6,g64("001101101100100000000") },
            { L_,   "0..0",   DSTA,     7,g64("011011011001000000000") },
            { L_,   "0..0",   DSTA,     8,g64("110110110010000000000") },

            { L_,    SW_01, "0..0",     0,                           0 },
            { L_,    SW_01, "0..0",     1,                           1 },
            { L_,    SW_01, "0..0", BPS-1,                  g64(SW_01) },
            { L_,    SW_01, "0..0",   BPS,                  g64(SW_01) },

            { L_,    SW_01, "1..1",     0,                          ~0 },
            { L_,    SW_01, "1..1",     1,                          ~0 },
            { L_,    SW_01, "1..1", BPS-1,         g64(SW_01)|int64Min },
            { L_,    SW_01, "1..1",   BPS,                  g64(SW_01) },

            { L_,    SW_01,  SW_01,     0,                  g64(SW_01) },
            { L_,    SW_01,  SW_01,     1,                g64(SW_10)|1 },
            { L_,    SW_01,  SW_01, BPS-1,         g64(SW_01)|int64Min },
            { L_,    SW_01,  SW_01,   BPS,                  g64(SW_01) },

            { L_,    SW_01,  SW_10,     0,                  g64(SW_10) },
            { L_,    SW_01,  SW_10,     1,                  g64(SW_01) },
            { L_,    SW_01,  SW_10, BPS-1,                  g64(SW_01) },
            { L_,    SW_01,  SW_10,   BPS,                  g64(SW_01) },

            //DSTA    =                   g64("000000001101101100100") },
            { L_,    SW_01,   DSTA,     4,g64("000011011011001000101") },
            { L_,    SW_01,   DSTA,     5,g64("000110110110010010101") },
            { L_,    SW_01,   DSTA,     6,g64("001101101100100010101") },
            { L_,    SW_01,   DSTA,     7,g64("011011011001001010101") },
            { L_,    SW_01,   DSTA,     8,g64("110110110010001010101") },

            { L_,   "1..1", "0..0",     0,                           0 },
            { L_,   "1..1", "0..0",     1,                           1 },
            { L_,   "1..1", "0..0", BPS-1,                    int64Max },
            { L_,   "1..1", "0..0",   BPS,                          ~0 },

            { L_,   "1..1", "1..1",     0,                          ~0 },
            { L_,   "1..1", "1..1",     1,                          ~0 },
            { L_,   "1..1", "1..1", BPS-1,                          ~0 },
            { L_,   "1..1", "1..1",   BPS,                          ~0 },

            { L_,   "1..1",  SW_01,     0,                  g64(SW_01) },
            { L_,   "1..1",  SW_01,     1,                g64(SW_10)|1 },
            { L_,   "1..1",  SW_01, BPS-1,                          ~0 },
            { L_,   "1..1",  SW_01,   BPS,                          ~0 },

            { L_,   "1..1",  SW_10,     0,                  g64(SW_10) },
            { L_,   "1..1",  SW_10,     1,                  g64(SW_01) },
            { L_,   "1..1",  SW_10, BPS-1,                    int64Max },
            { L_,   "1..1",  SW_10,   BPS,                          ~0 },

            // DSTA    =                  g64("000000001101101100100")
            { L_,   "1..1",   DSTA,     4,g64("000011011011001001111") },
            { L_,   "1..1",   DSTA,     5,g64("000110110110010011111") },
            { L_,   "1..1",   DSTA,     6,g64("001101101100100111111") },
            { L_,   "1..1",   DSTA,     7,g64("011011011001001111111") },
            { L_,   "1..1",   DSTA,     8,g64("110110110010011111111") },

            { L_,    SW_10, "0..0",     0,                           0 },
            { L_,    SW_10, "0..0",     1,                           0 },
            { L_,    SW_10, "0..0", BPS-1,         g64(SW_10)&int64Max },
            { L_,    SW_10, "0..0",   BPS,                  g64(SW_10) },

            { L_,    SW_10, "1..1",     0,                          ~0 },
            { L_,    SW_10, "1..1",     1,                          ~1 },
            { L_,    SW_10, "1..1", BPS-1,                  g64(SW_10) },
            { L_,    SW_10, "1..1",   BPS,                  g64(SW_10) },

            { L_,    SW_10,  SW_01,     0,                  g64(SW_01) },
            { L_,    SW_10,  SW_01,     1,                  g64(SW_10) },
            { L_,    SW_10,  SW_01, BPS-1,                  g64(SW_10) },
            { L_,    SW_10,  SW_01,   BPS,                  g64(SW_10) },

            { L_,    SW_10,  SW_10,     0,                  g64(SW_10) },
            { L_,    SW_10,  SW_10,     1,                g64(SW_01)^1 },
            { L_,    SW_10,  SW_10, BPS-1,         g64(SW_10)&int64Max },
            { L_,    SW_10,  SW_10,   BPS,                  g64(SW_10) },

            // DSTA   =                   g64("000000001101101100100")
            { L_,    SW_10,   DSTA,     4,g64("000011011011001001010") },
            { L_,    SW_10,   DSTA,     5,g64("000110110110010001010") },
            { L_,    SW_10,   DSTA,     6,g64("001101101100100101010") },
            { L_,    SW_10,   DSTA,     7,g64("011011011001000101010") },
            { L_,    SW_10,   DSTA,     8,g64("110110110010010101010") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE        = DATA_B[di].d_lineNum;
            const char *valueSpec = DATA_B[di].d_valueSpec;
            const char *dstSpec   = DATA_B[di].d_dst;
            int nBits             = DATA_B[di].d_nBits;
            Int64 expected        = DATA_B[di].d_insertValueUpTo;

            Int64 value = g64(valueSpec);
            Int64 src   = g64(dstSpec);
            Int64 dst   = src;
            Util::insertValueUpTo64(&dst, nBits, value);

            if (veryVerbose) {
                P_(LINE); P_(dstSpec); P_(valueSpec); P_(expected); P(dst);
            }

            LOOP7_ASSERT(LINE, dstSpec, valueSpec, expected, dst, src, nBits,
                                                              expected == dst);
        }
}

static
void testE(int test)
{
    switch (test) {
      case 25: {
        testE25();
      } break;
      case 24: {
        testE24();
      } break;
      case 23: {
        testE23();
      } break;
      case 22: {
        testE22();
      } break;
      case 21: {
        testE21();
      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testD20()
{
        // --------------------------------------------------------------------
        // TESTING SETBITVALUE AND REPLACEBITVALUE FUNCTIONS:
        //   The 'setBitValue' and 'replaceBitValue' methods have relatively
        //   simple implementations that perform calculations using
        //   already-tested methods.  A relatively small set of test data is
        //   sufficient.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source
        //   category, verify that the 'setBitValue' and 'replaceBitValue'
        //   functions each give the expected value.  Note that the "set"
        //   function returns its result while the "replace" function
        //   loads its result into a specified integer.
        //
        // Testing:
        //   int setBitValue(int sInt, int sIdx, int value);
        //   bsls_Types::Int64 setBitValue(
        //       bsls_Types::Int64 srcInteger, int srcIndex, int value);
        //   int setBitValue(int sInt, int sIdx, int value);
        //   void replaceBitValue(int *dInt, int dIdx, int value);
        //   void replaceBitValue64(bsls_Types::Int64 *dstInteger,
        //                          int dstIndex,
        //                          int value);
        // --------------------------------------------------------------------

        // value0 int array represents cases where an integer's least
        // significant bit is 0.
        const int value0[] = { g(FW_0), g(FW_10), g("1..10"), g("0..0110") };

        // value1 int array represents cases where an integer's least
        // significant bit is 1.
        const int value1[] = { g(FW_1), g(FW_01), g("0..01"), g("1..1001") };

        const int NUM_VAL0 = sizeof value0 / sizeof *value0;
        const int NUM_VAL1 = sizeof value1 / sizeof *value1;

        ASSERT(NUM_VAL0 == NUM_VAL1);

        if (verbose) cout << endl
            << "Testing setBitValue and replaceBitValue Functions\n"
            << "=================================================\n";

        const char *SRCA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum; // line number
            const char *d_dst;     // source integer
            int         d_index;   // source index
            int         d_result0; // expected result from
                                   // setBitValue/replaceBitValue function when
                                   // value's least significant bit is '0', for
                                   // example: g(FW_10).
            int         d_result1; // expected result from
                                   // setBitValue/replaceBitValue function when
                                   // value's least significant bit is '1', for
                                   // example: g(FW_01).
        } DATA_A[] = {
            // value's least significant bit =      0,                    1
            //L#     src  index               result0               result1
            //--  ------ ------           -----------           -----------
            { L_, "0..0",     0,                    0,                    1 },
            { L_, "0..0",     1,                    0,                    2 },
            { L_, "0..0", BPW-2,                    0,           1<<(BPW-2) },
            { L_, "0..0", BPW-1,                    0,              INT_MIN },

            { L_, "1..1",     0,                   ~1,                   ~0 },
            { L_, "1..1",     1,                   ~2,                   ~0 },
            { L_, "1..1", BPW-2,        ~(1<<(BPW-2)),                   ~0 },
            { L_, "1..1", BPW-1,              INT_MAX,                   ~0 },

            { L_,  FW_01,     0,           g(FW_01)^1,             g(FW_01) },
            { L_,  FW_01,     1,             g(FW_01),           g(FW_01)+2 },
            { L_,  FW_01, BPW-2,g(FW_01)^(1<<(BPW-2)),             g(FW_01) },
            { L_,  FW_01, BPW-1,             g(FW_01),     g(FW_01)|INT_MIN },

            { L_,  FW_10,     0,             g(FW_10),           g(FW_10)|1 },
            { L_,  FW_10,     1,           g(FW_10)^2,             g(FW_10) },
            { L_,  FW_10, BPW-2,             g(FW_10),g(FW_10)|(1<<(BPW-2)) },
            { L_,  FW_10, BPW-1,     g(FW_10)&INT_MAX,             g(FW_10) },

            // SRCA =              g("1101101100100")    g("1101101100100")
            { L_,   SRCA,     4,   g("1101101100100"),   g("1101101110100") },
            { L_,   SRCA,     5,   g("1101101000100"),   g("1101101100100") },
            { L_,   SRCA,     6,   g("1101100100100"),   g("1101101100100") },
            { L_,   SRCA,     7,   g("1101101100100"),   g("1101111100100") },
            { L_,   SRCA,     8,   g("1101001100100"),   g("1101101100100") },
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            for (int dj = 0; dj < NUM_VAL0; ++dj) {
                const int LINE    = DATA_A[di].d_lineNum;
                const char *spec  = DATA_A[di].d_dst;
                const int index   = DATA_A[di].d_index;
                const int result0 = DATA_A[di].d_result0;
                const int result1 = DATA_A[di].d_result1;

                int src = g(spec);
                int res = Util::setBitValue(src, index, value0[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result0); P(res);
                }

                LOOP_ASSERT(LINE, result0 == res);

                res = Util::setBitValue(src, index, value1[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result1); P(res);
                }

                LOOP_ASSERT(LINE, result1 == res);

                int dst = src;
                Util::replaceBitValue(&dst, index, value0[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result0); P(dst);
                }

                LOOP_ASSERT(LINE, result0 == dst);

                dst = src;
                Util::replaceBitValue(&dst, index, value1[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result1); P(dst);
                }

                LOOP_ASSERT(LINE, result1 == dst);
            }
        }

        if (verbose) cout << endl
            << "Testing setBitValue64 and replaceBitValue64 Functions\n"
            << "=====================================================\n";

        const Int64 one64 = 1;
        static const struct {
            int         d_lineNum; // line number
            const char *d_dst;     // source integer
            int         d_index;   // source index
            Int64       d_result0; // expected result from
                                   // setBitValue/replaceBitValue function when
                                   // value's least significant bit is '0', for
                                   // example: g(FW_10).
            Int64       d_result1; // expected result from
                                   // setBitValue/replaceBitValue function when
                                   // value's least significant bit is '1', for
                                   // example: g(FW_01).
        } DATA_B[] = {
            // value's least significant bit =      0,                    1
            //L#     src  index               result0               result1
            //--  ------ ------           -----------           -----------
            { L_, "0..0",     0,                    0,                    1 },
            { L_, "0..0",     1,                    0,                    2 },
            { L_, "0..0", BPS-2,                    0,       one64<<(BPS-2) },
            { L_, "0..0", BPS-1,                    0,             int64Min },

            { L_, "1..1",     0,                   ~1,                   ~0 },
            { L_, "1..1",     1,                   ~2,                   ~0 },
            { L_, "1..1", BPS-2,    ~(one64<<(BPS-2)),                   ~0 },
            { L_, "1..1", BPS-1,             int64Max,                   ~0 },

            { L_,  SW_01,     0,         g64(SW_01)^1,           g64(SW_01) },
            { L_,  SW_01,     1,           g64(SW_01),         g64(SW_01)+2 },
            { L_,  SW_01, BPS-2,g64(SW_01)^(one64<<(BPS-2)),     g64(SW_01) },
            { L_,  SW_01, BPS-1,           g64(SW_01),  g64(SW_01)|int64Min },

            { L_,  SW_10,     0,           g64(SW_10),         g64(SW_10)|1 },
            { L_,  SW_10,     1,         g64(SW_10)^2,           g64(SW_10) },
            { L_,  SW_10, BPS-2,    g64(SW_10),  g64(SW_10)|(one64<<(BPS-2))},
            { L_,  SW_10, BPS-1,  g64(SW_10)&int64Max,           g64(SW_10) },

            // SRCA =            g64("1101101100100")  g64("1101101100100")
            { L_,   SRCA,     4, g64("1101101100100"), g64("1101101110100") },
            { L_,   SRCA,     5, g64("1101101000100"), g64("1101101100100") },
            { L_,   SRCA,     6, g64("1101100100100"), g64("1101101100100") },
            { L_,   SRCA,     7, g64("1101101100100"), g64("1101111100100") },
            { L_,   SRCA,     8, g64("1101001100100"), g64("1101101100100") },
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            for (int dj = 0; dj < NUM_VAL0; ++dj) {
                const int LINE      = DATA_B[di].d_lineNum;
                const char *spec    = DATA_B[di].d_dst;
                const int index     = DATA_B[di].d_index;
                const Int64 result0 = DATA_B[di].d_result0;
                const Int64 result1 = DATA_B[di].d_result1;

                Int64 src = g64(spec);
                Int64 res = Util::setBitValue64(src, index, value0[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result0); P(res);
                }

                LOOP5_ASSERT(LINE, spec, index, result0, res, result0 == res);

                res = Util::setBitValue64(src, index, value1[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result1); P(res);
                }

                LOOP5_ASSERT(LINE, spec, index, result1, res, result1 == res);

                Int64 dst = src;
                Util::replaceBitValue64(&dst, index, value0[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result0); P(dst);
                }

                LOOP5_ASSERT(LINE, spec, index, result0, dst, result0 == dst);

                dst = src;
                Util::replaceBitValue64(&dst, index, value1[dj]);

                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(index); P_(result1); P(dst);
                }

                LOOP5_ASSERT(LINE, spec, index, result1, dst, result1 == dst);
            }
        }
}

static
void testD19()
{
        // --------------------------------------------------------------------
        // TESTING SETBIT AND REPLACEBIT FUNCTIONS:
        //   The 'setBitX' and 'replaceBitX' methods have very simple
        //   implementations that perform calculations using already-tested
        //   methods.  A relatively small set of test data is sufficient.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by source
        //   category, verify that the 'setBit0', 'setBit1', 'replaceBit0' and
        //   'replaceBit1' functions each give the expected value.  Note that
        //   the "set" functions return their results while the "replace"
        //   functions load their results into a specified integer.
        //
        // Testing:
        //   int setBitZero(  int sInt, int sIdx);
        //   int setBitZero64(int sInt, int sIdx);
        //   int setBitOne(   int sInt, int sIdx);
        //   int setBitOne64( int sInt, int sIdx);
        //   void replaceBitZero(  int *dInt, int dIdx);
        //   void replaceBitZero64(int *dInt, int dIdx);
        //   void replaceBitOne64( int *dInt, int dIdx);
        //   void replaceBitOne(   int *dInt, int dIdx);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 32-bit setBit and replaceBit Functions" << endl
            << "==============================================" << endl;

        const char *SRCA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum; // line number
            const char *d_dst;     // source integer
            int         d_index;   // source index
            int         d_result0; // expected result from setBit0/replaceBit0
            int         d_result1; // expected result from setBit1/replaceBit1
        } DATA_A[] = {
            //                                setBit0               setBit1
            //L#     src  index           replaceBit0           replaceBit1
            //--  ------ ------           -----------           -----------
            { L_, "0..0",     0,                    0,                    1 },
            { L_, "0..0",     1,                    0,                    2 },
            { L_, "0..0", BPW-2,                    0,           1<<(BPW-2) },
            { L_, "0..0", BPW-1,                    0,              INT_MIN },

            { L_, "1..1",     0,                   ~1,                   ~0 },
            { L_, "1..1",     1,                   ~2,                   ~0 },
            { L_, "1..1", BPW-2,        ~(1<<(BPW-2)),                   ~0 },
            { L_, "1..1", BPW-1,              INT_MAX,                   ~0 },

            { L_,  FW_01,     0,           g(FW_01)^1,             g(FW_01) },
            { L_,  FW_01,     1,             g(FW_01),           g(FW_01)|2 },
            { L_,  FW_01, BPW-2,g(FW_01)^(1<<(BPW-2)),             g(FW_01) },
            { L_,  FW_01, BPW-1,             g(FW_01),     g(FW_01)|INT_MIN },

            { L_,  FW_10,     0,             g(FW_10),           g(FW_10)|1 },
            { L_,  FW_10,     1,           g(FW_10)^2,             g(FW_10) },
            { L_,  FW_10, BPW-2,             g(FW_10),g(FW_10)|(1<<(BPW-2)) },
            { L_,  FW_10, BPW-1,     g(FW_10)&INT_MAX,             g(FW_10) },

            // SRCA =              g("1101101100100")    g("1101101100100")
            { L_,   SRCA,     4,   g("1101101100100"),   g("1101101110100") },
            { L_,   SRCA,     5,   g("1101101000100"),   g("1101101100100") },
            { L_,   SRCA,     6,   g("1101100100100"),   g("1101101100100") },
            { L_,   SRCA,     7,   g("1101101100100"),   g("1101111100100") },
            { L_,   SRCA,     8,   g("1101001100100"),   g("1101101100100") },
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int src = g(spec);

            int resA = Util::setBitZero(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result0);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result0 == resA);

            resA = Util::setBitOne(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result1);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result1 == resA);

            int dstA = src;

            Util::replaceBitZero(&dstA, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result0);P_(dstA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result0 == dstA);

            dstA = src;

            Util::replaceBitOne(&dstA, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result1);P_(dstA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result1 == dstA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose)
            cout << "Testing setBit0, setBit1, replaceBit0, replaceBit1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int src = g(spec);

            int resA = Util::setBit0(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result0);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result0 == resA);

            resA = Util::setBit1(src, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result1);P_(resA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result1 == resA);

            int dstA = src;

            Util::replaceBit0(&dstA, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result0);P_(dstA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result0 == dstA);

            dstA = src;

            Util::replaceBit1(&dstA, DATA_A[di].d_index);

            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_result1);P_(dstA);
            }

            LOOP_ASSERT(LINE, DATA_A[di].d_result1 == dstA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing 64-bit setBit and replaceBit Functions" << endl
            << "==============================================" << endl;

        const char *SRCB = "1101101100100";    // typical case
        const char *SRCC = "110110110010..0";    // typical case
        const Int64 one = 1;
        static const struct {
            int         d_lineNum; // line number
            const char *d_dst;     // source integer
            int         d_index;   // source index
            Int64       d_result0; // expected result from setBit0/replaceBit0
            Int64       d_result1; // expected result from setBit1/replaceBit1
        } DATA_B[] = {
            //                                setBit0               setBit1
            //L#     src  index           replaceBit0           replaceBit1
            //--  ------ ------           -----------           -----------
            { L_, "0..0",     0,                    0,                    1 },
            { L_, "0..0",     1,                    0,                    2 },
            { L_, "0..0", BPS-2,                    0,         one<<(BPS-2) },
            { L_, "0..0", BPS-1,                    0,             int64Min },

            { L_, "1..1",     0,                   ~1,                   ~0 },
            { L_, "1..1",     1,                   ~2,                   ~0 },
            { L_, "1..1", BPS-2,      ~(one<<(BPS-2)),                   ~0 },
            { L_, "1..1", BPS-1,             int64Max,                   ~0 },

            { L_,  SW_01,     0,         g64(SW_01)^1,           g64(SW_01) },
            { L_,  SW_01,     1,           g64(SW_01),         g64(SW_01)|2 },
            { L_,  SW_01, BPS-2,g64(SW_01)^(one<<(BPS-2)),       g64(SW_01) },
            { L_,  SW_01, BPS-1,           g64(SW_01),  g64(SW_01)|int64Min },

            { L_,  SW_10,     0,           g64(SW_10),         g64(SW_10)|1 },
            { L_,  SW_10,     1,         g64(SW_10)^2,           g64(SW_10) },
            { L_,  SW_10, BPS-2,     g64(SW_10),  g64(SW_10)|(one<<(BPS-2)) },
            { L_,  SW_10, BPS-1,  g64(SW_10)&int64Max,           g64(SW_10) },

            // SRCB =            g64("1101101100100")  g64("1101101100100")
            { L_,   SRCB,     4, g64("1101101100100"), g64("1101101110100") },
            { L_,   SRCB,     5, g64("1101101000100"), g64("1101101100100") },
            { L_,   SRCB,     6, g64("1101100100100"), g64("1101101100100") },
            { L_,   SRCB,     7, g64("1101101100100"), g64("1101111100100") },
            { L_,   SRCB,     8, g64("1101001100100"), g64("1101101100100") },

            // SRCC =           g64("110110110010..0") g64("110110110010..0")
            { L_,   SRCC,    55,g64("110110110010..0"),g64("110110111010..0")},
            { L_,   SRCC,    56,g64("110110100010..0"),g64("110110110010..0")},
            { L_,   SRCC,    57,g64("110110010010..0"),g64("110110110010..0")},
            { L_,   SRCC,    58,g64("110110110010..0"),g64("110111110010..0")},
            { L_,   SRCC,    59,g64("110100110010..0"),g64("110110110010..0")},
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 src = g64(spec);

            Int64 res = Util::setBitZero64(src, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_result0); P(res);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_result0 == res);

            res = Util::setBitOne64(src, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_result1); P(res);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_result1 == res);

            Int64 dst = src;

            Util::replaceBitZero64(&dst, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_result0); P(dst);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_result0 == dst);

            dst = src;

            Util::replaceBitOne64(&dst, DATA_B[di].d_index);

            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_result1); P(dst);
            }

            LOOP_ASSERT(LINE, DATA_B[di].d_result1 == dst);
        }
}

static
void testD18()
{
        // --------------------------------------------------------------------
        // TESTING SETVALUEUPTO AND REPLACEVALUEUPTO FUNCTION:
        //   The 'setValueUpTo' and 'replaceValueUpTo' methods have relatively
        //   simple implementations that perform calculations using
        //   already-tested methods.  A relatively small set of test data is
        //   sufficient, but we choose a more thorough and systematic set of
        //   test data to completely probe the algorithm and not just the
        //   implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by input
        //   value and then source category, verify that 'setValueUpTo' and
        //   'replaceValueUpTo' each give the expected value.  Note that the
        //   "set" function returns its result while the "replace" function
        //   loads its result into a specified integer.
        //
        // Testing:
        //   int setValueUpTo(int sInt, int sIdx, int value);
        //   void replaceValueUpTo(int *dInt, int dIdx, int sInt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing setValueUpTo and replaceValueUpTo Function\n"
            << "==================================================\n";

        const char *SRCA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;  // line number
            int         d_value;    // value
            const char *d_src;      // source integer
            int         d_index;    // source index
            int         d_res;      // expected result from
                                    // setValueUpTo/replaceValueUpTo functions
        } DATA_A[] = {
            //L#   value        src  Index                   result
            //--  ---------  ------  -----           --------------
            { L_, g("0..0"), "0..0",     0,                       0 },
            { L_, g("0..0"), "0..0",     1,                       0 },
            { L_, g("0..0"), "0..0", BPW-1,                       0 },
//          { L_, g("0..0"), "0..0",   BPW,                       0 },

            { L_, g("0..0"), "1..1",     0,                      ~0 },
            { L_, g("0..0"), "1..1",     1,                      ~1 },
            { L_, g("0..0"), "1..1", BPW-1,                 INT_MIN },
//          { L_, g("0..0"), "1..1",   BPW,                       0 },

            { L_, g("0..0"),  FW_01,     0,                g(FW_01) },
            { L_, g("0..0"),  FW_01,     1,              g(FW_01)^1 },
            { L_, g("0..0"),  FW_01, BPW-1,                       0 },
//          { L_, g("0..0"),  FW_01,   BPW,                       0 },

            { L_, g("0..0"),  FW_10,     0,                g(FW_10) },
            { L_, g("0..0"),  FW_10,     1,                g(FW_10) },
            { L_, g("0..0"),  FW_10, BPW-1,                 INT_MIN },
//          { L_, g("0..0"),  FW_10,   BPW,                       0 },

            //SRCA    =                          g("1101101100100")
            { L_, g("0..0"),   SRCA,     4,      g("1101101100000") },
            { L_, g("0..0"),   SRCA,     5,      g("1101101100000") },
            { L_, g("0..0"),   SRCA,     6,      g("1101101000000") },
            { L_, g("0..0"),   SRCA,     7,      g("1101100000000") },
            { L_, g("0..0"),   SRCA,     8,      g("1101100000000") },

            { L_, g("1..1"), "0..0",     0,                       0 },
            { L_, g("1..1"), "0..0",     1,                       1 },
            { L_, g("1..1"), "0..0", BPW-1,                 INT_MAX },
//          { L_, g("1..1"), "0..0",   BPW,                      ~0 },

            { L_, g("1..1"), "1..1",     0,                      ~0 },
            { L_, g("1..1"), "1..1",     1,                      ~0 },
            { L_, g("1..1"), "1..1", BPW-1,                      ~0 },
//          { L_, g("1..1"), "1..1",   BPW,                      ~0 },

            { L_, g("1..1"),  FW_01,     0,                g(FW_01) },
            { L_, g("1..1"),  FW_01,     1,                g(FW_01) },
            { L_, g("1..1"),  FW_01, BPW-1,                 INT_MAX },
//          { L_, g("1..1"),  FW_01,   BPW,                      ~0 },

            { L_, g("1..1"),  FW_10,     0,                g(FW_10) },
            { L_, g("1..1"),  FW_10,     1,              g(FW_10)|1 },
            { L_, g("1..1"),  FW_10, BPW-1,                      ~0 },
//          { L_, g("1..1"),  FW_10,   BPW,               g("1..1") },

            //SRCA    =                          g("1101101100100")
            { L_, g("1..1"),   SRCA,     4,      g("1101101101111") },
            { L_, g("1..1"),   SRCA,     5,      g("1101101111111") },
            { L_, g("1..1"),   SRCA,     6,      g("1101101111111") },
            { L_, g("1..1"),   SRCA,     7,      g("1101101111111") },
            { L_, g("1..1"),   SRCA,     8,      g("1101111111111") },

            { L_,  g(FW_01),  "0..0",     0,                      0 },
            { L_,  g(FW_01),  "0..0",     1,                      1 },
            { L_,  g(FW_01),  "0..0", BPW-1,               g(FW_01) },
//          { L_,  g(FW_01),  "0..0",   BPW,               g(FW_01) },

            { L_,  g(FW_01),  "1..1",     0,                     ~0 },
            { L_,  g(FW_01),  "1..1",     1,                     ~0 },
            { L_,  g(FW_01),  "1..1", BPW-1,       g(FW_01)|INT_MIN },
//          { L_,  g(FW_01),  "1..1",   BPW,               g(FW_01) },

            { L_,  g(FW_01),   FW_01,     0,               g(FW_01) },
            { L_,  g(FW_01),   FW_01,     1,               g(FW_01) },
            { L_,  g(FW_01),   FW_01, BPW-1,               g(FW_01) },
//          { L_,  g(FW_01),   FW_01,   BPW,               g(FW_01) },

            { L_,  g(FW_01),   FW_10,     0,               g(FW_10) },
            { L_,  g(FW_01),   FW_10,     1,             g(FW_10)|1 },
            { L_,  g(FW_01),   FW_10, BPW-1,       g(FW_01)|INT_MIN },
//          { L_,  g(FW_01),   FW_10,   BPW,               g(FW_01) },

            //SRCA    =                          g("1101101100100")
            { L_,  g(FW_01),    SRCA,     4,     g("1101101100101") },
            { L_,  g(FW_01),    SRCA,     5,     g("1101101110101") },
            { L_,  g(FW_01),    SRCA,     6,     g("1101101010101") },
            { L_,  g(FW_01),    SRCA,     7,     g("1101101010101") },
            { L_,  g(FW_01),    SRCA,     8,     g("1101101010101") },

            { L_,  g(FW_10),  "0..0",     0,                      0 },
            { L_,  g(FW_10),  "0..0",     1,                      0 },
            { L_,  g(FW_10),  "0..0", BPW-1,       g(FW_10)&INT_MAX },
//          { L_,  g(FW_10),  "0..0",   BPW,               g(FW_10) },

            { L_,  g(FW_10),  "1..1",     0,                     ~0 },
            { L_,  g(FW_10),  "1..1",     1,                     ~1 },
            { L_,  g(FW_10),  "1..1", BPW-1,               g(FW_10) },
//          { L_,  g(FW_10),  "1..1",   BPW,               g(FW_10) },

            { L_,  g(FW_10),   FW_01,     0,               g(FW_01) },
            { L_,  g(FW_10),   FW_01,     1,             g(FW_01)^1 },
            { L_,  g(FW_10),   FW_01, BPW-1,       g(FW_10)&INT_MAX },
//          { L_,  g(FW_10),   FW_01,   BPW,               g(FW_10) },

            { L_,  g(FW_10),   FW_10,     0,               g(FW_10) },
            { L_,  g(FW_10),   FW_10,     1,               g(FW_10) },
            { L_,  g(FW_10),   FW_10, BPW-1,               g(FW_10) },
//          { L_,  g(FW_10),   FW_10,   BPW,               g(FW_10) },

            //SRCA    =                          g("1101101100100")
            { L_,  g(FW_10),    SRCA,     4,     g("1101101101010") },
            { L_,  g(FW_10),    SRCA,     5,     g("1101101101010") },
            { L_,  g(FW_10),    SRCA,     6,     g("1101101101010") },
            { L_,  g(FW_10),    SRCA,     7,     g("1101100101010") },
            { L_,  g(FW_10),    SRCA,     8,     g("1101110101010") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE  = DATA_A[di].d_lineNum;
            const char *spec  = DATA_A[di].d_src;
            const int   index = DATA_A[di].d_index;
            const int   value = DATA_A[di].d_value;

            int src = g(spec);
            int res = Util::setValueUpTo(src, index, value);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res); P(res);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res == res);

            Util::replaceValueUpTo(&src, index, value);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res); P(src);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res == src);
        }

        if (verbose) cout << endl
            << "Testing setValueUpTo64 and replaceValueUpTo64 Function\n"
            << "======================================================\n";

        static const struct {
            int         d_lineNum;  // line number
            const char *d_value;    // value
            const char *d_src;      // source integer
            int         d_index;    // source index
            Int64       d_res;      // expected result from
                                    // setValueUpTo/replaceValueUpTo functions
        } DATA_B[] = {
            //L#   value        src  Index                   result
            //--  ---------  ------  -----           --------------
            { L_,   "0..0",  "0..0",     0,                       0 },
            { L_,   "0..0",  "0..0",     1,                       0 },
            { L_,   "0..0",  "0..0", BPS-1,                       0 },
//          { L_,   "0..0",  "0..0",   BPS,                       0 },

            { L_,   "0..0",  "1..1",     0,                      ~0 },
            { L_,   "0..0",  "1..1",     1,                      ~1 },
            { L_,   "0..0",  "1..1", BPS-1,                int64Min },
//          { L_,   "0..0",  "1..1",   BPS,                       0 },

            { L_,   "0..0",   SW_01,     0,              g64(SW_01) },
            { L_,   "0..0",   SW_01,     1,            g64(SW_01)^1 },
            { L_,   "0..0",   SW_01, BPS-1,                       0 },
//          { L_,   "0..0",   SW_01,   BPS,                       0 },

            { L_,   "0..0",   SW_10,     0,              g64(SW_10) },
            { L_,   "0..0",   SW_10,     1,              g64(SW_10) },
            { L_,   "0..0",   SW_10, BPS-1,                int64Min },
//          { L_,   "0..0",   SW_10,   BPS,                       0 },

            //SRCA    =                        g64("1101101100100")
            { L_,   "0..0",    SRCA,     4,    g64("1101101100000") },
            { L_,   "0..0",    SRCA,     5,    g64("1101101100000") },
            { L_,   "0..0",    SRCA,     6,    g64("1101101000000") },
            { L_,   "0..0",    SRCA,     7,    g64("1101100000000") },
            { L_,   "0..0",    SRCA,     8,    g64("1101100000000") },

            { L_,   "1..1",  "0..0",     0,                       0 },
            { L_,   "1..1",  "0..0",     1,                       1 },
            { L_,   "1..1",  "0..0", BPS-1,                int64Max },
//          { L_,   "1..1",  "0..0",   BPS,                      ~0 },

            { L_,   "1..1",  "1..1",     0,                      ~0 },
            { L_,   "1..1",  "1..1",     1,                      ~0 },
            { L_,   "1..1",  "1..1", BPS-1,                      ~0 },
//          { L_,   "1..1",  "1..1",   BPS,                      ~0 },

            { L_,   "1..1",   SW_01,     0,              g64(SW_01) },
            { L_,   "1..1",   SW_01,     1,              g64(SW_01) },
            { L_,   "1..1",   SW_01, BPS-1,                int64Max },
//          { L_,   "1..1",   SW_01,   BPS,                      ~0 },

            { L_,   "1..1",   SW_10,     0,              g64(SW_10) },
            { L_,   "1..1",   SW_10,     1,            g64(SW_10)|1 },
            { L_,   "1..1",   SW_10, BPS-1,                      ~0 },
//          { L_,   "1..1",   SW_10,   BPS,             g64("1..1") },

            //SRCA    =                        g64("1101101100100")
            { L_,   "1..1",    SRCA,     4,    g64("1101101101111") },
            { L_,   "1..1",    SRCA,     5,    g64("1101101111111") },
            { L_,   "1..1",    SRCA,     6,    g64("1101101111111") },
            { L_,   "1..1",    SRCA,     7,    g64("1101101111111") },
            { L_,   "1..1",    SRCA,     8,    g64("1101111111111") },

            { L_,    SW_01,   "0..0",     0,                      0 },
            { L_,    SW_01,   "0..0",     1,                      1 },
            { L_,    SW_01,   "0..0", BPS-1,             g64(SW_01) },
//          { L_,    SW_01,   "0..0",   BPS,             g64(SW_01) },

            { L_,    SW_01,   "1..1",     0,                     ~0 },
            { L_,    SW_01,   "1..1",     1,                     ~0 },
            { L_,    SW_01,   "1..1", BPS-1,    g64(SW_01)|int64Min },
//          { L_,    SW_01,   "1..1",   BPS,             g64(SW_01) },

            { L_,    SW_01,    SW_01,     0,             g64(SW_01) },
            { L_,    SW_01,    SW_01,     1,             g64(SW_01) },
            { L_,    SW_01,    SW_01, BPS-1,             g64(SW_01) },
//          { L_,    SW_01,    SW_01,   BPS,             g64(SW_01) },

            { L_,    SW_01,    SW_10,     0,             g64(SW_10) },
            { L_,    SW_01,    SW_10,     1,           g64(SW_10)|1 },
            { L_,    SW_01,    SW_10, BPS-1,    g64(SW_01)|int64Min },
//          { L_,    SW_01,    SW_10,   BPS,             g64(SW_01) },

            //SRCA    =                        g64("1101101100100")
            { L_,    SW_01,     SRCA,     4,   g64("1101101100101") },
            { L_,    SW_01,     SRCA,     5,   g64("1101101110101") },
            { L_,    SW_01,     SRCA,     6,   g64("1101101010101") },
            { L_,    SW_01,     SRCA,     7,   g64("1101101010101") },
            { L_,    SW_01,     SRCA,     8,   g64("1101101010101") },

            { L_,    SW_10,   "0..0",     0,                      0 },
            { L_,    SW_10,   "0..0",     1,                      0 },
            { L_,    SW_10,   "0..0", BPS-1,    g64(SW_10)&int64Max },
//          { L_,    SW_10,   "0..0",   BPS,             g64(SW_10) },

            { L_,    SW_10,   "1..1",     0,                     ~0 },
            { L_,    SW_10,   "1..1",     1,                     ~1 },
            { L_,    SW_10,   "1..1", BPS-1,             g64(SW_10) },
//          { L_,    SW_10,   "1..1",   BPS,             g64(SW_10) },

            { L_,    SW_10,    SW_01,     0,             g64(SW_01) },
            { L_,    SW_10,    SW_01,     1,           g64(SW_01)^1 },
            { L_,    SW_10,    SW_01, BPS-1,    g64(SW_10)&int64Max },
//          { L_,    SW_10,    SW_01,   BPS,             g64(SW_10) },

            { L_,    SW_10,    SW_10,     0,             g64(SW_10) },
            { L_,    SW_10,    SW_10,     1,             g64(SW_10) },
            { L_,    SW_10,    SW_10, BPS-1,             g64(SW_10) },
//          { L_,    SW_10,    SW_10,   BPS,             g64(SW_10) },

            //SRCA    =                        g64("1101101100100")
            { L_,    SW_10,     SRCA,     4,   g64("1101101101010") },
            { L_,    SW_10,     SRCA,     5,   g64("1101101101010") },
            { L_,    SW_10,     SRCA,     6,   g64("1101101101010") },
            { L_,    SW_10,     SRCA,     7,   g64("1101100101010") },
            { L_,    SW_10,     SRCA,     8,   g64("1101110101010") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE  = DATA_B[di].d_lineNum;
            const char *spec  = DATA_B[di].d_src;
            const int   index = DATA_B[di].d_index;
            const char *vSpec = DATA_B[di].d_value;

            Int64 src   = g64(spec);
            Int64 value = g64(vSpec);
            Int64 res = Util::setValueUpTo64(src, index, value);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res == res);

            Util::replaceValueUpTo64(&src, index, value);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res); P(src);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res == src);
        }
}

static
void testD17()
{
        // --------------------------------------------------------------------
        // TESTING MINUSEQUAL FUNCTION:
        //   The three-parameter 'minusEqual' function has a relatively simple
        //   implementation that branches on the value of 'booleanValue', but
        //   otherwise relies on tested functionality.  A relatively small set
        //   of test data is sufficient, but we choose a more thorough and
        //   systematic set of test data to completely probe the algorithm and
        //   not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'minusEqual' sets the expected
        //   value.
        //
        // Testing:
        //   void minusEqual(int *dInt, int dIdx, int booleanValue);
        //   void minusEqual64(bsls_Types::Int64 *dInt, int dIdx,
        //                     int booleanValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing minusEqual Function" << endl
                          << "===========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_minusEqual0; // expected result (minusEqual(..., 0))
            int         d_minusEqual1; // expected result (minusEqual(..., 1))
        } DATA_A[] = {
            //    boolean value =                   0                    1
            //L#       dst  Index   minusEqual result    minusEqual result
            //--    ------  -----   -----------------    -----------------
            { L_,   "0..0",     0,                  0,                   0 },
            { L_,   "0..0",     1,                  0,                   0 },
            { L_,   "0..0", BPW-2,                  0,                   0 },
            { L_,   "0..0", BPW-1,                  0,                   0 },

            { L_,  "0..01",     0,                  1,                   0 },
            { L_,  "0..01",     1,                  1,                   1 },
            { L_,  "0..01", BPW-2,                  1,                   1 },
            { L_,  "0..01", BPW-1,                  1,                   1 },

            { L_, "0..010",     0,                  2,                   2 },
            { L_, "0..010",     1,                  2,                   0 },
            { L_, "0..010", BPW-2,                  2,                   2 },
            { L_, "0..010", BPW-1,                  2,                   2 },

            { L_, "0..011",     0,                  3,                   2 },
            { L_, "0..011",     1,                  3,                   1 },
            { L_, "0..011", BPW-2,                  3,                   3 },
            { L_, "0..011", BPW-1,                  3,                   3 },

            { L_,  "10..0",     0,            INT_MIN,             INT_MIN },
            { L_,  "10..0",     1,            INT_MIN,             INT_MIN },
            { L_,  "10..0", BPW-2,            INT_MIN,             INT_MIN },
            { L_,  "10..0", BPW-1,            INT_MIN,                   0 },

            { L_, "010..0",     0,        g("010..0"),         g("010..0") },
            { L_, "010..0",     1,        g("010..0"),         g("010..0") },
            { L_, "010..0", BPW-2,        g("010..0"),                   0 },
            { L_, "010..0", BPW-1,        g("010..0"),         g("010..0") },

            { L_, "110..0",     0,        g("110..0"),         g("110..0") },
            { L_, "110..0",     1,        g("110..0"),         g("110..0") },
            { L_, "110..0", BPW-2,        g("110..0"),             INT_MIN },
            { L_, "110..0", BPW-1,        g("110..0"),         g("010..0") },

            { L_, "10..01",     0,        INT_MIN + 1,             INT_MIN },
            { L_, "10..01",     1,        INT_MIN + 1,         INT_MIN + 1 },
            { L_, "10..01", BPW-2,        INT_MIN + 1,         INT_MIN + 1 },
            { L_, "10..01", BPW-1,        INT_MIN + 1,                   1 },

            { L_,   "1..1",     0,                 ~0,                  ~1 },
            { L_,   "1..1",     1,                 ~0,                  ~2 },
            { L_,   "1..1", BPW-2,                 ~0,       ~(1<<(BPW-2)) },
            { L_,   "1..1", BPW-1,                 ~0,             INT_MAX },

            { L_,  "1..10",     0,         g("1..10"),          g("1..10") },
            { L_,  "1..10",     1,         g("1..10"),         g("1..100") },
            { L_,  "1..10", BPW-2,         g("1..10"),        g("101..10") },
            { L_,  "1..10", BPW-1,         g("1..10"),         g("01..10") },

            { L_, "1..101",     0,        g("1..101"),         g("1..100") },
            { L_, "1..101",     1,        g("1..101"),         g("1..101") },
            { L_, "1..101", BPW-2,        g("1..101"),       g("101..101") },
            { L_, "1..101", BPW-1,        g("1..101"),        g("01..101") },

            { L_, "1..100",     0,        g("1..100"),         g("1..100") },
            { L_, "1..100",     1,        g("1..100"),         g("1..100") },
            { L_, "1..100", BPW-2,        g("1..100"),       g("101..100") },
            { L_, "1..100", BPW-1,        g("1..100"),        g("01..100") },

            { L_,  "01..1",     0,            INT_MAX,         INT_MAX - 1 },
            { L_,  "01..1",     1,            INT_MAX,         INT_MAX - 2 },
            { L_,  "01..1", BPW-2,            INT_MAX,         INT_MAX >>1 },
            { L_,  "01..1", BPW-1,            INT_MAX,             INT_MAX },

            { L_, "101..1",     0,        g("101..1"),        g("101..10") },
            { L_, "101..1",     1,        g("101..1"),       g("101..101") },
            { L_, "101..1", BPW-2,        g("101..1"),         g("101..1") },
            { L_, "101..1", BPW-1,        g("101..1"),         g("001..1") },

            { L_, "001..1",     0,        g("001..1"),        g("001..10") },
            { L_, "001..1",     1,        g("001..1"),       g("001..101") },
            { L_, "001..1", BPW-2,        g("001..1"),         g("001..1") },
            { L_, "001..1", BPW-1,        g("001..1"),         g("001..1") },

            { L_, "01..10",     0,        INT_MAX - 1,         INT_MAX - 1 },
            { L_, "01..10",     1,        INT_MAX - 1,         INT_MAX - 3 },
            { L_, "01..10", BPW-2,        INT_MAX - 1,        g("001..10") },
            { L_, "01..10", BPW-1,        INT_MAX - 1,         INT_MAX - 1 },

            { L_,    FW_01,     0,           g(FW_01),          g(FW_01)^1 },
            { L_,    FW_01,     1,           g(FW_01),            g(FW_01) },
            { L_,    FW_01, BPW-2,           g(FW_01), g(FW_01)^1<<(BPW-2) },
            { L_,    FW_01, BPW-1,           g(FW_01),            g(FW_01) },

            { L_,    FW_10,     0,           g(FW_10),            g(FW_10) },
            { L_,    FW_10,     1,           g(FW_10),          g(FW_10)^2 },
            { L_,    FW_10, BPW-2,           g(FW_10),            g(FW_10) },
            { L_,    FW_10, BPW-1,           g(FW_10),    g(FW_10)&INT_MAX },

            //        DSTA         g("1101101100100")   g("1101101100100")
            { L_,     DSTA,    4,  g("1101101100100"),  g("1101101100100") },
            { L_,     DSTA,    5,  g("1101101100100"),  g("1101101000100") },
            { L_,     DSTA,    6,  g("1101101100100"),  g("1101100100100") },
            { L_,     DSTA,    7,  g("1101101100100"),  g("1101101100100") },
            { L_,     DSTA,    8,  g("1101101100100"),  g("1101001100100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int src = g(spec);
            int dst0 = src;
            Util::minusEqual(&dst0, DATA_A[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_minusEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_minusEqual0 == dst0);

            for (int si = 0, arg = 1; si < 20; ++si, arg = 23 * arg + 7) {
                int dst1 = src;
                Util::minusEqual(&dst1, DATA_A[di].d_index, arg);
                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(DATA_A[di].d_minusEqual1); P(dst1);
                }
                LOOP_ASSERT(LINE, DATA_A[di].d_minusEqual1 == dst1);
            }
        }

        if (verbose) cout << endl
                          << "Testing minusEqual64 Function" << endl
                          << "=============================" << endl;

        const Int64 one64 = 1;
        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            Int64       d_minusEqual0; // expected result (minusEqual(..., 0))
            Int64       d_minusEqual1; // expected result (minusEqual(..., 1))
        } DATA_B[] = {
            //    boolean value =                   0                    1
            //L#       dst  Index   minusEqual result    minusEqual result
            //--    ------  -----   -----------------    -----------------
            { L_,   "0..0",     0,                  0,                   0 },
            { L_,   "0..0",     1,                  0,                   0 },
            { L_,   "0..0", BPS-2,                  0,                   0 },
            { L_,   "0..0", BPS-1,                  0,                   0 },

            { L_,  "0..01",     0,                  1,                   0 },
            { L_,  "0..01",     1,                  1,                   1 },
            { L_,  "0..01", BPS-2,                  1,                   1 },
            { L_,  "0..01", BPS-1,                  1,                   1 },

            { L_, "0..010",     0,                  2,                   2 },
            { L_, "0..010",     1,                  2,                   0 },
            { L_, "0..010", BPS-2,                  2,                   2 },
            { L_, "0..010", BPS-1,                  2,                   2 },

            { L_, "0..011",     0,                  3,                   2 },
            { L_, "0..011",     1,                  3,                   1 },
            { L_, "0..011", BPS-2,                  3,                   3 },
            { L_, "0..011", BPS-1,                  3,                   3 },

            { L_,  "10..0",     0,           int64Min,            int64Min },
            { L_,  "10..0",     1,           int64Min,            int64Min },
            { L_,  "10..0", BPS-2,           int64Min,            int64Min },
            { L_,  "10..0", BPS-1,           int64Min,                   0 },

            { L_, "010..0",     0,      g64("010..0"),       g64("010..0") },
            { L_, "010..0",     1,      g64("010..0"),       g64("010..0") },
            { L_, "010..0", BPS-2,      g64("010..0"),                   0 },
            { L_, "010..0", BPS-1,      g64("010..0"),       g64("010..0") },

            { L_, "110..0",     0,      g64("110..0"),       g64("110..0") },
            { L_, "110..0",     1,      g64("110..0"),       g64("110..0") },
            { L_, "110..0", BPS-2,      g64("110..0"),            int64Min },
            { L_, "110..0", BPS-1,      g64("110..0"),       g64("010..0") },

            { L_, "10..01",     0,       int64Min + 1,            int64Min },
            { L_, "10..01",     1,       int64Min + 1,        int64Min + 1 },
            { L_, "10..01", BPS-2,       int64Min + 1,        int64Min + 1 },
            { L_, "10..01", BPS-1,       int64Min + 1,                   1 },

            { L_,   "1..1",     0,                 ~0,                  ~1 },
            { L_,   "1..1",     1,                 ~0,                  ~2 },
            { L_,   "1..1", BPS-2,                 ~0,   ~(one64<<(BPS-2)) },
            { L_,   "1..1", BPS-1,                 ~0,            int64Max },

            { L_,  "1..10",     0,       g64("1..10"),        g64("1..10") },
            { L_,  "1..10",     1,       g64("1..10"),       g64("1..100") },
            { L_,  "1..10", BPS-2,       g64("1..10"),      g64("101..10") },
            { L_,  "1..10", BPS-1,       g64("1..10"),       g64("01..10") },

            { L_, "1..101",     0,      g64("1..101"),       g64("1..100") },
            { L_, "1..101",     1,      g64("1..101"),       g64("1..101") },
            { L_, "1..101", BPS-2,      g64("1..101"),     g64("101..101") },
            { L_, "1..101", BPS-1,      g64("1..101"),      g64("01..101") },

            { L_, "1..100",     0,      g64("1..100"),       g64("1..100") },
            { L_, "1..100",     1,      g64("1..100"),       g64("1..100") },
            { L_, "1..100", BPS-2,      g64("1..100"),     g64("101..100") },
            { L_, "1..100", BPS-1,      g64("1..100"),      g64("01..100") },

            { L_,  "01..1",     0,           int64Max,        int64Max - 1 },
            { L_,  "01..1",     1,           int64Max,        int64Max - 2 },
            { L_,  "01..1", BPS-2,           int64Max,        int64Max >>1 },
            { L_,  "01..1", BPS-1,           int64Max,            int64Max },

            { L_, "101..1",     0,      g64("101..1"),      g64("101..10") },
            { L_, "101..1",     1,      g64("101..1"),     g64("101..101") },
            { L_, "101..1", BPS-2,      g64("101..1"),       g64("101..1") },
            { L_, "101..1", BPS-1,      g64("101..1"),       g64("001..1") },

            { L_, "001..1",     0,      g64("001..1"),      g64("001..10") },
            { L_, "001..1",     1,      g64("001..1"),     g64("001..101") },
            { L_, "001..1", BPS-2,      g64("001..1"),       g64("001..1") },
            { L_, "001..1", BPS-1,      g64("001..1"),       g64("001..1") },

            { L_, "01..10",     0,       int64Max - 1,        int64Max - 1 },
            { L_, "01..10",     1,       int64Max - 1,        int64Max - 3 },
            { L_, "01..10", BPS-2,       int64Max - 1,      g64("001..10") },
            { L_, "01..10", BPS-1,       int64Max - 1,        int64Max - 1 },

            { L_,    SW_01,     0,         g64(SW_01),        g64(SW_01)^1 },
            { L_,    SW_01,     1,         g64(SW_01),          g64(SW_01) },
            { L_,    SW_01, BPS-2,  g64(SW_01),  g64(SW_01)^one64<<(BPS-2) },
            { L_,    SW_01, BPS-1,         g64(SW_01),          g64(SW_01) },

            { L_,    SW_10,     0,         g64(SW_10),          g64(SW_10) },
            { L_,    SW_10,     1,         g64(SW_10),        g64(SW_10)^2 },
            { L_,    SW_10, BPS-2,         g64(SW_10),          g64(SW_10) },
            { L_,    SW_10, BPS-1,         g64(SW_10), g64(SW_10)&int64Max },

            //        DSTA       g64("1101101100100") g64("1101101100100")
            { L_,     DSTA,    4,g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTA,    5,g64("1101101100100"),g64("1101101000100") },
            { L_,     DSTA,    6,g64("1101101100100"),g64("1101100100100") },
            { L_,     DSTA,    7,g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTA,    8,g64("1101101100100"),g64("1101001100100") },

            //        DSTB     g64("110110110010..0") g64("110110110010..0")
            { L_,     DSTB, 55,g64("110110110010..0"),g64("110110110010..0") },
            { L_,     DSTB, 56,g64("110110110010..0"),g64("110110100010..0") },
            { L_,     DSTB, 57,g64("110110110010..0"),g64("110110010010..0") },
            { L_,     DSTB, 58,g64("110110110010..0"),g64("110110110010..0") },
            { L_,     DSTB, 59,g64("110110110010..0"),g64("110100110010..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 src = g64(spec);
            Int64 dst0 = src;
            Util::minusEqual64(&dst0, DATA_B[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_minusEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_minusEqual0 == dst0);

            for (int si = 0, arg = 1; si < 20; ++si, arg = 23 * arg + 7) {
                Int64 dst1 = src;
                Util::minusEqual64(&dst1, DATA_B[di].d_index, arg);
                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(DATA_B[di].d_minusEqual1); P(dst1);
                }
                LOOP_ASSERT(LINE, DATA_B[di].d_minusEqual1 == dst1);
                if (DATA_B[di].d_minusEqual1 != dst1) {
                    break;
                }
            }
        }
}

static
void testD16()
{
        // --------------------------------------------------------------------
        // TESTING XOREQUAL FUNCTION:
        //   The three-parameter 'xorEqual' function has a relatively simple
        //   implementation that branches on the value of 'booleanValue', but
        //   otherwise relies on tested functionality.  A relatively small set
        //   of test data is sufficient, but we choose a more thorough and
        //   systematic set of test data to completely probe the algorithm and
        //   not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'xorEqual' sets the expected
        //   value.
        //
        // Testing:
        //   void xorEqual(int *dInt, int dIdx, int booleanValue);
        //   void xorEqual64(bsls_Types::Int64 *dInt,
        //                   int dIdx, int booleanValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing xorEqual Function" << endl
                          << "=========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_xorEqual0; // expected result from xorEqual function
            int         d_xorEqual1; // expected result from xorEqual function
        } DATA_A[] = {
            //    boolean value =                   0                     1
            //L#       dst  Index     xorEqual result       xorEqual result
            //--    ------  -----     ---------------       ---------------
            { L_,   "0..0",     0,                  0,                    1 },
            { L_,   "0..0",     1,                  0,                    2 },
            { L_,   "0..0", BPW-2,                  0,           1<<(BPW-2) },
            { L_,   "0..0", BPW-1,                  0,              INT_MIN },

            { L_,  "0..01",     0,                  1,                    0 },
            { L_,  "0..01",     1,                  1,                    3 },
            { L_,  "0..01", BPW-2,                  1,         g("010..01") },
            { L_,  "0..01", BPW-1,                  1,          INT_MIN + 1 },

            { L_, "0..010",     0,                  2,                    3 },
            { L_, "0..010",     1,                  2,                    0 },
            { L_, "0..010", BPW-2,                  2,        g("010..010") },
            { L_, "0..010", BPW-1,                  2,         g("10..010") },

            { L_, "0..011",     0,                  3,                    2 },
            { L_, "0..011",     1,                  3,                    1 },
            { L_, "0..011", BPW-2,                  3,        g("010..011") },
            { L_, "0..011", BPW-1,                  3,         g("10..011") },

            { L_,  "10..0",     0,            INT_MIN,          INT_MIN + 1 },
            { L_,  "10..0",     1,            INT_MIN,          INT_MIN + 2 },
            { L_,  "10..0", BPW-2,            INT_MIN,          g("110..0") },
            { L_,  "10..0", BPW-1,            INT_MIN,                    0 },

            { L_, "010..0",     0,        g("010..0"),         g("010..01") },
            { L_, "010..0",     1,        g("010..0"),        g("010..010") },
            { L_, "010..0", BPW-2,        g("010..0"),                    0 },
            { L_, "010..0", BPW-1,        g("010..0"),          g("110..0") },

            { L_, "110..0",     0,        g("110..0") ,        g("110..01") },
            { L_, "110..0",     1,        g("110..0"),        g("110..010") },
            { L_, "110..0", BPW-2,        g("110..0"),              INT_MIN },
            { L_, "110..0", BPW-1,        g("110..0"),          g("010..0") },

            { L_, "10..01",     0,        INT_MIN + 1,              INT_MIN },
            { L_, "10..01",     1,        INT_MIN + 1,          INT_MIN + 3 },
            { L_, "10..01", BPW-2,        INT_MIN + 1,         g("110..01") },
            { L_, "10..01", BPW-1,        INT_MIN + 1,                    1 },

            { L_,   "1..1",     0,                 ~0,                   ~1 },
            { L_,   "1..1",     1,                 ~0,                   ~2 },
            { L_,   "1..1", BPW-2,                 ~0,        ~(1<<(BPW-2)) },
            { L_,   "1..1", BPW-1,                 ~0,              INT_MAX },

            { L_,  "1..10",     0,                 ~1,                   ~0 },
            { L_,  "1..10",     1,                 ~1,          g("1..100") },
            { L_,  "1..10", BPW-2,                 ~1,         g("101..10") },
            { L_,  "1..10", BPW-1,                 ~1,          g("01..10") },

            { L_, "1..101",     0,        g("1..101"),          g("1..100") },
            { L_, "1..101",     1,        g("1..101"),                   ~0 },
            { L_, "1..101", BPW-2,        g("1..101"),        g("101..101") },
            { L_, "1..101", BPW-1,        g("1..101"),         g("01..101") },

            { L_, "1..100",     0,        g("1..100"),          g("1..101") },
            { L_, "1..100",     1,        g("1..100"),                   ~1 },
            { L_, "1..100", BPW-2,        g("1..100"),        g("101..100") },
            { L_, "1..100", BPW-1,        g("1..100"),         g("01..100") },

            { L_,  "01..1",     0,            INT_MAX,          INT_MAX - 1 },
            { L_,  "01..1",     1,            INT_MAX,          INT_MAX - 2 },
            { L_,  "01..1", BPW-2,            INT_MAX,          INT_MAX >>1 },
            { L_,  "01..1", BPW-1,            INT_MAX,                   ~0 },

            { L_, "101..1",     0,        g("101..1"),         g("101..10") },
            { L_, "101..1",     1,        g("101..1"),        g("101..101") },
            { L_, "101..1", BPW-2,        g("101..1"),                   ~0 },
            { L_, "101..1", BPW-1,        g("101..1"),          g("001..1") },

            { L_, "001..1",     0,        g("001..1"),         g("001..10") },
            { L_, "001..1",     1,        g("001..1"),        g("001..101") },
            { L_, "001..1", BPW-2,        g("001..1"),              INT_MAX },
            { L_, "001..1", BPW-1,        g("001..1"),          g("101..1") },

            { L_, "01..10",     0,        INT_MAX - 1,              INT_MAX },
            { L_, "01..10",     1,        INT_MAX - 1,          INT_MAX - 3 },
            { L_, "01..10", BPW-2,        INT_MAX - 1,         g("001..10") },
            { L_, "01..10", BPW-1,        INT_MAX - 1,                   ~1 },

            { L_,    FW_01,     0,           g(FW_01),           g(FW_01)^1 },
            { L_,    FW_01,     1,           g(FW_01),           g(FW_01)|2 },
            { L_,    FW_01, BPW-2,           g(FW_01),g(FW_01)^(1<<(BPW-2)) },
            { L_,    FW_01, BPW-1,           g(FW_01),     g(FW_01)|INT_MIN },

            { L_,    FW_10,     0,           g(FW_10),           g(FW_10)|1 },
            { L_,    FW_10,     1,           g(FW_10),           g(FW_10)^2 },
            { L_,    FW_10, BPW-2,           g(FW_10),g(FW_10)|(1<<(BPW-2)) },
            { L_,    FW_10, BPW-1,           g(FW_10),     g(FW_10)&INT_MAX },

            //        DSTA          g("1101101100100")   g("1101101100100")
            { L_,     DSTA,     4,  g("1101101100100"),  g("1101101110100") },
            { L_,     DSTA,     5,  g("1101101100100"),  g("1101101000100") },
            { L_,     DSTA,     6,  g("1101101100100"),  g("1101100100100") },
            { L_,     DSTA,     7,  g("1101101100100"),  g("1101111100100") },
            { L_,     DSTA,     8,  g("1101101100100"),  g("1101001100100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst0 = g(spec);
            int dst1 = dst0;
            Util::xorEqual(&dst0, DATA_A[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_xorEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_xorEqual0 == dst0);

            Util::xorEqual(&dst1, DATA_A[di].d_index, 1);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_xorEqual1); P(dst1);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_xorEqual1 == dst1);
        }

        if (verbose) cout << endl
                          << "Testing xorEqual64 Function" << endl
                          << "===========================" << endl;

        const Int64 one64 = 1;
        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            Int64       d_xorEqual0; // expected result from xorEqual function
            Int64       d_xorEqual1; // expected result from xorEqual function
        } DATA_B[] = {
            //    boolean value =                   0                     1
            //L#       dst  Index     xorEqual result       xorEqual result
            //--    ------  -----     ---------------       ---------------
            { L_,   "0..0",     0,                  0,                    1 },
            { L_,   "0..0",     1,                  0,                    2 },
            { L_,   "0..0", BPS-2,                  0,       one64<<(BPS-2) },
            { L_,   "0..0", BPS-1,                  0,             int64Min },

            { L_,  "0..01",     0,                  1,                    0 },
            { L_,  "0..01",     1,                  1,                    3 },
            { L_,  "0..01", BPS-2,                  1,       g64("010..01") },
            { L_,  "0..01", BPS-1,                  1,         int64Min + 1 },

            { L_, "0..010",     0,                  2,                    3 },
            { L_, "0..010",     1,                  2,                    0 },
            { L_, "0..010", BPS-2,                  2,      g64("010..010") },
            { L_, "0..010", BPS-1,                  2,       g64("10..010") },

            { L_, "0..011",     0,                  3,                    2 },
            { L_, "0..011",     1,                  3,                    1 },
            { L_, "0..011", BPS-2,                  3,      g64("010..011") },
            { L_, "0..011", BPS-1,                  3,       g64("10..011") },

            { L_,  "10..0",     0,           int64Min,         int64Min + 1 },
            { L_,  "10..0",     1,           int64Min,         int64Min + 2 },
            { L_,  "10..0", BPS-2,           int64Min,        g64("110..0") },
            { L_,  "10..0", BPS-1,           int64Min,                    0 },

            { L_, "010..0",     0,      g64("010..0"),       g64("010..01") },
            { L_, "010..0",     1,      g64("010..0"),      g64("010..010") },
            { L_, "010..0", BPS-2,      g64("010..0"),                    0 },
            { L_, "010..0", BPS-1,      g64("010..0"),        g64("110..0") },

            { L_, "110..0",     0,      g64("110..0") ,      g64("110..01") },
            { L_, "110..0",     1,      g64("110..0"),      g64("110..010") },
            { L_, "110..0", BPS-2,      g64("110..0"),             int64Min },
            { L_, "110..0", BPS-1,      g64("110..0"),        g64("010..0") },

            { L_, "10..01",     0,       int64Min + 1,             int64Min },
            { L_, "10..01",     1,       int64Min + 1,         int64Min + 3 },
            { L_, "10..01", BPS-2,       int64Min + 1,       g64("110..01") },
            { L_, "10..01", BPS-1,       int64Min + 1,                    1 },

            { L_,   "1..1",     0,                 ~0,                   ~1 },
            { L_,   "1..1",     1,                 ~0,                   ~2 },
            { L_,   "1..1", BPS-2,                 ~0,    ~(one64<<(BPS-2)) },
            { L_,   "1..1", BPS-1,                 ~0,             int64Max },

            { L_,  "1..10",     0,                 ~1,                   ~0 },
            { L_,  "1..10",     1,                 ~1,        g64("1..100") },
            { L_,  "1..10", BPS-2,                 ~1,       g64("101..10") },
            { L_,  "1..10", BPS-1,                 ~1,        g64("01..10") },

            { L_, "1..101",     0,      g64("1..101"),        g64("1..100") },
            { L_, "1..101",     1,      g64("1..101"),                   ~0 },
            { L_, "1..101", BPS-2,      g64("1..101"),      g64("101..101") },
            { L_, "1..101", BPS-1,      g64("1..101"),       g64("01..101") },

            { L_, "1..100",     0,      g64("1..100"),        g64("1..101") },
            { L_, "1..100",     1,      g64("1..100"),                   ~1 },
            { L_, "1..100", BPS-2,      g64("1..100"),      g64("101..100") },
            { L_, "1..100", BPS-1,      g64("1..100"),       g64("01..100") },

            { L_,  "01..1",     0,           int64Max,         int64Max - 1 },
            { L_,  "01..1",     1,           int64Max,         int64Max - 2 },
            { L_,  "01..1", BPS-2,           int64Max,         int64Max >>1 },
            { L_,  "01..1", BPS-1,           int64Max,                   ~0 },

            { L_, "101..1",     0,      g64("101..1"),       g64("101..10") },
            { L_, "101..1",     1,      g64("101..1"),      g64("101..101") },
            { L_, "101..1", BPS-2,      g64("101..1"),                   ~0 },
            { L_, "101..1", BPS-1,      g64("101..1"),        g64("001..1") },

            { L_, "001..1",     0,      g64("001..1"),       g64("001..10") },
            { L_, "001..1",     1,      g64("001..1"),      g64("001..101") },
            { L_, "001..1", BPS-2,      g64("001..1"),             int64Max },
            { L_, "001..1", BPS-1,      g64("001..1"),        g64("101..1") },

            { L_, "01..10",     0,       int64Max - 1,             int64Max },
            { L_, "01..10",     1,       int64Max - 1,         int64Max - 3 },
            { L_, "01..10", BPS-2,       int64Max - 1,       g64("001..10") },
            { L_, "01..10", BPS-1,       int64Max - 1,                   ~1 },

            { L_,    SW_01,     0,         g64(SW_01),         g64(SW_01)^1 },
            { L_,    SW_01,     1,         g64(SW_01),         g64(SW_01)|2 },
            { L_,    SW_01, BPS-2,  g64(SW_01), g64(SW_01)^(one64<<(BPS-2)) },
            { L_,    SW_01, BPS-1,         g64(SW_01),  g64(SW_01)|int64Min },

            { L_,    SW_10,     0,         g64(SW_10),         g64(SW_10)|1 },
            { L_,    SW_10,     1,         g64(SW_10),         g64(SW_10)^2 },
            { L_,    SW_10, BPS-2,  g64(SW_10), g64(SW_10)|(one64<<(BPS-2)) },
            { L_,    SW_10, BPS-1,         g64(SW_10),  g64(SW_10)&int64Max },

            //        DSTA        g64("1101101100100") g64("1101101100100")
            { L_,     DSTA,     4,g64("1101101100100"),g64("1101101110100") },
            { L_,     DSTA,     5,g64("1101101100100"),g64("1101101000100") },
            { L_,     DSTA,     6,g64("1101101100100"),g64("1101100100100") },
            { L_,     DSTA,     7,g64("1101101100100"),g64("1101111100100") },
            { L_,     DSTA,     8,g64("1101101100100"),g64("1101001100100") },

            //        DSTB     g64("110110110010..0") g64("110110110010..0")
            { L_,     DSTB, 55,g64("110110110010..0"),g64("110110111010..0") },
            { L_,     DSTB, 56,g64("110110110010..0"),g64("110110100010..0") },
            { L_,     DSTB, 57,g64("110110110010..0"),g64("110110010010..0") },
            { L_,     DSTB, 58,g64("110110110010..0"),g64("110111110010..0") },
            { L_,     DSTB, 59,g64("110110110010..0"),g64("110100110010..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 src = g64(spec);
            Int64 dst0 = src;
            Util::xorEqual64(&dst0, DATA_B[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_xorEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_xorEqual0 == dst0);

            for (int si = 0, arg = 1; si < 20; ++si, arg = 23 * arg + 7) {
                Int64 dst1 = src;
                Util::xorEqual64(&dst1, DATA_B[di].d_index, arg);
                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(DATA_B[di].d_xorEqual1); P(dst1);
                }
                LOOP_ASSERT(LINE, DATA_B[di].d_xorEqual1 == dst1);
            }
        }
}

static
void testD(int test)
{
    switch (test) {
      case 20: {
        testD20();
      } break;
      case 19: {
        testD19();
      } break;
      case 18: {
        testD18();
      } break;
      case 17: {
        testD17();
      } break;
      case 16: {
        testD16();
      } break;
      default : {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testC(int test)
{
    switch (test) {
      case 15: {
        // --------------------------------------------------------------------
        // TESTING OREQUAL FUNCTION:
        //   The three-parameter 'orEqual' function has a relatively simple
        //   implementation that branches on the value of 'booleanValue', but
        //   otherwise relies on tested functionality.  A relatively small set
        //   of test data is sufficient, but we choose a more thorough and
        //   systematic set of test data to completely probe the algorithm and
        //   not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'orEqual' sets the expected
        //   value.
        //
        // Testing:
        //   void orEqual(int *dInt, int dIdx, int booleanValue);
        //   void orEqual64(bsls_Types::Int64 *dInt,
        //                  int dIdx, int booleanValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing orEqual Function" << endl
                          << "========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_orEqual0;  // expected result from orEqual(..., 0)
            int         d_orEqual1;  // expected result from orEqual(..., 1)
        } DATA_A[] = {
            //      boolean value =                 0                    1
            //L#       dst  Index      orEqual result       orEqual result
            //--    ------  -----      --------------       --------------
            { L_,   "0..0",     0,                  0,                   1 },
            { L_,   "0..0",     1,                  0,                   2 },
            { L_,   "0..0", BPW-2,                  0,          1<<(BPW-2) },
            { L_,   "0..0", BPW-1,                  0,             INT_MIN },

            { L_,  "0..01",     0,         g("0..01"),          g("0..01") },
            { L_,  "0..01",     1,         g("0..01"),         g("0..011") },
            { L_,  "0..01", BPW-2,         g("0..01"),        g("010..01") },
            { L_,  "0..01", BPW-1,         g("0..01"),         g("10..01") },

            { L_, "0..010",     0,        g("0..010"),         g("0..011") },
            { L_, "0..010",     1,        g("0..010"),         g("0..010") },
            { L_, "0..010", BPW-2,        g("0..010"),       g("010..010") },
            { L_, "0..010", BPW-1,        g("0..010"),        g("10..010") },

            { L_, "0..011",     0,        g("0..011"),         g("0..011") },
            { L_, "0..011",     1,        g("0..011"),         g("0..011") },
            { L_, "0..011", BPW-2,        g("0..011"),       g("010..011") },
            { L_, "0..011", BPW-1,        g("0..011"),        g("10..011") },

            { L_,  "10..0",     0,         g("10..0"),         g("10..01") },
            { L_,  "10..0",     1,         g("10..0"),        g("10..010") },
            { L_,  "10..0", BPW-2,         g("10..0"),         g("110..0") },
            { L_,  "10..0", BPW-1,         g("10..0"),          g("10..0") },

            { L_, "010..0",     0,        g("010..0"),        g("010..01") },
            { L_, "010..0",     1,        g("010..0"),       g("010..010") },
            { L_, "010..0", BPW-2,        g("010..0"),         g("010..0") },
            { L_, "010..0", BPW-1,        g("010..0"),         g("110..0") },

            { L_, "110..0",     0,        g("110..0"),        g("110..01") },
            { L_, "110..0",     1,        g("110..0"),       g("110..010") },
            { L_, "110..0", BPW-2,        g("110..0"),         g("110..0") },
            { L_, "110..0", BPW-1,        g("110..0"),         g("110..0") },

            { L_, "10..01",     0,        g("10..01"),         g("10..01") },
            { L_, "10..01",     1,        g("10..01"),        g("10..011") },
            { L_, "10..01", BPW-2,        g("10..01"),        g("110..01") },
            { L_, "10..01", BPW-1,        g("10..01"),         g("10..01") },

            { L_,   "1..1",     0,                 ~0,                  ~0 },
            { L_,   "1..1",     1,                 ~0,                  ~0 },
            { L_,   "1..1", BPW-2,                 ~0,                  ~0 },
            { L_,   "1..1", BPW-1,                 ~0,                  ~0 },

            { L_,  "1..10",     0,         g("1..10"),                  ~0 },
            { L_,  "1..10",     1,         g("1..10"),          g("1..10") },
            { L_,  "1..10", BPW-2,         g("1..10"),          g("1..10") },
            { L_,  "1..10", BPW-1,         g("1..10"),          g("1..10") },

            { L_, "1..101",     0,        g("1..101"),         g("1..101") },
            { L_, "1..101",     1,        g("1..101"),                  ~0 },
            { L_, "1..101", BPW-2,        g("1..101"),         g("1..101") },
            { L_, "1..101", BPW-1,        g("1..101"),         g("1..101") },

            { L_, "1..100",     0,        g("1..100"),         g("1..101") },
            { L_, "1..100",     1,        g("1..100"),          g("1..10") },
            { L_, "1..100", BPW-2,        g("1..100"),         g("1..100") },
            { L_, "1..100", BPW-1,        g("1..100"),         g("1..100") },

            { L_,  "01..1",     0,         g("01..1"),          g("01..1") },
            { L_,  "01..1",     1,         g("01..1"),          g("01..1") },
            { L_,  "01..1", BPW-2,         g("01..1"),          g("01..1") },
            { L_,  "01..1", BPW-1,         g("01..1"),           g("1..1") },

            { L_, "101..1",     0,        g("101..1"),         g("101..1") },
            { L_, "101..1",     1,        g("101..1"),         g("101..1") },
            { L_, "101..1", BPW-2,        g("101..1"),                  ~0 },
            { L_, "101..1", BPW-1,        g("101..1"),         g("101..1") },

            { L_, "001..1",     0,        g("001..1"),         g("001..1") },
            { L_, "001..1",     1,        g("001..1"),         g("001..1") },
            { L_, "001..1", BPW-2,        g("001..1"),            INT_MAX  },
            { L_, "001..1", BPW-1,        g("001..1"),         g("101..1") },

            { L_, "01..10",     0,        g("01..10"),          g("01..1") },
            { L_, "01..10",     1,        g("01..10"),         g("01..10") },
            { L_, "01..10", BPW-2,        g("01..10"),         g("01..10") },
            { L_, "01..10", BPW-1,        g("01..10"),          g("1..10") },

            { L_,    FW_01,     0,           g(FW_01),            g(FW_01) },
            { L_,    FW_01,     1,           g(FW_01),          g(FW_01)+2 },
            { L_,    FW_01, BPW-2,           g(FW_01),            g(FW_01) },
            { L_,    FW_01, BPW-1,           g(FW_01),    g(FW_01)|INT_MIN },

            { L_,    FW_10,     0,           g(FW_10),          g(FW_10)|1 },
            { L_,    FW_10,     1,           g(FW_10),            g(FW_10) },
            { L_,    FW_10, BPW-2,           g(FW_10), g(FW_10)^1<<(BPW-2) },
            { L_,    FW_10, BPW-1,           g(FW_10),            g(FW_10) },

            //        DSTA         g("1101101100100")   g("1101101100100")
            { L_,     DSTA,     4, g("1101101100100"),  g("1101101110100") },
            { L_,     DSTA,     5, g("1101101100100"),  g("1101101100100") },
            { L_,     DSTA,     6, g("1101101100100"),  g("1101101100100") },
            { L_,     DSTA,     7, g("1101101100100"),  g("1101111100100") },
            { L_,     DSTA,     8, g("1101101100100"),  g("1101101100100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int src = g(spec);
            int dst0 = src;
            Util::orEqual(&dst0, DATA_A[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_orEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_orEqual0 == dst0);

            for (int si = 0, arg = 1; si < 20; ++si, arg = arg * 23 + 7) {
                int dst1 = src;
                Util::orEqual(&dst1, DATA_A[di].d_index, 1);
                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(DATA_A[di].d_orEqual1); P(dst1);
                }
                LOOP_ASSERT(LINE, DATA_A[di].d_orEqual1 == dst1);
            }
        }

        if (verbose) cout << endl
                          << "Testing orEqual64 Function" << endl
                          << "==========================" << endl;

        const Int64 one64 = 1;
        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            Int64        d_orEqual0;  // expected result from orEqual(..., 0)
            Int64        d_orEqual1;  // expected result from orEqual(..., 1)
        } DATA_B[] = {
            //      boolean value =                 0                    1
            //L#       dst  Index      orEqual result       orEqual result
            //--    ------  -----      --------------       --------------
            { L_,   "0..0",     0,                  0,                   1 },
            { L_,   "0..0",     1,                  0,                   2 },
            { L_,   "0..0", BPS-2,                  0,      one64<<(BPS-2) },
            { L_,   "0..0", BPS-1,                  0,            int64Min },

            { L_,  "0..01",     0,       g64("0..01"),        g64("0..01") },
            { L_,  "0..01",     1,       g64("0..01"),       g64("0..011") },
            { L_,  "0..01", BPS-2,       g64("0..01"),      g64("010..01") },
            { L_,  "0..01", BPS-1,       g64("0..01"),       g64("10..01") },

            { L_, "0..010",     0,      g64("0..010"),       g64("0..011") },
            { L_, "0..010",     1,      g64("0..010"),       g64("0..010") },
            { L_, "0..010", BPS-2,      g64("0..010"),     g64("010..010") },
            { L_, "0..010", BPS-1,      g64("0..010"),      g64("10..010") },

            { L_, "0..011",     0,      g64("0..011"),       g64("0..011") },
            { L_, "0..011",     1,      g64("0..011"),       g64("0..011") },
            { L_, "0..011", BPS-2,      g64("0..011"),     g64("010..011") },
            { L_, "0..011", BPS-1,      g64("0..011"),      g64("10..011") },

            { L_,  "10..0",     0,       g64("10..0"),       g64("10..01") },
            { L_,  "10..0",     1,       g64("10..0"),      g64("10..010") },
            { L_,  "10..0", BPS-2,       g64("10..0"),       g64("110..0") },
            { L_,  "10..0", BPS-1,       g64("10..0"),        g64("10..0") },

            { L_, "010..0",     0,      g64("010..0"),      g64("010..01") },
            { L_, "010..0",     1,      g64("010..0"),     g64("010..010") },
            { L_, "010..0", BPS-2,      g64("010..0"),       g64("010..0") },
            { L_, "010..0", BPS-1,      g64("010..0"),       g64("110..0") },

            { L_, "110..0",     0,      g64("110..0"),      g64("110..01") },
            { L_, "110..0",     1,      g64("110..0"),     g64("110..010") },
            { L_, "110..0", BPS-2,      g64("110..0"),       g64("110..0") },
            { L_, "110..0", BPS-1,      g64("110..0"),       g64("110..0") },

            { L_, "10..01",     0,      g64("10..01"),       g64("10..01") },
            { L_, "10..01",     1,      g64("10..01"),      g64("10..011") },
            { L_, "10..01", BPS-2,      g64("10..01"),      g64("110..01") },
            { L_, "10..01", BPS-1,      g64("10..01"),       g64("10..01") },

            { L_,   "1..1",     0,                 ~0,                  ~0 },
            { L_,   "1..1",     1,                 ~0,                  ~0 },
            { L_,   "1..1", BPS-2,                 ~0,                  ~0 },
            { L_,   "1..1", BPS-1,                 ~0,                  ~0 },

            { L_,  "1..10",     0,       g64("1..10"),                  ~0 },
            { L_,  "1..10",     1,       g64("1..10"),        g64("1..10") },
            { L_,  "1..10", BPS-2,       g64("1..10"),        g64("1..10") },
            { L_,  "1..10", BPS-1,       g64("1..10"),        g64("1..10") },

            { L_, "1..101",     0,      g64("1..101"),       g64("1..101") },
            { L_, "1..101",     1,      g64("1..101"),                  ~0 },
            { L_, "1..101", BPS-2,      g64("1..101"),       g64("1..101") },
            { L_, "1..101", BPS-1,      g64("1..101"),       g64("1..101") },

            { L_, "1..100",     0,      g64("1..100"),       g64("1..101") },
            { L_, "1..100",     1,      g64("1..100"),        g64("1..10") },
            { L_, "1..100", BPS-2,      g64("1..100"),       g64("1..100") },
            { L_, "1..100", BPS-1,      g64("1..100"),       g64("1..100") },

            { L_,  "01..1",     0,       g64("01..1"),        g64("01..1") },
            { L_,  "01..1",     1,       g64("01..1"),        g64("01..1") },
            { L_,  "01..1", BPS-2,       g64("01..1"),        g64("01..1") },
            { L_,  "01..1", BPS-1,       g64("01..1"),         g64("1..1") },

            { L_, "101..1",     0,      g64("101..1"),       g64("101..1") },
            { L_, "101..1",     1,      g64("101..1"),       g64("101..1") },
            { L_, "101..1", BPS-2,      g64("101..1"),                  ~0 },
            { L_, "101..1", BPS-1,      g64("101..1"),       g64("101..1") },

            { L_, "001..1",     0,      g64("001..1"),       g64("001..1") },
            { L_, "001..1",     1,      g64("001..1"),       g64("001..1") },
            { L_, "001..1", BPS-2,      g64("001..1"),            int64Max },
            { L_, "001..1", BPS-1,      g64("001..1"),       g64("101..1") },

            { L_, "01..10",     0,      g64("01..10"),        g64("01..1") },
            { L_, "01..10",     1,      g64("01..10"),       g64("01..10") },
            { L_, "01..10", BPS-2,      g64("01..10"),       g64("01..10") },
            { L_, "01..10", BPS-1,      g64("01..10"),        g64("1..10") },

            { L_,    SW_01,     0,         g64(SW_01),          g64(SW_01) },
            { L_,    SW_01,     1,         g64(SW_01),        g64(SW_01)+2 },
            { L_,    SW_01, BPS-2,         g64(SW_01),          g64(SW_01) },
            { L_,    SW_01, BPS-1,         g64(SW_01), g64(SW_01)|int64Min },

            { L_,    SW_10,     0,         g64(SW_10),        g64(SW_10)|1 },
            { L_,    SW_10,     1,         g64(SW_10),          g64(SW_10) },
            { L_,    SW_10, BPS-2,  g64(SW_10),  g64(SW_10)^one64<<(BPS-2) },
            { L_,    SW_10, BPS-1,         g64(SW_10),          g64(SW_10) },

            //        DSTA       g64("1101101100100") g64("1101101100100")
            { L_,     DSTA,   4, g64("1101101100100"),g64("1101101110100") },
            { L_,     DSTA,   5, g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTA,   6, g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTA,   7, g64("1101101100100"),g64("1101111100100") },
            { L_,     DSTA,   8, g64("1101101100100"),g64("1101101100100") },

            //L#       dst Idx      orEqual0 result       orEqual1 result
            //--    ------ ---      ---------------       ---------------

            //        DSTB     g64("110110110010..0") g64("110110110010..0")
            { L_,     DSTB, 55,g64("110110110010..0"),g64("110110111010..0")},
            { L_,     DSTB, 56,g64("110110110010..0"),g64("110110110010..0")},
            { L_,     DSTB, 57,g64("110110110010..0"),g64("110110110010..0")},
            { L_,     DSTB, 58,g64("110110110010..0"),g64("110111110010..0")},
            { L_,     DSTB, 59,g64("110110110010..0"),g64("110110110010..0")}
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 src = g64(spec);
            Int64 dst0 = src;
            Util::orEqual64(&dst0, DATA_B[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_orEqual0); P(dst0);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_orEqual0 == dst0);

            for (int si = 0, arg = 1; si < 20; ++si, arg = arg * 23 + 7) {
                Int64 dst1 = src;
                Util::orEqual64(&dst1, DATA_B[di].d_index, arg);
                if (veryVerbose) {
                    P_(LINE); P_(spec); P_(DATA_B[di].d_orEqual1); P(dst1);
                }
                LOOP_ASSERT(LINE, DATA_B[di].d_orEqual1 == dst1);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ANDEQUAL FUNCTION:
        //   The three-parameter 'andEqual' function has a relatively simple
        //   implementation that branches on the value of 'booleanValue', but
        //   otherwise relies on tested functionality.  A relatively small set
        //   of test data is sufficient, but we choose a more thorough and
        //   systematic set of test data to completely probe the algorithm and
        //   not just the implementation.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'andEqual' sets the expected
        //   value.
        //
        // Testing:
        //   void andEqual(  int *dInt, int dIdx, int booleanValue);
        //   void andEqual64(int *dInt, int dIdx, int booleanValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing andEqual Function" << endl
                          << "=========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_andEqual0;   // expected result from andEqual(..., 0)
            int         d_andEqual1;   // expected result from andEqual(..., 1)
        } DATA_A[] = {
            //    boolean value =                    0                   1
            //L#     dst  Index        andEqual result     andEqual result
            //--  ------  -----        ---------------     ---------------
            { L_,   "0..0",     0,                   0,                  0 },
            { L_,   "0..0",     1,                   0,                  0 },
            { L_,   "0..0", BPW-2,                   0,                  0 },
            { L_,   "0..0", BPW-1,                   0,                  0 },

            { L_,  "0..01",     0,                   0,                  1 },
            { L_,  "0..01",     1,                   1,                  1 },
            { L_,  "0..01", BPW-2,                   1,                  1 },
            { L_,  "0..01", BPW-1,                   1,                  1 },

            { L_, "0..010",     0,                   2,                  2 },
            { L_, "0..010",     1,                   0,                  2 },
            { L_, "0..010", BPW-2,                   2,                  2 },
            { L_, "0..010", BPW-1,                   2,                  2 },

            { L_, "0..011",     0,                   2,                  3 },
            { L_, "0..011",     1,                   1,                  3 },
            { L_, "0..011", BPW-2,                   3,                  3 },
            { L_, "0..011", BPW-1,                   3,                  3 },

            { L_,  "10..0",     0,          g("10..0"),         g("10..0") },
            { L_,  "10..0",     1,          g("10..0"),         g("10..0") },
            { L_,  "10..0", BPW-2,          g("10..0"),         g("10..0") },
            { L_,  "10..0", BPW-1,                   0,         g("10..0") },

            { L_, "010..0",     0,         g("010..0"),        g("010..0") },
            { L_, "010..0",     1,         g("010..0"),        g("010..0") },
            { L_, "010..0", BPW-2,                   0,        g("010..0") },
            { L_, "010..0", BPW-1,         g("010..0"),        g("010..0") },

            { L_, "110..0",     0,         g("110..0"),        g("110..0") },
            { L_, "110..0",     1,         g("110..0"),        g("110..0") },
            { L_, "110..0", BPW-2,             INT_MIN,        g("110..0") },
            { L_, "110..0", BPW-1,         g("010..0"),        g("110..0") },

            { L_, "10..01",     0,          g("10..0"),        g("10..01") },
            { L_, "10..01",     1,         g("10..01"),        g("10..01") },
            { L_, "10..01", BPW-2,         g("10..01"),        g("10..01") },
            { L_, "10..01", BPW-1,                   1,        g("10..01") },

            { L_,   "1..1",     0,                  ~1,                 ~0 },
            { L_,   "1..1",     1,                  ~2,                 ~0 },
            { L_,   "1..1", BPW-2,       ~0^1<<(BPW-2),                 ~0 },
            { L_,   "1..1", BPW-1,             INT_MAX,                 ~0 },

            { L_,  "1..10",     0,                  ~1,                 ~1 },
            { L_,  "1..10",     1,         g("1..100"),                 ~1 },
            { L_,  "1..10", BPW-2,        g("101..10"),                 ~1 },
            { L_,  "1..10", BPW-1,         g("01..10"),                 ~1 },

            { L_, "1..101",     0,         g("1..100"),        g("1..101") },
            { L_, "1..101",     1,         g("1..101"),        g("1..101") },
            { L_, "1..101", BPW-2,       g("101..101"),        g("1..101") },
            { L_, "1..101", BPW-1,        g("01..101"),        g("1..101") },

            { L_, "1..100",     0,         g("1..100"),        g("1..100") },
            { L_, "1..100",     1,         g("1..100"),        g("1..100") },
            { L_, "1..100", BPW-2,       g("101..100"),        g("1..100") },
            { L_, "1..100", BPW-1,        g("01..100"),        g("1..100") },

            { L_,  "01..1",     0,         g("01..10"),         g("01..1") },
            { L_,  "01..1",     1,        g("01..101"),         g("01..1") },
            { L_,  "01..1", BPW-2,         g("001..1"),         g("01..1") },
            { L_,  "01..1", BPW-1,          g("01..1"),         g("01..1") },

            { L_, "101..1",     0,        g("101..10"),        g("101..1") },
            { L_, "101..1",     1,       g("101..101"),        g("101..1") },
            { L_, "101..1", BPW-2,         g("101..1"),        g("101..1") },
            { L_, "101..1", BPW-1,         g("001..1"),        g("101..1") },

            { L_, "001..1",     0,        g("001..10"),        g("001..1") },
            { L_, "001..1",     1,       g("001..101"),        g("001..1") },
            { L_, "001..1", BPW-2,         g("001..1"),        g("001..1") },
            { L_, "001..1", BPW-1,         g("001..1"),        g("001..1") },

            { L_, "01..10",     0,         g("01..10"),        g("01..10") },
            { L_, "01..10",     1,        g("01..100"),        g("01..10") },
            { L_, "01..10", BPW-2,        g("001..10"),        g("01..10") },
            { L_, "01..10", BPW-1,         g("01..10"),        g("01..10") },

            { L_,    FW_01,     0,          g(FW_01)^1,           g(FW_01) },
            { L_,    FW_01,     1,            g(FW_01),           g(FW_01) },
            { L_,    FW_01, BPW-2, g(FW_01)^1<<(BPW-2),           g(FW_01) },
            { L_,    FW_01, BPW-1,            g(FW_01),           g(FW_01) },

            { L_,    FW_10,     0,            g(FW_10),           g(FW_10) },
            { L_,    FW_10,     1,          g(FW_10)^2,           g(FW_10) },
            { L_,    FW_10, BPW-2,            g(FW_10),           g(FW_10) },
            { L_,    FW_10, BPW-1,    g(FW_10)^INT_MIN,           g(FW_10) },

            //        DSTA      =   g("1101101100100")  g("1101101100100")
            { L_,     DSTA,     4,  g("1101101100100"), g("1101101100100") },
            { L_,     DSTA,     5,  g("1101101000100"), g("1101101100100") },
            { L_,     DSTA,     6,  g("1101100100100"), g("1101101100100") },
            { L_,     DSTA,     7,  g("1101101100100"), g("1101101100100") },
            { L_,     DSTA,     8,  g("1101001100100"), g("1101101100100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int src = g(spec);
            int dst = src;
            Util::andEqual(&dst, DATA_A[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_andEqual0); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_andEqual0 == dst);

            dst = src;

            Util::andEqual(&dst, DATA_A[di].d_index, 1);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_andEqual1); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_andEqual1 == dst);
            LOOP_ASSERT(LINE, src == dst);
        }

        if (verbose) cout << endl
                          << "Testing andEqual64 Function" << endl
                          << "===========================" << endl;

        const char *DSTB = "1101101100100";    // typical case
        const char *DSTC = "1101101100..0";    // typical case
        const Int64 one64 = 1;
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            Int64       d_andEqual0;   // expected result from andEqual(..., 0)
            Int64       d_andEqual1;   // expected result from andEqual(..., 1)
        } DATA_B[] = {
            //    boolean value =                    0                   1
            //L#     dst  Index        andEqual result     andEqual result
            //--  ------  -----        ---------------     ---------------
            { L_,   "0..0",     0,                   0,                  0 },
            { L_,   "0..0",     1,                   0,                  0 },
            { L_,   "0..0", BPS-2,                   0,                  0 },
            { L_,   "0..0", BPS-1,                   0,                  0 },

            { L_,  "0..01",     0,                   0,                  1 },
            { L_,  "0..01",     1,                   1,                  1 },
            { L_,  "0..01", BPS-2,                   1,                  1 },
            { L_,  "0..01", BPS-1,                   1,                  1 },

            { L_, "0..010",     0,                   2,                  2 },
            { L_, "0..010",     1,                   0,                  2 },
            { L_, "0..010", BPS-2,                   2,                  2 },
            { L_, "0..010", BPS-1,                   2,                  2 },

            { L_, "0..011",     0,                   2,                  3 },
            { L_, "0..011",     1,                   1,                  3 },
            { L_, "0..011", BPS-2,                   3,                  3 },
            { L_, "0..011", BPS-1,                   3,                  3 },

            { L_,  "10..0",     0,        g64("10..0"),       g64("10..0") },
            { L_,  "10..0",     1,        g64("10..0"),       g64("10..0") },
            { L_,  "10..0", BPS-2,        g64("10..0"),       g64("10..0") },
            { L_,  "10..0", BPS-1,                   0,       g64("10..0") },

            { L_, "010..0",     0,       g64("010..0"),      g64("010..0") },
            { L_, "010..0",     1,       g64("010..0"),      g64("010..0") },
            { L_, "010..0", BPS-2,                   0,      g64("010..0") },
            { L_, "010..0", BPS-1,       g64("010..0"),      g64("010..0") },

            { L_, "110..0",     0,       g64("110..0"),      g64("110..0") },
            { L_, "110..0",     1,       g64("110..0"),      g64("110..0") },
            { L_, "110..0", BPS-2,            int64Min,      g64("110..0") },
            { L_, "110..0", BPS-1,       g64("010..0"),      g64("110..0") },

            { L_, "10..01",     0,        g64("10..0"),      g64("10..01") },
            { L_, "10..01",     1,       g64("10..01"),      g64("10..01") },
            { L_, "10..01", BPS-2,       g64("10..01"),      g64("10..01") },
            { L_, "10..01", BPS-1,                   1,      g64("10..01") },

            { L_,   "1..1",     0,                  ~1,                 ~0 },
            { L_,   "1..1",     1,                  ~2,                 ~0 },
            { L_,   "1..1", BPS-2,   ~(one64<<(BPS-2)),                 ~0 },
            { L_,   "1..1", BPS-1,            int64Max,                 ~0 },

            { L_,  "1..10",     0,                  ~1,                 ~1 },
            { L_,  "1..10",     1,       g64("1..100"),                 ~1 },
            { L_,  "1..10", BPS-2,      g64("101..10"),                 ~1 },
            { L_,  "1..10", BPS-1,       g64("01..10"),                 ~1 },

            { L_, "1..101",     0,       g64("1..100"),      g64("1..101") },
            { L_, "1..101",     1,       g64("1..101"),      g64("1..101") },
            { L_, "1..101", BPS-2,     g64("101..101"),      g64("1..101") },
            { L_, "1..101", BPS-1,      g64("01..101"),      g64("1..101") },

            { L_, "1..100",     0,       g64("1..100"),      g64("1..100") },
            { L_, "1..100",     1,       g64("1..100"),      g64("1..100") },
            { L_, "1..100", BPS-2,     g64("101..100"),      g64("1..100") },
            { L_, "1..100", BPS-1,      g64("01..100"),      g64("1..100") },

            { L_,  "01..1",     0,       g64("01..10"),       g64("01..1") },
            { L_,  "01..1",     1,      g64("01..101"),       g64("01..1") },
            { L_,  "01..1", BPS-2,       g64("001..1"),       g64("01..1") },
            { L_,  "01..1", BPS-1,        g64("01..1"),       g64("01..1") },

            { L_, "101..1",     0,      g64("101..10"),      g64("101..1") },
            { L_, "101..1",     1,     g64("101..101"),      g64("101..1") },
            { L_, "101..1", BPS-2,       g64("101..1"),      g64("101..1") },
            { L_, "101..1", BPS-1,       g64("001..1"),      g64("101..1") },

            { L_, "001..1",     0,      g64("001..10"),      g64("001..1") },
            { L_, "001..1",     1,     g64("001..101"),      g64("001..1") },
            { L_, "001..1", BPS-2,       g64("001..1"),      g64("001..1") },
            { L_, "001..1", BPS-1,       g64("001..1"),      g64("001..1") },

            { L_, "01..10",     0,       g64("01..10"),      g64("01..10") },
            { L_, "01..10",     1,      g64("01..100"),      g64("01..10") },
            { L_, "01..10", BPS-2,      g64("001..10"),      g64("01..10") },
            { L_, "01..10", BPS-1,       g64("01..10"),      g64("01..10") },

            { L_,    SW_01,     0,        g64(SW_01)^1,         g64(SW_01) },
            { L_,    SW_01,     1,          g64(SW_01),         g64(SW_01) },
            { L_,    SW_01, BPS-2,g64(SW_01)^one64<<(BPS-2),    g64(SW_01) },
            { L_,    SW_01, BPS-1,          g64(SW_01),         g64(SW_01) },

            { L_,    SW_10,     0,          g64(SW_10),         g64(SW_10) },
            { L_,    SW_10,     1,        g64(SW_10)^2,         g64(SW_10) },
            { L_,    SW_10, BPS-2,          g64(SW_10),         g64(SW_10) },
            { L_,    SW_10, BPS-1, g64(SW_10)^int64Min,         g64(SW_10) },

            //        DSTB     = g64("1101101100100")g64("1101101100100")
            { L_,     DSTB,    4,g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTB,    5,g64("1101101000100"),g64("1101101100100") },
            { L_,     DSTB,    6,g64("1101100100100"),g64("1101101100100") },
            { L_,     DSTB,    7,g64("1101101100100"),g64("1101101100100") },
            { L_,     DSTB,    8,g64("1101001100100"),g64("1101101100100") },

            //        DSTC     = g64("1101101100..0"),g64("1101101100..0")
            { L_,     DSTC,   63,g64("0101101100..0"),g64("1101101100..0") },
            { L_,     DSTC,   62,g64("1001101100..0"),g64("1101101100..0") },
            { L_,     DSTC,   61,g64("1101101100..0"),g64("1101101100..0") },
            { L_,     DSTC,   57,g64("1101100100..0"),g64("1101101100..0") },
            { L_,     DSTC,   56,g64("1101101000..0"),g64("1101101100..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 src = g64(spec);
            Int64 dst = src;
            Util::andEqual64(&dst, DATA_B[di].d_index, 0);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_andEqual0); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_andEqual0 == dst);

            dst = src;

            Util::andEqual64(&dst, DATA_B[di].d_index, 1);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_andEqual1); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_andEqual1 == dst);
            LOOP_ASSERT(LINE, src == dst);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TOGGLEBIT FUNCTION:
        //   'toggleBit' is implemented as a bitwise exclusive-or calculation
        //   using an already-tested function.  The concern is simply that the
        //   parameter is correctly used in the calculation.  A small number of
        //   test vectors is sufficient to address the concern.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'toggleBit' sets the expected
        //   value.
        //
        // Testing:
        //   void toggleBit(int *dInt, int dIdx);
        //   void toggleBit64(bsls_Types::Int64 *dInt, int dIdx);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing toggleBit Function" << endl
                          << "==========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;    // line number
            const char *d_dst;        // destination integer
            int         d_index;      // destination index
            int         d_toggleBit;  // expected result from toggle function
        } DATA_A[] = {
            //L#     dst   Index     toggleBit result
            //--   -----  ------     ----------------
            { L_, "0..0",     0,                    1 },
            { L_, "0..0",     1,                    2 },
            { L_, "0..0", BPW-2,           1<<(BPW-2) },
            { L_, "0..0", BPW-1,              INT_MIN },

            { L_, "1..1",     0,                   ~1 },
            { L_, "1..1",     1,                   ~2 },
            { L_, "1..1", BPW-2, g("1..1")^1<<(BPW-2) },
            { L_, "1..1", BPW-1,              INT_MAX },

            { L_,  FW_01,     0,           g(FW_01)^1 },
            { L_,  FW_01,     1,           g(FW_01)|2 },
            { L_,  FW_01, BPW-2,  g(FW_01)^1<<(BPW-2) },
            { L_,  FW_01, BPW-1,     g(FW_01)|INT_MIN },

            { L_,  FW_10,     0,           g(FW_10)|1 },
            { L_,  FW_10,     1,           g(FW_10)^2 },
            { L_,  FW_10, BPW-2,  g(FW_10)|1<<(BPW-2) },
            { L_,  FW_10, BPW-1,     g(FW_10)^INT_MIN },

            //     DSTA =          g("1101101100100")
            { L_,  DSTA,      4,   g("1101101110100") },
            { L_,  DSTA,      5,   g("1101101000100") },
            { L_,  DSTA,      6,   g("1101100100100") },
            { L_,  DSTA,      7,   g("1101111100100") },
            { L_,  DSTA,      8,   g("1101001100100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            Util::toggleBit(&dst, DATA_A[di].d_index);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_toggleBit); P(dst);
            }
            LOOP_ASSERT(LINE, dst == DATA_A[di].d_toggleBit);
        }

        if (verbose) cout << endl
                          << "Testing toggleBit64 Function" << endl
                          << "============================" << endl;

        const Int64 one64 = 1;
        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;    // line number
            const char *d_dst;        // destination integer
            int         d_index;      // destination index
            Int64       d_toggleBit;  // expected result from toggle function
        } DATA_B[] = {
            //L#     dst   Index     toggleBit result
            //--   -----  ------     ----------------
            { L_, "0..0",     0,                    1 },
            { L_, "0..0",     1,                    2 },
            { L_, "0..0", BPS-2,       one64<<(BPS-2) },
            { L_, "0..0", BPS-1,             int64Min },

            { L_, "1..1",     0,                   ~1 },
            { L_, "1..1",     1,                   ~2 },
            { L_, "1..1", BPS-2,g64("1..1")^one64<<(BPS-2) },
            { L_, "1..1", BPS-1,             int64Max },

            { L_,  SW_01,     0,         g64(SW_01)^1 },
            { L_,  SW_01,     1,         g64(SW_01)|2 },
            { L_,  SW_01, BPS-2,g64(SW_01)^one64<<(BPS-2) },
            { L_,  SW_01, BPS-1,  g64(SW_01)|int64Min },

            { L_,  SW_10,     0,         g64(SW_10)|1 },
            { L_,  SW_10,     1,         g64(SW_10)^2 },
            { L_,  SW_10, BPS-2,g64(SW_10)|one64<<(BPS-2) },
            { L_,  SW_10, BPS-1,  g64(SW_10)^int64Min },

            //     DSTA =        g64("1101101100100")
            { L_,  DSTA,      4, g64("1101101110100") },
            { L_,  DSTA,      5, g64("1101101000100") },
            { L_,  DSTA,      6, g64("1101100100100") },
            { L_,  DSTA,      7, g64("1101111100100") },
            { L_,  DSTA,      8, g64("1101001100100") },

            //     DSTB =        g64("110110110010..0")
            { L_,  DSTB,     55, g64("110110111010..0") },
            { L_,  DSTB,     56, g64("110110100010..0") },
            { L_,  DSTB,     57, g64("110110010010..0") },
            { L_,  DSTB,     58, g64("110111110010..0") },
            { L_,  DSTB,     59, g64("110100110010..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Util::toggleBit64(&dst, DATA_B[di].d_index);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_toggleBit); P(dst);
            }
            LOOP_ASSERT(LINE, dst == DATA_B[di].d_toggleBit);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING REMOVE FUNCTIONS:
        //   The 'remove' methods are each implemented using a multi-line
        //   calculation, and so the test data must be rich enough to catch
        //   relatively subtle errors that may otherwise evade detection.  In
        //   particular, there is a boundary at numBits = BITS_PER_WORD that
        //   must be verified.
        //
        // Plan:
        //   For each of a sequence of individual tests grouped by initial
        //   destination category, verify that 'remove0' and 'remove1' each set
        //   the expected value.
        //
        // Testing:
        //   void removeZero(  int *dInt, int dIdx, int nBits);
        //   void removeOne(   int *dInt, int dIdx, int nBits);
        //   void removeZero64(int *dInt, int dIdx, int nBits);
        //   void removeOne64( int *dInt, int dIdx, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "Testing removeZero and removeOne Functions" << endl
                  << "==========================================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_numBits;   // number of bits
            int         d_remove0;   // expected result from remove0 function
            int         d_remove1;   // expected result from remove1 function
        } DATA_A[] = {
            //L#     dst Index NumBits     remove0 result    remove1 result
            //--  ------ ----- -------     --------------    --------------
            { L_,"0..0",    0,    0,                 0,                   0 },
            { L_,"0..0",    0,    1,                 0,             INT_MIN },
            { L_,"0..0",    0,BPW-1,                 0,                  ~1 },
            { L_,"0..0",    0,  BPW,                 0,                  ~0 },
            { L_,"0..0",    1,    0,                 0,                   0 },
            { L_,"0..0",    1,    1,                 0,             INT_MIN },
            { L_,"0..0",    1,BPW-1,                 0,                  ~1 },
            { L_,"0..0",BPW-1,    0,                 0,                   0 },
            { L_,"0..0",BPW-1,    1,                 0,             INT_MIN },
            { L_,"0..0",  BPW,    0,                 0,                   0 },

            { L_,"1..1",    0,    0,                ~0,                  ~0 },
            { L_,"1..1",    0,    1,           INT_MAX,                  ~0 },
            { L_,"1..1",    0,BPW-1,                 1,                  ~0 },
            { L_,"1..1",    0,  BPW,                 0,                  ~0 },
            { L_,"1..1",    1,    0,                ~0,                  ~0 },
            { L_,"1..1",    1,    1,           INT_MAX,                  ~0 },
            { L_,"1..1",    1,BPW-1,                 1,                  ~0 },
            { L_,"1..1",BPW-1,    0,                ~0,                  ~0 },
            { L_,"1..1",BPW-1,    1,           INT_MAX,                  ~0 },
            { L_,"1..1",  BPW,    0,                ~0,                  ~0 },

            { L_, FW_01,    0,    0,          g(FW_01),            g(FW_01) },
            { L_, FW_01,    0,    1,  g(FW_10)&INT_MAX,            g(FW_10) },
            { L_, FW_01,    0,BPW-1,                 0,                  ~1 },
            { L_, FW_01,    0,  BPW,                 0,                  ~0 },
            { L_, FW_01,    1,    0,          g(FW_01),            g(FW_01) },
            { L_, FW_01,    1,    1,(g(FW_10)&INT_MAX)|1,        g(FW_10)|1 },
            { L_, FW_01,    1,BPW-1,                 1,                  ~0 },
            { L_, FW_01,BPW-1,    0,          g(FW_01),            g(FW_01) },
            { L_, FW_01,BPW-1,    1,          g(FW_01),    g(FW_01)|INT_MIN },
            { L_, FW_01,  BPW,    0,          g(FW_01),            g(FW_01) },

            { L_, FW_10,    0,    0,          g(FW_10),            g(FW_10) },
            { L_, FW_10,    0,    1,          g(FW_01),    g(FW_01)|INT_MIN },
            { L_, FW_10,    0,BPW-1,                 1,                  ~0 },
            { L_, FW_10,    0,  BPW,                 0,                  ~0 },
            { L_, FW_10,    1,    0,          g(FW_10),            g(FW_10) },
            { L_, FW_10,    1,    1,        g(FW_01)^1,(g(FW_01)^1)|INT_MIN },
            { L_, FW_10,    1,BPW-1,                 0,                  ~1 },
            { L_, FW_10,BPW-1,    0,          g(FW_10),            g(FW_10) },
            { L_, FW_10,BPW-1,    1,  g(FW_10)&INT_MAX,            g(FW_10) },
            { L_, FW_10,  BPW,    0,          g(FW_10),            g(FW_10) },

            // DSTA =            g("1101101100100"),  g("00001101101100100")
            { L_,  DSTA,  2, 4,  g("0000110110100"),  g("11110..0110110100") },
            { L_,  DSTA,  2, 5,  g("0000011011000"),  g("111110..011011000") },
            { L_,  DSTA,  2, 6,  g("0000001101100"),  g("1111110..01101100") },
            { L_,  DSTA,  2, 7,  g("0000000110100"),  g("11111110..0110100") },
            { L_,  DSTA,  2, 8,  g("0000000011000"),  g("111111110..011000") },
            { L_,  DSTA,  3, 4,  g("0000110110100"),  g("11110..0110110100") },
            { L_,  DSTA,  3, 5,  g("0000011011100"),  g("111110..011011100") },
            { L_,  DSTA,  3, 6,  g("0000001101100"),  g("1111110..01101100") },
            { L_,  DSTA,  3, 7,  g("0000000110100"),  g("11111110..0110100") },
            { L_,  DSTA,  3, 8,  g("0000000011100"),  g("111111110..011100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int dst0A = g(spec);
            int dst1A = dst0A;

            Util::removeZero(&dst0A, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_remove0);P_(dst0A);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_remove0 == dst0A);

            Util::removeOne(&dst1A, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_remove1);P_(dst1A);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_remove1 == dst1A);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing remove0, remove1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int dst0A = g(spec);
            int dst1A = dst0A;

            Util::remove0(&dst0A, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_remove0);P_(dst0A);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_remove0 == dst0A);

            Util::remove1(&dst1A, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_remove1);P_(dst1A);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_remove1 == dst1A);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
                  << "Testing removeZero64 and removeOne64 Functions" << endl
                  << "==============================================" << endl;

        const char *DSTB = "1101101100100";          // typical case
        const char *DSTC = "1101101100100" FW_01;    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_numBits;   // number of bits
            Int64       d_remove0;   // expected result from remove0 function
            Int64       d_remove1;   // expected result from remove1 function
        } DATA_B[] = {
            //L#     dst Index NumBits     remove0 result    remove1 result
            //--  ------ ----- -------     --------------    --------------
            { L_,"0..0",    0,    0,                 0,                   0 },
            { L_,"0..0",    0,    1,                 0,            int64Min },
            { L_,"0..0",    0,BPS-1,                 0,                  ~1 },
            { L_,"0..0",    0,  BPS,                 0,                  ~0 },
            { L_,"0..0",    1,    0,                 0,                   0 },
            { L_,"0..0",    1,    1,                 0,            int64Min },
            { L_,"0..0",    1,BPS-1,                 0,                  ~1 },
            { L_,"0..0",BPS-1,    0,                 0,                   0 },
            { L_,"0..0",BPS-1,    1,                 0,            int64Min },
            { L_,"0..0",  BPS,    0,                 0,                   0 },

            { L_,"1..1",    0,    0,                ~0,                  ~0 },
            { L_,"1..1",    0,    1,          int64Max,                  ~0 },
            { L_,"1..1",    0,BPS-1,                 1,                  ~0 },
            { L_,"1..1",    0,  BPS,                 0,                  ~0 },
            { L_,"1..1",    1,    0,                ~0,                  ~0 },
            { L_,"1..1",    1,    1,          int64Max,                  ~0 },
            { L_,"1..1",    1,BPS-1,                 1,                  ~0 },
            { L_,"1..1",BPS-1,    0,                ~0,                  ~0 },
            { L_,"1..1",BPS-1,    1,          int64Max,                  ~0 },
            { L_,"1..1",  BPS,    0,                ~0,                  ~0 },

            { L_, SW_01,    0,    0,        g64(SW_01),          g64(SW_01) },
            { L_, SW_01,    0,    1, g64(SW_10)&int64Max,        g64(SW_10) },
            { L_, SW_01,    0,BPS-1,                 0,                  ~1 },
            { L_, SW_01,    0,  BPS,                 0,                  ~0 },
            { L_, SW_01,    1,    0,        g64(SW_01),          g64(SW_01) },
            { L_, SW_01,    1,    1, (g64(SW_10)&int64Max)|1,  g64(SW_10)|1 },
            { L_, SW_01,    1,BPS-1,                 1,                  ~0 },
            { L_, SW_01,BPS-1,    0,        g64(SW_01),          g64(SW_01) },
            { L_, SW_01,BPS-1,    1,        g64(SW_01), g64(SW_01)|int64Min },
            { L_, SW_01,  BPS,    0,        g64(SW_01),          g64(SW_01) },

            { L_, SW_10,    0,    0,        g64(SW_10),          g64(SW_10) },
            { L_, SW_10,    0,    1,        g64(SW_01), g64(SW_01)|int64Min },
            { L_, SW_10,    0,BPS-1,                 1,                  ~0 },
            { L_, SW_10,    0,  BPS,                 0,                  ~0 },
            { L_, SW_10,    1,    0,        g64(SW_10),          g64(SW_10) },
            { L_, SW_10,    1,    1,   g64(SW_01)^1,(g64(SW_01)^1)|int64Min },
            { L_, SW_10,    1,BPS-1,                 0,                  ~1 },
            { L_, SW_10,BPS-1,    0,        g64(SW_10),          g64(SW_10) },
            { L_, SW_10,BPS-1,    1, g64(SW_10)&int64Max,        g64(SW_10) },
            { L_, SW_10,  BPS,    0,        g64(SW_10),          g64(SW_10) },

            // DSTB =          g64("1101101100100"),g64("00001101101100100")
            { L_,  DSTB,  2, 4,g64("0000110110100"),g64("11110..0110110100") },
            { L_,  DSTB,  2, 5,g64("0000011011000"),g64("111110..011011000") },
            { L_,  DSTB,  2, 6,g64("0000001101100"),g64("1111110..01101100") },
            { L_,  DSTB,  2, 7,g64("0000000110100"),g64("11111110..0110100") },
            { L_,  DSTB,  2, 8,g64("0000000011000"),g64("111111110..011000") },
            { L_,  DSTB,  3, 4,g64("0000110110100"),g64("11110..0110110100") },
            { L_,  DSTB,  3, 5,g64("0000011011100"),g64("111110..011011100") },
            { L_,  DSTB,  3, 6,g64("0000001101100"),g64("1111110..01101100") },
            { L_,  DSTB,  3, 7,g64("0000000110100"),g64("11111110..0110100") },
            { L_,  DSTB,  3, 8,g64("0000000011100"),g64("111111110..011100") },

        { L_,DSTC,34, 4,g64("0110110100"FW_01),g64("11110..0110110100"FW_01) },
        { L_,DSTC,34, 5,g64("0011011000"FW_01),g64("111110..011011000"FW_01) },
        { L_,DSTC,34, 6,g64("0001101100"FW_01),g64("1111110..01101100"FW_01) },
        { L_,DSTC,34, 7,g64("0000110100"FW_01),g64("11111110..0110100"FW_01) },
        { L_,DSTC,34, 8,g64("0000011000"FW_01),g64("111111110..011000"FW_01) },
        { L_,DSTC,35, 4,g64("0110110100"FW_01),g64("11110..0110110100"FW_01) },
        { L_,DSTC,35, 5,g64("0011011100"FW_01),g64("111110..011011100"FW_01) },
        { L_,DSTC,35, 6,g64("0001101100"FW_01),g64("1111110..01101100"FW_01) },
        { L_,DSTC,35, 7,g64("0000110100"FW_01),g64("11111110..0110100"FW_01) },
        { L_,DSTC,35, 8,g64("0000011100"FW_01),g64("111111110..011100"FW_01) }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE    = DATA_B[di].d_lineNum;
            const char *spec    = DATA_B[di].d_dst;
            const int   index   = DATA_B[di].d_index;
            const int   numBits = DATA_B[di].d_numBits;

            Int64 dst0 = g64(spec);
            Int64 dst1 = dst0;

            Util::removeZero64(&dst0, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_remove0); P(dst0);
            }
            LOOP6_ASSERT(LINE, spec, index, numBits, DATA_B[di].d_remove0,
                                           dst0, DATA_B[di].d_remove0 == dst0);

            Util::removeOne64(&dst1, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_remove1); P(dst1);
            }
            LOOP6_ASSERT(LINE, spec, index, numBits, DATA_B[di].d_remove1,
                                           dst1, DATA_B[di].d_remove1 == dst1);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'toggle' FUNCTION:
        //   'toggle' is implemented as a bitwise exclusive-or calculation
        //   using an already-tested function.  The concern is simply that
        //   parameters are correctly matched up in the calculation.  A small
        //   number of test vectors is sufficient to address the concern.
        //
        // Plan:
        //   For each of a tabulated sequence of individual tests grouped by
        //   initial destination category, verify that 'toggle' sets the
        //   expected value.
        //
        // Testing:
        //   void toggle(int *dInt, int dIdx, int nBits);
        //   void toggle64(bsls_Types::Int64 *dInt,
        //                 int dIdx,
        //                 int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'toggle' Function" << endl
                          << "=========================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_numBits;     // number of bits
            int         d_toggle;      // expected result from toggle function
        } DATA_A[] = {
            //L#     dst  Index NumBits      toggle result
            //--  ------  ----- -------      -------------
            { L_, "0..0",     0,     0,                  0 },
            { L_, "0..0",     0,     1,                  1 },
            { L_, "0..0",     0, BPW-1,            INT_MAX },
            { L_, "0..0",     0,   BPW,                 ~0 },
            { L_, "0..0",     1,     0,                  0 },
            { L_, "0..0",     1,     1,               1<<1 },
            { L_, "0..0",     1, BPW-1,                 ~1 },
            { L_, "0..0", BPW-1,     0,                  0 },
            { L_, "0..0", BPW-1,     1,            INT_MIN },
            { L_, "0..0",   BPW,     0,                  0 },

            { L_, "1..1",     0,     0,                 ~0 },
            { L_, "1..1",     0,     1,                 ~1 },
            { L_, "1..1",     0, BPW-1,            INT_MIN },
            { L_, "1..1",     0,   BPW,                  0 },
            { L_, "1..1",     1,     0,                 ~0 },
            { L_, "1..1",     1,     1,            ~(1<<1) },
            { L_, "1..1",     1, BPW-1,                  1 },
            { L_, "1..1", BPW-1,     0,                 ~0 },
            { L_, "1..1", BPW-1,     1,            INT_MAX },
            { L_, "1..1",   BPW,     0,                 ~0 },

            { L_,  FW_01,     0,     0,           g(FW_01) },
            { L_,  FW_01,     0,     1,         g(FW_01)^1 },
            { L_,  FW_01,     0, BPW-1,   g(FW_10)&INT_MAX },
            { L_,  FW_01,     0,   BPW,           g(FW_10) },
            { L_,  FW_01,     1,     0,           g(FW_01) },
            { L_,  FW_01,     1,     1,    g(FW_01)|(1<<1) },
            { L_,  FW_01,     1, BPW-1,         g(FW_10)|1 },
            { L_,  FW_01, BPW-1,     0,           g(FW_01) },
            { L_,  FW_01, BPW-1,     1,   g(FW_01)|INT_MIN },
            { L_,  FW_01,   BPW,     0,           g(FW_01) },

            { L_,  FW_10,     0,     0,           g(FW_10) },
            { L_,  FW_10,     0,     1,         g(FW_10)|1 },
            { L_,  FW_10,     0, BPW-1,   g(FW_01)|INT_MIN },
            { L_,  FW_10,     0,   BPW,           g(FW_01) },
            { L_,  FW_10,     1,     0,           g(FW_10) },
            { L_,  FW_10,     1,     1,         g(FW_10)^2 },
            { L_,  FW_10,     1, BPW-1,         g(FW_01)^1 },
            { L_,  FW_10, BPW-1,     0,           g(FW_10) },
            { L_,  FW_10, BPW-1,     1,   g(FW_10)&INT_MAX },
            { L_,  FW_10,   BPW,     0,           g(FW_10) },

            // DSTA =                    g("1011101100100")
            { L_,   DSTA,     2,     4,  g("1101101011000") },
            { L_,   DSTA,     2,     5,  g("1101100011000") },
            { L_,   DSTA,     2,     6,  g("1101110011000") },
            { L_,   DSTA,     2,     7,  g("1101010011000") },
            { L_,   DSTA,     2,     8,  g("1100010011000") },
            { L_,   DSTA,     3,     4,  g("1101100011100") },
            { L_,   DSTA,     3,     5,  g("1101110011100") },
            { L_,   DSTA,     3,     6,  g("1101010011100") },
            { L_,   DSTA,     3,     7,  g("1100010011100") },
            { L_,   DSTA,     3,     8,  g("1110010011100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_dst;

            int dst = g(spec);
            Util::toggle(&dst, DATA_A[di].d_index, DATA_A[di].d_numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_toggle); P(dst);
            }
            LOOP_ASSERT(LINE, dst == DATA_A[di].d_toggle);
        }

        if (verbose) cout << endl
                          << "Testing 'toggle64' Function" << endl
                          << "===========================" << endl;

        const char *DSTB = "110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_numBits;     // number of bits
            Int64       d_toggle;      // expected result from toggle function
        } DATA_B[] = {
            //L#     dst  Index NumBits      toggle result
            //--  ------  ----- -------      -------------
            { L_, "0..0",     0,     0,                  0 },
            { L_, "0..0",     0,     1,                  1 },
            { L_, "0..0",     0, BPS-1,           int64Max },
            { L_, "0..0",     0,   BPS,                 ~0 },
            { L_, "0..0",     1,     0,                  0 },
            { L_, "0..0",     1,     1,               1<<1 },
            { L_, "0..0",     1, BPS-1,                 ~1 },
            { L_, "0..0", BPS-1,     0,                  0 },
            { L_, "0..0", BPS-1,     1,           int64Min },
            { L_, "0..0",   BPS,     0,                  0 },

            { L_, "1..1",     0,     0,                 ~0 },
            { L_, "1..1",     0,     1,                 ~1 },
            { L_, "1..1",     0, BPS-1,           int64Min },
            { L_, "1..1",     0,   BPS,                  0 },
            { L_, "1..1",     1,     0,                 ~0 },
            { L_, "1..1",     1,     1,            ~(1<<1) },
            { L_, "1..1",     1, BPS-1,                  1 },
            { L_, "1..1", BPS-1,     0,                 ~0 },
            { L_, "1..1", BPS-1,     1,           int64Max },
            { L_, "1..1",   BPS,     0,                 ~0 },

            { L_,  SW_01,     0,     0,         g64(SW_01) },
            { L_,  SW_01,     0,     1,       g64(SW_01)^1 },
            { L_,  SW_01,     0, BPS-1,g64(SW_10)&int64Max },
            { L_,  SW_01,     0,   BPS,         g64(SW_10) },
            { L_,  SW_01,     1,     0,         g64(SW_01) },
            { L_,  SW_01,     1,     1,  g64(SW_01)|(1<<1) },
            { L_,  SW_01,     1, BPS-1,       g64(SW_10)|1 },
            { L_,  SW_01, BPS-1,     0,         g64(SW_01) },
            { L_,  SW_01, BPS-1,     1,g64(SW_01)|int64Min },
            { L_,  SW_01,   BPS,     0,         g64(SW_01) },

            { L_,  SW_10,     0,     0,         g64(SW_10) },
            { L_,  SW_10,     0,     1,       g64(SW_10)|1 },
            { L_,  SW_10,     0, BPS-1,g64(SW_01)|int64Min },
            { L_,  SW_10,     0,   BPS,         g64(SW_01) },
            { L_,  SW_10,     1,     0,         g64(SW_10) },
            { L_,  SW_10,     1,     1,       g64(SW_10)^2 },
            { L_,  SW_10,     1, BPS-1,       g64(SW_01)^1 },
            { L_,  SW_10, BPS-1,     1,g64(SW_10)&int64Max },
            { L_,  SW_10, BPS-1,     0,         g64(SW_10) },
            { L_,  SW_10,   BPS,     0,         g64(SW_10) },

            // DSTA =                  g64("1101101100100")
            { L_,   DSTA,     2,     4,g64("1101101011000") },
            { L_,   DSTA,     2,     5,g64("1101100011000") },
            { L_,   DSTA,     2,     6,g64("1101110011000") },
            { L_,   DSTA,     2,     7,g64("1101010011000") },
            { L_,   DSTA,     2,     8,g64("1100010011000") },
            { L_,   DSTA,     3,     4,g64("1101100011100") },
            { L_,   DSTA,     3,     5,g64("1101110011100") },
            { L_,   DSTA,     3,     6,g64("1101010011100") },
            { L_,   DSTA,     3,     7,g64("1100010011100") },
            { L_,   DSTA,     3,     8,g64("1110010011100") },

            // DSTB =                  g64("110110110010..0")
            { L_,   DSTB,    53,     4,g64("110110101100..0") },
            { L_,   DSTB,    53,     5,g64("110110001100..0") },
            { L_,   DSTB,    53,     6,g64("110111001100..0") },
            { L_,   DSTB,    53,     7,g64("110101001100..0") },
            { L_,   DSTB,    53,     8,g64("110001001100..0") },
            { L_,   DSTB,    54,     4,g64("110110001110..0") },
            { L_,   DSTB,    54,     5,g64("110111001110..0") },
            { L_,   DSTB,    54,     6,g64("110101001110..0") },
            { L_,   DSTB,    54,     7,g64("110001001110..0") },
            { L_,   DSTB,    54,     8,g64("111001001110..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_dst;

            Int64 dst = g64(spec);
            Util::toggle64(&dst, DATA_B[di].d_index, DATA_B[di].d_numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_toggle); P(dst);
            }
            LOOP_ASSERT(LINE, dst == DATA_B[di].d_toggle);
        }
      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testB(int test)
{
    switch (test) {
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INSERT FUNCTIONS:
        //   The two methods under test each perform a straightforward
        //   computation involving only the input parameters and already-tested
        //   functions, and so a relatively small set of test vectors is
        //   sufficient to probe the computation.
        //
        // Plan:
        //   For each of a tabulated sequence of individual tests grouped by
        //   initial destination category, verify that 'insert0' and 'insert1'
        //   each set the expected value.
        //
        // Testing:
        //   void insertZero(  int *dInt, int dIdx, int nBits);
        //   void insertZero64(int *dInt, int dIdx, int nBits);
        //   void insertOne(   int *dInt, int dIdx, int nBits);
        //   void insertOne64( int *dInt, int dIdx, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "Testing insertZero and insertOne Functions" << endl
                      << "==========================================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_numBits;     // number of bits
            int         d_insert0;     // expected result from insert0 function
            int         d_insert1;     // expected result from insert1 function
        } DATA_A[] = {
            //L#     dst  Index NumBits   insert0 result   insert1 result
            //--  ------  ----- -------   --------------   --------------
            { L_, "0..0",     0,     0,                0,               0 },
            { L_, "0..0",     0,     1,                0,               1 },
            { L_, "0..0",     0, BPW-1,                0,         INT_MAX },
            { L_, "0..0",     0,   BPW,                0,              ~0 },
            { L_, "0..0",     1,     0,                0,               0 },
            { L_, "0..0",     1,     1,                0,            1<<1 },
            { L_, "0..0",     1, BPW-1,                0,              ~1 },
            { L_, "0..0", BPW-1,     0,                0,               0 },
            { L_, "0..0", BPW-1,     1,                0,         INT_MIN },
            { L_, "0..0",   BPW,     0,                0,               0 },

            { L_, "1..1",     0,     0,               ~0,              ~0 },
            { L_, "1..1",     0,     1,               ~1,              ~0 },
            { L_, "1..1",     0, BPW-1,          INT_MIN,              ~0 },
            { L_, "1..1",     0,   BPW,                0,              ~0 },
            { L_, "1..1",     1,     0,               ~0,              ~0 },
            { L_, "1..1",     1,     1,               ~2,              ~0 },
            { L_, "1..1",     1, BPW-1,                1,              ~0 },
            { L_, "1..1", BPW-1,     0,               ~0,              ~0 },
            { L_, "1..1", BPW-1,     1,          INT_MAX,              ~0 },
            { L_, "1..1",   BPW,     0,               ~0,              ~0 },

            { L_,  FW_01,     0,     0,         g(FW_01),        g(FW_01) },
            { L_,  FW_01,     0,     1,         g(FW_10),      g(FW_10)|1 },
            { L_,  FW_01,     0, BPW-1,          INT_MIN,              ~0 },
            { L_,  FW_01,     0,   BPW,                0,              ~0 },
            { L_,  FW_01,     1,     0,         g(FW_01),        g(FW_01) },
            { L_,  FW_01,     1,     1,       g(FW_10)^3,      g(FW_10)|1 },
            { L_,  FW_01,     1, BPW-1,                1,              ~0 },
            { L_,  FW_01, BPW-1,     0,         g(FW_01),        g(FW_01) },
            { L_,  FW_01, BPW-1,     1,         g(FW_01),g(FW_01)|INT_MIN },
            { L_,  FW_01,   BPW,     0,         g(FW_01),        g(FW_01) },

            { L_,  FW_10,     0,     0,         g(FW_10),        g(FW_10) },
            { L_,  FW_10,     0,     1,       g(FW_01)^1,        g(FW_01) },
            { L_,  FW_10,     0, BPW-1,                0,         INT_MAX },
            { L_,  FW_10,     0,   BPW,                0,              ~0 },
            { L_,  FW_10,     1,     0,         g(FW_10),        g(FW_10) },
            { L_,  FW_10,     1,     1,       g(FW_01)^1,      g(FW_01)^3 },
            { L_,  FW_10,     1, BPW-1,                0,              ~1 },
            { L_,  FW_10, BPW-1,     0,         g(FW_10),        g(FW_10) },
            { L_,  FW_10, BPW-1,     1, g(FW_10)&INT_MAX,        g(FW_10) },
            { L_,  FW_10,   BPW,     0,         g(FW_10),        g(FW_10) },

  //v-------^
  // DST =          g("000000001101101100100")  g("000000001101101100100")
  { L_, DSTA, 2, 4, g("000011011011001000000"), g("000011011011001111100") },
  { L_, DSTA, 2, 5, g("000110110110010000000"), g("000110110110011111100") },
  { L_, DSTA, 2, 6, g("001101101100100000000"), g("001101101100111111100") },
  { L_, DSTA, 2, 7, g("011011011001000000000"), g("011011011001111111100") },
  { L_, DSTA, 2, 8, g("110110110010000000000"), g("110110110011111111100") },
  { L_, DSTA, 3, 4, g("000011011011000000100"), g("000011011011001111100") },
  { L_, DSTA, 3, 5, g("000110110110000000100"), g("000110110110011111100") },
  { L_, DSTA, 3, 6, g("001101101100000000100"), g("001101101100111111100") },
  { L_, DSTA, 3, 7, g("011011011000000000100"), g("011011011001111111100") },
  { L_, DSTA, 3, 8, g("110110110000000000100"), g("110110110011111111100") }
  //^-------v
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int dstSrc = g(spec);
            int dst = dstSrc;

            Util::insertZero(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_insert0); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_insert0 == dst);

            dst = dstSrc;
            Util::insertOne(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_insert1); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_insert1 == dst);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing insert0, insert1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int dstSrc = g(spec);
            int dst = dstSrc;

            Util::insert0(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_insert0); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_insert0 == dst);

            dst = dstSrc;
            Util::insert1(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_insert1); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_insert1 == dst);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
                  << "Testing insertZero64 and insertOne64 Functions" << endl
                  << "==============================================" << endl;

        const char *DSTB = "1101101100100";            // typical case
        const char *DSTC = "000000110110110010..0";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_dst;         // destination integer
            int         d_index;       // destination index
            int         d_numBits;     // number of bits
            Int64       d_insert0;     // expected result from insert0 function
            Int64       d_insert1;     // expected result from insert1 function
        } DATA_B[] = {
            //L#     dst  Index NumBits   insert0 result   insert1 result
            //--  ------  ----- -------   --------------   --------------
            { L_, "0..0",     0,     0,                0,               0 },
            { L_, "0..0",     0,     1,                0,               1 },
            { L_, "0..0",     0, BPS-1,                0,        int64Max },
            { L_, "0..0",     0,   BPS,                0,              ~0 },
            { L_, "0..0",     1,     0,                0,               0 },
            { L_, "0..0",     1,     1,                0,            1<<1 },
            { L_, "0..0",     1, BPS-1,                0,              ~1 },
            { L_, "0..0", BPS-1,     0,                0,               0 },
            { L_, "0..0", BPS-1,     1,                0,        int64Min },
            { L_, "0..0",   BPS,     0,                0,               0 },

            { L_, "1..1",     0,     0,               ~0,              ~0 },
            { L_, "1..1",     0,     1,               ~1,              ~0 },
            { L_, "1..1",     0, BPS-1,         int64Min,              ~0 },
            { L_, "1..1",     0,   BPS,                0,              ~0 },
            { L_, "1..1",     1,     0,               ~0,              ~0 },
            { L_, "1..1",     1,     1,               ~2,              ~0 },
            { L_, "1..1",     1, BPS-1,                1,              ~0 },
            { L_, "1..1", BPS-1,     0,               ~0,              ~0 },
            { L_, "1..1", BPS-1,     1,         int64Max,              ~0 },
            { L_, "1..1",   BPS,     0,               ~0,              ~0 },

            { L_,  SW_01,     0,     0,       g64(SW_01),      g64(SW_01) },
            { L_,  SW_01,     0,     1,       g64(SW_10),    g64(SW_10)|1 },
            { L_,  SW_01,     0, BPS-1,         int64Min,              ~0 },
            { L_,  SW_01,     0,   BPS,                0,              ~0 },
            { L_,  SW_01,     1,     0,       g64(SW_01),      g64(SW_01) },
            { L_,  SW_01,     1,     1,     g64(SW_10)^3,    g64(SW_10)|1 },
            { L_,  SW_01,     1, BPS-1,                1,              ~0 },
            { L_,  SW_01, BPS-1,     0,       g64(SW_01),      g64(SW_01) },
            { L_,  SW_01, BPS-1,     1,       g64(SW_01),g64(SW_01)|int64Min },
            { L_,  SW_01,   BPS,     0,       g64(SW_01),      g64(SW_01) },

            { L_,  SW_10,     0,     0,       g64(SW_10),      g64(SW_10) },
            { L_,  SW_10,     0,     1,     g64(SW_01)^1,      g64(SW_01) },
            { L_,  SW_10,     0, BPS-1,                0,        int64Max },
            { L_,  SW_10,     0,   BPS,                0,              ~0 },
            { L_,  SW_10,     1,     0,       g64(SW_10),      g64(SW_10) },
            { L_,  SW_10,     1,     1,     g64(SW_01)^1,    g64(SW_01)^3 },
            { L_,  SW_10,     1, BPS-1,                0,              ~1 },
            { L_,  SW_10, BPS-1,     0,       g64(SW_10),      g64(SW_10) },
            { L_,  SW_10, BPS-1,     1,g64(SW_10)&int64Max,    g64(SW_10) },
            { L_,  SW_10,   BPS,     0,       g64(SW_10),      g64(SW_10) },

  //v-------^
  // DST =         g64("000000001101101100100") g64("000000001101101100100")
  { L_, DSTB, 2, 4,g64("000011011011001000000"),g64("000011011011001111100") },
  { L_, DSTB, 2, 5,g64("000110110110010000000"),g64("000110110110011111100") },
  { L_, DSTB, 2, 6,g64("001101101100100000000"),g64("001101101100111111100") },
  { L_, DSTB, 2, 7,g64("011011011001000000000"),g64("011011011001111111100") },
  { L_, DSTB, 2, 8,g64("110110110010000000000"),g64("110110110011111111100") },
  { L_, DSTB, 3, 4,g64("000011011011000000100"),g64("000011011011001111100") },
  { L_, DSTB, 3, 5,g64("000110110110000000100"),g64("000110110110011111100") },
  { L_, DSTB, 3, 6,g64("001101101100000000100"),g64("001101101100111111100") },
  { L_, DSTB, 3, 7,g64("011011011000000000100"),g64("011011011001111111100") },
  { L_, DSTB, 3, 8,g64("110110110000000000100"),g64("110110110011111111100") },

  // DST               "000000110110110010..0"      "000000000110110010..0"
  { L_, DSTC,47, 4,g64("001101101100100000..0"),g64("001101101100111110..0") },
  { L_, DSTC,47, 5,g64("011011011001000000..0"),g64("011011011001111110..0") },
  { L_, DSTC,47, 6,g64("110110110010000000..0"),g64("110110110011111110..0") },
  { L_, DSTC,47, 7,g64("101101100100000000..0"),g64("101101100111111110..0") },
  { L_, DSTC,47, 8,g64("011011001000000000..0"),g64("011011001111111110..0") },
  { L_, DSTC,48, 4,g64("001101101100000010..0"),g64("001101101100111110..0") },
  { L_, DSTC,48, 5,g64("011011011000000010..0"),g64("011011011001111110..0") },
  { L_, DSTC,48, 6,g64("110110110000000010..0"),g64("110110110011111110..0") },
  { L_, DSTC,48, 7,g64("101101100000000010..0"),g64("101101100111111110..0") },
  { L_, DSTC,48, 8,g64("011011000000000010..0"),g64("011011001111111110..0") }
  //^-------v
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE    = DATA_B[di].d_lineNum;
            const char *spec    = DATA_B[di].d_dst;
            const int   index   = DATA_B[di].d_index;
            const int   numBits = DATA_B[di].d_numBits;

            Int64 dstSrc = g64(spec);
            Int64 dst = dstSrc;

            Util::insertZero64(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_insert0); P(dst);
            }
            LOOP3_ASSERT(LINE, DATA_B[di].d_insert0, dst,
                                                 DATA_B[di].d_insert0 == dst);

            dst = dstSrc;
            Util::insertOne64(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_insert1); P(dst);
            }
            LOOP3_ASSERT(LINE, DATA_B[di].d_insert1, dst,
                                                 DATA_B[di].d_insert1 == dst);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING SET AND REPLACE FUNCTIONS:
        //   The four 'set' and 'replace' methods under test each perform very
        //   simple computations involving only the input parameters and
        //   already-tested methods.  A modest set of test vectors is
        //   sufficient to probe the computational logic.
        //
        // Plan:
        //   For each of a tabulated sequence of individual tests grouped by
        //   initial destination category, verify that 'set0', 'set1',
        //   'replace0' and 'replace1' each give the expected value.  Note
        //   that the "set" functions return a value while the "replace"
        //   functions load a value into an input argument.
        //
        // Testing:
        //   int setZero(int sInt, int sIdx, int nBits);
        //   int setZero64(int sInt, int sIdx, int nBits);
        //   int setOne(int sInt, int sIdx, int nBits);
        //   int setOne64(int sInt, int sIdx, int nBits);
        //   void replaceZero(int *dInt, int dIdx, int nBits);
        //   void replaceZero64(int *dInt, int dIdx, int nBits);
        //   void replaceOne(int *dInt, int dIdx, int nBits);
        //   void replaceOne64(int *dInt, int dIdx, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing set and replace Functions" << endl
                          << "=================================" << endl;

        const char *DSTA = "1101101100100";    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_numBits;   // number of bits
            int         d_res0;      // expected result from set0/replace0
            int         d_res1;      // expected result from set1/replace1
        } DATA_A[] = {
            //L#     dst Index NumBits         result0           result1
            //--  ------   ----- -------     -----------       -----------
            { L_, "0..0",     0,     0,                0,                0 },
            { L_, "0..0",     0,     1,                0,                1 },
            { L_, "0..0",     0, BPW-1,                0,          INT_MAX },
            { L_, "0..0",     0,   BPW,                0,               ~0 },
            { L_, "0..0",     1,     0,                0,                0 },
            { L_, "0..0",     1,     1,                0,             1<<1 },
            { L_, "0..0",     1, BPW-1,                0,               ~1 },
            { L_, "0..0", BPW-1,     0,                0,                0 },
            { L_, "0..0", BPW-1,     1,                0,          INT_MIN },
            { L_, "0..0",   BPW,     0,                0,                0 },

            { L_, "1..1",     0,     0,               ~0,               ~0 },
            { L_, "1..1",     0,     1,               ~1,               ~0 },
            { L_, "1..1",     0, BPW-1,          INT_MIN,               ~0 },
            { L_, "1..1",     0,   BPW,                0,               ~0 },
            { L_, "1..1",     1,     0,               ~0,               ~0 },
            { L_, "1..1",     1,     1,               ~2,               ~0 },
            { L_, "1..1",     1, BPW-1,                1,               ~0 },
            { L_, "1..1", BPW-1,     0,               ~0,               ~0 },
            { L_, "1..1", BPW-1,     1,          INT_MAX,               ~0 },
            { L_, "1..1",   BPW,     0,               ~0,               ~0 },

            { L_,  FW_01,     0,     0,         g(FW_01),         g(FW_01) },
            { L_,  FW_01,     0,     1,      g(FW_01)&~1,         g(FW_01) },
            { L_,  FW_01,     0, BPW-1,                0,          INT_MAX },
            { L_,  FW_01,     0,   BPW,                0,               ~0 },
            { L_,  FW_01,     1,     0,         g(FW_01),         g(FW_01) },
            { L_,  FW_01,     1,     1,         g(FW_01),    g(FW_01)|1<<1 },
            { L_,  FW_01,     1, BPW-1,                1,               ~0 },
            { L_,  FW_01, BPW-1,     0,         g(FW_01),         g(FW_01) },
            { L_,  FW_01, BPW-1,     1,         g(FW_01), g(FW_01)|INT_MIN },
            { L_,  FW_01,   BPW,     0,         g(FW_01),         g(FW_01) },

            { L_,  FW_10,     0,     0,         g(FW_10),         g(FW_10) },
            { L_,  FW_10,     0,     1,         g(FW_10),       g(FW_10)|1 },
            { L_,  FW_10,     0, BPW-1,          INT_MIN,               ~0 },
            { L_,  FW_10,     0,   BPW,                0,               ~0 },
            { L_,  FW_10,     1,     0,         g(FW_10),         g(FW_10) },
            { L_,  FW_10,     1,     1,   g(FW_10)&~1<<1,         g(FW_10) },
            { L_,  FW_10,     1, BPW-1,                0,               ~1 },
            { L_,  FW_10, BPW-1,     0,         g(FW_10),         g(FW_10) },
            { L_,  FW_10, BPW-1,     1, g(FW_10)&INT_MAX,         g(FW_10) },
            { L_,  FW_10,   BPW,     0,         g(FW_10),         g(FW_10) },

            // DSTA =               g("1101101100100"), g("1101101100100")
            { L_,   DSTA,   2,   4, g("1101101000000"), g("1101101111100") },
            { L_,   DSTA,   2,   5, g("1101100000000"), g("1101101111100") },
            { L_,   DSTA,   2,   6, g("1101100000000"), g("1101111111100") },
            { L_,   DSTA,   2,   7, g("1101000000000"), g("1101111111100") },
            { L_,   DSTA,   2,   8, g("1100000000000"), g("1101111111100") },
            { L_,   DSTA,   3,   4, g("1101100000100"), g("1101101111100") },
            { L_,   DSTA,   3,   5, g("1101100000100"), g("1101111111100") },
            { L_,   DSTA,   3,   6, g("1101000000100"), g("1101111111100") },
            { L_,   DSTA,   3,   7, g("1100000000100"), g("1101111111100") },
            { L_,   DSTA,   3,   8, g("1100000000100"), g("1111111111100") },
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int src = g(spec);

            int resA = Util::setZero(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res0 == resA);

            resA = Util::setOne(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res1 == resA);

            resA = src;

            Util::replaceZero(&resA, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res0 == resA);

            resA = src;

            Util::replaceOne(&resA, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing set0, set1, replace0, replace1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_dst;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int src = g(spec);

            int resA = Util::set0(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res0 == resA);

            resA = Util::set1(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res1 == resA);

            resA = src;

            Util::replace0(&resA, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res0 == resA);

            resA = src;

            Util::replace1(&resA, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_res1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_res1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
                  << "Testing 64 bit set and replace Functions" << endl
                  << "========================================" << endl;

        const char *DSTB = "1101101100100";          // typical case
        const char *DSTC = "1101101100100" FW_01;    // typical case
        static const struct {
            int         d_lineNum;   // line number
            const char *d_dst;       // destination integer
            int         d_index;     // destination index
            int         d_numBits;   // number of bits
            Int64       d_res0;      // expected result from set0/replace0
            Int64       d_res1;      // expected result from set1/replace1
        } DATA_B[] = {
            //L#     dst Index NumBits         result0           result1
            //--  ------   ----- -------     -----------       -----------
            { L_, "0..0",     0,     0,                0,                0 },
            { L_, "0..0",     0,     1,                0,                1 },
            { L_, "0..0",     0, BPS-1,                0,         int64Max },
            { L_, "0..0",     0,   BPS,                0,               ~0 },
            { L_, "0..0",     1,     0,                0,                0 },
            { L_, "0..0",     1,     1,                0,             1<<1 },
            { L_, "0..0",     1, BPS-1,                0,               ~1 },
            { L_, "0..0", BPS-1,     0,                0,                0 },
            { L_, "0..0", BPS-1,     1,                0,         int64Min },
            { L_, "0..0",   BPS,     0,                0,                0 },

            { L_, "1..1",     0,     0,               ~0,               ~0 },
            { L_, "1..1",     0,     1,               ~1,               ~0 },
            { L_, "1..1",     0, BPS-1,         int64Min,               ~0 },
            { L_, "1..1",     0,   BPS,                0,               ~0 },
            { L_, "1..1",     1,     0,               ~0,               ~0 },
            { L_, "1..1",     1,     1,               ~2,               ~0 },
            { L_, "1..1",     1, BPS-1,                1,               ~0 },
            { L_, "1..1", BPS-1,     0,               ~0,               ~0 },
            { L_, "1..1", BPS-1,     1,         int64Max,               ~0 },
            { L_, "1..1",   BPS,     0,               ~0,               ~0 },

            { L_,  SW_01,     0,     0,       g64(SW_01),       g64(SW_01) },
            { L_,  SW_01,     0,     1,    g64(SW_01)&~1,       g64(SW_01) },
            { L_,  SW_01,     0, BPS-1,                0,         int64Max },
            { L_,  SW_01,     0,   BPS,                0,               ~0 },
            { L_,  SW_01,     1,     0,       g64(SW_01),       g64(SW_01) },
            { L_,  SW_01,     1,     1,       g64(SW_01),  g64(SW_01)|1<<1 },
            { L_,  SW_01,     1, BPS-1,                1,               ~0 },
            { L_,  SW_01, BPS-1,     0,       g64(SW_01),       g64(SW_01) },
            { L_,  SW_01, BPS-1,     1,      g64(SW_01),g64(SW_01)|int64Min},
            { L_,  SW_01,   BPS,     0,       g64(SW_01),       g64(SW_01) },

            { L_,  SW_10,     0,     0,       g64(SW_10),       g64(SW_10) },
            { L_,  SW_10,     0,     1,       g64(SW_10),     g64(SW_10)|1 },
            { L_,  SW_10,     0, BPS-1,         int64Min,               ~0 },
            { L_,  SW_10,     0,   BPS,                0,               ~0 },
            { L_,  SW_10,     1,     0,       g64(SW_10),       g64(SW_10) },
            { L_,  SW_10,     1,     1, g64(SW_10)&~1<<1,       g64(SW_10) },
            { L_,  SW_10,     1, BPS-1,                0,               ~1 },
            { L_,  SW_10, BPS-1,     0,       g64(SW_10),       g64(SW_10) },
            { L_,  SW_10, BPS-1,     1, g64(SW_10)&int64Max,    g64(SW_10) },
            { L_,  SW_10,   BPS,     0,       g64(SW_10),       g64(SW_10) },

            //DSTB =             g64("1101101100100"),g64("1101101100100")
            { L_,   DSTB,  2,  4,g64("1101101000000"),g64("1101101111100") },
            { L_,   DSTB,  2,  5,g64("1101100000000"),g64("1101101111100") },
            { L_,   DSTB,  2,  6,g64("1101100000000"),g64("1101111111100") },
            { L_,   DSTB,  2,  7,g64("1101000000000"),g64("1101111111100") },
            { L_,   DSTB,  2,  8,g64("1100000000000"),g64("1101111111100") },
            { L_,   DSTB,  3,  4,g64("1101100000100"),g64("1101101111100") },
            { L_,   DSTB,  3,  5,g64("1101100000100"),g64("1101111111100") },
            { L_,   DSTB,  3,  6,g64("1101000000100"),g64("1101111111100") },
            { L_,   DSTB,  3,  7,g64("1100000000100"),g64("1101111111100") },
            { L_,   DSTB,  3,  8,g64("1100000000100"),g64("1111111111100") },

          { L_,DSTC,34,4,g64("1101101000000"FW_01),g64("1101101111100"FW_01) },
          { L_,DSTC,34,5,g64("1101100000000"FW_01),g64("1101101111100"FW_01) },
          { L_,DSTC,34,6,g64("1101100000000"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,34,7,g64("1101000000000"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,34,8,g64("1100000000000"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,35,4,g64("1101100000100"FW_01),g64("1101101111100"FW_01) },
          { L_,DSTC,35,5,g64("1101100000100"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,35,6,g64("1101000000100"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,35,7,g64("1100000000100"FW_01),g64("1101111111100"FW_01) },
          { L_,DSTC,35,8,g64("1100000000100"FW_01),g64("1111111111100"FW_01) },
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE    = DATA_B[di].d_lineNum;
            const char *spec    = DATA_B[di].d_dst;
            const int   index   = DATA_B[di].d_index;
            const int   numBits = DATA_B[di].d_numBits;

            Int64 src = g64(spec);

            Int64 res = Util::setZero64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res0); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res0 == res);

            res = Util::setOne64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res1); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res1 == res);

            Int64 dst = src;

            Util::replaceZero64(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res0); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res0 == dst);

            dst = src;

            Util::replaceOne64(&dst, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_res1); P(dst);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_res1 == dst);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'isAnySetZero' AND 'isAnySetOne' FUNCTIONS:
        //   These two functions each perform transparently-simple computations
        //   on the one input parameter.  A very modest set of test vectors is
        //   sufficient to probe the computational logic.
        //
        // Plan:
        //   For each of a tabulated sequence of individual test grouped by
        //   source category, verify that 'isAnySet0' and 'isAnySet1' each
        //   return the expected value.
        //
        // Testing:
        //   bool isAnySetZero(  int sInt);
        //   bool isAnySetZero64(int sInt);
        //   bool isAnySetOne(   int sInt);
        //   bool isAnySetOne64( int sInt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'isAnySetZero' and 'isAnySetOne' Functions" << endl
            << "==================================================" << endl;

        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_isAnySet0;   // expected result from isAnySet0
            int         d_isAnySet1;   // expected result from isAnySet1
        } DATA_A[] = {
           //L#         src  isAnySet0  isAnySet1
           //--   ---------  ---------  ---------
           { L_,     "0..0",         1,         0 },  // no position
           { L_,     "1..1",         0,         1 },

           { L_,    "01..1",         1,         1 },  // leading position
           { L_,    "10..0",         1,         1 },

           { L_,    "1..10",         1,         1 },  // trailing position
           { L_,    "0..01",         1,         1 },

           { L_,  "0..0100",         1,         1 },  // interior position
           { L_, "1..10111",         1,         1 }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::isAnySetZero(src);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_isAnySet0);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_isAnySet0 == resA);

            resA = Util::isAnySetOne(src);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_isAnySet0);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_isAnySet1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing isAnySet0, isAnySet1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE = DATA_A[di].d_lineNum;
            const char *spec = DATA_A[di].d_src;

            int src = g(spec);
            int resA = Util::isAnySet0(src);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_isAnySet0);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_isAnySet0 == resA);

            resA = Util::isAnySet1(src);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_isAnySet0);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_isAnySet1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
            << "Testing 'isAnySetZero64' and 'isAnySetOne64' Functions\n"
            << "======================================================\n";

        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_isAnySet0;   // expected result from isAnySet0
            int         d_isAnySet1;   // expected result from isAnySet1
        } DATA_B[] = {
           //L#         src  isAnySet0  isAnySet1
           //--   ---------  ---------  ---------
           { L_,     "0..0",         1,         0 },  // no position
           { L_,     "1..1",         0,         1 },

           { L_,    "01..1",         1,         1 },  // leading position
           { L_,    "10..0",         1,         1 },

           { L_,    "1..10",         1,         1 },  // trailing position
           { L_,    "0..01",         1,         1 },

           { L_,  "0..0100",         1,         1 },  // interior position
           { L_, "1..10111",         1,         1 }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE = DATA_B[di].d_lineNum;
            const char *spec = DATA_B[di].d_src;

            Int64 src = g64(spec);
            int res = Util::isAnySetZero64(src);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_isAnySet0); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_isAnySet0 == res);

            res = Util::isAnySetOne64(src);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_isAnySet1); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_isAnySet1 == res);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING EXTRACT FUNCTION:
        //   These two three-parameter functions are implemented using
        //   relatively straightforward computations involving only the
        //   arguments and already-tested functions.  The test data needs to
        //   probe only the logic of the computation.
        //
        // Plan:
        //   For each of a tabulated sequence of individual tests grouped by
        //   source category, verify that 'extract0' and 'extract1' each return
        //   the expected value.
        //
        // Testing:
        //   int extractZero(  int sInt, int sIdx, int nBits);
        //   int extractZero64(int sInt, int sIdx, int nBits);
        //   int extractOne(   int sInt, int sIdx, int nBits);
        //   int extractOne64( int sInt, int sIdx, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'extractZero' and 'extractOne' Functions" << endl
            << "================================================" << endl;

        const char *SRCA = "1011101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_index;       // source index
            int         d_numBits;     // number of bits
            int         d_extract0;    // expected result from extract0
            int         d_extract1;    // expected result from extract1
        } DATA_A[] = {
            //L#     src  Index NumBits   extract0 result  extract1 result
            //--  ------  ----- -------   ---------------  ---------------
            { L_, "0..0",     0,     0,                0,               ~0 },
            { L_, "0..0",     0,     1,                0,               ~1 },
            { L_, "0..0",     0, BPW-1,                0,          INT_MIN },
            { L_, "0..0",     0,   BPW,                0,                0 },
            { L_, "0..0",     1,     0,                0,               ~0 },
            { L_, "0..0",     1,     1,                0,               ~1 },
            { L_, "0..0",     1, BPW-1,                0,          INT_MIN },
            { L_, "0..0", BPW-1,     0,                0,               ~0 },
            { L_, "0..0", BPW-1,     1,                0,               ~1 },
            { L_, "0..0",   BPW,     0,                0,               ~0 },

            { L_, "1..1",     0,     0,                0,               ~0 },
            { L_, "1..1",     0,     1,                1,               ~0 },
            { L_, "1..1",     0, BPW-1,          INT_MAX,               ~0 },
            { L_, "1..1",     0,   BPW,               ~0,               ~0 },
            { L_, "1..1",     1,     0,                0,               ~0 },
            { L_, "1..1",     1,     1,                1,               ~0 },
            { L_, "1..1",     1, BPW-1,          INT_MAX,               ~0 },
            { L_, "1..1", BPW-1,     0,                0,               ~0 },
            { L_, "1..1", BPW-1,     1,                1,               ~0 },
            { L_, "1..1",   BPW,     0,                0,               ~0 },

            { L_,  FW_01,     0,     0,                0,               ~0 },
            { L_,  FW_01,     0,     1,                1,               ~0 },
            { L_,  FW_01,     0, BPW-1,         g(FW_01), g(FW_01)|INT_MIN },
            { L_,  FW_01,     0,   BPW,         g(FW_01),         g(FW_01) },
            { L_,  FW_01,     1,     0,                0,               ~0 },
            { L_,  FW_01,     1,     1,                0,               ~1 },
            { L_,  FW_01,     1, BPW-1, g(FW_10)&INT_MAX,         g(FW_10) },
            { L_,  FW_01, BPW-1,     0,                0,               ~0 },
            { L_,  FW_01, BPW-1,     1,                0,               ~1 },
            { L_,  FW_01,   BPW,     0,                0,               ~0 },

            { L_,  FW_10,     0,     0,                0,               ~0 },
            { L_,  FW_10,     0,     1,                0,               ~1 },
            { L_,  FW_10,     0, BPW-1, g(FW_10)&INT_MAX,         g(FW_10) },
            { L_,  FW_10,     0,   BPW,         g(FW_10),         g(FW_10) },
            { L_,  FW_10,     1,     0,                0,               ~0 },
            { L_,  FW_10,     1,     1,                1,               ~0 },
            { L_,  FW_10,     1, BPW-1,         g(FW_01), g(FW_01)|INT_MIN },
            { L_,  FW_10, BPW-1,     0,                0,               ~0 },
            { L_,  FW_10, BPW-1,     1,                1,               ~0 },
            { L_,  FW_10,   BPW,     0,                0,               ~0 },

            // SRCA =              g("1101101100100"),  g("1101101100100")
            { L_,   SRCA,  2,  4,  g("0000000001001"),  g("1..1111111001") },
            { L_,   SRCA,  2,  6,  g("0000000011001"),  g("1..1111011001") },
            { L_,   SRCA,  2,  7,  g("0000001011001"),  g("1..1111011001") },
            { L_,   SRCA,  2,  8,  g("0000011011001"),  g("1..1111011001") },
            { L_,   SRCA,  3,  4,  g("0000000001100"),  g("1..1111111100") },
            { L_,   SRCA,  3,  5,  g("0000000001100"),  g("1..1111101100") },
            { L_,   SRCA,  3,  6,  g("0000000101100"),  g("1..1111101100") },
            { L_,   SRCA,  3,  7,  g("0000001101100"),  g("1..1111101100") },
            { L_,   SRCA,  3,  8,  g("0000011101100"),  g("1..1111101100") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_src;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int src = g(spec);
            int resA = Util::extractZero(src, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract0);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_extract0 == resA);

            resA = Util::extractOne(src, index, numBits);
            if (veryVerbose) {
                P_(LINE);P_(spec);P_(DATA_A[di].d_extract1);P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_extract1 == resA);
        }

        if (verbose) cout << endl
            << "Testing 'extractZero64' and 'extractOne64' Functions" << endl
            << "====================================================" << endl;

        const char *SRCB = "1011101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_index;       // source index
            int         d_numBits;     // number of bits
            Int64       d_extract0;    // expected result from extract0
            Int64       d_extract1;    // expected result from extract1
        } DATA_B[] = {
            //L#     src  Index NumBits   extract0 result  extract1 result
            //--  ------  ----- -------   ---------------  ---------------
            { L_, "0..0",     0,     0,                0,               ~0 },
            { L_, "0..0",     0,     1,                0,               ~1 },
            { L_, "0..0",     0, BPS-1,                0,         int64Min },
            { L_, "0..0",     0,   BPS,                0,                0 },
            { L_, "0..0",     1,     0,                0,               ~0 },
            { L_, "0..0",     1,     1,                0,               ~1 },
            { L_, "0..0",     1, BPS-1,                0,         int64Min },
            { L_, "0..0", BPS-1,     0,                0,               ~0 },
            { L_, "0..0", BPS-1,     1,                0,               ~1 },
            { L_, "0..0",   BPS,     0,                0,               ~0 },

            { L_, "1..1",     0,     0,                0,               ~0 },
            { L_, "1..1",     0,     1,                1,               ~0 },
            { L_, "1..1",     0, BPS-1,         int64Max,               ~0 },
            { L_, "1..1",     0,   BPS,               ~0,               ~0 },
            { L_, "1..1",     1,     0,                0,               ~0 },
            { L_, "1..1",     1,     1,                1,               ~0 },
            { L_, "1..1",     1, BPS-1,         int64Max,               ~0 },
            { L_, "1..1", BPS-1,     0,                0,               ~0 },
            { L_, "1..1", BPS-1,     1,                1,               ~0 },
            { L_, "1..1",   BPS,     0,                0,               ~0 },

            { L_,  SW_01,     0,     0,                0,               ~0 },
            { L_,  SW_01,     0,     1,                1,               ~0 },
            { L_,  SW_01,     0, BPS-1,       g64(SW_01),g64(SW_01)|int64Min},
            { L_,  SW_01,     0,   BPS,       g64(SW_01),       g64(SW_01) },
            { L_,  SW_01,     1,     0,                0,               ~0 },
            { L_,  SW_01,     1,     1,                0,               ~1 },
            { L_,  SW_01,     1, BPS-1, g64(SW_10)&int64Max,    g64(SW_10) },
            { L_,  SW_01, BPS-1,     0,                0,               ~0 },
            { L_,  SW_01, BPS-1,     1,                0,               ~1 },
            { L_,  SW_01,   BPS,     0,                0,               ~0 },

            { L_,  SW_10,     0,     0,                0,               ~0 },
            { L_,  SW_10,     0,     1,                0,               ~1 },
            { L_,  SW_10,     0, BPS-1, g64(SW_10)&int64Max,    g64(SW_10) },
            { L_,  SW_10,     0,   BPS,       g64(SW_10),       g64(SW_10) },
            { L_,  SW_10,     1,     0,                0,               ~0 },
            { L_,  SW_10,     1,     1,                1,               ~0 },
            { L_,  SW_10,     1, BPS-1,       g64(SW_01),g64(SW_01)|int64Min},
            { L_,  SW_10, BPS-1,     0,                0,               ~0 },
            { L_,  SW_10, BPS-1,     1,                1,               ~0 },
            { L_,  SW_10,   BPS,     0,                0,               ~0 },

            // SRCB =            g64("1101101100100"),g64("1101101100100")
            { L_,   SRCB,  2,  4,g64("0000000001001"),g64("1..1111111001") },
            { L_,   SRCB,  2,  6,g64("0000000011001"),g64("1..1111011001") },
            { L_,   SRCB,  2,  7,g64("0000001011001"),g64("1..1111011001") },
            { L_,   SRCB,  2,  8,g64("0000011011001"),g64("1..1111011001") },
            { L_,   SRCB,  3,  4,g64("0000000001100"),g64("1..1111111100") },
            { L_,   SRCB,  3,  5,g64("0000000001100"),g64("1..1111101100") },
            { L_,   SRCB,  3,  6,g64("0000000101100"),g64("1..1111101100") },
            { L_,   SRCB,  3,  7,g64("0000001101100"),g64("1..1111101100") },
            { L_,   SRCA,  3,  8,g64("0000011101100"),g64("1..1111101100") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE    = DATA_B[di].d_lineNum;
            const char *spec    = DATA_B[di].d_src;
            const int   index   = DATA_B[di].d_index;
            const int   numBits = DATA_B[di].d_numBits;

            Int64 src = g64(spec);
            Int64 res = Util::extractZero64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_extract0); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_extract0 == res);

            res = Util::extractOne64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_extract1); P(res);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_extract1 == res);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING RANGE FUNCTIONS:
        //   These two functions are implemented using a bitwise computation
        //   involving only already-tested methods, and so this test needs only
        //   to probe the logic of the computation.  A relatively small number
        //   of test inputs is sufficient.  Each function has, however, three
        //   input parameters, and so the test input must be rich enough to
        //   expose subtle typographic or logical errors.
        //
        // Plan:
        //   For each of a tabulated sequence of individual tests grouped by
        //   source category, verify that the 'range0' and 'range1' functions
        //   each return the expected value.
        //
        // Testing:
        //   int rangeZero(    int sInt, int sIdx, int nBits);
        //   Int64 rangeZero64(int sInt, int sIdx, int nBits);
        //   int rangeOne(     int sInt, int sIdx, int nBits);
        //   Int64 rangeOne64( int sInt, int sIdx, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                    "Testing 'range0', 'rangeZero', 'range1' and 'rangeOne'\n"
                    "======================================================\n";

        const char *SRCA = "1011101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_index;       // source index
            int         d_numBits;     // number of bits
            int         d_range0;      // expected result from range0 function
            int         d_range1;      // expected result from range1 function
        } DATA_A[] = {
            //L#     src  Index NumBits      range0 result    range1 result
            //-- -------- ----- -------      -------------    -------------
            { L_, "0..0",     0,     0,                 0,               ~0 },
            { L_, "0..0",     0,     1,                 0,               ~1 },
            { L_, "0..0",     0, BPW-1,                 0,          INT_MIN },
            { L_, "0..0",     0,   BPW,                 0,                0 },
            { L_, "0..0",     1,     0,                 0,               ~0 },
            { L_, "0..0",     1,     1,                 0,               ~2 },
            { L_, "0..0",     1, BPW-1,                 0,                1 },
            { L_, "0..0", BPW-1,     0,                 0,               ~0 },
            { L_, "0..0", BPW-1,     1,                 0,          INT_MAX },
            { L_, "0..0",   BPW,     0,                 0,               ~0 },

            { L_, "1..1",     0,     0,                 0,               ~0 },
            { L_, "1..1",     0,     1,                 1,               ~0 },
            { L_, "1..1",     0, BPW-1,           INT_MAX,               ~0 },
            { L_, "1..1",     0,   BPW,                ~0,               ~0 },
            { L_, "1..1",     1,     0,                 0,               ~0 },
            { L_, "1..1",     1,     1,                 2,               ~0 },
            { L_, "1..1",     1, BPW-1,                ~1,               ~0 },
            { L_, "1..1", BPW-1,     0,                 0,               ~0 },
            { L_, "1..1", BPW-1,     1,           INT_MIN,               ~0 },
            { L_, "1..1",   BPW,     0,                 0,               ~0 },

            { L_,  FW_01,     0,     0,                 0,               ~0 },
            { L_,  FW_01,     0,     1,                 1,               ~0 },
            { L_,  FW_01,     0, BPW-1,          g(FW_01), g(FW_01)|INT_MIN },
            { L_,  FW_01,     0,   BPW,          g(FW_01),         g(FW_01) },
            { L_,  FW_01,     1,     0,                 0,               ~0 },
            { L_,  FW_01,     1,     1,                 0,               ~2 },
            { L_,  FW_01,     1, BPW-1,       g(FW_01)&~1,         g(FW_01) },
            { L_,  FW_01, BPW-1,     0,                 0,               ~0 },
            { L_,  FW_01, BPW-1,     1,                 0,          INT_MAX },
            { L_,  FW_01,   BPW,     0,                 0,               ~0 },

            { L_,  FW_10,     0,     0,                 0,               ~0 },
            { L_,  FW_10,     0,     1,                 0,               ~1 },
            { L_,  FW_10,     0, BPW-1,  g(FW_10)&INT_MAX,         g(FW_10) },
            { L_,  FW_10,     0,   BPW,          g(FW_10),         g(FW_10) },
            { L_,  FW_10,     1,     0,                 0,               ~0 },
            { L_,  FW_10,     1,     1,                 2,               ~0 },
            { L_,  FW_10,     1, BPW-1,          g(FW_10),       g(FW_10)|1 },
            { L_,  FW_10, BPW-1,     0,                 0,               ~0 },
            { L_,  FW_10, BPW-1,     1,           INT_MIN,               ~0 },
            { L_,  FW_10,   BPW,     0,                 0,               ~0 },

            //SRCA =               g("1011101100100")  g("001011101100100")
            { L_,   SRCA,  2,  4,  g("0000000100100"), g("1..111111100111") },
            { L_,   SRCA,  2,  5,  g("0000001100100"), g("1..111111100111") },
            { L_,   SRCA,  2,  6,  g("0000001100100"), g("1..111101100111") },
            { L_,   SRCA,  2,  7,  g("0000101100100"), g("1..111101100111") },
            { L_,   SRCA,  2,  8,  g("0001101100100"), g("1..111101100111") },
            { L_,   SRCA,  3,  4,  g("0000001100000"), g("1..111111100111") },
            { L_,   SRCA,  3,  5,  g("0000001100000"), g("1..111101100111") },
            { L_,   SRCA,  3,  6,  g("0000101100000"), g("1..111101100111") },
            { L_,   SRCA,  3,  7,  g("0001101100000"), g("1..111101100111") },
            { L_,   SRCA,  3,  8,  g("0011101100000"), g("1..111101100111") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_src;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int src = g(spec);
            int resA = Util::rangeZero(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_range0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_range0 == resA);

            resA = Util::rangeOne(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_range1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_range1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing range0, range1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int   LINE    = DATA_A[di].d_lineNum;
            const char *spec    = DATA_A[di].d_src;
            const int   index   = DATA_A[di].d_index;
            const int   numBits = DATA_A[di].d_numBits;

            int src = g(spec);
            int resA = Util::range0(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_range0); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_range0 == resA);

            resA = Util::range1(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_A[di].d_range1); P_(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_range1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << "Testing 'rangeZero64' and 'rangeOne64'\n"
                             "======================================\n";

        const char *SRCB = "1011101100100";    // typical case
        static const struct {
            int         d_lineNum;     // line number
            const char *d_src;         // source integer
            int         d_index;       // source index
            int         d_numBits;     // number of bits
            Int64       d_range0;      // expected result from range0 function
            Int64       d_range1;      // expected result from range1 function
        } DATA_B[] = {
            //L#     src  Index NumBits      range0 result    range1 result
            //-- -------- ----- -------      -------------    -------------
            { L_, "0..0",     0,     0,                 0,               -1 },
            { L_, "0..0",     0,     1,                 0,               -2 },
            { L_, "0..0",     0, BPS-1,                 0,         int64Min },
            { L_, "0..0",     0,   BPS,                 0,                0 },
            { L_, "0..0",     1,     0,                 0,               -1 },
            { L_, "0..0",     1,     1,                 0,      ~ (Int64) 2 },
            { L_, "0..0",     1, BPS-1,                 0,                1 },
            { L_, "0..0", BPS-1,     0,                 0,               -1 },
            { L_, "0..0", BPS-1,     1,                 0,         int64Max },
            { L_, "0..0",   BPS,     0,                 0,               -1 },

            { L_, "1..1",     0,     0,                 0,               -1 },
            { L_, "1..1",     0,     1,                 1,               -1 },
            { L_, "1..1",     0, BPS-1,          int64Max,               -1 },
            { L_, "1..1",     0,   BPS,                -1,               -1 },
            { L_, "1..1",     1,     0,                 0,               -1 },
            { L_, "1..1",     1,     1,                 2,               -1 },
            { L_, "1..1",     1, BPS-1,                -2,               -1 },
            { L_, "1..1", BPS-1,     0,                 0,               -1 },
            { L_, "1..1", BPS-1,     1,          int64Min,               -1 },
            { L_, "1..1",   BPS,     0,                 0,               -1 },

            { L_,  SW_01,     0,     0,                 0,               -1 },
            { L_,  SW_01,     0,     1,                 1,               -1 },
            { L_,  SW_01,     0, BPS-1,   g64(SW_01),    g64(SW_01)|int64Min},
            { L_,  SW_01,     0,   BPS,        g64(SW_01),       g64(SW_01) },
            { L_,  SW_01,     1,     0,                 0,               -1 },
            { L_,  SW_01,     1,     1,                 0,      ~ (Int64) 2 },
            { L_,  SW_01,     1, BPS-1,g64(SW_01)&~(Int64)1,    g64(SW_01) },
            { L_,  SW_01, BPS-1,     0,                 0,               -1 },
            { L_,  SW_01, BPS-1,     1,                 0,         int64Max },
            { L_,  SW_01,   BPS,     0,                 0,               -1 },

            { L_,  SW_10,     0,     0,                 0,               -1 },
            { L_,  SW_10,     0,     1,                 0,               -2 },
            { L_,  SW_10,     0, BPS-1, g64(SW_10)&int64Max,     g64(SW_10) },
            { L_,  SW_10,     0,   BPS,        g64(SW_10),       g64(SW_10) },
            { L_,  SW_10,     1,     0,                 0,               -1 },
            { L_,  SW_10,     1,     1,                 2,               -1 },
            { L_,  SW_10,     1, BPS-1,        g64(SW_10),     g64(SW_10)|1 },
            { L_,  SW_10, BPS-1,     0,                 0,               -1 },
            { L_,  SW_10, BPS-1,     1,          int64Min,               -1 },
            { L_,  SW_10,   BPS,     0,                 0,               -1 },

            //SRCB =               g("1011101100100")   g("001011101100100")
            { L_,   SRCB,  2,  4, g64("000000100100"), g64("1..11111100111") },
            { L_,   SRCB,  2,  5, g64("000001100100"), g64("1..11111100111") },
            { L_,   SRCB,  2,  6, g64("000001100100"), g64("1..11101100111") },
            { L_,   SRCB,  2,  7, g64("000101100100"), g64("1..11101100111") },
            { L_,   SRCB,  2,  8, g64("001101100100"), g64("1..11101100111") },
            { L_,   SRCB,  3,  4, g64("000001100000"), g64("1..11111100111") },
            { L_,   SRCB,  3,  5, g64("000001100000"), g64("1..11101100111") },
            { L_,   SRCB,  3,  6, g64("000101100000"), g64("1..11101100111") },
            { L_,   SRCB,  3,  7, g64("001101100000"), g64("1..11101100111") },
            { L_,   SRCB,  3,  8, g64("011101100000"), g64("1..11101100111") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int   LINE    = DATA_B[di].d_lineNum;
            const char *spec    = DATA_B[di].d_src;
            const int   index   = DATA_B[di].d_index;
            const int   numBits = DATA_B[di].d_numBits;

            Int64 src = g64(spec);
            Int64 resA = Util::rangeZero64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_range0); P(resA);
            }
            LOOP3_ASSERT(LINE, DATA_B[di].d_range0, resA,
                                                  DATA_B[di].d_range0 == resA);

            resA = Util::rangeOne64(src, index, numBits);
            if (veryVerbose) {
                P_(LINE); P_(spec); P_(DATA_B[di].d_range1); P(resA);
            }
            LOOP3_ASSERT(LINE, DATA_B[di].d_range1, resA,
                                                  DATA_B[di].d_range1 == resA);
        }

      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testA(int test)
{
    switch (test) {
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MASK0, MASK1 GENERATION FUNCTIONS:
        //   These four functions are implemented using a straightforward
        //   bitwise computation involving only already-tested methods, and so
        //   this test needs only to probe the logic of the computation.  A
        //   small number of test inputs is sufficient.
        //
        // Plan:
        //   For each of an enumerated sequence of individual tests ordered
        //   by initial input, verify that each function returns the expected
        //   value.
        //
        // Testing:
        //   int zeroMask(int index, int nBits);
        //   int maskZero64(int index, int nBits);
        //   int oneMask(int index, int nBits);
        //   int maskOne64(int index, int nBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
             << "Testing 'maskZero' and 'maskOne' Function" << endl
             << "=========================================" << endl;

        static const struct {
            int d_lineNum;     // line number
            int d_index;       // index
            int d_numBits;     // number of bits
            int d_mask0;       // expected result from mask0 function
            int d_mask1;       // expected result from mask1 function
        } DATA_A[] = {
          //L#  Index NumBits mask0 result    mask1 result
          //--  ----- ------- ------------    ------------
          { L_,    0,    0,             ~0,              0 }, // index 0
          { L_,    0,    1,             ~1,              1 },
          { L_,    0,BPW-1,        INT_MIN,        INT_MAX },
          { L_,    0,  BPW,              0,             ~0 },

          { L_,    1,    0,             ~0,              0 }, // index 1
          { L_,    1,    1,             ~2,              2 },
          { L_,    1,BPW-1,              1,             ~1 },

          { L_,BPW-1,    0,             ~0,              0 }, // BPW-1
          { L_,BPW-1,    1,        INT_MAX,        INT_MIN },

          { L_,  BPW,    0,             ~0,              0 }, // BPW

          { L_,    2,    4,g("1..1000011"),g("0..0111100") }, // typical case
          { L_,BPW-5,    3, g("110001..1"), g("001110..0") }
        };
        const int NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A;

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE     = DATA_A[di].d_lineNum;
            const int INDEX    = DATA_A[di].d_index;
            const int NUM_BITS = DATA_A[di].d_numBits;

            int resA = Util::zeroMask(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_mask0);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_mask0 == resA);

            resA = Util::oneMask(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_mask1);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_mask1 == resA);
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) cout << "Testing mask0, mask1.\n";

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE     = DATA_A[di].d_lineNum;
            const int INDEX    = DATA_A[di].d_index;
            const int NUM_BITS = DATA_A[di].d_numBits;

            int resA = Util::mask0(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_mask0);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_mask0 == resA);

            resA = Util::mask1(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_mask1);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_mask1 == resA);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << endl
             << "Testing 'maskZero64' and 'maskOne64' Function" << endl
             << "=============================================" << endl;

        static const struct {
            int   d_lineNum;     // line number
            int   d_index;       // index
            int   d_numBits;     // number of bits
            Int64 d_mask0;       // expected result from mask0 function
            Int64 d_mask1;       // expected result from mask1 function
        } DATA_B[] = {
          //L#  Index NumBits      mask0 result    mask1 result
          //--  ----- -------      ------------    ------------
          { L_,    0,      0,                ~0,              0 }, // index 0
          { L_,    0,      1,                ~1,              1 },
          { L_,    0,  BPS-1,          int64Min,       int64Max },
          { L_,    0,    BPS,                 0,             ~0 },

          { L_,    1,      0,                ~0,              0 }, // index 1
          { L_,    1,      1,                ~2,              2 },
          { L_,    1,  BPS-1,                 1,             ~1 },

          { L_,BPS-1,      0,                ~0,              0 }, // BPS-1
          { L_,BPS-1,      1,          int64Max,       int64Min },

          { L_,BPS,        0,                ~0,              0 }, // BPW

          { L_,    2,      4,  g64("1..1000011"),  g64("0..0111100") },
          { L_,BPS-5,      3,  g64("110001..1"),   g64("001110..0") }
        };
        const int NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B;

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE     = DATA_B[di].d_lineNum;
            const int INDEX    = DATA_B[di].d_index;
            const int NUM_BITS = DATA_B[di].d_numBits;

            Int64 resA = Util::zeroMask64(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_B[di].d_mask0);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_mask0 == resA);

            resA = Util::oneMask64(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_B[di].d_mask1);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_mask1 == resA);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SIX BASIC MASK GENERATION FUNCTIONS:
        //   These functions are all implemented using tabulated values, and so
        //   must be tested exhaustively.
        //
        // Plan:
        //   For each of the six single-argument mask generation functions,
        //   verify (in a loop over all valid 'index' values) that each valid
        //   'index' results in the desired mask value.  As a special case for
        //   each mask, verify that index == BITS_PER_WORD produces either 0x0
        //   or ~0 as appropriate.  Note that once a function has been tested,
        //   it may be used as an oracle for testing other functions.
        //
        // Testing:
        //   int eqMask(int index);
        //   bsls_Types::Int64 eqMask64(int index);
        //   int neMask(int index);
        //   bsls_Types::Int64 neMask64(int index);
        //   int geMask(int index);
        //   bsls_Types::Int64 geMask64(int index);
        //   int gtMask(int index);
        //   bsls_Types::Int64 gtMask64(int index);
        //   int leMask(int index);
        //   bsls_Types::Int64 leMask64(int index);
        //   int ltMask(int index);
        //   bsls_Types::Int64 ltMask64(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Mask Generation Functions" << endl
                          << "=================================" << endl;

        int i;  // This keeps MS compiler happy w/o setting flags.

        if (verbose) cout << "Testing 'eqMask'" << endl;
        for (i = 0; i < BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, 1 << i == Util::eqMask(i));
        }
        ASSERT(0 == Util::eqMask(BITS_PER_WORD));

        if (verbose) cout << "Testing 'neMask'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~(1 << i) == Util::neMask(i));
        }
        ASSERT(~0 == Util::neMask(BITS_PER_WORD));

        if (verbose) cout << "Testing 'geMask'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~0 << i == Util::geMask(i));
        }
        ASSERT(0 == Util::geMask(BITS_PER_WORD));

        if (verbose) cout << "Testing 'gtMask'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD - 1; ++i) {
            LOOP_ASSERT(i, ~0 << (i + 1) == Util::gtMask(i));
        }
        ASSERT(0 == Util::gtMask(BITS_PER_WORD - 1));
        ASSERT(0 == Util::gtMask(BITS_PER_WORD));

        if (verbose) cout << "Testing 'leMask'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD - 1; ++i) {
            LOOP_ASSERT(i, (1 << (i + 1)) - 1 == Util::leMask(i));
        }
        ASSERT(~0 == Util::leMask(BITS_PER_WORD - 1));
        ASSERT(~0 == Util::leMask(BITS_PER_WORD));

        if (verbose) cout << "Testing 'ltMask'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, (1 << i) - 1 == Util::ltMask(i));
            ASSERT(Util::ltMask(i) == ~Util::geMask(i));
        }
        ASSERT(~0 == Util::ltMask(BITS_PER_WORD));

        if (verbose) cout << "Testing relationships between mask functions\n";
        for (i = 0; i <= (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~Util::geMask(i) == Util::ltMask(i));
            LOOP_ASSERT(i, ~Util::leMask(i) == Util::gtMask(i));
            LOOP_ASSERT(i, ~Util::neMask(i) == Util::eqMask(i));
        }

        const Int64 one64 = 1;
        const Int64 zero64 = 0;

        if (verbose) cout << "Testing 'eqMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64; ++i) {
            LOOP_ASSERT(i, one64 << i == Util::eqMask64(i));
        }
        ASSERT(0 == Util::eqMask64(i));

        if (verbose) cout << "Testing 'neMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64; ++i) {
            LOOP_ASSERT(i, ~(one64 << i) == Util::neMask64(i));
        }
        ASSERT(~zero64 == Util::neMask64(i));

        if (verbose) cout << "Testing 'geMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64; ++i) {
            LOOP_ASSERT(i, ~zero64 << i == Util::geMask64(i));
        }
        ASSERT(0 == Util::geMask64(i));

        if (verbose) cout << "Testing 'gtMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64 - 1; ++i) {
            LOOP_ASSERT(i, ~zero64 << (i + 1) == Util::gtMask64(i));
        }
        ASSERT(0 == Util::gtMask64(BITS_PER_INT64 - 1));
        ASSERT(0 == Util::gtMask64(BITS_PER_INT64));

        if (verbose) cout << "Testing 'leMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64 - 1; ++i) {
            LOOP_ASSERT(i, (one64 << (i + 1)) - 1 == Util::leMask64(i));
        }
        ASSERT(~zero64 == Util::leMask64(BITS_PER_INT64 - 1));
        ASSERT(~zero64 == Util::leMask64(BITS_PER_INT64));

        if (verbose) cout << "Testing 'ltMask64'" << endl;
        for (i = 0; i < (int) BITS_PER_INT64; ++i) {
            LOOP_ASSERT(i, (one64 << i) - 1 == Util::ltMask64(i));
        }
        ASSERT(~zero64 == Util::ltMask64(i));

        if (verbose) cout << "Testing relationships between mask functions\n";
        for (i = 0; i <= (int) BITS_PER_INT64; ++i) {
            LOOP_ASSERT(i, ~Util::geMask64(i) == Util::ltMask64(i));
            LOOP_ASSERT(i, ~Util::leMask64(i) == Util::gtMask64(i));
            LOOP_ASSERT(i, ~Util::neMask64(i) == Util::eqMask64(i));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ENUM TYPE VARIABLES:
        //   Each 'enum' must have the correct value.  The value of
        //   'BITS_PER_WORD' is the result of a computation; the tests must be
        //   inspected carefully to avoid accidental duplicate mistakes in the
        //   test logic.
        //
        // Plan:
        //   Carefully define a set of 'const' local "helper" variables
        //   initialized to appropriate intermediate or final values.  Then,
        //   for each of the four 'enum' variables under test, use only the
        //   helper variables to verify that it the 'enum' holds the expected
        //   value.
        //
        // Testing:
        //   enum { WORD_SIZE = sizeof(int) };
        //   enum { BITS_PER_BYTE = 8 };
        //   enum { BITS_PER_WORD = BITS_PER_BYTE * WORD_SIZE };
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'enum' variables" << endl
                          << "========================" << endl;

        const int EIGHT     = 8;           // one fact in one place for testing
        const int LOG_OF_32 = 5;
        const int LOG_OF_64 = 6;
        const int BPW       = EIGHT * sizeof(int);

        if (veryVerbose) {
            T_();  P(EIGHT);
            T_();  P(LOG_OF_32);
            T_();  P(LOG_OF_64);
            T_();  P(BPW);  cout << endl;

            T_();  P(Util::BDES_WORD_SIZE);
            T_();  P(CHAR_BIT);
            T_();  P(Util::BDES_BITS_PER_INT);
        }
        ASSERT(sizeof(int) == Util::BDES_WORD_SIZE);
        ASSERT(EIGHT       == CHAR_BIT);
        ASSERT(BPW         == Util::BDES_BITS_PER_INT);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS, g & g64
        //   'g' must correctly parse 'spec' according to its specific
        //   language, and return the corresponding integer if 'spec' is valid.
        //   'g' must also correctly diagnose and report an invalid 'spec'.
        //   'g's error-report-suppression flag must be set before testing
        //   invalid 'spec's, and must be explicitly unset after testing.
        //
        // Plan:
        //   For each 'spec' in a tabulated sequence, verify that 'g' returns
        //   the expected value.  First specify a sequence of valid 'spec'
        //   values, then temporarily disable 'g's error-reporting and provide
        //   a sequence of invalid 'spec' values.
        //
        // Testing:
        //   GENERATOR FUNCTION: int g(const char *spec)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Functions, 'g' & 'g64'\n"
                          << "========================================\n";

        if (verbose) cout << "\nVerify behavior of g() for valid input.\n";
        {
            static const struct {
                int         d_lineNum;  // line number
                const char *d_spec;     // input spec.
                int         d_value;    // resulting value
            } DATA[] = {
                //L#  Input Specification               Resulting Value
                //--  -------------------               ---------------

                { L_, "",                               0               },
                { L_, "0",                              0               },
                { L_, "1",                              1               },
                { L_, "00",                             0               },
                { L_, "01",                             1               },
                { L_, "10",                             2               },
                { L_, "11",                             3               },
                { L_, "000",                            0               },
                { L_, "001",                            1               },
                { L_, "010",                            2               },
                { L_, "011",                            3               },
                { L_, "100",                            4               },
                { L_, "101",                            5               },
                { L_, "110",                            6               },
                { L_, "111",                            7               },

                { L_, "0..0",                           0               },
                { L_, "00..0",                          0               },
                { L_, "0..00",                          0               },
                { L_, "10..0",                          INT_MIN         },
                { L_, "0..01",                          1               },

                { L_, "1..1",                           ~0              },
                { L_, "11..1",                          ~0              },
                { L_, "1..11",                          ~0              },
                { L_, "01..1",                          INT_MAX         },
                { L_, "1..10",                          ~0^1            },

                { L_, " ",                              g("")           },
                { L_, " 0 ",                            g("0")          },
                { L_, " 1 1 ",                          g("11")         },
                { L_, " 1 0 1 0 . . 0 0 1 0 ",          g("1010..0010") },
                { L_, " 1 0 1 0 . . 0 0 1 0 ",          g("1010..0010") },

                { L_, " 0 . . 0 ",                      g("0..0")       },
                { L_, " 1 0 . . 0 ",                    g("10..0")      },
                { L_, " 0 . . 0 1 ",                    g("0..01")      },

                { L_, " 1 . . 1 ",                      g("1..1")       },
                { L_, " 0 1 . . 1 ",                    g("01..1")      },
                { L_, " 1 . . 1 0",                     g("1..10")      },

                { L_, " 01 10 .. 01 01",                g("0110..0101") },
                { L_, "\t0110.\t.0101\t",               g("0110..0101") },
                { L_, "\t0110.\t.0101\t",               g("0110..0101") },

                { L_, " 10  11. .11 10",                g("1011..1110") },
                { L_, "1   011..11   10",               g("1011..1110") },
                { L_, " 1 0 \t 1 1 . \t . 1 1 \t 1 0 ", g("1011..1110") },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int   LINE = DATA[di].d_lineNum;
                const char *spec = DATA[di].d_spec;
                int         exp  = DATA[di].d_value;
                int         res  = g(DATA[di].d_spec);

                if (veryVerbose) { P_(LINE); P_(spec); P_(exp); P(res); }
                LOOP_ASSERT(LINE, res == exp);
            }
        }

        if (verbose) cout << "\nVerify invalid input is detected by g().\n";
        {
            static const struct {
                int         d_lineNum;  // line number
                const char *d_spec;     // input spec.
                int         d_value;    // resulting error code
            } DATA[] = {
                //L#  Input Specification   Resulting Error Code
                //--  -------------------   --------------------
                { L_, "A",                  G_ILLEGAL_CHARACTER     },
                { L_, "2",                  G_ILLEGAL_CHARACTER     },
                { L_, ":",                  G_ILLEGAL_CHARACTER     },
                { L_, "z0",                 G_ILLEGAL_CHARACTER     },
                { L_, "09",                 G_ILLEGAL_CHARACTER     },
                { L_, "0_0",                G_ILLEGAL_CHARACTER     },

                { L_, "0..1",               G_MISMATCHED_RANGE      },
                { L_, "0..10",              G_MISMATCHED_RANGE      },
                { L_, "0..11",              G_MISMATCHED_RANGE      },
                { L_, "00..1",              G_MISMATCHED_RANGE      },
                { L_, "10..1",              G_MISMATCHED_RANGE      },

                { L_, "1..0",               G_MISMATCHED_RANGE      },
                { L_, "1..00",              G_MISMATCHED_RANGE      },
                { L_, "1..01",              G_MISMATCHED_RANGE      },
                { L_, "01..0",              G_MISMATCHED_RANGE      },
                { L_, "11..0",              G_MISMATCHED_RANGE      },

                { L_, "0..",                G_MISSING_RANGE_END     },
                { L_, "00..",               G_MISSING_RANGE_END     },
                { L_, "10..",               G_MISSING_RANGE_END     },

                { L_, "1..",                G_MISSING_RANGE_END     },
                { L_, "01..",               G_MISSING_RANGE_END     },
                { L_, "11..",               G_MISSING_RANGE_END     },

                { L_, "..0",                G_MISSING_RANGE_START   },
                { L_, "..00",               G_MISSING_RANGE_START   },
                { L_, "..01",               G_MISSING_RANGE_START   },

                { L_, "..1",                G_MISSING_RANGE_START   },
                { L_, "..10",               G_MISSING_RANGE_START   },
                { L_, "..11",               G_MISSING_RANGE_START   },

                { L_, "0.1",                G_MISSING_SECOND_DOT    },
                { L_, "0.10",               G_MISSING_SECOND_DOT    },
                { L_, "0.11",               G_MISSING_SECOND_DOT    },
                { L_, "00.1",               G_MISSING_SECOND_DOT    },
                { L_, "10.1",               G_MISSING_SECOND_DOT    },

                { L_, "1.0",                G_MISSING_SECOND_DOT    },
                { L_, "1.00",               G_MISSING_SECOND_DOT    },
                { L_, "1.01",               G_MISSING_SECOND_DOT    },
                { L_, "01.0",               G_MISSING_SECOND_DOT    },
                { L_, "11.0",               G_MISSING_SECOND_DOT    },

                { L_, "0..0.",              G_MULTIPLE_RANGES       },
                { L_, "0..00.",             G_MULTIPLE_RANGES       },
                { L_, "0..01.",             G_MULTIPLE_RANGES       },

                { L_, "1..1.",              G_MULTIPLE_RANGES       },
                { L_, "1..10.",             G_MULTIPLE_RANGES       },
                { L_, "1..11.",             G_MULTIPLE_RANGES       },

                { L_, FW_0,                 0                       },
                { L_, FW_0"0",              G_TOO_MANY_BITS         },
                { L_, FW_0"1",              G_TOO_MANY_BITS         },
                { L_, "0"FW_0,              G_TOO_MANY_BITS         },
                { L_, "1"FW_0,              G_TOO_MANY_BITS         },

                { L_, FW_1,                 -1                      },
                { L_, FW_1"0",              G_TOO_MANY_BITS         },
                { L_, FW_1"1",              G_TOO_MANY_BITS         },
                { L_, "0"FW_1,              G_TOO_MANY_BITS         },
                { L_, "1"FW_1,              G_TOO_MANY_BITS         },

                { L_, "1..1"FW_0,           0                       },
                { L_, HW_0"1..1"HW_0,       0                       },
                { L_, FW_0"1..1",           0                       },

                { L_, "1..1"FW_0"0",        G_TOO_MANY_BITS         },
                { L_, HW_0"1..1"HW_0"0",    G_TOO_MANY_BITS         },
                { L_, "0"FW_0"1..1",        G_TOO_MANY_BITS         },

                { L_, "0..0"FW_1,           -1                      },
                { L_, HW_1"0..0"HW_1,       -1                      },
                { L_, FW_1"0..0",           -1                      },

                { L_, "0..0"FW_1"0",        G_TOO_MANY_BITS         },
                { L_, HW_1"0..0"HW_1"1",    G_TOO_MANY_BITS         },
                { L_, "1"FW_1"0..0",        G_TOO_MANY_BITS         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            G_OFF = 1;  // set to 1 to enable testing of G function errors

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int   LINE = DATA[di].d_lineNum;
                const char *spec = DATA[di].d_spec;
                int         exp  = DATA[di].d_value;
                int         res  = g(DATA[di].d_spec);

                if (veryVerbose) { P_(LINE); P_(spec); P_(exp); P(res); }

                LOOP_ASSERT(LINE, res == exp);
            }

            G_OFF = 0;  // set to 1 to enable testing of G function errors
        }

        if (verbose) cout <<
                    "\nVerify behavior of the g64 function for valid input.\n";
        {
            static const struct {
                int         d_lineNum;  // line number
                const char *d_spec;     // input spec.
                Int64       d_value;    // resulting value
            } DATA[] = {
                //L#  Input Specification               Resulting Value
                //--  -------------------               ---------------

                { L_, "",                               0               },
                { L_, "0",                              0               },
                { L_, "1",                              1               },
                { L_, "00",                             0               },
                { L_, "01",                             1               },
                { L_, "10",                             2               },
                { L_, "11",                             3               },
                { L_, "000",                            0               },
                { L_, "001",                            1               },
                { L_, "010",                            2               },
                { L_, "011",                            3               },
                { L_, "100",                            4               },
                { L_, "101",                            5               },
                { L_, "110",                            6               },
                { L_, "111",                            7               },

                { L_, "0..0",                           0               },
                { L_, "00..0",                          0               },
                { L_, "0..00",                          0               },
                { L_, "10..0",                          int64Min        },
                { L_, "0..01",                          1               },

                { L_, "1..1",                           -1              },
                { L_, "11..1",                          -1              },
                { L_, "1..11",                          -1              },
                { L_, "01..1",                          int64Max        },
                { L_, "1..10",                          -2              },

                { L_, " ",                              g64("")           },
                { L_, " 0 ",                            g64("0")          },
                { L_, " 1 1 ",                          g64("11")         },
                { L_, " 1 0 1 0 . . 0 0 1 0 ",          g64("1010..0010") },
                { L_, " 1 0 1 0 . . 0 0 1 0 ",          g64("1010..0010") },

                { L_, " 0 . . 0 ",                      g64("0..0")       },
                { L_, " 1 0 . . 0 ",                    g64("10..0")      },
                { L_, " 0 . . 0 1 ",                    g64("0..01")      },

                { L_, " 1 . . 1 ",                      g64("1..1")       },
                { L_, " 0 1 . . 1 ",                    g64("01..1")      },
                { L_, " 1 . . 1 0",                     g64("1..10")      },

                { L_, " 01 10 .. 01 01",                g64("0110..0101") },
                { L_, "\t0110.\t.0101\t",               g64("0110..0101") },
                { L_, "\t0110.\t.0101\t",               g64("0110..0101") },

                { L_, " 10  11. .11 10",                g64("1011..1110") },
                { L_, "1   011..11   10",               g64("1011..1110") },
                { L_, " 1 0 \t 1 1 . \t . 1 1 \t 1 0 ", g64("1011..1110") },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int   LINE =     DATA[di].d_lineNum;
                const char *spec =     DATA[di].d_spec;
                Int64       exp  =     DATA[di].d_value;
                Int64       res  = g64(DATA[di].d_spec);

                if (veryVerbose) { P_(LINE); P_(spec); P_(exp); P(res); }
                LOOP3_ASSERT(LINE, res, exp, res == exp);
            }
        }

        if (verbose) cout << "\nVerify invalid input is detected." << endl;
        {
            static const struct {
                int         d_lineNum;  // line number
                const char *d_spec;     // input spec.
                Int64       d_value;    // resulting error code
            } DATA[] = {
                //L#  Input Specification   Resulting Error Code
                //--  -------------------   --------------------
                { L_, "A",                  G_ILLEGAL_CHARACTER     },
                { L_, "2",                  G_ILLEGAL_CHARACTER     },
                { L_, ":",                  G_ILLEGAL_CHARACTER     },
                { L_, "z0",                 G_ILLEGAL_CHARACTER     },
                { L_, "09",                 G_ILLEGAL_CHARACTER     },
                { L_, "0_0",                G_ILLEGAL_CHARACTER     },

                { L_, "0..1",               G_MISMATCHED_RANGE      },
                { L_, "0..10",              G_MISMATCHED_RANGE      },
                { L_, "0..11",              G_MISMATCHED_RANGE      },
                { L_, "00..1",              G_MISMATCHED_RANGE      },
                { L_, "10..1",              G_MISMATCHED_RANGE      },

                { L_, "1..0",               G_MISMATCHED_RANGE      },
                { L_, "1..00",              G_MISMATCHED_RANGE      },
                { L_, "1..01",              G_MISMATCHED_RANGE      },
                { L_, "01..0",              G_MISMATCHED_RANGE      },
                { L_, "11..0",              G_MISMATCHED_RANGE      },

                { L_, "0..",                G_MISSING_RANGE_END     },
                { L_, "00..",               G_MISSING_RANGE_END     },
                { L_, "10..",               G_MISSING_RANGE_END     },

                { L_, "1..",                G_MISSING_RANGE_END     },
                { L_, "01..",               G_MISSING_RANGE_END     },
                { L_, "11..",               G_MISSING_RANGE_END     },

                { L_, "..0",                G_MISSING_RANGE_START   },
                { L_, "..00",               G_MISSING_RANGE_START   },
                { L_, "..01",               G_MISSING_RANGE_START   },

                { L_, "..1",                G_MISSING_RANGE_START   },
                { L_, "..10",               G_MISSING_RANGE_START   },
                { L_, "..11",               G_MISSING_RANGE_START   },

                { L_, "0.1",                G_MISSING_SECOND_DOT    },
                { L_, "0.10",               G_MISSING_SECOND_DOT    },
                { L_, "0.11",               G_MISSING_SECOND_DOT    },
                { L_, "00.1",               G_MISSING_SECOND_DOT    },
                { L_, "10.1",               G_MISSING_SECOND_DOT    },

                { L_, "1.0",                G_MISSING_SECOND_DOT    },
                { L_, "1.00",               G_MISSING_SECOND_DOT    },
                { L_, "1.01",               G_MISSING_SECOND_DOT    },
                { L_, "01.0",               G_MISSING_SECOND_DOT    },
                { L_, "11.0",               G_MISSING_SECOND_DOT    },

                { L_, "0..0.",              G_MULTIPLE_RANGES       },
                { L_, "0..00.",             G_MULTIPLE_RANGES       },
                { L_, "0..01.",             G_MULTIPLE_RANGES       },

                { L_, "1..1.",              G_MULTIPLE_RANGES       },
                { L_, "1..10.",             G_MULTIPLE_RANGES       },
                { L_, "1..11.",             G_MULTIPLE_RANGES       },

                { L_, SW_0,                 0                       },
                { L_, SW_0"0",              G_TOO_MANY_BITS         },
                { L_, SW_0"1",              G_TOO_MANY_BITS         },
                { L_, "0"SW_0,              G_TOO_MANY_BITS         },
                { L_, "1"SW_0,              G_TOO_MANY_BITS         },

                { L_, SW_1,                 -1                      },
                { L_, SW_1"0",              G_TOO_MANY_BITS         },
                { L_, SW_1"1",              G_TOO_MANY_BITS         },
                { L_, "0"SW_1,              G_TOO_MANY_BITS         },
                { L_, "1"SW_1,              G_TOO_MANY_BITS         },

                { L_, "1..1"SW_0,           0                       },
                { L_, FW_0"1..1"FW_0,       0                       },
                { L_, SW_0"1..1",           0                       },

                { L_, "1..1"SW_0"0",        G_TOO_MANY_BITS         },
                { L_, FW_0"1..1"FW_0"0",    G_TOO_MANY_BITS         },
                { L_, "0"SW_0"1..1",        G_TOO_MANY_BITS         },

                { L_, "0..0"SW_1,           -1                      },
                { L_, FW_1"0..0"FW_1,       -1                      },
                { L_, SW_1"0..0",           -1                      },

                { L_, "0..0"SW_1"0",        G_TOO_MANY_BITS         },
                { L_, FW_1"0..0"FW_1"1",    G_TOO_MANY_BITS         },
                { L_, "1"SW_1"0..0",        G_TOO_MANY_BITS         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            G_OFF = 1;  // set to 1 to enable testing of G function errors

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int   LINE =     DATA[di].d_lineNum;
                const char *spec =     DATA[di].d_spec;
                Int64       exp  =     DATA[di].d_value;
                Int64       res  = g64(DATA[di].d_spec);

                if (veryVerbose) { P_(LINE); P_(spec); P_(exp); P(res); }

                LOOP3_ASSERT(LINE, res, exp, res == exp);
            }

            G_OFF = 0;  // set to 1 to enable testing of G function errors
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   A utility component typically does not need a breathing test.
        //   This case is provided as a temporary workspace during development.
        //
        // Testing:
        //   This case exercises functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

      } break;
      default: {
        LOOP_ASSERT(test, 0 && "test not found");
      } break;
    }
}

static
void testMinus(int test)
{
    switch (test) {
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST:
        //   The shift implementation of 'find0AtLargestIndex' and related
        //   functions on some platforms can be very slow for long long.  Make
        //   measurements.
        //
        // Testing:
        //   This case exercises functionality.
        // --------------------------------------------------------------------

        performanceTest();
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // DOMAIN TEST:
        //   Check behavior of shifts over wide domain.
        // --------------------------------------------------------------------

        domainTest();
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // TEST OF CASTING TO INT64 FROM '!'
        // --------------------------------------------------------------------

        for (int i = 0; i < 64; ++i) {
            for (int b = 0; b < 2; ++b) {
                Int64 j = (Int64) 1 << i;
                Int64 k = j & -!b;
                LOOP_ASSERT(i, b || j == k);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 41: {
        testUsage();
      } break;
      case 40:
      case 39:
      case 38:
      case 37:
      case 36: {
        testH(test);
      } break;
      case 35:
      case 34:
      case 33:
      case 32:
      case 31: {
        testG(test);
      } break;
      case 30:
      case 29:
      case 28:
      case 27:
      case 26: {
        testF(test);
      } break;
      case 25:
      case 24:
      case 23:
      case 22:
      case 21: {
        testE(test);
      } break;
      case 20:
      case 19:
      case 18:
      case 17:
      case 16: {
        testD(test);
      } break;
      case 15:
      case 14:
      case 13:
      case 12:
      case 11: {
        testC(test);
      } break;
      case 10:
      case 9:
      case 8:
      case 7:
      case 6: {
        testB(test);
      } break;
      case 5:
      case 4:
      case 3:
      case 2:
      case 1: {
        testA(test);
      } break;
      case -1:
      case -2:
      case -3:
      default: {
        testMinus(test);
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
