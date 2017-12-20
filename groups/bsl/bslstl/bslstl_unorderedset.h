// bslstl_unorderedset.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSET
#define INCLUDED_BSLSTL_UNORDEREDSET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_set' container.
//
//@CLASSES:
//   bsl::unordered_set : STL-compliant 'unordered_set' container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::unordered_set', implementing the standard container holding a
// collection of unique keys with no guarantees on ordering.
//
// An instantiation of 'unordered_set' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of keys) and the set of
// keys the 'unordered_set' contains, without regard to their order.  If
// 'unordered_set' is instantiated with a key type that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// In particular, if the key type cannot be tested for equality, then an
// 'unordered_set' containing that type cannot be tested for equality.  It is
// even possible to instantiate 'unordered_set' with a key type that does not
// have an accessible copy-constructor, in which case the 'unordered_set' will
// not be copyable.  Note that the equality operator for each element is used
// to determine when two 'unordered_set' objects have the same value, and not
// the equality comparator supplied at construction.
//
// An 'unordered_set' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_set' implemented here adheres to the C++11 standard, except that
// it may rehash when setting the 'max_load_factor' in order to preserve the
// property that the value is always respected (which is a potentially throwing
// operation).
//
///Requirements on 'KEY'
///---------------------
// An 'unordered_set' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' template parameters is fully
// value-semantic.  It is possible to instantiate an 'unordered_set' with 'KEY'
// parameter arguments that do not provide a full set of value-semantic
// operations, but then some methods of the container may not be instantiable.
// The following terminology, adopted from the C++11 standard, is used in the
// function documentation of 'unordered_set' to describe a function's
// requirements for the 'KEY' template parameter.  These terms are also defined
// in section [utility.arg.requirements] of the C++11 standard.  Note that, in
// the context of an 'unordered_set' instantiation, the requirements apply
// specifically to the 'unordered_set's element type, 'value_type', which is an
// alias for 'KEY'.
//
// Legend
// ------
// 'X'    - denotes an allocator-aware container type (e.g., 'unordered_set')
// 'T'    - 'value_type' associated with 'X'
// 'A'    - type of the allocator used by 'X'
// 'm'    - lvalue of type 'A' (allocator)
// 'p'    - address ('T *') of uninitialized storage for a 'T' within an 'X'
// 'rv'   - rvalue of type (non-'const') 'T'
// 'v'    - rvalue or lvalue of type (possibly 'const') 'T'
// 'args' - 0 or more arguments
//
// The following terms are used to more precisely specify the requirements on
// template parameter types in function-level documentation.
//:
//: *default-insertable*: 'T' has a default constructor.  More precisely, 'T'
//:     is 'default-insertable' into 'X' means that the following expression is
//:     well-formed:
//:
//:      'allocator_traits<A>::construct(m, p)'
//:
//: *move-insertable*: 'T' provides a constructor that takes an rvalue of type
//:     (non-'const') 'T'.  More precisely, 'T' is 'move-insertable' into 'X'
//:     means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, rv)'
//:
//: *copy-insertable*: 'T' provides a constructor that takes an lvalue or
//:     rvalue of type (possibly 'const') 'T'.  More precisely, 'T' is
//:     'copy-insertable' into 'X' means that the following expression is
//:     well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, v)'
//:
//: *move-assignable*: 'T' provides an assignment operator that takes an rvalue
//:     of type (non-'const') 'T'.
//:
//: *copy-assignable*: 'T' provides an assignment operator that takes an lvalue
//:     or rvalue of type (possibly 'const') 'T'.
//:
//: *emplace-constructible*: 'T' is 'emplace-constructible' into 'X' from
//:     'args' means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, args)'
//:
//: *erasable*: 'T' provides a destructor.  More precisely, 'T' is 'erasable'
//:     from 'X' means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::destroy(m, p)'
//:
//: *equality-comparable*: The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
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
// that set will allocate memory.  The 'unordered_set' template supports
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
// If the parameterized 'ALLOCATOR' type of an 'unordered_set' instantiation
// is 'bsl::allocator', then objects of that set type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a set accepts
// an optional 'bslma::Allocator' argument at construction.  If the address of
// a 'bslma::Allocator' object is explicitly supplied at construction, it will
// be used to supply memory for the 'unordered_set' throughout its lifetime;
// otherwise, the 'unordered_set' will use the default allocator installed at
// the time of the 'unordered_set's construction (see 'bslma_default').  In
// addition to directly allocating memory from the indicated
// 'bslma::Allocator', an 'unordered_set' supplies that allocator's address to
// the constructors of contained objects of the parameterized 'KEY' types with
// the 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_set':
//..
//  Legend
//  ------
//  'K'             - parameterized 'KEY' type of the unordered set
//  'a', 'b'        - two distinct objects of type 'unordered_set<K>'
//  'rv'            - modifiable rvalue of type 'unordered_set<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unordered_set<K>::value_type
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'rk'            - modifiable rvalue of type 'K'
//  'v'             - an object of type 'value_type'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//  distance(p1,p2) - the number of elements in the range [p1, p2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_set<K> a;    (default construction)      | O[1]               |
//  | unordered_set<K> a(al);                            |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(b); (copy construction)         | Average: O[n]      |
//  | unordered_set<K> a(b, al);                         | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | set<K> a(rv); (move construction)                  | O[1] if 'a' and    |
//  | set<K> a(rv, al);                                  | 'rv' use the same  |
//  |                                                    | allocator;         |
//  |                                                    | otherwise,         |
//  |                                                    | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(w);                             | O[n]               |
//  | unordered_set<K> a(w, hf);                         |                    |
//  | unordered_set<K> a(w, hf, eq);                     |                    |
//  | unordered_set<K> a(w, hf, eq, al);                 |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(i1, i2);                        | Average: O[N]      |
//  | unordered_set<K> a(i1, i2, w)                      | Worst:   O[N^2]    |
//  | unordered_set<K> a(i1, i2, w, hf);                 | where N =          |
//  | unordered_set<K> a(i1, i2, w, hf, eq);             |  distance(i1, i2)] |
//  | unordered_set<K> a(i1, i2, w, hf, eq, al);         |                    |
//  |                                                    |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_set<K>(); (destruction)               | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a = rv;         (move assignment)                  | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator;         |
//  |                                                    | otherwise,         |
//  |                                                    | Average: O[n]      |
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
//  | a.insert(v)                                        | Average: O[1]      |
//  | a.insert(rk)                                       | Worst:   O[n]      |
//  | a.emplace(Args&&...)                               |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | Average: O[1]      |
//  | a.insert(p1, rk)                                   | Worst:   O[n]      |
//  | a.emplace_hint(p1, Args&&...)                      |                    |
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
// No method of 'unordered_set' invalidates a pointer or reference to an
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
///Unordered Set Configuration
///---------------------------
// The unordered set has interfaces that can provide insight into and control
// of its inner workings.  The syntax and semantics of these interfaces for
// 'bslstl_unorderedset' are identical to those of 'bslstl_unorderedmap'.  See
// the discussion in {'bslstl_unorderedmap'|Unordered Map Configuration} and
// the illustrative material in {'bslstl_unorderedmap'|Example 2}.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance an unordered set (and any of the
// other unordered containers) is the choice of hash function.  Please see the
// discussion in {'bslstl_unorderedmap'|Practical Requirements on 'HASH'}.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Categorizing Data
/// - - - - - - - - - - - - - -
// Unordered set are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  This data can be handled by creating an enumeration for each
// of the attributes:
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
//  static const struct CustomerProfile {
//      CustomerCode d_customer;
//      LocationCode d_location;
//      ProjectCode  d_project;
//  } customerProfiles[] = {
//      { IMPULSE   , CANADA  , SMITH },
//      { NON_PROFIT, USA_EAST, GREEN },
//      ...
//      { INSTITUTE , USA_EAST, TOAST },
//      { NON_PROFIT, ENGLAND , FAST  },
//      { NON_PROFIT, USA_WEST, TIDY  },
//      { REPEAT    , MEXICO  , TOAST },
//  };
//  const int numCustomerProfiles = sizeof  customerProfiles
//                                / sizeof *customerProfiles;
//..
// Suppose, as the first step in analysis, we wish to determine the number of
// unique combinations of customer attributes that exist in our data set.  We
// can do that by inserting each data item into an (unordered) set: the first
// insert of a combination will succeed, the others will fail, but at the end
// of the process, the set will contain one entry for every unique combination
// in our data.
//
// First, as there are no standard methods for hashing or comparing our user-
// defined types, we define 'CustomerProfileHash' and 'CustomerProfileEqual'
// classes, each a stateless functor.  Note that there is no meaningful
// ordering of the attribute values, they are merely arbitrary code numbers;
// nothing is lost by using an unordered set instead of an ordered set:
//..
//  class CustomerProfileHash
//  {
//    public:
//      // CREATORS
//      //! CustomerProfileHash() = default;
//          // Create a 'CustomerProfileHash' object.
//
//      //! CustomerProfileHash(const CustomerProfileHash& original) = default;
//          // Create a 'CustomerProfileHash' object.  Note that as
//          // 'CustomerProfileHash' is an empty (stateless) type, this
//          // operation has no observable effect.
//
//      //! ~CustomerProfileHash() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      std::size_t operator()(CustomerProfile x) const;
//          // Return a hash value computed using the specified 'x'.
//  };
//..
// The hash function combines the several enumerated values from the class
// (each a small 'int' value) into a single, unique 'int' value, and then
// applying the default hash function for 'int'.  See {Practical Requirements
// on 'HASH'}.
//..
//  // ACCESSORS
//  std::size_t CustomerProfileHash::operator()(CustomerProfile x) const
//  {
//      return bsl::hash<int>()(x.d_location * 100 * 100
//                            + x.d_customer * 100
//                            + x.d_project);
//  }
//
//  class CustomerProfileEqual
//  {
//    public:
//      // CREATORS
//      //! CustomerProfileEqual() = default;
//          // Create a 'CustomerProfileEqual' object.
//
//      //! CustomerProfileEqual(const CustomerProfileEqual& original)
//      //!                                                          = default;
//          // Create a 'CustomerProfileEqual' object.  Note that as
//          // 'CustomerProfileEqual' is an empty (stateless) type, this
//          // operation has no observable effect.
//
//      //! ~CustomerProfileEqual() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      bool operator()(const CustomerProfile& lhs,
//                      const CustomerProfile& rhs) const;
//          // Return 'true' if the specified 'lhs' have the same value as the
//          // specified 'rhs', and 'false' otherwise.
//  };
//
//  // ACCESSORS
//  bool CustomerProfileEqual::operator()(const CustomerProfile& lhs,
//                                        const CustomerProfile& rhs) const
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
// Then, we define the type of the unordered set and a convenience aliases:
//..
//  typedef bsl::unordered_set<CustomerProfile,
//                             CustomerProfileHash,
//                             CustomerProfileEqual> ProfileCategories;
//  typedef ProfileCategories::const_iterator        ProfileCategoriesConstItr;
//..
// Next, we create an unordered set and insert each item of 'data'.
//..
//  ProfileCategories profileCategories;
//
//  for (int idx = 0; idx < numCustomerProfiles; ++idx) {
//     profileCategories.insert(customerProfiles[idx]);
//  }
//
//  assert(numCustomerProfiles >= profileCategories.size());
//..
// Notice that we ignore the status returned by the 'insert' method.  We fully
// expect some operations to fail.
//
// Now, the size of 'profileCategories' matches the number of unique customer
// profiles in this data set.
//..
//  printf("%d %d\n", numCustomerProfiles, profileCategories.size());
//..
// Standard output shows:
//..
//  100 84
//..
// Finally, we can examine the unique set by iterating through the unordered
// set and printing each element.  Note the use of the several 'toAscii'
// functions defined earlier to make the output comprehensible:
//..
//  for (ProfileCategoriesConstItr itr  = profileCategories.begin(),
//                                 end  = profileCategories.end();
//                                 end != itr; ++itr) {
//      printf("%-10s %-8s %-5s\n",
//             toAscii(itr->d_customer),
//             toAscii(itr->d_location),
//             toAscii(itr->d_project));
//  }
//..
// We find on standard output:
//..
//  NON_PROFIT ENGLAND  FAST
//  DISCOUNT   CANADA   TIDY
//  IMPULSE    USA_WEST GREEN
//  ...
//  DISCOUNT   USA_EAST GREEN
//  DISCOUNT   MEXICO   SMITH
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedset.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
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

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#include <bslma_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>  // Can probably escape with a fwd-decl, but
#endif                           // not very user friendly

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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_CPP11
#include <bsls_cpp11.h>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#ifndef INCLUDED_INITIALIZER_LIST
#include <initializer_list>
#define INCLUDED_INITIALIZER_LIST
#endif
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

                        // ===================
                        // class unordered_set
                        // ===================

