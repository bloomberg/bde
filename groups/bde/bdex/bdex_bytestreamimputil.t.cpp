// bdex_bytestreamimputil.t.cpp                                       -*-C++-*-

#include <bdex_bytestreamimputil.h>

#include <bsl_c_ctype.h>        // isspace()
#include <bsl_c_stdio.h>
#include <bsl_cstdlib.h>        // atoi()
#include <bsl_cstring.h>        // memcpy(), memcmp(), memcpy(), strcpy()
#include <bsl_iostream.h>
#include <bsl_ostream.h>

#ifdef BSLS_PLATFORM_CMP_MSVC
// disable lots of warnings asking to confirm we know the rules for operator
// precedence.  These warnings are noise, and fixing them in code will render
// the tests as significantly less readable.
#pragma warning(disable: 4554)
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a suite of pure procedures, each of which is independent
// (except for those that are inline-forwarded).  We should make sure that
// we fully understand the nature of a double on this platform -- i.e., that
// it conforms to the IEEE standard.  After that, it is a simple matter of
// verifying that each function produces the expected encoding.  Once encoded,
// we should be able to decode it back to the identical value.  Therefore,
// pairing corresponding put/get functions seems to make sense.  In each
// case, endianness will be tested only if we explicitly verify the encoded
// results.  Two general routines (one to encode byte strings from a bit
// string, and one to compare byte strings) will be needed.  A table driven
// approach and then be used throughout.
//
// Note also that since most 64-bit platforms treat short as 16 bits and int
// as 32 bits, we either need to assert this as fact or make sure the test
// case verifies sign- and zero- extensions for get functions appropriately
// (we have opted to do the latter).
//--------------------------------------------------------------------------
// [ 5] putInt64(char *buf, Int64 val);
// [ 6] putInt56(char *buf, Int64 val);
// [ 7] putInt48(char *buf, Int64 val);
// [ 8] putInt40(char *buf, Int64 val);
// [ 9] putInt32(char *buf, int val);
// [10] putInt24(char *buf, int val);
// [11] putInt16(char *buf, int val);
// [12] putInt8(char *buf, int val);
// [13] putFloat64(char *buf, double val);
// [14] putFloat32(char *buf, float val);
//
// [ 5] getInt64(Int64 *var, const char *buf);
// [ 5] getUint64(Uint64 *var, const char *buf);
// [ 6] getInt56(Int64 *var, const char *buf);
// [ 6] getUint56(Uint64 *var, const char *buf);
// [ 7] getInt48(Int64 *var, const char *buf);
// [ 7] getUint48(Uint64 *var, const char *buf);
// [ 8] getInt40(Int64 *var, const char *buf);
// [ 8] getUint40(Uint64 *var, const char *buf);
// [ 9] getInt32(int *var, const char *buf);
// [ 9] getUint32(unsigned int *var, const char *buf);
// [10] getInt24(int *var, const char *buf);
// [10] getUint24(unsigned int *var, const char *buf);
// [11] getInt16(short *var, const char *buf);
// [11] getUint16(unsigned short *var, const char *buf);
// [12] getInt8(char *var, const char *buf);
// [12] getInt8(signed char *var, const char *buf);
// [12] getInt8(unsigned char *var, const char *buf);
// [13] getFloat64(double *var, const char *buf);
// [14] getFloat32(float *var, const char *buf);
//
// [15] putArrayInt64(char *buf, const Int64 *ary, int cnt);
// [15] putArrayInt64(char *buf, const Uint64 *ary, int cnt);
// [16] putArrayInt56(char *buf, const Int64 *ary, int cnt);
// [16] putArrayInt56(char *buf, const Uint64 *ary, int cnt);
// [17] putArrayInt48(char *buf, const Int64 *ary, int cnt);
// [17] putArrayInt48(char *buf, const Uint64 *ary, int cnt);
// [18] putArrayInt40(char *buf, const Int64 *ary, int cnt);
// [18] putArrayInt40(char *buf, const Uint64 *ary, int cnt);
// [19] putArrayInt32(char *buf, const int *ary, int cnt);
// [19] putArrayInt32(char *buf, const unsigned int *ary, int cnt);
// [20] putArrayInt24(char *buf, const int *ary, int cnt);
// [20] putArrayInt24(char *buf, const unsigned int *ary, int cnt);
// [21] putArrayInt16(char *buf, const short *ary, int cnt);
// [21] putArrayInt16(char *buf, const unsigned short *ary, int cnt);
// [22] putArrayInt8(char *buf, const char *ary, int cnt);
// [22] putArrayInt8(char *buf, const signed char *ary, int cnt);
// [22] putArrayInt8(char *buf, const unsigned char *ary, int cnt);
// [23] putArrayFloat64(char *buf, const double *ary, int cnt);
// [24] putArrayFloat32(char *buf, const float *ary, int cnt);
//
// [15] getArrayInt64(Int64 *ary, const char *buf, int cnt);
// [15] getArrayUint64(Uint64 *ary, const char *buf, int cnt);
// [16] getArrayInt56(Int64 *ary, const char *buf, int cnt);
// [16] getArrayUint56(Uint64 *ary, const char *buf, int cnt);
// [17] getArrayInt48(Int64 *ary, const char *buf, int cnt);
// [17] getArrayUint48(Uint64 *ary, const char *buf, int cnt);
// [18] getArrayInt40(Int64 *ary, const char *buf, int cnt);
// [18] getArrayUint40(Uint64 *ary, const char *buf, int cnt);
// [19] getArrayInt32(int *ary, const char *buf, int cnt);
// [19] getArrayUint32(unsigned int *ary, const char *buf, int cnt);
// [20] getArrayInt24(int *ary, const char *buf, int cnt);
// [20] getArrayUint24(unsigned int *ary, const char *buf, int cnt);
// [21] getArrayInt16(short *ary, const char *buf, int cnt);
// [21] getArrayUint16(unsigned short *ary, const char *buf, int cnt);
// [22] getArrayInt8(char *ary, const char *buf, int cnt);
// [22] getArrayInt8(signed char *ary, const char *buf, int cnt);
// [22] getArrayInt8(unsigned char *ary, const char *buf, int cnt);
// [23] getArrayFloat64(double *ary, const char *buf, int cnt);
// [24] getArrayFloat32(float *ary, const char *buf, int cnt);
//--------------------------------------------------------------------------
// [ 1] VERIFY TESTING APPARATUS -- make sure our test functions work.
// [ 1] SWAP FUNCTION: static inline void swap(T *x, T *y)
// [ 1] REVERSE FUNCTION: void reverse(T *array, int numElements)
// [ 1] SET FUNCTION: void setBits(char *byte, int mask, int boolValue)
// [ 1] RESET FUNCTION: resetBits(char *buffer, int nBits, int boolValue)
// [ 1] EQ FUNCTION: int eq(const char *lhs, const char *rhs, int nBits)
// [ 2] GENERATOR FUNCTION: int g(char *buf, const char spec, int nBits)
// [ 3] EXPLORE DOUBLE FORMAT -- make sure format is IEEE COMPLIANT
// [ 4] EXPLORE FLOAT FORMAT -- make sure format is IEEE COMPLIANT
// [25] USAGE TEST - Make sure usage example compiles/works as advertized.
// [26] STRESS TEST - Used to determine performance characteristics.
//==========================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//==========================================================================
//        GENERATOR FUNCTION 'int g(buffer, spec, numBits)' FOR TESTING
//--------------------------------------------------------------------------
// The following function interprets the given 'spec' in order from left to
// right to configure a byte array to hold a sequence of 'numBits' according
// to a custom language.  Valid meaningful characters are the binary digits
// ('0' and '1') and a period ('.') used to indicate a sequence (e.g., "0..0"
// or "1..1".  t most one sequence may appear in a single spec.  Whitespace
// characters are ignored; all other characters are invalid.
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
// <ITEM>       ::= 0..0 | 1..1                ; 0.., 1..0, and ..1 are illegal
//
// <DIGIT>      ::= 0 | 1
//
// <EMPTY>      ::=                            ; ignore all whitespace
//
// Spec String          #b   Description [MS..LS] [MS..  Comments
// -----------          --   --------------------------  ---------------------
// ""                   18   [00000000] [00000000] [00]  0's are the default
// "   "                 6   [000000]                    white space is ignored
// " 1 "                 9   [10000000] [0]              9 bits are affected
// " 01"                 2   [01]                        2 bits are affected
// "101"                 4   [1010]                      4 bits are affected
// "0..01"               8   [00000001]                  fill with leading 0's
// "1..1"               11   [11111111] [111]            fill with 1's
// "1..10"              11   [11111111] [110]            fill with leading 1's
// "1 .\t. 1 0 1"        6   [111101]                    white space is ignored
// "11..1"               8   [111111]                    1 and trailing 1's
// "01..1"               8   [011111]                    0 and trailing 1's
// "10..0"               8   [100000]                    1 and trailing 0's
//
// "a"                   8    error   bad character
// "0..1"                8    error   left and right fill value must match
// "..1"                 8    error   missing left fill value
// "0.."                 8    error   missing right fill value
// "1..11..1"            8    error   at most one fill item per spec
// "010101..10101"       8    error   number of digits exceeds numBits
//--------------------------------------------------------------------------

static ostream& pByte(char c)
  // Print a single byte as 8 bits.
{
    for (int i = 7; i >= 0; --i) {
        cout << !!(c & (1 << i));
    }
    return cout;
}

static ostream& pBytes(const char *c, int n)
    // Print a sequence of bytes as space-separated 8-bit words.
{
    for (int i = 0; i < n; ++i) {
        if (i) {
            cout << ' ';
        }
        pByte(c[i]);
    }
    return cout;
}

static void setBits(char *byte, int mask, int booleanValue)
    // Set each bit in the specified 'byte' at positions corresponding
    // to '1'-bits in the specified 'mask' to the specified booleanValue.
{
    if (booleanValue) {
        *byte |= mask;
    }
    else {
        *byte &= ~mask;
    }
}

static void resetBits(char *buffer, int numBits, int booleanValue)
    // Reset each of the specified leading 'numBits' in the specified byte
    // 'buffer' to the specified boolean value.  Note that it is the more
    // significant bits in a partial byte that are set.
{
    ASSERT(0 <= numBits);
    ASSERT(!!booleanValue == booleanValue);

    int endByteIndex = numBits / 8;
    for (int byteIndex = 0; byteIndex < endByteIndex; ++byteIndex) {
        setBits(buffer + byteIndex, ~0, booleanValue);
    }

    int endBits = numBits % 8;
    if (endBits) {
        int remainingBits = 8 - endBits;
        int mask = 0xff >> remainingBits << remainingBits;
        setBits(buffer + endByteIndex, mask, booleanValue);
    }
}

static void setTrailing(char *buffer, int numBits,
                        const char *endOfSpec, int charCount)
    // Set the trailing bits in the specified 'buffer' containing the specified
    // 'numBits' corresponding to '0' and '1' characters in the specified
    // 'charCount' characters preceding the specified 'endOfSpec'.  All other
    // characters are ignored.
{
    int byteIndex = numBits / 8;
    int bitIndex = numBits % 8;

    const int start = -charCount;
    for (int i = -1; i >= start; --i) {
        char ch = endOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            --bitIndex;
            if (bitIndex < 0) {
                bitIndex = 7;
                --byteIndex;
            }
            setBits(buffer + byteIndex, 1 << (7 - bitIndex), '1' == ch);
        }
    }
    ASSERT(0 <= byteIndex);
}

static void setLeading(char *buffer, int numBits,
                       const char *startOfSpec, int charCount)
    // Set the leading bits in the specified 'buffer' containing the specified
    // '0' and '1' characters in the specified 'charCount' characters starting
    // at the specified 'startOfSpec'.  All other characters are ignored.
    // Note that the first Bit in the spec corresponds to the most significant
    // bit of the least significant byte in the byte array.
    // [ 0 1 2 3 4 5 6 7 ] [ 8 9 10 11 ...  ]
{
    int byteIndex = 0;
    int bitIndex = 7;

    for (int i = 0; i < charCount; ++i) {
        char ch = startOfSpec[i];
        switch (ch) {
          default: continue;
          case '0':
          case '1':
            setBits(buffer + byteIndex, 1 << bitIndex, '1' == ch);
            --bitIndex;
            if (bitIndex < 0) {
                bitIndex = 7;
                ++byteIndex;
            }
        }
    }
}

static int G_OFF = 0;  // set to 1 only to enable testing of G function errors

enum {
    G_SUCCESS               = 0,
    G_ILLEGAL_CHARACTER     = 1001,
    G_MISMATCHED_RANGE      = 1002,
    G_MISSING_RANGE_END     = 1003,
    G_MISSING_RANGE_START   = 1004,
    G_MISSING_SECOND_DOT    = 1005,
    G_MULTIPLE_RANGES       = 1006,
    G_TOO_MANY_BITS         = 1007
};

static int g(char *buffer, const char *spec, int numBits)
    // Encode in the specified character buffer, the specified 'numBits'
    // bits as described in 'spec' according to the above language.
    // Return 0 on success and non-zero on error; set the global
    // variable G_OFF to 1, to suppress automatic error reporting
    // when testing this function directly.
{
    ASSERT(0 <= numBits);
    int bitCount = 0;           // total number of bits encountered
    int lastBitIndex = -1;      // index of last bit encountered

    int rangeStartIndex = -1;   // index of first D in D..D
    int rangeEndIndex;          // index of second D in D..D

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

    if (bitCount > numBits) {
        LOOP2_ASSERT(bitCount, numBits, G_OFF || !"Too Many Bits");
        return G_TOO_MANY_BITS;
    }

    if (rangeStartIndex != -1) {
        resetBits(buffer, numBits, '1' == spec[rangeStartIndex]);
        setLeading(buffer, numBits, spec, rangeStartIndex);
        setTrailing(buffer, numBits, spec + i, i - 1 - rangeEndIndex);
    }
    else {
        resetBits(buffer, numBits, 0);
        setLeading(buffer, numBits, spec, i);
    }

    return G_SUCCESS;
}

//==========================================================================
//                      SUPPLEMENTARY TEST FUNCTIONALITY
//--------------------------------------------------------------------------

static inline int isLittleEndian()
    // Return 1 if this platform is inconsistent with network byte order
    // and zero otherwise; the LSB is at low end of the word.
{
    const int dummy = 1;
    return ((char *)&dummy)[0]; // Least significant byte has lowest address.
}

