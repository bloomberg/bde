// bslalg_numericformatterutil.t.cpp                                  -*-C++-*-
#include <bslalg_numericformatterutil.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_stopwatch.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
# include <charconv>
#endif

#include <algorithm>
#include <string>
#include <sstream>
#include <streambuf>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

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
// In the following cases, 'TYPE' means every integral fundamental type, signed
// or unsigned, up to 64 bits long.
// ----------------------------------------------------------------------------
// [ 5] char *toChars(char *, char *, TYPE, int); // Random Unsigned
// [ 4] char *toChars(char *, char *, TYPE, int); // Random Signed
// [ 3] char *toChars(char *, char *, TYPE, int); // Corner Cases
// [ 2] char *toChars(char *, char *, TYPE, int); // Table-Driven
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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

#define T2_          printf("  ");

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

typedef bslalg::NumericFormatterUtil Util;
typedef bsls::Types::Uint64          Uint64;
typedef bsls::Types::Int64           Int64;
typedef bsls::Types::IntPtr          IntPtr;

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

namespace {
namespace u {

char *generateStringRep(char *back, Uint64 value, bool sign, unsigned base)
    // Write a string representation of the specified 'value' using the
    // specified 'base', with the specified 'sign' if the 'value' is to be
    // negated, to the buffer ending at the specified 'back'.  Return a pointer
    // to the beginning of the written value.
{
    BSLS_ASSERT(value || !sign);

    do {
        unsigned digit = static_cast<unsigned>(value % base);
        *--back = static_cast<char>(digit < 10 ? '0' + digit
                                               : 'a' + (digit - 10));
        value /= base;
    } while (0 < value);

    if (sign) {
        *--back = '-';
    }

    return back;
}

int parseInt(Uint64     *result,
             const char *first,
             const char *last,
             unsigned    base)
    // Parse as a number the string specified by '[ first, last )' in the
    // specified 'base', setting '*result' to the value represented by the
    // string.  Negative numbers can be signified by '-' which must be the
    // value of '*first'.  Fail an 'ASSERT' and return a negative number on
    // error, and return +1 if the value represented can't be stored in an
    // 'Int64'.
{
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    const Uint64 maxU = numeric_limits<Uint64>::max();
    const Uint64 maxI = numeric_limits<Int64 >::max();
    Uint64 tmpResult = 0;

    bool isMinI     = false;
    bool passesMinI = false;

    for (Uint64 radix = 1, prevRadix = 0; first < last--;
                                            prevRadix = radix, radix *= base) {
        const char c = *last;

        if (radix <= prevRadix && '-' != c) {
            ASSERT(0 && "number too long, radix wrapped");

            return -1;                                                // RETURN
        }

        unsigned digit;
        if      ('0' <= c && c <= '9') {
            digit = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            digit = 10 + c - 'a';
        }
        else if ('-' == c) {
            *result = ~tmpResult + 1;

            if (first != last) {
                ASSERTV(last - first, first == last);

                return -1;                                            // RETURN
            }

            if (passesMinI) {
                ASSERT(passesMinI);

                return -1;                                            // RETURN
            }

            return 0;                                                 // RETURN
        }
        else {
            ASSERTV(c, static_cast<int>(c), 0);

            return -1;                                                // RETURN
        }

        if (base <= digit) {
            ASSERTV(digit, base, digit < base);

            return -1;                                                // RETURN
        }

        if (maxU / radix < digit) {
            ASSERTV(digit, radix, digit <= maxU / radix);

            return -1;                                                // RETURN
        }
        if (maxU - tmpResult < digit * radix) {
            ASSERTV(digit, radix, maxU, tmpResult,
                                            digit * radix <= maxU - tmpResult);

            return -1;                                                // RETURN
        }

        if (maxI / radix < digit) {
            if ((maxI + 1) / radix == digit) {
                isMinI = true;
            }
            else {
                passesMinI = true;
            }
        }
        if (maxI - tmpResult < digit * radix) {
            if ((maxI + 1) - tmpResult == digit * radix) {
                isMinI = true;
            }
            else {
                passesMinI = true;
            }
        }

        tmpResult += digit * radix;
    }

    *result = tmpResult;

    return isMinI || passesMinI ? 1 : 0;
}

Uint64 mmixRand64(bool reset = false)
    // MMIX Linear Congruential Generator algorithm by Donald Knuth (modified).
    // Optionally specified 'reset' which, if 'true', indicates that the
    // accumulator is to be reset to zero.
{
    static Uint64 randAccum = 0;
    if (reset) {
        randAccum = 0;
    }

    enum { k_MASK = ~static_cast<unsigned>(0) };

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

template <class TYPE>
void randTest(Uint64 iterationsForByte)
    // Test the function under test for a number of iterations specified by
    // 'iterationsForByte', passing randomly-generated values of the specified
    // 'TYPE' to the function, where the actual number of iterations will be
    // scaled by 2 to the power of 'sizeof(TYPE)'.  The random number generator
    // is a modified form of Knuth's 'MMIX' algorithm.  Errors are detected and
    // reported via 'ASSERT' and 'ASSERTV'.  Each randomly generated value is
    // tested with all bases in the range '[ 2 .. 36 ]'.
    //
    // Unlike the function 'testValue' above, we are not just doing corner
    // cases so more effort is made here to test as many values as quickly as
    // possible.
{
    const char *name = bsls::NameOf<TYPE>().name();

    static const char starBuffer[] = {
                        "**************************************************"
                        "**************************************************" };
    BSLMF_ASSERT(sizeof(starBuffer) == 101);

    const Uint64 iterations = iterationsForByte << sizeof(TYPE);

    mmixRand64(true);

    Uint64 mask = ~static_cast<Uint64>(0);
    if (sizeof(TYPE) < sizeof(Uint64)) {
        // Microsoft compilers freak out about shifts by 64 in here even though
        // they're impossible, so make them more impossible.

        unsigned shift = sizeof(TYPE) * 8;
        shift = sizeof(TYPE) < sizeof(Uint64) ? shift : 63;
        mask = (1ULL << shift) - 1;
    }
    const Uint64 hiMask = ~mask;

    if (veryVerbose) printf("randTest: type: %s mask: 0x%llx\n", name, mask);

    for (Uint64 ii = 0; ii < iterations; ++ii) {
        Uint64     bigNum = mmixRand64() & mask;
        const TYPE value  = static_cast<TYPE>(bigNum);

        const bool sign = value < 0;
        if (sign) {
            // make 'bigNum' the absolute value of 'value'

            // in the imp, we just assign 'value' to a 'Uint64' and believe
            // that it will sign extend, and we negate a 'Uint64' by compliment
            // and add 1.  Here we do it through a different maneuver to test
            // that we get the same result.

            bigNum |= hiMask;                        // sign extend
            ASSERT(static_cast<Int64>(bigNum) == static_cast<Int64>(value));

            bigNum = -static_cast<Int64>(bigNum);    // negate
        }

        for (unsigned base = 2; base <= 36; ++base) {
            char tBuffer[100], gBuffer[100];
            char *result;

            char *rear = gBuffer + 99;
            *rear = 0;
            char *front = u::generateStringRep(rear, bigNum, sign, base);
            const unsigned len = static_cast<unsigned>(rear - front);
            ASSERTV(len, len <= 64U + sign);

            Uint64 cmpResult;
            int rc = u::parseInt(&cmpResult, front, rear, base);
            ASSERT(0 <= rc);
            if (sign) {
                ASSERT(0 == rc);
                const Int64 iCmpResult = cmpResult;
                ASSERT(iCmpResult < 0);

                const bool test = iCmpResult == static_cast<Int64>(value);
                if (!test) { P_(name); P_(bigNum); P_(sign); P_(iCmpResult);
                             P_(front); P_(base);  P_(value); P(cmpResult); }
                ASSERT(test);
            }
            else {
                ASSERT(0 <= value);

                ASSERTV(value, bigNum, sign, cmpResult,
                                      cmpResult == static_cast<Uint64>(value));
            }

            // 'addLen' will be a random number in the range '[ 0 .. 2 ]'.  If
            // it's 0 (25% chance) there won't be enough room for the result.

            unsigned addLen = static_cast<unsigned>(mmixRand64()) & 3;
            addLen = std::min(addLen, 2U);

            char *tail = tBuffer + std::max<int>(len - 1 + addLen, 1);
            for (; tail <= tBuffer + len; ++tail) {
                memset(tBuffer, '*', tail - tBuffer);

                result = Util::toChars(tBuffer, tail, value, base);
                if (tail < tBuffer + len) {
                    if (veryVeryVeryVerbose) printf("too short");

                    ASSERTV(name, value, 0 == result);
                    ASSERTV(name, 0 == std::memcmp(tBuffer + sign,
                                                   starBuffer,
                                                   tail - sign - tBuffer));
                }
                else {
                    if (veryVeryVeryVerbose) printf("long enough");

                    ASSERTV(name, bigNum, tail - tBuffer, result);
                    ASSERTV(name, value, tBuffer + len == result);
                    ASSERTV(name, value, 0 == memcmp(front, tBuffer, len));
                    ASSERTV(name, value, 0 == memcmp(result,
                                                     starBuffer,
                                                     tail - (tBuffer + len)));

                    if (bigNum == 0) {
                        ASSERT(1 == len);
                        ASSERT('0' == *tBuffer);
                    }
                    else {
                        if (sign) {
                            ASSERTV(name, value, '-' == *tBuffer);
                            ASSERTV(name, value, '0' != tBuffer[1]);
                        }
                        else {
                            ASSERTV(name, value, '-' != *tBuffer);
                            ASSERTV(name, value, '0' != *tBuffer);
                        }
                    }
                }
            }
        }
    }
}

template <class TYPE>
void testValue(const Uint64 numArg, unsigned base)
    // Assign the specified 'numArg', and, in the case of signed types, the
    // negation of 'numArg', to a variable of type 'TYPE', and test the
    // function under test on it in the specified 'base'.  The testing is
    // performed by checks with the 'ASSERT' and 'ASSERTV' macros, which will
    // produce traces and cause the program to return a non-zero value
    // (indicating failure) if the expressions evaluated by the macros evaluate
    // 'false'.  This function returns without doing any checks if 'numArg' is
    // too large to be represented in a 'TYPE' variable.  This function uses
    // 'u::generateStringRep' as an oracle function whose result is compared to
    // the functions under test, and uses 'u::parseInt' as an inverse oracle
    // function.  The function under test is called only with 'base'.  For
    // bases 8, 10, and 16, 'sprintf' is also used as a redundant oracle.  If
    // 'native_std::to_chars' is available, it is also used as a redundant (but
    // highly reliable) oracle.
{
    const char *name = bsls::NameOf<TYPE>().name();

    const bool signedType = static_cast<TYPE>(-1) < 0;

    if (signedType) {
        Uint64 flipMin = static_cast<Uint64>(numeric_limits<TYPE>::min());
        flipMin = ~flipMin + 1;
        if (flipMin < numArg) {
            return;                                                   // RETURN
        }
    }
    else if (static_cast<Uint64>(numeric_limits<TYPE>::max()) < numArg) {
        return;                                                       // RETURN
    }

    for (int ii = 0; ii < 2; ++ii) {
        bool flip = signedType && !ii && numArg;

        TYPE num = static_cast<TYPE>(flip ? ~numArg + 1
                                          :  numArg);

        static const char starBuffer[] = {
                        "**************************************************"
                        "**************************************************" };

        char tBuffer[100], gBuffer[100];
        char *result;

        char *rear = gBuffer + 99;
        *rear = 0;
        char *front = u::generateStringRep(rear, numArg, flip, base);
        const size_t len = rear - front;
        ASSERTV(len, len <= 64U + flip);

        if (10 == base || (!flip && (8 == base || 16 == base))) {
            char sBuffer[100];

            if (flip) {
                Int64 signedArg = numArg;
                signedArg = -signedArg;

                sprintf(sBuffer, "%lld", signedArg);
            }
            else {
                const char *fmt = 10 == base
                                ? "%llu"
                                : 8 == base
                                ? "%llo"
                                : "%llx";
                sprintf(sBuffer, fmt, numArg);
            }

            ASSERT(std::strlen(sBuffer) == len);
            ASSERT(!std::strcmp(sBuffer, front));
            ASSERT(!std::memcmp(sBuffer, front, len));
        }

        Uint64 cmpResult;
        int rc = u::parseInt(&cmpResult, front, rear, base);
        ASSERT(0 <= rc);
        if (flip) {
            ASSERT(0 == rc);
            const Int64 iCmpResult = cmpResult;

            const bool test = iCmpResult == static_cast<Int64>(num);
            if (!test) { P_(name); P_(numArg); P_(flip); P_(iCmpResult);
                         P_(front); P_(base);  P_(num); P(cmpResult); }
            ASSERT(test);
        }
        else {
            ASSERT(0 <= num);

            ASSERTV(num, numArg, flip, cmpResult,
                                        cmpResult == static_cast<Uint64>(num));
        }

        for (char *tail = std::max(tBuffer + 1, tBuffer + len - 2);
                                            tail < tBuffer + len + 2; ++tail) {
            memset(tBuffer, '*', tail - tBuffer);

            result = Util::toChars(tBuffer, tail, num, base);
            if (tail < tBuffer + len) {
                ASSERTV(name, num, 0 == result);
                ASSERTV(name, 0 == std::memcmp(tBuffer + flip,
                                               starBuffer,
                                               tail - flip - tBuffer));
            }
            else {
                ASSERTV(name, numArg, flip, tail - tBuffer, result);
                ASSERTV(name, num, len, base, result - tBuffer,
                                                      tBuffer + len == result);
                ASSERTV(name, num, 0 == memcmp(result,
                                               starBuffer,
                                               tail - (tBuffer + len)));
                *result = 0;
                ASSERTV(name, num, front, tBuffer,
                                             0 == memcmp(front, tBuffer, len));
                rc = u::parseInt(&cmpResult, tBuffer, result, base);
                if (signedType && flip) {
                    ASSERT(0 == rc);
                    Int64 iCmpResult = cmpResult;
                    ASSERT(iCmpResult ==  static_cast<Int64>(num));
                }
                else {
                    ASSERT(cmpResult == static_cast<Uint64>(num));
                }

                if (numArg != 0) {
                    if (flip) {
                        ASSERTV(name, num, '-' == *tBuffer);
                        ASSERTV(name, num, '0' != tBuffer[1]);
                    }
                    else {
                        ASSERTV(name, num, '0' != *tBuffer);
                    }
                }

#if             defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                {
                    char nBuffer[100];
                    char *nTail = nBuffer + (tail - tBuffer);
                    const std::to_chars_result nResult =
                                      std::to_chars(nBuffer, nTail, num, base);
                    if (result) {
                        ASSERT(result - tBuffer == nResult.ptr - nBuffer);
                        ASSERT(0 == memcmp(tBuffer,
                                           nBuffer,
                                           result - tBuffer));
                        ASSERT(nResult.ec == std::errc());
                    }
                    else {
                        ASSERT(nResult.ec  == std::errc::value_too_large);
                        ASSERT(nResult.ptr == nTail);
                    }
                }
#endif
            }
        }

        if (!signedType || 0 == num ||
                   static_cast<Uint64>(numeric_limits<TYPE>::max()) < numArg) {
            return;                                                   // RETURN
        }
    }
}

void testValueMeta(Uint64 num, unsigned base)
    // Run 'u::testValue<TYPE>' on the specified 'num' and the specified 'base'
    // for every integral fundamental type, signed and unsigned, up to 64 bits
    // long.
{
#undef  TEST
#define TEST(type)    u::testValue<type>(num, base)

    TEST(char);
    TEST(signed char);
    TEST(unsigned char);

    TEST(short);
    TEST(unsigned short);

    TEST(int);
    TEST(unsigned int);

    TEST(Uint64);
    TEST(Int64);

#undef TEST
}

const Uint64 uint64Max = ~0ULL;    // max value a 'Uint64' can represent

}  // close namespace u
}  // close unnamed namespace

