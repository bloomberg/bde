// bdlb_bitstringutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLB_BITSTRINGUTIL
#define INCLUDED_BDLB_BITSTRINGUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient operations on a multi-word sequence of bits.
//
//@CLASSES:
// bdlb::BitStringUtil: namespace for common bit-manipulation procedures
//
//@SEE_ALSO: bdlb_bitutil, bdlb_bitmaskutil, bdlb_bitstringimputil,
//           bdlc_bitarray
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlb::BitStringUtil', that serves as a namespace for a collection of
// efficient, bit-level procedures on "bit strings", sequences of bits stored
// in arrays of 64-bit 'uint64_t' values.  A number of operations of various
// types are provided: bitwise-logical, copy, assignment, read, insert/remove,
// compare, find, count, and print operations are offered, among others.
//
///The "Bit String" Pseudo-Type
///----------------------------
// A contiguous sequence of bits that occupy a positive integral number of
// sequential 'uint64_t' values can be viewed as a string of bits.  This
// component supports operations on such sequences.  The notion of "bit
// string", a pseudo-type, is used to document those operations.
// Correspondingly, 'BitStringUtil' operations are categorized as either
// "manipulators", operations that modify bit strings; or "accessors",
// operations that return information and guarantee that no change to bit
// strings occurs.
//
// A bit string has two "structural" attributes:
//..
//  Capacity - The capacity, in bits, of a bit string is the number of
//             'uint64_t' values in the array multiplied by
//             'BitStringUtil::k_BITS_PER_UINT64'.  Note that the capacity of a
//             bit string is analogous to the capacity of a 'bsl::vector'.
//
//  Length   - The number of significant bits stored in the bit string.  The
//             length can never exceed the capacity, and the length is
//             analogous to the length of a 'bsl::vector'.
//..
// Since the bit string is a pseudo-type, there is no language support for
// managing these values; the user must do so explicitly.
//
// Many operations on a bit string refer to a "position" within a bit string,
// or a range of positions within a bit string:
//..
//  Position - The offset (in bits) of a bit value from the beginning of a
//             bit string (also called the "index" of a bit).
//..
// The notion of "position" used in this component is a generalization of the
// notion of a bit's position in a single integer value.
//
// Bits within a 64-bit 'uint64_t' (irrespective of the endian-ness of a
// platform) are here numbered, starting at 0, from the least-significant bit
// to the most-significant bit.  In illustrations, we typically show the
// high-order bits on the left:
//..
//   63 62  . . . . .   5  4  3  2  1  0
//  +-----------------------------------+
//  | 1| 0| . . . . . | 1| 1| 0| 0| 1| 0|
//  +-----------------------------------+
//..
// Thus, left-shifting (e.g., caused by 'insert'ing low-order bits) causes bits
// to move up in bit-position (to positions of higher significance) and
// right-shifting (e.g., caused by 'remove'ing low-order bits) causes bits to
// move into positions of less significance.
//
// This component extends this representation to an arbitrary sequence of bits
// represented using an array of 64-bit 'uint64_t' values.  For example, the
// bit string shown below is built on an array of three 'uint64_t' values.
// Thus, it has a capacity of 192 (i.e.,
// '3 * BitStringUtil::k_BITS_PER_UINT64').  Note that words are ordered
// right-to-left, so the lowest-order bits are to the right.  This is also how
// the 'bdlb::BitStringUtil::print' function orders the words it outputs:
//..
//  |<------ word 2 ------>|<------ word 1 ------>|<------ word 0 ------>|
//  | 191 190 . .  129 128 | 127 126 . . .  65 64 | 63 62 . . . . .  1 0 |
//  +----------------------+----------------------+----------------------+
//..
//
///Manipulator Functions
///---------------------
// Manipulator functions return 'void', and take the address of an integer as
// the first argument in order to modify it in place.
//..
//
//
//                                 Assignment
// +--------------------------------------------------------------------------+
// | assign     | Overloaded; assign 0 or more contiguous bits to a specified |
// |            | 'bool' value.                                               |
// +--------------------------------------------------------------------------+
// | assign0    | Overloaded; assign 0 or more contiguous bits to 'false'.    |
// +--------------------------------------------------------------------------+
// | assign1    | Overloaded; assign 0 or more contiguous bits to 'true'.     |
// +--------------------------------------------------------------------------+
// | assignBits | Assign up to one word of contiguous bits, taken from a      |
// |            | 'uint64_t' argument.                                        |
// +--------------------------------------------------------------------------+
//
//
//                                Bitwise-Logical
// +--------------------------------------------------------------------------+
// | andEqual   | Bitwise-AND ranges of equal length from two bit strings, a  |
// |            | 'dstBitString' and a 'srcBitString', writing the result     |
// |            | over the range from 'dstBitString'.                         |
// +--------------------------------------------------------------------------+
// | minusEqual | Bitwise-MINUS ranges of equal length from two bit strings,  |
// |            | a 'srcBitString' from a 'dstBitString', writing the result  |
// |            | over the range from 'dstBitString'.                         |
// +--------------------------------------------------------------------------+
// | orEqual    | Bitwise-OR ranges of equal length from two bit strings, a   |
// |            | 'dstBitString' and a 'srcBitString', writing the result     |
// |            | over the range from 'dstBitString'.                         |
// +--------------------------------------------------------------------------+
// | xorEqual   | Bitwise-XOR ranges of equal length from two bit strings, a  |
// |            | 'dstBitString' and a 'srcBitString', writing the result     |
// |            | over the range from 'dstBitString'.                         |
// +--------------------------------------------------------------------------+
//
//
//                                      Copy
// +--------------------------------------------------------------------------+
// | copyRaw | Copy a range from one bit string to another range, with some   |
// |         | restrictions on overlap between the two ranges.                |
// +--------------------------------------------------------------------------+
// | copy    | Copy a range from one bit string to another range, with no     |
// |         | restrictions on overlap between the two ranges.                |
// +--------------------------------------------------------------------------+
//
//
//                                 Insert / Remove
// +--------------------------------------------------------------------------+
// | insert    | Overloaded; insert 0 or more bits of a specified 'bool'      |
// |           | value.                                                       |
// +--------------------------------------------------------------------------+
// | insert0   | Overloaded; insert 0 or more 'false' bits.                   |
// +--------------------------------------------------------------------------+
// | insert1   | Overloaded; insert 0 or more 'true' bits.                    |
// +--------------------------------------------------------------------------+
// | insertRaw | Make room for additional bits in a bit string by moving all  |
// |           | bits above a given index up, leaving the values of the       |
// |           | newly-inserted bits undefined.                               |
// +--------------------------------------------------------------------------+
// | remove         | Remove 0 or more bits from a bit string.                |
// +--------------------------------------------------------------------------+
// | removeAndFill0 | Remove 0 or more bits from a bit string and assign      |
// |                | 'false' to vacated higher-order bits.                   |
// +--------------------------------------------------------------------------+
// | removeAndFill1 | Remove 0 or more bits from a bit string and assign      |
// |                | 'true' to vacated higher-order bits.                    |
// +--------------------------------------------------------------------------+
//
//
//                                Other Manipulators
// +--------------------------------------------------------------------------+
// | swapRaw | Swap two ranges of bit strings, which must not overlap.        |
// +--------------------------------------------------------------------------+
// | toggle  | Negate all the bits in a range of a bit string.                |
// +--------------------------------------------------------------------------+
//
//..
//
///Accessor Functions
///------------------
// Accessor function return a value but do not modify a bit string.
//..
//
//                                    Compare
// +--------------------------------------------------------------------------+
// | areEqual | Compare two ranges of bits for equality.                      |
// +--------------------------------------------------------------------------+
//
//
//                                     Read
// +--------------------------------------------------------------------------+
// | bit  | Return the boolean value of a single bit from a bit string.       |
// +--------------------------------------------------------------------------+
// | bits | Return a 'uint64_t' containing at most 'k_BITS_PER_UINT64'        |
// |      | adjacent bits from a bit string.                                  |
// +--------------------------------------------------------------------------+
//
//
//                                     Find
// +--------------------------------------------------------------------------+
// | find0AtMaxIndex | Locate the highest-order 0 bit in a range.             |
// +--------------------------------------------------------------------------+
// | find0AtMinIndex | Locate the lowest-order 0 bit in a range.              |
// +--------------------------------------------------------------------------+
// | find1AtMaxIndex | Locate the highest-order 1 bit in a range.             |
// +--------------------------------------------------------------------------+
// | find1AtMinIndex | Locate the lowest-order 1 bit in a range.              |
// +--------------------------------------------------------------------------+
//
//
//                                     Count
// +--------------------------------------------------------------------------+
// | isAny0 | Return 'true' if any bit in a range is 0, and 'false'           |
// |        | otherwise.                                                      |
// +--------------------------------------------------------------------------+
// | isAny1 | Return 'true' if any bit in a range is 1, and 'false'           |
// |        | otherwise.                                                      |
// +--------------------------------------------------------------------------+
// | num0   | Return the number of 0 bits in a range.                         |
// +--------------------------------------------------------------------------+
// | num1   | Return the number of 1 bits in a range.                         |
// +--------------------------------------------------------------------------+
//
//
//                                    Output
// +--------------------------------------------------------------------------+
// | print | Output a bit string in hex.                                      |
// +--------------------------------------------------------------------------+
//
//..
//
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
//
//  enum {
//      JAN =        0,    // Note: First DOY is 'JAN + 1'.
//      FEB = JAN + 31,
//      MAR = FEB + 28,    // 2013 was not a leap year.
//      APR = MAR + 31,
//      MAY = APR + 30,
//      JUN = MAY + 31,
//      JUL = JUN + 30,
//      AUG = JUL + 31,
//      SEP = AUG + 31,
//      OCT = SEP + 30,
//      NOV = OCT + 31,
//      DEC = NOV + 30
//  };
//..
// Then, create a bit string with sufficient capacity to represent every day
// of a year (note that 64 * 6 = 384) and set a 1-bit in the indices
// corresponding to the day-of-year (DOY) for each weekend day.  For
// convenience in date calculations, the 0 index is not used; the 365 days of
// the year are at indices '[1 .. 365]'.  Further note that the values set
// below correspond to the year 2013:
//..
//  uint64_t weekends[6] = { 0 };
//
//  // We are marking only weekend days, so start with the first weekend day
//  // of the year: Saturday, January 5, 2013.
//
//  for (int i = 5; i < 366; i += 7) {
//      bdlb::BitStringUtil::assign(weekends, i,   1);
//      if (i + 1 < 366) {
//          bdlb::BitStringUtil::assign(weekends, i + 1, 1);
//      }
//  }
//..
// Next, we can easily use 'bdlb::BitStringUtil' methods to find days of
// interest.  For example, we can find the first and last weekend days of the
// year:
//..
//  const int firstWeekendDay = bdlb::BitStringUtil::find1AtMinIndex(weekends,
//                                                                   365 + 1);
//  const int lastWeekendDay  = bdlb::BitStringUtil::find1AtMaxIndex(weekends,
//                                                                   365 + 1);
//
//  assert(JAN +  5 == firstWeekendDay);
//  assert(DEC + 29 ==  lastWeekendDay);
//..
// Then, we define the following enumeration that allows us to easily represent
// the US holidays of the year:
//..
//  uint64_t holidays[6] = { 0 };
//
//  enum USHolidays2013 {
//      NEW_YEARS_DAY             = JAN +  1,
//      MARTIN_LUTHER_KING_JR_DAY = JAN + 21,
//      PRESIDENTS_DAY            = FEB + 18,
//      GOOD_FRIDAY               = MAR + 29,
//      MEMORIAL_DAY              = MAY + 27,
//      INDEPENDENCE_DAY          = JUL +  4,
//      LABOR_DAY                 = SEP +  2,
//      THANKSGIVING              = NOV + 28,
//      CHRISTMAS                 = DEC + 25
//  };
//
//  bdlb::BitStringUtil::assign(holidays, NEW_YEARS_DAY,             true);
//  bdlb::BitStringUtil::assign(holidays, MARTIN_LUTHER_KING_JR_DAY, true);
//  bdlb::BitStringUtil::assign(holidays, PRESIDENTS_DAY,            true);
//  bdlb::BitStringUtil::assign(holidays, GOOD_FRIDAY,               true);
//  bdlb::BitStringUtil::assign(holidays, MEMORIAL_DAY,              true);
//  bdlb::BitStringUtil::assign(holidays, INDEPENDENCE_DAY,          true);
//  bdlb::BitStringUtil::assign(holidays, LABOR_DAY,                 true);
//  bdlb::BitStringUtil::assign(holidays, THANKSGIVING,              true);
//  bdlb::BitStringUtil::assign(holidays, CHRISTMAS,                 true);
//..
// Next, the following enumeration indicates the beginning of fiscal quarters:
//..
//  enum {
//      Q1 = JAN + 1,
//      Q2 = APR + 1,
//      Q3 = JUN + 1,
//      Q4 = OCT + 1
//  };
//..
// Now, we can query our calendar for the first holiday in the third quarter,
// if any:
//..
//  const bsl::size_t firstHolidayOfQ3 = bdlb::BitStringUtil::find1AtMinIndex(
//                                                                    holidays,
//                                                                    Q3,
//                                                                    Q4);
//  assert(INDEPENDENCE_DAY == firstHolidayOfQ3);
//..
// Finally, our weekend and holiday calendars are readily combined to represent
// days off for either reason (i.e., holiday or weekend):
//..
//  uint64_t allDaysOff[6] = { 0 };
//  bdlb::BitStringUtil::orEqual(allDaysOff, 1, weekends, 1, 365);
//  bdlb::BitStringUtil::orEqual(allDaysOff, 1, holidays, 1, 365);
//
//  bool isOffMay24 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 24);
//  bool isOffMay25 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 25);
//  bool isOffMay26 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 26);
//  bool isOffMay27 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 27);
//  bool isOffMay28 = bdlb::BitStringUtil::bit(allDaysOff, MAY + 28);
//
//  assert(false == isOffMay24);
//  assert(true  == isOffMay25);    // Saturday
//  assert(true  == isOffMay26);    // Sunday
//  assert(true  == isOffMay27);    // Note May 27, 2013 is Memorial Day.
//  assert(false == isOffMay28);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_BITUTIL
#include <bdlb_bitutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlb {

                            // ====================
                            // struct BitStringUtil
                            // ====================

