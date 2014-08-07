// bdlb_sysrandom.t.cpp                                               -*-C++-*-
#include <bdlb_sysrandom.h>

// Note the headers are in non-standard order.  This was required to silence an
// error from clang 3.4.
//..
//  /usr/include/unistd.h:449:12: error: declaration conflicts with target of
//  using declaration already in scope
//  extern int rename(const char *, const char *);
//..
// This is an acknowledged issue in LLVM:
// <http://lists.cs.uiuc.edu/pipermail/llvmbugs/2012-May/023328.html>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>              // for benchmarking only

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>                     // Sleep()
#else
#include <unistd.h>                      // usleep()
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
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------
#if defined(BSLS_PLATFORM_OS_WINDOWS)
#define sleep(x) Sleep((x))
#else
#define sleep(x) usleep((x) * 100)
#endif

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
template <class CHOICE_TYPE>
class RandomChoice {
  // This class manages selecting pseudo-random elements out of an array
  // sampling with replacement.

  // DATA
    CHOICE_TYPE *d_choices;  // the possibilities (used not owned)
    int          d_size;     // the number of elements to choose amongst

  public:
    // CREATORS
    RandomChoice(CHOICE_TYPE choices[], int numChoices);
        // Create an object to return a random one of the first specified
        // 'numChoices' elements of the specified 'choices' array.

    ~RandomChoice();
        // Delete this object

    // ACCESSOR
    const CHOICE_TYPE& choice() const;
        // Return a random member of the 'choices', sampling with replacement.
};

// CREATORS
template <class CHOICE_TYPE>
RandomChoice<CHOICE_TYPE>::RandomChoice(CHOICE_TYPE choices[],
                                        int         numChoices)
: d_choices(choices), d_size(numChoices)
{
}

template <class CHOICE_TYPE>
RandomChoice<CHOICE_TYPE>::~RandomChoice()
{
}

// ACCESSORS
template <class CHOICE_TYPE>
const CHOICE_TYPE& RandomChoice<CHOICE_TYPE>::choice() const
{
    int index;
    bdlb::SysRandom::getRandomBytesNonBlocking(
                                     reinterpret_cast<unsigned char *>(&index),
                                     sizeof(index));
    return d_choices[index % d_size];
}

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

        if (verbose) cout << "\nTesting usage example." << endl;
///Usage
///-----
  // The following snippets of code illustrate how to create and use a
  // 'bdlb_SysRandom' object.
  //..
//..
// Initialize an array of colors to choose between.
//..
    bsl::string colors[] = {"Red" , "Orange", "Yellow", "Green",
                       "Blue", "Indigo", "Violet"};
    unsigned numColors = sizeof(colors)/sizeof(colors[0]);
//..
// Request a random color.
//..
    RandomChoice<string> chooseColor(colors, numColors);
