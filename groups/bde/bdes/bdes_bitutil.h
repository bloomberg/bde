// bdes_bitutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDES_BITUTIL
#define INCLUDED_BDES_BITUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide bit-manipulation functionality for 32- and 64-bit integers.
//
//@CLASSES:
//  bdes_BitUtil: namespace for efficient bit-level operations on integers
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a utility 'struct', 'bdes_BitUtil',
// which serves as a namespace for a collection of efficient, bit-level
// procedures on the built-in 32- and 64-bit integer types, 'int' and
// 'bsls_Types::Int64', respectively.  These class-'static' methods fall
// into three categories:
//
//: 1 Mask Generators -- e.g., 'eqMask', 'gtMask', 'leMask', 'oneMask'
//:
//: 2 Manipulators -- e.g., 'andEq', insertZero', merge, 'replaceValueUpTo'
//:
//: 3 Accessors -- e.g., 'extractZero', 'intAbs', numSetOne, 'setValue'
//
// In general, each routine that takes an 'int', e.g.,
//..
//  numSetOne(int)
//..
// has a corresponding routine that takes a 'bsls_Types::Int64', and whose name
// is suffixed with "64", e.g.,
//..
//  numSetOne64(bsls_Types::Int64)
//..
// Note that, in C and C++, shifting a value by the number of bits in that
// value's type (e.g., shifting an 'int' value by 32) is undefined behavior.
// One advantage of invoking these routines, instead of performing bitwise
// operations directly, is that these routines gracefully handle the scenario
// above with minimal impact on performance.
//
///Usage
///-----
// The following usage examples illustrate how members of each of the three
// categories of methods are used.  Note that, in all of these examples, the
// low-order bit is considered bit '0' and resides on the right edge of the
// bit string.
//
///Mask Generators
///- - - - - - - -
// 'geMask' will return a bit mask with all bits below the specified 'index'
// cleared, and all bits at or above the 'index' set:
//..
//  assert(static_cast<int>(0xffff0000) == bdes_BitUtil::geMask(16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::geMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits at and above bit 16 are set:  11111111111111110000000000000000 |
//  +------------------------------------------------------------------------*/
//..
// Similarly, 'ltMask' returns a bit mask with all bits above the specified
// 'index' cleared, and all bits at or below 'index' set:
//..
//  assert(static_cast<int>(0x0000ffff) == bdes_BitUtil::ltMask(16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::ltMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits at and below bit 16 are set:  00000000000000001111111111111111 |
//  +------------------------------------------------------------------------*/
//..
// 'eqMask' returns a bit mask with only the bit at the specified 'index' set:
//..
//  assert(static_cast<int>(0x00800000) == bdes_BitUtil::eqMask(23));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::eqMask(23)' in binary:                                   |
//  |                                                                         |
//  | bit 23:                                        *                        |
//  | Only bit 23 is set:                    00000000100000000000000000000000 |
//  +------------------------------------------------------------------------*/
//..
// Similarly, 'neMask' returns a bit mask with only the bit at the specified
// 'index' cleared:
//..
//  assert(static_cast<int>(0xfffeffff) == bdes_BitUtil::neMask(16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::neMask(16)' in binary:                                   |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | All bits other than bit 16 are set:    11111111111111101111111111111111 |
//  +------------------------------------------------------------------------*/
//..
// Finally, 'oneMask' ('zeroMask') returns a bit mask with all bits within a
// specified 'range' set (cleared):
//..
//  assert(static_cast<int>(0x000f0000) == bdes_BitUtil::oneMask(16, 4));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::oneMask(16, 4)' in binary:                               |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits set:           00000000000011110000000000000000 |
//  +------------------------------------------------------------------------*/
//
//  assert(static_cast<int>(0xfff0ffff) == bdes_BitUtil::zeroMask(16, 4));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::zeroMask(16, 4)' in binary:                              |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits cleared:       11111111111100001111111111111111 |
//  +------------------------------------------------------------------------*/
//..
///Manipulators
/// - - - - - -
// The overloaded 'andEqual' logically ANDs a given bit (or set of bits) with
// an integer operand:
//..
//  int myInt = 0x3333;
//  bdes_BitUtil::andEqual(&myInt, 9, false);
//  assert(0x3133 == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::andEqual(&myInt, 9, false)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:                                11001100110011 |
//  | bit 9:                                                       *          |
//  | and Equal(&myInt, 9, false) clears bit 9:                    0          |
//  | 'myInt' after in binary:                                 11000100110011 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x3333;
//  bdes_BitUtil::andEqual(&myInt, 9, true);
//  assert(0x3333 == myInt);
//
//  myInt = 0x3333;
//  bdes_BitUtil::andEqual(&myInt, 8,  0, 8);
//  assert(  0x33 == myInt);
//
//  myInt = 0x3333;
//  bdes_BitUtil::andEqual(&myInt, 8, -1, 8);    // Note '-1' is all 1's.
//  assert(0x3333 == myInt);
//..
// 'insertOne' ('insertZero') will split a number open and insert 1's (0's)
// in the middle of it:
//..
//  myInt = 0x3333;
//  bdes_BitUtil::insertOne(&myInt, 4, 8);
//  assert(0x333ff3 == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::insertOne(&myInt, 4, 8)' in binary:                      |
//  |                                                                         |
//  | 'myInt' before in binary:                                11001100110011 |
//  | shift bits starting at 4 left 8 to make room:    1100110011        0011 |
//  | insert 8 1's:                                              11111111     |
//  | 'myInt' after in binary:                         1100110011111111110011 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x3333;
//  bdes_BitUtil::insertZero(&myInt, 4, 8);
//  assert(0x333003 == myInt);
//..
// 'orEqual' will take a slice of a second integer 'srcInteger' and bitwise
// or it with another integer:
//..
//  myInt = 0x33333333;
//  bdes_BitUtil::orEqual(&myInt, 16, -1, 8);
//  assert(static_cast<int>(0x33ff3333) == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::orEqual(&myInt, 16, -1, 8)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:              00110011001100110011001100110011 |
//  | 'srcInteger == -1' in binary:          11111111111111111111111111111111 |
//  | bitwise OR the 8 bits of 'srcInteger'                                   |
//  | at index 16:                                   11111111                 |
//  | 'myInt' after in binary:               00110011111111110011001100110011 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0;
//  bdes_BitUtil::orEqual(&myInt, 16, -1, 8);
//  assert(static_cast<int>(0x00ff0000) == myInt);
//..
// 'removeOne' and 'removeZero' each removes bits from the middle of a number;
// 'removeOne' will fill in the vacated high-order bits with 1's, while
// 'removeZero' will fill them in with 0's:
//..
//  myInt = 0x12345678;
//  bdes_BitUtil::removeOne(&myInt, 8, 16);
//  assert(static_cast<int>(0xffff1278) == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::removeOne(&myInt, 8, 16)' in binary:                     |
//  |                                                                         |
//  | 'myInt' before in binary:              00010010001101000101011001111000 |
//  | remove 16 bits starting at bit 8:      00010010                01111000 |
//  | replace empty bits with upper bits:                    0001001001111000 |
//  | fill all vacated top bits with 1's:    1111111111111111                 |
//  | 'myInt' after in binary:               11111111111111110001001001111000 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x12345678;
//  bdes_BitUtil::removeZero(&myInt, 8, 16);
//  assert(static_cast<int>(0x00001278) == myInt);
//
//  myInt = 0;
//  bdes_BitUtil::removeOne(&myInt, 8, 16);
//  assert(static_cast<int>(0xffff0000) == myInt);
//..
// 'toggle' will toggle either one bit or a range of bits:
//..
//  myInt = 0x11111111;
//  bdes_BitUtil::toggle(&myInt, 8, 16);
//  assert(static_cast<int>(0x11eeee11) == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::toggle(&myInt, 8, 16)' in binary:                        |
//  |                                                                         |
//  | 'myInt' before in binary:              00010001000100010001000100010001 |
//  | toggle 16 bits starting at bit 8:              ****************         |
//  | 'myInt' after in binary:               00010001111011101110111000010001 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x55555555;
//  bdes_BitUtil::toggle(&myInt, 8, 16);
//  assert(static_cast<int>(0x55aaaa55) == myInt);
//..
// One overload of 'xorEqual' will XOR a single bit at a specified index with
// a boolean value:
//..
//  myInt = 0x11111111;
//  bdes_BitUtil::xorEqual(&myInt, 16, true);
//  assert(static_cast<int>(0x11101111) == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::xorEqual(&myInt, 16, true)' in binary:                   |
//  |                                                                         |
//  | 'myInt' before in binary:              00010001000100010001000100010001 |
//  | xor bit 16:                                           1                 |
//  | 'myInt' after in binary:               00010001000100000001000100010001 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x11111111;
//  bdes_BitUtil::xorEqual(&myInt, 16, false);
//  assert(static_cast<int>(0x11111111) == myInt);
//..
// Another overload of 'xorEqual' will take a section of a second integer
// and XOR it into the first:
//..
//  myInt = 0x77777777;
//  bdes_BitUtil::xorEqual(&myInt, 16, 0xff, 8);
//  assert(static_cast<int>(0x77887777) == myInt);
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::xorEqual(&myInt, 16, 0xff, 8)' in binary:                |
//  |                                                                         |
//  | 'myInt' before in binary:              01110111011101110111011101110111 |
//  | bits to affect: 8 bits at offset 16:           ********                 |
//  | value to put in those 8 bits is 0xff:          11111111                 |
//  | xor those 8 bits, leaving other bits unaffected                         |
//  | 'myInt' after in binary:               01110111100010000111011101110111 |
//  +------------------------------------------------------------------------*/
//
//  myInt = 0x77777777;
//  bdes_BitUtil::xorEqual(&myInt, 16, 0x55, 8);
//  assert(static_cast<int>(0x77227777) == myInt);
//..
///Accessors
///- - - - -
// The suite of "find[01]At(Largest|Smallest)*" routines are used for finding
// the highest (or lowest) set (or cleared) bits in an integer (or subrange of
// the integer):
//..
//  assert( 8 == bdes_BitUtil::find1AtLargestIndex(0x00000101));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtLargestIndex(0x00000101)' in binary:              |
//  |                                                                         |
//  | input:                                 00000000000000000000000100000001 |
//  | highest set bit:                                              1         |
//  | index of highest set bit == 8                                           |
//  +------------------------------------------------------------------------*/
//
//  assert( 8 == bdes_BitUtil::find0AtLargestIndex( ~0x00000101));
//  assert( 8 == bdes_BitUtil::find0AtLargestIndex(  0xfffffe55));
//  assert( 8 == bdes_BitUtil::find1AtLargestIndexGE(0x00000101,  3));
//  assert(-1 == bdes_BitUtil::find1AtLargestIndexGE(0x00000155, 16));
//  assert(-1 == bdes_BitUtil::find1AtLargestIndexLT(0xffff0100,  3));
//
//  assert( 8 == bdes_BitUtil::find1AtLargestIndexLT(0xffff0100, 16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtLargestIndexLT(0xffff0100, 16)' in binary:        |
//  |                                                                         |
//  | input:                                 11111111111111110000000100000000 |
//  | bit 16:                                               *                 |
//  | range to look for highest bit in:                      **************** |
//  | highest set bit in that range:                                1         |
//  | index of that bit is 8                                                  |
//  +------------------------------------------------------------------------*/
//
//  assert( 8 == bdes_BitUtil::find1AtSmallestIndex(  0xffff0100));
//  assert(16 == bdes_BitUtil::find1AtSmallestIndexGE(0xffff0100, 10));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::find1AtSmallestIndexGE(0xffff0100, 10)' in binary:       |
//  |                                                                         |
//  | input:                                 11111111111111110000000100000000 |
//  | bit 10:                                                     *           |
//  | range to look for lowest bit in:       **********************           |
//  | lowest set bit in that range:                         1                 |
//  | index of that bit is 16                                                 |
//  +------------------------------------------------------------------------*/
//
//  assert( 8 == bdes_BitUtil::find1AtSmallestIndex(0xffff0100));
//..
// 'intAbs', 'intMax', and 'intMin' are faster than 'bsl::abs', 'bsl::max',
// 'bsl::min', and are not template functions:
//..
//  assert(      6 == bdes_BitUtil::intAbs( 6));
//  assert(      6 == bdes_BitUtil::intAbs(-6));
//  assert(INT_MAX == bdes_BitUtil::intAbs(-INT_MAX));
//  assert(INT_MIN == bdes_BitUtil::intAbs( INT_MIN));   // see note below
//
//  assert(12 == bdes_BitUtil::intMax(-5, 12));
//  assert(-5 == bdes_BitUtil::intMin(-5, 12));
//..
// Note that 'INT_MIN' is a special case.  Since the maximum representable
// positive integer, 'INT_MAX', is one less the absolute value of 'INT_MIN',
// 'bdes_BitUtil::intAbs(INT_MIN)' cannot be represented as a positive integer.
// Therefore, the function has no effect and returns 'INT_MIN'.
//
// 'merge' takes 3 numbers, 'zeroSrcInteger', 'oneSrcInteger', and 'mask'.
// For each bit of output, this function looks at the corresponding bit of
// 'mask' -- if a '0', it takes the corresponding bit of 'zeroSrcInteger'; if
// a '1', it takes the corresponding bit of 'oneSrcInteger':
//..
//  assert(static_cast<int>(0x0f0f0f0f) ==
//                    bdes_BitUtil::merge(0x0f0f0000, 0xffff0f0f, 0x0000ffff));
//
//  /*------------------------------------------------------------------------+
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
//  +------------------------------------------------------------------------*/
//
//  assert(static_cast<int>(0x55555555) ==
//                    bdes_BitUtil::merge(0x00000000, 0xffffffff, 0x55555555));
//..
// 'numSetOne' ('numSetZero') counts the number of set (cleared) bits:
//..
//  assert(16 == bdes_BitUtil::numSetOne(0x55555555));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::numSetOne( 0x55555555)' in binary:                       |
//  |                                                                         |
//  | input in binary:                       01010101010101010101010101010101 |
//  | that has 16 ones set.  result: 16                                       |
//  +------------------------------------------------------------------------*/
//
//  assert(16 == bdes_BitUtil::numSetOne( 0xffff0000));
//  assert(16 == bdes_BitUtil::numSetOne( 0x0000ffff));
//  assert( 1 == bdes_BitUtil::numSetOne( 0x00010000));
//  assert(31 == bdes_BitUtil::numSetZero(0x00010000));
//..
// 'rangeZero' and 'rangeOne' mask out a sub-range of bits from a value,
// filling the rest with '0's or '1's:
//..
//  assert(static_cast<int>(0x00333300) ==
//                                 bdes_BitUtil::rangeZero(0x33333333, 8, 16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::rangeZero(0x33333333,  8, 16)' in binary:                |
//  |                                                                         |
//  | input in binary:                       00110011001100110011001100110011 |
//  | select 16 bits starting at bit 8:              ****************         |
//  | all other bits 0:                      00000000                00000000 |
//  | result:                                00000000001100110011001100000000 |
//  +------------------------------------------------------------------------*/
//
//  assert(static_cast<int>(0x00f0f000) ==
//                                 bdes_BitUtil::rangeZero(0xf0f0f0f0, 8, 16));
//  assert(static_cast<int>(0xff3333ff) ==
//                                 bdes_BitUtil::rangeOne (0x33333333, 8, 16));
//..
// 'base2Log' takes a binary log of a number, rounded up to the next integer:
//..
//  assert(7 == bdes_BitUtil::base2Log(100));
//  assert(7 == bdes_BitUtil::base2Log(128));
//  assert(8 == bdes_BitUtil::base2Log(129));
//..
// 'roundUpToBinaryPower' rounds a number up to the next power of 2:
//..
//  assert(128 == bdes_BitUtil::roundUpToBinaryPower(100));
//  assert(128 == bdes_BitUtil::roundUpToBinaryPower(128));
//..
// 'setBitOne' will set a bit (or bits) to one, leaving other bits unaffected:
//..
//  assert(static_cast<int>(0x66676666) ==
//                                    bdes_BitUtil::setBitOne(0x66666666, 16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdes_BitUtil::setBitOne(0x66666666, 16)' in binary:                    |
//  |                                                                         |
//  | srcInteger in binary:                  01100110011001100110011001100110 |
//  | set bit 16:                                           1                 |
//  | result:                                01100110011001110110011001100110 |
//  +------------------------------------------------------------------------*/
//
//  assert(static_cast<int>(0x00010000) == bdes_BitUtil::setBitOne(0, 16));
//
//  assert(static_cast<int>(0x33ffff33) ==
//                                    bdes_BitUtil::setOne(0x33333333, 8, 16));
//..
// 'sizeInBits' will return the size of an object in bits, rather than bytes:
//..
//  char              thisChar   = 0;
//  short             thisShort  = 0;
//  int               thisInt    = 0;
//  bsls_Types::Int64 thisBigInt = 0;
//
//  assert( 8 == bdes_BitUtil::sizeInBits(thisChar));
//  assert(16 == bdes_BitUtil::sizeInBits(thisShort));
//  assert(32 == bdes_BitUtil::sizeInBits(thisInt));
//  assert(64 == bdes_BitUtil::sizeInBits(thisBigInt));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