struct BitStringUtil {
    // This 'struct' provides a namespace for a suite of static functions to
    // manipulate and access sequences of bits stored in an array of 'uint64_t'
    // (also known as a "bit string"; see {The "Bit String" Pseudo-Type}).

    // PUBLIC TYPES
    enum { k_BITS_PER_UINT64 = 64 };  // number of bits in a 'uint64_t'

    // PUBLIC CLASS CONSTANTS
    static const bsl::size_t k_INVALID_INDEX = ~static_cast<bsl::size_t>(0);

    // CLASS METHODS

                                // Assign

    static void assign(bsl::uint64_t *bitString,
                       bsl::size_t    index,
                       bool           value);
        // Set the bit at the specified 'index' in the specified 'bitString' to
        // the specified 'value'.  The behavior is undefined unless 'index' is
        // less than the capacity of 'bitString'.

    static void assign(bsl::uint64_t *bitString,
                       bsl::size_t    index,
                       bool           value,
                       bsl::size_t    numBits);
        // Set the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitString' to the specified 'value'.  The behavior is
        // undefined unless 'bitString' has a capacity of at least
        // 'index + numBits'.

    static void assign0(bsl::uint64_t *bitString, bsl::size_t index);
        // Set the bit at the specified 'index' in the specified 'bitString' to
        // 'false'.  The behavior is undefined unless 'index' is less than the
        // capacity of 'bitString'.