template <class KEY,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<KEY> >
class unordered_set {
    // This class template implements a value-semantic container type holding
    // an unordered set of unique values (of template parameter type 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for BDEX serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:

    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR>                 AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef KEY                                              ValueType;
        // This typedef is an alias for the type of values maintained by this
        // unordered set.

    typedef ::BloombergLP::bslstl::UnorderedSetKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This typedef is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the values maintained by
        // this unordered set.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR>      HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this set.

    typedef ::BloombergLP::bslalg::BidirectionalLink         HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    typedef BloombergLP::bslmf::MovableRefUtil               MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    // FRIEND
    template <class KEY2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                        const unordered_set<KEY2, HASH2, EQUAL2, ALLOCATOR2>&,
                        const unordered_set<KEY2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef KEY                                        value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;
    typedef value_type&                                reference;
    typedef const value_type&                          const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;
    typedef ::BloombergLP::bslstl::HashTableIterator<
                    const value_type, difference_type> iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                    const value_type, difference_type> local_iterator;

    typedef iterator                                   const_iterator;
    typedef local_iterator                             const_local_iterator;


  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    unordered_set,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

  private:
    // DATA
    HashTable  d_impl;

  public:
    // CREATORS
    unordered_set();
    explicit unordered_set(size_type        initialNumBuckets,
                           const HASH&      hashFunction = HASH(),
                           const EQUAL&     keyEqual = EQUAL(),
                           const ALLOCATOR& basicAllocator = ALLOCATOR());
    unordered_set(size_type        initialNumBuckets,
                  const HASH&      hashFunction,
                  const ALLOCATOR& basicAllocator);
    unordered_set(size_type        initialNumBuckets,
                  const ALLOCATOR& basicAllocator);
    explicit unordered_set(const ALLOCATOR& basicAllocator);
        // Create an empty unordered set.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // a single bucket is used.  Optionally specify a 'hashFunction' used
        // to generate the hash values for the keys contained in this set.  If
        // 'hashFunction' is not supplied, a default-constructed object of the
        // (template parameter) type 'HASH' is used.  Optionally specify a
        // key-equality functor 'keyEqual' used to verify that two key are
        // equivalent.  If 'keyEqual' is not supplied, a default-constructed
        // object of the (template parameter) type 'EQUAL' is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

