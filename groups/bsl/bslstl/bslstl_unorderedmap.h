// bslstl_unorderedmap.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAP
#define INCLUDED_BSLSTL_UNORDEREDMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_map' container.
//
//@CLASSES:
//   bsl::unordered_map : STL-compliant 'unordered_map' container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::unordered_map', implementing the standard container holding a
// collection of unique keys, each mapped to an associated value with no
// guarantees on ordering.
//
// An instantiation of 'unordered_map' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of keys) and the set of
// key-value pairs the 'unordered_map' contains, without regard to their order.
// If 'unordered_map' is instantiated with a key type or mapped value-type that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.  In particular, if the key or value type cannot be
// tested for equality, then an 'unordered_map' containing that type cannot be
// tested for equality.  It is even possible to instantiate 'unordered_map'
// with types that do not have an accessible copy-constructor, in which case
// the 'unordered_map' will not be copyable.  Note that the equality-comparison
// operator for each key-value pair is used to determine when two
// 'unordered_map' objects have the same value, and not the instance of the
// 'EQUAL' template parameter supplied at construction.
//
// An 'unordered_map' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_map' implemented here adheres to the C++11 standard, except that
// it may rehash when setting the 'max_load_factor' in order to preserve the
// property that the value is always respected (which is a potentially throwing
// operation) and it does not have interfaces that take rvalue references,
// 'initializer_list', 'emplace', or operations taking a variadic number of
// template parameters.  Note that excluded C++11 features are those that
// require (or are greatly simplified by) C++11 compiler support.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// An 'unordered_map' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' and 'VALUE' template
// parameters are fully value-semantic.  It is possible to instantiate an
// 'unordered_map' with 'KEY' and 'VALUE' parameter arguments that do not
// provide a full set of value-semantic operations, but then some methods of
// the container may not be instantiable.  The following terminology, adopted
// from the C++11 standard, is used in the function documentation of
// 'unordered_map' to describe a function's requirements for the 'KEY' and
// 'VALUE' template parameters.  These terms are also defined in section
// [utility.arg.requirements] of the C++11 standard.  Note that, in the context
// of an 'unordered_map' instantiation, the requirements apply specifically to
// the 'unordered_map's element type, 'value_type', which is an alias for
// 'std::pair<const KEY, VALUE>'.
//
//: "default-constructible":
//:     The type provides an accessible default constructor.
//:
//: "copy-constructible":
//:     The type provides an accessible copy constructor.
//:
//: "equality-comparable":
//:     The type provides an equality-comparison operator that defines an
//:     equivalence relationship and is both reflexive and transitive.
//
///Requirements on 'HASH' and 'EQUAL'
///----------------------------------
// The (template parameter) types 'HASH' and 'EQUAL' must be copy-constructible
// function-objects.  Note that this requirement is somewhat stronger than the
// requirement currently in the standard; see the discussion for Issue 2215
// (http://cplusplus.github.com/LWG/lwg-active.html#2215);
//
// Naturally, if either 'HASH' or 'EQUAL' is to be the default for its type, it
// must be default-constructible as well.
//
// 'HASH' shall support a function call operator compatible with the following
// statements:
//..
//  HASH        hash;
//  KEY         key;
//  std::size_t result = hash(key);
//..
// where the definition of the called function meets the requirements of a
// hash function as specified in {'bslstl_hash'|Standard Hash Function}.
//
// 'EQUAL' shall support the a function call operator compatible with the
//  following statements:
//..
//  EQUAL equal;
//  KEY   key1, key2;
//  bool  result = equal(key1, key2);
//..
// where the definition of the called function defines an equivalence
// relationship on keys that is both reflexive and transitive.
//
// 'HASH' and 'EQUAL' function-objects are further constrained, such for any
// two objects whose keys compare equal by the comparator, shall produce the
// same value from the hasher.
//
///Memory Allocation
///-----------------
// The type supplied as the 'ALLOCATOR' template parameter determines how
// memory will be allocated.  The 'unordered_map' template supports allocators
// meeting the requirements of the C++11 standard [allocator.requirements],
// and, in addition, it supports scoped-allocators derived from the
// 'bslma::Allocator' memory allocation protocol.  Clients intending to use
// 'bslma' style allocators should use the template's default 'ALLOCATOR' type:
// The default type for the 'ALLOCATOR' template parameter, 'bsl::allocator',
// provides a C++11 standard-compatible adapter for a 'bslma::Allocator'
// object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'unordered_map'
// instantiation is 'bsl::allocator', then objects of that unordered map type
// will conform to the standard behavior of a 'bslma'-allocator-enabled type.
// Such an unordered map accepts an optional 'bslma::Allocator' argument at
// construction.  If the address of a 'bslma::Allocator' object is explicitly
// supplied at construction, it is used to supply memory for the
// 'unordered_map' throughout its lifetime; otherwise, the 'unordered_map' will
// use the default allocator installed at the time of the 'unordered_map's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', an 'unordered_map' supplies
// that allocator's address to the constructors of contained objects of the
// (template parameter) types 'KEY' and 'VALUE' having the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_map':
//..
//  Legend
//  ------
//  'K'               - template parameter type 'KEY' of the unordered map
//  'V'               - template parameter type 'VALUE' of the unordered map
//  'a', 'b'          - two distinct objects of type 'unordered_map<K, V>'
//  'n', 'm'          - number of elements in 'a' and 'b', respectively
//  'w'               - number of buckets of 'a'
//  'value_type'      - 'unordered_map<K, V>::value_type'
//  'hf'              - hash functor hashing objects of type 'K'
//  'eq'              - equality functor comparing objects of type 'K'
//  'al               - an STL-style memory allocator
//  'i1', 'i2'        - two iterators defining a sequence of 'value_type'
//                      objects
//  'k'               - an object of type 'K'
//  'v'               - an object of type 'value_type'
//  'p1', 'p2'        - two iterators belonging to 'a'
//  'distance(i1,i2)' - the number of elements in the range [i1, i2)
//  'distance(p1,p2)' - the number of elements in the range [p1, p2)
//  'z'               - a floating point value representing a load factor
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_map<K, V> a;    (default construction)   | O[1]               |
//  | unordered_map<K, V> a(al);                         |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(b); (copy construction)      | Average: O[m]      |
//  | unordered_map<K, V> a(b, al);                      | Worst:   O[m^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(w);                          | O[n]               |
//  | unordered_map<K, V> a(w, hf);                      |                    |
//  | unordered_map<K, V> a(w, hf, eq);                  |                    |
//  | unordered_map<K, V> a(w, hf, eq, al);              |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(i1, i2);                     | Average: O[N]      |
//  | unordered_map<K, V> a(i1, i2, w)                   | Worst:   O[N^2]    |
//  | unordered_map<K, V> a(i1, i2, w, hf);              | where N =          |
//  | unordered_map<K, V> a(i1, i2, w, hf, eq);          |  distance(i1, i2)] |
//  | unordered_map<K, V> a(i1, i2, w, hf, eq, al);      |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_map<K, V>(); (destruction)            | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1] if 'a' and    |
//  |                                                    | 'b' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.key_eq()                                         | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.hash_function()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.get_allocator()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a[k]                                               | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.at(k)                                            | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(v)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | Average: O[        |
//  |                                                    |   distance(i1, i2)]|
//  |                                                    | Worst:   O[n *     |
//  |                                                    |   distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | Average:           |
//  |                                                    |       O[a.count(k)]|
//  |                                                    | Worst:             |
//  |                                                    |       O[n]         |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | Average: O[        |
//  |                                                    |   distance(p1, p2)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_count()                                   | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_bucket_count()                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket(k)                                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_size(k)                                   | O[a.bucket_size(k)]|
//  +----------------------------------------------------+--------------------+
//  | a.load_factor()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_load_factor()                                | O[1]               |
//  | a.max_load_factor(z)                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.rehash(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.reserve(k)                                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//..
//
///Iterator, Pointer, and Reference Invalidation
///---------------------------------------------
// No method of 'unordered_map' invalidates a pointer or reference to an
// element in the set, unless it also erases that element, such as any 'erase'
// overload, 'clear', or the destructor (that erases all elements).  Pointers
// and references are stable through a rehash.
//
// Iterators to elements in the container are invalidated by any rehash, so
// iterators may be invalidated by an 'insert' or 'emplace' call if it triggers
// a rehash (but not otherwise).  Iterators to specific elements are also
// invalidated when that element is erased.  Note that the 'end' iterator is
// not an iterator referring to any element in the container, so may be
// invalidated by any non-'const' method.
//
///Unordered Map Configuration
///---------------------------
// The unordered map has interfaces that can provide insight into and control
// of its inner workings.  The unordered map is implemented using a hash table
// (see {'bslstl_hashtable'}), a dynamically sized array of "buckets".  If two
// elements hash to the same bucket (termed a "collision"), then that bucket
// will house multiple elements.  As elements are added to the unordered map,
// the number of buckets is increased (and the existing elements redistributed)
// to keep the average number of elements per bucket (the "loading factor")
// below the specified maximum (the "maximum load factor", 1 by default).
// {Example 2: Examining and Setting Unordered Map Configuration} illustrates
// the use of these interfaces.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance an unordered map (and any of the
// other unordered containers) is the choice of hash function.  In general, one
// wants the hash function to return uniformly distributed values that can be
// assigned to buckets (see {Unordered Map Configuration}) with few collisions.
//
// The 'bsl' package provides general purpose, default hash functions for
// 'bsl::string', 'bslstl::StringRef', and the arithmetic types (e.g., 'int');
// however, custom defined hash functions may do better, especially if one has
// information about the distribution of keys; there is considerable literature
// on designing hash functions.
//
// When a user-defined class is used as a key, hasher must be provided (and
// equality functor, if equality is not otherwise defined).  Two examples,
// {Example 3} and {'bslstl_unorderedset'|Example 1}, address this issue by
// adapting the existing default hash functions for primitive types, an
// approach that may not always prove adequate.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - -
// Unordered maps are useful in situations when there is no meaningful way to
// order the key values, when the order of the keys is irrelevant to the
// problem domain (see {Example 3}), and (even if there is a meaningful
// ordering) the value of ordering the results is outweighed by the higher
// performance provided by unordered maps (compared to ordered maps).
//
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a data base.  Gathering those statistics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of unique
// words appearing in those documents may be high.  The English language has in
// excess of a million words (albeit many appear infrequently), and, if the
// documents contain serial numbers, or Social Security numbers, or chemical
// formulas, etc., then the 'O[log(n)]' insertion time of ordered maps may well
// be inadequate.  The unordered map, having an 'O[1]' typical insertion cost,
// is a viable alternative.  In many problem domains, sorting, if needed, can
// be done after the data is gathered.
//
// This example illustrates the use of 'bsl::unordered_map' to gather one
// simple statistic (counts of unique words) on a single document.  To avoid
// irrelevant details of acquiring the data, several modestly sized documents
// are stored in static arrays:
//..
//  static char document0[] =
//  " IN CONGRESS, July 4, 1776.\n"
//  "\n"
//  " The unanimous Declaration of the thirteen united States of America,\n"
//  "\n"
//  " When in the Course of human events, it becomes necessary for one\n"
//  " people to dissolve the political bands which have connected them with\n"
//  " another, and to assume among the powers of the earth, the separate\n"
//  " and equal station to which the Laws of Nature and of Nature's God\n"
//  " entitle them, a decent respect to the opinions of mankind requires\n"
//  " that they should declare the causes which impel them to the\n"
//  " separation.  We hold these truths to be self-evident, that all men\n"
//  " are created equal, that they are endowed by their Creator with\n"
//  " certain unalienable Rights, that among these are Life, Liberty and\n"
//  " the pursuit of Happiness.--That to secure these rights, Governments\n"
//  " are instituted among Men, deriving their just powers from the consent\n"
//  " of the governed, --That whenever any Form of Government becomes\n"
//  ...
//  " States may of right do.  And for the support of this Declaration,\n"
//  " with a firm reliance on the protection of divine Providence, we\n"
//  " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
//  " Honor.\n";
//
//  static char document1[] =
//  "/The Universal Declaration of Human Rights\n"
//  "/-----------------------------------------\n"
//  "/Preamble\n"
//  "/ - - - -\n"
//  " Whereas recognition of the inherent dignity and of the equal and\n"
//  " inalienable rights of all members of the human family is the\n"
//  " foundation of freedom, justice and peace in the world,\n"
//  ...
//  "/Article 30\n"
//  "/ - - - - -\n"
//  " Nothing in this Declaration may be interpreted as implying for any\n"
//  " State, group or person any right to engage in any activity or to\n"
//  " perform any act aimed at the destruction of any of the rights and\n"
//  " freedoms set forth herein.\n";
//
//  static char document2[] =
//  "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
//  "/---------------------------------------------------\n"
//  " PREAMBLE\n"
//  "\n"
//  " The peoples of Europe, in creating an ever closer union among them,\n"
//  " are resolved to share a peaceful future based on common values.\n"
//  ...
//  "/Article 54\n"
//  "/-  -  -  -\n"
//  " Prohibition of abuse of rights\n"
//  "\n"
//  " Nothing in this Charter shall be interpreted as implying any right to\n"
//  " engage in any activity or to perform any act aimed at the destruction\n"
//  " of any of the rights and freedoms recognized in this Charter or at\n"
//  " their limitation to a greater extent than is provided for herein.\n";
//
//  static char * const documents[] = { document0,
//                                      document1,
//                                      document2
//                                    };
//  const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define an alias to make our code more comprehensible.
//..
//  typedef bsl::unordered_map<bsl::string, int> WordTally;
//..
// Next, we create an (empty) unordered map to hold our word tallies.  The
// output from the 'printf' statements will be discussed in {Example 2}.
//..
//  WordTally wordTally;
//
//  printf("size             %4d initial\n", wordTally.size());
//  printf("bucket_count     %4d initial\n", wordTally.bucket_count());
//  printf("load_factor      %f  initial\n", wordTally.load_factor());
//  printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
//..
// Then, we define the set of characters that define word boundaries:
//..
//  const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// For each iteration of the inner loop, that method looks for a map entry
// matching the given key value.  On the first occurrence of a word, the map
// has no such entry, so one is created with a default value of the mapped
// value (0, just what we want in this case) and inserted into the map where it
// is found on any subsequent occurrences of the word.  The 'operator[]' method
// returns a reference providing modifiable access to the mapped value.  Here,
// we apply the '++' operator to that reference to maintain a tally for the
// word.
//..
//  for (int idx = 0; idx < numDocuments; ++idx) {
//      for (char *cur = strtok(documents[idx], delimiters);
//                 cur;
//                 cur = strtok(NULL,     delimiters)) {
//          ++wordTally[bsl::string(cur)];
//      }
//  }
//..
// Now that the data has been (quickly) gathered, we can indulge in analysis
// that is more time consuming.  For example, we can define a comparison
// function, copy the data to another container (e.g., 'bsl::vector'), sort the
// entries, and determine the 20 most commonly used words in the given
// documents:
//..
//  typedef bsl::pair<bsl::string, int> WordTallyEntry;
//      // Assignable equivalent to 'WordTally::value_type'.  Note that
//      // 'bsl::vector' requires assignable types.
//
//  struct WordTallyEntryCompare {
//      static bool lessValue(const WordTallyEntry& a,
//                            const WordTallyEntry& b) {
//          return a.second < b.second;
//      }
//      static bool moreValue(const WordTallyEntry& a,
//                            const WordTallyEntry& b) {
//          return !lessValue(a, b);
//      }
//  };
//
//  bsl::vector<WordTallyEntry> array(wordTally.cbegin(), wordTally.cend());
//
//  assert(20 <= array.size());
//
//  std::partial_sort(array.begin(),
//                    array.begin() + 20,
//                    array.end(),
//                    WordTallyEntryCompare::moreValue);
//..
// Notice that 'partial_sort' suffices here since we seek only the 20 most used
// words, not a complete distribution of word counts.
//
// Finally, we print the sorted portion of 'array':
//..
//  for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
//                                                   end  = cur + 20;
//                                                   end != cur; ++cur) {
//      printf("%-10s %4d\n", cur->first.c_str(), cur->second);
//  }
//..
// and standard output shows:
//..
//  the         463
//  -           398
//  of          361
//  and         349
//  to          306
//  in          141
//  or          106
//  right        93
//  be           90
//  Article      86
//  has          79
//  a            76
//  shall        69
//  for          69
//  by           62
//  with         50
//  Everyone     49
//  rights       44
//  their        44
//  is           43
//..
// Notice that "-" (used as an header underscore in our markup) appears in the
// word count.  That could be eliminated by adding '-' to the set of
// delimiters; however, that would partition hyphenated words into separate
// words.  In practice, one defines a "stop list" of common words (e.g., "the",
// "of", "and", "is") that one does not wish to tally.  We could easily and "-"
// to the stop list.
//
///Example 2: Examining and Setting Unordered Map Configuration
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to examine (and possibly influence) the performance of an
// unordered map.  The unordered map provides several interfaces that allow us
// to do so.  Several of these were used in {Example 1} (code repeated below):
//..
//  WordTally wordTally;
//
//  printf("size             %4d initial\n", wordTally.size());
//  printf("bucket_count     %4d initial\n", wordTally.bucket_count());
//  printf("load_factor      %f  initial\n", wordTally.load_factor());
//  printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
//..
// First, we examine the metrics of of this newly created (empty) unordered
// map:
//..
//  size                0 initial
//  bucket_count        1 initial
//  load_factor      0.000000  initial
//  max_load_factor  1.000000  initial
//..
// Notice that even when there are no elements ('size' is 0) there is one
// bucket.  Since there are no elements, the average number of elements per
// bucket (the 'load_factor' above) must be 0.
//
// Next, after 'wordTally' has been loaded, we examine its metrics:
//..
//  printf("size             %4d\n", wordTally.size());
//  printf("bucket_count     %4d\n", wordTally.bucket_count());
//  printf("load_factor      %f\n",  wordTally.load_factor());
//  printf("max_load_factor  %f\n",  wordTally.max_load_factor());
//..
// and find at standard output:
//..
//  size             1504
//  bucket_count     2099
//  load_factor      0.716532
//  max_load_factor  1.000000
//..
// Notice how the number of buckets has increased.  (Sampling this metric as
// the map was loaded would show that the increase was done in several stages.)
//
// Then, we see that the load factor is indeed below the specified maximum;
// however we obtain further details of how the buckets are used.
//
// Using the 'bucket_count' method, the unordered map's interface for the
// number of elements in each bucket, we can easily determine the bucket with
// the greatest number of elements (i.e., the greatest number of collisions):
//..
//  bsl::vector<int> bucketSizes;
//  bucketSizes.reserve(wordTally.bucket_count());
//
//  for (size_t idx = 0; idx < wordTally.bucket_count(); ++idx) {
//     bucketSizes.push_back(static_cast<int>(wordTally.bucket_size(idx)));
//  }
//
//  assert(0 < bucketSizes.size());
//  int maxBucketSize = *std::max_element(bucketSizes.begin(),
//                                        bucketSizes.end());
//  printf("maxBucketSize    %4d\n", maxBucketSize);
//..
// and find on standard output:
//..
//  maxBucketSize       5
//..
// We can also count the number of empty buckets, and the number of buckets at
// 'maxBucketSize'.
//..
//  int numEmptyBuckets = static_cast<int>(std::count(bucketSizes.begin(),
//                                                    bucketSizes.end(),
//                                                    0));
//  printf("numEmptyBuckets  %4d\n", numEmptyBuckets);
//
//  int numMaxBuckets = static_cast<int>(std::count(bucketSizes.begin(),
//                                                  bucketSizes.end(),
//                                                  maxBucketSize));
//  printf("numMaxBuckets    %4d\n", numMaxBuckets);
//..
// which shows on standard output:
//..
//  numEmptyBuckets  1031
//  numMaxBuckets       3
//..
// Suppose we are not satisfied with this distribution.  (Perhaps the load
// factor is too high.)  We can create a second, differently configured table.
//
// Next, create a new table 'wordTally2' with twice the bucket count shown by
// the first table ('wordTally'), and examine its initial metrics.
//..
//  WordTally wordTally2(wordTally.bucket_count() * 2);
//
//  printf("size2            %4d initial\n", wordTally2.size());
//  printf("bucket_count2    %4d initial\n", wordTally2.bucket_count());
//  printf("load_factor2     %f  initial\n", wordTally2.load_factor());
//  printf("max_load_factor2 %f  initial\n", wordTally2.max_load_factor());
//..
// Standard output shows:
//..
//  size2               0 initial
//  bucket_count2    4201 initial
//  load_factor2     0.000000  initial
//  max_load_factor2 1.000000  initial
//..
// Notice that although we requested 4198 buckets (2 * 2099), we created a
// table with 4201 buckets.  (4201 is the smallest prime number greater than
// 4198).
//
// Then, we load our new table and examine its metrics.  For simplicity, we
// load data from the first table rather than re-tokenize our documents.
//..
//  wordTally2 = wordTally;
//
//  printf("size2            %4d\n", wordTally2.size());
//  printf("bucket_count2    %4d\n", wordTally2.bucket_count());
//  printf("load_factor2     %f\n",  wordTally2.load_factor());
//  printf("max_load_factor2 %f\n",  wordTally2.max_load_factor());
//
//  bsl::vector<int> bucketSizes2;
//  bucketSizes2.reserve(wordTally2.bucket_count());
//
//  for (size_t idx = 0; idx < wordTally2.bucket_count(); ++idx) {
//     bucketSizes2.push_back(static_cast<int>(wordTally2.bucket_size(idx)));
//  }
//
//  assert(0 < bucketSizes2.size());
//  int maxBucketSize2 = *std::max_element(bucketSizes2.begin(),
//                                         bucketSizes2.end());
//  printf("maxBucketSize2   %4d\n", maxBucketSize2);
//
//  int numEmptyBuckets2 = static_cast<int>(std::count(bucketSizes2.begin(),
//                                          bucketSizes2.end(),
//                                          0));
//  printf("numEmptyBuckets2 %4d\n", numEmptyBuckets2);
//
//  int numMaxBuckets2 = static_cast<int>(std::count(bucketSizes2.begin(),
//                                        bucketSizes2.end(),
//                                        maxBucketSize2));
//  printf("numMaxBuckets2   %4d\n", numMaxBuckets2);
//..
// Finally, we see on standard output:
//..
//  size2            1504
//  bucket_count2    4201
//  load_factor2     0.358010
//  max_load_factor2 1.000000
//  maxBucketSize2      4
//  numEmptyBuckets2 2971
//  numMaxBuckets2      5
//..
// Notice that the loading factor has been (roughly) cut in half; we have
// achieved our goal.  Also notice that the bucket count is unchanged since
// construction; thus, there were no rehashes during the loading this unordered
// map.  Finally, notice that the number of empty (unused) buckets is
// significantly higher, and there's been a modest decrease in the largest
// bucket size, but more instances of them.
//
// Thus, the unordered map provides facilities by which we can make trade-offs
// in performance characteristics of the containers we create.
//
///Example 3: Inverse Concordance
/// - - - - - - - - - - - - - - -
// If one has a concordance for a set of documents (an index of the position of
// every unique word in those documents), then words of interest can be
// efficiently located.  Suppose after locating a word of interest one also
// needs the surrounding words (for context).  Searching in the original
// document requires re-tokenization (time consuming).  Alternatively, one can
// use the concordance to create an inverse concordance to provide a fast
// lookup of the words at given locations in a document and then examine words
// near the word of interest.
//
// First, we define the types required (and convenient aliases) to create an
// unordered map from a word location to the corresponding word.  The "key"
// value will be 'WordLocation', a pair of 'int' values: the first being the
// document code number (arbitrarily assigned), and second the word offset in
// that document (the first word of the document is at offset 0).  The "value"
// of each entry is a 'bsl::string' containing the word at that location.
//..
//  typedef bsl::pair<int, int> WordLocation;
//      // Document code number ('first') and word offset ('second') in that
//      // document specify a word location.  The first word in the document
//      // is at word offset 0.
//..
// Notice that that 'WordLocation', the type of the key value, has no natural
// ordering.  The assignment of document codes is arbitrary so there is no
// reason to consider the words on one document to sort below those in any
// another.
//
// Then, since there is no default hash function for the 'WordLocation' type,
// we define one.  The document code and the word offset are individually
// hashed using the default hasher for the 'int' type and those results bitwise
// exclusive OR-ed a combined result.  This trivial combination formula
// suffices for this problem, but is *not* a general solution for combining
// hashes; see {Practical Requirements on 'HASH'}.
//..
//  class WordLocationHash
//  {
//    private:
//      WordLocationHash& operator=(const WordLocationHash& rhs);
//
//    public:
//      // CREATORS
//      //! WordLocationHash() = default;
//          // Create a 'WordLocationHash' object.
//
//      //! WordLocationHash(const WordLocationHash& original) = default;
//          // Create a 'WordLocationHash' object.  Note that as
//          // 'WordLocationHash' is an empty (stateless) type, this operation
//          // has no observable effect.
//
//      //! ~WordLocationHash() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      std::size_t operator()(WordLocation x) const
//          // Return a hash value computed using the specified 'x'.
//      {
//          bsl::hash<int> hasher;
//          return hasher(x.first) ^ hasher(x.second);
//      }
//  };
//..
// Notice that many of the required methods of the hash type are compiler
// generated.  (The declaration of those methods are commented out and suffixed
// by an '= default' comment.)
//
// In addition to a hash functor, the unordered map requires an equality
// comparison functor.  In this example, the unordered map uses 'operator=='
// method of 'std::pair' by default.  If the mapped type has no such method, a
// equality-comparison functor must be provided explicitly.
//
// Next, we define the type of the unordered map and associated convenience
// aliases:
//..
//  typedef bsl::unordered_map<WordLocation, bsl::string, WordLocationHash>
//                                               InverseConcordance;
//
//  typedef InverseConcordance::const_iterator   InverseConcordanceConstItr;
//..
// Next, we obtain a concordance for the document set (see
// {'bslstl_unorderedmultimap'|Example 1}).  Here, the concordance is provided
// as a statically initialized array:
//..
//  const static struct {
//      const char *d_word;
//      int         d_documentCode;
//      int         d_wordOffset;
//  } concordance[] = {
//      { "extent",             2,  3597 }, { "to",                 2,  1225 },
//      ...
//      { "to",                 2,  1252 }, { "Every",              2,  1049 }
//  };
//  const int numConcordance = sizeof concordance/sizeof *concordance;
//..
// Then, we create 'inverseConcordance', an unordered map, and initialize it
// with values obtained from 'concordance'.
//..
//  InverseConcordance inverseConcordance;
//
//  for (int idx = 0; idx < numConcordance; ++idx) {
//      bsl::string word         = concordance[idx].d_word;
//      int         documentCode = concordance[idx].d_documentCode;
//      int         wordOffset   = concordance[idx].d_wordOffset;
//
//      WordLocation                   location(documentCode, wordOffset);
//      InverseConcordance::value_type value(location, word);
//      bool                           status =
//                                     inverseConcordance.insert(value).second;
//      assert(status);
//  }
//..
// Notice that we expect every 'insert' to be successful, as the concordance
// should not show more than one word at any location.
//
// Next, suppose we knew the location of the word "unalienable" in the document
// set (see {'bslstl_unorderedmultimap'|Example 1}) and want to know its
// context?
//..
//  "unalienable",  0,  109
//..
// We use the 'find' method of 'inverseConcordance' to determine the words
// within offset 'delta' of "unalienable".  Note that we must check the
// validity of the returned iterator, in case we probe beyond the boundaries of
// the document.
//..
//  const int docCode =   0;
//  const int origin  = 109;
//  const int delta   =  16;
//
//  for (int offset = origin - delta; offset < origin + delta; ++offset) {
//      WordLocation               location(docCode, offset);
//      InverseConcordanceConstItr itr = inverseConcordance.find(location);
//
//      if (inverseConcordance.end() != itr) {
//          printf("%d %4d: %s\n",
//                 itr->first.first,
//                 itr->first.second,
//                 itr->second.c_str());
//          assert(origin != offset
//              || bsl::string("unalienable") == itr->second);
//      }
//  }
//..
// Notice that the assertion confirms that "unalienable" is found in our
// inverse location at the location we obtained from the concordance.
//
// Finally, we find on standard output:
//..
//  0   93: evident
//  0   94: that
//  0   95: all
//  0   96: men
//  0   97: are
//  0   98: created
//  0   99: equal
//  0  100: that
//  0  101: they
//  0  102: are
//  0  103: endowed
//  0  104: by
//  0  105: their
//  0  106: Creator
//  0  107: with
//  0  108: certain
//  0  109: unalienable
//  0  110: Rights
//  0  111: that
//  0  112: among
//  0  113: these
//  0  114: are
//  0  115: Life
//  0  116: Liberty
//  0  117: and
//  0  118: the
//  0  119: pursuit
//  0  120: of
//  0  121: Happiness
//  0  122: That
//  0  123: to
//  0  124: secure
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedmap.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_EQUALTO
#include <bslstl_equalto.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLE
#include <bslstl_hashtable.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEBUCKETITERATOR
#include <bslstl_hashtablebucketiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEITERATOR
#include <bslstl_hashtableiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATORUTIL
#include <bslstl_iteratorutil.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION
#include <bslstl_unorderedmapkeyconfiguration.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {
                        // =======================
                        // class bsl::unorderedmap
                        // =======================