    static void assign0(bsl::uint64_t *bitString,
                        bsl::size_t    index,
                        bsl::size_t    numBits);
        // Set the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitString' to 'false'.  The behavior is undefined
        // unless 'bitString' has a capacity of at least 'index + numBits'.

    static void assign1(bsl::uint64_t *bitString, bsl::size_t index);
        // Set the bit at the specified 'index' in the specified 'bitString' to
        // 'true'.  The behavior is undefined unless 'index' is less than the
        // capacity of 'bitString'.

    static void assign1(bsl::uint64_t *bitString,
                        bsl::size_t    index,
                        bsl::size_t    numBits);
        // Set the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitString' to 'true'.  The behavior is undefined
        // unless 'bitString' has a capacity of at least 'index + numBits'.

    static void assignBits(bsl::uint64_t *bitString,
                           bsl::size_t    index,
                           bsl::uint64_t  srcValue,
                           bsl::size_t    numBits);
        // Assign the low-order specified 'numBits' from the specified
        // 'srcValue' to the 'numBits' starting at the specified 'index' in the
        // specified 'bitString'.  The behavior is undefined unless
        // 'numBits <= k_BITS_PER_UINT64' and 'bitString' has a capacity of at
        // least 'index + numBits'.

                                // Bitwise-Logical

