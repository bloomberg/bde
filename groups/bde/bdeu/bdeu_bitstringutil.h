// bdeu_bitstringutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDEU_BITSTRINGUTIL
#define INCLUDED_BDEU_BITSTRINGUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide efficient operations on a sequence of bits.
//
//@CLASSES:
// bdeu_BitstringUtil: namespace for common bit-manipulation procedures
//
//@SEE_ALSO: bdes_bitutil, bdea_bitarray
//
//@AUTHOR: Anthony Comerico (acomeric), Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdeu_BitstringUtil', that serves as a namespace for a collection of
// efficient, bit-level procedures on a sequence of bits represented by an
// array of integer values.
//
///The 'Bitstring', a Pseudo-Type
///------------------------------
// A contiguous sequence of bits that occupy one or more contiguous 'int'
// values can be viewed as a string of bits.  This component supports
// operations on such sequences.  The notion of 'Bitstring', an "informal
// type", is used to document those operations.  Correspondingly,
// 'Bitstring' operations are categorized as either "accessors", operations
// that return information and guarantee no change; and "manipulators",
// operations that modify the 'Bitstring'.
//
// A 'Bitstring' has two "structural" attributes:
//..
//  Capacity - The maximum number of bits that can be represented by a given
//             'Bitstring'.
//
//  Length   - The number of bits currently in use by a 'Bitstring'.
//..
// Since this is a pseudo-type, there is no language support for managing these
// values; the user must do so explicitly.
//
// Many operations on 'Bitstring' refer to a "position" within a 'Bitstring';
// or a range of positions within a 'Bitstring':
//..
//  Position - The offset (in bits) of a bit value from the beginning of a
//             'Bitstring' (also called the "index" of a bit).
//..
// The notion of "position" used in this component is a generalization of the
// notion of a bit's position in a single integer value.
//
// Bits within a 32-bit integer (irrespective of the endian-ness of a platform)
// are here numbered, starting at 0, from the least-significant bit to the
// most-significant bit.  In illustrations, we typically show the high-order
// bits on the left:
//..
//   31 30  . . . . .   5  4  3  2  1  0
//  +-----------------------------------+
//  | 1| 0| . . . . . | 1| 1| 0| 0| 1| 0|
//  +-----------------------------------+
//..
// Thus, "left"-shifting (i.e., using the '<<' operator) causes low-order bits
// to move up in bit-position (to positions of higher significance) and
// "right"-shifting (i.e., using the '>>' operator) causes higher-order bits to
// move into positions of less significance.
//
// This component extends this representation to an extensible sequence of bits
// implemented using an array of integers.  For example, the 'Bitstring' shown
// below is built on an array of three 'int' values.  Thus, it has a capacity
// of 96 (i.e., '3 * sizeof(int) * CHAR_BIT').  Values have been set in
// positions 0 to 90, inclusive; thus it has a length of 91.  The 'X' values
// indicate bit values in the 5 unused bit positions.  If needed, up to 5 bit
// values could be validly appended to this 'Bitstring':
//..
//  |<----- int 0 ----->|<----- int 1 --->|<--------- int 2 --------->
//   31 30 29  . . .   0 63 62  . . .  32 95  . . .  91 90  . . .  64
//  +-------------------+----------------+---------------------------+
//  | 0| 1| 0| . . . | 1| 0| 1| . . . | 0| X| . . . | X| 1| . . . | 1|
//  +-------------------+----------------+---------------------------+
//..
// When using such representations, remember that "left"-shifting may move
// values from the most-significant bit of one integer right (relative to the
// page) to the least-significant bit of the next integer.  For example, use of
// the '<<' operator on the 'Bitstring' shown above moves the value in bit 31
// of integer 0 to bit 0 of integer 1.
//
///Function Categories
///-------------------
// The functions implemented in this utility component can be broadly
// categorized as "accessors" (i.e., those that return a value but do not
// modify any argument), and "manipulators" (i.e., those that return 'void',
// but take the address of an integer as the first argument in order to modify
// it in place).
//
// Most accessor methods refer to a single range of bits within a single
// 'Bitstring'.  Note that 'get' is in this category even though the width of
// its 'Bitstring' is always 1.  The accessors include:
//..
//  areEqual
//  isAny0
//  isAny1
//  find0AtLargestIndex
//  find0AtLargestIndexGE
//  find0AtLargestIndexGT
//  find0AtLargestIndexLE
//  find0AtLargestIndexLT
//  find0AtSmallestIndex
//  find0AtSmallestIndexGE
//  find0AtSmallestIndexGT
//  find0AtSmallestIndexLE
//  find0AtSmallestIndexLT
//  find1AtLargestIndex
//  find1AtLargestIndexGE
//  find1AtLargestIndexGT
//  find1AtLargestIndexLE
//  find1AtLargestIndexLT
//  find1AtSmallestIndex
//  find1AtSmallestIndexGE
//  find1AtSmallestIndexGT
//  find1AtSmallestIndexLE
//  find1AtSmallestIndexLT
//  get
//  num0
//  num1
//..
// Manipulators include:
//..
//  andEqual
//  append
//  append0
//  append1
//  copy
//  copyRaw
//  insert
//  insert0
//  insert1
//  insertRaw
//  minusEqual
//  orEqual
//  remove
//  removeAndFill
//  removeAndFill0
//  removeAndFill1
//  set
//  swapRaw
//  toggle
//  xorEqual
//..
// Additionally, the 'print' method formats a 'Bitstring' to an output stream.
//
///Usage
///-----
// A 'Bitstring' can be used to efficiently represent a business calendar and
// provide operations on such a calendar.  First, create a 'Bitstring' having
// sufficient capacity to represent every day of a given year (i.e.,
// '384 == 32 * 12'), and set a 1 bit at each index corresponding to the
// day-of-year (DOY) for each weekend day.  For convenience in date
// calculations, the 0 index is not used; the 365 days of a (non-leap) year are
// at indices '[ 1 .. 365 ]'.  Further note that the values set below all
// correspond to the year 2010:
//..
//  int weekends[12] = { 0 };
//
//  for (int i = 2 /* first Sat */; i < 366; i += 7) {
//      bdeu_BitstringUtil::set(weekends, i, 1);
//      bdeu_BitstringUtil::set(weekends, i + 1, 1);
//  }
//..
// Now, we can easily use 'bdeu_BitstringUtil' methods to find days of
// interest.  For example:
//..
//  int firstWeekendDay = bdeu_BitstringUtil::find1AtSmallestIndex(weekends,
//                                                                 365 + 1);
//  int lastWeekendDay  = bdeu_BitstringUtil::find1AtLargestIndex(weekends,
//                                                                365 + 1);
//..
// We can define an enumeration to assist us in representing these DOY values
// into conventional dates and confirm the calculated values:
//..
//  enum {
//      JAN = 0,  // Note: First DOY is 'JAN + 1'.
//      FEB = JAN + 31,
//      MAR = FEB + 28,
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
//
//  assert(JAN +  2 == firstWeekendDay);
//  assert(DEC + 26 ==  lastWeekendDay);
//..
// The enumeration allows us to easily represent the business holidays of the
// year and significant dates in the business calendar:
//..
//  int holidays[12] = { 0 };
//
//  enum {
//      NEW_YEARS_DAY    = JAN +  1,
//      MLK_DAY          = JAN + 18,
//      PRESIDENTS_DAY   = FEB + 15,
//      GOOD_FRIDAY      = APR +  2,
//      MEMORIAL_DAY     = MAY + 31,
//      INDEPENDENCE_DAY = JUL +  5,
//      LABOR_DAY        = SEP +  6,
//      THANKSGIVING     = NOV + 25,
//      CHRISTMAS        = DEC + 24
//  };
//
//  bdeu_BitstringUtil::set(holidays, NEW_YEARS_DAY,    1);
//  bdeu_BitstringUtil::set(holidays, MLK_DAY,          1);
//  bdeu_BitstringUtil::set(holidays, PRESIDENTS_DAY,   1);
//  bdeu_BitstringUtil::set(holidays, GOOD_FRIDAY,      1);
//  bdeu_BitstringUtil::set(holidays, MEMORIAL_DAY,     1);
//  bdeu_BitstringUtil::set(holidays, INDEPENDENCE_DAY, 1);
//  bdeu_BitstringUtil::set(holidays, LABOR_DAY,        1);
//  bdeu_BitstringUtil::set(holidays, THANKSGIVING,     1);
//  bdeu_BitstringUtil::set(holidays, CHRISTMAS,        1);
//
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
//  int firstHolidayOfQ3 = bdeu_BitstringUtil::find1AtSmallestIndexGT(holidays,
//                                                                    Q4,
//                                                                    Q3);
//  assert(INDEPENDENCE_DAY == firstHolidayOfQ3);
//..
// Our calendars are readily combined to represent days off for either reason
// (i.e., holiday or weekend):
//..
//  int allDaysOff[12] = { 0 };
//  bdeu_BitstringUtil::orEqual(allDaysOff, 1, weekends, 1, 365);
//  bdeu_BitstringUtil::orEqual(allDaysOff, 1, holidays, 1, 365);
//
//  int isOffMay07 = bdeu_BitstringUtil::get(allDaysOff, MAY +  7);
//  int isOffMay08 = bdeu_BitstringUtil::get(allDaysOff, MAY +  8);
//  int isOffMay09 = bdeu_BitstringUtil::get(allDaysOff, MAY +  9);
//  int isOffMay10 = bdeu_BitstringUtil::get(allDaysOff, MAY + 10);
//  int isOffXmas  = bdeu_BitstringUtil::get(allDaysOff, CHRISTMAS);
//
//  assert(0 == isOffMay07);
//  assert(1 == isOffMay08);
//  assert(1 == isOffMay09);
//  assert(0 == isOffMay10);
//  assert(1 == isOffXmas);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // =========================
                        // struct bdeu_BitstringUtil
                        // =========================