template<typename T>
inline void swap(T *x, T *y)
    // Swap the locations of the specified objects x and y;
    // Note that T must have value semantic operators COPY CTOR and OP=.
{
    T t = *x;
    *x = *y;
    *y = t;
}

template<typename T>
inline void reverse(T *array, int numElements)
    // Reverse the locations of the objects in the specified array.
{
    T *top = array + numElements - 1;
    int middle = numElements/2;  // if odd, middle is reversed already
    for (int i = 0; i < middle; ++i) {
        swap(array + i, top - i);
    }
}

static bool eq(const char *lhs, const char *rhs, int numBits)
    // Return 'true' if the specified leading 'numBits' are the same
    // for the specified 'lhs' and 'rhs' byte arrays, and 'false' otherwise.
    // Note that it is the most significant bits in a partial byte that
    // are compared.
{
    ASSERT(0 <= numBits);
    int wholeBytes = numBits / 8;
    int extraBits = numBits % 8;

    for (int i = 0; i < wholeBytes; ++i) {
        if (lhs[i] ^ rhs[i]) {
            return false;   // different
        }
    }

    if (extraBits) {
        int diff = lhs[wholeBytes] ^ rhs[wholeBytes];
        int remaingBits = 8 - extraBits;
        int mask = 0xff >> remaingBits << remaingBits;
        diff &= mask;   // if 0 != diff they're not equal
        return 0 == diff;
    }

    return true;   // same
}


//==========================================================================
//                      FUNCTIONS TO MANIPULATE DOUBLES
//--------------------------------------------------------------------------
//  sign bit    11-bit exponent             52-bit mantissa
//    /        /                           /
//  +-+-----------+----------------------------------------------------+
//  |s|e10......e0|m0...............................................m51|
//  +-+-----------+----------------------------------------------------+
//  LSB                                                              MSB
//--------------------------------------------------------------------------

static ostream& printDoubleBits(ostream& stream, double number)
    // Format the bits in the specified 'number' to the specified 'stream'
    // from least to most significant (space-separated) byte, printing
    // most-significant bits in each byte first.  Return a reference to the
    // modifiable 'stream'.
{
    const int SIZE = sizeof number;
    ASSERT(8 == SIZE);

    union {
        double d_double;
        unsigned char d_chars[1];
    } u;

    u.d_double = number;

    if (isLittleEndian()) {
        swap(u.d_chars + SIZE - 1, u.d_chars + 0);
        swap(u.d_chars + SIZE - 2, u.d_chars + 1);
        swap(u.d_chars + SIZE - 3, u.d_chars + 2);
        swap(u.d_chars + SIZE - 4, u.d_chars + 3);
    }

    for (int i = 0; i < SIZE; ++i) {
        if (i) {
            stream << ' ';
        }
        stream << !!(u.d_chars[i] & 0x80)
               << !!(u.d_chars[i] & 0x40)
               << !!(u.d_chars[i] & 0x20)
               << !!(u.d_chars[i] & 0x10)
               << !!(u.d_chars[i] & 0x08)
               << !!(u.d_chars[i] & 0x04)
               << !!(u.d_chars[i] & 0x02)
               << !!(u.d_chars[i] & 0x01);
    }

    return stream;
}

static void printDouble(ostream& stream, double number)
    // Format the bits in the specified 'number' to the specified 'stream'
    // from least to most significant (space separated) byte, printing
    // most-significant bits in each byte first; followed by the a colon (:),
    // the number, and a newline.
{
    printDoubleBits(stream, number) << ": " << number << endl;
}

//==========================================================================
//                      FUNCTIONS TO MANIPULATE FLOATS
//--------------------------------------------------------------------------
//  sign bit    8-bit exponent        23-bit mantissa
//     /       /                     /
//    +-+--------+-----------------------+
//    |s|e7....e0|m0..................m22|
//    +-+--------+-----------------------+
//    LSB                              MSB
//--------------------------------------------------------------------------

static ostream& printFloatBits(ostream& stream, float number)
    // Format the bits in the specified 'number' to the specified 'stream'
    // from least to most significant (space-separated) byte, printing
    // most-significant bits in each byte first.  Return a reference to the
    // modifiable 'stream'.
{
    const int SIZE = sizeof number;
    ASSERT(4 == SIZE);

    union {
        float d_float;
        unsigned char d_chars[1];
    } u;

    u.d_float = number;

    if (isLittleEndian()) {
        swap(u.d_chars + SIZE - 1, u.d_chars + 0);
        swap(u.d_chars + SIZE - 2, u.d_chars + 1);
        swap(u.d_chars + SIZE - 3, u.d_chars + 2);
        swap(u.d_chars + SIZE - 4, u.d_chars + 3);
    }

    for (int i = 0; i < SIZE; ++i) {
        if (i) {
            stream << ' ';
        }
        stream << !!(u.d_chars[i] & 0x80)
               << !!(u.d_chars[i] & 0x40)
               << !!(u.d_chars[i] & 0x20)
               << !!(u.d_chars[i] & 0x10)
               << !!(u.d_chars[i] & 0x08)
               << !!(u.d_chars[i] & 0x04)
               << !!(u.d_chars[i] & 0x02)
               << !!(u.d_chars[i] & 0x01);
    }

    return stream;
}

static void printFloat(ostream& stream, float number)
    // Format the bits in the specified 'number' to the specified 'stream'
    // from least to most significant (space separated) byte, printing
    // most-significant bits in each byte first; followed by the a colon (:),
    // the number, and a newline.
{
    printFloatBits(stream, number) << ": " << number << endl;
}

//==========================================================================
//                            USAGE EXAMPLE
//--------------------------------------------------------------------------

class MyPoint {
    int d_x;
    int d_y;

    MyPoint(const MyPoint&);            // not used in example
    MyPoint& operator=(const MyPoint&); // not used in example

  public:
    MyPoint() : d_x(0), d_y(0) {}
    MyPoint(int x, int y) : d_x(x), d_y(y) {}

    // ...

    void setX(int x) { d_x = x; }
    void setY(int y) { d_y = y; }

    int x() const { return d_x; }
    int y() const { return d_y; }
};

inline bool operator==(const MyPoint& lhs, const MyPoint& rhs)  {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}
inline bool operator!=(const MyPoint& lhs, const MyPoint& rhs)  {
    return lhs.x() != rhs.x() || lhs.y() != rhs.y();
}

int savePoint(char *buffer, int bufferLength, const MyPoint& point)
{
    enum { INSUFFICIENT_SPACE = -1, SUCCESS = 0 };
    if (bufferLength < 4 + 4) {
        return INSUFFICIENT_SPACE;
    }
    bdex_ByteStreamImpUtil::putInt32(buffer + 0, point.x());
    bdex_ByteStreamImpUtil::putInt32(buffer + 4, point.y());
    return SUCCESS;
}

int restorePoint(MyPoint *point, const char *buffer, int bufferLength)
{
    enum { SUCCESS = 0, INSUFFICIENT_DATA = 1 };
    if (bufferLength < 4 + 4) {
        return INSUFFICIENT_DATA;
    }

    int result;
    bdex_ByteStreamImpUtil::getInt32(&result, buffer + 0 );
    point->setX(result);
    bdex_ByteStreamImpUtil::getInt32(&result, buffer + 4 );
    point->setY(result);
    return SUCCESS;
}

void assignPointTheHardWay(MyPoint *dstPoint, const MyPoint& srcPoint)
{
    ASSERT(dstPoint);

    char buffer[sizeof srcPoint];

    int s;
    s = savePoint(buffer, sizeof buffer - 1, srcPoint);  ASSERT(0 != s);
    s = savePoint(buffer, sizeof buffer,     srcPoint);  ASSERT(0 == s);

    s = restorePoint(dstPoint, buffer, sizeof buffer - 1); ASSERT(0 != s);
    s = restorePoint(dstPoint, buffer, sizeof buffer    ); ASSERT(0 == s);

    ASSERT(*dstPoint == srcPoint);
}