// TBD remove after verifying dev build (through api)
#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifdef BSLS_PLATFORM__CMP_IBM
#ifndef INCLUDED_BUILTINS
#include <builtins.h>
#define INCLUDED_BUILTINS
#endif
#endif

#ifdef BSLS_PLATFORM__CMP_HP
#ifndef INCLUDED_MACHINE_SYS_BUILTINS
#include <machine/sys/builtins.h>
#define INCLUDED_MACHINE_SYS_BUILTINS
#endif
#endif

namespace BloombergLP {

                               // ===================
                               // struct bdes_BitUtil
                               // ===================

struct bdes_BitUtil {
    // This utility 'struct' provides a namespace for a set of pure, bit-level
    // procedures that operate on the built-in 32- and 64-bit integer types
    // 'int' and 'bsls_Types::Int64', respectively.  The class-'static' methods
    // are organized into three regions: (1) Mask Generators, e.g., 'eqMask';
    // (2) Manipulators, e.g., 'andEq'; and (3) Accessors, e.g., 'extractZero'.
    // In general, each method that takes an 'int' -- e.g., 'numSetOne(int)' --
    // also has a corresponding routine that takes a 'bsls_Types::Int64', and
    // whose name is suffixed with "64" -- e.g.,
    // 'numSetOne64(bsls_Types::Int64)'.

    // TYPES
    typedef bsls_Types::Int64  Int64;
    typedef bsls_Types::Uint64 Uint64;
        // 'Int64' and 'Uint64' are aliases for the native 64-bit signed
        // integer type and 64-bit unsigned integer type, respectively.

    // CONSTANTS
    enum {
        BDES_WORD_SIZE      = sizeof(int),
        BDES_INT64_SIZE     = sizeof(Int64),
        BDES_BITS_PER_INT   = CHAR_BIT * BDES_WORD_SIZE,
        BDES_BITS_PER_INT64 = CHAR_BIT * BDES_INT64_SIZE
#ifndef BDE_OMIT_TRANSITIONAL // DEPRECATED
      , BDES_BITS_PER_BYTE  = CHAR_BIT
      , BDES_BITS_PER_WORD  = BDES_BITS_PER_INT
#endif  // BDE_OMIT_TRANSITIONAL

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , WORD_SIZE           = BDES_WORD_SIZE
      , INT64_SIZE          = BDES_INT64_SIZE
      , BITS_PER_BYTE       = CHAR_BIT
      , BITS_PER_WORD       = BDES_BITS_PER_WORD
      , BITS_PER_INT64      = BDES_BITS_PER_INT64
#endif

    };

  private:
    // PRIVATE CLASS METHODS
    static int privateFind1AtLargestIndex(int srcInteger);
    static int privateFind1AtLargestIndex64(Int64 srcInteger);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and -1 otherwise.

    static int privateFind1AtSmallestIndex(int srcInteger);
    static int privateFind1AtSmallestIndex64(Int64 srcInteger);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.

  public:
    // CLASS METHODS

                             // Mask Generators

    static int eqMask(int index);
    static Int64 eqMask64(int index);
        // Return an integer having the bit at the specified 'index' position
        // set to 1, and all other bits set to 0.  The behavior is undefined
        // unless '0 <= index <= sizeInBits(ReturnType)', where "ReturnType"
        // is the particular C++ type of the integer value returned by this
        // function.  Note that supplying an 'index' value of
        // 'sizeInBits(ReturnType)' results in a mask having all bits set to
        // 0.

    static int geMask(int index);
    static Int64 geMask64(int index);
        // Return an integer having bits at positions greater than or equal to
        // the specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless
        // '0 <= index <= sizeInBits(ReturnType)', where "ReturnType" is the
        // particular C++ type of the integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)'
        // results in a mask having all bits set to 0.

    static int gtMask(int index);
    static Int64 gtMask64(int index);
        // Return an integer having bits at positions greater than the
        // specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless
        // '0 <= index <= sizeInBits(ReturnType)', where "ReturnType" is the
        // particular C++ type of the integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)'
        // results in a mask having all bits set to 0.

    static int leMask(int index);
    static Int64 leMask64(int index);
        // Return an integer having bits at positions less than or equal to the
        // specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless
        // '0 <= index <= sizeInBits(ReturnType)', where "ReturnType" is the
        // particular C++ type of the integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)'
        // results in a mask having all bits set to 0.

    static int ltMask(int index);
    static Int64 ltMask64(int index);
        // Return an integer having bits at positions less than the specified
        // 'index' set to 1, and all other bits set to 0.  The behavior is
        // undefined unless '0 <= index <= sizeInBits(ReturnType)', where
        // "ReturnType" is the particular C++ type of the integer value
        // returned by this function.  Note that supplying an 'index' value of
        // 'sizeInBits(ReturnType)' results in a mask having all bits set to
        // 0.

    static int oneMask(int index, int numBits);
    static Int64 oneMask64(int index, int numBits);
        // Return an integer having the specified 'numBits' starting at the
        // specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(ReturnType)', where "ReturnType" is
        // the particular C++ type of integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)',
        // and a 'numBits' value of 0, results in a mask having all bits set to
        // 0.

    static int zeroMask(int index, int numBits);
    static Int64 zeroMask64(int index, int numBits);
        // Return an integer having the specified 'numBits' starting at the
        // specified 'index' set to 0, and all other bits set to 1.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(ReturnType)', where "ReturnType" is
        // the particular C++ type of integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)',
        // and a 'numBits' value of 0, results in a mask having all bits set to
        // 1.

    static int neMask(int index);
    static Int64 neMask64(int index);
        // Return an integer having the bit at the specified 'index' position
        // set to 0, and all other bits set to 1.  The behavior is undefined
        // unless '0 <= index <= sizeInBits(ReturnType)', where "ReturnType"
        // is the particular C++ type of the integer value returned by this
        // function.  Note that supplying an 'index' value of
        // 'sizeInBits(ReturnType)' results in a mask having all bits set to
        // 1.

                             // Manipulators

    static void andEqual(int *dstInteger, int dstIndex, bool value);
    static void andEqual64(Int64 *dstInteger, int dstIndex, bool value);
        // Logically AND the specified boolean 'value' to the bit at the
        // specified 'dstIndex' position in the specified 'dstInteger'.  The
        // behavior is undefined unless
        // '0 <= dstIndex < sizeInBits(*dstInteger)'.

    static void andEqual(int *dstInteger,
                         int  dstIndex,
                         int  srcInteger,
                         int  numBits);
    static void andEqual64(Int64 *dstInteger,
                           int    dstIndex,
                           Int64  srcInteger,
                           int    numBits);
        // Bitwise AND the specified least-significant 'numBits' in the
        // specified 'srcInteger' to those in the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)'.

    static void andEqual(int *dstInteger,
                         int  dstIndex,
                         int  srcInteger,
                         int  srcIndex,
                         int  numBits);
    static void andEqual64(Int64 *dstInteger,
                           int    dstIndex,
                           Int64  srcInteger,
                           int    srcIndex,
                           int    numBits);
        // Bitwise AND the specified 'numBits' in the specified 'srcInteger'
        // starting at the specified 'srcIndex' to those in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', '0 <= numBits',
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)', and
        // 'srcIndex + numBits <= bdes_BitUtil::sizeInBits(*srcInteger)'.

    static void insertOne(  int   *dstInteger, int dstIndex, int numBits);
    static void insertOne64(Int64 *dstInteger, int dstIndex, int numBits);
        // Insert the specified 'numBits' 1 bits into the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  Bits at or above
        // 'dstIndex' are shifted up by 'numBits' index positions.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that bits
        // initially at or above position 'sizeInBits(*dstInteger) - numBits'
        // are discarded.

    static void insertZero(int *dstInteger, int dstIndex, int numBits);
    static void insertZero64(Int64 *dstInteger, int dstIndex, int numBits);
        // Insert the specified 'numBits' 0 bits into the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  Bits at or above
        // 'dstIndex' are shifted up by 'numBits' index positions.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that bits
        // initially at or above position 'sizeInBits(*dstInteger) - numBits'
        // are discarded.

    static void insertValue(int *dstInteger,
                            int  dstIndex,
                            int  srcInteger,
                            int  numBits);
    static void insertValue64(Int64 *dstInteger,
                              int    dstIndex,
                              Int64  srcInteger,
                              int    numBits);
        // Insert the specified least-significant 'numBits' of the specified
        // 'srcInteger' into the specified 'dstInteger' starting at the
        // specified 'dstIndex'.  Bits at or above 'dstIndex' are shifted up by
        // 'numBits' index positions.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that bits
        // initially at or above position 'sizeInBits(*dstInteger) - numBits'
        // are discarded.

    static void insertValueUpTo(int *dstInteger, int numBits, int srcInteger);
    static void insertValueUpTo64(Int64 *dstInteger,
                                  int    numBits,
                                  Int64  srcInteger);
        // Insert the specified least-significant 'numBits' of the specified
        // 'srcInteger' into the specified 'dstInteger' starting at index
        // position 0.  All bits in 'dstInteger' are shifted up by 'numBits'
        // index positions.  The behavior is undefined unless
        // '0 <= numBits <= sizeInBits(*dstInteger)'.  Note that bits initially
        // at or above position 'sizeInBits(*dstInteger) - numBits' are
        // discarded.

    static void minusEqual(int *dstInteger, int dstIndex, bool value);
    static void minusEqual64(Int64 *dstInteger, int dstIndex, bool value);
        // Logically MINUS the specified boolean 'value' from the bit at the
        // specified 'dstIndex' position in the specified 'dstInteger'.  The
        // behavior is undefined unless
        // '0 <= dstIndex < sizeInBits(*dstInteger)'.  Note that the bitwise
        // difference, 'a - b', is defined in C++ code as 'a & ~b'.

    static void minusEqual(int *dstInteger,
                           int  dstIndex,
                           int  srcInteger,
                           int  numBits);
    static void minusEqual64(Int64 *dstInteger,
                             int    dstIndex,
                             Int64  srcInteger,
                             int    numBits);
        // Bitwise MINUS the specified least-significant 'numBits' in the
        // specified 'srcInteger' from those in the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)'.  Note
        // that the bitwise difference, 'a - b', is defined in C++ code as
        // 'a & ~b'.