struct bdeu_BitstringUtil {
    // This 'struct' provides a namespace for a suite of pure procedures to
    // manipulate sequences of bits represented as an array of 'int' (also
    // known as a 'Bitstring').

    // CLASS METHODS

                             // Manipulators

    static void andEqual(int       *dstBitstring,
                         int        dstIndex,
                         const int *srcBitstring,
                         int        srcIndex,
                         int        numBits);
        // Bitwise AND the specified 'numBits' in the specified 'srcBitstring',
        // beginning at the specified 'srcIndex', to those in the specified
        // 'dstBitstring', beginning at the specified 'dstIndex'.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstBitstring' has a length of at least
        // 'dstIndex + numBits', and 'srcBitstring' has a length of at least
        // 'srcIndex + numBits'.

    static void append(int  *bitstring,
                       int   length,
                       bool  value,
                       int   numBits);
        // Append to the specified 'bitstring' having the specified 'length'
        // the specified 'numBits' having the specified 'value'.  The behavior
        // is undefined unless '0 <= length', '0 <= numBits', and 'bitstring'
        // has a capacity of at least 'length + numBits'.  Note that this
        // function has the same behavior as:
        //..
        //  insert(bitstring, length, value, numBits);
        //..

    static void append0(int *bitstring, int length, int numBits);
        // Append to the specified 'bitstring' having the specified 'length,
        // the specified 'numBits' 0 bits.  The behavior is undefined unless
        // '0 <= length', '0 <= numBits', and 'bitstring' has a capacity of at
        // least 'length + numBits'.  Note that this function is logically
        // equivalent to:
        //..
        //  insert0(bitstring, length, numBits);
        //..

    static void append1(int *bitstring, int length, int numBits);
        // Append to the specified 'bitstring' having the specified 'length,
        // the specified 'numBits' 1 bits.  The behavior is undefined unless
        // '0 <= length', '0 <= numBits', and 'bitstring' has a capacity of at
        // least 'length + numBits'.  Note that this function is logically
        // equivalent to:
        //..
        //  insert1(bitstring, length, numBits);
        //..

    static void copy(int       *dstBitstring,
                     int        dstIndex,
                     const int *srcBitstring,
                     int        srcIndex,
                     int        numBits);
        // Copy to the specified 'dstBitstring', beginning at the specified
        // 'dstIndex', from the specified 'srcBitstring', beginning at the
        // specified 'srcIndex', the specified 'numBits'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', 'dstBitstring'
        // has a capacity of at least 'dstIndex + numBits', and 'srcBitstring'
        // has a capacity of at least 'srcIndex + numBits'.  Note that even if
        // there is an overlap between the source and destination ranges the
        // resulting bits in the destination range will match the bits
        // originally in the source range (as if the bits had been copied into
        // a temporary buffer and then copied into the destination range).  See
        // 'copyRaw'.

    static void copyRaw(int       *dstBitstring,
                        int        dstIndex,
                        const int *srcBitstring,
                        int        srcIndex,
                        int        numBits);
        // Copy to the specified 'dstBitstring', beginning at the specified
        // 'dstIndex', from the specified 'srcBitstring', beginning at the
        // specified 'srcIndex', the specified 'numBits'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', 'dstBitstring'
        // has a capacity of at least 'dstIndex + numBits', and 'srcBitstring'
        // has a capacity of at least 'srcIndex + numBits'.  Note that if there
        // is an overlap between the source and destination ranges the
        // resulting bits in the destination range may not match the bits
        // originally in the source range.  See 'copy'.

