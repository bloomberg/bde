// bdlb_hashutil.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_hashutil.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cmath.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides two hash functions.  We test them on a
// two kinds of buffers (fixed-length - integer, and variable-length) during
// the breathing test.  Together with the usage example that performs various
// experiments to be reported on in the component-level documentation, this is
// appropriate testing.  There are no other concerns about this component.
// ----------------------------------------------------------------------------
// [ 3] hash0(char                 key, int mod);
// [ 3] hash0(signed char          key, int mod);
// [ 3] hash0(unsigned char        key, int mod);
// [ 3] hash0(short                key, int mod);
// [ 3] hash0(unsigned short       key, int mod);
// [ 3] hash0(int                  key, int mod);
// [ 3] hash0(unsigned int         key, int mod);
// [ 3] hash0(long                 key, int mod);
// [ 3] hash0(unsigned long        key, int mod);
// [ 3] hash0(bsls::Types::Int64   key, int mod);
// [ 3] hash0(bsls::Types::Uint64  key, int mod);
// [ 3] hash0(float                key, int mod);
// [ 3] hash0(double               key, int mod);
// [ 3] hash0(const void          *key, int mod);
// [ 3] hash0(const char *string, int mod);
// [ 3] hash0(const char *string, int len, int mod);
// [ 2] hash1(const char *data, int len);
// [ 2] hash2(const char *data, int len);
// [ 2] hash1(char                 key);
// [ 2] hash1(signed char          key);
// [ 2] hash1(unsigned char        key);
// [ 2] hash1(short                key);
// [ 2] hash1(unsigned short       key);
// [ 2] hash1(int                  key);
// [ 2] hash1(unsigned int         key);
// [ 2] hash1(long                 key);
// [ 2] hash1(unsigned long        key);
// [ 2] hash1(bsls::Types::Int64   key);
// [ 2] hash1(bsls::Types::Uint64  key);
// [ 2] hash1(float                key);
// [ 2] hash1(double               key);
// [ 2] hash1(const void          *key);
// [ 2] hash2(char                 key);
// [ 2] hash2(signed char          key);
// [ 2] hash2(unsigned char        key);
// [ 2] hash2(short                key);
// [ 2] hash2(unsigned short       key);
// [ 2] hash2(int                  key);
// [ 2] hash2(unsigned int         key);
// [ 2] hash2(long                 key);
// [ 2] hash2(unsigned long        key);
// [ 2] hash2(bsls::Types::Int64   key);
// [ 2] hash2(bsls::Types::Uint64  key);
// [ 2] hash2(float                key);
// [ 2] hash2(double               key);
// [ 2] hash2(const void          *key);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

BSLMF_ASSERT(sizeof(int)                == sizeof(long)
          || sizeof(bsls::Types::Int64) == sizeof(long));

BSLMF_ASSERT(INT_MAX   == LONG_MAX
          || LLONG_MAX == LONG_MAX);

typedef bdlb::HashUtil Util;

const int k_LENGTH = 1257;  // not a power of two

