// bslstl_unorderedmultiset.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMULTISET
#define INCLUDED_BSLSTL_UNORDEREDMULTISET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_multiset' container.
//
//@CLASSES:
//   bsl::unordered_multiset : STL-compliant 'unordered_multiset' container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::unordered_multiset', implementing the standard container holding a
// collection of multiple keys with no guarantees on ordering (unless keys
// have the same value).
//
// An instantiation of 'unordered_multiset' is an allocator-aware,
// value-semantic type whose salient attributes are its size (number of keys)
// and the set of keys the 'unordered_multiset' contains, without regard to
// their order.  If 'unordered_multiset' is instantiated with a key type that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.  In particular, if the key type cannot be tested
// for equality, then an 'unordered_multiset' containing that type cannot be
// tested for equality.  It is even possible to instantiate
// 'unordered_multiset' with a key type that does not have an accessible
// copy-constructor, in which case the 'unordered_multiset' will not be
// copyable.  Note that the equality-comparison operator 'operator==' for each
// element is used to determine when two 'unordered_multiset' objects have the
// same value, and not the equality comparator supplied at construction.
//
// An 'unordered_multiset' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_multiset' implemented here adheres to the C++11 standard, except
// that it may rehash when setting the 'max_load_factor' in order to preserve
// the property that the value is always respected (which is a potentially
// throwing operation) and it does not have interfaces that take rvalue
// references, 'initializer_list', 'emplace', or operations taking a variadic
// number of template parameters.  Note that excluded C++11 features are those
// that require (or are greatly simplified by) C++11 compiler support.
//
///Requirements on 'KEY'
///---------------------
// An 'unordered_multiset' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' template parameters is
// fully value-semantic.  It is possible to instantiate an 'unordered_multiset'
// with 'KEY' parameter arguments that do not provide a full set of
// value-semantic operations, but then some methods of the container may not be
// instantiable.  The following terminology, adopted from the C++11 standard,
// is used in the function documentation of 'unordered_multiset' to describe a
// function's requirements for the 'KEY' template parameter.  These terms are
// also defined in section [utility.arg.requirements] of the C++11 standard.
// Note that, in the context of an 'unordered_multiset' instantiation, the
// requirements apply specifically to the 'unordered_multiset's element type,
// 'value_type', which is an alias for 'KEY'.
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
// 'HASH' shall support a function call operator compatible with the following
// statements:
//..
//  HASH        hash;
//  KEY         key;
//  std::size_t result = hash(key);
//..
// where the definition of the called function meets the requirements of a
// hash function, as specified in {'bslstl_hash'|Standard Hash Function}.
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
// The type supplied as a set's 'ALLOCATOR' template parameter determines how
// that set will allocate memory.  The 'unordered_multiset' template supports
// allocators meeting the requirements of the C++11 standard
// [allocator.requirements], and in addition it supports scoped-allocators
// derived from the 'bslma::Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use the template's default
// 'ALLOCATOR' type: The default type for the 'ALLOCATOR' template parameter,
// 'bsl::allocator', provides a C++11 standard-compatible adapter for a
// 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'unordered_multiset'
// instantiation is 'bsl::allocator', then objects of that set type will
// conform to the standard behavior of a 'bslma'-allocator-enabled type.  Such
// a set accepts an optional 'bslma::Allocator' argument at construction.  If
// the address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it is used to supply memory for the 'unordered_multiset'
// throughout its lifetime; otherwise, the 'unordered_multiset' will use the
// default allocator installed at the time of the 'unordered_multiset's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', an 'unordered_multiset'
// supplies that allocator's address to the constructors of contained objects
// of the parameterized 'KEY' types with the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_multiset':
//..
//  Legend
//  ------
//  'K'             - parameterized 'KEY' type of the unordered multiset
//  'a', 'b'        - two distinct objects of type 'unordered_multiset<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unordered_multiset<K>::value_type
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'v'             - an object of type 'value_type'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//  distance(p1,p2) - the number of elements in the range [p1, p2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_multiset<K> a; (default construction)    | O[1]               |
//  | unordered_multiset<K> a(al);                       |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(b); (copy construction)    | Average: O[n]      |
//  | unordered_multiset<K> a(b, al);                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(w);                        | O[n]               |
//  | unordered_multiset<K> a(w, hf);                    |                    |
//  | unordered_multiset<K> a(w, hf, eq);                |                    |
//  | unordered_multiset<K> a(w, hf, eq, al);            |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(i1, i2);                   | Average: O[        |
//  | unordered_multiset<K> a(i1, i2, w)                 |   distance(i1, i2)]|
//  | unordered_multiset<K> a(i1, i2, w, hf);            | Worst:   O[n^2]    |
//  | unordered_multiset<K> a(i1, i2, w, hf, eq);        |                    |
//  | unordered_multiset<K> a(i1, i2, w, hf, eq, al);    |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_multiset<K>(); (destruction)          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a,b)                               | O[1] if 'a' and    |
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
//  | get_allocator()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(v)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | Average O[         |
//  |                                                    |   distance(i1, i2)]|
//  |                                                    | Worst:  O[ n *     |
//  |                                                    |   distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
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
// No method of 'unordered_multiset' invalidates a pointer or reference to an
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
///Unordered Multi-Set Configuration
///---------------------------------
// The unordered multi-set has interfaces that can provide insight into and
// control of its inner workings.  The syntax and semantics of these interfaces
// for 'bslstl_unorderedmultiset' are identical to those of
// 'bslstl_unorderedmap'.  See the discussion in
// {'bslstl_unorderedmap'|Unordered Map Configuration} and the illustrative
// material in {'bslstl_unorderedmap'|Example 2}.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance an unordered multi-set (and any of
// the other unordered containers) is the choice of hash function.  Please see
// the discussion in {'bslstl_unorderedmap'|Practical Requirements on 'HASH'}.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Categorizing Data
/// - - - - - - - - - - - - - -
// Unordered sets are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// One uses a multi-set (ordered or unordered) when there may be more than one
// instance of an element of a set and when that multiplicity must be
// preserved.
//
// Note that the data type described below is an augmentation of that used in
// {'bslstl_unorderedset'|Example 1}.  The data itself (randomly generated) is
// different.
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  Additionally, there is some financial data associated with
// each customer: past sales and pending sales.
//
// The several customer attributes are modeled by several enumerations:
//..
//  typedef enum {
//      REPEAT
//    , DISCOUNT
//    , IMPULSE
//    , NEED_BASED
//    , BUSINESS
//    , NON_PROFIT
//    , INSTITUTE
//      // ...
//  } CustomerCode;
//
//  typedef enum {
//      USA_EAST
//    , USA_WEST
//    , CANADA
//    , MEXICO
//    , ENGLAND
//    , SCOTLAND
//    , FRANCE
//    , GERMANY
//    , RUSSIA
//      // ...
//  } LocationCode;
//
//  typedef enum {
//      TOAST
//    , GREEN
//    , FAST
//    , TIDY
//    , PEARL
//    , SMITH
//      // ...
//  } ProjectCode;
//..
// For printing these values in a human-readable form, we define these helper
// functions:
//..
//  static const char *toAscii(CustomerCode value)
//  {
//      switch (value) {
//        case REPEAT:     return "REPEAT";
//        case DISCOUNT:   return "DISCOUNT";
//        case IMPULSE:    return "IMPULSE";
//        case NEED_BASED: return "NEED_BASED";
//        case BUSINESS:   return "BUSINESS";
//        case NON_PROFIT: return "NON_PROFIT";
//        case INSTITUTE:  return "INSTITUTE";
//        // ...
//        default: return "(* UNKNOWN *)";
//      }
//  }
//
//  static const char *toAscii(LocationCode value)
//  {
//      ...
//  }
//
//  static const char *toAscii(ProjectCode  value)
//  {
//      ...
//  }
//..
// The data set (randomly generated for this example) is provided in a
// statically initialized array:
//..
//  static const struct CustomerDatum {
//      CustomerCode d_customer;
//      LocationCode d_location;
//      ProjectCode  d_project;
//      double       d_past;
//      double       d_pending;
//  } customerData[] = {
//     { REPEAT    , RUSSIA  , SMITH,   75674.00,     455.00 },
//     { REPEAT    , ENGLAND , TOAST,   35033.00,    8377.00 },
//     { BUSINESS  , USA_EAST, SMITH,   53942.00,    2782.00 },
//     ...
//     { DISCOUNT  , MEXICO  , GREEN,   99737.00,    3872.00 },
//  };
//
//  const int numCustomerData = sizeof customerData / sizeof *customerData;
//..
// Suppose, as a step in analysis, we wish to determine the average of the past
// sales and the average of the pending sales for each customer for each unique
// combination of customer attributes (i.e., for each customer profile in the
// data set).  To do so, we must aggregate our data items by customer profile
// but also retain the unique financial data for each item.  The
// 'bslstl_unorderedmultiset' provides those semantics.
//
// First, as there are no standard methods for hashing or comparing our user-
// defined types, we define 'CustomerDatumHash' and 'CustomerDatumEqual'
// classes, each a stateless functor.  Note that there is no meaningful
// ordering of the attribute values, they are merely arbitrary code numbers;
// nothing is lost by using an unordered set instead of an ordered set:
//..
//  class CustomerDatumHash
//  {
//    public:
//      // CREATORS
//      //! CustomerDatumHash() = default;
//          // Create a 'CustomerDatumHash' object.
//
//      //! hash(const CustomerDatumHash& original) = default;
//          // Create a 'CustomerDatumHash' object.  Note that as
//          // 'CustomerDatumHash' is an empty (stateless) type, this operation
//          // has no observable effect.
//
//      //! ~CustomerDatumHash() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      std::size_t operator()(CustomerDatum x) const;
//          // Return a hash value computed using the specified 'x'.
//  };
//
//  // ACCESSORS
//  std::size_t CustomerDatumHash::operator()(CustomerDatum x) const
//  {
//      return bsl::hash<int>()(x.d_location * 100 * 100
//                            + x.d_customer * 100
//                            + x.d_project);
//  }
//
//  class CustomerDatumEqual
//  {
//    public:
//      // CREATORS
//      //! CustomerDatumEqual() = default;
//          // Create a 'CustomerDatumEqual' object.
//
//      //! CustomerDatumEqual(const CustomerDatumEqual& original) = default;
//          // Create a 'CustomerDatumEqual' object.  Note that as
//          // 'CustomerDatumEqual' is an empty (stateless) type, this
//          // operation has no observable effect.
//
//      //! ~CustomerDatumEqual() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      bool operator()(const CustomerDatum& lhs,
//                      const CustomerDatum& rhs) const;
//  };
//
//  // ACCESSORS
//  bool CustomerDatumEqual::operator()(const CustomerDatum& lhs,
//                                      const CustomerDatum& rhs) const
//  {
//      return lhs.d_location == rhs.d_location
//          && lhs.d_customer == rhs.d_customer
//          && lhs.d_project  == rhs.d_project;
//  }
//..
// Notice that many of the required methods of the hash and comparator types
// are compiler generated.  (The declaration of those methods are commented out
// and suffixed by an '= default' comment.)
//
// Also notice that the boolean operation provided by 'CustomerDatumEqual' is
// more properly thought of as "equivalence", not "equality".  There may be
// more than one data item with the same customer profile (i.e., the same for
// our purpose here), but they have distinct financial data so the two items
// are not equal (unless the financial data also happens to match).
//
// Next, we define the type of the unordered set and a convenience aliases:
//..
//  typedef bsl::unordered_multiset<CustomerDatum,
//                                  CustomerDatumHash,
//                                  CustomerDatumEqual> DataByProfile;
//  typedef DataByProfile::const_iterator               DataByProfileConstItr;
//..
// Now, create a helper function to calculate the average financials for a
// category of customer profiles within the unordered map.
//..
//  void processCategory(DataByProfileConstItr  start,
//                       DataByProfileConstItr  end,
//                       FILE                  *out)
//      // Print to the specified 'out' in some human-readable format the
//      // averages of the 'past' and 'pending' attributes of every
//      // 'CustomerInfoData' object from the specified 'start' up to (but not
//      // including) the specified 'end'.  The behavior is undefined unless
//      // 'end != start'.
//  {
//      assert(end != start);
//      assert(out);
//
//      double sumPast    = 0.0;
//      double sumPending = 0.0;
//      int    count      = 0;
//
//      for (DataByProfileConstItr itr = start; end != itr; ++itr) {
//          sumPast    += itr->d_past;
//          sumPending += itr->d_pending;
//          ++count;
//      }
//      printf("%-10s %-8s %-5s %10.2f %10.2f\n",
//             toAscii(start->d_customer),
//             toAscii(start->d_location),
//             toAscii(start->d_project),
//             sumPast/count,
//             sumPending/count);
//  }
//..
// Then, we create an unordered set and insert each item of 'data'.
//..
//  DataByProfile dataByProfile;
//
//  for (int idx = 0; idx < numCustomerData; ++idx) {
//     dataByProfile.insert(customerData[idx]);
//  }
//  assert(numCustomerData == dataByProfile.size());
//..
// Finally, to calculate the statistics we need, we must detect the transition
// between categories as we iterate through 'customerInfoData'.
//..
//  CustomerDatumEqual    areEquivalent;
//  DataByProfileConstItr end             = dataByProfile.end();
//  DataByProfileConstItr startOfCategory = end;
//
//  for (DataByProfileConstItr itr  = dataByProfile.begin();
//                             end != itr; ++itr) {
//      if (end == startOfCategory) {
//          startOfCategory = itr;
//          continue;
//      }
//
//      if (!areEquivalent(*startOfCategory, *itr)) {
//          processCategory(startOfCategory, itr, stdout);
//          startOfCategory = itr;
//      }
//  }
//  if (end != startOfCategory) {
//      processCategory(startOfCategory, end, stdout);
//  }
//..
// We find on standard output:
//..
//  BUSINESS   GERMANY  TIDY    84553.00    3379.00
//  DISCOUNT   ENGLAND  TIDY    74110.00    2706.00
//  NEED_BASED CANADA   FAST    97479.00     681.00
//  ...
//  NEED_BASED SCOTLAND TOAST   27306.00    5084.50
//  INSTITUTE  CANADA   TIDY    83528.00    4722.33
//  NEED_BASED FRANCE   FAST    83741.50    5396.50
//  REPEAT     MEXICO   TOAST    7469.00    5958.00
//  BUSINESS   SCOTLAND FAST    24443.00    4247.00
//  INSTITUTE  FRANCE   FAST    19349.00    3982.00
//  NEED_BASED RUSSIA   TIDY    50712.00    8647.00
//  INSTITUTE  SCOTLAND TIDY    78240.00    6635.00
//  BUSINESS   RUSSIA   PEARL   29386.00    3623.00
//  INSTITUTE  FRANCE   PEARL   47747.00    3533.00
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedmultiset.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>  // Can probably escape with a fwd-decl, but not
#endif                         // very user friendly

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
#include <bslstl_pair.h>  // result type of 'equal_range' method
#endif

