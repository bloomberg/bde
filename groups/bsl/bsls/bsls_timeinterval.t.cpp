// bsls_timeinterval.t.cpp                                            -*-C++-*-

#include <bsls_timeinterval.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_nativestd.h>
#include <bsls_platform.h>

#include <algorithm>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>

#include <limits.h>  // LLONG_MAX
#include <math.h>    // fabs
#include <stdio.h>   // printf
#include <stdlib.h>
#include <string.h>  // memset, memcpy

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test implements a value-semantic type.  The canonical
// representation for the time interval is in seconds and nanoseconds, where
// nanoseconds must be in the range '[ -999999999..999999999 ]'.  This test
// driver is boot-strapped using the manipulators and accessors for the
// canonical representation, and then extended for the complete set of methods.
//
// We follow our standard ten-case approach to testing value-semantic types,
// except that we will verify test apparatus in case 3 (in lieu of the
// generator function, 'gg'), with the default constructor, (trivial)
// destructor, and primary manipulator ('setTotalMilliseconds') tested fully in
// case 2.
//
// Primary Manipulators:
//: o void setIntervalRaw(bsls::Types::Int64 secs, int nanoseconds);
//
//
// Basic Accessors:
//: o int nanoseconds() const;
//: o bsls::Types::Int64 seconds() const;
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [22] bool isValid(Int64, int);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] TimeInterval();
// [12] TimeInterval(Int64, int);
// [13] TimeInterval(double);
// [ 7] TimeInterval(const TimeInterval&);
// [ 2] ~TimeInterval();
//
// MANIPULATORS
// [ 9] TimeInterval& operator=(const TimeInterval&);
// [13] TimeInterval& operator=(double);
// [19] TimeInterval& operator+=(const TimeInterval&);
// [19] TimeInterval& operator+=(double);
// [19] TimeInterval& operator-=(const TimeInterval&);
// [19] TimeInterval& operator-=(double);
// [18] TimeInterval& addDays(bsls::Types::Int64);
// [18] TimeInterval& addHours(bsls::Types::Int64);
// [18] TimeInterval& addMinutes(bsls::Types::Int64);
// [18] TimeInterval& addSeconds(bsls::Types::Int64);
// [18] TimeInterval& addMilliseconds(Int64);
// [18] TimeInterval& addMicroseconds(Int64);
// [18] TimeInterval& addNanoseconds(Int64);
// [16] void setTotalDays(int);
// [16] void setTotalHours(Int64);
// [16] void setTotalMinutes(Int64);
// [16] void setTotalSeconds(Int64);
// [16] void setTotalMilliseconds(Int64);
// [16] void setTotalMicroseconds(Int64);
// [16] void setTotalNanoseconds(Int64);
// [18] void addInterval(Int64, int);
// [11] void setInterval(Int64, int);
// [ 2] void setIntervalRaw(Int64, int);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] bsls::Types::Int64 seconds() const;
// [ 4] int nanoseconds() const;
// [17] Int64 totalSeconds() const;
// [17] Int64 totalMinutes() const;
// [17] Int64 totalHours() const;
// [17] Int64 totalDays() const;
// [17] Int64 totalMilliseconds() const;
// [17] Int64 totalMicroseconds() const;
// [17] Int64 totalNanoseconds() const;
// [17] double totalSecondsAsDouble() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream&, int, int) const;
//
// FREE OPERATORS
// [20] TimeInterval operator+(const Obj& lhs, const Obj& rhs);
// [20] TimeInterval operator+(const Obj& lhs, double rhs);
// [20] TimeInterval operator+(double lhs, const Obj& rhs);
// [20] TimeInterval operator-(const Obj& lhs, const Obj& rhs);
// [20] TimeInterval operator-(const Obj& lhs, double rhs);
// [20] TimeInterval operator-(double lhs, const Obj& rhs);
// [21] TimeInterval operator-(const TimeInterval& rhs);
// [ 6] bool operator==(const TimeInterval&, const TimeInterval&);
// [14] bool operator==(const TimeInterval&, double);
// [14] bool operator==(double, const TimeInterval&);
// [ 6] bool operator!=(const TimeInterval&, const TimeInterval&);
// [14] bool operator!=(const TimeInterval&, double);
// [14] bool operator!=(double, const TimeInterval&);
// [15] bool operator< (const TimeInterval&, const TimeInterval&);
// [15] bool operator< (const TimeInterval&, double);
// [15] bool operator< (double, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, double);
// [15] bool operator<=(double, const TimeInterval&);
// [15] bool operator> (const TimeInterval&, const TimeInterval&);
// [15] bool operator> (const TimeInterval&, double);
// [15] bool operator> (double, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, double);
// [15] bool operator>=(double, const TimeInterval&);
// [ 5] ostream& operator<<(ostream&, const TimeInterval&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING TEST-DRIVER MACHINERY
// [24] USAGE EXAMPLE
// [ 8] Reserved for 'swap' testing.
// [23] CONCERN: DRQS 65043434

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::TimeInterval Obj;
typedef bsls::Types::Int64 Int64;

#define BUFFER_SIZE 512            // must be large enough to enable BDEX tests
#define VERSION_SELECTOR 20140601

const int k_MILLISECS_PER_SEC     = 1000;        // one thousand
const int k_MICROSECS_PER_SEC     = 1000000;     // one million
const int k_NANOSECS_PER_MICROSEC = 1000;        // one thousand
const int k_NANOSECS_PER_MILLISEC = 1000000;     // one million
const int k_NANOSECS_PER_SEC      = 1000000000;  // one billion

const int k_SECS_PER_MIN          = 60;
const int k_SECS_PER_HOUR         = 60 * k_SECS_PER_MIN;
const int k_SECS_PER_DAY          = 24 * k_SECS_PER_HOUR;

const bsls::Types::Int64 k_SECS_MAX = LLONG_MAX;         // max number of secs
const bsls::Types::Int64 k_SECS_MIN = LLONG_MIN;         // min number of secs

const bsls::Types::Int64 k_MINS_MAX  = LLONG_MAX / 60;   // max number of mins
const bsls::Types::Int64 k_MINS_MIN  = LLONG_MIN / 60;   // min number of mins

const bsls::Types::Int64 k_HOURS_MAX = LLONG_MAX / 3600; // max number of hours
const bsls::Types::Int64 k_HOURS_MIN = LLONG_MIN / 3600; // min number of hours

const bsls::Types::Int64 k_DAYS_MAX  = LLONG_MAX / 86400; // max number of days
const bsls::Types::Int64 k_DAYS_MIN  = LLONG_MIN / 86400; // min number of days

const int k_BDEX_SIZEOF_INT32 = 4;
const int k_BDEX_SIZEOF_INT64 = 8;

// ============================================================================
//                        HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//                   GENERIC STREAMING OPERATOR SIMULATION
// ----------------------------------------------------------------------------

namespace testadl {

struct CustomStream {
    int d_counter;
};

template <class OBJECT_TYPE>
CustomStream& operator<<(CustomStream& stream, const OBJECT_TYPE& /* object */)
    // Increment the internal counter of the specified 'stream', and return a
    // reference providing modifiable access to the 'stream'.
{
    ++stream.d_counter;

    return stream;
}

template <class OBJECT_TYPE>
native_std::ostream& operator<<(native_std::ostream& stream,
                                const OBJECT_TYPE&   /* object */)
    // Return a reference providing modifiable access to the specified
    // 'stream'.
{
    return stream;
}

struct UserType {
};

native_std::ostream& operator<<(native_std::ostream& stream,
                                const UserType&      /* object */)
    // Return a reference providing modifiable access to the specified
    // 'stream'.
{
    return stream;
}

}  // close namespace testadl


// ----------------------------------------------------------------------------
//                           BSLS_TESTUTIL SUPPORT
// ----------------------------------------------------------------------------


namespace BloombergLP {
namespace bsls {

void debugprint(const bsls::TimeInterval& timeInterval)
    // Write the specified 'timeInterval' to the console.
{
    printf("(%lld, %d)", timeInterval.seconds(), timeInterval.nanoseconds());
}

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_MSVC
// Microsoft Visual Studio annoyingly doesn't (yet) implement 'snprintf'.  The
// following will do for the purposes of 'TestStream'; note however the
// error behavior for '_snprintf' is different from 'snprintf'.

#define snprintf _snprintf
#endif

                        // ===========================
                        // class TestInStreamException
                        // ===========================

class TestInStreamException {
    // This class defines an exception object for unexternalization operations.
    // Instances of this class contain information about an unexternalization
    // request.

    // DATA
    int d_dataType;  // type of the input data requested

  public:
    // CREATORS
    explicit TestInStreamException(int type);
        // Create an exception object initialized with the specified 'type'.

    //! ~TestInStreamException() = default;
        // Destroy this object.  Note that this method's definition is compiler
        // generated.

    // ACCESSORS
    int dataType() const;
        // Return the type code that was supplied at construction of this
        // exception object.
};
// CREATORS
inline
TestInStreamException::TestInStreamException(int type)
: d_dataType(type)
{
}

// ACCESSORS
inline
int TestInStreamException::dataType() const
{
    return d_dataType;
}

               // ============================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
               // ============================================

#ifdef BDE_BUILD_TARGET_EXC

class TestInStream_ProxyBase {
    // This class provides a common base class for the parameterized
    // 'TestInStream_Proxy' class (below).  Note that the 'virtual'
    // 'setInputLimit' method, although a "setter", *must* be declared 'const'.

  public:
    virtual ~TestInStream_ProxyBase()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const = 0;
};

template <class BSLX_STREAM_TYPE>
class TestInStream_Proxy: public TestInStream_ProxyBase {
    // This class provides a proxy to the test stream that is supplied to the
    // 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' macro.  This proxy may be
    // instantiated with 'TestInStream', or with a type that supports the same
    // interface as 'TestInStream'.

    // DATA
    BSLX_STREAM_TYPE *d_stream_p;  // stream used in '*_BEGIN' and
                                   // '*_END' macros (held, not owned)

  public:
    // CREATORS
    TestInStream_Proxy(BSLX_STREAM_TYPE *stream)
    : d_stream_p(stream)
    {
    }

