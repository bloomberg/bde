// bdlu_bitutil.t.cpp                                                 -*-C++-*-

#include <bdlu_bitutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

#include <bsl_cctype.h>      // isspace()
#include <bsl_climits.h>     // INT_MIN, INT_MAX
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()

#include <time.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"  \
                     << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t"  \
                    << #J << ": " << J << "\t"  \
                    << #K << ": " << K << "\n"; \
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
//-----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// Exploit sign-extension to simulate leading 1's and 0's portably.
// Loop over the number of bits per word, where that makes sense.  Use a
// special generator function g(spec) that recognizes up to one fill pattern
// of the form 0..0 or 1..1 to create integers of unspecified length more
// easily.  Note that the behavior of shifting the number of bits per word on
// a integer is undefined.  The general category partitioning is based on
// boundary cases such as 0, 1, BITS_PER_WORD - 1, and BITS_PER_WORD.
// Note that it was necessary to break 'main' up into separate files because
// of unacceptably long build times on windows.
//-----------------------------------------------------------------------------
//
// [ 2] bool isBitSet(uint32_t value, int index);
// [ 2] bool isBitSet(uint64_t value, int index);
// [ 6] int log2(uint32_t value);
// [ 6] int log2(uint64_t value);
// [ 4] int numBitsSet(uint32_t value);
// [ 4] int numBitsSet(uint64_t value);
// [ 5] int numLeadingUnsetBits(uint32_t value);
// [ 5] int numLeadingUnsetBits(uint64_t value);
// [ 5] int numTrailingUnsetBits(uint32_t value);
// [ 5] int numTrailingUnsetBits(uint64_t value);
// [ 6] uint32_t roundUpToBinaryPower(uint32_t value);
// [ 6] uint64_t roundUpToBinaryPower(uint64_t value);
// [ 1] int sizeInBits(INTEGER value);
// [ 3] uint32_t withBitCleared(uint32_t value, int index);
// [ 3] uint64_t withBitCleared(uint64_t value, int index);
// [ 3] uint32_t withBitSet(uint32_t value, int index);
// [ 3] uint64_t withBitSet(uint64_t value, int index);

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlu::BitUtil        Util;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                          GLOBAL VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'numLeadingUnsetBits' and 'numTrailingUnsetBits'
        //
        // Concerns:
        //: 1 methods correctly return the number of unset bits prior to a set
        //:   bit for the specified direction
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result
        //:
        //: 2 verify result for case where all bits are set (C-1)
        //
        // Testing:
        //   int numLeadingUnsetBits(uint32_t value);
        //   int numLeadingUnsetBits(uint64_t value);
        //   int numTrailingUnsetBits(uint32_t value);
        //   int numTrailingUnsetBits(uint64_t value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'numLeadingUnsetBits'"
                          << "\n=============================="
                          << endl;

        { // no bits set
            uint32_t value32 = (uint32_t)0;
            ASSERT(32 == Util::numLeadingUnsetBits(value32));
            uint64_t value64 = (uint64_t)0;
            ASSERT(64 == Util::numLeadingUnsetBits(value64));
        }

        { // test for 1 to 3 bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k <= j; ++k) {
                        uint32_t value32 = (((uint32_t)1 << i)
                                            | ((uint32_t)1 << j)
                                            | ((uint32_t)1 << k));
                        LOOP2_ASSERT(value32,
                                     Util::numLeadingUnsetBits(value32),
                                     31 - i
                                        == Util::numLeadingUnsetBits(value32));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k <= j; ++k) {
                        uint64_t value64 = (((uint64_t)1 << i)
                                            | ((uint64_t)1 << j)
                                            | ((uint64_t)1 << k));
                        LOOP2_ASSERT(value64,
                                     Util::numLeadingUnsetBits(value64),
                                     63 - i
                                        == Util::numLeadingUnsetBits(value64));
                    }
                }
            }
        }

        { // all bits set
            uint32_t value32 = (uint32_t)-1;
            ASSERT(0 == Util::numLeadingUnsetBits(value32));
            uint64_t value64 = (uint64_t)-1;
            ASSERT(0 == Util::numLeadingUnsetBits(value64));
        }

        if (verbose) cout << "\nTesting 'numTrailingUnsetBits'"
                          << "\n=============================="
                          << endl;

        { // no bits set
            uint32_t value32 = (uint32_t)0;
            ASSERT(32 == Util::numTrailingUnsetBits(value32));
            uint64_t value64 = (uint64_t)0;
            ASSERT(64 == Util::numTrailingUnsetBits(value64));
        }

        { // test for 1 to 3 bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i; j < 32; ++j) {
                    for (int k = j; k < 32; ++k) {
                        uint32_t value32 = (((uint32_t)1 << i)
                                            | ((uint32_t)1 << j)
                                            | ((uint32_t)1 << k));
                        LOOP2_ASSERT(value32,
                                     Util::numTrailingUnsetBits(value32),
                                     i == Util::numTrailingUnsetBits(value32));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i; j < 64; ++j) {
                    for (int k = j; k < 64; ++k) {
                        uint64_t value64 = (((uint64_t)1 << i)
                                            | ((uint64_t)1 << j)
                                            | ((uint64_t)1 << k));
                        LOOP2_ASSERT(value64,
                                     Util::numTrailingUnsetBits(value64),
                                     i == Util::numTrailingUnsetBits(value64));
                    }
                }
            }
        }

        { // all bits set
            uint32_t value32 = (uint32_t)-1;
            ASSERT(0 == Util::numTrailingUnsetBits(value32));
            uint64_t value64 = (uint64_t)-1;
            ASSERT(0 == Util::numTrailingUnsetBits(value64));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'numBitsSet'
        //
        // Concerns:
        //: 1 'numBitsSet' correctly returns the number of set bits
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result
        //:
        //: 2 verify result for case where all bits are set (C-1)
        //
        // Testing:
        //   int numBitsSet(uint32_t value);
        //   int numBitsSet(uint64_t value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'numBitsSet'"
                          << "\n===================="
                          << endl;

        { // depth 0; no bits set
            uint32_t value32 = 0;
            ASSERT(0 == Util::numBitsSet(value32));
            uint64_t value64 = 0;
            ASSERT(0 == Util::numBitsSet(value64));
        }

        { // depth 1; one bit set
            for (int i = 0; i < 32; ++i) {
                uint32_t value32 = (uint32_t)1 << i;
                ASSERT(1 == Util::numBitsSet(value32));
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = (uint64_t)1 << i;
                ASSERT(1 == Util::numBitsSet(value64));
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = (((uint32_t)1 << i)
                                        | ((uint32_t)1 << j));
                    ASSERT(2 == Util::numBitsSet(value32));
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = (((uint64_t)1 << i)
                                        | ((uint64_t)1 << j));
                    ASSERT(2 == Util::numBitsSet(value64));
                }
            }
        }

        { // all bits set
            uint32_t value32 = (uint32_t)-1;
            ASSERT(32 == Util::numBitsSet(value32));
            uint64_t value64 = (uint64_t)-1;
            ASSERT(64 == Util::numBitsSet(value64));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'withBitCleared' and 'withBitSet'
        //
        // Concerns:
        //: 1 the methods correctly adjust the bit in the specified index
        //: 2 the methods do not adjust the bits in other index positions
        //: 3 the methods work for all index positions
        //: 4 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 starting with the values 0 and all-bits-set, for every index
        //:   perform both operations and verify the result values (C-1..3)
        //:
        //: 2 verify defensive checks are triggered for invalid attribute
        //:   values (C-4)
        //
        // Testing:
        //   uint32_t withBitCleared(uint32_t value, int index);
        //   uint64_t withBitCleared(uint64_t value, int index);
        //   uint32_t withBitSet(uint32_t value, int index);
        //   uint64_t withBitSet(uint64_t value, int index);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'withBitCleared' and 'withBitSet'"
                          << "\n========================================="
                          << endl;

        { // starting from 0
            for (int index = 0; index < 32; ++index) {
                uint32_t value32 = 0;
                ASSERT(0 == Util::withBitCleared(value32, index));
                ASSERT(((uint32_t)1 << index)
                                          == Util::withBitSet(value32, index));
            }
            for (int index = 0; index < 64; ++index) {
                uint64_t value64 = 0;
                ASSERT(0 == Util::withBitCleared(value64, index));
                ASSERT(((uint64_t)1 << index)
                                          == Util::withBitSet(value64, index));
            }
        }
        { // starting from all-bits-set
            for (int index = 0; index < 32; ++index) {
                uint32_t value32 = (uint32_t)-1;
                ASSERT(~((uint32_t)1 << index)
                                      == Util::withBitCleared(value32, index));
                ASSERT(value32 == Util::withBitSet(value32, index));
            }
            for (int index = 0; index < 64; ++index) {
                uint64_t value64 = (uint64_t)-1;
                ASSERT(~((uint64_t)1 << index)
                                      == Util::withBitCleared(value64, index));
                ASSERT(value64 == Util::withBitSet(value64, index));
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            uint32_t value32 = 0;
            uint64_t value64 = 0;

            ASSERT_SAFE_FAIL(Util::withBitCleared(value32, -1));
            ASSERT_SAFE_PASS(Util::withBitCleared(value32, 0));
            ASSERT_SAFE_PASS(Util::withBitCleared(value32, 31));
            ASSERT_SAFE_FAIL(Util::withBitCleared(value32, 32));

            ASSERT_SAFE_FAIL(Util::withBitCleared(value64, -1));
            ASSERT_SAFE_PASS(Util::withBitCleared(value64, 0));
            ASSERT_SAFE_PASS(Util::withBitCleared(value64, 63));
            ASSERT_SAFE_FAIL(Util::withBitCleared(value64, 64));

            ASSERT_SAFE_FAIL(Util::withBitSet(value32, -1));
            ASSERT_SAFE_PASS(Util::withBitSet(value32, 0));
            ASSERT_SAFE_PASS(Util::withBitSet(value32, 31));
            ASSERT_SAFE_FAIL(Util::withBitSet(value32, 32));

            ASSERT_SAFE_FAIL(Util::withBitSet(value64, -1));
            ASSERT_SAFE_PASS(Util::withBitSet(value64, 0));
            ASSERT_SAFE_PASS(Util::withBitSet(value64, 63));
            ASSERT_SAFE_FAIL(Util::withBitSet(value64, 64));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isBitSet'
        //
        // Concerns:
        //: 1 'isBitSet' correctly returns the state of the specified bit
        //: 2 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result (C-1)
        //:
        //: 2 verify defensive checks are triggered for invalid attribute
        //:   values (C-2)
        //
        // Testing:
        //   bool isBitSet(uint32_t value, int index);
        //   bool isBitSet(uint64_t value, int index);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isBitSet'"
                          << "\n=================="
                          << endl;

        { // depth 0; no bits set
            uint32_t value32 = 0;
            for (int index = 0; index < 32; ++index) {
                ASSERT(false == Util::isBitSet(value32, index));
            }
            uint64_t value64 = 0;
            for (int index = 0; index < 64; ++index) {
                ASSERT(false == Util::isBitSet(value64, index));
            }
        }

        { // depth 1; one bit set
            for (int i = 0; i < 32; ++i) {
                uint32_t value32 = (uint32_t)1 << i;
                for (int index = 0; index < 32; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value32, index));
                }
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = (uint64_t)1 << i;
                for (int index = 0; index < 64; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value64, index));
                }
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = (((uint32_t)1 << i)
                                        | ((uint32_t)1 << j));
                    for (int index = 0; index < 32; ++index) {
                        ASSERT((index == i || index == j)
                                            == Util::isBitSet(value32, index));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = (((uint64_t)1 << i)
                                        | ((uint64_t)1 << j));
                    for (int index = 0; index < 64; ++index) {
                        ASSERT((index == i || index == j)
                                            == Util::isBitSet(value64, index));
                    }
                }
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            uint32_t value32 = 0;
            uint64_t value64 = 0;

            ASSERT_SAFE_FAIL(Util::isBitSet(value32, -1));
            ASSERT_SAFE_PASS(Util::isBitSet(value32, 0));
            ASSERT_SAFE_PASS(Util::isBitSet(value32, 31));
            ASSERT_SAFE_FAIL(Util::isBitSet(value32, 32));

            ASSERT_SAFE_FAIL(Util::isBitSet(value64, -1));
            ASSERT_SAFE_PASS(Util::isBitSet(value64, 0));
            ASSERT_SAFE_PASS(Util::isBitSet(value64, 63));
            ASSERT_SAFE_FAIL(Util::isBitSet(value64, 64));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'sizeInBits'
        //
        // Concerns:
        //: 1 'sizeInBits' correctly returns the size of variables in bits
        //
        // Plan:
        //: 1 verify results on various sized types (C-1)
        //
        // Testing:
        //   int sizeInBits(INTEGER value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'sizeInBits'"
                          << "\n===================="
                          << endl;

        BSLMF_ASSERT(4 == sizeof(int32_t) && 8 == sizeof(int64_t));
        BSLMF_ASSERT(1 == sizeof(char) && 2 == sizeof(short));
        BSLMF_ASSERT(4 == sizeof(int) && 8 == sizeof(Int64));

        char myChar = 0;
        unsigned char myUnsignedChar = 0;
        signed char mySignedChar = 0;

        short myShort = 0;
        unsigned short myUnsignedShort = 0;

        int32_t myExactInt32 = 0;
        uint32_t myExactUint32 = 0;

        int myInt = 0;
        unsigned myUnsigned = 0;

        long myLong = 0;
        unsigned long myUnsignedLong = 0;

        void *myPtr = 0;

        int64_t myExactInt64 = 0;
        uint64_t myExactUint64 = 0;

        Int64 myInt64 = 0;
        Uint64 myUint64 = 0;

        ASSERT(8 == Util::sizeInBits(myChar));
        ASSERT(8 == Util::sizeInBits(myUnsignedChar));
        ASSERT(8 == Util::sizeInBits(mySignedChar));

        ASSERT(16 == Util::sizeInBits(myShort));
        ASSERT(16 == Util::sizeInBits(myUnsignedShort));

        ASSERT(32 == Util::sizeInBits(myExactInt32));
        ASSERT(32 == Util::sizeInBits(myExactUint32));

        ASSERT(32 == Util::sizeInBits(myInt));
        ASSERT(32 == Util::sizeInBits(myUnsigned));

        ASSERT(sizeof(long) * CHAR_BIT == Util::sizeInBits(myLong));
        ASSERT(sizeof(unsigned long) * CHAR_BIT ==
                                             Util::sizeInBits(myUnsignedLong));

        ASSERT(sizeof(void *) * CHAR_BIT == Util::sizeInBits(myPtr));

        ASSERT(64 == Util::sizeInBits(myExactInt64));
        ASSERT(64 == Util::sizeInBits(myExactUint64));

        ASSERT(64 == Util::sizeInBits(myInt64));
        ASSERT(64 == Util::sizeInBits(myUint64));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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