//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Demonstrating Writing a Number to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that writes an 'int' to a 'streambuf'.
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
// Next, we call the function:
//..
        char *ret = bslalg::NumericFormatterUtil::toChars(buffer,
                                                          buffer + 11,
                                                          value);
//..
// Then, we check that the buffer was long enough, which should always be the
// case:
//..
        ASSERT(0 != ret);
//..
// Now, we check that 'ret' is in the range
// '[ buffer + 1, buffer + sizeof(buffer) ]', which will always be the case if
// 'toChars' succeeded.
//..
        ASSERT(buffer <  ret);
        ASSERT(ret    <= buffer + sizeof(buffer));
//..
// Finally, we write our buffer to the 'streambuf':
//..
        result->sputn(buffer, ret - buffer);
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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Create the function 'writeJsonScalar' which is the usage example,
        //:   and call it a few times here to make sure it functions as
        //:   expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // This is not part of the usage example, here we just test that the
        // function defined by the usage example works.

        std::ostringstream  oss;
        std::streambuf     *sb = oss.rdbuf();

        writeJsonScalar(sb, 0);
        ASSERT("0" == oss.str());

        oss.str("");

        writeJsonScalar(sb, 99);
        ASSERT("99" == oss.str());

        oss.str("");

        writeJsonScalar(sb, -1234567890);    // worst case int, max string len
        ASSERT("-1234567890" == oss.str());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RANDOM VALUES TEST
        //
        // Concerns:
        //: 1 That the function under test performs well when translating
        //:   random values.
        //
        // Plan:
        //: 1 Use an MMIX-style random number generator to generate random
        //:   values.
        //:
        //: 2 This test tests the function when called with all unsigned
        //:   fundamental types.
        //:
        //: 3 Each random value is tested with all bases in the range
        //:   '[ 2 .. 36 ]', which is all bases supported by the function under
        //:   test.
        //
        // Testing:
        //   char *toChars(char *, char *, TYPE, int); // Random Unsigned
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM VALUES TEST -- UNSIGNED TYPESS\n"
                            "=====================================\n");

        Uint64 iterationsForByte = 200;
        if (verbose) {
            Uint64 it = std::atoi(argv[2]);
            if (it) {
                iterationsForByte = it;
                P(iterationsForByte);
            }
        }

        // do the unsigned types

        u::randTest<unsigned char>( iterationsForByte);
        u::randTest<unsigned short>(iterationsForByte);
        u::randTest<unsigned int>(  iterationsForByte);
        u::randTest<Uint64>(        iterationsForByte);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RANDOM VALUES TEST
        //
        // Concerns:
        //: 1 That the function under test performs well when translating
        //:   random values.
        //
        // Plan:
        //: 1 Use an MMIX-style random number generator to generate random
        //:   values.
        //:
        //: 2 This test tests the function when called with all signed
        //:   fundamental types.
        //:
        //: 3 Each random value is tested with all bases in the range
        //:   '[ 2 .. 36 ]', which is all bases supported by the function under
        //:   test.
        //
        // Testing:
        //   char *toChars(char *, char *, TYPE, int); // Random Signed
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM VALUES TEST -- SIGNED TYPESS\n"
                            "===================================\n");

        Uint64 iterationsForByte = 200;
        if (verbose) {
            Uint64 it = std::atoi(argv[2]);
            if (it) {
                iterationsForByte = it;
                P(iterationsForByte);
            }
        }

        // do the signed types

        u::randTest<char>(       iterationsForByte);
        u::randTest<signed char>(iterationsForByte);
        u::randTest<short>(      iterationsForByte);
        u::randTest<int>(        iterationsForByte);
        u::randTest<Int64>(      iterationsForByte);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CORNER CASES TEST
        //
        // Concerns:
        //: 1 The function works as expected for all "interesting" corner cases
        //:   of input value and base where "interesting" for a given base is:
        //:
        //:   o All input values that are a power of the given base:
        //:     '[ 1, base, base**2, base**3, ... ceiling ]' and 'ceiling' is
        //:     the largest value in the series representable by the 'Uint64'
        //:     type.
        //:
        //:   o A value of 0.
        //:
        //:   o The maximum and minimum values that can be represented by any
        //:     integral fundamental type.
        //:
        //:   o Also, 1 more and 1 less that each input value in the above
        //:     series.  Note the variable in question is unsigned, and in some
        //:     cases adding or subtracting 1 may overflow or underflow, but
        //:     this is not undefined behavior.
        //:
        //: 2 The function works as expected for the series (see C-1) for every
        //:   supported base (i.e., '[2 .. 36]').
        //:
        //: 3 The function works as expected for:
        //:
        //:   o Each of the supported integer types:
        //:     o 'char'
        //:     o 'signed char'
        //:     o 'unsigned char'
        //:     o 'short'
        //:     o 'unsigned short'
        //:     o 'int'
        //:     o 'unsigned int'
        //:     o 'Uint64'
        //:     o 'Int64'
        //:
        //:   o And, for signed types each of the input values in the test
        //:     series is also tested for its negated value
        //:
        //:   o And, the input values of each type are curtailed for the
        //:     maximum (and minimum) value of the type.
        //
        // Plan:
        //: 1 Have a function 'testValueMeta' that will assign the 'value'
        //:   passed to it to all integral types capable of holding it, and
        //:   also do the negative value, if the type is a signed type, and
        //:   then call 'toChar's to print out the variables to strings, and
        //:   check that the strings output are correct.
        //:
        //: 2 For good measure, for each base we print the strings with, we
        //:   test all powers of all supported bases.
        //:
        //: 3 For each base, test 0.
        //:
        //: 4 For each base, for the size of every integral type from 'char' to
        //:   'bsls::Types::Uint64', test the maximum and minimum values the
        //:   type can represent.
        //:
        //: 5 For each value 'N' that is tested, also test 'N + 1' and 'N - 1'.
        //
        // Testing:
        //   char *toChars(char *, char *, TYPE, int); // Corner Cases
        // --------------------------------------------------------------------

        if (verbose) printf("CORNER CASES TEST\n"
                            "=================\n");

        for (int base = 2; base <= 36; ++base) {
            if (veryVerbose) { T2_; P_(base); Q(zero:); }

            u::testValueMeta(       -1,               base);
            u::testValueMeta(        0,               base);
            u::testValueMeta(        1,               base);

            if (veryVerbose) { T2_; P_(base); Q(escalation loop:); }

            for (int escalation = 2; escalation <= 36; ++escalation) {
                Uint64 ceiling = ~0ULL / escalation;

                if (veryVeryVerbose) { T2_; T2_; P_(escalation); P(ceiling); }

                for (Uint64 ii = 1, prev = 0; prev <= ceiling;
                                                 prev = ii, ii *= escalation) {
                    if (veryVeryVeryVerbose) {
                        T2_; T2_; P(ii);
                    }

                    u::testValueMeta(ii - 1,          base);
                    u::testValueMeta(ii,              base);
                    u::testValueMeta(ii + 1,          base);
                }
            }

            if (veryVerbose) { T2_; P_(base); Q(max min loop:); }

            for (int bits = 8; bits <= 64; bits *= 2) {
                const Uint64 maxUnsigned = bits < 64
                                         ? (1ULL << bits) - 1
                                         : ~0ULL;
                const Uint64 minSigned   = 1ULL << (bits - 1);
                const Int64  sMinSigned  = minSigned;

                if (veryVeryVerbose) {
                    printf("    maxUnsigned: %llu = 0x%llx\n",
                                                     maxUnsigned, maxUnsigned);
                    printf("    minSigned:   %lld = 0x%llx\n",
                                                        sMinSigned, minSigned);
                }

                u::testValueMeta(    maxUnsigned - 1, base);
                u::testValueMeta(    maxUnsigned,     base);    // UINT_MAX
                u::testValueMeta(    maxUnsigned + 1, base);    // 0

                u::testValueMeta(    minSigned   - 1, base);    // INT_MAX
                u::testValueMeta(    minSigned,       base);    // INT_MIN
                u::testValueMeta(    minSigned   + 1, base);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // QUALITY TEST & ACCURACY ON SOME BASES TEST
        //
        // Concern:
        //: 1 That, in all supported bases,
        //:   o negative numbers are always output beginning with '-'.
        //:
        //:   o positive numbers don't contain '-'.
        //:
        //:   o output other than an appropriate beginning '-' consists
        //:     entirely of digits in the range '[ '0' ..  <base> )', where
        //:     digits above '9' are represented by alphabetical characters.
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
        //:   o If we are on a platform that supports '<charconv>', apply
        //:     'std::to_chars' to the unsigned and signed values and observe
        //:     that the output strings match.
        //:
        //: 3 Iterate through the table of numbers.  For each number:
        //:   o write the number with both 'sprintf' and 'toChars' as unsigned
        //:     decimal, hex, and octal strings, and using 'sprintf' as an
        //:     "oracle" for comparing the resulting strings for perfect
        //:     accuracy.
        //:
        //:   o Assign it to a signed type, with it with both 'sprintf' and
        //:     'toChars', and observe they match to test for perfect
        //:     accuracy.  Then negate the signed type and repeat the
        //:     experiment.
        //:
        //: 4 Iterate through the table and for unsigned decimal, hex, or
        //:   octal, if the string in the table is of that radix, compare the
        //:   string in the table with the string generated by 'toChars' to
        //:   check that they match for perfect accuracy.
        //:
        //: 5 Note that there is some redundancy of testing 0 multiple times,
        //:   but this was maintained to preserve the symmetry of the test
        //:   pattern.
        //
        // Testing:
        //   char *toChars(char *, char *, TYPE, int); // Table-Driven
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

        if (verbose) printf("quality tests (not testing for accuracy)\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            Uint64          value  = data.d_value;

            for (unsigned base = 2; base <= 36; ++base) {
                char toCharsBuffer[66];
                char *result;

                result = Util::toChars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       value,
                                       base);
                ASSERT(result);
                ASSERT(toCharsBuffer < result);
                ASSERT(result < toCharsBuffer + sizeof(toCharsBuffer));
                for (const char *pc = toCharsBuffer; pc < result; ++pc) {
                    unsigned digit = '0' <= *pc && *pc <= '9'
                                   ? *pc - '0'
                                   : 'a' <= *pc && *pc <= 'z'
                                   ? 10 + *pc - 'a'
                                   : 100;
                    ASSERT(digit < base);
                }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                char iBuffer[1000];
                std::to_chars_result iSts = std::to_chars(
                                                     iBuffer,
                                                     iBuffer + sizeof(iBuffer),
                                                     value,
                                                     base);
                ASSERT(std::errc() == iSts.ec);
                ASSERT(iBuffer < iSts.ptr);
                ASSERT(iSts.ptr < iBuffer + sizeof(iBuffer));
                ASSERT(iSts.ptr - iBuffer == result - toCharsBuffer);
                ASSERT(0 == std::memcmp(toCharsBuffer,
                                        iBuffer,
                                        iSts.ptr - iBuffer));
#endif

                if (value != 0) {
                    const Int64 len = result - toCharsBuffer;
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

                    result = Util::toChars(
                                         toCharsBuffer,
                                         toCharsBuffer + sizeof(toCharsBuffer),
                                         value,
                                         base);
                    ASSERT(result);
                    ASSERT(toCharsBuffer < result);
                    ASSERT(result < toCharsBuffer + sizeof(toCharsBuffer));

                    svalue = -svalue;
                }

                char  toCharsBufferS[66];
                char *resultS;

                if (0 <= svalue) {
                    resultS = Util::toChars(
                                        toCharsBufferS,
                                        toCharsBufferS + sizeof(toCharsBuffer),
                                        svalue,
                                        base);
                    ASSERT(resultS);
                    ASSERT(resultS <
                                      toCharsBufferS + sizeof(toCharsBufferS));
                    ASSERT(resultS - toCharsBufferS == result - toCharsBuffer);
                    ASSERT(!std::memcmp(toCharsBuffer,
                                        toCharsBufferS,
                                        result - toCharsBuffer));

                    if (0 == svalue) {
                        continue;
                    }

                    svalue = -svalue;
                }
                ASSERT(svalue < 0);

                resultS = Util::toChars(toCharsBufferS,
                                        toCharsBufferS + sizeof(toCharsBuffer),
                                        svalue,
                                        base);
                ASSERT(resultS);
                ASSERT(toCharsBufferS < resultS);
                ASSERT(resultS < toCharsBufferS + sizeof(toCharsBufferS));
                ASSERT('-' == toCharsBufferS[0]);
                ASSERT(resultS - toCharsBufferS == 1 + result - toCharsBuffer);
                ASSERT(!std::memcmp(toCharsBuffer,
                                    toCharsBufferS + 1,
                                    result - toCharsBuffer));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                iSts = std::to_chars(iBuffer,
                                     iBuffer + sizeof(iBuffer),
                                     svalue,
                                     base);
                ASSERT(std::errc() == iSts.ec);
                ASSERT(iSts.ptr - iBuffer == resultS - toCharsBufferS);
                ASSERT(0 == std::memcmp(toCharsBufferS,
                                        iBuffer,
                                        iSts.ptr - iBuffer));
#endif
            }
        }

        if (verbose) printf("Accuracy 1: sprintf comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;

            char  sprintfBuffer[40];
            char  toCharsBuffer[40];
            char *result;

            sprintf(sprintfBuffer, "%llu", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   10);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));

            for (Int64 sValue = VALUE, ii = 0; ii < 2; ++ii, sValue = -sValue){
                sprintf(sprintfBuffer, "%lld", sValue);
                result = Util::toChars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       sValue,
                                       10);
                ASSERT(result);
                *result = 0;
                ASSERTV(LINE, !std::strcmp(sprintfBuffer,
                                           toCharsBuffer));
            }

            sprintf(sprintfBuffer, "%llo", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   8);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));

            sprintf(sprintfBuffer, "%llx", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   16);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, !std::strcmp(sprintfBuffer, toCharsBuffer));
        }

        if (verbose) printf("Accuracy 2: Source string comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;
            const char     *STRING = data.d_string_p;
            const unsigned  BASE   = data.d_base;

            char  toCharsBuffer[40];
            char *result;

            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer),
                                   VALUE,
                                   BASE);
            ASSERT(result);
            *result = 0;
            std::string s = STRING;
            ASSERTV(LINE, s == toCharsBuffer);
            if (0 < VALUE && VALUE <= (1ULL << 63)) {
                const Int64 sValue = -VALUE;
                result = Util::toChars(toCharsBuffer,
                                       toCharsBuffer + sizeof(toCharsBuffer),
                                       sValue,
                                       BASE);
                *result = 0;

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
        //: 1 Perform and ad-hoc test of the function under test.
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
            sprintf(sprintfBuf, "%d", value);
            const IntPtr len = std::strlen(sprintfBuf);

            char toCharsBuf[20];
            char *result =
                         bslalg::NumericFormatterUtil::toChars(toCharsBuf,
                                                               toCharsBuf + 19,
                                                               value,
                                                               10);
            ASSERT(result);
            ASSERT(result - toCharsBuf == len);
            *result = 0;
            ASSERTV(value, toCharsBuf, sprintfBuf,
                                         !std::strcmp(toCharsBuf, sprintfBuf));
        }
      } break;