void time_hash1(const char *key,
                int         length)
{
    enum { k_ITERATIONS = 1000000 }; // 1M
    unsigned int    value = 0;
    bsls::Stopwatch timer;
    timer.start();
    for (int i = 0; i < k_ITERATIONS; ++i) {
        value += Util::hash1(key, length) % k_LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type (key == const char*, length == " << length << ")"
         << endl;
    (void)value;
}

template <class TYPE>
void time_hash1(const TYPE&  key,
                const char  *TYPEID)
{
    enum { k_ITERATIONS = 1000000 }; // 1M
    unsigned int    value = 0;
    bsls::Stopwatch timer;
    timer.start();
    for (int i = 0; i < k_ITERATIONS; ++i) {
        value += Util::hash1(key) % k_LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type " << TYPEID << endl;
    (void)value;
}

void time_hash2(const char *key,
                int         length)
{
    enum { k_ITERATIONS = 1000000 }; // 1M
    unsigned int    value = 0;
    bsls::Stopwatch timer;
    timer.start();
    for (int i = 0; i < k_ITERATIONS; ++i) {
        value += Util::hash2(key, length) % k_LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type (key == const char*, length == " << length << ")"
         << endl;
    (void)value;
}

template <class TYPE>
void time_hash2(const TYPE&  key,
                const char  *TYPEID)
{
    enum { k_ITERATIONS = 1000000 }; // 1M
    unsigned int    value = 0;
    bsls::Stopwatch timer;
    timer.start();
    for (int i = 0; i < k_ITERATIONS; ++i) {
        value += Util::hash2(key) % k_LENGTH;
    }
    timer.stop();
    cout << "Hashing 1M values (in seconds): " << timer.elapsedTime()
         << "\tof type " << TYPEID << endl;
    (void)value;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
namespace BDEU_HASHUTIL_USAGE_EXAMPLE {

///Usage
///-----
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
            // Create an experimental result reporting the optionally specified
            // 'max', 'avg', and 'sigma' values.
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
//  const char *data   = generator.data();   // address of storage of current
//                                           // value
//  int         length = generator.length(); // length of storage of current
//                                           // value
//
//  generator.next();                        // advance to next value
//..
//
///Example 1: Using Chaining
///- - - - - - - - - - - - -
// The following code will check the number of collisions for a chaining-based
// hash table given a certain distribution of keys:
//..
    template <class GENERATOR>
    ExperimentalResult computeChainingCollisions(int       numElements,
                                                 int       size,
                                                 GENERATOR input)
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
            unsigned int hash = Util::hash1(input.data(),
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
        avgLength   /= size;
        sigmaLength  = bsl::sqrt(sigmaLength / size - avgLength * avgLength);

        return ExperimentalResult(maxLength, avgLength, sigmaLength);
    }
//..
//
///Example 2: Using Double-Hashing
///- - - - - - - - - - - - - - - -
// The following code will check the number of collisions for a double-hashing
// based hash table (such as 'bdlc_hashtable2') given a certain distribution
// of keys:
//..
    template <class GENERATOR>
    ExperimentalResult computeDoubleHashingCollisions(int       numElements,
                                                      int       size,
                                                      GENERATOR input)
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
            unsigned int hash1 = Util::hash1(input.data(),
                                                      input.length());

            int chainLength = 0;
            int bucket      = hash1 % size;
            if (ResultType() == table[bucket]) {
                table[bucket] = input.current();
            } else {
                unsigned int hash2 = Util::hash2(input.data(),
                                                          input.length());
                hash2 = 1 + hash2 % (size - 1);

                while (++chainLength < size) {
                    bucket = (bucket + hash2) % size;

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
                maxLength   = static_cast<int>(
                                      bsl::numeric_limits<double>::infinity());
                return ExperimentalResult(maxLength, avgLength, sigmaLength);
                                                                      // RETURN
            }

            avgLength   += chainLength;
            sigmaLength += chainLength * chainLength;
            maxLength    = bsl::max(maxLength, chainLength);
        }
        avgLength   /= numElements;
        sigmaLength  = bsl::sqrt(sigmaLength / numElements
                               -   avgLength * avgLength);

        return ExperimentalResult(maxLength, avgLength, sigmaLength);
    }
//..
//
///Example 3: Creating a Generator of Sequential Integers
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
        explicit SequentialIntegers(int first = 1, int increment = 1)
            // Create a generator returning integers in a sequence starting at
            // the optionally specified 'first' integer, with the optionally
            // specified 'increment'.
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
//
///Example 4: Creating a Character String Generator
/// - - - - - - - - - - - - - - - - - - - - - - - -
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
        explicit SequentialStrings(bsl::string const& initial)
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
//
///Example 5: Creating a Multiple-Field Keys Generator
///- - - - - - - - - - - - - - - - - - - - - - - - - -
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
        explicit SequentialVector(bsl::vector<char> const& ranges)
            // Create a generator returning strings having the same length as
            // the specified 'ranges' (i.e., 'ranges.size()') in a sequence
            // starting at the string with all null characters and looping
            // through all the strings with each character at position 'i' in
            // the specified range from 0 until 'ranges[i]' (excluded).  The
            // behavior is undefined unless 'ranges' does not contain zero
            // entries.
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
//
///Example 6: Experimental Results
///- - - - - - - - - - - - - - - -
// Checking the above distributions is done in a straightforward manner.  We do
// this at various load factors (the load factor is the percentage of the table
// that actually holds data; it is defined as 'N / SIZE' where 'N' is the
// number of elements and 'SIZE' is the size of the hash table).  Note that
// chaining accommodates arbitrary load factors, while double hashing requires
// that the load factor be strictly less than 1.
//..
    int usageExample(int verbose, int veryVerbose, int /* veryVeryVerbose */) {
        const int SIZE = 10000;
        const int INC  = SIZE / 5; // load factors for every 20% percentile
        const int COLS = (4*SIZE)/INC;

        {
            ExperimentalResult results[3][COLS];
            bsls::Stopwatch    timer;

            if (verbose) cout << "\nUsing chaining"
                              << "\n--------------" << endl;

            if (verbose) cout << "Sequential Integers\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                results[0][i] = computeChainingCollisions(
                                                         n,
                                                         SIZE,
                                                         SequentialIntegers());
                ASSERT(results[0][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Strings\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                bsl::string initial("abcdefghij"); // period = 10! = 3628800
                results[1][i] = computeChainingCollisions(
                                                   n,
                                                   SIZE,
                                                   SequentialStrings(initial));
                ASSERT(results[1][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Vector\n";
            timer.start();
            for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                bsl::vector<char> ranges(6, static_cast<char>(4));
                                                         // period = 4^6 = 4096
                results[2][i] = computeChainingCollisions(
                                                     n,
                                                     SIZE,
                                                     SequentialVector(ranges));
                ASSERT(results[2][i].d_average < 1.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) {
                cout << "\nDisplaying average (max) for chaining:";
                cout << "\n--------------------------------------n";
                const char *ROW_LABELS[] = { "\nIntegers:",
                                             "\nStrings :",
                                             "\nVector  :",
                                             "\nLoad factor:",
                };
                const int   NUM_ROWS     = sizeof  ROW_LABELS
                                         / sizeof *ROW_LABELS - 1;

                cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
                for (int n = INC; n < 4*SIZE; n += INC) {
                    cout << "\t" << static_cast<double>(n) / SIZE;
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
            bsls::Stopwatch    timer;

            if (verbose) cout << "\nUsing double hashing"
                              << "\n--------------------" << endl;

            if (verbose) cout << "Sequential Integers\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                results[0][i] = computeDoubleHashingCollisions(
                                                         n,
                                                         SIZE,
                                                         SequentialIntegers());
                ASSERT(results[0][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Strings\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                bsl::string initial("abcdefghij"); // period = 10! = 3628800
                results[1][i] = computeDoubleHashingCollisions(
                                                   n,
                                                   SIZE,
                                                   SequentialStrings(initial));
                ASSERT(results[1][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) cout << "Sequential Vector\n";
            timer.start();
            for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
                if (veryVerbose)
                    cout << "\tWith load factor " <<
                                         static_cast<double>(n) / SIZE << "\n";

                bsl::vector<char> ranges(7, static_cast<char>(8));
                                                      // period = 8^7 = 2097152
                results[2][i] = computeDoubleHashingCollisions(
                                                     n,
                                                     SIZE,
                                                     SequentialVector(ranges));
                ASSERT(results[2][i].d_average < 2.5 * n / SIZE);
            }
            if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
            timer.reset();

            if (verbose) {
                cout << "\nDisplaying average (max) for double-hashing:";
                cout << "\n--------------------------------------------\n";
                const char *ROW_LABELS[] = { "\nIntegers:",
                                             "\nStrings :",
                                             "\nVector  :",
                                             "\nLoad factor:",
                };
                const int   NUM_ROWS     = sizeof  ROW_LABELS
                                         / sizeof *ROW_LABELS -1;

                cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
                for (int n = INC/2; n < SIZE; n += INC) {
                    cout << "\t" << static_cast<double>(n) / SIZE;
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
//  Displaying average(max) for chaining:
//  ---------------------------------------------------------------------------
//  Load factor:    0.2     0.4     0.6     0.8     1       1.2     1.4
//  Integers:       0.2(3)  0.4(5)  0.6(6)  0.8(6)  1(7)    1.2(7)  1.4(8)
//  Strings :       0.2(4)  0.4(4)  0.6(5)  0.8(7)  1(7)    1.2(7)  1.4(7)
//  Vector  :       0.2(5)  0.4(5)  0.6(10) 0.8(10) 1(15)   1.2(15) 1.4(20)
//
//  Load factor:    1.6     1.8     2       2.2     2.4     2.6     2.8
//  Integers:       1.6(9)  1.8(9)  2(10)   2.2(10) 2.4(10) 2.6(10) 2.8(11)
//  Strings :       1.6(7)  1.8(8)  2(9)    2.2(11) 2.4(11) 2.6(11) 2.8(11)
//  Vector  :       1.6(20) 1.8(24) 2(25)   2.2(29) 2.4(30) 2.6(34) 2.8(35)
//
//  Load factor:    3       3.2     3.4     3.6     3.8
//  Integers:       3(11)   3.2(11) 3.4(12) 3.6(12) 3.8(13)
//  Strings :       3(12)   3.2(12) 3.4(13) 3.6(13) 3.8(13)
//  Vector  :       3(39)   3.2(40) 3.4(42) 3.6(45) 3.8(46)
//
//  Displaying average(max) for double-hashing:
//  ---------------------------------------------------------------------------
//  Load factor:    0.1      0.3     0.5      0.7      0.9
//  Integers:       0.046(2) 0.20(4) 0.37(10) 0.71(15) 1.6(59)
//  Strings :       0.064(2) 0.20(6) 0.40(12) 0.75(18) 1.6(50)
//  Vector  :       0.05(2)  0.19(5) 0.58(9)  1.20(15) 2.4(64)
//..

}  // close namespace BDEU_HASHUTIL_USAGE_EXAMPLE
// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose) cout << "\n" "USAGE EXAMPLE" "\n"
                                  "=============" "\n";

        using namespace BDEU_HASHUTIL_USAGE_EXAMPLE;
        usageExample(verbose, veryVerbose, veryVeryVerbose);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // HASH0 FUNCTIONS
        //
        // Concerns:
        //: 1 The hash return value is constant across all platforms for a
        //:   given input.
        //
        // Plan:
        //: 1 Using the table-driven technique, specifying a set of test
        //:   vectors with the input and expected value, then verify the return
        //:   value from the hash function.
        //
        // Testing:
        //   hash0(char                 key, int mod);
        //   hash0(signed char          key, int mod);
        //   hash0(unsigned char        key, int mod);
        //   hash0(short                key, int mod);
        //   hash0(unsigned short       key, int mod);
        //   hash0(int                  key, int mod);
        //   hash0(unsigned int         key, int mod);
        //   hash0(long                 key, int mod);
        //   hash0(unsigned long        key, int mod);
        //   hash0(bsls::Types::Int64   key, int mod);
        //   hash0(bsls::Types::Uint64  key, int mod);
        //   hash0(float                key, int mod);
        //   hash0(double               key, int mod);
        //   hash0(const void          *key, int mod);
        //   hash0(const char *string, int mod);
        //   hash0(const char *string, int len, int mod);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "HASH0 FUNCTIONS" "\n"
                                  "===============" "\n";

        if (verbose) cout << "\nTesting 'char' hash" << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                char         d_value;       // value to hash
                int          d_size;        // size of target hash table
                unsigned int d_exp;         // expected return value
            } DATA[] = {
                //LINE  VALUE          SIZE    EXP
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
                const int          LINE  = DATA[ti].d_lineNum;
                const char         VALUE = DATA[ti].d_value;
                const int          SIZE  = DATA[ti].d_size;
                const unsigned int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P_(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                               static_cast<signed char>(VALUE),
                                               SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                             static_cast<unsigned char>(VALUE),
                                             SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'short' hash" << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                short        d_value;       // value to hash
                int          d_size;        // size of target hash table
                unsigned int d_exp;         // expected return value
            } DATA[] = {
                //LINE  VALUE          SIZE    EXP
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
                const int          LINE  = DATA[ti].d_lineNum;
                const short        VALUE = DATA[ti].d_value;
                const int          SIZE  = DATA[ti].d_size;
                const unsigned int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                            static_cast<unsigned short>(VALUE),
                                            SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'int' hash" << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                int          d_value;       // value to hash
                int          d_size;        // size of target hash table
                unsigned int d_exp;         // expected return value
            } DATA[] = {
                //LINE  VALUE          SIZE    EXP
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
                const int          LINE  = DATA[ti].d_lineNum;
                const int          VALUE = DATA[ti].d_value;
                const int          SIZE  = DATA[ti].d_size;
                const unsigned int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P_(EXP);
                    P(Util::hash0(VALUE, SIZE));
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                              static_cast<unsigned int>(VALUE),
                                              SIZE));
#if INT_MAX == LONG_MAX
                LOOP_ASSERT(LINE, EXP == Util::hash0(static_cast<long>(VALUE),
                                                     SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                             static_cast<unsigned long>(VALUE),
                                             SIZE));
#endif
            }
        }

        if (verbose) cout << "\nTesting 'int64' hash" << endl;
        {
            static const struct {
                int          d_lineNum;  // source line number
                int          d_valueA;   // first  half of value to hash
                int          d_valueB;   // second half of value to hash
                int          d_size;     // size of hash table
                unsigned int d_exp;      // expected return value
            } DATA[] = {
                //LINE  VALUE_A            VALUE_B            SIZE    EXP
                //----  -----------------  -----------------  ------  ------
                { L_,                  0,                 0,    257,      0 },
                { L_,                 -1,                -1,    257,      0 },
                { L_,                  0,        2147483647,    257,    128 },
                { L_,                 -1,
                               static_cast<int>(0x80000000),    257,    129 },
                { L_,                  0,                 0,  65537,      0 },
                { L_,                 -1,                -1,  65537,      0 },
                { L_,                  0,        2147483647,  65537,  32768 },
                { L_,                 -1,
                               static_cast<int>(0x80000000),  65537,  32769 },
                { L_,         2147483647,                 0,  65537,  32768 },
                { L_,         0x12345678,                 0,  65537,  17476 },
                { L_,         0x12345678,
                               static_cast<int>(0x90ABCDEF),  65537,  63745 },
                { L_,         0x23453453,        0x33253452,  65537,  62008 },
                { L_,   static_cast<int>(0x99999999),
                               static_cast<int>(0x99999999),  65537,      0 },
                { L_,   static_cast<int>(0xFFFFFFFF),
                               static_cast<int>(0xFFFFFFFF),  65537,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const int                VALUE_A = DATA[ti].d_valueA;
                const int                VALUE_B = DATA[ti].d_valueB;
                const int                SIZE    = DATA[ti].d_size;
                const unsigned int       EXP     = DATA[ti].d_exp;
                const bsls::Types::Int64 VALUE =
                                     (bsls::Types::Int64(VALUE_A) << 32) |
                                     (VALUE_B & 0xFFFFFFFF);

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                       static_cast<bsls::Types::Uint64>(VALUE),
                                       SIZE));
#if LLONG_MAX == LONG_MAX
                LOOP_ASSERT(LINE, EXP == Util::hash0(static_cast<long>(VALUE),
                                                     SIZE));
                LOOP_ASSERT(LINE, EXP == Util::hash0(
                                             static_cast<unsigned long>(VALUE),
                                             SIZE));
#endif
            }
        }

        if (verbose) cout << "\nTesting 'float' hash" << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                float        d_value;       // value to hash
                int          d_size;        // size of target hash table
                unsigned int d_exp;         // expected return value
            } DATA[] = {
                //LINE VALUE  SIZE   EXP
                //---- -----  -----  -----
                { L_,   0.00,   257,     0 },
                { L_,   1.00,   257,   177 },
                { L_,   2.00,   257,   193 },
                { L_,   4.00,   257,   209 },
                { L_,   0.50,   257,   161 },
                { L_,   0.25,   257,   145 },
                { L_,  -1.00,   257,    49 },
                { L_,  -2.00,   257,    65 },
                { L_,  -4.00,   257,    81 },
                { L_,  -0.50,   257,    33 },
                { L_,  -0.25,   257,    17 },
                { L_,   1.1F,   257,    18 },
                { L_,  -1.1F,   257,   147 },
                { L_,   0.00, 65537,     0 },
                { L_,   1.00, 65537, 49169 },
                { L_,   2.00, 65537, 49153 },
                { L_,   4.00, 65537, 49137 },
                { L_,   0.50, 65537, 49185 },
                { L_,   0.25, 65537, 49201 },
                { L_,  -1.00, 65537, 16401 },
                { L_,  -2.00, 65537, 16385 },
                { L_,  -4.00, 65537, 16369 },
                { L_,  -0.50, 65537, 16417 },
                { L_,  -0.25, 65537, 16433 },
                { L_,   1.1F, 65537, 47529 },
                { L_,  -1.1F, 65537, 14761 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int          LINE  = DATA[ti].d_lineNum;
                const float        VALUE = DATA[ti].d_value;
                const int          SIZE  = DATA[ti].d_size;
                const unsigned int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'double' hash" << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                double       d_value;       // value to hash
                int          d_size;        // size of target hash table
                unsigned int d_exp;         // expected return value
            } DATA[] = {
                //LINE VALUE  SIZE   EXP
                //---- ------ ------ -----
                { L_,   0.00,   257,     0 },
                { L_,   1.00,   257,   177 },
                { L_,   2.00,   257,   193 },
                { L_,   4.00,   257,   209 },
                { L_,   0.50,   257,   161 },
                { L_,   0.25,   257,   145 },
                { L_,  -1.00,   257,    49 },
                { L_,  -2.00,   257,    65 },
                { L_,  -4.00,   257,    81 },
                { L_,  -0.50,   257,    33 },
                { L_,  -0.25,   257,    17 },
                { L_,   1.10,   257,   144 },
                { L_,  -1.10,   257,    15 },
                { L_,   0.00, 65537,     0 },
                { L_,   1.00, 65537, 49169 },
                { L_,   2.00, 65537, 49153 },
                { L_,   4.00, 65537, 49137 },
                { L_,   0.50, 65537, 49185 },
                { L_,   0.25, 65537, 49201 },
                { L_,  -1.00, 65537, 16401 },
                { L_,  -2.00, 65537, 16385 },
                { L_,  -4.00, 65537, 16369 },
                { L_,  -0.50, 65537, 16417 },
                { L_,  -0.25, 65537, 16433 },
                { L_,   1.10, 65537,  9881 },
                { L_,  -1.10, 65537, 42651 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int          LINE  = DATA[ti].d_lineNum;
                const double       VALUE = DATA[ti].d_value;
                const int          SIZE  = DATA[ti].d_size;
                const unsigned int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE, SIZE));
            }
        }

        if (verbose) cout << "\nTesting 'void *' hash" << endl;
        {
            static const struct {
                int           d_lineNum;     // source line number
                void         *d_value;       // value to hash
                int           d_size;        // size of target hash table
                unsigned int  d_exp;         // expected return value
            } DATA[] = {
                //LINE  VALUE          SIZE    EXP
                //----  -------------  ------  ------
                { L_,   (void *)    0,     23,      0 },
                { L_,   (void *)   10,     23,     10 },
                { L_,   (void *)    0,    257,      0 },
                { L_,   (void *)   10,    257,     10 },
                { L_,   (void *)   46,    257,     46 },
                { L_,   (void *)  127,    257,    127 },
                { L_,   (void *)32767,    257,    128 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int           LINE  = DATA[ti].d_lineNum;
                const void         *VALUE = DATA[ti].d_value;
                const int           SIZE  = DATA[ti].d_size;
                const unsigned int  EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE) P_(SIZE) P(EXP)
                    P(Util::hash0(VALUE, SIZE))
                }
                LOOP_ASSERT(LINE, EXP == Util::hash0(VALUE,SIZE));
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
                // LINE  STRING      SIZE    EXP
                // ----  ----------  ------  -----
                {  L_,   ""        ,   256,      0  },
                {  L_,   "a"       ,   256,     76  },
                {  L_,   "aa"      ,   256,    168  },
                {  L_,   "aaa"     ,   256,    148  },
                {  L_,   "aaaa"    ,   256,    208  },
                {  L_,   "aaaaa"   ,   256,     92  },
                {  L_,   "aaaaaa"  ,   256,    120  },
                {  L_,   "aaaaaaa" ,   256,    164  },
                {  L_,   "aaaaaaaa",   256,     96  },
                {  L_,   "b"       ,   256,     89  },
                {  L_,   "bb"      ,   256,     94  },
                {  L_,   "bbb"     ,   256,    107  },
                {  L_,   "bbbb"    ,   256,    212  },
                {  L_,   "bbbbb"   ,   256,    157  },
                {  L_,   "bbbbbb"  ,   256,     82  },
                {  L_,   "bbbbbbb" ,   256,    207  },
                {  L_,   "bbbbbbbb",   256,     40  },
                {  L_,   ""        , 65536,      0  },
                {  L_,   "a"       , 65536,  40524  },
                {  L_,   "aa"      , 65536,  59816  },
                {  L_,   "aaa"     , 65536,  59540  },
                {  L_,   "aaaa"    , 65536,  26064  },
                {  L_,   "aaaaa"   , 65536,  41820  },
                {  L_,   "aaaaaa"  , 65536,  35704  },
                {  L_,   "aaaaaaa" , 65536,  63908  },
                {  L_,   "aaaaaaaa", 65536,   8288  },
                {  L_,   "b"       , 65536,   1113  },
                {  L_,   "bb"      , 65536,  44126  },
                {  L_,   "bbb"     , 65536,  39531  },
                {  L_,   "bbbb"    , 65536,  23508  },
                {  L_,   "bbbbb"   , 65536,   8093  },
                {  L_,   "bbbbbb"  , 65536,  11602  },
                {  L_,   "bbbbbbb" , 65536,  24271  },
                {  L_,   "bbbbbbbb", 65536,  45096  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (veryVerbose)
                cout << "\tTesting 'hash0(const char *value, int size)'."
                     << endl;
            {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE = DATA[ti].d_lineNum;
                    const char *SPEC = DATA[ti].d_spec_p;
                    const int   SIZE = DATA[ti].d_size;
                    const int   HASH = DATA[ti].d_hash;

                    const int hash = Util::hash0(SPEC, SIZE);

                    if (veryVeryVerbose)
                        cout << SPEC << ", " << SIZE << " ---> " << hash
                             << endl;

                    LOOP_ASSERT(LINE, HASH == hash);
                }
            }

            if (veryVerbose)
                cout << "\tTesting "
                        "'hash0(const char *value, int length, int size)'."
                     << endl;
            {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE = DATA[ti].d_lineNum;
                    const char *SPEC = DATA[ti].d_spec_p;
                    const int   SIZE = DATA[ti].d_size;
                    const int   HASH = DATA[ti].d_hash;

                    const int hash = Util::hash0(SPEC,
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
        // HASHING FUNDAMENTAL TYPES
        //
        // Concerns:
        //: 1 The hash should output a reasonable value, which does not depend
        //:   on the endianness of the platform.
        //
        // Plan:
        //: 1 Compare return value to expected values computed on a given
        //:   platform.
        //
        // Testing:
        //   hash1(const char *data, int len);
        //   hash2(const char *data, int len);
        //   hash1(char                 key);
        //   hash1(signed char          key);
        //   hash1(unsigned char        key);
        //   hash1(short                key);
        //   hash1(unsigned short       key);
        //   hash1(int                  key);
        //   hash1(unsigned int         key);
        //   hash1(long                 key);
        //   hash1(unsigned long        key);
        //   hash1(bsls::Types::Int64   key);
        //   hash1(bsls::Types::Uint64  key);
        //   hash1(float                key);
        //   hash1(double               key);
        //   hash1(const void          *key);
        //   hash2(char                 key);
        //   hash2(signed char          key);
        //   hash2(unsigned char        key);
        //   hash2(short                key);
        //   hash2(unsigned short       key);
        //   hash2(int                  key);
        //   hash2(unsigned int         key);
        //   hash2(long                 key);
        //   hash2(unsigned long        key);
        //   hash2(bsls::Types::Int64   key);
        //   hash2(bsls::Types::Uint64  key);
        //   hash2(float                key);
        //   hash2(double               key);
        //   hash2(const void          *key);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "HASHING FUNDAMENTAL TYPES" "\n"
                                  "=========================" "\n";

        ASSERT(703514648U  == Util::hash1(         (char)'a'));
        ASSERT(703514648U  == Util::hash1(  (signed char)'a'));
        ASSERT(703514648U  == Util::hash1((unsigned char)'a'));

        ASSERT(614942571U  == Util::hash1(         (short)12355));
        ASSERT(614942571U  == Util::hash1((unsigned short)12355));

        ASSERT(1553323673U == Util::hash1(         (int)0x12345678));
        ASSERT(1553323673U == Util::hash1((unsigned int)0x12345678));
        switch (sizeof(long)) {
          case 8:
            ASSERT(338172012U == Util::hash1(         (long)0x12345678));
            ASSERT(338172012U == Util::hash1((unsigned long)0x12345678));
            break;
          case 4:
            ASSERT(1553323673U == Util::hash1(         (long)0x12345678));
            ASSERT(1553323673U == Util::hash1((unsigned long)0x12345678));
            break;
          default:
            ASSERT(0);
            break;
        }
        ASSERT(338172012U  == Util::hash1( (bsls::Types::Int64)0x12345678));
        ASSERT(338172012U  == Util::hash1((bsls::Types::Uint64)0x12345678));

        ASSERT(3552274171U == Util::hash1( (float)3.1415926536          ));
        ASSERT(1503624784U == Util::hash1((double)3.14159265358979323844));
        ASSERT(3552274171U == Util::hash1( (float)3.1415926536          ));
        ASSERT(1503624784U == Util::hash1((double)3.14159265358979323844));

#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(2283516099U == Util::hash1((void *)0xffab13f1324e5473LL));
#else
        ASSERT(1741718275U == Util::hash1((void *)0xffab13f1          ));
#endif

        ASSERT(3392050242U == Util::hash2(         (char)'a'));
        ASSERT(3392050242U == Util::hash2(  (signed char)'a'));
        ASSERT(3392050242U == Util::hash2((unsigned char)'a'));

        ASSERT(3111500981U == Util::hash2(         (short)12355));
        ASSERT(3111500981U == Util::hash2((unsigned short)12355));

        ASSERT(2509914878U == Util::hash2(         (int)0x12345678));
        ASSERT(2509914878U == Util::hash2((unsigned int)0x12345678));

        ASSERT(2509914878U == Util::hash2(         (long)0x12345678));
        ASSERT(2509914878U == Util::hash2((unsigned long)0x12345678));

        ASSERT(2509914878U == Util::hash2( (bsls::Types::Int64)0x12345678));
        ASSERT(2509914878U == Util::hash2((bsls::Types::Uint64)0x12345678));

        ASSERT(2343743579U == Util::hash2( (float)3.1415926536          ));
        ASSERT(3721749206U == Util::hash2((double)3.14159265358979323844));

#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(2631003531U == Util::hash2((void *)0xffab13f1324e5473LL));
#else
        ASSERT(1747622670U == Util::hash2((void *)0xffab13f1          ));
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
        //: 1 Hash a selection of different types, print the output, and check
        //:   that result is a reasonable value.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "BREATHING TEST" "\n"
                                  "==============" "\n";

        struct {
            const int   d_line;
            const char *d_string;
        } STRING_DATA[] = {
            //LINE  STRING
            //----  ------------
            { L_,   ""           },
            { L_,   "this"       },
            { L_,   "is"         },
            { L_,   "a"          },
            { L_,   "random"     },
            { L_,   "collection" },
            { L_,   "of"         },
            { L_,   "strings"    },
        };
        const int NUM_STRING_DATA = sizeof STRING_DATA / sizeof *STRING_DATA;

        for (int ti = 0; ti < NUM_STRING_DATA; ++ti) {
            const int   LINE   = STRING_DATA[ti].d_line;
            const char *STRING = STRING_DATA[ti].d_string;
            const int   LENGTH = bsl::strlen(STRING);

            int sum = 0;
            if (veryVerbose) {
                P(LINE);
                cout << "H1: " << Util::hash1(STRING, LENGTH) << "\n";
                cout << "H2: " << Util::hash2(STRING, LENGTH) << "\n";
            } else {
                // Prevent code to be optimized away in optimized mode.  It is
                // highly unlikely that 'sum' would be zero, and anyway this is
                // a deterministic test so data can be adjusted if that should
                // ever be the case.
                sum += Util::hash1(STRING, LENGTH);
                LOOP_ASSERT(ti, 0 != sum);
            }
        }

        struct {
            const int d_line;
            const int d_int;
        } INT_DATA[] = {
            //LINE  INT
            //----  ---------------
            { L_,   (int)0x00000000 },
            { L_,   (int)0x00000001 },
            { L_,   (int)0x00000002 },
            { L_,   (int)0x0000000f },
            { L_,   (int)0x000000f0 },
            { L_,   (int)0x00000f00 },
            { L_,   (int)0x0000f000 },
            { L_,   (int)0x000f0000 },
            { L_,   (int)0x00f00000 },
            { L_,   (int)0x0f000000 },
            { L_,   (int)0xf0000000 },
            { L_,   (int)0xffffffff },
            { L_,        INT_MAX    },
        };
        const int NUM_INT_DATA = sizeof INT_DATA / sizeof *INT_DATA;

        for (int ti = 0; ti < NUM_INT_DATA; ++ti) {
            const int   LINE   = INT_DATA[ti].d_line;
            const char *INT    = reinterpret_cast<const char*>
                                                         (&INT_DATA[ti].d_int);
            const int   LENGTH = sizeof(int);

            int sum = 0;
            if (veryVerbose) {
                P(LINE);
                cout << "H1: " << Util::hash1(INT, LENGTH) << endl;
                cout << "H2: " << Util::hash2(INT, LENGTH) << endl;
            } else {
                // Prevent code to be optimized away in optimized mode.
                sum += Util::hash1(INT, LENGTH);
                LOOP_ASSERT(ti, 0 != sum);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE MEASUREMENTS
        //
        // Concerns:
        //: 1 The hash are fairly thorough but are they fast?  Let us evaluate
        //:   performance here.
        //
        // Plan:
        //: 1 Perform the test of case 2 inside a loop and report the timing
        //:   using 'bsls_stopwatch'.
        //
        // Testing:
        //    bdlb::HashUtil::hash1(char);
        //    bdlb::HashUtil::hash1(signed char);
        //    bdlb::HashUtil::hash1(unsigned char);
        //    bdlb::HashUtil::hash1(short);
        //    bdlb::HashUtil::hash1(unsigned short);
        //    bdlb::HashUtil::hash1(int);
        //    bdlb::HashUtil::hash1(unsigned int);
        //    bdlb::HashUtil::hash1(long);
        //    bdlb::HashUtil::hash1(unsigned long);
        //    bdlb::HashUtil::hash1(bsls::Types::Int64);
        //    bdlb::HashUtil::hash1(bsls::Types::Uint64);
        //    bdlb::HashUtil::hash1(float);
        //    bdlb::HashUtil::hash1(double);
        //    bdlb::HashUtil::hash1(void*);
        //    bdlb::HashUtil::hash2(char);
        //    bdlb::HashUtil::hash2(signed char);
        //    bdlb::HashUtil::hash2(unsigned char);
        //    bdlb::HashUtil::hash2(short);
        //    bdlb::HashUtil::hash2(unsigned short);
        //    bdlb::HashUtil::hash2(int);
        //    bdlb::HashUtil::hash2(unsigned int);
        //    bdlb::HashUtil::hash2(long);
        //    bdlb::HashUtil::hash2(unsigned long);
        //    bdlb::HashUtil::hash2(bsls::Types::Int64);
        //    bdlb::HashUtil::hash2(bsls::Types::Uint64);
        //    bdlb::HashUtil::hash2(float);
        //    bdlb::HashUtil::hash2(double);
        //    bdlb::HashUtil::hash2(void*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "PERFORMANCE MEASUREMENTS" "\n"
                                  "========================" "\n";

        const char TEXT[] = "Now is the time for all good men to come to "
                            "the aid of their country";
        const int  LENGTH = sizeof TEXT;

        for (int length = 2; length < LENGTH; length *= 2) {
            time_hash1(TEXT, length);
        }
        time_hash1(         (char)'a', "char"         );
        time_hash1(  (signed char)'a', "signed char"  );
        time_hash1((unsigned char)'a', "unsigned char");

        time_hash1(         (short)12355, "short"         );
        time_hash1((unsigned short)12355, "unsigned short");

        time_hash1(           (int)0x12345678, "int"         );
        time_hash1(  (unsigned int)0x12345678, "unsigned int");
#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash1(         (long)0x12345678, "long"         );
        time_hash1((unsigned long)0x12345678, "unsigned long");
#else
        time_hash1(         (long)0x12345678, "long"         );
        time_hash1((unsigned long)0x12345678, "unsigned long");
#endif
        time_hash1( (bsls::Types::Int64)0x12345678, "bsls::Types::Int64" );
        time_hash1((bsls::Types::Uint64)0x12345678, "bsls::Types::Uint64");

        time_hash1( (float)3.1415926536,           "float" );
        time_hash1((double)3.14159265358979323844, "double");
        time_hash1( (float)3.1415926536,           "float" );
        time_hash1((double)3.14159265358979323844, "double");

#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash1((void *)0xffab13f1324e5473LL, "void*");
#else
        time_hash1((void *)0xffab13f1,           "void*");
#endif

        for (int length = 2; length < LENGTH; length *= 2) {
            time_hash2(TEXT, length);
        }
        time_hash2(         (char)'a', "char"         );
        time_hash2(  (signed char)'a', "signed char"  );
        time_hash2((unsigned char)'a', "unsigned char");

        time_hash2(         (short)12355, "short"         );
        time_hash2((unsigned short)12355, "unsigned short");

        time_hash2(         (int)0x12345678, "int"         );
        time_hash2((unsigned int)0x12345678, "unsigned int");

        time_hash2(         (long)0x12345678, "long"         );
        time_hash2((unsigned long)0x12345678, "unsigned long");

        time_hash2( (bsls::Types::Int64)0x12345678, "bsls::Types::Int64" );
        time_hash2((bsls::Types::Uint64)0x12345678, "bsls::Types::Uint64");

        time_hash2( (float)3.1415926536,           "float" );
        time_hash2((double)3.14159265358979323844, "double");
#ifdef BSLS_PLATFORM_CPU_64_BIT
        time_hash2((void *)0xffab13f1324e5473LL, "void *");
#else
        time_hash2((void *)0xffab13f1,           "void *");
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