    static void minusEqual(int *dstInteger,
                           int  dstIndex,
                           int  srcInteger,
                           int  srcIndex,
                           int  numBits);
    static void minusEqual64(Int64 *dstInteger,
                             int    dstIndex,
                             Int64  srcInteger,
                             int    srcIndex,
                             int    numBits);
        // Bitwise MINUS the specified 'numBits' in the specified 'srcInteger'
        // starting at the specified 'srcIndex' to those in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', '0 <= numBits',
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)', and
        // 'srcIndex + numBits <= bdes_BitUtil::sizeInBits(*srcInteger)'.  Note
        // that the bitwise difference, 'a - b', is defined in C++ code as
        // 'a & ~b'.

    static void orEqual(int *dstInteger, int dstIndex, bool value);
    static void orEqual64(Int64 *dstInteger, int dstIndex, bool value);
        // Logically OR the specified boolean 'value' to the bit at the
        // specified 'dstIndex' position in the specified 'dstInteger'.  The
        // behavior is undefined unless
        // '0 <= dstIndex < sizeInBits(*dstInteger)'.

    static void orEqual(int *dstInteger,
                        int  dstIndex,
                        int  srcInteger,
                        int  numBits);
    static void orEqual64(Int64 *dstInteger,
                          int    dstIndex,
                          Int64  srcInteger,
                          int    numBits);
        // Bitwise OR the specified least-significant 'numBits' in the
        // specified 'srcInteger' to those in the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)'.

    static void orEqual(int *dstInteger,
                        int  dstIndex,
                        int  srcInteger,
                        int  srcIndex,
                        int  numBits);
    static void orEqual64(Int64 *dstInteger,
                          int    dstIndex,
                          Int64  srcInteger,
                          int    srcIndex,
                          int    numBits);
        // Bitwise OR the specified 'numBits' in the specified 'srcInteger'
        // starting at the specified 'srcIndex' to those in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', '0 <= numBits',
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)', and
        // 'srcIndex + numBits <= bdes_BitUtil::sizeInBits(*srcInteger)'.

    static void removeOne(int *dstInteger, int dstIndex, int numBits);
    static void removeOne64(Int64 *dstInteger, int dstIndex, int numBits);
        // Remove the specified 'numBits' from the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  Bits above 'dstIndex' are
        // shifted down by 'numBits' positions, with vacated high-order bits
        // set to 1.  The behavior is undefined unless '0 <= dstIndex',
        // '0 <= numBits', and 'dstIndex + numBits <= sizeInBits(*dstInteger)'.

    static void removeZero(int *dstInteger, int dstIndex, int numBits);
    static void removeZero64(Int64 *dstInteger, int dstIndex, int numBits);
        // Remove the specified 'numBits' from the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  Bits above 'dstIndex' are
        // shifted down by 'numBits' positions, with vacated high-order bits
        // set to 0.  The behavior is undefined unless '0 <= dstIndex',
        // '0 <= numBits', and 'dstIndex + numBits <= sizeInBits(*dstInteger)'.

    static void replaceBitOne(int *dstInteger, int dstIndex);
    static void replaceBitOne64(Int64 *dstInteger, int dstIndex);
        // Replace with 1 the bit in the specified 'dstInteger' at the
        // specified 'dstIndex' position.  The behavior is undefined unless
        // '0 <= index < sizeInBits(*dstInteger)'.

    static void replaceBitZero(int *dstInteger, int dstIndex);
    static void replaceBitZero64(Int64 *dstInteger, int dstIndex);
        // Replace with 0 the bit in the specified 'dstInteger' at the
        // specified 'dstIndex' position.  The behavior is undefined unless
        // '0 <= index < sizeInBits(*dstInteger)'.

    static void replaceBitValue(int *dstInteger,
                                int  dstIndex,
                                int  value);
    static void replaceBitValue64(Int64 *dstInteger,
                                  int    dstIndex,
                                  int    value);
        // Replace the bit in the specified 'dstInteger' at the specified
        // 'dstIndex' position with the value of the least-significant bit of
        // the specified 'value'.  The behavior is undefined unless
        // '0 <= index < sizeInBits(*dstInteger)'.

    static void replaceOne(int *dstInteger, int dstIndex, int numBits);
    static void replaceOne64(Int64 *dstInteger, int dstIndex, int numBits);
        // Replace with 1 bits the specified 'numBits' in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.

    static void replaceValue(int *dstInteger,
                             int  dstIndex,
                             int  srcInteger,
                             int  numBits);
    static void replaceValue64(Int64 *dstInteger,
                               int    dstIndex,
                               Int64  srcInteger,
                               int    numBits);
        // Replace the specified 'numBits' in the specified 'dstInteger'
        // starting at the specified 'dstIndex' with the least-significant
        // 'numBits' of the specified 'srcInteger'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.

    static void replaceValueUpTo(int *dstInteger,
                                 int  dstIndex,
                                 int  srcInteger);
    static void replaceValueUpTo64(Int64 *dstInteger,
                                   int    dstIndex,
                                   Int64  srcInteger);
        // Replace in the specified 'dstInteger' the least-significant bits up
        // to (but not including) the specified 'dstIndex' position with the
        // 'dstIndex' least-significant bits of the specified 'srcInteger'.
        // The behavior is undefined unless
        // '0 <= dstIndex <= sizeInBits(*dstInteger)'.  Note that if 'dstIndex'
        // is 0 then 'dstInteger' will be unchanged.

    static void replaceZero(int *dstInteger, int dstIndex, int numBits);
    static void replaceZero64(Int64 *dstInteger, int dstIndex, int numBits);
        // Replace with 0 bits the specified 'numBits' in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.

    static void toggle(int *dstInteger, int dstIndex, int numBits);
    static void toggle64(Int64 *dstInteger, int dstIndex, int numBits);
        // Invert the values of the specified 'numBits' in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that the
        // behavior of this function is the same as
        //..
        //  xorEqual(dstInteger, dstIndex, ~0, numBits)
        //..

    static void toggleBit(int *dstInteger, int dstIndex);
    static void toggleBit64(Int64 *dstInteger, int dstIndex);
        // Invert the value of the bit in the specified 'dstInteger' at the
        // specified 'dstIndex'.  The behavior is undefined unless
        // '0 <= dstIndex < sizeInBits(*dstInteger)'.  Note that the behavior
        // of this function is the same as
        //..
        //  xorEqual(dstInteger, dstIndex, true)
        //..

    static void xorEqual(int *dstInteger, int dstIndex, bool value);
    static void xorEqual64(Int64 *dstInteger, int dstIndex, bool value);
        // Logically XOR the specified boolean 'value' to the bit at the
        // specified 'dstIndex' position in the specified 'dstInteger'.  The
        // behavior is undefined unless
        // '0 <= dstIndex < sizeInBits(*dstInteger)'.

    static void xorEqual(int *dstInteger,
                         int  dstIndex,
                         int  srcInteger,
                         int  numBits);
    static void xorEqual64(Int64 *dstInteger,
                           int    dstIndex,
                           Int64  srcInteger,
                           int    numBits);
        // Bitwise XOR the specified least-significant 'numBits' in the
        // specified 'srcInteger' to those in the specified 'dstInteger'
        // starting at the specified 'dstIndex'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)'.

    static void xorEqual(int *dstInteger,
                         int  dstIndex,
                         int  srcInteger,
                         int  srcIndex,
                         int  numBits);
    static void xorEqual64(Int64 *dstInteger,
                           int    dstIndex,
                           Int64  srcInteger,
                           int    srcIndex,
                           int    numBits);
        // Bitwise XOR the specified 'numBits' in the specified 'srcInteger'
        // starting at the specified 'srcIndex' to those in the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', '0 <= numBits',
        // 'dstIndex + numBits <= bdes_BitUtil::sizeInBits(*dstInteger)', and
        // 'srcIndex + numBits <= bdes_BitUtil::sizeInBits(*srcInteger)'.

                             // Accessors

    static int base2Log(unsigned srcInteger);
    static int base2Log64(Uint64 srcInteger);
        // Return the base-2 logarithm of the specified 'srcInteger' rounded up
        // to the nearest integer.  The behavior is undefined unless
        // 'srcInteger > 0'.

    static int extractOne(int srcInteger, int srcIndex, int numBits);
    static Int64 extractOne64(Int64 srcInteger, int srcIndex, int numBits);
    static int extractOne(int srcInteger,
                          int srcIndex,
                          int numBits,
                          int offset);
    static Int64 extractOne64(Int64 srcInteger,
                              int   srcIndex,
                              int   numBits,
                              int   offset);
        // Return an integer value incorporating the specified 'numBits'
        // extracted from the specified 'srcInteger' starting at the specified
        // 'srcIndex', and all other bits set to 1.  Optionally specify a
        // non-negative 'offset' indicating the position of the
        // least-significant bit of the extracted bits in the result value.  If
        // 'offset' is not specified (or 0), the extracted bits will be the
        // least-significant 'numBits' in the value returned.  The behavior is
        // undefined unless '0 <= srcIndex', '0 <= numBits', '0 <= offset', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.  Note that some or
        // all of the extracted bits will be discarded unless
        // 'offset + numBits <= 'sizeInBits(srcInteger)'.

    static int extractZero(int srcInteger, int srcIndex, int numBits);
    static Int64 extractZero64(Int64 srcInteger, int srcIndex, int numBits);
    static int extractZero(int srcInteger,
                           int srcIndex,
                           int numBits,
                           int offset);
    static Int64 extractZero64(Int64 srcInteger,
                               int   srcIndex,
                               int   numBits,
                               int   offset);
        // Return an integer value incorporating the specified 'numBits'
        // extracted from the specified 'srcInteger' starting at the specified
        // 'srcIndex', and all other bits set to 0.  Optionally specify a
        // non-negative 'offset' indicating the position of the
        // least-significant bit of the extracted bits in the result value.  If
        // 'offset' is not specified (or 0), the extracted bits will be the
        // least-significant 'numBits' in the value returned.  The behavior is
        // undefined unless '0 <= srcIndex', '0 <= numBits', '0 <= offset', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.  Note that some or
        // all of the extracted bits will be discarded unless
        // 'offset + numBits <= 'sizeInBits(srcInteger)'.

    static int find0AtLargestIndex(int srcInteger);
    static int find0AtLargestIndex64(Int64 srcInteger);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger', if such a bit exists, and -1 otherwise.

    static int find0AtLargestIndexGE(int srcInteger, int srcIndex);
    static int find0AtLargestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and -1 otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value of 'sizeInBits(srcInteger)' always returns -1.

    static int find0AtLargestIndexGT(int srcInteger, int srcIndex);
    static int find0AtLargestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note
        // that supplying a 'srcIndex' value that is greater than or equal to
        // 'sizeInBits(srcInteger) - 1' always returns -1.

    static int find0AtLargestIndexLE(int srcInteger, int srcIndex);
    static int find0AtLargestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find0AtLargestIndexLT(int srcInteger, int srcIndex);
    static int find0AtLargestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and -1 otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find0AtSmallestIndex(int srcInteger);
    static int find0AtSmallestIndex64(Int64 srcInteger);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.

    static int find0AtSmallestIndexGE(int srcInteger, int srcIndex);
    static int find0AtSmallestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and
        // 'sizeInBits(srcInteger)' otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that
        // supplying a 'srcIndex' value of 'sizeInBits(srcInteger)' always
        // returns 'sizeInBits(srcInteger)'.

    static int find0AtSmallestIndexGT(int srcInteger, int srcIndex);
    static int find0AtSmallestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value that is greater than or equal to
        // 'sizeInBits(srcInteger) - 1' always returns
        // 'sizeInBits(srcInteger)'.

    static int find0AtSmallestIndexLE(int srcInteger, int srcIndex);
    static int find0AtSmallestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find0AtSmallestIndexLT(int srcInteger, int srcIndex);
    static int find0AtSmallestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and 'sizeInBits(srcInteger)' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find1AtLargestIndex(int srcInteger);
    static int find1AtLargestIndex64(Int64 srcInteger);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and -1 otherwise.

    static int find1AtLargestIndexGE(int srcInteger, int srcIndex);
    static int find1AtLargestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and -1 otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' of 'sizeInBits(srcInteger)' always returns -1.

    static int find1AtLargestIndexGT(int srcInteger, int srcIndex);
    static int find1AtLargestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note
        // that supplying a 'srcIndex' value that is greater than or equal to
        // 'sizeInBits(srcInteger) - 1' always returns -1.

    static int find1AtLargestIndexLE(int srcInteger, int srcIndex);
    static int find1AtLargestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find1AtLargestIndexLT(int srcInteger, int srcIndex);
    static int find1AtLargestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and -1 otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find1AtSmallestIndex(int srcInteger);
    static int find1AtSmallestIndex64(Int64 srcInteger);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.

    static int find1AtSmallestIndexGE(int srcInteger, int srcIndex);
    static int find1AtSmallestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and
        // 'sizeInBits(srcInteger)' otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that
        // supplying a 'srcIndex' value of 'sizeInBits(srcInteger)' always
        // returns 'sizeInBits(srcInteger)'.

    static int find1AtSmallestIndexGT(int srcInteger, int srcIndex);
    static int find1AtSmallestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value that is greater than or equal to
        // 'sizeInBits(srcInteger) - 1' always returns
        // 'sizeInBits(srcInteger)'.

    static int find1AtSmallestIndexLE(int srcInteger, int srcIndex);
    static int find1AtSmallestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int find1AtSmallestIndexLT(int srcInteger, int srcIndex);
    static int find1AtSmallestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and 'sizeInBits(srcInteger)' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.

    static int   intAbs(  int   srcInteger);
    static Int64 intAbs64(Int64 srcInteger);
        // Return the absolute value of the specified 'srcInteger' unless,
        // 'srcInteger' represents the most negative value possible, in which
        // case return that (negative) value.  Note that this function is
        // implemented without using any branches and should therefore be
        // faster than 'bsl::abs'.

    static int   intMax(  int   srcIntegerX, int   srcIntegerY);
    static Int64 intMax64(Int64 srcIntegerX, Int64 srcIntegerY);
        // Return the maximum of the specified 'srcIntegerX' and 'srcIntegerY'.
        // Note that this function is implemented without using any branches
        // and should therefore be faster than 'bsl::max'.

    static int   intMin(  int   srcIntegerX, int   srcIntegerY);
    static Int64 intMin64(Int64 srcIntegerX, Int64 srcIntegerY);
        // Return the minimum of the specified 'srcIntegerX' and 'srcIntegerY'.
        // Note that this function is implemented without using any branches
        // and should therefore be faster than 'bsl::min'.

    static bool isAnySetOne(int srcInteger);
    static bool isAnySetOne64(Int64 srcInteger);
        // Return 'true' if any bit in the specified 'srcInteger' is set to 1,
        // and 'false' otherwise.

    static bool isAnySetZero(int srcInteger);
    static bool isAnySetZero64(Int64 srcInteger);
        // Return 'true' if any bit in the specified 'srcInteger' is set to 0,
        // and 'false' otherwise.

    static bool isSetOne(int srcInteger, int srcIndex);
    static bool isSetOne64(Int64 srcInteger, int srcIndex);
        // Return 'true' if the bit in the specified 'srcInteger' at the
        // specified 'srcIndex' is set to 1, and 'false' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.

    static bool isSetZero(int srcInteger, int srcIndex);
    static bool isSetZero64(Int64 srcInteger, int srcIndex);
        // Return 'true' if the bit in the specified 'srcInteger' at the
        // specified 'srcIndex' is set to 0, and 'false' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.

    static int merge(int zeroSrcInteger, int oneSrcInteger, int mask);
    static Int64 merge64(Int64 zeroSrcInteger,
                         Int64 oneSrcInteger,
                         Int64 mask);
        // Return the bitwise merge of the specified 'zeroSrcInteger' and the
        // specified 'oneSrcInteger', where the result bit at each bit position
        // is determined by the corresponding bit value in the specified
        // 'mask': A 0 bit in 'mask' selects the corresponding bit value from
        // 'zeroSrcInteger', whereas a 1 bit in 'mask' selects it from
        // 'oneSrcInteger'.

    static int numSetOne(int srcInteger);
    static int numSetOne64(Int64 srcInteger);
        // Return the number of 1 bits in the specified 'srcInteger'.

    static int numSetZero(int srcInteger);
    static int numSetZero64(Int64 srcInteger);
        // Return the number of 0 bits in the specified 'srcInteger'.

    static int rangeOne(int srcInteger, int srcIndex, int numBits);
    static Int64 rangeOne64(Int64 srcInteger, int srcIndex, int numBits);
        // Return an integer preserving the specified 'numBits' from the
        // specified 'srcInteger' starting at the specified 'srcIndex', but
        // with all other bits set to 1.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.

    static int rangeZero(int srcInteger, int srcIndex, int numBits);
    static Int64 rangeZero64(Int64 srcInteger, int srcIndex, int numBits);
        // Return an integer preserving the specified 'numBits' from the
        // specified 'srcInteger' starting at the specified 'srcIndex', but
        // with all other bits set to 0.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.

    static unsigned roundUpToBinaryPower(unsigned srcInteger);
    static Uint64 roundUpToBinaryPower64(Uint64 srcInteger);
        // Return the least power of 2 that is greater than or equal to the
        // specified 'srcInteger'.  Return 0 if '0 == srcInteger' or
        // 'srcInteger > 2 ^ (sizeInBits(srcInteger) - 1)'.

    static int setBitOne(int srcInteger, int srcIndex);
    static Int64 setBitOne64(Int64 srcInteger, int srcIndex);
        // Return the value of the specified 'srcInteger', but with the bit at
        // the specified 'srcIndex' position set to 1.  The behavior is
        // undefined unless '0 <= srcIndex < sizeInBits(srcInteger)'.

    static int setBitZero(int srcInteger, int srcIndex);
    static Int64 setBitZero64(Int64 srcInteger, int srcIndex);
        // Return the value of the specified 'srcInteger', but with the bit at
        // the specified 'srcIndex' position set to 0.  The behavior is
        // undefined unless '0 <= srcIndex < sizeInBits(srcInteger)'.

    static int setBitValue(int srcInteger, int srcIndex, int value);
    static Int64 setBitValue64(Int64 srcInteger, int srcIndex, int value);
        // Return the value of the specified 'srcInteger', but with the bit at
        // the specified 'srcIndex' position set to the least-significant bit
        // of the specified 'value'.  The behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.

    static int setOne(int srcInteger, int srcIndex, int numBits);
    static Int64 setOne64(Int64 srcInteger, int srcIndex, int numBits);
        // Return the value of the specified 'srcInteger', but with each of the
        // specified 'numBits' starting at the specified 'srcIndex' set to 1.
        // The behavior is undefined unless '0 <= srcIndex', '0 <= numBits',
        // and 'srcIndex + numBits <= sizeInBits(srcInteger)'.

    static int setValue(int srcInteger,
                        int srcIndex,
                        int value,
                        int numBits);
    static Int64 setValue64(Int64 srcInteger,
                            int   srcIndex,
                            Int64 value,
                            int   numBits);
        // Return the value of the specified 'srcInteger', but with the
        // specified 'numBits' starting at the specified 'srcIndex' set to the
        // least-significant 'numBits' of the specified 'value'.  The behavior
        // is undefined unless '0 <= srcIndex', '0 <= numBits', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.

    static int setValueUpTo(int srcInteger, int srcIndex, int value);
    static Int64 setValueUpTo64(Int64 srcInteger, int srcIndex, Int64 value);
        // Return the value of the specified 'srcInteger' with bits at
        // positions up to (but not including) the specified 'srcIndex' set to
        // the 'srcIndex' least-significant bits of the specified 'value'.  The
        // behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value of 0 always returns 'srcInteger'.

    static int setZero(int srcInteger, int srcIndex, int numBits);
    static Int64 setZero64(Int64 srcInteger, int srcIndex, int numBits);
        // Return the value of the specified 'srcInteger', but with each of the
        // specified 'numBits' starting at the specified 'srcIndex' set to 0.
        // The behavior is undefined unless '0 <= srcIndex', '0 <= numBits',
        // and 'srcIndex + numBits <= sizeInBits(srcInteger)'.

    template <typename TYPE>
    static int sizeInBits(TYPE value);
        // Return the number of bits in the specified 'value' of parameterized
        // 'TYPE'.  Note that this method is used extensively throughout this
        // component to document the valid range of input for which functional
        // behavior is defined.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int findBit0AtLargestIndex(int srcInteger);
    static int findBit0AtLargestIndex64(Int64 srcInteger);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger', if such a bit exists, and -1 otherwise.
        //
        // DEPRECATED: Use 'find0AtLargestIndex' and 'find0AtLargestIndex64'
        //             instead.

    static int findBit0AtLargestIndexGE(int srcInteger, int srcIndex);
    static int findBit0AtLargestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and -1 otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value that is equal to 'sizeInBits(srcInteger)' always
        // results in a value of -1.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGE' and
        //             'find0AtLargestIndexGE64' instead.

    static int findBit0AtLargestIndexGT(int srcInteger, int srcIndex);
    static int findBit0AtLargestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note
        // that supplying a 'srcIndex' value that is greater than
        // 'sizeInBits(srcInteger) - 2' always results in a value of -1.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGT' and
        //             'find0AtLargestIndexGT64' instead.

    static int findBit0AtLargestIndexLE(int srcInteger, int srcIndex);
    static int findBit0AtLargestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLE' and
        //             'find0AtLargestIndexLE64' instead.

    static int findBit0AtLargestIndexLT(int srcInteger, int srcIndex);
    static int findBit0AtLargestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 0 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and -1 otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLT' and
        //             'find0AtLargestIndexLT64' instead.

    static int findBit0AtSmallestIndex(int srcInteger);
    static int findBit0AtSmallestIndex64(Int64 srcInteger);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.
        //
        // DEPRECATED: Use 'find0AtSmallestIndex' and
        //             'find0AtSmallestIndex64' instead.

    static int findBit0AtSmallestIndexGE(int srcInteger, int srcIndex);
    static int findBit0AtSmallestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and
        // 'sizeInBits(srcInteger)' otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that
        // supplying a 'srcIndex' value that is equal to
        // 'sizeInBits(srcInteger)' always results in a value of
        // 'sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGE' and
        //             'find0AtSmallestIndexGE64' instead.

    static int findBit0AtSmallestIndexGT(int srcInteger, int srcIndex);
    static int findBit0AtSmallestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value that is greater than 'sizeInBits(srcInteger) - 2'
        // always results in a value of 'sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGT' and
        //             'find0AtSmallestIndexGT64' instead.

    static int findBit0AtSmallestIndexLE(int srcInteger, int srcIndex);
    static int findBit0AtSmallestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLE' and
        //             'find0AtSmallestIndexLE64' instead.

    static int findBit0AtSmallestIndexLT(int srcInteger, int srcIndex);
    static int findBit0AtSmallestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 0 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and 'sizeInBits(srcInteger)' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLT' and
        //             'find0AtSmallestIndexLT64' instead.

    static int findBit1AtLargestIndex(int srcInteger);
    static int findBit1AtLargestIndex64(Int64 srcInteger);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and -1 otherwise.
        //
        // DEPRECATED: Use 'find1AtLargestIndex' and 'find1AtLargestIndex64'
        //             instead.

    static int findBit1AtLargestIndexGE(int srcInteger, int srcIndex);
    static int findBit1AtLargestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and -1 otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' that is equal to 'sizeInBits(srcInteger)' always results
        // in -1.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGE' and
        //             'find1AtLargestIndexGE64' instead.

    static int findBit1AtLargestIndexGT(int srcInteger, int srcIndex);
    static int findBit1AtLargestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note
        // that supplying a 'srcIndex' value that is greater than
        // 'sizeInBits(srcInteger) - 2' always results in a value of -1.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGT' and
        //             'find1AtLargestIndexGT64' instead.

    static int findBit1AtLargestIndexLE(int srcInteger, int srcIndex);
    static int findBit1AtLargestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and -1 otherwise.  The behavior is
        // undefined unless '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLE' and
        //             'find1AtLargestIndexLE64' instead.

    static int findBit1AtLargestIndexLT(int srcInteger, int srcIndex);
    static int findBit1AtLargestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the most-significant 1 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and -1 otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLT' and
        //             'find1AtLargestIndexLT64' instead.

    static int findBit1AtSmallestIndex(int srcInteger);
    static int findBit1AtSmallestIndex64(Int64 srcInteger);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.
        //
        // DEPRECATED: Use 'find1AtSmallestIndex' and
        //             'find1AtSmallestIndex64' instead.

    static int findBit1AtSmallestIndexGE(int srcInteger, int srcIndex);
    static int findBit1AtSmallestIndexGE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than or equal to the
        // specified 'srcIndex', if such a bit exists, and
        // 'sizeInBits(srcInteger)' otherwise.  The behavior is undefined
        // unless '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that
        // supplying a 'srcIndex' value of 'sizeInBits(srcInteger)' always
        // results in a value of 'sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGE' and
        //             'find1AtSmallestIndexGE64' instead.

    static int findBit1AtSmallestIndexGT(int srcInteger, int srcIndex);
    static int findBit1AtSmallestIndexGT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is greater than the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.  Note that supplying a
        // 'srcIndex' value that is greater than 'sizeInBits(srcInteger) - 2'
        // always results in a value of 'sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGT' and
        //             'find1AtSmallestIndexGT64' instead.

    static int findBit1AtSmallestIndexLE(int srcInteger, int srcIndex);
    static int findBit1AtSmallestIndexLE64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is less than or equal to the specified
        // 'srcIndex', if such a bit exists, and 'sizeInBits(srcInteger)'
        // otherwise.  The behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLE' and
        //             'find1AtSmallestIndexLE64' instead.

    static int findBit1AtSmallestIndexLT(int srcInteger, int srcIndex);
    static int findBit1AtSmallestIndexLT64(Int64 srcInteger, int srcIndex);
        // Return the index of the least-significant 1 bit in the specified
        // 'srcInteger' whose position is less than the specified 'srcIndex',
        // if such a bit exists, and 'sizeInBits(srcInteger)' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLT' and
        //             'find1AtSmallestIndexLT64' instead.

    static int   abs32(int   srcInteger);
    static Int64 abs64(Int64 srcInteger);
        // Return the absolute value of the specified 'srcInteger' unless,
        // 'srcInteger' represents the most negative value possible, in which
        // case return that (negative) value.  Note that this function is
        // implemented without using any branches and should therefore be
        // faster than 'bsl::abs'.
        //
        // DEPRECATED: Use 'intAbs' and 'intAbs64' instead.

    static int   max32(int   srcIntegerX, int   srcIntegerY);
    static Int64 max64(Int64 srcIntegerX, Int64 srcIntegerY);
        // Return the maximum of the specified 'srcIntegerX' and 'srcIntegerY'.
        // Note that this function is implemented without using any branches
        // and should therefore be faster than 'bsl::max'.
        //
        // DEPRECATED: Use 'intMax' and 'intMax64' instead.

    static int   min32(int   srcIntegerX, int   srcIntegerY);
    static Int64 min64(Int64 srcIntegerX, Int64 srcIntegerY);
        // Return the minimum of the specified 'srcIntegerX' and 'srcIntegerY'.
        // Note that this function is implemented without using any branches
        // and should therefore be faster than 'bsl::min'.
        //
        // DEPRECATED: Use 'intMin' and 'intMin64' instead.

    static int numSet0(int srcInteger);
        // Return the number of 0 bits in the specified 'srcInteger'.
        //
        // DEPRECATED: Use 'numSetZero' instead.

    static int numSet1(int srcInteger);
        // Return the number of 1 bits in the specified 'srcInteger'.
        //
        // DEPRECATED: Use 'numSetOne' instead.

    static bool isSet0(int srcInteger, int srcIndex);
        // Return 'true' if the bit at the specified 'srcIndex' in the
        // specified 'srcInteger' is set to 0, and 'false' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'isSetZero' instead.

    static bool isSet1(int srcInteger, int srcIndex);
        // Return 'true' if the bit at the specified 'srcIndex' in the
        // specified 'srcInteger' is set to 1, and 'false' otherwise.  The
        // behavior is undefined unless
        // '0 <= srcIndex < sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'isSetOne' instead.

    static int setBit0(int srcInteger, int srcIndex);
        // Return an integer equal to the specified 'srcInteger' with the bit
        // at the specified 'srcIndex' set to 0.  The behavior is undefined
        // unless '0 <= srcIndex < sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'setBitZero' instead.

    static int setBit1(int srcInteger, int srcIndex);
        // Return an integer equal to the specified 'srcInteger' with the bit
        // at the specified 'srcIndex' set to 1.  The behavior is undefined
        // unless '0 <= srcIndex < sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'setBitOne' instead.

    static int extract0(int srcInteger, int srcIndex, int numBits);
    static int extract0(int srcInteger, int srcIndex, int numBits, int offset);
        // Return an integer value having the specified 'numBits' extracted
        // from the specified 'srcInteger', starting at the specified
        // 'srcIndex', with the copied bits shifted to index position 0, and
        // all other bits set to 0.  Optionally specify a non-negative offset
        // from the start of the resulting integer to (left) shift the copied
        // bits before returning.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', '0 <= offset', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.  Note that some or
        // all of the extracted bits will be lost if
        // 'sizeInBits(srcInteger) < offset + numBits'.
        //
        // DEPRECATED: Use 'extractZero' instead.

    static int extract1(int srcInteger, int srcIndex, int numBits);
    static int extract1(int srcInteger, int srcIndex, int numBits, int offset);
        // Return an integer value having the specified 'numBits' extracted
        // from the specified 'srcInteger', starting at the specified
        // 'srcIndex', with the copied bits shifted to index position 0, and
        // all other bits set to 1.  Optionally specify a non-negative offset
        // from the start of the resulting integer to (left) shift the copied
        // bits before returning.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', '0 <= offset', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.  Note that some or
        // all of the extracted bits will be lost if
        // 'sizeInBits(srcInteger) < offset + numBits'.
        //
        // DEPRECATED: Use 'extractOne' instead.

    static void replaceBit0(int *dstInteger, int dstIndex);
        // Replace the bit at the specified 'dstIndex' in the specified
        // 'dstInteger' with 0.  The behavior is undefined unless
        // '0 <= index < sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'replaceBitZero' instead.

    static void replaceBit1(int *dstInteger, int dstIndex);
        // Replace the bit at the specified 'dstIndex' in the specified
        // 'dstInteger' with 1.  The behavior is undefined unless
        // '0 <= index < sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'replaceBitOne' instead.

    static void remove0(int *dstInteger, int dstIndex, int numBits);
        // Remove from the specified 'dstInteger' the specified 'numBits'
        // positions starting at the specified 'dstIndex' with high-order bits
        // set to 0.  Bits above the removed region are shifted down 'numBits'
        // bit positions.  The behavior is undefined unless '0 <= dstIndex',
        // '0 <= numBits', and 'dstIndex + numBits <= sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'removeZero' instead.

    static void remove1(int *dstInteger, int dstIndex, int numBits);
        // Remove from the specified 'dstInteger' the specified 'numBits'
        // positions starting at the specified 'dstIndex' with high-order bits
        // set to 1.  Bits above the removed region are shifted down 'numBits'
        // bit positions.  The behavior is undefined unless '0 <= dstIndex',
        // '0 <= numBits', and 'dstIndex + numBits <= sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'removeOne' instead.

    static void insert0(int *dstInteger, int dstIndex, int numBits);
        // Insert the specified 'numBits' 0 bits into the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  Bits at or above
        // 'dstIndex' are shifted up by 'numBits' index positions.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that bits
        // shifted to index positions at or above 'sizeInBits(*dstInteger)' are
        // lost.
        //
        // DEPRECATED: Use 'insertZero' instead.

    static void insert1(int *dstInteger, int dstIndex, int numBits);
        // Insert the specified 'numBits' 1 bits into the specified
        // 'dstInteger' starting at the specified 'dstIndex'.  Bits at or above
        // 'dstIndex' are shifted up by 'numBits' index positions.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.  Note that bits
        // shifted to index positions at or above 'sizeInBits(*dstInteger)' are
        // lost.
        //
        // DEPRECATED: Use 'insertOne' instead.

    static int set0(int srcInteger, int srcIndex, int numBits);
        // Return an integer equal to the specified 'srcInteger' with each of
        // the specified 'numBits' starting at the specified 'srcIndex' set 0.
        // The behavior is undefined unless '0 <= srcIndex', '0 <= numBits',
        // and 'srcIndex + numBits <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'setZero' instead.

    static int set1(int srcInteger, int srcIndex, int numBits);
        // Return an integer equal to the specified 'srcInteger' with each of
        // the specified 'numBits' starting at the specified 'srcIndex' set to
        // 1.  The behavior is undefined unless '0 <= srcIndex',
        // '0 <= numBits', and 'srcIndex + numBits <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'setOne' instead.

    static void replace0(int *dstInteger, int dstIndex, int numBits);
        // Replace the 'numBits' starting at the specified 'dstIndex' in the
        // specified 'dstInteger' with 0 bits.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'replaceZero' instead.

    static void replace1(int *dstInteger, int dstIndex, int numBits);
        // Replace the 'numBits' starting at the specified 'dstIndex' in the
        // specified 'dstInteger' with 1 bits.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= sizeInBits(*dstInteger)'.
        //
        // DEPRECATED: Use 'replaceOne' instead.

    static bool isAnySet0(int srcInteger);
        // Return 'true' if any bit in the specified 'srcInteger' is set to 0,
        // and 'false' otherwise.
        //
        // DEPRECATED: Use 'isAnySetZero' instead.

    static bool isAnySet1(int srcInteger);
        // Return 'true' if any bit in the specified 'srcInteger' is set to 1,
        // and 'false' otherwise.
        //
        // DEPRECATED: Use 'isAnySetOne' instead.

    static int range0(int srcInteger, int srcIndex, int numBits);
        // Return an integer containing the specified 'numBits' from the
        // specified 'srcInteger' starting at the specified 'srcIndex' with all
        // other bits set to 0.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'rangeZero' instead.

    static int range1(int srcInteger, int srcIndex, int numBits);
        // Return an integer containing the specified 'numBits' from the
        // specified 'srcInteger' starting at the specified 'srcIndex' with all
        // other bits set to 1.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numBits', and
        // 'srcIndex + numBits <= sizeInBits(srcInteger)'.
        //
        // DEPRECATED: Use 'rangeOne' instead.

    static int maskOne(int index, int numBits);
    static Int64 maskOne64(int index, int numBits);
        // Return an integer having the specified 'numBits' starting at the
        // specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(ReturnType)', where "ReturnType" is
        // the particular C++ type of integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)',
        // and a 'numBits' value of 0, results in a mask having all bits set to
        // 0.
        //
        // DEPRECATED: Use 'oneMask' and 'oneMask64' instead.

    static int maskZero(int index, int numBits);
    static Int64 maskZero64(int index, int numBits);
        // Return an integer having the specified 'numBits' starting at the
        // specified 'index' set to 0, and all other bits set to 1.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(ReturnType)', where "ReturnType" is
        // the particular C++ type of integer value returned by this function.
        // Note that supplying an 'index' value of 'sizeInBits(ReturnType)',
        // and a 'numBits' value of 0, results in a mask having all bits set to
        // 1.
        //
        // DEPRECATED: Use 'zeroMask' and 'zeroMask64' instead.

    static int mask0(int index, int numBits);
        // Return an integer with the specified 'numBits' starting at the
        // specified 'index' set to 0, and all other bits set to 1.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(result)' where "result" is the return
        // type of this function.
        //
        // DEPRECATED: Use 'zeroMask' instead.

    static int mask1(int index, int numBits);
        // Return an integer with the specified 'numBits' starting at the
        // specified 'index' set to 1, and all other bits set to 0.  The
        // behavior is undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= sizeInBits(result)' where "result" is the return
        // type of this function.
        //
        // DEPRECATED: Use 'oneMask' instead.

    template <typename TYPE>
    static int numBits(TYPE value);
        // Return the number of bits in the specified 'value' of parameterized
        // 'TYPE'.  Note that this method is used extensively in this
        //
        // DEPRECATED: Use 'sizeInBits' instead.