#if defined(BSLS_LIBRARYFEATURES_HAS_INT_CPP17_CHARCONV)
      case -1: {
        // --------------------------------------------------------------------
        // SPEED TRIAL
        //
        // Concern:
        //: 1 Measure how the performance of the function under test compares
        //:   to 'std::to_chars' and 'sprintf'.
        //
        // Plan:
        //: 1 Call the 3 functions many times and time them using
        //:   'bsls_stopwatch'.
        //
        //  Results on Linux host: bldlnx-ob-009
        //  Build: UFID: opt_exc_mt_cpp17 Compiler: /opt/bb/bin/g++-10
        //
        //  Digits:  1: Base:  8:       toChars: 32 bit: 15.2477 nsec
        //  Digits:  1: Base:  8: std::to_chars: 32 bit: 5.2592 nsec
        //  Digits:  1: Base:  8:       sprintf: 32 bit: 118.942 nsec
        //
        //  Digits:  2: Base:  8:       toChars: 32 bit: 17.9973 nsec
        //  Digits:  2: Base:  8: std::to_chars: 32 bit: 5.62915 nsec
        //  Digits:  2: Base:  8:       sprintf: 32 bit: 118.562 nsec
        //
        //  Digits:  3: Base:  8:       toChars: 32 bit: 20.5269 nsec
        //  Digits:  3: Base:  8: std::to_chars: 32 bit: 6.14909 nsec
        //  Digits:  3: Base:  8:       sprintf: 32 bit: 117.392 nsec
        //
        //  Digits:  4: Base:  8:       toChars: 32 bit: 23.2965 nsec
        //  Digits:  4: Base:  8: std::to_chars: 32 bit: 6.33905 nsec
        //  Digits:  4: Base:  8:       sprintf: 32 bit: 118.392 nsec
        //
        //  Digits:  5: Base:  8:       toChars: 32 bit: 27.0926 nsec
        //  Digits:  5: Base:  8: std::to_chars: 32 bit: 7.2307 nsec
        //  Digits:  5: Base:  8:       sprintf: 32 bit: 120.092 nsec
        //
        //  Digits:  6: Base:  8:       toChars: 32 bit: 29.3617 nsec
        //  Digits:  6: Base:  8: std::to_chars: 32 bit: 7.5982 nsec
        //  Digits:  6: Base:  8:       sprintf: 32 bit: 122.072 nsec
        //
        //  Digits:  7: Base:  8:       toChars: 32 bit: 33.3025 nsec
        //  Digits:  7: Base:  8: std::to_chars: 32 bit: 8.034 nsec
        //  Digits:  7: Base:  8:       sprintf: 32 bit: 122.194 nsec
        //
        //  Digits:  8: Base:  8:       toChars: 32 bit: 36.1149 nsec
        //  Digits:  8: Base:  8: std::to_chars: 32 bit: 8.20035 nsec
        //  Digits:  8: Base:  8:       sprintf: 32 bit: 122.159 nsec
        //
        //  Digits:  9: Base:  8:       toChars: 32 bit: 39.5342 nsec
        //  Digits:  9: Base:  8: std::to_chars: 32 bit: 9.00638 nsec
        //  Digits:  9: Base:  8:       sprintf: 32 bit: 124.823 nsec
        //
        //  Digits: 10: Base:  8:       toChars: 32 bit: 42.6108 nsec
        //  Digits: 10: Base:  8: std::to_chars: 32 bit: 8.93934 nsec
        //  Digits: 10: Base:  8:       sprintf: 32 bit: 122.99 nsec
        //
        //  Digits: 11: Base:  8:       toChars: 32 bit: 44.9369 nsec
        //  Digits: 11: Base:  8: std::to_chars: 32 bit: 9.79696 nsec
        //  Digits: 11: Base:  8:       sprintf: 32 bit: 125.297 nsec
        //
        //  Digits:  1: Base:  8:       toChars: 64 bit: 19.0871 nsec
        //  Digits:  1: Base:  8: std::to_chars: 64 bit: 7.87881 nsec
        //  Digits:  1: Base:  8:       sprintf: 64 bit: 128.27 nsec
        //
        //  Digits:  2: Base:  8:       toChars: 64 bit: 21.3968 nsec
        //  Digits:  2: Base:  8: std::to_chars: 64 bit: 8.19875 nsec
        //  Digits:  2: Base:  8:       sprintf: 64 bit: 128.43 nsec
        //
        //  Digits:  3: Base:  8:       toChars: 64 bit: 24.9763 nsec
        //  Digits:  3: Base:  8: std::to_chars: 64 bit: 9.55857 nsec
        //  Digits:  3: Base:  8:       sprintf: 64 bit: 127.291 nsec
        //
        //  Digits:  4: Base:  8:       toChars: 64 bit: 26.896 nsec
        //  Digits:  4: Base:  8: std::to_chars: 64 bit: 10.0385 nsec
        //  Digits:  4: Base:  8:       sprintf: 64 bit: 130.201 nsec
        //
        //  Digits:  5: Base:  8:       toChars: 64 bit: 29.9229 nsec
        //  Digits:  5: Base:  8: std::to_chars: 64 bit: 11.2211 nsec
        //  Digits:  5: Base:  8:       sprintf: 64 bit: 129.332 nsec
        //
        //  Digits:  6: Base:  8:       toChars: 64 bit: 32.6152 nsec
        //  Digits:  6: Base:  8: std::to_chars: 64 bit: 12.2232 nsec
        //  Digits:  6: Base:  8:       sprintf: 64 bit: 130.221 nsec
        //
        //  Digits:  7: Base:  8:       toChars: 64 bit: 35.5443 nsec
        //  Digits:  7: Base:  8: std::to_chars: 64 bit: 13.3291 nsec
        //  Digits:  7: Base:  8:       sprintf: 64 bit: 132.013 nsec
        //
        //  Digits:  8: Base:  8:       toChars: 64 bit: 38.1054 nsec
        //  Digits:  8: Base:  8: std::to_chars: 64 bit: 13.6712 nsec
        //  Digits:  8: Base:  8:       sprintf: 64 bit: 133.756 nsec
        //
        //  Digits:  9: Base:  8:       toChars: 64 bit: 40.7261 nsec
        //  Digits:  9: Base:  8: std::to_chars: 64 bit: 15.1671 nsec
        //  Digits:  9: Base:  8:       sprintf: 64 bit: 133.867 nsec
        //
        //  Digits: 10: Base:  8:       toChars: 64 bit: 43.5886 nsec
        //  Digits: 10: Base:  8: std::to_chars: 64 bit: 14.9361 nsec
        //  Digits: 10: Base:  8:       sprintf: 64 bit: 134.286 nsec
        //
        //  Digits: 11: Base:  8:       toChars: 64 bit: 46.9955 nsec
        //  Digits: 11: Base:  8: std::to_chars: 64 bit: 15.8795 nsec
        //  Digits: 11: Base:  8:       sprintf: 64 bit: 132.488 nsec
        //
        //  Digits: 12: Base:  8:       toChars: 64 bit: 49.5784 nsec
        //  Digits: 12: Base:  8: std::to_chars: 64 bit: 16.4229 nsec
        //  Digits: 12: Base:  8:       sprintf: 64 bit: 131.943 nsec
        //
        //  Digits: 13: Base:  8:       toChars: 64 bit: 52.5967 nsec
        //  Digits: 13: Base:  8: std::to_chars: 64 bit: 17.3656 nsec
        //  Digits: 13: Base:  8:       sprintf: 64 bit: 134.466 nsec
        //
        //  Digits: 14: Base:  8:       toChars: 64 bit: 54.4012 nsec
        //  Digits: 14: Base:  8: std::to_chars: 64 bit: 17.7171 nsec
        //  Digits: 14: Base:  8:       sprintf: 64 bit: 135.448 nsec
        //
        //  Digits: 15: Base:  8:       toChars: 64 bit: 57.7411 nsec
        //  Digits: 15: Base:  8: std::to_chars: 64 bit: 18.8671 nsec
        //  Digits: 15: Base:  8:       sprintf: 64 bit: 135.219 nsec
        //
        //  Digits: 16: Base:  8:       toChars: 64 bit: 60.1708 nsec
        //  Digits: 16: Base:  8: std::to_chars: 64 bit: 19.437 nsec
        //  Digits: 16: Base:  8:       sprintf: 64 bit: 134.05 nsec
        //
        //  Digits: 17: Base:  8:       toChars: 64 bit: 64.1203 nsec
        //  Digits: 17: Base:  8: std::to_chars: 64 bit: 21.4467 nsec
        //  Digits: 17: Base:  8:       sprintf: 64 bit: 138.619 nsec
        //
        //  Digits: 18: Base:  8:       toChars: 64 bit: 66.05 nsec
        //  Digits: 18: Base:  8: std::to_chars: 64 bit: 20.8968 nsec
        //  Digits: 18: Base:  8:       sprintf: 64 bit: 137.759 nsec
        //
        //  Digits: 19: Base:  8:       toChars: 64 bit: 68.0696 nsec
        //  Digits: 19: Base:  8: std::to_chars: 64 bit: 22.3566 nsec
        //  Digits: 19: Base:  8:       sprintf: 64 bit: 140.689 nsec
        //
        //  Digits: 20: Base:  8:       toChars: 64 bit: 71.6691 nsec
        //  Digits: 20: Base:  8: std::to_chars: 64 bit: 22.4766 nsec
        //  Digits: 20: Base:  8:       sprintf: 64 bit: 135.669 nsec
        //
        //  Digits: 21: Base:  8:       toChars: 64 bit: 74.0487 nsec
        //  Digits: 21: Base:  8: std::to_chars: 64 bit: 23.5964 nsec
        //  Digits: 21: Base:  8:       sprintf: 64 bit: 140.459 nsec
        //
        //  Digits:  1: Base: 10:       toChars: 32 bit: 6.58899 nsec
        //  Digits:  1: Base: 10: std::to_chars: 32 bit: 6.01909 nsec
        //  Digits:  1: Base: 10:       sprintf: 32 bit: 111.493 nsec
        //
        //  Digits:  2: Base: 10:       toChars: 32 bit: 6.79897 nsec
        //  Digits:  2: Base: 10: std::to_chars: 32 bit: 6.16906 nsec
        //  Digits:  2: Base: 10:       sprintf: 32 bit: 112.473 nsec
        //
        //  Digits:  3: Base: 10:       toChars: 32 bit: 10.2984 nsec
        //  Digits:  3: Base: 10: std::to_chars: 32 bit: 6.36903 nsec
        //  Digits:  3: Base: 10:       sprintf: 32 bit: 114.063 nsec
        //
        //  Digits:  4: Base: 10:       toChars: 32 bit: 11.1483 nsec
        //  Digits:  4: Base: 10: std::to_chars: 32 bit: 6.639 nsec
        //  Digits:  4: Base: 10:       sprintf: 32 bit: 115.252 nsec
        //
        //  Digits:  5: Base: 10:       toChars: 32 bit: 12.7981 nsec
        //  Digits:  5: Base: 10: std::to_chars: 32 bit: 8.47871 nsec
        //  Digits:  5: Base: 10:       sprintf: 32 bit: 120.042 nsec
        //
        //  Digits:  6: Base: 10:       toChars: 32 bit: 13.7379 nsec
        //  Digits:  6: Base: 10: std::to_chars: 32 bit: 9.60854 nsec
        //  Digits:  6: Base: 10:       sprintf: 32 bit: 119.782 nsec
        //
        //  Digits:  7: Base: 10:       toChars: 32 bit: 14.7378 nsec
        //  Digits:  7: Base: 10: std::to_chars: 32 bit: 10.8583 nsec
        //  Digits:  7: Base: 10:       sprintf: 32 bit: 122.801 nsec
        //
        //  Digits:  8: Base: 10:       toChars: 32 bit: 15.1777 nsec
        //  Digits:  8: Base: 10: std::to_chars: 32 bit: 11.0783 nsec
        //  Digits:  8: Base: 10:       sprintf: 32 bit: 124.901 nsec
        //
        //  Digits:  9: Base: 10:       toChars: 32 bit: 17.9473 nsec
        //  Digits:  9: Base: 10: std::to_chars: 32 bit: 13.138 nsec
        //  Digits:  9: Base: 10:       sprintf: 32 bit: 124.601 nsec
        //
        //  Digits: 10: Base: 10:       toChars: 32 bit: 17.9383 nsec
        //  Digits: 10: Base: 10: std::to_chars: 32 bit: 13.3619 nsec
        //  Digits: 10: Base: 10:       sprintf: 32 bit: 127.802 nsec
        //
        //  Digits: 11: Base: 10:       toChars: 32 bit: 17.9709 nsec
        //  Digits: 11: Base: 10: std::to_chars: 32 bit: 13.7666 nsec
        //  Digits: 11: Base: 10:       sprintf: 32 bit: 129.854 nsec
        //
        //  Digits:  1: Base: 10:       toChars: 64 bit: 11.1483 nsec
        //  Digits:  1: Base: 10: std::to_chars: 64 bit: 7.38887 nsec
        //  Digits:  1: Base: 10:       sprintf: 64 bit: 115.662 nsec
        //
        //  Digits:  2: Base: 10:       toChars: 64 bit: 11.3683 nsec
        //  Digits:  2: Base: 10: std::to_chars: 64 bit: 7.59885 nsec
        //  Digits:  2: Base: 10:       sprintf: 64 bit: 118.652 nsec
        //
        //  Digits:  3: Base: 10:       toChars: 64 bit: 14.6678 nsec
        //  Digits:  3: Base: 10: std::to_chars: 64 bit: 19.797 nsec
        //  Digits:  3: Base: 10:       sprintf: 64 bit: 123.631 nsec
        //
        //  Digits:  4: Base: 10:       toChars: 64 bit: 15.5776 nsec
        //  Digits:  4: Base: 10: std::to_chars: 64 bit: 22.1566 nsec
        //  Digits:  4: Base: 10:       sprintf: 64 bit: 129.91 nsec
        //
        //  Digits:  5: Base: 10:       toChars: 64 bit: 17.1974 nsec
        //  Digits:  5: Base: 10: std::to_chars: 64 bit: 40.2039 nsec
        //  Digits:  5: Base: 10:       sprintf: 64 bit: 137.109 nsec
        //
        //  Digits:  6: Base: 10:       toChars: 64 bit: 18.0772 nsec
        //  Digits:  6: Base: 10: std::to_chars: 64 bit: 43.8233 nsec
        //  Digits:  6: Base: 10:       sprintf: 64 bit: 146.688 nsec
        //
        //  Digits:  7: Base: 10:       toChars: 64 bit: 19.897 nsec
        //  Digits:  7: Base: 10: std::to_chars: 64 bit: 56.7314 nsec
        //  Digits:  7: Base: 10:       sprintf: 64 bit: 153.087 nsec
        //
        //  Digits:  8: Base: 10:       toChars: 64 bit: 20.0469 nsec
        //  Digits:  8: Base: 10: std::to_chars: 64 bit: 61.0807 nsec
        //  Digits:  8: Base: 10:       sprintf: 64 bit: 162.295 nsec
        //
        //  Digits:  9: Base: 10:       toChars: 64 bit: 22.5066 nsec
        //  Digits:  9: Base: 10: std::to_chars: 64 bit: 77.3083 nsec
        //  Digits:  9: Base: 10:       sprintf: 64 bit: 172.094 nsec
        //
        //  Digits: 10: Base: 10:       toChars: 64 bit: 40.9038 nsec
        //  Digits: 10: Base: 10: std::to_chars: 64 bit: 79.6979 nsec
        //  Digits: 10: Base: 10:       sprintf: 64 bit: 180.153 nsec
        //
        //  Digits: 11: Base: 10:       toChars: 64 bit: 42.2536 nsec
        //  Digits: 11: Base: 10: std::to_chars: 64 bit: 93.1958 nsec
        //  Digits: 11: Base: 10:       sprintf: 64 bit: 188.531 nsec
        //
        //  Digits: 12: Base: 10:       toChars: 64 bit: 59.9409 nsec
        //  Digits: 12: Base: 10: std::to_chars: 64 bit: 102.054 nsec
        //  Digits: 12: Base: 10:       sprintf: 64 bit: 192.701 nsec
        //
        //  Digits: 13: Base: 10:       toChars: 64 bit: 68.3596 nsec
        //  Digits: 13: Base: 10: std::to_chars: 64 bit: 122.791 nsec
        //  Digits: 13: Base: 10:       sprintf: 64 bit: 193.571 nsec
        //
        //  Digits: 14: Base: 10:       toChars: 64 bit: 87.3667 nsec
        //  Digits: 14: Base: 10: std::to_chars: 64 bit: 132.98 nsec
        //  Digits: 14: Base: 10:       sprintf: 64 bit: 198.24 nsec
        //
        //  Digits: 15: Base: 10:       toChars: 64 bit: 95.9354 nsec
        //  Digits: 15: Base: 10: std::to_chars: 64 bit: 150.087 nsec
        //  Digits: 15: Base: 10:       sprintf: 64 bit: 208.418 nsec
        //
        //  Digits: 16: Base: 10:       toChars: 64 bit: 111.013 nsec
        //  Digits: 16: Base: 10: std::to_chars: 64 bit: 157.196 nsec
        //  Digits: 16: Base: 10:       sprintf: 64 bit: 207.029 nsec
        //
        //  Digits: 17: Base: 10:       toChars: 64 bit: 119.792 nsec
        //  Digits: 17: Base: 10: std::to_chars: 64 bit: 176.423 nsec
        //  Digits: 17: Base: 10:       sprintf: 64 bit: 177.633 nsec
        //
        //  Digits: 18: Base: 10:       toChars: 64 bit: 137.779 nsec
        //  Digits: 18: Base: 10: std::to_chars: 64 bit: 191.271 nsec
        //  Digits: 18: Base: 10:       sprintf: 64 bit: 186.542 nsec
        //
        //  Digits: 19: Base: 10:       toChars: 64 bit: 144.358 nsec
        //  Digits: 19: Base: 10: std::to_chars: 64 bit: 209.348 nsec
        //  Digits: 19: Base: 10:       sprintf: 64 bit: 243.543 nsec
        //
        //  Digits:  1: Base: 16:       toChars: 32 bit: 15.4276 nsec
        //  Digits:  1: Base: 16: std::to_chars: 32 bit: 5.66914 nsec
        //  Digits:  1: Base: 16:       sprintf: 32 bit: 114.193 nsec
        //
        //  Digits:  2: Base: 16:       toChars: 32 bit: 17.6973 nsec
        //  Digits:  2: Base: 16: std::to_chars: 32 bit: 5.18921 nsec
        //  Digits:  2: Base: 16:       sprintf: 32 bit: 113.603 nsec
        //
        //  Digits:  3: Base: 16:       toChars: 32 bit: 20.8469 nsec
        //  Digits:  3: Base: 16: std::to_chars: 32 bit: 5.71914 nsec
        //  Digits:  3: Base: 16:       sprintf: 32 bit: 114.113 nsec
        //
        //  Digits:  4: Base: 16:       toChars: 32 bit: 23.63 nsec
        //  Digits:  4: Base: 16: std::to_chars: 32 bit: 5.71242 nsec
        //  Digits:  4: Base: 16:       sprintf: 32 bit: 115.769 nsec
        //
        //  Digits:  5: Base: 16:       toChars: 32 bit: 26.739 nsec
        //  Digits:  5: Base: 16: std::to_chars: 32 bit: 6.66467 nsec
        //  Digits:  5: Base: 16:       sprintf: 32 bit: 116.28 nsec
        //
        //  Digits:  6: Base: 16:       toChars: 32 bit: 29.7024 nsec
        //  Digits:  6: Base: 16: std::to_chars: 32 bit: 6.85349 nsec
        //  Digits:  6: Base: 16:       sprintf: 32 bit: 119.918 nsec
        //
        //  Digits:  7: Base: 16:       toChars: 32 bit: 33.448 nsec
        //  Digits:  7: Base: 16: std::to_chars: 32 bit: 7.74743 nsec
        //  Digits:  7: Base: 16:       sprintf: 32 bit: 117.098 nsec
        //
        //  Digits:  8: Base: 16:       toChars: 32 bit: 36.3328 nsec
        //  Digits:  8: Base: 16: std::to_chars: 32 bit: 8.24234 nsec
        //  Digits:  8: Base: 16:       sprintf: 32 bit: 120.409 nsec
        //
        //  Digits:  1: Base: 16:       toChars: 64 bit: 19.437 nsec
        //  Digits:  1: Base: 16: std::to_chars: 64 bit: 8.95864 nsec
        //  Digits:  1: Base: 16:       sprintf: 64 bit: 119.692 nsec
        //
        //  Digits:  2: Base: 16:       toChars: 64 bit: 21.5367 nsec
        //  Digits:  2: Base: 16: std::to_chars: 64 bit: 7.85881 nsec
        //  Digits:  2: Base: 16:       sprintf: 64 bit: 120.372 nsec
        //
        //  Digits:  3: Base: 16:       toChars: 64 bit: 25.7162 nsec
        //  Digits:  3: Base: 16: std::to_chars: 64 bit: 9.3386 nsec
        //  Digits:  3: Base: 16:       sprintf: 64 bit: 121.902 nsec
        //
        //  Digits:  4: Base: 16:       toChars: 64 bit: 27.3316 nsec
        //  Digits:  4: Base: 16: std::to_chars: 64 bit: 10.1443 nsec
        //  Digits:  4: Base: 16:       sprintf: 64 bit: 123.002 nsec
        //
        //  Digits:  5: Base: 16:       toChars: 64 bit: 30.5431 nsec
        //  Digits:  5: Base: 16: std::to_chars: 64 bit: 11.0911 nsec
        //  Digits:  5: Base: 16:       sprintf: 64 bit: 124.561 nsec
        //
        //  Digits:  6: Base: 16:       toChars: 64 bit: 32.9444 nsec
        //  Digits:  6: Base: 16: std::to_chars: 64 bit: 12.0145 nsec
        //  Digits:  6: Base: 16:       sprintf: 64 bit: 121.944 nsec
        //
        //  Digits:  7: Base: 16:       toChars: 64 bit: 35.6233 nsec
        //  Digits:  7: Base: 16: std::to_chars: 64 bit: 13.707 nsec
        //  Digits:  7: Base: 16:       sprintf: 64 bit: 123.877 nsec
        //
        //  Digits:  8: Base: 16:       toChars: 64 bit: 38.2101 nsec
        //  Digits:  8: Base: 16: std::to_chars: 64 bit: 15.7807 nsec
        //  Digits:  8: Base: 16:       sprintf: 64 bit: 124.369 nsec
        //
        //  Digits:  9: Base: 16:       toChars: 64 bit: 41.1421 nsec
        //  Digits:  9: Base: 16: std::to_chars: 64 bit: 14.6536 nsec
        //  Digits:  9: Base: 16:       sprintf: 64 bit: 122.936 nsec
        //
        //  Digits: 10: Base: 16:       toChars: 64 bit: 44.1328 nsec
        //  Digits: 10: Base: 16: std::to_chars: 64 bit: 15.0476 nsec
        //  Digits: 10: Base: 16:       sprintf: 64 bit: 125.02 nsec
        //
        //  Digits: 11: Base: 16:       toChars: 64 bit: 47.7125 nsec
        //  Digits: 11: Base: 16: std::to_chars: 64 bit: 16.4474 nsec
        //  Digits: 11: Base: 16:       sprintf: 64 bit: 125.71 nsec
        //
        //  Digits: 12: Base: 16:       toChars: 64 bit: 49.6224 nsec
        //  Digits: 12: Base: 16: std::to_chars: 64 bit: 16.3875 nsec
        //  Digits: 12: Base: 16:       sprintf: 64 bit: 124.301 nsec
        //
        //  Digits: 13: Base: 16:       toChars: 64 bit: 52.0921 nsec
        //  Digits: 13: Base: 16: std::to_chars: 64 bit: 17.3773 nsec
        //  Digits: 13: Base: 16:       sprintf: 64 bit: 126.331 nsec
        //
        //  Digits: 14: Base: 16:       toChars: 64 bit: 54.4617 nsec
        //  Digits: 14: Base: 16: std::to_chars: 64 bit: 18.1372 nsec
        //  Digits: 14: Base: 16:       sprintf: 64 bit: 127.161 nsec
        //
        //  Digits: 15: Base: 16:       toChars: 64 bit: 58.1612 nsec
        //  Digits: 15: Base: 16: std::to_chars: 64 bit: 20.1769 nsec
        //  Digits: 15: Base: 16:       sprintf: 64 bit: 129.96 nsec
        //
        //  Digits: 16: Base: 16:       toChars: 64 bit: 60.5408 nsec
        //  Digits: 16: Base: 16: std::to_chars: 64 bit: 19.957 nsec
        //  Digits: 16: Base: 16:       sprintf: 64 bit: 131.55 nsec
        //
        //  Thoughts: Before writing this component, we did a lot of
        //  examination of the implementation of 'std::to_chars' (being a
        //  template function, the whole thing is in the include files).
        //
        //  'std::to_chars' has special hard-coded implementations for octal
        //  and hex, whereas 'toChars' is using a general function that can
        //  handle all binary bases, so it's not entirely surprising that
        //  theirs is faster.  The performance is good enough that it's not
        //  worth doing custom octal and hex functions.
        //
        //  'toChars' has a fully-custom decimal implementation that is very
        //  similar to that of 'std::to_chars' with a couple of improvements.
        //
        //  Generally, the goal is to provide a replacement for 'to_chars' on
        //  compilers where the native imp isn't available.  What we have is
        //  performing well enough for that.
        //
        //  Note that this component's decimal implementation outperforms
        //  'std::to_chars', and that this performance difference widens as the
        //  number of digits grows.  We feel that decimal is by far the most
        //  important base supported by the function, and that is where most of
        //  the optimization effort was focused.
        // --------------------------------------------------------------------

        printf("SPEED TRIAL");

        enum { k_HUNDRED_MILLION = 100 * 1000 * 1000 };

        bsls::Stopwatch sw;

        unsigned bases[] = { 8, 10, 16 };
        const char *intFmts[] = { "%o", "%d", "%x" };
        const char *int64Fmts[] = { "%llo", "%lld", "%llx" };

        for (int baseIdx = 0; baseIdx < 3; ++baseIdx) {
            unsigned base = bases[baseIdx];

            bool quit = false;
            for (int digits = 1; !quit; ++digits) {
                int ceiling = 1;

                for (int ii = 0, prev = ceiling; ii < digits; ++ii) {
                    prev = ceiling;
                    ceiling *= base;
                    if (ceiling < prev) {
                        quit = true;
                        ceiling = std::numeric_limits<int>::max();
                    }
                }
                const int delta  = ceiling < k_HUNDRED_MILLION
                                 ? 1
                                 : ceiling / k_HUNDRED_MILLION;

                const int trials = std::max<unsigned>(
                                     1, k_HUNDRED_MILLION / (ceiling / delta));
                double actions = ceiling;
                actions /= delta;
                actions *= trials;

                const char *fmt = intFmts[baseIdx];

                for (int kk = 0; kk < 3; ++kk) {
                    printf((0 == kk
                         ? "\nDigits: %2d: Base: %2u:       toChars: 32 bit: "
                         :  1 == kk
                         ? "Digits: %2d: Base: %2u: std::to_chars: 32 bit: "
                         : "Digits: %2d: Base: %2u:       sprintf: 32 bit: "),
                                                                 digits, base);

                    sw.reset();
                    sw.start(true);

                    for (int ii = trials; 0 < ii--;) {
                        bool sign = false;
                        for (int jj = 0, prev = -1; prev < jj && jj < ceiling;
                                        prev = jj, jj += delta, sign = !sign) {
                            int num = sign ? -jj : jj;
                            char buf[100];
                            switch (kk) {
                              case 0: {
                                char *result = Util::toChars(buf,
                                                              buf + 100,
                                                              num,
                                                              base);
                                BSLS_ASSERT_OPT(result);
                              } break;
                              case 1: {
                                std::to_chars_result result =
                                                       std::to_chars(buf,
                                                                     buf + 100,
                                                                     num,
                                                                     base);
                                BSLS_ASSERT_OPT(result.ec == std::errc());
                              } break;
                              case 2: {
                                sprintf(buf, fmt, num);
                                BSLS_ASSERT_OPT(0 != *buf);
                              } break;
                            }
                        }
                    }

                    sw.stop();
                    printf("%g nsec\n",
                                      1e9 * sw.accumulatedUserTime()/ actions);
                }
            }

            quit = false;
            for (int digits = 1; !quit; ++digits) {
                Int64 ceiling = 1;
                for (Int64 ii = 0, prev = ceiling; ii < digits; ++ii) {
                    prev = ceiling;
                    ceiling *= base;
                    if (ceiling < prev) {
                        quit = true;
                        ceiling = std::numeric_limits<Int64>::max();
                    }
                }
                const Int64 delta = ceiling < k_HUNDRED_MILLION
                                  ? 1
                                  : ceiling / k_HUNDRED_MILLION;
                const Int64 trials = std::max<Int64>(
                                     1, k_HUNDRED_MILLION / (ceiling / delta));
                double actions = static_cast<double>(ceiling);
                actions /= static_cast<double>(delta);
                actions *= static_cast<double>(trials);

                const char *fmt = int64Fmts[baseIdx];

                for (int kk = 0; kk < 3; ++kk) {
                    printf((0 == kk
                         ? "\nDigits: %2d: Base: %2u:       toChars: 64 bit: "
                         :  1 == kk
                         ? "Digits: %2d: Base: %2u: std::to_chars: 64 bit: "
                         : "Digits: %2d: Base: %2u:       sprintf: 64 bit: "),
                                                                 digits, base);

                    sw.reset();
                    sw.start(true);

                    for (Int64 ii = trials; 0 < ii--;) {
                        bool sign = false;
                        for (Int64 jj = 0, prev = -1;
                                         prev < jj && jj < ceiling;
                                        prev = jj, jj += delta, sign = !sign) {
                            Int64 num = sign ? -jj : jj;
                            char buf[100];
                            switch (kk) {
                              case 0: {
                                char *result = Util::toChars(buf,
                                                             buf + 100,
                                                             num,
                                                             base);
                                BSLS_ASSERT_OPT(result);
                              } break;
                              case 1: {
                                std::to_chars_result result =
                                                       std::to_chars(buf,
                                                                     buf + 100,
                                                                     num,
                                                                     base);
                                BSLS_ASSERT_OPT(result.ec == std::errc());
                              } break;
                              case 2: {
                                sprintf(buf, fmt, num);
                                BSLS_ASSERT_OPT(0 != *buf);
                              } break;
                            }
                        }
                    }

                    sw.stop();
                    printf("%g nsec\n",
                                      1e9 * sw.accumulatedUserTime()/ actions);
                }
            }
        }
      } break;
#endif
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
