// bdeu_bitstringutil.t.cpp                  -*-C++-*-

#include <bdeu_bitstringutil.h>

#include <bdes_bitutil.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cctype.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// TBD:
// Add insert cases where the index == length
//-----------------------------------------------------------------------------
// [15] void andEqual(int       *dstBitstring,
//                    int        dstIndex,
//                    const int *srcBitstring,
//                    int        srcIndex,
//                    int        numBits);
// [ 8] void append(int *bitstring, int length, bool value, int numBits);
// [ 8] void append0(int *bitstring, int length, int numBits);
// [ 8] void append1(int *bitstring, int length, int numBits);
// [ 3] bool areEqual(const int *lhsBitstring,
//                    int        lhsIndex,
//                    const int *rhsBitstring,
//                    int        rhsIndex,
//                    int        numBits);
// [ 7] void copyRaw(int       *dstBitstring,
//                   int        dstIndex,
//                   const int *srcBitstring,
//                   int        srcIndex,
//                   int        numBits);
// [20] int find0AtLargestIndex(const int *bitstring, int length);
// [20] int find0AtLargestIndexGE(const int *bitstring,
//                                int        length,
//                                int        index);
// [20] int find0AtLargestIndexGT(const int *bitstring,
//                                int        length,
//                                int        index);
// [20] int find0AtLargestIndexLE(const int *bitstring, int index);
// [20] int find0AtLargestIndexLT(const int *bitstring, int index);
// [22] int find0AtSmallestIndex(const int *bitstring, int length);
// [22] int find0AtSmallestIndexGE(const int *bitstring,
//                                 int        length,
//                                 int        index);
// [22] int find0AtSmallestIndexGT(const int *bitstring,
//                                 int        length,
//                                 int        index);
// [22] int find0AtSmallestIndexLE(const int *bitstring, int index);
// [22] int find0AtSmallestIndexLT(const int *bitstring, int index);
// [21] int find1AtLargestIndex(const int *bitstring, int length);
// [21] int find1AtLargestIndexGE(const int *bitstring,
//                                int        length,
//                                int        index);
// [21] int find1AtLargestIndexGT(const int *bitstring,
//                                int        length,
//                                int        index);
// [21] int find1AtLargestIndexLE(const int *bitstring, int index);
// [21] int find1AtLargestIndexLT(const int *bitstring, int index);
// [23] int find1AtSmallestIndex(const int *bitstring, int length);
// [23] int find1AtSmallestIndexGE(const int *bitstring,
//                                 int        length,
//                                 int        index);
// [23] int find1AtSmallestIndexGT(const int *bitstring,
//                                 int        length,
//                                 int        index);
// [23] int find1AtSmallestIndexLE(const int *bitstring, int index);
// [23] int find1AtSmallestIndexLT(const int *bitstring, int index);
// [ 4] bool get(const int *bitstring, int index);
// [ 5] int get(const int *bitstring, int index, int numBits);
// [10] void insert(int  *bitstring,
//                  int   length,
//                  int   index,
//                  bool  value,
//                  int   numBits);
// [10] void insert0(int *bitstring, int length, int index, int numBits);
// [10] void insert1(int *bitstring, int length, int index, int numBits);
// [10] void insertRaw(int *bitstring, int length, int index, int numBits);
// [ 6] bool isAny0(const int *bitstring, int index, int numBits);
// [ 6] bool isAny1(const int *bitstring, int index, int numBits);
// [16] void minusEqual(int       *dstBitstring,
//                      int        dstIndex,
//                      const int *srcBitstring,
//                      int        srcIndex,
//                      int        numBits);
// [ 9] void copy(int       *dstBitstring,
//                int        dstIndex,
//                const int *srcBitstring,
//                int        srcIndex,
//                int        numBits);
// [14] int num0(const int *bitstring, int index, int numBits);
// [14] int num1(const int *bitstring, int index, int numBits);
// [17] void orEqual(int       *dstBitstring,
//                   int        dstIndex,
//                   const int *srcBitstring,
//                   int        srcIndex,
//                   int        numBits);
// [11] void removeAndFill(int  *bitstring,
//                         int   length,
//                         int   index,
//                         bool  value,
//                         int   numBits);
// [11] void removeAndFill0(int *bitstring, int len, int idx, int numBits);
// [11] void removeAndFill1(int *bitstring, int len, int idx, int numBits);
// [11] void remove(int *bitstring, int len, int idx, int numBits);
// [ 4] void set(int *bitstring, int index, bool value);
// [ 5] void set(int *bitstring, int index, bool value, int numBits);
// [12] void swapRaw(int *lhsBitstring,
//                   int  lhsIndex,
//                   int *rhsBitstring,
//                   int  rhsIndex,
//                   int  numBits);
// [13] bsl::ostream& print(bsl::ostream&  stream,
//                          const int     *bitstring,
//                          int            numBits,
//                          int            level = 1,
//                          int            spl = 4,
//                          int            unitsPerLine = 4,
//                          int            bitsPerSubUnit = 8);
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [ 2] HELPER FUNCTIONS
// [24] REFERENCE TEST
// [25] USAGE example
//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
namespace {
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
} // closed unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush; // P(X) no nl
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;        // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bdeu_BitstringUtil BSU;