//..
// Finally we stream the value of this color to 'stdout':
//..
if (verbose)
    cout << chooseColor.choice() << endl;
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'int getRandomBytesNonBlocking(buf, numB)' TEST
        //
        // Concerns:
        //   1) If a number is passed, that many bytes are set.
        //   2) The random bytes are distributed uniformly (probabilistic)
        //
        // Plan:
        //   Request a large pool and random bytes from non-blocking random
        //   number generator.  Verify that each is unique. Verify that the
        //   numbers approximate a uniform distribution.
        //
        // Testing:
        //   static int getRandomBytesNonBlocking(buf, numB);
        // --------------------------------------------------------------------
        enum { NUM_ITERATIONS = 25};
        enum { NUM_TRIALS = 10};
        int cnt = 0;
        unsigned char buffer [NUM_ITERATIONS * 4]      = {0};
        unsigned char prev_buffer [NUM_ITERATIONS * 4] = {0};
        const unsigned NUM_BYTES = sizeof(buffer);

        if (verbose)
          cout << endl
               << "'int getRandomBytesNonBlocking(buf, numB)' TEST" << endl
               << "===============================================" << endl;

        // 1) If a number is passed, that many bytes are set.
        if (veryVerbose) cout << "\nTesting the number of bytes set."
                              << endl;
        for (unsigned i = 0; i < 5; ++i) {
            memset(buffer, 0, NUM_BYTES);
            // Repeat the accession of random bytes 'NUM_TRIALS' times to
            // prevent false negatives
            for (int j = 0; j < NUM_TRIALS; ++j) {
                ASSERT(0 == bdlb::SysRandom::getRandomBytesNonBlocking(buffer,
                                                                       i));
                // sum the bytes
                for (unsigned k = 0; k < NUM_BYTES; ++k) {
                    buffer[k] =  static_cast<unsigned char>(buffer[k] +
                                                            prev_buffer[k]);
                }
                // copy the buffer
                memcpy(prev_buffer, buffer, sizeof(buffer));
                if (veryVerbose) P(buffer[i])
            }
            int sum = 0;
            // check that the bytes set are non-zero
            unsigned j;
            for (j = 0; j < i; ++j)    {
                if (veryVerbose) cout << "Random bytes: " << buffer[j] << endl;
                sum += static_cast<int>(buffer[j]) ;
            }
            if (i > 0) {
              LOOP_ASSERT(i, 0 != sum)
            }
            //
            for (; j < NUM_BYTES; ++j) {
                if (veryVerbose) cout << "Random bytes: " << buffer[j] << endl;
                LOOP2_ASSERT(i, j, 0 == buffer[j]);
            }
        }

        if (veryVerbose) cout << "\nTesting the distribution of rand."
                              << endl;
        // 3) The random bytes are uniformly distributed (probabilistic)
        int numbers[NUM_ITERATIONS] = {0};
        for (int i = 0; i< NUM_ITERATIONS; ++i) {
            int rand;
            ASSERT(0 == bdlb::SysRandom::getRandomBytesNonBlocking(
                                      reinterpret_cast<unsigned char *>(&rand),
                                      sizeof(rand)));
            numbers[i] = rand;
            if (veryVerbose) cout << "rand[" << i << "]: " << rand << endl;
            for (int j = 0; j < i; ++j) {
                ASSERT(numbers[j] != rand);
                if (veryVerbose)
                    cout << "rand[" << j << "]: " << numbers[j] << endl;
            }

            for (int b = 0; b < 15; ++b) {
                cnt += rand & 1;
                rand >>= 1;
                if (veryVerbose) cout << "Cnt:  " << cnt << endl;
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
        enum { NUM_ITERATIONS = 8};

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        if (verbose) cout << "testing random" << endl;
        int rand;
        int numbers [NUM_ITERATIONS];
        // fill buffer with random bytes
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            unsigned char *p = reinterpret_cast<unsigned char *>(&rand);
            ASSERT(0 == bdlb::SysRandom::getRandomBytes(p, sizeof(rand)));
            numbers[i] = rand;
            if (veryVerbose) P(rand);
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
        enum { NUM_ITERATIONS = 25};
        int cnt = 0;
        int numbers [NUM_ITERATIONS]      = {0};
        // int prev_numbers [NUM_ITERATIONS] = {0};
        const unsigned NUM_BYTES = sizeof(numbers);

        if (verbose)
          cout << endl
               << "'int getRandomBytes(void *buf, int numBytes) TEST'" << endl
               << "==================================================" << endl;

        if (veryVerbose) cout << "\nTesting the number of bytes set."
                              << endl;
        // 2) If a number is passed, that many bytes are set.
        for (unsigned i = 0; i < 5; ++i) {
            unsigned j;
            unsigned char *p = reinterpret_cast<unsigned char *>(numbers);
            memset(numbers, 0, NUM_BYTES);
            ASSERT(0 == bdlb::SysRandom::getRandomBytesNonBlocking(p,
                                                                   i));

            for (j = 0; j < i; ++j)    {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i, j, 0 != p[j]);
            }
            for (; j < NUM_BYTES; ++j) {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i, j, 0 == p[j]);
            }
        }

        if (veryVerbose) cout << "\nTesting the distribution of rand."
                              << endl;

        // 3) The random bytes are distributed uniform (probabilistic)
        for (int i = 0; i<NUM_ITERATIONS; ++i) {
            int rand_int = 0;
            unsigned char *p1 = reinterpret_cast<unsigned char *>(rand_int);
            ASSERT(0 == bdlb::SysRandom::getRandomBytesNonBlocking(
                                                            p1,
                                                            sizeof(rand_int)));
            numbers[i] = rand_int;
            if (veryVerbose) cout << "rand[" << i << "]: " << rand_int << endl;
            for (int j = 0; j < i; ++j) {
                ASSERT(numbers[j] != rand_int);
                if (veryVerbose)
                    cout << "rand_int[" << j << "]: " << numbers[j] << endl;
            }

            for (int b = 0; b < 15; ++b) {
                cnt += rand_int & 1;
                rand_int >>= 1;
                if (veryVerbose) cout << "Cnt:  " << cnt << endl;
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
        enum { NUM_ITERATIONS = 4};
        bsls::Stopwatch s;
        const int MAX_GRANUALARITY = 1 << NUM_ITERATIONS;
        unsigned char buffer [MAX_GRANUALARITY] = {0};
        unsigned char prev_buffer[MAX_GRANUALARITY] = {0};
        int i, granularity;
        for (granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1)
        {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            for (i = 0; i <= MAX_GRANUALARITY; i +=  granularity)
            {
                ASSERT(0 ==
                       bdlb::SysRandom::getRandomBytes(buffer + granularity,
                                                       granularity));
            }
            s.stop();
            LOOP2_ASSERT(granularity,
                         i,
                         0 != memcmp(buffer, prev_buffer, MAX_GRANUALARITY));
            double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                          s.accumulatedWallTime();
            cout << "Granularity : " << granularity  << endl
                 << "Time Elapsed: " << time << endl
                 << "--------------" << endl << endl;
            s.reset();
        }
    } break;
    case -3: {
      // ----------------------------------------------------------------------
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
        if (verbose) cout << endl
                         << "PERFORMANCE: 'getRandomBytesNonBlocking'" << endl
                         << "========================================" << endl;
        int rand_int;
        for (int i = 0; i < 15; ++i) {
            ASSERT(0 ==
                 bdlb::SysRandom::getRandomBytesNonBlocking(
                                  reinterpret_cast<unsigned char *>(&rand_int),
                                                            sizeof(rand_int)));
        }

        enum { NUM_ITERATIONS = 4};
        bsls::Stopwatch s;

        const int MAX_GRANUALARITY = 1 << NUM_ITERATIONS;
        unsigned char buffer [MAX_GRANUALARITY] = {0};
        unsigned char prev_buffer[MAX_GRANUALARITY] = {0};
        int i, granularity;
        for (granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1)
        {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            for (i = 0; i <= MAX_GRANUALARITY; i +=  granularity)
            {
                ASSERT(0 ==
                      bdlb::SysRandom::getRandomBytesNonBlocking(
                                                          buffer + granularity,
                                                          granularity));
            }
            s.stop();
            LOOP2_ASSERT(granularity,
                         i,
                         0 != memcmp(buffer, prev_buffer, MAX_GRANUALARITY));
            double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                          s.accumulatedWallTime();
            if (veryVerbose) cout << "Granularity : " << granularity  << endl
                                  << "Time Elapsed: " << time << endl
                                  << "-------------"  << endl << endl;
            s.reset();
        }
    } break;
    case -4: {
      // ----------------------------------------------------------------------
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
      bsls::Stopwatch s;

      int rand_int;
      unsigned char *p1 = reinterpret_cast<unsigned char *>(&rand_int);
      for (int i = 0; i < 15; ++i) {
         ASSERT(0 == bdlb::SysRandom::getRandomBytes(p1,
                                                     sizeof(int)));
      }
      enum { MAX_SLEEP = 10};
      for (unsigned curr_sleep = 0; curr_sleep <= MAX_SLEEP; ++curr_sleep) {
          ASSERT(0 == bdlb::SysRandom::getRandomBytes(p1,
                                                      sizeof(int)));
          ///sleep(curr_sleep);
          s.start(true);
          ASSERT(0 == bdlb::SysRandom::getRandomBytes(p1,
                                                      sizeof(int)));
          s.stop();
          double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                        s.accumulatedWallTime();
          s.reset();
      if (veryVerbose)
          cout << "Current Delay             : " << curr_sleep
                << " ms" << endl
                <<  "Time to get next number: "  << time * 100
                << " ms" << endl
                << "-------------------------"   << endl  << endl;
      }
    } break;
    case -5: {
      // ----------------------------------------------------------------------
      // PERFORMANCE: 'getRandomBytes'
      //
      // Concerns:
      //   Measure the amount of time required to acquire 'NUM_ITERATIONS'
      //   'int'(s).
      //
      // Plan:
      //   Call 'SysRandom::getRandomBytes' 'NUM_ITERATIONS' times, each
      //   time
      //   requesting a 'int'.
      //
      // Testing:
      //   PERFORMANCE: 'getRandomBytes'
      // ----------------------------------------------------------------------
      if (verbose)
            cout << endl
                 << "PERFORMANCE: 'getRandomBytes'" << endl
                 << "=============================" << endl;
        bsls::Stopwatch s;
        int rand_int;
        unsigned char *p1 = reinterpret_cast<unsigned char *>(&rand_int);
        enum {NUM_ITERATIONS = 15};

        s.start(true);
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
                ASSERT(0 == bdlb::SysRandom::getRandomBytes(p1,
                                                      sizeof(int)));
        }
        s.stop();
        double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                      s.accumulatedWallTime();
        s.reset();
        if (verbose)
            cout << "Time to aquire " << NUM_ITERATIONS <<  " random ints: "
                 << time << endl;
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------------------------------------------------------