    unordered_set(const unordered_set& original);
        // Create an unordered set having the same value as the specified
        // 'original' object.  Use a copy of 'original.hash_function()' to
        // generate hash values for the keys contained in this set.  Use a copy
        // of 'original.key_eq()' to verify that two keys are equivalent.  Use
        // the allocator returned by 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this set (see {Requirements on
        // 'KEY'}).

    unordered_set(BloombergLP::bslmf::MovableRef<unordered_set> original);
        // Create an unordered set having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new set.  Use a copy of 'original.hash_function()'
        // to generate hash values for the keys contained in this set.  Use a
        // copy of 'original.key_eq()' to verify that two keys are equivalent.
        // The allocator associated with 'original' is propagated for use in
        // the newly-created set.  'original' is left in a valid but
        // unspecified state.

    unordered_set(const unordered_set&  original,
                  const ALLOCATOR&      basicAllocator);
        // Create an unordered set having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.hash_function()' to generate hash
        // values for the keys contained in this set.  Use a copy of
        // 'original.key_eq()' to verify that two keys are equivalent.  This
        // method requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this set (see {Requirements on 'KEY'}).  Note
        // that a 'bslma::Allocator *' can be supplied for 'basicAllocator' if
        // the (template parameter) type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

    unordered_set(
                 BloombergLP::bslmf::MovableRef<unordered_set> original,
                 const ALLOCATOR&                              basicAllocator);
        // Create an unordered set having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  The contents of 'original' are moved (in constant time) to
        // the new set if 'basicAllocator == original.get_allocator()', and are
        // move-inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  Use a copy of
        // 'original.hash_function()' to generate hash values for the keys
        // contained in this set.  Use a copy of 'original.key_eq()' to verify
        // that two keys are equivalent.  This method requires that the
        // (template parameter) type 'KEY' be 'move-insertable' (see
        // {Requirements on 'KEY'}).  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the (template parameter) type
        // 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITERATOR>
    unordered_set(INPUT_ITERATOR   first,
                  INPUT_ITERATOR   last,
                  size_type        initialNumBuckets = 0,
                  const HASH&      hashFunction = HASH(),
                  const EQUAL&     keyEqual = EQUAL(),
                  const ALLOCATOR& basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    unordered_set(INPUT_ITERATOR   first,
                  INPUT_ITERATOR   last,
                  size_type        initialNumBuckets,
                  const HASH&      hashFunction,
                  const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_set(INPUT_ITERATOR   first,
                  INPUT_ITERATOR   last,
                  size_type        initialNumBuckets,
                  const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_set(INPUT_ITERATOR   first,
                  INPUT_ITERATOR   last,
                  const ALLOCATOR& basicAllocator);
        // Create an unordered set, and insert each 'value_type' object in the
        // sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element, ignoring those keys
        // having a value equivalent to that which appears earlier in the
        // sequence.  Optionally specify an 'initialNumBuckets' indicating the
        // initial size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, a single bucket is used.
        // Optionally specify a 'hashFunction' used to generate hash values for
        // the keys contained in this set.  If 'hashFunction' is not supplied,
        // a default-constructed object of (template parameter) type 'HASH' is
        // used.  Optionally specify a key-equality functor 'keyEqual' used to
        // verify that two key values are the same.  If 'keyEqual' is not
        // supplied, a default-constructed object of (template parameter) type
        // 'EQUAL' is used.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used to supply memory.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type', and
        // 'value_type' must be 'emplace-constructible' from '*i' into this
        // unordered set, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)' (see {Requirements on 'KEY'}).  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.  Note
        // that a 'bslma::Allocator *' can be supplied for 'basicAllocator' if
        // the type 'ALLOCATOR' is 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    unordered_set(std::initializer_list<KEY> values,
                  size_type                  initialNumBuckets = 0,
                  const HASH&                hashFunction = HASH(),
                  const EQUAL&               keyEqual = EQUAL(),
                  const ALLOCATOR&           basicAllocator = ALLOCATOR());
    unordered_set(std::initializer_list<KEY> values,
                  size_type                  initialNumBuckets,
                  const HASH&                hashFunction,
                  const ALLOCATOR&           basicAllocator);
    unordered_set(std::initializer_list<KEY> values,
                  size_type                  initialNumBuckets,
                  const ALLOCATOR&           basicAllocator);
    unordered_set(std::initializer_list<KEY> values,
                  const ALLOCATOR&           basicAllocator);
        // Create an unordered set and insert each 'value_type' object in the
        // specified 'values' initializer list, ignoring those keys having a
        // value equivalent to that which appears earlier in the list.
        // Optionally specify an 'initialNumBuckets' indicating the initial
        // size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, a single bucket is used.
        // Optionally specify a 'hashFunction' used to generate the hash values
        // for the keys contained in this set.  If 'hashFunction' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'HASH' is used.  Optionally specify a key-equality functor
        // 'keyEqual' used to verify that two keys are equivalent.  If
        // 'keyEqual' is not supplied, a default-constructed object of the
        // (template parameter) type 'EQUAL' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used to supply memory.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'copy-constructible' (see {Requirements on 'KEY'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).
#endif

    ~unordered_set();
        // Destroy this object.

    // MANIPULATORS
    unordered_set& operator=(const unordered_set& rhs);
        // Assign to this object the value, hash function, and equality
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  This
        // method requires that the (template parameter) type 'KEY' be
        // 'copy-assignable' and 'copy-insertable" into this set (see
        // {Requirements on 'KEY'}).

    unordered_set&
    operator=(BloombergLP::bslmf::MovableRef<unordered_set> rhs)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Assign to this object the value, hash function, and equality
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_move_assignment', and return a reference
        // providing modifiable access to this object.  The contents of 'rhs'
        // are moved (in constant time) to this set if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this set are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this set.  'rhs' is left in a valid but
        // unspecified state, and if an exception is thrown, '*this' is left in
        // a valid but unspecified state.  This method requires that the
        // (template parameter) type 'KEY' be both 'move-assignable' and
        // 'move-insertable' into this set (see {Requirements on 'KEY'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    unordered_set& operator=(std::initializer_list<KEY> values);
        // Assign to this object the value resulting from first clearing this
        // unordered set and then inserting each 'value_type' object in the
        // specified 'values' initializer list, ignoring those keys having a
        // value equivalent to that which appears earlier in the list; return a
        // reference providing modifiable access to this object.  This method
        // requires that the (template parameter) type 'KEY' type be
        // 'copy-insertable' into this set (see {Requirements on 'KEY'}).
#endif

    iterator begin() BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this set, or the 'end' iterator if this set is empty.

    iterator end() BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // unordered set.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index', in the array of buckets
        // maintained by this set, or the 'end(index)' otherwise.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index's, in the array of buckets
        // maintained by this set.

    pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  Return a pair whose 'first' member is an iterator referring
        // to the (possibly newly inserted) 'value_type' object in this set
        // that is equivalent to 'value', and whose 'second' member is 'true'
        // if a new value was inserted, and 'false' if the key was already
        // present.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' (see {Requirements on 'KEY'}).

    pair<iterator, bool> insert(
                             BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  'value' is left in a valid but unspecified state.  Return a
        // pair whose 'first' member is an iterator referring to the (possibly
        // newly inserted) 'value_type' object in this set that is equivalent
        // to 'value', and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' if the key was already present.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'move-insertable' into this set (see {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  Return an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this set that is equivalent to
        // 'value'.  The average and worst case complexity of this operation is
        // not affected by the specified 'hint'.  This method requires that the
        // (template parameter) type 'KEY' be 'copy-constructible' into this
        // set (see {Requirements on 'KEY'}).  The behavior is undefined unless
        // 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).  Note that 'hint' is ignored (other than
        // possibly asserting its validity in some build modes).

    iterator insert(const_iterator                             hint,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  'value' is left in a valid but unspecified state.  Return
        // an iterator referring to the (possibly newly inserted) 'value_type'
        // object in this set that is equivalent to 'value'.  The average and
        // worst case complexity of this operation is not affected by the
        // specified 'hint'.  This method requires that the (template
        // parameter) type 'KEY' be 'move-insertable' (see {Requirements on
        // 'KEY'}) into this set.  The behavior is undefined unless 'hint' is
        // an iterator in the range '[begin() .. end()]' (both endpoints
        // included).  Note that 'hint' is ignored (other than possibly
        // asserting its validity in some build modes).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this set the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, if a  key
        // equivalent to the object is not already contained in this set.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this set, where 'i' is a dereferenceable iterator in the
        // range '[first .. last)' (see {Requirements on 'KEY'}).  The behavior
        // is undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<KEY> values);
        // Insert into this set the value of each 'value_type' object in the
        // specified 'values' initializer list if a key equivalent to the
        // object is not already contained in this set.  This method requires
        // that the (template parameter) type 'KEY' be 'copy-insertable' (see
        // {Requirements on 'KEY'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    pair<iterator, bool> emplace(Args&&... arguments);
        // Insert into this unordered set a newly created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'value_type', if a key equivalent to such a value
        // does not already exist in this set; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return a pair whose 'first' member is an iterator
        // referring to the (possibly newly created and inserted) object in
        // this set whose value is equivalent to that of an object constructed
        // from 'arguments', and whose 'second' member is 'true' if a new value
        // was inserted, and 'false' if an equivalent key was already present.
        // This method requires that the (template parameter) type 'KEY' be
        // 'emplace-constructible' into this set from 'arguments' (see
        // {Requirements on 'KEY'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... arguments);
        // Insert into this unordered set a newly created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'value_type', if a key equivalent to such a value
        // does not already exists in this set; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return an iterator referring to the (possibly newly
        // created and inserted) object in this set whose value is equivalent
        // to that of an object constructed from 'arguments'.  The average and
        // worst case complexity of this operation is not affected by the
        // specified 'hint'.  This method requires that the (template
        // parameter) type 'KEY' be 'emplace-constructible' into this set from
        // 'arguments' (see {Requirements on 'KEY'}).  The behavior is
        // undefined unless 'hint' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).  Note that 'hint' is
        // ignored (other than possibly asserting its validity in some build
        // modes).

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_unorderedset.h
    pair<iterator, bool> emplace(
                         );

    template <class Args_01>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class Args_01,
              class Args_02>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class Args_01,
              class Args_02,
              class Args_03>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    pair<iterator, bool> emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);


    iterator emplace_hint(const_iterator hint);

    template <class Args_01>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class Args_01,
              class Args_02>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class Args_01,
              class Args_02,
              class Args_03>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    iterator emplace_hint(const_iterator hint,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    pair<iterator, bool> emplace(
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class... Args>
    iterator emplace_hint(const_iterator hint,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    iterator erase(const_iterator position);
        // Remove from this unordered set the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this set.  This method
        // invalidates only iterators and references to the removed element and
        // previously saved values of the 'end()' iterator, and preserves the
        // relative order of the elements not removed.  The behavior is
        // undefined unless 'position' refers to a 'value_type' object in this
        // unordered set.

    size_type erase(const key_type& key);
        // Remove from this set the 'value_type' object that is equivalent to
        // the specified 'key', if such an entry exists, and return 1;
        // otherwise, if there is no 'value_type' object that is equivalent to
        // 'key', return 0 with no other effect.  This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator, and preserves the relative order of
        // the elements not removed.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this set the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  This method invalidates only iterators
        // and references to the removed element and previously saved values of
        // the 'end()' iterator, and preserves the relative order of the
        // elements not removed.  The behavior is undefined unless 'first' and
        // 'last' either refer to elements in this set or are the 'end'
        // iterator, and the 'first' position is at or before the 'last'
        // position in the sequence provided by this container.

    void swap(unordered_set& other)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Exchange the value, hash function, and equality comparator of this
        // object with the value, hash function, and equality comparator of the
        // specified 'other' object.  Additionally, if
        // 'bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee if and only
        // if the hash function and equality comparator provide a no-throw
        // swap.  The behavior is undefined unless either both 'a' and 'b' use
        // the same allocator or 'propagate_on_container_swap' is 'true'.

    void clear() BSLS_CPP11_NOEXCEPT;
        // Remove all entries from this unordered set.  Note that the set is
        // empty after this call, but allocated memory may be retained for
        // future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this set that is equivalent to the specified 'key', if
        // such an entry exists, and the past-the-end ('end') iterator
        // otherwise.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered set that are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered set
        // contains no 'value_type' objects equivalent to 'key', then the two
        // returned iterators will have the same value.  Note that since a set
        // maintains unique keys, the range will contain at most one element.

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to the specified 'numBuckets', and redistribute all the contained
        // elements into the new sequence of buckets, according to their hash
        // values.  Note that this operation has no effect if rehashing the
        // elements into 'numBuckets' would cause this set to exceed its
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

    // ACCESSORS
    ALLOCATOR get_allocator() const BSLS_CPP11_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // unordered set.

    const_iterator begin() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this set, or the 'end' iterator if this set is empty.

    const_iterator end() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects
        // maintained by this set.

    const_iterator cbegin() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this set, or the 'cend' iterator if this set is empty.

    const_iterator cend() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this set.

    bool empty() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if this set contains no elements, and 'false'
        // otherwise.

    size_type size() const BSLS_CPP11_NOEXCEPT;
        // Return the number of elements in this set.

    size_type max_size() const BSLS_CPP11_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this set could possibly hold.  Note that there is no guarantee
        // that the set can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    EQUAL key_eq() const;
        // Return (a copy of) the key-equality binary functor that returns
        // 'true' if the value of two 'key_type' objects are equivalent, and
        // 'false' otherwise.

    HASH hash_function() const;
        // Return (a copy of) the hash unary functor used by this set to
        // generate a hash value (of type 'size_t') for a 'key_type' object.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this set that is equivalent to the specified
        // 'key', if such an entry exists, and the past-the-end ('end')
        // iterator otherwise.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this set that are
        // equivalent to the specified 'key'.  Note that since an unordered set
        // maintains unique keys, the returned value will be either 0 or 1.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this set that are equivalent to
        // the specified 'key', where the first iterator is positioned at the
        // start of the sequence and the second iterator is positioned one past
        // the end of the sequence.  If this set contains no 'value_type'
        // objects equivalent to 'key', then the two returned iterators will
        // have the same value.  Note that since a set maintains unique keys,
        // the range will contain at most one element.

    size_type bucket_count() const BSLS_CPP11_NOEXCEPT;
        // Return the number of buckets in the array of buckets maintained by
        // this set.

    size_type max_bucket_count() const BSLS_CPP11_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the set can successfully grow to the returned size,
        // or even close to that size without running out of resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where a value equivalent to the specified 'key' would be
        // inserted.

    const_local_iterator begin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this set, or the 'end(index)' otherwise.

    const_local_iterator end(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this set.

    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this set, or the 'cend(index)' otherwise.

    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this set.

    float load_factor() const BSLS_CPP11_NOEXCEPT;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor leads to an increased
        // number of collisions, thus resulting in a loss performance.

    float max_load_factor() const BSLS_CPP11_NOEXCEPT;
        // Return the maximum load factor allowed for this container.  If an
        // insert operation would cause 'load_factor' to exceed the
        // 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets the (see rehash).
};

// FREE OPERATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_set' objects have the same
    // value if they have the same number of value-elements, and for each
    // value-element that is contained in 'lhs' there is a value-element
    // contained in 'rhs' having the same value, and vice-versa.  Note that
    // this method requires that the (template parameter) type 'KEY' be
    // 'equality-comparable' (see {Requirements on 'KEY'}).

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_set' objects do not
    // have the same value if they do not have the same number of
    // value-elements, or that for some value-element contained in 'lhs' there
    // is not a value-element in 'rhs' having the same value, and vice-versa.
    // Note that this method requires that the (template parameter) type 'KEY'
    // and be 'equality-comparable' (see {Requirements on 'KEY'}).

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& a,
          unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& b)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
    // Exchange the value, hash function, and equality comparator of the
    // specified 'a' object with the value, hash function, and equality
    // comparator of the specified 'b' object.  Additionally, if
    // 'bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This function provides the no-throw
    // exception-safety guarantee if and only if the hash function and equality
    // comparator provide a no-throw swap operation.  Note that 'a' and 'b' are
    // left in valid but unspecified states if an exception is thrown (in the
    // case where 'propagate_on_container_swap' is 'false' and 'a' and 'b' were
    // created with different allocators), such as when the comparator objects
    // are swapped.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //--------------------
                        // class unordered_set
                        //--------------------

// CREATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set()
: d_impl(HASH(), EQUAL(), 0, 1.0f, ALLOCATOR())
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            size_type        initialNumBuckets,
                                            const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                               const ALLOCATOR& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                                 const unordered_set& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                        BloombergLP::bslmf::MovableRef<unordered_set> original)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                           const unordered_set& original,
                                           const ALLOCATOR&     basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                  BloombergLP::bslmf::MovableRef<unordered_set> original,
                  const ALLOCATOR&                              basicAllocator)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl), basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                               INPUT_ITERATOR   first,
                                               INPUT_ITERATOR   last,
                                               const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), 0, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                  std::initializer_list<KEY> values,
                                  size_type                  initialNumBuckets,
                                  const hasher&              hashFunction,
                                  const key_equal&           keyEqual,
                                  const ALLOCATOR&           basicAllocator)
: unordered_set(values.begin(), values.end(), initialNumBuckets,
                hashFunction, keyEqual, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                  std::initializer_list<KEY> values,
                                  size_type                  initialNumBuckets,
                                  const HASH&                hashFunction,
                                  const ALLOCATOR&           basicAllocator)
: unordered_set(values.begin(),
                values.end(),
                initialNumBuckets,
                hashFunction,
                EQUAL(),
                basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                 std::initializer_list<KEY> values,
                                 size_type                  initialNumBuckets,
                                 const ALLOCATOR&           basicAllocator)
: unordered_set(values.begin(),
                values.end(),
                initialNumBuckets,
                HASH(),
                EQUAL(),
                basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                 std::initializer_list<KEY> values,
                                 const ALLOCATOR&           basicAllocator)
: unordered_set(values.begin(),
                values.end(),
                0,
                HASH(),
                EQUAL(),
                basicAllocator)
{
}

#endif

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::~unordered_set()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::operator=(const unordered_set& rhs)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    d_impl = rhs.d_impl;