    ~TestInStream_Proxy()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const
    {
        d_stream_p->setInputLimit(limit);
    }
};

template <class BSLX_STREAM_TYPE>
inline
TestInStream_Proxy<BSLX_STREAM_TYPE>
TestInStream_getProxy(BSLX_STREAM_TYPE *stream)
    // Return, by value, a test stream proxy for the specified parameterized
    // 'stream'.
{
    return TestInStream_Proxy<BSLX_STREAM_TYPE>(stream);
}

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(BSLX_TESTINSTREAM)             \
{                                                                             \
    const TestInStream_ProxyBase& testInStream =                              \
                             TestInStream_getProxy(&BSLX_TESTINSTREAM);       \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime)                                         \
                printf("### BSLX EXCEPTION TEST -- (ENABLED) --\n");          \
        firstTime = 0;                                                        \
    }                                                                         \
    if (veryVeryVerbose) printf("### Begin BSLX exception test.\n");          \
    int bslxExceptionCounter = 0;                                             \
    static int bslxExceptionLimit = 100;                                      \
    testInStream.setInputLimit(bslxExceptionCounter);                         \
    do {                                                                      \
        try {
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(testInStream)                  \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) {                                               \
        printf("### BSLX EXCEPTION TEST -- (NOT ENABLED) --\n");              \
        firstTime = 0;                                                        \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#endif  // BDE_BUILD_TARGET_EXC

               // ==========================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_END
               // ==========================================

#ifdef BDE_BUILD_TARGET_EXC

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END                                  \
        } catch (TestInStreamException& e) {                                  \
            if ((veryVerbose && bslxExceptionLimit) || veryVeryVerbose)       \
            {                                                                 \
                --bslxExceptionLimit;                                         \
                printf("(%i)", bslxExceptionCounter);                         \
                if (veryVeryVerbose) {                                        \
                    printf(" BSLX EXCEPTION: input limit = %i,"               \
                           "last data type = %i",                             \
                           bslxExceptionCounter,                              \
                           e.dataType());                                     \
                }                                                             \
                else if (0 == bslxExceptionLimit) {                           \
                    printf(" [ Note: 'bslxExceptionLimit' reached. ]");       \
                }                                                             \
                printf("\n");                                                 \
            }                                                                 \
            testInStream.setInputLimit(++bslxExceptionCounter);               \
            continue;                                                         \
        }                                                                     \
        testInStream.setInputLimit(-1);                                       \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) {                                                    \
        printf("### End BSLX exception test.\n");                             \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_END

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#endif

#endif  // BDE_BUILD_TARGET_EXC

                         // ==================
                         // class ByteInStream
                         // ==================

class ByteInStream {
    // This class provides input methods to unexternalize 'bsls::Types::Int64'
    // and 'int' values from their byte representations.

    // DATA
    const char *d_buffer;      // bytes to be unexternalized

    int         d_numBytes;    // number of bytes in 'd_buffer'

    bool        d_validFlag;   // stream validity flag; 'true' if stream is in
                               // valid state, 'false' otherwise

    int         d_inputLimit;  // number of input op's before exception

    int         d_cursor;      // index of the next byte to be extracted from
                               // this stream

  private:
    // PRIVATE MANIPULATORS
    void throwExceptionIfInputLimitExhausted(int code);
        // Decrement the internal input limit of this test stream.  If the
        // input limit becomes negative and exception-handling is enabled
        // (i.e., '-DBDE_BUILD_TARGET_EXC' was supplied at compile time), then
        // throw a 'TestInStreamException' object initialized with the
        // specified type 'code'.  If exception-handling is not enabled, this
        // method has no effect.

    // NOT IMPLEMENTED
    ByteInStream(const ByteInStream&);
    ByteInStream& operator=(const ByteInStream&);

  public:
    // CREATORS
    ByteInStream(const char *buffer, int numBytes);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  The behavior is undefined
        // unless '0 <= numBytes' and, if '0 == buffer', then '0 == numBytes'.

    ~ByteInStream();
        // Destroy this object.

    // MANIPULATORS
    ByteInStream& getInt64(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the
        // 64-bit signed integer value into the specified 'variable', update
        // the cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.

    ByteInStream& getInt32(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the
        // 32-bit signed integer value into the specified 'variable', update
        // the cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    void setInputLimit(int limit);
        // Set the number of input operations allowed on this stream to the
        // specified 'limit' before an exception is thrown.  If 'limit' is less
        // than 0, no exception is to be thrown.  By default, no exception is
        // scheduled.

    void reset();
        // Set the index of the next byte to be extracted from this stream to 0
        // (i.e., the beginning of the stream) and validate this stream if it
        // is currently invalid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an input operation was
        // detected to have failed.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable external memory
        // buffer of this stream.  The behavior of accessing elements outside
        // the range '[ data() .. data() + (length() - 1) ]' is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream in which insufficient or invalid data was
        // detected during an extraction operation.  Note that an empty stream
        // will be valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  Note
        // that this function enables higher-level components to verify that,
        // after successfully reading all expected data, no data remains.

    int length() const;
        // Return the total number of bytes stored in the external memory
        // buffer.
};

// PRIVATE MANIPULATORS
inline
void ByteInStream::throwExceptionIfInputLimitExhausted(int code)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw TestInStreamException(code);
        }
    }
#endif
}

// CREATORS
inline
ByteInStream::ByteInStream(const char *buffer, int numBytes)
: d_buffer(buffer)
, d_numBytes(numBytes)
, d_validFlag(1)
, d_inputLimit(-1)
, d_cursor(0)
{
    BSLS_ASSERT_SAFE(buffer || 0 == numBytes);
    BSLS_ASSERT_SAFE(0 <= numBytes);
}

inline
ByteInStream::~ByteInStream()
{
}

// MANIPULATORS
inline
ByteInStream& ByteInStream::getInt64(bsls::Types::Int64& variable)
{
    throwExceptionIfInputLimitExhausted(64);

    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    if (cursor() + k_BDEX_SIZEOF_INT64 <= length()) {
        const char *buffer = d_buffer + cursor();

        if (sizeof variable > k_BDEX_SIZEOF_INT64) {
            variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
        }

        char *bytes = reinterpret_cast<char *>(&variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
        bytes[7] = buffer[0];
        bytes[6] = buffer[1];
        bytes[5] = buffer[2];
        bytes[4] = buffer[3];
        bytes[3] = buffer[4];
        bytes[2] = buffer[5];
        bytes[1] = buffer[6];
        bytes[0] = buffer[7];
#else
        memcpy(bytes, buffer, 8);
#endif

        d_cursor += k_BDEX_SIZEOF_INT64;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt32(int& variable)
{
    throwExceptionIfInputLimitExhausted(32);

    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    if (cursor() + k_BDEX_SIZEOF_INT32 <= length()) {
        const char *buffer = d_buffer + cursor();

        if (sizeof variable > k_BDEX_SIZEOF_INT32) {
            variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
        }

        char *bytes = reinterpret_cast<char *>(&variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
        bytes[3] = buffer[0];
        bytes[2] = buffer[1];
        bytes[1] = buffer[2];
        bytes[0] = buffer[3];
#else
        memcpy(bytes, buffer, 4);
#endif

        d_cursor += k_BDEX_SIZEOF_INT32;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
void ByteInStream::invalidate()
{
    d_validFlag = false;
}

inline
void ByteInStream::setInputLimit(int limit)
{
    d_inputLimit = limit;
}

inline
void ByteInStream::reset()
{
    d_validFlag = true;
    d_cursor    = 0;
}

// ACCESSORS
inline
ByteInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
int ByteInStream::cursor() const
{
    return d_cursor;
}

inline
const char *ByteInStream::data() const
{
    return d_numBytes ? d_buffer : 0;
}

inline
bool ByteInStream::isValid() const
{
    return d_validFlag;
}

inline
bool ByteInStream::isEmpty() const
{
    return cursor() == length();
}

inline
int ByteInStream::length() const
{
    return d_numBytes;
}

typedef ByteInStream  In;

                         // ===================
                         // class ByteOutStream
                         // ===================

class ByteOutStream {
    // This class provides output methods to externalize 'bsls::Types::Int64'
    // and 'int' values from their byte representations.  In particular, each
    // 'put' method of this class is guaranteed to write stream data that can
    // be read by the corresponding 'get' method of 'ByteInStream'.

    // DATA
    char            d_buffer[BUFFER_SIZE];  // byte buffer to write to

    int             d_size;         // size of data written to buffer

    int             d_validFlag;    // stream validity flag; 'true' if stream
                                    // is in valid state, 'false' otherwise

    // NOT IMPLEMENTED
    ByteOutStream(const ByteOutStream&);
    ByteOutStream& operator=(const ByteOutStream&);

  private:
    // PRIVATE MANIPULATORS
    void validate();
        // Put this output stream into a valid state.  This function has no
        // effect if this stream is already valid.

  public:
    // CREATORS
    explicit ByteOutStream(int serializationVersion);
        // Create an empty output byte stream.  The specified
        // 'serializationVersion' is ignored.

    ~ByteOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.

    ByteOutStream& putInt64(bsls::Types::Int64 value);
        // Write to this stream the eight-byte, two's complement integer (in
        // network byte order) comprised of the least-significant eight bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putInt32(int value);
        // Write to this stream the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    int length() const;
        // Return the number of bytes in this stream.
};

// PRIVATE MANIPULATORS
inline
void ByteOutStream::validate()
{
    d_validFlag = true;
}

// CREATORS
inline
ByteOutStream::ByteOutStream(int /* serializationVersion */)
: d_size(0)
, d_validFlag(true)
{
}

inline
ByteOutStream::~ByteOutStream()
{
}

// MANIPULATORS
inline
void ByteOutStream::invalidate()
{
    d_validFlag = false;
}
inline
ByteOutStream& ByteOutStream::putInt64(bsls::Types::Int64 value)
{
    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    // Write to the buffer the specified 'value'.

    char *buffer = d_buffer + d_size;

    char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[7];
    buffer[1] = bytes[6];
    buffer[2] = bytes[5];
    buffer[3] = bytes[4];
    buffer[4] = bytes[3];
    buffer[5] = bytes[2];
    buffer[6] = bytes[1];
    buffer[7] = bytes[0];
#else
    buffer[0] = bytes[sizeof value - 8];
    buffer[1] = bytes[sizeof value - 7];
    buffer[2] = bytes[sizeof value - 6];
    buffer[3] = bytes[sizeof value - 5];
    buffer[4] = bytes[sizeof value - 4];
    buffer[5] = bytes[sizeof value - 3];
    buffer[6] = bytes[sizeof value - 2];
    buffer[7] = bytes[sizeof value - 1];
#endif

    d_size += k_BDEX_SIZEOF_INT64;

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt32(int value)
{
    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    // Write to the buffer the specified 'value'.

    char *buffer = d_buffer + d_size;

    char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[3];
    buffer[1] = bytes[2];
    buffer[2] = bytes[1];
    buffer[3] = bytes[0];
#else
    buffer[0] = bytes[sizeof value - 4];
    buffer[1] = bytes[sizeof value - 3];
    buffer[2] = bytes[sizeof value - 2];
    buffer[3] = bytes[sizeof value - 1];
#endif

    d_size += k_BDEX_SIZEOF_INT32;

    return *this;
}

// ACCESSORS
inline
ByteOutStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
const char *ByteOutStream::data() const
{
    return d_buffer;
}

inline
bool ByteOutStream::isValid() const
{
    return d_validFlag;
}

inline
int ByteOutStream::length() const
{
    return static_cast<int>(d_size);
}

typedef ByteOutStream Out;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 24: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        {
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Modifying a 'bsls::TimeInterval'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and manipulate a
// 'bsls::TimeInterval' object.
//
// First, we default construct a 'TimeInterval' object, 'interval':
//..
    bsls::TimeInterval interval;
//
    ASSERT(0 == interval.seconds());
    ASSERT(0 == interval.nanoseconds());
//..
// Next, we set the value of 'interval' to 1 second and 10 nanoseconds (a
// time interval of 1000000010 nanoseconds):
//..
    interval.setInterval(1, 10);
//
    ASSERT( 1 == interval.seconds());
    ASSERT(10 == interval.nanoseconds());
//..
// Then, we add 3 seconds to 'interval':
//..
    interval.addInterval(3, 0);
//
    ASSERT( 4 == interval.seconds());
    ASSERT(10 == interval.nanoseconds());
//..
// Next, we create a copy of 'interval', 'intervalPrime':
//..
    bsls::TimeInterval intervalPrime(interval);
//
    ASSERT(intervalPrime == interval);
//..
// Finally, we assign 3.14 seconds to 'intervalPrime', and then add 2.73
// seconds more:
//..
    intervalPrime =  3.14;
    intervalPrime += 2.73;
//
    ASSERT(        5 == intervalPrime.seconds());
    ASSERT(870000000 == intervalPrime.nanoseconds());
//..

        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING: DRQS 65043434
        //   The previous implementation of 'operator<<' for
        //   'bsls::TimeInterval' was templatized on the *stream* type to avoid
        //   a dependency on 'bsl::ostream' (which would have been a
        //   levelization violation).  Ticket DRQS 65043434 reported that this
        //   signature caused ambiguity in ADL lookup in the presence of
        //   facilities that provide a streaming operator templatized on the
        //   *object* type.  Facilities that provide such a fall-back streaming
        //   operator include Google Test, BDEX, and various logging
        //   frameworks.
        //
        // Concerns:
        //:  1 'bsls::operator<<' for 'bsls::TimeInterval' can be unambiguously
        //:    called in a scope that can also see a generic 'operator<<' that
        //:    is templatized on the 'object' parameter.
        //:
        //:  2 Invoking 'operator<<' with arguments of types
        //:    'bsl::ostream' and 'bsls::TimeInterval' calls the function
        //:    provided by 'bsls_timeinterval'.
        //:
        //:  3 Invoking a generic 'operator<<' that does not provide a
        //:    specialization for 'bsls::TimeInterval' will call the generic
        //:    function.
        //
        // Plan:
        //:  1 Define generic streaming operators templatized on the 'object'
        //:    type, and attempt to use them to stream objects of type
        //:    'bsls::TimeInterval'.  Under the old implementation of
        //:    'bsls::operator<<' for 'bsls::TimeInterval', these calls will
        //:    not compile.  Under the new implementation, these calls will
        //:    compile.  (C-1)
        //:
        //:  2 Define a streaming operator that accepts arbitrary
        //:    objects to be streamed to an 'bsl::ostream', and that does not
        //:    modify the stream.  Confirm that streaming a
        //:    'bsls::TimeInterval' to a 'bsl::ostream' does change the stream,
        //:    thereby confirming that the correct streaming operator is
        //:    selected.  (C-2)
        //:
        //:  3 Define a streaming operator that accepts arbitrary
        //:    objects to be streamed to a user-defined stream, with some
        //:    observable side-effect.  Confirm that streaming a
        //:    'bsls::TimeInterval' to such a 'bsl::ostream' causes the
        //:    side-effect, thereby confirming that the correct streaming
        //:    operator is selected.  (C-3)
        //
        // Testing:
        //   CONCERN: DRQS 65043434
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: DRQS 65043434"
                            "\n======================\n");

        const bsls::TimeInterval interval;
        const int                integer = 42;
        const testadl::UserType  object  = {};

        using namespace testadl;

        if (verbose) printf("\nStreaming to native_std::ostream\n");
        {
            native_std::ostringstream stream;

            native_std::string::size_type lastLength = 0;
            ASSERT(lastLength == stream.str().length());

            stream << interval;
            ASSERT(lastLength < stream.str().length());
            lastLength = stream.str().length();

            stream << integer;
            ASSERT(lastLength < stream.str().length());
            lastLength = stream.str().length();

            stream << object;
            ASSERT(lastLength == stream.str().length());
        }

        if (verbose) printf("\nStreaming to custom stream\n");
        {
            CustomStream stream = { 0 };

            stream << interval;
            ASSERT(1 == stream.d_counter);

            stream << integer;
            ASSERT(2 == stream.d_counter);

            stream << object;
            ASSERT(3 == stream.d_counter);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING: 'isValid'
        //   Ensure that 'isValid' correctly indicates whether the
        //   input is a valid time interval.
        //
        // Concerns:
        //: 1 That 'isValid' is a class method.
        //:
        //: 2 That 'isValid' returns true for any combination of 'second' and
        //:   'nanosecond' values that can be represented using a
        //:   'TimeInterval', and 'false' otherwise.
        //
        // Plan:
        //: 1 Using a table driven technique, call 'isValid' and test the
        //:   result against the expected result. (C-1..2)
        //
        // Testing:
        //   bool isValid(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'isValid'"
                            "\n==================\n");

        struct {
            int    d_lineNum;
            Int64  d_secs;
            int    d_nsecs;
            bool   d_isValid;
        } DATA[] = {
            //line     secs     nanosecs isValid
            //----     ----     -------- -------
            { L_,         0,           0,  true },
            { L_, LLONG_MAX,           0,  true },
            { L_, LLONG_MAX,   999999999,  true },
            { L_, LLONG_MAX,  -999999999,  true },
            { L_, LLONG_MAX, -1000000000,  true },
            { L_, LLONG_MAX,  1000000000, false },
            { L_, LLONG_MAX,  1000000001, false },
            { L_, LLONG_MAX,  1999999999, false },
            { L_, LLONG_MAX,     INT_MAX, false },
            { L_, LLONG_MIN,           0,  true },
            { L_, LLONG_MIN,  -999999999,  true },
            { L_, LLONG_MIN,   999999999,  true },
            { L_, LLONG_MIN,  1000000000,  true },
            { L_, LLONG_MIN, -1000000000, false },
            { L_, LLONG_MIN, -1000000001, false },
            { L_, LLONG_MIN, -1999999999, false },
            { L_, LLONG_MIN,     INT_MIN, false },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'isValid' with test table\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;
            const bool  IEXP   = DATA[i].d_isValid;

            if (veryVerbose) { T_;  P_(ISECS); P_(INSECS); P(IEXP); }

            ASSERTV(ILINE, IEXP == Obj::isValid(ISECS, INSECS));
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING: 'operator-' (NEGATION)
        //   Ensure the negation operator returns a time interval having the
        //   negation of the supplied input.
        //
        // Concerns:
        //: 1 The object that is returned by the free 'operator-' (negation)
        //:   has a time interval that represents the negation of the input
        //:   value.
        //:
        //: 2 The operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The value of the source object is not modified.
        //:
        //: 4 Non-modifiable objects can be negated (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 5 The operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the address of 'operator-' to initialize a function pointer
        //:   having the appropriate signature and return type for the
        //:   negation operator defined in this component.  (C-4..5)
        //:
        //: 2 Using a table driven technique, generate a test object for each
        //:   test value, negate that test object and copy the value to a
        //:   new object.  Verify the canonical properties of the resulting
        //:   object are the negation of the original object. (C-1..3)
        //
        // Testing:
        //   TimeInterval operator-(const TimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'operator-' (NEGATION)"
                           "\n===============================\n");

        if (verbose) printf(
                     "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj (*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorNeg = bsls::operator-;

            (void)operatorNeg;  // quash potential compiler warnings
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator-' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;


            if (veryVerbose) { T_;  P(X); }

            Obj mY = -X; const Obj& Y = mY;

            ASSERTV(ILINE, ILINE, -ISECS   == Y.seconds());
            ASSERTV(ILINE, ILINE, -INSECS  == Y.nanoseconds());
            ASSERTV(ILINE, ILINE,  ISECS   == X.seconds());
            ASSERTV(ILINE, ILINE,  INSECS  == X.nanoseconds());
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // ARITHMETIC FREE OPERATORS (+, -)
        //   Ensure that each operator correctly computes the underlying
        //   canonical representation of the result object.
        //
        // Concerns:
        //: 1 The object that is returned by the free 'operator+' ('operator-')
        //:   has a time interval that represents the sum (difference) of those
        //:   of the two operands.
        //:
        //: 2 Each operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The values of the two source objects supplied to each operator
        //:   are not modified.
        //:
        //: 4 Non-modifiable objects can be added and subtracted (i.e., objects
        //:   or references providing only non-modifiable access).
        //:
        //: 5 The operators' signatures and return types are standard.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+' and 'operator-' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the six free binary arithmetic operators
        //:   defined in this component.  (C-4..5)
        //:
        //: 2 Using a table driven technique, generate 2 test objects for a
        //:   cross-product of test values, and then add and substract (using
        //:   the operator under test) those two values, comparing the result
        //:   to an expected value generated by using previously tested
        //:   arithmetic member methods.  (C-1..3)
        //:
        //: 3 Using a table driven technique, generate a test object and
        //:   a 'double' value for a cross-product of test values, and then add
        //:   and substract (using the operator under test) those two values,
        //:   comparing the result to an expected value generated by using
        //:   previously tested arithmetic member methods.  (C-1..3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval operator+(const Obj& lhs, const Obj& rhs);
        //   TimeInterval operator+(const Obj& lhs, double rhs);
        //   TimeInterval operator+(double lhs, const Obj& rhs);
        //   TimeInterval operator-(const Obj& lhs, const Obj& rhs);
        //   TimeInterval operator-(const Obj& lhs, double rhs);
        //   TimeInterval operator-(double lhs, const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nARITHMETIC FREE OPERATORS (+, -)"
                            "\n================================\n");

        if (verbose) printf(
            "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef Obj (*operatorPtr)(const Obj&, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
            {
                typedef Obj (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
            {
                typedef Obj (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator+' and 'operator-' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                Obj mY(JSECS, JNSECS); const Obj& Y = mY;

                if (veryVerbose) { T_;  P_(X);  P(Y); }

                {
                    // Addition

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    mZ += Y;

                    mW = X + Y;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    mZ -= Y;

                    mW = X - Y;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
            }
        }

        if (verbose) printf(
            "\nTesting: 'operator+' and 'operator-' with 'double'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                const double dYv = static_cast<double>(JSECS) +
                                   static_cast<double>(JNSECS) /
                                   static_cast<double>(k_NANOSECS_PER_SEC);


                if (veryVerbose) { T_;  P_(X);  P(dYv); }

                {
                    // Addition

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    Obj mV;    const Obj& V = mV;

                    mZ += dYv;

                    mW = X + dYv;
                    mV = dYv + X;

                    if (veryVerbose) { T_; T_; P_(V); P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, Z       == V);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    Obj mV;    const Obj& V = mV;

                    mZ -= dYv;

                    // TimeInterval - double.

                    mW = X - dYv;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());

                    mZ = Obj(dYv);
                    mZ -= X;

                    // double - TimeInterval

                    mV = dYv - X;

                    if (veryVerbose) { T_; T_; P_(V);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == V);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'operator+ w/ 'TimeInterval'\n");
            {
                Obj mX(LLONG_MAX, 999999998); const Obj& X = mX;
                Obj mY(        0,         1); const Obj& Y = mY;


                ASSERT_PASS(X + Y);

                mX.addNanoseconds(1);

                ASSERT_FAIL(X + Y);

                mX.setInterval(LLONG_MIN, -999999998);
                mY.setInterval(        0,         -1);

                ASSERT_PASS(X + Y);

                mX.addNanoseconds(-1);

                ASSERT_FAIL(X + Y);
            }

            if (veryVerbose) printf("\t'operator- w/ 'TimeInterval'\n");
            {
                Obj mX(LLONG_MAX, 999999998); const Obj& X = mX;
                Obj mY(        0,        -1); const Obj& Y = mY;


                ASSERT_PASS(X - Y);

                mX.addNanoseconds(1);

                ASSERT_FAIL(X - Y);

                mX.setInterval(LLONG_MIN, -999999998);
                mY.setInterval(        0,          1);

                ASSERT_PASS(X - Y);

                mX.addNanoseconds(-1);

                ASSERT_FAIL(X - Y);
            }

            if (veryVerbose) printf("\t'operator+ w/ 'double'\n");
            {
                Obj mX(static_cast<double>(LLONG_MAX) * .9);
                const Obj& X = mX;

                double dYv = static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   + dYv);
                ASSERT_PASS(dYv + X);

                ASSERT_PASS(mX += dYv);

                ASSERT_FAIL(X   + dYv);
                ASSERT_FAIL(dYv + X);

                mX  = Obj(static_cast<double>(LLONG_MIN) * .9);
                dYv = -1 * static_cast<double>(LLONG_MAX) * .08;

                dYv = LLONG_MIN * .09;

                ASSERT_PASS(X   + dYv);
                ASSERT_PASS(dYv + X);

                ASSERT_PASS(mX += dYv);

                ASSERT_FAIL(X   + dYv);
                ASSERT_FAIL(dYv + X);
            }

            if (veryVerbose) printf("\t'operator- w/ 'double'\n");
            {
                Obj mX(static_cast<double>(LLONG_MAX) * -.9);
                const Obj& X = mX;

                double dYv = static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);

                ASSERT_PASS(mX -= dYv);

                ASSERT_FAIL(X   - dYv);
                ASSERT_FAIL(dYv - X);

                mX  = Obj(static_cast<double>(LLONG_MIN) * .9);
                dYv = -1 * static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);

                ASSERT_PASS(mX -= dYv);

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // ARITHMETIC ASSIGNMENT OPERATORS (+=, -=)
        //   Ensure that each operator correctly adjusts the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each compound assignment operator can change the value of any
        //:   modifiable target object based on any source object that does not
        //:   violate the method's documented preconditions.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 The reference returned from each operator is to the target object
        //:   (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified, unless it is an
        //:   alias for the target object.
        //:
        //: 5 A compound assignment of an object to itself behaves as expected
        //:   (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+=' and 'operator-=' to
        //:   initialize member-function pointers having the appropriate
        //:   signatures and return types for the two compound assignment
        //:   operators defined in this component.  (C-2)
        //:
        //: 2 Using a table driven technique, generate a test object for a
        //:   series of initial values, and then add and substract (using the
        //:   operator under test) a series of 'TimeInterval' values from that
        //:   initial value.  Verify the results. (C-1, 3..4).
        //:
        //: 3 Using a table driven technique, create a test-value and add and
        //:   substract it from itself.  Verify the return value, and the
        //:   resulting object value. (C-5).
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval& operator+=(const TimeInterval&);
        //   TimeInterval& operator-=(const TimeInterval&);
        //   TimeInterval& operator+=(double);
        //   TimeInterval& operator-=(double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nARITHMETIC ASSIGNMENT OPERATORS (+=, -=)"
                            "\n========================================\n");

        if (verbose) printf(
            "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef Obj& (Obj::*operatorPtr)(const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAddAssignment = &Obj::operator+=;
                operatorPtr operatorSubAssignment = &Obj::operator-=;

                (void)operatorAddAssignment; // quash compiler warning
                (void)operatorSubAssignment;
            }
            {
                typedef Obj& (Obj::*operatorPtr)(double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAddAssignment = &Obj::operator+=;
                operatorPtr operatorSubAssignment = &Obj::operator-=;

                (void)operatorAddAssignment; // quash compiler warning
                (void)operatorSubAssignment;
            }
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator+=' and 'operator-=' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                Obj mY(JSECS, JNSECS); const Obj& Y = mY;

                if (veryVerbose) { T_;  P_(X);  P_(Y); }

                {
                    // Addition.

                    Obj *mR = &(mX += Y);

                    Obj mZ(ISECS + JSECS, INSECS + JNSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z       == X);
                    ASSERTV(ILINE, JLINE, mR      == &X);
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj *mR = &(mX -= Y);

                    Obj mZ(ISECS, INSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z       == X);
                    ASSERTV(ILINE, JLINE, mR      == &X);
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());

                }
            }
        }

        if (verbose) printf("\t'Testing self-assignment.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            const Obj& Z = mX;

            if (veryVerbose) { T_ P_(X) }

            Obj mY(2 * ISECS, 2 * INSECS);  const Obj& Y = mY;

            if (veryVerbose) { T_ T_ P(Y) }

            {
                // Addition.

                Obj *mR = &(mX += Z);

                ASSERTV(ILINE,  Y,   X,  Y == X);
                ASSERTV(ILINE, mR, &mX, mR == &mX);
            }

            mY.setInterval(0, 0);

            {
                // Subtraction.

                Obj *mR = &(mX -= Z);

                ASSERTV(ILINE,  Y,   X,  Y == X);
                ASSERTV(ILINE, mR, &mX, mR == &mX);
            }
        }

        if (verbose) printf(
            "\nTesting: 'operator+=' and 'operator-=' with 'double'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                double y = static_cast<double>(JSECS) +
                           static_cast<double>(JNSECS) /
                           static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_;  P_(X);  P_(y); }

                {
                    // Addition.

                    Obj *mR = &(mX += y);

                    Obj mZ(ISECS + JSECS, INSECS + JNSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z  == X);
                    ASSERTV(ILINE, JLINE, mR == &X);
                }
                {
                    // Subtraction.

                    Obj *mR = &(mX -= y);

                    Obj mZ(ISECS, INSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z  == X);
                    ASSERTV(ILINE, JLINE, mR == &X);
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'operator+= w/ 'TimeInterval'\n");
            {
                Obj mX(0, 0);
                Obj mY(LLONG_MAX, 999999999); const Obj& Y = mY;

                ASSERT_PASS(mX += Y);
                ASSERT_FAIL(mX += Y);

                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN, -999999999);

                ASSERT_PASS(mX += Y);
                ASSERT_FAIL(mX += Y);

            }

            if (veryVerbose) printf("\t'operator-= w/ 'TimeInterval'\n");
            {
                Obj mX(0, 0);
                Obj mY(LLONG_MAX, 999999999); const Obj& Y = mY;

                ASSERT_PASS(mX -= Y);
                ASSERT_FAIL(mX -= Y);


                // Note that we cannot subtract a LLONG_MIN number of seconds.
                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN + 1, -999999999);

                ASSERT_PASS(mX -= Y);
                ASSERT_FAIL(mX -= Y);

                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN, -999999999);

                ASSERT_SAFE_FAIL(mX -= Y);
            }

            if (veryVerbose) printf("\t'operator+= w/ 'double'\n");
            {
                Obj mX(0, 0);
                double y = static_cast<double>(LLONG_MAX) * .999;

                ASSERT_PASS(mX += y);
                ASSERT_FAIL(mX += y);

                mX.setInterval(0, 0);
                y = -y;

                ASSERT_PASS(mX += y);
                ASSERT_FAIL(mX += y);

            }

            if (veryVerbose) printf("\t'operator-= w/ 'double'\n");
            {
                Obj mX(0, 0);
                double y = static_cast<double>(LLONG_MAX) * .999;

                ASSERT_PASS(mX -= y);
                ASSERT_FAIL(mX -= y);

                mX.setInterval(0, 0);
                y = -y;

                ASSERT_PASS(mX -= y);
                ASSERT_FAIL(mX -= y);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING: 'add*' MANIPULATORS
        //   Ensure that each method correctly adjusts the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each method correctly updates the object state from the state on
        //:   entry and the supplied argument.
        //:
        //: 2 The 'nanoseconds' argument to 'addInterval' defaults to 0.
        //:
        //: 3 Each manipulator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, call 'addInterval' on a
        //:   default constructed 'TimeInterval' and verify the result is
        //:   equivalent to using the value constructor.
        //:
        //: 2 Using the table-driven technique, call 'addInterval' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 3 Using the table-driven technique, call 'addInterval' on a
        //:   'TimeInterval' assigned some initial value, without supplying a
        //:   number of 'nanoseconds', and verify the result uses the expected
        //:   default argument value of 0.  (C-2)
        //:
        //: 4 Using the table-driven technique, call 'addDays' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 5 Using the table-driven technique, call 'addHours' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 6 Using the table-driven technique, call 'addMinutes' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 7 Using the table-driven technique, call 'addSeconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 8 Using the table-driven technique, call 'addMilliseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 9 Using the table-driven technique, call 'addMicroseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //:10 Using the table-driven technique, call 'addNanoseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //:11 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not
        //:   triggered for adjacent valid ones (using the
        //:   'BSLS_ASSERTTEST_*' macros). (C-4)
        //
        // Testing:
        //   TimeInterval& addDays(bsls::Types::Int64);
        //   TimeInterval& addHours(bsls::Types::Int64);
        //   TimeInterval& addMinutes(bsls::Types::Int64);
        //   TimeInterval& addSeconds(bsls::Types::Int64);
        //   TimeInterval& addMilliseconds(Int64);
        //   TimeInterval& addMicroseconds(Int64);
        //   TimeInterval& addNanoseconds(Int64);
        //   void addInterval(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'add*' MANIPULATORS"
                            "\n============================\n");

        if (verbose) printf(
            "\nTesting: 'addInterval' w/o initial value\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
            } DATA[] = {
                //line         secs      nanosecs
                //----         ----      --------
                { L_,            0,             0 },
                { L_,            1,             2 },
                { L_,            1,    1000000000 },
                { L_,           -1,   -1000000000 },
                { L_,            2,   -1000000001 },
                { L_,           -2,    1000000001 },
                { L_,   k_SECS_MAX,     999999999 },
                { L_,   k_SECS_MIN,    -999999999 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const int                NSECS  = DATA[i].d_nsecs;

                Obj mX; const Obj& X = mX;

                mX.addInterval(SECS, NSECS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(SECS, NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf(
            "\nTesting: 'addInterval' w/ initial value\n");

        {
            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add seconds
                int                d_addNSecs;    // Add nanoseconds
            } DATA[] = {
                //line    init secs init nanosecs    add secs     add nsecs
                //----    --------- -------------    --------     ---------
                { L_,            1,             2,      50000,      123123 },
                { L_,            1,             2,     -50000,      123123 },
                { L_,            1,             2,     -50000,     -123123 },
                { L_,            1,             2,     -50000,  1000000000 },
                { L_,            1,             2,     -50000,  1100000000 },
                { L_,            1,             2,     -50000, -1100000000 },
                { L_,       -56711,         76522,      50000,      123123 },
                { L_,       -56711,         76542,     -50000,      123123 },
                { L_,       -56711,         53245,     -50000,     -123123 },
                { L_,       -56711,         74562,     -50000,  1000000000 },
                { L_,       -56711,         43552,     -50000,  1100000000 },
                { L_,       -56711,         54322,     -50000, -1100000000 },
                { L_,   k_SECS_MAX,     999999999,          0,           0 },
                { L_,   k_SECS_MAX,     999999999,          0,          -1 },
                { L_,   k_SECS_MAX,     999999998,          0,           1 },
                { L_,   k_SECS_MIN,    -999999999,          0,           0 },
                { L_,   k_SECS_MIN,    -999999999,          0,           1 },
                { L_,   k_SECS_MIN,    -999999998,          0,          -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS    = DATA[i].d_addSecs;
                const int                ADD_NSECS   = DATA[i].d_addNSecs;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addInterval(ADD_SECS, ADD_NSECS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS + ADD_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf(
            "\nTesting: 'addInterval' default 'nanoseconds' value\n");

        {
            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add seconds
            } DATA[] = {
                //line    init secs init nanosecs    add secs
                //----    --------- -------------    --------
                { L_,            1,             2,      50000 },
                { L_,            1,             2,     -50000 },
                { L_,       -56711,         76522,      50000 },
                { L_,       -56711,         76542,     -50000 },
                { L_,       -56711,         43552,     -50000 },
                { L_,       -56711,         54322,     -50000 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS    = DATA[i].d_addSecs;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addInterval(ADD_SECS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addDays'\n");
        {

            // These constants define the maximum (and minimum) second value to
            // which 'k_DAYS_MAX' (and 'k_DAYS_MIN') days can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_DAYS_MAX * 86400;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_DAYS_MAX * 86400;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addDays;     // Add days
            } DATA[] = {
                //line           init secs    nanosecs        add days
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_DAYS_MAX },
                { L_,                    0,          0,    k_DAYS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,    k_DAYS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,    k_DAYS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,   k_SECS_MAX - 86400,  999999999,             1 },
                { L_,   k_SECS_MIN + 86400,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_DAYS    = DATA[i].d_addDays;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addDays(ADD_DAYS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_DAYS * 86400, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addHours'\n");
        {
            // These constants define the maximum (and minimum) second value to
            // which 'k_HOURS_MAX' (and 'k_HOURS_MIN') hours can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_HOURS_MAX * 3600;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_HOURS_MAX * 3600;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addHours;     // Add hours
            } DATA[] = {
                //line           init secs    nanosecs         add hours
                //----           ---------    --------         --------
                { L_,                    1,          2,             123 },
                { L_,                    1,          2,            -321 },
                { L_,                    1,          2,             123 },
                { L_,                    1,          2,            -321 },
                { L_,               123456,     654321,           10000 },
                { L_,               123456,     654321,          -10000 },
                { L_,              -123456,    -654321,           10000 },
                { L_,              -123456,    -654321,          -10000 },
                { L_,                    0,          0,     k_HOURS_MAX },
                { L_,                    0,          0,     k_HOURS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,     k_HOURS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,     k_HOURS_MIN },
                { L_,           k_SECS_MAX,  999999999,               0 },
                { L_,           k_SECS_MIN, -999999999,               0 },
                { L_,    k_SECS_MAX - 3600,  999999999,               1 },
                { L_,    k_SECS_MIN + 3600,  999999999,              -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_HOURS    = DATA[i].d_addHours;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addHours(ADD_HOURS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_HOURS * 3600, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMinutes'\n");
        {
            // These constants define the maximum (and minimum) second value to
            // which 'k_MINS_MAX' (and 'k_MINS_MIN') minutes can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_MINS_MAX * 60;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_MINS_MAX * 60;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMins;     // Add mins
            } DATA[] = {
                //line           init secs    nanosecs        add mins
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_MINS_MAX },
                { L_,                    0,          0,    k_MINS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,    k_MINS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,    k_MINS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,      k_SECS_MAX - 60,  999999999,             1 },
                { L_,      k_SECS_MIN + 60,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MINS    = DATA[i].d_addMins;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addMinutes(ADD_MINS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_MINS * 60, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addSeconds'\n");
        {

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add secs
            } DATA[] = {
                //line           init secs    nanosecs        add secs
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_SECS_MAX },
                { L_,                    0,          0,    k_SECS_MIN },
                { L_,                    1,          0, -1+k_SECS_MAX },
                { L_,                   -1,          0,  1+k_SECS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,       k_SECS_MAX - 1,  999999999,             1 },
                { L_,       k_SECS_MIN + 1,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS    = DATA[i].d_addSecs;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addSeconds(ADD_SECS);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMilliseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // milliseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_MILLISECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                999999999 - (LLONG_MAX % k_MILLISECS_PER_SEC) *
                                                      k_NANOSECS_PER_MILLISEC);

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_MILLISECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
               -999999999 - (LLONG_MIN % k_MILLISECS_PER_SEC) *
                                                      k_NANOSECS_PER_MILLISEC);


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMillis;   // Add millis
            } DATA[] = {
                //line        init secs           nanosecs      add millis
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,  999999999 - 1000000,            1 },
                { L_,        k_SECS_MIN, -999999999 + 1000000,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MILLIS  = DATA[i].d_addMillis;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addMilliseconds(ADD_MILLIS);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_MILLIS / k_MILLISECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_MILLIS % k_MILLISECS_PER_SEC) *
                                                   k_NANOSECS_PER_MILLISEC));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMicroseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // microseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_MICROSECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                999999999 - (LLONG_MAX % k_MICROSECS_PER_SEC) *
                                                      k_NANOSECS_PER_MICROSEC);

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_MICROSECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
               -999999999 - (LLONG_MIN % k_MICROSECS_PER_SEC) *
                                                      k_NANOSECS_PER_MICROSEC);


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMicros;   // Add micros
            } DATA[] = {
                //line        init secs           nanosecs      add micros
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,     999999999 - 1000,            1 },
                { L_,        k_SECS_MIN,    -999999999 + 1000,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MICROS  = DATA[i].d_addMicros;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addMicroseconds(ADD_MICROS);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_MICROS / k_MICROSECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_MICROS % k_MICROSECS_PER_SEC) *
                                                   k_NANOSECS_PER_MICROSEC));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addNanoseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // nanoseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_NANOSECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                                 999999999 - (LLONG_MAX % k_NANOSECS_PER_SEC));

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_NANOSECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
                                -999999999 - (LLONG_MIN % k_NANOSECS_PER_SEC));


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addNanos;    // Add nanos
            } DATA[] = {
                //line        init secs           nanosecs      add nanos
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,        999999999 - 1,            1 },
                { L_,        k_SECS_MIN,       -999999999 + 1,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE        = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS   = DATA[i].d_initSecs;
                const int                INIT_NSECS  = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_NANOS   = DATA[i].d_addNanos;

                Obj mX; const Obj& X = mX;

                mX.setInterval(INIT_SECS, INIT_NSECS);
                mX.addNanoseconds(ADD_NANOS);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_NANOS / k_NANOSECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_NANOS % k_NANOSECS_PER_SEC)));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'addInterval'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_PASS(mX.addInterval(LLONG_MAX, 999999999));

                    mX.setInterval(0, 0);

                    ASSERT_FAIL(mX.addInterval(LLONG_MAX, 1000000000));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_PASS(mX.addInterval(LLONG_MIN, -999999999));

                    mX.setInterval(0, 0);

                    ASSERT_FAIL(mX.addInterval(LLONG_MIN, -1000000000));
                }
            }

            if (veryVerbose) printf("\t'addDays'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addDays(k_DAYS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addDays(k_DAYS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 86400,  999999999);

                    ASSERT_SAFE_PASS(mX.addDays(1));

                    mX.setInterval(k_SECS_MAX - 86399,  999999999);

                    ASSERT_SAFE_FAIL(mX.addDays(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addDays(k_DAYS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addDays(k_DAYS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 86400,  -999999999);

                    ASSERT_SAFE_PASS(mX.addDays(-1));

                    mX.setInterval(k_SECS_MIN + 86399,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addDays(-1));
                }
            }

            if (veryVerbose) printf("\t'addHours'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 3600,  999999999);

                    ASSERT_SAFE_PASS(mX.addHours(1));

                    mX.setInterval(k_SECS_MAX - 3599,  999999999);

                    ASSERT_SAFE_FAIL(mX.addHours(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 3600,  -999999999);

                    ASSERT_SAFE_PASS(mX.addHours(-1));

                    mX.setInterval(k_SECS_MIN + 3599,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addHours(-1));
                }
            }

            if (veryVerbose) printf("\t'addMinutes'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 60,  999999999);

                    ASSERT_SAFE_PASS(mX.addMinutes(1));

                    mX.setInterval(k_SECS_MAX - 59,  999999999);

                    ASSERT_SAFE_FAIL(mX.addMinutes(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 60,  -999999999);

                    ASSERT_SAFE_PASS(mX.addMinutes(-1));

                    mX.setInterval(k_SECS_MIN + 59,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addMinutes(-1));
                }
            }

            if (veryVerbose) printf("\t'addSeconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addSeconds(LLONG_MAX));

                    mX.setInterval(1, 0);

                    ASSERT_SAFE_FAIL(mX.addSeconds(LLONG_MAX));

                    mX.setInterval(LLONG_MAX - 1,  999999999);

                    ASSERT_SAFE_PASS(mX.addSeconds(1));

                    mX.setInterval(LLONG_MAX,  999999999);

                    ASSERT_SAFE_FAIL(mX.addSeconds(1));
                }
                {
                    // Minimum

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addSeconds(LLONG_MIN));

                    mX.setInterval(-1, 0);

                    ASSERT_SAFE_FAIL(mX.addSeconds(LLONG_MIN));

                    mX.setInterval(LLONG_MIN + 1,  -999999999);

                    ASSERT_SAFE_PASS(mX.addSeconds(-1));

                    mX.setInterval(LLONG_MIN,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addSeconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addMilliseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1000000);

                    ASSERT_PASS(mX.addMilliseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999 - 999999);

                    ASSERT_FAIL(mX.addMilliseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1000000);

                    ASSERT_PASS(mX.addMilliseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999 + 999999);

                    ASSERT_FAIL(mX.addMilliseconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addMicroseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1000);

                    ASSERT_PASS(mX.addMicroseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999 - 999);

                    ASSERT_FAIL(mX.addMicroseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1000);

                    ASSERT_PASS(mX.addMicroseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999 + 999);

                    ASSERT_FAIL(mX.addMicroseconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addNanoseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1);

                    ASSERT_PASS(mX.addNanoseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999);

                    ASSERT_FAIL(mX.addNanoseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1);

                    ASSERT_PASS(mX.addNanoseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999);

                    ASSERT_FAIL(mX.addNanoseconds(-1));
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // 'total*' ACCESSORS
        //   Ensure each 'total*' accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the "total" value corresponding to the
        //:   units indicated by the method's name.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 The 'totalSecondsAsDouble' method does not lose precision in the
        //:   conversion to 'double' of reasonably large time intervals.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        //
        // Plan:
        //   In case 15, we demonstrated that each "total" accessor (with the
        //   exception of 'totalSecondsAsDouble') works properly when invoked
        //   immediately following an application of its corresponding "total"
        //   setter (e.g., 'setTotalMinutes' followed by 'totalMinutes').  Here
        //   we use a more varied set of object values to further corroborate
        //   that these accessors properly interpret object state, and that
        //   'totalSecondsAsDouble' works as expected.
        //
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1   (C-1..3)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'X', with
        //:     the value from 'R'.
        //:
        //:   2 Verify that each 'totalSeconds', 'totalHours', and 'totalDays'
        //:     return values consistent with 'seconds' (C-1..2)
        //:
        //:   3 In the case of 'totalSecondsAsDouble', verification of that
        //:     method is restricted to those values that should not lose
        //:     precision when converted among 'Int64' and 'double'.  (C-3)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalMilliseconds accessor (note that the range of
        //:   'totalMilliseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalMilliseconds' against
        //:   the expected number of milliseconds. (C-1..2)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalMicroseconds accessor (note that the range of
        //:   'totalMicroseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalMicroseconds' against
        //:   the expected number of microseconds.  (C-1..2)
        //:
        //: 5 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalNanoseconds accessor (note that the range of
        //:   'totalNanoseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalNanoseconds' against
        //:   the expected number of nanoseconds.  (C-1..2)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   Int64 totalSeconds() const;
        //   Int64 totalMinutes() const;
        //   Int64 totalHours() const;
        //   Int64 totalDays() const;
        //   Int64 totalMicroseconds() const;
        //   Int64 totalMilliseconds() const;
        //   Int64 totalNanoseconds() const;
        //   double totalSecondsAsDouble() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'total*' ACCESSORS"
                            "\n==================\n");

        // These constants define the seconds and milliseconds attributes of
        // the canonical representations for the maximum and minimum
        // 'TimeInterval' value upon which you can call 'milliseconds' and
        // 'microseconds (respectively).

        // Nanoseconds.

        const Int64 k_MAX_NANOSECS_SECS  = LLONG_MAX / k_NANOSECS_PER_SEC;
        const int   k_MAX_NANOSECS_NANOS = LLONG_MAX % k_NANOSECS_PER_SEC;

        const Int64 k_MIN_NANOSECS_SECS  = LLONG_MIN / k_NANOSECS_PER_SEC;
        const int   k_MIN_NANOSECS_NANOS = LLONG_MIN % k_NANOSECS_PER_SEC;

        // Microseconds.

        const Int64 k_MAX_MICROSECS_SECS  = LLONG_MAX / k_MICROSECS_PER_SEC;
        const int   k_MAX_MICROSECS_NANOS =
           (LLONG_MAX % k_MICROSECS_PER_SEC + 1) * k_NANOSECS_PER_MICROSEC - 1;

        const Int64 k_MIN_MICROSECS_SECS  = LLONG_MIN / k_MICROSECS_PER_SEC;
        const int   k_MIN_MICROSECS_NANOS =
           (LLONG_MIN % k_MICROSECS_PER_SEC - 1) * k_NANOSECS_PER_MICROSEC + 1;

        // Milliseconds.

        const Int64 k_MAX_MILLISECS_SECS  = LLONG_MAX / k_MILLISECS_PER_SEC;
        const int   k_MAX_MILLISECS_NANOS =
           (LLONG_MAX % k_MILLISECS_PER_SEC + 1) * k_NANOSECS_PER_MILLISEC - 1;

        const Int64 k_MIN_MILLISECS_SECS  = LLONG_MIN / k_MILLISECS_PER_SEC;
        const int   k_MIN_MILLISECS_NANOS =
           (LLONG_MIN % k_MILLISECS_PER_SEC - 1) * k_NANOSECS_PER_MILLISEC + 1;

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,           513454,     2131241 },
            { L_,          -123123,      982173 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
           "\nTest secs, mins, hours, days, totalSecondsAsDouble w/ table.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
            const int                NSECS   = DATA[ti].d_nsecs;


            const Obj X(SECONDS, NSECS);

            if (veryVerbose) { T_ P_(LINE) P(X) }

            ASSERTV(LINE, X, X.totalDays(),
                    X.seconds() / k_SECS_PER_DAY == X.totalDays());

            ASSERTV(LINE, X, X.totalHours(),
                    X.seconds() / k_SECS_PER_HOUR == X.totalHours());

            ASSERTV(LINE, X, X.totalMinutes(),
                    X.seconds() / k_SECS_PER_MIN == X.totalMinutes());

            ASSERTV(LINE, X, X.totalSeconds(),
                    X.seconds() +
                    X.nanoseconds() / k_NANOSECS_PER_SEC == X.totalSeconds());

            // The intermediate, volatile values are required to ensure that
            // the assertion compares two variables of type 'double', not one
            // variable and one floating point expression.  A floating-point
            // expression is allowed to have more precision than a variable of
            // type 'double'.

            volatile double EXP =
                static_cast<double>(X.seconds()) +
                static_cast<double>(X.nanoseconds()) / k_NANOSECS_PER_SEC;

            volatile double RESULT = X.totalSecondsAsDouble();

            ASSERTV(LINE, X, EXP, RESULT, EXP == RESULT);
        }

        if (verbose) printf("\nTest milliseconds with test table.\n");

        {
            static const struct {
                    int                d_lineNum;
                    bsls::Types::Int64 d_seconds;
                    int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,  k_MAX_MILLISECS_SECS,   k_MAX_MILLISECS_NANOS },
                { L_,  k_MIN_MILLISECS_SECS,   k_MIN_MILLISECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP = X.seconds()     * k_MILLISECS_PER_SEC +
                                  X.nanoseconds() / k_NANOSECS_PER_MILLISEC;

                ASSERTV(LINE, X, EXP, X.totalMilliseconds(),
                        EXP == X.totalMilliseconds());
            }
        }

        if (verbose) printf("\nTest microseconds with test table.\n");

        {

            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_seconds;
                int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,  k_MAX_MICROSECS_SECS,   k_MAX_MICROSECS_NANOS },
                { L_,  k_MIN_MICROSECS_SECS,   k_MIN_MICROSECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP = X.seconds()     * k_MICROSECS_PER_SEC +
                                  X.nanoseconds() / k_NANOSECS_PER_MICROSEC;

                ASSERTV(LINE, X, EXP, X.totalMicroseconds(),
                        EXP == X.totalMicroseconds());
            }
        }

        if (verbose) printf("\nTest nanoseconds with test table.\n");

        {

            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_seconds;
                int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,   k_MAX_NANOSECS_SECS,    k_MAX_NANOSECS_NANOS },
                { L_,   k_MIN_NANOSECS_SECS,    k_MIN_NANOSECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP =
                    X.seconds() * k_NANOSECS_PER_SEC + X.nanoseconds();

                ASSERTV(LINE, X, EXP, X.totalNanoseconds(),
                        EXP == X.totalNanoseconds());
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'totalMilliseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_MILLISECS_SECS,
                                      k_MAX_MILLISECS_NANOS);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_MILLISECS_SECS,
                                      k_MIN_MILLISECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.setIntervalRaw(LLONG_MIN, -999999999);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                }
            }

            if (veryVerbose) printf("\t'totalMicroseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_MICROSECS_SECS,
                                      k_MAX_MICROSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_MICROSECS_SECS,
                                      k_MIN_MICROSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.setIntervalRaw(LLONG_MIN, 0);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                }
            }
            if (veryVerbose) printf("\t'totalNanoseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_NANOSECS_SECS,
                                      k_MAX_NANOSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_NANOSECS_SECS,
                                      k_MIN_NANOSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.setIntervalRaw(LLONG_MIN, 0);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'setTotal*' MANIPULATORS
        //   Ensure that each method correctly computes the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each manipulator can set an object to have any "total" value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 Each manipulator is not affected by the state of the object on
        //:   entry.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'setTotalDays', use the table-driven technique to specify
        //:   a set of distinct object values (one per row) in terms of their
        //:   total-days representation.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create an object, 'W', having the value from 'R1' interpreted
        //:     as total milliseconds.  (This expedient reuse of 'R1' is for
        //:     giving 'W' a unique value in each iteration of the loop.)
        //:
        //:   2 For each row 'R2' in the table of P-1:  (C-1..2)
        //:
        //:     1 Use the copy constructor to create an object, 'X', from 'W'.
        //:
        //:     2 Use 'setTotalDays' to set 'X' to have the total number of
        //:       days from 'R2'.
        //:
        //:     3 Create an object 'EXP' using the previously tested
        //:       'setIntervalRaw' function.
        //:
        //:     4 Verify 'EXP' equals 'X'.
        //:
        //: 3 Repeat steps similar to those described in P-1..2 to test
        //:   'setTotalHours' ('setTotalMinutes', 'setTotalSeconds',
        //:   'setTotalMilliseconds', 'setTotalMicroseconds',
        //:   'setTotalNanoseconds') except that, this time (a) the rows of
        //:   the table (P-1) are in terms of total hours (total minutes,
        //:   total seconds, total milliseconds, total microseconds, total
        //:   nanoseconds) instead of total days, and (b) 'setTotalHours'
        //:   ('setTotalMinutes', 'setTotalSeconds', 'setTotalMilliseconds',
        //:   'setTotalMicroseconds', 'setTotalNanoseconds') is applied
        //:   instead of 'setTotalDays'.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   void setTotalDays(int);
        //   void setTotalHours(Int64);
        //   void setTotalMinutes(Int64);
        //   void setTotalSeconds(Int64);
        //   void setTotalMilliseconds(Int64);
        //   void setTotalMicroseconds(Int64);
        //   void setTotalNanoseconds(Int64);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'setTotal*' MANIPULATORS"
                            "\n========================\n");

        if (verbose) printf("\nTesting 'setTotalDays'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalDays;
            } DATA[] = {
                //LINE   TOTAL DAYS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_DAYS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_DAYS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_DAYS = DATA[ti].d_totalDays;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_DAYS * 86400, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_DAYS = DATA[tj].d_totalDays;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalDays(JTOTAL_DAYS);

                    Obj exp(JTOTAL_DAYS * 86400, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_DAYS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_DAYS, X, JTOTAL_DAYS == X.totalDays());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalHours'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalHours;
            } DATA[] = {
                //LINE   TOTAL HOURS
                //----   -----------
                { L_,              0 },

                { L_,              1 },
                { L_,          13027 },
                { L_,    k_HOURS_MAX },

                { L_,             -1 },
                { L_,         -42058 },
                { L_,    k_HOURS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE        = DATA[ti].d_line;
                const Int64 ITOTAL_HOURS = DATA[ti].d_totalHours;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_HOURS * 3600, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE        = DATA[tj].d_line;
                    const Int64 JTOTAL_HOURS = DATA[tj].d_totalHours;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalHours(JTOTAL_HOURS);

                    Obj exp(JTOTAL_HOURS * 3600, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_HOURS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_HOURS, X, JTOTAL_HOURS == X.totalHours());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMinutes'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMins;
            } DATA[] = {
                //LINE   TOTAL MINS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_MINS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_MINS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_MINS = DATA[ti].d_totalMins;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MINS * 60, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_MINS = DATA[tj].d_totalMins;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMinutes(JTOTAL_MINS);

                    Obj exp(JTOTAL_MINS * 60, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_MINS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MINS, X, JTOTAL_MINS == X.totalMinutes());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalSeconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalSecs;
            } DATA[] = {
                //LINE   TOTAL SECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_SECS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_SECS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_SECS = DATA[ti].d_totalSecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_SECS, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_SECS = DATA[tj].d_totalSecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalSeconds(JTOTAL_SECS);

                    Obj exp(JTOTAL_SECS, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_SECS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);


                    ASSERTV(JTOTAL_SECS, X, JTOTAL_SECS == X.totalSeconds());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMilliseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMillisecs;
            } DATA[] = {
                //LINE   TOTAL MILLISECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_MILLISECS = DATA[ti].d_totalMillisecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MILLISECS / k_MILLISECS_PER_SEC,
                               static_cast<int>(
                                (ITOTAL_MILLISECS % k_MILLISECS_PER_SEC)
                                                  * k_NANOSECS_PER_MILLISEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_MILLISECS = DATA[tj].d_totalMillisecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMilliseconds(JTOTAL_MILLISECS);

                    Obj exp(JTOTAL_MILLISECS / k_MILLISECS_PER_SEC,
                            static_cast<int>(
                                (JTOTAL_MILLISECS % k_MILLISECS_PER_SEC)
                                                  * k_NANOSECS_PER_MILLISEC));
                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_MILLISECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MILLISECS, X,
                            JTOTAL_MILLISECS == X.totalMilliseconds());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMicroseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMicrosecs;
            } DATA[] = {
                //LINE   TOTAL MICROSECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_MICROSECS = DATA[ti].d_totalMicrosecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MICROSECS / k_MICROSECS_PER_SEC,
                               static_cast<int>(
                                (ITOTAL_MICROSECS % k_MICROSECS_PER_SEC)
                                                  * k_NANOSECS_PER_MICROSEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_MICROSECS = DATA[tj].d_totalMicrosecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMicroseconds(JTOTAL_MICROSECS);

                    Obj exp(JTOTAL_MICROSECS / k_MICROSECS_PER_SEC,
                            static_cast<int>(
                                (JTOTAL_MICROSECS % k_MICROSECS_PER_SEC)
                                                  * k_NANOSECS_PER_MICROSEC));
                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_MICROSECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MICROSECS, X,
                            JTOTAL_MICROSECS == X.totalMicroseconds());

                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalNanoseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalNanosecs;
            } DATA[] = {
                //LINE   TOTAL NANOSECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_NANOSECS = DATA[ti].d_totalNanosecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_NANOSECS / k_NANOSECS_PER_SEC,
                               static_cast<int>(
                               ITOTAL_NANOSECS % k_NANOSECS_PER_SEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_NANOSECS = DATA[tj].d_totalNanosecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalNanoseconds(JTOTAL_NANOSECS);

                    Obj exp(JTOTAL_NANOSECS / k_NANOSECS_PER_SEC,
                            static_cast<int>(
                            JTOTAL_NANOSECS % k_NANOSECS_PER_SEC));

                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_NANOSECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'setTotalDays'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalDays(k_DAYS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalDays(k_DAYS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalDays(k_DAYS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalDays(k_DAYS_MAX + 1));
            }

            if (veryVerbose) printf("\t'setTotalHours'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MAX + 1));
            }

            if (veryVerbose) printf("\t'setTotalMinutes'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MAX + 1));
            }

            // 'setTotalSeconds', 'setTotalMilliseconds',
            // 'setTotalMicroseconds' and 'setTotalNanoseconds' do not have
            // undefined behavior.
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)
        //   Ensure that each operator defines the correct relationship between
        //   the underlying canonical representations of its operands.
        //
        // Concerns:
        //: 1 An object 'X' is in relation to an object 'Y' as the
        //:   canonical representation of 'X' is in relation to the
        //:   canonical representation of 'Y'.
        //:
        //: 2 'false == (X <  X)' (i.e., irreflexivity).
        //:
        //: 3 'true  == (X <= X)' (i.e., reflexivity).
        //:
        //: 4 'false == (X >  X)' (i.e., irreflexivity).
        //:
        //: 5 'true  == (X >= X)' (i.e., reflexivity).
        //:
        //: 6 If 'X < Y', then '!(Y < X)' (i.e., asymmetry).
        //:
        //: 7 'X <= Y' if and only if 'X < Y' exclusive-or 'X == Y'.
        //:
        //: 8 If 'X > Y', then '!(Y > X)' (i.e., asymmetry).
        //:
        //: 9 'X >= Y' if and only if 'X > Y' exclusive-or 'X == Y'.
        //:
        //:10 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., all relational-comparison operators are free functions).
        //:
        //:11 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:12 The relational-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator<', 'operator<=',
        //:   'operator>', and 'operator>=' to initialize function pointers
        //:   having the appropriate signatures and return types for the 12
        //:   relational-comparison operators defined in this component.
        //:   (C-10..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their second and
        //:   milliseconds.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..9)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create an object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the anti-reflexive (reflexive) property of
        //:     '<' and '>' ('<=' and '>=') in the presence of aliasing.
        //:     (C-2..5)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 6..9)
        //:
        //:     1 Use the default constructor and 'setIntervalRaw' to create an
        //:       object, 'X', having the value from 'R1'.
        //:
        //:     2 Use the default constructor and 'setIntervalRaw' to create a
        //:       second object, 'Y', having the value from 'R2'.
        //:
        //:     3 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '<'.  (C-6)
        //:
        //:     5 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:     6 Using 'X' and 'Y', verify the expected return value for '<='.
        //:       (C-7)
        //:
        //:     7 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     8 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '>'.  (C-8)
        //:
        //:     9 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:    10 Using 'X' and 'Y', verify the expected return value for '>='.
        //:       (C-1, 9)
        //
        // Testing:
        //   bool operator< (const TimeInterval&, const TimeInterval&);
        //   bool operator< (const TimeInterval&, double);
        //   bool operator< (double, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, double);
        //   bool operator<=(double, const TimeInterval&);
        //   bool operator> (const TimeInterval&, const TimeInterval&);
        //   bool operator> (const TimeInterval&, double);
        //   bool operator> (double, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, double);
        //   bool operator>=(double, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nRELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)"
                         "\n==============================================\n");

        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef bool (*operatorPtr)(const Obj&, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
            {
                typedef bool (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
            {
                typedef bool (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
        }
        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);


        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;


            if (veryVerbose) { T_ P_(ILINE) P_(ISECONDS) P(INSECS) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ISECONDS, INSECS);

                ASSERTV(ILINE, W, !(W <  W));
                ASSERTV(ILINE, W,   W <= W);
                ASSERTV(ILINE, W, !(W >  W));
                ASSERTV(ILINE, W,   W >= W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                double dIv = static_cast<double>(ISECONDS) +
                                 static_cast<double>(INSECS) /
                                 static_cast<double>(k_NANOSECS_PER_SEC);
                double dJv = static_cast<double>(JSECONDS) +
                                 static_cast<double>(JNSECS) /
                                 static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_ P_(JLINE) P_(JSECONDS) P(JNSECS) }

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(ISECONDS, INSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ P_(X) P(Y) }

                // Verify 'operator<'.

                {
                    const bool EXP = dIv < dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   < Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   < dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv < Y));
                    }

                    if (EXP) {
                        ASSERTV(ILINE, JLINE, Y, X, !(Y < X));
                    }
                }

                // Verify 'operator<='.

                {
                    const bool EXP = dIv <= dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   <= Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   <= dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv <= Y));
                    }
                    ASSERTV(ILINE, JLINE, X, Y,
                                 EXP == ((X < Y) ^ (X == Y)));

                }

                // Verify 'operator>'.

                {
                    const bool EXP = dIv > dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   > Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   > dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv > Y));
                    }

