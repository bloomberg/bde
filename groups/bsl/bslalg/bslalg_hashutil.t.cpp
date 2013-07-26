// bslalg_hashutil.t.cpp                                              -*-C++-*-

#include <bslalg_hashutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <limits.h>  // INT_MAX
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using bslalg::HashUtil;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides two hash functions.  We test them on a
// two kinds of buffers (fixed-length - integer, and variable-length) during
// the breathing test.  Together with the usage example which performs various
// experiments to be reported on in the component-level documentation, this is
// appropriate testing.  There are no other concerns about this component.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HASHING FUNDAMENTAL TYPES
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;
int veryVeryVerbose;

const int LENGTH = 1257;  // not a power of two

template <class TYPE>
void time_computeHash(const TYPE&   key,
                const char   *TYPEID)
{
    enum { ITERATIONS = 1000000 }; // 1M
    unsigned int value = 0;
    bsls::Stopwatch timer;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        value += HashUtil::computeHash(key) % LENGTH;
    }
    timer.stop();
    printf("Hashing 1M values (in seconds): %g\tof type %s\n",
           timer.elapsedTime(),
           TYPEID);
    (void) value;
}

int countBits(native_std::size_t value)
{
    int ret = 0;
    for (; value; value >>= 1) {
        ret += value & 1;
    }

    return ret;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   This test is at the same time a usage example and a set of
        //   measurement experiments.
        //
        // Plan:
        //   Verify that the code compiles and output the results of
        //   the experiments.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Suppose we want to analyze our hash function by seeing how it distributes
// integers across buckets.   We will declare 64 buckets, and distribute hits
// among the bucket by indexing them with the low order 6 bits of the hash.
// Then we will display the distribution of hits in each bucket, to see if
// the hash function is distributing them evenly.

        int buckets[64];

// First, we hash on the values of i in the range '[ 0, 1 << 15 )':

        {
            memset(buckets, 0, sizeof(buckets));
            for (int i = 0; i < (1 << 15); ++i) {
                unsigned int hash = bslalg::HashUtil::computeHash(i);

                ++buckets[hash & 63];
            }
            if (verbose) printf("Straight hash:\n");
            int col = 0;
            for (int i = 0; i < 64; ++i) {
                if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
                                                                  buckets[i]);
                ++col;
                if (8 == col) {
                    col = 0;
                    if (verbose) printf("\n");
                }
            }
        }

// Then, we will hash on the values of '4 * i' for i in the range
// '[ 0, 1 << 15 )'.  This is interesting because pointers will often be 4-byte
// aligned and have the 2 low-order bits always zero, so this will be a
// simulation of that:

        {
            memset(buckets, 0, sizeof(buckets));
            for (int i = 0; i < (1 << 15); ++i) {
                unsigned int hash = bslalg::HashUtil::computeHash(4 * i);

                ++buckets[hash & 63];
            }
            if (verbose) printf("\nStraight * 4 hash:\n");
            int col = 0;
            for (int i = 0; i < 64; ++i) {
                if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
                                                                  buckets[i]);
                ++col;
                if (8 == col) {
                    col = 0;
                    if (verbose) printf("\n");
                }
            }
        }

// Next, we will xor the bottom 30 bits of the hash into the bottom 6 bits, so
// we'll be observing more of the whole word:

        {
            memset(buckets, 0, sizeof(buckets));
            for (int i = 0; i < (1 << 15); ++i) {
                unsigned int hash = bslalg::HashUtil::computeHash(i);
                hash = hash ^ (hash >> 6) ^ (hash >> 12) ^ (hash >> 18) ^
                              (hash >> 24);

                ++buckets[hash & 63];
            }
            if (verbose) printf("\nFolded hash:\n");
            int col = 0;
            for (int i = 0; i < 64; ++i) {
                if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
                                                                  buckets[i]);
                ++col;
                if (8 == col) {
                    col = 0;
                    if (verbose) printf("\n");
                }
            }
        }