    static void andEqual(bsl::uint64_t       *dstBitString,
                         bsl::size_t          dstIndex,
                         const bsl::uint64_t *srcBitString,
                         bsl::size_t          srcIndex,
                         bsl::size_t          numBits);
        // Bitwise AND the specified 'numBits' of the specified 'dstBitString'
        // starting at the specified 'dstIndex' with the 'numBits' of the
        // specified 'srcBitString' starting at the specified 'srcIndex', and
        // write the result over the bits that were read from 'dstBitString'.
        // The behavior is undefined unless 'dstBitString' has a length of at
        // least 'dstIndex + numBits' and 'srcBitString' has a length of at
        // least 'srcIndex + numBits'.

    static void minusEqual(bsl::uint64_t       *dstBitString,
                           bsl::size_t          dstIndex,
                           const bsl::uint64_t *srcBitString,
                           bsl::size_t          srcIndex,
                           bsl::size_t          numBits);
        // Bitwise MINUS the specified 'numBits' of the specified
        // 'srcBitString' starting at the specified 'srcIndex' from the
        // 'numBits' of the specified 'dstBitString' starting at the specified
        // 'dstIndex', and write the result over the bits that were read from
        // 'dstBitString'.  The behavior is undefined unless 'dstBitString' has
        // a length of at least 'dstIndex + numBits' and 'srcBitString' has a
        // length of at least 'srcIndex + numBits'.  Note that the logical
        // difference 'A - B' is defined to be 'A & !B'.