#endif
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                               // -------------------
                               // struct bdes_BitUtil
                               // -------------------

                               // -----------------
                               // Level-0 Functions
                               // -----------------

// CLASS METHODS
inline
int bdes_BitUtil::eqMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(BDES_BITS_PER_INT))
           ? 1 << index
           : 0;
}

inline
bsls_Types::Int64 bdes_BitUtil::eqMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 index < static_cast<int>(BDES_BITS_PER_INT64))
           ? 1LL << index
           : 0;
}

inline
int bdes_BitUtil::geMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(BDES_BITS_PER_INT))
           ? ~((1 << index) - 1)
           : 0;
}

inline
bsls_Types::Int64 bdes_BitUtil::geMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 index < static_cast<int>(BDES_BITS_PER_INT64))
           ? ~((1LL << index) - 1)
           : 0;
}

inline
int bdes_BitUtil::gtMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(BDES_BITS_PER_INT))
           ? ~((1 << index) - 1)
           : 0;
}

inline
bsls_Types::Int64 bdes_BitUtil::gtMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 index < static_cast<int>(BDES_BITS_PER_INT64))
           ? ~((1LL << index) - 1)
           : 0;
}

inline
int bdes_BitUtil::leMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(BDES_BITS_PER_INT))
           ? (1 << index) - 1
           : -1;
}