// Now, bear in mind that an identity hash will perform very optimally on the
// first and third tests we did.  This time we will take the difference between
// the current hash and the previous one, a test for which the identity
// function would perform abominably:

        {
            memset(buckets, 0, sizeof(buckets));
            unsigned int prev = 0;
            for (int i = 0; i < (1 << 15); ++i) {
                unsigned int hash = bslalg::HashUtil::computeHash(i);

                ++buckets[(hash - prev) & 63];
                prev = hash;
            }
            if (verbose) printf("\nDiff hash:\n");
            int col = 0;
            for (int i = 0; i < 64; ++i) {
                if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
                                                                  buckets[i]);
                ++col;
                if (8 == col) {
                    col = 0;
                    if (verbose) printf("\n");
                }
            }
        }

// Finally, take the difference between the previous hash and the current one,
// only this time, instead of subtracting, take a bitwise xor:

        {
            memset(buckets, 0, sizeof(buckets));
            unsigned int prev = 0;
            for (int i = 0; i < (1 << 15); ++i) {
                unsigned int hash = bslalg::HashUtil::computeHash(i);

                ++buckets[(hash ^ prev) & 63];
                prev = hash;
            }
            if (verbose) printf("\nXor diff hash:\n");
            int col = 0;
            for (int i = 0; i < 64; ++i) {
                if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
                                                                  buckets[i]);
                ++col;
                if (8 == col) {
                    col = 0;
                    if (verbose) printf("\n");
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HASHING FUNDAMENTAL TYPES
        //
        // Concerns:
        //   The hash should output a reasonable value, which does not depend
        //   on the endianness of the platform.
        //
        // Plan:
        //   Compare return value to expected values computed on a given
        //   platform.
        //
        // Testing:
        //    HashUtil::computeHash(char);
        //    HashUtil::computeHash(signed char);
        //    HashUtil::computeHash(unsigned char);
        //    HashUtil::computeHash(short);
        //    HashUtil::computeHash(unsigned short);
        //    HashUtil::computeHash(int);
        //    HashUtil::computeHash(unsigned int);
        //    HashUtil::computeHash(long);
        //    HashUtil::computeHash(unsigned long);
        //    HashUtil::computeHash(long long);
        //    HashUtil::computeHash(unsigned long long);
        //    HashUtil::computeHash(float);
        //    HashUtil::computeHash(double);
        //    HashUtil::computeHash(void*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nHASHING FUNDAMENTAL TYPES"
                            "\n=========================\n");

        ASSERT(3392050242U == HashUtil::computeHash((char)'a'));
        ASSERT(3392050242U == HashUtil::computeHash((signed char)'a'));
        ASSERT(3392050242U == HashUtil::computeHash((unsigned char)'a'));
        ASSERT(3111500981U == HashUtil::computeHash((short)12355));
        ASSERT(3111500981U == HashUtil::computeHash((unsigned short)12355));
        ASSERT(2509914878U == HashUtil::computeHash((int)0x12345678));
        ASSERT(2509914878U == HashUtil::computeHash((unsigned int)0x12345678));
        ASSERT(2509914878U == HashUtil::computeHash((long)0x12345678));
        ASSERT(2509914878U ==
                             HashUtil::computeHash((unsigned long)0x12345678));
        ASSERT(2509914878U ==
                   HashUtil::computeHash((long long)0x12345678));
        ASSERT(2509914878U ==
                  HashUtil::computeHash((unsigned long long)0x12345678));
        ASSERT(2343743579U == HashUtil::computeHash((float)3.1415926536));
        ASSERT(3721749206U ==
                        HashUtil::computeHash((double)3.14159265358979323844));
#ifdef BSLS_PLATFORM__CPU_64_BIT
        ASSERT(2631003531U ==
                           HashUtil::computeHash((void*)0xffab13f1324e5473LL));
#else
        ASSERT(1747622670U == HashUtil::computeHash((void*)0xffab13f1));
#endif
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
        //:   Verify that small changes to the input value result in many
        //:   bits being toggled in the output.  Note that the hash function
        //:   currently only sets the low-order 32 bits of the result.  When
        //:   this is fixed the asserts should be changed to demand that at
        //:   least a quarter of the bits in a 'size_t' have chagned.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) Q(Incrementing integers);

        native_std::size_t lastHash = -1;
        for (int i = 0; i < 100; ++i) {
            native_std::size_t hash = HashUtil::computeHash(i);
            unsigned changed = countBits(hash ^ lastHash);
            ASSERTV(i, changed, hash, lastHash,
                                           changed > sizeof(unsigned) * 8 / 4);
            ASSERT(changed > sizeof(unsigned) * 8 / 4);
            if (verbose) printf(
                         "%2d: %8x, hash: %8x, lastHash: %8x, changed: %d\n",
                  i, i, (unsigned) hash, (unsigned) lastHash, changed);
            lastHash = hash;
        }

        if (verbose) Q(Shifting Bit);

        lastHash = -1;
        long long valueToHash;
        for (int i = 0; i < 64; ++i) {
            valueToHash = (long long) 1 << i;
            native_std::size_t hash = HashUtil::computeHash(valueToHash);
            unsigned changed = countBits(hash ^ lastHash);
            ASSERT(changed > sizeof(unsigned) * 8 / 4);
            if (verbose) printf(
                        "%2d: %16llx, hash: %8x, lastHash: %8x, changed: %d\n",
                i, valueToHash, (unsigned) hash, (unsigned) lastHash, changed);
            lastHash = hash;
        }

        if (verbose) Q(Setting One Bit at a Time);

        lastHash = -1;
        valueToHash = 0;
        for (int i = 0; i < 64; ++i) {
            valueToHash |= (long long) 1 << i;
            native_std::size_t hash = HashUtil::computeHash(
                                                           (long long) 1 << i);
            unsigned changed = countBits(hash ^ lastHash);
            ASSERT(changed > sizeof(unsigned) * 8 / 4);
            if (verbose) printf(
                        "%2d: %16llx, hash: %8x, lastHash: %8x, changed: %d\n",
                i, valueToHash, (unsigned) hash, (unsigned) lastHash, changed);
            lastHash = hash;
        }

        if (verbose) Q(Clearing One Bit at a Time);

        valueToHash = -1;
        for (int i = 0; i < 64; ++i) {
            valueToHash &= ~((long long) 1 << i);
            native_std::size_t hash = HashUtil::computeHash(
                                                           (long long) 1 << i);
            unsigned changed = countBits(hash ^ lastHash);
            ASSERT(changed > sizeof(unsigned) * 8 / 4);
            if (verbose) printf(
                        "%2d: %16llx, hash: %8x, lastHash: %8x, changed: %d\n",
                i, valueToHash, (unsigned) hash, (unsigned) lastHash, changed);
            lastHash = hash;
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE MEASUREMENTS
        //
        // Concerns:
        //   The hash are fairly thorough but are they fast?  Let's evaluate
        //   performance here.
        //
        // Plan:
        //   Perform the test of case 2 inside a loop and report the timing
        //   using a 'bdes_stopwatch'.
        //
        // Testing:
        //    HashUtil::computeHash(char);
        //    HashUtil::computeHash(signed char);
        //    HashUtil::computeHash(unsigned char);
        //    HashUtil::computeHash(short);
        //    HashUtil::computeHash(unsigned short);
        //    HashUtil::computeHash(int);
        //    HashUtil::computeHash(unsigned int);
        //    HashUtil::computeHash(long);
        //    HashUtil::computeHash(unsigned long);
        //    HashUtil::computeHash(long long);
        //    HashUtil::computeHash(unsigned long long);
        //    HashUtil::computeHash(float);
        //    HashUtil::computeHash(double);
        //    HashUtil::computeHash(void*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nHASHING FUNDAMENTAL TYPES"
                            "\n=========================\n");

        time_computeHash((char)'a', "char");
        time_computeHash((signed char)'a', "signed char");
        time_computeHash((unsigned char)'a', "unsigned char");
        time_computeHash((short)12355, "short");
        time_computeHash((unsigned short)12355, "unsigned short");
        time_computeHash((int)0x12345678, "int");
        time_computeHash((unsigned int)0x12345678, "unsigned int");
        time_computeHash((long)0x12345678, "long");
        time_computeHash((unsigned long)0x12345678, "unsigned long");
        time_computeHash((long long)0x12345678, "bsls_Types::Int64");
        time_computeHash((unsigned long long)0x12345678,
                         "bsls_Types::Uint64");
        time_computeHash((float)3.1415926536, "float");
        time_computeHash((double)3.14159265358979323844, "double");
#ifdef BSLS_PLATFORM__CPU_64_BIT
        time_computeHash((void*)0xffab13f1324e5473LL, "void*");
#else
        time_computeHash((void*)0xffab13f1, "void*");
#endif
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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