                    if (EXP) {
                        ASSERTV(ILINE, JLINE, Y, X, !(Y > X));
                    }

                }

                // Verify 'operator>='.

                {
                    const bool EXP = dIv >= dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   >= Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   >= dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv >= Y));
                    }

                    ASSERTV(ILINE, JLINE, X, Y,
                                 EXP == ((X > Y) ^ (X == Y)));

                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // COMPARISONS WITH DOUBLE (==, !=)
        //   Ensure that '==' and '!=' correctly compare values with 'double'.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y' (where one "object" is a 'double'
        //:   value), compare equal if and only if their corresponding
        //:   canonical representations compare equal.
        //:
        //: 2 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator<', 'operator<=',
        //:   'operator>', and 'operator>=' to initialize function pointers
        //:   having the appropriate signatures and return types for the 12
        //:   relational-comparison operators defined in this component.
        //:   (C-2)
        //:
        //: 2 Using a table-driven technique, create objects 'X' and 'Y' for a
        //:   cross-product of test values using the explicit conversion from
        //:   double (previously tested).  Verify the comparisons with double
        //:   return true if, and only-if, the original test value is the same
        //:   (for all the comparison overloads).  (C-1..3)
        //:
        // Testing:
        //   bool operator==(double, const TimeInterval&);
        //   bool operator==(const TimeInterval&, double);
        //   bool operator!=(double, const TimeInterval&);
        //   bool operator!=(const TimeInterval&, double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPARISONS WITH DOUBLE (==, !=)"
                            "\n================================\n");


        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef bool (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bsls::operator==;
                operatorPtr operatorNe = bsls::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }
            {
                typedef bool (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bsls::operator==;
                operatorPtr operatorNe = bsls::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            if (veryVerbose) { T_ P_(ILINE) P_(ISECONDS) P(INSECS) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                double dIv = static_cast<double>(ISECONDS) +
                             static_cast<double>(INSECS) /
                             static_cast<double>(k_NANOSECS_PER_SEC);
                double dJv = static_cast<double>(JSECONDS) +
                             static_cast<double>(JNSECS) /
                             static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_ P_(JLINE) P_(JSECONDS) P(JNSECS) }

                Obj mX(dIv);  const Obj& X = mX;
                Obj mY(dJv);  const Obj& Y = mY;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }

                // Verify 'operator=='.

                bool EXP = ti == tj;

                ASSERTV(ILINE, JLINE, X, Y, EXP == (X   == Y));
                ASSERTV(ILINE, JLINE, X, Y, EXP == (X   == dJv));
                ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv == Y));

                ASSERTV(ILINE, JLINE, X, Y, EXP != (X   != Y));
                ASSERTV(ILINE, JLINE, X, Y, EXP != (X   != dJv));
                ASSERTV(ILINE, JLINE, X, Y, EXP != (dIv != Y));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONVERT AND ASSIGN FROM 'double'
        //   Ensure that we can put an object into any valid initial state.
        //
        // Concerns:
        //: 1 The 'double' conversion can create an object having any value
        //:   that does not violate the constructor's documented preconditions.
        //:
        //: 2 The 'double' assignment can set an object to any value
        //:   that does not violate the assignment operator's documented
        //:   preconditions.
        //:
        //: 3 The 'double' assignment returns a reference providing modifiable
        //:   access to the assigned object.
        //
        // Plan:
        //: 1 Using the table-driven technique, for a set of test values,
        //:   construct a test object from a 'double' and verify its
        //:   properties. (C-1)
        //:
        //: 2 Using the table-driven technique, for a set of test values,
        //:   assign a test object using a 'double' and verify its
        //:   properties. (C-2..3)
        //
        // Testing:
        //   TimeInterval(double);
        //   TimeInterval& operator=(double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERT AND ASSIGN FROM 'double'"
                            "\n========================================\n");

        static const struct {
            int    d_lineNum;
            double d_secs;
            int    e_secs;
            int    e_nsecs;
        } DATA[] = {
            //line     double      secs     nanosecs
            //----     ------      ----     --------
            { L_,            0.0,   0,            0 },
            { L_,            1.0,   1,            0 },
            { L_,   1.9999999994,   1,    999999999 },
            { L_,  -1.9999999994,  -1,   -999999999 },
            { L_,   2.9999999995,   3,            0 },
            { L_,  -2.9999999995,  -3,            0 },
            { L_,   3.0000000004,   3,            0 },
            { L_,  -3.0000000004,  -3,            0 },
            { L_,   4.0000000005,   4,            1 },
            { L_,  -4.0000000005,  -4,           -1 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nTest conversion from 'double'.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE  = DATA[i].d_lineNum;
            const double DSECS = DATA[i].d_secs;
            const int    SECS  = DATA[i].e_secs;
            const int    NSECS = DATA[i].e_nsecs;

            // Test conversion from 'double'.
            {
                Obj mX(DSECS);  const Obj& X = mX;
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTest assignment from 'double'.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE  = DATA[i].d_lineNum;
            const double DSECS = DATA[i].d_secs;
            const int    SECS  = DATA[i].e_secs;
            const int    NSECS = DATA[i].e_nsecs;

            // Test assignment  from 'double'.
            {
                Obj mX; const Obj& X = mX;

                Obj *mR = &(mX = DSECS);

                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }

                ASSERTV(LINE, &X    == mR);
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: 'TimeInterval(Int64, int)'
        //   Ensure that we can put an object into any valid initial state.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The value constructor accepts time intervals that are specified
        //:   using a mix of positive, negative, and zero values for seconds
        //:   and nanoseconds fields.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of distinct object values (one per row) in terms
        //:     of their 'second' and 'nanosecond' values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the value constructor to create an object, 'X', from the
        //:     value in 'R'.
        //:
        //:   2 Construct a expected value 'EXPECTED' using the primary
        //:     manipulators.
        //:
        //:   3 Verify 'X' equals 'EXPECTED', and verify the attributes of 'X'
        //:     using its primary manipulators.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   TimeInterval(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'TimeInterval(Int64, int)'"
                            "\n===================================\n");

        static const struct {
            int                d_lineNum;  // Source line number
            bsls::Types::Int64 d_secs;     // Input seconds
            int                d_nsecs;    // Input nanoseconds
            bsls::Types::Int64 d_expSecs;  // Expected seconds
            int                d_expNsecs; // Expected nanoseconds
        } DATA[] = {
     //line         secs      nanosecs      expected secs   expected nanosecs
     //----         ----      --------      -------------   -----------------
      { L_,            0,             0,                0,                0 },
      { L_,            1,             2,                1,                2 },
      { L_,            1,    1000000000,                2,                0 },
      { L_,           -1,   -1000000000,               -2,                0 },
      { L_,            2,   -1000000001,                0,        999999999 },
      { L_,           -2,    1000000001,                0,       -999999999 },
      { L_,   k_SECS_MAX,     999999999,       k_SECS_MAX,        999999999 },
      { L_,   k_SECS_MIN,    -999999999,       k_SECS_MIN,       -999999999 },
            };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nUse a table of distinct object values.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE   = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECS   = DATA[ti].d_secs;
            const int                NSECS  = DATA[ti].d_nsecs;
            const bsls::Types::Int64 ESECS  = DATA[ti].d_expSecs;
            const int                ENSECS = DATA[ti].d_expNsecs;

            Obj mX(SECS, NSECS); const Obj& X = mX;

            if (veryVerbose) { T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            Obj expected; const Obj& EXPECTED = expected;
            expected.setInterval(SECS, NSECS);

            ASSERT(EXPECTED == X);
            ASSERT(ESECS    == X.seconds());
            ASSERT(ENSECS   == X.nanoseconds());
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);


            if (veryVerbose) printf("\tMaximum interval\n");
            {
                ASSERT_SAFE_PASS(Obj(k_SECS_MAX, k_NANOSECS_PER_SEC - 1));
                ASSERT_SAFE_FAIL(Obj(k_SECS_MAX, k_NANOSECS_PER_SEC));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                ASSERT_SAFE_PASS(Obj(k_SECS_MIN, -k_NANOSECS_PER_SEC + 1));
                ASSERT_SAFE_FAIL(Obj(k_SECS_MIN, -k_NANOSECS_PER_SEC));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING: 'setInterval(Int64, int)'
        //
        // Concerns:
        //: 1 The 'setInterval' method can set an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 2 'setInterval' is not affected by the state of the object
        //:   on entry.
        //:
        //: 3 The 'nanoseconds' parameter of 'setInterval' defaults to 0.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 2 For each row 'R1' in the table of P-3:  (C-1..3)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   2 Using the 'setInterval' manipulator, set 'X' to the value
        //:     specified in 'R1'.
        //:
        //:   3 Verify, using 'seconds' and 'nanoseconds', that 'X' has the
        //:     expected value.  (C-1..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   void setInterval(Int64, int);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING: 'setInterval(Int64, int)'"
                            "\n==================================\n");

        if (verbose) printf("\nTesting 'setInterval'.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
                bsls::Types::Int64 d_expSecs;  // Expected seconds
                int                d_expNsecs; // Expected nanoseconds
            } DATA[] = {
     //line         secs      nanosecs      expected secs   expected nanosecs
     //----         ----      --------      -------------   -----------------
      { L_,            0,             0,                0,                0 },
      { L_,            1,             2,                1,                2 },
      { L_,            1,    1000000000,                2,                0 },
      { L_,           -1,   -1000000000,               -2,                0 },
      { L_,            2,   -1000000001,                0,        999999999 },
      { L_,           -2,    1000000001,                0,       -999999999 },
      { L_,   k_SECS_MAX,     999999999,       k_SECS_MAX,        999999999 },
      { L_,   k_SECS_MIN,    -999999999,       k_SECS_MIN,       -999999999 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const int                NSECS  = DATA[i].d_nsecs;
                const bsls::Types::Int64 ESECS  = DATA[i].d_expSecs;
                const int                ENSECS = DATA[i].d_expNsecs;

                Obj mX;  const Obj& X = mX;
                mX.setInterval(SECS, NSECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, ESECS  == X.seconds());
                ASSERTV(LINE, ENSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTesting default 'nanoseconds' value.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
            } DATA[] = {
                //line         secs
                //----         ----
                { L_,            0 },
                { L_,            1 },
                { L_,           -1 },
                { L_,         1000 },
                { L_,        -1000 },
                { L_,   k_SECS_MIN },
                { L_,   k_SECS_MAX },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;

                Obj mX;  const Obj& X = mX;
                mX.setInterval(SECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS);
                }
                ASSERTV(LINE, SECS == X.seconds());
                ASSERTV(LINE, 0    == X.nanoseconds());
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Int64 SMAX = k_SECS_MAX;
            const Int64 SMIN = k_SECS_MIN;

            ASSERT( Obj::isValid(SMAX, k_NANOSECS_PER_SEC - 1));
            ASSERT(!Obj::isValid(SMAX, k_NANOSECS_PER_SEC));
            ASSERT(!Obj::isValid(SMAX, k_NANOSECS_PER_SEC + 1));

            ASSERT( Obj::isValid(SMIN, -k_NANOSECS_PER_SEC + 1));
            ASSERT(!Obj::isValid(SMIN, -k_NANOSECS_PER_SEC));
            ASSERT(!Obj::isValid(SMIN, -k_NANOSECS_PER_SEC - 1));

            if (veryVerbose) printf("\tMaximum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setInterval(SMAX, k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setInterval(SMAX, k_NANOSECS_PER_SEC-1));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setInterval(SMIN, -k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setInterval(SMIN, -k_NANOSECS_PER_SEC+1));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   Verify the BDEX streaming implementation works correctly.
        //   Specific concerns include wire format, handling of stream states
        //   (valid, empty, invalid, incomplete, and corrupted), and exception
        //   neutrality.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods.
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int versionSelector);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEX STREAMING"
                            "\n======================\n");

        // Scalar object values used in various stream tests.
        const Obj VA(  0,  0);
        const Obj VB(  0,  1);
        const Obj VC(  1,  0);
        const Obj VD( 10, 59);
        const Obj VE( 23,  0);
        const Obj VF( 23, 22);
        const Obj VG( 24,  0);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            printf("\nTesting 'maxSupportedBdexVersion'.\n");
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
        }

        const int VERSION = Obj::maxSupportedBdexVersion(0);

        if (verbose) {
            printf("\nDirect initial trial of 'bdexStreamOut' and (valid) "
                   "'bdexStreamIn' functionality.\n");
        }
        {
            const Obj X(VC);
            Out       out(VERSION_SELECTOR);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        if (verbose) {
            printf("\nThorough test.\n");
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR);

                Out& rvOut = X.bdexStreamOut(out, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = mT.bdexStreamIn(in, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) {
            printf("\tOn empty streams and non-empty, invalid streams.\n");
        }

        // Verify correct behavior for empty streams (valid and invalid).

        {
            Out               out(VERSION_SELECTOR);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // Verify correct behavior for non-empty, invalid streams.

        {
            Out               out(VERSION_SELECTOR);

            Out& rvOut = Obj().bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            printf("\tOn incomplete (but otherwise valid) data.\n");
        }
        {
            const Obj W1 = VA, X1 = VB;
            const Obj W2 = VB, X2 = VC;
            const Obj W3 = VC, X3 = VD;

            Out out(VERSION_SELECTOR);

            Out& rvOut1 = X1.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut1);
            const int         LOD1 = out.length();

            Out& rvOut2 = X2.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut2);
            const int         LOD2 = out.length();

            Out& rvOut3 = X3.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut3);
            const int         LOD3 = out.length();
            const char *const OD3  = out.data();

            for (int i = 0; i < LOD3; ++i) {
                In in(OD3, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !i == in.isEmpty());

                    Obj mT1(W1);  const Obj& T1 = mT1;
                    Obj mT2(W2);  const Obj& T2 = mT2;
                    Obj mT3(W3);  const Obj& T3 = mT3;

                    if (i < LOD1) {
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else if (i < LOD2) {
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 <= i) LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else {  // 'LOD2 <= i < LOD3'
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                    }

                    // Verify the objects are in a valid state.

                    LOOP_ASSERT(i, Obj::isValid(T1.seconds(),
                                                T1.nanoseconds()));

                    LOOP_ASSERT(i, Obj::isValid(T2.seconds(),
                                                T2.nanoseconds()));

                    LOOP_ASSERT(i, Obj::isValid(T3.seconds(),
                                                T3.nanoseconds()));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            printf("\tOn corrupted data.\n");
        }

        const Obj W;             // default value
        const Obj X(1, 3);       // original (control)
        const Obj Y(0, 2);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            printf("\t\tGood stream (for control).\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }

        if (verbose) {
            printf("\t\tBad version.\n");
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tSeconds positive, nanoseconds negative.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64( 1);
            out.putInt32(-1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tSeconds negative, nanoseconds positive.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(-1);
            out.putInt32( 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tNanoseconds too small.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(-k_NANOSECS_PER_SEC);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tNanoseconds too large.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(k_NANOSECS_PER_SEC);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\nWire format direct tests.\n");
        }
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_seconds;      // specification seconds
                int         d_nanoseconds;  // specification nanoseconds
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //LINE  SEC   NS   VER  LEN  FORMAT
                //----  ----  ---  ---  ---  -------------------
                { L_,      0,   0,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"  },
                { L_,      0,   1,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"  },
                { L_,      1,   0,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00"  },
                { L_,     20,   8,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x08"  }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         SECONDS     = DATA[i].d_seconds;
                const int         NANOSECONDS = DATA[i].d_nanoseconds;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                {
                    Obj        mX(SECONDS, NANOSECONDS);
                    const Obj& X = mX;

                    Out  out(VERSION_SELECTOR);
                    Out& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            printf("\\x%c%c",
                                   hex[static_cast<unsigned char>
                                           ((*(out.data() + j) >> 4) & 0x0f)],
                                   hex[static_cast<unsigned char>
                                                  (*(out.data() + j) & 0x0f)]);
                        }
                        printf("\n");
                    }

                    Obj mY;  const Obj& Y = mY;

                    In  in(out.data(), out.length());
                    In& rvIn = mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', having the value from
        //:     'R1'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the default constructor and 'setIntervalRaw' to
        //:       create a modifiable 'Obj', 'mX', having the value from 'R2'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-3 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are created to have the value from 'R1'.  For each
        //:   'R1' in the table of P-2:  (C-5)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create a modifiable 'Obj', 'mX', having the value from 'R1';
        //:     also use the default constructor and 'setIntervalRaw' to
        //:     create a 'const' 'Obj', 'ZZ', also having the value from 'R1'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z' ('mX'), still has the same value as that of
        //:     'ZZ'.  (C-5)
        //
        // Testing:
        //   TimeInterval& operator=(const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose) printf(
                 "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setIntervalRaw(ISECONDS, INSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setIntervalRaw(ISECONDS, INSECS);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ILINE, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                ASSERTV(ILINE, JLINE, Z, X,
                             (Z == X) == (ILINE == JLINE));

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, JLINE,  Z,   X,  Z == X);
                ASSERTV(ILINE, JLINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, JLINE, ZZ,   Z, ZZ == Z);
            }

            // self-assignment

            {
                Obj mX;
                mX. setIntervalRaw(ISECONDS, INSECS);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setIntervalRaw(ISECONDS, INSECS);

                const Obj& Z = mX;

                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                            "\n==============================\n");

        if (verbose) printf("Not implemented for 'bsls::TimeInterval'.\n");

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their seconds and
        //:   nanoseconds.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the default constructor to create two objects, 'Z' and
        //:     'ZZ', then set both 'Z' and 'ZZ' to the value from 'R' (using
        //:     'setTotalMilliseconds').
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     with a reference providing non-modifiable access to 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   TimeInterval(const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
            const int                NSECS   = DATA[ti].d_nsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setIntervalRaw(SECONDS, NSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setIntervalRaw(SECONDS, NSECS);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            // Verify the value of the object.

            ASSERTV(LINE, Z,  X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding canonical representations compare equal.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their seconds and
        //:   nanoseconds.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create an object 'W' using the default constructor, then set
        //:     'W' to the value from 'R1' (using 'setTimeInterval').
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Create an object 'X' using the default constructor, then set
        //:       'X' to the value from 'R1' (using 'setTimeInterval').
        //:
        //:     3 Create an object 'Y' using the default constructor, then set
        //:       'Y' to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const TimeInterval&, const TimeInterval&);
        //   bool operator!=(const TimeInterval&, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bsls::operator==;
            operatorPtr operatorNe = bsls::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            if (veryVerbose) { T_; P_(ILINE); P_(ISECONDS); P(INSECS); }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ISECONDS, INSECS);

                ASSERTV(ILINE, W,   W == W);
                ASSERTV(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(ILINE, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                if (veryVerbose) { T_; P_(JLINE); P_(JSECONDS); P(JNSECS); }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(ISECONDS, INSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against canonical comparison.

                ASSERTV(ILINE, JLINE, X, Y,
                             EXP == ((X.seconds()     == Y.seconds() &&
                                     (X.nanoseconds() == Y.nanoseconds()))));

               ASSERTV(ILINE, JLINE, X, Y,
                            !EXP == ((X.seconds()     != Y.seconds() ||
                                     (X.nanoseconds() != Y.nanoseconds()))));

                // Verify value and commutativity.

                ASSERTV(ILINE, JLINE, X, Y,  EXP == (X == Y));
                ASSERTV(ILINE, JLINE, Y, X,  EXP == (Y == X));

                ASSERTV(ILINE, JLINE, X, Y, !EXP == (X != Y));
                ASSERTV(ILINE, JLINE, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding time interval field, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel'):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //:
        //: 3 Using the table-driven technique, further corroborate that the
        //:   'print' method and 'operator<<' format object values correctly by
        //:   testing an additional set of time intervals (including extremal
        //:   values), but this time fixing the 'level' and 'spacesPerLevel'
        //:   arguments to 0 and -1, respectively.
        //
        // Testing:
        //   ostream& print(ostream&, int, int) const;
        //   ostream& operator<<(ostream&, const TimeInterval&);
        // --------------------------------------------------------------------


        if (verbose) printf("\nPRINT AND OUTPUT OPERATOR (<<)"
                            "\n==============================\n");

        if (verbose) printf("\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables.\n");
        {

            typedef native_std::ostream& (Obj::*funcPtr)(
                                         native_std::ostream&, int, int) const;
            typedef native_std::ostream& (*operatorPtr)(
                                             native_std::ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = bsls::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) printf(
             "\nCreate a table of distinct value/format combinations.\n");

        static const struct {
            int         d_line;
            int         d_level;
            int         d_spacesPerLevel;
            Int64       d_secs;
            int         d_nsecs;
            const char *d_expected;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---

        { L_,  0,  0,      0,      0,  "(0, 0)"              NL },
        { L_,  0,  1,      0,      0,  "(0, 0)"              NL },
        { L_,  0, -1,      0,      0,  "(0, 0)"                 },
        { L_,  0,  4,      0,      0,  "(0, 0)"              NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  3,  0,      0,      0,  "(0, 0)"              NL },
        { L_,  3,  2,      0,      0,  "      (0, 0)"        NL },
        { L_,  3, -2,      0,      0,  "      (0, 0)"           },
        { L_,  3,  4,      0,      0,  "            (0, 0)"  NL },
        { L_, -3,  0,      0,      0,  "(0, 0)"              NL },
        { L_, -3,  2,      0,      0,  "(0, 0)"              NL },
        { L_, -3, -2,      0,      0,  "(0, 0)"                 },
        { L_, -3,  4,      0,      0,  "(0, 0)"              NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  2,  3,   -123,  -5000,  "      (-123, -5000)" NL },


        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  0,  4,      0,      0,  "(0, 0)"              NL },
        { L_,  0,  4,   -123,  -5000,  "(-123, -5000)"       NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  0, -1,      0,      0,  "(0, 0)"                 },
        { L_,  0, -1,   -123,  -5000,  "(-123, -5000)"          },

#undef NL

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nTesting with various print specifications.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const Int64       SECS  = DATA[ti].d_secs;
                const int         NSECS = DATA[ti].d_nsecs;
                const char *const EXP   = DATA[ti].d_expected;

                typedef native_std::ostringstream       OSStream;
                typedef native_std::ostream             OStream;

                Obj mX(SECS, NSECS); const Obj& X = mX;

                if (veryVerbose) {
                    T_; P_(LINE); P_(L); P(SPL); P_(SECS); P_(NSECS); P(EXP);
                }

                // Test with no default arguments.

                {
                    OSStream out;
                    OStream *mR = &X.print(out, L, SPL);

                    const native_std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));

                }

                // Test with default 'spacesPerLevel'.


                if (4 == SPL) {
                    OSStream out;
                    OStream *mR = &X.print(out, L);

                    const native_std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));

                }

                // Test with default 'level' and 'spacesPerLevel'.


                if (0 == L && 4 == SPL) {
                    OSStream out;
                    OStream *mR = &X.print(out);

                    const native_std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));
                }
            }
        }

        if (verbose) printf("\nTesting 'operator<<' (ostream).\n");
        {
            static const struct {
                int                 d_lineNum;  // source line number
                bsls::Types::Int64  d_seconds;  // second field value
                int                 d_nsecs;    // nanosecond field value
                const char         *d_expected;    // expected output format
            } DATA[] = {
                //line     secs        nsecs        output format
                //---- ------------   ---------   ---------------------------
                { L_,             0,          0, "(0, 0)"                    },
                { L_,             0,        100, "(0, 100)"                  },
                { L_,             0,       -100, "(0, -100)"                 },
                { L_,          -123,      -5000, "(-123, -5000)"             },
                { L_,  3000000000LL,  999999999, "(3000000000, 999999999)"   },
                { L_, -3000000000LL, -999999999, "(-3000000000, -999999999)" }
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int                LINE     = DATA[di].d_lineNum;
                const bsls::Types::Int64 SECONDS  = DATA[di].d_seconds;
                const int                NSECS    = DATA[di].d_nsecs;
                const char *const        EXPECTED = DATA[di].d_expected;

                typedef native_std::ostringstream       OSStream;
                typedef native_std::ostream             OStream;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECONDS, NSECS);

                OSStream out;

                OStream *mR = &(out << X);

                const native_std::string RESULT_STRING = out.str();
                const char *RESULT = RESULT_STRING.c_str();

                if (veryVerbose) { T_; P_(EXPECTED); P(RESULT); }

                ASSERTV(LINE, mR               == &out);
                ASSERTV(LINE, strlen(EXPECTED) == strlen(RESULT));
                ASSERTV(LINE, 0                == strcmp(EXPECTED, RESULT));

                // Compare with 'print(stream, 0, -1)
                {
                    OSStream printResult;
                    const native_std::string PRINT_RESULT_STRING = out.str();
                    const char *PRINT_RESULT = PRINT_RESULT_STRING.c_str();

                    X.print(printResult, 0, -1);

                    ASSERTV(LINE, PRINT_RESULT, RESULT,
                                            0 == strcmp(PRINT_RESULT, RESULT));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 The 'seconds' and 'nanoseconds' accessors return the value of
        //:   the corresponding field of the time interval.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their five-field
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   3 Using 'setIntervalRaw', set 'X' to the value computed in
        //:     P-2.1.
        //:
        //:   4 Verify that each basic accessor, invoked on a reference
        //:     providing non-modifiable access to the object created in P-2.2,
        //:     returns the expected value.  (C-1..3)
        //
        // Testing:
        //   bsls::Types::Int64 seconds() const;
        //   int nanoseconds() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf(
                "\nVerify all basic accessors report expected values.\n");

        {
            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_secs;
                int                d_nsecs;
            } DATA[] = {
                //line             secs      nanosecs
                //----       ----------    ---------
                { L_,                 0,           0 },
                { L_,                 0,           1 },
                { L_,                 1,           1 },
                { L_,                -1,          -1 },
                { L_,        k_SECS_MAX,   999999999 },
                { L_,        k_SECS_MIN,  -999999999 }
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const bsls::Types::Int64 SECS = DATA[i].d_secs;
                const int LINE  = DATA[i].d_lineNum;
                const int NSECS = DATA[i].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS, NSECS);

                if (veryVerbose) {
                    T_; P_(i);    P(X);
                    T_; P_(SECS); P(NSECS);
                }

                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST-DRIVER MACHINERY
        //   Test the test-driver machinery used in this test-driver
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   TESTING TEST-DRIVER MACHINERY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST-DRIVER MACHINERY"
                            "\n=============================\n");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 An object can be safely destroyed.
        //:
        //: 3 The 'setIntervalRaw' method can set an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 4 'setIntervalRaw' is not affected by the state of the object
        //:   on entry.
        //:
        //: 5 The 'nanoseconds' parameter of 'setIntervalRaw' defaults to 0.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor.  Verify, using
        //:   the (as yet unproven) 'seconds' and 'nanoseconds' accessors,
        //:   that the resulting object has a time interval of 0.  (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope.  (C-2)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-3..5)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   2 Using the 'setIntervalRaw' (primary) manipulator, set 'X'
        //:     to the value specified in 'R1'.
        //:
        //:   3 Verify, using 'seconds' and 'nanoseconds'', that 'X' has the
        //:     expected value.  (C-3..5)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval();
        //   ~TimeInterval();
        //   void setIntervalRaw(Int64, int);
        // --------------------------------------------------------------------
        if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                            "\n==========================================\n");

        if (verbose) printf("\nTesting default constructor.\n");

        {
            Obj mX;  const Obj& X = mX;

            if (veryVerbose) P(X);

            ASSERT(0 == X.seconds());
            ASSERT(0 == X.nanoseconds());
        }

        if (verbose) printf("\nTesting 'setIntervalRaw'.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
            } DATA[] = {
                //line         secs      nanosecs
                //----         ----      --------
                { L_,            0,                       0 },
                { L_,            0,  k_NANOSECS_PER_SEC - 1 },
                { L_,            0, -k_NANOSECS_PER_SEC + 1 },
                { L_,         9999,                    9999 },
                { L_,        -9999,                   -9999 },
                { L_,   k_SECS_MAX,                       0 },
                { L_,   k_SECS_MIN,                       0 },
                { L_,   k_SECS_MAX,  k_NANOSECS_PER_SEC - 1 },
                { L_,   k_SECS_MIN, -k_NANOSECS_PER_SEC + 1 },


            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const int                NSECS  = DATA[i].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS, NSECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTesting default 'nanoseconds' value.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
            } DATA[] = {
                //line         secs
                //----         ----
                { L_,            0 },
                { L_,            1 },
                { L_,           -1 },
                { L_,         1000 },
                { L_,        -1000 },
                { L_,   k_SECS_MIN },
                { L_,   k_SECS_MAX },

            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS);
                }
                ASSERTV(LINE, SECS == X.seconds());
                ASSERTV(LINE, 0    == X.nanoseconds());
            }
        }


        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\tMaximum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(0, k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(0, k_NANOSECS_PER_SEC-1));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(0, -k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(0, -k_NANOSECS_PER_SEC+1));
            }

            if (veryVerbose) printf("\tMatching sign values\n");
            {
                Obj mX;
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 1,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(-1,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0,  1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0, -1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 1,  1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(-1, -1));

                ASSERT_SAFE_FAIL(mX.setIntervalRaw( 1, -1));
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(-1,  1));
            }

        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int SA = 1, NA = 2;  // VA fields
        const int SB = 3, NB = 4;  // VB fields
        const int SC = 5, NC = 6;  // VC fields

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 1. Create a default object x1."
                            "\t\t\t{ x1:0 }\n");
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check initial state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 2. Create an object x2 (copy from x1)."
                            "\t\t{ x1:0  x2:0 }\n");
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check the initial state of x2.\n");
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 3. Set x1 to a new value VA."
                            "\t\t\t{ x1:VA x2:0 }\n");
        mX1.setInterval(SA, NA);
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(SA == X1.seconds());
        ASSERT(NA == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 4. Set x2 to a new value VA."
                            "\t\t\t{ x1:VA x2:VA }\n");
        mX2.addSeconds(SA);     // Use different manipulators to get to the
        mX2.addNanoseconds(NA); // same place...
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check initial state of x2.\n");
        ASSERT(SA == X2.seconds());
        ASSERT(NA == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 5. Set x2 to VB."
                            "\t\t\t\t{ x1:VA x2:VB }\n");
        mX2.setInterval(SB, NB);
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(SB == X2.seconds());
        ASSERT(NB == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 6. Set x1 to 0."
                            "\t\t\t\t{ x1:0  x2:VB }\n");
        mX1.setInterval(0, 0);
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 7. Create an object x3 with value VC."
                            "\t\t{ x1:0  x2:VB x3:VC }\n");

        Obj mX3(SC, NC);  const Obj& X3 = mX3;
        if (verbose) { T_;  P(X3); }

        if (verbose) printf("\ta. Check new state of x3.\n");
        ASSERT(SC == X3.seconds());
        ASSERT(NC == X3.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x3 <op> x1, x2, x3.\n");
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 8. Create an object x4 (copy from x1)."
                             "\t\t{ x1:0  x2:VB x3:VC x4:0 }\n");

        Obj mX4(X1);  const Obj& X4 = mX4;
        if (verbose) { T_;  P(X4); }

        if (verbose) printf("\ta. Check initial state of x4.\n");
        ASSERT(0 == X4.seconds());
        ASSERT(0 == X4.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 9. Assign x2 = x1."
                            "\t\t\t\t{ x1:0  x2:0  x3:VC x4:0 }\n");

        mX2 = X1;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 10. Assign x2 = x3."
                            "\t\t\t\t{ x1:0  x2:VC x3:VC x4:0 }\n");

        mX2 = X3;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(SC == X2.seconds());
        ASSERT(NC == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 11. Assign x1 = x1 (aliasing)."
                            "\t\t\t{ x1:0  x2:VC x3:VC x4:0 }\n");

        mX1 = X1;
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
