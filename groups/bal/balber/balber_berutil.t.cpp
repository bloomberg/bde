// balber_berutil.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berutil.h>

#include <balber_berconstants.h>

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_random.h>

#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bslim_testutil.h>

#include <bdldfp_decimalutil.h>

#include <bdlsb_memoutstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>
#include <bdlt_datetz.h>
#include <bdlt_datetimetz.h>
#include <bdlt_prolepticdateimputil.h>
#include <bdlt_timetz.h>

#include <bdlb_float.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsla_maybeunused.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_numeric.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a utility 'struct' used to encode and
// decode objects of 'bdlat' 'Simple' type according to the specification of
// the BER ("Basic Encoding Rules") format defined by ITU-T X.690.  The
// 'Simple' types include the C++ fundamental types, BDE date and time types,
// and strings, etc.  The utility also provides ancillary operations associated
// with the encoding and decoding of these types in BER, such operating on
// encoded BER meta-information, etc.  This component also provides
// non-standard extensions to the BER specification, such as an optional
// alternate, compact format for representing date and time values.
//
// The primary operations provided by this component are
// 'balber::BerUtil::putValue', and 'balber::BerUtil::getValue', which are
// responsible for the encoding and decoding objects of 'Simple' types,
// respectively.
//
// This test driver checks "behavioral fingerprints" of 'putValue', and
// 'getValue', which are checksums of the output for a large, pseudo-randomly
// generated set of inputs for said functions.  In order to generate the input,
// this test driver provides machinery for pseudo-randomly creating values of
// types used in the input space of the functions.  Further, it provides an
// implementation of the MD5 Message Digest Algorithm, which is known to be a
// high-quality checksum algorithm with a specification.  The checksum
// algorithm is high-quality in that it can be formally shown to have an
// astronomically low probability of hash collision between input bit sequences
// with low edit distance.  Basically, in MD5, every bit matters and
// nearly-identical inputs have different hashes with near certainty.
//
// Global Concerns:
//: o Except for non-standard extensions, the encoding and decoding of objects
//:   provided by this component comply with the Basic Encoding Rules of the
//:   ITU-T X.690 specification, according to the ASN.1 interpretation of the
//:   values of said objects.
//: o Ancillary functions provided by this component that calculate values
//:   specified by ITU-T X.690 comply with the specification.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [16] int getEndOfContentsOctets(bsl::streambuf *, int *nBytes);
// [17] int getIdentifierOctets(streambuf *, cls *, ty *, tag *, int *nBytes);
// [12] int getLength(bsl::streambuf *, int *result, int *nBytes);
// [23] int getValue(bsl::streambuf *, TYPE *, int length, const Options&);
// [23] int getValue(bsl::streambuf *, TYPE *, int *nBytes, const Options&);
// [16] int putEndOfContentOctets(bsl::streambuf *);
// [17] int putIdentifierOctets(bsl::streambuf *, cls *, ty *, int tag);
// [16] int putIndefiniteLengthOctet(bsl::streambuf *);
// [12] int putLength(bsl::streambuf *, int length);
// [22] int putValue(bsl::streambuf *, const TYPE& value, const Options * = 0);
// ----------------------------------------------------------------------------
// [-1] PERFORMANCE TEST
// [ 1] BREATHING TEST
// [ 2] CONCERN: 'putValue' & 'getValue' for bool values
// [ 3] CONCERN: 'putValue' & 'getValue' for signed char values
// [ 4] CONCERN: 'putValue' & 'getValue' for unsigned char values
// [ 5] CONCERN: 'numBytesToStream' (component-private)
// [ 6] CONCERN: 'numBytesToStream' (component-private) for unsigned types
// [ 7] CONCERN: 'putIntegerGivenLength' (component-private)
// [ 8] CONCERN: 'get/putIntegetGivenLength' for unsigned values
// [ 9] CONCERN: 'putValue' & 'getValue' for signed integral values
// [10] CONCERN: 'putValue' & 'getValue' for unsigned integral types
// [11] CONCERN: 'get/putDoubleValue' (c-p) for 'float' and 'double'
// [12] CONCERN: 'putLength' & 'getLength'
// [13] CONCERN: 'putValue' & 'getValue' for 'bsl::string'
// [14] CONCERN: 'putValue' & 'getValue' for 'bslstl::StringRef'
// [15] CONCERN: 'putValue' & 'getValue' for date/time types
// [18] CONCERN: 'putValue' & 'getValue' for date/time types
// [19] CONCERN: 'putValue' & 'getVaule' for date/time types brute force
// [20] CONCERN: 'putValue' for date/time types
// [21] CONCERN: 'getValue' for date/time and timezone variant types
// [27] CONCERN: 'getValue' reports all failures to read from stream buffer
// [28] CONCERN: 'put'- & 'getValue' for date/time types in extended binary fmt
// [29] CONCERN: 'putValue' encoding formation selection

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define ASSERT_EQ(X,Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X,Y,X != Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)
#define LOOP2_ASSERT_EQ(L,M,X,Y) ASSERTV(L,M,X,Y,X == Y)
#define LOOP2_ASSERT_NE(L,M,X,Y) ASSERTV(L,M,X,Y,X != Y)

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

// ============================================================================
//              NON-STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define PRINT(STREAM, X)                                                      \
    do {                                                                      \
        (STREAM) << X;                                                        \
    } while (false)

#define PRINT_TAB(STREAM) PRINT(STREAM, "\t")
#define PRINT_NEWLINE(STREAM) PRINT(STREAM, "\n")

#define PRINT_VARIABLE(STREAM, X)                                             \
    do {                                                                      \
        (STREAM) << #X << ": " << (X);                                        \
    } while (false)

#define PRINT_UNLESS(STREAM, X)                                               \
    do {                                                                      \
        if (!(X)) {                                                           \
            PRINT_VARIABLE(STREAM, X);                                        \
            PRINT_NEWLINE(STREAM);                                            \
        }                                                                     \
    } while (false)

#define PRINT_UNLESS1(STREAM, I, X)                                           \
    do {                                                                      \
        if (!(X)) {                                                           \
            PRINT_VARIABLE(STREAM, I);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, X);                                        \
            PRINT_NEWLINE(STREAM);                                            \
        }                                                                     \
    } while (false)

#define PRINT_UNLESS2(STREAM, I, J, X)                                        \
    do {                                                                      \
        if (!(X)) {                                                           \
            PRINT_VARIABLE(STREAM, I);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, J);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, X);                                        \
            PRINT_NEWLINE(STREAM);                                            \
        }                                                                     \
    } while (false)

#define PRINT_UNLESS3(STREAM, I, J, K, X)                                     \
    do {                                                                      \
        if (!(X)) {                                                           \
            PRINT_VARIABLE(STREAM, I);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, J);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, K);                                        \
            PRINT_TAB(STREAM);                                                \
            PRINT_VARIABLE(STREAM, X);                                        \
            PRINT_NEWLINE(STREAM);                                            \
        }                                                                     \
    } while (false)

#define PRINT_UNLESS_EQ(STREAM, X, Y) PRINT_UNLESS2(STREAM, X, Y, X == Y)
#define PRINT_UNLESS_NE(STREAM, X, Y) PRINT_UNLESS2(STREAM, X, Y, X != Y)

#define PRINT_LINE_UNLESS(STREAM, X) PRINT_UNLESS1(STREAM, __LINE__, X)

#define PRINT_LINE_UNLESS_EQ(STREAM, X, Y)                                    \
    PRINT_UNLESS3(STREAM, __LINE__, X, Y, X == Y)

#define PRINT_LINE_UNLESS_NE(STREAM, X, Y)                                    \
    PRINT_UNLESS3(STREAM, __LINE__, X, Y, X != Y)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };
enum { SUCCESS = 0, FAILURE = -1 };

typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

typedef balber::BerUtil         Util;
typedef bslstl::StringRef       StringRef;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

int numOctets(const char *s)
    // Return the number of octets contained in the specified 's'.  Note that
    // it is assumed that each octet in 's' is specified in hex format.
{
    int length = 0;
    for (; *s; ++s) {
        if (' ' == *s) {
            continue;
        }

        ++length;
    }
    return length / 2;
}

int getIntValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';                                               // RETURN
    }
    c = static_cast<char>(toupper(c));
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;                                          // RETURN
    }
    ASSERT(0);
    return -1;
}

int compareBuffers(const char *stream, const char *buffer)
    // Compare the data written to the specified 'stream' with the data in the
    // specified 'buffer'.  Return 0 on success, and -1 otherwise.
{
    while (*buffer) {
        if (' ' == *buffer) {
            ++buffer;
            continue;
        }
        char temp = static_cast<char>(getIntValue(*buffer) << 4);
        ++buffer;
        temp = static_cast<char>(temp | getIntValue(*buffer));
        if (*stream != temp) {
           return -1;                                                 // RETURN
        }
        ++stream;
        ++buffer;
    }
    return 0;
}

void printBuffer(bsl::ostream& stream, const char *buffer, bsl::size_t length)
    // Print the specified 'buffer' of the specified 'length' in hex form to
    // the specified 'stream'.
{
    stream << hex;
    int numOutput = 0;
    for (bsl::size_t i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            stream << '0';
        }
        stream << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            stream << " ";
        }
    }
    stream << dec << endl;
}

void printBuffer(const char *buffer, bsl::size_t length)
    // Print the specified 'buffer' of the specified 'length' in hex form
{
    printBuffer(cout, buffer, length);
}

#define DOUBLE_MANTISSA_MASK   0xfffffffffffffLL
#define DOUBLE_SIGN_MASK       ((long long) ((long long) 1                   \
                                               << (sizeof(long long) * 8 - 1)))

void assembleDouble(double *value, int sign, int exponent, long long mantissa)
{
    enum {
        DOUBLE_EXPONENT_SHIFT  = 52,
        DOUBLE_BIAS            = 1023
    };

    unsigned long long *longLongValue
                               = reinterpret_cast<unsigned long long *>(value);

    *longLongValue  = (unsigned long long) exponent << DOUBLE_EXPONENT_SHIFT;
    *longLongValue |= mantissa & DOUBLE_MANTISSA_MASK;

    if (sign) {
        *longLongValue |= DOUBLE_SIGN_MASK;
    }
}

// ============================================================================
//                            ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

// GLOBAL DATA
static bool verbose         = false;
static bool veryVerbose     = false;
static bool veryVeryVerbose = false;

namespace u {

                               // ===============
                               // struct TestUtil
                               // ===============

struct TestUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by various tests in this test driver.

    // CLASS METHODS
    template <class TYPE>
    static bool bytesDecodeToValue(bsl::ostream&  log,
                                   const char    *buffer,
                                   bsl::size_t    bufferLength,
                                   const TYPE&    value,
                                   int            numBytesConsumed);
        // Return 'true' if 'balber::BerUtil::getValue' decodes the contents of
        // the specified 'buffer' having the specified 'bufferLength' to the
        // specified 'value' and consumes the specified 'numBytesConsumed' in
        // the process, and return 'false' otherwise.  If 'buffer' does not
        // successfully decode to 'value', then log an unspecified,
        // human-readable description of the error condition to the specified
        // 'log'.

    template <class TYPE>
    static bool valueEncodesToBytes(
                       bsl::ostream&                     log,
                       bsl::streambuf                   *bytes,
                       const TYPE&                       value,
                       const balber::BerEncoderOptions&  options,
                       const char                       *hexadecimalExpression,
                       bsl::size_t                       numBytesConsumed);
        // Return 'true' if 'balber::BerUtil::putValue' encodes the specified
        // 'value' to the bytes specified by the 'hexadecimalExpression' using
        // the specified 'options' and consumes the specified
        // 'numBytesConsumed' from the bytes specified by
        // 'hexadecimalExpression' when doing so, and return 'false' otherwise.
        // Load into the specified 'bytes' the encoded contents of 'value'.  If
        // 'value' does not encode to the bytes specified by
        // 'hexadecimalExpression', load an unspecified, human-readable
        // description of the error condition to the specified 'log'.
};


                             // ==================
                             // class Case27Tester
                             // ==================

class Case27Tester {
    // This function-object class implements an operation that tests that
    // 'getValue' returns a non-zero value when the supplied stream buffer
    // reaches the end of its source before a value has finished being read.

  public:
    // ACCESSORS
    template <class SIMPLE_TYPE>
    void operator()(int LINE, const SIMPLE_TYPE& VALUE) const;
        // Increment the 'testStatus' and log an unspecified human-readable
        // error message mentioning the specified 'LINE' to 'bsl::cout' unless
        // the conditions that are concerns in test case 27 are verified.  See
        // the documentation for test case 27 for more details.
};

                            // =====================
                            // struct ByteBufferUtil
                            // =====================

struct ByteBufferUtil {
    // This utility 'struct' provides a namespace for a suite of function that
    // operate arrays of 'char' values.

    // CLASS METHODS
    static int loadBuffer(int        *numBytesWritten,
                          char       *buffer,
                          int         bufferSize,
                          const char *expression,
                          int         expressionSize);
        // Load to the specified 'buffer' the bytes corresponding to the
        // sequence of big-endian hexadecimal digits defined by the specified
        // 'expression' having the specified 'expressionSize' length.  Load to
        // the specified 'numBytesWritten' the number of bytes loaded to the
        // 'buffer'.  Return 0 on success, and a non-zero value otherwise.
        // Return a non-zero value if the 'expression' defines a sequence of
        // hexadecimal digits that is longer than 'buffer'.  The behavior is
        // undefined unless 'expressionSize' matches the regular expression '('
        // '|[A-F]|[0-9])*', where whitespace has no semantic meaning, 'A-F'
        // indicate numerical values 10-15 respectively, and '0-9' indicate
        // numeric values 0-9 respectively.  Each digit loaded to 'buffer' is
        // defined by a pair of digits in 'expression'.  The behavior is
        // undefined if 'expression' contains an odd number of digits.
};

                          // =========================
                          // class RandomInputIterator
                          // =========================

class RandomInputIterator {
    // This value-semantic class provides an implementation of the
    // 'InputIterator' concept for a sequence of pseudo-random
    // 'unsigned char' values.

    // DATA
    int           d_seed;   // linear congruential generator seed
    unsigned char d_value;  // current pseudo-random value

    // PRIVATE CLASS METHODS
    static unsigned char generateValue(int *seed);
        // Return a pseudo-random value deterministically generated from the
        // value of the integer addressed by the specified 'seed' and update
        // set the integer to a new pseudo-random value.

  public:
    // TRAITS
    typedef int                      difference_type;
    typedef unsigned char            value_type;
    typedef const unsigned char     *pointer;
    typedef const unsigned char&     reference;
    typedef bsl::input_iterator_tag  iterator_category;

    // CLASS METHODS
    static bool areEqual(const RandomInputIterator& lhs,
                         const RandomInputIterator& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' have the same value,
        // and return 'false' otherwise.  Two 'RandomInputIterator' objects
        // have the same value if and only if they have the same 'seed'
        // attribute.

    // CREATORS
    RandomInputIterator();
        // Create a new 'RandomInputIterator' object having a 0 'seed'
        // attribute.

    explicit RandomInputIterator(int seed);
        // Create a new 'RandomInputIterator' object having a 'seed' attribute
        // with the specified 'seed' value.

    RandomInputIterator(const RandomInputIterator& original);
        // Create a new 'RandomInputIterator' object having a copy of the value
        // of the specified 'original' object.

    // MANIPULATORS
    RandomInputIterator& operator=(const RandomInputIterator& original);
        // Assign a copy of the value of the specified 'original' object to the
        // value of this object.  Return a reference providing modifiable
        // access to this object.

    RandomInputIterator& operator++();
        // Assign to this object a new pseudo-random value deterministically
        // generated from the 'seed' attribute of this object.  Return a
        // reference providing modifiable access to this object.

    RandomInputIterator operator++(int);
        // Assign to this object a new pseudo-random value deterministically
        // generated from the 'seed' attribute of this object.  Return a copy
        // of the value of this object prior to assigning a new value.

    // ACCESSORS
    const unsigned char& operator*() const;
        // Return a reference providing non-modifiable access to the
        // current pseudo-random value generated by this object.

    const unsigned char *operator->() const;
        // Return a pointer providing non-modifiable access to the
        // current pseudo-random value generated by this object.
};

                        // ============================
                        // class BasicRandomValueLoader
                        // ============================

template <class INPUT_ITERATOR>
class BasicRandomValueLoader {
    // This mechanism class provides a function-object whose function-call
    // operator may be used to load pseudo-random values into objects of some
    // fundamental types.  These pseudo-random values are deterministically
    // generated based on the sequence of 'unsigned char' values received from
    // an internally-managed object of the specified 'INPUT_ITERATOR'.  The
    // program is ill-formed unless the 'INPUT_ITERATOR' satisfies the
    // 'InputIterator' concept having an 'unsigned char' 'value_type', and
    // which has no 'end'.

  public:
    // TYPES
    typedef INPUT_ITERATOR InputIteratorType;
        // The 'InputIterator' implementation wrapped by this object that
        // provides the underlying source of pseudo-random data.

  private:
    // DATA
    InputIteratorType d_iterator;
        // underlying source of pseudo-random data

  public:
    // CREATORS
    BasicRandomValueLoader();
        // Create a new 'BasicRandomValueLoader' object having a default
        // underlying 'iterator' attribute.

    explicit BasicRandomValueLoader(InputIteratorType iterator);
        // Create a new 'BasicRandomValueLoader' object an underlying
        // 'iterator' attribute with a copy of the value of the
        // specified 'iterator'.

    BasicRandomValueLoader(const BasicRandomValueLoader& original);
        // Create a new 'BasicRandomValueLoader' object having a copy of value
        // of the specified 'original' object.

    // MANIPULATORS
    BasicRandomValueLoader& operator=(const BasicRandomValueLoader& original);
        // Assign to this object a copy of the value of the specified
        // 'original' object and return a reference providing modifiable access
        // to this object.

    void operator()(bool *value);
    void operator()(char *value);
    void operator()(unsigned char *value);
    void operator()(signed char *value);
    void operator()(unsigned *value);
    void operator()(int *value);
    void operator()(unsigned long long *value);
    void operator()(long long *value);
    void operator()(float *value);
    void operator()(double *value);
        // Deterministically load a pseudo-random value into the specified
        // 'value' based on the underlying 'iterator' attribute of this object
        // and increment the 'iterator' attribute of this object.
};

                       // ==============================
                       // namespace RandomValueFunctions
                       // ==============================

namespace RandomValueFunctions {

    // FREE FUNCTIONS
    template <class VALUE_TYPE, class LOADER>
    void loadRandomValue(VALUE_TYPE *value, LOADER& loader);
        // Deterministically load a pseudo-random value into the specified
        // 'value' using the specified 'loader'.

}  // close RandomValueFunctions namespace

                           // ======================
                           // struct RandomValueUtil
                           // ======================

struct RandomValueUtil {
    // This utility 'struct' provides a namespace for a suite of functions that
    // provide non-primitive functionality for psuedo-random value 'loader'
    // objects.

    // CLASS METHODS
    template <class VALUE_TYPE, class LOADER>
    static void load(VALUE_TYPE *value, LOADER& loader);
        // Deterministically load a pseudo-random value into the specified
        // 'value' using the specified 'loader' object.

    template <class VALUE_TYPE, class LOADER>
    static VALUE_TYPE generate(LOADER& loader);
        // Return a pseudo-random value of the specified 'VALUE_TYPE'
        // using the specified 'loader' object.

    template <class INTEGRAL_TYPE, class LOADER>
    static INTEGRAL_TYPE generateModulo(LOADER& loader, INTEGRAL_TYPE base);
        // Return a pseudo-random value of the specified 'INTEGRAL_TYPE' having
        // a value with magnitude no greater than the specified 'base' using
        // the specified 'loader'.  The program is ill-formed unless the
        // 'INTEGRAL_TYPE' is a fundamental integral type.

    template <class INTEGRAL_TYPE, class LOADER>
    static INTEGRAL_TYPE generateInInterval(LOADER&       loader,
                                            INTEGRAL_TYPE minimum,
                                            INTEGRAL_TYPE maximum);
        // Return a pseudo-random value of the specified 'INTEGRAL_TYPE' having
        // a value no less than 'minimum' and no greater than 'maximum' using
        // the specified 'loader'.  The program is ill-formed unless the
        // 'INTEGRAL_TYPE' is a fundamental integral type.
};

                     // ===================================
                     // customization point loadRandomValue
                     // ===================================

template <class LOADER>
void loadRandomValue(bdldfp::Decimal64 *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bsl::string *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::Date *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::DateTz *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::Datetime *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::DatetimeTz *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::Time *value, LOADER& loader);
template <class LOADER>
void loadRandomValue(bdlt::TimeTz *value, LOADER& loader);
template <class LOADER, class TYPE, class TYPETZ>
void loadRandomValue(bdlb::Variant2<TYPE, TYPETZ> *value, LOADER& loader);
    // Deterministically load a pseudo-random value into the specified 'value'
    // using the specified 'loader'.  Note that the values are not guaranteed
    // to be distributed according to any particular criteria.  Individual
    // values considered uninteresting may be common and repeated.  This
    // function is intended to be used to sample large numbers of values, such
    // that potential low quality of the sample distribution is compensated by
    // the large number of samples.

                            // ===================
                            // class ByteArrayUtil
                            // ===================

struct ByteArrayUtil {
    // This utility 'struct' provides a suite of functions that provide
    // non-primitive functionality on a "byte array" pseudo-type defined by a
    // pair of 'begin' and 'end' pointers to a contiguous array of 'unsigned
    // char' objects.

    // CLASS METHODS
    static void setTheUintAt(unsigned char *begin,
                             unsigned char *end,
                             bsl::size_t    index,
                             unsigned int   value);
        // Write the sequence of 4 bytes of the little-endian representation of
        // the specified 'value' to the index in the '[begin, end)' byte array
        // starting at the position 'index * 4'.  The behavior is undefined
        // unless 'end - begin end > index * 4'.

    static void setTheUint64At(unsigned char       *begin,
                               unsigned char       *end,
                               bsl::size_t          index,
                               bsls::Types::Uint64  value);
        // Write the sequence of 7 bytes of the little-endian representation of
        // the specified 'value' to the index in the '[begin, end)' byte array
        // starting at the position 'index * 8'.  The behavior is undefined
        // unless 'end - begin > index * 8'.

    static unsigned int theUintAt(const unsigned char *begin,
                                  const unsigned char *end,
                                  bsl::size_t          index);
        // Returned the 'unsigned int' value synthesized by interpreting
        // the 4 bytes in the range
        // '[begin + index * 4, begin + (index + 1) * 4)' as a little-endian
        // representation of an unsigned integer value.  The behavior is
        // undefined unless 'end - begin > index * 4'.
};

                            // ====================
                            // class Md5Fingerprint
                            // ====================

class Md5Fingerprint {
    // This in-core value-semantic class represents an MD5 "fingerprint", or
    // "digest", which is an ordered sequence of 128 bits.  This class provides
    // modifiable access to this sequence through 'unsigned char' and
    // 'unsigned int' values.  Many array-like operations are provided by this
    // class, including the ability to access iterators to the underlying
    // 'unsigned char' array.
    //
    // The underlying array of 16 'unsigned char' values the define the value
    // of this class are referred to as its 'array' attribute.

  private:
    // DATA
    unsigned char d_value[16];
        // underlying array of 128 bits, represented using 'unsigned char's

  public:
    // PUBLIC CLASS DATA
    enum {
        k_SIZE = 16 // number of 'unsigned char' values in the underlying array
    };

    // CREATORS
    Md5Fingerprint();
        // Create a new 'Md5Fingerprint' object having the zero value for each
        // 'unsigned char' element in its 'array' attribute.

    Md5Fingerprint(const Md5Fingerprint& original);
        // Create a new 'Md5Fingerprint' object having a copy of the value of
        // the specified 'original' object.

    // MANIPULATORS
    Md5Fingerprint& operator=(const Md5Fingerprint& original);
        // Assign to this object a copy of the value of the specified
        // 'original' object and return a reference providing modifiable access
        // to this object.

    unsigned char& operator[](bsl::size_t index);
        // Return a reference providing modifiable access to the 'unsigned char'
        // element of the 'array' attribute of this object at the specified
        // 'index'.  The behavior is undefined unless '16 > index'.

    void setTheUintAt(bsl::size_t index, unsigned int value);
        // Assign to the 4 'unsigned char' elements of the 'array' attribute of
        // this object at the specified 'index * 4' index the little-endian
        // representation of the specified 'value'.  The behavior is undefined
        // unless '4 > index'.

    unsigned char *data();
        // Return a pointer providing access to the first 'unsigned char'
        // element of the contiguous array of 'unsigned char' objects that make
        // up the 'array' attribute of this object.

    unsigned char *begin();
        // Return a pointer to the first element of the 'array' attribute of
        // this object.

    unsigned char *end();
        // Return a pointer to the "1-past-the-end" element of the 'array'
        // attribute of this object.

    // ACCESSORS
    const unsigned char& operator[](bsl::size_t index) const;
        // Return a reference providing non-modifiable access to the 'unsigned
        // char' element of the 'array' attribute of this object at the
        // specified 'index'.  The behavior is undefined unless '16 > index'.

    unsigned int theUintAt(bsl::size_t index) const;
        // Return the 'unsigned int' value that results from interpreting the 4
        // 'unsigned char' objects of the 'array' attribute of this object in
        // the range [index * 4, (index + 1) * 4)' as the little-endian
        // representation of an 'unsigned int' value.

    const unsigned char *data() const;
        // Return a pointer providing non-modifiable access to the first
        // 'unsigned char' element of the contiguous array of 'unsigned char'
        // objects that make up the 'array' attribute of this object.

    bsl::size_t size() const;
        // Return the size of the 'array' attribute of this object.

    const unsigned char *begin() const;
        // Return a pointer providing non-modifiable access to the first
        // element of the 'array' attribute of this object.

    const unsigned char *end() const;
        // Return a pointer providing non-modifiable access to the
        // "1-past-the-end" element of the 'array' attribute of this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format the referenced object to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation
        // 'level' and return a reference to 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested
        // objects.  If 'level' is negative, suppress indentation of the first
        // line.  If 'spacesPerLevel' is negative, format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.  For scalars and arrays of scalars, this 'print' function
        // delegates to the appropriate printing mechanism for referenced
        // object -- the aggregate adds no additional text to the
        // output.  (E.g., the result of printing an aggregate that references
        // a string is indistinguishable from the result of printing the
        // string directly.)  For list, row, choice, choice array item, table,
        // and choice array, this print function prepend's each field with the
        // name of the field.
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const Md5Fingerprint& rhs);
    // Format the specified 'rhs' in a human-readable form (same format as
    // 'rhs.print(stream, 0, -1)') and return a reference providing modifiable
    // access to the specified 'stream'.

                          // ========================
                          // class Md5FingerprintUtil
                          // ========================

struct Md5FingerprintUtil {
    // This utility 'struct' provides a suite of functions that provide
    // non-primitive functionality for 'Md5Fingerprint' objects.

  private:
    // PRIVATE CLASS DATA
    enum {
        k_A_SEED_VALUE = 0x67452301,
        k_B_SEED_VALUE = 0xEFCDAB89,
        k_C_SEED_VALUE = 0x98BADCFE,
        k_D_SEED_VALUE = 0x10325476
    };

  public:
    // CLASS METHODS
    static Md5Fingerprint getSeedValue();
        // Return an 'Md5Fingerprint' object having the initial value for
        // the "MD5" algorithm.
};

                               // ==============
                               // class Md5Block
                               // ==============

class Md5Block {
    // This in-core value-semantic class provides a representation of a
    // resizable array of 'unsigned char' values having a maximum size of '64'.

  public:
    // PUBLIC CLASS DATA
    enum {
        k_CAPACITY = 64
    };

  private:
    // DATA
    unsigned char d_bytes[k_CAPACITY]; // underlying byte array
    unsigned char d_numBytes;          // number of occupied bytes

  public:
    // CREATORS
    Md5Block();
        // Create a new 'Md5Block' object having an 'array' attribute with size
        // 0.

    template <class INPUT_ITERATOR>
    Md5Block(INPUT_ITERATOR begin, INPUT_ITERATOR end);
        // Create a new 'Md5Block' object having an 'array' attribute defined
        // by the sequence of bytes specified by the range '[begin, end)'.  The
        // program is ill-formed unless 'begin' and 'end' satisfy the
        // 'InputIterator' concept having an 'unsigned char' 'value_type'.  The
        // behavior is undefined unless the range '[begin, end)' has a size
        // no greater than 64.

    Md5Block(const Md5Block& original);
        // Create a new 'Md5Block' object having a copy of the value of the
        // specified 'original' object.

    // MANIPULATORS
    Md5Block& operator=(const Md5Block& original);
        // Assign to this object a copy of the value of the specified
        // 'original' object and return a reference providing modifiable access
        // to this object.

    unsigned char& operator[](bsl::size_t index);
        // Return a reference providing modifiable access to the 'unsigned
        // char' element of the 'array' attribute of this object at the
        // specified 'index'.  The behavior is undefined unless
        // 'size() > index'.

    void setTheUintAt(bsl::size_t index, unsigned int value);
        // Assign to the 4 'unsigned char' elements of the 'array' attribute of
        // this object at the specified 'index * 4' index the little-endian
        // representation of the specified 'value'.  The behavior is undefined
        // unless '4 > index'.

    unsigned char *begin();
        // Return a pointer to the first element of the 'array' attribute of
        // this object.

    unsigned char *end();
        // Return a pointer to the "1-past-the-end" element of the 'array'
        // attribute of this object.

    void clear();
        // Reset this object to the empty value, having an 'array' attribute
        // with size 0.

    void resize(bsl::size_t newSize);
        // Change the 'array' attribute to have the specified 'newSize' size.
        // If 'newSize == size()', do nothing.  If 'newSize > size()' then
        // append 'newSize() - size' 'unsigned char' elements having the value
        // 0 to the end of the array attribute of this object.  Otherwise,
        // remove the 'size() - newSize' elements from the end of the 'array'
        // attribute of this object.  The behavior is undefined unless '64 >=
        // newSize'.

    // ACCESSORS
    const unsigned char& operator[](bsl::size_t index) const;
        // Return a reference providing non-modifiable access to the 'unsigned
        // char' element of the 'array' attribute of this object at the
        // specified 'index'.  The behavior is undefined unless '16 > index'.

    unsigned int theUintAt(bsl::size_t index) const;
        // Return the 'unsigned int' value that results from interpreting the 4
        // 'unsigned char' objects of the 'array' attribute of this object in
        // the range [index * 4, (index + 1) * 4)' as the little-endian
        // representation of an 'unsigned int' value.

    const unsigned char *begin() const;
        // Return a pointer providing non-modifiable access to the first
        // element of the 'array' attribute of this object.

    const unsigned char *end() const;
        // Return a pointer providing non-modifiable access to the
        // "1-past-the-end" element of the 'array' attribute of this object.

    bool empty() const;
        // Return 'true' if '0 == size()', and return 'false' otherwise.

    bool full() const;
        // Return 'true' if '64 == size()', and return 'false' otherwise.

    bsl::size_t size() const;
        // Return the number of elements in the 'unsigned char' attribute of
        // this object.
};

// FREE FUNCTIONS
BSLA_MAYBE_UNUSED
bool operator==(const Md5Block& lhs, const Md5Block& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'Md5Block' objects have the same value if and
    // only if their 'array' attributes have the same value (including the same
    // size.)

BSLA_MAYBE_UNUSED
bool operator!=(const Md5Block& lhs, const Md5Block& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the different
    // values, and 'false' otherwise.  Two 'Md5Block' objects have different
    // values if and only if their 'array' attributes have different values,
    // (which may or may not including having different sizes.)

                            // ===================
                            // struct Md5BlockUtil
                            // ===================

struct Md5BlockUtil {
    // This utility 'struct' is a namespace for a suite of functions that
    // provide non-primitive functionality for 'Md5Block' objects.

    // TYPES
    typedef bsl::pair<Md5Block, Md5Block> Md5BlockPair;
        // Convenience alias for a pair of 'Md5Block' objects.

    // CLASS METHODS
    static void appendUint64(Md5Block *block, bsls::Types::Uint64 value);
        // Append the 8 bytes making up the little-endian representation of the
        // specified 'value' to the end of the specified 'block'.  The 'size'
        // of the 'block' increases by 8.  The behavior is undefined unless
        // '56 >= block->size()'.

    static void concatenate(Md5Block *first, Md5Block *second);
        // If the specified 'first' block is not full, append the lesser of
        // '64 - first->size()' or 'second->size()' bytes from the front of
        // the specified 'second' block to the 'first', and remove those bytes
        // from the 'second' block.
};

                         // ===========================
                         // class Md5BlockInputIterator
                         // ===========================

class Md5BlockInputIterator {
    // This class provides read-only uni-directional access to a sequence of
    // 'Md5Block' objects.  A pair of 'begin' and 'end' pointers that address a
    // valid contiguous '[begin, end)' range of 'unsigned char' objects specify
    // the sequence of 'Md5Block' objects accessed by a
    // 'Md5BlockInputIterator'.  A 'Md5BlockInputIterator' iterates over groups
    // of up to 64 elements of the underlying range to provide access to a
    // 'Md5Block' consisting of those elements.  If less than 64 elements are
    // available in the range, the size of the provided 'Md5Block' object will
    // be equal to the number of elements available in the range.

    // DATA
    Md5Block             d_block;       // current block adapted from range
    const unsigned char *d_iterator_p;  // pointer to first element of range
    const unsigned char *d_end_p;       // pointer to last+1 element of range

  public:
    // CLASS METHODS
    static bool areEqual(const Md5BlockInputIterator& lhs,
                         const Md5BlockInputIterator& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and return 'false' otherwise.  Two 'Md5BlockInputIterator'
        // objects have the same value if and only if they address the same
        // first 'unsigned char' and have the same 'end'.

    // CREATORS
    Md5BlockInputIterator();
        // Create a 'Md5BlockInputIterator' object that has the past-the-end
        // value for an unspecified range.

    Md5BlockInputIterator(const unsigned char *begin,
                          const unsigned char *end);
        // Create a 'Md5BlockInputIterator' object that addresses an 'Md5Block'
        // consisting of the lesser of the first 64 bytes in the '[begin, end)'
        // range or the size of the range.

    Md5BlockInputIterator(const Md5BlockInputIterator& original);
        // Create a 'Md5BlockInputIterator' object having the same value as the
        // specified 'original' object.

    // MANIPULATORS
    Md5BlockInputIterator& operator=(const Md5BlockInputIterator& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // Return a reference providing modifiable access to this object.

    Md5BlockInputIterator& operator++();
        // Advance this object to the next position.  Return a reference
        // providing modifiable access to this object.  The next position is 64
        // elements past the current position in the '[begin, end)' range
        // supplied on construction if there are at least 64 elements remaining
        // in the range.  If there are less than 64 elements remaining, the
        // next position includes all remaining elements.  Otherwise, the next
        // position is the past-the-end position.  The behavior is undefined if
        // this object is in the past-the-end position.

    Md5BlockInputIterator operator++(int);
        // Advance this object to the next position, and return, *by* *value*,
        // an iterator referring to the original position (*before* the
        // advancement).  The behavior is undefined if this object is in the
        // past-the-end position.

    // ACCESSORS
    const Md5Block& operator*() const;
        // Return a reference providing non-modifiable access to the 'Md5Block'
        // at the position in the underlying sequence referred to by this
        // object.  The behavior is undefined if this object is in the
        // past-the-end position.

    const Md5Block *operator->() const;
        // Return an address providing non-modifiable access to the 'Md5Block'
        // at the position in the underlying sequence referred to by this
        // object.  The behavior is undefined if this object is in the
        // past-the-end position.

};

// FREE FUNCTIONS
BSLA_MAYBE_UNUSED
bool operator==(const Md5BlockInputIterator& lhs,
                const Md5BlockInputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and return 'false' otherwise.  Two 'Md5BlockInputIterator'
    // objects have the same value if and only if they address the same first
    // 'unsigned char' and have the same 'end'.

BSLA_MAYBE_UNUSED
bool operator!=(const Md5BlockInputIterator& lhs,
                const Md5BlockInputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have different
    // values, and return 'false' otherwise.  Two 'Md5BlockInputIterator'
    // objects have different values if and only if they address different
    // first 'unsigned char' objects or have different 'end' values.

                          // ========================
                          // class Md5BlockInputRange
                          // ========================

class Md5BlockInputRange {
    // This class provides an adapter from a '[begin, end)' contiguous range of
    // 'unsigned char' values to a range of 'Md5Block' objects.  The value of
    // each subsequent object in the adapted range is the 'Md5Block' value
    // defined by the subsequence 64 elements of the underlying range.  When
    // less than 64 elements are available (at the end of the range, or the
    // beginning of a small range, for example), the value of the 'Md5Block'
    // will consist of all remaining elements.

    // DATA
    Md5BlockInputIterator d_begin;  // iterator to first block
    Md5BlockInputIterator d_end;    // iterator at the past-the-end position

  public:
    // CREATORS
    Md5BlockInputRange(const unsigned char *begin,
                       const unsigned char *end);
        // Create a 'Md5BlockInputRange' that adapts the contiguous range of
        // defined by '[begin, end)' to a range of 'Md5Block' objects.

    // ACCESSORS
    Md5BlockInputIterator begin() const;
        // Return an iterator to the first 'Md5Block' of the range supplied
        // to this object upon construction.

    Md5BlockInputIterator end() const;
        // Return the past-the-end iterator of the range supplied to this
        // object upon construction.
};

                               // ==============
                               // class Md5State
                               // ==============

class Md5State {
    // This in-core value-semantic attribute class provides a representation
    // of the 3 fields quantities maintained during the calculation of a
    // MD5 fingerprint for a stream of data.

    // DATA
    Md5Fingerprint      d_fingerprint;       // current MD5 fingerprint
    Md5Block            d_block;             // current incomplete block
    bsls::Types::Uint64 d_numBlocksConsumed; // number of blocks consumed

  public:
    // CREATORS
    Md5State();
        // Create a 'Md5State' object having a zero fingerprint, an empty
        // block, and having consumed no blocks.

    explicit Md5State(const Md5Fingerprint& fingerprint);
        // Create a 'Md5State' object having the specified 'fingerprint',
        // an empty block, and having consumed no blocks.

    Md5State(const Md5Fingerprint& fingerprint, const Md5Block& block);
        // Create a 'Md5State' object having the specified 'fingerprint',
        // the specified 'block', and having consumed no blocks.

    Md5State(const Md5Fingerprint& fingerprint,
             const Md5Block&       block,
             bsls::Types::Uint64   numBlocksConsumed);
        // Create a 'Md5State' object having the specified 'fingerprint',
        // the specified 'block', and having consumed the specified
        // 'numBlocksConsumed' number of blocks.

    Md5State(const Md5State& original);
        // Create a 'Md5State' object having the same value as the specified
        // 'original' object.

    // MANIPULATORS
    Md5State& operator=(const Md5State& rhs);
        // Assign to this object the same value as the specified 'rhs' object.
        // Return a reference providing non-modifiable access to this object.

    void setFingerprint(const Md5Fingerprint& value);
        // Set the value of the 'fingerprint' attribute of this object to
        // the specified 'value'.

    void setBlock(const Md5Block& value);
        // Set the 'block' attribute of this object to the specified 'value'.

    void setNumBlocksConsumed(bsls::Types::Uint64 value);
        // set the 'number of blocks consumed' attribute of this object to
        // the specified 'value'.

    // ACCESSORS
    const Md5Fingerprint& fingerprint() const;
        // Return a reference providing non-modifiable access to the
        // 'fingerprint' attribute of this object.

    const Md5Block& block() const;
        // Return a reference providing non-modifiable access to the
        // 'block' attribute of this object.

    bsls::Types::Uint64 numBlocksConsumed() const;
        // Return the value of the 'number of blocks consumed' attribute of
        // this object.
};

                             // ===================
                             // struct Md5StateUtil
                             // ===================

struct Md5StateUtil {
    // This utility 'struct' is a namespace for a suite of functions that
    // provide non-primitive functionality for 'Md5State' objects.

    // CLASS METHODS
    static Md5State getSeedValue();
        // Return a 'Md5State' object having the initial value for the
        // "MD5" algorithm.

    static void loadSeedValue(Md5State *value);
        // Load into the specified 'value' the initial value used for
        // the "MD5" algorithm.

    static void append(Md5State *state, const Md5Block& block);
        // Append the specified 'block' to the end of the 'block' of the
        // specified 'state'.  If, during the course of this operation,
        // the 'size' of the 'block' of the 'state' reaches 64, first
        // digest the 'state' (thereby updating its 'fingerprint') and
        // then reset its 'block' before continuing to append the
        // remainder of the contents of the specified 'block'.

    static void append(Md5State            *state,
                       const unsigned char *begin,
                       const unsigned char *end);
        // Append the bytes in the specified contiguous range '[begin, end)' to
        // the end of the 'block' of the specified 'state'.  When, during the
        // course of this operation, the 'size' of the 'block' of the 'state'
        // reaches 64, first digest the 'state' (thereby updating its
        // 'fingerprint') and then reset its 'block' before continuing to
        // append the remainder of the content of the specified range.  Note
        // that the digest operation may be performed more than once or not at
        // all, depending on the size of the specified range.

    static void appendPaddingAndLength(Md5State *state);
        // Load into the 'fingerprint' of the specified 'state' the final MD5
        // fingerprint of all of the data supplied to the 'state' so far.  If
        // the 'block' of the state has a 'size() < 56', then append a single
        // '0x80' byte to the block, followed by '63 - size()' zero bytes.  If
        // the 'block' has a 'size() == 56', reset the 'block' to one '0x80'
        // byte followed by 55 zero bytes.  Otherwise, reset the block to '56'
        // zero bytes.

    static void digest(Md5State *state);
        // Load into the 'fingerprint' of the specified 'state' the next
        // non-final MD5 fingerprint defined by the current 'fingerprint'
        // and 'block' of the state.

    static void digest(Md5Fingerprint *fingerprint, const Md5Block& block);
        // Load into the 'fingerprint' the next non-final MD5 fingerprint
        // defined by the current value of 'fingerprint' and the specified
        // 'block'.

    static Md5Fingerprint digest(const Md5State& state);
        // Return the next non-final MD5 fingerprint of the specified 'state'
        // defined by the current 'fingerprint' and 'block' of the state.

    static Md5Fingerprint digest(const Md5Fingerprint& fingerprint,
                                 const Md5Block&       block);
        // Return the next non-final MD5 fingerprint defined by the specified
        // 'fingerprint' and 'block'.
};

                        // ============================
                        // struct Md5StateUtil_ImplUtil
                        // ============================

struct Md5StateUtil_ImplUtil {
    // This utility 'struct' is a namespace for a suite of functions that
    // provide non-primitive functionality on 'Md5State' objects.
    // Note that many of the member names of this class are short and opaque.
    // These names are meant to reflect the names used in the specification
    // of the MD5 algorithm from the April 1992 revision of IETF RFC 1321.

  private:
    // CLASS INVARIANTS
    BSLMF_ASSERT(4 == sizeof(int));

    // PRIVATE CLASS METHODS
    static unsigned int f(unsigned int x, unsigned int y, unsigned int z);
        // Return 'xy | ~xz' where juxtaposition denotes binary conjunction,
        // '|' binary disjunction, and '~' binary negation.

    static unsigned int g(unsigned int x, unsigned int y, unsigned int z);
        // Return 'xz | y(~z)' where juxtaposition denotes binary conjunction,
        // '|' binary disjunction, and '~' binary negation.

    static unsigned int h(unsigned int x, unsigned int y, unsigned int z);
        // Return 'z XOR y XOR z'.

    static unsigned int i(unsigned int x, unsigned int y, unsigned int z);
        // Return 'y XOR (x | ~z)' where '|' denotes binary disjunction and
        // '~' binary negation.

    static unsigned int rotateLeft(unsigned int value, unsigned int numBits);
        // Return the result of circularly rotating the bits of the specified
        // 'value' to the left by the specified 'numBits' number of bits.

  public:
    // CLASS DATA
    enum {
        k_BYTES_PER_BLOCK = 64,
        k_BLOCK_SIZE      = 16
    };

    static const unsigned int k_T_TABLE[64];
        // Table of contents used as the values indexed by the values in the
        // 'k_I_TABLE'.  This is used in the implementations of 'round1Op',
        // 'round2Op', 'round3Op', and 'round4Op'.

    static const unsigned int k_I_TABLE[4][16];
        // Table of constants used as the argument for the 'i' parameter in
        // 'round1Op', 'round2Op', 'round3Op', and 'round4Op' when digesting
        // an Md5Block as part of computing an MD5 fingerprint.  This table
        // is first indexed by round number and then by the index of the
        // operation in that round.

    static const unsigned int k_K_TABLE[4][16];
        // Table of constants used as the argument for the 'k' parameter in
        // 'round1Op', 'round2Op', 'round3Op', and 'round4Op' when digesting
        // an Md5Block as part of computing an MD5 fingerprint.  This table
        // is first indexed by round number and then by the index of the
        // operation in that round.

    static const unsigned int k_S_TABLE[4][16];
        // Table of constants used as the argument for the 's' parameter in
        // 'round1Op', 'round2Op', 'round3Op', and 'round4Op' when digesting
        // an Md5Block as part of computing an MD5 fingerprint.  This table
        // is first indexed by round number and then by the index of the
        // operation in that round.

    // CLASS METHODS
    static void round1Op(unsigned int    *a,
                         unsigned int    *b,
                         unsigned int    *c,
                         unsigned int    *d,
                         const Md5Block&  x,
                         unsigned int     i,
                         unsigned int     k,
                         unsigned int     s);
        // Load into the specified 'a', 'b', 'c', and 'd' a value
        // deterministically determined from the specified 'x', 'i', 'k', and
        // 's'.   The essential behavior of this function is equivalent to the
        // first round operation for updating the MD5 state as specified in the
        // April 1992 revision of IETF RFC 1321.

    static void round2Op(unsigned int    *a,
                         unsigned int    *b,
                         unsigned int    *c,
                         unsigned int    *d,
                         const Md5Block&  x,
                         unsigned int     i,
                         unsigned int     k,
                         unsigned int     s);
        // Load into the specified 'a', 'b', 'c', and 'd' a value
        // deterministically determined from the specified 'x', 'i', 'k', and
        // 's'.   The essential behavior of this function is equivalent to the
        // second round operation for updating the MD5 state as specified in the
        // April 1992 revision of IETF RFC 1321.

    static void round3Op(unsigned int    *a,
                         unsigned int    *b,
                         unsigned int    *c,
                         unsigned int    *d,
                         const Md5Block&  x,
                         unsigned int     i,
                         unsigned int     k,
                         unsigned int     s);
        // Load into the specified 'a', 'b', 'c', and 'd' a value
        // deterministically determined from the specified 'x', 'i', 'k', and
        // 's'.   The essential behavior of this function is equivalent to the
        // third round operation for updating the MD5 state as specified in the
        // April 1992 revision of IETF RFC 1321.

    static void round4Op(unsigned int    *a,
                         unsigned int    *b,
                         unsigned int    *c,
                         unsigned int    *d,
                         const Md5Block&  x,
                         unsigned int     i,
                         unsigned int     k,
                         unsigned int     s);
        // Load into the specified 'a', 'b', 'c', and 'd' a value
        // deterministically determined from the specified 'x', 'i', 'k', and
        // 's'.   The essential behavior of this function is equivalent to the
        // fourth round operation for updating the MD5 state as specified in
        // the April 1992 revision of IETF RFC 1321.
};

                               // ==============
                               // struct Md5Util
                               // ==============

struct Md5Util {
    // This utility 'struct' is a namespace for a suite of top-level functions
    // pertaining to computing the MD5 fingerprint of a sequence of data.

    // CLASS METHODS
    static Md5Fingerprint getFingerprint(const unsigned char *begin,
                                         const unsigned char *end);
        // Return the MD5 fingerprint of the data in the contiguous range
        // '[begin, end)'.
};

                         // ==========================
                         // class Md5ChecksumAlgorithm
                         // ==========================

class Md5ChecksumAlgorithm {
    // This function-object class provides an implementation of the
    // requirements for a hashing algorithm as specified in the 'bslh_hash'
    // component.  It is suitable for use as a message digest.  Note that
    // the 'result_type' of this function object is not convertible to
    // 'bsl::size_t', and so this class cannot be used as the hashing
    // algorithm for a 'bslh::Hash' object.

    // DATA
    Md5State d_state; // current MD5 state

    // NOT IMPLEMENTED
    Md5ChecksumAlgorithm(const Md5ChecksumAlgorithm&) BSLS_KEYWORD_DELETED;
    Md5ChecksumAlgorithm& operator=(const Md5ChecksumAlgorithm)
            BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef Md5Fingerprint result_type;
        // 'result_type' is an alias to the value type returned by this hash
        // algorithm.

    // CREATORS
    Md5ChecksumAlgorithm();
        // Create a 'Md5ChecksumAlgorithm' object.

    // MANIPULATORS
    void operator()(const void *data, bsl::size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes' length, into the internal state of this hashing
        // algorithm.  Every bit of data incorporated into the internal state
        // of the algorithm will contribute to the final hash produced by
        // 'computeHash()'.  The same hash value will be produced regardless of
        // whether a sequence of bytes is passed in all at once or through
        // multiple calls to this member function.  Input where 'numBytes' is 0
        // will have no affect on the internal state of the algorithm.  The
        // behavior is undefined unless 'data' addresses a contiguous array of
        // at least 'numBytes' initialized memory.

    result_type computeChecksum();
        // Return the finalized version of the hash that has been accumulated
        // and make an unspecified change to the internal state of the
        // algorithm.  Note that calling 'computeHash' multiple times in a row
        // will return different results, and only the first result returned
        // will match the expected result of the algorithm.
};

                               // ==============
                               // class Checksum
                               // ==============

template <class CHECKSUM_ALGORITHM>
class Checksum {
    // This function-object class provides a wrapper around the specified
    // 'HASH_ALGORITHM' capable of computing a checksum, or "fingerprint" of
    // objects.  This class template is structured similarly to 'bslh::Hash',
    // but has the ability to return the 'result_type' of the 'HASH_ALGORITHM',
    // rather than 'bsl::size_t'.  The program is ill-formed unless the
    // 'HASH_ALGORITHM' class meets the requirements of a 'HashAlgorithm'
    // specified in the 'bslh_hash' component.

  public:
    // TYPES
    typedef CHECKSUM_ALGORITHM ChecksumAlgorithm;
        // 'ChecksumAlgorithm' is an alias to the specified
        // 'CHECKSUM_ALGORITHM'.

    typedef typename ChecksumAlgorithm::result_type result_type;
        // 'result_type' is an alias to the value type returned by the
        // function-call operator of objects of 'HASH_ALGORITHM' type, as well
        // as the function-call operator of this class.

    // ACCESSORS
    template <class TYPE>
    result_type operator()(const TYPE& object) const;
        // Return a hash value generated by the 'HASH_ALGORITHM' for the
        // specified 'object'.
};

                             // ===================
                             // struct ChecksumUtil
                             // ===================

struct ChecksumUtil {
    // This utility 'struct' is a namespace for a suite of functions that
    // compute a checksum for an object.

    // CLASS METHODS
    template <class OBJECT_TYPE>
    static Md5Fingerprint getMd5(const OBJECT_TYPE& object);
        // Return the MD5 fingerprint of the specified 'object' as specified by
        // applying the MD5 Message-Digest algorithm to the sequence of bytes
        // supplied to the 'HASH_ALG' of the overload of 'hashAppend' provided
        // for the specified 'object'.  The behavior is undefined unless a
        // well-formed overload of 'hashAppend' for the specified 'object' is
        // reachable.
};

                         // =========================
                         // class PutValueFingerprint
                         // =========================

class PutValueFingerprint {
    // This in-core value-semantic attribute type provides a set of
    // configuration parameters that determine an equivalence class of
    // partially-applied 'balber::BerUtil::putValue' functions.  The
    // 'hashAppend' overload provided by this class appends the hash values of
    // the output for a large, deterministically and pseudo-randomly generated
    // set of input for 'balber::BerUtil::putValue'.  The attributes of this
    // class constitute the partially-applied parameters, are not
    // pseudo-randomly generated during the calculation of the hash of this
    // object, and instead are fixed to the value of the attribute.

    // DATA
    int  d_seed;
        // pseudo-random seed

    int  d_numSamples;
        // number of input-output pairs to generate

    int  d_fractionalSecondPrecision;
        // value to fix for the 'datetimeFractionalSecondPrecision'
        // 'balber::BerEncoderOptions' attribute to supply to
        // 'balber::BerUtil::putValue'

    bool d_encodeDateAndTimeTypesAsBinary;
        // value to fix for the 'encodeDateAndTimeTypesAsBinary'
        // 'balber::BerEncoderOptions' attribute to supply to
        // 'balber::BerUtil::putValue'

  public:
    // CREATORS
    PutValueFingerprint();
        // Create a 'PutValueFingerprint' object having a 0 'seed' attribute, a
        // 0 'numSamples' attribute, a 0 'fractionalSecondPrecision' attribute,
        // and a 'false' 'encodeDateAndTimeTypesAsBinary' attribute.

    // MANIPULATORS
    void setSeed(int value);
        // Assign the specified 'value' to the 'seed' attribute of this
        // object.

    void setNumSamples(int value);
        // Assign the specified 'value' to the 'numSamples' attribute of this
        // object.

    void setFractionalSecondPrecision(int value);
        // Assign the specified 'value' to the 'fractionalSecondPrecision'
        // attribute of this object.

    void setEncodeDateAndTimeTypesAsBinary(bool value);
        // Assign the specified 'value' to the 'encodeDateAndTimeTypesAsBinary'
        // attribute of this object.

    // ACCESSORS
    int seed() const;
        // Return the value of the 'seed' attribute of this object.

    int numSamples() const;
        // Return the value of the 'numSamples' attribute of this object.

    int fractionalSecondPrecision() const;
        // Return the value of the 'fractionalSecondPrecision' attribute of
        // this object.

    bool encodeDateAndTimeTypesAsBinary() const;
        // Return the value of the 'encodeDateAndTimeTypesAsBinary' attribute
        // of this object.
};

// FREE FUNCTIONS
template <class ALGORITHM>
void checksumAppend(ALGORITHM& algorithm, const PutValueFingerprint& object);
    // Deterministically and pseudo-randomly generate 'object.numSamples()'
    // number of inputs for 'balber::BerUtil::putValue' using 'object.seed()'
    // as a pseudo-random seed.  Supply a 'balber::BerEncoderOptions' object
    // having the default value except for the attributes
    // 'datetimeFractionalSecondPrecision' and
    // 'encodeDateAndTimeTypesAsBinary', which have the value
    // 'object.fractionalSecondPrecision()' and
    // 'object.encodeDateAndTimeTypesAsBinary()' respectively, instead.  Supply
    // all of the output of invoking 'balber::BerUtil::putValue' with the
    // encoding options and generated input to the specified checksum
    // 'algorithm'.

                     // ===================================
                     // struct PutValueFingerprint_ImplUtil
                     // ===================================

struct PutValueFingerprint_ImplUtil {
    // This utility 'struct' is a namespace for a suite of functions used in
    // the implementation of the 'PutValueFingerprint' class.

    // CLASS METHODS
    template <class VALUE, class LOADER>
    static void putRandomValue(bsl::streambuf                   *streamBuf,
                               LOADER&                           loader,
                               const balber::BerEncoderOptions&  options);
        // Encode a pseudo-random value loaded by the specified 'loader' into
        // the specified 'streamBuf' using the encoding provided by invoking
        // 'balber::BerUtil::putValue' with the pseudo-random value and the
        // specified 'options'.
};

                          // =========================
                          // class GetValueFingerprint
                          // =========================

class GetValueFingerprint {
    // This in-core value-semantic attribute type provides a set of
    // configuration parameters that determine an equivalence class of
    // partially-applied 'balber::BerUtil::getValue' functions.  The
    // 'hashAppend' overload provided by this class appends the hash values of
    // the output of a large, deterministically, and pseudo-randomly generated
    // set of input for 'balber::BerUtil::getValue'.  The attributes of this
    // class constitute the partially-applied parameters, are not
    // pseudo-randomly generated during the calculation of the hash value of
    // this object, and instead are fixed to the value of the attribute.

    // DATA
    int  d_seed;
        // pseudo-random seed

    int  d_numSamples;
        // number of input-output pairs to generate

    int  d_fractionalSecondPrecision;
        // value to fix for the 'datetimeFractionalSecondPrecision'
        // 'balber::BerEncoderOptions' attribute to supply to
        // 'balber::BerUtil::putValue'

    bool d_encodeDateAndTimeTypesAsBinary;
        // value to fix for the 'encodeDateAndTimeTypesAsBinary'
        // 'balber::BerEncoderOptions' attribute to supply to
        // 'balber::BerUtil::putValue'

  public:
    // CREATORS
    GetValueFingerprint();
        // Create a 'GetValueFingerprint' object having a 0 'seed' attribute, a
        // 0 'numSamples' attribute, a 0 'fractionalSecondPrecision' attribute,
        // and a 'false' 'encodeDateAndTimeTypesAsBinary' attribute.

    // MANIPULATORS
    void setSeed(int value);
        // Assign the specified 'value' to the 'seed' attribute of this
        // object.

    void setNumSamples(int value);
        // Assign the specified 'value' to the 'numSamples' attribute of this
        // object.

    void setFractionalSecondPrecision(int value);
        // Assign the specified 'value' to the 'fractionalSecondPrecision'
        // attribute of this object.

    void setEncodeDateAndTimeTypesAsBinary(bool value);
        // Assign the specified 'value' to the 'encodeDateAndTimeTypesAsBinary'
        // attribute of this object.

    // ACCESSORS
    int seed() const;
        // Return the value of the 'seed' attribute of this object.

    int numSamples() const;
        // Return the value of the 'numSamples' attribute of this object.

    int fractionalSecondPrecision() const;
        // Return the value of the 'fractionalSecondPrecision' attribute of
        // this object.

    bool encodeDateAndTimeTypesAsBinary() const;
        // Return the value of the 'encodeDateAndTimeTypesAsBinary' attribute
        // of this object.
};

// FREE FUNCTIONS
template <class ALGORITHM>
void checksumAppend(ALGORITHM& algorithm, const GetValueFingerprint& object);
    // Deterministically and pseudo-randomly generate 'object.numSamples()'
    // number of inputs for 'balber::BerUtil::getValue' using 'object.seed()'
    // as a pseudo-random seed.  Provide input to 'balber::BerUtil::getValue'
    // using an invocation of 'balber::BerUtil::putValue' supplied with a
    // 'balber::BerEncoderOptions' object having the default value except for
    // the attributes 'datetimeFractionalSecondPrecision' and
    // 'encodeDateAndTimeTypesAsBinary', which have the value
    // 'object.fractionalSecondPrecision()' and
    // 'object.encodeDateAndTimeTypesAsBinary()' respectively, instead.  Supply
    // all of the output of invoking 'balber::BerUtil::getValue' with the
    // encoding options and generated input to the specified checksum
    // 'algorithm'.

                     // ===================================
                     // struct GetValueFingerprint_ImplUtil
                     // ===================================

struct GetValueFingerprint_ImplUtil {
    // This utility 'struct' is a namespace for a suite of functions used in
    // the implementation of the 'GetValueFingerprint' class.

    // CLASS METHODS
    template <class VALUE, class VALUETZ, class LOADER>
    static void getRandomValue(
                       bdlb::Variant2<VALUE, VALUETZ>   *value,
                       int                              *accumNumBytesConsumed,
                       LOADER&                           loader,
                       const balber::BerEncoderOptions&  options);
    template <class VALUE, class LOADER>
    static void getRandomValue(
                       VALUE                            *value,
                       int                              *accumNumBytesConsumed,
                       LOADER&                           loader,
                       const balber::BerEncoderOptions&  options);
        // Decode a pseudo-random value loaded by the specified 'loader' and
        // encoded by supplied the value and the specified 'options' to
        // 'balber::BerUtil::putValue'.  Load the value of the decoded object
        // into the specified 'value', and the number of bytes decoded to
        // produce the value into the specified 'accumNumBytesConsumed'.
};

                     // ==================================
                     // customization point checksumAppend
                     // ==================================

template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, bool value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, char value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, signed char value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, unsigned char value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, int value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           bsls::Types::Int64  value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, unsigned int value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           bsls::Types::Uint64  value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, float value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum, double value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM&      checksum,
                           const bdldfp::Decimal64& value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           const bsl::string&  value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           const bdlt::Date&   value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           const bdlt::DateTz& value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM&   checksum,
                           const bdlt::Datetime& value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM&     checksum,
                           const bdlt::DatetimeTz& value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           const bdlt::Time&   value);
template <class CHECKSUM_ALGORITHM>
static void checksumAppend(CHECKSUM_ALGORITHM& checksum,
                           const bdlt::TimeTz& value);
template <class CHECKSUM_ALGORITHM, class VALUE_1, class VALUE_2>
static void checksumAppend(CHECKSUM_ALGORITHM&                     checksum,
                           const bdlb::Variant2<VALUE_1, VALUE_2>& value);
    // Pass the specified 'value' into the specified 'checksum', which combines
    // the value into the internal state of the algorithm.  The internal state
    // of the algorithm is used to produce the resulting checksum value.

                            // ===================
                            // struct TestDataUtil
                            // ===================

struct TestDataUtil {
    // This utility 'struct' is a namespace for a set of static,
    // constant-initialized data used in the implementation of some cases of
    // this test driver.

    // CLASS DATA
    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION = 35500
    };

    static const unsigned char s_RANDOM_GARBAGE_1K[1024];
        // 1024 bytes sampled from '/dev/urandom'.

    static const char s_RANDOM_LOREM_IPSUM[];
        // 5 paragraphs of randomly-generated "lorem ipsum" placeholder text.
};

// ===========================================================================
//                INLINE DEFINITIONS FOR ENTITIES FOR TESTING
// ===========================================================================

                               // ---------------
                               // struct TestUtil
                               // ---------------

// CLASS METHODS
template <class TYPE>
bool TestUtil::bytesDecodeToValue(bsl::ostream&  log,
                                  const char    *buffer,
                                  bsl::size_t    bufferLength,
                                  const TYPE&    value,
                                  int            numBytesConsumed)
{
    bdlsb::FixedMemInStreamBuf valueInStreamBuf(buffer, bufferLength);

    TYPE valueOut;
    int  actualBytesConsumed = 0;

    int rc =
        Util::getValue(&valueInStreamBuf, &valueOut, &actualBytesConsumed);
    PRINT_LINE_UNLESS_EQ(log, 0, rc);
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    PRINT_LINE_UNLESS_EQ(log, actualBytesConsumed, numBytesConsumed);
    if (actualBytesConsumed != numBytesConsumed) {
        return false;                                                 // RETURN
    }

    PRINT_LINE_UNLESS_EQ(log, value, valueOut);
    if (value != valueOut) {
        return false;                                                 // RETURN
    }

    return true;
}

template <class TYPE>
bool TestUtil::valueEncodesToBytes(
                       bsl::ostream&                     log,
                       bsl::streambuf                   *bytes,
                       const TYPE&                       value,
                       const balber::BerEncoderOptions&  options,
                       const char                       *hexadecimalExpression,
                       bsl::size_t                       numBytesConsumed)
{
    bdlsb::MemOutStreamBuf valueOutStreamBuf;

    int rc = Util::putValue(&valueOutStreamBuf, value, &options);
    PRINT_LINE_UNLESS_EQ(log, 0, rc);
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    rc = compareBuffers(valueOutStreamBuf.data(), hexadecimalExpression);
    PRINT_LINE_UNLESS_EQ(log, 0, rc);
    if (0 != rc) {
        if (veryVerbose) {
            log << "ACTUAL: ";
            printBuffer(
                log, valueOutStreamBuf.data(), valueOutStreamBuf.length());
        }

        return false;                                                 // RETURN
    }

    PRINT_LINE_UNLESS_EQ(log, numBytesConsumed, valueOutStreamBuf.length());
    if (numBytesConsumed != valueOutStreamBuf.length()) {
        return false;                                                 // RETURN
    }

    bsl::copy(valueOutStreamBuf.data(),
              valueOutStreamBuf.data() + valueOutStreamBuf.length(),
              bsl::ostreambuf_iterator<char>(bytes));

    return true;
}

                             // ------------------
                             // class Case27Tester
                             // ------------------

// ACCESSORS
template <class SIMPLE_TYPE>
void Case27Tester::operator()(int LINE1, const SIMPLE_TYPE& VALUE) const
{
    static const struct {
        int  d_line;
        int  d_precision;   // datetime fractional second precision
        int  d_conformance; // bde version conformance
        bool d_binaryFlag;  // encode date and time types as binary
    } DATA[] = {
        { L_, 3,     0, false },
        { L_, 3,     0, true  },
        { L_, 3, 34400, false },
        { L_, 3, 34400, true  },
        { L_, 6,     0, false },
        { L_, 6,     0, true  },
        { L_, 6, 34400, false },
        { L_, 6, 34400, true  }
    };

    static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

    for (int i = 0; i != NUM_DATA; ++i) {
        const int LINE2 = DATA[i].d_line;
        const int PRECISION = DATA[i].d_precision;
        const int CONFORMANCE = DATA[i].d_conformance;
        const bool BINARY_FLAG = DATA[i].d_binaryFlag;

        bdlsb::MemOutStreamBuf outStreamBuf;

        balber::BerEncoderOptions options;
        options.bdeVersionConformance() = CONFORMANCE;
        options.setDatetimeFractionalSecondPrecision(PRECISION);
        options.setEncodeDateAndTimeTypesAsBinary(BINARY_FLAG);

        int rc = Util::putValue(&outStreamBuf, VALUE, &options);
        LOOP2_ASSERT_EQ(LINE1, LINE2, 0, rc);
        if (0 != rc) continue;

        bsl::size_t inStreamBufLength = 0;

        do {
            LOOP2_ASSERT(
                LINE1, LINE2, inStreamBufLength <= outStreamBuf.length());
            bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                   inStreamBufLength);

            SIMPLE_TYPE inValue;
            int         accumNumBytesConsumed = 0;

            rc =
                Util::getValue(&inStreamBuf, &inValue, &accumNumBytesConsumed);

            if (0 == rc) {
                LOOP2_ASSERT_EQ(LINE1, LINE2, VALUE, inValue);
                if (VALUE != inValue) continue;

                LOOP2_ASSERT_EQ(LINE1,
                                LINE2,
                                static_cast<bsl::size_t>(accumNumBytesConsumed),
                                inStreamBufLength);
                if (static_cast<bsl::size_t>(accumNumBytesConsumed) !=
                    inStreamBufLength) {
                    continue;
                }
            }
            else {
                ++inStreamBufLength;
            }
        } while (0 != rc);

        LOOP2_ASSERT_EQ(LINE1, LINE2, inStreamBufLength, outStreamBuf.length());

    }
}

                            // ---------------------
                            // struct ByteBufferUtil
                            // ---------------------

// CLASS METHODS
int ByteBufferUtil::loadBuffer(int        *numBytesWritten,
                               char       *buffer,
                               int         bufferSize,
                               const char *expression,
                               int         expressionSize)
{
    enum MachineStateId {
        e_NEEDS_HIGH_NIBBLE,
        e_NEEDS_LOW_NIBBLE
    };

    MachineStateId state           = e_NEEDS_HIGH_NIBBLE;
    unsigned char  stateHighNibble = 0;

    *numBytesWritten = 0;
    const char *const bufferEnd = buffer + bufferSize;

    for (const char *eIt  = expression;
                     eIt != expression + expressionSize;
                   ++eIt) {
        const char character = *eIt;

        if (' ' == character) {
            continue;                                               // CONTINUE
        }

        if (buffer == bufferEnd) {
            return -1;                                                // RETURN
        }

        unsigned char currentNibble;
        if ('A' <= character && character <= 'Z') {
            currentNibble = static_cast<unsigned char>('\x0A') +
                            (static_cast<unsigned char>(character) -
                             static_cast<unsigned char>('A'));
        }
        else if ('0' <= character && character <= '9') {
            currentNibble = static_cast<unsigned char>('\x00') +
                            (static_cast<unsigned char>(character) -
                             static_cast<unsigned char>('0'));
        }
        else {
            return -1;                                                // RETURN
        }

        switch (state) {
          case e_NEEDS_HIGH_NIBBLE: {
              stateHighNibble = static_cast<unsigned char>(currentNibble << 4);
              state           = e_NEEDS_LOW_NIBBLE;
          } break;
          case e_NEEDS_LOW_NIBBLE: {
            const char byte =
                static_cast<char>(stateHighNibble | currentNibble);

            *buffer++ = byte;
            ++*numBytesWritten;
            state = e_NEEDS_HIGH_NIBBLE;
          } break;
        }
    }

    if (e_NEEDS_HIGH_NIBBLE != state) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                          // -------------------------
                          // class RandomInputIterator
                          // -------------------------

// PRIVATE CLASS METHODS
unsigned char RandomInputIterator::generateValue(int *seed)
{
    ///Implementation Note
    ///-------------------
    // The following function implements a 15-bit linear congruential generator
    // based on the implementation of 'bdlb::Random::generate15' from BDE
    // 3.44.0.  This function implements its own LCG and not use
    // 'bdlb::Random::generate15' to ensure that randomly generated data in
    // this test driver does not change if 'bdlb::Random' were to change
    // behavior.  It is modified from its original form to fit the API
    // requirement of yielding 'unsigned char' values, rather than 'int'
    // values.

    unsigned int unsignedSeed = *seed;
    unsignedSeed *= 1103515245;
    unsignedSeed += 12345;

    *seed = unsignedSeed;

    unsigned char hiByte = (unsignedSeed >> 24) & 0x7F;
    unsigned char loByte = (unsignedSeed >> 16) & 0xFF;

    return hiByte ^ loByte;
}

// CLASS METHODS
bool RandomInputIterator::areEqual(const RandomInputIterator& lhs,
                                   const RandomInputIterator& rhs)
{
    BSLS_ASSERT(lhs.d_seed  == rhs.d_seed  ? lhs.d_value == rhs.d_value
                                           : true);
    BSLS_ASSERT(lhs.d_value != rhs.d_value ? lhs.d_seed != rhs.d_seed
                                           : true);
    return lhs.d_seed == rhs.d_seed;
}

// CREATORS
RandomInputIterator::RandomInputIterator()
: d_seed(0)
, d_value(generateValue(&d_seed))
{
}

RandomInputIterator::RandomInputIterator(int seed)
: d_seed(seed)
, d_value(generateValue(&d_seed))
{
}

RandomInputIterator::RandomInputIterator(const RandomInputIterator& original)
: d_seed(original.d_seed)
, d_value(original.d_value)
{
}

// MANIPULATORS
RandomInputIterator& RandomInputIterator::operator=(
                                           const RandomInputIterator& original)
{
    d_seed = original.d_seed;
    d_value = original.d_value;

    return *this;
}

RandomInputIterator& RandomInputIterator::operator++()
{
    d_value = generateValue(&d_seed);

    return *this;
}

RandomInputIterator RandomInputIterator::operator++(int)
{
    RandomInputIterator copy = *this;

    this->operator++();

    return copy;
}

// ACCESSORS
const unsigned char& RandomInputIterator::operator*() const
{
    return d_value;
}

const unsigned char *RandomInputIterator::operator->() const
{
    return &d_value;
}

                        // ----------------------------
                        // class BasicRandomValueLoader
                        // ----------------------------

// CREATORS
template <class INPUT_ITERATOR>
BasicRandomValueLoader<INPUT_ITERATOR>::BasicRandomValueLoader()
: d_iterator()
{
}

template <class INPUT_ITERATOR>
BasicRandomValueLoader<INPUT_ITERATOR>::BasicRandomValueLoader(
                                                    InputIteratorType iterator)
: d_iterator(iterator)
{
}

template <class INPUT_ITERATOR>
BasicRandomValueLoader<INPUT_ITERATOR>::BasicRandomValueLoader(
                                        const BasicRandomValueLoader& original)
: d_iterator(original.d_iterator)
{
}

// MANIPULATORS
template <class INPUT_ITERATOR>
BasicRandomValueLoader<INPUT_ITERATOR>&
BasicRandomValueLoader<INPUT_ITERATOR>::operator=(
                                        const BasicRandomValueLoader& original)
{
    d_iterator = original.d_iterator;

    return *this;
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(bool *value)
{
    *value = static_cast<bool>(*d_iterator++ % 2);
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(char *value)
{
    *value = static_cast<char>(*d_iterator++);
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(unsigned char *value)
{
    *value = *d_iterator++;
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(signed char *value)
{
    *value = static_cast<signed char>(*d_iterator++);
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(unsigned *value)
{
    BSLMF_ASSERT(4 == sizeof(unsigned));

    const unsigned int byte0 =
            static_cast<unsigned int>(*d_iterator++) << (8 * 0);
    const unsigned int byte1 =
            static_cast<unsigned int>(*d_iterator++) << (8 * 1);
    const unsigned int byte2 =
            static_cast<unsigned int>(*d_iterator++) << (8 * 2);
    const unsigned int byte3 =
            static_cast<unsigned int>(*d_iterator++) << (8 * 3);

    *value = byte0 + byte1 + byte2 + byte3;
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(int *value)
{
    BSLMF_ASSERT(4 == sizeof(int));

    unsigned unsignedValue;
    this->operator()(&unsignedValue);

    *value = static_cast<int>(unsignedValue);
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(
                                                     unsigned long long *value)
{
    BSLMF_ASSERT(8 == sizeof(unsigned long long));

    const unsigned long long byte0 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 0);
    const unsigned long long byte1 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 1);
    const unsigned long long byte2 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 2);
    const unsigned long long byte3 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 3);
    const unsigned long long byte4 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 4);
    const unsigned long long byte5 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 5);
    const unsigned long long byte6 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 6);
    const unsigned long long byte7 =
            static_cast<unsigned long long>(*d_iterator++) << (8 * 7);

    *value = byte0 + byte1 + byte2 + byte3 + byte4 + byte5 + byte6 + byte7;
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(long long *value)
{
    BSLMF_ASSERT(8 == sizeof(long long));

    unsigned long long unsignedValue;
    this->operator()(&unsignedValue);

    *value = static_cast<long long>(unsignedValue);
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(float *value)
{
    enum ValueCategories {
        e_NEGATIVE_INFINITY,
        e_NEGATIVE_ZERO,
        e_POSITIVE_ZERO,
        e_POSITIVE_INFINITY,
        e_SNAN,
        e_QNAN,
        e_SUBNORMAL,
        e_NORMAL,

        k_NUM_VALUE_CATEGORIES
    };

    switch (*d_iterator++ % k_NUM_VALUE_CATEGORIES) {
      case e_NEGATIVE_INFINITY: {
          *value = -bsl::numeric_limits<float>::infinity();
          BSLS_ASSERT(bdlb::Float::isInfinite(*value));
          BSLS_ASSERT(1 == bdlb::Float::signBit(*value));
      } break;
      case e_NEGATIVE_ZERO: {
          *value = 0.f * -1.f;
          BSLS_ASSERT(bdlb::Float::isZero(*value));
          BSLS_ASSERT(1 == bdlb::Float::signBit(*value));
      } break;
      case e_POSITIVE_ZERO: {
          *value = 0.f;
          BSLS_ASSERT(bdlb::Float::isZero(*value));
          BSLS_ASSERT(0 == bdlb::Float::signBit(*value));
      } break;
      case e_POSITIVE_INFINITY: {
          *value = bsl::numeric_limits<float>::infinity();
          BSLS_ASSERT(bdlb::Float::isInfinite(*value));
          BSLS_ASSERT(0 == bdlb::Float::signBit(*value));
      } break;
      case e_SNAN: {
          BSLMF_ASSERT(bsl::numeric_limits<float>::has_signaling_NaN);
          *value = bsl::numeric_limits<float>::signaling_NaN();
          // Some platforms do not support signaling NaN values and may
          // implicitly convert such values to non-signaling NaN values.
          //BSLS_ASSERT(bdlb::Float::isSignalingNan(*value));
      } break;
      case e_QNAN: {
          BSLMF_ASSERT(bsl::numeric_limits<float>::has_quiet_NaN);
          *value = bsl::numeric_limits<float>::quiet_NaN();
          BSLS_ASSERT(bdlb::Float::isQuietNan(*value));
      } break;
      case e_SUBNORMAL: {
          static const float minSubnormal = 1.4012984E-45f;

          int mantissa;
          this->operator()(&mantissa);
          mantissa %= 1 << 22;

          *value = static_cast<float>(mantissa) * minSubnormal;
          BSLS_ASSERT(bdlb::Float::isSubnormal(*value));
      } break;
      case e_NORMAL: {
          int mantissa;
          this->operator()(&mantissa);
          mantissa %= 1 << 23;

          int mantissaExponent;
          float floatMantissa = bsl::frexp(static_cast<float>(mantissa),
                                           &mantissaExponent);
          BSLS_ASSERT(floatMantissa >= 0
                      ?  0.5f <= floatMantissa
                      : -0.5f >= floatMantissa);
          BSLS_ASSERT(floatMantissa >= 0
                      ?  1.0f >= floatMantissa
                      : -1.0f <= floatMantissa);

          unsigned char exponent;
          this->operator()(&exponent);

          const int integerExponent = static_cast<int>(exponent % 253) - 125;
          BSLS_ASSERT(-125 <= integerExponent);
          BSLS_ASSERT( 127 >= integerExponent);

          *value = bsl::ldexp(floatMantissa, integerExponent);

          BSLS_ASSERT(bdlb::Float::isNormal(*value));
      } break;
    }
}

template <class INPUT_ITERATOR>
void BasicRandomValueLoader<INPUT_ITERATOR>::operator()(double *value)
{
    enum ValueCategories {
        e_NEGATIVE_INFINITY,
        e_NEGATIVE_ZERO,
        e_POSITIVE_ZERO,
        e_POSITIVE_INFINITY,
        e_SNAN,
        e_QNAN,
        e_SUBNORMAL,
        e_NORMAL,

        k_NUM_VALUE_CATEGORIES
    };

    switch (*d_iterator++ % k_NUM_VALUE_CATEGORIES) {
      case e_NEGATIVE_INFINITY: {
          *value = -bsl::numeric_limits<double>::infinity();
          BSLS_ASSERT(bdlb::Float::isInfinite(*value));
          BSLS_ASSERT(1 == bdlb::Float::signBit(*value));
      } break;
      case e_NEGATIVE_ZERO: {
          *value = 0.0 * -1.0;
          BSLS_ASSERT(bdlb::Float::isZero(*value));
          BSLS_ASSERT(1 == bdlb::Float::signBit(*value));
      } break;
      case e_POSITIVE_ZERO: {
          *value = 0.0;
          BSLS_ASSERT(bdlb::Float::isZero(*value));
          BSLS_ASSERT(0 == bdlb::Float::signBit(*value));
      } break;
      case e_POSITIVE_INFINITY: {
          *value = bsl::numeric_limits<double>::infinity();
          BSLS_ASSERT(bdlb::Float::isInfinite(*value));
          BSLS_ASSERT(0 == bdlb::Float::signBit(*value));
      } break;
      case e_SNAN: {
          BSLMF_ASSERT(bsl::numeric_limits<double>::has_signaling_NaN);
          *value = bsl::numeric_limits<double>::signaling_NaN();
          // Some platforms do not support signaling NaN values and may
          // implicitly convert such values to non-signaling NaN values.
          //BSLS_ASSERT(bdlb::Float::isSignalingNan(*value));
      } break;
      case e_QNAN: {
          BSLMF_ASSERT(bsl::numeric_limits<double>::has_quiet_NaN);
          *value = bsl::numeric_limits<double>::quiet_NaN();
          BSLS_ASSERT(bdlb::Float::isQuietNan(*value));
      } break;
      case e_SUBNORMAL: {
          static const double minSubnormal = 4.950656458412E-324;

          bsls::Types::Int64 mantissa;
          this->operator()(&mantissa);
          mantissa %= 1ll << 52;

          *value = static_cast<double>(mantissa) * minSubnormal;
          BSLS_ASSERT(bdlb::Float::isSubnormal(*value));
      } break;
      case e_NORMAL: {
          bsls::Types::Int64 mantissa;
          this->operator()(&mantissa);
          mantissa %= 1ll << 53;

          int mantissaExponent;
          double doubleMantissa = bsl::frexp(static_cast<double>(mantissa),
                                             &mantissaExponent);
          BSLS_ASSERT(doubleMantissa >= 0
                      ?  0.5 <= doubleMantissa
                      : -0.5 >= doubleMantissa);
          BSLS_ASSERT(doubleMantissa >= 0
                      ?  1.0 >= doubleMantissa
                      : -1.0 <= doubleMantissa);

          unsigned int exponent;
          this->operator()(&exponent);

          const int integerExponent = static_cast<int>(exponent % 2045) - 1021;
          BSLS_ASSERT(-1022 <= integerExponent);
          BSLS_ASSERT( 1023 >= integerExponent);

          *value = bsl::ldexp(doubleMantissa, integerExponent);

          BSLS_ASSERT(bdlb::Float::isNormal(*value));
      } break;
    }
}

                       // ------------------------------
                       // namespace RandomValueFunctions
                       // ------------------------------

namespace RandomValueFunctions {

// FREE FUNCTIONS
template <class VALUE_TYPE, class LOADER>
void loadRandomValue(VALUE_TYPE *value, LOADER& loader)
{
    return loader(value);
}

}  // close RandomValueFunctions namespace

                           // ----------------------
                           // struct RandomValueUtil
                           // ----------------------

// CLASS METHODS
template <class VALUE_TYPE, class LOADER>
void RandomValueUtil::load(VALUE_TYPE *value, LOADER& loader)
{
    using namespace RandomValueFunctions;
    loadRandomValue(value, loader);
}

template <class VALUE_TYPE, class LOADER>
VALUE_TYPE RandomValueUtil::generate(LOADER& loader)
{
    VALUE_TYPE result;
    load(&result, loader);
    return result;
}

template <class INTEGRAL_TYPE, class LOADER>
INTEGRAL_TYPE RandomValueUtil::generateModulo(LOADER& loader,
                                              INTEGRAL_TYPE base)
{
    BSLS_ASSERT(base >= 0);
    return generate<INTEGRAL_TYPE>(loader) % base;
}

template <class INTEGRAL_TYPE, class LOADER>
INTEGRAL_TYPE RandomValueUtil::generateInInterval(LOADER& loader,
                                                  INTEGRAL_TYPE minimum,
                                                  INTEGRAL_TYPE maximum)
{
    BSLS_ASSERT(maximum >= minimum);

    const INTEGRAL_TYPE offset = generateModulo(loader, maximum - minimum + 1);
    return offset >= 0 ? minimum + offset : maximum + offset;
}

                     // -----------------------------------
                     // customization point loadRandomValue
                     // -----------------------------------

template <class LOADER>
void loadRandomValue(bdldfp::Decimal64 *value, LOADER& loader)
{
    static const bsls::Types::Int64 minSignificand = -9999999999999999ll;
    static const bsls::Types::Int64 maxSignificand =  9999999999999999ll;
    static const int minExponent = -398;
    static const int maxExponent = 369;

    const bsls::Types::Int64 significand = RandomValueUtil::generateInInterval(
                                                               loader         ,
                                                               minSignificand ,
                                                               maxSignificand);
    const int exponent = RandomValueUtil::generateInInterval(loader,
                                                             minExponent,
                                                             maxExponent);

    *value = bdldfp::DecimalUtil::makeDecimalRaw64(significand, exponent);
}

template <class LOADER>
void loadRandomValue(bsl::string *value, LOADER& loader)
{
    static const char *k_WORDS[] = {
        "lorem",
        "ipsum",
        "dolor",
        "sit",
        "amet",
        "consectiteur",
    };

    enum {
        k_NUM_WORDS = sizeof k_WORDS / sizeof k_WORDS[0],
        k_MAX_WORDS = 10
    };

    bdlsb::MemOutStreamBuf valueStreamBuf;
    bsl::ostream           valueStream(&valueStreamBuf);

    const int numWords =
        RandomValueUtil::generateInInterval<int>(loader, 0, k_MAX_WORDS);

    for (int i = 0; i != numWords; ++i) {
        if (0 != i) {
            valueStream << " ";
        }

        const int wordIdx = RandomValueUtil::generateInInterval<int>(
            loader, 0, k_NUM_WORDS - 1);

        const char *word = k_WORDS[wordIdx];

        valueStream << bslstl::StringRef(word);
    }

    *value = bslstl::StringRef(valueStreamBuf.data(),
                               valueStreamBuf.length());
}

template <class LOADER>
void loadRandomValue(bdlt::Date *value, LOADER& loader)
{
    enum {
        k_MAX_SERIAL_DATE = 3652059
    };

    const int serialDate = RandomValueUtil::generateInInterval(
        loader, 1, static_cast<int>(k_MAX_SERIAL_DATE));

    BSLS_ASSERT(bdlt::ProlepticDateImpUtil::isValidSerial(serialDate));

    int year;
    int month;
    int day;
    bdlt::ProlepticDateImpUtil::serialToYmd(&year, &month, &day, serialDate);

    value->setYearMonthDay(year, month, day);
}

template <class LOADER>
void loadRandomValue(bdlt::DateTz *value, LOADER& loader)
{
    enum {
        k_MIN_TIMEZONE_OFFSET = -1439,
        k_MAX_TIMEZONE_OFFSET =  1439
    };

    const int offset = RandomValueUtil::generateInInterval(
        loader,
        static_cast<int>(k_MIN_TIMEZONE_OFFSET),
        static_cast<int>(k_MAX_TIMEZONE_OFFSET));

    bdlt::Date localDate;
    RandomValueUtil::load(&localDate, loader);

    BSLS_ASSERT(bdlt::DateTz::isValid(localDate, offset));

    value->setDateTz(localDate, offset);
}

template <class LOADER>
void loadRandomValue(bdlt::Datetime *value, LOADER& loader)
{
    bdlt::Date date;
    RandomValueUtil::load(&date, loader);

    bdlt::Time time;
    RandomValueUtil::load(&time, loader);

    BSLS_ASSERT(bdlt::Datetime::isValid(date.year(),
                                        date.month(),
                                        date.day(),
                                        time.hour(),
                                        time.minute(),
                                        time.second(),
                                        time.millisecond(),
                                        time.microsecond()));

    value->setDatetime(date, time);
}

template <class LOADER>
void loadRandomValue(bdlt::DatetimeTz *value, LOADER& loader)
{
    enum {
        k_MIN_TIMEZONE_OFFSET = -1439,
        k_MAX_TIMEZONE_OFFSET =  1439
    };

    const int offset = RandomValueUtil::generateInInterval(
        loader,
        static_cast<int>(k_MIN_TIMEZONE_OFFSET),
        static_cast<int>(k_MAX_TIMEZONE_OFFSET));

    bdlt::Datetime localDatetime;
    RandomValueUtil::load(&localDatetime, loader);

    BSLS_ASSERT(bdlt::DatetimeTz::isValid(localDatetime, offset));

    value->setDatetimeTz(localDatetime, offset);
}

template <class LOADER>
void loadRandomValue(bdlt::Time *value, LOADER& loader)
{
    typedef bdlt::TimeUnitRatio Ratio;

    const bsls::Types::Uint64 numMicroseconds =
        RandomValueUtil::generateInInterval<bsls::Types::Uint64>(
            loader, 0, Ratio::k_MICROSECONDS_PER_DAY);

    const unsigned hour = static_cast<unsigned>(
        numMicroseconds /
        static_cast<bsls::Types::Uint64>(Ratio::k_MICROSECONDS_PER_HOUR));

    const unsigned hourRemainder = static_cast<unsigned>(
        numMicroseconds %
        static_cast<bsls::Types::Uint64>(Ratio::k_MICROSECONDS_PER_HOUR));

    const unsigned minute = hourRemainder / Ratio::k_MICROSECONDS_PER_MINUTE;
    const unsigned minuteRemainder =
        hourRemainder % Ratio::k_MICROSECONDS_PER_MINUTE;

    const unsigned second = minuteRemainder / Ratio::k_MICROSECONDS_PER_SECOND;
    const unsigned secondRemainder =
        minuteRemainder % Ratio::k_MICROSECONDS_PER_SECOND;

    const unsigned millisecond =
        secondRemainder / Ratio::k_MICROSECONDS_PER_MILLISECOND;
    const unsigned millisecondRemainder =
        secondRemainder % Ratio::k_MICROSECONDS_PER_MILLISECOND;

    const unsigned microsecond = millisecondRemainder;

    BSLS_ASSERT(
           bdlt::Time::isValid(hour, minute, second, millisecond, microsecond));

    value->setTime(hour, minute, second, millisecond, microsecond);
}

template <class LOADER>
void loadRandomValue(bdlt::TimeTz *value, LOADER& loader)
{
    enum {
        k_MIN_TIMEZONE_OFFSET = -1439,
        k_MAX_TIMEZONE_OFFSET =  1439
    };

    const int offset = RandomValueUtil::generateInInterval(
        loader,
        static_cast<int>(k_MIN_TIMEZONE_OFFSET),
        static_cast<int>(k_MAX_TIMEZONE_OFFSET));

    bdlt::Time time;
    RandomValueUtil::load(&time, loader);

    BSLS_ASSERT(bdlt::TimeTz::isValid(time, offset));

    value->setTimeTz(time, offset);
}

template <class LOADER, class TYPE, class TYPETZ>
void loadRandomValue(bdlb::Variant2<TYPE, TYPETZ> *value, LOADER& loader)
{
    enum {
        e_TYPE   = 0,
        e_TYPETZ = 1
    };

    switch (RandomValueUtil::generate<bool>(loader)) {
      case e_TYPE: {
          value->template createInPlace<TYPE>();
          RandomValueUtil::load(&value->template the<TYPE>(),
                                loader);
      } break;
      case e_TYPETZ: {
          value->template createInPlace<TYPETZ>();
          RandomValueUtil::load(&value->template the<TYPETZ>(),
                                loader);
      } break;
    }
}

                            // --------------------
                            // struct ByteArrayUtil
                            // --------------------

// CLASS METHODS
void ByteArrayUtil::setTheUintAt(unsigned char *begin,
                                 unsigned char *end,
                                 bsl::size_t    index,
                                 unsigned int   value)
{
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(static_cast<bsl::size_t>((end - begin) /
                                         sizeof(unsigned int)) > index);
    static_cast<void>(end);

    begin += index * sizeof(unsigned int);

    *begin++ = static_cast<unsigned char>((value >> (8 * 0)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 1)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 2)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 3)) & 0xFF);
}

void ByteArrayUtil::setTheUint64At(unsigned char       *begin,
                                   unsigned char       *end,
                                   bsl::size_t          index,
                                   bsls::Types::Uint64  value)
{
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(static_cast<bsl::size_t>((end - begin) /
                                         sizeof(bsls::Types::Uint64)) > index);
    static_cast<void>(end);

    begin += index * sizeof(unsigned int);

    *begin++ = static_cast<unsigned char>((value >> (8 * 0)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 1)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 2)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 3)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 4)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 5)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 6)) & 0xFF);
    *begin++ = static_cast<unsigned char>((value >> (8 * 7)) & 0xFF);
}

unsigned int ByteArrayUtil::theUintAt(const unsigned char *begin,
                                      const unsigned char *end,
                                      bsl::size_t          index)
{
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(static_cast<bsl::size_t>((end - begin) /
                                         sizeof(unsigned int)) > index);
    static_cast<void>(end);

    begin += index * sizeof(unsigned int);

    const unsigned int byte0 = static_cast<unsigned char>(*begin++);
    const unsigned int byte1 = static_cast<unsigned char>(*begin++);
    const unsigned int byte2 = static_cast<unsigned char>(*begin++);
    const unsigned int byte3 = static_cast<unsigned char>(*begin++);

    return (byte0 << (8 * 0)) + (byte1 << (8 * 1)) + (byte2 << (8 * 2)) +
           (byte3 << (8 * 3));
}

                            // --------------------
                            // class Md5Fingerprint
                            // --------------------

// CREATORS
Md5Fingerprint::Md5Fingerprint()
: d_value()
{
}

Md5Fingerprint::Md5Fingerprint(const Md5Fingerprint& original)
: d_value()
{
    bsl::copy(original.d_value,
              original.d_value + sizeof(original.d_value),
              d_value);
}

// MANIPULATORS
Md5Fingerprint& Md5Fingerprint::operator=(const Md5Fingerprint& original)
{
    bsl::copy(original.d_value,
              original.d_value + sizeof(original.d_value),
              d_value);

    return *this;
}

unsigned char& Md5Fingerprint::operator[](bsl::size_t index)
{
    BSLS_ASSERT(k_SIZE > index);

    return d_value[index];
}

void Md5Fingerprint::setTheUintAt(bsl::size_t index, unsigned int value)
{
    ByteArrayUtil::setTheUintAt(d_value, d_value + k_SIZE, index, value);
}

unsigned char *Md5Fingerprint::data()
{
    return d_value;
}

unsigned char *Md5Fingerprint::begin()
{
    return d_value;
}

unsigned char *Md5Fingerprint::end()
{
    return d_value + k_SIZE;
}

// ACCESSORS
const unsigned char& Md5Fingerprint::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(k_SIZE > index);

    return d_value[index];
}

unsigned int Md5Fingerprint::theUintAt(bsl::size_t index) const
{
    return ByteArrayUtil::theUintAt(d_value, d_value + k_SIZE, index);
}

bsl::size_t Md5Fingerprint::size() const
{
    return k_SIZE;
}

const unsigned char *Md5Fingerprint::begin() const
{
    return d_value;
}

const unsigned char *Md5Fingerprint::end() const
{
    return d_value + k_SIZE;
}

bsl::ostream& Md5Fingerprint::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bsl::string leadingWhitespace(level * spacesPerLevel, ' ');
    stream << leadingWhitespace;

    static const char nibbleCharacters[16] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f'
    };

    for (const unsigned char *it = d_value; it != d_value + k_SIZE; ++it) {
        const unsigned char value = *it;
        const unsigned char hiNibble = (value >> 4) & 0x0F;
        const unsigned char loNibble = (value >> 0) & 0x0F;

        const char hiNibbleChar = nibbleCharacters[hiNibble];
        const char loNibbleChar = nibbleCharacters[loNibble];

        stream << hiNibbleChar << loNibbleChar;
    }

    return stream;
}

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const Md5Fingerprint& object)
{
    return object.print(stream);
}

                          // -------------------------
                          // struct Md5FingerprintUtil
                          // -------------------------

// CLASS METHODS
Md5Fingerprint Md5FingerprintUtil::getSeedValue()
{
    Md5Fingerprint result;

    result.setTheUintAt(0, k_A_SEED_VALUE);
    result.setTheUintAt(1, k_B_SEED_VALUE);
    result.setTheUintAt(2, k_C_SEED_VALUE);
    result.setTheUintAt(3, k_D_SEED_VALUE);

    return result;
}

                               // --------------
                               // class Md5Block
                               // --------------

// CREATORS
Md5Block::Md5Block()
: d_bytes()
, d_numBytes(0)
{
}

template <class INPUT_ITERATOR>
Md5Block::Md5Block(INPUT_ITERATOR begin, INPUT_ITERATOR end)
: d_bytes()
, d_numBytes(static_cast<unsigned char>(end - begin))
{
    if (begin == end) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(begin != end);

    for (unsigned char i = 0; begin != end && i != k_CAPACITY; ++i, ++begin) {
        d_bytes[i] = *begin;
    }

    BSLS_ASSERT(begin == end);
}

Md5Block::Md5Block(const Md5Block& original)
: d_bytes()
, d_numBytes(original.d_numBytes)
{
    BSLS_ASSERT(0 <= original.d_numBytes);
    BSLS_ASSERT(k_CAPACITY >= original.d_numBytes);

    bsl::copy(original.d_bytes,
              original.d_bytes + original.d_numBytes,
              d_bytes);
}

// MANIPULATORS
Md5Block& Md5Block::operator=(const Md5Block& original)
{
    BSLS_ASSERT(0 <= original.d_numBytes);
    BSLS_ASSERT(k_CAPACITY >= original.d_numBytes);

    bsl::copy(original.d_bytes,
              original.d_bytes + original.d_numBytes,
              d_bytes);

    d_numBytes = original.d_numBytes;

    return *this;
}

unsigned char &Md5Block::operator[](bsl::size_t index)
{
    BSLS_ASSERT(d_numBytes > index);

    return d_bytes[index];
}

void Md5Block::setTheUintAt(bsl::size_t index, unsigned int value)
{
    ByteArrayUtil::setTheUintAt(d_bytes, d_bytes + d_numBytes, index, value);
}

unsigned char *Md5Block::begin()
{
    return d_bytes;
}

unsigned char *Md5Block::end()
{
    return d_bytes + d_numBytes;
}

void Md5Block::clear()
{
    bsl::fill(d_bytes, d_bytes + d_numBytes, 0);
    d_numBytes = 0;
}

void Md5Block::resize(bsl::size_t newSize)
{
    BSLS_ASSERT(k_CAPACITY >= newSize);

    unsigned char *const clearBegin =
        d_bytes + bsl::min(static_cast<bsl::size_t>(d_numBytes), newSize);
    unsigned char *const clearEnd =
        d_bytes + bsl::max(static_cast<bsl::size_t>(d_numBytes), newSize);

    bsl::fill(clearBegin, clearEnd, 0);

    d_numBytes = static_cast<unsigned char>(newSize);
}

// ACCESSORS
const unsigned char& Md5Block::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(d_numBytes > index);

    return d_bytes[index];
}

unsigned int Md5Block::theUintAt(bsl::size_t index) const
{
    return ByteArrayUtil::theUintAt(d_bytes, d_bytes + d_numBytes, index);
}

const unsigned char *Md5Block::begin() const
{
    return d_bytes;
}

const unsigned char *Md5Block::end() const
{
    return d_bytes + d_numBytes;
}

bool Md5Block::full() const
{
    return k_CAPACITY == d_numBytes;
}

bool Md5Block::empty() const
{
    return 0 == d_numBytes;
}

bsl::size_t Md5Block::size() const
{
    return d_numBytes;
}

// FREE FUNCTIONS
bool operator==(const Md5Block& lhs, const Md5Block& rhs)
{
    return lhs.size() == rhs.size() &&
           bsl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool operator!=(const Md5Block& lhs, const Md5Block& rhs)
{
    return lhs.size() != rhs.size() ||
           !bsl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

                             // -------------------
                             // struct Md5BlockUtil
                             // -------------------

// CLASS METHODS
void Md5BlockUtil::appendUint64(Md5Block *block, bsls::Types::Uint64 value)
{
    BSLS_ASSERT(block->size() <=
                Md5Block::k_CAPACITY - sizeof(bsls::Types::Uint64));

    block->resize(block->size() + sizeof(bsls::Types::Uint64));

    unsigned char *const begin = block->end() - sizeof(bsls::Types::Uint64);
    unsigned char *const end   = block->end();

    ByteArrayUtil::setTheUint64At(begin, end, 0, value);
}

void Md5BlockUtil::concatenate(Md5Block *first, Md5Block *second)
{
    const bsl::size_t totalSize = first->size() + second->size();

    const bsl::size_t newFirstSize =
        bsl::min(totalSize, static_cast<bsl::size_t>(Md5Block::k_CAPACITY));
    const bsl::size_t newSecondSize = totalSize - newFirstSize;

    const bsl::size_t sliceIndex =
        bsl::min(second->size(), Md5Block::k_CAPACITY - first->size());

    BSLS_ASSERT_SAFE(Md5Block::k_CAPACITY >= newFirstSize);
    BSLS_ASSERT_SAFE(Md5Block::k_CAPACITY != newFirstSize
                         ? 0 == newSecondSize
                         : true);
    BSLS_ASSERT_SAFE(newFirstSize + newSecondSize == totalSize);

    BSLS_ASSERT_SAFE(newFirstSize >= first->size());
    const bsl::size_t oldFirstSize = first->size();
    first->resize(newFirstSize);
    unsigned char *const firstInsertionPoint = first->begin() + oldFirstSize;

    BSLS_ASSERT_SAFE(Md5Block::k_CAPACITY >= oldFirstSize + sliceIndex);
    bsl::copy(
        second->begin(), second->begin() + sliceIndex, firstInsertionPoint);

    bsl::rotate(second->begin(), second->begin() + sliceIndex, second->end());
    BSLS_ASSERT_SAFE(newSecondSize <= second->size());
    second->resize(newSecondSize);
}

                         // ---------------------------
                         // class Md5BlockInputIterator
                         // ---------------------------

// CLASS METHODS
bool Md5BlockInputIterator::areEqual(const Md5BlockInputIterator& lhs,
                                     const Md5BlockInputIterator& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_iterator_p == rhs.d_iterator_p &&
                             lhs.d_end_p == rhs.d_end_p
                         ? lhs.d_block == rhs.d_block
                         : true);
    BSLS_ASSERT_SAFE(lhs.d_block != rhs.d_block
                         ? lhs.d_iterator_p != rhs.d_iterator_p ||
                               lhs.d_end_p != rhs.d_end_p
                         : true);

    return lhs.d_iterator_p == rhs.d_iterator_p && lhs.d_end_p == rhs.d_end_p;
}

// CREATORS
Md5BlockInputIterator::Md5BlockInputIterator()
: d_block()
, d_iterator_p(0)
, d_end_p(0)
{
}

Md5BlockInputIterator::Md5BlockInputIterator(const unsigned char *begin,
                                             const unsigned char *end)
: d_block()
, d_iterator_p(begin)
, d_end_p(end)
{
    if (begin < end) {
        const bsl::size_t firstBlockSize =
            bsl::min(static_cast<bsl::size_t>(Md5Block::k_CAPACITY),
                     static_cast<bsl::size_t>(end - begin));

        BSLS_ASSERT_SAFE(Md5Block::k_CAPACITY >= firstBlockSize);

        d_block.resize(firstBlockSize);
        bsl::copy(begin, begin + firstBlockSize, d_block.begin());
    }
}

Md5BlockInputIterator::Md5BlockInputIterator(
                                         const Md5BlockInputIterator& original)
: d_block(original.d_block)
, d_iterator_p(original.d_iterator_p)
, d_end_p(original.d_end_p)
{
}

// MANIPULATORS
Md5BlockInputIterator& Md5BlockInputIterator::operator=(
                                         const Md5BlockInputIterator& original)
{
    d_block      = original.d_block;
    d_iterator_p = original.d_iterator_p;
    d_end_p      = original.d_end_p;

    return *this;
}

Md5BlockInputIterator& Md5BlockInputIterator::operator++()
{
    d_iterator_p += Md5Block::k_CAPACITY;

    if (d_iterator_p < d_end_p) {
        const bsl::size_t blockSize =
            bsl::min(static_cast<bsl::size_t>(Md5Block::k_CAPACITY),
                     static_cast<bsl::size_t>(d_end_p - d_iterator_p));

        BSLS_ASSERT_SAFE(Md5Block::k_CAPACITY >= blockSize);

        d_block.resize(blockSize);
        bsl::copy(d_iterator_p, d_iterator_p + blockSize, d_block.begin());
    }
    else {
        d_block.clear();
    }

    return *this;
}

Md5BlockInputIterator Md5BlockInputIterator::operator++(int)
{
    Md5BlockInputIterator result = *this;
    this->operator++();
    return result;
}

// ACCESSORS
const Md5Block& Md5BlockInputIterator::operator*() const
{
    return d_block;
}

const Md5Block *Md5BlockInputIterator::operator->() const
{
    return &d_block;
}

// FREE FUNCTIONS
bool operator==(const Md5BlockInputIterator& lhs,
                const Md5BlockInputIterator& rhs)
{
    return Md5BlockInputIterator::areEqual(lhs, rhs);
}

bool operator!=(const Md5BlockInputIterator& lhs,
                const Md5BlockInputIterator& rhs)
{
    return !Md5BlockInputIterator::areEqual(lhs, rhs);
}

                          // ------------------------
                          // class Md5BlockInputRange
                          // ------------------------

// CREATORS
Md5BlockInputRange::Md5BlockInputRange(const unsigned char *begin,
                                       const unsigned char *end)
: d_begin(begin, end)
, d_end(end + (Md5Block::k_CAPACITY - ((end - begin) % Md5Block::k_CAPACITY)),
        end)
{
    BSLS_ASSERT(begin <= end);
}

// ACCESSORS
Md5BlockInputIterator Md5BlockInputRange::begin() const
{
    return d_begin;
}

Md5BlockInputIterator Md5BlockInputRange::end() const
{
    return d_end;
}

                               // --------------
                               // class Md5State
                               // --------------

// CREATORS
Md5State::Md5State()
: d_fingerprint()
, d_block()
, d_numBlocksConsumed(0)
{
}

Md5State::Md5State(const Md5Fingerprint& fingerprint)
: d_fingerprint(fingerprint)
, d_block()
, d_numBlocksConsumed(0)
{
}

Md5State::Md5State(const Md5Fingerprint& fingerprint, const Md5Block& block)
: d_fingerprint(fingerprint)
, d_block(block)
, d_numBlocksConsumed(0)
{
}

Md5State::Md5State(const Md5Fingerprint& fingerprint,
                   const Md5Block&       block,
                   bsls::Types::Uint64   numBlocksConsumed)
: d_fingerprint(fingerprint)
, d_block(block)
, d_numBlocksConsumed(numBlocksConsumed)
{
}

Md5State::Md5State(const Md5State& original)
: d_fingerprint(original.d_fingerprint)
, d_block(original.d_block)
, d_numBlocksConsumed(original.d_numBlocksConsumed)
{
}

// MANIPULATORS
Md5State& Md5State::operator=(const Md5State& original)
{
    d_fingerprint       = original.d_fingerprint;
    d_block             = original.d_block;
    d_numBlocksConsumed = original.d_numBlocksConsumed;

    return *this;
}

void Md5State::setFingerprint(const Md5Fingerprint& value)
{
    d_fingerprint = value;
}

void Md5State::setBlock(const Md5Block& value)
{
    d_block = value;
}

void Md5State::setNumBlocksConsumed(bsls::Types::Uint64 value)
{
    d_numBlocksConsumed = value;
}

// ACCESSORS
const Md5Fingerprint& Md5State::fingerprint() const
{
    return d_fingerprint;
}

const Md5Block& Md5State::block() const
{
    return d_block;
}

bsls::Types::Uint64 Md5State::numBlocksConsumed() const
{
    return d_numBlocksConsumed;
}

                             // -------------------
                             // struct Md5StateUtil
                             // -------------------

// CLASS METHODS
Md5State Md5StateUtil::getSeedValue()
{
    Md5State result;
    loadSeedValue(&result);
    return result;
}

void Md5StateUtil::loadSeedValue(Md5State *value)
{
    value->setFingerprint(Md5FingerprintUtil::getSeedValue());
    value->setBlock(Md5Block());
    value->setNumBlocksConsumed(0);
}

void Md5StateUtil::append(Md5State *state, const Md5Block& block)
{
    Md5Block block0 = state->block();
    Md5Block block1 = block;
    Md5BlockUtil::concatenate(&block0, &block1);

    if (!block0.full()) {
        BSLS_ASSERT(block1.empty());

        state->setBlock(block0);
        return;                                                       // RETURN
    }

    BSLS_ASSERT(block0.full());
    state->setFingerprint(digest(state->fingerprint(), block0));
    state->setBlock(block1);
    state->setNumBlocksConsumed(state->numBlocksConsumed() + 1);
}

void Md5StateUtil::appendPaddingAndLength(Md5State *state)
{
    enum {
        k_PADDED_SIZE = 56
    };

    const bsl::size_t paddingSize =
            state->block().size() < k_PADDED_SIZE
          ? k_PADDED_SIZE - state->block().size()
          : k_PADDED_SIZE + Md5Block::k_CAPACITY - state->block().size();

    BSLS_ASSERT(0 != paddingSize);
    BSLS_ASSERT(Md5Block::k_CAPACITY >= paddingSize);

    static const unsigned char paddingBuffer[Md5Block::k_CAPACITY] = {0x80};
    const unsigned char *const beginPadding = paddingBuffer;
    const unsigned char *const endPadding   = paddingBuffer + paddingSize;

    Md5Block paddedBlock0 = state->block();
    Md5Block paddedBlock1(beginPadding, endPadding);
    Md5BlockUtil::concatenate(&paddedBlock0, &paddedBlock1);

    Md5Fingerprint       fingerprint        = state->fingerprint();
    Md5Block            *finalBlockPtr      = &paddedBlock0;
    bsls::Types::Uint64  numBlocksToConsume = 1;

    if (!paddedBlock1.empty()) {
        BSLS_ASSERT(Md5Block::k_CAPACITY == paddedBlock0.size());
        BSLS_ASSERT(k_PADDED_SIZE == paddedBlock1.size());

        digest(&fingerprint, paddedBlock0);
        finalBlockPtr      = &paddedBlock1;
        numBlocksToConsume = 2;
    }

    BSLS_ASSERT(k_PADDED_SIZE == finalBlockPtr->size());

    const bsls::Types::Uint64 numBitsConsumed =
            8 * (state->numBlocksConsumed() * Md5Block::k_CAPACITY +
                 state->block().size());
    Md5BlockUtil::appendUint64(finalBlockPtr, numBitsConsumed);

    BSLS_ASSERT(Md5Block::k_CAPACITY == finalBlockPtr->size());

    digest(&fingerprint, *finalBlockPtr);

    state->setFingerprint(fingerprint);
    state->setBlock(Md5Block());
    state->setNumBlocksConsumed(state->numBlocksConsumed() +
                                numBlocksToConsume);
}

void Md5StateUtil::digest(Md5Fingerprint *fingerprint, const Md5Block& block)
{
    BSLS_ASSERT(block.full());

    unsigned int a = fingerprint->theUintAt(0);
    unsigned int b = fingerprint->theUintAt(1);
    unsigned int c = fingerprint->theUintAt(2);
    unsigned int d = fingerprint->theUintAt(3);

    // Create a concise alias for 'block'.
    const Md5Block& x = block;

    // Save 'a' as 'aa', 'b' as 'bb', 'c' as 'cc', and 'd' as 'dd'.
    unsigned int aa = a;
    unsigned int bb = b;
    unsigned int cc = c;
    unsigned int dd = d;

    // Create a local alias to the implementation utility.
    typedef Md5StateUtil_ImplUtil ImplUtil;

    // Create local aliases for the tables used to provide constants
    // to the bit manipulation operations.
    const unsigned int (&i)[4][16] = ImplUtil::k_I_TABLE;
    const unsigned int (&k)[4][16] = ImplUtil::k_K_TABLE;
    const unsigned int (&s)[4][16] = ImplUtil::k_S_TABLE;

    // Round 1: Do the following 16 operations,
    ImplUtil::round1Op(&a, &b, &c, &d, x, i[0][ 0], k[0][ 0], s[0][ 0]);
    ImplUtil::round1Op(&d, &a, &b, &c, x, i[0][ 1], k[0][ 1], s[0][ 1]);
    ImplUtil::round1Op(&c, &d, &a, &b, x, i[0][ 2], k[0][ 2], s[0][ 2]);
    ImplUtil::round1Op(&b, &c, &d, &a, x, i[0][ 3], k[0][ 3], s[0][ 3]);

    ImplUtil::round1Op(&a, &b, &c, &d, x, i[0][ 4], k[0][ 4], s[0][ 4]);
    ImplUtil::round1Op(&d, &a, &b, &c, x, i[0][ 5], k[0][ 5], s[0][ 5]);
    ImplUtil::round1Op(&c, &d, &a, &b, x, i[0][ 6], k[0][ 6], s[0][ 6]);
    ImplUtil::round1Op(&b, &c, &d, &a, x, i[0][ 7], k[0][ 7], s[0][ 7]);

    ImplUtil::round1Op(&a, &b, &c, &d, x, i[0][ 8], k[0][ 8], s[0][ 8]);
    ImplUtil::round1Op(&d, &a, &b, &c, x, i[0][ 9], k[0][ 9], s[0][ 9]);
    ImplUtil::round1Op(&c, &d, &a, &b, x, i[0][10], k[0][10], s[0][10]);
    ImplUtil::round1Op(&b, &c, &d, &a, x, i[0][11], k[0][11], s[0][11]);

    ImplUtil::round1Op(&a, &b, &c, &d, x, i[0][12], k[0][12], s[0][12]);
    ImplUtil::round1Op(&d, &a, &b, &c, x, i[0][13], k[0][13], s[0][13]);
    ImplUtil::round1Op(&c, &d, &a, &b, x, i[0][14], k[0][14], s[0][14]);
    ImplUtil::round1Op(&b, &c, &d, &a, x, i[0][15], k[0][15], s[0][15]);

    // Round 2: Do the following 16 operations,
    ImplUtil::round2Op(&a, &b, &c, &d, x, i[1][ 0], k[1][ 0], s[1][ 0]);
    ImplUtil::round2Op(&d, &a, &b, &c, x, i[1][ 1], k[1][ 1], s[1][ 1]);
    ImplUtil::round2Op(&c, &d, &a, &b, x, i[1][ 2], k[1][ 2], s[1][ 2]);
    ImplUtil::round2Op(&b, &c, &d, &a, x, i[1][ 3], k[1][ 3], s[1][ 3]);

    ImplUtil::round2Op(&a, &b, &c, &d, x, i[1][ 4], k[1][ 4], s[1][ 4]);
    ImplUtil::round2Op(&d, &a, &b, &c, x, i[1][ 5], k[1][ 5], s[1][ 5]);
    ImplUtil::round2Op(&c, &d, &a, &b, x, i[1][ 6], k[1][ 6], s[1][ 6]);
    ImplUtil::round2Op(&b, &c, &d, &a, x, i[1][ 7], k[1][ 7], s[1][ 7]);

    ImplUtil::round2Op(&a, &b, &c, &d, x, i[1][ 8], k[1][ 8], s[1][ 8]);
    ImplUtil::round2Op(&d, &a, &b, &c, x, i[1][ 9], k[1][ 9], s[1][ 9]);
    ImplUtil::round2Op(&c, &d, &a, &b, x, i[1][10], k[1][10], s[1][10]);
    ImplUtil::round2Op(&b, &c, &d, &a, x, i[1][11], k[1][11], s[1][11]);

    ImplUtil::round2Op(&a, &b, &c, &d, x, i[1][12], k[1][12], s[1][12]);
    ImplUtil::round2Op(&d, &a, &b, &c, x, i[1][13], k[1][13], s[1][13]);
    ImplUtil::round2Op(&c, &d, &a, &b, x, i[1][14], k[1][14], s[1][14]);
    ImplUtil::round2Op(&b, &c, &d, &a, x, i[1][15], k[1][15], s[1][15]);

    // Round 3: Do the following 16 operations,
    ImplUtil::round3Op(&a, &b, &c, &d, x, i[2][ 0], k[2][ 0], s[2][ 0]);
    ImplUtil::round3Op(&d, &a, &b, &c, x, i[2][ 1], k[2][ 1], s[2][ 1]);
    ImplUtil::round3Op(&c, &d, &a, &b, x, i[2][ 2], k[2][ 2], s[2][ 2]);
    ImplUtil::round3Op(&b, &c, &d, &a, x, i[2][ 3], k[2][ 3], s[2][ 3]);

    ImplUtil::round3Op(&a, &b, &c, &d, x, i[2][ 4], k[2][ 4], s[2][ 4]);
    ImplUtil::round3Op(&d, &a, &b, &c, x, i[2][ 5], k[2][ 5], s[2][ 5]);
    ImplUtil::round3Op(&c, &d, &a, &b, x, i[2][ 6], k[2][ 6], s[2][ 6]);
    ImplUtil::round3Op(&b, &c, &d, &a, x, i[2][ 7], k[2][ 7], s[2][ 7]);

    ImplUtil::round3Op(&a, &b, &c, &d, x, i[2][ 8], k[2][ 8], s[2][ 8]);
    ImplUtil::round3Op(&d, &a, &b, &c, x, i[2][ 9], k[2][ 9], s[2][ 9]);
    ImplUtil::round3Op(&c, &d, &a, &b, x, i[2][10], k[2][10], s[2][10]);
    ImplUtil::round3Op(&b, &c, &d, &a, x, i[2][11], k[2][11], s[2][11]);

    ImplUtil::round3Op(&a, &b, &c, &d, x, i[2][12], k[2][12], s[2][12]);
    ImplUtil::round3Op(&d, &a, &b, &c, x, i[2][13], k[2][13], s[2][13]);
    ImplUtil::round3Op(&c, &d, &a, &b, x, i[2][14], k[2][14], s[2][14]);
    ImplUtil::round3Op(&b, &c, &d, &a, x, i[2][15], k[2][15], s[2][15]);

    // Round 4: Do the following 16 operations,
    ImplUtil::round4Op(&a, &b, &c, &d, x, i[3][ 0], k[3][ 0], s[3][ 0]);
    ImplUtil::round4Op(&d, &a, &b, &c, x, i[3][ 1], k[3][ 1], s[3][ 1]);
    ImplUtil::round4Op(&c, &d, &a, &b, x, i[3][ 2], k[3][ 2], s[3][ 2]);
    ImplUtil::round4Op(&b, &c, &d, &a, x, i[3][ 3], k[3][ 3], s[3][ 3]);

    ImplUtil::round4Op(&a, &b, &c, &d, x, i[3][ 4], k[3][ 4], s[3][ 4]);
    ImplUtil::round4Op(&d, &a, &b, &c, x, i[3][ 5], k[3][ 5], s[3][ 5]);
    ImplUtil::round4Op(&c, &d, &a, &b, x, i[3][ 6], k[3][ 6], s[3][ 6]);
    ImplUtil::round4Op(&b, &c, &d, &a, x, i[3][ 7], k[3][ 7], s[3][ 7]);

    ImplUtil::round4Op(&a, &b, &c, &d, x, i[3][ 8], k[3][ 8], s[3][ 8]);
    ImplUtil::round4Op(&d, &a, &b, &c, x, i[3][ 9], k[3][ 9], s[3][ 9]);
    ImplUtil::round4Op(&c, &d, &a, &b, x, i[3][10], k[3][10], s[3][10]);
    ImplUtil::round4Op(&b, &c, &d, &a, x, i[3][11], k[3][11], s[3][11]);

    ImplUtil::round4Op(&a, &b, &c, &d, x, i[3][12], k[3][12], s[3][12]);
    ImplUtil::round4Op(&d, &a, &b, &c, x, i[3][13], k[3][13], s[3][13]);
    ImplUtil::round4Op(&c, &d, &a, &b, x, i[3][14], k[3][14], s[3][14]);
    ImplUtil::round4Op(&b, &c, &d, &a, x, i[3][15], k[3][15], s[3][15]);

    fingerprint->setTheUintAt(0, a + aa);
    fingerprint->setTheUintAt(1, b + bb);
    fingerprint->setTheUintAt(2, c + cc);
    fingerprint->setTheUintAt(3, d + dd);
}

Md5Fingerprint Md5StateUtil::digest(const Md5State& state)
{
    return digest(state.fingerprint(), state.block());
}

Md5Fingerprint Md5StateUtil::digest(const Md5Fingerprint& fingerprint,
                                    const Md5Block&       block)
{
    Md5Fingerprint result = fingerprint;
    digest(&result, block);
    return result;
}

                        // ----------------------------
                        // struct Md5StateUtil_ImplUtil
                        // ----------------------------

// CLASS DATA
const unsigned int Md5StateUtil_ImplUtil::k_T_TABLE[64] = {
    // Round 1
    0xD76AA478, //  1
    0xE8C7B756, //  2
    0x242070DB, //  3
    0xC1BDCEEE, //  4
    0xF57C0FAF, //  5
    0x4787C62A, //  6
    0xA8304613, //  7
    0xFD469501, //  8
    0x698098D8, //  9
    0x8B44F7AF, // 10
    0xFFFF5BB1, // 11
    0x895CD7BE, // 12
    0x6B901122, // 13
    0xFD987193, // 14
    0xA679438E, // 15
    0x49B40821, // 16

    // Round 2
    0xF61E2562, // 17
    0xC040B340, // 18
    0x265E5A51, // 19
    0xE9B6C7AA, // 20
    0xD62F105D, // 21
    0x02441453, // 22
    0xD8a1E681, // 23
    0xE7D3FBC8, // 24
    0x21E1CDE6, // 25
    0xC33707D6, // 26
    0xF4D50D87, // 27
    0x455A14ED, // 28
    0xA9E3E905, // 29
    0xFCEFA3F8, // 30
    0x676F02D9, // 31
    0x8D2A4C8A, // 32

    // Round 3
    0xFFFA3942, // 33
    0x8771F681, // 34
    0x6D9D6122, // 35
    0xFDE5380C, // 36
    0xA4BEEA44, // 37
    0x4BDECFA9, // 38
    0xF6BB4B60, // 39
    0xBEBFBC70, // 40
    0x289B7EC6, // 41
    0xEAA127FA, // 42
    0xD4EF3085, // 43
    0x04881D05, // 44
    0xD9D4D039, // 45
    0xE6DB99E5, // 46
    0x1FA27CF8, // 47
    0xC4AC5665, // 48

    // Round 4
    0xF4292244, // 49
    0x432AFF97, // 50
    0xAB9423A7, // 51
    0xFC93A039, // 52
    0x655B59C3, // 53
    0x8F0CCC92, // 54
    0xFFEFF47D, // 55
    0x85845DD1, // 56
    0x6FA87E4F, // 57
    0xFE2CE6E0, // 58
    0xA3014314, // 59
    0x4E0811A1, // 60
    0xF7537E82, // 61
    0xBD3AF235, // 62
    0x2AD7D2BB, // 63
    0xEB86D391  // 64
};

const unsigned int Md5StateUtil_ImplUtil::k_I_TABLE[4][16] = {
    {
        // Round 1
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15
    },
    {
        // Round 2
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31
    },
    {
        // Round 3
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47
    },
    {
        // Round 4
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63
    }
};

const unsigned int Md5StateUtil_ImplUtil::k_K_TABLE[4][16] = {
    {
        // Round 1
        0,
        1,
        2,
        3,

        4,
        5,
        6,
        7,

        8,
        9,
        10,
        11,

        12,
        13,
        14,
        15
    },
    {
        // Round 2
        1,
        6,
        11,
        0,

        5,
        10,
        15,
        4,

        9,
        14,
        3,
        8,

        13,
        2,
        7,
        12
    },
    {
        // Round 3
        5,
        8,
        11,
        14,

        1,
        4,
        7,
        10,

        13,
        0,
        3,
        6,

        9,
        12,
        15,
        2
    },
    {
        // Round 4
        0,
        7,
        14,
        5,

        12,
        3,
        10,
        1,

        8,
        15,
        6,
        13,

        4,
        11,
        2,
        9
    },
};

const unsigned int Md5StateUtil_ImplUtil::k_S_TABLE[4][16] = {
    {
        // Round 1
        7,
        12,
        17,
        22,

        7,
        12,
        17,
        22,

        7,
        12,
        17,
        22,

        7,
        12,
        17,
        22
    },
    {
        // Round 2
        5,
        9,
        14,
        20,

        5,
        9,
        14,
        20,

        5,
        9,
        14,
        20,

        5,
        9,
        14,
        20
    },
    {
        // Round 3
        4,
        11,
        16,
        23,

        4,
        11,
        16,
        23,

        4,
        11,
        16,
        23,

        4,
        11,
        16,
        23
    },
    {
        // Round 4
        6,
        10,
        15,
        21,

        6,
        10,
        15,
        21,

        6,
        10,
        15,
        21,

        6,
        10,
        15,
        21
    },
};

// PRIVATE CLASS METHODS
unsigned int Md5StateUtil_ImplUtil::f(unsigned int x,
                                      unsigned int y,
                                      unsigned int z)
{
    return (x & y) | (~x & z);
}

unsigned int Md5StateUtil_ImplUtil::g(unsigned int x,
                                      unsigned int y,
                                      unsigned int z)
{
    return (x & z) | (y & ~z);
}

unsigned int Md5StateUtil_ImplUtil::h(unsigned int x,
                                      unsigned int y,
                                      unsigned int z)
{
    return (x ^ y) ^ z;
}

unsigned int Md5StateUtil_ImplUtil::i(unsigned int x,
                                      unsigned int y,
                                      unsigned int z)
{
    return y ^ (x | ~z);
}

inline
unsigned int Md5StateUtil_ImplUtil::rotateLeft(unsigned int value,
                                               unsigned int numBits)
{
    BSLMF_ASSERT(4 == sizeof(unsigned int));

    return (value << (numBits % 32)) | (value >> (32 - (numBits % 32)));
}

// CLASS METHODS
void Md5StateUtil_ImplUtil::round1Op(unsigned int    *aPtr,
                                     unsigned int    *bPtr,
                                     unsigned int    *cPtr,
                                     unsigned int    *dPtr,
                                     const Md5Block&  x,
                                     unsigned int     i,
                                     unsigned int     k,
                                     unsigned int     s)
{
    unsigned int& a = *aPtr;
    unsigned int& b = *bPtr;
    unsigned int& c = *cPtr;
    unsigned int& d = *dPtr;

    const unsigned int (&t)[64] = k_T_TABLE;

    a = b + rotateLeft(a + f(b,c,d) + x.theUintAt(k) + t[i], s);
}

void Md5StateUtil_ImplUtil::round2Op(unsigned int    *aPtr,
                                     unsigned int    *bPtr,
                                     unsigned int    *cPtr,
                                     unsigned int    *dPtr,
                                     const Md5Block&  x,
                                     unsigned int     i,
                                     unsigned int     k,
                                     unsigned int     s)
{
    unsigned int& a = *aPtr;
    unsigned int& b = *bPtr;
    unsigned int& c = *cPtr;
    unsigned int& d = *dPtr;

    const unsigned int (&t)[64] = k_T_TABLE;

    a = b + rotateLeft(a + g(b,c,d) + x.theUintAt(k) + t[i], s);
}

void Md5StateUtil_ImplUtil::round3Op(unsigned int    *aPtr,
                                     unsigned int    *bPtr,
                                     unsigned int    *cPtr,
                                     unsigned int    *dPtr,
                                     const Md5Block&  x,
                                     unsigned int     i,
                                     unsigned int     k,
                                     unsigned int     s)
{
    unsigned int& a = *aPtr;
    unsigned int& b = *bPtr;
    unsigned int& c = *cPtr;
    unsigned int& d = *dPtr;

    const unsigned int (&t)[64] = k_T_TABLE;

    a = b + rotateLeft(a + h(b,c,d) + x.theUintAt(k) + t[i], s);
}

void Md5StateUtil_ImplUtil::round4Op(unsigned int    *aPtr,
                                     unsigned int    *bPtr,
                                     unsigned int    *cPtr,
                                     unsigned int    *dPtr,
                                     const Md5Block&  x,
                                     unsigned int     iIdx,
                                     unsigned int     k,
                                     unsigned int     s)
{
    unsigned int& a = *aPtr;
    unsigned int& b = *bPtr;
    unsigned int& c = *cPtr;
    unsigned int& d = *dPtr;

    const unsigned int (&t)[64] = k_T_TABLE;

    a = b + rotateLeft(a + i(b,c,d) + x.theUintAt(k) + t[iIdx], s);
}

                               // --------------
                               // struct Md5Util
                               // --------------

// CLASS METHODS
Md5Fingerprint Md5Util::getFingerprint(const unsigned char *begin,
                                       const unsigned char *end)
{
    const Md5BlockInputRange range(begin, end);

    Md5State state;
    Md5StateUtil::loadSeedValue(&state);

    for (Md5BlockInputIterator it  = range.begin();
                               it != range.end();
                             ++it) {
        Md5StateUtil::append(&state, *it);
    }

    Md5StateUtil::appendPaddingAndLength(&state);

    return state.fingerprint();
}

                         // --------------------------
                         // class Md5ChecksumAlgorithm
                         // --------------------------


// CREATORS
Md5ChecksumAlgorithm::Md5ChecksumAlgorithm()
: d_state(Md5StateUtil::getSeedValue())
{
}

// MANIPULATORS
void Md5ChecksumAlgorithm::operator()(const void *data, bsl::size_t numBytes)
{
    const Md5BlockInputRange range(static_cast<const unsigned char *>(data),
                                   static_cast<const unsigned char *>(data) +
                                       numBytes);

    for (Md5BlockInputIterator it  = range.begin();
                               it != range.end();
                             ++it) {
        Md5StateUtil::append(&d_state, *it);
    }
}

Md5ChecksumAlgorithm::result_type Md5ChecksumAlgorithm::computeChecksum()
{
    Md5StateUtil::appendPaddingAndLength(&d_state);
    return d_state.fingerprint();
}

                            // ---------------------
                            // class FingerprintHash
                            // ---------------------

// ACCESSORS
template <class CHECKSUM_ALGORITHM>
template <class TYPE>
typename Checksum<CHECKSUM_ALGORITHM>::result_type
Checksum<CHECKSUM_ALGORITHM>::operator()(const TYPE& object) const
{
    CHECKSUM_ALGORITHM algorithm;

    checksumAppend(algorithm, object);

    return static_cast<result_type>(algorithm.computeChecksum());
}

                             // -------------------
                             // struct ChecksumUtil
                             // -------------------

// CLASS METHODS
template <class OBJECT_TYPE>
Md5Fingerprint ChecksumUtil::getMd5(const OBJECT_TYPE& object)
{
    Checksum<Md5ChecksumAlgorithm> hash;
    return hash(object);
}

                          // -------------------------
                          // class PutValueFingerprint
                          // -------------------------

// CREATORS
PutValueFingerprint::PutValueFingerprint()
: d_seed(0)
, d_numSamples(0)
, d_fractionalSecondPrecision(0)
, d_encodeDateAndTimeTypesAsBinary(false)
{
}

// MANIPULATORS
void PutValueFingerprint::setSeed(int value)
{
    d_seed = value;
}

void PutValueFingerprint::setNumSamples(int value)
{
    d_numSamples = value;
}

void PutValueFingerprint::setFractionalSecondPrecision(int value)
{
    d_fractionalSecondPrecision = value;
}

void PutValueFingerprint::setEncodeDateAndTimeTypesAsBinary(bool value)
{
    d_encodeDateAndTimeTypesAsBinary = value;
}

// ACCESSORS
int PutValueFingerprint::seed() const
{
    return d_seed;
}

int PutValueFingerprint::numSamples() const
{
    return d_numSamples;
}

int PutValueFingerprint::fractionalSecondPrecision() const
{
    return d_fractionalSecondPrecision;
}

bool PutValueFingerprint::encodeDateAndTimeTypesAsBinary() const
{
    return d_encodeDateAndTimeTypesAsBinary;
}

// FREE FUNCTIONS
template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM&        checksumAlg,
                    const PutValueFingerprint& object)
{
    balber::BerEncoderOptions encoderOptions;
    encoderOptions.setDatetimeFractionalSecondPrecision(
        object.fractionalSecondPrecision());
    encoderOptions.setEncodeDateAndTimeTypesAsBinary(
        object.encodeDateAndTimeTypesAsBinary());

    enum SupportedTypes {
        e_BOOL,
        e_CHAR,
        e_SIGNED_CHAR,
        e_UNSIGNED_CHAR,
        e_INT,
        e_INT64,
        e_UINT,
        e_UINT64,
        e_FLOAT,
        e_DOUBLE,
        e_DECIMAL64,
        e_STRING,
        e_DATE,
        e_DATETZ,
        e_DATETIME,
        e_DATETIMETZ,
        e_TIME,
        e_TIMETZ,

        k_NUM_SUPPORTED_TYPES
    };

    for (int i = 0; i != object.numSamples(); ++i) {
        const int sampleSeed = object.seed() + i;

        typedef BasicRandomValueLoader<RandomInputIterator> RandomValueLoader;
        RandomInputIterator randomIt(sampleSeed);
        RandomValueLoader   randomValueLoader(randomIt);

        typedef PutValueFingerprint_ImplUtil ImplUtil;
        bdlsb::MemOutStreamBuf               streamBuf;

        switch (RandomValueUtil::generateInInterval<int>(
            randomValueLoader, 0, k_NUM_SUPPORTED_TYPES - 1)) {
          case e_BOOL: {
            ImplUtil::putRandomValue<bool>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_CHAR: {
            ImplUtil::putRandomValue<char>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_SIGNED_CHAR: {
            ImplUtil::putRandomValue<signed char>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_UNSIGNED_CHAR: {
            ImplUtil::putRandomValue<unsigned char>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_INT: {
            ImplUtil::putRandomValue<int>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_INT64: {
            ImplUtil::putRandomValue<bsls::Types::Int64>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_UINT: {
            ImplUtil::putRandomValue<unsigned int>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_UINT64: {
            ImplUtil::putRandomValue<bsls::Types::Uint64>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_FLOAT: {
            ImplUtil::putRandomValue<float>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DOUBLE: {
            ImplUtil::putRandomValue<double>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DECIMAL64: {
            ImplUtil::putRandomValue<bdldfp::Decimal64>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_STRING: {
            ImplUtil::putRandomValue<bsl::string>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DATE: {
            ImplUtil::putRandomValue<bdlt::Date>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DATETZ: {
            ImplUtil::putRandomValue<bdlt::DateTz>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DATETIME: {
            ImplUtil::putRandomValue<bdlt::Datetime>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_DATETIMETZ: {
            ImplUtil::putRandomValue<bdlt::DatetimeTz>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_TIME: {
            ImplUtil::putRandomValue<bdlt::Time>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          case e_TIMETZ: {
            ImplUtil::putRandomValue<bdlt::TimeTz>(
                &streamBuf, randomValueLoader, encoderOptions);
          } break;
          default: {
            BSLS_ASSERT_OPT(!"Unreachable");
            return;                                                   // RETURN
          } break;
        }

        checksumAlg(streamBuf.data(), streamBuf.length());
    }
}

                     // -----------------------------------
                     // struct PutValueFingerprint_ImplUtil
                     // -----------------------------------

template <class VALUE, class LOADER>
void PutValueFingerprint_ImplUtil::putRandomValue(
                                   bsl::streambuf                   *streamBuf,
                                   LOADER&                           loader,
                                   const balber::BerEncoderOptions&  options)
{
    VALUE value;
    RandomValueUtil::load(&value, loader);

    int rc = Util::putValue(streamBuf, value, &options);
    BSLS_ASSERT(0 == rc);
}

                          // -------------------------
                          // class GetValueFingerprint
                          // -------------------------

// CREATORS
GetValueFingerprint::GetValueFingerprint()
: d_seed(0)
, d_numSamples(0)
, d_fractionalSecondPrecision(0)
, d_encodeDateAndTimeTypesAsBinary(false)
{
}

// MANIPULATORS
void GetValueFingerprint::setSeed(int value)
{
    d_seed = value;
}

void GetValueFingerprint::setNumSamples(int value)
{
    d_numSamples = value;
}

void GetValueFingerprint::setFractionalSecondPrecision(int value)
{
    d_fractionalSecondPrecision = value;
}

void GetValueFingerprint::setEncodeDateAndTimeTypesAsBinary(bool value)
{
    d_encodeDateAndTimeTypesAsBinary = value;
}

// ACCESSORS
int GetValueFingerprint::seed() const
{
    return d_seed;
}

int GetValueFingerprint::numSamples() const
{
    return d_numSamples;
}

int GetValueFingerprint::fractionalSecondPrecision() const
{
    return d_fractionalSecondPrecision;
}

bool GetValueFingerprint::encodeDateAndTimeTypesAsBinary() const
{
    return d_encodeDateAndTimeTypesAsBinary;
}

// FREE FUNCTIONS
template <class HASHALG>
void checksumAppend(HASHALG& hashAlg, const GetValueFingerprint& object)
{
    balber::BerEncoderOptions encoderOptions;
    encoderOptions.setDatetimeFractionalSecondPrecision(
        object.fractionalSecondPrecision());
    encoderOptions.setEncodeDateAndTimeTypesAsBinary(
        object.encodeDateAndTimeTypesAsBinary());

    enum SupportedTypes {
        e_BOOL,
        e_CHAR,
        e_SIGNED_CHAR,
        e_UNSIGNED_CHAR,
        e_INT,
        e_INT64,
        e_UINT,
        e_UINT64,
        e_FLOAT,
        e_DOUBLE,
        e_DECIMAL64,
        e_STRING,
        e_DATE,
        e_DATETZ,
        e_DATE_VARIANT,
        e_DATETIME,
        e_DATETIMETZ,
        e_DATETIME_VARIANT,
        e_TIME,
        e_TIMETZ,
        e_TIME_VARIANT,

        k_NUM_SUPPORTED_TYPES
    };

    for (int i = 0; i != object.numSamples(); ++i) {
        const int sampleSeed = object.seed() + i;

        typedef BasicRandomValueLoader<RandomInputIterator> RandomValueLoader;
        RandomInputIterator randomIt(sampleSeed);
        RandomValueLoader   randomValueLoader(randomIt);

        typedef GetValueFingerprint_ImplUtil ImplUtil;

        if (veryVeryVerbose) {
            bsl::cout << "[GetValueFingerprint ";
        }

        switch (RandomValueUtil::generateInInterval<int>(
            randomValueLoader, 0, k_NUM_SUPPORTED_TYPES - 1)) {
          case e_BOOL: {
            bool value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "bool value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_CHAR: {
            char value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "char value: " << static_cast<int>(value)
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_SIGNED_CHAR: {
            signed char value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "signed char value: " << static_cast<int>(value)
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_UNSIGNED_CHAR: {
            unsigned char value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "unsigned char value: " << static_cast<int>(value)
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_INT: {
            int value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "int value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_INT64: {
            bsls::Types::Int64 value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "int64 value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_UINT: {
            unsigned int value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "unsigned int value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_UINT64: {
            bsls::Types::Uint64 value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "unsigned int64 value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_FLOAT: {
            float value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::ostream::fmtflags oldFlags     = bsl::cout.flags();
                bsl::streamsize        oldPrecision = bsl::cout.precision();

                static const int floatMaxDigits10 = 9;

                bsl::cout.flags(bsl::ostream::scientific);
                bsl::cout.precision(floatMaxDigits10);

                bsl::cout << "float value: " << value
                          << " num bytes: " << numBytes;

                bsl::cout.flags(oldFlags);
                bsl::cout.precision(oldPrecision);
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DOUBLE: {
            double value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::ostream::fmtflags oldFlags     = bsl::cout.flags();
                bsl::streamsize        oldPrecision = bsl::cout.precision();

                static const int doubleMaxDigits10 = 17;

                bsl::cout.flags(bsl::ostream::scientific);
                bsl::cout.precision(doubleMaxDigits10);

                bsl::cout << "double value: " << value
                          << " num bytes: " << numBytes;

                bsl::cout.flags(oldFlags);
                bsl::cout.precision(oldPrecision);
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DECIMAL64: {
            bdldfp::Decimal64 value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "decimal64 value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_STRING: {
            bsl::string value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "string value: \"" << value
                          << "\" num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATE: {
            bdlt::Date value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "date value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATETZ: {
            bdlt::DateTz value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "datetz value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATE_VARIANT: {
            bdlb::Variant2<bdlt::Date, bdlt::DateTz> value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose && value.is<bdlt::Date>()) {
                bsl::cout << "date variant date value: "
                          << value.the<bdlt::Date>()
                          << " num bytes: " << numBytes;
            }
            else if (veryVeryVerbose && value.is<bdlt::DateTz>()) {
                bsl::cout << "date variant datetz value: "
                          << value.the<bdlt::DateTz>()
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATETIME: {
            bdlt::Datetime value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "datetime value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATETIMETZ: {
            bdlt::DatetimeTz value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "datetimetz value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_DATETIME_VARIANT: {
            bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose && value.is<bdlt::Datetime>()) {
                bsl::cout << "datetime variant datetime value: "
                          << value.the<bdlt::Datetime>()
                          << " num bytes: " << numBytes;
            }
            else if (veryVeryVerbose && value.is<bdlt::DatetimeTz>()) {
                bsl::cout << "datetime variant datetimetz value: "
                          << value.the<bdlt::DatetimeTz>()
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_TIME: {
            bdlt::Time value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "time value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_TIMETZ: {
            bdlt::TimeTz value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose) {
                bsl::cout << "timetz value: " << value
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          case e_TIME_VARIANT: {
            bdlb::Variant2<bdlt::Time, bdlt::TimeTz> value;
            int numBytes = 0;
            ImplUtil::getRandomValue(
                &value, &numBytes, randomValueLoader, encoderOptions);

            if (veryVeryVerbose && value.is<bdlt::Time>()) {
                bsl::cout << "time variant time value: "
                          << value.the<bdlt::Time>()
                          << " num bytes: " << numBytes;
            }
            else if (veryVeryVerbose && value.is<bdlt::TimeTz>()) {
                bsl::cout << "time variant timetz value: "
                          << value.the<bdlt::TimeTz>()
                          << " num bytes: " << numBytes;
            }

            checksumAppend(hashAlg, value);
            checksumAppend(hashAlg, numBytes);
          } break;
          default: {
            BSLS_ASSERT_OPT(!"Unreachable");
            return;                                                   // RETURN
          } break;
        }

        if (veryVeryVerbose) {
            bsl::cout << "]" << bsl::endl;
        }
    }
}

                     // -----------------------------------
                     // struct GetValueFingerprint_ImplUtil
                     // -----------------------------------

// CLASS METHODS
template <class VALUE, class VALUETZ, class LOADER>
void GetValueFingerprint_ImplUtil::getRandomValue(
                       bdlb::Variant2<VALUE, VALUETZ>   *value,
                       int                              *accumNumBytesConsumed,
                       LOADER&                           loader,
                       const balber::BerEncoderOptions&  options)
{
    bdlsb::MemOutStreamBuf outStreamBuf;

    if (RandomValueUtil::generate<bool>(loader)) {
        VALUE originalValue;
        RandomValueUtil::load(&originalValue, loader);

        int rc = Util::putValue(&outStreamBuf, originalValue, &options);
        BSLS_ASSERT(0 == rc);
    }
    else {
        VALUETZ originalValue;
        RandomValueUtil::load(&originalValue, loader);

        int rc = Util::putValue(&outStreamBuf, originalValue, &options);
        BSLS_ASSERT(0 == rc);
    }

    bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                           outStreamBuf.length());

    int rc = Util::getValue(&inStreamBuf, value, accumNumBytesConsumed);
    BSLS_ASSERT(0 == rc);
}

template <class VALUE, class LOADER>
void GetValueFingerprint_ImplUtil::getRandomValue(
                       VALUE                            *value,
                       int                              *accumNumBytesConsumed,
                       LOADER&                           loader,
                       const balber::BerEncoderOptions&  options)
{
    VALUE originalValue;
    RandomValueUtil::load(&originalValue, loader);

    bdlsb::MemOutStreamBuf outStreamBuf;
    int rc = Util::putValue(&outStreamBuf, originalValue, &options);
    BSLS_ASSERT(0 == rc);

    bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                           outStreamBuf.length());

    rc = Util::getValue(&inStreamBuf, value, accumNumBytesConsumed);
    BSLS_ASSERT(0 == rc);
}

                     // ----------------------------------
                     // customization point checksumAppend
                     // ----------------------------------

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, bool value)
{
    const char normalizedValue = value;
    BSLMF_ASSERT(1 == sizeof(normalizedValue));
    checksum(&normalizedValue, sizeof(normalizedValue));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, char value)
{
    BSLMF_ASSERT(1 == sizeof(value));
    checksum(&value, sizeof(value));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, signed char value)
{
    BSLMF_ASSERT(1 == sizeof(value));
    checksum(&value, sizeof(value));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, unsigned char value)
{
    BSLMF_ASSERT(1 == sizeof(value));
    checksum(&value, sizeof(value));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, int value)
{
    BSLMF_ASSERT(4 == sizeof(value));

    const signed char byte0 = (value >> (0 * 8)) & 0xFF;
    const signed char byte1 = (value >> (1 * 8)) & 0xFF;
    const signed char byte2 = (value >> (2 * 8)) & 0xFF;
    const signed char byte3 = (value >> (3 * 8)) & 0xFF;

    checksumAppend(checksum, byte0);
    checksumAppend(checksum, byte1);
    checksumAppend(checksum, byte2);
    checksumAppend(checksum, byte3);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, bsls::Types::Int64 value)
{
    BSLMF_ASSERT(8 == sizeof(bsls::Types::Int64));

    const signed char byte0 = (value >> (0 * 8)) & 0xFF;
    const signed char byte1 = (value >> (1 * 8)) & 0xFF;
    const signed char byte2 = (value >> (2 * 8)) & 0xFF;
    const signed char byte3 = (value >> (3 * 8)) & 0xFF;
    const signed char byte4 = (value >> (4 * 8)) & 0xFF;
    const signed char byte5 = (value >> (5 * 8)) & 0xFF;
    const signed char byte6 = (value >> (6 * 8)) & 0xFF;
    const signed char byte7 = (value >> (7 * 8)) & 0xFF;

    checksumAppend(checksum, byte0);
    checksumAppend(checksum, byte1);
    checksumAppend(checksum, byte2);
    checksumAppend(checksum, byte3);
    checksumAppend(checksum, byte4);
    checksumAppend(checksum, byte5);
    checksumAppend(checksum, byte6);
    checksumAppend(checksum, byte7);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, unsigned int value)
{
    BSLMF_ASSERT(4 == sizeof(unsigned int));

    const unsigned char byte0 = (value >> (0 * 8)) & 0xFF;
    const unsigned char byte1 = (value >> (1 * 8)) & 0xFF;
    const unsigned char byte2 = (value >> (2 * 8)) & 0xFF;
    const unsigned char byte3 = (value >> (3 * 8)) & 0xFF;

    checksumAppend(checksum, byte0);
    checksumAppend(checksum, byte1);
    checksumAppend(checksum, byte2);
    checksumAppend(checksum, byte3);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, bsls::Types::Uint64 value)
{
    BSLMF_ASSERT(8 == sizeof(bsls::Types::Uint64));

    const unsigned char byte0 = (value >> (0 * 8)) & 0xFF;
    const unsigned char byte1 = (value >> (1 * 8)) & 0xFF;
    const unsigned char byte2 = (value >> (2 * 8)) & 0xFF;
    const unsigned char byte3 = (value >> (3 * 8)) & 0xFF;
    const unsigned char byte4 = (value >> (4 * 8)) & 0xFF;
    const unsigned char byte5 = (value >> (5 * 8)) & 0xFF;
    const unsigned char byte6 = (value >> (6 * 8)) & 0xFF;
    const unsigned char byte7 = (value >> (7 * 8)) & 0xFF;

    checksumAppend(checksum, byte0);
    checksumAppend(checksum, byte1);
    checksumAppend(checksum, byte2);
    checksumAppend(checksum, byte3);
    checksumAppend(checksum, byte4);
    checksumAppend(checksum, byte5);
    checksumAppend(checksum, byte6);
    checksumAppend(checksum, byte7);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, float value)
{
    // Note that the implementation of this operation requires that the
    // platform represents 'float' values using the 32 bit IEEE 754 binary
    // floating point format.

    BSLMF_ASSERT(4 == sizeof(value));


    if (bdlb::Float::isNan(value)) {
        checksumAppend(checksum, 0);
        return;                                                       // RETURN
    }

    unsigned char bytes[sizeof(value)];
    bsl::memcpy(&bytes, &value, sizeof(value));

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsl::reverse(bytes, bytes + sizeof(bytes));
#endif

    checksum(bytes, sizeof(bytes));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, double value)
{
    // Note that the implementation of this operation requires that the
    // platform represents 'float' values using the 32 bit IEEE 754 binary
    // floating point format.

    BSLMF_ASSERT(8 == sizeof(value));

    if (bdlb::Float::isNan(value)) {
        checksumAppend(checksum, 0);
        return;                                                       // RETURN
    }

    unsigned char bytes[sizeof(value)];
    bsl::memcpy(&bytes, &value, sizeof(value));

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsl::reverse(bytes, bytes + sizeof(bytes));
#endif

    checksum(bytes, sizeof(bytes));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM&      checksum,
                    const bdldfp::Decimal64& value)
{
    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    int                 classification;
    switch (bdldfp::DecimalUtil::decompose(
        &sign, &significand, &exponent, value)) {
      case FP_NAN: {
          classification = 0;
      } break;
      case FP_INFINITE: {
          classification = 1;
      } break;
      case FP_SUBNORMAL: {
          classification = 2;
      } break;
      case FP_ZERO: {
          classification = 3;
      } break;
      case FP_NORMAL: {
          classification = 4;
      } break;
      default: {
          BSLS_ASSERT(!"Unreachable");
          return;
      } break;
    }

    checksumAppend(checksum, sign);
    checksumAppend(checksum, significand);
    checksumAppend(checksum, exponent);
    checksumAppend(checksum, classification);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bsl::string& value)
{
    checksum(value.data(), static_cast<int>(value.size()));
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bdlt::Date& value)
{
    int year;
    int month;
    int day;

    value.getYearMonthDay(&year, &month, &day);

    checksumAppend(checksum, year);
    checksumAppend(checksum, month);
    checksumAppend(checksum, day);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bdlt::DateTz& value)
{
    checksumAppend(checksum, value.localDate());
    checksumAppend(checksum, value.offset());
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bdlt::Datetime& value)
{
    checksumAppend(checksum, value.date());
    checksumAppend(checksum, value.time());
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM&     checksum,
                    const bdlt::DatetimeTz& value)
{
    checksumAppend(checksum, value.localDatetime());
    checksumAppend(checksum, value.offset());
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bdlt::Time& value)
{
    int hour;
    int minute;
    int second;
    int millisecond;
    int microsecond;

    value.getTime(&hour, &minute, &second, &millisecond, &microsecond);

    checksumAppend(checksum, hour);
    checksumAppend(checksum, minute);
    checksumAppend(checksum, second);
    checksumAppend(checksum, millisecond);
    checksumAppend(checksum, microsecond);
}

template <class CHECKSUM_ALGORITHM>
void checksumAppend(CHECKSUM_ALGORITHM& checksum, const bdlt::TimeTz& value)
{
    checksumAppend(checksum, value.localTime());
    checksumAppend(checksum, value.offset());
}

template <class CHECKSUM_ALGORITHM, class VALUE_1, class VALUE_2>
void checksumAppend(CHECKSUM_ALGORITHM&                     checksum,
                    const bdlb::Variant2<VALUE_1, VALUE_2>& value)
{
    const int typeIndex = value.typeIndex();
    checksumAppend(checksum, typeIndex);

    switch (typeIndex) {
      case 1: {
          checksumAppend(checksum, value.template the<VALUE_1>());
      } break;
      case 2: {
          checksumAppend(checksum, value.template the<VALUE_2>());
      } break;
      default: {
          BSLS_ASSERT(0 == typeIndex);
      } break;
    }
}

                            // -------------------
                            // struct TestDataUtil
                            // -------------------

// CLASS DATA
#define NL "\n"

const char TestDataUtil::s_RANDOM_LOREM_IPSUM[] =
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc rutrum enim" NL
"ante, ac fermentum augue vulputate vitae. Vestibulum scelerisque, lacus" NL
"nec volutpat posuere, lectus ante congue augue, a consequat elit urna in" NL
"diam.  Vestibulum vestibulum placerat varius. Aliquam viverra blandit" NL
"blandit.  Nullam volutpat orci orci, a euismod ligula tristique vel." NL
"Phasellus sollicitudin interdum luctus. In justo leo, accumsan sed" NL
"aliquet sit amet, porta vel nulla. Vivamus at purus vitae nunc rutrum" NL
"posuere ut eu libero.  Fusce porttitor dui quis orci pretium, vel rutrum" NL
"quam interdum. Suspendisse rutrum lacus nec nisl venenatis, in ornare" NL
"diam molestie. Pellentesque quis odio non massa semper porttitor ac vel" NL
"nulla. Aliquam a blandit mauris. Duis id condimentum arcu, semper" NL
"consequat magna. Praesent sollicitudin elit quis aliquam volutpat." NL
"" NL
"Donec commodo nibh quis massa fringilla pellentesque. Vivamus faucibus" NL
"metus turpis, ac vehicula est efficitur eu. Fusce at iaculis neque, eget" NL
"lacinia nunc. Aliquam a pretium tortor. Nunc a justo a nisl vestibulum" NL
"tempus et ut magna. Pellentesque molestie nisl vitae lacus efficitur, ac" NL
"blandit mi consequat. Etiam non quam fringilla, mattis enim nec, porta" NL
"massa. Quisque volutpat quis quam eu consequat. Pellentesque maximus" NL
"aliquet nisl, eu pretium massa pellentesque ut. Etiam leo mauris, pretium" NL
"sed turpis auctor, cursus lacinia sapien. Donec sit amet sodales nisl, in" NL
"rhoncus enim.  Curabitur maximus varius nisl. Pellentesque lobortis" NL
"vestibulum ante sit amet auctor. Sed molestie erat ornare lacus fermentum" NL
"luctus. Nam rutrum vitae orci sed luctus." NL
"" NL
"Class aptent taciti sociosqu ad litora torquent per conubia nostra, per" NL
"inceptos himenaeos. Fusce vel sapien non tellus mattis gravida vel ac" NL
"lorem.  Nam ligula metus, cursus vel diam id, sagittis pellentesque" NL
"risus. Maecenas a purus vulputate, luctus mi eu, ullamcorper lectus." NL
"Donec tincidunt quis odio ut hendrerit. Cras at aliquam elit, at" NL
"pellentesque tellus. Pellentesque luctus sapien a dolor egestas porta." NL
"Aliquam a semper ipsum, eget auctor urna.  Vestibulum at lectus nisl." NL
"Cras maximus pharetra lobortis. Fusce condimentum efficitur diam in" NL
"porttitor." NL
"" NL
"Nullam ut fringilla orci. Quisque urna lacus, dictum a dolor id, mattis" NL
"placerat nulla. Phasellus facilisis dapibus magna id scelerisque." NL
"Phasellus vel velit massa. Nam pulvinar quis enim sit amet accumsan. Cras" NL
"efficitur ipsum quis mattis maximus. Donec placerat diam sem, nec lacinia" NL
"nisl ullamcorper eget. Sed dapibus mi sit amet metus vehicula porttitor." NL
"Quisque eget nunc ante.  Aenean libero leo, sodales a odio nec, auctor" NL
"efficitur risus. Pellentesque ut tortor vitae mauris tempor auctor." NL
"" NL
"Suspendisse in rutrum sapien. Ut posuere est sed dui malesuada egestas." NL
"Sed eu consectetur quam. Quisque rutrum nisi velit, quis rutrum lacus" NL
"sagittis ac. Ut sit amet ligula turpis. In volutpat, sapien sit amet" NL
"eleifend faucibus, lacus ligula faucibus justo, ut ultricies nisl enim eu" NL
"dui.  Vestibulum non nulla sed turpis tincidunt egestas sit amet tempus" NL
"nulla.  Phasellus arcu nibh, maximus ultricies pulvinar a, egestas id ex." NL
"Etiam at erat sed tellus tincidunt fermentum nec ut orci. Phasellus sed" NL
"turpis congue, finibus ligula a, porttitor ipsum. Integer dapibus cursus" NL
"elit id mollis.  Mauris quis purus sit amet justo dapibus cursus." NL
;

#undef NL

const unsigned char TestDataUtil::s_RANDOM_GARBAGE_1K[1024] = {
  0xef, 0x0f, 0x6b, 0x44, 0xd5, 0x3a, 0xcd, 0x43, 0x5c, 0x38, 0xc6, 0x27,
  0xe7, 0x02, 0xbd, 0xb6, 0x42, 0xcf, 0xff, 0xd9, 0x68, 0x01, 0x3f, 0xba,
  0x5e, 0x91, 0xf7, 0x41, 0xaa, 0xa3, 0xa9, 0xcc, 0x05, 0x5d, 0xa9, 0xb5,
  0xb0, 0x24, 0xf8, 0xe8, 0x2a, 0xb4, 0x83, 0x58, 0x50, 0xfe, 0x76, 0x54,
  0x15, 0xad, 0x2c, 0xbe, 0xf5, 0x60, 0xd7, 0x8d, 0x8d, 0x6d, 0xbe, 0xe3,
  0x3b, 0x0b, 0x0d, 0x74, 0x3f, 0xa3, 0x95, 0x00, 0x49, 0x9a, 0x05, 0xf6,
  0xa4, 0x67, 0xac, 0x5e, 0x29, 0xd5, 0x7e, 0x76, 0x1d, 0xa0, 0x88, 0x80,
  0xf5, 0x5a, 0x8f, 0x33, 0xc1, 0xd5, 0x72, 0xd1, 0xd3, 0x65, 0xe1, 0x98,
  0x57, 0xee, 0xd0, 0x94, 0x24, 0xd8, 0xac, 0x1d, 0x7a, 0x67, 0x51, 0xca,
  0x57, 0x84, 0x36, 0x36, 0x05, 0x97, 0x9d, 0xaf, 0xfa, 0x88, 0xae, 0x3e,
  0xdb, 0x68, 0x64, 0xf1, 0x5f, 0x2e, 0x3a, 0x90, 0x02, 0xea, 0x44, 0xa0,
  0xcd, 0x4b, 0xe0, 0xbd, 0xc8, 0xbe, 0x74, 0xd2, 0xb5, 0x43, 0x7a, 0xea,
  0xa0, 0x99, 0x00, 0xf4, 0x77, 0x7d, 0x6b, 0xbb, 0x3b, 0x70, 0x9b, 0xdd,
  0x63, 0x89, 0x90, 0x5a, 0x7d, 0x9c, 0x72, 0x37, 0x48, 0xd4, 0xa5, 0xf5,
  0x6a, 0xc4, 0x9a, 0x26, 0x3b, 0xee, 0x01, 0x6f, 0xb5, 0xef, 0x40, 0xf8,
  0x98, 0x4c, 0x28, 0x0d, 0x55, 0xb3, 0x65, 0xcc, 0x16, 0x5e, 0x2c, 0xcd,
  0x56, 0x03, 0x02, 0x16, 0x19, 0xdb, 0x48, 0x24, 0x82, 0x57, 0x6e, 0xcd,
  0xbd, 0x56, 0xbc, 0xf3, 0x59, 0x9a, 0x14, 0x20, 0x97, 0x09, 0x3f, 0x34,
  0x60, 0x8a, 0xcb, 0x5b, 0xd7, 0x46, 0x17, 0x61, 0x32, 0xb6, 0x3a, 0x04,
  0x8f, 0x1c, 0x59, 0x29, 0x85, 0x9a, 0x3a, 0x9d, 0x41, 0x46, 0xf6, 0x5a,
  0x64, 0x7a, 0x2c, 0xfa, 0x8e, 0xd6, 0xab, 0x28, 0xd9, 0xe5, 0xe2, 0x61,
  0x53, 0x93, 0x33, 0x3b, 0xad, 0x72, 0x18, 0x7e, 0xce, 0x3a, 0xbc, 0xf1,
  0x31, 0xf0, 0x41, 0x91, 0x56, 0x4c, 0x80, 0x70, 0x6f, 0x0c, 0xd7, 0x60,
  0xb8, 0x84, 0xdc, 0x4d, 0x1d, 0xf1, 0x1c, 0xed, 0x2e, 0xc4, 0x72, 0xe4,
  0x4a, 0x22, 0x5d, 0x63, 0xc1, 0x64, 0x4e, 0xda, 0x92, 0x6c, 0xc3, 0x38,
  0xe3, 0xfd, 0xff, 0x1d, 0x03, 0xee, 0x1b, 0x3d, 0xae, 0x4a, 0xb3, 0x15,
  0xd0, 0xa8, 0x1a, 0x5c, 0x46, 0x88, 0x96, 0xa0, 0x13, 0x26, 0xae, 0x05,
  0x08, 0xee, 0xde, 0x9c, 0xa8, 0x5b, 0x9e, 0xfc, 0xf3, 0x61, 0xa6, 0x97,
  0x76, 0xdc, 0xfa, 0x00, 0xc1, 0xf0, 0x3c, 0xc2, 0xec, 0x41, 0xaa, 0x10,
  0x48, 0x18, 0xaf, 0x4d, 0x68, 0x34, 0xe5, 0xe0, 0x77, 0x5d, 0xc3, 0x92,
  0xac, 0xc7, 0x81, 0x7a, 0x46, 0xb1, 0x74, 0xd7, 0x26, 0x76, 0xeb, 0x58,
  0x85, 0x6d, 0x71, 0xb6, 0x2d, 0x0c, 0xdb, 0xcb, 0xc9, 0x56, 0xc4, 0x4c,
  0x03, 0x7f, 0xf7, 0x6e, 0xbd, 0x6a, 0x62, 0xb2, 0x7d, 0x9a, 0xf0, 0xb7,
  0x36, 0x91, 0x38, 0xbf, 0xb9, 0x6d, 0x68, 0xa3, 0xf9, 0xb3, 0x64, 0xc7,
  0x8e, 0xda, 0xd9, 0xaa, 0x23, 0xff, 0xfc, 0x19, 0x5b, 0xa2, 0xfe, 0x62,
  0xf4, 0xf9, 0xec, 0x09, 0xbd, 0x3d, 0x45, 0x74, 0xe7, 0x2b, 0x2a, 0xae,
  0x15, 0xac, 0xff, 0xdc, 0x9a, 0x2a, 0x77, 0x17, 0x09, 0x2b, 0x88, 0x22,
  0xc8, 0x16, 0x42, 0x0a, 0xc7, 0xac, 0x45, 0xf8, 0x5d, 0xd8, 0xeb, 0x2d,
  0x9d, 0xff, 0xc8, 0xfc, 0x5d, 0xb0, 0x9d, 0xa1, 0x20, 0xc9, 0x14, 0x0b,
  0x53, 0x5d, 0x33, 0x10, 0x72, 0x1a, 0x03, 0x3d, 0x06, 0x44, 0x8d, 0x3a,
  0x7a, 0x9a, 0x64, 0x6c, 0x17, 0x70, 0x4a, 0x11, 0x95, 0x9d, 0x03, 0x92,
  0x2c, 0xf4, 0xeb, 0xab, 0xbd, 0x6e, 0x39, 0x70, 0x7d, 0x6d, 0x49, 0xb2,
  0x72, 0xa1, 0x12, 0xaa, 0x3e, 0xab, 0xcf, 0x86, 0x08, 0x80, 0x36, 0xf0,
  0xe8, 0x5f, 0xc3, 0x11, 0x56, 0x90, 0x87, 0x07, 0x1f, 0xc9, 0xf7, 0xb4,
  0xeb, 0x76, 0x2f, 0x48, 0x8d, 0x9c, 0xa5, 0xd5, 0x62, 0x5e, 0xd8, 0xe6,
  0x3e, 0x4f, 0x28, 0x05, 0xba, 0x68, 0x64, 0x23, 0x93, 0x4d, 0xa0, 0x29,
  0x19, 0x4d, 0x87, 0x18, 0x36, 0x90, 0xc8, 0x36, 0x20, 0xb5, 0x5d, 0x57,
  0x00, 0x32, 0xcb, 0x38, 0xe4, 0x6e, 0xb2, 0xfb, 0xf6, 0xa2, 0xcf, 0x51,
  0x8f, 0x74, 0x25, 0xa2, 0x7d, 0x4c, 0xd5, 0xa8, 0x61, 0x80, 0x2d, 0x38,
  0x11, 0x24, 0x64, 0xd0, 0x18, 0x83, 0xb7, 0x70, 0x2e, 0xde, 0x94, 0x4b,
  0x7a, 0x35, 0x69, 0x32, 0x22, 0x9a, 0xe4, 0x2e, 0x45, 0x76, 0x21, 0xe6,
  0x96, 0x11, 0x18, 0xb7, 0x22, 0x72, 0x5e, 0x62, 0xc4, 0xa5, 0xc5, 0x07,
  0x04, 0x4e, 0x75, 0x2e, 0xa9, 0xa2, 0x20, 0xa5, 0xa4, 0x96, 0x06, 0x71,
  0xe8, 0x98, 0x05, 0x1b, 0x0c, 0xfc, 0xe5, 0x95, 0x4d, 0x81, 0x34, 0x0d,
  0x1f, 0xde, 0x98, 0x2f, 0x34, 0xea, 0x74, 0xb9, 0x75, 0x4f, 0x90, 0x75,
  0x03, 0xd7, 0x8e, 0x66, 0x53, 0x57, 0x0b, 0xc7, 0xc6, 0x65, 0x67, 0x75,
  0xc2, 0xfd, 0xdb, 0x0e, 0xe0, 0xd6, 0x38, 0x4d, 0x77, 0xfe, 0x96, 0x00,
  0x92, 0xa6, 0x08, 0x5f, 0xfc, 0xb0, 0x09, 0x1c, 0x24, 0x8d, 0x22, 0x0c,
  0xdd, 0x88, 0x30, 0x79, 0xe3, 0x6d, 0x8e, 0x50, 0x67, 0xc6, 0xf2, 0x11,
  0x60, 0xe8, 0xb3, 0xad, 0x21, 0x6e, 0xd3, 0xd3, 0x2e, 0x4a, 0x02, 0x64,
  0xc6, 0x09, 0x24, 0xc7, 0xdd, 0x02, 0x33, 0xc6, 0xf7, 0x8e, 0x66, 0xef,
  0x65, 0xca, 0xc9, 0x5a, 0xf2, 0x36, 0x26, 0xfe, 0x02, 0x65, 0x57, 0xa1,
  0xc4, 0x12, 0x4e, 0xdf, 0x60, 0x8f, 0x34, 0x01, 0x41, 0x09, 0xe9, 0xc6,
  0xb2, 0xc9, 0xba, 0xb0, 0x54, 0xd3, 0x31, 0x15, 0x12, 0x9f, 0x34, 0x93,
  0x57, 0x81, 0x3e, 0x69, 0xc7, 0xad, 0x7a, 0xdb, 0x94, 0x98, 0x7c, 0x9f,
  0xd9, 0x0d, 0xf7, 0xbe, 0xe5, 0x8b, 0xb8, 0x28, 0x6e, 0x4b, 0xe7, 0x4e,
  0x2f, 0x32, 0x81, 0x95, 0x6a, 0xa9, 0x64, 0xdf, 0x9f, 0xeb, 0x18, 0x71,
  0xe1, 0xa1, 0x68, 0xb2, 0xb7, 0x8f, 0xd9, 0x30, 0x45, 0xc9, 0xbc, 0x07,
  0x82, 0x46, 0x45, 0x45, 0xfc, 0x3d, 0x9c, 0xcf, 0xe3, 0x96, 0x49, 0x28,
  0xb3, 0x40, 0x79, 0x0e, 0xdc, 0x99, 0x81, 0x81, 0x35, 0xeb, 0x5e, 0xdc,
  0x0d, 0x23, 0xc1, 0x4a, 0x95, 0xa1, 0xad, 0x60, 0xa3, 0xed, 0x19, 0x50,
  0xfe, 0x5a, 0x74, 0x5e, 0xe3, 0x39, 0xfc, 0xe2, 0x5c, 0x87, 0xa7, 0xa4,
  0x5c, 0x73, 0x8a, 0x92, 0x5e, 0x56, 0xa1, 0x18, 0xd4, 0x50, 0x47, 0x72,
  0x02, 0xaf, 0x2c, 0xad, 0x71, 0x36, 0xcf, 0x0e, 0x80, 0xed, 0x9e, 0xf7,
  0xc2, 0x53, 0x18, 0xc8, 0x2f, 0x66, 0xd6, 0x52, 0x93, 0x55, 0xc9, 0x54,
  0xae, 0xbf, 0x56, 0xc1, 0xd9, 0x97, 0x66, 0xfc, 0x3b, 0xd2, 0x46, 0x40,
  0xbf, 0x76, 0x8d, 0x2f, 0x0f, 0x85, 0x0e, 0x5b, 0x61, 0x02, 0x7f, 0xa0,
  0xed, 0x98, 0x47, 0x65, 0xb6, 0xe6, 0x0b, 0xa6, 0x19, 0x43, 0xbf, 0xf3,
  0x22, 0xfa, 0x27, 0xa1, 0xf9, 0x8b, 0xd1, 0xdf, 0x97, 0x4a, 0xa4, 0xd6,
  0x70, 0xb5, 0x70, 0x52, 0x68, 0x5b, 0xc3, 0x7c, 0x1f, 0xd9, 0xab, 0x43,
  0xd6, 0xb2, 0xa4, 0x9d, 0x2b, 0x23, 0xfe, 0x0f, 0x17, 0x14, 0xd6, 0xc4,
  0x1a, 0x7b, 0xbf, 0x3e, 0xef, 0x09, 0x8b, 0x76, 0x20, 0xe0, 0x9c, 0x77,
  0xb6, 0x58, 0x98, 0x5e, 0x55, 0x77, 0xc2, 0xae, 0x2f, 0xb4, 0x35, 0x21,
  0x25, 0x81, 0x69, 0x65, 0x80, 0xfc, 0x4f, 0x96, 0x74, 0xa4, 0xf0, 0x09,
  0x70, 0xef, 0xbb, 0xc8, 0x24, 0xfd, 0xfe, 0x36, 0xe2, 0x9e, 0x83, 0x2c,
  0x75, 0x6d, 0x88, 0xac
};

}  // close u namespace
}  // close unnamed namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 30: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n" "USAGE EXAMPLE" "\n"
                                       "=============" "\n";
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:  Reading and Writing Identifier Octets
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of this component.  Due
// to the low-level nature of this component, an extended usage example is not
// necessary.
//
// Suppose we wanted to write the identifier octets for a BER tag having the
// following properties:
//..
//    Tag Class:   Context-specific
//    Tag Type:    Primitive
//    Tag Number:  31
//..
// According to the BER specification, this should generate two octets
// containing the values 0x9F and 0x1F.  The following function demonstrates
// this:
//..
    bdlsb::MemOutStreamBuf osb;

    balber::BerConstants::TagClass tagClass  =
                                      balber::BerConstants::e_CONTEXT_SPECIFIC;
    balber::BerConstants::TagType  tagType   =
                                             balber::BerConstants::e_PRIMITIVE;
    int                            tagNumber = 31;

    int retCode = balber::BerUtil::putIdentifierOctets(&osb,
                                                       tagClass,
                                                       tagType,
                                                       tagNumber);
    ASSERT(0    == retCode);
    ASSERT(2    == osb.length());
    ASSERT(0x9F == (unsigned char)osb.data()[0]);
    ASSERT(0x1F == (unsigned char)osb.data()[1]);
//..
// The next part of the function will read the identifier octets from the
// stream and verify its contents:
//..
    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    balber::BerConstants::TagClass tagClassIn;
    balber::BerConstants::TagType  tagTypeIn;
    int                            tagNumberIn;
    int                            numBytesConsumed = 0;

    retCode = balber::BerUtil::getIdentifierOctets(&isb,
                                                   &tagClassIn,
                                                   &tagTypeIn,
                                                   &tagNumberIn,
                                                   &numBytesConsumed);
    ASSERT(0         == retCode);
    ASSERT(2         == numBytesConsumed);
    ASSERT(tagClass  == tagClassIn);
    ASSERT(tagType   == tagTypeIn);
    ASSERT(tagNumber == tagNumberIn);
//..

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING DATE/TIME FORMAT SELECTION
        //   This case tests the encoding format that
        //   'balber::BerUtil::putValue'  selects for data and time types given
        //   a value to encode and a set of encoding options.
        //
        // Concerns:
        //: 1 'putValue' selects the encoding format for all 'bdlt::Time',
        //:   'bdlt::TimeTz', 'bdlt::Datetime', and 'bdlt::DatetimeTz' values
        //:   according to the following algorithm:
        //:  o if the 'EncodeDateAndTimeTypesAsBinary' option is 'false',
        //:    use the ISO 8601 format, otherwise
        //:  o if the 'BdeVersionConformance' option is no lower than
        //:    35500, (indicating a BDE version no lower than 3.55.0) then
        //:    use the extended-binary format if one or both
        //:    of the following is true:
        //:    o the 'DatetimeFractionalSecondPrecision' option is 6
        //:    o the value of the 'time' or 'localTime' attribute, whichever
        //:      is appropriate for the corresponding value, is '24:00:00',
        //:      otherwise,
        //:  o use the compact-binary format
        //
        // Plan:
        //: 1 For a large selection of boundary values of the 'bdlt::Time',
        //:   'bdlt::TimeTz', 'bdlt::Datetime', and 'bdlt::DatetimeTz' types,
        //:   and combinations of each relevant encoding option as described in
        //:   the above algorithm, encode the value using the configuration
        //:   options and verify that the encoded representation is in the
        //:   format prescribed by the above algorithm.
        //
        // Testing:
        //   CONCERN: 'putValue' encoding formation selection
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING DATE/TIME FORMAT SELECTION"
                      << bsl::endl
                      << "=================================="
                      << bsl::endl;

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Time' FORMAT SELECTION"
                      << bsl::endl
                      << "-------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 25;

            static const int V =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;

            static const bool T = true;
            static const bool F = false;

            enum ExpFormat {
                BIN,
                TXT
            };

            static const struct {
                int         d_line;
                int         d_bdeVersionConformance;
                int         d_fractionalSecondPrecision;
                bool        d_encodeDateAndTimeTypesAsBinary;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_microsecond;
                ExpFormat   d_expFormat;
                const char *d_exp;
            } DATA[] = {
{ L_,     0, 3, F,  0,  0,  0,   0,   0, TXT, "\x0C"    "00:00:00.000"    },
{ L_,     0, 3, F,  0,  0,  0,   0,   1, TXT, "\x0C"    "00:00:00.000"    },
{ L_,     0, 3, F,  0,  0,  0,   1,   0, TXT, "\x0C"    "00:00:00.001"    },
{ L_,     0, 3, F,  0,  0,  1,   0,   0, TXT, "\x0C"    "00:00:01.000"    },
{ L_,     0, 3, F,  0,  1,  0,   0,   0, TXT, "\x0C"    "00:01:00.000"    },
{ L_,     0, 3, F,  1,  0,  0,   0,   0, TXT, "\x0C"    "01:00:00.000"    },
{ L_,     0, 3, F,  0,  0,  0,   0, 999, TXT, "\x0C"    "00:00:00.000"    },
{ L_,     0, 3, F,  0,  0,  0, 999,   0, TXT, "\x0C"    "00:00:00.999"    },
{ L_,     0, 3, F,  0,  0, 59,   0,   0, TXT, "\x0C"    "00:00:59.000"    },
{ L_,     0, 3, F,  0, 59,  0,   0,   0, TXT, "\x0C"    "00:59:00.000"    },
{ L_,     0, 3, F, 23,  0,  0,   0,   0, TXT, "\x0C"    "23:00:00.000"    },
{ L_,     0, 3, F, 23, 59, 59, 999, 999, TXT, "\x0C"    "23:59:59.999"    },
{ L_,     0, 3, F, 24,  0,  0,   0,   0, TXT, "\x0C"    "24:00:00.000"    },

{ L_,     0, 6, F,  0,  0,  0,   0,   0, TXT, "\x0F"    "00:00:00.000000" },
{ L_,     0, 6, F,  0,  0,  0,   0,   1, TXT, "\x0F"    "00:00:00.000001" },
{ L_,     0, 6, F,  0,  0,  0,   1,   0, TXT, "\x0F"    "00:00:00.001000" },
{ L_,     0, 6, F,  0,  0,  1,   0,   0, TXT, "\x0F"    "00:00:01.000000" },
{ L_,     0, 6, F,  0,  1,  0,   0,   0, TXT, "\x0F"    "00:01:00.000000" },
{ L_,     0, 6, F,  1,  0,  0,   0,   0, TXT, "\x0F"    "01:00:00.000000" },
{ L_,     0, 6, F,  0,  0,  0,   0, 999, TXT, "\x0F"    "00:00:00.000999" },
{ L_,     0, 6, F,  0,  0,  0, 999,   0, TXT, "\x0F"    "00:00:00.999000" },
{ L_,     0, 6, F,  0,  0, 59,   0,   0, TXT, "\x0F"    "00:00:59.000000" },
{ L_,     0, 6, F,  0, 59,  0,   0,   0, TXT, "\x0F"    "00:59:00.000000" },
{ L_,     0, 6, F, 23,  0,  0,   0,   0, TXT, "\x0F"    "23:00:00.000000" },
{ L_,     0, 6, F, 23, 59, 59, 999, 999, TXT, "\x0F"    "23:59:59.999999" },
{ L_,     0, 6, F, 24,  0,  0,   0,   0, TXT, "\x0F"    "24:00:00.000000" },

{ L_,     0, 3, T,  0,  0,  0,   0,   0, BIN, "01                     00" },
{ L_,     0, 3, T,  0,  0,  0,   0,   1, BIN, "01                     00" },
{ L_,     0, 3, T,  0,  0,  0,   1,   0, BIN, "01                     01" },
{ L_,     0, 3, T,  0,  0,  1,   0,   0, BIN, "02                  03 E8" },
{ L_,     0, 3, T,  0,  1,  0,   0,   0, BIN, "03               00 EA 60" },
{ L_,     0, 3, T,  1,  0,  0,   0,   0, BIN, "03               36 EE 80" },
{ L_,     0, 3, T,  0,  0,  0,   0, 999, BIN, "01                     00" },
{ L_,     0, 3, T,  0,  0,  0, 999,   0, BIN, "02                  03 E7" },
{ L_,     0, 3, T,  0,  0, 59,   0,   0, BIN, "03               00 E6 78" },
{ L_,     0, 3, T,  0, 59,  0,   0,   0, BIN, "03               36 04 20" },
{ L_,     0, 3, T, 23,  0,  0,   0,   0, BIN, "04            04 EF 6D 80" },
{ L_,     0, 3, T, 23, 59, 59, 999,   0, BIN, "04            05 26 5B FF" },
{ L_,     0, 3, T, 23, 59, 59, 999, 999, BIN, "04            05 26 5B FF" },
{ L_,     0, 3, T, 24,  0,  0,   0,   0, BIN, "01                     00" },

{ L_,     0, 6, T,  0,  0,  0,   0,   0, BIN, "01                     00" },
{ L_,     0, 6, T,  0,  0,  0,   0,   1, BIN, "01                     00" },
{ L_,     0, 6, T,  0,  0,  0,   1,   0, BIN, "01                     01" },
{ L_,     0, 6, T,  0,  0,  1,   0,   0, BIN, "02                  03 E8" },
{ L_,     0, 6, T,  0,  1,  0,   0,   0, BIN, "03               00 EA 60" },
{ L_,     0, 6, T,  1,  0,  0,   0,   0, BIN, "03               36 EE 80" },
{ L_,     0, 6, T,  0,  0,  0,   0, 999, BIN, "01                     00" },
{ L_,     0, 6, T,  0,  0,  0, 999,   0, BIN, "02                  03 E7" },
{ L_,     0, 6, T,  0,  0, 59,   0,   0, BIN, "03               00 E6 78" },
{ L_,     0, 6, T,  0, 59,  0,   0,   0, BIN, "03               36 04 20" },
{ L_,     0, 6, T, 23,  0,  0,   0,   0, BIN, "04            04 EF 6D 80" },
{ L_,     0, 6, T, 23, 59, 59, 999,   0, BIN, "04            05 26 5B FF" },
{ L_,     0, 6, T, 23, 59, 59, 999, 999, BIN, "04            05 26 5B FF" },
{ L_,     0, 6, T, 24,  0,  0,   0,   0, BIN, "01                     00" },

{ L_, V    , 3, F,  0,  0,  0,   0,   0, TXT, "\x0C"    "00:00:00.000"    },
{ L_, V    , 3, F,  0,  0,  0,   0,   1, TXT, "\x0C"    "00:00:00.000"    },
{ L_, V    , 3, F,  0,  0,  0,   1,   0, TXT, "\x0C"    "00:00:00.001"    },
{ L_, V    , 3, F,  0,  0,  1,   0,   0, TXT, "\x0C"    "00:00:01.000"    },
{ L_, V    , 3, F,  0,  1,  0,   0,   0, TXT, "\x0C"    "00:01:00.000"    },
{ L_, V    , 3, F,  1,  0,  0,   0,   0, TXT, "\x0C"    "01:00:00.000"    },
{ L_, V    , 3, F,  0,  0,  0,   0, 999, TXT, "\x0C"    "00:00:00.000"    },
{ L_, V    , 3, F,  0,  0,  0, 999,   0, TXT, "\x0C"    "00:00:00.999"    },
{ L_, V    , 3, F,  0,  0, 59,   0,   0, TXT, "\x0C"    "00:00:59.000"    },
{ L_, V    , 3, F,  0, 59,  0,   0,   0, TXT, "\x0C"    "00:59:00.000"    },
{ L_, V    , 3, F, 23,  0,  0,   0,   0, TXT, "\x0C"    "23:00:00.000"    },
{ L_, V    , 3, F, 23, 59, 59, 999, 999, TXT, "\x0C"    "23:59:59.999"    },
{ L_, V    , 3, F, 24,  0,  0,   0,   0, TXT, "\x0C"    "24:00:00.000"    },

{ L_, V    , 6, F,  0,  0,  0,   0,   0, TXT, "\x0F"    "00:00:00.000000" },
{ L_, V    , 6, F,  0,  0,  0,   0,   1, TXT, "\x0F"    "00:00:00.000001" },
{ L_, V    , 6, F,  0,  0,  0,   1,   0, TXT, "\x0F"    "00:00:00.001000" },
{ L_, V    , 6, F,  0,  0,  1,   0,   0, TXT, "\x0F"    "00:00:01.000000" },
{ L_, V    , 6, F,  0,  1,  0,   0,   0, TXT, "\x0F"    "00:01:00.000000" },
{ L_, V    , 6, F,  1,  0,  0,   0,   0, TXT, "\x0F"    "01:00:00.000000" },
{ L_, V    , 6, F,  0,  0,  0,   0, 999, TXT, "\x0F"    "00:00:00.000999" },
{ L_, V    , 6, F,  0,  0,  0, 999,   0, TXT, "\x0F"    "00:00:00.999000" },
{ L_, V    , 6, F,  0,  0, 59,   0,   0, TXT, "\x0F"    "00:00:59.000000" },
{ L_, V    , 6, F,  0, 59,  0,   0,   0, TXT, "\x0F"    "00:59:00.000000" },
{ L_, V    , 6, F, 23,  0,  0,   0,   0, TXT, "\x0F"    "23:00:00.000000" },
{ L_, V    , 6, F, 23, 59, 59, 999, 999, TXT, "\x0F"    "23:59:59.999999" },
{ L_, V    , 6, F, 24,  0,  0,   0,   0, TXT, "\x0F"    "24:00:00.000000" },

{ L_, V    , 3, T,  0,  0,  0,   0,   0, BIN, "01                     00" },
{ L_, V    , 3, T,  0,  0,  0,   0,   1, BIN, "01                     00" },
{ L_, V    , 3, T,  0,  0,  0,   1,   0, BIN, "01                     01" },
{ L_, V    , 3, T,  0,  0,  1,   0,   0, BIN, "02                  03 E8" },
{ L_, V    , 3, T,  0,  1,  0,   0,   0, BIN, "03               00 EA 60" },
{ L_, V    , 3, T,  1,  0,  0,   0,   0, BIN, "03               36 EE 80" },
{ L_, V    , 3, T,  0,  0,  0,   0, 999, BIN, "01                     00" },
{ L_, V    , 3, T,  0,  0,  0, 999,   0, BIN, "02                  03 E7" },
{ L_, V    , 3, T,  0,  0, 59,   0,   0, BIN, "03               00 E6 78" },
{ L_, V    , 3, T,  0, 59,  0,   0,   0, BIN, "03               36 04 20" },
{ L_, V    , 3, T, 23,  0,  0,   0,   0, BIN, "04            04 EF 6D 80" },
{ L_, V    , 3, T, 23, 59, 59, 999,   0, BIN, "04            05 26 5B FF" },
{ L_, V    , 3, T, 23, 59, 59, 999, 999, BIN, "04            05 26 5B FF" },
{ L_, V    , 3, T, 24,  0,  0,   0,   0, BIN, "07  80 00  14 1D D7 60 00" },

{ L_, V    , 6, T,  0,  0,  0,   0,   0, BIN, "07  80 00  00 00 00 00 00" },
{ L_, V    , 6, T,  0,  0,  0,   0,   1, BIN, "07  80 00  00 00 00 00 01" },
{ L_, V    , 6, T,  0,  0,  0,   1,   0, BIN, "07  80 00  00 00 00 03 E8" },
{ L_, V    , 6, T,  0,  0,  1,   0,   0, BIN, "07  80 00  00 00 0F 42 40" },
{ L_, V    , 6, T,  0,  1,  0,   0,   0, BIN, "07  80 00  00 03 93 87 00" },
{ L_, V    , 6, T,  1,  0,  0,   0,   0, BIN, "07  80 00  00 D6 93 A4 00" },
{ L_, V    , 6, T,  0,  0,  0,   0, 999, BIN, "07  80 00  00 00 00 03 E7" },
{ L_, V    , 6, T,  0,  0,  0, 999,   0, BIN, "07  80 00  00 00 0F 3E 58" },
{ L_, V    , 6, T,  0,  0, 59,   0,   0, BIN, "07  80 00  00 03 84 44 C0" },
{ L_, V    , 6, T,  0, 59,  0,   0,   0, BIN, "07  80 00  00 D3 00 1D 00" },
{ L_, V    , 6, T, 23,  0,  0,   0,   0, BIN, "07  80 00  13 47 43 BC 00" },
{ L_, V    , 6, T, 23, 59, 59, 999,   0, BIN, "07  80 00  14 1D D7 5C 18" },
{ L_, V    , 6, T, 23, 59, 59, 999, 999, BIN, "07  80 00  14 1D D7 5F FF" },
{ L_, V    , 6, T, 24,  0,  0,   0,   0, BIN, "07  80 00  14 1D D7 60 00" },

{ L_,     0, 3, T, 24,  0,  0,   0,   0, BIN, "01                     00" },
{ L_, V - 1, 3, T, 24,  0,  0,   0,   0, BIN, "01                     00" },
{ L_, V    , 3, T, 24,  0,  0,   0,   0, BIN, "07  80 00  14 1D D7 60 00" },
{ L_, V + 1, 3, T, 24,  0,  0,   0,   0, BIN, "07  80 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  VERSION   = DATA[i].d_bdeVersionConformance;
                const int  PRECISION = DATA[i].d_fractionalSecondPrecision;
                const bool BINARY = DATA[i].d_encodeDateAndTimeTypesAsBinary;
                const int  HOUR   = DATA[i].d_hour;
                const int  MINUTE = DATA[i].d_minute;
                const int  SECOND = DATA[i].d_second;
                const int  MILLISECOND       = DATA[i].d_millisecond;
                const int  MICROSECOND       = DATA[i].d_microsecond;
                const ExpFormat   EXP_FORMAT = DATA[i].d_expFormat;
                const char *const EXP        = DATA[i].d_exp;

                const bdlt::Time TIME(HOUR,
                                      MINUTE,
                                      SECOND,
                                      MILLISECOND,
                                      MICROSECOND);

                bdlsb::MemOutStreamBuf outStreamBuf;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(BINARY);
                options.setDatetimeFractionalSecondPrecision(PRECISION);

                int rc = Util::putValue(&outStreamBuf, TIME, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                char buffer[k_MAX_BUFFER_SIZE];
                int bufferSize = 0;

                switch (EXP_FORMAT) {
                  case BIN: {
                    rc = u::ByteBufferUtil::loadBuffer(
                        &bufferSize,
                        buffer,
                        sizeof(buffer),
                        EXP,
                        static_cast<int>(bsl::strlen(EXP)));
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc)
                        continue;  // CONTINUE
                  } break;
                  case TXT: {
                    bsl::strncpy(buffer, EXP, sizeof(buffer));
                    bufferSize = static_cast<int>(bsl::strlen(buffer));
                  } break;
                }

                LOOP1_ASSERT_EQ(LINE,
                                outStreamBuf.length(),
                                static_cast<bsl::size_t>(bufferSize));
                if (outStreamBuf.length() !=
                    static_cast<bsl::size_t>(bufferSize)) continue; // CONTINUE

                const bool BUFFERS_EQUAL = bsl::equal(
                    buffer, buffer + bufferSize, outStreamBuf.data());
                LOOP1_ASSERT(LINE, BUFFERS_EQUAL);
                if (!BUFFERS_EQUAL && veryVerbose) {
                    bsl::cout << "ACTUAL: ["
                              << bslstl::StringRef(outStreamBuf.data(),
                                                   outStreamBuf.length())
                              << "]"
                              << bsl::endl;
                }

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());

                bdlt::Time time;
                int        accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &inStreamBuf, &time, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                if (TIME == bdlt::Time() && VERSION < V && BINARY) {
                    LOOP1_ASSERT_EQ(LINE, time.hour(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.minute(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.second(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.millisecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.microsecond(), 0);
                }
                else if ((VERSION < V && BINARY) || 3 == PRECISION) {
                    LOOP1_ASSERT_EQ(LINE, time.hour(), TIME.hour());
                    LOOP1_ASSERT_EQ(LINE, time.minute(), TIME.minute());
                    LOOP1_ASSERT_EQ(LINE, time.second(), TIME.second());
                    LOOP1_ASSERT_EQ(
                        LINE, time.millisecond(), TIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, time.microsecond(), 0);
                }
                else {
                    LOOP1_ASSERT_EQ(LINE, time, TIME);
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::TimeTz' FORMAT SELECTION"
                      << bsl::endl
                      << "---------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 25;

            static const int V =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;

            static const bool T = true;
            static const bool F = false;

            enum ExpFormat {
                BIN,
                TXT
            };

            static const struct {
                int d_line;
                int d_bdeVersionConformance;
                int d_fractionalSecondPrecision;
                bool d_encodeDateAndTimeTypesAsBinary;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
                ExpFormat d_expFormat;
                const char *d_exp;
            } DATA[] = {
{ L_,  0, 3, F,  0,  0,  0,   0,   0,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_,  0, 3, F,  0,  0,  0,   0,   0,    1, TXT, "\x12" "00:00:00.000+00:01" },
{ L_,  0, 3, F,  0,  0,  0,   0,   0,   -1, TXT, "\x12" "00:00:00.000-00:01" },
{ L_,  0, 3, F,  0,  0,  0,   0,   1,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_,  0, 3, F,  0,  0,  0,   1,   0,    0, TXT, "\x12" "00:00:00.001+00:00" },
{ L_,  0, 3, F,  0,  0,  1,   0,   0,    0, TXT, "\x12" "00:00:01.000+00:00" },
{ L_,  0, 3, F,  0,  1,  0,   0,   0,    0, TXT, "\x12" "00:01:00.000+00:00" },
{ L_,  0, 3, F,  1,  0,  0,   0,   0,    0, TXT, "\x12" "01:00:00.000+00:00" },
{ L_,  0, 3, F,  0,  0,  0,   0,   0, 1439, TXT, "\x12" "00:00:00.000+23:59" },
{ L_,  0, 3, F,  0,  0,  0,   0,   0,-1439, TXT, "\x12" "00:00:00.000-23:59" },
{ L_,  0, 3, F,  0,  0,  0,   0,   0,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_,  0, 3, F,  0,  0,  0,   0, 999,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_,  0, 3, F,  0,  0,  0, 999,   0,    0, TXT, "\x12" "00:00:00.999+00:00" },
{ L_,  0, 3, F,  0,  0, 59,   0,   0,    0, TXT, "\x12" "00:00:59.000+00:00" },
{ L_,  0, 3, F,  0, 59,  0,   0,   0,    0, TXT, "\x12" "00:59:00.000+00:00" },
{ L_,  0, 3, F, 23,  0,  0,   0,   0,    0, TXT, "\x12" "23:00:00.000+00:00" },
{ L_,  0, 3, F, 23, 59, 59, 999, 999, 1439, TXT, "\x12" "23:59:59.999+23:59" },
{ L_,  0, 3, F, 23, 59, 59, 999, 999,-1439, TXT, "\x12" "23:59:59.999-23:59" },
{ L_,  0, 3, F, 24,  0,  0,   0,   0,    0, TXT, "\x12" "24:00:00.000+00:00" },

{ L_,  0, 6, F,  0,  0,  0,   0,   0,    0, TXT, "\x15" "00:00:00.000000+00:00" },
{ L_,  0, 6, F,  0,  0,  0,   0,   0,    1, TXT, "\x15" "00:00:00.000000+00:01" },
{ L_,  0, 6, F,  0,  0,  0,   0,   0,   -1, TXT, "\x15" "00:00:00.000000-00:01" },
{ L_,  0, 6, F,  0,  0,  0,   0,   1,    0, TXT, "\x15" "00:00:00.000001+00:00" },
{ L_,  0, 6, F,  0,  0,  0,   1,   0,    0, TXT, "\x15" "00:00:00.001000+00:00" },
{ L_,  0, 6, F,  0,  0,  1,   0,   0,    0, TXT, "\x15" "00:00:01.000000+00:00" },
{ L_,  0, 6, F,  0,  1,  0,   0,   0,    0, TXT, "\x15" "00:01:00.000000+00:00" },
{ L_,  0, 6, F,  1,  0,  0,   0,   0,    0, TXT, "\x15" "01:00:00.000000+00:00" },
{ L_,  0, 6, F,  0,  0,  0,   0,   0, 1439, TXT, "\x15" "00:00:00.000000+23:59" },
{ L_,  0, 6, F,  0,  0,  0,   0,   0,-1439, TXT, "\x15" "00:00:00.000000-23:59" },
{ L_,  0, 6, F,  0,  0,  0,   0,   0,    0, TXT, "\x15" "00:00:00.000000+00:00" },
{ L_,  0, 6, F,  0,  0,  0,   0, 999,    0, TXT, "\x15" "00:00:00.000999+00:00" },
{ L_,  0, 6, F,  0,  0,  0, 999,   0,    0, TXT, "\x15" "00:00:00.999000+00:00" },
{ L_,  0, 6, F,  0,  0, 59,   0,   0,    0, TXT, "\x15" "00:00:59.000000+00:00" },
{ L_,  0, 6, F,  0, 59,  0,   0,   0,    0, TXT, "\x15" "00:59:00.000000+00:00" },
{ L_,  0, 6, F, 23,  0,  0,   0,   0,    0, TXT, "\x15" "23:00:00.000000+00:00" },
{ L_,  0, 6, F, 23, 59, 59, 999, 999, 1439, TXT, "\x15" "23:59:59.999999+23:59" },
{ L_,  0, 6, F, 23, 59, 59, 999, 999,-1439, TXT, "\x15" "23:59:59.999999-23:59" },
{ L_,  0, 6, F, 24,  0,  0,   0,   0,    0, TXT, "\x15" "24:00:00.000000+00:00" },

{ L_,  0, 3, T,  0,  0,  0,   0,   0,    0, BIN, "01                  00" },
{ L_,  0, 3, T,  0,  0,  0,   0,   0,    1, BIN, "05  00 01     00 00 00" },
{ L_,  0, 3, T,  0,  0,  0,   0,   0,   -1, BIN, "05  FF FF     00 00 00" },
{ L_,  0, 3, T,  0,  0,  0,   0,   1,    0, BIN, "01                  00" },
{ L_,  0, 3, T,  0,  0,  0,   1,   0,    0, BIN, "01                  01" },
{ L_,  0, 3, T,  0,  0,  1,   0,   0,    0, BIN, "02               03 E8" },
{ L_,  0, 3, T,  0,  1,  0,   0,   0,    0, BIN, "03            00 EA 60" },
{ L_,  0, 3, T,  1,  0,  0,   0,   0,    0, BIN, "03            36 EE 80" },
{ L_,  0, 3, T,  0,  0,  0,   0,   0, 1439, BIN, "05  05 9F     00 00 00" },
{ L_,  0, 3, T,  0,  0,  0,   0,   0,-1439, BIN, "05  FA 61     00 00 00" },
{ L_,  0, 3, T,  0,  0,  0,   0, 999,    0, BIN, "01                  00" },
{ L_,  0, 3, T,  0,  0,  0, 999,   0,    0, BIN, "02               03 E7" },
{ L_,  0, 3, T,  0,  0, 59,   0,   0,    0, BIN, "03            00 E6 78" },
{ L_,  0, 3, T,  0, 59,  0,   0,   0,    0, BIN, "03            36 04 20" },
{ L_,  0, 3, T, 23,  0,  0,   0,   0,    0, BIN, "04         04 EF 6D 80" },
{ L_,  0, 3, T, 23, 59, 59, 999,   0, 1439, BIN, "06  05 9F  05 26 5B FF" },
{ L_,  0, 3, T, 23, 59, 59, 999,   0,-1439, BIN, "06  FA 61  05 26 5B FF" },
{ L_,  0, 3, T, 23, 59, 59, 999, 999, 1439, BIN, "06  05 9F  05 26 5B FF" },
{ L_,  0, 3, T, 23, 59, 59, 999, 999,-1439, BIN, "06  FA 61  05 26 5B FF" },
{ L_,  0, 3, T, 24,  0,  0,   0,   0,    0, BIN, "01                  00" },

{ L_,  0, 6, T,  0,  0,  0,   0,   0,    0, BIN, "01                  00" },
{ L_,  0, 6, T,  0,  0,  0,   0,   0,    1, BIN, "05  00 01     00 00 00" },
{ L_,  0, 6, T,  0,  0,  0,   0,   0,   -1, BIN, "05  FF FF     00 00 00" },
{ L_,  0, 6, T,  0,  0,  0,   0,   1,    0, BIN, "01                  00" },
{ L_,  0, 6, T,  0,  0,  0,   1,   0,    0, BIN, "01                  01" },
{ L_,  0, 6, T,  0,  0,  1,   0,   0,    0, BIN, "02               03 E8" },
{ L_,  0, 6, T,  0,  1,  0,   0,   0,    0, BIN, "03            00 EA 60" },
{ L_,  0, 6, T,  1,  0,  0,   0,   0,    0, BIN, "03            36 EE 80" },
{ L_,  0, 6, T,  0,  0,  0,   0,   0, 1439, BIN, "05  05 9F     00 00 00" },
{ L_,  0, 6, T,  0,  0,  0,   0,   0,-1439, BIN, "05  FA 61     00 00 00" },
{ L_,  0, 6, T,  0,  0,  0,   0, 999,    0, BIN, "01                  00" },
{ L_,  0, 6, T,  0,  0,  0, 999,   0,    0, BIN, "02               03 E7" },
{ L_,  0, 6, T,  0,  0, 59,   0,   0,    0, BIN, "03            00 E6 78" },
{ L_,  0, 6, T,  0, 59,  0,   0,   0,    0, BIN, "03            36 04 20" },
{ L_,  0, 6, T, 23,  0,  0,   0,   0,    0, BIN, "04         04 EF 6D 80" },
{ L_,  0, 6, T, 23, 59, 59, 999,   0, 1439, BIN, "06  05 9F  05 26 5B FF" },
{ L_,  0, 6, T, 23, 59, 59, 999,   0,-1439, BIN, "06  FA 61  05 26 5B FF" },
{ L_,  0, 6, T, 23, 59, 59, 999, 999, 1439, BIN, "06  05 9F  05 26 5B FF" },
{ L_,  0, 6, T, 23, 59, 59, 999, 999,-1439, BIN, "06  FA 61  05 26 5B FF" },
{ L_,  0, 6, T, 24,  0,  0,   0,   0,    0, BIN, "01                  00" },

{ L_, V , 3, F,  0,  0,  0,   0,   0,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_, V , 3, F,  0,  0,  0,   0,   0,    1, TXT, "\x12" "00:00:00.000+00:01" },
{ L_, V , 3, F,  0,  0,  0,   0,   0,   -1, TXT, "\x12" "00:00:00.000-00:01" },
{ L_, V , 3, F,  0,  0,  0,   0,   1,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_, V , 3, F,  0,  0,  0,   1,   0,    0, TXT, "\x12" "00:00:00.001+00:00" },
{ L_, V , 3, F,  0,  0,  1,   0,   0,    0, TXT, "\x12" "00:00:01.000+00:00" },
{ L_, V , 3, F,  0,  1,  0,   0,   0,    0, TXT, "\x12" "00:01:00.000+00:00" },
{ L_, V , 3, F,  1,  0,  0,   0,   0,    0, TXT, "\x12" "01:00:00.000+00:00" },
{ L_, V , 3, F,  0,  0,  0,   0,   0, 1439, TXT, "\x12" "00:00:00.000+23:59" },
{ L_, V , 3, F,  0,  0,  0,   0,   0,-1439, TXT, "\x12" "00:00:00.000-23:59" },
{ L_, V , 3, F,  0,  0,  0,   0,   0,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_, V , 3, F,  0,  0,  0,   0, 999,    0, TXT, "\x12" "00:00:00.000+00:00" },
{ L_, V , 3, F,  0,  0,  0, 999,   0,    0, TXT, "\x12" "00:00:00.999+00:00" },
{ L_, V , 3, F,  0,  0, 59,   0,   0,    0, TXT, "\x12" "00:00:59.000+00:00" },
{ L_, V , 3, F,  0, 59,  0,   0,   0,    0, TXT, "\x12" "00:59:00.000+00:00" },
{ L_, V , 3, F, 23,  0,  0,   0,   0,    0, TXT, "\x12" "23:00:00.000+00:00" },
{ L_, V , 3, F, 23, 59, 59, 999, 999, 1439, TXT, "\x12" "23:59:59.999+23:59" },
{ L_, V , 3, F, 23, 59, 59, 999, 999,-1439, TXT, "\x12" "23:59:59.999-23:59" },
{ L_, V , 3, F, 24,  0,  0,   0,   0,    0, TXT, "\x12" "24:00:00.000+00:00" },

{ L_, V , 6, F,  0,  0,  0,   0,   0,    0, TXT, "\x15" "00:00:00.000000+00:00" },
{ L_, V , 6, F,  0,  0,  0,   0,   0,    1, TXT, "\x15" "00:00:00.000000+00:01" },
{ L_, V , 6, F,  0,  0,  0,   0,   0,   -1, TXT, "\x15" "00:00:00.000000-00:01" },
{ L_, V , 6, F,  0,  0,  0,   0,   1,    0, TXT, "\x15" "00:00:00.000001+00:00" },
{ L_, V , 6, F,  0,  0,  0,   1,   0,    0, TXT, "\x15" "00:00:00.001000+00:00" },
{ L_, V , 6, F,  0,  0,  1,   0,   0,    0, TXT, "\x15" "00:00:01.000000+00:00" },
{ L_, V , 6, F,  0,  1,  0,   0,   0,    0, TXT, "\x15" "00:01:00.000000+00:00" },
{ L_, V , 6, F,  1,  0,  0,   0,   0,    0, TXT, "\x15" "01:00:00.000000+00:00" },
{ L_, V , 6, F,  0,  0,  0,   0,   0, 1439, TXT, "\x15" "00:00:00.000000+23:59" },
{ L_, V , 6, F,  0,  0,  0,   0,   0,-1439, TXT, "\x15" "00:00:00.000000-23:59" },
{ L_, V , 6, F,  0,  0,  0,   0,   0,    0, TXT, "\x15" "00:00:00.000000+00:00" },
{ L_, V , 6, F,  0,  0,  0,   0, 999,    0, TXT, "\x15" "00:00:00.000999+00:00" },
{ L_, V , 6, F,  0,  0,  0, 999,   0,    0, TXT, "\x15" "00:00:00.999000+00:00" },
{ L_, V , 6, F,  0,  0, 59,   0,   0,    0, TXT, "\x15" "00:00:59.000000+00:00" },
{ L_, V , 6, F,  0, 59,  0,   0,   0,    0, TXT, "\x15" "00:59:00.000000+00:00" },
{ L_, V , 6, F, 23,  0,  0,   0,   0,    0, TXT, "\x15" "23:00:00.000000+00:00" },
{ L_, V , 6, F, 23, 59, 59, 999, 999, 1439, TXT, "\x15" "23:59:59.999999+23:59" },
{ L_, V , 6, F, 23, 59, 59, 999, 999,-1439, TXT, "\x15" "23:59:59.999999-23:59" },
{ L_, V , 6, F, 24,  0,  0,   0,   0,    0, TXT, "\x15" "24:00:00.000000+00:00" },

{ L_, V , 3, T,  0,  0,  0,   0,   0,    0, BIN, "01                     00" },
{ L_, V , 3, T,  0,  0,  0,   0,   0,    1, BIN, "05  00 01        00 00 00" },
{ L_, V , 3, T,  0,  0,  0,   0,   0,   -1, BIN, "05  FF FF        00 00 00" },
{ L_, V , 3, T,  0,  0,  0,   0,   1,    0, BIN, "01                     00" },
{ L_, V , 3, T,  0,  0,  0,   1,   0,    0, BIN, "01                     01" },
{ L_, V , 3, T,  0,  0,  1,   0,   0,    0, BIN, "02                  03 E8" },
{ L_, V , 3, T,  0,  1,  0,   0,   0,    0, BIN, "03               00 EA 60" },
{ L_, V , 3, T,  1,  0,  0,   0,   0,    0, BIN, "03               36 EE 80" },
{ L_, V , 3, T,  0,  0,  0,   0,   0, 1439, BIN, "05  05 9F        00 00 00" },
{ L_, V , 3, T,  0,  0,  0,   0,   0,-1439, BIN, "05  FA 61        00 00 00" },
{ L_, V , 3, T,  0,  0,  0,   0, 999,    0, BIN, "01                     00" },
{ L_, V , 3, T,  0,  0,  0, 999,   0,    0, BIN, "02                  03 E7" },
{ L_, V , 3, T,  0,  0, 59,   0,   0,    0, BIN, "03               00 E6 78" },
{ L_, V , 3, T,  0, 59,  0,   0,   0,    0, BIN, "03               36 04 20" },
{ L_, V , 3, T, 23,  0,  0,   0,   0,    0, BIN, "04            04 EF 6D 80" },
{ L_, V , 3, T, 23, 59, 59, 999,   0, 1439, BIN, "06  05 9F     05 26 5B FF" },
{ L_, V , 3, T, 23, 59, 59, 999,   0,-1439, BIN, "06  FA 61     05 26 5B FF" },
{ L_, V , 3, T, 23, 59, 59, 999, 999, 1439, BIN, "06  05 9F     05 26 5B FF" },
{ L_, V , 3, T, 23, 59, 59, 999, 999,-1439, BIN, "06  FA 61     05 26 5B FF" },
{ L_, V , 3, T, 24,  0,  0,   0,   0,    0, BIN, "07  90 00  14 1D D7 60 00" },

{ L_, V , 6, T,  0,  0,  0,   0,   0,    0, BIN, "07  90 00  00 00 00 00 00" },
{ L_, V , 6, T,  0,  0,  0,   0,   0,    1, BIN, "07  90 01  00 00 00 00 00" },
{ L_, V , 6, T,  0,  0,  0,   0,   0,   -1, BIN, "07  9F FF  00 00 00 00 00" },
{ L_, V , 6, T,  0,  0,  0,   0,   1,    0, BIN, "07  90 00  00 00 00 00 01" },
{ L_, V , 6, T,  0,  0,  0,   1,   0,    0, BIN, "07  90 00  00 00 00 03 E8" },
{ L_, V , 6, T,  0,  0,  1,   0,   0,    0, BIN, "07  90 00  00 00 0F 42 40" },
{ L_, V , 6, T,  0,  1,  0,   0,   0,    0, BIN, "07  90 00  00 03 93 87 00" },
{ L_, V , 6, T,  1,  0,  0,   0,   0,    0, BIN, "07  90 00  00 D6 93 A4 00" },
{ L_, V , 6, T,  0,  0,  0,   0,   0, 1439, BIN, "07  95 9F  00 00 00 00 00" },
{ L_, V , 6, T,  0,  0,  0,   0,   0,-1439, BIN, "07  9A 61  00 00 00 00 00" },
{ L_, V , 6, T,  0,  0,  0,   0, 999,    0, BIN, "07  90 00  00 00 00 03 E7" },
{ L_, V , 6, T,  0,  0,  0, 999,   0,    0, BIN, "07  90 00  00 00 0F 3E 58" },
{ L_, V , 6, T,  0,  0, 59,   0,   0,    0, BIN, "07  90 00  00 03 84 44 C0" },
{ L_, V , 6, T,  0, 59,  0,   0,   0,    0, BIN, "07  90 00  00 D3 00 1D 00" },
{ L_, V , 6, T, 23,  0,  0,   0,   0,    0, BIN, "07  90 00  13 47 43 BC 00" },
{ L_, V , 6, T, 23, 59, 59, 999,   0, 1439, BIN, "07  95 9F  14 1D D7 5C 18" },
{ L_, V , 6, T, 23, 59, 59, 999,   0,-1439, BIN, "07  9A 61  14 1D D7 5C 18" },
{ L_, V , 6, T, 23, 59, 59, 999, 999, 1439, BIN, "07  95 9F  14 1D D7 5F FF" },
{ L_, V , 6, T, 23, 59, 59, 999, 999,-1439, BIN, "07  9A 61  14 1D D7 5F FF" },
{ L_, V , 6, T, 24,  0,  0,   0,   0,    0, BIN, "07  90 00  14 1D D7 60 00" },

{ L_,  0, 3, T, 24,  0,  0,   0,   0,    0, BIN, "01                     00" },
{ L_,V-1, 3, T, 24,  0,  0,   0,   0,    0, BIN, "01                     00" },
{ L_,V  , 3, T, 24,  0,  0,   0,   0,    0, BIN, "07  90 00  14 1D D7 60 00" },
{ L_,V+1, 3, T, 24,  0,  0,   0,   0,    0, BIN, "07  90 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  VERSION   = DATA[i].d_bdeVersionConformance;
                const int  PRECISION = DATA[i].d_fractionalSecondPrecision;
                const bool BINARY = DATA[i].d_encodeDateAndTimeTypesAsBinary;
                const int  HOUR   = DATA[i].d_hour;
                const int  MINUTE = DATA[i].d_minute;
                const int  SECOND = DATA[i].d_second;
                const int  MILLISECOND       = DATA[i].d_millisecond;
                const int  MICROSECOND       = DATA[i].d_microsecond;
                const int  OFFSET            = DATA[i].d_offset;
                const ExpFormat   EXP_FORMAT = DATA[i].d_expFormat;
                const char *const EXP        = DATA[i].d_exp;

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND,
                                      MICROSECOND);

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                bdlsb::MemOutStreamBuf outStreamBuf;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(BINARY);
                options.setDatetimeFractionalSecondPrecision(PRECISION);

                int rc = Util::putValue(&outStreamBuf, TIMETZ, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                char buffer[k_MAX_BUFFER_SIZE];
                int bufferSize = 0;

                switch (EXP_FORMAT) {
                  case BIN: {
                    rc = u::ByteBufferUtil::loadBuffer(
                        &bufferSize,
                        buffer,
                        sizeof(buffer),
                        EXP,
                        static_cast<int>(bsl::strlen(EXP)));
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc)
                        continue;  // CONTINUE
                  } break;
                  case TXT: {
                    bsl::strncpy(buffer, EXP, sizeof(buffer));
                    bufferSize = static_cast<int>(bsl::strlen(buffer));
                  } break;
                }

                LOOP1_ASSERT_EQ(LINE,
                                outStreamBuf.length(),
                                static_cast<bsl::size_t>(bufferSize));
                if (outStreamBuf.length() !=
                    static_cast<bsl::size_t>(bufferSize)) continue; // CONTINUE

                const bool BUFFERS_EQUAL = bsl::equal(
                    buffer, buffer + bufferSize, outStreamBuf.data());
                LOOP1_ASSERT(LINE, BUFFERS_EQUAL);
                if (!BUFFERS_EQUAL && veryVerbose && BINARY) {
                    bsl::cout << "ACTUAL: [";
                    bdlb::Print::singleLineHexDump(
                        bsl::cout,
                        outStreamBuf.data(),
                        static_cast<int>(outStreamBuf.length()));
                    bsl::cout << "]" << bsl::endl;
                }
                else if (!BUFFERS_EQUAL && veryVerbose) {
                    bsl::cout << "ACTUAL: ["
                              << bslstl::StringRef(outStreamBuf.data(),
                                                   outStreamBuf.length())
                              << "]"
                              << bsl::endl;
                }

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());

                bdlt::TimeTz timeTz;
                int accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &inStreamBuf, &timeTz, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                if (TIMETZ.localTime() == bdlt::Time() && VERSION < V &&
                    BINARY) {
                    const bdlt::Time& time = timeTz.localTime();
                    LOOP1_ASSERT_EQ(LINE, time.hour(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.minute(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.second(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.millisecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, time.microsecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, timeTz.offset(), 0);
                }
                else if ((VERSION < V && BINARY) || 3 == PRECISION) {
                    const bdlt::Time& TIME = TIMETZ.localTime();
                    const bdlt::Time& time = timeTz.localTime();
                    LOOP1_ASSERT_EQ(LINE, time.hour(), TIME.hour());
                    LOOP1_ASSERT_EQ(LINE, time.minute(), TIME.minute());
                    LOOP1_ASSERT_EQ(LINE, time.second(), TIME.second());
                    LOOP1_ASSERT_EQ(
                        LINE, time.millisecond(), TIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, timeTz.offset(), TIMETZ.offset());
                }
                else {
                    LOOP1_ASSERT_EQ(LINE, timeTz, TIMETZ);
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Datetime' FORMAT SELECTION"
                      << bsl::endl
                      << "-----------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 30;

            static const int V =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;

            static const bool T = true;
            static const bool F = false;

            enum ExpFormat {
                BIN,
                TXT
            };

            static const struct {
                int d_line;
                int d_bdeVersionConformance;
                int d_fractionalSecondPrecision;
                bool d_encodeDateAndTimeTypesAsBinary;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                ExpFormat d_expFormat;
                const char *d_exp;
            } DATA[] = {
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   1, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   1,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.001"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  1,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:01.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  1,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:01:00.000"    },
{ L_, 0, 3, F,    1,  1,  1,  1,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T01:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  2,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-02T00:00:00.000"    },
{ L_, 0, 3, F,    1,  2,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-02-01T00:00:00.000"    },
{ L_, 0, 3, F,    2,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0002-01-01T00:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0, 999, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0, 999,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.999"    },
{ L_, 0, 3, F,    1,  1,  1,  0,  0, 59,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:59.000"    },
{ L_, 0, 3, F,    1,  1,  1,  0, 59,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:59:00.000"    },
{ L_, 0, 3, F,    1,  1,  1, 23,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T23:00:00.000"    },
{ L_, 0, 3, F,    1,  1, 31,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-31T00:00:00.000"    },
{ L_, 0, 3, F,    1, 12,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-12-01T00:00:00.000"    },
{ L_, 0, 3, F, 9999,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "9999-01-01T00:00:00.000"    },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0, TXT,
                                         "\x17" "9999-12-31T23:59:59.999"    },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999, TXT,
                                         "\x17" "9999-12-31T23:59:59.999"    },
{ L_, 0, 3, F, 9999, 12, 31, 24,  0,  0,   0,   0, TXT,
                                         "\x17" "9999-12-31T24:00:00.000"    },
{ L_, 0, 3, F,    1,  1,  1, 24,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T24:00:00.000"    },

{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000000" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   1, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000001" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   1,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.001000" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  1,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:01.000000" },
{ L_, 0, 6, F,    1,  1,  1,  0,  1,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:01:00.000000" },
{ L_, 0, 6, F,    1,  1,  1,  1,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T01:00:00.000000" },
{ L_, 0, 6, F,    1,  1,  2,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-02T00:00:00.000000" },
{ L_, 0, 6, F,    1,  2,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-02-01T00:00:00.000000" },
{ L_, 0, 6, F,    2,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0002-01-01T00:00:00.000000" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0, 999, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000999" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0, 999,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.999000" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0, 59,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:59.000000" },
{ L_, 0, 6, F,    1,  1,  1,  0, 59,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:59:00.000000" },
{ L_, 0, 6, F,    1,  1,  1, 23,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T23:00:00.000000" },
{ L_, 0, 6, F,    1,  1, 31,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-31T00:00:00.000000" },
{ L_, 0, 6, F,    1, 12,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-12-01T00:00:00.000000" },
{ L_, 0, 6, F, 9999,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "9999-01-01T00:00:00.000000" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0, TXT,
                                         "\x1A" "9999-12-31T23:59:59.999000" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999, TXT,
                                         "\x1A" "9999-12-31T23:59:59.999999" },
{ L_, 0, 6, F, 9999, 12, 31, 24,  0,  0,   0,   0, TXT,
                                         "\x1A" "9999-12-31T24:00:00.000000" },
{ L_, 0, 6, F,    1,  1,  1, 24,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T24:00:00.000000" },

{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   1, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   1,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  1,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, 0, 3, T,    1,  1,  1,  0,  1,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, 0, 3, T,    1,  1,  1,  1,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, 0, 3, T,    1,  1,  2,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, 0, 3, T,    1,  2,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, 0, 3, T,    2,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0, 999, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0, 999,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0, 59,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, 0, 3, T,    1,  1,  1,  0, 59,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, 0, 3, T,    1,  1,  1, 23,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, 0, 3, T,    1,  1, 31,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, 0, 3, T,    1, 12,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, 0, 3, T, 9999,  1,  1,  0,  0,  0,   0,   0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 24,  0,  0,   0,   0, BIN,
                                           "09  00 00  00 E5 08 6E 92 98 00" },
{ L_, 0, 3, T,    1,  1,  1, 24,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },

{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   1, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   1,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  1,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, 0, 6, T,    1,  1,  1,  0,  1,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, 0, 6, T,    1,  1,  1,  1,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, 0, 6, T,    1,  1,  2,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, 0, 6, T,    1,  2,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, 0, 6, T,    2,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0, 999, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0, 999,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0, 59,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, 0, 6, T,    1,  1,  1,  0, 59,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, 0, 6, T,    1,  1,  1, 23,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, 0, 6, T,    1,  1, 31,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, 0, 6, T,    1, 12,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, 0, 6, T, 9999,  1,  1,  0,  0,  0,   0,   0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 24,  0,  0,   0,   0, BIN,
                                           "09  00 00  00 E5 08 6E 92 98 00" },
{ L_, 0, 6, T,    1,  1,  1, 24,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },

{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0,   1, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   1,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.001"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  1,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:01.000"    },
{ L_, V, 3, F,    1,  1,  1,  0,  1,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:01:00.000"    },
{ L_, V, 3, F,    1,  1,  1,  1,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T01:00:00.000"    },
{ L_, V, 3, F,    1,  1,  2,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-02T00:00:00.000"    },
{ L_, V, 3, F,    1,  2,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-02-01T00:00:00.000"    },
{ L_, V, 3, F,    2,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0002-01-01T00:00:00.000"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0, 999, TXT,
                                         "\x17" "0001-01-01T00:00:00.000"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0, 999,   0, TXT,
                                         "\x17" "0001-01-01T00:00:00.999"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0, 59,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:00:59.000"    },
{ L_, V, 3, F,    1,  1,  1,  0, 59,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T00:59:00.000"    },
{ L_, V, 3, F,    1,  1,  1, 23,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T23:00:00.000"    },
{ L_, V, 3, F,    1,  1, 31,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-31T00:00:00.000"    },
{ L_, V, 3, F,    1, 12,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-12-01T00:00:00.000"    },
{ L_, V, 3, F, 9999,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x17" "9999-01-01T00:00:00.000"    },
{ L_, V, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0, TXT,
                                         "\x17" "9999-12-31T23:59:59.999"    },
{ L_, V, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999, TXT,
                                         "\x17" "9999-12-31T23:59:59.999"    },
{ L_, V, 3, F, 9999, 12, 31, 24,  0,  0,   0,   0, TXT,
                                         "\x17" "9999-12-31T24:00:00.000"    },
{ L_, V, 3, F,    1,  1,  1, 24,  0,  0,   0,   0, TXT,
                                         "\x17" "0001-01-01T24:00:00.000"    },

{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000000" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   1, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000001" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   1,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.001000" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  1,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:01.000000" },
{ L_, V, 6, F,    1,  1,  1,  0,  1,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:01:00.000000" },
{ L_, V, 6, F,    1,  1,  1,  1,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T01:00:00.000000" },
{ L_, V, 6, F,    1,  1,  2,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-02T00:00:00.000000" },
{ L_, V, 6, F,    1,  2,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-02-01T00:00:00.000000" },
{ L_, V, 6, F,    2,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0002-01-01T00:00:00.000000" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0, 999, TXT,
                                         "\x1A" "0001-01-01T00:00:00.000999" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0, 999,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:00.999000" },
{ L_, V, 6, F,    1,  1,  1,  0,  0, 59,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:00:59.000000" },
{ L_, V, 6, F,    1,  1,  1,  0, 59,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T00:59:00.000000" },
{ L_, V, 6, F,    1,  1,  1, 23,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T23:00:00.000000" },
{ L_, V, 6, F,    1,  1, 31,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-31T00:00:00.000000" },
{ L_, V, 6, F,    1, 12,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-12-01T00:00:00.000000" },
{ L_, V, 6, F, 9999,  1,  1,  0,  0,  0,   0,   0, TXT,
                                         "\x1A" "9999-01-01T00:00:00.000000" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0, TXT,
                                         "\x1A" "9999-12-31T23:59:59.999000" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999, TXT,
                                         "\x1A" "9999-12-31T23:59:59.999999" },
{ L_, V, 6, F, 9999, 12, 31, 24,  0,  0,   0,   0, TXT,
                                         "\x1A" "9999-12-31T24:00:00.000000" },
{ L_, V, 6, F,    1,  1,  1, 24,  0,  0,   0,   0, TXT,
                                         "\x1A" "0001-01-01T24:00:00.000000" },

{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   1, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   1,   0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  1,   0,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, V, 3, T,    1,  1,  1,  0,  1,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, V, 3, T,    1,  1,  1,  1,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, V, 3, T,    1,  1,  2,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, V, 3, T,    1,  2,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, V, 3, T,    2,  1,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0, 999, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0, 999,   0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, V, 3, T,    1,  1,  1,  0,  0, 59,   0,   0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, V, 3, T,    1,  1,  1,  0, 59,  0,   0,   0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, V, 3, T,    1,  1,  1, 23,  0,  0,   0,   0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, V, 3, T,    1,  1, 31,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, V, 3, T,    1, 12,  1,  0,  0,  0,   0,   0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, V, 3, T, 9999,  1,  1,  0,  0,  0,   0,   0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 24,  0,  0,   0,   0, BIN,
                                       "0A  80 00  37 B9 DA  14 1D D7 60 00" },
{ L_, V, 3, T,    1,  1,  1, 24,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  14 1D D7 60 00" },

{ L_, V, 6, T,   1,   1,  1,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   1, BIN,
                                       "0A  80 00  00 00 00  00 00 00 00 01" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   1,   0, BIN,
                                       "0A  80 00  00 00 00  00 00 00 03 E8" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  1,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 00 0F 42 40" },
{ L_, V, 6, T,    1,  1,  1,  0,  1,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 03 93 87 00" },
{ L_, V, 6, T,    1,  1,  1,  1,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 D6 93 A4 00" },
{ L_, V, 6, T,    1,  1,  2,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 01  00 00 00 00 00" },
{ L_, V, 6, T,    1,  2,  1,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 1F  00 00 00 00 00" },
{ L_, V, 6, T,    2,  1,  1,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 01 6D  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0, 999, BIN,
                                       "0A  80 00  00 00 00  00 00 00 03 E7" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0, 999,   0, BIN,
                                       "0A  80 00  00 00 00  00 00 0F 3E 58" },
{ L_, V, 6, T,    1,  1,  1,  0,  0, 59,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 03 84 44 C0" },
{ L_, V, 6, T,    1,  1,  1,  0, 59,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  00 D3 00 1D 00" },
{ L_, V, 6, T,    1,  1,  1, 23,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  13 47 43 BC 00" },
{ L_, V, 6, T,    1,  1, 31,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 1E  00 00 00 00 00" },
{ L_, V, 6, T,    1, 12,  1,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 01 4E  00 00 00 00 00" },
{ L_, V, 6, T, 9999,  1,  1,  0,  0,  0,   0,   0, BIN,
                                       "0A  80 00  37 B8 6E  00 00 00 00 00" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0, BIN,
                                       "0A  80 00  37 B9 DA  14 1D D7 5C 18" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999, BIN,
                                       "0A  80 00  37 B9 DA  14 1D D7 5F FF" },
{ L_, V, 6, T, 9999, 12, 31, 24,  0,  0,   0,   0, BIN,
                                       "0A  80 00  37 B9 DA  14 1D D7 60 00" },
{ L_, V, 6, T,    1,  1,  1, 24,  0,  0,   0,   0, BIN,
                                       "0A  80 00  00 00 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  VERSION   = DATA[i].d_bdeVersionConformance;
                const int  PRECISION = DATA[i].d_fractionalSecondPrecision;
                const bool BINARY = DATA[i].d_encodeDateAndTimeTypesAsBinary;
                const int YEAR = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY = DATA[i].d_day;
                const int  HOUR   = DATA[i].d_hour;
                const int  MINUTE = DATA[i].d_minute;
                const int  SECOND = DATA[i].d_second;
                const int  MILLISECOND       = DATA[i].d_millisecond;
                const int  MICROSECOND       = DATA[i].d_microsecond;
                const ExpFormat   EXP_FORMAT = DATA[i].d_expFormat;
                const char *const EXP        = DATA[i].d_exp;

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                bdlsb::MemOutStreamBuf outStreamBuf;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(BINARY);
                options.setDatetimeFractionalSecondPrecision(PRECISION);

                int rc = Util::putValue(&outStreamBuf, DATETIME, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                char buffer[k_MAX_BUFFER_SIZE];
                int bufferSize = 0;

                switch (EXP_FORMAT) {
                  case BIN: {
                    rc = u::ByteBufferUtil::loadBuffer(
                        &bufferSize,
                        buffer,
                        sizeof(buffer),
                        EXP,
                        static_cast<int>(bsl::strlen(EXP)));
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc)
                        continue;  // CONTINUE
                  } break;
                  case TXT: {
                    bsl::strncpy(buffer, EXP, sizeof(buffer));
                    bufferSize = static_cast<int>(bsl::strlen(buffer));
                  } break;
                }

                LOOP1_ASSERT_EQ(LINE,
                                outStreamBuf.length(),
                                static_cast<bsl::size_t>(bufferSize));
                if (outStreamBuf.length() !=
                    static_cast<bsl::size_t>(bufferSize)) continue; // CONTINUE

                const bool BUFFERS_EQUAL = bsl::equal(
                    buffer, buffer + bufferSize, outStreamBuf.data());
                LOOP1_ASSERT(LINE, BUFFERS_EQUAL);
                if (!BUFFERS_EQUAL && veryVerbose && BINARY) {
                    bsl::cout << "ACTUAL: [";
                    bdlb::Print::singleLineHexDump(
                        bsl::cout,
                        outStreamBuf.data(),
                        static_cast<int>(outStreamBuf.length()));
                    bsl::cout << "]" << bsl::endl;
                }
                else if (!BUFFERS_EQUAL && veryVerbose) {
                    bsl::cout << "ACTUAL: ["
                              << bslstl::StringRef(outStreamBuf.data(),
                                                   outStreamBuf.length())
                              << "]"
                              << bsl::endl;
                }

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());

                bdlt::Datetime datetime;
                int            accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &inStreamBuf, &datetime, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                if (DATETIME.time() == bdlt::Time() && VERSION < V && BINARY) {
                    LOOP1_ASSERT_EQ(LINE, datetime.year(), DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, datetime.month(), DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, datetime.day(), DATETIME.day());
                    LOOP1_ASSERT_EQ(LINE, datetime.hour(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.minute(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.second(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.millisecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.microsecond(), 0);
                }
                else if ((VERSION < V && BINARY) || 3 == PRECISION) {
                    LOOP1_ASSERT_EQ(LINE, datetime.year(), DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, datetime.month(), DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, datetime.day(), DATETIME.day());
                    LOOP1_ASSERT_EQ(
                        LINE, datetime.second(), DATETIME.second());
                    LOOP1_ASSERT_EQ(
                        LINE, datetime.millisecond(), DATETIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, datetime.microsecond(), 0);
                }
                else {
                    LOOP1_ASSERT_EQ(LINE, datetime, DATETIME);
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::DatetimeTz' FORMAT SELECTION"
                      << bsl::endl
                      << "-------------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 38;

            static const int V =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;

            static const bool T = true;
            static const bool F = false;

            enum ExpFormat {
                BIN,
                TXT
            };

            static const struct {
                int d_line;
                int d_bdeVersionConformance;
                int d_fractionalSecondPrecision;
                bool d_encodeDateAndTimeTypesAsBinary;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
                ExpFormat d_expFormat;
                const char *d_exp;
            } DATA[] = {
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   0,     1, TXT,
                                      "\x1D" "0001-01-01T00:00:00.000+00:01" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   0,    -1, TXT,
                                      "\x1D" "0001-01-01T00:00:00.000-00:01" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0,   1,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   1,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:00.001+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  1,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:01.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  1,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:01:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  1,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T01:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  2,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-02T00:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  2,  1,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-02-01T00:00:00.000+00:00" },
{ L_, 0, 3, F,    2,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0002-01-01T00:00:00.000+00:00" },
{ L_, 0, 3, F,    2,  1,  1,  0,  0,  0,   0,   0,  1439, TXT,
                                      "\x1D" "0002-01-01T00:00:00.000+23:59" },
{ L_, 0, 3, F,    2,  1,  1,  0,  0,  0,   0,   0, -1439, TXT,
                                      "\x1D" "0002-01-01T00:00:00.000-23:59" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0,   0, 999,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0,  0, 999,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:00.999+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0,  0, 59,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:00:59.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1,  0, 59,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T00:59:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1, 23,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T23:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1, 31,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-31T00:00:00.000+00:00" },
{ L_, 0, 3, F,    1, 12,  1,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-12-01T00:00:00.000+00:00" },
{ L_, 0, 3, F, 9999,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "9999-01-01T00:00:00.000+00:00" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0,     0, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999+00:00" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999+23:59" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999-23:59" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999,     0, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999+00:00" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999+23:59" },
{ L_, 0, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, TXT,
                                      "\x1D" "9999-12-31T23:59:59.999-23:59" },
{ L_, 0, 3, F, 9999, 12, 31, 24,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "9999-12-31T24:00:00.000+00:00" },
{ L_, 0, 3, F,    1,  1,  1, 24,  0,  0,   0,   0,     0, TXT,
                                      "\x1D" "0001-01-01T24:00:00.000+00:00" },

{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,     1, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+00:01" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,    -1, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000-00:01" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   1,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000001+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   1,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.001000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  1,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:01.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  1,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:01:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  1,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T01:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  2,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-02T00:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  2,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-02-01T00:00:00.000000+00:00" },
{ L_, 0, 6, F,    2,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0002-01-01T00:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,  1439, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+23:59" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0,   0, -1439, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000-23:59" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0,   0, 999,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000999+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0,  0, 999,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.999000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0,  0, 59,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:59.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1,  0, 59,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:59:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1, 23,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T23:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1, 31,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-31T00:00:00.000000+00:00" },
{ L_, 0, 6, F,    1, 12,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-12-01T00:00:00.000000+00:00" },
{ L_, 0, 6, F, 9999,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "9999-01-01T00:00:00.000000+00:00" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0,     0, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000+00:00" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000+23:59" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000-23:59" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999,     0, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999+00:00" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999+23:59" },
{ L_, 0, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999-23:59" },
{ L_, 0, 6, F, 9999, 12, 31, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "9999-12-31T24:00:00.000000+00:00" },
{ L_, 0, 6, F,    1,  1,  1, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T24:00:00.000000+00:00" },

{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,     1, BIN,
                                           "08  00 01     C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,    -1, BIN,
                                           "08  FF FF     C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0,   1,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   1,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  1,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, 0, 3, T,    1,  1,  1,  0,  1,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, 0, 3, T,    1,  1,  1,  1,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, 0, 3, T,    1,  1,  2,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, 0, 3, T,    1,  2,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, 0, 3, T,    2,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, 0, 3, T,    2,  1,  1,  0,  0,  0,   0,   0,  1439, BIN,
                                           "08  05 9F     C6 14 E7 1D 6C 00" },
{ L_, 0, 3, T,    2,  1,  1,  0,  0,  0,   0,   0, -1439, BIN,
                                           "08  FA 61     C6 14 E7 1D 6C 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0,   0, 999,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0,  0, 999,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, 0, 3, T,    1,  1,  1,  0,  0, 59,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, 0, 3, T,    1,  1,  1,  0, 59,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, 0, 3, T,    1,  1,  1, 23,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, 0, 3, T,    1,  1, 31,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, 0, 3, T,    1, 12,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, 0, 3, T, 9999,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 3, T, 9999, 12, 31, 24,  0,  0,   0,   0,     0, BIN,
                                           "09  00 00  00 E5 08 6E 92 98 00" },
{ L_, 0, 3, T,    1,  1,  1, 24,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },

{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,     1, BIN,
                                           "08  00 01     C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,    -1, BIN,
                                           "08  FF FF     C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0,   1,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   1,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  1,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, 0, 6, T,    1,  1,  1,  0,  1,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, 0, 6, T,    1,  1,  1,  1,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, 0, 6, T,    1,  1,  2,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, 0, 6, T,    1,  2,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, 0, 6, T,    2,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, 0, 6, T,    2,  1,  1,  0,  0,  0,   0,   0,  1439, BIN,
                                           "08  05 9F     C6 14 E7 1D 6C 00" },
{ L_, 0, 6, T,    2,  1,  1,  0,  0,  0,   0,   0, -1439, BIN,
                                           "08  FA 61     C6 14 E7 1D 6C 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0,   0, 999,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0,  0, 999,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, 0, 6, T,    1,  1,  1,  0,  0, 59,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, 0, 6, T,    1,  1,  1,  0, 59,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, 0, 6, T,    1,  1,  1, 23,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, 0, 6, T,    1,  1, 31,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, 0, 6, T,    1, 12,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, 0, 6, T, 9999,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, 0, 6, T, 9999, 12, 31, 24,  0,  0,   0,   0,     0, BIN,
                                           "09  00 00  00 E5 08 6E 92 98 00" },
{ L_, 0, 6, T,    1,  1,  1, 24,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },

{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0,   1,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   1,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:00.001+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  1,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:01.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  1,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:01:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  1,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T01:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  2,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-02T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  2,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-02-01T00:00:00.000+00:00"    },
{ L_, V, 3, F,    2,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0002-01-01T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0,   0, 999,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0,  0, 999,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:00.999+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0,  0, 59,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:00:59.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1,  0, 59,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T00:59:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1, 23,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T23:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1, 31,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-31T00:00:00.000+00:00"    },
{ L_, V, 3, F,    1, 12,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-12-01T00:00:00.000+00:00"    },
{ L_, V, 3, F, 9999,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "9999-01-01T00:00:00.000+00:00"    },
{ L_, V, 3, F, 9999, 12, 31, 23, 59, 59, 999,   0,     0, TXT,
                                   "\x1D" "9999-12-31T23:59:59.999+00:00"    },
{ L_, V, 3, F, 9999, 12, 31, 23, 59, 59, 999, 999,     0, TXT,
                                   "\x1D" "9999-12-31T23:59:59.999+00:00"    },
{ L_, V, 3, F, 9999, 12, 31, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "9999-12-31T24:00:00.000+00:00"    },
{ L_, V, 3, F,    1,  1,  1, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x1D" "0001-01-01T24:00:00.000+00:00"    },

{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,     1, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+00:01" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,    -1, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000-00:01" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   1,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000001+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   1,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.001000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  1,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:01.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  1,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:01:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  1,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T01:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  2,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-02T00:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  2,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-02-01T00:00:00.000000+00:00" },
{ L_, V, 6, F,    2,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0002-01-01T00:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0,  1439, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000+23:59" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0,   0, -1439, TXT,
                                   "\x20" "0001-01-01T00:00:00.000000-23:59" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0,   0, 999,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.000999+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0,  0, 999,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:00.999000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0,  0, 59,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:00:59.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1,  0, 59,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T00:59:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1, 23,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T23:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  1, 31,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-31T00:00:00.000000+00:00" },
{ L_, V, 6, F,    1, 12,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-12-01T00:00:00.000000+00:00" },
{ L_, V, 6, F, 9999,  1,  1,  0,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "9999-01-01T00:00:00.000000+00:00" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0,     0, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000+00:00" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000+23:59" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999000-23:59" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999,     0, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999+00:00" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999+23:59" },
{ L_, V, 6, F, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, TXT,
                                   "\x20" "9999-12-31T23:59:59.999999-23:59" },
{ L_, V, 6, F, 9999, 12, 31, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "9999-12-31T24:00:00.000000+00:00" },
{ L_, V, 6, F,    1,  1,  1, 24,  0,  0,   0,   0,     0, TXT,
                                   "\x20" "0001-01-01T24:00:00.000000+00:00" },

{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,     1, BIN,
                                           "08  00 01     C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   0,    -1, BIN,
                                           "08  FF FF     C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0,   1,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   1,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 01" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  1,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E8" },
{ L_, V, 3, T,    1,  1,  1,  0,  1,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 2A 60" },
{ L_, V, 3, T,    1,  1,  1,  1,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A3 2E 80" },
{ L_, V, 3, T,    1,  1,  2,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 92 9C 00" },
{ L_, V, 3, T,    1,  2,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 2F 11 64 00" },
{ L_, V, 3, T,    2,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 E7 1D 6C 00" },
{ L_, V, 3, T,    2,  1,  1,  0,  0,  0,   0,   0,  1439, BIN,
                                           "08  05 9F     C6 14 E7 1D 6C 00" },
{ L_, V, 3, T,    2,  1,  1,  0,  0,  0,   0,   0, -1439, BIN,
                                           "08  FA 61     C6 14 E7 1D 6C 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0,   0, 999,     0, BIN,
                                                     "06  C6 0D 8F 6C 40 00" },
{ L_, V, 3, T,    1,  1,  1,  0,  0,  0, 999,   0,     0, BIN,
                                                     "06  C6 0D 8F 6C 43 E7" },
{ L_, V, 3, T,    1,  1,  1,  0,  0, 59,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F 6D 26 78" },
{ L_, V, 3, T,    1,  1,  1,  0, 59,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 8F A2 44 20" },
{ L_, V, 3, T,    1,  1,  1, 23,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0D 94 5B AD 80" },
{ L_, V, 3, T,    1,  1, 31,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 0E 29 EB 08 00" },
{ L_, V, 3, T,    1, 12,  1,  0,  0,  0,   0,   0,     0, BIN,
                                                     "06  C6 14 47 78 48 00" },
{ L_, V, 3, T, 9999,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                           "09  00 00  00 E5 01 1C 07 C8 00" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999,     0, BIN,
                                           "09  00 00  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, BIN,
                                           "09  05 9F  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, BIN,
                                           "09  FA 61  00 E5 08 73 B8 F3 FF" },
{ L_, V, 3, T, 9999, 12, 31, 24,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  37 B9 DA  14 1D D7 60 00" },
{ L_, V, 3, T,    1,  1,  1, 24,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  14 1D D7 60 00" },

{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,     1, BIN,
                                       "0A  90 01  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,    -1, BIN,
                                       "0A  9F FF  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   1,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 00 00 01" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   1,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 00 03 E8" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  1,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 0F 42 40" },
{ L_, V, 6, T,    1,  1,  1,  0,  1,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 03 93 87 00" },
{ L_, V, 6, T,    1,  1,  1,  1,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 D6 93 A4 00" },
{ L_, V, 6, T,    1,  1,  2,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 01  00 00 00 00 00" },
{ L_, V, 6, T,    1,  2,  1,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 1F  00 00 00 00 00" },
{ L_, V, 6, T,    2,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 01 6D  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   0,  1439, BIN,
                                       "0A  95 9F  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0,   0, -1439, BIN,
                                       "0A  9A 61  00 00 00  00 00 00 00 00" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0,   0, 999,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 00 03 E7" },
{ L_, V, 6, T,    1,  1,  1,  0,  0,  0, 999,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 00 0F 3E 58" },
{ L_, V, 6, T,    1,  1,  1,  0,  0, 59,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 03 84 44 C0" },
{ L_, V, 6, T,    1,  1,  1,  0, 59,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  00 D3 00 1D 00" },
{ L_, V, 6, T,    1,  1,  1, 23,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  13 47 43 BC 00" },
{ L_, V, 6, T,    1,  1, 31,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 1E  00 00 00 00 00" },
{ L_, V, 6, T,    1, 12,  1,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 01 4E  00 00 00 00 00" },
{ L_, V, 6, T, 9999,  1,  1,  0,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  37 B8 6E  00 00 00 00 00" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0,     0, BIN,
                                       "0A  90 00  37 B9 DA  14 1D D7 5C 18" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0,  1439, BIN,
                                       "0A  95 9F  37 B9 DA  14 1D D7 5C 18" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999,   0, -1439, BIN,
                                       "0A  9A 61  37 B9 DA  14 1D D7 5C 18" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999,     0, BIN,
                                       "0A  90 00  37 B9 DA  14 1D D7 5F FF" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999,  1439, BIN,
                                       "0A  95 9F  37 B9 DA  14 1D D7 5F FF" },
{ L_, V, 6, T, 9999, 12, 31, 23, 59, 59, 999, 999, -1439, BIN,
                                       "0A  9A 61  37 B9 DA  14 1D D7 5F FF" },
{ L_, V, 6, T, 9999, 12, 31, 24,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  37 B9 DA  14 1D D7 60 00" },
{ L_, V, 6, T,    1,  1,  1, 24,  0,  0,   0,   0,     0, BIN,
                                       "0A  90 00  00 00 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  VERSION   = DATA[i].d_bdeVersionConformance;
                const int  PRECISION = DATA[i].d_fractionalSecondPrecision;
                const bool BINARY = DATA[i].d_encodeDateAndTimeTypesAsBinary;
                const int  YEAR   = DATA[i].d_year;
                const int  MONTH  = DATA[i].d_month;
                const int  DAY    = DATA[i].d_day;
                const int  HOUR   = DATA[i].d_hour;
                const int  MINUTE = DATA[i].d_minute;
                const int  SECOND = DATA[i].d_second;
                const int  MILLISECOND       = DATA[i].d_millisecond;
                const int  MICROSECOND       = DATA[i].d_microsecond;
                const int  OFFSET            = DATA[i].d_offset;
                const ExpFormat   EXP_FORMAT = DATA[i].d_expFormat;
                const char *const EXP        = DATA[i].d_exp;

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                bdlsb::MemOutStreamBuf outStreamBuf;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(BINARY);
                options.setDatetimeFractionalSecondPrecision(PRECISION);

                int rc = Util::putValue(&outStreamBuf, DATETIMETZ, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                char buffer[k_MAX_BUFFER_SIZE];
                int bufferSize = 0;

                switch (EXP_FORMAT) {
                  case BIN: {
                    rc = u::ByteBufferUtil::loadBuffer(
                        &bufferSize,
                        buffer,
                        sizeof(buffer),
                        EXP,
                        static_cast<int>(bsl::strlen(EXP)));
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc)
                        continue;  // CONTINUE
                  } break;
                  case TXT: {
                    bsl::strncpy(buffer, EXP, sizeof(buffer));
                    bufferSize = static_cast<int>(bsl::strlen(buffer));
                  } break;
                }

                LOOP1_ASSERT_EQ(LINE,
                                outStreamBuf.length(),
                                static_cast<bsl::size_t>(bufferSize));
                if (outStreamBuf.length() !=
                    static_cast<bsl::size_t>(bufferSize)) continue; // CONTINUE

                const bool BUFFERS_EQUAL = bsl::equal(
                    buffer, buffer + bufferSize, outStreamBuf.data());
                LOOP1_ASSERT(LINE, BUFFERS_EQUAL);
                if (!BUFFERS_EQUAL && veryVerbose && BINARY) {
                    bsl::cout << "ACTUAL: [";
                    bdlb::Print::singleLineHexDump(
                        bsl::cout,
                        outStreamBuf.data(),
                        static_cast<int>(outStreamBuf.length()));
                    bsl::cout << "]" << bsl::endl;
                }
                else if (!BUFFERS_EQUAL && veryVerbose) {
                    bsl::cout << "ACTUAL: ["
                              << bslstl::StringRef(outStreamBuf.data(),
                                                   outStreamBuf.length())
                              << "]"
                              << bsl::endl;
                }

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());

                bdlt::DatetimeTz datetimeTz;
                int              accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &inStreamBuf, &datetimeTz, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                if (DATETIMETZ.localDatetime().time() == bdlt::Time() &&
                    VERSION < V && BINARY) {
                    const bdlt::Datetime& DATETIME = DATETIMETZ.localDatetime();
                    const bdlt::Datetime& datetime = datetimeTz.localDatetime();
                    LOOP1_ASSERT_EQ(LINE, datetime.year(), DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, datetime.month(), DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, datetime.day(), DATETIME.day());
                    LOOP1_ASSERT_EQ(LINE, datetime.hour(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.minute(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.second(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.millisecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetime.microsecond(), 0);
                    LOOP1_ASSERT_EQ(LINE, datetimeTz.offset(), 0);
                }
                else if ((VERSION < V && BINARY) || 3 == PRECISION) {
                    const bdlt::Datetime& DATETIME = DATETIMETZ.
                    localDatetime();
                    const bdlt::Datetime& datetime = datetimeTz.
                    localDatetime();
                    LOOP1_ASSERT_EQ(LINE, datetime.year(), DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, datetime.month(), DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, datetime.day(), DATETIME.day());
                    LOOP1_ASSERT_EQ(LINE, datetime.hour(), DATETIME.hour());
                    LOOP1_ASSERT_EQ(
                        LINE, datetime.minute(), DATETIME.minute());
                    LOOP1_ASSERT_EQ(
                        LINE, datetime.second(), DATETIME.second());
                    LOOP1_ASSERT_EQ(
                        LINE, datetime.millisecond(), DATETIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, datetime.microsecond(), 0);
                    LOOP1_ASSERT_EQ(
                        LINE, datetimeTz.offset(), DATETIMETZ.offset());
                }
                else {
                    LOOP1_ASSERT_EQ(LINE, datetimeTz, DATETIMETZ);
                }
            }
        }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING EXTENDED BINARY DATE/TIME FORMAT
        //   This case tests the essential behavior of
        //   'balber::BerUtil::putValue' and 'balber::BerUtil::getValue' for
        //   the extended-binary representation of date and time types.
        //
        // Concerns:
        //: 1 The encoded representation of all date and time types in the
        //:   extended binary representation is correct w.r.t. the
        //:   specification of the extended-binary representation.
        //:
        //: 2 Any error to write a character to the stream buffer during
        //:   encoding an extended-binary formatted date or time type
        //:   is propagated to the caller
        //:
        //: 3 When a decoding operation has proceeded for enough to determine
        //:   it will decode using the extended-binary representation,
        //:   encountering any bit pattern that is not a valid extended-binary
        //:   representation causes the decoding operation to fail
        //:
        //: 4 The composition of decoding with encoding with the compact-binary
        //:   representation is an isomorphism for all date and time types.
        //:
        //: 5 Decoding a 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' from a
        //:   compact-binary encoded 'bdlt::Time' loads the expected value to
        //:   the 'bdlt::Time' selection of the variant.
        //:
        //: 6 Decoding a 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' from a
        //:   compact-binary encoded 'bdlt::TImeTz' loads the expected value to
        //:   the 'bdlt::TimeTz' selection of the variant.
        //:
        //: 7 Decoding a
        //:   'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>' from a
        //:   compact-binary encoded 'bdlt::Datetime' loads the expected value
        //:   to the 'bdlt::Datetime' selection of the variant.
        //:
        //: 8 Decoding a
        //:   'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>' from a
        //:   compact-binary encoded 'bdlt::DatetimeTz' loads the expected
        //:   value to the 'bdlt::DatetimeTz' selection of the variant.
        //
        // Plan:
        //: 1 Verify that the encoded representation of boundary 'bdlt::Time',
        //:   'bdlt::TimeTz', 'bdlt::Datetime', and 'bdlt::DatetimeTz'
        //:   values all conform to the specification of the compact-binary
        //:   time representation when that representation is selected for use.
        //:
        //: 2 Verify that encoding extended-binary representations of boundary
        //:   'bdlt::Time', 'bdlt::TimeTz', 'bdlt::Datetime', and
        //:   'bdlt::DatetimeTz' values to fixed-size buffers that are too
        //:   small always results in 'putValue' returning a non-zero status.
        //:
        //: 3 Verify that decoding many invalid bit-patterns as extended-binary
        //:   representations of a 'bdlt::Time', 'bdlt::TimeTz',
        //:   'bdlt::Datetime', or 'bdlt::DatetimeTz' always results in
        //:   'getValue' returning a non-zero status.
        //:
        //: 4 Verify, for a large swatch of values, that decoding the encoding
        //:   of a 'bdlt::Time', 'bdlt::TimeTz', 'bdlt::Datetime', or
        //:   'bdlt::DatetimeTz' value in the extended-binary representation
        //:   results in exactly that same value that was encoded.
        //:   In essence, verify that 'getValue' composed with 'putValue' is
        //:   an isomorphism when the extended-binary representation is chosen.
        //
        // Testing:
        //   CONCERN: 'put'- & 'getValue' for date/time types in ext-bin fmt
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING EXTENDED BINARY DATE/TIME FORMAT"
                      << bsl::endl
                      << "========================================"
                      << bsl::endl;

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Time' ENCODING"
                      << bsl::endl
                      << "-----------------------------"
                      << bsl::endl;
        {
            static const struct {
                int         d_line;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_microsecond;
                const char *d_exp;
            } DATA[] = {
                { L_,  0,  0,  0,   0,   0, "07  80 00  00 00 00 00 00" },
                { L_,  0,  0,  0,   0,   1, "07  80 00  00 00 00 00 01" },
                { L_,  0,  0,  0,   1,   0, "07  80 00  00 00 00 03 E8" },
                { L_,  0,  0,  1,   0,   0, "07  80 00  00 00 0F 42 40" },
                { L_,  0,  1,  0,   0,   0, "07  80 00  00 03 93 87 00" },
                { L_,  1,  0,  0,   0,   0, "07  80 00  00 D6 93 A4 00" },

                { L_,  0,  0,  0,   0, 999, "07  80 00  00 00 00 03 E7" },
                { L_,  0,  0,  0, 999,   0, "07  80 00  00 00 0F 3E 58" },
                { L_,  0,  0, 59,   0,   0, "07  80 00  00 03 84 44 C0" },
                { L_,  0, 59,  0,   0,   0, "07  80 00  00 D3 00 1D 00" },
                { L_, 23,  0,  0,   0,   0, "07  80 00  13 47 43 BC 00" },

                { L_, 23, 59, 59, 999, 999, "07  80 00  14 1D D7 5F FF" },
                { L_, 24,  0,  0,   0,   0, "07  80 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            const balber::BerEncoderOptions& OPTIONS = options;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const char *const EXP         = DATA[i].d_exp;

                // First, verify that the specified time value encodes to the
                // specified byte array.

                const bdlt::Time TIME(HOUR,
                                      MINUTE,
                                      SECOND,
                                      MILLISECOND,
                                      MICROSECOND);

                static const int NUM_BYTES = 8;

                bdlsb::MemOutStreamBuf timeOutBuffer;
                const bool             timeEncodesToBytes =
                    u::TestUtil::valueEncodesToBytes(
                        cout, &timeOutBuffer, TIME, OPTIONS, EXP, NUM_BYTES);
                LOOP1_ASSERT(LINE, timeEncodesToBytes);
                if (!timeEncodesToBytes) {
                    continue;
                }

                const bool bytesDecodeToTime =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    timeOutBuffer.data(),
                                                    timeOutBuffer.length(),
                                                    TIME,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToTime);
                if (!bytesDecodeToTime) {
                    continue;
                }

                const bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TIME_OR_TIMETZ(
                                                                         TIME);

                const bool bytesDecodeToVariant =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    timeOutBuffer.data(),
                                                    timeOutBuffer.length(),
                                                    TIME_OR_TIMETZ,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToVariant);
                if (!bytesDecodeToVariant) {
                    continue;
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Time' ENCODING FAILURE MODES"
                      << bsl::endl
                      << "-------------------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
            } DATA[] = {
                { L_,  0,  0,  0,   0,   0 },
                { L_,  0,  0,  0,   0,   1 },
                { L_,  0,  0,  0,   1,   0 },
                { L_,  0,  0,  1,   0,   0 },
                { L_,  0,  1,  0,   0,   0 },
                { L_,  1,  0,  0,   0,   0 },
                { L_,  1,  1,  1,   1,   1 },
                { L_,  0,  0,  0,   0, 999 },
                { L_,  0,  0,  0, 999,   0 },
                { L_,  0,  0, 59,   0,   0 },
                { L_,  0, 59,  0,   0,   0 },
                { L_, 23,  0,  0,   0,   0 },
                { L_, 23, 59, 59, 999,   0 },
                { L_, 23, 59, 59, 999, 999 },
                { L_, 24,  0,  0,   0,   0 }
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                const int HOUR = DATA[i].d_hour;
                const int MINUTE = DATA[i].d_minute;
                const int SECOND = DATA[i].d_second;
                const int MILLISECOND = DATA[i].d_millisecond;
                const int MICROSECOND = DATA[i].d_microsecond;

                const bdlt::Time TIME(HOUR        ,
                                      MINUTE      ,
                                      SECOND      ,
                                      MILLISECOND ,
                                      MICROSECOND);

                bsl::vector<char> timeStreamBufPutArea;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(true);
                options.setDatetimeFractionalSecondPrecision(6);

                int timeStreamBufPutAreaSize = 0;
                int rc = 0;

                do {
                    timeStreamBufPutArea.clear();
                    timeStreamBufPutArea.resize(timeStreamBufPutAreaSize);

                    bdlsb::FixedMemOutStreamBuf timeStreamBuf(
                        timeStreamBufPutArea.data(),
                        timeStreamBufPutArea.size());

                    rc = Util::putValue(&timeStreamBuf, TIME, &options);
                    if (0 != rc) {
                        timeStreamBufPutAreaSize++;
                    }
                } while (0 != rc);

                LOOP1_ASSERT_EQ(LINE, 8, timeStreamBufPutAreaSize);
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Time' DECODING FAILURE MODES"
                      << bsl::endl
                      << "-------------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 9;

            static const struct {
                int         d_line;
                const char *d_exp;
                bool        d_success;
            } DATA[] = {
                { L_, "01  80"                       , false },
                { L_, "02  80 00"                    , false },
                { L_, "03  80 00  00"                , false },
                { L_, "04  80 00  00 00"             , false },
                { L_, "05  80 00  00 00 00"          , false },
                { L_, "06  80 00  00 00 00 00"       , false },
                { L_, "07  80 00  00 00 00 00 00"    , true  },
                { L_, "08  80 00  00 00 00 00 00  00", false },
                    // Verify that the decode operation requires the value to
                    // be at exactly 7 octets long (not counting the 1 byte
                    // length prefix).

                { L_, "07  7F FF  00 00 00 00 00"    , false },
                { L_, "07  80 01  00 00 00 00 00"    , false },
                    // Verify that off-by-1 numbers in the header cause
                    // decoding failures.

                { L_, "07  00 00  00 00 00 00 00"    , false },
                { L_, "07  10 00  00 00 00 00 00"    , false },
                { L_, "07  20 00  00 00 00 00 00"    , false },
                { L_, "07  30 00  00 00 00 00 00"    , false },
                { L_, "07  40 00  00 00 00 00 00"    , false },
                { L_, "07  50 00  00 00 00 00 00"    , false },
                { L_, "07  60 00  00 00 00 00 00"    , false },
                { L_, "07  70 00  00 00 00 00 00"    , false },
                { L_, "07  80 00  00 00 00 00 00"    , true  },
                { L_, "07  90 00  00 00 00 00 00"    , true  },
                { L_, "07  A0 00  00 00 00 00 00"    , false },
                { L_, "07  B0 00  00 00 00 00 00"    , false },
                { L_, "07  C0 00  00 00 00 00 00"    , false },
                { L_, "07  D0 00  00 00 00 00 00"    , false },
                { L_, "07  E0 00  00 00 00 00 00"    , false },
                { L_, "07  F0 00  00 00 00 00 00"    , false },
                    // Verify that the decode operation requires the control
                    // nibble field of the header to be '0b1000'.

                { L_, "07  80 01  00 00 00 00 00"    , false },
                { L_, "07  80 02  00 00 00 00 00"    , false },
                { L_, "07  80 04  00 00 00 00 00"    , false },
                { L_, "07  80 08  00 00 00 00 00"    , false },
                { L_, "07  80 10  00 00 00 00 00"    , false },
                { L_, "07  80 20  00 00 00 00 00"    , false },
                { L_, "07  80 40  00 00 00 00 00"    , false },
                { L_, "07  80 80  00 00 00 00 00"    , false },
                { L_, "07  81 00  00 00 00 00 00"    , false },
                { L_, "07  82 00  00 00 00 00 00"    , false },
                { L_, "07  84 00  00 00 00 00 00"    , false },
                { L_, "07  88 00  00 00 00 00 00"    , false },
                    // Verify that the decode operation requires the 12-bit
                    // timezone field of the header to be 0.

                { L_, "07  80 00  14 1D D7 60 00"    , true  },
                { L_, "07  80 00  14 1D D7 60 01"    , false },
                { L_, "07  80 00  FF FF FF FF FF"    , false },
                    // Verify that the decode operation requires the number
                    // of milliseconds since midnight to be a value less
                    // than or equal to the number of microseconds in 24 hours.
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_line;
                const char *const EXP     = DATA[i].d_exp;
                const bool        SUCCESS = DATA[i].d_success;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE


                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlt::Time time;
                    int        accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &time, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << time << bsl::endl;
                        }
                    }
                }

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> timeOrTimeTz;
                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &timeOrTimeTz, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << timeOrTimeTz
                                      << bsl::endl;
                        }
                    }
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Time' ENCODING/DECODING ISOMORPHISM"
                      << bsl::endl
                      << "--------------------------------------------------"
                      << bsl::endl;
        {
            // Note that the special case value of hour 24 is tested
            // explicitly above.
            static const int HOURS[] = {0,  1,  2,  3, 20, 21, 22, 23};

            static const int NUM_HOURS = sizeof(HOURS) / sizeof(HOURS[0]);

            static const int MINUTES[] = {
                0, 1, 2, 3, 32, 33, 57, 58, 59};

            static const int NUM_MINUTES = sizeof(MINUTES) / sizeof(MINUTES[0]);

            static const int SECONDS[] = {
                0, 1, 2, 3, 4, 5, 31, 32, 33, 57, 58, 59};

            static const int NUM_SECONDS = sizeof(SECONDS) / sizeof(SECONDS[0]);

            static const int MILLISECONDS[] = {
                0,   1,   2,   3,   4,   5,   6,   7,   8,   15,
                511, 512, 513, 997, 998, 999};

            static const int NUM_MILLISECONDS = sizeof(MILLISECONDS) /
                    sizeof(MILLISECONDS[0]);

            static const int MICROSECONDS[] = {
                0,   1,   2,   3,   4,   5,   6,   7,   8,   15,
                511, 512, 513, 997, 998, 999};

            static const int NUM_MICROSECONDS = sizeof(MICROSECONDS)
                    / sizeof(MICROSECONDS[0]);

            static const int NUM_DATA = NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                                        NUM_MILLISECONDS * NUM_MICROSECONDS;

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() = u::TestDataUtil::
            k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            bdlsb::MemOutStreamBuf     outStreamBuf;
            bdlsb::FixedMemInStreamBuf inStreamBuf(0, 0);

            bdlt::Time                               time;
            bdlb::Variant2<bdlt::Time, bdlt::TimeTz> timeOrTimeTz;
            int                                      accumNumBytesConsumed = 0;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE = i;

                const int HOUR =
                    HOURS[i / (NUM_MINUTES * NUM_SECONDS * NUM_MILLISECONDS *
                               NUM_MICROSECONDS)];

                const int MINUTE =
                    MINUTES[(i / (NUM_SECONDS * NUM_MILLISECONDS *
                                  NUM_MICROSECONDS)) %
                            NUM_MINUTES];

                const int SECOND =
                    SECONDS[(i / (NUM_MILLISECONDS * NUM_MICROSECONDS)) %
                            NUM_SECONDS];

                const int MILLISECOND =
                    MILLISECONDS[(i / NUM_MICROSECONDS) % NUM_MILLISECONDS];

                const int MICROSECOND = MICROSECONDS[i % NUM_MICROSECONDS];

                const bdlt::Time TIME(HOUR        ,
                                      MINUTE      ,
                                      SECOND      ,
                                      MILLISECOND ,
                                      MICROSECOND);

                outStreamBuf.reset();
                int rc = Util::putValue(&outStreamBuf, TIME, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(
                        &inStreamBuf, &time, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);

                    LOOP1_ASSERT_EQ(LINE, TIME, time);
                    LOOP1_ASSERT_EQ(LINE, HOUR, time.hour());
                    LOOP1_ASSERT_EQ(LINE, MINUTE, time.minute());
                    LOOP1_ASSERT_EQ(LINE, SECOND, time.second());
                    LOOP1_ASSERT_EQ(LINE, MILLISECOND, time.millisecond());
                    LOOP1_ASSERT_EQ(LINE, MICROSECOND, time.microsecond());
                }

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(
                        &inStreamBuf, &timeOrTimeTz, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);

                    LOOP1_ASSERT(LINE, timeOrTimeTz.is<bdlt::Time>());
                    if (!timeOrTimeTz.is<bdlt::Time>()) {
                        continue;
                    }

                    LOOP1_ASSERT_EQ(
                        LINE, TIME, timeOrTimeTz.the<bdlt::Time>());
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::TimeTz' ENCODING"
                      << bsl::endl
                      << "-------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
                const char *d_exp;
            } DATA[] = {
              { L_,  0,  0,  0,   0,   0,     0, "07  90 00  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,     1, "07  90 01  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,     2, "07  90 02  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,   255, "07  90 FF  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,  1439, "07  95 9F  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,    -1, "07  9F FF  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,    -2, "07  9F FE  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0,  -255, "07  9F 01  00 00 00 00 00" },
              { L_,  0,  0,  0,   0,   0, -1439, "07  9A 61  00 00 00 00 00" },

              { L_,  0,  0,  0,   0,   1,     0, "07  90 00  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,     1, "07  90 01  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,     2, "07  90 02  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,   255, "07  90 FF  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,  1439, "07  95 9F  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,    -1, "07  9F FF  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,    -2, "07  9F FE  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1,  -255, "07  9F 01  00 00 00 00 01" },
              { L_,  0,  0,  0,   0,   1, -1439, "07  9A 61  00 00 00 00 01" },

              { L_,  0,  0,  0,   1,   0,     0, "07  90 00  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,     1, "07  90 01  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,     2, "07  90 02  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,   255, "07  90 FF  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,  1439, "07  95 9F  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,    -1, "07  9F FF  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,    -2, "07  9F FE  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0,  -255, "07  9F 01  00 00 00 03 E8" },
              { L_,  0,  0,  0,   1,   0, -1439, "07  9A 61  00 00 00 03 E8" },

              { L_,  0,  0,  1,   0,   0,     0, "07  90 00  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,     1, "07  90 01  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,     2, "07  90 02  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,   255, "07  90 FF  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,  1439, "07  95 9F  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,    -1, "07  9F FF  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,    -2, "07  9F FE  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0,  -255, "07  9F 01  00 00 0F 42 40" },
              { L_,  0,  0,  1,   0,   0, -1439, "07  9A 61  00 00 0F 42 40" },

              { L_,  0,  1,  0,   0,   0,     0, "07  90 00  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,     1, "07  90 01  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,     2, "07  90 02  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,   255, "07  90 FF  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,  1439, "07  95 9F  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,    -1, "07  9F FF  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,    -2, "07  9F FE  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0,  -255, "07  9F 01  00 03 93 87 00" },
              { L_,  0,  1,  0,   0,   0, -1439, "07  9A 61  00 03 93 87 00" },

              { L_,  1,  0,  0,   0,   0,     0, "07  90 00  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,     1, "07  90 01  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,     2, "07  90 02  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,   255, "07  90 FF  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,  1439, "07  95 9F  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,    -1, "07  9F FF  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,    -2, "07  9F FE  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0,  -255, "07  9F 01  00 D6 93 A4 00" },
              { L_,  1,  0,  0,   0,   0, -1439, "07  9A 61  00 D6 93 A4 00" },

              { L_,  0,  0,  0,   0, 999,     0, "07  90 00  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,     1, "07  90 01  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,     2, "07  90 02  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,   255, "07  90 FF  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,  1439, "07  95 9F  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,    -1, "07  9F FF  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,    -2, "07  9F FE  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999,  -255, "07  9F 01  00 00 00 03 E7" },
              { L_,  0,  0,  0,   0, 999, -1439, "07  9A 61  00 00 00 03 E7" },

              { L_,  0,  0,  0, 999,   0,     0, "07  90 00  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,     1, "07  90 01  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,     2, "07  90 02  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,   255, "07  90 FF  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,  1439, "07  95 9F  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,    -1, "07  9F FF  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,    -2, "07  9F FE  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0,  -255, "07  9F 01  00 00 0F 3E 58" },
              { L_,  0,  0,  0, 999,   0, -1439, "07  9A 61  00 00 0F 3E 58" },

              { L_,  0,  0, 59,   0,   0,     0, "07  90 00  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,     1, "07  90 01  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,     2, "07  90 02  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,   255, "07  90 FF  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,  1439, "07  95 9F  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,    -1, "07  9F FF  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,    -2, "07  9F FE  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0,  -255, "07  9F 01  00 03 84 44 C0" },
              { L_,  0,  0, 59,   0,   0, -1439, "07  9A 61  00 03 84 44 C0" },

              { L_,  0, 59,  0,   0,   0,     0, "07  90 00  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,     1, "07  90 01  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,     2, "07  90 02  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,   255, "07  90 FF  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,  1439, "07  95 9F  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,    -1, "07  9F FF  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,    -2, "07  9F FE  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0,  -255, "07  9F 01  00 D3 00 1D 00" },
              { L_,  0, 59,  0,   0,   0, -1439, "07  9A 61  00 D3 00 1D 00" },

              { L_, 23,  0,  0,   0,   0,     0, "07  90 00  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,     1, "07  90 01  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,     2, "07  90 02  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,   255, "07  90 FF  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,  1439, "07  95 9F  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,    -1, "07  9F FF  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,    -2, "07  9F FE  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0,  -255, "07  9F 01  13 47 43 BC 00" },
              { L_, 23,  0,  0,   0,   0, -1439, "07  9A 61  13 47 43 BC 00" },

              { L_, 23, 59, 59, 999, 999,     0, "07  90 00  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,     1, "07  90 01  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,     2, "07  90 02  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,   255, "07  90 FF  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,  1439, "07  95 9F  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,    -1, "07  9F FF  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,    -2, "07  9F FE  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999,  -255, "07  9F 01  14 1D D7 5F FF" },
              { L_, 23, 59, 59, 999, 999, -1439, "07  9A 61  14 1D D7 5F FF" },

              { L_, 24,  0,  0,   0,   0,     0, "07  90 00  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() = u::TestDataUtil::
            k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            const balber::BerEncoderOptions& OPTIONS = options;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const int         OFFSET      = DATA[i].d_offset;
                const char *const EXP         = DATA[i].d_exp;

                const bdlt::Time TIME(HOUR        ,
                                      MINUTE      ,
                                      SECOND      ,
                                      MILLISECOND ,
                                      MICROSECOND);

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                static const int NUM_BYTES = 8;

                bdlsb::MemOutStreamBuf timeTzOutBuffer;
                const bool             timeTzEncodesToBytes =
                    u::TestUtil::valueEncodesToBytes(cout,
                                                     &timeTzOutBuffer,
                                                     TIMETZ,
                                                     OPTIONS,
                                                     EXP,
                                                     NUM_BYTES);
                LOOP1_ASSERT(LINE, timeTzEncodesToBytes);
                if (!timeTzEncodesToBytes) {
                    continue;
                }

                const bool bytesDecodeToTimeTz =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    timeTzOutBuffer.data(),
                                                    timeTzOutBuffer.length(),
                                                    TIMETZ,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToTimeTz);
                if (!bytesDecodeToTimeTz) {
                    continue;
                }

                const bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TIME_OR_TIMETZ(
                                                                       TIMETZ);

                const bool bytesDecodeToVariant =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    timeTzOutBuffer.data(),
                                                    timeTzOutBuffer.length(),
                                                    TIME_OR_TIMETZ,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToVariant);
                if (!bytesDecodeToVariant) {
                    continue;
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::TimeTz' ENCODING FAILURE MODES"
                      << bsl::endl
                      << "---------------------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
            } DATA[] = {
                { L_,  0,  0,  0,   0,   0,     0 },
                { L_,  0,  0,  0,   0,   0,     1 },
                { L_,  0,  0,  0,   0,   0,    -1 },
                { L_,  0,  0,  0,   0,   1,     0 },
                { L_,  0,  0,  0,   1,   0,     0 },
                { L_,  0,  0,  1,   0,   0,     0 },
                { L_,  0,  1,  0,   0,   0,     0 },
                { L_,  1,  0,  0,   0,   0,     0 },
                { L_,  0,  0,  0,   0,   0,  1439 },
                { L_,  0,  0,  0,   0,   0, -1439 },
                { L_,  0,  0,  0,   0, 999,     0 },
                { L_,  0,  0,  0, 999,   0,     0 },
                { L_,  0,  0, 59,   0,   0,     0 },
                { L_,  0, 59,  0,   0,   0,     0 },
                { L_, 23,  0,  0,   0,   0,     0 },
                { L_, 23, 59, 59, 999, 999,  1439 },
                { L_, 23, 59, 59, 999, 999, -1439 },
                { L_, 24,  0,  0,   0,   0,     0 },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_line;
                const int HOUR        = DATA[i].d_hour;
                const int MINUTE      = DATA[i].d_minute;
                const int SECOND      = DATA[i].d_second;
                const int MILLISECOND = DATA[i].d_millisecond;
                const int MICROSECOND = DATA[i].d_microsecond;
                const int OFFSET      = DATA[i].d_offset;

                const bdlt::Time TIME(HOUR,
                                      MINUTE,
                                      SECOND,
                                      MILLISECOND,
                                      MICROSECOND);

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                bsl::vector<char> timeStreamBufPutArea;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = u::TestDataUtil::
                k_EXTENDED_BINARY_MIN_BDE_VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(true);
                options.setDatetimeFractionalSecondPrecision(6);

                int timeStreamBufPutAreaSize = 0;
                int rc = 0;

                do {
                    timeStreamBufPutArea.clear();
                    timeStreamBufPutArea.resize(timeStreamBufPutAreaSize);

                    bdlsb::FixedMemOutStreamBuf timeStreamBuf(
                        timeStreamBufPutArea.data(),
                        timeStreamBufPutArea.size());

                    rc = Util::putValue(&timeStreamBuf, TIMETZ, &options);

                    if (0 != rc) {
                        timeStreamBufPutAreaSize++;
                    }
                } while (0 != rc);

                LOOP1_ASSERT_EQ(LINE, 8, timeStreamBufPutAreaSize);
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::TimeTz' DECODING FAILURE MODES"
                      << bsl::endl
                      << "---------------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 9;

            static const struct {
                int         d_line;
                const char *d_exp;
                bool        d_success;
            } DATA[] = {
                { L_, "01  80"                        , false },
                { L_, "02  80 00"                     , false },
                { L_, "03  80 00  00"                 , false },
                { L_, "04  80 00  00 00"              , false },
                { L_, "05  80 00  00 00 00"           , false },
                { L_, "06  80 00  00 00 00 00"        , false },
                { L_, "07  80 00  00 00 00 00 00"     , true  },
                { L_, "08  80 00  00 00 00 00 00  00" , false },
                    // Verify that the decode operation requires the value to
                    // be at exactly 7 octets long (not counting the 1 byte
                    // length prefix).

                { L_, "07  7F FF  00 00 00 00 00"     , false },
                { L_, "07  80 01  00 00 00 00 00"     , false },
                    // Verify that off-by-1 numbers in the header cause
                    // decoding failures.

                { L_, "07  00 00  00 00 00 00 00"     , false },
                { L_, "07  10 00  00 00 00 00 00"     , false },
                { L_, "07  20 00  00 00 00 00 00"     , false },
                { L_, "07  30 00  00 00 00 00 00"     , false },
                { L_, "07  40 00  00 00 00 00 00"     , false },
                { L_, "07  50 00  00 00 00 00 00"     , false },
                { L_, "07  60 00  00 00 00 00 00"     , false },
                { L_, "07  70 00  00 00 00 00 00"     , false },
                { L_, "07  80 00  00 00 00 00 00"     , true  },
                { L_, "07  90 00  00 00 00 00 00"     , true  },
                { L_, "07  A0 00  00 00 00 00 00"     , false },
                { L_, "07  B0 00  00 00 00 00 00"     , false },
                { L_, "07  C0 00  00 00 00 00 00"     , false },
                { L_, "07  D0 00  00 00 00 00 00"     , false },
                { L_, "07  E0 00  00 00 00 00 00"     , false },
                { L_, "07  F0 00  00 00 00 00 00"     , false },
                    // Verify that the decode operation requires the control
                    // nibble field of the header to be '0b1000' or '0b1001'.

                { L_, "07  80 01  00 00 00 00 00"     , false },
                { L_, "07  80 02  00 00 00 00 00"     , false },
                { L_, "07  80 04  00 00 00 00 00"     , false },
                { L_, "07  80 08  00 00 00 00 00"     , false },
                { L_, "07  80 10  00 00 00 00 00"     , false },
                { L_, "07  80 20  00 00 00 00 00"     , false },
                { L_, "07  80 40  00 00 00 00 00"     , false },
                { L_, "07  80 80  00 00 00 00 00"     , false },
                { L_, "07  81 00  00 00 00 00 00"     , false },
                { L_, "07  82 00  00 00 00 00 00"     , false },
                { L_, "07  84 00  00 00 00 00 00"     , false },
                { L_, "07  88 00  00 00 00 00 00"     , false },
                    // Verify that the decode operation requires the 12-bit
                    // timezone field of the header to be 0.

                { L_, "07  80 00  14 1D D7 60 00"     , true  },
                { L_, "07  90 00  14 1D D7 60 00"     , true  },
                { L_, "07  80 00  14 1D D7 60 01"     , false },
                { L_, "07  90 00  14 1D D7 60 01"     , false },
                { L_, "07  80 00  FF FF FF FF FF"     , false },
                { L_, "07  90 00  FF FF FF FF FF"     , false },
                    // Verify that the decode operation requires the number
                    // of milliseconds since midnight to be a value less
                    // than or equal to the number of microseconds in 24 hours.
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_line;
                const char *const EXP     = DATA[i].d_exp;
                const bool        SUCCESS = DATA[i].d_success;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlt::TimeTz timeTz;
                    int          accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &timeTz, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << timeTz << bsl::endl;
                        }
                    }
                }

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> timeOrTimeTz;
                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &timeOrTimeTz, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << timeOrTimeTz
                                      << bsl::endl;
                        }
                    }
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::TimeTz' ENCODING/DECODING ISOMORPHISM"
                      << bsl::endl
                      << "----------------------------------------------------"
                      << bsl::endl;
        {
            static const int HOURS[] = {0, 1, 2, 3, 20, 21, 22, 23};

            static const int NUM_HOURS = sizeof(HOURS) / sizeof(HOURS[0]);

            static const int MINUTES[] = {
                0, 1, 2, 3, 32, 33, 57, 58, 59};

            static const int NUM_MINUTES = sizeof(MINUTES) / sizeof(MINUTES[0]);

            static const int SECONDS[] = {
                0, 1, 2, 3, 4, 5, 31, 32, 33, 57, 58, 59};

            static const int NUM_SECONDS = sizeof(SECONDS) / sizeof(SECONDS[0]);

            static const int MILLISECONDS[] = {
                0,   1,   2,   3,   4,   5,   6,   7,   8,   15,
                511, 512, 513, 997, 998, 999};

            static const int NUM_MILLISECONDS = sizeof(MILLISECONDS) /
                    sizeof(MILLISECONDS[0]);

            static const int MICROSECONDS[] = {
                0,   1,   2,   3,   4,   5,   6,   7,   8,   15,
                511, 512, 513, 997, 998, 999};

            static const int NUM_MICROSECONDS = sizeof(MICROSECONDS)
                    / sizeof(MICROSECONDS[0]);

            static const int OFFSETS[] = {
                -1439, -1438, -1024, -8 - 2, -1, 0, 1, 2, 8, 1024, 1438, 1439};

            static const int NUM_OFFSETS =
                sizeof(OFFSETS) / sizeof(OFFSETS[0]);

            static const int NUM_DATA = NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                                        NUM_MILLISECONDS * NUM_MICROSECONDS *
                                        NUM_OFFSETS;

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() = u::TestDataUtil::
            k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            bdlsb::MemOutStreamBuf     outStreamBuf;
            bdlsb::FixedMemInStreamBuf inStreamBuf(0, 0);

            bdlt::Time                               time;
            bdlt::TimeTz                             timeTz;
            bdlb::Variant2<bdlt::Time, bdlt::TimeTz> timeOrTimeTz;
            int                                      accumNumBytesConsumed = 0;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE = i;

                const int HOUR =
                    HOURS[i / (NUM_MINUTES * NUM_SECONDS * NUM_MILLISECONDS *
                               NUM_MICROSECONDS * NUM_OFFSETS)];

                const int MINUTE =
                    MINUTES[(i / (NUM_SECONDS * NUM_MILLISECONDS *
                                  NUM_MICROSECONDS * NUM_OFFSETS)) %
                            NUM_MINUTES];

                const int SECOND =
                    SECONDS[(i / (NUM_MILLISECONDS * NUM_MICROSECONDS *
                                  NUM_OFFSETS)) %
                            NUM_SECONDS];

                const int MILLISECOND =
                    MILLISECONDS[(i / (NUM_MICROSECONDS * NUM_OFFSETS)) %
                                 NUM_MILLISECONDS];

                const int MICROSECOND =
                    MICROSECONDS[(i / NUM_OFFSETS) % NUM_MICROSECONDS];

                const int OFFSET = OFFSETS[i % NUM_OFFSETS];

                const bdlt::Time TIME(HOUR        ,
                                      MINUTE      ,
                                      SECOND      ,
                                      MILLISECOND ,
                                      MICROSECOND);

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                outStreamBuf.reset();
                int rc = Util::putValue(&outStreamBuf, TIMETZ, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(
                        &inStreamBuf, &timeTz, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, TIMETZ, timeTz);
                }

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(
                        &inStreamBuf, &timeOrTimeTz, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 8, accumNumBytesConsumed);

                    LOOP1_ASSERT(LINE, timeOrTimeTz.is<bdlt::TimeTz>());
                    if (!timeOrTimeTz.is<bdlt::TimeTz>()) {
                        continue;
                    }

                    LOOP1_ASSERT_EQ(
                        LINE, TIMETZ, timeOrTimeTz.the<bdlt::TimeTz>());
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING TIME VARIANT DECODING"
                      << bsl::endl
                      << "-----------------------------"
                      << bsl::endl;
        {
            enum Selection {
                TIME,
                TIMETZ
            };

            static const int NA = -1;

            static const struct {
                int         d_line;
                const char *d_exp;
                Selection   d_selection;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_microsecond;
                int         d_offset;
            } DATA[] = {
  { L_, "07  80 00  00 00 00 00 00", TIME  ,  0,  0,  0,   0,   0, NA    },
  { L_, "07  80 00  00 00 00 00 01", TIME  ,  0,  0,  0,   0,   1, NA    },
  { L_, "07  80 00  00 00 00 03 E8", TIME  ,  0,  0,  0,   1,   0, NA    },
  { L_, "07  80 00  00 00 0F 42 40", TIME  ,  0,  0,  1,   0,   0, NA    },
  { L_, "07  80 00  00 03 93 87 00", TIME  ,  0,  1,  0,   0,   0, NA    },
  { L_, "07  80 00  00 D6 93 A4 00", TIME  ,  1,  0,  0,   0,   0, NA    },

  { L_, "07  80 00  00 00 00 03 E7", TIME  ,  0,  0,  0,   0, 999, NA    },
  { L_, "07  80 00  00 00 0F 3E 58", TIME  ,  0,  0,  0, 999,   0, NA    },
  { L_, "07  80 00  00 03 84 44 C0", TIME  ,  0,  0, 59,   0,   0, NA    },
  { L_, "07  80 00  00 D3 00 1D 00", TIME  ,  0, 59,  0,   0,   0, NA    },
  { L_, "07  80 00  13 47 43 BC 00", TIME  , 23,  0,  0,   0,   0, NA    },

  { L_, "07  80 00  14 1D D7 5F FF", TIME  , 23, 59, 59, 999, 999, NA    },
  { L_, "07  80 00  14 1D D7 60 00", TIME  , 24,  0,  0,   0,   0, NA    },

  { L_, "07  90 00  00 00 00 00 00", TIMETZ,  0,  0,  0,   0,   0,     0 },
  { L_, "07  90 01  00 00 00 00 00", TIMETZ,  0,  0,  0,   0,   0,     1 },
  { L_, "07  9F FF  00 00 00 00 00", TIMETZ,  0,  0,  0,   0,   0,    -1 },
  { L_, "07  90 00  00 00 00 00 01", TIMETZ,  0,  0,  0,   0,   1,     0 },
  { L_, "07  90 00  00 00 00 03 E8", TIMETZ,  0,  0,  0,   1,   0,     0 },
  { L_, "07  90 00  00 00 0F 42 40", TIMETZ,  0,  0,  1,   0,   0,     0 },
  { L_, "07  90 00  00 03 93 87 00", TIMETZ,  0,  1,  0,   0,   0,     0 },
  { L_, "07  90 00  00 D6 93 A4 00", TIMETZ,  1,  0,  0,   0,   0,     0 },

  { L_, "07  95 9F  00 00 00 00 00", TIMETZ,  0,  0,  0,   0,   0,  1439 },
  { L_, "07  9A 61  00 00 00 00 00", TIMETZ,  0,  0,  0,   0,   0, -1439 },
  { L_, "07  90 00  00 00 00 03 E7", TIMETZ,  0,  0,  0,   0, 999,     0 },
  { L_, "07  90 00  00 00 0F 3E 58", TIMETZ,  0,  0,  0, 999,   0,     0 },
  { L_, "07  90 00  00 03 84 44 C0", TIMETZ,  0,  0, 59,   0,   0,     0 },
  { L_, "07  90 00  00 D3 00 1D 00", TIMETZ,  0, 59,  0,   0,   0,     0 },
  { L_, "07  90 00  13 47 43 BC 00", TIMETZ, 23,  0,  0,   0,   0,     0 },

  { L_, "07  90 00  14 1D D7 5F FF", TIMETZ, 23, 59, 59, 999, 999,     0 },
  { L_, "07  95 9F  14 1D D7 5F FF", TIMETZ, 23, 59, 59, 999, 999,  1439 },
  { L_, "07  9A 61  14 1D D7 5F FF", TIMETZ, 23, 59, 59, 999, 999, -1439 },
  { L_, "07  90 00  14 1D D7 60 00", TIMETZ, 24,  0,  0,   0,   0,     0 },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char *const EXP         = DATA[i].d_exp;
                const Selection   SELECTION   = DATA[i].d_selection;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const int         OFFSET      = DATA[i].d_offset;

                static const int k_MAX_BUFFER_SIZE = 8;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                bdlb::Variant2<bdlt::Time, bdlt::TimeTz> timeOrTimeTz;
                int accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &streamBuf, &timeOrTimeTz, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                switch (SELECTION) {
                  case TIME: {
                      LOOP1_ASSERT(LINE, timeOrTimeTz.is<bdlt::Time>());
                      if (!timeOrTimeTz.is<bdlt::Time>()) continue; // CONTINUE

                      const bdlt::Time& TIME = timeOrTimeTz.the<bdlt::Time>();
                      LOOP1_ASSERT_EQ(LINE, HOUR, TIME.hour());
                      LOOP1_ASSERT_EQ(LINE, MINUTE, TIME.minute());
                      LOOP1_ASSERT_EQ(LINE, SECOND, TIME.second());
                      LOOP1_ASSERT_EQ(LINE, MILLISECOND, TIME.millisecond());
                      LOOP1_ASSERT_EQ(LINE, MICROSECOND, TIME.microsecond());
                  } break;
                  case TIMETZ: {
                      LOOP1_ASSERT(LINE, timeOrTimeTz.is<bdlt::TimeTz>());
                      if (!timeOrTimeTz.is<bdlt::TimeTz>()) continue;
                                                                    // CONTINUE

                      const bdlt::TimeTz& TIMETZ =
                          timeOrTimeTz.the<bdlt::TimeTz>();
                      const bdlt::Time& TIME = TIMETZ.localTime();

                      LOOP1_ASSERT_EQ(LINE, HOUR, TIME.hour());
                      LOOP1_ASSERT_EQ(LINE, MINUTE, TIME.minute());
                      LOOP1_ASSERT_EQ(LINE, SECOND, TIME.second());
                      LOOP1_ASSERT_EQ(LINE, MILLISECOND, TIME.millisecond());
                      LOOP1_ASSERT_EQ(LINE, MICROSECOND, TIME.microsecond());
                      LOOP1_ASSERT_EQ(LINE, OFFSET, TIMETZ.offset());
                  } break;
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Datetime' ENCODING"
                      << bsl::endl
                      << "---------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int         d_line;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_microsecond;
                const char *d_exp;
            } DATA[] = {

{ L_,    1,  1,  1,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   1,
                                       "0A  80 00  00 00 00  00 00 00 00 01" },
{ L_,    1,  1,  1,  0,  0,  0,   1,   0,
                                       "0A  80 00  00 00 00  00 00 00 03 E8" },
{ L_,    1,  1,  1,  0,  0,  1,   0,   0,
                                       "0A  80 00  00 00 00  00 00 0F 42 40" },
{ L_,    1,  1,  1,  0,  1,  0,   0,   0,
                                       "0A  80 00  00 00 00  00 03 93 87 00" },
{ L_,    1,  1,  1,  1,  0,  0,   0,   0,
                                       "0A  80 00  00 00 00  00 D6 93 A4 00" },
{ L_,    1,  1,  2,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 00 01  00 00 00 00 00" },
{ L_,    1,  2,  1,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 00 1F  00 00 00 00 00" },
{ L_,    2,  1,  1,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 01 6D  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0, 999,
                                       "0A  80 00  00 00 00  00 00 00 03 E7" },
{ L_,    1,  1,  1,  0,  0,  0, 999,   0,
                                       "0A  80 00  00 00 00  00 00 0F 3E 58" },
{ L_,    1,  1,  1,  0,  0, 59,   0,   0,
                                       "0A  80 00  00 00 00  00 03 84 44 C0" },
{ L_,    1,  1,  1,  0, 59,  0,   0,   0,
                                       "0A  80 00  00 00 00  00 D3 00 1D 00" },
{ L_,    1,  1,  1, 23,  0,  0,   0,   0,
                                       "0A  80 00  00 00 00  13 47 43 BC 00" },
{ L_,    1,  1, 31,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 00 1E  00 00 00 00 00" },
{ L_,    1, 12,  1,  0,  0,  0,   0,   0,
                                       "0A  80 00  00 01 4E  00 00 00 00 00" },
{ L_, 9999,  1,  1,  0,  0,  0,   0,   0,
                                       "0A  80 00  37 B8 6E  00 00 00 00 00" },
{ L_, 9999, 12, 31, 23, 59, 59, 999,   0,
                                       "0A  80 00  37 B9 DA  14 1D D7 5C 18" },
{ L_, 9999, 12, 31, 23, 59, 59, 999, 999,
                                       "0A  80 00  37 B9 DA  14 1D D7 5F FF" },
{ L_, 9999, 12, 31, 24,  0,  0,   0,   0,
                                       "0A  80 00  37 B9 DA  14 1D D7 60 00" },

            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            const balber::BerEncoderOptions& OPTIONS = options;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const char *const EXP         = DATA[i].d_exp;

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                static const int NUM_BYTES = 11;

                bdlsb::MemOutStreamBuf datetimeOutBuffer;
                const bool             datetimeEncodesToBytes =
                    u::TestUtil::valueEncodesToBytes(cout,
                                                     &datetimeOutBuffer,
                                                     DATETIME,
                                                     OPTIONS,
                                                     EXP,
                                                     NUM_BYTES);
                LOOP1_ASSERT(LINE, datetimeEncodesToBytes);
                if (!datetimeEncodesToBytes) {
                    continue;
                }

                const bool bytesDecodeToDatetime =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    datetimeOutBuffer.data(),
                                                    datetimeOutBuffer.length(),
                                                    DATETIME,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToDatetime);
                if (!bytesDecodeToDatetime) {
                    continue;
                }

                const bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                DATETIME_OR_DATETIMETZ(DATETIME);

                const bool bytesDecodeToVariant =
                    u::TestUtil::bytesDecodeToValue(cout,
                                                    datetimeOutBuffer.data(),
                                                    datetimeOutBuffer.length(),
                                                    DATETIME_OR_DATETIMETZ,
                                                    NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToVariant);
                if (!bytesDecodeToVariant) {
                    continue;
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Datetime' ENCODING FAILURE MODES"
                      << bsl::endl
                      << "-----------------------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
            } DATA[] = {
                { L_,    1,  1,  1,  0,  0,  0,   0,   0 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   1 },
                { L_,    1,  1,  1,  0,  0,  0,   1,   0 },
                { L_,    1,  1,  1,  0,  0,  1,   0,   0 },
                { L_,    1,  1,  1,  0,  1,  0,   0,   0 },
                { L_,    1,  1,  1,  1,  0,  0,   0,   0 },
                { L_,    1,  1,  2,  0,  0,  0,   0,   0 },
                { L_,    1,  2,  1,  0,  0,  0,   0,   0 },
                { L_,    2,  1,  1,  0,  0,  0,   0,   0 },
                { L_,    1,  1,  1,  0,  0,  0,   0, 999 },
                { L_,    1,  1,  1,  0,  0,  0, 999,   0 },
                { L_,    1,  1,  1,  0,  0, 59,   0,   0 },
                { L_,    1,  1,  1,  0, 59,  0,   0,   0 },
                { L_,    1,  1,  1, 23,  0,  0,   0,   0 },
                { L_,    1,  1, 31,  0,  0,  0,   0,   0 },
                { L_,    1, 12,  1,  0,  0,  0,   0,   0 },
                { L_, 9999,  1,  1,  0,  0,  0,   0,   0 },
                { L_, 9999, 12, 31, 23, 59, 59, 999,   0 },
                { L_, 9999, 12, 31, 23, 59, 59, 999, 999 },
                { L_,    1,  1,  1, 24,  0,  0,   0,   0 },
                { L_, 9999, 12, 31, 24,  0,  0,   0,   0 }
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_line;
                const int YEAR        = DATA[i].d_year;
                const int MONTH       = DATA[i].d_month;
                const int DAY         = DATA[i].d_day;
                const int HOUR        = DATA[i].d_hour;
                const int MINUTE      = DATA[i].d_minute;
                const int SECOND      = DATA[i].d_second;
                const int MILLISECOND = DATA[i].d_millisecond;
                const int MICROSECOND = DATA[i].d_microsecond;

                const bdlt::Datetime DATETIME(YEAR,
                                              MONTH,
                                              DAY,
                                              HOUR,
                                              MINUTE,
                                              SECOND,
                                              MILLISECOND,
                                              MICROSECOND);

                bsl::vector<char> streamBufPutArea;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = u::TestDataUtil::
                k_EXTENDED_BINARY_MIN_BDE_VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(true);
                options.setDatetimeFractionalSecondPrecision(6);

                int streamBufPutAreaSize = 0;
                int rc = 0;

                do {
                    streamBufPutArea.clear();
                    streamBufPutArea.resize(streamBufPutAreaSize);

                    bdlsb::FixedMemOutStreamBuf timeStreamBuf(
                        streamBufPutArea.data(),
                        streamBufPutArea.size());

                    rc = Util::putValue(&timeStreamBuf, DATETIME, &options);

                    if (0 != rc) {
                        streamBufPutAreaSize++;
                    }
                } while (0 != rc);

                LOOP1_ASSERT_EQ(LINE, 11, streamBufPutAreaSize);
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::Datetime' DECODING FAILURE MODES"
                      << bsl::endl
                      << "-----------------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 25;

            static const struct {
                int         d_line;
                const char *d_exp;
                bool        d_success;
                bool        d_variantSuccess;
            } DATA[] = {
              { L_, "01  80"                                 , true  , true  },

              { L_, "02  80 00"                              , true  , true  },
              { L_, "03  80 00  00"                          , true  , true  },
              { L_, "04  80 00  00 00"                       , true  , true  },
              { L_, "05  80 00  00 00 00"                    , true  , true  },
              { L_, "06  80 00  00 00 00  00"                , false , false },
              { L_, "07  80 00  00 00 00  00 00"             , true  , false },
              { L_, "08  80 00  00 00 00  00 00 00"          , true  , false },
              { L_, "09  80 00  00 00 00  00 00 00 00"       , true  , false },
              { L_, "0A  80 00  00 00 00  00 00 00 00 00"    , true  , true  },
              { L_, "0B  80 00  00 00 00  00 00 00 00 00  00", false , false },

              { L_, "0A  7F FF  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 01  00 00 00  00 00 00 00 00"    , false , false },

              { L_, "0A  00 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  10 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  20 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  30 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  40 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  50 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  60 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  70 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 00  00 00 00  00 00 00 00 00"    , true  , true  },
              { L_, "0A  90 00  00 00 00  00 00 00 00 00"    , true  , true  },
              { L_, "0A  A0 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  B0 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  C0 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  D0 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  E0 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  F0 00  00 00 00  00 00 00 00 00"    , false , false },

              { L_, "0A  80 01  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 02  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 04  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 08  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 10  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 20  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 40  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  80 80  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  81 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  82 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  84 00  00 00 00  00 00 00 00 00"    , false , false },
              { L_, "0A  88 00  00 00 00  00 00 00 00 00"    , false , false },

              { L_, "0A  80 00  00 00 00  14 1D D7 60 00"    , true  , true  },
              { L_, "0A  80 00  00 00 00  14 1D D7 60 01"    , false , false },
              { L_, "0A  80 00  00 00 00  FF FF FF FF FF"    , false , false },

              { L_, "0A  80 00  37 B9 DA  00 00 00 00 00"    , true  , true  },
              { L_, "0A  80 00  37 B9 DB  00 00 00 00 00"    , false , false },
              { L_, "0A  80 00  FF FF FF  00 00 00 00 00"    , false , false }
            };

            static const char NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE            = DATA[i].d_line;
                const char *const EXP             = DATA[i].d_exp;
                const bool        SUCCESS         = DATA[i].d_success;
                const bool        VARIANT_SUCCESS = DATA[i].d_variantSuccess;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlt::Datetime datetime;
                    int            accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &datetime, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE,
                                        bufferSize,
                                        accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << datetime << bsl::endl;
                        }
                    }
                }

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                        datetimeOrDatetimeTz;
                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(&streamBuf,
                                        &datetimeOrDatetimeTz,
                                        &accumNumBytesConsumed);
                    if (VARIANT_SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE,
                                        bufferSize,
                                        accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << datetimeOrDatetimeTz
                                      << bsl::endl;
                        }
                    }
                }
            }
        }

        if (veryVerbose)
            bsl::cout
                << bsl::endl
                << "TESTING 'bdlt::Datetime' ENCODING/DECODING ISOMORPHISM"
                << bsl::endl
                << "------------------------------------------------------"
                << bsl::endl;
        {
            static const int YEARS[] = {1, 2, 999, 1000, 1001, 9998, 9999};

            static const int NUM_YEARS = sizeof(YEARS) / sizeof(YEARS[0]);

            static const int MONTHS[] = {1, 2, 3, 11, 12};

            static const int NUM_MONTHS = sizeof(MONTHS) / sizeof(MONTHS[0]);

            static const int DAYS[] = {1, 2, 27, 28, 29, 30, 31};

            static const int NUM_DAYS = sizeof(DAYS) / sizeof(DAYS[0]);

            static const int HOURS[] = {0, 1, 11, 12, 13, 22, 23};

            static const int NUM_HOURS = sizeof(HOURS) / sizeof(HOURS[0]);

            static const int MINUTES[] = {0, 1, 29, 30, 31, 58, 59};

            static const int NUM_MINUTES =
                sizeof(MINUTES) / sizeof(MINUTES[0]);

            static const int SECONDS[] = {0, 1, 29, 30, 31, 58, 59};

            static const int NUM_SECONDS =
                sizeof(SECONDS) / sizeof(SECONDS[0]);

            static const int MILLISECONDS[] = {0, 1, 500, 998, 999};

            static const int NUM_MILLISECONDS =
                sizeof(MILLISECONDS) / sizeof(MILLISECONDS[0]);

            static const int MICROSECONDS[] = {0, 1, 500, 998, 999};

            static const int NUM_MICROSECONDS =
                sizeof(MICROSECONDS) / sizeof(MICROSECONDS[0]);

            static const int NUM_DATA = NUM_YEARS * NUM_DAYS * NUM_HOURS *
                                        NUM_MINUTES * NUM_SECONDS *
                                        NUM_MILLISECONDS * NUM_MICROSECONDS;

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() = u::TestDataUtil::
            k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            bdlsb::MemOutStreamBuf     outStreamBuf;
            bdlsb::FixedMemInStreamBuf inStreamBuf(0, 0);

            bdlt::Datetime datetime;
            bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                datetimeOrDatetimeTz;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE = i;

                const int YEAR =
                    YEARS[i /
                          (NUM_DAYS * NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                           NUM_MILLISECONDS * NUM_MICROSECONDS)];

                const int MONTH =
                    MONTHS[(i /
                            (NUM_DAYS * NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                             NUM_MILLISECONDS * NUM_MICROSECONDS)) %
                           NUM_MONTHS];

                const int DAY =
                    DAYS[(i / (NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                               NUM_MILLISECONDS * NUM_MICROSECONDS)) %
                         NUM_DAYS];

                if (!bdlt::ProlepticDateImpUtil::isValidYearMonthDay(
                        YEAR, MONTH, DAY)) {
                    continue;                                       // CONTINUE
                }

                const int HOUR = HOURS[i /
                                       (NUM_MINUTES * NUM_SECONDS *
                                        NUM_MILLISECONDS * NUM_MICROSECONDS) %
                                       NUM_HOURS];

                const int MINUTE =
                    MINUTES[(i / (NUM_SECONDS * NUM_MILLISECONDS *
                                  NUM_MICROSECONDS)) %
                            NUM_MINUTES];

                const int SECOND =
                    SECONDS[(i / (NUM_MILLISECONDS * NUM_MICROSECONDS)) %
                            NUM_SECONDS];

                const int MILLISECOND =
                    MILLISECONDS[(i / NUM_MICROSECONDS) % NUM_MILLISECONDS];

                const int MICROSECOND =
                    MICROSECONDS[i % NUM_MICROSECONDS];

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                {
                    outStreamBuf.reset();
                    int rc = Util::putValue(&outStreamBuf, DATETIME, &options);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }

                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &inStreamBuf, &datetime, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }

                    LOOP1_ASSERT_EQ(LINE, 11, accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, DATETIME, datetime);
                }

                {
                    outStreamBuf.reset();
                    int rc = Util::putValue(&outStreamBuf, DATETIME, &options);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }

                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(&inStreamBuf,
                                        &datetimeOrDatetimeTz,
                                        &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }

                    LOOP1_ASSERT(LINE,
                                 datetimeOrDatetimeTz.is<bdlt::Datetime>());
                    if (!datetimeOrDatetimeTz.is<bdlt::Datetime>()) {
                        continue;
                    }

                    LOOP1_ASSERT_EQ(LINE, 11, accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(
                        LINE,
                        DATETIME,
                        datetimeOrDatetimeTz.the<bdlt::Datetime>());
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::DatetimeTz' ENCODING"
                      << bsl::endl
                      << "-----------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
                const char *d_exp;
            } DATA[] = {
{ L_,    1,  1,  1,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   0,     1,
                                       "0A  90 01  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   0,    -1,
                                       "0A  9F FF  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   1,     0,
                                       "0A  90 00  00 00 00  00 00 00 00 01" },
{ L_,    1,  1,  1,  0,  0,  0,   1,   0,     0,
                                       "0A  90 00  00 00 00  00 00 00 03 E8" },
{ L_,    1,  1,  1,  0,  0,  1,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 00 0F 42 40" },
{ L_,    1,  1,  1,  0,  1,  0,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 03 93 87 00" },
{ L_,    1,  1,  1,  1,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 D6 93 A4 00" },
{ L_,    1,  1,  2,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 01  00 00 00 00 00" },
{ L_,    1,  2,  1,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 1F  00 00 00 00 00" },
{ L_,    2,  1,  1,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 01 6D  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   0,  1439,
                                       "0A  95 9F  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0,   0, -1439,
                                       "0A  9A 61  00 00 00  00 00 00 00 00" },
{ L_,    1,  1,  1,  0,  0,  0,   0, 999,     0,
                                       "0A  90 00  00 00 00  00 00 00 03 E7" },
{ L_,    1,  1,  1,  0,  0,  0, 999,   0,     0,
                                       "0A  90 00  00 00 00  00 00 0F 3E 58" },
{ L_,    1,  1,  1,  0,  0, 59,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 03 84 44 C0" },
{ L_,    1,  1,  1,  0, 59,  0,   0,   0,     0,
                                       "0A  90 00  00 00 00  00 D3 00 1D 00" },
{ L_,    1,  1,  1, 23,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 00  13 47 43 BC 00" },
{ L_,    1,  1, 31,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 00 1E  00 00 00 00 00" },
{ L_,    1, 12,  1,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  00 01 4E  00 00 00 00 00" },
{ L_, 9999,  1,  1,  0,  0,  0,   0,   0,     0,
                                       "0A  90 00  37 B8 6E  00 00 00 00 00" },
{ L_, 9999, 12, 31, 23, 59, 59, 999,   0,     0,
                                       "0A  90 00  37 B9 DA  14 1D D7 5C 18" },
{ L_, 9999, 12, 31, 23, 59, 59, 999,   0,  1439,
                                       "0A  95 9F  37 B9 DA  14 1D D7 5C 18" },
{ L_, 9999, 12, 31, 23, 59, 59, 999,   0, -1439,
                                       "0A  9A 61  37 B9 DA  14 1D D7 5C 18" },
{ L_, 9999, 12, 31, 23, 59, 59, 999, 999,     0,
                                       "0A  90 00  37 B9 DA  14 1D D7 5F FF" },
{ L_, 9999, 12, 31, 23, 59, 59, 999, 999,  1439,
                                       "0A  95 9F  37 B9 DA  14 1D D7 5F FF" },
{ L_, 9999, 12, 31, 23, 59, 59, 999, 999, -1439,
                                       "0A  9A 61  37 B9 DA  14 1D D7 5F FF" },
{ L_, 9999, 12, 31, 24,  0,  0,   0,   0,     0,
                                       "0A  90 00  37 B9 DA  14 1D D7 60 00" },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() =
                u::TestDataUtil::k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            const balber::BerEncoderOptions& OPTIONS = options;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const int         OFFSET      = DATA[i].d_offset;
                const char *const EXP         = DATA[i].d_exp;

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                static const int NUM_BYTES = 11;

                bdlsb::MemOutStreamBuf datetimeTzOutBuffer;
                const bool datetimeTzEncodesToBytes =
                        u::TestUtil::valueEncodesToBytes(cout,
                                                         &datetimeTzOutBuffer,
                                                         DATETIMETZ,
                                                         OPTIONS,
                                                         EXP,
                                                         NUM_BYTES);
                LOOP1_ASSERT(LINE, datetimeTzEncodesToBytes);
                if (!datetimeTzEncodesToBytes) {
                    continue;
                }

                const bool bytesDecodeToDatetimeTz =
                    u::TestUtil::bytesDecodeToValue(
                        cout,
                        datetimeTzOutBuffer.data(),
                        datetimeTzOutBuffer.length(),
                        DATETIMETZ,
                        NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToDatetimeTz);
                if (!bytesDecodeToDatetimeTz) {
                    continue;
                }

                const bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                DATETIME_OR_DATETIMETZ(DATETIMETZ);

                const bool bytesDecodeToVariant =
                    u::TestUtil::bytesDecodeToValue(
                        cout,
                        datetimeTzOutBuffer.data(),
                        datetimeTzOutBuffer.length(),
                        DATETIME_OR_DATETIMETZ,
                        NUM_BYTES);
                LOOP1_ASSERT(LINE, bytesDecodeToVariant);
                if (!bytesDecodeToVariant) {
                    continue;
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::DatetimeTz' ENCODING FAILURE MODES"
                      << bsl::endl
                      << "-------------------------------------------------"
                      << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
            } DATA[] = {
                { L_,    1,  1,  1,  0,  0,  0,   0,   0,     0 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   0,     1 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   0,    -1 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   1,     0 },
                { L_,    1,  1,  1,  0,  0,  0,   1,   0,     0 },
                { L_,    1,  1,  1,  0,  0,  1,   0,   0,     0 },
                { L_,    1,  1,  1,  0,  1,  0,   0,   0,     0 },
                { L_,    1,  1,  1,  1,  0,  0,   0,   0,     0 },
                { L_,    1,  1,  2,  0,  0,  0,   0,   0,     0 },
                { L_,    1,  2,  1,  0,  0,  0,   0,   0,     0 },
                { L_,    2,  1,  1,  0,  0,  0,   0,   0,     0 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   0,  1439 },
                { L_,    1,  1,  1,  0,  0,  0,   0,   0, -1439 },
                { L_,    1,  1,  1,  0,  0,  0,   0, 999,     0 },
                { L_,    1,  1,  1,  0,  0,  0, 999,   0,     0 },
                { L_,    1,  1,  1,  0,  0, 59,   0,   0,     0 },
                { L_,    1,  1,  1,  0, 59,  0,   0,   0,     0 },
                { L_,    1,  1,  1, 23,  0,  0,   0,   0,     0 },
                { L_,    1,  1, 31,  0,  0,  0,   0,   0,     0 },
                { L_,    1, 12,  1,  0,  0,  0,   0,   0,     0 },
                { L_, 9999,  1,  1,  0,  0,  0,   0,   0,     0 },
                { L_, 9999, 12, 31,  0,  0,  0,   0,   0,     0 },
                { L_, 9999, 12, 31,  0,  0,  0,   0,   0,  1439 },
                { L_, 9999, 12, 31,  0,  0,  0,   0,   0, -1439 },
                { L_, 9999, 12, 31, 23, 59, 59, 999,   0,  1439 },
                { L_, 9999, 12, 31, 23, 59, 59, 999,   0, -1439 },
                { L_, 9999, 12, 31, 23, 59, 59, 999, 999,  1439 },
                { L_, 9999, 12, 31, 23, 59, 59, 999, 999, -1439 },
                { L_,    1,  1,  1, 24,  0,  0,   0,   0,     0 },
                { L_, 9999, 12, 31, 24,  0,  0,   0,   0,     0 },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_line;
                const int YEAR        = DATA[i].d_year;
                const int MONTH       = DATA[i].d_month;
                const int DAY         = DATA[i].d_day;
                const int HOUR        = DATA[i].d_hour;
                const int MINUTE      = DATA[i].d_minute;
                const int SECOND      = DATA[i].d_second;
                const int MILLISECOND = DATA[i].d_millisecond;
                const int MICROSECOND = DATA[i].d_microsecond;
                const int OFFSET      = DATA[i].d_offset;

                const bdlt::Datetime DATETIME(YEAR,
                                              MONTH,
                                              DAY,
                                              HOUR,
                                              MINUTE,
                                              SECOND,
                                              MILLISECOND,
                                              MICROSECOND);

                const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                bsl::vector<char> streamBufPutArea;

                balber::BerEncoderOptions options;
                options.bdeVersionConformance() = u::TestDataUtil::
                k_EXTENDED_BINARY_MIN_BDE_VERSION;
                options.setEncodeDateAndTimeTypesAsBinary(true);
                options.setDatetimeFractionalSecondPrecision(6);

                int streamBufPutAreaSize = 0;
                int rc = 0;

                do {
                    streamBufPutArea.clear();
                    streamBufPutArea.resize(streamBufPutAreaSize);

                    bdlsb::FixedMemOutStreamBuf timeStreamBuf(
                        streamBufPutArea.data(),
                        streamBufPutArea.size());

                    rc = Util::putValue(&timeStreamBuf, DATETIMETZ, &options);

                    if (0 != rc) {
                        streamBufPutAreaSize++;
                    }
                } while (0 != rc);

                LOOP1_ASSERT_EQ(LINE, 11, streamBufPutAreaSize);
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING 'bdlt::DatetimeTz' DECODING FAILURE MODES"
                      << bsl::endl
                      << "-------------------------------------------------"
                      << bsl::endl;
        {
            static const int k_MAX_BUFFER_SIZE = 25;

            static const struct {
                int         d_line;
                const char *d_exp;
                bool        d_success;
            } DATA[] = {
                { L_, "01  80"                                 , true  },
                { L_, "02  80 00"                              , true  },
                { L_, "03  80 00  00"                          , true  },
                { L_, "04  80 00  00 00"                       , true  },
                { L_, "05  80 00  00 00 00"                    , true  },
                { L_, "06  80 00  00 00 00  00"                , false },
                { L_, "07  80 00  00 00 00  00 00"             , false },
                { L_, "08  80 00  00 00 00  00 00 00"          , false },
                { L_, "09  80 00  00 00 00  00 00 00 00"       , false },
                { L_, "0A  80 00  00 00 00  00 00 00 00 00"    , true  },
                { L_, "0B  80 00  00 00 00  00 00 00 00 00  00", false },

                { L_, "0A  7F FF  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 01  00 00 00  00 00 00 00 00"    , false },

                { L_, "0A  00 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  10 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  20 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  30 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  40 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  50 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  60 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  70 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 00  00 00 00  00 00 00 00 00"    , true  },
                { L_, "0A  90 00  00 00 00  00 00 00 00 00"    , true  },
                { L_, "0A  A0 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  B0 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  C0 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  D0 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  E0 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  F0 00  00 00 00  00 00 00 00 00"    , false },

                { L_, "0A  80 01  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 02  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 04  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 08  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 10  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 20  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 40  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  80 80  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  81 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  82 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  84 00  00 00 00  00 00 00 00 00"    , false },
                { L_, "0A  88 00  00 00 00  00 00 00 00 00"    , false },

                { L_, "0A  80 00  00 00 00  14 1D D7 60 00"    , true  },
                { L_, "0A  80 00  00 00 00  14 1D D7 60 01"    , false },
                { L_, "0A  80 00  00 00 00  FF FF FF FF FF"    , false },

                { L_, "0A  80 00  37 B9 DA  00 00 00 00 00"    , true  },
                { L_, "0A  80 00  37 B9 DB  00 00 00 00 00"    , false },
                { L_, "0A  80 00  FF FF FF  00 00 00 00 00"    , false },
            };

            static const char NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_line;
                const char *const EXP     = DATA[i].d_exp;
                const bool        SUCCESS = DATA[i].d_success;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlt::DatetimeTz datetimeTz;
                    int              accumNumBytesConsumed = 0;

                    rc = Util::getValue(
                        &streamBuf, &datetimeTz, &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE,
                                        bufferSize,
                                        accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << datetimeTz << bsl::endl;
                        }
                    }
                }

                {
                    bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                        datetimeOrDatetimeTz;
                    int accumNumBytesConsumed = 0;

                    rc = Util::getValue(&streamBuf,
                                        &datetimeOrDatetimeTz,
                                        &accumNumBytesConsumed);
                    if (SUCCESS) {
                        LOOP1_ASSERT_EQ(LINE, 0, rc);
                        LOOP1_ASSERT_EQ(LINE,
                                        bufferSize,
                                        accumNumBytesConsumed);
                    }
                    else {
                        LOOP1_ASSERT_NE(LINE, 0, rc);
                        if (0 == rc) {
                            bsl::cout << "ACTUAL: " << datetimeOrDatetimeTz
                                      << bsl::endl;
                        }
                    }
                }
            }
        }

        if (veryVerbose)
            bsl::cout
                << bsl::endl
                << "TESTING 'bdlt::DatetimeTz' ENCODING/DECODING ISOMORPHISM"
                << bsl::endl
                << "--------------------------------------------------------"
                << bsl::endl;
        {
            static const int YEARS[] = {1, 2, 999, 1000, 1001, 9998, 9999};

            static const int NUM_YEARS = sizeof(YEARS) / sizeof(YEARS[0]);

            static const int MONTHS[] = {1, 2, 3, 11, 12};

            static const int NUM_MONTHS = sizeof(MONTHS) / sizeof(MONTHS[0]);

            static const int DAYS[] = {1, 2, 27, 28, 29, 30, 31};

            static const int NUM_DAYS = sizeof(DAYS) / sizeof(DAYS[0]);

            static const int HOURS[] = {0, 1, 11, 12, 13, 22, 23};

            static const int NUM_HOURS = sizeof(HOURS) / sizeof(HOURS[0]);

            static const int MINUTES[] = {0, 1, 29, 30, 31, 58, 59};

            static const int NUM_MINUTES =
                sizeof(MINUTES) / sizeof(MINUTES[0]);

            static const int SECONDS[] = {0, 1, 29, 30, 31, 58, 59};

            static const int NUM_SECONDS =
                sizeof(SECONDS) / sizeof(SECONDS[0]);

            static const int MILLISECONDS[] = {0, 1, 500, 998, 999};

            static const int NUM_MILLISECONDS =
                sizeof(MILLISECONDS) / sizeof(MILLISECONDS[0]);

            static const int MICROSECONDS[] = {0, 1, 500, 998, 999};

            static const int NUM_MICROSECONDS =
                sizeof(MICROSECONDS) / sizeof(MICROSECONDS[0]);

            static const int OFFSETS[] = {
                -1439, -1438, -1, 0, 1, 1438, 1439};

            static const int NUM_OFFSETS = sizeof(OFFSETS) / sizeof(OFFSETS[0]);

            static const int NUM_DATA = NUM_YEARS * NUM_DAYS * NUM_HOURS *
                                        NUM_MINUTES * NUM_SECONDS *
                                        NUM_MILLISECONDS * NUM_MICROSECONDS *
                                        NUM_OFFSETS;

            balber::BerEncoderOptions options;
            options.bdeVersionConformance() = u::TestDataUtil::
            k_EXTENDED_BINARY_MIN_BDE_VERSION;
            options.setEncodeDateAndTimeTypesAsBinary(true);
            options.setDatetimeFractionalSecondPrecision(6);

            bdlsb::MemOutStreamBuf     outStreamBuf;
            bdlsb::FixedMemInStreamBuf inStreamBuf(0, 0);

            bdlt::DatetimeTz datetimeTz;
            bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                datetimeOrDatetimeTz;
            int accumNumBytesConsumed = 0;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE = i;

                const int YEAR =
                    YEARS[i /
                          (NUM_DAYS * NUM_HOURS * NUM_MINUTES * NUM_SECONDS *
                           NUM_MILLISECONDS * NUM_MICROSECONDS * NUM_OFFSETS)];

                const int MONTH =
                    MONTHS[(i / (NUM_DAYS * NUM_HOURS * NUM_MINUTES *
                                 NUM_SECONDS * NUM_MILLISECONDS *
                                 NUM_MICROSECONDS * NUM_OFFSETS)) %
                           NUM_MONTHS];

                const int DAY = DAYS[(i / (NUM_HOURS * NUM_MINUTES *
                                           NUM_SECONDS * NUM_MILLISECONDS *
                                           NUM_MICROSECONDS * NUM_OFFSETS)) %
                                     NUM_DAYS];

                if (!bdlt::ProlepticDateImpUtil::isValidYearMonthDay(
                        YEAR, MONTH, DAY)) {
                    continue;                                       // CONTINUE
                }

                const int HOUR =
                    HOURS[i /
                          (NUM_MINUTES * NUM_SECONDS * NUM_MILLISECONDS *
                           NUM_MICROSECONDS * NUM_OFFSETS) %
                          NUM_HOURS];

                const int MINUTE =
                    MINUTES[(i / (NUM_SECONDS * NUM_MILLISECONDS *
                                  NUM_MICROSECONDS * NUM_OFFSETS)) %
                            NUM_MINUTES];

                const int SECOND =
                    SECONDS[(i / (NUM_MILLISECONDS * NUM_MICROSECONDS *
                                  NUM_OFFSETS)) %
                            NUM_SECONDS];

                const int MILLISECOND =
                    MILLISECONDS[(i / NUM_MICROSECONDS * NUM_OFFSETS) %
                                 NUM_MILLISECONDS];

                const int MICROSECOND =
                    MICROSECONDS[(i / NUM_OFFSETS) % NUM_MICROSECONDS];

                const int OFFSET = OFFSETS[i % NUM_OFFSETS];

                const bdlt::Datetime DATETIME(YEAR        ,
                                              MONTH       ,
                                              DAY         ,
                                              HOUR        ,
                                              MINUTE      ,
                                              SECOND      ,
                                              MILLISECOND ,
                                              MICROSECOND);

                const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                outStreamBuf.reset();
                int rc = Util::putValue(&outStreamBuf, DATETIMETZ, &options);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) {
                    continue;
                }

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(
                        &inStreamBuf, &datetimeTz, &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 11, accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, DATETIMETZ, datetimeTz);
                }

                {
                    inStreamBuf.pubsetbuf(outStreamBuf.data(),
                                          outStreamBuf.length());

                    accumNumBytesConsumed = 0;
                    rc = Util::getValue(&inStreamBuf,
                                        &datetimeOrDatetimeTz,
                                        &accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(LINE, 0, rc);
                    if (0 != rc) {
                        continue;
                    }

                    LOOP1_ASSERT(LINE,
                                 datetimeOrDatetimeTz.is<bdlt::DatetimeTz>());
                    if (!datetimeOrDatetimeTz.is<bdlt::DatetimeTz>()) {
                        continue;
                    }
                    LOOP1_ASSERT_EQ(LINE, 11, accumNumBytesConsumed);
                    LOOP1_ASSERT_EQ(
                        LINE,
                        DATETIMETZ,
                        datetimeOrDatetimeTz.the<bdlt::DatetimeTz>());
                }
            }
        }

        if (veryVerbose)
            bsl::cout << bsl::endl
                      << "TESTING DATETIME VARIANT DECODING"
                      << bsl::endl
                      << "---------------------------------"
                      << bsl::endl;
        {
            enum Selection {
                DATETIME,
                DATETIMETZ
            };

            static const int NA = -1;

            static const struct {
                int         d_line;
                const char *d_exp;
                Selection   d_selection;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_microsecond;
                int         d_offset;
            } DATA[] = {
{ L_, "0A  80 00  00 00 00  00 00 00 00 00",
                       DATETIME  ,    1,  1,  1,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 00 00 00 01",
                       DATETIME  ,    1,  1,  1,  0,  0,  0,   0,   1, NA    },
{ L_, "0A  80 00  00 00 00  00 00 00 03 E8",
                       DATETIME  ,    1,  1,  1,  0,  0,  0,   1,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 00 0F 42 40",
                       DATETIME  ,    1,  1,  1,  0,  0,  1,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 03 93 87 00",
                       DATETIME  ,    1,  1,  1,  0,  1,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 D6 93 A4 00",
                       DATETIME  ,    1,  1,  1,  1,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 01  00 00 00 00 00",
                       DATETIME  ,    1,  1,  2,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 1F  00 00 00 00 00",
                       DATETIME  ,    1,  2,  1,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 01 6D  00 00 00 00 00",
                       DATETIME  ,    2,  1,  1,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 00 00 03 E7",
                       DATETIME  ,    1,  1,  1,  0,  0,  0,   0, 999, NA    },
{ L_, "0A  80 00  00 00 00  00 00 0F 3E 58",
                       DATETIME  ,    1,  1,  1,  0,  0,  0, 999,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 03 84 44 C0",
                       DATETIME  ,    1,  1,  1,  0,  0, 59,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  00 D3 00 1D 00",
                       DATETIME  ,    1,  1,  1,  0, 59,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  13 47 43 BC 00",
                       DATETIME  ,    1,  1,  1, 23,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 1E  00 00 00 00 00",
                       DATETIME  ,    1,  1, 31,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 01 4E  00 00 00 00 00",
                       DATETIME  ,    1, 12,  1,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  37 B8 6E  00 00 00 00 00",
                       DATETIME  , 9999,  1,  1,  0,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  37 B9 DA  14 1D D7 5F FF",
                       DATETIME  , 9999, 12, 31, 23, 59, 59, 999, 999, NA    },
{ L_, "0A  80 00  00 00 00  14 1D D7 60 00",
                       DATETIME  ,    1,  1,  1, 24,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  00 00 00  14 1D D7 60 00",
                       DATETIME  ,    1,  1,  1, 24,  0,  0,   0,   0, NA    },
{ L_, "0A  80 00  37 B9 DA  14 1D D7 60 00",
                       DATETIME  , 9999, 12, 31, 24,  0,  0,   0,   0, NA    },
{ L_, "0A  90 00  00 00 00  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 01  00 00 00  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   0,     1 },
{ L_, "0A  9F FF  00 00 00  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   0,    -1 },
{ L_, "0A  90 00  00 00 00  00 00 00 00 01",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   1,     0 },
{ L_, "0A  90 00  00 00 00  00 00 00 03 E8",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   1,   0,     0 },
{ L_, "0A  90 00  00 00 00  00 00 0F 42 40",
                       DATETIMETZ,    1,  1,  1,  0,  0,  1,   0,   0,     0 },
{ L_, "0A  90 00  00 00 00  00 03 93 87 00",
                       DATETIMETZ,    1,  1,  1,  0,  1,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 00 00  00 D6 93 A4 00",
                       DATETIMETZ,    1,  1,  1,  1,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 00 01  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  2,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 00 1F  00 00 00 00 00",
                       DATETIMETZ,    1,  2,  1,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 01 6D  00 00 00 00 00",
                       DATETIMETZ,    2,  1,  1,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  95 9F  00 00 00  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   0,  1439 },
{ L_, "0A  9A 61  00 00 00  00 00 00 00 00",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0,   0, -1439 },
{ L_, "0A  90 00  00 00 00  00 00 00 03 E7",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0,   0, 999,     0 },
{ L_, "0A  90 00  00 00 00  00 00 0F 3E 58",
                       DATETIMETZ,    1,  1,  1,  0,  0,  0, 999,   0,     0 },
{ L_, "0A  90 00  00 00 00  00 03 84 44 C0",
                       DATETIMETZ,    1,  1,  1,  0,  0, 59,   0,   0,     0 },
{ L_, "0A  90 00  00 00 00  00 D3 00 1D 00",
                       DATETIMETZ,    1,  1,  1,  0, 59,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 00 00  13 47 43 BC 00",
                       DATETIMETZ,    1,  1,  1, 23,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 00 1E  00 00 00 00 00",
                       DATETIMETZ,    1,  1, 31,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  00 01 4E  00 00 00 00 00",
                       DATETIMETZ,    1, 12,  1,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  37 B8 6E  00 00 00 00 00",
                       DATETIMETZ, 9999,  1,  1,  0,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  37 B9 DA  14 1D D7 5F FF",
                       DATETIMETZ, 9999, 12, 31, 23, 59, 59, 999, 999,     0 },
{ L_, "0A  95 9F  37 B9 DA  14 1D D7 5F FF",
                       DATETIMETZ, 9999, 12, 31, 23, 59, 59, 999, 999,  1439 },
{ L_, "0A  9A 61  37 B9 DA  14 1D D7 5F FF",
                       DATETIMETZ, 9999, 12, 31, 23, 59, 59, 999, 999, -1439 },
{ L_, "0A  90 00  00 00 00  14 1D D7 60 00",
                       DATETIMETZ,    1,  1,  1, 24,  0,  0,   0,   0,     0 },
{ L_, "0A  90 00  37 B9 DA  14 1D D7 60 00",
                       DATETIMETZ, 9999, 12, 31, 24,  0,  0,   0,   0,     0 },
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char *const EXP         = DATA[i].d_exp;
                const Selection   SELECTION   = DATA[i].d_selection;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const int         OFFSET      = DATA[i].d_offset;

                static const int k_MAX_BUFFER_SIZE = 12;

                char buffer[k_MAX_BUFFER_SIZE];
                int  bufferSize;

                int rc = u::ByteBufferUtil::loadBuffer(
                    &bufferSize,
                    buffer,
                    sizeof(buffer),
                    EXP,
                    static_cast<int>(bsl::strlen(EXP)));
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                bdlsb::FixedMemInStreamBuf streamBuf(buffer, bufferSize);

                bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                    datetimeOrDatetimeTz;
                int accumNumBytesConsumed = 0;

                rc = Util::getValue(
                    &streamBuf, &datetimeOrDatetimeTz, &accumNumBytesConsumed);
                LOOP1_ASSERT_EQ(LINE, 0, rc);
                if (0 != rc) continue;                              // CONTINUE

                switch (SELECTION) {
                  case DATETIME: {
                    LOOP1_ASSERT(LINE,
                                 datetimeOrDatetimeTz.is<bdlt::Datetime>());
                    if (!datetimeOrDatetimeTz.is<bdlt::Datetime>())
                        continue;                                   // CONTINUE

                    const bdlt::Datetime& DATETIME =
                        datetimeOrDatetimeTz.the<bdlt::Datetime>();
                    LOOP1_ASSERT_EQ(LINE, YEAR, DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, MONTH, DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, DAY, DATETIME.day());
                    LOOP1_ASSERT_EQ(LINE, HOUR, DATETIME.hour());
                    LOOP1_ASSERT_EQ(LINE, MINUTE, DATETIME.minute());
                    LOOP1_ASSERT_EQ(LINE, SECOND, DATETIME.second());
                    LOOP1_ASSERT_EQ(LINE, MILLISECOND, DATETIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, MICROSECOND, DATETIME.microsecond());
                  } break;
                  case DATETIMETZ: {
                    LOOP1_ASSERT(LINE,
                                 datetimeOrDatetimeTz.is<bdlt::DatetimeTz>());
                    if (!datetimeOrDatetimeTz.is<bdlt::DatetimeTz>())
                        continue;                                   // CONTINUE

                    const bdlt::DatetimeTz& DATETIMETZ =
                        datetimeOrDatetimeTz.the<bdlt::DatetimeTz>();
                    const bdlt::Datetime& DATETIME = DATETIMETZ.
                    localDatetime();

                    LOOP1_ASSERT_EQ(LINE, YEAR, DATETIME.year());
                    LOOP1_ASSERT_EQ(LINE, MONTH, DATETIME.month());
                    LOOP1_ASSERT_EQ(LINE, DAY, DATETIME.day());
                    LOOP1_ASSERT_EQ(LINE, HOUR, DATETIME.hour());
                    LOOP1_ASSERT_EQ(LINE, MINUTE, DATETIME.minute());
                    LOOP1_ASSERT_EQ(LINE, SECOND, DATETIME.second());
                    LOOP1_ASSERT_EQ(LINE, MILLISECOND, DATETIME.millisecond());
                    LOOP1_ASSERT_EQ(LINE, MICROSECOND, DATETIME.microsecond());
                    LOOP1_ASSERT_EQ(LINE, OFFSET, DATETIMETZ.offset());
                  } break;
                }
            }
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' FAILURE REPORTING
        //   This case tests that 'getValue' returns a non-zero value when
        //   the stream buffer reaches the end of its source before a
        //   value has finished being read.  Note that the "source" of a
        //   'bsl::streambuf' is standard shorthand for the input-side of
        //   the associated character sequence of the 'bsl::streambuf'.
        //
        // Concerns:
        //: 1 For all 'Simple' types, 'getValue' returns a non-zero value
        //:   when the stream buffer reaches the end of its source before
        //:   the value has finished being read.
        //
        // Plan:
        //: 1 For several boundary values of each supported 'Simple' type:
        //:
        //:   1 Encode the value to a buffer
        //:
        //:   2 Starting with a 1-byte view of the buffer, and repeatedly
        //:     increasing the view size by 1 until the full buffer is visible:
        //:
        //:     1 Create an input stream given the view of the buffer and
        //:       attempt to read back the same value
        //:
        //:     2a If the buffer is not the full view, verify that the read
        //:        operation fails
        //:
        //:     2b If the buffer is the full view, verify that the read
        //:        operation succeeds
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING 'getValue' FAILURE REPORTING"
                      << bsl::endl
                      << "===================================="
                      << bsl::endl;

        u::Case27Tester t;

        // 'char'
        t(L_, '\x00');
        t(L_, '\x01');
        t(L_, '\xFE');
        t(L_, '\xFF');

        // 'signed char'
        t(L_, static_cast<signed char>('\x00'));
        t(L_, static_cast<signed char>('\x01'));
        t(L_, static_cast<signed char>('\xFE'));
        t(L_, static_cast<signed char>('\xFF'));

        // 'unsigned char'
        t(L_, static_cast<unsigned char>('\x00'));
        t(L_, static_cast<unsigned char>('\x01'));
        t(L_, static_cast<unsigned char>('\xFE'));
        t(L_, static_cast<unsigned char>('\xFF'));

        // 'bool'
        t(L_, true);
        t(L_, false);

        // 'int'
        t(L_, INT_MIN    );
        t(L_, INT_MIN + 1);
        t(L_,          -1);
        t(L_,           0);
        t(L_,           1);
        t(L_, INT_MAX - 1);
        t(L_, INT_MAX    );

        // 'unsigned int'
        t(L_,           0u);
        t(L_, UINT_MAX - 1);
        t(L_, UINT_MAX    );

        // 'bsls::Types::Int64'
        t(L_, bsl::numeric_limits<bsls::Types::Int64>::min()    );
        t(L_, bsl::numeric_limits<bsls::Types::Int64>::min() + 1);
        t(L_, static_cast<bsls::Types::Int64>(-1)               );
        t(L_, static_cast<bsls::Types::Int64>( 0)               );
        t(L_, static_cast<bsls::Types::Int64>( 1)               );
        t(L_, bsl::numeric_limits<bsls::Types::Int64>::max() - 1);
        t(L_, bsl::numeric_limits<bsls::Types::Int64>::max()    );

        // 'bsls::Types::Uint64'
        t(L_, bsl::numeric_limits<bsls::Types::Uint64>::min()    );
        t(L_, bsl::numeric_limits<bsls::Types::Uint64>::min() + 1);
        t(L_, static_cast<bsls::Types::Uint64>(-1)               );
        t(L_, static_cast<bsls::Types::Uint64>( 0)               );
        t(L_, static_cast<bsls::Types::Uint64>( 1)               );
        t(L_, bsl::numeric_limits<bsls::Types::Uint64>::max() - 1);
        t(L_, bsl::numeric_limits<bsls::Types::Uint64>::max()    );

        // 'float'
        t(L_, bsl::numeric_limits<float>::min());
        t(L_, -1.f);
        t(L_, 0.f);
        t(L_, 1.f);
        t(L_, bsl::numeric_limits<float>::max());

        // 'double'
        t(L_, bsl::numeric_limits<double>::min());
        t(L_, 1.0);
        t(L_, 0.0);
        t(L_, -1.0);
        t(L_, bsl::numeric_limits<double>::max());

        // 'bdldfp::Decimal64'
        t(L_, bsl::numeric_limits<bdldfp::Decimal64>::min());
        t(L_, bdldfp::Decimal64(-1.0));
        t(L_, bdldfp::Decimal64(0.0));
        t(L_, bdldfp::Decimal64(1.0));
        t(L_, bsl::numeric_limits<bdldfp::Decimal64>::max());

        // 'bsl::string'
        t(L_, bsl::string("Lorem ipsum dolor sit amet"));

        // 'bdlt::Date'
        t(L_, bdlt::Date());
        t(L_, bdlt::Date(1, 1, 1));
        t(L_, bdlt::Date(9999, 12, 31));

        // 'bdlt::DateTz'
        t(L_, bdlt::DateTz(bdlt::Date(), 0));
        t(L_, bdlt::DateTz(bdlt::Date(1, 1, 1), -1439));
        t(L_, bdlt::DateTz(bdlt::Date(1, 1, 1), 0));
        t(L_, bdlt::DateTz(bdlt::Date(1, 1, 1), 1439));
        t(L_, bdlt::DateTz(bdlt::Date(9999, 12, 31), -1439));
        t(L_, bdlt::DateTz(bdlt::Date(9999, 12, 31), 0));
        t(L_, bdlt::DateTz(bdlt::Date(9999, 12, 31), 1439));

        // 'bdlt::Datetime'
        t(L_, bdlt::Datetime(1, 1, 1, 0, 0, 0, 0, 0));
        t(L_, bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 0));

        // 'bdlt::DatetimeTz'
        t(L_, bdlt::DatetimeTz(bdlt::Datetime(1, 1, 1, 0, 0, 0, 0, 0), -1439));
        t(L_, bdlt::DatetimeTz(bdlt::Datetime(1, 1, 1, 0, 0, 0, 0, 0), 0));
        t(L_, bdlt::DatetimeTz(bdlt::Datetime(1, 1, 1, 0, 0, 0, 0, 0), 1439));
        t(L_,
          bdlt::DatetimeTz(bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 0),
                           -1439));
        t(L_,
          bdlt::DatetimeTz(bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 0),
                           -0));
        t(L_,
          bdlt::DatetimeTz(bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 0),
                           1439));

        // 'bdlt::Time'
        t(L_, bdlt::Time(0, 0, 0, 0, 0));
        t(L_, bdlt::Time(23, 59, 59, 999, 0));

        // 'bdlt::TimeTz'
        t(L_, bdlt::TimeTz(bdlt::Time(0, 0, 0, 0, 0), -1439));
        t(L_, bdlt::TimeTz(bdlt::Time(0, 0, 0, 0, 0), 0));
        t(L_, bdlt::TimeTz(bdlt::Time(0, 0, 0, 0, 0), 1439));
        t(L_, bdlt::TimeTz(bdlt::Time(23, 59, 59, 999, 0), -1439));
        t(L_, bdlt::TimeTz(bdlt::Time(23, 59, 59, 999, 0), 0));
        t(L_, bdlt::TimeTz(bdlt::Time(23, 59, 59, 999, 0), 1439));

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' BEHAVIORAL FINGERPRINT
        //   This case tests that the values decoded by
        //   'balber::BerUtil::getValue' for a large, deterministically and
        //   pseudo-randomly generated set of inputs, are *likely* equivalent to
        //   the values it decodes as of BDE 3.44.0.  This case tests the
        //   decoded values for too many encodings to explicitly enumerate.
        //   Instead, the encodings are generated during the execution of the
        //   test, and the results of each invocation of 'getValue' are
        //   concatenated and the resulting information "fingerprinted" using
        //   an implementation of the MD5 Message-Digest Algorithm, which
        //   removes the need to explicitly enumerate said results.  This
        //   "fingerprint" is compared against the known fingerprint of the
        //   function applied to the same input as of BDE 3.44.0.
        //
        //   This case provides a regression test for the input-output space of
        //   'balber::BerUtil::getValue' from BDE release 3.44.0.  If the
        //   behavior of 'getValue' changes in any way, this test provides a
        //   high likelihood of failing.  Intentional behavioral modifications
        //   should update the behavioral fingerprints in this test case
        //   accordingly.
        //
        //   Note that this test merely indicates a *high* *probability* of
        //   expected behavior, not *certainty*.  The entire input-output space
        //   of the operation cannot be enumerated, because it is infinite.
        //   Additionally, because the "fingerprint" is only 128 bits, there is
        //   a chance that two different output sets have the same fingerprint.
        //   Note however, that this probability is astronomically small.
        //
        // Concerns:
        //: 1 The sampled input-output space does not have incidental patterns
        //:   arising from the use of an underlying linear congruential
        //:   generator supplied with a single seed.
        //:
        //: 2 The fingerprint of 'getValue' does not change when the
        //:   encoding is configured to use binary date and time values and
        //:   the fractional second precision is varied between 3 and 6.
        //
        // Plan:
        //: 1 Using 3 different random seeds, and taking 50,000 samples with
        //:   each seed, test the fingerprint of 'getValue' using a default
        //:   'balber::BerEncoderOptions' except for the following options, the
        //:   for which all combinations of the values mentioned should be
        //:   permuted:
        //:
        //:   1 'encodeDateAndTimeTypesAsBinary', which may take values
        //:      'true' and 'false'.
        //:
        //:   2 'datetimeFractionalSecondPrecision', which may take values
        //:      3 and 6.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING 'getValue' BEHAVIORAL FINGERPRINT"
                      << bsl::endl
                      << "========================================="
                      << bsl::endl;

        enum {
            SEED_0 = 0,          // a sensible default seed
            SEED_1 = -24036583,  // a decently large, negated mersenne prime
            SEED_2 = 32582657    // a decently large mersenne prime
        };

        static const struct {
            int         d_line;             // line number
            int         d_randomSeed;       // seed to generate random input
            int         d_numSamples;       // number of input-output samples
            int         d_secondPrecision;  // an encoding option parameter
            bool        d_encodeDateAndTimeTypesAsBinary;
                // an encoding option parameter
            const char *d_md5;  // md5 fingerprint of all sampled output
        } DATA[] = {
            //               RANDOM SEED TO GENERATE INPUT FOR BEHAVIORS
            //              .-------------------------------------------
            //             /      NUMBER OF SAMPLE BEHAVIORS IN FINGERPRINT
            //            /      .-----------------------------------------
            //           /      /    FRACTIONAL SECOND PRECISION PARAM
            //          /      /    .---------------------------------
            //   LINE  /      /    /    BINARY DATE AND TIME ENCODING PARAM
            //  .---- /      /    /    .-----------------------------------
            // /     /      /    /    /    'putValue' BEHAVIORAL FINGERPRINT
            //-- ------- ------ -- ------ ------------------------------------
            { L_, SEED_0, 50000, 3, false, "a4f4796fce831c62afed26b178c63715" },
            { L_, SEED_0, 50000, 3, true , "a9e9d0fbbc1487449bf928907792f211" },
            { L_, SEED_0, 50000, 6, false, "0bab0341289bddcd8c66fd607b0b76dc" },
            { L_, SEED_0, 50000, 6, true , "a9e9d0fbbc1487449bf928907792f211" },
            { L_, SEED_1, 50000, 3, false, "53229ec3841b3815e8efb6cc8e64a098" },
            { L_, SEED_1, 50000, 3, true , "1c7ceb60dbd74c17be929311f86ab185" },
            { L_, SEED_1, 50000, 6, false, "4f884d423a3fbb65b531c5f4fe1ec0ed" },
            { L_, SEED_1, 50000, 6, true , "1c7ceb60dbd74c17be929311f86ab185" },
            { L_, SEED_2, 50000, 3, false, "01defb86e00fc10ca4c4a5dc802f9c54" },
            { L_, SEED_2, 50000, 3, true , "de75fb921b25090f0f6975b6e4bf8bd3" },
            { L_, SEED_2, 50000, 6, false, "af150f3a022e5fd55ccb5b400bfbc487" },
            { L_, SEED_2, 50000, 6, true , "de75fb921b25090f0f6975b6e4bf8bd3" },
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int  LINE             = DATA[i].d_line;
            const int  RANDOM_SEED      = DATA[i].d_randomSeed;
            const int  NUM_SAMPLES      = DATA[i].d_numSamples;
            const int  SECOND_PRECISION = DATA[i].d_secondPrecision;
            const bool ENCODE_DATE_AND_TIME_TYPES_AS_BINARY =
                DATA[i].d_encodeDateAndTimeTypesAsBinary;
            const bslstl::StringRef MD5 = DATA[i].d_md5;

            u::GetValueFingerprint getValueFingerprint;
            getValueFingerprint.setSeed(RANDOM_SEED);
            getValueFingerprint.setNumSamples(NUM_SAMPLES);
            getValueFingerprint.setFractionalSecondPrecision(SECOND_PRECISION);
            getValueFingerprint.setEncodeDateAndTimeTypesAsBinary(
                                          ENCODE_DATE_AND_TIME_TYPES_AS_BINARY);

            const u::Md5Fingerprint md5Fingerprint =
                u::ChecksumUtil::getMd5(getValueFingerprint);

            bdlsb::MemOutStreamBuf md5FingerprintStreamBuf;
            bsl::ostream md5FingerprintStream(&md5FingerprintStreamBuf);
            md5FingerprintStream << md5Fingerprint;

            const bslstl::StringRef md5FingerprintString(
                md5FingerprintStreamBuf.data(),
                md5FingerprintStreamBuf.length());

            LOOP1_ASSERT_EQ(LINE, md5FingerprintString, MD5);
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' BEHAVIORAL FINGERPRINT
        //   This case tests that the encodings provided by
        //   'balber::BerUtil::putValue' for a large, deterministically and
        //   pseudo-randomly generated set of inputs, is *likely* equivalent to
        //   the encodings it provides as of BDE 3.44.0.  This case tests the
        //   encodings for too many inputs to explicitly enumerate.  Instead,
        //   the input is generated during the execution of the test, and the
        //   result of invoking 'putValue' on all of the input is collated
        //   and "fingerprinted" using an implementation of the MD5
        //   Message-Digest Algorithm, which removes the need to explicitly
        //   enumerate said results.  This "fingerprint" is compared against
        //   the known fingerprint of the function applied to the same input as
        //   of BDE 3.44.0.
        //
        //   This case provides a regression test for the input-output space of
        //   'balber::BerUtil::putValue' from BDE release 3.44.0.  If the
        //   behavior of 'putValue' changes in any way, this test provides a
        //   high likelihood of failing.  Intentional behavioral modifications
        //   should update the behavioral fingerprints in this test case
        //   accordingly.
        //
        //   Note that this test merely indicates a *high* *probability* of
        //   expected behavior, not *certainty*.  The entire input-output space
        //   of the operation cannot be enumerated, because it is infinite.
        //   Additionally, because the "fingerprint" is only 128 bits, there is
        //   a chance that two different output sets have the same fingerprint.
        //   Note however, that this probability is astronomically small.
        //
        // Concerns:
        //: 1 The sampled input-output space of 'putValue' does not have
        //:   incidental patterns arising from the use of an underlying
        //:   linear congruential generator supplied with a single seed.
        //:
        //: 2 The fingerprint of 'putValue' does not change when the
        //:   encoding is configured to use binary date and time values and
        //:   the fractional second precision is varied between 3 and 6.
        //
        // Test Plan:
        //: 1 Using 3 different random seeds, and taking 50,000 samples with
        //:   each seed, test the fingerprint of 'putValue' using a default
        //:   'balber::BerEncoderOptions' except for the following options, the
        //:   for which all combinations of the values mentioned should be
        //:   permuted:
        //:
        //:   1 'encodeDateAndTimeTypesAsBinary', which may take values
        //:     'true' and 'false'.
        //:
        //:   2 'datetimeFractionalSecondPrecision', which may take values
        //:     3 and 6.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING 'putValue' BEHAVIORAL FINGERPRINT"
                      << bsl::endl
                      << "========================================="
                      << bsl::endl;

        enum {
            SEED_0 = 0,          // a sensible default seed
            SEED_1 = -24036583,  // a decently large, negated mersenne prime
            SEED_2 = 32582657    // a decently large mersenne prime
        };

        static const struct {
            int         d_line;             // line number
            int         d_randomSeed;       // seed to generate random input
            int         d_numSamples;       // number of input-output samples
            int         d_secondPrecision;  // an encoding option parameter
            bool        d_encodeDateAndTimeTypesAsBinary;
                // an encoding option parameter
            const char *d_md5;  // md5 fingerprint of all sampled output
        } DATA[] = {
            //               RANDOM SEED TO GENERATE INPUT FOR BEHAVIORS
            //              .-------------------------------------------
            //             /      NUMBER OF SAMPLE BEHAVIORS IN FINGERPRINT
            //            /      .-----------------------------------------
            //           /      /    FRACTIONAL SECOND PRECISION
            //          /      /    .---------------------------
            //   LINE  /      /    /    USE BINARY DATE AND TIME ENCODING
            //  .---- /      /    /    .---------------------------------
            // /     /      /    /    /    'putValue' BEHAVIORAL FINGERPRINT
            //-- ------- ------ -- ------ ------------------------------------
            { L_, SEED_0, 50000, 3, false, "a893e5c4643b5b40b45aa8d93c90a097" },
            { L_, SEED_0, 50000, 3, true , "7166428b5ca3e18a0953877091f37ce7" },
            { L_, SEED_0, 50000, 6, false, "95acf3bfe61bed5bf29c686c61ff6269" },
            { L_, SEED_0, 50000, 6, true , "7166428b5ca3e18a0953877091f37ce7" },
            { L_, SEED_1, 50000, 3, false, "37ce54c6d2f92fd9a822080aeda006e2" },
            { L_, SEED_1, 50000, 3, true , "d0a8c8d46f37a89f15e71dae0c64d492" },
            { L_, SEED_1, 50000, 6, false, "9d3d66bd3b64fc76d51ba638c2d88531" },
            { L_, SEED_1, 50000, 6, true , "d0a8c8d46f37a89f15e71dae0c64d492" },
            { L_, SEED_2, 50000, 3, false, "06c17b7af732eaa78f2fb8a03351d0fc" },
            { L_, SEED_2, 50000, 3, true , "2689b7bf2a0a5002170e1c631fdf29ef" },
            { L_, SEED_2, 50000, 6, false, "b3bf9ce8ffa3a8601e8edf915b8c418a" },
            { L_, SEED_2, 50000, 6, true , "2689b7bf2a0a5002170e1c631fdf29ef" },
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int  LINE             = DATA[i].d_line;
            const int  RANDOM_SEED      = DATA[i].d_randomSeed;
            const int  NUM_SAMPLES      = DATA[i].d_numSamples;
            const int  SECOND_PRECISION = DATA[i].d_secondPrecision;
            const bool ENCODE_DATE_AND_TIME_TYPES_AS_BINARY =
                DATA[i].d_encodeDateAndTimeTypesAsBinary;
            const bslstl::StringRef MD5 = DATA[i].d_md5;

            u::PutValueFingerprint putValueFingerprint;
            putValueFingerprint.setSeed(RANDOM_SEED);
            putValueFingerprint.setNumSamples(NUM_SAMPLES);
            putValueFingerprint.setFractionalSecondPrecision(SECOND_PRECISION);
            putValueFingerprint.setEncodeDateAndTimeTypesAsBinary(
                                          ENCODE_DATE_AND_TIME_TYPES_AS_BINARY);

            const u::Md5Fingerprint md5Fingerprint =
                u::ChecksumUtil::getMd5(putValueFingerprint);

            bdlsb::MemOutStreamBuf md5FingerprintStreamBuf;
            bsl::ostream md5FingerprintStream(&md5FingerprintStreamBuf);
            md5FingerprintStream << md5Fingerprint;

            const bslstl::StringRef md5FingerprintString(
                md5FingerprintStreamBuf.data(),
                md5FingerprintStreamBuf.length());

            LOOP1_ASSERT_EQ(LINE, md5FingerprintString, MD5);
        }

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING MD5 Test Apparatus
        //   This case tests that the testing apparatus for computing the MD5
        //   fingerprint of a string of bits is a correct implementation of the
        //   MD5 Message-Digest Algorithm.  The specification for this
        //   algorithm comes from the April 1992 revision of IETF RFC 1321.  In
        //   this test case, the data provided to the algorithm will be
        //   referred to as the "message", which is structured as an ordered
        //   sequence of bytes.  A "chunk" refers to a sub-sequence of the
        //   bytes of a message.  The "chunking" of a message refers to the
        //   sequence of variably-sized chunks used to provide a message to the
        //   algorithm.  Each message has one or more possible valid chunkings,
        //   all of which have the same fingerprint (or "digest").  The sample
        //   implementation in IETF RFC 1321 Appendix A will be referred to as
        //   the "reference implementation".
        //
        // Concerns:
        //: 1 The MD5 apparatus produces the same fingerprints as the
        //:   reference implementation for the messages specified in the
        //:   'MDTestSuite' function in the April 1992 revision of IETF RFC
        //:   1321 Appendix A.
        //:
        //: 2 Fingerprints do not depend on the chunking of a message.
        //:
        //: 3 That MD5 is demonstrably a high-quality message digest algorithm,
        //:   where quality is defined as a measure of the probability that two
        //:   randomly-selected and near identical messages have different
        //:   fingerprints, and where message similarity can be measured by
        //:   minimum edit distance.  Stated another way, it is a measure
        //:   of the risk that two similar messages will have a checksum
        //:   collision.
        //
        // Plan:
        //: 1 Verify that the MD5 apparatus produces the same fingerprints
        //:   as the reference implementation for the messages specified
        //:   in the 'MDTestSuite' function in the April 1992 revision of
        //:   IETF RFC 1321 Appendix A.
        //:
        //: 2 Verify that the apparatus produces the same fingerprint
        //:   regardless of the chunking of a message.
        //:
        //: 3 Verify that the quality of MD5 is at least 99% for a reasonable
        //:   sample set of messages.
        //
        // Testing:
        //   u::Md5Fingerprint
        //   u::Md5FingerprintUtil
        //   u::Md5Block
        //   u::Md5BlockUtil
        //   u::Md5BlockInputIterator
        //   u::Md5BlockInputRange
        //   u::Md5State
        //   u::Md5StateUtil
        //   u::Md5Util
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING MD5 Test Apparatus" << bsl::endl
                      << "==========================" << bsl::endl;

        // Verify that the apparatus produces the same fingerprints as the
        // reference implementation.
        {
            static const struct {
                int         d_line;
                const char *d_string;
                const char *d_fingerprint;
            } DATA[] = {
                //  LINE
                //  ----
                // /      MESSAGE              MESSAGE FINGERPRINT
                //-- ----------------- ------------------------------------
                { L_, ""              , "d41d8cd98f00b204e9800998ecf8427e" },
                    // Verify that the MD5 apparatus calculates the correct
                    // fingerprint for the empty message, as provided in the
                    // 'MDTestSuite' from Appendix A of the specification.

                { L_, "a"             , "0cc175b9c0f1b6a831c399e269772661" },
                { L_, "abc"           , "900150983cd24fb0d6963f7d28e17f72" },
                { L_, "message digest", "f96b697d7cb7938d525a2f31aaf161d0" },
                { L_, "abcdefghijklmn"
                      "opqrstuvwxyz"  , "c3fcd3d76192e4007dfb496cca67e13b" },
                { L_, "ABCDEFGHIJKLMN"
                      "OPQRSTUVWXYZ"
                      "abcdefghijklmn"
                      "opqrstuvwxyz"
                      "0123456789"    , "d174ab98d277d9f5a5611c2c9f419d9f" },
                { L_, "1234567890"
                      "1234567890"
                      "1234567890"
                      "1234567890"
                      "1234567890"
                      "1234567890"
                      "1234567890"
                      "1234567890"    , "57edf4a22be3c955ac49da2e2107b67a" },
                    // Verify that the MD5 apparatus passes the 'MDTestSuite'
                    // from Appendix A of the specification.

                { L_, u::TestDataUtil::s_RANDOM_LOREM_IPSUM
                                      , "c6ee2c717ffcedd951e082e8194f408f" }
                    // Verify that the fingerprint is calculated correctly
                    // for large messages.
            };

            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char *const STRING      = DATA[i].d_string;
                const char *const FINGERPRINT = DATA[i].d_fingerprint;

                const bslstl::StringRef STRING_REF(STRING);
                const bslstl::StringRef FINGERPRINT_REF(FINGERPRINT);

                bsl::vector<unsigned char> data(STRING_REF.begin(),
                                                STRING_REF.end());

                u::Md5Fingerprint fingerprint =
                    u::Md5Util::getFingerprint(data.begin(), data.end());

                bdlsb::MemOutStreamBuf fingerprintStreamBuf;
                bsl::ostream fingerprintStream(&fingerprintStreamBuf);
                fingerprintStream << fingerprint;

                const bslstl::StringRef fingerprintRef(
                    fingerprintStreamBuf.data(),
                    fingerprintStreamBuf.length());

                LOOP1_ASSERT_EQ(LINE, fingerprintRef, FINGERPRINT_REF);
            }
        }

        // Verify that the apparatus produces the same fingerprint regardless
        // of the chunking of a message.
        {
            enum {
                k_NUM_CHUNK_SIZES = 10 // num chunk sizes per data row
            };

            static const unsigned char *k_DATA_BEGIN =
                u::TestDataUtil::s_RANDOM_GARBAGE_1K;
                // A pointer to the first byte of
                // 'u::TestDataUtil::s_RANDOM_GARBAGE_1K', which acts as an
                // iterator to that byte.

            static const char k_EXPECTED_FINGERPRINT[] =
                "044d5905fa983dd9845075cb302dbe76";
                // The expected fingerprint for
                // 'u::TestDataUtil::s_RANDOM_GARBAGE_1K'.  Note that this
                // fingerprint was computed using a third party implementation
                // that is assumed to be correct.

            static const struct {
                int d_line;                           // line number
                int d_chunkSizes[k_NUM_CHUNK_SIZES];  // seq of chunk sizes
            } DATA[] = {
                //  LINE
                //  ----
                // /                      CHUNK SIZES
                //--, -----------------------------------------------------
                { L_, { 1024,   0,   0,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {  512, 512,   0,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {  256, 256, 256, 256,   0,   0,   0,   0,   0,   0 } },
                { L_, {  128, 128, 128, 128, 128, 128, 128, 128,   0,   0 } },
                    // Verify the fingerprint is correct for all possible
                    // chunkings of the message where the chunk sizes are
                    // all equal and no more than 10 chunks are used.

                { L_, {    1,   2,   4,   8,  16,  32,  64, 128, 256, 513 } },
                { L_, {    2,   4,   8,  16,  32,  64, 128, 256, 512,   2 } },
                { L_, {    4,   8,  16,  32,  64, 128, 256, 512,   1,   3 } },
                { L_, {    8,  16,  32,  64, 128, 256, 512,   1,   2,   5 } },
                { L_, {   16,  32,  64, 128, 256, 512,   1,   2,   4,   9 } },
                { L_, {   32,  64, 128, 256, 512,   1,   2,   4,   8,  17 } },
                { L_, {   64, 128, 256, 512,   1,   2,   4,   8,  16,  33 } },
                { L_, {  128, 256, 512,   1,   2,   4,   8,  16,  32,  65 } },
                { L_, {  256, 512,   1,   2,   4,   8,  16,  32,  64, 129 } },
                { L_, {  512,   1,   2,   4,   8,  16,  32,  64, 128, 257 } },
                    // Verify the fingerprint is correct for chunkings using
                    // rotated orders of exponentially increasing chunk sizes.

                { L_, {  512, 256, 128,  64,  32,  16,   8,   4,   2,   2 } },
                { L_, {  256, 128,  64,  32,  16,   8,   4,   2,   1, 513 } },
                { L_, {  128,  64,  32,  16,   8,   4,   2,   1, 512, 257 } },
                { L_, {   64,  32,  16,   8,   4,   2,   1, 512, 256, 129 } },
                { L_, {   32,  16,   8,   4,   2,   1, 512, 256, 128,  65 } },
                { L_, {   16,   8,   4,   2,   1, 512, 256, 128,  64,  33 } },
                { L_, {    8,   4,   2,   1, 512, 256, 128,  64,  32,  17 } },
                { L_, {    4,   2,   1, 512, 256, 128,  64,  32,  16,   9 } },
                { L_, {    2,   1, 512, 256, 128,  64,  32,  16,   8,   5 } },
                { L_, {    1, 512, 256, 128,  64,  32,  16,   8,   4,   3 } },
                    // Verify the fingerprint is correct for chunkings using
                    // rotated orders of exponentially decreasing chunk sizes.

                { L_, {    0,   0,1024,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  55, 969,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  56, 968,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  57, 967,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  63, 961,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  64, 960,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {    0,  65, 959,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  55, 914,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  56, 913,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  57, 912,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  63, 906,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  64, 905,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   55,  65, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  55, 913,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  56, 912,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  57, 911,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  63, 905,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  64, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   56,  65, 903,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  55, 912,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  56, 911,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  57, 910,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  63, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  64, 903,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   57,  65, 902,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  55, 906,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  56, 905,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  57, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  63, 898,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  64, 897,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   63,  65, 896,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  55, 905,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  56, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  57, 903,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  63, 897,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  64, 896,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   64,  65, 895,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  55, 904,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  56, 903,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  57, 902,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  63, 896,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  64, 895,   0,   0,   0,   0,   0,   0,   0 } },
                { L_, {   65,  65, 894,   0,   0,   0,   0,   0,   0,   0 } },
                    // Verify the fingerprint is correct for chunkings making
                    // use of chunk sizes that create edge cases in the
                    // algorithm, such as '56', '64'.  Note that '56' is the
                    // size that the final 'u::Md5Block' needs to be padded to
                    // reach, and '64' is the capacity of a  'u::Md5Block'.
            };

            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int LINE              = DATA[i].d_line;
                const int(&CHUNK_SIZES)[10] = DATA[i].d_chunkSizes;

                u::Md5State state;
                u::Md5StateUtil::loadSeedValue(&state);

                const unsigned char *dataIt;
                const unsigned char *dataEnd = k_DATA_BEGIN;
                for (const int *it  = CHUNK_SIZES;
                                it != CHUNK_SIZES + k_NUM_CHUNK_SIZES;
                              ++it) {
                    const int chunkSize = *it;

                    dataIt   = dataEnd;
                    dataEnd += chunkSize;

                    u::Md5BlockInputRange dataRange(dataIt, dataEnd);
                    for (u::Md5BlockInputIterator blockIt  = dataRange.begin();
                                                  blockIt != dataRange.end();
                                                ++blockIt) {
                        u::Md5StateUtil::append(&state, *blockIt);
                    }
                }

                u::Md5StateUtil::appendPaddingAndLength(&state);
                const u::Md5Fingerprint fingerprint = state.fingerprint();

                bdlsb::MemOutStreamBuf fingerprintStreamBuf;
                bsl::ostream fingerprintStream(&fingerprintStreamBuf);
                fingerprintStream << fingerprint;

                const bslstl::StringRef fingerprintString(
                    fingerprintStreamBuf.data(),
                    fingerprintStreamBuf.length());

                LOOP1_ASSERT_EQ(LINE                   ,
                                fingerprintString      ,
                                k_EXPECTED_FINGERPRINT);
            }
        }

        // Verify that MD5 is a high-quality message-digest algorithm by
        // verifying that modifying any individual bit in a sample message of
        // 8192 bits produces a different fingerprint.  Note that different
        // checksums are not guaranteed, it just so happens that no collisions
        // occur with any 1-bit perturbation of this sample message.
        {
            const unsigned char (&k_RANDOM_GARBAGE)[1024] =
                    u::TestDataUtil::s_RANDOM_GARBAGE_1K;

            static const char k_FINGERPRINT[] =
                "044d5905fa983dd9845075cb302dbe76";
                // The expected fingerprint for
                // 'u::TestDataUtil::s_RANDOM_GARBAGE_1K'.  Note that this
                // fingerprint was computed using a third party implementation
                // that is assumed to be correct.

            for (int i = 0; i != 8 * sizeof(k_RANDOM_GARBAGE); ++i) {
                bsl::vector<unsigned char> randomGarbageCopy(
                    k_RANDOM_GARBAGE,
                    k_RANDOM_GARBAGE + sizeof(k_RANDOM_GARBAGE));
                // A copy of the value of 'RANDOM_GARBAGE' for which to change
                // a single bit.

                const unsigned char byteAtIdx = randomGarbageCopy[i / 8];

                // Create 2 masks.  One for a single bit in the byte at the
                // current index, the other for all but that one bit.
                const unsigned char bitMask =
                    static_cast<unsigned char>(0x01 << (i % 8));
                const unsigned char byteMask = ~bitMask;

                // Use the single-bit mask to get a byte having all bits 0,
                // except for the masked bit from the byte at the current
                // index (which may be 0 or 1).
                const unsigned char bitAtIdx = byteAtIdx & bitMask;

                // Calculate the unsigned char value having the same value
                // as 'byteAtIdx' except for having exactly 1 of its 8 bits
                // flipped.
                const unsigned char newByteAtIdx = (byteAtIdx & byteMask) |
                                                   (~bitAtIdx & bitMask);

                // Replace the byte at the specified index with the above
                // calculated value, thereby flipping a single bit in the
                // message.
                randomGarbageCopy[i / 8] = newByteAtIdx;

                // Verify that this changes the fingerprint of the message.
                u::Md5Fingerprint fingerprint =
                        u::Md5Util::getFingerprint(randomGarbageCopy.begin(),
                                                   randomGarbageCopy.end());

                bdlsb::MemOutStreamBuf fingerprintStreamBuf;
                bsl::ostream fingerprintStream(&fingerprintStreamBuf);
                fingerprintStream << fingerprint;

                const bslstl::StringRef fingerprintRef(
                    fingerprintStreamBuf.data(),
                    fingerprintStreamBuf.length());

                LOOP1_ASSERT_NE(i, fingerprintRef, k_FINGERPRINT);
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for textual time values of differing lengths
        //   This case tests whether, for differing lengths, decoding date/time
        //   values into a variant are considered to have their timezone
        //   information or not.
        //
        // Concerns:
        //: 1 Sample some ISO 8601-encoded time values for which decoding into
        //:   a date/time value that optionally includes a time zone either
        //:   erroneously drops the time zone information, or erroneously
        //:   treats non-time zone information as a time zone.
        //
        // Plan:
        //: 1 Enumerate some of the aforementioned values and verify the
        //:   defective behavior.
        //
        // Testing:
        //  int getValue(bsl::streambuf *, TYPE *, int length, const Options&);
        // --------------------------------------------------------------------

        enum Type {
            TIME,
            TIMETZ
        };

        enum {
            NA = 0 // "Not applicable" used in a test row to indicate that
                   // the value will not be used
        };

        // 'getValue' currently possesses some defects with regards to whether
        // it interprets encoded date and time values of varying lengths as
        // having timezone information or not.  For backwards-compatibility
        // purposes, some of these defects are checked below.  Lines in the
        // test table marked with a [1] or [2] indicate a behavior that can
        // be considered defective.
        //
        // [1] below indicates that variant does not have the expected
        //     selection, but the encoding needed to witness this defect is not
        //     supported.
        //
        // [2] below indicates that the variant does not have the expected
        //     selection, and that the encoding needed to witness this defect
        //     is supported.

        static const struct {
            int         d_lineNum;     // line number
            const char *d_exp;         // ISO 8601 date/time expression
            Type        d_type;        // either TIME or TIMETZ
            int         d_hour;        // decoded hour value
            int         d_minute;      // decoded minute value
            int         d_second;      // decoded second value
            int         d_millisecond; // decoded millisecond value
            int         d_offset;      // decoded timezone offset
        } DATA[] = {
        //                                          MINUTES
        //                                  HOURS  .-------
        //   LINE                           ----- /   SECONDS
        //  .----                           |    /   .-------
        // /    ISO 8601 TIME       TYPE    |   /   /   MSECS  TZ OFFSET
        //-- --------------------- ------- --- --- --- ------- ---------
        { L_, "00:00:01"          , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.0"        , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.00"       , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.000"      , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.0000"     , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.00000"    , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.000000"   , TIME  , 00, 00, 01, 000000, NA   },
        { L_, "00:00:01.0000000"  , TIMETZ, 00, 00, 01, 000000, 0000 }, // [1]
        { L_, "00:00:01.00000000" , TIMETZ, 00, 00, 01, 000000, 0000 }, // [1]
        { L_, "00:00:01.000000000", TIMETZ, 00, 00, 01, 000000, 0000 }, // [1]

        { L_, "00:00:01Z"         , TIME  , 00, 00, 01, 000000, NA   }, // [2]
        { L_, "00:00:01+0000"     , TIME  , 00, 00, 01, 000000, NA   }, // [2]
        { L_, "00:00:01+0001"     , TIME  , 23, 59, 01, 000000, NA   }, // [2]
        { L_, "00:00:01.0+0000"   , TIME  , 00, 00, 01, 000000, NA   }, // [2]
        { L_, "00:00:01.00+0000"  , TIMETZ, 00, 00, 01, 000000, 0000 },
        { L_, "00:00:01.000+0000" , TIMETZ, 00, 00, 01, 000000, 0000 },

        { L_, "00:00:01+00:00"    , TIME  , 00, 00, 01, 000000, NA   }, // [2]
        { L_, "00:00:01+00:01"    , TIME  , 23, 59, 01, 000000, NA   }, // [2]
        { L_, "00:00:01.0+00:01"  , TIMETZ, 00, 00, 01, 000000, 0001 },
        { L_, "00:00:01.00+00:01" , TIMETZ, 00, 00, 01, 000000, 0001 },
        { L_, "00:00:01.000+00:01", TIMETZ, 00, 00, 01, 000000, 0001 },
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int   LINE        = DATA[i].d_lineNum;
            const char *EXP         = DATA[i].d_exp;
            const Type  TYPE        = DATA[i].d_type;
            const int   HOUR        = DATA[i].d_hour;
            const int   MINUTE      = DATA[i].d_minute;
            const int   SECOND      = DATA[i].d_second;
            const int   MILLISECOND = DATA[i].d_millisecond;
            const int   OFFSET      = DATA[i].d_offset;

            bdlsb::FixedMemInStreamBuf streamBuf(EXP, bsl::strlen(EXP));
            bdlb::Variant2<bdlt::Time, bdlt::TimeTz> value;
            const int length = static_cast<int>(bsl::strlen(EXP));

            int rc = Util::getValue(&streamBuf, &value, length);
            LOOP1_ASSERT_EQ(LINE, 0, rc);
            if (0 != rc) continue;                                  // CONTINUE

            switch (TYPE) {
              case TIME: {
                  LOOP1_ASSERT(LINE,  value.is<bdlt::Time>());
                  LOOP1_ASSERT(LINE, !value.is<bdlt::TimeTz>());

                  if (value.is<bdlt::TimeTz>()) {
                      const bdlt::TimeTz& error = value.the<bdlt::TimeTz>();
                      const bdlt::Time&   errorLocalTime = error.localTime();
                      const int           errorOffset    = error.offset();

                      LOOP1_ASSERT_NE(LINE, HOUR, errorLocalTime.hour());
                      LOOP1_ASSERT_NE(LINE, MINUTE, errorLocalTime.minute());
                      LOOP1_ASSERT_NE(LINE, SECOND, errorLocalTime.second());
                      LOOP1_ASSERT_NE(
                          LINE, MILLISECOND, errorLocalTime.millisecond());
                      LOOP1_ASSERT_NE(LINE, OFFSET, errorOffset);
                  }

                  if (!value.is<bdlt::Time>()) continue;            // CONTINUE

                  const bdlt::Time& time = value.the<bdlt::Time>();

                  LOOP1_ASSERT_EQ(LINE, HOUR, time.hour());
                  LOOP1_ASSERT_EQ(LINE, MINUTE, time.minute());
                  LOOP1_ASSERT_EQ(LINE, SECOND, time.second());
                  LOOP1_ASSERT_EQ(LINE, MILLISECOND, time.millisecond());
              } break;
              case TIMETZ: {
                  LOOP1_ASSERT(LINE, !value.is<bdlt::Time>());
                  LOOP1_ASSERT(LINE,  value.is<bdlt::TimeTz>());

                  if (value.is<bdlt::Time>()) {
                      const bdlt::Time& error = value.the<bdlt::Time>();

                      LOOP1_ASSERT_EQ(LINE, HOUR, error.hour());
                      LOOP1_ASSERT_EQ(LINE, MINUTE, error.minute());
                      LOOP1_ASSERT_NE(LINE, SECOND, error.second());
                      LOOP1_ASSERT_NE(LINE, MILLISECOND, error.millisecond());
                  }

                  if (!value.is<bdlt::TimeTz>()) continue;          // CONTINUE

                  const bdlt::TimeTz& timeTz = value.the<bdlt::TimeTz>();

                  LOOP1_ASSERT_EQ(LINE, HOUR, timeTz.localTime().hour());
                  LOOP1_ASSERT_EQ(LINE, MINUTE, timeTz.localTime().minute());
                  LOOP1_ASSERT_EQ(LINE, SECOND, timeTz.localTime().second());
                  LOOP1_ASSERT_EQ(
                      LINE, MILLISECOND, timeTz.localTime().millisecond());
                  LOOP1_ASSERT_EQ(LINE, OFFSET, timeTz.offset());
              } break;
            }
        }

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' and 'getValue' for Decimal64
        //
        // Concerns:
        //: 1 'putValue' insert correctly encodes a Decimal64 value by first
        //:   insert a length and then the output of
        //:   'DecimalConvertUtil::decimal64ToMultiWidthEncoding'.
        //:
        //: 2 'getValue' decodes a Decimal64 by delegating its operation to
        //:   'DecimalConvertUtil::decimal64FromMultiWidthEncoding'.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        bdldfp::Decimal64 (*MDF)(long long, int) =
                                           &bdldfp::DecimalUtil::makeDecimal64;

        static const struct {
            int d_lineNum;
            bdldfp::Decimal64 d_value;
            const char *d_exp;
        } DATA[] = {
            {  L_,  MDF(1, 0),  "02 c0 01"  },
            {  L_,  MDF(0, 1),  "03 e0 00 00"  },
            {  L_,  MDF((1 << 14), 2), "04 48 00 40 00"  }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const bdldfp::Decimal64  VALUE = DATA[i]. d_value;
            const char              *EXP   = DATA[i].d_exp;
            const int                LEN   = numOctets(EXP);

            balber::BerEncoderOptions options;
            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == Util::putValue(&osb, VALUE, &options));

            if (veryVerbose) {
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            LOOP2_ASSERT(LEN,
                         osb.length(),
                         LEN == static_cast<int>(osb.length()));
            LOOP2_ASSERT(osb.data(),
                         EXP,
                         0 == compareBuffers(osb.data(), EXP));
            {
                bdldfp::Decimal64 value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' FOR DATE/TIME COMPONENTS USING A VARIANT
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                         "TESTING 'getValue' FOR DATE/TIME USING VARIANT" "\n"
                         "==============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //-------^
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
   //-------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Date'"
                               << " and 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { T_ P_(LINE) P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Date>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Date>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) { T_ P_(LINE) P_(Y) P_(M) P_(D) P(OFF) }

                const Type VALUE(bdlt::Date(Y, M, D), OFF);
                Variant    value1, value2;

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DateTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DateTz>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Time'"
                               << " and 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { T_ P_(LINE) P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Time>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Time>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) {
                                 T_ P_(LINE) P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::TimeTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::TimeTz>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Datetime'"
                               << " and 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) {
                       T_ P_(LINE) P_(Y) P_(M) P_(D) P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(Y, M, D, H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (LENGTH > 6) {
                        // Datetime objects having length greater that 6 bytes
                        // are always encoded with a time zone.

                        continue;
                    }

                    if (veryVerbose) {
                        P(LINE)
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Datetime>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Datetime>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) {
                          T_ P_(Y) P_(M) P_(D) P_(H) P_(MM) P_(S) P_(MS) P(OFF)
                }

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (OFF) {

                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value1, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DatetimeTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value2, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DatetimeTz>());
                }
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' FOR DATE/TIME COMPONENTS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n" "TESTING 'putValue' FOR DATE/TIME" "\n"
                                       "================================" "\n";

        if (verbose) bsl::cout << "\nTesting Date." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   opt  exp
  //----  -----  -----   ---   ---  ---
  {   L_, 2020,      1,    1,    1, "01 00"                       },
  {   L_, 2020,      1,    1,    0, "0a 323032 302d3031 2d3031"   },

  {   L_, 2020,      1,    2,    1, "01 01"                       },
  {   L_, 2020,      1,    2,    0, "0a 323032 302d3031 2d3032"   },

  {   L_, 2020,      1,   31,    1, "01 1E"                       },
  {   L_, 2020,      1,   31,    0, "0a 323032 302d3031 2d3331"   },

  {   L_, 2020,      2,    1,    1, "01 1F"                       },
  {   L_, 2020,      2,    1,    0, "0a 323032 302d3032 2d3031"   },

  {   L_, 2020,      2,   28,    1, "01 3A"                       },
  {   L_, 2020,      2,   28,    0, "0a 323032 302d3032 2d3238"   },

  {   L_, 2020,      2,   29,    1, "01 3B"                       },
  {   L_, 2020,      2,   29,    0, "0a 323032 302d3032 2d3239"   },

  {   L_, 2020,      3,    1,    1, "01 3C"                       },
  {   L_, 2020,      3,    1,    0, "0a 323032 302d3033 2d3031"   },

  {   L_, 2020,      5,    7,    1, "01 7F"                       },
  {   L_, 2020,      5,    7,    0, "0a 323032 302d3035 2d3037"   },

  {   L_, 2020,      5,    8,    1, "02 0080"                     },
  {   L_, 2020,      5,    8,    0, "0a 323032 302d3035 2d3038"   },

  {   L_, 2020,     12,   31,    1, "02 016D"                     },
  {   L_, 2020,     12,   31,    0, "0a 323032 302d3132 2d3331"   },

  {   L_, 2021,      1,    1,    1, "02 016E"                     },
  {   L_, 2021,      1,    1,    0, "0a 323032 312d3031 2d3031"   },

  {   L_, 2023,     12,   31,    1, "02 05B4"                     },
  {   L_, 2023,     12,   31,    0, "0a 323032 332d3132 2d3331"   },

  {   L_, 2024,      1,    1,    1, "02 05B5"                     },
  {   L_, 2024,      1,    1,    0, "0a 323032 342d3031 2d3031"   },

  {   L_, 2099,     12,   31,    1, "02 7223"                     },
  {   L_, 2099,     12,   31,    0, "0a 323039 392d3132 2d3331"   },

  {   L_, 2100,      1,    1,    1, "02 7224"                     },
  {   L_, 2100,      1,    1,    0, "0a 323130 302d3031 2d3031"   },

  {   L_, 2100,      2,   28,    1, "02 725E"                     },
  {   L_, 2100,      2,   28,    0, "0a 323130 302d3032 2d3238"   },

  {   L_, 2100,      3,    1,    1, "02 725F"                     },
  {   L_, 2100,      3,    1,    0, "0a 323130 302d3033 2d3031"   },

  {   L_, 2109,      9,   18,    1, "02 7FFF"                     },
  {   L_, 2109,      9,   18,    0, "0a 323130 392d3039 2d3138"   },

  {   L_, 2109,      9,   19,    1, "03 008000"                   },
  {   L_, 2109,      9,   19,    0, "0a 323130 392d3039 2d3139"   },

  {   L_, 2119,     12,   31,    1, "03 008EAB"                   },
  {   L_, 2119,     12,   31,    0, "0a 323131 392d3132 2d3331"   },

  {   L_, 2120,      1,    1,    1, "03 008EAC"                   },
  {   L_, 2120,      1,    1,    0, "0a 323132 302d3031 2d3031"   },

  {   L_, 2019,     12,   31,    1, "01 FF"                       },
  {   L_, 2019,     12,   31,    0, "0a 323031 392d3132 2d3331"   },

  {   L_, 2019,      8,   26,    1, "01 80"                       },
  {   L_, 2019,      8,   26,    0, "0a 323031 392d3038 2d3236"   },

  {   L_, 2019,      8,   25,    1, "02 FF7F"                     },
  {   L_, 2019,      8,   25,    0, "0a 323031 392d3038 2d3235"   },

  {   L_, 2016,      2,   28,    1, "02 FA85"                     },
  {   L_, 2016,      2,   28,    0, "0a 323031 362d3032 2d3238"   },

  {   L_, 2016,      2,   29,    1, "02 FA86"                     },
  {   L_, 2016,      2,   29,    0, "0a 323031 362d3032 2d3239"   },

  {   L_, 2016,      3,    1,    1, "02 FA87"                     },
  {   L_, 2016,      3,    1,    0, "0a 323031 362d3033 2d3031"   },

  {   L_, 2012,      2,   28,    1, "02 F4D0"                     },
  {   L_, 2012,      2,   28,    0, "0a 323031 322d3032 2d3238"   },

  {   L_, 2012,      2,   29,    1, "02 F4D1"                     },
  {   L_, 2012,      2,   29,    0, "0a 323031 322d3032 2d3239"   },

  {   L_, 2012,      3,    1,    1, "02 F4D2"                     },
  {   L_, 2012,      3,    1,    0, "0a 323031 322d3033 2d3031"   },

  {   L_, 2000,      2,   28,    1, "02 E3B1"                     },
  {   L_, 2000,      2,   28,    0, "0a 323030 302d3032 2d3238"   },

  {   L_, 2000,      2,   29,    1, "02 E3B2"                     },
  {   L_, 2000,      2,   29,    0, "0a 323030 302d3032 2d3239"   },

  {   L_, 2000,      3,    1,    1, "02 E3B3"                     },
  {   L_, 2000,      3,    1,    0, "0a 323030 302d3033 2d3031"   },

  {   L_, 1930,      4,   15,    1, "02 8000"                     },
  {   L_, 1930,      4,   15,    0, "0a 313933 302d3034 2d3135"   },

  {   L_, 1930,      4,   14,    1, "03 FF7FFF"                   },
  {   L_, 1930,      4,   14,    0, "0a 313933 302d3034 2d3134"   },

  {   L_, 1066,     10,   14,    1, "03 FAB005"                   },
  {   L_, 1066,     10,   14,    0, "0a 313036 362d3130 2d3134"   },

  {   L_, 9999,     12,   31,    1, "03 2C794A"                   },
  {   L_, 9999,     12,   31,    0, "0a 393939 392d3132 2d3331"   },

  {   L_,    1,      1,    1,    1, "03 F4BF70"                   },
  {   L_,    1,      1,    1,    0, "0a 303030 312d3031 2d3031"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) {
                           T_ P_(YEAR) P_(MONTH) P_(DAY) P_(BIN) P_(EXP) P(LEN)
                }

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Date VALUE(YEAR, MONTH, DAY);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Date value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting DateTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_tzoffset;  // time zone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   off   opt  exp
  //----  -----  -----   ---   ---   ---  ---
  {   L_, 2020,      1,    1,    0,    1, "01 00"                            },
  {   L_, 2020,      1,    1,    0,    0,
                                 "10 323032 302d3031 2d30312B 30303A30 30"   },

  {   L_, 2020,      1,    1, 1439,    1, "04 059F0000"                      },
  {   L_, 2020,      1,    1, 1439,    0,
                                 "10 323032 302d3031 2d30312B 32333A35 39"   },

  {   L_, 2020,      1,    1,-1439,    1, "04 FA610000"                      },
  {   L_, 2020,      1,    1,-1439,    0,
                                 "10 323032 302d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      1,    2,    0,    1, "01 01"                            },
  {   L_, 2020,      1,    2,    0,    0,
                                 "10 323032 302d3031 2d30322B 30303A30 30"   },

  {   L_, 2020,      1,    2, 1439,    1, "04 059F0001"                      },
  {   L_, 2020,      1,    2, 1439,    0,
                                 "10 323032 302d3031 2d30322B 32333A35 39"   },

  {   L_, 2020,      1,    2,-1439,    1, "04 FA610001"                      },
  {   L_, 2020,      1,    2,-1439,    0,
                                 "10 323032 302d3031 2d30322D 32333A35 39"   },

  {   L_, 2019,     12,   31,    0,    1, "01 FF"                            },
  {   L_, 2019,     12,   31,    0,    0,
                                 "10 323031 392d3132 2d33312B 30303A30 30"   },

  {   L_, 2019,     12,   31, 1439,    1, "04 059FFFFF"                      },
  {   L_, 2019,     12,   31, 1439,    0,
                                 "10 323031 392d3132 2d33312B 32333A35 39"   },

  {   L_, 2019,     12,   31,-1439,    1, "04 FA61FFFF"                      },
  {   L_, 2019,     12,   31,-1439,    0,
                                 "10 323031 392d3132 2d33312D 32333A35 39"   },

  {   L_,    1,      1,    1,    0,    1, "03 F4BF70"                        },
  {   L_,    1,      1,    1,    0,    0,
                                 "10 303030 312d3031 2d30312B 30303A30 30"   },

  {   L_,    1,      1,    1, 1439,    1, "05 059FF4BF 70"                   },
  {   L_,    1,      1,    1, 1439,    0,
                                 "10 303030 312d3031 2d30312B 32333A35 39"   },

  {   L_,    1,      1,    1,-1439,    1, "05 FA61F4BF 70"                   },
  {   L_,    1,      1,    1,-1439,    0,
                                 "10 303030 312d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      5,    7,    0,    1, "01 7F"                            },
  {   L_, 2020,      5,    7,    0,    0,
                                 "10 323032 302d3035 2d30372B 30303A30 30"   },

  {   L_, 2020,      5,    7, 1439,    1, "04 059F007F"                      },
  {   L_, 2020,      5,    7, 1439,    0,
                                 "10 323032 302d3035 2d30372B 32333A35 39"   },

  {   L_, 2020,      5,    7,-1439,    1, "04 FA61007F"                      },
  {   L_, 2020,      5,    7,-1439,    0,
                                 "10 323032 302d3035 2d30372D 32333A35 39"   },

  {   L_, 2020,      5,    8,    0,    1, "02 0080"                          },
  {   L_, 2020,      5,    8,    0,    0,
                                 "10 323032 302d3035 2d30382B 30303A30 30"   },

  {   L_, 2020,      5,    8, 1439,    1, "04 059F0080"                      },
  {   L_, 2020,      5,    8, 1439,    0,
                                 "10 323032 302d3035 2d30382B 32333A35 39"   },

  {   L_, 2020,      5,    8,-1439,    1, "04 FA610080"                      },
  {   L_, 2020,      5,    8,-1439,    0,
                                 "10 323032 302d3035 2d30382D 32333A35 39"   },

  {   L_, 2109,      9,   18,    0,    1, "02 7FFF"                          },
  {   L_, 2109,      9,   18,    0,    0,
                                 "10 323130 392d3039 2d31382B 30303A30 30"   },

  {   L_, 2109,      9,   18, 1439,    1, "04 059F7FFF"                      },
  {   L_, 2109,      9,   18, 1439,    0,
                                 "10 323130 392d3039 2d31382B 32333A35 39"   },

  {   L_, 2109,      9,   18,-1439,    1, "04 FA617FFF"                      },
  {   L_, 2109,      9,   18,-1439,    0,
                                 "10 323130 392d3039 2d31382D 32333A35 39"   },

  {   L_, 2109,      9,   19,    0,    1, "03 008000"                        },
  {   L_, 2109,      9,   19,    0,    0,
                                 "10 323130 392d3039 2d31392B 30303A30 30"   },

  {   L_, 2109,      9,   19, 1439,    1, "05 059F008000"                    },
  {   L_, 2109,      9,   19, 1439,    0,
                                 "10 323130 392d3039 2d31392B 32333A35 39"   },

  {   L_, 2109,      9,   19,-1439,    1, "05 FA61008000"                    },
  {   L_, 2109,      9,   19,-1439,    0,
                                 "10 323130 392d3039 2d31392D 32333A35 39"   },

  {   L_, 2019,      8,   26,    0,    1, "01 80"                            },
  {   L_, 2019,      8,   26,    0,    0,
                                 "10 323031 392d3038 2d32362B 30303A30 30"   },

  {   L_, 2019,      8,   26, 1439,    1, "04 059FFF80"                      },
  {   L_, 2019,      8,   26, 1439,    0,
                                 "10 323031 392d3038 2d32362B 32333A35 39"   },

  {   L_, 2019,      8,   26,-1439,    1, "04 FA61FF80"                      },
  {   L_, 2019,      8,   26,-1439,    0,
                                 "10 323031 392d3038 2d32362D 32333A35 39"   },

  {   L_, 2019,      8,   25,    0,    1, "02 FF7F"                          },
  {   L_, 2019,      8,   25,    0,    0,
                                 "10 323031 392d3038 2d32352B 30303A30 30"   },

  {   L_, 2019,      8,   25, 1439,    1, "04 059FFF7F"                      },
  {   L_, 2019,      8,   25, 1439,    0,
                                 "10 323031 392d3038 2d32352B 32333A35 39"   },

  {   L_, 2019,      8,   25,-1439,    1, "04 FA61FF7F"                      },
  {   L_, 2019,      8,   25,-1439,    0,
                                 "10 323031 392d3038 2d32352D 32333A35 39"   },

  {   L_, 1930,      4,   15,    0,    1, "02 8000"                          },
  {   L_, 1930,      4,   15,    0,    0,
                                 "10 313933 302d3034 2d31352B 30303A30 30"   },

  {   L_, 1930,      4,   15, 1439,    1, "04 059F8000"                      },
  {   L_, 1930,      4,   15, 1439,    0,
                                 "10 313933 302d3034 2d31352B 32333A35 39"   },

  {   L_, 1930,      4,   15,-1439,    1, "04 FA618000"                      },
  {   L_, 1930,      4,   15,-1439,    0,
                                 "10 313933 302d3034 2d31352D 32333A35 39"   },

  {   L_, 1930,      4,   14,    0,    1, "03 FF7FFF"                        },
  {   L_, 1930,      4,   14,    0,    0,
                                 "10 313933 302d3034 2d31342B 30303A30 30"   },

  {   L_, 1930,      4,   14, 1439,    1, "05 059FFF7FFF"                    },
  {   L_, 1930,      4,   14, 1439,    0,
                                 "10 313933 302d3034 2d31342B 32333A35 39"   },

  {   L_, 1930,      4,   14,-1439,    1, "05 FA61FF7FFF"                    },
  {   L_, 1930,      4,   14,-1439,    0,
                                 "10 313933 302d3034 2d31342D 32333A35 39"   },

  {   L_, 1066,     10,   14,    0,    1, "03 FAB005"                        },
  {   L_, 1066,     10,   14,    0,    0,
                                 "10 313036 362d3130 2d31342B 30303A30 30"   },

  {   L_, 1066,     10,   14, 1439,    1, "05 059FFAB0 05"                   },
  {   L_, 1066,     10,   14, 1439,    0,
                                 "10 313036 362d3130 2d31342B 32333A35 39"   },

  {   L_, 1066,     10,   14,-1439,    1, "05 FA61FAB0 05"                   },
  {   L_, 1066,     10,   14,-1439,    0,
                                 "10 313036 362d3130 2d31342D 32333A35 39"   },

  {   L_, 9999,     12,   31,    0,    1, "03 2C794A"                        },
  {   L_, 9999,     12,   31,    0,    0,
                                 "10 393939 392d3132 2d33312B 30303A30 30"   },

  {   L_, 9999,     12,   31, 1439,    1, "05 059F2C79 4A"                   },
  {   L_, 9999,     12,   31, 1439,    0,
                                 "10 393939 392d3132 2d33312B 32333A35 39"   },

  {   L_, 9999,     12,   31,-1439,    1, "05 FA612C79 4A"                   },
  {   L_, 9999,     12,   31,-1439,    0,
                                 "10 393939 392d3132 2d33312D 32333A35 39"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   OFF   = DATA[i].d_tzoffset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                ASSERT(bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::DateTz VALUE(bdlt::Date(YEAR, MONTH, DAY), OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::DateTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting Time." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms   opt  exp
  //----  -----  ---  ---  ---  ---  ---
  {   L_,     0,   0,   0,   0,   1, "01 00"                                 },
  {   L_,     0,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },

  {   L_,     0,   0,   0, 127,   1, "01 7F"                                 },
  {   L_,     0,   0,   0, 127,   0, "0C 30303A30 303A3030 2E313237"         },

  {   L_,     0,   0,   0, 128,   1, "02 0080"                               },
  {   L_,     0,   0,   0, 128,   0, "0C 30303A30 303A3030 2E313238"         },

  {   L_,     0,   0,   0, 255,   1, "02 00FF"                               },
  {   L_,     0,   0,   0, 255,   0, "0C 30303A30 303A3030 2E323535"         },

  {   L_,     0,   0,   0, 256,   1, "02 0100"                               },
  {   L_,     0,   0,   0, 256,   0, "0C 30303A30 303A3030 2E323536"         },

  {   L_,     0,   0,  32, 767,   1, "02 7FFF"                               },
  {   L_,     0,   0,  32, 767,   0, "0C 30303A30 303A3332 2E373637"         },

  {   L_,     0,   0,  32, 768,   1, "03 008000"                             },
  {   L_,     0,   0,  32, 768,   0, "0C 30303A30 303A3332 2E373638"         },

  {   L_,     2,  19,  48, 607,   1, "03 7FFFFF"                             },
  {   L_,     2,  19,  48, 607,   0, "0C 30323A31 393A3438 2E363037"         },

  {   L_,     2,  19,  48, 608,   1, "04 00800000"                           },
  {   L_,     2,  19,  48, 608,   0, "0C 30323A31 393A3438 2E363038"         },

  {   L_,    12,  33,  45, 999,   1, "04 02B2180F"                           },
  {   L_,    12,  33,  45, 999,   0, "0C 31323A33 333A3435 2E393939"         },

  {   L_,    23,  59,  59, 999,   1, "04 05265BFF"                           },
  {   L_,    23,  59,  59, 999,   0, "0C 32333A35 393A3539 2E393939"         },

  {   L_,    24,   0,   0,   0,   1, "01 00"                                 },
// TBD: Currently doesn't work
// {  L_,    24,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P(MSEC) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Time VALUE(HOUR, MIN, SECS, MSEC);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Time value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                if (24 == HOUR) {
                    LOOP2_ASSERT(LINE, value, bdlt::Time(0) == value);
                }
                else {
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting TimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_offset;    // offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms     off  opt  exp
  //----  -----  ---  ---  ---    ---  ---  ---
  {   L_,     0,   0,   0,   0,     0,  1, "01 00"                           },
  {   L_,     0,   0,   0,   0,     0,  0,
                               "12 30303A30 303A3030 2E303030 2B30303A 3030" },

  {   L_,     0,   0,   0,   0,  1439,  1, "05 059F0000 00"                  },
  {   L_,     0,   0,   0,   0,  1439,  0,
                               "12 30303A30 303A3030 2E303030 2B32333A 3539" },

  {   L_,     0,   0,   0,   0, -1439,  1, "05 FA610000 00"                  },
  {   L_,     0,   0,   0,   0, -1439,  0,
                               "12 30303A30 303A3030 2E303030 2D32333A 3539" },

  {   L_,     0,   0,   0, 127,     0,  1, "01 7F"                           },
  {   L_,     0,   0,   0, 127,     0,  0,
                               "12 30303A30 303A3030 2E313237 2B30303A 3030" },

  {   L_,     0,   0,   0, 127,  1439,  1, "05 059F0000 7F"                  },
  {   L_,     0,   0,   0, 127,  1439,  0,
                               "12 30303A30 303A3030 2E313237 2B32333A 3539" },

  {   L_,     0,   0,   0, 127, -1439,  1, "05 FA610000 7F"                  },
  {   L_,     0,   0,   0, 127, -1439,  0,
                               "12 30303A30 303A3030 2E313237 2D32333A 3539" },

  {   L_,     0,   0,   0, 128,     0,  1, "02 0080"                         },
  {   L_,     0,   0,   0, 128,     0,  0,
                               "12 30303A30 303A3030 2E313238 2B30303A 3030" },

  {   L_,     0,   0,   0, 128,  1439,  1, "05 059F0000 80"                  },
  {   L_,     0,   0,   0, 128,  1439,  0,
                               "12 30303A30 303A3030 2E313238 2B32333A 3539" },

  {   L_,     0,   0,   0, 128, -1439,  1, "05 FA610000 80"                  },
  {   L_,     0,   0,   0, 128, -1439,  0,
                               "12 30303A30 303A3030 2E313238 2D32333A 3539" },

  {   L_,     0,   0,  32, 767,     0,  1, "02 7FFF"                         },
  {   L_,     0,   0,  32, 767,     0,  0,
                               "12 30303A30 303A3332 2E373637 2B30303A 3030" },

  {   L_,     0,   0,  32, 767,  1439,  1, "05 059F007F FF"                  },
  {   L_,     0,   0,  32, 767,  1439,  0,
                               "12 30303A30 303A3332 2E373637 2B32333A 3539" },

  {   L_,     0,   0,  32, 767, -1439,  1, "05 FA61007F FF"                  },
  {   L_,     0,   0,  32, 767, -1439,  0,
                               "12 30303A30 303A3332 2E373637 2D32333A 3539" },

  {   L_,     0,   0,  32, 768,     0,  1, "03 008000"                       },
  {   L_,     0,   0,  32, 768,     0,  0,
                               "12 30303A30 303A3332 2E373638 2B30303A 3030" },

  {   L_,     0,   0,  32, 768,  1439,  1, "05 059F0080 00"                  },
  {   L_,     0,   0,  32, 768,  1439,  0,
                               "12 30303A30 303A3332 2E373638 2B32333A 3539" },

  {   L_,     0,   0,  32, 768, -1439,  1, "05 FA610080 00"                  },
  {   L_,     0,   0,  32, 768, -1439,  0,
                               "12 30303A30 303A3332 2E373638 2D32333A 3539" },

  {   L_,     2,  19,  48, 607,     0,  1, "03 7FFFFF"                       },
  {   L_,     2,  19,  48, 607,     0,  0,
                               "12 30323A31 393A3438 2E363037 2B30303A 3030" },

  {   L_,     2,  19,  48, 607,  1439,  1, "05 059F7FFFFF"                   },
  {   L_,     2,  19,  48, 607,  1439,  0,
                               "12 30323A31 393A3438 2E363037 2B32333A 3539" },

  {   L_,     2,  19,  48, 607, -1439,  1, "05 FA617FFFFF"                   },
  {   L_,     2,  19,  48, 607, -1439,  0,
                               "12 30323A31 393A3438 2E363037 2D32333A 3539" },

  {   L_,     2,  19,  48, 608,     0,  1, "04 00800000"                     },
  {   L_,     2,  19,  48, 608,     0,  0,
                               "12 30323A31 393A3438 2E363038 2B30303A 3030" },

  {   L_,     2,  19,  48, 608,  1439,  1, "06 059F0080 0000"                },
  {   L_,     2,  19,  48, 608,  1439,  0,
                               "12 30323A31 393A3438 2E363038 2B32333A 3539" },

  {   L_,     2,  19,  48, 608, -1439,  1, "06 FA610080 0000"                },
  {   L_,     2,  19,  48, 608, -1439,  0,
                               "12 30323A31 393A3438 2E363038 2D32333A 3539" },

  {   L_,    12,  33,  45, 999,     0,  1, "04 02B2180F"                     },
  {   L_,    12,  33,  45, 999,     0,  0,
                               "12 31323A33 333A3435 2E393939 2B30303A 3030" },

  {   L_,    12,  33,  45, 999,  1439,  1, "06 059F02B2 180F"                },
  {   L_,    12,  33,  45, 999,  1439,  0,
                               "12 31323A33 333A3435 2E393939 2B32333A 3539" },

  {   L_,    12,  33,  45, 999, -1439,  1, "06 FA6102B2 180F"                },
  {   L_,    12,  33,  45, 999, -1439,  0,
                               "12 31323A33 333A3435 2E393939 2D32333A 3539" },

  {   L_,    23,  59,  59, 999,     0,  1, "04 05265BFF"                     },
  {   L_,    23,  59,  59, 999,     0,  0,
                               "12 32333A35 393A3539 2E393939 2B30303A 3030" },

  {   L_,    23,  59,  59, 999,  1439,  1, "06 059F0526 5BFF"                },
  {   L_,    23,  59,  59, 999,  1439,  0,
                               "12 32333A35 393A3539 2E393939 2B32333A 3539" },

  {   L_,    23,  59,  59, 999, -1439,  1, "06 FA610526 5BFF"                },
  {   L_,    23,  59,  59, 999, -1439,  0,
                               "12 32333A35 393A3539 2E393939 2D32333A 3539" },

  {   L_,    24,   0,   0,   0,     0,  1, "01 00"                           },
// TBD: Current doesn't work
//{   L_,    24,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P_(MSEC) P(OFF) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::TimeTz VALUE(bdlt::Time(HOUR, MIN, SECS, MSEC),
                                         OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::TimeTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                if (24 == HOUR) {
                    LOOP2_ASSERT(LINE, value,
                                bdlt::TimeTz(bdlt::Time(0), OFF) == value);
                }
                else {
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year  mon  day  hour   min  sec    ms   us   opt  exp
  //----  ----- ---  ---  ----   ---  ---    --   --   ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   0,   1,
                                               "01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    0,   0,    1,   0,   1,
                                               "01 01"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    1,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   1,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,   0,   1,
                                               "01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127, 127,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 313237 313237" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,   0,   1,
                                               "02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128, 128,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 313238 313238" },

  {   L_, 2020,   1,   1,    0,    0,   0,  999,   0,   1,
                                               "02 03E7"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  999, 999,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30302E 393939 393939" },

  {   L_, 2020,   1,   1,    0,    0,   1,    0,   0,   1,
                                               "02 03E8"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    0,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30312E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    0,   1,    1,   0,   1,
                                               "02 03E9"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    1,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30312E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    0,   1,    0,   1,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A30312E 303030 303031" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,   0,   1,
                                               "02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767, 898,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A33322E 373637 383938" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,   0,   1,
                                               "03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768, 898,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A33322E 373638 383938" },

  {   L_, 2020,   1,   1,    0,    0,  59,  999,   0,   1,
                                               "03 00EA5F"                   },
  {   L_, 2020,   1,   1,    0,    0,  59,  999, 999,   0,
             "1A 32303230 2d30312d 30315430 303A3030 3A35392E 393939 393939" },

  {   L_, 2020,   1,   1,    0,    1,   0,    0,   0,   1,
                                               "03 00EA60"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    0,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3031 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    1,   0,    1,   0,   1,
                                               "03 00EA61"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    1,   0,   0,
             "1A 32303230 2d30312d 30315430 303A3031 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    1,   0,    0,   1,   0,
             "1A 32303230 2d30312d 30315430 303A3031 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    0,   59,  59,  999,   0,   1,
                                               "03 36EE7F"                   },
  {   L_, 2020,   1,   1,    0,   59,  59,  999, 999,   0,
             "1A 32303230 2d30312d 30315430 303A3539 3A35392E 393939 393939" },

  {   L_, 2020,   1,   1,    1,    0,   0,    0,   0,   1,
                                               "03 36EE80"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    0,   0,   0,
             "1A 32303230 2d30312d 30315430 313A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    1,    0,   0,    1,   0,   1,
                                               "03 36EE81"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    1,   0,   0,
             "1A 32303230 2d30312d 30315430 313A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    1,    0,   0,    0,   1,   0,
             "1A 32303230 2d30312d 30315430 313A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,   0,   1,
                                               "03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607, 809,   0,
             "1A 32303230 2d30312d 30315430 323A3139 3A34382E 363037 383039" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,   0,   1,
                                               "04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608, 901,   0,
             "1A 32303230 2d30312d 30315430 323A3139 3A34382E 363038 393031" },

  {   L_, 2020,   1,   1,   23,   59,  59,  999,   0,   1,
                                               "04 05265BFF"                 },
  {   L_, 2020,   1,   1,   23,   59,  59,  999, 999,   0,
             "1A 32303230 2d30312d 30315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   1,   2,    0,    0,   0,    0,   0,   1,
                                               "04 05265C00"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    0,   0,   0,
             "1A 32303230 2d30312d 30325430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   2,    0,    0,   0,    1,   0,   1,
                                               "04 05265C01"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    1,   0,   0,
             "1A 32303230 2d30312d 30325430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   2,    0,    0,   0,    0,   1,   0,
             "1A 32303230 2d30312d 30325430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,   0,   1,
                                               "04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,   534,   0,
             "1A 32303230 2d30312d 32355432 303A3331 3A32332E 363437 353334" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,   0,   1,
                                               "05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,   537,   0,
             "1A 32303230 2d30312d 32355432 303A3331 3A32332E 363438 353337" },

  {   L_, 2020,   1,  25,   20,   31,  23,  649,   0,   1,
                                               "05 00800000 01"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  649,   538,   0,
             "1A 32303230 2d30312d 32355432 303A3331 3A32332E 363439 353338" },

  {   L_, 2020,   1,  31,   23,   59,  59,  999,   0,   1,
                                               "05 009FA523 FF"              },
  {   L_, 2020,   1,  31,   23,   59,  59,  999, 999,   0,
             "1A 32303230 2d30312d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   2,   1,    0,    0,   0,    0,   0,   1,
                                               "05 009FA524 00"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    0,   0,   0,
             "1A 32303230 2d30322d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   2,   1,    0,    0,   0,    1,   0,   1,
                                               "05 009FA524 01"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    1,   0,   0,
             "1A 32303230 2d30322d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   2,   1,    0,    0,   0,    0,   1,   0,
             "1A 32303230 2d30322d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   2,  28,   23,   59,  59,  999,   0,   1,
                                               "05 012FD733 FF"              },
  {   L_, 2020,   2,  28,   23,   59,  59,  999, 999,   0,
             "1A 32303230 2d30322d 32385432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   2,  29,    0,    0,   0,    0,   0,   1,
                                               "05 012FD734 00"              },
  {   L_, 2020,   2,  29,    0,    0,   0,    0,   0,   0,
             "1A 32303230 2d30322d 32395430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   2,  29,   23,   59,  59,  999,   0,   1,
                                               "05 0134FD8F FF"              },
  {   L_, 2020,   2,  29,   23,   59,  59,  999, 999,   0,
             "1A 32303230 2d30322d 32395432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   3,   1,    0,    0,   0,    0,   0,   1,
                                               "05 0134FD90 00"              },
  {   L_, 2020,   3,   1,    0,    0,   0,    0,   0,   0,
             "1A 32303230 2d30332d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,  12,  31,   23,   59,  59,  999,   0,   1,
                                               "05 075CD787 FF"              },
  {   L_, 2020,  12,  31,   23,   59,  59,  999, 999,   0,
             "1A 32303230 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2021,   1,   1,    0,    0,   0,    0,   0,   1,
                                               "05 075CD788 00"              },
  {   L_, 2021,   1,   1,    0,    0,   0,    0,   0,   0,
             "1A 32303231 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2023,  12,  31,   23,   59,  59,  999,   0,   1,
                                               "05 1D63EB0B FF"              },
  {   L_, 2023,  12,  31,   23,   59,  59,  999, 999,   0,
             "1A 32303233 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2024,   1,   1,    0,    0,   0,    0,   0,   1,
                                               "05 1D63EB0C 00"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    0,   0,   0,
             "1A 32303234 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2024,   1,   1,    0,    0,   0,    1,   0,   1,
                                               "05 1D63EB0C 01"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    1,   0,   0,
             "1A 32303234 2d30312d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2024,   1,   1,    0,    0,   0,    0,   1,   0,
             "1A 32303234 2d30312d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,   0,   1,
                                               "05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 887,   0,
             "1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383837" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,   0,   1,
                                               "06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 888,   0,
             "1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383838" },

  {   L_, 2037,   6,   2,   21,   56,  53,  889,   0,   1,
                                               "06 00800000 0001"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 889,   0,
             "1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383839" },

  {   L_, 2099,  12,  31,   23,   59,  59,  999,   0,   1,
                                               "06 024BCE5C EFFF"            },
  {   L_, 2099,  12,  31,   23,   59,  59,  999, 999,   0,
             "1A 32303939 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2100,   1,   1,    0,    0,   0,    0,   0,   1,
                                               "06 024BCE5C F000"            },
  {   L_, 2100,   1,   1,    0,    0,   0,    0,   0,   0,
             "1A 32313030 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,   0,   1,
                                               "06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327, 216,   0,
             "1A 36343739 2d31302d 31375430 323A3435 3A35352E 333237 323136" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,   0,   1,
                                               "09 00000080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328, 217,   0,
             "1A 36343739 2d31302d 31375430 323A3435 3A35352E 333238 323137" },

  {   L_, 6479,  10,  17,    2,   45,  55,  329,   0,   1,
                                               "09 00000080 00000000 01"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  329, 218,   0,
             "1A 36343739 2d31302d 31375430 323A3435 3A35352E 333239 323138" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,   0,   1,
                                               "01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999, 999,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,   0,   1,
                                               "01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872, 761,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 383732 373631" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,   0,   1,
                                               "02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871, 760,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 383731 373630" },

  {   L_, 2019,  12,  31,   23,   59,  59,    1,   0,   1,
                                               "02 FC19"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    1,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 303031 303030" },
  {   L_, 2019,  12,  31,   23,   59,  59,    0,   1,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 303030 303031" },

  {   L_, 2019,  12,  31,   23,   59,  59,    0,   0,   1,
                                               "02 FC18"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    0,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35392E 303030 303030" },

  {   L_, 2019,  12,  31,   23,   59,  58,  999,   0,   1,
                                               "02 FC17"                     },
  {   L_, 2019,  12,  31,   23,   59,  58,  999, 999,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A35382E 393939 393939" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,   0,   1,
                                               "02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232, 343,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A32372E 323332 333433" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,   0,   1,
                                               "03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231, 342,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A32372E 323331 333432" },

  {   L_, 2019,  12,  31,   23,   59,   0,    1,   0,   1,
                                               "03 FF15A1"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    1,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A30302E 303031 303030" },
  {   L_, 2019,  12,  31,   23,   59,   0,    0,   1,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A30302E 303030 303031" },

  {   L_, 2019,  12,  31,   23,   59,   0,    0,   0,   1,
                                               "03 FF15A0"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    0,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3539 3A30302E 303030 303030" },

  {   L_, 2019,  12,  31,   23,   58,  59,  999,   0,   1,
                                               "03 FF159F"                   },
  {   L_, 2019,  12,  31,   23,   58,  59,  999, 999,   0,
             "1A 32303139 2d31322d 33315432 333A3538 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   23,    0,   0,    1,   0,   1,
                                               "03 C91181"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    1,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3030 3A30302E 303031 303030" },

  {   L_, 2019,  12,  31,   23,    0,   0,    0,   0,   1,
                                               "03 C91180"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    0,   0,   0,
             "1A 32303139 2d31322d 33315432 333A3030 3A30302E 303030 303030" },

  {   L_, 2019,  12,  31,   22,   59,  59,  999,   0,   1,
                                               "03 C9117F"                   },
  {   L_, 2019,  12,  31,   22,   59,  59,  999, 999,   0,
             "1A 32303139 2d31322d 33315432 323A3539 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,   0,   1,
                                               "03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392, 281,   0,
             "1A 32303139 2d31322d 33315432 313A3430 3A31312E 333932 323831" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,   0,   1,
                                               "04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391, 280,   0,
             "1A 32303139 2d31322d 33315432 313A3430 3A31312E 333931 323830" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,   0,   1,
                                               "04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352, 463,   0,
             "1A 32303139 2d31322d 30375430 333A3238 3A33362E 333532 343633" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,   0,   1,
                                               "05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351, 462,   0,
             "1A 32303139 2d31322d 30375430 333A3238 3A33362E 333531 343632" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,   0,   1,
                                               "05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112, 223,   0,
             "1A 32303032 2d30372d 33315430 323A3033 3A30362E 313132 323233" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,   0,   1,
                                               "06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111, 222,   0,
             "1A 32303032 2d30372d 33315430 323A3033 3A30362E 313131 323232" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,   0,   1,
                                               "06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,   0,   0,
             "1A 30303031 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,   0,   1,
                                               "09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999, 999,   0,
             "1A 39393939 2d31322d 33315432 333A3539 3A35392E 393939 393939" },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);
                options.setDatetimeFractionalSecondPrecision(6);

                const bdlt::Datetime VALUE(YEAR, MONTH, DAY,
                                          HOUR, MIN, SECS, MSEC, USEC);

                bdlsb::MemOutStreamBuf osb;
                if (i % 3 == 0 && !BIN) {
                    // Test the default options for some of the values.
                    ASSERT(0 == Util::putValue(&osb, VALUE));
                }
                else {
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                }
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Datetime value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }


        if (verbose) bsl::cout << "\nTesting Datetime "
                               << "with milliseconds precision."
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year  mon  day  hour   min  sec    ms   us  exp
  //----  ----- ---  ---  ----   ---  ---    --   --  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 303030" },
  {   L_, 2020,   1,   1,    0,    0,   0,    1,   0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 303031" },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,   0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 313237" },
  {   L_, 2019,  12,  31,   23,   59,  59,    1,   0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 303031" },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,   0,
                    "17 39393939 2d31322d 33315432 333A3539 3A35392E 393939" },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                // options.setDatetimeFractionalSecondPrecision(3); // default

                const bdlt::Datetime VALUE(YEAR, MONTH, DAY,
                                          HOUR, MIN, SECS, MSEC, USEC);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Datetime value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting DatetimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                int         d_offset;    // timezone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //line  year  mon  day  hour   min  sec    ms    us    off    opt  exp
  //----  ----- ---  ---  ----   ---  ---    --    --    ---    ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,     0,     1,
                                               "01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,     0,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,  1439,     1,
                                               "07 059F0000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,  1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0, -1439,     1,
                                               "07 FA610000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0, -1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,     0,     1,
                                               "01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127,     0,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,  1439,     1,
                                               "07 059F0000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127,  1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0, -1439,     1,
                                               "07 FA610000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127, -1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0,     0,     1,
                                               "02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128,     0,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0,  1439,     1,
                                               "07 059F0000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128,  1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0, -1439,     1,
                                               "07 FA610000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128, -1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0,     0,     1,
                                               "02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  767,     0,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363737 36372B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0,  1439,     1,
                                               "07 059F0000 007FFF"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  767,  1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363737 36372B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0, -1439,     1,
                                               "07 FA610000 007FFF"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  767, -1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363737 36372D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0,     0,     1,
                                               "03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768,     0,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0,  1439,     1,
                                               "07 059F0000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768,  1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0, -1439,     1,
                                               "07 FA610000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768, -1439,     0,
"20 32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382D32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0,     0,     1,
                                               "03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607,     0,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372B30 303A3030" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0,  1439,     1,
                                               "07 059F0000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607,  1439,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372B32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0, -1439,     1,
                                               "07 FA610000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607, -1439,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372D32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0,     0,     1,
                                               "04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608,     0,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382B30 303A3030" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0,  1439,     1,
                                               "07 059F0000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608,  1439,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382B32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0, -1439,     1,
                                               "07 FA610000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608, -1439,     0,
"20 32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382D32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0,     0,     1,
                                               "04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647,     0,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372B30 303A3030" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0,  1439,     1,
                                               "07 059F007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647,  1439,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372B32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0, -1439,     1,
                                               "07 FA61007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647, -1439,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372D32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0,     0,     1,
                                               "05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648,     0,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382B30 303A3030" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0,  1439,     1,
                                               "07 059F0080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648,  1439,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382B32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0, -1439,     1,
                                               "07 FA610080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648, -1439,     0,
"20 32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382D32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0,     0,     1,
                                               "05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887,     0,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372B30 303A3030" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0,  1439,     1,
                                               "07 059F7FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887,  1439,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372B32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0, -1439,     1,
                                               "07 FA617FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887, -1439,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372D32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0,     0,     1,
                                               "06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888,     0,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382B30 303A3030" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0,  1439,     1,
                                               "08 059F0080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888,  1439,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382B32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0, -1439,     1,
                                               "08 FA610080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888, -1439,     0,
"20 32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382D32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,     0,     1,
                                               "06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327,     0,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372B30 303A3030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,  1439,     1,
                                               "08 059F7FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327,  1439,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372B32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0, -1439,     1,
                                               "08 FA617FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327, -1439,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372D32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0,     0,     1,
                                               "09 00000080 00000000 00 "    },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328,     0,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382B30 303A3030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0,  1439,     1,
                                               "09 059F0080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328,  1439,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382B32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0, -1439,     1,
                                               "09 FA610080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328, -1439,     0,
"20 36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0,     0,     1,
                                               "01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999,     0,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0,  1439,     1,
                                               "07 059FFFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999,  1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0, -1439,     1,
                                               "07 FA61FFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999, -1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0,     0,     1,
                                               "01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872,     0,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0,  1439,     1,
                                               "07 059FFFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872,  1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0, -1439,     1,
                                               "07 FA61FFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872, -1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0,     0,     1,
                                               "02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871,     0,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0,  1439,     1,
                                               "07 059FFFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871,  1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0, -1439,     1,
                                               "07 FA61FFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871, -1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0,     0,     1,
                                               "02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232,     0,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0,  1439,     1,
                                               "07 059FFFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232,  1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0, -1439,     1,
                                               "07 FA61FFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232, -1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0,     0,     1,
                                               "03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231,     0,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0,  1439,     1,
                                               "07 059FFFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231,  1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0, -1439,     1,
                                               "07 FA61FFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231, -1439,     0,
"20 32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312D32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0,     0,     1,
                                               "03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392,     0,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322B30 303A3030" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0,  1439,     1,
                                               "07 059FFFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392,  1439,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322B32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0, -1439,     1,
                                               "07 FA61FFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392, -1439,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322D32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0,     0,     1,
                                               "04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391,     0,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312B30 303A3030" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0,  1439,     1,
                                               "07 059FFFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391,  1439,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312B32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0, -1439,     1,
                                               "07 FA61FFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391, -1439,     0,
"20 32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312D32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0,     0,     1,
                                               "04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352,     0,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322B30 303A3030" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0,  1439,     1,
                                               "07 059FFF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352,  1439,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322B32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0, -1439,     1,
                                               "07 FA61FF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352, -1439,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322D32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0,     0,     1,
                                               "05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351,     0,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312B30 303A3030" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0,  1439,     1,
                                               "07 059FFF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351,  1439,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312B32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0, -1439,     1,
                                               "07 FA61FF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351, -1439,     0,
"20 32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312D32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0,     0,     1,
                                               "05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112,     0,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322B30 303A3030" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0,  1439,     1,
                                               "07 059F8000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112,  1439,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322B32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0, -1439,     1,
                                               "07 FA618000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112, -1439,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322D32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0,     0,     1,
                                               "06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111,     0,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312B30 303A3030" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0,  1439,     1,
                                               "08 059FFF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111,  1439,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312B32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0, -1439,     1,
                                               "08 FA61FF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111, -1439,     0,
"20 32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312D32 333A3539" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,     0,     1,
                                               "06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,     0,     0,
"20 30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302B30 303A3030" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,  1439,     1,
                                               "08 059FC60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,  1439,     0,
"20 30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302B32 333A3539" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0, -1439,     1,
                                               "08 FA61C60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0, -1439,     0,
"20 30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302D32 333A3539" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,     0,     1,
                                               "09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999,     0,     0,
"20 39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392B30 303A3030" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,  1439,     1,
                                               "09 059F00E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999,  1439,     0,
"20 39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392B32 333A3539" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0, -1439,     1,
                                               "09 FA6100E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999, -1439,     0,
"20 39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392D32 333A3539" },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);
                options.setDatetimeFractionalSecondPrecision(6);

                const bdlt::DatetimeTz VALUE(bdlt::Datetime(YEAR, MONTH, DAY,
                                                            HOUR, MIN, SECS,
                                                            MSEC, USEC),
                                             OFF);

                bdlsb::MemOutStreamBuf osb;
                if (i % 3 == 0 && !BIN) {
                    // Test the default options for some of the values.
                    ASSERT(0 == Util::putValue(&osb, VALUE));
                }
                else {
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                }
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP3_ASSERT(LINE, osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::DatetimeTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting DatetimeTz "
                               << "with milliseconds precision."
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                int         d_offset;    // timezone offset
                const char *d_exp;       // expected output
            } DATA[] = {
  //line  year  mon  day  hour   min  sec    ms    us    off    exp
  //----  ----- ---  ---  ----   ---  ---    --    --    ---    ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302B 30303A30 30" },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,   1439,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302B 32333A35 39" },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,  -1439,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302D 32333A35 39" },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372B 30303A30 30" },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,   1439,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372B 32333A35 39" },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,  -1439,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372D 32333A35 39" },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,   1439,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332372B 32333A35 39" },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,  -1439,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332372D 32333A35 39" },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,   1439,
      "1D 39393939 2d31322d 33315432 333A3539 3A35392E 3939392B 32333A35 39" },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,  -1439,
      "1D 39393939 2d31322d 33315432 333A3539 3A35392E 3939392D 32333A35 39" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const int   OFF   = DATA[i].d_offset;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                // options.setDatetimeFractionalSecondPrecision(3); // default

                const bdlt::DatetimeTz VALUE(bdlt::Datetime(YEAR, MONTH, DAY,
                                                            HOUR, MIN, SECS,
                                                            MSEC, USEC),
                                             OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP3_ASSERT(LINE, osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::DatetimeTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &value, &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING BRUTE FORCE 'putValue'/'getValue' FOR DATE/TIME
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                           "TESTING 'putValue', 'getValue' FOR DATE/TIME" "\n"
                           "============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nTesting Date Brute force." << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int i = 0; i < NUM_YEARS; ++i) {
            for (int j = 0; j < NUM_MONTHS; ++j) {
            for (int k = 0; k < NUM_DAYS; ++k) {

                const int YEAR  = YEARS[i];
                const int MONTH = MONTHS[j];
                const int DAY   = DAYS[k];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::ProlepticDateImpUtil::isValidYearMonthDay(YEAR,
                                                                    MONTH,
                                                                    DAY)) {

                    if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }

                    const bdlt::Date VALUE(YEAR, MONTH, DAY);
                    bdlt::Date value;

                    const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                    const bdlt::DateTz VALUE1(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF1);
                    const bdlt::DateTz VALUE2(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF2);
                    const bdlt::DateTz VALUE3(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF3);
                    bdlt::DateTz value1, value2, value3;

                    {
                        bdlsb::MemOutStreamBuf osb;
                        ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                        const int LENGTH = static_cast<int>(osb.length());

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb.data(), osb.length());
                        }
                        int numBytesConsumed = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(SUCCESS == Util::getValue(&isb,
                                                         &value,
                                                         &numBytesConsumed));
                        ASSERT(0      == isb.length());
                        ASSERT(LENGTH == numBytesConsumed);
                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                        const int LENGTH = static_cast<int>(osb.length());

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb.data(), osb.length());
                        }
                        int numBytesConsumed = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(SUCCESS == Util::getValue(&isb,
                                                         &value,
                                                         &numBytesConsumed));
                        ASSERT(0      == isb.length());
                        ASSERT(LENGTH == numBytesConsumed);
                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                        ASSERT(0 == Util::putValue(&osb1, VALUE1, &options));
                        const int LENGTH1 = static_cast<int>(osb1.length());

                        ASSERT(0 == Util::putValue(&osb2, VALUE2, &options));
                        const int LENGTH2 = static_cast<int>(osb2.length());

                        ASSERT(0 == Util::putValue(&osb3, VALUE3, &options));
                        const int LENGTH3 = static_cast<int>(osb3.length());

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }
                        int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(SUCCESS ==
                               Util::getValue(&isb1, &value1, &nbc1));
                        ASSERT(SUCCESS ==
                               Util::getValue(&isb2, &value2, &nbc2));
                        ASSERT(SUCCESS ==
                               Util::getValue(&isb3, &value3, &nbc3));

                        ASSERT(0       == isb1.length());
                        ASSERT(0       == isb2.length());
                        ASSERT(0       == isb3.length());

                        ASSERT(LENGTH1 == nbc1);
                        ASSERT(LENGTH2 == nbc2);
                        ASSERT(LENGTH3 == nbc3);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                        ASSERT(0 == Util::putValue(&osb1, VALUE1, &DEFOPTS));
                        const int LENGTH1 = static_cast<int>(osb1.length());

                        ASSERT(0 == Util::putValue(&osb2, VALUE2, &DEFOPTS));
                        const int LENGTH2 = static_cast<int>(osb2.length());

                        ASSERT(0 == Util::putValue(&osb3, VALUE3, &DEFOPTS));
                        const int LENGTH3 = static_cast<int>(osb3.length());

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }
                        int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(SUCCESS ==
                               Util::getValue(&isb1, &value1, &nbc1));
                        ASSERT(SUCCESS ==
                               Util::getValue(&isb2, &value2, &nbc2));
                        ASSERT(SUCCESS ==
                               Util::getValue(&isb3, &value3, &nbc3));

                        ASSERT(0       == isb1.length());
                        ASSERT(0       == isb2.length());
                        ASSERT(0       == isb3.length());

                        ASSERT(LENGTH1 == nbc1);
                        ASSERT(LENGTH2 == nbc2);
                        ASSERT(LENGTH3 == nbc3);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                    }
                }
            }
            }
            }
        }

        if (verbose) bsl::cout << "\nTesting Time Brute force." << bsl::endl;
        {
            for (int hour = 0; hour <= 23; ++hour) {
                for (int min = 0; min < 60; ++min) {
                    for (int sec = 0; sec < 60; ++sec) {

                        if (veryVerbose) { P_(hour) P_(min) P(sec) }

                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::TimeTz VALUE1(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS1),
                                                 OFF1);
                        const bdlt::TimeTz VALUE2(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS2),
                                                 OFF2);
                        const bdlt::TimeTz VALUE3(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS3),
                                                 OFF3);
                        bdlt::TimeTz value1, value2, value3;

                        {
                            const int MS = 0;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            const int MS = 999;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &options));
                            const int LENGTH1 = static_cast<int>(osb1.length());

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &options));
                            const int LENGTH2 = static_cast<int>(osb2.length());

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &options));
                            const int LENGTH3 = static_cast<int>(osb3.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb1, &value1, &nbc1));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb2, &value2, &nbc2));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb3, &value3, &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }

                        {
                            const int MS = 0;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &DEFOPTS));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            const int MS = 999;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &DEFOPTS));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &DEFOPTS));
                            const int LENGTH1 = static_cast<int>(osb1.length());

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &DEFOPTS));
                            const int LENGTH2 = static_cast<int>(osb2.length());

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &DEFOPTS));
                            const int LENGTH3 = static_cast<int>(osb3.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb1, &value1, &nbc1));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb2, &value2, &nbc2));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb3, &value3, &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime Brute force."
                               << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int di = 0; di < NUM_YEARS; ++di) {
            for (int dj = 0; dj < NUM_MONTHS; ++dj) {
            for (int dk = 0; dk < NUM_DAYS; ++dk) {

                const int YEAR  = YEARS[di];
                const int MONTH = MONTHS[dj];
                const int DAY   = DAYS[dk];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::ProlepticDateImpUtil::isValidYearMonthDay(YEAR,
                                                                    MONTH,
                                                                    DAY)) {

                    const int HOURS[] = { 0, 12, 23 };
                    const int NUM_HOURS = sizeof HOURS / sizeof *HOURS;

                    const int MINS[] = { 0, 30, 59 };
                    const int NUM_MINS = sizeof MINS / sizeof *MINS;

                    const int SECONDS[] = { 0, 30, 59 };
                    const int NUM_SECS = sizeof SECONDS / sizeof *SECONDS;

                    for (int ti = 0; ti < NUM_HOURS; ++ti) {
                    for (int tj = 0; tj < NUM_MINS; ++tj) {
                    for (int tk = 0; tk < NUM_SECS; ++tk) {

                        const int HOUR = HOURS[ti];
                        const int MIN  = MINS[tj];
                        const int SECS = SECONDS[tk];

                        if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                        if (veryVerbose) { P_(HOUR) P_(MIN) P(SECS) }

                        const int MS = 0;
                        const bdlt::Date DATE(YEAR, MONTH, DAY);
                        const bdlt::Time TIME(HOUR, MIN, SECS, MS);
                        const bdlt::Datetime VALUE(DATE, TIME);
                        bdlt::Datetime value;

                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::Date DATE1(YEAR, MONTH, DAY);
                        const bdlt::Time TIME1(HOUR, MIN, SECS, MS1);

                        const bdlt::Date DATE2(YEAR, MONTH, DAY);
                        const bdlt::Time TIME2(HOUR, MIN, SECS, MS2);

                        const bdlt::Date DATE3(YEAR, MONTH, DAY);
                        const bdlt::Time TIME3(HOUR, MIN, SECS, MS3);

                        const bdlt::Datetime DT1(DATE1, TIME1);
                        const bdlt::Datetime DT2(DATE2, TIME2);
                        const bdlt::Datetime DT3(DATE3, TIME3);

                        const bdlt::DatetimeTz VALUE1(DT1, OFF1);
                        const bdlt::DatetimeTz VALUE2(DT2, OFF2);
                        const bdlt::DatetimeTz VALUE3(DT3, OFF3);

                        bdlt::DatetimeTz value1, value2, value3;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &options));
                            const int LENGTH1 = static_cast<int>(osb1.length());

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &options));
                            const int LENGTH2 = static_cast<int>(osb2.length());

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &options));
                            const int LENGTH3 = static_cast<int>(osb3.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb1, &value1, &nbc1));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb2, &value2, &nbc2));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb3, &value3, &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                            const int LENGTH = static_cast<int>(osb.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS ==
                                   Util::getValue(
                                       &isb, &value, &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &DEFOPTS));
                            const int LENGTH1 = static_cast<int>(osb1.length());

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &DEFOPTS));
                            const int LENGTH2 = static_cast<int>(osb2.length());

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &DEFOPTS));
                            const int LENGTH3 = static_cast<int>(osb3.length());

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb1, &value1, &nbc1));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb2, &value2, &nbc2));
                            ASSERT(SUCCESS ==
                                   Util::getValue(&isb3, &value3, &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }
                    }
                    }
                    }
                }
            }
            }
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'putValue', 'getValue' FOR DATE/TIME
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                           "TESTING 'putValue', 'getValue' FOR DATE/TIME" "\n"
                           "============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                const Type VALUE(Y, M, D); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error (Testing undefined behavior)
#ifndef BSLS_ASSERT_IS_ACTIVE
            {
                const Type VALUE1(0, 1, 1);
                const Type VALUE2(1, 0, 1);
                const Type VALUE3(1, 1, 0);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));

                // The date value is converted to a valid date in this case and
                // the error remains undetected.  This is fine since the user
                // is already in undefined behavior and our error is at best a
                // convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE2));
//                 ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE3));
                ASSERT(0 != Util::putValue(&osb, VALUE3, &options));
            }
#endif
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error (Testing undefined behavior)
#ifndef BSLS_ASSERT_IS_ACTIVE
            {
                const Type VALUE1(bdlt::Date(0, 1, 1), 0);
                const Type VALUE2(bdlt::Date(0, 1, 1), 2000);
                const Type VALUE3(bdlt::Date(1, 0, 1), 0);
                const Type VALUE4(bdlt::Date(1, 0, 1), 2000);
                const Type VALUE5(bdlt::Date(1, 1, 0), 0);
                const Type VALUE6(bdlt::Date(1, 1, 0), 2000);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE2));
                ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                // The date value is converted to a valid date in this case and
                // the error remains undetected.  This is fine since the user
                // is already in undefined behavior and our error is at best a
                // convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE3));
//                 ASSERT(0 != Util::putValue(&osb, VALUE3, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE4));
                ASSERT(0 != Util::putValue(&osb, VALUE4, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE5));
                ASSERT(0 != Util::putValue(&osb, VALUE5, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE6));
                ASSERT(0 != Util::putValue(&osb, VALUE6, &options));
            }
#endif
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(H, MM, S, MS); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error (Testing undefined behavior)
#ifndef BSLS_ASSERT_IS_ACTIVE
            {
                bdlt::Datetime value1(bdlt::Date(1, 1, 1));
                value1.addDays(-1);

                bdlt::Datetime value2(bdlt::Date(9999, 12, 31));
                value2.addDays(1);

                const Type VALUE1(value1);
                const Type VALUE2(value2);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE2));
                ASSERT(0 != Util::putValue(&osb, VALUE2, &options));
            }
#endif
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = static_cast<int>(osb.length());

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS ==
                           Util::getValue(&isb, &value, &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error (Testing undefined behavior)
#ifndef BSLS_ASSERT_IS_ACTIVE
            {
                const Type VALUE1(bdlt::Datetime(bdlt::Date(0, 1, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE2(bdlt::Datetime(bdlt::Date(0, 1, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);
                const Type VALUE3(bdlt::Datetime(bdlt::Date(1, 0, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE4(bdlt::Datetime(bdlt::Date(1, 0, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);
                const Type VALUE5(bdlt::Datetime(bdlt::Date(1, 1, 0),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE6(bdlt::Datetime(bdlt::Date(1, 1, 0),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE2));
                ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                // The date value is converted to a valid date in this case and
                // the error remains undetected.  This is fine since the user
                // is already in undefined behavior and our error is at best a
                // convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE3));
//                 ASSERT(0 != Util::putValue(&osb, VALUE3, &options));
//                 ASSERT(0 != Util::putValue(&osb, VALUE4));
//                 ASSERT(0 != Util::putValue(&osb, VALUE4, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE5));
                ASSERT(0 != Util::putValue(&osb, VALUE5, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE6));
                ASSERT(0 != Util::putValue(&osb, VALUE6, &options));
            }
#endif
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'put/getIdentifierOctets'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                                      "TESTING 'put/getIdentifierOctets'" "\n"
                                      "=================================" "\n";

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            enum {
                UNIVERSAL        = balber::BerConstants::e_UNIVERSAL,
                APPLICATION      = balber::BerConstants::e_APPLICATION,
                CONTEXT_SPECIFIC = balber::BerConstants::e_CONTEXT_SPECIFIC,
                PRIVATE          = balber::BerConstants::e_PRIVATE,
                PRIMITIVE        = balber::BerConstants::e_PRIMITIVE,
                CONSTRUCTED      = balber::BerConstants::e_CONSTRUCTED
            };

            static const struct {
                int         d_line;    // line number
                int         d_class;   // tag class
                int         d_type;    // tag type
                int         d_number;  // tag number
                const char *d_exp;     // expected output onto the stream
            } DATA[] = {
                //line  class        type         number   exp
                //----  -----        ----         ------   ---
                { L_,   UNIVERSAL,   PRIMITIVE,   0,       "00"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   1,       "01"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   29,      "1D"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   30,      "1E"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   31,      "1F 1F"           },
                { L_,   UNIVERSAL,   PRIMITIVE,   127,     "1F 7F"           },
                { L_,   UNIVERSAL,   PRIMITIVE,   128,     "1F 81 00"        },
                { L_,   UNIVERSAL,   PRIMITIVE,   129,     "1F 81 01"        },
                { L_,   UNIVERSAL,   PRIMITIVE,   INT_MAX, "1F 87 FF FF FF 7F"
                                                                             },

                { L_,   UNIVERSAL,   CONSTRUCTED, 0,       "20"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 1,       "21"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 29,      "3D"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 30,      "3E"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 31,      "3F 1F"           },
                { L_,   UNIVERSAL,   CONSTRUCTED, 127,     "3F 7F"           },
                { L_,   UNIVERSAL,   CONSTRUCTED, 128,     "3F 81 00"        },
                { L_,   UNIVERSAL,   CONSTRUCTED, 129,     "3F 81 01"        },
                { L_,   UNIVERSAL,   CONSTRUCTED, INT_MAX, "3F 87 FF FF FF 7F"
                                                                             },

                { L_,   APPLICATION, PRIMITIVE,   0,       "40"              },
                { L_,   APPLICATION, PRIMITIVE,   1,       "41"              },
                { L_,   APPLICATION, PRIMITIVE,   29,      "5D"              },
                { L_,   APPLICATION, PRIMITIVE,   30,      "5E"              },
                { L_,   APPLICATION, PRIMITIVE,   31,      "5F 1F"           },
                { L_,   APPLICATION, PRIMITIVE,   127,     "5F 7F"           },
                { L_,   APPLICATION, PRIMITIVE,   128,     "5F 81 00"        },
                { L_,   APPLICATION, PRIMITIVE,   129,     "5F 81 01"        },
                { L_,   APPLICATION, PRIMITIVE,   INT_MAX, "5F 87 FF FF FF 7F"
                                                                             },

                { L_,   APPLICATION, CONSTRUCTED, 0,       "60"              },
                { L_,   APPLICATION, CONSTRUCTED, 1,       "61"              },
                { L_,   APPLICATION, CONSTRUCTED, 29,      "7D"              },
                { L_,   APPLICATION, CONSTRUCTED, 30,      "7E"              },
                { L_,   APPLICATION, CONSTRUCTED, 31,      "7F 1F"           },
                { L_,   APPLICATION, CONSTRUCTED, 127,     "7F 7F"           },
                { L_,   APPLICATION, CONSTRUCTED, 128,     "7F 81 00"        },
                { L_,   APPLICATION, CONSTRUCTED, 129,     "7F 81 01"        },
                { L_,   APPLICATION, CONSTRUCTED, INT_MAX, "7F 87 FF FF FF 7F"
                                                                             },

                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   0,       "80"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   1,       "81"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   29,      "9D"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   30,      "9E"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   31,      "9F 1F"           },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   INT_MAX, "9F 87 FF FF FF 7F"
                                                                             },

                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 0,       "A0"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 1,       "A1"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 29,      "BD"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 30,      "BE"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 31,      "BF 1F"           },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, INT_MAX, "BF 87 FF FF FF 7F"
                                                                             },

                { L_,   PRIVATE,     PRIMITIVE,   0,       "C0"              },
                { L_,   PRIVATE,     PRIMITIVE,   1,       "C1"              },
                { L_,   PRIVATE,     PRIMITIVE,   29,      "DD"              },
                { L_,   PRIVATE,     PRIMITIVE,   30,      "DE"              },
                { L_,   PRIVATE,     PRIMITIVE,   31,      "DF 1F"           },
                { L_,   PRIVATE,     PRIMITIVE,   INT_MAX, "DF 87 FF FF FF 7F"
                                                                             },

                { L_,   PRIVATE,     CONSTRUCTED, 0,       "E0"              },
                { L_,   PRIVATE,     CONSTRUCTED, 1,       "E1"              },
                { L_,   PRIVATE,     CONSTRUCTED, 29,      "FD"              },
                { L_,   PRIVATE,     CONSTRUCTED, 30,      "FE"              },
                { L_,   PRIVATE,     CONSTRUCTED, 31,      "FF 1F"           },
                { L_,   PRIVATE,     CONSTRUCTED, INT_MAX, "FF 87 FF FF FF 7F"
                                                                             },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const balber::BerConstants::TagClass
                            CLASS   =
                               (balber::BerConstants::TagClass)DATA[i].d_class;
                const balber::BerConstants::TagType
                            TYPE    =
                                 (balber::BerConstants::TagType)DATA[i].d_type;
                const int   NUMBER  = DATA[i].d_number;
                const char* EXP     = DATA[i].d_exp;
                const int   EXP_LEN = numOctets(EXP);

                if (veryVerbose) {
                    P_(LINE) P_(CLASS) P_(TYPE) P_(NUMBER) P(EXP)
                }

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putIdentifierOctets(
                                                                      &osb,
                                                                      CLASS,
                                                                      TYPE,
                                                                      NUMBER));
                LOOP_ASSERT(LINE, EXP_LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0       == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                balber::BerConstants::TagClass theClass;
                balber::BerConstants::TagType  theType;
                int                         theNumber        = -1;
                int                         numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf   isb(osb.data(), osb.length());

                LOOP_ASSERT(LINE, SUCCESS  == Util::getIdentifierOctets(
                                                          &isb, &theClass,
                                                          &theType, &theNumber,
                                                          &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP3_ASSERT(LINE, CLASS,  theClass,  CLASS  == theClass);
                LOOP3_ASSERT(LINE, TYPE,   theType,   TYPE   == theType);
                LOOP3_ASSERT(LINE, NUMBER, theNumber, NUMBER == theNumber);
                LOOP3_ASSERT(LINE, EXP_LEN,   numBytesConsumed,
                                   EXP_LEN == numBytesConsumed);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'putIndefiniteLengthOctet' & 'put/getEndOfContentOctets'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIndefiniteLengthOctet', "
                                  "'put/getEndOfContentOctets'"
                               << "\n===================================="
                                  "=================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'putIndefiniteLengthOctet'."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putIndefiniteLengthOctet(&osb));
            ASSERT(1       == osb.length());

            enum {
                k_INDEFINITE_LENGTH_OCTET = 0x80
            };

            ASSERT(k_INDEFINITE_LENGTH_OCTET == (unsigned char)osb.data()[0]);
        }

        if (verbose) bsl::cout << "\nTesting 'putEndOfContentOctets'."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putEndOfContentOctets(&osb));
            ASSERT(2       == osb.length());
            ASSERT(0       == osb.data()[0]);
            ASSERT(0       == osb.data()[1]);
        }

        if (verbose) bsl::cout << "\nTesting 'getEndOfContentOctets'."
                               << bsl::endl;
        {
            char buffer[] = { 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 0);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 1);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
            ASSERT(2       == numBytesConsumed);
        }
        {
            char buffer[] = { -1, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, -1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { -1, -1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 3);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
            ASSERT(2       == numBytesConsumed);
        }

        if (verbose) bsl::cout << "End of test." << bsl::endl;
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for date/time components
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                            << "\nTESTING 'putValue', 'getValue' for date/time"
                            << "\n============================================"
                            << bsl::endl;

        balber::BerEncoderOptions options;
        ASSERT(options.datetimeFractionalSecondPrecision() == 3);
        options.setDatetimeFractionalSecondPrecision(6);

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlt::Date Type;

            const Type  VALUE(2005, 02, 23);
            const char *EXP    = "0A 32 30 30 35 2D 30 32 2D 32 33";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlt::DateTz Type;

            const Type  VALUE(bdlt::Date(2005, 02, 23), 45);
            const char *EXP    = "10 32 30 30 35 2D 30 32 2D 32 33 2B 30 30 3A"
                                 " 34 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlt::Datetime Type;

            const Type  VALUE(2005, 02, 23, 3, 47, 21);
            const char *EXP    = "1A 32 30 30 35 2D 30 32 2D 32 33 54 30 33 3A"
                                 " 34 37 3A 32 31 2E 30 30 30 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlt::DatetimeTz Type;

            const Type  VALUE(bdlt::Datetime(2005, 02, 23, 3, 47, 21), 45);
            const char *EXP    = "20 32 30 30 35 2D 30 32 2D 32 33 54 30 33 "
                                 "3A 34 37 3A 32 31 2E 30 30 30 30 30 30 2B "
                                 "30 30 3A 34 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlt::Time Type;

            const Type  VALUE(21, 26, 43);
            const char *EXP    = "0F 32 31 3A 32 36 3A 34 33 2E "
                                 "30 30 30 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlt::TimeTz Type;

            const Type  VALUE(bdlt::Time(21, 26, 43), 45);
            const char *EXP    = "15 32 31 3A 32 36 3A 34 33 2E 30 30 30 "
                                 "30 30 30 2B 30 30 3A 34 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for 'bslstl::StringRef'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue', 'getValue' for string"
                               << "\n========================================="
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_string;   // string value
                const char *d_exp;      // expected value
            } DATA[] = {
                //line no.  string                exp
                //-------   ------                ---
                { L_,        "",                  "00" },
                { L_,        " ",                 "01 20" },
                { L_,        "-+",                "02 2D 2B" },
                { L_,        "Hello",             "05 48 65 6C 6C 6F" },
                { L_,        "12345",             "05 31 32 33 34 35" },
                { L_,        "&$#",               "03 26 24 23" },
                { L_,        "Have a nice day",   "0F 48 61 76 65 20 61 20 6E"
                                                  "69 63 65 20 64 61 79" },
                { L_,        "QWERTY",            "06 51 57 45 52 54 59" },
                { L_,        "This is an extremely long line that spans "
                             "a few lines",       "35 54 68 69 73 20 69 73 20"
                                                  "61 6E 20 65 78 74 72 65 6D"
                                                  "65 6C 79 20 6C 6F 6E 67 20"
                                                  "6C 69 6E 65 20 74 68 61 74"
                                                  "20 73 70 61 6E 73 20 61 20"
                                                "66 65 77 20 6C 69 6E 65 73" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int        LINE = DATA[di].d_lineNum;
                const StringRef  VAL  = DATA[di].d_string;
                const char      *EXP  = DATA[di].d_exp;
                const int        LEN  = numOctets(EXP);

                string  val;

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE,
                            SUCCESS ==
                                Util::getValue(&isb, &val, &numBytesConsumed));
                LOOP_ASSERT(LINE, 0 == isb.length());
                LOOP2_ASSERT(VAL, val, VAL == val);
                LOOP3_ASSERT(LINE,
                             LEN,
                             numBytesConsumed,
                             LEN == numBytesConsumed);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for bsl::string
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue', 'getValue' for string"
                               << "\n========================================="
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_string;   // string value
                const char *d_exp;      // expected value
            } DATA[] = {
                //line no.  string                exp
                //-------   ------                ---
                { L_,        "",                  "00" },
                { L_,        " ",                 "01 20" },
                { L_,        "-+",                "02 2D 2B" },
                { L_,        "Hello",             "05 48 65 6C 6C 6F" },
                { L_,        "12345",             "05 31 32 33 34 35" },
                { L_,        "&$#",               "03 26 24 23" },
                { L_,        "Have a nice day",   "0F 48 61 76 65 20 61 20 6E"
                                                  "69 63 65 20 64 61 79" },
                { L_,        "QWERTY",            "06 51 57 45 52 54 59" },
                { L_,        "This is an extremely long line that spans "
                             "a few lines",       "35 54 68 69 73 20 69 73 20"
                                                  "61 6E 20 65 78 74 72 65 6D"
                                                  "65 6C 79 20 6C 6F 6E 67 20"
                                                  "6C 69 6E 65 20 74 68 61 74"
                                                  "20 73 70 61 6E 73 20 61 20"
                                                "66 65 77 20 6C 69 6E 65 73" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int     LINE = DATA[di].d_lineNum;
                const string  VAL  = DATA[di].d_string;
                const char   *EXP  = DATA[di].d_exp;
                const int     LEN  = numOctets(EXP);
                      string  val;

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE,
                            SUCCESS ==
                                Util::getValue(&isb, &val, &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP2_ASSERT(VAL, val, VAL == val);
                LOOP3_ASSERT(LINE, LEN, numBytesConsumed,
                             LEN == numBytesConsumed);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'putLength' & 'getLength'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putLength', 'getLength'"
                               << "\n================================"
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            static const struct {
                int         d_line;   // line number
                int         d_length; // length value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line       length    exp
                //  ----       ------    ---
                { L_,              0,    "00"               },
                { L_,              1,    "01"               },
                { L_,             15,    "0F"               },
                { L_,             16,    "10"               },
                { L_,             32,    "20"               },
                { L_,             64,    "40"               },
                { L_,            127,    "7F"               },
                { L_,            128,    "81 80"            },
                { L_,            255,    "81 FF"            },
                { L_,            256,    "82 01 00"         },
                { L_,          32767,    "82 7F FF"         },
                { L_,          32768,    "82 80 00"         },
                { L_,          65535,    "82 FF FF"         },
                { L_,        8388608,    "83 80 00 00"      },
                { L_,       16777215,    "83 FF FF FF"      },
                { L_,     2147483646,    "84 7F FF FF FE"   },
                { L_,        INT_MAX,    "84 7F FF FF FF"   },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const int            LEN  = DATA[i].d_length;
                const char          *EXP  = DATA[i].d_exp;
                const int            EXP_LEN  = numOctets(EXP);
                      int            len;

                if (veryVerbose) { P_(i) P(LEN) }

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putLength(&osb, LEN));
                LOOP_ASSERT(LINE, EXP_LEN  == (int)osb.length());
                LOOP_ASSERT(LINE, 0        == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE, SUCCESS  == Util::getLength(
                                                           &isb, &len,
                                                           &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP3_ASSERT(LINE, LEN, len, len == LEN);
                LOOP3_ASSERT(LINE, EXP_LEN, numBytesConsumed,
                             EXP_LEN == numBytesConsumed);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'putDoubleValue' & 'getDoubleValue' for float and double
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putDoubleValue','getDoubleValue'"
                               << "\n========================================="
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_FloatingPointImpUtil DoubleUtil;
            typedef balber::BerUtil_LengthImpUtil        LengthUtil;

            static const struct {
                int         d_line;   // line number
                double      d_value;  // unsigned char value stored in an int
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line      value    exp
                //  ----      -----    ---
                { L_,           1.1,    "09 80 CD 08 CC CC CC CC CC CD"    },
                { L_,        99.234,    "09 80 D2 18 CE F9 DB 22 D0 E5"    },
                { L_,             0,    "00"                               },
                { L_,          1.25,    "03 80 FE 05"                      },
                { L_,           1.1,    "09 80 CD 08 CC CC CC CC CC CD"    },
                { L_,        0.0176,    "09 80 C6 12 05 BC 01 A3 6E 2F"    },
                { L_,    -7.8752345,    "09 C0 CE 1F 80 3D 79 07 52 DB"    },
                { L_,        99.234,
                                        "09 80 D2 18 CE F9 DB 22 D0 E5"    },
                { L_,      -100.987,
                                        "09 C0 D3 0C 9F 95 81 06 24 DD"    },
                { L_,    -77723.875,    "05 C0 FD 09 7C DF"                },
                { L_, 19998989.1234,    "09 80 E4 13 12 90 D1 F9 72 47"    },
                { L_,   79879879249686698E-100,
                                        "0A 81 FE B7 1F 09 39 59 03 93 B3" },
                { L_,   -9999999999999999E25,
                                        "09 C0 55 09 2E FD 1B 8D 0C F3"    },
                { L_,   1.8768E-307,    "0A 81 FB D1 10 DE 9A FC 0F BA 43" },
                { L_,   1.8768E-306,    "0A 81 FB D6 05 45 90 6E C4 EA 35" },
                { L_,   1.8768E-305,    "0A 81 FB D7 1A 5B D2 29 D8 93 09" },

                // Single precision denormalized numbers
                { L_,   1.4E-45,        "0A 81 FF 37 0F FC 34 5F A6 CA B5" },
                { L_,   -1.4E-45,       "0A C1 FF 37 0F FC 34 5F A6 CA B5" },

                // Double precision denormalized numbers

                // Commented test data that are out of range:
                //  DBL_MIN  - DBL_MAX
                // This is done because we can not guarantee portability across
                // platforms.

                //{ L_,   1E-314,         "07 81 FB CE 78 A4 22 05"          },
                //{ L_,   -1E-314,        "07 C1 FB CE 78 A4 22 05"          },

                { L_,  0.1e-307,        "0A 81 FB CF 03 98 6B 3C 0C F4 69" },
                //{ L_,  1.32423e-308,    "0A 81 FB CE 09 85 B1 4E 40 CE 07" },

                //{ L_,  1.8768e-310,     "09 81 FB D0 08 A3 20 3F 84 FB" },
                { L_,  783462315e-312,  "0A 81 FB DD 11 31 53 DC 2E 24 55" },
                //{ L_,  9.2332323e-316,  "07 81 FB CF 05 91 CD 47" },

                { L_,  -0.1e-307,       "0A C1 FB CF 03 98 6B 3C 0C F4 69" },
                //{ L_, -1.32423e-308,    "0A C1 FB CE 09 85 B1 4E 40 CE 07" },

                //{ L_, -1.8768e-310,     "09 C1 FB D0 08 A3 20 3F 84 FB" },
                { L_, -783462315e-312,  "0A C1 FB DD 11 31 53 DC 2E 24 55" },
                //{ L_, -9.2332323e-316,  "07 C1 FB CF 05 91 CD 47" },

                //{ L_,  1.11254E-308,    "0A 81 FB CF 04 00 00 B9 3A AB 81" },
                //{ L_, -9.7347E-309,     "0A C1 FB CE 07 00 00 16 8C 57 15" },
                //{ L_,  5.56268E-309,    "0A 81 FB CE 03 FF FF C7 F2 67 77" },

                // FLT_MAX & FLT_MIN
                { L_,   1.175494351E-38,
                                        "0A 81 FF 4E 10 00 00 00 0A 63 9B" },
                { L_,   3.402823466E+38,
                                        "09 80 4C 0F FF FF EF F8 38 1B"    },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int     LINE = DATA[i].d_line;
                const double  VAL  = DATA[i].d_value;
                const float   F    = (float) DATA[i].d_value;
                const char   *EXP  = DATA[i].d_exp;
                const int     LEN  = numOctets(EXP);

                if (veryVerbose) { P_(i) P(VAL) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS ==
                                    DoubleUtil::putDoubleValue(&osb, VAL));
                    LOOP_ASSERT(LINE, LEN      == (int)osb.length());
                    LOOP_ASSERT(LINE, 0        ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }

                    int    numBytesConsumed = 0;
                    int    length           = 0;
                    double val              = 0.0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS ==
                                    LengthUtil::getLength(
                                        &length, &numBytesConsumed, &isb));
                    LOOP_ASSERT(LINE,
                                SUCCESS == DoubleUtil::getDoubleValue(
                                               &val, &isb, length));
                    numBytesConsumed += length;
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP3_ASSERT(LINE, VAL, val, val == VAL);
                    LOOP3_ASSERT(LINE, LEN, numBytesConsumed,
                                 LEN == numBytesConsumed);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS  == Util::putValue(&osb, F));

                    if (veryVerbose) {
                        P(VAL)
                        cout << "output buffer:";
                        printBuffer(osb.data(), osb.length());
                    }

                    int   numBytesConsumed = 0;
                    float f                = 0.0f;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &f, &numBytesConsumed));
                    LOOP3_ASSERT(LINE, F, f, F == f);
                    LOOP_ASSERT(LINE, (int)osb.length() == numBytesConsumed);
                }
            }

            if (verbose) { cout << "\nTesting for special values" << endl; }
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    int         d_sign;     // sign value (0 - +ve, 1 - -ve)
                    int         d_exponent; // biased (IEEE) exponent value
                    long long   d_mantissa; // mantissa value
                    const char *d_exp;      // encoded result
                    int         d_result;   // SUCCESS (0) / FAILURE (-1)
                } REAL[] = {
                    //line  sign  exp      man          exp          res
                    //----  ----  ---      ---          ------       ---
                    // +ve & -ve infinity
                    { L_,      0, 2047,     0,         "01 40",   SUCCESS },
                    { L_,      1, 2047,     0,         "01 41",   SUCCESS },

                    // +ve & -ve NaN
                    { L_,      0, 2047,     1,         "01 42",   SUCCESS },
                    { L_,      1, 2047,     1,         "01 42",   SUCCESS },

                    // +ve & -ve 0
                    { L_,      0,    0,     0,         "00",      SUCCESS },
                    { L_,      1,    0,     0,         "00",      SUCCESS },

                    // Denormalized numbers
                    { L_,      0,    0,     1,         "04 81 FB CE 01",
                                                                  SUCCESS },
                    { L_,      1,    0,     1,         "04 C1 FB CE 01",
                                                                  SUCCESS },
                    { L_,      0,    0,  1987,         "05 81 FB CE 07 C3",
                                                                  SUCCESS },
                    { L_,      1,    0,  9832,         "05 C1 FB D1 04 CD",
                                                                  SUCCESS },
                };

                bdlsb::MemOutStreamBuf osb;

                const int NUM_DATA = sizeof REAL / sizeof *REAL;

                for (int di = 0; di < NUM_DATA; ++di) {
                    const int        LINE     = REAL[di].d_lineNum;
                    const int        SIGN     = REAL[di].d_sign;
                    const int        EXPONENT = REAL[di].d_exponent;
                    const long long  MANTISSA = REAL[di].d_mantissa;
                    const char      *EXP      = REAL[di].d_exp;
                    const int        LEN      = numOctets(EXP);
                    const int        RESULT   = REAL[di].d_result;
                          double     outVal, inVal;

                    bdlsb::MemOutStreamBuf osb;

                    assembleDouble(&outVal, SIGN, EXPONENT, MANTISSA);

                    LOOP_ASSERT(LINE,
                                RESULT ==
                                    DoubleUtil::putDoubleValue(&osb, outVal));
                    if (SUCCESS == RESULT) {
                        LOOP_ASSERT(LINE, LEN == (int)osb.length());
                        LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(),
                                                                EXP));
                    }

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    if (SUCCESS == RESULT) {
                        int numBytesConsumed = 0;
                        int length           = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        LOOP_ASSERT(LINE,
                                    SUCCESS ==
                                        LengthUtil::getLength(
                                            &length, &numBytesConsumed, &isb));
                        LOOP_ASSERT(LINE,
                                    SUCCESS == DoubleUtil::getDoubleValue(
                                                   &inVal, &isb, length));
                        numBytesConsumed += length;
                        LOOP_ASSERT(LINE, 0     == isb.length());

                        // to avoid NaN comparisons
                        if (inVal == inVal)
                           LOOP_ASSERT(LINE, inVal == outVal);

                        LOOP_ASSERT(LINE, LEN   == numBytesConsumed);
                    }
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned integral values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int                 d_line;   // line number
                unsigned long long  d_value;  // value
                const char         *d_exp;    // expected output
            } DATA[] = {
                //  line            value      exp
                //  ----            -----      ---
               { L_,                    0, "01 00"                          },
               { L_,                    1, "01 01"                          },
               { L_,                    5, "01 05"                          },
               { L_,                   65, "01 41"                          },
               { L_,                   66, "01 42"                          },
               { L_,                   67, "01 43"                          },
               { L_,                  126, "01 7E"                          },
               { L_,                  127, "01 7F"                          },
               { L_,                  128, "02 00 80"                       },
               { L_,                  129, "02 00 81"                       },
               { L_,                  254, "02 00 FE"                       },
               { L_,                  255, "02 00 FF"                       },
               { L_,                  256, "02 01 00"                       },
               { L_,                  257, "02 01 01"                       },
               { L_,                32766, "02 7F FE"                       },
               { L_,                32767, "02 7F FF"                       },
               { L_,                32768, "03 00 80 00"                    },
               { L_,                32769, "03 00 80 01"                    },
               { L_,              8388606, "03 7F FF FE"                    },
               { L_,              8388607, "03 7F FF FF"                    },
               { L_,              8388608, "04 00 80 00 00"                 },
               { L_,              8388609, "04 00 80 00 01"                 },
               { L_,           2147483645, "04 7F FF FF FD"                 },
               { L_,           2147483646, "04 7F FF FF FE"                 },
               { L_,              INT_MAX, "04 7F FF FF FF"                 },
               { L_,(unsigned)INT_MAX + 1, "05 00 80 00 00 00"              },
               { L_,         UINT_MAX - 1, "05 00 FF FF FF FE"              },
               { L_,             UINT_MAX, "05 00 FF FF FF FF"              },
               { L_,      0x7FFFFFFFFEULL, "05 7F FF FF FF FE"              },
               { L_,      0x7FFFFFFFFFULL, "05 7F FF FF FF FF"              },
               { L_,      0x8000000000ULL, "06 00 80 00 00 00 00"           },
               { L_,      0x8000000001ULL, "06 00 80 00 00 00 01"           },
               { L_,    0x7FFFFFFFFFFEULL, "06 7F FF FF FF FF FE"           },
               { L_,    0x7FFFFFFFFFFFULL, "06 7F FF FF FF FF FF"           },
               { L_,    0x800000000000ULL, "07 00 80 00 00 00 00 00"        },
               { L_,    0x800000000001ULL, "07 00 80 00 00 00 00 01"        },
               { L_,  0x7FFFFFFFFFFFFEULL, "07 7F FF FF FF FF FF FE"        },
               { L_,  0x7FFFFFFFFFFFFFULL, "07 7F FF FF FF FF FF FF"        },
               { L_,  0x80000000000000ULL, "08 00 80 00 00 00 00 00 00"     },
               { L_,  0x80000000000001ULL, "08 00 80 00 00 00 00 00 01"     },
               { L_,0x7FFFFFFFFFFFFFFEULL, "08 7F FF FF FF FF FF FF FE"     },
               { L_,0x7FFFFFFFFFFFFFFFULL, "08 7F FF FF FF FF FF FF FF"     },
               { L_,0x8000000000000000ULL, "09 00 80 00 00 00 00 00 00 00"  },
               { L_,0x8000000000000001ULL, "09 00 80 00 00 00 00 00 00 01"  },
               { L_,0xFFFFFFFFFFFFFFFEULL, "09 00 FF FF FF FF FF FF FF FE"  },
               { L_,0xFFFFFFFFFFFFFFFFULL, "09 00 FF FF FF FF FF FF FF FF"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for unsigned long long"
                                    << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int                 LINE = DATA[i].d_line;
                    const unsigned long long  LL   = DATA[i].d_value;
                    const char               *EXP  = DATA[i].d_exp;
                    const int                 LEN  = numOctets(EXP);
                          unsigned long long  ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &ll, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, ll  == LL);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UINT_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned int   VI   = (unsigned int) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned int   vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &vi, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, vi  == VI);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > ULONG_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned long  L    =
                                               (unsigned long) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned long  l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &l, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, l   == L);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL = DATA[i].d_value;
                    if (LL > USHRT_MAX) {
                        continue;
                    }

                    const int             LINE = DATA[i].d_line;
                    const unsigned short  S    =
                                              (unsigned short) DATA[i].d_value;
                    const char           *EXP  = DATA[i].d_exp;
                    const int             LEN  = numOctets(EXP);
                          unsigned short  s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &s, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, s   == S);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UCHAR_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned char  C    =
                                               (unsigned char) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned char  c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &c, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, c   == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for signed integral values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int         d_line;   // line number
                long long   d_value;  // value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line                value  exp
                //  ----                -----  ---
                {   L_,                    0,  "01 00"                      },
                {   L_,                    1,  "01 01"                      },
                {   L_,                    5,  "01 05"                      },
                {   L_,                   -1,  "01 FF"                      },
                {   L_,                   -2,  "01 FE"                      },
                {   L_,                   -5,  "01 FB"                      },
                {   L_,                  126,  "01 7E"                      },
                {   L_,                  127,  "01 7F"                      },
                {   L_,                  128,  "02 00 80"                   },
                {   L_,                  129,  "02 00 81"                   },
                {   L_,                 -127,  "01 81"                      },
                {   L_,                 -128,  "01 80"                      },
                {   L_,                 -129,  "02 FF 7F"                   },
                {   L_,                32766,  "02 7F FE"                   },
                {   L_,                32767,  "02 7F FF"                   },
                {   L_,                32768,  "03 00 80 00"                },
                {   L_,                32769,  "03 00 80 01"                },
                {   L_,               -32767,  "02 80 01"                   },
                {   L_,               -32768,  "02 80 00"                   },
                {   L_,               -32769,  "03 FF 7F FF"                },
                {   L_,              8388606,  "03 7F FF FE"                },
                {   L_,              8388607,  "03 7F FF FF"                },
                {   L_,              8388608,  "04 00 80 00 00"             },
                {   L_,              8388609,  "04 00 80 00 01"             },
                {   L_,             -8388607,  "03 80 00 01"                },
                {   L_,             -8388608,  "03 80 00 00"                },
                {   L_,             -8388609,  "04 FF 7F FF FF"             },
                {   L_,             -8388610,  "04 FF 7F FF FE"             },
                {   L_,           2147483646,  "04 7F FF FF FE"             },
                {   L_,              INT_MAX,  "04 7F FF FF FF"             },
                {   L_,          -2147483647,  "04 80 00 00 01"             },
                {   L_,              INT_MIN,  "04 80 00 00 00"             },
                {   L_,       0x7F12345678LL,  "05 7F 12 34 56 78"          },
                {   L_,       0x7FFFFFFFFFLL,  "05 7F FF FF FF FF"          },
                {   L_,       0x8000000000LL,  "06 00 80 00 00 00 00"       },
                {   L_,       0x8000000001LL,  "06 00 80 00 00 00 01"       },
                {   L_, static_cast<long long>(0xFFFFFF8000000000LL),
                                              "05 80 00 00 00 00"          },
                {   L_, static_cast<long long>(0xFFFFFF8000000001LL),
                                              "05 80 00 00 00 01"          },
                {   L_, static_cast<long long>(0xFFFFFF8123456789LL),
                                              "05 81 23 45 67 89"          },
                {   L_, static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),
                                              "06 FF 7F FF FF FF FF"       },
                {   L_,     0x7FFFFFFFFFFFLL,  "06 7F FF FF FF FF FF"       },
                {   L_,     0x800000000000LL,  "07 00 80 00 00 00 00 00"    },
                {   L_, static_cast<long long>(0xFFFF800000000000LL),
                                              "06 80 00 00 00 00 00"       },
                {   L_, static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),
                                              "07 FF 7F FF FF FF FF FF"    },
                {   L_,   0x7FFFFFFFFFFFFFLL,  "07 7F FF FF FF FF FF FF"    },
                {   L_,   0x80000000000000LL,  "08 00 80 00 00 00 00 00 00" },
                {   L_, static_cast<long long>(0xFF80000000000000LL),
                                              "07 80 00 00 00 00 00 00"    },
                {   L_, static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),
                                              "08 FF 7F FF FF FF FF FF FF" },
                {   L_, 0x7FFFFFFFFFFFFFFFLL,  "08 7F FF FF FF FF FF FF FF" },
                {   L_, static_cast<long long>(0x8000000000000001LL),
                                              "08 80 00 00 00 00 00 00 01" },
                {   L_, static_cast<long long>(0x8000000000000000LL),
                                              "08 80 00 00 00 00 00 00 00" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for long long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int            LINE = DATA[i].d_line;
                    const long long      LL   = DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          long long      ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &ll, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0       == isb.length());
                    LOOP_ASSERT(LINE, ll  == LL);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > INT_MAX || LL < INT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const int      VI   = (int) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          int      vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &vi, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi  == VI);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > LONG_MAX || LL < LONG_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const long     L    = (long) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          long     l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &l, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, l   == L);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SHRT_MAX || LL < SHRT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const short    S    = (short) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          short    s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &s, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, s   == S);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SCHAR_MAX || LL < SCHAR_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const char     C    = (char) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          char     c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &c, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, c   == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING put and get given length functions for unsigned types
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIntegerGivenLength' "
                               << "and 'getIntegerGivenLength' for unsigned"
                               << "\n================================="
                               << "========================================"
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_IntegerImpUtil IntegerUtil;

            static const struct {
                int                 d_line;   // line number
                unsigned long long  d_value;  // int value stored in an int
                int                 d_length; // length
                const char         *d_exp;    // expected output
            } DATA[] = {
                //  line            value len exp
                //  ----            ----- --- ---
               { L_,                    0, 1, "00"                          },
               { L_,                    1, 1, "01"                          },
               { L_,                  127, 1, "7F"                          },
               { L_,                32767, 2, "7F FF"                       },
               { L_,              8388607, 3, "7F FF FF"                    },
               { L_,           2147483646, 4, "7F FF FF FE"                 },
               { L_,              INT_MAX, 4, "7F FF FF FF"                 },
               { L_,      0x7FFFFFFFFFULL, 5, "7F FF FF FF FF"              },
               { L_,      0x8000000000ULL, 6, "00 80 00 00 00 00"           },
               { L_,    0x7FFFFFFFFFFFULL, 6, "7F FF FF FF FF FF"           },
               { L_,    0x800000000000ULL, 7, "00 80 00 00 00 00 00"        },
               { L_,  0x7FFFFFFFFFFFFFULL, 7, "7F FF FF FF FF FF FF"        },
               { L_,  0x80000000000000ULL, 8, "00 80 00 00 00 00 00 00"     },
               { L_,0x7FFFFFFFFFFFFFFFULL, 8, "7F FF FF FF FF FF FF FF"     },
               { L_,0x8000000000000001ULL, 8, "80 00 00 00 00 00 00 01"     },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for unsigned long long"
                                    << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int                 LINE = DATA[i].d_line;
                    const unsigned long long  LL   = DATA[i].d_value;
                    const int                 LEN  = DATA[i].d_length;
                    const char               *EXP  = DATA[i].d_exp;
                          unsigned long long  ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &ll, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, ll == LL);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UINT_MAX) {
                        continue;
                    }

                    const int           LINE = DATA[i].d_line;
                    const unsigned int  VI   = (unsigned int) DATA[i].d_value;
                    const int           LEN  = DATA[i].d_length;
                    const char         *EXP  = DATA[i].d_exp;
                          unsigned int  vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   IntegerUtil::getIntegerValue(&vi, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi == VI);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > ULONG_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned long  L    = (unsigned long)DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          unsigned long  l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &l, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, l == L);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > USHRT_MAX) {
                        continue;
                    }

                    const int             LINE = DATA[i].d_line;
                    const unsigned short  S    =
                                              (unsigned short) DATA[i].d_value;
                    const int             LEN  = DATA[i].d_length;
                    const char           *EXP  = DATA[i].d_exp;
                          unsigned short  s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &s, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, s == S);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UCHAR_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned char  C    = (unsigned char)DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          unsigned char  c    = 0;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &c, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'putIntegerGivenLength' & 'getIntegerValue'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIntegerGivenLength' "
                               << "and 'getIntegerValue'"
                               << "\n================================="
                               << "==========================="
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_IntegerImpUtil IntegerUtil;

            static const struct {
                int         d_line;   // line number
                long long   d_value;  // int value stored in an int
                int         d_length; // length
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line               value len  exp
                //  ----               ----- ---  ---
                {  L_,                    0,  1,  "00"                      },
                {  L_,                    1,  1,  "01"                      },
                {  L_,                   -2,  1,  "FE"                      },
                {  L_,                  127,  1,  "7F"                      },
                {  L_,                  128,  2,  "00 80"                   },
                {  L_,                 -128,  1,  "80"                      },
                {  L_,                 -129,  2,  "FF 7F"                   },
                {  L_,                32767,  2,  "7F FF"                   },
                {  L_,                32768,  3,  "00 80 00"                },
                {  L_,               -32768,  2,  "80 00"                   },
                {  L_,               -32769,  3,  "FF 7F FF"                },
                {  L_,              8388607,  3,  "7F FF FF"                },
                {  L_,              8388608,  4,  "00 80 00 00"             },
                {  L_,             -8388608,  3,  "80 00 00"                },
                {  L_,             -8388609,  4,  "FF 7F FF FF"             },
                {  L_,           2147483646,  4,  "7F FF FF FE"             },
                {  L_,              INT_MAX,  4,  "7F FF FF FF"             },
                {  L_,          -2147483647,  4,  "80 00 00 01"             },
                {  L_,              INT_MIN,  4,  "80 00 00 00"             },
                {  L_,       0x7FFFFFFFFFLL,  5,  "7F FF FF FF FF"          },
                {  L_,       0x8000000000LL,  6,  "00 80 00 00 00 00"       },
                {  L_, static_cast<long long>(0xFFFFFF8000000000LL),
                                              5,  "80 00 00 00 00"          },
                {  L_, static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),
                                              6,  "FF 7F FF FF FF FF"       },
                {  L_,     0x7FFFFFFFFFFFLL,  6,  "7F FF FF FF FF FF"       },
                {  L_,     0x800000000000LL,  7,  "00 80 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFFFF800000000000LL),
                                              6,  "80 00 00 00 00 00"       },
                {  L_, static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),
                                              7,  "FF 7F FF FF FF FF FF"    },
                {  L_,   0x7FFFFFFFFFFFFFLL,  7,  "7F FF FF FF FF FF FF"    },
                {  L_,   0x80000000000000LL,  8,  "00 80 00 00 00 00 00 00" },
                {  L_, static_cast<long long>(0xFF80000000000000LL),
                                              7,  "80 00 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),
                                              8,  "FF 7F FF FF FF FF FF FF" },
                {  L_, 0x7FFFFFFFFFFFFFFFLL,  8,  "7F FF FF FF FF FF FF FF" },
                {  L_, static_cast<long long>(0x8000000000000001LL),
                                              8,  "80 00 00 00 00 00 00 01" },
                {  L_, static_cast<long long>(0x8000000000000000LL),
                                              8,  "80 00 00 00 00 00 00 00" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for long long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int            LINE = DATA[i].d_line;
                    const long long      LL   = DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          long long      ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &ll, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, ll == LL);
                }
            }

            if (veryVerbose) { cout << "Testing for int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > INT_MAX || LL < INT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const int      VI   = (int) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          int      vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &vi, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi == VI);
                }
            }

            if (veryVerbose) { cout << "Testing for long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > LONG_MAX || LL < LONG_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const long     L    = (long) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          long     l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &l, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, l == L);
                }
            }

            if (veryVerbose) { cout << "Testing for short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SHRT_MAX || LL < SHRT_MIN) {
                        continue;
                    }

                    const int    LINE = DATA[i].d_line;
                    const short  S    = (short) DATA[i].d_value;
                    const int    LEN  = DATA[i].d_length;
                    const char  *EXP  = DATA[i].d_exp;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    short s;

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &s, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, s == S);
                }
            }

            if (veryVerbose) { cout << "Testing for char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SCHAR_MAX || LL < SCHAR_MIN) {
                        continue;
                    }

                    const int   LINE = DATA[i].d_line;
                    const char  C    = (char) DATA[i].d_value;
                    const int   LEN  = DATA[i].d_length;
                    const char *EXP  = DATA[i].d_exp;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    char     c = 0;

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::putIntegerGivenLength(
                                               &osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == IntegerUtil::getIntegerValue(
                                               &c, &isb, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'numBytesToStream' for unsigned types
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int numBytesToStream(TYPE value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'numBytesToStream' for unsigned"
                               << "\n======================================="
                               << bsl::endl;

        {
            static const struct {
                int                d_line;   // line number
                unsigned long long d_value;  // integer value
                int                d_result; // expected result
            } DATA[] = {
                //  line                       value       res
                //  ----                       -----       ---
                {  L_,                            0,         1 },
                {  L_,                            1,         1 },
                {  L_,                          127,         1 },
                {  L_,                          128,         2 },
                {  L_,                        32767,         2 },
                {  L_,                        32768,         3 },
                {  L_,                      8388607,         3 },
                {  L_,                      8388608,         4 },
                {  L_,                   2147483646,         4 },
                {  L_,                      INT_MAX,         4 },
                {  L_,               0x7FFFFFFFFFLL,         5 },
                {  L_,               0x8000000000LL,         6 },
                {  L_,             0x7FFFFFFFFFFFLL,         6 },
                {  L_,             0x800000000000LL,         7 },
                {  L_,           0x7FFFFFFFFFFFFFLL,         7 },
                {  L_,           0x80000000000000LL,         8 },
                {  L_,         0x7FFFFFFFFFFFFFFFLL,         8 },
                {  L_,         0x8000000000000000LL,         9 },
                {  L_,         0xFFFFFFFFFFFFFFFFLL,         9 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE = DATA[i].d_line;
                const unsigned long long VAL  = DATA[i].d_value;
                const int                RES  = DATA[i].d_result;

                if (veryVerbose) { P_(i) P(VAL) }

                typedef balber::BerUtil_IntegerImpUtil IntegerUtil;

                LOOP3_ASSERT(
                            LINE, VAL, RES,
                            RES == IntegerUtil::getNumOctetsToStream(VAL));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'numBytesToStream'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int numBytesToStream(TYPE value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'numBytesToStream'"
                               << "\n=========================="
                               << bsl::endl;

        {
            static const struct {
                int       d_line;   // line number
                long long d_value;  // integer value
                int       d_result; // expected result
            } DATA[] = {
                //  line                                       value       res
                //  ----                                       -----       ---
                {  L_,                                             0,      1 },
                {  L_,                                             1,      1 },
                {  L_,                                            -1,      1 },
                {  L_,                                           127,      1 },
                {  L_,                                           128,      2 },
                {  L_,                                          -128,      1 },
                {  L_,                                          -129,      2 },
                {  L_,                                         32767,      2 },
                {  L_,                                         32768,      3 },
                {  L_,                                        -32768,      2 },
                {  L_,                                        -32769,      3 },
                {  L_,                                       8388607,      3 },
                {  L_,                                       8388608,      4 },
                {  L_,                                      -8388608,      3 },
                {  L_,                                      -8388609,      4 },
                {  L_,                                    2147483646,      4 },
                {  L_,                                       INT_MAX,      4 },
                {  L_,                                   -2147483647,      4 },
                {  L_,                                       INT_MIN,      4 },

                {  L_,                               0x7FFFFFFFFFLL ,      5 },
                {  L_,                               0x8000000000LL ,      6 },
                {  L_,  static_cast<long long>(0xFFFFFF8000000000LL),      5 },
                {  L_,  static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),      6 },
                {  L_,                             0x7FFFFFFFFFFFLL ,      6 },
                {  L_,                             0x800000000000LL ,      7 },
                {  L_,  static_cast<long long>(0xFFFF800000000000LL),      6 },
                {  L_,  static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),      7 },
                {  L_,                           0x7FFFFFFFFFFFFFLL ,      7 },
                {  L_,                           0x80000000000000LL ,      8 },
                {  L_,  static_cast<long long>(0xFF80000000000000LL),      7 },
                {  L_,  static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),      8 },
                {  L_,                         0x7FFFFFFFFFFFFFFFLL ,      8 },
                {  L_,  static_cast<long long>(0x8000000000000001LL),      8 },
                {  L_,  static_cast<long long>(0x8000000000000000LL),      8 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE = DATA[i].d_line;
                const long long LL   = DATA[i].d_value;
                const int       RES  = DATA[i].d_result;

                if (veryVerbose) { P_(i) P(LL) }

                typedef balber::BerUtil_IntegerImpUtil IntegerUtil;

                LOOP4_ASSERT(
                      LINE, LL, RES, IntegerUtil::getNumOctetsToStream(LL),
                      RES == IntegerUtil::getNumOctetsToStream(LL));

                if (LL <= SHRT_MAX && LL >= SHRT_MIN) {
                    const short S = (short) LL;
                    const int   I = (int)   LL;
                    const long  L = (long)  LL;

                    LOOP4_ASSERT(
                        LINE, S, RES, IntegerUtil::getNumOctetsToStream(S),
                        RES == IntegerUtil::getNumOctetsToStream(S));
                    LOOP4_ASSERT(
                        LINE, I, RES, IntegerUtil::getNumOctetsToStream(I),
                        RES == IntegerUtil::getNumOctetsToStream(I));
                    LOOP4_ASSERT(
                        LINE, L, RES, IntegerUtil::getNumOctetsToStream(L),
                        RES == IntegerUtil::getNumOctetsToStream(L));
                    continue;
                }

                if (LL <= INT_MAX && LL >= INT_MIN) {
                    const int  I = (int)  LL;
                    const long L = (long) LL;

                    LOOP3_ASSERT(
                              LINE, I, RES,
                              RES == IntegerUtil::getNumOctetsToStream(I));
                    LOOP3_ASSERT(
                              LINE, L, RES,
                              RES == IntegerUtil::getNumOctetsToStream(L));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned char values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int            d_line;   // line number
                unsigned char  d_value;  // unsigned char value
                const char    *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line  value       exp
                //  ----  -----       ---
                {  L_,       0,      "01 00"    },
                {  L_,       1,      "01 01"    },
                {  L_,      95,      "01 5F"    },
                {  L_,     127,      "01 7F"    },
                {  L_,     128,      "02 00 80" },
                {  L_,     200,      "02 00 C8" },
                {  L_,     255,      "02 00 FF" },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const unsigned char  UC   = DATA[i].d_value;
                const char          *EXP  = DATA[i].d_exp;
                const int            LEN  = numOctets(EXP);

                unsigned char  uc;

                {
                    bdlsb::MemOutStreamBuf osb;

                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, UC));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &uc, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, uc  == UC);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for signed char values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int         d_line;   // line number
                char        d_value;  // char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                //  line  value       exp
                //  ----  -----       ---
                {  L_,       0,      "01 00" },
                {  L_,       1,      "01 01" },
                {  L_,      -2,      "01 FE" },
                {  L_,      95,      "01 5F" },
                {  L_,    -106,      "01 96" },
                {  L_,     127,      "01 7F" },
                {  L_,    -127,      "01 81" },
                {  L_,    -128,      "01 80" },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE = DATA[i].d_line;
                const char         C    = DATA[i].d_value;
                const signed char  SC   = (signed char) DATA[i].d_value;
                const char        *EXP  = DATA[i].d_exp;
                const int          LEN  = numOctets(EXP);

                char          c;
                signed char  sc = 0;

                if (veryVerbose) { cout << "\nTesting char values" << endl; }
                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &c, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }

                if (veryVerbose) { cout << "\nTesting signed char values"
                                        << endl; }
                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, SC));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Util::getValue(
                                               &isb, &sc, &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, sc  == SC);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for bool values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            const bool XA1 = true;  bool XA2; const char *EA = "01 01";
            const bool XB1 = false; bool XB2; const char *EB = "01 00";

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XA1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EA));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &XA2, &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(XA1     == XA2);
                ASSERT(2       == numBytesConsumed);
            }

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XB1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EB));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS ==
                       Util::getValue(&isb, &XB2, &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(XB1     == XB2);
                ASSERT(2       == numBytesConsumed);
            }
        }
      } break;
      case 1: {
        // BREATHING TEST
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns: There are several concerns about performance.  For one
        //   thing, the use of 'bsl::streambuf' can slow things down (if the
        //   length of the buffer is known in advance, a direct access through
        //   a 'bdlsb::FixedMemInput' or 'bdlsb::FixedMemOutput' can bypass
        //   virtual function calls.  Then there is the encoding/decoding
        //   proper.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        bsl::cout << "\nPERFORMANCE TESTING"
                  << "\n==================="
                  << bsl::endl;

        enum { MAX_ITER = 10000000 };
        int maxIter = MAX_ITER;
        if (argc > 2) {
            maxIter = bsl::atoi(argv[2]);
        }

        bsls::Stopwatch timer;

        char buffer[64];
        bdlsb::FixedMemOutStreamBuf osb(buffer, 64);
        bdlsb::FixedMemInStreamBuf isb(buffer, 64);
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            const bool B = i & 0x1;
            Util::putValue(&osb, B);

            int   numBytesConsumed = 0;
            bool  b                = true;
            Util::getValue(&isb, &b, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(bool)/get(&bool): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            const char C = static_cast<char>('0' + i % 64);
            Util::putValue(&osb, C);

            int   numBytesConsumed = 0;
            char  c                = ' ';
            Util::getValue(&isb, &c, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(char)/get(&char): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        int I = 1;
        for (int i = 0; i < maxIter / 2; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time

            Util::putValue(&osb, I);
            Util::putValue(&osb, -I);

            int   numBytesConsumed = 0;
            int   x                = 0;
            Util::getValue(&isb, &x, &numBytesConsumed);
            Util::getValue(&isb, &x, &numBytesConsumed);

            if (I > INT_MAX / 7) {
                I = 1;
            }
            else {
                I *= 7;
            }
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(int)/get(&int): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        long long LL = 1;
        for (int i = 0; i < maxIter / 2; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time

            Util::putValue(&osb, LL);
            Util::putValue(&osb, -LL);

            int   numBytesConsumed = 0;
            long long   x                = 0;
            Util::getValue(&isb, &x, &numBytesConsumed);
            Util::getValue(&isb, &x, &numBytesConsumed);

            if (LL > LLONG_MAX / 7) {
                LL = 1;
            }
            else {
                LL *= 7;
            }
        }
        timer.stop();
        bsl::cout << maxIter
                  << " iterations for put(long long)/get(&long long): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        const double F = 3.1415927;
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            Util::putValue(&osb, F);

            int   numBytesConsumed = 0;
            float f                = 0.0f;
            Util::getValue(&isb, &f, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(double)/get(&double): "
                  << timer.elapsedTime() << bsl::endl;

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