    static void insert(int  *bitstring,
                       int   length,
                       int   dstIndex,
                       bool  value,
                       int   numBits);
        // Insert the specified 'numBits', each having the specified 'value',
        // into the specified 'bitstring' having the specified 'length',
        // beginning at the specified 'dstIndex'.  Bits at or above 'dstIndex'
        // are shifted up by 'numBits' index positions.  The behavior is
        // undefined unless '0 <= numBits', '0 <= dstIndex <= length', and
        // 'bitstring' has a capacity of at least 'length + numBits'.

    static void insert0(int *bitstring,
                        int  length,
                        int  dstIndex,
                        int  numBits);
        // Insert the specified 'numBits' 0 bits into the specified 'bitstring'
        // having the specified 'length' beginning at the specified 'dstIndex'.
        // Bits at or above 'dstIndex' are shifted up by 'numBits' index
        // positions.  The behavior is undefined unless '0 <= numBits',
        // '0 <= dstIndex <= length', and 'bitstring' has a capacity of at
        // least 'length + numBits'.

    static void insert1(int *bitstring,
                        int  length,
                        int  dstIndex,
                        int  numBits);
        // Insert the specified 'numBits' 1 bits into the specified 'bitstring'
        // having the specified 'length' beginning at the specified 'dstIndex'.
        // Bits at or above 'dstIndex' are shifted up by 'numBits' index
        // positions.  The behavior is undefined unless '0 <= numBits',
        // '0 <= dstIndex <= length', and 'bitstring' has a capacity of at
        // least 'length + numBits'.

    static void insertRaw(int *bitstring,
                          int  length,
                          int  dstIndex,
                          int  numBits);
        // Insert the specified 'numBits' into the specified 'bitstring' having
        // the specified 'length' beginning at the specified 'dstIndex'.  Bits
        // at or above 'dstIndex' are shifted up by 'numBits' index positions.
        // The behavior is undefined unless '0 <= numBits',
        // '0 <= dstIndex <= length', and 'bitstring' has a capacity of at
        // least 'length + numBits'.  Note that the inserted bits are not
        // assigned any value.

    static void minusEqual(int       *dstBitstring,
                           int        dstIndex,
                           const int *srcBitstring,
                           int        srcIndex,
                           int        numBits);
        // Bitwise MINUS the specified 'numBits' in the specified
        // 'srcBitstring', beginning at the specified 'srcIndex', to those in
        // the specified 'dstBitstring', beginning at the specified 'dstIndex'.
        // The behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstBitstring' has a length of at least
        // 'dstIndex + numBits', and 'srcBitstring' has a length of at least
        // 'srcIndex + numBits'.  Note that the logical difference 'A - B' is
        // defined to be 'A & !B'.

    static void orEqual(int       *dstBitstring,
                        int        dstIndex,
                        const int *srcBitstring,
                        int        srcIndex,
                        int        numBits);
        // Bitwise OR the specified 'numBits' in the specified 'srcBitstring',
        // beginning at the specified 'srcIndex', to those in the specified
        // 'dstBitstring', beginning at the specified 'dstIndex'.  The behavior
        // is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstBitstring' has a length of at least
        // 'dstIndex + numBits', and 'srcBitstring' has a length of at least
        // 'srcIndex + numBits'.

    static void remove(int *bitstring,
                       int  length,
                       int  index,
                       int  numBits);
        // Remove the specified 'numBits' from the specified 'bitstring' of
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the length of 'bitstring' is reduced by 'numBits'.
        // The behavior is undefined unless '0 <= length', '0 <= index',
        // '0 <= numBits', and 'index + numBits <= length'.  Note that
        // the value of the removed high-order bits is not modified.

    static void removeAndFill(int  *bitstring,
                              int   length,
                              int   index,
                              bool  value,
                              int   numBits);
        // Remove the specified 'numBits' from the specified 'bitstring' having
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the last 'numBits' of 'bitstring' are set to the
        // specified 'value'.  The behavior is undefined unless '0 <= length',
        // '0 <= index', '0 <= numBits', and 'index + numBits <= length'.  Note
        // that the length of 'bitstring' is not changed.

    static void removeAndFill0(int *bitstring,
                               int  length,
                               int  index,
                               int  numBits);
        // Remove the specified 'numBits' from the specified 'bitstring' having
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the last 'numBits' of 'bitstring' are set to 0.  The
        // behavior is undefined unless '0 <= length', '0 <= index',
        // '0 <= numBits', and 'index + numBits <= length'.  Note that the
        // length of 'bitstring' is not changed.

    static void removeAndFill1(int *bitstring,
                               int  length,
                               int  index,
                               int  numBits);
        // Remove the specified 'numBits' from the specified 'bitstring' having
        // the specified 'length' beginning at the specified 'index'.  Bits
        // above 'index + numBits' are shifted down by 'numBits' index
        // positions and the last 'numBits' of 'bitstring' are set to 1.  The
        // behavior is undefined unless '0 <= length', '0 <= index',
        // '0 <= numBits', and 'index + numBits <= length'.  Note that the
        // length of 'bitstring' is not changed.

    static void set(int *bitstring, int index, bool value);
        // Set the bit at the specified 'index' in the specified 'bitstring' to
        // the specified 'value'.  The behavior is undefined unless
        // '0 <= index' and 'index' is less than the length of 'bitstring'.

    static void set(int *bitstring, int index, bool value, int numBits);
        // Set the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitstring' to the specified 'value'.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and 'bitstring' has a
        // capacity of at least 'index + numBits'.

    static void swapRaw(int *bitstring1,
                        int  index1,
                        int *bitstring2,
                        int  index2,
                        int  numBits);
        // Exchange the specified 'numBits' beginning at the specified 'index1'
        // in the specified 'bitstring1' with the 'numBits' beginning at the
        // specified 'index2' in the specified 'bitstring2'.  The behavior is
        // undefined unless '0 <= index1', '0 <= index2', '0 <= numBits',
        // 'bitstring1' has a length of at least 'index1 + numBits',
        // 'bitstring2' has a length of at least 'index2 + numBits', and there
        // is *no* overlap between the swapped ranges of bits.

    static void toggle(int *bitstring, int index, int numBits);
        // Invert the values of the specified 'numBits' in the specified
        // 'bitstring' beginning at the specified 'index'.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and 'bitstring' has a
        // length of at least 'index + numBits'.