    return *this;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                             BloombergLP::bslmf::MovableRef<unordered_set> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    unordered_set& lvalue = rhs;

    d_impl = MoveUtil::move(lvalue.d_impl);

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                                             std::initializer_list<KEY> values)
{
    unordered_set tmp(values, d_impl.allocator());

    d_impl.swap(tmp.d_impl);

    return *this;
}
#endif

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin() BSLS_CPP11_NOEXCEPT
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end() BSLS_CPP11_NOEXCEPT
{
    return iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::clear() BSLS_CPP11_NOEXCEPT
{
    d_impl.removeAll();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(Args&&... arguments)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                                                         Args&&... arguments)
{
    // There is no realistic use-case for the 'hint' in an 'unordered_set' of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...).first;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_unorderedset.h
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                          )
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09,
          class Args_10>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                         BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));

    return ResultType(iterator(result), isInsertedFlag);
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator)
{

    return
        this->emplace().first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                      BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                      BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                      BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                      BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                      BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                      BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                      BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                      BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                      BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                      BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09)
                      ).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09,
          class Args_10>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                      BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                      BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                      BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                      BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                      BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                      BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                      BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                      BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                      BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10)
                      ).first;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace(
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.emplaceIfMissing(
           &isInsertedFlag, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(const_iterator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{

    return
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...).first;
}
// }}} END GENERATED CODE
#endif

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator,
          typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(const key_type& key)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    iterator first  = this->find(key);
    if (first == this->end()) {
        return ResultType(first, first);                              // RETURN
    }
    else {
        iterator next = first;
        return ResultType(first, ++next);                             // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    if (HashTableLink *target = d_impl.find(key)) {
        d_impl.remove(target);
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator first,
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

    return iterator(first.node());          // convert from const_iterator
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::find(const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(const value_type& value)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag,
                                                   MoveUtil::move(value));

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(const_iterator,
                                                   const value_type& value)
{
    // There is no realistic use-case for the 'hint' in an 'unordered_set' of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    return this->insert(value).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                              const_iterator,
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    // There is no realistic use-case for the 'hint' in an 'unordered_set' of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    return this->insert(MoveUtil::move(value)).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                        INPUT_ITERATOR last)
{
    if (size_type maxInsertions = static_cast<size_type>(
           ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last))) {
        this->reserve(this->size() + maxInsertions);
    }

    bool isInsertedFlag;  // value is not used

    while (first != last) {
        d_impl.insertIfMissing(&isInsertedFlag, *first);
        ++first;
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                             std::initializer_list<KEY> values)
{
    insert(values.begin(), values.end());
}
#endif

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                           float newLoadFactor)
{
    d_impl.setMaxLoadFactor(newLoadFactor);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::rehash(size_type numBuckets)
{
    d_impl.rehashForNumBuckets(numBuckets);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::reserve(size_type numElements)
{
    d_impl.reserveForNumElements(numElements);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::swap(unordered_set& other)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::get_allocator() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return d_impl.allocator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cbegin() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cend() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::empty() const BSLS_CPP11_NOEXCEPT
{
    return 0 == d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size() const BSLS_CPP11_NOEXCEPT
{
    return d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_size() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::hasher
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::find(const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::count(const key_type& key) const
{
    return 0 != d_impl.find(key);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator,
          typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    const_iterator first = this->find(key);
    if (first == this->end()) {
        return ResultType(first, first);                              // RETURN
    }
    else {
        const_iterator next = first;
        return ResultType(first, ++next);                             // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket_count() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return d_impl.numBuckets();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_bucket_count() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return d_impl.maxNumBuckets();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket_size(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket(const key_type& key) const
{
    BSLS_ASSERT_SAFE(this->bucket_count() > 0);

    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cbegin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cend(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::load_factor() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return d_impl.loadFactor();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return d_impl.maxLoadFactor();
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void bsl::swap(bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& a,
               bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& b)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bitwise movable if both functors
//:   and the allocator are bitwise movable.
//: o An unordered associative container uses 'bslma' allocators if the
//:   (template parameter) type 'ALLOCATOR' is convertible from
//:   'bslma::Allocator *'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR> >
     : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR> >
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
