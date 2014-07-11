// bdlb_sysrandom.t.cpp                                               -*-C++-*-
#include <bdlb_sysrandom.h>

// Note the headers are in non-standard order. This was reqired to silence an
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

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// [-1] static int randomN(void *buffer, unsigned numBytes = 1);
// [ 2] static int urandomN(void *buffer, unsigned numBytes = 1);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-2] PERFORMANCE: 'RANDOMN'
// [-3] PERFORMANCE: 'URANDOMN'
// [-4] PERFORMANCE: 'RANDOMN'
// [-5] PERFORMANCE: 'RANDOMN'
// [ 3] USAGE EXAMPLE
// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
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
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
template <class CHOICE_TYPE>
class RandomChoice
{
    //DATA
    CHOICE_TYPE *d_choices;  // the possibilities
    unsigned     d_size;

  public:
    // CREATORS
    RandomChoice(CHOICE_TYPE choices[], unsigned numChoices);
        // Create an object to return a random one of the first specified
        // 'numChoices' elements of the specified 'choices' array.

    ~RandomChoice();
        // Delete this object

    // ACCESSOR
    const CHOICE_TYPE& choice() const;
        // Return a random member of the 'choices'.
};

// CREATORS
template <class CHOICE_TYPE>
RandomChoice<CHOICE_TYPE>::RandomChoice(CHOICE_TYPE choices[],
                                        unsigned     numChoices)
: d_size(numChoices)
{
    d_choices = new CHOICE_TYPE[numChoices];
    for (unsigned i = 0; i < numChoices; ++i)
    {
        d_choices[i] = choices[i];
    }
}

template <class CHOICE_TYPE>
RandomChoice<CHOICE_TYPE>::~RandomChoice()
{
    delete  [] d_choices;
}