inline
bsls_Types::Int64 bdes_BitUtil::leMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 index < static_cast<int>(BDES_BITS_PER_INT64))
           ? (1LL << index) - 1
           : -1;
}

inline
int bdes_BitUtil::ltMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index == static_cast<int>(BDES_BITS_PER_INT))
           ? -1
           : (1 << index) - 1;
}

inline
bsls_Types::Int64 bdes_BitUtil::ltMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                index == static_cast<int>(BDES_BITS_PER_INT64))
           ? -1
           : (1LL << index) - 1;
}

inline
int bdes_BitUtil::neMask(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index == static_cast<int>(BDES_BITS_PER_INT))
           ? -1
           : ~(1 << index);
}

inline
bsls_Types::Int64 bdes_BitUtil::neMask64(int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <= static_cast<int>(BDES_BITS_PER_INT64));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                index == static_cast<int>(BDES_BITS_PER_INT64))
           ? -1
           : ~(1LL << index);
}

inline
int bdes_BitUtil::find1AtLargestIndex(int srcInteger)
{
#if defined(BSLS_PLATFORM__CMP_IBM)
    return 31 - __cntlz4(srcInteger);
#elif defined(BSLS_PLATFORM__CMP_GNU)
    return (31 - __builtin_clz(srcInteger)) | -!srcInteger;
#else
    return privateFind1AtLargestIndex(srcInteger);
#endif
}

inline
int bdes_BitUtil::find1AtLargestIndex64(Int64 srcInteger)
{
#if defined(BSLS_PLATFORM__CMP_IBM)
    return 63 - __cntlz8(srcInteger);
#elif defined(BSLS_PLATFORM__CMP_GNU)
    return (63 - __builtin_clzll(srcInteger))| -static_cast<int>(!srcInteger);
#else
    return privateFind1AtLargestIndex64(srcInteger);
#endif
}

inline
int bdes_BitUtil::find1AtSmallestIndex(int srcInteger)
{
#if defined(BSLS_PLATFORM__CMP_IBM)
    return __cnttz4(static_cast<unsigned>(srcInteger));
#elif defined(BSLS_PLATFORM__CMP_GNU)
    enum {
        BDES_INT_MASK = BDES_BITS_PER_INT - 1
    };
    const unsigned a = __builtin_ffs(static_cast<unsigned>(srcInteger)) - 1;
    return (a & BDES_INT_MASK) + (a >> BDES_INT_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffs(static_cast<unsigned>(srcInteger)) - 1)
    //       ^ ((-!srcInteger) & ~BDES_BITS_PER_INT);
    //..
#else
    return privateFind1AtSmallestIndex(srcInteger);
#endif
}

inline
int bdes_BitUtil::find1AtSmallestIndex64(Int64 srcInteger)
{
#if defined(BSLS_PLATFORM__CMP_IBM)
    return __cnttz8(static_cast<Uint64>(srcInteger));
#elif defined(BSLS_PLATFORM__CMP_GNU)
    enum {
        BDES_INT64_MASK = BDES_BITS_PER_INT64 - 1
      , BDES_INT_MASK   = BDES_BITS_PER_INT  - 1
    };
    const unsigned a = __builtin_ffsll(static_cast<Uint64>(srcInteger)) - 1;
    return (a & BDES_INT64_MASK) + (a >> BDES_INT_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffsll(static_cast<Uint64>(srcInteger)) - 1)
    //       ^ ((-!srcInteger) & ~BDES_BITS_PER_INT64);
    //..
#else
    return privateFind1AtSmallestIndex64(srcInteger);
#endif
}

inline
int bdes_BitUtil::numSetOne(int srcInteger)
{
    unsigned input = srcInteger;

#if defined(BSLS_PLATFORM__CMP_IBM)
    return __popcnt4(input);
#elif defined(BSLS_PLATFORM__CMP_GNU)
    return __builtin_popcount(input);
#else

    // First we use a tricky way of getting every 2-bit half-nibble to
    // represent the number of bits that were set in those two bits.

    input -= (input >> 1) & 0x55555555;

    // Henceforth, we just accumulate the sum down into lower and lower bits.

    {
        const int mask = 0x33333333;
        input = ((input >> 2) & mask) + (input & mask);
    }

    // Any 4-bit nibble is now guaranteed to be '<= 4', so we don't have to
    // mask both sides of the addition.  We must mask after the addition so
    // 8-bit bytes are the sum of bits in those 8 bits.

    input = ((input >> 4) + input) & 0x0f0f0f0f;

    // It is no longer necessary to mask the additions, because it is
    // impossible for any bit groups to add up to more than 256 and carry, thus
    // interfering with adjacent groups.  Each 8-bit byte is independent from
    // now on.

    input = (input >>  8) + input;
    input = (input >> 16) + input;

    return input & 0x000000ff;
#endif
}

inline
int bdes_BitUtil::numSetOne64(Int64 srcInteger)
{
    Uint64 input = srcInteger;

#if defined(BSLS_PLATFORM__CMP_IBM)
    return __popcnt8(input);
#elif defined(BSLS_PLATFORM__CMP_GNU)
    return __builtin_popcountll(input);
#else

    // First we use a tricky way of getting every 2-bit half-nibble to
    // represent the number of bits that were set in those two bits.

    input -= (input >> 1) & 0x5555555555555555LL;

    // Henceforth, we just accumulate the sum down into lower and lower bits.

    {
        const Int64 mask = 0x3333333333333333LL;
        input = ((input >> 2) & mask) + (input & mask);
    }

    // Any 4-bit nibble is now guaranteed to be '<= 4', so we don't have to
    // mask both sides of the addition.  We must mask after the addition so
    // 8-bit bytes are the sum of bits in those 8 bits.

    input = ((input >> 4) + input) & 0x0f0f0f0f0f0f0f0fLL;

    // It is no longer necessary to mask the additions, because it is
    // impossible for any bit groups to add up to more than 256 and carry, thus
    // interfering with adjacent groups.  Each 8-bit byte is independent from
    // now on.

    input = (input >>  8) + input;
    input = (input >> 16) + input;
    input = (input >> 32) + input;

    return static_cast<int>(input & 0xff);
#endif
}