    static void xorEqual(int       *dstBitstring,
                         int        dstIndex,
                         const int *srcBitstring,
                         int        srcIndex,
                         int        numBits);
        // Bitwise XOR the specified 'numBits' in the specified 'srcBitstring',
        // beginning at the specified 'srcIndex', to those in the specified
        // 'dstBitstring', beginning at the specified 'dstIndex'.  The behavior
        // is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstBitstring' has a length of at least
        // 'dstIndex + numBits', and 'srcBitstring' has a length of at least
        // 'srcIndex + numBits'.

    static bsl::ostream& print(bsl::ostream&  stream,
                               const int     *bitstring,
                               int            numBits,
                               int            level          = 1,
                               int            spacesPerLevel = 4,
                               int            unitsPerLine   = 4,
                               int            bitsPerUnit    = 8);
        // Format to the specified output 'stream' the specified 'numBits' in
        // the specified 'bitstring' having the specified 'length' and return a
        // reference to the modifiable 'stream'.  The bit values at increasing
        // indexes are formatted from left to right.  Optionally specify
        // 'level', the indentation level for each line output.  Optionally
        // specify 'spacesPerLevel', the number of spaces per indentation
        // level.  Each line is indented by the absolute value of
        // 'level * spacesPerLevel'.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // suppress line breaks and format the entire output on one line.
        // Optionally specify 'unitsPerLine', the number of blank separated
        // groups of bit values output per line.  Optionally specify
        // 'bitsPerUnit', the number of bit values between blanks (i.e., ' ')
        // and thereby define the "unit" managed by 'unitsPerLine'.  If
        // 'stream' is initially invalid, this operation has no effect.  Note
        // that a trailing newline is provided in multiline mode only.

                                 // Accessors

    static bool areEqual(const int *bitstring1,
                         int        index1,
                         const int *bitstring2,
                         int        index2,
                         int        numBits);
        // Return 'true' if the specified 'numBits' beginning at the specified
        // 'index1' in the specified 'bitstring1' are bitwise equal to the
        // 'numBits' beginning at the specified 'index2' in the specified
        // 'bitstring2', and 'false' otherwise.  The behavior is undefined
        // unless '0 <= index1', '0 <= index2', '0 <= numBits', 'bitstring1'
        // has a length of at least 'index1 + numBits', and 'bitstring2' has a
        // length of at least 'index2 + numBits'.