enum { BITS_PER_INT = sizeof(int) * CHAR_BIT };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
void populateBitstring(int *bitstring, int index, const char *ascii)
    // Populate the bits starting at the specified 'index' in the specified
    // 'bitstring' with the values specified by the characters in the
    // specified 'ascii'.  The behavior is undefined unless the characters in
    // 'ascii' are either '0' or '1', and 'bitstring' has a capacity of at
    // least 'index + bsl::strlen(ascii)' bits.
{
    ASSERT(bitstring);
    ASSERT(0 <= index);
    ASSERT(ascii);

    int  idx      = index / BITS_PER_INT;
    int  pos      = index % BITS_PER_INT;
    int *intPtr   = &bitstring[idx];
    int  numChars = bsl::strlen(ascii);

    while (numChars > 0) {
        char currValue = ascii[numChars - 1];
        if (bsl::isspace(currValue)) {
            --numChars;
            continue;
        }

        ASSERT('0' == currValue || '1' == currValue);

        if (pos == BITS_PER_INT) {
            pos = 0;
            ++intPtr;
        }
        bdes_BitUtil::replaceBitValue(intPtr, pos, currValue - '0');
        ++pos;
        --numChars;
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

bool checkControl(const int *lhsBitstring,
                  int        lhsBitstringIdx,
                  const int *rhsBitstring,
                  int        rhsBitstringIdx,
                  int        totalNumBits,
                  int        exceptLhsIdx = -1,
                  int        exceptRhsIdx = -1,
                  int        exceptNumBits = 0)
    // Compare the specified 'totalNumBits' starting at the specified
    // 'lhsBitstringIdx' in the specified 'lhsBitstring' with the
    // 'totalNumBits' starting at the specified 'rhsBitstringIdx' in the
    // specified 'rhsBitstring'.  Optionally specify 'exceptNumBits' starting
    // at the optionally specified 'exceptLhsIdx' in 'lhsBitstring' and the
    // optionally specified 'exceptRhsIdx' in 'rhsBitstring' that should not be
    // compared.  Return 'true' if the bits are equal and 'false' otherwise.
    // The behavior is undefined unless '0 <= lhsBitstringIdx',
    // '0 <= rhsBitstringIdx', '0 <= totalNumBits', and if 'exceptNumBits > 0'
    // then 'lhsBitstringIdx <= exceptLhsIdx',
    // 'exceptLhsIdx < lhsBitstringIdx + totalNumBits',
    // 'rhsBitstringIdx <= exceptRhsIdx', and
    // 'exceptRhsIdx < rhsBitstringIdx + totalNumBits'.
{
    ASSERT(0 <= lhsBitstringIdx);
    ASSERT(0 <= rhsBitstringIdx);
    ASSERT(0 <= totalNumBits);

    if (0 == exceptNumBits) {
        return BSU::areEqual(lhsBitstring,
                                lhsBitstringIdx,
                                rhsBitstring,
                                rhsBitstringIdx,
                                totalNumBits);
    }

    ASSERT(lhsBitstringIdx <= exceptLhsIdx
        && exceptLhsIdx < lhsBitstringIdx + totalNumBits);
    ASSERT(rhsBitstringIdx <= exceptRhsIdx
        && exceptRhsIdx < rhsBitstringIdx + totalNumBits);

    return BSU::areEqual(lhsBitstring, lhsBitstringIdx, rhsBitstring,
                            rhsBitstringIdx, exceptLhsIdx - lhsBitstringIdx)
        && BSU::areEqual(
                lhsBitstring,
                exceptLhsIdx + exceptNumBits,
                rhsBitstring,
                exceptRhsIdx + exceptNumBits,
                lhsBitstringIdx + totalNumBits - exceptLhsIdx - exceptNumBits);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_vector.h>

                        // ==============
                        // class IntArray
                        // ==============

 class IntArray {
    // This class implements an in-place array of 'int' values stored
    // contiguously in memory.  In addition to storing valid 'int' values
     // users can also store null 'int's.  The physical capacity of this
     // array may grow, but never shrinks.  Capacity will be increased
     // automatically as needed.

     enum { BITS_PER_INT = sizeof(int) * CHAR_BIT };

     // DATA
     bsl::vector<int>  d_values;            // integer values
     int              *d_nullBits_p;        // nullness bit array
     int               d_nullBitsArrayLen;  // length of the nullness bits
                                            // integer array (in words)
     bslma_Allocator  *d_allocator_p;       // memory allocator

     // NOT IMPLEMENTED
     IntArray(const IntArray& original);
     IntArray& operator=(const IntArray& rhs);

     // PRIVATE MANIPULATORS
     void resizeNullnessBitsArray();
         // Resize the nullness bits array stored by this 'int' array.

   public:
     // CREATORS
     explicit IntArray(int size = 0, bslma_Allocator *basicAllocator = 0);
         // Construct an 'int' array.  Optionally specify 'size' elements
         // initialized to '0' that would be the initial size of the
         // array.  Optionally specify a 'basicAllocator' used to supply
         // memory.  If 'basicAllocator' is 0, the currently installed
         // default allocator is used.

     ~IntArray();
         // Destroy this 'int' array.

     // MANIPULATORS
     int& operator[](int index);
         // Return a reference to the modifiable integer value at the
         // specified 'index' in this 'int' array.  The behavior is
         // undefined unless '0 <= index < length()'.  Note that if the
         // element at 'index' is null then the nullness flag is reset and the
         // returned value is 'INT_MIN'.

     void appendInt(int value);
         // Append the specified 'value' to this 'int' array.

     void appendNullInt();
         // Append a null 'int' to this 'int' array.  Note that the appended
         // 'int' will have a value of 'INT_MIN'.

     void insertInt(int index, int value);
         // Insert the specified 'value' into this 'int' array at the
         // specified 'index'.  The behavior is undefined unless
         // '0 <= index <= length()'.

     void insertNullInt(int index);
         // Insert a null 'int' into this 'int' array at the specified
         // 'index'.  The behavior is undefined unless
         // '0 <= index <= length()'.  Note that the inserted 'int' will have a
         // value of 'INT_MIN'.

     void makeNull(int index);
         // Make the 'int' at the specified 'index' null.  The behavior
         // is undefined unless '0 <= index < length()'.  Note that the new
         // value of the element will be 'INT_MIN'.

     void makeNonNull(int index);
         // Make the integer at the specified 'index' non-null.  The
         // behavior is undefined unless '0 <= index < length()'.

     // ACCESSORS
     int length() const;
         // Return the length of this 'int' array.

     const int& operator[](int index) const;
         // Return a reference to the non-modifiable 'int' value at the
         // specified 'index' in this 'int' array.  The behavior is
         // undefined unless '0 <= index < length()'.  Note that if the
         // element at 'index' is null then the nullness flag is not reset
         // and the returned value is 'INT_MIN'.

     bool isNull(int index) const;
         // Return 'true' if the integer at the specified 'index' is null
         // and 'false' otherwise.  The behavior is undefined unless
         // '0 <= index < length()'.
 };
                     // --------------
                     // class IntArray
                     // --------------

     // PRIVATE MANIPULATORS
     void IntArray::resizeNullnessBitsArray()
     {
         // Increment by one 'int' at a time
         int *newBitArray = (int *) d_allocator_p->allocate(
                                    (d_nullBitsArrayLen + 1) * sizeof(int));
         if (d_nullBitsArrayLen > 0) {
             bdeu_BitstringUtil::copyRaw(newBitArray,
                                          0,
                                          d_nullBits_p,
                                          0,
                                          d_nullBitsArrayLen * BITS_PER_INT);
             d_allocator_p->deallocate(d_nullBits_p);
         }
         ++d_nullBitsArrayLen;
         d_nullBits_p = newBitArray;
     }

     // CREATORS
     IntArray::IntArray(int size, bslma_Allocator *basicAllocator)
     : d_values(size, 0, basicAllocator)
     , d_nullBits_p(0)
     , d_nullBitsArrayLen(0)
     , d_allocator_p(bslma_Default::allocator(basicAllocator))
     {
         d_nullBitsArrayLen = (size + BITS_PER_INT - 1) / BITS_PER_INT;
         d_nullBits_p       = (int *) d_allocator_p->allocate(
                                          sizeof(int) * d_nullBitsArrayLen);
     }

     IntArray::~IntArray()
     {
         d_allocator_p->deallocate(d_nullBits_p);
     }

     // MANIPULATORS
     int& IntArray::operator[](int index)
     {
         ASSERT(0 <= index && index < length());
         bdeu_BitstringUtil::set(d_nullBits_p, index, false);
         return d_values[index];
     }

     void IntArray::appendInt(int value)
     {
         insertInt(length(), value);
     }

     void IntArray::appendNullInt()
     {
         insertNullInt(length());
     }

     void IntArray::insertInt(int index, int value)
     {
         ASSERT(0 <= index && index <= length());
         d_values.insert(d_values.begin() + index, value);
         if (d_values.size() > d_nullBitsArrayLen * sizeof(int)) {
             resizeNullnessBitsArray();
         }
         bdeu_BitstringUtil::insert0(d_nullBits_p,
                                         length() - 1,
                                         index,
                                         1);
     }

     void IntArray::insertNullInt(int index)
     {
         ASSERT(0 <= index && index <= length());
         d_values.insert(d_values.begin() + index, INT_MIN);
         if (d_values.size() > d_nullBitsArrayLen * sizeof(int)) {
             resizeNullnessBitsArray();
         }
         bdeu_BitstringUtil::insert1(d_nullBits_p,
                                         length() - 1,
                                         index,
                                         1);
     }

     void IntArray::makeNull(int index)
     {
         ASSERT(0 <= index && index < length());
         d_values[index] = INT_MIN;
         bdeu_BitstringUtil::set(d_nullBits_p, index, true);
     }

     void IntArray::makeNonNull(int index)
     {
         ASSERT(0 <= index && index < length());
         bdeu_BitstringUtil::set(d_nullBits_p, index, false);
     }

     // ACCESSORS
     int IntArray::length() const
     {
         return d_values.size();
     }

     const int& IntArray::operator[](int index) const
     {
         ASSERT(0 <= index && index < length());
         return d_values[index];
     }

     bool IntArray::isNull(int index) const
     {
         ASSERT(0 <= index && index < length());
         return bdeu_BitstringUtil::get(d_nullBits_p, index);
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

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 27: {
      // ----------------------------------------------------------------------
      // USAGE TEST
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      // ----------------------------------------------------------------------
      if (verbose) bsl::cout << "Testing USAGE *new* Example" << bsl::endl;

// Bitstreams can be used to represent efficient business calendars and do
// operations on such calendars.  First, create a 'Bitstream' with sufficient
// capacity to represent every day of a year (note that 32 * 12 = 384) and set
// a 1-bit in the indices corresponding to the day-of-year (DOY) for each
// weekend day.  For convenience in date calculations the 0 index is not used;
// the 365 days of the year are at indices [1,365].  Further note that the
// values set below all correspond to the year 2010:
//..
    int weekends[12] = {0};

    for (int i = 2 /* first Sat */; i < 366; i += 7) {
        bdeu_BitstringUtil::set(weekends, i, 1);
        bdeu_BitstringUtil::set(weekends, i+1, 1);
    }
//..
// Now, we can easily use 'bdeu_bitstreamutil' methods to find days of
// interest.  For example:
//..
    int firstWeekendDay = bdeu_BitstringUtil::find1AtSmallestIndex(
                                                                  weekends,
                                                                  365 + 1);
    int lastWeekendDay  = bdeu_BitstringUtil::find1AtLargestIndex(
                                                                  weekends,
                                                                  365 + 1);
//..
// We can define an enumeration to assist us in representing these DOY values
// into convention dates and confirm the calculated values:
//..
    enum {
        JAN = 0,  // Note: First DOY is 'JAN + 1'.
        FEB = JAN + 31,
        MAR = FEB + 28,
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

    ASSERT(JAN +  2 == firstWeekendDay);
    ASSERT(DEC + 26 ==  lastWeekendDay);
//..
// The enumeration allows us to easily represent the business holidays of the
// year and significant dates in the business calendar:
//..
    int holidays[12] = {0};

    enum {
        NEW_YEARS_DAY    = JAN +  1,
        MLK_DAY          = JAN + 18,
        PRESIDENTS_DAY   = FEB + 15,
        GOOD_FRIDAY      = APR +  2,
        MEMORIAL_DAY     = MAY + 31,
        INDEPENDENCE_DAY = JUL +  5,
        LABOR_DAY        = SEP +  6,
        THANKSGIVING     = NOV + 25,
        CHRISTMAS        = DEC + 24
    };

    bdeu_BitstringUtil::set(holidays, NEW_YEARS_DAY,    1);
    bdeu_BitstringUtil::set(holidays, MLK_DAY,          1);
    bdeu_BitstringUtil::set(holidays, PRESIDENTS_DAY,   1);
    bdeu_BitstringUtil::set(holidays, GOOD_FRIDAY,      1);
    bdeu_BitstringUtil::set(holidays, MEMORIAL_DAY,     1);
    bdeu_BitstringUtil::set(holidays, INDEPENDENCE_DAY, 1);
    bdeu_BitstringUtil::set(holidays, LABOR_DAY,        1);
    bdeu_BitstringUtil::set(holidays, THANKSGIVING,     1);
    bdeu_BitstringUtil::set(holidays, CHRISTMAS,        1);

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
    int firstHolidayOfQ3 = bdeu_BitstringUtil::find1AtSmallestIndexGT(holidays,
                                                                      Q4,
                                                                      Q3);
    ASSERT(INDEPENDENCE_DAY == firstHolidayOfQ3);
//..
//  Our calendars are readily combined to represent days off for either reason
//  (i.e., holiday or weekend):
//..
    int allDaysOff[12] = {0};
    bdeu_BitstringUtil::orEqual(allDaysOff, 1, weekends, 1, 365);
    bdeu_BitstringUtil::orEqual(allDaysOff, 1, holidays, 1, 365);

    int isOffMay07 = bdeu_BitstringUtil::get(allDaysOff, MAY +  7);
    int isOffMay08 = bdeu_BitstringUtil::get(allDaysOff, MAY +  8);
    int isOffMay09 = bdeu_BitstringUtil::get(allDaysOff, MAY +  9);
    int isOffMay10 = bdeu_BitstringUtil::get(allDaysOff, MAY + 10);
    int isOffXmas  = bdeu_BitstringUtil::get(allDaysOff, CHRISTMAS);

    ASSERT(0 == isOffMay07);
    ASSERT(1 == isOffMay08);
    ASSERT(1 == isOffMay09);
    ASSERT(0 == isOffMay10);
    ASSERT(1 == isOffXmas);
//..
    } break;
    case 26: {
      // ----------------------------------------------------------------------
      // USAGE TEST
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      // ----------------------------------------------------------------------
      if (verbose) bsl::cout << "Testing Sample Usages" << bsl::endl;

      int S[] = { 0xB, 0x0 };

      ASSERT(BSU::areEqual(S, 0, S, 0, 10)         ==   true);
      ASSERT(BSU::areEqual(S, 3, S, 1,  2)         ==   true);
      ASSERT(BSU::areEqual(S, 3, S, 0,  3)         ==   false);

      ASSERT(BSU::isAny0(S, 0, 2)                  ==   false);
      ASSERT(BSU::isAny0(S, 4, 6)                  ==   true);
      ASSERT(BSU::isAny1(S, 0, 2)                  ==   true);
      ASSERT(BSU::isAny1(S, 4, 6)                  ==   false);

      ASSERT(BSU::num0(S, 0, 2)                   ==   0);
      ASSERT(BSU::num0(S, 4, 6)                   ==   6);
      ASSERT(BSU::num1(S, 0, 2)                   ==   2);
      ASSERT(BSU::num1(S, 4, 6)                   ==   0);
      ASSERT(BSU::find0AtLargestIndex (S, 10)      ==   9);
      ASSERT(BSU::find1AtLargestIndex (S, 10)      ==   3);
      ASSERT(BSU::find0AtSmallestIndex(S, 10)      ==   2);
      ASSERT(BSU::find1AtSmallestIndex(S, 10)      ==   0);

      ASSERT(BSU::find0AtLargestIndexGE(S, 10, 4)  ==   9);
      ASSERT(BSU::find0AtLargestIndexGT(S, 10, 4)  ==   9);
      ASSERT(BSU::find0AtLargestIndexLE(S,     4)  ==   4);
      ASSERT(BSU::find0AtLargestIndexLT(S,     4)  ==   2);

      ASSERT(BSU::find1AtLargestIndexGE(S, 10, 3)  ==   3);
      ASSERT(BSU::find1AtLargestIndexGT(S, 10, 3)  ==  -1);
      ASSERT(BSU::find1AtLargestIndexLE(S,     2)  ==   1);
      ASSERT(BSU::find1AtLargestIndexLT(S,     1)  ==   0);

      ASSERT(BSU::find0AtSmallestIndexGE(S, 10, 4) ==  4);
      ASSERT(BSU::find0AtSmallestIndexGT(S, 10, 4) ==  5);
      ASSERT(BSU::find0AtSmallestIndexLE(S,     2) ==  2);
      ASSERT(BSU::find0AtSmallestIndexLT(S,     2) == -1);

      ASSERT(BSU::find1AtSmallestIndexGE(S, 10, 3) ==  3);
      ASSERT(BSU::find1AtSmallestIndexGT(S, 10, 3) == -1);
      ASSERT(BSU::find1AtSmallestIndexLE(S,     3) ==  0);
      ASSERT(BSU::find1AtSmallestIndexLT(S,     1) ==  0);

      ASSERT(BSU::get (S, 0)                       ==  1);
      ASSERT(BSU::get (S, 1)                       ==  1);
      ASSERT(BSU::get (S, 2)                       ==  0);
      ASSERT(BSU::get(S, 0, 1)                    ==  1);
      ASSERT(BSU::get(S, 0, 2)                    ==  3);
      ASSERT(BSU::get(S, 0, 3)                    ==  3);
      ASSERT(BSU::get(S, 1, 9)                    ==  5);
      ASSERT(BSU::get(S, 4, 9)                    ==  0);

      const char *EXP = "    [\n        11010000 00\n    ]\n";
      bsl::ostringstream stream;
      BSU::print(stream, S, 10);
      ASSERT(0 == bsl::strcmp(EXP, stream.str().c_str()));



// Manipulators
//- - - - -
// The following manipulator methods operate on a single bitstring.
//..
//                                    index: 9  8  7  6  5  4  3  2  1  0   Ln
//                                           -  -  -  -  -  -  -  -  -  -   --
//    Source Argument                    D:  0  0  0  0  0  0  1  0  1  1   10
//
//    Example Usage                   Destination After Operation           Ln
//    =============                   ===================================  ====
//    append (&D, 10, V, 2)           [V  V] 0  0  0  0  0  0  1  0  1  1  [12]
    {
        int D0 = 0xB;
        BSU::append(&D0, 10, false, 2);
        int E0 = 0x0;
        populateBitstring(&E0, 0, "000000001011");
        ASSERT(E0 == D0);

        int D1 = 0xB;
        BSU::append(&D1, 10, true, 2);
        int E1 = 0x0;
        populateBitstring(&E1, 0, "110000001011");

        ASSERT(E1 == D1);
    }
//    append0(&D, 10, 2)              [0  0] 0  0  0  0  0  0  1  0  1  1  [12]
    {
        int D = 0xB;
        BSU::append0(&D, 10, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "000000001011");
        ASSERT(E == D);
    }
//    append1(&D, 10, 2)              [1  1] 0  0  0  0  0  0  1  0  1  1  [12]
    {
        int D = 0xB;
        BSU::append1(&D, 10, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "110000001011");
        ASSERT(E == D);
    }
//
//    insert   (&D, 10, 4, V, 2)       0  0  0  0  0  0 [V  V] 1  0  1  1  [12]
    {
        int D0 = 0xB;
        BSU::insert(&D0, 10, 4, false, 2);
        int E0 = 0x0;
        populateBitstring(&E0, 0, "000000001011");
        ASSERT(E0 == D0);

        int D1 = 0xB;
        BSU::insert(&D1, 10, 4, true, 2);
        int E1 = 0x0;
        populateBitstring(&E1, 0, "000000111011");
        ASSERT(E1 == D1);
    }
//    insert0  (&D, 10, 4, 2)          0  0  0  0  0  0 [0  0] 1  0  1  1  [12]
    {
        int D = 0xB;
        BSU::insert0(&D, 10, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "000000001011");
        ASSERT(E == D);
    }
//    insert1  (&D, 10, 4, 2)          0  0  0  0  0  0 [1  1] 1  0  1  1  [12]
    {
        int D = 0xB;
        BSU::insert1(&D, 10, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "000000111011");
        ASSERT(E == D);
    }
//    insertRaw(&D, 10, 4, 2)          0  0  0  0  0  0 [X  X] 1  0  1  1  [12]
    {
        int D = 0xB;
        BSU::insertRaw(&D, 10, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "000000001011");
        ASSERT(E == (D & 0x0FF));
        ASSERT(E == (D & 0x3FF));
    }
//
//    removeAndFill(&D, 10, 3, V, 3)        [V  V  V] 0 [0  0  0] 0  1  1   10
    {
        int D0 = 0xB;
        BSU::removeAndFill(&D0, 10, 3, false, 3);
        int E0 = 0x0;
        populateBitstring(&E0, 0, "0000000011");
        ASSERT(E0 == D0);

        int D1 = 0xB;
        BSU::removeAndFill(&D1, 10, 3, true, 3);
        int E1 = 0x0;
        populateBitstring(&E1, 0, "1110000011");
        ASSERT(E1 == D1);
    }
//    removeAndFill0  (&D, 10, 3, 3)        [0  0  0] 0 [0  0  0] 0  1  1   10
    {
        int D = 0xB;
        BSU::removeAndFill0(&D, 10, 3, 3);
        int E = 0x0;
        populateBitstring(&E, 0, "0000000011");
        ASSERT(E == D);
    }
//    removeAndFill1  (&D, 10, 3, 3)        [1  1  1] 0 [0  0  0] 0  1  1   10
    {
        int D = 0xB;
        BSU::removeAndFill1(&D, 10, 3, 3);
        int E = 0x0;
        populateBitstring(&E, 0, "1110000011");
        ASSERT(E == D);
    }
//    remove(&D, 10, 3, 3)                            0 [0  0  0] 0  1  1  [ 7]
    {
        int D = 0xB;
        BSU::remove(&D, 10, 3, 3);
        int E = 0x0;
        populateBitstring(&E, 0, "0000011");
        ASSERT(E == (D & 0x7F));
    }
//
//    set    (&D, 5, 1)                      0  0  0  0 [1] 0  1  0  1  1   10
    {
        int D = 0xB;
        BSU::set(&D, 5, 1);
        int E = 0x0;
        populateBitstring(&E, 0, "0000101011");
        ASSERT(E == D);
    }
//    set   (&D, 6, true, 2)                 0  0 [1  1] 0  0  1  0  1  1   10
    {
        int D = 0xB;
        BSU::set(&D, 6, true, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0011001011");
        ASSERT(E == D);
    }
//    toggle(&D, 2, 3)                       0  0  0  0  0 [1  0  1] 1  1   10
    {
        int D = 0xB;
        BSU::toggle(&D, 2, 3);
        int E = 0x0;
        populateBitstring(&E, 0, "0000010111");
        ASSERT(E == D);
    }
//..
// The following manipulator methods have a both a destination bitstring and
// a 'const', source bitstring.
//..
//                                    index: 9  8  7  6  5  4  3  2  1  0   Ln
//                                           -  -  -  -  -  -  -  -  -  -   --
//    Destination Argument               D:  0  0  0  0  0  0  1  0  1  1   10
//    Source Argument                    S:        1  0  1  0  1  0  0  1    8
//
//    Example Usage                          Destination After Operation    Ln
//    =============                          ============================  ====
//    andEqual  (&D, 6, &S, 4, 2)            0  0 [0  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::andEqual(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0000001011");
        ASSERT(E == D);
    }
//    copyRaw      (&D, 6, &S, 4, 2)         0  0 [1  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::copyRaw(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0010001011");
        ASSERT(E == D);
    }
//    minusEqual(&D, 6, &S, 4, 2)            0  0 [0  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::minusEqual(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0000001011");
        ASSERT(E == D);
    }
//    copy      (&D, 6, &S, 4, 2)            0  0 [1  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::copy(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0010001011");
        ASSERT(E == D);
    }
//    orEqual   (&D, 6, &S, 4, 2)            0  0 [1  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::orEqual(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0010001011");
        ASSERT(E == D);
    }
//    xorEqual  (&D, 6, &S, 4, 2)            0  0 [1  0] 0  0  1  0  1  1   10
    {
        int D = 0x0B;
        int S = 0xA9;
        BSU::xorEqual(&D, 6, &S, 4, 2);
        int E = 0x0;
        populateBitstring(&E, 0, "0010001011");
        ASSERT(E == D);
    }
//..
// There is also a 'swapRaw' method in which the second bitstring is
// non-'const'.
//..
//                                    index: 9  8  7  6  5  4  3  2  1  0   Ln
//                                           -  -  -  -  -  -  -  -  -  -   --
//    Arguments                         S1:  0  0  0  0  0  0  1  0  1  1   10
//                                      S2:        1  0  1  0  1  0  0  1    8
//
//    Example Usage                     Bitstrings After Operation          Ln
//    =============                     =================================  ====
//    swapRaw(&S1, 6, &S2, 4, 2)        S1:  0  0 [1  0] 0  0  1  0  1  1   10
//                                      S2:        1  0 [0  0] 1  0  0  1    8
    {
        int S1 = 0x0B;
        int S2 = 0xA9;
        BSU::swapRaw(&S1, 6, &S2, 4, 2);
        int ES1 = 0x0;
        populateBitstring(&ES1, 0, "0010001011");
        int ES2 = 0x0;
        populateBitstring(&ES2, 0,   "10001001");
        ASSERT(ES1 == S1);
        ASSERT(ES2 == S2);
    }
//..
    } break;
    case 25: {
      // ----------------------------------------------------------------------
      // USAGE TEST
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      // ----------------------------------------------------------------------
      if (verbose) bsl::cout << "Testing USAGE Example" << bsl::endl;

      IntArray array; const IntArray& ARRAY = array;
      ASSERT(0       == ARRAY.length());

      array.appendInt(5);
      ASSERT(1       == ARRAY.length());
      ASSERT(5       == ARRAY[0]);
      ASSERT(false   == ARRAY.isNull(0));

      array.appendNullInt();
      ASSERT(2       == ARRAY.length());
      ASSERT(5       == ARRAY[0]);
      ASSERT(INT_MIN == ARRAY[1]);
      ASSERT(false   == ARRAY.isNull(0));
      ASSERT(true    == ARRAY.isNull(1));

      array.insertInt(0, 10);
      ASSERT(3       == ARRAY.length());
      ASSERT(10      == ARRAY[0]);
      ASSERT(5       == ARRAY[1]);
      ASSERT(INT_MIN == ARRAY[2]);
      ASSERT(false   == ARRAY.isNull(0));
      ASSERT(false   == ARRAY.isNull(1));
      ASSERT(true    == ARRAY.isNull(2));

      array.insertNullInt(0);
      ASSERT(4       == ARRAY.length());
      ASSERT(INT_MIN == ARRAY[0]);
      ASSERT(10      == ARRAY[1]);
      ASSERT(5       == ARRAY[2]);
      ASSERT(INT_MIN == ARRAY[3]);
      ASSERT(true    == ARRAY.isNull(0));
      ASSERT(false   == ARRAY.isNull(1));
      ASSERT(false   == ARRAY.isNull(2));
      ASSERT(true    == ARRAY.isNull(3));

      array.makeNull(1);
      ASSERT(4       == ARRAY.length());
      ASSERT(INT_MIN == ARRAY[0]);
      ASSERT(INT_MIN == ARRAY[1]);
      ASSERT(5       == ARRAY[2]);
      ASSERT(INT_MIN == ARRAY[3]);
      ASSERT(true    == ARRAY.isNull(0));
      ASSERT(true    == ARRAY.isNull(1));
      ASSERT(false   == ARRAY.isNull(2));
      ASSERT(true    == ARRAY.isNull(3));

      array.makeNonNull(0);
      ASSERT(4       == ARRAY.length());
      ASSERT(INT_MIN == ARRAY[0]);
      ASSERT(INT_MIN == ARRAY[1]);
      ASSERT(5       == ARRAY[2]);
      ASSERT(INT_MIN == ARRAY[3]);
      ASSERT(false   == ARRAY.isNull(0));
      ASSERT(true    == ARRAY.isNull(1));
      ASSERT(false   == ARRAY.isNull(2));
      ASSERT(true    == ARRAY.isNull(3));
    } break;
    case 24: {
      // ----------------------------------------------------------------------
      // REFERENCE TEST
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      // --------------------------------------------------------------------

//                                  index: 7  6  5  4  3  2  1  0
//                                         -  -  -  -  -  -  -  -
//    Source Arguments                S1:  0  0  0  0  1  0  1  1
//                                    S2:  0  0  0  0  0  0  0  0
//                                    S3:  1  1  1  1  1  1  1  1
//
        const int X1 = 0xB;
        const int X2 = 0;
        const int X3 = 0xFF;

        int mS1 = X1; int *S1 = &mS1;
        int mS2 = X2; int *S2 = &mS2;
        int mS3 = X3; int *S3 = &mS3;

//    Accessor Functions                   Integer Value Returned
//    ==================                   ======================
//
//    isAny0(S1, 0, 2)                            0
//    isAny0(S1, 3, 4)                            1
//    isAny0(S2, 0, 8)                            1
//    isAny0(S3, 0, 8)                            0

        ASSERT(!BSU::isAny0(S1, 0, 2));
        ASSERT(BSU::isAny0(S1, 3, 4));
        ASSERT(BSU::isAny0(S2, 0, 8));
        ASSERT(!BSU::isAny0(S3, 0, 8));
//
//    isAny1(S1, 0, 3)                            1
//    isAny1(S1, 4, 4)                            0
//    isAny1(S2, 0, 8)                            0
//    isAny1(S3, 0, 8)                            1

        ASSERT(BSU::isAny1(S1, 0, 3));
        ASSERT(!BSU::isAny1(S1, 4, 4));
        ASSERT(!BSU::isAny1(S2, 0, 8));
        ASSERT(BSU::isAny1(S3, 0, 8));
//
//    get(S1, 0)                                  1
//    get(S1, 6)                                  0
//    get(S2, 3)                                  0
//    get(S3, 2)                                  1

        ASSERT(BSU::get(S1, 0));
        ASSERT(!BSU::get(S1, 6));
        ASSERT(!BSU::get(S2, 3));
        ASSERT(BSU::get(S3, 2));
//
//    get(S1, 2, 4)                              0  0  0  0  0  0  1  0
//    get(S1, 1, 3)                              0  0  0  0  0  1  0  1
//    get(S2, 5, 2)                              0  0  0  0  0  0  0  0
//    get(S3, 0, 3)                              0  0  0  0  0  1  1  1

        ASSERT(2 == BSU::get(S1, 2, 4));
        ASSERT(5 == BSU::get(S1, 1, 3));
        ASSERT(0 == BSU::get(S2, 5, 2));
        ASSERT(7 == BSU::get(S3, 0, 3));
//
//    areEqual(S1, 0, S2, 2, 4)                   0
//    areEqual(S1, 4, S2, 4, 4)                   1
//    areEqual(S2, 0, S3, 0, 8)                   0

        ASSERT(!BSU::areEqual(S1, 0, S2, 2, 4));
        ASSERT(BSU::areEqual(S1, 4, S2, 4, 4));
        ASSERT(!BSU::areEqual(S2, 0, S3, 0, 8));
//
//    Manipulator Functions                               Result value
//    =====================                               ============
//
//    set(S1, 5, 1)                               0  0 [1] 0  1  0  1  1
//    set(S2, 1, 1)                               0  0  0  0  0  0 [1] 0
//    set(S3, 7, 0)                              [0] 1  1  1  1  1  1  1
//
        ASSERT(!BSU::get(S1, 5));
        ASSERT(!BSU::get(S2, 1));
        ASSERT(BSU::get(S3, 7));

        BSU::set(S1, 5, 1);
        BSU::set(S2, 1, 1);
        BSU::set(S3, 7, 0);

        ASSERT(BSU::get(S1, 5));
        ASSERT(BSU::get(S2, 1));
        ASSERT(!BSU::get(S3, 7));

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

//    set(S1, 6, true, 2)                          [1  1] 0  0  1  0  1  1
//    set(S2, 0, true, 4)                           0  0  0  0 [1  1  1  1]
//    set(S3, 4, false, 4)                          [0  0  0  0] 1  1  1  1
//
        BSU::set(S1, 6, true, 2);
        BSU::set(S2, 0, true, 4);
        BSU::set(S3, 4, false, 4);

        ASSERT(0xCB == *S1);
        ASSERT(0x0F == *S2);
        ASSERT(0x0F == *S3);

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

//    insert1(S1, 8, 5, 2)                       0 [1  1] 0  1  0  1  1
//    insert1(S2, 8, 0, 3)                       0  0  0  0  0 [1  1  1]
//    insert0(S3, 8, 0, 8)                      [0  0  0  0  0  0  0  0]
//
        BSU::insert1(S1, 8, 5, 2);
        BSU::insert1(S2, 8, 0, 3);
        BSU::insert0(S3, 8, 0, 8);

        ASSERT(0x6B   == *S1);
        ASSERT(0x07   == *S2);
        ASSERT(0xFF00 == *S3);

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

//    removeAndFill0(S1, 8, 3, 3)                       [0  0  0  0  0] 0  1  1
//    removeAndFill1(S2, 8, 3, 3)                       [1  1  1  0  0] 0  0  0
//    removeAndFill0(S3, 8, 4, 3)                       [0  0  0  1] 1  1  1  1
//
        BSU::removeAndFill0(S1, 8, 3, 3);
        BSU::removeAndFill1(S2, 8, 3, 3);
        BSU::removeAndFill0(S3, 8, 4, 3);

        ASSERT(0x03 == *S1);
        ASSERT(0xE0 == *S2);
        ASSERT(0x1F == *S3);

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

//    copyRaw(S1, 4, S3, 4, 4)                     [1  1  1  1] 1  0  1  1
//    copyRaw(S2, 2, S1, 1, 4)                      0  0 [0  1  0  1] 0  0
//    copyRaw(S1, 5, S1, 0, 2)                      0 [1  1] 0  1  0  1  1
//
        BSU::copyRaw(S1, 4, S3, 4, 4);
        ASSERT(0xFB == *S1);
        mS1 = X1;

        BSU::copyRaw(S2, 2, S1, 1, 4);
        BSU::copyRaw(S1, 5, S1, 0, 2);

        ASSERT(0x14 == *S2);
        ASSERT(0x6B == *S1);

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

//    copy(S1, 2, S1, 0, 4)                      0  0 [1  0  1  1] 1  1
//    copy(S2, 3, S2, 0, 4)                      0 [0  0  0  0] 0  0  0
//    copy(S1, 4, S1, 0, 4)                     [1  0  1  1] 1  0  1  1

        BSU::copy(S1, 2, S1, 0, 4);
        ASSERT(0x2F == *S1);
        mS1 = X1;

        BSU::copy(S2, 3, S2, 0, 4);
        BSU::copy(S1, 4, S1, 0, 4);

        ASSERT(0x00 == *S2);
        ASSERT(0xBB == *S1);

        mS1 = X1;
        mS2 = X2;
        mS3 = X3;

///..
    } break;
    case 23: {
      // --------------------------------------------------------------------
      // TESTING FINDBIT1ATSMALLESTINDEX METHODS
      //
      // Plan:
      //
      // Testing:
      //   int find1AtSmallestIndex(const int *bitstring, int length);
      //   int find1AtSmallestIndexGE(const int *bitstring,
      //                              int        length,
      //                              int        index);
      //   int find1AtSmallestIndexGT(const int *bitstring,
      //                              int        length,
      //                              int        index);
      //   int find1AtSmallestIndexLE(const int *bitstring, int index);
      //   int find1AtSmallestIndexLT(const int *bitstring, int index);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'findBit1AtSmallest functions'"
                             << "\n====================================="
                             << bsl::endl;

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
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *STR       = DATA[i].d_array_p;
          const int   LEN       = numBits(STR);
          const int   IDX       = DATA[i].d_index;
          const int   EXP_S     = DATA[i].d_smallestIdx;
          const int   EXP_GT    = DATA[i].d_greaterThanIdx;
          const int   EXP_GE    = DATA[i].d_greaterThanEqualIdx;
          const int   EXP_LT    = DATA[i].d_lessThanIdx;
          const int   EXP_LE    = DATA[i].d_lessThanEqualIdx;

          if (veryVerbose) {
              P_(LINE) P_(STR) P_(IDX) P_(EXP_S) P_(EXP_LT) P_(EXP_LE)
              P_(EXP_GT) P_(EXP_GE)
          }

          int array[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(array, 0, STR);

          if (veryVerbose) {
              BSU::print(bsl::cout, array, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          const int S  = BSU::find1AtSmallestIndex(array, LEN);
          const int GT = BSU::find1AtSmallestIndexGT(array, LEN, IDX);
          const int GE = BSU::find1AtSmallestIndexGE(array, LEN, IDX);
          const int LT = BSU::find1AtSmallestIndexLT(array, IDX);
          const int LE = BSU::find1AtSmallestIndexLE(array, IDX);
          LOOP3_ASSERT(LINE, S, EXP_S, S == EXP_S);
          LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
          LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
          LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
          LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
      }
    } break;
    case 22: {
      // --------------------------------------------------------------------
      // TESTING FINDBIT0ATSMALLESTINDEX METHODS
      //
      // Plan:
      //
      // Testing:
      //   int find0AtSmallestIndex(const int *bitstring, int length);
      //   int find0AtSmallestIndexGE(const int *bitstring,
      //                              int        length,
      //                              int        index);
      //   int find0AtSmallestIndexGT(const int *bitstring,
      //                              int        length,
      //                              int        index);
      //   int find0AtSmallestIndexLE(const int *bitstring, int index);
      //   int find0AtSmallestIndexLT(const int *bitstring, int index);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'findBit0AtSmallest functions'"
                             << "\n====================================="
                             << bsl::endl;

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
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *STR       = DATA[i].d_array_p;
          const int   LEN       = numBits(STR);
          const int   IDX       = DATA[i].d_index;
          const int   EXP_S     = DATA[i].d_smallestIdx;
          const int   EXP_GT    = DATA[i].d_greaterThanIdx;
          const int   EXP_GE    = DATA[i].d_greaterThanEqualIdx;
          const int   EXP_LT    = DATA[i].d_lessThanIdx;
          const int   EXP_LE    = DATA[i].d_lessThanEqualIdx;

          if (veryVerbose) {
              P_(LINE) P_(STR) P_(IDX) P_(EXP_S) P_(EXP_LT) P_(EXP_LE)
              P_(EXP_GT) P_(EXP_GE)
          }

          int array[MAX_ARRAY_SIZE];
          bsl::memset(array, 0xFF, sizeof array);
          populateBitstring(array, 0, STR);

          if (veryVerbose) {
              BSU::print(bsl::cout, array, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          const int S  = BSU::find0AtSmallestIndex(array, LEN);
          const int GT = BSU::find0AtSmallestIndexGT(array, LEN, IDX);
          const int GE = BSU::find0AtSmallestIndexGE(array, LEN, IDX);
          const int LT = BSU::find0AtSmallestIndexLT(array, IDX);
          const int LE = BSU::find0AtSmallestIndexLE(array, IDX);
          LOOP3_ASSERT(LINE, S, EXP_S, S == EXP_S);
          LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
          LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
          LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
          LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
      }
    } break;
    case 21: {
      // --------------------------------------------------------------------
      // TESTING FINDBIT1ATLARGESTINDEX METHODS
      //
      // Plan:
      //
      // Testing:
      //   int find1AtLargestIndex(const int *bitstring, int length);
      //   int find1AtLargestIndexGE(const int *bitstring,
      //                             int        length,
      //                             int        index);
      //   int find1AtLargestIndexGT(const int *bitstring,
      //                             int        length,
      //                             int        index);
      //   int find1AtLargestIndexLE(const int *bitstring, int index);
      //   int find1AtLargestIndexLT(const int *bitstring, int index);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'findBit1AtLargest functions'"
                             << "\n====================================="
                             << bsl::endl;

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
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *STR       = DATA[i].d_array_p;
          const int   LEN       = numBits(STR);
          const int   IDX       = DATA[i].d_index;
          const int   EXP_L     = DATA[i].d_largestIdx;
          const int   EXP_GT    = DATA[i].d_greaterThanIdx;
          const int   EXP_GE    = DATA[i].d_greaterThanEqualIdx;
          const int   EXP_LT    = DATA[i].d_lessThanIdx;
          const int   EXP_LE    = DATA[i].d_lessThanEqualIdx;

          if (veryVerbose) {
              P_(LINE) P_(STR) P_(IDX) P_(EXP_L) P_(EXP_LT) P_(EXP_LE)
              P_(EXP_GT) P_(EXP_GE)
          }

          int array[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(array, 0, STR);

          if (veryVerbose) {
              BSU::print(bsl::cout, array, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          const int L  = BSU::find1AtLargestIndex(array, LEN);
          const int GT = BSU::find1AtLargestIndexGT(array, LEN, IDX);
          const int GE = BSU::find1AtLargestIndexGE(array, LEN, IDX);
          const int LT = BSU::find1AtLargestIndexLT(array, IDX);
          const int LE = BSU::find1AtLargestIndexLE(array, IDX);
          LOOP3_ASSERT(LINE, L, EXP_L, L == EXP_L);
          LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
          LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
          LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
          LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
      }
    } break;
    case 20: {
      // --------------------------------------------------------------------
      // TESTING FINDBIT0ATLARGESTINDEX METHODS
      //
      // Plan:
      //
      // Testing:
      //   int find0AtLargestIndex(const int *bitstring, int length);
      //   int find0AtLargestIndexGE(const int *bitstring,
      //                             int        length,
      //                             int        index);
      //   int find0AtLargestIndexGT(const int *bitstring,
      //                             int        length,
      //                             int        index);
      //   int find0AtLargestIndexLE(const int *bitstring, int index);
      //   int find0AtLargestIndexLT(const int *bitstring, int index);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'findBit0AtLargest functions'"
                             << "\n====================================="
                             << bsl::endl;

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
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *STR       = DATA[i].d_array_p;
          const int   LEN       = numBits(STR);
          const int   IDX       = DATA[i].d_index;
          const int   EXP_L     = DATA[i].d_largestIdx;
          const int   EXP_GT    = DATA[i].d_greaterThanIdx;
          const int   EXP_GE    = DATA[i].d_greaterThanEqualIdx;
          const int   EXP_LT    = DATA[i].d_lessThanIdx;
          const int   EXP_LE    = DATA[i].d_lessThanEqualIdx;

          if (veryVerbose) {
              P_(LINE) P_(STR) P_(IDX) P_(EXP_L) P_(EXP_LT) P_(EXP_LE)
              P_(EXP_GT) P_(EXP_GE)
          }

          int array[MAX_ARRAY_SIZE];
          bsl::memset(array, 0xFF, sizeof array);
          populateBitstring(array, 0, STR);

          if (veryVerbose) {
              BSU::print(bsl::cout, array, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          const int L  = BSU::find0AtLargestIndex(array, LEN);
          const int GT = BSU::find0AtLargestIndexGT(array, LEN, IDX);
          const int GE = BSU::find0AtLargestIndexGE(array, LEN, IDX);
          const int LT = BSU::find0AtLargestIndexLT(array, IDX);
          const int LE = BSU::find0AtLargestIndexLE(array, IDX);
          LOOP3_ASSERT(LINE, L, EXP_L, L == EXP_L);
          LOOP3_ASSERT(LINE, GT, EXP_GT, GT == EXP_GT);
          LOOP3_ASSERT(LINE, GE, EXP_GE, GE == EXP_GE);
          LOOP3_ASSERT(LINE, LT, EXP_LT, LT == EXP_LT);
          LOOP3_ASSERT(LINE, LE, EXP_LE, LE == EXP_LE);
      }
    } break;
    case 19: {
      // --------------------------------------------------------------------
      // TESTING 'toggle'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void toggle(int *bitstring, int index, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'toggle'"
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
      {   L_,  "",                     0,   0,  ""    },
      {   L_,  "1",                    0,   0,  "1"   },
      {   L_,  "1",                    0,   1,  "0"   },
      {   L_,  "0",                    0,   1,  "1"   },
      {   L_,  "00",                   0,   1,  "01"  },
      {   L_,  "00",                   1,   1,  "10"  },
      {   L_,  "10",                   1,   1,  "00"  },
      {   L_,  "01",                   1,   1,  "11"  },
      {   L_,  "101",                  1,   1,  "111" },
      {   L_,  "101",                  0,   1,  "100" },
      {   L_,  "101",                  2,   1,  "001" },
      {   L_,  "10",                   0,   2,  "01"  },
      {   L_,  "01",                   0,   2,  "10"  },
      {   L_,  "00",                   0,   2,  "11"  },
      {   L_,  "11",                   0,   2,  "00"  },
      {   L_,  "101",                  1,   2,  "011" },
      {   L_,  "010",                  1,   2,  "100" },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       0,     10,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110011 00001111",
     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       2,     20,
     "11110000 11110000 11110000 11110000 11110000 11001111 00001111 00001100",
     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       7,     32,
     "11110000 11110000 11110000 11001111 00001111 00001111 00001111 01110000",
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     1,
"1010 11110000 11110000 11110000 11110000 10110000 11110000 11110000 11110000",
       //                                  ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     2,
"1010 11110000 11110000 11110000 11110000 00110000 11110000 11110000 11110000",
       //                                 ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     3,
"1010 11110000 11110000 11110000 11110001 00110000 11110000 11110000 11110000",
       //                               ^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     4,
"1010 11110000 11110000 11110000 11110011 00110000 11110000 11110000 11110000",
       //                              ^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     33,
"1010 10001111 00001111 00001111 00001111 00110000 11110000 11110000 11110000",
 //    ^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     34,
"1010 00001111 00001111 00001111 00001111 00110000 11110000 11110000 11110000",
 //   ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     35,
"1011 00001111 00001111 00001111 00001111 00110000 11110000 11110000 11110000",
 // ^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       30,     36,
"1001 00001111 00001111 00001111 00001111 00110000 11110000 11110000 11110000",
// ^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     1,
"1010 11110000 11110000 11110000 11110000 01110000 11110000 11110000 11110000",
       //                                 ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     2,
"1010 11110000 11110000 11110000 11110001 01110000 11110000 11110000 11110000",
       //                               ^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     3,
"1010 11110000 11110000 11110000 11110011 01110000 11110000 11110000 11110000",
       //                              ^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     32,
"1010 10001111 00001111 00001111 00001111 01110000 11110000 11110000 11110000",
//     ^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     33,
"1010 00001111 00001111 00001111 00001111 01110000 11110000 11110000 11110000",
//    ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     34,
"1011 00001111 00001111 00001111 00001111 01110000 11110000 11110000 11110000",
//  ^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       31,     35,
"1001 00001111 00001111 00001111 00001111 01110000 11110000 11110000 11110000",
// ^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     1,
"1010 11110000 11110000 11110000 11110001 01110000 11110000 11110000 11110000",
       //                               ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     2,
"1010 11110000 11110000 11110000 11110011 01110000 11110000 11110000 11110000",
       //                              ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     31,
"1010 10001111 00001111 00001111 00001111 11110000 11110000 11110000 11110000",
//     ^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     32,
"1010 00001111 00001111 00001111 00001111 11110000 11110000 11110000 11110000",
//    ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     33,
"1011 00001111 00001111 00001111 00001111 11110000 11110000 11110000 11110000",
//  ^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       32,     34,
"1001 00001111 00001111 00001111 00001111 11110000 11110000 11110000 11110000",
// ^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       63,     1,
"1010 10110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//     ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       63,     2,
"1010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//    ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       63,     3,
"1011 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//  ^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       63,     4,
"1001 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
// ^^ ^^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       64,     1,
"1010 01110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//    ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       64,     2,
"1011 01110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//  ^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       64,     3,
"1001 01110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
// ^^ ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       65,     1,
"1011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
//  ^
     },
     { L_,
"1010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
       65,     2,
"1001 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
// ^^
     },
      };

      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *STR       = DATA[i].d_array_p;
          const int   LEN       = bsl::strlen(STR);
          const int   IDX       = DATA[i].d_index;
          const int   NB        = DATA[i].d_numBits;
          const char *RESULT    = DATA[i].d_result_p;
          LOOP_ASSERT(LINE, LEN == bsl::strlen(RESULT));

          if (veryVerbose) {
              P_(LINE) P_(STR) P_(IDX) P_(NB) P(RESULT)
          }

          int array[MAX_ARRAY_SIZE]  = { 0 };
          int result[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(array, 0, STR);
          populateBitstring(result, 0, RESULT);

          if (veryVerbose) {
              BSU::print(bsl::cout, array, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, result, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          BSU::toggle(array, IDX, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(array, 0, result, 0, NB));
      }
    } break;
    case 18: {
      // --------------------------------------------------------------------
      // TESTING 'xorEqual'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void xorEqual(int       *dstBitstring,
      //                 int        dstIndex,
      //                 const int *srcBitstring,
      //                 int        srcIndex,
      //                 int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'xorEqual'"
                             << "\n==================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_dstBitstring_p;
          int         d_dstIndex;
          const char *d_srcBitstring_p;
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
{  L_,  "1",    0, "1",    0,  1,     "0"   },
{  L_,  "00",   1, "0",    0,  1,     "00"  },
{  L_,  "10",   1, "0",    0,  1,     "10"  },
{  L_,  "00",   1, "1",    0,  1,     "10"  },
{  L_,  "11",   1, "1",    0,  1,     "01"  },
{  L_,  "10",   1, "01",   1,  1,     "10"  },
{  L_,  "00",   1, "11",   1,  1,     "10"  },
{  L_,  "00",   1, "01",   1,  1,     "00"  },
{  L_,  "00",   1, "11",   1,  1,     "10"  },
{  L_,  "11",   1, "11",   1,  1,     "01"  },
{  L_,  "11",   0, "01",   0,  2,     "10"  },
{  L_,  "11",   0, "00",   0,  2,     "11"  },
{  L_,  "11",   0, "10",   0,  2,     "01"  },
{  L_,  "10",   0, "00",   0,  2,     "10"  },
{  L_,  "01",   0, "11",   0,  2,     "10"  },
{  L_,  "10",   0, "10",   0,  2,     "00"  },
{  L_,  "10",   0, "11",   0,  2,     "01"  },
{  L_,  "11",   0, "11",   0,  2,     "00"  },
{
    L_,  "11110010 10110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  30,
         "00000001 10001100 11001100 11001100 11001100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  30,
         "11111011 00101110 11101110 11101110 11101111"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
        "111100110 11111000 01111000 01111000 0111101",    0,  30,
         "11110010 01001000 10001000 10001000 10001010"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  30,
         "11110010 01001100 11001100 11001100 11001100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  31,
         "00000001 01001100 11001100 11001100 11001100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  31,
         "11111011 01101110 11101110 11101110 11101111"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  31,
         "11110010 11001000 10001000 10001000 10001010"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  31,
         "11110010 11001100 11001100 11001100 11001100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  32,
         "00000000 11001100 11001100 11001100 11001100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  32,
         "11111010 11101110 11101110 11101110 11101111"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  32,
         "11110001 11001000 10001000 10001000 10001010"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  32,
         "11110001 11001100 11001100 11001100 11001100"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  33,
         "00000001 11001100 11001100 11001100 11001100"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110001",   0,
          "1111011 11011110 00011110 00011110 000111101",  1,  33,
         "11111001 11101110 11101110 11101110 11101111"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  33,
         "11110011 11001000 10001000 10001000 10001010"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
         "11110111 11111100 00111100 00111100 00111101",   1,  33,
         "11110011 11001100 11001100 11001100 11001100"
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  62,
"010 00001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  62,
"010 00001110 11101110 11101110 11101110 11101110 11101110 11101110 11101110",
},
{
    L_,
 "010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0110 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  62,
 "010 01001000 10001000 10001000 10001000 10001000 10001000 10001000 10001001",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  62,
"010 01001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  63,
"010 01001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  63,
"010 01001110 11101110 11101110 11101110 11101110 11101110 11101110 11101110",
},
{
    L_,
 "010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  63,
 "010 11001000 10001000 10001000 10001000 10001000 10001000 10001000 10001001",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  63,
"010 11001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  64,
"010 11001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  64,
"010 11001110 11101110 11101110 11101110 11101110 11101110 11101110 11101110",
},
{
    L_,
 "000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  64,
 "001 11001000 10001000 10001000 10001000 10001000 10001000 10001000 10001001",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  64,
"001 11001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  65,
"001 11001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "11 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  65,
"001 11001110 11101110 11101110 11101110 11101110 11101110 11101110 11101110",
},
{
    L_,
 "000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  65,
 "011 11001000 10001000 10001000 10001000 10001000 10001000 10001000 10001001",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"111 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  65,
"011 11001100 11001100 11001100 11001100 11001100 11001100 11001100 11001100",
},
};
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *DSTR      = DATA[i].d_dstBitstring_p;
          const int   DI        = DATA[i].d_dstIndex;
          const char *SSTR      = DATA[i].d_srcBitstring_p;
          const int   SI        = DATA[i].d_srcIndex;
          const int   NB        = DATA[i].d_numBits;
          const char *RESULT    = DATA[i].d_result_p;

          if (veryVerbose) {
              P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
          }

          int dst[MAX_ARRAY_SIZE] = { 0 };
          int src[MAX_ARRAY_SIZE] = { 0 };
          int result[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(dst, 0, DSTR);
          populateBitstring(src, 0, SSTR);
          populateBitstring(result, 0, RESULT);

          if (veryVerbose) {
              BSU::print(bsl::cout, dst, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, src, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, result, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          BSU::xorEqual(dst, DI, src, SI, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, result, DI, NB));
      }
    } break;
    case 17: {
      // --------------------------------------------------------------------
      // TESTING 'orEqual'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void orEqual(int       *dstBitstring,
      //                int        dstIndex,
      //                const int *srcBitstring,
      //                int        srcIndex,
      //                int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'orEqual'"
                             << "\n=================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_dstBitstring_p;
          int         d_dstIndex;
          const char *d_srcBitstring_p;
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
{  L_,  "01",   1, "0",    0,  1,     "01"  },
{  L_,  "11",   1, "0",    0,  1,     "11"  },
{  L_,  "00",   1, "1",    0,  1,     "10"  },
{  L_,  "10",   1, "1",    0,  1,     "10"  },
{  L_,  "10",   1, "01",   1,  1,     "10"  },
{  L_,  "01",   1, "11",   1,  1,     "11"  },
{  L_,  "10",   1, "01",   1,  1,     "10"  },
{  L_,  "00",   1, "11",   1,  1,     "10"  },
{  L_,  "11",   1, "11",   1,  1,     "11"  },
{  L_,  "11",   0, "01",   0,  2,     "11"  },
{  L_,  "11",   0, "00",   0,  2,     "11"  },
{  L_,  "11",   0, "10",   0,  2,     "11"  },
{  L_,  "10",   0, "00",   0,  2,     "10"  },
{  L_,  "01",   0, "11",   0,  2,     "11"  },
{  L_,  "10",   0, "10",   0,  2,     "10"  },
{  L_,  "10",   0, "11",   0,  2,     "11"  },
{  L_,  "11",   0, "11",   0,  2,     "11"  },
{
    L_,  "11110010 10110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  30,
         "11110010 10111100 11111100 11111100 11111100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  30,
         "11111111 01111110 11111110 11111110 11111111"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
        "111100110 11111000 01111000 01111000 0111101",    0,  30,
         "11110010 01111000 11111000 11111000 11111010"
},
{
    L_,  "11110010 01110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  30,
         "11110010 01111100 11111100 11111100 11111100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  31,
         "11110010 01111100 11111100 11111100 11111100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  31,
         "11111111 01111110 11111110 11111110 11111111"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  31,
         "11110010 11111000 11111000 11111000 11111010"
},
{
    L_,  "11110010 01110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  31,
         "11110010 11111100 11111100 11111100 11111100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  32,
         "11110010 11111100 11111100 11111100 11111100"
},
{
    L_,  "11110010 00110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  32,
         "11111110 11111110 11111110 11111110 11111111"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  32,
         "11110001 11111000 11111000 11111000 11111010"
},
{
    L_,  "11110000 01110000 11110000 11110000 11110000",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  32,
         "11110001 11111100 11111100 11111100 11111100"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  33,
         "11110001 11111100 11111100 11111100 11111100"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110001",   0,
          "1111011 11011110 00011110 00011110 000111101",  1,  33,
         "11111101 11111110 11111110 11111110 11111111"
},
{
    L_,  "11110000 00110000 11110000 11110000 11110000",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  33,
         "11110011 11111000 11111000 11111000 11111010"
},
{
    L_,  "11110000 01110000 11110000 11110000 11110000",   1,
         "11110111 11111100 00111100 00111100 00111101",   1,  33,
         "11110011 11111100 11111100 11111100 11111100"
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  62,
"010 00111100 11111100 11111100 11111100 11111100 11111100 11111100 11111100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  62,
"010 00111110 11111110 11111110 11111110 11111110 11111110 11111110 11111110",
},
{
    L_,
 "010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0110 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  62,
 "010 01111000 11111000 11111000 11111000 11111000 11111000 11111000 11111001",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  62,
"010 01111100 11111100 11111100 11111100 11111100 11111100 11111100 11111101",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  63,
"010 01111100 11111100 11111100 11111100 11111100 11111100 11111100 11111100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  63,
"010 01111110 11111110 11111110 11111110 11111110 11111110 11111110 11111110",
},
{
    L_,
 "010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  63,
 "010 11111000 11111000 11111000 11111000 11111000 11111000 11111000 11111001",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  63,
"001 11111100 11111100 11111100 11111100 11111100 11111100 11111100 11111101",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  64,
"010 11111100 11111100 11111100 11111100 11111100 11111100 11111100 11111100",
},
{
    L_,
"010 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  64,
"010 11111110 11111110 11111110 11111110 11111110 11111110 11111110 11111110",
},
{
    L_,
 "000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  64,
 "001 11111000 11111000 11111000 11111000 11111000 11111000 11111000 11111001",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"111 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  64,
"011 11111100 11111100 11111100 11111100 11111100 11111100 11111100 11111101",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  65,
"001 11111100 11111100 11111100 11111100 11111100 11111100 11111100 11111100",
},
{
    L_,
"000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "11 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  65,
"001 11111110 11111110 11111110 11111110 11111110 11111110 11111110 11111110",
},
{
    L_,
 "000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  65,
 "011 11111000 11111000 11111000 11111000 11111000 11111000 11111000 11111001",
},
{
    L_,
"0000 00110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"1111 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  65,
"0111 11111100 11111100 11111100 11111100 11111100 11111100 11111100 11111101",
},
};
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *DSTR      = DATA[i].d_dstBitstring_p;
          const int   DI        = DATA[i].d_dstIndex;
          const char *SSTR      = DATA[i].d_srcBitstring_p;
          const int   SI        = DATA[i].d_srcIndex;
          const int   NB        = DATA[i].d_numBits;
          const char *RESULT    = DATA[i].d_result_p;

          if (veryVerbose) {
              P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
          }

          int dst[MAX_ARRAY_SIZE] = { 0 };
          int src[MAX_ARRAY_SIZE] = { 0 };
          int result[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(dst, 0, DSTR);
          populateBitstring(src, 0, SSTR);
          populateBitstring(result, 0, RESULT);

          if (veryVerbose) {
              BSU::print(bsl::cout, dst, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, src, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, result, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          BSU::orEqual(dst, DI, src, SI, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, result, DI, NB));
      }
    } break;
    case 16: {
      // --------------------------------------------------------------------
      // TESTING 'minusEqual'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void minusEqual(int       *dstBitstring,
      //                   int        dstIndex,
      //                   const int *srcBitstring,
      //                   int        srcIndex,
      //                   int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'minusEqual'"
                             << "\n====================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_dstBitstring_p;
          int         d_dstIndex;
          const char *d_srcBitstring_p;
          int         d_srcIndex;
          int         d_numBits;
          const char *d_result_p;
      } DATA [] = {
// Line dst  dIdx  src  sIdx  NB   Result
// ---- ---  ----  ---- ----  --   ------
{  L_,  "",     0, "",     0,  0,     ""    },
{  L_,  "0",    0, "0",    0,  1,     "0"   },
{  L_,  "0",    0, "1",    0,  1,     "0"   },
{  L_,  "1",    0, "0",    0,  1,     "1"   },
{  L_,  "1",    0, "1",    0,  1,     "0"   },
{  L_,  "00",   1, "0",    0,  1,     "00"  },
{  L_,  "10",   1, "0",    0,  1,     "10"  },
{  L_,  "00",   1, "1",    0,  1,     "00"  },
{  L_,  "10",   1, "1",    0,  1,     "00"  },
{  L_,  "11",   1, "01",   1,  1,     "11"  },
{  L_,  "01",   1, "11",   1,  1,     "01"  },
{  L_,  "11",   1, "01",   1,  1,     "11"  },
{  L_,  "01",   1, "11",   1,  1,     "01"  },
{  L_,  "11",   1, "11",   1,  1,     "01"  },
{  L_,  "11",   0, "01",   0,  2,     "10"  },
{  L_,  "11",   0, "00",   0,  2,     "11"  },
{  L_,  "11",   0, "10",   0,  2,     "01"  },
{  L_,  "10",   0, "00",   0,  2,     "10"  },
{  L_,  "01",   0, "11",   0,  2,     "00"  },
{  L_,  "10",   0, "10",   0,  2,     "00"  },
{  L_,  "10",   0, "11",   0,  2,     "00"  },
{  L_,  "11",   0, "11",   0,  2,     "00"  },
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  30,
         "11110010 11000000 11000000 11000000 11000000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   0,
          "1111001 10011110 00011110 00011110 000111101",  1,  30,
         "11110000 11100000 11100000 11100000 11100001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
        "111100110 11111000 01111000 01111000 0111101",    0,  30,
         "11100010 10000000 10000000 10000000 10000001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  30,
         "11110010 10000000 11000000 11000000 11000001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  31,
         "11110010 10000000 11000000 11000000 11000000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  31,
         "11110000 10100000 11100000 11100000 11100001"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110001",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  31,
         "11100011 00000000 10000000 10000000 10000001"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  32,
         "11110011 00000000 11000000 11000000 11000000"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110001",   0,
          "1111001 11011110 00011110 00011110 000111101",  1,  32,
         "11110001 00100000 11100000 11100000 11100001"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110001",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  32,
         "11100010 00000000 10000000 10000000 10000001"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110001",   1,
         "11110011 11111100 00111100 00111100 00111101",   1,  32,
         "11110010 00000000 11000000 11000000 11000001"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110000",   0,
         "11110011 11111100 00111100 00111100 00111100",   0,  33,
         "11110010 00000000 11000000 11000000 11000000"
},
{
    L_,  "11110011 11110000 11110000 11110000 11110001",   0,
          "1111011 11011110 00011110 00011110 000111101",  1,  33,
         "11110010 00100000 11100000 11100000 11100001"
},
{
    L_,  "11110111 11110000 11110000 11110000 11110001",   1,
        "111100111 11111000 01111000 01111000 0111101",    0,  33,
         "11100100 00000000 10000000 10000000 10000001"
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  62,
"010 11000000 11000000 11000000 11000000 11000000 11000000 11000000 11000000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  62,
"010 11000000 11100000 11100000 11100000 11100000 11100000 11100000 11100000",
},
{
    L_,
 "010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0110 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  62,
 "010 10000000 10000000 10000000 10000000 10000000 10000000 10000000 10000001",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  62,
"010 10000000 11000000 11000000 11000000 11000000 11000000 11000000 11000001",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  63,
"010 10000000 11000000 11000000 11000000 11000000 11000000 11000000 11000000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  63,
"010 10000000 11100000 11100000 11100000 11100000 11100000 11100000 11100000",
},
{
    L_,
 "011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  63,
 "011 00000000 10000000 10000000 10000000 10000000 10000000 10000000 10000001",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  63,
"011 00000000 11000000 11000000 11000000 11000000 11000000 11000000 11000001",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  64,
"011 00000000 11000000 11000000 11000000 11000000 11000000 11000000 11000000",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "11 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  64,
"011 00000000 11100000 11100000 11100000 11100000 11100000 11100000 11100000",
},
{
    L_,
 "011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  64,
 "010 00000000 10000000 10000000 10000000 10000000 10000000 10000000 10000001",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  64,
"010 00000000 11000000 11000000 11000000 11000000 11000000 11000000 11000001",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  65,
"010 00000000 11000000 11000000 11000000 11000000 11000000 11000000 11000000",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "11 11111110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  65,
"010 00000000 11100000 11100000 11100000 11100000 11100000 11100000 11100000",
},
{
    L_,
 "111 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"0111 11111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  65,
 "100 00000000 10000000 10000000 10000000 10000000 10000000 10000000 10000001",
},
{
    L_,
"111 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"111 11111100 00111100 00111100 00111100 00111100 00111100 00111100 00111101",
    1,  65,
"100 00000000 11000000 11000000 11000000 11000000 11000000 11000000 11000001",
},
};
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *DSTR      = DATA[i].d_dstBitstring_p;
          const int   DI        = DATA[i].d_dstIndex;
          const char *SSTR      = DATA[i].d_srcBitstring_p;
          const int   SI        = DATA[i].d_srcIndex;
          const int   NB        = DATA[i].d_numBits;
          const char *RESULT    = DATA[i].d_result_p;

          if (veryVerbose) {
              P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
          }

          int dst[MAX_ARRAY_SIZE] = { 0 };
          int src[MAX_ARRAY_SIZE] = { 0 };
          int result[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(dst, 0, DSTR);
          populateBitstring(src, 0, SSTR);
          populateBitstring(result, 0, RESULT);

          if (veryVerbose) {
              BSU::print(bsl::cout, dst, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, src, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, result, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          BSU::minusEqual(dst, DI, src, SI, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, result, DI, NB));
      }
    } break;
    case 15: {
      // --------------------------------------------------------------------
      // TESTING 'andEqual'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void andEqual(int       *dstBitstring,
      //                 int        dstIndex,
      //                 const int *srcBitstring,
      //                 int        srcIndex,
      //                 int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'andEqual'"
                             << "\n==================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_dstBitstring_p;
          int         d_dstIndex;
          const char *d_srcBitstring_p;
          int         d_srcIndex;
          int         d_numBits;
          const char *d_result_p;
      } DATA [] = {
// Line dst  dIdx  src  sIdx  NB   Result
// ---- ---  ----  ---- ----  --   ------
{  L_,  "",     0, "",     0,  0,     ""    },
{  L_,  "0",    0, "0",    0,  1,     "0"   },
{  L_,  "0",    0, "1",    0,  1,     "0"   },
{  L_,  "1",    0, "0",    0,  1,     "0"   },
{  L_,  "1",    0, "1",    0,  1,     "1"   },
{  L_,  "01",   1, "0",    0,  1,     "01"  },
{  L_,  "11",   1, "0",    0,  1,     "01"  },
{  L_,  "01",   1, "1",    0,  1,     "01"  },
{  L_,  "11",   1, "1",    0,  1,     "11"  },
{  L_,  "11",   1, "00",   1,  1,     "01"  },
{  L_,  "01",   1, "10",   1,  1,     "01"  },
{  L_,  "11",   1, "00",   1,  1,     "01"  },
{  L_,  "01",   1, "11",   1,  1,     "01"  },
{  L_,  "11",   1, "10",   1,  1,     "11"  },
{  L_,  "10",   0, "01",   0,  2,     "00"  },
{  L_,  "00",   0, "10",   0,  2,     "00"  },
{  L_,  "10",   0, "00",   0,  2,     "00"  },
{  L_,  "01",   0, "11",   0,  2,     "01"  },
{  L_,  "10",   0, "10",   0,  2,     "10"  },
{  L_,  "10",   0, "11",   0,  2,     "10"  },
{  L_,  "11",   0, "11",   0,  2,     "11"  },
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
         "11110011 00111100 00111100 00111100 00111100",   0,  30,
         "11110010 11110000 00110000 00110000 00110000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   0,
          "1111001 10011110 00011110 00011110 000111101",  1,  30,
         "11110000 10010000 00010000 00010000 00010000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
        "111100110 01111000 01111000 01111000 0111101",    0,  30,
         "11100010 11110000 01110000 01110000 01110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
         "11110011 00111100 00111100 00111100 00111100",   1,  30,
         "11110010 10110000 00110000 00110000 00110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
         "11110011 00111100 00111100 00111100 00111100",   0,  31,
         "11110010 00110000 00110000 00110000 00110000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
          "1111001 00011110 00011110 00011110 000111100",  1,  31,
         "11110010 10010000 00010000 00010000 00010000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
        "111100110 01111000 01111000 01111000 0111101",    0,  31,
         "11100010 01110000 01110000 01110000 01110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
         "11110011 00111100 00111100 00111100 00111100",   1,  31,
         "11110010 00110000 00110000 00110000 00110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   0,
         "11110011 00111100 00111100 00111100 00111100",   0,  32,
         "11110010 00110000 00110000 00110000 00110000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
          "1111001 00011110 00011110 00011110 000111100",  1,  32,
         "11110010 00010000 00010000 00010000 00010000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
        "111100110 01111000 01111000 01111000 0111101",    0,  32,
         "11100010 01110000 01110000 01110000 01110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   1,
         "11110011 00111100 00111100 00111100 00111100",   1,  32,
         "11110010 00110000 00110000 00110000 00110001"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110001",   0,
         "11110011 00111100 00111100 00111100 00111100",   0,  33,
         "11110010 00110000 00110000 00110000 00110000"
},
{
    L_,  "11110010 11110000 11110000 11110000 11110000",   0,
          "1111001 00011110 00011110 00011110 000111100",  1,  33,
         "11110010 00010000 00010000 00010000 00010000"
},
{
    L_,  "11110111 11110000 11110000 11110000 11110001",   1,
        "111100000 11111000 01111000 01111000 0111101",    0,  33,
         "11100100 11110000 01110000 01110000 01110001"
},
{
    L_,  "11110111 11110000 11110000 11110000 11110001",   1,
         "11110001 00111100 00111100 00111100 00111100",   1,  33,
         "11110101 00110000 00110000 00110000 00110001"
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  62,
"010 11110000 00110000 00110000 00110000 00110000 00110000 00110000 00110000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 10011110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  62,
"010 11010000 00010000 00010000 00010000 00010000 00010000 00010000 00010000",
},
{
    L_,
 "010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    1,
"0110 00111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  62,
 "010 10110000 01110000 01110000 01110000 01110000 01110000 01110000 01110000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    1,  62,
"010 10110000 00110000 00110000 00110000 00110000 00110000 00110000 00110001",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  63,
"010 10110000 00110000 00110000 00110000 00110000 00110000 00110000 00110000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "01 10011110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  63,
"010 10010000 00010000 00010000 00010000 00010000 00010000 00010000 00010000",
},
{
    L_,
 "010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    1,
"0110 00111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  63,
 "010 00110000 01110000 01110000 01110000 01110000 01110000 01110000 01110000",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"011 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    1,  63,
"010 00110000 00110000 00110000 00110000 00110000 00110000 00110000 00110001",
},
{
    L_,
"010 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"011 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  64,
"010 00110000 00110000 00110000 00110000 00110000 00110000 00110000 00110000",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "00 00011110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  64,
"011 00010000 00010000 00010000 00010000 00010000 00010000 00010000 00010000",
},
{
    L_,
 "011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    1,
"0100 00111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  64,
 "010 00110000 01110000 01110000 01110000 01110000 01110000 01110000 01110000",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"000 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    1,  64,
"010 00110000 00110000 00110000 00110000 00110000 00110000 00110000 00110001",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
"000 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    0,  65,
"010 00110000 00110000 00110000 00110000 00110000 00110000 00110000 00110000",
},
{
    L_,
"011 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    0,
 "00 00011110 00011110 00011110 00011110 00011110 00011110 00011110 000111100",
    1,  65,
"010 00010000 00010000 00010000 00010000 00010000 00010000 00010000 00010000",
},
{
    L_,
 "111 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
    1,
"0000 00111000 01111000 01111000 01111000 01111000 01111000 01111000 0111100",
    0,  65,
 "100 00110000 01110000 01110000 01110000 01110000 01110000 01110000 01110000",
},
{
    L_,
"111 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110001",
    1,
"000 00111100 00111100 00111100 00111100 00111100 00111100 00111100 00111100",
    1,  65,
"100 00110000 00110000 00110000 00110000 00110000 00110000 00110000 00110001",
},
};
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *DSTR      = DATA[i].d_dstBitstring_p;
          const int   DI        = DATA[i].d_dstIndex;
          const char *SSTR      = DATA[i].d_srcBitstring_p;
          const int   SI        = DATA[i].d_srcIndex;
          const int   NB        = DATA[i].d_numBits;
          const char *RESULT    = DATA[i].d_result_p;

          if (veryVerbose) {
              P_(LINE) P_(DSTR) P_(DI) P_(SSTR) P_(SI) P_(NB) P(RESULT)
          }

          int dst[MAX_ARRAY_SIZE] = { 0 };
          int src[MAX_ARRAY_SIZE] = { 0 };
          int result[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(dst, 0, DSTR);
          populateBitstring(src, 0, SSTR);
          populateBitstring(result, 0, RESULT);

          if (veryVerbose) {
              BSU::print(bsl::cout, dst, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, src, MAX_ARRAY_SIZE * BITS_PER_INT);
              BSU::print(bsl::cout, result, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          BSU::andEqual(dst, DI, src, SI, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, result, DI, NB));
      }
    } break;
    case 14: {
      // --------------------------------------------------------------------
      // TESTING 'num0' and 'num1'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   int num0(const int *bitstring, int index, int numBits);
      //   int num1(const int *bitstring, int index, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'num0' and 'num1'"
                             << "\n========================="
                             << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_numBits;
          int         d_num0;
          int         d_num1;
      } DATA [] = {
   // Line  InputString                         Index NumBits    num0  num1
   // ----  -----------                         ----- -------    ----  ----
     { L_,  "",                                    0,      0,      0,     0  },
     { L_,  "0",                                   0,      0,      0,     0  },
     { L_,  "0",                                   0,      1,      1,     0  },
     { L_,  "1",                                   0,      0,      0,     0  },
     { L_,  "1",                                   0,      1,      0,     1  },

     { L_,  "10",                                  1,      1,      0,     1  },
     { L_,  "10",                                  0,      2,      1,     1  },

     { L_,  "110",                                 0,      3,      1,     2  },
     { L_,  "110",                                 1,      2,      0,     2  },

     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   0,      4,      4,     0 },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   4,      4,      0,     4  },

     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   2,     31,     15,    16 },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   2,     32,     16,    16 },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   2,     33,     17,    16 },

     { L_,
     "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
                                                   2,     31,     16,    15  },
     { L_,
     "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
                                                   2,     32,     16,    16  },
     { L_,
     "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
                                                   2,     33,     16,    17  },

     { L_,
     "11010 11110000 11110000 11110000 11110000 11110000 11110000 11110000"
     " 11110000",
                                                   1,     63,     31,    32  },
     { L_,
     "11010 11110000 11110000 11110000 11110000 11110000 11110000 11110000"
     " 11110000",
                                                   1,     64,     32,    32  },
     { L_,
     "11010 11110000 11110000 11110000 11110000 11110000 11110000 11110000"
     " 11110000",
                                                   1,     65,     32,    33  },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   INDEX     = DATA[i].d_index;
          const int   NUM_BITS  = DATA[i].d_numBits;
          const int   NUM_SET0  = DATA[i].d_num0;
          const int   NUM_SET1  = DATA[i].d_num1;

          if (veryVerbose) {
              P_(LINE) P_(INPUT_STR) P_(INDEX) P_(NUM_BITS)
              P_(NUM_SET0) P(NUM_SET1)
          }

          int bits[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(bits, 0, INPUT_STR);

          if (veryVerbose) {
              BSU::print(bsl::cout, bits, MAX_ARRAY_SIZE * BITS_PER_INT);
          }

          LOOP2_ASSERT(LINE, NUM_SET0,
                       NUM_SET0 == BSU::num0(bits, INDEX, NUM_BITS));
          LOOP2_ASSERT(LINE, NUM_SET1,
                       NUM_SET1 == BSU::num1(bits, INDEX, NUM_BITS));
      }
    } break;
    case 13: {
      // --------------------------------------------------------------------
      // TESTING 'print'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   bsl::ostream& print(bsl::ostream&  stream,
      //                       const int     *bitstring,
      //                       int            numBits,
      //                       int            level,
      //                       int            spl,
      //                       int            unitsPerLine,
      //                       int            bitsPerUnit)
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'print'"
                             << "\n===============" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_numBits;
          int         d_level;
          int         d_spl;
          int         d_unitsPerLine;
          int         d_bitsPerUnit;
          const char *d_expString_p;
      } DATA [] = {
          {
              L_,
              "",
              0,
              1,
              4,
              4,
              8,
              "    [\n"
              "    ]\n"
          },
          {
              L_,
              "",
              0,
              1,
              4,
              1,
              2,
              "    [\n"
              "    ]\n"
          },
          {
              L_,
              "",
              0,
              -1,
              4,
              1,
              2,
              "[\n"
              "    ]\n"
          },
          {
              L_,
              "",
              0,
              1,
              -4,
              1,
              2,
              "    [ ]"
          },
          {
              L_,
              "",
              0,
              -1,
              -4,
              1,
              2,
              "[ ]"
          },

          {
              L_,
              "",
              0,
              0,
              0,
              4,
              8,
              "[\n"
              "]\n"
          },
          {
              L_,
              "",
              0,
              0,
              4,
              4,
              8,
              "[\n"
              "]\n"
          },
          {
              L_,
              "",
              0,
              1,
              0,
              4,
              8,
              "[\n"
              "]\n"
          },
          {
              L_,
              "",
              0,
              0,
              0,
              4,
              8,
              "[\n"
              "]\n"
          },

          {
              L_,
              "1",
              1,
              1,
              4,
              4,
              8,
              "    [\n"
              "        1\n"
              "    ]\n"
          },
          {
              L_,
              "1",
              1,
              -1,
              4,
              4,
              8,
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
              4,
              8,
              "    [ 1 ]"
          },
          {
              L_,
              "1",
              1,
              -1,
              -4,
              4,
              8,
              "[ 1 ]"
          },
          {
              L_,
              "1",
              1,
              0,
              4,
              4,
              8,
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
              4,
              8,
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
              4,
              8,
              "[\n"
              "1\n"
              "]\n"
          },

          {
              L_,
              "01",
              2,
              1,
              4,
              4,
              1,
              "    [\n"
              "        1 0\n"
              "    ]\n"
          },
          {
              L_,
              "01",
              2,
              -1,
              4,
              4,
              1,
              "[\n"
              "        1 0\n"
              "    ]\n"
          },
          {
              L_,
              "01",
              2,
              1,
              -4,
              4,
              1,
              "    [ 1 0 ]"
          },
          {
              L_,
              "01",
              2,
              -1,
              -4,
              4,
              1,
              "[ 1 0 ]"
          },
          {
              L_,
              "01",
              2,
              0,
              4,
              4,
              1,
              "[\n"
              "    1 0\n"
              "]\n"
          },
          {
              L_,
              "01",
              2,
              1,
              0,
              4,
              1,
              "[\n"
              "1 0\n"
              "]\n"
          },
          {
              L_,
              "01",
              2,
              0,
              0,
              4,
              1,
              "[\n"
              "1 0\n"
              "]\n"
          },

          {
              L_,
              "11001",
              2,
              1,
              4,
              4,
              1,
              "    [\n"
              "        1 0\n"
              "    ]\n"
          },
          {
              L_,
              "11001",
              2,
              -1,
              4,
              4,
              1,
              "[\n"
              "        1 0\n"
              "    ]\n"
          },
          {
              L_,
              "11001",
              2,
              1,
              -4,
              4,
              1,
              "    [ 1 0 ]"
          },
          {
              L_,
              "11001",
              2,
              -1,
              -4,
              4,
              1,
              "[ 1 0 ]"
          },
          {
              L_,
              "11001",
              2,
              0,
              4,
              4,
              1,
              "[\n"
              "    1 0\n"
              "]\n"
          },
          {
              L_,
              "11001",
              2,
              1,
              0,
              4,
              1,
              "[\n"
              "1 0\n"
              "]\n"
          },
          {
              L_,
              "11001",
              2,
              0,
              0,
              4,
              1,
              "[\n"
              "1 0\n"
              "]\n"
          },

          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              1,
              4,
              4,
              4,
              "    [\n"
              "        0011 1100 0011 1100\n"
              "        0011 1100 0011 1100\n"
              "        0\n"
              "    ]\n"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              -1,
              4,
              4,
              4,
              "[\n"
              "        0011 1100 0011 1100\n"
              "        0011 1100 0011 1100\n"
              "        0\n"
              "    ]\n"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              1,
              -4,
              4,
              4,
              "    [ 0011 1100 0011 1100 0011 1100 0011 1100 0 ]"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              -1,
              -4,
              4,
              4,
              "[ 0011 1100 0011 1100 0011 1100 0011 1100 0 ]"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              0,
              4,
              4,
              4,
              "[\n"
              "    0011 1100 0011 1100\n"
              "    0011 1100 0011 1100\n"
              "    0\n"
              "]\n"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              1,
              0,
              4,
              4,
              "[\n"
              "0011 1100 0011 1100\n"
              "0011 1100 0011 1100\n"
              "0\n"
              "]\n"
          },
          {
              L_,
              "00111100 00111100 00111100 00111100 00111100",
              33,
              0,
              0,
              4,
              4,
              "[\n"
              "0011 1100 0011 1100\n"
              "0011 1100 0011 1100\n"
              "0\n"
              "]\n"
          },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE  = DATA[i].d_line;
          const char *INPUT = DATA[i].d_inputString_p;
          int         NB    = DATA[i].d_numBits;
          int         UPL   = DATA[i].d_unitsPerLine;
          int         BPU   = DATA[i].d_bitsPerUnit;
          int         LEVEL = DATA[i].d_level;
          int         SPL   = DATA[i].d_spl;
          const char *EXP   = DATA[i].d_expString_p;

          if (veryVerbose) {
              P(LINE) P_(INPUT) P_(UPL) P_(BPU)
              P_(LEVEL) P_(SPL) P_(EXP)
          }

          bsl::ostringstream stream;
          int bitstring[MAX_ARRAY_SIZE] = { 0 };

          populateBitstring(bitstring, 0, INPUT);
          BSU::print(stream,
                     bitstring,
                     NB,
                     LEVEL,
                     SPL,
                     UPL,
                     BPU);
          int rc = bsl::strcmp(stream.str().c_str(), EXP);
          LOOP3_ASSERT(LINE, EXP, stream.str(),
                       0 == rc);
          if (rc) {
              int                len = bsl::strlen(EXP);
              const bsl::string& s   = stream.str();
              for (int j = 0; j < len; ++j) {
                  if (EXP[j] != s[j]) {
                      P_(j) P_(EXP[j]) P_(s[j])
                  }
              }
          }
        }
    } break;
    case 12: {
      // --------------------------------------------------------------------
      // TESTING 'swapRaw'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void swapRaw(const int *lhsBitstring,
      //                 int        lhsIndex,
      //                 const int *rhsBitstring,
      //                 int        rhsIndex,
      //                 int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'swapRaw'"
                             << "\n=================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_lhsString_p;
          int         d_lhsIndex;
          const char *d_rhsString_p;
          int         d_rhsIndex;
          int         d_numBits;
      } DATA [] = {
    // Line  LhsString          LI  RhsString          RI    NB
    // ----  ---------          --  ---------          --    --
    {   L_,  "",                0,  "",                0,    0               },

    {   L_,  "0",               0,  "1",               0,    0               },
    {   L_,  "0",               0,  "1",               0,    1               },

    {   L_,  "01",              0,  "10",              0,    0               },
    {   L_,  "01",              0,  "10",              0,    1               },
    {   L_,  "01",              0,  "10",              1,    1               },
    {   L_,  "01",              1,  "10",              0,    1               },
    {   L_,  "01",              1,  "10",              1,    1               },

    {   L_,  "11111",           0,  "00000",           0,    0               },
    {   L_,  "11111",           0,  "00000",           0,    2               },
    {   L_,  "11111",           1,  "00000",           0,    2               },
    {   L_,  "11111",           0,  "00000",           1,    2               },

    { L_,  "00001111 00001111 00001111 00001111",      0,
           "11110000 11110000 11110000 11110000",      0,  0 },
    { L_,  "00001111 00001111 00001111 00001111",      0,
           "11110000 11110000 11110000 11110000",      0,  31 },
    { L_,  "00001111 00001111 00001111 00001111",      1,
           "11110000 11110000 11110000 11110000",      0,  31 },
    { L_,  "00001111 00001111 00001111 00001111",      0,
           "11110000 11110000 11110000 11110000",      1,  31 },
    { L_,  "00001111 00001111 00001111 00001111",      1,
           "11110000 11110000 11110000 11110000",      1,  31 },
    { L_,  "00001111 00001111 00001111 00001111",      0,
           "11110000 11110000 11110000 11110000",      0,  32 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   0
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   32
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   32
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   32
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   32
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   5,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   10,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   10,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   5,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   30,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   30,
   35
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 }
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE  = DATA[i].d_line;
          const char *LSTR  = DATA[i].d_lhsString_p;
          const int   LI    = DATA[i].d_lhsIndex;
          const char *RSTR  = DATA[i].d_rhsString_p;
          const int   RI    = DATA[i].d_rhsIndex;
          const int   NB    = DATA[i].d_numBits;

          if (veryVerbose) {
              P(LINE) P_(LSTR) P(LI) P_(RSTR) P_(RI) P(NB)
          }

          int lhs[MAX_ARRAY_SIZE] = { 0 };
          int lctrl[MAX_ARRAY_SIZE] = { 0 };
          int rhs[MAX_ARRAY_SIZE] = { 0 };
          int rctrl[MAX_ARRAY_SIZE] = { 0 };
          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

          populateBitstring(lhs, 0, LSTR);
          bsl::memcpy(lctrl, lhs, sizeof lctrl);
          populateBitstring(rhs, 0, RSTR);
          bsl::memcpy(rctrl, rhs, sizeof rctrl);

          BSU::swapRaw(lhs, LI, rhs, RI, NB);

          LOOP_ASSERT(LINE, checkControl(lhs, 0, lctrl, 0,
                                         MAX_NUM_BITS, LI, LI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(lhs, LI, rctrl, RI, NB));

          LOOP_ASSERT(LINE, checkControl(rhs, 0, rctrl, 0,
                                         MAX_NUM_BITS, RI, RI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(rhs, RI, lctrl, LI, NB));
      }
    } break;
    case 11: {
      // --------------------------------------------------------------------
      // TESTING 'removeAndFill'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void removeAndFill(int  *bitstring,
      //                      int   length,
      //                      int   index,
      //                      bool  value,
      //                      int   numBits);
      //   void removeAndFill0(int *bitstring, int len, int idx, int numBits);
      //   void removeAndFill1(int *bitstring, int len, int idx, int numBits);
      //   void remove(int *bitstring, int len, int idx, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'removeAndFill'"
                             << "\n=======================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_numBits;
      } DATA [] = {
    // Line  InputString                                              DI  NB
    // ----  -----------                                              --  --
     { L_,   "",                                                      0,  0  },

     { L_,   "0",                                                     0,  0  },
     { L_,   "0",                                                     0,  1  },

     { L_, "11110000 11110000 11110000 11110000",                     0,  0  },
     { L_, "11110000 11110000 11110000 11110000",                     0,  1  },
     { L_, "11110000 11110000 11110000 11110000",                     0, 32  },

     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0,  0  },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0,  1  },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0, 32  },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0, 33  },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0, 63  },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                      0, 64  },

     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     31,  0  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     31,  1  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     31, 33  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     31, 34  },

     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     32,  0  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     32,  1  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     32, 32  },
     { L_,
"0000 11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                                     32, 33  },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE  = DATA[i].d_line;
          const char *STR   = DATA[i].d_inputString_p;
          const int   LEN   = bsl::strlen(STR);
          const int   DI    = DATA[i].d_index;
          const int   NB    = DATA[i].d_numBits;

          if (veryVerbose) {
              P(LINE) P_(STR) P_(DI) P(NB)
          }

          int actual0[MAX_ARRAY_SIZE] = { 0 };
          int actual1[MAX_ARRAY_SIZE] = { 0 };
          int actual[MAX_ARRAY_SIZE]  = { 0 };
          int actualRaw[MAX_ARRAY_SIZE] = { 0 };
          int control[MAX_ARRAY_SIZE] = { 0 };
          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

          populateBitstring(actual0, 0, STR);
          bsl::memcpy(actual1, actual0, sizeof control);
          bsl::memcpy(actual, actual0, sizeof control);
          bsl::memcpy(actualRaw, actual0, sizeof control);
          bsl::memcpy(control, actual0, sizeof control);

          BSU::removeAndFill0(actual0, LEN, DI, NB);
          BSU::removeAndFill1(actual1, LEN, DI, NB);
          BSU::remove(actualRaw, LEN, DI, NB);

          LOOP_ASSERT(LINE, BSU::areEqual(actual0, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actual0, DI,
                                             control, DI + NB, LEN - DI - NB));
          LOOP_ASSERT(LINE, !BSU::isAny1(actual0, LEN - NB, NB));

          LOOP_ASSERT(LINE, BSU::areEqual(actual1, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actual1, DI,
                                             control, DI + NB, LEN - DI - NB));
          LOOP_ASSERT(LINE, !BSU::isAny0(actual1, LEN - NB, NB));

          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, DI,
                                             control, DI + NB, LEN - DI - NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, LEN - NB,
                                             control, LEN - NB, NB));

          BSU::removeAndFill(actual, LEN, DI, false, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(actual, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actual, DI,
                                             control, DI + NB, LEN - DI - NB));
          LOOP_ASSERT(LINE, !BSU::isAny1(actual, LEN - NB, NB));

          populateBitstring(actual, 0, STR);

          BSU::removeAndFill(actual, LEN, DI, true, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(actual, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actual, DI,
                                             control, DI + NB, LEN - DI - NB));
          LOOP_ASSERT(LINE, !BSU::isAny0(actual, LEN - NB, NB));
      }
    } break;
    case 10: {
      // --------------------------------------------------------------------
      // TESTING 'insert'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void insert(int  *bitstring,
      //               int   length,
      //               int   index,
      //               bool  value,
      //               int   numBits);
      //   void insert0(int *bitstring, int length, int idx, int numBits);
      //   void insert1(int *bitstring, int length, int idx, int numBits);
      //   void insertRaw(int *bitstring, int len, int idx, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'insert'"
                             << "\n================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_numBits;
      } DATA [] = {
   // Line  InputString                                              DI  NB
   // ----  -----------                                              --  --
     { L_,   "",                                                      0,  0  },
     { L_,   "",                                                      0,  1  },
     { L_,   "",                                                      0, 31  },
     { L_,   "",                                                      0, 32  },
     { L_,   "",                                                      0, 33  },
     { L_,   "",                                                      0, 63  },
     { L_,   "",                                                      0, 64  },
     { L_,   "",                                                      0, 65  },

     { L_,   "0",                                                     0,  0  },
     { L_,   "0",                                                     0,  1  },
     { L_,   "0",                                                     0, 31  },
     { L_,   "0",                                                     0, 32  },
     { L_,   "0",                                                     0, 33  },
     { L_,   "0",                                                     0, 63  },
     { L_,   "0",                                                     0, 64  },
     { L_,   "0",                                                     0, 65  },

     { L_,   "0",                                                     1,  0  },
     { L_,   "0",                                                     1,  1  },
     { L_,   "0",                                                     1, 31  },
     { L_,   "0",                                                     1, 32  },
     { L_,   "0",                                                     1, 33  },
     { L_,   "0",                                                     1, 63  },
     { L_,   "0",                                                     1, 64  },
     { L_,   "0",                                                     1, 65  },

     { L_, "1110000111100001111000011110000",                         0,  0  },
     { L_, "1110000111100001111000011110000",                         0,  1  },
     { L_, "1110000111100001111000011110000",                         0, 32  },
     { L_, "1110000111100001111000011110000",                         0, 33  },
     { L_, "1110000111100001111000011110000",                         0, 64  },
     { L_, "1110000111100001111000011110000",                         0, 65  },

     { L_, "1110000111100001111000011110000",                        15,  0  },
     { L_, "1110000111100001111000011110000",                        15,  1  },
     { L_, "1110000111100001111000011110000",                        15, 32  },
     { L_, "1110000111100001111000011110000",                        15, 33  },
     { L_, "1110000111100001111000011110000",                        15, 64  },
     { L_, "1110000111100001111000011110000",                        15, 65  },

     { L_, "1110000111100001111000011110000",                        31,  0  },
     { L_, "1110000111100001111000011110000",                        31,  1  },
     { L_, "1110000111100001111000011110000",                        31, 32  },
     { L_, "1110000111100001111000011110000",                        31, 33  },
     { L_, "1110000111100001111000011110000",                        31, 64  },
     { L_, "1110000111100001111000011110000",                        31, 65  },

     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                      0,  0  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                      0,  1  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                      0, 32  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                      0, 33  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                      0, 34  },

     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     31,  0  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     31,  1  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     31, 32  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     31, 33  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     31, 34  },

     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     32,  0  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     32,  1  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     32, 32  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     32, 33  },
     { L_,
       "111000011110000111100001111000011110000111100001111000011110000",
                                                                     32, 34  },

      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 5;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE  = DATA[i].d_line;
          const char *STR   = DATA[i].d_inputString_p;
          const int   LEN   = bsl::strlen(STR);
          const int   DI    = DATA[i].d_index;
          const int   NB    = DATA[i].d_numBits;

          if (veryVerbose) {
              P(LINE) P_(STR) P_(DI) P(NB)
          }

          int actual0[MAX_ARRAY_SIZE] = { 0 };
          int actual1[MAX_ARRAY_SIZE] = { 0 };
          int actual[MAX_ARRAY_SIZE]  = { 0 };
          int actualRaw[MAX_ARRAY_SIZE] = { 0 };
          int control[MAX_ARRAY_SIZE] = { 0 };
          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

          populateBitstring(actual0, 0, STR);
          bsl::memcpy(actual1, actual0, sizeof control);
          bsl::memcpy(actual, actual0, sizeof control);
          bsl::memcpy(actualRaw, actual0, sizeof control);
          bsl::memcpy(control, actual0, sizeof control);

          BSU::insert0(actual0, LEN, DI, NB);
          BSU::insert1(actual1, LEN, DI, NB);
          BSU::insertRaw(actualRaw, LEN, DI, NB);

          LOOP_ASSERT(LINE, BSU::areEqual(actual0, 0, control, 0, DI));
          LOOP_ASSERT(LINE, !BSU::isAny1(actual0, DI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actual0, DI + NB,
                                             control, DI,
                                             MAX_NUM_BITS - DI - NB));

          LOOP_ASSERT(LINE, BSU::areEqual(actual1, 0, control, 0, DI));
          LOOP_ASSERT(LINE, !BSU::isAny0(actual1, DI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actual1, DI + NB,
                                             control, DI,
                                             MAX_NUM_BITS - DI - NB));

          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, 0, control, 0, DI));
          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, DI,
                                             control, DI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actualRaw, DI + NB,
                                             control, DI,
                                             MAX_NUM_BITS - DI - NB));

          BSU::insert(actual, LEN, DI, true, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(actual, 0, control, 0, DI));
          LOOP_ASSERT(LINE, !BSU::isAny0(actual, DI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actual, DI + NB,
                                             control, DI,
                                             MAX_NUM_BITS - DI - NB));

          populateBitstring(actual, 0, STR);

          BSU::insert(actual, LEN, DI, false, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(actual, 0, control, 0, DI));
          LOOP_ASSERT(LINE, !BSU::isAny1(actual, DI, NB));
          LOOP_ASSERT(LINE, BSU::areEqual(actual, DI + NB,
                                             control, DI,
                                             MAX_NUM_BITS - DI - NB));
      }
    } break;
    case 9: {
      // --------------------------------------------------------------------
      // TESTING 'copy'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void copy(int       *dstBitstring,
      //             int        dstIndex,
      //             const int *srcBitstring,
      //             int        srcIndex,
      //             int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'copy'"
                             << "\n==============" << bsl::endl;

      if (verbose) {
          bsl::cout << "Unaliased copy of bits" << bsl::endl;
      }
      {
          const struct {
              int         d_line;
              const char *d_dstString_p;
              int         d_dstIndex;
              const char *d_srcString_p;
              int         d_srcIndex;
              int         d_numBits;
          } DATA [] = {
    // Line  DstString                  DI  SrcString                SI  NB
    // ----  ---------                  --  ---------                --  --
 { L_,  "",                        0,  "",                       0,  0  },

 { L_,  "0",                       0,  "0",                      0,  0  },
 { L_,  "0",                       0,  "0",                      0,  1  },
 { L_,  "0",                       0,  "1",                      0,  1  },

 { L_,  "00",                      1,  "1",                      0,  1  },
 { L_,  "0",                       0,  "11",                     1,  1  },
 { L_,  "00",                      0,  "11",                     0,  1  },
 { L_,  "00",                      1,  "11",                     0,  1  },
 { L_,  "00",                      0,  "11",                     1,  1  },

 { L_,  "00",                      0,  "11",                     0,  2  },
 { L_,  "011",                     1,  "101",                    0,  2  },
 { L_,  "011",                     1,  "101",                    1,  2  },
 { L_,  "011",                     0,  "101",                    1,  2  },

 { L_,  "00",                      0,  "11",                     0,  2  },
 { L_,  "011",                     1,  "101",                    0,  2  },
 { L_,  "011",                     1,  "101",                    1,  2  },
 { L_,  "011",                     0,  "101",                    1,  2  },

 { L_,  "00001111 00001111 00001111 00001111",                      0,
        "11110000 11110000 11110000 11110000",                      0,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      1,
        "11110000 11110000 11110000 11110000",                      0,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      0,
        "11110000 11110000 11110000 11110000",                      1,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      1,
        "11110000 11110000 11110000 11110000",                      1,  31 },

 { L_,  "00001111 00001111 00001111 00001111 00001111",              0,
        "11110000 11110000 11110000 11110000 11110000",              0,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              1,
        "11110000 11110000 11110000 11110000 11110000",              0,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              0,
        "11110000 11110000 11110000 11110000 11110000",              1,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              1,
        "11110000 11110000 11110000 11110000 11110000",              1,  32 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   5,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   10,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   10,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   5,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   30,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   30,
   35
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 }
          };
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          const int MAX_ARRAY_SIZE = 4;
          for (int i = 0; i < NUM_DATA; ++i) {
              const int   LINE  = DATA[i].d_line;
              const char *DSTR  = DATA[i].d_dstString_p;
              const int   DI    = DATA[i].d_dstIndex;
              const char *SSTR  = DATA[i].d_srcString_p;
              const int   SI    = DATA[i].d_srcIndex;
              const int   NB    = DATA[i].d_numBits;

              if (veryVerbose) {
                  P(LINE) P_(DSTR) P(DI) P_(SSTR) P_(SI) P(NB)
              }

              int dst[MAX_ARRAY_SIZE] = { 0 };
              int src[MAX_ARRAY_SIZE] = { 0 };
              int control[MAX_ARRAY_SIZE];
              const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

              populateBitstring(dst, 0, DSTR);
              populateBitstring(src, 0, SSTR);
              bsl::memcpy(control, dst, MAX_ARRAY_SIZE * sizeof(int));

              BSU::copy(dst, DI, src, SI, NB);
              LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, src, SI, NB));
              LOOP_ASSERT(LINE, checkControl(dst, 0, control, 0,
                                             MAX_NUM_BITS, DI, DI, NB));
          }
      }

      if (verbose) {
          bsl::cout << "Aliased copy of bits" << bsl::endl;
      }

      {
          const struct {
              int         d_line;
              const char *d_inputString_p;
              int         d_dstIndex;
              int         d_srcIndex;
              int         d_numBits;
              const char *d_expString_p;
          } DATA [] = {
// Line  InputStr                DI  SI  NB  ExpString
// ----  --------                --  --  --  ---------
 { L_,  "",                       0,  0,  0, ""                         },
 { L_,  "0",                      0,  0,  0, "0"                        },
 { L_,  "0",                      0,  0,  1, "0"                        },
 { L_,  "1",                      0,  0,  1, "1"                        },

 { L_,  "01",                     0,  0,  1, "01"                       },
 { L_,  "01",                     0,  1,  1, "00"                       },
 { L_,  "01",                     1,  0,  1, "11"                       },
 { L_,  "01",                     1,  1,  1, "01"                       },
 { L_,  "01",                     0,  0,  2, "01"                       },

 { L_,  "101",                    0,  0,  2, "101"                      },
 { L_,  "101",                    0,  1,  2, "110"                      },
 { L_,  "101",                    1,  0,  2, "011"                      },
 { L_,  "101",                    1,  1,  2, "101"                      },

 {
   L_,
   "00001111 00001111 00001111 00001111",
   12,
   8,
   16,
   "00000000 11110000 11111111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111",
   8,
   12,
   16,
   "00001111 11110000 11110000 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111",
   0,
   10,
   22,
   "00001111 00000011 11000011 11000011",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111",
   10,
    0,
   22,
   "00111100 00111100 00111111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   10,
   28,
   "00001111 00001111 00000011 11000011 11000011 11000011",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111",
   10,
   0,
   28,
   "00001111 00111100 00111100 00111100 00111111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111",
   30,
   28,
   4,
   "00001111 00001100 00001111 00001111 00001111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111",
   28,
   30,
   4,
   "00001111 00001111 11001111 00001111 00001111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   0,
   0,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   1,
   31,
   "00001111 00001111 00001111 00001111 00001111"
   "00000111 10000111 10000111 10000111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   0,
   31,
   "00001111 00001111 00001111 00001111 00001111"
   "00011110 00011110 00011110 00011111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   1,
   31,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   1,
   32,
   "00001111 00001111 00001111 00001111 00001111"
   "10000111 10000111 10000111 10000111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   0,
   32,
   "00001111 00001111 00001111 00001111 00001110"
   "00011110 00011110 00011110 00011111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   1,
   32,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   5,
   20,
   30,
   "00001111 00001111 00001111 00001111 00001110"
   "00011110 00011110 00011110 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   20,
   5,
   30,
   "00001111 00001111 00001111 10000111 10000111"
   "10000111 10001111 00001111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   20,
   20,
   30,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   0,
   63,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   1,
   63,
   "00001111 00000111 10000111 10000111 10000111"
   "10000111 10000111 10000111 10000111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   0,
   63,
   "00001111 00011110 00011110 00011110 00011110"
   "00011110 00011110 00011110 00011111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   1,
   63,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   0,
   64,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   0,
   1,
   64,
   "00001111 10000111 10000111 10000111 10000111"
   "10000111 10000111 10000111 10000111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   0,
   64,
   "00001110 00011110 00011110 00011110 00011110"
   "00011110 00011110 00011110 00011111",
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   1,
   1,
   64,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
 },
          };
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          const int MAX_ARRAY_SIZE = 4;
          for (int i = 0; i < NUM_DATA; ++i) {
              const int   LINE  = DATA[i].d_line;
              const char *ISTR  = DATA[i].d_inputString_p;
              const int   LEN   = bsl::strlen(ISTR);
              const int   DI    = DATA[i].d_dstIndex;
              const int   SI    = DATA[i].d_srcIndex;
              const int   NB    = DATA[i].d_numBits;
              const char *EXP   = DATA[i].d_expString_p;

              if (veryVerbose) {
                  P(LINE) P_(ISTR) P_(DI)  P_(SI) P_(NB) P(EXP)
              }

              ASSERT(LEN == bsl::strlen(EXP));

              int actual[MAX_ARRAY_SIZE] = { 0 };
              int exp[MAX_ARRAY_SIZE] = { 0 };
              int control[MAX_ARRAY_SIZE];
              const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

              populateBitstring(actual, 0, ISTR);
              populateBitstring(exp, 0, EXP);
              bsl::memcpy(control, actual, MAX_ARRAY_SIZE * sizeof(int));

              BSU::copy(actual, DI, actual, SI, NB);
              LOOP_ASSERT(LINE, BSU::areEqual(actual, 0, exp, 0, LEN));
              LOOP_ASSERT(LINE, checkControl(actual, 0, control, 0,
                                             MAX_NUM_BITS,
                                             DI, DI, NB));

          }
      }
    } break;
    case 8: {
      // --------------------------------------------------------------------
      // TESTING 'append' series of functions
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void append0(int *bitstring, int length, int numBits);
      //   void append1(int *bitstring, int length, int numBits);
      //   void append(int *bitstring, int len, bool value, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'append0' and 'append1'"
                             << "\n==============================="
                             << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_numBits;
      } DATA [] = {
        // Line  InputString                                     NumBits
        // ----  -----------                                     -------
        {   L_,  "",                                                   0   },
        {   L_,  "",                                                   1   },
        {   L_,  "",                                                  31   },
        {   L_,  "",                                                  32   },
        {   L_,  "",                                                  33   },
        {   L_,  "",                                                  63   },
        {   L_,  "",                                                  64   },
        {   L_,  "",                                                  65   },

        {   L_,  "0",                                                  0   },
        {   L_,  "0",                                                  1   },
        {   L_,  "1",                                                 31   },
        {   L_,  "1",                                                 32   },
        {   L_,  "1",                                                 63   },
        {   L_,  "0",                                                 64   },

        {   L_,  "110",                                                0   },
        {   L_,  "010",                                                1   },
        {   L_,  "011",                                               29   },
        {   L_,  "111",                                               30   },
        {   L_,  "000",                                               61   },
        {   L_,  "111",                                               62   },

        {   L_,  "0001111 00001111 00001111 00001111",                 0    },
        {   L_,  "0001111 00001111 00001111 00001111",                 1    },
        {   L_,  "0001111 00001111 00001111 00001111",                 2    },
        {   L_,  "0001111 00001111 00001111 00001111",                33    },
        {   L_,  "0001111 00001111 00001111 00001111",                34    },

        {   L_,  "00001111 00001111 00001111 00001111 00001111",     24    },
        {   L_,  "00001111 00001111 00001111 00001111 00001111",     25    },
        {   L_,  "00001111 00001111 00001111 00001111 00001111",     55    },
        {   L_,  "00001111 00001111 00001111 00001111 00001111",     56    },
        {   L_,  "00001111 00001111 00001111 00001111 00001111",     57    },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   LEN       = bsl::strlen(INPUT_STR);
          const int   NB        = DATA[i].d_numBits;

          ASSERT(LEN + NB <= MAX_ARRAY_SIZE * BITS_PER_INT);

          if (veryVerbose) {
              P_(LINE) P_(INPUT_STR) P(NB)
          }

          int actual0[MAX_ARRAY_SIZE] = { 0 };
          int actual1[MAX_ARRAY_SIZE] = { 0 };
          int actual[MAX_ARRAY_SIZE]  = { 0 };
          int exp[MAX_ARRAY_SIZE]     = { 0 };

          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

          populateBitstring(actual0, 0, INPUT_STR);
          bsl::memcpy(actual1, actual0, sizeof exp);
          bsl::memcpy(actual, actual0, sizeof exp);
          bsl::memcpy(exp, actual0, sizeof exp);

          BSU::append0(actual0, LEN, NB);
          BSU::set(exp, LEN, false, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(exp, 0, actual0, 0,
                                             MAX_NUM_BITS));

          BSU::append1(actual1, LEN, NB);
          BSU::set(exp, LEN, true, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(exp, 0, actual1, 0,
                                             MAX_NUM_BITS));

          BSU::append(actual, LEN, true, NB);
          BSU::set(exp, LEN, true, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(exp, 0, actual, 0,
                                             MAX_NUM_BITS));

          populateBitstring(actual, 0, INPUT_STR);

          BSU::append(actual, LEN, false, NB);
          BSU::set(exp, LEN, false, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(exp, 0, actual, 0,
                                             MAX_NUM_BITS));
      }
    } break;
    case 7: {
      // --------------------------------------------------------------------
      // TESTING 'copyRaw'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   void copyRaw(int       *dstBitstring,
      //                int        dstIndex,
      //                const int *srcBitstring,
      //                int        srcIndex,
      //                int        numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'copyRaw'"
                             << "\n=================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_dstString_p;
          int         d_dstIndex;
          const char *d_srcString_p;
          int         d_srcIndex;
          int         d_numBits;
      } DATA [] = {
    // Line  DstString                  DI  SrcString                SI  NB
    // ----  ---------                  --  ---------                --  --
 { L_,  "",                        0,  "",                       0,  0  },

 { L_,  "0",                       0,  "0",                      0,  0  },
 { L_,  "0",                       0,  "0",                      0,  1  },
 { L_,  "0",                       0,  "1",                      0,  1  },

 { L_,  "00",                      1,  "1",                      0,  1  },
 { L_,  "0",                       0,  "11",                     1,  1  },
 { L_,  "00",                      0,  "11",                     0,  1  },
 { L_,  "00",                      1,  "11",                     0,  1  },
 { L_,  "00",                      0,  "11",                     1,  1  },

 { L_,  "00",                      0,  "11",                     0,  2  },
 { L_,  "011",                     1,  "101",                    0,  2  },
 { L_,  "011",                     1,  "101",                    1,  2  },
 { L_,  "011",                     0,  "101",                    1,  2  },

 { L_,  "00",                      0,  "11",                     0,  2  },
 { L_,  "011",                     1,  "101",                    0,  2  },
 { L_,  "011",                     1,  "101",                    1,  2  },
 { L_,  "011",                     0,  "101",                    1,  2  },

 { L_,  "00001111 00001111 00001111 00001111",                      0,
        "11110000 11110000 11110000 11110000",                      0,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      1,
        "11110000 11110000 11110000 11110000",                      0,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      0,
        "11110000 11110000 11110000 11110000",                      1,  31 },
 { L_,  "00001111 00001111 00001111 00001111",                      1,
        "11110000 11110000 11110000 11110000",                      1,  31 },

 { L_,  "00001111 00001111 00001111 00001111 00001111",              0,
        "11110000 11110000 11110000 11110000 11110000",              0,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              1,
        "11110000 11110000 11110000 11110000 11110000",              0,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              0,
        "11110000 11110000 11110000 11110000 11110000",              1,  32 },
 { L_,  "00001111 00001111 00001111 00001111 00001111",              1,
        "11110000 11110000 11110000 11110000 11110000",              1,  32 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   0,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   0,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   1,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   1,
   33
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   5,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   10,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   10,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   5,
   33
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   25,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   30,
   45
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
   25,
   45
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   30,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   30,
   35
 },

 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   32,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   31,
   35
 },
 {
   L_,
   "00001111 00001111 00001111 00001111 00001111"
   "00001111 00001111 00001111 00001111",
   31,
   "11110000 11110000 11110000 11110000 11110000"
   "11110000 11110000 11110000 11110000",
   32,
   35
 }
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE  = DATA[i].d_line;
          const char *DSTR  = DATA[i].d_dstString_p;
          const int   DI    = DATA[i].d_dstIndex;
          const char *SSTR  = DATA[i].d_srcString_p;
          const int   SI    = DATA[i].d_srcIndex;
          const int   NB    = DATA[i].d_numBits;

          if (veryVerbose) {
              P(LINE) P_(DSTR) P(DI) P_(SSTR) P_(SI) P(NB)
          }

          int dst[MAX_ARRAY_SIZE] = { 0 };
          int src[MAX_ARRAY_SIZE] = { 0 };

          populateBitstring(dst, 0, DSTR);
          populateBitstring(src, 0, SSTR);

          int control[MAX_ARRAY_SIZE] = { 0 };
          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;
          bsl::memcpy(control, dst, MAX_ARRAY_SIZE * sizeof(int));

          BSU::copyRaw(dst, DI, src, SI, NB);
          LOOP_ASSERT(LINE, BSU::areEqual(dst, DI, src, SI, NB));
          LOOP_ASSERT(LINE, checkControl(dst, 0, control, 0,
                                         MAX_NUM_BITS, DI, DI, NB));
      }
    } break;
    case 6: {
      // --------------------------------------------------------------------
      // TESTING 'isAny0' and 'isAny1'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   bool isAny0(const int *bitstring, int index, int numBits);
      //   bool isAny1(const int *bitstring, int index, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'isAny0' and 'isAny1'"
                             << "\n============================="
                             << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_numBits;
          bool        d_isAny0;
          bool        d_isAny1;
      } DATA [] = {
   // Line  InputString                         Index NumBits    set0  set1
   // ----  -----------                         ----- -------    ----  ----
     { L_,  "",                                    0,      0,   false, false },
     { L_,  "0",                                   0,      0,   false, false },
     { L_,  "0",                                   0,      1,   true,  false },
     { L_,  "1",                                   0,      1,   false, true  },

     { L_,  "10",                                  1,      1,   false, true  },
     { L_,  "10",                                  0,      2,   true,  true  },

     { L_,  "110",                                 0,      3,   true,  true  },
     { L_,  "110",                                 1,      2,   false, true  },

     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   0,      4,   true,  false },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   4,      4,   false, true  },

     { L_,
     "11110000 11110000 11110000 11110000 00000000 00000000 00000000 00000000",
                                                   2,     31,   true,  false },
     { L_,
     "11110000 11110000 11110000 11110000 00000000 00000000 00000000 00000000",
                                                   2,     32,   true,  false },
     { L_,
     "11110000 11110000 11110000 11110000 00000000 00000000 00000000 00000000",
                                                   2,     33,   true,  false },

     { L_,
     "11110000 11110000 11110000 11111111 11111111 11111111 11111111 11111111",
                                                   2,     31,   false, true  },
     { L_,
     "11110000 11110000 11110000 11111111 11111111 11111111 11111111 11111111",
                                                   2,     32,   false, true  },
     { L_,
     "11110000 11110000 11110000 11111111 11111111 11111111 11111111 11111111",
                                                   2,     33,   false, true  },

     { L_,
     "00011 11111111 11111111 11111111 11111111 11111111 11111111 11111111"
     " 11111111",
                                                   1,     64,   false, true  },
     { L_,
     "11100 00000000 00000000 00000000 00000000 00000000 00000000 00000000"
     " 00000000",
                                                   1,     64,   true, false  },
     { L_,
     "11100 00000001 00000000 00000000 00000000 00100000 00000000 00000100"
     " 00000000",
                                                   1,     64,   true, true   },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   INDEX     = DATA[i].d_index;
          const int   NUM_BITS  = DATA[i].d_numBits;
          const bool  SET0      = DATA[i].d_isAny0;
          const bool  SET1      = DATA[i].d_isAny1;

          if (veryVerbose) {
              P_(LINE) P_(INPUT_STR) P_(INDEX) P_(NUM_BITS) P_(SET0) P(SET1)
          }

          int bits[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(bits, 0, INPUT_STR);
          LOOP_ASSERT(LINE, SET0 == BSU::isAny0(bits, INDEX, NUM_BITS));
          LOOP_ASSERT(LINE, SET1 == BSU::isAny1(bits, INDEX, NUM_BITS));
      }
    } break;
    case 5: {
      // --------------------------------------------------------------------
      // TESTING 'get' and 'set'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   int get(const int *bitstring, int index, int numBits);
      //   void set(int *bitstring, int index, bool value, int numBits);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'get' and 'set'"
                             << "\n======================="
                             << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_numBits;
          int         d_expValue;
      } DATA [] = {
        // Line  InputString                         Index NumBits    ExpValue
        // ----  -----------                         ----- -------    --------
     { L_,  "",                                    0,      0,          0     },
     { L_,  "0",                                   0,      0,          0     },
     { L_,  "0",                                   0,      1,          0     },
     { L_,  "1",                                   0,      1,          1     },

     { L_,  "10",                                  0,      1,          0     },
     { L_,  "10",                                  1,      1,          1     },
     { L_,  "10",                                  0,      2,          2     },

     { L_,  "110",                                 0,      3,          6     },
     { L_,  "110",                                 1,      2,          3     },

     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   0,     10,       0xF0     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   0,     32, 0xF0F0F0F0     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   1,     32, 0x78787878     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                   2,     32, 0x3C3C3C3C     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                  30,     32, 0xC3C3C3C3     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                  31,     32, 0xE1E1E1E1     },
     { L_,
     "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000",
                                                  32,     32, 0xF0F0F0F0     },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   INDEX     = DATA[i].d_index;
          const int   NUM_BITS  = DATA[i].d_numBits;
                int   exp       = DATA[i].d_expValue;

          if (veryVerbose) {
              P_(LINE) P_(INPUT_STR) P_(INDEX) P_(NUM_BITS) P(exp)
          }

          int bits[MAX_ARRAY_SIZE] = { 0 }, control[MAX_ARRAY_SIZE];
          populateBitstring(bits, 0, INPUT_STR);
          bsl::memcpy(control, bits, sizeof bits);
          const int MAX_NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;
          LOOP_ASSERT(LINE, exp == BSU::get(bits, INDEX, NUM_BITS));

          BSU::set(bits, INDEX, true, NUM_BITS);
          exp = bdes_BitUtil::oneMask(0, NUM_BITS);
          LOOP_ASSERT(LINE, exp == BSU::get(bits, INDEX, NUM_BITS));
          LOOP_ASSERT(LINE, checkControl(bits, 0, control, 0,
                                        MAX_NUM_BITS, INDEX, INDEX, NUM_BITS));

          BSU::set(bits, INDEX, false, NUM_BITS);
          LOOP_ASSERT(LINE, 0 == BSU::get(bits, INDEX, NUM_BITS));
          LOOP_ASSERT(LINE, checkControl(bits, 0, control, 0,
                                        MAX_NUM_BITS, INDEX, INDEX, NUM_BITS));
      }
    } break;
    case 4: {
      // --------------------------------------------------------------------
      // TESTING 'get' and 'set'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   bool get(const int *bitstring, int index);
      //   void set(int *bitstring, int index, bool value);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'get' and 'set'"
                             << "\n=======================" << bsl::endl;

      const struct {
          int         d_line;
          const char *d_inputString_p;
      } DATA [] = {
        // Line  InputString
        // ----  -----------
        {   L_,  ""                                                        },
        {   L_,  "0"                                                       },
        {   L_,  "1"                                                       },
        {   L_,  "01"                                                      },
        {   L_,  "110"                                                     },
        {   L_,  "00011101"                                                },
        {   L_,  "1100 11110000 11110000 11110000 11110000"                },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   LEN       = bsl::strlen(INPUT_STR);

          if (veryVerbose) {
              P_(LINE) P(INPUT_STR)
          }

          int bits[MAX_ARRAY_SIZE] = { 0 };
          populateBitstring(bits, 0, INPUT_STR);
          for (int j = 0, index = 0; j < LEN; ++j) {
              char value = INPUT_STR[LEN - 1 - j];
              if (bsl::isspace(value)) {
                  continue;
              }

              int control[MAX_ARRAY_SIZE];
              const int NUM_BITS = MAX_ARRAY_SIZE * BITS_PER_INT;

              bsl::memcpy(control, bits, sizeof bits);

              const bool EXP = value - '0';
              LOOP3_ASSERT(LINE, j, index, EXP == BSU::get(bits, index));

              BSU::set(bits, index, true);
              LOOP3_ASSERT(LINE, j, index, true == BSU::get(bits, index));
              LOOP3_ASSERT(LINE, j, index, checkControl(bits, 0, control, 0,
                                                        NUM_BITS, index, index,
                                                        1));

              BSU::set(bits, index, false);
              LOOP3_ASSERT(LINE, j, index, false == BSU::get(bits, index));
              LOOP3_ASSERT(LINE, j, index, checkControl(bits, 0, control, 0,
                                                        NUM_BITS, index, index,
                                                        1));
              ++index;
          }
      }
    } break;
    case 3: {
      // --------------------------------------------------------------------
      // TESTING 'areEqual'
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      //   bool areEqual(const int *lhsBitstring,
      //                 int        lhsIndex,
      //                 const int *rhsBitstring,
      //                 int        rhsIndex,
      //                 int        numBits);
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
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int MAX_ARRAY_SIZE = 4;
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

          int lhs[MAX_ARRAY_SIZE] = { 0 };
          int rhs[MAX_ARRAY_SIZE] = { 0 };

          populateBitstring(lhs, 0, LSTR);
          populateBitstring(rhs, 0, RSTR);

          LOOP_ASSERT(LINE, EXP == BSU::areEqual(lhs, LI, rhs, RI, NB));
      }
    } break;
    case 2: {
      // --------------------------------------------------------------------
      // TESTING 'populateBitstring' HELPER FUNCTION
      //
      // Concerns:
      //   The helper function populates the bit array according to the input.
      //
      // Plan:
      //
      // Testing:
      //   void populateBitstring(int *bitstring, int idx, const char *ascii);
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nTESTING 'populateBitstring' HELPER FUNCTION"
                             << "\n==========================================="
                             << bsl::endl;
      const int MAX_RESULT_SIZE = 3;
      const struct {
          int         d_line;
          const char *d_inputString_p;
          int         d_index;
          int         d_expResult[MAX_RESULT_SIZE];
      } DATA [] = {
    // Line  InputString       Index                      Expected
    // ----  -----------       -----                      --------
    {   L_,  "",                  0,   {          0,          0,          0 }},
    {   L_,  "",                  1,   {          0,          0,          0 }},
    {   L_,  "",                 31,   {          0,          0,          0 }},
    {   L_,  "",                 32,   {          0,          0,          0 }},
    {   L_,  "",                 63,   {          0,          0,          0 }},
    {   L_,  "",                 64,   {          0,          0,          0 }},
    {   L_,  "",                 95,   {          0,          0,          0 }},

    {   L_, "1",                  0,   {          1,          0,          0 }},
    {   L_, "1",                  1,   {          2,          0,          0 }},
    {   L_, "1",                 31,   { 0x80000000,          0,          0 }},
    {   L_, "1",                 32,   {          0,          1,          0 }},
    {   L_, "1",                 63,   {          0, 0x80000000,          0 }},
    {   L_, "1",                 64,   {          0,          0,          1 }},
    {   L_, "1",                 95,   {          0,          0, 0x80000000 }},

    {   L_, "10",                 0,   {          2,          0,          0 }},
    {   L_, "10",                 1,   {          4,          0,          0 }},
    {   L_, "10",                30,   { 0x80000000,          0,          0 }},
    {   L_, "10",                31,   {          0,          1,          0 }},
    {   L_, "10",                32,   {          0,          2,          0 }},
    {   L_, "10",                62,   {          0, 0x80000000,          0 }},
    {   L_, "10",                63,   {          0,          0,          1 }},
    {   L_, "10",                64,   {          0,          0,          2 }},
    {   L_, "10",                94,   {          0,          0, 0x80000000 }},

    {   L_, "10101",              0,   {       0x15,          0,          0 }},
    {   L_, "10101",              1,   {       0x2A,          0,          0 }},
    {   L_, "10101",             29,   { 0xA0000000,          2,          0 }},
    {   L_, "10101",             30,   { 0x40000000,          5,          0 }},
    {   L_, "10101",             31,   { 0x80000000,        0xA,          0 }},
    {   L_, "10101",             32,   {          0,       0x15,          0 }},
    {   L_, "10101",             62,   {          0, 0x40000000,          5 }},
    {   L_, "10101",             63,   {          0, 0x80000000,        0xA }},
    {   L_, "10101",             64,   {          0,          0,       0x15 }},
    {   L_, "10101",             91,   {          0,          0, 0xA8000000 }},

    {   L_, "1111 00001111 00001111 00001111 00001111",
                                  0,   { 0x0F0F0F0F,        0xF,          0 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                  2,   { 0x3C3C3C3C,       0x3C,          0 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 15,   { 0x87878000,    0x78787,          0 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 30,   { 0xC0000000, 0xC3C3C3C3,          3 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 31,   { 0x80000000, 0x87878787,          7 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 32,   {          0, 0x0F0F0F0F,        0xF }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 48,   {          0, 0x0F0F0000,    0xF0F0F }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 55,   {          0, 0x87800000,  0x7878787 }},
    {   L_, "1111 00001111 00001111 00001111 00001111",
                                 60,   {          0, 0xF0000000, 0xF0F0F0F0 }},

      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *INPUT_STR = DATA[i].d_inputString_p;
          const int   INDEX     = DATA[i].d_index;
          const int  *EXP       = DATA[i].d_expResult;
          int actual[MAX_RESULT_SIZE] = { 0 };

          if (veryVerbose) {
              P_(LINE) P_(INPUT_STR) P(INDEX)
              P_(EXP[0]) P_(EXP[1]) P_(EXP[2])
          }

          populateBitstring(actual, INDEX, INPUT_STR);
          LOOP_ASSERT(LINE, 0 == bsl::memcmp(actual, EXP, sizeof actual));
      }
    } break;
    case 1: {
      // --------------------------------------------------------------------
      // BREATHING TEST
      //   This test !exercises! basic functionality, but !tests! nothing.
      //
      // Concerns:
      //   Operations on "bit strings" should produce reasonable results.
      //
      // Plan:
      //   A utility component typically does not need a breathing test.
      //   This case is provided as a temporary workspace during development.
      //
      // Testing:
      //   This test !exercises! basic functionality, but !tests! nothing.
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nBREATHING TEST"
                             << "\n==============" << bsl::endl;

      const int SIZE            = 3;
      int       bitstring0[SIZE] = { 0 };
      const int ARRAY_SIZE      = sizeof bitstring0;
      const int ARRAY_NUM_BITS  = ARRAY_SIZE * CHAR_BIT;
      const int num0        = (SIZE - 1) * sizeof(int) * CHAR_BIT;

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num0; ++i) {
          LOOP_ASSERT(i, !BSU::get(bitstring0, i));
      }

      BSU::set(bitstring0, 8, 1);

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num0; ++i) {
          LOOP_ASSERT(i, (8 == i) ?  BSU::get(bitstring0, i)
                                  : !BSU::get(bitstring0, i));
      }

      BSU::copyRaw(bitstring0, 32, bitstring0, 0, 32);

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num0; ++i) {
          LOOP_ASSERT(i, (8 == i || 40 == i)
                       ? BSU::get(bitstring0, i)
                       : !BSU::get(bitstring0, i));
      }

      // All 1's
      int bitstring1[SIZE] = { -1, -1, -1 };

      const int num1  = sizeof(int) * (SIZE - 1) * CHAR_BIT;

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num1; ++i) {
          LOOP_ASSERT(i, BSU::get(bitstring1, i));
      }

      BSU::set(bitstring1, 9, 0);

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num1; ++i) {
          LOOP_ASSERT(i, (9 == i) ? !BSU::get(bitstring1, i)
                                  :  BSU::get(bitstring1, i));
      }

      BSU::copyRaw(bitstring1, 32, bitstring1, 0, 32);

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      for (int i = 0; i < num1; ++i) {
          LOOP_ASSERT(i, (9 == i || 41 == i)
                       ? !BSU::get(bitstring1, i)
                       :  BSU::get(bitstring1, i));
      }

      // combinations
      BSU::copyRaw(bitstring0, 0, bitstring1, 32, 32);

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      ASSERT(bitstring0[0] == bitstring1[0]);
      ASSERT(bitstring0[1] != bitstring1[1]);

      BSU::copyRaw(bitstring1, 32, bitstring0, 32, 32);

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      ASSERT(bitstring0[0] == bitstring1[0]);
      ASSERT(bitstring0[1] == bitstring1[1]);

      int expBitstring[SIZE];
      bsl::memcpy(expBitstring, bitstring1, ARRAY_SIZE);

      BSU::set(bitstring1, 2, 35, 0);

      ASSERT(0 == bsl::memcmp(expBitstring, bitstring1, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      BSU::set(bitstring1, 33, 28, 1);

      expBitstring[1] |= 0x00000002;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring1, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      BSU::set(bitstring1, 0, 0, 9);
      expBitstring[0] &= 0xFFFFFC00;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring1, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      BSU::set(bitstring0, 0, false, 16);
      BSU::set(bitstring0, 16, true, 16);
      BSU::set(bitstring0, 32, false, 16);
      BSU::set(bitstring0, 48, true, 16);

      ASSERT(0xFFFF0000 == bitstring0[0]);
      ASSERT(0xFFFF0000 == bitstring0[1]);

      BSU::set(bitstring1, 0, true, 16);
      BSU::set(bitstring1, 16, false, 16);
      BSU::set(bitstring1, 32, true, 16);
      BSU::set(bitstring1, 48, false, 16);

      ASSERT(0x0000FFFF == bitstring1[0]);
      ASSERT(0x0000FFFF == bitstring1[1]);

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      //////////////////////////////////////////////////////
      BSU::copyRaw(bitstring1, 5, bitstring0, 2, 40);
      expBitstring[0] = 0xFFF8001F;
      expBitstring[1] = 0x0000E007;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring1, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      bsl::memcpy(expBitstring, bitstring0, ARRAY_SIZE);
      BSU::copyRaw(bitstring0, 33, bitstring1, 2, 22);
      expBitstring[1] = 0xFFFC000E;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring0, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
      }

      BSU::copyRaw(bitstring1, 21, bitstring1, 2, 18);
      expBitstring[0] = 0x00F8001F;
      expBitstring[1] = 0x0000E040;
      expBitstring[2] = 0xFFFFFFFF;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring1, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nbitstring1 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring1, ARRAY_NUM_BITS);
          bsl::cout << "\nexpBitstring = " << bsl::endl;
          BSU::print(bsl::cout, expBitstring, ARRAY_NUM_BITS);
      }

      BSU::insert1(bitstring0, num0, 6, 5);
      expBitstring[0] = 0xFFE007C0;
      expBitstring[1] = 0xFF8001DF;
      expBitstring[2] = 0x0000001F;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring0, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nexpBitstring = " << bsl::endl;
          BSU::print(bsl::cout, expBitstring, ARRAY_NUM_BITS);
      }

      BSU::removeAndFill0(bitstring0, num0, 6, 5);
      expBitstring[0] = 0xFFFF0000;
      expBitstring[1] = 0x07FC000E;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring0, ARRAY_SIZE));

      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nexpBitstring = " << bsl::endl;
          BSU::print(bsl::cout, expBitstring, ARRAY_NUM_BITS);
      }

      bitstring0[0] = 0xAAAAAAAA;
      bitstring0[1] = 0xAAAAAAAA;
      bsl::memcpy(expBitstring, bitstring0, ARRAY_SIZE);
      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      BSU::copy(bitstring0, 0, bitstring0, 1, 33);
      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      bsl::memcpy(bitstring0, expBitstring, ARRAY_SIZE);
      BSU::copy(bitstring0, 1, bitstring0, 0, 33);
      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }

      bitstring0[0] = 0;
      bitstring0[1] = 0;
      bitstring0[2] = 0;

      BSU::insert1(bitstring0, num0, 0, 1);
      expBitstring[0] = 1;
      expBitstring[1] = 0;
      expBitstring[2] = 0;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring0, ARRAY_SIZE));
      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
          bsl::cout << "\nexpBitstring = " << bsl::endl;
          BSU::print(bsl::cout, expBitstring, ARRAY_NUM_BITS);
      }

      BSU::removeAndFill0(bitstring0, num0, 0, 1);
      expBitstring[0] = 0;
      ASSERT(0 == bsl::memcmp(expBitstring, bitstring0, ARRAY_SIZE));
      if (veryVerbose) {
          bsl::cout << "\nbitstring0 = " << bsl::endl;
          BSU::print(bsl::cout, bitstring0, ARRAY_NUM_BITS);
      }
    } break;
    default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
