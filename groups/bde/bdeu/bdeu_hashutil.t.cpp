// bdeu_hashutil.t.cpp    -*-C++-*-

#include <bdeu_hashutil.h>

#include <bsls_platformutil.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cmath.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t";   \
               cout << #J << ": " << J << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t";   \
               cout << #J << ": " << J << "\t";   \
               cout << #K << ": " << K << "\n";   \
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

const int LENGTH = 1257;  // not a power of two

void time_hash1(const char   *key,
                int           length)
{
    enum { ITERATIONS = 1000000 }; // 1M
    unsigned int value = 0;
    bsls_Stopwatch timer;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        value += bdeu_HashUtil::hash1(key, length) % LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type (key == const char*, length == " << length << ")"
         << endl;
    (void)value;
}

template <class TYPE>
void time_hash1(const TYPE&   key,
                const char   *TYPEID)
{
    enum { ITERATIONS = 1000000 }; // 1M
    unsigned int value = 0;
    bsls_Stopwatch timer;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        value += bdeu_HashUtil::hash1(key) % LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type " << TYPEID << endl;
    (void)value;
}

void time_hash2(const char   *key,
                int           length)
{
    enum { ITERATIONS = 1000000 }; // 1M
    unsigned int value = 0;
    bsls_Stopwatch timer;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        value += bdeu_HashUtil::hash2(key, length) % LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type (key == const char*, length == " << length << ")"
         << endl;
    (void)value;
}