    static bool isAny0(const int *bitstring, int index, int numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitstring' are 0, and 'false'
        // otherwise.  The behavior is undefined unless '0 <= numBits',
        // '0 <= index', and 'bitstring' has a length of at least
        // 'index + numBits'.

    static bool isAny1(const int *bitstring, int index, int numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitstring' are 1, and 'false'
        // otherwise.  The behavior is undefined unless '0 <= numBits',
        // '0 <= index', and 'bitstring' has a length of at least
        // 'index + numBits'.

    static int find0AtLargestIndex(const int *bitstring, int length);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.

    static int find0AtLargestIndexGE(const int *bitstring,
                                     int        length,
                                     int        index);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' whose position is greater than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index < length'.

    static int find0AtLargestIndexGT(const int *bitstring,
                                     int        length,
                                     int        index);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' whose position is greater than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length'.

    static int find0AtLargestIndexLE(const int *bitstring, int index);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' whose position is less than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index' and 'index' is less than
        // the length of 'bitstring'.

    static int find0AtLargestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' whose position is less than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.

    static int find0AtSmallestIndex(const int *bitstring, int length);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.

    static int find0AtSmallestIndexGE(const int *bitstring,
                                      int        length,
                                      int        index);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' having the specified 'length' whose position is greater
        // than or equal to the specified 'index', if such a bit exists, and a
        // negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.

    static int find0AtSmallestIndexGT(const int *bitstring,
                                      int        length,
                                      int        index);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' having the specified 'length' whose position is greater
        // than the specified 'index', if such a bit exists, and a negative
        // value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.

    static int find0AtSmallestIndexLE(const int *bitstring,
                                      int        index);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' whose position is less than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index' and 'index' is less than
        // the length of 'bitstring'.

    static int find0AtSmallestIndexLT(const int *bitstring, int index);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' whose position is less than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.

    static int find1AtLargestIndex(const int *bitstring, int length);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.

    static int find1AtLargestIndexGE(const int *bitstring,
                                     int        length,
                                     int        index);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' whose position is greater than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index < length'.

    static int find1AtLargestIndexGT(const int *bitstring,
                                     int        length,
                                     int        index);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' whose position is greater than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length'.

    static int find1AtLargestIndexLE(const int *bitstring, int index);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' whose position is less than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index' and 'index' is less than
        // the length of 'bitstring'.

    static int find1AtLargestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' whose position is less than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.

    static int find1AtSmallestIndex(const int *bitstring, int length);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.

    static int find1AtSmallestIndexGE(const int *bitstring,
                                      int        length,
                                      int        index);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' having the specified 'length' whose position is greater
        // than or equal to the specified 'index', if such a bit exists, and a
        // negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.

    static int find1AtSmallestIndexGT(const int *bitstring,
                                      int        length,
                                      int        index);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' having the specified 'length' whose position is greater
        // than the specified 'index', if such a bit exists, and a negative
        // value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.

    static int find1AtSmallestIndexLE(const int *bitstring, int index);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' whose position is less than or equal to the specified
        // 'index', if such a bit exists, and a negative value otherwise.  The
        // behavior is undefined unless '0 <= index' and 'index' is less than
        // the length of 'bitstring'.

    static int find1AtSmallestIndexLT(const int *bitstring, int index);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' whose position is less than the specified 'index', if
        // such a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.

    static bool get(const int *bitstring, int index);
        // Return the bit value at the specified 'index' in the specified
        // 'bitstring'.  The behavior is undefined unless '0 <= index' and
        // 'index' is less than the length of 'bitstring'.

    static int get(const int *bitstring, int index, int numBits);
        // Return the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitstring' as the low-order bits of the returned
        // value.  The behavior is undefined unless '0 <= index',
        // '0 <= numBits <= sizeof(int) * CHAR_BIT', and 'bitstring' has a
        // length of at least 'index + numBits'.

    static int num0(const int *bitstring, int index, int numBits);
        // Return the number of 0 bits in the specified 'numBits' beginning at
        // the specified 'index' in the specified 'bitstring'.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and 'bitstring' has a
        // length of at least 'index + numBits'.

    static int num1(const int *bitstring, int index, int numBits);
        // Return the number of 1 bits in the specified 'numBits' beginning at
        // the specified 'index' in the specified 'bitstring'.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and 'bitstring' has a
        // length of at least 'index + numBits'.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static void appendBits(int  *bitstring,
                           int   length,
                           int   numBits,
                           bool  value);
        // Append to the specified 'bitstring' having the specified 'length'
        // the specified 'numBits' having the specified 'value'.  The behavior
        // is undefined unless '0 <= length', '0 <= numBits', and 'bitstring'
        // has a capacity of at least 'length + numBits'.  Note that this
        // function has the same behavior as:
        //..
        //  insert(bitstring, length, value, numBits);
        //..
        //
        // DEPRECATED: Use 'append' instead.

    static void appendBits0(int *bitstring, int length, int numBits);
        // Append to the specified 'bitstring' having the specified 'length'
        // the specified 'numBits' 0 bits.  The behavior is undefined unless
        // '0 <= length', '0 <= numBits', and 'bitstring' has a capacity of at
        // least 'length + numBits'.  Note that this function is logically
        // equivalent to:
        //..
        //  insert0(bitstring, length, numBits);
        //..
        //
        // DEPRECATED: Use 'append0' instead.

    static void appendBits1(int *bitstring, int length, int numBits);
        // Append to the specified 'bitstring' having the specified 'length'
        // the specified 'numBits' 1 bits.  The behavior is undefined unless
        // '0 <= length', '0 <= numBits', and 'bitstring' has a capacity of at
        // least 'length + numBits'.  Note that this function is logically
        // equivalent to:
        //..
        //  insert1(bitstring, length, numBits);
        //..
        //
        // DEPRECATED: Use 'append1' instead.

    static bool compareBits(const int *lhsArray,
                            int        lhsArrayIndex,
                            const int *rhsArray,
                            int        rhsArrayIndex,
                            int        numBits);
        // Return 'true' if the specified 'numBits' beginning at the specified
        // 'index1' in the specified 'bitstring1' are bitwise equal to the
        // 'numBits' beginning at the specified 'index2' in the specified
        // 'bitstring2', and 'false' otherwise.  The behavior is undefined
        // unless '0 <= index1', '0 <= index2', '0 <= numBits', 'bitstring1'
        // has a length of at least 'index1 + numBits', and 'bitstring2' has a
        // length of at least 'index2 + numBits'.
        //
        // DEPRECATED: Use 'areEqual' instead.

    static void copyBits(int       *dstArray,
                         int        dstArrayIndex,
                         const int *srcArray,
                         int        srcArrayIndex,
                         int        numBits);
        // Copy to the specified 'dstBitstring', beginning at the specified
        // 'dstIndex', from the specified 'srcBitstring', beginning at the
        // specified 'srcIndex', the specified 'numBits'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', 'dstBitstring'
        // has a capacity of at least 'dstIndex + numBits', and 'srcBitstring'
        // has a capacity of at least 'srcIndex + numBits'.  Note that if there
        // is an overlap between the source and destination ranges the
        // resulting bits in the destination range may not match the bits
        // originally in the source range.  See 'copy'.
        //
        // DEPRECATED: Use 'copyRaw' instead.

    static int findBit0AtLargestIndex(const int *bitstring, int length);
        // Return the index of the most-significant 0 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.
        //
        // DEPRECATED: Use 'find0AtLargestIndex' instead.

    static int findBit0AtLargestIndexGE(const int *bitstring,
                                        int        length,
                                        int        index);
        // Return the index of the most-significant 0 bit at a position greater
        // than or equal to the specified 'index' in the specified 'bitstring'
        // having the specified 'length', if such a bit exists, and a negative
        // value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGE' instead.

    static int findBit0AtLargestIndexGT(const int *bitstring,
                                        int        length,
                                        int        index);
        // Return the index of the most-significant 0 bit at a position greater
        // than the specified 'index' in the specified 'bitstring' having the
        // specified 'length', if such a bit exists, and a negative value
        // otherwise.  The behavior is undefined unless '0 <= index < length'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGT' instead.

    static int findBit0AtLargestIndexLE(const int *bitstring, int index);
        // Return the index of the most-significant 0 bit at a position less
        // than or equal to the specified 'index' in the specified 'bitstring',
        // if such a bit exists, and a negative value otherwise.  The behavior
        // is undefined unless '0 <= index' and 'index' is less than the length
        // of 'bitstring'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLE' instead.

    static int findBit0AtLargestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 0 bit at a position less
        // than the specified 'index' in the specified 'bitstring', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLT' instead.

    static int findBit0AtSmallestIndex(const int *bitstring, int length);
        // Return the index of the least-significant 0 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndex' instead.

    static int findBit0AtSmallestIndexGE(const int *bitstring,
                                         int        length,
                                         int        index);
        // Return the index of the least-significant 0 bit at a position
        // greater than or equal to the specified 'index' in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGE' instead.

    static int findBit0AtSmallestIndexGT(const int *bitstring,
                                         int        length,
                                         int        index);
        // Return the index of the least-significant 0 bit at a position
        // greater than the specified 'index' in the specified 'bitstring' of
        // the specified 'length', if such a bit exists, and a negative value
        // otherwise.  The behavior is undefined unless '0 <= index < length'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGT' instead.

    static int findBit0AtSmallestIndexLE(const int *bitstring,
                                         int        index);
        // Return the index of the least-significant 0 bit at a position less
        // than or equal to the specified 'index' in the specified 'bitstring',
        // if such a bit exists, and a negative value otherwise.  The behavior
        // is undefined unless '0 <= index' and 'index' is less than the length
        // of 'bitstring'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLE' instead.

    static int findBit0AtSmallestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 0 bit at a position less
        // than the specified 'index' in the specified 'bitstring', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLT' instead.

    static int findBit1AtLargestIndex(const int *bitstring, int length);
        // Return the index of the most-significant 1 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.
        //
        // DEPRECATED: Use 'find1AtLargestIndex' instead.

    static int findBit1AtLargestIndexGE(const int *bitstring,
                                        int        length,
                                        int        index);
        // Return the index of the most-significant 1 bit at a position greater
        // than or equal to the specified 'index' in the specified 'bitstring'
        // having the specified 'length', if such a bit exists, and a negative
        // value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGE' instead.

    static int findBit1AtLargestIndexGT(const int *bitstring,
                                        int        length,
                                        int        index);
        // Return the index of the most-significant 1 bit at a position greater
        // than the specified 'index' in the specified 'bitstring' having the
        // specified 'length', if such a bit exists, and a negative value
        // otherwise.  The behavior is undefined unless '0 <= index < length'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGT' instead.

    static int findBit1AtLargestIndexLE(const int *bitstring, int index);
        // Return the index of the most-significant 1 bit at a position less
        // than or equal to the specified 'index' in the specified 'bitstring',
        // if such a bit exists, and a negative value otherwise.  The behavior
        // is undefined unless '0 <= index' and 'index' is less than the length
        // of 'bitstring'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLE' instead.

    static int findBit1AtLargestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 1 bit at a position less
        // than the specified 'index' in the specified 'bitstring', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLT' instead.

    static int findBit1AtSmallestIndex(const int *bitstring, int length);
        // Return the index of the least-significant 1 bit in the specified
        // 'bitstring' having the specified 'length', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= length'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndex' instead.

    static int findBit1AtSmallestIndexGE(const int *bitstring,
                                         int        length,
                                         int        index);
        // Return the index of the least-significant 1 bit at a position
        // greater than or equal to the specified 'index' in the specified
        // 'bitstring' having the specified 'length', if such a bit exists, and
        // a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGE' instead.

    static int findBit1AtSmallestIndexGT(const int *bitstring,
                                         int        length,
                                         int        index);
        // Return the index of the least-significant 1 bit at a position
        // greater than the specified 'index' in the specified 'bitstring' of
        // the specified 'length', if such a bit exists, and a negative value
        // otherwise.  The behavior is undefined unless '0 <= index < length'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGT' instead.

    static int findBit1AtSmallestIndexLE(const int *bitstring, int index);
        // Return the index of the least-significant 1 bit at a position less
        // than or equal to the specified 'index' in the specified 'bitstring',
        // if such a bit exists, and a negative value otherwise.  The behavior
        // is undefined unless '0 <= index' and 'index' is less than the length
        // of 'bitstring'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLE' instead.

    static int findBit1AtSmallestIndexLT(const int *bitstring, int index);
        // Return the index of the most-significant 1 bit at a position less
        // than the specified 'index' in the specified 'bitstring', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index' and 'index' is less than the length of
        // 'bitstring'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLT' instead.

    static bool getBit(const int *bitstring, int index);
        // Return the bit value at the specified 'index' in the specified
        // 'bitstring'.  The behavior is undefined unless '0 <= index' and
        // 'index' is less than the length of 'bitstring'.
        //
        // DEPRECATED: Use 'get' instead.

    static int getBits(const int *bitstring, int index, int numBits);
        // Return the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitstring' as the low-order bits of the returned
        // value.  The behavior is undefined unless '0 <= index',
        // '0 <= numBits <= sizeof(int) * CHAR_BIT', and 'bitstring' has a
        // length of at least 'index + numBits'.
        //
        // DEPRECATED: Use 'get' instead.

    static void insertBits(int  *bitstring,
                           int   length,
                           int   dstIndex,
                           int   numBits,
                           bool  value);
        // Insert into the specified 'bitstring' having the specified 'length'
        // at the specified 'dstIndex' the specified 'numBits' each having the
        // specified 'value'.  All bits having indices at or above 'dstIndex'
        // are shifted up by 'numBits' index positions.  The behavior is
        // undefined unless '0 <= numBits', '0 <= dstIndex <= length', and
        // 'bitstring' has a capacity of at least 'length + numBits'.
        //
        // DEPRECATED: Use 'insert' instead.

    static void insertBits0(int *bitstring,
                            int  length,
                            int  dstIndex,
                            int  numBits);
        // Insert into the specified 'bitstring' having the specified 'length'
        // at the specified 'dstIndex' the specified 'numBits' 0 bits.  All
        // bits having indices at or above 'dstIndex' are shifted up by
        // 'numBits' index positions.  The behavior is undefined unless
        // '0 <= numBits', '0 <= dstIndex <= length', and 'bitstring' has a
        // capacity of at least 'length + numBits'.
        //
        // DEPRECATED: Use 'insert0' instead.

    static void insertBits1(int *bitstring,
                            int  length,
                            int  dstIndex,
                            int  numBits);
        // Insert into the specified 'bitstring' having the specified 'length'
        // at the specified 'dstIndex' the specified 'numBits' 1 bits.  All
        // bits having indices at or above 'dstIndex' are shifted up by
        // 'numBits' index positions.  The behavior is undefined unless
        // '0 <= numBits', '0 <= dstIndex <= length', and 'bitstring' has a
        // capacity of at least 'length + numBits'.
        //
        // DEPRECATED: Use 'insert1' instead.

    static void insertBitsRaw(int *bitstring,
                              int  length,
                              int  dstIndex,
                              int  numBits);
        // Shift, in the specified 'bitstring' having the specified 'length',
        // the bits having indices at or above the specified 'dstIndex' upward
        // by the specified 'numBits'.  The behavior is undefined unless
        // '0 <= numBits', '0 <= dstIndex <= length', and 'bitstring' has a
        // capacity of at least 'length + numBits'.
        //
        // DEPRECATED: Use 'insertRaw' instead.

    static bool isAnyBit0(const int *bitstring, int index, int numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitstring' are 0, and 'false'
        // otherwise.  The behavior is undefined unless '0 <= numBits',
        // '0 <= index', and 'bitstring' has a length of at least
        // 'index + numBits'.
        //
        // DEPRECATED: Use 'isAny0' instead.

    static bool isAnyBit1(const int *bitstring, int index, int numBits);
        // Return 'true' if any of the specified 'numBits' beginning at the
        // specified 'index' in the specified 'bitstring' are 1, and 'false'
        // otherwise.  The behavior is undefined unless '0 <= numBits',
        // '0 <= index', and 'bitstring' has a length of at least
        // 'index + numBits'.
        //
        // DEPRECATED: Use 'isAny1' instead.

    static void moveBits(int       *dstArray,
                         int        dstArrayIndex,
                         const int *srcArray,
                         int        srcArrayIndex,
                         int        numBits);
        // Copy to the specified 'dstBitstring', beginning at the specified
        // 'dstIndex', from the specified 'srcBitstring', beginning at the
        // specified 'srcIndex', the specified 'numBits'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex', 'dstBitstring'
        // has a capacity of at least 'dstIndex + numBits', and 'srcBitstring'
        // has a capacity of at least 'srcIndex + numBits'.  Note that even if
        // there is an overlap between the source and destination ranges the
        // resulting bits in the destination range will match the bits
        // originally in the source range (as if the bits had been copied into
        // a temporary buffer and then copied into the destination range).  See
        // 'copyRaw'.
        //
        // DEPRECATED: Use 'copy' instead.

    static void removeBits(int  *bitstring,
                           int   length,
                           int   index,
                           int   numBits,
                           bool  value);
        // Remove the specified 'numBits' beginning at the specified 'index'
        // from the specified 'bitstring' having the specified 'length'.  Any
        // bits at or above 'index + numBits' are shifted down by 'numBits'
        // index positions and the last 'numBits' of 'bitstring' are set to the
        // specified 'value'.  The behavior is undefined unless '0 <= length',
        // '0 <= index', '0 <= numBits', and 'index + numBits <= length'.  Note
        // that the length of 'bitstring' is not changed.
        //
        // DEPRECATED: Use 'removeAndFill' instead.

    static void removeBits0(int *bitstring,
                            int  length,
                            int  index,
                            int  numBits);
        // Remove the specified 'numBits' beginning at the specified 'index'
        // from the specified 'bitstring' having the specified 'length'.  Any
        // bits at or above 'index + numBits' are shifted down by 'numBits'
        // index positions and the last 'numBits' of 'bitstring' are set to 0.
        // The behavior is undefined unless '0 <= length', '0 <= index',
        // '0 <= numBits', and 'index + numBits <= length'.  Note that the
        // length of 'bitstring' is not changed.
        //
        // DEPRECATED: Use 'removeAndFill0' instead.

    static void removeBits1(int *bitstring,
                            int  length,
                            int  index,
                            int  numBits);
        // Remove the specified 'numBits' beginning at the specified 'index'
        // from the specified 'bitstring' having the specified 'length'.  Any
        // bits at or above 'index + numBits' are shifted down by 'numBits'
        // index positions and the last 'numBits' of 'bitstring' are set to 1.
        // The behavior is undefined unless '0 <= length', '0 <= index',
        // '0 <= numBits', and 'index + numBits <= length'.  Note that the
        // length of 'bitstring' is not changed.
        //
        // DEPRECATED: Use 'removeAndFill1' instead.

    static void removeBitsRaw(int *bitstring,
                              int  length,
                              int  index,
                              int  numBits);
        // Remove the specified 'numBits' beginning at the specified 'index'
        // from the specified 'bitstring' having the specified 'length'.  Any
        // bits at or above 'index + numBits' are shifted down by 'numBits'
        // index positions and the length of 'bitstring' is reduced by
        // 'numBits'.  The behavior is undefined unless '0 <= length',
        // '0 <= index', '0 <= numBits', and 'index + numBits <= length'.
        //
        // DEPRECATED: Use 'remove' instead.

    static void setBit(int *bitstring, int index, bool value);
        // Set the bit at the specified 'index' in the specified 'bitstring' to
        // the specified 'value'.  The behavior is undefined unless
        // '0 <= index' and 'index' is less than the length of 'bitstring'.
        //
        // DEPRECATED: Use 'set' instead.

    static void setBits(int *bitstring, int index, int numBits, bool value);
        // Set the specified 'numBits' beginning at the specified 'index' in
        // the specified 'bitstring' to the specified 'value'.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and 'bitstring' has a
        // capacity of at least 'index + numBits'.
        //
        // DEPRECATED: Use 'set' instead.

    static void swapBits(int *lhsArray,
                         int  lhsArrayIndex,
                         int *rhsArray,
                         int  rhsArrayIndex,
                         int  numBits);
        // Exchange the specified 'numBits' beginning at the specified 'index1'
        // in the specified 'bitstring1' with the 'numBits' beginning at the
        // specified 'index2' in the specified 'bitstring2'.  The behavior is
        // undefined unless '0 <= index1', '0 <= index2', '0 <= numBits',
        // 'bitstring1' has a length of at least 'index1 + numBits',
        // 'bitstring2' has a length of at least 'index2 + numBits', and there
        // is *no* overlap between the swapped ranges of bits.
        //
        // DEPRECATED: Use 'swapRaw' instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------
                        // struct bdeu_BitstringUtil
                        // -------------------------

// CLASS METHODS

                             // Manipulators

inline
void bdeu_BitstringUtil::append(int  *bitstring,
                                int   length,
                                bool  value,
                                int   numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    set(bitstring, length, value, numBits);
}

inline
void bdeu_BitstringUtil::append0(int *bitstring, int length, int numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    set(bitstring, length, false, numBits);
}

inline
void bdeu_BitstringUtil::append1(int *bitstring, int length, int numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    set(bitstring, length, true, numBits);
}

inline
void bdeu_BitstringUtil::insert(int  *bitstring,
                                int   length,
                                int   dstIndex,
                                bool  value,
                                int   numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    insertRaw(bitstring, length, dstIndex, numBits);
    set(bitstring, dstIndex, value, numBits);
}

inline
void bdeu_BitstringUtil::insert0(int *bitstring,
                                 int  length,
                                 int  dstIndex,
                                 int  numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    insertRaw(bitstring, length, dstIndex, numBits);
    set(bitstring, dstIndex, false, numBits);
}

inline
void bdeu_BitstringUtil::insert1(int *bitstring,
                                 int  length,
                                 int  dstIndex,
                                 int  numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length);
    BSLS_ASSERT_SAFE(0 <= numBits);

    insertRaw(bitstring, length, dstIndex, numBits);
    set(bitstring, dstIndex, true, numBits);
}

inline
void bdeu_BitstringUtil::removeAndFill(int  *bitstring,
                                       int   length,
                                       int   index,
                                       bool  value,
                                       int   numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= length);

    remove(bitstring, length, index, numBits);
    set(bitstring, length - numBits, value, numBits);
}

inline
void bdeu_BitstringUtil::removeAndFill0(int *bitstring,
                                        int  length,
                                        int  index,
                                        int  numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= length);

