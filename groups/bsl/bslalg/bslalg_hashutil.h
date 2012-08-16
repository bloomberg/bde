// bslalg_hashutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHUTIL
#define INCLUDED_BSLALG_HASHUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility of hash functions.
//
//@CLASSES:
//  bslalg::HashUtil: utility for hash functions
//
//@AUTHOR: Vladimir Kliatchko (vkliatch), Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdecs_hashtable2, bdeimp_inthash, bdeimp_strhash, bdede_crc32
//
//@DESCRIPTION:  This component provides two hash functions, 'hash1' and
// 'computeHash' that have a good funneling property, which can be formalized as
// documented below.  In short, these hashes have the property that even if
// two inputs differ in only a few bits, they have a negligible probability of
// producing collisions.  In addition, these two hash functions produce full
// 32-bit results and allow hash table sizes to be a power of two.  They are
// fast and reliable.  They should work equally well on all types of keys.
//
// The two hash functions 'hash1' and 'computeHash' have similar characteristics.
// 'computeHash' is a bit faster than 'hash1', especially for longer keys; it also
// does not produce many collisions as determined experimentally for the key
// types given in the example below.  On the other hand, it does not have the
// formal no-funnel guarantees of 'hash1' which is reasonably fast and provably
// achieves low collision probability.
//
// Furthermore, this component provides a third hash function 'hash0' that
// performs a simple hash by taking the modulus of a user specified size on the
// input key.  This hash function has a poor distribution, as the function
// exhibits a clustering effect when the input keys only differ slightly.
// However, 'hash0' is about 6 to 8 times faster than 'hash1' and 'computeHash'.
//
// In general, use 'hash0' when speed matters, and 'hash1' or 'computeHash' when a
// more thorough hash is needed (e.g., keys exhibit a pattern that results in
// many collision when using 'bdeimp', or cost of collision is high but hashing
// is not a bottleneck).
//
///Hashing Fundamental Types
///-------------------------
// In order to facilitate hashing user-defined types (see next section), this
// component supports hashing fundamental types in a manner that is both
// seemingly random (i.e., the hash is good) but predictable (i.e., identical
// on all platforms, irrespective of endianness).  Note that the following
// expressions for some 'key' of a fundamental integral type:
//..
//  bdeu_HashUtil::hash1((const char *)&key, sizeof(key));  // not recommended
//  bdeu_HashUtil::computeHash((const char *)&key, sizeof(key));  // not recommended
//..
// return values which differ on big- and little-endian platforms if the key
// size is more than one byte.  Instead, the recommended way to hash this 'key'
// is simply to:
//..
//  bdeu_HashUtil::hash1(key);
//  bdeu_HashUtil::computeHash(key);
//..
// This works if 'key' is of one of the following fundamental types:
//..
//  char, signed char, unsigned char,
//  short, unsigned short,
//  int, unsigned int,
//  long, unsigned long,
//  bsls_Types::Int64, bsls_Types::Uint64,
//  float, double,
//  const void *
//..
//
///Hashing User-defined Types
///--------------------------
// This component can be used for hashing a user-defined type as well.  The
// recommended way to do this is to provide a 'hash' class method for the type,
// that takes an instance of the class and a size.  For instance, let us
// consider the following class (the meaning and implementation are irrelevant
// here) with the following private data members:
//..
//  class UserDefinedTickerType {
//      // This class is an aggregate.
//
//      enum { CUSIP_LENGTH = 10 };
//
//      // PRIVATE DATA MEMBERS
//      bool      d_isConstant;          // not recommended, because of padding
//      char      d_cusip[CUSIP_LENGTH];
//      int       d_value;
//..
// A hash class method can be provided and documented as follows:
//..
//    public:
//      // CLASS METHODS
//      static int hash(const UserDefinedTickerType& object, int size);
//          // Return a hash value calculated from the specified 'object' using
//          // the specified 'size' as the number of slots.  The hash value
//          // is guaranteed to be in the range [0, size).
//
//      // Rest of the class definition omitted...
//  };
//..
// The implementation of 'hash' can be very simple, depending on the semantics
// of the class.  Here we assume that 'd_isConstant' is an implementation
// detail and that data members 'd_cusip' and 'd_value' should participate in
// the hash and that the character string should be hashed by value.  The
// implementation follows:
//..
//  int UserDefinedTickerType::hash(const UserDefinedTickerType& object,
//                                  int                          size)
//  {
//      return (bdeu_HashUtil::hash1(object.d_cusip, CUSIP_LENGTH) +
//                                bdeu_HashUtil::hash1(object.d_value)) % size;
//  }
//..
// Note that more intricate combinations of the individual hashes in the
// aggregate can be appropriate if the individual members in the aggregate my
// exhibit a pattern.
//
// CAVEAT: When hashing user-defined classes, because of the potential padding
// in the class footprint, the simple-minded hashes
//..
//  const UserDefinedTickerType& key;
//  bdeu_HashUtil::hash1((const char *)&key, sizeof(key)); // wrong, because
//  bdeu_HashUtil::computeHash((const char *)&key, sizeof(key)); // of padding
//..
// may actually *not* produce the same hash values for copies of the same
// object.
//
///Definitions and Formal Statement of No-Funneling Property
///-------------------------------------------------------
// Let us say a given bit in the input affects a given bit in the output (which
// has 'v' bits) if two keys differing only at that input bit will differ at
// the given output bit about half the time.  We define a hash to be a
// funneling hash if there is some subset of size 't' of all the input bits
// which can affect only a subset of size 'u' of bits in the output, and 't >
// u' and 'v > u', and we say that the hash function has a funnel of those 't'
// input bits into those 'u' bits.  In that case, then 'u' of those 't' bits
// can cancel out the effects of the other 't - u', and so the set of keys
// differing only in the input bits of the funnel can produce no more than half
// that number of hash values.  (Those '2^t' keys can produce no more than
// '2^u' out of '2^v' hash values.)  Differing in only a few bits is a common
// pattern in human and computer keys, so a funneling hash is seriously flawed.
//
// This component offers a hash function ('hash1') that has no funnels, except
// for a funnel of 32 bits into 31 bits, which is a negligible vulnerability.
// The 'computeHash' does not have known no-funneling properties, but operates on the
// same principle and is just as good (and faster) in experiments with
// chaining as in the usage below.  In addition, these two hash functions
// produce full 32-bit results and allow hash table sizes to be a power of two.
//
// For further details and references, the interested reader is invited to
// consult 'http://burtleburtle.net/bob/hash/evahash.html'.
//
///Usage
///-----
// We illustrate the usage of this component by some experiments (code and
// results) on the number of collision expected for various usage.  For
// returning the results of an experiment, we will use the following structure:
//..
//  struct ExperimentalResult {
//      // DATA MEMBERS
//      int    d_max;     // maximum length of a chain
//      double d_average; // average length of a chain
//      double d_sigma;   // standard deviation
//      // CREATORS
//      explicit
//      ExperimentalResult(int max = 0, double avg = 0, double sigma = 0)
//          // Create an experimental result reporting the specified 'max',
//          // 'avg', and 'sigma' values.
//      : d_max(max), d_average(avg), d_sigma(sigma)
//      {}
//  };
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
//  template <class GENERATOR>
//  ExperimentalResult
//  computeChainingCollisions(int numElements, int size, GENERATOR input,
//                            int veryVerbose, int veryVeryVerbose)
//      // Simulate insertion of the specified 'numElements' generated by the
//      // specified 'input' generator into a hash table of the specified
//      // 'size' using chaining as the mechanism to resolve collisions.
//      // Return the results (maximum and average length of a chain, with
//      // standard deviation).  'GENERATOR' must be a model of the
//      // 'GeneratorConcept'.  For the good functioning of this function, it
//      // is required that 'input' never be default nor repeat itself.
//  {
//      bsl::vector<int> table(size);   // all counters are init'ed to 0
//
//      for (int i = 0; i < numElements; ++i, input.next()) {
//          unsigned int hash = bdeu_HashUtil::hash1(input.data(),
//                                                   input.length());
//          ++table[hash % size];
//      }
//
//      double avgLength    = 0; // average number of collisions
//      double sigmaLength  = 0; // standard deviation from average
//      int    maxLength    = 0; // maximum length of a chain
//
//      for (int k = 0; k < size; ++k) {
//          avgLength   += table[k];
//          sigmaLength += table[k] * table[k];
//          maxLength    = bsl::max(maxLength, table[k]);
//      }
//      avgLength /= size;
//      sigmaLength = sqrt( sigmaLength / size - avgLength * avgLength );
//
//      return ExperimentalResult(maxLength, avgLength, sigmaLength);
//  }
//..
///Using double-hashing
/// - - - - - - - - - -
// The following code will check the number of collisions for a double-hashing
// based hash table (such as 'bdecs_hashtable2') given a certain distribution
// of keys:
//..
//  template <class GENERATOR>
//  ExperimentalResult
//  computeDoubleHashingCollisions(int numElements, int size, GENERATOR input,
//                                 int veryVerbose, int veryVeryVerbose)
//      // Simulate insertion of the specified 'numElements' generated by the
//      // specified 'input' generator into a hash table of the specified
//      // 'size' using double hashing as the mechanism to resolve collisions.
//      // Return the results (maximum and average length of a chain, with
//      // standard deviation).  'GENERATOR' must be a model of the
//      // 'GeneratorConcept'.  For the good functioning of this function, it
//      // is required that 'input' never be default nor repeat itself.
//  {
//      typedef typename GENERATOR::ResultType  ResultType;
//      bsl::vector<ResultType> table(size); // all counters are default-ct'ed
//
//      double avgLength    = 0; // average number of collisions
//      double sigmaLength  = 0; // standard deviation from average
//      int    maxLength    = 0; // maximum length of a chain
//
//      for (int i = 0; i < numElements; ++i, input.next()) {
//          unsigned int hash1 = bdeu_HashUtil::hash1(input.data(),
//                                                    input.length());
//
//          int chainLength = 0;
//          int bucket = hash1 % size;
//          if (ResultType() == table[bucket]) {
//              table[bucket] = input.current();
//          } else {
//              unsigned int computeHash = bdeu_HashUtil::computeHash(input.data(),
//                                                        input.length());
//              computeHash = 1 + computeHash % (size - 1);
//
//              while (++chainLength < size) {
//                  bucket = (bucket + computeHash) % size;
//                  if (veryVeryVerbose) { T_(i); P(bucket); }
//
//                  if (ResultType() == table[bucket]) {
//                      table[bucket] = input.current();
//                      break; // while loop
//                  }
//              }
//          }
//
//          if (chainLength == size) {
//              bsl::cerr << "\tCould not insert in doubly-hashed table\n";
//              avgLength   = bsl::numeric_limits<double>::infinity();
//              sigmaLength = bsl::numeric_limits<double>::infinity();
//              maxLength   = bsl::numeric_limits<double>::infinity();
//              return ExperimentalResult(maxLength, avgLength, sigmaLength);
//          }
//
//          avgLength   += chainLength;
//          sigmaLength += chainLength * chainLength;
//          maxLength    = bsl::max(maxLength, chainLength);
//      }
//      avgLength /= numElements;
//      sigmaLength = sqrt(sigmaLength / numElements - avgLength * avgLength);
//
//      return ExperimentalResult(maxLength, avgLength, sigmaLength);
//  }
//..
///Sequential integers generator
///- - - - - - - - - - - - - - -
// Here is a simple generator that returns a sequence of integers.  This
// generator has a period of 'INT_MAX / gcd(increment, INT_MAX+1)'.
//..
//  class SequentialIntegers {
//      int d_current;
//      int d_inc;
//
//    public:
//      // TYPES
//      typedef int ResultType;
//          // Type returned by this generator.
//
//      // CREATORS
//      SequentialIntegers(int first = 1, int increment = 1)
//          // Create a generator returning integers in a sequence starting at
//          // the specified 'first' integer, with the specified 'increment'.
//          : d_current(first), d_inc(increment) {}
//
//      // MANIPULATORS
//      void next()
//          // Advance to the next element.
//      {
//          d_current += d_inc;
//      }
//
//      // ACCESSORS
//      ResultType current() const
//          // Return current element.
//      {
//          return d_current;
//      }
//
//      const char *data() const
//          // Return data buffer of result type.
//      {
//          return reinterpret_cast<const char*>(&d_current);
//      }
//
//      int length() const
//          // Return length of result type (in bytes).
//      {
//          return sizeof(ResultType);
//      }
//  };
//..
///Character strings generator
///- - - - - - - - - - - - - -
// Here is a simple generator that returns a sequence of strings that are a
// permutation of an initial string.  This generator has a period of
// 'factorial(initial.length())' where 'factorial(N)' returns the number of
// permutations of 'N' distinct elements.
//..
//  class SequentialStrings {
//      int         d_length;
//      bsl::string d_current;
//
//    public:
//      // TYPES
//      typedef bsl::string ResultType;
//          // Type returned by this generator.
//
//      // CREATORS
//      SequentialStrings(bsl::string const& initial)
//          // Create a generator returning strings in a sequence starting at
//          // the specified 'initial' string (sorted by characters) and
//          // looping through all permutations of 'str'.  The behavior is
//          // undefined unless all the characters of the 'initial' string are
//          // distinct.
//          : d_length(initial.length()), d_current(initial)
//      {
//          bsl::sort(d_current.begin(), d_current.end());
//          ASSERT(bsl::unique(d_current.begin(), d_current.end()) ==
//                                                            d_current.end());
//      }
//
//      // MANIPULATORS
//      void next()
//          // Advance to the next element.
//      {
//          bsl::next_permutation(d_current.begin(), d_current.end());
//      }
//
//      // ACCESSORS
//      ResultType current() const
//          // Return current element.
//      {
//          return d_current;
//      }
//
//      const char *data() const
//          // Return data buffer of result type.
//      {
//          return d_current.data();
//      }
//
//      int length() const
//          // Return length of result type (in bytes).
//      {
//          return d_current.length();
//      }
//  };
//..
///Multiple-field keys generator
///- - - - - - - - - - - - - - -
// It is not uncommon for keys to consist of concatenated keys in multiple
// combinations.  We simulate this by a character string in which each
// character loops through a specified number of values.  The period of this
// generator is the product of the length of each character range.
//..
//  struct SequentialVector {
//      bsl::vector<char> d_ranges;
//      int               d_length;
//      bsl::string       d_current;
//
//    public:
//      // TYPES
//      typedef bsl::string ResultType;
//          // Type returned by this generator.
//
//      // CREATORS
//      SequentialVector(bsl::vector<char> const& ranges)
//          // Create a generator returning strings of length 'ranges.size()'
//          // in a sequence starting at the string with all null characters
//          // and looping through all the strings with each character at
//          // position 'i' in the specified range from 0 until 'ranges[i]'
//          // (excluded).  The behavior is undefined unless 'ranges' does not
//          // contain zero entries.
//          : d_ranges(ranges)
//          , d_length(ranges.size())
//          , d_current(d_length, (char)0)
//      {
//      }
//
//      // MANIPULATORS
//      void next()
//          // Advance to the next element.
//      {
//          for (int i = 0;
//               i < d_length && ++d_current[i] == d_ranges[i]; ++i) {
//              d_current[i] = 0;
//          }
//      }
//
//      // ACCESSORS
//      ResultType current() const
//          // Return current element.
//      {
//          return d_current;
//      }
//
//      const char *data() const
//          // Return data buffer of current value.
//      {
//          return d_current.data();
//      }
//
//      int length() const
//          // Return length of current value (in bytes).
//      {
//          return d_current.length();
//      }
//  };
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
//  int main() {
//      const int SIZE = 10007;
//      const int INC  = SIZE / 5; // load factors for every 20% percentile
//      const int COLS = (4*SIZE)/INC;
//
//      // Using chaining.
//      {
//          ExperimentalResult results[3][COLS];
//          bsls_Stopwatch timer;
//
//          timer.start();
//          for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
//              results[0][i] = computeChainingCollisions(n, SIZE,
//                                               SequentialIntegers(),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[0][i].d_average < 1.5 * n / SIZE);
//          }
//          timer.reset();
//
//          timer.start();
//          for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
//              bsl::string initial("abcdefghij"); // period = 10! = 3628800
//              results[1][i] = computeChainingCollisions(n, SIZE,
//                                               SequentialStrings(initial),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[1][i].d_average < 1.5 * n / SIZE);
//          }
//          timer.reset();
//
//          timer.start();
//          for (int n = INC, i = 0; n < 4*SIZE; n += INC, ++i) {
//              bsl::vector<char> ranges(6, (char)4); // period = 4^6 = 4096
//              results[2][i] = computeChainingCollisions(n, SIZE,
//                                               SequentialVector(ranges),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[2][i].d_average < 1.5 * n / SIZE);
//          }
//          timer.reset();
//
//          cout << "\nDisplaying average (max) for chaining:";
//          cout << "\n--------------------------------------n";
//          const char *ROW_LABELS[] = {
//              "\nIntegers   :",
//              "\nStrings    :",
//              "\nMultifield :",
//              "\nLoad factor:",
//          };
//          const int NUM_ROWS = sizeof ROW_LABELS / sizeof *ROW_LABELS -1;
//          cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
//          for (int n = INC; n < 4*SIZE; n += INC) {
//              cout << "\t" << (double)n / SIZE;
//          }
//          for (int j = 0; j < NUM_ROWS; ++j) {
//              cout << ROW_LABELS[j];
//              for (int i = 0; i < COLS; ++i) {
//                  cout << "\t" << results[j][i].d_average;
//                  cout << "(" << results[j][i].d_max << ")";
//              }
//              cout << "\n";
//          }
//      }
//..
// We repeat the same steps with double hashing, except that due to the nature
// of the collision-resolution mechanism, the tolerance for the average must be
// slightly increased to 2.5 times the load factor, when the load factor is
// high.
//..
//      // Using double hashing.
//      {
//          ExperimentalResult results[3][COLS];
//          bsls_Stopwatch timer;
//
//          timer.start();
//          for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
//              results[0][i] = computeDoubleHashingCollisions(n, SIZE,
//                                               SequentialIntegers(),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[0][i].d_average < 2.5 * n / SIZE);
//          }
//          if (verbose) cout << "\t... in " << timer.elapsedTime() << "\n";
//          timer.reset();
//
//          timer.start();
//          for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
//              bsl::string initial("abcdefghij"); // period = 10! = 3628800
//              results[1][i] = computeDoubleHashingCollisions(n, SIZE,
//                                               SequentialStrings(initial),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[1][i].d_average < 2.5 * n / SIZE);
//          }
//          timer.reset();
//
//          timer.start();
//          for (int n = INC/2, i = 0; n < SIZE; n += INC, ++i) {
//              bsl::vector<char> ranges(7, (char)8); // period = 8^7 = 2097152
//              results[2][i] = computeDoubleHashingCollisions(n, SIZE,
//                                               SequentialVector(ranges),
//                                               veryVerbose, veryVeryVerbose);
//              assert(results[2][i].d_average < 2.5 * n / SIZE);
//          }
//          timer.reset();
//
//          cout << "\nDisplaying average (max) for double-hashing:";
//          cout << "\n--------------------------------------------\n";
//          const char *ROW_LABELS[] = {
//              "\nIntegers   :",
//              "\nStrings    :",
//              "\nMultifield :",
//              "\nLoad factor:",
//          };
//          const int NUM_ROWS = sizeof ROW_LABELS / sizeof *ROW_LABELS -1;
//          cout << ROW_LABELS[NUM_ROWS] << bsl::setprecision(2);
//          for (int n = INC/2; n < SIZE; n += INC) {
//              cout << "\t" << (double)n / SIZE;
//          }
//          for (int j = 0; j < NUM_ROWS; ++j) {
//              cout << ROW_LABELS[j];
//              for (int i = 0; i < COLS; ++i) {
//                  cout << "\t" << results[j][i].d_average;
//                  cout << "(" << results[j][i].d_max << ")";
//              }
//              cout << "\n";
//          }
//      }
//
//      return 0;
//  }
//..
// The above code produces the following results, slightly reformatted for
// breaking long lines.  The results for chaining are identical for 'hash1'
// (used in the code above) or 'computeHash' (code identical except 'computeHash' is used
// in place of 'hash1' in 'computeChainingCollisions').
//..
// Displaying average(max) for chaining:
// -------------------------------------
// Load factor:       0.2     0.4     0.6     0.8     1       1.2     1.4
// Integers   :       0.2(3)  0.4(5)  0.6(6)  0.8(6)  1(7)    1.2(7)  1.4(8)
// Strings    :       0.2(4)  0.4(4)  0.6(5)  0.8(7)  1(7)    1.2(7)  1.4(7)
// Multifield :       0.2(5)  0.4(5)  0.6(10) 0.8(10) 1(15)   1.2(15) 1.4(20)
//
// Load factor:       1.6     1.8     2       2.2     2.4     2.6     2.8
// Integers   :       1.6(9)  1.8(9)  2(10)   2.2(10) 2.4(10) 2.6(10) 2.8(11)
// Strings    :       1.6(7)  1.8(8)  2(9)    2.2(11) 2.4(11) 2.6(11) 2.8(11)
// Multifield :       1.6(20) 1.8(24) 2(25)   2.2(29) 2.4(30) 2.6(34) 2.8(35)
//
// Load factor:       3       3.2     3.4     3.6     3.8
// Integers   :       3(11)   3.2(11) 3.4(12) 3.6(12) 3.8(13)
// Strings    :       3(12)   3.2(12) 3.4(13) 3.6(13) 3.8(13)
// Multifield :       3(39)   3.2(40) 3.4(42) 3.6(45) 3.8(46)
//
// Displaying average(max) for double-hashing:
// -------------------------------------------
// Load factor:       0.1      0.3     0.5      0.7      0.9
// Integers   :       0.046(2) 0.20(4) 0.37(10) 0.71(15) 1.6(59)
// Strings    :       0.064(2) 0.20(6) 0.40(12) 0.75(18) 1.6(50)
// Multifield :       0.05(2)  0.19(5) 0.58(9)  1.20(15) 2.4(64)
//..
// Note that the hashes provide near-identical results for all types of data
// distributions, as they should, and that chaining achieves a remarkable
// uniformity on average chain length.  The only difference is that the maximum
// chain and average double-hashing lengths are a bit longer for multiple-field
// keys.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP
{

namespace bslalg
{
                        // ===============
                        // struct HashUtil
                        // ===============

struct HashUtil {
    // This 'struct' provides a namespace for hash functions.

    // CLASS METHODS
    static unsigned int computeHash(char key);
    static unsigned int computeHash(signed char key);
    static unsigned int computeHash(unsigned char key);
    static unsigned int computeHash(short key);
    static unsigned int computeHash(unsigned short key);
    static unsigned int computeHash(int key);
    static unsigned int computeHash(unsigned int key);
    static unsigned int computeHash(long key);
    static unsigned int computeHash(unsigned long key);
    static unsigned int computeHash(long long key);
    static unsigned int computeHash(unsigned long long key);
    static unsigned int computeHash(float key);
    static unsigned int computeHash(double key);
    static unsigned int computeHash(const void *key);
        // Return an unsigned integer hash value corresponding to the specified
        // 'key'.  Note that the return value is seemingly random (i.e., the
        // hash is good) but identical on all platforms (irrespective of
        // endianness).  Both functions 'hash1' and 'computeHash' return a hash, but
        // both hashes can be assumed to be independent (i.e., there are no
        // known correlations between the results of both hash functions given
        // the same input data).
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

}  // close namespace BloombergLP::bslalg
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