    static void orEqual(bsl::uint64_t       *dstBitString,
                        bsl::size_t          dstIndex,
                        const bsl::uint64_t *srcBitString,
                        bsl::size_t          srcIndex,
                        bsl::size_t          numBits);
        // Bitwise OR the specified 'numBits' of the specified 'dstBitString'
        // starting at the specified 'dstIndex' with the 'numBits' of the
        // specified 'srcBitString' starting at the specified 'srcIndex', and
        // write the result over the bits that were read from 'dstBitString'.
        // The behavior is undefined unless 'dstBitString' has a length of at
        // least 'dstIndex + numBits' and 'srcBitString' has a length of at
        // least 'srcIndex + numBits'.

    static void xorEqual(bsl::uint64_t       *dstBitString,
                         bsl::size_t          dstIndex,
                         const bsl::uint64_t *srcBitString,
                         bsl::size_t          srcIndex,
                         bsl::size_t          numBits);
        // Bitwise XOR the specified 'numBits' of the specified 'dstBitString'
        // starting at the specified 'dstIndex' with the 'numBits' of the
        // specified 'srcBitString' starting at the specified 'srcIndex', and
        // write the result over the bits that were read from 'dstBitString'.
        // The behavior is undefined unless 'dstBitString' has a length of at
        // least 'dstIndex + numBits' and 'srcBitString' has a length of at
        // least 'srcIndex + numBits'.

                                // Copy

    static void copy(bsl::uint64_t       *dstBitString,
                     bsl::size_t          dstIndex,
                     const bsl::uint64_t *srcBitString,
                     bsl::size_t          srcIndex,
                     bsl::size_t          numBits);
        // Copy to the specified 'dstBitString', beginning at the specified
        // 'dstIndex', the specified 'numBits' beginning at the specified
        // 'srcIndex' in the specified 'srcBitString'.  This function works
        // correctly regardless of whether the source and destination ranges
        // overlap.  The behavior is undefined unless 'dstBitString' has a
        // capacity of at least 'dstIndex + numBits' and 'srcBitString' has a
        // length of at least 'srcIndex + numBits'.

    static void copyRaw(bsl::uint64_t       *dstBitString,
                        bsl::size_t          dstIndex,
                        const bsl::uint64_t *srcBitString,
                        bsl::size_t          srcIndex,
                        bsl::size_t          numBits);
        // Copy to the specified 'dstBitString', beginning at the specified
        // 'dstIndex', the specified 'numBits' beginning at the specified
        // 'srcIndex' in the specified 'srcBitString'.  The behavior is
        // undefined unless 'dstBitString' has a capacity of at least
        // 'dstIndex + numBits', 'srcBitString' has a length of at least
        // 'srcIndex + numBits', and the source and destination ranges either
        // do not overlap, or the destination range is equal to the source
        // range, or the start of the destination range is below the start of
        // the source range.

                                // Insert / Remove

    static void insert(bsl::uint64_t *bitString,
                       bsl::size_t    initialLength,
                       bsl::size_t    dstIndex,
                       bool           value,
                       bsl::size_t    numBits);
        // Insert the specified 'numBits', each having the specified 'value',
        // into the specified 'bitString' having the specified 'initialLength',
        // beginning at the specified 'dstIndex'.  Bits at or above 'dstIndex'
        // are shifted up by 'numBits' index positions and the length of
        // 'bitString' is increased by 'numBits'.  The behavior is undefined
        // unless 'dstIndex <= initialLength' and 'bitString' has a capacity of
        // at least 'initialLength + numBits'.