//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 26: {
        // -----------------------------------------------------------------
        // STRESS TEST
        //   Provide mechanism to determine performance characteristics.
        //   We will put/get either an individual double or a double
        //   array a specified number of times.
        //
        // Testing:
        //   STRESS TEST - Used to determine performance characteristics.
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRESS TEST" << endl
                          << "===========" << endl;

        const int DEFAULT = argc > 2 ? 1000 : 1; // don't repeat in production.
        int mSize = argc > 2 ? (atoi(argv[2]) ? atoi(argv[2]) : DEFAULT)
                             : DEFAULT; // A zero value implies the default.

        int testIndividualDoubles = argc > 3;

        const int COUNT = 10000;
        if (testIndividualDoubles) {
            if (verbose) cout << "put/getFloat64" << endl;
            mSize *= COUNT;
        }
        else {
            if (verbose) cout << "put/getArrayFloat64" << endl;
        }
        const int SIZE = mSize;
        if (verbose) cout << "\nSIZE = " << SIZE << endl;
        const int NUM_FEEDBACKS = 50;
        int feedback = (SIZE / NUM_FEEDBACKS <= 0 ? 1 : SIZE / NUM_FEEDBACKS);

        if (verbose) cerr << "     "
            "----+----+----+----+----+----+----+----+----+----+" << endl;
        if (verbose) cerr << "BEGIN";

        int i;
        double x = 123;
        double y = 0;

        double xa[COUNT];
        double ya[COUNT];
        char b[sizeof x];
        char buffer[sizeof xa];

        for (i = 0; i < COUNT; ++i) {
            xa[i] = i;
            ya[i] = -1;
        }

        ASSERT (x != y);
        for (i = 0; i < COUNT; ++i) {
            LOOP_ASSERT(i, xa[i] != ya[i]);
        }

        if (testIndividualDoubles) {
            for (i = SIZE - 1; i >= 0; --i)  {
                if (verbose && 0 == i % feedback) cerr << '.';

                bdex_ByteStreamImpUtil::putFloat64(b, x);
                bdex_ByteStreamImpUtil::getFloat64(&y, b);
            }
            ASSERT (x == y);
            for (i = 0; i < COUNT; ++i) {
                LOOP_ASSERT(i, xa[i] != ya[i]);
            }
        }
        else {
            for (i = SIZE - 1; i >= 0; --i)  {
                if (verbose && 0 == i % feedback) cerr << '.';

                bdex_ByteStreamImpUtil::putArrayFloat64(buffer, xa, COUNT);
                bdex_ByteStreamImpUtil::getArrayFloat64(ya, buffer, COUNT);
            }

            ASSERT (x != y);
            for (i = 0; i < COUNT; ++i) {
                LOOP_ASSERT(i, xa[i] == ya[i]);
            }
        }

        if (verbose) cerr << "END" << endl;

      } break;
      case 25: {
        // -----------------------------------------------------------------
        // USAGE EXAMPLE
        //   Verify that we can put/get a MyPoint type via the public
        //   interface.  Demonstrate that we can assign a value in this way.
        //
        // Testing:
        //   USAGE TEST - Make sure usage example compiles/ works as advertized
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE TEST" << endl
                          << "==================" << endl;

        MyPoint x(3, 2);
        MyPoint y;                      ASSERT(x != y); ASSERT(!(x == y));
        assignPointTheHardWay(&y, x);   ASSERT(x == y); ASSERT(!(x != y));

      } break;
      case 24: {
        // -----------------------------------------------------------------
        // PUT/GET 32-BIT FLOAT ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayFloat32(char *buf, const float *ary, int cnt);
        //   getArrayFloat32(float *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 32-Bit Float Arrays" << endl
                          << "===========================" << endl;

        typedef float T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20

        // 2^23 + 1 = 8388609 sets the LSB

        const T A = 8 * M + 1 * 1
                          + 2 * Z
                          + 3 * Z * Z;

        const char *const A_SPEC = "01001011 00000011 00000010 00000001";

        const T B = -8 * M - 14 * 1
                           - 13 * Z
                           - 12 * Z * Z;

        const char *const B_SPEC = "11001011 00001100 00001101 00001110";

        const T C = 8 * M + 4 * 1
                          + 5 * Z
                          + 6 * Z * Z;

        const char *const C_SPEC = "01001011 00000110 00000101 00000100";

        const T D = -8 * M - 11 * 1
                           - 10 * Z
                           -  9 * Z * Z;

        const char *const D_SPEC = "11001011 00001001 00001010 00001011";


        const T W = 4 * M - 1;          // 2^22 - 1
        const T E = 8 * M + W + W + 1 ; // 2^24 - 1

        const char *const E_SPEC = "01001011 01111111 11111111 11111111";


        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 4;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;
        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putFloat32(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x00';
        const char YY = '\xff';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayFloat32(
                                        buffer1 + align, input, length);
                bdex_ByteStreamImpUtil::putArrayFloat32(
                                        buffer2 + align, input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                T result2[NUM_TRIALS];
                const T POS = +2.0F;     // mantissa is 0000 0000 0000 ...
                const T NEG = -1.6F;     // mantissa is 1001 1001 1001 ...
                {
                    if (veryVerbose) {
                        P_(POS); P(NEG);
                        printFloat(cout, POS);
                        printFloat(cout, NEG);
                    }
                }

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // mantissa should be zero filled
                    result2[i] = NEG;   // mantissa should be zero filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayFloat32(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayFloat32(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 23: {
        // -----------------------------------------------------------------
        // PUT/GET 64-BIT FLOAT ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayFloat64(char *buf, const double *ary, int cnt);
        //   getArrayFloat64(double *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 64-Bit Float Arrays" << endl
                          << "===========================" << endl;

        typedef double T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20

        // 2^52 + 1 = 4503599627370497 sets the LSB

        const T A = 4 * K * M * M + 1 * 1
                                  + 2 * Z
                                  + 3 * Z * Z
                                  + 4 * Z * Z * Z
                                  + 5 * Z * Z * Z * Z
                                  + 6 * Z * Z * Z * Z * Z
                                  + 7 * Z * Z * Z * Z * Z * Z;

        const char *const A_SPEC =
                "01000011 00110111 00000110 00000101"
                "00000100 00000011 00000010 00000001";

        const T B = -4 * K * M * M - 14 * 1
                                   - 13 * Z
                                   - 12 * Z * Z
                                   - 11 * Z * Z * Z
                                   - 10 * Z * Z * Z * Z
                                   -  9 * Z * Z * Z * Z * Z
                                   -  8 * Z * Z * Z * Z * Z * Z;

        const char *const B_SPEC =
                "11000011 00111000 00001001 00001010"
                "00001011 00001100 00001101 00001110";

        const T C = 4 * K * M * M + 7 * 1
                                  + 6 * Z
                                  + 5 * Z * Z
                                  + 4 * Z * Z * Z
                                  + 3 * Z * Z * Z * Z
                                  + 2 * Z * Z * Z * Z * Z
                                  + 1 * Z * Z * Z * Z * Z * Z;

        const char *const C_SPEC =
                "01000011 00110001 00000010 00000011"
                "00000100 00000101 00000110 00000111";

        const T D = -4 * K * M * M - 8 * 1
                                   - 9 * Z
                                   - 10 * Z * Z
                                   - 11 * Z * Z * Z
                                   - 12 * Z * Z * Z * Z
                                   - 13 * Z * Z * Z * Z * Z
                                   - 14 * Z * Z * Z * Z * Z * Z;

        const char *const D_SPEC =
                "11000011 00111110 00001101 00001100"
                "00001011 00001010 00001001 00001000";

        const T W = 2 * K * M * M - 1;                  // 2^51 - 1
        const T E = 4 * K * M * M + W + W + 1 ;         // 2^53 - 1

        const char *const E_SPEC =
                "01000011 00111111 11111111 11111111"
                "11111111 11111111 11111111 11111111";


        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 8;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putFloat64(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x00';
        const char YY = '\xff';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayFloat64(
                                        buffer1 + align, input, length);
                bdex_ByteStreamImpUtil::putArrayFloat64(
                                        buffer2 + align, input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                T result2[NUM_TRIALS];
                const T POS = +2.0;     // mantissa is 0000 0000 0000 ...
                const T NEG = -1.6;     // mantissa is 1001 1001 1001 ...
                {
                    if (veryVerbose) {
                        P_(POS); P(NEG);
                        printDouble(cout, POS);
                        printDouble(cout, NEG);
                    }
                }

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // mantissa should be zero filled
                    result2[i] = NEG;   // mantissa should be zero filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayFloat64(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayFloat64(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 22: {
        // -----------------------------------------------------------------
        // PUT/GET 8-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt8(char *buf, const char *ary, int cnt);
        //   putArrayInt8(char *buf, const signed char *ary, int cnt);
        //   putArrayInt8(char *buf, const unsigned char *ary, int cnt);
        //   getArrayInt8(char *ary, const char *buf, int cnt);
        //   getArrayInt8(signed char *ary, const char *buf, int cnt);
        //   getArrayInt8(unsigned char *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 6-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef char T;
        typedef signed char S;
        typedef unsigned char U;

        const T A = (T)  0x05;  // POSITIVE NUMBER
        const char *A_SPEC = "00000101";

        const T B = (T)  0x80;  // NEGATIVE NUMBER
        const char *B_SPEC = "10000000";

        const T C = (T)  0x10;  // POSITIVE NUMBER
        const char *C_SPEC = "00010000";

        const T D = (T)  0x08;  // POSITIVE NUMBER
        const char *D_SPEC = "00001000";

        const T E = (T)  0xfd;  // NEGATIVE NUMBER
        const char *E_SPEC = "11111101";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 1;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt8(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i] & 0xFF)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];  // char
        char buffer2[SIZE * (1 + NUM_TRIALS)];  // unsigned char
        char buffer3[SIZE * (1 + NUM_TRIALS)];  // signed char

        const char XX = '\x69';                 // char
        const char YY = '\xA5';                 // unsigned char
        const char ZZ = '\x58';                 // signed char

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);
                memset(buffer3, ZZ, sizeof buffer3);

                bdex_ByteStreamImpUtil::putArrayInt8(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt8(
                                        buffer2 + align, (U*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt8(
                                        buffer3 + align, (S*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xFF)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xFF)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer3 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xff)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer3 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                    LOOP3_ASSERT(length, align, k, ZZ == buffer3[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                    LOOP3_ASSERT(length, align, k, ZZ == buffer3[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                S result3[NUM_TRIALS];
                const S POS = +1;
                const U NEG = (U)(S)-1;

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // could be signed or unsigned
                    result2[i] = NEG;   // no issue of filling here
                    result3[i] = POS;   // no issue of filling here
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) != result2[i]);
                    LOOP3_ASSERT(length, align, i, S(input[i]) != result3[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt8(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayInt8(result2,
                                        buffer2 + align, length);
                bdex_ByteStreamImpUtil::getArrayInt8(result3,
                                        buffer3 + align, length);
                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || U(input[i]) != result2[i]
                                    || S(input[i]) != result3[i]) {
                        P(i);    P(input[i] & 0xff);    P(result1[i] & 0xff);
                                 P(result2[i] & 0xff);  P(result3[i] & 0xff);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) == result2[i]);
                    LOOP3_ASSERT(length, align, i, S(input[i]) == result3[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, T(POS) == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(NEG) == result2[i]);
                    LOOP3_ASSERT(length, align, i, S(POS) == result3[i]);
                }
            }
        }
      } break;
      case 21: {
        // -----------------------------------------------------------------
        // PUT/GET 16-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt16(char *buf, const short *ary, int cnt);
        //   putArrayInt16(char *buf, const unsigned short *ary, int cnt);
        //   getArrayInt16(short *ary, const char *buf, int cnt);
        //   getArrayUint16(unsigned short *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 16-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef short T;
        typedef unsigned short U;

        const T A = (     (T)  0x01 << 8) + 0x02;  // POSITIVE NUMBER
        const char *A_SPEC = "00000001 00000010";

        const T B = (T) ( (T)  0x80 << 8) + 0x70;  // NEGATIVE NUMBER
        const char *B_SPEC = "10000000 01110000";

        const T C =     ( (T)  0x10 << 8) + 0x20;  // POSITIVE NUMBER
        const char *C_SPEC = "00010000 00100000";

        const T D =     ( (T)  0x08 << 8) + 0x07;  // POSITIVE NUMBER
        const char *D_SPEC = "00001000 00000111";

        const T E = (T) ( (T)  0xFF << 8) + 0xFE;  // NEGATIVE NUMBER
        const char *E_SPEC = "11111111 11111110";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 2;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt16(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt16(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt16(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U)(T)-1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) != result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt16(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint16(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || U(input[i]) != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 20: {
        // -----------------------------------------------------------------
        // PUT/GET 24-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt24(char *buf, const int *ary, int cnt);
        //   putArrayInt24(char *buf, const unsigned int *ary, int cnt);
        //   getArrayInt24(int *ary, const char *buf, int cnt);
        //   getArrayUint24(unsigned int *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 24-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef int T;
        typedef unsigned int U;

        const T A = (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04;

        const char *A_SPEC = "00000000 00000010 00000011 00000100";

        const T B = (( ( (T)  // NEGATIVE NUMBER
                        0xFF << 8) + 0x80 << 8) + 0x70 << 8) + 0x60;

        const char *B_SPEC = "11111111 10000000 01110000 01100000";

        const T C = (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x20 << 8) + 0x30 << 8) + 0x40;

        const char *C_SPEC = "00000000 00100000 00110000 01000000";

        const T D = (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x07 << 8) + 0x06 << 8) + 0x05;

        const char *D_SPEC = "00000000 00000111 00000110 00000101";

        const T E = (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xfe << 8) + 0xfd << 8) + 0xfc;

        const char *E_SPEC = "11111111 11111110 11111101 11111100";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 3;
        const int OFFSET = 1;
        const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
        const int NUM_BITS_IN_BUFFER = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE + OFFSET];
                g(exp, SPECS[i], NUM_BITS_IN_VALUE);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt24(buffer, VALUES[i]);
                bool isEq = eq(exp + OFFSET, buffer, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xA5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt24(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt24(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE + OFFSET];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS_IN_VALUE);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp + OFFSET,
                                   buffer1 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp + OFFSET, buffer2 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U) -1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt24(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint24(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || ((input[i] != (T) result2[i])
                                        && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0)
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 19: {
        // -----------------------------------------------------------------
        // PUT/GET 32-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt32(char *buf, const int *ary, int cnt);
        //   putArrayInt32(char *buf, const unsigned int *ary, int cnt);
        //   getArrayInt32(int *ary, const char *buf, int cnt);
        //   getArrayUint32(unsigned int *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 32-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef int T;
        typedef unsigned int U;

        const T A = (( ( (T)  // POSITIVE NUMBER
                        0x01 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04;

        const char *A_SPEC = "00000001 00000010 00000011 00000100";

        const T B = (( ( (T)  // NEGATIVE NUMBER
                        0x80 << 8) + 0x70 << 8) + 0x60 << 8) + 0x50;

        const char *B_SPEC = "10000000 01110000 01100000 01010000";

        const T C = (( ( (T)  // POSITIVE NUMBER
                        0x10 << 8) + 0x20 << 8) + 0x30 << 8) + 0x40;

        const char *C_SPEC = "00010000 00100000 00110000 01000000";

        const T D = (( ( (T)  // POSITIVE NUMBER
                        0x08 << 8) + 0x07 << 8) + 0x06 << 8) + 0x05;

        const char *D_SPEC = "00001000 00000111 00000110 00000101";

        const T E = (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xfe << 8) + 0xfd << 8) + 0xfc;

        const char *E_SPEC = "11111111 11111110 11111101 11111100";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 4;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt32(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xA5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt32(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt32(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length; k < (int)sizeof buffer1; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U) -1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt32(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint32(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != (T) result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 18: {
        // -----------------------------------------------------------------
        // PUT/GET 40-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt40(char *buf, const Int64 *ary, int cnt);
        //   putArrayInt40(char *buf, const Uint64 *ary, int cnt);
        //   getArrayInt40(Int64 *ary, const char *buf, int cnt);
        //   getArrayUint40(Uint64 *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 40-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x00 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000000 00000000 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0xff << 8) + 0x80 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 11111111 11111111 10000000"
                             "01000000 00110000 00100000 00010000";

        const T C = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x00 << 8) + 0x40 << 8)
                      + 0x50 << 8) + 0x60 << 8) + 0x70 << 8) + 0x80;

        const char *C_SPEC = "00000000 00000000 00000000 01000000"
                             "01010000 01100000 01110000 10000000";

        const T D = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x00 << 8) + 0x05 << 8)
                      + 0x04 << 8) + 0x03 << 8) + 0x02 << 8) + 0x01;

        const char *D_SPEC = "00000000 00000000 00000000 00000101"
                             "00000100 00000011 00000010 00000001";

        const T E = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0xff << 8) + 0xfc << 8)
                      + 0xfb << 8) + 0xfa << 8) + 0xf9 << 8) + 0xf8;

        const char *E_SPEC = "11111111 11111111 11111111 11111100"
                             "11111011 11111010 11111001 11111000";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 5;
        const int OFFSET = 3;
        const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
        const int NUM_BITS_IN_BUFFER = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE + OFFSET];
                g(exp, SPECS[i], NUM_BITS_IN_VALUE);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt40(buffer, VALUES[i]);
                bool isEq = eq(exp + OFFSET, buffer, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt40(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt40(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE + OFFSET];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS_IN_VALUE);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp + OFFSET,
                                   buffer1 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp + OFFSET, buffer2 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length; k < (int)sizeof buffer1;
                                                                         ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U)(T)-1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt40(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint40(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || ((input[i] != (T) result2[i])
                                        && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0)
                        LOOP3_ASSERT(length, align, i,
                            input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 17: {
        // -----------------------------------------------------------------
        // PUT/GET 48-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt48(char *buf, const Int64 *ary, int cnt);
        //   putArrayInt48(char *buf, const Uint64 *ary, int cnt);
        //   getArrayInt48(Int64 *ary, const char *buf, int cnt);
        //   getArrayUint48(Uint64 *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 48-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x03 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000000 00000011 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0x80 << 8) + 0x50 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 11111111 10000000 01010000"
                             "01000000 00110000 00100000 00010000";

        const T C = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x30 << 8) + 0x40 << 8)
                      + 0x50 << 8) + 0x60 << 8) + 0x70 << 8) + 0x80;

        const char *C_SPEC = "00000000 00000000 00110000 01000000"
                             "01010000 01100000 01110000 10000000";

        const T D = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x06 << 8) + 0x05 << 8)
                      + 0x04 << 8) + 0x03 << 8) + 0x02 << 8) + 0x01;

        const char *D_SPEC = "00000000 00000000 00000110 00000101"
                             "00000100 00000011 00000010 00000001";

        const T E = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0xfd << 8) + 0xfc << 8)
                      + 0xfb << 8) + 0xfa << 8) + 0xf9 << 8) + 0xf8;

        const char *E_SPEC = "11111111 11111111 11111101 11111100"
                             "11111011 11111010 11111001 11111000";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 6;
        const int OFFSET = 2;
        const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
        const int NUM_BITS_IN_BUFFER = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE + OFFSET];
                g(exp, SPECS[i], NUM_BITS_IN_VALUE);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt48(buffer, VALUES[i]);
                bool isEq = eq(exp + OFFSET, buffer, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt48(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt48(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE + OFFSET];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS_IN_VALUE);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp + OFFSET,
                                   buffer1 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp + OFFSET, buffer2 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length; k < (int)sizeof buffer1;
                                                                         ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U)(T)-1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt48(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint48(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || ((input[i] != (T) result2[i])
                                        && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0)
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 16: {
        // -----------------------------------------------------------------
        // PUT/GET 56-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt56(char *buf, const Int64 *ary, int cnt);
        //   putArrayInt56(char *buf, const Uint64 *ary, int cnt);
        //   getArrayInt56(Int64 *ary, const char *buf, int cnt);
        //   getArrayUint56(Uint64 *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 56-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000010 00000011 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0x80 << 8) + 0x60 << 8) + 0x50 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 10000000 01100000 01010000"
                             "01000000 00110000 00100000 00010000";

        const T C = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x20 << 8) + 0x30 << 8) + 0x40 << 8)
                      + 0x50 << 8) + 0x60 << 8) + 0x70 << 8) + 0x80;

        const char *C_SPEC = "00000000 00100000 00110000 01000000"
                             "01010000 01100000 01110000 10000000";

        const T D = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x07 << 8) + 0x06 << 8) + 0x05 << 8)
                      + 0x04 << 8) + 0x03 << 8) + 0x02 << 8) + 0x01;

        const char *D_SPEC = "00000000 00000111 00000110 00000101"
                             "00000100 00000011 00000010 00000001";

        const T E = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xfe << 8) + 0xfd << 8) + 0xfc << 8)
                      + 0xfb << 8) + 0xfa << 8) + 0xf9 << 8) + 0xf8;

        const char *E_SPEC = "11111111 11111110 11111101 11111100"
                             "11111011 11111010 11111001 11111000";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 7;
        const int OFFSET = 1;
        const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
        const int NUM_BITS_IN_BUFFER = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE + OFFSET];
                g(exp, SPECS[i], NUM_BITS_IN_VALUE);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt56(buffer, VALUES[i]);
                bool isEq = eq(exp + OFFSET, buffer, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt56(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt56(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE + OFFSET];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS_IN_VALUE);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp + OFFSET,
                                   buffer1 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE + OFFSET) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp + OFFSET, buffer2 + off, NUM_BITS_IN_BUFFER);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length; k < (int)sizeof buffer1;
                                                                         ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U)(T)-1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt56(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint56(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || ((input[i] != (T) result2[i])
                                        && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0)
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 15: {
        // -----------------------------------------------------------------
        // PUT/GET 64-BIT INTEGER ARRAYS
        //   We can assume that individual elements can be written/read.
        //   The purpose here is to test that arrays of length 0, 1, 2, ...
        //   write properly and can be read for both signed and unsigned
        //   values.  We'll re-check buffer alignment for good measure.
        //
        // Testing:
        //   putArrayInt64(char *buf, const Int64 *ary, int cnt);
        //   putArrayInt64(char *buf, const Uint64 *ary, int cnt);
        //   getArrayInt64(Int64 *ary, const char *buf, int cnt);
        //   getArrayUint64(Uint64 *ary, const char *buf, int cnt);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 64-Bit Integer Arrays" << endl
                          << "=============================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x01 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000001 00000010 00000011 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0x80 << 8) + 0x70 << 8) + 0x60 << 8) + 0x50 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "10000000 01110000 01100000 01010000"
                             "01000000 00110000 00100000 00010000";

        const T C = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x10 << 8) + 0x20 << 8) + 0x30 << 8) + 0x40 << 8)
                      + 0x50 << 8) + 0x60 << 8) + 0x70 << 8) + 0x80;

        const char *C_SPEC = "00010000 00100000 00110000 01000000"
                             "01010000 01100000 01110000 10000000";

        const T D = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x08 << 8) + 0x07 << 8) + 0x06 << 8) + 0x05 << 8)
                      + 0x04 << 8) + 0x03 << 8) + 0x02 << 8) + 0x01;

        const char *D_SPEC = "00001000 00000111 00000110 00000101"
                             "00000100 00000011 00000010 00000001";

        const T E = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xfe << 8) + 0xfd << 8) + 0xfc << 8)
                      + 0xfb << 8) + 0xfa << 8) + 0xf9 << 8) + 0xf8;

        const char *E_SPEC = "11111111 11111110 11111101 11111100"
                             "11111011 11111010 11111001 11111000";

        T VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int NUM_VALUES = sizeof VALUES/sizeof *VALUES;
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 8;             // size (in bytes) of data in buffer
        const int NUM_BITS = 8 * SIZE;

        int i, k;

        ASSERT(SIZE <= (int)sizeof(T));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                char exp[SIZE];
                g(exp, SPECS[i], NUM_BITS);
                char buffer[SIZE];
                bdex_ByteStreamImpUtil::putInt64(buffer, VALUES[i]);
                bool isEq = eq(exp, buffer, NUM_BITS);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                bdex_ByteStreamImpUtil::putArrayInt64(
                                        buffer1 + align, (T*) input, length);
                bdex_ByteStreamImpUtil::putArrayInt64(
                                        buffer2 + align, (U*) input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    char exp[SIZE];
                    g(exp, SPECS[i % NUM_VALUES], NUM_BITS);
                    int off = align + SIZE * i;

                    bool isEq = eq(exp, buffer1 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = eq(exp, buffer2 + off, NUM_BITS);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length; k < (int)sizeof buffer1;
                                                                         ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T result1[NUM_TRIALS];
                U result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = (U)(T)-1;
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                bdex_ByteStreamImpUtil::getArrayInt64(result1,
                                        buffer1 + align, length);
                bdex_ByteStreamImpUtil::getArrayUint64(result2,
                                        buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != (T) result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }
      } break;
      case 14: {
        // -----------------------------------------------------------------
        // PUT/GET 32-BIT FLOATS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putFloat32(char *buf, float val);
        //   getFloat32(float *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 32-Bit Floats" << endl
                          << "=====================" << endl;

        typedef float T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20
        const T B = K * M; // 2^30

        const T W = 4 * M - 1;                  // 2^22 - 1
        const T ONES = 8 * M + W + W + 1 ;      // 2^24 - 1

        const char *const ONES_SPEC =
                "01001011 01111111 11111111 11111111";

        // 2^23 + 1 = 8388609 sets the LSB

        const T X = 8 * M + 1 * 1
                          + 2 * Z
                          + 3 * Z * Z;

        const char *const X_SPEC =
                "01001011 00000011 00000010 00000001";

        const T Y = -8 * M - 14 * 1
                           - 13 * Z
                           - 12 * Z * Z;

        const char *const Y_SPEC =
                "11001011 00001100 00001101 00001110";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "0 0..0"                                },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "1 0..0"                                },