inline
int bdes_BitUtil::oneMask(int index, int numBits)
{
    BSLS_ASSERT_SAFE(              0 <= index);
    BSLS_ASSERT_SAFE(              0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= BDES_BITS_PER_INT);

    return ltMask(index + numBits) & geMask(index);
}

inline
bsls_Types::Int64 bdes_BitUtil::oneMask64(int index, int numBits)
{
    BSLS_ASSERT_SAFE(              0 <= index);
    BSLS_ASSERT_SAFE(              0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= BDES_BITS_PER_INT64);

    return ltMask64(index + numBits) & geMask64(index);
}

inline
int bdes_BitUtil::zeroMask(int index, int numBits)
{
    BSLS_ASSERT_SAFE(              0 <= index);
    BSLS_ASSERT_SAFE(              0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= BDES_BITS_PER_INT);

    return ltMask(index) | geMask(index + numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::zeroMask64(int index, int numBits)
{
    BSLS_ASSERT_SAFE(              0 <= index);
    BSLS_ASSERT_SAFE(              0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= BDES_BITS_PER_INT64);

    return ltMask64(index) | geMask64(index + numBits);
}

inline
int bdes_BitUtil::find0AtLargestIndex(int srcInteger)
{
    // Note that both AIX xlC and GNU GCC compilers have no built-in routines
    // for counting leading '1's the way '__cntlz4' and '__builtin_clz' count
    // leading 0's, so the fastest way to do this is to just complement
    // 'srcInteger' and call 'find1AtLargestIndex64'.

    return find1AtLargestIndex(~srcInteger);
}

inline
int bdes_BitUtil::find0AtLargestIndex64(Int64 srcInteger)
{
    // Note that both AIX xlC and GNU GCC compilers have no built-in routines
    // for counting leading '1's the way '__cntlz8' and '__builtin_clzll' count
    // leading 0's, so the fastest way to do this is to just complement
    // 'srcInteger' and call 'find1AtLargestIndex64'.

    return find1AtLargestIndex64(~srcInteger);
}

inline
int bdes_BitUtil::find0AtSmallestIndex(int srcInteger)
{
    // Note that AIX xlC compiler has no built-in primitive that counts
    // trailing '1's the way '__cnttz4' counts trailing 0's.  GNU GCC compiler
    // has no built-in primitive to look for the lowest-order '0' bit the way
    // that '__builtin_ffs' finds the lowest-order '1' bit.  So the fastest way
    // to do this is to just complement 'srcInteger' and call
    // 'find1AtSmallestIndex'.

    return find1AtSmallestIndex(~srcInteger);
}

inline
int bdes_BitUtil::find0AtSmallestIndex64(Int64 srcInteger)
{
    // Note that AIX xlC compiler has no built-in primitive that counts
    // trailing '1's the way '__cnttz8' counts trailing 0's.  GNU GCC compiler
    // has no built-in primitive to look for the lowest-order '0' bit the way
    // that '__builtin_ffsll' finds the lowest-order '1' bit.  So the fastest
    // way to do this is to just complement 'srcInteger' and call
    // 'find1AtSmallestIndex'.

    return find1AtSmallestIndex64(~srcInteger);
}

inline
int bdes_BitUtil::rangeOne(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    // Note that the following is equivalent to:
    //..
    //  srcInteger | zeroMask(srcIndex, numBits)
    //..
    // but may be easier to inline for some compilers.

    return srcInteger | ltMask(srcIndex) | geMask(srcIndex + numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::rangeOne64(Int64 srcInteger,
                                           int   srcIndex,
                                           int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    // Note that the following is equivalent to:
    //..
    //  srcInteger | zeroMask64(srcIndex, numBits)
    //..
    // but may be easier to inline for some compilers.

    return srcInteger | ltMask64(srcIndex) | geMask64(srcIndex + numBits);
}

inline
int bdes_BitUtil::rangeZero(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    // Note that the following is equivalent to
    //..
    //  srcInteger & oneMask(srcIndex, numBits)
    //..
    // but may be easier to inline for some compilers.

    return srcInteger & ltMask(srcIndex + numBits) & geMask(srcIndex);
}

inline
bsls_Types::Int64 bdes_BitUtil::rangeZero64(Int64 srcInteger,
                                            int   srcIndex,
                                            int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    // Note that the following is equivalent to:
    //..
    //  srcInteger & oneMask64(srcIndex, numBits)
    //..
    // but may be easier to inline for some compilers.

    return srcInteger & ltMask64(srcIndex + numBits) & geMask64(srcIndex);
}

inline
unsigned bdes_BitUtil::roundUpToBinaryPower(unsigned srcInteger)
{
    int      index = find1AtLargestIndex(srcInteger);
    unsigned ret   = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 <= index)
                     ? eqMask(index)
                     : 0;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ret == srcInteger)) {
        return ret;                                                   // RETURN
    }
    return ret << 1;
}

inline
bsls_Types::Uint64 bdes_BitUtil::roundUpToBinaryPower64(Uint64 srcInteger)
{
    int    index = find1AtLargestIndex64(srcInteger);
    Uint64 ret   = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 <= index)
                   ? eqMask64(index)
                   : 0;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ret == srcInteger)) {
        return ret;                                                   // RETURN
    }
    return ret << 1;
}

                               // -------------------
                               // All Other Functions
                               // -------------------

inline
void bdes_BitUtil::andEqual(int *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger &= neMask(dstIndex) | -static_cast<int>(value);
}

inline
void bdes_BitUtil::andEqual64(Int64 *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger &= neMask64(dstIndex) | -static_cast<int>(value);
}

inline
void bdes_BitUtil::andEqual(int *dstInteger,
                            int  dstIndex,
                            int  srcInteger,
                            int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 dstIndex < BDES_BITS_PER_INT)
                        ? srcInteger << dstIndex
                        : 0;
    *dstInteger &= rangeOne(shiftUp, dstIndex, numBits);
}

inline
void bdes_BitUtil::andEqual64(Int64 *dstInteger,
                              int    dstIndex,
                              Int64  srcInteger,
                              int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                dstIndex < BDES_BITS_PER_INT64)
                          ? srcInteger << dstIndex
                          : 0;
    *dstInteger &= rangeOne64(shiftUp, dstIndex, numBits);
}

inline
void bdes_BitUtil::andEqual(int *dstInteger,
                            int  dstIndex,
                            int  srcInteger,
                            int  srcIndex,
                            int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shift = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                      ? srcInteger >> srcIndex
                      : 0;

    // Note that the following is equivalent to:
    //..
    //  andEqual(dstInteger, dstIndex, shift, numBits)
    //..
    // but may be easier to inline for some compilers.

    const int tmp =
            BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT)
            ? shift << dstIndex
            : 0;
    *dstInteger &= rangeOne(tmp, dstIndex, numBits);
}

inline
void bdes_BitUtil::andEqual64(Int64 *dstInteger,
                              int    dstIndex,
                              Int64  srcInteger,
                              int    srcIndex,
                              int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shift = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                        ? srcInteger >> srcIndex
                        : 0;

    // Note that the following is equivalent to:
    //..
    //  andEqual64(dstInteger, dstIndex, shift, numBits)
    //..
    // but may be easier to inline for some compilers.

    const Int64 tmp =
            BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT64)
            ? shift << dstIndex
            : 0;
    *dstInteger &= rangeOne64(tmp, dstIndex, numBits);
}

inline
void bdes_BitUtil::insertOne(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int tmp = *dstInteger | geMask(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT)) {
        *dstInteger <<= numBits;
    }
    else {
        *dstInteger = 0;
    }
    *dstInteger |= ltMask(dstIndex + numBits);
    *dstInteger &= tmp;
}

inline
void bdes_BitUtil::insertOne64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 tmp = *dstInteger | geMask64(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT64)) {
        *dstInteger <<= numBits;
    }
    else {
        *dstInteger = 0;
    }
    *dstInteger |= ltMask64(dstIndex + numBits);
    *dstInteger &= tmp;
}

inline
void bdes_BitUtil::insertZero(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int tmp = *dstInteger & ltMask(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT)) {
        *dstInteger <<= numBits;
    }
    else {
        *dstInteger = 0;
    }
    *dstInteger &= geMask(dstIndex + numBits);
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::insertZero64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 tmp = *dstInteger & ltMask64(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT64)) {
        *dstInteger <<= numBits;
    }
    else {
        *dstInteger = 0;
    }
    *dstInteger &= geMask64(dstIndex + numBits);
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::insertValue(int *dstInteger,
                               int  dstIndex,
                               int  srcInteger,
                               int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 dstIndex < BDES_BITS_PER_INT)
                        ? ((srcInteger & ltMask(numBits)) << dstIndex)
                        : 0;
    const int tmp     = shiftUp | (*dstInteger & ltMask(dstIndex));
    *dstInteger = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                  numBits < BDES_BITS_PER_INT)
                  ? (*dstInteger & geMask(dstIndex)) << numBits
                  : 0;
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::insertValue64(Int64 *dstInteger,
                                 int    dstIndex,
                                 Int64  srcInteger,
                                 int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                dstIndex < BDES_BITS_PER_INT64)
                          ? (srcInteger & ltMask64(numBits)) << dstIndex
                          : 0;
    const Int64 tmp     = shiftUp | (*dstInteger & ltMask64(dstIndex));
    *dstInteger = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 numBits < BDES_BITS_PER_INT64)
                  ? (*dstInteger & geMask64(dstIndex)) << numBits
                  : 0;
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::insertValueUpTo(int *dstInteger,
                                   int  numBits,
                                   int  srcInteger)
{
    BSLS_ASSERT_SAFE(      0 <= numBits);
    BSLS_ASSERT_SAFE(numBits <= BDES_BITS_PER_INT);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT)) {
        *dstInteger <<= numBits;
    }
    *dstInteger &= geMask(numBits);
    *dstInteger |= srcInteger & ltMask(numBits);
}

inline
void bdes_BitUtil::insertValueUpTo64(Int64 *dstInteger,
                                     int    numBits,
                                     Int64  srcInteger)
{
    BSLS_ASSERT_SAFE(      0 <= numBits);
    BSLS_ASSERT_SAFE(numBits <= BDES_BITS_PER_INT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT64)) {
        *dstInteger <<= numBits;
    }
    *dstInteger &= geMask64(numBits);
    *dstInteger |= srcInteger & ltMask64(numBits);
}

inline
void bdes_BitUtil::minusEqual(int *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <= BDES_BITS_PER_INT);

    *dstInteger &= neMask(dstIndex) | -static_cast<int>(!value);
}

inline
void bdes_BitUtil::minusEqual64(Int64 *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <= BDES_BITS_PER_INT64);

    *dstInteger &= neMask64(dstIndex) | -static_cast<int>(!value);
}

inline
void bdes_BitUtil::minusEqual(int *dstInteger,
                              int  dstIndex,
                              int  srcInteger,
                              int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 dstIndex < BDES_BITS_PER_INT)
                        ? ~srcInteger << dstIndex
                        : 0;
    *dstInteger &= rangeOne(shiftUp, dstIndex, numBits);
}

inline
void bdes_BitUtil::minusEqual64(Int64 *dstInteger,
                                int    dstIndex,
                                Int64  srcInteger,
                                int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                dstIndex < BDES_BITS_PER_INT64)
                          ? ~srcInteger << dstIndex
                          : 0;
    *dstInteger &= rangeOne64(shiftUp, dstIndex, numBits);
}

inline
void bdes_BitUtil::minusEqual(int *dstInteger,
                              int  dstIndex,
                              int  srcInteger,
                              int  srcIndex,
                              int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftUp =
              BSLS_PERFORMANCEHINT_PREDICT_LIKELY(srcIndex < BDES_BITS_PER_INT)
              ? srcInteger >> srcIndex
              : 0;

    // Note that the following is equivalent to:
    //..
    //  minusEqual(dstInteger, dstIndex, shiftUp, numBits)
    //..
    // but may be easier to inline for some compilers.

    const int tmp =
              BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT)
              ? ~shiftUp << dstIndex
              : 0;
    *dstInteger &= rangeOne(tmp, dstIndex, numBits);
}

inline
void bdes_BitUtil::minusEqual64(Int64 *dstInteger,
                                int    dstIndex,
                                Int64  srcInteger,
                                int    srcIndex,
                                int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                          ? srcInteger >> srcIndex
                          : 0;

    // Note that the following is equivalent to:
    //..
    //  minusEqual64(dstInteger, dstIndex, shiftUp, numBits)
    //..
    // but may be easier to inline for some compilers.

    const Int64 tmp =
            BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT64)
            ? ~shiftUp << dstIndex
            : 0;
    *dstInteger &= rangeOne64(tmp, dstIndex, numBits);
}

inline
void bdes_BitUtil::orEqual(int *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger |= eqMask(dstIndex) & -static_cast<int>(value);
}

inline
void bdes_BitUtil::orEqual64(Int64 *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger |= eqMask64(dstIndex) & -static_cast<int>(value);
}

inline
void bdes_BitUtil::orEqual(int *dstInteger,
                           int  dstIndex,
                           int  srcInteger,
                           int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 dstIndex < BDES_BITS_PER_INT)
                        ? (srcInteger & ltMask(numBits)) << dstIndex
                        : 0;
    *dstInteger |= shiftUp;
}

inline
void bdes_BitUtil::orEqual64(Int64 *dstInteger,
                             int    dstIndex,
                             Int64  srcInteger,
                             int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftUp = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                dstIndex < BDES_BITS_PER_INT64)
                          ? (srcInteger & ltMask64(numBits)) << dstIndex
                          : 0;
    *dstInteger |= shiftUp;
}

inline
void bdes_BitUtil::orEqual(int *dstInteger,
                           int  dstIndex,
                           int  srcInteger,
                           int  srcIndex,
                           int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                          ? srcInteger >> srcIndex
                          : 0;

    // Note that the following is equivalent to:
    //..
    //  orEqual(dstInteger, dstIndex, shiftDown, numBits)
    //..
    // but may be easier to inline for some compilers.

    *dstInteger |= (shiftDown & ltMask(numBits)) << dstIndex;
}