    remove(bitstring, length, index, numBits);
    set(bitstring, length - numBits, false, numBits);
}

inline
void bdeu_BitstringUtil::removeAndFill1(int *bitstring,
                                        int  length,
                                        int  index,
                                        int  numBits)
{
    BSLS_ASSERT_SAFE(bitstring);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= length);

    remove(bitstring, length, index, numBits);
    set(bitstring, length - numBits, true, numBits);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
void bdeu_BitstringUtil::appendBits(int  *bitstring,
                                    int   length,
                                    int   numBits,
                                    bool  value)
{
    return append(bitstring, length, value, numBits);
}

inline
void bdeu_BitstringUtil::appendBits0(int *bitstring, int length, int numBits)
{
    return append0(bitstring, length, numBits);
}

inline
void bdeu_BitstringUtil::appendBits1(int *bitstring, int length, int numBits)
{
    return append1(bitstring, length, numBits);
}

inline
bool bdeu_BitstringUtil::compareBits(const int *lhsArray,
                                     int        lhsArrayIndex,
                                     const int *rhsArray,
                                     int        rhsArrayIndex,
                                     int        numBits)
{
    return areEqual(lhsArray, lhsArrayIndex, rhsArray, rhsArrayIndex, numBits);
}

inline
void bdeu_BitstringUtil::copyBits(int       *dstArray,
                                  int        dstArrayIndex,
                                  const int *srcArray,
                                  int        srcArrayIndex,
                                  int        numBits)
{
    copyRaw(dstArray, dstArrayIndex, srcArray, srcArrayIndex, numBits);
}