#else
            { L_,  -0.0,        "0 0..0"                                },
#endif
            { L_,   ONES,       ONES_SPEC                               },

            // small integers
            { L_,   5.0,        "0 100 0000 1  010 0000 0..0"           },
            { L_,   4.0,        "0 100 0000 1  000 0000 0..0"           },
            { L_,   3.75,       "0 100 0000 0  111 0000 0..0"           },
            { L_,   3.5,        "0 100 0000 0  110 0000 0..0"           },
            { L_,   3.25,       "0 100 0000 0  101 0000 0..0"           },
            { L_,   3.0,        "0 100 0000 0  100 0000 0..0"           },
            { L_,   2.75,       "0 100 0000 0  011 0000 0..0"           },
            { L_,   2.5 ,       "0 100 0000 0  010 0000 0..0"           },
            { L_,   2.0,        "0 100 0000 0  000 0000 0..0"           },
            { L_,   1.5,        "0 011 1111 1  100 0000 0..0"           },
            { L_,   1.0,        "0 011 1111 1  000 0000 0..0"           },

            // base-2 fractions
            { L_,   0.75,       "0 011 1111 0  100 0000 0..0"           },
            { L_,   0.625,      "0 011 1111 0  010 0000 0..0"           },
            { L_,   0.5,        "0 011 1111 0  000 0000 0..0"           },
            { L_,   0.375,      "0 011 1110 1  100 0000 0..0"           },
            { L_,   0.25,       "0 011 1110 1  000 0000 0..0"           },
            { L_,   0.125,      "0 011 1110 0  000 0000 0..0"           },

            // larger integers
            { L_,   1020,       "0 100 0100 0  111 1111 0000 0..0"      },
            { L_,   1021,       "0 100 0100 0  111 1111 0100 0..0"      },
            { L_,   1022,       "0 100 0100 0  111 1111 1000 0..0"      },
            { L_,   1023,       "0 100 0100 0  111 1111 1100 0..0"      },
            { L_,   1024,       "0 100 0100 1  000 0000 0000 0..0"      },
            { L_,   1025,       "0 100 0100 1  000 0000 0010 0..0"      },
            { L_,   1026,       "0 100 0100 1  000 0000 0100 0..0"      },
            { L_,   1027,       "0 100 0100 1  000 0000 0110 0..0"      },

            // really big integers (powers of 2)
            { L_,   K,          "0 100 0100 1  000 0000 0..0"           },
            { L_,   M,          "0 100 1001 1  000 0000 0..0"           },
            { L_,   B,          "0 100 1110 1  000 0000 0..0"           },
            { L_,   K * B,      "0 101 0011 1  000 0000 0..0"           },
            { L_,   M * B,      "0 101 1000 1  000 0000 0..0"           },
            { L_,   B * B,      "0 101 1101 1  000 0000 0..0"           },
            { L_,   K * B * B,  "0 110 0010 1  000 0000 0..0"           },
            { L_,   M * B * B,  "0 110 0111 1  000 0000 0..0"           },
            { L_,   B * B * B,  "0 110 1100 1  000 0000 0..0"           },

            // least significant bits of mantissa
            { L_,   1*M + 1,    "0 100 1001 1  000 0000 0..0 1000"      },
            { L_,   2*M + 1,    "0 100 1010 0  000 0000 0..0 0100"      },
            { L_,   3*M + 1,    "0 100 1010 0  100 0000 0..0 0100"      },
            { L_,   4*M + 1,    "0 100 1010 1  000 0000 0..0 0010"      },
            { L_,   5*M + 1,    "0 100 1010 1  010 0000 0..0 0010"      },
            { L_,   6*M + 1,    "0 100 1010 1  100 0000 0..0 0010"      },
            { L_,   7*M + 1,    "0 100 1010 1  110 0000 0..0 0010"      },
            { L_,   8*M + 1,    "0 100 1011 0  000 0000 0..0 0001"      },
            { L_,   9*M + 2,    "0 100 1011 0  001 0000 0..0 0010"      },
            { L_,  10*M + 5,    "0 100 1011 0  010 0000 0..0 0101"      },

            // test each byte independently
            { L_,   X,          X_SPEC                                  },
            { L_,   Y,          Y_SPEC                                  },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T number = DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 4;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];

            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat32(buffer + i, number);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(number)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                // check negative
                memset(buffer, '\x32', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat32(buffer + i, -number);
                buffer[i] ^=0x80;
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                // check positive again
                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat32(buffer + i, number);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static T INITIAL_VALUES[6];
                INITIAL_VALUES[0] = 0.0000001F;
                INITIAL_VALUES[1] = -0.0000001F;
                INITIAL_VALUES[2] = 1.234567F;
                INITIAL_VALUES[3]  = -765432.1F;
                INITIAL_VALUES[4] = ONES-1;
                INITIAL_VALUES[5] = 1-ONES;

                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        bdex_ByteStreamImpUtil::getFloat32(&x, buffer + i);
                        if (number != x) {
                            P_(number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, number == x);
                    }
                    {   // negate buffer value
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        buffer[i] ^=0x80;
                        bdex_ByteStreamImpUtil::getFloat32(&x, buffer + i);
                        buffer[i] ^=0x80;
                        if (-number != x) {
                            P_(-number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, -number == x);
                    }

                }
            }
        }
      } break;
      case 13: {
        // -----------------------------------------------------------------
        // PUT/GET 64-BIT FLOATS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putFloat64(char *buf, double val);
        //   getFloat64(double *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 64-Bit Floats" << endl
                          << "=====================" << endl;

        typedef double T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20
        const T B = K * M; // 2^30

        const T W = 2 * B * M - 1;              // 2^51 - 1
        const T ONES = 4 * B * M + W + W + 1 ;  // 2^53 - 1

        const char *const ONES_SPEC =
                "01000011 00111111 11111111 11111111"
                "11111111 11111111 11111111 11111111";

        // 2^52 + 1 = 4503599627370497 sets the LSB

        const T X = 4 * M * B + 1 * 1
                              + 2 * Z
                              + 3 * Z * Z
                              + 4 * Z * Z * Z
                              + 5 * Z * Z * Z * Z
                              + 6 * Z * Z * Z * Z * Z
                              + 7 * Z * Z * Z * Z * Z * Z;

        const char *const X_SPEC =
                "01000011 00110111 00000110 00000101"
                "00000100 00000011 00000010 00000001";

        const T Y = -4 * M * B - 14 * 1
                               - 13 * Z
                               - 12 * Z * Z
                               - 11 * Z * Z * Z
                               - 10 * Z * Z * Z * Z
                                - 9 * Z * Z * Z * Z * Z
                                - 8 * Z * Z * Z * Z * Z * Z;

        const char *const Y_SPEC =
                "11000011 00111000 00001001 00001010"
                "00001011 00001100 00001101 00001110";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "0 0..0"                                },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "1 0..0"                                },
#else
            { L_,  -0.0,        "0 0..0"                                },