inline
void bdes_BitUtil::orEqual64(Int64 *dstInteger,
                             int    dstIndex,
                             Int64  srcInteger,
                             int    srcIndex,
                             int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                            ? srcInteger >> srcIndex
                            : 0;

    // Note that the following is equivalent to:
    //..
    //  orEqual64(dstInteger, dstIndex, shiftDown, numBits)
    //..
    // but may be easier to inline for some compilers.

    *dstInteger |= (shiftDown & ltMask64(numBits)) << dstIndex;
}

inline
void bdes_BitUtil::removeOne(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int tmp = *dstInteger | geMask(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT)) {
        *dstInteger >>= numBits;
    }
    *dstInteger |= geMask(BDES_BITS_PER_INT - numBits);
    *dstInteger |= ltMask(dstIndex);
    *dstInteger &= tmp;
}

inline
void bdes_BitUtil::removeOne64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 tmp = *dstInteger | geMask64(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT64)) {
        *dstInteger >>= numBits;
    }
    *dstInteger |= geMask64(BDES_BITS_PER_INT64 - numBits);
    *dstInteger |= ltMask64(dstIndex);
    *dstInteger &= tmp;
}

inline
void bdes_BitUtil::removeZero(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    const int tmp = *dstInteger & ltMask(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT)) {
        *dstInteger >>= numBits;
    }
    *dstInteger &= oneMask(dstIndex, BDES_BITS_PER_INT - numBits - dstIndex);
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::removeZero64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 tmp = *dstInteger & ltMask64(dstIndex);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBits < BDES_BITS_PER_INT64)) {
        *dstInteger >>= numBits;
    }
    *dstInteger &= oneMask64(dstIndex,
                             BDES_BITS_PER_INT64 - numBits - dstIndex);
    *dstInteger |= tmp;
}

inline
void bdes_BitUtil::replaceBitOne(int *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger |= eqMask(dstIndex);
}

inline
void bdes_BitUtil::replaceBitOne64(Int64 *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger |= eqMask64(dstIndex);
}

inline
void bdes_BitUtil::replaceBitValue(int *dstInteger,
                                   int  dstIndex,
                                   int  value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger = (*dstInteger & neMask(dstIndex))
                | ((value & 0x1) << dstIndex);
}

inline
void bdes_BitUtil::replaceBitValue64(Int64 *dstInteger,
                                     int    dstIndex,
                                     int    value)
{
    BSLS_ASSERT_SAFE(       0 <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger = (*dstInteger & neMask64(dstIndex))
                | (static_cast<Int64>(value & 0x1) << dstIndex);
}

inline
void bdes_BitUtil::replaceBitZero(int *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger &= neMask(dstIndex);
}

inline
void bdes_BitUtil::replaceBitZero64(Int64 *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger &= neMask64(dstIndex);
}

inline
void bdes_BitUtil::replaceOne(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    *dstInteger |= oneMask(dstIndex, numBits);
}

inline
void bdes_BitUtil::replaceOne64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    *dstInteger |= oneMask64(dstIndex, numBits);
}

inline
void bdes_BitUtil::replaceValue(int *dstInteger,
                                int  dstIndex,
                                int  srcInteger,
                                int  numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    *dstInteger &= zeroMask(dstIndex, numBits);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT)) {
        *dstInteger |= (srcInteger & ltMask(numBits)) << dstIndex;
    }
}

inline
void bdes_BitUtil::replaceValue64(Int64 *dstInteger,
                                  int    dstIndex,
                                  Int64  srcInteger,
                                  int    numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    *dstInteger &= zeroMask64(dstIndex, numBits);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < BDES_BITS_PER_INT64)) {
        *dstInteger |= (srcInteger & ltMask64(numBits)) << dstIndex;
    }
}

inline
void bdes_BitUtil::replaceValueUpTo(int *dstInteger,
                                    int  dstIndex,
                                    int  srcInteger)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <= BDES_BITS_PER_INT);

    *dstInteger =
            (*dstInteger & geMask(dstIndex)) | (srcInteger & ltMask(dstIndex));
}

inline
void bdes_BitUtil::replaceValueUpTo64(Int64 *dstInteger,
                                      int    dstIndex,
                                      Int64  srcInteger)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <= BDES_BITS_PER_INT64);

    *dstInteger =
        (*dstInteger & geMask64(dstIndex)) | (srcInteger & ltMask64(dstIndex));
}

inline
void bdes_BitUtil::replaceZero(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    *dstInteger &= zeroMask(dstIndex, numBits);
}

inline
void bdes_BitUtil::replaceZero64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    *dstInteger &= zeroMask64(dstIndex, numBits);
}

inline
void bdes_BitUtil::toggle(int *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    *dstInteger ^= oneMask(dstIndex, numBits);
}

inline
void bdes_BitUtil::toggle64(Int64 *dstInteger, int dstIndex, int numBits)
{
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    *dstInteger ^= oneMask64(dstIndex, numBits);
}

inline
void bdes_BitUtil::toggleBit(int *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger ^= eqMask(dstIndex);
}

inline
void bdes_BitUtil::toggleBit64(Int64 *dstInteger, int dstIndex)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger ^= eqMask64(dstIndex);
}

inline
void bdes_BitUtil::xorEqual(int *dstInteger, int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT);

    *dstInteger ^= eqMask(dstIndex) & -static_cast<int>(value);
}

inline
void bdes_BitUtil::xorEqual64(Int64 *dstInteger,
                              int    dstIndex,
                              bool   value)
{
    BSLS_ASSERT_SAFE(0        <= dstIndex);
    BSLS_ASSERT_SAFE(dstIndex <  BDES_BITS_PER_INT64);

    *dstInteger ^= eqMask64(dstIndex) & -static_cast<int>(value);
}

inline
void bdes_BitUtil::xorEqual(int *dstInteger,
                            int  dstIndex,
                            int  srcInteger,
                            int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);

    *dstInteger ^= (srcInteger & ltMask(numBits)) << dstIndex;
}

inline
void bdes_BitUtil::xorEqual64(Int64 *dstInteger,
                              int    dstIndex,
                              Int64  srcInteger,
                              int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);

    *dstInteger ^= (srcInteger & ltMask64(numBits)) << dstIndex;
}

inline
void bdes_BitUtil::xorEqual(int *dstInteger,
                            int  dstIndex,
                            int  srcInteger,
                            int  srcIndex,
                            int  numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                          ? srcInteger >> srcIndex
                          : 0;

    // Note that the following is equivalent to:
    //..
    //  xorEqual(dstInteger, dstIndex, shiftDown, numBits)
    //..
    // but may be easier to inline for some compilers.

    *dstInteger ^= (shiftDown & ltMask(numBits)) << dstIndex;
}

inline
void bdes_BitUtil::xorEqual64(Int64 *dstInteger,
                              int    dstIndex,
                              Int64  srcInteger,
                              int    srcIndex,
                              int    numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= BDES_BITS_PER_INT64);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                            ? srcInteger >> srcIndex
                            : 0;

    // Note that the following is equivalent to:
    //..
    //  xorEqual64(dstInteger, dstIndex, shiftDown, numBits)
    //..
    // but may be easier to inline for some compilers.

    *dstInteger ^= (shiftDown & ltMask64(numBits)) << dstIndex;
}

inline
int bdes_BitUtil::base2Log(unsigned srcInteger)
{
    return numSetOne(roundUpToBinaryPower(srcInteger) - 1);
}

inline
int bdes_BitUtil::base2Log64(Uint64 srcInteger)
{
    return numSetOne64(roundUpToBinaryPower64(srcInteger) - 1);
}

inline
int bdes_BitUtil::extractOne(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                          ? srcInteger >> srcIndex
                          : 0;
    return shiftDown | geMask(numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::extractOne64(Int64 srcInteger,
                                             int   srcIndex,
                                             int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                            ? srcInteger >> srcIndex
                            : 0;
    return shiftDown | geMask64(numBits);
}

inline
int bdes_BitUtil::extractOne(int srcInteger,
                             int srcIndex,
                             int numBits,
                             int offset)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= offset);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT)) {
        return (extractOne(srcInteger, srcIndex, numBits) << offset)
             | ltMask(offset);                                        // RETURN
    }
    return -1;
}

inline
bsls_Types::Int64 bdes_BitUtil::extractOne64(Int64 srcInteger,
                                             int   srcIndex,
                                             int   numBits,
                                             int   offset)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= offset);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT64)) {
        return (extractOne64(srcInteger, srcIndex, numBits) << offset)
             | ltMask64(offset);                                      // RETURN
    }
    return static_cast<Int64>(-1);
}

inline
int bdes_BitUtil::extractZero(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    const int shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                    ? srcInteger >> srcIndex
                    : 0;
    return shiftDown & ltMask(numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::extractZero64(Int64 srcInteger,
                                              int   srcIndex,
                                              int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    const Int64 shiftDown = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                      ? srcInteger >> srcIndex
                      : 0;
    return shiftDown & ltMask64(numBits);
}

inline
int bdes_BitUtil::extractZero(int srcInteger,
                              int srcIndex,
                              int numBits,
                              int offset)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= offset);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    // Note that the following is equivalent to:
    //..
    //  return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT)
    //         ? (extractZero(srcInteger, srcIndex, numBits) << offset)
    //         : 0;
    //..
    // but may be easier to inline for some compilers.

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT)
           ? (ltMask(numBits) & (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                 srcIndex < BDES_BITS_PER_INT)
                                 ? srcInteger >> srcIndex
                                 : 0)) << offset
           : 0;
}

inline
bsls_Types::Int64 bdes_BitUtil::extractZero64(Int64 srcInteger,
                                              int   srcIndex,
                                              int   numBits,
                                              int   offset)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(                 0 <= offset);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    // Note that the following is equivalent to:
    //..
    //  return
    //        BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT64)
    //        ? (extractZero64(srcInteger, srcIndex, numBits) << offset)
    //        : 0;
    //..
    // but may be easier to inline for some compilers.

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset < BDES_BITS_PER_INT64)
           ? (ltMask64(numBits) & (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                                srcIndex < BDES_BITS_PER_INT64)
                                   ? srcInteger >> srcIndex
                                   : 0)) << offset
           : 0;
}