// ACCESSORS
template <class CHOICE_TYPE>
const CHOICE_TYPE& RandomChoice<CHOICE_TYPE>::choice() const
{
    int index;
    bdlb::SysRandom::urandomN(&index, sizeof(index));
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
    string colors[] = {"Red" , "Orange", "Yellow", "Green",
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
        // STATIC INT URANDOMN(VOID *BUFFER, UNSIGNED NUMBYTES) TEST
        //
        // Concerns:
        //   1) The default number of random bytes is 1.
        //   2) If a number is passed, that many bytes are set.
        //   3) The random bytes are distributed normally (probabilistic)
        //
        // Plan:
        //   Request a large pool and random bytes from non-blocking random 
        //   number generator.  Verify that each is unique. Verify that the 
        //   numbers approximate a uniform distribution.
        //
        // Testing:
        //   static int urandomN(void *buffer, unsigned numBytes = 1);
        // --------------------------------------------------------------------
        enum { NUM_ITERATIONS = 25};
        int cnt = 0;
        int numbers [NUM_ITERATIONS]      = {0};
        int prev_numbers [NUM_ITERATIONS] = {0};
        const unsigned NUM_BYTES = sizeof(numbers);
        
        if (verbose)
          cout << endl
               << "STATIC INT URANDOMN(VOID *BUFFER, UNSIGNED NUMBYTES) TEST" 
               << endl
               << "=========================================================" 
               << endl;
        //   1) The default number of random bytes is 1.
        if (veryVerbose) cout << "\nTesting the default." << endl;
        for (unsigned i = 0; i < NUM_BYTES; ++i) {
             char *p  = reinterpret_cast<char *>  (numbers) + i;
             char *p2 = reinterpret_cast<char *>(prev_numbers) + i;
             ASSERT(0 == bdlb::SysRandom::urandomN(p));
             if (veryVerbose) cout << "random[" << i <<"]: " << *p << endl;
             if (i > 0)  {
                 ASSERT(0 == memcmp(numbers, prev_numbers, i - 1));
             }
             *p2 = *p;
        }
        
        // 2) If a number is passed, that many bytes are set.
        if (veryVerbose) cout << "\nTesting the number of bytes set."
                              << endl;
        for (unsigned i = 0; i < 5; ++i) {
            unsigned j;
            char *p = reinterpret_cast<char *>(numbers);
            memset(numbers, 0, NUM_BYTES);
            ASSERT(0 == bdlb::SysRandom::urandomN(numbers, i));

            for (j = 0; j < i; ++j)    {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i,j, 0 != p[j]);
            }
            for (; j < NUM_BYTES; ++j) {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i,j, 0 == p[j]);
            }
        }

        if (veryVerbose) cout << "\nTesting the distribution of rand."
                              << endl;
        // 3) The random bytes are distributed normally (probabilistic)
        for (int i = 0; i<NUM_ITERATIONS; ++i) {
            int rand;
            ASSERT(0 == bdlb::SysRandom::urandomN(&rand, sizeof(rand)));
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
        enum { NUM_ITERATIONS = 15};

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        if (verbose) cout << "testing random" << endl;
        int rand;
        ASSERT(0 == bdlb::SysRandom::randomN(&rand, sizeof(rand)));
        for (char*p1 = reinterpret_cast<char *>(&rand);
             p1 < reinterpret_cast<char *>(&rand + 1);
             ++p1)
        {
            for (char*p2 = p1 + 1;
                 p2 < reinterpret_cast<char *>(&rand + 1);
                 ++p2)
            {
                ASSERT(*p1 != *p2);
                if (veryVerbose) cout << *p1 << ": " << *p2 << endl;
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // STATIC INT RANDOMN(VOID *BUFFER, UNSIGNED NUMBYTES = 1)
        //
        // Concerns:
        //   1) The default number of random bytes is 1.
        //   2) If a number is passed, that many bytes are set.
        //   3) The random bytes are distributed normally (probabilistic)
        //
        // Plan:
        //   Request a large pool and random bytes from non-blocking random 
        //   number generator.  Verify that each is unique. Verify that the 
        //   numbers approximate a uniform distribution.
        //
        // Testing:
        //   static int randomN(void *buffer, unsigned numBytes = 1);
        // --------------------------------------------------------------------
        enum { NUM_ITERATIONS = 25};
        int cnt = 0;
        int numbers [NUM_ITERATIONS]      = {0};
        int prev_numbers [NUM_ITERATIONS] = {0};
        const unsigned NUM_BYTES = sizeof(numbers);
        
        if (verbose)
          cout << endl
               << "STATIC INT RANDOMN(VOID *BUFFER, UNSIGNED NUMBYTES = 1);"
               << endl
               << "========================================================"
               << endl;

        if (veryVerbose) cout << "\nTesting the default." << endl;

        //   1) The default number of random bytes is 1.
        for (unsigned i = 0; i < NUM_BYTES; ++i) {
             char *p  = reinterpret_cast<char *>  (numbers) + i;
             char *p2 = reinterpret_cast<char *>(prev_numbers) + i;
             ASSERT(0 == bdlb::SysRandom::urandomN(p));
             if (veryVerbose) cout << "random[" << i <<"]: " << *p << endl;
             if (i > 0)  {
                 ASSERT(0 == memcmp(numbers, prev_numbers, i - 1));
             }
             *p2 = *p;
        }

        if (veryVerbose) cout << "\nTesting the number of bytes set."
                              << endl;
        // 2) If a number is passed, that many bytes are set.
        for (unsigned i = 0; i < 5; ++i) {
            unsigned j;
            char *p = reinterpret_cast<char *>(numbers);
            memset(numbers, 0, NUM_BYTES);
            ASSERT(0 == bdlb::SysRandom::urandomN(numbers, i));

            for (j = 0; j < i; ++j)    {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i,j, 0 != p[j]);
            }
            for (; j < NUM_BYTES; ++j) {
                if (veryVerbose) cout << "Random bytes: " << p[j] << endl;
                LOOP2_ASSERT(i,j, 0 == p[j]);
            }
        }

        if (veryVerbose) cout << "\nTesting the distribution of rand."
                              << endl;
        
        // 3) The random bytes are distributed normally (probabilistic)
        for (int i = 0; i<NUM_ITERATIONS; ++i) {
            int rand;
            ASSERT(0 == bdlb::SysRandom::urandomN(&rand, sizeof(rand)));
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
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'RANDOMN'
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
        //   PERFORMANCE: 'RANDOMN'
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE: 'RANDOMN'" << endl
                          << "======================" << endl;
        enum { NUM_ITERATIONS = 4};
        bsls::Stopwatch s;
        const int MAX_GRANUALARITY = 1 << NUM_ITERATIONS;
        char buffer [MAX_GRANUALARITY] = {0};
        char prev_buffer[MAX_GRANUALARITY] = {0};
        int i, granularity;
        for (granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1)
        {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            for (i = 0; i <= MAX_GRANUALARITY; i +=  granularity)
            {
                ASSERT(0 == bdlb::SysRandom::randomN(buffer + granularity,
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
      // PERFORMANCE: 'URANDOMN'
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
      //   PERFORMANCE: 'URANDOMN'
      //---------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE: 'URANDOMN'" << endl
                          << "=======================" << endl;
        int rand_int;
        for (int i = 0; i < 15; ++i) {
            ASSERT(0 == bdlb::SysRandom::urandomN(&rand_int,
                                                  sizeof(rand_int)));
        }
          enum { NUM_ITERATIONS = 4};
          bsls::Stopwatch s;
        const int MAX_GRANUALARITY = 1 << NUM_ITERATIONS;
        char buffer [MAX_GRANUALARITY] = {0};
        char prev_buffer[MAX_GRANUALARITY] = {0};
        int i, granularity;
        for (granularity = 1;
             granularity < MAX_GRANUALARITY;
             granularity <<= 1)
        {
            memcpy(prev_buffer, buffer, MAX_GRANUALARITY);
            s.start(true);
            for (i = 0; i <= MAX_GRANUALARITY; i +=  granularity)
            {
                ASSERT(0 == bdlb::SysRandom::urandomN(buffer + granularity,
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
                                  << "-------------" << endl << endl;
            s.reset();
        }
    } break;
    case -4: {
      // ----------------------------------------------------------------------
      // PERFORMANCE: 'RANDOMN'
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
      //   PERFORMANCE: 'RANDOMN'
      //---------------------------------------------------------------------
      if (verbose) cout << "PERFORMANCE: 'RANDOMN'"
                        << "======================" << endl;
      bsls::Stopwatch s;

      int rand_int;
      for (int i = 0; i < 15; ++i) {
         ASSERT(0 == bdlb::SysRandom::randomN(&rand_int, sizeof(rand_int)));
      }
      enum { MAX_SLEEP = 10};
      for (unsigned curr_sleep = 0; curr_sleep <= MAX_SLEEP; ++curr_sleep) {
          ASSERT(0 == bdlb::SysRandom::randomN(&rand_int, sizeof(rand_int)));
          ///sleep(curr_sleep);
          s.start(true);
          ASSERT(0 == bdlb::SysRandom::randomN(&rand_int, sizeof(rand_int)));
          s.stop();
          double time = s.accumulatedUserTime() + s.accumulatedSystemTime() +
                        s.accumulatedWallTime();
          s.reset();
      if (veryVerbose)
          cout << "Current Delay             : " << curr_sleep
                << " ms" << endl
                <<  "Time to get next number: "   << time * 100
                << " ms" << endl
                << "-------------------------"    << endl  << endl;
      }
    } break;
    case -5: {
      // ----------------------------------------------------------------------
      // PERFORMANCE: 'RANDOMN'
      //
      // Concerns:
      //      Measure the amount of time required to acquire 'NUM_ITERATIONS'
      //       'int'(s).
      //
      //  Plan:
      //      Call 'SysRandom::randomN' 'NUM_ITERATIONS' times, each time re
      //       requesting a 'int'.
      //
      // Testing:
      //   PERFORMANCE: 'RANDOMN'
      // ----------------------------------------------------------------------
      if (verbose)
            cout << endl
                 << "PERFORMANCE: 'RANDOMN'" << endl
                 << "======================" << endl;
        bsls::Stopwatch s;
        int rand_int;
        enum {NUM_ITERATIONS = 15};

        s.start(true);
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
                ASSERT(0 == bdlb::SysRandom::randomN(&rand_int,
                                                      sizeof(rand_int)));
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
