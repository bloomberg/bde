// bdlb_randomdevice.t.cpp                                            -*-C++-*-
#include <bdlb_randomdevice.h>

// Note the headers are in non-standard order.  This was required to silence an
// error from clang 3.4.
//..
//  /usr/include/unistd.h:449:12: error: declaration conflicts with target of
//  using declaration already in scope
//  extern int rename(const char *, const char *);
//..
// This is an acknowledged issue:
// <http://lists.cs.uiuc.edu/pipermail/llvmbugs/2012-May/023328.html>

#include <bslim_testutil.h>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>              // for benchmarking only

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>                     // 'Sleep'
#else
#include <unistd.h>                      // 'usleep'
#endif

#include <bsl_iostream.h>
#include <bsl_vector.h>                  // for usage example
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// [-1] static int getRandomBytes(unsigned char *buf, size_t numB);
// [ 2] static int getRandomBytesNonBlocking(buf, numB);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-2] PERFORMANCE: 'getRandomBytes'
// [-3] PERFORMANCE: 'getRandomBytesNonBlocking'
// [-4] PERFORMANCE: 'getRandomBytes'
// [-5] PERFORMANCE: 'getRandomBytes'
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::RandomDevice Util;

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#define sleep(x) Sleep((x))
#else
#define sleep(x) usleep((x) * 100)
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//..
///Example 1: Seeding the Random-Number Generator
/// - - - - - - - - - - - - - - - - - - - - - - -
// System-provided random-number generators generally must be initialized with
// a seed value from which they go on to produce their stream of pseudo-random
// numbers.  We can use 'RandomDevice' to provide such a seed.
//
// First, we obtain the results of invoking the random-number generator without
// having seeded it:
//..
    int unseededR1 = rand();
    int unseededR2 = rand();
//..
// Then, we obtain a random number:
//..
    int seed = 0;
    int status = bdlb::RandomDevice::getRandomBytes(
        reinterpret_cast<unsigned char *>(&seed), sizeof(seed));
    ASSERT(0 == status);
    ASSERT(0 != seed);    // This will fail every few billion attempts...
//..
// Next, we seed the random-number generator with our seed:
//..
    srand(seed);