template <class TYPE>
void time_hash2(const TYPE&   key,
                const char   *TYPEID)
{
    enum { ITERATIONS = 1000000 }; // 1M
    unsigned int value = 0;
    bsls_Stopwatch timer;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        value += bdeu_HashUtil::hash2(key) % LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type " << TYPEID << endl;
    (void)value;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BDEU_HASHUTIL_USAGE_EXAMPLE {

// We illustrate the usage of this component by some experiments (code and
// results) on the number of collision expected for various usage.  For
// returning the results of an experiment, we will use the following structure:
//..
    struct ExperimentalResult {
        // DATA MEMBERS
        int    d_max;     // maximum length of a chain
        double d_average; // average length of a chain
        double d_sigma;   // standard deviation
        // CREATORS
        explicit
        ExperimentalResult(int max = 0, double avg = 0, double sigma = 0)
            // Create an experimental result reporting the specified 'max',
            // 'avg', and 'sigma' values.
        : d_max(max), d_average(avg), d_sigma(sigma)
        {}
    };
//..
// For generating the data, we use a parameterized 'GENERATOR' of which we give
// three implementations below (sequential integers, character strings, and
// multiple-field keys).  This template parameter needs to validate the
// following syntactic expressions, which we call in this usage example our
// 'GeneratorConcept'.  In those requirements, 'generator' is an instance of
// 'GENERATOR'.
//..
//  const char *data = generator.data(); // address of storage of current value
//  int length = generator.length();     // length of storage of current value
//  generator.next();                    // advance to next value
//..
//
///Using chaining
/// - - - - - - -
// The following code will check the number of collisions for a chaining-based
// hash table given a certain distribution of keys:
//..
    template <class GENERATOR>
    ExperimentalResult
    computeChainingCollisions(int numElements, int size, GENERATOR input,
                              int veryVerbose, int veryVeryVerbose)
        // Simulate insertion of the specified 'numElements' generated by the
        // specified 'input' generator into a hash table of the specified
        // 'size' using chaining as the mechanism to resolve collisions.
        // Return the results (maximum and average length of a chain, with
        // standard deviation).  'GENERATOR' must be a model of the
        // 'GeneratorConcept'.  For the good functioning of this function, it
        // is required that 'input' never be default nor repeat itself.
    {
        bsl::vector<int> table(size);   // all counters are init'ed to 0

        for (int i = 0; i < numElements; ++i, input.next()) {
            unsigned int hash = bdeu_HashUtil::hash1(input.data(),
                                                     input.length());
            ++table[hash % size];
        }

        double avgLength    = 0; // average number of collisions
        double sigmaLength  = 0; // standard deviation from average
        int    maxLength    = 0; // maximum length of a chain

        for (int k = 0; k < size; ++k) {
            avgLength   += table[k];
            sigmaLength += table[k] * table[k];
            maxLength    = bsl::max(maxLength, table[k]);
        }
        avgLength /= size;
        sigmaLength = sqrt( sigmaLength / size - avgLength * avgLength );

        return ExperimentalResult(maxLength, avgLength, sigmaLength);
    }
//..
///Using double-hashing
/// - - - - - - - - - -
// The following code will check the number of collisions for a double-hashing
// based hash table (such as 'bdecs_hashtable2') given a certain distribution
// of keys:
//..
    template <class GENERATOR>
    ExperimentalResult
    computeDoubleHashingCollisions(int numElements, int size, GENERATOR input,
                                   int veryVerbose, int veryVeryVerbose)
        // Simulate insertion of the specified 'numElements' generated by the
        // specified 'input' generator into a hash table of the specified
        // 'size' using double hashing as the mechanism to resolve collisions.
        // Return the results (maximum and average length of a chain, with
        // standard deviation).  'GENERATOR' must be a model of the
        // 'GeneratorConcept'.  For the good functioning of this function, it
        // is required that 'input' never be default nor repeat itself.
    {
        typedef typename GENERATOR::ResultType  ResultType;
        bsl::vector<ResultType> table(size); // all counters are default-ct'ed

        double avgLength    = 0; // average number of collisions
        double sigmaLength  = 0; // standard deviation from average
        int    maxLength    = 0; // maximum length of a chain

        for (int i = 0; i < numElements; ++i, input.next()) {
            unsigned int hash1 = bdeu_HashUtil::hash1(input.data(),
                                                      input.length());

            int chainLength = 0;
            int bucket = hash1 % size;
            if (ResultType() == table[bucket]) {
                table[bucket] = input.current();
            } else {
                unsigned int hash2 = bdeu_HashUtil::hash2(input.data(),
                                                          input.length());
                hash2 = 1 + hash2 % (size - 1);

                while (++chainLength < size) {
                    bucket = (bucket + hash2) % size;
                    if (veryVeryVerbose) { T_(i); P(bucket); }

                    if (ResultType() == table[bucket]) {
                        table[bucket] = input.current();
                        break; // while loop
                    }
                }
            }

            if (chainLength == size) {
                bsl::cerr << "\tCould not insert in doubly-hashed table\n";
                avgLength   = bsl::numeric_limits<double>::infinity();
                sigmaLength = bsl::numeric_limits<double>::infinity();
                maxLength   = bsl::numeric_limits<double>::infinity();
                return ExperimentalResult(maxLength, avgLength, sigmaLength);
            }

            avgLength   += chainLength;
            sigmaLength += chainLength * chainLength;
            maxLength    = bsl::max(maxLength, chainLength);
        }
        avgLength /= numElements;
        sigmaLength = sqrt(sigmaLength / numElements - avgLength * avgLength);

        return ExperimentalResult(maxLength, avgLength, sigmaLength);
    }
//..
///Sequential integers generator
///- - - - - - - - - - - - - - -
// Here is a simple generator that returns a sequence of integers.  This
// generator has a period of 'INT_MAX / gcd(increment, INT_MAX+1)'.
//..
    class SequentialIntegers {
        int d_current;
        int d_inc;

      public:
        // TYPES
        typedef int ResultType;
            // Type returned by this generator.

        // CREATORS
        SequentialIntegers(int first = 1, int increment = 1)
            // Create a generator returning integers in a sequence starting at
            // the specified 'first' integer, with the specified 'increment'.
            : d_current(first), d_inc(increment) {}

        // MANIPULATORS
        void next()
            // Advance to the next element.
        {
            d_current += d_inc;
        }

        // ACCESSORS
        ResultType current() const
            // Return current element.
        {
            return d_current;
        }

        const char *data() const
            // Return data buffer of result type.
        {
            return reinterpret_cast<const char*>(&d_current);
        }

        int length() const
            // Return length of result type (in bytes).
        {
            return sizeof(ResultType);
        }
    };
//..
///Character strings generator
///- - - - - - - - - - - - - -
// Here is a simple generator that returns a sequence of strings that are a
// permutation of an initial string.  This generator has a period of
// 'factorial(initial.length())' where 'factorial(N)' returns the number of
// permutations of 'N' distinct elements.
//..
    class SequentialStrings {
        int         d_length;
        bsl::string d_current;

      public:
        // TYPES
        typedef bsl::string ResultType;
            // Type returned by this generator.

        // CREATORS
        SequentialStrings(bsl::string const& initial)
            // Create a generator returning strings in a sequence starting at
            // the specified 'initial' string (sorted by characters) and
            // looping through all permutations of 'str'.  The behavior is
            // undefined unless all the characters of the 'initial' string are
            // distinct.
            : d_length(initial.length()), d_current(initial)
        {
            bsl::sort(d_current.begin(), d_current.end());
            ASSERT(bsl::unique(d_current.begin(), d_current.end()) ==
                                                              d_current.end());
        }

        // MANIPULATORS
        void next()
            // Advance to the next element.
        {
            bsl::next_permutation(d_current.begin(), d_current.end());
        }

        // ACCESSORS
        ResultType current() const
            // Return current element.
        {
            return d_current;
        }

        const char *data() const
            // Return data buffer of result type.
        {
            return d_current.data();
        }
        int length() const
            // Return length of result type (in bytes).
        {
            return d_current.length();
        }
    };
//..
///Multiple-field keys generator
///- - - - - - - - - - - - - - -
// It is not uncommon for keys to consist of concatenated keys in multiple
// combinations.  We simulate this by a character string in which each
// character loops through a specified number of values.  The period of this
// generator is the product of the length of each character range.
//..
    struct SequentialVector {
        bsl::vector<char> d_ranges;
        int               d_length;
        bsl::string       d_current;

      public:
        // TYPES
        typedef bsl::string ResultType;
            // Type returned by this generator.

        // CREATORS
        SequentialVector(bsl::vector<char> const& ranges)
            // Create a generator returning strings of length 'ranges.size()'
            // in a sequence starting at the string with all null characters
            // and looping through all the strings with each character at
            // position 'i' in the specified range from 0 until 'ranges[i]'
            // (excluded).  The behavior is undefined unless 'ranges' does not
            // contain zero entries.
            : d_ranges(ranges)
            , d_length(ranges.size())
            , d_current(d_length, (char)0)
        {
        }

        // MANIPULATORS
        void next()
            // Advance to the next element.
        {
            for (int i = 0;
                 i < d_length && ++d_current[i] == d_ranges[i]; ++i) {
                d_current[i] = 0;
            }
        }

        // ACCESSORS
        ResultType current() const
            // Return current element.
        {
            return d_current;
        }

        const char *data() const
            // Return data buffer of current value.
        {
            return d_current.data();
        }

        int length() const
            // Return length of current value (in bytes).
        {
            return d_current.length();
        }
    };
//..
///Experimental Results
///- - - - - - - - - -
// Checking the above distributions is done in a straightforward manner.  We do
// this at various load factors (the load factor is the percentage of the table
// that actually holds data; it is defined as 'N / SIZE' where 'N' is the
// number of elements and 'SIZE' is the size of the hash table).  Note that
// chaining accommodates arbitrary load factors, while double hashing requires
// that the load factor be strictly less than 1.
//..
    int usageExample(int verbose, int veryVerbose, int veryVeryVerbose) {
        const int SIZE = 10000;
        const int INC  = SIZE / 5; // load factors for every 20% percentile
        const int COLS = (4*SIZE)/INC;

        {
            ExperimentalResult results[3][COLS];
            bsls_Stopwatch timer;

            if (verbose) cout << "\nUsing chaining"
                              << "\n--------------" << endl;

            if (verbose) cout << "Sequential Integers\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                results[0][i] = computeChainingCollisions(n, SIZE,
                                                 SequentialIntegers(),
                                                 veryVerbose, veryVeryVerbose);
                ASSERT(results[0][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Strings\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                bsl::string initial("abcdefghij"); // period = 10! = 3628800
                results[1][i] = computeChainingCollisions(n, SIZE,
                                                 SequentialStrings(initial),
                                                 veryVerbose, veryVeryVerbose);
                ASSERT(results[1][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Vector\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                bsl::vector<char> ranges(6, (char)4); // period = 4^6 = 4096
                results[2][i] = computeChainingCollisions(n, SIZE,
                                          SequentialVector(ranges),
                                          veryVerbose, veryVeryVerbose);
                ASSERT(results[2][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) {
                cout << "\nDisplaying average (max) for chaining:";
                cout << "\n--------------------------------------n";
                const char *ROW_LABELS[] = {
                    "\nIntegers:",
                    "\nStrings :",
                    "\nVector  :",
                    "\nLoad factor:",
                };
                const int NUM_ROWS = sizeof ROW_LABELS / sizeof *ROW_LABELS -1;
                cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
                for (int n = INC; n < 4*SIZE; n += INC) {
                    cout << "\t" << (double)n / SIZE;
                }
                for (int j = 0; j < NUM_ROWS; ++j) {
                    cout << ROW_LABELS[j];
                    for (int i = 0; i < COLS; ++i) {
                        cout << "\t" << results[j][i].d_average;
                        cout << "(" << results[j][i].d_max << ")";
                    }
                    cout << "\n";
                }
            }
        }
//..
// We repeat the same steps with double hashing, except that due to the nature
// of the collision-resolution mechanism, the tolerance for the average must be
// slightly increased to 2.5 times the load factor, when the load factor is
// high.
//..
        {
            // const int SIZE = 1000003;   // must be a prime number
            const int SIZE = 10007;     // must be a prime number
            const int INC  = SIZE / 5; // load factors for every 20% percentile
            const int COLS = SIZE/INC;

            ExperimentalResult results[3][COLS];
            bsls_Stopwatch timer;

            if (verbose) cout << "\nUsing double hashing"
                              << "\n--------------------" << endl;

            if (verbose) cout << "Sequential Integers\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                results[0][i] = computeDoubleHashingCollisions(n, SIZE,
                                                 SequentialIntegers(),
                                                 veryVerbose, veryVeryVerbose);
                ASSERT(results[0][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Strings\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                bsl::string initial("abcdefghij"); // period = 10! = 3628800
                results[1][i] = computeDoubleHashingCollisions(n, SIZE,
                                                 SequentialStrings(initial),
                                                 veryVerbose, veryVeryVerbose);
                ASSERT(results[1][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Vector\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " << (double)n / SIZE << "\n";

                bsl::vector<char> ranges(7, (char)8); // period = 8^7 = 2097152
                results[2][i] = computeDoubleHashingCollisions(n, SIZE,
                                                 SequentialVector(ranges),
                                                 veryVerbose, veryVeryVerbose);
                ASSERT(results[2][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) {
                cout << "\nDisplaying average (max) for double-hashing:";
                cout << "\n--------------------------------------------\n";
                const char *ROW_LABELS[] = {
                    "\nIntegers:",
                    "\nStrings :",
                    "\nVector  :",
                    "\nLoad factor:",
                };
                const int NUM_ROWS = sizeof ROW_LABELS / sizeof *ROW_LABELS -1;
                cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
                for (int n = INC/2; n < SIZE; n += INC) {
                    cout << "\t" << (double)n / SIZE;
                }
                for (int j = 0; j < NUM_ROWS; ++j) {
                    cout << ROW_LABELS[j];
                    for (int i = 0; i < COLS; ++i) {
                        cout << "\t" << results[j][i].d_average;
                        cout << "(" << results[j][i].d_max << ")";
                    }
                    cout << "\n";
                }
            }
        }

        return 0;
    }
//..
// The above code produces the following results.  The results for chaining are
// identical for 'hash1' (used in the code above) or 'hash2' (code identical
// except 'hash2' is used in place of 'hash1' in 'computeChainingCollisions').
//..
// Displaying average(max) for chaining:
// --------------------------------------
// Load factor:    0.2     0.4     0.6     0.8     1       1.2     1.4
// Integers:       0.2(3)  0.4(5)  0.6(6)  0.8(6)  1(7)    1.2(7)  1.4(8)
// Strings :       0.2(4)  0.4(4)  0.6(5)  0.8(7)  1(7)    1.2(7)  1.4(7)
// Vector  :       0.2(5)  0.4(5)  0.6(10) 0.8(10) 1(15)   1.2(15) 1.4(20)
//
// Load factor:    1.6     1.8     2       2.2     2.4     2.6     2.8
// Integers:       1.6(9)  1.8(9)  2(10)   2.2(10) 2.4(10) 2.6(10) 2.8(11)
// Strings :       1.6(7)  1.8(8)  2(9)    2.2(11) 2.4(11) 2.6(11) 2.8(11)
// Vector  :       1.6(20) 1.8(24) 2(25)   2.2(29) 2.4(30) 2.6(34) 2.8(35)
//
// Load factor:    3       3.2     3.4     3.6     3.8
// Integers:       3(11)   3.2(11) 3.4(12) 3.6(12) 3.8(13)
// Strings :       3(12)   3.2(12) 3.4(13) 3.6(13) 3.8(13)
// Vector  :       3(39)   3.2(40) 3.4(42) 3.6(45) 3.8(46)
//
// Displaying average(max) for double-hashing:
// --------------------------------------------
// Load factor:    0.1      0.3     0.5      0.7      0.9
// Integers:       0.046(2) 0.20(4) 0.37(10) 0.71(15) 1.6(59)
// Strings :       0.064(2) 0.20(6) 0.40(12) 0.75(18) 1.6(50)
// Vector  :       0.05(2)  0.19(5) 0.58(9)  1.20(15) 2.4(64)
//..

} // close namespace BDEU_HASHUTIL_USAGE_EXAMPLE
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
        case 4: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        using namespace BDEU_HASHUTIL_USAGE_EXAMPLE;
        usageExample(verbose, veryVerbose, veryVeryVerbose);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HASH0 FUNCTIONS
        //
        // Concerns:
        //   That the hash return value is constant across all platforms for a
        //   given input.
        //
        // Plan:
        //   1. Using the table-driven technique, specifying a set of test
        //      vectors with the input and expected value, then verify the
        //      return value from the hash function.
        //
        // Testing:
        //   bdeu_HashUtil::hash0(char, int);
        //   bdeu_HashUtil::hash0(signed char, int);
        //   bdeu_HashUtil::hash0(unsigned char, int);
        //   bdeu_HashUtil::hash0(short, int);
        //   bdeu_HashUtil::hash0(unsigned short, int);
        //   bdeu_HashUtil::hash0(int, int);
        //   bdeu_HashUtil::hash0(unsigned int, int);
        //   bdeu_HashUtil::hash0(long, int);
        //   bdeu_HashUtil::hash0(unsigned long, int);
        //   bdeu_HashUtil::hash0(bsls_PlatformUtil::Int64, int);
        //   bdeu_HashUtil::hash0(bsls_PlatformUtil::Uint64, int);
        //   bdeu_HashUtil::hash0(float, int);
        //   bdeu_HashUtil::hash0(double, int);
        //   bdeu_HashUtil::hash0(void *, int);
        //   bdeu_HashUtil::hash0(const char *, int);
        //   bdeu_HashUtil::hash0(const char *, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING HASH0 FUNCTIONS"
                          << endl << "=======================" << endl;

        if (verbose) cout << "\nTesting 'char' hash" << endl;
        {
            static const struct {
                int  d_lineNum;     // source line number
                char d_value;       // value to hash
                int  d_size;        // size of target hash table
                int  d_exp;         // expected return value
            } DATA[] = {
                //line  value          size    exp
                //----  -------------  ------  ------
                { L_,              0,     23,      0 },
                { L_,             10,     23,     10 },
                { L_,      (char)246,     23,     16 },
                { L_,      (char)255,     23,      2 },
                { L_,              0,    257,      0 },
                { L_,             10,    257,     10 },
                { L_,             46,    257,     46 },
                { L_,            127,    257,    127 },
                { L_,      (char)128,    257,    128 },
                { L_,      (char)210,    257,    210 },
                { L_,      (char)246,    257,    246 },
                { L_,      (char)255,    257,    255 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const char VALUE = DATA[ti].d_value;
                const int  SIZE  = DATA[ti].d_size;
                const int  EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P_(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'short' hash" << endl;
        {
            static const struct {
                int   d_lineNum;     // source line number
                short d_value;       // value to hash
                int   d_size;        // size of target hash table
                int   d_exp;         // expected return value
            } DATA[] = {
                //line  value          size    exp
                //----  -------------  ------  ------
                { L_,              0,     23,      0 },
                { L_,             -1,     23,      8 },
                { L_,             10,     23,     10 },
                { L_,            -10,     23,     22 },
                { L_,              0,    257,      0 },
                { L_,             -1,    257,      0 },
                { L_,             10,    257,     10 },
                { L_,            -10,    257,    248 },
                { L_,             46,    257,     46 },
                { L_,            -46,    257,    212 },
                { L_,            127,    257,    127 },
                { L_,           -128,    257,    130 },
                { L_,          32767,    257,    128 },
                { L_,         -32768,    257,    129 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const short VALUE = DATA[ti].d_value;
                const int   SIZE  = DATA[ti].d_size;
                const int   EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'int' hash" << endl;
        {
            static const struct {
                int d_lineNum;     // source line number
                int d_value;       // value to hash
                int d_size;        // size of target hash table
                int d_exp;         // expected return value
            } DATA[] = {
                //line  value          size    exp
                //----  -------------  ------  ------
                { L_,              0,    257,      0 },
                { L_,             -1,    257,      0 },
                { L_,     2147483647,    257,    128 },
                { L_,  -2147483647-1,    257,    129 },  // windows hack
                { L_,              0,  65537,      0 },
                { L_,             -1,  65537,      0 },
                { L_,     2147483647,  65537,  32768 },
                { L_,  -2147483647-1,  65537,  32769 },  // windows hack
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const int  VALUE = DATA[ti].d_value;
                const int  SIZE  = DATA[ti].d_size;
                const int  EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P_(EXP);
                    P(bdeu_HashUtil::hash0(VALUE, SIZE));
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'int64' hash" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_valueA;   // first half of value to hash
                int d_valueB;   // second half of value to hash
                int d_size;     // size of hash table
                int d_exp;      // expected return value
            } DATA[] = {
                //line  valueA             valueB              size    exp
                //----  -----------------  -----------------  ------  ------
                { L_,                  0,                 0,    257,      0 },
                { L_,                 -1,                -1,    257,      0 },
                { L_,                  0,        2147483647,    257,    128 },
                { L_,                 -1,        0x80000000,    257,    129 },
                { L_,                  0,                 0,  65537,      0 },
                { L_,                 -1,                -1,  65537,      0 },
                { L_,                  0,        2147483647,  65537,  32768 },
                { L_,                 -1,        0x80000000,  65537,  32769 },
                { L_,         2147483647,                 0,  65537,  32768 },
                { L_,         0x12345678,                 0,  65537,  17476 },
                { L_,         0x12345678,        0x90ABCDEF,  65537,  63745 },
                { L_,         0x23453453,        0x33253452,  65537,  62008 },
                { L_,         0x99999999,        0x99999999,  65537,      0 },
                { L_,         0xFFFFFFFF,        0xFFFFFFFF,  65537,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int                      LINE  = DATA[ti].d_lineNum;
                const int                      VA    = DATA[ti].d_valueA;
                const int                      VB    = DATA[ti].d_valueB;
                const int                      SIZE  = DATA[ti].d_size;
                const int                      EXP   = DATA[ti].d_exp;
                const bsls_PlatformUtil::Int64 VALUE =
                                     (bsls_PlatformUtil::Int64(VA) << 32) |
                                     (VB & 0xFFFFFFFF);

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'float' hash" << endl;
        {
            static const struct {
                int   d_lineNum;     // source line number
                float d_value;       // value to hash
                int   d_size;        // size of target hash table
                int   d_exp;         // expected return value
            } DATA[] = {
                //line value                 size   exp
                //---- --------------------- ------ -----
                { L_,  0.0                 ,   257,     0 },
                { L_,  1.0                 ,   257,   177 },
                { L_,  2.0                 ,   257,   193 },
                { L_,  4.0                 ,   257,   209 },
                { L_,  0.5                 ,   257,   161 },
                { L_,  0.25                ,   257,   145 },
                { L_,  -1.0                ,   257,    49 },
                { L_,  -2.0                ,   257,    65 },
                { L_,  -4.0                ,   257,    81 },
                { L_,  -0.5                ,   257,    33 },
                { L_,  -0.25               ,   257,    17 },
                { L_,  1.1F                ,   257,    18 },
                { L_,  -1.1F               ,   257,   147 },
                { L_,  0.0                 , 65537,     0 },
                { L_,  1.0                 , 65537, 49169 },
                { L_,  2.0                 , 65537, 49153 },
                { L_,  4.0                 , 65537, 49137 },
                { L_,  0.5                 , 65537, 49185 },
                { L_,  0.25                , 65537, 49201 },
                { L_,  -1.0                , 65537, 16401 },
                { L_,  -2.0                , 65537, 16385 },
                { L_,  -4.0                , 65537, 16369 },
                { L_,  -0.5                , 65537, 16417 },
                { L_,  -0.25               , 65537, 16433 },
                { L_,  1.1F                , 65537, 47529 },
                { L_,  -1.1F               , 65537, 14761 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const float VALUE = DATA[ti].d_value;
                const int   SIZE  = DATA[ti].d_size;
                const int   EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'double' hash" << endl;
        {
            static const struct {
                int    d_lineNum;     // source line number
                double d_value;       // value to hash
                int    d_size;        // size of target hash table
                int    d_exp;         // expected return value
            } DATA[] = {
                //line value                 size   exp
                //---- --------------------- ------ -----
                { L_,  0.0                 ,   257,     0 },
                { L_,  1.0                 ,   257,   177 },
                { L_,  2.0                 ,   257,   193 },
                { L_,  4.0                 ,   257,   209 },
                { L_,  0.5                 ,   257,   161 },
                { L_,  0.25                ,   257,   145 },
                { L_,  -1.0                ,   257,    49 },
                { L_,  -2.0                ,   257,    65 },
                { L_,  -4.0                ,   257,    81 },
                { L_,  -0.5                ,   257,    33 },
                { L_,  -0.25               ,   257,    17 },
                { L_,  1.1                 ,   257,   144 },
                { L_,  -1.1                ,   257,    15 },
                { L_,  0.0                 , 65537,     0 },
                { L_,  1.0                 , 65537, 49169 },
                { L_,  2.0                 , 65537, 49153 },
                { L_,  4.0                 , 65537, 49137 },
                { L_,  0.5                 , 65537, 49185 },
                { L_,  0.25                , 65537, 49201 },
                { L_,  -1.0                , 65537, 16401 },
                { L_,  -2.0                , 65537, 16385 },
                { L_,  -4.0                , 65537, 16369 },
                { L_,  -0.5                , 65537, 16417 },
                { L_,  -0.25               , 65537, 16433 },
                { L_,  1.1                 , 65537,  9881 },
                { L_,  -1.1                , 65537, 42651 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE  = DATA[ti].d_lineNum;
                const double VALUE = DATA[ti].d_value;
                const int    SIZE  = DATA[ti].d_size;
                const int    EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'void *' hash" << endl;
        {
            static const struct {
                int   d_lineNum;     // source line number
                void *d_value;       // value to hash
                int   d_size;        // size of target hash table
                int   d_exp;         // expected return value
            } DATA[] = {
                //line  value          size    exp
                //----  -------------  ------  ------
                { L_,       (void *)0,     23,      0 },
                { L_,      (void *)10,     23,     10 },
                { L_,       (void *)0,    257,      0 },
                { L_,      (void *)10,    257,     10 },
                { L_,      (void *)46,    257,     46 },
                { L_,     (void *)127,    257,    127 },
                { L_,   (void *)32767,    257,    128 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const void *VALUE = DATA[ti].d_value;
                const int   SIZE  = DATA[ti].d_size;
                const int   EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(bdeu_HashUtil::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == bdeu_HashUtil::hash0(VALUE,SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'char *' hash" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // string
                int         d_size;     // size of hash table
                int         d_hash;     // expected output
            } DATA[] = {
                // line  string                size    exp
                // ----  ----------            ------  -----
                {  L_,   ""                  ,   256,      0  },
                {  L_,   "a"                 ,   256,     76  },
                {  L_,   "aa"                ,   256,    168  },
                {  L_,   "aaa"               ,   256,    148  },
                {  L_,   "aaaa"              ,   256,    208  },
                {  L_,   "aaaaa"             ,   256,     92  },
                {  L_,   "aaaaaa"            ,   256,    120  },
                {  L_,   "aaaaaaa"           ,   256,    164  },
                {  L_,   "aaaaaaaa"          ,   256,     96  },
                {  L_,   "b"                 ,   256,     89  },
                {  L_,   "bb"                ,   256,     94  },
                {  L_,   "bbb"               ,   256,    107  },
                {  L_,   "bbbb"              ,   256,    212  },
                {  L_,   "bbbbb"             ,   256,    157  },
                {  L_,   "bbbbbb"            ,   256,     82  },
                {  L_,   "bbbbbbb"           ,   256,    207  },
                {  L_,   "bbbbbbbb"          ,   256,     40  },
                {  L_,   ""                  , 65536,      0  },
                {  L_,   "a"                 , 65536,  40524  },
                {  L_,   "aa"                , 65536,  59816  },
                {  L_,   "aaa"               , 65536,  59540  },
                {  L_,   "aaaa"              , 65536,  26064  },
                {  L_,   "aaaaa"             , 65536,  41820  },
                {  L_,   "aaaaaa"            , 65536,  35704  },
                {  L_,   "aaaaaaa"           , 65536,  63908  },
                {  L_,   "aaaaaaaa"          , 65536,   8288  },
                {  L_,   "b"                 , 65536,   1113  },
                {  L_,   "bb"                , 65536,  44126  },
                {  L_,   "bbb"               , 65536,  39531  },
                {  L_,   "bbbb"              , 65536,  23508  },
                {  L_,   "bbbbb"             , 65536,   8093  },
                {  L_,   "bbbbbb"            , 65536,  11602  },
                {  L_,   "bbbbbbb"           , 65536,  24271  },
                {  L_,   "bbbbbbbb"          , 65536,  45096  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (veryVerbose)
                cout << "\tTesting 'hash(const char *value, int size)'."
                     << endl;
            {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE = DATA[ti].d_lineNum;
                    const char *SPEC = DATA[ti].d_spec_p;
                    const int   SIZE = DATA[ti].d_size;
                    const int   HASH = DATA[ti].d_hash;

                    const int hash = bdeu_HashUtil::hash0(SPEC, SIZE);

                    if (veryVeryVerbose)
                        cout << SPEC << ", " << SIZE << " ---> " << hash
                             << endl;

                    LOOP_ASSERT(LINE, HASH == hash);
                }
            }

            if (veryVerbose)
                cout << "\tTesting "
                        "'hash(const char *value, int length, int size)'."
                     << endl;
            {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE = DATA[ti].d_lineNum;
                    const char *SPEC = DATA[ti].d_spec_p;
                    const int   SIZE = DATA[ti].d_size;
                    const int   HASH = DATA[ti].d_hash;

                    const int hash = bdeu_HashUtil::hash0(SPEC,
                                                          bsl::strlen(SPEC),
                                                          SIZE);

                    if (veryVeryVerbose)
                        cout << SPEC << ", " << SIZE << " ---> " << hash
                             << endl;

                    LOOP_ASSERT(LINE, HASH == hash);
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
        //    bdeu_HashUtil::hash1(char);
        //    bdeu_HashUtil::hash1(signed char);
        //    bdeu_HashUtil::hash1(unsigned char);
        //    bdeu_HashUtil::hash1(short);
        //    bdeu_HashUtil::hash1(unsigned short);
        //    bdeu_HashUtil::hash1(int);
        //    bdeu_HashUtil::hash1(unsigned int);
        //    bdeu_HashUtil::hash1(long);
        //    bdeu_HashUtil::hash1(unsigned long);
        //    bdeu_HashUtil::hash1(bsls_PlatformUtil::Int64);
        //    bdeu_HashUtil::hash1(bsls_PlatformUtil::Uint64);
        //    bdeu_HashUtil::hash1(float);
        //    bdeu_HashUtil::hash1(double);
        //    bdeu_HashUtil::hash1(void*);
        //    bdeu_HashUtil::hash2(char);
        //    bdeu_HashUtil::hash2(signed char);
        //    bdeu_HashUtil::hash2(unsigned char);
        //    bdeu_HashUtil::hash2(short);
        //    bdeu_HashUtil::hash2(unsigned short);
        //    bdeu_HashUtil::hash2(int);
        //    bdeu_HashUtil::hash2(unsigned int);
        //    bdeu_HashUtil::hash2(long);
        //    bdeu_HashUtil::hash2(unsigned long);
        //    bdeu_HashUtil::hash2(bsls_PlatformUtil::Int64);
        //    bdeu_HashUtil::hash2(bsls_PlatformUtil::Uint64);
        //    bdeu_HashUtil::hash2(float);
        //    bdeu_HashUtil::hash2(double);
        //    bdeu_HashUtil::hash2(void*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASHING FUNDAMENTAL TYPES"
                          << "\n=========================" << endl;

        ASSERT(703514648U == bdeu_HashUtil::hash1((char)'a'));
        ASSERT(703514648U == bdeu_HashUtil::hash1((signed char)'a'));
        ASSERT(703514648U == bdeu_HashUtil::hash1((unsigned char)'a'));
        ASSERT(614942571U == bdeu_HashUtil::hash1((short)12355));
        ASSERT(614942571U == bdeu_HashUtil::hash1((unsigned short)12355));
        ASSERT(1553323673U == bdeu_HashUtil::hash1((int)0x12345678));
        ASSERT(1553323673U == bdeu_HashUtil::hash1((unsigned int)0x12345678));
        if(sizeof(long) == 8) {
            ASSERT(338172012U == bdeu_HashUtil::hash1((long)0x12345678));
            ASSERT(338172012U ==
                              bdeu_HashUtil::hash1((unsigned long)0x12345678));
        }
        else if (sizeof(long) == 4) {
            ASSERT(1553323673U == bdeu_HashUtil::hash1((long)0x12345678));
            ASSERT(1553323673U ==
                              bdeu_HashUtil::hash1((unsigned long)0x12345678));
        }
        else {
            ASSERT(0);
        }
        ASSERT(338172012U ==
                   bdeu_HashUtil::hash1((bsls_PlatformUtil::Int64)0x12345678));
        ASSERT(338172012U ==
                  bdeu_HashUtil::hash1((bsls_PlatformUtil::Uint64)0x12345678));
        ASSERT(3552274171U == bdeu_HashUtil::hash1((float)3.1415926536));
        ASSERT(1503624784U ==
                         bdeu_HashUtil::hash1((double)3.14159265358979323844));
        ASSERT(3552274171U == bdeu_HashUtil::hash1((float)3.1415926536));
        ASSERT(1503624784U ==
                         bdeu_HashUtil::hash1((double)3.14159265358979323844));

#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(2283516099U ==
                            bdeu_HashUtil::hash1((void*)0xffab13f1324e5473LL));
#else
        ASSERT(1741718275U == bdeu_HashUtil::hash1((void*)0xffab13f1));
#endif

        ASSERT(3392050242U == bdeu_HashUtil::hash2((char)'a'));
        ASSERT(3392050242U == bdeu_HashUtil::hash2((signed char)'a'));
        ASSERT(3392050242U == bdeu_HashUtil::hash2((unsigned char)'a'));
        ASSERT(3111500981U == bdeu_HashUtil::hash2((short)12355));
        ASSERT(3111500981U == bdeu_HashUtil::hash2((unsigned short)12355));
        ASSERT(2509914878U == bdeu_HashUtil::hash2((int)0x12345678));
        ASSERT(2509914878U == bdeu_HashUtil::hash2((unsigned int)0x12345678));
        ASSERT(2509914878U == bdeu_HashUtil::hash2((long)0x12345678));
        ASSERT(2509914878U == bdeu_HashUtil::hash2((unsigned long)0x12345678));
        ASSERT(2509914878U ==
                   bdeu_HashUtil::hash2((bsls_PlatformUtil::Int64)0x12345678));
        ASSERT(2509914878U ==
                  bdeu_HashUtil::hash2((bsls_PlatformUtil::Uint64)0x12345678));
        ASSERT(2343743579U == bdeu_HashUtil::hash2((float)3.1415926536));
        ASSERT(3721749206U ==
                         bdeu_HashUtil::hash2((double)3.14159265358979323844));
#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(2631003531U ==
                            bdeu_HashUtil::hash2((void*)0xffab13f1324e5473LL));
#else
        ASSERT(1747622670U == bdeu_HashUtil::hash2((void*)0xffab13f1));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   The hash should output a reasonable value.
        //
        // Plan:
        //   Hash a selection of different types and print the output.
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        struct {
            const int   d_line;
            const char *d_string;
        } STRING_DATA[] = {
            { L_, "" },
            { L_, "this" },
            { L_, "is" },
            { L_, "a" },
            { L_, "random" },
            { L_, "collection" },
            { L_, "of" },
            { L_, "strings" },
        };
        const int NUM_STRING_DATA = sizeof STRING_DATA / sizeof *STRING_DATA;

        for (int i = 0; i < NUM_STRING_DATA; ++i) {
            const int   LINE   = STRING_DATA[i].d_line;
            const char *STRING = STRING_DATA[i].d_string;
            const int   LENGTH = bsl::strlen(STRING);

            int sum = 0;
            if (verbose) {
                P(LINE);
                cout << "H1: " << bdeu_HashUtil::hash1(STRING, LENGTH) << endl;
                cout << "H2: " << bdeu_HashUtil::hash2(STRING, LENGTH) << endl;
            } else {
                // Prevent code to be optimized away in optimized mode.
                // It's highly unlikely that 'sum' would be zero, and anyway
                // this is a deterministic test so data can be adjusted if that
                // should ever be the case.
                sum += bdeu_HashUtil::hash1(STRING, LENGTH);
                LOOP_ASSERT(i, 0 != sum);
            }
        }

        struct {
            const int d_line;
            const int d_int;
        } INT_DATA[] = {
            { L_, (int)0x00000000  },
            { L_, (int)0x00000001  },
            { L_, (int)0x00000002  },
            { L_, (int)0x0000000f  },
            { L_, (int)0x000000f0  },
            { L_, (int)0x00000f00  },
            { L_, (int)0x0000f000  },
            { L_, (int)0x000f0000  },
            { L_, (int)0x00f00000  },
            { L_, (int)0x0f000000  },
            { L_, (int)0xf0000000  },
            { L_, (int)0xffffffff  },
            { L_,      INT_MAX     },
        };
        const int NUM_INT_DATA = sizeof INT_DATA / sizeof *INT_DATA;

        for (int i = 0; i < NUM_INT_DATA; ++i) {
            const int   LINE   = INT_DATA[i].d_line;
            const char *INT    = reinterpret_cast<const char*>
                                                          (&INT_DATA[i].d_int);
            const int   LENGTH = sizeof(int);

            int sum = 0;
            if (verbose) {
                P(LINE);
                cout << "H1: " << bdeu_HashUtil::hash1(INT, LENGTH) << endl;
                cout << "H2: " << bdeu_HashUtil::hash2(INT, LENGTH) << endl;
            } else {
                // Prevent code to be optimized away in optimized mode.
                sum += bdeu_HashUtil::hash1(INT, LENGTH);
                LOOP_ASSERT(i, 0 != sum);
            }
        }

        if (verbose) cout << "\nEnd of test." << endl;
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
        //    bdeu_HashUtil::hash1(char);
        //    bdeu_HashUtil::hash1(signed char);
        //    bdeu_HashUtil::hash1(unsigned char);
        //    bdeu_HashUtil::hash1(short);
        //    bdeu_HashUtil::hash1(unsigned short);
        //    bdeu_HashUtil::hash1(int);
        //    bdeu_HashUtil::hash1(unsigned int);
        //    bdeu_HashUtil::hash1(long);
        //    bdeu_HashUtil::hash1(unsigned long);
        //    bdeu_HashUtil::hash1(bsls_PlatformUtil::Int64);
        //    bdeu_HashUtil::hash1(bsls_PlatformUtil::Uint64);
        //    bdeu_HashUtil::hash1(float);
        //    bdeu_HashUtil::hash1(double);
        //    bdeu_HashUtil::hash1(void*);
        //    bdeu_HashUtil::hash2(char);
        //    bdeu_HashUtil::hash2(signed char);
        //    bdeu_HashUtil::hash2(unsigned char);
        //    bdeu_HashUtil::hash2(short);
        //    bdeu_HashUtil::hash2(unsigned short);
        //    bdeu_HashUtil::hash2(int);
        //    bdeu_HashUtil::hash2(unsigned int);
        //    bdeu_HashUtil::hash2(long);
        //    bdeu_HashUtil::hash2(unsigned long);
        //    bdeu_HashUtil::hash2(bsls_PlatformUtil::Int64);
        //    bdeu_HashUtil::hash2(bsls_PlatformUtil::Uint64);
        //    bdeu_HashUtil::hash2(float);
        //    bdeu_HashUtil::hash2(double);
        //    bdeu_HashUtil::hash2(void*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASHING FUNDAMENTAL TYPES"
                          << "\n=========================" << endl;

        const char TEXT[] = "Now is the time for all good men to come to "
                            "the aid of their country";
        const int LENGTH = sizeof TEXT;

        for (int length = 2; length < LENGTH; length *= 2) {
            time_hash1(TEXT, length);
        }
        time_hash1((char)'a', "char");
        time_hash1((signed char)'a', "signed char");
        time_hash1((unsigned char)'a', "unsigned char");
        time_hash1((short)12355, "short");
        time_hash1((unsigned short)12355, "unsigned short");
        time_hash1( (int)0x12345678, "int");
        time_hash1( (unsigned int)0x12345678, "unsigned int");
#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash1((long)0x12345678, "long");
        time_hash1((unsigned long)0x12345678, "unsigned long");
#else
        time_hash1((long)0x12345678, "long");
        time_hash1((unsigned long)0x12345678, "unsigned long");
#endif
        time_hash1((bsls_PlatformUtil::Int64)0x12345678,
                    "bsls_PlatformUtil::Int64");
        time_hash1((bsls_PlatformUtil::Uint64)0x12345678,
                    "bsls_PlatformUtil::Uint64");
        time_hash1((float)3.1415926536, "float");
        time_hash1((double)3.14159265358979323844, "double");
        time_hash1((float)3.1415926536, "float");
        time_hash1((double)3.14159265358979323844, "double");

#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash1((void*)0xffab13f1324e5473LL, "void*");
#else
        time_hash1((void*)0xffab13f1, "void*");
#endif

        for (int length = 2; length < LENGTH; length *= 2) {
            time_hash2(TEXT, length);
        }
        time_hash2((char)'a', "char");
        time_hash2((signed char)'a', "signed char");
        time_hash2((unsigned char)'a', "unsigned char");
        time_hash2((short)12355, "short");
        time_hash2((unsigned short)12355, "unsigned short");
        time_hash2((int)0x12345678, "int");
        time_hash2((unsigned int)0x12345678, "unsigned int");
        time_hash2((long)0x12345678, "long");
        time_hash2((unsigned long)0x12345678, "unsigned long");
        time_hash2((bsls_PlatformUtil::Int64)0x12345678,
                    "bsls_PlatformUtil::Int64");
        time_hash2((bsls_PlatformUtil::Uint64)0x12345678,
                    "bsls_PlatformUtil::Uint64");
        time_hash2((float)3.1415926536, "float");
        time_hash2((double)3.14159265358979323844, "double");
#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash2((void*)0xffab13f1324e5473LL, "void*");
#else
        time_hash2((void*)0xffab13f1, "void*");
#endif
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