    static void insert0(bsl::uint64_t *bitString,
                        bsl::size_t    initialLength,
                        bsl::size_t    dstIndex,
                        bsl::size_t    numBits);
        // Insert the specified 'numBits' 0 bits into the specified 'bitString'
        // having the specified 'initialLength' beginning at the specified
        // 'dstIndex'.  Bits at or above 'dstIndex' are shifted up by 'numBits'
        // index positions and the length of 'bitString' is increased by
        // 'numBits'.  The behavior is undefined unless
        // 'dstIndex <= initialLength' and 'bitString' has a capacity of at
        // least 'initialLength + numBits'.

    static void insert1(bsl::uint64_t *bitString,
                        bsl::size_t    initialLength,
                        bsl::size_t    dstIndex,
                        bsl::size_t    numBits);
        // Insert the specified 'numBits' 1 bits into the specified 'bitString'
        // having the specified 'initialLength' beginning at the specified
        // 'dstIndex'.  Bits at or above 'dstIndex' are shifted up by 'numBits'
        // index positions and the length of 'bitString' is increased by
        // 'numBits'.  The behavior is undefined unless
        // 'dstIndex <= initialLength' and 'bitString' has a capacity of at
        // least 'initialLength + numBits'.

    static void insertRaw(bsl::uint64_t *bitString,
                          bsl::size_t    initialLength,
                          bsl::size_t    dstIndex,
                          bsl::size_t    numBits);
        // Insert the specified 'numBits' into the specified 'bitString' having
        // the specified 'initialLength' beginning at the specified 'dstIndex'.
        // Bits at or above 'dstIndex' are shifted up by 'numBits' index
        // positions and the length of 'bitString' is increased by 'numBits'.
        // The values of the inserted bits are undefined.  The behavior is
        // undefined unless 'dstIndex <= initialLength' and 'bitString' has a
        // capacity of at least 'initialLength + numBits'.  Note that the
        // inserted bits are not assigned any value.

    static void remove(bsl::uint64_t *bitString,
                       bsl::size_t    length,
                       bsl::size_t    index,
                       bsl::size_t    numBits);
        // Remove the specified 'numBits' from the specified 'bitString' of the
        // specified 'length' beginning at the specified 'index'.  Bits above
        // 'index + numBits' are shifted down by 'numBits' index positions and
        // the length of 'bitString' is reduced by 'numBits'.  The values of
        // the vacated high-order bits are not modified.  The behavior is
        // undefined unless 'index + numBits <= length'.

    static void removeAndFill0(bsl::uint64_t *bitString,
                               bsl::size_t    length,
                               bsl::size_t    index,
                               bsl::size_t    numBits);
        // Remove the specified 'numBits' from the specified 'bitString' having
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the last 'numBits' of 'bitString' are set to 0.  The
        // length of 'bitString' is not changed.  The behavior is undefined
        // unless 'index + numBits <= length'.

    static void removeAndFill1(bsl::uint64_t *bitString,
                               bsl::size_t    length,
                               bsl::size_t    index,
                               bsl::size_t    numBits);
        // Remove the specified 'numBits' from the specified 'bitString' having
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the last 'numBits' of 'bitString' are set to 1.  The
        // length of 'bitString' is not changed.  The behavior is undefined
        // unless 'index + numBits <= length'.

                                // Other Manipulators

    static void swapRaw(bsl::uint64_t *bitString1,
                        bsl::size_t    index1,
                        bsl::uint64_t *bitString2,
                        bsl::size_t    index2,
                        bsl::size_t    numBits);
        // Exchange the specified 'numBits' beginning at the specified 'index1'
        // in the specified 'bitString1' with the 'numBits' beginning at the
        // specified 'index2' in the specified 'bitString2'.  The behavior is
        // undefined unless 'bitString1' has a length of at least
        // 'index1 + numBits', 'bitString2' has a length of at least
        // 'index2 + numBits', and there is *no* overlap between the swapped
        // ranges of bits.

    static void toggle(bsl::uint64_t *bitString,
                       bsl::size_t    index,
                       bsl::size_t    numBits);
        // Invert the values of the specified 'numBits' in the specified
        // 'bitString' beginning at the specified 'index'.  The behavior is
        // undefined unless 'bitString' has a length of at least
        // 'index + numBits'.

                                // Compare

    static bool areEqual(const bsl::uint64_t *bitString1,
                         const bsl::uint64_t *bitString2,
                         bsl::size_t          numBits);
        // Return 'true' if the specified low-order 'numBits' in the specified
        // 'bitString1' are bitwise equal to the corresponding bits in the
        // specified 'bitString2', and 'false' otherwise.  The behavior is
        // undefined unless both 'bitString1' and 'bitString2' have a length of
        // at least 'numBits'.

