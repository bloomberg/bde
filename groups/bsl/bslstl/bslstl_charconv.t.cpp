// bslstl_charconv.t.cpp                                              -*-C++-*-
#include <bslstl_charconv.h>

#include <bslstl_ostringstream.h>
#include <bslstl_stringref.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <algorithm>
#include <streambuf>
#include <string>

#include <cstdlib>
#include <cstring>
#include <limits>

#include <limits.h>
#include <stdio.h>

#if defined(DELETE)
# undef DELETE  // Awkward microsoft macro, that we would rather live without
#endif

using namespace BloombergLP;
using std::numeric_limits;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// Normally, a BDE component provides many functions, that are individually
// fairly simple, individual test cases in test drivers test one, or a few,
// functions per test case, and test those functions thoroughly.  This
// component, however, provides only one function, and that function is a
// template function with a potentially wide range of inputs, so ALL the test
// cases here test that one function, and test it in different ways.
//
// ----------------------------------------------------------------------------
// We have two "oracle" functions defined in this test driver:
//
//                       // u::parseAndTestSignedInteger
//
// We have 'parseAndTestUnsignedInteger' that, passed a 'stringRef', an
// 'Int64', and a 'base', will parse the string ref in 'base' to a number and
// compare it with the number passed.  The function will return 'true' if the
// string ref parses properly to a number that matches the specified number and
// 'false' if they don't match or if the string ref is not a valid description
// of a number.  Note that no white space is tolerated in the string ref.  This
// function was copied from 'bdlb_numericparseutil' with considerable
// modification.
//
//                       // u::parseAndTestUnsignedInteger
//
// We have 'parseAndTestUnsignedInteger' that is just like
// 'parseAndTestSignedInteger' except that it takes a signed variable to
// compare and can parse a signed string.
//
// ----------------------------------------------------------------------------
// We have two 'testing' functions defined in our test driver, that given a
// value, calls the function under test on that value and similar values
// derived from it:
//
//                       // u::transformAndTestValue<TYPE>
//
// We have 'transformAndTestValue<TYPE>', a template function that, passed a
// value, will run several transforms on it (including the identity transform,
// bitwise complement, and negation) and test each of them, calling 'to_chars'
// multiple times, starting with a buffer that is always big enough and then
// decreasing the buffer size until the function fails, and
// 'parseAndTestUnsignedInteger' or 'parseAndTestSignedInteger' (depending on
// whether 'TYPE' is signed) to verify that the string produced is correct.
// The function repeats the test for all supported values of 'base' passed to
// 'to_chars'.
//
//                          // u::metaTestValue
//
// We have 'metaTestValue' that, passed a value, calls
// 'transformAndTestValue<TYPE>' using all fundamental integral types as
// 'TYPE'.  It calls only those types that are large enough to be capable of
// representing the value.
//
// ----------------------------------------------------------------------------
//                             // Random Tests
//
// The random tests are driven by the 64-bit 'mmixRand64' random number
// generator.  The random tests of less than 8 bytes are done by masking the
// result of 'mmixRand64' down to the appropriate number of bytes.
// ----------------------------------------------------------------------------
// [ 9] USAGE EXAMPLE
// [ 8] FROM_CHARS FOR C++17 TEST
// [ 7] RANDOM 8-BYTE VALUES TEST
// [ 6] RANDOM 4-BYTE VALUES TEST
// [ 5] RANDOM 2-BYTE VALUES TEST
// [ 4] EXHAUSTIVE ONE-BYTE VALUES TEST
// [ 3] ALL CORNER CASES TEST
// [ 2] TABLE-DRIVEN TEST
// [ 1] BREATHING TEST
// [-1] SPEED TRIAL

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//                       GLOBAL TYPEDEFS AND FUNCTIONS
// ----------------------------------------------------------------------------

typedef bsls::Types::Uint64  Uint64;
typedef bsls::Types::Int64   Int64;
typedef bsls::Types::IntPtr  IntPtr;
typedef bsls::Types::UintPtr UintPtr;

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