#endif
            { L_,   ONES,       ONES_SPEC                               },

            // small integers
            { L_,   5.00,       "0 100 0000 0001 0100 0000 0..0"        },
            { L_,   4.00,       "0 100 0000 0001 0000 0000 0..0"        },
            { L_,   3.75,       "0 100 0000 0000 1110 0000 0..0"        },
            { L_,   3.5,        "0 100 0000 0000 1100 0000 0..0"        },
            { L_,   3.25,       "0 100 0000 0000 1010 0000 0..0"        },
            { L_,   3.0 ,       "0 100 0000 0000 1000 0000 0..0"        },
            { L_,   2.75,       "0 100 0000 0000 0110 0000 0..0"        },
            { L_,   2.5,        "0 100 0000 0000 0100 0000 0..0"        },
            { L_,   2.0,        "0 100 0000 0000 0000 0000 0..0"        },
            { L_,   1.5,        "0 011 1111 1111 1000 0000 0..0"        },
            { L_,   1.0,        "0 011 1111 1111 0000 0000 0..0"        },

            // base-2 fractions
            { L_,   0.75,       "0 011 1111 1110 1000 0000 0..0"        },
            { L_,   0.625,      "0 011 1111 1110 0100 0000 0..0"        },
            { L_,   0.5,        "0 011 1111 1110 0000 0000 0..0"        },
            { L_,   0.375,      "0 011 1111 1101 1000 0000 0..0"        },
            { L_,   0.25,       "0 011 1111 1101 0000 0000 0..0"        },
            { L_,   0.125,      "0 011 1111 1100 0000 0000 0..0"        },

            // larger integers
            { L_,   1020,       "0 100 0000 1000  1111 1110 0000 0..0"  },
            { L_,   1021,       "0 100 0000 1000  1111 1110 1000 0..0"  },
            { L_,   1022,       "0 100 0000 1000  1111 1111 0000 0..0"  },
            { L_,   1023,       "0 100 0000 1000  1111 1111 1000 0..0"  },
            { L_,   1024,       "0 100 0000 1001  0000 0000 0000 0..0"  },
            { L_,   1025,       "0 100 0000 1001  0000 0000 0100 0..0"  },
            { L_,   1026,       "0 100 0000 1001  0000 0000 1000 0..0"  },
            { L_,   1027,       "0 100 0000 1001  0000 0000 1100 0..0"  },

            // really big integers (powers of 2)
            { L_,   K,          "0 100 0000 1001  0000 0000 0000 0..0"  },
            { L_,   M,          "0 100 0001 0011  0000 0000 0000 0..0"  },
            { L_,   B,          "0 100 0001 1101  0000 0000 0000 0..0"  },
            { L_,   K * B,      "0 100 0010 0111  0000 0000 0000 0..0"  },
            { L_,   M * B,      "0 100 0011 0001  0000 0000 0000 0..0"  },
            { L_,   B * B,      "0 100 0011 1011  0000 0000 0000 0..0"  },
            { L_,   K * B * B,  "0 100 0100 0101  0000 0000 0000 0..0"  },
            { L_,   M * B * B,  "0 100 0100 1111  0000 0000 0000 0..0"  },
            { L_,   B * B * B,  "0 100 0101 1001  0000 0000 0000 0..0"  },

            // least significant bits of mantissa
            { L_,   1*M*B + 1,  "0 100 0011 0001  0000 0..0 0000 0100"  },
            { L_,   2*M*B + 1,  "0 100 0011 0010  0000 0..0 0000 0010"  },
            { L_,   3*M*B + 1,  "0 100 0011 0010  1000 0..0 0000 0010"  },
            { L_,   4*M*B + 1,  "0 100 0011 0011  0000 0..0 0000 0001"  },
            { L_,   5*M*B + 2,  "0 100 0011 0011  0100 0..0 0000 0010"  },
            { L_,   6*M*B + 3,  "0 100 0011 0011  1000 0..0 0000 0011"  },
            { L_,   7*M*B + 4,  "0 100 0011 0011  1100 0..0 0000 0100"  },

            // test each byte independently
            { L_,   X,          X_SPEC                                  },
            { L_,   Y,          Y_SPEC                                  },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T number = DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 8;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];

            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat64(buffer + i, number);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(number)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                // check negative
                memset(buffer, '\x64', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat64(buffer + i, -number);
                buffer[i] ^=0x80;
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                // check positive again
                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putFloat64(buffer + i, number);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static T INITIAL_VALUES[6];
                INITIAL_VALUES[0] = 0.0000001,
                INITIAL_VALUES[1] = -0.0000001;
                INITIAL_VALUES[2] = 1.234567;
                INITIAL_VALUES[3] = -765432.1;
                INITIAL_VALUES[4] = ONES-1;
                INITIAL_VALUES[5] = 1-ONES;

                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        bdex_ByteStreamImpUtil::getFloat64(&x, buffer + i);
                        if (number != x) {
                            P_(number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, number == x);
                    }
                    {   // negate buffer value
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        buffer[i] ^=0x80;
                        bdex_ByteStreamImpUtil::getFloat64(&x, buffer + i);
                        buffer[i] ^=0x80;
                        if (-number != x) {
                            P_(-number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, -number == x);
                    }

                }
            }
        }
      } break;
      case 12: {
        // -----------------------------------------------------------------
        // PUT/GET 8-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt8(char *buf, int val);
        //   getInt8(char *var, const char *buf);
        //   getInt8(signed char *var, const char *buf);
        //   getInt8(unsigned char *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 8-Bit Integers" << endl
                          << "=======================" << endl;

        typedef char T;
        typedef unsigned char U;
        typedef signed char S;

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  0x71,        "0111 0001"                             },
            { L_,  (T) 0x8C,    "1000 1100"                             },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T xsignedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const S signedNumber = (S) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 1;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt8(buffer + i, xsignedNumber);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(xsignedNumber & 0xff)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xa5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt8(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, xsignedNumber != x);
                        bdex_ByteStreamImpUtil::getInt8(&x, buffer + i);
                        if (xsignedNumber != x) {
                            P_(xsignedNumber & 0xff) P(x & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, xsignedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        bdex_ByteStreamImpUtil::getInt8(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber & 0xff) P(y & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                    {
                        S z = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != z);
                        bdex_ByteStreamImpUtil::getInt8(&z, buffer + i);
                        if (signedNumber != z) {
                            P_(signedNumber & 0xff) P(z & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == z);
                   }
                }
            }
        }

      } break;
      case 11: {
        // -----------------------------------------------------------------
        // PUT/GET 16-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt16(char *buf, int val);
        //   getInt16(short *var, const char *buf);
        //   getUint16(unsigned short *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 16-Bit Integers" << endl
                          << "=======================" << endl;

        typedef short T;
        typedef unsigned short U;

        const T A = (0x01 << 8) + 0x02;      // POSITIVE NUMBER

        const char *A_SPEC = "00000001 00000010";

        const T B = (T) (0x80 << 8) + 0x70; // NEGATIVE NUMBER

        const char *B_SPEC = "10000000 01110000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 2;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt16(buffer + i, signedNumber);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt16(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt16(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        bdex_ByteStreamImpUtil::getUint16(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                }
            }
        }
      } break;
      case 10: {
        // -----------------------------------------------------------------
        // PUT/GET 24-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt24(char *buf, int val);
        //   getInt24(int *var, const char *buf);
        //   getUint24(unsigned int *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 24-Bit Integers" << endl
                          << "=======================" << endl;

        typedef int T;
        typedef unsigned int U;
        const T A = (( ( // POSITIVE NUMBER
                        0x00 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04;

        const char *A_SPEC = "00000000 00000010 00000011 00000100";

        const T B = (( ( // NEGATIVE NUMBER
                        0xff << 8) + 0x80 << 8) + 0x60 << 8) + 0x50;

        const char *B_SPEC = "11111111 10000000 01100000 01010000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 3;
            const int OFFSET = 1;
            const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
            const int NUM_BITS_IN_BUFFER = 8 * SIZE;
            char exp[SIZE + OFFSET];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS_IN_VALUE));


            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt24(buffer + i, signedNumber);
                bool isEq = eq(exp + OFFSET, buffer + i, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt24(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp + OFFSET,
                                         buffer + i,   NUM_BITS_IN_BUFFER));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt24(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            bdex_ByteStreamImpUtil::getUint24(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }
      } break;
      case 9: {
        // -----------------------------------------------------------------
        // PUT/GET 32-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt32(char *buf, int val);
        //   getInt32(int *var, const char *buf);
        //   getUint32(unsigned int *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 32-Bit Integers" << endl
                          << "=======================" << endl;

        typedef int T;
        typedef unsigned int U;

        const T A = (( ( // POSITIVE NUMBER
                        0x01 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04;

        const char *A_SPEC = "00000001 00000010 00000011 00000100";

        const T B = (( ( // NEGATIVE NUMBER
                        0x80 << 8) + 0x70 << 8) + 0x60 << 8) + 0x50;

        const char *B_SPEC = "10000000 01110000 01100000 01010000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 4;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt32(buffer + i, signedNumber);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt32(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt32(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        bdex_ByteStreamImpUtil::getUint32(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                }
            }
        }
      } break;
      case 8: {
        // -----------------------------------------------------------------
        // PUT/GET 40-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt40(char *buf, Int64 val);
        //   getInt40(Int64 *var, const char *buf);
        //   getUint40(Uint64 *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 40-Bit Integers" << endl
                          << "=======================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x00 << 8) + 0x01 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000000 00000000 00000001"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0xff << 8) + 0x80 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 11111111 11111111 10000000"
                             "01000000 00110000 00100000 00010000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 5;
            const int OFFSET = 3;
            const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
            const int NUM_BITS_IN_BUFFER = 8 * SIZE;
            char exp[SIZE + OFFSET];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS_IN_VALUE));


            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt40(buffer + i, signedNumber);
                bool isEq = eq(exp + OFFSET, buffer + i, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt40(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp + OFFSET,
                                         buffer + i,   NUM_BITS_IN_BUFFER));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt40(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            bdex_ByteStreamImpUtil::getUint40(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }
      } break;
      case 7: {
        // -----------------------------------------------------------------
        // PUT/GET 48-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt48(char *buf, Int64 val);
        //   getInt48(Int64 *var, const char *buf);
        //   getUint48(Uint64 *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 48-Bit Integers" << endl
                          << "=======================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x00 << 8) + 0x01 << 8) + 0x02 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000000 00000001 00000010"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0xff << 8) + 0x80 << 8) + 0x70 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 11111111 10000000 01110000"
                             "01000000 00110000 00100000 00010000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 6;
            const int OFFSET = 2;
            const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
            const int NUM_BITS_IN_BUFFER = 8 * SIZE;
            char exp[SIZE + OFFSET];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS_IN_VALUE));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt48(buffer + i, signedNumber);
                bool isEq = eq(exp + OFFSET, buffer + i, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp + OFFSET, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt48(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp + OFFSET,
                                         buffer + i,   NUM_BITS_IN_BUFFER));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt48(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            bdex_ByteStreamImpUtil::getUint48(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }
      } break;
      case 6: {
        // -----------------------------------------------------------------
        // PUT/GET 56-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt56(char *buf, Int64 val);
        //   getInt56(Int64 *var, const char *buf);
        //   getUint56(Uint64 *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 56-Bit Integers" << endl
                          << "=======================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x00 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000000 00000010 00000011 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0xff << 8) + 0x80 << 8) + 0x60 << 8) + 0x50 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "11111111 10000000 01100000 01010000"
                             "01000000 00110000 00100000 00010000";


        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 7;
            const int OFFSET = 1;
            const int NUM_BITS_IN_VALUE = 8 * (SIZE + OFFSET);
            const int NUM_BITS_IN_BUFFER = 8 * SIZE;
            char exp[SIZE + OFFSET];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS_IN_VALUE));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,
                                          buffer + i,   NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt56(buffer + i, signedNumber);
                bool isEq = eq(exp + OFFSET, buffer + i, NUM_BITS_IN_BUFFER);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE + OFFSET) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp + OFFSET,      buffer + i,
                                          NUM_BITS_IN_BUFFER));

                bdex_ByteStreamImpUtil::putInt56(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp + OFFSET,
                                         buffer + i,   NUM_BITS_IN_BUFFER));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                    sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt56(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            bdex_ByteStreamImpUtil::getUint56(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                   }
                }
            }
        }
      } break;
      case 5: {
        // -----------------------------------------------------------------
        // PUT/GET 64-BIT INTEGERS
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified specified value is correctly encoded in a byte
        //   stream at various alignments, irrespective of the endianness of
        //   the platform and decoded to both signed and unsigned variants,
        //   preinitialized to various (positive and negative) values.
        //
        // Testing:
        //   putInt64(char *buf, Int64 val);
        //   getInt64(Int64 *var, const char *buf);
        //   getUint64(Uint64 *var, const char *buf);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Put/Get 64-Bit Integers" << endl
                          << "=======================" << endl;

        typedef bsls_PlatformUtil::Int64 T;
        typedef bsls_PlatformUtil::Uint64 U;

        const T A = (( (( (( ( (T)  // POSITIVE NUMBER
                        0x01 << 8) + 0x02 << 8) + 0x03 << 8) + 0x04 << 8)
                      + 0x05 << 8) + 0x06 << 8) + 0x07 << 8) + 0x08;

        const char *A_SPEC = "00000001 00000010 00000011 00000100"
                             "00000101 00000110 00000111 00001000";

        const T B = (( (( (( ( (T)  // NEGATIVE NUMBER
                        0x80 << 8) + 0x70 << 8) + 0x60 << 8) + 0x50 << 8)
                      + 0x40 << 8) + 0x30 << 8) + 0x20 << 8) + 0x10;

        const char *B_SPEC = "10000000 01110000 01100000 01010000"
                             "01000000 00110000 00100000 00010000";

        static const struct {
            int d_lineNum;                      // line number
            T d_number;                         // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "0..0"                                  },
            { L_,  1,           "0..01"                                 },
            { L_, -1,           "1..1"                                  },
            { L_, -2,           "1..10"                                 },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const T signedNumber = DATA[di].d_number;
            const U unsignedNumber = (U) DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = 8;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt64(buffer + i, signedNumber);
                bool isEq = eq(exp, buffer + i, NUM_BITS);
                if (veryVerbose || !isEq) {
                    const char *e = isLittleEndian()
                                    ? "little endian " : "BIG ENDIAN ";
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, !eq(exp, buffer + i, NUM_BITS));

                bdex_ByteStreamImpUtil::putInt64(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, eq(exp, buffer + i, NUM_BITS));

                static const T INITIAL_VALUES[] = { -99, +99 };
                const int NUM_VALUES =
                   sizeof INITIAL_VALUES/sizeof*INITIAL_VALUES;

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        bdex_ByteStreamImpUtil::getInt64(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        bdex_ByteStreamImpUtil::getUint64(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                   }
                }
            }
        }

      } break;
      case 4: {
        // -----------------------------------------------------------------
        // VERIFY SINGLE-PRECISION FORMAT
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified single-precision number is represented in IEEE
        //   64-bit format.  If the machine is little endian, the bytes
        //   are swapped before they are compared.
        //
        // Testing:
        //   EXPLORE FLOAT FORMAT -- make sure format is IEEE COMPLIANT
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Verifying Single-Precision Format" << endl
                          << "=================================" << endl;

        const double K = 1024;
        const double M = K * K;
        const double B = M * K;

        typedef float T;

        static const struct {
            int d_lineNum;                      // line number
            float d_number;                     // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,  0.0,        "0 0..0"                                },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_, -0.0,        "1 0..0"                                },
#else
            { L_, -0.0,        "0 0..0"                                },
#endif

            // small integers
            { L_,  5.0,        "0 100 0000 1  010 0000 0..0"           },
            { L_,  4.0,        "0 100 0000 1  000 0000 0..0"           },
            { L_,  3.75,       "0 100 0000 0  111 0000 0..0"           },
            { L_,  3.5,        "0 100 0000 0  110 0000 0..0"           },
            { L_,  3.25,       "0 100 0000 0  101 0000 0..0"           },
            { L_,  3.0,        "0 100 0000 0  100 0000 0..0"           },
            { L_,  2.75,       "0 100 0000 0  011 0000 0..0"           },
            { L_,  2.5 ,       "0 100 0000 0  010 0000 0..0"           },
            { L_,  2.0,        "0 100 0000 0  000 0000 0..0"           },
            { L_,  1.5,        "0 011 1111 1  100 0000 0..0"           },
            { L_,  1.0,        "0 011 1111 1  000 0000 0..0"           },

            // base-2 fractions
            { L_,  0.75,       "0 011 1111 0  100 0000 0..0"           },
            { L_,  0.625,      "0 011 1111 0  010 0000 0..0"           },
            { L_,  0.5,        "0 011 1111 0  000 0000 0..0"           },
            { L_,  0.375,      "0 011 1110 1  100 0000 0..0"           },
            { L_,  0.25,       "0 011 1110 1  000 0000 0..0"           },
            { L_,  0.125,      "0 011 1110 0  000 0000 0..0"           },

            // larger integers
            { L_,  1020,       "0 100 0100 0  111 1111 0000 0..0"      },
            { L_,  1021,       "0 100 0100 0  111 1111 0100 0..0"      },
            { L_,  1022,       "0 100 0100 0  111 1111 1000 0..0"      },
            { L_,  1023,       "0 100 0100 0  111 1111 1100 0..0"      },
            { L_,  1024,       "0 100 0100 1  000 0000 0000 0..0"      },
            { L_,  1025,       "0 100 0100 1  000 0000 0010 0..0"      },
            { L_,  1026,       "0 100 0100 1  000 0000 0100 0..0"      },
            { L_,  1027,       "0 100 0100 1  000 0000 0110 0..0"      },

            // really big integers (powers of 2)
            { L_, (T)   K,          "0 100 0100 1  000 0000 0..0"      },
            { L_, (T)   M,          "0 100 1001 1  000 0000 0..0"      },
            { L_, (T)   B,          "0 100 1110 1  000 0000 0..0"      },
            { L_, (T)  (K * B),     "0 101 0011 1  000 0000 0..0"      },
            { L_, (T)  (M * B),     "0 101 1000 1  000 0000 0..0"      },
            { L_, (T)  (B * B),     "0 101 1101 1  000 0000 0..0"      },
            { L_, (T)  (K * B * B), "0 110 0010 1  000 0000 0..0"      },
            { L_, (T)  (M * B * B), "0 110 0111 1  000 0000 0..0"      },
            { L_, (T)  (B * B * B), "0 110 1100 1  000 0000 0..0"      },

            // least significant bits of mantissa
            { L_, (T)  (1*M + 1),   "0 100 1001 1  000 0000 0..0 1000" },
            { L_, (T)  (2*M + 1),   "0 100 1010 0  000 0000 0..0 0100" },
            { L_, (T)  (3*M + 1),   "0 100 1010 0  100 0000 0..0 0100" },
            { L_, (T)  (4*M + 1),   "0 100 1010 1  000 0000 0..0 0010" },
            { L_, (T)  (5*M + 1),   "0 100 1010 1  010 0000 0..0 0010" },
            { L_, (T)  (6*M + 1),   "0 100 1010 1  100 0000 0..0 0010" },
            { L_, (T)  (7*M + 1),   "0 100 1010 1  110 0000 0..0 0010" },
            { L_, (T)  (8*M + 1),   "0 100 1011 0  000 0000 0..0 0001" },
            { L_, (T)  (9*M + 2),   "0 100 1011 0  001 0000 0..0 0010" },
            { L_, (T) (10*M + 5),   "0 100 1011 0  010 0000 0..0 0101" },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const float number = DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = sizeof number;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            union {
                float d_number;
                char d_bytes[1];
            } u;

            u.d_number = number;
            if (isLittleEndian()) {
                reverse(u.d_bytes, SIZE);
            }

            bool isEq = eq(u.d_bytes, exp, NUM_BITS);

            if (veryVerbose || !isEq) {
                const char *e = isLittleEndian()
                                ? "little endian " : "BIG ENDIAN ";
                cout << e; P(number)
                cout << "exp: "; pBytes(exp, SIZE) << endl;
                cout << "act: "; printFloatBits(cout, number) << endl;
                cout << "     " "S^--EXP--|^-------MANTISSA--------|" << endl;
            }
            LOOP_ASSERT(LINE, isEq);

            // To avoid having to explicitly repeat negative numbers,
            // if we negate the number and toggle the bit seven in the LSB
            // the results should be compatible.

            if (isEq) {         // If the original test failed skip this one.
                u.d_number = -number;
                if (isLittleEndian()) {
                    reverse(u.d_bytes, SIZE);
                }
                exp[0] ^= 0x80;
                LOOP_ASSERT(LINE, eq(u.d_bytes, exp, NUM_BITS));
            }
        }
      } break;
      case 3: {
        // -----------------------------------------------------------------
        // VERIFY DOUBLE-PRECISION FORMAT
        //   For each of an enumerated sequence of individual tests, verify
        //   that the specified double-precision number is represented in IEEE
        //   64-bit format.  If the machine is little endian, the bytes
        //   are swapped before they are compared.
        //
        // Testing:
        //   EXPLORE DOUBLE FORMAT -- make sure format is IEEE COMPLIANT
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Verifying Double-Precision Format" << endl
                          << "=================================" << endl;

        const double K = 1024;
        const double M = K * K;
        const double B = M * K;

        static const struct {
            int d_lineNum;                      // line number
            double d_number;                    // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "0 0..0"                                },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "1 0..0"                                },
#else
            { L_,  -0.0,        "0 0..0"                                },
#endif

            // small integers
            { L_,   5.00,       "0 100 0000 0001 0100 0000 0..0"        },
            { L_,   4.00,       "0 100 0000 0001 0000 0000 0..0"        },
            { L_,   3.75,       "0 100 0000 0000 1110 0000 0..0"        },
            { L_,   3.5,        "0 100 0000 0000 1100 0000 0..0"        },
            { L_,   3.25,       "0 100 0000 0000 1010 0000 0..0"        },
            { L_,   3.0 ,       "0 100 0000 0000 1000 0000 0..0"        },
            { L_,   2.75,       "0 100 0000 0000 0110 0000 0..0"        },
            { L_,   2.5,        "0 100 0000 0000 0100 0000 0..0"        },
            { L_,   2.0,        "0 100 0000 0000 0000 0000 0..0"        },
            { L_,   1.5,        "0 011 1111 1111 1000 0000 0..0"        },
            { L_,   1.0,        "0 011 1111 1111 0000 0000 0..0"        },

            // base-2 fractions
            { L_,   0.75,       "0 011 1111 1110 1000 0000 0..0"        },
            { L_,   0.625,      "0 011 1111 1110 0100 0000 0..0"        },
            { L_,   0.5,        "0 011 1111 1110 0000 0000 0..0"        },
            { L_,   0.375,      "0 011 1111 1101 1000 0000 0..0"        },
            { L_,   0.25,       "0 011 1111 1101 0000 0000 0..0"        },
            { L_,   0.125,      "0 011 1111 1100 0000 0000 0..0"        },

            // larger integers
            { L_,   1020,       "0 100 0000 1000  1111 1110 0000 0..0"  },
            { L_,   1021,       "0 100 0000 1000  1111 1110 1000 0..0"  },
            { L_,   1022,       "0 100 0000 1000  1111 1111 0000 0..0"  },
            { L_,   1023,       "0 100 0000 1000  1111 1111 1000 0..0"  },
            { L_,   1024,       "0 100 0000 1001  0000 0000 0000 0..0"  },
            { L_,   1025,       "0 100 0000 1001  0000 0000 0100 0..0"  },
            { L_,   1026,       "0 100 0000 1001  0000 0000 1000 0..0"  },
            { L_,   1027,       "0 100 0000 1001  0000 0000 1100 0..0"  },

            // really big integers (powers of 2)
            { L_,   K,          "0 100 0000 1001  0000 0000 0000 0..0"  },
            { L_,   M,          "0 100 0001 0011  0000 0000 0000 0..0"  },
            { L_,   B,          "0 100 0001 1101  0000 0000 0000 0..0"  },
            { L_,   K * B,      "0 100 0010 0111  0000 0000 0000 0..0"  },
            { L_,   M * B,      "0 100 0011 0001  0000 0000 0000 0..0"  },
            { L_,   B * B,      "0 100 0011 1011  0000 0000 0000 0..0"  },
            { L_,   K * B * B,  "0 100 0100 0101  0000 0000 0000 0..0"  },
            { L_,   M * B * B,  "0 100 0100 1111  0000 0000 0000 0..0"  },
            { L_,   B * B * B,  "0 100 0101 1001  0000 0000 0000 0..0"  },

            // least significant bits of mantissa
            { L_,   1*M*B + 1,  "0 100 0011 0001  0000 0..0 0000 0100"  },
            { L_,   2*M*B + 1,  "0 100 0011 0010  0000 0..0 0000 0010"  },
            { L_,   3*M*B + 1,  "0 100 0011 0010  1000 0..0 0000 0010"  },
            { L_,   4*M*B + 1,  "0 100 0011 0011  0000 0..0 0000 0001"  },
            { L_,   5*M*B + 2,  "0 100 0011 0011  0100 0..0 0000 0010"  },
            { L_,   6*M*B + 3,  "0 100 0011 0011  1000 0..0 0000 0011"  },
            { L_,   7*M*B + 4,  "0 100 0011 0011  1100 0..0 0000 0100"  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_lineNum;
            const double number = DATA[di].d_number;
            const char *spec = DATA[di].d_spec;

            const int SIZE = sizeof number;
            const int NUM_BITS = 8 * SIZE;
            char exp[SIZE];
            LOOP_ASSERT(LINE, 0 == g(exp, spec, NUM_BITS));

            union {
                double d_number;
                char d_bytes[1];
            } u;

            u.d_number = number;
            if (isLittleEndian()) {
                reverse(u.d_bytes, SIZE);
            }

            bool isEq = eq(u.d_bytes, exp, NUM_BITS);

            if (veryVerbose || !isEq) {
                const char *e = isLittleEndian()
                                ? "little endian " : "BIG ENDIAN ";
                cout << e; P(number)
                cout << "exp: "; pBytes(exp, SIZE) << endl;
                cout << "act: "; printDoubleBits(cout, number) << endl;
                cout << "     "
      "S^-EXPONENT-|^----------------------MANTISSA--------------------------|"
                                                                       << endl;
            }
            LOOP_ASSERT(LINE, isEq);

            // To avoid having to explicitly repeat negative numbers,
            // if we negate the number and toggle the bit seven in the LSB
            // the results should be compatible.

            if (isEq) {         // If the original test failed skip this one.
                u.d_number = -number;
                if (isLittleEndian()) {
                    reverse(u.d_bytes, SIZE);
                }
                exp[0] ^= 0x80;
                LOOP_ASSERT(LINE, eq(u.d_bytes, exp, NUM_BITS));
            }
        }
      } break;
      case 2: {
        // -----------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //   For each of an enumerated sequence of individual tests, verify
        //   that the g function loads the correct value according to the
        //   given specification.  Also verify that it correctly detects
        //   invalid syntax.
        //
        // Testing:
        //   GENERATOR FUNCTION: int g(char *buf, const char *spec, int nBits)
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function, 'g'" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nVerify behavior for valid input." << endl;
        {
            const int SIZE = 10;
            static const struct {
                int d_lineNum;                 // line number
                int d_numBits;                 // number of bits in buffer
                const char *d_spec;            // input spec.
                // NOTE: Microsoft fails if next item is const
                unsigned char d_exp[SIZE];     // expected byte values
            } DATA[] = {
    //---------^
    //L#  #b  Input Specification               Expected Byte Values
    //--  --  -------------------               --------------------

    // testing leading bits in a single byte
    { L_,  0, "",                               { 0x00,          } },
    { L_,  1, "",                               { 0x00,          } },
    { L_,  8, "",                               { 0x00,          } },
    { L_, 10, "",                               { 0x00,          } },

    { L_,  1, "1",                              { 0x80,          } },
    { L_,  8, "1",                              { 0x80,          } },
    { L_,  9, "1",                              { 0x80,          } },

    { L_,  2, "10",                             { 0x80,          } },
    { L_,  8, "10",                             { 0x80,          } },
    { L_,  9, "10",                             { 0x80,          } },

    { L_,  2, "01",                             { 0x40,          } },
    { L_,  8, "01",                             { 0x40,          } },
    { L_,  9, "01",                             { 0x40,          } },

    { L_,  3, "101",                            { 0xA0,          } },
    { L_,  9, "101",                            { 0xA0,          } },
    { L_, 10, "101",                            { 0xA0,          } },

    { L_,  4, "0111",                           { 0x70,          } },
    { L_,  9, "0111",                           { 0x70,          } },
    { L_, 10, "0111",                           { 0x70,          } },

    { L_,  4, "1011",                           { 0xB0,          } },
    { L_,  9, "1011",                           { 0xB0,          } },
    { L_, 10, "1011",                           { 0xB0,          } },

    // testing leading byte boundary
    { L_, 10, "0110 1001",                      { 0x69,          } },
    { L_, 10, "1001 0110",                      { 0x96,          } },

    { L_, 10, "0101 1010  1",                   { 0x5A, 0x80,    } },
    { L_, 10, "1010 0101  1",                   { 0xA5, 0x80,    } },
    { L_, 10, "0101 1010  0",                   { 0x5A, 0x00,    } },
    { L_, 10, "1010 0101  0",                   { 0xA5, 0x00,    } },

    { L_, 10, "0101 1010  10",                  { 0x5A, 0x80,    } },
    { L_, 10, "1010 0101  10",                  { 0xA5, 0x80,    } },
    { L_, 10, "0101 1010  01",                  { 0x5A, 0x40,    } },
    { L_, 10, "1010 0101  01",                  { 0xA5, 0x40,    } },

    // testing trailing bits in single byte
    { L_, 0, "0..0",                            { 0x00, 0x00,    } }, // N = 0
    { L_, 0, "1..1",                            { 0x00, 0x00,    } },


    { L_, 1, "0..0",                            { 0x00, 0x00,    } }, // N = 1
    { L_, 1, "1..1",                            { 0x80, 0x00,    } },

    { L_, 1, "0..00",                           { 0x00, 0x00,    } },
    { L_, 1, "0..01",                           { 0x80, 0x00,    } },
    { L_, 1, "1..10",                           { 0x00, 0x00,    } },
    { L_, 1, "1..11",                           { 0x80, 0x00,    } },


    { L_, 2, "0..0",                            { 0x00, 0x00,    } }, // N = 2
    { L_, 2, "1..1",                            { 0xC0, 0x00,    } },

    { L_, 2, "0..00",                           { 0x00, 0x00,    } },
    { L_, 2, "1..10",                           { 0x80, 0x00,    } },
    { L_, 2, "0..01",                           { 0x40, 0x00,    } },
    { L_, 2, "1..11",                           { 0xc0, 0x00,    } },

    { L_, 2, "0..000",                          { 0x00, 0x00,    } },
    { L_, 2, "1..100",                          { 0x00, 0x00,    } },
    { L_, 2, "0..010",                          { 0x80, 0x00,    } },
    { L_, 2, "1..110",                          { 0x80, 0x00,    } },
    { L_, 2, "0..001",                          { 0x40, 0x00,    } },
    { L_, 2, "1..101",                          { 0x40, 0x00,    } },
    { L_, 2, "0..011",                          { 0xC0, 0x00,    } },
    { L_, 2, "1..111",                          { 0xC0, 0x00,    } },


    { L_, 3, "0..0",                            { 0x00, 0x00,    } }, // N = 3
    { L_, 3, "1..1",                            { 0xE0, 0x00,    } },

    { L_, 3, "0..00",                           { 0x00, 0x00,    } },
    { L_, 3, "1..10",                           { 0xC0, 0x00,    } },
    { L_, 3, "0..01",                           { 0x20, 0x00,    } },
    { L_, 3, "1..11",                           { 0xE0, 0x00,    } },

    { L_, 3, "0..000",                          { 0x00, 0x00,    } },
    { L_, 3, "1..100",                          { 0x80, 0x00,    } },
    { L_, 3, "0..010",                          { 0x40, 0x00,    } },
    { L_, 3, "1..110",                          { 0xc0, 0x00,    } },
    { L_, 3, "0..001",                          { 0x20, 0x00,    } },
    { L_, 3, "1..101",                          { 0xA0, 0x00,    } },
    { L_, 3, "0..011",                          { 0x60, 0x00,    } },
    { L_, 3, "1..111",                          { 0xE0, 0x00,    } },

    // testing trailing byte boundary (aligned)
    { L_, 16, "0..0 0110 1001",                 { 0x00, 0x69,    } }, // B = 8
    { L_, 16, "0..0 1001 0110",                 { 0x00, 0x96,    } },

    { L_, 16, "1..1 0110 1001",                 { 0xff, 0x69,    } },
    { L_, 16, "1..1 1001 0110",                 { 0xff, 0x96,    } },


    { L_, 16, "0..0 0 0101 1010",               { 0x00, 0x5a,    } }, // B = 9
    { L_, 16, "0..0 0 1010 0101",               { 0x00, 0xa5,    } },
    { L_, 16, "0..0 1 0101 1010",               { 0x01, 0x5a,    } },
    { L_, 16, "0..0 1 1010 0101",               { 0x01, 0xa5,    } },

    { L_, 16, "1..1 0 0101 1010",               { 0xfe, 0x5a,    } },
    { L_, 16, "1..1 0 1010 0101",               { 0xfe, 0xa5,    } },
    { L_, 16, "1..1 1 0101 1010",               { 0xff, 0x5a,    } },
    { L_, 16, "1..1 1 1010 0101",               { 0xff, 0xa5,    } },


    { L_, 16, "0..0 00 0101 1001",              { 0x00, 0x59,    } }, // B = 10
    { L_, 16, "0..0 01 0101 1001",              { 0x01, 0x59,    } },

    { L_, 16, "0..0 10 0101 1001",              { 0x02, 0x59,    } },
    { L_, 16, "0..0 11 0101 1001",              { 0x03, 0x59,    } },

    { L_, 16, "1..1 00 0101 1001",              { 0xfc, 0x59,    } },
    { L_, 16, "1..1 01 0101 1001",              { 0xfd, 0x59,    } },

    { L_, 16, "1..1 10 0101 1001",              { 0xfe, 0x59,    } },
    { L_, 16, "1..1 11 0101 1001",              { 0xff, 0x59,    } },

    // testing trailing byte boundary (unaligned)
    { L_, 16, "          0..0 1000 0111",       { 0x00, 0x87,    } },
    { L_, 15, "         0..01 0000 111",        { 0x01, 0x0e,    } },
    { L_, 14, "        0..010 0001 11",         { 0x02, 0x1c,    } },
    { L_, 13, "       0..0100 0011 1",          { 0x04, 0x38,    } },

    { L_, 12, "     0..0 1000 0111",            { 0x08, 0x70,    } },
    { L_, 11, "    0..01 0000 111",             { 0x10, 0xe0,    } },
    { L_, 10, "   0..010 0001 11",              { 0x21, 0xc0,    } },
    { L_,  9, "  0..0100 0011 1",               { 0x43, 0x80,    } },

    { L_,  8, "0..0 1000 0111",                 { 0x87, 0x00,    } },

    { L_, 16, "          1..1 1110 0001",       { 0xff, 0xe1,    } },
    { L_, 15, "         1..11 1100 001",        { 0xff, 0xc2,    } },
    { L_, 14, "        1..111 1000 01",         { 0xff, 0x84,    } },
    { L_, 13, "       1..1111 0000 1",          { 0xff, 0x08,    } },

    { L_, 12, "     1..1 1110 0001",            { 0xfe, 0x10,    } },
    { L_, 11, "    1..11 1100 001",             { 0xfc, 0x20,    } },
    { L_, 10, "   1..111 1000 01",              { 0xf8, 0x40,    } },
    { L_,  9, "  1..1111 0000 1",               { 0xf0, 0x80,    } },

    { L_,  8, "1..1 1110 0001",                 { 0xe1, 0x00,    } },

    // testing leading and trailing values
    { L_, 16, "00..00",                         { 0x00, 0x00,    } },
    { L_, 16, "00..01",                         { 0x00, 0x01,    } },
    { L_, 16, "10..00",                         { 0x80, 0x00,    } },
    { L_, 16, "10..01",                         { 0x80, 0x01,    } },

    { L_, 16, "01..10",                         { 0x7f, 0xfe,    } },
    { L_, 16, "01..11",                         { 0x7f, 0xff,    } },
    { L_, 16, "11..10",                         { 0xff, 0xfe,    } },
    { L_, 16, "11..11",                         { 0xff, 0xff,    } },

    { L_, 16, "110..0 1101",                    { 0xc0, 0x0d,    } },
    { L_, 16, "111..1 1101",                    { 0xff, 0xfd,    } },
    { L_, 16, "000..0 0010",                    { 0x00, 0x02,    } },
    { L_, 16, "001..1 0010",                    { 0x3f, 0xf2,    } },

    { L_, 15, "110..01 101",                    { 0xc0, 0x1a,    } },
    { L_, 15, "111..11 101",                    { 0xff, 0xfa,    } },
    { L_, 15, "000..00 010",                    { 0x00, 0x04,    } },
    { L_, 15, "001..10 010",                    { 0x3f, 0xe4,    } },

    // testing other whitespace characters (e.g., tab);
    { L_, 15, "0\t01 . . 10 0\t10",             { 0x3f, 0xe4,    } },
    //---------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int numBits = DATA[di].d_numBits;
                const char *origSpec = DATA[di].d_spec;
                const char *exp = (char *) DATA[di].d_exp;

                char spec[1000];    // big buffer to hold "perturbed specs
                const int N = 3;    // perturb specs N different ways
                int status = 0;     // don't modify spec if original failed.

                for (int k = 0; k < N && 0 == status; ++k) {
                    switch (k) {
                      case/**/ 0: {
                        //---------------------------------------
                        // Copy original Spec as is.
                        //---------------------------------------
                        strcpy(spec, origSpec);
                      } break;
                      case/**/ 1: {
                        //----------------------------------------
                        // Remove all space (' ') characters.
                        //----------------------------------------
                        char *q = spec;
                        for (const char *p = origSpec; *p; ++p) {
                            if (' ' == *p) {
                                continue;
                            }
                            *q++ = *p;
                        }
                        *q = '\0';
                      } break;
                      case/**/ 2: {
                        //-------------------------------------------------
                        // Insert varying #'s of space (' ') between chars.
                        //-------------------------------------------------
                        static int count = 1;
                        char *q = spec;
                        for (const char *p = origSpec; *p; ++p) {
                            count = 1 + ++count % 3;
                            for (int i = 0; i < count; ++i) {
                                *q++ = ' ';
                            }
                            *q++ = *p;
                        }
                        *q = '\0';
                      } break;
                      default/**/: {
                        LOOP2_ASSERT(LINE, k, !"No Spec Perturbation");
                      }
                    }

                    char buf[SIZE];
                    if (veryVerbose) {
                        cout << "=========" << flush; P_(LINE)
                        Q(=================================)
                        P_(k) P(spec);
                    }
                    resetBits(buf, 8*SIZE, 0);
                    LOOP2_ASSERT(LINE, k, 0 == g(buf, spec, numBits));
                    if (veryVerbose) {
                        cout << "buf: "; pBytes(buf , 3) << endl;
                        cout << "exp: "; pBytes(exp,3) << endl;
                    }

                    bool isEq = eq(buf, exp, 8*SIZE);
                    LOOP2_ASSERT(LINE, k, "First try!" && isEq);
                    if (0 == k && !isEq) {
                        status = !0;
                        continue; // don't keep going if initial spec fails.
                    }

                    resetBits(buf, 8*SIZE, 1);
                    LOOP2_ASSERT(LINE, k, 0 == g(buf, spec, numBits));
                    if (veryVerbose) {
                        cout << "buf: "; pBytes(buf, 3) << endl;
                    }

                    LOOP2_ASSERT(LINE, k, eq(buf, exp, numBits));

                    // make sure any trailing bits are all 1's
                    int lastByte = numBits / 8;
                    if (lastByte < SIZE) {
                        int numLeft = 8 - numBits % 8;
                        int mask = (1 << numLeft) - 1;
                        int bb = buf[lastByte] & 0xff;
                        int diff = ((bb & mask) ^ mask) & 0xff;

                        if (veryVerbose) {
                            P_(numBits)
                            P_(numLeft)
                            P_(lastByte)
                            P_(bb)
                            P_(mask)
                            P(diff)
                         }

                        LOOP_ASSERT(LINE, 0 == diff);
                        ++lastByte;
                        for (; lastByte < 10; ++lastByte) {
                            int d = 0xff ^ buf[lastByte] & 0xff;
                            LOOP3_ASSERT(LINE, k, lastByte, 0 == d);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nVerify invalid input is detected." << endl;
        {
            static const struct {
                int d_lineNum;                  // line number
                int d_numBits;                  // number of bits in buffer
                const char *d_spec;             // input spec.
                int d_exp;                      // expected return code
            } DATA[] = {
                //L#  #b  Input Specification           Resulting Error Code
                //--  --  ---------------------------   --------------------
                { L_,  0, "A",                          G_ILLEGAL_CHARACTER  },
                { L_,  0, "2",                          G_ILLEGAL_CHARACTER  },
                { L_,  0, ":",                          G_ILLEGAL_CHARACTER  },
                { L_,  0, "z0",                         G_ILLEGAL_CHARACTER  },
                { L_,  0, "09",                         G_ILLEGAL_CHARACTER  },
                { L_,  0, "0_0",                        G_ILLEGAL_CHARACTER  },

                { L_,  0, "0..1",                       G_MISMATCHED_RANGE   },
                { L_,  0, "0..10",                      G_MISMATCHED_RANGE   },
                { L_,  0, "0..11",                      G_MISMATCHED_RANGE   },
                { L_,  0, "00..1",                      G_MISMATCHED_RANGE   },
                { L_,  0, "10..1",                      G_MISMATCHED_RANGE   },

                { L_,  0, "1..0",                       G_MISMATCHED_RANGE   },
                { L_,  0, "1..00",                      G_MISMATCHED_RANGE   },
                { L_,  0, "1..01",                      G_MISMATCHED_RANGE   },
                { L_,  0, "01..0",                      G_MISMATCHED_RANGE   },
                { L_,  0, "11..0",                      G_MISMATCHED_RANGE   },

                { L_,  0, "0..",                        G_MISSING_RANGE_END  },
                { L_,  0, "00..",                       G_MISSING_RANGE_END  },
                { L_,  0, "10..",                       G_MISSING_RANGE_END  },

                { L_,  0, "1..",                        G_MISSING_RANGE_END  },
                { L_,  0, "01..",                       G_MISSING_RANGE_END  },
                { L_,  0, "11..",                       G_MISSING_RANGE_END  },

                { L_,  0, "..0",                        G_MISSING_RANGE_START},
                { L_,  0, "..00",                       G_MISSING_RANGE_START},
                { L_,  0, "..01",                       G_MISSING_RANGE_START},

                { L_,  0, "..1",                        G_MISSING_RANGE_START},
                { L_,  0, "..10",                       G_MISSING_RANGE_START},
                { L_,  0, "..11",                       G_MISSING_RANGE_START},

                { L_,  0, "0.1",                        G_MISSING_SECOND_DOT },
                { L_,  0, "0.10",                       G_MISSING_SECOND_DOT },
                { L_,  0, "0.11",                       G_MISSING_SECOND_DOT },
                { L_,  0, "00.1",                       G_MISSING_SECOND_DOT },
                { L_,  0, "10.1",                       G_MISSING_SECOND_DOT },

                { L_,  0, "1.0",                        G_MISSING_SECOND_DOT },
                { L_,  0, "1.00",                       G_MISSING_SECOND_DOT },
                { L_,  0, "1.01",                       G_MISSING_SECOND_DOT },
                { L_,  0, "01.0",                       G_MISSING_SECOND_DOT },
                { L_,  0, "11.0",                       G_MISSING_SECOND_DOT },

                { L_,  0, "0..0.",                      G_MULTIPLE_RANGES    },
                { L_,  0, "0..00.",                     G_MULTIPLE_RANGES    },
                { L_,  0, "0..01.",                     G_MULTIPLE_RANGES    },

                { L_,  0, "1..1.",                      G_MULTIPLE_RANGES    },
                { L_,  0, "1..10.",                     G_MULTIPLE_RANGES    },
                { L_,  0, "1..11.",                     G_MULTIPLE_RANGES    },

                { L_,  0, "",                           0                    },
                { L_,  0, "1..1",                       0                    },
                { L_,  0, "0..0",                       0                    },
                { L_,  0, "1",                          G_TOO_MANY_BITS      },
                { L_,  0, "0",                          G_TOO_MANY_BITS      },
                { L_,  0, "01..1",                      G_TOO_MANY_BITS      },
                { L_,  0, "00..0",                      G_TOO_MANY_BITS      },
                { L_,  0, "11..1",                      G_TOO_MANY_BITS      },
                { L_,  0, "10..0",                      G_TOO_MANY_BITS      },
                { L_,  0, "1..10",                      G_TOO_MANY_BITS      },
                { L_,  0, "0..00",                      G_TOO_MANY_BITS      },
                { L_,  0, "1..11",                      G_TOO_MANY_BITS      },
                { L_,  0, "0..01",                      G_TOO_MANY_BITS      },
                { L_,  0, "0..01",                      G_TOO_MANY_BITS      },

                { L_,  1, "0",                          0                    },
                { L_,  1, "1",                          0                    },
                { L_,  1, "01..1",                      0                    },
                { L_,  1, "0..01",                      0                    },
                { L_,  1, "10",                         G_TOO_MANY_BITS      },
                { L_,  1, "001..1",                     G_TOO_MANY_BITS      },
                { L_,  1, "01..10",                     G_TOO_MANY_BITS      },
                { L_,  1, "1..100",                     G_TOO_MANY_BITS      },

                { L_,  8, "1111 1111 1..1",             0                    },
                { L_,  7, "1111 1111 1..1",             G_TOO_MANY_BITS      },
                { L_,  7, "0..0 1111 1111",             G_TOO_MANY_BITS      },

                { L_,  9, "0 1111 1111 1..1",           0                    },
                { L_,  8, "0 1111 1111 1..1",           G_TOO_MANY_BITS      },
                { L_,  8, "0..0 1111 1111 0",           G_TOO_MANY_BITS      },
                { L_,  8, "1111 1..1 1111 0",           G_TOO_MANY_BITS      },

                { L_, 10, "10 1111 1111 0..0",          0                    },
                { L_,  9, "10 1111 1111 0..0",          G_TOO_MANY_BITS      },
                { L_,  9, "0..0 1111 1111 01",          G_TOO_MANY_BITS      },
                { L_,  9, "10 1111 0..0 1111",          G_TOO_MANY_BITS      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 10;
            char control_0[SIZE];       memset(control_0, '\x00', SIZE);
            char control_1[SIZE];       memset(control_1, '\xff', SIZE);

            G_OFF = 1;  // set to 1 only to enable testing of G function errors

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int numBits = DATA[di].d_numBits;
                const char *spec = DATA[di].d_spec;
                int exp = DATA[di].d_exp;

                char result_0[SIZE];    // not touched on error;
                char result_1[SIZE];    // not touched on error;

                memcpy(result_0, control_0, SIZE);
                memcpy(result_1, control_1, SIZE);

                // Make sure result has no effect on outcome.
                int res   = g(result_0, spec, numBits);
                int res_1 = g(result_1, spec, numBits);
                LOOP_ASSERT(di, res == res_1);

                if (res) { // If failure, result is not affected.
                    LOOP_ASSERT(LINE, 0 == memcmp(result_0, control_0, SIZE));
                    LOOP_ASSERT(LINE, 0 == memcmp(result_1, control_1, SIZE));
                }
                else { // on success, the two results are the same.
                    LOOP_ASSERT(LINE, 1 == eq(result_0, result_1, numBits));
                }

                if (veryVerbose) {
                    P_(LINE); P_(numBits) P_(spec); P_(res); P(exp); }

                // The following test is primary: make sure that the code is
                // failing for the right reason (i.e., why we think it should).

                LOOP_ASSERT(LINE, res == exp);

            }

            G_OFF = 0;  // set to 1 only to enable testing of G function errors
        }
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // VERIFY TEST APPARATUS
        //   Before we get started, let's make sure that the basic supporting
        //   test functions work as expected.
        // Testing:
        //   SWAP FUNCTION: static inline void swap(T *x, T *y)
        //   REVERSE FUNCTION: void reverse(T *array, int numElements)
        //   SET FUNCTION: void setBits(char *byte, int mask, int boolValue)
        //   RESET FUNCTION: resetBits(char *buffer, int nBits, int boolValue)
        //   EQ FUNCTION: int eq(const char *lhs, const char *rhs, int nBits)
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "VERIFY TESTING APPARATUS" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting swap(T*, T*)" << endl;
        {
            {
                const char A = 1, B = 2;        ASSERT(1 == A); ASSERT(2 == B);
                char a = A, b = B;              ASSERT(A == a); ASSERT(B == b);
                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
            {
                const int A = 1000, B = 2000;   ASSERT(A == 1000);
                int a = A, b = B;               ASSERT(A == a); ASSERT(B == b);
                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
            {
                double A = 1e-10, B = 2e-10;    ASSERT(B == 2e-10);
                double a = A, b = B;            ASSERT(A == a); ASSERT(B == b);
                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
        }

        if (verbose) cout << "\nTesting reverse(T*, numElements)" << endl;
        {
            {
                int a[] = { 1 };                int S = sizeof a/sizeof *a;
                ASSERT(1 == a[0]);
                reverse(a, S);
                ASSERT(1 == a[0]);
                reverse(a, S);
                ASSERT(1 == a[0]);
            }
            {
                char a[] = { 1, 2 };            int S = sizeof a/sizeof *a;
                ASSERT(1 == a[0]); ASSERT(2 == a[1]);
                reverse(a, S);
                ASSERT(2 == a[0]); ASSERT(1 == a[1]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1])
            }
            {
                float a[] = { 1, 2, 3 };        int S = sizeof a/sizeof *a;
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                reverse(a, S);
                ASSERT(3 == a[0]); ASSERT(2 == a[1]); ASSERT(1 == a[2]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
            }
            {
                double a[] = { 1, 2, 3, 4, 5, 6 }; int S = sizeof a/sizeof *a;
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                ASSERT(4 == a[3]); ASSERT(5 == a[4]); ASSERT(6 == a[5]);
                reverse(a, S);
                ASSERT(6 == a[0]); ASSERT(5 == a[1]); ASSERT(4 == a[2]);
                ASSERT(3 == a[3]); ASSERT(2 == a[4]); ASSERT(1 == a[5]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                ASSERT(4 == a[3]); ASSERT(5 == a[4]); ASSERT(6 == a[5]);
            }

        }

        if (verbose) cout << "\nTesting setBits (single byte)" << endl;
        {
            unsigned char byte = 0x0;

            setBits((char *)&byte, 0x00, 0);            ASSERT(0x00 == byte);
            setBits((char *)&byte, 0x00, 1);            ASSERT(0x00 == byte);

            setBits((char *)&byte, 0x01, 1);            ASSERT(0x01 == byte);
            setBits((char *)&byte, 0x02, 1);            ASSERT(0x03 == byte);
            setBits((char *)&byte, 0x01, 0);            ASSERT(0x02 == byte);
            setBits((char *)&byte, 0x0C, 1);            ASSERT(0x0E == byte);
            setBits((char *)&byte, 0x04, 0);            ASSERT(0x0A == byte);

            setBits((char *)&byte, 0x00, 0);            ASSERT(0x0A == byte);
            setBits((char *)&byte, 0x00, 1);            ASSERT(0x0A == byte);

            setBits((char *)&byte, 0x10, 1);            ASSERT(0x1A == byte);
            setBits((char *)&byte, 0x20, 1);            ASSERT(0x3A == byte);
            setBits((char *)&byte, 0x10, 0);            ASSERT(0x2A == byte);
            setBits((char *)&byte, 0xC0, 1);            ASSERT(0xEA == byte);
            setBits((char *)&byte, 0x40, 0);            ASSERT(0xAA == byte);

            setBits((char *)&byte, 0x00, 0);            ASSERT(0xAA == byte);
            setBits((char *)&byte, 0x00, 1);            ASSERT(0xAA == byte);
        }

        if (verbose) cout << "\nTesting resetBits (byte array)" << endl;
        {
            static unsigned char b[10];
            char *p = (char *)b;

            if (verbose) cout << "\tTesting first of two bytes" << endl;

            resetBits(p, 0, 1);  ASSERT(0x00 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 1, 1);  ASSERT(0x80 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 2, 1);  ASSERT(0xc0 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 1, 0);  ASSERT(0x40 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 3, 1);  ASSERT(0xe0 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 2, 0);  ASSERT(0x20 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 8, 1);  ASSERT(0xff == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 1, 0);  ASSERT(0x7f == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 4, 0);  ASSERT(0x0f == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 7, 0);  ASSERT(0x01 == b[0]); ASSERT(0x00 == b[1]);
            resetBits(p, 8, 0);  ASSERT(0x00 == b[0]); ASSERT(0x00 == b[1]);

            if (verbose) cout << "\tTesting second of two bytes" << endl;

            resetBits(p, 10, 1); ASSERT(0xff == b[0]); ASSERT(0xc0 == b[1]);
            resetBits(p,  9, 0); ASSERT(0x00 == b[0]); ASSERT(0x40 == b[1]);
            resetBits(p, 16, 1); ASSERT(0xff == b[0]); ASSERT(0xff == b[1]);
            resetBits(p, 15, 0); ASSERT(0x00 == b[0]); ASSERT(0x01 == b[1]);
            resetBits(p, 13, 1); ASSERT(0xff == b[0]); ASSERT(0xf9 == b[1]);
            resetBits(p, 11, 0); ASSERT(0x00 == b[0]); ASSERT(0x19 == b[1]);
            resetBits(p, 10, 1); ASSERT(0xff == b[0]); ASSERT(0xd9 == b[1]);
            resetBits(p,  9, 0); ASSERT(0x00 == b[0]); ASSERT(0x59 == b[1]);
            resetBits(p,  9, 1); ASSERT(0xff == b[0]); ASSERT(0xd9 == b[1]);
            resetBits(p,  8, 0); ASSERT(0x00 == b[0]); ASSERT(0xd9 == b[1]);
            resetBits(p,  8, 1); ASSERT(0xff == b[0]); ASSERT(0xd9 == b[1]);
        }

        if (verbose) cout << "\nTesting eq(lhsBytes, rhsBytes, nBits)" << endl;
        {
            const int SIZE = 4;
            static const struct {
                // NOTE: Microsoft fails if any members are const
                int d_lineNum;                  // line number
                unsigned char d_lhs[SIZE];      // first array
                unsigned char d_rhs[SIZE];      // second array
                int d_nBits;                    // number of leading bits.
            } DATA[] = {
                // nBits denotes the maximum number of leading equal bits.
                //Line  ________LHS______     ______RHS_______   nBits

                // verify boundary
                { L_, { 0x00, 0x00, 0x00 }, { 0xff, 0xff, 0xff },  0 },
                { L_, { 0x00, 0x00, 0x00 }, { 0x80, 0x00, 0x00 },  0 },
                { L_, { 0xff, 0xff, 0xff }, { 0x7f, 0xff, 0xff },  0 },

                // verify msb vs lsb
                { L_, { 0x80, 0x00, 0x00 }, { 0xff, 0xff, 0xff },  1 },
                { L_, { 0x00, 0x00, 0x00 }, { 0x7f, 0xff, 0xff },  1 },
                { L_, { 0xc0, 0x00, 0x00 }, { 0xef, 0xff, 0xff },  2 },
                { L_, { 0x80, 0x00, 0x00 }, { 0xbf, 0xff, 0xff },  2 },

                // test across first byte and byte boundary (default 0)
                { L_, { 0x80, 0x00, 0x00 }, { 0xc0, 0x00, 0x00 },  1 },
                { L_, { 0xc0, 0x00, 0x00 }, { 0xe0, 0x00, 0x00 },  2 },
                { L_, { 0xe0, 0x00, 0x00 }, { 0xf0, 0x00, 0x00 },  3 },
                { L_, { 0xf0, 0x00, 0x00 }, { 0xf8, 0x00, 0x00 },  4 },
                { L_, { 0xf8, 0x00, 0x00 }, { 0xfc, 0x00, 0x00 },  5 },
                { L_, { 0xfc, 0x00, 0x00 }, { 0xfe, 0x00, 0x00 },  6 },
                { L_, { 0xfe, 0x00, 0x00 }, { 0xff, 0x00, 0x00 },  7 },
                { L_, { 0xff, 0x00, 0x00 }, { 0xff, 0x80, 0x00 },  8 },
                { L_, { 0xff, 0x80, 0x00 }, { 0xff, 0xc0, 0x00 },  9 },
                { L_, { 0xff, 0xc0, 0x00 }, { 0xff, 0xe0, 0x00 }, 10 },
                { L_, { 0xff, 0xe0, 0x00 }, { 0xff, 0xf0, 0x00 }, 11 },

                // test across first byte and byte boundary (default 1)
                { L_, { 0xbf, 0xff, 0xff }, { 0xff, 0xff, 0xff },  1 },
                { L_, { 0xdf, 0xff, 0xff }, { 0xff, 0xff, 0xff },  2 },
                { L_, { 0xef, 0xff, 0xff }, { 0xff, 0xff, 0xff },  3 },
                { L_, { 0xf7, 0xff, 0xff }, { 0xff, 0xff, 0xff },  4 },
                { L_, { 0xfb, 0xff, 0xff }, { 0xff, 0xff, 0xff },  5 },
                { L_, { 0xfd, 0xff, 0xff }, { 0xff, 0xff, 0xff },  6 },
                { L_, { 0xfe, 0xff, 0xff }, { 0xff, 0xff, 0xff },  7 },
                { L_, { 0xff, 0x7f, 0xff }, { 0xff, 0xff, 0xff },  8 },
                { L_, { 0xff, 0xbf, 0xff }, { 0xff, 0xff, 0xff },  9 },
                { L_, { 0xff, 0xdf, 0xff }, { 0xff, 0xff, 0xff }, 10 },
                { L_, { 0xff, 0xef, 0xff }, { 0xff, 0xff, 0xff }, 11 },

                // some "random" test cases
                { L_, { 0x69, 0x96, 0x81 }, { 0x69, 0x96, 0x80 }, 23 },
                { L_, { 0x69, 0x95, 0x81 }, { 0x69, 0x96, 0x81 }, 15 },
                { L_, { 0x68, 0x96, 0x81 }, { 0x69, 0x96, 0x81 },  7 },

                { L_, { 0x80, 0x18, 0x24 }, { 0x81, 0x18, 0x24 },  7 },
                { L_, { 0x89, 0x18, 0x24 }, { 0x81, 0x18, 0x24 },  4 },
                { L_, { 0x81, 0x08, 0x24 }, { 0x81, 0x18, 0x24 }, 11 },
                { L_, { 0x81, 0x98, 0x24 }, { 0x81, 0x18, 0x24 },  8 },


            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const char *lhs = (char *) DATA[di].d_lhs;
                const char *rhs = (char *) DATA[di].d_rhs;
                const int nBits = DATA[di].d_nBits;

                if (veryVerbose) {
                    cout << "=====> "; P_(LINE); P_(nBits); cout << "\t";
                    Q(----------------------------------);
                    cout << "\t\t\t\tlhs: "; pBytes(lhs, SIZE) <<  endl;
                    cout << "\t\t\t\trhs: "; pBytes(rhs, SIZE) << endl;
                }

                int i;
                for (i = 0; i < nBits; ++i) {
                    LOOP2_ASSERT(LINE, i, 1 == eq(lhs, rhs, i));
                }
                for (++i; i < 8 * SIZE; ++i) {
                    LOOP2_ASSERT(LINE, i, 0 == eq(lhs, rhs, i));
                }
            }
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