    static bool areEqual(const bsl::uint64_t *bitString1,
                         bsl::size_t          index1,
                         const bsl::uint64_t *bitString2,
                         bsl::size_t          index2,
                         bsl::size_t          numBits);
        // Return 'true' if the specified 'numBits' beginning at the specified
        // 'index1' in the specified 'bitString1' are bitwise equal to the
        // 'numBits' beginning at the specified 'index2' in the specified
        // 'bitString2', and 'false' otherwise.  The behavior is undefined
        // unless 'bitString1' has a length of at least 'index1 + numBits' and
        // 'bitString2' has a length of at least 'index2 + numBits'.

                                // Read

    static bool bit(const bsl::uint64_t *bitString, bsl::size_t index);
        // Return the bit value at the specified 'index' in the specified
        // 'bitString'.  The behavior is undefined unless 'index' is less than
        // the length of 'bitString'.

    static bsl::uint64_t bits(const bsl::uint64_t *bitString,
                              bsl::size_t          index,
                              bsl::size_t          numBits);
        // Return the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitString' as the low-order bits of the returned
        // value.  The behavior is undefined unless
        // 'numBits <= k_BITS_PER_UINT64' and 'bitString' has a length of at
        // least 'index + numBits'.

                                // Find

    static bsl::size_t find0AtMaxIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          length);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitString' having the specified 'length', if such a bit exists, and
        // 'k_INVALID_INDEX' otherwise.

    static bsl::size_t find0AtMaxIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          begin,
                                       bsl::size_t          end);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitString' in the specified range '[begin .. end)', if such a bit
        // exists, and 'k_INVALID_INDEX' otherwise.  The behavior is undefined
        // unless 'begin <= end' and 'end' is less than or equal to the length
        // of 'bitString'.

    static bsl::size_t find0AtMinIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          length);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitString' having the specified 'length', if such a bit exists, and
        // 'k_INVALID_INDEX' otherwise.

    static bsl::size_t find0AtMinIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          begin,
                                       bsl::size_t          end);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitString' in the specified range '[begin .. end)', if such a bit
        // exists, and 'k_INVALID_INDEX' otherwise.  The behavior is undefined
        // unless 'begin <= end' and 'end' is less than or equal to the length
        // of 'bitString'.

    static bsl::size_t find1AtMaxIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          length);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitString' having the specified 'length', if such a bit exists, and
        // 'k_INVALID_INDEX' otherwise.

    static bsl::size_t find1AtMaxIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          begin,
                                       bsl::size_t          end);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitString' in the specified range '[begin .. end)', if such a bit
        // exists, and 'k_INVALID_INDEX' otherwise.  The behavior is undefined
        // unless 'begin <= end' and 'end' is less than or equal to the length
        // of 'bitString'.

    static bsl::size_t find1AtMinIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          length);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitString' having the specified 'length', if such a bit exists, and
        // 'k_INVALID_INDEX' otherwise.

    static bsl::size_t find1AtMinIndex(const bsl::uint64_t *bitString,
                                       bsl::size_t          begin,
                                       bsl::size_t          end);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitString' in the specified range '[begin .. end)', if such a bit
        // exists, and 'k_INVALID_INDEX' otherwise.  The behavior is undefined
        // unless 'begin <= end' and 'end' is less than or equal to the length
        // of 'bitString'.

                                // Count

    static bool isAny0(const bsl::uint64_t *bitString,
                       bsl::size_t          index,
                       bsl::size_t          numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitString' are 0, and 'false'
        // otherwise.  The behavior is undefined unless 'bitString' has a
        // length of at least 'index + numBits'.

    static bool isAny1(const bsl::uint64_t *bitString,
                       bsl::size_t          index,
                       bsl::size_t          numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitString' are 1, and 'false'
        // otherwise.  The behavior is undefined unless 'bitString' has a
        // length of at least 'index + numBits'.

    static bsl::size_t num0(const bsl::uint64_t *bitString,
                            bsl::size_t          index,
                            bsl::size_t          numBits);
        // Return the number of 0 bits in the specified 'numBits' beginning at
        // the specified 'index' in the specified 'bitString'.  The behavior is
        // undefined unless 'bitString' has a length of at least
        // 'index + numBits'.

    static bsl::size_t num1(const bsl::uint64_t *bitString,
                            bsl::size_t          index,
                            bsl::size_t          numBits);
        // Return the number of 1 bits in the specified 'numBits' beginning at
        // the specified 'index' in the specified 'bitString'.  The behavior is
        // undefined unless 'bitString' has a length of at least
        // 'index + numBits'.

                                // Printing

    static bsl::ostream& print(bsl::ostream&        stream,
                               const bsl::uint64_t *bitString,
                               bsl::size_t          numBits,
                               int                  level          = 1,
                               int                  spacesPerLevel = 4);
        // Format to the specified output 'stream' the specified low-order
        // 'numBits' in the specified 'bitString' in hexadecimal, and return a
        // reference to 'stream'.  The highest order bits are printed first, in
        // groups of 16 nibbles, 64 nibbles per line (in the case of multi-line
        // output).  Optionally specify 'level', the indentation level for each
        // line output.  Optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level.  Each line is indented by the absolute
        // value of 'level * spacesPerLevel'.  If 'spacesPerLevel' is negative,
        // suppress line breaks and format the entire output on one line.  If
        // 'stream' is initially invalid, this operation has no effect.  Note
        // that a trailing newline is provided in multiline mode only.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct BitStringUtil
                            // --------------------

// CLASS METHODS

                            // Manipulators

                                // Assign

inline
void BitStringUtil::assign(bsl::uint64_t *bitString,
                           bsl::size_t    index,
                           bool           value)
{
    BSLS_ASSERT_SAFE(bitString);

    const bsl::size_t idx =                       index  / k_BITS_PER_UINT64;
    const int         pos = static_cast<unsigned>(index) % k_BITS_PER_UINT64;

    if (value) {
        bitString[idx] |=  (1ULL << pos);
    }
    else {
        bitString[idx] &= ~(1ULL << pos);
    }
}

inline
void BitStringUtil::assign0(bsl::uint64_t *bitString, bsl::size_t index)
{
    BSLS_ASSERT_SAFE(bitString);

    const bsl::size_t idx =                       index  / k_BITS_PER_UINT64;
    const int         pos = static_cast<unsigned>(index) % k_BITS_PER_UINT64;

    bitString[idx] &= ~(1ULL << pos);
}

inline
void BitStringUtil::assign1(bsl::uint64_t *bitString, bsl::size_t index)
{
    BSLS_ASSERT_SAFE(bitString);

    const bsl::size_t idx =                       index  / k_BITS_PER_UINT64;
    const int         pos = static_cast<unsigned>(index) % k_BITS_PER_UINT64;

    bitString[idx] |= 1ULL << pos;
}

                                // Insert / Remove

inline
void BitStringUtil::insert(bsl::uint64_t *bitString,
                           bsl::size_t    initialLength,
                           bsl::size_t    dstIndex,
                           bool           value,
                           bsl::size_t    numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    insertRaw(bitString, initialLength, dstIndex, numBits);
    assign(bitString, dstIndex, value, numBits);
}

inline
void BitStringUtil::insert0(bsl::uint64_t *bitString,
                            bsl::size_t    initialLength,
                            bsl::size_t    dstIndex,
                            bsl::size_t    numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    insertRaw(bitString, initialLength, dstIndex, numBits);
    assign0(bitString, dstIndex, numBits);
}

inline
void BitStringUtil::insert1(bsl::uint64_t *bitString,
                            bsl::size_t    initialLength,
                            bsl::size_t    dstIndex,
                            bsl::size_t    numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    insertRaw(bitString, initialLength, dstIndex, numBits);
    assign1(bitString, dstIndex, numBits);
}

inline
void BitStringUtil::removeAndFill0(bsl::uint64_t *bitString,
                                   bsl::size_t    length,
                                   bsl::size_t    index,
                                   bsl::size_t    numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    remove(bitString, length, index, numBits);
    assign0(bitString, length - numBits, numBits);
}

inline
void BitStringUtil::removeAndFill1(bsl::uint64_t *bitString,
                                   bsl::size_t    length,
                                   bsl::size_t    index,
                                   bsl::size_t    numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    remove(bitString, length, index, numBits);
    assign1(bitString, length - numBits, numBits);
}

                                // Accessors

                                // Read
inline
bool BitStringUtil::bit(const bsl::uint64_t *bitString, bsl::size_t index)
{
    BSLS_ASSERT_SAFE(bitString);

    const bsl::size_t idx =                       index  / k_BITS_PER_UINT64;
    const int         pos = static_cast<unsigned>(index) % k_BITS_PER_UINT64;

    return bitString[idx] & (1ULL << pos);
}

                                // Count

inline
bsl::size_t BitStringUtil::num0(const bsl::uint64_t *bitString,
                                bsl::size_t          index,
                                bsl::size_t          numBits)
{
    BSLS_ASSERT_SAFE(bitString);

    return numBits - num1(bitString, index, numBits);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