//..
// Finally, we observe that we obtain different numbers:
//..
    ASSERT(unseededR1 != rand());
    ASSERT(unseededR2 != rand());
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'int getRandomBytesNonBlocking(buf, numB)' TEST
        //
        // Concerns:
        //: 1 If a number is passed, that many bytes are set.
        //: 2 The random bytes are distributed uniformly (probabilistic).
        //
        // Plan:
        //: 1 Request a large pool and random bytes from non-blocking random
        //:   number generator.  Verify that each is unique. Verify that the
        //:   numbers approximate a uniform distribution.
        //
        // Testing:
        //   static int getRandomBytesNonBlocking(buf, numB);
        // --------------------------------------------------------------------
        const int      NUM_ITERATIONS                   = 25;
        const int      NUM_TRIALS                       = 10;
        int            cnt                              = 0;
        unsigned char  buffer[NUM_ITERATIONS * 4]       = { };
        unsigned char  prev_buffer[NUM_ITERATIONS * 4]  = { };
        const unsigned NUM_BYTES                        = sizeof buffer;

        if (verbose)
            cout << endl
                 << "'int getRandomBytesNonBlocking(buf, numB)' TEST" << endl
                 << "===============================================" << endl;

        // 1) If a number is passed, that many bytes are set.
        if (veryVerbose) {
            cout << "\nTesting the number of bytes set." << endl;
        }
        for (unsigned i = 0; i < 5; ++i) {
            memset(buffer, 0, NUM_BYTES);
            // Repeat the accession of random bytes 'NUM_TRIALS' times to
            // prevent false negatives
            for (int j = 0; j < NUM_TRIALS; ++j) {
                if (veryVerbose) { P(j) }
                ASSERT(0 == Util::getRandomBytesNonBlocking(buffer, i));
                // sum the bytes
                for (unsigned k = 0; k < NUM_BYTES; ++k) {
                    buffer[k] =  static_cast<unsigned char>(buffer[k] +
                                                            prev_buffer[k]);
                }
                // copy the buffer
                memcpy(prev_buffer, buffer, sizeof buffer);
            }
            int sum = 0;
            // check that the bytes set are non-zero
            unsigned j;
            for (j = 0; j < i; ++j)    {
                sum += static_cast<int>(buffer[j]) ;
            }
            LOOP2_ASSERT(i, sum, 0 == i || 0 != sum);
            // check that remaining bytes are still unset.
            for (; j < NUM_BYTES; ++j) {
                LOOP3_ASSERT(i, j, int(buffer[j]), 0 == buffer[j]);
            }
        }

        if (veryVerbose) {
            cout << "\nTesting the distribution of rand." << endl;
        }
        // 3) The random bytes are uniformly distributed (probabilistic)
        int numbers[NUM_ITERATIONS] = { };
        for (int i = 0; i< NUM_ITERATIONS; ++i) {
            int rand;
            if (veryVerbose) { P(i) }
            ASSERT(0 == Util::getRandomBytesNonBlocking(
                                      reinterpret_cast<unsigned char *>(&rand),
                                      sizeof rand));
            numbers[i] = rand;
            if (veryVerbose) { P_(i) P(rand) }
            for (int j = 0; j < i; ++j) {
                ASSERT(numbers[j] != rand);
                if (veryVerbose) { P_(j) P(numbers[j]) }
            }

            for (int b = 0; b < 15; ++b) {
                cnt += rand & 1;
                rand >>= 1;
                if (veryVerbose) { P(cnt) }
            }
        }
        double expected = (NUM_ITERATIONS * 15) / 2;
        ASSERT(cnt < (expected * 1.2));
        ASSERT(cnt > (expected * 0.8));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise a broad cross-section of the mechanism of reading random
        //   numbers from the system.  Probe that functionality systematically
        //   and incrementally to discover basic errors in isolation.
        //
        // Plan:
        //     Request a large pool of random 'ints' from each of the system's
        //     random number generators. Verify the uniqueness, and the
        //     distribution.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        const int NUM_ITERATIONS = 8;

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        int rand;
        int numbers [NUM_ITERATIONS];
        // fill buffer with random bytes
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            unsigned char *p = reinterpret_cast<unsigned char *>(&rand);
            ASSERT(0 == Util::getRandomBytes(p, sizeof rand));
            numbers[i] = rand;
            if (veryVerbose) { P_(i) P(rand) }
        }
        // verify uniqueness
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            for (int j = i + 1; j < NUM_ITERATIONS; ++j) {
                LOOP2_ASSERT(i, j, numbers[i] != numbers[j]);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // 'int getRandomBytes(void *buf, int numBytes) TEST'
        //
        // Concerns:
        //   1) If a number is passed, that many bytes are set.
        //   2) The random bytes are distributed uniform (probabilistic)
        //
        // Plan:
        //   Request a large pool and random bytes from non-blocking random
        //   number generator.  Verify that each is unique. Verify that the
        //   numbers approximate a uniform distribution.
        //
        // Testing:
        //   static int getRandomBytes(unsigned char *buf, size_t numB);
        // --------------------------------------------------------------------
        const int      NUM_ITERATIONS = 25;
        int            cnt = 0;
        int            numbers[NUM_ITERATIONS] = { };
        const unsigned NUM_BYTES = sizeof numbers;

        if (verbose)
            cout << endl
                 << "'int getRandomBytes(void *buf, int numBytes) TEST'"
                 << endl
                 << "=================================================="
                 << endl;

        if (veryVerbose) {
            cout << "\nTesting the number of bytes set." << endl;
        }
        // 2) If a number is passed, that many bytes are set.
        for (unsigned i = 0; i < 5; ++i) {
            unsigned       j;
            unsigned char *p = reinterpret_cast<unsigned char *>(numbers);
            memset(numbers, 0, NUM_BYTES);
            if (veryVerbose) { P(i) }
            ASSERT(0 == Util::getRandomBytesNonBlocking(p, i));

            for (j = 0; j < i; ++j)    {
                LOOP3_ASSERT(i, j, p[j], 0 != p[j]);
            }
            for (; j < NUM_BYTES; ++j) {
                LOOP3_ASSERT(i, j, p[j], 0 == p[j]);
            }
        }

        if (veryVerbose) {
            cout << "\nTesting the distribution of rand." << endl;
        }

        // 3) The random bytes are distributed uniform (probabilistic)
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            int            rand_int = 0;
            unsigned char *p1 = reinterpret_cast<unsigned char *>(&rand_int);
            if (veryVerbose) { P(i); }
            ASSERT(0 == Util::getRandomBytesNonBlocking(p1, sizeof rand_int));
            numbers[i] = rand_int;
            for (int j = 0; j < i; ++j) {
                LOOP5_ASSERT(i, j, rand_int, numbers[i], numbers[j],
                             numbers[j] != numbers[i]);
            }

            for (int b = 0; b < 15; ++b) {
                cnt += rand_int & 1;
                rand_int >>= 1;
                if (veryVerbose) { P(cnt) }
            }
        }
        double expected = (NUM_ITERATIONS * 15) / 2;
        ASSERT(cnt < (expected * 1.2));
        ASSERT(cnt > (expected * 0.8));
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'getRandomBytes'
        //
        // Concerns:
        //   Measure the effect of requesting larger random numbers per
        //   request from the blocking random generator.
        //
        // Plan:
        //   Request a large number of random numbers to consume the entropy
        //   on process start-up. Next, request the same total number of
        //   bytes, each time changing the size of the request, measure the
        //   time to complete each request.
        //
        // Testing:
        //   PERFORMANCE: 'getRandomBytes'
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE: 'getRandomBytes'" << endl
                          << "=============================" << endl;
        bsls::Stopwatch s;
        const int       NUM_ITERATIONS                = 4;
        const int       MAX_GRANUALARITY              = 1 << NUM_ITERATIONS;
        unsigned char   buffer[MAX_GRANUALARITY]      = { };
        unsigned char   prev_buffer[MAX_GRANUALARITY] = { };
        for (int granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1) {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            int i;
            for (i = 0; i <= MAX_GRANUALARITY; i += granularity) {
                if (veryVerbose) { P(i) }
                ASSERT(0 == Util::getRandomBytes(buffer + granularity,
                                                 granularity));
            }
            s.stop();
            LOOP2_ASSERT(granularity,
                         i,
                         0 != memcmp(buffer, prev_buffer, MAX_GRANUALARITY));
            double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                          s.accumulatedWallTime();
            P_(granularity) P(time);
            s.reset();
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'getRandomBytesNonBlocking'
        //
        // Concerns:
        //    Measure the effect of requesting larger random numbers per
        //    request from the non-blocking random generator.
        //
        // Plan:
        //      Request a large number of random numbers to consume the entropy
        //      on process start-up. Next, request the same total number of
        //      bytes, each time changing the size of the request, measure the
        //      time to complete each request.
        //
        // Testing:
        //   PERFORMANCE: 'getRandomBytesNonBlocking'
        //---------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "PERFORMANCE: 'getRandomBytesNonBlocking'" << endl
                 << "========================================" << endl;

        int rand_int;
        for (int i = 0; i < 15; ++i) {
            if (veryVerbose) { P(i) }
            ASSERT(0 == Util::getRandomBytesNonBlocking(
                                  reinterpret_cast<unsigned char *>(&rand_int),
                                  sizeof rand_int));
        }

        bsls::Stopwatch s;
        const int       NUM_ITERATIONS                = 4;
        const int       MAX_GRANUALARITY              = 1 << NUM_ITERATIONS;
        unsigned char   buffer [MAX_GRANUALARITY]     = { };
        unsigned char   prev_buffer[MAX_GRANUALARITY] = { };

        for (int granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1) {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            int i;
            for (i = 0; i <= MAX_GRANUALARITY; i +=  granularity) {
                if (veryVerbose) { P(i) }
                ASSERT(0 == Util::getRandomBytesNonBlocking(
                                                          buffer + granularity,
                                                          granularity));
            }
            s.stop();
            LOOP2_ASSERT(granularity,
                         i,
                         0 != memcmp(buffer, prev_buffer, MAX_GRANUALARITY));
            double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                          s.accumulatedWallTime();
            if (veryVerbose) { P_(granularity) P(time) }
            s.reset();
        }
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'getRandomBytes'
        //
        // Concerns:
        //   Measure the amount of time necessary to wait between successive
        //   request for random 'int'(s).
        //
        // Plan:
        //   Request a large number of random numbers to empty out the entropy
        //   source. Next request a random number and vary the delay before
        //   requesting another random number, each time measuring the time
        //   required to receive the next random number.
        //
        // Testing:
        //   PERFORMANCE: 'getRandomBytes'
        //---------------------------------------------------------------------
        if (verbose) cout << "PERFORMANCE: 'getRandomBytes'"
                          << "=============================" << endl;
        bsls::Stopwatch  s;
        const int        MAX_SLEEP = 10;
        int              rand_int;
        unsigned char   *p1 = reinterpret_cast<unsigned char *>(&rand_int);

        for (int i = 0; i < 15; ++i) {
            if (veryVerbose) { P(i) }
            ASSERT(0 == Util::getRandomBytes(p1, sizeof rand_int));
        }

        for (unsigned curr_sleep = 0; curr_sleep <= MAX_SLEEP; ++curr_sleep) {
            if (veryVerbose) { P(curr_sleep) }
            ASSERT(0 == Util::getRandomBytes(p1, sizeof rand_int));
            sleep(curr_sleep);
            s.start(true);
            if (veryVerbose) { P(curr_sleep) }
            ASSERT(0 == Util::getRandomBytes(p1, sizeof rand_int));
            s.stop();
            double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                          s.accumulatedWallTime();
            s.reset();
            if (veryVerbose) {
                cout << "Current Delay             : " << curr_sleep
                     << " ms" << endl
                     <<  "Time to get next number: "  << time * 100
                     << " ms" << endl
                     << "-------------------------"   << endl  << endl;
            }
        }
      } break;
      case -5: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'getRandomBytes'
        //
        // Concerns:
        //   Measure the amount of time required to acquire 'NUM_ITERATIONS'
        //   'int'(s).
        //
        // Plan:
        //   Call 'RandomDevice::getRandomBytes' 'NUM_ITERATIONS' times, each
        //   time
        //   requesting a 'int'.
        //
        // Testing:
        //   PERFORMANCE: 'getRandomBytes'
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE: 'getRandomBytes'" << endl
                          << "=============================" << endl;

        bsls::Stopwatch  s;
        const int        NUM_ITERATIONS = 15;
        int              rand_int;
        unsigned char   *p1 = reinterpret_cast<unsigned char *>(&rand_int);

        s.start(true);
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            if (veryVerbose) { P(i) }
            ASSERT(0 == Util::getRandomBytes(p1, sizeof rand_int));
        }
        s.stop();
        double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                      s.accumulatedWallTime();
        s.reset();
        if (verbose) {
            cout << "Time to aquire " << NUM_ITERATIONS <<  " random ints: "
                 << time << endl;
        }
      } break;
      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