inline
int bdes_BitUtil::find0AtLargestIndexGE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtLargestIndex(srcInteger | ltMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexGE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtLargestIndex64(srcInteger | ltMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexGT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtLargestIndex(srcInteger | leMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexGT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtLargestIndex64(srcInteger | leMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexLE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtLargestIndex(srcInteger | gtMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexLE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtLargestIndex64(srcInteger | gtMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexLT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtLargestIndex(srcInteger | geMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtLargestIndexLT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtLargestIndex64(srcInteger | geMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexGE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtSmallestIndex(srcInteger | ltMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexGE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtSmallestIndex64(srcInteger | ltMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexGT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtSmallestIndex(srcInteger | leMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexGT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtSmallestIndex64(srcInteger | leMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexLE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtSmallestIndex(srcInteger | gtMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexLE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtSmallestIndex64(srcInteger | gtMask64(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexLT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find0AtSmallestIndex(srcInteger | geMask(srcIndex));
}

inline
int bdes_BitUtil::find0AtSmallestIndexLT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find0AtSmallestIndex64(srcInteger | geMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexGE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtLargestIndex(srcInteger & geMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexGE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtLargestIndex64(srcInteger & geMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexGT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtLargestIndex(srcInteger & gtMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexGT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtLargestIndex64(srcInteger & gtMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexLE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtLargestIndex(srcInteger & leMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexLE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtLargestIndex64(srcInteger & leMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexLT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtLargestIndex(srcInteger & ltMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtLargestIndexLT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtLargestIndex64(srcInteger & ltMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexGE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtSmallestIndex(srcInteger & geMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexGE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtSmallestIndex64(srcInteger & geMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexGT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtSmallestIndex(srcInteger & gtMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexGT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtSmallestIndex64(srcInteger & gtMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexLE(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtSmallestIndex(srcInteger & leMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexLE64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtSmallestIndex64(srcInteger & leMask64(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexLT(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT);

    return find1AtSmallestIndex(srcInteger & ltMask(srcIndex));
}

inline
int bdes_BitUtil::find1AtSmallestIndexLT64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <= BDES_BITS_PER_INT64);

    return find1AtSmallestIndex64(srcInteger & ltMask64(srcIndex));
}

inline
int bdes_BitUtil::intAbs(int srcInteger)
{
    return srcInteger ^ ((-srcInteger ^ srcInteger) & -(srcInteger < 0));
}

inline
bsls_Types::Int64 bdes_BitUtil::intAbs64(Int64 srcInteger)
{
    return srcInteger ^ ((-srcInteger ^ srcInteger)
                       & -static_cast<int>(srcInteger < 0));
}

inline
int bdes_BitUtil::intMax(int srcIntegerX, int srcIntegerY)
{
    return srcIntegerX ^ ((srcIntegerY ^ srcIntegerX)
                        & -(srcIntegerY > srcIntegerX));
}

inline
bsls_Types::Int64 bdes_BitUtil::intMax64(Int64 srcIntegerX, Int64 srcIntegerY)
{
    return srcIntegerX ^ ((srcIntegerY ^ srcIntegerX)
                        & -static_cast<int>(srcIntegerY > srcIntegerX));
}

inline
int bdes_BitUtil::intMin(int srcIntegerX, int srcIntegerY)
{
    return srcIntegerX ^ ((srcIntegerY ^ srcIntegerX)
                        & -(srcIntegerY < srcIntegerX));
}

inline
bsls_Types::Int64 bdes_BitUtil::intMin64(Int64 srcIntegerX, Int64 srcIntegerY)
{
    return srcIntegerX ^ ((srcIntegerY ^ srcIntegerX)
                        & -static_cast<int>(srcIntegerY < srcIntegerX));
}

inline
bool bdes_BitUtil::isAnySetOne(int srcInteger)
{
    return !!srcInteger;
}

inline
bool bdes_BitUtil::isAnySetOne64(Int64 srcInteger)
{
    return !!srcInteger;
}

inline
bool bdes_BitUtil::isAnySetZero(int srcInteger)
{
    return srcInteger != ~0;
}

inline
bool bdes_BitUtil::isAnySetZero64(Int64 srcInteger)
{
    return srcInteger != ~static_cast<Int64>(0);
}

inline
bool bdes_BitUtil::isSetOne(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    return !!(eqMask(srcIndex) & srcInteger);
}

inline
bool bdes_BitUtil::isSetOne64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    return !!(eqMask64(srcIndex) & srcInteger);
}

inline
bool bdes_BitUtil::isSetZero(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    return !(eqMask(srcIndex) & srcInteger);
}

inline
bool bdes_BitUtil::isSetZero64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    return !(eqMask64(srcIndex) & srcInteger);
}

inline
int bdes_BitUtil::merge(int zeroSrcInteger, int oneSrcInteger, int mask)
{
    return zeroSrcInteger ^ ((zeroSrcInteger ^ oneSrcInteger) & mask);
}

inline
bsls_Types::Int64 bdes_BitUtil::merge64(Int64 zeroSrcInteger,
                                        Int64 oneSrcInteger,
                                        Int64 mask)
{
    return zeroSrcInteger ^ ((zeroSrcInteger ^ oneSrcInteger) & mask);
}

inline
int bdes_BitUtil::numSetZero(int srcInteger)
{
    return numSetOne(~srcInteger);
}

inline
int bdes_BitUtil::numSetZero64(Int64 srcInteger)
{
    return numSetOne64(~srcInteger);
}

inline
int bdes_BitUtil::setBitOne(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    return srcInteger | eqMask(srcIndex);
}

inline
bsls_Types::Int64 bdes_BitUtil::setBitOne64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    return srcInteger | eqMask64(srcIndex);
}

inline
int bdes_BitUtil::setBitValue(int srcInteger, int srcIndex, int value)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    // Note that the following is equivalent to:
    //..
    //  replaceBitValue(&srcInteger, srcIndex, value);
    //  return srcInteger;
    //..
    // but may be easier to inline for some compilers.

    return (srcInteger & neMask(srcIndex)) | ((value & 1) << srcIndex);
}

inline
bsls_Types::Int64 bdes_BitUtil::setBitValue64(Int64 srcInteger,
                                              int   srcIndex,
                                              int   value)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    // Note that the following is equivalent to:
    //..
    //  replaceBitValue64(&srcInteger, srcIndex, value);
    //  return srcInteger;
    //..
    // but may be easier to inline for some compilers.

    return (srcInteger & neMask64(srcIndex))
         | (static_cast<Int64>(value & 1) << srcIndex);
}

inline
int bdes_BitUtil::setBitZero(int srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    return srcInteger & neMask(srcIndex);
}

inline
bsls_Types::Int64 bdes_BitUtil::setBitZero64(Int64 srcInteger, int srcIndex)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    return srcInteger & neMask64(srcIndex);
}

inline
int bdes_BitUtil::setOne(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    return srcInteger | oneMask(srcIndex, numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::setOne64(Int64 srcInteger,
                                         int   srcIndex,
                                         int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    return srcInteger | oneMask64(srcIndex, numBits);
}

inline
int bdes_BitUtil::setValue(int srcInteger,
                           int srcIndex,
                           int value,
                           int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    int ret = srcInteger & zeroMask(srcIndex, numBits);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(srcIndex < BDES_BITS_PER_INT)) {
        return ret | ((value & ltMask(numBits)) << srcIndex);         // RETURN
    }
    return ret;
}

inline
bsls_Types::Int64 bdes_BitUtil::setValue64(Int64 srcInteger,
                                           int   srcIndex,
                                           Int64 value,
                                           int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    Int64 ret = srcInteger & zeroMask64(srcIndex, numBits);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(srcIndex < BDES_BITS_PER_INT64)) {
        return ret | ((value & ltMask64(numBits)) << srcIndex);       // RETURN
    }
    return ret;
}

inline
int bdes_BitUtil::setValueUpTo(int srcInteger, int srcIndex, int value)
{
    BSLS_ASSERT_SAFE(       0 <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT);

    return (value & ltMask(srcIndex)) | (srcInteger & geMask(srcIndex));
}

inline
bsls_Types::Int64 bdes_BitUtil::setValueUpTo64(Int64 srcInteger,
                                               int   srcIndex,
                                               Int64 value)
{
    BSLS_ASSERT_SAFE(0        <= srcIndex);
    BSLS_ASSERT_SAFE(srcIndex <  BDES_BITS_PER_INT64);

    return (value & ltMask64(srcIndex)) | (srcInteger & geMask64(srcIndex));
}

inline
int bdes_BitUtil::setZero(int srcInteger, int srcIndex, int numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT);

    return srcInteger & zeroMask(srcIndex, numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::setZero64(Int64 srcInteger,
                                          int   srcIndex,
                                          int   numBits)
{
    BSLS_ASSERT_SAFE(                 0 <= srcIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= BDES_BITS_PER_INT64);

    return srcInteger & zeroMask64(srcIndex, numBits);
}

template <typename TYPE>
inline
int bdes_BitUtil::sizeInBits(TYPE)
{
    return CHAR_BIT * sizeof(TYPE);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdes_BitUtil::findBit0AtLargestIndex(int srcInteger)
{
    return find0AtLargestIndex(srcInteger);
}

inline
int bdes_BitUtil::findBit0AtLargestIndex64(Int64 srcInteger)
{
    return find0AtLargestIndex64(srcInteger);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexGE(int srcInteger, int srcIndex)
{
    return find0AtLargestIndexGE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexGE64(Int64 srcInteger, int srcIndex)
{
    return find0AtLargestIndexGE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexGT(int srcInteger, int srcIndex)
{
    return find0AtLargestIndexGT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexGT64(Int64 srcInteger, int srcIndex)
{
    return find0AtLargestIndexGT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexLE(int srcInteger, int srcIndex)
{
    return find0AtLargestIndexLE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexLE64(Int64 srcInteger, int srcIndex)
{
    return find0AtLargestIndexLE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexLT(int srcInteger, int srcIndex)
{
    return find0AtLargestIndexLT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtLargestIndexLT64(Int64 srcInteger, int srcIndex)
{
    return find0AtLargestIndexLT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndex(int srcInteger)
{
    return find0AtSmallestIndex(srcInteger);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndex64(Int64 srcInteger)
{
    return find0AtSmallestIndex64(srcInteger);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexGE(int srcInteger, int srcIndex)
{
    return find0AtSmallestIndexGE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexGE64(Int64 srcInteger, int srcIndex)
{
    return find0AtSmallestIndexGE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexGT(int srcInteger, int srcIndex)
{
    return find0AtSmallestIndexGT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexGT64(Int64 srcInteger, int srcIndex)
{
    return find0AtSmallestIndexGT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexLE(int srcInteger, int srcIndex)
{
    return find0AtSmallestIndexLE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexLE64(Int64 srcInteger, int srcIndex)
{
    return find0AtSmallestIndexLE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexLT(int srcInteger, int srcIndex)
{
    return find0AtSmallestIndexLT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit0AtSmallestIndexLT64(Int64 srcInteger, int srcIndex)
{
    return find0AtSmallestIndexLT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndex(int srcInteger)
{
    return find1AtLargestIndex(srcInteger);
}

inline
int bdes_BitUtil::findBit1AtLargestIndex64(Int64 srcInteger)
{
    return find1AtLargestIndex64(srcInteger);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexGE(int srcInteger, int srcIndex)
{
    return find1AtLargestIndexGE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexGE64(Int64 srcInteger, int srcIndex)
{
    return find1AtLargestIndexGE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexGT(int srcInteger, int srcIndex)
{
    return find1AtLargestIndexGT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexGT64(Int64 srcInteger, int srcIndex)
{
    return find1AtLargestIndexGT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexLE(int srcInteger, int srcIndex)
{
    return find1AtLargestIndexLE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexLE64(Int64 srcInteger, int srcIndex)
{
    return find1AtLargestIndexLE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexLT(int srcInteger, int srcIndex)
{
    return find1AtLargestIndexLT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtLargestIndexLT64(Int64 srcInteger, int srcIndex)
{
    return find1AtLargestIndexLT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndex(int srcInteger)
{
    return find1AtSmallestIndex(srcInteger);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndex64(Int64 srcInteger)
{
    return find1AtSmallestIndex64(srcInteger);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexGE(int srcInteger, int srcIndex)
{
    return find1AtSmallestIndexGE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexGE64(Int64 srcInteger, int srcIndex)
{
    return find1AtSmallestIndexGE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexGT(int srcInteger, int srcIndex)
{
    return find1AtSmallestIndexGT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexGT64(Int64 srcInteger, int srcIndex)
{
    return find1AtSmallestIndexGT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexLE(int srcInteger, int srcIndex)
{
    return find1AtSmallestIndexLE(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexLE64(Int64 srcInteger, int srcIndex)
{
    return find1AtSmallestIndexLE64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexLT(int srcInteger, int srcIndex)
{
    return find1AtSmallestIndexLT(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::findBit1AtSmallestIndexLT64(Int64 srcInteger, int srcIndex)
{
    return find1AtSmallestIndexLT64(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::abs32(int srcInteger)
{
    return intAbs(srcInteger);
}

inline
bsls_Types::Int64 bdes_BitUtil::abs64(Int64 srcInteger)
{
    return intAbs64(srcInteger);
}

inline
int bdes_BitUtil::max32(int srcIntegerX, int srcIntegerY)
{
    return intMax(srcIntegerX, srcIntegerY);
}

inline
bsls_Types::Int64 bdes_BitUtil::max64(Int64 srcIntegerX, Int64 srcIntegerY)
{
    return intMax64(srcIntegerX, srcIntegerY);
}

inline
int bdes_BitUtil::min32(int srcIntegerX, int srcIntegerY)
{
    return intMin(srcIntegerX, srcIntegerY);
}

inline
bsls_Types::Int64 bdes_BitUtil::min64(Int64 srcIntegerX, Int64 srcIntegerY)
{
    return intMin64(srcIntegerX, srcIntegerY);
}

inline
int bdes_BitUtil::numSet0(int srcInteger)
{
    return numSetZero(srcInteger);
}

inline
int bdes_BitUtil::numSet1(int srcInteger)
{
    return numSetOne(srcInteger);
}

inline
bool bdes_BitUtil::isSet0(int srcInteger, int srcIndex)
{
    return isSetZero(srcInteger, srcIndex);
}

inline
bool bdes_BitUtil::isSet1(int srcInteger, int srcIndex)
{
    return isSetOne(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::setBit0(int srcInteger, int srcIndex)
{
    return setBitZero(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::setBit1(int srcInteger, int srcIndex)
{
    return setBitOne(srcInteger, srcIndex);
}

inline
int bdes_BitUtil::extract0(int srcInteger, int srcIndex, int numBits)
{
    return extractZero(srcInteger, srcIndex, numBits);
}

inline
int bdes_BitUtil::extract0(int srcInteger,
                           int srcIndex,
                           int numBits,
                           int offset)
{
    return extractZero(srcInteger, srcIndex, numBits, offset);
}

inline
int bdes_BitUtil::extract1(int srcInteger, int srcIndex, int numBits)
{
    return extractOne(srcInteger, srcIndex, numBits);
}

inline
int bdes_BitUtil::extract1(int srcInteger,
                           int srcIndex,
                           int numBits,
                           int offset)
{
    return extractOne(srcInteger, srcIndex, numBits, offset);
}

inline
void bdes_BitUtil::replaceBit0(int *dstInteger, int dstIndex)
{
    return replaceBitZero(dstInteger, dstIndex);
}

inline
void bdes_BitUtil::replaceBit1(int *dstInteger, int dstIndex)
{
    return replaceBitOne(dstInteger, dstIndex);
}

inline
void bdes_BitUtil::remove0(int *dstInteger, int dstIndex, int numBits)
{
    return removeZero(dstInteger, dstIndex, numBits);
}

inline
void bdes_BitUtil::remove1(int *dstInteger, int dstIndex, int numBits)
{
    return removeOne(dstInteger, dstIndex, numBits);
}

inline
void bdes_BitUtil::insert0(int *dstInteger, int dstIndex, int numBits)
{
    return insertZero(dstInteger, dstIndex, numBits);
}

inline
void bdes_BitUtil::insert1(int *dstInteger, int dstIndex, int numBits)
{
    return insertOne(dstInteger, dstIndex, numBits);
}

inline
int bdes_BitUtil::set0(int srcInteger, int srcIndex, int numBits)
{
    return setZero(srcInteger, srcIndex, numBits);
}

inline
int bdes_BitUtil::set1(int srcInteger, int srcIndex, int numBits)
{
    return setOne(srcInteger, srcIndex, numBits);
}

inline
void bdes_BitUtil::replace0(int *dstInteger, int dstIndex, int numBits)
{
    return replaceZero(dstInteger, dstIndex, numBits);
}

inline
void bdes_BitUtil::replace1(int *dstInteger, int dstIndex, int numBits)
{
    return replaceOne(dstInteger, dstIndex, numBits);
}

inline
bool bdes_BitUtil::isAnySet0(int srcInteger)
{
    return isAnySetZero(srcInteger);
}

inline
bool bdes_BitUtil::isAnySet1(int srcInteger)
{
    return isAnySetOne(srcInteger);
}

inline
int bdes_BitUtil::range0(int srcInteger, int srcIndex, int numBits)
{
    return rangeZero(srcInteger, srcIndex, numBits);
}

inline
int bdes_BitUtil::range1(int srcInteger, int srcIndex, int numBits)
{
    return rangeOne(srcInteger, srcIndex, numBits);
}

inline
int bdes_BitUtil::maskZero(int index, int numBits)
{
    return zeroMask(index, numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::maskZero64(int index, int numBits)
{
    return zeroMask64(index, numBits);
}

inline
int bdes_BitUtil::maskOne(int index, int numBits)
{
    return oneMask(index, numBits);
}

inline
bsls_Types::Int64 bdes_BitUtil::maskOne64(int index, int numBits)
{
    return oneMask64(index, numBits);
}

inline
int bdes_BitUtil::mask0(int index, int numBits)
{
    return zeroMask(index, numBits);
}

inline
int bdes_BitUtil::mask1(int index, int numBits)
{
    return oneMask(index, numBits);
}

template <typename TYPE>
inline
int bdes_BitUtil::numBits(TYPE value)
{
    return sizeInBits(value);
}

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