#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION
#include <bslstl_unorderedsetkeyconfiguration.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
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

                        // ========================
                        // class unordered_multiset
                        // ========================

template <class KEY,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<KEY> >
class unordered_multiset
{
    // This class template implements a value-semantic container type holding
    // an unordered multi set of values (of template parameter type 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:

      // PRIVATE TYPE
    typedef bsl::allocator_traits<ALLOCATOR> AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef KEY ValueType;
        // This typedef is an alias for the type of values maintained by this
        // set.

    typedef ::BloombergLP::bslstl::UnorderedSetKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This typedef is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the values maintained by
        // this multi-set.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this multi-set.

    typedef ::BloombergLP::bslalg::BidirectionalLink HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    // FRIEND
    template <class KEY2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                   const unordered_multiset<KEY2, HASH2, EQUAL2, ALLOCATOR2>&,
                   const unordered_multiset<KEY2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef KEY                                        value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    unordered_multiset,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

    typedef ::BloombergLP::bslstl::HashTableIterator<
                                   const value_type, difference_type> iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                             const value_type, difference_type> local_iterator;

    typedef iterator                                            const_iterator;
    typedef local_iterator                                const_local_iterator;

  private:
    // DATA
    HashTable d_impl;

  public:
    // CREATORS
    explicit unordered_multiset(
                      size_type             initialNumBuckets = 0,
                      const hasher&         hashFunction = hasher(),
                      const key_equal&      keyEqual = key_equal(),
                      const allocator_type& basicAllocator = allocator_type());
        // Construct an empty unordered multi-set.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // an implementation defined value is used.  Optionally specify a
        // 'hashFunction' used to generate the hash values associated to the
        // keys extracted from the values contained in this object.  If
        // 'hashFunction' is not supplied, a default-constructed object of type
        // 'hasher' is used.  Optionally specify a key-equality functor
        // 'keyEqual' used to verify that two key values are the same.  If
        // 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify the 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used to supply memory.