template <class KEY,
          class VALUE,
          class HASH      = bsl::hash<KEY>,
          class EQUAL     = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class unordered_map {
    // This class template implements a value-semantic container type holding
    // an unordered set of key-value pairs having unique keys that provide a
    // mapping from keys (of template parameter type 'KEY') to their associated
    // values (of template parameter type 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:
    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR> AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits type associated
        // with this container.

    typedef bsl::pair<const KEY, VALUE>  ValueType;
        // This 'typedef' is an alias for the type of key-value pair objects
        // maintained by this unordered map.

    typedef BloombergLP::bslstl::UnorderedMapKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This 'typedef' is an alias for the policy used internally by this
        // unordered map to extract the 'KEY' value from the key-value pair
        // objects maintained by this unordered map.

    typedef BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this container.

    typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    typedef typename HashTable::NodeType HashTableNode;
        // This typedef is an alias for the type of nodes that hold the values
        // in this unordered map.

    // FRIENDS
    template <class KEY2,
              class VALUE2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                const unordered_map<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&,
                const unordered_map<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    unordered_map,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef VALUE                                      mapped_type;
    typedef bsl::pair<const KEY, VALUE>                value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef BloombergLP::bslstl::HashTableIterator<
                                         value_type, difference_type> iterator;
    typedef BloombergLP::bslstl::HashTableIterator<
                             const value_type, difference_type> const_iterator;
    typedef BloombergLP::bslstl::HashTableBucketIterator<
                                   value_type, difference_type> local_iterator;
    typedef BloombergLP::bslstl::HashTableBucketIterator<
                       const value_type, difference_type> const_local_iterator;

  private:
    // DATA
    HashTable d_impl;  // underlying hash table used by this unordered map

  public:
    // CREATORS
    explicit unordered_map(
                   size_type             initialNumBuckets = 0,
                   const hasher&         hashFunction      = hasher(),
                   const key_equal&      keyEqual          = key_equal(),
                   const allocator_type& basicAllocator    = allocator_type());
        // Create an empty unordered map having a 'max_load_factor' of 1.0.
        // Optionally specify an 'initialNumBuckets' indicating the minimum
        // initial size of the array of buckets of this unordered map.  If
        // 'initialNumBuckets' is not supplied, one empty bucket shall be used
        // and no memory allocated.  Optionally specify a 'hashFunction' used
        // to generate the hash values associated with the key-value pairs
        // contained in this unordered map.  If 'hashFunction' is not supplied,
        // a default-constructed object of type 'hasher' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to determine whether
        // two keys have the same value.  If 'keyEqual' is not supplied, a
        // default-constructed object of type 'key_equal' is used.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'allocator_type' is used.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then
        // 'basicAllocator' shall be convertible to 'bslma::Allocator *'.  If
        // the 'allocator_type' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  Note that more than 'initialNumBuckets' buckets may
        // be created in order to preserve the bucket allocation strategy of
        // the hash-table (but never fewer).

    explicit unordered_map(const allocator_type& basicAllocator);
        // Create an empty unordered map, having a 'max_load_factor' of 1.0,
        // that uses the specified 'basicAllocator' to supply memory.  Use a
        // default-constructed object of type 'hasher' to generate hash values
        // for the key-value pairs contained in this unordered map, and use a
        // default-constructed object of type 'key_equal' to determine whether
        // two keys have the same value.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    unordered_map(const unordered_map& original);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator, and 'max_load_factor' as the specified 'original'.  Use
        // the allocator returned by 'bsl::allocator_traits<allocator_type>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // supply memory.  If the 'allocator_type' is 'bsl::allocator' (the
        // default), the currently installed default allocator is used to
        // supply memory.

    unordered_map(const unordered_map&  original,
                  const allocator_type& basicAllocator);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator, and 'max_load_factor' as the specified 'original', and
        // using the specified 'basicAllocator' to supply memory.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then
        // 'basicAllocator' shall be convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_map(INPUT_ITERATOR        first,
                  INPUT_ITERATOR        last,
                  size_type             initialNumBuckets = 0,
                  const hasher&         hashFunction      = hasher(),
                  const key_equal&      keyEqual          = key_equal(),
                  const allocator_type& basicAllocator    = allocator_type());
        // Create an empty unordered map, having a 'max_load_factor' of 1.0,
        // and then create a 'value_type' object for each iterator in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, by converting from the object
        // referred to by each iterator.  Insert into this unordered map each
        // such object, ignoring those having a key that appears earlier in the
        // sequence.  Optionally specify a minimum 'initialNumBuckets'
        // indicating the minimum initial size of the array of buckets of this
        // unordered map.  If 'initialNumBuckets' is not supplied, and 'first'
        // and 'last' denote an empty range, a single empty bucket shall be
        // supplied.  Optionally specify a 'hashFunction' used to generate hash
        // values associated with the key-value pairs contained in this
        // unordered map.  If 'hashFunction' is not supplied, a
        // default-constructed object of type 'hasher' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to verify that two
        // key values are the same.  If 'keyEqual' is not supplied, a
        // default-constructed object of type 'key_equal' is used.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator is used to supply memory.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type'.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.
        // Note that more than 'initialNumBuckets' buckets may be created in
        // order to preserve the bucket allocation strategy of the hash-table
        // (but never fewer).

    ~unordered_map();
        // Destroy this object and each of its elements.

    // MANIPULATORS
    unordered_map& operator=(const unordered_map& rhs);
        // Assign to this object the value, hasher, key-equality functor, and
        // 'max_load_factor' of the specified 'rhs' object, propagate to this
        // object the allocator of 'rhs' if 'allocator_type' has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    mapped_type& operator[](const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key' in this unordered map; if this
        // unordered map does not already contain a 'value_type' object with
        // 'key', first insert a new 'value_type' object having 'key' and a
        // default-constructed 'VALUE' object.  Note that this method requires
        // that the (template parameter) type 'KEY' is "copy-constructible" and
        // the (template parameter) 'VALUE' is "default-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).

    mapped_type& at(const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key', if such an entry exists;
        // otherwise throw a 'std::out_of_range' exception.  Note that this
        // method is not exception-neutral.

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this unordered map, or the 'end' iterator if this
        // unordered map is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // unordered map.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map, or the 'end(index)' iterator if
        // the bucket is empty.  The behavior is undefined unless 'index <
        // bucket_count()'.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    void clear();
        // Remove all entries from this unordered map.  Note that this
        // unordered map will be empty after calling this method, but allocated
        // memory may be retained for future use.

    iterator erase(const_iterator position);
        // Remove from this unordered map the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this unordered map.  The
        // behavior is undefined unless 'position' refers to a 'value_type'
        // object in this unordered map.

    size_type erase(const key_type& key);
        // Remove from this unordered map the 'value_type' object having the
        // specified 'key', if it exists, and return 1; otherwise (there is no
        // 'value_type' object having 'key' in this unordered map) return 0
        // with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this unordered map the 'value_type' objects starting at
        // the specified 'first' position up to, but not including, the
        // specified 'last' position, and return 'last'.  The behavior is
        // undefined unless 'first' and 'last' either refer to elements in this
        // unordered map or are both the 'end' iterator, and the 'first'
        // position is at or before the 'last' position in the iteration
        // sequence provided by this container.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this unordered map having the specified 'key', if such an
        // entry exists, and the past-the-end iterator ('end') otherwise.

    template <class SOURCE_TYPE>
    pair<iterator, bool> insert(const SOURCE_TYPE& value);
        // Insert the specified 'value' into this unordered map if the key (the
        // 'first' element) of a 'value_type' object constructed from 'value'
        // does not already exist in this unordered map; otherwise, this method
        // has no effect (a 'value_type' object having the same key as the
        // converted 'value' already exists in this unordered map) .  Return a
        // 'pair' whose 'first' member is an iterator referring to the
        // (possibly newly inserted) 'value_type' object in this unordered map
        // whose key is the same as that of 'value', and whose 'second' member
        // is 'true' if a new value was inserted, and 'false' if the value was
        // already present.  Note that this method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be "copy-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).  Also note that this one
        // template stands in for two 'insert' functions in the C++11 standard.

    template <class SOURCE_TYPE>
    iterator insert(const_iterator hint, const SOURCE_TYPE& value);
        // Insert the specified 'value' into this unordered map if the key (the
        // 'first' element) of a 'value_type' object constructed from 'value'
        // does not already exist in this unordered map.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this unordered map whose key is the same as that of the converted
        // 'value'.  The behavior is undefined unless the specified 'hint' is a
        // valid iterator into this unordered map.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).
        // Also note that 'hint' is not used by this method template, and this
        // one template stands in for two 'insert' functions in the C++11
        // standard.

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Create a 'value_type' object for each iterator in the range starting
        // at the specified 'first' iterator and ending immediately before the
        // specified 'last' iterator, by converting from the object referred to
        // by each iterator.  Insert into this unordered map each such object
        // whose key is not already contained.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered map having the
        // specified 'key', where the first iterator is positioned at the start
        // of the sequence, and the second is positioned one past the end of
        // the sequence.  If this unordered map contains no 'value_type' object
        // having 'key', then the two returned iterators will have the same
        // value, 'end()'.  Note that since an unordered map maintains unique
        // keys, the range will contain at most one element.

    void max_load_factor(float newMaxLoadFactor);
        // Set the maximum load factor of this unordered map to the specified
        // 'newMaxLoadFactor'.  If 'newMaxLoadFactor < loadFactor()', this
        // operator require an immediate rehash; otherwise, it has a
        // constant-time cost.  The behavior is undefined unless '0 <
        // newMaxLoadFactor'.  Note that the C++11 standard does not allow this
        // operation to rehash, as it requires a constant cost for all
        // (positive) values of 'newMaxLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this unordered
        // map to at least the specified 'numBuckets', and redistribute all the
        // contained elements into the new sequence of buckets, according to
        // their hash values.  After this call, 'load_factor' will be less than
        // or equal to 'max_load_factor'.  This operation has no effect if
        // rehashing the elements into 'numBuckets' would cause this map to
        // exceed its 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this set to a quantity such that
        // the ratio between the specified 'numElements' and this quantity does
        // not exceed 'max_load_factor'.  Note that this guarantees that, after
        // the reserve, elements can be inserted to grow the container to
        // 'size() == numElements' without rehashing.  Also note that memory
        // allocations may still occur when growing the container to 'size() ==
        // numElements'.  Also note that this operation has no effect if
        // 'numElements <= size()'.

    void swap(unordered_map& other);
        // Exchange the value of this object as well as its hasher,
        // key-equality functor, and 'max_load_factor' with those of the
        // specified 'other' object.  If 'allocator_type' has the trait
        // 'propagate_on_container_swap', exchange the allocator of this object
        // with that of the 'other' object, and do not modify either allocator
        // otherwise.  This method provides the no-throw exception-safety
        // guarantee and guarantees 'O[1]' complexity, as long as the (template
        // parameter) 'HASH' and (template parameter) 'EQUAL' do not throw when
        // swapped.  The behavior is undefined unless either this object was
        // created with the same allocator as 'other' or 'allocator_type' has
        // the 'propagate_on_container_swap' trait.

    // ACCESSORS
    const mapped_type& at(const key_type& key) const;
        // Return a reference providing non-modifiable access to the
        // mapped-value associated with the specified 'key', if such an entry
        // exists; otherwise throw a 'std::out_of_range' exception.  Note that
        // this method is not exception-neutral.

    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this unordered map, or the 'end' iterator if this
        // unordered map is empty.

    const_iterator end() const;
    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects
        // maintained by this unordered map.

    const_local_iterator begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map, or the 'end(index)' iterator if
        // the bucket is empty.  The behavior is undefined unless
        // 'index < bucket_count()'.

    const_local_iterator end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets maintained
        // by this unordered map, where values having the specified 'key' would
        // be inserted.

    size_type bucket_count() const;
        // Return the number of buckets in the array of buckets maintained by
        // this unordered map.

    size_type max_bucket_count() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this unordered map could possibly manage.  Note that there is
        // no guarantee that the unordered map can successfully grow to the
        // returned size, or even close to that size, without running out of
        // resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects contained within this
        // unordered map having the specified 'key'.  Note that since an
        // unordered map maintains unique keys, the returned value will be
        // either 0 or 1.

    bool empty() const;
        // Return 'true' if this unordered map contains no elements, and
        // 'false' otherwise.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this unordered map having the
        // specified 'key', where the first iterator is positioned at the start
        // of the sequence, and the second is positioned one past the end of
        // the sequence.  If this unordered map contains no 'value_type' object
        // having 'key', then the two returned iterators will have the same
        // value, 'end()'.  Note that since an unordered map maintains unique
        // keys, the range will contain at most one element.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this unordered map having the specified
        // 'key', if such an entry exists, and the past-the-end iterator
        // ('end') otherwise.

    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // unordered map.

    hasher hash_function() const;
        // Return (a copy of) the unary hash functor used by this unordered map
        // to generate a hash value (of type 'std::size_t') for a 'key_type'
        // object.

    key_equal key_eq() const;
        // Return (a copy of) binary the key-equality functor used by this
        // unordered map that returns 'true' if two 'key_type' objects have the
        // same value, and 'false' otherwise.

    float load_factor() const;
        // Return the current ratio between the 'size' of this unordered map
        // and the number of buckets.  The load factor is a measure of how
        // full the container is, and a higher load factor typically leads to
        // an increased number of collisions, thus resulting in a loss of
        // performance.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this unordered map.  Note
        // that if an insert operation would cause the load factor to exceed
        // the 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets (see 'rehash').

    size_type size() const;
        // Return the number of elements in this unordered map.

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this unordered map could possibly hold.  Note that there is no
        // guarantee that the unordered map can successfully grow to the
        // returned size, or even close to that size, without running out of
        // resources.
};

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_map' objects have the
    // same value if they have the same number of key-value pairs, and for each
    // key-value pair that is contained in 'lhs' there is a key-value pair
    // contained in 'rhs' having the same value, and vice versa.  Note that
    // this method requires that the (template parameter) types 'KEY' and
    // 'VALUE' both be "equality-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_map' objects do not
    // have the same value if they do not have the same number of key-value
    // pairs, or for some key-value pair that is contained in 'lhs' there is
    // not a key-value pair in 'rhs' having the same value or vice-versa.  Note
    // that this method requires that the (template parameter) types 'KEY' and
    // 'VALUE' both be "equality-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
          unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b);
    // Exchange the value, the hasher, the key-equality functor, and the
    // 'max_load_factor' of the specified 'a' object with those of the
    // specified 'b' object.  If the (template parameter) type 'ALLOCATOR' has
    // the trait 'propagate_on_container_swap', exchange the allocator of 'a'
    // with that of 'b', and do not modify either allocator otherwise.  This
    // function provides the no-throw exception-safety guarantee and guarantees
    // 'O[1]' complexity, as long as the (template parameter) type 'HASH' and
    // the (template parameter) type 'EQUAL' do not throw when swapped.  The
    // behavior is undefined unless 'a' and 'b' were created with the same
    // allocator, or 'ALLOCATOR' has the 'propagate_on_container_swap' trait.

}  // close namespace bsl

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace bsl
{
                        //--------------------
                        // class unordered_map
                        //--------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
unordered_map(size_type             initialNumBuckets,
              const hasher&         hashFunction,
              const key_equal&      keyEqual,
              const allocator_type& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                       INPUT_ITERATOR        first,
                                       INPUT_ITERATOR        last,
                                       size_type             initialNumBuckets,
                                       const hasher&         hashFunction,
                                       const key_equal&      keyEqual,
                                       const allocator_type& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                                 const unordered_map& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                          const allocator_type& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                          const unordered_map&  original,
                                          const allocator_type& basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::~unordered_map()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                                                      const unordered_map& rhs)
{
    unordered_map(rhs, this->get_allocator()).swap(*this);
    return *this;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator[](
                                                           const key_type& key)
{
    HashTableLink *node = d_impl.insertIfMissing(key);
    return static_cast<HashTableNode *>(node)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::at(const key_type& key)
{
    HashTableLink *node = d_impl.find(key);

    if (!node) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                        "unordered_map<...>::at(key_type): invalid key value");
    }

    return static_cast<HashTableNode *>(node)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin()
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end()
{
    return iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::clear()
{
    d_impl.removeAll();
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT_SAFE(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    if (HashTableLink *target = d_impl.find(key)) {
        d_impl.remove(target);
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(const_iterator first,
                                                         const_iterator last)
{

#if defined BDE_BUILD_TARGET_SAFE_2
    if (first != last) {
        iterator it        = this->begin();
        const iterator end = this->end();
        for (; it != first; ++it) {
            BSLS_ASSERT(last != it);
            BSLS_ASSERT(end  != it);
        }
        for (; it != last; ++it) {
            BSLS_ASSERT(end  != it);
        }
    }
#endif

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node()); // convert from const_iterator
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
bsl::pair<typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
          bool>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                      const_iterator,
                                                      const SOURCE_TYPE& value)
{
    // There is no realistic use-case for the 'hint' in an 'unordered_map' of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    bool isInsertedFlag;  // not used

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return iterator(result);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
void unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                          INPUT_ITERATOR first,
                                                          INPUT_ITERATOR last)
{
    size_type maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);
    if (maxInsertions) {
        this->reserve(this->size() + maxInsertions);
    }

    bool isInsertedFlag;  // not used
    while (first != last) {
        d_impl.insertIfMissing(&isInsertedFlag, *first);
        ++first;
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
          typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                           const key_type& key)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    HashTableLink *first = d_impl.find(key);
    return first
         ? ResultType(iterator(first), iterator(first->nextLink()))
         : ResultType(iterator(0),     iterator(0));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                        float newMaxLoadFactor)

{
    d_impl.setMaxLoadFactor(newMaxLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::rehash(size_type numBuckets)
{
    d_impl.rehashForNumBuckets(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::reserve(
                                                         size_type numElements)
{
    d_impl.reserveForNumElements(numElements);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::swap(unordered_map& other)
{
    BSLS_ASSERT_SAFE(this->get_allocator() == other.get_allocator());

    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
const typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::at(
                                                     const key_type& key) const
{
    HashTableLink *target = d_impl.find(key);
    if (!target ){
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                        "unordered_map<...>::at(key_type): invalid key value");
    }
    return static_cast<HashTableNode *>(target)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
        unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket(
                                                     const key_type& key) const
{
    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_count() const
{
    return d_impl.numBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_bucket_count() const
{
    return d_impl.maxNumBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_size(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::count(
                                                     const key_type& key) const
{
    return d_impl.find(key) != 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::empty() const
{
    return 0 == d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_map<KEY,
                                 VALUE,
                                 HASH,
                                 EQUAL,
                                 ALLOCATOR>::const_iterator,
          typename unordered_map<KEY,
                                 VALUE,
                                 HASH,
                                 EQUAL,
                                 ALLOCATOR>::const_iterator>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    HashTableLink *first = d_impl.find(key);
    return first
         ? ResultType(const_iterator(first), const_iterator(first->nextLink()))
         : ResultType(const_iterator(0),     const_iterator(0));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
       unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::get_allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hasher
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::load_factor() const
{
    return d_impl.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
{
    return d_impl.maxLoadFactor();
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_size() const
{
    return d_impl.maxSize();
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bsl::swap(bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
          bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bit-wise moveable if both functors
//:      and the allocator are bit-wise moveable.
//: o An unordered associative container uses 'bslma' allocators if the
//:   (template parameter) type 'ALLOCATOR' is convertible from
//:   'bslma::Allocator *'.

namespace BloombergLP {
namespace bslalg {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR> >
     : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_map<KEY,
                                             VALUE,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> >
     : bsl::is_convertible<Allocator*, ALLOCATOR>::type
{};

}  // close namespace bslma

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