namespace {
namespace u {

bool parseAndTestUnsignedInteger(const Uint64              exp,
                                 const bslstl::StringRef&  inputString,
                                 unsigned                  base);
    // Return 'true' if the specified 'inputString', parsed by the specified
    // 'base', yields the specified 'exp' and 'false' otherwise.
    //
    // Forward declaration -- jumping through hoops to achieve alphabetical
    // order and appease bde verify.

bool parseAndTestSignedInteger(bsls::Types::Int64 exp,
                               bslstl::StringRef  inputString,
                               unsigned           base)
    // Return 'true' if the specified 'inputString', parsed by the specified
    // 'base', yields the specified 'exp' and 'false' otherwise.
{
    const UintPtr length = inputString.length();
    if (0 == length) {
        return false;                                                 // RETURN
    }

    const bool sign = '-' == inputString[0];
    if (sign != (exp < 0)) {
        return false;                                                 // RETURN
    }

    if (sign) {
        exp = -exp;
        inputString.assign(inputString.begin() + 1, inputString.end());
    }

    return parseAndTestUnsignedInteger(exp, inputString, base);
}

bool parseAndTestUnsignedInteger(const Uint64              exp,
                                 const bslstl::StringRef&  inputString,
                                 unsigned                  base)
    // Return 'true' if the specified 'inputString', parsed by the specified
    // 'base', yields the specified 'exp' and 'false' otherwise.
{
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { k_ALPHA_OFFSET = 'a' - 10 };

    const Uint64  maxValue = numeric_limits<Uint64>::max();
    const Uint64  maxCheck = maxValue / base;
    const UintPtr length   = inputString.length();
    if (0 == length) {
        return false;                                                 // RETURN
    }

    bsls::Types::Uint64 res = 0;
    for (UintPtr ii = 0; ii < length; ++ii) {
        char     c = inputString[ii];
        unsigned digit = '0' <= c && c <= '9'
                       ? c - '0'
                       : 'a' <= c
                       ? c - k_ALPHA_OFFSET
                       : base;
        if (base <= digit) {
            return false;                                             // RETURN
        }

        if (res < maxCheck || (res == maxCheck && digit <= maxValue % base)) {
            res = res * base + digit;
        }
        else {
            return false;                                             // RETURN
        }
    }

    return exp == res;
}

template <class TYPE>
void transformAndTestValue(Uint64 seedValue)
    // Start with the specified 'seedValue'.  The function will be called with
    // values of the parameterized 'TYPE' consisting of all the integral
    // fundamental types in C++ except for 'bool'.  Translate 'seedValue'
    // through six transforms.  For each transform, cast the value to a 'TYPE'
    // object, then use 'to_chars' to translate that to a string.  Parse the
    // string to an integral type with oracle parsing functions from this
    // source file, and verify that it matches the original value passed to
    // 'to_chars'.  The behavior is undefined if 'seedValue' cannot be
    // represented as a 'TYPE'.
    //
    // If the operating system provides 'std::to_chars', then that is what will
    // forward to 'bsl::to_chars', in which case 'bslstl::to_chars' is a
    // different function.  In that case, also translate the 'TYPE' object with
    // 'bslstl::to_chars' and check that the resulting string matches what was
    // obtained using 'bsl::to_chars'.
{
    enum { k_IS_SIGNED = static_cast<TYPE>(-1) < 0,
           k_SHIFT     = 8 == sizeof(TYPE) ? 0 : sizeof(TYPE) * 8 };

    const char *type = bsls::NameOf<TYPE>().name();

    static const Uint64 mask       = 8 == sizeof(TYPE) ? ~0ULL
                                                       : (1ULL << k_SHIFT) - 1;
    static const Uint64 signExtend = ~(mask >> 1);

    BSLS_ASSERT(0 == (~mask & seedValue));

    if (veryVeryVerbose) printf("transformAndTestValue<%s>(0x%llx == %llu)\n",
                               type, static_cast<Int64>(seedValue), seedValue);

    for (int mode = 0, modeDone = false; !modeDone; ++mode) {
        Uint64 vValue = 0;                        // Transformed value
        switch (mode) {
          case 0: {
            vValue =  seedValue;
          } break;
          case 1: {
            vValue =  seedValue - 1;
          } break;
          case 2: {
            vValue =  seedValue + 1;
          } break;
          case 3: {
            vValue = ~seedValue;
          } break;
          case 4: {
            vValue = ~seedValue + 1;
          } break;
          case 5: {
            vValue = ~seedValue - 1;
            modeDone = true;
          } break;
          default: {
            BSLS_ASSERT(0 && "bad mode");
          } break;
        }

        vValue &= mask;

        if (k_IS_SIGNED && (signExtend & vValue)) {
            vValue |= signExtend;
        }

        const Int64 sValue = vValue;            // 'vValue' translated to
                                                // signed 64 bit value.
        const TYPE  tValue = static_cast<TYPE>(vValue & mask);
                                                // 'vValue' translate to 'TYPE'
                                                // variable

        ASSERT(k_IS_SIGNED ? sValue == static_cast<Int64>( tValue)
                           : tValue == static_cast<Uint64>(tValue));

        if (veryVeryVerbose) { T_;    P(tValue); }

        for (unsigned base = 2; base <= 36; ++base) {
            // 'k_BUFFER_LEN' is big enough to represent any signed 'TYPE' in
            // binary.  Note we don't have a terminating '\0'.

            enum { k_BUFFER_LEN = sizeof(TYPE) * 8 + 1 };
            char buffer[k_BUFFER_LEN];

            bool failed = false, succeeded = false, spotOn = false;
            const char *prevPtr = 0;
            for (IntPtr len = k_BUFFER_LEN, overShootBy1 = len; 0 < len;
                                       len = std::min(len - 1, overShootBy1)) {
                bsl::to_chars_result sts = bsl::to_chars(buffer,
                                                         buffer + len,
                                                         tValue,
                                                         base);
                if (bsl::ErrcEnum() == sts.ec) {
                    ASSERT(!failed);

                    if (prevPtr) {
                        ASSERT(prevPtr == sts.ptr);
                    }
                    else {
                        prevPtr = sts.ptr;

                        // skip down to just a little bigger than necessary to
                        // save time.

                        overShootBy1 = sts.ptr + 1 - buffer;
                    }
                    succeeded = true;

                    ASSERT(!spotOn);
                    if (buffer + len == sts.ptr) {
                        spotOn = true;

                        if (veryVeryVeryVerbose) {
                            const bsl::string s(buffer, sts.ptr - buffer);

                            T_;    T_;    P_(base);    P(s.c_str());
                        }
                    }

                    ASSERTV(type, len, mode, vValue, !failed);
                    ASSERTV(type, buffer  < sts.ptr);
                    ASSERTV(type, sts.ptr <= buffer + k_BUFFER_LEN);

                    if (k_IS_SIGNED) {
                        ASSERTV(type, base, mode, tValue, buffer,
                             u::parseAndTestSignedInteger(tValue,
                                                          bslstl::StringRef(
                                                              buffer, sts.ptr),
                                                          base));
                    }
                    else {
                        ASSERTV(type, base, mode, tValue, buffer,
                             u::parseAndTestUnsignedInteger(tValue,
                                                            bslstl::StringRef(
                                                              buffer, sts.ptr),
                                                            base));
                    }
                }
                else {
                    ASSERT(bsl::errc::value_too_large == sts.ec);
                    failed = true;
                }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                namespace imp = BloombergLP::bslstl;

                char iBuffer[1000];
                imp::to_chars_result iSts = imp::to_chars(iBuffer,
                                                          iBuffer + len,
                                                          tValue,
                                                          base);
                ASSERT((bsl::ErrcEnum() == iSts.ec) ==
                                                  (bsl::ErrcEnum() == sts.ec));
                ASSERT(iSts.ptr - iBuffer == sts.ptr - buffer);
                if (bsl::ErrcEnum() == iSts.ec) {
                    ASSERT(0 == std::memcmp(buffer,
                                            iBuffer,
                                            sts.ptr - buffer));
                }
                else {
                    ASSERT(bsl::errc::value_too_large == iSts.ec);
                }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
            }

            ASSERT(succeeded);
            ASSERT(spotOn);
            ASSERT((vValue < base) == !failed);
        }
    }
}

void metaTestValue(Uint64 value)
    // Run 'u::transformAndTestValue' on the specified 'value' for all
    // fundamental integral types, but only those types capable of representing
    // the value.
{
    u::transformAndTestValue<Uint64>(value);
    if (0x8000000000000000ULL < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<Int64>(value);
    if (0xffffffff < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<unsigned>(value);
    if (0x80000000 < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<int>(value);
    if (0xffff < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<unsigned short>(value);
    if (0x8000 < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<short>(value);
    if (0xff < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<unsigned char>(value);
    if (0x80 < value) {
        return;                                                       // RETURN
    }
    u::transformAndTestValue<signed char>(value);
    u::transformAndTestValue<char>(value);
}

Uint64 mmixRand64(Uint64 seed = 0)
    // Return a random number generated by a modified form of the MMIX Linear
    // Congruential Generator algorithm developed by Donald Knuth (modified).
    // If the optionally specified 'seed' is non-zero, reset the accumulator to
    // 'seed'.
{
    static Uint64 randAccum = 0;
    if (seed) {
        randAccum = seed;
    }

    static const Uint64 a    = 6364136223846793005ULL;
    static const Uint64 c    = 1442695040888963407ULL;
    static const Uint64 mask = ((1ULL << 32) - 1) << 32;

    BSLMF_ASSERT(0 != mask);
    BSLMF_ASSERT(0 == (~static_cast<unsigned>(0) & mask));
    BSLMF_ASSERT(static_cast<Int64>(mask) < 0);

    randAccum = randAccum * a + c;
    Uint64 hi = randAccum & mask;
    randAccum = randAccum * a + c;

    return hi | (randAccum >> 32);
}

const Uint64 uint64Max = ~0ULL;    // max value a 'Uint64' can represent

}  // close namespace u
}  // close unnamed namespace

//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Demonstrating Writing a number to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a function that writes an 'int' to a 'streambuf'.
// We can use 'bsl::to_chars' to write the 'int' to a buffer, then write the
// buffer to the 'streambuf'.
//
// First, we declare our function:
//..
    void writeJsonScalar(std::streambuf *result, int value)
        // Write the specified 'value', in decimal, to the specified 'result'.
    {
//..
// Then, we declare a buffer long enough to store any 'int' value in decimal.
//..
        char buffer[11];        // size large enough to write 'INT_MIN', the
                                // worst-case value, in decimal.
//..
// Next, we declare a variable to store the return value:
//..
        bsl::to_chars_result sts;
//..
// Then, we call the function:
//..
        sts = bsl::to_chars(buffer, buffer + sizeof(buffer), value);
//..
// Next, we check that the buffer was long enough, which should always be the
// case:
//..
        ASSERT(bsl::ErrcEnum() == sts.ec);
//..
// Now, we check that 'sts.ptr' is in the range
// '[ buffer + 1, buffer + sizeof(buffer) ]', which will always be the case
// whether 'to_chars' succeeded or failed.
//..
        ASSERT(buffer  <  sts.ptr);
        ASSERT(sts.ptr <= buffer + sizeof(buffer));
//..
// Finally, we write our buffer to the 'streambuf':
//..
        result->sputn(buffer, sts.ptr - buffer);
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;    (void) verbose;
            veryVerbose = argc > 3;    (void) veryVerbose;
        veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    const int multiplier = verbose ? std::max(std::atoi(argv[2]), 1) : 1;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 The function 'writeJsonScalar' (above) demonstrates how the
        //:   component could be used to write a number, in ascii, to a
        //:   'streambuf'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // This is not part of the usage example, here we just test that the
        // function defined by the usage example works.

        bsl::ostringstream oss;
        std::streambuf *sb = oss.rdbuf();

        writeJsonScalar(sb, 0);
        ASSERT("0" == oss.str());

        oss.str("");

        writeJsonScalar(sb, 99);
        ASSERT("99" == oss.str());

        oss.str("");

        writeJsonScalar(sb, -1234567890);    // worst case int, max string len
        ASSERT("-1234567890" == oss.str());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // FROM_CHARS FOR C++17 TEST
        //
        // Concern:
        //: 1 That the function 'from_chars' and the associated types are
        //:   available in the namespace 'bsl'.
        //
        // Plan:
        //: 1 Call the function 'bsl::from_chars' and check the results.
        //:   Because we're importing from the native standard library, we
        //:   don't need exhaustive tests, just making sure that the routines
        //:   are present.
        //
        // Testing:
        //   FROM_CHARS FOR C++17 TEST
        // --------------------------------------------------------------------

        if (verbose) printf("FROM_CHARS FOR C++17 TEST\n"
                            "=========================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
        const char             *numStr = "123.654";
        const char             *intStrEnd = std::strchr(numStr, '.');
        int                     val = -42;
        bsl::from_chars_result  res;

        res = bsl::from_chars(numStr, intStrEnd, val, 10);
        ASSERT(bsl::errc{} == res.ec);
        ASSERT(123 == val);
        ASSERT(res.ptr == intStrEnd);

        res = bsl::from_chars(numStr, intStrEnd, val, 16);
        ASSERT(bsl::errc{} == res.ec);
        ASSERT(0x123 == val);
        ASSERT(res.ptr == intStrEnd);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
        const char *numStrEnd = numStr + strlen(numStr);

        double dVal;
        res = bsl::from_chars(numStr,
                              numStrEnd,
                              dVal,
                              bsl::chars_format::general);
        ASSERT(bsl::errc{} == res.ec);
        ASSERT(123.654 == dVal);
        ASSERT(res.ptr == numStrEnd);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // RANDOM 8-BYTE VALUES TEST
        //
        // Concern:
        //: 1 That the component under test performs correctly for values
        //:   greater than 0xffffffff.
        //
        // Plan:
        //: 1 Generate random 64-bit values and feed them to 'metaTestValue'.
        //
        // Testing:
        //   RANDOM 8-BYTE VALUES TEST
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM 8-BYTE VALUES TEST\n"
                            "=========================\n");

        const int iterations = 1200 * multiplier;
        ASSERT(0 < iterations);

        if (verbose) P(iterations);

        for (int ii = 0; ii < iterations; ++ii) {
            const Uint64 value = u::mmixRand64();

            if (veryVerbose) printf("value: 0x%016llx\n", value);

            u::metaTestValue(value);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // RANDOM 4-BYTE VALUES TEST
        //
        // Concern:
        //: 1 That the component under test performs correctly for random
        //:   values in the range '[ 0x1000 .. 0xffffffff ]'.
        //
        // Plan:
        //: 1 Generate random 32-bit values and feed them to 'metaTestValue'.
        //
        // Testing:
        //   RANDOM 4-BYTE VALUES TEST
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM 4-BYTE VALUES TEST\n"
                            "=========================\n");

        const int iterations = 1200 * multiplier;
        ASSERT(0 < iterations);

        if (verbose) P(iterations);

        for (int ii = 0; ii < iterations; ++ii) {
            const Uint64 value = u::mmixRand64() & 0xffffffff;

            if (veryVerbose) printf("value: 0x%08llx\n", value);

            u::metaTestValue(value);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RANDOM 2-BYTE VALUES TEST
        //
        // Concern:
        //: 1 That the component under test performs correctly for random
        //:   values in the range '[ 256 .. 0xffff ]'.
        //
        // Plan:
        //: 1 Generate random 16-bit values and feed them to 'metaTestValue'.
        //:
        //: 2 Skip testing values in the range '[ 0 .. 0xff ]' since they were
        //:   already covered in test case 4.
        //
        // Testing:
        //   RANDOM 2-BYTE VALUES TEST
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM 2-BYTE VALUES TEST\n"
                            "=========================\n");

        const int iterations = 1200 * multiplier;
        ASSERT(0 < iterations);

        if (verbose) P(iterations);

        for (int ii = 0; ii < iterations; ++ii) {
            const Uint64 value = u::mmixRand64() & 0xffff;
            if (value < 0x100) {
                // We already tested this value in the previous test.

                --ii;
                continue;
            }

            if (veryVerbose) printf("value: 0x%04llx\n", value);

            u::metaTestValue(value);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EXHAUSTIVE ONE-BYTE VALUES TEST
        //
        // Concern:
        //: 1 That the function under test operates correctly for all values
        //:   in the range '[ 0 .. 0xff ]' for all fundamental integral types.
        //
        // Plan:
        //: 1 Iterate through all possible values that single byte types
        //:   represent, the range '[ 0 .. 0xff ]', and pass the values to
        //:   'u::metaTestValue'.
        //
        // Testing:
        //   EXHAUSTIVE ONE-BYTE VALUES TEST
        // --------------------------------------------------------------------

        if (verbose) printf("EXHAUSTIVE ONE-BYTE VALUES TEST\n"
                            "===============================\n");

        for (Uint64 value = 0; value <= 0xff; ++value) {
            if (veryVerbose) P(value);

            u::metaTestValue(value);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALL CORNER CASES TEST
        //
        // Concern:
        //: 1 Run the function on all corner cases in all bases and check that
        //:   it yields a completely correct result in each case.
        //
        // Plan:
        //: 1 Loop through all powers of all supported bases and pass those
        //:   values to 'u::metaTestValue', with all bases in each case, that
        //:   will call 'u::transformAndTestValue' on the value for all
        //:   integral types capable of representing it.
        //
        // Testing:
        //   ALL CORNER CASES TEST
        // --------------------------------------------------------------------

        if (verbose) printf("ALL CORNER CASES TEST\n"
                            "=====================\n");

        // We are interested in running the test on all powers of all possible
        // bases, and running each of those values on all possible bases.

        // First, set those values of 'redundantPowers' whose index is a number
        // that is a power of a lower integral value.  It is not necessary to
        // test those values of 'power' in the later loop, saving time.

        static bool redundantPowers[37] = { 0 };
        for (unsigned power = 2; power <= 36; ++power) {
            for (unsigned rPower = power * power; rPower <= 36;
                                                             rPower *= power) {
                redundantPowers[rPower] = true;
            }
        }

        // Next, iterate through all powers of all bases supported by the
        // function under test.

        for (unsigned power = 2; power <= 36; ++power) {
            if (redundantPowers[power]) {
                continue;
            }

            // Increase 'value' by a factor of 'power' repeatedly until the
            // result can no longer be represented as a 'Uint64'.

            for (Uint64 value = 1, ceiling = u::uint64Max / power;
                                                          value <= ceiling; ) {
                value *= power;

                // 'u::metaTestValue' will run the function under test on
                // 'value' and 5 transforms of it, for all possible bases, for
                // every integral fundamental type capable of representing
                // 'value'.

                u::metaTestValue(value);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // QUALITY TEST & ACCURACY ON SOME BASES TEST
        //
        // Concern:
        //: 1 That, in any base,
        //:   o negative numbers are always output beginning with '-'.
        //:
        //:   o positive numbers don't contain '-'.
        //:
        //:   o output other than an appropriate beginning '-' consists
        //:     entirely of digits in the range '[ '0' ..  <base> )'.
        //:
        //:   o that a negative number is output as the same string as the
        //:     corresponding positive number, except with a leading '-' added.
        //:
        //:   o that the number of digits output is appropriate to the log of
        //:     the value in the given base.
        //:
        //: 2 That the function under test can accurately output decimal, hex,
        //:   and octal strings of given values.
        //
        // Plan:
        //: 1 Have a table with positive constants in both string form and
        //:   parsed as integral types, as either decimal, hex, or octal
        //:   source, and a field indicating the base of the source number.
        //:
        //: 2 Iterate through the table of numbers.
        //:   o Iterate through all bases in the range '[ 2 .. 36 ]'.
        //:
        //:   o Output the numbers and observe that they contain only digits
        //:     in the range '[ 0 .. <base> )'.
        //:
        //:   o If the number is not 0, observe that the number of digits
        //:     output is as expected for the size of the number and the base
        //:     of the output.
        //:
        //:   o Assign the value to a signed type.  Output it again and observe
        //:     we get the same result.
        //:
        //:   o Negate the signed type and output it again, and observe that
        //:     we got the same value preceded by '-'.
        //:
        //: 3 Iterate through the table of numbers.  For each number:
        //:   o write the number with both 'snprintf' and 'to_chars' as
        //:     unsigned decimal, hex, and octal strings, and using 'sprintf'
        //:     as an "oracle" for comparing the resulting strings for perfect
        //:     accuracy.
        //:
        //:   o Assign it to a signed type, with it with both 'snprintf' and
        //:     'to_chars', and observe they match to test for perfect
        //:     accuracy.  Then negate the signed type and repeat the
        //:     experiment.
        //:
        //: 4 Iterate through the table and for unsigned decimal, hex, or
        //:   octal, if the string in the table is of that radix, compare the
        //:   string in the table with the string generated by 'to_chars' to
        //:   check that they match for perfect accuracy.
        //
        // Testing:
        //   TABLE-DRIVEN TEST
        // --------------------------------------------------------------------

        if (verbose) printf("QUALITY TEST & ACCURACY ON SOME BASES TEST\n"
                            "==========================================\n");

#undef  D
#undef  H
#undef  O

#define D(s)    #s,       s ## ULL, 10
#define H(s)    #s, 0x ## s ## ULL, 16
#define O(s)    #s,  0 ## s ## ULL,  8

        static const struct Data {
            int         d_line;
            const char *d_string_p;
            Uint64      d_value;
            unsigned    d_base;
        } DATA[] = {
            { L_, H(0) },
            { L_, H(1) },
            { L_, H(2) },
            { L_, H(4) },
            { L_, H(8) },
            { L_, H(10) },
            { L_, H(20) },
            { L_, H(40) },
            { L_, H(80) },
            { L_, H(100) },
            { L_, H(200) },
            { L_, H(400) },
            { L_, H(800) },
            { L_, H(1000) },
            { L_, H(2000) },
            { L_, H(4000) },
            { L_, H(8000) },
            { L_, H(10000) },
            { L_, H(20000) },
            { L_, H(40000) },
            { L_, H(80000) },
            { L_, H(100000) },
            { L_, H(200000) },
            { L_, H(400000) },
            { L_, H(800000) },
            { L_, H(1000000) },
            { L_, H(2000000) },
            { L_, H(4000000) },
            { L_, H(8000000) },
            { L_, H(10000000) },
            { L_, H(20000000) },
            { L_, H(40000000) },
            { L_, H(80000000) },
            { L_, H(100000000) },
            { L_, H(200000000) },
            { L_, H(400000000) },
            { L_, H(800000000) },
            { L_, H(1000000000) },
            { L_, H(2000000000) },
            { L_, H(4000000000) },
            { L_, H(8000000000) },
            { L_, H(10000000000) },
            { L_, H(20000000000) },
            { L_, H(40000000000) },
            { L_, H(80000000000) },
            { L_, H(100000000000) },
            { L_, H(200000000000) },
            { L_, H(400000000000) },
            { L_, H(800000000000) },
            { L_, H(1000000000000) },
            { L_, H(2000000000000) },
            { L_, H(4000000000000) },
            { L_, H(8000000000000) },
            { L_, H(10000000000000) },
            { L_, H(20000000000000) },
            { L_, H(40000000000000) },
            { L_, H(80000000000000) },
            { L_, H(100000000000000) },
            { L_, H(200000000000000) },
            { L_, H(400000000000000) },
            { L_, H(800000000000000) },
            { L_, H(1000000000000000) },
            { L_, H(2000000000000000) },
            { L_, H(4000000000000000) },
            { L_, H(8000000000000000) },

            { L_, O(0) },
            { L_, O(1) },
            { L_, O(2) },
            { L_, O(4) },
            { L_, O(10) },
            { L_, O(20) },
            { L_, O(40) },
            { L_, O(100) },
            { L_, O(200) },
            { L_, O(400) },
            { L_, O(1000) },
            { L_, O(2000) },
            { L_, O(4000) },
            { L_, O(10000) },
            { L_, O(20000) },
            { L_, O(40000) },
            { L_, O(100000) },
            { L_, O(200000) },
            { L_, O(400000) },
            { L_, O(1000000) },
            { L_, O(2000000) },
            { L_, O(4000000) },
            { L_, O(10000000) },
            { L_, O(20000000) },
            { L_, O(40000000) },
            { L_, O(100000000) },
            { L_, O(200000000) },
            { L_, O(400000000) },
            { L_, O(1000000000) },
            { L_, O(2000000000) },
            { L_, O(4000000000) },
            { L_, O(10000000000) },
            { L_, O(20000000000) },
            { L_, O(40000000000) },
            { L_, O(100000000000) },
            { L_, O(200000000000) },
            { L_, O(400000000000) },
            { L_, O(1000000000000) },
            { L_, O(2000000000000) },
            { L_, O(4000000000000) },
            { L_, O(10000000000000) },
            { L_, O(20000000000000) },
            { L_, O(40000000000000) },
            { L_, O(100000000000000) },
            { L_, O(200000000000000) },
            { L_, O(400000000000000) },
            { L_, O(1000000000000000) },
            { L_, O(2000000000000000) },
            { L_, O(4000000000000000) },
            { L_, O(10000000000000000) },
            { L_, O(20000000000000000) },
            { L_, O(40000000000000000) },
            { L_, O(100000000000000000) },
            { L_, O(200000000000000000) },
            { L_, O(400000000000000000) },
            { L_, O(1000000000000000000) },
            { L_, O(2000000000000000000) },
            { L_, O(4000000000000000000) },
            { L_, O(10000000000000000000) },
            { L_, O(20000000000000000000) },
            { L_, O(40000000000000000000) },
            { L_, O(100000000000000000000) },
            { L_, O(200000000000000000000) },
            { L_, O(400000000000000000000) },
            { L_, O(1000000000000000000000) },

            { L_, D(0) },
            { L_, D(1) },
            { L_, D(10) },
            { L_, D(100) },
            { L_, D(1000) },
            { L_, D(10000) },
            { L_, D(100000) },
            { L_, D(1000000) },
            { L_, D(10000000) },
            { L_, D(100000000) },
            { L_, D(1000000000) },
            { L_, D(10000000000) },
            { L_, D(100000000000) },
            { L_, D(1000000000000) },
            { L_, D(10000000000000) },
            { L_, D(100000000000000) },
            { L_, D(1000000000000000) },
            { L_, D(10000000000000000) },
            { L_, D(100000000000000000) },
            { L_, D(1000000000000000000) },

            { L_, H(1) },
            { L_, H(12) },
            { L_, H(123) },
            { L_, H(1234) },
            { L_, H(12345) },
            { L_, H(123456) },
            { L_, H(1234567) },
            { L_, H(12345678) },
            { L_, H(123456789) },
            { L_, H(1234567890) },
            { L_, H(12345678901) },
            { L_, H(123456789012) },
            { L_, H(1234567890123) },
            { L_, H(12345678901234) },
            { L_, H(123456789012345) },
            { L_, H(1234567890123456) },

            { L_, H(9) },
            { L_, H(98) },
            { L_, H(987) },
            { L_, H(9876) },
            { L_, H(98765) },
            { L_, H(987654) },
            { L_, H(9876543) },
            { L_, H(98765432) },
            { L_, H(987654321) },
            { L_, H(9876543219) },
            { L_, H(98765432198) },
            { L_, H(987654321987) },
            { L_, H(9876543219876) },
            { L_, H(98765432198765) },
            { L_, H(987654321987654) },
            { L_, H(9876543219876543) },

            { L_, H(f) },
            { L_, H(fe) },
            { L_, H(fed) },
            { L_, H(fedc) },
            { L_, H(fedcb) },
            { L_, H(fedcba) },
            { L_, H(fedcba9) },
            { L_, H(fedcba98) },
            { L_, H(fedcba987) },
            { L_, H(fedcba9876) },
            { L_, H(fedcba98765) },
            { L_, H(fedcba987654) },
            { L_, H(fedcba9876543) },
            { L_, H(fedcba98765432) },
            { L_, H(fedcba987654321) },
            { L_, H(fedcba9876543210) },

            { L_, D(1) },
            { L_, D(12) },
            { L_, D(123) },
            { L_, D(1234) },
            { L_, D(12345) },
            { L_, D(123456) },
            { L_, D(1234567) },
            { L_, D(12345678) },
            { L_, D(123456789) },
            { L_, D(1234567890) },
            { L_, D(12345678901) },
            { L_, D(123456789012) },
            { L_, D(1234567890123) },
            { L_, D(12345678901234) },
            { L_, D(123456789012345) },
            { L_, D(1234567890123456) },
            { L_, D(12345678901234567) },
            { L_, D(123456789012345678) },
            { L_, D(1234567890123456789) },
            { L_, D(12345678901234567890) },

            { L_, O(7) },
            { L_, O(76) },
            { L_, O(765) },
            { L_, O(7654) },
            { L_, O(76543) },
            { L_, O(765432) },
            { L_, O(7654321) },
            { L_, O(76543210) },
            { L_, O(765432107) },
            { L_, O(7654321076) },
            { L_, O(76543210765) },
            { L_, O(765432107654) },
            { L_, O(7654321076543) },
            { L_, O(76543210765432) },
            { L_, O(765432107654321) },
            { L_, O(7654321076543210) },
            { L_, O(76543210765432107) },
            { L_, O(765432107654321076) },
            { L_, O(7654321076543210765) },
            { L_, O(76543210765432107654) },
            { L_, O(765432107654321076543) },

            { L_, O(1) },
            { L_, O(12) },
            { L_, O(123) },
            { L_, O(1234) },
            { L_, O(12345) },
            { L_, O(123456) },
            { L_, O(1234567) },
            { L_, O(12345670) },
            { L_, O(123456701) },
            { L_, O(1234567012) },
            { L_, O(12345670123) },
            { L_, O(123456701234) },
            { L_, O(1234567012345) },
            { L_, O(12345670123456) },
            { L_, O(123456701234567) },
            { L_, O(1234567012345670) },
            { L_, O(12345670123456701) },
            { L_, O(123456701234567012) },
            { L_, O(1234567012345670123) },
            { L_, O(12345670123456701234) },
            { L_, O(123456701234567012345) },
            { L_, O(1234567012345670123456) },

            { L_, D(9) },
            { L_, D(98) },
            { L_, D(987) },
            { L_, D(9876) },
            { L_, D(98765) },
            { L_, D(987654) },
            { L_, D(9876543) },
            { L_, D(98765432) },
            { L_, D(987654321) },
            { L_, D(9876543219) },
            { L_, D(98765432198) },
            { L_, D(987654321987) },
            { L_, D(9876543219876) },
            { L_, D(98765432198765) },
            { L_, D(987654321987654) },
            { L_, D(9876543219876543) },
            { L_, D(98765432198765432) },
            { L_, D(987654321987654321) } };

        enum { k_NUM_DATA = sizeof DATA /  sizeof *DATA };

#undef D
#undef H
#undef O

        const Int64 int64Min = std::numeric_limits<Int64>::min();

        if (verbose) printf("quality tests (not testing for accuracy)\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            Uint64          value  = data.d_value;

            for (unsigned base = 2; base <= 36; ++base) {
                char toCharsBuffer[66];
                bsl::to_chars_result result;

                result = bsl::to_chars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       value,
                                       base);
                ASSERT(bsl::ErrcEnum() == result.ec);
                ASSERT(toCharsBuffer < result.ptr);
                ASSERT(result.ptr < toCharsBuffer + sizeof(toCharsBuffer));
                for (const char *pc = toCharsBuffer; pc < result.ptr; ++pc) {
                    unsigned digit = '0' <= *pc && *pc <= '9'
                                   ? *pc - '0'
                                   : 'a' <= *pc && *pc <= 'z'
                                   ? 10 + *pc - 'a'
                                   : 100;
                    ASSERT(digit < base);
                }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                namespace imp = BloombergLP::bslstl;

                char iBuffer[1000];
                imp::to_chars_result iSts = imp::to_chars(
                                                     iBuffer,
                                                     iBuffer + sizeof(iBuffer),
                                                     value,
                                                     base);
                ASSERT(bsl::ErrcEnum() == iSts.ec);
                ASSERT(iBuffer < iSts.ptr);
                ASSERT(iSts.ptr < iBuffer + sizeof(iBuffer));
                ASSERT(iSts.ptr - iBuffer == result.ptr - toCharsBuffer);
                ASSERT(0 == std::memcmp(toCharsBuffer,
                                        iBuffer,
                                        iSts.ptr - iBuffer));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV

                if (value != 0) {
                    const Int64 len = result.ptr - toCharsBuffer;
                    Uint64 pow = 1;
                    for (int uu = 0; uu < len - 1; ++uu) {
                        pow *= base;
                    }
                    ASSERT(pow <= value);
                    const Uint64 nextPow = pow * base;
                    ASSERTV(LINE, nextPow, pow, base, value,
                                 value < nextPow || u::uint64Max / pow < base);
                }

                Int64 svalue = value;
                if (svalue < 0) {
                    value = -svalue;

                    result = bsl::to_chars(
                                         toCharsBuffer,
                                         toCharsBuffer + sizeof(toCharsBuffer),
                                         value,
                                         base);
                    ASSERT(bsl::ErrcEnum() == result.ec);
                    ASSERT(toCharsBuffer < result.ptr);
                    ASSERT(result.ptr < toCharsBuffer + sizeof(toCharsBuffer));

                    if (int64Min != svalue) {    // prevent undefined behavior
                        svalue = -svalue;
                    }
                }

                char toCharsBufferS[66];
                bsl::to_chars_result resultS;

                if (0 <= svalue) {
                    resultS = bsl::to_chars(
                                        toCharsBufferS,
                                        toCharsBufferS + sizeof(toCharsBuffer),
                                        svalue,
                                        base);
                    ASSERT(bsl::ErrcEnum() == resultS.ec);
                    ASSERT(resultS.ptr <
                                      toCharsBufferS + sizeof(toCharsBufferS));
                    ASSERTV(LINE, value, svalue, base, 1ULL << 63,
                                                resultS.ptr - toCharsBufferS ==
                                                   result.ptr - toCharsBuffer);
                    ASSERTV(LINE, value, svalue, base,
                                     !std::memcmp(toCharsBuffer,
                                                  toCharsBufferS,
                                                  result.ptr - toCharsBuffer));

                    if (0 == svalue) {
                        continue;
                    }


                    if (int64Min != svalue) {    // prevent undefined behavior
                        svalue = -svalue;
                    }
                    ASSERTV(LINE, value, svalue, base, svalue < 0);
                }

                resultS = bsl::to_chars(toCharsBufferS,
                                        toCharsBufferS + sizeof(toCharsBuffer),
                                        svalue,
                                        base);
                ASSERT(bsl::ErrcEnum() == resultS.ec);
                ASSERT(toCharsBufferS < resultS.ptr);
                ASSERT(resultS.ptr < toCharsBufferS + sizeof(toCharsBufferS));
                ASSERT('-' == toCharsBufferS[0]);
                ASSERT(resultS.ptr - toCharsBufferS ==
                                               1 + result.ptr - toCharsBuffer);
                ASSERT(!std::memcmp(toCharsBuffer,
                                    toCharsBufferS + 1,
                                    result.ptr - toCharsBuffer));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                iSts = imp::to_chars(iBuffer,
                                     iBuffer + sizeof(iBuffer),
                                     svalue,
                                     base);
                ASSERT(bsl::ErrcEnum() == iSts.ec);
                ASSERT(iSts.ptr - iBuffer == resultS.ptr - toCharsBufferS);
                ASSERT(0 == std::memcmp(toCharsBufferS,
                                        iBuffer,
                                        iSts.ptr - iBuffer));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
            }
        }

        if (verbose) printf("Accuracy 1: snprintf comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;

            char sprintfBuffer[40];
            char toCharsBuffer[40];
            bsl::to_chars_result result;

            snprintf(sprintfBuffer, sizeof(sprintfBuffer), "%llu", VALUE);
            result = bsl::to_chars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   10);
            ASSERT(bsl::ErrcEnum() == result.ec);
            *result.ptr = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));

            for (Int64 sValue = VALUE, ii = 0; ii < 2; ++ii, sValue = -sValue){
                snprintf(sprintfBuffer, sizeof(sprintfBuffer), "%lld", sValue);
                result = bsl::to_chars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       sValue,
                                       10);
                ASSERT(bsl::ErrcEnum() == result.ec);
                *result.ptr = 0;
                ASSERTV(LINE, sprintfBuffer, toCharsBuffer,
                                   !std::strcmp(sprintfBuffer, toCharsBuffer));

                if (int64Min == sValue) {
                    break;    // prevent undefined behavior
                }
            }

            snprintf(sprintfBuffer, sizeof(sprintfBuffer), "%llo", VALUE);
            result = bsl::to_chars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   8);
            ASSERT(bsl::ErrcEnum() == result.ec);
            *result.ptr = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));

            snprintf(sprintfBuffer, sizeof(sprintfBuffer), "%llx", VALUE);
            result = bsl::to_chars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   16);
            ASSERT(bsl::ErrcEnum() == result.ec);
            *result.ptr = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));
        }

        if (verbose) printf("Accuracy 2: Source string comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;
            const char     *STRING = data.d_string_p;
            const unsigned  BASE   = data.d_base;

            char toCharsBuffer[40];
            bsl::to_chars_result result;

            result = bsl::to_chars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   BASE);
            ASSERT(bsl::ErrcEnum() == result.ec);
            *result.ptr = 0;
            std::string s = STRING;
            ASSERTV(LINE, s == toCharsBuffer);
            if (0 < VALUE && VALUE <= (1ULL << 63)) {
                const Int64 sValue = -VALUE;
                result = bsl::to_chars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       sValue,
                                       BASE);
                *result.ptr = 0;

                s.insert(0, "-");
                ASSERTV(LINE, s.c_str(), toCharsBuffer, s == toCharsBuffer);
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        int values[] = { 0, 1, -1, 2, 5, 7, 17, 1000, -1000, 1024, 1025,
                         12345678, -12345678 };
        enum { k_NUM_VALUES = sizeof values / sizeof *values };

        for (int ii = 0; ii < k_NUM_VALUES; ++ii) {
            const int value = values[ii];

            if (veryVerbose) P(value);

            char sprintfBuf[20];
            snprintf(sprintfBuf, sizeof(sprintfBuf), "%d", value);
            const IntPtr len = std::strlen(sprintfBuf);

            char toCharsBuf[20];
            bsl::to_chars_result result = bsl::to_chars(toCharsBuf,
                                                        toCharsBuf + 19,
                                                        value,
                                                        10);
            ASSERT(bsl::ErrcEnum() == result.ec);
            ASSERT(result.ptr - toCharsBuf == len);
            *result.ptr = 0;
            ASSERTV(value, toCharsBuf, sprintfBuf,
                                0 == std::memcmp(toCharsBuf, sprintfBuf, len));
        }
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
// Copyright 2020 Bloomberg Finance L.P.
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