inline
int bdeu_BitstringUtil::findBit0AtLargestIndex(const int *bitstring,
                                               int        length)
{
    return find0AtLargestIndex(bitstring, length);
}

inline
int bdeu_BitstringUtil::findBit0AtLargestIndexGE(const int *bitstring,
                                                 int        length,
                                                 int        index)
{
    return find0AtLargestIndexGE(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit0AtLargestIndexGT(const int *bitstring,
                                                 int        length,
                                                 int        index)
{
    return find0AtLargestIndexGT(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit0AtLargestIndexLE(const int *bitstring,
                                                 int        index)
{
    return find0AtLargestIndexLE(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit0AtLargestIndexLT(const int *bitstring,
                                                 int        index)
{
    return find0AtLargestIndexLT(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit0AtSmallestIndex(const int *bitstring,
                                                int        length)
{
    return find0AtSmallestIndex(bitstring, length);
}

inline
int bdeu_BitstringUtil::findBit0AtSmallestIndexGE(const int *bitstring,
                                                  int        length,
                                                  int        index)
{
    return find0AtSmallestIndexGE(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit0AtSmallestIndexGT(const int *bitstring,
                                                  int        length,
                                                  int        index)
{
    return find0AtSmallestIndexGT(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit0AtSmallestIndexLE(const int *bitstring,
                                                  int        index)
{
    return find0AtSmallestIndexLE(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit0AtSmallestIndexLT(const int *bitstring,
                                                  int        index)
{
    return find0AtSmallestIndexLT(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit1AtLargestIndex(const int *bitstring,
                                               int        length)
{
    return find1AtLargestIndex(bitstring, length);
}

inline
int bdeu_BitstringUtil::findBit1AtLargestIndexGE(const int *bitstring,
                                                 int        length,
                                                 int        index)
{
    return find1AtLargestIndexGE(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit1AtLargestIndexGT(const int *bitstring,
                                                 int        length,
                                                 int        index)
{
    return find1AtLargestIndexGT(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit1AtLargestIndexLE(const int *bitstring,
                                                 int        index)
{
    return find1AtLargestIndexLE(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit1AtLargestIndexLT(const int *bitstring,
                                                 int        index)
{
    return find1AtLargestIndexLT(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit1AtSmallestIndex(const int *bitstring,
                                                int        length)
{
    return find1AtSmallestIndex(bitstring, length);
}

inline
int bdeu_BitstringUtil::findBit1AtSmallestIndexGE(const int *bitstring,
                                                  int        length,
                                                  int        index)
{
    return find1AtSmallestIndexGE(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit1AtSmallestIndexGT(const int *bitstring,
                                                  int        length,
                                                  int        index)
{
    return find1AtSmallestIndexGT(bitstring, length, index);
}

inline
int bdeu_BitstringUtil::findBit1AtSmallestIndexLE(const int *bitstring,
                                                  int        index)
{
    return find1AtSmallestIndexLE(bitstring, index);
}

inline
int bdeu_BitstringUtil::findBit1AtSmallestIndexLT(const int *bitstring,
                                                  int        index)
{
    return find1AtSmallestIndexLT(bitstring, index);
}

inline
bool bdeu_BitstringUtil::getBit(const int *bitstring, int index)
{
    return get(bitstring, index);
}

inline
int bdeu_BitstringUtil::getBits(const int *bitstring, int index, int numBits)
{
    return get(bitstring, index, numBits);
}

inline
void bdeu_BitstringUtil::insertBits(int  *bitstring,
                                    int   length,
                                    int   dstIndex,
                                    int   numBits,
                                    bool  value)
{
    insert(bitstring, length, dstIndex, value, numBits);
}

inline
void bdeu_BitstringUtil::insertBits0(int *bitstring,
                                     int  length,
                                     int  dstIndex,
                                     int  numBits)
{
    insert0(bitstring, length, dstIndex, numBits);
}

inline
void bdeu_BitstringUtil::insertBits1(int *bitstring,
                                     int  length,
                                     int  dstIndex,
                                     int  numBits)
{
    insert1(bitstring, length, dstIndex, numBits);
}

inline
void bdeu_BitstringUtil::insertBitsRaw(int *bitstring,
                                       int  length,
                                       int  dstIndex,
                                       int  numBits)
{
    insertRaw(bitstring, length, dstIndex, numBits);
}

inline
bool bdeu_BitstringUtil::isAnyBit0(const int *bitstring,
                                   int        index,
                                   int        numBits)
{
    return isAny0(bitstring, index, numBits);
}

inline
bool bdeu_BitstringUtil::isAnyBit1(const int *bitstring,
                                   int        index,
                                   int        numBits)
{
    return isAny1(bitstring, index, numBits);
}

inline
void bdeu_BitstringUtil::moveBits(int       *dstArray,
                                  int        dstArrayIndex,
                                  const int *srcArray,
                                  int        srcArrayIndex,
                                  int        numBits)
{
    copy(dstArray, dstArrayIndex, srcArray, srcArrayIndex, numBits);
}

inline
void bdeu_BitstringUtil::removeBits(int  *bitstring,
                                    int   length,
                                    int   index,
                                    int   numBits,
                                    bool  value)
{
    return removeAndFill(bitstring, length, index, value, numBits);
}

inline
void bdeu_BitstringUtil::removeBits0(int *bitstring,
                                     int  length,
                                     int  index,
                                     int  numBits)
{
    removeAndFill0(bitstring, length, index, numBits);
}

inline
void bdeu_BitstringUtil::removeBits1(int *bitstring,
                                     int  length,
                                     int  index,
                                     int  numBits)
{
    removeAndFill1(bitstring, length, index, numBits);
}

inline
void bdeu_BitstringUtil::removeBitsRaw(int *bitstring,
                                       int  length,
                                       int  index,
                                       int  numBits)
{
    remove(bitstring, length, index, numBits);
}

inline
void bdeu_BitstringUtil::setBit(int *bitstring, int index, bool value)
{
    set(bitstring, index, value);
}

inline
void bdeu_BitstringUtil::setBits(int  *bitstring,
                                 int   index,
                                 int   numBits,
                                 bool  value)
{
    set(bitstring, index, value, numBits);
}

inline
void bdeu_BitstringUtil::swapBits(int *lhsArray,
                                  int  lhsArrayIndex,
                                  int *rhsArray,
                                  int  rhsArrayIndex,
                                  int  numBits)
{
    swapRaw(lhsArray, lhsArrayIndex, rhsArray, rhsArrayIndex, numBits);
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