    explicit unordered_multiset(const allocator_type& basicAllocator);
        // Construct an empty unordered multi-set that uses the specified
        // 'basicAllocator' to supply memory.  Use a default-constructed object
        // of type 'hasher' to generate hash values for the key extracted from
        // the values contained in this object.  Also, use a
        // default-constructed object of type 'key_equal' to verify that two
        // key values are the same.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    unordered_multiset(const unordered_multiset& original);
    unordered_multiset(const unordered_multiset& original,
                       const allocator_type&     basicAllocator);
        // Construct an unordered set having the same value as that of the
        // specified 'original'.  Use a default-constructed object of type
        // 'hasher' to generate hash values for the key extracted from the
        // values contained in this object.  Also, use a default-constructed
        // object of type 'key_equal' to verify that two key values are the
        // same.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not supplied, a default-constructed
        // object of type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_multiset(INPUT_ITERATOR        first,
                       INPUT_ITERATOR        last,
                       size_type             initialNumBuckets = 0,
                       const hasher&         hashFunction = hasher(),
                       const key_equal&      keyEqual = key_equal(),
                       const allocator_type& basicAllocator =allocator_type());
        // Construct an empty unordered multi-set and insert each 'value_type'
        // object in the sequence starting at the specified 'first' element,
        // and ending immediately before the specified 'last' element, ignoring
        // those pairs having a key that appears earlier in the sequence.
        // Optionally specify an 'initialNumBuckets' indicating the initial
        // size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, an implementation defined value
        // is used.  Optionally specify a 'hashFunction' used to generate hash
        // values for the keys extracted from the values contained in this
        // object.  If 'hashFunction' is not supplied, a default-constructed
        // object of type 'hasher' is used.  Optionally specify a key-equality
        // functor 'keyEqual' used to verify that two key values are the same.
        // If 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify the 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used to supply memory.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type'.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.
        // Note that this method requires that the (template parameter) type
        // 'KEY' be "copy-constructible" (see {Requirements on 'KEY'}).

    ~unordered_multiset();
        // Destroy this object.

    // MANIPULATORS
    unordered_multiset& operator=(const unordered_multiset& rhs);
        // Assign to this object the value, hasher, and key-equality functor of
        // the specified 'rhs' object, propagate to this object the allocator
        // of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  Note that this method
        // requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this multi-set, or the 'end' iterator if this
        // multi-set is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // set.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index', in the array of buckets
        // maintained by this multi-set, or the 'end(index)' otherwise.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index's, in the array of buckets
        // maintained by this multi-set.

    void clear();
        // Remove all entries from this multi-set.  Note that the container is
        // empty after this call, but allocated memory may be retained for
        // future use.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multi-set having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  If this multi-set contains no 'value_type' objects
        // having 'key', then the two returned iterators will have the same
        // value.

    size_type erase(const key_type& key);
        // Remove from this multi-set all 'value_type' objects having the
        // specified 'key', if they exist, and return the number of object
        // erased; otherwise, if there is no 'value_type' object having 'key',
        // return 0 with no other effect.

    iterator erase(const_iterator position);
        // Remove from this multi-set the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this multi-set.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this multi-set.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from multi-set the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this multi-set or are
        // the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the ordered sequence provided by this container.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the first
        // 'value_type' objects in the sequence of all the value-elements of
        // this multi-set having the specified 'key', if such value-elements
        // exist, and the past-the-end ('end') iterator otherwise.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into multi-set;  if a 'value_type'
        // object having the same key (according to 'key_equal') as 'value'
        // already exists in multi-set, the operation is guaranteed to insert
        // 'value' in a position contiguous to another value having the same
        // key.  Return an iterator referring to the 'value_type' object
        // inserted.  Note that this method requires that the (template
        // parameter) type 'KEY' be "copy-constructible" (see {Requirements on
        // 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into multi-set (in constant time if the
        // specified 'hint' is a valid element in the bucket to which 'value'
        // belongs);  if a 'value_type' object having the same key (according
        // to 'key_equal') as 'value' already exists in this set, this method
        // is guaranteed to insert 'value' in a position contiguous to another
        // value in the set having the same key.  Return an iterator referring
        // to the newly inserted 'value_type' object.  If 'hint' is not a
        // position in the bucket of the key of 'value', this operation has
        // worst case 'O[N]' and average case constant-time complexity, where
        // 'N' is the size of multi-set.  The behavior is undefined unless
        // 'hint' is a valid iterator into this unordered multi set.  Note that
        // this method requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}), and that '*hint'
        // is an element contained in this container.

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into multi-set the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type'.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.
        // Note that this method requires that the (template parameter) type
        // 'KEY' be "copy-constructible" (see {Requirements on 'KEY'}).

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to the specified 'numBuckets', and redistribute all the contained
        // elements into the new sequence of buckets, according to their hash
        // values.  Note that this operation has no effect if rehashing the
        // elements into 'numBuckets' would cause multi-set to exceed its
        // 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this set to a quantity such that
        // the ratio between the specified 'numElements' and this quantity does
        // not exceed 'max_load_factor'.  Note that this guarantees that, after
        // the reserve, elements can be inserted to grow the container to
        // 'size() == numElements' without rehashing.  Also note that memory
        // allocations may still occur when growing the container to
        // 'size() == numElements'.  Also note that this operation has no
        // effect if 'numElements <= size()'.

    void swap(unordered_multiset& other);
        // Exchange the value of this object as well as its hasher and
        // key-equality functor with those of the specified 'other' object.
        // Additionally, if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees 'O[1]' complexity.  The behavior is undefined unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    // ACCESSORS
    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this multi-set, or the 'end' iterator if this
        // multi-set is empty.

    const_iterator end() const;
    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this multi-set.

    const_local_iterator begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this multi-set, or the 'end(index)' otherwise.

    const_local_iterator end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this multi-set.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where values having the specified 'key' would be
        // inserted.

    size_type bucket_count() const;
        // Return the number of buckets in the array of buckets maintained by
        // multi-set.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this map having the
        // specified 'key'.  Note that since an unordered set maintains unique
        // keys, the returned value will be either 0 or 1.

    bool empty() const;
        // Return 'true' if multi-set contains no elements, and 'false'
        // otherwise.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this container having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence and the second iterator is positioned one past
        // the end of the sequence.  If multi-set contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.  Note that since a set maintains unique keys, the range
        // will contain at most one element.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' objects in the sequence of value-elements of this
        // multi-set having the specified 'key', if such value-elements exist,
        // and the past-the-end ('end') iterator otherwise.

    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // set.

    key_equal key_eq() const;
        // Return (a copy of) the key-equality binary functor that returns
        // 'true' if the value of two 'key_type' objects is the same, and
        // 'false' otherwise.

    hasher hash_function() const;
        // Return (a copy of) the hash unary functor used by multi-set to
        // generate a hash value (of type 'size_t') for a 'key_type' object.

    float load_factor() const;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor leads to an increased
        // number of collisions, thus resulting in a loss performance.

    size_type max_bucket_count() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the set can successfully grow to the returned size,
        // or even close to that size without running out of resources.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this container.  If an
        // insert operation would cause 'load_factor' to exceed the
        // 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets the (see rehash).

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that multi-set could possibly hold.  Note that there is no guarantee
        // that the set can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    size_type size() const;
        // Return the number of elements in multi-set.

    // FRIEND
    template <class KEY2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                   const unordered_multiset<KEY2, HASH2, EQUAL2, ALLOCATOR2>&,
                   const unordered_multiset<KEY2, HASH2, EQUAL2, ALLOCATOR2>&);

};

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_multiset' objects have the
    // same value if they have the same number of value-elements, and for each
    // value-element that is contained in 'lhs' there is a value-element
    // contained in 'rhs' having the same value, and vice-versa.  Note that
    // this method requires that the (template parameter) type 'KEY' be
    // "equality-comparable" (see {Requirements on 'KEY'}).

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_multiset' objects do
    // not have the same value if they do not have the same number of
    // value-elements, or that for some value-element contained in 'lhs' there
    // is not a value-element in 'rhs' having the same value, and vice-versa.
    // Note that this method requires that the (template parameter) type 'KEY'
    // and be "equality-comparable" (see {Requirements on 'KEY'}).

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& a,
          unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& b);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  The behavior is
    // undefined unless this object was created with the same allocator as
    // 'other'.  Additionally, if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees 'O[1]' complexity.  The
    // behavior is undefined unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //-------------------------
                        // class unordered_multiset
                        //-------------------------

// CREATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::
unordered_multiset(size_type             initialNumBuckets,
                   const hasher&         hashFunction,
                   const key_equal&      keyEqual,
                   const allocator_type& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
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

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                          const allocator_type& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                      const unordered_multiset& original,
                                      const allocator_type&     basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                            const unordered_multiset& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::~unordered_multiset()
{
    // All memory management is handled by the base 'd_impl' member.
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                                                 const unordered_multiset& rhs)
{
    // Actually, need to check propagate_on_copy_assign trait

    unordered_multiset(rhs, get_allocator()).swap(*this);
    return *this;

}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
ALLOCATOR
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::get_allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::empty() const
{
    return 0 == d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_size() const
{
    return d_impl.maxSize();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin()
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end()
{
    return iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::clear()
{
    d_impl.removeAll();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::find(const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator,
          typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                           const key_type& key)
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return bsl::pair<iterator, iterator>(iterator(first), iterator(last));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    if (HashTableLink *target = d_impl.find(key)) {
        target = d_impl.remove(target);
        size_type result = 1;
        while (target &&
               this->key_eq()(key, ListConfiguration::extractKey(
                                     static_cast<BNode *>(target)->value()))) {
            target = d_impl.remove(target);
            ++result;
        }
        return result;                                                // RETURN
    }

    return 0;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator first,
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

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const value_type& value)
{
    return iterator(d_impl.insert(value));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const_iterator    hint,
                                                       const value_type& value)
{
    return iterator(d_impl.insert(value, hint.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                        INPUT_ITERATOR last)
{
    if (size_type maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last)) {
        this->reserve(this->size() + maxInsertions);
    }

    while (first != last) {
        d_impl.insert(*first);
        ++first;
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                           float newLoadFactor)
{
    d_impl.setMaxLoadFactor(newLoadFactor);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::rehash(size_type numBuckets)
{
    d_impl.rehashForNumBuckets(numBuckets);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::reserve(size_type numElements)
{
    d_impl.reserveForNumElements(numElements);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::swap(
                                                     unordered_multiset& other)
{
    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end() const
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cend() const
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
 unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
 unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cbegin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cend(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::bucket(
                                                     const key_type& key) const
{
    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::bucket_count() const
{
    return d_impl.numBuckets();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::bucket_size(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::count(
                                                     const key_type& key) const
{
    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    size_type result = 0;
    for (HashTableLink *cursor = d_impl.find(key);
         cursor;
         ++result, cursor = cursor->nextLink()) {

        BNode *cursorNode = static_cast<BNode *>(cursor);
        if (!this->key_eq()(
                         key,
                         ListConfiguration::extractKey(cursorNode->value()))) {
            break;
        }
    }
    return result;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::hasher
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<
      typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator,
      typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator>
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return bsl::pair<const_iterator, const_iterator>(const_iterator(first),
                                                     const_iterator(last));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_bucket_count() const
{
    return d_impl.maxNumBuckets();
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::load_factor() const
{
    return d_impl.loadFactor();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
{
    return d_impl.maxLoadFactor();
}

}  // close namespace bsl

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bsl::swap(bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& a,
          bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& b)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bitwise moveable if the both
//:      functors and the allocator are bitwise moveable.
//: o An unordered associative container uses 'bslma' allocators if the
//:      parameterized 'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR> >
     : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_multiset<KEY,
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
